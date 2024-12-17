// ==WindhawkMod==
// @id              taskbar-wheel-cycle
// @name            Cycle taskbar buttons with mouse wheel
// @description     Use the mouse wheel while hovering over the taskbar to cycle between taskbar buttons (Windows 11 only)
// @version         1.1.5
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -loleaut32 -lole32 -lruntimeobject -lwininet
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

Only Windows 11 is currently supported. For older Windows versions check out [7+
Taskbar Tweaker](https://tweaker.ramensoftware.com/).

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
- cycleLeftKeyboardShortcut: Alt+VK_OEM_4
  $name: Cycle left keyboard shortcut
  $description: >-
    Possible modifier keys: Alt, Ctrl, Shift, Win. For possible shortcut keys,
    refer to the following page:
    https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
- cycleRightKeyboardShortcut: Alt+VK_OEM_6
  $name: Cycle right keyboard shortcut
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <commctrl.h>
#include <windowsx.h>
#include <wininet.h>

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>

#include <algorithm>
#include <memory>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

struct {
    bool skipMinimizedWindows;
    bool wrapAround;
    bool reverseScrollingDirection;
    string_setting_unique_ptr cycleLeftKeyboardShortcut;
    string_setting_unique_ptr cycleRightKeyboardShortcut;
} g_settings;

HWND g_lastScrollTarget = nullptr;
DWORD g_lastScrollTime;
short g_lastScrollDeltaRemainder;

HWND g_hTaskbarWnd;
bool g_hotkeyLeftRegistered = false;
bool g_hotkeyRightRegistered = false;

enum {
    kHotkeyIdLeft = 1682530408,  // From epochconverter.com
    kHotkeyIdRight,
};

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

void* CImmersiveTaskItem_vftable;

using CTaskBand_SwitchTo_t =
    HRESULT(WINAPI*)(PVOID pThis,
                     PVOID taskItem,
                     BOOL trueMeansBringToFrontFalseMeansToggleMinimizeRestore);
CTaskBand_SwitchTo_t CTaskBand_SwitchTo_Original;

#pragma region offsets

void* CTaskListWnd_GetFocusedBtn;
void* CTaskListWnd__FixupTaskIndicies;

size_t OffsetFromAssembly(void* func,
                          size_t defValue,
                          std::string opcode = "mov",
                          int limit = 30) {
    // Example: mov rax, [rcx+0xE0]
    std::regex regex(
        opcode +
        R"( r(?:[a-z]{2}|\d{1,2}), \[r(?:[a-z]{2}|\d{1,2})\+(0x[0-9A-F]+)\])");

    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Failed for %p", func);
    return defValue;
}

HDPA* EV_MM_TASKLIST_BUTTON_GROUPS_HDPA(LONG_PTR lp) {
    static size_t offset = OffsetFromAssembly(CTaskListWnd_GetFocusedBtn, 0xE0);

    return (HDPA*)(lp + offset);
}

LONG_PTR** EV_MM_TASKLIST_ACTIVE_BUTTON_GROUP(LONG_PTR lp) {
    static size_t offset =
        OffsetFromAssembly(CTaskListWnd__FixupTaskIndicies, 0x130, "cmp");

    return (LONG_PTR**)(lp + offset);
}

int* EV_MM_TASKLIST_ACTIVE_BUTTON_INDEX(LONG_PTR lp) {
    return (int*)(EV_MM_TASKLIST_ACTIVE_BUTTON_GROUP(lp) + 1);
}

#pragma endregion  // offsets

#pragma region scroll

PVOID GetTaskBand() {
    static PVOID taskBand = nullptr;
    if (taskBand) {
        return taskBand;
    }

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    DWORD processId = 0;
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &processId) &&
        processId == GetCurrentProcessId()) {
        HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
        if (hTaskSwWnd) {
            taskBand = (PVOID)GetWindowLongPtr(hTaskSwWnd, 0);
        }
    }

    return taskBand;
}

