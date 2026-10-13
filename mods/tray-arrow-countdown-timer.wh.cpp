// ==WindhawkMod==
// @id              tray-arrow-countdown-timer
// @name            Tray Arrow Countdown Timer
// @description     A countdown timer on the Windows 11 taskbar, next to the "Show hidden icons" arrow, with a sound alert
// @version         1.0
// @author          paran01d-qq
// @github          https://github.com/paran01d-qq
// @include         explorer.exe
// @compilerOptions -lgdi32 -lshell32 -lwinmm -lgdiplus -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray Arrow Countdown Timer

Adds a countdown timer (HH:MM:SS) to the Windows 11 taskbar, right to the left
of the "Show hidden icons" arrow (^). When the time is up, the timer flashes red
and plays a sound.

## Controls
- **▶ / ⏸** — start / pause (after the alarm: reset)
- **▲ / ▼** above and below the digits — change hours, minutes or seconds
  (hold the button to scroll quickly)
- **Mouse wheel** over hours / minutes / seconds — change that field
- **Right click** — menu with preset intervals
- **Middle click** — reset

Each field wraps around independently (59 → 00), like on an alarm clock.
The maximum time is 99:59:59.

The timer is hidden while a fullscreen app (video, game, presentation) is
active on the same monitor. The countdown and the alarm sound keep working.

## Troubleshooting
If the timer is not placed next to the arrow, enable **Debug logging**, find the
line describing the arrow in the mod log and enter its Name or ID in the
**Arrow name/ID** setting. If the arrow is hidden (all tray icons are shown),
the timer is placed at a fixed offset from the right edge of the taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultMinutes: 5
  $name: Default time (minutes)
- soundFile: C:\Windows\Media\Alarm01.wav
  $name: Sound file (.wav)
  $description: Leave empty to use the system sound
- repeatCount: 3
  $name: Number of times to play the alarm
- repeatIntervalSec: 5
  $name: Interval between alarm repeats (seconds)
- gap: 2
  $name: Gap from the arrow (px)
- fallbackOffset: 160
  $name: Offset from the right edge of the taskbar if the arrow is not found (px)
- chevronName: ""
  $name: Arrow name/ID
  $description: Fill in only if the arrow is not detected automatically
- hideInFullscreen: true
  $name: Hide when a fullscreen app is active
- debugLog: false
  $name: Debug logging
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <objbase.h>
#include <uiautomation.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    int defaultMinutes = 5;
    std::wstring soundFile;
    int repeatCount = 3;
    int repeatIntervalSec = 5;
    int gap = 2;
    int fallbackOffset = 160;
    std::wstring chevronName;
    bool hideInFullscreen = true;
    bool debugLog = false;
};

static Settings g_settings;
static std::mutex g_settingsMutex;

static Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

static std::wstring ReadStringSetting(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static void LoadSettings() {
    Settings s;
    s.defaultMinutes = std::clamp(Wh_GetIntSetting(L"defaultMinutes"), 1, 5999);
    s.soundFile = ReadStringSetting(L"soundFile");
    s.repeatCount = std::clamp(Wh_GetIntSetting(L"repeatCount"), 1, 100);
    s.repeatIntervalSec = std::clamp(Wh_GetIntSetting(L"repeatIntervalSec"), 1, 60);
    s.gap = std::clamp(Wh_GetIntSetting(L"gap"), 0, 100);
    s.fallbackOffset = std::clamp(Wh_GetIntSetting(L"fallbackOffset"), 0, 2000);
    s.chevronName = ReadStringSetting(L"chevronName");
    s.hideInFullscreen = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    s.debugLog = Wh_GetIntSetting(L"debugLog") != 0;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = s;
}

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

constexpr UINT WM_APP_REPOSITION = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS   = WM_APP + 2;
constexpr UINT WM_APP_EXIT       = WM_APP + 3;

constexpr UINT_PTR TIMER_TICK   = 1;
constexpr UINT_PTR TIMER_ALARM  = 2;
constexpr UINT_PTR TIMER_REPEAT = 3;

constexpr LONGLONG kMinute = 60 * 1000LL;
constexpr LONGLONG kMaxSec = 99 * 3600 + 59 * 60 + 59;  // 99:59:59

// Element sizes at 100% scale (96 DPI)
constexpr float kPad96    = 2;
constexpr float kPlayW96  = 24;
constexpr float kGap96    = 2;
constexpr float kSegW96   = 20;
constexpr float kColonW96 = 5;
constexpr int kWidth96 =
    (int)(kPad96 + kPlayW96 + kGap96 + kSegW96 * 3 + kColonW96 * 2 + kPad96);

constexpr wchar_t kWindowClass[] = L"WhTaskbarCountdownTimer";

static HANDLE g_stopEvent = nullptr;
static HANDLE g_uiThread = nullptr;
static std::atomic<HWND> g_hwnd{nullptr};
static std::atomic<bool> g_forceSearch{false};
static HINSTANCE g_hInstance = nullptr;

// Position computed by the positioning thread
static std::mutex g_posMutex;
static RECT g_targetRect{};
static UINT g_targetDpi = 96;
static bool g_targetValid = false;

// Timer state (UI thread only)
enum class State { Idle, Running, Paused, Finished };
static State g_state = State::Idle;
static LONGLONG g_durationMs = 5 * kMinute;
static LONGLONG g_remainingMs = 5 * kMinute;
static ULONGLONG g_endTick = 0;
static int g_alarmPlayed = 0;
static bool g_hover = false;
static bool g_lightTheme = false;
static bool g_visible = false;
static int g_tickCount = 0;

// Hit-test regions
enum Hit {
    HitNone = -1,
    HitPlay = 0,
    HitUp0, HitUp1, HitUp2,
    HitDown0, HitDown1, HitDown2,
    HitSeg0, HitSeg1, HitSeg2,
    HitBackground,
};
static int g_hoverHit = HitNone;
static int g_repeatHit = HitNone;

// UI Automation GUIDs
static const CLSID kCLSID_CUIAutomation = {
    0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
static const IID kIID_IUIAutomation = {
    0x30cbe57d, 0xd9d0, 0x452a, {0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee}};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static HINSTANCE GetSelfModule() {
    HMODULE m = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetSelfModule, &m);
    return m;
}

static bool ReadLightTheme() {
    DWORD v = 0, size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v,
                     &size) == ERROR_SUCCESS) {
        return v != 0;
    }
    return false;
}

