// ==WindhawkMod==
// @id              batteryhub-custom-taskbar-integration
// @name            BatteryHub Integration to Custom Taskbar
// @description     Places the BatteryHub mouse/keyboard battery widget next to the tray arrow on custom-styled taskbars, without its background box
// @version         1.4
// @author          jaysix06
// @github          https://github.com/jaysix06
// @homepage        https://github.com/jaysix06/batteryhub-custom-taskbar-integration
// @include         BatteryHub.exe
// @architecture    x86-64
// @license         MIT
// @compilerOptions -lole32 -loleaut32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# BatteryHub Integration to Custom Taskbar

BatteryHub shows wireless mouse/keyboard battery levels as a
widget on the Windows 11 taskbar. It positions that widget relative to the
classic tray window, so with custom taskbar styles (for example the
*Windows 11 Taskbar Styler* FluentGlass theme) the widget ends up in the wrong
place, overlapping the clock, with its own dark box.

This mod makes the widget look like part of the custom taskbar:

- Puts the widget just left of the system tray "show hidden icons" arrow
- Centres it vertically on the tray icons
- Removes BatteryHub's own background box (optional), keeping smooth text/icons
- Keeps it in place while hovering (BatteryHub's redraws can't move it back)
- Slides together with an auto-hiding taskbar (e.g. *Taskbar auto-hide when
  maximized* + pressing the Windows key)

## Tested with

- Windows 11 24H2 (build 26100)
- BatteryHub 1.1.0
- Windows 11 Taskbar Styler 1.11.2 - **FluentGlass** theme
- Taskbar height and icon size 1.3.10, Taskbar Dock Animation 1.9.2,
  Taskbar auto-hide when maximized 1.2.6, Taskbar tray system icon tweaks 1.3

Other styler themes should work as long as the tray still contains the standard
tray buttons. The default (unstyled) taskbar works too.

## Settings

- **Gap** - space between the widget and the tray arrow
- **Extra horizontal offset** - fine-tune left/right
- **Check interval** - how often the tray position is re-checked
- **Remove widget background** - turn off to keep BatteryHub's own box

## Disabling

After the mod is disabled, the widget keeps its position and look until
BatteryHub redraws it (for example on hover or a battery update). Restart
BatteryHub to restore its default placement right away.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- gap: 4
  $name: Gap (px)
  $description: Space between the widget and the tray arrow
- offsetX: 0
  $name: Extra horizontal offset (px)
  $description: Positive moves the widget right, negative moves it left
- pollIntervalMs: 750
  $name: Check interval (ms)
- removeBackground: true
  $name: Remove widget background
  $description: Strip BatteryHub's own box so only the icons and text show on the taskbar
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>

#include <atomic>
#include <cstdint>

#include <uiautomation.h>
#include <windows.h>

struct {
    int gap;
    int offsetX;
    int pollIntervalMs;
    bool removeBackground;
} g_settings;

static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_wakeEvent = nullptr;

static void LoadSettings() {
    g_settings.gap = Wh_GetIntSetting(L"gap");
    g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
    g_settings.pollIntervalMs = Wh_GetIntSetting(L"pollIntervalMs");
    if (g_settings.pollIntervalMs < 100) {
        g_settings.pollIntervalMs = 100;
    }
    g_settings.removeBackground = Wh_GetIntSetting(L"removeBackground") != 0;
}

static bool IsBatteryWidget(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    WCHAR cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return wcscmp(cls, L"WirelessBatteryWidget") == 0;
}

// ---------------------------------------------------------------------------
// Per-taskbar state
//
// The anchor says where the widget goes, in the taskbar's client coordinates
// (so it slides together with an auto-hiding taskbar): the widget's right edge
// at `right`, vertically centred on `centerY`. Both values are packed into one
// atomic so the hooks on BatteryHub's UI thread never see a torn pair.
// ---------------------------------------------------------------------------

constexpr uint64_t kNoAnchor = 0;

