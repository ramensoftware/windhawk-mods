// ==WindhawkMod==
// @id              win-d-per-monitor
// @name            Win+D per monitor(show desktop)
// @description     Press Win+D to only manage the windows on the monitor where the mouse is located.
// @description:zh-CN   按下Win+D时 只最小化/还原鼠标所在显示器的窗口
// @version         1.1.20251123
// @author          easyatm
// @github          https://github.com/easyatm
// @include         explorer.exe
// @compilerOptions -luser32 -lDwmapi
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- ignoreTopmostNoTitleBarWindows: true
  $name: Ignore topmost tool windows without title bar
  $description: Ignore the topmost tool windows without title bar (always on top tool windows)
  $description:zh-CN: 忽略最顶部且无标题栏的工具窗口（始终在顶部的工具窗口）
- minWindowSize: 100
  $name: Minimum window size
  $description: Minimum width or height for windows to be processed (pixels)
  $description:zh-CN: 处理窗口的最小宽度或高度（像素）

*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Win+D per monitor

When you press win+d, it only minimizes/restores the windows on the monitor
where the mouse cursor is located.

按下win+d时,只最小化/还原鼠标所在监视器上的窗口

## Changelog

### 2025-11-23 (v1.1.20251123)
- Fixed window control to use ShowWindow instead of PostMessage for better compatibility with windows that have popups, such as Excel
- 修复窗口控制，使用 ShowWindow 而非 PostMessage，以更好地兼容带有弹出窗口的窗口，如 Excel

Fixes:
- https://github.com/ramensoftware/windhawk-mods/issues/2709

### 2025-08-11 (v1.1.20250811)
- Added option to ignore topmost tool windows without title bar during Win+D operation
- 新增忽略置顶且无标题栏的工具窗口选项，在Win+D操作时保持这类窗口可见

- Added configurable minimum window size setting
- 新增可配置的最小窗口尺寸设置

Fixes:
- https://github.com/ramensoftware/windhawk/discussions/597
- https://github.com/ramensoftware/windhawk-mods/issues/2056


*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <errhandlingapi.h>
#include <format>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <winsvc.h>

