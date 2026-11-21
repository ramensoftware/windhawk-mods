// ==WindhawkMod==
// @id              desktop-folder-portals
// @name            Desktop Folder Portals (Windows 11)
// @description     Interactive desktop folder portals with smooth morphing animations, glass blur, and file grids.
// @description:ru-RU Интерактивные папки-порталы на рабочем столе с плавной морфинг-анимацией, эффектом стекла и сеткой файлов.
// @version         1.5.0
// @author          Eswerd2324
// @github          https://github.com/Eswerd2324
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luxtheme -ldwmapi -lshlwapi -lgdiplus -lole32 -lshell32 -luuid -lcomctl32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Folder Portals

![Translucent Glass](https://raw.githubusercontent.com/Eswerd2324/PREVIEW-Desktop-Folder-Portals-/refs/heads/main/Translucent%20Glass.jpg)
*Translucent dark glass backdrop over a light desktop wallpaper*

### Dark Glass Theme
![Dark Glass](https://raw.githubusercontent.com/Eswerd2324/PREVIEW-Desktop-Folder-Portals-/refs/heads/main/Dark%20Glass.jpg)
*Dark glass styling with customizable blur*

### Folder Grid
![Folder Grid](https://raw.githubusercontent.com/Eswerd2324/PREVIEW-Desktop-Folder-Portals-/refs/heads/main/Folder%20Grid%20%282-5%20folders%29.jpg)
*Customize window size and folder grid layout*

### Folder List
![Folder Grid](https://raw.githubusercontent.com/Eswerd2324/PREVIEW-Desktop-Folder-Portals-/refs/heads/main/Folder%20List.png)
*Change the file display from grid to list*

### Hidden Scroll
![Folder Grid](https://raw.githubusercontent.com/Eswerd2324/PREVIEW-Desktop-Folder-Portals-/refs/heads/main/Hidden%20Scroll.png)
*A scrollbar appears when the cursor is hovered over it.*


# Desktop Folder Portals (Windows 11)

Desktop Folder Portals is a Windows 11 modification that brings interactive, on-hover folder content previews directly to your desktop without opening separate File Explorer windows.

### Key Features

* **Seamless Hover Previews:** Hovering over any desktop folder smoothly expands an interactive mini-window displaying its contents, collapsing once the cursor moves away.
* **Classic Shell Context Menu (`IContextMenu`):** Right-clicking an item opens the classic Win32 shell context menu (`TrackPopupMenuEx`) for standard file operations and property inspection, while selecting "Rename" opens the parent folder in File Explorer with the item selected in inline edit mode (`SHOpenFolderAndSelectItems`).
* **Effortless Drag-and-Drop:** Transfer files straight from the preview portal to the desktop, active folders, or third-party applications via native OLE Drag-and-Drop.
* **Fluent-Inspired Glass Visuals:** Styled with DWM rounded corners, drop shadows, smooth DWM background blur on levels 1–2, and Acrylic blur (`ACCENT_ENABLE_ACRYLICBLURBEHIND`) on level 3.
* **Granular Customization:** Configure hover response delays, animation duration, grid columns, and visual styling via the Windhawk settings interface.
* **Smart Item Filtering & Single-Click Launch:** Displays up to 45 items per folder (sorted with folders first), automatically skips hidden and system files, and opens any file or subfolder with a single left click.

---

# Desktop Folder Portals для Windows 11 (RU)

Desktop Folder Portals — модификация для Windows 11, внедряющая интерактивный предпросмотр содержимого папок прямо на рабочем столе без необходимости их открытия в Проводнике.

### Ключевые возможности

* **Интерактивные порталы:** Плавное появление компактного всплывающего окна с файлами при наведении курсора на директорию и его исчезновение при отведении мыши.
* **Классическое контекстное меню оболочки (`IContextMenu`):** Правый клик вызывает классическое системное контекстное меню Win32 (`TrackPopupMenuEx`) для стандартных файловых операций и просмотра свойств, а выбор пункта «Переименовать» открывает родительскую папку в Проводнике с выделением элемента в режиме редактирования имени (`SHOpenFolderAndSelectItems`).
* **Полноценный Drag-and-Drop:** Удобный перенос файлов из окна предпросмотра на рабочий стол, в открытые папки или сторонние приложения через системный OLE Drag-and-Drop.
* **Стеклянное оформление в духе Fluent:** Скругленные углы DWM, мягкие тени, гладкое DWM-размытие фона на уровнях 1–2 и эффект Acrylic (`ACCENT_ENABLE_ACRYLICBLURBEHIND`) на 3-м уровне размытия.
* **Гибкая кастомизация:** Настройка задержки срабатывания, длительности анимации, числа колонок сетки и параметров прозрачности в панели Windhawk.
* **Умная выборка и запуск в один клик:** Отображает до 45 элементов папки (с приоритетной сортировкой подпапок), автоматически пропускает скрытые и системные файлы и позволяет открывать любые файлы или вложенные папки одиночным левым кликом мыши.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hoverDelayMs: 220
  $name: Hover Delay (ms)
  $name:ru-RU: Задержка срабатывания (мс)
  $description: Time to hold the cursor over a folder before the portal opens (50-2000 ms).
  $description:ru-RU: Время удержания курсора над папкой до открытия портала (50-2000 мс).
- animDurationMs: 150
  $name: Animation Duration (ms)
  $name:ru-RU: Длительность анимации (мс)
  $description: Opening and closing animation duration (60-1000 ms).
  $description:ru-RU: Время анимации открытия и закрытия (60-1000 мс).
- portalWidth: 340
  $name: Portal Window Width (px)
  $name:ru-RU: Ширина окна портала (px)
  $description: Base width of the popup mini-window in pixels (at 96 DPI).
  $description:ru-RU: Ширина всплывающего мини-окна в пикселях.
- portalHeight: 280
  $name: Portal Window Height (px)
  $name:ru-RU: Высота окна портала (px)
  $description: Base height of the popup mini-window in pixels (at 96 DPI).
  $description:ru-RU: Высота всплывающего мини-окна в пикселях.
- bgOpacity: 195
  $name: Window Background Opacity (10-255)
  $name:ru-RU: Непрозрачность фона окна (10-255)
  $description: Opacity of the window backdrop only (icons and text always remain 100% crisp and bright).
  $description:ru-RU: Прозрачность только подложки окна (иконки и текст всегда остаются 100% четкими и яркими).
- enableBlur: true
  $name: Enable Glass Blur
  $name:ru-RU: Включить размытие стекла (Blur)
  $description: Enables smooth, grain-free glass background blur.
  $description:ru-RU: Включает гладкое стеклянное размытие фона без зернистости.
- blurStrength: level2
  $name: Blur Strength (1, 2, 3)
  $name:ru-RU: Сила размытия (1, 2, 3)
  $options:
    - level1: Level 1 (Soft Glass)
    - level2: Level 2 (Medium Glass)
    - level3: Level 3 (Deep Frosted Glass)
  $options:ru-RU:
    - level1: Уровень 1 (Мягкое стекло)
    - level2: Уровень 2 (Среднее стекло)
    - level3: Уровень 3 (Глубокое матовое стекло)
- borderBrightness: 85
  $name: Border Brightness (0-255)
  $name:ru-RU: Яркость тонкой светлой рамки (0-255)
  $description: Intensity of the 1px contour border around the portal.
  $description:ru-RU: Интенсивность контурной рамки толщиной 1px вокруг портала.
- gridColumns: 3
  $name: File Grid Columns
  $name:ru-RU: Колонок в сетке файлов
  $description: Number of items per row inside the portal (from 2 to 5).
  $description:ru-RU: Количество элементов в одном ряду внутри портала (от 2 до 5).
- showFileMetadata: true
  $name: Show File Metadata
  $name:ru-RU: Показывать размер файлов
  $description: Displays file size or "Folder" label underneath the item name.
  $description:ru-RU: Отображает подпись с размером файла или типом "Папка" под именем.
- viewMode: grid
  $name: Portal View Mode
  $name:ru-RU: Режим отображения файлов
  $description: Choose whether folder contents inside the portal are displayed as a grid or a vertical list.
  $description:ru-RU: Выберите способ отображения содержимого папки внутри портала — сеткой или вертикальным списком.
  $options:
    - grid: Grid
    - list: List
  $options:ru-RU:
    - grid: Сетка
    - list: Список
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <commoncontrols.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <cmath>
#include <algorithm>
#include <functional>
#include <thread>
#include <condition_variable>
#include <deque>
#include <list>
#include <unordered_map>
#include <memory>

#include <windhawk_utils.h>

using namespace Gdiplus;

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

typedef BOOL(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

constexpr UINT_PTR TIMER_ID_MOUSECHECK   = 1002;
constexpr UINT_PTR TIMER_ID_FRAME_RENDER = 1003;
constexpr UINT_PTR TIMER_ID_SCROLL_IDLE  = 1004;
constexpr UINT     WM_APP_PORTAL_ASYNC_READY = WM_APP + 101;
constexpr UINT     WM_APP_PORTAL_VSYNC_TICK  = WM_APP + 102;
constexpr wchar_t  PORTAL_CLASS_NAME[]   = L"WindhawkDesktopFolderPortalWnd";

struct ModSettings {
    int hoverDelayMs = 220;
    int animDurationMs = 150;
    int portalWidth = 340;
    int portalHeight = 280;
    BYTE bgOpacity = 195;
    bool enableBlur = true;
    int blurStrength = 2; // 1, 2, 3
    int borderBrightness = 85;
    int gridColumns = 3;
    bool showFileMetadata = true;
    bool listViewMode = false;
} g_settings;

std::mutex g_stateMutex;
std::mutex g_subclassMutex;
std::unordered_set<HWND> g_subclassedWindows;
std::atomic<bool> g_unloading{false};
std::atomic<int> g_activeModalCallCount{0};
ULONG_PTR g_gdiplusToken = 0;
HINSTANCE g_hInstance = nullptr;
bool g_portalClassRegistered = false;
pfnSetWindowCompositionAttribute g_pSetWCA = nullptr;

// RAII-сторож для отслеживания активных модальных циклов (контекстное меню, InvokeCommand, Drag-and-Drop)
struct ScopedModalCallGuard {
    ScopedModalCallGuard() noexcept {
        g_activeModalCallCount.fetch_add(1, std::memory_order_acq_rel);
    }
    ~ScopedModalCallGuard() noexcept {
        g_activeModalCallCount.fetch_sub(1, std::memory_order_acq_rel);
    }
    ScopedModalCallGuard(const ScopedModalCallGuard&) = delete;
    ScopedModalCallGuard& operator=(const ScopedModalCallGuard&) = delete;
};

// Определение языка интерфейса пользователя для локализации строк внутри портала (кэшировано)
bool IsRussianUILanguage() {
    static const bool s_isRu = (PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_RUSSIAN);
    return s_isRu;
}

// Получение реального DPI целевого монитора через GetDpiForMonitor с резервным фоллбэком
int GetMonitorDpi(HMONITOR hMon) {
    if (hMon) {
        using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
        static GetDpiForMonitor_t pGetDpiForMonitor = []() -> GetDpiForMonitor_t {
            HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
            if (!hShcore) {
                hShcore = LoadLibraryW(L"shcore.dll");
            }
            return hShcore ? reinterpret_cast<GetDpiForMonitor_t>(GetProcAddress(hShcore, "GetDpiForMonitor")) : nullptr;
        }();

        if (pGetDpiForMonitor) {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(pGetDpiForMonitor(hMon, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY)) && dpiX > 0) {
                return static_cast<int>(dpiX);
            }
        }
    }
    HDC hdc = GetDC(nullptr);
    int dpi = 96;
    if (hdc) {
        dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(nullptr, hdc);
    }
    return dpi > 0 ? dpi : 96;
}

inline int ScaleByDpi(int val, int dpi) {
    return MulDiv(val, dpi, 96);
}

// Высокоточный таймер на базе QueryPerformanceCounter (без 15.6 мс квантования GetTickCount64)
inline double GetQpcNowMs() noexcept {
    static const double s_invFreqMs = []() -> double {
        LARGE_INTEGER freq = {};
        QueryPerformanceFrequency(&freq);
        return (freq.QuadPart > 0) ? (1000.0 / static_cast<double>(freq.QuadPart)) : 0.0;
    }();
    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart) * s_invFreqMs;
}

// Легковесный VSync-триггер на базе DwmFlush() в спящем потоке (пробуждается только на время Opening/Closing)
class PortalVsyncAnimator {
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_thread;
    HWND m_targetWnd = nullptr;
    DWORD m_fallbackIntervalMs = 16;
    bool m_active = false;
    bool m_stop = false;
    std::atomic<bool> m_tickPending{false};

    void WorkerLoop() {
        for (;;) {
            HWND hWnd = nullptr;
            DWORD fallbackMs = 16;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]() { return m_stop || m_active; });
                if (m_stop) break;
                hWnd = m_targetWnd;
                fallbackMs = m_fallbackIntervalMs;
            }

            const double t0 = GetQpcNowMs();
            const HRESULT hr = DwmFlush();
            const double elapsed = GetQpcNowMs() - t0;
            if (FAILED(hr) || elapsed < 0.5) {
                Sleep(fallbackMs);
            }

            if (g_unloading.load(std::memory_order_acquire)) break;

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_stop || !m_active || m_targetWnd != hWnd) continue;
            }

            if (!hWnd || !IsWindow(hWnd)) {
                StopAnimation();
                continue;
            }

            if (!m_tickPending.exchange(true, std::memory_order_acq_rel)) {
                if (!PostMessageW(hWnd, WM_APP_PORTAL_VSYNC_TICK, 0, 0)) {
                    m_tickPending.store(false, std::memory_order_release);
                }
            }
        }
    }

public:
    void StartThread() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_thread.joinable()) return;
        m_stop = false;
        m_active = false;
        m_targetWnd = nullptr;
        m_tickPending.store(false, std::memory_order_release);
        m_thread = std::thread(&PortalVsyncAnimator::WorkerLoop, this);
    }

    void StopThread() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = true;
            m_active = false;
            m_targetWnd = nullptr;
        }
        m_tickPending.store(false, std::memory_order_release);
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    void StartAnimation(HWND hWnd, UINT fallbackIntervalMs) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stop) return;
            m_targetWnd = hWnd;
            m_fallbackIntervalMs = std::clamp<DWORD>(fallbackIntervalMs, 4u, 16u);
            m_active = true;
        }
        m_tickPending.store(false, std::memory_order_release);
        m_cv.notify_one();
    }

    void StopAnimation() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_active = false;
        m_targetWnd = nullptr;
        m_tickPending.store(false, std::memory_order_release);
    }

    void AcknowledgeTick() noexcept {
        m_tickPending.store(false, std::memory_order_release);
    }
} g_vsyncAnimator;

// Точное целочисленное деление на 255 с округлением без аппаратного деления (v in [0..65025])
inline uint32_t FastDiv255(uint32_t v) noexcept {
    const uint32_t t = v + 128u;
    return (t + (t >> 8)) >> 8;
}

// Запечённая premultiplied 32-bit BGRA поверхность иконки (thread-safe, zero-copy в LRU-кэше)
struct BakedIcon {
    int size = 0;
    std::vector<uint32_t> pixels;
};

// Переиспользуемый буфер-скретч для запекания иконок без повторного создания DIB
struct IconBakeScratch {
    HDC hdc = nullptr;
    HBITMAP hbm = nullptr;
    HGDIOBJ hOld = nullptr;
    void* pvBits = nullptr;
    int bufSize = 0;

    ~IconBakeScratch() { Release(); }

    bool Ensure(int size) {
        if (hdc && hbm && pvBits && bufSize >= size) return true;
        Release();

        HDC hdcScreen = GetDC(nullptr);
        if (!hdcScreen) return false;
        hdc = CreateCompatibleDC(hdcScreen);
        ReleaseDC(nullptr, hdcScreen);
        if (!hdc) return false;

        bufSize = std::max<int>(size, 64);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = bufSize;
        bmi.bmiHeader.biHeight = -bufSize;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
        if (!hbm || !pvBits) {
            Release();
            return false;
        }
        hOld = SelectObject(hdc, hbm);
        return true;
    }

    void Release() {
        if (hdc && hOld) {
            SelectObject(hdc, hOld);
            hOld = nullptr;
        }
        if (hbm) {
            DeleteObject(hbm);
            hbm = nullptr;
        }
        if (hdc) {
            DeleteDC(hdc);
            hdc = nullptr;
        }
        pvBits = nullptr;
        bufSize = 0;
    }
};

std::shared_ptr<BakedIcon> BakeIconToPremultipliedPixels(HICON hIcon, int iconSize, IconBakeScratch& scratch) {
    if (!hIcon || iconSize <= 0 || !scratch.Ensure(iconSize)) return nullptr;

    BYTE* bits = static_cast<BYTE*>(scratch.pvBits);
    const int stride = scratch.bufSize * 4;
    for (int y = 0; y < iconSize; ++y) {
        memset(bits + y * stride, 0, static_cast<size_t>(iconSize) * 4);
    }

    DrawIconEx(scratch.hdc, 0, 0, hIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
    GdiFlush();

    bool hasAlpha = false;
    for (int y = 0; y < iconSize && !hasAlpha; ++y) {
        const BYTE* row = bits + y * stride;
        for (int x = 0; x < iconSize; ++x) {
            if (row[x * 4 + 3] != 0) {
                hasAlpha = true;
                break;
            }
        }
    }

    auto baked = std::make_shared<BakedIcon>();
    baked->size = iconSize;
    baked->pixels.resize(static_cast<size_t>(iconSize) * static_cast<size_t>(iconSize));
    uint32_t* dst = baked->pixels.data();

    for (int y = 0; y < iconSize; ++y) {
        const BYTE* row = bits + y * stride;
        uint32_t* dstRow = dst + static_cast<size_t>(y) * static_cast<size_t>(iconSize);
        if (hasAlpha) {
            for (int x = 0; x < iconSize; ++x) {
                const BYTE b = row[x * 4 + 0];
                const BYTE g = row[x * 4 + 1];
                const BYTE r = row[x * 4 + 2];
                const BYTE a = row[x * 4 + 3];
                const BYTE pb = (b > a) ? a : b;
                const BYTE pg = (g > a) ? a : g;
                const BYTE pr = (r > a) ? a : r;
                dstRow[x] = static_cast<uint32_t>(pb) |
                            (static_cast<uint32_t>(pg) << 8) |
                            (static_cast<uint32_t>(pr) << 16) |
                            (static_cast<uint32_t>(a) << 24);
            }
        } else {
            for (int x = 0; x < iconSize; ++x) {
                const BYTE b = row[x * 4 + 0];
                const BYTE g = row[x * 4 + 1];
                const BYTE r = row[x * 4 + 2];
                const BYTE a = (b != 0 || g != 0 || r != 0) ? 255 : 0;
                dstRow[x] = static_cast<uint32_t>(b) |
                            (static_cast<uint32_t>(g) << 8) |
                            (static_cast<uint32_t>(r) << 16) |
                            (static_cast<uint32_t>(a) << 24);
            }
        }
    }
    return baked;
}

// Контекст и строго типизированная stdcall-процедура хука для совместимости с x86 и x64 компилятором Windhawk
struct RunOnWndThreadContext {
    const std::function<void()>* pFunc;
    bool executed;
};

LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        static const UINT uMsgId = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_DesktopFolderPortals");
        if (cwp && cwp->message == uMsgId && cwp->lParam != 0) {
            auto* pCtx = reinterpret_cast<RunOnWndThreadContext*>(cwp->lParam);
            if (!pCtx->executed) {
                pCtx->executed = true;
                (*pCtx->pFunc)();
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Вспомогательная функция для безопасного синхронного выполнения кода в потоке целевого окна
bool RunFromWindowThread(HWND hWnd, const std::function<void()>& func) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }

    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        func();
        return true;
    }

    static const UINT uExecMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_DesktopFolderPortals");
    if (!uExecMsg) {
        return false;
    }

    RunOnWndThreadContext ctx{ &func, false };

    HHOOK hHook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        RunFromWindowThreadHookProc,
        nullptr,
        dwThreadId);

    if (!hHook) {
        return false;
    }

    SendMessageW(hWnd, uExecMsg, 0, reinterpret_cast<LPARAM>(&ctx));
    UnhookWindowsHookEx(hHook);
    return ctx.executed;
}

// Кэшированные ресурсы GDI/GDI+ для мгновенного рендеринга без пересоздания на каждом кадре
struct RenderCache {
    HDC hdcMem = nullptr;
    HBITMAP hbmMem = nullptr;
    HGDIOBJ hOldBmp = nullptr;
    void* pvBits = nullptr;
    int bufW = 0;
    int bufH = 0;
    int fontDpi = 0;
    int primDpi = 0;

