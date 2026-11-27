// ==WindhawkMod==
// @id              drag-to-delete
// @name            Drag to Delete
// @description     Drag files onto a floating target to move them to the Recycle Bin.
// @version         1.1.0
// @author          iMAboud
// @github          https://github.com/iMAboud
// @include         explorer.exe
// @compilerOptions -lole32 -lgdi32 -lshell32 -lgdiplus -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Drag to Delete

Drag files onto a floating target to move them to the Recycle Bin.

![Demonstration](https://i.imgur.com/bKMRf5A.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Placement: follow
  $name: Target placement
  $options:
    - follow: Follow cursor
    - nearby: Near the starting point
    - top: Top of screen (original)
- ShowShadow: true
  $name: Show shadow
  $description: Render a subtle drop shadow behind the target.
- IconSource: drawn
  $name: Drag icon appearance
  $options:
    - drawn: Modern glass trash can (default)
    - system: Current Windows Recycle Bin
- IconSize: 68
  $name: Icon size (px)
  #! $min: 24
  #! $max: 72
  #! $format: slider
- DesktopDistance: 72
  $name: Distance on desktop (px)
  #! $min: 48
  #! $max: 220
  #! $format: slider
  #! $hideIf: {Placement: top}
- FolderDistance: 100
  $name: Distance in folders (px)
  #! $min: 48
  #! $max: 220
  #! $format: slider
  #! $hideIf: {Placement: top}
- AutoHideDistance: 280
  $name: Auto-hide distance (0 = off)
  $description: Hides after dragging far away for 150 ms, then restores when you return closer. Follow measures from the drag start, Nearby from the bin. Top mode is unaffected.
  #! $min: 0
  #! $max: 600
  #! $format: slider
  #! $hideIf: {Placement: top}
- TopMargin: 8
  $name: Top margin (px)
  #! $min: 0
  #! $max: 100
  #! $hideIf: {Placement: [follow, nearby]}
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <gdiplus.h>
#include <windhawk_utils.h>

using namespace Gdiplus;

static const int BASE_WND_WIDTH    = 140;
static const int BASE_WND_HEIGHT   = 140;
static const float BASE_CENTER_X   = 70.0f;
static const float BASE_CENTER_Y   = 70.0f;
static const float BASE_HIT_MARGIN = 12.0f;

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
static POINT g_overlayOrigin = {};
static HICON g_hRecycleIcon = nullptr;
static LONG g_dragFromDesktop = 0;
static LONG g_dragFromStartEverything = 0;

static bool g_hovering = false;
static bool g_lastRenderedHover = false;

struct UserSettings {
    std::wstring placement = L"follow";
    bool showShadow = true;
    std::wstring iconSource = L"drawn";
    int iconSize = 68;
    int desktopDistance = 72;
    int folderDistance = 100;
    int autoHideDistance = 280;
    int topMargin = 8;
};

static UserSettings g_settings;

static POINT g_lastCursor = {};
static bool g_haveLastCursor = false;
static POINT g_autoHideDragStart = {};
static float g_autoHideInitialDistance = 0.0f;
static bool g_autoHideConcealed = false;
static bool g_autoHideWindowHidden = false;
static ULONGLONG g_autoHideDistantSince = 0;
static ULONGLONG g_autoHideFadeStarted = 0;
static float g_autoHideOpacity = 1.0f;
static float g_autoHideFadeStart = 1.0f;
static float g_autoHideFadeTarget = 1.0f;

static float g_currentDpiScale = 1.0f;
static AnimState g_animState = AnimState::HIDDEN;
static float g_currentScale = 0.0f;
static float g_targetScale = 0.0f;
static float g_lastRenderedScale = -1.0f;

#define WM_UPDATE_DRAG_STATE (WM_USER + 101)
#define TIMER_ANIM 1

static std::wstring ReadString(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

static int ReadNumber(PCWSTR name, int min, int max) {
    return std::clamp(Wh_GetIntSetting(name), min, max);
}

static void ReadSettings() {
    g_settings.placement = ReadString(L"Placement");
    if (g_settings.placement != L"follow" &&
        g_settings.placement != L"nearby" &&
        g_settings.placement != L"top") {
        g_settings.placement = L"follow";
    }

    g_settings.showShadow = Wh_GetIntSetting(L"ShowShadow") != 0;

    g_settings.iconSource = ReadString(L"IconSource");
    if (g_settings.iconSource != L"drawn" && g_settings.iconSource != L"system") {
        g_settings.iconSource = L"drawn";
    }

    g_settings.iconSize = ReadNumber(L"IconSize", 24, 72);
    g_settings.desktopDistance = ReadNumber(L"DesktopDistance", 48, 220);
    g_settings.folderDistance = ReadNumber(L"FolderDistance", 48, 220);
    g_settings.autoHideDistance = ReadNumber(L"AutoHideDistance", 0, 600);
    g_settings.topMargin = ReadNumber(L"TopMargin", 0, 100);
}

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

static bool IsDesktopDragStart() {
    POINT pt = {};
    if (!GetCursorPos(&pt)) return false;
    HWND wnd = WindowFromPoint(pt);
    wchar_t klass[80] = {};
    for (HWND parent = wnd; parent; parent = GetParent(parent)) {
        klass[0] = 0;
        GetClassNameW(parent, klass, ARRAYSIZE(klass));
        if (lstrcmpiW(klass, L"CabinetWClass") == 0) return false;
        if (lstrcmpiW(klass, L"Progman") == 0 || lstrcmpiW(klass, L"WorkerW") == 0) return true;
    }
    return false;
}

static HICON GetSharpRecycleBinIcon(int desiredPx) {
    HICON hIcon = nullptr;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, KF_FLAG_DEFAULT, nullptr, &pidl)) && pidl) {
        IShellItem* pItem = nullptr;
        if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&pItem))) && pItem) {
            IShellItemImageFactory* pFactory = nullptr;
            if (SUCCEEDED(pItem->QueryInterface(IID_PPV_ARGS(&pFactory))) && pFactory) {
                SIZE size = { desiredPx, desiredPx };
                HBITMAP hBmp = nullptr;
                if (SUCCEEDED(pFactory->GetImage(size, SIIGBF_ICONONLY, &hBmp)) && hBmp) {
                    HBITMAP hMask = CreateBitmap(desiredPx, desiredPx, 1, 1, nullptr);
                    if (hMask) {
                        ICONINFO ii = {};
                        ii.fIcon = TRUE;
                        ii.hbmColor = hBmp;
                        ii.hbmMask = hMask;
                        hIcon = CreateIconIndirect(&ii);
                        DeleteObject(hMask);
                    }
                    DeleteObject(hBmp);
                }
                pFactory->Release();
            }
            pItem->Release();
        }
        ILFree(pidl);
    }
    if (!hIcon) {
        SHSTOCKICONINFO stock = { sizeof(stock) };
        if (SUCCEEDED(SHGetStockIconInfo(SIID_RECYCLER, SHGSI_ICON | SHGSI_LARGEICON, &stock))) {
            hIcon = stock.hIcon;
        }
    }
    return hIcon;
}

