// ==WindhawkMod==
// @id              smooth-scroll-win32
// @name            Smooth Scroll for Win32
// @name:pt         Rolagem Suave para Win32
// @name:es         Desplazamiento Suave para Win32
// @description     Adds smooth scrolling with spring physics to legacy Win32 apps
// @description:pt  Adiciona rolagem suave com fisica de mola a aplicativos Win32 antigos
// @description:es  Anade desplazamiento suave con fisica de resorte a aplicaciones Win32
// @version         1.0.4
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @include         regedit.exe
// @include         mmc.exe
// @include         control.exe
// @include         wordpad.exe
// @include         taskmgr.exe
// @compilerOptions -luser32 -lgdi32 -lcomctl32 -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smooth Scroll for Win32

## English

Adds smooth scrolling with spring physics to legacy Win32 applications.
Only modifies legacy controls — modern WinUI windows are not affected.

### What's new in 1.0.4

- **Native pixel scrolling in File Explorer** — the folder view now scrolls
  through Explorer's own pixel-precise pan path instead of UI Automation.
  Each notch travels the distance of the Windows wheel step (at most a third
  of the view, so large icons stay easy to follow), works in grouped views
  and no longer brings background windows to the front. It uses Explorer's
  symbols, which Windhawk downloads once; until they are available, UI
  Automation is used as before.
- **Task Manager and Control Panel** — their DirectUI pages now scroll
  smoothly by pixels, and Control Panel pages no longer ignore the wheel
  from time to time.
- **Pixel scrolling in classic Win32 controls** — tree views, list boxes,
  multi-line text boxes and Details-view lists still move by whole lines
  underneath, but every frame is drawn at its pixel offset, so they glide
  instead of jumping a line at a time. A control whose drawing doesn't match
  what it shows keeps line scrolling, and a click or key press during the
  animation finishes it at once. RichEdit boxes and icon views scroll by
  pixels directly, with the native distance per notch.
- **No more legacy list box animation** — list boxes no longer play the old
  per-line Windows animation underneath the mod's.
- **Refresh-rate pacing** — frames follow the compositor clock (or a
  high-resolution timer at the monitor's refresh rate) and each step uses
  the real elapsed time, instead of a ~60 Hz timer.
- **WinUI-style Smooth Only mode** — scrolling eases out with the
  exponential shape of WinUI inertia and settles in about a quarter second.
- **Steady finish** — the last pixels of every scroll move at an even pace
  instead of stepping one pixel every few frames.
- **Simpler settings** — Scroll Multiplier and Smooth Speed were removed.
  The scroll distance follows the Windows "lines per notch" setting.

### Scroll modes (auto-detected per window)

| Control | Method | Granularity |
|---|---|---|
| File Explorer folder view | LineScroller::ScrollPreciseDelta | Pixel |
| DirectUI ScrollViewer (Task Manager) | BaseScrollViewer::SetYOffset | Pixel |
| Other DirectUIHWND | UIA SetScrollPercent | Continuous |
| SysListView32 icon views | LVM_SCROLL | Pixel |
| RichEdit | EM_SETSCROLLPOS | Pixel |
| SysTreeView32, ListBox, multi-line Edit, Details view | Line steps drawn at pixel offsets | Pixel |
| Other scrollable windows | WM_VSCROLL | Line |
| Unknown / modern | Passthrough | Not modified |

### Adding more programs

Go to the mod's **Advanced** tab in Windhawk and add custom
`@include` entries (e.g. `totalcmd.exe`). The mod auto-detects
scrollable controls in any Win32 application. Modern WinUI windows
inside any included process are automatically skipped.

### Known limitations

- Modal dialog boxes, such as the file open/save dialogs, run their own
  message loop inside Windows, which the mod doesn't see, so they keep
  native scrolling.
- Task Manager's WinUI frame, header and settings page are not affected.
- The pixel paths for File Explorer and DirectUI are 64-bit only;
  32-bit programs use UI Automation for `DirectUIHWND` content.
- Where UI Automation is used, scrolling a background window brings it to
  the foreground: `IUIAutomationScrollPattern::SetScrollPercent` activates
  the target window as part of the accessibility model.

---

## Português

Adiciona rolagem suave com física de mola a aplicativos Win32 antigos.
Só modifica controles legados. Janelas WinUI não são afetadas.

### Novidades na 1.0.4

- **Rolagem nativa em pixels no Explorador de Arquivos** — a lista de
  arquivos agora rola pelo próprio caminho de pan em pixels do Explorer, em
  vez de UI Automation. Cada notch percorre a distância do passo da roda do
  Windows (no máximo um terço da área visível, para ícones grandes ficarem
  fáceis de acompanhar), funciona em exibições agrupadas e não traz mais
  janelas em segundo plano para a frente. Usa os símbolos do Explorer, que o
  Windhawk baixa uma vez; enquanto não estão disponíveis, a UI Automation é
  usada como antes.
- **Gerenciador de Tarefas e Painel de Controle** — as páginas DirectUI
  agora rolam suave, em pixels, e as páginas do Painel de Controle não
  ignoram mais a roda de vez em quando.
- **Rolagem em pixels nos controles Win32 clássicos** — árvores, caixas de
  lista, caixas de texto de várias linhas e listas em Detalhes continuam
  andando por linhas inteiras por baixo, mas cada quadro é desenhado no seu
  deslocamento em pixels, então deslizam em vez de pular uma linha por vez.
  Um controle cujo desenho não bate com o que ele mostra continua rolando por
  linhas, e um clique ou tecla durante a animação a termina na hora. Caixas
  RichEdit e exibições de ícones rolam direto em pixels, com a distância
  nativa por notch.
- **Fim da animação antiga das caixas de lista** — as caixas de lista não
  tocam mais a animação por linha antiga do Windows por baixo da do mod.
- **Ritmo pela taxa de atualização** — os quadros seguem o relógio do
  compositor (ou um timer de alta resolução na frequência do monitor) e cada
  passo usa o tempo real decorrido, em vez de um timer de ~60 Hz.
- **Modo Apenas Suave no estilo WinUI** — a rolagem desacelera com a forma
  exponencial da inércia do WinUI e assenta em cerca de um quarto de segundo.
- **Final estável** — os últimos pixels de cada rolagem andam em ritmo
  uniforme, em vez de avançar um pixel a cada vários quadros.
- **Configurações mais simples** — Multiplicador de rolagem e Velocidade
  Suave foram removidos. A distância segue a configuração do Windows de
  linhas por notch.

### Modos de rolagem (detectados por janela)

| Controle | Método | Granularidade |
|---|---|---|
| Lista de arquivos do Explorador | LineScroller::ScrollPreciseDelta | Pixel |
| ScrollViewer DirectUI (Gerenciador de Tarefas) | BaseScrollViewer::SetYOffset | Pixel |
| Outros DirectUIHWND | UIA SetScrollPercent | Contínua |
| Exibições de ícones do SysListView32 | LVM_SCROLL | Pixel |
| RichEdit | EM_SETSCROLLPOS | Pixel |
| SysTreeView32, ListBox, Edit de várias linhas, Detalhes | Passos de linha desenhados em pixels | Pixel |
| Outras janelas roláveis | WM_VSCROLL | Linha |
| Desconhecidas / modernas | Sem alteração | Não modificado |

### Adicionando mais programas

Vá na aba **Avançado** do mod no Windhawk e adicione entradas
`@include` (ex: `totalcmd.exe`). O mod detecta automaticamente
controles roláveis em qualquer aplicativo Win32. Janelas WinUI
dentro de qualquer processo incluído são ignoradas automaticamente.

### Limitações conhecidas

- Caixas de diálogo modais, como os diálogos de abrir/salvar arquivo,
  rodam um laço de mensagens próprio dentro do Windows, que o mod não
  enxerga, então mantêm a rolagem nativa.
- A moldura, o cabeçalho e a página de configurações do Gerenciador de
  Tarefas, que são WinUI, não são afetados.
- Os caminhos em pixels do Explorador e do DirectUI são só de 64 bits;
  programas de 32 bits usam UI Automation no conteúdo `DirectUIHWND`.
- Onde a UI Automation é usada, rolar uma janela em segundo plano a traz
  para a frente: `IUIAutomationScrollPattern::SetScrollPercent` ativa a
  janela alvo como parte do modelo de acessibilidade.

---

## Español

Añade desplazamiento suave con física de resorte a las aplicaciones Win32 antiguas.
Solo modifica los controles antiguos. Las ventanas de WinUI no se ven afectadas.

### Novedades en 1.0.4

- **Desplazamiento nativo en píxeles en el Explorador de archivos** — la
  vista de carpetas ahora se desplaza por la propia ruta de paneo en píxeles
  del Explorador, en lugar de UI Automation. Cada notch recorre la distancia
  del paso de la rueda de Windows (como máximo un tercio de la vista, para
  que los iconos grandes sean fáciles de seguir), funciona en vistas
  agrupadas y ya no trae al frente las ventanas en segundo plano. Usa los
  símbolos del Explorador, que Windhawk descarga una vez; mientras no están
  disponibles, se usa UI Automation como antes.
- **Administrador de tareas y Panel de control** — sus páginas DirectUI
  ahora se desplazan suavemente, en píxeles, y las páginas del Panel de
  control ya no ignoran la rueda de vez en cuando.
- **Desplazamiento en píxeles en los controles Win32 clásicos** — árboles,
  cuadros de lista, cuadros de texto de varias líneas y listas en Detalles
  siguen moviéndose por líneas enteras por debajo, pero cada fotograma se
  dibuja en su desplazamiento en píxeles, así que se deslizan en lugar de
  saltar una línea a la vez. Un control cuyo dibujo no coincide con lo que
  muestra sigue desplazándose por líneas, y un clic o una tecla durante la
  animación la termina al instante. Los cuadros RichEdit y las vistas de
  iconos se desplazan directamente en píxeles, con la distancia nativa por
  notch.
- **Adiós a la animación antigua de los cuadros de lista** — los cuadros de
  lista ya no reproducen la antigua animación por línea de Windows debajo de
  la del mod.
- **Ritmo según la frecuencia de actualización** — los fotogramas siguen el
  reloj del compositor (o un temporizador de alta resolución a la frecuencia
  del monitor) y cada paso usa el tiempo real transcurrido, en lugar de un
  temporizador de ~60 Hz.
- **Modo Solo Suave al estilo WinUI** — el desplazamiento desacelera con la
  forma exponencial de la inercia de WinUI y se asienta en cerca de un cuarto
  de segundo.
- **Final estable** — los últimos píxeles de cada desplazamiento avanzan a
  ritmo uniforme, en lugar de un píxel cada varios fotogramas.
- **Ajustes más simples** — se eliminaron Multiplicador de desplazamiento y
  Velocidad Suave. La distancia sigue el ajuste de Windows de líneas por notch.

### Modos de desplazamiento (detectados por ventana)

| Control | Método | Granularidad |
|---|---|---|
| Vista de carpetas del Explorador | LineScroller::ScrollPreciseDelta | Píxel |
| ScrollViewer DirectUI (Administrador de tareas) | BaseScrollViewer::SetYOffset | Píxel |
| Otros DirectUIHWND | UIA SetScrollPercent | Continua |
| Vistas de iconos de SysListView32 | LVM_SCROLL | Píxel |
| RichEdit | EM_SETSCROLLPOS | Píxel |
| SysTreeView32, ListBox, Edit de varias líneas, Detalles | Pasos de línea dibujados en píxeles | Píxel |
| Otras ventanas desplazables | WM_VSCROLL | Línea |
| Desconocidas / modernas | Sin cambios | No modificado |

### Agregar más programas

Ve a la pestaña **Avanzado** del mod en Windhawk y añade entradas
`@include` (p. ej., `totalcmd.exe`). El mod detecta automáticamente
los controles desplazables en cualquier aplicación Win32. Las ventanas
WinUI dentro de cualquier proceso incluido se omiten automáticamente.

### Limitaciones conocidas

- Los cuadros de diálogo modales, como los de abrir/guardar archivo,
  ejecutan su propio bucle de mensajes dentro de Windows, que el mod no
  ve, así que mantienen el desplazamiento nativo.
- El marco, el encabezado y la página de configuración del Administrador de
  tareas, que son WinUI, no se ven afectados.
- Las rutas en píxeles del Explorador y de DirectUI son solo de 64 bits;
  los programas de 32 bits usan UI Automation en el contenido `DirectUIHWND`.
- Donde se usa UI Automation, desplazar una ventana en segundo plano la trae
  al frente: `IUIAutomationScrollPattern::SetScrollPercent` activa la
  ventana objetivo como parte del modelo de accesibilidad.
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

- smoothOnly: false
  $name: Smooth Only (no spring)
  $name:pt: Apenas Suave (sem mola)
  $name:es: Solo Suave (sin resorte)
  $description: Disables spring physics. Scrolling eases out like WinUI and settles in about a quarter second — no bounce, no overshoot
  $description:pt: Desativa a física de mola. A rolagem desacelera como no WinUI e assenta em cerca de um quarto de segundo — sem bounce, sem ultrapassagem
  $description:es: Desactiva la física de resorte. El desplazamiento desacelera como en WinUI y se asienta en cerca de un cuarto de segundo — sin rebote, sin sobreimpulso

*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <richedit.h>
#include <windowsx.h>
#include <objbase.h>
#include <uiautomation.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <initializer_list>
#include <map>
#include <thread>

#ifndef UIA_ScrollPatternNoScroll
constexpr double UIA_ScrollPatternNoScroll = -1.0;
#endif

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct {
    double springK;
    double damping;
    bool smoothOnly;
    bool overshoot;      // underdamp the spring for a subtle bounce
} g_cfg;

// Smooth Only eases out exponentially, the shape of WinUI's inertia, and
// covers 95% of a notch in 0.25 s (ln(20) / 0.25).
constexpr double kSmoothRate = 11.98292909421596;
// Slowest tail pace in pixels per second at 96 DPI: the view moves in whole
// pixels, so a slower finish would advance one pixel every few frames and
// visibly stutter.
constexpr double kTailMinPxPerSec = 160.0;

// Protects g_cfg and g_sysLines against concurrent reads (Tick thread)
// and writes (LoadSettings called from Wh_ModSettingsChanged).
static SRWLOCK g_cfgLock = SRWLOCK_INIT;
static UINT g_sysLines_g = 3; // renamed to avoid collision with local in LoadSettings

// Snapshot of settings read atomically under shared lock.
// Tick and Handle call this once per invocation to avoid repeated lock acquires.
struct CfgSnap {
    double springK, damping;
    bool smoothOnly;
    bool overshoot;
    UINT sysLines;
};

static CfgSnap SnapCfg() {
    AcquireSRWLockShared(&g_cfgLock);
    CfgSnap s;
    s.springK    = g_cfg.springK;
    s.damping    = g_cfg.damping;
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
// Both modes advance by the real elapsed time of the frame.
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
        double old = pos;
        // Fixed 4 ms substeps keep the explicit integration stable at any frame time.
        int n = (int)std::ceil(dt / 0.004);
        if (n < 1) n = 1;
        double h = dt / n;
        for (int i = 0; i < n; i++) {
            double disp = pos - target;
            double absDisp = std::abs(disp);
            double extraDamp = 0;
            if (absDisp < 1.0) {
                extraDamp = (1.0 - absDisp) / 1.0 * 3.0;
            }
            double c = dr * 2.0 * std::sqrt(k) + extraDamp;
            double f = -k * disp - c * vel;
            vel += f * h;
            pos += vel * h;
        }
        return pos - old;
    }

    // Smooth-only mode: exponential approach to the target.
    double StepSmooth(double rate, double dt) {
        double old = pos;
        pos += (target - pos) * (1.0 - std::exp(-rate * dt));
        vel = 0;
        return pos - old;
    }

    // Within the last tenth of a second of travel, a step slower than
    // minSpeed (units per second) is raised to it, landing exactly on the
    // target. Steps moving away from it (overshoot) are left alone.
    double FloorTail(double before, double minSpeed, double dt) {
        double rem = target - before;
        double step = pos - before;
        double minStep = minSpeed * dt;
        if (minSpeed <= 0 || std::abs(rem) > minSpeed * 0.1 || std::abs(step) >= minStep)
            return step;
        if (step != 0 && (step > 0) != (rem > 0))
            return step;
        step = std::abs(rem) <= minStep ? rem : (rem > 0 ? minStep : -minStep);
        pos = before + step;
        if (pos == target) vel = 0;
        return step;
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
    DuiPrecise  = 4,   // Explorer items view: LineScroller::ScrollPreciseDelta
    DuiViewer   = 5,   // classic DirectUI::ScrollViewer: BaseScrollViewer::SetYOffset
    PixelRE     = 6,   // RichEdit: EM_SETSCROLLPOS
    LineComp    = 7,   // stock line controls: whole-line moves composed at pixel offsets
};

// Set once the DirectUI items-view / ScrollViewer hooks are applied (x64 only).
static std::atomic<bool> g_duiLive{false};
static std::atomic<bool> g_duiViewerLive{false};

// ---------------------------------------------------------------------------
// Per-monitor refresh rate detection
// ---------------------------------------------------------------------------
static int FramePeriodUs(HMONITOR hmon) {
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    DEVMODEW dm{};
    dm.dmSize = sizeof(dm);
    if (!hmon || !GetMonitorInfoW(hmon, &mi) ||
        !EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) ||
        dm.dmDisplayFrequency < 24)
        return 16667;
    return (int)(1000000 / dm.dmDisplayFrequency);
}