static std::wstring BstrToString(BSTR b) {
    std::wstring s = b ? std::wstring(b, SysStringLen(b)) : L"";
    if (b) SysFreeString(b);
    return s;
}

static bool ContainsI(std::wstring hay, std::wstring needle) {
    if (hay.empty() || needle.empty()) return false;
    CharLowerBuffW(hay.data(), (DWORD)hay.size());
    CharLowerBuffW(needle.data(), (DWORD)needle.size());
    return hay.find(needle) != std::wstring::npos;
}

static bool GetGeometry(RECT* r, UINT* dpi) {
    std::lock_guard<std::mutex> lock(g_posMutex);
    if (!g_targetValid) return false;
    *r = g_targetRect;
    *dpi = g_targetDpi;
    return r->right > r->left && r->bottom > r->top;
}

// ---------------------------------------------------------------------------
// Locating the "Show hidden icons" arrow via UI Automation
// ---------------------------------------------------------------------------

static IUIAutomationElement* FindChevron(IUIAutomation* uia, HWND taskbar,
                                         const Settings& s) {
    IUIAutomationElement* root = nullptr;
    if (FAILED(uia->ElementFromHandle(taskbar, &root)) || !root) return nullptr;

    IUIAutomationCondition* cond = nullptr;
    uia->CreateTrueCondition(&cond);

    IUIAutomationElementArray* arr = nullptr;
    if (cond) root->FindAll(TreeScope_Descendants, cond, &arr);

    IUIAutomationElement* found = nullptr;
    if (arr) {
        int len = 0;
        arr->get_Length(&len);
        for (int i = 0; i < len && !found; i++) {
            IUIAutomationElement* el = nullptr;
            if (FAILED(arr->GetElement(i, &el)) || !el) continue;

            BSTR b = nullptr;
            el->get_CurrentClassName(&b);
            std::wstring cls = BstrToString(b);
            b = nullptr;
            el->get_CurrentAutomationId(&b);
            std::wstring aid = BstrToString(b);
            b = nullptr;
            el->get_CurrentName(&b);
            std::wstring name = BstrToString(b);

            if (s.debugLog) {
                Wh_Log(L"[%d] class='%s' id='%s' name='%s'", i, cls.c_str(),
                       aid.c_str(), name.c_str());
            }

            bool match;
            if (!s.chevronName.empty()) {
                PCWSTR n = s.chevronName.c_str();
                match = _wcsicmp(name.c_str(), n) == 0 ||
                        _wcsicmp(aid.c_str(), n) == 0 ||
                        _wcsicmp(cls.c_str(), n) == 0;
            } else {
                match = ContainsI(cls, L"chevron") || ContainsI(aid, L"chevron") ||
                        ContainsI(name, L"hidden icons") ||
                        ContainsI(name, L"скрытые значки");  // Russian UI
            }

            if (match) {
                RECT r{};
                if (SUCCEEDED(el->get_CurrentBoundingRectangle(&r)) &&
                    r.right > r.left && r.bottom > r.top) {
                    found = el;
                    if (s.debugLog) Wh_Log(L"Arrow found: [%d]", i);
                    continue;
                }
            }
            el->Release();
        }
        arr->Release();
    }

    if (cond) cond->Release();
    root->Release();
    return found;
}

