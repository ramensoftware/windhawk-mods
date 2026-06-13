// ==WindhawkMod==
// @id              file-explorer-content-animation
// @name            File Explorer WinUI-Like Content Animation
// @name:pt         Animação de Conteúdo do Explorador de Arquivos estilo WinUI
// @name:es         Animación de Contenido del Explorador de Archivos estilo WinUI
// @description     Smooth WinUI-like page transition animations for the File Explorer content area.
// @description:pt  Animações suaves de transição de página inspiradas no WinUI para a área de conteúdo do Explorador de Arquivos.
// @description:es  Animaciones suaves de transición de página inspiradas en WinUI para el área de contenido del Explorador de Archivos.
// @version         1.0.1
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lole32 -lshell32 -lcomctl32 -ld3d11 -ldxgi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

![Preview](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/file-explorer-content-animation.gif)

---

## English

A smooth, WinUI-inspired page transition system for the File Explorer content area on Windows 11.

### What's new in 1.0.1

- **GPU-accelerated fade** - The fade effect now uses a DirectComposition overlay capturing the previous folder's content (including Mica/Acrylic). The overlay fades out on the GPU compositor thread, fully independent of CPU load from thumbnail rendering.
- **Page transitions** - Detects the navigation type and applies a different animation for each context:
  - *Entrance* (slide up) - Opening Explorer, new tabs, and general navigation.
  - *Enter folder* (short fast slide from right) - Navigating into subfolders.
  - *Back slide* (slide from left) - Going back via button, breadcrumb, or Up arrow.
  - *Forward slide* (slide from right) - Going forward after going back.
- **Reverse direction** - Content can slide down instead of up.
- **Navigation pane animation** - The nav pane tree slides in from the left when opening Explorer or a new tab.
- **Property page animation** - Animates tab switching in folder and file Properties windows.
- Requires symbol resolution on explorerframe.dll (handled automatically by Windhawk).

### Features

- Smooth slide animation with refined easing
- Context-aware page transitions (back, forward, drill-in)
- Optional fade: GPU-accelerated DirectComposition overlay, compatible with Mica and Acrylic
- Per-window first open delay (optional)
- Navigation delay to hide the "Working on it..." message
- Zero flicker / no black flashes
- Compatible with the Translucent Windows mod

### Settings

- **Animation Duration** - Controls how long the slide lasts.
- **Slide Distance** - Controls how far the content moves before settling.
- **Fade** - GPU-accelerated fade overlay showing the previous folder content. Compatible with Mica/Acrylic backdrop mods.
- **Reverse Direction** - Slide down instead of up. Does not apply to property pages or page transitions.
- **Page Transitions** - Smart detection of back/forward/drill navigation with distinct animations.
- **Animate Nav Pane** - Slide the navigation tree in from the left.
- **Animate Property Pages** - Animates tab switching in Properties and Folder Options windows.
- **Property Page Tab Transitions** - Directional animations when switching tabs in Properties windows. Tabs to the right slide forward, to the left slide back. Requires Animate Property Pages.
- **First Open Delay** - Applies only when a new Explorer window is opened.
- **Navigation Delay** - Brief delay during folder switching to hide loading text.

### Navigation pane in dark mode

When the navigation pane animation is enabled in dark mode, a white background may briefly appear
before the tree content fills in. This is a Windows GDI limitation: the system does not respect
the dark theme for the SysTreeView32 background during the first paint. This behaviour has been
observed on stable Windows 11 builds; it appears to be resolved in recent Insider/Beta builds.

---

## Portugues

Uma animacao suave de transicao de pagina inspirada no WinUI para o Explorador de Arquivos no Windows 11.

### Novidades na 1.0.1

- **Fade acelerado por GPU** - O fade agora usa um overlay DirectComposition que captura o conteudo da pasta anterior (incluindo Mica/Acrilico). O overlay desaparece no compositor GPU, completamente independente da carga de CPU do carregamento de miniaturas.
- **Transicoes de pagina** - Detecta o tipo de navegacao e aplica uma animacao diferente para cada contexto:
  - *Entrance* (deslizar para cima) - Ao abrir o Explorer, novas abas e navegacao geral.
  - *Entrar na pasta* (slide curto e rapido pela direita) - Ao navegar para subpastas.
  - *Back slide* (deslizar da esquerda) - Ao voltar via botao, breadcrumb ou seta para cima.
  - *Forward slide* (deslizar da direita) - Ao avancar apos ter voltado.
- **Direcao reversa** - O conteudo pode descer ao inves de subir.
- **Animacao do painel de navegacao** - A arvore de navegacao desliza da esquerda ao abrir o Explorer ou nova aba.
- **Animacao de paginas de propriedades** - Anima a troca de abas em janelas de Propriedades e Opcoes de Pasta.
- Requer resolucao de simbolos no explorerframe.dll (feito automaticamente pelo Windhawk).

### Recursos

- Animacao suave com easing refinado
- Transicoes de pagina contextuais (voltar, avancar, subpasta)
- Fade opcional: overlay DirectComposition acelerado por GPU, compativel com Mica e Acrilico
- Delay opcional na primeira abertura
- Delay de navegacao para esconder o "Trabalhando nisso..."
- Sem flicker / sem flashes pretos
- Compativel com o mod Translucent Windows

### Configuracoes

- **Duracao da Animacao** - Define quanto tempo a animacao leva.
- **Distancia do Slide** - Define o deslocamento inicial.
- **Fade** - Overlay de fade GPU mostrando o conteudo da pasta anterior. Compativel com mods de backdrop Mica/Acrilico.
- **Direcao Reversa** - Faz o conteudo descer ao inves de subir. Nao se aplica a paginas de Propriedades nem a transicoes de pagina.
- **Transicoes de Pagina** - Deteccao inteligente de navegacao voltar/avancar/subpasta com animacoes distintas.
- **Animar Painel de Navegacao** - Desliza a arvore de navegacao da esquerda.
- **Animar Paginas de Propriedades** - Anima a troca de abas em Propriedades e Opcoes de Pasta.
- **Transicoes de Aba nas Propriedades** - Animacoes direcionais ao trocar abas nas janelas de Propriedades. Aba a direita desliza para frente, a esquerda para tras. Requer Animar Paginas de Propriedades ativo.
- **Delay na Primeira Abertura** - Aplicado apenas ao abrir uma nova janela.
- **Delay ao Trocar de Pasta** - Pequeno atraso para esconder o texto de carregamento.

### Painel de navegacao no modo escuro

Com a animacao do painel de navegacao ativa no modo escuro, um fundo branco pode aparecer
brevemente antes dos itens da arvore serem exibidos. Isso e uma limitacao do GDI do Windows:
o sistema nao respeita o tema escuro para o fundo do SysTreeView32 durante o primeiro paint.
Esse comportamento foi observado em builds estaveis do Windows 11; parece estar resolvido em
builds recentes do canal Insider/Beta.

---

## Espanol

Una animacion suave de transicion de pagina inspirada en WinUI para el Explorador de Archivos en Windows 11.

### Novedades en 1.0.1

- **Fundido acelerado por GPU** - El fundido ahora usa un overlay DirectComposition que captura el contenido de la carpeta anterior (incluido Mica/Acrilico). El overlay desaparece en el compositor GPU, completamente independiente de la carga de CPU del renderizado de miniaturas.
- **Transiciones de pagina** - Detecta el tipo de navegacion y aplica una animacion diferente para cada contexto:
  - *Entrance* (deslizar hacia arriba) - Al abrir el Explorador, nuevas pestanas y navegacion general.
  - *Entrar a carpeta* (deslizamiento corto y rapido desde la derecha) - Al navegar a subcarpetas.
  - *Back slide* (deslizar desde la izquierda) - Al volver via boton, breadcrumb o flecha arriba.
  - *Forward slide* (deslizar desde la derecha) - Al avanzar despues de haber retrocedido.
- **Direccion inversa** - El contenido puede deslizarse hacia abajo en lugar de hacia arriba.
- **Animacion del panel de navegacion** - El arbol de navegacion se desliza desde la izquierda al abrir el Explorador o una nueva pestana.
- **Animacion de paginas de propiedades** - Anima el cambio de pestanas en ventanas de Propiedades y Opciones de carpeta.
- Requiere resolucion de simbolos en explorerframe.dll (manejado automaticamente por Windhawk).

### Caracteristicas

- Animacion suave con easing refinado
- Transiciones de pagina contextuales (atras, adelante, subcarpeta)
- Fundido opcional: overlay DirectComposition acelerado por GPU, compatible con Mica y Acrilico
- Retraso opcional en la primera apertura
- Retraso de navegacion para ocultar el texto de carga
- Sin parpadeo / sin destellos negros
- Compatible con el mod Translucent Windows

### Configuracion

- **Duracion de la Animacion** - Controla cuanto dura el efecto.
- **Distancia del Deslizamiento** - Define el desplazamiento inicial.
- **Fundido** - Overlay de fundido GPU mostrando el contenido de la carpeta anterior. Compatible con mods de backdrop Mica/Acrilico.
- **Direccion Inversa** - Desliza hacia abajo en lugar de hacia arriba. No se aplica a paginas de Propiedades ni a transiciones de pagina.
- **Transiciones de Pagina** - Deteccion inteligente de navegacion atras/adelante/subcarpeta con animaciones distintas.
- **Animar Panel de Navegacion** - Desliza el arbol desde la izquierda.
- **Animar Paginas de Propiedades** - Anima el cambio de pestanas en Propiedades.
- **Transiciones de Pestana en Propiedades** - Animaciones direccionales al cambiar pestanas en ventanas de Propiedades. Las pestanas a la derecha deslizan hacia adelante, a la izquierda hacia atras. Requiere Animar Paginas de Propiedades activado.
- **Retraso en la Primera Apertura** - Solo al abrir una nueva ventana.
- **Retraso al Cambiar de Carpeta** - Pequeno retraso para ocultar el texto de carga.

### Panel de navegacion en modo oscuro

Con la animacion del panel de navegacion activa en modo oscuro, puede aparecer brevemente un
fondo blanco antes de que se muestren los elementos del arbol. Esta es una limitacion del GDI
de Windows: el sistema no respeta el tema oscuro para el fondo del SysTreeView32 durante el
primer pintado. Este comportamiento se observo en builds estables de Windows 11; parece estar
resuelto en builds recientes del canal Insider/Beta.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration: 420
  $name: Animation duration (ms)
  $name:pt: Duracao da animacao (ms)
  $name:es: Duracion de la animacion (ms)

- distance: 32
  $name: Slide distance (px)
  $name:pt: Distancia do slide (px)
  $name:es: Distancia del deslizamiento (px)

- fade: false
  $name: Fade
  $name:pt: Fade
  $name:es: Fundido
  $description: Fades the background out smoothly while navigating, using a DirectComposition opacity animation (GPU). Compatible with Mica, Acrylic and transparency mods.
  $description:pt: Faz o fundo desaparecer gradualmente ao navegar, usando animacao de opacidade via DirectComposition (GPU). Compativel com Mica, Acrilico e mods de transparencia.
  $description:es: Desvanece el fondo suavemente al navegar, usando animacion de opacidad via DirectComposition (GPU). Compatible con Mica, Acrilico y mods de transparencia.

- reverseDirection: false
  $name: Reverse direction
  $name:pt: Direcao reversa
  $name:es: Direccion inversa
  $description: Content slides down instead of up. Does not apply to Properties pages, the navigation pane, or page transitions (back/forward/drill).
  $description:pt: O conteudo desce ao inves de subir. Nao se aplica as paginas de Propriedades, ao painel de navegacao, nem as transicoes de pagina (voltar/avancar/drill).
  $description:es: El contenido se desliza hacia abajo en lugar de hacia arriba. No se aplica a las paginas de Propiedades, al panel de navegacion, ni a las transiciones de pagina (atras/adelante/drill).