void SwitchToTaskItem(PVOID taskItem) {
    PVOID taskBand = GetTaskBand();
    if (!taskBand) {
        return;
    }

    CTaskBand_SwitchTo_Original((BYTE*)taskBand + 0x48, taskItem, TRUE);
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

LONG_PTR* TaskbarScroll(LONG_PTR lpMMTaskListLongPtr,
                        int nRotates,
                        BOOL bSkipMinimized,
                        BOOL bWarpAround,
                        LONG_PTR* src_task_item) {
    if (nRotates == 0) {
        return nullptr;
    }

    LONG_PTR* plp =
        (LONG_PTR*)*EV_MM_TASKLIST_BUTTON_GROUPS_HDPA(lpMMTaskListLongPtr);
    if (!plp) {
        return nullptr;
    }

    int button_groups_count = (int)plp[0];
    LONG_PTR** button_groups = (LONG_PTR**)plp[1];

    int button_group_index_active, button_index_active;

    if (src_task_item) {
        int i;
        for (i = 0; i < button_groups_count; i++) {
            int button_group_type =
                CTaskBtnGroup_GetGroupType(button_groups[i]);
            if (button_group_type == 1 || button_group_type == 3) {
                int buttons_count = CTaskBtnGroup_GetNumItems(button_groups[i]);

                int j;
                for (j = 0; j < buttons_count; j++) {
                    if ((LONG_PTR*)CTaskBtnGroup_GetTaskItem(
                            button_groups[i], j) == src_task_item) {
                        button_group_index_active = i;
                        button_index_active = j;
                        break;
                    }
                }

                if (j < buttons_count) {
                    break;
                }
            }
        }

        if (i == button_groups_count) {
            button_group_index_active = -1;
            button_index_active = -1;
        }
    } else {
        LONG_PTR* button_group_active =
            *EV_MM_TASKLIST_ACTIVE_BUTTON_GROUP(lpMMTaskListLongPtr);
        button_index_active =
            *EV_MM_TASKLIST_ACTIVE_BUTTON_INDEX(lpMMTaskListLongPtr);

        if (button_group_active && button_index_active >= 0) {
            int i;
            for (i = 0; i < button_groups_count; i++) {
                if (button_groups[i] == button_group_active) {
                    button_group_index_active = i;
                    break;
                }
            }

            if (i == button_groups_count) {
                return nullptr;
            }
        } else {
            button_group_index_active = -1;
            button_index_active = -1;
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
            SwitchToTaskItem(targetTaskItem);
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

HWND TaskListFromPoint(POINT pt) {
    HWND hPointWnd = WindowFromPoint(pt);
    if (!hPointWnd) {
        return nullptr;
    }

    HWND hRootWnd = GetAncestor(hPointWnd, GA_ROOT);
    if (!hRootWnd) {
        return nullptr;
    }

    WCHAR szClassName[32];
    if (!GetClassName(hRootWnd, szClassName, ARRAYSIZE(szClassName))) {
        return nullptr;
    }

    if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
        return TaskListFromTaskbarWnd(hRootWnd);
    }

    if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
        return TaskListFromSecondaryTaskbarWnd(hRootWnd);
    }

    return nullptr;
}

using TaskbarFrame_OnPointerWheelChanged_t = int(WINAPI*)(PVOID pThis,
                                                          PVOID pArgs);
TaskbarFrame_OnPointerWheelChanged_t
    TaskbarFrame_OnPointerWheelChanged_Original;
int TaskbarFrame_OnPointerWheelChanged_Hook(PVOID pThis, PVOID pArgs) {
    Wh_Log(L">");

    auto original = [&]() {
        return TaskbarFrame_OnPointerWheelChanged_Original(pThis, pArgs);
    };

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

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_" WH_MOD_ID);

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
        SUBCLASSPROC pfnSubclass;
        UINT_PTR uIdSubclass;
        DWORD_PTR dwRefData;
        BOOL result;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
                    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                        (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
                    param->result =
                        SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                          param->uIdSubclass, param->dwRefData);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
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

    HWND hTaskListWnd = TaskListFromTaskbarWnd(hWnd);
    if (!hTaskListWnd) {
        return false;
    }

    int clicks = hotkeyId == kHotkeyIdLeft ? -1 : 1;

    LONG_PTR lpTaskListLongPtr = GetWindowLongPtr(hTaskListWnd, 0);
    PVOID targetTaskItem = TaskbarScroll(lpTaskListLongPtr, clicks,
                                         g_settings.skipMinimizedWindows,
                                         g_settings.wrapAround, nullptr);
    if (targetTaskItem) {
        SwitchToTaskItem(targetTaskItem);
    }

    return true;
}

UINT g_hotkeyUpdatedRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_hotkeyUpdated_" WH_MOD_ID);

LRESULT CALLBACK TaskbarWindowSubclassProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           UINT_PTR uIdSubclass,
                                           DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
    }

    LRESULT result = 0;

    switch (uMsg) {
        case WM_HOTKEY:
            switch (wParam) {
                case kHotkeyIdLeft:
                case kHotkeyIdRight:
                    OnTaskbarHotkey(hWnd, static_cast<int>(wParam));
                    break;

                default:
                    result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    break;
            }
            break;

        default:
            if (uMsg == g_subclassRegisteredMsg) {
                if (wParam) {
                    RegisterHotkeys(hWnd);
                } else {
                    UnregisterHotkeys(hWnd);
                }
            } else if (uMsg == g_hotkeyUpdatedRegisteredMsg) {
                UnregisterHotkeys(hWnd);
                RegisterHotkeys(hWnd);
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;
    }

    return result;
}

void SubclassTaskbarWindow(HWND hWnd) {
    SetWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc, 0, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd) {
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    SubclassTaskbarWindow(hWnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
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
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    }

    return hWnd;
}

struct SYMBOL_HOOK {
    std::vector<std::wstring_view> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 const SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount,
                 bool cacheOnly = false) {
    const WCHAR cacheVer = L'1';
    const WCHAR cacheSep = L'#';
    constexpr size_t cacheMaxSize = 10240;

    WCHAR moduleFilePath[MAX_PATH];
    if (!GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned an unsupported path");
        return false;
    }

    moduleFileName++;

    WCHAR cacheBuffer[cacheMaxSize + 1];
    std::wstring cacheStrKey = std::wstring(L"symbol-cache-") + moduleFileName;
    Wh_GetStringValue(cacheStrKey.c_str(), cacheBuffer, ARRAYSIZE(cacheBuffer));

    std::wstring_view cacheBufferView(cacheBuffer);

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

    auto cacheParts = splitStringView(cacheBufferView, cacheSep);

    std::vector<bool> symbolResolved(symbolHooksCount, false);
    std::wstring newSystemCacheStr;

    auto onSymbolResolved = [symbolHooks, symbolHooksCount, &symbolResolved,
                             &newSystemCacheStr,
                             module](std::wstring_view symbol, void* address) {
        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i]) {
                continue;
            }

            bool match = false;
            for (auto hookSymbol : symbolHooks[i].symbols) {
                if (hookSymbol == symbol) {
                    match = true;
                    break;
                }
            }

            if (!match) {
                continue;
            }

            if (symbolHooks[i].hookFunction) {
                Wh_SetFunctionHook(address, symbolHooks[i].hookFunction,
                                   symbolHooks[i].pOriginalFunction);
                Wh_Log(L"Hooked %p: %.*s", address, symbol.length(),
                       symbol.data());
            } else {
                *symbolHooks[i].pOriginalFunction = address;
                Wh_Log(L"Found %p: %.*s", address, symbol.length(),
                       symbol.data());
            }

            symbolResolved[i] = true;

            newSystemCacheStr += cacheSep;
            newSystemCacheStr += symbol;
            newSystemCacheStr += cacheSep;
            newSystemCacheStr +=
                std::to_wstring((ULONG_PTR)address - (ULONG_PTR)module);

            break;
        }
    };

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    newSystemCacheStr += cacheVer;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += timeStamp;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += imageSize;

    if (cacheParts.size() >= 3 &&
        cacheParts[0] == std::wstring_view(&cacheVer, 1) &&
        cacheParts[1] == timeStamp && cacheParts[2] == imageSize) {
        for (size_t i = 3; i + 1 < cacheParts.size(); i += 2) {
            auto symbol = cacheParts[i];
            auto address = cacheParts[i + 1];
            if (address.length() == 0) {
                continue;
            }

            void* addressPtr =
                (void*)(std::stoull(std::wstring(address), nullptr, 10) +
                        (ULONG_PTR)module);

            onSymbolResolved(symbol, addressPtr);
        }

        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i] || !symbolHooks[i].optional) {
                continue;
            }

            size_t noAddressMatchCount = 0;
            for (size_t j = 3; j + 1 < cacheParts.size(); j += 2) {
                auto symbol = cacheParts[j];
                auto address = cacheParts[j + 1];
                if (address.length() != 0) {
                    continue;
                }

                for (auto hookSymbol : symbolHooks[i].symbols) {
                    if (hookSymbol == symbol) {
                        noAddressMatchCount++;
                        break;
                    }
                }
            }

            if (noAddressMatchCount == symbolHooks[i].symbols.size()) {
                Wh_Log(L"Optional symbol %d doesn't exist (from cache)", i);

                symbolResolved[i] = true;

                for (auto hookSymbol : symbolHooks[i].symbols) {
                    newSystemCacheStr += cacheSep;
                    newSystemCacheStr += hookSymbol;
                    newSystemCacheStr += cacheSep;
                }
            }
        }

        if (std::all_of(symbolResolved.begin(), symbolResolved.end(),
                        [](bool b) { return b; })) {
            return true;
        }
    }

    Wh_Log(L"Couldn't resolve all symbols from cache");

    if (cacheOnly) {
        return false;
    }

    WH_FIND_SYMBOL findSymbol;
    HANDLE findSymbolHandle = Wh_FindFirstSymbol(module, nullptr, &findSymbol);
    if (!findSymbolHandle) {
        Wh_Log(L"Wh_FindFirstSymbol failed");
        return false;
    }

    do {
        onSymbolResolved(findSymbol.symbol, findSymbol.address);
    } while (Wh_FindNextSymbol(findSymbolHandle, &findSymbol));

    Wh_FindCloseSymbol(findSymbolHandle);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (symbolResolved[i]) {
            continue;
        }

        if (!symbolHooks[i].optional) {
            Wh_Log(L"Unresolved symbol: %d", i);
            return false;
        }

        Wh_Log(L"Optional symbol %d doesn't exist", i);

        for (auto hookSymbol : symbolHooks[i].symbols) {
            newSystemCacheStr += cacheSep;
            newSystemCacheStr += hookSymbol;
            newSystemCacheStr += cacheSep;
        }
    }

    if (newSystemCacheStr.length() <= cacheMaxSize) {
        Wh_SetStringValue(cacheStrKey.c_str(), newSystemCacheStr.c_str());
    } else {
        Wh_Log(L"Cache is too large (%zu)", newSystemCacheStr.length());
    }

    return true;
}

