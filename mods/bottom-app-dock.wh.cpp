// ==WindhawkMod==
// @id              bottom-app-dock
// @name            Bottom App Dock
// @description     Dock premium inferior, multi-monitor, com apps fixados, auto-hide e integração visual ao Windows
// @version         0.9
// @author          Keygreen3D
// @github          https://github.com/keygreen3d
// @include         explorer.exe
// @compilerOptions -lshell32 -ldwmapi -lgdi32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bottom App Dock v0.9 Premium

Mantém toda a lógica estável da v0.8.2 e adiciona acabamento visual.

## Visual
- System Backdrop do Windows 11.
- Tema claro/escuro automático.
- Cor de destaque do Windows.
- Cantos arredondados pelo DWM.
- Borda nativa removida.
- Hover premium.
- Ícone aumenta sem deslocar os vizinhos.
- Cápsula discreta no hover.
- Indicadores arredondados.
- Visual compacto e flutuante.

## Comportamento
- Sem janela maximizada: dock permanece visível.
- Janela maximizada: auto-hide.
- Mouse na borda inferior: revela a dock.
- Multi-monitor.
- Ordem dos apps estável.
- Apps fixados persistentes.

## Mouse
Clique esquerdo:
- fechado -> abre;
- minimizado -> restaura;
- segundo plano -> traz para frente;
- ativo -> minimiza.

Clique do meio:
- nova instância.

Botão direito:
- abrir nova janela;
- fixar/desafixar;
- escolher janela;
- restaurar/minimizar;
- fechar.

O menu abre acima da dock e fecha ao clicar fora.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iconSize: 32
  $name: Tamanho dos ícones

- dockHeight: 52
  $name: Altura da dock

- iconSpacing: 10
  $name: Espaçamento dos ícones

- sidePadding: 14
  $name: Espaço lateral

- bottomMargin: 10
  $name: Margem inferior

- opacity: 225
  $name: Opacidade
  $description: Valor entre 40 e 255

- activationZone: 3
  $name: Zona de ativação
  $description: Pixels da borda inferior que revelam a dock

- hideDelay: 650
  $name: Tempo para esconder
  $description: Tempo em milissegundos

- hoverScale: 118
  $name: Ampliação no hover
  $description: Percentual do tamanho do ícone apontado

- premiumBackdrop: true
  $name: Efeito premium
  $description: Ativa o material de fundo do Windows 11

- hoverHighlight: true
  $name: Fundo no hover
  $description: Mostra uma cápsula discreta atrás do ícone apontado
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <dwmapi.h>

#include <vector>
#include <string>
#include <algorithm>


// ============================================================
// CONSTANTES DWM
// Mantidas aqui para compilar mesmo se o SDK usado pelo
// Windhawk não expuser os nomes mais novos.
// ============================================================

constexpr DWORD DWM_ATTR_DARK_MODE = 20;
constexpr DWORD DWM_ATTR_CORNER_PREFERENCE = 33;
constexpr DWORD DWM_ATTR_BORDER_COLOR = 34;
constexpr DWORD DWM_ATTR_SYSTEM_BACKDROP = 38;

constexpr int DWM_CORNER_ROUND = 2;

// DWM_SYSTEMBACKDROP_TYPE:
// 0 Auto
// 1 None
// 2 Mica
// 3 Transient/Acrylic-like
// 4 Tabbed
constexpr int DWM_BACKDROP_TRANSIENT = 3;

constexpr COLORREF DWM_COLOR_NONE_VALUE =
    static_cast<COLORREF>(0xFFFFFFFE);


// ============================================================
// ESTRUTURAS
// ============================================================

struct Settings {
    int iconSize = 32;
    int dockHeight = 52;
    int iconSpacing = 10;
    int sidePadding = 14;
    int bottomMargin = 10;
    int opacity = 225;
    int activationZone = 3;
    int hideDelay = 650;

    int hoverScale = 118;

    bool premiumBackdrop = true;
    bool hoverHighlight = true;
};

struct WindowInfo {
    HWND hwnd = nullptr;
    std::wstring title;
};

struct AppGroup {
    std::wstring path;
    std::wstring title;

    std::vector<WindowInfo> windows;

    HICON icon = nullptr;
    RECT rect = {};

    bool pinned = false;
    bool running = false;

    size_t cycleIndex = 0;
};

struct EnumeratedWindow {
    std::wstring path;
    std::wstring title;

    HWND hwnd = nullptr;
    HICON icon = nullptr;
};


// ============================================================
// GLOBAIS
// ============================================================

static Settings g_settings;

static HWND g_dockWindow = nullptr;
static HANDLE g_thread = nullptr;

static std::vector<AppGroup> g_apps;
static std::vector<std::wstring> g_pinnedPaths;
static std::vector<std::wstring> g_dynamicOrder;
static std::vector<EnumeratedWindow> g_enumWindows;

static HMONITOR g_currentMonitor = nullptr;

static bool g_visible = false;
static bool g_lightTheme = false;
static bool g_contextMenuOpen = false;

static bool g_trackingMouse = false;
static int g_hoveredIndex = -1;

static ULONGLONG g_outsideSince = 0;
static ULONGLONG g_lastRefresh = 0;
static ULONGLONG g_keepVisibleUntil = 0;

static COLORREF g_accentColor =
    RGB(0, 120, 215);

constexpr UINT_PTR MAIN_TIMER = 1;

constexpr UINT WM_DOCK_SETTINGS_CHANGED =
    WM_APP + 1;

static const wchar_t* REG_PATH =
    L"Software\\Windhawk\\BottomAppDock";


// ============================================================
// HELPERS
// ============================================================

static bool PathEquals(
    const std::wstring& a,
    const std::wstring& b) {

    return _wcsicmp(
               a.c_str(),
               b.c_str()) == 0;
}


static bool ContainsPath(
    const std::vector<std::wstring>& list,
    const std::wstring& path) {

    for (const auto& item : list) {

        if (PathEquals(item, path))
            return true;
    }

    return false;
}


