// ==WindhawkMod==
// @id startallback-jumplist-width
// @name StartAllBack Jump List Width
// @description Resizes StartAllBack Jump List together with its internal list controls.
// @version 1.2
// @author Murtuzoff
// @github https://github.com/Murtuzoff
// @include explorer.exe
// @architecture x86-64
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# StartAllBack Jump List Width

Changes the width of StartAllBack jump lists and resizes their internal list controls to match.

The mod targets StartAllBack's `SIBJumpView` window hierarchy inside `explorer.exe`. It adjusts the outer jump-list window, its `SIBBarHost` containers, and the nested `SysListView32` controls so the list keeps a consistent layout at the configured width.

## Settings

- **Jump List width** - overall target width of the StartAllBack jump list. The value is limited to 120-600 pixels.
- The internal `SysListView32` controls use a fixed 6-pixel right margin.

Changes are applied immediately to existing StartAllBack jump-list windows when the settings are changed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 225
  $name: Jump List width
  $description: Overall width of the StartAllBack Jump List.
  $name:ru-RU: Ширина Jump List
  $description:ru-RU: Общая ширина StartAllBack Jump List.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <algorithm>

#include <atomic>

#include <cwchar>

// ============================================================

// Settings

// ============================================================

static std::atomic<int> g_width{225};

static constexpr int kEdgeMargin = 6;

static void LoadSettings()

{

    int width = Wh_GetIntSetting(L"width");

    width = std::clamp(width, 120, 600);

    g_width.store(width, std::memory_order_relaxed);

}

static int GetTargetWidth()

{

    return g_width.load(std::memory_order_relaxed);

}

// ============================================================

// Window-class helpers

// ============================================================

static bool IsClass(HWND hWnd, const wchar_t* wanted)

{

    if (!hWnd)

        return false;

    wchar_t className[128] = {};

    if (!GetClassNameW(

        hWnd,

        className,

        ARRAYSIZE(className)))

    {

        return false;

    }

    return wcscmp(className, wanted) == 0;

}

static bool IsSIBJumpView(HWND hWnd)

{

    return IsClass(hWnd, L"SIBJumpView");

}

static bool IsJumpBarHost(HWND hWnd)

{

    if (!IsClass(hWnd, L"SIBBarHost"))

        return false;

    HWND parent = GetParent(hWnd);

    return IsSIBJumpView(parent);

}

static bool IsJumpListView(HWND hWnd)

{

    if (!IsClass(hWnd, L"SysListView32"))

        return false;

    HWND host = GetParent(hWnd);

    if (!IsClass(host, L"SIBBarHost"))

        return false;

    HWND jumpView = GetParent(host);

    return IsSIBJumpView(jumpView);

}

// ============================================================

// Relative child geometry

// ============================================================

static bool GetRelativeRect(

    HWND hWnd,

    HWND parent,

    RECT* rect)

{

    if (!rect)

        return false;

    if (!GetWindowRect(hWnd, rect))

        return false;

    MapWindowPoints(

        HWND_DESKTOP,

        parent,

        reinterpret_cast<LPPOINT>(rect),

        2

    );

    return true;

}

// ============================================================

// Original functions

// ============================================================

using SetWindowPos_t =

    decltype(&SetWindowPos);

static SetWindowPos_t

    SetWindowPos_Original = nullptr;

using MoveWindow_t =

    decltype(&MoveWindow);

static MoveWindow_t

    MoveWindow_Original = nullptr;

using ShowWindow_t =

    decltype(&ShowWindow);

static ShowWindow_t

    ShowWindow_Original = nullptr;

// ============================================================

// Resize internal StartAllBack controls

//

// Штатная структура из диагностики:

//

// SIBJumpView

// ├─ SIBBarHost

// │  └─ SysListView32  x=4, right margin=4

// └─ SIBBarHost

//    └─ SysListView32  x=4, right margin=4

// ============================================================

static void ResizeChildren(HWND jumpView)

