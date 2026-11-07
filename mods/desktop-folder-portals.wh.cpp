// ==WindhawkMod==
// @id              desktop-folder-portals
// @name            Desktop Folder Portals (Windows 11)
// @description     Interactive desktop folder portals with smooth morphing animations, glass blur, and file grids.
// @description:ru-RU Интерактивные папки-порталы на рабочем столе с плавной морфинг-анимацией, эффектом стекла и сеткой файлов.
// @version         1.2.0
// @author          Eswerd2324
// @github          https://github.com/Eswerd2324
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luxtheme -ldwmapi -lshlwapi -lgdiplus -lole32 -lshell32 -luuid -lcomctl32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Folder Portals (Windows 11)

Desktop Folder Portals is an elegant Windows 11 modification that brings interactive, on-hover folder content previews directly to your desktop without opening separate File Explorer windows.

### Key Features

* **Seamless Hover Previews:** Hovering over any desktop folder smoothly expands an interactive mini-window displaying its contents, elegantly collapsing once the cursor moves away.
* **Native Windows 11 Context Menu:** Full integration with the authentic system right-click menu, enabling quick renaming, property inspection, and standard file operations.
* **Effortless Drag-and-Drop:** Clean, artifact-free file transfers straight from the preview portal to the desktop, active folders, or third-party applications.
* **Modern Fluent Design:** Styled with native Mica and Acrylic translucency, rounded corners, soft ambient drop shadows, and fluid transformation animations.
* **Granular Customization:** Configure hover response delays, visual styling, and behavioral triggers effortlessly via the Windhawk settings interface.
* **Optimized & Safe Engine:** Written in clean, modern C++ with strict GDI/USER resource management and full Per-Monitor DPI scaling to prevent memory leaks and maintain complete desktop UI responsiveness.

---

# Desktop Folder Portals для Windows 11 (RU)

Desktop Folder Portals — элегантная модификация для Windows 11, внедряющая интерактивный предпросмотр содержимого папок прямо на рабочем столе без необходимости их открытия в Проводнике.

### Ключевые возможности

* **Интерактивные порталы:** Плавное появление компактного всплывающего окна с файлами при наведении курсора на директорию и его аккуратное исчезновение при отведении мыши.
* **Нативная интеграция с Windows 11:** Полный доступ к оригинальному системному контекстному меню по правому клику мыши для переименования, открытия свойств и стандартных файловых операций.
* **Полноценный Drag-and-Drop:** Удобный перенос файлов из окна предпросмотра на рабочий стол или в любые другие открытые окна без графических артефактов.
* **Современная эстетика Fluent Design:** Скругленные углы, эффект полупрозрачности Acrylic/Mica, мягкие тени и плавная анимация развертывания окна.
* **Гибкая кастомизация:** Быстрая настройка задержки срабатывания, визуальных эффектов и параметров отображения в панели Windhawk.
* **Надежность и производительность:** Оптимизированная реализация на C++ с поддержкой масштабирования DPI, контролем выделения системных ресурсов GDI/USER, отсутствием утечек памяти и плавным откликом без задержек `explorer.exe`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable Folder Portals
  $name:ru-RU: Включить папки-порталы
  $description: Activates or disables popup folder portals on the desktop.
  $description:ru-RU: Активирует или отключает всплывающие порталы на рабочем столе.
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

constexpr UINT_PTR TIMER_ID_ANIMATION  = 1001;
constexpr UINT_PTR TIMER_ID_MOUSECHECK = 1002;
constexpr wchar_t  PORTAL_CLASS_NAME[] = L"WindhawkDesktopFolderPortalWnd";

struct ModSettings {
    bool enabled = true;
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
} g_settings;

std::mutex g_stateMutex;
std::mutex g_subclassMutex;
std::unordered_set<HWND> g_subclassedWindows;
std::atomic<bool> g_unloading{false};
ULONG_PTR g_gdiplusToken = 0;
HINSTANCE g_hInstance = nullptr;
bool g_portalClassRegistered = false;
pfnSetWindowCompositionAttribute g_pSetWCA = nullptr;