- pageTransitions: false
  $name: Page transitions
  $name:pt: Transicoes de pagina
  $name:es: Transiciones de pagina
  $description: Automatically detects the navigation type and applies different animations. Back slides from the left, forward from the right, subfolders use a short fast slide. Requires symbol resolution on explorerframe.dll (automatic). When disabled, all navigations use the default entrance animation.
  $description:pt: Detecta automaticamente o tipo de navegacao e aplica animacoes diferentes. Voltar desliza da esquerda, avancar da direita, subpasta usa slide curto e rapido. Requer resolucao de simbolos no explorerframe.dll (automatico). Quando desativado, todas as navegacoes usam a animacao de entrada padrao.
  $description:es: Detecta automaticamente el tipo de navegacion y aplica animaciones diferentes. Atras desliza desde la izquierda, adelante desde la derecha, subcarpetas usan deslizamiento corto y rapido. Requiere resolucion de simbolos en explorerframe.dll (automatico). Cuando esta desactivado, todas las navegaciones usan la animacion de entrada predeterminada.

- animateNavPane: false
  $name: Animate navigation pane
  $name:pt: Animar painel de navegacao
  $name:es: Animar panel de navegacion
  $description: The navigation tree slides in from the left when opening Explorer or a new tab. In dark mode, a white background may briefly appear -- a Windows GDI limitation observed on stable builds. Appears resolved in recent Insider/Beta builds.
  $description:pt: A arvore de navegacao desliza da esquerda ao abrir o Explorer ou uma nova aba. No modo escuro, um fundo branco pode aparecer brevemente -- limitacao do GDI observada em builds estaveis. Parece resolvido em builds recentes do canal Insider/Beta.
  $description:es: El arbol de navegacion se desliza desde la izquierda al abrir el Explorador o una nueva pestana. En modo oscuro, puede aparecer brevemente un fondo blanco -- limitacion del GDI observada en builds estables. Parece resuelto en builds recientes del canal Insider/Beta.

- propertyPages:
    - animate: false
      $name: Animate property pages
      $name:pt: Animar paginas de propriedades
      $name:es: Animar paginas de propiedades
      $description: Animates tab switching in folder and file Properties windows (General, Tools, etc.). Reverse direction does not apply here.
      $description:pt: Anima a troca de abas em janelas de Propriedades de pasta e arquivo (Geral, Ferramentas, etc.). A direcao reversa nao se aplica aqui.
      $description:es: Anima el cambio de pestanas en ventanas de Propiedades de carpeta y archivo (General, Herramientas, etc.). La direccion inversa no se aplica aqui.
    - tabTransitions: false
      $name: Tab transitions
      $name:pt: Transicoes de aba
      $name:es: Transiciones de pestana
      $description: Applies directional animations when switching tabs. Tabs to the right slide forward, to the left slide back. Requires "Animate property pages" to be enabled.
      $description:pt: Aplica animacoes direcionais ao trocar abas. Aba a direita desliza para frente, a esquerda para tras. Requer "Animar paginas de propriedades" ativo.
      $description:es: Aplica animaciones direccionales al cambiar pestanas. Las pestanas a la derecha deslizan hacia adelante, a la izquierda hacia atras. Requiere "Animar paginas de propiedades" activado.
  $name: Property Pages
  $name:pt: Paginas de Propriedades
  $name:es: Paginas de Propiedades

- firstOpenDelay: 500
  $name: First open delay (ms)
  $name:pt: Delay na primeira abertura (ms)
  $name:es: Retraso en la primera apertura (ms)
  $description: Delay applied only to the first animation of each Explorer window.
  $description:pt: Atraso aplicado apenas na primeira animacao de cada janela do Explorer.
  $description:es: Retraso aplicado solo en la primera animacion de cada ventana del Explorador.

- navigationDelay: 140
  $name: Folder navigation delay (ms)
  $name:pt: Delay ao trocar de pasta (ms)
  $name:es: Retraso al cambiar de carpeta (ms)
  $description: Time used to hide the "Working on it..." message during navigation.
  $description:pt: Tempo para esconder o "Trabalhando nisso..." durante navegacao.
  $description:es: Tiempo utilizado para ocultar el mensaje "Trabajando en ello..." durante la navegacion.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <dcomp.h>
#include <dxgi.h>
#include <d3d11.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>     // IShellWindows
#include <servprov.h>   // IServiceProvider
#include <commctrl.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// -----------------------------------------------
// Tipo de transicao de pagina
// -----------------------------------------------

enum class TransitionType {
    Entrance,     // slide vertical padrao (abrir janela, aba, navegacao geral)
    EnterFolder,  // slide vertical rapido (entrar em subpasta)
    SlideBack,    // slide horizontal da esquerda (voltar)
    SlideForward  // slide horizontal da direita (avancar)
};

// -----------------------------------------------
// Configuracoes
// -----------------------------------------------

struct Settings {
    int  duration;
    int  distance;
    int  firstOpenDelay;
    int  navigationDelay;
    bool alphaFade;
    bool reverseDirection;
    bool pageTransitions;
    bool animateNavPane;
    bool animatePropertyPages;
    bool propPageTransitions;
};

static Settings g_s{420, 32, 500, 140, false, false, false, false, false, false};
static SRWLOCK  g_sLock = SRWLOCK_INIT;


static void LoadSettings() {
    Settings fresh;
    fresh.duration             = std::clamp(Wh_GetIntSetting(L"duration"),           60,  800);
    fresh.distance             = std::clamp(Wh_GetIntSetting(L"distance"),             0,  120);
    fresh.firstOpenDelay       = std::clamp(Wh_GetIntSetting(L"firstOpenDelay"),       0, 5000);
    fresh.navigationDelay      = std::clamp(Wh_GetIntSetting(L"navigationDelay"),      0,  500);
    fresh.alphaFade            = !!Wh_GetIntSetting(L"fade");
    fresh.reverseDirection     = !!Wh_GetIntSetting(L"reverseDirection");
    fresh.pageTransitions      = !!Wh_GetIntSetting(L"pageTransitions");
    fresh.animateNavPane       = !!Wh_GetIntSetting(L"animateNavPane");
    fresh.animatePropertyPages = !!Wh_GetIntSetting(L"propertyPages.animate");
    fresh.propPageTransitions  = !!Wh_GetIntSetting(L"propertyPages.tabTransitions");

    AcquireSRWLockExclusive(&g_sLock);
    g_s = fresh;
    ReleaseSRWLockExclusive(&g_sLock);
}

// -----------------------------------------------
// Easing
// -----------------------------------------------

// Ease-out cubico com ramp de arranque nos primeiros 12%.
// A ramp quadratica inicial (0 a 5% do percurso) evita o salto brusco
// do primeiro frame; o restante usa ease-out cubico (inv^3) que
// desacelera de forma gradual -- o mesmo padrao visual do WinUI, sem
// comprimir o movimento principal como curvas de potencia maior fariam.
static inline float EaseOutCubicWithRamp(float t) {
    const float rampZone = 0.12f;
    if (t < rampZone) {
        float local = t / rampZone;
        return local * local * 0.05f;
    }
    float adjusted = (t - rampZone) / (1.f - rampZone);
    float inv = 1.f - adjusted;
    return 0.05f + (1.f - inv * inv * inv) * 0.95f;
}

// Ease-out cubico puro. Usado para back/forward (sem ramp) e para o alpha fade.
static inline float EaseOutCubic(float t) {
    float inv = 1.f - t;
    return 1.f - inv * inv * inv;
}

// -----------------------------------------------
// Estado de animacao
//
// g_unloading: sinaliza para threads em voo que o mod esta sendo
// descarregado. Threads checam isso antes de acessar estado global.
// -----------------------------------------------

static std::unordered_set<HWND>       g_animating;
static std::mutex                      g_animMtx;
static std::condition_variable         g_animDoneCv;   // notificada quando g_animating encolhe
static std::atomic<bool>               g_unloading{false};

// Permite que SleepUntil seja interrompido imediatamente quando o mod
// comeca a descarregar, evitando use-after-free com firstOpenDelay longo.
static std::mutex              g_sleepMtx;
static std::condition_variable g_sleepCv;

// -----------------------------------------------
// Transicao pendente por root window
//
// Gravada pelo hook de BrowseObject antes da navegacao executar.
// Consumida por Classify() quando o novo conteudo fica visivel.
// Expira apos 3s para evitar transicoes orfas.
// -----------------------------------------------

struct PendingNav {
    TransitionType type;
    ULONGLONG      timestamp;
};

static std::unordered_map<HWND, PendingNav> g_pendingNav;
static std::mutex                           g_pendingMtx;

static constexpr ULONGLONG PENDING_EXPIRE_MS = 3000;

// -----------------------------------------------
// Subclass de CabinetWClass para interceptar WM_APPCOMMAND
//
// Fallback para quando o PDB de explorerframe.dll nao esta disponivel
// (comum em builds Insider/Beta). Captura Back e Forward sem precisar
// de simbolo, usando a mensagem padrao do sistema.
//
// WM_APPCOMMAND dispara ANTES da navegacao, entao gravamos o tipo
// pendente antes do ShowWindow/SetWindowPos do novo conteudo disparar.
// hwnd no subclass proc = CabinetWClass = GetAncestor(content, GA_ROOT),
// o mesmo root que ConsumePendingTransition usa. Mapeamento correto.
// -----------------------------------------------

// Mensagens customizadas registadas via RegisterWindowMessageW para evitar
// colisao com WM_USER+N que CabinetWClass ou outros mods possam usar.
// Valores no range 0xC000-0xFFFF, unicos por processo.
static UINT WM_WH_REMOVE_SUBCLASS = 0;
static UINT WM_WH_TRY_VTABLE_HOOK = 0;


// Forward declarations -- definidos mais abaixo, usados no subclass proc
// e em TrySubclassExplorerWindow que aparecem antes no arquivo.
using BrowseObject_t = HRESULT(WINAPI*)(void*, LPCITEMIDLIST, UINT);
static BrowseObject_t g_origBrowseObject = nullptr;
static void TryHookBrowseObjectViaVtable();
static constexpr UINT_PTR APPCOMMAND_SUBCLASS_ID = 0xFECA;

static std::unordered_set<HWND> g_subclassedWindows;
static std::mutex                g_subclassMtx;

// Estado de transicao de abas por PropertySheet.
// pendingOldIndex: indice capturado em TCN_SELCHANGING e consumido em Classify.
// -1 significa primeira abertura (Entrance) ou sem transicao pendente.
// hTab e buttonRowTop sao cacheados na primeira subclasse -- PropertySheets
// tem tamanho fixo, entao buttonRowTop nao muda durante a vida da janela.
struct PropSheetState {
    HWND hTab;            // SysTabControl32 cacheado
    int  buttonRowTop;    // posicao Y do row de botoes, cacheada na subclasse
    int  pendingOldIndex; // -1 = primeira abertura ou sem pendencia
};
static std::unordered_map<HWND, PropSheetState> g_propSheets;
static std::mutex                                g_propSheetMtx;

static constexpr UINT_PTR PROPSHEET_SUBCLASS_ID = 0xFECB;

