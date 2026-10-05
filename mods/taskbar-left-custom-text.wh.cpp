// ==WindhawkMod==
// @id              taskbar-left-custom-text
// @name            Taskbar Left Custom Text
// @description     Displays customizable dynamic text, file content, or script output natively attached to the bottom-left corner of the Windows 11 taskbar with single or two-line layouts, interactive pill hover, and click actions
// @version         1.3.2
// @author          Duke Nguyen
// @github          https://github.com/anhducad1111
// @homepage        https://github.com/anhducad1111/personal
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Left Custom Text

A lightweight, robust Windhawk mod that embeds customizable dynamic text directly into the bottom-left corner of the Windows 11 taskbar as an interactive native child window.

## Features
- **Native Taskbar Integration**:
  - Embedded as a direct child window of the current process's `Shell_TrayWnd`.
  - Runs directly on the taskbar's UI thread using the standard `RunFromWindowThread` hook pattern, preventing any cross-thread input queue coupling.
  - Multi-process explorer safe: only binds to the taskbar owned by the current `explorer.exe` process.
  - Transparent colorkey background allows the taskbar's native mica/acrylic surface to show through cleanly.
- **Two-Line / Multi-Line Layout**:
  - Automatically renders 2 compact, beautifully aligned lines (identical typography to the Windows 11 system clock/date) when text contains newlines (`\n`) or the delimiter (` | `).
  - Preserves single-line centered rendering when only 1 line of text is provided.
- **Authentic Windows 11 Typography & DPI Scaling**:
  - Uses `Segoe UI Variable Small` with normal weight (400), matching the Windows 11 taskbar clock.
  - Full Per-Monitor DPI scaling: font, padding, rounded pill, and window dimensions scale seamlessly across mixed-DPI setups.
- **Interactive Pill Button**:
  - **Hover Micro-interaction**: Highlights with a sleek rounded pill background matching Windows 11 taskbar buttons.
  - **Left-Click Action**: Triggers an instant asynchronous refresh on a background worker thread.
  - **Right-Click Context Menu**: Provides quick options to "Refresh Now" and "Copy Status Text" to clipboard.
- **Flexible Data Sources & Safe Defaults**:
  - **File Mode (Default)**: Monitors a local text file (default: `%TEMP%\taskbar_text.txt`). Supports environment variables.
  - **Command Mode**: Periodically runs a custom command or script (Python, PowerShell, Batch, curl) and displays its standard output.
  - **Safety**: Does not execute any unverified scripts by default. Non-blocking pipe streaming prevents deadlocks on large outputs.

## Placement Note
The widget docks to the left edge of the primary taskbar. On taskbars with left-aligned app icons or centered taskbars with the Widgets/weather button enabled, adjust the **Left Offset** setting to position the widget cleanly alongside other elements.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- textSource: file
  $name: Data Source Mode
  $description: Select whether to read from a local file or execute a command/script.
  $options:
    - file: Read from file
    - command: Execute command or script
- filePath: "%TEMP%\\taskbar_text.txt"
  $name: File Path (for File mode)
  $description: Path to the text file to read. Supports environment variables (e.g. %TEMP%\\taskbar_text.txt).
- commandLine: ""
  $name: Command Line (for Command mode)
  $description: Command line or script to run periodically (e.g. python "C:\\path\\script.py").
- lineMode: auto
  $name: Line Layout Mode
  $description: Select how lines are laid out. "auto" automatically renders 2 lines if output contains newlines or the delimiter.
  $options:
    - auto: Auto (split on \n or delimiter)
    - double: Force two lines (split on delimiter or \n)
    - single: Force single line
- delimiter: " | "
  $name: Multi-line Delimiter
  $description: String used to split single-line output into two lines (e.g. " | "). Leave empty to disable delimiter splitting.
- fallbackText: "Taskbar Text"
  $name: Fallback / Placeholder Text
  $description: Text shown when output is empty or pending first load.
- refreshIntervalMs: 15000
  $name: Refresh Interval (ms)
  $description: Interval between file reads or command executions (minimum 1000 ms).
