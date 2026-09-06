// ==WindhawkMod==
// @id              bottom-app-dock
// @name            Bottom App Dock
// @description     Dock premium inferior, multi-monitor, com apps fixados, auto-hide e integração visual ao Windows
// @version         0.12.4
// @author          Keygreen3D
// @github          https://github.com/keygreen3d
// @include         explorer.exe
// @compilerOptions -lshell32 -ldwmapi -lgdi32 -ladvapi32 -lole32 -lgdiplus
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bottom App Dock v0.12.4 Premium

Mantém a base visual estável e adiciona previews estilo GNOME, ícones estáveis e reconciliação dinâmica de monitores.

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

- hoverScale: 100
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
#include <shlobj.h>
#include <dwmapi.h>
#include <gdiplus.h>

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
constexpr DWORD DWM_ATTR_NCRENDERING_POLICY = 2;
constexpr int DWM_NCRP_DISABLED = 1;

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

    int hoverScale = 100;

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

struct DockInstance {
    HWND hwnd = nullptr;
    HMONITOR monitor = nullptr;

    bool visible = false;
    bool trackingMouse = false;

    int hoveredIndex = -1;
    ULONGLONG hoverSince = 0;

    ULONGLONG outsideSince = 0;
};

struct PreviewItem {
    HWND target = nullptr;
    HTHUMBNAIL thumbnail = nullptr;
    RECT rect = {};
    RECT closeRect = {};
    std::wstring title;
};

struct PreviewState {
    HWND hwnd = nullptr;
    HWND ownerDock = nullptr;
    std::wstring appPath;
    std::vector<PreviewItem> items;
    int hoveredItem = -1;
    int hoveredClose = -1;
    ULONGLONG hideDeadline = 0;
    bool visible = false;
};


// ============================================================
// GLOBAIS
// ============================================================

static Settings g_settings;

static HANDLE g_thread = nullptr;

static std::vector<AppGroup> g_apps;
static std::vector<std::wstring> g_pinnedPaths;
static std::vector<std::wstring> g_dynamicOrder;
static std::vector<EnumeratedWindow> g_enumWindows;

struct RestoreWindowState {
    HWND hwnd = nullptr;
    bool wasMaximized = false;
};

static std::vector<RestoreWindowState>
    g_restoreWindowStates;


static std::vector<DockInstance> g_dockInstances;
static PreviewState g_preview;

static HINSTANCE g_moduleInstance = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
static const wchar_t* g_dockClassName = L"WindhawkBottomAppDockV010";
static const wchar_t* g_previewClassName = L"WindhawkBottomAppPreviewV010";

static bool g_lightTheme = false;
static bool g_contextMenuOpen = false;

static ULONGLONG g_lastRefresh = 0;
static ULONGLONG g_keepVisibleUntil = 0;

static COLORREF g_accentColor =
    RGB(0, 120, 215);

// Lixeira fixa estilo macOS
static RECT g_recycleRect = {};
static int g_recycleSeparatorX = -1;
static HICON g_recycleIcon = nullptr;
static bool g_recycleFull = false;
static ULONGLONG g_lastRecycleRefresh = 0;

constexpr int HIT_RECYCLE_BIN = -2;
constexpr UINT_PTR MAIN_TIMER = 1;

constexpr UINT WM_DOCK_SETTINGS_CHANGED =
    WM_APP + 1;

constexpr ULONGLONG PREVIEW_HOVER_DELAY = 320;
constexpr ULONGLONG PREVIEW_HIDE_DELAY = 260;

static void EnsureDockInstancesForCurrentMonitors();
static void HidePreview();
static void SwitchWindowStrong(HWND hwnd);
static COLORREF BlendColor(COLORREF a, COLORREF b, int percentB);
static void DrawRoundedFill(HDC hdc, const RECT& rect, int radius, COLORREF color);
static void ApplyPreviewRegion();

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


static bool IsExcludedAppPath(
    const std::wstring& path) {

    std::wstring fileName =
        GetFileName(path);

    // Raycast
    if (_wcsicmp(
            fileName.c_str(),
            L"Raycast.exe") == 0) {
        return true;
    }

    // Ferramenta de Captura do Windows
    if (_wcsicmp(
            fileName.c_str(),
            L"SnippingTool.exe") == 0) {
        return true;
    }

    if (_wcsicmp(
            fileName.c_str(),
            L"ScreenClippingHost.exe") == 0) {
        return true;
    }

    // Painel de notificações e superfícies temporárias do shell do Windows.
    if (_wcsicmp(
            fileName.c_str(),
            L"ShellExperienceHost.exe") == 0) {
        return true;
    }

    if (_wcsicmp(
            fileName.c_str(),
            L"ShellHost.exe") == 0) {
        return true;
    }

    return false;
}

static DockInstance* FindDockInstance(
    HWND hwnd) {

    for (auto& dock : g_dockInstances) {
        if (dock.hwnd == hwnd)
            return &dock;
    }

    return nullptr;
}


static bool IsDockWindow(
    HWND hwnd) {

    return FindDockInstance(hwnd) != nullptr ||
           (g_preview.hwnd && hwnd == g_preview.hwnd);
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


    bool previewWindow =
        g_preview.hwnd &&
        hwnd == g_preview.hwnd;

    int corner =
        previewWindow
            ? 1   // DWMWCP_DONOTROUND
            : DWM_CORNER_ROUND;

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


    bool isPreview =
        g_preview.hwnd &&
        hwnd == g_preview.hwnd;

    // O preview usa SetWindowRgn para definir exatamente o que aparece.
    // Portanto o DWM não deve desenhar backdrop nem sombra/moldura própria
    // ao redor da área transparente.
    int backdrop =
        (g_settings.premiumBackdrop && !isPreview)
            ? DWM_BACKDROP_TRANSIENT
            : 1;

    DwmSetWindowAttribute(
        hwnd,
        static_cast<DWMWINDOWATTRIBUTE>(
            DWM_ATTR_SYSTEM_BACKDROP),
        &backdrop,
        sizeof(backdrop));

    if (isPreview) {

        int ncPolicy =
            DWM_NCRP_DISABLED;

        DwmSetWindowAttribute(
            hwnd,
            static_cast<DWMWINDOWATTRIBUTE>(
                DWM_ATTR_NCRENDERING_POLICY),
            &ncPolicy,
            sizeof(ncPolicy));
    }

    // Força o DWM a recalcular a moldura sem efeitos antigos presos.
    SetWindowPos(
        hwnd,
        nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE |
        SWP_NOSIZE |
        SWP_NOZORDER |
        SWP_NOACTIVATE |
        SWP_FRAMECHANGED);
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


    if (IsDockWindow(hwnd))
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
            L"WorkerW") == 0 ||

        // Windows 10/11 shell flyouts: notificações, calendário,
        // quick settings e superfícies XAML temporárias.
        _wcsicmp(
            cls,
            L"Windows.UI.Core.CoreWindow") == 0 ||

        _wcsicmp(
            cls,
            L"XamlExplorerHostIslandWindow") == 0 ||

        _wcsicmp(
            cls,
            L"ControlCenterWindow") == 0 ||

        _wcsicmp(
            cls,
            L"NotificationCenterWindow") == 0) {

        return false;
    }