static void DrawModernTrashIcon(Graphics& g, float cx, float cy, float scale, Color color) {
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
    float cx = BASE_CENTER_X * dpiScale;
    float cy = BASE_CENTER_Y * dpiScale;
    float baseRadius = (g_settings.iconSize / 2.0f) * dpiScale;

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
        float iconScale = (g_settings.iconSize / 68.0f) * g_currentScale * dpiScale;

        if (g_settings.iconSource == L"system") {
            if (g_settings.showShadow) {
                SolidBrush shadowBrush(Color(25, 0, 0, 0));
                g.FillEllipse(&shadowBrush, cx - r - 3.0f * dpiScale, cy - r + 1.0f * dpiScale,
                              (r + 3.0f * dpiScale) * 2, (r + 3.0f * dpiScale) * 2 + 4.0f * dpiScale);
            }

            if (g_targetScale > 1.02f) {
                SolidBrush hoverBrush(Color(40, 255, 255, 255));
                g.FillEllipse(&hoverBrush, cx - r - 2.0f * dpiScale, cy - r - 2.0f * dpiScale,
                              (r + 2.0f * dpiScale) * 2, (r + 2.0f * dpiScale) * 2);
            }

            if (!g_hRecycleIcon) {
                DrawModernTrashIcon(g, cx, cy, iconScale, Color(245, 248, 250));
            }
        } else {
            if (g_settings.showShadow) {
                SolidBrush shadowBrush(Color(25, 0, 0, 0));
                g.FillEllipse(&shadowBrush, cx - r - 4.0f * dpiScale, cy - r - 1.0f * dpiScale,
                              (r + 4.0f * dpiScale) * 2, (r + 4.0f * dpiScale) * 2 + 5.0f * dpiScale);
            }

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

            DrawModernTrashIcon(g, cx, cy, iconScale, Color(245, 248, 250));
        }
    }

    if (g_settings.iconSource == L"system" && g_hRecycleIcon) {
        int iside = (int)(g_settings.iconSize * dpiScale * g_currentScale + 0.5f);
        DrawIconEx(hdcMem, (int)(cx - iside / 2.0f), (int)(cy - iside / 2.0f),
                   g_hRecycleIcon, iside, iside, 0, nullptr, DI_NORMAL);
    }

    int wndLeft = g_overlayOrigin.x;
    int wndTop = g_overlayOrigin.y;

    POINT ptDst = { wndLeft, wndTop };
    POINT ptSrc = { 0, 0 };
    SIZE wndSize = { wndWidth, wndHeight };
    BYTE alpha = static_cast<BYTE>(std::clamp(g_autoHideOpacity, 0.0f, 1.0f) * 255.0f);
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &wndSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

