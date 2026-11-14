// ==WindhawkMod==
// @id              center-new-windows
// @name            Center New Windows
// @name:pt-BR      Centralizar Novas Janelas Automaticamente
// @name:es         Centrar Nuevas Ventanas Automáticamente
// @description     Opens all new windows centered on the screen, including UWP and WinUI apps
// @description:pt-BR Abre todas as novas janelas centralizadas na tela, incluindo apps UWP e WinUI
// @description:es  Abre todas las ventanas nuevas centradas en la pantalla, incluyendo apps UWP y WinUI
// @version         1.0.1
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         *
// @compilerOptions -luser32 -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- fadeTrickEnabled: false
  $name: Enable Fade Trick
  $name:pt-BR: Ativar Fade Trick
  $name:es: Activar Fade Trick
  $description: >
    When enabled, the window is made transparent before being shown, then
    repositioned while invisible, becoming visible only after the delay below.
    Use this if an app overrides the centered position after ShowWindow,
    causing a visible jump.
  $description:pt-BR: >
    Quando ativado, a janela é tornada transparente antes de ser exibida e
    reposicionada enquanto invisível, tornando-se visível apenas após o
    atraso abaixo. Use isso se um aplicativo sobrescreve a posição
    centralizada após o ShowWindow, causando um salto visível.
  $description:es: >
    Cuando está activado, la ventana se vuelve transparente antes de
    mostrarse y se reposiciona mientras es invisible, haciéndose visible
    solo después del retraso indicado. Úsalo si una aplicación anula la
    posición centrada después de ShowWindow, causando un salto visible.
- fadeDelayMs: 25
  $name: Fade delay (ms)
  $name:pt-BR: Atraso do fade (ms)
  $name:es: Retraso del fade (ms)
  $description: >
    Only used when Fade Trick is enabled above.
    How many milliseconds the window stays fully transparent while being
    repositioned to its centered position. One frame at 60 Hz takes ~16 ms;
    the default of 25 ms adds a small safety margin so the repositioned
    frame is fully composited before the window becomes visible again.
    Safe range: 1–500. Values below 16 may cause a brief flash on slow GPUs.
  $description:pt-BR: >
    Usado apenas quando o Fade Trick acima está ativo.
    Define por quantos milissegundos a janela permanece completamente
    transparente enquanto é reposicionada para o centro da tela. Um quadro
    a 60 Hz dura ~16 ms; o padrão de 25 ms adiciona uma margem de segurança
    para que o quadro reposicionado seja totalmente composto antes de a
    janela voltar a ficar visível. Intervalo seguro: 1–500. Valores abaixo
    de 16 podem causar um flash breve em GPUs lentas.
  $description:es: >
    Solo se usa cuando el Fade Trick de arriba está activado.
    Define cuántos milisegundos permanece la ventana completamente
    transparente mientras se reposiciona al centro de la pantalla. Un
    fotograma a 60 Hz dura ~16 ms; el valor predeterminado de 25 ms agrega
    un margen de seguridad para que el fotograma reposicionado esté
    totalmente compuesto antes de que la ventana vuelva a ser visible.
    Rango seguro: 1–500. Valores por debajo de 16 pueden causar un destello
    breve en GPUs lentas.
- resizeEnabled: false
  $name: Resize windows to default size
  $name:pt-BR: Redimensionar janelas para tamanho padrão
  $name:es: Redimensionar ventanas al tamaño predeterminado
  $description: >
    When enabled, new windows are resized to the width and height defined
    below before being centered. The app may also override this if it
    enforces its own minimum or fixed size. Dialogs (see below) are never
    resized. UWP windows are never resized.
  $description:pt-BR: >
    Quando ativado, as novas janelas são redimensionadas para a largura e
    altura definidas abaixo antes de serem centralizadas. O aplicativo também
    pode ignorar isso se impuser seu próprio tamanho mínimo ou fixo.
    Diálogos (veja abaixo) nunca são redimensionados. Janelas UWP nunca
    são redimensionadas.
  $description:es: >
    Cuando está activado, las nuevas ventanas se redimensionan al ancho y
    alto definidos a continuación antes de centrarse. La aplicación también
    puede ignorar esto si impone su propio tamaño mínimo o fijo. Los
    diálogos (ver abajo) nunca se redimensionan. Las ventanas UWP nunca
    se redimensionan.
- resizeWidth: 1024
  $name: Default width (px)
  $name:pt-BR: Largura padrão (px)
  $name:es: Ancho predeterminado (px)
  $description: Target window width in pixels when resizing is enabled.
  $description:pt-BR: Largura alvo da janela em pixels quando o redimensionamento está ativado.
  $description:es: Ancho objetivo de la ventana en píxeles cuando el redimensionamiento está activado.
- resizeHeight: 768
  $name: Default height (px)
  $name:pt-BR: Altura padrão (px)
  $name:es: Alto predeterminado (px)
  $description: Target window height in pixels when resizing is enabled.
  $description:pt-BR: Altura alvo da janela em pixels quando o redimensionamento está ativado.
  $description:es: Alto objetivo de la ventana en píxeles cuando el redimensionamiento está activado.
