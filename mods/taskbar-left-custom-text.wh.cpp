// ==WindhawkMod==
// @id              taskbar-left-custom-text
// @name            Taskbar Left Custom Text
// @description     Displays customizable dynamic text, file content, or script output natively attached to the bottom-left corner of the Windows 11 taskbar with single or two-line layouts, interactive pill hover, and click actions
// @version         1.3.1
// @author          Duke Nguyen
// @github          https://github.com/anhducad1111
// @homepage        https://github.com/anhducad1111/personal
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -ldwmapi -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Left Custom Text

A lightweight, versatile Windhawk mod that embeds customizable dynamic text directly into the bottom-left corner of the Windows 11 taskbar as an interactive native child window.

## Features
- **Native Taskbar Embedding**:
  - Attached directly as a child window (`WS_CHILD`) of `Shell_TrayWnd`.
  - Automatically moves, docks, hides, and restores together with the Windows taskbar.
  - Transparent colorkey background allows the taskbar's native mica/acrylic surface to show through.
  - Dynamic hit-testing (`HTTRANSPARENT`) passes mouse clicks outside the active pill directly through to the taskbar.
- **Two-Line / Multi-Line Layout**:
  - Automatically renders 2 compact, beautifully aligned lines (identical typography to Windows 11 system clock/date) when text contains newlines (`\n`) or a delimiter (` | `).
  - Preserves single-line centered rendering when only 1 line of text is provided.
- **Authentic Windows 11 Typography**:
  - Uses `Segoe UI Variable Small` with normal weight (400), matching the Windows 11 taskbar clock.
- **Interactive Pill Button**:
  - **Hover Micro-interaction**: Highlights with a sleek rounded pill background matching Windows 11 taskbar button styles.
  - **Left-Click Action**: Triggers an instant asynchronous status refresh with non-blocking UI feedback.
  - **Right-Click Context Menu**: Provides quick options to "Refresh Now" and "Copy Status Text" to clipboard.
- **Flexible Data Sources**:
  - **Command Mode**: Periodically runs a custom command or script (Python, PowerShell, Batch, curl) and displays its standard output.
  - **File Mode**: Displays contents of any local text file. Supports environment variables like `%TEMP%` and `%USERPROFILE%`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- textSource: command
  $name: Data Source Mode
  $description: Select whether to execute a command/script or read from a local file.
  $options:
    - command: Execute command or script
    - file: Read from file
- commandLine: "python C:\\repos\\personal\\scripts\\check_quota.py"
  $name: Command Line (for Command mode)
  $description: Command line or script to run periodically (e.g. python "C:\\path\\script.py").
- filePath: "%TEMP%\\quota_status.txt"
  $name: File Path (for File mode)
  $description: Path to the text file to read. Supports environment variables (e.g. %TEMP%\\quota_status.txt).
- lineMode: auto
  $name: Line Layout Mode
  $description: Select how lines are laid out. "auto" automatically renders 2 lines if output contains newlines or the delimiter.
  $options:
    - auto: Auto (split on \n or delimiter)
    - double: Force two lines (split on delimiter or \n)
    - single: Force single line
- delimiter: " | "
  $name: Multi-line Delimiter
  $description: String used to split single-line output into two lines (e.g. " | ").
- fallbackText: "Loading..."
  $name: Fallback / Placeholder Text
  $description: Text shown when output is empty or pending first load.
- refreshIntervalMs: 15000
  $name: Refresh Interval (ms)
  $description: Interval between file reads or command executions (minimum 1000 ms).
- offsetLeft: 16
  $name: Left Offset (px)
  $description: Horizontal distance from the left edge of the taskbar.
- fontSize: 11
  $name: Single-Line Font Size (pt)
  $description: Font size for single-line mode (multi-line automatically scales to 9.5pt to match taskbar clock).
- fontFamily: "Segoe UI Variable Small"
  $name: Font Family
  $description: Font family name (e.g. "Segoe UI Variable Small" for Windows 11 clock, "Segoe UI Variable Text", "Cascadia Code").
- fontWeight: normal
  $name: Font Weight
  $description: Text font weight. "normal" (400) perfectly matches the Windows 11 system clock.
  $options:
    - normal: Normal (400 - Matches Windows 11 Clock)
    - semibold: Semi-Bold (600)
    - bold: Bold (700)
- width: 340
  $name: Widget Width (px)
  $description: Maximum width of the widget area.
- height: 34
  $name: Widget Height (px)
  $description: Height of the widget area.
