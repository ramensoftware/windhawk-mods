// ==WindhawkMod==
// @id              saved-run-commands
// @name            Saved Run Commands
// @description     Adds a compact saved commands list to Run-style dialogs.
// @version         1.1.0
// @author          communism420
// @github          https://github.com/communism420
// @include         explorer.exe
// @include         Taskmgr.exe
// @compilerOptions -lcomctl32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Saved Run Commands

Adds a small button to the standard Windows Run dialog and Task Manager's
"Create new task" dialog. The button opens a compact "Saved Commands" window.
A double click on a saved command inserts it into the dialog edit box and
immediately executes it.
The Saved Commands window can also save either the current Run command or a
command typed directly into its own input field.
Saved commands can be edited with the "Update selected" button.
Multiple saved commands can be selected and deleted at once.
Each saved command has a per-row "Run as administrator" checkbox.

## Screenshots

![Windows Run dialog with the Saved Run Commands button](https://raw.githubusercontent.com/communism420/Media-Host-For-My-Windhawk-Mods/refs/heads/main/photo%20saved%20run%20commands%20%28button%29.png)

![Saved Commands window](https://raw.githubusercontent.com/communism420/Media-Host-For-My-Windhawk-Mods/refs/heads/main/photo%20saved%20run%20commands%20%28window%29.png)

Notes:
- Commands are stored as one line per command in the mod storage folder.
- Elevated commands expand environment variables before a best-effort
  ShellExecute runas launch.
- The UI added by this mod is intentionally English-only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxCommands: 40
  $name: Maximum saved commands
  $description: Maximum number of commands to keep in the saved commands list.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cwctype>
#include <string>
#include <vector>

namespace {

constexpr int kRunEditId = 0x3E9;
constexpr int kRunCommandComboId = 0x300A;
constexpr int kRunSavedButtonId = 0x51A7;

constexpr int kSavedListId = 0x6001;
constexpr int kSaveButtonId = 0x6002;
constexpr int kDeleteButtonId = 0x6003;
constexpr int kCloseButtonId = 0x6004;
constexpr int kTypedCommandEditId = 0x6005;
constexpr int kSaveTypedButtonId = 0x6006;
constexpr int kUpdateSelectedButtonId = 0x6007;

constexpr UINT_PTR kCommandsListSubclassId = 2;
constexpr UINT_PTR kTypedCommandEditSubclassId = 3;

const wchar_t kSavedWindowClass[] = L"WindhawkSavedRunCommandsWindow";
const wchar_t kRunButtonInstalledProp[] =
    L"WindhawkSavedRunCommandsButtonInstalled";
const wchar_t kRunDialogSubclassedProp[] =
    L"WindhawkSavedRunCommandsDialogSubclassed";
const wchar_t kCommandsFileName[] = L"commands.txt";

HMODULE g_hModule;
std::vector<std::wstring> g_commands;
std::vector<BYTE> g_runAsAdmin;
CRITICAL_SECTION g_commandsLock;
bool g_commandsLockInitialized;
bool g_savedWindowClassRegistered;
volatile LONG g_unloading;
volatile LONG g_maxCommands = 40;
PVOID volatile g_savedCommandsWindow;
UINT g_msgRefreshSavedList;
UINT g_msgExecuteSelectedCommand;
thread_local HHOOK g_runFileDlgCbtHook;

using RunFileDlg_t = void(WINAPI*)(HWND,
                                   HICON,
                                   LPCWSTR,
                                   LPCWSTR,
                                   LPCWSTR,
                                   UINT);
RunFileDlg_t g_originalRunFileDlg;

void SubclassRunFileDialog(HWND hwnd, bool installIfReady);
bool IsDialogClass(HWND hwnd);
std::wstring GetListCommandAt(HWND savedWindow, int index);
void SelectSingleCommand(HWND savedWindow, int index);

int ClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

bool IsUnloading() {
    return InterlockedCompareExchange(&g_unloading, 0, 0) != 0;
}

void SetUnloading() {
    InterlockedExchange(&g_unloading, 1);
}

int GetMaxCommands() {
    return static_cast<int>(InterlockedCompareExchange(&g_maxCommands, 0, 0));
}

void SetMaxCommands(int maxCommands) {
    InterlockedExchange(&g_maxCommands, maxCommands);
}

HWND GetSavedCommandsWindow() {
    return static_cast<HWND>(InterlockedCompareExchangePointer(
        &g_savedCommandsWindow, nullptr, nullptr));
}

void SetSavedCommandsWindow(HWND hwnd) {
    InterlockedExchangePointer(&g_savedCommandsWindow, hwnd);
}

bool InitRegisteredMessages() {
    g_msgRefreshSavedList = RegisterWindowMessageW(
        L"Windhawk_SavedRunCommands_Refresh_saved-run-commands");
    g_msgExecuteSelectedCommand = RegisterWindowMessageW(
        L"Windhawk_SavedRunCommands_ExecuteSelected_saved-run-commands");

    if (!g_msgRefreshSavedList || !g_msgExecuteSelectedCommand) {
        Wh_Log(L"RegisterWindowMessageW failed, error %u", GetLastError());
        return false;
    }

    return true;
}

UINT GetDpiForWindowCompat(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);

    static auto pGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));

    if (pGetDpiForWindow) {
        return pGetDpiForWindow(hwnd);
    }

    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        return 96;
    }

    UINT dpi = static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX));
    ReleaseDC(hwnd, hdc);
    return dpi ? dpi : 96;
}

int ScaleForWindow(HWND hwnd, int value) {
    return MulDiv(value, static_cast<int>(GetDpiForWindowCompat(hwnd)), 96);
}

std::wstring TrimString(const std::wstring& value) {
    size_t begin = 0;
    while (begin < value.size() && iswspace(value[begin])) {
        begin++;
    }

    size_t end = value.size();
    while (end > begin && iswspace(value[end - 1])) {
        end--;
    }

    return value.substr(begin, end - begin);
}

bool ContainsCommand(const std::vector<std::wstring>& commands,
                     const std::wstring& command) {
    return std::find(commands.begin(), commands.end(), command) != commands.end();
}

void NormalizeCommandFlagsLocked() {
    if (g_runAsAdmin.size() < g_commands.size()) {
        g_runAsAdmin.resize(g_commands.size(), 0);
    } else if (g_runAsAdmin.size() > g_commands.size()) {
        g_runAsAdmin.resize(g_commands.size());
    }
}

void TrimCommandsToLimitLocked() {
    int maxCommands = GetMaxCommands();
    if (maxCommands < 1) {
        maxCommands = 40;
        SetMaxCommands(maxCommands);
    }

    NormalizeCommandFlagsLocked();

    while (static_cast<int>(g_commands.size()) > maxCommands) {
        g_commands.pop_back();
        g_runAsAdmin.pop_back();
    }
}

void LoadSettings() {
    int maxCommands = Wh_GetIntSetting(L"maxCommands");
    if (maxCommands <= 0) {
        maxCommands = 40;
    }

    SetMaxCommands(ClampInt(maxCommands, 1, 500));
}

bool GetStorageFolder(std::wstring* folder) {
    std::vector<wchar_t> buffer(32768);
    size_t length = Wh_GetModStoragePath(buffer.data(), buffer.size());
    if (length == 0 || length >= buffer.size()) {
        Wh_Log(L"Wh_GetModStoragePath failed");
        return false;
    }

    folder->assign(buffer.data(), length);
    return !folder->empty();
}

bool GetCommandsFilePath(std::wstring* path) {
    std::wstring folder;
    if (!GetStorageFolder(&folder)) {
        return false;
    }

    CreateDirectoryW(folder.c_str(), nullptr);

    if (!folder.empty() && folder.back() != L'\\' && folder.back() != L'/') {
        folder += L'\\';
    }

    *path = folder + kCommandsFileName;
    return true;
}

