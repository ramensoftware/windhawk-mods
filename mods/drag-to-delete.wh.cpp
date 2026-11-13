// ==WindhawkMod==
// @id              drag-to-delete
// @name            Drag to Delete
// @description     Drag files onto a floating target to move them to the Recycle Bin.
// @version         1.0.0
// @author          iMAboud
// @github          https://github.com/iMAboud
// @include         explorer.exe
// @compilerOptions -lole32 -lgdi32 -lshell32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Drag to Delete

Drag files onto a floating target to move them to the Recycle Bin.

![Demonstration](https://i.imgur.com/bKMRf5A.gif)
*/
// ==/WindhawkModReadme==

#include <shlobj.h>
#include <gdiplus.h>
#include <vector>
#include <windhawk_utils.h>

using namespace Gdiplus;

static const int BASE_WND_WIDTH    = 200;
static const int BASE_WND_HEIGHT   = 160;
static const int BASE_TOP_Y        = 8;
static const float BASE_CENTER_X   = 100.0f;
static const float BASE_CENTER_Y   = 50.0f;
static const float BASE_RADIUS     = 34.0f;
static const float BASE_HIT_MARGIN = 16.0f;

constexpr WCHAR kClassName[] = L"WindhawkDragToDeleteOverlay";

enum class AnimState { HIDDEN, VISIBLE, DROPPED };

static HWND g_hOverlayWnd = nullptr;
static HANDLE g_hUIThread = nullptr;
static DWORD g_dwUIThreadId = 0;
static HANDLE g_readyEvent = nullptr;
static LONG g_activeDrags = 0;
static ULONG_PTR g_gdiplusToken = 0;

static HANDLE g_hDeleteWorker = nullptr;
static HANDLE g_hDeleteQueueEvent = nullptr;
static HANDLE g_hDeleteStopEvent = nullptr;
static CRITICAL_SECTION g_deleteCs;
static std::vector<std::vector<wchar_t>> g_deleteQueue;

static RECT g_targetWorkArea = {};
static float g_currentDpiScale = 1.0f;
static AnimState g_animState = AnimState::HIDDEN;
static float g_currentScale = 0.0f;
static float g_targetScale = 0.0f;
static float g_lastRenderedScale = -1.0f;

#define WM_UPDATE_DRAG_STATE (WM_USER + 101)
#define TIMER_ANIM 1

HMODULE GetCurrentModuleHandle() {
    HMODULE hModule = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)GetCurrentModuleHandle,
        &hModule
    );
    return hModule;
}

static UINT GetMonitorDpi(HMONITOR hMon) {
    if (hMon) {
        static auto pfnGetDpiForMonitor = (HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*))
            GetProcAddress(GetModuleHandleW(L"shcore.dll"), "GetDpiForMonitor");
        if (!pfnGetDpiForMonitor) {
            HMODULE hShcore = LoadLibraryW(L"shcore.dll");
            if (hShcore) {
                pfnGetDpiForMonitor = (HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*))
                    GetProcAddress(hShcore, "GetDpiForMonitor");
            }
        }
        if (pfnGetDpiForMonitor) {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(pfnGetDpiForMonitor(hMon, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY))) {
                if (dpiX > 0) return dpiX;
            }
        }
    }
    return 96;
}

void RenderFrame(HWND hWnd);
void MoveFilesToRecycleBin(const std::vector<wchar_t>& buffer);

void DrawModernTrashIcon(Graphics& g, float cx, float cy, float scale, Color color) {
    SolidBrush brush(color);
    float s = scale;

    Pen handlePen(color, 2.5f * s);
    handlePen.SetStartCap(LineCapRound);
    handlePen.SetEndCap(LineCapRound);
    g.DrawLine(&handlePen, cx - 4.5f * s, cy - 16.5f * s, cx + 4.5f * s, cy - 16.5f * s);

    Pen lidPen(color, 3.5f * s);
    lidPen.SetStartCap(LineCapRound);
    lidPen.SetEndCap(LineCapRound);
    g.DrawLine(&lidPen, cx - 14.0f * s, cy - 12.0f * s, cx + 14.0f * s, cy - 12.0f * s);

    GraphicsPath bodyPath;
    float topW = 12.0f * s;
    float botW = 9.5f * s;
    float topY = cy - 8.0f * s;
    float botY = cy + 15.0f * s;
    float r = 3.5f * s;

    bodyPath.AddLine(cx - topW, topY, cx + topW, topY);
    bodyPath.AddLine(cx + topW, topY, cx + botW, botY - r);
    bodyPath.AddArc(cx + botW - r * 2, botY - r * 2, r * 2, r * 2, 0, 90);
    bodyPath.AddLine(cx + botW - r, botY, cx - botW + r, botY);
    bodyPath.AddArc(cx - botW, botY - r * 2, r * 2, r * 2, 90, 90);
    bodyPath.AddLine(cx - botW, botY - r, cx - topW, topY);
    bodyPath.CloseFigure();
    g.FillPath(&brush, &bodyPath);

    Pen slotPen(Color(180, 22, 22, 26), 1.8f * s);
    slotPen.SetStartCap(LineCapRound);
    slotPen.SetEndCap(LineCapRound);
    for (float off : { -4.5f * s, 0.0f, 4.5f * s }) {
        g.DrawLine(&slotPen, cx + off, topY + 4.0f * s, cx + off, botY - 4.0f * s);
    }
}

