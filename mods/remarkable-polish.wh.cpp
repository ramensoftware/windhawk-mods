// ==WindhawkMod==
// @id              remarkable-polish
// @name            reMarkable App Polish
// @description     Make the screen share window capturable, add option to lock its aspect ratio.
// @version         1.0
// @author          Steffan Donal
// @github          https://github.com/SteffanDonal
// @include         reMarkable.exe
// @license         MIT
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# reMarkable App Polish

![Screenshot of Chrome's "Share Your Screen" dialog, where
the reMarkable app's "Share Screen" window is selectable.
](https://raw.githubusercontent.com/SteffanDonal/windhawk-mods/refs/heads/SteffanDonal-assets/remarkable-polish/google-meet-screenshot.png)

Adds two major improvements to the "Screen Share" feature:

1. Makes Screen Share a regular top-level window, so all of your apps can
actually see it, so, you know, you may _share your screen..._
2. Adds a toggle to lock the Screen Share window to your device's aspect ratio
when resized.

## Context

_over a year ago, at reMarkable HQ..._

> Hey, so the reMarkable Paper Pro comes out soon - our first full-color tablet!
We should probably spruce up that desktop app we've not touched in a long
time...

> Oooh, yeah, you're right! We could tighten everything up - use a subtle
off-white color for backgrounds... A new font! Hover effects, the works! Oh, I
can't wait to tweak every little option available to us!

#### With their impeccable style and taste, surely nothing could go wrong...

### They broke screen sharing. And it's been broken for over a year.

You can share your screen to your Windows PC, but **you cannot** select it as a
window in basically any of the major video conferencing apps!

### Why?! ...  HOW?!?

During the redesign, they made the screen share window a separate "child" of the
main app window.

In doing so, they removed the screen share window's taskbar button, which also
happens to be closely related to the method most video conferencing apps use to
identify capture targets! Hooray!

### Right... so... submit a bug report?

I did. Like, three times. I made a post on Reddit.
I _tried_. Radio silence.

It'd be _so_ easy for them to fix, but it's still broken today.

Thankfully, Windhawk is a thing - so here we are.
I fixed it myself. Hopefully we can delete this some day.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LockWindowAspect: true
  $name: Lock Screen Share Aspect Ratio
  $description: >
    When enabled, the screen share window will be locked to
    the selected device's aspect ratio during resizing.


    Choose the orientation by the edge you grab:

    - Top/bottom edges for landscape.

    - Left/right edges for portrait.

- Device: rmPP
  $name: Device
  $description: The device to lock the aspect ratio to.
  $options:
    - rmPPM: reMarkable Paper Pro Move
    - rmPP: reMarkable Paper Pro
    - rm2: reMarkable 2
    - rm1: reMarkable 1

- ScreenShareWindowCaption: Screen Share
  $name: Screen Share Window Name
  $description: >
    Change this if your reMarkable app's screen share
    window is not named "Screen Share".
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

static bool g_aspectLockEnabled = true;
static int g_aspectLong = 0;
static int g_aspectShort = 0;
static wchar_t g_targetCaption[256] = L"Screen Share";

static std::unordered_map<std::wstring, std::tuple<int, int>>
    g_device_resolution{
        {L"rmPPM", {1696, 954}},
        {L"rmPP", {2160, 1620}},
        {L"rm2", {1872, 1404}},
        {L"rm1", {1872, 1404}},
    };

static void LoadSettings() {
    g_aspectLockEnabled = Wh_GetIntSetting(L"LockWindowAspect") != 0;

    if (PCWSTR s = Wh_GetStringSetting(L"ScreenShareWindowCaption")) {
        wcsncpy_s(g_targetCaption, s, _TRUNCATE);
        Wh_FreeStringSetting(s);
    }

    if (PCWSTR s = Wh_GetStringSetting(L"Device")) {
        const std::wstring key{s};

        if (auto it = g_device_resolution.find(key);
            it != g_device_resolution.end()) {
            auto [w, h] = it->second;
            g_aspectLong = w;
            g_aspectShort = h;
        }

        Wh_Log(L"Selected resolution: %dx%d", g_aspectLong, g_aspectShort);

        Wh_FreeStringSetting(s);
    }
}

static const wchar_t* kTargetProp = L"WH_RM_SCREENCAP_TARGET";
static void MarkTarget(HWND hWnd) {
    if (hWnd && IsWindow(hWnd))
        SetPropW(hWnd, kTargetProp, (HANDLE)1);
}
static bool IsMarkedTarget(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd))
        return false;
    return GetPropW(hWnd, kTargetProp) != nullptr;
}

