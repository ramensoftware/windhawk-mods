// ==WindhawkMod==
// @id              smooth-scroll-win32
// @name            Smooth Scroll for Win32
// @name:pt         Rolagem Suave para Win32
// @name:es         Desplazamiento Suave para Win32
// @description     Adds smooth scrolling with spring physics to legacy Win32 apps
// @description:pt  Adiciona rolagem suave com fisica de mola a aplicativos Win32 antigos
// @description:es  Anade desplazamiento suave con fisica de resorte a aplicaciones Win32
// @version         1.0.2
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @include         regedit.exe
// @include         mmc.exe
// @include         control.exe
// @include         wordpad.exe
// @compilerOptions -luser32 -lcomctl32 -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smooth Scroll for Win32

## English

Adds smooth scrolling with spring physics to legacy Win32 applications.
Only modifies legacy controls — modern WinUI windows are not affected.

### Scroll modes (auto-detected per window)

| Control | Method | Granularity |
|---|---|---|
| SysListView32 | LVM_SCROLL | Pixel |
| DirectUIHWND | UIA SetScrollPercent | Continuous |
| SysTreeView32, Edit, etc | WM_VSCROLL | Line |
| Unknown / modern | Passthrough | Not modified |

### Adding more programs

Go to the mod's **Advanced** tab in Windhawk and add custom
`@include` entries (e.g. `totalcmd.exe`). The mod auto-detects
scrollable controls in any Win32 application. Modern WinUI windows
inside any included process are automatically skipped.

### Known limitations

- File open/save dialogs use Direct Manipulation for scroll input,
  which bypasses the Win32 message queue. These cannot be smoothed.
- WinUI/XAML tabs in apps like Task Manager (Processes, Performance)
  are not affected. Classic tabs (Details, Services) work normally.

---

## Português

Adiciona rolagem suave com física de mola a aplicativos Win32 antigos.
Só modifica controles legados. Janelas WinUI não são afetadas.

### Adicionando mais programas

Vá na aba **Avançado** do mod no Windhawk e adicione entradas
`@include` (ex: `totalcmd.exe`). O mod detecta automaticamente
controles roláveis em qualquer aplicativo Win32.

### Limitações conhecidas

- Diálogos de abrir/salvar arquivo usam Direct Manipulation para
  input de scroll, que não passa pela fila de mensagens Win32.
- Abas WinUI/XAML em apps como o Gerenciador de Tarefas (Processos,
  Desempenho) não são afetadas. Abas clássicas (Detalhes, Serviços)
  funcionam normalmente.

---

## Español

Añade desplazamiento suave con física de resorte a las aplicaciones Win32 antiguas.
Solo modifica los controles antiguos. Las ventanas de WinUI no se ven afectadas.

### Agregar mas programas

Ve a la pestaña **Avanzado** del mod en Windhawk y añade entradas:
`@include` (p. ej., `totalcmd.exe`). El mod detecta automáticamente
los controles desplazables en cualquier aplicación Win32.

### Limitaciones conocidas

- Los dialogos de abrir/guardar archivo usan Direct Manipulation para
  la entrada de desplazamiento, que no pasa por la cola de mensajes Win32.
- Las pestañas WinUI/XAML en apps como el Administrador de Tareas
  (Procesos, Rendimiento) no se ven afectadas. Las pestañas clasicas
  (Detalles, Servicios) funcionan normalmente.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- springConstant: 700
  $name: Spring Constant
  $name:pt: Constante da Mola
  $name:es: Constante del Resorte
  $description: Animation speed. Higher is snappier, lower is smoother. 50 to 1000
  $description:pt: Velocidade da animação. Quanto maior, mais rápida; quanto menor, mais suave. 50 a 1000
  $description:es: Velocidad de la animación. Cuanto mayor, más rápida; cuanto menor, más suave. 50 a 1000