static bool HasFileDragFormat(IDataObject* source) {
    if (!source) return false;
    FORMATETC hdrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&hdrop) == S_OK) return true;
    CLIPFORMAT shellFormat = static_cast<CLIPFORMAT>(RegisterClipboardFormatW(CFSTR_SHELLIDLIST));
    if (!shellFormat) return false;
    FORMATETC shell = { shellFormat, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&shell) != S_OK) return false;

    IShellItemArray* items = nullptr;
    if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(source, IID_PPV_ARGS(&items))) && items) {
        SFGAOF attribs = 0;
        HRESULT hr = items->GetAttributes(SIATTRIBFLAGS_AND, SFGAO_FILESYSTEM, &attribs);
        items->Release();
        if (SUCCEEDED(hr) && (attribs & SFGAO_FILESYSTEM)) {
            return true;
        }
    }
    return false;
}

static bool ExtractDragPaths(IDataObject* source, std::vector<wchar_t>& output) {
    if (!source) return false;
    FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&fmt) == S_OK) {
        STGMEDIUM medium = {};
        if (FAILED(source->GetData(&fmt, &medium))) return false;
        bool ok = false;
        HDROP files = reinterpret_cast<HDROP>(GlobalLock(medium.hGlobal));
        if (files) {
            UINT count = DragQueryFileW(files, 0xFFFFFFFF, nullptr, 0);
            if (count && count <= 10000) {
                ok = true;
                for (UINT n = 0; n < count; ++n) {
                    UINT len = DragQueryFileW(files, n, nullptr, 0);
                    if (!len || len >= 32760) { ok = false; break; }
                    size_t start = output.size();
                    output.resize(start + len + 1, 0);
                    if (DragQueryFileW(files, n, output.data() + start, len + 1) != len) {
                        ok = false; break;
                    }
                }
            }
            GlobalUnlock(medium.hGlobal);
        }
        ReleaseStgMedium(&medium);
        if (!ok) output.clear();
        else output.push_back(L'\0');
        return ok;
    }

    IShellItemArray* items = nullptr;
    HRESULT hr = SHCreateShellItemArrayFromDataObject(source, IID_PPV_ARGS(&items));
    if (FAILED(hr) || !items) return false;
    DWORD count = 0;
    bool ok = SUCCEEDED(items->GetCount(&count)) && count > 0 && count <= 10000;
    if (ok) {
        for (DWORD n = 0; n < count; ++n) {
            IShellItem* item = nullptr;
            PWSTR path = nullptr;
            if (FAILED(items->GetItemAt(n, &item)) || !item) {
                ok = false; break;
            }
            HRESULT pathHr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
            item->Release();
            if (FAILED(pathHr) || !path || !*path) {
                if (path) CoTaskMemFree(path);
                ok = false; break;
            }
            size_t len = wcslen(path);
            if (len >= 32760) {
                CoTaskMemFree(path);
                ok = false; break;
            }
            output.insert(output.end(), path, path + len + 1);
            CoTaskMemFree(path);
        }
    }
    items->Release();
    if (!ok) output.clear();
    else output.push_back(L'\0');
    return ok;
}

static bool IsStartEverythingSearchDrag() {
    POINT cursor = {};
    if (!GetCursorPos(&cursor)) return false;
    HWND cursorWindow = WindowFromPoint(cursor);
    HWND windows[] = { cursorWindow, GetForegroundWindow() };
    for (HWND window : windows) {
        for (int depth = 0; window && depth < 16; ++depth) {
            wchar_t className[128] = {};
            if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"StartEverything_SearchWindowPrototype") == 0) {
                return true;
            }
            window = GetParent(window);
        }
    }
    return false;
}

static DWORD AllowedRecycleEffect(DWORD offered) {
    if (offered & DROPEFFECT_MOVE) return DROPEFFECT_MOVE;
    if ((offered & DROPEFFECT_COPY) &&
        InterlockedCompareExchange(&g_dragFromStartEverything, 0, 0)) {
        return DROPEFFECT_COPY;
    }
    return DROPEFFECT_NONE;
}

class CRecycleDropTarget final : public IDropTarget {
    LONG m_refCount = 1;
    bool m_hasHDrop = false;