bool DecodeTextFile(const std::vector<BYTE>& bytes, std::wstring* text) {
    text->clear();

    if (bytes.empty()) {
        return true;
    }

    if (bytes.size() >= 2 && bytes[0] == 0xFF && bytes[1] == 0xFE) {
        size_t wcharCount = (bytes.size() - 2) / sizeof(wchar_t);
        text->resize(wcharCount);
        if (wcharCount > 0) {
            memcpy(&(*text)[0], bytes.data() + 2,
                   wcharCount * sizeof(wchar_t));
        }

        return true;
    }

    if (bytes.size() >= 2 && bytes[0] == 0xFE && bytes[1] == 0xFF) {
        size_t wcharCount = (bytes.size() - 2) / 2;
        text->reserve(wcharCount);
        for (size_t i = 0; i < wcharCount; i++) {
            wchar_t ch =
                static_cast<wchar_t>((bytes[2 + i * 2] << 8) |
                                     bytes[2 + i * 2 + 1]);
            text->push_back(ch);
        }

        return true;
    }

    DWORD offset = 0;
    if (bytes.size() >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB &&
        bytes[2] == 0xBF) {
        offset = 3;
    }

    int byteCount = static_cast<int>(bytes.size() - offset);
    int charCount = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        reinterpret_cast<LPCCH>(bytes.data() + offset), byteCount, nullptr, 0);

    UINT codePage = CP_UTF8;
    DWORD flags = MB_ERR_INVALID_CHARS;
    if (charCount == 0) {
        codePage = CP_ACP;
        flags = 0;
        charCount = MultiByteToWideChar(
            codePage, flags, reinterpret_cast<LPCCH>(bytes.data() + offset),
            byteCount, nullptr, 0);
    }

    if (charCount == 0) {
        Wh_Log(L"Failed to decode commands file, error %u", GetLastError());
        return false;
    }

    text->resize(charCount);
    MultiByteToWideChar(codePage, flags,
                        reinterpret_cast<LPCCH>(bytes.data() + offset),
                        byteCount, &(*text)[0], charCount);
    return true;
}

struct ParsedCommands {
    std::vector<std::wstring> commands;
    std::vector<BYTE> runAsAdmin;
};

bool TryParseCommandLine(const std::wstring& line,
                         std::wstring* command,
                         BYTE* runAsAdmin) {
    constexpr wchar_t kAdminEnabledPrefix[] = L"admin=1\t";
    constexpr wchar_t kAdminDisabledPrefix[] = L"admin=0\t";
    constexpr size_t kAdminPrefixLength = ARRAYSIZE(kAdminEnabledPrefix) - 1;

    *runAsAdmin = 0;
    *command = line;

    if (line.compare(0, kAdminPrefixLength, kAdminEnabledPrefix) == 0) {
        *runAsAdmin = 1;
        *command = line.substr(kAdminPrefixLength);
    } else if (line.compare(0, kAdminPrefixLength, kAdminDisabledPrefix) == 0) {
        *command = line.substr(kAdminPrefixLength);
    }

    *command = TrimString(*command);
    return !command->empty();
}

ParsedCommands ParseCommands(const std::wstring& text) {
    ParsedCommands parsed;
    size_t pos = 0;
    int maxCommands = GetMaxCommands();

    while (pos < text.size() &&
           static_cast<int>(parsed.commands.size()) < maxCommands) {
        size_t lineEnd = text.find_first_of(L"\r\n", pos);
        std::wstring line = TrimString(
            text.substr(pos, lineEnd == std::wstring::npos ? lineEnd
                                                           : lineEnd - pos));

        std::wstring command;
        BYTE runAsAdmin = 0;
        if (TryParseCommandLine(line, &command, &runAsAdmin) &&
            !ContainsCommand(parsed.commands, command)) {
            parsed.commands.push_back(command);
            parsed.runAsAdmin.push_back(runAsAdmin);
        }

        if (lineEnd == std::wstring::npos) {
            break;
        }

        pos = lineEnd + 1;
        while (pos < text.size() && (text[pos] == L'\r' || text[pos] == L'\n')) {
            pos++;
        }
    }

    return parsed;
}

void LoadCommandsFromFile() {
    std::wstring path;
    if (!GetCommandsFilePath(&path)) {
        return;
    }

    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) {
            Wh_Log(L"CreateFileW for reading failed, error %u", error);
        }

        return;
    }

    LARGE_INTEGER fileSize = {};
    if (!GetFileSizeEx(file, &fileSize) || fileSize.QuadPart < 0 ||
        fileSize.QuadPart > 1024 * 1024) {
        Wh_Log(L"Commands file size is invalid or too large");
        CloseHandle(file);
        return;
    }

    std::vector<BYTE> bytes(static_cast<size_t>(fileSize.QuadPart));
    DWORD bytesRead = 0;
    BOOL readOk =
        bytes.empty()
            ? TRUE
            : ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()),
                       &bytesRead, nullptr);
    CloseHandle(file);

    if (!readOk || bytesRead != bytes.size()) {
        Wh_Log(L"ReadFile failed, error %u", GetLastError());
        return;
    }

    std::wstring text;
    if (!DecodeTextFile(bytes, &text)) {
        return;
    }

    ParsedCommands parsed = ParseCommands(text);

    EnterCriticalSection(&g_commandsLock);
    g_commands = std::move(parsed.commands);
    g_runAsAdmin = std::move(parsed.runAsAdmin);
    TrimCommandsToLimitLocked();
    LeaveCriticalSection(&g_commandsLock);
}

void SaveCommandsToFile() {
    std::vector<std::wstring> commands;
    std::vector<BYTE> runAsAdmin;

    EnterCriticalSection(&g_commandsLock);
    NormalizeCommandFlagsLocked();
    commands = g_commands;
    runAsAdmin = g_runAsAdmin;
    LeaveCriticalSection(&g_commandsLock);

    std::wstring path;
    if (!GetCommandsFilePath(&path)) {
        return;
    }

    std::wstring text;
    for (size_t i = 0; i < commands.size(); i++) {
        text += runAsAdmin[i] ? L"admin=1\t" : L"admin=0\t";
        const std::wstring& command = commands[i];
        text += command;
        text += L"\r\n";
    }

    HANDLE file =
        CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateFileW for writing failed, error %u", GetLastError());
        return;
    }

    BYTE bom[] = {0xFF, 0xFE};
    DWORD bytesWritten = 0;
    BOOL ok = WriteFile(file, bom, sizeof(bom), &bytesWritten, nullptr);

    if (ok && !text.empty()) {
        DWORD bytesToWrite = static_cast<DWORD>(text.size() * sizeof(wchar_t));
        ok = WriteFile(file, text.data(), bytesToWrite, &bytesWritten, nullptr);
    }

    if (!ok) {
        Wh_Log(L"WriteFile failed, error %u", GetLastError());
    }

    CloseHandle(file);
}

bool AddOrPromoteCommand(const std::wstring& command,
                         bool runAsAdmin = false,
                         bool updateRunAsAdmin = false) {
    std::wstring trimmed = TrimString(command);
    if (trimmed.empty()) {
        return false;
    }

    EnterCriticalSection(&g_commandsLock);
    NormalizeCommandFlagsLocked();

    auto it = std::find(g_commands.begin(), g_commands.end(), trimmed);
    if (it != g_commands.end()) {
        size_t index = static_cast<size_t>(it - g_commands.begin());
        BYTE existingRunAsAdmin = g_runAsAdmin[index];
        g_commands.erase(it);
        g_runAsAdmin.erase(g_runAsAdmin.begin() + index);
        runAsAdmin = updateRunAsAdmin ? runAsAdmin : existingRunAsAdmin;
    }

    g_commands.insert(g_commands.begin(), trimmed);
    g_runAsAdmin.insert(g_runAsAdmin.begin(), runAsAdmin ? 1 : 0);
    TrimCommandsToLimitLocked();

    LeaveCriticalSection(&g_commandsLock);
    return true;
}