static std::wstring GetFileName(
    const std::wstring& path) {

    size_t pos =
        path.find_last_of(L"\\/");

    if (pos == std::wstring::npos)
        return path;

    return path.substr(pos + 1);
}


// ============================================================
// SETTINGS
// ============================================================

static void LoadSettings() {

    g_settings.iconSize =
        Wh_GetIntSetting(L"iconSize");

    g_settings.dockHeight =
        Wh_GetIntSetting(L"dockHeight");

    g_settings.iconSpacing =
        Wh_GetIntSetting(L"iconSpacing");

    g_settings.sidePadding =
        Wh_GetIntSetting(L"sidePadding");

    g_settings.bottomMargin =
        Wh_GetIntSetting(L"bottomMargin");

    g_settings.opacity =
        Wh_GetIntSetting(L"opacity");

    g_settings.activationZone =
        Wh_GetIntSetting(L"activationZone");

    g_settings.hideDelay =
        Wh_GetIntSetting(L"hideDelay");

    g_settings.hoverScale =
        Wh_GetIntSetting(L"hoverScale");

    g_settings.premiumBackdrop =
        Wh_GetIntSetting(L"premiumBackdrop") != 0;

    g_settings.hoverHighlight =
        Wh_GetIntSetting(L"hoverHighlight") != 0;


    if (g_settings.iconSize < 20)
        g_settings.iconSize = 20;

    if (g_settings.iconSize > 72)
        g_settings.iconSize = 72;

    if (g_settings.dockHeight <
        g_settings.iconSize + 14) {

        g_settings.dockHeight =
            g_settings.iconSize + 14;
    }

    if (g_settings.iconSpacing < 0)
        g_settings.iconSpacing = 0;

    if (g_settings.sidePadding < 0)
        g_settings.sidePadding = 0;

    if (g_settings.opacity < 40)
        g_settings.opacity = 40;

    if (g_settings.opacity > 255)
        g_settings.opacity = 255;

    if (g_settings.activationZone < 1)
        g_settings.activationZone = 1;

    if (g_settings.activationZone > 30)
        g_settings.activationZone = 30;

    if (g_settings.hideDelay < 100)
        g_settings.hideDelay = 100;

    if (g_settings.hoverScale < 100)
        g_settings.hoverScale = 100;

    if (g_settings.hoverScale > 145)
        g_settings.hoverScale = 145;
}


// ============================================================
// TEMA DO WINDOWS
// ============================================================

static void LoadWindowsTheme() {

    DWORD light = 0;
    DWORD size = sizeof(light);

    HKEY key = nullptr;

    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0,
            KEY_READ,
            &key) == ERROR_SUCCESS) {

        RegQueryValueExW(
            key,
            L"AppsUseLightTheme",
            nullptr,
            nullptr,
            reinterpret_cast<BYTE*>(&light),
            &size);

        RegCloseKey(key);
    }

    g_lightTheme =
        light != 0;


    DWORD argb = 0;
    BOOL opaque = FALSE;

    if (SUCCEEDED(
            DwmGetColorizationColor(
                &argb,
                &opaque))) {

        BYTE r =
            (argb >> 16) & 0xFF;

        BYTE g =
            (argb >> 8) & 0xFF;

        BYTE b =
            argb & 0xFF;

        g_accentColor =
            RGB(r, g, b);
    }
}


// ============================================================
// DWM PREMIUM
// ============================================================

static void ApplyPremiumDwm(
    HWND hwnd) {

    if (!hwnd)
        return;


    BOOL dark =
        g_lightTheme
            ? FALSE
            : TRUE;

    DwmSetWindowAttribute(
        hwnd,
        static_cast<DWMWINDOWATTRIBUTE>(
            DWM_ATTR_DARK_MODE),
        &dark,
        sizeof(dark));


    int corner =
        DWM_CORNER_ROUND;

    DwmSetWindowAttribute(
        hwnd,
        static_cast<DWMWINDOWATTRIBUTE>(
            DWM_ATTR_CORNER_PREFERENCE),
        &corner,
        sizeof(corner));


    COLORREF borderColor =
        DWM_COLOR_NONE_VALUE;

    DwmSetWindowAttribute(
        hwnd,
        static_cast<DWMWINDOWATTRIBUTE>(
            DWM_ATTR_BORDER_COLOR),
        &borderColor,
        sizeof(borderColor));


    int backdrop =
        g_settings.premiumBackdrop
            ? DWM_BACKDROP_TRANSIENT
            : 1;

    DwmSetWindowAttribute(
        hwnd,
        static_cast<DWMWINDOWATTRIBUTE>(
            DWM_ATTR_SYSTEM_BACKDROP),
        &backdrop,
        sizeof(backdrop));
}


// ============================================================
// APPS FIXADOS
// ============================================================

static bool IsPinned(
    const std::wstring& path) {

    return ContainsPath(
        g_pinnedPaths,
        path);
}


static void LoadPinnedApps() {

    g_pinnedPaths.clear();

    HKEY key = nullptr;

    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            REG_PATH,
            0,
            KEY_READ,
            &key) != ERROR_SUCCESS) {

        return;
    }

    DWORD type = 0;
    DWORD size = 0;

    if (RegQueryValueExW(
            key,
            L"Pinned",
            nullptr,
            &type,
            nullptr,
            &size) != ERROR_SUCCESS ||
        type != REG_MULTI_SZ ||
        size == 0) {

        RegCloseKey(key);
        return;
    }


    std::vector<wchar_t> buffer(
        size / sizeof(wchar_t) + 2,
        L'\0');


    if (RegQueryValueExW(
            key,
            L"Pinned",
            nullptr,
            nullptr,
            reinterpret_cast<BYTE*>(
                buffer.data()),
            &size) == ERROR_SUCCESS) {


        const wchar_t* ptr =
            buffer.data();


        while (*ptr) {

            g_pinnedPaths.emplace_back(
                ptr);

            ptr +=
                wcslen(ptr) + 1;
        }
    }


    RegCloseKey(key);
}