{

    if (!IsSIBJumpView(jumpView))

        return;

    int targetWidth = GetTargetWidth();

    constexpr int edgeMargin = kEdgeMargin;

    // Только непосредственные SIBBarHost.

    for (

        HWND host = GetWindow(jumpView, GW_CHILD);

        host;

        host = GetWindow(host, GW_HWNDNEXT))

    {

        if (!IsClass(host, L"SIBBarHost"))

            continue;

        RECT hostRect = {};

        if (!GetRelativeRect(

            host,

            jumpView,

            &hostRect))

        {

            continue;

        }

        int hostX = hostRect.left;

        int hostY = hostRect.top;

        int hostHeight =

            hostRect.bottom -

            hostRect.top;

        if (hostHeight <= 0)

            continue;

        // SIBBarHost штатно доходит до правого края SIBJumpView.

        int newHostWidth =

            targetWidth - hostX;

        if (newHostWidth < 1)

            newHostWidth = 1;

        // Сначала считываем геометрию ListView,

        // пока она ещё доступна.

        for (

            HWND list = GetWindow(host, GW_CHILD);

            list;

            list = GetWindow(list, GW_HWNDNEXT))

        {

            if (!IsClass(list, L"SysListView32"))

                continue;

            RECT listRect = {};

            if (!GetRelativeRect(

                list,

                host,

                &listRect))

            {

                continue;

            }

            int listX = listRect.left;

            int listY = listRect.top;

            int listHeight =

                listRect.bottom -

                listRect.top;

            if (listHeight <= 0)

                continue;

            // Штатно:

            // x = 4

            // right margin = 4

            //

            // Поэтому при width=225:

            // 225 - 4 - 4 = 217 px

            int newListWidth =

                newHostWidth -

                listX -

                edgeMargin;

            if (newListWidth < 1)

                newListWidth = 1;

            SetWindowPos_Original(

                list,

                nullptr,

                listX,

                listY,

                newListWidth,

                listHeight,

                SWP_NOZORDER |

                SWP_NOACTIVATE |

                SWP_NOOWNERZORDER

            );

        }

        SetWindowPos_Original(

            host,

            nullptr,

            hostX,

            hostY,

            newHostWidth,

            hostHeight,

            SWP_NOZORDER |

            SWP_NOACTIVATE |

            SWP_NOOWNERZORDER

        );

    }

}

// ============================================================

// Apply complete geometry

// ============================================================

static void ApplyLayout(HWND jumpView)

{

    if (!IsSIBJumpView(jumpView))

        return;

    RECT rc = {};

    if (!GetWindowRect(jumpView, &rc))

        return;

    int height =

        rc.bottom -

        rc.top;

    if (height <= 0)

        return;

    int targetWidth =

        GetTargetWidth();

    // Parent.

    SetWindowPos_Original(

        jumpView,

        nullptr,

        0,

        0,

        targetWidth,

        height,

        SWP_NOMOVE |

        SWP_NOZORDER |

        SWP_NOACTIVATE |

        SWP_NOOWNERZORDER

    );

    // Children.

    ResizeChildren(jumpView);

    RedrawWindow(

        jumpView,

        nullptr,

        nullptr,

        RDW_INVALIDATE |

        RDW_FRAME |

        RDW_ALLCHILDREN

    );

}

// ============================================================

// SetWindowPos hook

// ============================================================

static BOOL WINAPI SetWindowPos_Hook(

    HWND hWnd,

    HWND hWndInsertAfter,

    int X,

    int Y,

    int cx,

    int cy,

    UINT flags)

{

    int targetWidth = GetTargetWidth();

    constexpr int edgeMargin = kEdgeMargin;

    // --------------------------------------------------------

    // SIBJumpView

    // --------------------------------------------------------

    if (IsSIBJumpView(hWnd))

    {

        if (!(flags & SWP_NOSIZE))

        {

            cx = targetWidth;

        }

        else if (flags & SWP_SHOWWINDOW)

        {

            RECT rc = {};

            if (GetWindowRect(hWnd, &rc))

            {

                int height =

                    rc.bottom -

                    rc.top;

                if (height > 0)

                {

                    cx = targetWidth;

                    cy = height;

                    flags &= ~SWP_NOSIZE;

                }

            }

        }

        // Важно: подгоняем children ДО первого видимого кадра.

        ResizeChildren(hWnd);

        BOOL result =

            SetWindowPos_Original(

                hWnd,

                hWndInsertAfter,

                X,

                Y,

                cx,

                cy,

                flags

            );

        // И ещё раз после layout StartAllBack.

        ResizeChildren(hWnd);

        return result;

    }

    // --------------------------------------------------------

    // SIBBarHost

    // --------------------------------------------------------

    if (IsJumpBarHost(hWnd))

    {

        if (!(flags & SWP_NOSIZE))

        {

            HWND jumpView = GetParent(hWnd);

            int x = X;

            if (flags & SWP_NOMOVE)

            {

                RECT rc = {};

                if (GetRelativeRect(

                    hWnd,

                    jumpView,

                    &rc))

                {

                    x = rc.left;

                }

            }

            cx =

                targetWidth -

                x;

            if (cx < 1)

                cx = 1;

        }

        return SetWindowPos_Original(

            hWnd,

            hWndInsertAfter,

            X,

            Y,

            cx,

            cy,

            flags

        );

    }

    // --------------------------------------------------------

    // SysListView32 inside SIBBarHost

    // --------------------------------------------------------

    if (IsJumpListView(hWnd))

    {

        if (!(flags & SWP_NOSIZE))

        {

            HWND host =

                GetParent(hWnd);

            RECT hostClient = {};

            if (GetClientRect(

                host,

                &hostClient))

            {

                int listX = X;

                if (flags & SWP_NOMOVE)

                {

                    RECT rc = {};

                    if (GetRelativeRect(

                        hWnd,

                        host,

                        &rc))

                    {

                        listX = rc.left;

                    }

                }

                int hostWidth =

                    hostClient.right -

                    hostClient.left;

                cx =

                    hostWidth -

                    listX -

                    edgeMargin;

                if (cx < 1)

                    cx = 1;

            }

        }

        return SetWindowPos_Original(

            hWnd,

            hWndInsertAfter,

            X,

            Y,

            cx,

            cy,

            flags

        );

    }

    // Всё остальное Explorer не трогаем.

    return SetWindowPos_Original(

        hWnd,

        hWndInsertAfter,

        X,

        Y,

        cx,

        cy,

        flags

    );

}