wchar_t windowTitle[512] = {};

GetWindowTextW(
    hwnd,
    windowTitle,
    ARRAYSIZE(windowTitle));

if (_wcsicmp(
        windowTitle,
        L"Nova notificação") == 0 ||
    _wcsicmp(
        windowTitle,
        L"New notification") == 0 ||
    _wcsicmp(
        windowTitle,
        L"Notificações") == 0 ||
    _wcsicmp(
        windowTitle,
        L"Notifications") == 0 ||
    _wcsicmp(
        windowTitle,
        L"Central de Ações") == 0 ||
    _wcsicmp(
        windowTitle,
        L"Action Center") == 0) {

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

    if (IsExcludedAppPath(path))
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

    g_pinnedPaths.erase(
        std::remove_if(
            g_pinnedPaths.begin(),
            g_pinnedPaths.end(),
            [](const std::wstring& path) {
                return IsExcludedAppPath(path);
            }),
        g_pinnedPaths.end());

    g_dynamicOrder.erase(
        std::remove_if(
            g_dynamicOrder.begin(),
            g_dynamicOrder.end(),
            [](const std::wstring& path) {
                return IsExcludedAppPath(path);
            }),
        g_dynamicOrder.end());


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

        if (IsExcludedAppPath(path))
            return;


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


            // O ícone da dock representa o aplicativo, não a pasta/documento
            // atualmente exibido pela janela. O título continua específico
            // para uso nos previews.
            if (app.title == GetFileName(path)) {
                app.title = item.title;
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

}


// ============================================================
// LIXEIRA FIXA
// ============================================================

static bool RefreshRecycleBinIcon() {

    SHQUERYRBINFO rbInfo = {};
    rbInfo.cbSize = sizeof(rbInfo);

    HRESULT hr =
        SHQueryRecycleBinW(
            nullptr,
            &rbInfo);

    if (FAILED(hr))
        return false;

    bool full =
        rbInfo.i64NumItems > 0;

    if (g_recycleIcon &&
        full == g_recycleFull) {
        return false;
    }

    // Obtém o ícone real da Lixeira diretamente do namespace do Shell.
    // Assim não dependemos de IDs numéricos de SHSTOCKICONID.
    PIDLIST_ABSOLUTE pidl = nullptr;
    HICON newIcon = nullptr;

    if (SUCCEEDED(
            SHParseDisplayName(
                L"shell:RecycleBinFolder",
                nullptr,
                &pidl,
                0,
                nullptr)) &&
        pidl) {

        SHFILEINFOW sfi = {};

        if (SHGetFileInfoW(
                reinterpret_cast<LPCWSTR>(pidl),
                0,
                &sfi,
                sizeof(sfi),
                SHGFI_PIDL |
                SHGFI_ICON |
                SHGFI_LARGEICON)) {

            newIcon = sfi.hIcon;
        }

        CoTaskMemFree(pidl);
    }

    if (!newIcon)
        return false;

    HICON oldIcon =
        g_recycleIcon;

    g_recycleIcon =
        newIcon;

    g_recycleFull =
        full;

    if (oldIcon)
        DestroyIcon(oldIcon);

    return true;
}

static void OpenRecycleBin() {

    ShellExecuteW(
        nullptr,
        L"open",
        L"shell:RecycleBinFolder",
        nullptr,
        nullptr,
        SW_SHOWNORMAL);
}


static void EmptyRecycleBin(
    HWND owner) {

    SHEmptyRecycleBinW(
        owner,
        nullptr,
        0);

    RefreshRecycleBinIcon();

    for (const auto& dock :
         g_dockInstances) {

        if (dock.hwnd) {
            InvalidateRect(
                dock.hwnd,
                nullptr,
                TRUE);
        }
    }
}


static void ShowRecycleContextMenu(
    HWND hwnd,
    POINT screenPoint) {

    HidePreview();

    HMENU menu =
        CreatePopupMenu();

    if (!menu)
        return;

    AppendMenuW(
        menu,
        MF_STRING,
        1,
        L"Abrir Lixeira");

    AppendMenuW(
        menu,
        MF_SEPARATOR,
        0,
        nullptr);

    AppendMenuW(
        menu,
        MF_STRING,
        2,
        L"Esvaziar Lixeira");

    LONG_PTR oldExStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE);

    g_contextMenuOpen = true;
    g_keepVisibleUntil =
        GetTickCount64() + 5000;

    SetWindowLongPtrW(
        hwnd,
        GWL_EXSTYLE,
        oldExStyle &
            ~WS_EX_NOACTIVATE);

    SetWindowPos(
        hwnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
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
            screenPoint.x,
            screenPoint.y,
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
        0, 0, 0, 0,
        SWP_NOMOVE |
        SWP_NOSIZE |
        SWP_NOACTIVATE |
        SWP_FRAMECHANGED);

    g_contextMenuOpen = false;

    if (command == 1) {
        OpenRecycleBin();
    } else if (command == 2) {
        EmptyRecycleBin(hwnd);
    }
}


// ============================================================
// POSIÇÃO
// ============================================================

