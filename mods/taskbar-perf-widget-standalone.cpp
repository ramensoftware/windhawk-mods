// ==WindhawkMod==
// @id              taskbar-perf-widget-standalone
// @name            Taskbar Performance Widget
// @description     Módulo avanzado con renderizado Acrílico, centrado perfecto y cursor nativo.
// @version         9.3
// @author          Agarciao10
// @include         explorer.exe
// @compilerOptions -lpdh -lole32 -ldwmapi -lgdi32 -luser32 -lshcore -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Performance Widget

Un widget de monitorización de rendimiento de alta precisión, diseñado para
integrarse de forma nativa y simétrica en la barra de tareas de Windows 11.

## Características principales
- **Monitorización en tiempo real:** Visualización instantánea de CPU, Memoria
(RAM), Red (Bajada/Subida) y GPU.
- **Arquitectura Aislada:** Utiliza un proceso independiente para garantizar la
estabilidad total del explorador (`explorer.exe`).
- **Diseño Nativo:** Integración visual mediante los materiales oficiales de
Windows 11 (Acrílico y Mica).
- **Altamente Personalizable:** Ajusta la geometría, materiales de fondo y
visibilidad de indicadores desde la interfaz de usuario.
- **Centrado Geométrico:** Algoritmo avanzado de renderizado vectorial que
garantiza una disposición simétrica y elegante.

## Configuración
- **Material de Fondo:** Elige entre el efecto transparente para una integración
invisible, o aplica materiales nativos para un aspecto tipo Fluent Design.
- **Geometría:** Ajusta la altura y el ancho por columna para adaptar el widget
a cualquier escala de interfaz o densidad de barra de tareas.
- **Visibilidad:** Activa solo los indicadores necesarios para optimizar el
espacio disponible.

## Notas Técnicas
- El widget utiliza la interfaz de datos de rendimiento (PDH) para obtener
telemetría de hardware de bajo impacto.
- El renderizado vectorial está optimizado mediante GDI+ para eliminar el
parpadeo visual y mantener la nitidez en monitores de alta densidad (HiDPI).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showCpu: true
  $name: "Mostrar CPU"
- showRam: true
  $name: "Mostrar Memoria (RAM)"
- showNetRecv: true
  $name: "Mostrar Red (Descarga)"
- showNetSend: true
  $name: "Mostrar Red (Subida)"
- showGpu: true
  $name: "Mostrar GPU"
- offsetRight: 150
  $name: "Desplazamiento Horizontal (Offset)"
  $description: "Distancia en píxeles desde el borde derecho."
- height: 48
  $name: "Altura del Módulo"
  $description: "Altura total en píxeles del recuadro."
- colWidth: 84
  $name: "Ancho por Indicador"
  $description: "Espacio horizontal asignado a cada métrica activa."
- bgOpacity: 64
  $name: "Opacidad del Acrílico (Tinte)"
  $description: "Controla la densidad del fondo cuando el Tema Automático está
desactivado."
- autoTheme: true
  $name: "Tema Automático"
  $description: "Adapta el color de la tipografía y el tinte del fondo al modo
del sistema."
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <gdiplus.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <windhawk_api.h>
#include <windows.h>
#include <cwchar>
#include <mutex>
#include <string>
#include <thread>

using namespace Gdiplus;

// --- DWM y Materiales No Documentados ---
typedef enum _WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
} WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;

typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;

typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef BOOL(
    WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

// --- Parámetros Estructurales Dinámicos ---
struct ModSettings {
    int colWidth = 84;
    int height = 48;
    int offsetRight = 150;
    bool showCpu = true;
    bool showRam = true;
    bool showNetRecv = true;
    bool showNetSend = true;
    bool showGpu = true;
    int bgOpacity = 64;
    bool autoTheme = true;
} g_Settings;

// --- Estado Global ---
HWND g_hWidgetWnd = NULL;
bool g_Running = true;
HWINEVENTHOOK g_TaskbarHook = nullptr;
UINT g_TaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

PDH_HQUERY pdhQuery = NULL;
PDH_HCOUNTER pdhCpuTotal = NULL;
PDH_HCOUNTER pdhRamUsage = NULL;
PDH_HCOUNTER pdhNetRecv = NULL;
PDH_HCOUNTER pdhNetSend = NULL;
PDH_HCOUNTER pdhGpuUsage = NULL;

double currentCpu = 0.0;
double currentRam = 0.0;
double currentNetRecv = 0.0;
double currentNetSend = 0.0;
double currentGpu = 0.0;

// --- Detección de Tema ---
bool IsSystemLightMode() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Pe"
                     L"rsonalize",
                     L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

// --- Integración Exacta del Material Multimedia ---
void UpdateAppearance(HWND hwnd) {
    if (!hwnd)
        return;

    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                          sizeof(preference));

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(
            hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            DWORD tint = 0;
            if (g_Settings.autoTheme) {
                tint = IsSystemLightMode() ? 0x40FFFFFF : 0x40000000;
            } else {
                tint = (g_Settings.bgOpacity << 24) | (0xFFFFFF);
            }
            ACCENT_POLICY policy = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint,
                                    0};
            WINDOWCOMPOSITIONATTRIBDATA data = {WCA_ACCENT_POLICY, &policy,
                                                sizeof(ACCENT_POLICY)};
            SetComp(hwnd, &data);
        }
    }
}

