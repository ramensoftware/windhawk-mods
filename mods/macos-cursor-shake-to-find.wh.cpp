// ==WindhawkMod==
// @id              macos-cursor-shake-to-find
// @name            macOS Cursor & Shake to Find (4K HD)
// @description     Complete built-in 4K macOS cursor suite (no external files needed) with smooth "Shake to Find" magnification.
// @version         4.0.4
// @author          brcarvp
// @github          https://github.com/brcarvp
// @include         explorer.exe
// @include         chrome.exe
// @include         msedge.exe
// @include         brave.exe
// @compilerOptions -luser32 -lgdi32 -lgdiplus -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS Cursor & Shake to Find (4K HD) - Golden Gate Edition

Transform all your Windows mouse pointers into the authentic **macOS cursor suite** (true-to-scale macOS proportions with crisp white borders) and unlock the native macOS **"Shake to Find"** feature—**with zero external files or manual setup required!**

## Features

- **Authentic Built-In 4K macOS Cursors:** Procedurally renders all 13 system pointers directly in memory based on the latest macOS cursor design language.
- **Hardware Accelerated `.ani` Compilation:** Dynamically builds native Windows animated files for the Wait (Rainbow Beachball) and AppStarting (Arrow + Quadrant Blue Orb) cursors on launch for zero-flicker, 60fps buttery smooth system animations.
- **Shake to Find:** Rapidly shaking the mouse triggers a smooth, 60 FPS bicubic-interpolated magnification overlay.
- **Native Windows Cursors in Word:** Automatically restores 100% native Windows cursors when Microsoft Word or custom excluded apps are focused. **Shake to Find still works perfectly inside Word.**
- **Keep Cursor Visible While Typing:** Prevents Windows and browsers from hiding the cursor while you type.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- applyMacCursors: true
  $name: "Apply Built-in macOS Cursor Theme"
  $description: "Replaces default Windows pointers with built-in high-DPI macOS cursors"
- enableShakeToFind: true
  $name: "Enable Shake to Find"
  $description: "Temporarily enlarges the macOS cursor when shaking the mouse rapidly"
- maxScalePercent: 350
  $name: "Maximum Shake Scale (%)"
  $description: "Magnification percentage when shaking the mouse (350 = 3.5x normal size)"
- shakeSensitivity: 3
  $name: "Shake Sensitivity (Reversals)"
  $description: "Number of rapid direction changes required to trigger (2 = very easy, 4 = normal)"
- restoreDelayMs: 700
  $name: "Restore Delay (ms)"
  $description: "Time in milliseconds after stopping before the cursor shrinks back to normal"
- keepCursorVisibleWhileTyping: true
  $name: "Keep Cursor Visible While Typing"
  $description: "Prevents Windows and Chrome/Edge/Brave from hiding the cursor when typing on the keyboard"
- excludeOfficeAndExternalApps: true
  $name: "Use Default Windows Cursors in Word & External Apps"
  $description: "Restores original Windows cursors while Word or custom excluded apps are focused (Shake to Find remains active)"
- customExcludedApps: "photoshop.exe;illustrator.exe;acad.exe;blender.exe;code.exe"
  $name: "Additional Excluded Executables (separated by ;)"
  $description: "Extra .exe names where native Windows cursors should be restored when focused"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <cmath>
#include <atomic>
#include <cwctype>

#ifndef OCR_NORMAL
#define OCR_NORMAL      32512
#define OCR_IBEAM       32513
#define OCR_WAIT        32514
#define OCR_CROSS       32515
#define OCR_UP          32516
#define OCR_SIZENWSE    32642
#define OCR_SIZENESW    32643
#define OCR_SIZEWE      32644
#define OCR_SIZENS      32645
#define OCR_SIZEALL     32646
#define OCR_NO          32648
#define OCR_HAND        32649
#define OCR_APPSTARTING 32650
#define OCR_HELP        32651
#endif

struct ModSettings {
    bool applyMacCursors;
    bool enableShakeToFind;
    float maxScale;
    int shakeSensitivity;
    int restoreDelayMs;
    bool keepCursorVisibleWhileTyping;
    bool excludeOfficeAndExternalApps;
    std::vector<std::wstring> customExcludedList;
} g_settings;

const DWORD g_allCursorIds[] = {
    OCR_NORMAL, OCR_IBEAM, OCR_WAIT, OCR_CROSS, OCR_UP,
    OCR_SIZENWSE, OCR_SIZENESW, OCR_SIZEWE, OCR_SIZENS, OCR_SIZEALL,
    OCR_NO, OCR_HAND, OCR_APPSTARTING, OCR_HELP
};

const wchar_t* g_builtInExcludedApps[] = {
    L"winword.exe"
};

constexpr float MAC_TRUE_SCALE = 0.82f;

bool g_isExplorer = false;
HANDLE g_thread = NULL;
std::atomic<bool> g_running{false};
HWND g_hwndOverlay = NULL;
Gdiplus::Bitmap* g_pHighResArrowBmp = nullptr;
bool g_sysCursorHidden = false;
bool g_macCursorsCurrentlyApplied = false;
BOOL g_origMouseVanish = TRUE;
bool g_savedMouseVanish = false;

// ============================================================================
// HELPERS DO COMPILADOR DE .ANI (ARQUIVOS DE ANIMAÇÃO NATIVOS)
// ============================================================================

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

void WriteDWORD(std::vector<BYTE>& buf, DWORD v) {
    buf.push_back(v & 0xFF); buf.push_back((v >> 8) & 0xFF);
    buf.push_back((v >> 16) & 0xFF); buf.push_back((v >> 24) & 0xFF);
}

void WriteFourCC(std::vector<BYTE>& buf, const char* fcc) {
    buf.push_back(fcc[0]); buf.push_back(fcc[1]);
    buf.push_back(fcc[2]); buf.push_back(fcc[3]);
}