static void SavePinnedApps() {

    HKEY key = nullptr;


    if (RegCreateKeyExW(
            HKEY_CURRENT_USER,
            REG_PATH,
            0,
            nullptr,
            0,
            KEY_WRITE,
            nullptr,
            &key,
            nullptr) != ERROR_SUCCESS) {

        return;
    }


    std::vector<wchar_t> buffer;


    for (const auto& path :
         g_pinnedPaths) {

        buffer.insert(
            buffer.end(),
            path.begin(),
            path.end());

        buffer.push_back(L'\0');
    }


    buffer.push_back(L'\0');


    if (g_pinnedPaths.empty())
        buffer.push_back(L'\0');


    RegSetValueExW(
        key,
        L"Pinned",
        0,
        REG_MULTI_SZ,
        reinterpret_cast<const BYTE*>(
            buffer.data()),
        static_cast<DWORD>(
            buffer.size() *
            sizeof(wchar_t)));


    RegCloseKey(key);
}


static void TogglePinned(
    const std::wstring& path) {

    for (auto it =
             g_pinnedPaths.begin();
         it != g_pinnedPaths.end();
         ++it) {

        if (PathEquals(
                *it,
                path)) {

            g_pinnedPaths.erase(it);

            SavePinnedApps();

            return;
        }
    }


    g_pinnedPaths.push_back(path);


    g_dynamicOrder.erase(
        std::remove_if(
            g_dynamicOrder.begin(),
            g_dynamicOrder.end(),

            [&](const std::wstring& item) {

                return PathEquals(
                    item,
                    path);
            }),

        g_dynamicOrder.end());


    SavePinnedApps();
}


// ============================================================
// PROCESSOS / ÍCONES
// ============================================================

static std::wstring GetProcessPath(
    DWORD pid) {

    HANDLE process =
        OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            pid);


    if (!process)
        return L"";


    wchar_t buffer[4096] = {};

    DWORD size =
        ARRAYSIZE(buffer);


    std::wstring result;


    if (QueryFullProcessImageNameW(
            process,
            0,
            buffer,
            &size)) {

        result.assign(
            buffer,
            size);
    }


    CloseHandle(process);

    return result;
}


static HICON GetIconFromPath(
    const std::wstring& path) {

    SHFILEINFOW sfi = {};


    if (SHGetFileInfoW(
            path.c_str(),
            FILE_ATTRIBUTE_NORMAL,
            &sfi,
            sizeof(sfi),
            SHGFI_ICON |
            SHGFI_LARGEICON)) {

        return sfi.hIcon;
    }


    HICON fallback =
        LoadIconW(
            nullptr,
            IDI_APPLICATION);


    return fallback
        ? CopyIcon(fallback)
        : nullptr;
}


static HICON GetWindowIcon(
    HWND hwnd,
    const std::wstring& path) {

    HICON icon =
        reinterpret_cast<HICON>(
            SendMessageW(
                hwnd,
                WM_GETICON,
                ICON_BIG,
                0));


    if (!icon) {

        icon =
            reinterpret_cast<HICON>(
                SendMessageW(
                    hwnd,
                    WM_GETICON,
                    ICON_SMALL2,
                    0));
    }


    if (!icon) {

        icon =
            reinterpret_cast<HICON>(
                GetClassLongPtrW(
                    hwnd,
                    GCLP_HICON));
    }


    if (icon)
        return CopyIcon(icon);


    return GetIconFromPath(path);
}


// ============================================================
// FILTRO DE JANELAS
// ============================================================

static bool IsWindowEligible(
    HWND hwnd) {

    if (!IsWindowVisible(hwnd))
        return false;


    if (hwnd == g_dockWindow)
        return false;


    if (hwnd == GetShellWindow())
        return false;


    LONG_PTR exStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE);


    if (exStyle &
        WS_EX_TOOLWINDOW)
        return false;


    wchar_t cls[256] = {};


    GetClassNameW(
        hwnd,
        cls,
        ARRAYSIZE(cls));


    if (_wcsicmp(
            cls,
            L"Shell_TrayWnd") == 0 ||

        _wcsicmp(
            cls,
            L"Shell_SecondaryTrayWnd") == 0 ||

        _wcsicmp(
            cls,
            L"Progman") == 0 ||

        _wcsicmp(
            cls,
            L"WorkerW") == 0) {

        return false;
    }


    BOOL cloaked = FALSE;


    if (SUCCEEDED(
            DwmGetWindowAttribute(
                hwnd,
                DWMWA_CLOAKED,
                &cloaked,
                sizeof(cloaked))) &&
        cloaked) {

        return false;
    }


    if (GetWindowTextLengthW(hwnd) <= 0)
        return false;


    return true;
}


// ============================================================
// ENUMERAÇÃO
// ============================================================

static BOOL CALLBACK EnumWindowsProc(
    HWND hwnd,
    LPARAM) {

    if (!IsWindowEligible(hwnd))
        return TRUE;


    DWORD pid = 0;


    GetWindowThreadProcessId(
        hwnd,
        &pid);


    if (!pid)
        return TRUE;


    std::wstring path =
        GetProcessPath(pid);


    if (path.empty())
        return TRUE;


    wchar_t title[512] = {};


    GetWindowTextW(
        hwnd,
        title,
        ARRAYSIZE(title));


    EnumeratedWindow item;


    item.path = path;
    item.hwnd = hwnd;


    item.title =
        title[0]
            ? title
            : GetFileName(path);


    item.icon =
        GetWindowIcon(
            hwnd,
            path);


    g_enumWindows.push_back(
        std::move(item));


    return TRUE;
}


// ============================================================
// LIMPEZA
// ============================================================

static void ClearApps() {

    for (auto& app :
         g_apps) {

        if (app.icon)
            DestroyIcon(app.icon);
    }


    g_apps.clear();
}


static void ClearEnumeratedWindows() {

    for (auto& item :
         g_enumWindows) {

        if (item.icon)
            DestroyIcon(item.icon);
    }


    g_enumWindows.clear();
}


