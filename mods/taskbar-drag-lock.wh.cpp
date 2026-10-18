// ==WindhawkMod==
// @id              taskbar-drag-lock
// @name            Windows 11 Taskbar Drag and Lock
// @description     Drag the taskbar to any screen edge and lock it from the taskbar context menu
// @version         1.0.0
// @author          yorii
// @github          https://github.com/yoriichi111012
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ladvapi32 -lshell32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Bring back the Windows 10 ability to drag the taskbar to left, top, right and bottom.

Requires the native Windows 11 taskbar position setting (Windows 11 build 26300+).

The position is changed through the same native setting as Windows Settings and
applies immediately, without restarting Explorer. Unloading the mod leaves the
chosen position in place.

Based on the taskbar-empty-space-clicks mod by m1lhaus (input handling,
GPL-3.0), the Taskbar Move mod by Asteski (native position setting), and the
Taskbar Restart Explorer mod by Mgrmjp (context menu injection).
*/
// ==/WindhawkModReadme==

#include <initguid.h>

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <winver.h>
#include <roapi.h>
#include <inspectable.h>
#include <uiautomation.h>

#undef GetCurrentTime

#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <unordered_set>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

static constexpr wchar_t kLockTaskbarText[] = L"Lock Taskbar";
static constexpr wchar_t kItemName[] = L"WindhawkTaskbarLockItem";
static constexpr wchar_t kSeparatorName[] = L"WindhawkTaskbarLockSeparator";

static constexpr wchar_t kAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

static std::atomic<bool> g_taskbarLocked = false;
static std::atomic<bool> g_taskbarViewModuleHooked = false;

static thread_local int g_taskbarSettingsMenuDepth = 0;
static thread_local bool g_currentMenuInjected = false;

bool IsWindhawkInjectedItem(wuxc::MenuFlyoutItemBase const& baseItem) {
    try {
        if (auto frameworkElement = baseItem.try_as<wux::FrameworkElement>()) {
            auto name = frameworkElement.Name();
            return std::wstring_view(name.c_str(), name.size())
                .starts_with(L"Windhawk");
        }
    } catch (...) {
    }

    return false;
}

winrt::hstring GetMenuItemText(wuxc::MenuFlyoutItemBase const& baseItem) {
    try {
        if (auto item = baseItem.try_as<wuxc::MenuFlyoutItem>()) {
            return item.Text();
        }
        if (auto subMenu = baseItem.try_as<wuxc::MenuFlyoutSubItem>()) {
            return subMenu.Text();
        }
    } catch (...) {
    }

    return {};
}

bool IsSeparator(wuxc::MenuFlyoutItemBase const& baseItem) {
    try {
        return !!baseItem.try_as<wuxc::MenuFlyoutSeparator>();
    } catch (...) {
    }

    return false;
}