    HDC hdcContent = nullptr;
    HBITMAP hbmContent = nullptr;
    HGDIOBJ hOldContentBmp = nullptr;
    void* pvContentBits = nullptr;
    int contentBufW = 0;
    int contentBufH = 0;
    int cachedContentW = 0;
    int cachedContentH = 0;
    int cachedContentDpi = 0;
    int cachedContentScrollY = -1;
    uint64_t cachedContentRevision = 0;

    IconBakeScratch uiIconScratch;

    FontFamily* pFamily = nullptr;
    Gdiplus::Font* titleFont = nullptr;
    Gdiplus::Font* countFont = nullptr;
    Gdiplus::Font* itemFont = nullptr;
    Gdiplus::Font* metaFont = nullptr;

    // Кэшированные форматы текста, кисти, перья и пути
    StringFormat* centerFormat = nullptr;
    StringFormat* itemFormat = nullptr;
    StringFormat* leftFormat = nullptr;
    StringFormat* rightFormat = nullptr;

    SolidBrush* bgBrush = nullptr;
    SolidBrush* titleBrush = nullptr;
    SolidBrush* subBrush = nullptr;
    SolidBrush* hoverFillBrush = nullptr;
    SolidBrush* placeholderBrush = nullptr;
    SolidBrush* scrollbarBrush = nullptr;

    Pen* hoverBorderPen = nullptr;
    Pen* headerLinePen = nullptr;
    Pen* lightBorderPen = nullptr;

    GraphicsPath* borderPath = nullptr;
    int borderPathW = 0;
    int borderPathH = 0;
    REAL borderPathRadius = -1.0f;
    REAL borderPathInset = -1.0f;

    GraphicsPath* hoverPath = nullptr;
    int hoverPathW = 0;
    int hoverPathH = 0;
    int hoverPathRad = 0;

    GraphicsPath* placeholderPath = nullptr;
    int placeholderSize = 0;
    int placeholderRad = 0;

    GraphicsPath* scrollbarPath = nullptr;

    void ReleaseFonts() {
        delete titleFont; titleFont = nullptr;
        delete countFont; countFont = nullptr;
        delete itemFont;  itemFont = nullptr;
        delete metaFont;  metaFont = nullptr;
        delete pFamily;   pFamily = nullptr;
        fontDpi = 0;
    }

    void ReleasePrimitives() {
        delete centerFormat; centerFormat = nullptr;
        delete itemFormat;   itemFormat = nullptr;
        delete leftFormat;   leftFormat = nullptr;
        delete rightFormat;  rightFormat = nullptr;

        delete bgBrush;          bgBrush = nullptr;
        delete titleBrush;       titleBrush = nullptr;
        delete subBrush;         subBrush = nullptr;
        delete hoverFillBrush;   hoverFillBrush = nullptr;
        delete placeholderBrush; placeholderBrush = nullptr;
        delete scrollbarBrush;   scrollbarBrush = nullptr;

        delete hoverBorderPen; hoverBorderPen = nullptr;
        delete headerLinePen;  headerLinePen = nullptr;
        delete lightBorderPen; lightBorderPen = nullptr;

        delete borderPath;      borderPath = nullptr;
        delete hoverPath;       hoverPath = nullptr;
        delete placeholderPath; placeholderPath = nullptr;
        delete scrollbarPath;   scrollbarPath = nullptr;

        primDpi = 0;
        borderPathW = borderPathH = 0;
        borderPathRadius = -1.0f;
        borderPathInset = -1.0f;
        hoverPathW = hoverPathH = hoverPathRad = 0;
        placeholderSize = placeholderRad = 0;
    }

    void EnsureFonts(int dpi) {
        if (titleFont && fontDpi == dpi) return;
        ReleaseFonts();
        fontDpi = dpi;
        pFamily = new FontFamily(L"Segoe UI Variable Text");
        const FontFamily* fam = pFamily->IsAvailable() ? pFamily : FontFamily::GenericSansSerif();
        titleFont = new Gdiplus::Font(fam, static_cast<REAL>(MulDiv(14, dpi, 96)), FontStyleRegular, UnitPixel);
        countFont = new Gdiplus::Font(fam, static_cast<REAL>(MulDiv(11, dpi, 96)), FontStyleRegular, UnitPixel);
        itemFont  = new Gdiplus::Font(fam, static_cast<REAL>(MulDiv(11, dpi, 96)), FontStyleRegular, UnitPixel);
        metaFont  = new Gdiplus::Font(fam, static_cast<REAL>(MulDiv(11, dpi, 96)), FontStyleRegular, UnitPixel);
    }

    void EnsurePrimitives(int dpi) {
        if (!centerFormat) {
            centerFormat = new StringFormat();
            centerFormat->SetAlignment(StringAlignmentCenter);
            centerFormat->SetLineAlignment(StringAlignmentCenter);

            itemFormat = new StringFormat();
            itemFormat->SetAlignment(StringAlignmentCenter);
            itemFormat->SetLineAlignment(StringAlignmentNear);
            itemFormat->SetTrimming(StringTrimmingEllipsisCharacter);
            itemFormat->SetFormatFlags(StringFormatFlagsNoWrap);

            leftFormat = new StringFormat();
            leftFormat->SetAlignment(StringAlignmentNear);
            leftFormat->SetLineAlignment(StringAlignmentCenter);
            leftFormat->SetTrimming(StringTrimmingEllipsisCharacter);
            leftFormat->SetFormatFlags(StringFormatFlagsNoWrap);

            rightFormat = new StringFormat();
            rightFormat->SetAlignment(StringAlignmentFar);
            rightFormat->SetLineAlignment(StringAlignmentCenter);
            rightFormat->SetTrimming(StringTrimmingEllipsisCharacter);
            rightFormat->SetFormatFlags(StringFormatFlagsNoWrap);

            bgBrush          = new SolidBrush(Color(195, 26, 27, 32));
            titleBrush       = new SolidBrush(Color(255, 248, 250, 255));
            subBrush         = new SolidBrush(Color(204, 248, 250, 255));
            hoverFillBrush   = new SolidBrush(Color(50, 255, 255, 255));
            placeholderBrush = new SolidBrush(Color(34, 255, 255, 255));
            scrollbarBrush   = new SolidBrush(Color(115, 255, 255, 255));

            hoverBorderPen = new Pen(Color(95, 255, 255, 255), 1.0f);
            headerLinePen  = new Pen(Color(38, 255, 255, 255), 1.0f);
            lightBorderPen = new Pen(Color(85, 240, 246, 255), 1.2f);

            borderPath      = new GraphicsPath();
            hoverPath       = new GraphicsPath();
            placeholderPath = new GraphicsPath();
            scrollbarPath   = new GraphicsPath();
        }
        if (primDpi != dpi) {
            primDpi = dpi;
            REAL pen1px = static_cast<REAL>(ScaleByDpi(1, dpi));
            hoverBorderPen->SetWidth(pen1px);
            headerLinePen->SetWidth(pen1px);
            lightBorderPen->SetWidth(1.2f * (static_cast<float>(dpi) / 96.0f));
        }
    }

    static void BuildRoundedRectPath(GraphicsPath* path, int x, int y, int w, int h, int diameter) {
        path->Reset();
        if (w <= 0 || h <= 0) return;
        int d = std::min<int>(diameter, std::min<int>(w, h));
        if (d <= 1) {
            path->AddRectangle(Rect(x, y, w, h));
            return;
        }
        path->AddArc(x, y, d, d, 180.0f, 90.0f);
        path->AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
        path->AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
        path->AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
        path->CloseFigure();
    }

    static void BuildRoundedRectPathF(GraphicsPath* path, REAL x, REAL y, REAL w, REAL h, REAL radius) {
        path->Reset();
        if (w <= 0.0f || h <= 0.0f) return;
        REAL maxR = std::min<REAL>(w, h) * 0.5f;
        REAL r = std::clamp<REAL>(radius, 0.0f, maxR);
        REAL d = r * 2.0f;
        if (d <= 0.5f) {
            path->AddRectangle(RectF(x, y, w, h));
            return;
        }
        path->AddArc(x, y, d, d, 180.0f, 90.0f);
        path->AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
        path->AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
        path->AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
        path->CloseFigure();
    }

    GraphicsPath* GetBorderPath(int width, int height, REAL radius, REAL inset) {
        if (!borderPath) return nullptr;
        if (borderPathW != width || borderPathH != height ||
            std::fabs(borderPathRadius - radius) > 0.05f ||
            std::fabs(borderPathInset - inset) > 0.05f) {
            borderPathW = width;
            borderPathH = height;
            borderPathRadius = radius;
            borderPathInset = inset;
            BuildRoundedRectPathF(
                borderPath,
                inset,
                inset,
                std::max<REAL>(1.0f, static_cast<REAL>(width) - inset * 2.0f),
                std::max<REAL>(1.0f, static_cast<REAL>(height) - inset * 2.0f),
                radius);
        }
        return borderPath;
    }

    GraphicsPath* GetHoverPath(int w, int h, int rad) {
        if (!hoverPath) return nullptr;
        if (hoverPathW != w || hoverPathH != h || hoverPathRad != rad) {
            hoverPathW = w;
            hoverPathH = h;
            hoverPathRad = rad;
            BuildRoundedRectPath(hoverPath, 0, 0, w, h, rad);
        }
        return hoverPath;
    }

    GraphicsPath* GetPlaceholderPath(int size, int rad) {
        if (!placeholderPath) return nullptr;
        if (placeholderSize != size || placeholderRad != rad) {
            placeholderSize = size;
            placeholderRad = rad;
            BuildRoundedRectPath(placeholderPath, 0, 0, size, size, rad);
        }
        return placeholderPath;
    }

    bool EnsureBitmap(int w, int h) {
        if (hdcMem && hbmMem && pvBits && bufW >= w && bufH >= h) {
            return true;
        }
        ReleaseBitmap();

        HDC hdcScreen = GetDC(nullptr);
        if (!hdcScreen) return false;
        hdcMem = CreateCompatibleDC(hdcScreen);
        ReleaseDC(nullptr, hdcScreen);
        if (!hdcMem) return false;

        bufW = std::max<int>(w, 640);
        bufH = std::max<int>(h, 560);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = bufW;
        bmi.bmiHeader.biHeight = -bufH;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        hbmMem = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
        if (!hbmMem || !pvBits) {
            ReleaseBitmap();
            return false;
        }
        hOldBmp = SelectObject(hdcMem, hbmMem);
        return true;
    }

    bool EnsureContentBitmap(int w, int h) {
        if (hdcContent && hbmContent && pvContentBits && contentBufW >= w && contentBufH >= h) {
            return true;
        }
        ReleaseContentBitmap();

        HDC hdcScreen = GetDC(nullptr);
        if (!hdcScreen) return false;
        hdcContent = CreateCompatibleDC(hdcScreen);
        ReleaseDC(nullptr, hdcScreen);
        if (!hdcContent) return false;

        contentBufW = std::max<int>(w, 640);
        contentBufH = std::max<int>(h, 560);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = contentBufW;
        bmi.bmiHeader.biHeight = -contentBufH;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        hbmContent = CreateDIBSection(hdcContent, &bmi, DIB_RGB_COLORS, &pvContentBits, nullptr, 0);
        if (!hbmContent || !pvContentBits) {
            ReleaseContentBitmap();
            return false;
        }
        hOldContentBmp = SelectObject(hdcContent, hbmContent);
        return true;
    }

    void InvalidateContentCache() {
        cachedContentW = 0;
        cachedContentH = 0;
        cachedContentDpi = 0;
        cachedContentScrollY = -1;
        cachedContentRevision = 0;
    }

    void ReleaseBitmap() {
        if (hdcMem && hOldBmp) {
            SelectObject(hdcMem, hOldBmp);
            hOldBmp = nullptr;
        }
        if (hbmMem) {
            DeleteObject(hbmMem);
            hbmMem = nullptr;
        }
        if (hdcMem) {
            DeleteDC(hdcMem);
            hdcMem = nullptr;
        }
        pvBits = nullptr;
        bufW = 0;
        bufH = 0;
    }

    void ReleaseContentBitmap() {
        if (hdcContent && hOldContentBmp) {
            SelectObject(hdcContent, hOldContentBmp);
            hOldContentBmp = nullptr;
        }
        if (hbmContent) {
            DeleteObject(hbmContent);
            hbmContent = nullptr;
        }
        if (hdcContent) {
            DeleteDC(hdcContent);
            hdcContent = nullptr;
        }
        pvContentBits = nullptr;
        contentBufW = 0;
        contentBufH = 0;
        InvalidateContentCache();
    }

    void ReleaseAll() {
        ReleaseBitmap();
        ReleaseContentBitmap();
        uiIconScratch.Release();
        ReleasePrimitives();
        ReleaseFonts();
    }
} g_renderCache;

// ---------------------------------------------------------------------------
// Минималистичная реализация IDropSource для системного Drag-and-Drop
// ---------------------------------------------------------------------------
class PortalDropSource final : public IDropSource {
    LONG m_ref = 1;
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropSource)) {
            *ppv = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (r == 0) delete this;
        return r;
    }
    STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) override {
        if (fEscapePressed) return DRAGDROP_S_CANCEL;
        if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON))) return DRAGDROP_S_DROP;
        return S_OK;
    }
    STDMETHODIMP GiveFeedback(DWORD) override {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
};

// ---------------------------------------------------------------------------
// Структура элемента внутри папки-портала
// ---------------------------------------------------------------------------
struct PortalFileItem {
    std::wstring name;
    std::wstring fullPath;
    std::wstring sizeOrTypeText;
    ULONGLONG rawFileSize = 0;
    std::shared_ptr<BakedIcon> bakedIcon;
    bool isDirectory = false;
    bool iconLoaded = false;
    bool iconRequested = false;
    RECT boundsRect = {};
};

std::atomic<uint64_t> g_portalGenerationId{0};

// Извлечение системной иконки элемента с кэшированием IImageList и учетом DPI (выполняется в фоновом потоке)
HICON ExtractItemIconOnWorker(
    const std::wstring& fullPath,
    const std::wstring& name,
    bool isDirectory,
    int targetIconSize,
    IImageList* cachedImageLists[3] = nullptr)
{
    if (fullPath.empty()) return nullptr;

    DWORD attrs = isDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
    SHFILEINFOW sfi = {};

    bool needsDiskProbe = false;
    if (!isDirectory) {
        LPCWSTR ext = PathFindExtensionW(name.c_str());
        if (ext && (_wcsicmp(ext, L".exe") == 0 ||
                    _wcsicmp(ext, L".lnk") == 0 ||
                    _wcsicmp(ext, L".ico") == 0 ||
                    _wcsicmp(ext, L".url") == 0)) {
            needsDiskProbe = true;
        }
    }

    UINT flags = SHGFI_SYSICONINDEX | (needsDiskProbe ? 0 : SHGFI_USEFILEATTRIBUTES);
    if (!SHGetFileInfoW(fullPath.c_str(), attrs, &sfi, sizeof(sfi), flags)) {
        SHGetFileInfoW(fullPath.c_str(), attrs, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
    }

    HICON hResult = nullptr;
    int slot = (targetIconSize > 48) ? 2 : ((targetIconSize > 32) ? 1 : 0);
    int iilSize = (slot == 2) ? SHIL_JUMBO : ((slot == 1) ? SHIL_EXTRALARGE : SHIL_LARGE);

    IImageList* pImgList = nullptr;
    bool ownsList = false;
    if (cachedImageLists) {
        if (!cachedImageLists[slot]) {
            SHGetImageList(iilSize, __uuidof(IImageList), reinterpret_cast<void**>(&cachedImageLists[slot]));
        }
        pImgList = cachedImageLists[slot];
    } else {
        if (SUCCEEDED(SHGetImageList(iilSize, __uuidof(IImageList), reinterpret_cast<void**>(&pImgList))) && pImgList) {
            ownsList = true;
        }
    }

    if (pImgList) {
        HICON hExtracted = nullptr;
        if (SUCCEEDED(pImgList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hExtracted)) && hExtracted) {
            hResult = hExtracted;
        }
        if (ownsList) {
            pImgList->Release();
        }
    }

    if (!hResult) {
        if (SHGetFileInfoW(fullPath.c_str(), attrs, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | (needsDiskProbe ? 0 : SHGFI_USEFILEATTRIBUTES))) {
            hResult = sfi.hIcon;
        } else if (SHGetFileInfoW(fullPath.c_str(), attrs, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES)) {
            hResult = sfi.hIcon;
        }
    }
    return hResult;
}

void EnsureItemIconLoaded(PortalFileItem& item, int targetIconSize) {
    if (item.iconLoaded && item.bakedIcon && item.bakedIcon->size == targetIconSize) return;
    item.iconLoaded = true;
    item.iconRequested = true;
    HICON hIcon = ExtractItemIconOnWorker(item.fullPath, item.name, item.isDirectory, targetIconSize);
    if (hIcon) {
        item.bakedIcon = BakeIconToPremultipliedPixels(hIcon, targetIconSize, g_renderCache.uiIconScratch);
        DestroyIcon(hIcon);
    }
}

// ---------------------------------------------------------------------------
// LRU-кэш директорий (последние 8 папок) с отслеживанием изменений через FindFirstChangeNotificationW
// и zero-copy разделяемыми запечёнными иконками BakedIcon
// ---------------------------------------------------------------------------
std::wstring NormalizeFolderPathKey(const std::wstring& path) {
    std::wstring key = path;
    while (key.size() > 3 && (key.back() == L'\\' || key.back() == L'/')) {
        key.pop_back();
    }
    if (!key.empty()) {
        CharLowerBuffW(key.data(), static_cast<DWORD>(key.size()));
    }
    return key;
}

class DirectoryLruCache {
    static constexpr size_t MAX_ENTRIES = 8;

    struct CachedItem {
        std::wstring name;
        std::wstring fullPath;
        std::wstring sizeOrTypeText;
        ULONGLONG rawFileSize = 0;
        std::shared_ptr<BakedIcon> bakedIcon;
        bool isDirectory = false;
        bool iconLoaded = false;
    };

    struct CacheEntry {
        std::wstring pathKey;
        std::wstring folderPath;
        size_t totalItemCount = 0;
        int iconSize = 0;
        HANDLE hChangeNotif = INVALID_HANDLE_VALUE;
        std::vector<CachedItem> items;

        ~CacheEntry() {
            if (hChangeNotif != INVALID_HANDLE_VALUE) {
                FindCloseChangeNotification(hChangeNotif);
                hChangeNotif = INVALID_HANDLE_VALUE;
            }
        }
        CacheEntry() = default;
        CacheEntry(const CacheEntry&) = delete;
        CacheEntry& operator=(const CacheEntry&) = delete;
        CacheEntry(CacheEntry&& other) noexcept { *this = std::move(other); }
        CacheEntry& operator=(CacheEntry&& other) noexcept {
            if (this != &other) {
                if (hChangeNotif != INVALID_HANDLE_VALUE) {
                    FindCloseChangeNotification(hChangeNotif);
                }
                pathKey = std::move(other.pathKey);
                folderPath = std::move(other.folderPath);
                totalItemCount = other.totalItemCount;
                iconSize = other.iconSize;
                hChangeNotif = other.hChangeNotif;
                items = std::move(other.items);
                other.hChangeNotif = INVALID_HANDLE_VALUE;
            }
            return *this;
        }
    };

    std::mutex m_mutex;
    std::list<CacheEntry> m_lru;
    std::unordered_map<std::wstring, std::list<CacheEntry>::iterator> m_index;

public:
    bool TryGet(const std::wstring& folderPath, int targetIconSize, std::vector<PortalFileItem>& outItems, size_t& outTotalCount) {
        std::wstring key = NormalizeFolderPathKey(folderPath);
        if (key.empty()) return false;

        std::lock_guard<std::mutex> lock(m_mutex);
        auto mapIt = m_index.find(key);
        if (mapIt == m_index.end()) return false;

        auto listIt = mapIt->second;
        // Проверяем, не изменилось ли содержимое папки с момента кэширования
        if (listIt->hChangeNotif == INVALID_HANDLE_VALUE ||
            WaitForSingleObject(listIt->hChangeNotif, 0) != WAIT_TIMEOUT) {
            m_index.erase(mapIt);
            m_lru.erase(listIt);
            return false;
        }

        // Перемещаем запись в начало LRU
        m_lru.splice(m_lru.begin(), m_lru, listIt);

        const bool sameIconSize = (listIt->iconSize == targetIconSize);
        if (!sameIconSize) {
            listIt->iconSize = targetIconSize;
            for (auto& ci : listIt->items) {
                ci.bakedIcon.reset();
                ci.iconLoaded = false;
            }
        }

        outItems.clear();
        outItems.reserve(listIt->items.size());
        for (const auto& ci : listIt->items) {
            PortalFileItem pi;
            pi.name = ci.name;
            pi.fullPath = ci.fullPath;
            pi.sizeOrTypeText = ci.sizeOrTypeText;
            pi.rawFileSize = ci.rawFileSize;
            pi.isDirectory = ci.isDirectory;
            if (sameIconSize && ci.iconLoaded) {
                pi.bakedIcon = ci.bakedIcon;
                pi.iconLoaded = true;
                pi.iconRequested = true;
            }
            outItems.push_back(std::move(pi));
        }
        outTotalCount = listIt->totalItemCount;
        return true;
    }