// ============================================================
// MONTA LISTA
// ============================================================

static void BuildDockApps() {

    ClearApps();
    ClearEnumeratedWindows();


    EnumWindows(
        EnumWindowsProc,
        0);


    std::vector<std::wstring>
        runningPaths;


    for (const auto& item :
         g_enumWindows) {

        if (!ContainsPath(
                runningPaths,
                item.path)) {

            runningPaths.push_back(
                item.path);
        }
    }


    g_dynamicOrder.erase(
        std::remove_if(
            g_dynamicOrder.begin(),
            g_dynamicOrder.end(),

            [&](const std::wstring& path) {

                return
                    !ContainsPath(
                        runningPaths,
                        path) ||
                    IsPinned(path);
            }),

        g_dynamicOrder.end());


    for (const auto& path :
         runningPaths) {

        if (IsPinned(path))
            continue;


        if (!ContainsPath(
                g_dynamicOrder,
                path)) {

            g_dynamicOrder.push_back(
                path);
        }
    }


    auto addApp =
        [&](const std::wstring& path,
            bool pinned) {


        AppGroup app;


        app.path = path;
        app.pinned = pinned;
        app.running = false;


        app.title =
            GetFileName(path);


        for (const auto& item :
             g_enumWindows) {

            if (!PathEquals(
                    item.path,
                    path)) {

                continue;
            }


            WindowInfo info;


            info.hwnd =
                item.hwnd;


            info.title =
                item.title;


            app.windows.push_back(
                std::move(info));


            app.running = true;


            if (!app.icon &&
                item.icon) {

                app.icon =
                    CopyIcon(item.icon);


                app.title =
                    item.title;
            }
        }


        if (!app.icon) {

            app.icon =
                GetIconFromPath(path);
        }


        g_apps.push_back(
            std::move(app));
    };


    for (const auto& path :
         g_pinnedPaths) {

        addApp(
            path,
            true);
    }


    for (const auto& path :
         g_dynamicOrder) {

        addApp(
            path,
            false);
    }


    ClearEnumeratedWindows();


    if (g_hoveredIndex >=
        static_cast<int>(
            g_apps.size())) {

        g_hoveredIndex = -1;
    }
}


// ============================================================
// POSIÇÃO
// ============================================================

static void PositionDock(
    HMONITOR monitor) {

    if (!g_dockWindow ||
        !monitor)
        return;


    MONITORINFO mi = {};

    mi.cbSize =
        sizeof(mi);


    if (!GetMonitorInfoW(
            monitor,
            &mi))
        return;


    int count =
        static_cast<int>(
            g_apps.size());


    int width =
        g_settings.sidePadding * 2;


    if (count > 0) {

        width +=
            count *
            g_settings.iconSize;

        width +=
            (count - 1) *
            g_settings.iconSpacing;

    } else {

        width = 56;
    }


    int screenWidth =
        mi.rcMonitor.right -
        mi.rcMonitor.left;


    int x =
        mi.rcMonitor.left +
        (screenWidth - width) /
            2;


    int y =
        mi.rcMonitor.bottom -
        g_settings.dockHeight -
        g_settings.bottomMargin;


    SetWindowPos(
        g_dockWindow,
        HWND_TOPMOST,
        x,
        y,
        width,
        g_settings.dockHeight,
        SWP_NOACTIVATE |
        SWP_NOOWNERZORDER);


    int left =
        g_settings.sidePadding;


    for (auto& app :
         g_apps) {

        int top =
            (g_settings.dockHeight -
             g_settings.iconSize) /
            2 - 2;


        app.rect = {

            left,
            top,

            left +
                g_settings.iconSize,

            top +
                g_settings.iconSize
        };


        left +=
            g_settings.iconSize +
            g_settings.iconSpacing;
    }
}


// ============================================================
// REFRESH
// ============================================================

static void RefreshDock() {

    BuildDockApps();


    if (!g_currentMonitor) {

        POINT cursor = {};


        GetCursorPos(
            &cursor);


        g_currentMonitor =
            MonitorFromPoint(
                cursor,
                MONITOR_DEFAULTTOPRIMARY);
    }


    PositionDock(
        g_currentMonitor);


    InvalidateRect(
        g_dockWindow,
        nullptr,
        TRUE);
}


// ============================================================
// MAXIMIZADA
// ============================================================

struct MaxWindowSearchData {
    HMONITOR monitor = nullptr;
    bool found = false;
};


static BOOL CALLBACK FindMaximizedWindowProc(
    HWND hwnd,
    LPARAM lParam) {

    auto* data =
        reinterpret_cast<
            MaxWindowSearchData*>(
                lParam);


    if (!IsWindowEligible(hwnd))
        return TRUE;


    if (IsIconic(hwnd))
        return TRUE;


    HMONITOR windowMonitor =
        MonitorFromWindow(
            hwnd,
            MONITOR_DEFAULTTONULL);


    if (windowMonitor !=
        data->monitor)
        return TRUE;


    WINDOWPLACEMENT placement = {};


    placement.length =
        sizeof(placement);


    if (!GetWindowPlacement(
            hwnd,
            &placement)) {

        return TRUE;
    }


    if (placement.showCmd ==
        SW_SHOWMAXIMIZED) {

        data->found = true;

        return FALSE;
    }


    return TRUE;
}


static bool HasMaximizedWindowOnMonitor(
    HMONITOR monitor) {

    if (!monitor)
        return false;


    MaxWindowSearchData data;


    data.monitor =
        monitor;


    EnumWindows(
        FindMaximizedWindowProc,
        reinterpret_cast<LPARAM>(
            &data));


    return data.found;
}


// ============================================================
// ATIVAÇÃO
// ============================================================

typedef VOID (WINAPI*
    SwitchToThisWindow_t)(
        HWND,
        BOOL);


