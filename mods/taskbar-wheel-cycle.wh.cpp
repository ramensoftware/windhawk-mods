// ==WindhawkMod==
// @id              taskbar-wheel-cycle
// @name            Cycle taskbar buttons with mouse wheel
// @description     Use the mouse wheel and/or keyboard shortcuts to cycle between taskbar buttons
// @version         1.1.10
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Cycle taskbar buttons with mouse wheel

Use the mouse wheel while hovering over the taskbar to cycle between taskbar
buttons.

In addition, keyboard shortcuts can be used. The default shortcuts are `Alt+[`
and `Alt+]`, but they can be changed in the mod settings.

Only Windows 10 64-bit and Windows 11 are supported. For older Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

![Demonstration](https://i.imgur.com/FtpUjt1.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- skipMinimizedWindows: true
  $name: Skip minimized windows
- wrapAround: true
  $name: Wrap around
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
- enableMouseWheelCycling: true
  $name: Enable mouse wheel cycling
  $description: >-
    Disable to only use keyboard shortcuts for cycling between taskbar buttons.
- cycleLeftKeyboardShortcut: Alt+VK_OEM_4
  $name: Cycle left keyboard shortcut
  $description: >-
    Possible modifier keys: Alt, Ctrl, Shift, Win. For possible shortcut keys,
    refer to the following page:
    https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

    Set to an empty string to disable.
- cycleRightKeyboardShortcut: Alt+VK_OEM_6
  $name: Cycle right keyboard shortcut
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <psapi.h>
#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

struct {
    bool skipMinimizedWindows;
    bool wrapAround;
    bool reverseScrollingDirection;
    bool enableMouseWheelCycling;
    WindhawkUtils::StringSetting cycleLeftKeyboardShortcut;
    WindhawkUtils::StringSetting cycleRightKeyboardShortcut;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

struct TaskBtnGroupButtonInfo {
    void* taskBtnGroup;
    int buttonIndex;
};

std::unordered_map<void*, TaskBtnGroupButtonInfo> g_lastTaskListActiveItem;

HWND g_lastScrollTarget = nullptr;
DWORD g_lastScrollTime;
short g_lastScrollDeltaRemainder;

bool g_hotkeyLeftRegistered = false;
bool g_hotkeyRightRegistered = false;

enum {
    kHotkeyIdLeft = 1682530408,  // From epochconverter.com
    kHotkeyIdRight,
};

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

HWND GetTaskBandWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        return (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    }

    return nullptr;
}

void* CTaskListWnd_vftable_ITaskListUI;
void* CTaskListWnd_vftable_ITaskListSite;
void* CTaskListWnd_vftable_ITaskListAcc;
void* CImmersiveTaskItem_vftable;

using CTaskListWnd_GetButtonGroupCount_t = int(WINAPI*)(void* pThis);
CTaskListWnd_GetButtonGroupCount_t CTaskListWnd_GetButtonGroupCount;

using CTaskListWnd__GetTBGroupFromGroup_t = void*(WINAPI*)(void* pThis,
                                                           void* taskGroup,
                                                           int* index);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType;

using CTaskBtnGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetNumItems_t CTaskBtnGroup_GetNumItems;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(void* pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

using CTaskListWnd_SwitchToItem_t = void(WINAPI*)(void* pThis, void* taskItem);
CTaskListWnd_SwitchToItem_t CTaskListWnd_SwitchToItem_Original;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

void* QueryViaVtableBackwards(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr - 1;
    }
    return ptr;
}

#pragma region scroll

void SwitchToTaskItem(LONG_PTR lpMMTaskListLongPtr, void* taskItem) {
    void* pThis_ITaskListSite = QueryViaVtable(
        (void*)lpMMTaskListLongPtr, CTaskListWnd_vftable_ITaskListSite);

    CTaskListWnd_SwitchToItem_Original(pThis_ITaskListSite, taskItem);
}

HWND GetTaskItemWnd(PVOID taskItem) {
    if (*(void**)taskItem == CImmersiveTaskItem_vftable) {
        return CImmersiveTaskItem_GetWindow_Original(taskItem);
    } else {
        return CWindowTaskItem_GetWindow_Original(taskItem);
    }
}

BOOL IsMinimizedTaskItem(LONG_PTR* task_item) {
    return IsIconic(GetTaskItemWnd(task_item));
}

BOOL TaskbarScrollRight(int button_groups_count,
                        LONG_PTR** button_groups,
                        int* p_button_group_index,
                        int* p_button_index) {
    int button_group_index = *p_button_group_index;
    int button_index = *p_button_index;
    int button_group_type;

    int buttons_count =
        button_group_index == -1
            ? 0
            : CTaskBtnGroup_GetNumItems(button_groups[button_group_index]);
    if (++button_index >= buttons_count) {
        do {
            button_group_index++;
            if (button_group_index >= button_groups_count) {
                return FALSE;
            }

            button_group_type =
                CTaskBtnGroup_GetGroupType(button_groups[button_group_index]);
        } while (button_group_type != 1 && button_group_type != 3);

        button_index = 0;
    }

    *p_button_group_index = button_group_index;
    *p_button_index = button_index;

    return TRUE;
}

BOOL TaskbarScrollLeft(int button_groups_count,
                       LONG_PTR** button_groups,
                       int* p_button_group_index,
                       int* p_button_index) {
    int button_group_index = *p_button_group_index;
    int button_index = *p_button_index;
    int button_group_type;

    if (button_group_index == -1 || --button_index < 0) {
        if (button_group_index == -1) {
            button_group_index = button_groups_count;
        }

        do {
            button_group_index--;
            if (button_group_index < 0) {
                return FALSE;
            }

            button_group_type =
                CTaskBtnGroup_GetGroupType(button_groups[button_group_index]);
        } while (button_group_type != 1 && button_group_type != 3);

        int buttons_count =
            CTaskBtnGroup_GetNumItems(button_groups[button_group_index]);
        button_index = buttons_count - 1;
    }

    *p_button_group_index = button_group_index;
    *p_button_index = button_index;

    return TRUE;
}

LONG_PTR* TaskbarScrollHelper(int button_groups_count,
                              LONG_PTR** button_groups,
                              int button_group_index_active,
                              int button_index_active,
                              int nRotates,
                              BOOL bSkipMinimized,
                              BOOL bWarpAround) {
    int button_group_index, button_index;
    BOOL bRotateRight;
    int prev_button_group_index, prev_button_index;
    BOOL bScrollSucceeded;

    button_group_index = button_group_index_active;
    button_index = button_index_active;

    bRotateRight = TRUE;
    if (nRotates < 0) {
        bRotateRight = FALSE;
        nRotates = -nRotates;
    }

    prev_button_group_index = button_group_index;
    prev_button_index = button_index;

    while (nRotates--) {
        if (bRotateRight) {
            bScrollSucceeded =
                TaskbarScrollRight(button_groups_count, button_groups,
                                   &button_group_index, &button_index);
            while (bScrollSucceeded && bSkipMinimized &&
                   IsMinimizedTaskItem((LONG_PTR*)CTaskBtnGroup_GetTaskItem(
                       button_groups[button_group_index], button_index))) {
                bScrollSucceeded =
                    TaskbarScrollRight(button_groups_count, button_groups,
                                       &button_group_index, &button_index);
            }
        } else {
            bScrollSucceeded =
                TaskbarScrollLeft(button_groups_count, button_groups,
                                  &button_group_index, &button_index);
            while (bScrollSucceeded && bSkipMinimized &&
                   IsMinimizedTaskItem((LONG_PTR*)CTaskBtnGroup_GetTaskItem(
                       button_groups[button_group_index], button_index))) {
                bScrollSucceeded =
                    TaskbarScrollLeft(button_groups_count, button_groups,
                                      &button_group_index, &button_index);
            }
        }

        if (!bScrollSucceeded) {
            // If no results were found in the whole taskbar
            if (prev_button_group_index == -1) {
                return nullptr;
            }

            if (bWarpAround) {
                // Continue from the beginning
                button_group_index = -1;
                button_index = -1;
                nRotates++;
            } else {
                // Use the last successful result and stop
                button_group_index = prev_button_group_index;
                button_index = prev_button_index;

                break;
            }
        }

        prev_button_group_index = button_group_index;
        prev_button_index = button_index;
    }

    if (button_group_index == button_group_index_active &&
        button_index == button_index_active) {
        return nullptr;
    }

    return (LONG_PTR*)CTaskBtnGroup_GetTaskItem(
        button_groups[button_group_index], button_index);
}

HDPA GetTaskBtnGroupsArray(void* taskList_ITaskListUI) {
    // This is a horrible hack, but it's the best way I found to get the array
    // of task button groups from a task list. It relies on the implementation
    // of CTaskListWnd::GetButtonGroupCount being just this:
    //
    // return DPA_GetPtrCount(this->buttonGroupsArray);
    //
    // Or in other words:
    //
    // return *(int*)this[buttonGroupsArrayOffset];
    //
    // Instead of calling it with a real taskList object, we call it with an
    // array of pointers to ints. The returned int value is actually the offset
    // to the array member.

    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }

        return CTaskListWnd_GetButtonGroupCount(arrayOfIntPtrs);
    }();

    return (HDPA)((void**)taskList_ITaskListUI)[offset];
}