static DWORD WINAPI PositionThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IUIAutomation* uia = nullptr;
    CoCreateInstance(kCLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                     kIID_IUIAutomation, (void**)&uia);
    if (!uia) Wh_Log(L"Failed to create IUIAutomation");

    IUIAutomationElement* chevron = nullptr;
    ULONGLONG lastSearch = 0;
    RECT lastRect{};
    bool lastValid = false;

    while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT) {
        if (g_forceSearch.exchange(false)) {
            if (chevron) { chevron->Release(); chevron = nullptr; }
            lastSearch = 0;
            lastValid = false;
        }

        HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!taskbar) {
            if (chevron) { chevron->Release(); chevron = nullptr; }
            continue;
        }

        Settings s = GetSettings();

        RECT cr{};
        bool haveChevron = false;
        if (chevron) {
            if (SUCCEEDED(chevron->get_CurrentBoundingRectangle(&cr)) &&
                cr.right > cr.left && cr.bottom > cr.top) {
                haveChevron = true;
            } else {
                chevron->Release();
                chevron = nullptr;
            }
        }

        if (!chevron && uia && GetTickCount64() - lastSearch > 3000) {
            lastSearch = GetTickCount64();
            chevron = FindChevron(uia, taskbar, s);
            if (chevron && SUCCEEDED(chevron->get_CurrentBoundingRectangle(&cr)) &&
                cr.right > cr.left && cr.bottom > cr.top) {
                haveChevron = true;
            }
        }

        UINT dpi = GetDpiForWindow(taskbar);
        if (!dpi) dpi = 96;
        int w = MulDiv(kWidth96, dpi, 96);

        RECT target{};
        if (haveChevron) {
            target.right = cr.left - MulDiv(s.gap, dpi, 96);
            target.left = target.right - w;
            target.top = cr.top;
            target.bottom = cr.bottom;
        } else {
            RECT tr{};
            GetWindowRect(taskbar, &tr);
            int pad = MulDiv(4, dpi, 96);
            target.right = tr.right - MulDiv(s.fallbackOffset, dpi, 96);
            target.left = target.right - w;
            target.top = tr.top + pad;
            target.bottom = tr.bottom - pad;
        }

        if (!lastValid || !EqualRect(&target, &lastRect)) {
            {
                std::lock_guard<std::mutex> lock(g_posMutex);
                g_targetRect = target;
                g_targetDpi = dpi;
                g_targetValid = true;
            }
            lastRect = target;
            lastValid = true;
            HWND h = g_hwnd.load();
            if (h) PostMessageW(h, WM_APP_REPOSITION, 0, 0);
        }
    }

    if (chevron) chevron->Release();
    if (uia) uia->Release();
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------

struct Layout {
    Gdiplus::RectF play;
    Gdiplus::RectF seg[3];
    Gdiplus::RectF up[3];        // ▲ click area (top 40%)
    Gdiplus::RectF down[3];      // ▼ click area (bottom 40%)
    Gdiplus::RectF upDraw[3];    // where ▲ is drawn
    Gdiplus::RectF downDraw[3];  // where ▼ is drawn
    Gdiplus::RectF colon[2];
};

static Layout ComputeLayout(int w, int h, UINT dpi) {
    float s = dpi / 96.0f;
    float H = (float)h;
    float hitH = H * 0.40f;
    float drawH = H * 0.26f;
    Layout L;

    float x = kPad96 * s;
    L.play = Gdiplus::RectF(x, 0, kPlayW96 * s, H);
    x += (kPlayW96 + kGap96) * s;

    for (int i = 0; i < 3; i++) {
        float segW = kSegW96 * s;
        L.seg[i] = Gdiplus::RectF(x, 0, segW, H);
        L.up[i] = Gdiplus::RectF(x, 0, segW, hitH);
        L.down[i] = Gdiplus::RectF(x, H - hitH, segW, hitH);
        L.upDraw[i] = Gdiplus::RectF(x, 0, segW, drawH);
        L.downDraw[i] = Gdiplus::RectF(x, H - drawH, segW, drawH);
        x += segW;
        if (i < 2) {
            L.colon[i] = Gdiplus::RectF(x, 0, kColonW96 * s, H);
            x += kColonW96 * s;
        }
    }
    return L;
}

