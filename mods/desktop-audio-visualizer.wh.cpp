// ==WindhawkMod==
// @id                 desktop-audio-visualizer
// @name               Desktop Audio Visualizer
// @description        Real-time audio visualizer on your Windows desktop with customizable appearance
// @description:ru-RU  Аудиовизуализатор в реальном времени на рабочем столе Windows с настраиваемым внешним видом
// @version            1.0.0
// @author             Salyts
// @github             https://github.com/Salyts
// @include            explorer.exe
// @architecture       x86-64
// @compilerOptions    -ldxgi -ld2d1 -ld3d11 -ldcomp -ldwmapi -lgdi32 -lshcore -lshlwapi -lole32 -lshell32 -lksuser -lwindowscodecs -lruntimeobject -lwindowsapp -luuid -luser32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Desktop Audio Visualizer 1.0.0

A real-time audio spectrum visualizer that displays on your Windows desktop. Captures system audio output and renders animated frequency bars directly on the desktop background.

![video](https://i.imgur.com/N2QScsV.gif)

## Features

### Visual Styles
* **6 Bar Shapes**:
  - **Stereo** — classic frequency spectrum bars
  - **Mountain** — center-peak, bars taper toward the edges
  - **Mirror** — bars grow from edges toward center
  - **Wave** — bars modulated by a sine wave over time
  - **Breathe** — slow breathing pulse synchronized with audio
  - **Dots** — bar columns made of stacked shapes
* **7 Color Modes**:
  - **Solid** — single flat color
  - **Static gradient** — fixed color gradient across bars
  - **Reactive gradient** — gradient shifts toward second color with audio level
  - **Windows accent color** — automatically matches your Windows accent color, updates instantly on change
  - **Album art** — flat dominant color extracted from the currently playing track's cover art
  - **Dynamic album** — gradient from primary to secondary album art color; shifts with bar position and audio level
  - **Acrylic** — fixed RGB color with amplitude-driven alpha (nearly transparent at silence, semi-opaque at full volume)
* **2 Orientations**: Horizontal (bars grow vertically) or Vertical (bars grow horizontally)
* **Vertical Anchor**: Control bar growth direction — Bottom, Top, or Middle

### Customization
* **Bar Configuration**: Count (1-1000), thickness, gap, max size, idle size, per-corner radius (`"10"` or `"10 10 0 0"`)
* **Position Control**: Freely position on any monitor with horizontal/vertical percentage coordinates
* **Background Effects**:
  - Optional background panel with padding, per-corner radius and border
  - Wallpaper blur effect behind the background
  - Custom ARGB hex colors for background and border

### Audio Processing
* **WASAPI Loopback Capture**: Captures all system audio output in real time
* **Real-time FFT**: 7-band frequency spectrum with Hann windowing
* **6 EQ Presets**: Balanced, Bass, Rock, Pop, Jazz, Electronic
* **Adjustable Sensitivity**: 0–300 range

### Performance
* **Adjustable Frame Rate**: Set any FPS value for maximum smoothness or power-saving rendering
* **Fullscreen Pause**: Detects both exclusive fullscreen (DX9/11) and borderless fullscreen windows; pauses automatically and resumes when the app closes
* **Silence Detection**: Reduces refresh rate after a configurable idle period to save CPU/GPU

---

### Credits
* **[Salyts](https://github.com/Salyts)** — Author of Desktop Audio Visualizer
* Borrowed the audio visualizer code from **[GR0UD](https://github.com/GR0UD)**.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position:
  - horizontalPosition: 50
    $name: Horizontal position (0=left, 50=center, 100=right)
    $name:ru-RU: Положение по горизонтали (0=слева, 50=центр, 100=справа)
  - verticalPosition: 90
    $name: Vertical position (0=top, 50=center, 100=bottom)
    $name:ru-RU: Положение по вертикали (0=сверху, 50=центр, 100=снизу)
  - monitor: 1
    $name: Monitor (1-based)
    $name:ru-RU: Монитор (нумерация с 1)
  $name: Position
  $name:ru-RU: Положение
- appearance:
  - shape: stereo
    $name: Bar shape
    $name:ru-RU: Форма визуализатора
    $options:
    - stereo: Stereo
    - mountain: Mountain
    - mirror: Mirror
    - wave: Wave
    - breathe: Breathe
    - dots: Dots
  - eqPreset: default
    $name: EQ preset
    $name:ru-RU: Пресет эквалайзера
    $options:
    - default: Balanced
    - bass: Bass
    - rock: Rock
    - pop: Pop
    - jazz: Jazz
    - electronic: Electronic
    $options:ru-RU:
    - default: Сбалансированный
    - bass: Бас
    - rock: Рок
    - pop: Поп
    - jazz: Джаз
    - electronic: Электроника
  - orientation: horizontal
    $name: Orientation
    $name:ru-RU: Ориентация
    $options:
    - horizontal: Horizontal
    - vertical: Vertical
    $options:ru-RU:
    - horizontal: Горизонтальная
    - vertical: Вертикальная
  - verticalAnchor: bottom
    $name: Vertical anchor
    $name:ru-RU: Вертикальная привязка
    $options:
    - top: Top
    - middle: Middle
    - bottom: Bottom
    $options:ru-RU:
    - top: Сверху
    - middle: По центру
    - bottom: Снизу
  - barCount: 20
    $name: Number of bars (1-1000)
    $name:ru-RU: Количество столбиков (1-1000)
  - barWidth: 10
    $name: Bar thickness (px)
    $name:ru-RU: Толщина столбика (пикс.)
  - barGap: 5
    $name: Gap between bars (px)
    $name:ru-RU: Промежуток между столбиками (пикс.)
  - barMaxSize: 100
    $name: Maximum bar size (px)
    $name:ru-RU: Максимальный размер столбика (пикс.)
  - barIdleSize: 10
    $name: Idle (resting) bar size (px)
    $name:ru-RU: Размер столбика в покое (пикс.)
  - barCornerRadius: "10"
    $name: Bar corner radius (px)
    $name:ru-RU: Скругление столбиков (пикс.)
    $description: >-
      Single value (e.g., "5") for uniform corners, or four space-separated values
      (e.g., "10 10 0 0") for top-left, top-right, bottom-right, bottom-left.
    $description:ru-RU: >-
      Одно значение (например, "5") для одинаковых углов, или четыре через пробел
      (например, "10 10 0 0") для верх-лево, верх-право, низ-право, низ-лево.
  - colorMode: solid
    $name: Color mode
    $name:ru-RU: Цветовой режим
    $options:
    - solid: Solid color
    - gradient: Static gradient
    - reactive_gradient: Reactive gradient
    - accent: Windows accent color
    - album_art: Album art color
    - dynamic_album: Dynamic album art color
    - acrylic: Acrylic
    $options:ru-RU:
    - solid: Сплошной цвет
    - gradient: Статичный градиент
    - reactive_gradient: Реактивный градиент
    - accent: Системный акцентный цвет Windows
    - album_art: Цвет обложки альбома
    - dynamic_album: Динамический цвет обложки
    - acrylic: Акрил
  - color: "#FFFFFFFF"
    $name: Bar color (ARGB hex)
    $name:ru-RU: Цвет столбиков (ARGB hex)
  - gradientColor1: "#FFFFFFFF"
    $name: Gradient color 1 (ARGB hex)
    $name:ru-RU: Цвет градиента 1 (ARGB hex)
  - gradientColor2: "#FF1ED760"
    $name: Gradient color 2 (ARGB hex)
    $name:ru-RU: Цвет градиента 2 (ARGB hex)
  - sensitivity: 150
    $name: Sensitivity (0-300)
    $name:ru-RU: Чувствительность (0-300)
  $name: Appearance
  $name:ru-RU: Внешний вид
- background:
  - enabled: true
    $name: Enabled
    $name:ru-RU: Включено
  - color: "#60000000"
    $name: Background color (ARGB hex)
    $name:ru-RU: Цвет фона (ARGB hex)
  - padding: 20
    $name: Padding (px)
    $name:ru-RU: Отступы (пикс.)
  - cornerRadius: "15"
    $name: Corner radius (px)
    $name:ru-RU: Скругление углов (пикс.)
    $description: >-
      Single value (e.g., "15") for uniform corners, or four space-separated values
      (e.g., "15 15 0 0") for top-left, top-right, bottom-right, bottom-left.
    $description:ru-RU: >-
      Одно значение (например, "15") для одинаковых углов, или четыре через пробел
      (например, "15 15 0 0") для верх-лево, верх-право, низ-право, низ-лево.
  - blur: 0
    $name: Wallpaper blur radius (0 = disabled)
    $name:ru-RU: Радиус размытия обоев (0 = выключено)
  - borderSize: 0
    $name: Border size (px)
    $name:ru-RU: Толщина рамки (пикс.)
  - borderColor: "#40FFFFFF"
    $name: Border color (ARGB hex)
    $name:ru-RU: Цвет рамки (ARGB hex)
  $name: Background
  $name:ru-RU: Фон
- performance:
  - targetFps: 60
    $name: Target frame rate
    $name:ru-RU: Целевая частота кадров
    $description: >-
      Higher = smoother. 60 is good for most displays, 120 for high refresh rate monitors.
    $description:ru-RU: >-
      Чем выше значение, тем плавнее изображение. 60 подходит для большинства дисплеев, 120 — для мониторов с высокой частотой обновления.
  - pauseOnFullscreen: true
    $name: Pause on fullscreen apps / games
    $name:ru-RU: Пауза в полноэкранных приложениях и играх
    $description: >-
      Stops audio capture and rendering while a fullscreen exclusive app or
      game is running, to save CPU/GPU. Resumes automatically afterwards.
    $description:ru-RU: >-
      Останавливает захват звука и отрисовку, пока запущено полноэкранное
      эксклюзивное приложение или игра, чтобы сэкономить CPU/GPU.
      Возобновляется автоматически.
  - pauseWhenSilentSeconds: 10
    $name: Slow down when silent (seconds, 0 = disabled)
    $name:ru-RU: Замедление при тишине (секунды, 0 = выключено)
    $description: >-
      After this many seconds without audio, the refresh rate is reduced to
      save resources. Instantly returns to full speed once audio is detected.
    $description:ru-RU: >-
      После стольких секунд без звука частота обновления снижается для
      экономии ресурсов. Мгновенно возвращается к полной скорости при
      появлении звука.
  $name: Performance
  $name:ru-RU: Производительность
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <wrl/client.h>
#include <wincodec.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <thread>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Storage::Streams;

using Microsoft::WRL::ComPtr;

#define TIMER_ID_MSG_DISPLAY_CHANGE 1
#define TIMER_ID_MSG_RECREATE_OVERLAY 2
#define TIMER_ID_MSG_WALLPAPER_REFRESH 3
#define TIMER_ID_MSG_FULLSCREEN_WATCH 4

#define WM_APP_CLEANUP (WM_APP + 1)
#define WM_APP_SETTINGS_CHANGED (WM_APP + 2)
#define WM_APP_RENDER_TICK (WM_APP + 3)
#define OVERLAY_WINDOW_CLASS (L"DesktopAudioVisOverlay_" WH_MOD_ID)
#define MESSAGE_WINDOW_CLASS (L"DesktopAudioVisMessage_" WH_MOD_ID)

enum class VizShape { Stereo, Mountain, Mirror, Wave, Breathe, Dots };
enum class VizColorMode { Solid, Gradient, ReactiveGradient, Accent, AlbumArt, DynamicAlbum, Acrylic };
enum class VizEQ { Default, Bass, Rock, Pop, Jazz, Electronic };
enum class VizOrientation { Horizontal, Vertical };
enum class VizAnchor { Top, Middle, Bottom };

struct Settings {
    VizShape shape = VizShape::Stereo;
    VizOrientation orientation = VizOrientation::Horizontal;
    int barCount = 32;
    int barWidth = 6;
    int barGap = 4;
    int barMaxSize = 140;
    int barIdleSize = 4;
    float barRadiusTL = 3.0f, barRadiusTR = 3.0f, barRadiusBR = 3.0f, barRadiusBL = 3.0f;
    VizColorMode colorMode = VizColorMode::Solid;
    BYTE colorA = 255, colorR = 255, colorG = 255, colorB = 255;
    BYTE grad1A = 255, grad1R = 30, grad1G = 215, grad1B = 96;
    BYTE grad2A = 255, grad2R = 0, grad2G = 180, grad2B = 255;
    int sensitivity = 150;
    VizEQ eq = VizEQ::Default;

    int horizontalPosition = 50;
    int verticalPosition = 88;
    int monitor = 1;
    VizAnchor verticalAnchor = VizAnchor::Bottom;

    bool backgroundEnabled = true;
    BYTE bgA = 0x60, bgR = 0, bgG = 0, bgB = 0;
    int bgPadding = 24;
    float bgRadiusTL = 14.0f, bgRadiusTR = 14.0f, bgRadiusBR = 14.0f, bgRadiusBL = 14.0f;
    int bgBlur = 0;
    int bgBorderSize = 0;
    BYTE borderA = 0x40, borderR = 255, borderG = 255, borderB = 255;

    int targetFps = 60;
    bool pauseOnFullscreen = true;
    int pauseWhenSilentSeconds = 10;
};

constexpr int VIZ_BARS_MAX = 1000;
constexpr int VIZ_FFT_SIZE = 1024;
constexpr int VIZ_NUM_BANDS = 7;
constexpr float VIZ_PI = 3.14159265f;

Settings g_settings;

std::atomic<bool> g_lazyInitialized{false};
std::atomic<bool> g_initSucceeded{false};
std::atomic<bool> g_unloading{false};

ComPtr<ID3D11Device> g_d3dDevice;
ComPtr<IDXGIDevice> g_dxgiDevice;
ComPtr<IDXGIFactory2> g_dxgiFactory;
ComPtr<ID2D1Factory1> g_d2dFactory;
ComPtr<ID2D1Device> g_d2dDevice;

HWND g_messageWnd;

std::atomic<HWND> g_overlayWnd{nullptr};
ComPtr<IDXGISwapChain1> g_swapChain;
ComPtr<ID2D1DeviceContext> g_dc;
ComPtr<IDCompositionDevice> g_compositionDevice;
ComPtr<IDCompositionTarget> g_compositionTarget;
ComPtr<IDCompositionVisual> g_compositionVisual;
ComPtr<ID2D1SolidColorBrush> g_barBrush;
ComPtr<ID2D1SolidColorBrush> g_barBrush2;
ComPtr<ID2D1SolidColorBrush> g_backgroundBrush;
ComPtr<ID2D1SolidColorBrush> g_borderBrush;
ComPtr<ID2D1Bitmap> g_wallpaperBitmap;
ComPtr<ID2D1Effect> g_blurEffect;

static const IID kCLSID_D2D1GaussianBlur = {
    0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

float g_dpiScale = 1.0f;
FILETIME g_lastWallpaperTime = {};

std::atomic<bool> g_captureRunning{false};
std::thread* g_captureThread = nullptr;
HANDLE g_captureEvent = nullptr;
std::atomic<bool> g_deviceChanged{false};
std::atomic<float> g_bands[VIZ_NUM_BANDS] = {};

std::thread* g_renderThread = nullptr;
std::atomic<bool> g_renderThreadRunning{false};
std::atomic<bool> g_renderTickPending{false};

float g_hannWindow[VIZ_FFT_SIZE] = {};
float g_twiddleRe[VIZ_FFT_SIZE / 2] = {};
float g_twiddleIm[VIZ_FFT_SIZE / 2] = {};
int g_logBinStart[VIZ_NUM_BANDS + 1] = {};

float g_vizPeak[VIZ_BARS_MAX] = {};
float g_vizTarget[VIZ_BARS_MAX] = {};
float g_vizBreatheEnv = 0.f;

static float VIZ_SEEDS[VIZ_BARS_MAX] = {};

void BuildVizSeeds() {
    unsigned int state = 0x12345678u;
    for (int i = 0; i < VIZ_BARS_MAX; i++) {
        state = state * 1664525u + 1013904223u;
        float t = (float)(state >> 8) / (float)(1u << 24);
        VIZ_SEEDS[i] = 0.25f + t * 1.20f;
    }
}

std::atomic<bool> g_fullscreenPaused{false};
std::atomic<ULONGLONG> g_lastAudibleTickMs{0};
bool g_slowMode = false;

static std::atomic<DWORD> g_albumArtColor{0xFFFFFFFF};
static std::atomic<DWORD> g_albumArtColorSecondary{0xFFAAAAAA};
static std::atomic<bool>  g_albumArtColorReady{false};
static std::atomic<bool>  g_albumArtFetchPending{false};
static std::atomic<DWORD> g_accentColorCache{0xFF0078D4};

static HANDLE g_gsmtcStopEvent = nullptr;
[[clang::no_destroy]] static std::thread g_gsmtcThread;
static std::thread* g_albumArtThread = nullptr;

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                static const UINT kM =
                    RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == kM) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }
    return module;
}

float GetMonitorDpiScale(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return dpiX / 96.0f;
    }
    return 1.0f;
}

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR monitorResult = nullptr;
    int currentMonitorId = 0;

    auto monitorEnumProc = [&monitorResult, &currentMonitorId,
                            monitorId](HMONITOR hMonitor) -> BOOL {
        if (currentMonitorId == monitorId) {
            monitorResult = hMonitor;
            return FALSE;
        }
        currentMonitorId++;
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

bool ParseColorHex(PCWSTR colorStr, BYTE* a, BYTE* r, BYTE* g, BYTE* b) {
    if (!colorStr || !*colorStr) {
        return false;
    }
    if (*colorStr == L'#') {
        colorStr++;
    }
    size_t len = wcslen(colorStr);
    unsigned int value = 0;
    for (size_t i = 0; i < len; i++) {
        WCHAR c = colorStr[i];
        int digit;
        if (c >= L'0' && c <= L'9') digit = c - L'0';
        else if (c >= L'A' && c <= L'F') digit = c - L'A' + 10;
        else if (c >= L'a' && c <= L'f') digit = c - L'a' + 10;
        else return false;
        value = (value << 4) | digit;
    }
    if (len == 6) {
        *a = 255;
        *r = (value >> 16) & 0xFF;
        *g = (value >> 8) & 0xFF;
        *b = value & 0xFF;
    } else if (len == 8) {
        *a = (value >> 24) & 0xFF;
        *r = (value >> 16) & 0xFF;
        *g = (value >> 8) & 0xFF;
        *b = value & 0xFF;
    } else {
        return false;
    }
    return true;
}

bool IsFolderViewWnd(HWND hWnd) {
    WCHAR buffer[64];

    if (!GetClassName(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SysListView32")) {
        return false;
    }
    if (!GetWindowText(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"FolderView")) {
        return false;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (!hParentWnd) return false;
    if (!GetClassName(hParentWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SHELLDLL_DefView")) {
        return false;
    }
    if (GetWindowTextLength(hParentWnd) > 0) return false;

    HWND hParentWnd2 = GetAncestor(hParentWnd, GA_PARENT);
    if (!hParentWnd2) return false;
    if ((!GetClassName(hParentWnd2, buffer, ARRAYSIZE(buffer)) ||
         _wcsicmp(buffer, L"Progman")) &&
        hParentWnd2 != GetShellWindow()) {
        return false;
    }

    return true;
}

HWND GetWorkerW() {
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (!hProgman) return nullptr;

    DWORD progmanProcessId = 0;
    GetWindowThreadProcessId(hProgman, &progmanProcessId);
    if (progmanProcessId != GetCurrentProcessId()) return nullptr;

    SendMessage(hProgman, 0x052C, 0xD, 0);
    SendMessage(hProgman, 0x052C, 0xD, 1);

    HWND hWorkerW = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
                return TRUE;
            }
            HWND hWorker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
            if (hWorker) {
                *(HWND*)lParam = hWorker;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&hWorkerW);

    if (!hWorkerW) {
        SendMessage(hProgman, 0x052C, 0, 0);
        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
                    return TRUE;
                }
                HWND hWorker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
                if (hWorker) {
                    *(HWND*)lParam = hWorker;
                    return FALSE;
                }
                return TRUE;
            },
            (LPARAM)&hWorkerW);
    }

    if (!hWorkerW) {
        hWorkerW = FindWindowEx(hProgman, nullptr, L"WorkerW", nullptr);
    }
    if (!hWorkerW) {
        hWorkerW = hProgman;
    }

    return hWorkerW;
}

