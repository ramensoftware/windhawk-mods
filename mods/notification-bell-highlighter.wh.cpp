// ==WindhawkMod==
// @id              notification-bell-highlighter
// @name            Notification Bell Highlighter
// @description     Pulsing, bright overlay with a counter on the taskbar notification bell
// @version         0.5.2
// @author          mwalker212
// @github          https://github.com/mwalker212
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lgdi32 -lshell32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Notification Bell Highlighter
Makes the Windows notification bell more obvious when notifications are waiting.
Draws a pulsing, bright overlay with an optional counter over the taskbar notification bell when notifications are pending.

Settings for shape (circle or square), size, position, colour, pulse rate and counter visibility

![Screenshot](https://raw.githubusercontent.com/mwalker212/windhawk-assets/refs/heads/main/bell-notification.gif)

Windows 11 only. Tested on 26H2 build 26300.9711.

Limitations:
- Overlay rather than restyled icon, this mod highlights the bell, while 'Taskbar tray system icon tweaks' can hide it.
- Primary taskbar only.
- Relies on the English accessible name, e.g. "Notifications 1 new notification".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sizePercent: 100
  $name: Overlay size (% of taskbar button height)
- color: "FF2D2D"
  $name: Colour (hex RRGGBB)
- shape: circle
  $name: Shape
  $options:
  - circle: Circle
  - square: Square
- pulseMs: 1100
  $name: Pulse period (milliseconds)
- showCount: true
  $name: Show notification count
- offsetX: 10
  $name: Horizontal offset (px)
- offsetY: 0
  $name: Vertical offset (px)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <stdio.h>
#include <climits>
#include <cmath>
#include <string>
#include <windhawk_utils.h>

struct Settings {
    int sizePercent = 100;
    COLORREF color = RGB(255, 45, 45);
    bool square = false;
    int pulseMs = 1100;
    bool showCount = true;
    int offsetX = 10;
    int offsetY = 0;
} g;

static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static HWND volatile g_wnd = nullptr;
static int g_count = 0;
static int g_curSize = 0;
static BYTE g_alpha = 255;
static bool g_pulseRunning = false;

// Overlay bitmap cache
static HDC g_memDC = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_oldBmp = nullptr;
static int g_dibSize = 0, g_dibCount = -1;
static COLORREF g_dibColor = 0;
static bool g_dibShowCount = false;
static bool g_dibSquare = false;

// Cached bell element (overlay thread only)
static IUIAutomationElement* g_bell = nullptr;
static HWND g_bellTray = nullptr;
static DWORD g_lastFindTick = 0;

static void LoadSettings() {
    g.sizePercent = Wh_GetIntSetting(L"sizePercent");
    if (g.sizePercent < 50) g.sizePercent = 50;
    if (g.sizePercent > 300) g.sizePercent = 300;
    g.pulseMs = Wh_GetIntSetting(L"pulseMs");
    if (g.pulseMs < 200) g.pulseMs = 200;
    g.showCount = Wh_GetIntSetting(L"showCount") != 0;
    g.offsetX = Wh_GetIntSetting(L"offsetX");
    g.offsetY = Wh_GetIntSetting(L"offsetY");

    {
        auto color = WindhawkUtils::StringSetting::make(L"color");
        const wchar_t* hex = color.get();
        if (*hex == L'#') hex++;                  // allow "#FF2D2D"
        unsigned long v = wcstoul(hex, nullptr, 16);
        g.color = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
    }
    {
        auto shape = WindhawkUtils::StringSetting::make(L"shape");
        g.square = wcscmp(shape.get(), L"square") == 0;
    }
}

static int ParseCount(const wchar_t* name) {
    std::wstring s = name;
    for (auto& ch : s) ch = towlower(ch);
    size_t p = s.find(L"new notification");
    if (p == std::wstring::npos) return 0;      // "no new notifications" etc.
    size_t e = p;
    while (e > 0 && s[e - 1] == L' ') e--;
    size_t b = e;
    while (b > 0 && iswdigit(s[b - 1])) b--;
    if (b == e) return 0;
    return _wtoi(s.substr(b, e - b).c_str());
}

static void ReleaseBell() {
    if (g_bell) {
        g_bell->Release();
        g_bell = nullptr;
    }
    g_bellTray = nullptr;
}

static IUIAutomationCondition* MakeCondition(IUIAutomation* uia, PROPERTYID prop,
                                             PCWSTR text, bool substring) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(text);
    IUIAutomationCondition* cond = nullptr;
    HRESULT hr = substring
        // 1 = IgnoreCase, 2 = MatchSubstring (numeric: older headers lack the names)
        ? uia->CreatePropertyConditionEx(prop, v, (PropertyConditionFlags)(1 | 2), &cond)
        : uia->CreatePropertyCondition(prop, v, &cond);
    VariantClear(&v);
    return SUCCEEDED(hr) ? cond : nullptr;
}