static uint64_t PackAnchor(int right, int centerY) {
    // Offset so that a valid anchor is never equal to kNoAnchor.
    return (uint64_t)(uint32_t)(right + 0x40000000) << 32 |
           (uint32_t)(centerY + 0x40000000);
}

static void UnpackAnchor(uint64_t packed, int* right, int* centerY) {
    *right = (int)(uint32_t)(packed >> 32) - 0x40000000;
    *centerY = (int)(uint32_t)packed - 0x40000000;
}

struct TrayState {
    std::atomic<HWND> tray{nullptr};
    std::atomic<uint64_t> anchor{kNoAnchor};
    // Worker thread only:
    IUIAutomationElement* button = nullptr;
    int pollsSinceSearch = 0;
};

static TrayState g_trays[8];

// Re-search the tray button now and then, so that e.g. a "show hidden icons"
// arrow that appears later replaces a fallback element.
constexpr int kResearchEveryPolls = 8;

static TrayState* FindTrayState(HWND tray) {
    for (auto& s : g_trays) {
        if (s.tray.load() == tray) {
            return &s;
        }
    }
    return nullptr;
}

static TrayState* GetOrCreateTrayState(HWND tray) {
    if (TrayState* s = FindTrayState(tray)) {
        return s;
    }
    for (auto& s : g_trays) {
        HWND old = s.tray.load();
        if (!old || !IsWindow(old)) {
            if (s.button) {
                s.button->Release();
                s.button = nullptr;
            }
            s.anchor.store(kNoAnchor);
            s.pollsSinceSearch = 0;
            s.tray.store(tray);
            return &s;
        }
    }
    return nullptr;
}

static void ReleaseTrayStates() {
    for (auto& s : g_trays) {
        if (s.button) {
            s.button->Release();
            s.button = nullptr;
        }
    }
}

// ---------------------------------------------------------------------------
// Finding the tray arrow with UI Automation
// ---------------------------------------------------------------------------

static IUIAutomationElement* FindFirstWithIds(IUIAutomation* uia,
                                              IUIAutomationElement* root,
                                              PCWSTR className,
                                              PCWSTR automationId) {
    VARIANT vClass;
    vClass.vt = VT_BSTR;
    vClass.bstrVal = SysAllocString(className);
    VARIANT vId;
    vId.vt = VT_BSTR;
    vId.bstrVal = SysAllocString(automationId);

    IUIAutomationCondition* classCond = nullptr;
    IUIAutomationCondition* idCond = nullptr;
    IUIAutomationCondition* both = nullptr;
    IUIAutomationElement* found = nullptr;

    if (SUCCEEDED(uia->CreatePropertyCondition(UIA_ClassNamePropertyId, vClass,
                                               &classCond)) &&
        SUCCEEDED(uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, vId,
                                               &idCond)) &&
        SUCCEEDED(uia->CreateAndCondition(classCond, idCond, &both))) {
        root->FindFirst(TreeScope_Descendants, both, &found);
    }

    if (both) {
        both->Release();
    }
    if (idCond) {
        idCond->Release();
    }
    if (classCond) {
        classCond->Release();
    }
    VariantClear(&vId);
    VariantClear(&vClass);
    return found;
}

// The "show hidden icons" arrow is a SystemTray.NormalButton with the
// AutomationId SystemTrayIcon. Without the arrow, fall back to the first
// notification icon (the left edge of the notification icons).
static IUIAutomationElement* FindTrayButton(IUIAutomation* uia, HWND tray) {
    IUIAutomationElement* root = nullptr;
    if (FAILED(uia->ElementFromHandle(tray, &root)) || !root) {
        return nullptr;
    }
    IUIAutomationElement* found = FindFirstWithIds(
        uia, root, L"SystemTray.NormalButton", L"SystemTrayIcon");
    if (!found) {
        found = FindFirstWithIds(uia, root, L"SystemTray.NormalButton",
                                 L"NotifyItemIcon");
    }
    root->Release();
    return found;
}

