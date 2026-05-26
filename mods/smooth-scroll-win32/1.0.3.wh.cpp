// ==WindhawkMod==
// @id              smooth-scroll-win32
// @name            Smooth Scroll for Win32
// @name:pt         Rolagem Suave para Win32
// @name:es         Desplazamiento Suave para Win32
// @description     Adds smooth scrolling with spring physics to legacy Win32 apps
// @description:pt  Adiciona rolagem suave com fisica de mola a aplicativos Win32 antigos
// @description:es  Anade desplazamiento suave con fisica de resorte a aplicaciones Win32
// @version         1.0.3
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @include         regedit.exe
// @include         mmc.exe
// @include         control.exe
// @include         wordpad.exe
// @include         taskmgr.exe
// @compilerOptions -luser32 -lcomctl32 -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smooth Scroll for Win32

## English

Adds smooth scrolling with spring physics to legacy Win32 applications.
Only modifies legacy controls — modern WinUI windows are not affected.

### What's new in 1.0.3

- **Consistent scroll speed across folder sizes** — the UIA percent-per-line
  estimate now uses a multi-range curve (quadratic for view sizes 25–70,
  power-5 for 70–92, power-20 for 92–100) that keeps scroll distance
  consistent regardless of how many items a folder contains.
  Credit: **JTFA** (github.com/JTFA).
- **Smooth-only mode** — a new setting disables spring physics entirely.
  Scrolling uses linear easing like a touchscreen: no bounce, no overshoot,
  just a clean deceleration. Tune the speed with **Smooth Speed**.
- **Improved spring physics** — progressive damping near the target
  eliminates micro-oscillation at rest without affecting the feel of
  the main animation.
- **Control Panel scroll no longer blocked** — legacy `DirectUIHWND` containers
  (Control Panel) are now correctly identified via the root window's WndProc
  module and passed through to the native scroll handler. Detection is stable
  from the first scroll, eliminating the intermittent first-open failure.
- **Overshoot** — new setting that underdamps the spring for a subtle
  mobile-like bounce. The progressive near-target damping still prevents
  endless oscillation. No effect in Smooth-only mode.
- **Automatic refresh rate** — Animation Interval and V-Sync settings removed.
  The timer interval is now detected per window from the actual monitor's
  refresh rate and updates automatically when the window moves to another
  display.

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
- Scrolling the modern Explorer content area (`DirectUIHWND`) while the
  window is in the background will bring it to the foreground. This is
  an inherent behaviour of `IUIAutomationScrollPattern::SetScrollPercent`,
  which activates the target window as part of the UI Automation
  accessibility model. There is no public API to scroll this control
  without activating it.

---

## Português

Adiciona rolagem suave com física de mola a aplicativos Win32 antigos.
Só modifica controles legados. Janelas WinUI não são afetadas.

### Novidades na 1.0.3

- **Velocidade de scroll consistente entre pastas** — o cálculo de
  percent-per-line via UIA agora usa uma curva multi-faixa (quadrática
  para viewSize 25–70, potência-5 para 70–92, potência-20 para 92–100)
  que mantém a distância de scroll consistente independente do número de
  itens na pasta. Crédito: **JTFA** (github.com/JTFA).
- **Modo apenas suave** — novo setting que desativa a física de mola.
  A rolagem usa suavização linear como tela de toque: sem bounce, sem
  ultrapassagem, só desaceleração limpa. Ajuste a velocidade com
  **Velocidade Suave**.
- **Física de mola melhorada** — amortecimento progressivo próximo ao
  alvo elimina micro-oscilação em repouso sem afetar o comportamento
  da animação principal.
- **Rolagem do Painel de Controle corrigida** — containers `DirectUIHWND`
  legados (Painel de Controle) agora são corretamente identificados pelo
  módulo WndProc da janela raiz e repassados ao handler nativo. A detecção
  é estável desde o primeiro scroll, eliminando a falha intermitente.
- **Ultrapassagem** — novo setting que sub-amorte a mola para um leve bounce
  estilo mobile. O amortecimento progressivo próximo ao alvo ainda impede
  oscilação infinita. Sem efeito no modo Apenas Suave.