- centerDialogs: false
  $name: Always center dialogs (file picker, Save As, Open…)
  $name:pt-BR: Sempre centralizar diálogos (seletor de arquivo, Salvar como, Abrir…)
  $name:es: Centrar siempre los diálogos (selector de archivo, Guardar como, Abrir…)
  $description: >
    When enabled, owned Win32 dialogs (window class #32770) are always
    centered on the screen every time they appear, regardless of where the
    parent application places them. This covers the file picker (Save As,
    Open File), print dialogs, font pickers, and other standard Win32
    dialogs. Dialogs are never resized. Note: this setting affects all
    standard Win32 dialogs, not only file pickers.
  $description:pt-BR: >
    Quando ativado, diálogos Win32 owned (classe de janela #32770) são
    sempre centralizados na tela toda vez que aparecem, independentemente
    de onde o aplicativo pai os posiciona. Isso cobre o seletor de arquivo
    (Salvar como, Abrir arquivo), diálogos de impressão, seletores de
    fonte e outros diálogos Win32 padrão. Diálogos nunca são
    redimensionados. Observação: esta configuração afeta todos os diálogos
    Win32 padrão, não apenas seletores de arquivo.
  $description:es: >
    Cuando está activado, los diálogos Win32 owned (clase de ventana
    #32770) siempre se centran en la pantalla cada vez que aparecen,
    independientemente de dónde los coloque la aplicación padre. Esto
    incluye el selector de archivos (Guardar como, Abrir archivo),
    diálogos de impresión, selectores de fuente y otros diálogos Win32
    estándar. Los diálogos nunca se redimensionan. Nota: esta
    configuración afecta a todos los diálogos Win32 estándar, no solo
    a los selectores de archivo.
- centerUWP: true
  $name: Center UWP and WinUI windows
  $name:pt-BR: Centralizar janelas UWP e WinUI
  $name:es: Centrar ventanas UWP y WinUI
  $description: >
    When enabled, UWP and WinUI application windows (such as Windows
    Settings, Microsoft Store, Calculator, etc.) are centered on the screen
    when they first appear. Some apps like Microsoft Store may show a brief
    position jump because they create and reposition multiple internal
    windows during initialization.
  $description:pt-BR: >
    Quando ativado, janelas de aplicativos UWP e WinUI (como Configurações,
    Microsoft Store, Calculadora, etc.) são centralizadas na tela ao
    aparecerem pela primeira vez. Alguns apps como a Microsoft Store podem
    apresentar um breve salto de posição porque criam e reposicionam
    múltiplas janelas internas durante a inicialização.
  $description:es: >
    Cuando está activado, las ventanas de aplicaciones UWP y WinUI (como
    Configuración, Microsoft Store, Calculadora, etc.) se centran en la
    pantalla cuando aparecen por primera vez. Algunas apps como Microsoft
    Store pueden mostrar un breve salto de posición porque crean y
    reposicionan múltiples ventanas internas durante la inicialización.
- centerOffsetY: 0
  $name: Vertical offset (px)
  $name:pt-BR: Deslocamento vertical (px)
  $name:es: Desplazamiento vertical (px)
  $description: >
    Shifts the centered position up or down by this many pixels.
    Use a negative value (e.g. -30) to move windows up, or a positive
    value (e.g. 30) to move them down. Useful if you use a dock or
    status bar that is not detected by the system work area.
  $description:pt-BR: >
    Desloca a posição centralizada para cima ou para baixo por esta
    quantidade de pixels. Use um valor negativo (ex.: -30) para mover
    as janelas para cima, ou positivo (ex.: 30) para mover para baixo.
    Útil se você usa uma dock ou barra de status que não é detectada
    pela área de trabalho do sistema.
  $description:es: >
    Desplaza la posición centrada hacia arriba o abajo por esta cantidad
    de píxeles. Use un valor negativo (ej.: -30) para mover las ventanas
    hacia arriba, o positivo (ej.: 30) para mover hacia abajo. Útil si
    usa un dock o barra de estado que no es detectada por el área de
    trabajo del sistema.
- excludeClasses:
    - ""
  $name: Excluded window classes
  $name:pt-BR: Classes de janela excluídas
  $name:es: Clases de ventana excluidas
  $description: >
    Window class names that should NOT be centered or resized.
    Use Spy++ (included with Visual Studio) or the free WinSpy++ tool
    to find a window's class name.
  $description:pt-BR: >
    Nomes de classe de janela que NÃO devem ser centralizados nem
    redimensionados. Use o Spy++ (incluído no Visual Studio) ou a ferramenta
    gratuita WinSpy++ para encontrar o nome de classe de uma janela.
  $description:es: >
    Nombres de clase de ventana que NO deben centrarse ni redimensionarse.
    Use Spy++ (incluido con Visual Studio) o la herramienta gratuita WinSpy++
    para encontrar el nombre de clase de una ventana.
- excludeProcesses:
    - ""
  $name: Excluded processes
  $name:pt-BR: Processos excluídos
  $name:es: Procesos excluidos
  $description: >
    Executable names whose windows should NOT be centered or resized
    (lowercase, e.g. onedrive.exe).
  $description:pt-BR: >
    Nomes de executável cujas janelas NÃO devem ser centralizadas nem
    redimensionadas (em minúsculas, ex.: onedrive.exe).
  $description:es: >
    Nombres de ejecutable cuyas ventanas NO deben centrarse ni redimensionarse
    (en minúsculas, ej.: onedrive.exe).
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Center New Windows

Automatically centers newly created application windows on the active
monitor, similar to the default behavior in macOS. Also centers UWP and
WinUI application windows such as Windows Settings, Microsoft Store,
Calculator, and other modern apps.

## What's new in 1.0.1

- **`SetWindowPos` grace period** — after a window is first centred, a
  2-second grace period blocks any `SetWindowPos` call that tries to move
  it elsewhere. This fixes a known issue with Firefox-based browsers
  (Firefox, Floorp, Zen, LibreWolf and others) that restore their saved
  session position via `SetWindowPos` ~700 ms after `ShowWindow`,
  overriding the centred position.
- **Vertical offset setting** — shifts the centred position up or down by
  a fixed number of pixels. Useful for third-party docks or status bars
  that are not reflected in the system work area.
- **DPI-unaware app fix** — windows and dialogs that belong to processes
  using system scaling (e.g. Audacity, older Win32 apps) are now correctly
  centred at all scale levels. The previous calculation mixed physical
  monitor coordinates with virtual window coordinates, placing windows
  above and to the left of true centre.

---

### Regular windows

The mod hooks `ShowWindow()` and `SetWindowPos()` in every process.

When a suitable window is shown for the first time, it is optionally
resized and then centered on the monitor before becoming visible.

A 2-second grace period prevents apps from overriding the centered
position. This is needed for apps like Firefox and Floorp that restore
their saved session position via `SetWindowPos` after `ShowWindow`.
During the grace period, any `SetWindowPos` call that tries to move
a recently centered window is silently redirected back to the centered
position. The user can still move windows manually after the grace
period expires.

Windows restored from the system tray (e.g. WhatsApp, Discord) are
**always re-centered** but the grace period is **not applied** on
restore — only on first appearance.

Owned windows — dialogs, settings panels, export windows, the Run
dialog, and similar pop-ups — are intentionally ignored by default.

The **Always center dialogs** option is an exception: when enabled,
standard Win32 dialogs (window class `#32770`) such as the file picker,
print dialogs, and font pickers are **always centered** every time they
appear. Child dialogs inside property sheets and sub-dialogs are
automatically excluded.

Some applications reposition their windows after `ShowWindow`. For those
cases, the **Fade Trick** can be enabled in settings to temporarily make
the window transparent while it is being repositioned.

### UWP and WinUI windows

UWP windows have two layers:

- **ApplicationFrameWindow** — the top-level frame (title bar, borders)
- **CoreWindow** — the app content, positioned as a child of the frame

The Windows shell (`explorer.exe`) positions the CoreWindow via
`SetWindowPos`. This mod detects that moment, finds the parent
ApplicationFrameWindow, and moves the **frame** to the center of
the screen. A grace period prevents the shell from overriding the
centered position.

As a fallback, the mod also intercepts `ShowWindow` in
`ApplicationFrameHost.exe` to catch frames at first appearance,
using short timer-based polling until the frame has its real size.

> **Note:** Some UWP apps like Microsoft Store may show a brief position
> jump when opening. This happens because these apps create and
> reposition multiple internal windows during initialization, and the
> shell may momentarily override the centered position before the grace
> period takes effect.

---

## Settings

### Fade Trick + Fade delay (ms)

When **Fade Trick** is enabled, the window is made transparent before
being shown, repositioned while invisible, and then restored after
the configured delay.

At 60 Hz one frame takes about **16 ms**; the default value of
**25 ms** adds a small safety margin.

Safe range: **1–500 ms**.

### Resize windows to default size

When enabled, new windows are resized to the configured width and
height before being centered. Dialogs and UWP windows are never resized.

### Always center dialogs

When enabled, all standard Win32 top-level dialogs (`#32770`) are
centered every time they appear. This includes file pickers (Save As,
Open File), print dialogs, font pickers, and other standard dialogs.

Child dialogs inside property sheets (such as the tab pages in
"Drive Properties") and sub-dialogs opened from another dialog are
automatically excluded — only the first-level dialog is ever centered.

### Center UWP and WinUI windows

When enabled, UWP and WinUI application windows (Settings, Store,
Calculator, etc.) are centered on the screen when they first appear.

### Vertical offset

Shifts the centered position up or down by a fixed number of pixels.
Useful if you use a third-party dock (e.g. MyDockFinder, Nexus) or a
status bar that is not reflected in the system work area.

Use a **negative** value to move windows **up**, or a **positive** value
to move them **down**. Default is **0** (no offset).

### Exclusion lists

The mod maintains an internal list of always-excluded processes and
window classes that are known to break or have no visible content.

You can add your own entries in the settings panel for any additional
apps or window classes you want to skip.

**Always excluded (hardcoded processes):**

| Entry | Reason |
|------|------|
| `photoshop.exe` | Adobe Photoshop — blank layout after splash |
| `shellexperiencehost.exe` | Start menu, Action Center |
| `startmenuexperiencehost.exe` | Start menu |
| `searchhost.exe` | Windows Search UI |
| `lockapp.exe` | Lock screen |
| `systemsettings.exe` | Windows Settings (UWP host process) |
| `textinputhost.exe` | Touch keyboard / emoji panel |
| `widgets.exe` | Windows Widgets |
| `phoneexperiencehost.exe` | Phone Link host |
| `gamebarpresencewriter.exe` | Xbox Game Bar |
| `runtimebroker.exe` | UWP permission broker |
| `dllhost.exe` | COM surrogate host |
| `sihost.exe` | Shell Infrastructure Host |
| `taskhostw.exe` | Task scheduler host |
| `ctfmon.exe` | Input method framework |
| `credentialuibroker.exe` | Credential dialog broker |
| `windowsterminal.exe` | Manages its own window |
| `backgroundtaskhost.exe` | Background task runner |
| `securityhealthsystray.exe` | Windows Security tray |
| `onedrive.exe` | OneDrive tray pop-up |
| `mspcmanager.exe` | PC Manager tray pop-up |
| `msedgewebview2.exe` | WebView2 embedded browser |
| `conhost.exe` | Console host |
| `svchost.exe` | Service host |

**Always excluded (hardcoded classes):**

| Entry | Reason |
|------|------|
| `MsoSplash` | Office splash screens |
| `Microsoft.UI.Content.PopupWindowSiteBridge` | WinUI pop-up host |
| `DropdownWindow` | Generic drop-down menus |
| `Windows.UI.Core.CoreWindow` | UWP core window |
| `Shell_TrayWnd` | Taskbar |
| `Shell_SecondaryTrayWnd` | Secondary monitor taskbar |
| `NotifyIconOverflowWindow` | Tray overflow area |
| `Progman` | Desktop program manager |
| `WorkerW` | Desktop wallpaper worker |
| `Windows.UI.Input.InputSite.WindowClass` | Input site host |
| `XamlExplorerHostIslandWindow` | XAML island host |

---

### How DPI scaling is handled

For DPI-unaware applications (older Win32 apps that rely on the OS to scale
their windows), `GetWindowRect` returns coordinates in virtual pixels while
`GetMonitorInfo` always returns physical pixels. Mixing the two without
correction places windows above and to the left of true centre.

The fix detects virtualisation by comparing `GetWindowRect` against
`DWMWA_EXTENDED_FRAME_BOUNDS` (which always returns physical pixels). When a
mismatch is found, `GetDpiForSystem()` supplies the exact system scale factor,
which is used to convert the monitor work area to virtual pixels before the
centred position is computed.

---

# Centralizar Novas Janelas Automaticamente

Centraliza automaticamente novas janelas de aplicativos no monitor
ativo, de forma semelhante ao comportamento do macOS. Também centraliza
janelas UWP e WinUI como Configurações, Microsoft Store, Calculadora
e outros apps modernos.

Um grace period de 2 segundos impede que apps como Firefox e Floorp
sobrescrevam a posição centralizada ao restaurar a posição salva da
sessão anterior.

A centralização lida corretamente com aplicativos que não suportam
nativamente alta DPI: ao detectar que o OS está virtualizando
coordenadas, o mod converte a área de trabalho do monitor para o espaço
de pixels virtual antes de calcular a posição centralizada.

**Sempre excluídos (fixos no código):**

O mod mantém uma lista interna de processos e classes de janela sempre
excluídos. Você pode adicionar exclusões adicionais nas configurações.

---

# Centrar Nuevas Ventanas Automáticamente

Centra automáticamente las nuevas ventanas de aplicaciones en el
monitor activo, similar al comportamiento de macOS. También centra
ventanas UWP y WinUI como Configuración, Microsoft Store, Calculadora
y otras apps modernas.

Un grace period de 2 segundos impide que apps como Firefox y Floorp
sobrescriban la posición centrada al restaurar la posición guardada
de la sesión anterior.

El centrado maneja correctamente las aplicaciones que no soportan
nativamente DPI alto: al detectar que el SO está virtualizando
coordenadas, el mod convierte el área de trabajo del monitor al
espacio de píxeles virtual antes de calcular la posición centrada.

**Siempre excluidos (fijos en el código):**

El mod mantiene una lista interna de procesos y clases de ventana
siempre excluidos. Puede agregar exclusiones adicionales en la
configuración.
*/
// ==/WindhawkModReadme==

/**
 * Implementation notes
 * ────────────────────
 * Three hook strategies cover all supported window types:
 *
 * 1. ShowWindow (all processes):
 *    · Centers windows on first appearance.
 *    · DWM shadow compensation via DWMWA_EXTENDED_FRAME_BOUNDS.
 *    · g_centeredAtom tracks first-time vs tray-restore.
 *    · Optional Fade Trick for apps that override position after show.
 *
 * 2. SetWindowPos (all processes) — regular window grace period:
 *    · After first-time centering, a 2-second grace period blocks any
 *      SetWindowPos calls that try to move the window elsewhere.
 *    · Catches Firefox/Floorp session-restore (~700ms after ShowWindow).
 *    · Timestamp stored via SetProp(hwnd, g_graceAtom, tick) — thread-safe,
 *      no shared array needed. Position is recomputed from current size on
 *      each blocked call so resizes during the grace period are handled.
 *
 * 3. SetWindowPos (explorer.exe + ApplicationFrameHost.exe) — UWP:
 *    · Path 1: CoreWindow positioned → center parent frame.
 *    · Path 2: Frame repositioned during 3s grace → redirect to center.
 *    · ShowWindow fallback with timer polling for late-sized frames.
 */

#include <windhawk_api.h>
#include <windows.h>
#include <dwmapi.h>
#include <algorithm>
#include <atomic>
#include <cwctype>
#include <new>
#include <string>
#include <vector>

// ════════════════════════════════════════════════════════════════════════════
// UWP frame tracking
// ════════════════════════════════════════════════════════════════════════════

struct UwpTrackedFrame {
    HWND      frameWnd;
    int       centeredX;
    int       centeredY;
    ULONGLONG tick;
};

#define UWP_MAX_TRACKED 16
static UwpTrackedFrame g_uwpTracked[UWP_MAX_TRACKED] = {};
static constexpr ULONGLONG UWP_GRACE_MS = 3000;

static UwpTrackedFrame* UwpFindFrame(HWND frame) {
    for (int i = 0; i < UWP_MAX_TRACKED; i++)
        if (g_uwpTracked[i].frameWnd == frame) return &g_uwpTracked[i];
    return nullptr;
}

static UwpTrackedFrame* UwpAllocFrame(HWND frame) {
    ULONGLONG now = GetTickCount64();
    for (int i = 0; i < UWP_MAX_TRACKED; i++) {
        if (g_uwpTracked[i].frameWnd == nullptr ||
            !IsWindow(g_uwpTracked[i].frameWnd) ||
            now - g_uwpTracked[i].tick > UWP_GRACE_MS * 2)
        {
            g_uwpTracked[i] = {};
            g_uwpTracked[i].frameWnd = frame;
            return &g_uwpTracked[i];
        }
    }
    return nullptr;
}

// ════════════════════════════════════════════════════════════════════════════
// Regular window grace period tracking
// ════════════════════════════════════════════════════════════════════════════
//
// After a window is first centered, a 2-second grace period blocks
// SetWindowPos calls that try to move it. This catches apps like
// Firefox and Floorp that restore saved session position ~700ms after
// ShowWindow.
//
// Thread-safety: per m417z's review, the previous fixed array was not
// thread-safe. We now store only a timestamp on the HWND via SetProp so
// that state is owned by the HWND itself — no shared array needed.
// In HookedSetWindowPos we retrieve the timestamp, check the grace period,
// and recompute the centered position from the current window size so the
// position stays correct even if the app resized the window after centering.
// SetProp/GetProp are thread-safe for the same HWND.

static ATOM g_graceAtom = 0; // stores GetTickCount64() low 32 bits on the HWND
static constexpr ULONGLONG REG_GRACE_MS = 2000;

// ════════════════════════════════════════════════════════════════════════════
// Common state
// ════════════════════════════════════════════════════════════════════════════

static ATOM g_centeredAtom = 0;
static thread_local bool g_inHook = false;

static std::wstring g_thisExeName;
static bool g_isExplorer     = false;
static bool g_isAppFrameHost = false;

// ════════════════════════════════════════════════════════════════════════════
// Settings
// ════════════════════════════════════════════════════════════════════════════

// Per m417z's review: centerOffsetY is read from multiple threads (hook
// callbacks) and written by LoadSettings. A single std::atomic<int> is both
// more efficient and more correct than protecting one int with a full SRWLOCK.
static std::atomic<int> g_centerOffsetY{0};

struct Settings {
    bool fadeTrickEnabled = false;
    int  fadeDelayMs      = 25;
    bool resizeEnabled    = false;
    int  resizeWidth      = 1024;
    int  resizeHeight     = 768;
    bool centerDialogs    = false;
    bool centerUWP        = true;
    std::vector<std::wstring> excludeClasses;
    std::vector<std::wstring> excludeProcesses;
};

static Settings g_settings;
static SRWLOCK  g_srw = SRWLOCK_INIT;

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
    return s;
}

static void LoadSettings() {
    Settings fresh;
    fresh.fadeTrickEnabled = Wh_GetIntSetting(L"fadeTrickEnabled") != 0;
    int delay = Wh_GetIntSetting(L"fadeDelayMs");
    fresh.fadeDelayMs = (delay < 1) ? 1 : (delay > 500) ? 500 : delay;
    fresh.resizeEnabled = Wh_GetIntSetting(L"resizeEnabled") != 0;
    int rw = Wh_GetIntSetting(L"resizeWidth");
    int rh = Wh_GetIntSetting(L"resizeHeight");
    fresh.resizeWidth  = (rw > 0) ? rw : 1024;
    fresh.resizeHeight = (rh > 0) ? rh : 768;
    fresh.centerDialogs = Wh_GetIntSetting(L"centerDialogs") != 0;
    fresh.centerUWP     = Wh_GetIntSetting(L"centerUWP") != 0;
    g_centerOffsetY.store(Wh_GetIntSetting(L"centerOffsetY"), std::memory_order_relaxed);

    for (int i = 0; ; ++i) {
        PCWSTR val = Wh_GetStringSetting(L"excludeClasses[%d]", i);
        if (!val || val[0] == L'\0') { Wh_FreeStringSetting(val); break; }
        fresh.excludeClasses.emplace_back(val);
        Wh_FreeStringSetting(val);
    }
    for (int i = 0; ; ++i) {
        PCWSTR val = Wh_GetStringSetting(L"excludeProcesses[%d]", i);
        if (!val || val[0] == L'\0') { Wh_FreeStringSetting(val); break; }
        fresh.excludeProcesses.emplace_back(ToLower(val));
        Wh_FreeStringSetting(val);
    }

    AcquireSRWLockExclusive(&g_srw);
    g_settings = std::move(fresh);
    ReleaseSRWLockExclusive(&g_srw);
}

static Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_srw);
    Settings copy = g_settings;
    ReleaseSRWLockShared(&g_srw);
    return copy;
}