bool IsWindowFullscreen(HWND hwnd, HMONITOR targetMonitor = nullptr) {
    if (!hwnd || !IsWindowVisible(hwnd)) return false;

    RECT windowRect;
    if (!GetWindowRect(hwnd, &windowRect)) return false;

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (targetMonitor && monitor != targetMonitor) return false;

    MONITORINFO monitorInfo = {sizeof(MONITORINFO)};
    if (!GetMonitorInfo(monitor, &monitorInfo)) return false;

    return windowRect.left <= monitorInfo.rcMonitor.left &&
           windowRect.top <= monitorInfo.rcMonitor.top &&
           windowRect.right >= monitorInfo.rcMonitor.right &&
           windowRect.bottom >= monitorInfo.rcMonitor.bottom;
}

bool IsFullscreenOrGameActive() {
    HMONITOR targetMonitor = GetMonitorById(g_settings.monitor - 1);
    if (!targetMonitor) targetMonitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);

    HWND hwndForeground = GetForegroundWindow();
    if (!hwndForeground) return false;

    WCHAR className[256];
    if (GetClassName(hwndForeground, className, ARRAYSIZE(className))) {
        if (_wcsicmp(className, L"Progman") == 0 ||
            _wcsicmp(className, L"WorkerW") == 0 ||
            _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            return false;
        }
    }

    HMONITOR foregroundMonitor = MonitorFromWindow(hwndForeground, MONITOR_DEFAULTTONEAREST);
    if (foregroundMonitor != targetMonitor) return false;

    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE)
            return true;
    }

    if (IsWindowFullscreen(hwndForeground, targetMonitor))
        return true;

    return false;
}