static LRESULT CALLBACK ExplorerSubclassProc(HWND hwnd, UINT msg,
    WPARAM wp, LPARAM lp, UINT_PTR) {
    if (msg == WM_APPCOMMAND && !g_unloading.load(std::memory_order_relaxed)) {
        int cmd = GET_APPCOMMAND_LPARAM(lp);
        if (cmd == APPCOMMAND_BROWSER_BACKWARD ||
            cmd == APPCOMMAND_BROWSER_FORWARD) {
            TransitionType type = (cmd == APPCOMMAND_BROWSER_BACKWARD)
                                  ? TransitionType::SlideBack
                                  : TransitionType::SlideForward;
            Wh_Log(L"[APPCOMMAND] hwnd=%p cmd=%d -> %s", hwnd, cmd,
                   type == TransitionType::SlideBack ? L"SlideBack" : L"SlideForward");
            std::lock_guard<std::mutex> lk(g_pendingMtx);
            g_pendingNav[hwnd] = {type, GetTickCount64()};
        }
    }
    // Retry do hook via vtable apos a janela estar totalmente inicializada.
    // Executado no thread STA do Explorer onde COM ja esta ativo.
    if (msg == WM_WH_TRY_VTABLE_HOOK) {
        if (!g_origBrowseObject) {
            TryHookBrowseObjectViaVtable();
            if (g_origBrowseObject)
                Wh_ApplyHookOperations();
        }
        return 0;
    }
    // Solicitacao de remocao antes do DLL descarregar (enviada por Wh_ModBeforeUninit).
    if (msg == WM_WH_REMOVE_SUBCLASS) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        g_subclassedWindows.erase(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        // RemoveWindowSubclassFromAnyThread garante que a subclass e
        // removida com os parametros correctos (SubclassProcWrapper + id).
        // Sem isso, a limpeza dependeria exclusivamente do auto-remove em
        // WM_NCDESTROY, o que e seguro mas atrasa a remocao.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        g_subclassedWindows.erase(hwnd);
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Subclass proc de PropertySheet: interceta TCN_SELCHANGING para capturar
// o indice da aba anterior antes do CommCtrl mostrar a nova pagina.
// Corre no UI thread do Explorer -- sem concorrencia com Classify.
static LRESULT CALLBACK PropSheetSubclassProc(HWND hwnd, UINT msg,
    WPARAM wp, LPARAM lp, UINT_PTR) {
    if (msg == WM_NOTIFY && !g_unloading.load(std::memory_order_relaxed)) {
        auto* hdr = reinterpret_cast<NMHDR*>(lp);
        if (hdr && hdr->code == TCN_SELCHANGING) {
            std::lock_guard<std::mutex> lk(g_propSheetMtx);
            auto it = g_propSheets.find(hwnd);
            // Verificar que e o tab control correto (PropertySheet pode ter
            // outros controlos que emitem WM_NOTIFY).
            if (it != g_propSheets.end() && it->second.hTab == hdr->hwndFrom)
                it->second.pendingOldIndex = TabCtrl_GetCurSel(hdr->hwndFrom);
        }
    }
    if (msg == WM_WH_REMOVE_SUBCLASS) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, PropSheetSubclassProc);
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        g_propSheets.erase(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, PropSheetSubclassProc);
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        g_propSheets.erase(hwnd);
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Subclasseia um PropertySheet na primeira vez que uma das suas tab pages
// e classificada para animacao. hTab e buttonRowTop sao cacheados para evitar
// enumeracao de filhos nas trocas subsequentes. Nao-op se ja subclassado.
static void TrySubclassPropSheet(HWND hwndSheet, HWND hTab, int buttonRowTop) {
    std::lock_guard<std::mutex> lk(g_propSheetMtx);
    if (g_propSheets.count(hwndSheet)) return;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwndSheet, PropSheetSubclassProc, PROPSHEET_SUBCLASS_ID))
        g_propSheets[hwndSheet] = {hTab, buttonRowTop, -1};
}

static void TrySubclassExplorerWindow(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        if (g_subclassedWindows.count(hwnd)) return;
    }
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd, ExplorerSubclassProc, APPCOMMAND_SUBCLASS_ID)) {
        Wh_Log(L"[SUBCLASS] CabinetWClass hwnd=%p subclassed OK", hwnd);
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        g_subclassedWindows.insert(hwnd);
    } else {
        Wh_Log(L"[SUBCLASS] falhou para hwnd=%p", hwnd);
        return;
    }
    // Se o hook via vtable ainda nao foi instalado, postar uma mensagem
    // para retry depois que a janela estiver totalmente inicializada.
    // PostMessage garante execucao no thread STA do Explorer, com
    // a janela ja registrada no IShellWindows.
    if (!g_origBrowseObject)
        PostMessageW(hwnd, WM_WH_TRY_VTABLE_HOOK, 0, 0);
}

// -----------------------------------------------
// Batch por root window
// -----------------------------------------------

static constexpr ULONGLONG BATCH_WINDOW_MS = 500;

struct BatchInfo { ULONGLONG startAt, createdAt; };

static std::unordered_set<HWND>            g_rootSeen;
static std::unordered_map<HWND, BatchInfo> g_rootBatch;
static std::mutex                          g_rootMtx;

static ULONGLONG GetBatchStartAt(HWND root) {
    // Snapshot dos delays sob lock antes de adquirir g_rootMtx para
    // evitar inversao de ordem de lock (g_sLock -> g_rootMtx).
    int navDelay, firstDelay;
    AcquireSRWLockShared(&g_sLock);
    navDelay   = g_s.navigationDelay;
    firstDelay = g_s.firstOpenDelay;
    ReleaseSRWLockShared(&g_sLock);

    std::lock_guard<std::mutex> lk(g_rootMtx);
    // Limpeza periodica de HWNDs invalidos (janelas fechadas).
    // Executada no maximo uma vez a cada 10s para evitar IsWindow()
    // em pastas com muitos arquivos onde animacoes disparam frequentemente.
    static ULONGLONG lastCleanup = 0;
    ULONGLONG nowClean = GetTickCount64();
    if (nowClean - lastCleanup >= 10000) {
        lastCleanup = nowClean;
        for (auto it = g_rootSeen.begin();  it != g_rootSeen.end();  )
            it = IsWindow(*it)       ? ++it : g_rootSeen.erase(it);
        for (auto it = g_rootBatch.begin(); it != g_rootBatch.end(); )
            it = IsWindow(it->first) ? ++it : g_rootBatch.erase(it);
    }
    ULONGLONG now = GetTickCount64();
    auto it = g_rootBatch.find(root);
    if (it != g_rootBatch.end() && (now - it->second.createdAt) < BATCH_WINDOW_MS)
        return it->second.startAt;
    bool      seen    = g_rootSeen.count(root) > 0;
    int       delay   = seen ? navDelay : firstDelay;
    g_rootSeen.insert(root);
    ULONGLONG startAt = now + (ULONGLONG)delay;
    g_rootBatch[root] = {startAt, now};
    return startAt;
}

static void SleepUntil(ULONGLONG startAt) {
    ULONGLONG now = GetTickCount64();
    if (now >= startAt) return;
    // Usa condition_variable para que Wh_ModBeforeUninit possa acordar
    // este thread imediatamente ao sinalizar g_unloading, independente
    // de quao longo seja firstOpenDelay (pode ser ate 5000ms).
    std::unique_lock<std::mutex> lk(g_sleepMtx);
    g_sleepCv.wait_for(lk,
        std::chrono::milliseconds(startAt - now),
        [] { return g_unloading.load(std::memory_order_relaxed); });
}

// -----------------------------------------------
// Eixo de animacao
// -----------------------------------------------

enum class AnimAxis { Vertical, Horizontal };

// -----------------------------------------------
// Deteccao de janelas
// -----------------------------------------------

static bool IsExplorerContent(HWND hwnd) {
    wchar_t c[128] = {}; RealGetWindowClassW(hwnd, c, 128);
    if (wcscmp(c, L"UIItemsView") &&
        wcscmp(c, L"ItemsView")   &&
        wcscmp(c, L"SHELLDLL_DefView"))
        return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    wchar_t rc[64] = {}; RealGetWindowClassW(root, rc, 64);
    return !wcscmp(rc, L"CabinetWClass");
}

static bool IsNavPaneContent(HWND hwnd, bool animateNavPane) {
    if (!animateNavPane) return false;
    wchar_t c[128] = {}; RealGetWindowClassW(hwnd, c, 128);
    if (wcscmp(c, L"SysTreeView32") && wcscmp(c, L"UITreeView"))
        return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    wchar_t rc[64] = {}; RealGetWindowClassW(root, rc, 64);
    return !wcscmp(rc, L"CabinetWClass");
}

static bool IsPropertyPage(HWND hwnd, bool animatePropertyPages) {
    if (!animatePropertyPages) return false;
    wchar_t c[128]; GetClassNameW(hwnd, c, 128);
    if (wcscmp(c, L"#32770")) return false;
    // Tab pages sao WS_CHILD do PropertySheet. Sub-dialogs owned (ex:
    // "Atributos Avancados") sao WS_POPUP top-level -- RunSlideIn usa
    // ScreenToClient(parent) que e invalido para janelas top-level,
    // quebrando o posicionamento e a renderizacao DWM durante a animacao.
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) return false;
    HWND parent = GetParent(hwnd);
    if (!parent) return false;
    wchar_t pc[128]; GetClassNameW(parent, pc, 128);
    if (wcscmp(pc, L"#32770")) return false;
    HWND child = GetWindow(parent, GW_CHILD);
    while (child) {
        wchar_t sc[64] = {}; GetClassNameW(child, sc, 64);
        if (!wcscmp(sc, L"SysTabControl32")) return true;
        child = GetWindow(child, GW_HWNDNEXT);
    }
    return false;
}

// -----------------------------------------------
// SetWindowRgn helpers
// -----------------------------------------------

// Oculta a janela antes de ShowWindow para evitar o flash do primeiro frame.
// SetWindowRgn assume ownership do HRGN em sucesso; libera em falha.
static void HideViaRgn(HWND hwnd) {
    HRGN empty = CreateRectRgn(0, 0, 0, 0);
    if (empty && !SetWindowRgn(hwnd, empty, FALSE))
        DeleteObject(empty);
}

static void RestoreRgn(HWND hwnd) {
    SetWindowRgn(hwnd, nullptr, TRUE);
}

// -----------------------------------------------
// DirectComposition fade overlay
//
// Substitui o fade CPU (WS_EX_LAYERED + SetLayeredWindowAttributes por frame)
// por um overlay DComp de opacidade 1->0, correndo inteiramente na GPU.
//
// Fluxo:
//   1. Antes de ShowWindow: captura o ecra na posicao da janela (inclui
//      Mica/Acrylic via DC do ecra composto pelo DWM)
//   2. Cria target DComp topmost sobre o HWND com o bitmap capturado
//   3. Anima opacidade 1->0 em 83ms (spec WinUI) via IDCompositionEffectGroup
//   4. ShowWindow + RunSlideIn correm normalmente (sem BeginFade/SetAlpha/EndFade)
//   5. Timer leve destroi recursos DComp apos 83ms + margem
//
// Compativel com Mica, Acrylic e mods de transparencia porque:
//   - Nao aplica WS_EX_LAYERED na janela de conteudo
//   - O overlay e uma layer DComp separada, nao interfere com o backdrop DWM
// -----------------------------------------------

// Duracao do fade: 34% da duracao total da animacao.
// Minimo de 50ms para ser perceptivel; maximo de effectiveDuration.
static int CalcFadeDuration(int effectiveDuration) {
    return std::max(50, std::min(effectiveDuration,
                                 (int)(effectiveDuration * 0.34f)));
}

static ID3D11Device*               g_d3dDevice  = nullptr;
static IDXGIDevice*                g_dxgiDevice = nullptr;
static std::atomic<IDCompositionDesktopDevice*> g_dcDevice{nullptr};

struct DCompFadeAnim {
    IDCompositionTarget*      pTarget  = nullptr;
    IDCompositionVisual2*     pVisual  = nullptr;
    IDCompositionSurface*     pSurface = nullptr;
    IDCompositionEffectGroup* pEffect  = nullptr;
    uint64_t                  generation = 0; // identifica instancia unica do fade
};

static std::mutex                               g_dcFadeMtx;
static std::unordered_map<HWND, DCompFadeAnim>  g_dcFades;
static std::atomic<uint64_t>                    g_dcFadeGen{0}; // contador de geracao