// ════════════════════════════════════════════════════════════════════════════
// Internal exclusion lists
// ════════════════════════════════════════════════════════════════════════════

static const wchar_t* const g_internalExcludeProcesses[] = {
    L"photoshop.exe",
    L"shellexperiencehost.exe",   L"startmenuexperiencehost.exe",
    L"searchhost.exe",            L"lockapp.exe",
    L"systemsettings.exe",        L"textinputhost.exe",
    L"widgets.exe",               L"phoneexperiencehost.exe",
    L"gamebarpresencewriter.exe", L"runtimebroker.exe",
    L"dllhost.exe",               L"sihost.exe",
    L"taskhostw.exe",             L"ctfmon.exe",
    L"credentialuibroker.exe",    L"windowsterminal.exe",
    L"backgroundtaskhost.exe",    L"securityhealthsystray.exe",
    L"onedrive.exe",              L"mspcmanager.exe",
    L"msedgewebview2.exe",        L"conhost.exe",
    L"svchost.exe",
};

static bool IsInternallyExcluded() {
    for (const auto* name : g_internalExcludeProcesses)
        if (g_thisExeName == name) return true;
    return false;
}

static const wchar_t* const g_internalExcludeClasses[] = {
    L"MsoSplash",
    L"Microsoft.UI.Content.PopupWindowSiteBridge",
    L"DropdownWindow",
    L"Windows.UI.Core.CoreWindow",
    L"Shell_TrayWnd",              L"Shell_SecondaryTrayWnd",
    L"NotifyIconOverflowWindow",
    L"Progman",                    L"WorkerW",
    L"Windows.UI.Input.InputSite.WindowClass",
    L"XamlExplorerHostIslandWindow",
};

