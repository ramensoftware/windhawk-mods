// ==WindhawkMod==
// @id              explorer-double-f2-rename-extension
// @name            Select filename extension on double F2
// @description     When pressing F2 in Explorer to rename a file, the filename is selected as usual, but double-pressing now selects the extension. Triple F2 selects the full name, quadruple F2 selects the base name again, etc.
// @version         2
// @author          Marnes <leaumar@sent.com>
// @github          https://github.com/leaumar
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
QTTabbar and some other tools provide a feature where double-pressing F2 to
rename a file first selects the base name (existing Explorer behavior) but the
second press selects just the extension. That's handy to e.g. rename a zip file
to a cbz file. This mod implements that same feature: double-press F2
in Explorer to rename a file's extension.

Since version 2, triple-pressing F2 selects the full name. Subsequent (>3)
presses repeat the cycle by selecting the base name again (Explorer default
behavior), then the extension, then the full name, etc.

This mod works great together with
[extension-change-no-warning](https://windhawk.net/mods/extension-change-no-warning).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DoubleF2MilliSeconds: 1000
*/
// ==/WindhawkModSettings==

// I am not an experienced C++ programmer and do not know the Windows APIs.
// This code was cobbled together with a lot trial & error, using input from
// ChatGPT and the Windhawk Discord server.

#include <optional>
#include <string>
#include <unordered_map>

namespace ExplorerUtils {
static std::optional<std::wstring> FindExplorerFileViewClass(HWND windowHandle,
                                                             DWORD threadId) {
    if (windowHandle != nullptr && IsWindow(windowHandle)) {
        wchar_t clazz[64];
        if (GetClassNameW(windowHandle, clazz, _countof(clazz))) {
            // legacy explorer, modern explorer, desktop
            if (_wcsicmp(clazz, L"CabinetWClass") == 0 ||
                _wcsicmp(clazz, L"ExploreWClass") == 0 ||
                _wcsicmp(clazz, L"Progman") == 0) {
                return clazz;
            }
        }
    }
    return std::nullopt;
}

static bool IsEditControl(HWND focus) {
    if (focus == nullptr) {
        return false;
    }
    wchar_t cls[32];
    GetClassNameW(focus, cls, _countof(cls));
    return _wcsicmp(cls, L"Edit") == 0;
}
}  // namespace ExplorerUtils

class Selection {
   private:
    HWND editControl;
    std::wstring text;
    size_t dotIndex;

    Selection(HWND ctrl, std::wstring buffer, size_t dot)
        : editControl(ctrl), text(buffer), dotIndex(dot) {}

   public:
    // default f2 behavior
    // if it's a dotfile or dotless, explorer selects the whole name
    void SelectBaseName() {
        if (dotIndex == std::wstring::npos || dotIndex == 0) {
            SelectWholeName();
        } else {
            SendMessageW(editControl, EM_SETSEL, 0, (int)dotIndex);
            std::wstring base = text.substr(0, (int)dotIndex);
            Wh_Log(L"Selected base name \"%s\".", base.c_str());
        }
    }

    // files normally can't end in a dot, i.e. have extension ""
    void SelectExtension() {
        if (dotIndex == std::wstring::npos) {
            SelectWholeName();
        } else {
            int start = (int)dotIndex + 1;
            SendMessageW(editControl, EM_SETSEL, start, (WPARAM)text.size());
            std::wstring extension = text.substr(start);
            Wh_Log(L"Selected extension \"%s\".", extension.c_str());
        }
    }

    void SelectWholeName() {
        SendMessageW(editControl, EM_SETSEL, 0, (WPARAM)text.size());
        Wh_Log(L"Selected whole name \"%s\".", text.c_str());
    }

    static std::optional<Selection> inside(HWND editControl) {
        // typical max filename length
        std::wstring text(260, L'\0');
        int copied = GetWindowTextW(editControl, text.data(), (int)text.size());
        if (copied > 0) {
            // trim to actual length
            text.resize(copied);
            size_t dotIndex = text.find_last_of(L'.');
            return Selection(editControl, text, dotIndex);
        } else {
            return std::nullopt;
        }
    }
};

class F2Streak {
   private:
    ULONGLONG lastF2Time = 0;
    short streak = 0;

   public:
    short CheckStreak(int nCode, WPARAM wParam, LPARAM lParam) {
        bool shouldProcess = nCode >= 0;
        bool isKeyUp = lParam & 0x80000000;
        if (!shouldProcess || !isKeyUp) {
            return -1;
        }

        bool isF2 = wParam == VK_F2;
        if (!isF2) {
            return streak = 0;
        }

        auto doubleF2Time = (DWORD)Wh_GetIntSetting(L"DoubleF2MilliSeconds");
        doubleF2Time =
            std::max((DWORD)100, std::min(doubleF2Time, (DWORD)10000));
        ULONGLONG now = GetTickCount64();
        auto timeSinceLastF2 = now - lastF2Time;
        lastF2Time = now;

        Wh_Log(L"F2 pressed again after %llums.", timeSinceLastF2);

        bool tooSlow = timeSinceLastF2 > doubleF2Time;
        if (tooSlow) {
            return streak = 1;
        }

        streak += 1;

        Wh_Log(L"F2 streak: %dx.", streak);

        return streak;
    }
};

class KeyboardHooks {
   private:
    std::unordered_map<DWORD, HHOOK> hooks = {};
    HOOKPROC CALLBACK callback;

   public:
    KeyboardHooks(HOOKPROC cb) : callback(cb) {}

    bool Attach(DWORD threadId) {
        if (threadId == 0) {
            Wh_Log(L"Refusing keyboard hook on bad thread id.");
            return false;
        }

        if (hooks.find(threadId) != hooks.end()) {
            Wh_Log(L"Thread %u already has a keyboard hook, skipping.",
                   threadId);
            return false;
        }

        HHOOK hook =
            SetWindowsHookExW(WH_KEYBOARD, callback, nullptr, threadId);

        if (hook == nullptr) {
            Wh_Log(L"Failed to hook thread %u.", threadId);
            return false;
        }

        hooks[threadId] = hook;
        Wh_Log(L"Installed keyboard hook %p on thread %u.", hook, threadId);
        return true;
    }

    void DetachAll() {
        for (auto& [threadId, hook] : hooks) {
            if (hook != nullptr) {
                bool ok = UnhookWindowsHookEx(hook);
                Wh_Log(L"Unhook %p -> %d.", hook, ok);
            }
        }
        hooks.clear();
    }
};

namespace WindowCreatedHook {
// window handle, thread id
using HookCallback = void (*)(HWND, DWORD);

static inline HookCallback callback = nullptr;

static inline HWND(WINAPI* previousHandleWindowCreated)(DWORD dwExStyle,
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
                                                        LPVOID lpParam) =
    nullptr;

static HWND WINAPI HandleWindowCreated(DWORD dwExStyle,
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
    // always call the original first
    HWND hwnd = previousHandleWindowCreated(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);
    auto threadId = GetCurrentThreadId();
    callback(hwnd, threadId);
    return hwnd;
}

inline void Attach(HookCallback cb) {
    if (callback != nullptr) {
        return;
    }
    callback = cb;
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)HandleWindowCreated,
                       (void**)&previousHandleWindowCreated);
}
}  // namespace WindowCreatedHook