    bool IsOverTrash(POINTL pt) {
        if (!g_hOverlayWnd) return false;

        float dpiScale = g_currentDpiScale;
        float centerX = BASE_CENTER_X * dpiScale;
        float centerY = BASE_CENTER_Y * dpiScale;
        float baseRadius = (g_settings.iconSize / 2.0f) * dpiScale;
        float hitMargin = BASE_HIT_MARGIN * dpiScale;

        int wndLeft = g_overlayOrigin.x;
        int wndTop = g_overlayOrigin.y;

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
        m_hasHDrop = HasFileDragFormat(pDataObj);

        DWORD recycleEffect = pdwEffect ? AllowedRecycleEffect(*pdwEffect) : DROPEFFECT_NONE;
        if (m_hasHDrop && recycleEffect && IsOverTrash(pt)) {
            *pdwEffect = recycleEffect;
            g_targetScale = 1.2f;
            g_hovering = true;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
            g_hovering = false;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* pdwEffect) override {
        DWORD recycleEffect = pdwEffect ? AllowedRecycleEffect(*pdwEffect) : DROPEFFECT_NONE;
        if (m_hasHDrop && recycleEffect && IsOverTrash(pt)) {
            *pdwEffect = recycleEffect;
            g_targetScale = 1.2f;
            g_hovering = true;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_targetScale = 1.0f;
            g_hovering = false;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        m_hasHDrop = false;
        g_targetScale = 1.0f;
        g_hovering = false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        DWORD recycleEffect = pdwEffect ? AllowedRecycleEffect(*pdwEffect) : DROPEFFECT_NONE;
        bool accepted = false;
        if (m_hasHDrop && recycleEffect && IsOverTrash(pt)) {
            std::vector<wchar_t> buffer;
            if (ExtractDragPaths(pDataObj, buffer) && buffer.size() > 2) {
                EnterCriticalSection(&g_deleteCs);
                g_deleteQueue.push_back(std::move(buffer));
                LeaveCriticalSection(&g_deleteCs);
                SetEvent(g_hDeleteQueueEvent);
                accepted = true;
                g_currentScale = 1.25f;
                g_targetScale = 0.0f;
                g_animState = AnimState::DROPPED;
            } else {
                Wh_Log(L"Drag did not contain a complete recyclable file selection");
            }
        }
        if (pdwEffect) *pdwEffect = accepted ? recycleEffect : DROPEFFECT_NONE;
        m_hasHDrop = false;
        g_hovering = false;
        return S_OK;
    }
};

static CRecycleDropTarget* g_pDropTarget = nullptr;

void MoveFilesToRecycleBin(const std::vector<wchar_t>& buffer) {
    if (buffer.empty()) return;
    std::vector<PIDLIST_ABSOLUTE> ids;
    std::vector<PCIDLIST_ABSOLUTE> pidls;
    bool valid = true;
    size_t index = 0;
    while (index < buffer.size() && buffer[index] != L'\0') {
        PCWSTR path = buffer.data() + index;
        size_t len = wcslen(path);
        if (!len || len >= 32760) { valid = false; break; }
        PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path);
        if (!pidl) {
            Wh_Log(L"Failed to resolve Shell path: 0x%08X", GetLastError());
            valid = false; break;
        }
        ids.push_back(pidl);
        pidls.push_back(pidl);
        index += len + 1;
    }

    if (!valid || pidls.empty()) {
        for (auto pidl : ids) ILFree(pidl);
        Wh_Log(L"Recycle batch aborted: one or more paths could not be resolved");
        return;
    }

    IShellItemArray* group = nullptr;
    HRESULT hr = SHCreateShellItemArrayFromIDLists((UINT)pidls.size(), pidls.data(), &group);
    for (auto pidl : ids) ILFree(pidl);

    if (FAILED(hr) || !group) {
        Wh_Log(L"Unable to build grouped Shell selection: 0x%08X", hr);
        return;
    }

    IFileOperation* operation = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&operation));
    if (SUCCEEDED(hr) && operation) {
        const DWORD flags = FOF_ALLOWUNDO | FOF_SILENT | FOF_WANTNUKEWARNING |
                            FOFX_RECYCLEONDELETE | FOFX_ADDUNDORECORD;
        hr = operation->SetOperationFlags(flags);
        if (SUCCEEDED(hr)) hr = operation->DeleteItems(group);
        if (SUCCEEDED(hr)) hr = operation->PerformOperations();
        operation->Release();
    } else {
        Wh_Log(L"IFileOperation unavailable: 0x%08X", hr);
    }
    group->Release();
}

DWORD WINAPI DeleteWorkerThread(LPVOID) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(comResult)) {
        Wh_Log(L"Cannot initialize COM apartment: 0x%08X", comResult);
        return 0;
    }
    HANDLE handles[2] = { g_hDeleteStopEvent, g_hDeleteQueueEvent };
    while (true) {
        DWORD wait = MsgWaitForMultipleObjectsEx(2, handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 + 2) {
            MSG msg = {};
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message != WM_QUIT) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
            continue;
        }
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
    CoUninitialize();
    return 0;
}