// ════════════════════════════════════════════════════════════════════════════
// Window class detection
// ════════════════════════════════════════════════════════════════════════════

static bool IsCoreWindow(HWND hwnd) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    return wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

static bool IsAppFrame(HWND hwnd) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    return wcscmp(cls, L"ApplicationFrameWindow") == 0;
}

static HWND FindParentFrame(HWND coreWnd) {
    HWND root = GetAncestor(coreWnd, GA_ROOT);
    if (root && root != coreWnd && IsAppFrame(root))
        return root;
    return nullptr;
}

// ════════════════════════════════════════════════════════════════════════════
// Regular window helpers
// ════════════════════════════════════════════════════════════════════════════

static bool IsBlocklisted(HWND hwnd, const Settings& s,
                           const wchar_t* cachedClass = nullptr) {
    for (const auto& p : s.excludeProcesses)
        if (p == g_thisExeName) return true;

    wchar_t buf[256] = {};
    const wchar_t* cn = cachedClass;
    if (!cn) { GetClassNameW(hwnd, buf, 256); cn = buf; }

    for (const auto* cls : g_internalExcludeClasses)
        if (wcscmp(cn, cls) == 0) return true;
    for (const auto& c : s.excludeClasses)
        if (c == cn) return true;

    return false;
}

static bool IsTopLevelDialogWindow(HWND hwnd, const wchar_t* cn) {
    if (wcscmp(cn, L"#32770") != 0) return false;
    DWORD style = static_cast<DWORD>(GetWindowLong(hwnd, GWL_STYLE));
    return !(style & WS_CHILD);
}