static Method Detect(HWND h) {
    WCHAR c[64] = {};
    if (!GetClassNameW(h, c, ARRAYSIZE(c)))
        return Method::Pass;

    // Details view rounds LVM_SCROLL to whole rows, so it scrolls by lines.
    if (_wcsicmp(c, L"SysListView32") == 0)
        return (GetWindowLongW(h, GWL_STYLE) & LVS_TYPEMASK) == LVS_REPORT
                   ? Method::LineScroll : Method::PixelLV;

    if (_wcsicmp(c, L"RICHEDIT50W") == 0 ||
        _wcsicmp(c, L"RichEdit20W") == 0 ||
        _wcsicmp(c, L"RichEdit20A") == 0)
        return Method::PixelRE;

    if (_wcsicmp(c, L"DirectUIHWND") == 0) {
        // DirectUIHWND inside NativeHWNDHost is a WinUI-hosted container
        // (e.g. Task Manager tabs) — pass through.
        WCHAR pc[64] = {};
        HWND parent = GetParent(h);
        if (parent) GetClassNameW(parent, pc, ARRAYSIZE(pc));
        if (_wcsicmp(pc, L"NativeHWNDHost") == 0)
            return Method::Pass;
        // Explorer's items view scrolls by pixels in the LineScroller::OnInput
        // hook; the wheel must reach DirectUI untouched.
        if (_wcsicmp(pc, L"SHELLDLL_DefView") == 0 &&
            g_duiLive.load(std::memory_order_acquire))
            return Method::Pass;
        // DirectUI pages (Control Panel) scroll through the ScrollViewer hook.
        // The root window test below depends on who subclasses the frame,
        // so it does not reliably tell them from the items view.
        if (_wcsicmp(pc, L"DUIViewWndClassName") == 0 &&
            g_duiViewerLive.load(std::memory_order_acquire))
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
        _wcsicmp(c, L"ComboLBox") == 0)
        return Method::LineScroll;

    if (GetWindowLongW(h, GWL_STYLE) & (WS_VSCROLL | WS_HSCROLL))
        return Method::LineScroll;

    // A hidden bar still scrolls when the window set its page; one that was
    // never set up reports the default 0-100 range with no page.
    SCROLLINFO si = { sizeof(si), SIF_RANGE | SIF_PAGE };
    if (GetScrollInfo(h, SB_VERT, &si) && si.nMax > si.nMin && si.nPage > 0)
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
// Line sizes of the pixel-scrolled controls
// ---------------------------------------------------------------------------

// Icon and small icon views scroll half the large-icon spacing per line,
// whatever the icon size (measured against the native wheel).
static int LVLineH(HWND h) {
    int cy = HIWORD(SendMessageW(h, LVM_GETITEMSPACING, FALSE, 0)) / 2;
    if (cy > 0) return cy;
    UINT dpi = GetDpiForWindow(h);
    return MulDiv(20, dpi ? dpi : 96, 96);
}

// RichEdit's pixel position, furthest position and the height of its first
// visible line. Fails where the position isn't the scroll bar's (older
// RichEdit reports 16-bit positions), so those keep line scrolling.
static bool REMetrics(HWND h, int* y, int* maxY, int* lineH) {
    SCROLLINFO si = { sizeof(si), SIF_ALL };
    if (!GetScrollInfo(h, SB_VERT, &si) || si.nMax <= si.nMin) return false;
    POINT p = {};
    SendMessageW(h, EM_GETSCROLLPOS, 0, (LPARAM)&p);
    if (p.y != si.nPos) return false;
    LRESULT first = SendMessageW(h, EM_GETFIRSTVISIBLELINE, 0, 0);
    LRESULT c0 = SendMessageW(h, EM_LINEINDEX, first, 0);
    LRESULT c1 = SendMessageW(h, EM_LINEINDEX, first + 1, 0);
    if (c0 < 0 || c1 <= c0) return false;
    POINTL p0 = {}, p1 = {};
    SendMessageW(h, EM_POSFROMCHAR, (WPARAM)&p0, c0);
    SendMessageW(h, EM_POSFROMCHAR, (WPARAM)&p1, c1);
    *y = p.y;
    *maxY = std::max(si.nMax - std::max((int)si.nPage - 1, 0), si.nMin);
    *lineH = p1.y - p0.y;
    return *lineH > 0;
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

enum class CompKind { Tree, List, Edit, LV };

// Line composition of one window (see "Line composition" below). Used only
// on that window's thread, except `live` and `land`, guarded by g_cs.
struct Comp {
    HWND hwnd = nullptr;
    CompKind kind = CompKind::List;
    int lh = 0;                 // pixels per line
    SIZE size = {};             // client size of the captures
    RECT area = {};             // part of the client that scrolls
    HDC dc[2] = {};
    HBITMAP bm[2] = {};
    HGDIOBJ old[2] = {};
    int cap[2] = { -1, -1 };    // top line captured in each slot
    double pos = 0;             // top line on screen, fractional
    int maxTop = 0;
    int moving = 0;             // inside our own moves and captures
    bool caretHidden = false;
    bool subclassed = false;
    bool live = true;           // false once retired; ends at the next WM_PAINT
    int land = 0;               // line to finish on once retired
};

struct State {
    int periodUs = 16667;  // frame period of the window's monitor
    double minSpeed = 0;   // tail pace floor, in the method's units per second
    Spring sV, sH;
    bool active = false;       // animating; the pacer posts ticks
    bool tickPending = false;  // a tick message is queued for this window
    LONGLONG lastStepQpc = 0;
    LONGLONG postedQpc = 0;
    Method method = Method::Pass;
    double pplV = 0, pplH = 0;
    double accV = 0, accH = 0;
    IUIAutomationScrollPattern* pat = nullptr;
    bool uiaOk = true;
    bool hasVScroll = true;
    bool hasHScroll = false;
    RECT lastClientRect = {};  // cache key for EstimatePPL: skip when window unchanged
    void* scroller = nullptr;  // DuiPrecise/DuiViewer element, only while animating
    bool duiHorizontal = false;
    bool duiRtl = false;
    int duiStep = 0;           // DuiPrecise: measured distance of one notch
    int duiStepDir = 0;        // sign of the wheel delta it was measured for
    int duiStepExtent = 0;     // view size it was measured at
    int viewerApplied = 0;     // DuiViewer/PixelRE: last absolute position written
    Comp* comp = nullptr;      // LineComp; owned by the window's thread
};

static CRITICAL_SECTION g_cs;
static std::map<HWND, State> g_st;
static std::atomic<bool> g_unloading{false};

// Number of states holding a LineScroller pointer; lets the hot
// Element::OnDestroy hook skip the lock when nothing is tracked.
static std::atomic<int> g_duiTracked{0};

static void SetScroller(State& s, void* scroller) {
    if (s.scroller == scroller) return;
    if (s.scroller) g_duiTracked.fetch_sub(1, std::memory_order_acq_rel);
    if (scroller)   g_duiTracked.fetch_add(1, std::memory_order_acq_rel);
    s.scroller = scroller;
}

// Any thread, under g_cs: the composition ends on its window's thread at
// the next WM_PAINT, with the control on `line`.
static void Retire(State& s, int line) {
    Comp* c = s.comp;
    if (!c || !c->live) return;
    c->live = false;
    c->land = line;
    InvalidateRect(c->hwnd, nullptr, FALSE);
}

static int CompNearest(const Comp& c, double t);

static void Release(State& s) {
    if (s.pat) { s.pat->Release(); s.pat = nullptr; }
    SetScroller(s, nullptr);
    if (s.comp) Retire(s, CompNearest(*s.comp, s.comp->pos));
}

static void Stop(State& s) {
    s.sV.Snap(); s.sH.Snap();
    s.accV = s.accH = 0;
    s.active = false;
    if (s.method == Method::DuiPrecise || s.method == Method::DuiViewer)
        SetScroller(s, nullptr);
    Retire(s, (int)std::lround(s.sV.target));
}

static bool Settled(const State& s) {
    double tV, tH, tVel;
    switch (s.method) {
    case Method::LineScroll: tV = tH = 0.4; tVel = 2.0; break;
    case Method::LineComp:   tV = tH = 0.04; tVel = 0.15; break;  // lines, under a pixel
    case Method::PixelLV:
    case Method::PixelRE:
    case Method::DuiPrecise:
    case Method::DuiViewer:  tV = tH = 0.8; tVel = 3.0; break;
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

// Idle states of windows on this thread are dropped once the map grows;
// UIA patterns must be released on the thread that created them.
static void PruneStates() {
    constexpr size_t kMaxStates = 64;
    if (g_st.size() <= kMaxStates) return;
    DWORD tid = GetCurrentThreadId();
    for (auto it = g_st.begin(); it != g_st.end(); ) {
        if (!it->second.active && !it->second.tickPending && !it->second.comp &&
            (!IsWindow(it->first) || GetWindowThreadProcessId(it->first, nullptr) == tid)) {
            Release(it->second);
            it = g_st.erase(it);
        } else {
            ++it;
        }
    }
}

// ---------------------------------------------------------------------------
// Load settings
// ---------------------------------------------------------------------------

static void LoadSettings() {
    int k = Wh_GetIntSetting(L"spring.springConstant");
    int d = Wh_GetIntSetting(L"spring.dampingX10");
    bool sm = Wh_GetIntSetting(L"smoothOnly") != 0;
    bool ov = Wh_GetIntSetting(L"spring.overshoot") != 0;

    UINT ln = 3;
    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &ln, 0);
    if (ln == 0) ln = 3;
    if (ln == WHEEL_PAGESCROLL) ln = 20;

    double logK, logD;
    AcquireSRWLockExclusive(&g_cfgLock);
    g_cfg.springK    = logK = (k >= 50 && k <= 1000) ? (double)k : 700.0;
    g_cfg.damping    = logD = (d >= 3  && d <= 30)   ? d / 10.0  : 1.2;
    g_cfg.smoothOnly = sm;
    g_cfg.overshoot  = ov;
    g_sysLines_g     = ln;
    ReleaseSRWLockExclusive(&g_cfgLock);

    Wh_Log(L"Cfg: k=%.0f d=%.2f ov=%d sm=%d sl=%u",
            logK, logD, (int)ov, (int)sm, ln);
}

// ---------------------------------------------------------------------------
// DirectUI — native pixel scrolling
//
// Explorer's items view (ExplorerFrame LineScroller) turns the wheel into
// whole-row jumps, but its pan-gesture path, ScrollPreciseDelta, moves by
// pixels and reports the part of the delta that fell past an edge.
// Classic DirectUI::ScrollViewer (Task Manager, Control Panel pages) keeps
// its scroll position as a pixel YOffset. Wheel events are taken in each
// OnInput and replayed frame by frame through those paths.
// ---------------------------------------------------------------------------

#ifdef _WIN64
using LineScroller_OnInput_t = void (*)(void* self, void* inputEvent);
using LineScroller_ScrollPreciseDelta_t = void (*)(void* self, const SIZE* delta, SIZE* unapplied);
using LineScroller_GetInt_t = int (*)(void* self);
using LineScroller_RowFromY_t = long (*)(void* self, int y, void* rowInfo, int unused);
using LineScroller_NextRow_t = int (*)(void* self, const void* rowInfo, int dir, void* nextRowInfo);
using Element_OnDestroy_t = void (*)(void* self);
using Element_IsRTL_t = bool (*)(void* self);
using ScrollViewer_OnInput_t = void (*)(void* self, void* inputEvent);
using ScrollViewer_GetYOffset_t = int (*)(void* self);
using ScrollViewer_SetYOffset_t = long (*)(void* self, int offset);
using ScrollViewer_GetVScroll_t = void* (*)(void* self);

static LineScroller_OnInput_t g_lsOnInputOrig = nullptr;
static LineScroller_ScrollPreciseDelta_t g_lsScrollPreciseDelta = nullptr;
static LineScroller_GetInt_t g_lsGetVirtualDirection = nullptr;
static LineScroller_RowFromY_t g_lsRowFromY = nullptr;
static LineScroller_NextRow_t g_lsNextRow = nullptr;
static Element_OnDestroy_t g_elementOnDestroyOrig = nullptr;
static Element_IsRTL_t g_elementIsRTL = nullptr;
static ScrollViewer_OnInput_t g_svOnInputOrig = nullptr;
static ScrollViewer_GetYOffset_t g_svGetYOffset = nullptr;
static ScrollViewer_SetYOffset_t g_svSetYOffset = nullptr;
static ScrollViewer_GetVScroll_t g_svGetVScroll = nullptr;
static ScrollViewer_GetVScroll_t g_styledSvGetVScroll = nullptr;

// LineScroller RowInfo (0x78 bytes, 16-byte aligned): +0x18 section
// (-1 when unset, as LineScroller initializes it), +0x20 valid,
// +0x58 row index, +0x64 row RECT in viewer coordinates.
struct alignas(16) RowInfoBuf {
    BYTE b[0x80] = {};
    RowInfoBuf() { *(int*)(b + 0x18) = -1; }
};
#endif

// Lifetime (Element::OnDestroy) and ScrollViewer hooks need only dui70;
// the items view also needs ExplorerFrame's symbols.
static std::atomic<bool> g_duiCoreDone{false};
static std::atomic<bool> g_duiItemsDone{false};
static HMODULE g_explorerFrameRef = nullptr;
static HMODULE g_dui70Ref = nullptr;

// The window whose WM_MOUSEWHEEL this thread is dispatching.
static thread_local HWND t_wheelHwnd = nullptr;
static thread_local int t_ownScroll = 0;  // inside our own line steps

static bool StartAnimation(State& s, HWND hwnd);

#ifdef _WIN64
static void ForgetScroller(void* element) {
    EnterCriticalSection(&g_cs);
    for (auto& p : g_st) {
        if (p.second.scroller == element) Stop(p.second);
    }
    LeaveCriticalSection(&g_cs);
}

static void Element_OnDestroy_Hook(void* self) {
    if (g_duiTracked.load(std::memory_order_acquire) > 0)
        ForgetScroller(self);
    g_elementOnDestroyOrig(self);
}

static bool DuiWheelHost(HWND* host) {
    *host = t_wheelHwnd;
    return *host && IsWindow(*host) &&
           GetWindowThreadProcessId(*host, nullptr) == GetCurrentThreadId();
}

// Leading edge of a visual row, through GetRowInfoForNextRow from the
// neighbouring index so every row is measured in the same coordinates.
static bool RowEdge(void* scroller, int row, bool horizontal, int* edge) {
    if (row < 0) return false;
    RowInfoBuf from{}, to{};
    int dir = 1;
    *(int*)(from.b + 0x58) = row - 1;
    if (row == 0) {
        *(int*)(from.b + 0x58) = 1;
        dir = -1;
    }
    if (!g_lsNextRow(scroller, from.b, dir, to.b) || !*(int*)(to.b + 0x20))
        return false;
    const RECT* r = (const RECT*)(to.b + 0x64);
    *edge = horizontal ? r->left : r->top;
    return true;
}

struct RowMark { int row; int edge; };

// Leading edges of the rows laid out from the top of the viewer down;
// rows outside the realized range report no geometry.
static int CaptureRows(void* scroller, bool horizontal, RowMark* marks, int max) {
    RowInfoBuf ri;
    if (g_lsRowFromY(scroller, 0, ri.b, 0) < 0 || !*(int*)(ri.b + 0x20))
        return 0;
    int n = 0;
    for (int row = *(int*)(ri.b + 0x58); n < max; row++) {
        int edge;
        if (!RowEdge(scroller, row, horizontal, &edge)) break;
        marks[n++] = { row, edge };
    }
    return n;
}

// Moves the view toward the start by `px` (negative = toward the end).
static int PreciseScroll(void* scroller, bool horizontal, bool rtl, int px) {
    // The pan path takes screen deltas, so horizontal ones are mirrored for RTL.
    int screen = horizontal && rtl ? -px : px;
    SIZE d = horizontal ? SIZE{ screen, 0 } : SIZE{ 0, screen };
    SIZE unapplied = {};
    g_lsScrollPreciseDelta(scroller, &d, &unapplied);
    int applied = screen - (horizontal ? unapplied.cx : unapplied.cy);
    return horizontal && rtl ? -applied : applied;
}

// The native handler is the distance oracle, as for the classic
// ScrollViewer: it jumps, a row laid out both before and after the jump
// gives the distance, ScrollPreciseDelta moves back before anything paints,
// and that distance is animated. Returns false only if the native handler
// did not run.
static bool DuiWheel(void* scroller, void* inputEvent) {
    HWND host;
    if (!DuiWheelHost(&host)) return false;

    const CfgSnap cfg = SnapCfg();
    bool horizontal = g_lsGetVirtualDirection(scroller) == 1;
    bool rtl = g_elementIsRTL && g_elementIsRTL(scroller);

    const short delta = *(const short*)((const BYTE*)inputEvent + 0x30);
    const int notches = delta / WHEEL_DELTA;
    const bool wholeNotches = notches && notches * WHEEL_DELTA == delta;
    const int dir = delta > 0 ? 1 : -1;
    RECT rc = {};
    GetClientRect(host, &rc);
    const int extent = horizontal ? rc.right : rc.bottom;

    // Mouse notches that continue a running animation reuse its measured
    // step: the native jump realizes rows twice, tens of ms in heavy views.
    int undone = 0;
    bool reused = false;
    if (wholeNotches) {
        EnterCriticalSection(&g_cs);
        auto it = g_st.find(host);
        if (it != g_st.end()) {
            const State& p = it->second;
            if (p.active && p.method == Method::DuiPrecise && p.scroller == scroller &&
                p.duiHorizontal == horizontal && p.duiStep && p.duiStepDir == dir &&
                p.duiStepExtent == extent) {
                undone = p.duiStep * std::abs(notches);
                reused = true;
            }
        }
        LeaveCriticalSection(&g_cs);
    }

    bool found = reused;
    if (!reused) {
        RowMark marks[64];
        int count = CaptureRows(scroller, horizontal, marks, ARRAYSIZE(marks));
        g_lsOnInputOrig(scroller, inputEvent);

        // Positive when the native jump moved the content toward the end.
        int moved = 0;
        for (int i = 0; i < count && !found; i++) {
            int edge;
            if (RowEdge(scroller, marks[i].row, horizontal, &edge)) {
                moved = marks[i].edge - edge;
                found = true;
            }
        }
        undone = found && moved ? PreciseScroll(scroller, horizontal, rtl, moved) : 0;
    }

    EnterCriticalSection(&g_cs);
    State& s = g_st[host];
    if (s.method != Method::DuiPrecise || s.scroller != scroller) {
        Release(s);
        s.sV = Spring{}; s.sH = Spring{};
        s.accV = s.accH = 0;
        s.method = Method::DuiPrecise;
        s.active = false;
        s.duiStep = 0;
    }
    if (!reused) {
        // Touchpad deltas build up natively, so only whole notches set a step.
        bool keep = wholeNotches && undone;
        s.duiStep = keep ? undone / std::abs(notches) : 0;
        s.duiStepDir = dir;
        s.duiStepExtent = extent;
    }
    if (!undone) {
        // Nothing to animate, or a jump past every laid-out row: the native
        // position stands, and a running animation must not fight it.
        if (!found && s.active) Stop(s);
        LeaveCriticalSection(&g_cs);
        return true;
    }
    // The native step is whole rows, up to most of a page with tall rows
    // (large icons); a notch travels at most a third of the viewport.
    int cap = extent / 3;
    int travel = undone;
    if (cap > 0 && std::abs(travel) > cap) travel = travel > 0 ? cap : -cap;

    SetScroller(s, scroller);
    s.duiHorizontal = horizontal;
    s.duiRtl = rtl;
    s.sV.Push(-travel, cfg.springK, cfg.smoothOnly);
    bool ok = StartAnimation(s, host);
    if (!ok) {
        s.sV.target += travel;
        Stop(s);
    }
    PruneStates();
    LeaveCriticalSection(&g_cs);
    if (!ok) PreciseScroll(scroller, horizontal, rtl, -undone);
    return true;
}

// DirectUI InputEvent (x64): +0x08 handled flag, +0x0C device (0/2 mouse),
// +0x10 mouse code, +0x14 == 5 for the wheel, +0x30 wheel delta.
static bool IsPlainWheel(const BYTE* e) {
    return e && !e[8] &&
           (*(const DWORD*)(e + 0x0C) & ~2u) == 0 &&
           *(const DWORD*)(e + 0x10) == 0 && *(const DWORD*)(e + 0x14) == 5 &&
           *(const short*)(e + 0x30) != 0 &&
           GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0 &&
           !g_unloading.load(std::memory_order_acquire);
}

static void LineScroller_OnInput_Hook(void* self, void* inputEvent) {
    auto* e = (BYTE*)inputEvent;
    if (g_duiLive.load(std::memory_order_acquire) && IsPlainWheel(e) &&
        DuiWheel(self, inputEvent)) {
        e[8] = 1;
        return;
    }
    g_lsOnInputOrig(self, inputEvent);
}

// Follows `jmp [rip+x]` import thunks, which modules use in their own
// vtables for virtuals imported from dui70.
static void* ResolveImportThunk(void* p) {
    auto* b = (const BYTE*)p;
    for (int i = 0; i < 2 && b; i++) {
        int skip = (b[0] == 0x48 && b[1] == 0xFF && b[2] == 0x25) ? 1 : 0;
        if (b[skip] != 0xFF || b[skip + 1] != 0x25) break;
        LONG rel = *(const LONG*)(b + skip + 2);
        b = *(BYTE* const*)(b + skip + 6 + rel);
    }
    return (void*)b;
}

// BaseScrollViewer::OnInput reaches the scroll bar through this vtable
// slot. Only viewers whose slot holds ScrollViewer's or StyledScrollViewer's
// own GetVScroll are classic; TouchScrollViewer (DirectManipulation) and
// unknown layouts keep the native wheel.
static bool IsClassicScrollViewer(void* self) {
    constexpr size_t kGetVScrollSlot = 0x180 / sizeof(void*);
    void* slot = ResolveImportThunk((*(void***)self)[kGetVScrollSlot]);
    return slot && (slot == (void*)g_svGetVScroll || slot == (void*)g_styledSvGetVScroll);
}

// The native handler is the distance oracle: it steps from the animation's
// target, the result is read back and the viewer restored before anything
// paints, so notches keep native line size, page mode and clamping.
static bool ViewerWheel(void* viewer, void* inputEvent) {
    HWND host;
    if (!DuiWheelHost(&host) || !IsClassicScrollViewer(viewer)) return false;

    const CfgSnap cfg = SnapCfg();
    int current = g_svGetYOffset(viewer);

    EnterCriticalSection(&g_cs);
    State& s = g_st[host];
    bool fresh = s.method != Method::DuiViewer || s.scroller != viewer || !s.active;
    int from = fresh ? current : (int)std::lround(s.sV.target);
    LeaveCriticalSection(&g_cs);

    if (from != current) g_svSetYOffset(viewer, from);
    g_svOnInputOrig(viewer, inputEvent);
    int dest = g_svGetYOffset(viewer);
    if (dest != current) g_svSetYOffset(viewer, current);
    if (dest == from) return true;  // at an edge; the event is handled either way

    EnterCriticalSection(&g_cs);
    State& s2 = g_st[host];
    if (fresh || s2.method != Method::DuiViewer || s2.scroller != viewer) {
        Release(s2);
        s2.sV = Spring{}; s2.sH = Spring{};
        s2.sV.pos = s2.sV.target = current;
        s2.accV = s2.accH = 0;
        s2.method = Method::DuiViewer;
        s2.active = false;
    }
    SetScroller(s2, viewer);
    s2.viewerApplied = current;
    s2.sV.Push(dest - s2.sV.target, cfg.springK, cfg.smoothOnly);
    bool ok = StartAnimation(s2, host);
    if (!ok) {
        // No frames available: land where the native step would have.
        s2.sV.target = dest;
        Stop(s2);
        LeaveCriticalSection(&g_cs);
        g_svSetYOffset(viewer, dest);
        return true;
    }
    PruneStates();
    LeaveCriticalSection(&g_cs);
    return true;
}

static void ScrollViewer_OnInput_Hook(void* self, void* inputEvent) {
    auto* e = (BYTE*)inputEvent;
    if (g_duiViewerLive.load(std::memory_order_acquire) && IsPlainWheel(e) &&
        ViewerWheel(self, inputEvent)) {
        e[8] = 1;
        return;
    }
    g_svOnInputOrig(self, inputEvent);
}

// Element::OnDestroy plus the optional ScrollViewer path; dui70 exports only.
static bool DuiCoreStage() {
    HMODULE dui = nullptr;
    if (!GetModuleHandleExW(0, L"dui70.dll", &dui))
        return false;
    g_duiCoreDone.store(true, std::memory_order_release);
    g_dui70Ref = dui;

    auto destroy = (Element_OnDestroy_t)GetProcAddress(dui, "?OnDestroy@Element@DirectUI@@UEAAXXZ");
    g_elementIsRTL = (Element_IsRTL_t)GetProcAddress(dui, "?IsRTL@Element@DirectUI@@QEAA_NXZ");
    if (!destroy ||
        !WindhawkUtils::SetFunctionHook(destroy, Element_OnDestroy_Hook, &g_elementOnDestroyOrig)) {
        Wh_Log(L"DirectUI: Element::OnDestroy unavailable; native DirectUI scrolling off");
        return false;
    }

    auto onInput = (ScrollViewer_OnInput_t)GetProcAddress(dui,
        "?OnInput@BaseScrollViewer@DirectUI@@UEAAXPEAUInputEvent@2@@Z");
    g_svGetYOffset = (ScrollViewer_GetYOffset_t)GetProcAddress(dui,
        "?GetYOffset@BaseScrollViewer@DirectUI@@QEAAHXZ");
    g_svSetYOffset = (ScrollViewer_SetYOffset_t)GetProcAddress(dui,
        "?SetYOffset@BaseScrollViewer@DirectUI@@QEAAJH@Z");
    g_svGetVScroll = (ScrollViewer_GetVScroll_t)GetProcAddress(dui,
        "?GetVScroll@ScrollViewer@DirectUI@@MEAAPEAVBaseScrollBar@2@XZ");
    g_styledSvGetVScroll = (ScrollViewer_GetVScroll_t)GetProcAddress(dui,
        "?GetVScroll@StyledScrollViewer@DirectUI@@MEAAPEAVBaseScrollBar@2@XZ");
    if (!onInput || !g_svGetYOffset || !g_svSetYOffset || !g_svGetVScroll ||
        !WindhawkUtils::SetFunctionHook(onInput, ScrollViewer_OnInput_Hook, &g_svOnInputOrig)) {
        Wh_Log(L"DirectUI: ScrollViewer exports unavailable; its wheel stays native");
        g_svOnInputOrig = nullptr;
    }
    return true;
}

static bool DuiItemsStage() {
    HMODULE ef = nullptr;
    if (!GetModuleHandleExW(0, L"ExplorerFrame.dll", &ef))
        return false;
    g_duiItemsDone.store(true, std::memory_order_release);
    g_explorerFrameRef = ef;

    WindhawkUtils::SYMBOL_HOOK explorerframeDllHooks[] = {
        {{L"public: virtual void __cdecl LineScroller::OnInput(struct DirectUI::InputEvent *)"},
         &g_lsOnInputOrig, LineScroller_OnInput_Hook, true},
        {{L"public: void __cdecl LineScroller::ScrollPreciseDelta(struct tagSIZE const &,struct tagSIZE *)"},
         &g_lsScrollPreciseDelta, nullptr, true},
        {{L"public: enum VIRTUALDIRECTION __cdecl LineScroller::GetVirtualDirection(void)"},
         &g_lsGetVirtualDirection, nullptr, true},
        {{L"public: long __cdecl LineScroller::GetRowFromYPosition(int,class RowInfo *,int)"},
         &g_lsRowFromY, nullptr, true},
        {{L"public: int __cdecl LineScroller::GetRowInfoForNextRow(class RowInfo const &,enum SCROLLDIR,class RowInfo *)"},
         &g_lsNextRow, nullptr, true},
    };
    WindhawkUtils::HookSymbols(ef, explorerframeDllHooks, ARRAYSIZE(explorerframeDllHooks));

    // OnInput may be staged even when a later symbol failed; its hook
    // stays a pass-through until g_duiLive is set.
    if (!g_lsOnInputOrig || !g_lsScrollPreciseDelta || !g_lsGetVirtualDirection ||
        !g_lsRowFromY || !g_lsNextRow) {
        Wh_Log(L"Items view: symbols unavailable (input=%d delta=%d dir=%d rowFromY=%d next=%d); using UI Automation",
               g_lsOnInputOrig != nullptr, g_lsScrollPreciseDelta != nullptr,
               g_lsGetVirtualDirection != nullptr, g_lsRowFromY != nullptr,
               g_lsNextRow != nullptr);
        return false;
    }
    return true;
}
#endif

// Stages whatever DirectUI pieces have become available. `apply` is false
// inside Wh_ModInit, where staged hooks are applied when it returns.
static void DuiSetup(bool apply) {
#ifdef _WIN64
    if (g_unloading.load(std::memory_order_acquire))
        return;
    bool coreStaged = false, itemsStaged = false;
    if (!g_duiCoreDone.load(std::memory_order_acquire))
        coreStaged = DuiCoreStage();
    // The items view keeps element pointers, so it needs the lifetime hook.
    bool coreReady = coreStaged || g_elementOnDestroyOrig;
    if (coreReady && !g_duiItemsDone.load(std::memory_order_acquire))
        itemsStaged = DuiItemsStage();
    if (!coreStaged && !itemsStaged)
        return;
    if (apply && !Wh_ApplyHookOperations()) {
        Wh_Log(L"DirectUI: applying hooks failed; scrolling stays on the fallback paths");
        return;
    }
    if (coreStaged && g_svOnInputOrig) {
        g_duiViewerLive.store(true, std::memory_order_release);
        Wh_Log(L"DirectUI: ScrollViewer pixel scrolling enabled");
    }
    if (itemsStaged) {
        g_duiLive.store(true, std::memory_order_release);
        Wh_Log(L"Items view: native pixel scrolling enabled");
    }
#endif
}

static bool DuiSetupPending() {
    return (!g_duiCoreDone.load(std::memory_order_acquire) && GetModuleHandleW(L"dui70.dll")) ||
           (!g_duiItemsDone.load(std::memory_order_acquire) && GetModuleHandleW(L"ExplorerFrame.dll"));
}

// ---------------------------------------------------------------------------
// Frame pacing
//
// A worker waits for the compositor clock (or a high-resolution timer at
// the monitor's rate) and posts g_tickMsg to each animating window. The
// DispatchMessage hook runs the tick on the window's own thread.
// ---------------------------------------------------------------------------

using WaitForCompositorClock_t = DWORD (WINAPI*)(UINT count, const HANDLE* handles, DWORD timeoutInMs);

static UINT g_tickMsg = 0;
static double g_qpcFreq = 1.0;
static HANDLE g_pacerStop = nullptr;
static HANDLE g_pacerWake = nullptr;
static HANDLE g_duiSetupEvent = nullptr;
static std::thread* g_pacer = nullptr;
static HMODULE g_dcomp = nullptr;
static WaitForCompositorClock_t g_waitForCompositorClock = nullptr;

static LONGLONG QpcNow() {
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return c.QuadPart;
}

// Called under g_cs.
static bool StartAnimation(State& s, HWND hwnd) {
    if (!g_pacer || g_unloading.load(std::memory_order_acquire))
        return false;
    if (!s.active) {
        s.active = true;
        s.lastStepQpc = QpcNow();
        s.periodUs = FramePeriodUs(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY));
        UINT dpi = GetDpiForWindow(hwnd);
        if (!dpi) dpi = 96;
        switch (s.method) {
        case Method::LineScroll: s.minSpeed = 6.0; break;   // lines
        case Method::UIAPercent: s.minSpeed = 2.0; break;   // percent
        default: s.minSpeed = kTailMinPxPerSec * dpi / 96.0; break;
        }
        SetEvent(g_pacerWake);
    }
    return true;
}

// Posts one tick to every animating window that has none queued and
// returns the shortest frame period among them, or 0 when all are idle.
static int PostFrames() {
    int periodUs = 0;
    LONGLONG now = QpcNow();
    EnterCriticalSection(&g_cs);
    for (auto& p : g_st) {
        State& s = p.second;
        if (!s.active) continue;
        if (s.tickPending) {
            // A tick that never reached the dispatch hook (a modal loop that
            // bypasses DispatchMessage) must not keep the pacer spinning.
            // The flag clears after the step paints, so allow slow paints.
            if (now - s.postedQpc > (LONGLONG)g_qpcFreq) {
                s.tickPending = false;
                Stop(s);
                continue;
            }
        } else if (PostMessageW(p.first, g_tickMsg, 0, 0)) {
            s.tickPending = true;
            s.postedQpc = now;
        } else {
            Stop(s);
            continue;
        }
        if (!periodUs || s.periodUs < periodUs) periodUs = s.periodUs;
    }
    LeaveCriticalSection(&g_cs);
    return periodUs;
}

// Returns false when the pacer must stop.
static bool WaitFrame(HANDLE timer, int periodUs) {
    if (g_waitForCompositorClock) {
        // A frame returns index `count`, like WaitForMultipleObjects, though
        // the docs give STATUS_SUCCESS; accept both. Other statuses (display
        // off) return at once, so fall through to the timer for this frame.
        DWORD status = g_waitForCompositorClock(1, &g_pacerStop, 100);
        if (WaitForSingleObject(g_pacerStop, 0) == WAIT_OBJECT_0) return false;
        if (status == 0 || status == 1) return true;
    }
    LARGE_INTEGER due;
    due.QuadPart = -(LONGLONG)periodUs * 10;
    if (timer && SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE)) {
        HANDLE hs[2] = { g_pacerStop, timer };
        return WaitForMultipleObjects(2, hs, FALSE, INFINITE) != WAIT_OBJECT_0;
    }
    return WaitForSingleObject(g_pacerStop, (DWORD)(periodUs / 1000 + 1)) == WAIT_TIMEOUT;
}

static void PacerLoop() {
    HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr,
                                          CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                          TIMER_ALL_ACCESS);
    if (!timer) timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);

    // A new Explorer window process loads ExplorerFrame shortly after the
    // mod starts; watch for it briefly so the first wheel already takes the
    // native path. Later loads (file dialogs) are caught by RequestDuiSetup.
    constexpr ULONGLONG kSetupWatchMs = 15000;
    ULONGLONG watchUntil = GetTickCount64() + kSetupWatchMs;

    for (;;) {
        bool watching = GetTickCount64() < watchUntil &&
                        !(g_duiCoreDone.load(std::memory_order_acquire) &&
                          g_duiItemsDone.load(std::memory_order_acquire));
        HANDLE idle[3] = { g_pacerStop, g_pacerWake, g_duiSetupEvent };
        DWORD w = WaitForMultipleObjects(3, idle, FALSE, watching ? 250 : INFINITE);
        if (w == WAIT_TIMEOUT || w == WAIT_OBJECT_0 + 2) {
            if (DuiSetupPending()) DuiSetup(true);
            continue;
        }
        if (w != WAIT_OBJECT_0 + 1) break;

        bool stop = false;
        for (;;) {
            int periodUs = PostFrames();
            if (!periodUs) break;
            if (!WaitFrame(timer, periodUs)) { stop = true; break; }
        }
        if (stop) break;
    }

    if (timer) CloseHandle(timer);
}