- **Taxa de atualização automática** — os settings Intervalo de Animação e
  V-Sync foram removidos. O intervalo do timer agora é detectado por janela
  a partir do monitor real e atualiza automaticamente ao mover para outro display.

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
- Rolar a área de conteúdo moderna do Explorer (`DirectUIHWND`) com a
  janela em segundo plano fará ela vir para a frente. Isso é um
  comportamento inerente de `IUIAutomationScrollPattern::SetScrollPercent`,
  que ativa a janela alvo como parte do modelo de acessibilidade da UI
  Automation. Não há API pública para scrollar esse controle sem ativá-lo.

---

## Español

Añade desplazamiento suave con física de resorte a las aplicaciones Win32 antiguas.
Solo modifica los controles antiguos. Las ventanas de WinUI no se ven afectadas.

### Novedades en 1.0.3

- **Velocidad de desplazamiento consistente entre carpetas** — el cálculo
  de porcentaje por línea via UIA ahora usa una curva de múltiples rangos
  (cuadrática para viewSize 25–70, potencia-5 para 70–92, potencia-20 para
  92–100) que mantiene la distancia de desplazamiento consistente
  independientemente del número de elementos en la carpeta.
  Crédito: **JTFA** (github.com/JTFA).
- **Modo solo suave** — nuevo ajuste que desactiva la física de resorte.
  El desplazamiento usa suavizado lineal como una pantalla táctil: sin
  rebote, sin sobreimpulso, solo deceleración limpia. Ajusta la velocidad
  con **Velocidad Suave**.
- **Física de resorte mejorada** — amortiguación progresiva cerca del
  objetivo elimina micro-oscilaciones en reposo sin afectar el
  comportamiento de la animación principal.
- **Desplazamiento del Panel de Control corregido** — los contenedores
  `DirectUIHWND` legacy (Panel de Control) ahora se identifican correctamente
  por el módulo WndProc de la ventana raíz y se pasan al controlador nativo.
- **Rebote** — nuevo ajuste que sub-amortigua el resorte para un leve rebote
  estilo móvil. La amortiguación progresiva cerca del objetivo aún previene
  oscilaciones. Sin efecto en modo Solo Suave.
- **Tasa de actualización automática** — los ajustes Intervalo de Animación y
  V-Sync fueron eliminados. El intervalo del temporizador ahora se detecta por
  ventana desde el monitor real y se actualiza al moverla a otra pantalla.

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
- Desplazar el área de contenido moderna del Explorador (`DirectUIHWND`)
  con la ventana en segundo plano la traerá al frente. Es un comportamiento
  inherente de `IUIAutomationScrollPattern::SetScrollPercent`, que activa
  la ventana objetivo como parte del modelo de accesibilidad de UI
  Automation. No existe API pública para desplazar este control sin
  activarlo.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- spring:
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
      $description: Damping ratio times 10. 10 is critical, 12 prevents bounce, 8 slight bounce
      $description:pt: Proporção de amortecimento vezes 10. 10 é crítico, 12 evita bounce, 8 permite leve bounce
      $description:es: Relación de amortiguación por 10. 10 es crítico, 12 evita el rebote, 8 permite un leve rebote
    - overshoot: false
      $name: Overshoot
      $name:pt: Ultrapassagem
      $name:es: Rebote
      $description: Underdamps the spring for a subtle mobile-like bounce. No effect in Smooth Only mode.
      $description:pt: Sub-amorte a mola para um leve bounce estilo mobile. Sem efeito no modo Apenas Suave.
      $description:es: Sub-amortigua el resorte para un leve rebote estilo móvil. Sin efecto en modo Solo Suave.
  $name: Spring Physics
  $name:pt: Física da Mola
  $name:es: Física del Resorte

- scrollMultiplierX10: 20
  $name: Scroll Multiplier x10
  $name:pt: Multiplicador de rolagem x10
  $name:es: Multiplicador de desplazamiento x10
  $description: Scroll amount times 10. 10 is default, 20 doubles it
  $description:pt: Quantidade de rolagem vezes 10. 10 é o padrão, 20 dobra o valor
  $description:es: Cantidad de desplazamiento por 10. 10 es el valor predeterminado, 20 lo duplica