// Определение языка интерфейса пользователя для локализации строк внутри портала
bool IsRussianUILanguage() {
    LANGID langId = GetUserDefaultUILanguage();
    return PRIMARYLANGID(langId) == LANG_RUSSIAN;
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

    FontFamily* pFamily = nullptr;
    Gdiplus::Font* titleFont = nullptr;
    Gdiplus::Font* countFont = nullptr;
    Gdiplus::Font* itemFont = nullptr;
    Gdiplus::Font* metaFont = nullptr;

    void ReleaseFonts() {
        delete titleFont; titleFont = nullptr;
        delete countFont; countFont = nullptr;
        delete itemFont;  itemFont = nullptr;
        delete metaFont;  metaFont = nullptr;
        delete pFamily;   pFamily = nullptr;
        fontDpi = 0;
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

    void ReleaseAll() {
        ReleaseBitmap();
        ReleaseFonts();
    }
} g_renderCache;

// ---------------------------------------------------------------------------
// Минималистичная реализация IDropSource для системного Drag-and-Drop
// ---------------------------------------------------------------------------
class PortalDropSource : public IDropSource {
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
    HICON hIcon = nullptr;
    bool isDirectory = false;
    bool iconLoaded = false;
    RECT boundsRect = {};

    ~PortalFileItem() {
        if (hIcon) {
            DestroyIcon(hIcon);
            hIcon = nullptr;
        }
    }
    PortalFileItem() = default;
    PortalFileItem(const PortalFileItem&) = delete;
    PortalFileItem& operator=(const PortalFileItem&) = delete;
    PortalFileItem(PortalFileItem&& other) noexcept {
        *this = std::move(other);
    }
    PortalFileItem& operator=(PortalFileItem&& other) noexcept {
        if (this != &other) {
            if (hIcon) DestroyIcon(hIcon);
            name = std::move(other.name);
            fullPath = std::move(other.fullPath);
            sizeOrTypeText = std::move(other.sizeOrTypeText);
            hIcon = other.hIcon;
            isDirectory = other.isDirectory;
            iconLoaded = other.iconLoaded;
            boundsRect = other.boundsRect;
            other.hIcon = nullptr;
        }
        return *this;
    }
};

// Ленивая подгрузка системной иконки с учетом масштаба DPI через SHGetImageList
void EnsureItemIconLoaded(PortalFileItem& item, int targetIconSize) {
    if (item.iconLoaded) return;
    item.iconLoaded = true;
    if (item.fullPath.empty()) return;

    DWORD attrs = item.isDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
    SHFILEINFOW sfi = {};
    if (!SHGetFileInfoW(item.fullPath.c_str(), attrs, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX)) {
        SHGetFileInfoW(item.fullPath.c_str(), attrs, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
    }

    // Используем встроенный в компилятор __uuidof(IImageList), чтобы не зависеть от символа IID_IImageList в libuuid.a
    int iilSize = (targetIconSize > 48) ? SHIL_JUMBO : ((targetIconSize > 32) ? SHIL_EXTRALARGE : SHIL_LARGE);
    IImageList* pImgList = nullptr;
    if (SUCCEEDED(SHGetImageList(iilSize, __uuidof(IImageList), reinterpret_cast<void**>(&pImgList))) && pImgList) {
        HICON hExtracted = nullptr;
        if (SUCCEEDED(pImgList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hExtracted)) && hExtracted) {
            item.hIcon = hExtracted;
        }
        pImgList->Release();
    }

    if (!item.hIcon) {
        if (SHGetFileInfoW(item.fullPath.c_str(), attrs, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
            item.hIcon = sfi.hIcon;
        } else if (SHGetFileInfoW(item.fullPath.c_str(), attrs, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES)) {
            item.hIcon = sfi.hIcon;
        }
    }
}

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

    // Быстрое чтение папки через FindFirstFileExW без блокирующих вызовов на старте
    bool PopulateItemsFromDirectory(const std::wstring& folderPath, std::vector<PortalFileItem>& outItems, size_t maxItems = 45) {
        outItems.clear();
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

        outItems.reserve(maxItems);
        const bool isRu = IsRussianUILanguage();
        do {
            if (fd.cFileName[0] == L'.' &&
                (fd.cFileName[1] == L'\0' || (fd.cFileName[1] == L'.' && fd.cFileName[2] == L'\0'))) {
                continue;
            }
            if (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
                continue;
            }

            PortalFileItem item;
            item.name = fd.cFileName;
            item.fullPath = baseDir + fd.cFileName;
            item.isDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            if (item.isDirectory) {
                item.sizeOrTypeText = isRu ? L"Папка" : L"Folder";
            } else {
                ULONGLONG fileSize = (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;
                wchar_t szBuf[64] = {};
                StrFormatByteSizeW(static_cast<LONGLONG>(fileSize), szBuf, ARRAYSIZE(szBuf));
                item.sizeOrTypeText = szBuf;
            }

            outItems.push_back(std::move(item));
        } while (outItems.size() < maxItems && FindNextFileW(hFind, &fd));

        FindClose(hFind);

        // Сортируем: сначала папки, затем файлы по алфавиту
        std::sort(outItems.begin(), outItems.end(), [](const PortalFileItem& a, const PortalFileItem& b) {
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory > b.isDirectory;
            }
            return CompareStringOrdinal(a.name.c_str(), -1, b.name.c_str(), -1, TRUE) == CSTR_LESS_THAN;
        });

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

    RECT iconScreenRect = {};
    RECT startCircleRect = {};
    RECT targetPortalRect = {};
    RECT currentRect = {};

    PortalAnimState animState = PortalAnimState::Hidden;
    ULONGLONG animStartTick = 0;
    float animProgress = 0.0f; // 0.0f (кружок в папке) -> 1.0f (полный прямоугольник)

    int hoveredGridIndex = -1;
    int scrollOffsetY = 0;
    int maxScrollY = 0;

    int pressedGridIndex = -1;
    POINT pressStartPt = {};
    bool isDraggingOrMenuActive = false;

    IContextMenu2* pActiveCtxMenu2 = nullptr;
    IContextMenu3* pActiveCtxMenu3 = nullptr;

    LightweightPortalShellBrowser* pShellBrowser = nullptr;
    std::vector<PortalFileItem> items;
} g_portal;

void RenderAndPresentPortal(HWND hWnd);
void StartPortalFadeOut();

// Вызов оригинального контекстного меню Windows 11 для элемента внутри портала
void ShowNativeShellContextMenu(HWND hPortal, int itemIndex, POINT ptScreen) {
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

                    if (cmd >= 1 && cmd <= 0x7FFF) {
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
                        if (g_portal.pShellBrowser && IsWindow(hPortal)) {
                            g_portal.pShellBrowser->PopulateItemsFromDirectory(g_portal.folderPath, g_portal.items, 45);
                            RenderAndPresentPortal(hPortal);
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
            if (SUCCEEDED(hrDrag) && IsWindow(hPortal) && g_portal.pShellBrowser) {
                g_portal.pShellBrowser->PopulateItemsFromDirectory(g_portal.folderPath, g_portal.items, 45);
                RenderAndPresentPortal(hPortal);
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

    // 1. Проверяем на Пользовательском и Общем рабочем столе
    const int csidlList[] = { CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY };
    for (int csidl : csidlList) {
        wchar_t desktopBase[MAX_PATH] = {};
        if (SUCCEEDED(SHGetFolderPathW(nullptr, csidl, nullptr, SHGFP_TYPE_CURRENT, desktopBase))) {
            wchar_t candidatePath[MAX_PATH] = {};
            if (PathCombineW(candidatePath, desktopBase, itemText)) {
                DWORD attrs = GetFileAttributesW(candidatePath);
                if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                    outFullPath = candidatePath;
                    return true;
                }
            }
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

    return !outFullPath.empty();
}

// Настройка DWM: скругление, тень и гладкое размытие без зернистости во всю площадь окна
void ApplyPortalDwmVisuals(HWND hPortal) {
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

    if (!g_pSetWCA) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            g_pSetWCA = reinterpret_cast<pfnSetWindowCompositionAttribute>(
                GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
        }
    }

    if (g_settings.enableBlur) {
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
    }
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

// Иконки GDI (DrawIconEx) могут сбрасывать альфа-канал в 0 для RGB-пикселей, восстанавливаем непрозрачность
void EnsureOpaqueRectInDib(BYTE* bits, int stride, int bufW, int bufH, const RECT& rc, BYTE targetAlpha) {
    int x0 = std::clamp<int>(rc.left, 0, bufW);
    int y0 = std::clamp<int>(rc.top, 0, bufH);
    int x1 = std::clamp<int>(rc.right, 0, bufW);
    int y1 = std::clamp<int>(rc.bottom, 0, bufH);

    for (int y = y0; y < y1; ++y) {
        BYTE* row = bits + y * stride;
        for (int x = x0; x < x1; ++x) {
            BYTE* px = row + x * 4;
            if (px[3] == 0 && (px[0] != 0 || px[1] != 0 || px[2] != 0)) {
                px[3] = targetAlpha;
            }
        }
    }
}

// Отрисовка окна через UpdateLayeredWindow: прозрачность фона не влияет на яркость иконок и текста
void RenderAndPresentPortal(HWND hWnd) {
    if (!IsWindow(hWnd)) return;

    const int dpi = g_portal.dpi > 0 ? g_portal.dpi : 96;
    int width  = std::max<int>(ScaleByDpi(16, dpi), g_portal.currentRect.right - g_portal.currentRect.left);
    int height = std::max<int>(ScaleByDpi(16, dpi), g_portal.currentRect.bottom - g_portal.currentRect.top);

    g_renderCache.EnsureFonts(dpi);
    if (!g_renderCache.EnsureBitmap(width, height)) return;

    HDC hdcMem = g_renderCache.hdcMem;
    BYTE* pvBits = static_cast<BYTE*>(g_renderCache.pvBits);
    int stride = g_renderCache.bufW * 4;

    // Очищаем рабочую область переиспользуемого буфера
    for (int y = 0; y < height; ++y) {
        memset(pvBits + y * stride, 0, width * 4);
    }

    float animAlphaFactor = std::clamp(g_portal.animProgress, 0.0f, 1.0f);
    // Непрозрачность фона зависит только от bgOpacity (и уровня размытия), а контент всегда 255
    int baseBgAlpha = std::clamp<int>(g_settings.bgOpacity, 10, 255);
    if (g_settings.enableBlur) {
        if (g_settings.blurStrength == 2) baseBgAlpha = std::min<int>(255, baseBgAlpha + 12);
        else if (g_settings.blurStrength == 3) baseBgAlpha = std::min<int>(255, baseBgAlpha + 26);
    }
    BYTE bgAlpha = static_cast<BYTE>(std::round(static_cast<float>(baseBgAlpha) * animAlphaFactor));

    {
        Graphics g(hdcMem);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

        // Полная равномерная заливка всей прямоугольной поверхности окна
        // (скругление углов делает сам DWM через DWMWCP_ROUND без светлых угловых артефактов)
        SolidBrush bgBrush(Color(bgAlpha, 26, 27, 32));
        g.FillRectangle(&bgBrush, 0, 0, width, height);

        float contentPhase = std::clamp((g_portal.animProgress - 0.2f) / 0.8f, 0.0f, 1.0f);
        BYTE contentAlpha = static_cast<BYTE>(std::round(255.0f * contentPhase));
        BYTE metaAlpha    = static_cast<BYTE>(std::round(195.0f * contentPhase));

        const int headerHeight = ScaleByDpi(42, dpi);
        const bool isRu = IsRussianUILanguage();

        if (contentPhase > 0.05f && width > ScaleByDpi(110, dpi) && height > ScaleByDpi(80, dpi)) {
            SolidBrush titleBrush(Color(contentAlpha, 248, 250, 255));
            SolidBrush subBrush(Color(metaAlpha, 182, 190, 206));

            // 1. Отрисовка сетки файлов с жестким отсечением (файлы гарантированно скрываются за шапкой)
            int clipTop = headerHeight + std::max<int>(1, ScaleByDpi(1, dpi));
            int clipBottom = height - ScaleByDpi(4, dpi);
            int sideClipMargin = ScaleByDpi(6, dpi);

            if (g_portal.items.empty()) {
                StringFormat centerFormat;
                centerFormat.SetAlignment(StringAlignmentCenter);
                centerFormat.SetLineAlignment(StringAlignmentCenter);
                RectF emptyRect(0.0f, static_cast<REAL>(clipTop), static_cast<REAL>(width), static_cast<REAL>(clipBottom - clipTop));
                g.DrawString(isRu ? L"Папка пуста" : L"Folder is empty", -1, g_renderCache.itemFont, emptyRect, &centerFormat, &subBrush);
            } else {
                int cols = std::clamp(g_settings.gridColumns, 2, 5);
                int gridPaddingX = ScaleByDpi(12, dpi);
                int gridTopY = headerHeight + ScaleByDpi(8, dpi) - g_portal.scrollOffsetY;
                int availWidth = width - (gridPaddingX * 2);
                int cellWidth = availWidth / cols;
                int cellHeight = ScaleByDpi(g_settings.showFileMetadata ? 88 : 72, dpi);

                StringFormat itemFormat;
                itemFormat.SetAlignment(StringAlignmentCenter);
                itemFormat.SetLineAlignment(StringAlignmentNear);
                itemFormat.SetTrimming(StringTrimmingEllipsisCharacter);
                itemFormat.SetFormatFlags(StringFormatFlagsNoWrap);

                // Ограничиваем область рисования как для GDI+, так и для GDI (DrawIconEx)
                Region gridClip(Rect(sideClipMargin, clipTop, width - sideClipMargin * 2, clipBottom - clipTop));
                g.SetClip(&gridClip, CombineModeReplace);

                int padX = ScaleByDpi(3, dpi);
                int padY = ScaleByDpi(2, dpi);
                int iconSize = ScaleByDpi(32, dpi);
                int iconTopOffset = ScaleByDpi(8, dpi);
                int textPadX = ScaleByDpi(4, dpi);
                int nameOffsetY = ScaleByDpi(4, dpi);
                int nameHeight = ScaleByDpi(18, dpi);
                int metaOffsetY = ScaleByDpi(21, dpi);
                int metaHeight = ScaleByDpi(17, dpi);
                int hoverRad = ScaleByDpi(10, dpi);

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

                    if (static_cast<int>(i) == g_portal.hoveredGridIndex) {
                        GraphicsPath hoverPath;
                        Rect cardRect(itemRc.left, itemRc.top, itemRc.right - itemRc.left, itemRc.bottom - itemRc.top);
                        hoverPath.AddArc(cardRect.X, cardRect.Y, hoverRad, hoverRad, 180.0f, 90.0f);
                        hoverPath.AddArc(cardRect.X + cardRect.Width - hoverRad, cardRect.Y, hoverRad, hoverRad, 270.0f, 90.0f);
                        hoverPath.AddArc(cardRect.X + cardRect.Width - hoverRad, cardRect.Y + cardRect.Height - hoverRad, hoverRad, hoverRad, 0.0f, 90.0f);
                        hoverPath.AddArc(cardRect.X, cardRect.Y + cardRect.Height - hoverRad, hoverRad, hoverRad, 90.0f, 90.0f);
                        hoverPath.CloseFigure();

                        SolidBrush hoverFill(Color(static_cast<BYTE>(50.0f * contentPhase), 255, 255, 255));
                        Pen hoverBorder(Color(static_cast<BYTE>(95.0f * contentPhase), 255, 255, 255), static_cast<REAL>(ScaleByDpi(1, dpi)));
                        g.FillPath(&hoverFill, &hoverPath);
                        g.DrawPath(&hoverBorder, &hoverPath);
                    }

                    // Подгружаем иконку только тогда, когда карточка видна и окно уже раскрывается
                    int iconX = cellX + (cellWidth - iconSize) / 2;
                    int iconY = cellY + iconTopOffset;
                    if (contentPhase > 0.15f) {
                        EnsureItemIconLoaded(g_portal.items[i], iconSize);
                    }
                    if (g_portal.items[i].hIcon) {
                        int savedDC = SaveDC(hdcMem);
                        IntersectClipRect(hdcMem, sideClipMargin, clipTop, width - sideClipMargin, clipBottom);
                        DrawIconEx(hdcMem, iconX, iconY, g_portal.items[i].hIcon,
                                   iconSize, iconSize, 0, nullptr, DI_NORMAL);
                        RestoreDC(hdcMem, savedDC);

                        RECT rcIconClip = {
                            iconX,
                            std::max<int>(iconY, clipTop),
                            iconX + iconSize,
                            std::min<int>(iconY + iconSize, clipBottom)
                        };
                        if (rcIconClip.bottom > rcIconClip.top) {
                            EnsureOpaqueRectInDib(pvBits, stride, width, height, rcIconClip, contentAlpha);
                        }
                    }

                    RectF nameRect(static_cast<REAL>(cellX + textPadX),
                                   static_cast<REAL>(iconY + iconSize + nameOffsetY),
                                   static_cast<REAL>(cellWidth - textPadX * 2),
                                   static_cast<REAL>(nameHeight));
                    g.DrawString(g_portal.items[i].name.c_str(), -1, g_renderCache.itemFont, nameRect, &itemFormat, &titleBrush);

                    if (g_settings.showFileMetadata && !g_portal.items[i].sizeOrTypeText.empty()) {
                        RectF metaRect(static_cast<REAL>(cellX + textPadX),
                                       static_cast<REAL>(iconY + iconSize + metaOffsetY),
                                       static_cast<REAL>(cellWidth - textPadX * 2),
                                       static_cast<REAL>(metaHeight));
                        g.DrawString(g_portal.items[i].sizeOrTypeText.c_str(), -1, g_renderCache.metaFont, metaRect, &itemFormat, &subBrush);
                    }
                }

                int totalRows = (static_cast<int>(g_portal.items.size()) + cols - 1) / cols;
                int totalContentHeight = totalRows * cellHeight + ScaleByDpi(12, dpi);
                int visibleGridHeight = clipBottom - clipTop;
                g_portal.maxScrollY = std::max<int>(0, totalContentHeight - visibleGridHeight);

                g.ResetClip();
            }

            // 2. Шапка портала поверх всего (одна тонкая линия на всю ширину окна, без короткой линии)
            StringFormat leftFormat;
            leftFormat.SetAlignment(StringAlignmentNear);
            leftFormat.SetLineAlignment(StringAlignmentCenter);
            leftFormat.SetTrimming(StringTrimmingEllipsisCharacter);
            leftFormat.SetFormatFlags(StringFormatFlagsNoWrap);

            int titleLeft = ScaleByDpi(16, dpi);
            int titleTop = ScaleByDpi(6, dpi);
            int titleRightMargin = ScaleByDpi(105, dpi);
            int titleHeight = ScaleByDpi(30, dpi);

            RectF titleRect(static_cast<REAL>(titleLeft),
                            static_cast<REAL>(titleTop),
                            static_cast<REAL>(width - titleRightMargin),
                            static_cast<REAL>(titleHeight));
            g.DrawString(g_portal.folderTitle.c_str(), -1, g_renderCache.titleFont, titleRect, &leftFormat, &titleBrush);

            wchar_t countText[64] = {};
            UINT itemCount = static_cast<UINT>(g_portal.items.size());
            if (isRu) {
                wsprintfW(countText, L"%u объект.", itemCount);
            } else {
                wsprintfW(countText, itemCount == 1 ? L"%u item" : L"%u items", itemCount);
            }
            StringFormat rightFormat;
            rightFormat.SetAlignment(StringAlignmentFar);
            rightFormat.SetLineAlignment(StringAlignmentCenter);
            RectF countRect(static_cast<REAL>(width - ScaleByDpi(95, dpi)),
                            static_cast<REAL>(titleTop),
                            static_cast<REAL>(ScaleByDpi(79, dpi)),
                            static_cast<REAL>(titleHeight));
            g.DrawString(countText, -1, g_renderCache.countFont, countRect, &rightFormat, &subBrush);

            // Единственная тонкая линия во всю ширину, отделяющая шапку
            Pen headerFullLinePen(Color(static_cast<BYTE>(38.0f * contentPhase), 255, 255, 255), static_cast<REAL>(ScaleByDpi(1, dpi)));
            g.DrawLine(&headerFullLinePen, 1, headerHeight, width - 2, headerHeight);
        }

        // 3. Тонкая светлая рамка точно по контуру скругления DWM (8px радиус = 16px диаметр при 96 DPI)
        int brAlpha = static_cast<int>(std::round(static_cast<float>(std::clamp(g_settings.borderBrightness, 0, 255)) * animAlphaFactor));
        if (brAlpha > 0) {
            GraphicsPath borderPath;
            int cornerD = ScaleByDpi(16, dpi);
            Rect bRect(0, 0, width - 1, height - 1);
            borderPath.AddArc(bRect.X, bRect.Y, cornerD, cornerD, 180.0f, 90.0f);
            borderPath.AddArc(bRect.X + bRect.Width - cornerD, bRect.Y, cornerD, cornerD, 270.0f, 90.0f);
            borderPath.AddArc(bRect.X + bRect.Width - cornerD, bRect.Y + bRect.Height - cornerD, cornerD, cornerD, 0.0f, 90.0f);
            borderPath.AddArc(bRect.X, bRect.Y + bRect.Height - cornerD, cornerD, cornerD, 90.0f, 90.0f);
            borderPath.CloseFigure();

            Pen lightBorderPen(Color(static_cast<BYTE>(brAlpha), 240, 246, 255), 1.2f * (static_cast<float>(dpi) / 96.0f));
            g.DrawPath(&lightBorderPen, &borderPath);
        }
    }

    HDC hdcScreen = GetDC(nullptr);
    if (hdcScreen) {
        POINT ptWinPos = { g_portal.currentRect.left, g_portal.currentRect.top };
        SIZE szWin = { width, height };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        UpdateLayeredWindow(hWnd, hdcScreen, &ptWinPos, &szWin, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
        ReleaseDC(nullptr, hdcScreen);
    }
}

void UpdatePortalMorphGeometry(HWND hPortal, float rawProgress, bool isOpening) {
    if (!IsWindow(hPortal)) return;

    float eased = isOpening ? EaseOutCubic(rawProgress) : (1.0f - EaseInCubic(1.0f - rawProgress));
    eased = std::clamp(eased, 0.0f, 1.0f);

    RECT r = {};
    r.left   = LerpInt(g_portal.startCircleRect.left,   g_portal.targetPortalRect.left,   eased);
    r.top    = LerpInt(g_portal.startCircleRect.top,    g_portal.targetPortalRect.top,    eased);
    r.right  = LerpInt(g_portal.startCircleRect.right,  g_portal.targetPortalRect.right,  eased);
    r.bottom = LerpInt(g_portal.startCircleRect.bottom, g_portal.targetPortalRect.bottom, eased);
    g_portal.currentRect = r;

    // Не используем SetWindowRgn во время анимации: именно GDI-регион обрезал темный слой,
    // оставляя прямоугольный DWM-backdrop со светлыми углами. Теперь весь прямоугольник
    // равномерно закрашивается и плавно скругляется самим DWM (DWMWCP_ROUND) с первого до последнего кадра!
    RenderAndPresentPortal(hPortal);
}

void DestroyPortalImmediately() {
    if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
        KillTimer(g_portal.hPortalWnd, TIMER_ID_ANIMATION);
        KillTimer(g_portal.hPortalWnd, TIMER_ID_MOUSECHECK);
        DestroyWindow(g_portal.hPortalWnd);
    }
    g_portal.hPortalWnd = nullptr;
    g_portal.animState = PortalAnimState::Hidden;
    g_portal.activeItemIndex = -1;
    g_portal.hoveredGridIndex = -1;
    g_portal.pressedGridIndex = -1;
    g_portal.isDraggingOrMenuActive = false;
    g_portal.scrollOffsetY = 0;
    g_portal.items.clear();

    if (g_portal.pShellBrowser) {
        g_portal.pShellBrowser->Release();
        g_portal.pShellBrowser = nullptr;
    }
}

void StartPortalFadeOut() {
    if (!g_portal.hPortalWnd || !IsWindow(g_portal.hPortalWnd)) {
        DestroyPortalImmediately();
        return;
    }
    if (g_portal.animState == PortalAnimState::Closing || g_portal.animState == PortalAnimState::Hidden) {
        return;
    }
    g_portal.animState = PortalAnimState::Closing;
    g_portal.animStartTick = GetTickCount64();
    SetTimer(g_portal.hPortalWnd, TIMER_ID_ANIMATION, 12, nullptr);
}

// Проверяет, находится ли курсор мыши в зоне иконки папки, окна портала или в коридоре между ними
bool IsCursorInsideSafeZone() {
    if (g_portal.isDraggingOrMenuActive) return true;

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

    case WM_PAINT: {
        PAINTSTRUCT ps = {};
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        RenderAndPresentPortal(hWnd);
        return 0;
    }

    case WM_MOUSEMOVE: {
        TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
        TrackMouseEvent(&tme);

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

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

        int newHover = -1;
        const int headerHitBottom = ScaleByDpi(42, g_portal.dpi > 0 ? g_portal.dpi : 96);
        // Игнорируем наведение на карточки, если курсор находится в области шапки
        if (pt.y > headerHitBottom) {
            for (size_t i = 0; i < g_portal.items.size(); ++i) {
                if (PtInRect(&g_portal.items[i].boundsRect, pt)) {
                    newHover = static_cast<int>(i);
                    break;
                }
            }
        }
        if (newHover != g_portal.hoveredGridIndex) {
            g_portal.hoveredGridIndex = newHover;
            RenderAndPresentPortal(hWnd);
        }
        return 0;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (g_portal.maxScrollY > 0) {
            int step = ScaleByDpi(delta / 3, g_portal.dpi > 0 ? g_portal.dpi : 96);
            g_portal.scrollOffsetY = std::clamp(g_portal.scrollOffsetY - step, 0, g_portal.maxScrollY);
            RenderAndPresentPortal(hWnd);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_portal.pressedGridIndex = -1;
        const int headerHitBottom = ScaleByDpi(42, g_portal.dpi > 0 ? g_portal.dpi : 96);
        if (pt.y > headerHitBottom) {
            for (size_t i = 0; i < g_portal.items.size(); ++i) {
                if (PtInRect(&g_portal.items[i].boundsRect, pt)) {
                    g_portal.pressedGridIndex = static_cast<int>(i);
                    g_portal.pressStartPt = pt;
                    SetCapture(hWnd);
                    break;
                }
            }
        }
        return 0;
    }

    case WM_LBUTTONUP: {
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
                if (!targetPath.empty()) {
                    PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(targetPath.c_str());
                    if (pidl && g_portal.pShellBrowser) {
                        g_portal.pShellBrowser->BrowseObject(pidl, SBSP_DEFBROWSER);
                        ILFree(pidl);
                    } else {
                        ShellExecuteW(nullptr, L"open", targetPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                    }
                    StartPortalFadeOut();
                }
            }
        }
        return 0;
    }

    case WM_RBUTTONDOWN:
        return 0;

    case WM_RBUTTONUP: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        const int headerHitBottom = ScaleByDpi(42, g_portal.dpi > 0 ? g_portal.dpi : 96);
        if (pt.y > headerHitBottom) {
            for (size_t i = 0; i < g_portal.items.size(); ++i) {
                if (PtInRect(&g_portal.items[i].boundsRect, pt)) {
                    POINT ptScreen = pt;
                    ClientToScreen(hWnd, &ptScreen);
                    ShowNativeShellContextMenu(hWnd, static_cast<int>(i), ptScreen);
                    break;
                }
            }
        }
        return 0;
    }

    case WM_MOUSELEAVE: {
        if (g_portal.isDraggingOrMenuActive) return 0;
        if (g_portal.hoveredGridIndex != -1) {
            g_portal.hoveredGridIndex = -1;
            RenderAndPresentPortal(hWnd);
        }
        if (!IsCursorInsideSafeZone()) {
            StartPortalFadeOut();
        }
        return 0;
    }

    case WM_TIMER: {
        if (wParam == TIMER_ID_ANIMATION) {
            ULONGLONG now = GetTickCount64();
            float duration = static_cast<float>(std::clamp(g_settings.animDurationMs, 60, 1000));
            float elapsed = static_cast<float>(now - g_portal.animStartTick);
            float t = std::clamp(elapsed / duration, 0.0f, 1.0f);

            if (g_portal.animState == PortalAnimState::Opening) {
                g_portal.animProgress = t;
                UpdatePortalMorphGeometry(hWnd, g_portal.animProgress, true);
                if (t >= 1.0f) {
                    g_portal.animState = PortalAnimState::Visible;
                    KillTimer(hWnd, TIMER_ID_ANIMATION);
                }
            } else if (g_portal.animState == PortalAnimState::Closing) {
                g_portal.animProgress = 1.0f - t;
                UpdatePortalMorphGeometry(hWnd, g_portal.animProgress, false);
                if (t >= 1.0f) {
                    KillTimer(hWnd, TIMER_ID_ANIMATION);
                    DestroyPortalImmediately();
                }
            }
            return 0;
        }
        if (wParam == TIMER_ID_MOUSECHECK) {
            if (g_portal.isDraggingOrMenuActive) return 0;
            if (!g_settings.enabled || !IsCursorInsideSafeZone()) {
                StartPortalFadeOut();
            }
            return 0;
        }
        break;
    }

    case WM_DESTROY:
        KillTimer(hWnd, TIMER_ID_ANIMATION);
        KillTimer(hWnd, TIMER_ID_MOUSECHECK);
        return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Создание и запуск портала для папки на рабочем столе
// ---------------------------------------------------------------------------
void OpenPortalForDesktopFolder(HWND hListView, int itemIndex, const std::wstring& folderPath, const std::wstring& folderTitle) {
    if (!IsWindow(hListView) || g_unloading.load()) return;

    // Если этот же портал уже открыт или открывается — не перезапускаем
    if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd) &&
        g_portal.activeItemIndex == itemIndex &&
        g_portal.animState != PortalAnimState::Closing) {
        return;
    }

    DestroyPortalImmediately();

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

    // Создаем окно с обязательными флагами WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW
    HWND hPortal = CreateWindowExW(
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
    g_portal.animStartTick = GetTickCount64();
    g_portal.animProgress = 0.0f;
    g_portal.hoveredGridIndex = -1;
    g_portal.pressedGridIndex = -1;
    g_portal.isDraggingOrMenuActive = false;
    g_portal.scrollOffsetY = 0;

    // Инициализируем легковесный экземпляр IShellBrowser и мгновенно считываем список файлов
    g_portal.pShellBrowser = new LightweightPortalShellBrowser(hPortal);
    g_portal.pShellBrowser->PopulateItemsFromDirectory(folderPath, g_portal.items, 45);

    ApplyPortalDwmVisuals(hPortal);

    g_portal.animProgress = 0.02f;
    UpdatePortalMorphGeometry(hPortal, g_portal.animProgress, true);
    ShowWindow(hPortal, SW_SHOWNOACTIVATE);

    SetTimer(hPortal, TIMER_ID_ANIMATION, 12, nullptr);
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

    switch (uMsg) {
    case WM_MOUSEMOVE: {
        if (g_settings.enabled) {
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.hwndTrack = hWnd;
            tme.dwHoverTime = static_cast<DWORD>(std::clamp(g_settings.hoverDelayMs, 50, 2000));
            TrackMouseEvent(&tme);

            if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
                if (!IsCursorInsideSafeZone()) {
                    StartPortalFadeOut();
                }
            }
        }
        break;
    }

    case WM_MOUSEHOVER: {
        if (g_settings.enabled && IsTrueDesktopListView(hWnd)) {
            LVHITTESTINFO ht = {};
            ht.pt.x = GET_X_LPARAM(lParam);
            ht.pt.y = GET_Y_LPARAM(lParam);
            int hitIndex = static_cast<int>(SendMessageW(hWnd, LVM_HITTEST, 0, reinterpret_cast<LPARAM>(&ht)));

            if (hitIndex >= 0 && (ht.flags & (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEM))) {
                std::wstring folderPath;
                std::wstring folderTitle;
                if (GetDesktopItemDirectoryPath(hWnd, hitIndex, folderPath, folderTitle)) {
                    OpenPortalForDesktopFolder(hWnd, hitIndex, folderPath, folderTitle);
                }
            }
        }
        break;
    }

    case WM_MOUSELEAVE: {
        if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
            if (!IsCursorInsideSafeZone()) {
                StartPortalFadeOut();
            }
        }
        break;
    }

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
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

    g_settings.enabled          = Wh_GetIntSetting(L"enabled") != 0;
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
    if (!classAtom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"Failed to register portal window class (error: %u)", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return FALSE;
    }
    g_portalClassRegistered = true;

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
            if (!g_settings.enabled) {
                DestroyPortalImmediately();
            } else if (g_portal.hPortalWnd && IsWindow(g_portal.hPortalWnd)) {
                ApplyPortalDwmVisuals(g_portal.hPortalWnd);
                RenderAndPresentPortal(g_portal.hPortalWnd);
            }
        });
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Desktop Folder Portals mod...");
    g_unloading.store(true);

    // Снимаем сабклассы и уничтожаем окно портала строго в потоке окна рабочего стола
    DetachAllSubclassedDesktopListViewsAndDestroyPortal();

    // Освобождаем общие графические ресурсы и деинициализируем GDI+ только после закрытия окна
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