static void RequestDuiSetup() {
    if (g_duiSetupEvent && DuiSetupPending())
        SetEvent(g_duiSetupEvent);
}

// ---------------------------------------------------------------------------
// Line composition
//
// TreeView, ListBox, multiline Edit and report ListView only move by whole
// lines. While one animates, it is stepped line by line with its scroll
// blits dropped, and WM_PAINT shows two WM_PRINTCLIENT captures, top line f
// and f + 1, offset by the fraction between them. A control whose capture
// doesn't match what it shows keeps plain line steps.
// ---------------------------------------------------------------------------

static UINT g_compEndMsg;
static bool g_scrollHooks = true;  // blits can be dropped

// Windows of this thread being composed; their scroll blits are dropped.
static thread_local HWND t_composed[4];

static bool Composed(HWND h) {
    for (HWND x : t_composed)
        if (x && x == h) return true;
    return false;
}

// Recent windows whose capture didn't match the screen (guarded by g_cs).
static HWND g_noComp[16];
static int g_noCompNext;

static bool NoComp(HWND h) {
    for (HWND x : g_noComp)
        if (x == h) return true;
    return false;
}

static bool CompTarget(HWND h, CompKind* kind) {
    WCHAR c[32] = {};
    if (!GetClassNameW(h, c, ARRAYSIZE(c))) return false;
    LONG style = GetWindowLongW(h, GWL_STYLE);
    if (_wcsicmp(c, L"SysTreeView32") == 0) {
        *kind = CompKind::Tree;
    } else if (_wcsicmp(c, L"ListBox") == 0 || _wcsicmp(c, L"ComboLBox") == 0) {
        if (style & (LBS_OWNERDRAWVARIABLE | LBS_MULTICOLUMN)) return false;
        *kind = CompKind::List;
    } else if (_wcsicmp(c, L"Edit") == 0) {
        if (!(style & ES_MULTILINE)) return false;
        *kind = CompKind::Edit;
    } else if (_wcsicmp(c, L"SysListView32") == 0) {
        // Group view scrolls by pixels, not rows.
        if ((style & LVS_TYPEMASK) != LVS_REPORT ||
            SendMessageW(h, LVM_ISGROUPVIEWENABLED, 0, 0))
            return false;
        *kind = CompKind::LV;
    } else {
        return false;
    }
    return true;
}