static void RestoreDragOverlayZOrder(HWND hWnd, bool force) {
    if (!IsWindowVisible(hWnd)) return;
    bool mustRaise = force || !(GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST);
    if (!mustRaise) {
        RECT overlayBounds = {};
        if (GetWindowRect(hWnd, &overlayBounds)) {
            for (HWND above = GetWindow(hWnd, GW_HWNDPREV); above;
                 above = GetWindow(above, GW_HWNDPREV)) {
                if (above == hWnd) continue;
                if (!IsWindowVisible(above) || IsIconic(above)) continue;
                wchar_t className[64] = {};
                GetClassNameW(above, className, ARRAYSIZE(className));
                if (wcscmp(className, L"SysDragImage") == 0 ||
                    wcscmp(className, L"tooltips_class32") == 0) {
                    continue;
                }
                RECT otherBounds = {}, intersection = {};
                if (GetWindowRect(above, &otherBounds) &&
                    IntersectRect(&intersection, &overlayBounds, &otherBounds)) {
                    mustRaise = true;
                    break;
                }
            }
        }
    }
    if (mustRaise) {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
}

static bool FollowCursor(HWND hWnd) {
    if (g_settings.placement != L"follow" ||
        g_animState != AnimState::VISIBLE || !g_haveLastCursor ||
        g_currentScale < 0.5f || g_targetScale > 1.02f) {
        return false;
    }

    POINT pt = {};
    if (!GetCursorPos(&pt)) return false;
    int deltaX = pt.x - g_lastCursor.x;
    int deltaY = pt.y - g_lastCursor.y;
    g_lastCursor = pt;
    if (!deltaX && !deltaY) return false;

    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(mon, &mi)) g_targetWorkArea = mi.rcWork;
    g_currentDpiScale = (float)GetMonitorDpi(mon) / 96.0f;

    float scale = g_currentDpiScale;
    int width = (int)(BASE_WND_WIDTH * scale);
    int height = (int)(BASE_WND_HEIGHT * scale);
    int centerX = g_overlayOrigin.x + width / 2;
    int centerY = g_overlayOrigin.y + height / 2;
    int toCenterX = centerX - pt.x;
    int toCenterY = centerY - pt.y;
    float capture = (g_settings.iconSize / 2.0f + 22.0f) * scale;
    if (toCenterX * toCenterX + toCenterY * toCenterY < capture * capture) return false;
    if (deltaX * toCenterX + deltaY * toCenterY > 0) return false;

    int distance = (InterlockedCompareExchange(&g_dragFromDesktop, 0, 0) ?
                    g_settings.desktopDistance : g_settings.folderDistance);
    int offsetX = (int)(distance * scale);
    int offsetY = (int)(distance * scale *
                        (InterlockedCompareExchange(&g_dragFromDesktop, 0, 0) ? 1.0f : 0.94f));
    int idealX = pt.x + offsetX;
    int idealY = pt.y - offsetY;
    int margin = (int)(8 * scale);
    if (idealX + width / 2 > g_targetWorkArea.right - margin) idealX = pt.x - offsetX;
    if (idealY - height / 2 < g_targetWorkArea.top + margin) idealY = pt.y + offsetY;

    int minX = g_targetWorkArea.left + margin;
    int minY = g_targetWorkArea.top + margin;
    int maxX = std::max(minX, (int)(g_targetWorkArea.right - margin - width));
    int maxY = std::max(minY, (int)(g_targetWorkArea.bottom - margin - height));
    int goalX = std::clamp(idealX - width / 2, minX, maxX);
    int goalY = std::clamp(idealY - height / 2, minY, maxY);
    int diffX = goalX - g_overlayOrigin.x;
    int diffY = goalY - g_overlayOrigin.y;
    if (abs(diffX) + abs(diffY) < 3) return false;
    int stepX = (int)(diffX * 0.38f);
    int stepY = (int)(diffY * 0.38f);
    if (!stepX && diffX) stepX = diffX > 0 ? 1 : -1;
    if (!stepY && diffY) stepY = diffY > 0 ? 1 : -1;
    g_overlayOrigin.x += stepX;
    g_overlayOrigin.y += stepY;
    return !!SetWindowPos(hWnd, nullptr, g_overlayOrigin.x, g_overlayOrigin.y,
                          0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER);
}

static void RepositionFollowTargetAfterAutoHide(HWND hWnd, POINT pt) {
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(monitor, &mi)) g_targetWorkArea = mi.rcWork;
    g_currentDpiScale = (float)GetMonitorDpi(monitor) / 96.0f;
    const float scale = g_currentDpiScale;
    const int width = (int)(BASE_WND_WIDTH * scale);
    const int height = (int)(BASE_WND_HEIGHT * scale);
    const bool desktop = InterlockedCompareExchange(&g_dragFromDesktop, 0, 0) != 0;
    const int offset = desktop ? g_settings.desktopDistance : g_settings.folderDistance;
    const int ox = (int)(offset * scale);
    const int oy = (int)(offset * (desktop ? 1.0f : 0.94f) * scale);
    const int margin = (int)(8 * scale);
    int centerX = pt.x + ox;
    int centerY = pt.y - oy;
    if (centerX + width / 2 > g_targetWorkArea.right - margin) centerX = pt.x - ox;
    if (centerY - height / 2 < g_targetWorkArea.top + margin) centerY = pt.y + oy;
    const int minX = g_targetWorkArea.left + margin;
    const int minY = g_targetWorkArea.top + margin;
    const int maxX = std::max(minX, (int)(g_targetWorkArea.right - margin - width));
    const int maxY = std::max(minY, (int)(g_targetWorkArea.bottom - margin - height));
    g_overlayOrigin.x = std::clamp(centerX - width / 2, minX, maxX);
    g_overlayOrigin.y = std::clamp(centerY - height / 2, minY, maxY);
    g_lastCursor = pt;
    g_haveLastCursor = true;
    SetWindowPos(hWnd, nullptr, g_overlayOrigin.x, g_overlayOrigin.y, 0, 0,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER);
}

