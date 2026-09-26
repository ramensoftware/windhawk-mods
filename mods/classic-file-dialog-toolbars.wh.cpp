// ==WindhawkMod==
// @id              classic-file-dialog-toolbars
// @name            File Dialog Toolbars and Classic Folder Tree
// @description     Brings the navigation bar - back and forward buttons, address bar and search box - back to the Open, Save As and folder picker dialogs, and gives their folder tree the look of the classic Explorer tree
// @name:ru         Панель навигации и классическое дерево папок в окнах выбора файла
// @description:ru  Возвращает в окна «Открыть», «Сохранить как» и выбора папки панель навигации - кнопки «Назад» и «Вперёд», адресную строку и строку поиска - и придаёт их дереву папок вид классического дерева Проводника
// @version         1.0
// @author          appEW
// @github          https://github.com/appEW
// @include         *
// @compilerOptions -lcomctl32 -lgdi32 -lole32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- RestoreNavigationBar: true
  $name: Restore the navigation bar
  $name:ru: Вернуть панель навигации
  $description: >-
    Brings back the toolbars, the address bar and the search box at the top of
    the file and folder picker dialogs.
  $description:ru: >-
    Возвращает тулбары, адресную строку и строку поиска в верхней части диалогов
    выбора файла и папки.
- CompactBands: false
  $name: Explorer band height
  $name:ru: Высота полос как в проводнике
  $description: >-
    Make the bands of the navigation bar as high as the address bar of Explorer
    is, instead of the much taller ones the dialog uses by default.
  $description:ru: >-
    Делает полосы панели навигации такой же высоты, как адресная строка в
    проводнике, вместо заметно более высоких полос диалога по умолчанию.
- BandSeparators: true
  $name: Separators around the toolbars
  $name:ru: Разделители вокруг тулбаров
  $description: >-
    Draw the borders around the bands that the Separators around File Explorer
    toolbars mod draws in Explorer.
  $description:ru: >-
    Рисует вокруг полос те же рамки, что мод Separators around File Explorer
    toolbars рисует в проводнике.
- ClassicTree: true
  $name: Classic folder tree
  $name:ru: Классическое дерево папок
  $description: >-
    Gives the folder tree of the dialog the classic look: dotted lines, +/-
    buttons, classic indent and item height, no pin icons and no hand cursor.
  $description:ru: >-
    Придаёт дереву папок в диалоге классический вид: пунктирные линии, кнопки
    +/-, классический отступ и высота строки, без значков-булавок и без курсора-руки.
- DrawLines: true
  $name: Draw dotted lines
  $name:ru: Пунктирные линии
  $description: Use the TVS_HASLINES style. Disable for a more XP-like look.
  $description:ru: Стиль TVS_HASLINES. Отключите для вида в стиле XP.
- LinesAtRoot: false
  $name: Lines at root
  $name:ru: Линии у корневых элементов
  $description: Use the TVS_LINESATROOT style.
  $description:ru: Стиль TVS_LINESATROOT.
- HotTracking: false
  $name: Item hot tracking
  $name:ru: Подсветка элемента под курсором
  $description: Use the TVS_TRACKSELECT style. Enable for a more XP-like look.
  $description:ru: Стиль TVS_TRACKSELECT. Включите для вида в стиле XP.
- DrawButtons: true
  $name: Draw +/- buttons
  $name:ru: Рисовать кнопки +/-
  $description: >-
    Draw the classic +/- buttons. Disable only if you are not using the Classic
    theme.
  $description:ru: >-
    Рисовать классические кнопки +/-. Отключайте только если не используется
    классическая тема.
- AlternateLineColor: Automatic
  $name: Alternate line color
  $name:ru: Альтернативный цвет линий
  $description: >-
    Use the highlight color instead of the shadow color for the dotted lines.
    May produce better results on some dark themes.
  $description:ru: >-
    Использовать цвет подсветки вместо цвета тени для пунктирных линий. Может
    выглядеть лучше на тёмных темах.
  $options:
    - False: Use the default color.
    - True: Use the alternate color.
    - Automatic: Use the alternate color if the contrast of the default one is too low.
  $options:ru:
    - False: Обычный цвет.
    - True: Альтернативный цвет.
    - Automatic: Альтернативный цвет, если обычный недостаточно контрастен.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# File Dialog Toolbars and Classic Folder Tree