void RefreshAccentColorCache() {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD color = 0, size = sizeof(color), type = 0;
        if (RegQueryValueExW(hKey, L"AccentColorMenu", nullptr, &type,
                             (LPBYTE)&color, &size) == ERROR_SUCCESS && type == REG_DWORD) {
            RegCloseKey(hKey);
            BYTE r = (color >>  0) & 0xFF;
            BYTE g = (color >>  8) & 0xFF;
            BYTE b = (color >> 16) & 0xFF;
            g_accentColorCache.store(0xFF000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | b,
                                     std::memory_order_relaxed);
            return;
        }
        if (RegQueryValueExW(hKey, L"AccentColor", nullptr, &type,
                             (LPBYTE)&color, &size) == ERROR_SUCCESS && type == REG_DWORD) {
            RegCloseKey(hKey);
            BYTE r = (color >>  0) & 0xFF;
            BYTE g = (color >>  8) & 0xFF;
            BYTE b = (color >> 16) & 0xFF;
            g_accentColorCache.store(0xFF000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | b,
                                     std::memory_order_relaxed);
            return;
        }
        RegCloseKey(hKey);
    }

    DWORD color = 0; BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque)))
        g_accentColorCache.store(0xFF000000 | (color & 0x00FFFFFF), std::memory_order_relaxed);
}

DWORD GetWindowsAccentColor() {
    return g_accentColorCache.load(std::memory_order_relaxed);
}

void FetchAlbumArtColorAsync() {
    bool expected = false;
    if (!g_albumArtFetchPending.compare_exchange_strong(expected, true))
        return;

    if (g_albumArtThread) {
        if (g_albumArtThread->joinable())
            g_albumArtThread->join();
        delete g_albumArtThread;
        g_albumArtThread = nullptr;
    }

    g_albumArtThread = new std::thread([]() {
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            auto mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (!mgr) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }
            auto session = mgr.GetCurrentSession();
            if (!session) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            auto props = session.TryGetMediaPropertiesAsync().get();
            if (!props) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            auto thumbRef = props.Thumbnail();
            if (!thumbRef) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            auto stream = thumbRef.OpenReadAsync().get();
            if (!stream) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            UINT64 sz = stream.Size();
            if (sz == 0 || sz > 4 * 1024 * 1024) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            DataReader reader(stream);
            reader.LoadAsync((UINT32)sz).get();
            std::vector<BYTE> thumbBytes((size_t)sz);
            reader.ReadBytes(winrt::array_view<BYTE>(thumbBytes));
            reader.DetachStream();

            IWICImagingFactory* pFactory = nullptr;
            if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                        IID_PPV_ARGS(&pFactory))) || !pFactory) {
                winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return;
            }
            IStream* pStream = SHCreateMemStream(thumbBytes.data(), (UINT)thumbBytes.size());
            if (!pStream) { pFactory->Release(); winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            IWICBitmapDecoder* pDecoder = nullptr;
            IWICBitmapFrameDecode* pFrame = nullptr;
            IWICFormatConverter* pConv = nullptr;
            std::vector<BYTE> pixels;
            int imgW = 0, imgH = 0;

            if (SUCCEEDED(pFactory->CreateDecoderFromStream(pStream, nullptr,
                    WICDecodeMetadataCacheOnDemand, &pDecoder)) &&
                SUCCEEDED(pDecoder->GetFrame(0, &pFrame)) &&
                SUCCEEDED(pFactory->CreateFormatConverter(&pConv))) {
                if (SUCCEEDED(pConv->Initialize(pFrame, GUID_WICPixelFormat32bppBGRA,
                        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut))) {
                    UINT w = 0, h = 0;
                    pConv->GetSize(&w, &h);
                    if (w > 0 && h > 0) {
                        pixels.resize((size_t)w * h * 4);
                        if (SUCCEEDED(pConv->CopyPixels(nullptr, w * 4,
                                (UINT)pixels.size(), pixels.data()))) {
                            imgW = (int)w; imgH = (int)h;
                        }
                    }
                }
            }
            if (pConv)    pConv->Release();
            if (pFrame)   pFrame->Release();
            if (pDecoder) pDecoder->Release();
            pStream->Release();
            pFactory->Release();

            if (!pixels.empty()) {
                struct Bucket { uint32_t r=0,g=0,b=0,n=0; };
                Bucket buckets[16][16][16]{};
                for (int y = 0; y < imgH; y += 4) {
                    for (int x = 0; x < imgW; x += 4) {
                        size_t idx = ((size_t)y * imgW + x) * 4;
                        if (idx + 4 > pixels.size()) continue;
                        BYTE pb = pixels[idx], pg = pixels[idx+1], pr = pixels[idx+2];
                        int luma = (pr * 299 + pg * 587 + pb * 114) / 1000;
                        if (luma < 24 || luma > 235) continue;
                        auto& bk = buckets[pr >> 4][pg >> 4][pb >> 4];
                        bk.r += pr; bk.g += pg; bk.b += pb; bk.n++;
                    }
                }

                struct Cand { float w; BYTE r,g,b; };
                std::vector<Cand> cands;
                cands.reserve(64);
                for (int R = 0; R < 16; R++) for (int G = 0; G < 16; G++) for (int B = 0; B < 16; B++) {
                    auto& bk = buckets[R][G][B];
                    if (bk.n < 8) continue;
                    float fr = bk.r/(float)bk.n/255.f, fg = bk.g/(float)bk.n/255.f, fb = bk.b/(float)bk.n/255.f;
                    float mx = std::max({fr,fg,fb}), mn = std::min({fr,fg,fb});
                    float sat = mx > 0 ? (mx - mn) / mx : 0;
                    cands.push_back({ bk.n * (0.3f + sat),
                                     (BYTE)(fr*255), (BYTE)(fg*255), (BYTE)(fb*255) });
                }

                if (!cands.empty()) {
                    std::sort(cands.begin(), cands.end(),
                              [](const Cand& a, const Cand& b){ return a.w > b.w; });

                    BYTE pR = cands[0].r, pG = cands[0].g, pB = cands[0].b;

                    BYTE sR = pR, sG = pG, sB = pB;
                    for (auto& c : cands) {
                        int dr = (int)c.r - (int)pR;
                        int dg = (int)c.g - (int)pG;
                        int db = (int)c.b - (int)pB;
                        if (dr*dr + dg*dg + db*db > 3264) {
                            sR = c.r; sG = c.g; sB = c.b;
                            break;
                        }
                    }

                    DWORD col  = 0xFF000000 | ((DWORD)pR << 16) | ((DWORD)pG << 8) | pB;
                    DWORD col2 = 0xFF000000 | ((DWORD)sR << 16) | ((DWORD)sG << 8) | sB;
                    g_albumArtColor.store(col,  std::memory_order_relaxed);
                    g_albumArtColorSecondary.store(col2, std::memory_order_relaxed);
                    g_albumArtColorReady.store(true, std::memory_order_relaxed);
                }
            }
        } catch (...) {}
        try { winrt::uninit_apartment(); } catch (...) {}
        g_albumArtFetchPending.store(false);
    });
}

static winrt::event_token g_gsmtcMediaPropsToken{};
static winrt::event_token g_gsmtcSessionToken{};
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSessionManager g_gsmtcMgr{ nullptr };
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSession        g_gsmtcSession{ nullptr };

void SetupGsmtcSessionListener() {
    if (!g_gsmtcMgr) return;
    try {
        if (g_gsmtcSession) {
            try { g_gsmtcSession.MediaPropertiesChanged(g_gsmtcMediaPropsToken); } catch (...) {}
            g_gsmtcSession = nullptr;
        }
        g_gsmtcSession = g_gsmtcMgr.GetCurrentSession();
        if (!g_gsmtcSession) return;
        g_gsmtcMediaPropsToken = g_gsmtcSession.MediaPropertiesChanged(
            [](auto const&, auto const&) {
                if (g_settings.colorMode == VizColorMode::AlbumArt ||
                    g_settings.colorMode == VizColorMode::DynamicAlbum)
                    FetchAlbumArtColorAsync();
            });
    } catch (...) {}
}

void InitGsmtcListener() {
    if (g_gsmtcStopEvent) return;
    g_gsmtcStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_gsmtcStopEvent) return;

    g_gsmtcThread = std::thread([]() {
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            g_gsmtcMgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (!g_gsmtcMgr) { winrt::uninit_apartment(); return; }

            g_gsmtcSessionToken = g_gsmtcMgr.CurrentSessionChanged(
                [](auto const&, auto const&) {
                    SetupGsmtcSessionListener();
                    if (g_settings.colorMode == VizColorMode::AlbumArt ||
                        g_settings.colorMode == VizColorMode::DynamicAlbum)
                        FetchAlbumArtColorAsync();
                });

            SetupGsmtcSessionListener();
            if (g_settings.colorMode == VizColorMode::AlbumArt ||
                g_settings.colorMode == VizColorMode::DynamicAlbum)
                FetchAlbumArtColorAsync();
        } catch (...) {}

        if (g_gsmtcStopEvent)
            WaitForSingleObject(g_gsmtcStopEvent, INFINITE);

        try {
            if (g_gsmtcSession) {
                g_gsmtcSession.MediaPropertiesChanged(g_gsmtcMediaPropsToken);
            }
            if (g_gsmtcMgr) {
                g_gsmtcMgr.CurrentSessionChanged(g_gsmtcSessionToken);
            }
            g_gsmtcSession = nullptr;
            g_gsmtcMgr     = nullptr;
            winrt::uninit_apartment();
        } catch (...) {}
    });
}

static bool g_gsmtcStarted = false;

void BuildHannWindow() {
    for (int i = 0; i < VIZ_FFT_SIZE; i++)
        g_hannWindow[i] = 0.5f * (1.f - cosf(2.f * VIZ_PI * i / (VIZ_FFT_SIZE - 1)));
}

void BuildTwiddleFactors() {
    for (int i = 0; i < VIZ_FFT_SIZE / 2; i++) {
        float ang = -2.0f * VIZ_PI * i / VIZ_FFT_SIZE;
        g_twiddleRe[i] = cosf(ang);
        g_twiddleIm[i] = sinf(ang);
    }
}

void BuildLogBins(UINT32 sampleRate) {
    static constexpr float FREQ_EDGES[VIZ_NUM_BANDS + 1] = {
        20.f, 120.f, 300.f, 800.f, 2500.f, 6000.f, 14000.f, 20000.f};
    for (int b = 0; b <= VIZ_NUM_BANDS; b++) {
        int bin = (int)(FREQ_EDGES[b] * VIZ_FFT_SIZE / (float)sampleRate);
        g_logBinStart[b] = std::max(1, std::min(VIZ_FFT_SIZE / 2 - 1, bin));
    }
}

void VizFFT(std::vector<float>& re, std::vector<float>& im) {
    int n = (int)re.size();
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) {
            std::swap(re[i], re[j]);
            std::swap(im[i], im[j]);
        }
    }
    for (int len = 2; len <= n; len <<= 1) {
        int halfLen = len / 2;
        int stride = n / len;
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < halfLen; j++) {
                float wRe = g_twiddleRe[j * stride];
                float wIm = g_twiddleIm[j * stride];
                float uRe = re[i + j], uIm = im[i + j];
                float vRe = re[i + j + halfLen] * wRe - im[i + j + halfLen] * wIm;
                float vIm = re[i + j + halfLen] * wIm + im[i + j + halfLen] * wRe;
                re[i + j] = uRe + vRe;
                im[i + j] = uIm + vIm;
                re[i + j + halfLen] = uRe - vRe;
                im[i + j + halfLen] = uIm - vIm;
            }
        }
    }
}