// Native Settings ISettingItem ABI, verified against the Windows 11 public
// symbols. QueryInterface validates the contract before any methods are used.
struct NativeSettingItem : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Id(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Type(INT32*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsSetByGroupPolicy(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsEnabled(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsApplicable(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsUpdating(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValue(HSTRING, IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetValue(HSTRING, IInspectable*) = 0;
};
__CRT_UUID_DECL(NativeSettingItem, 0x40c037cc, 0xd8bf, 0x489e,
                0x86, 0x97, 0xd6, 0x6b, 0xaa, 0x32, 0x21, 0xbf)

static constexpr wchar_t kLocationSettingClass[] =
    L"SystemSettings.Desktop.Taskbar.DesktopTaskbarLoSetting";

// DesktopTaskbarLoSetting uses the Settings tile order, not ABE_* order.
// Verified against LocationModeFromSettingIndex on 26200.9278.
static constexpr DWORD kLocationEdges[] = {ABE_BOTTOM, ABE_TOP, ABE_LEFT,
                                           ABE_RIGHT};

winrt::com_ptr<NativeSettingItem> OpenLocationSetting() {
    winrt::hstring name{kLocationSettingClass};
    winrt::com_ptr<IInspectable> object;
    winrt::check_hresult(RoActivateInstance(
        reinterpret_cast<HSTRING>(winrt::get_abi(name)), object.put()));
    return object.as<NativeSettingItem>();
}

bool LocationSettingUsable(NativeSettingItem* setting) {
    boolean enabled = false, applicable = false, managed = true;
    return SUCCEEDED(setting->get_IsEnabled(&enabled)) && enabled &&
           SUCCEEDED(setting->get_IsApplicable(&applicable)) && applicable &&
           SUCCEEDED(setting->get_IsSetByGroupPolicy(&managed)) && !managed;
}

bool SetTaskbarLocationByEdge(DWORD edge) {
    DWORD index = 0;
    bool found = false;
    for (DWORD i = 0; i < ARRAYSIZE(kLocationEdges); i++) {
        if (kLocationEdges[i] == edge) {
            index = i;
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    try {
        auto setting = OpenLocationSetting();
        if (!LocationSettingUsable(setting.get())) {
            Wh_Log(L"Taskbar position setting is unavailable or managed");
            return false;
        }

        winrt::hstring property{L"Value"};
        auto boxed = winrt::box_value(static_cast<int32_t>(index));
        winrt::check_hresult(setting->SetValue(
            reinterpret_cast<HSTRING>(winrt::get_abi(property)),
            reinterpret_cast<IInspectable*>(winrt::get_abi(boxed))));

        boolean updating = false;
        winrt::check_hresult(setting->get_IsUpdating(&updating));
        if (updating) {
            Wh_Log(L"Taskbar position change accepted; update pending");
            return true;
        }

        // The handlers on tested builds are synchronous, but verify the
        // completed write since some builds swallow a failed registry write.
        DWORD saved = 0;
        DWORD bytes = sizeof(saved);
        if (RegGetValueW(HKEY_CURRENT_USER, kAdvancedKey, L"TaskbarLocation",
                         RRF_RT_REG_DWORD, nullptr, &saved,
                         &bytes) != ERROR_SUCCESS ||
            saved != kLocationEdges[index]) {
            Wh_Log(L"Taskbar position was not saved");
            return false;
        }

        Wh_Log(L"Taskbar moved to edge %lu", edge);
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Taskbar position change failed: %08X",
               static_cast<unsigned>(e.code().value));
    } catch (...) {
        Wh_Log(L"Taskbar position change failed");
    }

    return false;
}

bool GetCurrentTaskbarEdge(DWORD* edge) {
    // Use the actual edge rather than the saved preference: another taskbar
    // mod can move the bar without changing the native registry setting.
    APPBARDATA data = {};
    data.cbSize = sizeof(data);
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &data) || data.uEdge > ABE_BOTTOM) {
        return false;
    }
    *edge = data.uEdge;
    return true;
}

DWORD NearestEdgeFromPoint(POINT pt) {
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return static_cast<DWORD>(-1);
    }

    LONG distLeft = pt.x - info.rcMonitor.left;
    LONG distRight = info.rcMonitor.right - pt.x;
    LONG distTop = pt.y - info.rcMonitor.top;
    LONG distBottom = info.rcMonitor.bottom - pt.y;

    LONG minDist = distBottom;
    DWORD edge = ABE_BOTTOM;
    if (distLeft < minDist) {
        minDist = distLeft;
        edge = ABE_LEFT;
    }
    if (distTop < minDist) {
        minDist = distTop;
        edge = ABE_TOP;
    }
    if (distRight < minDist) {
        edge = ABE_RIGHT;
    }
    return edge;
}

void MoveTaskbarToNearestEdge(POINT pt) {
    DWORD target = NearestEdgeFromPoint(pt);
    if (target > ABE_BOTTOM) {
        return;
    }

    DWORD current = 0;
    if (!GetCurrentTaskbarEdge(&current)) {
        Wh_Log(L"Could not determine the current taskbar edge");
        return;
    }
    if (current == target) {
        Wh_Log(L"Taskbar is already at edge %lu", target);
        return;
    }

    SetTaskbarLocationByEdge(target);
}

// Lazy UIAutomation setup. The taskbar thread is already initialized as an STA,
// so creation is deferred to first use; initialization at mod init has been
// observed to break unrelated global shortcuts.
static IUIAutomation* g_uia = nullptr;
static bool g_uiaUnavailable = false;

IUIAutomation* GetUIAutomation() {
    if (g_uia || g_uiaUnavailable) {
        return g_uia;
    }

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IUIAutomation* uia = nullptr;
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                reinterpret_cast<void**>(&uia))) ||
        !uia) {
        g_uiaUnavailable = true;
        Wh_Log(L"UIAutomation init failed; empty-space detection disabled");
        return nullptr;
    }

    // ponytail: intentionally leaked; it is bound to the taskbar thread's
    // apartment and releasing it from an unloading thread would be unsafe.
    g_uia = uia;
    return g_uia;
}

bool IsEmptySpaceAtPoint(POINT pt) {
    IUIAutomation* uia = GetUIAutomation();
    if (!uia) {
        return false;
    }

    // UIAutomation reliably returns the class name of the clicked element;
    // plain window APIs only ever report the wrapping Shell_TrayWnd.
    IUIAutomationElement* element = nullptr;
    if (FAILED(uia->ElementFromPoint(pt, &element)) || !element) {
        return false;
    }

    bool empty = false;
    BSTR className = nullptr;
    if (SUCCEEDED(element->get_CurrentClassName(&className)) && className) {
        empty = wcscmp(className, L"Shell_TrayWnd") == 0 ||
                wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
                wcscmp(className, L"Taskbar.TaskbarFrameAutomationPeer") == 0 ||
                wcscmp(className, L"Windows.UI.Input.InputSite.WindowClass") ==
                    0;
        SysFreeString(className);
    }
    element->Release();
    return empty;
}

bool IsTaskbarClassWindow(HWND hWnd) {
    if (!hWnd) {
        return false;
    }
    wchar_t className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

enum class DragPointerKind {
    None,
    Mouse,
    TouchPen,
};

DragPointerKind ClassifyPointer(UINT32 pointerId, WPARAM wParam) {
    POINTER_INFO info;
    if (!GetPointerInfo(pointerId, &info)) {
        return DragPointerKind::None;
    }

    if (info.pointerType == PT_MOUSE) {
        return IS_POINTER_FIRSTBUTTON_WPARAM(wParam) ? DragPointerKind::Mouse
                                                     : DragPointerKind::None;
    }
    if (info.pointerType == PT_TOUCH || info.pointerType == PT_PEN) {
        return IS_POINTER_PRIMARY_WPARAM(wParam) ? DragPointerKind::TouchPen
                                                 : DragPointerKind::None;
    }
    return DragPointerKind::None;
}

struct DragState {
    bool tracking = false;
    bool moving = false;
    bool captured = false;
    bool isMouse = false;
    UINT32 pointerId = 0;
    POINT start = {0, 0};
    HWND hWnd = nullptr;
};
static DragState g_drag;

bool ExceededDragThreshold(POINT a, POINT b) {
    const LONG cx = std::max<LONG>(8, GetSystemMetrics(SM_CXDRAG));
    const LONG cy = std::max<LONG>(8, GetSystemMetrics(SM_CYDRAG));
    LONG dx = a.x - b.x;
    if (dx < 0) {
        dx = -dx;
    }
    LONG dy = a.y - b.y;
    if (dy < 0) {
        dy = -dy;
    }
    return dx > cx || dy > cy;
}

WNDPROC InputSiteWindowProc_Original;

LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam) {
    if (uMsg == WM_POINTERDOWN || uMsg == WM_POINTERUPDATE ||
        uMsg == WM_POINTERUP || uMsg == WM_POINTERCAPTURECHANGED) {
        const UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

        switch (uMsg) {
            case WM_POINTERDOWN:
                if (!g_taskbarLocked.load() && !g_drag.tracking &&
                    IsTaskbarClassWindow(GetAncestor(hWnd, GA_ROOT))) {
                    DragPointerKind kind = ClassifyPointer(pointerId, wParam);
                    if (kind != DragPointerKind::None &&
                        IsEmptySpaceAtPoint(pt)) {
                        g_drag = DragState{};
                        g_drag.tracking = true;
                        g_drag.isMouse = kind == DragPointerKind::Mouse;
                        g_drag.pointerId = pointerId;
                        g_drag.start = pt;
                        g_drag.hWnd = hWnd;
                    }
                }
                break;

            case WM_POINTERUPDATE:
                if (g_drag.tracking && !g_drag.moving &&
                    pointerId == g_drag.pointerId &&
                    ExceededDragThreshold(pt, g_drag.start)) {
                    g_drag.moving = true;
                    if (g_drag.isMouse && SetCapture(hWnd)) {
                        g_drag.captured = true;
                    }
                    Wh_Log(L"Taskbar drag started");
                }
                break;

            case WM_POINTERUP:
                if (g_drag.tracking && pointerId == g_drag.pointerId) {
                    const bool moved = g_drag.moving;
                    if (g_drag.captured) {
                        ReleaseCapture();
                    }
                    g_drag = DragState{};
                    if (moved) {
                        MoveTaskbarToNearestEdge(pt);
                    }
                }
                break;

            case WM_POINTERCAPTURECHANGED:
                // Capture can change mid-drag; re-capture so the release
                // point is still received when the pointer leaves the bar.
                if (g_drag.tracking && g_drag.moving && g_drag.isMouse &&
                    pointerId == g_drag.pointerId) {
                    if (SetCapture(g_drag.hWnd)) {
                        g_drag.captured = true;
                    }
                }
                break;
        }
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

void OnLockToggleClick(wf::IInspectable const& sender,
                       wux::RoutedEventArgs const&) {
    try {
        const bool locked = !g_taskbarLocked.load();
        g_taskbarLocked.store(locked);
        Wh_SetIntValue(L"LockTaskbar", locked ? 1 : 0);

        if (auto item = sender.try_as<wuxc::MenuFlyoutItem>()) {
            if (auto icon = item.Icon()) {
                icon.Opacity(locked ? 1.0 : 0.0);
            }
        }

        Wh_Log(L"Taskbar lock %s", locked ? L"enabled" : L"disabled");
    } catch (...) {
        Wh_Log(L"Taskbar lock toggle failed");
    }
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

void AppendInjectedItems(void* vectorThis) {
    const bool locked = g_taskbarLocked.load();

    wuxc::MenuFlyoutItem item;
    item.Name(kItemName);
    item.Text(kLockTaskbarText);

    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(L"\xE73E");
    icon.FontSize(16);
    icon.Opacity(locked ? 1.0 : 0.0);
    item.Icon(icon);

    item.Click(wux::RoutedEventHandler{&OnLockToggleClick});

    wuxc::MenuFlyoutSeparator separator;
    separator.Name(kSeparatorName);

    MenuFlyoutItemBaseVector_Append_Original(vectorThis, item);
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);

    g_currentMenuInjected = true;
    Wh_Log(L"Injected taskbar lock toggle into the context menu");
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    if (g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (!IsSeparator(item) && !IsWindhawkInjectedItem(item) &&
                !GetMenuItemText(item).empty()) {
                AppendInjectedItems(pThis);
            }
        } catch (...) {
            Wh_Log(L"Taskbar menu append inspection failed; skipping injection");
        }
    }

    MenuFlyoutItemBaseVector_Append_Original(pThis, item);
}

struct ScopedTaskbarSettingsMenuBuild {
    bool outer = false;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_currentMenuInjected = false;
        }
    }

    ~ScopedTaskbarSettingsMenuBuild() {
        if (outer && !g_currentMenuInjected) {
            Wh_Log(L"Skipping injection: no menu item append observed");
        }

        g_taskbarSettingsMenuDepth--;
    }
};