LONG_PTR* TaskbarScroll(LONG_PTR lpMMTaskListLongPtr,
                        int nRotates,
                        BOOL bSkipMinimized,
                        BOOL bWarpAround,
                        LONG_PTR* src_task_item) {
    if (nRotates == 0) {
        return nullptr;
    }

    void* taskList_ITaskListUI = QueryViaVtable(
        (void*)lpMMTaskListLongPtr, CTaskListWnd_vftable_ITaskListUI);

    LONG_PTR* plp = (LONG_PTR*)GetTaskBtnGroupsArray(taskList_ITaskListUI);
    if (!plp) {
        return nullptr;
    }

    int button_groups_count = (int)plp[0];
    LONG_PTR** button_groups = (LONG_PTR**)plp[1];

    int button_group_index_active = -1;
    int button_index_active = -1;

    if (src_task_item) {
        for (int i = 0; i < button_groups_count; i++) {
            int button_group_type =
                CTaskBtnGroup_GetGroupType(button_groups[i]);
            if (button_group_type == 1 || button_group_type == 3) {
                int buttons_count = CTaskBtnGroup_GetNumItems(button_groups[i]);
                for (int j = 0; j < buttons_count; j++) {
                    if ((LONG_PTR*)CTaskBtnGroup_GetTaskItem(
                            button_groups[i], j) == src_task_item) {
                        button_group_index_active = i;
                        button_index_active = j;
                        break;
                    }
                }

                if (button_group_index_active != -1) {
                    break;
                }
            }
        }
    } else if (auto it =
                   g_lastTaskListActiveItem.find((void*)lpMMTaskListLongPtr);
               it != g_lastTaskListActiveItem.end()) {
        LONG_PTR* last_button_group_active = (LONG_PTR*)it->second.taskBtnGroup;
        int last_button_index_active = it->second.buttonIndex;
        if (last_button_group_active && last_button_index_active >= 0) {
            for (int i = 0; i < button_groups_count; i++) {
                if (button_groups[i] == last_button_group_active) {
                    int buttons_count =
                        CTaskBtnGroup_GetNumItems(button_groups[i]);
                    if (buttons_count > 0) {
                        button_group_index_active = i;
                        button_index_active = std::min(last_button_index_active,
                                                       buttons_count - 1);
                    }
                    break;
                }
            }
        }
    }

    return TaskbarScrollHelper(button_groups_count, button_groups,
                               button_group_index_active, button_index_active,
                               nRotates, bSkipMinimized, bWarpAround);
}