    void Put(const std::wstring& folderPath, int iconSize, const std::vector<PortalFileItem>& items, size_t totalCount) {
        std::wstring key = NormalizeFolderPathKey(folderPath);
        if (key.empty()) return;

        HANDLE hNotif = FindFirstChangeNotificationW(
            folderPath.c_str(),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_ATTRIBUTES);

        if (hNotif == INVALID_HANDLE_VALUE) {
            return;
        }

        CacheEntry entry;
        entry.pathKey = key;
        entry.folderPath = folderPath;
        entry.totalItemCount = totalCount;
        entry.iconSize = iconSize;
        entry.hChangeNotif = hNotif;
        entry.items.reserve(items.size());
        for (const auto& item : items) {
            CachedItem ci;
            ci.name = item.name;
            ci.fullPath = item.fullPath;
            ci.sizeOrTypeText = item.sizeOrTypeText;
            ci.rawFileSize = item.rawFileSize;
            ci.isDirectory = item.isDirectory;
            ci.iconLoaded = item.iconLoaded;
            ci.bakedIcon = item.bakedIcon;
            entry.items.push_back(std::move(ci));
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        auto existing = m_index.find(key);
        if (existing != m_index.end()) {
            m_lru.erase(existing->second);
            m_index.erase(existing);
        }
        while (m_lru.size() >= MAX_ENTRIES) {
            const std::wstring& evictKey = m_lru.back().pathKey;
            m_index.erase(evictKey);
            m_lru.pop_back();
        }
        m_lru.push_front(std::move(entry));
        m_index[key] = m_lru.begin();
    }

    void UpdateItemIcon(
        const std::wstring& folderPath,
        size_t itemIndex,
        const std::wstring& fullPath,
        int iconSize,
        const std::shared_ptr<BakedIcon>& bakedIcon)
    {
        std::wstring key = NormalizeFolderPathKey(folderPath);
        if (key.empty()) return;

        std::lock_guard<std::mutex> lock(m_mutex);
        auto mapIt = m_index.find(key);
        if (mapIt == m_index.end()) return;

        auto& entry = *(mapIt->second);
        if (entry.iconSize != iconSize || itemIndex >= entry.items.size()) return;
        auto& ci = entry.items[itemIndex];
        if (ci.fullPath != fullPath) return;

        ci.bakedIcon = bakedIcon;
        ci.iconLoaded = true;
    }

    void Invalidate(const std::wstring& folderPath) {
        std::wstring key = NormalizeFolderPathKey(folderPath);
        if (key.empty()) return;

        std::lock_guard<std::mutex> lock(m_mutex);
        auto mapIt = m_index.find(key);
        if (mapIt != m_index.end()) {
            m_lru.erase(mapIt->second);
            m_index.erase(mapIt);
        }
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_index.clear();
        m_lru.clear();
    }
} g_dirCache;

// ---------------------------------------------------------------------------
// Асинхронный рабочий поток чтения директорий и запекания иконок с токеном отмены generationId
// ---------------------------------------------------------------------------
struct AsyncIconRequest {
    size_t itemIndex = 0;
    std::wstring fullPath;
    std::wstring name;
    bool isDirectory = false;
};

struct AsyncDirLoadResult {
    uint64_t generationId = 0;
    size_t totalItemCount = 0;
    size_t preloadedCount = 0;
    std::vector<PortalFileItem> items;
};

struct AsyncIconLoadResult {
    uint64_t generationId = 0;
    size_t itemIndex = 0;
    std::wstring fullPath;
    std::shared_ptr<BakedIcon> bakedIcon;
};

// Чтение папки с проверкой атомарного токена отмены generationId на каждой итерации
bool PopulateDirectoryItemsAsync(
    const std::wstring& folderPath,
    uint64_t generationId,
    std::vector<PortalFileItem>& outItems,
    size_t& outTotalCount,
    size_t maxItems = 45)
{
    outItems.clear();
    outTotalCount = 0;
    if (folderPath.empty()) return false;

    std::wstring searchPattern = folderPath;
    if (searchPattern.back() != L'\\' && searchPattern.back() != L'/') {
        searchPattern += L'\\';
    }
    std::wstring baseDir = searchPattern;
    searchPattern += L"*";

    WIN32_FIND_DATAW fd = {};
    HANDLE hFind = FindFirstFileExW(
        searchPattern.c_str(),
        FindExInfoBasic,
        &fd,
        FindExSearchNameMatch,
        nullptr,
        FIND_FIRST_EX_LARGE_FETCH);

    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }

    outItems.reserve(128);
    bool cancelled = false;
    do {
        if (g_unloading.load(std::memory_order_acquire) ||
            g_portalGenerationId.load(std::memory_order_acquire) != generationId) {
            cancelled = true;
            break;
        }

        if (fd.cFileName[0] == L'.' &&
            (fd.cFileName[1] == L'\0' || (fd.cFileName[1] == L'.' && fd.cFileName[2] == L'\0'))) {
            continue;
        }
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
            continue;
        }

        ++outTotalCount;

        PortalFileItem item;
        item.name = fd.cFileName;
        item.isDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        item.rawFileSize = (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;

        outItems.push_back(std::move(item));
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    if (cancelled) {
        outItems.clear();
        outTotalCount = 0;
        return false;
    }

    auto cmp = [](const PortalFileItem& a, const PortalFileItem& b) {
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory > b.isDirectory;
        }
        return CompareStringOrdinal(a.name.c_str(), -1, b.name.c_str(), -1, TRUE) == CSTR_LESS_THAN;
    };

    if (outItems.size() > maxItems) {
        std::partial_sort(outItems.begin(), outItems.begin() + maxItems, outItems.end(), cmp);
        outItems.resize(maxItems);
    } else {
        std::sort(outItems.begin(), outItems.end(), cmp);
    }

    const bool isRu = IsRussianUILanguage();
    for (auto& item : outItems) {
        item.fullPath = baseDir + item.name;
        if (item.isDirectory) {
            item.sizeOrTypeText = isRu ? L"Папка" : L"Folder";
        } else {
            wchar_t szBuf[64] = {};
            StrFormatByteSizeW(static_cast<LONGLONG>(item.rawFileSize), szBuf, ARRAYSIZE(szBuf));
            item.sizeOrTypeText = szBuf;
        }
    }

    return true;
}

class PortalAsyncWorker {
    enum class TaskType {
        LoadDirectory,
        LoadIcons
    };

    struct Task {
        TaskType type = TaskType::LoadDirectory;
        uint64_t generationId = 0;
        HWND hPortalWnd = nullptr;
        std::wstring folderPath;
        int iconSize = 32;
        size_t firstScreenCount = 12;
        std::vector<AsyncIconRequest> iconRequests;
    };

    std::mutex m_taskMutex;
    std::condition_variable m_cv;
    std::deque<Task> m_tasks;
    bool m_stop = false;
    std::thread m_thread;

    std::mutex m_resultMutex;
    std::deque<AsyncDirLoadResult> m_completedDirs;
    std::deque<AsyncIconLoadResult> m_completedIcons;
    std::atomic<bool> m_notifyPending{false};

    bool IsStale(uint64_t generationId) const {
        return g_unloading.load(std::memory_order_acquire) ||
               g_portalGenerationId.load(std::memory_order_acquire) != generationId;
    }

    void NotifyPortalUi(HWND hPortalWnd) {
        if (!IsWindow(hPortalWnd)) return;
        if (!m_notifyPending.exchange(true, std::memory_order_acq_rel)) {
            if (!PostMessageW(hPortalWnd, WM_APP_PORTAL_ASYNC_READY, 0, 0)) {
                m_notifyPending.store(false, std::memory_order_release);
            }
        }
    }

    void WorkerLoop() {
        HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        IImageList* cachedImageLists[3] = { nullptr, nullptr, nullptr };
        IconBakeScratch bakeScratch;

        auto processIconRequests = [&](const Task& task, const std::vector<AsyncIconRequest>& reqs) {
            for (const auto& req : reqs) {
                if (IsStale(task.generationId)) break;
                HICON hIcon = ExtractItemIconOnWorker(req.fullPath, req.name, req.isDirectory, task.iconSize, cachedImageLists);
                if (IsStale(task.generationId)) {
                    if (hIcon) DestroyIcon(hIcon);
                    break;
                }
                std::shared_ptr<BakedIcon> baked;
                if (hIcon) {
                    baked = BakeIconToPremultipliedPixels(hIcon, task.iconSize, bakeScratch);
                    DestroyIcon(hIcon);
                }
                if (IsStale(task.generationId)) break;

                g_dirCache.UpdateItemIcon(task.folderPath, req.itemIndex, req.fullPath, task.iconSize, baked);

                {
                    AsyncIconLoadResult iconRes;
                    iconRes.generationId = task.generationId;
                    iconRes.itemIndex = req.itemIndex;
                    iconRes.fullPath = req.fullPath;
                    iconRes.bakedIcon = std::move(baked);
                    std::lock_guard<std::mutex> rlock(m_resultMutex);
                    m_completedIcons.push_back(std::move(iconRes));
                }
                NotifyPortalUi(task.hPortalWnd);
            }
        };

        for (;;) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(m_taskMutex);
                m_cv.wait(lock, [this]() { return m_stop || !m_tasks.empty(); });
                if (m_stop) break;
                task = std::move(m_tasks.front());
                m_tasks.pop_front();
            }

            if (IsStale(task.generationId)) continue;

            if (task.type == TaskType::LoadDirectory) {
                std::vector<PortalFileItem> items;
                size_t totalCount = 0;
                PopulateDirectoryItemsAsync(task.folderPath, task.generationId, items, totalCount, 45);
                if (IsStale(task.generationId)) continue;

                // Сохраняем список элементов в LRU-кэш с подпиской на изменения ФС
                g_dirCache.Put(task.folderPath, task.iconSize, items, totalCount);

                // Готовим список иконок первого экрана для фонового извлечения сразу после отправки плейсхолдеров
                const size_t preloadCount = std::min<size_t>(items.size(), task.firstScreenCount);
                std::vector<AsyncIconRequest> firstScreenReqs;
                firstScreenReqs.reserve(preloadCount);
                for (size_t i = 0; i < preloadCount; ++i) {
                    AsyncIconRequest req;
                    req.itemIndex = i;
                    req.fullPath = items[i].fullPath;
                    req.name = items[i].name;
                    req.isDirectory = items[i].isDirectory;
                    firstScreenReqs.push_back(std::move(req));
                }

                {
                    AsyncDirLoadResult dirRes;
                    dirRes.generationId = task.generationId;
                    dirRes.totalItemCount = totalCount;
                    dirRes.preloadedCount = preloadCount;
                    dirRes.items = std::move(items);
                    std::lock_guard<std::mutex> rlock(m_resultMutex);
                    m_completedDirs.push_back(std::move(dirRes));
                }
                NotifyPortalUi(task.hPortalWnd);

                processIconRequests(task, firstScreenReqs);
            } else if (task.type == TaskType::LoadIcons) {
                processIconRequests(task, task.iconRequests);
            }
        }

        bakeScratch.Release();
        for (int i = 0; i < 3; ++i) {
            if (cachedImageLists[i]) {
                cachedImageLists[i]->Release();
                cachedImageLists[i] = nullptr;
            }
        }

        if (SUCCEEDED(hrCom)) {
            CoUninitialize();
        }
    }

public:
    void Start() {
        std::lock_guard<std::mutex> lock(m_taskMutex);
        if (m_thread.joinable()) return;
        m_stop = false;
        m_thread = std::thread(&PortalAsyncWorker::WorkerLoop, this);
    }

    void Stop() {
        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            m_stop = true;
            m_tasks.clear();
        }
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
        ClearCompletedResults();
    }

    void ClearCompletedResults() {
        m_notifyPending.store(false, std::memory_order_release);
        std::lock_guard<std::mutex> lock(m_resultMutex);
        m_completedDirs.clear();
        m_completedIcons.clear();
    }

    void EnqueueDirectoryLoad(uint64_t generationId, HWND hPortalWnd, const std::wstring& folderPath, int iconSize, size_t firstScreenCount) {
        Task task;
        task.type = TaskType::LoadDirectory;
        task.generationId = generationId;
        task.hPortalWnd = hPortalWnd;
        task.folderPath = folderPath;
        task.iconSize = iconSize;
        task.firstScreenCount = firstScreenCount;
        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            if (m_stop) return;
            m_tasks.clear();
            m_tasks.push_back(std::move(task));
        }
        m_cv.notify_one();
    }

    void EnqueueIconsLoad(uint64_t generationId, HWND hPortalWnd, const std::wstring& folderPath, int iconSize, std::vector<AsyncIconRequest>&& requests) {
        if (requests.empty()) return;
        Task task;
        task.type = TaskType::LoadIcons;
        task.generationId = generationId;
        task.hPortalWnd = hPortalWnd;
        task.folderPath = folderPath;
        task.iconSize = iconSize;
        task.iconRequests = std::move(requests);
        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            if (m_stop) return;
            m_tasks.push_back(std::move(task));
        }
        m_cv.notify_one();
    }

    void DrainCompletedResults(std::deque<AsyncDirLoadResult>& outDirs, std::deque<AsyncIconLoadResult>& outIcons) {
        m_notifyPending.store(false, std::memory_order_release);
        std::lock_guard<std::mutex> lock(m_resultMutex);
        outDirs.swap(m_completedDirs);
        outIcons.swap(m_completedIcons);
    }
} g_asyncWorker;

// ---------------------------------------------------------------------------
// Реализация легковесного интерфейса IShellBrowser (по спецификации п. 3)
// ---------------------------------------------------------------------------
class LightweightPortalShellBrowser : public IShellBrowser {
private:
    LONG m_refCount = 1;
    HWND m_hPortalWnd = nullptr;

public:
    explicit LightweightPortalShellBrowser(HWND hPortal) : m_hPortalWnd(hPortal) {}
    virtual ~LightweightPortalShellBrowser() = default;

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override {
        if (!ppvObject) return E_POINTER;
        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, IID_IOleWindow) ||
            IsEqualIID(riid, IID_IShellBrowser)) {
            *ppvObject = static_cast<IShellBrowser*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    STDMETHODIMP_(ULONG) Release() override {
        ULONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // IOleWindow
    STDMETHODIMP GetWindow(HWND* phwnd) override {
        if (!phwnd) return E_POINTER;
        *phwnd = m_hPortalWnd;
        return S_OK;
    }
    STDMETHODIMP ContextSensitiveHelp(BOOL) override { return E_NOTIMPL; }

    // IShellBrowser
    STDMETHODIMP InsertMenusSB(HMENU, LPOLEMENUGROUPWIDTHS) override { return E_NOTIMPL; }
    STDMETHODIMP SetMenuSB(HMENU, HOLEMENU, HWND) override { return E_NOTIMPL; }
    STDMETHODIMP RemoveMenusSB(HMENU) override { return E_NOTIMPL; }
    STDMETHODIMP SetStatusTextSB(LPCWSTR) override { return S_OK; }
    STDMETHODIMP EnableModelessSB(BOOL) override { return S_OK; }
    STDMETHODIMP TranslateAcceleratorSB(MSG*, WORD) override { return S_FALSE; }

    STDMETHODIMP BrowseObject(PCUIDLIST_RELATIVE pidl, UINT) override {
        ScopedModalCallGuard modalGuard;
        if (pidl) {
            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_IDLIST;
            sei.lpIDList = const_cast<PIDLIST_RELATIVE>(pidl);
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
            return S_OK;
        }
        return E_FAIL;
    }

    STDMETHODIMP GetViewStateStream(DWORD, IStream** ppStrm) override {
        if (ppStrm) *ppStrm = nullptr;
        return E_NOTIMPL;
    }
    STDMETHODIMP GetControlWindow(UINT, HWND* phwnd) override {
        if (phwnd) *phwnd = nullptr;
        return E_NOTIMPL;
    }
    STDMETHODIMP SendControlMsg(UINT, UINT, WPARAM, LPARAM, LRESULT*) override { return E_NOTIMPL; }
    STDMETHODIMP QueryActiveShellView(IShellView** ppshv) override {
        if (ppshv) *ppshv = nullptr;
        return E_NOTIMPL;
    }
    STDMETHODIMP OnViewWindowActive(IShellView*) override { return S_OK; }
    STDMETHODIMP SetToolbarItems(LPTBBUTTONSB, UINT, UINT) override { return E_NOTIMPL; }

    // Быстрое чтение папки через FindFirstFileExW с подсчетом истинного числа элементов,
    // частичной сортировкой (std::partial_sort) только топ-45 объектов и форматированием строк только для них
    bool PopulateItemsFromDirectory(const std::wstring& folderPath, std::vector<PortalFileItem>& outItems, size_t& outTotalCount, size_t maxItems = 45) {
        outItems.clear();
        outTotalCount = 0;
        if (folderPath.empty()) return false;

        std::wstring searchPattern = folderPath;
        if (searchPattern.back() != L'\\' && searchPattern.back() != L'/') {
            searchPattern += L'\\';
        }
        std::wstring baseDir = searchPattern;
        searchPattern += L"*";

        WIN32_FIND_DATAW fd = {};
        HANDLE hFind = FindFirstFileExW(
            searchPattern.c_str(),
            FindExInfoBasic,
            &fd,
            FindExSearchNameMatch,
            nullptr,
            FIND_FIRST_EX_LARGE_FETCH);

        if (hFind == INVALID_HANDLE_VALUE) {
            return false;
        }

        outItems.reserve(128);
        do {
            if (fd.cFileName[0] == L'.' &&
                (fd.cFileName[1] == L'\0' || (fd.cFileName[1] == L'.' && fd.cFileName[2] == L'\0'))) {
                continue;
            }
            if (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
                continue;
            }

            ++outTotalCount;

            PortalFileItem item;
            item.name = fd.cFileName;
            item.isDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            item.rawFileSize = (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;

            outItems.push_back(std::move(item));
        } while (FindNextFileW(hFind, &fd));

        FindClose(hFind);

        // Частичная сортировка O(N log K): отбираем и сортируем только первые maxItems (45) элементов
        auto cmp = [](const PortalFileItem& a, const PortalFileItem& b) {
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory > b.isDirectory;
            }
            return CompareStringOrdinal(a.name.c_str(), -1, b.name.c_str(), -1, TRUE) == CSTR_LESS_THAN;
        };

        if (outItems.size() > maxItems) {
            std::partial_sort(outItems.begin(), outItems.begin() + maxItems, outItems.end(), cmp);
            outItems.resize(maxItems);
        } else {
            std::sort(outItems.begin(), outItems.end(), cmp);
        }

        // Форматируем полный путь и строку размера только для отображаемых элементов (до 45 шт.)
        const bool isRu = IsRussianUILanguage();
        for (auto& item : outItems) {
            item.fullPath = baseDir + item.name;
            if (item.isDirectory) {
                item.sizeOrTypeText = isRu ? L"Папка" : L"Folder";
            } else {
                wchar_t szBuf[64] = {};
                StrFormatByteSizeW(static_cast<LONGLONG>(item.rawFileSize), szBuf, ARRAYSIZE(szBuf));
                item.sizeOrTypeText = szBuf;
            }
        }

        return true;
    }
};

// ---------------------------------------------------------------------------
// Глобальное состояние активного окна-портала
// ---------------------------------------------------------------------------
enum class PortalAnimState {
    Hidden,
    Opening,
    Visible,
    Closing
};

struct PortalWindowState {
    HWND hPortalWnd = nullptr;
    HWND hDesktopListView = nullptr;
    int activeItemIndex = -1;
    int dpi = 96;
    std::wstring folderPath;
    std::wstring folderTitle;
    size_t totalItemCount = 0;

    RECT iconScreenRect = {};
    RECT startCircleRect = {};
    RECT targetPortalRect = {};
    RECT currentRect = {};

    PortalAnimState animState = PortalAnimState::Hidden;
    double animStartMs = 0.0;
    float animProgress = 0.0f; // 0.0f (кружок в папке) -> 1.0f (полный прямоугольник)
    float morphProgress = 1.0f;

    int hoveredGridIndex = -1;
    int scrollOffsetY = 0;
    int maxScrollY = 0;

    bool scrollbarEdgeHovered = false;
    bool isDraggingScrollbar = false;
    int scrollbarDragStartY = 0;
    int scrollbarDragStartOffset = 0;
    int scrollbarTrackTop = 0;
    int scrollbarTrackHeight = 0;
    int scrollbarThumbY = 0;
    int scrollbarThumbHeight = 0;

    int pressedGridIndex = -1;
    POINT pressStartPt = {};
    bool isDraggingOrMenuActive = false;

    IContextMenu2* pActiveCtxMenu2 = nullptr;
    IContextMenu3* pActiveCtxMenu3 = nullptr;

    bool renderDirty = false;
    bool frameTimerActive = false;
    bool isScrolling = false;
    bool isLoadingDirectory = false;
    bool dwmVisualsDirty = true;
    bool dwmBlurActive = false;
    bool mouseTrackingActive = false;
    uint64_t contentRevision = 1;
    UINT frameIntervalMs = 16;

    LightweightPortalShellBrowser* pShellBrowser = nullptr;
    std::vector<PortalFileItem> items;
} g_portal;

void RenderAndPresentPortal(HWND hWnd);
void StartPortalFadeOut();

// Определяем интервал кадра (мс) по частоте обновления экрана через DwmGetCompositionTimingInfo
UINT GetPortalFrameIntervalMs() {
    DWM_TIMING_INFO ti = {};
    ti.cbSize = sizeof(ti);
    if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &ti)) &&
        ti.rateRefresh.uiNumerator > 0 && ti.rateRefresh.uiDenominator > 0) {
        double hz = static_cast<double>(ti.rateRefresh.uiNumerator) /
                    static_cast<double>(ti.rateRefresh.uiDenominator);
        if (hz >= 30.0 && hz <= 360.0) {
            UINT ms = static_cast<UINT>(std::lround(1000.0 / hz));
            return std::clamp<UINT>(ms, 4u, 33u);
        }
    }
    return 16u;
}

