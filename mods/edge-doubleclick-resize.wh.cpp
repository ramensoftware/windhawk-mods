// ==WindhawkMod==
// @id              edge-doubleclick-resize
// @name            Double-Click Edge to Maximize Width/Height
// @description     دبل كليك على حافة النافذة اليسرى/اليمنى يكبر العرض، والعلوية/السفلية يكبر الطول
// @version         1.0
// @author          Hamid
// @github          https://github.com/nh4700-ai
// @include         *
// @compilerOptions -luser32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Double-Click Edge to Maximize Width/Height

دبل كليك على أي حافة من حواف النافذة:
- الحافة اليسرى أو اليمنى -> يكبّر العرض بالكامل (يبقى الطول كما هو).
- الحافة العلوية أو السفلية -> يكبّر الطول بالكامل (يبقى العرض كما هو).
- الزاوية -> يكبّر الاثنين معاً.

يشتغل على كل النوافذ (مطبق عالمياً عبر @include *).
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>

using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcW_t DefWindowProcW_Original;

using DefWindowProcA_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcA_t DefWindowProcA_Original;

// المنطق المشترك: يفحص hit-test code، ويكبر العرض و/أو الطول حسب الحافة
bool HandleEdgeDoubleClick(HWND hWnd, WPARAM wParam) {
    int hit = (int)wParam;

    bool onLeft   = (hit == HTLEFT   || hit == HTTOPLEFT    || hit == HTBOTTOMLEFT);
    bool onRight  = (hit == HTRIGHT  || hit == HTTOPRIGHT   || hit == HTBOTTOMRIGHT);
    bool onTop    = (hit == HTTOP    || hit == HTTOPLEFT    || hit == HTTOPRIGHT);
    bool onBottom = (hit == HTBOTTOM || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT);

    if (!(onLeft || onRight || onTop || onBottom))
        return false; // مو حافة، خلي المعالجة الافتراضية تكمل

    RECT wr; // الحدود الحقيقية للنافذة (تشمل الحافة غير المرئية)
    if (!GetWindowRect(hWnd, &wr))
        return false;

    RECT vr = wr; // الحدود المرئية الفعلية
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &vr, sizeof(vr));

    // مقدار الحافة غير المرئية على كل جهة (الفرق بين الحقيقي والمرئي)
    int leftInset   = vr.left   - wr.left;
    int topInset    = vr.top    - wr.top;
    int rightInset  = wr.right  - vr.right;
    int bottomInset = wr.bottom - vr.bottom;

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    int newX = wr.left;
    int newY = wr.top;
    int newW = wr.right - wr.left;
    int newH = wr.bottom - wr.top;

    if (onLeft || onRight) {
        // نخلي الحدود المرئية تلامس حافة الشاشة تماماً، مع تعويض الهامش غير المرئي
        newX = mi.rcWork.left - leftInset;
        newW = (mi.rcWork.right + rightInset) - newX;
    }

    if (onTop || onBottom) {
        newY = mi.rcWork.top - topInset;
        newH = (mi.rcWork.bottom + bottomInset) - newY;
    }

    SetWindowPos(hWnd, nullptr, newX, newY, newW, newH,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    return true; // تمت معالجتها، لا داعي لاستدعاء الدالة الأصلية
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_NCLBUTTONDBLCLK) {
        if (HandleEdgeDoubleClick(hWnd, wParam))
            return 0;
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_NCLBUTTONDBLCLK) {
        if (HandleEdgeDoubleClick(hWnd, wParam))
            return 0;
    }
    return DefWindowProcA_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook,
                        (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)DefWindowProcA, (void*)DefWindowProcA_Hook,
                        (void**)&DefWindowProcA_Original);
    return TRUE;
}