- autoTheme: true
  $name: Auto Theme
  $description: Automatically adapt text color to Windows Light and Dark modes.
- enableHoverPill: true
  $name: Enable Hover Pill Effect
  $description: Show a rounded pill highlight when mouse hovers over the widget.
- leftClickAction: refresh
  $name: Left Click Action
  $description: Action to perform when left-clicking the widget.
  $options:
    - refresh: Refresh status immediately
    - none: None
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <string>
#include <vector>

#define TIMER_UPDATE_ID 101
#define TIMER_POSITION_ID 102
#define WM_APP_UPDATE_TEXT (WM_APP + 1)
#define WIDGET_CLASS_NAME L"WindhawkTaskbarLeftChildWnd_v3"
#define COLORKEY_BG RGB(12, 12, 12)

#define IDM_REFRESH 2001
#define IDM_COPY 2002

struct ModSettings {
    std::wstring textSource = L"command";
    std::wstring commandLine = L"python C:\\repos\\personal\\scripts\\check_quota.py";
    std::wstring filePath = L"%TEMP%\\quota_status.txt";
    std::wstring lineMode = L"auto";
    std::wstring delimiter = L" | ";
    std::wstring fallbackText = L"Loading...";
    std::wstring fontFamily = L"Segoe UI Variable Small";
    std::wstring fontWeight = L"normal";
    std::wstring leftClickAction = L"refresh";
    int refreshIntervalMs = 15000;
    int offsetLeft = 16;
    int fontSize = 11;
    int width = 340;
    int height = 34;
    bool autoTheme = true;
    bool enableHoverPill = true;
} g_settings;

HANDLE g_hThread = NULL;
HWND g_hWndWidget = NULL;
HWND g_hTaskbar = NULL;
bool g_bRunning = false;
UINT g_uTaskbarCreatedMsg = 0;
std::wstring g_currentText = L"Loading...";

bool g_bHovered = false;
bool g_bRefreshing = false;
int g_lastPillWidth = 100;

LRESULT CALLBACK ChildWidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool IsSystemLightMode() {
    HKEY hKey;
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0,
            KEY_READ,
            &hKey
        ) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"SystemUsesLightTheme", NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value != 0;
        }
        RegCloseKey(hKey);
    }
    return false;
}

std::wstring ExpandPath(const std::wstring& input) {
    wchar_t expanded[MAX_PATH * 2];
    DWORD res = ExpandEnvironmentStringsW(input.c_str(), expanded, ARRAYSIZE(expanded));
    if (res > 0 && res < ARRAYSIZE(expanded)) {
        return std::wstring(expanded);
    }
    return input;
}

std::vector<std::wstring> ParseLines(const std::wstring& text, const std::wstring& lineMode, const std::wstring& delimiter) {
    std::vector<std::wstring> result;
    if (text.empty()) return result;

    if (lineMode == L"single") {
        result.push_back(text);
        return result;
    }

    // 1. Check for newline characters
    if (text.find(L'\n') != std::wstring::npos) {
        size_t start = 0;
        while (start < text.length()) {
            size_t pos = text.find(L'\n', start);
            if (pos == std::wstring::npos) {
                std::wstring sub = text.substr(start);
                while (!sub.empty() && (sub.back() == L'\r' || sub.back() == L' ')) sub.pop_back();
                while (!sub.empty() && sub.front() == L' ') sub.erase(sub.begin());
                if (!sub.empty()) result.push_back(sub);
                break;
            }
            std::wstring sub = text.substr(start, pos - start);
            while (!sub.empty() && (sub.back() == L'\r' || sub.back() == L' ')) sub.pop_back();
            while (!sub.empty() && sub.front() == L' ') sub.erase(sub.begin());
            if (!sub.empty()) result.push_back(sub);
            start = pos + 1;
        }
        if (!result.empty()) return result;
    }

    // 2. If lineMode is auto or double, check for delimiter
    if (!delimiter.empty() && (lineMode == L"auto" || lineMode == L"double")) {
        size_t pos = text.find(delimiter);
        if (pos != std::wstring::npos) {
            std::wstring part1 = text.substr(0, pos);
            std::wstring part2 = text.substr(pos + delimiter.length());
            while (!part1.empty() && part1.back() == L' ') part1.pop_back();
            while (!part1.empty() && part1.front() == L' ') part1.erase(part1.begin());
            while (!part2.empty() && part2.back() == L' ') part2.pop_back();
            while (!part2.empty() && part2.front() == L' ') part2.erase(part2.begin());
            if (!part1.empty()) result.push_back(part1);
            if (!part2.empty()) result.push_back(part2);
            return result;
        }
    }

    result.push_back(text);
    return result;
}

