// ==WindhawkMod==
// @id              monitor-sleep-button
// @name            Monitor Sleep Button
// @description     Tray icon to turn off the monitor after a 3-second countdown.
// @version         1.0.0
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @homepage        https://github.com/SilverAmd
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -lgdi32
// @license         MIT
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Monitor Sleep Button

Adds a small tray icon for turning off the monitor.

## Usage

- Left-click the tray icon.
- A large 3-second countdown appears on the screen.
- After the countdown, the monitor turns off.
- Move the mouse or press any key to wake the monitor again.

## Notes

This mod does not shut down, suspend, or lock the computer.
It only sends the Windows monitor power-off command.

Useful as a quick replacement for desktop shortcuts or batch files.
*/
// ==/WindhawkModReadme==


#include <windows.h>
#include <shellapi.h>


#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAYICON 1001

#define TIMER_COUNTDOWN 2001
#define COUNTDOWN_SECONDS 3
#define COUNTDOWN_INTERVAL_MS 1000

#define COUNTDOWN_TRANSPARENT_COLOR RGB(1, 2, 3)

static HWND g_hwnd = nullptr;
static HWND g_countdownHwnd = nullptr;

static HICON g_hIcon = nullptr;
static NOTIFYICONDATAW g_nid = {};
static UINT g_taskbarCreatedMessage = 0;

static bool g_countdownActive = false;
static int g_countdownValue = COUNTDOWN_SECONDS;

// ------------------------------------------------------------
// Monitor ausschalten
// ------------------------------------------------------------

void TurnMonitorOff() {
    Wh_Log(L"Turning monitor off...");

    // Entspricht praktisch:
    // PostMessage(-1, 0x0112, 0xF170, 2)
    PostMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
}


// ------------------------------------------------------------
// Einfaches eigenes Tray-Icon erzeugen
// ------------------------------------------------------------

HICON CreateSimpleTrayIcon() {
    const int size = 32;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size; // top-down bitmap
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;

    HDC hdc = GetDC(nullptr);
    HBITMAP hbmColor = CreateDIBSection(
        hdc,
        &bi,
        DIB_RGB_COLORS,
        reinterpret_cast<void**>(&pixels),
        nullptr,
        0
    );
    ReleaseDC(nullptr, hdc);

    if (!hbmColor || !pixels) {
        return LoadIconW(nullptr, IDI_APPLICATION);
    }

    // Transparenter Hintergrund
    for (int i = 0; i < size * size; i++) {
        pixels[i] = 0x00000000;
    }

    // Kleines Monitor-Symbol malen
    // Farbe: hellblau
    DWORD blue = 0xFF00A6FF;
    DWORD dark = 0xFF202020;

    // Bildschirmrahmen
    for (int y = 7; y <= 21; y++) {
        for (int x = 5; x <= 26; x++) {
            bool border =
                y == 7 || y == 21 ||
                x == 5 || x == 26;

            pixels[y * size + x] = border ? blue : dark;
        }
    }

    // Standfuß
    for (int y = 22; y <= 24; y++) {
        for (int x = 14; x <= 17; x++) {
            pixels[y * size + x] = blue;
        }
    }

    // Fuß unten
    for (int y = 25; y <= 26; y++) {
        for (int x = 10; x <= 21; x++) {
            pixels[y * size + x] = blue;
        }
    }

    HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, nullptr);
    if (!hbmMask) {
        DeleteObject(hbmColor);
        return LoadIconW(nullptr, IDI_APPLICATION);
    }

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = hbmColor;
    iconInfo.hbmMask = hbmMask;

    HICON hIcon = CreateIconIndirect(&iconInfo);

    DeleteObject(hbmColor);
    DeleteObject(hbmMask);

    if (!hIcon) {
        return LoadIconW(nullptr, IDI_APPLICATION);
    }

    return hIcon;
}


// ------------------------------------------------------------
// Tray-Icon hinzufügen / entfernen
// ------------------------------------------------------------

bool AddTrayIcon() {
    if (!g_hwnd) {
        return false;
    }

    if (!g_hIcon) {
        g_hIcon = CreateSimpleTrayIcon();
    }

    ZeroMemory(&g_nid, sizeof(g_nid));

    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = g_hwnd;
    g_nid.uID = ID_TRAYICON;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = g_hIcon;

    wcscpy_s(g_nid.szTip, L"Monitor Sleep Button - Left-click: Monitor off");

    BOOL result = Shell_NotifyIconW(NIM_ADD, &g_nid);

    if (result) {
        Wh_Log(L"Tray icon added.");
    } else {
        Wh_Log(L"Failed to add tray icon. Error: %u", GetLastError());
    }

    return result != FALSE;
}


void RemoveTrayIcon() {
    if (g_hwnd) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
    }

    if (g_hIcon) {
        DestroyIcon(g_hIcon);
        g_hIcon = nullptr;
    }
}

void DestroyCountdownWindow() {
    if (g_countdownHwnd) {
        DestroyWindow(g_countdownHwnd);
        g_countdownHwnd = nullptr;
    }
}


LRESULT CALLBACK CountdownWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            HBRUSH hBgBrush = CreateSolidBrush(COUNTDOWN_TRANSPARENT_COLOR);
            FillRect(hdc, &rect, hBgBrush);
            DeleteObject(hBgBrush);

            SetBkMode(hdc, TRANSPARENT);

            HFONT hFont = CreateFontW(
                260,
                0,
                0,
                0,
                FW_BOLD,
                FALSE,
                FALSE,
                FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_SWISS,
                L"Segoe UI"
            );

            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

            wchar_t text[16];
            wsprintfW(text, L"%d", g_countdownValue);

            // Schatten
            RECT shadowRect = rect;
            OffsetRect(&shadowRect, 6, 6);
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawTextW(
                hdc,
                text,
                -1,
                &shadowRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE
            );

            // Haupttext
            SetTextColor(hdc, RGB(255, 255, 255));
            DrawTextW(
                hdc,
                text,
                -1,
                &rect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE
            );

            SelectObject(hdc, oldFont);
            DeleteObject(hFont);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