std::vector<BYTE> BuildAniFile(const std::vector<std::vector<BYTE>>& frames, DWORD jifRate) {
    std::vector<BYTE> buf;
    WriteFourCC(buf, "RIFF");
    WriteDWORD(buf, 0); // Tamanho placeholder
    WriteFourCC(buf, "ACON");
    
    WriteFourCC(buf, "anih");
    WriteDWORD(buf, 36); 
    
    DWORD cFrames = frames.size();
    WriteDWORD(buf, 36);
    WriteDWORD(buf, cFrames);
    WriteDWORD(buf, cFrames);
    WriteDWORD(buf, 0); 
    WriteDWORD(buf, 0); 
    WriteDWORD(buf, 0); 
    WriteDWORD(buf, 0); 
    WriteDWORD(buf, jifRate); // JifRate (velocidade do frame)
    WriteDWORD(buf, 1);       // Flags (AF_ICON)
    
    WriteFourCC(buf, "LIST");
    DWORD listSize = 4; // "fram"
    for (const auto& f : frames) {
        listSize += 8 + f.size();
        if (f.size() % 2 != 0) listSize += 1;
    }
    WriteDWORD(buf, listSize);
    WriteFourCC(buf, "fram");
    
    for (const auto& f : frames) {
        WriteFourCC(buf, "icon");
        WriteDWORD(buf, f.size());
        buf.insert(buf.end(), f.begin(), f.end());
        if (f.size() % 2 != 0) buf.push_back(0); // Pad alignment
    }
    
    DWORD fileSize = buf.size() - 8;
    buf[4] = fileSize & 0xFF; buf[5] = (fileSize >> 8) & 0xFF;
    buf[6] = (fileSize >> 16) & 0xFF; buf[7] = (fileSize >> 24) & 0xFF;
    
    return buf;
}

std::wstring GetTempAniPath(const std::wstring& name) {
    WCHAR tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    return std::wstring(tempPath) + name;
}

// ============================================================================
// DETECÇÃO DE JANELA E HOOKS DO SISTEMA
// ============================================================================

std::wstring ToLowerStr(const std::wstring& str) {
    std::wstring res = str;
    for (auto& ch : res) ch = static_cast<wchar_t>(towlower(ch));
    return res;
}

bool IsProcessExcluded(HWND hwnd) {
    if (!hwnd) return false;
    HWND hRoot = GetAncestor(hwnd, GA_ROOT);
    if (!hRoot) hRoot = hwnd;

    DWORD pid = 0;
    GetWindowThreadProcessId(hRoot, &pid);
    if (pid == 0) return false;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;

    WCHAR fullPath[MAX_PATH] = { 0 };
    DWORD size = MAX_PATH;
    bool matched = false;

    if (QueryFullProcessImageNameW(hProc, 0, fullPath, &size)) {
        const WCHAR* fileName = wcsrchr(fullPath, L'\\');
        fileName = fileName ? (fileName + 1) : fullPath;
        std::wstring exeLower = ToLowerStr(fileName);

        for (const wchar_t* builtIn : g_builtInExcludedApps) {
            if (exeLower == builtIn) { matched = true; break; }
        }
        if (!matched) {
            for (const auto& customExe : g_settings.customExcludedList) {
                if (!customExe.empty() && exeLower == customExe) { matched = true; break; }
            }
        }
    }
    CloseHandle(hProc);
    return matched;
}

bool IsForegroundAppExcluded() {
    if (!g_settings.excludeOfficeAndExternalApps) return false;
    HWND hFg = GetForegroundWindow();
    return IsProcessExcluded(hFg);
}

using SetCursor_t = decltype(&SetCursor);
SetCursor_t SetCursor_Orig = nullptr;

HCURSOR WINAPI SetCursor_Hook(HCURSOR hCursor) {
    if (hCursor == NULL && g_settings.keepCursorVisibleWhileTyping) {
        HWND hFg = GetForegroundWindow();
        if (hFg) {
            RECT rcWin{}, rcMon{};
            GetWindowRect(hFg, &rcWin);
            HMONITOR hMon = MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(MONITORINFO) };
            if (GetMonitorInfoW(hMon, &mi)) {
                rcMon = mi.rcMonitor;
                bool isFullscreen = (rcWin.left <= rcMon.left && rcWin.top <= rcMon.top &&
                                     rcWin.right >= rcMon.right && rcWin.bottom >= rcMon.bottom);
                if (!isFullscreen) {
                    HCURSOR hCurrent = GetCursor();
                    return hCurrent ? hCurrent : LoadCursorW(NULL, IDC_IBEAM);
                }
            }
        }
    }
    return SetCursor_Orig(hCursor);
}

// ============================================================================
// DESENHOS VETORIAIS (GOLDEN GATE & MAC AUTHENTIC)
// ============================================================================

void SetupHighQualityGraphics(Gdiplus::Graphics& g) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));
}

Gdiplus::Bitmap* RenderMacOSArrow(int size, float scaleFactor = MAC_TRUE_SCALE, float offsetX = 0.0f, float offsetY = 0.0f) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp);
    SetupHighQualityGraphics(g);

    float s = (static_cast<float>(size) / 32.0f) * scaleFactor;

    Gdiplus::PointF pts[7] = {
        { offsetX +  6.2f * s, offsetY +  4.8f * s },
        { offsetX +  6.2f * s, offsetY + 24.8f * s },
        { offsetX + 10.6f * s, offsetY + 20.4f * s },
        { offsetX + 14.0f * s, offsetY + 28.2f * s },
        { offsetX + 16.7f * s, offsetY + 27.0f * s },
        { offsetX + 13.3f * s, offsetY + 19.2f * s },
        { offsetX + 19.6f * s, offsetY + 19.2f * s }
    };

    Gdiplus::Matrix shadowMat;
    shadowMat.Translate(0.7f * s, 1.5f * s);
    g.SetTransform(&shadowMat);
    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(65, 0, 0, 0));
    Gdiplus::Pen shadowPen(Gdiplus::Color(50, 0, 0, 0), 3.1f * s);
    shadowPen.SetLineJoin(Gdiplus::LineJoinMiter);
    shadowPen.SetMiterLimit(10.0f);
    g.FillPolygon(&shadowBrush, pts, 7);
    g.DrawPolygon(&shadowPen, pts, 7);
    g.ResetTransform();

    Gdiplus::Pen whiteBorder(Gdiplus::Color(255, 255, 255, 255), 2.85f * s);
    whiteBorder.SetLineJoin(Gdiplus::LineJoinMiter);
    whiteBorder.SetMiterLimit(10.0f);
    g.DrawPolygon(&whiteBorder, pts, 7);

    Gdiplus::SolidBrush blackFill(Gdiplus::Color(255, 16, 18, 22));
    g.FillPolygon(&blackFill, pts, 7);

    return bmp;
}