bool DeleteCommandAt(int index) {
    bool deleted = false;

    EnterCriticalSection(&g_commandsLock);
    NormalizeCommandFlagsLocked();
    if (index >= 0 && index < static_cast<int>(g_commands.size())) {
        g_commands.erase(g_commands.begin() + index);
        g_runAsAdmin.erase(g_runAsAdmin.begin() + index);
        deleted = true;
    }
    LeaveCriticalSection(&g_commandsLock);

    return deleted;
}

bool GetRunAsAdminAt(int index) {
    bool runAsAdmin = false;

    EnterCriticalSection(&g_commandsLock);
    NormalizeCommandFlagsLocked();
    if (index >= 0 && index < static_cast<int>(g_runAsAdmin.size())) {
        runAsAdmin = g_runAsAdmin[index] != 0;
    }
    LeaveCriticalSection(&g_commandsLock);

    return runAsAdmin;
}

bool ToggleRunAsAdminAt(int index) {
    bool toggled = false;

    EnterCriticalSection(&g_commandsLock);
    NormalizeCommandFlagsLocked();
    if (index >= 0 && index < static_cast<int>(g_runAsAdmin.size())) {
        g_runAsAdmin[index] = g_runAsAdmin[index] ? 0 : 1;
        toggled = true;
    }
    LeaveCriticalSection(&g_commandsLock);

    return toggled;
}

bool UpdateCommandAt(int index, const std::wstring& command, int* newIndex) {
    std::wstring trimmed = TrimString(command);
    if (trimmed.empty()) {
        return false;
    }

    bool updated = false;
    int finalIndex = index;

    EnterCriticalSection(&g_commandsLock);
    NormalizeCommandFlagsLocked();

    if (index >= 0 && index < static_cast<int>(g_commands.size())) {
        BYTE runAsAdmin = g_runAsAdmin[index];

        g_commands.erase(g_commands.begin() + index);
        g_runAsAdmin.erase(g_runAsAdmin.begin() + index);

        auto duplicateIt = std::find(g_commands.begin(), g_commands.end(), trimmed);
        if (duplicateIt != g_commands.end()) {
            size_t duplicateIndex =
                static_cast<size_t>(duplicateIt - g_commands.begin());
            g_commands.erase(duplicateIt);
            g_runAsAdmin.erase(g_runAsAdmin.begin() + duplicateIndex);
            if (static_cast<int>(duplicateIndex) < finalIndex) {
                finalIndex--;
            }
        }

        finalIndex =
            ClampInt(finalIndex, 0, static_cast<int>(g_commands.size()));
        g_commands.insert(g_commands.begin() + finalIndex, trimmed);
        g_runAsAdmin.insert(g_runAsAdmin.begin() + finalIndex, runAsAdmin);
        updated = true;
    }

    LeaveCriticalSection(&g_commandsLock);

    if (newIndex) {
        *newIndex = updated ? finalIndex : -1;
    }

    return updated;
}

bool IsDialogClass(HWND hwnd) {
    wchar_t className[32] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return wcscmp(className, L"#32770") == 0;
}

bool IsWindowClass(HWND hwnd, PCWSTR expectedClassName) {
    wchar_t className[32] = {};
    if (!hwnd || !GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, expectedClassName) == 0;
}

bool IsRunCommandEdit(HWND hwnd) {
    return IsWindowVisible(hwnd) && IsWindowClass(hwnd, L"Edit");
}

bool IsRunCommandCombo(HWND hwnd) {
    return IsWindowVisible(hwnd) &&
           (IsWindowClass(hwnd, L"ComboBox") ||
            IsWindowClass(hwnd, L"ComboBoxEx32"));
}

HWND FindRunCommandControl(HWND runDialog) {
    HWND control = GetDlgItem(runDialog, kRunEditId);
    if (IsRunCommandEdit(control) || IsRunCommandCombo(control)) {
        return control;
    }

    HWND combo = GetDlgItem(runDialog, kRunCommandComboId);
    if (!IsRunCommandCombo(combo)) {
        return nullptr;
    }

    HWND edit = GetDlgItem(combo, kRunEditId);
    if (IsRunCommandEdit(edit)) {
        return edit;
    }

    edit = FindWindowExW(combo, nullptr, L"Edit", nullptr);
    if (IsRunCommandEdit(edit)) {
        return edit;
    }

    return combo;
}

void ChildRectToParent(HWND parent, HWND child, RECT* rect) {
    GetWindowRect(child, rect);
    MapWindowPoints(HWND_DESKTOP, parent, reinterpret_cast<POINT*>(rect), 2);
}

struct BrowseButtonSearch {
    HWND runDialog;
    HWND okButton;
    HWND cancelButton;
    HWND bestButton;
    int bestScore;
};

BOOL CALLBACK FindBrowseButtonProc(HWND child, LPARAM lParam) {
    auto search = reinterpret_cast<BrowseButtonSearch*>(lParam);
    wchar_t className[32] = {};
    if (!GetClassNameW(child, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"Button") != 0 || !IsWindowVisible(child)) {
        return TRUE;
    }

    if (child == search->okButton || child == search->cancelButton ||
        GetDlgCtrlID(child) == kRunSavedButtonId) {
        return TRUE;
    }

    RECT rect = {};
    RECT parentRect = {};
    if (!GetWindowRect(child, &rect) ||
        !GetWindowRect(search->runDialog, &parentRect)) {
        return TRUE;
    }

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width < 40 || height < 16) {
        return TRUE;
    }

    int score = rect.left - parentRect.left;
    if (rect.top > parentRect.top + (parentRect.bottom - parentRect.top) / 2) {
        score += 10000;
    }

    if (score > search->bestScore) {
        search->bestScore = score;
        search->bestButton = child;
    }

    return TRUE;
}

HWND FindBrowseButton(HWND runDialog) {
    BrowseButtonSearch search = {
        runDialog, GetDlgItem(runDialog, IDOK), GetDlgItem(runDialog, IDCANCEL),
        nullptr, 0};
    EnumChildWindows(runDialog, FindBrowseButtonProc,
                     reinterpret_cast<LPARAM>(&search));
    return search.bestButton;
}

struct RunDialogShapeSearch {
    int visibleStaticCount;
    int visibleIconCount;
};

BOOL CALLBACK CheckRunDialogShapeProc(HWND child, LPARAM lParam) {
    auto search = reinterpret_cast<RunDialogShapeSearch*>(lParam);
    if (!search || !IsWindowVisible(child)) {
        return TRUE;
    }

    wchar_t className[32] = {};
    if (!GetClassNameW(child, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"Static") == 0) {
        LONG_PTR style = GetWindowLongPtrW(child, GWL_STYLE);
        if ((style & SS_TYPEMASK) == SS_ICON) {
            search->visibleIconCount++;
        } else {
            search->visibleStaticCount++;
        }
    }

    return TRUE;
}

bool HasRunDialogShape(HWND hwnd) {
    RunDialogShapeSearch search = {};
    EnumChildWindows(hwnd, CheckRunDialogShapeProc,
                     reinterpret_cast<LPARAM>(&search));

    return search.visibleStaticCount >= 1 && search.visibleIconCount >= 1;
}

bool IsRunDialog(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsDialogClass(hwnd)) {
        return false;
    }

    if (!GetDlgItem(hwnd, IDOK) || !GetDlgItem(hwnd, IDCANCEL)) {
        return false;
    }

    if (!FindRunCommandControl(hwnd)) {
        return false;
    }

    return FindBrowseButton(hwnd) != nullptr && HasRunDialogShape(hwnd);
}

