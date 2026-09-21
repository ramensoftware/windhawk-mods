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

Subclassing the clicked control does not work, and neither does intercepting
the frame's WM_PARENTNOTIFY. Logging every window inside an Explorer frame
shows WM_PARENTNOTIFY propagating all the way up the chain while
WM_MBUTTONDOWN itself reaches no window procedure at all: Explorer consumes
the click in its message pump, in the pre-translate step that runs before
DispatchMessage. WM_PARENTNOTIFY still fires because the system emits it when
the message is queued, not when it is delivered.

So the message has to be caught on its way out of the queue. This mod hooks
GetMessageW and PeekMessageW and rewrites the middle-button messages bound for
those two surfaces into WM_NULL, which happens before anything in Explorer can
look at them.
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

// Walks up from hWnd looking for a window of the given class. The depth limit
// keeps a malformed chain from turning into a long climb.
bool HasAncestorClass(HWND hWnd, PCWSTR className) {
    HWND parent = GetParent(hWnd);
    for (int depth = 0; parent && depth < 24; depth++) {
        WCHAR parentClass[256];
        if (!GetClassName(parent, parentClass, ARRAYSIZE(parentClass))) {
            return false;
        }
        if (wcscmp(parentClass, className) == 0) {
            return true;
        }
        parent = GetParent(parent);
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

// Rewrites a middle-button message into WM_NULL when it targets a surface the
// user asked us to silence. The class walk only runs for the three middle
// button messages, so the common path through the message pump stays a single
// integer comparison.
void NeutralizeIfBlocked(MSG* msg) {
    if (!msg) {
        return;
    }

    switch (msg->message) {
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
            break;
        default:
            return;
    }

    bool block;
    switch (ClassifyWindow(msg->hwnd)) {
        case kTargetFileList:
            block = g_blockFileList.load();
            break;
        case kTargetNavPane:
            block = g_blockNavPane.load();
            break;
        default:
            return;
    }

    if (!block) {
        return;
    }

    Wh_Log(L"Dropped 0x%04X for %p", msg->message, msg->hwnd);
    msg->message = WM_NULL;
    msg->wParam = 0;
    msg->lParam = 0;
}

using GetMessageW_t = decltype(&GetMessageW);
GetMessageW_t GetMessageW_Original;

BOOL WINAPI GetMessageW_Hook(LPMSG lpMsg,
                             HWND hWnd,
                             UINT wMsgFilterMin,
                             UINT wMsgFilterMax) {
    BOOL result =
        GetMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    if (result > 0) {
        NeutralizeIfBlocked(lpMsg);
    }
    return result;
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
    if (result) {
        // Rewritten for PM_NOREMOVE too: the caller must not see the real
        // message either way. The message stays queued in that case, and the
        // later removing call gets neutralized in turn.
        NeutralizeIfBlocked(lpMsg);
    }
    return result;
}

BOOL Wh_ModInit() {
    LoadSettings();

    Wh_SetFunctionHook((void*)GetMessageW, (void*)GetMessageW_Hook,
                       (void**)&GetMessageW_Original);
    Wh_SetFunctionHook((void*)PeekMessageW, (void*)PeekMessageW_Hook,
                       (void**)&PeekMessageW_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
