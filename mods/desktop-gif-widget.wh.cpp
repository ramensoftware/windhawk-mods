// ==WindhawkMod==
// @id              desktop-gif-widget
// @name            Desktop GIF Widget
// @description     Animated GIF sticker with one-click toggle between Always-on-Top and Desktop-only modes
// @version         1.0.0
// @author          9Angel99
// @github          https://github.com
// @include         explorer.exe
// @compilerOptions -lgdiplus -lgdi32 -lcomdlg32 -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop GIF Widget

Places an animated GIF sticker that can float above windows (like Discord) or stay behind open windows on the desktop in one click.

### Controls
* **Hover & Click `▲` / `▼`** (or press **Tab**) — toggle between **Always on Top** and **Desktop Only** modes.
* **Hover & Click `✕`** (or press **Esc**) — hide/close the widget.
* **Double-click (LMB)** or **Drag & Drop** — select a `.gif` file.
* **Click & Drag (LMB)** — move the sticker.
* **Mouse Wheel** — smoothly resize (60px – 1200px).
* **Right-click (RMB)** — remove GIF with animation and return to picker.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gifPath: ""
  $name: Path to GIF or image
- size: 0
  $name: Width (px, 0 = auto-save)
- posX: -1
  $name: Position X (-1 = auto-save)
- posY: -1
  $name: Position Y (-1 = auto-save)
- alwaysOnTop: true
  $name: Start Always on Top
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <commdlg.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace Gdiplus;

HWND g_hWnd = nullptr;
HANDLE g_hThread = nullptr;
HANDLE g_hMutex = nullptr;
ULONG_PTR g_gdiplusToken = 0;

Image* g_pImage = nullptr;
GUID g_dimID = FrameDimensionTime;
UINT g_frames = 0;
UINT g_curFrame = 0;
PropertyItem* g_pDelays = nullptr;
float g_ratio = 1.0f;

bool g_hover = false;
float g_pulse = 0.0f;
DWORD g_resizeTick = 0;
DWORD g_clearTick = 0;
int g_curW = 320;
int g_curH = 180;

struct ModState {
    std::wstring path;
    int size;
    int x;
    int y;
    bool top;
} g_st;

const wchar_t* REG_KEY = L"Software\\WindhawkDesktopGifWidget";

void SaveReg() {
    HKEY k;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, nullptr, 0, KEY_WRITE, nullptr, &k, nullptr) == ERROR_SUCCESS) {
        DWORD topVal = g_st.top ? 1 : 0;
        RegSetValueExW(k, L"path", 0, REG_SZ, (BYTE*)g_st.path.c_str(), (DWORD)((g_st.path.size() + 1) * 2));
        RegSetValueExW(k, L"size", 0, REG_DWORD, (BYTE*)&g_st.size, 4);
        RegSetValueExW(k, L"x", 0, REG_DWORD, (BYTE*)&g_st.x, 4);
        RegSetValueExW(k, L"y", 0, REG_DWORD, (BYTE*)&g_st.y, 4);
        RegSetValueExW(k, L"top", 0, REG_DWORD, (BYTE*)&topVal, 4);
        RegCloseKey(k);
    }
}

void LoadAll() {
    g_st = {L"", 300, 200, 200, true};
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ, &k) == ERROR_SUCCESS) {
        std::wstring buf(MAX_PATH, L'\0');
        DWORD bs = MAX_PATH * 2;
        DWORD v = 0;
        DWORD vs = 4;
        if (RegQueryValueExW(k, L"path", nullptr, nullptr, (LPBYTE)buf.data(), &bs) == ERROR_SUCCESS) {
            g_st.path = buf.c_str();
        }
        if (RegQueryValueExW(k, L"size", nullptr, nullptr, (LPBYTE)&v, &vs) == ERROR_SUCCESS && v >= 60 && v <= 1200) {
            g_st.size = v;
        }
        if (RegQueryValueExW(k, L"x", nullptr, nullptr, (LPBYTE)&v, &vs) == ERROR_SUCCESS) {
            g_st.x = v;
        }
        if (RegQueryValueExW(k, L"y", nullptr, nullptr, (LPBYTE)&v, &vs) == ERROR_SUCCESS) {
            g_st.y = v;
        }
        if (RegQueryValueExW(k, L"top", nullptr, nullptr, (LPBYTE)&v, &vs) == ERROR_SUCCESS) {
            g_st.top = (v != 0);
        }
        RegCloseKey(k);
    }

    PCWSTR p = Wh_GetStringSetting(L"gifPath");
    if (p && wcslen(p) > 0) {
        g_st.path = p;
    }
    Wh_FreeStringSetting(p);

    int s = Wh_GetIntSetting(L"size");
    int x = Wh_GetIntSetting(L"posX");
    int y = Wh_GetIntSetting(L"posY");
    if (s >= 60 && s <= 1200) {
        g_st.size = s;
    }
    if (x >= 0) {
        g_st.x = x;
    }
    if (y >= 0) {
        g_st.y = y;
    }
}
void FreeImg() {
    if (g_pImage) {
        delete g_pImage;
        g_pImage = nullptr;
    }
    if (g_pDelays) {
        free(g_pDelays);
        g_pDelays = nullptr;
    }
    g_frames = 0;
    g_curFrame = 0;
    g_ratio = 1.0f;
}