// Worker de cleanup DComp: substitui thread detachada por navegacao por um
// unico thread de longa duracao. Elimina criacao de thread em navegacao rapida.
// A fila armazena refs de device AddRef'd -- sempre libertados no cleanup ou drain.
// hwndRoot e a chave usada para consultar g_dcFades no momento do cleanup.
// generation identifica a instancia exata do fade que este cleanup deve destruir --
// se a geracao no mapa nao bater (nova navegacao substituiu o fade), o cleanup
// apenas liberta o ref do device sem destruir o visual corrente.
struct PendingCleanup {
    HWND                        hwndRoot;
    IDCompositionDesktopDevice* dev;      // AddRef'd
    ULONGLONG                   destroyAt;
    uint64_t                    generation;
};
static std::vector<PendingCleanup> g_cleanupQueue;
static std::mutex                  g_cleanupMtx;
static std::condition_variable     g_cleanupCv;
static std::thread*                g_cleanupThread = nullptr;
static std::thread*                g_initThread    = nullptr; // D3D/DComp init em background

// Frame capturado no BrowseObject hook (conteudo antigo visivel e dimensionado).
// Consumido em HookShowWindow para criar o overlay DComp.
struct CapturedFrame {
    HBITMAP hbm      = nullptr;
    POINT   ptInRoot = {};   // offset do conteudo dentro do root (CabinetWClass)
    int     w        = 0;
    int     h        = 0;
};

// Worker de cleanup: unico thread de longa duracao que substitui N threads
// detachadas. Dorme ate o proximo item ficar pronto, processa em batch,
// e drena a fila em shutdown (join ocorre antes de Wh_ModUninit libertar devices).
//
// Ordem de locks: StartDCompFade adquire g_dcFadeMtx como lock externo (cobre
// TODAS as chamadas DComp) e depois g_cleanupMtx (aninhado, para enfileirar).
// CleanupWorkerProc liberta g_cleanupMtx (lk.unlock) antes de adquirir
// g_dcFadeMtx para teardown -- nunca aninhados no worker, sem inversao.
static void CleanupWorkerProc() {
    std::unique_lock<std::mutex> lk(g_cleanupMtx);
    for (;;) {
        // Dorme enquanto a fila estiver vazia e o mod nao estiver a descarregar.
        g_cleanupCv.wait(lk, [] {
            return g_unloading.load(std::memory_order_relaxed) ||
                   !g_cleanupQueue.empty();
        });

        if (g_unloading.load(std::memory_order_relaxed)) {
            // Drenagem em shutdown: processar tudo imediatamente.
            // g_dcDevice ainda valido aqui -- join ocorre antes de Wh_ModUninit.
            std::vector<PendingCleanup> drain = std::move(g_cleanupQueue);
            lk.unlock();
            for (auto& c : drain) {
                std::lock_guard<std::mutex> lkf(g_dcFadeMtx);
                auto it = g_dcFades.find(c.hwndRoot);
                if (it != g_dcFades.end() &&
                    it->second.generation == c.generation) {
                    DCompFadeAnim a = it->second;
                    g_dcFades.erase(it);
                    if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
                    if (a.pEffect)  a.pEffect->Release();
                    if (a.pVisual)  a.pVisual->Release();
                    if (a.pSurface) a.pSurface->Release();
                    if (c.dev) { c.dev->Commit(); c.dev->Release(); }
                } else {
                    // Fade cancelado por nova navegacao (geracao diferente ou
                    // entrada ausente): resources ja libertados em StartDCompFade.
                    if (c.dev) c.dev->Release();
                }
            }
            return;
        }

        // Calcular o destroyAt mais proximo entre os itens pendentes.
        ULONGLONG nearest = (ULONGLONG)(~0ULL);
        for (auto& c : g_cleanupQueue)
            if (c.destroyAt < nearest) nearest = c.destroyAt;

        ULONGLONG now = GetTickCount64();
        if (now < nearest) {
            // Dorme ate o proximo item (ou nova notificacao/stop).
            // wait_for retorna true se g_unloading -> volta ao topo via continue.
            // wait_for retorna false (timeout ou novo item) -> continua para collect.
            bool stopping = g_cleanupCv.wait_for(lk,
                std::chrono::milliseconds(nearest - now),
                [] { return g_unloading.load(std::memory_order_relaxed); });
            // Em ambos os casos volta ao topo: stop trata-se la, novo item
            // com destroyAt mais cedo e re-avaliado na proxima iteracao.
            (void)stopping;
            continue;
        }

        // Coletar todos os itens cujo tempo chegou.
        std::vector<PendingCleanup> ready;
        now = GetTickCount64();
        for (auto it = g_cleanupQueue.begin(); it != g_cleanupQueue.end(); ) {
            if (it->destroyAt <= now) {
                ready.push_back(*it);
                it = g_cleanupQueue.erase(it);
            } else {
                ++it;
            }
        }
        lk.unlock();

        for (auto& c : ready) {
            std::lock_guard<std::mutex> lkf(g_dcFadeMtx);
            auto it = g_dcFades.find(c.hwndRoot);
            if (it != g_dcFades.end() &&
                it->second.generation == c.generation) {
                DCompFadeAnim a = it->second;
                g_dcFades.erase(it);
                if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
                if (a.pEffect)  a.pEffect->Release();
                if (a.pVisual)  a.pVisual->Release();
                if (a.pSurface) a.pSurface->Release();
                if (c.dev) { c.dev->Commit(); c.dev->Release(); }
            } else {
                // Fade cancelado por nova navegacao rapida (geracao diferente
                // ou entrada ausente): resources ja libertados em
                // StartDCompFade. Libertar apenas o ref do device.
                if (c.dev) c.dev->Release();
            }
        }

        lk.lock();
    }
}

// Inicia o fade DComp sobre a area de conteudo do Explorer.
// Chamado em BrowseObject (conteudo antigo ainda visivel) para garantir
// que o overlay esta commitado antes de qualquer nova janela aparecer.
// hwndRoot: CabinetWClass — sempre visivel e dimensionado.
// frame: conteudo capturado do ecra (inclui Mica/Acrylic).
// topmost=FALSE: overlay abaixo de layers existentes (pill do TW visivel).
// StartDCompFade toma posse de frame.hbm e garante DeleteObject em todos
// os caminhos (sucesso e falha). O caller nunca deve libertar hbm apos esta chamada.
static bool StartDCompFade(HWND hwndRoot, CapturedFrame frame, int fadeMs) {
    // Snapshot atomico do device -- usado durante toda a funcao.
    // Evita data race com o init thread que escreve g_dcDevice via store.
    IDCompositionDesktopDevice* dcDev = g_dcDevice.load(std::memory_order_acquire);
    if (!dcDev || !frame.hbm || frame.w <= 0 || frame.h <= 0) {
        DeleteObject(frame.hbm);
        return false;
    }


    // Serializar TODAS as chamadas DComp no device e nos seus objectos.
    // DirectComposition nao e internamente thread-safe; sem este lock, o
    // cleanup worker pode chamar SetRoot/Release/Commit concorrentemente
    // com a criacao de um novo fade, corrompendo o estado de composicao.
    std::lock_guard<std::mutex> lkDC(g_dcFadeMtx);

    // Cancelar fade anterior neste root se existir.
    {
        auto it = g_dcFades.find(hwndRoot);
        if (it != g_dcFades.end()) {
            DCompFadeAnim prev = it->second;
            g_dcFades.erase(it);
            if (prev.pTarget) { prev.pTarget->SetRoot(nullptr); prev.pTarget->Release(); }
            if (prev.pEffect)  prev.pEffect->Release();
            if (prev.pVisual)  prev.pVisual->Release();
            if (prev.pSurface) prev.pSurface->Release();
            dcDev->Commit();
        }
    }

    DCompFadeAnim anim;
    HRESULT hr;

    // topmost=FALSE: overlay abaixo de layers existentes (pill TW visivel).
    hr = dcDev->CreateTargetForHwnd(hwndRoot, FALSE, &anim.pTarget);
    if (FAILED(hr)) {
        DeleteObject(frame.hbm); return false;
    }

    hr = dcDev->CreateSurface((UINT)frame.w, (UINT)frame.h,
                                   DXGI_FORMAT_B8G8R8A8_UNORM,
                                   DXGI_ALPHA_MODE_IGNORE,
                                   &anim.pSurface);
    if (FAILED(hr)) {
        anim.pTarget->Release(); DeleteObject(frame.hbm); return false;
    }

    // Upload do bitmap via GDI DC e libertar imediatamente apos BitBlt.
    {
        POINT offset{};
        IDXGISurface1* pGdi = nullptr;
        if (SUCCEEDED(anim.pSurface->BeginDraw(nullptr,
                          __uuidof(IDXGISurface1), (void**)&pGdi, &offset)) && pGdi) {
            HDC hdcSurf = nullptr;
            if (SUCCEEDED(pGdi->GetDC(FALSE, &hdcSurf))) {
                HDC     hdcTmp = CreateCompatibleDC(hdcSurf);
                HGDIOBJ hOld   = SelectObject(hdcTmp, frame.hbm);
                BitBlt(hdcSurf, offset.x, offset.y, frame.w, frame.h,
                       hdcTmp, 0, 0, SRCCOPY);
                SelectObject(hdcTmp, hOld);
                DeleteDC(hdcTmp);
                pGdi->ReleaseDC(nullptr);
            }
            pGdi->Release();
            anim.pSurface->EndDraw();
        }
    }
    // Bitmap ja copiado para a superficie DComp — libertar agora.
    // A partir daqui, frame.hbm nao e mais necessario em nenhum caminho.
    DeleteObject(frame.hbm);

    hr = dcDev->CreateVisual(&anim.pVisual);
    if (FAILED(hr)) {
        anim.pSurface->Release(); anim.pTarget->Release(); return false;
    }
    anim.pVisual->SetContent(anim.pSurface);
    anim.pVisual->SetOffsetX((float)frame.ptInRoot.x);
    anim.pVisual->SetOffsetY((float)frame.ptInRoot.y);

    // Animacao de opacidade: linear 1->0 em fadeMs ms (WinUI spec).
    IDCompositionEffectGroup* pEffect = nullptr;
    if (SUCCEEDED(dcDev->CreateEffectGroup(&pEffect))) {
        IDCompositionAnimation* pAnimO = nullptr;
        if (SUCCEEDED(dcDev->CreateAnimation(&pAnimO))) {
            double durSec = fadeMs / 1000.0;
            pAnimO->AddCubic(0.0, 1.0f, (float)(-1.0 / durSec), 0.0f, 0.0f);
            pAnimO->End(durSec, 0.0f);
            pEffect->SetOpacity(pAnimO);
            pAnimO->Release();
        }
        anim.pVisual->SetEffect(pEffect);
        anim.pEffect = pEffect;
    }

    // Visual raiz para posicionar o content visual com offset.
    IDCompositionVisual2* pRoot = nullptr;
    if (SUCCEEDED(dcDev->CreateVisual(&pRoot))) {
        pRoot->AddVisual(anim.pVisual, FALSE, nullptr);
        anim.pTarget->SetRoot(pRoot);
        pRoot->Release();
    } else {
        anim.pTarget->SetRoot(anim.pVisual);
    }

    hr = dcDev->Commit();
    if (FAILED(hr)) {
        if (anim.pEffect)  anim.pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    // Atribuir geracao unica a esta instancia do fade.
    uint64_t gen = g_dcFadeGen.fetch_add(1, std::memory_order_relaxed);
    anim.generation = gen;
    g_dcFades[hwndRoot] = anim;

    // Enfileirar cleanup no worker de longa duracao.
    // AddRef no device para garantir validade ate o worker processar o item,
    // independente de quando Wh_ModUninit correr.
    dcDev->AddRef();
    {
        std::lock_guard<std::mutex> lk(g_cleanupMtx);
        g_cleanupQueue.push_back({hwndRoot, dcDev,
                                  GetTickCount64() + (ULONGLONG)(fadeMs + 50),
                                  gen});
        g_cleanupCv.notify_one();
    }

    return true;
}



// -----------------------------------------------
// Alpha fade helpers (CPU — usado quando DComp nao disponivel)
//
// Aplicado apenas ao conteudo do Explorer (IsExplorerContent), nao a nav
// pane nem as prop pages -- esses dois tem comportamentos de background
// GDI e clip de layout que interagem mal com WS_EX_LAYERED.
//
// WS_EX_LAYERED em janela filha (Windows 8+) nao afeta o backdrop DWM da
// raiz quando o backdrop e Mica/Acrylic nativo. No entanto, mods que
// estendem o backdrop via DwmExtendFrameIntoClientArea ou DwmSetWindowAttribute
// aplicam o efeito no nivel do swap chain da raiz -- o DWM compoe a filha
// layered sobre o background cru do parent (preto antes do efeito ser
// aplicado), resultando no flash preto que o aviso do setting descreve.
// -----------------------------------------------

static void BeginFade(HWND hwnd) {
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
    // alpha=1: forca o DWM a inicializar a superficie de composicao antes
    // do primeiro frame visivel, eliminando o flash de "nao composto -> composto".
    // Visualmente indistinguivel de 0.
    SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA);
}