static int HitTest(int x, int y) {
    RECT r;
    UINT dpi;
    if (!GetGeometry(&r, &dpi)) return HitNone;
    Layout L = ComputeLayout(r.right - r.left, r.bottom - r.top, dpi);
    Gdiplus::PointF p((Gdiplus::REAL)x, (Gdiplus::REAL)y);

    for (int i = 0; i < 3; i++) {
        if (L.up[i].Contains(p)) return HitUp0 + i;
        if (L.down[i].Contains(p)) return HitDown0 + i;
        if (L.seg[i].Contains(p)) return HitSeg0 + i;
    }
    if (L.play.Contains(p)) return HitPlay;
    return HitBackground;
}

// ---------------------------------------------------------------------------
// Timer logic
// ---------------------------------------------------------------------------

static LONGLONG RemainingMs() {
    switch (g_state) {
        case State::Running: {
            LONGLONG r = (LONGLONG)g_endTick - (LONGLONG)GetTickCount64();
            return r > 0 ? r : 0;
        }
        case State::Finished:
            return 0;
        default:
            return g_remainingMs;
    }
}

static void SplitTime(LONGLONG ms, int f[3]) {
    LONGLONG sec = (ms + 999) / 1000;
    f[0] = (int)(sec / 3600);
    f[1] = (int)((sec % 3600) / 60);
    f[2] = (int)(sec % 60);
}

