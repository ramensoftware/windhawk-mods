// ==WindhawkMod==
// @id              win-d-per-monitor
// @name            Win+D per monitor
// @description     Press Win+D to only manage the windows on the monitor where the mouse is located.
// @description:zh-CN   按下Win+D时 只最小化/还原鼠标所在显示器的窗口
// @version         1.0.20241214
// @author          easyatm
// @github          https://github.com/easyatm
// @include         explorer.exe
// @compilerOptions -luser32 -lDwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win+D per monitor

When you press win+d, it only minimizes/restores the windows on the monitor
where the mouse cursor is located.

按下win+d时,只最小化/还原鼠标所在监视器上的窗口

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

    static void activeWnd(HWND hWnd)
    {
        if (hWnd == nullptr)
            return;

        HWND hForeWnd = nullptr;
        DWORD dwForeId;
        DWORD dwCurId;
        hForeWnd = GetForegroundWindow();
        dwCurId = GetCurrentThreadId();
        dwForeId = GetWindowThreadProcessId(hForeWnd, nullptr);
        AttachThreadInput(dwCurId, dwForeId, TRUE);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetForegroundWindow(hWnd);
        AttachThreadInput(dwCurId, dwForeId, FALSE);
    }

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

    static HMONITOR getCurrentMonitor()
    {
        POINT pos = {0};
        if (!GetCursorPos(&pos))
        {
            return nullptr;
        }
        return MonitorFromPoint(pos, MONITOR_DEFAULTTONEAREST);
    }

    static bool isInvisibleWin10BackgroundAppWindow(HWND hWnd)
    {
        int cloakedVal;
        HRESULT hRes = DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloakedVal, sizeof(cloakedVal));
        if (hRes != S_OK)
            cloakedVal = 0;
        return cloakedVal != 0;
    }

    static bool showOwnerWnd(HMONITOR hMonitor, bool isActive)
    {
        auto &listWnd = mapWnd[hMonitor];
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

    static std::string getWindowClassName(HWND hwnd)
    {
        char className[256];
        GetClassNameA(hwnd, className, sizeof(className));
        return std::string(className);
    }

  public:
    static bool showDesktop()
    {
#define MIN_SIZE_WND (100 * 100)

        HWND hDesktop = getDesktopWnd();
        HMONITOR hMonitor = getCurrentMonitor();

        if (!hDesktop || !hMonitor)
            return false;

        auto enumMonitorWnd = [&](HWND hDesktop, HMONITOR hMonitor) {
            std::vector<WndInfo> vec;
            HWND hWndCcc = hDesktop;

            while (hWndCcc)
            {
                hWndCcc = GetNextWindow(hWndCcc, GW_HWNDPREV);

                if (!::IsWindowVisible(hWndCcc) || IsIconic(hWndCcc) || isInvisibleWin10BackgroundAppWindow(hWndCcc))
                {
                    continue;
                }

                auto wndClass = getWindowClassName(hWndCcc);

                // 处理需要忽略的类名.
                {
                    static std::set<std::string> ignoredClass = {"Shell_SecondaryTrayWnd", "Shell_TrayWnd", "WorkerW", "SysShadow", "TaskListThumbnailWnd"};
                    if (ignoredClass.find(wndClass) != ignoredClass.end())
                    {
                        continue;
                    }
                }

                RECT rct = {0};
                if (!::GetWindowRect(hWndCcc, &rct))
                    continue;

                if (MonitorFromRect(&rct, MONITOR_DEFAULTTONEAREST) != hMonitor)
                    continue;

                if ((rct.bottom - rct.top) * (rct.right - rct.left) < MIN_SIZE_WND)
                    continue;

                if ((rct.bottom - rct.top) < 50 || (rct.right - rct.left) < 50)
                    continue;

                vec.push_back({hWndCcc, wndClass, ::GetWindow(hWndCcc, GW_OWNER)});
            }
            return vec;
        };

        auto vecCur = enumMonitorWnd(hDesktop, hMonitor);
        auto &listWnd = mapWnd[hMonitor];

        if (!vecCur.empty())
        {
            for (auto it = listWnd.begin(); it != listWnd.end();)
            {
                auto curIt = it++;
                if (!curIt->ownerWnd)
                    listWnd.erase(curIt);
            }

            for (auto &rc : vecCur)
            {
                if (!IsWindowVisible(rc.wnd) || IsIconic(rc.wnd))
                    continue;

                if (rc.ownerWnd)
                    ShowOwnedPopups(rc.ownerWnd, false);
                else
                    ::ShowWindow(rc.wnd, SW_MINIMIZE);

                listWnd.emplace_back(std::move(rc));
            }

            activeWnd(hDesktop);
        }
        else
        {
            HWND hLastWnd = nullptr;

            for (auto it = listWnd.begin(); it != listWnd.end(); it++)
            {
                auto &rc = *it;
                if (!::IsWindow(rc.wnd))
                    continue;

                if (rc.ownerWnd)
                {
                    ShowOwnedPopups(rc.ownerWnd, true);
                }
                else
                {
                    ::ShowWindow(rc.wnd, SW_SHOWNOACTIVATE);
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

void (*__cdecl HandleGlobalHotkey_Original)(unsigned __int64, __int64);
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

BOOL Wh_ModInit()
{
    Wh_Log(L"init");

    // explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(protected: void __cdecl CTray::_HandleGlobalHotkey(unsigned __int64,__int64))"},
            (void **)&HandleGlobalHotkey_Original,
            (void *)HandleGlobalHotkey_Hook,
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

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged()
{
    // Wh_Log(L"SettingsChanged");
}