static bool ShouldHandleWindow(HWND hwnd, const Settings& s,
                                const wchar_t* dlgClass) {
    if (GetWindow(hwnd, GW_OWNER) != nullptr) return false;

    DWORD style = static_cast<DWORD>(GetWindowLong(hwnd, GWL_STYLE));
    if (style & WS_CHILD)      return false;
    if (!(style & WS_CAPTION)) return false;
    if (style & WS_MAXIMIZE)   return false;
    if (wcscmp(dlgClass, L"#32770") == 0) return false;

    DWORD exStyle = static_cast<DWORD>(GetWindowLong(hwnd, GWL_EXSTYLE));
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    if (exStyle & WS_EX_NOACTIVATE) return false;

    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) return false;
    if ((rect.right - rect.left) < 100) return false;
    if ((rect.bottom - rect.top) <  50) return false;

    if (IsBlocklisted(hwnd, s)) return false;
    return true;
}

static bool CanResizeWindow(HWND hwnd) {
    DWORD style   = static_cast<DWORD>(GetWindowLong(hwnd, GWL_STYLE));
    DWORD exStyle = static_cast<DWORD>(GetWindowLong(hwnd, GWL_EXSTYLE));

    if (!(style & WS_THICKFRAME))   return false;
    if (!(style & WS_MAXIMIZEBOX))  return false;
    if (style & WS_POPUP)           return false;
    if (exStyle & WS_EX_TOOLWINDOW) return false;

    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) return false;
    if ((rect.right - rect.left) < 150) return false;
    if ((rect.bottom - rect.top) < 100) return false;
    return true;
}

// ════════════════════════════════════════════════════════════════════════════
// Centering functions
// ════════════════════════════════════════════════════════════════════════════

using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t pOrigSetWindowPos = nullptr;

static BOOL CallRealSetWindowPos(HWND hWnd, HWND hAfter,
                                  int X, int Y, int cx, int cy, UINT f) {
    if (pOrigSetWindowPos)
        return pOrigSetWindowPos(hWnd, hAfter, X, Y, cx, cy, f);
    return SetWindowPos(hWnd, hAfter, X, Y, cx, cy, f);
}