static F2Streak f2Streak;

static LRESULT CALLBACK HandleKeyEvent(int nCode,
                                       WPARAM wParam,
                                       LPARAM lParam) {
    auto f2Count = f2Streak.CheckStreak(nCode, wParam, lParam);
    if (f2Count > 1) {
        HWND focus = GetFocus();
        if (ExplorerUtils::IsEditControl(focus)) {
            Wh_Log(L"%d times F2 in edit control, applying selection.",
                   f2Count);
            auto selection = Selection::inside(focus);
            if (selection.has_value()) {
                auto timesF2 = f2Count % 3;
                switch (timesF2) {
                    // standard explorer f2
                    case 1:
                        selection.value().SelectBaseName();
                        break;
                    // original feature: double f2 = extension
                    case 2:
                        selection.value().SelectExtension();
                        break;
                    // new: triple f2 = whole name
                    case 0:
                        selection.value().SelectWholeName();
                        break;
                }
                return 0;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static KeyboardHooks keyboardHooks(HandleKeyEvent);

static void HookIfExplorerFileView(HWND windowHandle, DWORD threadId) {
    auto clazz =
        ExplorerUtils::FindExplorerFileViewClass(windowHandle, threadId);
    if (clazz.has_value()) {
        keyboardHooks.Attach(threadId);
        Wh_Log(L"Hooked Explorer window: hwnd=0x%p class=%s.", windowHandle,
               clazz.value().c_str());
    }
}

static BOOL CALLBACK HandleWindowEnumerated(HWND hwnd, LPARAM lParam) {
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    bool isExplorer = processId == GetCurrentProcessId();

    if (isExplorer) {
        HookIfExplorerFileView(hwnd, threadId);
    }

    // continue enumeration
    return true;
}

void Wh_ModInit() {
    Wh_Log(L"Hooking the desktop (shell) thread.");
    DWORD shellThreadId = GetWindowThreadProcessId(GetShellWindow(), nullptr);
    keyboardHooks.Attach(shellThreadId);

    Wh_Log(L"Hooking already open Explorer windows.");
    EnumWindows(HandleWindowEnumerated, 0);

    Wh_Log(L"Hooking Explorer window creation.");
    WindowCreatedHook::Attach(HookIfExplorerFileView);
}

void Wh_ModUninit() {
    Wh_Log(L"Removing all keyboard hooks.");
    keyboardHooks.DetachAll();
}