- dampingX10: 12
  $name: Damping x10
  $name:pt: Amortecimento x10
  $name:es: Amortiguación x10
  $description: Damping ratio times 10. 10 is critical, 11 prevents bounce, 8 slight bounce
  $description:pt: Proporção de amortecimento vezes 10. 10 é crítico, 11 evita bounce, 8 permite leve bounce
  $description:es: Relación de amortiguación por 10. 10 es crítico, 11 evita el rebote, 8 permite un leve rebote

- scrollMultiplierX10: 20
  $name: Scroll Multiplier x10
  $name:pt: Multiplicador de rolagem x10
  $name:es: Multiplicador de desplazamiento x10
  $description: Scroll amount times 10. 10 is default, 20 doubles it
  $description:pt: Quantidade de rolagem vezes 10. 10 é o padrão, 20 dobra o valor
  $description:es: Cantidad de desplazamiento por 10. 10 es el valor predeterminado, 20 lo duplica

- animationIntervalMs: 8
  $name: Animation Interval
  $name:pt: Intervalo da Animação
  $name:es: Intervalo de Animación
  $description: Timer in ms. 8 for 120hz+, 16 for 60hz
  $description:pt: Intervalo em ms. 8 para 120 Hz ou mais, 16 para 60 Hz
  $description:es: Intervalo en ms. 8 para 120 Hz o más, 16 para 60 Hz

- vsync: false
  $name: V-Sync
  $name:pt: V-Sync
  $name:es: V-Sync
  $description: Match animation interval to display refresh rate. Overrides Animation Interval
  $description:pt: Ajustar intervalo ao refresh do monitor. Substitui o Intervalo de Animacao
  $description:es: Ajustar intervalo al refresco del monitor. Reemplaza el Intervalo de Animacion

*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windowsx.h>
#include <objbase.h>
#include <uiautomation.h>

#include <cmath>
#include <map>

#ifndef UIA_ScrollPatternNoScroll
constexpr double UIA_ScrollPatternNoScroll = -1.0;
#endif

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct {
    double springK;
    double damping;
    double multiplier;
    int intervalMs;
    bool vsync;
} g_cfg;

// ---------------------------------------------------------------------------
// Spring physics (mass-spring-damper)
//
// Uses semi-implicit Euler integration with an initial velocity boost
// for immediate response on the first frame (no sluggish start).
// ---------------------------------------------------------------------------

struct Spring {
    double target = 0;
    double pos = 0;
    double vel = 0;

    // Advance one step, return position delta.
    double Step(double dt, double k, double dr) {
        double disp = pos - target;

        // Progressive damping: as the spring approaches the target,
        // increase damping to kill trailing drift smoothly.
        // Far from target (|disp| > 3): no effect (normal scrolling).
        // Close to target (|disp| -> 0): extra damping ramps up,
        // making the final approach quick without a visible snap.
        double absDisp = std::abs(disp);
        double extraDamp = 0;
        if (absDisp < 3.0) {
            extraDamp = (3.0 - absDisp) / 3.0 * 12.0;
        }

        double c = dr * 2.0 * std::sqrt(k) + extraDamp;
        double f = -k * disp - c * vel;
        vel += f * dt;
        double old = pos;
        pos += vel * dt;
        return pos - old;
    }

    // Add scroll target with velocity boost for instant response.
    void Push(double delta, double k) {
        target += delta;
        // Gentle kick to avoid sluggish start.  Lower values = smoother
        // onset, higher = more immediate but more "springy".
        vel += delta * std::sqrt(k) * 0.20;
    }

    void Snap() { pos = target; vel = 0; }
};

// ---------------------------------------------------------------------------
// Scroll method
// ---------------------------------------------------------------------------

enum class Method {
    LineScroll  = 0,   // WM_VSCROLL SB_LINEUP/DOWN
    PixelLV     = 1,   // LVM_SCROLL
    UIAPercent  = 2,   // IUIAutomationScrollPattern::SetScrollPercent
    Pass        = 3,   // Don't touch
};

