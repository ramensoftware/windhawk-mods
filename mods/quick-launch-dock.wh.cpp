// ==WindhawkMod==
// @id              quick-launch-dock
// @name            Quick Launch Dock
// @name:ru-RU      Панель быстрого запуска
// @description     A slim animated dock on the screen edge: drop a shortcut or pick a program with "+", then launch it in one click
// @description:ru-RU Тонкая анимированная панель у края экрана: перетащи ярлык или выбери программу через «+» и запускай в один клик
// @version         1.0.0
// @author          cheliks1123
// @github          https://github.com/cheliks1123
// @include         windhawk.exe
// @compilerOptions -lshell32 -lcomdlg32 -lgdi32 -luser32 -lole32 -loleaut32 -ldwmapi -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Launch Dock

![Quick Launch Dock demo - dragging a shortcut onto the dock](https://raw.githubusercontent.com/cheliks1123/Quick-launch-dock/main/quck-lauch-dock.gif)

*Drag a shortcut onto the dock and it stays there. Click an icon to launch it.*

A slim, smoothly animated dock that lives on the edge of your screen and gives you
one-click access to your favourite programs, files and folders. Think of it as a
tiny launcher bar with "+" slots: drop a shortcut onto it, or click "+" and pick a
program, and it stays there until you remove it.

## Features

- **Add programs your way**
  - Drag and drop shortcuts, executables, files or folders onto the dock.
  - Click the **+** slot to choose a program in a file dialog (shortcuts are kept as shortcuts, so their icons stay correct).
  - Right-click the **+** slot to paste a path from the clipboard (quotes and `%ENVIRONMENT%` variables are handled).
- **One-click launch** — click an icon to start it. The working directory is set automatically.
- **Right-click menu** on any icon: Open, Run as administrator, Show in folder, Move up / Move down, Remove.
- **Persistent list** — your slots are saved by Windhawk and restored after a restart.
- **Auto-hide** — the dock collapses into a thin strip and smoothly slides out when you move the mouse to the screen edge.
- **Smooth animations** — slide-in/out, hover highlight fade, icon zoom on hover, appear/disappear and re-ordering animations. Frames are synchronized with your monitor's refresh rate, so it stays smooth on 60/144/240 Hz displays.
- **High-quality rendering** — anti-aliased rounded corners and high-quality icon scaling (GDI+), per-pixel transparent window, DPI-aware sizes.
- **Fullscreen aware** — optionally hides itself while a fullscreen app or game is active.
- **Fully themeable** — background, hover, accent (plus sign and handle) and border colors are configurable (`#RRGGBB`).
- **Left or right edge**, adjustable icon size, tooltips with the item name.
- Does not use the taskbar, does not hook any functions and does not modify system files. Disabling the mod removes the dock completely.

## How it differs from existing mods

- **Quick Launch & Media Panel** is a collapsible panel at the top of the screen with shortcut pages and media controls. This mod is a minimal vertical dock on the left or right screen edge: a column of icons and a "+" slot, with no pages and no media controls.
- **Left Taskbar Quick Pin Dock** is attached to the taskbar next to the Start button. This mod floats on the screen edge independently of the taskbar.
- What is specific to this mod: auto-hide into a thin strip that smoothly slides out on hover, hide-in-fullscreen, per-item "Run as administrator" and "Show in folder", and configurable colors.

## How to use

1. Enable the mod. A dock appears at the right edge of the primary monitor (vertically centered).
2. Drag a shortcut/program/folder onto it, or click **+**.
3. Click an icon to launch. Right-click an icon for more actions.
4. With auto-hide enabled, move the cursor to the screen edge to reveal the dock.

## Settings

| Setting | Description |
| --- | --- |
| Screen side | Right or left edge of the primary monitor. |
| Icon size | Icon size in pixels at 100% DPI (16–96). Scales with system DPI. |
| Auto-hide | Collapse to a thin strip until the cursor touches the edge. |
| Hide when a fullscreen app is active | Hides the dock above fullscreen windows/games. |
| Animations | Turns all animations on or off. |
| Background / Hover / Accent / Border color | Colors in `#RRGGBB` format. Invalid values fall back to the defaults. |

## Notes and limitations

- The mod runs as a standalone tool in its own dedicated process (it is not injected into Explorer and hooks nothing), so a problem in the mod can never affect the Windows shell, and only one instance is ever running.
- Drag and drop from windows running **as administrator** is blocked by Windows (UIPI). Use the **+** button in that case.
- The dock is placed on the **primary** monitor. With auto-hide and another monitor attached on the same side, the cursor may pass the edge too quickly to reveal the dock — disable auto-hide there.
- The number of slots is limited by the screen height.
- Slots are stored as file paths. If a file is moved or deleted, its icon becomes generic until you remove it.

---

# Панель быстрого запуска

Тонкая плавно анимированная панель у края экрана для быстрого запуска любимых
программ, файлов и папок. По сути — мини-лаунчер с «плюсиками»: перетащи на него
ярлык или нажми «+» и выбери программу — она останется на панели, пока ты сам её не уберёшь.

## Возможности

- **Добавление как удобно**
  - Перетаскивание ярлыков, exe-файлов, файлов и папок прямо на панель.
  - Клик по **+** — выбор программы через диалог (ярлыки сохраняются как ярлыки, поэтому иконки остаются правильными).
  - ПКМ по **+** — вставить путь из буфера обмена (кавычки и переменные вида `%APPDATA%` обрабатываются).
- **Запуск в один клик** — рабочая папка выставляется автоматически.
- **Контекстное меню** на любой иконке: Открыть, Запуск от имени администратора, Показать в папке, Переместить выше / ниже, Удалить.
- **Список сохраняется** — слоты хранятся в Windhawk и восстанавливаются после перезагрузки.
- **Автоскрытие** — в покое панель сворачивается в тонкую полоску и плавно выезжает, когда подводишь курсор к краю экрана.
- **Плавные анимации** — выезд и сворачивание, плавная подсветка, увеличение иконки при наведении, анимации появления, удаления и перестановки. Кадры синхронизированы с частотой монитора, поэтому всё плавно на 60/144/240 Гц.
- **Качественная отрисовка** — сглаженные скруглённые углы и качественное масштабирование иконок (GDI+), окно с попиксельной прозрачностью, учёт DPI.
- **Полноэкранные приложения** — при желании панель сама прячется, пока открыта полноэкранная программа или игра.
- **Настраиваемые цвета** — фон, подсветка, акцент (плюс и полоска) и рамка задаются в формате `#RRGGBB`.
- **Левый или правый край**, размер иконок, подсказки с названием элемента.
- Не использует панель задач, не перехватывает функции и не меняет системные файлы. При отключении мода панель полностью исчезает.

## Чем отличается от существующих модов

- **Quick Launch & Media Panel** — сворачиваемая панель у верхнего края экрана со страницами ярлыков и управлением медиа. Этот мод — минимальная вертикальная панель у левого или правого края экрана: колонка иконок и слот «+», без страниц и без медиа-кнопок.
- **Left Taskbar Quick Pin Dock** — панель, прикреплённая к панели задач рядом с кнопкой «Пуск». Этот мод свободно располагается у края экрана и от панели задач не зависит.
- Особенности этого мода: автоскрытие в тонкую полоску с плавным выездом при наведении, скрытие в полноэкранных приложениях, «Запуск от имени администратора» и «Показать в папке» для каждого элемента, настраиваемые цвета.

## Как пользоваться

1. Включи мод. У правого края основного монитора по центру по вертикали появится панель.
2. Перетащи на неё ярлык, программу или папку либо нажми **+**.
3. Клик по иконке — запуск. ПКМ по иконке — дополнительные действия.
4. При включённом автоскрытии подведи курсор к краю экрана, чтобы показать панель.

## Настройки

| Настройка | Описание |
| --- | --- |
| Сторона экрана | Правый или левый край основного монитора. |
| Размер иконок | Размер в пикселях при 100% DPI (16–96), масштабируется вместе с DPI системы. |
| Автоскрытие | Сворачивать в тонкую полоску, пока курсор не коснётся края. |
| Прятать в полноэкранных приложениях | Скрывает панель поверх полноэкранных окон и игр. |
| Анимации | Включает и выключает все анимации. |
| Цвета фона / подсветки / акцента / рамки | Формат `#RRGGBB`. При неверном значении используется цвет по умолчанию. |

## Примечания и ограничения

- Мод работает как самостоятельный инструмент в собственном процессе (он не внедряется в проводник и ничего не перехватывает), поэтому сбой мода не может повлиять на оболочку Windows, а одновременно запущена всегда только одна копия.
- Перетаскивание из окон, запущенных **от имени администратора**, блокируется Windows (UIPI). В этом случае используй кнопку **+**.
- Панель располагается на **основном** мониторе. При автоскрытии и втором мониторе с той же стороны курсор может проскакивать край слишком быстро — в таком случае отключи автоскрытие.
- Количество слотов ограничено высотой экрана.
- Слоты хранятся как пути к файлам. Если файл перенесён или удалён, иконка станет стандартной, пока ты не уберёшь слот.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- side: right
  $name: Screen side
  $name:ru-RU: Сторона экрана
  $options:
  - right: Right
  - left: Left
- iconSize: 36
  $name: Icon size (px at 100% DPI)
  $name:ru-RU: Размер иконок (px при 100% DPI)
- autoHide: true
  $name: Auto-hide (thin strip until hovered)
  $name:ru-RU: Автоскрытие (тонкая полоска до наведения)
- hideInFullscreen: true
  $name: Hide when a fullscreen app is active
  $name:ru-RU: Прятать в полноэкранных приложениях
- animations: true
  $name: Animations
  $name:ru-RU: Анимации
- bgColor: "#202020"
  $name: Background color (#RRGGBB)
  $name:ru-RU: Цвет фона (#RRGGBB)
- hoverColor: "#424242"
  $name: Hover highlight color (#RRGGBB)
  $name:ru-RU: Цвет подсветки при наведении (#RRGGBB)
- accentColor: "#BEBEBE"
  $name: Accent color - "+" sign and handle (#RRGGBB)
  $name:ru-RU: Акцентный цвет - плюс и полоска (#RRGGBB)
- borderColor: "#464646"
  $name: Border color (#RRGGBB)
  $name:ru-RU: Цвет рамки (#RRGGBB)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shldisp.h>
#include <servprov.h>
#include <commdlg.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <math.h>
#include <stddef.h>

#include <algorithm>
#include <string>
#include <vector>

struct GpStartupInput {
    UINT32 version;
    void* debugCallback;
    BOOL suppressBackgroundThread;
    BOOL suppressExternalCodecs;
};

static struct {
    HMODULE mod = nullptr;
    ULONG_PTR token = 0;
    bool ok = false;

    int(WINAPI* Startup)(ULONG_PTR*, const GpStartupInput*, void*);
    void(WINAPI* Shutdown)(ULONG_PTR);
    int(WINAPI* CreateBitmapFromScan0)(int, int, int, int, BYTE*, void**);
    int(WINAPI* GetImageGraphicsContext)(void*, void**);
    int(WINAPI* DeleteGraphics)(void*);
    int(WINAPI* SetSmoothing)(void*, int);
    int(WINAPI* SetInterp)(void*, int);
    int(WINAPI* SetPixOff)(void*, int);
    int(WINAPI* Clear)(void*, UINT32);
    int(WINAPI* CreateFill)(UINT32, void**);
    int(WINAPI* DelBrush)(void*);
    int(WINAPI* CreatePath)(int, void**);
    int(WINAPI* DeletePath)(void*);
    int(WINAPI* AddArc)(void*, float, float, float, float, float, float);
    int(WINAPI* ClosePathFig)(void*);
    int(WINAPI* FillPath)(void*, void*, void*);
    int(WINAPI* DrawPath)(void*, void*, void*);
    int(WINAPI* CreatePen)(UINT32, float, int, void**);
    int(WINAPI* DelPen)(void*);
    int(WINAPI* SetPenStartCap)(void*, int);
    int(WINAPI* SetPenEndCap)(void*, int);
    int(WINAPI* DrawLine)(void*, void*, float, float, float, float);
    int(WINAPI* BitmapFromHICON)(HICON, void**);
    int(WINAPI* DisposeImage)(void*);
    int(WINAPI* DrawImageRect)(void*, void*, float, float, float, float);
    int(WINAPI* GetImageWidth)(void*, UINT*);
} Gp;

#define GP_LOAD(field, sym)                                                                  \
    Gp.field = reinterpret_cast<decltype(Gp.field)>((void*)GetProcAddress(Gp.mod, sym));     \
    if (!Gp.field) return false;

static bool GpInit() {
    Gp.mod = LoadLibraryW(L"gdiplus.dll");
    if (!Gp.mod) return false;
    GP_LOAD(Startup, "GdiplusStartup");
    GP_LOAD(Shutdown, "GdiplusShutdown");
    GP_LOAD(CreateBitmapFromScan0, "GdipCreateBitmapFromScan0");
    GP_LOAD(GetImageGraphicsContext, "GdipGetImageGraphicsContext");
    GP_LOAD(DeleteGraphics, "GdipDeleteGraphics");
    GP_LOAD(SetSmoothing, "GdipSetSmoothingMode");
    GP_LOAD(SetInterp, "GdipSetInterpolationMode");
    GP_LOAD(SetPixOff, "GdipSetPixelOffsetMode");
    GP_LOAD(Clear, "GdipGraphicsClear");
    GP_LOAD(CreateFill, "GdipCreateSolidFill");
    GP_LOAD(DelBrush, "GdipDeleteBrush");
    GP_LOAD(CreatePath, "GdipCreatePath");
    GP_LOAD(DeletePath, "GdipDeletePath");
    GP_LOAD(AddArc, "GdipAddPathArc");
    GP_LOAD(ClosePathFig, "GdipClosePathFigure");
    GP_LOAD(FillPath, "GdipFillPath");
    GP_LOAD(DrawPath, "GdipDrawPath");
    GP_LOAD(CreatePen, "GdipCreatePen1");
    GP_LOAD(DelPen, "GdipDeletePen");
    GP_LOAD(SetPenStartCap, "GdipSetPenStartCap");
    GP_LOAD(SetPenEndCap, "GdipSetPenEndCap");
    GP_LOAD(DrawLine, "GdipDrawLine");
    GP_LOAD(BitmapFromHICON, "GdipCreateBitmapFromHICON");
    GP_LOAD(DisposeImage, "GdipDisposeImage");
    GP_LOAD(DrawImageRect, "GdipDrawImageRect");
    GP_LOAD(GetImageWidth, "GdipGetImageWidth");

    GpStartupInput in{1, nullptr, FALSE, FALSE};
    if (Gp.Startup(&Gp.token, &in, nullptr) != 0) return false;
    Gp.ok = true;
    return true;
}

static UINT32 Argb(COLORREF c, int a) {
    a = std::clamp(a, 0, 255);
    return ((UINT32)a << 24) | ((UINT32)GetRValue(c) << 16) | ((UINT32)GetGValue(c) << 8) |
           (UINT32)GetBValue(c);
}

struct Item {
    std::wstring path;
    std::wstring name;
    void* img = nullptr;
    void* imgBig = nullptr;
    float pos = 0;
    float hover = 0;
    float appear = 1;
};

struct PlusAnim {
    float pos = 0;
    float hover = 0;
};

static std::vector<Item> g_items;
static PlusAnim g_plus;
static HWND g_hwnd = nullptr;
static HWND g_tip = nullptr;
static HANDLE g_thread = nullptr;
static volatile bool g_stop = false;
static DWORD g_threadId = 0;

static bool g_expanded = true;
static bool g_modal = false;
static bool g_tracking = false;
static int g_hover = -1;
static int g_outsideTicks = 0;
static int g_toolCount = 0;
static int g_dpi = 96;

static double g_curW = 0, g_curH = 0, g_tgtW = 0, g_tgtH = 0;
static bool g_animRunning = false;
static double g_lastT = 0;

static HDC g_memDc = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_oldBmp = nullptr;
static int g_dibW = 0, g_dibH = 0;
static void* g_surf = nullptr;
static void* g_gfx = nullptr;

static struct {
    bool right = true;
    int icon = 36;
    bool autoHide = true;
    bool hideFs = true;
    bool anim = true;
    COLORREF bg = RGB(0x20, 0x20, 0x20);
    COLORREF hover = RGB(0x42, 0x42, 0x42);
    COLORREF accent = RGB(0xBE, 0xBE, 0xBE);
    COLORREF border = RGB(0x46, 0x46, 0x46);
} g_cfg;

constexpr UINT WM_RELOAD = WM_APP + 1;
constexpr UINT_PTR kTickId = 1;
constexpr wchar_t kClass[] = L"WhQuickLaunchDockWnd";

enum {
    ID_OPEN = 1, ID_ADMIN, ID_FOLDER, ID_REMOVE, ID_UP, ID_DOWN, ID_PICK, ID_PASTE
};

static int Sc(int v) { return MulDiv(v, g_dpi, 96); }
static int IconPx() { return Sc(g_cfg.icon); }
static int BigPx() { return (int)(IconPx() * 1.15f + 0.5f); }
static int Pad() { return Sc(8); }
static int Margin() { return Sc(4); }
static int StripW() { return Sc(6); }
static int SlotSize() { return IconPx() + Pad() * 2; }
static int FullW() { return SlotSize() + Margin() * 2; }
static int TotalSlots() { return (int)g_items.size() + 1; }
static bool Expanded() { return !g_cfg.autoHide || g_expanded; }

static bool IsRu() { return (GetUserDefaultUILanguage() & 0x3FF) == LANG_RUSSIAN; }
static PCWSTR Tr(PCWSTR en, PCWSTR ru) { return IsRu() ? ru : en; }

static HMONITOR PrimaryMon() {
    return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

static RECT WorkArea() {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(PrimaryMon(), &mi);
    return mi.rcWork;
}

static int MaxItems() {
    RECT wa = WorkArea();
    int avail = (wa.bottom - wa.top) - Sc(40) - Margin() * 2;
    return std::max(1, avail / SlotSize() - 1);
}

static RECT SlotRect(int i) {
    int s = SlotSize(), m = Margin();
    return RECT{m, m + i * s, m + s, m + (i + 1) * s};
}

static int HitTest(POINT pt) {
    if (!Expanded()) return -1;
    int s = SlotSize(), m = Margin();
    if (pt.x < m || pt.x >= m + s || pt.y < m) return -1;
    int i = (pt.y - m) / s;
    return i < TotalSlots() ? i : -1;
}

static double NowSec() {
    static double freq = 0;
    if (freq == 0) {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        freq = (double)f.QuadPart;
    }
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / freq;
}

static COLORREF ParseColor(PCWSTR s, COLORREF def) {
    if (!s) return def;
    if (*s == L'#') s++;
    if (wcslen(s) != 6) return def;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (!end || *end) return def;
    return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
}

static COLORREF ColorSetting(PCWSTR name, COLORREF def) {
    PCWSTR s = Wh_GetStringSetting(name);
    COLORREF c = ParseColor(s, def);
    if (s) Wh_FreeStringSetting(s);
    return c;
}

static void LoadSettings() {
    PCWSTR side = Wh_GetStringSetting(L"side");
    g_cfg.right = !(side && wcscmp(side, L"left") == 0);
    if (side) Wh_FreeStringSetting(side);
    g_cfg.icon = std::clamp((int)Wh_GetIntSetting(L"iconSize"), 16, 96);
    g_cfg.autoHide = Wh_GetIntSetting(L"autoHide") != 0;
    g_cfg.hideFs = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    g_cfg.anim = Wh_GetIntSetting(L"animations") != 0;
    g_cfg.bg = ColorSetting(L"bgColor", RGB(0x20, 0x20, 0x20));
    g_cfg.hover = ColorSetting(L"hoverColor", RGB(0x42, 0x42, 0x42));
    g_cfg.accent = ColorSetting(L"accentColor", RGB(0xBE, 0xBE, 0xBE));
    g_cfg.border = ColorSetting(L"borderColor", RGB(0x46, 0x46, 0x46));
}

typedef HRESULT(WINAPI* SHGetImageList_t)(int, REFIID, void**);

static HICON LoadIconFor(const std::wstring& path) {
    SHFILEINFOW sfi{};
    DWORD_PTR r = SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
    if (r) {
        static const GUID kIID_IImageList = {
            0x46EB5926, 0x582E, 0x4017, {0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x20}};
        static SHGetImageList_t pGet = reinterpret_cast<SHGetImageList_t>(
            (void*)GetProcAddress(GetModuleHandleW(L"shell32.dll"), "SHGetImageList"));
        int big = BigPx();
        int shil = big <= 40 ? 0  : (big <= 52 ? 2  : 4 );
        void* pil = nullptr;
        if (pGet && SUCCEEDED(pGet(shil, kIID_IImageList, &pil)) && pil) {
            HICON h = ImageList_GetIcon((HIMAGELIST)pil, sfi.iIcon, ILD_NORMAL);
            ((IUnknown*)pil)->Release();
            if (h) return h;
        }
    }
    SHFILEINFOW sfi2{};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi2, sizeof(sfi2), SHGFI_ICON | SHGFI_LARGEICON) &&
        sfi2.hIcon) {
        return sfi2.hIcon;
    }
    return CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
}

static void* Resample(void* src, int size) {
    void* bmp = nullptr;
    if (Gp.CreateBitmapFromScan0(size, size, 0, 0x000E200B, nullptr, &bmp) != 0 || !bmp) {
        return nullptr;
    }
    void* g = nullptr;
    if (Gp.GetImageGraphicsContext(bmp, &g) != 0 || !g) {
        Gp.DisposeImage(bmp);
        return nullptr;
    }
    Gp.SetInterp(g, 7);
    Gp.SetPixOff(g, 2);
    Gp.Clear(g, 0x00000000);
    Gp.DrawImageRect(g, src, 0.0f, 0.0f, (float)size, (float)size);
    Gp.DeleteGraphics(g);
    return bmp;
}

static void* ScaleTo(void* src, int dst) {
    UINT w = 0;
    Gp.GetImageWidth(src, &w);
    int cur = (int)w;
    void* tmp = nullptr;
    void* from = src;
    while (cur / 2 >= dst) {
        void* nb = Resample(from, cur / 2);
        if (!nb) break;
        if (tmp) Gp.DisposeImage(tmp);
        tmp = nb;
        from = nb;
        cur /= 2;
    }
    void* res = Resample(from, dst);
    if (tmp) Gp.DisposeImage(tmp);
    return res;
}

static void FreeImg(Item& it) {
    if (it.img) {
        Gp.DisposeImage(it.img);
        it.img = nullptr;
    }
    if (it.imgBig) {
        Gp.DisposeImage(it.imgBig);
        it.imgBig = nullptr;
    }
}

static void LoadImages(Item& it) {
    FreeImg(it);
    HICON h = LoadIconFor(it.path);
    if (!h) return;
    void* src = nullptr;
    if (Gp.BitmapFromHICON(h, &src) == 0 && src) {
        it.img = ScaleTo(src, IconPx());
        it.imgBig = ScaleTo(src, BigPx());
        if (!it.img && !it.imgBig) {
            it.img = src;
        } else {
            Gp.DisposeImage(src);
        }
    }
    DestroyIcon(h);
}

static std::wstring TitleOf(const std::wstring& p) {
    SHFILEINFOW sfi{};
    if (SHGetFileInfoW(p.c_str(), 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME) &&
        sfi.szDisplayName[0]) {
        return sfi.szDisplayName;
    }
    size_t pos = p.find_last_of(L"\\/");
    return pos == std::wstring::npos ? p : p.substr(pos + 1);
}

static void ReloadIcons() {
    for (auto& it : g_items) LoadImages(it);
}

static void SaveItems() {
    std::wstring s;
    for (auto& it : g_items) {
        if (!s.empty()) s += L'|';
        s += it.path;
    }
    Wh_SetStringValue(L"items", s.c_str());
}

static void LoadItems() {
    for (auto& it : g_items) FreeImg(it);
    g_items.clear();

    std::vector<wchar_t> buf(32768, 0);
    Wh_GetStringValue(L"items", buf.data(), buf.size());
    std::wstring all = buf.data();
    size_t pos = 0;
    while (pos < all.size()) {
        size_t e = all.find(L'|', pos);
        if (e == std::wstring::npos) e = all.size();
        std::wstring p = all.substr(pos, e - pos);
        if (!p.empty()) {
            Item it;
            it.path = p;
            it.name = TitleOf(p);
            LoadImages(it);
            it.pos = (float)g_items.size();
            g_items.push_back(std::move(it));
        }
        pos = e + 1;
    }
    g_plus.pos = (float)g_items.size();
    g_plus.hover = 0;
}

static void UpdateTooltips() {
    if (!g_tip) return;
    for (int i = 0; i < g_toolCount; i++) {
        TTTOOLINFOW ti{};
        ti.cbSize = (UINT)offsetof(TTTOOLINFOW, lParam);
        ti.hwnd = g_hwnd;
        ti.uId = i;
        SendMessageW(g_tip, TTM_DELTOOLW, 0, (LPARAM)&ti);
    }
    g_toolCount = 0;
    if (!Expanded()) return;

    for (int i = 0; i < TotalSlots(); i++) {
        TTTOOLINFOW ti{};
        ti.cbSize = (UINT)offsetof(TTTOOLINFOW, lParam);
        ti.uFlags = TTF_SUBCLASS;
        ti.hwnd = g_hwnd;
        ti.uId = i;
        ti.rect = SlotRect(i);
        ti.lpszText = i < (int)g_items.size() ? (LPWSTR)g_items[i].name.c_str()
                                              : (LPWSTR)Tr(L"Add a program", L"Добавить программу");
        if (SendMessageW(g_tip, TTM_ADDTOOLW, 0, (LPARAM)&ti)) g_toolCount = i + 1;
    }
}

static void FreeSurface() {
    if (g_gfx) {
        Gp.DeleteGraphics(g_gfx);
        g_gfx = nullptr;
    }
    if (g_surf) {
        Gp.DisposeImage(g_surf);
        g_surf = nullptr;
    }
}

static void FreeDib() {
    FreeSurface();
    if (g_memDc && g_oldBmp) SelectObject(g_memDc, g_oldBmp);
    if (g_dib) DeleteObject(g_dib);
    if (g_memDc) DeleteDC(g_memDc);
    g_dib = nullptr;
    g_memDc = nullptr;
    g_oldBmp = nullptr;
    g_dibW = g_dibH = 0;
}

static bool EnsureSurface(int w, int h) {
    if (g_dib && g_gfx && w <= g_dibW && h <= g_dibH) return true;

    RECT wa = WorkArea();
    int nw = std::max(std::max(w, g_dibW), FullW());
    int nh = std::max(std::max(h, g_dibH), (int)(wa.bottom - wa.top));

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = nw;
    bi.bmiHeader.biHeight = -nh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP nb = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!nb || !bits) {
        if (nb) DeleteObject(nb);
        return false;
    }

    FreeSurface();
    if (!g_memDc) g_memDc = CreateCompatibleDC(nullptr);
    HGDIOBJ old = SelectObject(g_memDc, nb);
    if (!g_oldBmp) g_oldBmp = old;
    if (g_dib) DeleteObject(g_dib);
    g_dib = nb;
    g_dibW = nw;
    g_dibH = nh;

    const int kPixelFormat32bppPARGB = 0x000E200B;
    if (Gp.CreateBitmapFromScan0(nw, nh, nw * 4, kPixelFormat32bppPARGB, (BYTE*)bits, &g_surf) != 0 ||
        !g_surf) {
        return false;
    }
    if (Gp.GetImageGraphicsContext(g_surf, &g_gfx) != 0 || !g_gfx) return false;
    Gp.SetSmoothing(g_gfx, 4);
    Gp.SetInterp(g_gfx, 7);
    Gp.SetPixOff(g_gfx, 2);
    return true;
}

static void* RoundPath(float x, float y, float w, float h, float r) {
    void* p = nullptr;
    if (Gp.CreatePath(0, &p) != 0 || !p) return nullptr;
    r = std::max(0.5f, std::min(r, std::min(w, h) / 2.0f));
    float d = r * 2;
    Gp.AddArc(p, x, y, d, d, 180, 90);
    Gp.AddArc(p, x + w - d, y, d, d, 270, 90);
    Gp.AddArc(p, x + w - d, y + h - d, d, d, 0, 90);
    Gp.AddArc(p, x, y + h - d, d, d, 90, 90);
    Gp.ClosePathFig(p);
    return p;
}

static void FillRound(void* g, float x, float y, float w, float h, float r, COLORREF c, int a) {
    void* path = RoundPath(x, y, w, h, r);
    if (!path) return;
    void* br = nullptr;
    if (Gp.CreateFill(Argb(c, a), &br) == 0 && br) {
        Gp.FillPath(g, br, path);
        Gp.DelBrush(br);
    }
    Gp.DeletePath(path);
}

static float ExpandFactor() {
    if (!g_cfg.autoHide) return 1.0f;
    double full = FullW(), strip = StripW();
    return std::clamp((float)((g_curW - strip) / (full - strip)), 0.0f, 1.0f);
}

static void Render() {
    if (!g_hwnd || !Gp.ok || g_curW <= 0) return;
    int w = (int)(g_curW + 0.5), h = (int)(g_curH + 0.5);
    if (w < 2 || h < 2) return;
    if (!EnsureSurface(w, h)) return;

    void* g = g_gfx;
    Gp.Clear(g, 0x00000000);

    float R = std::min((float)Sc(12), std::min(w, h) / 2.0f);
    void* bgPath = RoundPath(0.5f, 0.5f, w - 1.0f, h - 1.0f, R);
    if (bgPath) {
        void* br = nullptr;
        if (Gp.CreateFill(Argb(g_cfg.bg, 255), &br) == 0 && br) {
            Gp.FillPath(g, br, bgPath);
            Gp.DelBrush(br);
        }
        void* pen = nullptr;
        if (Gp.CreatePen(Argb(g_cfg.border, 255), 1.0f, 2, &pen) == 0 && pen) {
            Gp.DrawPath(g, pen, bgPath);
            Gp.DelPen(pen);
        }
        Gp.DeletePath(bgPath);
    }

    float e = ExpandFactor();

    if (e < 0.98f) {
        void* pen = nullptr;
        if (Gp.CreatePen(Argb(g_cfg.accent, (int)(150 * (1.0f - e))), (float)std::max(2, Sc(2)), 2,
                         &pen) == 0 && pen) {
            Gp.SetPenStartCap(pen, 2);
            Gp.SetPenEndCap(pen, 2);
            float cx = w / 2.0f, cy = h / 2.0f, half = (float)Sc(18);
            Gp.DrawLine(g, pen, cx, cy - half, cx, cy + half);
            Gp.DelPen(pen);
        }
    }

    if (e > 0.02f) {
        float s = (float)SlotSize(), m = (float)Margin(), px = (float)IconPx();

        float offX = g_cfg.right ? 0.0f : (float)(w - FullW());
        float left = m + offX;
        int nItems = (int)g_items.size();

        for (int i = 0; i <= nItems; i++) {
            bool isPlus = i == nItems;
            float pos = isPlus ? g_plus.pos : g_items[i].pos;
            float hov = isPlus ? g_plus.hover : g_items[i].hover;
            float app = isPlus ? 1.0f : g_items[i].appear;

            float top = m + pos * s;
            float cx = left + s / 2.0f, cy = top + s / 2.0f;

            if (hov > 0.01f) {
                FillRound(g, left + 1, top + 1, s - 2, s - 2, (float)Sc(10), g_cfg.hover,
                          (int)(hov * 255));
            }

            float scale = (1.0f + 0.15f * hov) * app;

            if (isPlus) {
                float arm = px * 0.25f * scale;
                void* pen = nullptr;
                if (Gp.CreatePen(Argb(g_cfg.accent, 255), (float)std::max(2, Sc(2)), 2, &pen) == 0 &&
                    pen) {
                    Gp.SetPenStartCap(pen, 2);
                    Gp.SetPenEndCap(pen, 2);
                    Gp.DrawLine(g, pen, cx - arm, cy, cx + arm, cy);
                    Gp.DrawLine(g, pen, cx, cy - arm, cx, cy + arm);
                    Gp.DelPen(pen);
                }
            } else {
                float sz = px * scale;

                void* im = (scale > 1.02f && g_items[i].imgBig) ? g_items[i].imgBig : g_items[i].img;
                if (!im) im = g_items[i].imgBig;
                if (sz >= 2.0f && im) Gp.DrawImageRect(g, im, cx - sz / 2, cy - sz / 2, sz, sz);
            }
        }
    }

    GdiFlush();

    RECT wa = WorkArea();
    POINT dst{g_cfg.right ? wa.right - w : wa.left, wa.top + ((wa.bottom - wa.top) - h) / 2};
    SIZE sz{w, h};
    POINT src{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, nullptr, &dst, &sz, g_memDc, &src, 0, &bf, ULW_ALPHA);
}

static float Approach(float cur, float tgt, float k) {
    if (k >= 0.999f) return tgt;
    float d = tgt - cur;
    if (fabsf(d) < 0.003f) return tgt;
    return cur + d * k;
}

static void AnimStep() {
    double now = NowSec();
    float dt = (float)std::min(now - g_lastT, 0.05);
    g_lastT = now;
    float k = g_cfg.anim ? 1.0f - expf(-dt / 0.055f) : 1.0f;
    bool busy = false;

    auto stepF = [&](float& v, float t) {
        v = Approach(v, t, k);
        if (v != t) busy = true;
    };
    auto stepD = [&](double& v, double t) {
        double d = t - v;
        if (k >= 0.999f || fabs(d) < 0.25) {
            v = t;
        } else {
            v += d * k;
            busy = true;
        }
    };

    stepD(g_curW, g_tgtW);
    stepD(g_curH, g_tgtH);

    int n = (int)g_items.size();
    for (int i = 0; i < n; i++) {
        stepF(g_items[i].pos, (float)i);
        stepF(g_items[i].hover, g_hover == i ? 1.0f : 0.0f);
        stepF(g_items[i].appear, 1.0f);
    }
    stepF(g_plus.pos, (float)n);
    stepF(g_plus.hover, g_hover == n ? 1.0f : 0.0f);

    Render();

    if (!busy) g_animRunning = false;
}

static void StartAnim() {
    if (!g_hwnd || g_animRunning) return;
    g_lastT = NowSec() - 0.016;
    g_animRunning = true;
}

static void Relayout() {
    if (!g_hwnd) return;
    g_tgtW = Expanded() ? FullW() : StripW();
    g_tgtH = TotalSlots() * SlotSize() + Margin() * 2;
    if (g_curW <= 0) {
        g_curW = g_tgtW;
        g_curH = g_tgtH;
        Render();
    }
    UpdateTooltips();
    StartAnim();
}

static void Msg(const std::wstring& text) {
    bool prev = g_modal;
    g_modal = true;
    MessageBoxW(g_hwnd, text.c_str(), L"Quick Launch Dock", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
    g_modal = prev;
}

static bool AddPath(std::wstring p) {
    while (!p.empty() && (p.front() == L'"' || iswspace(p.front()))) p.erase(p.begin());
    while (!p.empty() && (p.back() == L'"' || iswspace(p.back()))) p.pop_back();
    if (p.empty()) return false;

    wchar_t exp[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(p.c_str(), exp, ARRAYSIZE(exp));
    if (n && n <= ARRAYSIZE(exp)) p = exp;

    if (GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Msg(std::wstring(Tr(L"Path not found:", L"Путь не найден:")) + L"\n" + p);
        return false;
    }
    if ((int)g_items.size() >= MaxItems()) {
        Msg(Tr(L"There is no more room on the dock.", L"На панели больше нет места."));
        return false;
    }
    for (auto& it : g_items)
        if (_wcsicmp(it.path.c_str(), p.c_str()) == 0) return false;

    Item it;
    it.path = p;
    it.name = TitleOf(p);
    LoadImages(it);
    it.pos = (float)g_items.size();
    it.appear = g_cfg.anim ? 0.0f : 1.0f;
    g_items.push_back(std::move(it));
    return true;
}

static void Commit() {
    SaveItems();
    Relayout();
}

static void PickAndAdd() {
    wchar_t file[MAX_PATH * 2] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hwnd;
    ofn.lpstrFilter = IsRu() ? L"Программы и ярлыки\0*.exe;*.lnk;*.bat;*.cmd;*.url\0Все файлы\0*.*\0"
                            : L"Programs and shortcuts\0*.exe;*.lnk;*.bat;*.cmd;*.url\0All files\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.lpstrTitle = Tr(L"Choose a program", L"Выберите программу");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR |
                OFN_DONTADDTORECENT | OFN_NODEREFERENCELINKS;
    g_modal = true;
    BOOL ok = GetOpenFileNameW(&ofn);
    g_modal = false;
    if (ok && AddPath(file)) Commit();
}

static void PastePath() {
    std::wstring t;
    if (OpenClipboard(g_hwnd)) {
        HANDLE h = GetClipboardData(CF_UNICODETEXT);
        if (h) {
            auto p = (const wchar_t*)GlobalLock(h);
            if (p) {
                t = p;
                GlobalUnlock(h);
            }
        }
        CloseClipboard();
    }
    size_t nl = t.find_first_of(L"\r\n");
    if (nl != std::wstring::npos) t = t.substr(0, nl);
    if (t.empty()) {
        Msg(Tr(L"The clipboard does not contain a path.", L"В буфере обмена нет пути."));
        return;
    }
    if (AddPath(t)) Commit();
}

static const CLSID kCLSID_ShellWindows = {
    0x9BA05972, 0xF6A8, 0x11CF, {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};
static const GUID kSID_STopLevelBrowser = {
    0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

static bool LaunchViaExplorer(const std::wstring& file, const std::wstring& args,
                              const std::wstring& dir, PCWSTR verb) {
    IShellWindows* psw = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                __uuidof(IShellWindows), (void**)&psw)) ||
        !psw) {
        return false;
    }

    VARIANT vtLoc, vtEmpty;
    VariantInit(&vtLoc);
    VariantInit(&vtEmpty);
    vtLoc.vt = VT_I4;
    vtLoc.lVal = CSIDL_DESKTOP;
    long hwnd = 0;
    IDispatch* pdisp = nullptr;
    HRESULT hr = psw->FindWindowSW(&vtLoc, &vtEmpty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH, &pdisp);
    psw->Release();
    if (hr != S_OK || !pdisp) return false;

    IServiceProvider* psp = nullptr;
    hr = pdisp->QueryInterface(__uuidof(IServiceProvider), (void**)&psp);
    pdisp->Release();
    if (FAILED(hr) || !psp) return false;

    IShellBrowser* psb = nullptr;
    hr = psp->QueryService(kSID_STopLevelBrowser, __uuidof(IShellBrowser), (void**)&psb);
    psp->Release();
    if (FAILED(hr) || !psb) return false;

    IShellView* psv = nullptr;
    hr = psb->QueryActiveShellView(&psv);
    psb->Release();
    if (FAILED(hr) || !psv) return false;

    IDispatch* pbg = nullptr;
    hr = psv->GetItemObject(SVGIO_BACKGROUND, __uuidof(IDispatch), (void**)&pbg);
    psv->Release();
    if (FAILED(hr) || !pbg) return false;

    IShellFolderViewDual* pfvd = nullptr;
    hr = pbg->QueryInterface(__uuidof(IShellFolderViewDual), (void**)&pfvd);
    pbg->Release();
    if (FAILED(hr) || !pfvd) return false;

    IDispatch* papp = nullptr;
    hr = pfvd->get_Application(&papp);
    pfvd->Release();
    if (FAILED(hr) || !papp) return false;

    IShellDispatch2* psd = nullptr;
    hr = papp->QueryInterface(__uuidof(IShellDispatch2), (void**)&psd);
    papp->Release();
    if (FAILED(hr) || !psd) return false;

    BSTR bFile = SysAllocString(file.c_str());
    VARIANT vArgs, vDir, vVerb, vShow;
    VariantInit(&vArgs);
    VariantInit(&vDir);
    VariantInit(&vVerb);
    VariantInit(&vShow);
    if (!args.empty()) {
        vArgs.vt = VT_BSTR;
        vArgs.bstrVal = SysAllocString(args.c_str());
    }
    if (!dir.empty()) {
        vDir.vt = VT_BSTR;
        vDir.bstrVal = SysAllocString(dir.c_str());
    }
    vVerb.vt = VT_BSTR;
    vVerb.bstrVal = SysAllocString(verb);
    vShow.vt = VT_I4;
    vShow.lVal = SW_SHOWNORMAL;

    hr = psd->ShellExecute(bFile, vArgs, vDir, vVerb, vShow);

    SysFreeString(bFile);
    VariantClear(&vArgs);
    VariantClear(&vDir);
    VariantClear(&vVerb);
    psd->Release();
    return SUCCEEDED(hr);
}

static void RunShell(const std::wstring& file, const std::wstring& args, const std::wstring& dir,
                     PCWSTR verb) {
    AllowSetForegroundWindow(ASFW_ANY);
    if (LaunchViaExplorer(file, args, dir, verb)) return;
    ShellExecuteW(g_hwnd, verb, file.c_str(), args.empty() ? nullptr : args.c_str(),
                  dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
}

static void Launch(const std::wstring& path, bool admin) {
    std::wstring dir;
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos) dir = path.substr(0, pos);
    RunShell(path, L"", dir, admin ? L"runas" : L"open");
}

static void ShowMenu(POINT sp, int idx) {
    HMENU m = CreatePopupMenu();
    bool isItem = idx < (int)g_items.size();
    if (isItem) {
        AppendMenuW(m, MF_STRING, ID_OPEN, Tr(L"Open", L"Открыть"));
        AppendMenuW(m, MF_STRING, ID_ADMIN, Tr(L"Run as administrator", L"Запуск от имени администратора"));
        AppendMenuW(m, MF_STRING, ID_FOLDER, Tr(L"Show in folder", L"Показать в папке"));
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING | (idx == 0 ? MF_GRAYED : 0), ID_UP, Tr(L"Move up", L"Переместить выше"));
        AppendMenuW(m, MF_STRING | (idx + 1 >= (int)g_items.size() ? MF_GRAYED : 0), ID_DOWN,
                    Tr(L"Move down", L"Переместить ниже"));
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, ID_REMOVE, Tr(L"Remove from the dock", L"Удалить с панели"));
    } else {
        AppendMenuW(m, MF_STRING, ID_PICK, Tr(L"Choose a program...", L"Выбрать программу…"));
        AppendMenuW(m, MF_STRING, ID_PASTE, Tr(L"Paste a path from the clipboard", L"Вставить путь из буфера обмена"));
    }

    g_modal = true;
    SetForegroundWindow(g_hwnd);
    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON |
                                    (g_cfg.right ? TPM_RIGHTALIGN : TPM_LEFTALIGN),
                             sp.x, sp.y, 0, g_hwnd, nullptr);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(m);
    g_modal = false;
    g_outsideTicks = 0;

    switch (cmd) {
        case ID_OPEN:
            Launch(g_items[idx].path, false);
            break;
        case ID_ADMIN:
            Launch(g_items[idx].path, true);
            break;
        case ID_FOLDER: {
            std::wstring args = L"/select,\"" + g_items[idx].path + L"\"";
            RunShell(L"explorer.exe", args, L"", L"open");
            break;
        }
        case ID_REMOVE:
            FreeImg(g_items[idx]);
            g_items.erase(g_items.begin() + idx);
            g_hover = -1;
            Commit();
            break;
        case ID_UP:
            if (idx > 0) std::swap(g_items[idx], g_items[idx - 1]);
            Commit();
            break;
        case ID_DOWN:
            if (idx + 1 < (int)g_items.size()) std::swap(g_items[idx], g_items[idx + 1]);
            Commit();
            break;
        case ID_PICK:
            PickAndAdd();
            break;
        case ID_PASTE:
            PastePath();
            break;
    }
}

static void OnTick() {
    if (g_modal || !g_hwnd) return;

    bool hide = false;
    if (g_cfg.hideFs) {
        HWND fg = GetForegroundWindow();
        if (fg && fg != g_hwnd) {
            wchar_t cls[64] = {};
            GetClassNameW(fg, cls, ARRAYSIZE(cls));
            if (wcscmp(cls, L"Progman") && wcscmp(cls, L"WorkerW") &&
                wcscmp(cls, L"Shell_TrayWnd") && wcscmp(cls, L"Shell_SecondaryTrayWnd")) {
                HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
                if (mon == PrimaryMon()) {
                    RECT r;
                    GetWindowRect(fg, &r);
                    MONITORINFO mi{};
                    mi.cbSize = sizeof(mi);
                    GetMonitorInfoW(mon, &mi);
                    hide = r.left <= mi.rcMonitor.left && r.top <= mi.rcMonitor.top &&
                           r.right >= mi.rcMonitor.right && r.bottom >= mi.rcMonitor.bottom;
                }
            }
        }
    }

    bool vis = IsWindowVisible(g_hwnd);
    if (hide && vis) ShowWindow(g_hwnd, SW_HIDE);
    if (!hide && !vis) ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    if (hide) return;

    if (!g_cfg.autoHide) return;

    POINT pt;
    GetCursorPos(&pt);
    RECT wr;
    GetWindowRect(g_hwnd, &wr);
    if (PtInRect(&wr, pt)) {
        g_outsideTicks = 0;
        if (!g_expanded) {
            g_expanded = true;
            Relayout();
        }
    } else if (g_expanded && ++g_outsideTicks >= 6) {
        g_expanded = false;
        g_hover = -1;
        Relayout();
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            g_hwnd = hwnd;
            g_tip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                    WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, CW_USEDEFAULT,
                                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, nullptr,
                                    GetModuleHandleW(nullptr), nullptr);
            SetTimer(hwnd, kTickId, 100, nullptr);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            int hit = HitTest(pt);
            if (hit != g_hover) {
                g_hover = hit;
                StartAnim();
            }
            if (!g_tracking) {
                TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&t);
                g_tracking = true;
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_tracking = false;
            g_hover = -1;
            StartAnim();
            return 0;
        case WM_LBUTTONUP: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            int hit = HitTest(pt);
            if (hit >= 0) {
                if (hit < (int)g_items.size())
                    Launch(g_items[hit].path, false);
                else
                    PickAndAdd();
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            int hit = HitTest(pt);
            if (hit >= 0) {
                ClientToScreen(hwnd, &pt);
                ShowMenu(pt, hit);
            }
            return 0;
        }
        case WM_DROPFILES: {
            HDROP hd = (HDROP)wp;
            UINT n = DragQueryFileW(hd, 0xFFFFFFFF, nullptr, 0);
            bool changed = false;
            for (UINT i = 0; i < n; i++) {
                wchar_t buf[MAX_PATH * 2];
                if (DragQueryFileW(hd, i, buf, ARRAYSIZE(buf))) changed |= AddPath(buf);
            }
            DragFinish(hd);
            if (changed) Commit();
            return 0;
        }
        case WM_TIMER:
            if (wp == kTickId) OnTick();
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            Relayout();
            return 0;
        case WM_RELOAD:
            LoadSettings();
            ReloadIcons();
            g_expanded = true;
            FreeDib();
            Relayout();
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kTickId);
            g_animRunning = false;
            g_tip = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI UiThread(LPVOID) {
    MSG queueInit;
    PeekMessageW(&queueInit, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    if (g_stop) return 0;

    using SetDpiCtx_t = HANDLE(WINAPI*)(HANDLE);
    auto setDpiCtx = reinterpret_cast<SetDpiCtx_t>(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"));
    if (setDpiCtx) setDpiCtx((HANDLE)(INT_PTR)-4);

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (!GpInit()) {
        Wh_Log(L"Quick Launch Dock: GDI+ init failed");
        CoUninitialize();
        return 0;
    }

    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);

    HDC dc = GetDC(nullptr);
    g_dpi = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(nullptr, dc);

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClass;
    RegisterClassW(&wc);

    LoadSettings();
    LoadItems();
    g_curW = g_curH = 0;

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_ACCEPTFILES, kClass,
        L"QuickLaunchDock", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, hInst, nullptr);
    if (hwnd) {
        Relayout();
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        if (g_stop) DestroyWindow(hwnd);

        MSG msg;
        bool quit = false;
        while (!quit) {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    quit = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (quit) break;
            if (g_animRunning) {
                if (FAILED(DwmFlush())) Sleep(8);
                AnimStep();
            } else {
                WaitMessage();
            }
        }
        if (IsWindow(hwnd)) DestroyWindow(hwnd);
    }

    for (auto& it : g_items) FreeImg(it);
    g_items.clear();
    FreeDib();
    g_hwnd = nullptr;
    UnregisterClassW(kClass, hInst);
    if (Gp.ok && Gp.Shutdown) Gp.Shutdown(Gp.token);
    Gp.ok = false;
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_stop = false;
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    HWND h = g_hwnd;
    if (h) PostMessageW(h, WM_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    g_stop = true;
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
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