struct VizEQMul { float low, mid, high; };
VizEQMul GetVizEQMultipliers(VizEQ eq) {
    switch (eq) {
    case VizEQ::Bass: return {2.0f, 0.6f, 0.4f};
    case VizEQ::Rock: return {1.3f, 1.5f, 1.2f};
    case VizEQ::Pop: return {0.8f, 1.2f, 1.8f};
    case VizEQ::Jazz: return {1.1f, 0.8f, 0.6f};
    case VizEQ::Electronic: return {1.7f, 0.6f, 1.7f};
    default: return {1.0f, 1.0f, 1.0f};
    }
}

class VizEndpointNotificationClient : public IMMNotificationClient {
   public:
    virtual ~VizEndpointNotificationClient() = default;

    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) delete this;
        return ref;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole, LPCWSTR) override {
        if (flow == eRender) g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
        g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
        g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        return S_OK;
    }

   private:
    LONG m_ref = 1;
};

bool VizInitAudioClient(IMMDeviceEnumerator* pEnum, ComPtr<IAudioClient>& pClient,
                        ComPtr<IAudioCaptureClient>& pCapture, UINT32& sampleRate,
                        UINT32& channels, bool& isFloat, HANDLE hEvent) {
    pClient.Reset();
    pCapture.Reset();

    ComPtr<IMMDevice> pDev;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDev))) return false;

    ComPtr<IAudioClient> pC;
    if (FAILED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                              (void**)pC.GetAddressOf())))
        return false;

    WAVEFORMATEX* pwfx = nullptr;
    pC->GetMixFormat(&pwfx);
    if (!pwfx) return false;

    sampleRate = pwfx->nSamplesPerSec;
    channels = pwfx->nChannels;
    isFloat = (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
              (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
               reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->SubFormat ==
                   KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);

    HRESULT hr = pC->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        200000, 0, pwfx, nullptr);
    CoTaskMemFree(pwfx);
    if (FAILED(hr)) return false;

    if (hEvent) pC->SetEventHandle(hEvent);

    ComPtr<IAudioCaptureClient> pCap;
    if (FAILED(pC->GetService(__uuidof(IAudioCaptureClient), (void**)pCap.GetAddressOf())))
        return false;

    if (FAILED(pC->Start())) return false;

    pClient = pC;
    pCapture = pCap;
    return true;
}

void VizCaptureThreadProc() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    BuildHannWindow();
    BuildTwiddleFactors();
    BuildVizSeeds();

    ComPtr<IMMDeviceEnumerator> pEnum;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)pEnum.GetAddressOf()))) {
        g_captureRunning.store(false);
        CoUninitialize();
        return;
    }

    auto* notifyClient = new VizEndpointNotificationClient();
    bool notifyRegistered = SUCCEEDED(pEnum->RegisterEndpointNotificationCallback(notifyClient));

    ComPtr<IAudioClient> pClient;
    ComPtr<IAudioCaptureClient> pCapture;
    UINT32 sampleRate = 48000, channels = 2;
    bool isFloat = true;

    g_deviceChanged.store(false, std::memory_order_relaxed);
    if (VizInitAudioClient(pEnum.Get(), pClient, pCapture, sampleRate, channels, isFloat,
                           g_captureEvent))
        BuildLogBins(sampleRate);

    static constexpr int RING_CAP = VIZ_FFT_SIZE * 4;
    std::vector<float> ringBuf(RING_CAP, 0.f);
    int ringHead = 0, ringCount = 0;
    std::vector<float> re(VIZ_FFT_SIZE), im(VIZ_FFT_SIZE);

    float bandEnv[VIZ_NUM_BANDS] = {};
    static constexpr float GRAVITY[VIZ_NUM_BANDS] = {0.018f, 0.020f, 0.022f, 0.025f,
                                                     0.030f, 0.036f, 0.042f};
    ULONGLONG lastReinitAttempt = GetTickCount64() - 1000;

    while (g_captureRunning.load(std::memory_order_relaxed)) {
        if (g_captureEvent)
            WaitForSingleObject(g_captureEvent, 20);
        else
            Sleep(8);

        bool needsReinit = g_deviceChanged.exchange(false, std::memory_order_relaxed) || !pClient;
        if (needsReinit) {
            ULONGLONG now = GetTickCount64();
            if (now - lastReinitAttempt >= 500) {
                lastReinitAttempt = now;
                if (pClient) pClient->Stop();
                ringHead = 0;
                ringCount = 0;
                for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                    bandEnv[b] = 0.f;
                    g_bands[b].store(0.f, std::memory_order_relaxed);
                }
                if (VizInitAudioClient(pEnum.Get(), pClient, pCapture, sampleRate, channels,
                                       isFloat, g_captureEvent))
                    BuildLogBins(sampleRate);
            }
        }

        if (!pCapture) continue;

        UINT32 packetSize = 0;
        HRESULT hr = pCapture->GetNextPacketSize(&packetSize);
        if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
            g_deviceChanged.store(true, std::memory_order_relaxed);
            continue;
        }
        if (FAILED(hr) || packetSize == 0) {
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                bandEnv[b] = std::max(0.f, bandEnv[b] - GRAVITY[b]);
                g_bands[b].store(bandEnv[b], std::memory_order_relaxed);
            }
            continue;
        }

        while (packetSize > 0) {
            BYTE* pData = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;
            HRESULT hrBuf = pCapture->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
            if (hrBuf == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_deviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hrBuf)) break;

            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData && numFrames > 0) {
                if (isFloat) {
                    float* src = reinterpret_cast<float*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++) mono += src[f * channels + c];
                        mono /= (float)channels;
                        ringBuf[ringHead] = mono;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP) ringCount++;
                    }
                } else {
                    INT16* src = reinterpret_cast<INT16*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++)
                            mono += src[f * channels + c] / 32768.f;
                        mono /= (float)channels;
                        ringBuf[ringHead] = mono;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP) ringCount++;
                    }
                }
            }
            pCapture->ReleaseBuffer(numFrames);
            hr = pCapture->GetNextPacketSize(&packetSize);
            if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_deviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hr)) break;
        }

        while (ringCount >= VIZ_FFT_SIZE) {
            int readStart = (ringHead - ringCount + RING_CAP) % RING_CAP;
            for (int i = 0; i < VIZ_FFT_SIZE; i++) {
                re[i] = ringBuf[(readStart + i) % RING_CAP] * g_hannWindow[i];
                im[i] = 0.f;
            }
            ringCount -= VIZ_FFT_SIZE / 2;
            VizFFT(re, im);

            float t_sens = g_settings.sensitivity / 100.0f;
            float sliderGain = (t_sens <= 1.0f) ? 0.25f + t_sens * t_sens * 2.75f
                                                : 3.0f + (t_sens - 1.0f) * 4.0f;
            auto eq = GetVizEQMultipliers(g_settings.eq);

            static constexpr float BAND_SENSITIVITY[VIZ_NUM_BANDS] = {
                0.30f, 0.22f, 0.12f, 0.06f, 0.030f, 0.018f, 0.010f};
            static constexpr int BAND_EQ_ZONE[VIZ_NUM_BANDS] = {0, 0, 1, 1, 2, 2, 2};

            float maxMag = 0.f;
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                int bStart = g_logBinStart[b];
                int bEnd = g_logBinStart[b + 1];
                if (bEnd <= bStart) bEnd = bStart + 1;

                float sumSq = 0.f;
                int count = 0;
                for (int k = bStart; k < bEnd; k++) {
                    sumSq += re[k] * re[k] + im[k] * im[k];
                    count++;
                }
                float rms = (count > 0) ? sqrtf(sumSq / (float)count) : 0.f;
                float eqM = (BAND_EQ_ZONE[b] == 0)   ? eq.low
                            : (BAND_EQ_ZONE[b] == 1) ? eq.mid
                                                     : eq.high;
                float mag = std::max(
                    0.f, std::min(1.f, (rms / (VIZ_FFT_SIZE * 0.5f)) / BAND_SENSITIVITY[b] *
                                           sliderGain * eqM));

                bandEnv[b] = (mag >= bandEnv[b]) ? mag : std::max(0.f, bandEnv[b] - GRAVITY[b]);
                g_bands[b].store(bandEnv[b], std::memory_order_relaxed);
                maxMag = std::max(maxMag, bandEnv[b]);
            }

            if (maxMag > 0.03f) {
                g_lastAudibleTickMs.store(GetTickCount64(), std::memory_order_relaxed);
            }
        }
    }

    if (pClient) pClient->Stop();
    if (notifyRegistered) pEnum->UnregisterEndpointNotificationCallback(notifyClient);
    notifyClient->Release();
    CoUninitialize();
}

void StartVizCaptureThread() {
    if (g_captureRunning.load()) return;
    if (g_captureThread) {
        if (g_captureThread->joinable()) g_captureThread->join();
        delete g_captureThread;
        g_captureThread = nullptr;
    }
    if (!g_captureEvent) g_captureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_captureRunning.store(true);
    g_captureThread = new std::thread(VizCaptureThreadProc);
}

void StopVizCaptureThread() {
    g_captureRunning.store(false);
    if (g_captureEvent) SetEvent(g_captureEvent);
    if (g_captureThread) {
        if (g_captureThread->joinable()) g_captureThread->join();
        delete g_captureThread;
        g_captureThread = nullptr;
    }
    if (g_captureEvent) {
        CloseHandle(g_captureEvent);
        g_captureEvent = nullptr;
    }
    for (int i = 0; i < VIZ_NUM_BANDS; i++) g_bands[i].store(0.f);
}

void UpdateVisualizerTargets() {
    const int vizBars = std::max(1, std::min(g_settings.barCount, VIZ_BARS_MAX));

    float bands[VIZ_NUM_BANDS];
    float masterPeak = 0.f;
    for (int i = 0; i < VIZ_NUM_BANDS; i++) {
        bands[i] = g_bands[i].load(std::memory_order_relaxed);
        masterPeak = std::max(masterPeak, bands[i]);
    }

    auto eq = GetVizEQMultipliers(g_settings.eq);

    auto sampleBands = [&](float t) -> float {
        float pos = t * (VIZ_NUM_BANDS - 1);
        int lo = (int)pos;
        int hi = std::min(lo + 1, VIZ_NUM_BANDS - 1);
        return bands[lo] * (1.f - (pos - (float)lo)) + bands[hi] * (pos - (float)lo);
    };
    auto eqForT = [&](float t) -> float {
        return (t < 0.33f) ? eq.low : (t < 0.66f) ? eq.mid : eq.high;
    };

    float t = (float)GetTickCount64() * 0.001f;
    float center = (vizBars - 1) * 0.5f;

    for (int i = 0; i < vizBars; i++) {
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;

        switch (g_settings.shape) {
            case VizShape::Stereo:
                target = sampleBands(freqT) * eqForT(freqT);
                break;
            case VizShape::Mountain: {
                float dist = fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(dist) * eqForT(dist);
                float taper = 1.6f - dist * 0.9f;
                target = std::max(0.f, std::min(1.f, (energy + masterPeak * (0.2f - dist * 0.12f)) * taper));
                break;
            }
            case VizShape::Mirror: {
                float mirT = 1.f - fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(mirT) * eqForT(mirT);
                target = std::max(0.f, std::min(1.f, (energy + masterPeak * (0.1f + mirT * 0.12f)) * 1.3f));
                break;
            }
            case VizShape::Wave: {
                float phase = (float)i * (2.f * VIZ_PI / (float)vizBars);
                float wave = 0.55f + 0.45f * sinf(t * 3.5f - phase);
                float energy = sampleBands(freqT) * eqForT(freqT);
                target = std::max(0.f, std::min(1.f, energy * wave + masterPeak * 0.15f));
                break;
            }
            case VizShape::Breathe: {
                if (i == 0) {
                    float k = (masterPeak > g_vizBreatheEnv) ? 0.04f : 0.015f;
                    g_vizBreatheEnv += (masterPeak - g_vizBreatheEnv) * k;
                }
                float rate = 0.55f + VIZ_SEEDS[i % VIZ_BARS_MAX] * 0.18f;
                float inhale = 0.5f + 0.5f * sinf(t * rate + VIZ_SEEDS[i % VIZ_BARS_MAX] * 1.2f);
                target = std::max(0.f, std::min(1.f, inhale * (0.12f + g_vizBreatheEnv * 0.88f)));
                break;
            }
            case VizShape::Dots:
                target = sampleBands(freqT) * eqForT(freqT);
                break;
        }

        g_vizTarget[i] = std::max(0.f, std::min(1.f, target));
    }
}