// Коалесценция перерисовок: помечает окно dirty и планирует отрисовку на следующий кадр дисплея
void RequestPortalRepaint(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    g_portal.renderDirty = true;
    if (g_portal.animState == PortalAnimState::Opening || g_portal.animState == PortalAnimState::Closing) {
        return;
    }
    if (!g_portal.frameTimerActive) {
        g_portal.frameTimerActive = true;
        SetTimer(hWnd, TIMER_ID_FRAME_RENDER, g_portal.frameIntervalMs, nullptr);
    }
}

void ScheduleIconsLoadForRange(HWND hWnd, uint64_t generationId, size_t startIdx, size_t endIdx, int iconSize) {
    if (!hWnd || !IsWindow(hWnd) || g_portal.items.empty()) return;
    endIdx = std::min<size_t>(endIdx, g_portal.items.size());
    if (startIdx >= endIdx) return;

    std::vector<AsyncIconRequest> requests;
    requests.reserve(endIdx - startIdx);
    for (size_t i = startIdx; i < endIdx; ++i) {
        auto& item = g_portal.items[i];
        if (!item.iconLoaded && !item.iconRequested) {
            item.iconRequested = true;
            AsyncIconRequest req;
            req.itemIndex = i;
            req.fullPath = item.fullPath;
            req.name = item.name;
            req.isDirectory = item.isDirectory;
            requests.push_back(std::move(req));
        }
    }
    if (!requests.empty()) {
        g_asyncWorker.EnqueueIconsLoad(generationId, hWnd, g_portal.folderPath, iconSize, std::move(requests));
    }
}

void GetPortalLayoutMetrics(int dpi, int& outCols, int& outRowHeight, int& outTopMargin) {
    if (g_settings.listViewMode) {
        outCols = 1;
        outRowHeight = std::max<int>(1, ScaleByDpi(36, dpi));
        outTopMargin = ScaleByDpi(6, dpi);
    } else {
        outCols = std::clamp(g_settings.gridColumns, 2, 5);
        outRowHeight = std::max<int>(1, ScaleByDpi(g_settings.showFileMetadata ? 88 : 72, dpi));
        outTopMargin = ScaleByDpi(8, dpi);
    }
}

inline int GetPortalIconSize(int dpi) {
    return ScaleByDpi(g_settings.listViewMode ? 22 : 32, dpi);
}

// Ленивая подгрузка иконок для видимого диапазона после остановки скролла
void ScheduleVisibleIconsLoad(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || g_portal.items.empty() || g_portal.isLoadingDirectory) return;

    const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
    const int height = std::max<int>(ScaleByDpi(16, dpi), g_portal.currentRect.bottom - g_portal.currentRect.top);
    const int headerHeight = ScaleByDpi(42, dpi);
    const int clipTop = headerHeight + std::max<int>(1, ScaleByDpi(1, dpi));
    const int clipBottom = height - ScaleByDpi(4, dpi);
    int cols = 1;
    int cellHeight = 1;
    int topMargin = 0;
    GetPortalLayoutMetrics(dpi, cols, cellHeight, topMargin);
    const int gridTopY = headerHeight + topMargin - g_portal.scrollOffsetY;

    int firstRow = std::max<int>(0, (clipTop - gridTopY) / cellHeight);
    int lastRow  = std::max<int>(firstRow, (clipBottom - gridTopY) / cellHeight);

    size_t startIdx = static_cast<size_t>(firstRow * cols);
    size_t endIdx   = std::min<size_t>(g_portal.items.size(), static_cast<size_t>((lastRow + 1) * cols));
    int iconSize = GetPortalIconSize(dpi);

    ScheduleIconsLoadForRange(hWnd, g_portalGenerationId.load(std::memory_order_acquire), startIdx, endIdx, iconSize);
}

void RefreshPortalDirectoryAsync(HWND hPortal) {
    if (!hPortal || !IsWindow(hPortal) || g_portal.folderPath.empty()) return;
    g_dirCache.Invalidate(g_portal.folderPath);

    uint64_t genId = g_portalGenerationId.fetch_add(1, std::memory_order_acq_rel) + 1;
    g_asyncWorker.ClearCompletedResults();

    const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
    const int portalH = std::max<int>(ScaleByDpi(180, dpi), g_portal.targetPortalRect.bottom - g_portal.targetPortalRect.top);
    const int iconSize = GetPortalIconSize(dpi);
    int cols = 1;
    int cellHeight = 1;
    int topMargin = 0;
    GetPortalLayoutMetrics(dpi, cols, cellHeight, topMargin);
    const int visibleRows = (portalH / cellHeight) + 1;
    const size_t firstScreenCount = static_cast<size_t>(cols * visibleRows);

    g_portal.isLoadingDirectory = true;
    ++g_portal.contentRevision;
    g_asyncWorker.EnqueueDirectoryLoad(genId, hPortal, g_portal.folderPath, iconSize, firstScreenCount);
    RequestPortalRepaint(hPortal);
}

void ProcessAsyncPortalResults(HWND hWnd) {
    std::deque<AsyncDirLoadResult> dirs;
    std::deque<AsyncIconLoadResult> icons;
    g_asyncWorker.DrainCompletedResults(dirs, icons);

    const uint64_t currentGen = g_portalGenerationId.load(std::memory_order_acquire);
    bool needsRepaint = false;

    for (auto& dirRes : dirs) {
        if (dirRes.generationId != currentGen) continue;
        g_portal.isLoadingDirectory = false;
        g_portal.items = std::move(dirRes.items);
        g_portal.totalItemCount = dirRes.totalItemCount;
        const size_t marked = std::min<size_t>(dirRes.preloadedCount, g_portal.items.size());
        for (size_t i = 0; i < marked; ++i) {
            g_portal.items[i].iconRequested = true;
        }
        needsRepaint = true;
    }

    for (auto& iconRes : icons) {
        if (iconRes.generationId != currentGen) continue;
        if (iconRes.itemIndex >= g_portal.items.size()) continue;
        auto& item = g_portal.items[iconRes.itemIndex];
        if (item.fullPath != iconRes.fullPath) continue;

        item.bakedIcon = std::move(iconRes.bakedIcon);
        item.iconLoaded = true;
        item.iconRequested = true;
        needsRepaint = true;
    }

    if (needsRepaint) {
        ++g_portal.contentRevision;
        RequestPortalRepaint(hWnd);
    }
}

// Вызов оригинального контекстного меню Windows 11 для элемента внутри портала
void ShowNativeShellContextMenu(HWND hPortal, int itemIndex, POINT ptScreen) {
    if (g_unloading.load()) return;
    ScopedModalCallGuard modalGuard;

    if (itemIndex < 0 || itemIndex >= static_cast<int>(g_portal.items.size())) return;
    const std::wstring& itemPath = g_portal.items[itemIndex].fullPath;
    if (itemPath.empty()) return;

    PIDLIST_ABSOLUTE pidlFull = ILCreateFromPathW(itemPath.c_str());
    if (!pidlFull) return;

    IShellFolder* pParentFolder = nullptr;
    PCUITEMID_CHILD pidlChild = nullptr;
    if (SUCCEEDED(SHBindToParent(pidlFull, IID_IShellFolder, reinterpret_cast<void**>(&pParentFolder), &pidlChild)) && pParentFolder) {
        IContextMenu* pCtxMenu = nullptr;
        if (SUCCEEDED(pParentFolder->GetUIObjectOf(hPortal, 1, &pidlChild, IID_IContextMenu, nullptr, reinterpret_cast<void**>(&pCtxMenu))) && pCtxMenu) {
            HMENU hMenu = CreatePopupMenu();
            if (hMenu) {
                UINT uFlags = CMF_NORMAL | CMF_EXPLORE | CMF_CANRENAME;
                if (GetKeyState(VK_SHIFT) < 0) uFlags |= CMF_EXTENDEDVERBS;

                if (SUCCEEDED(pCtxMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, uFlags))) {
                    pCtxMenu->QueryInterface(IID_IContextMenu2, reinterpret_cast<void**>(&g_portal.pActiveCtxMenu2));
                    pCtxMenu->QueryInterface(IID_IContextMenu3, reinterpret_cast<void**>(&g_portal.pActiveCtxMenu3));

                    g_portal.isDraggingOrMenuActive = true;
                    SetForegroundWindow(hPortal);

                    UINT cmd = TrackPopupMenuEx(
                        hMenu,
                        TPM_RETURNCMD | TPM_RIGHTBUTTON,
                        ptScreen.x,
                        ptScreen.y,
                        hPortal,
                        nullptr);

                    if (g_portal.pActiveCtxMenu2) {
                        g_portal.pActiveCtxMenu2->Release();
                        g_portal.pActiveCtxMenu2 = nullptr;
                    }
                    if (g_portal.pActiveCtxMenu3) {
                        g_portal.pActiveCtxMenu3->Release();
                        g_portal.pActiveCtxMenu3 = nullptr;
                    }

                    if (cmd >= 1 && cmd <= 0x7FFF && !g_unloading.load()) {
                        UINT cmdOffset = cmd - 1;
                        wchar_t verbBuf[64] = {};
                        bool isRenameVerb = false;
                        if (SUCCEEDED(pCtxMenu->GetCommandString(cmdOffset, GCS_VERBW, nullptr, reinterpret_cast<LPSTR>(verbBuf), ARRAYSIZE(verbBuf)))) {
                            if (_wcsicmp(verbBuf, L"rename") == 0) {
                                isRenameVerb = true;
                            }
                        }

                        if (isRenameVerb) {
                            // Открываем родительскую папку с выделением файла для штатного переименования
                            SHOpenFolderAndSelectItems(pidlFull, 0, nullptr, OFASI_EDIT);
                        } else {
                            CMINVOKECOMMANDINFOEX ici = {};
                            ici.cbSize = sizeof(ici);
                            ici.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
                            if (GetKeyState(VK_CONTROL) < 0) ici.fMask |= CMIC_MASK_CONTROL_DOWN;
                            if (GetKeyState(VK_SHIFT) < 0)   ici.fMask |= CMIC_MASK_SHIFT_DOWN;
                            ici.hwnd = hPortal;
                            ici.lpVerb = MAKEINTRESOURCEA(cmdOffset);
                            ici.lpVerbW = MAKEINTRESOURCEW(cmdOffset);
                            ici.nShow = SW_SHOWNORMAL;
                            ici.ptInvoke = ptScreen;
                            pCtxMenu->InvokeCommand(reinterpret_cast<LPCMINVOKECOMMANDINFO>(&ici));
                        }

                        // Обновляем список файлов на случай удаления/изменения
                        if (!g_unloading.load() && IsWindow(hPortal)) {
                            RefreshPortalDirectoryAsync(hPortal);
                        }
                    }

                    g_portal.isDraggingOrMenuActive = false;
                }
                DestroyMenu(hMenu);
            }
            pCtxMenu->Release();
        }
        pParentFolder->Release();
    }
    ILFree(pidlFull);
}

// Запуск системного переноса файла (OLE Drag-and-Drop) на рабочий стол или в Проводник
void StartNativeFileDragAndDrop(HWND hPortal, int itemIndex) {
    if (g_unloading.load()) return;
    ScopedModalCallGuard modalGuard;

    if (itemIndex < 0 || itemIndex >= static_cast<int>(g_portal.items.size())) return;
    std::wstring itemPath = g_portal.items[itemIndex].fullPath;
    if (itemPath.empty()) return;

    PIDLIST_ABSOLUTE pidlFull = ILCreateFromPathW(itemPath.c_str());
    if (!pidlFull) return;

    IShellFolder* pParentFolder = nullptr;
    PCUITEMID_CHILD pidlChild = nullptr;
    if (SUCCEEDED(SHBindToParent(pidlFull, IID_IShellFolder, reinterpret_cast<void**>(&pParentFolder), &pidlChild)) && pParentFolder) {
        IDataObject* pDataObj = nullptr;
        if (SUCCEEDED(pParentFolder->GetUIObjectOf(hPortal, 1, &pidlChild, IID_IDataObject, nullptr, reinterpret_cast<void**>(&pDataObj))) && pDataObj) {
            PortalDropSource* pDropSrc = new PortalDropSource();
            DWORD dwEffect = 0;

            g_portal.isDraggingOrMenuActive = true;
            g_portal.hoveredGridIndex = -1;
            if (IsWindow(hPortal)) {
                RenderAndPresentPortal(hPortal);
            }

            HRESULT hrDrag = SHDoDragDrop(
                hPortal,
                pDataObj,
                pDropSrc,
                DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK,
                &dwEffect);

            g_portal.isDraggingOrMenuActive = false;
            pDropSrc->Release();
            pDataObj->Release();

            // Если файл был перемещен из папки, мгновенно обновляем список в портале
            if (!g_unloading.load() && SUCCEEDED(hrDrag) && IsWindow(hPortal)) {
                RefreshPortalDirectoryAsync(hPortal);
            }
        }
        pParentFolder->Release();
    }
    ILFree(pidlFull);
}

// Проверяет, что переданный HWND является SysListView32 рабочего стола
bool IsTrueDesktopListView(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;

    wchar_t className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) return false;
    if (wcscmp(className, L"SysListView32") != 0) return false;

    HWND hParent = GetParent(hWnd);
    if (!hParent || !IsWindow(hParent)) return false;

    wchar_t parentClass[64] = {};
    if (!GetClassNameW(hParent, parentClass, ARRAYSIZE(parentClass))) return false;
    if (wcscmp(parentClass, L"SHELLDLL_DefView") != 0) return false;

    HWND hGrandParent = GetParent(hParent);
    if (!hGrandParent || !IsWindow(hGrandParent)) return false;

    wchar_t grandParentClass[64] = {};
    if (!GetClassNameW(hGrandParent, grandParentClass, ARRAYSIZE(grandParentClass))) return false;

    return (wcscmp(grandParentClass, L"Progman") == 0 || wcscmp(grandParentClass, L"WorkerW") == 0);
}

// Получает полный файловый путь к элементу рабочего стола по его индексу в SysListView32
bool GetDesktopItemDirectoryPath(HWND hListView, int itemIndex, std::wstring& outFullPath, std::wstring& outDisplayName) {
    outFullPath.clear();
    outDisplayName.clear();

    if (!IsWindow(hListView) || itemIndex < 0) return false;

    wchar_t itemText[MAX_PATH] = {};
    LVITEMW lvi = {};
    lvi.mask = LVIF_TEXT;
    lvi.iItem = itemIndex;
    lvi.iSubItem = 0;
    lvi.pszText = itemText;
    lvi.cchTextMax = ARRAYSIZE(itemText);

    if (!SendMessageW(hListView, LVM_GETITEMTEXTW, itemIndex, reinterpret_cast<LPARAM>(&lvi)) || itemText[0] == L'\0') {
        return false;
    }
    outDisplayName = itemText;

    // 1. Быстрая проверка на Пользовательском и Общем рабочем столе (с кэшированием базовых путей)
    static std::wstring s_userDesktopDir;
    static std::wstring s_commonDesktopDir;
    static std::once_flag s_desktopPathsFlag;
    std::call_once(s_desktopPathsFlag, []() {
        wchar_t buf[MAX_PATH] = {};
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_CURRENT, buf))) {
            s_userDesktopDir = buf;
        }
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_CURRENT, buf))) {
            s_commonDesktopDir = buf;
        }
    });

    const std::wstring* desktopDirs[] = { &s_userDesktopDir, &s_commonDesktopDir };
    bool existsAsFileOnDesktop = false;
    for (const std::wstring* pBaseDir : desktopDirs) {
        if (pBaseDir->empty()) continue;
        wchar_t candidatePath[MAX_PATH] = {};
        if (PathCombineW(candidatePath, pBaseDir->c_str(), itemText)) {
            DWORD attrs = GetFileAttributesW(candidatePath);
            if (attrs != INVALID_FILE_ATTRIBUTES) {
                if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
                    outFullPath = candidatePath;
                    return true;
                }
                existsAsFileOnDesktop = true;
            }
        }
    }

    // Быстрая отсечка ярлыков (.lnk, .url) без обращения к COM
    if (!existsAsFileOnDesktop) {
        static constexpr LPCWSTR kShortcutExts[] = { L".lnk", L".url" };
        for (const std::wstring* pBaseDir : desktopDirs) {
            if (pBaseDir->empty() || existsAsFileOnDesktop) continue;
            for (LPCWSTR ext : kShortcutExts) {
                std::wstring withExt = outDisplayName + ext;
                wchar_t candidatePath[MAX_PATH] = {};
                if (PathCombineW(candidatePath, pBaseDir->c_str(), withExt.c_str())) {
                    DWORD attrs = GetFileAttributesW(candidatePath);
                    if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                        existsAsFileOnDesktop = true;
                        break;
                    }
                }
            }
        }
    }

    // Если элемент найден на рабочем столе и это обычный файл или ярлык, не запускаем тяжелый перебор IShellFolder
    if (existsAsFileOnDesktop) {
        return false;
    }

    // Компактный кэш виртуальных/системных значков со сбросом по изменению числа элементов или таймауту 5 сек
    struct DesktopVirtualCacheEntry {
        bool isDirectory = false;
        std::wstring fullPath;
        ULONGLONG timestampMs = 0;
    };
    static HWND s_cachedListView = nullptr;
    static int s_cachedItemCount = -1;
    static std::unordered_map<std::wstring, DesktopVirtualCacheEntry> s_virtualItemCache;

    const int currentItemCount = static_cast<int>(SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0));
    const ULONGLONG nowTickMs = GetTickCount64();
    if (hListView != s_cachedListView || currentItemCount != s_cachedItemCount) {
        s_virtualItemCache.clear();
        s_cachedListView = hListView;
        s_cachedItemCount = currentItemCount;
    }

    std::wstring nameKey = outDisplayName;
    if (!nameKey.empty()) {
        CharLowerBuffW(nameKey.data(), static_cast<DWORD>(nameKey.size()));
    }

    auto cacheIt = s_virtualItemCache.find(nameKey);
    if (cacheIt != s_virtualItemCache.end()) {
        if (nowTickMs - cacheIt->second.timestampMs <= 5000ULL) {
            if (cacheIt->second.isDirectory && !cacheIt->second.fullPath.empty()) {
                DWORD attrs = GetFileAttributesW(cacheIt->second.fullPath.c_str());
                if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                    outFullPath = cacheIt->second.fullPath;
                    return true;
                }
                s_virtualItemCache.erase(cacheIt);
            } else {
                return false;
            }
        } else {
            s_virtualItemCache.erase(cacheIt);
        }
    }

    // 2. Если имя виртуальное или нестандартное, перебираем элементы IShellFolder рабочего стола
    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop)) && pDesktop) {
        IEnumIDList* pEnum = nullptr;
        if (SUCCEEDED(pDesktop->EnumObjects(nullptr, SHCONTF_FOLDERS, &pEnum)) && pEnum) {
            PITEMID_CHILD pidl = nullptr;
            ULONG fetched = 0;
            while (pEnum->Next(1, &pidl, &fetched) == S_OK && fetched == 1) {
                STRRET strDisp = {};
                if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strDisp))) {
                    LPWSTR pszDisp = nullptr;
                    if (SUCCEEDED(StrRetToStrW(&strDisp, pidl, &pszDisp)) && pszDisp) {
                        bool match = (_wcsicmp(pszDisp, itemText) == 0);
                        CoTaskMemFree(pszDisp);

                        if (match) {
                            STRRET strParse = {};
                            if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &strParse))) {
                                LPWSTR pszParse = nullptr;
                                if (SUCCEEDED(StrRetToStrW(&strParse, pidl, &pszParse)) && pszParse) {
                                    DWORD attrs = GetFileAttributesW(pszParse);
                                    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                                        outFullPath = pszParse;
                                        CoTaskMemFree(pszParse);
                                        CoTaskMemFree(pidl);
                                        break;
                                    }
                                    CoTaskMemFree(pszParse);
                                }
                            }
                        }
                    }
                }
                CoTaskMemFree(pidl);
            }
            pEnum->Release();
        }
        pDesktop->Release();
    }

    if (s_virtualItemCache.size() >= 64) {
        s_virtualItemCache.clear();
    }
    DesktopVirtualCacheEntry entry;
    entry.isDirectory = !outFullPath.empty();
    entry.fullPath = outFullPath;
    entry.timestampMs = GetTickCount64();
    s_virtualItemCache[nameKey] = std::move(entry);

    return !outFullPath.empty();
}