// Only a real scroll bar counts: a window that never had one still reports
// the default 0-100 range, with no page.
static bool CompRange(HWND h, int* top, int* maxTop) {
    if (!(GetWindowLongW(h, GWL_STYLE) & WS_VSCROLL)) return false;
    SCROLLINFO si = { sizeof(si), SIF_ALL };
    if (!GetScrollInfo(h, SB_VERT, &si) || si.nMax <= si.nMin || !si.nPage) return false;
    *top = si.nPos;
    *maxTop = std::max(si.nMax - std::max((int)si.nPage - 1, 0), si.nMin);
    return *maxTop > si.nMin;
}

static int CompTop(HWND h) {
    SCROLLINFO si = { sizeof(si), SIF_POS };
    GetScrollInfo(h, SB_VERT, &si);
    return si.nPos;
}

static bool CompMetrics(Comp& c) {
    HWND h = c.hwnd;
    c.area = { 0, 0, c.size.cx, c.size.cy };
    switch (c.kind) {
    case CompKind::Tree:
        c.lh = (int)SendMessageW(h, TVM_GETITEMHEIGHT, 0, 0);
        break;
    case CompKind::List:
        c.lh = (int)SendMessageW(h, LB_GETITEMHEIGHT, 0, 0);
        break;
    case CompKind::Edit: {
        LRESULT first = SendMessageW(h, EM_GETFIRSTVISIBLELINE, 0, 0);
        LRESULT c0 = SendMessageW(h, EM_LINEINDEX, first, 0);
        LRESULT c1 = SendMessageW(h, EM_LINEINDEX, first + 1, 0);
        if (c0 < 0 || c1 <= c0) return false;
        LRESULT p0 = SendMessageW(h, EM_POSFROMCHAR, c0, 0);
        LRESULT p1 = SendMessageW(h, EM_POSFROMCHAR, c1, 0);
        if (p0 == -1 || p1 == -1) return false;
        c.lh = (short)HIWORD(p1) - (short)HIWORD(p0);
        // Lines scroll inside the formatting rectangle.
        RECT fr = {};
        SendMessageW(h, EM_GETRECT, 0, (LPARAM)&fr);
        c.area.top = std::max(fr.top, 0L);
        c.area.bottom = std::min(fr.bottom, (LONG)c.size.cy);
        break;
    }
    case CompKind::LV: {
        RECT r = { LVIR_BOUNDS };
        int top = (int)SendMessageW(h, LVM_GETTOPINDEX, 0, 0);
        if (!SendMessageW(h, LVM_GETITEMRECT, top, (LPARAM)&r)) return false;
        c.lh = r.bottom - r.top;
        // Rows scroll below the header, a child window that paints itself.
        HWND hdr = (HWND)SendMessageW(h, LVM_GETHEADER, 0, 0);
        if (hdr && IsWindowVisible(hdr)) {
            RECT hr;
            GetWindowRect(hdr, &hr);
            MapWindowPoints(nullptr, h, (POINT*)&hr, 2);
            c.area.top = std::max(hr.bottom, 0L);
        }
        break;
    }
    }
    return c.lh > 0 && c.area.bottom - c.area.top > c.lh;
}