std::optional<std::wstring> GetUrlContent(PCWSTR lpUrl) {
    HINTERNET hOpenHandle = InternetOpen(
        L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hOpenHandle) {
        return std::nullopt;
    }

    HINTERNET hUrlHandle =
        InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
                        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
                        0);
    if (!hUrlHandle) {
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    if (!HttpQueryInfo(hUrlHandle,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &dwStatusCode, &dwStatusCodeSize, nullptr) ||
        dwStatusCode != 200) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    LPBYTE pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (!pUrlContent) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwNumberOfBytesRead;
    InternetReadFile(hUrlHandle, pUrlContent, 0x400, &dwNumberOfBytesRead);
    DWORD dwLength = dwNumberOfBytesRead;

    while (dwNumberOfBytesRead) {
        LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(
            GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
        if (!pNewUrlContent) {
            InternetCloseHandle(hUrlHandle);
            InternetCloseHandle(hOpenHandle);
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        pUrlContent = pNewUrlContent;
        InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400,
                         &dwNumberOfBytesRead);
        dwLength += dwNumberOfBytesRead;
    }

    InternetCloseHandle(hUrlHandle);
    InternetCloseHandle(hOpenHandle);

    // Assume UTF-8.
    int charsNeeded = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent,
                                          dwLength, nullptr, 0);
    std::wstring unicodeContent(charsNeeded, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent, dwLength,
                        unicodeContent.data(), unicodeContent.size());

    HeapFree(GetProcessHeap(), 0, pUrlContent);

    return unicodeContent;
}