void RenderFrame(HWND hWnd) {
    if (g_animState == AnimState::HIDDEN || g_currentScale < 0.05f) return;

    float dpiScale = g_currentDpiScale;
    int wndWidth = (int)(BASE_WND_WIDTH * dpiScale);
    int wndHeight = (int)(BASE_WND_HEIGHT * dpiScale);
    int topY = (int)(BASE_TOP_Y * dpiScale);
    float cx = BASE_CENTER_X * dpiScale;
    float cy = BASE_CENTER_Y * dpiScale;
    float baseRadius = BASE_RADIUS * dpiScale;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = wndWidth;
    bmi.bmiHeader.biHeight = -wndHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HGDIOBJ hOldBitmap = SelectObject(hdcMem, hBitmap);

    {
        Bitmap bmp(wndWidth, wndHeight, wndWidth * 4, PixelFormat32bppPARGB, (BYTE*)pBits);
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        float r = baseRadius * g_currentScale;

        SolidBrush shadowBrush(Color(25, 0, 0, 0));
        g.FillEllipse(&shadowBrush, cx - r - 4.0f * dpiScale, cy - r - 1.0f * dpiScale, (r + 4.0f * dpiScale) * 2, (r + 4.0f * dpiScale) * 2 + 5.0f * dpiScale);

        RectF glassRect(cx - r, cy - r, r * 2, r * 2);
        LinearGradientBrush glassBrush(
            PointF(cx, cy - r),
            PointF(cx, cy + r),
            Color(215, 38, 40, 46),
            Color(190, 20, 21, 25)
        );
        g.FillEllipse(&glassBrush, glassRect);

        Pen innerEdgePen(Color(35, 255, 255, 255), 1.0f * dpiScale);
        g.DrawEllipse(&innerEdgePen, cx - r + 0.5f, cy - r + 0.5f, (r - 0.5f) * 2, (r - 0.5f) * 2);

        DrawModernTrashIcon(g, cx, cy, g_currentScale * dpiScale, Color(245, 248, 250));
    }

    int monLeft = g_targetWorkArea.left;
    int monWidth = g_targetWorkArea.right - g_targetWorkArea.left;
    int wndLeft = monLeft + (monWidth - wndWidth) / 2;
    int wndTop = g_targetWorkArea.top + topY;

    POINT ptDst = { wndLeft, wndTop };
    POINT ptSrc = { 0, 0 };
    SIZE wndSize = { wndWidth, wndHeight };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &wndSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

class CRecycleDropTarget final : public IDropTarget {
    LONG m_refCount = 1;
    bool m_hasHDrop = false;

    bool IsOverTrash(POINTL pt) {
        if (!g_hOverlayWnd) return false;

        float dpiScale = g_currentDpiScale;
        int wndWidth = (int)(BASE_WND_WIDTH * dpiScale);
        int topY = (int)(BASE_TOP_Y * dpiScale);
        float centerX = BASE_CENTER_X * dpiScale;
        float centerY = BASE_CENTER_Y * dpiScale;
        float baseRadius = BASE_RADIUS * dpiScale;
        float hitMargin = BASE_HIT_MARGIN * dpiScale;

        int monLeft = g_targetWorkArea.left;
        int monWidth = g_targetWorkArea.right - g_targetWorkArea.left;
        int wndLeft = monLeft + (monWidth - wndWidth) / 2;
        int wndTop = g_targetWorkArea.top + topY;

        float dx = (float)(pt.x - wndLeft) - centerX;
        float dy = (float)(pt.y - wndTop) - centerY;
        float hitRadius = baseRadius * g_currentScale + hitMargin;

        return (dx * dx + dy * dy) <= (hitRadius * hitRadius);
    }

public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDropTarget)) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) delete this;
        return count;
    }

    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        m_hasHDrop = (pDataObj && pDataObj->QueryGetData(&fmt) == S_OK);

        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;
            g_targetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* pdwEffect) override {
        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;
            g_targetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        m_hasHDrop = false;
        g_targetScale = 1.0f;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;

            FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stg;
            if (SUCCEEDED(pDataObj->GetData(&fmt, &stg))) {
                HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
                if (hDrop) {
                    UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                    if (fileCount > 0) {
                        std::vector<wchar_t> buffer;
                        for (UINT i = 0; i < fileCount; i++) {
                            UINT len = DragQueryFileW(hDrop, i, nullptr, 0);
                            size_t prevSize = buffer.size();
                            buffer.resize(prevSize + len + 1);
                            DragQueryFileW(hDrop, i, buffer.data() + prevSize, len + 1);
                        }
                        buffer.push_back(L'\0');

                        EnterCriticalSection(&g_deleteCs);
                        g_deleteQueue.push_back(std::move(buffer));
                        LeaveCriticalSection(&g_deleteCs);
                        SetEvent(g_hDeleteQueueEvent);
                    }
                    GlobalUnlock(stg.hGlobal);
                }
                ReleaseStgMedium(&stg);
            }

            g_currentScale = 1.25f;
            g_targetScale = 0.0f;
            g_animState = AnimState::DROPPED;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
        }
        m_hasHDrop = false;
        return S_OK;
    }
};