static Method Detect(HWND h) {
    WCHAR c[64] = {};
    if (!GetClassNameW(h, c, ARRAYSIZE(c)))
        return Method::Pass;

    if (_wcsicmp(c, L"SysListView32") == 0)  return Method::PixelLV;

    if (_wcsicmp(c, L"DirectUIHWND") == 0) {
        // DirectUIHWND inside NativeHWNDHost is a WinUI-hosted container
        // (e.g. Task Manager) that doesn't respond to UIA scroll.
        // Pass through so native scrolling works.
        WCHAR pc[64] = {};
        HWND parent = GetParent(h);
        if (parent) GetClassNameW(parent, pc, ARRAYSIZE(pc));
        if (_wcsicmp(pc, L"NativeHWNDHost") == 0)
            return Method::Pass;
        return Method::UIAPercent;
    }

    // Known scrollable Win32 classes.
    if (_wcsicmp(c, L"SysTreeView32") == 0 ||
        _wcsicmp(c, L"Edit") == 0 ||
        _wcsicmp(c, L"ListBox") == 0 ||
        _wcsicmp(c, L"ComboLBox") == 0 ||
        _wcsicmp(c, L"RICHEDIT50W") == 0 ||
        _wcsicmp(c, L"RichEdit20W") == 0 ||
        _wcsicmp(c, L"RichEdit20A") == 0)
        return Method::LineScroll;

    // Any window with a standard scrollbar.
    SCROLLINFO si = { sizeof(si), SIF_RANGE };
    if (GetScrollInfo(h, SB_VERT, &si) && si.nMax > si.nMin)
        return Method::LineScroll;

    if (GetWindowLongW(h, GWL_STYLE) & (WS_VSCROLL | WS_HSCROLL))
        return Method::LineScroll;

    return Method::Pass;
}

// ---------------------------------------------------------------------------
// Find scrollable child under cursor (for container windows)
// ---------------------------------------------------------------------------

struct FindCtx { POINT pt; HWND out; };

static BOOL CALLBACK FindChildProc(HWND h, LPARAM lp) {
    auto* x = (FindCtx*)lp;
    if (!IsWindowVisible(h)) return TRUE;
    RECT r;
    GetWindowRect(h, &r);
    if (!PtInRect(&r, x->pt)) return TRUE;
    if (Detect(h) != Method::Pass) { x->out = h; return FALSE; }
    return TRUE;
}

static HWND FindChild(HWND parent, POINT pt) {
    FindCtx x = { pt, nullptr };
    EnumChildWindows(parent, FindChildProc, (LPARAM)&x);
    return x.out;
}

// ---------------------------------------------------------------------------
// ListView line height
// ---------------------------------------------------------------------------

static int LVLineH(HWND h) {
    int n = (int)SendMessageW(h, LVM_GETITEMCOUNT, 0, 0);
    if (n > 0) {
        RECT r{}; r.left = LVIR_BOUNDS;
        if (SendMessageW(h, LVM_GETITEMRECT,
                          (WPARAM)SendMessageW(h, LVM_GETTOPINDEX, 0, 0),
                          (LPARAM)&r) && r.bottom > r.top)
            return r.bottom - r.top;
    }
    return 20;
}

// ---------------------------------------------------------------------------
// UI Automation (lazy init + pattern caching)
//
// g_uia is per-thread (thread_local) because COM STA objects should not
// be shared across apartment threads.  COM is initialized once per thread
// and kept alive for the lifetime of the thread — CoUninitialize is not
// called, which is acceptable:
// https://github.com/microsoft/windows-rs/issues/1169#issuecomment-925107412
// ---------------------------------------------------------------------------

static thread_local IUIAutomation* g_uia = nullptr;
static thread_local bool g_uiaInit = false;