// Seta Alternativa (Alternate / Up Arrow)
Gdiplus::Bitmap* RenderMacOSAlternate(int size) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp);
    SetupHighQualityGraphics(g);
    
    float trueS = static_cast<float>(size) / 32.0f;
    float s = trueS * MAC_TRUE_SCALE;
    float cx = 16.0f * trueS;

    Gdiplus::PointF pts[7] = { 
        { cx, 3.0f * s }, { cx + 7.5f * s, 18.0f * s }, { cx + 3.0f * s, 15.0f * s },
        { cx + 3.0f * s, 24.0f * s }, { cx - 3.0f * s, 24.0f * s }, { cx - 3.0f * s, 15.0f * s },
        { cx - 7.5f * s, 18.0f * s }
    };

    Gdiplus::Matrix shadowMat;
    shadowMat.Translate(0.0f, 1.5f * s);
    g.SetTransform(&shadowMat);
    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(65, 0, 0, 0));
    Gdiplus::Pen shadowPen(Gdiplus::Color(50, 0, 0, 0), 3.1f * s);
    shadowPen.SetLineJoin(Gdiplus::LineJoinMiter);
    g.FillPolygon(&shadowBrush, pts, 7);
    g.DrawPolygon(&shadowPen, pts, 7);
    g.ResetTransform();

    Gdiplus::Pen whiteBorder(Gdiplus::Color(255, 255, 255, 255), 2.8f * s);
    whiteBorder.SetLineJoin(Gdiplus::LineJoinMiter);
    Gdiplus::SolidBrush blackFill(Gdiplus::Color(255, 16, 18, 22));
    
    g.DrawPolygon(&whiteBorder, pts, 7);
    g.FillPolygon(&blackFill, pts, 7);

    return bmp;
}

// AppStarting - Animado (Seta + Orb Azul em Quadrantes)
Gdiplus::Bitmap* RenderMacOSAppStarting(int size, float angleOffset) {
    Gdiplus::Bitmap* bmp = RenderMacOSArrow(size, 0.64f, -0.8f * (size / 32.0f), -1.2f * (size / 32.0f));
    Gdiplus::Graphics g(bmp);
    
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

    float s = static_cast<float>(size) / 32.0f;
    float cx = 17.5f * s;
    float cy = 17.8f * s;
    float r = 6.8f * s;
    Gdiplus::RectF orbRect(cx - r, cy - r, r * 2.0f, r * 2.0f);

    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(65, 0, 0, 0));
    g.FillEllipse(&shadowBrush, cx - r + 0.5f * s, cy - r + 1.0f * s, r * 2.0f, r * 2.0f);

    Gdiplus::SolidBrush qLight(Gdiplus::Color(255, 60, 180, 255));
    Gdiplus::SolidBrush qDark(Gdiplus::Color(255, 30, 110, 230));

    g.FillPie(&qLight, orbRect, angleOffset + 0.0f, 90.5f);
    g.FillPie(&qDark,  orbRect, angleOffset + 90.0f, 90.5f);
    g.FillPie(&qLight, orbRect, angleOffset + 180.0f, 90.5f);
    g.FillPie(&qDark,  orbRect, angleOffset + 270.0f, 90.5f);

    Gdiplus::Pen rimPen(Gdiplus::Color(90, 255, 255, 255), 0.75f * s);
    g.DrawEllipse(&rimPen, orbRect);

    return bmp;
}

// Beachball - Animada Nativamente (Rainbow Swirl Colors)
Gdiplus::Bitmap* RenderMacOSBeachball(int size, float angleOffset) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp);
    SetupHighQualityGraphics(g);

    float s = static_cast<float>(size) / 32.0f;
    float cx = 16.0f * s;
    float cy = 16.0f * s;
    float r = 9.5f * s;
    Gdiplus::RectF rect(cx - r, cy - r, r * 2.0f, r * 2.0f);

    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(65, 0, 0, 0));
    g.FillEllipse(&shadowBrush, cx - r + 0.4f * s, cy - r + 0.9f * s, r * 2.0f, r * 2.0f);

    Gdiplus::Color colors[6] = {
        Gdiplus::Color(255, 235, 75, 80),   // Red
        Gdiplus::Color(255, 250, 150, 45),  // Orange
        Gdiplus::Color(255, 245, 215, 50),  // Yellow
        Gdiplus::Color(255, 100, 200, 80),  // Green
        Gdiplus::Color(255, 60, 160, 255),  // Blue
        Gdiplus::Color(255, 165, 100, 230)  // Purple
    };

    for (int i = 0; i < 6; ++i) {
        Gdiplus::SolidBrush brush(colors[i]);
        g.FillPie(&brush, rect, angleOffset + i * 60.0f, 60.5f);
    }

    Gdiplus::GraphicsPath glossPath;
    glossPath.AddEllipse(rect);
    Gdiplus::PathGradientBrush glossBrush(&glossPath);
    Gdiplus::Color centerColor(70, 255, 255, 255); 
    Gdiplus::Color surroundColor[1] = { Gdiplus::Color(0, 255, 255, 255) };
    int count = 1;
    glossBrush.SetCenterColor(centerColor);
    glossBrush.SetSurroundColors(surroundColor, &count);
    g.FillEllipse(&glossBrush, rect);

    Gdiplus::Pen borderPen(Gdiplus::Color(110, 255, 255, 255), 0.8f * s);
    g.DrawEllipse(&borderPen, rect);

    return bmp;
}