static CRecycleDropTarget* g_pDropTarget = nullptr;

void MoveFilesToRecycleBin(const std::vector<wchar_t>& buffer) {
    if (buffer.empty()) return;

    SHFILEOPSTRUCTW fileOp = {};
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = buffer.data();
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_WANTNUKEWARNING | FOF_SILENT;
    SHFileOperationW(&fileOp);
}

DWORD WINAPI DeleteWorkerThread(LPVOID) {
    HANDLE handles[2] = { g_hDeleteStopEvent, g_hDeleteQueueEvent };
    while (true) {
        DWORD wait = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0) {
            EnterCriticalSection(&g_deleteCs);
            auto queueCopy = std::move(g_deleteQueue);
            LeaveCriticalSection(&g_deleteCs);

            for (const auto& item : queueCopy) {
                MoveFilesToRecycleBin(item);
            }
            break;
        } else if (wait == WAIT_OBJECT_0 + 1) {
            while (true) {
                std::vector<wchar_t> item;
                EnterCriticalSection(&g_deleteCs);
                if (!g_deleteQueue.empty()) {
                    item = std::move(g_deleteQueue.front());
                    g_deleteQueue.erase(g_deleteQueue.begin());
                }
                LeaveCriticalSection(&g_deleteCs);

                if (item.empty()) break;
                MoveFilesToRecycleBin(item);
            }
        } else {
            break;
        }
    }
    return 0;
}

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_UPDATE_DRAG_STATE: {
            if (wParam != 0) {
                POINT ptCursor;
                GetCursorPos(&ptCursor);
                HMONITOR hMon = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                if (GetMonitorInfoW(hMon, &mi)) {
                    g_targetWorkArea = mi.rcWork;
                } else {
                    g_targetWorkArea = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                }

                g_currentDpiScale = (float)GetMonitorDpi(hMon) / 96.0f;
                g_animState = AnimState::VISIBLE;
                g_currentScale = 0.3f;
                g_targetScale = 1.0f;
                g_lastRenderedScale = -1.0f;
                ShowWindow(hWnd, SW_SHOWNOACTIVATE);
                SetTimer(hWnd, TIMER_ANIM, 16, nullptr);
            } else {
                if (g_animState != AnimState::DROPPED) {
                    g_targetScale = 0.0f;
                }
            }
            return 0;
        }

        case WM_TIMER: {
            if (wParam == TIMER_ANIM) {
                if (abs((int)((g_targetScale - g_currentScale) * 1000.0f)) < 5) {
                    g_currentScale = g_targetScale;
                } else {
                    g_currentScale += (g_targetScale - g_currentScale) * 0.25f;
                }

                if (g_targetScale == 0.0f && g_currentScale < 0.05f) {
                    g_animState = AnimState::HIDDEN;
                    KillTimer(hWnd, TIMER_ANIM);
                    ShowWindow(hWnd, SW_HIDE);
                    return 0;
                }

                if (g_currentScale != g_lastRenderedScale) {
                    g_lastRenderedScale = g_currentScale;
                    RenderFrame(hWnd);
                }
            }
            return 0;
        }

        case WM_DESTROY: {
            KillTimer(hWnd, TIMER_ANIM);
            RevokeDragDrop(hWnd);
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI OverlayUIThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    OleInitialize(nullptr);
    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);

    HINSTANCE hInstance = GetCurrentModuleHandle();
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kClassName;

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"Failed to register overlay window class (error: %lu)", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 0;
    }

    HMONITOR hPrimary = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    g_currentDpiScale = (float)GetMonitorDpi(hPrimary) / 96.0f;

    int wndWidth = (int)(BASE_WND_WIDTH * g_currentDpiScale);
    int wndHeight = (int)(BASE_WND_HEIGHT * g_currentDpiScale);
    int topY = (int)(BASE_TOP_Y * g_currentDpiScale);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);

    g_hOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName, nullptr, WS_POPUP,
        (screenWidth - wndWidth) / 2, topY,
        wndWidth, wndHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!g_hOverlayWnd) {
        Wh_Log(L"Failed to create overlay window (error: %lu)", GetLastError());
        UnregisterClassW(kClassName, hInstance);
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 0;
    }

    g_pDropTarget = new CRecycleDropTarget();
    HRESULT hrDrop = RegisterDragDrop(g_hOverlayWnd, g_pDropTarget);
    if (FAILED(hrDrop)) {
        Wh_Log(L"RegisterDragDrop failed with HRESULT 0x%08X", hrDrop);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hOverlayWnd) {
        DestroyWindow(g_hOverlayWnd); // WM_DESTROY revokes the drop target
        g_hOverlayWnd = nullptr;
    }

    UnregisterClassW(kClassName, hInstance);

    if (g_pDropTarget) {
        g_pDropTarget->Release();
        g_pDropTarget = nullptr;
    }

    GdiplusShutdown(g_gdiplusToken);
    OleUninitialize();
    return 0;
}