struct RGBA { BYTE a, r, g, b; };

RGBA LerpColor(RGBA a, RGBA b, float t) {
    auto L = [](BYTE x, BYTE y, float tt) -> BYTE {
        return (BYTE)((int)x + (int)((float)((int)y - (int)x) * tt));
    };
    return {L(a.a, b.a, t), L(a.r, b.r, t), L(a.g, b.g, t), L(a.b, b.b, t)};
}

bool InitDirectX() {
    HRESULT hr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0,
                           D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: 0x%08X", hr);
        return false;
    }

    hr = g_d3dDevice.As(&g_dxgiDevice);
    if (FAILED(hr)) return false;

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_dxgiFactory));
    if (FAILED(hr)) return false;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&g_d2dFactory));
    if (FAILED(hr)) return false;

    hr = g_d2dFactory->CreateDevice(g_dxgiDevice.Get(), &g_d2dDevice);
    if (FAILED(hr)) return false;

    return true;
}

void UninitDirectX() {
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
}

bool RecreateVisualResources();

bool CreateSwapChainResources(UINT width, UINT height) {
    HRESULT hr;

    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = width;
    scd.Height = height;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    hr = g_dxgiFactory->CreateSwapChainForComposition(g_dxgiDevice.Get(), &scd, nullptr,
                                                      &g_swapChain);
    if (FAILED(hr)) return false;

    hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_dc);
    if (FAILED(hr)) return false;

    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &targetBitmap);
    if (FAILED(hr)) return false;

    g_dc->SetTarget(targetBitmap.Get());

    hr = DCompositionCreateDevice(g_dxgiDevice.Get(), IID_PPV_ARGS(&g_compositionDevice));
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->CreateTargetForHwnd(g_overlayWnd, TRUE, &g_compositionTarget);
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->CreateVisual(&g_compositionVisual);
    if (FAILED(hr)) return false;

    hr = g_compositionVisual->SetContent(g_swapChain.Get());
    if (FAILED(hr)) return false;

    hr = g_compositionTarget->SetRoot(g_compositionVisual.Get());
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->Commit();
    if (FAILED(hr)) return false;

    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    g_dpiScale = GetMonitorDpiScale(monitor);

    return RecreateVisualResources();
}

FILETIME GetWallpaperFileTime() {
    WCHAR path[MAX_PATH] = {};
    SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    FILETIME ft = {};
    HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        GetFileTime(hFile, nullptr, nullptr, &ft);
        CloseHandle(hFile);
    }
    return ft;
}

void CaptureWallpaperBitmap() {
    g_wallpaperBitmap.Reset();
    if (!g_overlayWnd || !g_dc || !g_swapChain) return;

    g_lastWallpaperTime = GetWallpaperFileTime();

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_dc->EndDraw();
    g_swapChain->Present(1, 0);
    DwmFlush();

    HWND hParent = GetParent(g_overlayWnd);
    if (!hParent) return;

    RECT rc;
    GetClientRect(hParent, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;

    HWND hSource = FindWindow(L"Progman", nullptr);
    if (!hSource) hSource = hParent;

    HDC hdcScreen = GetDC(nullptr);
    if (!hdcScreen) return;
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) { ReleaseDC(nullptr, hdcScreen); return; }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    if (!hBmp) { DeleteDC(hdcMem); ReleaseDC(nullptr, hdcScreen); return; }

    HGDIOBJ hOldBmp = SelectObject(hdcMem, hBmp);

    if (!PrintWindow(hSource, hdcMem, 0x02 /*PW_RENDERFULLCONTENT*/)) {
        SelectObject(hdcMem, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return;
    }
    GdiFlush();

    BYTE* pixels = static_cast<BYTE*>(pvBits);
    for (int i = 0; i < w * h; i++) pixels[i * 4 + 3] = 255;

    D2D1_BITMAP_PROPERTIES bitmapProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    g_dc->CreateBitmap(D2D1::SizeU(w, h), pvBits, w * 4, bitmapProps, &g_wallpaperBitmap);

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

void ReleaseVisualResources() {
    g_blurEffect.Reset();
    g_wallpaperBitmap.Reset();
    g_borderBrush.Reset();
    g_backgroundBrush.Reset();
    g_barBrush.Reset();
    g_barBrush2.Reset();
}

void ReleaseSwapChainResources() {
    ReleaseVisualResources();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
    g_dc.Reset();
    g_swapChain.Reset();
}

bool RecreateVisualResources() {
    HRESULT hr;

    D2D1_COLOR_F barColor = D2D1::ColorF(g_settings.colorR / 255.0f, g_settings.colorG / 255.0f,
                                         g_settings.colorB / 255.0f, g_settings.colorA / 255.0f);
    hr = g_dc->CreateSolidColorBrush(barColor, &g_barBrush);
    if (FAILED(hr)) return false;

    D2D1_COLOR_F barColor2 = D2D1::ColorF(g_settings.grad2R / 255.0f, g_settings.grad2G / 255.0f,
                                          g_settings.grad2B / 255.0f, g_settings.grad2A / 255.0f);
    g_dc->CreateSolidColorBrush(barColor2, &g_barBrush2);

    if (g_settings.backgroundEnabled) {
        D2D1_COLOR_F bgColor = D2D1::ColorF(g_settings.bgR / 255.0f, g_settings.bgG / 255.0f,
                                            g_settings.bgB / 255.0f, g_settings.bgA / 255.0f);
        g_dc->CreateSolidColorBrush(bgColor, &g_backgroundBrush);

        if (g_settings.bgBlur > 0) {
            CaptureWallpaperBitmap();
            if (g_wallpaperBitmap) {
                hr = g_dc->CreateEffect(kCLSID_D2D1GaussianBlur, &g_blurEffect);
                if (SUCCEEDED(hr)) {
                    g_blurEffect->SetInput(0, g_wallpaperBitmap.Get());
                    g_blurEffect->SetValue(0, (FLOAT)g_settings.bgBlur);
                    g_blurEffect->SetValue(2, (UINT32)1);
                }
            }
        }

        if (g_settings.bgBorderSize > 0) {
            D2D1_COLOR_F borderColor =
                D2D1::ColorF(g_settings.borderR / 255.0f, g_settings.borderG / 255.0f,
                            g_settings.borderB / 255.0f, g_settings.borderA / 255.0f);
            g_dc->CreateSolidColorBrush(borderColor, &g_borderBrush);
        }
    }

    return true;
}

bool ResizeSwapChain(UINT width, UINT height) {
    if (!g_swapChain || !g_dc) return false;

    g_dc->SetTarget(nullptr);

    HRESULT hr = g_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) return false;

    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &targetBitmap);
    if (FAILED(hr)) return false;

    g_dc->SetTarget(targetBitmap.Get());
    return true;
}

void FillRoundedRectPerCorner(ID2D1DeviceContext* dc, ID2D1Factory1* factory,
                               const D2D1_RECT_F& r, ID2D1Brush* brush,
                               float rTL, float rTR, float rBR, float rBL) {
    if (rTL == rTR && rTR == rBR && rBR == rBL) {
        float clampedR = std::min(rTL, std::min(r.right - r.left, r.bottom - r.top) / 2.0f);
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(r, clampedR, clampedR), brush);
        return;
    }

    float w = r.right  - r.left;
    float h = r.bottom - r.top;

    rTL = std::min(rTL, std::min(w, h) / 2.0f);
    rTR = std::min(rTR, std::min(w, h) / 2.0f);
    rBR = std::min(rBR, std::min(w, h) / 2.0f);
    rBL = std::min(rBL, std::min(w, h) / 2.0f);

    ComPtr<ID2D1PathGeometry> geo;
    if (FAILED(factory->CreatePathGeometry(&geo))) return;

    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(geo->Open(&sink))) return;

    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure(D2D1::Point2F(r.left + rTL, r.top), D2D1_FIGURE_BEGIN_FILLED);

    sink->AddLine(D2D1::Point2F(r.right - rTR, r.top));
    if (rTR > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right, r.top + rTR),
            D2D1::SizeF(rTR, rTR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.right, r.bottom - rBR));
    if (rBR > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right - rBR, r.bottom),
            D2D1::SizeF(rBR, rBR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left + rBL, r.bottom));
    if (rBL > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left, r.bottom - rBL),
            D2D1::SizeF(rBL, rBL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left, r.top + rTL));
    if (rTL > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left + rTL, r.top),
            D2D1::SizeF(rTL, rTL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();

    dc->FillGeometry(geo.Get(), brush);
}

HRESULT CreateRoundedRectPath(ID2D1Factory1* factory, const D2D1_RECT_F& r,
                               float rTL, float rTR, float rBR, float rBL,
                               ID2D1PathGeometry** outGeo) {
    float w = r.right - r.left, h = r.bottom - r.top;
    float maxR = std::min(w, h) / 2.0f;
    rTL = std::min(rTL, maxR); rTR = std::min(rTR, maxR);
    rBR = std::min(rBR, maxR); rBL = std::min(rBL, maxR);

    ComPtr<ID2D1PathGeometry> geo;
    HRESULT hr = factory->CreatePathGeometry(&geo);
    if (FAILED(hr)) return hr;

    ComPtr<ID2D1GeometrySink> sink;
    hr = geo->Open(&sink);
    if (FAILED(hr)) return hr;

    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure(D2D1::Point2F(r.left + rTL, r.top), D2D1_FIGURE_BEGIN_FILLED);

    sink->AddLine(D2D1::Point2F(r.right - rTR, r.top));
    if (rTR > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right, r.top + rTR),
        D2D1::SizeF(rTR, rTR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.right, r.bottom - rBR));
    if (rBR > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right - rBR, r.bottom),
        D2D1::SizeF(rBR, rBR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left + rBL, r.bottom));
    if (rBL > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left, r.bottom - rBL),
        D2D1::SizeF(rBL, rBL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left, r.top + rTL));
    if (rTL > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left + rTL, r.top),
        D2D1::SizeF(rTL, rTL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    hr = sink->Close();
    if (FAILED(hr)) return hr;

    *outGeo = geo.Detach();
    return S_OK;
}

