// ==WindhawkMod==
// @id              taskbar-wheel-cycle
// @name            Cycle taskbar buttons with mouse wheel
// @description     Use the mouse wheel while hovering over the taskbar to cycle between taskbar buttons (Windows 11 only)
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32
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

![Demonstration](https://i.imgur.com/FtpUjt1.gif)

Only Windows 11 is currently supported. For older Windows versions check out [7+
Taskbar Tweaker](https://tweaker.ramensoftware.com/).
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
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <commctrl.h>
#include <windowsx.h>

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>

#include <algorithm>
#include <atomic>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

struct {
    bool skipMinimizedWindows;
    bool wrapAround;
    bool reverseScrollingDirection;
} g_settings;

HWND g_lastScrollTarget = nullptr;
DWORD g_lastScrollTime;
short g_lastScrollDeltaRemainder;

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

void* CTaskListWnd__GetRequiredCols;
void* CTaskListWnd__FixupTaskIndicies;

size_t OffsetFromAssembly(void* func,
                          size_t defValue,
                          std::string opcode = "mov",
                          int limit = 30) {
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

        // Example: mov rax, [rcx+0xE0]
        std::regex regex(
            opcode +
            R"( r(?:[a-z]{2}|\d{1,2}), \[r(?:[a-z]{2}|\d{1,2})\+(0x[0-9A-F]+)\])");
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
    static size_t offset =
        OffsetFromAssembly(CTaskListWnd__GetRequiredCols, 0xE0);

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
                        int* p_button_index,
                        int* p_buttons_count,
                        LONG_PTR*** p_buttons) {
    int button_group_index = *p_button_group_index;
    int button_index = *p_button_index;
    int buttons_count = *p_buttons_count;
    LONG_PTR** buttons = *p_buttons;
    LONG_PTR* plp;
    int button_group_type;

    if (button_group_index == -1 || ++button_index >= buttons_count) {
        do {
            button_group_index++;
            if (button_group_index >= button_groups_count)
                return FALSE;

            button_group_type = (int)button_groups[button_group_index][8];
        } while (button_group_type != 1 && button_group_type != 3);

        plp = (LONG_PTR*)button_groups[button_group_index][7];
        buttons_count = (int)plp[0];
        buttons = (LONG_PTR**)plp[1];

        button_index = 0;
    }

    *p_button_group_index = button_group_index;
    *p_button_index = button_index;
    *p_buttons_count = buttons_count;
    *p_buttons = buttons;

    return TRUE;
}

BOOL TaskbarScrollLeft(int button_groups_count,
                       LONG_PTR** button_groups,
                       int* p_button_group_index,
                       int* p_button_index,
                       int* p_buttons_count,
                       LONG_PTR*** p_buttons) {
    int button_group_index = *p_button_group_index;
    int button_index = *p_button_index;
    int buttons_count = *p_buttons_count;
    LONG_PTR** buttons = *p_buttons;
    LONG_PTR* plp;
    int button_group_type;

    if (button_group_index == -1 || --button_index < 0) {
        if (button_group_index == -1)
            button_group_index = button_groups_count;

        do {
            button_group_index--;
            if (button_group_index < 0)
                return FALSE;

            button_group_type = (int)button_groups[button_group_index][8];
        } while (button_group_type != 1 && button_group_type != 3);

        plp = (LONG_PTR*)button_groups[button_group_index][7];
        buttons_count = (int)plp[0];
        buttons = (LONG_PTR**)plp[1];

        button_index = buttons_count - 1;
    }

    *p_button_group_index = button_group_index;
    *p_buttons_count = buttons_count;
    *p_buttons = buttons;
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
    LONG_PTR* plp;
    int buttons_count;
    LONG_PTR** buttons;
    BOOL bScrollSucceeded;

    button_group_index = button_group_index_active;
    button_index = button_index_active;

    if (button_group_index != -1) {
        plp = (LONG_PTR*)button_groups[button_group_index][7];
        buttons_count = (int)plp[0];
        buttons = (LONG_PTR**)plp[1];
    }

    bRotateRight = TRUE;
    if (nRotates < 0) {
        bRotateRight = FALSE;
        nRotates = -nRotates;
    }

    prev_button_group_index = button_group_index;
    prev_button_index = button_index;

    while (nRotates--) {
        if (bRotateRight) {
            bScrollSucceeded = TaskbarScrollRight(
                button_groups_count, button_groups, &button_group_index,
                &button_index, &buttons_count, &buttons);
            while (bScrollSucceeded && bSkipMinimized &&
                   IsMinimizedTaskItem((LONG_PTR*)buttons[button_index][4])) {
                bScrollSucceeded = TaskbarScrollRight(
                    button_groups_count, button_groups, &button_group_index,
                    &button_index, &buttons_count, &buttons);
            }
        } else {
            bScrollSucceeded = TaskbarScrollLeft(
                button_groups_count, button_groups, &button_group_index,
                &button_index, &buttons_count, &buttons);
            while (bScrollSucceeded && bSkipMinimized &&
                   IsMinimizedTaskItem((LONG_PTR*)buttons[button_index][4])) {
                bScrollSucceeded = TaskbarScrollLeft(
                    button_groups_count, button_groups, &button_group_index,
                    &button_index, &buttons_count, &buttons);
            }
        }

        if (!bScrollSucceeded) {
            // If no results were found in the whole taskbar
            if (prev_button_group_index == -1) {
                return NULL;
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

                plp = (LONG_PTR*)button_groups[button_group_index][7];
                buttons_count = (int)plp[0];
                buttons = (LONG_PTR**)plp[1];

                break;
            }
        }

        prev_button_group_index = button_group_index;
        prev_button_index = button_index;
    }

    if (button_group_index == button_group_index_active &&
        button_index == button_index_active)
        return NULL;

    return (LONG_PTR*)buttons[button_index][4];
}

LONG_PTR* TaskbarScroll(LONG_PTR lpMMTaskListLongPtr,
                        int nRotates,
                        BOOL bSkipMinimized,
                        BOOL bWarpAround,
                        LONG_PTR* src_task_item) {
    LONG_PTR* button_group_active;
    int button_group_index_active, button_index_active;
    LONG_PTR* plp;
    int button_groups_count;
    LONG_PTR** button_groups;
    int button_group_type;
    int buttons_count;
    LONG_PTR** buttons;
    int i, j;

    if (nRotates == 0)
        return NULL;

    plp = (LONG_PTR*)*EV_MM_TASKLIST_BUTTON_GROUPS_HDPA(lpMMTaskListLongPtr);
    if (!plp)
        return NULL;

    button_groups_count = (int)plp[0];
    button_groups = (LONG_PTR**)plp[1];

    if (src_task_item) {
        for (i = 0; i < button_groups_count; i++) {
            button_group_type = (int)button_groups[i][8];
            if (button_group_type == 1 || button_group_type == 3) {
                plp = (LONG_PTR*)button_groups[i][7];
                buttons_count = (int)plp[0];
                buttons = (LONG_PTR**)plp[1];

                for (j = 0; j < buttons_count; j++) {
                    if ((LONG_PTR*)buttons[j][4] == src_task_item) {
                        button_group_index_active = i;
                        button_index_active = j;
                        break;
                    }
                }

                if (j < buttons_count)
                    break;
            }
        }

        if (i == button_groups_count) {
            button_group_index_active = -1;
            button_index_active = -1;
        }
    } else {
        button_group_active =
            *EV_MM_TASKLIST_ACTIVE_BUTTON_GROUP(lpMMTaskListLongPtr);
        button_index_active =
            *EV_MM_TASKLIST_ACTIVE_BUTTON_INDEX(lpMMTaskListLongPtr);

        if (button_group_active && button_index_active >= 0) {
            for (i = 0; i < button_groups_count; i++) {
                if (button_groups[i] == button_group_active) {
                    button_group_index_active = i;
                    break;
                }
            }

            if (i == button_groups_count)
                return NULL;
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
        HWND hReBarWindow32 =
            FindWindowEx(hRootWnd, nullptr, L"ReBarWindow32", nullptr);
        if (hReBarWindow32) {
            HWND hMSTaskSwWClass = FindWindowEx(hReBarWindow32, nullptr,
                                                L"MSTaskSwWClass", nullptr);
            if (hMSTaskSwWClass) {
                return FindWindowEx(hMSTaskSwWClass, nullptr,
                                    L"MSTaskListWClass", nullptr);
            }
        }
    } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
        HWND hWorkerWWnd = FindWindowEx(hRootWnd, nullptr, L"WorkerW", nullptr);
        if (hWorkerWWnd) {
            return FindWindowEx(hWorkerWWnd, nullptr, L"MSTaskListWClass",
                                nullptr);
        }
    }

    return nullptr;
}

// {7C3E0575-EB65-5A36-B1CF-8322C06C53C3}
constexpr winrt::guid ITaskListButton{
    0x7C3E0575,
    0xEB65,
    0x5A36,
    {0xB1, 0xCF, 0x83, 0x22, 0xC0, 0x6C, 0x53, 0xC3}};

using TaskListButton_OnPointerWheelChanged_t = int(WINAPI*)(PVOID pThis,
                                                            PVOID pArgs);
TaskListButton_OnPointerWheelChanged_t
    TaskListButton_OnPointerWheelChanged_Original;
int TaskListButton_OnPointerWheelChanged_Hook(PVOID pThis, PVOID pArgs) {
    Wh_Log(L">");

    auto original = [&]() {
        return TaskListButton_OnPointerWheelChanged_Original(pThis, pArgs);
    };

    winrt::Windows::Foundation::IUnknown taskListButton = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(ITaskListButton, winrt::put_abi(taskListButton));
    if (!taskListButton) {
        return original();
    }

    auto taskListButtonElement = taskListButton.as<UIElement>();

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

    auto currentPoint = args.GetCurrentPoint(taskListButtonElement);
    double delta = currentPoint.Properties().MouseWheelDelta();
    if (!delta) {
        return original();
    }

    // Allows to steal focus.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    OnTaskListScroll(hMMTaskListWnd, static_cast<short>(delta));

    return 0;
}

struct SYMBOL_HOOK {
    std::vector<std::wstring_view> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 const SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount) {
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
            }
        }

        if (std::all_of(symbolResolved.begin(), symbolResolved.end(),
                        [](bool b) { return b; })) {
            return true;
        }
    }

    Wh_Log(L"Couldn't resolve all symbols from cache");

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

void LoadSettings() {
    g_settings.skipMinimizedWindows = Wh_GetIntSetting(L"skipMinimizedWindows");
    g_settings.wrapAround = Wh_GetIntSetting(L"wrapAround");
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
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
    if (module) {
        Wh_Log(L"Taskbar view module couldn't be loaded");
    }

    SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            (void**)&TaskListButton_OnPointerWheelChanged_Original,
            (void*)TaskListButton_OnPointerWheelChanged_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

BOOL HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return FALSE;
    }

    SYMBOL_HOOK symbolHooks[] = {
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
            {LR"(protected: int __cdecl CTaskListWnd::_GetRequiredCols(int))"},
            (void**)&CTaskListWnd__GetRequiredCols,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_FixupTaskIndicies(struct ITaskBtnGroup *,int,int))"},
            (void**)&CTaskListWnd__FixupTaskIndicies,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
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

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
