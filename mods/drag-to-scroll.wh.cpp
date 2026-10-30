// ==WindhawkMod==
// @id              drag-to-scroll
// @name            Drag to Scroll
// @description     Hold Ctrl + Right-Click and drag to scroll smoothly in desktop applications with adaptive line precision, dynamic velocity, and hands-free cruising.
// @version         1.1.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @license         MIT
// @include         *
// @exclude         unsecapp.exe
// @exclude         rundll32.exe
// @exclude         svchost.exe
// @exclude         dwm.exe
// @exclude         conhost.exe
// @exclude         sihost.exe
// @exclude         taskhostw.exe
// @exclude         TextInputHost.exe
// @exclude         RuntimeBroker.exe
// @exclude         UserOOBEBroker.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         FileCoAuth.exe
// @exclude         OneDrive.exe
// @exclude         OneDrive.Sync.Service.exe
// @compilerOptions -luser32 -lgdi32 -lcrypt32
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# Drag to Scroll

Hold **`Ctrl` + Right-Click** and drag your mouse to scroll naturally and effortlessly across desktop apps—including web browsers (Chrome, Edge, Firefox), documents (Word, PDFs), code editors (VS Code), and File Explorer.

## Vertical & Horizontal Scrolling
![Drag to Scroll Demo 1](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/refs/heads/master/dts_preview_1.webp)


## Hands-Free Continuous Cruising
![Drag to Scroll Demo 2](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/refs/heads/master/dts_preview_2.webp)

---

## ⚡ Quick Start (How to Use)

* **To Scroll:** Hold down **`Ctrl`**, hold down the **Right Mouse Button**, and move your mouse up, down, left, or right.
* **To Stop:** Release the Right Mouse Button.
* **Normal Right-Click:** Regular Right-Click is completely untouched—click anywhere like you normally do to open context menus.
* **Need an App's Own Ctrl + Right-Click?** Just tap `Ctrl + Right-Click` in place without moving your mouse—it triggers the app's native shortcut as usual.
* **Hands-Free Cruising (Optional):** Turn on *Continuous Cruising* in settings. Drag to start moving, let go of the mouse button while still holding `Ctrl`, and the page will glide continuously on its own. Tap any key or click to stop.

---

## ✨ Everyday Highlights (Why You'll Love It)

* 🎯 **Smart Straight-Line Panning:** No more accidental diagonal wobbling when reading documents or scanning spreadsheets. Move up/down or left/right, and the mod keeps your movement locked straight on track.
* 👁️ **High-Contrast, All-Surface Cursors:** Custom drag cursors with protective contours are built directly into the mod. Whether you use pure Dark Mode, bright white documents, or customized themes, the cursor stays crisp, sharp, and easy to follow.
* 🔍 **Zero Accidental Zooming:** Holding `Ctrl` while scrolling in web browsers usually messes up your page zoom. This mod automatically shields your browser during both the drag and the gentle stop, so you never accidentally blow up text or shrink your screen.
* 🚀 **Flick & Glide Dynamic Speed:** Drag slowly for comfortable, line-by-line reading, or flick your mouse quickly to fly across lengthy web pages and documents.
* 📱 **Touchscreen or Traditional Feel:** Choose between natural pull-to-scroll (moving down drags content down, just like a phone or tablet) or classic wheel style in settings.

---

## 🛠️ Detailed Features

* **Directional Axis Locking & 2D Horizontal Panning:** Full sideways and vertical scrolling with smart threshold checks to lock your movement to the primary direction.
* **Silky Smooth Motion Filtering:** Low-pass input smoothing eliminates hand tremors and mouse sensor jitter, providing a fluid, trackpad-like glide.
* **Sub-Notch High-Resolution Micro-Scrolling:** Emits fractional wheel increments to modern web browsers and editors for pixel-level precision instead of jagged notch jumps.
* **Browser Compositor Drain Masking:** Keeps `Ctrl` and modifier flags suppressed during both active drags and kinetic momentum drain, preventing compositor zoom interference in Chromium and Gecko engines.
* **Embedded High-DPI Dual-Contour Cursors:** Custom self-contained cursor assets automatically scale to your system DPI display metrics (`SM_CXCURSOR`) with centered hotspots and zero external file requirements.
* **Legacy Control Detection:** Detects classic Win32 controls (e.g., File Explorer list views, TreeViews, text fields) and automatically switches to discrete line steps (`WM_VSCROLL` / `WM_HSCROLL`) for native stability.
* **Accidental Click Protection:** Tapping `Ctrl + Right-Click` within the deadzone seamlessly passes through as a standard `Ctrl + Right-Click` action.

---

## ⚠️ Compatibility & Limitations