// One line command; returns the new top line (unchanged at an edge).
static int CompStep(Comp& c, bool down) {
    c.moving++;
    t_ownScroll++;
    SendMessageW(c.hwnd, WM_VSCROLL, MAKEWPARAM(down ? SB_LINEDOWN : SB_LINEUP, 0), 0);
    SendMessageW(c.hwnd, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
    t_ownScroll--;
    c.moving--;
    return CompTop(c.hwnd);
}

// Moves the control toward a top line and returns where it stopped, the
// first position reached at or past it; scroll blits are dropped meanwhile.
// Tree items taller than one line (Explorer's gaps between groups) skip
// positions, so the tree steps one item at a time and checks.
static int CompMoveTo(Comp& c, int line) {
    int top = CompTop(c.hwnd);
    int n = line - top;
    if (!n) return top;
    if (c.kind != CompKind::Tree && (n > 2 || n < -2)) {
        c.moving++;
        t_ownScroll++;
        switch (c.kind) {
        case CompKind::List: SendMessageW(c.hwnd, LB_SETTOPINDEX, line, 0); break;
        case CompKind::Edit: SendMessageW(c.hwnd, EM_LINESCROLL, 0, n); break;
        default:             SendMessageW(c.hwnd, LVM_SCROLL, 0, (LPARAM)n * c.lh); break;
        }
        t_ownScroll--;
        c.moving--;
        return CompTop(c.hwnd);
    }
    while (top != line) {
        bool down = top < line;
        int next = CompStep(c, down);
        if (next == top || (down ? next >= line : next <= line)) return next;
        top = next;
    }
    return top;
}

// The nearest reachable top line at or before `line`.
static int CompSeekNotAfter(Comp& c, int line) {
    int top = CompMoveTo(c, line);
    while (top > line) {
        int prev = top;
        top = CompStep(c, false);
        if (top >= prev) break;
    }
    return top;
}

// The background the control paints on, for areas it leaves untouched.
static void CompFill(const Comp& c, HDC dc) {
    RECT r = { 0, 0, c.size.cx, c.size.cy };
    COLORREF bk = CLR_NONE;
    HBRUSH br = nullptr;
    HWND parent = GetParent(c.hwnd);
    DWORD pid = 0;
    if (parent) GetWindowThreadProcessId(parent, &pid);
    bool ownParent = pid == GetCurrentProcessId();
    switch (c.kind) {
    case CompKind::Tree: bk = (COLORREF)SendMessageW(c.hwnd, TVM_GETBKCOLOR, 0, 0); break;
    case CompKind::LV:   bk = (COLORREF)SendMessageW(c.hwnd, LVM_GETBKCOLOR, 0, 0); break;
    case CompKind::List:
        if (ownParent)
            br = (HBRUSH)SendMessageW(parent, WM_CTLCOLORLISTBOX, (WPARAM)dc, (LPARAM)c.hwnd);
        break;
    case CompKind::Edit: {
        LONG style = GetWindowLongW(c.hwnd, GWL_STYLE);
        UINT msg = (style & ES_READONLY) || !IsWindowEnabled(c.hwnd)
                       ? WM_CTLCOLORSTATIC : WM_CTLCOLOREDIT;
        if (ownParent) br = (HBRUSH)SendMessageW(parent, msg, (WPARAM)dc, (LPARAM)c.hwnd);
        break;
    }
    }
    if (br) {
        FillRect(dc, &r, br);
        return;
    }
    if (bk == CLR_NONE || bk == (COLORREF)-1) bk = GetSysColor(COLOR_WINDOW);
    SetDCBrushColor(dc, bk);
    FillRect(dc, &r, (HBRUSH)GetStockObject(DC_BRUSH));
}

// Captures the control where it stands.
static void CompCapture(Comp& c, int slot) {
    CompFill(c, c.dc[slot]);
    c.moving++;
    SendMessageW(c.hwnd, WM_PRINTCLIENT, (WPARAM)c.dc[slot], PRF_CLIENT | PRF_ERASEBKGND);
    c.moving--;
    c.cap[slot] = CompTop(c.hwnd);
}

static void CompSwap(Comp& c) {
    std::swap(c.dc[0], c.dc[1]);
    std::swap(c.bm[0], c.bm[1]);
    std::swap(c.old[0], c.old[1]);
    std::swap(c.cap[0], c.cap[1]);
}

// Slot 0 holds a reachable top line a <= t and slot 1 the next reachable
// one b (b == a at the bottom).
static bool CompCovers(const Comp& c, double t) {
    int a = c.cap[0], b = c.cap[1];
    if (a < 0 || b < a || t < a) return false;
    return b > a ? t <= b : true;
}

// The reachable line nearest t, for landing.
static int CompNearest(const Comp& c, double t) {
    t = std::clamp(t, 0.0, (double)c.maxTop);
    int a = c.cap[0], b = c.cap[1];
    if (CompCovers(c, t) && b > a) return t - a <= b - t ? a : b;
    if (CompCovers(c, t)) return a;
    return (int)std::lround(t);
}

// Captures for the current position; moves the control, so never from WM_PAINT.
static void CompPrepare(Comp& c) {
    double t = std::clamp(c.pos, 0.0, (double)c.maxTop);
    if (CompCovers(c, t)) return;
    // Held across the parent's WM_CTLCOLOR* too: a send to another thread
    // can deliver the unload request in between.
    c.moving++;
    // Crossing one line: the capture passed becomes the other neighbor.
    if (c.cap[0] >= 0 && c.cap[1] > c.cap[0] && t > c.cap[1]) {
        CompSwap(c);
        if (CompMoveTo(c, c.cap[0]) == c.cap[0]) {
            CompStep(c, true);
            CompCapture(c, 1);
        }
    } else if (c.cap[0] > 0 && c.cap[1] >= c.cap[0] && t < c.cap[0]) {
        CompSwap(c);
        if (CompMoveTo(c, c.cap[1]) == c.cap[1]) {
            CompStep(c, false);
            CompCapture(c, 0);
        }
    }
    if (!CompCovers(c, t)) {
        if (CompSeekNotAfter(c, (int)std::floor(t)) != c.cap[0]) CompCapture(c, 0);
        CompStep(c, true);
        CompCapture(c, 1);
    }
    // The control stops short of the scroll bar's range: that is the end.
    if (c.cap[1] == c.cap[0] && c.cap[0] >= 0) c.maxTop = std::min(c.maxTop, c.cap[0]);
    c.moving--;
}

static void CompDraw(const Comp& c, HDC dst) {
    double t = std::clamp(c.pos, 0.0, (double)c.maxTop);
    if (!CompCovers(c, t)) return;
    int a = c.cap[0], b = c.cap[1];
    int w = c.size.cx, h = c.area.bottom - c.area.top;
    // Margins around the band come from slot 0; the report header is a
    // child window and paints itself.
    if (c.area.top > 0 && c.kind != CompKind::LV)
        BitBlt(dst, 0, 0, w, c.area.top, c.dc[0], 0, 0, SRCCOPY);
    if (c.size.cy > c.area.bottom)
        BitBlt(dst, 0, c.area.bottom, w, c.size.cy - c.area.bottom, c.dc[0], 0, c.area.bottom, SRCCOPY);
    // Content between lines a and b still showing comes from a's capture,
    // the rest from b's; the two can be more than one line apart.
    int off = b > a ? (int)std::lround((t - a) * c.lh) : 0;
    int aRows = b > a ? std::min((b - a) * c.lh - off, h) : h;
    if (aRows > 0)
        BitBlt(dst, 0, c.area.top, w, aRows, c.dc[0], 0, c.area.top + off, SRCCOPY);
    if (h - aRows > 0)
        BitBlt(dst, 0, c.area.top + aRows, w, h - aRows, c.dc[1], 0, c.area.top, SRCCOPY);
}

// Compared only where the window is visible (elsewhere the readback is
// whatever covers it), by luminance: text drawn with ClearType on screen and
// grayscale into a memory DC (the Modernizer does this) differs in color,
// not lightness. Up to 1% may differ for caret and focus details; content
// off by one line already reaches 1.5%.
static bool CompMatchesScreen(const Comp& c, HDC wdc) {
    int w = c.size.cx, h = c.area.bottom - c.area.top;
    BITMAPINFO bi = {};
    bi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB };
    void* bits[2] = {};
    HBITMAP dib[2] = {};
    HDC m = CreateCompatibleDC(wdc);
    bool ok = m != nullptr;
    for (int i = 0; i < 2 && ok; i++) {
        dib[i] = CreateDIBSection(wdc, &bi, DIB_RGB_COLORS, &bits[i], nullptr, 0);
        ok = dib[i] != nullptr;
    }
    if (ok) {
        HGDIOBJ o = SelectObject(m, dib[0]);
        BitBlt(m, 0, 0, w, h, wdc, 0, c.area.top, SRCCOPY);
        SelectObject(m, dib[1]);
        BitBlt(m, 0, 0, w, h, c.dc[0], 0, c.area.top, SRCCOPY);
        SelectObject(m, o);
        GdiFlush();
        const DWORD* a = (const DWORD*)bits[0];
        const DWORD* b = (const DWORD*)bits[1];
        size_t visible = 0, diff = 0;
        HRGN rgn = CreateRectRgn(0, 0, 0, 0);
        RGNDATA* rd = nullptr;
        POINT org = { 0, c.area.top };
        ClientToScreen(c.hwnd, &org);
        if (rgn && GetRandomRgn(wdc, rgn, SYSRGN) == 1) {
            OffsetRgn(rgn, -org.x, -org.y);
            DWORD sz = GetRegionData(rgn, 0, nullptr);
            rd = sz ? (RGNDATA*)malloc(sz) : nullptr;
            if (rd && !GetRegionData(rgn, sz, rd)) { free(rd); rd = nullptr; }
        }
        for (DWORD k = 0; rd && k < rd->rdh.nCount; k++) {
            RECT q = ((const RECT*)rd->Buffer)[k];
            q.left = std::max(q.left, 0L);
            q.top = std::max(q.top, 0L);
            q.right = std::min(q.right, (LONG)w);
            q.bottom = std::min(q.bottom, (LONG)h);
            for (LONG y = q.top; y < q.bottom; y++) {
                for (LONG x = q.left; x < q.right; x++) {
                    DWORD pa = a[y * w + x], pb = b[y * w + x];
                    visible++;
                    auto luma = [](DWORD p) {
                        return (int)((((p >> 16) & 255) * 77 + ((p >> 8) & 255) * 150 +
                                      (p & 255) * 29) >> 8);
                    };
                    if (std::abs(luma(pa) - luma(pb)) > 48) diff++;
                }
            }
        }
        free(rd);
        if (rgn) DeleteObject(rgn);
        // Too little in view to judge: keep line steps.
        ok = visible * 10 >= (size_t)w * h && diff * 100 <= visible;
    }
    for (HBITMAP d : dib)
        if (d) DeleteObject(d);
    if (m) DeleteDC(m);
    return ok;
}