static void SetAlpha(HWND hwnd, BYTE alpha) {
    SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
}

static void EndFade(HWND hwnd) {
    if (!IsWindow(hwnd)) return;
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
    // Forcar repaint para o DWM recompor sem o flag layered.
    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

// -----------------------------------------------
// RunSlideIn
// -----------------------------------------------

static void RunSlideIn(HWND hwnd, AnimAxis axis, bool isPropPage,
                       bool useFade, bool useDCompFade, int buttonRowTop,
                       TransitionType transition) {
    if (!IsWindow(hwnd)) {
        {
            std::lock_guard<std::mutex> lk(g_animMtx);
            g_animating.erase(hwnd);
        }
        g_animDoneCv.notify_all();
        return;
    }

    HWND parent = GetParent(hwnd);
    if (!parent) {
        // Janela existe mas sem parent — restaurar RGN para desfazer
        // o HideViaRgn aplicado em HookShowWindow/HookSetWindowPos.
        RestoreRgn(hwnd);
        if (useFade && !useDCompFade) EndFade(hwnd);
        {
            std::lock_guard<std::mutex> lk(g_animMtx);
            g_animating.erase(hwnd);
        }
        g_animDoneCv.notify_all();
        return;
    }

    // Snapshot de configuracoes para evitar data race com LoadSettings.
    // Todos os acessos a settings durante a animacao usam esta copia local.
    Settings snap;
    AcquireSRWLockShared(&g_sLock);
    snap = g_s;
    ReleaseSRWLockShared(&g_sLock);

    // Parametros efetivos variam conforme o tipo de transicao:
    // - Entrance:     distancia e duracao completas, EaseOutCubicWithRamp
    // - EnterFolder:  distancia 1x, 70% duracao, horizontal da direita (como SlideForward)
    // - SlideBack/Forward: 2x distancia, mesma duracao, EaseOutCubic
    int effectiveDistance = snap.distance;
    int effectiveDuration = snap.duration;

    if (transition == TransitionType::EnterFolder) {
        // Mesma distancia que Entrance (1x) mas duracao 70% -- mais leve
        // que o SlideForward de historico mas mesma direcao (para a direita).
        effectiveDuration = std::max(80, (int)(snap.duration * 0.70f));
    } else if (transition == TransitionType::SlideBack ||
               transition == TransitionType::SlideForward) {
        // Slide horizontal usa 2x a distancia no conteudo do Explorer.
        // Para prop pages (dialogo pequeno) usa 1x para manter proporcao visual.
        effectiveDistance = isPropPage ? snap.distance : snap.distance * 2;
    }

    // Detectar refresh rate do monitor onde a janela esta para usar o
    // intervalo de frame correto (ex: 6944us a 144Hz em vez de 16667us a 60Hz).
    // MonitorFromWindow garante o monitor certo em setups multi-monitor.
    // Fallback para 60Hz se a deteccao falhar.
    long long frameIntervalUs = 16667LL;
    {
        HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEXW mi = {};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(hMon, reinterpret_cast<MONITORINFO*>(&mi))) {
            DEVMODEW dm = {};
            dm.dmSize = sizeof(dm);
            if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
                dm.dmDisplayFrequency >= 24) {
                frameIntervalUs = 1000000LL / dm.dmDisplayFrequency;
            }
        }
    }

    RECT wrc{}; GetWindowRect(hwnd, &wrc);
    POINT pt{wrc.left, wrc.top};
    ScreenToClient(parent, &pt);
    int origX = pt.x;
    int origY = pt.y;
    int pageW = wrc.right  - wrc.left;
    int pageH = wrc.bottom - wrc.top;

    int startX = origX;
    int startY = origY;

    switch (transition) {
        case TransitionType::Entrance: {
            bool reverse = snap.reverseDirection && !isPropPage;
            startY = reverse ? origY - effectiveDistance : origY + effectiveDistance;
            break;
        }
        case TransitionType::EnterFolder:
            // Mesmo sentido que SlideForward (da direita): entrar numa subpasta
            // e navegar para frente na hierarquia. Distancia 1x (nao 2x) e
            // duracao 70% distinguem do botao Avancar de historico.
            startX = origX + effectiveDistance;
            break;
        case TransitionType::SlideBack:
            startX = origX - effectiveDistance;
            break;
        case TransitionType::SlideForward:
            startX = origX + effectiveDistance;
            break;
    }

    // Para nav pane (sempre horizontal, nunca e page transition):
    if (axis == AnimAxis::Horizontal &&
        transition == TransitionType::Entrance) {
        startX = origX - effectiveDistance;
        startY = origY;
    }

    SetWindowPos(hwnd, nullptr, startX, startY, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    RestoreRgn(hwnd);

    int  lastCx     = INT_MIN;
    int  lastCy     = INT_MIN;
    int  lastClipH  = -1;
    BYTE lastAlpha  = 1;
    auto t0         = std::chrono::steady_clock::now();
    int  frameN     = 0;
    // Duracao do fade CPU (proporcional ao spec WinUI: 33% da duracao total).
    int fadeDuration = CalcFadeDuration(effectiveDuration);

    int  totalDurMs = useFade
                      ? std::max(effectiveDuration, fadeDuration)
                      : effectiveDuration;

    for (;;) {
        // Verificar IsWindow e g_unloading a cada frame.
        if (!IsWindow(hwnd) || g_unloading.load(std::memory_order_relaxed))
            break;

        float elapsed = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - t0).count();
        float t = std::min(elapsed / (float)effectiveDuration, 1.f);

        // Easing: Entrance usa cubico com ramp de arranque; todos os
        // outros tipos (EnterFolder, SlideBack, SlideForward) usam cubico puro.
        float e = (transition == TransitionType::Entrance)
                  ? EaseOutCubicWithRamp(t)
                  : EaseOutCubic(t);

        // -- Posicao --
        int cx = startX + (int)std::round(e * (float)(origX - startX));
        int cy = startY + (int)std::round(e * (float)(origY - startY));

        if (cx != lastCx || cy != lastCy) {
            // SWP_NOCOPYBITS NAO e usado: janelas GDI (prop pages) dependem do
            // bitblt para copiar pixels ao novo offset sem expor o background,
            // evitando flickering. Para janelas DWM-composited o bitblt e
            // inofensivo -- o DWM compoe do seu surface cache de qualquer forma.
            SetWindowPos(hwnd, nullptr, cx, cy, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            lastCx = cx; lastCy = cy;

            // Clip de layout para prop pages: limita a regiao visivel ao
            // espaco acima do button row conforme a janela se move.
            if (buttonRowTop >= 0) {
                int visibleH = buttonRowTop - cy;
                int clipH    = (visibleH > 0 && visibleH < pageH)
                               ? visibleH : pageH;
                if (clipH != lastClipH) {
                    lastClipH = clipH;
                    if (clipH < pageH) {
                        HRGN clip = CreateRectRgn(0, 0, pageW, clipH);
                        if (clip && !SetWindowRgn(hwnd, clip, TRUE))
                            DeleteObject(clip);
                    } else {
                        SetWindowRgn(hwnd, nullptr, TRUE);
                    }
                }
            }
        }

        // -- Alpha fade (CPU) -- apenas quando DComp nao disponivel.
        if (useFade && !useDCompFade) {
            float tFade  = std::min(elapsed / (float)fadeDuration, 1.f);
            BYTE  alpha  = (BYTE)std::clamp(
                (int)std::round(EaseOutCubic(tFade) * 255.f), 1, 255);
            if (alpha != lastAlpha) {
                SetAlpha(hwnd, alpha);
                lastAlpha = alpha;
            }
        }

        if (elapsed >= (float)totalDurMs) break;

        ++frameN;
        auto nextFrame = t0 + std::chrono::microseconds(frameN * frameIntervalUs);
        std::this_thread::sleep_until(nextFrame);
    }

    // Finalizacao -- so operar se a janela ainda existe
    if (IsWindow(hwnd)) {
        SetWindowPos(hwnd, nullptr, origX, origY, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        RestoreRgn(hwnd);
        if (useFade && !useDCompFade) EndFade(hwnd);
    }

    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        g_animating.erase(hwnd);
    }
    g_animDoneCv.notify_all();
}

// -----------------------------------------------
// AnimInfo + Classify
// -----------------------------------------------

struct AnimInfo {
    AnimAxis       axis;
    bool           isPropPage;
    bool           useFade;      // fade CPU (WS_EX_LAYERED) — usado quando DComp indisponivel
    bool           useDCompFade; // fade GPU (DComp overlay) — preferencial
    int            buttonRowTop;
    TransitionType transition;
};

// Consulta g_pendingNav para o root desta janela e consome a entrada.
// Retorna Entrance se nao houver pendencia ou se pageTransitions esta
// desativado. Chamada sempre no thread da UI do Explorer.
static TransitionType ConsumePendingTransition(HWND hwnd) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root)
        return TransitionType::Entrance;

    std::lock_guard<std::mutex> lk(g_pendingMtx);
    auto it = g_pendingNav.find(root);
    if (it == g_pendingNav.end())
        return TransitionType::Entrance;

    TransitionType type = it->second.type;
    ULONGLONG      ts   = it->second.timestamp;
    ULONGLONG      age  = GetTickCount64() - ts;
    g_pendingNav.erase(it);

    if (age > PENDING_EXPIRE_MS)
        return TransitionType::Entrance;

    Wh_Log(L"[CONSUME] root=%p -> type=%d (age=%llums)", root, (int)type, age);
    return type;
}