* **Microsoft Store & Modern UWP Apps (Not Supported):** Modern packaged applications installed from the Microsoft Store (such as Calculator, Weather, Microsoft To Do) as well as Windows system surfaces (Start menu, Action Center, Windows Settings) handle input through isolated modern pointer pipelines. Because they bypass classic Win32 mouse messages, the drag gesture cannot be detected over these windows.
* **Windows File Explorer Folder Views:** Modern Windows 11 File Explorer views scroll using standard wheel notches configured by system mouse settings (*Settings > Bluetooth & devices > Mouse > Lines to scroll at a time*), rather than fractional micro-scrolling.
* **Portable Windhawk & Elevation:** The mod works in elevated applications when using the standard Windhawk service. If you are using portable Windhawk without running it as Administrator, elevated application windows cannot be targeted.
* **Older Desktop Utilities (Setting Adjustment):** While modern browsers and code editors glide effortlessly with micro-scrolling, some legacy Win32 programs may stutter when receiving fractional wheel deltas. If scrolling feels jumpy in an older program, disable **Ultra-Smooth Scrolling** in settings to use standard wheel notches instead.
* **3D & Creative Suites (Custom Shortcuts):** Software with native `Ctrl + Right-Click` dragging shortcuts (such as Blender, AutoCAD, or video games) will conflict with this gesture. You can exclude those specific programs under the mod's **Advanced** tab in Windhawk.

---