static void CompFree(Comp& c) {
    for (int i = 0; i < 2; i++) {
        if (c.dc[i]) {
            if (c.old[i]) SelectObject(c.dc[i], c.old[i]);
            DeleteDC(c.dc[i]);
        }
        if (c.bm[i]) DeleteObject(c.bm[i]);
        c.dc[i] = nullptr;
        c.bm[i] = nullptr;
    }
    for (HWND& x : t_composed)
        if (x == c.hwnd) x = nullptr;
}

static LRESULT CALLBACK CompSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

// On the window's thread. Fails (nullptr) for controls that can't be composed.
static Comp* CompBegin(HWND h, CompKind kind) {
    HWND* slot = nullptr;
    for (HWND& x : t_composed)
        if (!x) { slot = &x; break; }
    if (!slot) return nullptr;

    RECT cr;
    GetClientRect(h, &cr);
    auto* c = new Comp;
    c->hwnd = h;
    c->kind = kind;
    c->size = { cr.right, cr.bottom };
    int top = 0;
    if (c->size.cx <= 0 || !CompMetrics(*c) || !CompRange(h, &top, &c->maxTop)) {
        delete c;
        return nullptr;
    }
    c->pos = top;

    UpdateWindow(h);  // the screen comparison needs a settled paint
    HDC wdc = GetDC(h);
    bool ok = wdc != nullptr;
    for (int i = 0; i < 2 && ok; i++) {
        c->dc[i] = CreateCompatibleDC(wdc);
        c->bm[i] = CreateCompatibleBitmap(wdc, c->size.cx, c->size.cy);
        ok = c->dc[i] && c->bm[i];
        if (ok) c->old[i] = SelectObject(c->dc[i], c->bm[i]);
    }
    *slot = h;
    if (ok) {
        CompCapture(*c, 0);
        ok = CompMatchesScreen(*c, wdc);
    }
    if (wdc) ReleaseDC(h, wdc);
    if (ok) ok = c->subclassed =
        WindhawkUtils::SetWindowSubclassFromAnyThread(h, CompSubclassProc, (DWORD_PTR)c);
    if (!ok) {
        CompFree(*c);
        delete c;
        return nullptr;
    }
    if (kind == CompKind::Edit) c->caretHidden = HideCaret(h) != FALSE;
    return c;
}