// Включение/отключение системного DWM Blur / Acrylic отдельно от базовых атрибутов окна (для отключения на фазе морфинга)
void SetPortalBlurEnabled(HWND hPortal, bool enable) {
    if (!IsWindow(hPortal)) return;

    if (!g_pSetWCA) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            g_pSetWCA = reinterpret_cast<pfnSetWindowCompositionAttribute>(
                GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
        }
    }

    if (enable && g_settings.enableBlur) {
        DWM_BLURBEHIND bb = {};
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = TRUE;
        DwmEnableBlurBehindWindow(hPortal, &bb);

        if (g_pSetWCA) {
            ACCENT_POLICY policy = {};
            if (g_settings.blurStrength == 1) {
                // Уровень 1: чистое гладкое размытие без зернистости на всю площадь окна
                policy.AccentState = ACCENT_ENABLE_BLURBEHIND;
                policy.AccentFlags = 0;
                policy.GradientColor = 0;
            } else if (g_settings.blurStrength == 2) {
                // Уровень 2: усиленное гладкое стекло с плотной заливкой краев
                policy.AccentState = ACCENT_ENABLE_BLURBEHIND;
                policy.AccentFlags = 2;
                DWORD alpha = static_cast<DWORD>( std::clamp<int>(g_settings.bgOpacity / 3, 20, 90) );
                policy.GradientColor = (alpha << 24) | 0x001E1E22u;
            } else {
                // Уровень 3: глубокое матовое стекло (равномерное по всей площади)
                policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
                policy.AccentFlags = 2;
                DWORD alpha = static_cast<DWORD>( std::clamp<int>(g_settings.bgOpacity / 2, 40, 140) );
                policy.GradientColor = (alpha << 24) | 0x001A1A20u;
            }
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
            g_pSetWCA(hPortal, &data);
        }
        g_portal.dwmBlurActive = true;
    } else {
        DWM_BLURBEHIND bb = {};
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = FALSE;
        DwmEnableBlurBehindWindow(hPortal, &bb);

        if (g_pSetWCA) {
            ACCENT_POLICY policy = {};
            policy.AccentState = ACCENT_DISABLED;
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
            g_pSetWCA(hPortal, &data);
        }
        g_portal.dwmBlurActive = false;
    }
}

// Настройка DWM: скругление, тень и гладкое размытие без зернистости во всю площадь окна
void ApplyPortalDwmVisuals(HWND hPortal, bool enableBlurEffect = true) {
    if (!IsWindow(hPortal)) return;

    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hPortal, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

    DWORD cornerPref = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hPortal, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hPortal, &margins);

    // Отключаем системный шумный Backdrop, чтобы использовать чистое стекло без виньетирования по краям
    DWORD backdropNone = 1; // DWMSBT_NONE
    DwmSetWindowAttribute(hPortal, DWMWA_SYSTEMBACKDROP_TYPE, &backdropNone, sizeof(backdropNone));

    SetPortalBlurEnabled(hPortal, enableBlurEffect);
}

float EaseOutCubic(float t) {
    float inv = 1.0f - t;
    return 1.0f - (inv * inv * inv);
}

float EaseInCubic(float t) {
    return t * t * t;
}

int LerpInt(int a, int b, float t) {
    return static_cast<int>(std::round(static_cast<float>(a) + static_cast<float>(b - a) * t));
}

// Аналитическая сглаженная маска скругленного прямоугольника/круга с отсечением по квадрату радиуса (без лишних sqrt)
void ApplyRoundedRectMaskToDib(BYTE* bits, int stride, int width, int height, float radius) {
    if (!bits || width <= 0 || height <= 0 || radius <= 0.25f) return;

    const float maxR = static_cast<float>(std::min<int>(width, height)) * 0.5f;
    const float r = std::clamp(radius, 0.0f, maxR);
    const float rOuter = r + 0.5f;
    const float rInner = std::max<float>(0.0f, r - 0.5f);
    const float rOuterSq = rOuter * rOuter;
    const float rInnerSq = rInner * rInner;

    const float cx = static_cast<float>(width) * 0.5f;
    const float cy = static_cast<float>(height) * 0.5f;
    const float bx = cx - r;
    const float by = cy - r;

    const int bandX = std::min<int>(width, static_cast<int>(std::ceil(r)) + 2);
    const int bandY = std::min<int>(height, static_cast<int>(std::ceil(r)) + 2);
    const int leftEnd = bandX;
    const int rightStart = std::max<int>(leftEnd, width - bandX);
    const int topEnd = bandY;
    const int botStart = std::max<int>(topEnd, height - bandY);

    auto maskRowRange = [&](int yStart, int yEnd) {
        for (int y = yStart; y < yEnd; ++y) {
            const float qy = std::fabs((static_cast<float>(y) + 0.5f) - cy) - by;
            if (qy <= 0.0f) continue;
            const float qySq = qy * qy;

            BYTE* row = bits + y * stride;
            auto maskXRange = [&](int xStart, int xEnd) {
                for (int x = xStart; x < xEnd; ++x) {
                    const float qx = std::fabs((static_cast<float>(x) + 0.5f) - cx) - bx;
                    if (qx <= 0.0f) continue;

                    const float distSq = qx * qx + qySq;
                    if (distSq >= rOuterSq) {
                        *reinterpret_cast<uint32_t*>(row + x * 4) = 0u;
                    } else if (distSq > rInnerSq) {
                        const float dist = std::sqrt(distSq) - r;
                        const uint32_t m = static_cast<uint32_t>(std::round((0.5f - dist) * 255.0f));
                        BYTE* px = row + x * 4;
                        px[0] = static_cast<BYTE>(FastDiv255(px[0] * m));
                        px[1] = static_cast<BYTE>(FastDiv255(px[1] * m));
                        px[2] = static_cast<BYTE>(FastDiv255(px[2] * m));
                        px[3] = static_cast<BYTE>(FastDiv255(px[3] * m));
                    }
                }
            };

            maskXRange(0, leftEnd);
            maskXRange(rightStart, width);
        }
    };

    maskRowRange(0, topEnd);
    maskRowRange(botStart, height);
}

// Сверхбыстрый блиттинг запечённой premultiplied BGRA иконки прямо в DIB (ноль вызовов GDI/DrawIconEx/GdiFlush)
void BlitBakedIconToDib(
    const BakedIcon& baked,
    BYTE* dstBits,
    int dstStride,
    int dstW,
    int dstH,
    int dstX,
    int dstY,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom)
{
    const int iconSize = baked.size;
    if (!dstBits || iconSize <= 0 || baked.pixels.empty()) return;

    const int drawX0 = std::max<int>(std::max<int>(dstX, clipLeft), 0);
    const int drawY0 = std::max<int>(std::max<int>(dstY, clipTop), 0);
    const int drawX1 = std::min<int>(std::min<int>(dstX + iconSize, clipRight), dstW);
    const int drawY1 = std::min<int>(std::min<int>(dstY + iconSize, clipBottom), dstH);
    if (drawX0 >= drawX1 || drawY0 >= drawY1) return;

    const uint32_t* srcBase = baked.pixels.data();
    for (int y = drawY0; y < drawY1; ++y) {
        const uint32_t* srcRow = srcBase + static_cast<size_t>(y - dstY) * static_cast<size_t>(iconSize);
        uint32_t* dstRow = reinterpret_cast<uint32_t*>(dstBits + y * dstStride);
        for (int x = drawX0; x < drawX1; ++x) {
            const uint32_t s = srcRow[x - dstX];
            if (s == 0u) continue;
            const uint32_t sa = s >> 24;
            if (sa == 255u) {
                dstRow[x] = s;
                continue;
            }
            const uint32_t d = dstRow[x];
            if (d == 0u) {
                dstRow[x] = s;
                continue;
            }
            const uint32_t invA = 255u - sa;
            const uint32_t ob = std::min<uint32_t>(255u, (s & 0xFFu) + FastDiv255((d & 0xFFu) * invA));
            const uint32_t og = std::min<uint32_t>(255u, ((s >> 8) & 0xFFu) + FastDiv255(((d >> 8) & 0xFFu) * invA));
            const uint32_t or_ = std::min<uint32_t>(255u, ((s >> 16) & 0xFFu) + FastDiv255(((d >> 16) & 0xFFu) * invA));
            const uint32_t oa = std::min<uint32_t>(255u, sa + FastDiv255((d >> 24) * invA));
            dstRow[x] = ob | (og << 8) | (or_ << 16) | (oa << 24);
        }
    }
}

// Быстрый композитинг кэшированного слоя контента поверх подложки с применением contentAlpha
// При совпадении габаритов — 1:1 проход, при морфинге — быстрое билинейное масштабирование без перерисовки GDI+
void CompositeContentLayerToDib(
    const BYTE* srcBits,
    int srcStride,
    int srcW,
    int srcH,
    BYTE* dstBits,
    int dstStride,
    int dstW,
    int dstH,
    BYTE contentAlpha)
{
    if (!srcBits || !dstBits || srcW <= 0 || srcH <= 0 || dstW <= 0 || dstH <= 0 || contentAlpha == 0) return;

    if (srcW == dstW && srcH == dstH) {
        if (contentAlpha == 255) {
            for (int y = 0; y < dstH; ++y) {
                const uint32_t* srcRow = reinterpret_cast<const uint32_t*>(srcBits + y * srcStride);
                uint32_t* dstRow = reinterpret_cast<uint32_t*>(dstBits + y * dstStride);
                for (int x = 0; x < dstW; ++x) {
                    const uint32_t s = srcRow[x];
                    if (s == 0u) continue;
                    const uint32_t sa = s >> 24;
                    if (sa == 255u) {
                        dstRow[x] = s;
                        continue;
                    }
                    const uint32_t d = dstRow[x];
                    const uint32_t invA = 255u - sa;
                    const uint32_t ob = std::min<uint32_t>(255u, (s & 0xFFu) + FastDiv255((d & 0xFFu) * invA));
                    const uint32_t og = std::min<uint32_t>(255u, ((s >> 8) & 0xFFu) + FastDiv255(((d >> 8) & 0xFFu) * invA));
                    const uint32_t or_ = std::min<uint32_t>(255u, ((s >> 16) & 0xFFu) + FastDiv255(((d >> 16) & 0xFFu) * invA));
                    const uint32_t oa = std::min<uint32_t>(255u, sa + FastDiv255((d >> 24) * invA));
                    dstRow[x] = ob | (og << 8) | (or_ << 16) | (oa << 24);
                }
            }
        } else {
            const uint32_t ca = contentAlpha;
            for (int y = 0; y < dstH; ++y) {
                const uint32_t* srcRow = reinterpret_cast<const uint32_t*>(srcBits + y * srcStride);
                uint32_t* dstRow = reinterpret_cast<uint32_t*>(dstBits + y * dstStride);
                for (int x = 0; x < dstW; ++x) {
                    const uint32_t s = srcRow[x];
                    if (s == 0u) continue;
                    const uint32_t sa = FastDiv255((s >> 24) * ca);
                    if (sa == 0u) continue;
                    const uint32_t sb = FastDiv255((s & 0xFFu) * ca);
                    const uint32_t sg = FastDiv255(((s >> 8) & 0xFFu) * ca);
                    const uint32_t sr = FastDiv255(((s >> 16) & 0xFFu) * ca);
                    const uint32_t d = dstRow[x];
                    const uint32_t invA = 255u - sa;
                    const uint32_t ob = std::min<uint32_t>(255u, sb + FastDiv255((d & 0xFFu) * invA));
                    const uint32_t og = std::min<uint32_t>(255u, sg + FastDiv255(((d >> 8) & 0xFFu) * invA));
                    const uint32_t or_ = std::min<uint32_t>(255u, sr + FastDiv255(((d >> 16) & 0xFFu) * invA));
                    const uint32_t oa = std::min<uint32_t>(255u, sa + FastDiv255((d >> 24) * invA));
                    dstRow[x] = ob | (og << 8) | (or_ << 16) | (oa << 24);
                }
            }
        }
        return;
    }

    const uint32_t ca = contentAlpha;
    const uint32_t stepX = (dstW > 1 && srcW > 1)
        ? ((static_cast<uint32_t>(srcW - 1) << 16) / static_cast<uint32_t>(dstW - 1))
        : 0u;
    const uint32_t stepY = (dstH > 1 && srcH > 1)
        ? ((static_cast<uint32_t>(srcH - 1) << 16) / static_cast<uint32_t>(dstH - 1))
        : 0u;

    uint32_t fy = 0u;
    for (int y = 0; y < dstH; ++y, fy += stepY) {
        int sy0 = static_cast<int>(fy >> 16);
        if (sy0 >= srcH - 1) sy0 = srcH - 1;
        const int sy1 = (sy0 + 1 < srcH) ? (sy0 + 1) : sy0;
        const uint32_t wy = (fy >> 8) & 0xFFu;
        const uint32_t invWy = 256u - wy;

        const uint32_t* srcRow0 = reinterpret_cast<const uint32_t*>(srcBits + sy0 * srcStride);
        const uint32_t* srcRow1 = reinterpret_cast<const uint32_t*>(srcBits + sy1 * srcStride);
        uint32_t* dstRow = reinterpret_cast<uint32_t*>(dstBits + y * dstStride);

        uint32_t fx = 0u;
        for (int x = 0; x < dstW; ++x, fx += stepX) {
            int sx0 = static_cast<int>(fx >> 16);
            if (sx0 >= srcW - 1) sx0 = srcW - 1;
            const int sx1 = (sx0 + 1 < srcW) ? (sx0 + 1) : sx0;

            const uint32_t p00 = srcRow0[sx0];
            const uint32_t p10 = srcRow0[sx1];
            const uint32_t p01 = srcRow1[sx0];
            const uint32_t p11 = srcRow1[sx1];
            if ((p00 | p10 | p01 | p11) == 0u) continue;

            const uint32_t wx = (fx >> 8) & 0xFFu;
            const uint32_t invWx = 256u - wx;
            const uint32_t w00 = invWx * invWy;
            const uint32_t w10 = wx * invWy;
            const uint32_t w01 = invWx * wy;
            const uint32_t w11 = wx * wy;

            const uint32_t a0 = (((p00 >> 24) * w00 + (p10 >> 24) * w10 + (p01 >> 24) * w01 + (p11 >> 24) * w11) + 32768u) >> 16;
            const uint32_t sa = FastDiv255(a0 * ca);
            if (sa == 0u) continue;

            const uint32_t b0 = (((p00 & 0xFFu) * w00 + (p10 & 0xFFu) * w10 + (p01 & 0xFFu) * w01 + (p11 & 0xFFu) * w11) + 32768u) >> 16;
            const uint32_t g0 = ((((p00 >> 8) & 0xFFu) * w00 + ((p10 >> 8) & 0xFFu) * w10 + ((p01 >> 8) & 0xFFu) * w01 + ((p11 >> 8) & 0xFFu) * w11) + 32768u) >> 16;
            const uint32_t r0 = ((((p00 >> 16) & 0xFFu) * w00 + ((p10 >> 16) & 0xFFu) * w10 + ((p01 >> 16) & 0xFFu) * w01 + ((p11 >> 16) & 0xFFu) * w11) + 32768u) >> 16;

            const uint32_t sb = FastDiv255(b0 * ca);
            const uint32_t sg = FastDiv255(g0 * ca);
            const uint32_t sr = FastDiv255(r0 * ca);

            const uint32_t d = dstRow[x];
            const uint32_t invA = 255u - sa;
            const uint32_t ob = std::min<uint32_t>(255u, sb + FastDiv255((d & 0xFFu) * invA));
            const uint32_t og = std::min<uint32_t>(255u, sg + FastDiv255(((d >> 8) & 0xFFu) * invA));
            const uint32_t or_ = std::min<uint32_t>(255u, sr + FastDiv255(((d >> 16) & 0xFFu) * invA));
            const uint32_t oa = std::min<uint32_t>(255u, sa + FastDiv255((d >> 24) * invA));
            dstRow[x] = ob | (og << 8) | (or_ << 16) | (oa << 24);
        }
    }
}