// ============================================================

// MoveWindow hook

// ============================================================

static BOOL WINAPI MoveWindow_Hook(

    HWND hWnd,

    int X,

    int Y,

    int nWidth,

    int nHeight,

    BOOL bRepaint)

{

    int targetWidth = GetTargetWidth();

    constexpr int edgeMargin = kEdgeMargin;

    if (IsSIBJumpView(hWnd))

    {

        nWidth = targetWidth;

        ResizeChildren(hWnd);

        BOOL result =

            MoveWindow_Original(

                hWnd,

                X,

                Y,

                nWidth,

                nHeight,

                bRepaint

            );

        ResizeChildren(hWnd);

        return result;

    }

    if (IsJumpBarHost(hWnd))

    {

        nWidth =

            targetWidth - X;

        if (nWidth < 1)

            nWidth = 1;

    }

    if (IsJumpListView(hWnd))

    {

        HWND host =

            GetParent(hWnd);

        RECT rc = {};

        if (GetClientRect(

            host,

            &rc))

        {

            int hostWidth =

                rc.right -

                rc.left;

            nWidth =

                hostWidth -

                X -

                edgeMargin;

            if (nWidth < 1)

                nWidth = 1;

        }

    }

    return MoveWindow_Original(

        hWnd,

        X,

        Y,

        nWidth,

        nHeight,

        bRepaint

    );

}

// ============================================================

// ShowWindow hook

// ============================================================

static BOOL WINAPI ShowWindow_Hook(

    HWND hWnd,

    int nCmdShow)

{

    if (nCmdShow != SW_HIDE &&

        IsSIBJumpView(hWnd))

    {

        // Исправляем всю иерархию ДО показа.

        ApplyLayout(hWnd);

    }

    BOOL result =

        ShowWindow_Original(

            hWnd,

            nCmdShow

        );

    if (nCmdShow != SW_HIDE &&

        IsSIBJumpView(hWnd))

    {

        ResizeChildren(hWnd);

    }

    return result;

}

// ============================================================

// Existing cached JumpView

// ============================================================

static BOOL CALLBACK EnumWindowsProc(

    HWND hWnd,

    LPARAM)

{

    DWORD processId = 0;

    GetWindowThreadProcessId(

        hWnd,

        &processId

    );

    if (processId !=

        GetCurrentProcessId())

    {

        return TRUE;

    }

    if (IsSIBJumpView(hWnd))

    {

        ApplyLayout(hWnd);

    }

    return TRUE;

}

// ============================================================

// Windhawk

// ============================================================

BOOL Wh_ModInit()

{

    Wh_Log(

        L"Initializing StartAllBack Jump List Width v1.2"

    );

    LoadSettings();

    WindhawkUtils::SetFunctionHook(
        SetWindowPos,
        SetWindowPos_Hook,
        &SetWindowPos_Original);

    WindhawkUtils::SetFunctionHook(
        MoveWindow,
        MoveWindow_Hook,
        &MoveWindow_Original);

    WindhawkUtils::SetFunctionHook(
        ShowWindow,
        ShowWindow_Hook,
        &ShowWindow_Original);

    return TRUE;

}

void Wh_ModAfterInit()

{

    EnumWindows(

        EnumWindowsProc,

        0

    );

}

void Wh_ModSettingsChanged()

{

    LoadSettings();

    EnumWindows(

        EnumWindowsProc,

        0

    );

}