// 将ANSI字符串转换为Unicode宽字符串
static std::wstring ansi_unicode(std::string_view str)
{
    if (str.empty())
        return L"";
    int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

using namespace std;
#define log(...) Wh_Log(L"%s", ansi_unicode(format(__VA_ARGS__)).c_str());

// 设置变量
struct
{
    bool ignoreTopmostNoTitleBarWindows;
    int minWindowSize;
} g_settings;

#define HOTKEY_ID_WIN_D 0x201
class WindShowDesktop
{
    struct WndInfo
    {
        HWND wnd = nullptr;
        std::string cls;
        HWND ownerWnd = nullptr;
    };
    inline static std::map<HMONITOR, std::list<WndInfo>> mapWnd;

    // 激活指定窗口
    static void activeWnd(HWND hWnd)
    {
        if (hWnd == nullptr)
            return;

        auto hForeWnd = GetForegroundWindow();
        auto dwCurId = GetCurrentThreadId();
        auto dwForeId = GetWindowThreadProcessId(hForeWnd, nullptr);
        AttachThreadInput(dwCurId, dwForeId, TRUE);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetForegroundWindow(hWnd);
        AttachThreadInput(dwCurId, dwForeId, FALSE);
    }

    // 获取桌面窗口句柄
    static HWND getDesktopWnd()
    {
        HWND hWnd = nullptr;

        do
        {
            HWND hWnd = ::FindWindowA("Progman", "Program Manager");
            if (::FindWindowExA(hWnd, nullptr, "SHELLDLL_DefView", nullptr) != nullptr)
            {
                return hWnd;
            }

            break;
        } while (true);

        hWnd = nullptr;
        do
        {
            hWnd = ::FindWindowExA(nullptr, hWnd, "WorkerW", nullptr);
            if (hWnd == nullptr)
                break;

            if (::FindWindowExA(hWnd, nullptr, "SHELLDLL_DefView", nullptr) != nullptr)
                return hWnd;
        } while (true);
        return nullptr;
    }

    // 获取当前鼠标所在的显示器
    static HMONITOR getCurrentMonitor()
    {
        POINT pos = { 0 };
        if (!GetCursorPos(&pos))
        {
            return nullptr;
        }
        return MonitorFromPoint(pos, MONITOR_DEFAULTTONEAREST);
    }

    // 检测是否为Win10后台应用的隐藏窗口
    static bool isInvisibleWin10BackgroundAppWindow(HWND hWnd)
    {
        int cloakedVal;
        HRESULT hRes = DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloakedVal, sizeof(cloakedVal));
        if (hRes != S_OK)
            cloakedVal = 0;
        return cloakedVal != 0;
    }

    // 显示指定显示器上的拥有者窗口
    static bool showOwnerWnd(HMONITOR hMonitor, bool isActive)
    {
        auto& listWnd = mapWnd[hMonitor];
        HWND hLastWnd = nullptr;
        for (auto it = listWnd.begin(); it != listWnd.end();)
        {
            auto curIt = it++;
            if (curIt->ownerWnd)
            {
                ShowOwnedPopups(curIt->ownerWnd, true);
                hLastWnd = curIt->wnd;
                listWnd.erase(curIt);
            }
        }
        if (hLastWnd && isActive)
            activeWnd(hLastWnd);
        return hLastWnd != nullptr;
    }

    // 获取窗口类名
    static std::string getWindowClassName(HWND hwnd)
    {
        char className[256];
        GetClassNameA(hwnd, className, sizeof(className));
        return std::string(className);
    }

    // 判断是否应该忽略此窗口（置顶且无标题栏的工具窗口）
    static bool ignoreTopmostWindow(HWND hwnd)
    {
        // 如果未启用忽略功能，不忽略任何窗口
        if (!g_settings.ignoreTopmostNoTitleBarWindows)
            return false;

        // 检查是否为置顶窗口
        DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        if ((exStyle & WS_EX_TOPMOST) == 0)
            return false;

        // 检查是否无标题栏
        DWORD style = GetWindowLong(hwnd, GWL_STYLE);
        if ((style & WS_CAPTION) != 0)
            return false;

        // 检查是否为工具窗口
        if ((exStyle & WS_EX_TOOLWINDOW) == 0)
            return false;

        // 满足所有条件，应该忽略此窗口
        return true;
    }

public:
    // 显示桌面功能的主要实现
    static bool showDesktop()
    {
        HWND hDesktop = getDesktopWnd();
        HMONITOR hMonitor = getCurrentMonitor();

        if (!hDesktop || !hMonitor)
        {
            log("Failed to get desktop window or current monitor");
            return false;
        }

        auto enumMonitorWnd = [ & ] (HWND hDesktop, HMONITOR hMonitor)
        {
            std::vector<WndInfo> vec;
            HWND hWndCcc = hDesktop;

            auto minSize = g_settings.minWindowSize * g_settings.minWindowSize;

            while (hWndCcc)
            {
                hWndCcc = GetNextWindow(hWndCcc, GW_HWNDPREV);

                if (!::IsWindowVisible(hWndCcc) || IsIconic(hWndCcc) || isInvisibleWin10BackgroundAppWindow(hWndCcc))
                {
                    continue;
                }

                // 检查是否应该忽略此窗口
                if (ignoreTopmostWindow(hWndCcc))
                {
                    continue;
                }

                auto wndClass = getWindowClassName(hWndCcc);

                // 处理需要忽略的类名.
                {
                    static std::set<std::string> ignoredClass = { "Shell_SecondaryTrayWnd", "Shell_TrayWnd", "WorkerW", "SysShadow", "TaskListThumbnailWnd" };
                    if (ignoredClass.find(wndClass) != ignoredClass.end())
                    {
                        continue;
                    }
                }

                RECT rct = { 0 };
                if (!::GetWindowRect(hWndCcc, &rct))
                    continue;

                if (MonitorFromRect(&rct, MONITOR_DEFAULTTONEAREST) != hMonitor)
                    continue;

                // 计算窗口大小
                int width = rct.right - rct.left;
                int height = rct.bottom - rct.top;

                if (width * height < minSize)
                    continue;

                if (height < g_settings.minWindowSize || width < g_settings.minWindowSize)
                    continue;

                HWND ownerWnd = ::GetWindow(hWndCcc, GW_OWNER);

                // 获取窗口标题
                char windowTitle[256] = { 0 };
                GetWindowTextA(hWndCcc, windowTitle, sizeof(windowTitle));

                // 输出窗口信息
                log("Processing2 window: class='{}', title='{}', size={}x{}, hwnd=0x{:x}, owner=0x{:x}", wndClass, windowTitle, width, height,
                    (uintptr_t)hWndCcc, (uintptr_t)ownerWnd);

                log("hWnd:0x{:x}", (uintptr_t)hWndCcc);
                vec.push_back({ hWndCcc, wndClass, ownerWnd });
            }
            return vec;
        };

        auto vecCur = enumMonitorWnd(hDesktop, hMonitor);
        auto& listWnd = mapWnd[hMonitor];

        log("Found {} windows to process on current monitor", vecCur.size());

        // 允许设置前台窗口，避免窗口在任务栏闪烁
        AllowSetForegroundWindow(ASFW_ANY);

        if (!vecCur.empty())
        {

            for (auto it = listWnd.begin(); it != listWnd.end();)
            {
                auto curIt = it++;
                if (!curIt->ownerWnd)
                    listWnd.erase(curIt);
            }

            for (auto& rc : vecCur)
            {
                if (!IsWindowVisible(rc.wnd) || IsIconic(rc.wnd))
                    continue;

                if (rc.ownerWnd)
                {
                    ShowOwnedPopups(rc.ownerWnd, false);
                    log("ShowOwnedPopups 0x{:x} false", (uintptr_t)rc.ownerWnd);
                }
                else
                {
                    log("ShowWindow SC_MINIMIZE 0x{:x}", (uintptr_t)rc.wnd);

                    ::ShowWindow(rc.wnd, SW_MINIMIZE);
                    //PostMessage(rc.wnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                }

                listWnd.emplace_back(std::move(rc));
            }

            activeWnd(hDesktop);
        }
        else
        {

            HWND hLastWnd = nullptr;

            for (auto it = listWnd.begin(); it != listWnd.end(); it++)
            {
                auto& rc = *it;
                if (!::IsWindow(rc.wnd))
                    continue;

                if (rc.ownerWnd)
                {
                    ShowOwnedPopups(rc.ownerWnd, true);
                }
                else
                {
                    ::ShowWindow(rc.wnd, SW_SHOWNOACTIVATE);
                    //PostMessage(rc.wnd, WM_SYSCOMMAND, SC_RESTORE, 0);
                    hLastWnd = rc.wnd;
                }

                SetWindowPos(rc.wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                SetWindowPos(rc.wnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
            }

            listWnd.clear();
            activeWnd(hLastWnd);
        }

        return true;
    }
};

// 原始的全局热键处理函数指针
void (*__cdecl HandleGlobalHotkey_Original)(unsigned __int64, __int64);

// 钩子函数：拦截Win+D热键
void __cdecl HandleGlobalHotkey_Hook(unsigned __int64 param, __int64 hotkey_id)
{
    if (hotkey_id == HOTKEY_ID_WIN_D)
    {
        WindShowDesktop::showDesktop();
        return;
    }
    log("hotkey id: {:x}", hotkey_id);
    HandleGlobalHotkey_Original(param, hotkey_id);
}

// 加载用户设置
void LoadSettings()
{
    g_settings.ignoreTopmostNoTitleBarWindows = Wh_GetIntSetting(L"ignoreTopmostNoTitleBarWindows");
    g_settings.minWindowSize = Wh_GetIntSetting(L"minWindowSize");
}

// 模块初始化函数
BOOL Wh_ModInit()
{
    Wh_Log(L"init");

    LoadSettings();

    // explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(protected: void __cdecl CTray::_HandleGlobalHotkey(unsigned __int64,__int64))"},
            (void**)&HandleGlobalHotkey_Original,
            (void*)HandleGlobalHotkey_Hook,
        },
    };

    HMODULE hExplorer = GetModuleHandle(L"explorer.exe");
    if (!hExplorer)
    {
        log("GetModuleHandle(explorer.exe) error:{}", ::GetLastError());
        return false;
    }

    if (!WindhawkUtils::HookSymbols(hExplorer, symbolHooks, ARRAYSIZE(symbolHooks)))
    {
        log("HookSymbols error");
        return false;
    }

    return true;
}

// 模块卸载时清理资源
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}

// 用户设置发生变化时重新加载设置
void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