// On the window's thread, outside our own moves. Puts the control on the
// retired landing line (unless destroying) and hands painting back.
static void CompEnd(Comp* c, bool destroying) {
    EnterCriticalSection(&g_cs);
    auto it = g_st.find(c->hwnd);
    if (it != g_st.end() && it->second.comp == c) it->second.comp = nullptr;
    int line = c->land;
    LeaveCriticalSection(&g_cs);

    HWND h = c->hwnd;
    if (!destroying) CompMoveTo(*c, line);
    if (c->caretHidden && !destroying) ShowCaret(h);
    CompFree(*c);
    if (c->subclassed)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, CompSubclassProc);
    delete c;
    if (!destroying) RedrawWindow(h, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE);
}

// Clicks, keys and layout changes finish at once on the line in view.
static void CompStopNow(Comp* c) {
    int line = CompNearest(*c, c->pos);
    EnterCriticalSection(&g_cs);
    auto it = g_st.find(c->hwnd);
    if (it != g_st.end() && it->second.comp == c) {
        it->second.sV.target = it->second.sV.pos = line;
        Stop(it->second);
    } else if (c->live) {
        c->live = false;
        c->land = line;
    }
    LeaveCriticalSection(&g_cs);
    if (!c->moving) CompEnd(c, false);
}

static bool IsInputMsg(UINT msg) {
    switch (msg) {
    case WM_KEYDOWN: case WM_SYSKEYDOWN: case WM_CHAR:
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    case WM_NCLBUTTONDOWN: case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN: case WM_NCMBUTTONDOWN: case WM_NCXBUTTONDOWN:
    case WM_CONTEXTMENU:
        return true;
    }
    return false;
}

static LRESULT CALLBACK CompSubclassProc(HWND h, UINT msg, WPARAM w, LPARAM l, DWORD_PTR ref) {
    Comp* c = (Comp*)ref;
    if (msg == WM_NCDESTROY) {
        CompEnd(c, true);
    } else if (msg == WM_ERASEBKGND) {
        return 1;
    } else if (msg == WM_PAINT) {
        if (c->moving) {
            // Mid-move the screen keeps the last composed frame.
            ValidateRect(h, nullptr);
            return 0;
        }
        EnterCriticalSection(&g_cs);
        bool live = c->live;
        LeaveCriticalSection(&g_cs);
        if (!live) {
            CompEnd(c, false);
            return DefSubclassProc(h, msg, w, l);
        }
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        if (dc) CompDraw(*c, dc);
        EndPaint(h, &ps);
        return 0;
    } else if (msg == g_compEndMsg) {
        CompStopNow(c);
        return 0;
    } else if (IsInputMsg(msg) || msg == WM_SIZE || msg == WM_SETFONT ||
               msg == WM_STYLECHANGED || msg == WM_THEMECHANGED ||
               msg == WM_DPICHANGED_AFTERPARENT) {
        CompStopNow(c);
    }
    return DefSubclassProc(h, msg, w, l);
}

// ---------------------------------------------------------------------------
// Tick (runs on the window's thread)
// ---------------------------------------------------------------------------

static void TickStep(HWND hw) {
    // Snapshot settings before acquiring g_cs: SnapCfg uses its own
    // g_cfgLock (shared), no need to hold both locks simultaneously.
    const CfgSnap cfg = SnapCfg();

    EnterCriticalSection(&g_cs);

    auto it = g_st.find(hw);
    if (it == g_st.end()) {
        LeaveCriticalSection(&g_cs);
        return;
    }

    State& s = it->second;
    if (!s.active || g_unloading.load(std::memory_order_acquire)) {
        LeaveCriticalSection(&g_cs);
        return;
    }

    LONGLONG now = QpcNow();
    double dt = (now - s.lastStepQpc) / g_qpcFreq;
    s.lastStepQpc = now;
    if (dt < 0) dt = 0;
    if (dt > 0.05) dt = 0.05;

    double v0 = s.sV.pos, h0 = s.sH.pos;
    if (cfg.smoothOnly) {
        s.sV.StepSmooth(kSmoothRate, dt);
        s.sH.StepSmooth(kSmoothRate, dt);
    } else {
        s.sV.StepSpring(dt, cfg.springK, cfg.damping, cfg.overshoot);
        s.sH.StepSpring(dt, cfg.springK, cfg.damping, cfg.overshoot);
    }
    double dV = s.sV.FloorTail(v0, s.minSpeed, dt);
    double dH = s.sH.FloorTail(h0, s.minSpeed, dt);

    // Composed lines finish on a line the control can show at the top:
    // touchpad notches leave fractions and tall tree items skip positions.
    if (s.method == Method::LineComp && s.comp && Settled(s)) {
        int land = CompNearest(*s.comp, s.sV.target);
        if (s.sV.target != land) s.sV.target = land;
    }

    if (Settled(s)) {
        // Absolute paths land exactly on the target.
        Method m = s.method;
        void* viewer = m == Method::DuiViewer ? s.scroller : nullptr;
        int finalY = (int)std::lround(s.sV.target);
        bool land = (viewer || m == Method::PixelRE) && finalY != s.viewerApplied;
        bool composed = s.comp != nullptr;
        Stop(s);  // a composition ends at the next paint
        LeaveCriticalSection(&g_cs);
        if (composed) UpdateWindow(hw);
        if (land) {
            if (m == Method::PixelRE) {
                POINT p = { 0, finalY };
                SendMessageW(hw, EM_GETSCROLLPOS, 0, (LPARAM)&p);
                p.y = finalY;
                SendMessageW(hw, EM_SETSCROLLPOS, 0, (LPARAM)&p);
            }
#ifdef _WIN64
            else {
                g_svSetYOffset(viewer, finalY);
            }
#endif
            UpdateWindow(hw);
        }
        return;
    }

    bool moved = false;

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
                moved = true;
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
                t_ownScroll++;
                for (int i = 0; i < n; i++)
                    SendMessageW(hw, WM_VSCROLL, MAKEWPARAM(cmd, 0), 0);
                SendMessageW(hw, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
                t_ownScroll--;
                EnterCriticalSection(&g_cs);
                moved = true;
            }
            break;
        }

        case Method::LineComp: {
            // Only this thread frees the composition, so it outlives the unlock.
            Comp* c = s.comp;
            if (!c || !c->live) { Stop(s); break; }
            if (c->moving) break;  // a control pumping messages inside our move
            c->pos = s.sV.pos;
            LeaveCriticalSection(&g_cs);
            CompPrepare(*c);
            InvalidateRect(hw, nullptr, FALSE);
            EnterCriticalSection(&g_cs);
            moved = true;
            // The control may end before its scroll bar says.
            auto i2 = g_st.find(hw);
            if (i2 != g_st.end() && i2->second.comp == c &&
                i2->second.sV.target > c->maxTop) {
                i2->second.sV.target = c->maxTop;
                i2->second.sV.pos = std::min(i2->second.sV.pos, (double)c->maxTop);
            }
            break;
        }

        case Method::PixelRE: {
            int y = (int)std::lround(s.sV.pos);
            if (y != s.viewerApplied) {
                s.viewerApplied = y;
                LeaveCriticalSection(&g_cs);
                POINT p = {};
                SendMessageW(hw, EM_GETSCROLLPOS, 0, (LPARAM)&p);
                p.y = y;
                SendMessageW(hw, EM_SETSCROLLPOS, 0, (LPARAM)&p);
                EnterCriticalSection(&g_cs);
                moved = true;
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

#ifdef _WIN64
        case Method::DuiPrecise: {
            s.accV += dV;
            int px = (int)s.accV;
            if (px && s.scroller) {
                s.accV -= px;
                // Positive moves toward the start; the pan path takes screen
                // deltas, so the horizontal one is mirrored for RTL.
                SIZE d = s.duiHorizontal ? SIZE{ s.duiRtl ? -px : px, 0 } : SIZE{ 0, px };
                SIZE unapplied = {};
                void* sc = s.scroller;
                LeaveCriticalSection(&g_cs);
                g_lsScrollPreciseDelta(sc, &d, &unapplied);
                EnterCriticalSection(&g_cs);
                moved = true;
                auto i2 = g_st.find(hw);
                if (i2 != g_st.end() && (unapplied.cx || unapplied.cy)) {
                    // Reached an edge: stop so the next notch reverses at once.
                    i2->second.sV.Snap();
                    i2->second.accV = 0;
                }
            } else if (!s.scroller) {
                Stop(s);
            }
            break;
        }

        case Method::DuiViewer: {
            if (!s.scroller) { Stop(s); break; }
            int y = (int)std::lround(s.sV.pos);
            if (y != s.viewerApplied) {
                s.viewerApplied = y;
                void* viewer = s.scroller;
                LeaveCriticalSection(&g_cs);
                g_svSetYOffset(viewer, y);
                EnterCriticalSection(&g_cs);
                moved = true;
            }
            break;
        }
#endif

        default: break;
        }

        it = g_st.find(hw);
        if (it == g_st.end()) {
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
                moved = true;
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
                t_ownScroll++;
                for (int i = 0; i < n; i++)
                    SendMessageW(hw, WM_HSCROLL, MAKEWPARAM(cmd, 0), 0);
                SendMessageW(hw, WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), 0);
                t_ownScroll--;
                EnterCriticalSection(&g_cs);
                moved = true;
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
    }

    LeaveCriticalSection(&g_cs);

    // WM_PAINT has the lowest priority; paint this step before the next tick.
    if (moved) UpdateWindow(hw);
}

// The next tick is posted only after this one has stepped and painted:
// a queued posted message would otherwise always outrank wheel input.
static void Tick(HWND hw) {
    TickStep(hw);
    EnterCriticalSection(&g_cs);
    auto it = g_st.find(hw);
    if (it != g_st.end()) it->second.tickPending = false;
    LeaveCriticalSection(&g_cs);
}

// ---------------------------------------------------------------------------
// Intercept WM_MOUSEWHEEL
// ---------------------------------------------------------------------------