Gdiplus::Bitmap* RenderMacOSHand(int size) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp);
    SetupHighQualityGraphics(g);
    float s = (static_cast<float>(size) / 32.0f) * MAC_TRUE_SCALE;
    Gdiplus::GraphicsPath handPath;
    handPath.AddBezier(9.2f * s, 4.8f * s, 8.9f * s, 3.0f * s, 11.7f * s, 2.6f * s, 12.1f * s, 4.4f * s);
    handPath.AddLine(12.1f * s, 4.4f * s, 13.7f * s, 12.2f * s);
    handPath.AddBezier(13.7f * s, 12.2f * s, 13.9f * s, 10.4f * s, 16.9f * s, 10.4f * s, 17.0f * s, 12.3f * s);
    handPath.AddBezier(17.0f * s, 12.3f * s, 17.2f * s, 10.8f * s, 20.1f * s, 10.9f * s, 20.2f * s, 12.7f * s);
    handPath.AddBezier(20.2f * s, 12.7f * s, 20.4f * s, 11.5f * s, 23.0f * s, 11.8f * s, 23.0f * s, 13.6f * s);
    handPath.AddBezier(23.0f * s, 13.6f * s, 23.1f * s, 17.5f * s, 22.9f * s, 21.8f * s, 22.0f * s, 25.4f * s);
    handPath.AddBezier(22.0f * s, 25.4f * s, 21.6f * s, 27.6f * s, 18.2f * s, 27.6f * s, 17.4f * s, 25.5f * s);
    handPath.AddBezier(17.4f * s, 25.5f * s, 16.6f * s, 27.6f * s, 13.0f * s, 27.6f * s, 12.2f * s, 25.6f * s);
    handPath.AddBezier(12.2f * s, 25.6f * s, 10.4f * s, 23.6f * s, 8.2f * s, 20.6f * s, 5.6f * s, 17.6f * s);
    handPath.AddBezier(5.6f * s, 17.6f * s, 4.3f * s, 16.0f * s, 6.2f * s, 14.0f * s, 7.8f * s, 15.4f * s);
    handPath.AddLine(7.8f * s, 15.4f * s, 10.6f * s, 18.0f * s);
    handPath.AddLine(10.6f * s, 18.0f * s, 9.2f * s, 4.8f * s);
    handPath.CloseFigure();
    Gdiplus::Matrix shadowMat; shadowMat.Translate(0.6f * s, 1.3f * s); g.SetTransform(&shadowMat);
    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(65, 0, 0, 0)); g.FillPath(&shadowBrush, &handPath); g.ResetTransform();
    Gdiplus::SolidBrush whiteFill(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::Pen blackOutline(Gdiplus::Color(255, 22, 23, 26), 2.15f * s); blackOutline.SetLineJoin(Gdiplus::LineJoinRound);
    g.FillPath(&whiteFill, &handPath); g.DrawPath(&blackOutline, &handPath);
    Gdiplus::Pen backLinePen(Gdiplus::Color(255, 22, 23, 26), 1.45f * s);
    backLinePen.SetStartCap(Gdiplus::LineCapRound); backLinePen.SetEndCap(Gdiplus::LineCapRound);
    g.DrawLine(&backLinePen, 14.2f * s, 17.5f * s, 14.2f * s, 22.3f * s);
    g.DrawLine(&backLinePen, 16.9f * s, 17.5f * s, 16.9f * s, 22.3f * s);
    g.DrawLine(&backLinePen, 19.6f * s, 17.5f * s, 19.6f * s, 22.3f * s);
    return bmp;
}

// Texto (I-Beam) - Atualizado Golden Gate (Sem barra central e curvas suaves)
Gdiplus::Bitmap* RenderMacOSIBeam(int size) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp); 
    SetupHighQualityGraphics(g);
    
    float s = static_cast<float>(size) / 32.0f; 
    float cx = 16.0f * s; 
    float cy = 16.0f * s;
    
    Gdiplus::GraphicsPath path; 
    path.AddLine(cx, cy - 7.5f * s, cx, cy + 7.5f * s);
    
    Gdiplus::GraphicsPath topArc;
    topArc.AddBezier(cx - 4.0f * s, cy - 9.5f * s, cx - 1.5f * s, cy - 9.5f * s, cx, cy - 8.5f * s, cx, cy - 7.5f * s);
    topArc.AddBezier(cx, cy - 7.5f * s, cx, cy - 8.5f * s, cx + 1.5f * s, cy - 9.5f * s, cx + 4.0f * s, cy - 9.5f * s);
    
    Gdiplus::GraphicsPath botArc;
    botArc.AddBezier(cx - 4.0f * s, cy + 9.5f * s, cx - 1.5f * s, cy + 9.5f * s, cx, cy + 8.5f * s, cx, cy + 7.5f * s);
    botArc.AddBezier(cx, cy + 7.5f * s, cx, cy + 8.5f * s, cx + 1.5f * s, cy + 9.5f * s, cx + 4.0f * s, cy + 9.5f * s);
    
    Gdiplus::Pen whitePen(Gdiplus::Color(255, 255, 255, 255), 3.4f * s);
    whitePen.SetStartCap(Gdiplus::LineCapRound); whitePen.SetEndCap(Gdiplus::LineCapRound); whitePen.SetLineJoin(Gdiplus::LineJoinRound);
    
    Gdiplus::Pen blackPen(Gdiplus::Color(255, 16, 18, 22), 1.5f * s);
    blackPen.SetStartCap(Gdiplus::LineCapRound); blackPen.SetEndCap(Gdiplus::LineCapRound); blackPen.SetLineJoin(Gdiplus::LineJoinRound);
    
    g.DrawPath(&whitePen, &path); g.DrawPath(&whitePen, &topArc); g.DrawPath(&whitePen, &botArc);
    g.DrawPath(&blackPen, &path); g.DrawPath(&blackPen, &topArc); g.DrawPath(&blackPen, &botArc);
    
    return bmp;
}

Gdiplus::Bitmap* RenderMacOSCross(int size) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp); SetupHighQualityGraphics(g);
    float s = static_cast<float>(size) / 32.0f; float cx = 16.0f * s; float cy = 16.0f * s; float arm = 8.4f * s;
    Gdiplus::Pen whitePen(Gdiplus::Color(255, 255, 255, 255), 3.6f * s);
    Gdiplus::Pen blackPen(Gdiplus::Color(255, 16, 18, 22), 1.45f * s);
    g.DrawLine(&whitePen, cx - arm, cy, cx + arm, cy); g.DrawLine(&whitePen, cx, cy - arm, cx, cy + arm);
    g.DrawLine(&blackPen, cx - arm, cy, cx + arm, cy); g.DrawLine(&blackPen, cx, cy - arm, cx, cy + arm);
    return bmp;
}