void PositionRunButton(HWND hwnd) {
    HWND button = GetDlgItem(hwnd, kRunSavedButtonId);
    if (!button) {
        return;
    }

    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int margin = ScaleForWindow(hwnd, 8);
    int gap = ScaleForWindow(hwnd, 6);
    int buttonWidth = ScaleForWindow(hwnd, 28);
    int buttonHeight = ScaleForWindow(hwnd, 23);

    RECT refRect = {};
    HWND browseButton = FindBrowseButton(hwnd);
    HWND okButton = GetDlgItem(hwnd, IDOK);

    if (browseButton) {
        ChildRectToParent(hwnd, browseButton, &refRect);
        buttonHeight = refRect.bottom - refRect.top;
        buttonWidth = std::max(buttonWidth, buttonHeight);
    } else if (okButton) {
        ChildRectToParent(hwnd, okButton, &refRect);
        buttonHeight = refRect.bottom - refRect.top;
    } else {
        refRect.left = margin;
        refRect.right = refRect.left + buttonWidth;
        refRect.bottom = clientRect.bottom - margin;
        refRect.top = refRect.bottom - buttonHeight;
    }

    int x = margin;
    int y = refRect.top;
    bool positionSet = false;

    if (browseButton) {
        int rightOfBrowse = refRect.right + gap;
        if (rightOfBrowse + buttonWidth <= clientRect.right - margin) {
            x = rightOfBrowse;
            y = refRect.top;
            positionSet = true;
        }
    }

    if (!positionSet && okButton) {
        ChildRectToParent(hwnd, okButton, &refRect);
        x = refRect.left - gap - buttonWidth;
        y = refRect.top;
        positionSet = true;
    }

    if (!positionSet && browseButton) {
        ChildRectToParent(hwnd, browseButton, &refRect);
        x = refRect.left - gap - buttonWidth;
        y = refRect.top;
    }

    if (x < margin) {
        x = margin;
    }

    if (y < margin || y + buttonHeight > clientRect.bottom - margin) {
        y = clientRect.bottom - margin - buttonHeight;
    }

    MoveWindow(button, x, y, buttonWidth, buttonHeight, TRUE);
}

void RemoveRunButton(HWND hwnd) {
    if (!GetPropW(hwnd, kRunButtonInstalledProp)) {
        return;
    }

    HWND button = GetDlgItem(hwnd, kRunSavedButtonId);
    if (button) {
        DestroyWindow(button);
    }

    RemovePropW(hwnd, kRunButtonInstalledProp);
}

bool InstallRunButton(HWND hwnd) {
    if (IsUnloading() || !IsWindow(hwnd)) {
        return false;
    }

    bool buttonMarkedInstalled =
        GetPropW(hwnd, kRunButtonInstalledProp) != nullptr;
    HWND existingButton = GetDlgItem(hwnd, kRunSavedButtonId);
    if (buttonMarkedInstalled) {
        if (existingButton) {
            PositionRunButton(hwnd);
            return true;
        }

        return false;
    }

    HFONT font = reinterpret_cast<HFONT>(SendMessageW(hwnd, WM_GETFONT, 0, 0));
    if (!font) {
        font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }

    SetPropW(hwnd, kRunButtonInstalledProp, reinterpret_cast<HANDLE>(1));

    HWND button = CreateWindowExW(
        0, L"Button", L"\x2605",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 1, 1, hwnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(kRunSavedButtonId)),
        g_hModule, nullptr);
    if (!button) {
        RemovePropW(hwnd, kRunButtonInstalledProp);
        Wh_Log(L"Failed to create Run dialog button, error %u", GetLastError());
        return false;
    }

    SendMessageW(button, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    PositionRunButton(hwnd);

    return true;
}

void FillCommandsList(HWND savedWindow) {
    HWND list = GetDlgItem(savedWindow, kSavedListId);
    if (!list) {
        return;
    }

    std::vector<std::wstring> commands;

    EnterCriticalSection(&g_commandsLock);
    NormalizeCommandFlagsLocked();
    commands = g_commands;
    LeaveCriticalSection(&g_commandsLock);

    SendMessageW(list, WM_SETREDRAW, FALSE, 0);
    SendMessageW(list, LB_RESETCONTENT, 0, 0);

    for (const std::wstring& command : commands) {
        SendMessageW(list, LB_ADDSTRING, 0,
                     reinterpret_cast<LPARAM>(command.c_str()));
    }

    SendMessageW(list, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(list, nullptr, TRUE);
}

RECT GetAdminCheckboxRect(HWND list, int index) {
    RECT itemRect = {};
    SendMessageW(list, LB_GETITEMRECT, index, reinterpret_cast<LPARAM>(&itemRect));

    int boxSize = ScaleForWindow(list, 13);
    int left = itemRect.left + ScaleForWindow(list, 6);
    int top = itemRect.top + ((itemRect.bottom - itemRect.top) - boxSize) / 2;

    RECT checkboxRect = {left, top, left + boxSize, top + boxSize};
    return checkboxRect;
}

bool IsPointInAdminCheckbox(HWND list, int index, LPARAM lParam) {
    if (index < 0) {
        return false;
    }

    POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    RECT checkboxRect = GetAdminCheckboxRect(list, index);
    InflateRect(&checkboxRect, ScaleForWindow(list, 4), ScaleForWindow(list, 4));
    return PtInRect(&checkboxRect, point) != FALSE;
}

void DrawCommandsListItem(HWND savedWindow, const DRAWITEMSTRUCT* drawItem) {
    if (!drawItem || drawItem->CtlID != kSavedListId ||
        drawItem->itemID == static_cast<UINT>(-1)) {
        return;
    }

    HWND list = drawItem->hwndItem;
    int index = static_cast<int>(drawItem->itemID);

    std::wstring command = GetListCommandAt(savedWindow, index);
    bool runAsAdmin = GetRunAsAdminAt(index);

    bool selected = (drawItem->itemState & ODS_SELECTED) != 0;
    COLORREF backgroundColor =
        GetSysColor(selected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
    COLORREF textColor =
        GetSysColor(selected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);

    HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor);
    FillRect(drawItem->hDC, &drawItem->rcItem, backgroundBrush);
    DeleteObject(backgroundBrush);

    RECT checkboxRect = GetAdminCheckboxRect(list, index);
    DrawFrameControl(drawItem->hDC, &checkboxRect, DFC_BUTTON,
                     DFCS_BUTTONCHECK | (runAsAdmin ? DFCS_CHECKED : 0));

    RECT textRect = drawItem->rcItem;
    textRect.left = checkboxRect.right + ScaleForWindow(list, 8);
    textRect.right -= ScaleForWindow(list, 4);

    int oldBkMode = SetBkMode(drawItem->hDC, TRANSPARENT);
    COLORREF oldTextColor = SetTextColor(drawItem->hDC, textColor);
    DrawTextW(drawItem->hDC, command.c_str(), -1, &textRect,
              DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    SetTextColor(drawItem->hDC, oldTextColor);
    SetBkMode(drawItem->hDC, oldBkMode);

    if ((drawItem->itemState & ODS_FOCUS) != 0) {
        DrawFocusRect(drawItem->hDC, &drawItem->rcItem);
    }
}

void ToggleRunAsAdminFromList(HWND list, int index) {
    if (index < 0 || !ToggleRunAsAdminAt(index)) {
        return;
    }

    SaveCommandsToFile();

    RECT itemRect = {};
    if (SendMessageW(list, LB_GETITEMRECT, index,
                     reinterpret_cast<LPARAM>(&itemRect)) != LB_ERR) {
        InvalidateRect(list, &itemRect, TRUE);
    } else {
        InvalidateRect(list, nullptr, TRUE);
    }
}

std::wstring GetRunEditText(HWND runDialog) {
    HWND control = FindRunCommandControl(runDialog);
    if (!control) {
        return {};
    }

    int length = GetWindowTextLengthW(control);
    if (length <= 0) {
        return {};
    }

    std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1);
    GetWindowTextW(control, buffer.data(), static_cast<int>(buffer.size()));
    return TrimString(buffer.data());
}

void SaveCurrentRunCommand(HWND savedWindow) {
    HWND runDialog = reinterpret_cast<HWND>(
        GetWindowLongPtrW(savedWindow, GWLP_USERDATA));
    if (!IsWindow(runDialog)) {
        Wh_Log(L"Run dialog is gone, cannot save current command");
        return;
    }

    std::wstring command = GetRunEditText(runDialog);
    if (command.empty()) {
        return;
    }

    if (AddOrPromoteCommand(command)) {
        SaveCommandsToFile();
        FillCommandsList(savedWindow);
        SelectSingleCommand(savedWindow, 0);
    }
}

std::wstring GetTypedCommandText(HWND savedWindow) {
    HWND edit = GetDlgItem(savedWindow, kTypedCommandEditId);
    if (!edit) {
        return {};
    }

    int length = GetWindowTextLengthW(edit);
    if (length <= 0) {
        return {};
    }

    std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1);
    GetWindowTextW(edit, buffer.data(), static_cast<int>(buffer.size()));
    return TrimString(buffer.data());
}