- offsetLeft: 16
  $name: Left Offset (DIP)
  $description: Horizontal distance from the left edge of the taskbar (in device-independent pixels).
- fontSize: 11
  $name: Single-Line Font Size (pt)
  $description: Font size for single-line mode (multi-line automatically uses 9pt to match taskbar clock).
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
  $name: Widget Width (DIP)
  $description: Maximum width of the widget area in device-independent pixels.
- height: 34
  $name: Widget Height (DIP)
  $description: Height of the widget area in device-independent pixels.
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
#include <string>
#include <vector>
#include <atomic>

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define TIMER_POSITION_ID 101
#define WM_APP_UPDATE_TEXT (WM_APP + 1)
#define WM_APP_SETTINGS_CHANGED (WM_APP + 2)
#define WIDGET_CLASS_NAME L"WindhawkTaskbarLeftChildWnd_v4"
#define COLORKEY_BG RGB(12, 12, 12)

#define IDM_REFRESH 2001
#define IDM_COPY 2002

struct ModSettings {
    std::wstring textSource = L"file";
    std::wstring commandLine = L"";
    std::wstring filePath = L"%TEMP%\\taskbar_text.txt";
    std::wstring lineMode = L"auto";
    std::wstring delimiter = L" | ";
    std::wstring fallbackText = L"Taskbar Text";
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
};

// Global synchronization & state
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static ModSettings g_settings;
static HWND g_hWndWidget = nullptr;
static HWND g_hTaskbarWnd = nullptr;
static HANDLE g_hWorkerThread = nullptr;
static HANDLE g_hStopEvent = nullptr;
static HANDLE g_hRefreshEvent = nullptr;
static std::wstring g_currentText = L"";
static bool g_bHovered = false;
static bool g_bManualRefreshing = false;
static int g_lastPillWidth = 120;

// Cached GDI resources to prevent per-paint recreation
static HFONT g_hCachedFont = nullptr;
static int g_cachedFontHeight = 0;
static std::wstring g_cachedFontFamily = L"";
static int g_cachedFontWeight = 0;
static bool g_cachedIsLightMode = false;
static DWORD g_lastThemeCheckTick = 0;

// Forward declarations
void LoadSettings();
void TriggerManualRefresh();
void RepositionChildWidget(HWND hwnd);

// ==================== DPI & SYSTEM HELPERS ====================

int GetDpiForHwnd(HWND hwnd) {
    typedef UINT (WINAPI *GetDpiForWindow_t)(HWND);
    static GetDpiForWindow_t pfnGetDpiForWindow =
        (GetDpiForWindow_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (pfnGetDpiForWindow && hwnd) {
        UINT dpi = pfnGetDpiForWindow(hwnd);
        if (dpi > 0) return (int)dpi;
    }
    HDC hdc = GetDC(NULL);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    return dpi > 0 ? dpi : 96;
}

int DipToPx(int dip, int dpi) {
    return MulDiv(dip, dpi, 96);
}

bool IsSystemLightMode() {
    DWORD now = GetTickCount();
    if (now - g_lastThemeCheckTick < 1000) {
        return g_cachedIsLightMode;
    }
    g_lastThemeCheckTick = now;

    HKEY hKey;
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0,
            KEY_READ,
            &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"SystemUsesLightTheme", NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            g_cachedIsLightMode = (value != 0);
        }
        RegCloseKey(hKey);
    }
    return g_cachedIsLightMode;
}

// ==================== TEXT PROCESSING HELPERS ====================

std::wstring ExpandPath(const std::wstring& path) {
    if (path.empty()) return L"";
    DWORD required = ExpandEnvironmentStringsW(path.c_str(), NULL, 0);
    if (required == 0) return path;

    std::vector<wchar_t> buffer(required);
    if (ExpandEnvironmentStringsW(path.c_str(), buffer.data(), required) != 0) {
        return std::wstring(buffer.data());
    }
    return path;
}