static void RestoreWindowPreservingState(
    HWND hwnd) {

    if (!hwnd ||
        !IsWindow(hwnd))
        return;


    WINDOWPLACEMENT placement = {};


    placement.length =
        sizeof(placement);


    if (!GetWindowPlacement(
            hwnd,
            &placement)) {

        ShowWindowAsync(
            hwnd,
            SW_RESTORE);

        return;
    }


    if (!IsIconic(hwnd))
        return;


    if (placement.flags &
        WPF_RESTORETOMAXIMIZED) {

        ShowWindowAsync(
            hwnd,
            SW_MAXIMIZE);

    } else {

        ShowWindowAsync(
            hwnd,
            SW_RESTORE);
    }
}


static void SwitchWindowStrong(
    HWND hwnd) {

    if (!hwnd ||
        !IsWindow(hwnd))
        return;


    RestoreWindowPreservingState(
        hwnd);


    keybd_event(
        VK_MENU,
        0,
        0,
        0);


    keybd_event(
        VK_MENU,
        0,
        KEYEVENTF_KEYUP,
        0);


    HMODULE user32 =
        GetModuleHandleW(
            L"user32.dll");


    if (user32) {

        auto switchFn =
            reinterpret_cast<
                SwitchToThisWindow_t>(

                GetProcAddress(
                    user32,
                    "SwitchToThisWindow"));


        if (switchFn) {

            switchFn(
                hwnd,
                TRUE);
        }
    }


    BringWindowToTop(hwnd);

    SetForegroundWindow(hwnd);
}


// ============================================================
// AÇÕES
// ============================================================

static void LaunchNewInstance(
    const AppGroup& app) {

    if (app.path.empty())
        return;


    ShellExecuteW(
        g_dockWindow,
        L"open",
        app.path.c_str(),
        nullptr,
        nullptr,
        SW_SHOWNORMAL);
}


static void CloseWindowSafe(
    HWND hwnd) {

    if (hwnd &&
        IsWindow(hwnd)) {

        PostMessageW(
            hwnd,
            WM_CLOSE,
            0,
            0);
    }
}


static void CloseAllWindows(
    const AppGroup& app) {

    for (const auto& window :
         app.windows) {

        CloseWindowSafe(
            window.hwnd);
    }
}


// ============================================================
// CLIQUE ESQUERDO
// ============================================================

static void ActivateGroup(
    AppGroup& app) {

    g_keepVisibleUntil =
        GetTickCount64() +
        1200;


    if (!app.running ||
        app.windows.empty()) {

        LaunchNewInstance(app);

        return;
    }


    app.windows.erase(
        std::remove_if(
            app.windows.begin(),
            app.windows.end(),

            [](const WindowInfo& info) {

                return
                    !IsWindow(
                        info.hwnd);
            }),

        app.windows.end());


    if (app.windows.empty()) {

        LaunchNewInstance(app);

        return;
    }


    HWND foreground =
        GetForegroundWindow();


    if (app.windows.size() == 1) {

        HWND hwnd =
            app.windows[0].hwnd;


        if (foreground == hwnd) {

            ShowWindowAsync(
                hwnd,
                SW_MINIMIZE);

            return;
        }


        SwitchWindowStrong(hwnd);

        return;
    }


    for (const auto& window :
         app.windows) {

        if (foreground ==
            window.hwnd) {

            ShowWindowAsync(
                window.hwnd,
                SW_MINIMIZE);

            return;
        }
    }


    if (app.cycleIndex >=
        app.windows.size()) {

        app.cycleIndex = 0;
    }


    HWND hwnd =
        app.windows[
            app.cycleIndex].hwnd;


    app.cycleIndex =
        (app.cycleIndex + 1) %
        app.windows.size();


    SwitchWindowStrong(hwnd);
}


// ============================================================
// AUTO-HIDE
// ============================================================

static void UpdateAutoHide() {

    if (!g_dockWindow)
        return;


    if (g_contextMenuOpen)
        return;


    POINT cursor = {};


    if (!GetCursorPos(
            &cursor))
        return;


    HMONITOR cursorMonitor =
        MonitorFromPoint(
            cursor,
            MONITOR_DEFAULTTONEAREST);


    if (cursorMonitor !=
        g_currentMonitor) {

        g_currentMonitor =
            cursorMonitor;

        g_hoveredIndex = -1;

        RefreshDock();
    }


    MONITORINFO mi = {};

    mi.cbSize =
        sizeof(mi);


    if (!GetMonitorInfoW(
            cursorMonitor,
            &mi))
        return;


    bool maximized =
        HasMaximizedWindowOnMonitor(
            cursorMonitor);


    if (!maximized) {

        PositionDock(
            cursorMonitor);


        if (!g_visible) {

            ShowWindow(
                g_dockWindow,
                SW_SHOWNOACTIVATE);


            SetWindowPos(
                g_dockWindow,
                HWND_TOPMOST,
                0,
                0,
                0,
                0,

                SWP_NOMOVE |
                SWP_NOSIZE |
                SWP_NOACTIVATE);


            g_visible = true;
        }


        g_outsideSince = 0;

        return;
    }


    ULONGLONG now =
        GetTickCount64();


    bool edge =

        cursor.x >=
            mi.rcMonitor.left &&

        cursor.x <
            mi.rcMonitor.right &&

        cursor.y >=
            mi.rcMonitor.bottom -
            g_settings.activationZone &&

        cursor.y <
            mi.rcMonitor.bottom;


    if (!g_visible) {

        if (edge) {

            g_currentMonitor =
                cursorMonitor;


            RefreshDock();


            ShowWindow(
                g_dockWindow,
                SW_SHOWNOACTIVATE);


            SetWindowPos(
                g_dockWindow,
                HWND_TOPMOST,
                0,
                0,
                0,
                0,

                SWP_NOMOVE |
                SWP_NOSIZE |
                SWP_NOACTIVATE);


            g_visible = true;
            g_outsideSince = 0;
        }


        return;
    }


    if (now <
        g_keepVisibleUntil) {

        return;
    }


    RECT rect = {};


    GetWindowRect(
        g_dockWindow,
        &rect);


    InflateRect(
        &rect,
        12,
        14);


    bool inside =
        PtInRect(
            &rect,
            cursor);


    if (inside ||
        edge) {

        g_outsideSince = 0;

        return;
    }


    if (!g_outsideSince) {

        g_outsideSince =
            now;

        return;
    }


    if (now -
            g_outsideSince >=
        static_cast<ULONGLONG>(
            g_settings.hideDelay)) {


        g_hoveredIndex = -1;


        ShowWindow(
            g_dockWindow,
            SW_HIDE);


        g_visible = false;
        g_outsideSince = 0;
    }
}