#pragma endregion  // scroll

void OnTaskListScroll(HWND hMMTaskListWnd, short delta) {
    if (g_lastScrollTarget == hMMTaskListWnd &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = -delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (clicks != 0) {
        if (g_settings.reverseScrollingDirection) {
            clicks = -clicks;
        }

        LONG_PTR lpMMTaskListLongPtr = GetWindowLongPtr(hMMTaskListWnd, 0);
        PVOID targetTaskItem = TaskbarScroll(lpMMTaskListLongPtr, clicks,
                                             g_settings.skipMinimizedWindows,
                                             g_settings.wrapAround, nullptr);
        if (targetTaskItem) {
            SwitchToTaskItem(lpMMTaskListLongPtr, targetTaskItem);
        }
    }

    g_lastScrollTarget = hMMTaskListWnd;
    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

HWND TaskListFromTaskbarWnd(HWND hTaskbarWnd) {
    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) {
        return nullptr;
    }

    HWND hMSTaskSwWClass =
        FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
    if (!hMSTaskSwWClass) {
        return nullptr;
    }

    return FindWindowEx(hMSTaskSwWClass, nullptr, L"MSTaskListWClass", nullptr);
}

HWND TaskListFromSecondaryTaskbarWnd(HWND hSecondaryTaskbarWnd) {
    HWND hWorkerWWnd =
        FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hWorkerWWnd) {
        return nullptr;
    }

    return FindWindowEx(hWorkerWWnd, nullptr, L"MSTaskListWClass", nullptr);
}

HWND TaskListFromMMTaskbarWnd(HWND hMMTaskbarWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hMMTaskbarWnd, szClassName, ARRAYSIZE(szClassName))) {
        return nullptr;
    }

    if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
        return TaskListFromTaskbarWnd(hMMTaskbarWnd);
    }

    if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
        return TaskListFromSecondaryTaskbarWnd(hMMTaskbarWnd);
    }

    return nullptr;
}

HWND TaskListFromPoint(POINT pt) {
    HWND hPointWnd = WindowFromPoint(pt);
    if (!hPointWnd) {
        return nullptr;
    }

    HWND hRootWnd = GetAncestor(hPointWnd, GA_ROOT);
    if (!hRootWnd) {
        return nullptr;
    }

    return TaskListFromMMTaskbarWnd(hRootWnd);
}

HWND GetTaskbarForMonitor(HWND hTaskbarWnd, HMONITOR monitor) {
    DWORD taskbarThreadId = 0;
    DWORD taskbarProcessId = 0;
    if (!(taskbarThreadId =
              GetWindowThreadProcessId(hTaskbarWnd, &taskbarProcessId)) ||
        taskbarProcessId != GetCurrentProcessId()) {
        return nullptr;
    }

    if (MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST) == monitor) {
        return hTaskbarWnd;
    }

    HWND hResultWnd = nullptr;

    auto enumWindowsProc = [monitor, &hResultWnd](HWND hWnd) -> BOOL {
        WCHAR szClassName[32];
        if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
            return TRUE;
        }

        if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0) {
            return TRUE;
        }

        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
            return TRUE;
        }

        hResultWnd = hWnd;
        return FALSE;
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hResultWnd;
}