Gdiplus::Bitmap* RenderMacOSMove(int size) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp); SetupHighQualityGraphics(g);
    float s = static_cast<float>(size) / 32.0f; float cx = 16.0f * s; float cy = 16.0f * s; float d = 9.5f * s;
    Gdiplus::PointF diamond[4] = { { cx, cy - d }, { cx + d, cy }, { cx, cy + d }, { cx - d, cy } };
    Gdiplus::Pen whiteBorder(Gdiplus::Color(255, 255, 255, 255), 2.3f * s); whiteBorder.SetLineJoin(Gdiplus::LineJoinMiter);
    Gdiplus::SolidBrush blackFill(Gdiplus::Color(255, 16, 18, 22));
    g.DrawPolygon(&whiteBorder, diamond, 4); g.FillPolygon(&blackFill, diamond, 4);
    Gdiplus::Pen innerWhite(Gdiplus::Color(255, 255, 255, 255), 1.4f * s);
    innerWhite.SetStartCap(Gdiplus::LineCapRound); innerWhite.SetEndCap(Gdiplus::LineCapRound);
    float l = 4.5f * s; g.DrawLine(&innerWhite, cx - l, cy, cx + l, cy); g.DrawLine(&innerWhite, cx, cy - l, cx, cy + l);
    Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::PointF topTip[3]   = { {cx, cy - 6.5f * s}, {cx - 2.0f * s, cy - 3.5f * s}, {cx + 2.0f * s, cy - 3.5f * s} };
    Gdiplus::PointF botTip[3]   = { {cx, cy + 6.5f * s}, {cx - 2.0f * s, cy + 3.5f * s}, {cx + 2.0f * s, cy + 3.5f * s} };
    Gdiplus::PointF leftTip[3]  = { {cx - 6.5f * s, cy}, {cx - 3.5f * s, cy - 2.0f * s}, {cx - 3.5f * s, cy + 2.0f * s} };
    Gdiplus::PointF rightTip[3] = { {cx + 6.5f * s, cy}, {cx + 3.5f * s, cy - 2.0f * s}, {cx + 3.5f * s, cy + 2.0f * s} };
    g.FillPolygon(&whiteBrush, topTip, 3); g.FillPolygon(&whiteBrush, botTip, 3);
    g.FillPolygon(&whiteBrush, leftTip, 3); g.FillPolygon(&whiteBrush, rightTip, 3);
    return bmp;
}

Gdiplus::Bitmap* RenderMacOSGoldenGateResize(int size, float angleDegrees) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp); SetupHighQualityGraphics(g);
    float s = static_cast<float>(size) / 32.0f; float cx = 16.0f * s; float cy = 16.0f * s;
    g.TranslateTransform(cx, cy); g.RotateTransform(angleDegrees); g.TranslateTransform(-cx, -cy);
    Gdiplus::PointF topTriangle[3] = { {cx, cy - 9.0f * s}, {cx - 5.5f * s, cy - 2.0f * s}, {cx + 5.5f * s, cy - 2.0f * s} };
    Gdiplus::PointF botTriangle[3] = { {cx, cy + 9.0f * s}, {cx - 5.5f * s, cy + 2.0f * s}, {cx + 5.5f * s, cy + 2.0f * s} };
    Gdiplus::Pen whiteBorder(Gdiplus::Color(255, 255, 255, 255), 2.5f * s); whiteBorder.SetLineJoin(Gdiplus::LineJoinMiter);
    Gdiplus::SolidBrush blackFill(Gdiplus::Color(255, 16, 18, 22));
    g.DrawPolygon(&whiteBorder, topTriangle, 3); g.FillPolygon(&blackFill, topTriangle, 3);
    g.DrawPolygon(&whiteBorder, botTriangle, 3); g.FillPolygon(&blackFill, botTriangle, 3);
    return bmp;
}

Gdiplus::Bitmap* RenderMacOSHelp(int size) {
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(size, size, PixelFormat32bppARGB);
    Gdiplus::Graphics g(bmp); SetupHighQualityGraphics(g);
    float s = static_cast<float>(size) / 32.0f; float cx = 16.0f * s; float cy = 16.0f * s;
    Gdiplus::GraphicsPath qPath;
    qPath.AddArc(cx - 4.5f * s, cy - 9.0f * s, 9.0f * s, 9.0f * s, 170.0f, 220.0f);
    qPath.AddBezier(cx + 4.5f * s, cy - 4.5f * s, cx + 4.5f * s, cy - 1.0f * s, cx, cy - 1.0f * s, cx, cy + 2.5f * s);
    Gdiplus::Pen whitePen(Gdiplus::Color(255, 255, 255, 255), 4.8f * s); whitePen.SetStartCap(Gdiplus::LineCapRound); whitePen.SetEndCap(Gdiplus::LineCapRound);
    Gdiplus::Pen blackPen(Gdiplus::Color(255, 16, 18, 22), 2.2f * s); blackPen.SetStartCap(Gdiplus::LineCapRound); blackPen.SetEndCap(Gdiplus::LineCapRound);
    g.DrawPath(&whitePen, &qPath); g.DrawPath(&blackPen, &qPath);
    Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255)); Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 16, 18, 22));
    g.FillEllipse(&whiteBrush, cx - 2.5f * s, cy + 5.5f * s, 5.0f * s, 5.0f * s);
    g.FillEllipse(&blackBrush, cx - 1.2f * s, cy + 6.8f * s, 2.4f * s, 2.4f * s);
    return bmp;
}

Gdiplus::Bitmap* RenderMacOSNo(int size) {
    Gdiplus::Bitmap* bmp = RenderMacOSArrow(size, 0.64f, -0.8f * (size / 32.0f), -1.2f * (size / 32.0f));
    Gdiplus::Graphics g(bmp); g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality); g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    float s = static_cast<float>(size) / 32.0f; float cx = 17.5f * s; float cy = 18.0f * s; float r = 6.0f * s;
    Gdiplus::Pen darkBorder(Gdiplus::Color(200, 16, 18, 22), 3.8f * s);
    g.DrawEllipse(&darkBorder, cx - r, cy - r, r * 2.0f, r * 2.0f); float offset = r * 0.707f; g.DrawLine(&darkBorder, cx - offset, cy - offset, cx + offset, cy + offset);
    Gdiplus::Pen whiteSign(Gdiplus::Color(255, 245, 245, 245), 1.9f * s);
    g.DrawEllipse(&whiteSign, cx - r, cy - r, r * 2.0f, r * 2.0f); g.DrawLine(&whiteSign, cx - offset, cy - offset, cx + offset, cy + offset);
    return bmp;
}

// ============================================================================
// CONSTRUÇÃO E APLICAÇÃO DOS CURSORES NO SISTEMA
// ============================================================================

