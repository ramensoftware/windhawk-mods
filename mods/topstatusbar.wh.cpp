// ==WindhawkMod==
// @id              topstatusbar
// @name            Top Status Bar
// @description     Minimalist top status bar with battery, resource monitoring, and smart system app detection.
// @version         1.1.0
// @author          AlexanderOG
// @github          https://github.com/alexandr0g/windhawk-mod-topstatusbar
// @include         explorer.exe
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lshell32 -ld2d1 -ldwrite -lole32 -lwindowscodecs -lshlwapi
// ==/WindhawkMod==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <vector>
#include <algorithm>

// --- CONSTANTES ---
#define BAR_HEIGHT 30
#define BTN_H 24
#define HIDE_OFFSET -55.0f
#define MOUSE_TRIGGER_ZONE 2
#define MARGIN_TOP 4.0f
#define TIMER_ID_MAIN 1
#define TIMER_ID_CLOCK 2
#define REFRESH_RATE 30 

// --- ESTRUCTURAS DE DATOS ---
struct SystemCache {
    BOOL darkMode, transparency;
    D2D1_COLOR_F textColor, hoverColor, pressColor, clockBgCol, grayLowCol;
    WCHAR userName[128], resourceString[128];
    FILETIME preIdle, preKernel, preUser;
    int cpuUsage = 0, ssdUsage = 0, ramUsage = 0;

    void Update() {
        DWORD v = 0, s = sizeof(v);
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &v, &s);
        darkMode = (v == 0);
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"EnableTransparency", RRF_RT_REG_DWORD, NULL, &v, &s);
        transparency = (v != 0);

        textColor = darkMode ? D2D1::ColorF(0.95f, 0.95f, 0.95f) : D2D1::ColorF(0.18f, 0.18f, 0.18f);
        hoverColor = D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.5f); 
        pressColor = D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.1f); 
        grayLowCol = D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.3f);
        clockBgCol = D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.2f); 

        DWORD unSize = 128; GetUserNameW(userName, &unSize);
        UpdateResources();
    }

    void UpdateResources() {
        FILETIME idle, kernel, user;
        if (GetSystemTimes(&idle, &kernel, &user)) {
            ULONGLONG i = (((ULONGLONG)idle.dwHighDateTime << 32) | idle.dwLowDateTime) - (((ULONGLONG)preIdle.dwHighDateTime << 32) | preIdle.dwLowDateTime);
            ULONGLONG k = (((ULONGLONG)kernel.dwHighDateTime << 32) | kernel.dwLowDateTime) - (((ULONGLONG)preKernel.dwHighDateTime << 32) | preKernel.dwLowDateTime);
            ULONGLONG u = (((ULONGLONG)user.dwHighDateTime << 32) | user.dwLowDateTime) - (((ULONGLONG)preUser.dwHighDateTime << 32) | preUser.dwLowDateTime);
            if (k + u > 0) cpuUsage = (int)((float)(k + u - i) * 100.0f / (float)(k + u));
            preIdle = idle; preKernel = kernel; preUser = user;
        }
        MEMORYSTATUSEX ms = {sizeof(ms)}; GlobalMemoryStatusEx(&ms); ramUsage = (int)ms.dwMemoryLoad;
        ULARGE_INTEGER fA, tB, tF;
        if (GetDiskFreeSpaceExW(L"C:", &fA, &tB, &tF)) { if (tB.QuadPart > 0) ssdUsage = (int)((tB.QuadPart - fA.QuadPart) * 100 / tB.QuadPart); }
        wsprintfW(resourceString, L"Cpu %d%% | Ram %d%% | Ssd %d%%", cpuUsage, ramUsage, ssdUsage);
    }
} g_cache;