int GetSingleSelectedCommandIndex(HWND savedWindow) {
    HWND list = GetDlgItem(savedWindow, kSavedListId);
    if (!list) {
        return -1;
    }

    int selectedCount =
        static_cast<int>(SendMessageW(list, LB_GETSELCOUNT, 0, 0));
    if (selectedCount != 1) {
        return -1;
    }

    int index = -1;
    if (SendMessageW(list, LB_GETSELITEMS, 1,
                     reinterpret_cast<LPARAM>(&index)) == LB_ERR) {
        return -1;
    }

    return index;
}

void SelectSingleCommand(HWND savedWindow, int index) {
    HWND list = GetDlgItem(savedWindow, kSavedListId);
    if (!list || index < 0) {
        return;
    }

    SendMessageW(list, LB_SETSEL, FALSE, -1);
    SendMessageW(list, LB_SETSEL, TRUE, index);
    SendMessageW(list, LB_SETCARETINDEX, index, 0);
}

void SaveTypedCommand(HWND savedWindow) {
    HWND edit = GetDlgItem(savedWindow, kTypedCommandEditId);
    std::wstring command = GetTypedCommandText(savedWindow);
    if (command.empty()) {
        return;
    }

    if (AddOrPromoteCommand(command)) {
        SaveCommandsToFile();
        FillCommandsList(savedWindow);
        SelectSingleCommand(savedWindow, 0);
        if (edit) {
            SetWindowTextW(edit, L"");
            SetFocus(edit);
        }
    }
}

void UpdateSelectedCommand(HWND savedWindow) {
    int index = GetSingleSelectedCommandIndex(savedWindow);
    if (index < 0) {
        return;
    }

    std::wstring command = GetTypedCommandText(savedWindow);
    if (command.empty()) {
        return;
    }

    int newIndex = -1;
    if (UpdateCommandAt(index, command, &newIndex)) {
        SaveCommandsToFile();
        FillCommandsList(savedWindow);
        SelectSingleCommand(savedWindow, newIndex);
    }
}

std::wstring GetListCommandAt(HWND savedWindow, int index) {
    HWND list = GetDlgItem(savedWindow, kSavedListId);
    if (!list || index < 0) {
        return {};
    }

    int length = static_cast<int>(SendMessageW(list, LB_GETTEXTLEN, index, 0));
    if (length == LB_ERR || length <= 0) {
        return {};
    }

    std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1);
    SendMessageW(list, LB_GETTEXT, index, reinterpret_cast<LPARAM>(buffer.data()));

    return buffer.data();
}

void LoadSingleSelectedCommandIntoEdit(HWND savedWindow) {
    int index = GetSingleSelectedCommandIndex(savedWindow);
    if (index < 0) {
        return;
    }

    HWND edit = GetDlgItem(savedWindow, kTypedCommandEditId);
    if (!edit) {
        return;
    }

    std::wstring command = GetListCommandAt(savedWindow, index);
    if (!command.empty()) {
        SetWindowTextW(edit, command.c_str());
    }
}

