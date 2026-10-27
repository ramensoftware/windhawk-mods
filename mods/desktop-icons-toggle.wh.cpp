// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Toggle desktop icons with a tray button or Ctrl+Alt+H
// @version         1.0.0
// @author          merpzzz
// @github          https://github.com/merpzzz
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icons Toggle

Left-click the tray icon or press **Ctrl+Alt+H** to hide desktop icons. Repeat
to show them again. Running app windows and files are not changed or deleted.
Right-click the tray icon for explicit Show and Hide commands.

If Ctrl+Alt+H is taken, the mod tries Ctrl+Alt+F12; the menu shows the actual
shortcut. If neither is available, use the tray icon. Do not run the standalone
Desktop Icons Toggle helper at the same time.

The mod runs locally inside Explorer. No telemetry, network access, keyboard
monitoring, downloads, or elevated helper is used. It restores the visibility
it found at startup when unloaded if its last change is still in effect.
Explorer's own desktop context menu > View > Show desktop icons remains usable.

Initial target: Windows 11 x64. Uses Explorer's Show desktop icons command,
which Microsoft could change in a future Windows release. Shell replacements
are not supported.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <cwchar>
#include <atomic>

namespace {
constexpr wchar_t kClass[] = L"Merpzzz.DesktopIconsToggle.Tray";
constexpr UINT kTray = WM_APP + 1;
constexpr UINT_PTR kRefresh = 1;
HANDLE stopEvent = nullptr;
HANDLE worker = nullptr;
HANDLE instanceMutex = nullptr;
std::atomic<HWND> workerWindow{nullptr};
HINSTANCE module = nullptr;
UINT taskbarCreated = 0;
NOTIFYICONDATAW tray{};
int initialVisible = -1, lastApplied = -1;
const wchar_t* shortcut = L"Tray button only";
HICON shownIcon = nullptr, hiddenIcon = nullptr;

HWND FindView() {
    HWND view = FindWindowExW(FindWindowW(L"Progman", nullptr), nullptr,
                              L"SHELLDLL_DefView", nullptr);
    if (!view) {
        EnumWindows([](HWND h, LPARAM p) -> BOOL {
            auto result = reinterpret_cast<HWND*>(p);
            *result = FindWindowExW(h, nullptr, L"SHELLDLL_DefView", nullptr);
            return !*result;
        }, reinterpret_cast<LPARAM>(&view));
    }
    return view;
}

int Visibility() {
    HWND view = FindView();
    HWND icons = view ? FindWindowExW(view, nullptr, L"SysListView32", nullptr) : nullptr;
    return icons ? (IsWindowVisible(icons) ? 1 : 0) : -1;
}

bool SetVisible(bool visible) {
    const int before = Visibility();
    if (before < 0) return false;
    if (before == static_cast<int>(visible)) return true;
    DWORD_PTR result = 0;
    HWND view = FindView();
    if (!view || !SendMessageTimeoutW(view, WM_COMMAND, 0x7402, 0,
                                      SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result))
        return false;
    lastApplied = visible ? 1 : 0;
    return true;
}

HICON MakeIcon(bool hidden) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 32;
    bi.bmiHeader.biHeight = -32;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    void* pixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bitmap) return nullptr;
    auto data = static_cast<DWORD*>(pixels);
    ZeroMemory(data, 32 * 32 * sizeof(DWORD));
    for (int y = 0; y < 32; ++y) for (int x = 0; x < 32; ++x) {
        bool square = ((x >= 5 && x < 14) || (x >= 18 && x < 27)) &&
                      ((y >= 5 && y < 14) || (y >= 18 && y < 27));
        if (square) data[y * 32 + x] = hidden ? 0xFF6E7D8C : 0xFF5ACDF5;
        if (hidden && x >= 4 && x < 28 && y >= 4 && y < 28 && abs(x + y - 31) <= 1)
            data[y * 32 + x] = 0xFFFFCD55;
    }
    HBITMAP mask = CreateBitmap(32, 32, 1, 1, nullptr);
    ICONINFO info{TRUE, 0, 0, mask, bitmap};
    HICON icon = mask ? CreateIconIndirect(&info) : nullptr;
    if (mask) DeleteObject(mask);
    DeleteObject(bitmap);
    return icon;
}

void Refresh(bool add = false) {
    int visible = Visibility();
    if (initialVisible < 0 && visible >= 0) initialVisible = visible;
    tray.hIcon = visible == 0 ? hiddenIcon : shownIcon;
    std::swprintf(tray.szTip, ARRAYSIZE(tray.szTip), L"Desktop icons: %ls | %ls",
               visible < 0 ? L"waiting" : visible ? L"shown" : L"hidden", shortcut);
    Shell_NotifyIconW(add ? NIM_ADD : NIM_MODIFY, &tray);
}