The Open, Save As and folder picker dialogs that every program shows are built
from the same parts as Explorer, but on a classic theme setup they often lose
their whole top bar and keep a Windows 11 folder tree. This mod brings back the
navigation bar - back, forward and up buttons, the address bar and the search
box - and gives the folder tree of the dialog the look of the classic Explorer
tree: dotted lines, +/- buttons, the classic indent, no pin icons.

Each part can be turned on and off in the settings. Explorer windows and the
folder trees of other programs are left alone.

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

The common file and folder picker dialog - the one every program opens for
"Open..." and "Save as..." - is the same shell browser that Explorer uses, but
on a classic theme setup it usually ends up looking like a bare list with a
places pane: no back and forward buttons, no address bar, no search box, and a
folder tree that still looks like Windows 11 with its pin icons, its wide
indent and no expand buttons at all.

This mod fixes both halves of that.

## The navigation bar

The navigation bar of the dialog - the travel band with the back, forward and
up buttons, the breadcrumb address bar with its drop-downs and refresh button,
and the search box - is one COM object, `CLSID_NavBar`
(`{056440FD-8568-48e7-A632-72157243B55B}`, registered as *Explorer Navigation
Bar* by `explorerframe.dll`). `comdlg32.dll` asks for it with
`CoCreateInstance` when it builds the dialog.

A very common classic-theme tweak is to shadow that class in
`HKEY_CURRENT_USER\Software\Classes\CLSID\{056440FD-...}\InprocServer32` with an
empty value, so the class can no longer be created. Explorer itself is not
affected, because it creates its own navigation bar internally instead of going
through COM - but the dialog is, and it silently loses its whole top bar while
still reserving the space for it.

The mod hooks `CoCreateInstance` and, when a request for `CLSID_NavBar` fails,
creates the object straight from `explorerframe.dll` through its
`DllGetClassObject` export, so the dialog gets its navigation bar back without
any registry change, and without changing what Explorer or anything else does.

Nothing else is touched: a request that succeeds on its own is passed through
untouched, so is every other class, and so is a failing request that does not
come from `comdlg32.dll` - the caller is checked, so the only thing that can
ever be handed the object it could not create before is the dialog itself.

## The bar itself

The bar the dialog builds is a plain rebar (`ReBarWindow32` in a `WorkerW`),
the same kind of bar the toolbars of Explorer live on. Its bands are left where
the dialog puts them - fixed, with no grippers, exactly as the dialog intends -
and only two things about it are touched, each behind a setting:

* **Separators around the toolbars** puts `RBS_BANDBORDERS` and `WS_BORDER` on
  the rebar, which is what the *Separators around File Explorer toolbars* mod
  does for Explorer - that one only runs in `explorer.exe`.
* **Explorer band height** shortens the bands to the height of the address bar
  of Explorer. It is off by default, because the round back and forward buttons
  of the dialog are drawn for a much taller band: shorten the bar and they are
  cut in half. It also keeps the search box filling its band, which it does not
  do on its own once the band is shorter.

## The folder tree