// Of all elements matching cond, returns the rightmost visible one.
static IUIAutomationElement* PickRightmost(IUIAutomationElement* root,
                                           IUIAutomationCondition* cond) {
    IUIAutomationElement* best = nullptr;
    LONG bestRight = LONG_MIN;
    IUIAutomationElementArray* arr = nullptr;
    if (SUCCEEDED(root->FindAll(TreeScope_Descendants, cond, &arr)) && arr) {
        int n = 0;
        arr->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* el = nullptr;
            if (FAILED(arr->GetElement(i, &el)) || !el) continue;
            RECT r{};
            if (SUCCEEDED(el->get_CurrentBoundingRectangle(&r)) &&
                r.right > r.left && r.bottom > r.top && r.right > bestRight) {
                if (best) best->Release();
                best = el;
                bestRight = r.right;
            } else {
                el->Release();
            }
        }
        arr->Release();
    }
    return best;
}

static IUIAutomationElement* FindBell(IUIAutomation* uia, HWND tray) {
    IUIAutomationElement* root = nullptr;
    if (FAILED(uia->ElementFromHandle(tray, &root)) || !root) return nullptr;

    IUIAutomationElement* best = nullptr;
    auto tryFind = [&](PROPERTYID prop, PCWSTR text, bool substring) {
        if (best) return;
        IUIAutomationCondition* cond = MakeCondition(uia, prop, text, substring);
        if (!cond) return;
        best = PickRightmost(root, cond);
        cond->Release();
    };

    tryFind(UIA_ClassNamePropertyId, L"SystemTray.OmniButtonRight", false);
    tryFind(UIA_NamePropertyId, L"Notification", true);   // fallback
    root->Release();

    if (best) {
        BSTR name = nullptr, id = nullptr, cls = nullptr;
        best->get_CurrentName(&name);
        best->get_CurrentAutomationId(&id);
        best->get_CurrentClassName(&cls);
        Wh_Log(L"Bell found: name='%s' automationId='%s' class='%s'",
               name ? name : L"", id ? id : L"", cls ? cls : L"");
        SysFreeString(name);
        SysFreeString(id);
        SysFreeString(cls);
    }
    return best;
}

// Reads state from the cached element. Fails if the element has gone away.
static bool ReadBell(IUIAutomationElement* el, RECT* rc, int* count) {
    BSTR name = nullptr;
    if (FAILED(el->get_CurrentName(&name))) return false;
    HRESULT hr = el->get_CurrentBoundingRectangle(rc);

    static std::wstring lastName;
    if (name && lastName != name) {
        lastName = name;
        Wh_Log(L"Bell name: %s", name);
    }
    *count = name ? ParseCount(name) : 0;
    SysFreeString(name);

    return SUCCEEDED(hr) && rc->right > rc->left && rc->bottom > rc->top;
}