// Перестроение кэшированного слоя статичного контента (текст, шапка, плейсхолдеры, иконки при alpha=255)
void RebuildStaticContentLayer(int width, int height, int dpi) {
    if (!g_renderCache.EnsureContentBitmap(width, height)) return;

    BYTE* contentBits = static_cast<BYTE*>(g_renderCache.pvContentBits);
    const int contentStride = g_renderCache.contentBufW * 4;
    for (int y = 0; y < height; ++y) {
        memset(contentBits + y * contentStride, 0, static_cast<size_t>(width) * 4);
    }

    const int headerHeight = ScaleByDpi(42, dpi);
    const int clipTop = headerHeight + std::max<int>(1, ScaleByDpi(1, dpi));
    const int clipBottom = height - ScaleByDpi(4, dpi);
    const int sideClipMargin = ScaleByDpi(6, dpi);
    const bool isRu = IsRussianUILanguage();

    struct DeferredBakedIcon {
        const BakedIcon* baked;
        int x;
        int y;
    };
    std::vector<DeferredBakedIcon> deferredIcons;
    deferredIcons.reserve(24);

    {
        Graphics g(g_renderCache.hdcContent);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        g.SetCompositingMode(CompositingModeSourceOver);

        g_renderCache.titleBrush->SetColor(Color(255, 248, 250, 255));
        g_renderCache.subBrush->SetColor(Color(204, 248, 250, 255));
        g_renderCache.placeholderBrush->SetColor(Color(34, 255, 255, 255));

        if (g_portal.items.empty()) {
            RectF emptyRect(0.0f, static_cast<REAL>(clipTop), static_cast<REAL>(width), static_cast<REAL>(clipBottom - clipTop));
            LPCWSTR statusText = g_portal.isLoadingDirectory
                ? (isRu ? L"Загрузка..." : L"Loading...")
                : (isRu ? L"Папка пуста" : L"Folder is empty");
            g.DrawString(statusText, -1, g_renderCache.itemFont, emptyRect, g_renderCache.centerFormat, g_renderCache.subBrush);
            g_portal.maxScrollY = 0;
        } else {
            int cols = 1;
            int cellHeight = 1;
            int contentTopMargin = 0;
            GetPortalLayoutMetrics(dpi, cols, cellHeight, contentTopMargin);

            int totalRows = (static_cast<int>(g_portal.items.size()) + cols - 1) / cols;
            int totalContentHeight = totalRows * cellHeight + ScaleByDpi(12, dpi);
            int visibleGridHeight = std::max<int>(1, clipBottom - clipTop);
            g_portal.maxScrollY = std::max<int>(0, totalContentHeight - visibleGridHeight);
            g_portal.scrollOffsetY = std::clamp(g_portal.scrollOffsetY, 0, g_portal.maxScrollY);

            int gridTopY = headerHeight + contentTopMargin - g_portal.scrollOffsetY;

            Region gridClip(Rect(sideClipMargin, clipTop, std::max<int>(1, width - sideClipMargin * 2), visibleGridHeight));
            g.SetClip(&gridClip, CombineModeReplace);

            if (g_settings.listViewMode) {
                int listPaddingX = ScaleByDpi(10, dpi);
                int rowPadY = ScaleByDpi(2, dpi);
                int iconSize = ScaleByDpi(22, dpi);
                int iconLeftPad = ScaleByDpi(8, dpi);
                int textGap = ScaleByDpi(10, dpi);
                int rightInnerPad = ScaleByDpi(8, dpi);
                int metaWidth = ScaleByDpi(68, dpi);
                int placeholderRad = ScaleByDpi(6, dpi);

                for (size_t i = 0; i < g_portal.items.size(); ++i) {
                    int rowY = gridTopY + static_cast<int>(i) * cellHeight;
                    RECT itemRc = { listPaddingX, rowY + rowPadY, width - listPaddingX, rowY + cellHeight - rowPadY };
                    g_portal.items[i].boundsRect = itemRc;

                    if (itemRc.bottom <= clipTop || itemRc.top >= clipBottom) {
                        continue;
                    }

                    int iconX = itemRc.left + iconLeftPad;
                    int iconY = rowY + (cellHeight - iconSize) / 2;
                    if (g_portal.items[i].bakedIcon && g_portal.items[i].bakedIcon->size == iconSize) {
                        deferredIcons.push_back({ g_portal.items[i].bakedIcon.get(), iconX, iconY });
                    } else {
                        GraphicsPath* pPhPath = g_renderCache.GetPlaceholderPath(iconSize, placeholderRad);
                        if (pPhPath) {
                            g.TranslateTransform(static_cast<REAL>(iconX), static_cast<REAL>(iconY));
                            g.FillPath(g_renderCache.placeholderBrush, pPhPath);
                            g.TranslateTransform(static_cast<REAL>(-iconX), static_cast<REAL>(-iconY));
                        }
                    }

                    int nameX = iconX + iconSize + textGap;
                    int contentRight = itemRc.right - rightInnerPad;
                    bool hasMeta = g_settings.showFileMetadata && !g_portal.items[i].sizeOrTypeText.empty();
                    int nameW = hasMeta
                        ? std::max<int>(ScaleByDpi(40, dpi), contentRight - metaWidth - ScaleByDpi(8, dpi) - nameX)
                        : std::max<int>(ScaleByDpi(40, dpi), contentRight - nameX);

                    RectF nameRect(static_cast<REAL>(nameX),
                                   static_cast<REAL>(rowY),
                                   static_cast<REAL>(nameW),
                                   static_cast<REAL>(cellHeight));
                    g.DrawString(g_portal.items[i].name.c_str(), -1, g_renderCache.itemFont, nameRect, g_renderCache.leftFormat, g_renderCache.titleBrush);

                    if (hasMeta) {
                        RectF metaRect(static_cast<REAL>(contentRight - metaWidth),
                                       static_cast<REAL>(rowY),
                                       static_cast<REAL>(metaWidth),
                                       static_cast<REAL>(cellHeight));
                        g.DrawString(g_portal.items[i].sizeOrTypeText.c_str(), -1, g_renderCache.metaFont, metaRect, g_renderCache.rightFormat, g_renderCache.subBrush);
                    }
                }
            } else {
                int gridPaddingX = ScaleByDpi(12, dpi);
                int availWidth = width - (gridPaddingX * 2);
                int cellWidth = availWidth / cols;

                int padX = ScaleByDpi(3, dpi);
                int padY = ScaleByDpi(2, dpi);
                int iconSize = ScaleByDpi(32, dpi);
                int iconTopOffset = ScaleByDpi(8, dpi);
                int textPadX = ScaleByDpi(4, dpi);
                int nameOffsetY = ScaleByDpi(4, dpi);
                int nameHeight = ScaleByDpi(18, dpi);
                int metaOffsetY = ScaleByDpi(21, dpi);
                int metaHeight = ScaleByDpi(17, dpi);
                int placeholderRad = ScaleByDpi(8, dpi);

                for (size_t i = 0; i < g_portal.items.size(); ++i) {
                    int row = static_cast<int>(i) / cols;
                    int col = static_cast<int>(i) % cols;

                    int cellX = gridPaddingX + col * cellWidth;
                    int cellY = gridTopY + row * cellHeight;

                    RECT itemRc = { cellX + padX, cellY + padY, cellX + cellWidth - padX, cellY + cellHeight - padY };
                    g_portal.items[i].boundsRect = itemRc;

                    if (itemRc.bottom <= clipTop || itemRc.top >= clipBottom) {
                        continue;
                    }

                    int iconX = cellX + (cellWidth - iconSize) / 2;
                    int iconY = cellY + iconTopOffset;
                    if (g_portal.items[i].bakedIcon && g_portal.items[i].bakedIcon->size == iconSize) {
                        deferredIcons.push_back({ g_portal.items[i].bakedIcon.get(), iconX, iconY });
                    } else {
                        GraphicsPath* pPhPath = g_renderCache.GetPlaceholderPath(iconSize, placeholderRad);
                        if (pPhPath) {
                            g.TranslateTransform(static_cast<REAL>(iconX), static_cast<REAL>(iconY));
                            g.FillPath(g_renderCache.placeholderBrush, pPhPath);
                            g.TranslateTransform(static_cast<REAL>(-iconX), static_cast<REAL>(-iconY));
                        }
                    }

                    RectF nameRect(static_cast<REAL>(cellX + textPadX),
                                   static_cast<REAL>(iconY + iconSize + nameOffsetY),
                                   static_cast<REAL>(cellWidth - textPadX * 2),
                                   static_cast<REAL>(nameHeight));
                    g.DrawString(g_portal.items[i].name.c_str(), -1, g_renderCache.itemFont, nameRect, g_renderCache.itemFormat, g_renderCache.titleBrush);

                    if (g_settings.showFileMetadata && !g_portal.items[i].sizeOrTypeText.empty()) {
                        RectF metaRect(static_cast<REAL>(cellX + textPadX),
                                       static_cast<REAL>(iconY + iconSize + metaOffsetY),
                                       static_cast<REAL>(cellWidth - textPadX * 2),
                                       static_cast<REAL>(metaHeight));
                        g.DrawString(g_portal.items[i].sizeOrTypeText.c_str(), -1, g_renderCache.metaFont, metaRect, g_renderCache.itemFormat, g_renderCache.subBrush);
                    }
                }
            }

            g.ResetClip();
        }

        // Шапка портала поверх всего (одна тонкая линия на всю ширину окна)
        int titleLeft = ScaleByDpi(16, dpi);
        int titleTop = ScaleByDpi(6, dpi);
        int titleRightMargin = ScaleByDpi(105, dpi);
        int titleHeight = ScaleByDpi(30, dpi);

        RectF titleRect(static_cast<REAL>(titleLeft),
                        static_cast<REAL>(titleTop),
                        static_cast<REAL>(width - titleRightMargin),
                        static_cast<REAL>(titleHeight));
        g.DrawString(g_portal.folderTitle.c_str(), -1, g_renderCache.titleFont, titleRect, g_renderCache.leftFormat, g_renderCache.titleBrush);

        wchar_t countText[64] = {};
        if (g_portal.isLoadingDirectory) {
            wcscpy_s(countText, L"...");
        } else {
            UINT itemCount = static_cast<UINT>(g_portal.totalItemCount);
            if (isRu) {
                wsprintfW(countText, L"%u объект.", itemCount);
            } else {
                wsprintfW(countText, itemCount == 1 ? L"%u item" : L"%u items", itemCount);
            }
        }
        RectF countRect(static_cast<REAL>(width - ScaleByDpi(95, dpi)),
                        static_cast<REAL>(titleTop),
                        static_cast<REAL>(ScaleByDpi(79, dpi)),
                        static_cast<REAL>(titleHeight));
        g.DrawString(countText, -1, g_renderCache.countFont, countRect, g_renderCache.rightFormat, g_renderCache.subBrush);

        g_renderCache.headerLinePen->SetColor(Color(38, 255, 255, 255));
        g.DrawLine(g_renderCache.headerLinePen, 1, headerHeight, width - 2, headerHeight);
    }

    GdiFlush();

    for (const auto& iconDraw : deferredIcons) {
        BlitBakedIconToDib(
            *iconDraw.baked,
            contentBits,
            contentStride,
            width,
            height,
            iconDraw.x,
            iconDraw.y,
            sideClipMargin,
            clipTop,
            width - sideClipMargin,
            clipBottom);
    }

    g_renderCache.cachedContentW = width;
    g_renderCache.cachedContentH = height;
    g_renderCache.cachedContentDpi = dpi;
    g_renderCache.cachedContentScrollY = g_portal.scrollOffsetY;
    g_renderCache.cachedContentRevision = g_portal.contentRevision;
}

// Отрисовка окна через UpdateLayeredWindow с двухуровневым композитингом (кэшированный контент + динамические слои)
void RenderAndPresentPortal(HWND hWnd) {
    if (!IsWindow(hWnd)) return;

    const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
    int width  = std::max<int>(ScaleByDpi(16, dpi), g_portal.currentRect.right - g_portal.currentRect.left);
    int height = std::max<int>(ScaleByDpi(16, dpi), g_portal.currentRect.bottom - g_portal.currentRect.top);
    const int targetW = std::max<int>(ScaleByDpi(16, dpi), g_portal.targetPortalRect.right - g_portal.targetPortalRect.left);
    const int targetH = std::max<int>(ScaleByDpi(16, dpi), g_portal.targetPortalRect.bottom - g_portal.targetPortalRect.top);
    const bool isFinalSize = (width == targetW && height == targetH);

    g_renderCache.EnsureFonts(dpi);
    g_renderCache.EnsurePrimitives(dpi);
    if (!g_renderCache.EnsureBitmap(width, height)) return;

    HDC hdcMem = g_renderCache.hdcMem;
    BYTE* pvBits = static_cast<BYTE*>(g_renderCache.pvBits);
    const int stride = g_renderCache.bufW * 4;

    float animAlphaFactor = std::clamp(g_portal.animProgress, 0.0f, 1.0f);
    int baseBgAlpha = std::clamp<int>(g_settings.bgOpacity, 10, 255);
    if (g_settings.enableBlur) {
        if (g_settings.blurStrength == 2) baseBgAlpha = std::min<int>(255, baseBgAlpha + 12);
        else if (g_settings.blurStrength == 3) baseBgAlpha = std::min<int>(255, baseBgAlpha + 26);
    }
    BYTE bgAlpha = static_cast<BYTE>(std::round(static_cast<float>(baseBgAlpha) * animAlphaFactor));

    float contentPhase = std::clamp((g_portal.animProgress - 0.2f) / 0.8f, 0.0f, 1.0f);
    BYTE contentAlpha = static_cast<BYTE>(std::round(255.0f * contentPhase));

    const float targetCornerRadius = static_cast<float>(ScaleByDpi(8, dpi));
    const float startCornerRadius = static_cast<float>(width) * 0.5f;
    const float morphT = std::clamp(g_portal.morphProgress, 0.0f, 1.0f);
    float morphRadius = startCornerRadius + (targetCornerRadius - startCornerRadius) * morphT;
    morphRadius = std::clamp(morphRadius, 0.0f, static_cast<float>(std::min<int>(width, height)) * 0.5f);

    const int headerHeight = ScaleByDpi(42, dpi);
    const int clipTop = headerHeight + std::max<int>(1, ScaleByDpi(1, dpi));
    const int clipBottom = height - ScaleByDpi(4, dpi);
    const int sideClipMargin = ScaleByDpi(6, dpi);
    g_portal.scrollbarTrackHeight = 0;
    g_portal.scrollbarThumbHeight = 0;

    // 1. Быстрая заливка подложки premultiplied BGRA (без накладных расходов GDI+ FillRectangle)
    const uint32_t bgB = FastDiv255(32u * bgAlpha);
    const uint32_t bgG = FastDiv255(27u * bgAlpha);
    const uint32_t bgR = FastDiv255(26u * bgAlpha);
    const uint32_t bgPixel = bgB | (bgG << 8) | (bgR << 16) | (static_cast<uint32_t>(bgAlpha) << 24);
    for (int y = 0; y < height; ++y) {
        uint32_t* row = reinterpret_cast<uint32_t*>(pvBits + y * stride);
        std::fill_n(row, width, bgPixel);
    }

    const bool drawContent = (contentPhase > 0.05f && width > ScaleByDpi(110, dpi) && height > ScaleByDpi(80, dpi));
    if (drawContent) {
        // 2. Перестраиваем статичный слой контента ОДИН РАЗ в финальных габаритах targetPortalRect
        if (g_renderCache.cachedContentW != targetW ||
            g_renderCache.cachedContentH != targetH ||
            g_renderCache.cachedContentDpi != dpi ||
            g_renderCache.cachedContentScrollY != g_portal.scrollOffsetY ||
            g_renderCache.cachedContentRevision != g_portal.contentRevision) {
            RebuildStaticContentLayer(targetW, targetH, dpi);
        }

        // 3. Подсветка наведённой карточки (рисуется под слоем текста и иконок в открытом состоянии)
        if (isFinalSize &&
            g_portal.hoveredGridIndex >= 0 &&
            g_portal.hoveredGridIndex < static_cast<int>(g_portal.items.size())) {
            const RECT& itemRc = g_portal.items[g_portal.hoveredGridIndex].boundsRect;
            if (itemRc.bottom > clipTop && itemRc.top < clipBottom) {
                Graphics gHover(hdcMem);
                gHover.SetSmoothingMode(SmoothingModeAntiAlias);
                int visibleGridHeight = std::max<int>(1, clipBottom - clipTop);
                Region gridClip(Rect(sideClipMargin, clipTop, std::max<int>(1, width - sideClipMargin * 2), visibleGridHeight));
                gHover.SetClip(&gridClip, CombineModeReplace);

                int hoverRad = ScaleByDpi(g_settings.listViewMode ? 8 : 10, dpi);
                GraphicsPath* pHoverPath = g_renderCache.GetHoverPath(
                    itemRc.right - itemRc.left,
                    itemRc.bottom - itemRc.top,
                    hoverRad);
                if (pHoverPath) {
                    g_renderCache.hoverFillBrush->SetColor(Color(static_cast<BYTE>(50.0f * contentPhase), 255, 255, 255));
                    g_renderCache.hoverBorderPen->SetColor(Color(static_cast<BYTE>(95.0f * contentPhase), 255, 255, 255));
                    gHover.TranslateTransform(static_cast<REAL>(itemRc.left), static_cast<REAL>(itemRc.top));
                    gHover.FillPath(g_renderCache.hoverFillBrush, pHoverPath);
                    gHover.DrawPath(g_renderCache.hoverBorderPen, pHoverPath);
                    gHover.TranslateTransform(static_cast<REAL>(-itemRc.left), static_cast<REAL>(-itemRc.top));
                }
                GdiFlush();
            }
        }

        // 4. Композитинг кэшированного контента (1:1 в открытом окне или быстрое масштабирование при морфинге)
        if (g_renderCache.pvContentBits) {
            CompositeContentLayerToDib(
                static_cast<const BYTE*>(g_renderCache.pvContentBits),
                g_renderCache.contentBufW * 4,
                targetW,
                targetH,
                pvBits,
                stride,
                width,
                height,
                contentAlpha);
        }

        // 5. Тонкий полупрозрачный скроллбар поверх контента при наведении на правый край окна
        if (isFinalSize && g_portal.maxScrollY > 0 && (g_portal.scrollbarEdgeHovered || g_portal.isDraggingScrollbar)) {
            int cols = 1;
            int cellHeight = 1;
            int contentTopMargin = 0;
            GetPortalLayoutMetrics(dpi, cols, cellHeight, contentTopMargin);
            int totalRows = (static_cast<int>(g_portal.items.size()) + cols - 1) / cols;
            int totalContentHeight = totalRows * cellHeight + ScaleByDpi(12, dpi);
            int visibleGridHeight = std::max<int>(1, clipBottom - clipTop);

            int thumbW = std::max<int>(3, ScaleByDpi(4, dpi));
            int thumbMarginRight = ScaleByDpi(3, dpi);
            int trackTop = clipTop + ScaleByDpi(4, dpi);
            int trackBottom = clipBottom - ScaleByDpi(4, dpi);
            int trackHeight = trackBottom - trackTop;
            if (trackHeight > ScaleByDpi(16, dpi)) {
                int minThumbH = ScaleByDpi(24, dpi);
                int thumbH = std::clamp<int>(
                    MulDiv(trackHeight, visibleGridHeight, std::max<int>(1, totalContentHeight)),
                    minThumbH,
                    trackHeight);
                int maxThumbTravel = std::max<int>(0, trackHeight - thumbH);
                int thumbY = trackTop + MulDiv(maxThumbTravel, g_portal.scrollOffsetY, g_portal.maxScrollY);
                int thumbX = width - thumbMarginRight - thumbW;

                g_portal.scrollbarTrackTop = trackTop;
                g_portal.scrollbarTrackHeight = trackHeight;
                g_portal.scrollbarThumbY = thumbY;
                g_portal.scrollbarThumbHeight = thumbH;

                Graphics gSb(hdcMem);
                gSb.SetSmoothingMode(SmoothingModeAntiAlias);
                BYTE sbAlpha = static_cast<BYTE>(std::round((g_portal.isDraggingScrollbar ? 165.0f : 115.0f) * contentPhase));
                g_renderCache.scrollbarBrush->SetColor(Color(sbAlpha, 255, 255, 255));
                RenderCache::BuildRoundedRectPathF(
                    g_renderCache.scrollbarPath,
                    static_cast<REAL>(thumbX),
                    static_cast<REAL>(thumbY),
                    static_cast<REAL>(thumbW),
                    static_cast<REAL>(thumbH),
                    static_cast<REAL>(thumbW) * 0.5f);
                gSb.FillPath(g_renderCache.scrollbarBrush, g_renderCache.scrollbarPath);
                GdiFlush();
            }
        }
    }

    // 6. Честная маска морфинга формы в DIB-буфере (интерполяция от круга R = width / 2 до R = 8 px)
    ApplyRoundedRectMaskToDib(pvBits, stride, width, height, morphRadius);

    // 7. Тонкая светлая рамка строго по текущему радиусу морфинга
    int brAlpha = static_cast<int>(std::round(static_cast<float>(std::clamp(g_settings.borderBrightness, 0, 255)) * animAlphaFactor));
    if (brAlpha > 0) {
        Graphics gBorder(hdcMem);
        gBorder.SetSmoothingMode(SmoothingModeAntiAlias);
        const REAL borderInset = 0.6f * (static_cast<REAL>(dpi) / 96.0f);
        GraphicsPath* pBorderPath = g_renderCache.GetBorderPath(
            width,
            height,
            std::max<REAL>(0.0f, static_cast<REAL>(morphRadius) - borderInset),
            borderInset);
        if (pBorderPath) {
            g_renderCache.lightBorderPen->SetColor(Color(static_cast<BYTE>(brAlpha), 240, 246, 255));
            gBorder.DrawPath(g_renderCache.lightBorderPen, pBorderPath);
        }
    }

    POINT ptWinPos = { g_portal.currentRect.left, g_portal.currentRect.top };
    SIZE szWin = { width, height };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, nullptr, &ptWinPos, &szWin, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
}

void UpdatePortalMorphGeometry(HWND hPortal, float rawProgress, bool isOpening) {
    if (!IsWindow(hPortal)) return;

    float eased = isOpening ? EaseOutCubic(rawProgress) : (1.0f - EaseInCubic(1.0f - rawProgress));
    eased = std::clamp(eased, 0.0f, 1.0f);
    g_portal.morphProgress = eased;

    RECT r = {};
    r.left   = LerpInt(g_portal.startCircleRect.left,   g_portal.targetPortalRect.left,   eased);
    r.top    = LerpInt(g_portal.startCircleRect.top,    g_portal.targetPortalRect.top,    eased);
    r.right  = LerpInt(g_portal.startCircleRect.right,  g_portal.targetPortalRect.right,  eased);
    r.bottom = LerpInt(g_portal.startCircleRect.bottom, g_portal.targetPortalRect.bottom, eased);
    g_portal.currentRect = r;

    RenderAndPresentPortal(hPortal);
}

void ResetPortalTransientState() {
    g_portal.animState = PortalAnimState::Hidden;
    g_portal.morphProgress = 1.0f;
    g_portal.activeItemIndex = -1;
    g_portal.hoveredGridIndex = -1;
    g_portal.pressedGridIndex = -1;
    g_portal.isDraggingOrMenuActive = false;
    g_portal.scrollbarEdgeHovered = false;
    g_portal.isDraggingScrollbar = false;
    g_portal.scrollbarTrackHeight = 0;
    g_portal.scrollbarThumbHeight = 0;
    g_portal.scrollOffsetY = 0;
    g_portal.totalItemCount = 0;
    g_portal.renderDirty = false;
    g_portal.frameTimerActive = false;
    g_portal.isScrolling = false;
    g_portal.isLoadingDirectory = false;
    g_portal.mouseTrackingActive = false;
    ++g_portal.contentRevision;
    g_portal.items.clear();

    if (g_portal.pShellBrowser) {
        g_portal.pShellBrowser->Release();
        g_portal.pShellBrowser = nullptr;
    }
}

// Скрытие окна-портала без уничтожения HWND (Window Pooling: ноль задержек CreateWindowExW + DWM при следующем открытии)
void HidePortalImmediately() {
    g_portalGenerationId.fetch_add(1, std::memory_order_acq_rel);
    g_asyncWorker.ClearCompletedResults();
    g_vsyncAnimator.StopAnimation();

    if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
        KillTimer(g_portal.hPortalWnd, TIMER_ID_MOUSECHECK);
        KillTimer(g_portal.hPortalWnd, TIMER_ID_FRAME_RENDER);
        KillTimer(g_portal.hPortalWnd, TIMER_ID_SCROLL_IDLE);
        if (g_portal.dwmBlurActive) {
            SetPortalBlurEnabled(g_portal.hPortalWnd, false);
        }
        ShowWindow(g_portal.hPortalWnd, SW_HIDE);
    }
    ResetPortalTransientState();
}