void RenderVisualizer() {
    if (g_unloading || !g_dc || !g_swapChain) return;

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    UpdateVisualizerTargets();

    int barCount = std::max(1, std::min(g_settings.barCount, VIZ_BARS_MAX));
    float barW = (float)std::max(1, g_settings.barWidth) * g_dpiScale;
    float barGap = (float)std::max(0, g_settings.barGap) * g_dpiScale;
    float maxSize = (float)std::max(2, g_settings.barMaxSize) * g_dpiScale;
    float idleSize = (float)std::max(0, g_settings.barIdleSize) * g_dpiScale;
    float rTL = g_settings.barRadiusTL * g_dpiScale;
    float rTR = g_settings.barRadiusTR * g_dpiScale;
    float rBR = g_settings.barRadiusBR * g_dpiScale;
    float rBL = g_settings.barRadiusBL * g_dpiScale;

    float attack = 0.55f, decay = 0.18f;
    switch (g_settings.shape) {
        case VizShape::Stereo:  attack = 0.72f; decay = 0.22f; break;
        case VizShape::Mirror:  attack = 0.52f; decay = 0.20f; break;
        case VizShape::Wave:    attack = 0.34f; decay = 0.17f; break;
        case VizShape::Breathe: attack = 0.20f; decay = 0.11f; break;
        default: break;
    }

    bool horizontal = (g_settings.orientation == VizOrientation::Horizontal);
    
    float barsThickness = barCount * barW + (barCount - 1) * barGap;
    
    float groupThickness = barsThickness;
    float groupExtent    = maxSize;

    float totalWidth  = horizontal ? groupThickness : groupExtent;
    float totalHeight = horizontal ? groupExtent    : groupThickness;

    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitorInfo{.cbSize = sizeof(monitorInfo)};
    if (GetMonitorInfo(monitor, &monitorInfo)) {
        int virtualScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int virtualScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);

        RECT workArea;
        workArea.left = monitorInfo.rcWork.left - virtualScreenX;
        workArea.top = monitorInfo.rcWork.top - virtualScreenY;
        workArea.right = monitorInfo.rcWork.right - virtualScreenX;
        workArea.bottom = monitorInfo.rcWork.bottom - virtualScreenY;

        float workWidth = (float)(workArea.right - workArea.left);
        float workHeight = (float)(workArea.bottom - workArea.top);

        float blockX = workArea.left +
                       (workWidth - totalWidth) * (g_settings.horizontalPosition / 100.0f);
        float blockY = workArea.top +
                       (workHeight - totalHeight) * (g_settings.verticalPosition / 100.0f);

        if (g_backgroundBrush) {
            float padding = (float)g_settings.bgPadding * g_dpiScale;
            float bgWidth  = totalWidth  + 2 * padding;
            float bgHeight = totalHeight + 2 * padding;

            float bgTL = g_settings.bgRadiusTL * g_dpiScale;
            float bgTR = g_settings.bgRadiusTR * g_dpiScale;
            float bgBR = g_settings.bgRadiusBR * g_dpiScale;
            float bgBL = g_settings.bgRadiusBL * g_dpiScale;

            D2D1_RECT_F bgRect = D2D1::RectF(blockX - padding, blockY - padding,
                                              blockX + totalWidth + padding,
                                              blockY + totalHeight + padding);

            ComPtr<ID2D1PathGeometry> bgGeo;
            CreateRoundedRectPath(g_d2dFactory.Get(), bgRect, bgTL, bgTR, bgBR, bgBL, &bgGeo);

            if (g_blurEffect && bgGeo) {
                g_dc->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), bgGeo.Get()),
                                nullptr);
                g_dc->DrawImage(g_blurEffect.Get());
                g_dc->PopLayer();
            }

            if (bgGeo)
                g_dc->FillGeometry(bgGeo.Get(), g_backgroundBrush.Get());

            if (g_borderBrush) {
                float bw = std::min((float)g_settings.bgBorderSize * g_dpiScale,
                                    std::min(bgWidth, bgHeight) / 2.0f);

                D2D1_RECT_F innerRect = D2D1::RectF(bgRect.left + bw, bgRect.top + bw,
                                                     bgRect.right - bw, bgRect.bottom - bw);
                float iTL = std::max(0.0f, bgTL - bw);
                float iTR = std::max(0.0f, bgTR - bw);
                float iBR = std::max(0.0f, bgBR - bw);
                float iBL = std::max(0.0f, bgBL - bw);

                ComPtr<ID2D1PathGeometry> innerGeo;
                CreateRoundedRectPath(g_d2dFactory.Get(), innerRect, iTL, iTR, iBR, iBL, &innerGeo);

                if (bgGeo && innerGeo) {
                    ID2D1Geometry* geos[] = {bgGeo.Get(), innerGeo.Get()};
                    ComPtr<ID2D1GeometryGroup> ring;
                    g_d2dFactory->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE, geos, 2, &ring);
                    if (ring) g_dc->FillGeometry(ring.Get(), g_borderBrush.Get());
                }
            }
        }

        RGBA c1{g_settings.colorA, g_settings.colorR, g_settings.colorG, g_settings.colorB};
        {
            if (g_settings.colorMode == VizColorMode::Accent) {
                DWORD dw = GetWindowsAccentColor();
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            } else if (g_settings.colorMode == VizColorMode::AlbumArt) {
                DWORD dw = g_albumArtColor.load(std::memory_order_relaxed);
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            } else if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
                DWORD dw = g_albumArtColor.load(std::memory_order_relaxed);
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            }
        }
        RGBA c2{g_settings.grad2A, g_settings.grad2R, g_settings.grad2G, g_settings.grad2B};
        RGBA cGrad1{g_settings.grad1A, g_settings.grad1R, g_settings.grad1G, g_settings.grad1B};
        if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
            DWORD dw = g_albumArtColorSecondary.load(std::memory_order_relaxed);
            c2    = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            cGrad1 = c1;
        }

        if (g_settings.shape == VizShape::Dots) {
            float dotR  = barW * 0.5f;
            float step  = barW + barGap;
            float dTL = g_settings.barRadiusTL * g_dpiScale;
            float dTR = g_settings.barRadiusTR * g_dpiScale;
            float dBR = g_settings.barRadiusBR * g_dpiScale;
            float dBL = g_settings.barRadiusBL * g_dpiScale;

            for (int i = 0; i < barCount; i++) {
                float tgt = g_vizTarget[i], cur = g_vizPeak[i];
                float nxt = cur + (tgt - cur) * ((tgt > cur) ? attack : decay);
                g_vizPeak[i] = (fabsf(nxt - cur) > 0.0005f) ? nxt : tgt;
                float fac = std::max(0.f, g_vizPeak[i]);
                float colSize = idleSize + fac * std::max(0.f, maxSize - idleSize);

                if (colSize < 0.5f) continue;

                RGBA col = c1;
                if (g_settings.colorMode == VizColorMode::Gradient)
                    col = LerpColor(cGrad1, c2, (barCount > 1) ? (float)i/(barCount-1) : 0.f);
                else if (g_settings.colorMode == VizColorMode::ReactiveGradient)
                    col = LerpColor(cGrad1, c2, fac);
                else if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
                    float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                    float freqT = std::min(1.f, t * 0.6f + fac * 0.4f);
                    col = LerpColor(cGrad1, c2, freqT);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(30, std::min(180, (int)(150.f * fac + 30.f)));
                    col = {aa, c1.r, c1.g, c1.b};
                }
                g_barBrush->SetColor(D2D1::ColorF(col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f));

                int numDots = (step > 0.5f) ? (int)(colSize / step) : 1;
                numDots = std::max(1, numDots);

                auto drawDot = [&](float cx2, float cy2) {
                    D2D1_RECT_F r = D2D1::RectF(cx2 - dotR, cy2 - dotR, cx2 + dotR, cy2 + dotR);
                    FillRoundedRectPerCorner(g_dc.Get(), g_d2dFactory.Get(), r,
                                             g_barBrush.Get(), dTL, dTR, dBR, dBL);
                };

                for (int d = 0; d < numDots; d++) {
                    if (horizontal) {
                        float cx2 = blockX + i * step + dotR;
                        switch (g_settings.verticalAnchor) {
                            case VizAnchor::Top: {
                                float cy2 = blockY + d * step + dotR;
                                if (cy2 - dotR > blockY + maxSize) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                            case VizAnchor::Middle: {
                                float centerY = blockY + maxSize * 0.5f;
                                if (d == 0) {
                                    drawDot(cx2, centerY);
                                } else {
                                    float cy2up   = centerY - d * step;
                                    float cy2down = centerY + d * step;
                                    bool upOk   = cy2up   - dotR >= blockY;
                                    bool downOk = cy2down + dotR <= blockY + maxSize;
                                    if (upOk)   drawDot(cx2, cy2up);
                                    if (downOk) drawDot(cx2, cy2down);
                                    if (!upOk && !downOk) goto next_dot_h;
                                }
                                break;
                            }
                            default: {
                                float cy2 = blockY + maxSize - d * step - dotR;
                                if (cy2 + dotR < blockY) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                        }
                        continue;
                        next_dot_h: break;
                    } else {
                        float cy2 = blockY + i * step + dotR;
                        switch (g_settings.verticalAnchor) {
                            case VizAnchor::Top: {
                                float cx2 = blockX + groupExtent - d * step - dotR;
                                if (cx2 + dotR < blockX) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                            case VizAnchor::Middle: {
                                float centerX = blockX + groupExtent * 0.5f;
                                if (d == 0) {
                                    drawDot(centerX, cy2);
                                } else {
                                    float cx2r = centerX + d * step;
                                    float cx2l = centerX - d * step;
                                    bool rOk = cx2r + dotR <= blockX + groupExtent;
                                    bool lOk = cx2l - dotR >= blockX;
                                    if (rOk) drawDot(cx2r, cy2);
                                    if (lOk) drawDot(cx2l, cy2);
                                    if (!rOk && !lOk) goto next_dot_v;
                                }
                                break;
                            }
                            default: {
                                float cx2 = blockX + d * step + dotR;
                                if (cx2 - dotR > blockX + groupExtent) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                        }
                        continue;
                        next_dot_v: break;
                    }
                }
            }
        }
        else {
            for (int i = 0; i < barCount; i++) {
                float tgt = g_vizTarget[i], cur = g_vizPeak[i];
                float next = cur + (tgt - cur) * ((tgt > cur) ? attack : decay);
                g_vizPeak[i] = (fabsf(next - cur) > 0.0005f) ? next : tgt;

                float fac = std::max(0.f, g_vizPeak[i]);
                float size = idleSize + fac * std::max(0.f, maxSize - idleSize);

                RGBA col = c1;
                if (g_settings.colorMode == VizColorMode::Gradient) {
                    float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                    col = LerpColor(cGrad1, c2, t);
                } else if (g_settings.colorMode == VizColorMode::ReactiveGradient) {
                    col = LerpColor(cGrad1, c2, fac);
                } else if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
                    float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                    float freqT = std::min(1.f, t * 0.6f + fac * 0.4f);
                    col = LerpColor(cGrad1, c2, freqT);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(30, std::min(180, (int)(150.f * fac + 30.f)));
                    col = {aa, c1.r, c1.g, c1.b};
                }

                D2D1_COLOR_F d2dCol = D2D1::ColorF(col.r / 255.0f, col.g / 255.0f, col.b / 255.0f,
                                                   col.a / 255.0f);
                g_barBrush->SetColor(d2dCol);

                D2D1_RECT_F barRect;
                if (horizontal) {
                    float x = blockX + i * (barW + barGap);
                    float y, yBottom;
                    switch (g_settings.verticalAnchor) {
                        case VizAnchor::Top:
                            y = blockY;
                            yBottom = blockY + size;
                            break;
                        case VizAnchor::Middle:
                            y = blockY + (maxSize - size) / 2.0f;
                            yBottom = y + size;
                            break;
                        default:
                            y = blockY + (maxSize - size);
                            yBottom = blockY + maxSize;
                            break;
                    }
                    barRect = D2D1::RectF(x, y, x + barW, yBottom);
                    FillRoundedRectPerCorner(g_dc.Get(), g_d2dFactory.Get(), barRect,
                                             g_barBrush.Get(), rTL, rTR, rBR, rBL);
                } else {
                    float y = blockY + i * (barW + barGap);
                    float x, xRight;
                    switch (g_settings.verticalAnchor) {
                        case VizAnchor::Top:
                            xRight = blockX + groupExtent;
                            x = xRight - size;
                            break;
                        case VizAnchor::Middle: {
                            float cx = blockX + groupExtent / 2.0f;
                            x = cx - size / 2.0f;
                            xRight = cx + size / 2.0f;
                            break;
                        }
                        default:
                            x = blockX;
                            xRight = blockX + size;
                            break;
                    }
                    barRect = D2D1::RectF(x, y, xRight, y + barW);
                    FillRoundedRectPerCorner(g_dc.Get(), g_d2dFactory.Get(), barRect,
                                             g_barBrush.Get(), rTL, rTR, rBR, rBL);
                }
            }
        }
    }

    g_dc->EndDraw();
    g_swapChain->Present(1, 0);
}