std::wstring ReadFileContent(const std::wstring& rawPath) {
    std::wstring fullPath = ExpandPath(rawPath);
    if (fullPath.empty()) {
        return L"";
    }

    HANDLE hFile = CreateFileW(
        fullPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return L"";
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE || dwFileSize > 4096) {
        CloseHandle(hFile);
        return L"";
    }

    std::string buffer(dwFileSize, '\0');
    DWORD dwBytesRead = 0;
    BOOL bSuccess = ReadFile(hFile, &buffer[0], dwFileSize, &dwBytesRead, NULL);
    CloseHandle(hFile);

    if (!bSuccess || dwBytesRead == 0) {
        return L"";
    }

    while (!buffer.empty() && (buffer.back() == '\r' || buffer.back() == '\n' || buffer.back() == ' ')) {
        buffer.pop_back();
    }

    int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer.c_str(), (int)buffer.length(), NULL, 0);
    if (wlen <= 0) {
        return L"";
    }

    std::wstring result(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, buffer.c_str(), (int)buffer.length(), &result[0], wlen);
    return result;
}

std::wstring ExecuteCommand(const std::wstring& cmd) {
    if (cmd.empty()) {
        return L"";
    }

    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    HANDLE hReadPipe = NULL;
    HANDLE hWritePipe = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return L"";
    }
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {0};
    std::vector<wchar_t> cmdBuffer(cmd.begin(), cmd.end());
    cmdBuffer.push_back(L'\0');

    if (!CreateProcessW(NULL, cmdBuffer.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        Wh_Log(L"CreateProcessW failed: %d", GetLastError());
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return L"";
    }

    CloseHandle(hWritePipe);

    std::string output;
    char buffer[512];
    DWORD bytesRead = 0;

    DWORD waitResult = WaitForSingleObject(pi.hProcess, 5000);
    if (waitResult == WAIT_OBJECT_0) {
        while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            output.append(buffer, bytesRead);
            if (output.size() > 2048) {
                break;
            }
        }
    } else {
        TerminateProcess(pi.hProcess, 1);
    }

    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    while (!output.empty() && (output.back() == '\r' || output.back() == '\n' || output.back() == ' ')) {
        output.pop_back();
    }

    if (output.empty()) {
        return L"";
    }

    int wlen = MultiByteToWideChar(CP_UTF8, 0, output.c_str(), (int)output.length(), NULL, 0);
    if (wlen <= 0) {
        return L"";
    }

    std::wstring result(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, output.c_str(), (int)output.length(), &result[0], wlen);
    return result;
}

std::wstring FetchLatestText() {
    std::wstring text = L"";

    if (g_settings.textSource == L"command" || g_settings.commandLine.rfind(L"python", 0) == 0) {
        std::wstring actualCmd = g_settings.commandLine;
        if (actualCmd.empty() && g_settings.filePath.rfind(L"python", 0) == 0) {
            actualCmd = g_settings.filePath;
        }
        text = ExecuteCommand(actualCmd);
    } else {
        std::wstring actualPath = g_settings.filePath;
        if (actualPath.rfind(L"python", 0) == 0) {
            text = ExecuteCommand(actualPath);
        } else {
            text = ReadFileContent(actualPath);
        }
    }

    if (text.empty()) {
        text = g_settings.fallbackText;
    }
    return text;
}