static void PlayAlarm() {
    Settings s = GetSettings();
    BOOL ok = FALSE;
    if (!s.soundFile.empty() &&
        GetFileAttributesW(s.soundFile.c_str()) != INVALID_FILE_ATTRIBUTES) {
        ok = PlaySoundW(s.soundFile.c_str(), nullptr,
                        SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
    if (!ok) PlaySoundW(L"SystemExclamation", nullptr, SND_ALIAS | SND_ASYNC);
}

static void StopAlarm(HWND hwnd) {
    KillTimer(hwnd, TIMER_ALARM);
    PlaySoundW(nullptr, nullptr, 0);
}

static void StartTimer() {
    if (g_state == State::Idle && g_remainingMs <= 0) {
        g_durationMs = GetSettings().defaultMinutes * kMinute;
        g_remainingMs = g_durationMs;
    }
    if ((g_state == State::Idle || g_state == State::Paused) && g_remainingMs > 0) {
        g_endTick = GetTickCount64() + g_remainingMs;
        g_state = State::Running;
    }
}

static void PauseTimer() {
    if (g_state == State::Running) {
        g_remainingMs = RemainingMs();
        g_state = State::Paused;
    }
}

static void ResetTimer(HWND hwnd) {
    StopAlarm(hwnd);
    g_state = State::Idle;
    g_remainingMs = g_durationMs;
}

static void ToggleTimer(HWND hwnd) {
    if (g_state == State::Running) PauseTimer();
    else if (g_state == State::Finished) ResetTimer(hwnd);
    else StartTimer();
}

static void FinishTimer(HWND hwnd) {
    g_state = State::Finished;
    g_alarmPlayed = 1;
    PlayAlarm();
    Settings s = GetSettings();
    if (s.repeatCount > 1) SetTimer(hwnd, TIMER_ALARM, s.repeatIntervalSec * 1000, nullptr);
}

// field: 0 = hours, 1 = minutes, 2 = seconds; dir: +1 / -1
// Each field wraps around independently (59 -> 00), like an alarm clock.
static void AdjustField(int field, int dir) {
    if (g_state == State::Finished) return;

    LONGLONG remMs = RemainingMs();
    int f[3];
    SplitTime(remMs, f);
    static const int kMod[3] = {100, 60, 60};
    f[field] = (f[field] + dir + kMod[field]) % kMod[field];

    LONGLONG newSec = std::clamp<LONGLONG>(
        (LONGLONG)f[0] * 3600 + f[1] * 60 + f[2], 0, kMaxSec);
    LONGLONG newMs = newSec * 1000;

    switch (g_state) {
        case State::Idle:
            g_durationMs = newMs;
            g_remainingMs = newMs;
            break;
        case State::Paused:
            g_remainingMs = newMs;
            break;
        case State::Running:
            if (newMs <= 0) newMs = 1000;
            g_endTick = GetTickCount64() + newMs;
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

static void AddRoundRect(Gdiplus::GraphicsPath& p, const Gdiplus::RectF& rc,
                         float r) {
    float d = (std::min)(r * 2, (std::min)(rc.Width, rc.Height));
    float x = rc.X, y = rc.Y, w = rc.Width, h = rc.Height;
    p.AddArc(x, y, d, d, 180, 90);
    p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}

static void FillRound(Gdiplus::Graphics& g, Gdiplus::RectF rc, float inset,
                      float radius, const Gdiplus::Color& c) {
    rc.Inflate(-inset, -inset);
    if (rc.Width <= 0 || rc.Height <= 0) return;
    Gdiplus::GraphicsPath path;
    AddRoundRect(path, rc, radius);
    Gdiplus::SolidBrush b(c);
    g.FillPath(&b, &path);
}

static void DrawArrow(Gdiplus::Graphics& g, const Gdiplus::RectF& rc, bool up,
                      float s, const Gdiplus::Color& c) {
    float cx = rc.X + rc.Width / 2;
    float cy = rc.Y + rc.Height / 2;
    float aw = 7.0f * s;
    float ah = (std::min)(3.5f * s, rc.Height * 0.6f);
    Gdiplus::PointF pts[3];
    if (up) {
        pts[0] = Gdiplus::PointF(cx - aw / 2, cy + ah / 2);
        pts[1] = Gdiplus::PointF(cx + aw / 2, cy + ah / 2);
        pts[2] = Gdiplus::PointF(cx, cy - ah / 2);
    } else {
        pts[0] = Gdiplus::PointF(cx - aw / 2, cy - ah / 2);
        pts[1] = Gdiplus::PointF(cx + aw / 2, cy - ah / 2);
        pts[2] = Gdiplus::PointF(cx, cy + ah / 2);
    }
    Gdiplus::SolidBrush b(c);
    g.FillPolygon(&b, pts, 3);
}

static void DrawPlayIcon(Gdiplus::Graphics& g, const Gdiplus::RectF& rc, float s,
                         const Gdiplus::Color& c) {
    float cx = rc.X + rc.Width / 2;
    float cy = rc.Y + rc.Height / 2;
    float sz = (std::min)(10.0f * s, rc.Height * 0.4f);
    Gdiplus::SolidBrush b(c);

    if (g_state == State::Running) {
        // Pause: two bars
        float bw = sz * 0.32f, gap = sz * 0.3f;
        g.FillRectangle(&b, cx - gap / 2 - bw, cy - sz / 2, bw, sz);
        g.FillRectangle(&b, cx + gap / 2, cy - sz / 2, bw, sz);
    } else if (g_state == State::Finished) {
        // Stop: square (reset)
        float q = sz * 0.85f;
        g.FillRectangle(&b, cx - q / 2, cy - q / 2, q, q);
    } else {
        // Play: triangle
        Gdiplus::PointF pts[3] = {
            Gdiplus::PointF(cx - sz * 0.38f, cy - sz / 2),
            Gdiplus::PointF(cx - sz * 0.38f, cy + sz / 2),
            Gdiplus::PointF(cx + sz * 0.5f, cy),
        };
        g.FillPolygon(&b, pts, 3);
    }
}

static void Render(HWND hwnd) {
    RECT r;
    UINT dpi;
    if (!GetGeometry(&r, &dpi)) return;
    int w = r.right - r.left, h = r.bottom - r.top;
    float s = dpi / 96.0f;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        DeleteDC(mem);
        ReleaseDC(nullptr, screen);
        return;
    }
    HGDIOBJ oldBmp = SelectObject(mem, bmp);

    {
        Gdiplus::Bitmap canvas(w, h, w * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Gdiplus::Graphics g(&canvas);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        g.Clear(Gdiplus::Color(1, 0, 0, 0));  // nearly transparent, but still receives clicks

        Layout L = ComputeLayout(w, h, dpi);
        Gdiplus::RectF all(0, 0, (Gdiplus::REAL)w, (Gdiplus::REAL)h);
        float radius = 4.0f * s;

        bool flashOn = g_state == State::Finished && (GetTickCount64() / 500) % 2 == 0;

        // Widget background
        if (flashOn) {
            FillRound(g, all, 0.5f, radius, Gdiplus::Color(200, 232, 17, 35));
        } else if (g_hover) {
            FillRound(g, all, 0.5f, radius,
                      g_lightTheme ? Gdiplus::Color(18, 0, 0, 0)
                                   : Gdiplus::Color(18, 255, 255, 255));
        }

        // Highlight the button under the cursor
        Gdiplus::Color hl = g_lightTheme ? Gdiplus::Color(30, 0, 0, 0)
                                         : Gdiplus::Color(35, 255, 255, 255);
        int activeHit = g_repeatHit != HitNone ? g_repeatHit : g_hoverHit;
        if (!flashOn) {
            if (activeHit == HitPlay) FillRound(g, L.play, 2.0f * s, radius, hl);
            for (int i = 0; i < 3; i++) {
                if (activeHit == HitUp0 + i) FillRound(g, L.upDraw[i], 0.5f * s, 2.0f * s, hl);
                if (activeHit == HitDown0 + i) FillRound(g, L.downDraw[i], 0.5f * s, 2.0f * s, hl);
            }
        }

        // Colors
        auto themed = [&](BYTE a) {
            return g_lightTheme ? Gdiplus::Color(a, 0, 0, 0)
                                : Gdiplus::Color(a, 255, 255, 255);
        };
        Gdiplus::Color digitColor;
        if (g_state == State::Finished) {
            digitColor = flashOn ? Gdiplus::Color(255, 255, 255, 255)
                                 : Gdiplus::Color(255, 232, 17, 35);
        } else {
            digitColor = themed(g_state == State::Running ? 255 : 170);
        }

        // Play/pause button
        DrawPlayIcon(g, L.play, s,
                     flashOn ? Gdiplus::Color(255, 255, 255, 255) : themed(230));

        // Arrows
        for (int i = 0; i < 3; i++) {
            BYTE aUp = activeHit == HitUp0 + i ? 255 : (g_hover ? 190 : 110);
            BYTE aDown = activeHit == HitDown0 + i ? 255 : (g_hover ? 190 : 110);
            if (g_state == State::Finished) aUp = aDown = 60;
            DrawArrow(g, L.upDraw[i], true, s, themed(aUp));
            DrawArrow(g, L.downDraw[i], false, s, themed(aDown));
        }

        // Digits HH:MM:SS
        int f[3];
        SplitTime(RemainingMs(), f);

        float fontPx = (std::min)(14.0f * s, (float)h * 0.42f);
        Gdiplus::Font font(L"Segoe UI", fontPx, Gdiplus::FontStyleBold,
                           Gdiplus::UnitPixel);
        Gdiplus::StringFormat sf;
        sf.SetAlignment(Gdiplus::StringAlignmentCenter);
        sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        sf.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap |
                          Gdiplus::StringFormatFlagsNoClip);
        Gdiplus::SolidBrush textBrush(digitColor);

        for (int i = 0; i < 3; i++) {
            int v = f[i] < 0 ? 0 : (f[i] > 99 ? 99 : f[i]);
            wchar_t buf[3] = {(wchar_t)(L'0' + v / 10), (wchar_t)(L'0' + v % 10), 0};
            g.DrawString(buf, 2, &font, L.seg[i], &sf, &textBrush);
            if (i < 2) g.DrawString(L":", -1, &font, L.colon[i], &sf, &textBrush);
        }
    }

    POINT dst{r.left, r.top};
    SIZE size{w, h};
    POINT src{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, screen, &dst, &size, mem, &src, 0, &bf, ULW_ALPHA);

    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

// ---------------------------------------------------------------------------
// Visibility (fullscreen detection, z-order)
// ---------------------------------------------------------------------------

static bool IsShellWindowClass(HWND hwnd) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    return wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
           wcscmp(cls, L"Shell_TrayWnd") == 0 ||
           wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
           wcscmp(cls, kWindowClass) == 0;
}

static bool IsFullscreenAppActive(HWND taskbar) {
    // Exclusive fullscreen games / presentation mode
    QUERY_USER_NOTIFICATION_STATE quns;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns)) &&
        (quns == QUNS_RUNNING_D3D_FULL_SCREEN || quns == QUNS_PRESENTATION_MODE)) {
        return true;
    }

    // Borderless fullscreen (browser video, players, most games)
    HWND fg = GetForegroundWindow();
    if (!fg || IsShellWindowClass(fg)) return false;

    // A maximized window with a title bar is not fullscreen
    // (matters when the taskbar is set to auto-hide)
    LONG style = GetWindowLongW(fg, GWL_STYLE);
    if (IsZoomed(fg) && (style & WS_CAPTION) == WS_CAPTION) return false;

    HMONITOR mon = MonitorFromWindow(taskbar, MONITOR_DEFAULTTOPRIMARY);
    if (MonitorFromWindow(fg, MONITOR_DEFAULTTONULL) != mon) return false;

    MONITORINFO mi{sizeof(mi)};
    RECT wr;
    if (!GetMonitorInfoW(mon, &mi) || !GetWindowRect(fg, &wr)) return false;

    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
}