### 🧩 More from this author
* **[Explorer Custom Shortcuts](https://windhawk.net/mods/explorer-custom-shortcuts)** – Customizable keyboard shortcuts for File Explorer: open terminal, copy paths, create files/folders, toggle hidden items, and launch apps with hotkeys.

### Author & Attribution
* **Created and maintained by ArvindSaini978 ([GitHub](https://github.com/ArvindSaini978)).**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- touch_style: true
  $name: "Natural / Touchscreen Scrolling"
  $description: "Drag down to pull content down (like a touchscreen). Disable to scroll like a standard mouse wheel."
- enable_horizontal: true
  $name: "Enable Horizontal Scrolling"
  $description: "Drag left or right to scroll sideways through wide documents, spreadsheets, and web pages."
- enable_cruising: false
  $name: "Continuous Cruising (Hold Ctrl)"
  $description: "Let go of Right-Click while still holding Ctrl to let the page keep scrolling on its own. Disengages on any click or keypress."
- pixels_per_notch: 45
  $name: "Drag Distance per Step (Pixels)"
  $description: "Distance in pixels required to trigger one scroll step (minimum 10 px). Lower numbers scroll faster; higher numbers make it gentler and more controlled."
- deadzone: 5
  $name: "Drag Deadzone (Pixels)"
  $description: "Small buffer before scrolling begins. Clicking without dragging still acts like a normal right-click."
- enable_velocity: true
  $name: "Dynamic Velocity Scaling"
  $description: "Move slowly for precise line-by-line reading, or flick quickly to speed through long pages."
- smooth_wheel: true
  $name: "Ultra-Smooth Scrolling"
  $description: "Enables silky, trackpad-style smooth scrolling. Disable this if scrolling feels jittery or unresponsive in older programs."
*/
// ==/WindhawkModSettings==

// clang-format on

#include <wincrypt.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>

constexpr ULONG_PTR MASK_EXTRA_INFO = 0x5343524C;    // Tag: 'SCRL'
constexpr ULONG_PTR REPLAY_EXTRA_INFO = 0x5245504C;  // Tag: 'REPL'
constexpr int SMOOTH_SCROLL_CHUNK = 15;

struct ModSettings {
    bool touch_style = true;
    bool enable_cruising = false;
    double pixels_per_notch = 45.0;
    int deadzone = 5;
    bool enable_velocity = true;
    bool smooth_wheel = true;
    bool enable_horizontal = true;
};

static ModSettings g_settings;

enum class ScrollState { Idle, Pending, Scrolling, Cruising };
static std::atomic<ScrollState> g_state{ScrollState::Idle};
static std::atomic<bool> g_swallowRButtonUp{false};
static std::atomic<UINT> g_swallowButtonUp{0};

enum class DragAxis { VerticalOnly, HorizontalOnly };
static DragAxis g_activeAxis = DragAxis::VerticalOnly;

static POINT g_anchorPt = {0, 0};
static POINT g_lastPt = {0, 0};
static double g_accumY = 0.0;
static double g_accumX = 0.0;
static double g_smoothDx = 0.0;
static double g_smoothDy = 0.0;
static RECT g_windowBounds = {0, 0, 0, 0};
static HWND g_captureHwnd = nullptr;

static HCURSOR g_curHand = nullptr;
static HCURSOR g_curSizeNS = nullptr;
static HCURSOR g_curSizeWE = nullptr;
static HCURSOR g_curCruising = nullptr;

static HANDLE g_hCruiseThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_cruiseEvent = nullptr;
static std::atomic<bool> g_threadCreated{false};

struct CruiseContext {
    POINT anchorPt;
    RECT windowBounds;
    double pixels_per_notch;
    int deadzone;
    bool touch_style;
    bool smooth_wheel;
    bool enable_horizontal;
    DragAxis axis;
};
static CruiseContext g_cruiseCtx;
static CRITICAL_SECTION g_cruiseCs;

using GetAsyncKeyState_t = SHORT(WINAPI*)(int);
static GetAsyncKeyState_t pfnGetAsyncKeyState_Original = nullptr;

HCURSOR CreateCursorFromBase64(const char* base64Str,
                               int fallbackHotX = 16,
                               int fallbackHotY = 16) {
    if (!base64Str)
        return nullptr;

    DWORD binSize = 0;
    if (!CryptStringToBinaryA(base64Str, 0, CRYPT_STRING_BASE64, nullptr,
                              &binSize, nullptr, nullptr) ||
        binSize < 22)
        return nullptr;

    auto* pData = static_cast<unsigned char*>(malloc(binSize));
    if (!pData)
        return nullptr;

    if (!CryptStringToBinaryA(base64Str, 0, CRYPT_STRING_BASE64, pData,
                              &binSize, nullptr, nullptr)) {
        free(pData);
        return nullptr;
    }

    WORD fileType = *reinterpret_cast<const WORD*>(pData + 2);
    DWORD dwBytesInRes = *reinterpret_cast<const DWORD*>(pData + 14);
    DWORD dwImageOffset = *reinterpret_cast<const DWORD*>(pData + 18);

    HCURSOR hResult = nullptr;

    if (dwImageOffset + dwBytesInRes <= binSize) {
        const unsigned char* pImg = pData + dwImageOffset;
        bool isPng = (dwBytesInRes >= 8 && pImg[0] == 0x89 && pImg[1] == 'P' &&
                      pImg[2] == 'N' && pImg[3] == 'G');

        BOOL loadAsIcon = (fileType == 1 || isPng) ? TRUE : FALSE;

        HICON hRaw = CreateIconFromResourceEx(
            const_cast<PBYTE>(pImg), dwBytesInRes, loadAsIcon, 0x00030000, 0, 0,
            LR_DEFAULTCOLOR);

        if (hRaw) {
            int targetWidth = GetSystemMetrics(SM_CXCURSOR);
            int targetHeight = GetSystemMetrics(SM_CYCURSOR);
            if (targetWidth <= 0)
                targetWidth = 32;
            if (targetHeight <= 0)
                targetHeight = 32;

            // Downscale raw image to standard cursor size
            HICON hScaled = static_cast<HICON>(CopyImage(
                hRaw, IMAGE_ICON, targetWidth, targetHeight, LR_COPYRETURNORG));
            DestroyIcon(hRaw);

            if (hScaled) {
                ICONINFO ii = {};
                if (GetIconInfo(hScaled, &ii)) {
                    ii.fIcon = FALSE;  // Force cursor type
                    ii.xHotspot = targetWidth / 2;
                    ii.yHotspot = targetHeight / 2;

                    hResult = CreateIconIndirect(&ii);

                    if (ii.hbmColor)
                        DeleteObject(ii.hbmColor);
                    if (ii.hbmMask)
                        DeleteObject(ii.hbmMask);
                }
                DestroyIcon(hScaled);
            }
        }
    }

    free(pData);
    return hResult;
}

// clang-format off

static const char* kCurVerticalBase64   = 
    "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";
;
static const char* kCurHorizontalBase64 =     
    "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"
;
static const char* kCurCruisingBase64   =     
    "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"
;

// clang-format on

static inline bool IsCtrlPhysicallyDown() {
    if (pfnGetAsyncKeyState_Original) {
        return (pfnGetAsyncKeyState_Original(VK_CONTROL) & 0x8000) != 0;
    }
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
}

static std::atomic<ULONGLONG> g_lastScrollEndTime{0};

static inline bool IsInScrollOrGracePeriod() {
    if (g_state.load() != ScrollState::Idle)
        return true;
    // 250ms grace window to let browser compositors drain kinetic wheel events
    return (GetTickCount64() - g_lastScrollEndTime.load()) < 250;
}

void LoadSettings() {
    g_settings.touch_style = Wh_GetIntSetting(L"touch_style") != 0;
    g_settings.enable_cruising = Wh_GetIntSetting(L"enable_cruising") != 0;
    g_settings.pixels_per_notch = static_cast<double>(
        std::max(10, Wh_GetIntSetting(L"pixels_per_notch")));
    g_settings.deadzone = std::clamp(Wh_GetIntSetting(L"deadzone"), 0, 1000);
    g_settings.enable_velocity = Wh_GetIntSetting(L"enable_velocity") != 0;
    g_settings.smooth_wheel = Wh_GetIntSetting(L"smooth_wheel") != 0;
    g_settings.enable_horizontal = Wh_GetIntSetting(L"enable_horizontal") != 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void SendMaskedWheelAsync(int deltaY) {
    if (deltaY == 0)
        return;

    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = static_cast<DWORD>(static_cast<LONG>(deltaY));
    input.mi.dwExtraInfo = MASK_EXTRA_INFO;
    SendInput(1, &input, sizeof(INPUT));
}

void SendMaskedHWheelAsync(int deltaX) {
    if (deltaX == 0)
        return;

    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
    input.mi.mouseData = static_cast<DWORD>(static_cast<LONG>(deltaX));
    input.mi.dwExtraInfo = MASK_EXTRA_INFO;
    SendInput(1, &input, sizeof(INPUT));
}

bool GetScrollTargetInfo(POINT pt, HWND& outTarget) {
    outTarget = WindowFromPoint(pt);
    if (!outTarget)
        return false;

    DWORD targetPid = 0;
    GetWindowThreadProcessId(outTarget, &targetPid);
    if (targetPid != GetCurrentProcessId()) {
        outTarget = nullptr;
        return false;
    }

    WCHAR cls[64] = {};
    GetClassNameW(outTarget, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"SysHeader32") == 0) {
        HWND hParent = GetParent(outTarget);
        if (hParent) {
            outTarget = hParent;
            GetClassNameW(outTarget, cls, ARRAYSIZE(cls));
        }
    }

    WCHAR lowerCls[64] = {};
    for (int i = 0; cls[i] && i < 63; ++i) {
        lowerCls[i] = static_cast<WCHAR>(towlower(cls[i]));
    }

    return (_wcsicmp(cls, L"SysListView32") == 0 ||
            _wcsicmp(cls, L"SysTreeView32") == 0 ||
            _wcsicmp(cls, L"Edit") == 0 || _wcsicmp(cls, L"ListBox") == 0 ||
            wcsstr(lowerCls, L"richedit") != nullptr);
}

int TakeWholeSteps(double& accum, int step) {
    if (step <= 0)
        return 0;

    int notches = static_cast<int>(accum / step);
    if (notches != 0) {
        accum -= static_cast<double>(notches * step);
        return notches;
    }
    return 0;
}

void SendScrollMessage(HWND hTarget, UINT msg, WPARAM cmd) {
    if (GetWindowThreadProcessId(hTarget, nullptr) == GetCurrentThreadId()) {
        SendMessageW(hTarget, msg, cmd, 0);
    } else {
        PostMessageW(hTarget, msg, cmd, 0);
    }
}

void DispatchScroll(int notchCount, HWND hTarget, bool isDiscrete) {
    if (notchCount == 0)
        return;

    if (isDiscrete && hTarget) {
        WPARAM cmd = (notchCount > 0) ? SB_LINEUP : SB_LINEDOWN;
        int count = std::abs(notchCount);
        for (int i = 0; i < count; ++i) {
            SendScrollMessage(hTarget, WM_VSCROLL, cmd);
        }
    } else {
        int chunkSize =
            g_settings.smooth_wheel ? SMOOTH_SCROLL_CHUNK : WHEEL_DELTA;
        int deltaY = notchCount * chunkSize;
        SendMaskedWheelAsync(deltaY);
    }
}

void DispatchHScroll(int notchCount, HWND hTarget, bool isDiscrete) {
    if (notchCount == 0)
        return;

    if (isDiscrete && hTarget) {
        WPARAM cmd = (notchCount > 0) ? SB_LINERIGHT : SB_LINELEFT;
        int count = std::abs(notchCount);
        for (int i = 0; i < count; ++i) {
            SendScrollMessage(hTarget, WM_HSCROLL, cmd);
        }
    } else {
        int chunkSize =
            g_settings.smooth_wheel ? SMOOTH_SCROLL_CHUNK : WHEEL_DELTA;
        int deltaX = notchCount * chunkSize;
        SendMaskedHWheelAsync(deltaX);
    }
}

DWORD WINAPI CruiseWorkerThread(LPVOID) {
    HANDLE handles[] = {g_stopEvent, g_cruiseEvent};
    while (true) {
        DWORD waitRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (waitRes == WAIT_OBJECT_0 || waitRes == WAIT_FAILED)
            return 0;

        double cruiseAccumY = 0.0;
        double cruiseAccumX = 0.0;
        EnterCriticalSection(&g_cruiseCs);
        CruiseContext ctx = g_cruiseCtx;
        LeaveCriticalSection(&g_cruiseCs);

        while (g_state.load() == ScrollState::Cruising) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0)
                return 0;

            if (!IsCtrlPhysicallyDown()) {
                g_state.store(ScrollState::Idle);
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                break;
            }

            POINT curPt;
            GetCursorPos(&curPt);
            if (!PtInRect(&ctx.windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                break;
            }

            int distX = curPt.x - ctx.anchorPt.x;
            int distY = curPt.y - ctx.anchorPt.y;

            if (ctx.axis == DragAxis::VerticalOnly)
                distX = 0;
            if (ctx.axis == DragAxis::HorizontalOnly)
                distY = 0;

            HWND hTarget = nullptr;
            bool isDiscrete = GetScrollTargetInfo(curPt, hTarget);

            if (ctx.axis == DragAxis::VerticalOnly &&
                std::abs(distY) >= ctx.deadzone) {
                double directionY = ctx.touch_style ? 1.0 : -1.0;
                double notchRateY =
                    (static_cast<double>(distY) / ctx.pixels_per_notch);

                if (isDiscrete) {
                    cruiseAccumY += notchRateY * directionY * 0.035 * 3.0;
                    if (int lines = TakeWholeSteps(cruiseAccumY, 1)) {
                        DispatchScroll(lines, hTarget, true);
                    }
                } else {
                    double unitsThisTickY =
                        notchRateY * directionY * WHEEL_DELTA * 0.035;
                    cruiseAccumY += unitsThisTickY;
                    int step =
                        ctx.smooth_wheel ? SMOOTH_SCROLL_CHUNK : WHEEL_DELTA;
                    if (int chunks = TakeWholeSteps(cruiseAccumY, step)) {
                        DispatchScroll(chunks, hTarget, false);
                    }
                }
            } else if (ctx.axis == DragAxis::HorizontalOnly &&
                       ctx.enable_horizontal &&
                       std::abs(distX) >= ctx.deadzone) {
                double directionX = ctx.touch_style ? -1.0 : 1.0;
                double notchRateX =
                    (static_cast<double>(distX) / ctx.pixels_per_notch);

                if (isDiscrete) {
                    cruiseAccumX += notchRateX * directionX * 0.035 * 3.0;
                    if (int chars = TakeWholeSteps(cruiseAccumX, 1)) {
                        DispatchHScroll(chars, hTarget, true);
                    }
                } else {
                    double unitsThisTickX =
                        notchRateX * directionX * WHEEL_DELTA * 0.035;
                    cruiseAccumX += unitsThisTickX;
                    int step =
                        ctx.smooth_wheel ? SMOOTH_SCROLL_CHUNK : WHEEL_DELTA;
                    if (int chunks = TakeWholeSteps(cruiseAccumX, step)) {
                        DispatchHScroll(chunks, hTarget, false);
                    }
                }
            }

            if (WaitForSingleObject(g_stopEvent, 20) == WAIT_OBJECT_0)
                return 0;
        }
    }
    return 0;
}

void StartCruiseThreadIfNeeded() {
    EnterCriticalSection(&g_cruiseCs);
    g_cruiseCtx.anchorPt = g_anchorPt;
    g_cruiseCtx.windowBounds = g_windowBounds;
    g_cruiseCtx.pixels_per_notch = g_settings.pixels_per_notch;
    g_cruiseCtx.deadzone = g_settings.deadzone;
    g_cruiseCtx.touch_style = g_settings.touch_style;
    g_cruiseCtx.smooth_wheel = g_settings.smooth_wheel;
    g_cruiseCtx.enable_horizontal = g_settings.enable_horizontal;
    g_cruiseCtx.axis = g_activeAxis;
    LeaveCriticalSection(&g_cruiseCs);

    bool expected = false;
    if (g_threadCreated.compare_exchange_strong(expected, true)) {
        g_hCruiseThread =
            CreateThread(nullptr, 0, CruiseWorkerThread, nullptr, 0, nullptr);
    }
    if (g_cruiseEvent)
        SetEvent(g_cruiseEvent);
}

static void EnsureCursorsLoaded() {
    if (!g_curSizeNS) {
        g_curSizeNS = CreateCursorFromBase64(kCurVerticalBase64);
        g_curSizeWE = CreateCursorFromBase64(kCurHorizontalBase64);
        g_curCruising = CreateCursorFromBase64(kCurCruisingBase64);
        g_curHand = CreateCursorFromBase64(kCurCruisingBase64);
    }
}

void UpdateScrollCursor() {
    ScrollState st = g_state.load();
    if (st == ScrollState::Idle)
        return;

    EnsureCursorsLoaded();

    HCURSOR hTarget = nullptr;
    if (st == ScrollState::Pending) {
        hTarget = g_curHand;
    } else if (st == ScrollState::Cruising) {
        hTarget = g_curCruising;
    } else if (st == ScrollState::Scrolling) {
        hTarget = (g_activeAxis == DragAxis::HorizontalOnly) ? g_curSizeWE
                                                             : g_curSizeNS;
    }

    if (hTarget) {
        SetCursor(hTarget);
    }
}

bool ProcessScrollInput(MSG* pMsg, bool canMutate) {
    if (!pMsg || !pMsg->hwnd)
        return false;

    UINT m = pMsg->message;
    if (!(m >= WM_MOUSEFIRST && m <= WM_MOUSELAST) &&
        !(m >= WM_KEYFIRST && m <= WM_KEYLAST)) {
        return false;
    }

    ULONG_PTR extra = GetMessageExtraInfo();
    if (extra == REPLAY_EXTRA_INFO)
        return false;

    if (m == WM_LBUTTONDOWN || m == WM_LBUTTONDBLCLK || m == WM_RBUTTONDOWN ||
        m == WM_RBUTTONDBLCLK || m == WM_MBUTTONDOWN || m == WM_MBUTTONDBLCLK ||
        m == WM_XBUTTONDOWN || m == WM_XBUTTONDBLCLK) {
        g_swallowButtonUp.store(0);
    }

    if (g_swallowButtonUp.load() != 0 && m == g_swallowButtonUp.load()) {
        g_swallowButtonUp.store(0);
        return true;
    }

    if (m == WM_MOUSEMOVE && !(pMsg->wParam & MK_RBUTTON)) {
        if (g_swallowRButtonUp.load()) {
            g_swallowRButtonUp.store(false);
            if (g_captureHwnd && GetCapture() == g_captureHwnd) {
                ReleaseCapture();
            }
            g_captureHwnd = nullptr;
            g_state.store(ScrollState::Idle);
        }
    }

    if (m == WM_MOUSEWHEEL || m == WM_MOUSEHWHEEL) {
        if (extra == MASK_EXTRA_INFO || IsInScrollOrGracePeriod()) {
            if (canMutate) {
                pMsg->wParam &= ~((WPARAM)(MK_CONTROL | MK_SHIFT | MK_RBUTTON));
            }
            return false;
        }
    }

    if (m == WM_SETCURSOR && g_state.load() != ScrollState::Idle) {
        UpdateScrollCursor();
        return true;
    }

    if (m == WM_KEYDOWN || m == WM_SYSKEYDOWN) {
        if (g_state.load() == ScrollState::Cruising) {
            if (pMsg->wParam != VK_CONTROL && pMsg->wParam != VK_LCONTROL &&
                pMsg->wParam != VK_RCONTROL) {
                g_state.store(ScrollState::Idle);
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
            }
        }
    }

    if ((m == WM_KEYUP || m == WM_SYSKEYUP) && extra != MASK_EXTRA_INFO) {
        if (pMsg->wParam == VK_CONTROL) {
            if (!IsCtrlPhysicallyDown()) {
                ScrollState cur = g_state.load();
                if (cur == ScrollState::Cruising ||
                    cur == ScrollState::Scrolling) {
                    g_lastScrollEndTime.store(GetTickCount64());
                    g_state.store(ScrollState::Idle);
                    SetCursor(LoadCursor(nullptr, IDC_ARROW));
                }
            }
            return false;
        }
    }

    if (g_state.load() == ScrollState::Cruising) {
        if ((m == WM_RBUTTONDOWN || m == WM_RBUTTONDBLCLK) &&
            (pMsg->wParam & MK_CONTROL)) {
            g_state.store(ScrollState::Idle);
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        } else {
            UINT upToSwallow = 0;
            switch (m) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONDBLCLK:
                    upToSwallow = WM_LBUTTONUP;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONDBLCLK:
                    upToSwallow = WM_RBUTTONUP;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONDBLCLK:
                    upToSwallow = WM_MBUTTONUP;
                    break;
                case WM_XBUTTONDOWN:
                case WM_XBUTTONDBLCLK:
                    upToSwallow = WM_XBUTTONUP;
                    break;
            }
            if (upToSwallow != 0) {
                g_swallowButtonUp.store(upToSwallow);
                g_state.store(ScrollState::Idle);
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                return true;
            }
        }
    }

    if (m == WM_RBUTTONDOWN || m == WM_RBUTTONDBLCLK) {
        if (pMsg->wParam & MK_CONTROL) {
            g_state.store(ScrollState::Pending);
            UpdateScrollCursor();
            g_swallowRButtonUp.store(true);
            g_captureHwnd = pMsg->hwnd;
            SetCapture(g_captureHwnd);

            GetCursorPos(&g_anchorPt);
            g_lastPt = g_anchorPt;
            g_accumY = 0.0;
            g_accumX = 0.0;
            g_smoothDx = 0.0;
            g_smoothDy = 0.0;
            g_activeAxis = DragAxis::VerticalOnly;

            HWND hTargetWnd = GetAncestor(pMsg->hwnd, GA_ROOT);
            if (!hTargetWnd)
                hTargetWnd = pMsg->hwnd;

            GetWindowRect(hTargetWnd, &g_windowBounds);
            LONG_PTR exStyle = GetWindowLongPtrW(hTargetWnd, GWL_EXSTYLE);
            if (!(exStyle & WS_EX_NOACTIVATE) &&
                GetForegroundWindow() != hTargetWnd) {
                SetForegroundWindow(hTargetWnd);
            }
            return true;
        }
    }

    if (m == WM_MOUSEMOVE) {
        ScrollState cur = g_state.load();
        if (cur == ScrollState::Pending || cur == ScrollState::Scrolling) {
            POINT curPt;
            GetCursorPos(&curPt);
            if (!PtInRect(&g_windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                return false;
            }

            int totalDx = curPt.x - g_anchorPt.x;
            int totalDy = curPt.y - g_anchorPt.y;

            bool deadzoneExceeded = false;
            if (g_settings.enable_horizontal) {
                deadzoneExceeded = (totalDx * totalDx + totalDy * totalDy) >=
                                   (g_settings.deadzone * g_settings.deadzone);
            } else {
                deadzoneExceeded = std::abs(totalDy) >= g_settings.deadzone;
            }

            if (cur == ScrollState::Pending && deadzoneExceeded) {
                g_state.store(ScrollState::Scrolling);
                g_lastPt = curPt;

                if (g_settings.enable_horizontal) {
                    int absX = std::abs(totalDx);
                    int absY = std::abs(totalDy);
                    g_activeAxis = (absY >= absX) ? DragAxis::VerticalOnly
                                                  : DragAxis::HorizontalOnly;
                } else {
                    g_activeAxis = DragAxis::VerticalOnly;
                }
                UpdateScrollCursor();
            }

            if (g_state.load() == ScrollState::Scrolling) {
                UpdateScrollCursor();

                int rawDx = curPt.x - g_lastPt.x;
                int rawDy = curPt.y - g_lastPt.y;
                g_lastPt = curPt;

                g_smoothDx = (g_smoothDx * 0.70) + (rawDx * 0.30);
                g_smoothDy = (g_smoothDy * 0.70) + (rawDy * 0.30);

                double dx = (g_activeAxis == DragAxis::HorizontalOnly)
                                ? static_cast<double>(rawDx)
                                : 0.0;
                double dy = (g_activeAxis == DragAxis::VerticalOnly)
                                ? static_cast<double>(rawDy)
                                : 0.0;

                HWND hTarget = nullptr;
                bool isDiscrete = GetScrollTargetInfo(curPt, hTarget);

                double velocityFactor = 1.0;
                if (g_settings.enable_velocity) {
                    double instantDist = std::hypot(rawDx, rawDy);
                    double smoothDist = std::hypot(g_smoothDx, g_smoothDy);
                    double speed = (instantDist * 0.7) + (smoothDist * 0.3);
                    velocityFactor =
                        std::clamp(0.85 + (speed * 0.08), 0.85, 3.5);
                }

                if (isDiscrete && hTarget) {
                    if (g_activeAxis == DragAxis::VerticalOnly && dy != 0.0) {
                        double dirY = g_settings.touch_style ? 1.0 : -1.0;
                        g_accumY += (dy * dirY * velocityFactor);
                        int notchStep =
                            static_cast<int>(g_settings.pixels_per_notch);
                        if (int lines = TakeWholeSteps(g_accumY, notchStep)) {
                            DispatchScroll(lines, hTarget, true);
                        }
                    } else if (g_activeAxis == DragAxis::HorizontalOnly &&
                               dx != 0.0 && g_settings.enable_horizontal) {
                        double dirX = g_settings.touch_style ? -1.0 : 1.0;
                        g_accumX += (dx * dirX * velocityFactor);
                        int notchStep =
                            static_cast<int>(g_settings.pixels_per_notch);
                        if (int chars = TakeWholeSteps(g_accumX, notchStep)) {
                            DispatchHScroll(chars, hTarget, true);
                        }
                    }
                } else {
                    double wheelPerPixel = static_cast<double>(WHEEL_DELTA) /
                                           g_settings.pixels_per_notch;
                    int stepUnits = g_settings.smooth_wheel
                                        ? SMOOTH_SCROLL_CHUNK
                                        : WHEEL_DELTA;

                    if (g_activeAxis == DragAxis::VerticalOnly && dy != 0.0) {
                        double dirY = g_settings.touch_style ? 1.0 : -1.0;
                        g_accumY +=
                            (dy * dirY * wheelPerPixel * velocityFactor);

                        if (int notchesY =
                                TakeWholeSteps(g_accumY, stepUnits)) {
                            DispatchScroll(notchesY, nullptr, false);
                        }
                    } else if (g_activeAxis == DragAxis::HorizontalOnly &&
                               dx != 0.0 && g_settings.enable_horizontal) {
                        double dirX = g_settings.touch_style ? -1.0 : 1.0;
                        g_accumX +=
                            (dx * dirX * wheelPerPixel * velocityFactor);

                        if (int notchesX =
                                TakeWholeSteps(g_accumX, stepUnits)) {
                            DispatchHScroll(notchesX, nullptr, false);
                        }
                    }
                }

                return true;
            }
        }
    }

    if (m == WM_RBUTTONUP && g_swallowRButtonUp.load()) {
        g_swallowRButtonUp.store(false);
        if (g_captureHwnd && GetCapture() == g_captureHwnd) {
            ReleaseCapture();
        }

        if (g_captureHwnd) {
            SendScrollMessage(g_captureHwnd, WM_VSCROLL, SB_ENDSCROLL);
            SendScrollMessage(g_captureHwnd, WM_HSCROLL, SB_ENDSCROLL);
        }
        g_captureHwnd = nullptr;

        ScrollState cur = g_state.load();
        if (cur == ScrollState::Pending) {
            g_state.store(ScrollState::Idle);
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_MOUSE;
            inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            inputs[0].mi.dwExtraInfo = REPLAY_EXTRA_INFO;
            inputs[1].type = INPUT_MOUSE;
            inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            inputs[1].mi.dwExtraInfo = REPLAY_EXTRA_INFO;
            SendInput(2, inputs, sizeof(INPUT));
        } else if (cur == ScrollState::Scrolling &&
                   g_settings.enable_cruising && IsCtrlPhysicallyDown()) {
            g_state.store(ScrollState::Cruising);
            UpdateScrollCursor();
            StartCruiseThreadIfNeeded();
        } else {
            if (cur == ScrollState::Scrolling) {
                g_lastScrollEndTime.store(GetTickCount64());
            }
            g_state.store(ScrollState::Idle);
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }
        return true;
    }
    return false;
}

// --- SAFE DISPATCH HOOKS ---
using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageW_t pfnDispatchMessageW_Original = nullptr;

LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (lpMsg && ProcessScrollInput(const_cast<MSG*>(lpMsg), true)) {
        return 0;  // Handled and swallowed
    }
    return pfnDispatchMessageW_Original(lpMsg);
}

using DispatchMessageA_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageA_t pfnDispatchMessageA_Original = nullptr;

LRESULT WINAPI DispatchMessageA_Hook(const MSG* lpMsg) {
    if (lpMsg && ProcessScrollInput(const_cast<MSG*>(lpMsg), true)) {
        return 0;  // Handled and swallowed
    }
    return pfnDispatchMessageA_Original(lpMsg);
}

using GetKeyState_t = SHORT(WINAPI*)(int);
GetKeyState_t pfnGetKeyState_Original = nullptr;
SHORT WINAPI GetKeyState_Hook(int nVirtKey) {
    if (IsInScrollOrGracePeriod() &&
        (nVirtKey == VK_CONTROL || nVirtKey == VK_LCONTROL ||
         nVirtKey == VK_RCONTROL)) {
        return 0;
    }
    return pfnGetKeyState_Original(nVirtKey);
}

SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
    if (IsInScrollOrGracePeriod() &&
        (vKey == VK_CONTROL || vKey == VK_LCONTROL || vKey == VK_RCONTROL)) {
        return 0;
    }
    return pfnGetAsyncKeyState_Original(vKey);
}

