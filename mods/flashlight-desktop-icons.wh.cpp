// ==WindhawkMod==
// @id              flashlight-desktop-icons
// @name            Flashlight Desktop Icons
// @description     Make desktop icons semi-transparent or invisible when idle, and reveal ONLY the icons near the mouse cursor within a customizable radius. Works flawlessly with Wallpaper Engine.
// @version         5.0.0
// @author          dantepicachu
// @github          https://github.com/dantepicachu11-coder
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luser32 -lmsimg32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Transparent Desktop Icons (v5 - Ultra Optimized)

Makes desktop icons semi-transparent (or completely invisible) when idle.
When your mouse cursor approaches an individual desktop icon, ONLY icons within your configured pixel radius smoothly reveal themselves, leaving the rest of the desktop idle.

* Fully compatible with Wallpaper Engine and Live Wallpapers.
* Zero CPU overhead when the mouse is idle.
* No overlay windows (does not block clicks or cause black screens).
* Double-clicking on an empty area toggles icon visibility on and off.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- idleOpacity: 0
  $name: Idle Opacity (0-255)
  $description: Opacity of icons when idle/far away from cursor (0 = invisible, 255 = fully opaque)
- activeOpacity: 255
  $name: Active Opacity (0-255)
  $description: Opacity of icons directly under or near the mouse cursor (255 = fully visible)
- spotlightRadius: 200
  $name: Mouse Spotlight Radius (pixels)
  $description: Radial distance in pixels from cursor where icons begin to fade into active opacity.
- idleDelay: 500
  $name: Mouse Idle Timeout (ms)
  $description: Delay in milliseconds before icons fade back to idle opacity after mouse stops moving
- spotlightFadeOutDuration: 800
  $name: Fade Out Duration (ms)
  $description: How long it takes for icons to fade out after the idle delay
- toggleOnDoubleClick: true
  $name: Toggle Icons on Double-Click
  $description: Double-click on empty desktop background to hide or show icons completely.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <Uxtheme.h>
#include <vssym32.h>
#include <algorithm>
#include <atomic>
#include <thread>
#include <cmath>
#include <windhawk_api.h>

////////////////////////////////////////////////////////////////////////////////// Settings

struct Settings {
    int idleOpacity;
    int activeOpacity;
    int spotlightRadius;
    int idleDelay;
    int spotlightFadeOutDuration;
    bool toggleOnDoubleClick;
} g_settings;

////////////////////////////////////////////////////////////////////////////////// Global State

HWND g_listViewWnd = nullptr;
HWND g_shellViewWnd = nullptr;
HTHEME g_hTheme = nullptr; // Cached theme handle for maximum performance

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_subclassed{false};

std::thread* g_initThread = nullptr;
std::thread* g_repaintThread = nullptr;
HHOOK g_hHook = nullptr;

// Thread-safe mouse tracking
std::atomic<int> g_mouseX{-9999};
std::atomic<int> g_mouseY{-9999};
std::atomic<ULONGLONG> g_lastMouseTime{0};

std::atomic<bool> g_overlayBypassed{false};

#define SUBCLASS_ID_SV 2
#define SUBCLASS_ID_LV 3

////////////////////////////////////////////////////////////////////////////////// Helper Math

float GetDistanceToRect(POINT pt, const RECT& rc) {
    int dx = (std::max)({ (int)(rc.left - pt.x), 0, (int)(pt.x - rc.right) });
    int dy = (std::max)({ (int)(rc.top - pt.y), 0, (int)(pt.y - rc.bottom) });
    return std::sqrt((float)(dx * dx + dy * dy));
}

BYTE CalculateIconAlpha(int iItem) {
    if (g_overlayBypassed.load(std::memory_order_relaxed)) {
        return 0; 
    }

    // Use cached mouse coordinates to prevent thousands of API calls per second
    POINT ptMouse = { g_mouseX.load(std::memory_order_relaxed), g_mouseY.load(std::memory_order_relaxed) };
    ScreenToClient(g_listViewWnd, &ptMouse);

    RECT rcBounds;
    rcBounds.left = LVIR_BOUNDS;
    if (!SendMessageW(g_listViewWnd, LVM_GETITEMRECT, iItem, (LPARAM)&rcBounds)) {
        return (BYTE)g_settings.idleOpacity;
    }

    float dist = GetDistanceToRect(ptMouse, rcBounds);
    float radius = (float)(std::max)(g_settings.spotlightRadius, 1);
    
    float targetSpotFade = 0.0f;
    if (dist <= radius) {
        targetSpotFade = 1.0f - (dist / radius); 
    }

    ULONGLONG elapsed = GetTickCount64() - g_lastMouseTime.load(std::memory_order_relaxed);
    float timeFade = 1.0f;
    if (elapsed > (ULONGLONG)g_settings.idleDelay) {
        float fadeTime = (float)(elapsed - g_settings.idleDelay);
        if (g_settings.spotlightFadeOutDuration > 0) {
            timeFade = 1.0f - (fadeTime / g_settings.spotlightFadeOutDuration);
            timeFade = std::clamp(timeFade, 0.0f, 1.0f);
        } else {
            timeFade = 0.0f;
        }
    }

    float finalFade = targetSpotFade * timeFade;
    int alpha = g_settings.idleOpacity + (int)((g_settings.activeOpacity - g_settings.idleOpacity) * finalFade);
    return (BYTE)std::clamp(alpha, 0, 255);
}