static void UpdateVisibility(HWND hwnd) {
    HWND taskbar = GetWindow(hwnd, GW_OWNER);
    RECT r;
    UINT dpi;
    bool show = taskbar && IsWindowVisible(taskbar) && GetGeometry(&r, &dpi);
    if (show && GetSettings().hideInFullscreen && IsFullscreenAppActive(taskbar))
        show = false;

    if (show && !g_visible) {
        Render(hwnd);
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                         SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
        g_visible = true;
    } else if (!show && g_visible) {
        ShowWindow(hwnd, SW_HIDE);
        g_visible = false;
        g_hover = false;
        g_hoverHit = HitNone;
    } else if (show && ++g_tickCount % 4 == 0) {
        // Once a second, make sure the taskbar hasn't covered the timer
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
}

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------

static void ShowMenu(HWND hwnd) {
    static const int presets[] = {1, 3, 5, 10, 15, 20, 25, 30, 45, 60, 90, 120};
    constexpr int kPresetBase = 100;
    constexpr int kCmdToggle = 1;
    constexpr int kCmdReset = 2;

    HMENU menu = CreatePopupMenu();
    for (int i = 0; i < (int)(sizeof(presets) / sizeof(presets[0])); i++) {
        std::wstring label = std::to_wstring(presets[i]) + L" min";
        AppendMenuW(menu, MF_STRING, kPresetBase + i, label.c_str());
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kCmdToggle,
                g_state == State::Running ? L"Pause" : L"Start");
    AppendMenuW(menu, MF_STRING, kCmdReset, L"Reset");

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                             pt.x, pt.y, 0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);

    if (cmd >= kPresetBase) {
        g_durationMs = presets[cmd - kPresetBase] * kMinute;
        ResetTimer(hwnd);
        StartTimer();
    } else if (cmd == kCmdToggle) {
        ToggleTimer(hwnd);
    } else if (cmd == kCmdReset) {
        ResetTimer(hwnd);
    }
    Render(hwnd);
}