static IUIAutomation* UIA() {
    if (g_uiaInit) return g_uia;
    g_uiaInit = true;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
                     __uuidof(IUIAutomation), (void**)&g_uia);
    return g_uia;
}

static IUIAutomationScrollPattern* GetPattern(HWND h) {
    IUIAutomation* u = UIA();
    if (!u) return nullptr;
    IUIAutomationElement* e = nullptr;
    if (FAILED(u->ElementFromHandle(h, &e)) || !e) return nullptr;
    IUIAutomationScrollPattern* p = nullptr;
    e->GetCurrentPatternAs(UIA_ScrollPatternId,
                            __uuidof(IUIAutomationScrollPattern), (void**)&p);
    e->Release();
    return p;
}

static bool UIAScroll(IUIAutomationScrollPattern* p, double dV, double dH) {
    double cV = -1, cH = -1;
    p->get_CurrentVerticalScrollPercent(&cV);
    p->get_CurrentHorizontalScrollPercent(&cH);

    double nV = UIA_ScrollPatternNoScroll, nH = UIA_ScrollPatternNoScroll;
    if (std::abs(dV) > 0.0001 && cV >= 0) {
        nV = cV + dV;
        if (nV < 0) nV = 0; if (nV > 100) nV = 100;
    }
    if (std::abs(dH) > 0.0001 && cH >= 0) {
        nH = cH + dH;
        if (nH < 0) nH = 0; if (nH > 100) nH = 100;
    }
    if (nV < -0.5 && nH < -0.5) return true;
    return SUCCEEDED(p->SetScrollPercent(nH, nV));
}

// Estimate percent-per-line using the window height to approximate
// how many rows are visible.  This scales correctly regardless of
// total item count — a folder with 30 or 3000 items scrolls the
// same visual distance per wheel notch.
static double EstimatePPL(IUIAutomationScrollPattern* p, HWND hwnd) {
    double vs = 0;
    p->get_CurrentVerticalViewSize(&vs);
    if (vs <= 0 || vs >= 100) return 1.0;

    RECT rc = {};
    GetClientRect(hwnd, &rc);
    int wh = rc.bottom - rc.top;
    if (wh < 1) wh = 400;

    // Approximate visible rows.  24px is a middle-ground row height
    // that works reasonably for both Details (~20px) and Icon views
    // (~80-120px per row).  The key insight: this only affects the
    // *visual distance* per notch, and users can tune with multiplier.
    double approxRows = wh / 24.0;
    if (approxRows < 3) approxRows = 3;

    double ppl = vs / approxRows;
    if (ppl < 0.05) ppl = 0.05;
    if (ppl > 5.0) ppl = 5.0;
    return ppl;
}

// ---------------------------------------------------------------------------
// Per-window state
// ---------------------------------------------------------------------------

struct State {
    Spring sV, sH;
    UINT_PTR timer = 0;
    Method method = Method::Pass;
    int lineH = 20;
    double ppl = 0;          // percent-per-line (UIA)
    double accV = 0, accH = 0;
    IUIAutomationScrollPattern* pat = nullptr;
    bool uiaOk = true;
    bool hasVScroll = true;
    bool hasHScroll = false;
};

static CRITICAL_SECTION g_cs;
static std::map<HWND, State> g_st;
static UINT g_sysLines = 3;

constexpr UINT_PTR TID = 0x534D5448;

static void Release(State& s) {
    if (s.pat) { s.pat->Release(); s.pat = nullptr; }
}

// Method-aware settling: is the remaining motion imperceptible?
static bool Settled(const State& s) {
    double tV, tH, tVel;
    switch (s.method) {
    case Method::LineScroll: tV = tH = 0.4; tVel = 2.0; break;
    case Method::PixelLV:   tV = tH = 0.8; tVel = 3.0; break;
    case Method::UIAPercent: tV = tH = 0.15; tVel = 1.5; break;
    default:                tV = tH = 0.3; tVel = 1.5; break;
    }
    return s.sV.pos - s.sV.target < tV &&
           s.sV.target - s.sV.pos < tV &&
           s.sH.pos - s.sH.target < tH &&
           s.sH.target - s.sH.pos < tH &&
           std::abs(s.sV.vel) < tVel &&
           std::abs(s.sH.vel) < tVel;
}