bool HookSymbolsWithOnlineCacheFallback(HMODULE module,
                                        const SYMBOL_HOOK* symbolHooks,
                                        size_t symbolHooksCount) {
    constexpr WCHAR kModIdForCache[] = L"taskbar-wheel-cycle";

    if (HookSymbols(module, symbolHooks, symbolHooksCount,
                    /*cacheOnly=*/true)) {
        return true;
    }

    Wh_Log(L"HookSymbols() from cache failed, trying to get an online cache");

    WCHAR moduleFilePath[MAX_PATH];
    DWORD moduleFilePathLen =
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath));
    if (!moduleFilePathLen || moduleFilePathLen == ARRAYSIZE(moduleFilePath)) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned unsupported path");
        return false;
    }

    moduleFileName++;

    DWORD moduleFileNameLen =
        moduleFilePathLen - (moduleFileName - moduleFilePath);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, moduleFileName,
                  moduleFileNameLen, moduleFileName, moduleFileNameLen, nullptr,
                  nullptr, 0);

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    std::wstring cacheStrKey =
#if defined(_M_IX86)
        L"symbol-x86-cache-";
#elif defined(_M_X64)
        L"symbol-cache-";
#else
#error "Unsupported architecture"
#endif
    cacheStrKey += moduleFileName;

    std::wstring onlineCacheUrl =
        L"https://ramensoftware.github.io/windhawk-mod-symbol-cache/";
    onlineCacheUrl += kModIdForCache;
    onlineCacheUrl += L'/';
    onlineCacheUrl += cacheStrKey;
    onlineCacheUrl += L'/';
    onlineCacheUrl += timeStamp;
    onlineCacheUrl += L'-';
    onlineCacheUrl += imageSize;
    onlineCacheUrl += L".txt";

    Wh_Log(L"Looking for an online cache at %s", onlineCacheUrl.c_str());

    auto onlineCache = GetUrlContent(onlineCacheUrl.c_str());
    if (onlineCache) {
        Wh_SetStringValue(cacheStrKey.c_str(), onlineCache->c_str());
    } else {
        Wh_Log(L"Failed to get online cache");
    }

    return HookSymbols(module, symbolHooks, symbolHooksCount);
}