The tree in the dialog is a `CNscTree` (`NamespaceTreeControl`), the same
control that the folder pane of Explorer is built from, so it is made classic
the same way the
[Classic Explorer Treeview](https://windhawk.net/mods/classic-explorer-treeview)
mod by Waldemar (CyprinusCarpio) makes the Explorer one classic - that mod only
touches the trees of Explorer windows (`ShellTabWindowClass`) and this one only
the trees of dialogs (`#32770`), so the two never meet over the same tree, even
inside Explorer, which shows a dialog of its own now and then - the folder
picker of `Run...` -> `Browse...`, for one.

A tree is taken to be the tree of the dialog when it sits in the DirectUI host
of a dialog window (`#32770` > `DUIViewWndClassName` > `DirectUIHWND` >
`CtrlNotifySink` > `NamespaceTreeControl`), which is how the common dialog, and
only the common dialog, builds it. A namespace tree a program hosts on its own
is left alone - it may be a tree with checkboxes, and those are drawn from the
very state image list the pin icons are.

For every namespace tree of a dialog the mod:

* sets `TVS_HASBUTTONS`, optionally `TVS_HASLINES` and `TVS_LINESATROOT`, and
  drops `TVS_TRACKSELECT`,
* sets the classic indent and a 16 pixel item height, and keeps them there,
  because the control resets both while it is filled,
* drops the state image list, which is what the pin icons are drawn from,
* replaces the rich tooltip with the classic one and the hand cursor with the
  arrow,
* draws the +/- buttons itself, since the control paints its own expandos and
  they are gone once the state image list is.

The drawing of the buttons and the line color logic are taken from Classic
Explorer Treeview, so both trees look exactly the same.

The folder band header ("Folders" with a close button) that the Explorer mod
draws is deliberately not added here - the dialog has no browser bar to close.

## Notes

* The mod is loaded everywhere, but it only ever does something for a
  navigation bar that `comdlg32.dll` asked for, and for a namespace tree that
  sits in the DirectUI host of a dialog. Everything else is left exactly as it
  was.
* If a program shows the old Windows 3.1 style file dialog instead - for
  example with the `classic-file-picker-dialog` mod enabled - there is no
  navigation bar and no namespace tree in it, and the mod leaves it alone.

---

## По-русски

Окна «Открыть», «Сохранить как» и выбора папки, которые показывают все
программы, собраны из тех же частей, что и Проводник, но при классической теме
у них часто пропадает вся верхняя панель, а дерево папок остаётся в стиле
Windows 11. Мод возвращает панель навигации - кнопки «Назад», «Вперёд» и
«Вверх», адресную строку и строку поиска - и придаёт дереву папок в этих окнах
вид классического дерева Проводника: пунктирные линии, кнопки +/-,
классический отступ, без значков-булавок.

Каждую часть можно включить или выключить в настройках. Окна самого Проводника
и деревья папок в других программах мод не трогает.

Панель навигации пропадает из-за распространённой настройки классической темы,
которая отключает в реестре COM-класс этой панели (`CLSID_NavBar`). Мод не
меняет реестр: он создаёт панель напрямую из `explorerframe.dll`, и только
когда её запрашивает сам диалог (`comdlg32.dll`). Дерево папок оформляется так
же, как это делает мод
[Classic Explorer Treeview](https://windhawk.net/mods/classic-explorer-treeview)
для Проводника.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <shobjidl.h>
#include <unknwn.h>

#include <cmath>
#include <mutex>
#include <vector>

#ifdef CFDT_DEBUG_LOG
#include <cstdarg>
#include <cstdio>
static void DbgLog(const char* format, ...) {
    FILE* file = fopen("C:/Users/appEW/AppData/Local/Temp/cfdt.log", "a");
    if (!file) {
        return;
    }
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
    fputc('\n', file);
    fclose(file);
}
#else
#define DbgLog(...) ((void)0)
#endif

#ifdef _WIN64
#define THISCALL  __cdecl
#define STHISCALL L"__cdecl"
#else
#define THISCALL  __thiscall
#define STHISCALL L"__thiscall"
#endif

////////////////////////////////////////////////////////////////////////////////
// Subclasses, and getting rid of every one of them when the mod is unloaded.
//
// A subclass left behind points into a DLL that is no longer there, and the
// window it sits on faults on its next message - which is what an open dialog
// does the moment the mod is disabled, reloaded, or its settings are changed.

struct Subclass {
    HWND hWnd;
    WindhawkUtils::WH_SUBCLASSPROC proc;
};

std::mutex g_subclassesMutex;
std::vector<Subclass> g_subclasses;

bool AddSubclass(HWND hWnd, WindhawkUtils::WH_SUBCLASSPROC proc, DWORD_PTR data) {
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, proc, data)) {
        return false;
    }

    std::lock_guard<std::mutex> guard(g_subclassesMutex);
    g_subclasses.push_back({hWnd, proc});
    return true;
}

void DropSubclass(HWND hWnd, WindhawkUtils::WH_SUBCLASSPROC proc) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, proc);

    std::lock_guard<std::mutex> guard(g_subclassesMutex);
    std::erase_if(g_subclasses, [hWnd, proc](const Subclass& subclass) {
        return subclass.hWnd == hWnd && subclass.proc == proc;
    });
}

void DropAllSubclasses() {
    std::vector<Subclass> subclasses;
    {
        std::lock_guard<std::mutex> guard(g_subclassesMutex);
        subclasses.swap(g_subclasses);
    }

    for (const Subclass& subclass : subclasses) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(subclass.hWnd, subclass.proc);
    }
}