void CopyTextToClipboard(HWND hwnd, const std::wstring& text) {
    if (!OpenClipboard(hwnd)) return;
    EmptyClipboard();
    size_t bytes = (text.length() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem) {
        void* pMem = GlobalLock(hMem);
        if (pMem) {
            memcpy(pMem, text.c_str(), bytes);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
    }
    CloseClipboard();
}

struct RefreshWorkerContext {
    HWND hwnd;
};

static DWORD WINAPI AsyncRefreshWorker(LPVOID lpParam) {
    RefreshWorkerContext* p = (RefreshWorkerContext*)lpParam;
    std::wstring text = FetchLatestText();

    std::wstring* pResult = new std::wstring(text);
    if (IsWindow(p->hwnd)) {
        PostMessageW(p->hwnd, WM_APP_UPDATE_TEXT, 0, (LPARAM)pResult);
    } else {
        delete pResult;
    }
    delete p;
    return 0;
}

void TriggerAsyncRefresh(HWND hwnd) {
    if (g_bRefreshing) return;
    g_bRefreshing = true;

    g_currentText = L"Refreshing...";
    InvalidateRect(hwnd, NULL, TRUE);

    RefreshWorkerContext* ctx = new RefreshWorkerContext{hwnd};
    QueueUserWorkItem(AsyncRefreshWorker, ctx, WT_EXECUTEDEFAULT);
}

void RepositionChildWidget(HWND hwnd) {
    if (!g_hTaskbar || !IsWindow(g_hTaskbar)) {
        g_hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    }

    if (!g_hTaskbar || !IsWindowVisible(g_hTaskbar)) {
        if (IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
        }
        return;
    }

    RECT rcTaskbar;
    GetClientRect(g_hTaskbar, &rcTaskbar);

    int taskbarHeight = rcTaskbar.bottom - rcTaskbar.top;
    int targetX = g_settings.offsetLeft;
    int targetY = (taskbarHeight - g_settings.height) / 2;

    SetWindowPos(
        hwnd,
        HWND_TOP,
        targetX,
        targetY,
        g_settings.width,
        g_settings.height,
        SWP_NOACTIVATE | SWP_SHOWWINDOW
    );

    BringWindowToTop(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK ChildWidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            SetTimer(hwnd, TIMER_UPDATE_ID, g_settings.refreshIntervalMs, NULL);
            SetTimer(hwnd, TIMER_POSITION_ID, 2000, NULL);
            return 0;
        }
        case WM_ERASEBKGND: {
            return 1;
        }
        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProcW(hwnd, msg, wParam, lParam);
            if (hit == HTCLIENT) {
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ScreenToClient(hwnd, &pt);
                // If cursor is outside the interactive pill width, pass through to taskbar
                if (pt.x > g_lastPillWidth) {
                    return HTTRANSPARENT;
                }
            }
            return hit;
        }
        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT && g_settings.enableHoverPill) {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                return TRUE;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (!g_bHovered) {
                g_bHovered = true;
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            g_bHovered = false;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_LBUTTONUP: {
            if (g_settings.leftClickAction == L"refresh") {
                TriggerAsyncRefresh(hwnd);
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT pt;
            GetCursorPos(&pt);

            HMENU hMenu = CreatePopupMenu();
            if (hMenu) {
                AppendMenuW(hMenu, MF_STRING, IDM_REFRESH, L"Refresh Now");
                AppendMenuW(hMenu, MF_STRING, IDM_COPY, L"Copy Status Text");
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, L"Taskbar Left Custom Text v1.3.0");

                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenuEx(
                    hMenu,
                    TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                    pt.x, pt.y,
                    hwnd,
                    NULL
                );
                DestroyMenu(hMenu);

                if (cmd == IDM_REFRESH) {
                    TriggerAsyncRefresh(hwnd);
                } else if (cmd == IDM_COPY) {
                    CopyTextToClipboard(hwnd, g_currentText);
                }
            }
            return 0;
        }
        case WM_APP_UPDATE_TEXT: {
            std::wstring* pNewText = (std::wstring*)lParam;
            if (pNewText) {
                g_currentText = *pNewText;
                delete pNewText;
            }
            g_bRefreshing = false;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            // 1. Fill entire widget canvas with colorkey for 100% transparency outside pill
            HBRUSH hbrBg = CreateSolidBrush(COLORKEY_BG);
            FillRect(hdc, &rc, hbrBg);
            DeleteObject(hbrBg);

            bool lightMode = g_settings.autoTheme ? IsSystemLightMode() : false;

            // 2. Parse lines
            std::vector<std::wstring> lines = ParseLines(g_currentText, g_settings.lineMode, g_settings.delimiter);
            if (lines.empty()) lines.push_back(g_currentText);

            bool isMultiLine = (lines.size() >= 2);
            int effectiveFontSize = isMultiLine ? 9 : g_settings.fontSize;

            // 3. Setup font matching Windows 11 taskbar clock
            int weight = FW_NORMAL;
            if (g_settings.fontWeight == L"semibold") weight = FW_SEMIBOLD;
            else if (g_settings.fontWeight == L"bold") weight = FW_BOLD;

            int fontHeight = -MulDiv(effectiveFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            HFONT hFont = CreateFontW(
                fontHeight,
                0, 0, 0, weight, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                g_settings.fontFamily.c_str()
            );

            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

            // 4. Measure maximum line width
            int maxLineWidth = 0;
            for (const auto& line : lines) {
                RECT rcMeasure = {0, 0, 0, 0};
                DrawTextW(hdc, line.c_str(), -1, &rcMeasure, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
                int w = rcMeasure.right - rcMeasure.left;
                if (w > maxLineWidth) maxLineWidth = w;
            }

            // 5. Compute rounded pill geometry
            int paddingX = 10;
            int pillWidth = maxLineWidth + paddingX * 2;
            if (pillWidth > rc.right) pillWidth = rc.right;
            g_lastPillWidth = pillWidth;

            RECT rcPill = {0, 1, pillWidth, rc.bottom - 1};

            // 6. Render interactive pill on mouse hover
            if (g_bHovered && g_settings.enableHoverPill) {
                COLORREF pillBgColor = lightMode ? RGB(232, 232, 235) : RGB(46, 46, 50);
                COLORREF pillBorderColor = lightMode ? RGB(205, 205, 210) : RGB(70, 70, 75);

                HPEN hPen = CreatePen(PS_SOLID, 1, pillBorderColor);
                HBRUSH hBrush = CreateSolidBrush(pillBgColor);
                HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
                HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

                RoundRect(hdc, rcPill.left, rcPill.top, rcPill.right, rcPill.bottom, 8, 8);

                SelectObject(hdc, oldPen);
                SelectObject(hdc, oldBrush);
                DeleteObject(hPen);
                DeleteObject(hBrush);
            }

            // 7. Draw text lines with ClearType
            SetBkMode(hdc, TRANSPARENT);
            COLORREF txtColor = lightMode ? RGB(25, 25, 25) : RGB(250, 250, 250);
            SetTextColor(hdc, txtColor);

            if (!isMultiLine) {
                RECT rcText = {rcPill.left + paddingX, rc.top, rcPill.right - paddingX, rc.bottom};
                DrawTextW(hdc, lines[0].c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            } else {
                int totalH = rc.bottom - rc.top;
                int halfH = totalH / 2;
                RECT rcLine1 = {rcPill.left + paddingX, rc.top + 2, rcPill.right - paddingX, rc.top + halfH + 1};
                RECT rcLine2 = {rcPill.left + paddingX, rc.top + halfH - 1, rcPill.right - paddingX, rc.bottom - 2};

                DrawTextW(hdc, lines[0].c_str(), -1, &rcLine1, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                DrawTextW(hdc, lines[1].c_str(), -1, &rcLine2, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }

            SelectObject(hdc, oldFont);
            DeleteObject(hFont);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_UPDATE_ID) {
                if (!g_bRefreshing) {
                    TriggerAsyncRefresh(hwnd);
                }
                RepositionChildWidget(hwnd);
            } else if (wParam == TIMER_POSITION_ID) {
                RepositionChildWidget(hwnd);
            }
            return 0;
        }
        case WM_SETTINGCHANGE: {
            RepositionChildWidget(hwnd);
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, TIMER_UPDATE_ID);
            KillTimer(hwnd, TIMER_POSITION_ID);
            PostQuitMessage(0);
            return 0;
        }
        default: {
            if (g_uTaskbarCreatedMsg && msg == g_uTaskbarCreatedMsg) {
                Wh_Log(L"Taskbar recreated");
                g_hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
                if (g_hTaskbar) {
                    SetParent(hwnd, g_hTaskbar);
                    RepositionChildWidget(hwnd);
                }
                return 0;
            }
            break;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WidgetThreadProc(LPVOID lpParam) {
    Wh_Log(L"WidgetThreadProc started (Interactive Native Taskbar Child v1.3.0)");

    g_uTaskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    // Locate Shell_TrayWnd
    g_hTaskbar = NULL;
    for (int i = 0; i < 20; i++) {
        g_hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
        if (g_hTaskbar) break;
        Sleep(200);
    }

    if (!g_hTaskbar) {
        Wh_Log(L"Fatal: Shell_TrayWnd could not be found!");
        return 0;
    }

    Wh_Log(L"Shell_TrayWnd found: %p", g_hTaskbar);

    HINSTANCE hInstance = GetModuleHandleW(NULL);
    UnregisterClassW(WIDGET_CLASS_NAME, hInstance);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ChildWidgetWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = WIDGET_CLASS_NAME;

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW: %d", GetLastError());
    }

    RECT rcTaskbar;
    GetClientRect(g_hTaskbar, &rcTaskbar);
    int taskbarHeight = rcTaskbar.bottom - rcTaskbar.top;
    int targetX = g_settings.offsetLeft;
    int targetY = (taskbarHeight - g_settings.height) / 2;

    // Create native child window without WS_EX_TRANSPARENT to allow interactive mouse events
    g_hWndWidget = CreateWindowExW(
        WS_EX_LAYERED,
        WIDGET_CLASS_NAME,
        L"Taskbar Left Custom Text",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        targetX, targetY, g_settings.width, g_settings.height,
        g_hTaskbar,
        NULL,
        hInstance,
        NULL
    );

    Wh_Log(L"CreateWindowExW interactive child: %p, err: %d, x=%d, y=%d", g_hWndWidget, GetLastError(), targetX, targetY);

    if (g_hWndWidget) {
        SetLayeredWindowAttributes(g_hWndWidget, COLORKEY_BG, 0, LWA_COLORKEY);

        g_currentText = FetchLatestText();
        Wh_Log(L"Initial text: %s", g_currentText.c_str());

        RepositionChildWidget(g_hWndWidget);

        MSG msg;
        while (GetMessageW(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnregisterClassW(WIDGET_CLASS_NAME, hInstance);
    Wh_Log(L"WidgetThreadProc finished");
    return 0;
}

std::wstring GetStringSettingHelper(PCWSTR name, const std::wstring& defaultValue) {
    PCWSTR val = Wh_GetStringSetting(name);
    if (val) {
        std::wstring result(val);
        Wh_FreeStringSetting(val);
        if (!result.empty()) {
            return result;
        }
    }
    return defaultValue;
}

void LoadSettings() {
    g_settings.textSource = GetStringSettingHelper(L"textSource", L"command");
    g_settings.commandLine = GetStringSettingHelper(L"commandLine", L"python C:\\repos\\personal\\scripts\\check_quota.py");
    g_settings.filePath = GetStringSettingHelper(L"filePath", L"%TEMP%\\quota_status.txt");
    g_settings.lineMode = GetStringSettingHelper(L"lineMode", L"auto");
    g_settings.delimiter = GetStringSettingHelper(L"delimiter", L" | ");
    g_settings.fallbackText = GetStringSettingHelper(L"fallbackText", L"Loading...");
    g_settings.fontFamily = GetStringSettingHelper(L"fontFamily", L"Segoe UI Variable Small");
    g_settings.fontWeight = GetStringSettingHelper(L"fontWeight", L"normal");
    g_settings.leftClickAction = GetStringSettingHelper(L"leftClickAction", L"refresh");

    g_settings.refreshIntervalMs = Wh_GetIntSetting(L"refreshIntervalMs");
    g_settings.offsetLeft = Wh_GetIntSetting(L"offsetLeft");
    g_settings.fontSize = Wh_GetIntSetting(L"fontSize");
    g_settings.width = Wh_GetIntSetting(L"width");
    g_settings.height = Wh_GetIntSetting(L"height");
    g_settings.autoTheme = Wh_GetIntSetting(L"autoTheme");
    g_settings.enableHoverPill = Wh_GetIntSetting(L"enableHoverPill");

    if (g_settings.refreshIntervalMs < 1000) g_settings.refreshIntervalMs = 15000;
    if (g_settings.offsetLeft <= 0) g_settings.offsetLeft = 16;
    if (g_settings.fontSize <= 0) g_settings.fontSize = 11;
    if (g_settings.width <= 0) g_settings.width = 340;
    if (g_settings.height <= 0) g_settings.height = 34;

    Wh_Log(L"Settings: source=%s, lineMode=%s, delim=%s", g_settings.textSource.c_str(), g_settings.lineMode.c_str(), g_settings.delimiter.c_str());
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init Taskbar Left Custom Text v1.3.0 (Multi-line Support)");
    LoadSettings();

    g_bRunning = true;
    g_hThread = CreateThread(NULL, 0, WidgetThreadProc, NULL, 0, NULL);
    return g_hThread != NULL;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit Taskbar Left Custom Text");
    g_bRunning = false;

    if (g_hWndWidget && IsWindow(g_hWndWidget)) {
        PostMessageW(g_hWndWidget, WM_CLOSE, 0, 0);
    }

    if (g_hThread) {
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged Taskbar Left Custom Text");
    LoadSettings();

    if (g_hWndWidget && IsWindow(g_hWndWidget)) {
        KillTimer(g_hWndWidget, TIMER_UPDATE_ID);
        SetTimer(g_hWndWidget, TIMER_UPDATE_ID, g_settings.refreshIntervalMs, NULL);

        TriggerAsyncRefresh(g_hWndWidget);
        RepositionChildWidget(g_hWndWidget);
    }
}