static bool GetTrayButtonRect(IUIAutomation* uia, TrayState* s, RECT* out) {
    if (s->button && ++s->pollsSinceSearch >= kResearchEveryPolls) {
        s->button->Release();
        s->button = nullptr;
    }
    for (int attempt = 0; attempt < 2; attempt++) {
        if (!s->button) {
            s->button = FindTrayButton(uia, s->tray.load());
            s->pollsSinceSearch = 0;
        }
        if (!s->button) {
            return false;
        }
        RECT r{};
        if (SUCCEEDED(s->button->get_CurrentBoundingRectangle(&r)) &&
            r.right > r.left) {
            *out = r;
            return true;
        }
        // Stale element (tray rebuilt): drop it and search again.
        s->button->Release();
        s->button = nullptr;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Positioning
// ---------------------------------------------------------------------------

using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t SetWindowPos_Original;

// Runs a block with per-monitor-v2 DPI awareness so that all coordinates match
// the ones UI Automation reported.
struct DpiScope {
    DPI_AWARENESS_CONTEXT prev;
    DpiScope() {
        prev = SetThreadDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }
    ~DpiScope() {
        if (prev) {
            SetThreadDpiAwarenessContext(prev);
        }
    }
};

// Target top-left of the widget in its parent taskbar's client coordinates.
static bool TargetClientPos(HWND widget, int w, int h, POINT* out) {
    TrayState* s = FindTrayState(GetParent(widget));
    if (!s) {
        return false;
    }
    uint64_t packed = s->anchor.load();
    if (packed == kNoAnchor) {
        return false;
    }
    int right, centerY;
    UnpackAnchor(packed, &right, &centerY);
    *out = POINT{right - w, centerY - h / 2};
    return true;
}

static void EnsureWidgetPosition(HWND widget) {
    DpiScope dpi;
    RECT wr;
    if (!GetWindowRect(widget, &wr)) {
        return;
    }
    POINT target;
    if (!TargetClientPos(widget, wr.right - wr.left, wr.bottom - wr.top,
                         &target)) {
        return;
    }
    POINT current = {wr.left, wr.top};
    MapWindowPoints(nullptr, GetParent(widget), &current, 1);
    if (target.x == current.x && target.y == current.y) {
        return;
    }
    UINT flags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE;
    // Never block on the widget's UI thread from another thread.
    if (GetWindowThreadProcessId(widget, nullptr) != GetCurrentThreadId()) {
        flags |= SWP_ASYNCWINDOWPOS;
    }
    SetWindowPos_Original(widget, nullptr, target.x, target.y, 0, 0, flags);
}

static void PositionWidgetInTray(IUIAutomation* uia, HWND tray) {
    HWND widget =
        FindWindowExW(tray, nullptr, L"WirelessBatteryWidget", nullptr);
    if (!widget || !IsWindowVisible(widget)) {
        return;
    }
    TrayState* s = GetOrCreateTrayState(tray);
    if (!s) {
        return;
    }

    RECT before, after, btn;
    if (!GetWindowRect(tray, &before)) {
        return;
    }
    bool gotBtn = GetTrayButtonRect(uia, s, &btn);
    if (!GetWindowRect(tray, &after)) {
        return;
    }

    // UI Automation lags behind the taskbar when it slides in/out (auto-hide),
    // reporting the button where it *was*. Only trust a reading taken while
    // the taskbar stood still and that lies inside the taskbar.
    bool trusted = gotBtn && EqualRect(&before, &after) &&
                   btn.top >= after.top && btn.bottom <= after.bottom &&
                   btn.left >= after.left && btn.right <= after.right;
    if (trusted) {
        POINT tl = {btn.left, btn.top};
        POINT br = {btn.right, btn.bottom};
        MapWindowPoints(nullptr, tray, &tl, 1);
        MapWindowPoints(nullptr, tray, &br, 1);
        s->anchor.store(PackAnchor(tl.x - g_settings.gap + g_settings.offsetX,
                                   (tl.y + br.y) / 2));
    }

    EnsureWidgetPosition(widget);
}

static BOOL CALLBACK EnumTrayProc(HWND hwnd, LPARAM lParam) {
    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
         wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0)) {
        PositionWidgetInTray(reinterpret_cast<IUIAutomation*>(lParam), hwnd);
    }
    return TRUE;
}