- smoothOnly: false
  $name: Smooth Only (no spring)
  $name:pt: Apenas Suave (sem mola)
  $name:es: Solo Suave (sin resorte)
  $description: Disables spring physics. Scrolling uses linear easing like a touchscreen — no bounce, no overshoot
  $description:pt: Desativa a fisica de mola. A rolagem usa suavizacao linear como uma tela de toque — sem bounce, sem ultrapassagem
  $description:es: Desactiva la fisica de resorte. El desplazamiento usa suavizado lineal como una pantalla tactil — sin rebote, sin sobreimpulso

- smoothSpeedX10: 3
  $name: Smooth Speed x10 (smooth only)
  $name:pt: Velocidade Suave x10 (apenas suave)
  $name:es: Velocidad Suave x10 (solo suave)
  $description: Easing speed when Smooth Only is on. 1=very smooth, 3=responsive, 5=fast. 1 to 8
  $description:pt: Velocidade de suavizacao quando Apenas Suave esta ativo. 1=muito suave, 3=responsivo, 5=rapido. 1 a 8
  $description:es: Velocidad de suavizado cuando Solo Suave esta activo. 1=muy suave, 3=responsivo, 5=rapido. 1 a 8

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
    bool smoothOnly;
    double smoothSpeed;  // 0.1 to 0.8
    bool overshoot;      // underdamp the spring for a subtle bounce
} g_cfg;

// Protects g_cfg and g_sysLines against concurrent reads (Tick thread)
// and writes (LoadSettings called from Wh_ModSettingsChanged).
static SRWLOCK g_cfgLock = SRWLOCK_INIT;
static UINT g_sysLines_g = 3; // renamed to avoid collision with local in LoadSettings

// Snapshot of settings read atomically under shared lock.
// Tick and Handle call this once per invocation to avoid repeated lock acquires.
struct CfgSnap {
    double springK, damping, multiplier, smoothSpeed;
    bool smoothOnly;
    bool overshoot;
    UINT sysLines;
};

static CfgSnap SnapCfg() {
    AcquireSRWLockShared(&g_cfgLock);
    CfgSnap s;
    s.springK    = g_cfg.springK;
    s.damping    = g_cfg.damping;
    s.multiplier = g_cfg.multiplier;
    s.smoothSpeed= g_cfg.smoothSpeed;
    s.smoothOnly = g_cfg.smoothOnly;
    s.overshoot  = g_cfg.overshoot;
    s.sysLines   = g_sysLines_g;
    ReleaseSRWLockShared(&g_cfgLock);
    return s;
}

// ---------------------------------------------------------------------------
// Spring physics — touch-screen style
//
// High velocity boost for immediate response, overdamped to prevent
// bounce, minimal progressive damping for quick clean stop.
// ---------------------------------------------------------------------------

struct Spring {
    double target = 0;
    double pos = 0;
    double vel = 0;

    // Spring mode: mass-spring-damper with progressive damping.
    // When overshoot=true the damping ratio is reduced to ~0.65 of its
    // configured value, making the spring underdamped. The progressive
    // extra-damping near the target still ensures a clean stop without
    // endless oscillation -- the result is one subtle overshoot that
    // snaps back cleanly, similar to a mobile scroll bounce.
    double StepSpring(double dt, double k, double dr, bool overshoot) {
        if (overshoot) dr *= 0.65;
        double disp = pos - target;

        double absDisp = std::abs(disp);
        double extraDamp = 0;
        if (absDisp < 1.0) {
            extraDamp = (1.0 - absDisp) / 1.0 * 3.0;
        }

        double c = dr * 2.0 * std::sqrt(k) + extraDamp;
        double f = -k * disp - c * vel;
        vel += f * dt;
        double old = pos;
        pos += vel * dt;
        return pos - old;
    }

    // Smooth-only mode: linear easing, no physics.
    // Each frame moves a fraction of remaining distance.
    double StepSmooth(double factor) {
        double old = pos;
        pos += (target - pos) * factor;
        vel = 0;
        return pos - old;
    }

    void Push(double delta, double k, bool smoothOnly) {
        target += delta;
        if (!smoothOnly) {
            vel += delta * std::sqrt(k) * 0.40;
        }
        // smoothOnly: no velocity boost — easing handles it.
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

// ---------------------------------------------------------------------------
// Per-monitor refresh rate detection
// ---------------------------------------------------------------------------
static int CalcFrameMsFromMonitor(HMONITOR hmon) {
    if (!hmon) return 8;
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hmon, &mi)) return 8;
    DEVMODEW dm{};
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) ||
        dm.dmDisplayFrequency == 0)
        return 8;
    int ms = 1000 / (int)dm.dmDisplayFrequency;
    return ms < 1 ? 1 : ms;
}