bool CanResolveExecutable(const std::wstring& candidate) {
    if (candidate.empty()) {
        return false;
    }

    std::vector<wchar_t> buffer(32768);
    DWORD result = SearchPathW(nullptr, candidate.c_str(), nullptr,
                               static_cast<DWORD>(buffer.size()), buffer.data(),
                               nullptr);
    if (result > 0 && result < buffer.size()) {
        DWORD attributes = GetFileAttributesW(buffer.data());
        return attributes != INVALID_FILE_ATTRIBUTES &&
               (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    result = SearchPathW(nullptr, candidate.c_str(), L".exe",
                         static_cast<DWORD>(buffer.size()), buffer.data(),
                         nullptr);
    if (result <= 0 || result >= buffer.size()) {
        return false;
    }

    DWORD attributes = GetFileAttributesW(buffer.data());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

std::wstring ExpandEnvironmentStringsInCommand(const std::wstring& command) {
    if (command.empty()) {
        return {};
    }

    DWORD requiredChars = ExpandEnvironmentStringsW(command.c_str(), nullptr, 0);
    if (requiredChars == 0) {
        Wh_Log(L"ExpandEnvironmentStringsW failed, error %u", GetLastError());
        return command;
    }

    std::vector<wchar_t> buffer(requiredChars);
    DWORD copiedChars = ExpandEnvironmentStringsW(
        command.c_str(), buffer.data(), static_cast<DWORD>(buffer.size()));
    if (copiedChars == 0 || copiedChars > buffer.size()) {
        Wh_Log(L"ExpandEnvironmentStringsW failed, error %u", GetLastError());
        return command;
    }

    return TrimString(buffer.data());
}

void SplitCommandForShellExecute(const std::wstring& command,
                                 std::wstring* executable,
                                 std::wstring* parameters) {
    *executable = command;
    parameters->clear();

    if (command.empty()) {
        return;
    }

    if (command.front() == L'"') {
        size_t quoteEnd = command.find(L'"', 1);
        if (quoteEnd != std::wstring::npos) {
            *executable = command.substr(1, quoteEnd - 1);
            *parameters = TrimString(command.substr(quoteEnd + 1));
        }

        return;
    }

    size_t searchEnd = command.size();
    while (searchEnd != std::wstring::npos) {
        std::wstring candidate = TrimString(command.substr(0, searchEnd));
        if (CanResolveExecutable(candidate)) {
            *executable = candidate;
            *parameters = TrimString(command.substr(searchEnd));
            return;
        }

        if (searchEnd == 0) {
            break;
        }

        searchEnd = command.find_last_of(L" \t", searchEnd - 1);
    }

    size_t firstSpace = command.find_first_of(L" \t");
    if (firstSpace != std::wstring::npos) {
        *executable = command.substr(0, firstSpace);
        *parameters = TrimString(command.substr(firstSpace + 1));
    }
}

bool RunCommandAsAdministrator(const std::wstring& command) {
    std::wstring expandedCommand = ExpandEnvironmentStringsInCommand(command);

    std::wstring executable;
    std::wstring parameters;
    SplitCommandForShellExecute(expandedCommand, &executable, &parameters);

    HINSTANCE result = ShellExecuteW(
        nullptr, L"runas", executable.c_str(),
        parameters.empty() ? nullptr : parameters.c_str(), nullptr,
        SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) > 32) {
        return true;
    }

    Wh_Log(L"Failed to run command as administrator, ShellExecuteW result %d",
           static_cast<int>(reinterpret_cast<INT_PTR>(result)));
    return false;
}

void ExecuteListCommand(HWND savedWindow, int index) {
    HWND runDialog = reinterpret_cast<HWND>(
        GetWindowLongPtrW(savedWindow, GWLP_USERDATA));

    if (index < 0) {
        HWND list = GetDlgItem(savedWindow, kSavedListId);
        if (list) {
            index = static_cast<int>(SendMessageW(list, LB_GETCARETINDEX, 0, 0));
        }
    }

    std::wstring command = GetListCommandAt(savedWindow, index);
    if (command.empty()) {
        return;
    }

    bool runAsAdmin = GetRunAsAdminAt(index);

    if (!IsWindow(runDialog)) {
        if (runAsAdmin) {
            DestroyWindow(savedWindow);
            RunCommandAsAdministrator(command);
        } else {
            DestroyWindow(savedWindow);
        }
        return;
    }

    HWND control = FindRunCommandControl(runDialog);
    if (!control || !SetWindowTextW(control, command.c_str())) {
        Wh_Log(L"Failed to set Run command text, error %u", GetLastError());
        return;
    }

    if (runAsAdmin) {
        DestroyWindow(savedWindow);
        if (RunCommandAsAdministrator(command)) {
            PostMessageW(runDialog, WM_CLOSE, 0, 0);
        }
        return;
    }

    HWND okButton = GetDlgItem(runDialog, IDOK);
    DestroyWindow(savedWindow);

    if (okButton) {
        PostMessageW(okButton, BM_CLICK, 0, 0);
    } else {
        PostMessageW(runDialog, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), 0);
    }
}

void DeleteSelectedCommand(HWND savedWindow) {
    HWND list = GetDlgItem(savedWindow, kSavedListId);
    if (!list) {
        return;
    }

    int selectedCount =
        static_cast<int>(SendMessageW(list, LB_GETSELCOUNT, 0, 0));
    if (selectedCount <= 0 || selectedCount == LB_ERR) {
        return;
    }

    std::vector<int> indices(static_cast<size_t>(selectedCount));
    int copiedCount = static_cast<int>(
        SendMessageW(list, LB_GETSELITEMS, selectedCount,
                     reinterpret_cast<LPARAM>(indices.data())));
    if (copiedCount <= 0 || copiedCount == LB_ERR) {
        return;
    }

    indices.resize(static_cast<size_t>(copiedCount));
    std::sort(indices.rbegin(), indices.rend());

    bool deleted = false;
    for (int index : indices) {
        deleted = DeleteCommandAt(index) || deleted;
    }

    if (deleted) {
        SaveCommandsToFile();
        FillCommandsList(savedWindow);
    }
}

void LayoutSavedWindow(HWND hwnd) {
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int margin = ScaleForWindow(hwnd, 10);
    int gap = ScaleForWindow(hwnd, 8);
    int buttonHeight = ScaleForWindow(hwnd, 24);
    int saveButtonWidth = ScaleForWindow(hwnd, 142);
    int saveTypedButtonWidth = ScaleForWindow(hwnd, 142);
    int updateSelectedButtonWidth = ScaleForWindow(hwnd, 112);
    int deleteButtonWidth = ScaleForWindow(hwnd, 100);
    int closeButtonWidth = ScaleForWindow(hwnd, 72);

    int topY = margin;
    int listY = topY + buttonHeight + gap;
    int buttonY = clientRect.bottom - margin - buttonHeight;
    int listHeight = std::max(ScaleForWindow(hwnd, 60), buttonY - listY - gap);

    HWND typedEdit = GetDlgItem(hwnd, kTypedCommandEditId);
    HWND saveTypedButton = GetDlgItem(hwnd, kSaveTypedButtonId);
    HWND updateSelectedButton = GetDlgItem(hwnd, kUpdateSelectedButtonId);
    HWND list = GetDlgItem(hwnd, kSavedListId);
    HWND saveButton = GetDlgItem(hwnd, kSaveButtonId);
    HWND deleteButton = GetDlgItem(hwnd, kDeleteButtonId);
    HWND closeButton = GetDlgItem(hwnd, kCloseButtonId);

    if (typedEdit) {
        int editWidth =
            std::max(ScaleForWindow(hwnd, 80),
                     static_cast<int>(clientRect.right) - margin * 2 -
                         saveTypedButtonWidth - updateSelectedButtonWidth -
                         gap * 2);
        MoveWindow(typedEdit, margin, topY, editWidth, buttonHeight, TRUE);
    }

    if (saveTypedButton) {
        MoveWindow(saveTypedButton, clientRect.right - margin -
                                        updateSelectedButtonWidth - gap -
                                        saveTypedButtonWidth,
                   topY, saveTypedButtonWidth, buttonHeight, TRUE);
    }

    if (updateSelectedButton) {
        MoveWindow(updateSelectedButton,
                   clientRect.right - margin - updateSelectedButtonWidth, topY,
                   updateSelectedButtonWidth, buttonHeight, TRUE);
    }

    if (list) {
        MoveWindow(list, margin, listY, clientRect.right - margin * 2,
                   listHeight, TRUE);
    }

    int x = margin;
    if (saveButton) {
        MoveWindow(saveButton, x, buttonY, saveButtonWidth, buttonHeight, TRUE);
        x += saveButtonWidth + gap;
    }

    if (deleteButton) {
        MoveWindow(deleteButton, x, buttonY, deleteButtonWidth, buttonHeight,
                   TRUE);
    }

    if (closeButton) {
        MoveWindow(closeButton, clientRect.right - margin - closeButtonWidth,
                   buttonY, closeButtonWidth, buttonHeight, TRUE);
    }
}

void SetChildFont(HWND child, HFONT font) {
    if (child && font) {
        SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    }
}

LRESULT CALLBACK TypedCommandEditSubclassProc(HWND hwnd,
                                              UINT msg,
                                              WPARAM wParam,
                                              LPARAM lParam,
                                              UINT_PTR subclassId,
                                              DWORD_PTR refData) {
    (void)lParam;
    (void)subclassId;

    HWND savedWindow = reinterpret_cast<HWND>(refData);

    switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN && IsWindow(savedWindow)) {
                int length = GetWindowTextLengthW(hwnd);
                if (length <= 0) {
                    return 0;
                }

                std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1);
                GetWindowTextW(hwnd, buffer.data(),
                               static_cast<int>(buffer.size()));

                if (!TrimString(buffer.data()).empty()) {
                    SaveTypedCommand(savedWindow);
                }

                return 0;
            }
            break;

        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, TypedCommandEditSubclassProc,
                                 kTypedCommandEditSubclassId);
            break;
    }

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK CommandsListSubclassProc(HWND hwnd,
                                          UINT msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          UINT_PTR subclassId,
                                          DWORD_PTR refData) {
    (void)subclassId;

    HWND savedWindow = reinterpret_cast<HWND>(refData);

    switch (msg) {
        case WM_LBUTTONDOWN: {
            DWORD hitTest =
                static_cast<DWORD>(SendMessageW(hwnd, LB_ITEMFROMPOINT, 0,
                                                lParam));
            if (HIWORD(hitTest) == 0) {
                int index = LOWORD(hitTest);
                if (IsPointInAdminCheckbox(hwnd, index, lParam)) {
                    SendMessageW(hwnd, LB_SETCARETINDEX, index, 0);
                    ToggleRunAsAdminFromList(hwnd, index);
                    SetFocus(hwnd);
                    return 0;
                }
            }

            break;
        }

        case WM_LBUTTONDBLCLK: {
            DWORD hitTest =
                static_cast<DWORD>(SendMessageW(hwnd, LB_ITEMFROMPOINT, 0,
                                                lParam));
            bool clickedItem = HIWORD(hitTest) == 0;

            if (clickedItem && IsPointInAdminCheckbox(hwnd, LOWORD(hitTest),
                                                      lParam)) {
                int index = LOWORD(hitTest);
                SendMessageW(hwnd, LB_SETCARETINDEX, index, 0);
                ToggleRunAsAdminFromList(hwnd, index);
                return 0;
            }

            LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);

            if (clickedItem && IsWindow(savedWindow)) {
                PostMessageW(savedWindow, g_msgExecuteSelectedCommand,
                             LOWORD(hitTest), 0);
            }

            return result;
        }

        case WM_RBUTTONDOWN: {
            DWORD hitTest =
                static_cast<DWORD>(SendMessageW(hwnd, LB_ITEMFROMPOINT, 0,
                                                lParam));
            if (HIWORD(hitTest) == 0) {
                int index = LOWORD(hitTest);
                LRESULT selected = SendMessageW(hwnd, LB_GETSEL, index, 0);
                if (selected == 0) {
                    SendMessageW(hwnd, LB_SETSEL, FALSE, -1);
                    SendMessageW(hwnd, LB_SETSEL, TRUE, index);
                }

                SendMessageW(hwnd, LB_SETCARETINDEX, index, 0);
                SetFocus(hwnd);
                return 0;
            }

            break;
        }

        case WM_KEYDOWN:
            if (wParam == VK_RETURN && IsWindow(savedWindow)) {
                int index =
                    static_cast<int>(SendMessageW(hwnd, LB_GETCARETINDEX, 0, 0));
                PostMessageW(savedWindow, g_msgExecuteSelectedCommand, index, 0);
                return 0;
            }

            if (wParam == VK_SPACE) {
                int index =
                    static_cast<int>(SendMessageW(hwnd, LB_GETCARETINDEX, 0, 0));
                ToggleRunAsAdminFromList(hwnd, index);
                return 0;
            }
            break;

        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, CommandsListSubclassProc,
                                 kCommandsListSubclassId);
            break;
    }

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK SavedCommandsWndProc(HWND hwnd,
                                      UINT msg,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    if (g_msgRefreshSavedList && msg == g_msgRefreshSavedList) {
        FillCommandsList(hwnd);
        return 0;
    }

    if (g_msgExecuteSelectedCommand && msg == g_msgExecuteSelectedCommand) {
        ExecuteListCommand(hwnd, static_cast<int>(wParam));
        return 0;
    }

    switch (msg) {
        case WM_NCCREATE: {
            auto createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(
                                  createStruct->lpCreateParams));
            return TRUE;
        }

        case WM_CREATE: {
            HFONT font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

            HWND typedEdit = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"Edit", nullptr,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 1, 1,
                hwnd,
                reinterpret_cast<HMENU>(
                    static_cast<INT_PTR>(kTypedCommandEditId)),
                g_hModule, nullptr);

            HWND saveTypedButton = CreateWindowExW(
                0, L"Button", L"Save typed command",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 1, 1,
                hwnd,
                reinterpret_cast<HMENU>(
                    static_cast<INT_PTR>(kSaveTypedButtonId)),
                g_hModule, nullptr);

            HWND updateSelectedButton = CreateWindowExW(
                0, L"Button", L"Update selected",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 1, 1,
                hwnd,
                reinterpret_cast<HMENU>(
                    static_cast<INT_PTR>(kUpdateSelectedButtonId)),
                g_hModule, nullptr);

            HWND list = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"ListBox", nullptr,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY |
                    LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED |
                    LBS_NOINTEGRALHEIGHT,
                0, 0, 1, 1, hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kSavedListId)),
                g_hModule, nullptr);

            HWND saveButton = CreateWindowExW(
                0, L"Button", L"Save current command",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 1, 1,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kSaveButtonId)),
                g_hModule, nullptr);

            HWND deleteButton = CreateWindowExW(
                0, L"Button", L"Delete selected",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 1, 1,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kDeleteButtonId)),
                g_hModule, nullptr);

            HWND closeButton = CreateWindowExW(
                0, L"Button", L"Close",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 1, 1,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kCloseButtonId)),
                g_hModule, nullptr);

            SetChildFont(typedEdit, font);
            SetChildFont(saveTypedButton, font);
            SetChildFont(updateSelectedButton, font);
            SetChildFont(list, font);
            SetChildFont(saveButton, font);
            SetChildFont(deleteButton, font);
            SetChildFont(closeButton, font);

            if (typedEdit) {
                SetWindowSubclass(typedEdit, TypedCommandEditSubclassProc,
                                  kTypedCommandEditSubclassId,
                                  reinterpret_cast<DWORD_PTR>(hwnd));
            }

            if (list) {
                SetWindowSubclass(list, CommandsListSubclassProc,
                                  kCommandsListSubclassId,
                                  reinterpret_cast<DWORD_PTR>(hwnd));
            }

            FillCommandsList(hwnd);
            LayoutSavedWindow(hwnd);
            return 0;
        }

        case WM_SIZE:
            LayoutSavedWindow(hwnd);
            return 0;

        case WM_MEASUREITEM:
            if (wParam == kSavedListId) {
                auto measureItem = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
                measureItem->itemHeight = ScaleForWindow(hwnd, 22);
                return TRUE;
            }
            break;

        case WM_DRAWITEM:
            if (wParam == kSavedListId) {
                DrawCommandsListItem(
                    hwnd, reinterpret_cast<const DRAWITEMSTRUCT*>(lParam));
                return TRUE;
            }
            break;

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            int notification = HIWORD(wParam);

            if (id == kSavedListId && notification == LBN_SELCHANGE) {
                LoadSingleSelectedCommandIntoEdit(hwnd);
                return 0;
            }

            if (id == kSaveButtonId && notification == BN_CLICKED) {
                SaveCurrentRunCommand(hwnd);
                return 0;
            }

            if (id == kSaveTypedButtonId && notification == BN_CLICKED) {
                SaveTypedCommand(hwnd);
                return 0;
            }

            if (id == kUpdateSelectedButtonId && notification == BN_CLICKED) {
                UpdateSelectedCommand(hwnd);
                return 0;
            }

            if (id == kDeleteButtonId && notification == BN_CLICKED) {
                DeleteSelectedCommand(hwnd);
                return 0;
            }

            if (id == kCloseButtonId && notification == BN_CLICKED) {
                DestroyWindow(hwnd);
                return 0;
            }

            break;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (GetSavedCommandsWindow() == hwnd) {
                SetSavedCommandsWindow(nullptr);
            }
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool RegisterSavedWindowClass() {
    if (g_savedWindowClassRegistered) {
        return true;
    }

    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = SavedCommandsWndProc;
    windowClass.hInstance = g_hModule;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1);
    windowClass.lpszClassName = kSavedWindowClass;

    if (RegisterClassExW(&windowClass)) {
        g_savedWindowClassRegistered = true;
        return true;
    }

    DWORD error = GetLastError();
    Wh_Log(L"RegisterClassExW failed, error %u", error);
    return false;
}