HCURSOR BitmapToHCURSOR(Gdiplus::Bitmap* bmp, int hotspotX, int hotspotY) {
    if (!bmp) return NULL;
    HBITMAP hbmColor = NULL;
    bmp->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hbmColor);
    if (!hbmColor) return NULL;

    int w = bmp->GetWidth();
    int h = bmp->GetHeight();
    HBITMAP hbmMask = CreateBitmap(bmp->GetWidth(), bmp->GetHeight(), 1, 1, NULL);

    ICONINFO ii = { 0 };
    ii.fIcon = FALSE;
    ii.xHotspot = hotspotX;
    ii.yHotspot = hotspotY;
    ii.hbmMask = hbmMask;
    ii.hbmColor = hbmColor;

    HCURSOR hCursor = CreateIconIndirect(&ii);
    DeleteObject(hbmMask);
    DeleteObject(hbmColor);
    return hCursor;
}

void SetRenderedCursor(DWORD ocrId, Gdiplus::Bitmap* bmp, int hotX, int hotY) {
    if (!bmp) return;
    HCURSOR hCur = BitmapToHCURSOR(bmp, hotX, hotY);
    delete bmp;
    if (hCur) SetSystemCursor(hCur, ocrId);
}

// Compila dinamicamente os arquivos .ani e os salva no diretório %TEMP%
void CompileAnimatedCursors(int sz) {
    std::vector<std::vector<BYTE>> appStartFrames;
    std::vector<std::vector<BYTE>> waitFrames;
    CLSID pngClsid;
    if (GetEncoderClsid(L"image/png", &pngClsid) == -1) return;

    auto ProcessFrame = [&](Gdiplus::Bitmap* bmp, int hotX, int hotY, std::vector<std::vector<BYTE>>& outFrames) {
        IStream* stream = nullptr;
        CreateStreamOnHGlobal(NULL, TRUE, &stream);
        bmp->Save(stream, &pngClsid, NULL);

        STATSTG stat;
        stream->Stat(&stat, STATFLAG_NONAME);
        ULONG pngSize = stat.cbSize.LowPart;

        std::vector<BYTE> pngData(pngSize);
        LARGE_INTEGER liZero = {};
        stream->Seek(liZero, STREAM_SEEK_SET, NULL);
        ULONG bytesRead;
        stream->Read(pngData.data(), pngSize, &bytesRead);
        stream->Release();

        std::vector<BYTE> icoData;
        icoData.push_back(0); icoData.push_back(0);
        icoData.push_back(2); icoData.push_back(0); // CURSOR
        icoData.push_back(1); icoData.push_back(0); // 1 Image
        
        int w = bmp->GetWidth(); int h = bmp->GetHeight();
        icoData.push_back(w >= 256 ? 0 : w);
        icoData.push_back(h >= 256 ? 0 : h);
        icoData.push_back(0); icoData.push_back(0);
        
        icoData.push_back(hotX & 0xFF); icoData.push_back((hotX >> 8) & 0xFF);
        icoData.push_back(hotY & 0xFF); icoData.push_back((hotY >> 8) & 0xFF);
        
        icoData.push_back(pngSize & 0xFF); icoData.push_back((pngSize >> 8) & 0xFF);
        icoData.push_back((pngSize >> 16) & 0xFF); icoData.push_back((pngSize >> 24) & 0xFF);
        
        int offset = 22; // Header size
        icoData.push_back(offset & 0xFF); icoData.push_back((offset >> 8) & 0xFF);
        icoData.push_back((offset >> 16) & 0xFF); icoData.push_back((offset >> 24) & 0xFF);
        
        icoData.insert(icoData.end(), pngData.begin(), pngData.end());
        outFrames.push_back(icoData);
        delete bmp;
    };

    for (int i = 0; i < 18; ++i) {
        ProcessFrame(RenderMacOSAppStarting(sz, i * 20.0f), static_cast<int>(3.2f * (sz / 32.0f)), static_cast<int>(1.8f * (sz / 32.0f)), appStartFrames);
    }
    for (int i = 0; i < 18; ++i) {
        ProcessFrame(RenderMacOSBeachball(sz, i * 20.0f), sz / 2, sz / 2, waitFrames);
    }

    std::vector<BYTE> appStartAni = BuildAniFile(appStartFrames, 3);
    std::vector<BYTE> waitAni = BuildAniFile(waitFrames, 3);

    HANDLE hFile1 = CreateFileW(GetTempAniPath(L"mac_appstart.ani").c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile1 != INVALID_HANDLE_VALUE) {
        DWORD written; WriteFile(hFile1, appStartAni.data(), appStartAni.size(), &written, NULL); CloseHandle(hFile1);
    }
    
    HANDLE hFile2 = CreateFileW(GetTempAniPath(L"mac_wait.ani").c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile2 != INVALID_HANDLE_VALUE) {
        DWORD written; WriteFile(hFile2, waitAni.data(), waitAni.size(), &written, NULL); CloseHandle(hFile2);
    }
}

void RestoreNativeWindowsCursors() {
    if (!g_isExplorer) return;
    SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0);
    g_sysCursorHidden = false;
    g_macCursorsCurrentlyApplied = false;
}