static void PositionDock(
    DockInstance& dock) {

    if (!dock.hwnd ||
        !dock.monitor)
        return;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(
            dock.monitor,
            &mi))
        return;

    const int count =
        static_cast<int>(
            g_apps.size());

    const int recycleGap = 14;
    const int separatorWidth = 1;

    int appsWidth = 0;

    if (count > 0) {
        appsWidth =
            count *
            g_settings.iconSize;

        appsWidth +=
            (count - 1) *
            g_settings.iconSpacing;
    }

    int width =
        g_settings.sidePadding * 2 +
        appsWidth +
        g_settings.iconSize;

    if (count > 0) {
        width +=
            recycleGap * 2 +
            separatorWidth;
    }

    int screenWidth =
        mi.rcMonitor.right -
        mi.rcMonitor.left;

    int x =
        mi.rcMonitor.left +
        (screenWidth - width) / 2;

    int y =
        mi.rcMonitor.bottom -
        g_settings.dockHeight -
        g_settings.bottomMargin;

    SetWindowPos(
        dock.hwnd,
        HWND_TOPMOST,
        x,
        y,
        width,
        g_settings.dockHeight,
        SWP_NOACTIVATE |
        SWP_NOOWNERZORDER);

    int left =
        g_settings.sidePadding;

    const int top =
        (g_settings.dockHeight -
         g_settings.iconSize) /
        2 - 2;

    for (auto& app :
         g_apps) {

        app.rect = {
            left,
            top,
            left + g_settings.iconSize,
            top + g_settings.iconSize
        };

        left +=
            g_settings.iconSize +
            g_settings.iconSpacing;
    }

    if (count > 0) {

        // Remove o último spacing virtual para obter o fim real dos apps.
        int appsRight =
            g_settings.sidePadding +
            appsWidth;

        g_recycleSeparatorX =
            appsRight +
            recycleGap;

        int recycleLeft =
            g_recycleSeparatorX +
            separatorWidth +
            recycleGap;

        g_recycleRect = {
            recycleLeft,
            top,
            recycleLeft + g_settings.iconSize,
            top + g_settings.iconSize
        };

    } else {

        g_recycleSeparatorX = -1;

        g_recycleRect = {
            g_settings.sidePadding,
            top,
            g_settings.sidePadding +
                g_settings.iconSize,
            top + g_settings.iconSize
        };
    }
}

// ============================================================
// REFRESH
// ============================================================