void LoadSettings() {
    g_settings.skipMinimizedWindows = Wh_GetIntSetting(L"skipMinimizedWindows");
    g_settings.wrapAround = Wh_GetIntSetting(L"wrapAround");
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.cycleLeftKeyboardShortcut.reset(
        Wh_GetStringSetting(L"cycleLeftKeyboardShortcut"));
    g_settings.cycleRightKeyboardShortcut.reset(
        Wh_GetStringSetting(L"cycleRightKeyboardShortcut"));
}

bool GetTaskbarViewDllPath(WCHAR path[MAX_PATH]) {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    // Windows 11 version 22H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    // Windows 11 version 21H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    return false;
}

bool HookTaskbarViewDllSymbols() {
    WCHAR dllPath[MAX_PATH];
    if (!GetTaskbarViewDllPath(dllPath)) {
        Wh_Log(L"Taskbar view module not found");
        return false;
    }

    HMODULE module =
        LoadLibraryEx(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module) {
        Wh_Log(L"Taskbar view module couldn't be loaded");
        return false;
    }

    // Taskbar.View.dll, ExplorerExtensions.dll
    SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            (void**)&TaskbarFrame_OnPointerWheelChanged_Original,
            (void*)TaskbarFrame_OnPointerWheelChanged_Hook,
        },
    };

    return HookSymbolsWithOnlineCacheFallback(module, symbolHooks,
                                              ARRAYSIZE(symbolHooks));
}

BOOL HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return FALSE;
    }

    SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            (void**)&CTaskBtnGroup_GetGroupType,
        },
        {
            {LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void))"},
            (void**)&CTaskBtnGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            (void**)&CTaskBtnGroup_GetTaskItem,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            (void**)&CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            (void**)&CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            (void**)&CImmersiveTaskItem_vftable,
        },
        {
            {LR"(public: virtual long __cdecl CTaskBand::SwitchTo(struct ITaskItem *,int))"},
            (void**)&CTaskBand_SwitchTo_Original,
        },
        // For offsets:
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::GetFocusedBtn(struct ITaskGroup * *,int *))"},
            (void**)&CTaskListWnd_GetFocusedBtn,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_FixupTaskIndicies(struct ITaskBtnGroup *,int,int))"},
            (void**)&CTaskListWnd__FixupTaskIndicies,
        },
    };

    return HookSymbolsWithOnlineCacheFallback(module, taskbarDllHooks,
                                              ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarViewDllSymbols()) {
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    DWORD dwProcessId;
    DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) &&
        dwProcessId == dwCurrentProcessId) {
        Wh_Log(L"Taskbar window found: %08X", (DWORD)(ULONG_PTR)hTaskbarWnd);
        HandleIdentifiedTaskbarWindow(hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);
    }

    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (g_hTaskbarWnd) {
        PostMessage(g_hTaskbarWnd, g_hotkeyUpdatedRegisteredMsg, 0, 0);
    }
}
