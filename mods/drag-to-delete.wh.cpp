// ==WindhawkMod==
// @id              drag-to-delete
// @name            Drag to Delete
// @description     Modern drop file to recycle bin.
// @version         1.0.0
// @author          iMAboud
// @github          https://github.com/iMAboud
// @include         explorer.exe
// @compilerOptions -lole32 -lgdi32 -lshell32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Drag to Recycle Bin (Modern Glass Edition)

Displays a floating circle at the top center of the screen when dragging files. Drag and drop any file or folder onto the icon to quickly move it to the Recycle Bin.

![Demonstration](https://i.imgur.com/bKMRf5A.gif)

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlobj.h>
#include <gdiplus.h>
#include <vector>
#include <windhawk_api.h>

using namespace Gdiplus;

// --- Dimensions & Layout ---
static const int WND_WIDTH  = 200;
static const int WND_HEIGHT = 160;
static const int WND_TOP_Y  = 8;
static const float CENTER_X = 100.0f;
static const float CENTER_Y = 50.0f;
static const float BASE_RADIUS = 34.0f;

// --- State Variables ---
enum class AnimState { HIDDEN, VISIBLE, DROPPED };

static HWND g_hOverlayWnd = nullptr;
static HANDLE g_hUIThread = nullptr;
static LONG g_activeDrags = 0;
static ULONG_PTR g_gdiplusToken = 0;

static AnimState g_animState = AnimState::HIDDEN;
static float g_currentScale = 0.0f;
static float g_targetScale = 0.0f;

#define WM_UPDATE_DRAG_STATE (WM_USER + 101)
#define TIMER_ANIM 1

// Forward Declarations
void RenderFrame(HWND hWnd);
void MoveFilesToRecycleBin(IDataObject* pDataObject);

// --- Drawing Helper: Modern Vector Trash Can ---
void DrawModernTrashIcon(Graphics& g, float cx, float cy, float scale, Color color) {
    SolidBrush brush(color);
    float s = scale;

    // 1. Handle (top rounded pill)
    Pen handlePen(color, 2.5f * s);
    handlePen.SetStartCap(LineCapRound);
    handlePen.SetEndCap(LineCapRound);
    g.DrawLine(&handlePen, cx - 4.5f * s, cy - 16.5f * s, cx + 4.5f * s, cy - 16.5f * s);

    // 2. Lid (horizontal rounded bar)
    Pen lidPen(color, 3.5f * s);
    lidPen.SetStartCap(LineCapRound);
    lidPen.SetEndCap(LineCapRound);
    g.DrawLine(&lidPen, cx - 14.0f * s, cy - 12.0f * s, cx + 14.0f * s, cy - 12.0f * s);

    // 3. Body (tapered bucket with rounded bottom corners)
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

    // 4. Vertical slot cutouts
    Pen slotPen(Color(180, 22, 22, 26), 1.8f * s);
    slotPen.SetStartCap(LineCapRound);
    slotPen.SetEndCap(LineCapRound);
    for (float off : { -4.5f * s, 0.0f, 4.5f * s }) {
        g.DrawLine(&slotPen, cx + off, topY + 4.0f * s, cx + off, botY - 4.0f * s);
    }
}

// --- Render Overlay Frame ---
void RenderFrame(HWND hWnd) {
    if (g_animState == AnimState::HIDDEN || g_currentScale < 0.05f) return;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = WND_WIDTH;
    bmi.bmiHeader.biHeight = -WND_HEIGHT; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HGDIOBJ hOldBitmap = SelectObject(hdcMem, hBitmap);

    {
        Bitmap bmp(WND_WIDTH, WND_HEIGHT, WND_WIDTH * 4, PixelFormat32bppPARGB, (BYTE*)pBits);
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        float r = BASE_RADIUS * g_currentScale;
        float cx = CENTER_X;
        float cy = CENTER_Y;

        // Ambient soft shadow
        SolidBrush shadowBrush(Color(25, 0, 0, 0));
        g.FillEllipse(&shadowBrush, cx - r - 4.0f, cy - r - 1.0f, (r + 4.0f) * 2, (r + 4.0f) * 2 + 5.0f);

        // Frosted glass circle body
        RectF glassRect(cx - r, cy - r, r * 2, r * 2);
        LinearGradientBrush glassBrush(
            PointF(cx, cy - r),
            PointF(cx, cy + r),
            Color(215, 38, 40, 46),
            Color(190, 20, 21, 25)
        );
        g.FillEllipse(&glassBrush, glassRect);

        // Chamfer / subtle rim glaze
        Pen innerEdgePen(Color(35, 255, 255, 255), 1.0f);
        g.DrawEllipse(&innerEdgePen, cx - r + 0.5f, cy - r + 0.5f, (r - 0.5f) * 2, (r - 0.5f) * 2);

        // Vector trash icon
        DrawModernTrashIcon(g, cx, cy, g_currentScale, Color(245, 248, 250));
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    POINT ptDst = { (screenWidth - WND_WIDTH) / 2, WND_TOP_Y };
    POINT ptSrc = { 0, 0 };
    SIZE wndSize = { WND_WIDTH, WND_HEIGHT };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &wndSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

// --- IDropTarget Implementation ---
class CRecycleDropTarget final : public IDropTarget {
    LONG m_refCount = 1;

    bool IsOverTrash(POINTL pt) {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        float dx = (float)(pt.x - (screenWidth - WND_WIDTH) / 2) - CENTER_X;
        float dy = (float)(pt.y - WND_TOP_Y) - CENTER_Y;
        float hitRadius = BASE_RADIUS * g_currentScale + 16.0f;
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
        if (pDataObj->QueryGetData(&fmt) == S_OK && IsOverTrash(pt)) {
            if (pdwEffect) *pdwEffect = DROPEFFECT_MOVE;
            g_targetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* pdwEffect) override {
        if (IsOverTrash(pt)) {
            if (pdwEffect) *pdwEffect = DROPEFFECT_MOVE;
            g_targetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        g_targetScale = 1.0f;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        if (IsOverTrash(pt)) {
            if (pdwEffect) *pdwEffect = DROPEFFECT_MOVE;
            MoveFilesToRecycleBin(pDataObj);
            g_currentScale = 1.25f; // Pop feedback on drop
            g_targetScale = 0.0f;   // Shrink away
            g_animState = AnimState::DROPPED;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
        }
        return S_OK;
    }
};

static CRecycleDropTarget* g_pDropTarget = nullptr;

// --- Recycle Bin Deletion ---
void MoveFilesToRecycleBin(IDataObject* pDataObject) {
    if (!pDataObject) return;

    FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg;
    if (SUCCEEDED(pDataObject->GetData(&fmt, &stg))) {
        HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
        if (hDrop) {
            UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            std::vector<wchar_t> buffer;

            for (UINT i = 0; i < fileCount; i++) {
                UINT len = DragQueryFileW(hDrop, i, nullptr, 0);
                size_t prevSize = buffer.size();
                buffer.resize(prevSize + len + 1);
                DragQueryFileW(hDrop, i, buffer.data() + prevSize, len + 1);
            }
            buffer.push_back(L'\0'); // Double null terminator

            GlobalUnlock(stg.hGlobal);
            ReleaseStgMedium(&stg);

            SHFILEOPSTRUCTW fileOp = {};
            fileOp.wFunc = FO_DELETE;
            fileOp.pFrom = buffer.data();
            fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;
            SHFileOperationW(&fileOp);
        } else {
            ReleaseStgMedium(&stg);
        }
    }
}

// --- Window Procedure ---
LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_UPDATE_DRAG_STATE: {
            if (wParam != 0) {
                g_animState = AnimState::VISIBLE;
                g_currentScale = 0.3f;
                g_targetScale = 1.0f;
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
                g_currentScale += (g_targetScale - g_currentScale) * 0.25f;

                if (g_targetScale == 0.0f && g_currentScale < 0.05f) {
                    g_animState = AnimState::HIDDEN;
                    KillTimer(hWnd, TIMER_ANIM);
                    ShowWindow(hWnd, SW_HIDE);
                    return 0;
                }

                RenderFrame(hWnd);
            }
            return 0;
        }

        case WM_DESTROY: {
            KillTimer(hWnd, TIMER_ANIM);
            RevokeDragDrop(hWnd);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// --- UI Thread ---
DWORD WINAPI OverlayUIThread(LPVOID) {
    OleInitialize(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"WindhawkDragToDeleteOverlay";
    RegisterClassExW(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    g_hOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, nullptr, WS_POPUP,
        (screenWidth - WND_WIDTH) / 2, WND_TOP_Y,
        WND_WIDTH, WND_HEIGHT,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    g_pDropTarget = new CRecycleDropTarget();
    RegisterDragDrop(g_hOverlayWnd, g_pDropTarget);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_pDropTarget) {
        g_pDropTarget->Release();
        g_pDropTarget = nullptr;
    }

    OleUninitialize();
    return 0;
}

// --- Hooked DoDragDrop ---
using DoDragDrop_t = HRESULT (WINAPI *)(IDataObject*, IDropSource*, DWORD, LPDWORD);
static DoDragDrop_t pOriginalDoDragDrop = nullptr;

HRESULT WINAPI Hooked_DoDragDrop(IDataObject* pDataObj, IDropSource* pDropSource, DWORD dwOKEffects, LPDWORD pdwEffect) {
    bool bIsFileDrag = false;
    if (pDataObj) {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        bIsFileDrag = (pDataObj->QueryGetData(&fmt) == S_OK);
    }

    if (bIsFileDrag && InterlockedIncrement(&g_activeDrags) == 1 && g_hOverlayWnd) {
        PostMessage(g_hOverlayWnd, WM_UPDATE_DRAG_STATE, 1, 0);
    }

    HRESULT hr = pOriginalDoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);

    if (bIsFileDrag && InterlockedDecrement(&g_activeDrags) == 0 && g_hOverlayWnd) {
        PostMessage(g_hOverlayWnd, WM_UPDATE_DRAG_STATE, 0, 0);
    }

    return hr;
}

// --- Mod Lifecycle ---
BOOL Wh_ModInit() {
    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);

    g_hUIThread = CreateThread(nullptr, 0, OverlayUIThread, nullptr, 0, nullptr);

    HMODULE hOle32 = GetModuleHandleW(L"ole32.dll");
    if (!hOle32) hOle32 = LoadLibraryW(L"ole32.dll");

    void* pDoDragDrop = (void*)GetProcAddress(hOle32, "DoDragDrop");
    if (pDoDragDrop) {
        Wh_SetFunctionHook(pDoDragDrop, (void*)Hooked_DoDragDrop, (void**)&pOriginalDoDragDrop);
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hOverlayWnd) {
        PostMessage(g_hOverlayWnd, WM_CLOSE, 0, 0);
    }
    if (g_hUIThread) {
        WaitForSingleObject(g_hUIThread, 2000);
        CloseHandle(g_hUIThread);
    }
    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
    }
}