using ContextMenus_ShowTaskbarSettingsContextMenu_t =
    void(__cdecl*)(wux::FrameworkElement const& target,
                   void* taskbarSettings,
                   wuxi::ContextRequestedEventArgs const& args,
                   unsigned long long options);
ContextMenus_ShowTaskbarSettingsContextMenu_t
    ContextMenus_ShowTaskbarSettingsContextMenu_Original;

void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(
    wux::FrameworkElement const& target,
    void* taskbarSettings,
    wuxi::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    ScopedTaskbarSettingsMenuBuild scopedBuild;

    ContextMenus_ShowTaskbarSettingsContextMenu_Original(
        target, taskbarSettings, args, options);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    void* fixedFileInfo = nullptr;
    UINT len = 0;

    HRSRC resource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (resource) {
        HGLOBAL global = LoadResource(module, resource);
        if (global) {
            void* data = LockResource(global);
            if (data) {
                if (!VerQueryValueW(data, L"\\", &fixedFileInfo, &len) ||
                    len == 0) {
                    fixedFileInfo = nullptr;
                }
            }
        }
    }

    return static_cast<VS_FIXEDFILEINFO*>(fixedFileInfo);
}

void LogModuleInfo(HMODULE module, const wchar_t* reason) {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(module, path, ARRAYSIZE(path));

    const wchar_t* moduleName = wcsrchr(path, L'\\');
    moduleName = moduleName ? moduleName + 1 : path;

    VS_FIXEDFILEINFO* versionInfo = GetModuleVersionInfo(module);
    if (versionInfo) {
        Wh_Log(L"%s module=%s path=%s version=%u.%u.%u.%u", reason, moduleName,
               path, HIWORD(versionInfo->dwFileVersionMS),
               LOWORD(versionInfo->dwFileVersionMS),
               HIWORD(versionInfo->dwFileVersionLS),
               LOWORD(versionInfo->dwFileVersionLS));
    } else {
        Wh_Log(L"%s module=%s path=%s version=<unavailable>", reason,
               moduleName, path);
    }
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    LogModuleInfo(module, L"Resolving taskbar symbols");

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
            },
            &ContextMenus_ShowTaskbarSettingsContextMenu_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )",
            },
            &MenuFlyoutItemBaseVector_Append_Original,
            MenuFlyoutItemBaseVector_Append_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Skipping menu injection: missing symbols");
        return false;
    }

    Wh_Log(L"Hook installed: ContextMenus::ShowTaskbarSettingsContextMenu");
    Wh_Log(L"Hook installed: IVector<MenuFlyoutItemBase>::Append");

    return true;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module,
                                     LPCWSTR loadedFileName,
                                     bool applyHookOperations) {
    if (!module || g_taskbarViewModuleHooked) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewModuleHooked.exchange(true)) {
        return;
    }

    Wh_Log(L"Taskbar module loaded via LoadLibraryExW: %s",
           loadedFileName ? loadedFileName : L"<null>");

    if (HookTaskbarViewDllSymbols(module)) {
        if (applyHookOperations) {
            Wh_ApplyHookOperations();
        }
    } else {
        Wh_Log(L"Skipping menu injection: unsupported build or missing symbols");
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName, true);
    }

    return module;
}