using DoDragDrop_t = decltype(&DoDragDrop);
static DoDragDrop_t pOriginalDoDragDrop = nullptr;

HRESULT WINAPI Hooked_DoDragDrop(IDataObject* pDataObj, IDropSource* pDropSource, DWORD dwOKEffects, LPDWORD pdwEffect) {
    bool bIsFileDrag = false;
    if (pDataObj) {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        bIsFileDrag = (pDataObj->QueryGetData(&fmt) == S_OK);
    }

    if (bIsFileDrag && InterlockedIncrement(&g_activeDrags) == 1 && g_hOverlayWnd) {
        PostMessageW(g_hOverlayWnd, WM_UPDATE_DRAG_STATE, 1, 0);
    }

    HRESULT hr = pOriginalDoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);

    if (bIsFileDrag && InterlockedDecrement(&g_activeDrags) == 0 && g_hOverlayWnd) {
        PostMessageW(g_hOverlayWnd, WM_UPDATE_DRAG_STATE, 0, 0);
    }

    return hr;
}

void Wh_ModUninit();

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_deleteCs);
    g_hDeleteQueueEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_hDeleteStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hDeleteQueueEvent || !g_hDeleteStopEvent) {
        Wh_Log(L"Failed to create deletion worker events");
        Wh_ModUninit();
        return FALSE;
    }

    g_hDeleteWorker = CreateThread(nullptr, 0, DeleteWorkerThread, nullptr, 0, nullptr);
    if (!g_hDeleteWorker) {
        Wh_Log(L"Failed to create deletion worker thread (error: %lu)", GetLastError());
        Wh_ModUninit();
        return FALSE;
    }

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"Failed to create ready event (error: %lu)", GetLastError());
        Wh_ModUninit();
        return FALSE;
    }

    g_hUIThread = CreateThread(nullptr, 0, OverlayUIThread, nullptr, 0, &g_dwUIThreadId);
    if (!g_hUIThread) {
        Wh_Log(L"Failed to create overlay UI thread (error: %lu)", GetLastError());
        Wh_ModUninit();
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DoDragDrop, Hooked_DoDragDrop, &pOriginalDoDragDrop)) {
        Wh_Log(L"Failed to hook DoDragDrop");
        Wh_ModUninit();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hUIThread) {
        if (g_readyEvent) WaitForSingleObject(g_readyEvent, INFINITE);
        PostThreadMessageW(g_dwUIThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hUIThread, INFINITE);
        CloseHandle(g_hUIThread);
        g_hUIThread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }

    if (g_hDeleteWorker) {
        if (g_hDeleteStopEvent) SetEvent(g_hDeleteStopEvent);
        WaitForSingleObject(g_hDeleteWorker, INFINITE);
        CloseHandle(g_hDeleteWorker);
        g_hDeleteWorker = nullptr;
    }
    if (g_hDeleteStopEvent) {
        CloseHandle(g_hDeleteStopEvent);
        g_hDeleteStopEvent = nullptr;
    }
    if (g_hDeleteQueueEvent) {
        CloseHandle(g_hDeleteQueueEvent);
        g_hDeleteQueueEvent = nullptr;
    }
    DeleteCriticalSection(&g_deleteCs);
}