static bool Classify(HWND hwnd, AnimInfo& info) {
    // Snapshot dos flags relevantes sob lock compartilhado para evitar
    // data race com LoadSettings (chamado do thread do Windhawk).
    bool alphaFade, animateNavPane, animatePropertyPages, pageTransitions, propPageTransitions;
    AcquireSRWLockShared(&g_sLock);
    alphaFade            = g_s.alphaFade;
    animateNavPane       = g_s.animateNavPane;
    animatePropertyPages = g_s.animatePropertyPages;
    pageTransitions      = g_s.pageTransitions;
    propPageTransitions  = g_s.propPageTransitions;
    ReleaseSRWLockShared(&g_sLock);

    // DComp fade preferencial quando disponivel — compativel com Mica/Acrylic.
    // CPU fade (WS_EX_LAYERED) como fallback enquanto DComp inicializa.
    bool useDCompFade = alphaFade && (g_dcDevice.load(std::memory_order_acquire) != nullptr);
    bool useCpuFade   = alphaFade && !useDCompFade;

    if (IsExplorerContent(hwnd)) {
        TransitionType transition = ConsumePendingTransition(hwnd);
        if (!pageTransitions)
            transition = TransitionType::Entrance;

        AnimAxis axis = AnimAxis::Vertical;
        if (transition == TransitionType::SlideBack   ||
            transition == TransitionType::SlideForward ||
            transition == TransitionType::EnterFolder)
            axis = AnimAxis::Horizontal;

        info = {axis, false, useCpuFade, useDCompFade, -1, transition};
        return true;
    }
    if (IsNavPaneContent(hwnd, animateNavPane)) {
        info = {AnimAxis::Horizontal, false, false, false, -1, TransitionType::Entrance};
        return true;
    }
    if (IsPropertyPage(hwnd, animatePropertyPages)) {
        int  buttonRowTop = -1;
        HWND parent       = GetParent(hwnd);
        HWND hTab         = nullptr;
        TransitionType transition = TransitionType::Entrance;

        if (propPageTransitions && parent) {
            // Fast path: PropertySheet ja subclassado -- ler hTab, buttonRowTop
            // e pendingOldIndex do cache sem enumeracao de filhos.
            // Cobre a grande maioria das trocas de aba apos a primeira abertura.
            std::unique_lock<std::mutex> lk(g_propSheetMtx);
            auto it = g_propSheets.find(parent);
            if (it != g_propSheets.end()) {
                hTab         = it->second.hTab;
                buttonRowTop = it->second.buttonRowTop;
                int oldIdx   = it->second.pendingOldIndex;
                it->second.pendingOldIndex = -1; // consumir: uma transicao por show
                lk.unlock();
                if (oldIdx >= 0 && hTab) {
                    int newIdx = TabCtrl_GetCurSel(hTab);
                    transition = (newIdx > oldIdx) ? TransitionType::SlideForward
                               : (newIdx < oldIdx) ? TransitionType::SlideBack
                               : TransitionType::Entrance;
                }
                // oldIdx == -1: primeira aba -> Entrance (default ja correto)
            } else {
                lk.unlock();
                // Primeira aba deste PropertySheet: enumerar filhos para obter
                // hTab e buttonRowTop, depois subclassar e cachear ambos.
                bool foundBtn = false;
                HWND sib = GetWindow(parent, GW_CHILD);
                while (sib) {
                    wchar_t sc[32] = {}; GetClassNameW(sib, sc, 32);
                    if (!foundBtn && !wcscmp(sc, L"Button")) {
                        RECT brc; GetWindowRect(sib, &brc);
                        POINT bp = {brc.left, brc.top};
                        ScreenToClient(parent, &bp);
                        buttonRowTop = bp.y;
                        foundBtn = true;
                    } else if (!hTab && !wcscmp(sc, L"SysTabControl32")) {
                        hTab = sib;
                    }
                    if (foundBtn && hTab) break;
                    sib = GetWindow(sib, GW_HWNDNEXT);
                }
                TrySubclassPropSheet(parent, hTab, buttonRowTop);
                // pendingOldIndex == -1 na insercao -> Entrance (default ja correto)
            }
        } else {
            // propPageTransitions desativado: procurar apenas Button para
            // buttonRowTop, sem tocar em SysTabControl32 nem em g_propSheets.
            if (parent) {
                HWND sib = GetWindow(parent, GW_CHILD);
                while (sib) {
                    wchar_t sc[32] = {}; GetClassNameW(sib, sc, 32);
                    if (!wcscmp(sc, L"Button")) {
                        RECT brc; GetWindowRect(sib, &brc);
                        POINT bp = {brc.left, brc.top};
                        ScreenToClient(parent, &bp);
                        buttonRowTop = bp.y;
                        break;
                    }
                    sib = GetWindow(sib, GW_HWNDNEXT);
                }
            }
        }

        AnimAxis axis = (transition == TransitionType::SlideBack ||
                         transition == TransitionType::SlideForward)
                        ? AnimAxis::Horizontal : AnimAxis::Vertical;
        info = {axis, true, false, false, buttonRowTop, transition};
        return true;
    }
    return false;
}

// -----------------------------------------------
// Hook: CShellBrowser::BrowseObject (explorerframe.dll)
//
// Detecta o tipo de navegacao ANTES da view ser recriada:
// - SBSP_NAVIGATEBACK / SBSP_PARENT -> SlideBack
// - SBSP_NAVIGATEFORWARD            -> SlideForward
// - SBSP_RELATIVE                   -> EnterFolder (subpasta relativa)
// - SBSP_ABSOLUTE + PIDL filho      -> EnterFolder (slide da direita, 70%)
// - SBSP_ABSOLUTE + PIDL ancestral  -> Entrance  (nav pane / breadcrumb)
// - Tudo mais                       -> Entrance
//
// O resultado e armazenado em g_pendingNav[rootHwnd] e consumido por
// Classify() quando o novo UIItemsView recebe ShowWindow/SetWindowPos.
//
// Este hook e opcional: se a resolucao de simbolos falhar, o mod
// funciona normalmente usando Entrance para todas as navegacoes.
// -----------------------------------------------

// -----------------------------------------------
// Hook: CreateWindowExW -- detecta novas janelas CabinetWClass
// -----------------------------------------------

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD,
    int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t g_origCWEW = nullptr;

HWND WINAPI HookCreateWindowExW(DWORD exStyle, LPCWSTR className,
    LPCWSTR windowName, DWORD style, int x, int y, int cx, int cy,
    HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = g_origCWEW(exStyle, className, windowName, style, x, y, cx, cy,
                            parent, menu, inst, param);
    // className pode ser um ATOM (inteiro de 16 bits passado como ponteiro,
    // ex: (LPCWSTR)0x8001). IS_INTRESOURCE detecta isso antes de qualquer
    // acesso via ponteiro -- sem esse check, wcscmp trava o processo.
    if (hwnd && className &&
        !IS_INTRESOURCE(className) &&
        !wcscmp(className, L"CabinetWClass"))
        TrySubclassExplorerWindow(hwnd);
    return hwnd;
}

// Pesquisa recursiva pelo SHELLDLL_DefView / UIItemsView / ItemsView
// visivel e correctamente dimensionado (w>1, h>1) na arvore de hwndParent.
// Funcao estatica: evita overhead e potenciais problemas do std::function.
static HWND FindVisibleContent(HWND hwndParent) {
    WCHAR c[64] = {};
    RealGetWindowClassW(hwndParent, c, 64);
    if (!wcscmp(c, L"UIItemsView") ||
        !wcscmp(c, L"ItemsView")   ||
        !wcscmp(c, L"SHELLDLL_DefView")) {
        if (IsWindowVisible(hwndParent)) {
            RECT r{}; GetClientRect(hwndParent, &r);
            if (r.right > 1 && r.bottom > 1) return hwndParent;
        }
    }
    HWND ch = GetWindow(hwndParent, GW_CHILD);
    while (ch) {
        if (HWND f = FindVisibleContent(ch)) return f;
        ch = GetWindow(ch, GW_HWNDNEXT);
    }
    return nullptr;
}

HRESULT WINAPI HookBrowseObject(void* pThis, LPCITEMIDLIST pidl, UINT wFlags) {
    if (!pThis || g_unloading.load(std::memory_order_relaxed)) {
        // g_origBrowseObject deve estar preenchido se o hook foi instalado,
        // mas checagem defensiva evita crash se chamado em estado inconsistente.
        if (g_origBrowseObject)
            return g_origBrowseObject(pThis, pidl, wFlags);
        return E_FAIL;
    }

    TransitionType type = TransitionType::Entrance;

    if (wFlags & SBSP_NAVIGATEBACK) {
        type = TransitionType::SlideBack;
        Wh_Log(L"[NAV] -> SlideBack (NAVIGATEBACK)");
    } else if (wFlags & SBSP_NAVIGATEFORWARD) {
        type = TransitionType::SlideForward;
        Wh_Log(L"[NAV] -> SlideForward (NAVIGATEFORWARD)");
    } else if (wFlags & SBSP_PARENT) {
        type = TransitionType::SlideBack;
        Wh_Log(L"[NAV] -> SlideBack (PARENT)");
    } else if (wFlags & SBSP_RELATIVE) {
        type = TransitionType::EnterFolder;
        Wh_Log(L"[NAV] -> EnterFolder (RELATIVE)");
    } else if (pidl) {
        IUnknown* pUnk = reinterpret_cast<IUnknown*>(pThis);
        IShellBrowser* pSB = nullptr;
        HRESULT hrQI = pUnk->QueryInterface(IID_PPV_ARGS(&pSB));
        if (SUCCEEDED(hrQI)) {
            IShellView* pSV = nullptr;
            HRESULT hrSV = pSB->QueryActiveShellView(&pSV);
            if (SUCCEEDED(hrSV)) {
                IFolderView2* pFV = nullptr;
                if (SUCCEEDED(pSV->QueryInterface(IID_PPV_ARGS(&pFV)))) {
                    IPersistFolder2* pPF = nullptr;
                    if (SUCCEEDED(pFV->GetFolder(IID_PPV_ARGS(&pPF)))) {
                        LPITEMIDLIST pidlCur = nullptr;
                        if (SUCCEEDED(pPF->GetCurFolder(&pidlCur))) {
                            // ILIsParent(pidlCur, pidl): pidl e filho direto/indireto do atual → EnterFolder
                            // ILIsParent(pidl, pidlCur): pidl e pai do atual → Entrance (pode vir do
                            // nav pane ou breadcrumb; SlideBack fica reservado ao botao Voltar).
                            if (ILIsParent(pidlCur, pidl, FALSE))
                                type = TransitionType::EnterFolder;
                            CoTaskMemFree(pidlCur);
                        }
                        pPF->Release();}
                    pFV->Release();}
                pSV->Release();
            }
            pSB->Release();
        }
        Wh_Log(L"[NAV] -> %s (PIDL)",
               type == TransitionType::EnterFolder     ? L"EnterFolder" :
               type == TransitionType::SlideBack    ? L"SlideBack"   :
               type == TransitionType::SlideForward ? L"SlideForward": L"Entrance");
    } else {
        Wh_Log(L"[NAV] -> Entrance (pidl=NULL sem flags de direcao)");
    }

    IUnknown* pUnkOW = reinterpret_cast<IUnknown*>(pThis);
    IOleWindow* pOW = nullptr;
    if (SUCCEEDED(pUnkOW->QueryInterface(IID_PPV_ARGS(&pOW)))) {
        HWND hwndSB = nullptr;
        pOW->GetWindow(&hwndSB);
        pOW->Release();
        if (hwndSB) {
            HWND root = GetAncestor(hwndSB, GA_ROOT);
            if (root) {
                std::lock_guard<std::mutex> lk(g_pendingMtx);
                auto it = g_pendingNav.find(root);
                // NAVIGATEBACK/FORWARD dispara dois BrowseObject seguidos:
                // o 1o com flag de direcao (SlideBack/Forward), o 2o com PIDL
                // absoluto. O 2o pode produzir Entrance ou EnterFolder (ILIsParent
                // compara current com target, mas current ja navegou no 1o call).
                // Regra: SlideBack/Forward so podem ser sobrescritos por outro
                // SlideBack/Forward -- Entrance e EnterFolder nunca os sobrescrevem.
                bool isExistingDirectional =
                    it != g_pendingNav.end() &&
                    (it->second.type == TransitionType::SlideBack ||
                     it->second.type == TransitionType::SlideForward);
                bool isNewNonDirectional =
                    type == TransitionType::Entrance ||
                    type == TransitionType::EnterFolder;
                bool overwrite = true;
                if (isExistingDirectional && isNewNonDirectional &&
                    GetTickCount64() - it->second.timestamp < 200) {
                    overwrite = false;
                    it->second.timestamp = GetTickCount64();
                    Wh_Log(L"[NAV] tipo=%d ignorado (protege direcional %d)",
                           (int)type, (int)it->second.type);
                }
                // Gravar apenas tipos nao-Entrance. Entrance e o valor
                // default de ConsumePendingTransition quando o mapa esta
                // vazio -- armazena-lo explicitamente cria entradas obsoletas
                // que podem ser consumidas incorretamente por eventos
                // subsequentes de UIItemsView (ex: 2a chamada PIDL apos
                // SlideBack ja ter sido consumido).
                if (overwrite && type != TransitionType::Entrance)
                    g_pendingNav[root] = {type, GetTickCount64()};

                // Capturar o ecra antes da navegacao: o conteudo antigo
                // esta visivel e correctamente dimensionado neste momento.
                // Snapshot de alphaFade e duration sob lock para consistencia
                // com LoadSettings (chamado do thread do Windhawk).
                bool snapFade;
                int  snapDuration;
                AcquireSRWLockShared(&g_sLock);
                snapFade     = g_s.alphaFade;
                snapDuration = g_s.duration;
                ReleaseSRWLockShared(&g_sLock);

                if (g_dcDevice.load(std::memory_order_acquire) && snapFade) {
                    HWND hwndContent = FindVisibleContent(hwndSB);

                    if (hwndContent) {
                        RECT rc{};
                        GetClientRect(hwndContent, &rc);
                        int w = rc.right, h = rc.bottom;
                        if (w > 1 && h > 1) {
                            POINT ptInRoot{0, 0};
                            MapWindowPoints(hwndContent, root, &ptInRoot, 1);

                            // GetDC(nullptr) + BitBlt: copia directamente os pixels
                            // do frame ja composto pelo DWM — a tecnica mais leve.
                            // Captura Mica/Acrylic correctamente.
                            POINT ptScreen{0, 0};
                            ClientToScreen(hwndContent, &ptScreen);
                            HDC hdcScreen = GetDC(nullptr);
                            HBITMAP hbm = nullptr;
                            if (hdcScreen) {
                                HDC hdcMem = CreateCompatibleDC(hdcScreen);
                                hbm = CreateCompatibleBitmap(hdcScreen, w, h);
                                if (hdcMem && hbm) {
                                    HGDIOBJ hOld = SelectObject(hdcMem, hbm);
                                    BitBlt(hdcMem, 0, 0, w, h,
                                           hdcScreen, ptScreen.x, ptScreen.y, SRCCOPY);
                                    SelectObject(hdcMem, hOld);
                                    DeleteDC(hdcMem);
                                } else {
                                    if (hbm) { DeleteObject(hbm); hbm = nullptr; }
                                    if (hdcMem) DeleteDC(hdcMem);
                                }
                                ReleaseDC(nullptr, hdcScreen);
                            }
                            if (hbm) {
                                CapturedFrame cf{hbm, ptInRoot, w, h};
                                int effDur = (type == TransitionType::EnterFolder)
                                             ? std::max(80, (int)(snapDuration * 0.70f))
                                             : snapDuration;
                                // StartDCompFade toma posse de hbm em todos os caminhos.
                                StartDCompFade(root, cf, CalcFadeDuration(effDur));
                            }
                        }
                    }
                }
            }   // if (root)
        }       // if (hwndSB)
    }           // if (SUCCEEDED(QI IOleWindow))

    return g_origBrowseObject(pThis, pidl, wFlags);
}