bool FromStringHotKey(std::wstring_view hotkeyString,
                      UINT* modifiersOut,
                      UINT* vkOut) {
    static const std::unordered_map<std::wstring_view, UINT> modifiersMap = {
        {L"ALT", MOD_ALT},           {L"CTRL", MOD_CONTROL},
        {L"NOREPEAT", MOD_NOREPEAT}, {L"SHIFT", MOD_SHIFT},
        {L"WIN", MOD_WIN},
    };

    static const std::unordered_map<std::wstring_view, UINT> vkMap = {
        {L"VK_LBUTTON", 0x01},
        {L"VK_RBUTTON", 0x02},
        {L"VK_CANCEL", 0x03},
        {L"VK_MBUTTON", 0x04},
        {L"VK_XBUTTON1", 0x05},
        {L"VK_XBUTTON2", 0x06},
        {L"VK_BACK", 0x08},
        {L"VK_TAB", 0x09},
        {L"VK_CLEAR", 0x0C},
        {L"VK_RETURN", 0x0D},
        {L"VK_SHIFT", 0x10},
        {L"VK_CONTROL", 0x11},
        {L"VK_MENU", 0x12},
        {L"VK_PAUSE", 0x13},
        {L"VK_CAPITAL", 0x14},
        {L"VK_KANA", 0x15},
        {L"VK_HANGUEL", 0x15},
        {L"VK_HANGUL", 0x15},
        {L"VK_IME_ON", 0x16},
        {L"VK_JUNJA", 0x17},
        {L"VK_FINAL", 0x18},
        {L"VK_HANJA", 0x19},
        {L"VK_KANJI", 0x19},
        {L"VK_IME_OFF", 0x1A},
        {L"VK_ESCAPE", 0x1B},
        {L"VK_CONVERT", 0x1C},
        {L"VK_NONCONVERT", 0x1D},
        {L"VK_ACCEPT", 0x1E},
        {L"VK_MODECHANGE", 0x1F},
        {L"VK_SPACE", 0x20},
        {L"VK_PRIOR", 0x21},
        {L"VK_NEXT", 0x22},
        {L"VK_END", 0x23},
        {L"VK_HOME", 0x24},
        {L"VK_LEFT", 0x25},
        {L"VK_UP", 0x26},
        {L"VK_RIGHT", 0x27},
        {L"VK_DOWN", 0x28},
        {L"VK_SELECT", 0x29},
        {L"VK_PRINT", 0x2A},
        {L"VK_EXECUTE", 0x2B},
        {L"VK_SNAPSHOT", 0x2C},
        {L"VK_INSERT", 0x2D},
        {L"VK_DELETE", 0x2E},
        {L"VK_HELP", 0x2F},
        {L"0", 0x30},
        {L"1", 0x31},
        {L"2", 0x32},
        {L"3", 0x33},
        {L"4", 0x34},
        {L"5", 0x35},
        {L"6", 0x36},
        {L"7", 0x37},
        {L"8", 0x38},
        {L"9", 0x39},
        {L"A", 0x41},
        {L"B", 0x42},
        {L"C", 0x43},
        {L"D", 0x44},
        {L"E", 0x45},
        {L"F", 0x46},
        {L"G", 0x47},
        {L"H", 0x48},
        {L"I", 0x49},
        {L"J", 0x4A},
        {L"K", 0x4B},
        {L"L", 0x4C},
        {L"M", 0x4D},
        {L"N", 0x4E},
        {L"O", 0x4F},
        {L"P", 0x50},
        {L"Q", 0x51},
        {L"R", 0x52},
        {L"S", 0x53},
        {L"T", 0x54},
        {L"U", 0x55},
        {L"V", 0x56},
        {L"W", 0x57},
        {L"X", 0x58},
        {L"Y", 0x59},
        {L"Z", 0x5A},
        {L"VK_LWIN", 0x5B},
        {L"VK_RWIN", 0x5C},
        {L"VK_APPS", 0x5D},
        {L"VK_SLEEP", 0x5F},
        {L"VK_NUMPAD0", 0x60},
        {L"VK_NUMPAD1", 0x61},
        {L"VK_NUMPAD2", 0x62},
        {L"VK_NUMPAD3", 0x63},
        {L"VK_NUMPAD4", 0x64},
        {L"VK_NUMPAD5", 0x65},
        {L"VK_NUMPAD6", 0x66},
        {L"VK_NUMPAD7", 0x67},
        {L"VK_NUMPAD8", 0x68},
        {L"VK_NUMPAD9", 0x69},
        {L"VK_MULTIPLY", 0x6A},
        {L"VK_ADD", 0x6B},
        {L"VK_SEPARATOR", 0x6C},
        {L"VK_SUBTRACT", 0x6D},
        {L"VK_DECIMAL", 0x6E},
        {L"VK_DIVIDE", 0x6F},
        {L"VK_F1", 0x70},
        {L"VK_F2", 0x71},
        {L"VK_F3", 0x72},
        {L"VK_F4", 0x73},
        {L"VK_F5", 0x74},
        {L"VK_F6", 0x75},
        {L"VK_F7", 0x76},
        {L"VK_F8", 0x77},
        {L"VK_F9", 0x78},
        {L"VK_F10", 0x79},
        {L"VK_F11", 0x7A},
        {L"VK_F12", 0x7B},
        {L"VK_F13", 0x7C},
        {L"VK_F14", 0x7D},
        {L"VK_F15", 0x7E},
        {L"VK_F16", 0x7F},
        {L"VK_F17", 0x80},
        {L"VK_F18", 0x81},
        {L"VK_F19", 0x82},
        {L"VK_F20", 0x83},
        {L"VK_F21", 0x84},
        {L"VK_F22", 0x85},
        {L"VK_F23", 0x86},
        {L"VK_F24", 0x87},
        {L"VK_NUMLOCK", 0x90},
        {L"VK_SCROLL", 0x91},
        {L"VK_LSHIFT", 0xA0},
        {L"VK_RSHIFT", 0xA1},
        {L"VK_LCONTROL", 0xA2},
        {L"VK_RCONTROL", 0xA3},
        {L"VK_LMENU", 0xA4},
        {L"VK_RMENU", 0xA5},
        {L"VK_BROWSER_BACK", 0xA6},
        {L"VK_BROWSER_FORWARD", 0xA7},
        {L"VK_BROWSER_REFRESH", 0xA8},
        {L"VK_BROWSER_STOP", 0xA9},
        {L"VK_BROWSER_SEARCH", 0xAA},
        {L"VK_BROWSER_FAVORITES", 0xAB},
        {L"VK_BROWSER_HOME", 0xAC},
        {L"VK_VOLUME_MUTE", 0xAD},
        {L"VK_VOLUME_DOWN", 0xAE},
        {L"VK_VOLUME_UP", 0xAF},
        {L"VK_MEDIA_NEXT_TRACK", 0xB0},
        {L"VK_MEDIA_PREV_TRACK", 0xB1},
        {L"VK_MEDIA_STOP", 0xB2},
        {L"VK_MEDIA_PLAY_PAUSE", 0xB3},
        {L"VK_LAUNCH_MAIL", 0xB4},
        {L"VK_LAUNCH_MEDIA_SELECT", 0xB5},
        {L"VK_LAUNCH_APP1", 0xB6},
        {L"VK_LAUNCH_APP2", 0xB7},
        {L"VK_OEM_1", 0xBA},
        {L"VK_OEM_PLUS", 0xBB},
        {L"VK_OEM_COMMA", 0xBC},
        {L"VK_OEM_MINUS", 0xBD},
        {L"VK_OEM_PERIOD", 0xBE},
        {L"VK_OEM_2", 0xBF},
        {L"VK_OEM_3", 0xC0},
        {L"VK_OEM_4", 0xDB},
        {L"VK_OEM_5", 0xDC},
        {L"VK_OEM_6", 0xDD},
        {L"VK_OEM_7", 0xDE},
        {L"VK_OEM_8", 0xDF},
        {L"VK_OEM_102", 0xE2},
        {L"VK_PROCESSKEY", 0xE5},
        {L"VK_PACKET", 0xE7},
        {L"VK_ATTN", 0xF6},
        {L"VK_CRSEL", 0xF7},
        {L"VK_EXSEL", 0xF8},
        {L"VK_EREOF", 0xF9},
        {L"VK_PLAY", 0xFA},
        {L"VK_ZOOM", 0xFB},
        {L"VK_NONAME", 0xFC},
        {L"VK_PA1", 0xFD},
        {L"VK_OEM_CLEAR", 0xFE},
    };

    // https://stackoverflow.com/a/46931770
    auto splitStringView = [](std::wstring_view s, WCHAR delimiter) {
        size_t pos_start = 0, pos_end;
        std::wstring_view token;
        std::vector<std::wstring_view> res;

        while ((pos_end = s.find(delimiter, pos_start)) !=
               std::wstring_view::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + 1;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    };

    // https://stackoverflow.com/a/54364173
    auto trimStringView = [](std::wstring_view s) {
        s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
        s.remove_suffix(std::min(
            s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
        return s;
    };

    UINT modifiers = 0;
    UINT vk = 0;

    auto hotkeyParts = splitStringView(hotkeyString, '+');
    for (auto hotkeyPart : hotkeyParts) {
        hotkeyPart = trimStringView(hotkeyPart);
        std::wstring hotkeyPartUpper{hotkeyPart};
        std::transform(hotkeyPartUpper.begin(), hotkeyPartUpper.end(),
                       hotkeyPartUpper.begin(), ::toupper);

        if (auto it = modifiersMap.find(hotkeyPartUpper);
            it != modifiersMap.end()) {
            modifiers |= it->second;
            continue;
        }

        if (vk) {
            // Only one is allowed.
            return false;
        }

        if (auto it = vkMap.find(hotkeyPartUpper); it != vkMap.end()) {
            vk = it->second;
            continue;
        }

        size_t pos;
        try {
            vk = std::stoi(hotkeyPartUpper, &pos, 0);
            if (hotkeyPartUpper[pos] != L'\0' || !vk) {
                return false;
            }
        } catch (const std::exception&) {
            return false;
        }
    }

    if (!vk) {
        return false;
    }

    *modifiersOut = modifiers;
    *vkOut = vk;
    return true;
}

void UnregisterHotkeys(HWND hWnd) {
    if (g_hotkeyLeftRegistered) {
        UnregisterHotKey(hWnd, kHotkeyIdLeft);
        g_hotkeyLeftRegistered = false;
    }

    if (g_hotkeyRightRegistered) {
        UnregisterHotKey(hWnd, kHotkeyIdRight);
        g_hotkeyRightRegistered = false;
    }
}

void RegisterHotkeys(HWND hWnd) {
    if (!*g_settings.cycleLeftKeyboardShortcut &&
        !*g_settings.cycleRightKeyboardShortcut) {
        return;
    }

    UINT modifiers;
    UINT vk;

    if (FromStringHotKey(g_settings.cycleLeftKeyboardShortcut.get(), &modifiers,
                         &vk)) {
        g_hotkeyLeftRegistered =
            RegisterHotKey(hWnd, kHotkeyIdLeft, modifiers, vk);
        if (!g_hotkeyLeftRegistered) {
            Wh_Log(L"Couldn't register hotkey: %s",
                   g_settings.cycleLeftKeyboardShortcut.get());
        }
    } else {
        Wh_Log(L"Couldn't parse hotkey: %s",
               g_settings.cycleLeftKeyboardShortcut.get());
    }

    if (FromStringHotKey(g_settings.cycleRightKeyboardShortcut.get(),
                         &modifiers, &vk)) {
        g_hotkeyRightRegistered =
            RegisterHotKey(hWnd, kHotkeyIdRight, modifiers, vk);
        if (!g_hotkeyRightRegistered) {
            Wh_Log(L"Couldn't register hotkey: %s",
                   g_settings.cycleRightKeyboardShortcut.get());
        }
    } else {
        Wh_Log(L"Couldn't parse hotkey: %s",
               g_settings.cycleRightKeyboardShortcut.get());
    }
}

bool OnTaskbarHotkey(HWND hWnd, int hotkeyId) {
    Wh_Log(L">");

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    HWND hTaskbarForMonitor = GetTaskbarForMonitor(hWnd, monitor);

    HWND hMMTaskListWnd = TaskListFromMMTaskbarWnd(
        hTaskbarForMonitor ? hTaskbarForMonitor : hWnd);
    if (!hMMTaskListWnd) {
        return false;
    }

    int clicks = hotkeyId == kHotkeyIdLeft ? -1 : 1;

    LONG_PTR lpTaskListLongPtr = GetWindowLongPtr(hMMTaskListWnd, 0);
    PVOID targetTaskItem = TaskbarScroll(lpTaskListLongPtr, clicks,
                                         g_settings.skipMinimizedWindows,
                                         g_settings.wrapAround, nullptr);
    if (targetTaskItem) {
        SwitchToTaskItem(lpTaskListLongPtr, targetTaskItem);
    }

    return true;
}

UINT g_hotkeyRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_hotkey_" WH_MOD_ID);

enum {
    HOTKEY_REGISTER,
    HOTKEY_UNREGISTER,
    HOTKEY_UPDATE,
};

using CTaskListWnd__SetActiveItem_t = void(WINAPI*)(void* pThis,
                                                    void* taskBtnGroup,
                                                    int buttonIndex);
CTaskListWnd__SetActiveItem_t CTaskListWnd__SetActiveItem_Original;
void WINAPI CTaskListWnd__SetActiveItem_Hook(void* pThis,
                                             void* taskBtnGroup,
                                             int buttonIndex) {
    Wh_Log(L">");

    g_lastTaskListActiveItem[pThis] = {
        .taskBtnGroup = taskBtnGroup,
        .buttonIndex = buttonIndex,
    };

    CTaskListWnd__SetActiveItem_Original(pThis, taskBtnGroup, buttonIndex);
}

using CTaskBand_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CTaskBand_v_WndProc_t CTaskBand_v_WndProc_Original;
LRESULT WINAPI CTaskBand_v_WndProc_Hook(void* pThis,
                                        HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam) {
    LRESULT result = 0;

    auto originalProc = [pThis](HWND hWnd, UINT Msg, WPARAM wParam,
                                LPARAM lParam) {
        return CTaskBand_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
    };

    switch (Msg) {
        case WM_HOTKEY:
            switch (wParam) {
                case kHotkeyIdLeft:
                case kHotkeyIdRight:
                    OnTaskbarHotkey(GetAncestor(hWnd, GA_ROOT),
                                    static_cast<int>(wParam));
                    break;

                default:
                    result = originalProc(hWnd, Msg, wParam, lParam);
                    break;
            }
            break;

        case WM_CREATE:
            result = originalProc(hWnd, Msg, wParam, lParam);
            RegisterHotkeys(hWnd);
            break;

        case WM_DESTROY:
            UnregisterHotkeys(hWnd);
            result = originalProc(hWnd, Msg, wParam, lParam);
            break;

        default:
            if (Msg == g_hotkeyRegisteredMsg) {
                switch (wParam) {
                    case HOTKEY_REGISTER:
                        RegisterHotkeys(hWnd);
                        break;

                    case HOTKEY_UNREGISTER:
                        UnregisterHotkeys(hWnd);
                        break;

                    case HOTKEY_UPDATE:
                        UnregisterHotkeys(hWnd);
                        RegisterHotkeys(hWnd);
                        break;
                }
            } else {
                result = originalProc(hWnd, Msg, wParam, lParam);
            }
            break;
    }

    return result;
}

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* flag) {
    if (Msg == WM_MOUSEWHEEL && g_settings.enableMouseWheelCycling) {
        HWND hTaskListWnd = TaskListFromTaskbarWnd(hWnd);

        RECT rc{};
        GetWindowRect(hTaskListWnd, &rc);

        POINT pt{
            .x = GET_X_LPARAM(lParam),
            .y = GET_Y_LPARAM(lParam),
        };

        if (PtInRect(&rc, pt)) {
            short delta = GET_WHEEL_DELTA_WPARAM(wParam);

            // Allows to steal focus.
            INPUT input{};
            SendInput(1, &input, sizeof(INPUT));

            OnTaskListScroll(hTaskListWnd, delta);

            *flag = false;
            return 0;
        }
    }

    LRESULT ret =
        TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);

    return ret;
}

using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (Msg == WM_MOUSEWHEEL && g_settings.enableMouseWheelCycling) {
        HWND hSecondaryTaskListWnd = TaskListFromSecondaryTaskbarWnd(hWnd);

        RECT rc{};
        GetWindowRect(hSecondaryTaskListWnd, &rc);

        POINT pt{
            .x = GET_X_LPARAM(lParam),
            .y = GET_Y_LPARAM(lParam),
        };

        if (PtInRect(&rc, pt)) {
            short delta = GET_WHEEL_DELTA_WPARAM(wParam);

            // Allows to steal focus.
            INPUT input{};
            SendInput(1, &input, sizeof(INPUT));

            OnTaskListScroll(hSecondaryTaskListWnd, delta);

            return 0;
        }
    }

    LRESULT ret =
        CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

    return ret;
}