struct BarInstance {
    HWND hwndBar, hwndTrigger;
    HMONITOR hMonitor;
    RECT monitorRect;
    ID2D1HwndRenderTarget* pRT = NULL;
    ID2D1SolidColorBrush *pTextBr = NULL, *pHoverBr = NULL, *pPressBr = NULL, *pClockBgBr = NULL, *pGrayBr = NULL;
    IDWriteTextFormat *pTextFmt = NULL, *pIconFmt = NULL, *pSmallFmt = NULL, *pTitleFmt = NULL;
    ID2D1Bitmap *pCachedIcon = NULL;
    HWND cachedHWND = NULL;
    float currentY = MARGIN_TOP, targetY = MARGIN_TOP;
    bool isAnimating = false, mouseTracked = false;

    void ReleaseBrushes() {
        if (pTextBr) pTextBr->Release(); pTextBr = NULL;
        if (pHoverBr) pHoverBr->Release(); pHoverBr = NULL;
        if (pPressBr) pPressBr->Release(); pPressBr = NULL;
        if (pClockBgBr) pClockBgBr->Release(); pClockBgBr = NULL;
        if (pGrayBr) pGrayBr->Release(); pGrayBr = NULL;
        if (pCachedIcon) pCachedIcon->Release(); pCachedIcon = NULL;
        cachedHWND = NULL;
    }

    void Release() {
        ReleaseBrushes();
        if (pRT) { pRT->Release(); pRT = NULL; }
        if (pTextFmt) { pTextFmt->Release(); pTextFmt = NULL; }
        if (pIconFmt) { pIconFmt->Release(); pIconFmt = NULL; }
        if (pSmallFmt) { pSmallFmt->Release(); pSmallFmt = NULL; }
        if (pTitleFmt) { pTitleFmt->Release(); pTitleFmt = NULL; }
    }
};

ID2D1Factory* g_pD2DFactory = NULL;
IDWriteFactory* g_pDWriteFactory = NULL;
IWICImagingFactory* g_pWICFactory = NULL;
std::vector<BarInstance*> g_bars;

// --- UTILIDADES ---

bool IsProtocolAvailable(LPCWSTR protocol) {
    WCHAR dummy[256];
    DWORD size = 256;
    // Verifica si hay una aplicación asociada al protocolo sin abrir la tienda
    HRESULT hr = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE, protocol, NULL, dummy, &size);
    return SUCCEEDED(hr);
}

void SendKeys(std::vector<WORD> keys) {
    std::vector<INPUT> in;
    for (auto k : keys) { 
        INPUT i = {0}; 
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = k; 
        in.push_back(i); 
    }
    for (auto it = keys.rbegin(); it != keys.rend(); ++it) { 
        INPUT i = {0}; 
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = *it; 
        i.ki.dwFlags = KEYEVENTF_KEYUP; 
        in.push_back(i); 
    }
    SendInput((UINT)in.size(), in.data(), sizeof(INPUT));
}

void DrawLightning(ID2D1HwndRenderTarget* pRT, float x, float y, float h, ID2D1Brush* pBr) {
    ID2D1PathGeometry* pG; g_pD2DFactory->CreatePathGeometry(&pG);
    ID2D1GeometrySink* pS; pG->Open(&pS);
    float w = h * 0.6f;
    pS->BeginFigure({x + w*0.6f, y}, D2D1_FIGURE_BEGIN_FILLED);
    pS->AddLine({x, y + h*0.55f});
    pS->AddLine({x + w*0.45f, y + h*0.55f});
    pS->AddLine({x + w*0.25f, y + h});
    pS->AddLine({x + w, y + h*0.4f});
    pS->AddLine({x + w*0.55f, y + h*0.4f});
    pS->AddLine({x + w*0.8f, y});
    pS->EndFigure(D2D1_FIGURE_END_CLOSED); pS->Close();
    pRT->FillGeometry(pG, pBr); pS->Release(); pG->Release();
}