LRESULT CALLBACK WindowProc(HWND h, UINT message, WPARAM w, LPARAM l) {
    if (message == taskbarCreated && taskbarCreated) { Refresh(true); return 0; }
    if (message == WM_TIMER && w == kRefresh) { Refresh(); return 0; }
    if (message == WM_HOTKEY && w == 1) {
        int current = Visibility();
        if (current >= 0) SetVisible(!current);
        Refresh(); return 0;
    }
    if (message == kTray) {
        if (l == WM_LBUTTONUP) {
            int current = Visibility(); if (current >= 0) SetVisible(!current);
            Refresh();
        } else if (l == WM_RBUTTONUP || l == WM_CONTEXTMENU) {
            HMENU menu = CreatePopupMenu();
            if (!menu) return 0;
            int current = Visibility();
            AppendMenuW(menu, MF_STRING | (current < 0 ? MF_GRAYED : 0), 1, L"Show desktop icons");
            AppendMenuW(menu, MF_STRING | (current < 0 ? MF_GRAYED : 0), 2, L"Hide desktop icons");
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(menu, MF_STRING | MF_GRAYED, 3, shortcut);
            POINT p{}; GetCursorPos(&p); SetForegroundWindow(h);
            UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                          p.x, p.y, 0, h, nullptr);
            DestroyMenu(menu); PostMessageW(h, WM_NULL, 0, 0);
            if (command == 1 || command == 2) SetVisible(command == 1);
            Refresh();
        }
        return 0;
    }
    return DefWindowProcW(h, message, w, l);
}

DWORD WINAPI Run(void*) {
    initialVisible = Visibility(); lastApplied = -1;
    WNDCLASSW wc{}; wc.hInstance = module; wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = kClass;
    if (!RegisterClassW(&wc)) return 1;
    // A hidden top-level window receives TaskbarCreated after Explorer reinitializes.
    HWND h = CreateWindowExW(WS_EX_TOOLWINDOW, kClass, L"Desktop Icons Toggle", 0,
                             0, 0, 0, 0, nullptr, nullptr, module, nullptr);
    if (!h) { UnregisterClassW(kClass, module); return 2; }
    workerWindow = h;
    shownIcon = MakeIcon(false); hiddenIcon = MakeIcon(true);
    if (!shownIcon || !hiddenIcon) {
        if (shownIcon) DestroyIcon(shownIcon);
        if (hiddenIcon) DestroyIcon(hiddenIcon);
        workerWindow = nullptr; DestroyWindow(h); UnregisterClassW(kClass, module); return 3;
    }
    shortcut = L"Tray button only";
    if (RegisterHotKey(h, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'H')) shortcut = L"Ctrl+Alt+H";
    else if (RegisterHotKey(h, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F12)) shortcut = L"Ctrl+Alt+F12";
    tray = {}; tray.cbSize = sizeof(tray); tray.hWnd = h; tray.uID = 1;
    tray.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; tray.uCallbackMessage = kTray;
    Refresh(true); SetTimer(h, kRefresh, 1000, nullptr);
    bool done = false;
    while (!done) {
        DWORD wait = MsgWaitForMultipleObjects(1, &stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (wait != WAIT_OBJECT_0 + 1) break;
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { done = true; break; }
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
    }
    KillTimer(h, kRefresh); UnregisterHotKey(h, 1); Shell_NotifyIconW(NIM_DELETE, &tray);
    if (initialVisible >= 0 && lastApplied >= 0 && Visibility() == lastApplied)
        SetVisible(initialVisible != 0);
    workerWindow = nullptr; DestroyWindow(h); DestroyIcon(shownIcon); DestroyIcon(hiddenIcon);
    UnregisterClassW(kClass, module); return 0;
}
}

BOOL Wh_ModInit() {
    // Only the desktop Explorer instance, not separate folder processes.
    HWND shell = GetShellWindow(); DWORD pid = 0;
    if (shell && (!GetWindowThreadProcessId(shell, &pid) || pid != GetCurrentProcessId())) return FALSE;
    instanceMutex = CreateMutexW(nullptr, FALSE, L"Local\\merpzzz.DesktopIconsToggle.Mod");
    if (!instanceMutex) return FALSE;
    if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(instanceMutex); instanceMutex = nullptr; return FALSE; }
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&Wh_ModInit), &module);
    taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) { CloseHandle(instanceMutex); instanceMutex = nullptr; return FALSE; }
    worker = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
    if (!worker) { CloseHandle(stopEvent); stopEvent = nullptr; CloseHandle(instanceMutex); instanceMutex = nullptr; return FALSE; }
    return TRUE;
}

void Wh_ModUninit() {
    if (HWND h = workerWindow.load()) PostMessageW(h, WM_CANCELMODE, 0, 0);
    if (stopEvent) SetEvent(stopEvent);
    if (worker) { WaitForSingleObject(worker, INFINITE); CloseHandle(worker); worker = nullptr; }
    if (stopEvent) { CloseHandle(stopEvent); stopEvent = nullptr; }
    if (instanceMutex) { CloseHandle(instanceMutex); instanceMutex = nullptr; }
}