using TaskbarFrame_OnPointerWheelChanged_t = int(WINAPI*)(PVOID pThis,
                                                          PVOID pArgs);
TaskbarFrame_OnPointerWheelChanged_t
    TaskbarFrame_OnPointerWheelChanged_Original;
int TaskbarFrame_OnPointerWheelChanged_Hook(PVOID pThis, PVOID pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskbarFrame_OnPointerWheelChanged_Original(pThis, pArgs);
    };

    if (!g_settings.enableMouseWheelCycling) {
        return original();
    }

    winrt::Windows::Foundation::IInspectable taskbarFrame = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(
            winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
            winrt::put_abi(taskbarFrame));

    if (!taskbarFrame) {
        return original();
    }

    auto className = winrt::get_class_name(taskbarFrame);
    Wh_Log(L"%s", className.c_str());

    if (className != L"Taskbar.TaskbarFrame") {
        return original();
    }

    auto taskbarFrameElement = taskbarFrame.as<UIElement>();

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    DWORD messagePos = GetMessagePos();
    POINT pt = {GET_X_LPARAM(messagePos), GET_Y_LPARAM(messagePos)};
    HWND hMMTaskListWnd = TaskListFromPoint(pt);
    if (!hMMTaskListWnd) {
        return original();
    }

    auto currentPoint = args.GetCurrentPoint(taskbarFrameElement);
    double delta = currentPoint.Properties().MouseWheelDelta();
    if (!delta) {
        return original();
    }

    // Allows to steal focus.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    OnTaskListScroll(hMMTaskListWnd, static_cast<short>(delta));

    args.Handled(true);
    return 0;
}