ID2D1Bitmap* CreateSymbolicIcon(ID2D1HwndRenderTarget* pRT, HICON hI) {
    if (!hI || !g_pWICFactory) return NULL;
    IWICBitmap* pWic = NULL; g_pWICFactory->CreateBitmapFromHICON(hI, &pWic);
    IWICFormatConverter* pConv = NULL; g_pWICFactory->CreateFormatConverter(&pConv);
    pConv->Initialize(pWic, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
    UINT w, h; pConv->GetSize(&w, &h); std::vector<UINT32> pxs(w * h); pConv->CopyPixels(NULL, w * 4, w * h * 4, (BYTE*)pxs.data());
    for (auto& p : pxs) {
        BYTE a = (p >> 24) & 0xFF; if (a < 10) { p = 0; continue; }
        BYTE r = (p >> 16) & 0xFF, g = (p >> 8) & 0xFF, b = p & 0xFF;
        float luma = (0.299f*r + 0.587f*g + 0.114f*b);
        BYTE gray = g_cache.darkMode ? (BYTE)std::min(255.0f, luma * 1.1f + 60.0f) : (BYTE)std::min(255.0f, luma * 0.5f);
        p = (a << 24) | (gray << 16) | (gray << 8) | gray;
    }
    ID2D1Bitmap* pD2D; pRT->CreateBitmap(D2D1::SizeU(w, h), pxs.data(), w * 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pD2D);
    pConv->Release(); pWic->Release(); return pD2D;
}

void CreateBarResources(BarInstance* b) {
    if (!b->pRT) {
        RECT rc; GetClientRect(b->hwndBar, &rc);
        g_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED}), D2D1::HwndRenderTargetProperties(b->hwndBar, D2D1::SizeU(rc.right, rc.bottom)), &b->pRT);
    }
    if (!b->pTextBr) b->pRT->CreateSolidColorBrush(g_cache.textColor, &b->pTextBr);
    if (!b->pHoverBr) b->pRT->CreateSolidColorBrush(g_cache.hoverColor, &b->pHoverBr);
    if (!b->pPressBr) b->pRT->CreateSolidColorBrush(g_cache.pressColor, &b->pPressBr);
    if (!b->pClockBgBr) b->pRT->CreateSolidColorBrush(g_cache.clockBgCol, &b->pClockBgBr);
    if (!b->pGrayBr) b->pRT->CreateSolidColorBrush(g_cache.grayLowCol, &b->pGrayBr);

    if (!b->pTextFmt) {
        float s = GetDpiForWindow(b->hwndBar) / 96.0f;
        g_pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f*s, L"es-es", &b->pTextFmt);
        g_pDWriteFactory->CreateTextFormat(L"Segoe MDL2 Assets", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13.0f*s, L"es-es", &b->pIconFmt);
        g_pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f*s, L"es-es", &b->pSmallFmt);
        g_pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f*s, L"es-es", &b->pTitleFmt);
        for(auto f : {b->pTextFmt, b->pIconFmt, b->pSmallFmt, b->pTitleFmt}) if(f) {
            f->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        b->pTitleFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        DWRITE_TRIMMING trim = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 }; b->pTitleFmt->SetTrimming(&trim, NULL);
    }
}

// --- GESTOR DE MÓDULOS ---
struct ModuleManager {
    int hoveredID = -1, pressedID = -1;
    struct ModRect { int id; D2D1_RECT_F rect; };
    std::vector<ModRect> rects;