void RenderThreadProc() {
    ULONGLONG lastRenderTick = 0;

    while (g_renderThreadRunning.load(std::memory_order_relaxed)) {
        HRESULT hr = DwmFlush();
        if (FAILED(hr)) {
            Sleep(8);
        }

        if (!g_renderThreadRunning.load(std::memory_order_relaxed) || g_unloading.load())
            break;

        HWND overlayWnd = g_overlayWnd.load(std::memory_order_relaxed);
        if (!overlayWnd || g_fullscreenPaused.load(std::memory_order_relaxed)) {
            lastRenderTick = 0;
            continue;
        }

        int fps = std::max(1, g_settings.targetFps);
        UINT interval = 1000 / (UINT)fps;
        if (g_settings.pauseWhenSilentSeconds > 0) {
            ULONGLONG lastAudible = g_lastAudibleTickMs.load(std::memory_order_relaxed);
            ULONGLONG idleMs = GetTickCount64() - lastAudible;
            g_slowMode = idleMs > (ULONGLONG)g_settings.pauseWhenSilentSeconds * 1000ULL;
            if (g_slowMode) {
                interval = 200;
            }
        } else {
            g_slowMode = false;
        }

        ULONGLONG now = GetTickCount64();
        if (lastRenderTick != 0 && (now - lastRenderTick) < interval)
            continue;
        lastRenderTick = now;

        bool expected = false;
        if (g_renderTickPending.compare_exchange_strong(expected, true)) {
            PostMessage(overlayWnd, WM_APP_RENDER_TICK, 0, 0);
        }
    }
}

void StartRenderThread() {
    if (g_renderThread) return;
    g_renderThreadRunning.store(true, std::memory_order_relaxed);
    g_renderThread = new std::thread(RenderThreadProc);
}

void StopRenderThread() {
    g_renderThreadRunning.store(false, std::memory_order_relaxed);
    if (g_renderThread) {
        if (g_renderThread->joinable()) g_renderThread->join();
        delete g_renderThread;
        g_renderThread = nullptr;
    }
    g_renderTickPending.store(false, std::memory_order_relaxed);
}

void PauseForFullscreen() {
    if (g_fullscreenPaused.exchange(true)) return;
    Wh_Log(L"Pausing visualizer: fullscreen detected");
    StopVizCaptureThread();
    if (g_dc && g_swapChain) {
        g_dc->BeginDraw();
        g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
        g_dc->EndDraw();
        g_swapChain->Present(1, 0);
    }
}

void ResumeFromFullscreen() {
    if (!g_fullscreenPaused.exchange(false)) return;
    Wh_Log(L"Resuming visualizer: fullscreen closed");
    StartVizCaptureThread();
    if (g_overlayWnd) {
        RenderVisualizer();
    }
}

void HandleDisplayChange() {
    if (!g_overlayWnd) return;

    HWND hWorkerW = GetParent(g_overlayWnd);
    if (!hWorkerW) return;

    RECT rc;
    GetWindowRect(hWorkerW, &rc);
    SetWindowPos(g_overlayWnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    float newDpiScale = GetMonitorDpiScale(monitor);
    if (newDpiScale != g_dpiScale) {
        ReleaseSwapChainResources();
        GetClientRect(g_overlayWnd, &rc);
        CreateSwapChainResources(rc.right - rc.left, rc.bottom - rc.top);
        RenderVisualizer();
    }

    if (g_messageWnd && g_settings.backgroundEnabled && g_settings.bgBlur > 0) {
        SetTimer(g_messageWnd, TIMER_ID_MSG_WALLPAPER_REFRESH, 500, nullptr);
    }
}

void CreateOverlayWindow();
void ApplySettingsChanged();

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_APP_RENDER_TICK:
            g_renderTickPending.store(false, std::memory_order_relaxed);
            if (!g_unloading && !g_fullscreenPaused.load()) {
                RenderVisualizer();
            }
            return 0;

        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE) && !g_unloading) {
                ResizeSwapChain(wp->cx, wp->cy);
                if (!g_fullscreenPaused.load()) RenderVisualizer();
            }
            break;
        }

        case WM_DESTROY:
            ReleaseSwapChainResources();
            g_overlayWnd = nullptr;
            if (!g_unloading && g_messageWnd) {
                SetTimer(g_messageWnd, TIMER_ID_MSG_RECREATE_OVERLAY, 200, nullptr);
            }
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;

        case WM_APP_SETTINGS_CHANGED:
            ApplySettingsChanged();
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DISPLAYCHANGE:
            if (!g_unloading) {
                SetTimer(hWnd, TIMER_ID_MSG_DISPLAY_CHANGE, 200, nullptr);
            }
            return 0;

        case WM_SETTINGCHANGE: {
            RefreshAccentColorCache();

            if (g_overlayWnd && !g_unloading && g_settings.backgroundEnabled &&
                g_settings.bgBlur > 0) {
                FILETIME ft = GetWallpaperFileTime();
                if (CompareFileTime(&ft, &g_lastWallpaperTime) != 0) {
                    SetTimer(hWnd, TIMER_ID_MSG_WALLPAPER_REFRESH, 2000, nullptr);
                }
            }
            return 0;
        }

        case WM_DWMCOLORIZATIONCOLORCHANGED:
            RefreshAccentColorCache();
            return 0;

        case WM_TIMER:
            if (g_unloading) return 0;
            if (wParam == TIMER_ID_MSG_DISPLAY_CHANGE) {
                KillTimer(hWnd, TIMER_ID_MSG_DISPLAY_CHANGE);
                HandleDisplayChange();
            } else if (wParam == TIMER_ID_MSG_RECREATE_OVERLAY) {
                KillTimer(hWnd, TIMER_ID_MSG_RECREATE_OVERLAY);
                CreateOverlayWindow();
            } else if (wParam == TIMER_ID_MSG_WALLPAPER_REFRESH) {
                KillTimer(hWnd, TIMER_ID_MSG_WALLPAPER_REFRESH);
                if (g_overlayWnd && g_settings.backgroundEnabled && g_settings.bgBlur > 0) {
                    ReleaseVisualResources();
                    RecreateVisualResources();
                    if (!g_fullscreenPaused.load()) RenderVisualizer();
                }
            } else if (wParam == TIMER_ID_MSG_FULLSCREEN_WATCH) {
                if (g_settings.pauseOnFullscreen) {
                    if (IsFullscreenOrGameActive()) {
                        PauseForFullscreen();
                    } else {
                        ResumeFromFullscreen();
                    }
                } else if (g_fullscreenPaused.load()) {
                    ResumeFromFullscreen();
                }
            }
            return 0;

        case WM_DESTROY:
            g_messageWnd = nullptr;
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool g_overlayClassRegistered = false;

bool RegisterOverlayWindowClass() {
    if (g_overlayClassRegistered) return true;
    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = OVERLAY_WINDOW_CLASS;
    if (!RegisterClass(&wc)) return false;
    g_overlayClassRegistered = true;
    return true;
}

void UnregisterOverlayWindowClass() {
    if (g_overlayClassRegistered) {
        UnregisterClass(OVERLAY_WINDOW_CLASS, GetCurrentModuleHandle());
        g_overlayClassRegistered = false;
    }
}

bool EnsureLazyInitialized() {
    if (g_lazyInitialized.exchange(true)) return g_initSucceeded;

    if (!InitDirectX()) {
        Wh_Log(L"InitDirectX failed");
        return false;
    }

    if (!g_settings.pauseOnFullscreen || !IsFullscreenOrGameActive()) {
        StartVizCaptureThread();
    } else {
        Wh_Log(L"Fullscreen active at startup, starting paused");
        g_fullscreenPaused.store(true);
    }

    StartRenderThread();

    g_initSucceeded = true;
    return true;
}

void CreateOverlayWindow() {
    if (g_overlayWnd) return;
    if (!EnsureLazyInitialized()) return;

    HWND hWorkerW = GetWorkerW();
    if (!hWorkerW) {
        Wh_Log(L"Failed to find WorkerW");
        return;
    }

    if (!RegisterOverlayWindowClass()) return;

    HINSTANCE hInstance = GetCurrentModuleHandle();

    RECT rc;
    GetWindowRect(hWorkerW, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    g_overlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, OVERLAY_WINDOW_CLASS,
        nullptr, WS_CHILD | WS_VISIBLE, 0, 0, width, height, hWorkerW, nullptr, hInstance,
        nullptr);
    if (!g_overlayWnd) return;

    if (!g_gsmtcStarted) {
        InitGsmtcListener();
        g_gsmtcStarted = true;
    }

    if (CreateSwapChainResources(width, height)) {
        if (!g_fullscreenPaused.load()) {
            RenderVisualizer();
        }
    }
}

bool g_messageClassRegistered = false;

bool RegisterMessageWindowClass() {
    if (g_messageClassRegistered) return true;
    WNDCLASS wc = {};
    wc.lpfnWndProc = MessageWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = MESSAGE_WINDOW_CLASS;
    if (!RegisterClass(&wc)) return false;
    g_messageClassRegistered = true;
    return true;
}

void UnregisterMessageWindowClass() {
    if (g_messageClassRegistered) {
        UnregisterClass(MESSAGE_WINDOW_CLASS, GetCurrentModuleHandle());
        g_messageClassRegistered = false;
    }
}