static void RenderOverlay(HWND hwnd, BYTE alpha) {
    int size = g_curSize;
    if (size <= 0) return;
    if (!g_memDC) g_memDC = CreateCompatibleDC(nullptr);

    if (!g_dib || size != g_dibSize || g_count != g_dibCount ||
        g.color != g_dibColor || g.showCount != g_dibShowCount ||
        g.square != g_dibSquare) {
        if (g_dib) {
            SelectObject(g_memDC, g_oldBmp);
            DeleteObject(g_dib);
            g_dib = nullptr;
        }
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = size;
        bi.bmiHeader.biHeight = -size;          // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        DWORD* bits = nullptr;
        g_dib = CreateDIBSection(g_memDC, &bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
        if (!g_dib || !bits) return;
        HGDIOBJ prev = SelectObject(g_memDC, g_dib);
        if (!g_oldBmp) g_oldBmp = prev;

        // Filled shape, premultiplied alpha: anti-aliased circle or solid square
        double r = size / 2.0;
        int cr = GetRValue(g.color), cg = GetGValue(g.color), cb = GetBValue(g.color);
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                double cov = 1.0;
                if (!g.square) {
                    double dx = x + 0.5 - r, dy = y + 0.5 - r;
                    cov = r - sqrt(dx * dx + dy * dy);
                    if (cov < 0) cov = 0;
                    if (cov > 1) cov = 1;
                }
                int a = (int)(cov * 255 + 0.5);
                bits[y * size + x] = (a << 24) | ((cr * a / 255) << 16) |
                                     ((cg * a / 255) << 8) | (cb * a / 255);
            }
        }

        if (g.showCount && g_count > 0) {
            wchar_t txt[16];
            if (g_count > 99) wcscpy(txt, L"99+");
            else wsprintfW(txt, L"%d", g_count);
            HFONT f = CreateFontW(-(size * 55 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0,
                                  DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0,
                                  L"Segoe UI");
            HGDIOBJ oldF = SelectObject(g_memDC, f);
            SetBkMode(g_memDC, TRANSPARENT);
            SetTextColor(g_memDC, RGB(255, 255, 255));
            RECT tr = {0, 0, size, size};
            DrawTextW(g_memDC, txt, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(g_memDC, oldF);
            DeleteObject(f);
        }
        GdiFlush();

        // GDI text may leave the alpha byte of glyph pixels at 0, which would
        // blend them with whatever is behind the overlay. Pixels that GDI
        // touched have alpha 0 but non-zero colour, so make them opaque.
        if (g.showCount && g_count > 0) {
            for (int i = 0; i < size * size; i++) {
                if ((bits[i] >> 24) == 0 && (bits[i] & 0x00FFFFFF)) {
                    bits[i] |= 0xFF000000;
                }
            }
        }

        g_dibSize = size;
        g_dibCount = g_count;
        g_dibColor = g.color;
        g_dibShowCount = g.showCount;
        g_dibSquare = g.square;
    }

    SIZE sz = {size, size};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, nullptr, nullptr, &sz, g_memDC, &src, 0, &bf, ULW_ALPHA);
}

static void HideOverlay(HWND hwnd) {
    if (g_pulseRunning) {
        KillTimer(hwnd, 1);
        g_pulseRunning = false;
    }
    ShowWindow(hwnd, SW_HIDE);
}

static void Reposition(HWND hwnd, const RECT& rc, int count) {
    bool hide = count <= 0;
    QUERY_USER_NOTIFICATION_STATE qs;
    if (SUCCEEDED(SHQueryUserNotificationState(&qs)) &&
        (qs == QUNS_BUSY || qs == QUNS_RUNNING_D3D_FULL_SCREEN ||
         qs == QUNS_PRESENTATION_MODE)) {
        hide = true;   // don't draw over fullscreen apps
    }
    if (hide) {
        HideOverlay(hwnd);
        return;
    }

    int h = rc.bottom - rc.top;
    int size = h * g.sizePercent / 100;
    UINT dpi = g_bellTray ? GetDpiForWindow(g_bellTray) : 96;
    if (!dpi) dpi = 96;
    int offX = MulDiv(g.offsetX, dpi, 96);
    int offY = MulDiv(g.offsetY, dpi, 96);
    int x = rc.right - size + offX;
    int y = rc.top + (h - size) / 2 + offY;

    g_curSize = size;
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, size, size,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    RenderOverlay(hwnd, g_alpha);

    if (!g_pulseRunning) {
        SetTimer(hwnd, 1, 33, nullptr);     // pulse animation, only while visible
        g_pulseRunning = true;
    }
}