static std::unordered_set<HWND> g_hookedInputSiteWindows;
static std::unordered_set<void*> g_hookedInputSiteProcs;
static std::atomic<bool> g_whInitialized = false;

bool IsTaskbarInputSiteWindow(HWND hWnd) {
    HWND bridge = GetParent(hWnd);
    if (!bridge) {
        return false;
    }

    wchar_t className[64];
    if (!GetClassNameW(bridge, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") !=
            0) {
        return false;
    }

    return IsTaskbarClassWindow(GetParent(bridge));
}

bool HookInputSiteWindow(HWND hWnd) {
    if (g_hookedInputSiteWindows.contains(hWnd)) {
        return true;
    }

    // The InputSite window cannot be subclassed: inputsite.dll validates its
    // own window proc and crashes if it was replaced. The shared window proc
    // is patched in place instead.
    void* wndProc =
        reinterpret_cast<void*>(GetWindowLongPtrW(hWnd, GWLP_WNDPROC));
    if (!g_hookedInputSiteProcs.contains(wndProc)) {
        if (!Wh_SetFunctionHook(wndProc,
                                reinterpret_cast<void*>(InputSiteWindowProc_Hook),
                                reinterpret_cast<void**>(
                                    &InputSiteWindowProc_Original))) {
            Wh_Log(L"Failed to hook InputSite wndproc %p", wndProc);
            return false;
        }
        g_hookedInputSiteProcs.insert(wndProc);
        Wh_Log(L"Hooked InputSite wndproc %p (hwnd %p)", wndProc, hWnd);
    }

    if (g_whInitialized.load()) {
        Wh_ApplyHookOperations();
    }

    g_hookedInputSiteWindows.insert(hWnd);
    return true;
}

void TryHookTaskbarInputSite(HWND hWnd, LPCWSTR className) {
    if (!hWnd || !className) {
        return;
    }
    // Skip atom class names (HTTP_ATOM-like small integers).
    if ((reinterpret_cast<ULONG_PTR>(className) & ~(ULONG_PTR)0xffff) == 0) {
        return;
    }
    if (_wcsicmp(className, L"Windows.UI.Input.InputSite.WindowClass") != 0) {
        return;
    }
    if (!IsTaskbarInputSiteWindow(hWnd)) {
        return;
    }

    HookInputSiteWindow(hWnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                 int Y, int nWidth, int nHeight, HWND hWndParent,
                                 HMENU hMenu, HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (hWnd) {
        TryHookTaskbarInputSite(hWnd, lpClassName);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                    int Y, int nWidth, int nHeight,
                                    HWND hWndParent, HMENU hMenu,
                                    HINSTANCE hInstance, LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName,
                                            dwStyle, X, Y, nWidth, nHeight,
                                            hWndParent, hMenu, hInstance,
                                            lpParam, dwBand);

    if (hWnd) {
        TryHookTaskbarInputSite(hWnd, lpClassName);
    }

    return hWnd;
}

BOOL CALLBACK EnumTaskbarWindowsProc(HWND hWnd, LPARAM) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }
    if (!IsTaskbarClassWindow(hWnd)) {
        return TRUE;
    }

    HWND bridge = FindWindowExW(
        hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
        nullptr);
    HWND inputSite = bridge ? FindWindowExW(
                                  bridge, nullptr,
                                  L"Windows.UI.Input.InputSite.WindowClass",
                                  nullptr)
                            : nullptr;
    if (inputSite) {
        HookInputSiteWindow(inputSite);
    }
    return TRUE;
}