// Settings.
bool g_settingRestoreNavBar = true;
bool g_settingCompactBands = false;
bool g_settingBandSeparators = true;
bool g_settingClassicTree = true;
bool g_settingDrawLines = true;
bool g_settingLinesAtRoot = false;
bool g_settingHotTracking = false;
bool g_settingDrawButtons = true;
int  g_settingLineColorOption = 2;  // 0 - default, 1 - alternate, 2 - automatic

// "Explorer Navigation Bar", the whole top bar of the dialog.
static const CLSID CLSID_NavBar_ = {
    0x056440FD, 0x8568, 0x48e7, {0xA6, 0x32, 0x72, 0x15, 0x72, 0x43, 0xB5, 0x5B}};
static const IID IID_IClassFactory_ = {
    0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};


bool IsClassName(HWND hWnd, PCWSTR name) {
    WCHAR className[64];
    if (!hWnd || !GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return wcscmp(className, name) == 0;
}

////////////////////////////////////////////////////////////////////////////////
// The navigation bar.

HRESULT CreateNavBarFromExplorerFrame(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppv) {
    static HMODULE hExplorerFrame =
        LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame) {
        return REGDB_E_CLASSNOTREG;
    }

    using DllGetClassObject_t = HRESULT(WINAPI*)(REFCLSID, REFIID, void**);
    static auto pDllGetClassObject = (DllGetClassObject_t)GetProcAddress(
        hExplorerFrame, "DllGetClassObject");
    if (!pDllGetClassObject) {
        return REGDB_E_CLASSNOTREG;
    }

    IClassFactory* pClassFactory = nullptr;
    HRESULT hr = pDllGetClassObject(CLSID_NavBar_, IID_IClassFactory_,
                                    (void**)&pClassFactory);
    if (FAILED(hr) || !pClassFactory) {
        return FAILED(hr) ? hr : E_NOINTERFACE;
    }

    hr = pClassFactory->CreateInstance(pUnkOuter, riid, ppv);
    pClassFactory->Release();
    return hr;
}