static void Poll(HWND hwnd, IUIAutomation* uia) {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray || !IsWindowVisible(tray)) {
        ReleaseBell();
        HideOverlay(hwnd);
        return;
    }
    if (g_bell && tray != g_bellTray) ReleaseBell();   // taskbar was recreated

    RECT rc{};
    int count = 0;
    bool ok = g_bell && ReadBell(g_bell, &rc, &count);

    if (!ok) {
        ReleaseBell();
        DWORD now = GetTickCount();
        // Full-tree search only when needed, and at most every 3 seconds.
        if (g_lastFindTick == 0 || now - g_lastFindTick >= 3000) {
            g_lastFindTick = now;
            g_bell = FindBell(uia, tray);
            g_bellTray = tray;
            ok = g_bell && ReadBell(g_bell, &rc, &count);
        }
    }

    if (ok) {
        g_count = count;
        Reposition(hwnd, rc, count);
    } else {
        HideOverlay(hwnd);
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static IUIAutomation* uia = nullptr;
    switch (msg) {
    case WM_CREATE:
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         IID_IUIAutomation, (void**)&uia);
        SetTimer(hwnd, 2, 700, nullptr);   // poll the cached bell element
        return 0;

    case WM_APP:                            // settings changed
        LoadSettings();
        ReleaseBell();
        g_lastFindTick = 0;
        if (uia) Poll(hwnd, uia);
        return 0;

    case WM_TIMER:
        if (wp == 1) {
            double t = (GetTickCount() % g.pulseMs) / (double)g.pulseMs;
            double k = 0.5 + 0.5 * sin(t * 6.2831853);
            g_alpha = (BYTE)(110 + 145 * k);
            RenderOverlay(hwnd, g_alpha);
        } else if (wp == 2 && uia) {
            Poll(hwnd, uia);
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;
    case WM_NCHITTEST:
        return HTTRANSPARENT;          // clicks fall through to the real bell
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        g_pulseRunning = false;
        ReleaseBell();
        if (uia) { uia->Release(); uia = nullptr; }
        if (g_memDC) {
            if (g_oldBmp) SelectObject(g_memDC, g_oldBmp);
            if (g_dib) DeleteObject(g_dib);
            DeleteDC(g_memDC);
            g_memDC = nullptr; g_dib = nullptr; g_oldBmp = nullptr;
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI OverlayThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WhBellHighlighter";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        SetEvent(g_readyEvent);
        CoUninitialize();
        return 0;
    }

    g_wnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr,
        wc.hInstance, nullptr);

    SetEvent(g_readyEvent);     // g_wnd is final from here on (window or null)

    if (g_wnd) {
        MSG m;
        while (GetMessageW(&m, nullptr, 0, 0) > 0) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }

    g_wnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = g_readyEvent
                   ? CreateThread(nullptr, 0, OverlayThread, nullptr, 0, nullptr)
                   : nullptr;
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    // Wait until the overlay thread has finished creating its window, so a
    // change made during startup isn't dropped. Safe: the overlay thread's
    // startup doesn't depend on this thread.
    if (g_readyEvent) WaitForSingleObject(g_readyEvent, INFINITE);
    if (g_wnd) PostMessageW(g_wnd, WM_APP, 0, 0);
}

void WhTool_ModUninit() {
    if (g_thread) {
        WaitForSingleObject(g_readyEvent, INFINITE);   // g_wnd is final now
        if (g_wnd) PostMessageW(g_wnd, WM_CLOSE, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
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