std::vector<std::wstring> ParseLines(const std::wstring& text, const std::wstring& mode, const std::wstring& delimiter) {
    std::vector<std::wstring> lines;
    if (text.empty()) return lines;

    if (mode == L"single") {
        std::wstring s = text;
        size_t n = s.find_first_of(L"\r\n");
        if (n != std::wstring::npos) s = s.substr(0, n);
        lines.push_back(s);
        return lines;
    }

    size_t newlinePos = text.find_first_of(L"\r\n");
    if (newlinePos != std::wstring::npos) {
        size_t start = 0;
        while (start < text.length()) {
            size_t end = text.find_first_of(L"\r\n", start);
            if (end == std::wstring::npos) end = text.length();
            std::wstring line = text.substr(start, end - start);
            if (!line.empty() || lines.empty()) lines.push_back(line);
            start = end;
            while (start < text.length() && (text[start] == L'\r' || text[start] == L'\n')) {
                start++;
            }
        }
        if (lines.size() >= 2) return lines;
    }

    if (!delimiter.empty()) {
        size_t delimPos = text.find(delimiter);
        if (delimPos != std::wstring::npos) {
            lines.push_back(text.substr(0, delimPos));
            lines.push_back(text.substr(delimPos + delimiter.length()));
            return lines;
        }
    }

    if (mode == L"double") {
        lines.push_back(text);
        lines.push_back(L"");
        return lines;
    }

    lines.push_back(text);
    return lines;
}

// ==================== DATA RETRIEVAL (WORKER THREAD ONLY) ====================

std::wstring ReadFileContent(const std::wstring& relativePath) {
    if (relativePath.empty()) return L"";
    std::wstring fullPath = ExpandPath(relativePath);

    HANDLE hFile = CreateFileW(
        fullPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) return L"";

    char buffer[4096];
    DWORD bytesRead = 0;
    BOOL bSuccess = ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
    CloseHandle(hFile);

    if (!bSuccess || bytesRead == 0) return L"";

    buffer[bytesRead] = '\0';
    std::string s(buffer);

    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return L"";
    size_t last = s.find_last_not_of(" \t\r\n");
    s = s.substr(first, (last - first + 1));

    int wlen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), NULL, 0);
    if (wlen <= 0) return L"";

    std::wstring result(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), &result[0], wlen);
    return result;
}

std::wstring ExecuteCommand(const std::wstring& cmd) {
    if (cmd.empty()) return L"";

    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
    HANDLE hReadPipe = NULL, hWritePipe = NULL;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) return L"";

    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = { 0 };
    std::vector<wchar_t> cmdBuffer(cmd.begin(), cmd.end());
    cmdBuffer.push_back(L'\0');

    BOOL created = CreateProcessW(
        NULL,
        cmdBuffer.data(),
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi
    );

    CloseHandle(hWritePipe);

    if (!created) {
        CloseHandle(hReadPipe);
        return L"";
    }

    HANDLE hJob = CreateJobObjectW(NULL, NULL);
    if (hJob) {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
        AssignProcessToJobObject(hJob, pi.hProcess);
    }

    std::string output;
    char buffer[1024];
    DWORD bytesRead = 0;
    HANDLE waitHandles[2] = { pi.hProcess, g_hStopEvent };
    ULONGLONG startTime = GetTickCount64();

    while (true) {
        DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, 50);
        if (waitRes == WAIT_OBJECT_0 + 1) {
            TerminateProcess(pi.hProcess, 0);
            break;
        }

        DWORD bytesAvail = 0;
        if (PeekNamedPipe(hReadPipe, NULL, 0, NULL, &bytesAvail, NULL) && bytesAvail > 0) {
            if (ReadFile(hReadPipe, buffer, min((DWORD)sizeof(buffer), bytesAvail), &bytesRead, NULL) && bytesRead > 0) {
                if (output.length() + bytesRead <= 4096) {
                    output.append(buffer, bytesRead);
                }
            }
        }

        if (waitRes == WAIT_OBJECT_0) {
            while (PeekNamedPipe(hReadPipe, NULL, 0, NULL, &bytesAvail, NULL) && bytesAvail > 0) {
                if (ReadFile(hReadPipe, buffer, min((DWORD)sizeof(buffer), bytesAvail), &bytesRead, NULL) && bytesRead > 0) {
                    if (output.length() + bytesRead <= 4096) {
                        output.append(buffer, bytesRead);
                    }
                } else break;
            }
            break;
        }

        if (GetTickCount64() - startTime > 5000) {
            TerminateProcess(pi.hProcess, 0);
            break;
        }
    }

    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    if (hJob) {
        CloseHandle(hJob);
    }

    size_t first = output.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return L"";
    size_t last = output.find_last_not_of(" \t\r\n");
    output = output.substr(first, (last - first + 1));

    int wlen = MultiByteToWideChar(CP_UTF8, 0, output.c_str(), (int)output.length(), NULL, 0);
    if (wlen <= 0) return L"";

    std::wstring result(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, output.c_str(), (int)output.length(), &result[0], wlen);
    return result;
}

