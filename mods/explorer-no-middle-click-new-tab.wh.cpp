// ==WindhawkMod==
// @id              explorer-no-middle-click-new-tab
// @name            Disable Middle Click New Tab in Explorer
// @description     Makes the middle mouse button do nothing in File Explorer, instead of opening folders in a new tab
// @version         1.0.0
// @author          luizgununes
// @github          https://github.com/luizgununes
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The MIT License.

// ==WindhawkModReadme==
/*
# Disable Middle Click New Tab in Explorer

In Windows 11, middle-clicking a folder in File Explorer opens it in a new tab.
This mod drops the middle button inside Explorer so nothing happens at all.

Only File Explorer windows are affected. The desktop, the taskbar and every
other application keep their normal middle-click behavior: the mod runs inside
explorer.exe alone, and there it only drops clicks aimed at the file list or
the folder tree.

## Scope

Each of the two surfaces has its own switch in the settings:

- **File list** - the main pane with the files and folders.
- **Navigation pane** - the folder tree on the left.

## Why it works this way

The middle-button messages are dropped on their way to the window procedure,
by hooking `DispatchMessageW` and returning without calling the original.
`PeekMessageW` is hooked as well, rewriting the same messages to `WM_NULL`,
because Explorer runs inner modal loops - drag detection after a button press,
for one - that pull messages straight out of the queue instead of going
through the main pump.

`GetMessageW` is deliberately *not* hooked. It blocks until a message arrives,
so a hook frame would sit on the stack of every idle message loop in
explorer.exe for as long as it waits, and Windhawk could not unmap the mod
while that is true - disabling or updating the mod would hang in the
"unloading" state.

## Interaction with other mods

Because the clicks are dropped before the window procedure runs, other mods
that act on the middle button in these two surfaces stop seeing it while this
mod is enabled - `autoscroll-win32`, for example. Mods keyed off
`WM_PARENTNOTIFY`, such as `click-on-empty-explorer`, still fire, since the
system sends that notification when the message is queued rather than when it
is delivered.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- blockFileList: true
  $name: Block in the file list
  $description: Ignore middle clicks in the main pane with the files and folders.
- blockNavPane: true
  $name: Block in the navigation pane
  $description: Ignore middle clicks in the folder tree on the left.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>

enum TargetKind {
    kTargetNone = 0,
    kTargetFileList,
    kTargetNavPane,
};

std::atomic<bool> g_blockFileList;
std::atomic<bool> g_blockNavPane;

void LoadSettings() {
    g_blockFileList = Wh_GetIntSetting(L"blockFileList") != 0;
    g_blockNavPane = Wh_GetIntSetting(L"blockNavPane") != 0;
}

// Walks up from hWnd looking for a window of the given class. GA_PARENT rather
// than GetParent: GetParent returns the owner for a top-level window, which
// could wander off a child chain into an owner chain. The depth limit keeps a
// malformed chain from turning into a long climb.
bool HasAncestorClass(HWND hWnd, PCWSTR className) {
    HWND desktop = GetDesktopWindow();
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    for (int depth = 0; parent && parent != desktop && depth < 24; depth++) {
        WCHAR parentClass[256];
        if (!GetClassName(parent, parentClass, ARRAYSIZE(parentClass))) {
            return false;
        }
        if (wcscmp(parentClass, className) == 0) {
            return true;
        }
        parent = GetAncestor(parent, GA_PARENT);
    }
    return false;
}

TargetKind ClassifyWindow(HWND hWnd) {
    if (!hWnd) {
        return kTargetNone;
    }

    WCHAR className[256];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return kTargetNone;
    }

    bool isFileList = wcscmp(className, L"DirectUIHWND") == 0 ||
                      wcscmp(className, L"SysListView32") == 0;
    bool isNavPane = wcscmp(className, L"SysTreeView32") == 0;
    if (!isFileList && !isNavPane) {
        return kTargetNone;
    }

    // Everything we touch must sit inside an Explorer tab. This is what keeps
    // the desktop out: its SHELLDLL_DefView hangs off Progman/WorkerW, never
    // off ShellTabWindowClass.
    if (!HasAncestorClass(hWnd, L"ShellTabWindowClass")) {
        return kTargetNone;
    }

    if (isFileList && HasAncestorClass(hWnd, L"SHELLDLL_DefView")) {
        return kTargetFileList;
    }
    if (isNavPane && HasAncestorClass(hWnd, L"NamespaceTreeControl")) {
        return kTargetNavPane;
    }
    return kTargetNone;
}

// True when the message is a middle-button click on a surface the user asked
// us to silence. The class walk only runs for the three middle button
// messages, so the common path through the message pump stays a single
// integer comparison.
bool ShouldBlock(const MSG* msg) {
    if (!msg) {
        return false;
    }

    switch (msg->message) {
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
            break;
        default:
            return false;
    }

    switch (ClassifyWindow(msg->hwnd)) {
        case kTargetFileList:
            return g_blockFileList.load();
        case kTargetNavPane:
            return g_blockNavPane.load();
        default:
            return false;
    }
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;

LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (ShouldBlock(lpMsg)) {
        Wh_Log(L"Dropped 0x%04X for %p", lpMsg->message, lpMsg->hwnd);
        return 0;
    }
    return DispatchMessageW_Original(lpMsg);
}

using PeekMessageW_t = decltype(&PeekMessageW);
PeekMessageW_t PeekMessageW_Original;

BOOL WINAPI PeekMessageW_Hook(LPMSG lpMsg,
                              HWND hWnd,
                              UINT wMsgFilterMin,
                              UINT wMsgFilterMax,
                              UINT wRemoveMsg) {
    BOOL result = PeekMessageW_Original(lpMsg, hWnd, wMsgFilterMin,
                                        wMsgFilterMax, wRemoveMsg);
    if (result && ShouldBlock(lpMsg)) {
        // Rewritten for PM_NOREMOVE too: the caller must not see the real
        // message either way. The message stays queued in that case, and the
        // later removing call gets neutralized in turn.
        Wh_Log(L"Dropped 0x%04X for %p (peek)", lpMsg->message, lpMsg->hwnd);
        lpMsg->message = WM_NULL;
        lpMsg->wParam = 0;
        lpMsg->lParam = 0;
    }
    return result;
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageW_Hook,
                                        &DispatchMessageW_Original)) {
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(PeekMessageW, PeekMessageW_Hook,
                                        &PeekMessageW_Original)) {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