void ClampWindowToWorkArea(HWND owner, int width, int height, int* x, int* y) {
    HMONITOR monitor = MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return;
    }

    const RECT& work = monitorInfo.rcWork;
    *x = ClampInt(*x, work.left, std::max(work.left, work.right - width));
    *y = ClampInt(*y, work.top, std::max(work.top, work.bottom - height));
}

void ShowSavedCommandsWindow(HWND runDialog) {
    if (!g_savedWindowClassRegistered) {
        Wh_Log(L"Saved Commands window class isn't registered");
        return;
    }

    HWND savedWindow = GetSavedCommandsWindow();
    if (savedWindow && IsWindow(savedWindow)) {
        SetWindowLongPtrW(savedWindow, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(runDialog));
        FillCommandsList(savedWindow);
        ShowWindow(savedWindow, SW_SHOWNORMAL);
        SetForegroundWindow(savedWindow);
        return;
    }

    SetSavedCommandsWindow(nullptr);

    int width = ScaleForWindow(runDialog, 540);
    int height = ScaleForWindow(runDialog, 285);

    RECT ownerRect = {};
    GetWindowRect(runDialog, &ownerRect);
    int x = ownerRect.left + ((ownerRect.right - ownerRect.left) - width) / 2;
    int y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - height) / 2;
    ClampWindowToWorkArea(runDialog, width, height, &x, &y);

    HWND hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT, kSavedWindowClass,
        L"Saved Commands", WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, x, y,
        width, height, runDialog, nullptr, g_hModule, runDialog);

    if (!hwnd) {
        Wh_Log(L"CreateWindowExW for Saved Commands failed, error %u",
               GetLastError());
        return;
    }

    SetSavedCommandsWindow(hwnd);
    SetForegroundWindow(hwnd);
}