void ApplyBuiltInMacOSCursors() {
    if (!g_isExplorer || !g_settings.applyMacCursors) return;

    int sz = GetSystemMetrics(SM_CXCURSOR);
    if (sz <= 0) sz = 32;
    float r = (static_cast<float>(sz) / 32.0f) * MAC_TRUE_SCALE;
    int center = sz / 2;

    if (!g_pHighResArrowBmp) g_pHighResArrowBmp = RenderMacOSArrow(256, MAC_TRUE_SCALE);

    SetRenderedCursor(OCR_NORMAL,      RenderMacOSArrow(sz),                    static_cast<int>(6.2f * r), static_cast<int>(3.5f * r));
    SetRenderedCursor(OCR_UP,          RenderMacOSAlternate(sz),                static_cast<int>(16.0f * (sz / 32.0f)), static_cast<int>(3.0f * r));
    SetRenderedCursor(OCR_HAND,        RenderMacOSHand(sz),                     static_cast<int>(10.0f * r), static_cast<int>(3.0f * r));
    
    // Atualização: Cursor de Texto I-Beam minimalista
    SetRenderedCursor(OCR_IBEAM,       RenderMacOSIBeam(sz),                    center,                   center);
    
    SetRenderedCursor(OCR_CROSS,       RenderMacOSCross(sz),                    center,                   center);
    SetRenderedCursor(OCR_SIZEALL,     RenderMacOSMove(sz),                     center,                   center);
    SetRenderedCursor(OCR_SIZENS,      RenderMacOSGoldenGateResize(sz, 0.0f),   center,                   center);
    SetRenderedCursor(OCR_SIZEWE,      RenderMacOSGoldenGateResize(sz, 90.0f),  center,                   center);
    SetRenderedCursor(OCR_SIZENWSE,    RenderMacOSGoldenGateResize(sz, -45.0f), center,                   center);
    SetRenderedCursor(OCR_SIZENESW,    RenderMacOSGoldenGateResize(sz, 45.0f),  center,                   center);
    SetRenderedCursor(OCR_HELP,        RenderMacOSHelp(sz),                     center,                   center);
    SetRenderedCursor(OCR_NO,          RenderMacOSNo(sz),                       static_cast<int>(3.2f * (sz / 32.0f)), static_cast<int>(1.8f * (sz / 32.0f)));

    HCURSOR hAppStart = (HCURSOR)LoadImageW(NULL, GetTempAniPath(L"mac_appstart.ani").c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
    if (hAppStart) SetSystemCursor(hAppStart, OCR_APPSTARTING);
    
    HCURSOR hWait = (HCURSOR)LoadImageW(NULL, GetTempAniPath(L"mac_wait.ani").c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
    if (hWait) SetSystemCursor(hWait, OCR_WAIT);

    g_sysCursorHidden = false;
    g_macCursorsCurrentlyApplied = true;
}

// ============================================================================
// LÓGICA DO SHAKE TO FIND
// ============================================================================

HCURSOR CreateBlankCursor() {
    int w = GetSystemMetrics(SM_CXCURSOR); int h = GetSystemMetrics(SM_CYCURSOR);
    if (w <= 0) w = 32; if (h <= 0) h = 32;
    size_t sz = (static_cast<size_t>(w + 7) / 8) * h;
    std::vector<BYTE> andM(sz, 0xFF), xorM(sz, 0x00);
    return CreateCursor(GetModuleHandleW(NULL), 0, 0, w, h, andM.data(), xorM.data());
}

void HideSystemCursorsForShake() {
    if (g_sysCursorHidden) return;
    HCURSOR hBlank = CreateBlankCursor();
    if (!hBlank) return;
    for (DWORD id : g_allCursorIds) {
        HCURSOR hCopy = CopyIcon(hBlank);
        if (hCopy && !SetSystemCursor(hCopy, id)) DestroyIcon(hCopy);
    }
    DestroyCursor(hBlank);
    g_sysCursorHidden = true;
}

void DrawScaledOverlay4K(HWND hwnd, POINT pt, float scale) {
    if (!g_pHighResArrowBmp) return;

    int baseDisplaySize = GetSystemMetrics(SM_CXCURSOR);
    if (baseDisplaySize <= 0) baseDisplaySize = 32;
    int targetW = static_cast<int>(baseDisplaySize * scale);
    int targetH = static_cast<int>(baseDisplaySize * scale);

    int winX = pt.x - static_cast<int>(targetW * (6.2f / 32.0f) * MAC_TRUE_SCALE);
    int winY = pt.y - static_cast<int>(targetH * (3.5f / 32.0f) * MAC_TRUE_SCALE);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi = { { sizeof(BITMAPINFOHEADER), targetW, -targetH, 1, 32, BI_RGB } };
    void* pBits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    if (hbm && pBits) {
        HBITMAP oldBm = (HBITMAP)SelectObject(hdcMem, hbm);

        {
            Gdiplus::Graphics graphics(hdcMem);
            graphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
            graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            graphics.DrawImage(g_pHighResArrowBmp, 0, 0, targetW, targetH);
        }

        DWORD* px = static_cast<DWORD*>(pBits);
        size_t count = static_cast<size_t>(targetW) * targetH;
        for (size_t i = 0; i < count; ++i) {
            DWORD c = px[i];
            BYTE a = (c >> 24) & 0xFF;
            if (a < 255) {
                BYTE r = (((c >> 16) & 0xFF) * a) / 255;
                BYTE g = (((c >> 8) & 0xFF) * a) / 255;
                BYTE b = ((c & 0xFF) * a) / 255;
                px[i] = (static_cast<DWORD>(a) << 24) | (static_cast<DWORD>(r) << 16) | (static_cast<DWORD>(g) << 8) | b;
            }
        }

        POINT ptZero = { 0, 0 }, ptWin = { winX, winY };
        SIZE sz = { targetW, targetH };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        UpdateLayeredWindow(hwnd, hdcScreen, &ptWin, &sz, hdcMem, &ptZero, 0, &blend, ULW_ALPHA);
        SelectObject(hdcMem, oldBm);
        DeleteObject(hbm);
    }
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

typedef BOOL (WINAPI *pfn_SetThreadDpiAwarenessContext)(HANDLE);

DWORD WINAPI ShakeMonitorThread(LPVOID) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto pSetDpi = (pfn_SetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
        if (pSetDpi) pSetDpi((HANDLE)-4);
    }

    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), 0, DefWindowProcW, 0, 0, GetModuleHandleW(NULL), NULL, NULL, NULL, NULL, L"WhMacSuiteShake4K", NULL };
    RegisterClassExW(&wc);
    g_hwndOverlay = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                    wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, NULL, NULL, wc.hInstance, NULL);

    POINT lastPt{};
    GetCursorPos(&lastPt);
    int lastDir = 0, accumDx = 0, reversals = 0;
    ULONGLONG lastRevTime = 0, lastShakeTime = 0;
    float currentScale = 1.0f, targetScale = 1.0f;
    bool visible = false;

    while (g_running.load()) {
        POINT pt{};
        ULONGLONG now = GetTickCount64();
        bool inExcludedApp = IsForegroundAppExcluded();

        if (!g_sysCursorHidden && !visible) {
            if (inExcludedApp && g_macCursorsCurrentlyApplied) RestoreNativeWindowsCursors();
            else if (!inExcludedApp && g_settings.applyMacCursors && !g_macCursorsCurrentlyApplied) ApplyBuiltInMacOSCursors();
        }

        if (g_settings.enableShakeToFind && GetCursorPos(&pt)) {
            int dx = pt.x - lastPt.x;
            lastPt = pt;

            if (now - lastRevTime > 320 && reversals > 0) { reversals = 0; lastDir = 0; accumDx = 0; }

            if (dx != 0) {
                int dir = (dx > 0) ? 1 : -1;
                if (dir == lastDir || lastDir == 0) {
                    accumDx += std::abs(dx);
                    if (lastDir == 0) { lastDir = dir; lastRevTime = now; }
                } else {
                    if (accumDx >= 20) {
                        reversals++;
                        lastRevTime = now;
                        if (reversals >= g_settings.shakeSensitivity) { targetScale = g_settings.maxScale; lastShakeTime = now; }
                    }
                    accumDx = std::abs(dx);
                    lastDir = dir;
                }
            }

            if (targetScale > 1.0f && (now - lastShakeTime > static_cast<ULONGLONG>(g_settings.restoreDelayMs))) targetScale = 1.0f;

            float speed = (targetScale > currentScale) ? 0.35f : 0.18f;
            currentScale += (targetScale - currentScale) * speed;

            if (currentScale > 1.05f) {
                HideSystemCursorsForShake();
                DrawScaledOverlay4K(g_hwndOverlay, pt, currentScale);
                if (!visible) {
                    SetWindowPos(g_hwndOverlay, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                    visible = true;
                }
            } else {
                if (visible) { ShowWindow(g_hwndOverlay, SW_HIDE); visible = false; }
                if (g_sysCursorHidden) {
                    if (inExcludedApp || !g_settings.applyMacCursors) RestoreNativeWindowsCursors();
                    else ApplyBuiltInMacOSCursors();
                }
            }
        } else if (GetCursorPos(&pt)) {
            lastPt = pt;
        }
        Sleep(16);
    }
    if (g_hwndOverlay) { DestroyWindow(g_hwndOverlay); g_hwndOverlay = NULL; }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void ParseCustomExcludedApps(const WCHAR* rawStr) {
    g_settings.customExcludedList.clear();
    if (!rawStr) return;
    std::wstring input = rawStr;
    size_t start = 0;
    while (start < input.size()) {
        size_t end = input.find(L';', start);
        if (end == std::wstring::npos) end = input.size();
        std::wstring token = input.substr(start, end - start);
        size_t first = token.find_first_not_of(L" \t\r\n");
        size_t last = token.find_last_not_of(L" \t\r\n");
        if (first != std::wstring::npos && last != std::wstring::npos) {
            g_settings.customExcludedList.push_back(ToLowerStr(token.substr(first, last - first + 1)));
        }
        start = end + 1;
    }
}

void LoadSettings() {
    g_settings.applyMacCursors = Wh_GetIntSetting(L"applyMacCursors") != 0;
    g_settings.enableShakeToFind = Wh_GetIntSetting(L"enableShakeToFind") != 0;
    int pct = Wh_GetIntSetting(L"maxScalePercent"); g_settings.maxScale = (pct >= 150 && pct <= 800) ? (pct / 100.0f) : 3.5f;
    int sens = Wh_GetIntSetting(L"shakeSensitivity"); g_settings.shakeSensitivity = (sens >= 2 && sens <= 10) ? sens : 3;
    int delay = Wh_GetIntSetting(L"restoreDelayMs"); g_settings.restoreDelayMs = (delay >= 200) ? delay : 700;
    g_settings.keepCursorVisibleWhileTyping = Wh_GetIntSetting(L"keepCursorVisibleWhileTyping") != 0;
    g_settings.excludeOfficeAndExternalApps = Wh_GetIntSetting(L"excludeOfficeAndExternalApps") != 0;
    PCWSTR customApps = Wh_GetStringSetting(L"customExcludedApps");
    ParseCustomExcludedApps(customApps);
    Wh_FreeStringSetting(customApps);
}

void UpdateMouseVanishSetting() {
    if (!g_isExplorer) return;
    if (g_settings.keepCursorVisibleWhileTyping) {
        if (!g_savedMouseVanish) { SystemParametersInfoW(SPI_GETMOUSEVANISH, 0, &g_origMouseVanish, 0); g_savedMouseVanish = true; }
        SystemParametersInfoW(SPI_SETMOUSEVANISH, 0, (PVOID)FALSE, 0);
    } else if (g_savedMouseVanish) {
        SystemParametersInfoW(SPI_SETMOUSEVANISH, 0, (PVOID)(INT_PTR)g_origMouseVanish, 0); g_savedMouseVanish = false;
    }
}

bool IsCurrentProcessExplorer() {
    WCHAR path[MAX_PATH] = { 0 };
    if (!GetModuleFileNameW(NULL, path, MAX_PATH)) return false;
    const WCHAR* fileName = wcsrchr(path, L'\\');
    fileName = fileName ? (fileName + 1) : path;
    return _wcsicmp(fileName, L"explorer.exe") == 0;
}

ULONG_PTR g_gdiplusToken = 0;

BOOL Wh_ModInit() {
    LoadSettings();
    g_isExplorer = IsCurrentProcessExplorer();

    if (!g_isExplorer) {
        Wh_SetFunctionHook((void*)SetCursor, (void*)SetCursor_Hook, (void**)&SetCursor_Orig);
        return TRUE;
    }

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    UpdateMouseVanishSetting();
    
    int sz = GetSystemMetrics(SM_CXCURSOR);
    CompileAnimatedCursors(sz > 0 ? sz : 32);

    if (!IsForegroundAppExcluded() && g_settings.applyMacCursors) ApplyBuiltInMacOSCursors();
    else RestoreNativeWindowsCursors();

    g_running = true;
    g_thread = CreateThread(NULL, 0, ShakeMonitorThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_isExplorer) return;
    g_running = false;
    if (g_thread) { WaitForSingleObject(g_thread, 2000); CloseHandle(g_thread); g_thread = NULL; }
    if (g_pHighResArrowBmp) { delete g_pHighResArrowBmp; g_pHighResArrowBmp = nullptr; }
    if (g_gdiplusToken) { Gdiplus::GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }
    if (g_savedMouseVanish) { SystemParametersInfoW(SPI_SETMOUSEVANISH, 0, (PVOID)(INT_PTR)g_origMouseVanish, 0); g_savedMouseVanish = false; }
    RestoreNativeWindowsCursors();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (!g_isExplorer) return;
    UpdateMouseVanishSetting();
    if (g_settings.applyMacCursors && !IsForegroundAppExcluded()) ApplyBuiltInMacOSCursors();
    else RestoreNativeWindowsCursors();
}