// ============================================================
// MENU
// ============================================================

static void ShowContextMenu(
    HWND hwnd,
    int index,
    POINT screenPoint) {

    if (index < 0 ||
        index >=
            static_cast<int>(
                g_apps.size()))
        return;


    AppGroup& app =
        g_apps[index];


    HMENU menu =
        CreatePopupMenu();


    if (!menu)
        return;


    AppendMenuW(
        menu,
        MF_STRING,
        10,
        L"Abrir nova janela");


    AppendMenuW(
        menu,
        MF_SEPARATOR,
        0,
        nullptr);


    AppendMenuW(
        menu,
        MF_STRING,
        20,

        app.pinned
            ? L"Desafixar da dock"
            : L"Fixar na dock");


    if (app.running &&
        !app.windows.empty()) {

        AppendMenuW(
            menu,
            MF_SEPARATOR,
            0,
            nullptr);


        UINT id = 100;


        for (const auto& window :
             app.windows) {

            std::wstring label =
                window.title.empty()
                    ? app.title
                    : window.title;


            AppendMenuW(
                menu,
                MF_STRING,
                id++,
                label.c_str());
        }


        AppendMenuW(
            menu,
            MF_SEPARATOR,
            0,
            nullptr);


        if (app.windows.size() == 1) {

            HWND target =
                app.windows[0].hwnd;


            if (IsIconic(target)) {

                AppendMenuW(
                    menu,
                    MF_STRING,
                    30,
                    L"Restaurar");

            } else {

                AppendMenuW(
                    menu,
                    MF_STRING,
                    31,
                    L"Minimizar");
            }


            AppendMenuW(
                menu,
                MF_STRING,
                40,
                L"Fechar janela");

        } else {

            AppendMenuW(
                menu,
                MF_STRING,
                41,
                L"Fechar todas as janelas");
        }
    }


    RECT dockRect = {};


    GetWindowRect(
        g_dockWindow,
        &dockRect);


    int menuX =
        screenPoint.x;


    int menuY =
        dockRect.top - 6;


    HWND previousForeground =
        GetForegroundWindow();


    LONG_PTR oldExStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE);


    g_contextMenuOpen = true;


    g_keepVisibleUntil =
        GetTickCount64() +
        5000;


    SetWindowLongPtrW(
        hwnd,
        GWL_EXSTYLE,
        oldExStyle &
            ~WS_EX_NOACTIVATE);


    SetWindowPos(
        hwnd,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,

        SWP_NOMOVE |
        SWP_NOSIZE |
        SWP_FRAMECHANGED);


    SetForegroundWindow(hwnd);

    SetActiveWindow(hwnd);

    BringWindowToTop(hwnd);


    UINT command =
        TrackPopupMenu(
            menu,

            TPM_RETURNCMD |
            TPM_RIGHTBUTTON |
            TPM_BOTTOMALIGN,

            menuX,
            menuY,

            0,
            hwnd,
            nullptr);


    PostMessageW(
        hwnd,
        WM_NULL,
        0,
        0);


    DestroyMenu(menu);


    SetWindowLongPtrW(
        hwnd,
        GWL_EXSTYLE,
        oldExStyle);


    SetWindowPos(
        hwnd,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,

        SWP_NOMOVE |
        SWP_NOSIZE |
        SWP_NOACTIVATE |
        SWP_FRAMECHANGED);


    g_contextMenuOpen = false;


    if (command == 0 &&
        previousForeground &&
        IsWindow(previousForeground)) {

        SetForegroundWindow(
            previousForeground);
    }


    if (command == 10) {

        LaunchNewInstance(app);

        return;
    }


    if (command == 20) {

        TogglePinned(
            app.path);

        RefreshDock();

        return;
    }


    if (command >= 100 &&
        command <
            100 +
            app.windows.size()) {

        size_t windowIndex =
            command - 100;


        SwitchWindowStrong(
            app.windows[
                windowIndex].hwnd);

        return;
    }


    if (command == 30 &&
        !app.windows.empty()) {

        SwitchWindowStrong(
            app.windows[0].hwnd);

        return;
    }


    if (command == 31 &&
        !app.windows.empty()) {

        ShowWindowAsync(
            app.windows[0].hwnd,
            SW_MINIMIZE);

        return;
    }


    if (command == 40 &&
        !app.windows.empty()) {

        CloseWindowSafe(
            app.windows[0].hwnd);

        return;
    }


    if (command == 41) {

        CloseAllWindows(app);

        return;
    }
}


// ============================================================
// DESENHO PREMIUM
// ============================================================

static bool AppIsForeground(
    const AppGroup& app) {

    HWND foreground =
        GetForegroundWindow();


    for (const auto& window :
         app.windows) {

        if (foreground ==
            window.hwnd)
            return true;
    }


    return false;
}


static COLORREF BlendColor(
    COLORREF a,
    COLORREF b,
    int percentB) {

    if (percentB < 0)
        percentB = 0;

    if (percentB > 100)
        percentB = 100;


    int percentA =
        100 - percentB;


    int r =
        (GetRValue(a) * percentA +
         GetRValue(b) * percentB) /
        100;


    int g =
        (GetGValue(a) * percentA +
         GetGValue(b) * percentB) /
        100;


    int bl =
        (GetBValue(a) * percentA +
         GetBValue(b) * percentB) /
        100;


    return RGB(r, g, bl);
}