// The navigation bar is only ever handed back to the common dialog, so that a
// process that asks for it for any other reason - Explorer, which builds the
// navigation bar of its own windows without going through COM, above all -
// keeps behaving exactly as it did before.
bool CallerIsCommonDialog(void* pReturnAddress) {
    HMODULE hCallerModule = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (PCWSTR)pReturnAddress, &hCallerModule)) {
        return false;
    }

    return hCallerModule == GetModuleHandleW(L"comdlg32.dll");
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
CoCreateInstance_t CoCreateInstance_orig;
HRESULT WINAPI CoCreateInstance_hook(REFCLSID rclsid,
                                     LPUNKNOWN pUnkOuter,
                                     DWORD dwClsContext,
                                     REFIID riid,
                                     LPVOID* ppv) {
    void* pReturnAddress = __builtin_return_address(0);

    HRESULT hr = CoCreateInstance_orig(rclsid, pUnkOuter, dwClsContext, riid, ppv);

    if (FAILED(hr) && g_settingRestoreNavBar && IsEqualCLSID(rclsid, CLSID_NavBar_) &&
        CallerIsCommonDialog(pReturnAddress)) {
        HRESULT hrFallback = CreateNavBarFromExplorerFrame(pUnkOuter, riid, ppv);
        Wh_Log(L"CLSID_NavBar: 0x%08X from the registry, 0x%08X from explorerframe.dll",
               (unsigned int)hr, (unsigned int)hrFallback);
        if (SUCCEEDED(hrFallback)) {
            hr = hrFallback;
        }
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// The height of the bands.

int GetDesiredBandHeight(HWND hDialog) {
    // The file name box of the dialog is a combo box with the same font as the
    // address bar of an Explorer window, and comes out at the same height, so
    // taking it as the band height is what lines the two windows up.
    HWND hComboBox = FindWindowEx(hDialog, nullptr, L"ComboBoxEx32", nullptr);
    if (hComboBox) {
        RECT rect;
        if (GetWindowRect(hComboBox, &rect)) {
            int height = rect.bottom - rect.top;
            if (height >= 16 && height <= 64) {
                return height;
            }
        }
    }

    return GetSystemMetrics(SM_CYSIZE) + 2 * GetSystemMetrics(SM_CYBORDER) + 2;
}

// The search box is inset inside its band and has to be told to fill it, the
// same way it has to be in an Explorer window.
void SyncSearchBoxToBand(HWND hRebar) {
    HWND hSearchBand = FindWindowEx(hRebar, nullptr, L"UniversalSearchBand", nullptr);
    if (!hSearchBand) {
        return;
    }

    HWND hSearchBox = FindWindowEx(hSearchBand, nullptr, L"Search Box", nullptr);
    if (!hSearchBox) {
        return;
    }

    RECT band;
    GetClientRect(hSearchBand, &band);

    RECT box;
    GetWindowRect(hSearchBox, &box);
    if (box.right - box.left == band.right && box.bottom - box.top == band.bottom) {
        return;
    }

    SetWindowPos(hSearchBox, nullptr, 0, 0, band.right, band.bottom,
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

// The address band insets its content by eight pixels top and bottom, whatever
// its own height is, so at the height of an Explorer band there is nothing left
// for the breadcrumbs to be drawn in. Everything in it is stretched back over
// the whole band instead.
void FillAddressBand(HWND hAddressBand) {
    HWND hProgress = FindWindowEx(hAddressBand, nullptr, L"msctls_progress32", nullptr);
    if (!hProgress) {
        return;
    }

    RECT band;
    GetClientRect(hAddressBand, &band);
    SetWindowPos(hProgress, nullptr, 0, 0, band.right, band.bottom,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    // The breadcrumb host and the refresh button sit in the progress bar, the
    // breadcrumb toolbar sits in the host, and none of them is laid out again
    // when their parent is resized. Only the height is taken over, so that the
    // columns the address band computed stay where they are.
    HWND hHosts[] = {hProgress, FindWindowEx(hProgress, nullptr, L"Breadcrumb Parent",
                                             nullptr)};
    for (HWND hHost : hHosts) {
        if (!hHost) {
            continue;
        }

        RECT host;
        GetClientRect(hHost, &host);

        for (HWND hChild = GetWindow(hHost, GW_CHILD); hChild;
             hChild = GetWindow(hChild, GW_HWNDNEXT)) {
            RECT child;
            GetWindowRect(hChild, &child);

            POINT topLeft = {child.left, child.top};
            ScreenToClient(hHost, &topLeft);

            SetWindowPos(hChild, nullptr, topLeft.x, 0, child.right - child.left,
                         host.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}

LRESULT CALLBACK AddressBandSubclassProc(HWND hWnd,
                                         UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY) {
        DropSubclass(hWnd, AddressBandSubclassProc);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (g_settingCompactBands && (uMsg == WM_SIZE || uMsg == WM_WINDOWPOSCHANGED)) {
        static thread_local bool filling = false;
        if (!filling) {
            filling = true;
            FillAddressBand(hWnd);
            filling = false;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK NavBarRebarSubclassProc(HWND hWnd,
                                         UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         DWORD_PTR dwRefData) {
    bool bandMessage = false;

    switch (uMsg) {
        case RB_INSERTBANDA:
        case RB_INSERTBANDW:
        case RB_SETBANDINFOA:
        case RB_SETBANDINFOW: {
            bandMessage = true;

            // The A and W flavours of the struct only differ in the type the
            // text pointer has, so the fields below sit at the same offset.
            auto* prbbi = (REBARBANDINFOW*)lParam;
            if (g_settingCompactBands && prbbi &&
                prbbi->cbSize >= offsetof(REBARBANDINFOW, cyIntegral) +
                                     sizeof(prbbi->cyIntegral)) {
                int height = GetDesiredBandHeight(GetAncestor(hWnd, GA_ROOT));

                prbbi->fMask |= RBBIM_CHILDSIZE;
                prbbi->cyMinChild = height;
                prbbi->cyChild = height;
                prbbi->cyMaxChild = height;
                prbbi->cyIntegral = 0;
            }
            break;
        }

        case WM_SIZE:
            bandMessage = true;
            break;

        case WM_NCDESTROY:
            DropSubclass(hWnd, NavBarRebarSubclassProc);
            break;
    }

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (bandMessage && g_settingCompactBands) {
        SyncSearchBoxToBand(hWnd);

        // The address band lays itself out again on every navigation, so it
        // gets a subclass of its own rather than being corrected from here.
        if (HWND hAddressBand =
                FindWindowEx(hWnd, nullptr, L"Address Band Root", nullptr)) {
            AddSubclass(hAddressBand, AddressBandSubclassProc, 0);
            FillAddressBand(hAddressBand);
        }
    }

    return result;
}

// The rebar of the navigation bar of a dialog: #32770 > WorkerW > ReBarWindow32.
bool IsNavBarRebar(HWND hRebar, HWND hParent) {
    return IsClassName(hRebar, L"ReBarWindow32") && IsClassName(hParent, L"WorkerW") &&
           IsClassName(GetParent(hParent), L"#32770");
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle,
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
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
                                     nWidth, nHeight, hWndParent, hMenu, hInstance,
                                     lpParam);

    if (hWnd && hWndParent &&
        (g_settingCompactBands || g_settingBandSeparators) &&
        (IS_INTRESOURCE(lpClassName) || wcscmp(lpClassName, L"ReBarWindow32") == 0) &&
        IsNavBarRebar(hWnd, hWndParent)) {
        AddSubclass(hWnd, NavBarRebarSubclassProc, 0);

        if (g_settingBandSeparators) {
            // The same border the Separators around File Explorer toolbars mod
            // puts on the rebar of Explorer.
            SetWindowLongPtr(hWnd, GWL_STYLE,
                             GetWindowLongPtr(hWnd, GWL_STYLE) | RBS_BANDBORDERS |
                                 WS_BORDER);
        }
    }

    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// The folder tree.

// Both of these are taken from the Classic Explorer Treeview mod by Waldemar
// (CyprinusCarpio), so that the tree of the dialog is drawn exactly like the
// folder pane of Explorer.

double ColorDistance(DWORD color1, DWORD color2) {
    int b1 = (color1 & 0xFF);
    int g1 = ((color1 >> 8) & 0xFF);
    int r1 = ((color1 >> 16) & 0xFF);
    int b2 = (color2 & 0xFF);
    int g2 = ((color2 >> 8) & 0xFF);
    int r2 = ((color2 >> 16) & 0xFF);

    return std::sqrt(std::pow(r2 - r1, 2) + std::pow(g2 - g1, 2) + std::pow(b2 - b1, 2));
}

bool UseAlternateLineColor() {
    if (g_settingLineColorOption == 1) {
        return true;
    }
    if (g_settingLineColorOption != 2) {
        return false;
    }

    DWORD windowColor = GetSysColor(COLOR_WINDOW);
    DWORD shadowColor = GetSysColor(COLOR_3DSHADOW);
    DWORD highlightColor = GetSysColor(COLOR_3DHIGHLIGHT);

    return ColorDistance(windowColor, highlightColor) >
           ColorDistance(windowColor, shadowColor);
}

void ApplyLineColor(HWND hTreeview) {
    if (g_settingLineColorOption == 0) {
        return;
    }

    TreeView_SetLineColor(hTreeview, UseAlternateLineColor()
                                         ? GetSysColor(COLOR_3DHIGHLIGHT)
                                         : GetSysColor(COLOR_3DSHADOW));
}

void DrawExpandoButton(HWND hTreeview, HDC hdc, HTREEITEM hItem) {
    RECT rect;
    if (!TreeView_GetItemRect(hTreeview, hItem, &rect, TRUE)) {
        return;
    }

    DWORD windowColor = GetSysColor(COLOR_WINDOW);
    bool useHighlight = UseAlternateLineColor();

    // The item height should be 16, a couple of other heights are accommodated
    // as well.
    switch (TreeView_GetItemHeight(hTreeview)) {
        default:
        case 16:
            rect.left -= 34;
            rect.top += 4;
            break;
        case 18:
            rect.left -= 33;
            rect.top += 6;
            break;
    }

    TVITEM tvi;
    tvi.mask = TVIF_CHILDREN;
    tvi.hItem = hItem;

    bool isTopLevel = rect.left <= (g_settingLinesAtRoot ? 10 : -10);

    if ((isTopLevel && !g_settingLinesAtRoot) || !TreeView_GetItem(hTreeview, &tvi) ||
        tvi.cChildren <= 0) {
        return;
    }

    UINT state = TreeView_GetItemState(hTreeview, hItem, TVIS_EXPANDED);

    HBRUSH brush = CreateSolidBrush(windowColor);
    HBRUSH original = (HBRUSH)SelectObject(hdc, brush);

    RECT buttonRect;
    buttonRect.left = rect.left;
    buttonRect.top = rect.top;
    buttonRect.right = rect.left + 9;
    buttonRect.bottom = rect.top + 9;

    FillRect(hdc, &buttonRect, brush);
    DeleteObject(brush);

    brush = CreateSolidBrush(useHighlight ? GetSysColor(COLOR_3DHIGHLIGHT)
                                          : GetSysColor(COLOR_3DSHADOW));
    FrameRect(hdc, &buttonRect, brush);
    DeleteObject(brush);

    SelectObject(hdc, original);

    HPEN hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, rect.left + 2, rect.top + 4, nullptr);
    LineTo(hdc, rect.left + 7, rect.top + 4);
    if (!(state & TVIS_EXPANDED)) {
        MoveToEx(hdc, rect.left + 4, rect.top + 2, nullptr);
        LineTo(hdc, rect.left + 4, rect.top + 7);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

LRESULT CALLBACK TreeviewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_SETCURSOR:
            // No hand cursor over the items.
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return TRUE;

        case TV_FIRST + 74:  // TVM_SETTOPMARGIN
            wParam = 0;
            break;

        case TVM_SETINDENT:
            wParam = 19;
            break;

        case TVM_SETITEMHEIGHT:
            wParam = 16;
            break;

        case TVM_SETIMAGELIST:
            // The pin icons of the dialog are drawn from the state image list.
            if (wParam == TVSIL_STATE) {
                return 0;
            }
            break;

        case TVM_INSERTITEM:
            // Every item has to have iIntegral set to 1 for the item height to
            // stay accurate.
            if (auto* lpis = (LPTVINSERTSTRUCT)lParam) {
                lpis->itemex.iIntegral = 1;
            }
            break;

        case TVM_SETITEM:
            if (auto* lptvi = (LPTVITEMEXW)lParam;
                lptvi && (lptvi->mask & TVIF_INTEGRAL)) {
                lptvi->iIntegral = 1;
            }
            break;

        case WM_SYSCOLORCHANGE:
            ApplyLineColor(hWnd);
            break;

        case WM_NCDESTROY:
            DropSubclass(hWnd, TreeviewSubclassProc);
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NamespaceTreeSubclassProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY) {
        DropSubclass(hWnd, NamespaceTreeSubclassProc);
    } else if (uMsg == WM_NOTIFY && g_settingDrawButtons) {
        auto* lpnmh = (LPNMHDR)lParam;
        switch (lpnmh->code) {
            case NM_CUSTOMDRAW: {
                HWND hTreeview = FindWindowEx(hWnd, nullptr, L"SysTreeView32", nullptr);
                if (!hTreeview) {
                    break;
                }

                auto* pCustomDraw = (LPNMTVCUSTOMDRAW)lParam;
                switch (pCustomDraw->nmcd.dwDrawStage) {
                    case CDDS_PREPAINT:
                        return CDRF_NOTIFYITEMDRAW;
                    case CDDS_ITEMPREPAINT:
                        return CDRF_NOTIFYPOSTPAINT;
                    case CDDS_ITEMPOSTPAINT:
                        DrawExpandoButton(hTreeview, pCustomDraw->nmcd.hdc,
                                          (HTREEITEM)pCustomDraw->nmcd.dwItemSpec);
                        return CDRF_DODEFAULT;
                }
                break;
            }

            case TVN_ITEMEXPANDED:
                // Redraw the whole tree, the buttons of the items that moved
                // are drawn over the old ones otherwise.
                InvalidateRect(lpnmh->hwndFrom, nullptr, TRUE);
                return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// The tree of the common dialog sits in the DirectUI host of a dialog window:
//
//     #32770 > DUIViewWndClassName > DirectUIHWND > CtrlNotifySink >
//     NamespaceTreeControl
//
// A namespace tree that a program hosts on its own looks nothing like that, and
// is left alone - it may well be a tree with checkboxes, which are drawn from
// the very state image list the pin icons are.
bool IsCommonDialogTree(HWND hNamespaceTree) {
    HWND hDirectUIHost = GetParent(GetParent(GetParent(hNamespaceTree)));

    return IsClassName(hDirectUIHost, L"DUIViewWndClassName") &&
           IsClassName(GetParent(hDirectUIHost), L"#32770");
}

void MakeTreeClassic(HWND hNamespaceTree, HWND hTreeview) {
    if (!AddSubclass(hNamespaceTree, NamespaceTreeSubclassProc, 0)) {
        return;
    }

    AddSubclass(hTreeview, TreeviewSubclassProc, 0);

    // Whatever was set before the subclass was in place has to go as well.
    TreeView_SetImageList(hTreeview, nullptr, TVSIL_STATE);

    DWORD style = TVS_HASBUTTONS | TVS_EDITLABELS | TVS_SHOWSELALWAYS | WS_TABSTOP |
                  WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD;
    if (g_settingLinesAtRoot) {
        style |= TVS_LINESATROOT;
    }
    if (g_settingDrawLines) {
        style |= TVS_HASLINES;
    }
    if (g_settingHotTracking) {
        style |= TVS_TRACKSELECT;
    }

    SetWindowLongPtr(hTreeview, GWL_STYLE, style);

    TreeView_SetIndent(hTreeview, 20);
    TreeView_SetItemHeight(hTreeview, 16);
    ApplyLineColor(hTreeview);

    // Bring back the classic tooltip.
    TreeView_SetExtendedStyle(hTreeview, TreeView_GetExtendedStyle(hTreeview) &
                                             ~TVS_EX_RICHTOOLTIP,
                              TVS_EX_RICHTOOLTIP);
}

HWND(THISCALL* CNscTree__CreateTreeview_orig)(void*, HWND);
HWND THISCALL CNscTree__CreateTreeview_hook(void* pThis, HWND hWnd) {
    HWND hTreeview = CNscTree__CreateTreeview_orig(pThis, hWnd);

    if (g_settingClassicTree && hTreeview && IsCommonDialogTree(hWnd)) {
        MakeTreeClassic(hWnd, hTreeview);
    }

    return hTreeview;
}

////////////////////////////////////////////////////////////////////////////////

void LoadSettings() {
    g_settingRestoreNavBar = Wh_GetIntSetting(L"RestoreNavigationBar");
    g_settingCompactBands = Wh_GetIntSetting(L"CompactBands");
    g_settingBandSeparators = Wh_GetIntSetting(L"BandSeparators");
    g_settingClassicTree = Wh_GetIntSetting(L"ClassicTree");
    g_settingDrawLines = Wh_GetIntSetting(L"DrawLines");
    g_settingLinesAtRoot = Wh_GetIntSetting(L"LinesAtRoot");
    g_settingHotTracking = Wh_GetIntSetting(L"HotTracking");
    g_settingDrawButtons = Wh_GetIntSetting(L"DrawButtons");

    PCWSTR lineColor = Wh_GetStringSetting(L"AlternateLineColor");
    g_settingLineColorOption = 2;
    if (wcscmp(lineColor, L"False") == 0) {
        g_settingLineColorOption = 0;
    } else if (wcscmp(lineColor, L"True") == 0) {
        g_settingLineColorOption = 1;
    }
    Wh_FreeStringSetting(lineColor);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hCom = LoadLibraryW(L"api-ms-win-core-com-l1-1-0.dll");
    void* pCoCreateInstance =
        hCom ? (void*)GetProcAddress(hCom, "CoCreateInstance") : nullptr;
    if (!pCoCreateInstance) {
        pCoCreateInstance = (void*)CoCreateInstance;
    }

    if (!Wh_SetFunctionHook(pCoCreateInstance, (void*)CoCreateInstance_hook,
                            (void**)&CoCreateInstance_orig)) {
        Wh_Log(L"Failed to hook CoCreateInstance");
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_hook,
                            (void**)&CreateWindowExW_orig)) {
        Wh_Log(L"Failed to hook CreateWindowExW, the bands keep their height");
    }

    HMODULE hExplorerFrame =
        LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame) {
        Wh_Log(L"Failed to load explorerframe.dll, the tree is left alone");
        return TRUE;
    }

    // explorerframe.dll
    WindhawkUtils::SYMBOL_HOOK explorerFrameHooks[] = {
        {
            {L"private: struct HWND__ * " STHISCALL
             L" CNscTree::_CreateTreeview(struct HWND__ *)"},
            &CNscTree__CreateTreeview_orig,
            CNscTree__CreateTreeview_hook,
            false,
        },
    };

    // The navigation bar is the important half, so a tree that cannot be
    // hooked is not a reason to fail.
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerFrameHooks,
                                    ARRAYSIZE(explorerFrameHooks))) {
        Wh_Log(L"Failed to hook explorerframe.dll, the tree is left alone");
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Any window still carrying a subclass of ours would call into a DLL that
    // is about to be gone.
    DropAllSubclasses();
}