static bool UpdateDragDistanceAutoHide(HWND hWnd) {
    if (g_animState != AnimState::VISIBLE ||
        g_settings.placement == L"top" || g_settings.autoHideDistance == 0 ||
        g_targetScale == 0.0f) {
        return false;
    }

    POINT cursor = {};
    if (!GetCursorPos(&cursor)) return false;
    const float scale = g_currentDpiScale;
    const POINT anchor = (g_settings.placement == L"follow")
        ? g_autoHideDragStart
        : POINT{ g_overlayOrigin.x + (int)(BASE_WND_WIDTH * scale) / 2,
                 g_overlayOrigin.y + (int)(BASE_WND_HEIGHT * scale) / 2 };

    const LONGLONG dx = (LONGLONG)cursor.x - anchor.x;
    const LONGLONG dy = (LONGLONG)cursor.y - anchor.y;
    const LONGLONG distanceSquared = dx * dx + dy * dy;

    const float configuredHide = g_settings.autoHideDistance * scale;
    const float hideRadius = (g_settings.placement == L"nearby")
        ? std::max(configuredHide, g_autoHideInitialDistance + (BASE_HIT_MARGIN + 32.0f) * scale)
        : configuredHide;

    const float restoreRadius = std::max(32.0f * scale,
        hideRadius - std::min(90.0f * scale, hideRadius * 0.35f));
    const bool isFar = (float)distanceSquared > hideRadius * hideRadius;
    const bool isNear = (float)distanceSquared < restoreRadius * restoreRadius;
    const ULONGLONG now = GetTickCount64();
    bool repositioned = false;

    if (!g_autoHideConcealed) {
        if (isFar && !g_hovering) {
            if (!g_autoHideDistantSince) g_autoHideDistantSince = now;
            if (now - g_autoHideDistantSince >= 150) {
                g_autoHideConcealed = true;
                g_autoHideFadeStart = g_autoHideOpacity;
                g_autoHideFadeTarget = 0.0f;
                g_autoHideFadeStarted = now;
            }
        } else {
            g_autoHideDistantSince = 0;
        }
    } else if (isNear || g_hovering) {
        g_autoHideConcealed = false;
        g_autoHideDistantSince = 0;
        g_autoHideFadeStart = g_autoHideOpacity;
        g_autoHideFadeTarget = 1.0f;
        g_autoHideFadeStarted = now;
        if (g_settings.placement == L"follow") {
            RepositionFollowTargetAfterAutoHide(hWnd, cursor);
            repositioned = true;
        }
    }

    const float previousOpacity = g_autoHideOpacity;
    if (g_autoHideOpacity != g_autoHideFadeTarget) {
        const float progress = std::min(1.0f, (float)(now - g_autoHideFadeStarted) / 120.0f);
        g_autoHideOpacity = g_autoHideFadeStart + (g_autoHideFadeTarget - g_autoHideFadeStart) * progress;
        if (progress >= 1.0f) g_autoHideOpacity = g_autoHideFadeTarget;
    }

    if (g_autoHideOpacity <= 0.0f && !g_autoHideWindowHidden) {
        g_autoHideOpacity = 0.0f;
        ShowWindow(hWnd, SW_HIDE);
        g_autoHideWindowHidden = true;
    } else if (g_autoHideOpacity > 0.0f && g_autoHideWindowHidden) {
        g_autoHideWindowHidden = false;
        ShowWindow(hWnd, SW_SHOWNOACTIVATE);
        RestoreDragOverlayZOrder(hWnd, true);
    }
    return repositioned || previousOpacity != g_autoHideOpacity;
}

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_UPDATE_DRAG_STATE: {
            if (wParam != 0) {
                InterlockedExchange(&g_dragFromDesktop, (wParam & 2) ? 1 : 0);
                InterlockedExchange(&g_dragFromStartEverything, (wParam & 4) ? 1 : 0);
                POINT ptCursor;
                GetCursorPos(&ptCursor);
                g_autoHideDragStart = ptCursor;
                g_autoHideConcealed = false;
                g_autoHideWindowHidden = false;
                g_autoHideDistantSince = 0;
                g_autoHideFadeStarted = GetTickCount64();
                g_autoHideFadeStart = 1.0f;
                g_autoHideFadeTarget = 1.0f;
                g_autoHideOpacity = 1.0f;

                HMONITOR hMon = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                if (GetMonitorInfoW(hMon, &mi)) {
                    g_targetWorkArea = mi.rcWork;
                } else {
                    g_targetWorkArea = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                }

                g_currentDpiScale = (float)GetMonitorDpi(hMon) / 96.0f;

                int w = (int)(BASE_WND_WIDTH * g_currentDpiScale);
                int h = (int)(BASE_WND_HEIGHT * g_currentDpiScale);
                int margin = (int)(8 * g_currentDpiScale);
                bool desktop = InterlockedCompareExchange(&g_dragFromDesktop, 0, 0) != 0;
                int offset = (desktop ? g_settings.desktopDistance : g_settings.folderDistance);
                int offsetX = (int)(offset * g_currentDpiScale);
                int offsetY = (int)(offset * (desktop ? 1.0f : 0.94f) * g_currentDpiScale);
                int targetX = ptCursor.x + offsetX;
                int targetY = ptCursor.y - offsetY;

                if (g_settings.placement == L"top") {
                    targetX = (g_targetWorkArea.left + g_targetWorkArea.right) / 2;
                    targetY = g_targetWorkArea.top + (int)(g_settings.topMargin * g_currentDpiScale) + h / 2;
                } else {
                    if (targetX + w / 2 > g_targetWorkArea.right - margin) targetX = ptCursor.x - offsetX;
                    if (targetY - h / 2 < g_targetWorkArea.top + margin) targetY = ptCursor.y + offsetY;
                }

                g_overlayOrigin.x = targetX - w / 2;
                g_overlayOrigin.y = targetY - h / 2;
                g_lastCursor = ptCursor;
                g_haveLastCursor = true;

                if (g_overlayOrigin.x < g_targetWorkArea.left + margin) g_overlayOrigin.x = g_targetWorkArea.left + margin;
                if (g_overlayOrigin.x + w > g_targetWorkArea.right - margin) g_overlayOrigin.x = g_targetWorkArea.right - margin - w;
                if (g_settings.placement != L"top") {
                    if (g_overlayOrigin.y < g_targetWorkArea.top + margin) g_overlayOrigin.y = g_targetWorkArea.top + margin;
                    if (g_overlayOrigin.y + h > g_targetWorkArea.bottom - margin) g_overlayOrigin.y = g_targetWorkArea.bottom - margin - h;
                }

                float initDx = (float)ptCursor.x - ((float)g_overlayOrigin.x + (float)w * 0.5f);
                float initDy = (float)ptCursor.y - ((float)g_overlayOrigin.y + (float)h * 0.5f);
                g_autoHideInitialDistance = sqrtf(initDx * initDx + initDy * initDy);

                g_hovering = false;
                g_lastRenderedHover = false;

                int desiredPx = (int)(g_settings.iconSize * g_currentDpiScale + 0.5f);
                if (g_settings.iconSource == L"system") {
                    if (g_hRecycleIcon) DestroyIcon(g_hRecycleIcon);
                    g_hRecycleIcon = GetSharpRecycleBinIcon(desiredPx);
                }

                g_animState = AnimState::VISIBLE;
                g_currentScale = 0.3f;
                g_targetScale = 1.0f;
                g_lastRenderedScale = -1.0f;

                RenderFrame(hWnd);
                g_lastRenderedScale = g_currentScale;

                SetWindowPos(hWnd, HWND_TOPMOST, g_overlayOrigin.x, g_overlayOrigin.y, w, h,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
                RestoreDragOverlayZOrder(hWnd, true);
                SetTimer(hWnd, TIMER_ANIM, 25, nullptr);
            } else {
                if (g_animState != AnimState::DROPPED) {
                    g_targetScale = 0.0f;
                }
                InterlockedExchange(&g_dragFromDesktop, 0);
                InterlockedExchange(&g_dragFromStartEverything, 0);
            }
            return 0;
        }

        case WM_TIMER: {
            if (wParam == TIMER_ANIM) {
                static unsigned zOrderCheck = 0;
                if (g_animState == AnimState::VISIBLE && ++zOrderCheck % 6 == 0) {
                    RestoreDragOverlayZOrder(hWnd, false);
                }

                if (abs((int)((g_targetScale - g_currentScale) * 1000.0f)) < 5) {
                    g_currentScale = g_targetScale;
                } else {
                    g_currentScale += (g_targetScale - g_currentScale) * 0.25f;
                }

                if (g_targetScale == 0.0f && g_currentScale < 0.05f) {
                    g_animState = AnimState::HIDDEN;
                    KillTimer(hWnd, TIMER_ANIM);
                    ShowWindow(hWnd, SW_HIDE);
                    if (g_hRecycleIcon) {
                        DestroyIcon(g_hRecycleIcon);
                        g_hRecycleIcon = nullptr;
                    }
                    g_hovering = false;
                    return 0;
                }

                bool opacityChanged = UpdateDragDistanceAutoHide(hWnd);
                bool moved = !g_autoHideConcealed && FollowCursor(hWnd);
                bool hoverChanged = g_hovering != g_lastRenderedHover;

                if (opacityChanged || moved || hoverChanged ||
                    g_currentScale != g_lastRenderedScale) {
                    g_lastRenderedScale = g_currentScale;
                    g_lastRenderedHover = g_hovering;
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
        Wh_Log(L"Failed to register overlay class: %lu", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 0;
    }

    HMONITOR hPrimary = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    g_currentDpiScale = (float)GetMonitorDpi(hPrimary) / 96.0f;

    int wndWidth = (int)(BASE_WND_WIDTH * g_currentDpiScale);
    int wndHeight = (int)(BASE_WND_HEIGHT * g_currentDpiScale);

    g_hOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName, nullptr, WS_POPUP,
        0, 0,
        wndWidth, wndHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!g_hOverlayWnd) {
        Wh_Log(L"Failed to create overlay window: %lu", GetLastError());
        UnregisterClassW(kClassName, hInstance);
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 0;
    }

    g_pDropTarget = new CRecycleDropTarget();
    HRESULT hrDrop = RegisterDragDrop(g_hOverlayWnd, g_pDropTarget);
    if (FAILED(hrDrop)) {
        Wh_Log(L"RegisterDragDrop failed: 0x%08X", hrDrop);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hOverlayWnd) {
        DestroyWindow(g_hOverlayWnd);
        g_hOverlayWnd = nullptr;
    }

    UnregisterClassW(kClassName, hInstance);

    if (g_pDropTarget) {
        g_pDropTarget->Release();
        g_pDropTarget = nullptr;
    }

    if (g_hRecycleIcon) {
        DestroyIcon(g_hRecycleIcon);
        g_hRecycleIcon = nullptr;
    }

    GdiplusShutdown(g_gdiplusToken);
    OleUninitialize();
    return 0;
}

static HWND FindSharedRecycleOverlay();
using DoDragDrop_t = decltype(&DoDragDrop);
static DoDragDrop_t pOriginalDoDragDrop = nullptr;

HRESULT WINAPI Hooked_DoDragDrop(IDataObject* pDataObj, IDropSource* pDropSource, DWORD dwOKEffects, LPDWORD pdwEffect) {
    bool bIsFileDrag = false;
    if (pDataObj) {
        bIsFileDrag = HasFileDragFormat(pDataObj);
    }

    if (bIsFileDrag && InterlockedIncrement(&g_activeDrags) == 1) {
        bool fromSearch = IsStartEverythingSearchDrag() && (dwOKEffects & DROPEFFECT_COPY);
        bool fromDesktop = !fromSearch && IsDesktopDragStart();
        InterlockedExchange(&g_dragFromStartEverything, fromSearch ? 1 : 0);
        InterlockedExchange(&g_dragFromDesktop, fromDesktop ? 1 : 0);

        HWND overlay = FindSharedRecycleOverlay();
        if (overlay) {
            PostMessageW(overlay, WM_UPDATE_DRAG_STATE,
                         1 | (fromDesktop ? 2 : 0) | (fromSearch ? 4 : 0), 0);
        }
    }

    HRESULT hr = pOriginalDoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);

    if (bIsFileDrag && InterlockedDecrement(&g_activeDrags) == 0) {
        InterlockedExchange(&g_dragFromStartEverything, 0);
        HWND overlay = FindSharedRecycleOverlay();
        if (overlay) {
            PostMessageW(overlay, WM_UPDATE_DRAG_STATE, 0, 0);
        }
    }

    return hr;
}

static bool g_isDesktopShellHost = false;

static bool IsDesktopShellHostProcess() {
    HWND shell = GetShellWindow();
    DWORD ownerPid = 0;
    if (shell) GetWindowThreadProcessId(shell, &ownerPid);
    if (ownerPid) return ownerPid == GetCurrentProcessId();

    PCWSTR commandLine = GetCommandLineW();
    return commandLine &&
        !wcsstr(commandLine, L"/factory,") &&
        !wcsstr(commandLine, L"-Embedding");
}

static HWND FindSharedRecycleOverlay() {
    if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) return g_hOverlayWnd;
    return FindWindowW(kClassName, nullptr);
}

void Wh_ModUninit();

BOOL Wh_ModInit() {
    g_isDesktopShellHost = IsDesktopShellHostProcess();
    if (!g_isDesktopShellHost) {
        return WindhawkUtils::SetFunctionHook(
                   DoDragDrop, Hooked_DoDragDrop, &pOriginalDoDragDrop)
                   ? TRUE : FALSE;
    }

    ReadSettings();
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

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_isDesktopShellHost) return;

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