// -----------------------------------------------
// Despacho de animacao
// -----------------------------------------------

static void DispatchAnimation(HWND hwnd, AnimInfo info) {
    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        // Nao iniciar novas animacoes durante o descarregamento.
        // Restaurar o estado da janela (HideViaRgn/BeginFade aplicados
        // no hook antes de chamar esta funcao) para evitar que a janela
        // fique invisivel ou semi-transparente permanentemente.
        if (g_unloading.load(std::memory_order_relaxed)) {
            RestoreRgn(hwnd);
            if (info.useFade && !info.useDCompFade) EndFade(hwnd);
            return;
        }
        if (g_animating.count(hwnd)) return;
        g_animating.insert(hwnd);
    }
    HWND      root    = GetAncestor(hwnd, GA_ROOT);
    ULONGLONG startAt = info.isPropPage
                        ? GetTickCount64()
                        : GetBatchStartAt(root);
    // O painel de navegacao recebe um offset de um frame para garantir que
    // o conteudo da pasta sempre inicie primeiro. Sem isso, jitter de
    // scheduling entre duas threads com o mesmo startAt pode fazer o nav
    // pane animar antes do conteudo. 16ms (um frame a 60Hz) e suficiente
    // para absorver qualquer variacao de scheduling do kernel e e
    // imperceptivel mesmo com firstOpenDelay longo.
    if (info.axis == AnimAxis::Horizontal &&
        info.transition == TransitionType::Entrance)
        startAt += 16;
    std::thread([hwnd, startAt, info]() {
        SleepUntil(startAt);
        if (!IsWindow(hwnd) || g_unloading.load(std::memory_order_relaxed)) {
            if (IsWindow(hwnd)) {
                RestoreRgn(hwnd);
                if (info.useFade && !info.useDCompFade) EndFade(hwnd);
            }
            {
                std::lock_guard<std::mutex> lk(g_animMtx);
                g_animating.erase(hwnd);
            }
            g_animDoneCv.notify_all();
            return;
        }
        RunSlideIn(hwnd, info.axis, info.isPropPage, info.useFade,
                   info.useDCompFade, info.buttonRowTop, info.transition);
    }).detach();
}

// -- Hook 1: ShowWindow ----------------------------
// Intercept ShowWindow para janelas de conteudo que serao mostradas pela
// primeira vez (IsWindowVisible == false). So os comandos que revelam uma
// janela oculta sao tratados; variantes de minimize/hide sao ignoradas.
using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t      g_origSW = nullptr;
static thread_local bool g_inSW   = false;

BOOL WINAPI HookShowWindow(HWND hwnd, int cmd) {
    if (g_inSW) return g_origSW(hwnd, cmd);
    if (IsWindowVisible(hwnd)) return g_origSW(hwnd, cmd);
    // Mod a descarregar: nao aplicar HideViaRgn/BeginFade porque
    // DispatchAnimation vai retornar sem iniciar RunSlideIn,
    // deixando a janela invisivel ou semi-transparente.
    if (g_unloading.load(std::memory_order_acquire)) return g_origSW(hwnd, cmd);

    if (cmd != SW_SHOW && cmd != SW_SHOWNA &&
        cmd != SW_SHOWNORMAL && cmd != SW_SHOWNOACTIVATE)
        return g_origSW(hwnd, cmd);

    AnimInfo info;
    if (!Classify(hwnd, info)) return g_origSW(hwnd, cmd);

    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        if (g_animating.count(hwnd)) return g_origSW(hwnd, cmd);
    }

    // Para DComp fade: commitar o overlay ANTES de ShowWindow para que esteja
    // em posicao quando a janela se torna visivel. HideViaRgn ainda necessario
    // para o slide (oculta a janela durante o posicionamento em RunSlideIn).
    // Para CPU fade: BeginFade antes de ShowWindow como antes.
    // Para sem fade: HideViaRgn apenas.
    if (info.useDCompFade) {
        // Overlay DComp ja commitado em BrowseObject. Se nao estiver activo
        // (ex: primeira abertura sem BrowseObject previo), tenta agora
        // mas sem frame capturado — sera no-op silencioso.
        HideViaRgn(hwnd);
    } else if (info.useFade) {
        BeginFade(hwnd);
    } else {
        HideViaRgn(hwnd);
    }

    g_inSW = true;
    BOOL r = g_origSW(hwnd, cmd);
    g_inSW = false;
    DispatchAnimation(hwnd, info);
    return r;
}

// -- Hook 2: SetWindowPos --------------------------
using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t    g_origSWP = nullptr;
static thread_local bool g_inSWP   = false;

BOOL WINAPI HookSetWindowPos(HWND hwnd, HWND hwndAfter,
                               int x, int y, int cx, int cy, UINT flags) {
    if (!g_inSWP && (flags & SWP_SHOWWINDOW) && !IsWindowVisible(hwnd)
        && !g_unloading.load(std::memory_order_acquire)) {
        AnimInfo info;
        if (Classify(hwnd, info)) {
            {
                std::lock_guard<std::mutex> lk(g_animMtx);
                if (g_animating.count(hwnd))
                    return g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);
            }
            if (info.useDCompFade) {
                HideViaRgn(hwnd); // overlay ja commitado em BrowseObject
            } else if (info.useFade) {
                BeginFade(hwnd);
            } else {
                HideViaRgn(hwnd);
            }
            g_inSWP = true;
            BOOL r = g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);
            g_inSWP = false;
            DispatchAnimation(hwnd, info);
            return r;
        }
    }
    return g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);
}

// -----------------------------------------------
// Windhawk entrypoints
// -----------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    // Registar mensagens customizadas no range 0xC000-0xFFFF para evitar
    // colisao com WM_USER+N que CabinetWClass possa usar internamente.
    WM_WH_REMOVE_SUBCLASS = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_RemoveSubclass");
    WM_WH_TRY_VTABLE_HOOK = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_TryVtableHook");

    // Registar hooks ANTES de iniciar threads.
    // Se qualquer Wh_SetFunctionHook falhar, retornamos FALSE e o Windhawk
    // nao chama Wh_ModUninit. Se as threads ja estivessem a correr,
    // ficariam orfas (sem join nem g_unloading).
    if (!WindhawkUtils::SetFunctionHook(
            ShowWindow, HookShowWindow, &g_origSW))
        return FALSE;

    if (!WindhawkUtils::SetFunctionHook(
            SetWindowPos, HookSetWindowPos, &g_origSWP))
        return FALSE;

    if (!WindhawkUtils::SetFunctionHook(
            CreateWindowExW, HookCreateWindowExW, &g_origCWEW))
        return FALSE;

    // Hooks registados com sucesso — agora iniciar threads de suporte.

    // Worker de cleanup DComp: dorme e processa a fila de cleanup.
    // Heap-allocated (como g_initThread) para evitar std::terminate() em
    // DLL_PROCESS_DETACH quando Explorer termina sem mod unload.
    g_cleanupThread = new std::thread(CleanupWorkerProc);

    // Iniciar D3D11/DComp em background para maximizar a probabilidade de
    // estar pronto antes da primeira navegacao do utilizador.
    // Heap-allocated para evitar std::terminate() em DLL_PROCESS_DETACH
    // (Explorer shutdown/restart -- ver std::thread destructor rules).
    // Joined em Wh_ModBeforeUninit para evitar UAF durante mod unload.
    g_initThread = new std::thread([]() {
        D3D_FEATURE_LEVEL level;

        // Tentar reutilizar um adapter DXGI ja carregado pelo Explorer para
        // evitar o custo de inicializacao do driver (~100-300ms na primeira chamada).
        // Se nao houver adapter disponivel, criar um device minimo.
        IDXGIFactory1* pFactory = nullptr;
        if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory1),
                                         reinterpret_cast<void**>(&pFactory)))) {
            IDXGIAdapter* pAdapter = nullptr;
            pFactory->EnumAdapters(0, &pAdapter); // primeiro adapter = GPU principal
            pFactory->Release();

            if (pAdapter) {
                if (g_unloading.load(std::memory_order_relaxed)) {
                    pAdapter->Release();
                    return;
                }
                D3D11CreateDevice(pAdapter,
                                  D3D_DRIVER_TYPE_UNKNOWN, // UNKNOWN quando adapter explicito
                                  nullptr,
                                  D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                                  nullptr, 0, D3D11_SDK_VERSION,
                                  &g_d3dDevice, &level, nullptr);
                pAdapter->Release();
            }
        }

        if (g_unloading.load(std::memory_order_relaxed)) return;

        // Fallback: device WARP (software) — sempre disponivel, init instantanea.
        if (!g_d3dDevice) {
            D3D11CreateDevice(nullptr,
                              D3D_DRIVER_TYPE_WARP,
                              nullptr,
                              D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                              nullptr, 0, D3D11_SDK_VERSION,
                              &g_d3dDevice, &level, nullptr);
        }

        if (g_unloading.load(std::memory_order_relaxed)) return;

        if (g_d3dDevice) {
            g_d3dDevice->QueryInterface(__uuidof(IDXGIDevice),
                                        reinterpret_cast<void**>(&g_dxgiDevice));
        }
        if (g_dxgiDevice) {
            using DCompCreate2_t = HRESULT(WINAPI*)(IUnknown*, REFIID, void**);
            HMODULE hDComp = LoadLibraryExW(L"dcomp.dll", nullptr,
                                            LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (hDComp) {
                auto pfn = (DCompCreate2_t)GetProcAddress(hDComp,
                                                          "DCompositionCreateDevice2");
                if (pfn) {
                    IDCompositionDesktopDevice* pDC = nullptr;
                    pfn(g_dxgiDevice,
                        __uuidof(IDCompositionDesktopDevice),
                        reinterpret_cast<void**>(&pDC));
                    // Store com release: garante que g_d3dDevice e g_dxgiDevice
                    // estao visiveis para qualquer thread que leia g_dcDevice != null.
                    g_dcDevice.store(pDC, std::memory_order_release);
                }
            }
        }
    });

    // Hook de CShellBrowser::BrowseObject em explorerframe.dll.
    // Estrategia em 2 camadas:
    // 1. WindhawkUtils::HookSymbols  -- cache de PDB, mostra UI de download.
    //    Tenta varios nomes possiveis; o nome exato varia entre builds.
    // 2. TryHookBrowseObjectViaVtable -- sem PDB, sempre funciona.
    //
    // Ambas correm aqui em Wh_ModInit. Se explorerframe.dll nao estiver
    // carregada ou nenhuma janela CabinetWClass existir (startup do Explorer),
    // ambas falham silenciosamente e o hook e diferido para
    // WM_WH_TRY_VTABLE_HOOK (postado por TrySubclassExplorerWindow quando
    // a primeira CabinetWClass aparece via CreateWindowExW hook).
    // Hooks registados em Wh_ModInit sao aplicados implicitamente pelo
    // Windhawk antes de Wh_ModAfterInit (ver mod lifetime flowchart).
    {
        HMODULE hEF = GetModuleHandleW(L"explorerframe.dll");
        if (hEF) {
            WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
                {
                    {
                        LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const __unaligned *,unsigned int))",
                        LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const * __ptr64,unsigned int))",
                        LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST const *,unsigned int))",
                    },
                    &g_origBrowseObject,
                    HookBrowseObject,
                    true,
                },
            };
            WindhawkUtils::HookSymbols(hEF, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks));
            if (g_origBrowseObject)
                Wh_Log(L"[INIT] BrowseObject hooked OK via HookSymbols");
        }

        if (!g_origBrowseObject) {
            TryHookBrowseObjectViaVtable();
            if (g_origBrowseObject)
                Wh_Log(L"[INIT] BrowseObject hooked OK via vtable");
            else
                Wh_Log(L"[INIT] BrowseObject diferido para WM_WH_TRY_VTABLE_HOOK");
        }
    }

    return TRUE;
}