static Method Detect(HWND h) {
    WCHAR c[64] = {};
    if (!GetClassNameW(h, c, ARRAYSIZE(c)))
        return Method::Pass;

    if (_wcsicmp(c, L"SysListView32") == 0)  return Method::PixelLV;

    if (_wcsicmp(c, L"DirectUIHWND") == 0) {
        // DirectUIHWND inside NativeHWNDHost is a WinUI-hosted container
        // (e.g. Task Manager tabs) — pass through.
        WCHAR pc[64] = {};
        HWND parent = GetParent(h);
        if (parent) GetClassNameW(parent, pc, ARRAYSIZE(pc));
        if (_wcsicmp(pc, L"NativeHWNDHost") == 0)
            return Method::Pass;
        // Control Panel detection: parent==DUIViewWndClassName but SetScrollPercent
        // is silently ignored by the legacy DirectUI container.
        // We check the ROOT window's WndProc module rather than the DirectUIHWND's
        // own WndProc to avoid an initialization race condition: when Control Panel
        // first opens, the DirectUIHWND's WndProc may not yet be registered by
        // COMCTL32, causing intermittent detection failures on the first scroll.
        // The root CabinetWClass WndProc is assigned at window creation and is
        // stable from the start: COMCTL32.dll for Control Panel, atlthunk.dll for
        // File Explorer (confirmed via window chain diagnostic logging).
        if (_wcsicmp(pc, L"DUIViewWndClassName") == 0) {
            HWND root = GetAncestor(h, GA_ROOT);
            WNDPROC rootWp = root ? (WNDPROC)GetWindowLongPtrW(root, GWLP_WNDPROC) : nullptr;
            HMODULE hmod = nullptr;
            if (rootWp && GetModuleHandleExW(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    (LPCWSTR)rootWp, &hmod)) {
                WCHAR path[MAX_PATH] = {};
                GetModuleFileNameW(hmod, path, ARRAYSIZE(path));
                const WCHAR* sep = wcsrchr(path, L'\\');
                if (sep && _wcsicmp(sep + 1, L"comctl32.dll") == 0)
                    return Method::Pass;
            }
        }
        return Method::UIAPercent;
    }

    if (_wcsicmp(c, L"SysTreeView32") == 0 ||
        _wcsicmp(c, L"Edit") == 0 ||
        _wcsicmp(c, L"ListBox") == 0 ||
        _wcsicmp(c, L"ComboLBox") == 0 ||
        _wcsicmp(c, L"RICHEDIT50W") == 0 ||
        _wcsicmp(c, L"RichEdit20W") == 0 ||
        _wcsicmp(c, L"RichEdit20A") == 0)
        return Method::LineScroll;

    SCROLLINFO si = { sizeof(si), SIF_RANGE };
    if (GetScrollInfo(h, SB_VERT, &si) && si.nMax > si.nMin)
        return Method::LineScroll;

    if (GetWindowLongW(h, GWL_STYLE) & (WS_VSCROLL | WS_HSCROLL))
        return Method::LineScroll;

    return Method::Pass;
}

// ---------------------------------------------------------------------------
// Find scrollable child under cursor
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
// UI Automation
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

// Estimate percent-per-line for vertical and horizontal axes.