// ---------------------------------------------------------------------------
// Window procedure
// ---------------------------------------------------------------------------

static void StopRepeat(HWND hwnd) {
    if (g_repeatHit != HitNone) {
        g_repeatHit = HitNone;
        KillTimer(hwnd, TIMER_REPEAT);
        if (GetCapture() == hwnd) ReleaseCapture();
    }
}

static void ApplyArrowHit(int hit) {
    if (hit >= HitUp0 && hit <= HitUp2) AdjustField(hit - HitUp0, +1);
    else if (hit >= HitDown0 && hit <= HitDown2) AdjustField(hit - HitDown0, -1);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_TIMER:
            if (wp == TIMER_TICK) {
                UpdateVisibility(hwnd);
                if (g_state == State::Running) {
                    if (RemainingMs() <= 0) FinishTimer(hwnd);
                    Render(hwnd);
                } else if (g_state == State::Finished) {
                    Render(hwnd);
                }
            } else if (wp == TIMER_ALARM) {
                if (g_state != State::Finished ||
                    ++g_alarmPlayed > GetSettings().repeatCount) {
                    KillTimer(hwnd, TIMER_ALARM);
                } else {
                    PlayAlarm();
                }
            } else if (wp == TIMER_REPEAT) {
                // Auto-repeat while an arrow is held
                if (g_repeatHit != HitNone) {
                    ApplyArrowHit(g_repeatHit);
                    SetTimer(hwnd, TIMER_REPEAT, 70, nullptr);
                    Render(hwnd);
                }
            }
            return 0;

        case WM_LBUTTONDOWN: {
            int hit = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (hit == HitPlay) {
                ToggleTimer(hwnd);
            } else if ((hit >= HitUp0 && hit <= HitDown2)) {
                ApplyArrowHit(hit);
                g_repeatHit = hit;
                SetCapture(hwnd);
                SetTimer(hwnd, TIMER_REPEAT, 400, nullptr);
            }
            Render(hwnd);
            return 0;
        }

        case WM_LBUTTONUP:
            StopRepeat(hwnd);
            Render(hwnd);
            return 0;

        case WM_CAPTURECHANGED:
            if ((HWND)lp != hwnd && g_repeatHit != HitNone) {
                g_repeatHit = HitNone;
                KillTimer(hwnd, TIMER_REPEAT);
                Render(hwnd);
            }
            return 0;

        case WM_MBUTTONUP:
            ResetTimer(hwnd);
            Render(hwnd);
            return 0;

        case WM_RBUTTONUP:
            StopRepeat(hwnd);
            ShowMenu(hwnd);
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(hwnd, &pt);
            int hit = HitTest(pt.x, pt.y);
            int field = 1;  // minutes by default
            if (hit >= HitUp0 && hit <= HitUp2) field = hit - HitUp0;
            else if (hit >= HitDown0 && hit <= HitDown2) field = hit - HitDown0;
            else if (hit >= HitSeg0 && hit <= HitSeg2) field = hit - HitSeg0;
            AdjustField(field, GET_WHEEL_DELTA_WPARAM(wp) > 0 ? +1 : -1);
            Render(hwnd);
            return 0;
        }

        case WM_MOUSEMOVE: {
            int hit = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            bool needRender = false;
            if (!g_hover) {
                g_hover = true;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                needRender = true;
            }
            if (hit != g_hoverHit) {
                g_hoverHit = hit;
                needRender = true;
            }
            if (needRender) Render(hwnd);
            return 0;
        }

        case WM_MOUSELEAVE:
            g_hover = false;
            g_hoverHit = HitNone;
            Render(hwnd);
            return 0;

        case WM_APP_REPOSITION:
            Render(hwnd);
            return 0;

        case WM_APP_SETTINGS:
            if (g_state == State::Idle) {
                g_durationMs = GetSettings().defaultMinutes * kMinute;
                g_remainingMs = g_durationMs;
            }
            Render(hwnd);
            return 0;

        case WM_APP_EXIT:
            EndMenu();
            return 0;

        case WM_SETTINGCHANGE:
            g_lightTheme = ReadLightTheme();
            Render(hwnd);
            break;

        case WM_DESTROY:
            // Also happens when Windows recreates the taskbar (our owner).
            // The UI thread will create a new window; the countdown continues.
            KillTimer(hwnd, TIMER_TICK);
            KillTimer(hwnd, TIMER_REPEAT);
            KillTimer(hwnd, TIMER_ALARM);
            if (g_hwnd.load() == hwnd) g_hwnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// UI thread