void DestroyPortalImmediately() {
    g_portalGenerationId.fetch_add(1, std::memory_order_acq_rel);
    g_asyncWorker.ClearCompletedResults();
    g_vsyncAnimator.StopAnimation();

    if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
        KillTimer(g_portal.hPortalWnd, TIMER_ID_MOUSECHECK);
        KillTimer(g_portal.hPortalWnd, TIMER_ID_FRAME_RENDER);
        KillTimer(g_portal.hPortalWnd, TIMER_ID_SCROLL_IDLE);
        DestroyWindow(g_portal.hPortalWnd);
    }
    g_portal.hPortalWnd = nullptr;
    g_portal.dwmVisualsDirty = true;
    g_portal.dwmBlurActive = false;
    ResetPortalTransientState();
}

void StartPortalFadeOut() {
    if (!g_portal.hPortalWnd || !IsWindow(g_portal.hPortalWnd)) {
        DestroyPortalImmediately();
        return;
    }
    if (g_portal.animState == PortalAnimState::Closing || g_portal.animState == PortalAnimState::Hidden) {
        return;
    }
    if (g_portal.dwmBlurActive) {
        SetPortalBlurEnabled(g_portal.hPortalWnd, false);
    }
    g_portal.animState = PortalAnimState::Closing;
    g_portal.animStartMs = GetQpcNowMs();
    g_vsyncAnimator.StartAnimation(g_portal.hPortalWnd, g_portal.frameIntervalMs);
}

// Математический O(1) hit-test элемента в сетке/списке с точной проверкой boundsRect
int HitTestPortalItem(POINT pt) {
    if (g_portal.items.empty()) return -1;
    const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
    const int headerHeight = ScaleByDpi(42, dpi);
    if (pt.y <= headerHeight) return -1;

    int cols = 1;
    int cellHeight = 1;
    int contentTopMargin = 0;
    GetPortalLayoutMetrics(dpi, cols, cellHeight, contentTopMargin);

    const int gridTopY = headerHeight + contentTopMargin - g_portal.scrollOffsetY;
    const int relY = pt.y - gridTopY;
    if (relY < 0) return -1;

    const int row = relY / cellHeight;
    int idx = -1;

    if (g_settings.listViewMode) {
        idx = row;
    } else {
        const int width = std::max<int>(1, g_portal.currentRect.right - g_portal.currentRect.left);
        const int gridPaddingX = ScaleByDpi(12, dpi);
        const int availWidth = width - (gridPaddingX * 2);
        const int cellWidth = availWidth / cols;
        if (cellWidth <= 0) return -1;
        const int relX = pt.x - gridPaddingX;
        if (relX < 0) return -1;
        const int col = relX / cellWidth;
        if (col >= cols) return -1;
        idx = row * cols + col;
    }

    if (idx >= 0 && idx < static_cast<int>(g_portal.items.size())) {
        if (PtInRect(&g_portal.items[idx].boundsRect, pt)) {
            return idx;
        }
    }
    return -1;
}

// Проверяет, находится ли курсор мыши в зоне иконки папки, окна портала или в коридоре между ними
bool IsCursorInsideSafeZone() {
    if (g_portal.isDraggingOrMenuActive || g_portal.isDraggingScrollbar) return true;

    POINT pt = {};
    if (!GetCursorPos(&pt)) return false;

    const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;

    RECT rcPortal = g_portal.currentRect;
    InflateRect(&rcPortal, ScaleByDpi(14, dpi), ScaleByDpi(14, dpi));
    if (PtInRect(&rcPortal, pt)) return true;

    RECT rcIcon = g_portal.iconScreenRect;
    InflateRect(&rcIcon, ScaleByDpi(12, dpi), ScaleByDpi(12, dpi));
    if (PtInRect(&rcIcon, pt)) return true;

    // Соединительный мостик между папкой и всплывшим справа/слева окном портала
    RECT rcBridge = {};
    rcBridge.left   = std::min<LONG>(rcIcon.left, rcPortal.left);
    rcBridge.right  = std::max<LONG>(rcIcon.right, rcPortal.right);
    rcBridge.top    = std::min<LONG>(rcIcon.top, rcPortal.top);
    rcBridge.bottom = std::max<LONG>(rcIcon.bottom, rcPortal.bottom);

    if (PtInRect(&rcBridge, pt)) {
        if (pt.y >= rcIcon.top - ScaleByDpi(24, dpi) && pt.y <= rcPortal.bottom + ScaleByDpi(16, dpi)) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Оконная процедура окна-портала (Layered Window)
// ---------------------------------------------------------------------------
LRESULT CALLBACK PortalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Перенаправляем сообщения отрисовки подменю в активный IContextMenu2/3 (например "Открыть с помощью" / "Отправить")
    if (uMsg == WM_INITMENUPOPUP || uMsg == WM_DRAWITEM || uMsg == WM_MEASUREITEM) {
        if (g_portal.pActiveCtxMenu3) {
            LRESULT lres = 0;
            if (SUCCEEDED(g_portal.pActiveCtxMenu3->HandleMenuMsg2(uMsg, wParam, lParam, &lres))) {
                return lres;
            }
        } else if (g_portal.pActiveCtxMenu2) {
            if (SUCCEEDED(g_portal.pActiveCtxMenu2->HandleMenuMsg(uMsg, wParam, lParam))) {
                return 0;
            }
        }
    } else if (uMsg == WM_MENUCHAR && g_portal.pActiveCtxMenu3) {
        LRESULT lres = 0;
        if (SUCCEEDED(g_portal.pActiveCtxMenu3->HandleMenuMsg2(uMsg, wParam, lParam, &lres))) {
            return lres;
        }
    }

    switch (uMsg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_APP_PORTAL_ASYNC_READY: {
        ProcessAsyncPortalResults(hWnd);
        return 0;
    }

    case WM_APP_PORTAL_VSYNC_TICK: {
        g_vsyncAnimator.AcknowledgeTick();
        if (g_portal.animState != PortalAnimState::Opening &&
            g_portal.animState != PortalAnimState::Closing) {
            return 0;
        }

        const double nowMs = GetQpcNowMs();
        const double durationMs = static_cast<double>(std::clamp(g_settings.animDurationMs, 60, 1000));
        const float t = std::clamp(static_cast<float>((nowMs - g_portal.animStartMs) / durationMs), 0.0f, 1.0f);

        if (g_portal.animState == PortalAnimState::Opening) {
            g_portal.animProgress = t;
            g_portal.renderDirty = false;
            UpdatePortalMorphGeometry(hWnd, g_portal.animProgress, true);
            if (t >= 1.0f) {
                g_portal.animState = PortalAnimState::Visible;
                g_vsyncAnimator.StopAnimation();
                if (g_settings.enableBlur && !g_portal.dwmBlurActive) {
                    SetPortalBlurEnabled(hWnd, true);
                }
                if (g_portal.renderDirty) {
                    RequestPortalRepaint(hWnd);
                }
            }
        } else if (g_portal.animState == PortalAnimState::Closing) {
            g_portal.animProgress = 1.0f - t;
            g_portal.renderDirty = false;
            UpdatePortalMorphGeometry(hWnd, g_portal.animProgress, false);
            if (t >= 1.0f) {
                g_vsyncAnimator.StopAnimation();
                HidePortalImmediately();
            }
        }
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps = {};
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        RequestPortalRepaint(hWnd);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (!g_portal.mouseTrackingActive) {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
            if (TrackMouseEvent(&tme)) {
                g_portal.mouseTrackingActive = true;
            }
        }

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        if (g_portal.isDraggingScrollbar && (wParam & MK_LBUTTON) && g_portal.maxScrollY > 0) {
            int trackRange = std::max<int>(1, g_portal.scrollbarTrackHeight - g_portal.scrollbarThumbHeight);
            int deltaY = pt.y - g_portal.scrollbarDragStartY;
            int newScroll = std::clamp<int>(
                g_portal.scrollbarDragStartOffset + MulDiv(deltaY, g_portal.maxScrollY, trackRange),
                0,
                g_portal.maxScrollY);
            if (newScroll != g_portal.scrollOffsetY) {
                g_portal.scrollOffsetY = newScroll;
                g_portal.isScrolling = true;
                RequestPortalRepaint(hWnd);
                SetTimer(hWnd, TIMER_ID_SCROLL_IDLE, 140, nullptr);
            }
            return 0;
        }

        // Если зажата левая кнопка мыши на файле и курсор сдвинулся — начинаем перетаскивание файла (Drag-and-Drop)
        if ((wParam & MK_LBUTTON) && g_portal.pressedGridIndex >= 0 && !g_portal.isDraggingOrMenuActive) {
            int dx = std::abs(pt.x - g_portal.pressStartPt.x);
            int dy = std::abs(pt.y - g_portal.pressStartPt.y);
            int dragCx = std::max<int>(4, GetSystemMetrics(SM_CXDRAG));
            int dragCy = std::max<int>(4, GetSystemMetrics(SM_CYDRAG));
            if (dx >= dragCx || dy >= dragCy) {
                int dragIdx = g_portal.pressedGridIndex;
                g_portal.pressedGridIndex = -1;
                ReleaseCapture();
                StartNativeFileDragAndDrop(hWnd, dragIdx);
                return 0;
            }
        }

        const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
        const int width = std::max<int>(1, g_portal.currentRect.right - g_portal.currentRect.left);
        const int headerHitBottom = ScaleByDpi(42, dpi);
        const int rightEdgeZone = ScaleByDpi(18, dpi);
        const bool nearRightEdge = (g_portal.maxScrollY > 0) &&
                                   (pt.y > headerHitBottom) &&
                                   (pt.x >= width - rightEdgeZone) &&
                                   (pt.x <= width);
        bool needsRepaint = false;
        if (nearRightEdge != g_portal.scrollbarEdgeHovered) {
            g_portal.scrollbarEdgeHovered = nearRightEdge;
            needsRepaint = true;
        }

        int newHover = -1;
        const bool overScrollbarStrip = (g_portal.maxScrollY > 0) &&
                                        (g_portal.scrollbarEdgeHovered || g_portal.isDraggingScrollbar) &&
                                        (pt.x >= width - ScaleByDpi(12, dpi));
        if (pt.y > headerHitBottom && !overScrollbarStrip) {
            newHover = HitTestPortalItem(pt);
        }
        if (newHover != g_portal.hoveredGridIndex) {
            g_portal.hoveredGridIndex = newHover;
            needsRepaint = true;
        }
        if (needsRepaint) {
            RequestPortalRepaint(hWnd);
        }
        return 0;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (g_portal.maxScrollY > 0) {
            int step = ScaleByDpi(delta / 3, g_portal.dpi > 0 ? g_portal.dpi : 96);
            int newScroll = std::clamp(g_portal.scrollOffsetY - step, 0, g_portal.maxScrollY);
            if (newScroll != g_portal.scrollOffsetY) {
                g_portal.scrollOffsetY = newScroll;
                g_portal.isScrolling = true;
                RequestPortalRepaint(hWnd);
                SetTimer(hWnd, TIMER_ID_SCROLL_IDLE, 140, nullptr);
            }
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_portal.pressedGridIndex = -1;
        const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
        const int width = std::max<int>(1, g_portal.currentRect.right - g_portal.currentRect.left);
        const int headerHitBottom = ScaleByDpi(42, dpi);

        if (g_portal.maxScrollY > 0 && g_portal.scrollbarTrackHeight > 0 &&
            pt.y > headerHitBottom && pt.x >= width - ScaleByDpi(12, dpi)) {
            g_portal.isDraggingScrollbar = true;
            g_portal.scrollbarEdgeHovered = true;
            if (pt.y < g_portal.scrollbarThumbY || pt.y > g_portal.scrollbarThumbY + g_portal.scrollbarThumbHeight) {
                int trackRange = std::max<int>(1, g_portal.scrollbarTrackHeight - g_portal.scrollbarThumbHeight);
                int targetThumbTop = std::clamp<int>(
                    pt.y - g_portal.scrollbarThumbHeight / 2 - g_portal.scrollbarTrackTop,
                    0,
                    trackRange);
                g_portal.scrollOffsetY = std::clamp<int>(
                    MulDiv(targetThumbTop, g_portal.maxScrollY, trackRange),
                    0,
                    g_portal.maxScrollY);
            }
            g_portal.scrollbarDragStartY = pt.y;
            g_portal.scrollbarDragStartOffset = g_portal.scrollOffsetY;
            SetCapture(hWnd);
            RequestPortalRepaint(hWnd);
            return 0;
        }

        if (pt.y > headerHitBottom) {
            int hitIdx = HitTestPortalItem(pt);
            if (hitIdx >= 0) {
                g_portal.pressedGridIndex = hitIdx;
                g_portal.pressStartPt = pt;
                SetCapture(hWnd);
            }
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (g_portal.isDraggingScrollbar) {
            if (GetCapture() == hWnd) {
                ReleaseCapture();
            }
            g_portal.isDraggingScrollbar = false;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
            const int width = std::max<int>(1, g_portal.currentRect.right - g_portal.currentRect.left);
            const int headerHitBottom = ScaleByDpi(42, dpi);
            g_portal.scrollbarEdgeHovered = (g_portal.maxScrollY > 0) &&
                                            (pt.y > headerHitBottom) &&
                                            (pt.x >= width - ScaleByDpi(18, dpi)) &&
                                            (pt.x <= width);
            RequestPortalRepaint(hWnd);
            ScheduleVisibleIconsLoad(hWnd);
            return 0;
        }

        if (GetCapture() == hWnd) {
            ReleaseCapture();
        }
        int clickedIndex = g_portal.pressedGridIndex;
        g_portal.pressedGridIndex = -1;

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        const int headerHitBottom = ScaleByDpi(42, g_portal.dpi > 0 ? g_portal.dpi : 96);
        if (pt.y > headerHitBottom && clickedIndex >= 0 && clickedIndex < static_cast<int>(g_portal.items.size())) {
            if (PtInRect(&g_portal.items[clickedIndex].boundsRect, pt)) {
                std::wstring targetPath = g_portal.items[clickedIndex].fullPath;
                if (!targetPath.empty() && !g_unloading.load()) {
                    ScopedModalCallGuard modalGuard;
                    PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(targetPath.c_str());
                    if (pidl && g_portal.pShellBrowser) {
                        g_portal.pShellBrowser->BrowseObject(pidl, SBSP_DEFBROWSER);
                        ILFree(pidl);
                    } else {
                        ShellExecuteW(nullptr, L"open", targetPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                    }
                    if (!g_unloading.load()) {
                        StartPortalFadeOut();
                    }
                }
            }
        }
        return 0;
    }

    case WM_CAPTURECHANGED: {
        if (g_portal.isDraggingScrollbar) {
            g_portal.isDraggingScrollbar = false;
            RequestPortalRepaint(hWnd);
        }
        return 0;
    }

    case WM_RBUTTONDOWN:
        return 0;

    case WM_RBUTTONUP: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int hitIdx = HitTestPortalItem(pt);
        if (hitIdx >= 0) {
            POINT ptScreen = pt;
            ClientToScreen(hWnd, &ptScreen);
            ShowNativeShellContextMenu(hWnd, hitIdx, ptScreen);
        }
        return 0;
    }

    case WM_MOUSELEAVE: {
        g_portal.mouseTrackingActive = false;
        if (g_portal.isDraggingOrMenuActive || g_portal.isDraggingScrollbar) return 0;
        bool needsRepaint = false;
        if (g_portal.hoveredGridIndex != -1) {
            g_portal.hoveredGridIndex = -1;
            needsRepaint = true;
        }
        if (g_portal.scrollbarEdgeHovered) {
            g_portal.scrollbarEdgeHovered = false;
            needsRepaint = true;
        }
        if (needsRepaint) {
            RequestPortalRepaint(hWnd);
        }
        if (!IsCursorInsideSafeZone()) {
            StartPortalFadeOut();
        }
        return 0;
    }

    case WM_TIMER: {
        if (wParam == TIMER_ID_FRAME_RENDER) {
            if (g_portal.renderDirty) {
                g_portal.renderDirty = false;
                RenderAndPresentPortal(hWnd);
            } else {
                KillTimer(hWnd, TIMER_ID_FRAME_RENDER);
                g_portal.frameTimerActive = false;
            }
            return 0;
        }
        if (wParam == TIMER_ID_SCROLL_IDLE) {
            KillTimer(hWnd, TIMER_ID_SCROLL_IDLE);
            g_portal.isScrolling = false;
            ScheduleVisibleIconsLoad(hWnd);
            return 0;
        }
        if (wParam == TIMER_ID_MOUSECHECK) {
            if (g_portal.isDraggingOrMenuActive) return 0;
            if (!IsCursorInsideSafeZone()) {
                StartPortalFadeOut();
            }
            return 0;
        }
        break;
    }

    case WM_DESTROY:
        g_vsyncAnimator.StopAnimation();
        KillTimer(hWnd, TIMER_ID_MOUSECHECK);
        KillTimer(hWnd, TIMER_ID_FRAME_RENDER);
        KillTimer(hWnd, TIMER_ID_SCROLL_IDLE);
        g_portal.frameTimerActive = false;
        return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void DismissDesktopListViewTooltip(HWND hListView) {
    if (!hListView || !IsWindow(hListView)) return;
    HWND hTip = reinterpret_cast<HWND>(SendMessageW(hListView, LVM_GETTOOLTIPS, 0, 0));
    if (hTip && IsWindow(hTip)) {
        SendMessageW(hTip, TTM_POP, 0, 0);
        if (IsWindowVisible(hTip)) {
            ShowWindow(hTip, SW_HIDE);
        }
    }
}

bool ShouldSuppressDesktopTooltip(HWND hListView, int explicitItemIndex = -1) {
    if (g_portal.animState != PortalAnimState::Hidden) {
        return true;
    }

    int targetIndex = explicitItemIndex;
    if (targetIndex < 0 && hListView && IsWindow(hListView)) {
        POINT pt = {};
        if (GetCursorPos(&pt)) {
            ScreenToClient(hListView, &pt);
            LVHITTESTINFO ht = {};
            ht.pt = pt;
            int hitIndex = static_cast<int>(SendMessageW(hListView, LVM_HITTEST, 0, reinterpret_cast<LPARAM>(&ht)));
            if (hitIndex >= 0 && (ht.flags & (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEM))) {
                targetIndex = hitIndex;
            }
        }
        if (targetIndex < 0) {
            int hotIndex = static_cast<int>(SendMessageW(hListView, LVM_GETHOTITEM, 0, 0));
            if (hotIndex >= 0) {
                targetIndex = hotIndex;
            }
        }
    }

    if (targetIndex < 0) return false;

    std::wstring folderPath;
    std::wstring folderTitle;
    return GetDesktopItemDirectoryPath(hListView, targetIndex, folderPath, folderTitle);
}

// ---------------------------------------------------------------------------
// Создание и запуск портала для папки на рабочем столе
// ---------------------------------------------------------------------------
void OpenPortalForDesktopFolder(HWND hListView, int itemIndex, const std::wstring& folderPath, const std::wstring& folderTitle) {
    if (!IsWindow(hListView) || g_unloading.load()) return;

    // Если этот же портал уже открыт или открывается — не перезапускаем
    if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd) &&
        g_portal.activeItemIndex == itemIndex &&
        g_portal.animState != PortalAnimState::Hidden &&
        g_portal.animState != PortalAnimState::Closing) {
        return;
    }

    DismissDesktopListViewTooltip(hListView);

    // Переиспользуем существующий скрытый HWND портала, если владелец (ListView) не изменился
    if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd) && g_portal.hDesktopListView == hListView) {
        HidePortalImmediately();
    } else {
        DestroyPortalImmediately();
    }

    // Вычисляем экранные координаты иконки папки на рабочем столе
    RECT rcItem = {};
    rcItem.left = LVIR_ICON;
    if (!SendMessageW(hListView, LVM_GETITEMRECT, itemIndex, reinterpret_cast<LPARAM>(&rcItem))) {
        return;
    }

    POINT ptTopLeft = { rcItem.left, rcItem.top };
    POINT ptBottomRight = { rcItem.right, rcItem.bottom };
    ClientToScreen(hListView, &ptTopLeft);
    ClientToScreen(hListView, &ptBottomRight);

    RECT iconScreenRect = { ptTopLeft.x, ptTopLeft.y, ptBottomRight.x, ptBottomRight.y };
    int iconCenterX = (iconScreenRect.left + iconScreenRect.right) / 2;
    int iconCenterY = (iconScreenRect.top + iconScreenRect.bottom) / 2;

    // Определяем целевой монитор и его реальный DPI через GetDpiForMonitor
    HMONITOR hMon = MonitorFromPoint({ iconCenterX, iconCenterY }, MONITOR_DEFAULTTONEAREST);
    const int dpi = GetMonitorDpi(hMon);

    // Стартовый кружок (внутри иконки папки) для морфинг-анимации с учетом DPI
    const int startCircleRadius = ScaleByDpi(18, dpi);
    RECT startCircleRect = {
        iconCenterX - startCircleRadius,
        iconCenterY - startCircleRadius,
        iconCenterX + startCircleRadius,
        iconCenterY + startCircleRadius
    };

    // Целевой прямоугольник справа от папки (с проверкой границ монитора и учетом масштаба DPI)
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(hMon, &mi);
    RECT workArea = mi.rcWork;

    int portalW = ScaleByDpi(std::clamp(g_settings.portalWidth, 240, 640), dpi);
    int portalH = ScaleByDpi(std::clamp(g_settings.portalHeight, 180, 560), dpi);
    int gapX = ScaleByDpi(12, dpi);
    int offsetY = ScaleByDpi(10, dpi);
    int edgeMargin = ScaleByDpi(8, dpi);

    int targetLeft = iconScreenRect.right + gapX;
    int targetTop  = iconScreenRect.top - offsetY;

    // Если справа от папки недостаточно места на экране — открываем слева от папки
    if (targetLeft + portalW > workArea.right - gapX) {
        targetLeft = iconScreenRect.left - portalW - gapX;
    }
    targetLeft = std::clamp(targetLeft, static_cast<int>(workArea.left + edgeMargin), static_cast<int>(workArea.right - portalW - edgeMargin));
    targetTop  = std::clamp(targetTop,  static_cast<int>(workArea.top + edgeMargin),  static_cast<int>(workArea.bottom - portalH - edgeMargin));

    RECT targetRect = { targetLeft, targetTop, targetLeft + portalW, targetTop + portalH };

    HWND hPortal = g_portal.hPortalWnd;
    bool newlyCreated = false;
    if (!hPortal || !IsWindow(hPortal)) {
        hPortal = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            PORTAL_CLASS_NAME,
            L"",
            WS_POPUP,
            startCircleRect.left,
            startCircleRect.top,
            startCircleRadius * 2,
            startCircleRadius * 2,
            hListView,
            nullptr,
            g_hInstance,
            nullptr);
        if (!hPortal || !IsWindow(hPortal)) {
            return;
        }
        newlyCreated = true;
        g_portal.dwmVisualsDirty = true;
    }

    g_portal.hPortalWnd = hPortal;
    g_portal.hDesktopListView = hListView;
    g_portal.activeItemIndex = itemIndex;
    g_portal.dpi = dpi;
    g_portal.folderPath = folderPath;
    g_portal.folderTitle = folderTitle;
    g_portal.iconScreenRect = iconScreenRect;
    g_portal.startCircleRect = startCircleRect;
    g_portal.targetPortalRect = targetRect;
    g_portal.currentRect = startCircleRect;
    g_portal.animState = PortalAnimState::Opening;
    g_portal.animProgress = 0.0f;
    g_portal.morphProgress = 0.0f;
    g_portal.hoveredGridIndex = -1;
    g_portal.pressedGridIndex = -1;
    g_portal.isDraggingOrMenuActive = false;
    g_portal.scrollbarEdgeHovered = false;
    g_portal.isDraggingScrollbar = false;
    g_portal.scrollbarTrackHeight = 0;
    g_portal.scrollbarThumbHeight = 0;
    g_portal.scrollOffsetY = 0;
    g_portal.mouseTrackingActive = false;

    g_portal.frameIntervalMs = GetPortalFrameIntervalMs();
    g_portal.renderDirty = false;
    g_portal.frameTimerActive = false;
    g_portal.isScrolling = false;
    ++g_portal.contentRevision;

    const uint64_t genId = g_portalGenerationId.fetch_add(1, std::memory_order_acq_rel) + 1;
    const int preloadIconSize = GetPortalIconSize(dpi);
    int cols = 1;
    int cellHeight = 1;
    int topMargin = 0;
    GetPortalLayoutMetrics(dpi, cols, cellHeight, topMargin);
    const int visibleRows = (portalH / cellHeight) + 1;
    const size_t firstScreenCount = static_cast<size_t>(cols * visibleRows);

    // Инициализируем легковесный экземпляр IShellBrowser
    g_portal.pShellBrowser = new LightweightPortalShellBrowser(hPortal);

    // Сначала проверяем LRU-кэш директорий (с валидацией через FindFirstChangeNotificationW)
    if (g_dirCache.TryGet(folderPath, preloadIconSize, g_portal.items, g_portal.totalItemCount)) {
        g_portal.isLoadingDirectory = false;
        ScheduleIconsLoadForRange(hPortal, genId, 0, firstScreenCount, preloadIconSize);
    } else {
        // Промах кэша: открываем портал мгновенно с плейсхолдерами и выносим чтение ФС и иконок в рабочий поток
        g_portal.isLoadingDirectory = true;
        g_portal.items.clear();
        g_portal.totalItemCount = 0;
        g_asyncWorker.EnqueueDirectoryLoad(genId, hPortal, folderPath, preloadIconSize, firstScreenCount);
    }

    if (newlyCreated || g_portal.dwmVisualsDirty) {
        ApplyPortalDwmVisuals(hPortal, false);
        g_portal.dwmVisualsDirty = false;
    } else if (g_portal.dwmBlurActive) {
        SetPortalBlurEnabled(hPortal, false);
    }

    // Запускаем отсчет времени анимации по QPC с VSync-синхронизацией через DwmFlush
    g_portal.animStartMs = GetQpcNowMs();
    g_portal.animProgress = 0.02f;
    UpdatePortalMorphGeometry(hPortal, g_portal.animProgress, true);
    ShowWindow(hPortal, SW_SHOWNOACTIVATE);

    g_vsyncAnimator.StartAnimation(hPortal, g_portal.frameIntervalMs);
    SetTimer(hPortal, TIMER_ID_MOUSECHECK, 50, nullptr);
}