static void ResizeWindow(HWND hwnd, const Settings& s) {
    if (!s.resizeEnabled)       return;
    if (!CanResizeWindow(hwnd)) return;
    CallRealSetWindowPos(hwnd, nullptr, 0, 0, s.resizeWidth, s.resizeHeight,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static void CenterWindow(HWND hwnd) {
    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) return;

    const int w = rect.right - rect.left;
    const int h = rect.bottom - rect.top;
    if (w <= 0 || h <= 0) return;

    // ── DWM frame bounds ─────────────────────────────────────────────────────
    // DWMWA_EXTENDED_FRAME_BOUNDS always returns physical pixels. GetWindowRect
    // returns virtual pixels for DPI-unaware processes. Comparing the two lets
    // us detect whether the OS is virtualising coordinates for this window
    // without relying on GetDpiForWindow, which returns the DPI as seen by the
    // calling process (always 96 when injected into a DPI-unaware app).
    bool  dwmAvailable = false;
    RECT  frame        = rect;
    float dwmScaleX    = 1.0f;

    if (IsWindowVisible(hwnd) &&
        SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                         &frame, sizeof(frame))))
    {
        dwmAvailable = true;
        const int fw = frame.right  - frame.left;
        if (fw > 0 && w > 0) dwmScaleX = static_cast<float>(fw) / static_cast<float>(w);
    }

    // hasScaling: true when the OS is applying coordinate virtualisation.
    // The DWM ratio is used only as a detection signal — it is slightly
    // imprecise because the OS rounds virtual→physical per-pixel.
    //
    // For the actual conversion we use GetDpiForSystem(), which returns the
    // system-wide DPI setting (e.g. 120 for 125%, 144 for 150%) regardless
    // of the calling process's DPI awareness. This gives an exact ratio.
    const bool hasScaling = (dwmScaleX > 1.01f);

    // Exact scale: GetDpiForSystem() / 96. Only used when hasScaling is true.
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    if (hasScaling) {
        const float sysDpi = static_cast<float>(GetDpiForSystem());
        scaleX = scaleY = (sysDpi > 0.f) ? sysDpi / 96.f : dwmScaleX;
    }

    // DWM shadow offsets: only meaningful without coordinate scaling.
    // When hasScaling, frame > rect due to virtualisation — shadow cannot be
    // isolated from the scale difference, so offsets are left as zero.
    int offsetL = 0, offsetT = 0, offsetR = 0, offsetB = 0;
    if (dwmAvailable && !hasScaling) {
        offsetL = std::max(0, (int)(frame.left   - rect.left));
        offsetT = std::max(0, (int)(frame.top    - rect.top));
        offsetR = std::max(0, (int)(rect.right   - frame.right));
        offsetB = std::max(0, (int)(rect.bottom  - frame.bottom));
    }

    const int visW = w - offsetL - offsetR;
    const int visH = h - offsetT - offsetB;

    HMONITOR    hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi   = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(hMon, &mi)) return;

    // rcWork is always in physical pixels. When hasScaling, divide by the
    // exact system scale to convert to virtual pixels (same space as
    // GetWindowRect and SetWindowPos for DPI-unaware windows).
    const LONG originX = mi.rcMonitor.left;
    const LONG originY = mi.rcMonitor.top;

    const int waLeft   = static_cast<int>(originX + (mi.rcWork.left   - originX) / scaleX);
    const int waTop    = static_cast<int>(originY + (mi.rcWork.top    - originY) / scaleY);
    const int waRight  = static_cast<int>(originX + (mi.rcWork.right  - originX) / scaleX);
    const int waBottom = static_cast<int>(originY + (mi.rcWork.bottom - originY) / scaleY);

    const int userOffsetY = g_centerOffsetY.load(std::memory_order_relaxed);

    const int x = waLeft + (waRight  - waLeft - visW) / 2 - offsetL;
    const int y = waTop  + (waBottom - waTop  - visH) / 2 - offsetT;

    CallRealSetWindowPos(hwnd, nullptr, x, y + userOffsetY, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// Center + register for grace period protection.
// Stores a timestamp on the HWND via SetProp so HookedSetWindowPos can detect
// whether the grace period is still active — thread-safe per m417z's review.
static void CenterWindowWithGrace(HWND hwnd) {
    CenterWindow(hwnd);

    if (g_graceAtom) {
        // Store the low 32 bits of GetTickCount64() as the grace start time.
        // Overflow after ~49 days is benign: the grace period check uses
        // subtraction, which wraps correctly in unsigned arithmetic.
        DWORD tick = static_cast<DWORD>(GetTickCount64());
        SetProp(hwnd, reinterpret_cast<LPCWSTR>(g_graceAtom),
                reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(tick)));
    }
}