std::wstring FetchLatestText(const ModSettings& settings) {
    std::wstring text = L"";
    if (settings.textSource == L"command") {
        if (!settings.commandLine.empty()) {
            text = ExecuteCommand(settings.commandLine);
        }
    } else {
        if (!settings.filePath.empty()) {
            text = ReadFileContent(settings.filePath);
        }
    }
    if (text.empty()) {
        text = settings.fallbackText;
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

// ==================== TASKBAR RUNFROMWINDOWTHREAD PATTERN ====================

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd)
    );
    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr,
        dwThreadId
    );
    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

// ==================== WIDGET WINDOW PROCEDURE ====================

LRESULT CALLBACK ChildWidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            SetTimer(hwnd, TIMER_POSITION_ID, 2000, NULL);
            return 0;
        }
        case WM_ERASEBKGND: {
            return 1;
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
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
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
                TriggerManualRefresh();
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
                std::wstring verStr = L"Taskbar Left Custom Text v" WH_MOD_VERSION;
                AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, verStr.c_str());

                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenuEx(
                    hMenu,
                    TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                    pt.x,
                    pt.y,
                    hwnd,
                    NULL
                );
                DestroyMenu(hMenu);

                if (cmd == IDM_REFRESH) {
                    TriggerManualRefresh();
                } else if (cmd == IDM_COPY) {
                    CopyTextToClipboard(hwnd, g_currentText);
                }
            }
            return 0;
        }
        case WM_APP_UPDATE_TEXT: {
            std::wstring* pText = reinterpret_cast<std::wstring*>(lParam);
            if (pText) {
                g_currentText = *pText;
                delete pText;
            }
            g_bManualRefreshing = false;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_APP_SETTINGS_CHANGED: {
            LoadSettings();
            RepositionChildWidget(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_DPICHANGED_AFTERPARENT: {
            if (g_hCachedFont) {
                DeleteObject(g_hCachedFont);
                g_hCachedFont = nullptr;
            }
            RepositionChildWidget(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_POSITION_ID) {
                RepositionChildWidget(hwnd);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            int dpi = GetDpiForHwnd(hwnd);

            // 1. Fill entire widget canvas with colorkey for 100% transparency outside pill
            HBRUSH hbrBg = CreateSolidBrush(COLORKEY_BG);
            FillRect(hdc, &rc, hbrBg);
            DeleteObject(hbrBg);

            bool lightMode = g_settings.autoTheme ? IsSystemLightMode() : false;

            // 2. Parse lines
            std::wstring displayText = g_bManualRefreshing ? L"Refreshing..." : g_currentText;
            std::vector<std::wstring> lines = ParseLines(displayText, g_settings.lineMode, g_settings.delimiter);
            if (lines.empty()) lines.push_back(displayText);

            bool isMultiLine = (lines.size() >= 2);
            int effectiveFontSize = isMultiLine ? 9 : g_settings.fontSize;

            // 3. Cache/Create font matching Windows 11 taskbar clock
            int weight = FW_NORMAL;
            if (g_settings.fontWeight == L"semibold") weight = FW_SEMIBOLD;
            else if (g_settings.fontWeight == L"bold") weight = FW_BOLD;

            int fontHeight = -MulDiv(effectiveFontSize, dpi, 72);
            if (!g_hCachedFont || g_cachedFontHeight != fontHeight ||
                g_cachedFontFamily != g_settings.fontFamily || g_cachedFontWeight != weight) {
                if (g_hCachedFont) DeleteObject(g_hCachedFont);
                g_hCachedFont = CreateFontW(
                    fontHeight,
                    0, 0, 0, weight, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                    g_settings.fontFamily.c_str()
                );
                g_cachedFontHeight = fontHeight;
                g_cachedFontFamily = g_settings.fontFamily;
                g_cachedFontWeight = weight;
            }

            HFONT oldFont = (HFONT)SelectObject(hdc, g_hCachedFont);

            // 4. Measure maximum line width
            int maxLineWidth = 0;
            for (const auto& line : lines) {
                RECT rcMeasure = { 0, 0, 0, 0 };
                DrawTextW(hdc, line.c_str(), -1, &rcMeasure, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
                int w = rcMeasure.right - rcMeasure.left;
                if (w > maxLineWidth) maxLineWidth = w;
            }

            // 5. Compute rounded pill geometry with DPI scaling
            int paddingX = DipToPx(10, dpi);
            int pillWidth = maxLineWidth + paddingX * 2;
            if (pillWidth > rc.right) pillWidth = rc.right;
            g_lastPillWidth = pillWidth;

            RECT rcPill = { 0, 1, pillWidth, rc.bottom - 1 };

            // 6. Render interactive pill on mouse hover
            if (g_bHovered && g_settings.enableHoverPill) {
                COLORREF pillBgColor = lightMode ? RGB(232, 232, 235) : RGB(46, 46, 50);
                COLORREF pillBorderColor = lightMode ? RGB(205, 205, 210) : RGB(65, 65, 70);

                HBRUSH hbrPill = CreateSolidBrush(pillBgColor);
                HPEN hPenPill = CreatePen(PS_SOLID, 1, pillBorderColor);
                HGDIOBJ oldBrush = SelectObject(hdc, hbrPill);
                HGDIOBJ oldPen = SelectObject(hdc, hPenPill);

                int radius = DipToPx(8, dpi);
                RoundRect(hdc, rcPill.left, rcPill.top, rcPill.right, rcPill.bottom, radius, radius);

                SelectObject(hdc, oldBrush);
                SelectObject(hdc, oldPen);
                DeleteObject(hbrPill);
                DeleteObject(hPenPill);
            }

            // 7. Render typography
            COLORREF textColor;
            if (g_bManualRefreshing) {
                textColor = lightMode ? RGB(120, 120, 125) : RGB(160, 160, 165);
            } else if (g_settings.autoTheme) {
                textColor = lightMode ? RGB(20, 20, 20) : RGB(255, 255, 255);
            } else {
                textColor = RGB(255, 255, 255);
            }

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, textColor);

            int textLeft = rcPill.left + paddingX;
            int textRight = rcPill.right - paddingX;

            if (isMultiLine) {
                int totalHeight = rcPill.bottom - rcPill.top;
                int line1Height = totalHeight / 2;

                RECT rcLine1 = { textLeft, rcPill.top + 1, textRight, rcPill.top + line1Height };
                RECT rcLine2 = { textLeft, rcPill.top + line1Height - 1, textRight, rcPill.bottom - 1 };

                DrawTextW(hdc, lines[0].c_str(), -1, &rcLine1, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                DrawTextW(hdc, lines[1].c_str(), -1, &rcLine2, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            } else {
                RECT rcText = { textLeft, rcPill.top, textRight, rcPill.bottom };
                DrawTextW(hdc, lines[0].c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }

            SelectObject(hdc, oldFont);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, TIMER_POSITION_ID);
            if (g_hCachedFont) {
                DeleteObject(g_hCachedFont);
                g_hCachedFont = nullptr;
            }
            g_hWndWidget = nullptr;
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ==================== WINDOW CREATION & REPOSITION ====================

void RepositionChildWidget(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    HWND hParent = GetParent(hwnd);
    if (!hParent || !IsWindow(hParent)) return;

    RECT rcParent;
    GetClientRect(hParent, &rcParent);
    int parentHeight = rcParent.bottom - rcParent.top;

    int dpi = GetDpiForHwnd(hwnd);
    int targetWidth = DipToPx(g_settings.width, dpi);
    int targetHeight = DipToPx(g_settings.height, dpi);
    int offsetLeftPx = DipToPx(g_settings.offsetLeft, dpi);

    int posY = (parentHeight - targetHeight) / 2;
    if (posY < 0) posY = 0;

    SetWindowPos(
        hwnd,
        HWND_TOP,
        offsetLeftPx,
        posY,
        targetWidth,
        targetHeight,
        SWP_NOACTIVATE | SWP_SHOWWINDOW
    );
}

void WINAPI CreateWidgetOnTaskbarThread(PVOID pTaskbar) {
    HWND hTaskbar = reinterpret_cast<HWND>(pTaskbar);
    if (!hTaskbar || !IsWindow(hTaskbar)) return;

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = ChildWidgetWndProc;
    wc.hInstance = HINST_THISCOMPONENT;
    wc.lpszClassName = WIDGET_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        return;
    }

    int dpi = GetDpiForHwnd(hTaskbar);
    int targetWidth = DipToPx(g_settings.width, dpi);
    int targetHeight = DipToPx(g_settings.height, dpi);
    int offsetLeftPx = DipToPx(g_settings.offsetLeft, dpi);

    g_hWndWidget = CreateWindowExW(
        WS_EX_LAYERED,
        WIDGET_CLASS_NAME,
        L"WindhawkTaskbarLeftWidget",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        offsetLeftPx,
        4,
        targetWidth,
        targetHeight,
        hTaskbar,
        NULL,
        HINST_THISCOMPONENT,
        NULL
    );

    if (g_hWndWidget) {
        SetLayeredWindowAttributes(g_hWndWidget, COLORKEY_BG, 0, LWA_COLORKEY);
        RepositionChildWidget(g_hWndWidget);
    }
}

void WINAPI DestroyWidgetOnTaskbarThread(PVOID) {
    if (g_hWndWidget && IsWindow(g_hWndWidget)) {
        DestroyWindow(g_hWndWidget);
        g_hWndWidget = nullptr;
    }
}

// ==================== BACKGROUND REFRESH WORKER ====================

DWORD WINAPI BackgroundWorkerThreadProc(LPVOID) {
    // 1. Wait for taskbar window to become available
    while (WaitForSingleObject(g_hStopEvent, 200) == WAIT_TIMEOUT) {
        HWND hTaskbar = FindCurrentProcessTaskbarWnd();
        if (hTaskbar) {
            g_hTaskbarWnd = hTaskbar;
            RunFromWindowThread(hTaskbar, CreateWidgetOnTaskbarThread, hTaskbar);
            break;
        }
    }

    if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
        return 0;
    }

    // 2. Main periodic refresh loop
    HANDLE waitHandles[2] = { g_hStopEvent, g_hRefreshEvent };

    while (true) {
        // Perform data fetch
        AcquireSRWLockShared(&g_settingsLock);
        ModSettings currentSettings = g_settings;
        ReleaseSRWLockShared(&g_settingsLock);

        std::wstring fetched = FetchLatestText(currentSettings);

        if (g_hWndWidget && IsWindow(g_hWndWidget)) {
            std::wstring* pText = new std::wstring(fetched);
            PostMessageW(g_hWndWidget, WM_APP_UPDATE_TEXT, 0, reinterpret_cast<LPARAM>(pText));
        }

        DWORD interval = (DWORD)max(1000, currentSettings.refreshIntervalMs);
        DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, interval);

        if (waitRes == WAIT_OBJECT_0) {
            // Stop event signaled -> exit thread cleanly
            break;
        } else if (waitRes == WAIT_OBJECT_0 + 1) {
            // Manual refresh event signaled -> loop immediately
            ResetEvent(g_hRefreshEvent);
        }
    }

    return 0;
}

void TriggerManualRefresh() {
    if (!g_bManualRefreshing) {
        g_bManualRefreshing = true;
        if (g_hWndWidget && IsWindow(g_hWndWidget)) {
            InvalidateRect(g_hWndWidget, NULL, TRUE);
        }
    }
    if (g_hRefreshEvent) {
        SetEvent(g_hRefreshEvent);
    }
}

// ==================== SETTINGS MANAGEMENT ====================

void LoadSettings() {
    AcquireSRWLockExclusive(&g_settingsLock);

    g_settings.textSource = WindhawkUtils::StringSetting::make(L"textSource").get();
    if (g_settings.textSource != L"command") {
        g_settings.textSource = L"file";
    }

    g_settings.commandLine = WindhawkUtils::StringSetting::make(L"commandLine").get();
    g_settings.filePath = WindhawkUtils::StringSetting::make(L"filePath").get();
    g_settings.lineMode = WindhawkUtils::StringSetting::make(L"lineMode").get();
    g_settings.delimiter = WindhawkUtils::StringSetting::make(L"delimiter").get();
    g_settings.fallbackText = WindhawkUtils::StringSetting::make(L"fallbackText").get();

    g_settings.fontFamily = WindhawkUtils::StringSetting::make(L"fontFamily").get();
    if (g_settings.fontFamily.empty()) {
        g_settings.fontFamily = L"Segoe UI Variable Small";
    }

    g_settings.fontWeight = WindhawkUtils::StringSetting::make(L"fontWeight").get();
    g_settings.leftClickAction = WindhawkUtils::StringSetting::make(L"leftClickAction").get();

    g_settings.refreshIntervalMs = Wh_GetIntSetting(L"refreshIntervalMs");
    if (g_settings.refreshIntervalMs < 1000) {
        g_settings.refreshIntervalMs = 15000;
    }

    g_settings.offsetLeft = Wh_GetIntSetting(L"offsetLeft");
    if (g_settings.offsetLeft < 0) {
        g_settings.offsetLeft = 0;
    }

    g_settings.fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_settings.fontSize < 6 || g_settings.fontSize > 36) {
        g_settings.fontSize = 11;
    }

    g_settings.width = Wh_GetIntSetting(L"width");
    if (g_settings.width < 50 || g_settings.width > 1200) {
        g_settings.width = 340;
    }

    g_settings.height = Wh_GetIntSetting(L"height");
    if (g_settings.height < 16 || g_settings.height > 100) {
        g_settings.height = 34;
    }

    g_settings.autoTheme = (Wh_GetIntSetting(L"autoTheme") != 0);
    g_settings.enableHoverPill = (Wh_GetIntSetting(L"enableHoverPill") != 0);

    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ==================== WINDHAWK ENTRYPOINTS ====================

BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Left Custom Text: Initializing v" WH_MOD_VERSION);

    // Only run in the primary explorer process that actually owns a taskbar
    HWND hTaskbar = FindCurrentProcessTaskbarWnd();
    if (!hTaskbar) {
        // Check if a taskbar window exists in another process. If so, this is a secondary explorer instance
        HWND hAnyTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
        if (hAnyTaskbar) {
            DWORD dwProcId = 0;
            GetWindowThreadProcessId(hAnyTaskbar, &dwProcId);
            if (dwProcId != GetCurrentProcessId()) {
                Wh_Log(L"Secondary explorer instance detected; skipping widget creation");
                return TRUE;
            }
        }
    }

    LoadSettings();

    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_hRefreshEvent = CreateEventW(NULL, FALSE, FALSE, NULL);

    g_hWorkerThread = CreateThread(
        NULL,
        0,
        BackgroundWorkerThreadProc,
        NULL,
        0,
        NULL
    );

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Taskbar Left Custom Text: Uninitializing");

    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, INFINITE);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
    }

    if (g_hTaskbarWnd && IsWindow(g_hTaskbarWnd)) {
        RunFromWindowThread(g_hTaskbarWnd, DestroyWidgetOnTaskbarThread, nullptr);
    }

    UnregisterClassW(WIDGET_CLASS_NAME, HINST_THISCOMPONENT);

    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
    }
    if (g_hRefreshEvent) {
        CloseHandle(g_hRefreshEvent);
        g_hRefreshEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Taskbar Left Custom Text: Settings changed");
    if (g_hWndWidget && IsWindow(g_hWndWidget)) {
        PostMessageW(g_hWndWidget, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
    TriggerManualRefresh();
}