// --- Lógica Matemática PDH ---
void LoadSettings() {
    g_Settings.showCpu = Wh_GetIntSetting(L"showCpu") != 0;
    g_Settings.showRam = Wh_GetIntSetting(L"showRam") != 0;
    g_Settings.showNetRecv = Wh_GetIntSetting(L"showNetRecv") != 0;
    g_Settings.showNetSend = Wh_GetIntSetting(L"showNetSend") != 0;
    g_Settings.showGpu = Wh_GetIntSetting(L"showGpu") != 0;
    g_Settings.offsetRight = Wh_GetIntSetting(L"offsetRight");

    g_Settings.height = Wh_GetIntSetting(L"height");
    if (g_Settings.height < 20)
        g_Settings.height = 48;

    g_Settings.colWidth = Wh_GetIntSetting(L"colWidth");
    if (g_Settings.colWidth < 50)
        g_Settings.colWidth = 84;

    g_Settings.bgOpacity = Wh_GetIntSetting(L"bgOpacity");
    if (g_Settings.bgOpacity < 0)
        g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255)
        g_Settings.bgOpacity = 255;

    g_Settings.autoTheme = Wh_GetIntSetting(L"autoTheme") != 0;
}

int GetActiveColumns() {
    return (g_Settings.showCpu ? 1 : 0) + (g_Settings.showRam ? 1 : 0) +
           (g_Settings.showNetRecv ? 1 : 0) + (g_Settings.showNetSend ? 1 : 0) +
           (g_Settings.showGpu ? 1 : 0);
}

double GetTotalCounterSum(PDH_HCOUNTER counter) {
    if (!counter)
        return 0.0;
    DWORD bufferSize = 0, itemCount = 0;
    PdhGetFormattedCounterArray(counter, PDH_FMT_DOUBLE, &bufferSize,
                                &itemCount, NULL);

    if (bufferSize > 0) {
        PDH_FMT_COUNTERVALUE_ITEM* items =
            (PDH_FMT_COUNTERVALUE_ITEM*)malloc(bufferSize);
        if (items) {
            if (PdhGetFormattedCounterArray(counter, PDH_FMT_DOUBLE,
                                            &bufferSize, &itemCount,
                                            items) == ERROR_SUCCESS) {
                double sum = 0.0;
                for (DWORD i = 0; i < itemCount; i++)
                    sum += items[i].FmtValue.doubleValue;
                free(items);
                return sum;
            }
            free(items);
        }
    }
    return 0.0;
}

void InitCounters() {
    if (PdhOpenQuery(NULL, NULL, &pdhQuery) != ERROR_SUCCESS)
        return;
    PdhAddEnglishCounter(pdhQuery, L"\\Processor(_Total)\\% Processor Time",
                         NULL, &pdhCpuTotal);
    PdhAddEnglishCounter(pdhQuery, L"\\Memory\\% Committed Bytes In Use", NULL,
                         &pdhRamUsage);
    PdhAddEnglishCounter(pdhQuery,
                         L"\\Network Interface(*)\\Bytes Received/sec", NULL,
                         &pdhNetRecv);
    PdhAddEnglishCounter(pdhQuery, L"\\Network Interface(*)\\Bytes Sent/sec",
                         NULL, &pdhNetSend);
    PdhAddEnglishCounter(pdhQuery, L"\\GPU Engine(*)\\Utilization Percentage",
                         NULL, &pdhGpuUsage);
    PdhCollectQueryData(pdhQuery);
}

void UpdateMetrics() {
    if (!pdhQuery)
        return;
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(pdhQuery);

    if (PdhGetFormattedCounterValue(pdhCpuTotal, PDH_FMT_DOUBLE, NULL,
                                    &counterVal) == ERROR_SUCCESS)
        currentCpu = counterVal.doubleValue;
    if (PdhGetFormattedCounterValue(pdhRamUsage, PDH_FMT_DOUBLE, NULL,
                                    &counterVal) == ERROR_SUCCESS)
        currentRam = counterVal.doubleValue;

    currentNetRecv = GetTotalCounterSum(pdhNetRecv);
    currentNetSend = GetTotalCounterSum(pdhNetSend);
    currentGpu = GetTotalCounterSum(pdhGpuUsage);
    if (currentGpu > 100.0)
        currentGpu = 100.0;
}