using GetKeyboardState_t = BOOL(WINAPI*)(PBYTE);
GetKeyboardState_t pfnGetKeyboardState_Original = nullptr;
BOOL WINAPI GetKeyboardState_Hook(PBYTE lpKeyState) {
    BOOL ret = pfnGetKeyboardState_Original(lpKeyState);
    if (ret && IsInScrollOrGracePeriod()) {
        lpKeyState[VK_CONTROL] &= 0x7F;
        lpKeyState[VK_LCONTROL] &= 0x7F;
        lpKeyState[VK_RCONTROL] &= 0x7F;
    }
    return ret;
}

using SetCursor_t = HCURSOR(WINAPI*)(HCURSOR);
static SetCursor_t pfnSetCursor_Original = nullptr;

HCURSOR WINAPI SetCursor_Hook(HCURSOR hCursor) {
    ScrollState st = g_state.load();
    if (st != ScrollState::Idle) {
        HCURSOR hTarget = nullptr;
        if (st == ScrollState::Pending) {
            hTarget = g_curHand;
        } else if (st == ScrollState::Cruising) {
            hTarget = g_curCruising;
        } else if (st == ScrollState::Scrolling) {
            hTarget = (g_activeAxis == DragAxis::HorizontalOnly) ? g_curSizeWE
                                                                 : g_curSizeNS;
        }

        if (hTarget) {
            return pfnSetCursor_Original(hTarget);
        }
    }
    return pfnSetCursor_Original(hCursor);
}