static void EstimatePPL(IUIAutomationScrollPattern* p, HWND hwnd,
                         double* outV, double* outH) {
    RECT rc = {};
    GetClientRect(hwnd, &rc);
    int wh = rc.bottom - rc.top;
    int ww = rc.right - rc.left;
    if (wh < 1) wh = 400;
    if (ww < 1) ww = 600;

    // Precompute expensive pow bases once per call (not per-frame — this
    // function is only called on first scroll and on view-mode changes).
    static const double pow70_5  = std::pow(70.0,  5.0);
    static const double pow92_20 = std::pow(92.0, 20.0);
    // Value of power-5 curve at vs=92 — anchor for power-20 curve.
    static const double anchor92 = std::pow(92.0, 5.0) * 196.0 / pow70_5;

    // Vertical ppl.
    double vsV = 0;
    p->get_CurrentVerticalViewSize(&vsV);
    if (vsV > 0) {
        double vs = vsV;
        if      (vs > 25 && vs < 70) vs = vs * vs / 25.0;
        else if (vs >= 70 && vs < 92) vs = std::pow(vs, 5.0) * 196.0 / pow70_5;
        else if (vs >= 92)            vs = std::pow(vs, 20.0) * anchor92 / pow92_20;
        double rows = wh / 24.0;
        if (rows < 3) rows = 3;
        *outV = vs / rows;
    } else {
        *outV = 1.0;
    }

    // Horizontal ppl — use window width and wider column estimate.
    double vsH = 0;
    p->get_CurrentHorizontalViewSize(&vsH);
    if (vsH > 0) {
        double vs = vsH;
        if (vs > 25) vs = vs * vs / 25.0;
        double cols = ww / 120.0;
        if (cols < 3) cols = 3;
        *outH = vs / cols;
    } else {
        *outH = 1.0;
    }
}

// ---------------------------------------------------------------------------
// Per-window state
// ---------------------------------------------------------------------------

struct State {
    int intervalMs = 8;    // per-window, auto-detected from monitor refresh rate
    HMONITOR lastMonitor = nullptr; // cached to avoid re-querying unchanged monitors
    Spring sV, sH;
    UINT_PTR timer = 0;
    Method method = Method::Pass;
    double pplV = 0, pplH = 0;
    double accV = 0, accH = 0;
    IUIAutomationScrollPattern* pat = nullptr;
    bool uiaOk = true;
    bool hasVScroll = true;
    bool hasHScroll = false;
    RECT lastClientRect = {};  // cache key for EstimatePPL: skip when window unchanged
};

static CRITICAL_SECTION g_cs;
static std::map<HWND, State> g_st;

constexpr UINT_PTR TID = 0x534D5448;

static void Release(State& s) {
    if (s.pat) { s.pat->Release(); s.pat = nullptr; }
}

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
    int k = Wh_GetIntSetting(L"spring.springConstant");
    int d = Wh_GetIntSetting(L"spring.dampingX10");
    int m = Wh_GetIntSetting(L"scrollMultiplierX10");
    bool sm = Wh_GetIntSetting(L"smoothOnly") != 0;
    bool ov = Wh_GetIntSetting(L"spring.overshoot") != 0;
    int sp = Wh_GetIntSetting(L"smoothSpeedX10");

    UINT ln = 3;
    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &ln, 0);
    if (ln == 0) ln = 3;
    if (ln == WHEEL_PAGESCROLL) ln = 20;

    double logK, logD, logM, logSp;
    AcquireSRWLockExclusive(&g_cfgLock);
    g_cfg.springK    = logK = (k >= 50 && k <= 1000) ? (double)k : 700.0;
    g_cfg.damping    = logD = (d >= 3  && d <= 30)   ? d / 10.0  : 1.2;
    g_cfg.multiplier = logM = (m >= 1  && m <= 100)  ? m / 10.0  : 2.0;
    g_cfg.smoothOnly = sm;
    g_cfg.overshoot  = ov;
    g_cfg.smoothSpeed= logSp = (sp >= 1 && sp <= 8)  ? sp / 10.0 : 0.3;
    g_sysLines_g     = ln;
    ReleaseSRWLockExclusive(&g_cfgLock);

    Wh_Log(L"Cfg: k=%.0f d=%.2f m=%.1f ov=%d sm=%d sp=%.1f sl=%u",
            logK, logD, logM, (int)ov, (int)sm, logSp, ln);
}

// ---------------------------------------------------------------------------
// Timer
// ---------------------------------------------------------------------------

