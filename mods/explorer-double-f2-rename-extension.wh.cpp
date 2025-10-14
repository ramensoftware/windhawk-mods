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
static std::optional<std::wstring> FindWindowClassName(HWND windowHandle) {
    // just in case the window closed in the meantime
    if (IsWindow(windowHandle)) {
        wchar_t clazz[64];
        if (GetClassNameW(windowHandle, clazz, _countof(clazz)) > 0) {
            return clazz;
        }
    }
    return std::nullopt;
}

static bool IsFileView(std::wstring windowClass) {
    // legacy explorer, modern explorer, desktop
    return _wcsicmp(windowClass.c_str(), L"CabinetWClass") == 0 ||
           _wcsicmp(windowClass.c_str(), L"ExploreWClass") == 0 ||
           _wcsicmp(windowClass.c_str(), L"Progman") == 0;
}

static bool IsEditControl(HWND focus) {
    wchar_t cls[32];
    GetClassNameW(focus, cls, _countof(cls));
    return _wcsicmp(cls, L"Edit") == 0;
}
}  // namespace ExplorerUtils

namespace ModSettings {
static unsigned int GetDoublePressMillis() {
    auto doubleF2Time = Wh_GetIntSetting(L"DoubleF2MilliSeconds");
    return std::max(100, std::min(doubleF2Time, 10000));
}
}  // namespace ModSettings

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

    static std::optional<Selection> InsideControl(HWND editControl) {
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

class KeyStreak {
   private:
    ULONGLONG lastPressTime = 0;
    short streak = 0;
    unsigned int targetKey;

   public:
    KeyStreak(unsigned int key) : targetKey(key) {}

    short Count(WPARAM pressedKey) {
        if (pressedKey != targetKey) {
            return streak = 0;
        }

        auto doublePressTime = ModSettings::GetDoublePressMillis();
        ULONGLONG now = GetTickCount64();
        auto timeSinceLastPress = now - lastPressTime;

        if (lastPressTime > 0) {
            Wh_Log(L"Key pressed again after %llums.", timeSinceLastPress);
        }
        lastPressTime = now;

        bool tooSlow = timeSinceLastPress > doublePressTime;
        if (tooSlow) {
            return streak = 1;
        }

        return ++streak;
    }
};

namespace KeyboardHooks {
// this is all static because keyboard hooks must be static
// any installed keyboard hook must be removed when the mod exits
using OnKeyUp = bool (*)(WPARAM pressedKey);

static std::unordered_map<DWORD, HHOOK> hooks = {};
static OnKeyUp onKeyUp;

static LRESULT CALLBACK HandleKeyEvent(int nCode,
                                       WPARAM wParam,
                                       LPARAM lParam) {
    bool shouldProcess = nCode >= 0;
    bool isKeyUp = lParam & 0x80000000;
    if (shouldProcess && isKeyUp) {
        bool handled = onKeyUp(wParam);
        if (handled) {
            return 0;
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static std::optional<HHOOK> AttachToWindow(DWORD threadId) {
    if (threadId == 0) {
        Wh_Log(L"Refusing keyboard hook on bad thread id.");
        return std::nullopt;
    }

    if (hooks.find(threadId) != hooks.end()) {
        Wh_Log(L"Thread %u already has a keyboard hook, skipping.", threadId);
        return std::nullopt;
    }

    HHOOK hook =
        SetWindowsHookExW(WH_KEYBOARD, HandleKeyEvent, nullptr, threadId);

    if (hook == nullptr) {
        Wh_Log(L"Failed to hook thread %u.", threadId);
        return std::nullopt;
    }

    hooks[threadId] = hook;
    return hook;
}

static void DetachAll() {
    for (auto& [threadId, hook] : hooks) {
        bool ok = UnhookWindowsHookEx(hook);
        Wh_Log(L"Unhook %p -> %d.", hook, ok);
    }
    hooks.clear();
}
};  // namespace KeyboardHooks

namespace ExplorerWindowCreatedHook {
// this is all static because window creation hooks must be static
using OnWindowCreated = void (*)(HWND windowHandle, DWORD threadId);

static OnWindowCreated onExplorerWindowCreated = nullptr;

static HWND(WINAPI* previousHandleWindowCreated)(DWORD dwExStyle,
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
                                                 LPVOID lpParam) = nullptr;

static HWND WINAPI HandleExplorerWindowCreated(DWORD dwExStyle,
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
    onExplorerWindowCreated(hwnd, threadId);
    return hwnd;
}

void ExecuteOnNewWindow(OnWindowCreated cb) {
    onExplorerWindowCreated = cb;
    // this will only trigger for processes windhawk injects this mod into
    // i.e. for new _explorer_ windows
    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)HandleExplorerWindowCreated,
                       (void**)&previousHandleWindowCreated);
}
}  // namespace ExplorerWindowCreatedHook

namespace WindowEnumeration {
// this is all static because window enumeration takes a static callback
using ExplorerWindowConsumer = void (*)(HWND windowHandle, DWORD threadId);

static ExplorerWindowConsumer onExplorerWindowFound;

static BOOL CALLBACK HandleGlobalWindowEnumerated(HWND hwnd, LPARAM lParam) {
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    bool isExplorer = processId == GetCurrentProcessId();

    if (isExplorer) {
        onExplorerWindowFound(hwnd, threadId);
    }

    // continue enumeration
    return true;
}

static void ForEachOpenExplorer(ExplorerWindowConsumer cb) {
    onExplorerWindowFound = cb;
    // this loops _all_ open windows, not just explorer's
    EnumWindows(HandleGlobalWindowEnumerated, 0);
}
}  // namespace WindowEnumeration

// -----------------------------------------------------------------------------

static KeyStreak f2Streak(VK_F2);

static bool ApplyMultiF2Selection(WPARAM pressedKey) {
    auto f2Count = f2Streak.Count(pressedKey);
    if (f2Count > 0) {
        Wh_Log(L"F2 streak: %dx.", f2Count);
    }
    if (f2Count > 1) {
        HWND focus = GetFocus();
        if (focus != nullptr && ExplorerUtils::IsEditControl(focus)) {
            Wh_Log(L"Applying selection for %d times F2 in an Edit field.",
                   f2Count);
            auto selection = Selection::InsideControl(focus);
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
                return true;
            }
        }
    }
    return false;
}

static void HookIfFileView(HWND windowHandle, DWORD threadId) {
    auto clazz = ExplorerUtils::FindWindowClassName(windowHandle);
    if (clazz.has_value() && ExplorerUtils::IsFileView(clazz.value())) {
        auto hook = KeyboardHooks::AttachToWindow(threadId);
        if (hook.has_value()) {
            Wh_Log(L"Hooked %s window: hwnd=0x%p hook=0x%p.",
                   clazz.value().c_str(), windowHandle, hook.value());
        }
    }
}

void Wh_ModInit() {
    KeyboardHooks::onKeyUp = ApplyMultiF2Selection;

    Wh_Log(L"Hooking Explorer window creation.");
    ExplorerWindowCreatedHook::ExecuteOnNewWindow(HookIfFileView);

    Wh_Log(L"Hooking already open Explorer windows.");
    WindowEnumeration::ForEachOpenExplorer(HookIfFileView);
}

void Wh_ModUninit() {
    Wh_Log(L"Removing all keyboard hooks.");
    KeyboardHooks::DetachAll();
}