void LoadSettings() {
    g_settings.skipMinimizedWindows = Wh_GetIntSetting(L"skipMinimizedWindows");
    g_settings.wrapAround = Wh_GetIntSetting(L"wrapAround");
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.enableMouseWheelCycling =
        Wh_GetIntSetting(L"enableMouseWheelCycling");
    g_settings.cycleLeftKeyboardShortcut =
        WindhawkUtils::StringSetting::make(L"cycleLeftKeyboardShortcut");
    g_settings.cycleRightKeyboardShortcut =
        WindhawkUtils::StringSetting::make(L"cycleRightKeyboardShortcut");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &TaskbarFrame_OnPointerWheelChanged_Original,
            TaskbarFrame_OnPointerWheelChanged_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(const CTaskListWnd::`vftable'{for `ITaskListUI'})"},
            &CTaskListWnd_vftable_ITaskListUI,
        },
        {
            {LR"(const CTaskListWnd::`vftable'{for `ITaskListSite'})"},
            &CTaskListWnd_vftable_ITaskListSite,
        },
        {
            {LR"(const CTaskListWnd::`vftable'{for `ITaskListAcc'})"},
            &CTaskListWnd_vftable_ITaskListAcc,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListWnd::GetButtonGroupCount(void))"},
            &CTaskListWnd_GetButtonGroupCount,
        },
        {
            {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))"},
            &CTaskListWnd__GetTBGroupFromGroup,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType,
        },
        {
            {LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void))"},
            &CTaskBtnGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::SwitchToItem(struct ITaskItem *))"},
            &CTaskListWnd_SwitchToItem_Original,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_SetActiveItem(struct ITaskBtnGroup *,int))"},
            &CTaskListWnd__SetActiveItem_Original,
            CTaskListWnd__SetActiveItem_Hook,
        },
        {
            {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CTaskBand_v_WndProc_Original,
            CTaskBand_v_WndProc_Hook,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(??_7CTaskListWnd@@6BITaskListUI@@@)",
         &CTaskListWnd_vftable_ITaskListUI},
        {R"(??_7CTaskListWnd@@6BITaskListSite@@@)",
         &CTaskListWnd_vftable_ITaskListSite},
        {R"(??_7CTaskListWnd@@6BITaskListAcc@@@)",
         &CTaskListWnd_vftable_ITaskListAcc},
        {R"(??_7CImmersiveTaskItem@@6BITaskItem@@@)",
         &CImmersiveTaskItem_vftable},
        {R"(?GetButtonGroupCount@CTaskListWnd@@UEAAHXZ)",
         &CTaskListWnd_GetButtonGroupCount},
        {R"(?_GetTBGroupFromGroup@CTaskListWnd@@IEAAPEAUITaskBtnGroup@@PEAUITaskGroup@@PEAH@Z)",
         &CTaskListWnd__GetTBGroupFromGroup},
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType},
        {R"(?GetNumItems@CTaskBtnGroup@@UEAAHXZ)", &CTaskBtnGroup_GetNumItems},
        {R"(?GetTaskItem@CTaskBtnGroup@@UEAAPEAUITaskItem@@H@Z)",
         &CTaskBtnGroup_GetTaskItem},
        {R"(?GetWindow@CWindowTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CWindowTaskItem_GetWindow_Original},
        {R"(?GetWindow@CImmersiveTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CImmersiveTaskItem_GetWindow_Original},
        {R"(?SwitchToItem@CTaskListWnd@@UEAAXPEAUITaskItem@@@Z)",
         &CTaskListWnd_SwitchToItem_Original},
        {R"(?_SetActiveItem@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@H@Z)",
         &CTaskListWnd__SetActiveItem_Original,
         CTaskListWnd__SetActiveItem_Hook},
        {R"(?v_WndProc@CTaskBand@@MEAA_JPEAUHWND__@@I_K_J@Z)",
         &CTaskBand_v_WndProc_Original, CTaskBand_v_WndProc_Hook},
        {R"(?WndProc@TrayUI@@UEAA_JPEAUHWND__@@I_K_JPEA_N@Z)",
         &TrayUI_WndProc_Original, TrayUI_WndProc_Hook},
        // Exported after 67.1:
        {R"(?v_WndProc@CSecondaryTray@@EEAA_JPEAUHWND__@@I_K_J@Z)",
         &CSecondaryTray_v_WndProc_Original, CSecondaryTray_v_WndProc_Hook,
         true},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (g_winVersion >= WinVersion::Win11) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                return FALSE;
            }
        } else {
            Wh_Log(L"Taskbar view module not loaded yet");
        }

        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    } else {
        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }

    if (HWND hTaskBandWnd = GetTaskBandWnd()) {
        SendMessage(hTaskBandWnd, g_hotkeyRegisteredMsg, HOTKEY_REGISTER, 0);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (HWND hTaskBandWnd = GetTaskBandWnd()) {
        SendMessage(hTaskBandWnd, g_hotkeyRegisteredMsg, HOTKEY_UNREGISTER, 0);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;
    if (*bReload) {
        return TRUE;
    }

    if (HWND hTaskBandWnd = GetTaskBandWnd()) {
        SendMessage(hTaskBandWnd, g_hotkeyRegisteredMsg, HOTKEY_UPDATE, 0);
    }

    return TRUE;
}