void CreateMessageWindow() {
    if (g_messageWnd) return;
    if (!RegisterMessageWindowClass()) return;

    HINSTANCE hInstance = GetCurrentModuleHandle();
    g_messageWnd = CreateWindowEx(0, MESSAGE_WINDOW_CLASS, nullptr, 0, 0, 0, 0, 0, nullptr,
                                  nullptr, hInstance, nullptr);
    if (g_messageWnd) {
        SetTimer(g_messageWnd, TIMER_ID_MSG_FULLSCREEN_WATCH, 1000, nullptr);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
                                         nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !IsFolderViewWnd(hWnd)) return hWnd;

    static UINT_PTR s_timer = 0;
    s_timer = SetTimer(nullptr, s_timer, 1000, [](HWND, UINT, UINT_PTR idEvent, DWORD) {
        KillTimer(nullptr, idEvent);
        CreateOverlayWindow();
        CreateMessageWindow();
    });

    return hWnd;
}

void LoadSettings() {
    PCWSTR shape = Wh_GetStringSetting(L"appearance.shape");
    g_settings.shape = (wcscmp(shape, L"mountain") == 0) ? VizShape::Mountain
                       : (wcscmp(shape, L"mirror") == 0) ? VizShape::Mirror
                       : (wcscmp(shape, L"wave") == 0)   ? VizShape::Wave
                       : (wcscmp(shape, L"breathe") == 0)? VizShape::Breathe
                       : (wcscmp(shape, L"dots") == 0)   ? VizShape::Dots
                                                         : VizShape::Stereo;
    Wh_FreeStringSetting(shape);

    PCWSTR orientation = Wh_GetStringSetting(L"appearance.orientation");
    g_settings.orientation =
        (wcscmp(orientation, L"vertical") == 0) ? VizOrientation::Vertical : VizOrientation::Horizontal;
    Wh_FreeStringSetting(orientation);

    g_settings.barCount = std::clamp(Wh_GetIntSetting(L"appearance.barCount"), 1, VIZ_BARS_MAX);
    g_settings.barWidth = std::max(1, Wh_GetIntSetting(L"appearance.barWidth"));
    g_settings.barGap = std::max(0, Wh_GetIntSetting(L"appearance.barGap"));
    g_settings.barMaxSize = std::max(2, Wh_GetIntSetting(L"appearance.barMaxSize"));
    g_settings.barIdleSize = std::max(0, Wh_GetIntSetting(L"appearance.barIdleSize"));

    PCWSTR barCornerRadiusStr = Wh_GetStringSetting(L"appearance.barCornerRadius");
    {
        float v[4] = {3.0f, 3.0f, 3.0f, 3.0f};
        int n = swscanf_s(barCornerRadiusStr, L"%f %f %f %f", &v[0], &v[1], &v[2], &v[3]);
        if (n == 1) { v[1] = v[2] = v[3] = v[0]; }
        for (int k = 0; k < 4; k++) v[k] = std::max(0.0f, v[k]);
        g_settings.barRadiusTL = v[0];
        g_settings.barRadiusTR = v[1];
        g_settings.barRadiusBR = v[2];
        g_settings.barRadiusBL = v[3];
    }
    Wh_FreeStringSetting(barCornerRadiusStr);

    PCWSTR colorMode = Wh_GetStringSetting(L"appearance.colorMode");
    g_settings.colorMode = (wcscmp(colorMode, L"gradient") == 0)          ? VizColorMode::Gradient
                           : (wcscmp(colorMode, L"reactive_gradient") == 0) ? VizColorMode::ReactiveGradient
                           : (wcscmp(colorMode, L"accent") == 0)            ? VizColorMode::Accent
                           : (wcscmp(colorMode, L"album_art") == 0)         ? VizColorMode::AlbumArt
                           : (wcscmp(colorMode, L"dynamic_album") == 0)     ? VizColorMode::DynamicAlbum
                           : (wcscmp(colorMode, L"acrylic") == 0)           ? VizColorMode::Acrylic
                                                                             : VizColorMode::Solid;
    Wh_FreeStringSetting(colorMode);

    PCWSTR color = Wh_GetStringSetting(L"appearance.color");
    if (!ParseColorHex(color, &g_settings.colorA, &g_settings.colorR, &g_settings.colorG,
                       &g_settings.colorB)) {
        g_settings.colorA = g_settings.colorR = g_settings.colorG = g_settings.colorB = 255;
    }
    Wh_FreeStringSetting(color);

    PCWSTR grad1 = Wh_GetStringSetting(L"appearance.gradientColor1");
    if (!ParseColorHex(grad1, &g_settings.grad1A, &g_settings.grad1R, &g_settings.grad1G,
                       &g_settings.grad1B)) {
        g_settings.grad1A = 255; g_settings.grad1R = 30; g_settings.grad1G = 215; g_settings.grad1B = 96;
    }
    Wh_FreeStringSetting(grad1);

    PCWSTR grad2 = Wh_GetStringSetting(L"appearance.gradientColor2");
    if (!ParseColorHex(grad2, &g_settings.grad2A, &g_settings.grad2R, &g_settings.grad2G,
                       &g_settings.grad2B)) {
        g_settings.grad2A = 255; g_settings.grad2R = 0; g_settings.grad2G = 180; g_settings.grad2B = 255;
    }
    Wh_FreeStringSetting(grad2);

    g_settings.sensitivity = std::clamp(Wh_GetIntSetting(L"appearance.sensitivity"), 0, 300);

    PCWSTR eq = Wh_GetStringSetting(L"appearance.eqPreset");
    g_settings.eq = (wcscmp(eq, L"bass") == 0)       ? VizEQ::Bass
                   : (wcscmp(eq, L"rock") == 0)       ? VizEQ::Rock
                   : (wcscmp(eq, L"pop") == 0)        ? VizEQ::Pop
                   : (wcscmp(eq, L"jazz") == 0)       ? VizEQ::Jazz
                   : (wcscmp(eq, L"electronic") == 0) ? VizEQ::Electronic
                                                       : VizEQ::Default;
    Wh_FreeStringSetting(eq);

    g_settings.horizontalPosition = std::clamp(Wh_GetIntSetting(L"position.horizontalPosition"), 0, 100);
    g_settings.verticalPosition = std::clamp(Wh_GetIntSetting(L"position.verticalPosition"), 0, 100);
    g_settings.monitor = std::max(1, Wh_GetIntSetting(L"position.monitor"));

    PCWSTR verticalAnchor = Wh_GetStringSetting(L"appearance.verticalAnchor");
    g_settings.verticalAnchor = (wcscmp(verticalAnchor, L"top")    == 0) ? VizAnchor::Top
                              : (wcscmp(verticalAnchor, L"middle") == 0) ? VizAnchor::Middle
                                                                         : VizAnchor::Bottom;
    Wh_FreeStringSetting(verticalAnchor);

    g_settings.backgroundEnabled = Wh_GetIntSetting(L"background.enabled") != 0;

    PCWSTR bgColor = Wh_GetStringSetting(L"background.color");
    if (!ParseColorHex(bgColor, &g_settings.bgA, &g_settings.bgR, &g_settings.bgG, &g_settings.bgB)) {
        g_settings.bgA = 0x60; g_settings.bgR = g_settings.bgG = g_settings.bgB = 0;
    }
    Wh_FreeStringSetting(bgColor);

    g_settings.bgPadding = std::max(0, Wh_GetIntSetting(L"background.padding"));

    PCWSTR bgCornerRadiusStr = Wh_GetStringSetting(L"background.cornerRadius");
    {
        float v[4] = {14.0f, 14.0f, 14.0f, 14.0f};
        int n = swscanf_s(bgCornerRadiusStr, L"%f %f %f %f", &v[0], &v[1], &v[2], &v[3]);
        if (n == 1) { v[1] = v[2] = v[3] = v[0]; }
        for (int k = 0; k < 4; k++) v[k] = std::max(0.0f, v[k]);
        g_settings.bgRadiusTL = v[0];
        g_settings.bgRadiusTR = v[1];
        g_settings.bgRadiusBR = v[2];
        g_settings.bgRadiusBL = v[3];
    }
    Wh_FreeStringSetting(bgCornerRadiusStr);
    g_settings.bgBlur = std::max(0, Wh_GetIntSetting(L"background.blur"));
    g_settings.bgBorderSize = std::max(0, Wh_GetIntSetting(L"background.borderSize"));

    PCWSTR borderColor = Wh_GetStringSetting(L"background.borderColor");
    if (!ParseColorHex(borderColor, &g_settings.borderA, &g_settings.borderR, &g_settings.borderG,
                       &g_settings.borderB)) {
        g_settings.borderA = 0x40; g_settings.borderR = g_settings.borderG = g_settings.borderB = 255;
    }
    Wh_FreeStringSetting(borderColor);

    g_settings.targetFps = std::max(1, Wh_GetIntSetting(L"performance.targetFps"));
    g_settings.pauseOnFullscreen = Wh_GetIntSetting(L"performance.pauseOnFullscreen") != 0;
    g_settings.pauseWhenSilentSeconds = std::max(0, Wh_GetIntSetting(L"performance.pauseWhenSilentSeconds"));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    RefreshAccentColorCache();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hWorkerW = GetWorkerW();
    if (hWorkerW) {
        RunFromWindowThread(
            hWorkerW,
            [](void*) {
                CreateOverlayWindow();
                CreateMessageWindow();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    StopRenderThread();

    if (g_overlayWnd) SendMessage(g_overlayWnd, WM_APP_CLEANUP, 0, 0);
    if (g_messageWnd) SendMessage(g_messageWnd, WM_APP_CLEANUP, 0, 0);

    UnregisterOverlayWindowClass();
    UnregisterMessageWindowClass();

    StopVizCaptureThread();
    UninitDirectX();

    if (g_gsmtcStopEvent) {
        SetEvent(g_gsmtcStopEvent);
    }
    if (g_gsmtcThread.joinable()) {
        g_gsmtcThread.join();
    }
    if (g_gsmtcStopEvent) {
        CloseHandle(g_gsmtcStopEvent);
        g_gsmtcStopEvent = nullptr;
    }

    if (g_albumArtThread) {
        if (g_albumArtThread->joinable()) {
            HANDLE hThread = g_albumArtThread->native_handle();
            if (WaitForSingleObject(hThread, 3000) == WAIT_OBJECT_0) {
                g_albumArtThread->join();
            } else {
                g_albumArtThread->detach();
            }
        }
        delete g_albumArtThread;
        g_albumArtThread = nullptr;
    }
    g_gsmtcStarted = false;
}

void ApplySettingsChanged() {
    Wh_Log(L">");

    int oldMonitor = g_settings.monitor;
    VizColorMode oldColorMode = g_settings.colorMode;

    LoadSettings();

    if ((g_settings.colorMode == VizColorMode::AlbumArt ||
         g_settings.colorMode == VizColorMode::DynamicAlbum) &&
        ((oldColorMode != VizColorMode::AlbumArt &&
          oldColorMode != VizColorMode::DynamicAlbum) || !g_albumArtColorReady.load()))
        FetchAlbumArtColorAsync();

    if (!g_lazyInitialized || !g_initSucceeded) return;

    if (!g_fullscreenPaused.load()) {
        if (g_settings.pauseOnFullscreen && IsFullscreenOrGameActive()) {
            PauseForFullscreen();
        }
    } else if (!g_settings.pauseOnFullscreen) {
        ResumeFromFullscreen();
    }

    if (!g_overlayWnd) return;

    ReleaseVisualResources();
    RecreateVisualResources();

    if (oldMonitor != g_settings.monitor) HandleDisplayChange();

    if (!g_fullscreenPaused.load()) {
        RenderVisualizer();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_overlayWnd) {
        SendMessage(g_overlayWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else {
        ApplySettingsChanged();
    }
}