    void DrawClockPiece(ID2D1HwndRenderTarget* pRT, D2D1_RECT_F r, ID2D1Brush* pBr, bool left, float rad) {
        ID2D1PathGeometry* pP; g_pD2DFactory->CreatePathGeometry(&pP); ID2D1GeometrySink* pS; pP->Open(&pS);
        if (left) {
            pS->BeginFigure({r.right, r.top}, D2D1_FIGURE_BEGIN_FILLED);
            pS->AddLine({r.left + rad, r.top}); pS->AddArc(D2D1::ArcSegment({r.left, r.top + rad}, {rad, rad}, 0, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            pS->AddLine({r.left, r.bottom - rad}); pS->AddArc(D2D1::ArcSegment({r.left + rad, r.bottom}, {rad, rad}, 0, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            pS->AddLine({r.right, r.bottom});
        } else {
            pS->BeginFigure({r.left, r.top}, D2D1_FIGURE_BEGIN_FILLED);
            pS->AddLine({r.right - rad, r.top}); pS->AddArc(D2D1::ArcSegment({r.right, r.top + rad}, {rad, rad}, 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            pS->AddLine({r.right, r.bottom - rad}); pS->AddArc(D2D1::ArcSegment({r.right - rad, r.bottom}, {rad, rad}, 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            pS->AddLine({r.left, r.bottom});
        }
        pS->EndFigure(D2D1_FIGURE_END_CLOSED); pS->Close(); pRT->FillGeometry(pP, pBr); pS->Release(); pP->Release();
    }

    void Render(BarInstance* b, HWND fg) {
        rects.clear();
        ID2D1HwndRenderTarget* pRT = b->pRT; float s = GetDpiForWindow(b->hwndBar) / 96.0f;
        float barW = (float)(b->monitorRect.right - b->monitorRect.left - 16);
        float offY = (BAR_HEIGHT * s - BTN_H * s) / 2.0f, btnH = BTN_H * s;

        auto DrawBtnBase = [&](D2D1_RECT_F r, int id) {
            if (pressedID == id) pRT->FillRoundedRectangle(D2D1::RoundedRect(r, 6*s, 6*s), b->pPressBr);
            else if (hoveredID == id) pRT->FillRoundedRectangle(D2D1::RoundedRect(r, 6*s, 6*s), b->pHoverBr);
        };

        // IZQUIERDA
        float nX = 4.0f * s;
        auto AddL = [&](float w, const wchar_t* lbl, int id, bool icn = true) {
            float fw = w * s; D2D1_RECT_F r = D2D1::RectF(nX, offY, nX + fw, offY + btnH); DrawBtnBase(r, id);
            if (id == 10) { 
                float sw = 5.5f*s, sx = r.left + (fw - sw*2 - 1*s)/2.0f, sy = r.top + (btnH - sw*2 - 1*s)/2.0f;
                pRT->FillRectangle({sx, sy, sx+sw, sy+sw}, b->pTextBr); pRT->FillRectangle({sx+sw+1*s, sy, sx+sw*2+1*s, sy+sw}, b->pTextBr);
                pRT->FillRectangle({sx, sy+sw+1*s, sx+sw, sy+sw*2+1*s}, b->pTextBr); pRT->FillRectangle({sx+sw+1*s, sy+sw+1*s, sx+sw*2+1*s, sy+sw*2+1*s}, b->pTextBr);
            } else if (id == 11) { 
                float ax = r.left + 13*s, ay = r.top + 6*s; pRT->FillRectangle({ax, ay, ax+5*s, ay+12*s}, b->pTextBr);
                pRT->FillRectangle({ax+6*s, ay, ax+11*s, ay+5*s}, b->pTextBr); pRT->FillRectangle({ax+6*s, ay+7*s, ax+11*s, ay+12*s}, b->pTextBr);
            } else pRT->DrawText(lbl, (UINT32)wcslen(lbl), icn ? b->pIconFmt : b->pTextFmt, r, b->pTextBr);
            rects.push_back({id, r}); nX += fw + 4*s;
        };

        AddL(38, L"", 10); AddL(38, L"", 11); AddL(38, L"\uE7C4", 12);
        AddL(100, g_cache.userName, 13, false); AddL(36, L"\uE713", 14);
        AddL(36, L"\uE838", 15); AddL(36, L"\uE774", 16);

        WCHAR szT[256]; GetWindowTextW(fg, szT, 256);
        D2D1_RECT_F rT = D2D1::RectF(nX, offY, nX + 180*s, offY + btnH); DrawBtnBase(rT, 17);
        if (fg != b->cachedHWND) {
            if (b->pCachedIcon) b->pCachedIcon->Release();
            HICON hI = (HICON)SendMessage(fg, WM_GETICON, ICON_SMALL2, 0); if(!hI) hI = (HICON)GetClassLongPtr(fg, GCLP_HICONSM);
            b->pCachedIcon = CreateSymbolicIcon(b->pRT, hI); b->cachedHWND = fg;
        }
        if (b->pCachedIcon) pRT->DrawBitmap(b->pCachedIcon, {rT.left + 6*s, rT.top + 4*s, rT.left + 22*s, rT.top + 20*s});
        pRT->DrawText(szT, (UINT32)wcslen(szT), b->pTitleFmt, {rT.left + 28*s, rT.top, rT.right - 4*s, rT.bottom}, b->pTextBr);
        rects.push_back({17, rT});

        // DERECHA
        float pX = barW - 4.0f * s;
        auto AddR = [&](float w, const wchar_t* lbl, int id, bool icn = true) {
            pX -= w*s; D2D1_RECT_F r = D2D1::RectF(pX, offY, pX+w*s, offY+btnH); DrawBtnBase(r, id);
            pRT->DrawText(lbl, (UINT32)wcslen(lbl), icn ? b->pIconFmt : b->pTextFmt, r, b->pTextBr);
            rects.push_back({id, r}); pX -= 4*s;
        };

        AddR(36, L"\uE7E8", 108); AddR(36, L"\uE7E7", 100);
        
        // MÓDULO BATERÍA
        pX -= 76*s; D2D1_RECT_F rB = D2D1::RectF(pX, offY, pX+76*s, offY+btnH); DrawBtnBase(rB, 107);
        SYSTEM_POWER_STATUS sps; GetSystemPowerStatus(&sps); int pct = std::min((int)sps.BatteryLifePercent, 100);
        float bStart = rB.left + 5*s;
        if (sps.ACLineStatus == 1) { 
            DrawLightning(pRT, bStart, offY + 6.5f*s, 11*s, b->pTextBr);
            bStart += 12*s;
        }
        float bw = 19*s, bh = 9*s, bx = bStart, by = rB.top + (btnH-bh)/2.0f;
        pRT->DrawRectangle(D2D1::RectF(bx, by, bx+bw, by+bh), b->pTextBr, 1.2f); 
        pRT->FillRectangle(D2D1::RectF(bx+bw, by+2.5f*s, bx+bw+2*s, by+bh-2.5f*s), b->pTextBr); 
        float fillW = (bw) * pct / 100.0f; 
        if (fillW > 0) pRT->FillRectangle(D2D1::RectF(bx, by, bx+fillW, by+bh), b->pTextBr);
        
        WCHAR pS[8]; wsprintfW(pS, L"%d%%", pct);
        pRT->DrawText(pS, (UINT32)wcslen(pS), b->pSmallFmt, D2D1::RectF(bx+bw+2*s, offY, rB.right, offY+btnH), b->pTextBr);
        rects.push_back({107, rB}); pX -= 4*s;

        AddR(34, L"\uE767", 103); AddR(34, L"\uE701", 106);
        AddR(34, L"\uE702", 105); AddR(34, L"\uE706", 104);
        AddR(200, g_cache.resourceString, 102, false);

        // CENTRO
        float mid = barW / 2.0f;
        WCHAR tT[16], tD[64]; GetDateFormatW(LOCALE_USER_DEFAULT, 0, NULL, L"dd MMM yyyy", tD, 64); GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, tT, 16);
        D2D1_RECT_F rd = D2D1::RectF(mid-96*s, offY, mid-1*s, offY+btnH), rt = D2D1::RectF(mid+1*s, offY, mid+65*s, offY+btnH);
        DrawClockPiece(pRT, rd, b->pClockBgBr, true, 8*s); DrawClockPiece(pRT, rt, b->pClockBgBr, false, 8*s);
        if(hoveredID == 0) DrawClockPiece(pRT, rd, b->pHoverBr, true, 8*s); if(hoveredID == 1) DrawClockPiece(pRT, rt, b->pHoverBr, false, 8*s);
        pRT->DrawText(tD, (UINT32)wcslen(tD), b->pTextFmt, rd, b->pTextBr); pRT->DrawText(tT, (UINT32)wcslen(tT), b->pTextFmt, rt, b->pTextBr);
        rects.push_back({0, rd}); rects.push_back({1, rt});
    }

    void HandleMouse(HWND h, UINT m, POINT p) {
        int oldH = hoveredID; hoveredID = -1;
        for (auto& r : rects) if (p.x >= r.rect.left && p.x <= r.rect.right && p.y >= r.rect.top && p.y <= r.rect.bottom) { hoveredID = r.id; break; }
        if (m == WM_LBUTTONDOWN) pressedID = hoveredID;
        if (m == WM_LBUTTONUP && pressedID != -1 && pressedID == hoveredID) {
            switch(pressedID) {
                case 0: // Clic en Fecha (Calendario)
                    if (IsProtocolAvailable(L"outlookcal:")) ShellExecuteW(NULL, L"open", L"outlookcal:", NULL, NULL, SW_SHOWNORMAL);
                    else if (IsProtocolAvailable(L"ms-calendar:")) ShellExecuteW(NULL, L"open", L"ms-calendar:", NULL, NULL, SW_SHOWNORMAL);
                    else ShellExecuteW(NULL, L"open", L"https://calendar.google.com", NULL, NULL, SW_SHOWNORMAL);
                    break;
                case 1: // Clic en Hora (Reloj)
                    if (IsProtocolAvailable(L"ms-clock:")) ShellExecuteW(NULL, L"open", L"ms-clock:", NULL, NULL, SW_SHOWNORMAL);
                    else ShellExecuteW(NULL, L"open", L"ms-settings:dateandtime", NULL, NULL, SW_SHOWNORMAL);
                    break;
                case 10: SendKeys({VK_LWIN}); break;
                case 11: SendKeys({VK_LWIN, VK_TAB}); break;
                case 12: SendKeys({VK_CONTROL, VK_MENU, VK_TAB}); break;
                case 13: ShellExecuteW(NULL, L"open", L"ms-settings:yourinfo", NULL, NULL, SW_SHOWNORMAL); break;
                case 14: ShellExecuteW(NULL, L"open", L"ms-settings:home", NULL, NULL, SW_SHOWNORMAL); break;
                case 15: ShellExecuteW(NULL, L"open", L"explorer.exe", NULL, NULL, SW_SHOWNORMAL); break;
                case 16: ShellExecuteW(NULL, L"open", L"http:", NULL, NULL, SW_SHOWNORMAL); break;
                case 102: ShellExecuteW(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_SHOWNORMAL); break;
                case 103: ShellExecuteW(NULL, L"open", L"ms-settings:apps-volume", NULL, NULL, SW_SHOWNORMAL); break;
                case 104: ShellExecuteW(NULL, L"open", L"ms-settings:display", NULL, NULL, SW_SHOWNORMAL); break;
                case 105: ShellExecuteW(NULL, L"open", L"ms-settings:bluetooth", NULL, NULL, SW_SHOWNORMAL); break;
                case 106: ShellExecuteW(NULL, L"open", L"ms-settings:network", NULL, NULL, SW_SHOWNORMAL); break;
                case 107: ShellExecuteW(NULL, L"open", L"ms-settings:batterysaver", NULL, NULL, SW_SHOWNORMAL); break;
                case 108: ShellExecuteW(NULL, L"open", L"powershell.exe", L"-Command \"(New-Object -ComObject Shell.Application).ShutdownWindows()\"", NULL, SW_HIDE); break;
            } pressedID = -1;
        }
        if (oldH != hoveredID || m == WM_LBUTTONDOWN || m == WM_LBUTTONUP) InvalidateRect(h, NULL, FALSE);
    }
} g_modManager;

// --- LÓGICA DE VENTANA ---
void SyncDwmStyle(HWND h) {
    DwmSetWindowAttribute(h, 20, &g_cache.darkMode, sizeof(BOOL));
    DWORD bdr = g_cache.transparency ? 3 : 4; DwmSetWindowAttribute(h, 38, &bdr, sizeof(bdr));
    DWORD crn = 2; DwmSetWindowAttribute(h, 33, &crn, sizeof(crn));
    COLORREF bord = 0xFFFFFFFF, cap = 0xFFFFFFFE; DwmSetWindowAttribute(h, 34, &bord, sizeof(bord)); DwmSetWindowAttribute(h, 35, &cap, sizeof(cap));
    MARGINS m = {-1,-1,-1,-1}; DwmExtendFrameIntoClientArea(h, &m);
    SetWindowPos(h, NULL, 0,0,0,0, SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}

void UpdateBarState(BarInstance* b) {
    HWND fg = GetForegroundWindow(); 
    POINT pt; GetCursorPos(&pt);
    LONG style = GetWindowLong(fg, GWL_STYLE);
    bool isAppWindow = (style & WS_CAPTION) == WS_CAPTION;
    bool isMaximized = (style & WS_MAXIMIZE) == WS_MAXIMIZE; 
    WCHAR cls[128]; GetClassNameW(fg, cls, 128);
    bool isSystem = (wcscmp(cls, L"WorkerW") == 0 || wcscmp(cls, L"Progman") == 0 || 
                     wcscmp(cls, L"Shell_TrayWnd") == 0 || wcscmp(cls, L"#32768") == 0 ||
                     wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 || 
                     wcscmp(cls, L"NotifyIconOverflowWindow") == 0);

    bool mouseIn = (pt.x >= (float)b->monitorRect.left && pt.x <= (float)b->monitorRect.right);
    bool over = (b->currentY <= HIDE_OFFSET + 5) ? 
                (mouseIn && pt.y >= (float)b->monitorRect.top && pt.y <= (float)b->monitorRect.top + MOUSE_TRIGGER_ZONE) : 
                (mouseIn && pt.y >= (float)b->monitorRect.top && pt.y <= (float)b->monitorRect.top + BAR_HEIGHT + 4);
    
    bool dodge = false;
    if (!isSystem && fg && IsWindow(fg) && MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST) == b->hMonitor) {
        if (isMaximized) dodge = true;
        else if (isAppWindow) {
            RECT rc; GetWindowRect(fg, &rc); 
            if (rc.top <= (b->monitorRect.top + BAR_HEIGHT + 4)) dodge = true;
        }
    }
    b->targetY = (over || !dodge) ? MARGIN_TOP : HIDE_OFFSET;
    float diff = b->targetY - b->currentY;
    if (abs(diff) > 0.05f) {
        b->isAnimating = true;
        if (abs(diff) < 0.8f) b->currentY = b->targetY;
        else b->currentY += (diff * 0.25f);
        SetWindowPos(b->hwndBar, HWND_TOPMOST, b->monitorRect.left+8, b->monitorRect.top+(int)b->currentY, (b->monitorRect.right-b->monitorRect.left)-16, BAR_HEIGHT, SWP_NOACTIVATE|SWP_ASYNCWINDOWPOS|SWP_NOCOPYBITS);
    } else if (b->isAnimating) { b->isAnimating = false; b->mouseTracked = false; InvalidateRect(b->hwndBar, NULL, FALSE); }
}

LRESULT CALLBACK BarWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    BarInstance* b = (BarInstance*)GetWindowLongPtr(h, GWLP_USERDATA);
    switch (m) {
        case WM_NCACTIVATE: return DefWindowProc(h, m, TRUE, l);
        case WM_MOUSEMOVE: {
            if (!b->mouseTracked) { TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, h, 0}; TrackMouseEvent(&tme); b->mouseTracked = true; }
            POINT p = {(int)(short)LOWORD(l), (int)(short)HIWORD(l)}; g_modManager.HandleMouse(h, m, p); return 0;
        }
        case WM_MOUSELEAVE: { b->mouseTracked = false; g_modManager.hoveredID = -1; g_modManager.pressedID = -1; InvalidateRect(h, NULL, FALSE); return 0; }
        case WM_LBUTTONDOWN: case WM_LBUTTONUP: { POINT p = {(int)(short)LOWORD(l), (int)(short)HIWORD(l)}; g_modManager.HandleMouse(h, m, p); return 0; }
        case WM_TIMER: if (w == TIMER_ID_MAIN && b) UpdateBarState(b); if (w == TIMER_ID_CLOCK) { g_cache.UpdateResources(); if(b && !b->isAnimating) InvalidateRect(h, NULL, FALSE); } return 0;
        case WM_PAINT: {
            if (!b) return 0;
            if (b->isAnimating) { ValidateRect(h, NULL); return 0; }
            CreateBarResources(b); b->pRT->BeginDraw(); b->pRT->Clear(D2D1::ColorF(0,0,0,0));
            g_modManager.Render(b, GetForegroundWindow()); b->pRT->EndDraw(); ValidateRect(h, NULL); return 0;
        }
        case WM_SETTINGCHANGE: g_cache.Update(); if(b) { b->ReleaseBrushes(); SyncDwmStyle(h); InvalidateRect(h, NULL, FALSE); } return 0;
        case WM_DESTROY: if (b) b->Release(); return 0;
    } return DefWindowProc(h, m, w, l);
}

BOOL CALLBACK MonitorEnum(HMONITOR hM, HDC, LPRECT, LPARAM) {
    MONITORINFO mi = {sizeof(mi)}; GetMonitorInfo(hM, &mi);
    BarInstance* b = new BarInstance(); b->hMonitor = hM; b->monitorRect = mi.rcMonitor;
    b->hwndTrigger = CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TRANSPARENT, L"TopT", NULL, WS_POPUP|WS_VISIBLE, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right-mi.rcMonitor.left, MOUSE_TRIGGER_ZONE, NULL, NULL, GetModuleHandle(NULL), NULL);
    b->hwndBar = CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_LAYERED, L"TopC", NULL, WS_POPUP|WS_VISIBLE, mi.rcMonitor.left+8, mi.rcMonitor.top+(int)MARGIN_TOP, (mi.rcMonitor.right-mi.rcMonitor.left)-16, BAR_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (b->hwndBar) {
        SetWindowLongPtr(b->hwndBar, GWLP_USERDATA, (LONG_PTR)b); SetLayeredWindowAttributes(b->hwndBar, 0, 255, LWA_ALPHA);
        SyncDwmStyle(b->hwndBar); SetTimer(b->hwndBar, TIMER_ID_MAIN, REFRESH_RATE, NULL); SetTimer(b->hwndBar, TIMER_ID_CLOCK, 1000, NULL); g_bars.push_back(b);
    }
    return TRUE;
}

BOOL Wh_ModInit() {
    CoInitialize(NULL); g_cache.Update(); D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_pDWriteFactory);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pWICFactory));
    WNDCLASSEXW wcB = {sizeof(wcB), CS_HREDRAW|CS_VREDRAW, BarWndProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"TopC", NULL}; RegisterClassExW(&wcB);
    WNDCLASSEXW wcT = {sizeof(wcT), 0, DefWindowProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"TopT", NULL}; RegisterClassExW(&wcT);
    EnumDisplayMonitors(NULL, NULL, MonitorEnum, 0); return TRUE;
}

void Wh_ModUninit() {
    for (auto b : g_bars) { DestroyWindow(b->hwndTrigger); DestroyWindow(b->hwndBar); b->Release(); delete b; }
    if (g_pD2DFactory) g_pD2DFactory->Release(); if (g_pDWriteFactory) g_pDWriteFactory->Release(); if (g_pWICFactory) g_pWICFactory->Release(); CoUninitialize();
}