static void DrawRoundedFill(
    HDC hdc,
    const RECT& rect,
    int radius,
    COLORREF color) {

    HBRUSH brush =
        CreateSolidBrush(color);


    HGDIOBJ oldBrush =
        SelectObject(
            hdc,
            brush);


    HGDIOBJ oldPen =
        SelectObject(
            hdc,
            GetStockObject(NULL_PEN));


    RoundRect(
        hdc,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom,
        radius,
        radius);


    SelectObject(
        hdc,
        oldPen);


    SelectObject(
        hdc,
        oldBrush);


    DeleteObject(brush);
}


static void PaintDock(
    HWND hwnd,
    HDC hdc) {

    RECT client = {};


    GetClientRect(
        hwnd,
        &client);


    // Fundo propositalmente mais suave,
    // pois o DWM já fornece o backdrop.
    COLORREF base =

        g_lightTheme
            ? RGB(245, 245, 245)
            : RGB(31, 31, 34);


    HBRUSH backgroundBrush =
        CreateSolidBrush(base);


    FillRect(
        hdc,
        &client,
        backgroundBrush);


    DeleteObject(
        backgroundBrush);


    for (int i = 0;
         i <
         static_cast<int>(
             g_apps.size());
         ++i) {


        const auto& app =
            g_apps[i];


        bool hovered =
            i ==
            g_hoveredIndex;


        // ====================================================
        // CÁPSULA HOVER
        // ====================================================

        if (hovered &&
            g_settings.hoverHighlight) {


            RECT hoverRect =
                app.rect;


            InflateRect(
                &hoverRect,
                5,
                5);


            COLORREF hoverColor =

                g_lightTheme

                    ? BlendColor(
                          RGB(245, 245, 245),
                          g_accentColor,
                          10)

                    : BlendColor(
                          RGB(31, 31, 34),
                          g_accentColor,
                          17);


            DrawRoundedFill(
                hdc,
                hoverRect,
                14,
                hoverColor);
        }


        // ====================================================
        // ÍCONE
        // ====================================================

        if (app.icon) {


            int drawSize =
                g_settings.iconSize;


            if (hovered) {

                drawSize =
                    g_settings.iconSize *
                    g_settings.hoverScale /
                    100;
            }


            // Limite para o ícone não escapar demais
            // da própria dock.
            int maxSize =
                g_settings.dockHeight - 6;


            if (drawSize > maxSize)
                drawSize = maxSize;


            int centerX =
                (app.rect.left +
                 app.rect.right) /
                2;


            int centerY =
                (app.rect.top +
                 app.rect.bottom) /
                2;


            int drawX =
                centerX -
                drawSize / 2;


            int drawY =
                centerY -
                drawSize / 2;


            // Ícone em hover sobe discretamente.
            if (hovered)
                drawY -= 2;


            DrawIconEx(
                hdc,
                drawX,
                drawY,
                app.icon,
                drawSize,
                drawSize,
                0,
                nullptr,
                DI_NORMAL);
        }


        // ====================================================
        // INDICADOR DE APP ABERTO
        // ====================================================

        if (app.running) {


            int center =
                (app.rect.left +
                 app.rect.right) /
                2;


            bool active =
                AppIsForeground(app);


            int indicatorWidth =
                active ? 15 : 5;


            RECT indicator = {

                center -
                    indicatorWidth / 2,

                g_settings.dockHeight - 5,

                center +
                    indicatorWidth / 2,

                g_settings.dockHeight - 2
            };


            COLORREF color =

                active
                    ? g_accentColor

                    : (g_lightTheme
                           ? RGB(
                                 115,
                                 115,
                                 115)

                           : RGB(
                                 178,
                                 178,
                                 178));


            DrawRoundedFill(
                hdc,
                indicator,
                4,
                color);
        }


        // ====================================================
        // MAIS DE UMA JANELA
        // ====================================================

        if (app.windows.size() > 1) {


            RECT marker = {

                app.rect.right - 8,
                app.rect.top + 1,

                app.rect.right - 2,
                app.rect.top + 4
            };


            DrawRoundedFill(
                hdc,
                marker,
                3,
                g_accentColor);
        }
    }
}


// ============================================================
// HOVER
// ============================================================

static int HitTestApp(
    POINT point) {

    for (int i = 0;
         i <
         static_cast<int>(
             g_apps.size());
         ++i) {

        RECT hitRect =
            g_apps[i].rect;


        InflateRect(
            &hitRect,
            4,
            4);


        if (PtInRect(
                &hitRect,
                point)) {

            return i;
        }
    }


    return -1;
}


static void UpdateHover(
    HWND hwnd,
    LPARAM lParam) {

    if (!g_trackingMouse) {


        TRACKMOUSEEVENT tracking = {};


        tracking.cbSize =
            sizeof(tracking);


        tracking.dwFlags =
            TME_LEAVE;


        tracking.hwndTrack =
            hwnd;


        if (TrackMouseEvent(
                &tracking)) {

            g_trackingMouse = true;
        }
    }


    POINT point = {

        GET_X_LPARAM(
            lParam),

        GET_Y_LPARAM(
            lParam)
    };


    int newHovered =
        HitTestApp(point);


    if (newHovered !=
        g_hoveredIndex) {

        g_hoveredIndex =
            newHovered;


        InvalidateRect(
            hwnd,
            nullptr,
            FALSE);
    }
}


// ============================================================
// WINDOW PROC
// ============================================================