bool CreateCountdownWindow() {
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    const wchar_t CLASS_NAME[] = L"MonitorSleepCountdownWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = CountdownWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    g_countdownHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"Monitor Sleep Countdown",
        WS_POPUP,
        0,
        0,
        screenWidth,
        screenHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_countdownHwnd) {
        Wh_Log(L"Failed to create countdown window. Error: %u", GetLastError());
        return false;
    }

    // Gesamtes Overlay leicht transparent
    SetLayeredWindowAttributes(
    g_countdownHwnd,
    COUNTDOWN_TRANSPARENT_COLOR,
    0,
    LWA_COLORKEY
    );

    ShowWindow(g_countdownHwnd, SW_SHOW);
    UpdateWindow(g_countdownHwnd);

    return true;
}


void StartCountdown(HWND hwnd) {
    if (g_countdownActive) {
        return;
    }

    g_countdownActive = true;
    g_countdownValue = COUNTDOWN_SECONDS;

    Wh_Log(L"Starting monitor sleep countdown.");

    CreateCountdownWindow();

    lstrcpynW(
        g_nid.szTip,
        L"Monitor Sleep Button - Countdown läuft...",
        ARRAYSIZE(g_nid.szTip)
    );
    g_nid.uFlags = NIF_TIP;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);

    SetTimer(hwnd, TIMER_COUNTDOWN, COUNTDOWN_INTERVAL_MS, nullptr);
}


void FinishCountdown(HWND hwnd) {
    KillTimer(hwnd, TIMER_COUNTDOWN);

    g_countdownActive = false;

    DestroyCountdownWindow();

    lstrcpynW(
        g_nid.szTip,
        L"Monitor Sleep Button - Linksklick: Monitor aus",
        ARRAYSIZE(g_nid.szTip)
    );
    g_nid.uFlags = NIF_TIP;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);

    TurnMonitorOff();
}


// ------------------------------------------------------------
// Fenster-Prozedur für Tray-Nachrichten
// ------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAYICON) {
        switch (lParam) {
            case WM_LBUTTONUP:
                StartCountdown(hwnd);
                return 0;
        }

        return 0;
    }

    if (msg == WM_TIMER) {
        if (wParam == TIMER_COUNTDOWN) {
            g_countdownValue--;

            if (g_countdownValue <= 0) {
                FinishCountdown(hwnd);
                return 0;
            }

            if (g_countdownHwnd) {
                InvalidateRect(g_countdownHwnd, nullptr, TRUE);
                UpdateWindow(g_countdownHwnd);
            }

            return 0;
        }
    }

    if (g_taskbarCreatedMessage != 0 && msg == g_taskbarCreatedMessage) {
        Wh_Log(L"TaskbarCreated received. Recreating tray icon.");
        AddTrayIcon();
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


// ------------------------------------------------------------
// Unsichtbares Fenster erstellen
// ------------------------------------------------------------

bool CreateHiddenWindow() {
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    const wchar_t CLASS_NAME[] = L"MonitorSleepButtonHiddenWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Monitor Sleep Button",
        WS_OVERLAPPED,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hwnd) {
        Wh_Log(L"Failed to create hidden window. Error: %u", GetLastError());
        return false;
    }

    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    return true;
}


// ------------------------------------------------------------
// Tool-Mod Logik
// ------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Monitor Sleep Button...");

    if (!CreateHiddenWindow()) {
        return FALSE;
    }

    AddTrayIcon();

    Wh_Log(L"Monitor Sleep Button initialized.");
    return TRUE;
}


void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Monitor Sleep Button...");

    if (g_hwnd) {
        KillTimer(g_hwnd, TIMER_COUNTDOWN);
    }

    DestroyCountdownWindow();

    RemoveTrayIcon();

    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }

    PostQuitMessage(0);

    Wh_Log(L"Monitor Sleep Button uninitialized.");
}


// ------------------------------------------------------------
// Windhawk Tool-Mod Boilerplate
// ------------------------------------------------------------

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;


void WINAPI EntryPoint_Hook() {
    Wh_Log(L"Tool mod process entry point hooked. Starting message loop.");

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Wh_Log(L"Tool mod message loop exited.");
}


BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed.");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);

        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed.");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running: %s", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));

        IMAGE_NT_HEADERS* ntHeaders =
            reinterpret_cast<IMAGE_NT_HEADERS*>(
                reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew
            );

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;

        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook), nullptr);

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

    DWORD length = GetModuleFileNameW(
        nullptr,
        currentProcessPath,
        ARRAYSIZE(currentProcessPath)
    );

    if (length == 0 || length == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileName failed.");
        return;
    }

    WCHAR commandLine[
        MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR))
    ];

    wsprintfW(
    commandLine,
    L"\"%s\" -tool-mod \"%s\"",
    currentProcessPath,
    WH_MOD_ID
);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");

    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");

        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll.");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken
    );

    auto pCreateProcessInternalW =
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW")
        );

    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW.");
        return;
    }

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi = {};

    BOOL created = pCreateProcessInternalW(
        nullptr,
        currentProcessPath,
        commandLine,
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi,
        nullptr
    );

    if (!created) {
        Wh_Log(L"CreateProcess failed. Error: %u", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