void FormatNetSpeed(double bytesSec, wchar_t* buffer, size_t maxLen) {
    double kbps = (bytesSec * 8.0) / 1000.0;
    if (kbps >= 1000.0)
        swprintf(buffer, maxLen, L"%.1f Mbps", kbps / 1000.0);
    else
        swprintf(buffer, maxLen, L"%.1f Kbps", kbps);
}

// --- Renderizado Vectorial GDI+ Centrado ---
void DrawPerformancePanel(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

    graphics.Clear(Color(0, 0, 0, 0));

    FontFamily iconFamily(L"Segoe MDL2 Assets", nullptr);
    Font iconFont(&iconFamily, 15, FontStyleRegular, UnitPixel);

    FontFamily textFamily(L"Segoe UI", nullptr);
    Font valFont(&textFamily, 13, FontStyleBold, UnitPixel);
    Font lblFont(&textFamily, 10, FontStyleRegular, UnitPixel);

    bool isLightMode = g_Settings.autoTheme ? IsSystemLightMode() : false;
    Color primaryTextColor =
        isLightMode ? Color(255, 0, 0, 0) : Color(255, 255, 255, 255);
    Color secondaryTextColor =
        isLightMode ? Color(180, 0, 0, 0) : Color(180, 255, 255, 255);

    SolidBrush iconBrush(primaryTextColor);
    SolidBrush valBrush(primaryTextColor);
    SolidBrush lblBrush(secondaryTextColor);

    StringFormat format;
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentNear);

    int currentIndex = 0;
    float centerY = (float)height / 2.0f;
    float iconY = centerY - 8.0f;
    float valY = centerY - 16.0f;
    float lblY = centerY;

    auto drawMetric = [&](const wchar_t* icon, const wchar_t* value,
                          const wchar_t* label) {
        float colCenter =
            (currentIndex * g_Settings.colWidth) + (g_Settings.colWidth / 2.0f);
        float baseX = colCenter - 28.0f;

        PointF iconPos(baseX, iconY);
        PointF valPos(baseX + 24.0f, valY);
        PointF lblPos(baseX + 24.0f, lblY);

        graphics.DrawString(icon, -1, &iconFont, iconPos, &format, &iconBrush);
        graphics.DrawString(value, -1, &valFont, valPos, &format, &valBrush);
        graphics.DrawString(label, -1, &lblFont, lblPos, &format, &lblBrush);
        currentIndex++;
    };

    if (g_Settings.showCpu) {
        wchar_t cpuStr[16];
        swprintf(cpuStr, 16, L"%d%%", (int)currentCpu);
        drawMetric(L"\xE9D9", cpuStr, L"CPU");
    }
    if (g_Settings.showRam) {
        wchar_t ramStr[16];
        swprintf(ramStr, 16, L"%d%%", (int)currentRam);
        drawMetric(L"\xE7F4", ramStr, L"Memory");
    }
    if (g_Settings.showNetRecv) {
        wchar_t recvStr[32];
        FormatNetSpeed(currentNetRecv, recvStr, 32);
        drawMetric(L"\xEC27", recvStr, L"Receive \x2193");
    }
    if (g_Settings.showNetSend) {
        wchar_t sendStr[32];
        FormatNetSpeed(currentNetSend, sendStr, 32);
        drawMetric(L"\xEC27", sendStr, L"Send \x2191");
    }
    if (g_Settings.showGpu) {
        wchar_t gpuStr[16];
        swprintf(gpuStr, 16, L"%d%%", (int)currentGpu);
        drawMetric(L"\xE7F4", gpuStr, L"GPU");
    }
}

// --- Subsistemas de Ventana e Interceptación ---
bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd)
        return false;
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

void CALLBACK TaskbarEventProc(HWINEVENTHOOK,
                               DWORD event,
                               HWND hwnd,
                               LONG,
                               LONG,
                               DWORD,
                               DWORD) {
    if (!IsTaskbarWindow(hwnd) || !g_hWidgetWnd)
        return;
    PostMessage(g_hWidgetWnd, WM_APP + 10, 0, 0);
}

void RegisterTaskbarHook(HWND hwnd) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        DWORD pid = 0, tid = GetWindowThreadProcessId(hTaskbar, &pid);
        if (tid != 0)
            g_TaskbarHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, TaskbarEventProc, pid, tid,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    }
    PostMessage(hwnd, WM_APP + 10, 0, 0);
}