static LRESULT CALLBACK DockWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam) {

    switch (msg) {


        case WM_MOUSEACTIVATE:

            if (g_contextMenuOpen)
                return MA_ACTIVATE;

            return MA_NOACTIVATE;


        case WM_CREATE: {

            SetTimer(
                hwnd,
                MAIN_TIMER,
                30,
                nullptr);


            g_lastRefresh =
                GetTickCount64();


            return 0;
        }


        case WM_MOUSEMOVE: {

            UpdateHover(
                hwnd,
                lParam);


            return 0;
        }


        case WM_MOUSELEAVE: {

            g_trackingMouse =
                false;


            if (g_hoveredIndex != -1) {

                g_hoveredIndex =
                    -1;


                InvalidateRect(
                    hwnd,
                    nullptr,
                    FALSE);
            }


            return 0;
        }


        case WM_TIMER: {

            if (wParam ==
                MAIN_TIMER) {


                UpdateAutoHide();


                ULONGLONG now =
                    GetTickCount64();


                if (now -
                        g_lastRefresh >=
                    800) {


                    g_lastRefresh =
                        now;


                    LoadWindowsTheme();


                    ApplyPremiumDwm(
                        hwnd);


                    RefreshDock();
                }
            }


            return 0;
        }


        case WM_SETTINGCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED: {


            LoadWindowsTheme();


            ApplyPremiumDwm(
                hwnd);


            InvalidateRect(
                hwnd,
                nullptr,
                TRUE);


            return 0;
        }


        case WM_DOCK_SETTINGS_CHANGED: {


            LoadSettings();

            LoadWindowsTheme();


            SetLayeredWindowAttributes(
                hwnd,
                0,
                static_cast<BYTE>(
                    g_settings.opacity),
                LWA_ALPHA);


            ApplyPremiumDwm(
                hwnd);


            RefreshDock();


            return 0;
        }


        case WM_LBUTTONDOWN: {


            POINT point = {

                GET_X_LPARAM(
                    lParam),

                GET_Y_LPARAM(
                    lParam)
            };


            int index =
                HitTestApp(point);


            if (index >= 0) {

                ActivateGroup(
                    g_apps[index]);
            }


            return 0;
        }


        case WM_MBUTTONUP: {


            POINT point = {

                GET_X_LPARAM(
                    lParam),

                GET_Y_LPARAM(
                    lParam)
            };


            int index =
                HitTestApp(point);


            if (index >= 0) {

                LaunchNewInstance(
                    g_apps[index]);
            }


            return 0;
        }


        case WM_RBUTTONUP: {


            POINT point = {

                GET_X_LPARAM(
                    lParam),

                GET_Y_LPARAM(
                    lParam)
            };


            int index =
                HitTestApp(point);


            if (index >= 0) {


                POINT screen =
                    point;


                ClientToScreen(
                    hwnd,
                    &screen);


                ShowContextMenu(
                    hwnd,
                    index,
                    screen);
            }


            return 0;
        }


        case WM_PAINT: {


            PAINTSTRUCT ps = {};


            HDC hdc =
                BeginPaint(
                    hwnd,
                    &ps);


            PaintDock(
                hwnd,
                hdc);


            EndPaint(
                hwnd,
                &ps);


            return 0;
        }


        case WM_ERASEBKGND:
            return 1;


        case WM_DESTROY: {


            KillTimer(
                hwnd,
                MAIN_TIMER);


            ClearApps();

            ClearEnumeratedWindows();


            g_dockWindow =
                nullptr;


            PostQuitMessage(0);


            return 0;
        }
    }


    return DefWindowProcW(
        hwnd,
        msg,
        wParam,
        lParam);
}


// ============================================================
// THREAD
// ============================================================

static DWORD WINAPI DockThreadProc(
    LPVOID) {


    const wchar_t* className =
        L"WindhawkBottomAppDockV09";


    HINSTANCE instance =
        GetModuleHandleW(
            nullptr);


    WNDCLASSEXW wc = {};


    wc.cbSize =
        sizeof(wc);


    wc.lpfnWndProc =
        DockWndProc;


    wc.hInstance =
        instance;


    wc.hCursor =
        LoadCursorW(
            nullptr,
            IDC_ARROW);


    wc.lpszClassName =
        className;


    // Sem background brush:
    // o conteúdo é pintado por nós e o DWM fica livre
    // para aplicar seu backdrop.
    wc.hbrBackground =
        nullptr;


    if (!RegisterClassExW(
            &wc)) {

        return 0;
    }


    HWND hwnd =
        CreateWindowExW(

            WS_EX_TOOLWINDOW |
            WS_EX_TOPMOST |
            WS_EX_LAYERED |
            WS_EX_NOACTIVATE,

            className,

            L"Bottom App Dock",

            WS_POPUP,

            0,
            0,
            100,
            g_settings.dockHeight,

            nullptr,
            nullptr,
            instance,
            nullptr);


    if (!hwnd)
        return 0;


    g_dockWindow =
        hwnd;


    SetLayeredWindowAttributes(
        hwnd,
        0,
        static_cast<BYTE>(
            g_settings.opacity),
        LWA_ALPHA);


    LoadPinnedApps();

    LoadWindowsTheme();


    ApplyPremiumDwm(
        hwnd);


    RefreshDock();


    ShowWindow(
        hwnd,
        SW_HIDE);


    g_visible =
        false;


    MSG msg = {};


    while (GetMessageW(
               &msg,
               nullptr,
               0,
               0) > 0) {


        TranslateMessage(
            &msg);


        DispatchMessageW(
            &msg);
    }


    UnregisterClassW(
        className,
        instance);


    return 0;
}


// ============================================================
// WINDHAWK
// ============================================================

BOOL Wh_ModInit() {


    LoadSettings();

    LoadWindowsTheme();


    g_thread =
        CreateThread(
            nullptr,
            0,
            DockThreadProc,
            nullptr,
            0,
            nullptr);


    return
        g_thread != nullptr;
}


void Wh_ModUninit() {


    if (g_dockWindow) {

        PostMessageW(
            g_dockWindow,
            WM_CLOSE,
            0,
            0);
    }


    if (g_thread) {

        WaitForSingleObject(
            g_thread,
            3000);


        CloseHandle(
            g_thread);


        g_thread =
            nullptr;
    }
}


void Wh_ModSettingsChanged() {


    LoadSettings();


    if (g_dockWindow) {

        PostMessageW(
            g_dockWindow,
            WM_DOCK_SETTINGS_CHANGED,
            0,
            0);
    }
}