static void CenterFrameUWP(HWND frame, UwpTrackedFrame* tf) {
    RECT rc = {};
    GetWindowRect(frame, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    HMONITOR hMon = MonitorFromWindow(frame, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi)) return;

    // UWP frames are always Per-Monitor-aware, so GetWindowRect and
    // SetWindowPos use physical pixels. No DPI virtualisation fix needed here.
    const RECT& wa = mi.rcWork;
    tf->centeredX = wa.left + (wa.right - wa.left - w) / 2;
    tf->centeredY = wa.top + (wa.bottom - wa.top - h) / 2;
    tf->tick = GetTickCount64();

    tf->centeredY += g_centerOffsetY.load(std::memory_order_relaxed);

    CallRealSetWindowPos(frame, nullptr, tf->centeredX, tf->centeredY, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static bool IsFrameMaximized(HWND frame) {
    WINDOWPLACEMENT wp = { sizeof(wp) };
    return GetWindowPlacement(frame, &wp) && wp.showCmd == SW_SHOWMAXIMIZED;
}

// ════════════════════════════════════════════════════════════════════════════
// Fade trick
// ════════════════════════════════════════════════════════════════════════════

struct FadeRestoreData {
    HWND hwnd; DWORD originalExStyle; BYTE originalAlpha; DWORD delayMs;
};

static DWORD WINAPI FadeRestoreThread(LPVOID param) {
    auto* d = reinterpret_cast<FadeRestoreData*>(param);
    Sleep(d->delayMs);
    if (IsWindow(d->hwnd)) {
        if (d->originalExStyle & WS_EX_LAYERED)
            SetLayeredWindowAttributes(d->hwnd, 0, d->originalAlpha, LWA_ALPHA);
        else {
            LONG cur = GetWindowLong(d->hwnd, GWL_EXSTYLE);
            SetWindowLong(d->hwnd, GWL_EXSTYLE, cur & ~WS_EX_LAYERED);
        }
    }
    delete d;
    return 0;
}

// ════════════════════════════════════════════════════════════════════════════
// Hook: SetWindowPos — all processes
// ════════════════════════════════════════════════════════════════════════════

BOOL WINAPI HookedSetWindowPos(HWND hWnd, HWND hWndInsertAfter,
                                int X, int Y, int cx, int cy, UINT uFlags) {
    // ── UWP paths (explorer.exe + ApplicationFrameHost.exe only) ─────
    if (g_isExplorer || g_isAppFrameHost) {
        bool uwpEnabled;
        {
            AcquireSRWLockShared(&g_srw);
            uwpEnabled = g_settings.centerUWP;
            ReleaseSRWLockShared(&g_srw);
        }

        if (uwpEnabled) {
            if (IsCoreWindow(hWnd) && !(uFlags & SWP_NOMOVE)) {
                RECT rc = {};
                GetWindowRect(hWnd, &rc);
                int w = (uFlags & SWP_NOSIZE) ? (rc.right - rc.left) : cx;
                int h = (uFlags & SWP_NOSIZE) ? (rc.bottom - rc.top) : cy;

                if (w >= 200 && h >= 200) {
                    HWND frame = FindParentFrame(hWnd);
                    if (frame && !UwpFindFrame(frame) && !IsFrameMaximized(frame)) {
                        UwpTrackedFrame* tf = UwpAllocFrame(frame);
                        if (tf) CenterFrameUWP(frame, tf);
                    }
                }
                return pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
            }

            if (IsAppFrame(hWnd)) {
                UwpTrackedFrame* tf = UwpFindFrame(hWnd);
                if (!tf) {
                    RECT rc = {};
                    GetWindowRect(hWnd, &rc);
                    int w = (uFlags & SWP_NOSIZE) ? (rc.right - rc.left) : cx;
                    int h = (uFlags & SWP_NOSIZE) ? (rc.bottom - rc.top) : cy;

                    if (w >= 200 && h >= 200 && !IsFrameMaximized(hWnd)) {
                        UwpTrackedFrame* newTf = UwpAllocFrame(hWnd);
                        if (newTf) {
                            HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                            MONITORINFO mi = { sizeof(mi) };
                            if (GetMonitorInfo(hMon, &mi)) {
                                const RECT& wa = mi.rcWork;
                                newTf->centeredX = wa.left + (wa.right - wa.left - w) / 2;
                                newTf->centeredY = wa.top + (wa.bottom - wa.top - h) / 2;
                                newTf->centeredY += g_centerOffsetY.load(std::memory_order_relaxed);
                                newTf->tick = GetTickCount64();
                                uFlags &= ~SWP_NOMOVE;
                                return pOrigSetWindowPos(hWnd, hWndInsertAfter,
                                    newTf->centeredX, newTf->centeredY, cx, cy, uFlags);
                            }
                        }
                    }
                } else if (!(uFlags & SWP_NOMOVE)) {
                    ULONGLONG now = GetTickCount64();
                    if (now - tf->tick < UWP_GRACE_MS) {
                        return pOrigSetWindowPos(hWnd, hWndInsertAfter,
                            tf->centeredX, tf->centeredY, cx, cy, uFlags);
                    }
                }
            }
        }
    }

    // ── Regular window grace period (all processes) ──────────────────
    // Per m417z's review: instead of a shared array (not thread-safe), we
    // store only a timestamp on the HWND via SetProp and recompute the
    // centered position from the current window size here. This is
    // thread-safe because SetProp/GetProp are per-HWND, and recalculating
    // from the current size handles the case where the app resized the
    // window after ShowWindow but before the grace period expires.
    if (!(uFlags & SWP_NOMOVE) && g_graceAtom) {
        HANDLE prop = GetProp(hWnd, reinterpret_cast<LPCWSTR>(g_graceAtom));
        if (prop) {
            DWORD storedTick = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(prop));
            DWORD now        = static_cast<DWORD>(GetTickCount64());
            if (now - storedTick < static_cast<DWORD>(REG_GRACE_MS)) {
                // Grace period still active. Per m417z's review: apply the
                // original call first (with SWP_NOMOVE) so any resize the app
                // requests takes effect, then centre using the final size.
                // Without this order, CenterWindow would use the pre-resize
                // size and the position would be off when cx/cy change.
                pOrigSetWindowPos(hWnd, hWndInsertAfter,
                    X, Y, cx, cy, uFlags | SWP_NOMOVE);
                g_inHook = true;
                CenterWindow(hWnd);
                g_inHook = false;
                return TRUE;
            } else {
                // Grace period expired — remove the prop so future calls pass through.
                RemoveProp(hWnd, reinterpret_cast<LPCWSTR>(g_graceAtom));
            }
        }
    }

    return pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// ════════════════════════════════════════════════════════════════════════════
// UWP timer callback
// ════════════════════════════════════════════════════════════════════════════

static VOID CALLBACK UwpCenterTimerProc(HWND hwnd, UINT, UINT_PTR id, DWORD) {
    KillTimer(hwnd, id);
    if (!IsWindow(hwnd) || !IsAppFrame(hwnd)) return;
    if (UwpFindFrame(hwnd)) return;

    RECT rc = {};
    GetWindowRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    if (w >= 200 && h >= 200) {
        if (IsFrameMaximized(hwnd)) return;
        UwpTrackedFrame* tf = UwpAllocFrame(hwnd);
        if (!tf) return;
        CenterFrameUWP(hwnd, tf);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Hook: ShowWindow — all processes
// ════════════════════════════════════════════════════════════════════════════

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t pOrigShowWindow = nullptr;

BOOL WINAPI HookedShowWindow(HWND hwnd, int nCmdShow) {
    if (g_inHook) goto call_original;

    // ── UWP frame path ───────────────────────────────────────────────
    if (!IsWindowVisible(hwnd) && IsAppFrame(hwnd) &&
        (nCmdShow == SW_SHOW || nCmdShow == SW_SHOWNORMAL ||
         nCmdShow == SW_SHOWNOACTIVATE || nCmdShow == SW_SHOWDEFAULT))
    {
        bool uwpEnabled;
        {
            AcquireSRWLockShared(&g_srw);
            uwpEnabled = g_settings.centerUWP;
            ReleaseSRWLockShared(&g_srw);
        }

        if (uwpEnabled && !UwpFindFrame(hwnd)) {
            g_inHook = true;
            BOOL ret = pOrigShowWindow(hwnd, nCmdShow);

            if (!UwpFindFrame(hwnd)) {
                RECT rc = {};
                GetWindowRect(hwnd, &rc);
                int w = rc.right - rc.left;
                int h = rc.bottom - rc.top;

                if (w >= 200 && h >= 200) {
                    if (!IsFrameMaximized(hwnd)) {
                        UwpTrackedFrame* tf = UwpAllocFrame(hwnd);
                        if (tf) CenterFrameUWP(hwnd, tf);
                    }
                } else {
                    SetTimer(hwnd, 0xCF01, 50,  UwpCenterTimerProc);
                    SetTimer(hwnd, 0xCF02, 150, UwpCenterTimerProc);
                    SetTimer(hwnd, 0xCF03, 300, UwpCenterTimerProc);
                    SetTimer(hwnd, 0xCF04, 500, UwpCenterTimerProc);
                }
            }

            g_inHook = false;
            return ret;
        }
    }

    // ── Regular path: nCmdShow filter ─────────────────────────────────
    switch (nCmdShow) {
        case SW_SHOW: case SW_SHOWNORMAL: case SW_SHOWDEFAULT:
        case SW_SHOWNA: case SW_RESTORE:
            break;
        default:
            goto call_original;
    }

    if (IsWindowVisible(hwnd)) goto call_original;

    // ── Slow path ─────────────────────────────────────────────────────
    {
        Settings s = GetSettingsSnapshot();
        wchar_t dlgClass[256] = {};
        GetClassNameW(hwnd, dlgClass, 256);

        // ── Dialog path ───────────────────────────────────────────────
        if (s.centerDialogs &&
            IsTopLevelDialogWindow(hwnd, dlgClass) &&
            !IsBlocklisted(hwnd, s, dlgClass))
        {
            HWND dlgOwner = GetWindow(hwnd, GW_OWNER);
            if (dlgOwner) {
                wchar_t ownerClass[16] = {};
                GetClassNameW(dlgOwner, ownerClass, 16);
                if (wcscmp(ownerClass, L"#32770") == 0) goto call_original;
            }

            g_inHook = true;
            BOOL result = pOrigShowWindow(hwnd, nCmdShow);
            CenterWindowWithGrace(hwnd);
            g_inHook = false;
            return result;
        }

        // ── Regular window path ───────────────────────────────────────
        if (ShouldHandleWindow(hwnd, s, dlgClass)) {
            const bool alreadyCentered =
                GetProp(hwnd, reinterpret_cast<LPCWSTR>(g_centeredAtom)) != nullptr;

            if (!alreadyCentered)
                SetProp(hwnd, reinterpret_cast<LPCWSTR>(g_centeredAtom),
                        reinterpret_cast<HANDLE>(1));

            g_inHook = true;

            if (s.fadeTrickEnabled) {
                DWORD origExStyle = static_cast<DWORD>(GetWindowLong(hwnd, GWL_EXSTYLE));
                bool wasLayered = (origExStyle & WS_EX_LAYERED) != 0;
                BYTE origAlpha  = 255;
                bool fadeApplied = false;

                if (!wasLayered) {
                    SetWindowLong(hwnd, GWL_EXSTYLE, origExStyle | WS_EX_LAYERED);
                    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                    fadeApplied = true;
                } else {
                    COLORREF key = 0; DWORD flags = 0;
                    if (GetLayeredWindowAttributes(hwnd, &key, &origAlpha, &flags)) {
                        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                        fadeApplied = true;
                    }
                }

                BOOL result = pOrigShowWindow(hwnd, nCmdShow);
                if (!alreadyCentered) ResizeWindow(hwnd, s);

                if (!alreadyCentered)
                    CenterWindowWithGrace(hwnd);
                else
                    CenterWindow(hwnd);

                if (fadeApplied) {
                    auto* d = new (std::nothrow) FadeRestoreData{
                        hwnd, origExStyle, origAlpha,
                        static_cast<DWORD>(s.fadeDelayMs)
                    };
                    if (d) {
                        HANDLE thread = CreateThread(nullptr, 0,
                            FadeRestoreThread, d, 0, nullptr);
                        if (thread) CloseHandle(thread);
                        else {
                            if (wasLayered) SetLayeredWindowAttributes(hwnd, 0, origAlpha, LWA_ALPHA);
                            else SetWindowLong(hwnd, GWL_EXSTYLE, origExStyle);
                            delete d;
                        }
                    } else {
                        if (wasLayered) SetLayeredWindowAttributes(hwnd, 0, origAlpha, LWA_ALPHA);
                        else SetWindowLong(hwnd, GWL_EXSTYLE, origExStyle);
                    }
                }

                g_inHook = false;
                return result;
            } else {
                if (!alreadyCentered) ResizeWindow(hwnd, s);

                if (!alreadyCentered)
                    CenterWindowWithGrace(hwnd);
                else
                    CenterWindow(hwnd);

                g_inHook = false;
            }
        }
    }

call_original:
    return pOrigShowWindow(hwnd, nCmdShow);
}

// ════════════════════════════════════════════════════════════════════════════
// Mod lifecycle
// ════════════════════════════════════════════════════════════════════════════

static void CacheExeName() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring full(path);
    const auto slash = full.find_last_of(L"\\/");
    g_thisExeName = ToLower(
        slash != std::wstring::npos ? full.substr(slash + 1) : full);
}

BOOL Wh_ModInit() {
    CacheExeName();
    g_isExplorer     = (g_thisExeName == L"explorer.exe");
    g_isAppFrameHost = (g_thisExeName == L"applicationframehost.exe");

    if (IsInternallyExcluded()) {
        Wh_Log(L"Center New Windows: skipped (excluded: %s)",
               g_thisExeName.c_str());
        return FALSE;
    }

    g_centeredAtom = GlobalAddAtomW(L"Wh_CenterNewWindows_v1");
    if (!g_centeredAtom) {
        Wh_Log(L"Center New Windows: failed to register centered atom");
        return FALSE;
    }

    g_graceAtom = GlobalAddAtomW(L"Wh_CenterNewWindows_Grace_v1");
    if (!g_graceAtom) {
        Wh_Log(L"Center New Windows: failed to register grace atom");
        GlobalDeleteAtom(g_centeredAtom);
        g_centeredAtom = 0;
        return FALSE;
    }

    LoadSettings();

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(ShowWindow),
            reinterpret_cast<void*>(HookedShowWindow),
            reinterpret_cast<void**>(&pOrigShowWindow)))
    {
        Wh_Log(L"Center New Windows: failed to hook ShowWindow");
        GlobalDeleteAtom(g_graceAtom);   g_graceAtom = 0;
        GlobalDeleteAtom(g_centeredAtom); g_centeredAtom = 0;
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(::SetWindowPos),
            reinterpret_cast<void*>(HookedSetWindowPos),
            reinterpret_cast<void**>(&pOrigSetWindowPos)))
    {
        Wh_Log(L"Center New Windows: failed to hook SetWindowPos");
        GlobalDeleteAtom(g_graceAtom);   g_graceAtom = 0;
        GlobalDeleteAtom(g_centeredAtom); g_centeredAtom = 0;
        return FALSE;
    }

    Wh_Log(L"Center New Windows v1.0.0: ready (process: %s)",
           g_thisExeName.c_str());
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Center New Windows: settings reloaded");
}

void Wh_ModUninit() {
    memset(g_uwpTracked, 0, sizeof(g_uwpTracked));

    if (g_graceAtom) {
        GlobalDeleteAtom(g_graceAtom);
        g_graceAtom = 0;
    }
    if (g_centeredAtom) {
        GlobalDeleteAtom(g_centeredAtom);
        g_centeredAtom = 0;
    }
    Wh_Log(L"Center New Windows: unloaded");
}