// ---------------------------------------------------------------------------
// Load settings
// ---------------------------------------------------------------------------

static void LoadSettings() {
    int k = Wh_GetIntSetting(L"springConstant");
    g_cfg.springK = (k >= 50 && k <= 1000) ? (double)k : 700.0;

    int d = Wh_GetIntSetting(L"dampingX10");
    g_cfg.damping = (d >= 3 && d <= 30) ? d / 10.0 : 1.2;

    int m = Wh_GetIntSetting(L"scrollMultiplierX10");
    g_cfg.multiplier = (m >= 1 && m <= 100) ? m / 10.0 : 2.0;

    int i = Wh_GetIntSetting(L"animationIntervalMs");
    g_cfg.intervalMs = (i >= 4 && i <= 100) ? i : 8;

    g_cfg.vsync = Wh_GetIntSetting(L"vsync");

    // V-Sync: override timer interval to match display refresh rate.
    // Non-blocking — just sets the right interval so we produce one
    // scroll update per display frame instead of blocking with DwmFlush.
    if (g_cfg.vsync) {
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &dm) &&
            dm.dmDisplayFrequency > 0) {
            g_cfg.intervalMs = 1000 / dm.dmDisplayFrequency;
            if (g_cfg.intervalMs < 4) g_cfg.intervalMs = 4;
        }
    }

    UINT ln = 3;
    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &ln, 0);
    if (ln == 0) ln = 3;
    if (ln == WHEEL_PAGESCROLL) ln = 20;
    g_sysLines = ln;

    Wh_Log(L"Cfg: k=%.0f d=%.2f m=%.1f i=%d vs=%d sl=%u",
            g_cfg.springK, g_cfg.damping, g_cfg.multiplier,
            g_cfg.intervalMs, g_cfg.vsync, g_sysLines);
}

// ---------------------------------------------------------------------------
// Timer — runs every intervalMs, steps the spring, sends scroll commands
// ---------------------------------------------------------------------------