static void RefreshDock() {

    g_restoreWindowStates.erase(
        std::remove_if(
            g_restoreWindowStates.begin(),
            g_restoreWindowStates.end(),
            [](const RestoreWindowState& state) {
                return
                    !state.hwnd ||
                    !IsWindow(state.hwnd);
            }),
        g_restoreWindowStates.end());

    BuildDockApps();

    for (auto& dock :
         g_dockInstances) {

        if (!dock.hwnd)
            continue;

        if (dock.hoveredIndex >=
            static_cast<int>(
                g_apps.size())) {

            dock.hoveredIndex = -1;
        }

        PositionDock(dock);

        InvalidateRect(
            dock.hwnd,
            nullptr,
            TRUE);
    }
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


static void RememberWindowRestoreState(
    HWND hwnd,
    bool wasMaximized) {

    for (auto& state :
         g_restoreWindowStates) {

        if (state.hwnd == hwnd) {
            state.wasMaximized =
                wasMaximized;
            return;
        }
    }

    RestoreWindowState state;
    state.hwnd = hwnd;
    state.wasMaximized =
        wasMaximized;

    g_restoreWindowStates.push_back(
        state);
}


static bool TakeWindowRestoreState(
    HWND hwnd,
    bool& wasMaximized) {

    for (auto it =
             g_restoreWindowStates.begin();
         it !=
             g_restoreWindowStates.end();
         ++it) {

        if (it->hwnd == hwnd) {

            wasMaximized =
                it->wasMaximized;

            g_restoreWindowStates.erase(
                it);

            return true;
        }
    }

    return false;
}


// ============================================================
// ATIVAÇÃO
// ============================================================

typedef VOID (WINAPI*
    SwitchToThisWindow_t)(
        HWND,
        BOOL);


static void MinimizeWindowPreservingState(
    HWND hwnd) {

    if (!hwnd ||
        !IsWindow(hwnd))
        return;

    bool wasMaximized =
        IsZoomed(hwnd) != FALSE;

    WINDOWPLACEMENT placement = {};
    placement.length =
        sizeof(placement);

    if (GetWindowPlacement(
            hwnd,
            &placement)) {

        if (placement.showCmd ==
            SW_SHOWMAXIMIZED) {

            wasMaximized = true;
        }
    }

    RememberWindowRestoreState(
        hwnd,
        wasMaximized);

    ShowWindowAsync(
        hwnd,
        SW_MINIMIZE);
}


static bool RestoreWindowForActivation(
    HWND hwnd) {

    if (!hwnd ||
        !IsWindow(hwnd))
        return false;

    if (!IsIconic(hwnd))
        return false;

    bool restoreMaximized = false;

    bool foundState =
        TakeWindowRestoreState(
            hwnd,
            restoreMaximized);

    if (!foundState) {

        WINDOWPLACEMENT placement = {};
        placement.length =
            sizeof(placement);

        if (GetWindowPlacement(
                hwnd,
                &placement)) {

            restoreMaximized =
                (placement.flags &
                 WPF_RESTORETOMAXIMIZED) != 0;
        }
    }

    // Primeiro tira a janela do estado minimizado.
    // A maximização final será aplicada DEPOIS de trazê-la ao foreground,
    // evitando que SwitchToThisWindow/SetForegroundWindow reverta o estado.
    ShowWindow(
        hwnd,
        SW_RESTORE);

    return restoreMaximized;
}


static void SwitchWindowStrong(
    HWND hwnd) {

    if (!hwnd ||
        !IsWindow(hwnd))
        return;

    bool wasIconic =
        IsIconic(hwnd) != FALSE;

    bool restoreMaximized =
        false;

    if (wasIconic) {

        restoreMaximized =
            RestoreWindowForActivation(
                hwnd);
    }

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

    BringWindowToTop(
        hwnd);

    SetForegroundWindow(
        hwnd);

    // IMPORTANTE:
    // maximiza por último. Nas versões anteriores a janela era
    // maximizada antes de SwitchToThisWindow, e essa chamada podia
    // restaurá-la novamente para o tamanho normal.
    if (wasIconic &&
        restoreMaximized &&
        IsWindow(hwnd)) {

        ShowWindow(
            hwnd,
            SW_MAXIMIZE);

        BringWindowToTop(
            hwnd);

        SetForegroundWindow(
            hwnd);
    }
}

// ============================================================
// AÇÕES
// ============================================================

static void LaunchNewInstance(
    const AppGroup& app) {

    if (app.path.empty())
        return;


    ShellExecuteW(
        nullptr,
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

            MinimizeWindowPreservingState(
                hwnd);

            return;
        }


        SwitchWindowStrong(hwnd);

        return;
    }


    for (const auto& window :
         app.windows) {

        if (foreground ==
            window.hwnd) {

            MinimizeWindowPreservingState(
                window.hwnd);

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
// PREVIEW DE JANELAS (ESTILO GNOME)
// ============================================================

static void ClearPreviewItems() {

    for (auto& item : g_preview.items) {
        if (item.thumbnail) {
            DwmUnregisterThumbnail(item.thumbnail);
            item.thumbnail = nullptr;
        }
    }

    g_preview.items.clear();
    g_preview.hoveredItem = -1;
    g_preview.hoveredClose = -1;
}


static void HidePreview() {

    ClearPreviewItems();

    if (g_preview.hwnd) {

        // Remove recorte anterior; o próximo preview cria uma nova região.
        SetWindowRgn(
            g_preview.hwnd,
            nullptr,
            FALSE);

        ShowWindow(
            g_preview.hwnd,
            SW_HIDE);
    }

    g_preview.ownerDock = nullptr;
    g_preview.appPath.clear();
    g_preview.hideDeadline = 0;
    g_preview.visible = false;
}


static int HitTestPreviewItem(POINT point) {

    for (int i = 0;
         i < static_cast<int>(g_preview.items.size());
         ++i) {

        if (PtInRect(&g_preview.items[i].rect, point))
            return i;
    }

    return -1;
}


static int HitTestPreviewClose(POINT point) {

    for (int i = 0;
         i < static_cast<int>(g_preview.items.size());
         ++i) {

        if (PtInRect(&g_preview.items[i].closeRect, point))
            return i;
    }

    return -1;
}



static void ApplyPreviewRegion() {

    if (!g_preview.hwnd)
        return;

    HRGN combined =
        CreateRectRgn(
            0, 0, 0, 0);

    if (!combined)
        return;

    for (const auto& item :
         g_preview.items) {

        // Cartão principal arredondado.
        HRGN card =
            CreateRoundRectRgn(
                item.rect.left,
                item.rect.top,
                item.rect.right + 1,
                item.rect.bottom + 1,
                18,
                18);

        if (card) {

            CombineRgn(
                combined,
                combined,
                card,
                RGN_OR);

            DeleteObject(card);
        }

        // Botão vermelho pode ficar parcialmente fora do cartão.
        HRGN closeCircle =
            CreateEllipticRgn(
                item.closeRect.left,
                item.closeRect.top,
                item.closeRect.right + 1,
                item.closeRect.bottom + 1);

        if (closeCircle) {

            CombineRgn(
                combined,
                combined,
                closeCircle,
                RGN_OR);

            DeleteObject(
                closeCircle);
        }
    }

    // Depois de SetWindowRgn, o sistema passa a ser dono da região.
    SetWindowRgn(
        g_preview.hwnd,
        combined,
        TRUE);
}


static void PaintPreview(HWND hwnd, HDC hdc) {

    RECT client = {};
    GetClientRect(hwnd, &client);

    COLORREF base =
        g_lightTheme
            ? RGB(245, 245, 245)
            : RGB(31, 31, 34);

    // GDI+ é usado somente para as formas do preview.
    // O thumbnail continua sendo fornecido pelo DWM.
    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

    auto makeColor =
        [](COLORREF c, BYTE alpha = 255) {
            return Gdiplus::Color(
                alpha,
                GetRValue(c),
                GetGValue(c),
                GetBValue(c));
        };

    auto addRoundedRect =
        [](Gdiplus::GraphicsPath& path,
           float x,
           float y,
           float width,
           float height,
           float radius) {

            float d = radius * 2.0f;

            if (d > width)
                d = width;

            if (d > height)
                d = height;

            path.AddArc(x, y, d, d, 180.0f, 90.0f);
            path.AddArc(x + width - d, y, d, d, 270.0f, 90.0f);
            path.AddArc(x + width - d, y + height - d, d, d, 0.0f, 90.0f);
            path.AddArc(x, y + height - d, d, d, 90.0f, 90.0f);
            path.CloseFigure();
        };

    // Cartões com antialiasing real.
    for (const auto& item : g_preview.items) {

        Gdiplus::GraphicsPath cardPath;

        addRoundedRect(
            cardPath,
            static_cast<float>(item.rect.left),
            static_cast<float>(item.rect.top),
            static_cast<float>(item.rect.right - item.rect.left),
            static_cast<float>(item.rect.bottom - item.rect.top),
            9.0f);

        Gdiplus::SolidBrush cardBrush(
            makeColor(base));

        graphics.FillPath(
            &cardBrush,
            &cardPath);
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(
        hdc,
        g_lightTheme
            ? RGB(28, 28, 30)
            : RGB(238, 238, 240));

    HFONT font =
        static_cast<HFONT>(
            GetStockObject(DEFAULT_GUI_FONT));

    HGDIOBJ oldFont =
        SelectObject(
            hdc,
            font);

    for (int i = 0;
         i < static_cast<int>(g_preview.items.size());
         ++i) {

        const auto& item =
            g_preview.items[i];

        if (i == g_preview.hoveredItem) {

            RECT highlight = item.rect;
            InflateRect(&highlight, 5, 5);

            COLORREF hoverColor =
                g_lightTheme
                    ? BlendColor(base, g_accentColor, 10)
                    : BlendColor(base, g_accentColor, 16);

            DrawRoundedFill(
                hdc,
                highlight,
                14,
                hoverColor);
        }

        RECT titleRect = item.rect;
        titleRect.top =
            titleRect.bottom - 26;
        titleRect.left += 7;
        titleRect.right -= 7;

        std::wstring title =
            item.title;

        if (title.size() > 72)
            title.resize(72);

        DrawTextW(
            hdc,
            title.c_str(),
            -1,
            &titleRect,
            DT_SINGLELINE |
            DT_VCENTER |
            DT_END_ELLIPSIS |
            DT_NOPREFIX);

        RECT closeRect =
            item.closeRect;

        bool closeHovered =
            i == g_preview.hoveredClose;

        const float left =
            static_cast<float>(
                closeRect.left);

        const float top =
            static_cast<float>(
                closeRect.top);

        const float width =
            static_cast<float>(
                closeRect.right -
                closeRect.left);

        const float height =
            static_cast<float>(
                closeRect.bottom -
                closeRect.top);

        // Normal: cinza. Hover: vermelho padrão de fechar.
        Gdiplus::SolidBrush closeBrush(
            closeHovered
                ? Gdiplus::Color(255, 232, 17, 35)
                : Gdiplus::Color(255, 225, 225, 225));

        graphics.FillEllipse(
            &closeBrush,
            Gdiplus::RectF(
                left,
                top,
                width,
                height));

        // X sempre visível e também antialiasado.
        Gdiplus::Pen closePen(
            closeHovered
                ? Gdiplus::Color(255, 255, 255, 255)
                : Gdiplus::Color(255, 45, 45, 45),
            1.25f);

        closePen.SetStartCap(
            Gdiplus::LineCapRound);

        closePen.SetEndCap(
            Gdiplus::LineCapRound);

        float cx =
            left + width / 2.0f;

        float cy =
            top + height / 2.0f;

        float arm =
            std::max(
                2.0f,
                std::min(width, height) * 0.22f);

        graphics.DrawLine(
            &closePen,
            cx - arm,
            cy - arm,
            cx + arm,
            cy + arm);

        graphics.DrawLine(
            &closePen,
            cx + arm,
            cy - arm,
            cx - arm,
            cy + arm);
    }

    SelectObject(
        hdc,
        oldFont);
}

static void UpdatePreviewThumbnails() {

    if (!g_preview.hwnd || !g_preview.visible)
        return;

    for (auto& item : g_preview.items) {

        if (!item.thumbnail)
            continue;

        SIZE source = {};
        if (FAILED(DwmQueryThumbnailSourceSize(
                item.thumbnail,
                &source))) {
            continue;
        }

        RECT area = item.rect;
        area.left += 7;
        area.right -= 7;
        area.top += 7;
        area.bottom -= 33;

        int areaW = area.right - area.left;
        int areaH = area.bottom - area.top;

        if (areaW <= 0 || areaH <= 0 ||
            source.cx <= 0 || source.cy <= 0) {
            continue;
        }

        double scaleX =
            static_cast<double>(areaW) /
            static_cast<double>(source.cx);

        double scaleY =
            static_cast<double>(areaH) /
            static_cast<double>(source.cy);

        double scale = std::min(scaleX, scaleY);

        int drawW =
            static_cast<int>(source.cx * scale);

        int drawH =
            static_cast<int>(source.cy * scale);

        int x = area.left + (areaW - drawW) / 2;
        int y = area.top + (areaH - drawH) / 2;

        DWM_THUMBNAIL_PROPERTIES props = {};
        props.dwFlags =
            DWM_TNP_RECTDESTINATION |
            DWM_TNP_VISIBLE |
            DWM_TNP_OPACITY |
            DWM_TNP_SOURCECLIENTAREAONLY;

        props.rcDestination = {
            x,
            y,
            x + drawW,
            y + drawH
        };

        props.opacity = 255;
        props.fVisible = TRUE;
        props.fSourceClientAreaOnly = FALSE;

        DwmUpdateThumbnailProperties(
            item.thumbnail,
            &props);
    }
}


static bool EnsurePreviewWindow() {

    if (g_preview.hwnd && IsWindow(g_preview.hwnd))
        return true;

    if (!g_moduleInstance)
        return false;

    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW |
        WS_EX_TOPMOST |
        WS_EX_NOACTIVATE,
        g_previewClassName,
        L"Bottom App Dock Preview",
        WS_POPUP,
        0,
        0,
        300,
        190,
        nullptr,
        nullptr,
        g_moduleInstance,
        nullptr);

    if (!hwnd)
        return false;

    g_preview.hwnd = hwnd;
    ApplyPremiumDwm(hwnd);
    return true;
}


static void ShowPreviewForApp(
    DockInstance& dock,
    int index) {

    if (index < 0 ||
        index >= static_cast<int>(g_apps.size())) {
        return;
    }

    AppGroup& app = g_apps[index];

    if (!app.running || app.windows.empty()) {
        HidePreview();
        return;
    }

    if (g_preview.visible &&
        g_preview.ownerDock == dock.hwnd &&
        PathEquals(g_preview.appPath, app.path)) {

        g_preview.hideDeadline = 0;
        return;
    }

    if (!EnsurePreviewWindow())
        return;

    ClearPreviewItems();

    g_preview.ownerDock = dock.hwnd;
    g_preview.appPath = app.path;
    g_preview.hideDeadline = 0;

    for (const auto& window : app.windows) {

        if (!window.hwnd || !IsWindow(window.hwnd))
            continue;

        PreviewItem item;
        item.target = window.hwnd;
        item.title = window.title.empty()
            ? app.title
            : window.title;

        if (FAILED(DwmRegisterThumbnail(
                g_preview.hwnd,
                window.hwnd,
                &item.thumbnail))) {

            item.thumbnail = nullptr;
        }

        g_preview.items.push_back(std::move(item));
    }

    if (g_preview.items.empty()) {
        HidePreview();
        return;
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(dock.monitor, &mi)) {
        HidePreview();
        return;
    }

    RECT dockRect = {};
    GetWindowRect(dock.hwnd, &dockRect);

    RECT iconRect = app.rect;
    OffsetRect(&iconRect, dockRect.left, dockRect.top);

    const int count =
        static_cast<int>(g_preview.items.size());

    // Margem física invisível ao redor do cartão.
    // Ela permite que o botão de fechar fique parcialmente para fora.
    const int outerTop = 8;
    const int outerSide = 8;

    const int padding = 10;
    const int gap = 8;
    const int itemHeight = 160;

    int maxPopupWidth =
        (mi.rcWork.right - mi.rcWork.left) - 24;

    int itemWidth = 220;

    int desiredWidth =
        outerSide * 2 +
        padding * 2 +
        count * itemWidth +
        (count - 1) * gap;

    if (desiredWidth > maxPopupWidth) {
        itemWidth =
            (maxPopupWidth -
             outerSide * 2 -
             padding * 2 -
             (count - 1) * gap) /
            count;

        if (itemWidth < 118)
            itemWidth = 118;
    }

    int popupWidth =
        outerSide * 2 +
        padding * 2 +
        count * itemWidth +
        (count - 1) * gap;

    if (popupWidth > maxPopupWidth)
        popupWidth = maxPopupWidth;

    int popupHeight =
        outerTop +
        padding * 2 +
        itemHeight;

    int centerX =
        (iconRect.left + iconRect.right) / 2;

    int x = centerX - popupWidth / 2;
    int y = dockRect.top - popupHeight - 10;

    if (x < mi.rcWork.left + 8)
        x = mi.rcWork.left + 8;

    if (x + popupWidth > mi.rcWork.right - 8)
        x = mi.rcWork.right - 8 - popupWidth;

    if (y < mi.rcWork.top + 8)
        y = mi.rcWork.top + 8;

    int left =
        outerSide + padding;

    for (auto& item :
         g_preview.items) {

        item.rect = {
            left,
            outerTop + padding,
            left + itemWidth,
            outerTop + padding + itemHeight
        };

        // 12x12 com o centro exatamente na quina:
        // 6 px dentro e 6 px fora do cartão.
        item.closeRect = {
            item.rect.right - 8,
            item.rect.top - 8,
            item.rect.right + 8,
            item.rect.top + 8
        };

        left +=
            itemWidth + gap;
    }

    SetWindowPos(
        g_preview.hwnd,
        HWND_TOPMOST,
        x,
        y,
        popupWidth,
        popupHeight,
        SWP_NOACTIVATE |
        SWP_NOOWNERZORDER);

    // Recorta a janela para deixar invisível toda a margem externa.
    ApplyPreviewRegion();

    g_preview.visible = true;
    g_keepVisibleUntil = GetTickCount64() + 500;

    ShowWindow(
        g_preview.hwnd,
        SW_SHOWNOACTIVATE);

    ApplyPremiumDwm(g_preview.hwnd);
    UpdatePreviewThumbnails();

    InvalidateRect(
        g_preview.hwnd,
        nullptr,
        TRUE);
}


static void UpdatePreviewKeepAlive() {

    if (!g_preview.visible ||
        !g_preview.hwnd ||
        !g_preview.ownerDock) {
        return;
    }

    DockInstance* dock =
        FindDockInstance(g_preview.ownerDock);

    if (!dock) {
        HidePreview();
        return;
    }

    POINT cursor = {};
    if (!GetCursorPos(&cursor))
        return;

    RECT previewRect = {};
    GetWindowRect(g_preview.hwnd, &previewRect);
    InflateRect(&previewRect, 6, 8);

    bool insidePreview =
        PtInRect(&previewRect, cursor);

    bool insideOwnerIcon = false;

    if (dock->hoveredIndex >= 0 &&
        dock->hoveredIndex <
            static_cast<int>(g_apps.size()) &&
        PathEquals(
            g_apps[dock->hoveredIndex].path,
            g_preview.appPath)) {

        RECT dockRect = {};
        GetWindowRect(dock->hwnd, &dockRect);

        RECT iconRect =
            g_apps[dock->hoveredIndex].rect;

        OffsetRect(
            &iconRect,
            dockRect.left,
            dockRect.top);

        InflateRect(&iconRect, 8, 12);
        insideOwnerIcon = PtInRect(&iconRect, cursor);
    }

    if (insidePreview || insideOwnerIcon) {
        g_preview.hideDeadline = 0;
        g_keepVisibleUntil = GetTickCount64() + 450;
        return;
    }

    ULONGLONG now = GetTickCount64();

    if (!g_preview.hideDeadline) {
        g_preview.hideDeadline =
            now + PREVIEW_HIDE_DELAY;
        return;
    }

    if (now >= g_preview.hideDeadline)
        HidePreview();
}


static LRESULT CALLBACK PreviewWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam) {

    switch (msg) {

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_MOUSEMOVE: {

            POINT point = {
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            };

            int hovered =
                HitTestPreviewItem(point);

            int hoveredClose =
                HitTestPreviewClose(point);

            if (hovered != g_preview.hoveredItem ||
                hoveredClose != g_preview.hoveredClose) {

                g_preview.hoveredItem =
                    hovered;

                g_preview.hoveredClose =
                    hoveredClose;

                InvalidateRect(
                    hwnd,
                    nullptr,
                    FALSE);
            }

            g_preview.hideDeadline = 0;
            return 0;
        }

        case WM_LBUTTONUP: {

            POINT point = {
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            };

            int closeIndex =
                HitTestPreviewClose(point);

            if (closeIndex >= 0 &&
                closeIndex <
                    static_cast<int>(
                        g_preview.items.size())) {

                HWND target =
                    g_preview.items[
                        closeIndex].target;

                HidePreview();
                CloseWindowSafe(target);

                return 0;
            }

            int index =
                HitTestPreviewItem(point);

            if (index >= 0 &&
                index < static_cast<int>(
                    g_preview.items.size())) {

                HWND target =
                    g_preview.items[index].target;

                HidePreview();
                SwitchWindowStrong(target);
            }

            return 0;
        }

        case WM_PAINT: {

            PAINTSTRUCT ps = {};
            HDC hdc = BeginPaint(hwnd, &ps);
            PaintPreview(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            ClearPreviewItems();
            g_preview.hwnd = nullptr;
            g_preview.visible = false;
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


// ============================================================
// AUTO-HIDE
// ============================================================

static void UpdateAutoHide(
    DockInstance& dock) {

    if (!dock.hwnd ||
        !dock.monitor)
        return;

    if (g_contextMenuOpen)
        return;


    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(
            dock.monitor,
            &mi))
        return;


    bool maximized =
        HasMaximizedWindowOnMonitor(
            dock.monitor);


    // Sem janela maximizada neste monitor:
    // a dock fica sempre visível.
    if (!maximized) {

        PositionDock(dock);

        if (!dock.visible) {

            ShowWindow(
                dock.hwnd,
                SW_SHOWNOACTIVATE);

            SetWindowPos(
                dock.hwnd,
                HWND_TOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE |
                SWP_NOSIZE |
                SWP_NOACTIVATE);

            dock.visible = true;
        }

        dock.outsideSince = 0;
        return;
    }


    // Com janela maximizada:
    // auto-hide independente neste monitor.
    POINT cursor = {};

    if (!GetCursorPos(
            &cursor))
        return;


    ULONGLONG now =
        GetTickCount64();


    bool edge =
        cursor.x >= mi.rcMonitor.left &&
        cursor.x < mi.rcMonitor.right &&
        cursor.y >=
            mi.rcMonitor.bottom -
            g_settings.activationZone &&
        cursor.y < mi.rcMonitor.bottom;


    if (!dock.visible) {

        if (edge) {

            PositionDock(dock);

            ShowWindow(
                dock.hwnd,
                SW_SHOWNOACTIVATE);

            SetWindowPos(
                dock.hwnd,
                HWND_TOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE |
                SWP_NOSIZE |
                SWP_NOACTIVATE);

            dock.visible = true;
            dock.outsideSince = 0;
        }

        return;
    }


    if (now <
        g_keepVisibleUntil)
        return;


    RECT rect = {};

    GetWindowRect(
        dock.hwnd,
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

        dock.outsideSince = 0;
        return;
    }


    if (!dock.outsideSince) {

        dock.outsideSince = now;
        return;
    }


    if (now -
            dock.outsideSince >=
        static_cast<ULONGLONG>(
            g_settings.hideDelay)) {

        dock.hoveredIndex = -1;

        ShowWindow(
            dock.hwnd,
            SW_HIDE);

        dock.visible = false;
        dock.outsideSince = 0;
    }
}


// ============================================================
// MENU
// ============================================================

static void ShowContextMenu(
    HWND hwnd,
    int index,
    POINT screenPoint) {

    HidePreview();

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
        hwnd,
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

        MinimizeWindowPreservingState(
            app.windows[0].hwnd);

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


        DockInstance* dock =
            FindDockInstance(hwnd);

        bool hovered =
            dock &&
            i ==
            dock->hoveredIndex;


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
                ? RGB(225, 225, 225)
                : RGB(45, 45, 48);


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
           // if (hovered)
             //   drawY -= 2;


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

    // ========================================================
    // LIXEIRA FIXA
    // ========================================================

    DockInstance* dock =
        FindDockInstance(hwnd);

    bool recycleHovered =
        dock &&
        dock->hoveredIndex ==
            HIT_RECYCLE_BIN;

    if (!g_apps.empty() &&
        g_recycleSeparatorX >= 0) {

        RECT separator = {
            g_recycleSeparatorX,
            9,
            g_recycleSeparatorX + 1,
            g_settings.dockHeight - 9
        };

        COLORREF separatorColor =
            g_lightTheme
                ? RGB(205, 205, 205)
                : RGB(70, 70, 74);

        HBRUSH separatorBrush =
            CreateSolidBrush(
                separatorColor);

        FillRect(
            hdc,
            &separator,
            separatorBrush);

        DeleteObject(
            separatorBrush);
    }

    if (recycleHovered &&
        g_settings.hoverHighlight) {

        RECT hoverRect =
            g_recycleRect;

        InflateRect(
            &hoverRect,
            5,
            5);

        COLORREF hoverColor =
            g_lightTheme
                ? RGB(225, 225, 225)
                : RGB(45, 45, 48);

        DrawRoundedFill(
            hdc,
            hoverRect,
            14,
            hoverColor);
    }

    if (g_recycleIcon) {

        DrawIconEx(
            hdc,
            g_recycleRect.left,
            g_recycleRect.top,
            g_recycleIcon,
            g_settings.iconSize,
            g_settings.iconSize,
            0,
            nullptr,
            DI_NORMAL);
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

        if (PtInRect(
                &hitRect,
                point)) {

            return i;
        }
    }

    RECT recycleHit =
        g_recycleRect;

    InflateRect(
        &recycleHit,
        3,
        3);

    if (PtInRect(
            &recycleHit,
            point)) {

        return HIT_RECYCLE_BIN;
    }

    return -1;
}

static void UpdateHover(
    HWND hwnd,
    LPARAM lParam) {

    DockInstance* dock =
        FindDockInstance(hwnd);

    if (!dock)
        return;


    if (!dock->trackingMouse) {

        TRACKMOUSEEVENT tracking = {};
        tracking.cbSize = sizeof(tracking);
        tracking.dwFlags = TME_LEAVE;
        tracking.hwndTrack = hwnd;

        if (TrackMouseEvent(
                &tracking)) {

            dock->trackingMouse = true;
        }
    }


    POINT point = {
        GET_X_LPARAM(lParam),
        GET_Y_LPARAM(lParam)
    };


    int newHovered =
        HitTestApp(point);


    if (newHovered !=
        dock->hoveredIndex) {

        dock->hoveredIndex =
            newHovered;

        dock->hoverSince =
            newHovered >= 0
                ? GetTickCount64()
                : 0;

        if (newHovered == HIT_RECYCLE_BIN &&
            g_preview.visible) {

            HidePreview();

        } else if (newHovered >= 0 &&
                   g_preview.visible &&
                   !PathEquals(
                       g_preview.appPath,
                       g_apps[newHovered].path)) {

            HidePreview();
        }

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

            DockInstance* dock =
                FindDockInstance(hwnd);

            if (dock) {

                dock->trackingMouse = false;

                if (dock->hoveredIndex != -1) {

                    dock->hoveredIndex = -1;
                    dock->hoverSince = 0;

                    if (g_preview.visible &&
                        g_preview.ownerDock == hwnd &&
                        !g_preview.hideDeadline) {

                        g_preview.hideDeadline =
                            GetTickCount64() +
                            PREVIEW_HIDE_DELAY;
                    }

                    InvalidateRect(
                        hwnd,
                        nullptr,
                        FALSE);
                }
            }

            return 0;
        }


        case WM_TIMER: {

            if (wParam ==
                MAIN_TIMER) {


                DockInstance* dock =
                    FindDockInstance(hwnd);

                if (dock) {
                    UpdateAutoHide(
                        *dock);

                    if (dock->hoveredIndex >= 0 &&
                        dock->hoveredIndex <
                            static_cast<int>(g_apps.size()) &&
                        dock->hoverSince &&
                        GetTickCount64() - dock->hoverSince >=
                            PREVIEW_HOVER_DELAY) {

                        ShowPreviewForApp(
                            *dock,
                            dock->hoveredIndex);
                    }
                }

                UpdatePreviewKeepAlive();


                ULONGLONG now =
                    GetTickCount64();


                // Atualiza o estado da Lixeira fora do WM_PAINT.
                // Consulta no máximo uma vez por segundo e redesenha
                // somente se o estado vazio/cheio realmente mudou.
                if (now -
                        g_lastRecycleRefresh >=
                    1000) {

                    g_lastRecycleRefresh =
                        now;

                    if (RefreshRecycleBinIcon()) {

                        for (const auto& dockItem :
                             g_dockInstances) {

                            if (dockItem.hwnd) {
                                InvalidateRect(
                                    dockItem.hwnd,
                                    &g_recycleRect,
                                    FALSE);
                            }
                        }
                    }
                }


                if (now -
                        g_lastRefresh >=
                    800) {


                    g_lastRefresh =
                        now;


                    EnsureDockInstancesForCurrentMonitors();
                    LoadWindowsTheme();


                    for (auto& dockItem :
                         g_dockInstances) {

                        if (dockItem.hwnd) {
                            ApplyPremiumDwm(
                                dockItem.hwnd);
                        }
                    }


                    RefreshDock();
                }
            }


            return 0;
        }


        case WM_DISPLAYCHANGE: {

            HidePreview();
            EnsureDockInstancesForCurrentMonitors();
            RefreshDock();
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


            // Sem alpha global: mantém ícones 100% opacos.
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


            if (index == HIT_RECYCLE_BIN) {

                HidePreview();
                OpenRecycleBin();

            } else if (index >= 0) {

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


            if (index == HIT_RECYCLE_BIN) {

                POINT screen =
                    point;

                ClientToScreen(
                    hwnd,
                    &screen);

                ShowRecycleContextMenu(
                    hwnd,
                    screen);

            } else if (index >= 0) {


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

            DockInstance* dock =
                FindDockInstance(hwnd);

            if (dock) {
                dock->hwnd = nullptr;
                dock->visible = false;
                dock->hoveredIndex = -1;
            }

            bool anyWindow = false;

            for (const auto& dockItem :
                 g_dockInstances) {

                if (dockItem.hwnd) {
                    anyWindow = true;
                    break;
                }
            }

            if (!anyWindow) {

                ClearApps();
                ClearEnumeratedWindows();
                PostQuitMessage(0);
            }

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

static BOOL CALLBACK EnumMonitorsProc(
    HMONITOR monitor,
    HDC,
    LPRECT,
    LPARAM lParam) {

    auto* monitors =
        reinterpret_cast<
            std::vector<HMONITOR>*>(
                lParam);

    monitors->push_back(monitor);
    return TRUE;
}


static std::vector<HMONITOR> GetCurrentMonitors() {

    std::vector<HMONITOR> monitors;

    EnumDisplayMonitors(
        nullptr,
        nullptr,
        EnumMonitorsProc,
        reinterpret_cast<LPARAM>(&monitors));

    if (monitors.empty()) {

        HMONITOR primary =
            MonitorFromPoint(
                POINT{0, 0},
                MONITOR_DEFAULTTOPRIMARY);

        if (primary)
            monitors.push_back(primary);
    }

    return monitors;
}


static bool MonitorStillExists(
    HMONITOR monitor,
    const std::vector<HMONITOR>& current) {

    return std::find(
        current.begin(),
        current.end(),
        monitor) != current.end();
}


static HWND CreateDockWindowForMonitor(
    HMONITOR monitor) {

    if (!g_moduleInstance || !monitor)
        return nullptr;

    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW |
        WS_EX_TOPMOST |
        WS_EX_NOACTIVATE,
        g_dockClassName,
        L"Bottom App Dock",
        WS_POPUP,
        0,
        0,
        100,
        g_settings.dockHeight,
        nullptr,
        nullptr,
        g_moduleInstance,
        nullptr);

    if (!hwnd)
        return nullptr;

    DockInstance dock;
    dock.hwnd = hwnd;
    dock.monitor = monitor;

    g_dockInstances.push_back(dock);

    // Não usamos alpha global na janela da dock, porque isso deixa
    // os próprios ícones transparentes. A transparência visual fica
    // por conta do material DWM.
    ApplyPremiumDwm(hwnd);

    // Posiciona imediatamente no monitor de destino.
    PositionDock(
        g_dockInstances.back());

    ShowWindow(hwnd, SW_HIDE);

    return hwnd;
}


static void EnsureDockInstancesForCurrentMonitors() {

    const auto monitors = GetCurrentMonitors();

    // Primeiro cria docks faltantes. Assim nunca ficamos sem nenhuma
    // janela durante uma troca completa de topologia/handles de monitor.
    for (HMONITOR monitor : monitors) {

        bool exists = false;

        for (const auto& dock : g_dockInstances) {
            if (dock.hwnd && dock.monitor == monitor) {
                exists = true;
                break;
            }
        }

        if (!exists)
            CreateDockWindowForMonitor(monitor);
    }

    std::vector<HWND> staleWindows;

    for (const auto& dock : g_dockInstances) {

        if (dock.hwnd &&
            !MonitorStillExists(dock.monitor, monitors)) {

            staleWindows.push_back(dock.hwnd);
        }
    }

    for (HWND hwnd : staleWindows) {

        if (g_preview.visible &&
            g_preview.ownerDock == hwnd) {
            HidePreview();
        }

        if (IsWindow(hwnd))
            DestroyWindow(hwnd);
    }

    g_dockInstances.erase(
        std::remove_if(
            g_dockInstances.begin(),
            g_dockInstances.end(),
            [](const DockInstance& dock) {
                return !dock.hwnd || !IsWindow(dock.hwnd);
            }),
        g_dockInstances.end());

    // Não reassociar pelo MonitorFromWindow aqui.
    // Uma dock recém-criada ainda está em (0,0), então o Windows a
    // classificaria como pertencente ao monitor principal antes de
    // PositionDock posicioná-la no monitor correto. O HMONITOR salvo
    // na criação já é a fonte de verdade.
}


static DWORD WINAPI DockThreadProc(
    LPVOID) {

    g_moduleInstance =
        GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DockWndProc;
    wc.hInstance = g_moduleInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = g_dockClassName;
    wc.hbrBackground = nullptr;

    if (!RegisterClassExW(&wc))
        return 0;

    WNDCLASSEXW previewClass = {};
    previewClass.cbSize = sizeof(previewClass);
    previewClass.lpfnWndProc = PreviewWndProc;
    previewClass.hInstance = g_moduleInstance;
    previewClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    previewClass.lpszClassName = g_previewClassName;
    previewClass.hbrBackground = nullptr;

    if (!RegisterClassExW(&previewClass)) {
        UnregisterClassW(g_dockClassName, g_moduleInstance);
        return 0;
    }

    LoadPinnedApps();
    LoadWindowsTheme();

    g_dockInstances.clear();
    EnsureDockInstancesForCurrentMonitors();

    if (g_dockInstances.empty()) {
        UnregisterClassW(g_previewClassName, g_moduleInstance);
        UnregisterClassW(g_dockClassName, g_moduleInstance);
        return 0;
    }

    RefreshDock();
    g_lastRefresh = GetTickCount64();

    MSG msg = {};

    while (GetMessageW(
               &msg,
               nullptr,
               0,
               0) > 0) {

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    HidePreview();

    if (g_preview.hwnd && IsWindow(g_preview.hwnd))
        DestroyWindow(g_preview.hwnd);

    UnregisterClassW(g_previewClassName, g_moduleInstance);
    UnregisterClassW(g_dockClassName, g_moduleInstance);

    if (g_recycleIcon) {
        DestroyIcon(g_recycleIcon);
        g_recycleIcon = nullptr;
    }

    g_moduleInstance = nullptr;
    return 0;
}


// ============================================================
// WINDHAWK
// ============================================================

BOOL Wh_ModInit() {

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;

    if (Gdiplus::GdiplusStartup(
            &g_gdiplusToken,
            &gdiplusStartupInput,
            nullptr) != Gdiplus::Ok) {

        g_gdiplusToken = 0;
    }



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

    if (g_preview.hwnd) {
        PostMessageW(
            g_preview.hwnd,
            WM_CLOSE,
            0,
            0);
    }

    for (const auto& dock :
         g_dockInstances) {

        if (dock.hwnd) {

            PostMessageW(
                dock.hwnd,
                WM_CLOSE,
                0,
                0);
        }
    }


    if (g_thread) {

        WaitForSingleObject(
            g_thread,
            3000);

        CloseHandle(
            g_thread);

        g_thread = nullptr;
    }

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(
            g_gdiplusToken);

        g_gdiplusToken = 0;
    }

}


void Wh_ModSettingsChanged() {

    LoadSettings();

    for (const auto& dock :
         g_dockInstances) {

        if (dock.hwnd) {

            PostMessageW(
                dock.hwnd,
                WM_DOCK_SETTINGS_CHANGED,
                0,
                0);
        }
    }
}