void HookExistingTaskbarInputSites() {
    EnumWindows(EnumTaskbarWindowsProc, 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_taskbarLocked.store(Wh_GetIntValue(L"LockTaskbar", 0) != 0);
    Wh_Log(L"Taskbar lock is %s", g_taskbarLocked.load() ? L"on" : L"off");

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewModuleHooked = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Menu symbols missing; only dragging will be available");
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW =
        kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;
    if (loadLibraryExW &&
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original)) {
        Wh_Log(L"Hook installed: LoadLibraryExW");
    }

    // Taskbar (and its InputSite window) can be recreated; catch creation.
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                            reinterpret_cast<void*>(CreateWindowExW_Hook),
                            reinterpret_cast<void**>(
                                &CreateWindowExW_Original))) {
        Wh_Log(L"CreateWindowExW hook failed");
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    void* createWindowInBand =
        user32 ? reinterpret_cast<void*>(
                     GetProcAddress(user32, "CreateWindowInBand"))
               : nullptr;
    if (createWindowInBand &&
        Wh_SetFunctionHook(createWindowInBand,
                           reinterpret_cast<void*>(CreateWindowInBand_Hook),
                           reinterpret_cast<void**>(
                               &CreateWindowInBand_Original))) {
        Wh_Log(L"Hook installed: CreateWindowInBand");
    } else {
        Wh_Log(L"CreateWindowInBand hook failed");
    }

    g_whInitialized.store(true);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");

    if (!g_taskbarViewModuleHooked) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewModuleHooked.exchange(true)) {
                Wh_Log(L"Taskbar view module found after init");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                } else {
                    Wh_Log(L"Skipping menu injection: missing symbols");
                }
            }
        }
    }

    // The taskbar usually exists before the mod is injected.
    HookExistingTaskbarInputSites();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