static std::mutex g_trackedMutex;
static std::unordered_set<HWND> g_trackedSubclassed;
static const UINT_PTR kSubclassId = 1;
static const UINT kMsgUnsubclass = WM_APP + 0x5A21;  // Magic. /Shrug

static void TrackSubclassed(HWND hWnd) {
    std::lock_guard<std::mutex> _{g_trackedMutex};
    g_trackedSubclassed.insert(hWnd);
}
static void UntrackSubclassed(HWND hWnd) {
    std::lock_guard<std::mutex> _{g_trackedMutex};
    g_trackedSubclassed.erase(hWnd);
}

static void ApplyAspectOnSizing(WPARAM edge, RECT* r) {
    if (g_aspectLong <= 0 || g_aspectShort <= 0)
        return;

    RECT pad{};
    if (!AdjustWindowRect(&pad, WS_OVERLAPPEDWINDOW, FALSE)) {
        pad.left = 0;
        pad.top = 0;
        pad.right = 0;
        pad.bottom = 0;
    }

    RECT* screenRect = (RECT*)r;
    RECT clientRect{screenRect->left - pad.left, screenRect->top - pad.top,
                    screenRect->right - pad.right,
                    screenRect->bottom - pad.bottom};

    int targetW = g_aspectLong;
    int targetH = g_aspectShort;

    int w = clientRect.right - clientRect.left;
    int h = clientRect.bottom - clientRect.top;

    auto heightFromWidth = [&](int ww) -> int {
        return (int)((long long)ww * (long long)targetH / (long long)targetW);
    };
    auto widthFromHeight = [&](int hh) -> int {
        return (int)((long long)hh * (long long)targetW / (long long)targetH);
    };

    bool driveByWidth = true;
    switch (edge) {
        case WMSZ_TOP:
        case WMSZ_BOTTOM:
            driveByWidth = false;
            break;
        default:
            driveByWidth = true;
            std::swap(targetW, targetH);
            break;
    }

    int newW = w, newH = h;
    if (driveByWidth) {
        newH = heightFromWidth(w);
    } else {
        newW = widthFromHeight(h);
    }

    switch (edge) {
        case WMSZ_LEFT:
        case WMSZ_TOPLEFT:
        case WMSZ_BOTTOMLEFT:
            clientRect.left = clientRect.right - newW;
            break;
        default:
            clientRect.right = clientRect.left + newW;
            break;
    }

    switch (edge) {
        case WMSZ_TOP:
        case WMSZ_TOPLEFT:
        case WMSZ_TOPRIGHT:
            clientRect.top = clientRect.bottom - newH;
            break;
        default:
            clientRect.bottom = clientRect.top + newH;
            break;
    }

    r->left = clientRect.left + pad.left;
    r->top = clientRect.top + pad.top;
    r->right = clientRect.right + pad.right;
    r->bottom = clientRect.bottom + pad.bottom;
}