////////////////////////////////////////////////////////////////////////////////// Window Search

HWND GetDesktopListView() {
    HWND hShellView = nullptr;
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (hProgman) {
        hShellView = FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
    }
    if (!hShellView) {
        HWND hWorkerW = nullptr;
        while ((hWorkerW = FindWindowExW(nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
            hShellView = FindWindowExW(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (hShellView) break;
        }
    }
    if (hShellView) {
        return FindWindowExW(hShellView, nullptr, L"SysListView32", nullptr);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////// Subclasses

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uId, DWORD_PTR dwRef) {
    if (!g_unloading) {
        if (uMsg == WM_ERASEBKGND) {
            return TRUE;
        }

        if (uMsg == WM_LBUTTONDBLCLK && g_settings.toggleOnDoubleClick) {
            LVHITTESTINFO hti = {};
            hti.pt.x = (short)LOWORD(lParam);
            hti.pt.y = (short)HIWORD(lParam);
            ListView_HitTest(hWnd, &hti);
            if (hti.flags & LVHT_NOWHERE) {
                bool nowBypassed = !g_overlayBypassed.load(std::memory_order_relaxed);
                g_overlayBypassed.store(nowBypassed, std::memory_order_relaxed);
                InvalidateRect(hWnd, NULL, FALSE);
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ShellViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uId, DWORD_PTR dwRef) {
    if (!g_unloading && uMsg == WM_NOTIFY) {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh && pnmh->hwndFrom == g_listViewWnd) {

            if (pnmh->code == NM_CUSTOMDRAW) {
                LPNMLVCUSTOMDRAW pLVCD = (LPNMLVCUSTOMDRAW)lParam;

                switch (pLVCD->nmcd.dwDrawStage) {
                    case CDDS_PREPAINT:
                        // Cache theme once globally for all icons instead of repeatedly opening/closing
                        if (!g_hTheme) {
                            g_hTheme = OpenThemeData(g_listViewWnd, L"Explorer::ListView");
                            if (!g_hTheme) g_hTheme = OpenThemeData(g_listViewWnd, L"ListView");
                        }
                        return CDRF_NOTIFYITEMDRAW;

                    case CDDS_ITEMPREPAINT:
                    {
                        int iItem = (int)pLVCD->nmcd.dwItemSpec;
                        
                        LVITEMW lvi = {0};
                        lvi.mask = LVIF_STATE;
                        lvi.iItem = iItem;
                        lvi.stateMask = LVIS_SELECTED;
                        SendMessageW(g_listViewWnd, LVM_GETITEMW, 0, (LPARAM)&lvi);
                        if (lvi.state & LVIS_SELECTED) {
                            return CDRF_DODEFAULT;
                        }

                        BYTE alpha = CalculateIconAlpha(iItem);

                        if (alpha == 255) return CDRF_DODEFAULT;
                        if (alpha == 0) return CDRF_SKIPDEFAULT;

                        HDC hdc = pLVCD->nmcd.hdc;

                        // 1. Draw Icon
                        HIMAGELIST himl = (HIMAGELIST)SendMessageW(g_listViewWnd, LVM_GETIMAGELIST, LVSIL_NORMAL, 0);
                        if (himl) {
                            lvi.mask = LVIF_IMAGE;
                            SendMessageW(g_listViewWnd, LVM_GETITEMW, 0, (LPARAM)&lvi);

                            RECT rcIcon; rcIcon.left = LVIR_ICON;
                            SendMessageW(g_listViewWnd, LVM_GETITEMRECT, iItem, (LPARAM)&rcIcon);

                            int iconWidth = 32, iconHeight = 32;
                            ImageList_GetIconSize(himl, &iconWidth, &iconHeight);

                            int drawX = rcIcon.left + ((rcIcon.right - rcIcon.left) - iconWidth) / 2;
                            int drawY = rcIcon.top + ((rcIcon.bottom - rcIcon.top) - iconHeight) / 2;

                            IMAGELISTDRAWPARAMS imldp = {0};
                            imldp.cbSize = sizeof(IMAGELISTDRAWPARAMS);
                            imldp.himl = himl;
                            imldp.i = lvi.iImage;
                            imldp.hdcDst = hdc;
                            imldp.x = drawX;
                            imldp.y = drawY;
                            imldp.cx = iconWidth;
                            imldp.cy = iconHeight;
                            imldp.fStyle = ILD_TRANSPARENT;
                            imldp.fState = ILS_ALPHA;
                            imldp.Frame = alpha;

                            ImageList_DrawIndirect(&imldp);
                        }

                        // 2. Draw Text (Using cached g_hTheme)
                        RECT rcLabel; rcLabel.left = LVIR_LABEL;
                        if (SendMessageW(g_listViewWnd, LVM_GETITEMRECT, iItem, (LPARAM)&rcLabel)) {
                            WCHAR szText[256] = {0};
                            lvi.mask = LVIF_TEXT;
                            lvi.pszText = szText;
                            lvi.cchTextMax = 256;
                            SendMessageW(g_listViewWnd, LVM_GETITEMW, 0, (LPARAM)&lvi);

                            if (szText[0] != L'\0') {
                                HDC hdcBuffer = NULL;
                                BP_PAINTPARAMS bpParams = { sizeof(BP_PAINTPARAMS) };
                                bpParams.dwFlags = BPPF_ERASE;

                                HPAINTBUFFER hbp = BeginBufferedPaint(hdc, &rcLabel, BPBF_TOPDOWNDIB, &bpParams, &hdcBuffer);
                                if (hbp) {
                                    HFONT hFont = (HFONT)SendMessageW(g_listViewWnd, WM_GETFONT, 0, 0);
                                    HFONT hOldFont = (HFONT)SelectObject(hdcBuffer, hFont);

                                    if (g_hTheme) {
                                        DTTOPTS dttOpts = { sizeof(DTTOPTS) };
                                        dttOpts.dwFlags = DTT_TEXTCOLOR | DTT_COMPOSITED | DTT_GLOWSIZE;
                                        dttOpts.crText = RGB(255, 255, 255);
                                        dttOpts.iGlowSize = 5;
                                        DrawThemeTextEx(g_hTheme, hdcBuffer, LVP_LISTITEM, 0, szText, -1, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX, &rcLabel, &dttOpts);
                                    } else {
                                        SetBkMode(hdcBuffer, TRANSPARENT);
                                        SetTextColor(hdcBuffer, RGB(255, 255, 255));
                                        DrawTextW(hdcBuffer, szText, -1, &rcLabel, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
                                    }

                                    SelectObject(hdcBuffer, hOldFont);

                                    // Per-pixel alpha scaling
                                    RGBQUAD* pBits = nullptr;
                                    int cxRow = 0;
                                    if (SUCCEEDED(GetBufferedPaintBits(hbp, &pBits, &cxRow)) && pBits) {
                                        int width = rcLabel.right - rcLabel.left;
                                        int height = rcLabel.bottom - rcLabel.top;

                                        for (int y = 0; y < height; ++y) {
                                            RGBQUAD* pRow = pBits + (y * cxRow);
                                            for (int x = 0; x < width; ++x) {
                                                if (!g_hTheme && pRow[x].rgbReserved == 0 && (pRow[x].rgbRed | pRow[x].rgbGreen | pRow[x].rgbBlue) > 0) {
                                                    pRow[x].rgbReserved = 255;
                                                }
                                                pRow[x].rgbReserved = (BYTE)((pRow[x].rgbReserved * alpha) / 255);
                                                pRow[x].rgbRed      = (BYTE)((pRow[x].rgbRed      * alpha) / 255);
                                                pRow[x].rgbGreen    = (BYTE)((pRow[x].rgbGreen    * alpha) / 255);
                                                pRow[x].rgbBlue     = (BYTE)((pRow[x].rgbBlue     * alpha) / 255);
                                            }
                                        }
                                    }

                                    EndBufferedPaint(hbp, TRUE);
                                }
                            }
                        }
                        
                        return CDRF_SKIPDEFAULT;
                    }
                }
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////// Threading & Hooks

LRESULT CALLBACK CallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && !g_subclassed && g_listViewWnd && g_shellViewWnd) {
        g_subclassed = true;
        SetWindowSubclass(g_listViewWnd, ListViewSubclassProc, SUBCLASS_ID_LV, 0);
        SetWindowSubclass(g_shellViewWnd, ShellViewSubclassProc, SUBCLASS_ID_SV, 0);
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void InitThreadProc() {
    while (!g_unloading) {
        if (g_listViewWnd && !IsWindow(g_listViewWnd)) {
            g_listViewWnd = nullptr;
            g_shellViewWnd = nullptr;
            g_subclassed = false;
        }
        
        if (!g_listViewWnd) {
            g_listViewWnd = GetDesktopListView();
            if (g_listViewWnd) {
                g_shellViewWnd = GetParent(g_listViewWnd);
                DWORD tid = GetWindowThreadProcessId(g_listViewWnd, nullptr);
                if (tid) {
                    g_hHook = SetWindowsHookExW(WH_CALLWNDPROC, CallWndProcHook, nullptr, tid);
                    PostMessageW(g_listViewWnd, WM_NULL, 0, 0);
                }
            }
        }
        Sleep(1000);
    }
}

// Optimized non-blocking update loop
void RepaintThreadProc() {
    while (!g_unloading) {
        int sleepTime = 100; // Default idle throttle

        if (g_subclassed && g_listViewWnd && IsWindow(g_listViewWnd)) {
            POINT pt;
            GetCursorPos(&pt);

            int lastX = g_mouseX.load(std::memory_order_relaxed);
            int lastY = g_mouseY.load(std::memory_order_relaxed);
            ULONGLONG lastTime = g_lastMouseTime.load(std::memory_order_relaxed);

            bool mouseMoved = (pt.x != lastX || pt.y != lastY);
            ULONGLONG timeSinceMove = GetTickCount64() - lastTime;
            
            // Check if we are inside the animation window (idle delay + fade out duration)
            bool isAnimating = timeSinceMove < (ULONGLONG)(g_settings.idleDelay + g_settings.spotlightFadeOutDuration + 100);

            if (mouseMoved) {
                g_mouseX.store(pt.x, std::memory_order_relaxed);
                g_mouseY.store(pt.y, std::memory_order_relaxed);
                g_lastMouseTime.store(GetTickCount64(), std::memory_order_relaxed);
                InvalidateRect(g_listViewWnd, NULL, FALSE);
                sleepTime = 16; // Ramp up to ~60 FPS
            } else if (isAnimating) {
                InvalidateRect(g_listViewWnd, NULL, FALSE);
                sleepTime = 16; // Maintain 60 FPS while fading out
            }
            // If neither is true, we skip InvalidateRect completely and sleep at 100ms
        }
        Sleep(sleepTime); 
    }
}

////////////////////////////////////////////////////////////////////////////////// Mod Lifecycle

void LoadSettings() {
    Settings s;
    s.idleOpacity              = std::clamp(Wh_GetIntSetting(L"idleOpacity"), 0, 255);
    s.activeOpacity            = std::clamp(Wh_GetIntSetting(L"activeOpacity"), 0, 255);
    s.spotlightRadius          = (std::max)(Wh_GetIntSetting(L"spotlightRadius"), 0);
    s.idleDelay                = (std::max)(Wh_GetIntSetting(L"idleDelay"), 0);
    s.spotlightFadeOutDuration = (std::max)(Wh_GetIntSetting(L"spotlightFadeOutDuration"), 0);
    s.toggleOnDoubleClick      = Wh_GetIntSetting(L"toggleOnDoubleClick") != 0;
    g_settings = s;

    if (g_subclassed && g_listViewWnd && IsWindow(g_listViewWnd)) {
        InvalidateRect(g_listViewWnd, NULL, FALSE);
    }
}

BOOL Wh_ModInit() {
    BufferedPaintInit();
    LoadSettings();

    g_initThread = new std::thread(InitThreadProc);
    g_repaintThread = new std::thread(RepaintThreadProc);

    return TRUE;
}

void Wh_ModUninit() {
    g_unloading = true;

    if (g_initThread) {
        if (g_initThread->joinable()) g_initThread->join();
        delete g_initThread;
        g_initThread = nullptr;
    }

    if (g_repaintThread) {
        if (g_repaintThread->joinable()) g_repaintThread->join();
        delete g_repaintThread;
        g_repaintThread = nullptr;
    }

    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = nullptr;
    }

    if (g_subclassed) {
        if (g_shellViewWnd) RemoveWindowSubclass(g_shellViewWnd, ShellViewSubclassProc, SUBCLASS_ID_SV);
        if (g_listViewWnd) RemoveWindowSubclass(g_listViewWnd, ListViewSubclassProc, SUBCLASS_ID_LV);
        
        if (g_listViewWnd && IsWindow(g_listViewWnd)) {
            InvalidateRect(g_listViewWnd, NULL, TRUE);
        }
    }

    if (g_hTheme) {
        CloseThemeData(g_hTheme);
        g_hTheme = nullptr;
    }

    BufferedPaintUnInit();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}