static bool Handle(const MSG* m) {
    if (!m || !m->hwnd || g_unloading.load(std::memory_order_acquire)) return false;

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

    // The items view may be reachable natively once ExplorerFrame's
    // symbols are resolved; UI Automation covers it until then.
    if (mt == Method::UIAPercent) RequestDuiSetup();

    // SysListView32 horizontal-only (Icon/Tile) or with nothing to scroll:
    // pass through. A bar never set up reports 0-100 with no page.
    if (mt == Method::PixelLV && vert) {
        SCROLLINFO si = { sizeof(si), SIF_RANGE | SIF_PAGE };
        if (!GetScrollInfo(tgt, SB_VERT, &si) || si.nMax <= si.nMin ||
            (!si.nPage && !(GetWindowLongW(tgt, GWL_STYLE) & WS_VSCROLL)))
            return false;
    }

    double lines = ((double)delta / WHEEL_DELTA) * cfg.sysLines;
    int lh = (mt == Method::PixelLV) ? LVLineH(tgt) : 0;

    // RichEdit wheels on its own timer, so there's no synchronous step to
    // read back; a notch moves the configured lines of the top line's height.
    int reY = 0, reMax = 0;
    if (mt == Method::PixelRE && (!vert || !REMetrics(tgt, &reY, &reMax, &lh)))
        mt = Method::LineScroll;

    CompKind ck = CompKind::List;
    if (mt == Method::LineScroll && vert && g_scrollHooks && CompTarget(tgt, &ck)) {
        EnterCriticalSection(&g_cs);
        bool skip = NoComp(tgt);
        LeaveCriticalSection(&g_cs);
        if (!skip) mt = Method::LineComp;
    }

    EnterCriticalSection(&g_cs);

    State& s = g_st[tgt];
    if (s.method != mt) {
        Release(s);
        s.sV = Spring{}; s.sH = Spring{};
        s.accV = s.accH = 0;
        s.active = false;
    }
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
        if (!StartAnimation(s2, tgt)) {
            pushedSpring->target -= pushValue;
            LeaveCriticalSection(&g_cs);
            return false;
        }
        PruneStates();
        LeaveCriticalSection(&g_cs);
        return true;
    }

    // Absolute lines. Clicks and keys end a composition, so one still live
    // and animating is where this notch continues from.
    if (mt == Method::LineComp) {
        int top = 0, maxTop = 0;
        if (s.comp && s.comp->moving) {
            LeaveCriticalSection(&g_cs);
            return true;  // dispatched from inside our own move
        }
        if (!s.active || !s.comp || !s.comp->live) {
            Comp* old = s.comp;  // retired, waiting for its paint
            LeaveCriticalSection(&g_cs);
            if (old) CompEnd(old, false);
            if (!CompRange(tgt, &top, &maxTop)) return false;
            if ((lines > 0 && top <= 0) || (lines < 0 && top >= maxTop))
                return true;  // at an edge
            Comp* c = CompBegin(tgt, ck);
            EnterCriticalSection(&g_cs);
            if (!c) {
                g_noComp[g_noCompNext] = tgt;
                g_noCompNext = (g_noCompNext + 1) % ARRAYSIZE(g_noComp);
                LeaveCriticalSection(&g_cs);
                return Handle(m);  // plain line steps
            }
            State& sc = g_st[tgt];
            Release(sc);
            sc.method = Method::LineComp;
            sc.sV = Spring{}; sc.sH = Spring{};
            sc.sV.pos = sc.sV.target = top;
            sc.accV = sc.accH = 0;
            sc.active = false;
            sc.comp = c;
        }
        State& sc = g_st[tgt];
        Comp* c = sc.comp;
        double before = sc.sV.target;
        double dest = std::clamp(before - lines, 0.0, (double)c->maxTop);
        if (dest == before) {
            LeaveCriticalSection(&g_cs);
            return true;
        }
        sc.sV.Push(dest - before, cfg.springK, cfg.smoothOnly);
        if (!StartAnimation(sc, tgt)) {
            sc.sV.target = before;
            Stop(sc);
            LeaveCriticalSection(&g_cs);
            return false;
        }
        UINT dpi = GetDpiForWindow(tgt);
        sc.minSpeed = kTailMinPxPerSec * (dpi ? dpi : 96) / 96.0 / c->lh;
        PruneStates();
        LeaveCriticalSection(&g_cs);
        return true;
    }

    // Absolute pixels: restart from the live position when idle or when
    // something else (thumb drag, keyboard) moved it under the animation.
    if (mt == Method::PixelRE) {
        if (!s.active || s.viewerApplied != reY) {
            s.sV = Spring{}; s.sH = Spring{};
            s.sV.pos = s.sV.target = reY;
            s.viewerApplied = reY;
        }
        double before = s.sV.target;
        double dest = std::clamp(before - lines * lh, 0.0, (double)reMax);
        if (dest == before) {
            LeaveCriticalSection(&g_cs);
            return true;  // at an edge
        }
        s.sV.Push(dest - before, cfg.springK, cfg.smoothOnly);
        if (!StartAnimation(s, tgt)) {
            s.sV.target = before;
            LeaveCriticalSection(&g_cs);
            return false;
        }
        PruneStates();
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

    if (!StartAnimation(s, tgt)) {
        pushedSpring->target -= pushValue;
        LeaveCriticalSection(&g_cs);
        return false;
    }

    PruneStates();
    LeaveCriticalSection(&g_cs);
    return true;
}

// ---------------------------------------------------------------------------
// ScrollWindowEx hook
// ---------------------------------------------------------------------------

// ListBox moves each line with SW_SMOOTHSCROLL, a ~100 ms animation of its
// own in a nested loop; during our steps the line moves at once. That call
// leaves invalidation to the animation, so it is asked for explicitly.
// A composed window paints only composed frames, so its blits are dropped
// and the scrolled area reported as needing a repaint.
using ScrollWindowEx_t = decltype(&ScrollWindowEx);
using ScrollDC_t = decltype(&ScrollDC);
static ScrollWindowEx_t g_scrollWindowExOrig;
static ScrollDC_t g_scrollDCOrig;

static void DroppedScroll(HWND h, const RECT* prcScroll, HRGN hrgnUpdate, LPRECT prcUpdate) {
    RECT r;
    if (prcScroll) r = *prcScroll;
    else GetClientRect(h, &r);
    if (hrgnUpdate) SetRectRgn(hrgnUpdate, r.left, r.top, r.right, r.bottom);
    if (prcUpdate) *prcUpdate = r;
}

static int WINAPI ScrollWindowEx_Hook(HWND hWnd, int dx, int dy, const RECT* prcScroll,
                                      const RECT* prcClip, HRGN hrgnUpdate,
                                      LPRECT prcUpdate, UINT flags) {
    if (hWnd && Composed(hWnd)) {
        DroppedScroll(hWnd, prcScroll, hrgnUpdate, prcUpdate);
        return SIMPLEREGION;
    }
    if (t_ownScroll > 0 && (flags & SW_SMOOTHSCROLL))
        flags = (flags & ~(SW_SMOOTHSCROLL | 0xFFFF0000u)) | SW_INVALIDATE | SW_ERASE;
    return g_scrollWindowExOrig(hWnd, dx, dy, prcScroll, prcClip, hrgnUpdate,
                                prcUpdate, flags);
}

// Edit scrolls its own DC.
static BOOL WINAPI ScrollDC_Hook(HDC hDC, int dx, int dy, const RECT* lprcScroll,
                                 const RECT* lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate) {
    if (t_composed[0] || t_composed[1] || t_composed[2] || t_composed[3]) {
        HWND h = WindowFromDC(hDC);
        if (h && Composed(h)) {
            DroppedScroll(h, lprcScroll, hrgnUpdate, lprcUpdate);
            return TRUE;
        }
    }
    return g_scrollDCOrig(hDC, dx, dy, lprcScroll, lprcClip, hrgnUpdate, lprcUpdate);
}

// ---------------------------------------------------------------------------
// DispatchMessage hooks
// ---------------------------------------------------------------------------

using DW = decltype(&DispatchMessageW);
using DA = decltype(&DispatchMessageA);
static DW g_origW;
static DA g_origA;

static LRESULT WINAPI HookW(const MSG* m) {
    if (m) {
        if (g_tickMsg && m->message == g_tickMsg) {
            Tick(m->hwnd);
            return 0;
        }
        if (m->message == WM_MOUSEWHEEL || m->message == WM_MOUSEHWHEEL) {
            if (Handle(m)) return 0;
            HWND prev = t_wheelHwnd;
            t_wheelHwnd = m->hwnd;
            LRESULT r = g_origW(m);
            t_wheelHwnd = prev;
            return r;
        }
    }
    return g_origW(m);
}

static LRESULT WINAPI HookA(const MSG* m) {
    if (m) {
        if (g_tickMsg && m->message == g_tickMsg) {
            Tick(m->hwnd);
            return 0;
        }
        if (m->message == WM_MOUSEWHEEL || m->message == WM_MOUSEHWHEEL) {
            if (Handle(m)) return 0;
            HWND prev = t_wheelHwnd;
            t_wheelHwnd = m->hwnd;
            LRESULT r = g_origA(m);
            t_wheelHwnd = prev;
            return r;
        }
    }
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

    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    g_qpcFreq = (double)f.QuadPart;
    g_tickMsg = RegisterWindowMessageW(L"Wh_SmoothScrollWin32_Tick");

    if (!WindhawkUtils::SetFunctionHook(DispatchMessageW, HookW, &g_origW) ||
        !WindhawkUtils::SetFunctionHook(DispatchMessageA, HookA, &g_origA)) {
        Wh_Log(L"DispatchMessage hooks failed");
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }
    g_compEndMsg = RegisterWindowMessageW(L"Wh_SmoothScrollWin32_CompEnd");
    if (!WindhawkUtils::SetFunctionHook(ScrollWindowEx, ScrollWindowEx_Hook,
                                        &g_scrollWindowExOrig) ||
        !WindhawkUtils::SetFunctionHook(ScrollDC, ScrollDC_Hook, &g_scrollDCOrig)) {
        Wh_Log(L"Scroll hooks failed; line controls keep plain line steps");
        g_scrollHooks = false;
    }

    // Whatever DirectUI modules are loaded now are hooked here; the pacer
    // thread stages the rest when they load.
    DuiSetup(false);
    return TRUE;
}

void Wh_ModAfterInit() {
    g_pacerStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_pacerWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_duiSetupEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_pacerStop || !g_pacerWake || !g_duiSetupEvent) {
        Wh_Log(L"Pacer events failed; scrolling stays native");
        return;
    }

    g_dcomp = LoadLibraryExW(L"dcomp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_dcomp)
        g_waitForCompositorClock = (WaitForCompositorClock_t)
            GetProcAddress(g_dcomp, "DCompositionWaitForCompositorClock");

    g_pacer = new std::thread(PacerLoop);
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);
    if (g_pacer) {
        SetEvent(g_pacerStop);
        if (g_pacer->joinable()) g_pacer->join();
        delete g_pacer;
        g_pacer = nullptr;
    }

    // Compositions end on their windows' threads, with the hooks still in
    // place to drop the landing move's blits. A window busy in one of our
    // own moves is asked again.
    for (;;) {
        HWND h = nullptr;
        Comp* c = nullptr;
        EnterCriticalSection(&g_cs);
        for (auto& p : g_st)
            if (p.second.comp) { h = p.first; c = p.second.comp; break; }
        LeaveCriticalSection(&g_cs);
        if (!h) break;
        bool alive = IsWindow(h);
        if (alive) SendMessageW(h, g_compEndMsg, 0, 0);
        EnterCriticalSection(&g_cs);
        auto it = g_st.find(h);
        bool pending = it != g_st.end() && it->second.comp == c;
        if (pending && !alive) it->second.comp = nullptr;
        LeaveCriticalSection(&g_cs);
        if (pending && !alive) {
            // Gone without reaching our WM_NCDESTROY: only the bitmaps remain.
            CompFree(*c);
            delete c;
        } else if (pending) {
            Sleep(10);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Smooth Scroll — Uninit");
    EnterCriticalSection(&g_cs);
    for (auto& p : g_st)
        Release(p.second);
    g_st.clear();
    LeaveCriticalSection(&g_cs);
    DeleteCriticalSection(&g_cs);

    for (HANDLE* h : { &g_pacerStop, &g_pacerWake, &g_duiSetupEvent }) {
        if (*h) { CloseHandle(*h); *h = nullptr; }
    }
    g_waitForCompositorClock = nullptr;
    if (g_dcomp) { FreeLibrary(g_dcomp); g_dcomp = nullptr; }
    if (g_explorerFrameRef) { FreeLibrary(g_explorerFrameRef); g_explorerFrameRef = nullptr; }
    if (g_dui70Ref) { FreeLibrary(g_dui70Ref); g_dui70Ref = nullptr; }
}