// -----------------------------------------------
// Vtable hook para BrowseObject
//
// Usado quando o PDB de explorerframe.dll nao esta disponivel (builds
// Insider/Beta). Obtem um IShellBrowser real via IShellWindows e le
// vtable[11] = BrowseObject. Como todos os CShellBrowser compartilham
// a mesma vtable, hookear um endereco cobre todas as instancias.
//
// Chamado de:
// - Wh_ModInit: hooks aplicados implicitamente pelo Windhawk.
// - WM_WH_TRY_VTABLE_HOOK: Wh_ApplyHookOperations chamado explicitamente.
// -----------------------------------------------

static void TryHookBrowseObjectViaVtable() {
    if (g_origBrowseObject) return;

    // CLSID_ShellWindows = {9BA05972-F6A8-11CF-A442-00A0C90A8F39}
    // Definido localmente para evitar dependencia de uuid.lib.
    static const CLSID CLSID_SW =
        {0x9BA05972, 0xF6A8, 0x11CF, {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};

    // SID_STopLevelBrowser = {4C96BE40-915C-11CF-99D3-00AA004AE837}
    // Tambem definido localmente pelo mesmo motivo.
    static const GUID SID_STL =
        {0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

    // Chamado de Wh_ModInit (main thread) ou WM_WH_TRY_VTABLE_HOOK
    // (UI thread do Explorer). Inicializar COM aqui para poder chamar
    // CoCreateInstance com seguranca em ambos os contextos.
    HRESULT hrCO = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCO) && hrCO != RPC_E_CHANGED_MODE) {
        Wh_Log(L"[VTABLE] CoInitializeEx falhou: 0x%08X", (unsigned)hrCO);
        return;
    }

    IShellWindows* psw = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_SW, NULL, CLSCTX_ALL, IID_PPV_ARGS(&psw));
    Wh_Log(L"[VTABLE] IShellWindows hr=0x%08X", (unsigned)hr);
    if (FAILED(hr)) {
        // S_OK: COM inicializado pela primeira vez neste thread -> CoUninitialize obrigatorio.
    // S_FALSE: COM ja estava inicializado (mesmo apartment) -> CoInitialize incrementou
    //   o refcount de qualquer forma, CoUninitialize e necessario para decrementar.
    // RPC_E_CHANGED_MODE: nao inicializamos COM -> nao chamar CoUninitialize.
    if (hrCO == S_OK || hrCO == S_FALSE) CoUninitialize();
        return;
    }

    long count = 0;
    psw->get_Count(&count);
    Wh_Log(L"[VTABLE] janelas abertas: %ld", count);

    for (long i = 0; i < count && !g_origBrowseObject; i++) {
        VARIANT v = {}; v.vt = VT_I4; v.lVal = i;
        IDispatch* pD = nullptr;
        if (FAILED(psw->Item(v, &pD)) || !pD) continue;

        IServiceProvider* pSP = nullptr;
        if (SUCCEEDED(pD->QueryInterface(IID_PPV_ARGS(&pSP)))) {
            IShellBrowser* pSB = nullptr;
            // QueryService com SID_STopLevelBrowser (definido localmente como SID_STL)
            if (SUCCEEDED(pSP->QueryService(
                    SID_STL, IID_PPV_ARGS(&pSB)))) {
                // vtable[11] = BrowseObject (IUnknown:3 + IOleWindow:2 + 6 metodos antes)
                void** vtbl = *reinterpret_cast<void***>(pSB);
                void* fn    = vtbl[11];
                Wh_Log(L"[VTABLE] BrowseObject @ %p", fn);

                if (fn && !g_origBrowseObject) {
                    if (Wh_SetFunctionHook(fn,
                            reinterpret_cast<void*>(HookBrowseObject),
                            reinterpret_cast<void**>(&g_origBrowseObject))) {
                        Wh_Log(L"[VTABLE] hook instalado, aplicando...");
                    } else {
                        Wh_Log(L"[VTABLE] Wh_SetFunctionHook falhou");
                    }
                }
                pSB->Release();
            }
            pSP->Release();
        }
        pD->Release();
    }
    psw->Release();

    if (hrCO == S_OK || hrCO == S_FALSE) CoUninitialize();
}

void Wh_ModAfterInit() {
    // Subclassear janelas CabinetWClass ja abertas quando o mod e instalado
    // com o Explorer em execucao. Novas janelas sao capturadas pelo hook de
    // CreateWindowExW.
    EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
        // Filtrar por processo: apenas janelas do nosso explorer.exe.
        // Sem filtro, CabinetWClass de outras instancias de Explorer
        // receberia tentativa de subclass cross-process.
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId()) return TRUE;

        wchar_t cls[64];
        if (GetClassNameW(hwnd, cls, 64) && !wcscmp(cls, L"CabinetWClass"))
            TrySubclassExplorerWindow(hwnd);
        return TRUE;
    }, 0);
}

void Wh_ModBeforeUninit() {
    // Sinaliza para todas as threads em voo que devem encerrar.
    g_unloading.store(true, std::memory_order_release);
    // Acorda imediatamente threads dormindo em SleepUntil.
    g_sleepCv.notify_all();

    // Aguardar o init thread D3D/DComp terminar antes de prosseguir.
    // Se o mod for descarregado durante a inicializacao (cold driver ~100-300ms),
    // sem este join o thread continuaria a escrever g_d3dDevice/g_dxgiDevice
    // enquanto Wh_ModUninit os liberta -> UAF.
    if (g_initThread && g_initThread->joinable())
        g_initThread->join();
    delete g_initThread;
    g_initThread = nullptr;

    // Remover subclasses antes do DLL descarregar para evitar crash com
    // proc invalido. Envia WM_WH_REMOVE_SUBCLASS para o thread da janela
    // (unico thread que pode chamar RemoveWindowSubclass com seguranca).
    // SendMessageTimeoutW garante que nao bloqueamos indefinidamente se
    // o Explorer estiver travado.
    {
        std::vector<HWND> toRemove;
        {
            std::lock_guard<std::mutex> lk(g_subclassMtx);
            toRemove.assign(g_subclassedWindows.begin(), g_subclassedWindows.end());
        }
        for (HWND hw : toRemove) {
            if (IsWindow(hw))
                SendMessageTimeoutW(hw, WM_WH_REMOVE_SUBCLASS, 0, 0,
                                    SMTO_BLOCK | SMTO_ABORTIFHUNG, 2000, nullptr);
        }
    }

    // Remover subclasses de PropertySheets abertos.
    {
        std::vector<HWND> toRemoveProp;
        {
            std::lock_guard<std::mutex> lk(g_propSheetMtx);
            for (auto& [hw, s] : g_propSheets)
                toRemoveProp.push_back(hw);
        }
        for (HWND hw : toRemoveProp) {
            if (IsWindow(hw))
                SendMessageTimeoutW(hw, WM_WH_REMOVE_SUBCLASS, 0, 0,
                                    SMTO_BLOCK | SMTO_ABORTIFHUNG, 2000, nullptr);
        }
    }

    // Aguardar threads de animacao em voo terminarem.
    // Condition variable notificada sempre que g_animating encolhe (RunSlideIn
    // e DispatchAnimation early exit). Timeout de 2s evita deadlock se alguma
    // thread travar (ex: descheduled pelo kernel sob carga extrema).
    {
        std::unique_lock<std::mutex> lk(g_animMtx);
        g_animDoneCv.wait_for(lk, std::chrono::milliseconds(2000),
            [] { return g_animating.empty(); });
    }

    // Acordar e aguardar o worker de cleanup DComp.
    // Join aqui garante que todos os refs de device AddRef'd sao libertados
    // antes de Wh_ModUninit libertar g_dcDevice.
    g_cleanupCv.notify_one();
    if (g_cleanupThread && g_cleanupThread->joinable())
        g_cleanupThread->join();
    delete g_cleanupThread;
    g_cleanupThread = nullptr;
}

void Wh_ModUninit() {
    // Limpar estado global. Neste ponto todas as threads ja terminaram
    // (garantido pelo Wh_ModBeforeUninit acima).

    // Limpar animacoes DComp em voo.
    {
        std::lock_guard<std::mutex> lkd(g_dcFadeMtx);
        for (auto& [hwnd, a] : g_dcFades) {
            if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
            if (a.pEffect)  a.pEffect->Release();
            if (a.pVisual)  a.pVisual->Release();
            if (a.pSurface) a.pSurface->Release();
        }
        g_dcFades.clear();
    }
    IDCompositionDesktopDevice* dcDev = g_dcDevice.exchange(nullptr, std::memory_order_relaxed);
    if (dcDev)  { dcDev->Commit();  dcDev->Release(); }
    if (g_dxgiDevice){ g_dxgiDevice->Release(); g_dxgiDevice = nullptr; }
    if (g_d3dDevice) { g_d3dDevice->Release();  g_d3dDevice  = nullptr; }

    std::lock_guard<std::mutex> lk(g_animMtx);
    g_animating.clear();

    {
        std::lock_guard<std::mutex> lk2(g_rootMtx);
        g_rootSeen.clear();
        g_rootBatch.clear();
    }
    {
        std::lock_guard<std::mutex> lk3(g_pendingMtx);
        g_pendingNav.clear();
    }
    {
        std::lock_guard<std::mutex> lk4(g_subclassMtx);
        g_subclassedWindows.clear();
    }
    {
        std::lock_guard<std::mutex> lk5(g_propSheetMtx);
        g_propSheets.clear();
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