LRESULT CALLBACK RunDialogSubclassProc(HWND hwnd,
                                       UINT msg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       DWORD_PTR refData) {
    (void)refData;

    switch (msg) {
        case WM_INITDIALOG: {
            LRESULT result =
                DefSubclassProc(hwnd, msg, wParam, lParam);
            InstallRunButton(hwnd);
            return result;
        }

        case WM_WINDOWPOSCHANGED:
        case WM_SHOWWINDOW:
        case WM_SETTEXT: {
            LRESULT result =
                DefSubclassProc(hwnd, msg, wParam, lParam);
            InstallRunButton(hwnd);
            return result;
        }

        case WM_SIZE:
            if (GetPropW(hwnd, kRunButtonInstalledProp)) {
                PositionRunButton(hwnd);
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == kRunSavedButtonId &&
                HIWORD(wParam) == BN_CLICKED) {
                ShowSavedCommandsWindow(hwnd);
                return 0;
            }
            break;

        case WM_NCDESTROY:
            RemoveRunButton(hwnd);
            RemovePropW(hwnd, kRunDialogSubclassedProp);
            break;
    }

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

void SubclassRunFileDialog(HWND hwnd, bool installIfReady) {
    if (IsUnloading() || !IsWindow(hwnd) || !IsDialogClass(hwnd)) {
        return;
    }

    if (!GetPropW(hwnd, kRunDialogSubclassedProp)) {
        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
                hwnd, RunDialogSubclassProc, 0)) {
            return;
        }

        SetPropW(hwnd, kRunDialogSubclassedProp, reinterpret_cast<HANDLE>(1));
    }

    if (installIfReady && IsRunDialog(hwnd)) {
        InstallRunButton(hwnd);
    }
}

LRESULT CALLBACK RunFileDlgCbtHookProc(int code, WPARAM wParam, LPARAM lParam) {
    (void)lParam;

    if (!IsUnloading() && code == HCBT_CREATEWND) {
        SubclassRunFileDialog(reinterpret_cast<HWND>(wParam), false);
    } else if (!IsUnloading() && code == HCBT_ACTIVATE) {
        SubclassRunFileDialog(reinterpret_cast<HWND>(wParam), true);
    }

    return CallNextHookEx(g_runFileDlgCbtHook, code, wParam, lParam);
}

void WINAPI RunFileDlg_Hook(HWND owner,
                            HICON icon,
                            LPCWSTR directory,
                            LPCWSTR title,
                            LPCWSTR description,
                            UINT flags) {
    HHOOK cbtHook = nullptr;

    // RunFileDlg is modal, so the CBT hook is scoped to this one call.
    if (!IsUnloading() && !g_runFileDlgCbtHook) {
        cbtHook = SetWindowsHookExW(WH_CBT, RunFileDlgCbtHookProc, nullptr,
                                    GetCurrentThreadId());
        g_runFileDlgCbtHook = cbtHook;
    }

    g_originalRunFileDlg(owner, icon, directory, title, description, flags);

    if (cbtHook) {
        UnhookWindowsHookEx(cbtHook);
        if (g_runFileDlgCbtHook == cbtHook) {
            g_runFileDlgCbtHook = nullptr;
        }
    }
}

bool HookRunFileDlg() {
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) {
        shell32 = LoadLibraryExW(L"shell32.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    if (!shell32) {
        Wh_Log(L"Failed to load shell32.dll, error %u", GetLastError());
        return false;
    }

    auto runFileDlg = reinterpret_cast<RunFileDlg_t>(
        GetProcAddress(shell32, MAKEINTRESOURCEA(61)));
    if (!runFileDlg) {
        Wh_Log(L"Failed to find shell32!RunFileDlg ordinal 61, error %u",
               GetLastError());
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(runFileDlg, RunFileDlg_Hook,
                                        &g_originalRunFileDlg)) {
        Wh_Log(L"Failed to hook shell32!RunFileDlg");
        return false;
    }

    return true;
}

BOOL CALLBACK UnsubclassWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD windowProcessId = 0;
    GetWindowThreadProcessId(hwnd, &windowProcessId);
    if (windowProcessId != static_cast<DWORD>(lParam)) {
        return TRUE;
    }

    if (IsDialogClass(hwnd)) {
        RemovePropW(hwnd, kRunDialogSubclassedProp);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd,
                                                         RunDialogSubclassProc);
    }

    return TRUE;
}

void UnsubclassRunDialogs() {
    EnumWindows(UnsubclassWindowsProc,
                static_cast<LPARAM>(GetCurrentProcessId()));
}

void CloseSavedCommandsWindow() {
    HWND hwnd = GetSavedCommandsWindow();
    if (hwnd && IsWindow(hwnd)) {
        SendMessageTimeoutW(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 1000,
                            nullptr);
    }

    SetSavedCommandsWindow(nullptr);
}

void InitModuleHandle() {
    if (g_hModule) {
        return;
    }

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&InitModuleHandle),
                       &g_hModule);
}

}  // namespace

BOOL Wh_ModInit() {
    InitModuleHandle();

    INITCOMMONCONTROLSEX initCommonControls = {};
    initCommonControls.dwSize = sizeof(initCommonControls);
    initCommonControls.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&initCommonControls);

    if (!InitRegisteredMessages()) {
        return FALSE;
    }

    if (!RegisterSavedWindowClass()) {
        return FALSE;
    }

    InitializeCriticalSection(&g_commandsLock);
    g_commandsLockInitialized = true;

    LoadSettings();
    LoadCommandsFromFile();

    if (!HookRunFileDlg()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModBeforeUninit() {
    SetUnloading();
    CloseSavedCommandsWindow();
    UnsubclassRunDialogs();
    SaveCommandsToFile();
}

void Wh_ModUninit() {
    if (g_hModule && g_savedWindowClassRegistered) {
        if (!UnregisterClassW(kSavedWindowClass, g_hModule)) {
            Wh_Log(L"UnregisterClassW failed, error %u", GetLastError());
        }
        g_savedWindowClassRegistered = false;
    }

    if (g_commandsLockInitialized) {
        DeleteCriticalSection(&g_commandsLock);
        g_commandsLockInitialized = false;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    EnterCriticalSection(&g_commandsLock);
    TrimCommandsToLimitLocked();
    LeaveCriticalSection(&g_commandsLock);

    SaveCommandsToFile();

    HWND savedWindow = GetSavedCommandsWindow();
    if (savedWindow && IsWindow(savedWindow)) {
        PostMessageW(savedWindow, g_msgRefreshSavedList, 0, 0);
    }
}
