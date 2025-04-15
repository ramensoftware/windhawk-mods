// ==WindhawkMod==
// @id              middle-click-copy-path
// @name            Middle click to copy the file path of the item that is middle clicled. on the taskbar
// @description     Copy the file path of the program with a middle click on the taskbar instead of closing it. a fork.
// @version         1.0.0
// @author          webber3242
// @github          https://github.com/webber3242
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion -lwininet
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
// Original work by m417z (https://github.com/m417z).

#include <psapi.h>
#include <wininet.h>
#include <dwmapi.h>
#include <algorithm>
#include <atomic>
#include <string>
#include <string_view>
#include <vector>
#ifndef DWMWA_APP_ID
#define DWMWA_APP_ID 26
#endif

enum {
    MULTIPLE_ITEMS_BEHAVIOR_NONE,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND,
};

struct {
    int multipleItemsBehavior;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

using CTaskListWnd_HandleClick_t = long(WINAPI*)(
    LPVOID pThis,
    LPVOID,  // ITaskGroup *
    LPVOID,  // ITaskItem *
    LPVOID   // winrt::Windows::System::LauncherOptions const &
);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;

using CTaskListWnd__HandleClick_t =
    void(WINAPI*)(LPVOID pThis,
                  LPVOID,  // ITaskBtnGroup *
                  int,
                  int,  // enum CTaskListWnd::eCLICKACTION
                  int,
                  int);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;

using CTaskBand_Launch_t = long(WINAPI*)(LPVOID pThis,
                                         LPVOID,  // ITaskGroup *
                                         LPVOID,  // tagPOINT const &
                                         int  // enum LaunchFromTaskbarOptions
);
CTaskBand_Launch_t CTaskBand_Launch_Original;

using CTaskListWnd_GetActiveBtn_t = HRESULT(WINAPI*)(LPVOID pThis,
                                                     LPVOID*,  // ITaskGroup **
                                                     int*);
CTaskListWnd_GetActiveBtn_t CTaskListWnd_GetActiveBtn_Original;

using CTaskListWnd_ProcessJumpViewCloseWindow_t =
    void(WINAPI*)(LPVOID pThis,
                  HWND,
                  LPVOID,  // struct ITaskGroup *
                  HMONITOR);
CTaskListWnd_ProcessJumpViewCloseWindow_t
    CTaskListWnd_ProcessJumpViewCloseWindow_Original;

using CTaskBand__EndTask_t = void(WINAPI*)(LPVOID pThis,
                                           HWND hWnd,