// ---------------------------------------------------------------------------

static DWORD WINAPI UiThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HWND taskbar = nullptr;
    for (;;) {
        taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (taskbar) {
            DWORD pid = 0;
            GetWindowThreadProcessId(taskbar, &pid);
            if (pid != GetCurrentProcessId()) return 0;
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 1000) == WAIT_OBJECT_0) return 0;
    }

    Gdiplus::GdiplusStartupInput gdiInput;
    ULONG_PTR gdiToken = 0;
    Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, nullptr);

    g_lightTheme = ReadLightTheme();
    {
        int mins = GetSettings().defaultMinutes;
        if (mins <= 0) mins = 5;
        g_durationMs = mins * kMinute;
        g_remainingMs = g_durationMs;
    }
    Wh_Log(L"Initial duration: %d sec", (int)(g_durationMs / 1000));

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_HAND);
    wc.lpszClassName = kWindowClass;
    RegisterClassExW(&wc);

    HANDLE posThread = CreateThread(nullptr, 0, PositionThread, nullptr, 0, nullptr);

    HWND hwnd = nullptr;
    HWND owner = nullptr;

    for (;;) {
        // (Re)create the window if the taskbar was recreated or our window is gone
        HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tb) {
            DWORD pid = 0;
            GetWindowThreadProcessId(tb, &pid);
            if (pid != GetCurrentProcessId()) break;
        }
        if (tb && (!hwnd || !IsWindow(hwnd) || owner != tb)) {
            if (hwnd && IsWindow(hwnd)) DestroyWindow(hwnd);
            hwnd = CreateWindowExW(
                WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
                kWindowClass, L"", WS_POPUP, 0, 0, 1, 1, tb, nullptr, g_hInstance,
                nullptr);
            owner = tb;
            g_visible = false;
            g_hover = false;
            g_hoverHit = HitNone;
            g_repeatHit = HitNone;
            if (hwnd) {
                Wh_Log(L"Timer window created");
                g_hwnd = hwnd;
                SetTimer(hwnd, TIMER_TICK, 250, nullptr);
                UpdateVisibility(hwnd);
            } else {
                Wh_Log(L"Failed to create the timer window");
            }
        }

        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, 1000, QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) break;

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_hwnd = nullptr;
    if (hwnd && IsWindow(hwnd)) DestroyWindow(hwnd);
    StopAlarm(nullptr);

    SetEvent(g_stopEvent);
    if (posThread) {
        WaitForSingleObject(posThread, INFINITE);
        CloseHandle(posThread);
    }

    UnregisterClassW(kWindowClass, g_hInstance);
    Gdiplus::GdiplusShutdown(gdiToken);
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    g_hInstance = GetSelfModule();
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    SetEvent(g_stopEvent);
    HWND h = g_hwnd.load();
    if (h) PostMessageW(h, WM_APP_EXIT, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    g_forceSearch = true;
    HWND h = g_hwnd.load();
    if (h) PostMessageW(h, WM_APP_SETTINGS, 0, 0);
}