// ---------------------------------------------------------------------------
// Subclass-процедура для SysListView32 рабочего стола
// (Сигнатура строго совместима с WindhawkUtils::WH_SUBCLASSPROC)
// ---------------------------------------------------------------------------
LRESULT CALLBACK DesktopListViewSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData)
{
    (void)dwRefData;

    if (g_unloading.load() && uMsg != WM_NCDESTROY) {
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    static LPARAM s_lastDesktopMousePos = -1;

    switch (uMsg) {
    case WM_MOUSEMOVE: {
        LRESULT lres = DefSubclassProc(hWnd, uMsg, wParam, lParam);

        if (!g_unloading.load() && IsWindow(hWnd)) {
            if (lParam != s_lastDesktopMousePos) {
                s_lastDesktopMousePos = lParam;
                TRACKMOUSEEVENT tme = {};
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_HOVER | TME_LEAVE;
                tme.hwndTrack = hWnd;
                tme.dwHoverTime = static_cast<DWORD>(std::clamp(g_settings.hoverDelayMs, 50, 2000));
                TrackMouseEvent(&tme);
            }

            if (g_portal.animState != PortalAnimState::Hidden &&
                g_portal.animState != PortalAnimState::Closing &&
                g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
                if (!IsCursorInsideSafeZone()) {
                    StartPortalFadeOut();
                }
            }
        }
        return lres;
    }

    case WM_NOTIFY: {
        if (lParam != 0) {
            auto* pNmhdr = reinterpret_cast<NMHDR*>(lParam);
            if (pNmhdr->code == TTN_GETDISPINFOW) {
                if (ShouldSuppressDesktopTooltip(hWnd)) {
                    auto* pDisp = reinterpret_cast<NMTTDISPINFOW*>(lParam);
                    pDisp->szText[0] = L'\0';
                    pDisp->lpszText = pDisp->szText;
                    pDisp->hinst = nullptr;
                    if (pNmhdr->hwndFrom && IsWindow(pNmhdr->hwndFrom)) {
                        PostMessageW(pNmhdr->hwndFrom, TTM_POP, 0, 0);
                    }
                    return 0;
                }
            } else if (pNmhdr->code == TTN_GETDISPINFOA) {
                if (ShouldSuppressDesktopTooltip(hWnd)) {
                    auto* pDisp = reinterpret_cast<NMTTDISPINFOA*>(lParam);
                    pDisp->szText[0] = '\0';
                    pDisp->lpszText = pDisp->szText;
                    pDisp->hinst = nullptr;
                    if (pNmhdr->hwndFrom && IsWindow(pNmhdr->hwndFrom)) {
                        PostMessageW(pNmhdr->hwndFrom, TTM_POP, 0, 0);
                    }
                    return 0;
                }
            } else if (pNmhdr->code == TTN_SHOW) {
                if (ShouldSuppressDesktopTooltip(hWnd)) {
                    if (pNmhdr->hwndFrom && IsWindow(pNmhdr->hwndFrom)) {
                        SetWindowPos(
                            pNmhdr->hwndFrom,
                            nullptr,
                            -32000,
                            -32000,
                            0,
                            0,
                            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                        PostMessageW(pNmhdr->hwndFrom, TTM_POP, 0, 0);
                    }
                    return TRUE;
                }
            }
        }
        break;
    }

#ifdef LVM_SETINFOTIP
    case LVM_SETINFOTIP: {
        int tipItem = -1;
        if (lParam != 0) {
            auto* pInfoTip = reinterpret_cast<PLVSETINFOTIP>(lParam);
            tipItem = pInfoTip->iItem;
        }
        if (ShouldSuppressDesktopTooltip(hWnd, tipItem)) {
            DismissDesktopListViewTooltip(hWnd);
            return FALSE;
        }
        break;
    }
#endif

    case WM_MOUSEHOVER: {
        s_lastDesktopMousePos = -1;
        LVHITTESTINFO ht = {};
        ht.pt.x = GET_X_LPARAM(lParam);
        ht.pt.y = GET_Y_LPARAM(lParam);
        int hitIndex = static_cast<int>(SendMessageW(hWnd, LVM_HITTEST, 0, reinterpret_cast<LPARAM>(&ht)));

        if (hitIndex >= 0 && (ht.flags & (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEM))) {
            // Если портал для этой же папки уже открыт — не выполняем повторных проверок пути
            if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd) &&
                g_portal.activeItemIndex == hitIndex &&
                g_portal.animState != PortalAnimState::Hidden &&
                g_portal.animState != PortalAnimState::Closing) {
                break;
            }
            std::wstring folderPath;
            std::wstring folderTitle;
            if (GetDesktopItemDirectoryPath(hWnd, hitIndex, folderPath, folderTitle)) {
                OpenPortalForDesktopFolder(hWnd, hitIndex, folderPath, folderTitle);
            }
        }
        break;
    }

    case WM_MOUSELEAVE: {
        s_lastDesktopMousePos = -1;
        if (g_portal.animState != PortalAnimState::Hidden &&
            g_portal.animState != PortalAnimState::Closing &&
            g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
            if (!IsCursorInsideSafeZone()) {
                StartPortalFadeOut();
            }
        }
        break;
    }

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        if (g_portal.animState != PortalAnimState::Hidden &&
            g_portal.animState != PortalAnimState::Closing &&
            g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
            StartPortalFadeOut();
        }
        break;
    }

    case WM_NCDESTROY: {
        if (g_portal.hDesktopListView == hWnd) {
            DestroyPortalImmediately();
            g_portal.hDesktopListView = nullptr;
        }
        {
            std::lock_guard<std::mutex> lock(g_subclassMutex);
            g_subclassedWindows.erase(hWnd);
        }
        // WindhawkUtils::SetWindowSubclassFromAnyThread сам снимает свой внутренний сабкласс на WM_NCDESTROY
        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Поиск и подключение к SysListView32 рабочего стола + хук CreateWindowExW
// ---------------------------------------------------------------------------
void TrySubclassDesktopListView(HWND hWnd) {
    if (g_unloading.load() || !IsTrueDesktopListView(hWnd)) return;

    {
        // Ошибка 2: Захватываем мьютекс исключительно для атомарной проверки и резервирования хэндла
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        if (!g_subclassedWindows.insert(hWnd).second) {
            return;
        }
    }

    // Вызываем SetWindowSubclassFromAnyThread (использующий синхронный SendMessage) строго вне мьютекса
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListViewSubclassProc, 0)) {
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        g_subclassedWindows.erase(hWnd);
    }
}

BOOL CALLBACK EnumDesktopWindowsProc(HWND hWnd, LPARAM) {
    HWND hShellDefView = FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
    if (hShellDefView) {
        HWND hListView = FindWindowExW(hShellDefView, nullptr, L"SysListView32", nullptr);
        if (hListView && IsWindow(hListView)) {
            TrySubclassDesktopListView(hListView);
        }
    }
    return TRUE;
}

void AttachToExistingDesktopListViews() {
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (hProgman) {
        EnumDesktopWindowsProc(hProgman, 0);
    }
    EnumWindows(EnumDesktopWindowsProc, 0);
}

HWND FindAnyDesktopWindow() {
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (hProgman && IsWindow(hProgman)) {
        return hProgman;
    }
    return nullptr;
}

void DetachAllSubclassedDesktopListViewsAndDestroyPortal() {
    std::vector<HWND> windowsToDetach;
    {
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        windowsToDetach.assign(g_subclassedWindows.begin(), g_subclassedWindows.end());
        g_subclassedWindows.clear();
    }

    bool portalCleanedOnThread = false;

    for (HWND hList : windowsToDetach) {
        if (hList && IsWindow(hList)) {
            if (!portalCleanedOnThread) {
                RunFromWindowThread(hList, [&portalCleanedOnThread]() {
                    DestroyPortalImmediately();
                    portalCleanedOnThread = true;
                });
            }
            // В WindhawkUtils::RemoveWindowSubclassFromAnyThread ровно 2 аргумента: (hWnd, pfnSubclass)
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hList, DesktopListViewSubclassProc);
        }
    }

    if (!portalCleanedOnThread) {
        HWND hTarget = g_portal.hPortalWnd;
        if (!hTarget || !IsWindow(hTarget)) {
            hTarget = g_portal.hDesktopListView;
        }
        if (!hTarget || !IsWindow(hTarget)) {
            hTarget = FindAnyDesktopWindow();
        }

        if (hTarget && IsWindow(hTarget)) {
            RunFromWindowThread(hTarget, []() {
                DestroyPortalImmediately();
            });
        } else {
            DestroyPortalImmediately();
        }
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t Orig_CreateWindowExW = nullptr;

HWND WINAPI Hook_CreateWindowExW(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
    HWND hWnd = Orig_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && !g_unloading.load() && reinterpret_cast<ULONG_PTR>(lpClassName) > 0xFFFF) {
        if (wcscmp(lpClassName, L"SysListView32") == 0) {
            TrySubclassDesktopListView(hWnd);
        }
    }
    return hWnd;
}

// ---------------------------------------------------------------------------
// Загрузка и обновление настроек Windhawk
// ---------------------------------------------------------------------------
void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_stateMutex);

    g_settings.hoverDelayMs     = std::clamp(Wh_GetIntSetting(L"hoverDelayMs"), 50, 2000);
    g_settings.animDurationMs   = std::clamp(Wh_GetIntSetting(L"animDurationMs"), 60, 1000);
    g_settings.portalWidth      = std::clamp(Wh_GetIntSetting(L"portalWidth"), 240, 640);
    g_settings.portalHeight     = std::clamp(Wh_GetIntSetting(L"portalHeight"), 180, 560);
    g_settings.bgOpacity        = static_cast<BYTE>(std::clamp(Wh_GetIntSetting(L"bgOpacity"), 10, 255));
    g_settings.enableBlur       = Wh_GetIntSetting(L"enableBlur") != 0;
    g_settings.borderBrightness = std::clamp(Wh_GetIntSetting(L"borderBrightness"), 0, 255);
    g_settings.gridColumns      = std::clamp(Wh_GetIntSetting(L"gridColumns"), 2, 5);
    g_settings.showFileMetadata = Wh_GetIntSetting(L"showFileMetadata") != 0;

    PCWSTR blurStrength = Wh_GetStringSetting(L"blurStrength");
    if (blurStrength) {
        if (wcscmp(blurStrength, L"level1") == 0) {
            g_settings.blurStrength = 1;
        } else if (wcscmp(blurStrength, L"level3") == 0) {
            g_settings.blurStrength = 3;
        } else {
            g_settings.blurStrength = 2;
        }
        Wh_FreeStringSetting(blurStrength);
    }

    PCWSTR viewMode = Wh_GetStringSetting(L"viewMode");
    if (viewMode) {
        g_settings.listViewMode = (wcscmp(viewMode, L"list") == 0);
        Wh_FreeStringSetting(viewMode);
    } else {
        g_settings.listViewMode = false;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Desktop Folder Portals mod...");
    g_unloading.store(false);

    // Получаем HINSTANCE текущего модуля мода (а не explorer.exe) для чистой регистрации класса
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&Wh_ModInit),
            &g_hInstance) || !g_hInstance) {
        g_hInstance = GetModuleHandleW(nullptr);
    }

    LoadSettings();

    // Инициализируем GDI+ для высококачественной векторной отрисовки портала
    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) != Ok) {
        Wh_Log(L"Failed to initialize GDI+");
        return FALSE;
    }

    // Регистрируем класс окна портала в модуле самого мода с обязательной проверкой результата
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = PortalWndProc;
    wc.hInstance = g_hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = PORTAL_CLASS_NAME;

    ATOM classAtom = RegisterClassExW(&wc);
    if (!classAtom) {
        Wh_Log(L"Failed to register portal window class (error: %u)", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return FALSE;
    }
    g_portalClassRegistered = true;

    g_asyncWorker.Start();
    g_vsyncAnimator.StartThread();

    // Устанавливаем хук на CreateWindowExW для перехвата пересоздания рабочего стола
    Wh_SetFunctionHook(
        reinterpret_cast<void*>(CreateWindowExW),
        reinterpret_cast<void*>(Hook_CreateWindowExW),
        reinterpret_cast<void**>(&Orig_CreateWindowExW));

    return TRUE;
}

void Wh_ModAfterInit() {
    AttachToExistingDesktopListViews();
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    HWND hTarget = g_portal.hPortalWnd;
    if (!hTarget || !IsWindow(hTarget)) {
        hTarget = g_portal.hDesktopListView;
    }
    if (!hTarget || !IsWindow(hTarget)) {
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        if (!g_subclassedWindows.empty()) {
            hTarget = *g_subclassedWindows.begin();
        }
    }

    if (hTarget && IsWindow(hTarget)) {
        RunFromWindowThread(hTarget, []() {
            g_portal.dwmVisualsDirty = true;
            ++g_portal.contentRevision;
            if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd) &&
                g_portal.animState != PortalAnimState::Hidden) {
                ApplyPortalDwmVisuals(g_portal.hPortalWnd, g_portal.animState == PortalAnimState::Visible);
                g_portal.dwmVisualsDirty = false;
                RenderAndPresentPortal(g_portal.hPortalWnd);
                ScheduleVisibleIconsLoad(g_portal.hPortalWnd);
            }
        });
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Desktop Folder Portals mod...");
    g_unloading.store(true);
    g_vsyncAnimator.StopAnimation();
    g_portalGenerationId.fetch_add(1, std::memory_order_acq_rel);

    // Если открыто контекстное меню портала, аккуратно закрываем его перед ожиданием завершения модальных вызовов
    HWND hPortalSnapshot = g_portal.hPortalWnd;
    if (hPortalSnapshot && IsWindow(hPortalSnapshot)) {
        PostMessageW(hPortalSnapshot, WM_CANCELMODE, 0, 0);
    }

    // Ожидаем полного выхода потока рабочего стола из всех модальных циклов (TrackPopupMenuEx, InvokeCommand, SHDoDragDrop)
    while (g_activeModalCallCount.load(std::memory_order_acquire) > 0) {
        Sleep(15);
    }

    // Останавливаем фоновые потоки до уничтожения окна и освобождения кэша
    g_vsyncAnimator.StopThread();
    g_asyncWorker.Stop();

    // Снимаем сабклассы и уничтожаем окно портала строго в потоке окна рабочего стола
    DetachAllSubclassedDesktopListViewsAndDestroyPortal();

    // Повторная проверка на случай, если модальный вызов завершался в момент переключения контекста
    while (g_activeModalCallCount.load(std::memory_order_acquire) > 0) {
        Sleep(15);
    }

    // Освобождаем кэш директорий (FindCloseChangeNotification + DestroyIcon) и графические ресурсы
    g_dirCache.Clear();
    g_renderCache.ReleaseAll();

    if (g_portalClassRegistered) {
        UnregisterClassW(PORTAL_CLASS_NAME, g_hInstance);
        g_portalClassRegistered = false;
    }

    if (g_gdiplusToken != 0) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