static DWORD WINAPI WorkerThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IUIAutomation* uia = nullptr;
    CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
                     __uuidof(IUIAutomation), reinterpret_cast<void**>(&uia));
    if (uia) {
        // Bound every UI Automation call, so that a busy or hung Explorer
        // can't keep this thread (and thus unloading) waiting.
        IUIAutomation2* uia2 = nullptr;
        if (SUCCEEDED(uia->QueryInterface(__uuidof(IUIAutomation2),
                                          reinterpret_cast<void**>(&uia2)))) {
            uia2->put_ConnectionTimeout(1000);
            uia2->put_TransactionTimeout(1000);
            uia2->Release();
        }
    } else {
        Wh_Log(L"Failed to create UI Automation instance");
    }

    HANDLE events[2] = {g_stopEvent, g_wakeEvent};
    while (true) {
        if (uia) {
            EnumWindows(EnumTrayProc, reinterpret_cast<LPARAM>(uia));
        }
        DWORD w = WaitForMultipleObjects(2, events, FALSE,
                                         g_settings.pollIntervalMs);
        if (w == WAIT_OBJECT_0) {
            break;
        }
    }

    ReleaseTrayStates();
    if (uia) {
        uia->Release();
    }
    CoUninitialize();
    return 0;
}

// Redirect BatteryHub's own moves of the widget straight to our position, so
// it never visibly jumps back (e.g. while hovering).
static BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                                     HWND after,
                                     int X,
                                     int Y,
                                     int cx,
                                     int cy,
                                     UINT flags) {
    if (!(flags & SWP_NOMOVE) && IsBatteryWidget(hWnd)) {
        DpiScope dpi;
        int w = cx;
        int h = cy;
        if (flags & SWP_NOSIZE) {
            RECT wr;
            GetWindowRect(hWnd, &wr);
            w = wr.right - wr.left;
            h = wr.bottom - wr.top;
        }
        POINT t;
        if (TargetClientPos(hWnd, w, h, &t)) {
            return SetWindowPos_Original(hWnd, after, t.x, t.y, cx, cy, flags);
        }
    }
    return SetWindowPos_Original(hWnd, after, X, Y, cx, cy, flags);
}

using MoveWindow_t = decltype(&MoveWindow);
static MoveWindow_t MoveWindow_Original;
static BOOL WINAPI
MoveWindow_Hook(HWND hWnd, int X, int Y, int w, int h, BOOL repaint) {
    if (IsBatteryWidget(hWnd)) {
        DpiScope dpi;
        POINT t;
        if (TargetClientPos(hWnd, w, h, &t)) {
            return MoveWindow_Original(hWnd, t.x, t.y, w, h, repaint);
        }
    }
    return MoveWindow_Original(hWnd, X, Y, w, h, repaint);
}

// ---------------------------------------------------------------------------
// Background removal
//
// BatteryHub renders the widget (box + icons + text) into a 32bpp premultiplied
// bitmap and shows it with UpdateLayeredWindow. We copy that bitmap, find the
// box colour (most common non-transparent pixel) and apply "colour to alpha":
// every pixel P = a*F + (1-a)*B becomes a*F, so the box disappears while the
// anti-aliased icons/text keep smooth edges.
// ---------------------------------------------------------------------------