// --- WINDHAWK LIFECYCLE ---
BOOL Wh_ModInit() {
    HWINSTA hWinSta = GetProcessWindowStation();
    if (hWinSta) {
        USEROBJECTFLAGS uof = {};
        if (GetUserObjectInformationW(hWinSta, UOI_FLAGS, &uof, sizeof(uof),
                                      nullptr)) {
            if (!(uof.dwFlags & WSF_VISIBLE))
                return FALSE;
        }
    }

    LPCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && (wcsstr(cmdLine, L"--type=") != nullptr ||
                    wcsstr(cmdLine, L"-Embedding") != nullptr)) {
        return FALSE;
    }

    Wh_Log(L"Initializing Drag to Scroll (Locked Axis)");

    LoadSettings();

    InitializeCriticalSection(&g_cruiseCs);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_cruiseEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_threadCreated.store(false);

    // DispatchMessage hooks: safe from idle GetMessage blocking
    WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageW_Hook,
                                   &pfnDispatchMessageW_Original);
    WindhawkUtils::SetFunctionHook(DispatchMessageA, DispatchMessageA_Hook,
                                   &pfnDispatchMessageA_Original);

    // Synchronous modifier and cursor hooks
    WindhawkUtils::SetFunctionHook(GetKeyState, GetKeyState_Hook,
                                   &pfnGetKeyState_Original);
    WindhawkUtils::SetFunctionHook(GetAsyncKeyState, GetAsyncKeyState_Hook,
                                   &pfnGetAsyncKeyState_Original);
    WindhawkUtils::SetFunctionHook(GetKeyboardState, GetKeyboardState_Hook,
                                   &pfnGetKeyboardState_Original);
    WindhawkUtils::SetFunctionHook(SetCursor, SetCursor_Hook,
                                   &pfnSetCursor_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading Drag to Scroll mod");

    g_state.store(ScrollState::Idle);

    if (g_stopEvent)
        SetEvent(g_stopEvent);
    if (g_cruiseEvent)
        SetEvent(g_cruiseEvent);

    if (g_hCruiseThread) {
        WaitForSingleObject(g_hCruiseThread, 200);
        CloseHandle(g_hCruiseThread);
        g_hCruiseThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_cruiseEvent) {
        CloseHandle(g_cruiseEvent);
        g_cruiseEvent = nullptr;
    }

    DeleteCriticalSection(&g_cruiseCs);
    g_threadCreated.store(false);

    if (g_curSizeNS) {
        DestroyCursor(g_curSizeNS);
        g_curSizeNS = nullptr;
    }
    if (g_curSizeWE) {
        DestroyCursor(g_curSizeWE);
        g_curSizeWE = nullptr;
    }
    if (g_curCruising) {
        DestroyCursor(g_curCruising);
        g_curCruising = nullptr;
    }
    if (g_curHand) {
        DestroyCursor(g_curHand);
        g_curHand = nullptr;
    }
}