#define IDT_POLL_HARDWARE 1001
#define APP_WM_CLOSE WM_APP

LRESULT CALLBACK WidgetWndProc(HWND hwnd,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            InitCounters();
            UpdateAppearance(hwnd);
            SetTimer(hwnd, IDT_POLL_HARDWARE, 1000, NULL);
            RegisterTaskbarHook(hwnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        case WM_DESTROY:
            if (g_TaskbarHook)
                UnhookWinEvent(g_TaskbarHook);
            if (pdhQuery)
                PdhCloseQuery(pdhQuery);
            PostQuitMessage(0);
            return 0;
        case WM_TIMER:
            if (wParam == IDT_POLL_HARDWARE) {
                UpdateMetrics();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        case WM_APP + 10: {
            HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
            if (!hTaskbar)
                break;

            int activeCols = GetActiveColumns();
            if (activeCols == 0 || !IsWindowVisible(hTaskbar)) {
                if (IsWindowVisible(hwnd))
                    ShowWindow(hwnd, SW_HIDE);
                return 0;
            } else {
                if (!IsWindowVisible(hwnd))
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }

            int currentWidth = activeCols * g_Settings.colWidth;
            RECT rc;
            GetWindowRect(hTaskbar, &rc);
            int taskbarHeight = rc.bottom - rc.top;
            int x = rc.right - currentWidth - g_Settings.offsetRight;
            int y = rc.top + (taskbarHeight / 2) - (g_Settings.height / 2);

            RECT myRc;
            GetWindowRect(hwnd, &myRc);
            if (myRc.left != x || myRc.top != y ||
                (myRc.right - myRc.left) != currentWidth ||
                (myRc.bottom - myRc.top) != g_Settings.height) {
                SetWindowPos(hwnd, HWND_TOPMOST, x, y, currentWidth,
                             g_Settings.height, SWP_NOACTIVATE);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap =
                CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

            DrawPerformancePanel(memDC, rc.right, rc.bottom);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            if (msg == g_TaskbarCreatedMsg) {
                if (g_TaskbarHook)
                    UnhookWinEvent(g_TaskbarHook);
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WidgetThread() {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WidgetWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // <-- Cursor estándar asignado
    wc.lpszClassName = TEXT("PsiNetStandalonePerfWidget");
    RegisterClass(&wc);

    int initialWidth = GetActiveColumns() * g_Settings.colWidth;

    g_hWidgetWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, wc.lpszClassName,
        TEXT("PerformanceWidget"), WS_POPUP | WS_VISIBLE, 0, 0, initialWidth,
        g_Settings.height, NULL, NULL, wc.hInstance, NULL);

    SetLayeredWindowAttributes(g_hWidgetWnd, 0, 255, LWA_ALPHA);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
}

std::thread* g_pWidgetThread = nullptr;

BOOL WhTool_ModInit() {
    LoadSettings();
    g_Running = true;
    g_pWidgetThread = new std::thread(WidgetThread);
    return TRUE;
}
void WhTool_ModUninit() {
    g_Running = false;
    if (g_hWidgetWnd)
        SendMessage(g_hWidgetWnd, APP_WM_CLOSE, 0, 0);
    if (g_pWidgetThread) {
        if (g_pWidgetThread->joinable())
            g_pWidgetThread->join();
        delete g_pWidgetThread;
        g_pWidgetThread = nullptr;
    }
}
void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hWidgetWnd) {
        UpdateAppearance(g_hWidgetWnd);
        SendMessage(g_hWidgetWnd, WM_APP + 10, 0, 0);
        InvalidateRect(g_hWidgetWnd, NULL, TRUE);
    }
}

// Subsistema de Proceso Aislado de Windhawk
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;
void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false, isToolModProcess = false,
         isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv)
        return FALSE;
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
            break;
        }
    }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0)
                isCurrentToolModProcess = true;
            break;
        }
    }
    LocalFree(argv);
    if (isService)
        return FALSE;
    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS ||
            !WhTool_ModInit())
            ExitProcess(1);
        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        void* entryPoint =
            (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolModProcess)
        return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher)
        return;
    WCHAR currentProcessPath[MAX_PATH];
    if (GetModuleFileName(nullptr, currentProcessPath,
                          ARRAYSIZE(currentProcessPath)) == 0)
        return;
    WCHAR
        commandLine[MAX_PATH + 2 +
                    (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                    1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule)
        kernelModule = GetModuleHandle(L"kernel32.dll");
    if (!kernelModule)
        return;
    using CreateProcessInternalW_t =
        BOOL(WINAPI*)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
                      LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCWSTR,
                      LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW)
        return;
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}
void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher)
        WhTool_ModSettingsChanged();
}
void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}