static LRESULT CALLBACK TargetSubclassProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           UINT_PTR uIdSubclass,
                                           DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_NCDESTROY:
            UntrackSubclassed(hWnd);
            RemovePropW(hWnd, kTargetProp);
            RemoveWindowSubclass(hWnd, TargetSubclassProc, uIdSubclass);
            break;

        case WM_SIZING: {
            if (!g_aspectLockEnabled)
                break;
            ApplyAspectOnSizing(wParam, (RECT*)lParam);
            return TRUE;
        }

        case kMsgUnsubclass: {
            Wh_Log(L"[Subclass] kMsgUnsubclass hwnd=%p", hWnd);

            UntrackSubclassed(hWnd);
            RemovePropW(hWnd, kTargetProp);
            RemoveWindowSubclass(hWnd, TargetSubclassProc, kSubclassId);
            return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void AttachSubclass(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        Wh_Log(L"[AttachSubclass] invalid hwnd=%p", hWnd);
        return;
    }

    if (SetWindowSubclass(hWnd, TargetSubclassProc, 1, 0)) {
        TrackSubclassed(hWnd);
        Wh_Log(L"[AttachSubclass] attached hwnd=%p", hWnd);
    } else {
        Wh_Log(L"[AttachSubclass] FAILED hwnd=%p gle=%lu", hWnd,
               GetLastError());
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Orig = nullptr;

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
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
    const bool eligible =
        (lpWindowName && wcscmp(lpWindowName, g_targetCaption) == 0);
    if (eligible) {
        Wh_Log(L"[CreateWindowExW::PreCreate] parent=%p, style=%p, exStyle=%p",
               hWndParent, dwStyle, dwExStyle);

        dwStyle = WS_TILEDWINDOW;
        dwExStyle = WS_EX_APPWINDOW;

        hWndParent = nullptr;
    }

    HWND hWnd = CreateWindowExW_Orig(dwExStyle, lpClassName, lpWindowName,
                                     dwStyle, X, Y, nWidth, nHeight, hWndParent,
                                     hMenu, hInstance, lpParam);

    if (eligible && hWnd) {
        Wh_Log(L"[CreateWindowExW::PostCreate] hwnd=%p", hWnd);
        MarkTarget(hWnd);
        AttachSubclass(hWnd);
    }
    return hWnd;
}

using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
static SetWindowLongPtrW_t SetWindowLongPtrW_Orig = nullptr;

// Block setting the Screen Share window's owner so it
// behaves independently from the main RM window.
static LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd,
                                              int nIndex,
                                              LONG_PTR dwNewLong) {
    if (nIndex == GWLP_HWNDPARENT && IsMarkedTarget(hWnd)) {
        Wh_Log(L"[SetWindowLongPtrW] Block owner set hwnd=%p attemptedOwner=%p",
               hWnd, (void*)dwNewLong);
        dwNewLong = 0;
    }
    return SetWindowLongPtrW_Orig(hWnd, nIndex, dwNewLong);
}

BOOL Wh_ModInit() {
    Wh_Log(L"[Init]");
    LoadSettings();

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                            (void**)&CreateWindowExW_Orig)) {
        Wh_Log(L"[Init] Wh_SetFunctionHook(CreateWindowExW) failed");
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void*)SetWindowLongPtrW,
                            (void*)SetWindowLongPtrW_Hook,
                            (void**)&SetWindowLongPtrW_Orig)) {
        Wh_Log(L"[Init] Wh_SetFunctionHook(SetWindowLongPtrW) failed");
        return FALSE;
    }

    Wh_Log(L"[Init] Complete");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[Uninit]");

    std::unordered_set<HWND> snapshot;
    {
        std::lock_guard<std::mutex> _{g_trackedMutex};
        snapshot = g_trackedSubclassed;
    }

    for (HWND hWnd : snapshot) {
        if (!hWnd || !IsWindow(hWnd))
            continue;

        DWORD_PTR result = 0;
        BOOL ok =
            SendMessageTimeoutW(hWnd, kMsgUnsubclass, 0, 0,
                                SMTO_BLOCK | SMTO_ABORTIFHUNG, 200, &result);

        Wh_Log(L"[Uninit] SendMessageTimeout unsubclass hwnd=%p ok=%d gle=%lu",
               hWnd, ok ? 1 : 0, GetLastError());
    }

    {
        std::lock_guard<std::mutex> _{g_trackedMutex};
        g_trackedSubclassed.clear();
    }

    Wh_Log(L"[Uninit] Complete");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[SettingsChanged]");
    LoadSettings();
    Wh_Log(L"[SettingsChanged] Complete");
}