// Returns false (leaving the pixels untouched) when no clear box colour exists,
// e.g. if BatteryHub draws without a box.
static bool RemoveBackground(DWORD* px, int count) {
    // Find the most common non-transparent pixel value (the box fill).
    const int kSlots = 64;
    DWORD vals[kSlots];
    int counts[kSlots];
    int used = 0;
    int sampled = 0;
    for (int i = 0; i < count; i += 3) {
        DWORD p = px[i];
        if ((p >> 24) == 0) {
            continue;
        }
        sampled++;
        int j = 0;
        for (; j < used; j++) {
            if (vals[j] == p) {
                counts[j]++;
                break;
            }
        }
        if (j == used && used < kSlots) {
            vals[used] = p;
            counts[used] = 1;
            used++;
        }
    }
    if (!used) {
        return false;
    }
    int best = 0;
    for (int j = 1; j < used; j++) {
        if (counts[j] > counts[best]) {
            best = j;
        }
    }
    // A box fill covers most of the widget; text or icon colours never do.
    if (counts[best] * 100 < sampled * 40) {
        return false;
    }
    DWORD bg = vals[best];

    int bA = bg >> 24;
    int bR = (bg >> 16) & 0xFF;
    int bG = (bg >> 8) & 0xFF;
    int bB = bg & 0xFF;

    for (int i = 0; i < count; i++) {
        DWORD p = px[i];
        int pA = p >> 24;
        int pR = (p >> 16) & 0xFF;
        int pG = (p >> 8) & 0xFF;
        int pB = p & 0xFF;
        if (pA == 0) {
            continue;
        }

        if (pA < bA) {
            // Anti-aliased edge of the box itself (e.g. rounded corners): work
            // in straight (un-premultiplied) colour so a faded box pixel is
            // recognised as box, not as dark foreground.
            float sc[3] = {pR * 255.0f / pA, pG * 255.0f / pA,
                           pB * 255.0f / pA};
            float bc[3] = {bR * 255.0f / bA, bG * 255.0f / bA,
                           bB * 255.0f / bA};
            float a = 0.0f;
            for (int c = 0; c < 3; c++) {
                float d = sc[c] - bc[c];
                float den = d > 0 ? 255.0f - bc[c] : bc[c];
                float v = den > 0 ? (d > 0 ? d : -d) / den : 0.0f;
                if (v > a) {
                    a = v;
                }
            }
            if (a > 1.0f) {
                a = 1.0f;
            }
            if (a < 0.08f) {
                px[i] = 0;
                continue;
            }
            float oA = pA * a;
            DWORD out = (DWORD)(oA + 0.5f) << 24;
            for (int c = 0; c < 3; c++) {
                float f = (sc[c] - (1.0f - a) * bc[c]) / a;  // straight fg
                if (f < 0) {
                    f = 0;
                }
                if (f > 255) {
                    f = 255;
                }
                out |= (DWORD)(f * oA / 255.0f + 0.5f) << (16 - 8 * c);
            }
            px[i] = out;
            continue;
        }

        // Coverage of the foreground over the box, per channel; take the max.
        float a = 0.0f;
        auto chan = [&](int pc, int bc) {
            float v;
            if (pc > bc) {
                v = (float)(pc - bc) / (float)(255 - bc ? 255 - bc : 1);
            } else if (pc < bc) {
                v = (float)(bc - pc) / (float)(bc ? bc : 1);
            } else {
                v = 0.0f;
            }
            if (v > a) {
                a = v;
            }
        };
        chan(pR, bR);
        chan(pG, bG);
        chan(pB, bB);
        if (bA < 255) {
            float va = (float)(pA - bA) / (float)(255 - bA);
            if (va > a) {
                a = va;
            }
        }
        if (a > 1.0f) {
            a = 1.0f;
        }
        if (a < 0.06f) {
            px[i] = 0;
            continue;
        }

        float k = 1.0f - a;
        int oA = (int)(pA - k * bA + 0.5f);
        int oR = (int)(pR - k * bR + 0.5f);
        int oG = (int)(pG - k * bG + 0.5f);
        int oB = (int)(pB - k * bB + 0.5f);
        if (oA < 0) {
            oA = 0;
        }
        if (oA > 255) {
            oA = 255;
        }
        auto clampc = [&](int c) { return c < 0 ? 0 : (c > oA ? oA : c); };
        px[i] = ((DWORD)oA << 24) | ((DWORD)clampc(oR) << 16) |
                ((DWORD)clampc(oG) << 8) | (DWORD)clampc(oB);
    }
    return true;
}

using UpdateLayeredWindow_t = decltype(&UpdateLayeredWindow);
static UpdateLayeredWindow_t UpdateLayeredWindow_Original;