static void CALLBACK Tick(HWND hw, UINT, UINT_PTR, DWORD) {
    EnterCriticalSection(&g_cs);

    auto it = g_st.find(hw);
    if (it == g_st.end() || !IsWindow(hw)) {
        KillTimer(hw, TID);
        if (it != g_st.end()) { Release(it->second); g_st.erase(it); }
        LeaveCriticalSection(&g_cs);
        return;
    }

    State& s = it->second;
    double dt = g_cfg.intervalMs / 1000.0;

    // Step springs.
    double dV = s.sV.Step(dt, g_cfg.springK, g_cfg.damping);
    double dH = s.sH.Step(dt, g_cfg.springK, g_cfg.damping);

    // Stop early if remaining motion is imperceptible.
    if (Settled(s)) {
        s.sV.Snap(); s.sH.Snap();
        s.accV = s.accH = 0;
        KillTimer(hw, TID);
        s.timer = 0;
        LeaveCriticalSection(&g_cs);
        return;
    }

    // ---- Vertical ----
    if (std::abs(dV) > 0.0001) {
        switch (s.method) {

        case Method::PixelLV: {
            s.accV += dV;
            int px = (int)s.accV;
            if (px) {
                s.accV -= px;
                int dy = -px;

                // Prevent overscroll: if already at top/bottom, snap the
                // spring to stop the animation instead of scrolling further.
                SCROLLINFO si = { sizeof(si), SIF_ALL };
                if (GetScrollInfo(hw, SB_VERT, &si)) {
                    int maxPos = si.nMax - (int)si.nPage + 1;
                    if ((dy < 0 && si.nPos <= si.nMin) ||
                        (dy > 0 && si.nPos >= maxPos)) {
                        s.sV.Snap();
                        s.accV = 0;
                        break;
                    }
                }

                LeaveCriticalSection(&g_cs);
                SendMessageW(hw, LVM_SCROLL, 0, (LPARAM)dy);
                EnterCriticalSection(&g_cs);
            }
            break;
        }

        case Method::LineScroll: {
            s.accV += dV;
            int ln = (int)s.accV;
            if (ln) {
                s.accV -= ln;
                UINT cmd = (ln > 0) ? SB_LINEUP : SB_LINEDOWN;
                int n = (ln > 0) ? ln : -ln;
                LeaveCriticalSection(&g_cs);
                for (int i = 0; i < n; i++)
                    SendMessageW(hw, WM_VSCROLL, MAKEWPARAM(cmd, 0), 0);
                SendMessageW(hw, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
                EnterCriticalSection(&g_cs);
            }
            break;
        }

        case Method::UIAPercent: {
            if (s.uiaOk && s.pat) {
                double pct = dV * s.ppl;

                // Boundary: snap if at top scrolling up or bottom down.
                double curPct = -1;
                s.pat->get_CurrentVerticalScrollPercent(&curPct);
                if (curPct >= 0 &&
                    ((pct < 0 && curPct <= 0.01) ||
                     (pct > 0 && curPct >= 99.99))) {
                    s.sV.Snap();
                    s.accV = 0;
                    break;
                }

                auto* p = s.pat;
                LeaveCriticalSection(&g_cs);
                if (!UIAScroll(p, pct, 0)) {
                    EnterCriticalSection(&g_cs);
                    auto i2 = g_st.find(hw);
                    if (i2 != g_st.end()) i2->second.uiaOk = false;
                    break;
                }
                EnterCriticalSection(&g_cs);
            }
            break;
        }

        default: break;
        }

        it = g_st.find(hw);
        if (it == g_st.end()) {
            KillTimer(hw, TID);
            LeaveCriticalSection(&g_cs);
            return;
        }
    }

    // ---- Horizontal ----
    if (std::abs(dH) > 0.0001 && it != g_st.end()) {
        State& sh = it->second;
        switch (sh.method) {

        case Method::PixelLV: {
            sh.accH += dH;
            int px = (int)sh.accH;
            if (px) {
                sh.accH -= px;
                int dx = -px;

                SCROLLINFO si = { sizeof(si), SIF_ALL };
                if (GetScrollInfo(hw, SB_HORZ, &si)) {
                    int maxPos = si.nMax - (int)si.nPage + 1;
                    if ((dx < 0 && si.nPos <= si.nMin) ||
                        (dx > 0 && si.nPos >= maxPos)) {
                        sh.sH.Snap();
                        sh.accH = 0;
                        break;
                    }
                }

                LeaveCriticalSection(&g_cs);
                SendMessageW(hw, LVM_SCROLL, (WPARAM)dx, 0);
                EnterCriticalSection(&g_cs);
            }
            break;
        }

        case Method::LineScroll: {
            sh.accH += dH;
            int ln = (int)sh.accH;
            if (ln) {
                sh.accH -= ln;
                UINT cmd = (ln > 0) ? SB_LINELEFT : SB_LINERIGHT;
                int n = (ln > 0) ? ln : -ln;
                LeaveCriticalSection(&g_cs);
                for (int i = 0; i < n; i++)
                    SendMessageW(hw, WM_HSCROLL, MAKEWPARAM(cmd, 0), 0);
                SendMessageW(hw, WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
                EnterCriticalSection(&g_cs);
            }
            break;
        }

        case Method::UIAPercent: {
            if (sh.uiaOk && sh.pat) {
                double pct = dH * sh.ppl;

                double curPct = -1;
                sh.pat->get_CurrentHorizontalScrollPercent(&curPct);
                if (curPct >= 0 &&
                    ((pct < 0 && curPct <= 0.01) ||
                     (pct > 0 && curPct >= 99.99))) {
                    sh.sH.Snap();
                    sh.accH = 0;
                    break;
                }

                auto* p = sh.pat;
                LeaveCriticalSection(&g_cs);
                UIAScroll(p, 0, pct);
                EnterCriticalSection(&g_cs);
            }
            break;
        }

        default: break;
        }

        it = g_st.find(hw);
        if (it == g_st.end()) {
            KillTimer(hw, TID);
            LeaveCriticalSection(&g_cs);
            return;
        }
    }

    LeaveCriticalSection(&g_cs);
}

// ---------------------------------------------------------------------------
// Intercept WM_MOUSEWHEEL
// ---------------------------------------------------------------------------

static bool Handle(const MSG* m) {
    if (!m || !m->hwnd) return false;

    short delta = GET_WHEEL_DELTA_WPARAM(m->wParam);
    if (!delta) return false;

    // Ctrl+wheel is zoom (icon size, font size, etc.) — let it through.
    if (GET_KEYSTATE_WPARAM(m->wParam) & MK_CONTROL)
        return false;

    bool vert = (m->message == WM_MOUSEWHEEL);
    HWND tgt = m->hwnd;
    Method mt = Detect(tgt);

    // Container? Try children.
    if (mt == Method::Pass) {
        POINT pt = { GET_X_LPARAM(m->lParam), GET_Y_LPARAM(m->lParam) };
        HWND ch = FindChild(tgt, pt);
        if (ch) { tgt = ch; mt = Detect(ch); }
    }
    if (mt == Method::Pass) return false;

    // Must be in-process.
    DWORD pid = 0;
    GetWindowThreadProcessId(tgt, &pid);
    if (pid != GetCurrentProcessId()) return false;

    // SysListView32 in horizontal-only modes (Icon, Table, Tile, etc.):
    // the default WM_MOUSEWHEEL handler already scrolls correctly in these
    // modes, so we pass through to avoid breaking it.  Only intercept when
    // the ListView has a vertical scrollbar (Details view, etc.).
    if (mt == Method::PixelLV && vert) {
        SCROLLINFO si = { sizeof(si), SIF_RANGE };
        if (!GetScrollInfo(tgt, SB_VERT, &si) || si.nMax <= si.nMin)
            return false;
    }

    double lines = ((double)delta / WHEEL_DELTA) * g_sysLines * g_cfg.multiplier;
    int lh = (mt == Method::PixelLV) ? LVLineH(tgt) : 20;

    EnterCriticalSection(&g_cs);

    State& s = g_st[tgt];
    s.method = mt;
    s.lineH = lh;

    // Lazy init UIA pattern.
    if (mt == Method::UIAPercent && s.uiaOk && !s.pat) {
        LeaveCriticalSection(&g_cs);
        auto* p = GetPattern(tgt);
        EnterCriticalSection(&g_cs);
        auto i2 = g_st.find(tgt);
        if (i2 == g_st.end()) {
            if (p) p->Release();
            LeaveCriticalSection(&g_cs);
            return false;
        }
        i2->second.pat = p;
        if (!p) {
            i2->second.uiaOk = false;
            LeaveCriticalSection(&g_cs);
            return false;
        }
    }

    // Refresh scroll axis availability on every event — the user may
    // switch views while the same DirectUIHWND is reused.
    if (mt == Method::UIAPercent && s.pat) {
        double pctV = -1, pctH = -1;
        s.pat->get_CurrentVerticalScrollPercent(&pctV);
        s.pat->get_CurrentHorizontalScrollPercent(&pctH);
        s.hasVScroll = (pctV >= 0);
        s.hasHScroll = (pctH >= 0);
        s.ppl = EstimatePPL(s.pat, tgt);
    }

    // Spring delta.
    double add = 0;
    switch (mt) {
    case Method::PixelLV:    add = lines * lh;  break;
    case Method::LineScroll: add = lines;       break;
    case Method::UIAPercent: add = -lines;      break;  // invert: up=decrease%
    default: break;
    }

    // Determine target axis and push.
    // For UIAPercent, check which scroll axes are available.
    // ListView mode in Explorer only has horizontal scroll — we reroute
    // the vertical wheel to horizontal in that case.
    Spring* pushedSpring = nullptr;
    double pushValue = 0;

    if (mt == Method::UIAPercent) {
        if (vert) {
            if (s.hasVScroll) {
                pushValue = add;
                s.sV.Push(pushValue, g_cfg.springK);
                pushedSpring = &s.sV;
            } else if (s.hasHScroll) {
                // Vertical wheel -> reroute to horizontal scroll.
                pushValue = add;
                s.sH.Push(pushValue, g_cfg.springK);
                pushedSpring = &s.sH;
            }
        } else {
            if (s.hasHScroll) {
                pushValue = -add;
                s.sH.Push(pushValue, g_cfg.springK);
                pushedSpring = &s.sH;
            }
        }
    } else {
        // Non-UIA methods: negate horizontal because convention is opposite
        // (positive add = scroll up, but SB_LINELEFT / -dx = scroll left).
        if (vert) {
            pushValue = add;
            s.sV.Push(pushValue, g_cfg.springK);
            pushedSpring = &s.sV;
        } else {
            pushValue = -add;
            s.sH.Push(pushValue, g_cfg.springK);
            pushedSpring = &s.sH;
        }
    }

    // If no scrollable axis found, let the original message through.
    if (!pushedSpring) {
        LeaveCriticalSection(&g_cs);
        return false;
    }

    // Start timer.
    if (!s.timer) {
        s.timer = SetTimer(tgt, TID, (UINT)g_cfg.intervalMs, Tick);
        if (!s.timer) {
            pushedSpring->target -= pushValue;
            LeaveCriticalSection(&g_cs);
            return false;
        }
    }

    LeaveCriticalSection(&g_cs);
    return true;
}

// ---------------------------------------------------------------------------
// DispatchMessage hooks
// ---------------------------------------------------------------------------

using DW = decltype(&DispatchMessageW);
using DA = decltype(&DispatchMessageA);
static DW g_origW;
static DA g_origA;

static LRESULT WINAPI HookW(const MSG* m) {
    if (m && (m->message == WM_MOUSEWHEEL || m->message == WM_MOUSEHWHEEL))
        if (Handle(m)) return 0;
    return g_origW(m);
}

static LRESULT WINAPI HookA(const MSG* m) {
    if (m && (m->message == WM_MOUSEWHEEL || m->message == WM_MOUSEHWHEEL))
        if (Handle(m)) return 0;
    return g_origA(m);
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void Wh_ModSettingsChanged() { LoadSettings(); }

BOOL Wh_ModInit() {
    Wh_Log(L"Smooth Scroll — Init");
    InitializeCriticalSection(&g_cs);
    LoadSettings();
    Wh_SetFunctionHook((void*)&DispatchMessageW, (void*)&HookW, (void**)&g_origW);
    Wh_SetFunctionHook((void*)&DispatchMessageA, (void*)&HookA, (void**)&g_origA);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Smooth Scroll — Uninit");
    EnterCriticalSection(&g_cs);
    for (auto& p : g_st) {
        if (p.second.timer && IsWindow(p.first))
            KillTimer(p.first, TID);
        Release(p.second);
    }
    g_st.clear();
    LeaveCriticalSection(&g_cs);
    DeleteCriticalSection(&g_cs);
    // Note: g_uia is thread_local and not released here — each thread's
    // IUIAutomation instance lives until the process exits.  The cached
    // ScrollPattern pointers in g_st are explicitly released above.
}