UINT GetDelay(UINT frame) {
    if (g_pDelays && g_pDelays->value) {
        UINT d = (*(((UINT*)g_pDelays->value) + frame)) * 10;
        return d < 20 ? 100 : d;
    }
    return 100;
}

void ApplyZOrder(HWND hWnd) {
    if (g_st.top) {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

void LoadGif(HWND hWnd, const std::wstring& path) {
    KillTimer(hWnd, 1);
    FreeImg();
    if (path.empty()) {
        g_curW = 320;
        g_curH = 180;
        SetWindowPos(hWnd, nullptr, 0, 0, 320, 180, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        ApplyZOrder(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
        return;
    }

    g_pImage = new Image(path.c_str());
    if (g_pImage->GetLastStatus() != Ok) {
        FreeImg();
        InvalidateRect(hWnd, nullptr, TRUE);
        return;
    }

    UINT cnt = g_pImage->GetFrameDimensionsCount();
    if (cnt > 0) {
        std::vector<GUID> ids(cnt);
        g_pImage->GetFrameDimensionsList(ids.data(), cnt);
        g_dimID = ids.at(0);
        g_frames = g_pImage->GetFrameCount(&g_dimID);
        if (g_frames <= 1 && cnt > 1) {
            g_dimID = ids.at(1);
            g_frames = g_pImage->GetFrameCount(&g_dimID);
        }
    }

    UINT ps = g_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
    if (ps > 0) {
        g_pDelays = (PropertyItem*)malloc(ps);
        g_pImage->GetPropertyItem(PropertyTagFrameDelay, ps, g_pDelays);
    }

    int w = g_pImage->GetWidth();
    int h = g_pImage->GetHeight();
    if (w > 0 && h > 0) {
        g_ratio = (float)w / (float)h;
        g_curW = g_st.size;
        g_curH = (int)(g_st.size / g_ratio);
        SetWindowPos(hWnd, nullptr, 0, 0, g_curW, g_curH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    ApplyZOrder(hWnd);
    InvalidateRect(hWnd, nullptr, TRUE);
    if (g_frames > 1) {
        SetTimer(hWnd, 1, GetDelay(0), nullptr);
    }
}

void PickFile(HWND hWnd) {
    std::wstring f(MAX_PATH, L'\0');
    OPENFILENAMEW o = {sizeof(o), hWnd, nullptr, L"GIF & Images\0*.gif;*.png;*.jpg;*.webp\0All Files\0*.*\0", nullptr, 0, 1, f.data(), MAX_PATH};
    o.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameW(&o)) {
        g_st.path = f.c_str();
        SaveReg();
        LoadGif(hWnd, g_st.path);
    }
}

void DrawRoundedBox(Graphics& g, Brush* br, Pen* pen, RectF r, float rad) {
    GraphicsPath p;
    p.AddArc(r.X, r.Y, rad, rad, 180, 90);
    p.AddArc(r.X + r.Width - rad, r.Y, rad, rad, 270, 90);
    p.AddArc(r.X + r.Width - rad, r.Y + r.Height - rad, rad, rad, 0, 90);
    p.AddArc(r.X, r.Y + r.Height - rad, rad, rad, 90, 90);
    p.CloseFigure();
    if (br) {
        g.FillPath(br, &p);
    }
    if (pen) {
        g.DrawPath(pen, &p);
    }
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CREATE) {
        DragAcceptFiles(hWnd, TRUE);
        SetTimer(hWnd, 2, 30, nullptr);
        return 0;
    }
    if (msg == WM_WINDOWPOSCHANGING && !g_st.top) {
        WINDOWPOS* pos = (WINDOWPOS*)lp;
        if (!(pos->flags & SWP_NOZORDER)) {
            pos->hwndInsertAfter = HWND_BOTTOM;
        }
    }
    if (msg == WM_MOUSEMOVE) {
        if (!g_hover) {
            g_hover = true;
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
            TrackMouseEvent(&tme);
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    }
    if (msg == WM_MOUSELEAVE) {
        g_hover = false;
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }
    if (msg == WM_KEYDOWN) {
        if (wp == VK_ESCAPE) {
            ShowWindow(hWnd, SW_HIDE);
            return 0;
        }
        if (wp == VK_TAB) {
            g_st.top = !g_st.top;
            ApplyZOrder(hWnd);
            SaveReg();
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
    }
    if (msg == WM_DROPFILES) {
        std::wstring f(MAX_PATH, L'\0');
        if (DragQueryFileW((HDROP)wp, 0, f.data(), MAX_PATH)) {
            g_st.path = f.c_str();
            SaveReg();
            LoadGif(hWnd, g_st.path);
        }
        DragFinish((HDROP)wp);
        return 0;
    }
    if (msg == WM_LBUTTONDOWN) {
        int mx = LOWORD(lp);
        int my = HIWORD(lp);
        RECT rc;
        GetClientRect(hWnd, &rc);
        if (mx >= rc.right - 30 && my <= 30) {
            ShowWindow(hWnd, SW_HIDE);
            return 0;
        }
        if (mx >= rc.right - 58 && mx < rc.right - 30 && my <= 30) {
            g_st.top = !g_st.top;
            ApplyZOrder(hWnd);
            SaveReg();
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        SetFocus(hWnd);
        ReleaseCapture();
        SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        GetWindowRect(hWnd, &rc);
        g_st.x = rc.left;
        g_st.y = rc.top;
        SaveReg();
        ApplyZOrder(hWnd);
        return 0;
    }
    if (msg == WM_LBUTTONDBLCLK) {
        PickFile(hWnd);
        return 0;
    }
    if (msg == WM_RBUTTONDOWN) {
        g_clearTick = GetTickCount();
        g_st.path = L"";
        SaveReg();
        LoadGif(hWnd, L"");
        return 0;
    }
    if (msg == WM_MOUSEWHEEL && g_pImage) {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        int cw = rc.right - rc.left;
        int step = (std::max)(15, cw / 10);
        int nw = (std::max)(60, (std::min)(1200, cw + (GET_WHEEL_DELTA_WPARAM(wp) > 0 ? step : -step)));
        g_st.size = nw;
        g_curW = nw;
        g_curH = (int)(nw / g_ratio);
        g_resizeTick = GetTickCount();
        SaveReg();
        SetWindowPos(hWnd, nullptr, 0, 0, g_curW, g_curH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;
    }
    if (msg == WM_TIMER) {
        if (wp == 1 && g_pImage && g_frames > 1) {
            g_curFrame = (g_curFrame + 1) % g_frames;
            g_pImage->SelectActiveFrame(&g_dimID, g_curFrame);
            InvalidateRect(hWnd, nullptr, FALSE);
            SetTimer(hWnd, 1, GetDelay(g_curFrame), nullptr);
        } else if (wp == 2) {
            g_pulse += 0.14f;
            bool anim = !g_pImage || (GetTickCount() - g_resizeTick) < 900 || (GetTickCount() - g_clearTick) < 600;
            if (anim) {
                InvalidateRect(hWnd, nullptr, FALSE);
            }
        }
        return 0;
    }
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        HDC mdc = CreateCompatibleDC(hdc);
        HBITMAP mbmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP obmp = (HBITMAP)SelectObject(mdc, mbmp);
        HBRUSH br = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(mdc, &rc, br);
        DeleteObject(br);

        Graphics g(mdc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        FontFamily ff(L"Segoe UI");
        StringFormat fmt;
        fmt.SetAlignment(StringAlignmentCenter);
        fmt.SetLineAlignment(StringAlignmentCenter);

        if (g_pImage) {
            g.DrawImage(g_pImage, 0, 0, rc.right, rc.bottom);
            DWORD dt = GetTickCount() - g_resizeTick;
            if (dt < 900 && rc.right > 90) {
                int alpha = (int)(220 * (1.0f - (float)dt / 900.0f));
                SolidBrush pill(Color(alpha, 20, 20, 25));
                SolidBrush txt(Color(alpha, 255, 255, 255));
                Font f(&ff, 11, FontStyleBold, UnitPixel);
                RectF pr((rc.right - 90) / 2.0f, (rc.bottom - 24) / 2.0f, 90, 24);
                DrawRoundedBox(g, &pill, nullptr, pr, 12);
                std::wstring dimText = std::to_wstring(rc.right) + L" × " + std::to_wstring(rc.bottom);
                g.DrawString(dimText.c_str(), -1, &f, pr, &fmt, &txt);
            }
        } else {
            float s = sinf(g_pulse);
            DWORD cdt = GetTickCount() - g_clearTick;
            int rCol = (cdt < 600) ? 220 : 138;
            int gCol = (cdt < 600) ? 70 : 100;
            int bCol = g_hover ? 255 : (int)(160 + 50 * s);
            SolidBrush card(Color(255, 26, 26, 32));
            Pen border(Color(255, rCol, gCol, bCol), g_hover ? 2.5f : 1.5f);
            DrawRoundedBox(g, &card, &border, RectF(2, 2, rc.right - 4.0f, rc.bottom - 4.0f), 20);

            float sz = 44.0f + 5.0f * s;
            SolidBrush box(Color(255, rCol, gCol, 246));
            RectF ir((rc.right - sz) / 2.0f, 24.0f - 2.5f * s, sz, sz);
            DrawRoundedBox(g, &box, nullptr, ir, 12);

            Font f1(&ff, 12, FontStyleBold, UnitPixel);
            Font f2(&ff, 11, FontStyleRegular, UnitPixel);
            SolidBrush wb(Color(255, 255, 255, 255));
            SolidBrush gb(Color(255, 205, 205, 215));
            g.DrawString(L"GIF", -1, &f1, ir, &fmt, &wb);
            g.DrawString(L"2x Click / Drop GIF — load\nDrag — move | Wheel — resize\nTop-right: Layer toggle & Close", -1, &f2, RectF(10, 78, rc.right - 20.0f, 80), &fmt, &gb);
        }

        if (g_hover) {
            Font cf(&ff, 11, FontStyleBold, UnitPixel);
            SolidBrush cw(Color(255, 255, 255, 255));
            SolidBrush lb(g_st.top ? Color(230, 110, 80, 230) : Color(230, 50, 150, 90));
            RectF lr(rc.right - 56.0f, 4.0f, 24.0f, 24.0f);
            DrawRoundedBox(g, &lb, nullptr, lr, 12);
            g.DrawString(g_st.top ? L"▲" : L"▼", -1, &cf, lr, &fmt, &cw);

            SolidBrush cb(Color(230, 220, 55, 65));
            RectF cr(rc.right - 28.0f, 4.0f, 24.0f, 24.0f);
            DrawRoundedBox(g, &cb, nullptr, cr, 12);
            g.DrawString(L"✕", -1, &cf, cr, &fmt, &cw);
        }

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, mdc, 0, 0, SRCCOPY);
        SelectObject(mdc, obmp);
        DeleteObject(mbmp);
        DeleteDC(mdc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    if (msg == WM_DESTROY) {
        KillTimer(hWnd, 1);
        KillTimer(hWnd, 2);
        FreeImg();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wp, lp);
}

DWORD WINAPI GuiThread(LPVOID) {
    GdiplusStartupInput in;
    GdiplusStartup(&g_gdiplusToken, &in, nullptr);
    WNDCLASSEXW wc = {sizeof(wc), CS_DBLCLKS, WndProc, 0, 0, GetModuleHandleW(nullptr), nullptr, LoadCursor(nullptr, IDC_SIZEALL), nullptr, nullptr, L"WhDesktopGifCls", nullptr};
    RegisterClassExW(&wc);
    g_hWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW, wc.lpszClassName, L"Desktop GIF", WS_POPUP | WS_VISIBLE, g_st.x, g_st.y, 320, 180, nullptr, nullptr, wc.hInstance, nullptr);
    SetLayeredWindowAttributes(g_hWnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
    ApplyZOrder(g_hWnd);
    if (!g_st.path.empty()) {
        LoadGif(g_hWnd, g_st.path);
    }
    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0)) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

BOOL Wh_ModInit() {
    DWORD pid = 0;
    HWND p = FindWindowW(L"Progman", nullptr);
    if (p) {
        GetWindowThreadProcessId(p, &pid);
    }
    if (pid != 0 && pid != GetCurrentProcessId()) {
        return FALSE;
    }
    g_hMutex = CreateMutexW(nullptr, TRUE, L"Local\\WhDesktopGifSingle");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        if (g_hMutex) {
            CloseHandle(g_hMutex);
        }
        return FALSE;
    }
    LoadAll();
    g_hThread = CreateThread(nullptr, 0, GuiThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hWnd) {
        PostMessageW(g_hWnd, WM_CLOSE, 0, 0);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
    }
    if (g_hMutex) {
        ReleaseMutex(g_hMutex);
        CloseHandle(g_hMutex);
    }
}

void Wh_ModSettingsChanged() {
    LoadAll();
    if (g_hWnd) {
        ShowWindow(g_hWnd, SW_SHOW);
        ApplyZOrder(g_hWnd);
        LoadGif(g_hWnd, g_st.path);
    }
}