// Shows the widget bitmap with the background removed. Returns false, without
// calling UpdateLayeredWindow, when the bitmap should be shown unchanged.
static bool UpdateWithoutBackground(HWND hWnd,
                                    HDC hdcDst,
                                    POINT* pptDst,
                                    SIZE* psize,
                                    HDC hdcSrc,
                                    POINT* pptSrc,
                                    COLORREF crKey,
                                    BLENDFUNCTION* pblend,
                                    DWORD dwFlags,
                                    BOOL* result) {
    // Only per-pixel alpha bitmaps carry a box we can make transparent; a
    // bitmap meant for ULW_OPAQUE / ULW_COLORKEY has no alpha channel.
    if (!(dwFlags & ULW_ALPHA) || !pblend ||
        !(pblend->AlphaFormat & AC_SRC_ALPHA) || !hdcSrc || !psize ||
        psize->cx <= 0 || psize->cy <= 0) {
        return false;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = psize->cx;
    bmi.bmiHeader.biHeight = -psize->cy;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC mem = CreateCompatibleDC(hdcSrc);
    HBITMAP dib = mem ? CreateDIBSection(mem, &bmi, DIB_RGB_COLORS, &bits,
                                         nullptr, 0)
                      : nullptr;
    if (!dib || !bits) {
        if (dib) {
            DeleteObject(dib);
        }
        if (mem) {
            DeleteDC(mem);
        }
        return false;
    }

    HGDIOBJ old = SelectObject(mem, dib);
    POINT src = pptSrc ? *pptSrc : POINT{0, 0};
    BitBlt(mem, 0, 0, psize->cx, psize->cy, hdcSrc, src.x, src.y, SRCCOPY);
    GdiFlush();

    bool changed =
        RemoveBackground(static_cast<DWORD*>(bits), psize->cx * psize->cy);
    if (changed) {
        POINT zero{0, 0};
        *result = UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize,
                                               mem, &zero, crKey, pblend,
                                               dwFlags);
    }

    SelectObject(mem, old);
    DeleteObject(dib);
    DeleteDC(mem);
    return changed;
}

static BOOL WINAPI UpdateLayeredWindow_Hook(HWND hWnd,
                                            HDC hdcDst,
                                            POINT* pptDst,
                                            SIZE* psize,
                                            HDC hdcSrc,
                                            POINT* pptSrc,
                                            COLORREF crKey,
                                            BLENDFUNCTION* pblend,
                                            DWORD dwFlags) {
    if (!IsBatteryWidget(hWnd)) {
        return UpdateLayeredWindow_Original(hWnd, hdcDst, pptDst, psize,
                                            hdcSrc, pptSrc, crKey, pblend,
                                            dwFlags);
    }

    // BatteryHub passes its own position on every redraw (hover animations
    // included). Drop it and keep the widget where we put it.
    POINT unused;
    bool fixPos = pptDst && TargetClientPos(hWnd, 0, 0, &unused);
    POINT* dst = fixPos ? nullptr : pptDst;

    BOOL ret = FALSE;
    if (!g_settings.removeBackground ||
        !UpdateWithoutBackground(hWnd, hdcDst, dst, psize, hdcSrc, pptSrc,
                                 crKey, pblend, dwFlags, &ret)) {
        ret = UpdateLayeredWindow_Original(hWnd, hdcDst, dst, psize, hdcSrc,
                                           pptSrc, crKey, pblend, dwFlags);
    }

    if (fixPos) {
        EnsureWidgetPosition(hWnd);
    }
    return ret;
}

// ---------------------------------------------------------------------------
// Mod entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(MoveWindow, MoveWindow_Hook,
                                   &MoveWindow_Original);
    WindhawkUtils::SetFunctionHook(UpdateLayeredWindow,
                                   UpdateLayeredWindow_Hook,
                                   &UpdateLayeredWindow_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
}

void Wh_ModUninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        // The worker's blocking calls are all bounded (UIA timeouts,
        // asynchronous SetWindowPos), so this wait always ends.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
    }
    g_stopEvent = g_wakeEvent = nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}