static void CALLBACK Tick(HWND hw, UINT, UINT_PTR, DWORD) {
    // Snapshot settings before acquiring g_cs: SnapCfg uses its own
    // g_cfgLock (shared), no need to hold both locks simultaneously.
    const CfgSnap cfg = SnapCfg();

    EnterCriticalSection(&g_cs);

    auto it = g_st.find(hw);
    if (it == g_st.end() || !IsWindow(hw)) {
        KillTimer(hw, TID);
        if (it != g_st.end()) { Release(it->second); g_st.erase(it); }
        LeaveCriticalSection(&g_cs);
        return;
    }

    State& s = it->second;

    // Per-monitor refresh: one cheap MonitorFromWindow per frame.
    // GetMonitorInfoW + EnumDisplaySettingsW only run when the window
    // actually moved to a different monitor (HMONITOR changed).
    {
        HMONITOR hmon = MonitorFromWindow(hw, MONITOR_DEFAULTTOPRIMARY);
        if (hmon != s.lastMonitor) {
            s.lastMonitor = hmon;
            s.intervalMs  = CalcFrameMsFromMonitor(hmon);
            KillTimer(hw, TID);
            s.timer = SetTimer(hw, TID, (UINT)s.intervalMs, Tick);
            if (!s.timer) {
                Release(s); g_st.erase(it);
                LeaveCriticalSection(&g_cs);
                return;
            }
        }
    }
    double dt = s.intervalMs / 1000.0;

    double dV, dH;
    if (cfg.smoothOnly) {
        dV = s.sV.StepSmooth(cfg.smoothSpeed);
        dH = s.sH.StepSmooth(cfg.smoothSpeed);
    } else {
        dV = s.sV.StepSpring(dt, cfg.springK, cfg.damping, cfg.overshoot);
        dH = s.sH.StepSpring(dt, cfg.springK, cfg.damping, cfg.overshoot);
    }

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
                double pct = dV * s.pplV;

                // Boundary: snap if at limit in scroll direction.
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
                double pct = dH * sh.pplH;

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

    // Ctrl+wheel = zoom — pass through.
    if (GET_KEYSTATE_WPARAM(m->wParam) & MK_CONTROL)
        return false;

    const CfgSnap cfg = SnapCfg();

    bool vert = (m->message == WM_MOUSEWHEEL);
    HWND tgt = m->hwnd;
    Method mt = Detect(tgt);

    if (mt == Method::Pass) {
        POINT pt = { GET_X_LPARAM(m->lParam), GET_Y_LPARAM(m->lParam) };
        HWND ch = FindChild(tgt, pt);
        if (ch) { tgt = ch; mt = Detect(ch); }
    }
    if (mt == Method::Pass) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(tgt, &pid);
    if (pid != GetCurrentProcessId()) return false;

    // SysListView32 horizontal-only (Icon/Tile): pass through.
    if (mt == Method::PixelLV && vert) {
        SCROLLINFO si = { sizeof(si), SIF_RANGE };
        if (!GetScrollInfo(tgt, SB_VERT, &si) || si.nMax <= si.nMin)
            return false;
    }

    double lines = ((double)delta / WHEEL_DELTA) * cfg.sysLines * cfg.multiplier;
    int lh = (mt == Method::PixelLV) ? LVLineH(tgt) : 0;

    EnterCriticalSection(&g_cs);

    State& s = g_st[tgt];
    s.method = mt;

    // Lazy init UIA pattern — GetPattern is a COM cross-process call,
    // must be done outside g_cs to avoid holding the lock during blocking I/O.
    if (mt == Method::UIAPercent && s.uiaOk && !s.pat) {
        LeaveCriticalSection(&g_cs);
        auto* p = GetPattern(tgt);
        // EstimatePPL is also a COM call — keep it outside the lock.
        double pplV = 1.0, pplH = 1.0;
        if (p) EstimatePPL(p, tgt, &pplV, &pplH);
        EnterCriticalSection(&g_cs);
        auto i2 = g_st.find(tgt);
        if (i2 == g_st.end()) {
            if (p) p->Release();
            LeaveCriticalSection(&g_cs);
            return false;
        }
        i2->second.pat = p;
        if (p) {
            i2->second.pplV = pplV;
            i2->second.pplH = pplH;
        } else {
            i2->second.uiaOk = false;
            LeaveCriticalSection(&g_cs);
            return false;
        }
    }

    // Refresh scroll axes on every event.
    // get_CurrentVerticalScrollPercent is a lightweight in-process call
    // (the COM proxy caches the value), acceptable under the lock.
    if (mt == Method::UIAPercent && s.pat) {
        double pctV = -1, pctH = -1;
        s.pat->get_CurrentVerticalScrollPercent(&pctV);
        s.pat->get_CurrentHorizontalScrollPercent(&pctH);
        s.hasVScroll = (pctV >= 0);
        s.hasHScroll = (pctH >= 0);

        // Re-estimate percent-per-line only when the client area changed
        // (window resize or first call). The view size in UIA is stable
        // between resize events, so calling EstimatePPL on every scroll
        // event is unnecessary COM overhead.
        RECT cr = {};
        GetClientRect(tgt, &cr);
        bool needsPPL = (s.pplV == 0 ||
                         cr.right  != s.lastClientRect.right ||
                         cr.bottom != s.lastClientRect.bottom);

        State* sp = &s;  // will point to re-found state if we leave the lock
        if (needsPPL) {
            auto* pat = s.pat;
            LeaveCriticalSection(&g_cs);
            double pplV = 1.0, pplH = 1.0;
            EstimatePPL(pat, tgt, &pplV, &pplH);
            EnterCriticalSection(&g_cs);
            auto i2 = g_st.find(tgt);
            if (i2 == g_st.end()) {
                LeaveCriticalSection(&g_cs);
                return false;
            }
            i2->second.pplV = pplV;
            i2->second.pplH = pplH;
            i2->second.lastClientRect = cr;
            sp = &i2->second;
        }
        State& s2 = *sp;

        // Spring delta and push using refreshed state.
        double add = -lines; // UIAPercent: up = decrease %
        Spring* pushedSpring = nullptr;
        double pushValue = 0;
        if (vert) {
            if (s2.hasVScroll) {
                pushValue = add;
                s2.sV.Push(pushValue, cfg.springK, cfg.smoothOnly);
                pushedSpring = &s2.sV;
            } else if (s2.hasHScroll) {
                pushValue = add;
                s2.sH.Push(pushValue, cfg.springK, cfg.smoothOnly);
                pushedSpring = &s2.sH;
            }
        } else {
            if (s2.hasHScroll) {
                pushValue = -add;
                s2.sH.Push(pushValue, cfg.springK, cfg.smoothOnly);
                pushedSpring = &s2.sH;
            }
        }
        if (!pushedSpring) { LeaveCriticalSection(&g_cs); return false; }
        if (!s2.timer) {
            s2.lastMonitor = MonitorFromWindow(tgt, MONITOR_DEFAULTTOPRIMARY);
            s2.intervalMs  = CalcFrameMsFromMonitor(s2.lastMonitor);
            s2.timer = SetTimer(tgt, TID, (UINT)s2.intervalMs, Tick);
            if (!s2.timer) {
                pushedSpring->target -= pushValue;
                LeaveCriticalSection(&g_cs);
                return false;
            }
        }
        LeaveCriticalSection(&g_cs);
        return true;
    }

    // Spring delta.
    double add = 0;
    switch (mt) {
    case Method::PixelLV:    add = lines * lh;  break;
    case Method::LineScroll: add = lines;       break;
    case Method::UIAPercent: add = -lines;      break;  // up = decrease %
    default: break;
    }

    // Push to correct axis.
    Spring* pushedSpring = nullptr;
    double pushValue = 0;

    if (mt == Method::UIAPercent) {
        if (vert) {
            if (s.hasVScroll) {
                pushValue = add;
                s.sV.Push(pushValue, cfg.springK, cfg.smoothOnly);
                pushedSpring = &s.sV;
            } else if (s.hasHScroll) {
                pushValue = add;
                s.sH.Push(pushValue, cfg.springK, cfg.smoothOnly);
                pushedSpring = &s.sH;
            }
        } else {
            if (s.hasHScroll) {
                pushValue = -add;
                s.sH.Push(pushValue, cfg.springK, cfg.smoothOnly);
                pushedSpring = &s.sH;
            }
        }
    } else {
        if (vert) {
            pushValue = add;
            s.sV.Push(pushValue, cfg.springK, cfg.smoothOnly);
            pushedSpring = &s.sV;
        } else {
            pushValue = -add;
            s.sH.Push(pushValue, cfg.springK, cfg.smoothOnly);
            pushedSpring = &s.sH;
        }
    }

    if (!pushedSpring) {
        LeaveCriticalSection(&g_cs);
        return false;
    }

    if (!s.timer) {
        s.lastMonitor = MonitorFromWindow(tgt, MONITOR_DEFAULTTOPRIMARY);
        s.intervalMs  = CalcFrameMsFromMonitor(s.lastMonitor);
        s.timer = SetTimer(tgt, TID, (UINT)s.intervalMs, Tick);
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
}
