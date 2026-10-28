// ==WindhawkMod==
// @id              file-explorer-content-animation
// @name            File Explorer WinUI-Like Content Animation
// @name:pt         Animação de Conteúdo do Explorador de Arquivos estilo WinUI
// @name:es         Animación de Contenido del Explorador de Archivos estilo WinUI
// @description     Smooth WinUI-like page transition animations for the File Explorer content area.
// @description:pt  Animações suaves de transição de página inspiradas no WinUI para a área de conteúdo do Explorador de Arquivos.
// @description:es  Animaciones suaves de transición de página inspiradas en WinUI para el área de contenido del Explorador de Archivos.
// @version         1.0.2
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lole32 -lshell32 -lcomctl32 -ld3d11 -ldxgi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

![Preview](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/Anima%C3%A7%C3%A3o%20WinUI%20do%20Explorador%20de%20Arquivos.gif)

---

## English

A smooth, WinUI-inspired page transition system for the File Explorer content area on Windows 11.

### What's new in 1.0.2

A big part of the mod was rewritten for this version. The animations now use DirectUI's own standard layer mechanism, so the motion and the item effects run on the system compositor instead of Explorer's UI thread: they stay smooth even while thumbnails and large folders are still loading. On builds where this isn't available, the mod keeps Explorer's native layout and the fade.

- **Smoother page transitions** - The folder content itself moves, without moving Explorer's windows. The defaults now follow WinUI: 450 ms (a 150 ms hold plus 300 ms of motion) and a 140 DIP entrance. Opening a folder uses the same entrance animation.
- **Staggered item reveal (optional)** - Items grow and fade in one after another, with the same curves as WinUI's AddDelete transition. On supported builds, deleted items also leave with a short animation.
- **Waits for your files** - In slow folders the page waits for the first files to show up before moving, instead of animating an empty view.
- **No more flashes** - The end of each animation is covered, so the content no longer flashes empty or shows duplicated text.
- **Responds to you** - Clicking or pressing a key during an animation finishes it quickly, and scrolling hands the content back right away, also with smooth scrolling mods.
- Requires symbol resolution on explorerframe.dll, plus duser.dll, dui70.dll and windows.storage.dll for staggering and deletions (handled automatically by Windhawk).

### Features

- Smooth slide animation with refined easing
- Context-aware page transitions (back, forward, entrance)
- Optional fade: GPU-accelerated DirectComposition overlay, compatible with Mica and Acrylic
- Optional staggered item reveal with the WinUI AddDelete curves
- Per-window first open delay (optional)
- Navigation delay to hide the "Working on it..." message
- Zero flicker / no black flashes
- Compatible with the Translucent Windows mod

### Settings

- **Animation Duration** - Controls how long the slide lasts.
- **Slide Distance** - Controls how far the content moves before settling.
- **Fade** - GPU-accelerated fade overlay showing the previous folder content. Compatible with Mica/Acrylic backdrop mods.
- **Staggered item reveal** - Uses native composition for WinUI AddDelete scale and opacity keyframes, with automatic DirectUI fallback when unavailable. Supported Shell builds also animate actual item removals with a short composition exit. Other items remain visually still until Explorer's final layout is revealed, without a reflow animation. Does not change native layout or XAML views. Disabled by default.
- **Reverse Direction** - Slide down instead of up. Does not apply to property pages or horizontal transitions.
- **Page Transitions** - Smart detection of back/forward navigation with distinct animations.
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

### Novidades na 1.0.2

Boa parte do mod foi reescrita nesta versao. As animacoes agora usam o mecanismo padrao de camadas da propria DirectUI, entao o movimento e os efeitos dos itens rodam no compositor do sistema, e nao na thread de interface do Explorer: continuam suaves mesmo enquanto miniaturas e pastas grandes ainda estao carregando. Nas builds em que isso nao estiver disponivel, o mod mantem o layout nativo do Explorer e o fade.

- **Transicoes de pagina mais suaves** - O proprio conteudo da pasta se move, sem mover as janelas do Explorer. Os padroes agora seguem o WinUI: 450 ms (150 ms de espera mais 300 ms de movimento) e entrada de 140 DIPs. Abrir uma pasta usa a mesma animacao de entrada.
- **Revelacao escalonada dos itens (opcional)** - Os itens crescem e aparecem um depois do outro, com as mesmas curvas da transicao AddDelete do WinUI. Nas builds suportadas, itens excluidos tambem saem com uma animacao curta.
- **Espera pelos seus arquivos** - Em pastas lentas a pagina espera os primeiros arquivos aparecerem antes de se mover, em vez de animar uma view vazia.
- **Sem flashes** - O fim de cada animacao fica coberto, e o conteudo nao pisca mais vazio nem mostra texto duplicado.
- **Responde a voce** - Clicar ou pressionar uma tecla durante a animacao a conclui rapido, e a rolagem devolve o conteudo na hora, inclusive com mods de rolagem suave.
- Requer resolucao de simbolos no explorerframe.dll, e tambem duser.dll, dui70.dll e windows.storage.dll para o escalonamento e as exclusoes (feito automaticamente pelo Windhawk).

### Recursos

- Animacao suave com easing refinado
- Transicoes de pagina contextuais (voltar, avancar, entrada)
- Fade opcional: overlay DirectComposition acelerado por GPU, compativel com Mica e Acrilico
- Revelacao escalonada opcional dos itens com as curvas AddDelete do WinUI
- Delay opcional na primeira abertura
- Delay de navegacao para esconder o "Trabalhando nisso..."
- Sem flicker / sem flashes pretos
- Compativel com o mod Translucent Windows

### Configuracoes

- **Duracao da Animacao** - Define quanto tempo a animacao leva.
- **Distancia do Slide** - Define o deslocamento inicial.
- **Fade** - Overlay de fade GPU mostrando o conteudo da pasta anterior. Compativel com mods de backdrop Mica/Acrilico.
- **Revelacao escalonada dos itens** - Usa composicao nativa para os keyframes de escala e opacidade AddDelete do WinUI, com fallback DirectUI automatico quando indisponivel. Builds Shell suportadas tambem animam a remocao real dos itens com uma saida curta em composicao. Os demais ficam visualmente parados ate revelar o layout final do Explorer, sem animacao de reorganizacao. Nao altera o layout nativo nem views XAML. Desativado por padrao.
- **Direcao Reversa** - Faz o conteudo descer ao inves de subir. Nao se aplica a paginas de Propriedades nem a transicoes horizontais.
- **Transicoes de Pagina** - Deteccao inteligente de navegacao voltar/avancar com animacoes distintas.
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

### Novedades en 1.0.2

Buena parte del mod se reescribio en esta version. Las animaciones ahora usan el mecanismo estandar de capas de la propia DirectUI, asi que el movimiento y los efectos de los elementos se ejecutan en el compositor del sistema y no en el hilo de interfaz del Explorador: siguen fluidos incluso mientras las miniaturas y las carpetas grandes todavia se estan cargando. En las versiones donde esto no esta disponible, el mod conserva la distribucion nativa del Explorador y el fundido.

- **Transiciones de pagina mas fluidas** - El propio contenido de la carpeta se mueve, sin mover las ventanas del Explorador. Los valores predeterminados ahora siguen WinUI: 450 ms (150 ms de espera mas 300 ms de movimiento) y una entrada de 140 DIPs. Abrir una carpeta usa la misma animacion de entrada.
- **Aparicion escalonada de elementos (opcional)** - Los elementos crecen y aparecen uno tras otro, con las mismas curvas de la transicion AddDelete de WinUI. En las versiones compatibles, los elementos eliminados tambien salen con una animacion corta.
- **Espera a tus archivos** - En carpetas lentas la pagina espera a que aparezcan los primeros archivos antes de moverse, en lugar de animar una vista vacia.
- **Sin destellos** - El final de cada animacion queda cubierto, y el contenido ya no parpadea vacio ni muestra texto duplicado.
- **Responde a ti** - Hacer clic o pulsar una tecla durante la animacion la termina rapido, y el desplazamiento devuelve el contenido al instante, tambien con mods de desplazamiento suave.
- Requiere resolucion de simbolos en explorerframe.dll, y tambien duser.dll, dui70.dll y windows.storage.dll para el escalonado y las eliminaciones (manejado automaticamente por Windhawk).

### Caracteristicas

- Animacion suave con easing refinado
- Transiciones de pagina contextuales (atras, adelante, entrada)
- Fundido opcional: overlay DirectComposition acelerado por GPU, compatible con Mica y Acrilico
- Aparicion escalonada opcional de elementos con las curvas AddDelete de WinUI
- Retraso opcional en la primera apertura
- Retraso de navegacion para ocultar el texto de carga
- Sin parpadeo / sin destellos negros
- Compatible con el mod Translucent Windows

### Configuracion

- **Duracion de la Animacion** - Controla cuanto dura el efecto.
- **Distancia del Deslizamiento** - Define el desplazamiento inicial.
- **Fundido** - Overlay de fundido GPU mostrando el contenido de la carpeta anterior. Compatible con mods de backdrop Mica/Acrilico.
- **Aparicion escalonada de elementos** - Usa composicion nativa para los keyframes de escala y opacidad AddDelete de WinUI, con alternativa DirectUI automatica cuando no esta disponible. Las versiones Shell compatibles tambien animan las eliminaciones reales con una salida corta en composicion. Los demas permanecen visualmente quietos hasta revelar la distribucion final del Explorador, sin animacion de reorganizacion. No cambia la distribucion nativa ni las vistas XAML. Desactivado por defecto.
- **Direccion Inversa** - Desliza hacia abajo en lugar de hacia arriba. No se aplica a paginas de Propiedades ni a transiciones horizontales.
- **Transiciones de Pagina** - Deteccion inteligente de navegacion atras/adelante con animaciones distintas.
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
- duration: 450
  $name: Animation duration (ms)
  $name:pt: Duracao da animacao (ms)
  $name:es: Duracion de la animacion (ms)
  $description: Total timeline including the initial hold. WinUI default is 150 ms hold plus 300 ms motion.
  $description:pt: Timeline total incluindo a espera inicial. O padrao WinUI e 150 ms de espera mais 300 ms de movimento.
  $description:es: Timeline total con la espera inicial. WinUI usa 150 ms de espera mas 300 ms de movimiento.

- distance: 140
  $name: Entrance distance (DIPs; WinUI default 140)
  $name:pt: Distancia da entrance (DIPs; padrao WinUI 140)
  $name:es: Distancia entrance (DIPs; predeterminado WinUI 140)

- fade: false
  $name: Fade
  $name:pt: Fade
  $name:es: Fundido
  $description: Fades the background out smoothly while navigating, using a DirectComposition opacity animation (GPU). Compatible with Mica, Acrylic and transparency mods.
  $description:pt: Faz o fundo desaparecer gradualmente ao navegar, usando animacao de opacidade via DirectComposition (GPU). Compativel com Mica, Acrilico e mods de transparencia.
  $description:es: Desvanece el fondo suavemente al navegar, usando animacion de opacidad via DirectComposition (GPU). Compatible con Mica, Acrilico y mods de transparencia.

- stagger: false
  $name: Staggered item reveal
  $name:pt: Revelacao escalonada dos itens
  $name:es: Aparicion escalonada de elementos
  $description: "Animates scale and opacity through native composition, with automatic DirectUI fallback. Uses WinUI AddDelete curves: 90% to 100% over 333 ms and an extra 80 ms entrance hold at the default duration. Supported Shell builds animate removals over 100 ms. Does not change layout. DirectUI views only; disabled by default."
  $description:pt: "Anima escala e opacidade por composicao nativa, com fallback DirectUI automatico. Usa curvas AddDelete do WinUI: 90% a 100% durante 333 ms e espera adicional de 80 ms na entrada com a duracao padrao. Builds Shell suportadas animam remocoes em 100 ms. Nao altera layout. Apenas views DirectUI; desativado por padrao."
  $description:es: "Anima escala y opacidad mediante composicion nativa, con alternativa DirectUI automatica. Usa curvas AddDelete de WinUI: del 90% al 100% durante 333 ms y espera adicional de 80 ms con la duracion predeterminada. Las versiones Shell compatibles animan eliminaciones en 100 ms. No cambia la distribucion. Solo vistas DirectUI; desactivado por defecto."

- reverseDirection: false
  $name: Reverse direction
  $name:pt: Direcao reversa
  $name:es: Direccion inversa
  $description: Content slides down instead of up. Does not apply to Properties pages, the navigation pane, or horizontal transitions (back/forward).
  $description:pt: O conteudo desce ao inves de subir. Nao se aplica as paginas de Propriedades, ao painel de navegacao, nem as transicoes horizontais (voltar/avancar).
  $description:es: El contenido se desliza hacia abajo en lugar de hacia arriba. No se aplica a las paginas de Propiedades, al panel de navegacion, ni a las transiciones horizontales (atras/adelante).

- pageTransitions: false
  $name: Page transitions
  $name:pt: Transicoes de pagina
  $name:es: Transiciones de pagina
  $description: Automatically detects the navigation type and applies different animations. Back slides from the left, forward from the right, and subfolders use the entrance transition. Requires symbol resolution on explorerframe.dll (automatic). When disabled, all navigations use the default entrance animation.
  $description:pt: Detecta automaticamente o tipo de navegacao e aplica animacoes diferentes. Voltar desliza da esquerda, avancar da direita e subpastas usam a transicao entrance. Requer resolucao de simbolos no explorerframe.dll (automatico). Quando desativado, todas as navegacoes usam a animacao de entrada padrao.
  $description:es: Detecta automaticamente el tipo de navegacion y aplica animaciones diferentes. Atras desliza desde la izquierda, adelante desde la derecha y las subcarpetas usan la transicion entrance. Requiere resolucion de simbolos en explorerframe.dll (automatico). Cuando esta desactivado, todas las navegaciones usan la animacion de entrada predeterminada.

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
#include <wrl/client.h>
#include <commctrl.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <cwctype>
#include <mutex>
#include <memory>
#include <new>
#include <string>
#include <system_error>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// -----------------------------------------------
// Tipo de transicao de pagina
// -----------------------------------------------

enum class TransitionType {
    Entrance,     // slide vertical padrao (abrir janela, aba, navegacao geral)
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
    bool stagger = false;
};

static Settings g_s{450, 140, 500, 140, false, false, false, false, false, false};
static SRWLOCK  g_sLock = SRWLOCK_INIT;


static void LoadSettings() {
    Settings fresh;
    fresh.duration             = std::clamp(Wh_GetIntSetting(L"duration"),           60,  800);
    fresh.distance             = std::clamp(Wh_GetIntSetting(L"distance"),             0,  600);
    fresh.firstOpenDelay       = std::clamp(Wh_GetIntSetting(L"firstOpenDelay"),       0, 5000);
    fresh.navigationDelay      = std::clamp(Wh_GetIntSetting(L"navigationDelay"),      0,  500);
    fresh.alphaFade            = !!Wh_GetIntSetting(L"fade");
    fresh.reverseDirection     = !!Wh_GetIntSetting(L"reverseDirection");
    fresh.pageTransitions      = !!Wh_GetIntSetting(L"pageTransitions");
    fresh.animateNavPane       = !!Wh_GetIntSetting(L"animateNavPane");
    fresh.animatePropertyPages = !!Wh_GetIntSetting(L"propertyPages.animate");
    fresh.propPageTransitions  = !!Wh_GetIntSetting(L"propertyPages.tabTransitions");
    fresh.stagger              = !!Wh_GetIntSetting(L"stagger");

    AcquireSRWLockExclusive(&g_sLock);
    g_s = fresh;
    ReleaseSRWLockExclusive(&g_sLock);
}

// -----------------------------------------------
// Easing
// -----------------------------------------------

// WinUI ThemeTransitions.cpp: spline de entrada (0.1, 0.9), (0.2, 1).
// A coordenada X representa tempo; inverter a Bezier evita tratar u como t.
static double WinUIEntranceEase(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    double lo = 0.0, hi = 1.0;
    for (int i = 0; i < 24; ++i) {
        double u = (lo + hi) * 0.5;
        double v = 1.0 - u;
        double x = 0.3 * v * v * u + 0.6 * v * u * u + u * u * u;
        if (x < t) lo = u; else hi = u;
    }
    double u = (lo + hi) * 0.5;
    double v = 1.0 - u;
    return 2.7 * v * v * u + 3.0 * v * u * u + u * u * u;
}

// -----------------------------------------------
// Estado de animacao
//
// g_unloading: sinaliza para threads em voo que o mod esta sendo
// descarregado. Threads checam isso antes de acessar estado global.
// -----------------------------------------------

static std::unordered_set<HWND>       g_animating;
struct SlideAnimation;
static std::unordered_map<HWND, std::shared_ptr<SlideAnimation>> g_slides;
static unsigned                       g_animationWorkers = 0;
static std::mutex                      g_animMtx;
static std::condition_variable         g_animDoneCv;
static std::atomic<bool>               g_unloading{false};

// Permite interromper imediatamente os workers quando o mod
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
    bool           consumed = false;
    bool           dcompPrepared = false;
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
static UINT WM_WH_TRY_BROWSER_HOOK = 0;
static UINT WM_WH_ANIMATION_FRAME = 0;


// Forward declarations -- definidos mais abaixo, usados no subclass proc
// e em TrySubclassExplorerWindow que aparecem antes no arquivo.
using BrowseObject_t = HRESULT(WINAPI*)(void*, LPCITEMIDLIST, UINT);
static BrowseObject_t g_origBrowseObject = nullptr;
// Serializa TryHookBrowseObjectSymbols -- sem isso, duas janelas CabinetWClass
// criadas quase ao mesmo tempo (ex: restauracao de sessao) podem chamar
// Wh_SetFunctionHook no mesmo endereco a partir de threads diferentes.
static std::mutex g_browseObjectHookMtx;
static std::atomic<bool> g_browserHookInstalled{false};
static void TryHookBrowseObjectSymbols();
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
    WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == WM_APPCOMMAND && !g_unloading.load(std::memory_order_relaxed)) {
        int cmd = GET_APPCOMMAND_LPARAM(lp);
        if (cmd == APPCOMMAND_BROWSER_BACKWARD ||
            cmd == APPCOMMAND_BROWSER_FORWARD) {
            TransitionType type = (cmd == APPCOMMAND_BROWSER_BACKWARD)
                                  ? TransitionType::SlideBack
                                  : TransitionType::SlideForward;
            std::lock_guard<std::mutex> lk(g_pendingMtx);
            g_pendingNav[hwnd] = {type, GetTickCount64()};
        }
    }
    // Retry do hook via simbolos apos a janela estar totalmente inicializada.
    // Executado no thread STA do Explorer onde COM ja esta ativo.
    if (msg == WM_WH_TRY_BROWSER_HOOK && !g_unloading.load(std::memory_order_relaxed)) {
        if (!g_browserHookInstalled.load(std::memory_order_acquire)) {
            TryHookBrowseObjectSymbols();
            if (g_browserHookInstalled.load(std::memory_order_acquire) &&
                !Wh_ApplyHookOperations())
                Wh_Log(L"[INIT] falha ao aplicar hook de navegacao");
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
        {
            std::lock_guard<std::mutex> lk(g_pendingMtx);
            g_pendingNav.erase(hwnd);
        }
        {
            std::lock_guard<std::mutex> lk(g_subclassMtx);
            g_subclassedWindows.erase(hwnd);
        }
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Subclass proc de PropertySheet: interceta TCN_SELCHANGING para capturar
// o indice da aba anterior antes do CommCtrl mostrar a nova pagina.
// Corre no UI thread do Explorer -- sem concorrencia com Classify.
static LRESULT CALLBACK PropSheetSubclassProc(HWND hwnd, UINT msg,
    WPARAM wp, LPARAM lp, DWORD_PTR) {
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
    {
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        if (g_propSheets.count(hwndSheet)) return;
    }
    // Lock liberado antes da chamada -- SetWindowSubclassFromAnyThread pode
    // bloquear (SendMessage cross-thread) se hwndSheet estiver noutra thread;
    // segurar g_propSheetMtx durante isso travaria PropSheetSubclassProc,
    // Classify() e Wh_ModUninit ate a chamada retornar.
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwndSheet, PropSheetSubclassProc, PROPSHEET_SUBCLASS_ID)) {
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        g_propSheets[hwndSheet] = {hTab, buttonRowTop, -1};
    }
}

static void TrySubclassExplorerWindow(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        if (g_subclassedWindows.count(hwnd)) return;
    }
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd, ExplorerSubclassProc, APPCOMMAND_SUBCLASS_ID)) {
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        g_subclassedWindows.insert(hwnd);
    } else {
        Wh_Log(L"[SUBCLASS] falhou para hwnd=%p", hwnd);
        return;
    }
    // Se o hook de navegacao ainda nao foi instalado, postar uma mensagem
    // para retry depois que a janela estiver totalmente inicializada.
    // PostMessage adia a resolucao ate o thread da janela processar mensagens.
    if (!g_browserHookInstalled.load(std::memory_order_acquire))
        PostMessageW(hwnd, WM_WH_TRY_BROWSER_HOOK, 0, 0);
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

// -----------------------------------------------
// Eixo de animacao
// -----------------------------------------------

enum class AnimAxis { Vertical, Horizontal };

// -----------------------------------------------
// Deteccao de janelas
// -----------------------------------------------

static BOOL CALLBACK DetectFolderPayload(HWND child, LPARAM data) {
        wchar_t name[64]{};
        if (GetClassNameW(child, name, ARRAYSIZE(name)) &&
            (!wcscmp(name, L"DirectUIHWND") ||
             !wcscmp(name, L"FileExplorer_FolderViewIsland") ||
             !wcscmp(name, L"UIItemsView") || !wcscmp(name, L"ItemsView") ||
             !wcscmp(name, L"SysListView32"))) {
            *reinterpret_cast<bool*>(data) = true;
            return FALSE;
        }
        return TRUE;
}
static bool HasFolderPayload(HWND hwnd) {
    bool found = false;
    EnumChildWindows(hwnd, DetectFolderPayload, reinterpret_cast<LPARAM>(&found));
    return found;
}

static bool IsExplorerContent(HWND hwnd) {
    wchar_t c[128] = {}; RealGetWindowClassW(hwnd, c, 128);
    bool defView = !wcscmp(c, L"SHELLDLL_DefView");
    if (!defView && wcscmp(c, L"UIItemsView") && wcscmp(c, L"ItemsView"))
        return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    wchar_t rc[64] = {}; RealGetWindowClassW(root, rc, 64);
    if (wcscmp(rc, L"CabinetWClass")) return false;
    // O DefView auxiliar do Painel de Controle nao hospeda a superficie:
    // ela pertence a um DUIViewWndClassName irmao. Um DefView ainda sendo
    // construido, sem esse irmao, continua elegivel para a entrance normal.
    if (defView && !HasFolderPayload(hwnd)) {
        HWND parent = GetParent(hwnd);
        wchar_t pc[64]{};
        RealGetWindowClassW(parent, pc, ARRAYSIZE(pc));
        if (!wcscmp(pc, L"ShellTabWindowClass")) {
            for (HWND child = GetWindow(parent, GW_CHILD); child;
                 child = GetWindow(child, GW_HWNDNEXT)) {
                wchar_t cc[64]{};
                RealGetWindowClassW(child, cc, ARRAYSIZE(cc));
                if (!wcscmp(cc, L"DUIViewWndClassName")) return false;
            }
        }
    }

    // Identidade canonica da pasta, usada pelo tracking de itens DirectUI.
    // O alvo visual e escolhido separadamente, sem duplicar host e payload.
    if (!defView) {
        for (HWND parent = GetParent(hwnd); parent && parent != root;
             parent = GetParent(parent)) {
            wchar_t pc[64]{};
            RealGetWindowClassW(parent, pc, 64);
            if (!wcscmp(pc, L"SHELLDLL_DefView")) return false;
        }
    }
    return true;
}

struct FolderChildSearch { PCWSTR name; HWND found; };
static BOOL CALLBACK FindFolderChildProc(HWND child, LPARAM data) {
    auto& search = *reinterpret_cast<FolderChildSearch*>(data);
    wchar_t name[64]{};
    if (GetClassNameW(child, name, ARRAYSIZE(name)) && !wcscmp(name, search.name)) {
        search.found = child;
        return FALSE;
    }
    return TRUE;
}
static HWND FindFolderChild(HWND view, PCWSTR className) {
    FolderChildSearch search{className, nullptr};
    EnumChildWindows(view, FindFolderChildProc, reinterpret_cast<LPARAM>(&search));
    return search.found;
}
static bool IsFolderIsland(HWND hwnd) {
    wchar_t name[64]{}, rootClass[64]{};
    GetClassNameW(hwnd, name, ARRAYSIZE(name));
    GetClassNameW(GetAncestor(hwnd, GA_ROOT), rootClass, ARRAYSIZE(rootClass));
    return !wcscmp(name, L"FileExplorer_FolderViewIsland") && !wcscmp(rootClass, L"CabinetWClass");
}
static HWND ResolveAnimationTarget(HWND hwnd) {
    if (!IsExplorerContent(hwnd)) return hwnd;
    if (HWND island = FindFolderChild(hwnd, L"FileExplorer_FolderViewIsland")) return island;
    // Um DefView ainda vazio aguarda o payload, em vez de mover o host.
    return HasFolderPayload(hwnd) ? hwnd : nullptr;
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
    // "Atributos Avancados") sao WS_POPUP top-level e nao pertencem ao
    // espaco de coordenadas cliente usado no slide das paginas filhas.
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
//   3. Anima opacidade via DComp
//   4. ShowWindow + ApplySlideFrame correm normalmente (sem BeginFade/SetAlpha/EndFade)
//   5. Worker leve destroi os recursos DComp apos a animacao
//
// Compativel com Mica, Acrylic e mods de transparencia porque:
//   - Nao aplica WS_EX_LAYERED na janela de conteudo
//   - O overlay e uma layer DComp separada, nao interfere com o backdrop DWM
// -----------------------------------------------

// WinUI Entrance: fade de saida de 150 ms numa timeline de 450 ms.
// A configuracao de duracao escala a mesma proporcao, inclusive na abertura.
static int CalcFadeDuration(int effectiveDuration) {
    return std::max(1, effectiveDuration / 3);
}

// Compensa o intervalo entre BrowseObject e ShowWindow para o bitmap antigo
// começar a desaparecer apenas quando a nova view ja estiver composta.
static constexpr int DCOMP_FADE_SYNC_MARGIN_MS = 32;
// Entradas preparadas em ApplySlideFrame (abertura e novas abas) recebem tempo
// suficiente para o compositor apresentar o hold e toda a transicao.
static constexpr int   ENTRANCE_DCOMP_HOLD_MS       = 48;

static ID3D11Device*               g_d3dDevice  = nullptr;
static IDXGIDevice*                g_dxgiDevice = nullptr;
static std::atomic<IDCompositionDesktopDevice*> g_dcDevice{nullptr};
// Mantem dcomp.dll carregada enquanto qualquer objeto criado por ela existir.
static HMODULE g_dcompModule = nullptr;

struct DCompFadeAnim {
    IDCompositionTarget*      pTarget  = nullptr;
    IDCompositionVisual2*     pVisual  = nullptr;
    IDCompositionSurface*     pSurface = nullptr;
    IDCompositionEffectGroup* pEffect  = nullptr;
    uint64_t                  generation = 0; // identifica instancia unica do fade
    int                       curtainHolders = 0; // > 0: cortina de devolucao ativa
    ULONGLONG                 curtainReadyAt = 0;
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

// Geometria da area de conteudo ainda composta pelo DWM, capturada antes
// da navegacao substituir o conteudo. StartDCompFade le direto do ecra
// (ptScreen) para dentro da superficie DComp -- sem HBITMAP intermediario.
struct CapturedFrame {
    POINT ptScreen  = {};   // origem no ecra, fonte do BitBlt em StartDCompFade
    POINT ptInRoot  = {};   // offset do conteudo dentro do root (CabinetWClass)
    int   w         = 0;
    int   h         = 0;
};

static bool CaptureContentFrame(HWND hwndContent, HWND root,
                                CapturedFrame& frame) {
    RECT rc{};
    if (!GetClientRect(hwndContent, &rc)) return false;

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 1 || h <= 1) return false;

    POINT ptInRoot{0, 0};
    SetLastError(ERROR_SUCCESS);
    if (!MapWindowPoints(hwndContent, root, &ptInRoot, 1)) {
        DWORD error = GetLastError();
        if (error != ERROR_SUCCESS) return false;
    }

    POINT ptScreen{0, 0};
    if (!ClientToScreen(hwndContent, &ptScreen)) return false;

    frame = {ptScreen, ptInRoot, w, h};
    return true;
}

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
            // Dorme ate o proximo item, uma notificacao (novo item, talvez com
            // destroyAt mais cedo) ou o stop; o topo do laco reavalia tudo.
            // Sem predicado: com ele, a notificacao de um novo item era ignorada.
            g_cleanupCv.wait_for(lk, std::chrono::milliseconds(nearest - now));
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

// Prepara o overlay DComp sobre a janela-alvo de conteudo ou navegacao.
// Chamado em BrowseObject (conteudo antigo ainda visivel) para garantir
// que o overlay esta commitado antes de qualquer nova janela aparecer.
// hwndRoot: CabinetWClass para a pasta, ou HWND da arvore para o painel.
// frame: conteudo capturado do ecra (inclui Mica/Acrylic).
// aboveChildWindows=TRUE e usado apenas nas entradas criadas em ApplySlideFrame:
// o bitmap precisa ficar acima da nova ItemsView para o crossfade ser visivel.
// BrowseObject mantem FALSE para preservar o empilhamento das navegacoes.
// Le os pixels direto do ecra (frame.ptScreen) para dentro da superficie
// DComp num unico BitBlt -- sem HBITMAP intermediario.
static bool StartDCompFade(HWND hwndRoot, CapturedFrame frame,
                           int fadeMs, int startDelayMs,
                           bool aboveChildWindows) {
    // Serializar TODAS as chamadas DComp no device e nos seus objectos.
    // DirectComposition nao e internamente thread-safe; sem este lock, o
    // cleanup worker pode chamar SetRoot/Release/Commit concorrentemente
    // com a criacao de um novo fade, corrompendo o estado de composicao.
    //
    // O device e lido AQUI DENTRO, ja com o lock adquirido, de proposito --
    // Wh_ModUninit tambem liberta g_dcDevice sob este mesmo mutex, entao ler
    // o ponteiro fora do lock deixaria uma janela onde StartDCompFade guarda
    // um dcDev valido no momento da leitura mas Wh_ModUninit o liberta antes
    // do lkDC ser adquirido, e as chamadas abaixo usariam um ponteiro solto
    // (use-after-free). Ler dentro do lock fecha essa janela: ou esta funcao
    // termina antes de Wh_ModUninit conseguir o lock, ou Wh_ModUninit ja
    // zerou g_dcDevice antes desta leitura e caimos no fallback abaixo.
    std::lock_guard<std::mutex> lkDC(g_dcFadeMtx);

    IDCompositionDesktopDevice* dcDev = g_dcDevice.load(std::memory_order_acquire);
    if (!dcDev || frame.w <= 0 || frame.h <= 0) {
        return false;
    }

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

    hr = dcDev->CreateTargetForHwnd(
        hwndRoot, aboveChildWindows ? TRUE : FALSE, &anim.pTarget);
    if (FAILED(hr)) {
        return false;
    }

    hr = dcDev->CreateSurface((UINT)frame.w, (UINT)frame.h,
                                   DXGI_FORMAT_B8G8R8A8_UNORM,
                                   DXGI_ALPHA_MODE_IGNORE,
                                   &anim.pSurface);
    if (FAILED(hr)) {
        anim.pTarget->Release(); return false;
    }

    // BitBlt unico direto do ecra para a superficie DComp -- elimina o
    // HBITMAP intermediario que CaptureContentFrame usava antes.
    bool drawn = false;
    {
        POINT offset{};
        IDXGISurface1* pGdi = nullptr;
        if (SUCCEEDED(anim.pSurface->BeginDraw(nullptr,
                          __uuidof(IDXGISurface1), (void**)&pGdi, &offset)) && pGdi) {
            HDC hdcSurf = nullptr;
            if (SUCCEEDED(pGdi->GetDC(FALSE, &hdcSurf))) {
                HDC hdcScreen = GetDC(nullptr);
                if (hdcScreen) {
                    drawn = !!BitBlt(hdcSurf, offset.x, offset.y, frame.w, frame.h,
                                     hdcScreen, frame.ptScreen.x, frame.ptScreen.y, SRCCOPY);
                    ReleaseDC(nullptr, hdcScreen);
                }
                pGdi->ReleaseDC(nullptr);
            }
            pGdi->Release();
            anim.pSurface->EndDraw();
        }
    }
    // Superficie IGNORE sem copia teria conteudo indefinido (em geral preto).
    if (!drawn) {
        anim.pSurface->Release(); anim.pTarget->Release(); return false;
    }

    hr = dcDev->CreateVisual(&anim.pVisual);
    if (FAILED(hr)) {
        anim.pSurface->Release(); anim.pTarget->Release(); return false;
    }
    anim.pVisual->SetContent(anim.pSurface);
    anim.pVisual->SetOffsetX((float)frame.ptInRoot.x);
    anim.pVisual->SetOffsetY((float)frame.ptInRoot.y);

    IDCompositionEffectGroup* pEffect = nullptr;
    hr = dcDev->CreateEffectGroup(&pEffect);
    if (FAILED(hr) || !pEffect) {
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    IDCompositionAnimation* pOpacity = nullptr;
    hr = dcDev->CreateAnimation(&pOpacity);
    if (FAILED(hr) || !pOpacity) {
        pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    const double durSec   = fadeMs / 1000.0;
    const double delaySec = startDelayMs / 1000.0;
    if (delaySec > 0.0)
        hr = pOpacity->AddCubic(0.0, 1.0f, 0.0f, 0.0f, 0.0f);
    // O hold sincroniza o crossfade com o inicio da nova view.
    if (SUCCEEDED(hr))
        hr = pOpacity->AddCubic(delaySec, 1.0f,
                               (float)(-1.0 / durSec), 0.0f, 0.0f);
    if (SUCCEEDED(hr)) hr = pOpacity->End(delaySec + durSec, 0.0f);
    if (SUCCEEDED(hr)) hr = pEffect->SetOpacity(pOpacity);
    pOpacity->Release();
    if (FAILED(hr)) {
        pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    hr = anim.pVisual->SetEffect(pEffect);
    if (FAILED(hr)) {
        pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }
    anim.pEffect = pEffect;

    hr = anim.pTarget->SetRoot(anim.pVisual);
    if (FAILED(hr)) {
        anim.pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
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

    // O overlay comeca imediatamente, como no caminho DComp original.
    dcDev->AddRef();
    {
        std::lock_guard<std::mutex> lk(g_cleanupMtx);
        g_cleanupQueue.push_back({hwndRoot, dcDev,
                                  GetTickCount64() +
                                      (ULONGLONG)(startDelayMs + fadeMs + 50),
                                  gen});
        g_cleanupCv.notify_one();
    }

    return true;
}

static bool HasDCompFade(HWND hwndRoot) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    return g_dcFades.find(hwndRoot) != g_dcFades.end();
}

// -----------------------------------------------
// Cortina de devolucao composicao -> GDI
//
// Ao despromover uma camada, o DirectUI repinta o conteudo pelo GDI da janela
// filha enquanto o visual da camada sai pelo DirectComposition; os dois chegam
// ao DWM sem sincronia (frame vazio ou texto sobreposto). Uma copia opaca da
// view, no device do mod, cobre a troca ate o GDI ja reparado ser apresentado.
// -----------------------------------------------

static constexpr int CURTAIN_READY_MS  = 34;  // copia apresentada antes de despromover
static constexpr int CURTAIN_HOLD_MS   = 34;  // reparo GDI apresentado antes do fade
static constexpr int CURTAIN_FADE_MS   = 80;
static constexpr int CURTAIN_SAFETY_MS = 600; // retirada automatica se o owner nao liberar

static void DestroyFadeLocked(IDCompositionDesktopDevice* dcDev, DCompFadeAnim& a) {
    if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
    if (a.pEffect)  a.pEffect->Release();
    if (a.pVisual)  a.pVisual->Release();
    if (a.pSurface) a.pSurface->Release();
    if (dcDev) dcDev->Commit();
}

// Retira na hora a cortina dessa view, inclusive ja em fade-out.
static void DropOverlay(HWND key) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(key);
    if (it == g_dcFades.end()) return;
    DCompFadeAnim dead = it->second;
    g_dcFades.erase(it);
    DestroyFadeLocked(g_dcDevice.load(std::memory_order_acquire), dead);
}

// Cortinas (inclusive em fade-out) das views desta thread.
static int CurtainViews(DWORD thread, HWND* out, int max) {
    int n = 0;
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    for (const auto& [key, anim] : g_dcFades)
        if (anim.curtainReadyAt && n < max && GetWindowThreadProcessId(key, nullptr) == thread)
            out[n++] = key;
    return n;
}

// Menu (Win32 ou WinUI), tooltip ou popup da propria thread sobre a view entraria
// na copia e ficaria como fantasma ao fechar. Outras threads (barra de tarefas
// auto-oculta, outras janelas) nao contam: o primeiro plano ja as cobre.
// EnumWindows percorre uma lista tirada antes dos callbacks, de cima para baixo na
// ordem Z: um laco GetWindow(GW_HWNDPREV) pode ciclar se a ordem mudar no meio.
struct PopupSearch {
    HWND root;
    RECT area;
    DWORD thread;
    HRESULT(WINAPI* cloakedQuery)(HWND, DWORD, PVOID, DWORD);
    int visited;
    bool found;
};
static BOOL CALLBACK PopupSearchProc(HWND window, LPARAM data) {
    auto& search = *reinterpret_cast<PopupSearch*>(data);
    if (window == search.root || ++search.visited > 4096) return FALSE;
    RECT rect{}, overlap{};
    if (GetWindowThreadProcessId(window, nullptr) != search.thread || !IsWindowVisible(window) ||
        !GetWindowRect(window, &rect) || !IntersectRect(&overlap, &rect, &search.area)) return TRUE;
    DWORD cloaked = 0; // DWMWA_CLOAKED
    if (search.cloakedQuery && SUCCEEDED(search.cloakedQuery(window, 14, &cloaked, sizeof(cloaked))) && cloaked)
        return TRUE;
    search.found = true;
    return FALSE;
}
static bool PopupOverView(HWND view, HWND root) {
    using CloakedFn = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);
    HMODULE dwm = GetModuleHandleW(L"dwmapi.dll");
    PopupSearch search{root, {}, GetWindowThreadProcessId(view, nullptr),
        dwm ? reinterpret_cast<CloakedFn>(GetProcAddress(dwm, "DwmGetWindowAttribute")) : nullptr, 0, false};
    if (!GetWindowRect(view, &search.area)) return false;
    EnumWindows(PopupSearchProc, reinterpret_cast<LPARAM>(&search));
    return search.found;
}

// A retirada de seguranca comeca em CURTAIN_SAFETY_MS desde a criacao.
static ULONGLONG CurtainSafetyAt(const DCompFadeAnim& a) {
    return a.curtainReadyAt - CURTAIN_READY_MS + CURTAIN_SAFETY_MS;
}

// Chamado no owner da view. Uma cortina ja levantada e compartilhada (contagem);
// generation identifica a instancia que cada dono deve liberar.
static bool RaiseCurtain(HWND view, ULONGLONG& readyAt, uint64_t& generation) {
    if (g_unloading.load(std::memory_order_acquire) || !IsWindowVisible(view)) return false;
    // A copia vem da tela: outra janela sobre o Explorer entraria nela.
    HWND root = GetAncestor(view, GA_ROOT);
    if (!root || GetForegroundWindow() != root) return false;
    {
        std::lock_guard<std::mutex> lk(g_dcFadeMtx);
        auto it = g_dcFades.find(view);
        if (it != g_dcFades.end() && it->second.curtainHolders > 0) {
            // Perto da retirada de seguranca nao cobriria a devolucao; uma copia
            // nova copiaria a propria cortina.
            if (GetTickCount64() + CURTAIN_HOLD_MS + CURTAIN_FADE_MS + 100 >= CurtainSafetyAt(it->second))
                return false;
            ++it->second.curtainHolders;
            readyAt = it->second.curtainReadyAt;
            generation = it->second.generation;
            return true;
        }
    }
    if (PopupOverView(view, root)) return false;
    CapturedFrame frame;
    if (!CaptureContentFrame(view, view, frame) ||
        !StartDCompFade(view, frame, CURTAIN_FADE_MS, CURTAIN_SAFETY_MS, true))
        return false;
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(view);
    if (it == g_dcFades.end()) return false;
    it->second.curtainHolders = 1;
    it->second.curtainReadyAt = readyAt = GetTickCount64() + CURTAIN_READY_MS;
    generation = it->second.generation;
    return true;
}

// Seguro em qualquer thread: so toca o device do mod.
static void LowerCurtain(HWND view, uint64_t generation) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(view);
    if (it == g_dcFades.end() || it->second.generation != generation ||
        it->second.curtainHolders <= 0 || --it->second.curtainHolders)
        return;
    // No unload o worker de cleanup ja terminou; Wh_ModUninit libera a entrada.
    IDCompositionDesktopDevice* dcDev = g_dcDevice.load(std::memory_order_acquire);
    if (!dcDev || g_unloading.load(std::memory_order_acquire)) return;
    // Ja na retirada de seguranca: reanimar traria a copia antiga de volta.
    if (GetTickCount64() >= CurtainSafetyAt(it->second)) return;
    DCompFadeAnim& a = it->second;
    const double hold = CURTAIN_HOLD_MS / 1000.0, fade = CURTAIN_FADE_MS / 1000.0;
    IDCompositionAnimation* opacity = nullptr;
    HRESULT hr = dcDev->CreateAnimation(&opacity);
    if (SUCCEEDED(hr)) hr = opacity->AddCubic(0.0, 1.0f, 0.0f, 0.0f, 0.0f);
    if (SUCCEEDED(hr)) hr = opacity->AddCubic(hold, 1.0f, (float)(-1.0 / fade), 0.0f, 0.0f);
    if (SUCCEEDED(hr)) hr = opacity->End(hold + fade, 0.0f);
    if (SUCCEEDED(hr)) hr = a.pEffect->SetOpacity(opacity);
    if (opacity) opacity->Release();
    if (SUCCEEDED(hr)) hr = dcDev->Commit();
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] retirada da cortina falhou: %08X", (unsigned)hr);
        DCompFadeAnim dead = a;
        g_dcFades.erase(it);
        DestroyFadeLocked(dcDev, dead);
        return;
    }
    dcDev->AddRef();
    std::lock_guard<std::mutex> lkq(g_cleanupMtx);
    g_cleanupQueue.push_back({view, dcDev,
        GetTickCount64() + (ULONGLONG)(CURTAIN_HOLD_MS + CURTAIN_FADE_MS + 50), a.generation});
    g_cleanupCv.notify_one();
}

// Rolagem do conteudo (definida depois de ItemStagger).
static void HandleContentScroll();

// Navegacao: o conteudo antigo nao pode permanecer sobre a pagina nova.
static void DropCurtain(HWND view) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(view);
    // Inclui a cortina ja em fade-out (sem donos): a view so e chave de cortinas.
    if (it == g_dcFades.end() || !it->second.curtainReadyAt) return;
    DCompFadeAnim dead = it->second;
    g_dcFades.erase(it);
    DestroyFadeLocked(g_dcDevice.load(std::memory_order_acquire), dead);
}



// -----------------------------------------------
// Revelacao por item: escala nativa DirectUser, nunca X/Y ou margens.
// DOCUMENTED EXCEPTION: DirectUser actions sao o frame source dos elementos
// sem HWND; OnDestroy e DeleteHandle cobrem elemento e contexto destruido.
struct AnimationTimeline {
    ULONGLONG tickOrigin = 0;
    std::chrono::steady_clock::time_point origin{};
    void Init() {
        origin = std::chrono::steady_clock::now();
        tickOrigin = GetTickCount64();
    }
    double Now() const {
        // Steady clock/QPC evita passos de GetTickCount64 em monitores rapidos.
        return double(tickOrigin) + std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - origin).count();
    }
};
namespace ItemStagger {
#ifdef _WIN64
#define STAGGER_MEMBER
#else
#define STAGGER_MEMBER __thiscall
#endif
struct Action {
    DWORD size;
    float delay, duration, period;
    int repeat;
    DWORD pause;
    void (WINAPI* callback)(void*);
    void* data;
};
static_assert(sizeof(Action) == (sizeof(void*) == 8 ? 0x28 : 0x20));
using DisplayFn = HRESULT(WINAPI*)(HANDLE, void*, void*);
using DestroyFn = void(STAGGER_MEMBER*)(void*);
using GetClassFn = PCWSTR(STAGGER_MEMBER*)(void*, void**);
using ReleaseFn = void(STAGGER_MEMBER*)(void*);
using NodeFn = HANDLE(STAGGER_MEMBER*)(void*);
using ParentFn = void*(STAGGER_MEMBER*)(void*);
using AlphaGetFn = int(STAGGER_MEMBER*)(void*);
using AlphaSetFn = HRESULT(STAGGER_MEMBER*)(void*, int);
using IdFn = WORD(STAGGER_MEMBER*)(void*);
using GetPairFn = BOOL(WINAPI*)(HANDLE, float*, float*);
using SetPairFn = BOOL(WINAPI*)(HANDLE, float, float);
using RotationFn = BOOL(WINAPI*)(HANDLE, float*);
using RectFn = BOOL(WINAPI*)(HANDLE, RECT*, DWORD);
using HwndFn = HWND(WINAPI*)(void*);
using CreateActionFn = HANDLE(WINAPI*)(const Action*);
using DeleteFn = BOOL(WINAPI*)(HANDLE);
using InvalidateFn = BOOL(WINAPI*)(HANDLE);
using GadgetStyleFn = DWORD(WINAPI*)(HANDLE);
using LayerRefFn = BOOL(WINAPI*)(HANDLE);
using GadgetVisualFn = BOOL(WINAPI*)(UINT, HANDLE, void**, void**, void**);
// ABI privada do simbolo DuVisual::xrDrawFull(PaintInfo*). Copiar o contexto
// completo: o nativo tambem consulta origem e modo de desenho da camada.
struct NativePaintInfo {
    const RECT* dirty;
    const RECT* bounds;
    void* surface;
    void* transform;
    void* inverseTransform;
    BOOL buffered;
    BOOL useAlpha;
    POINT origin;
    BOOL drawingLayer;
};
static_assert(offsetof(NativePaintInfo, useAlpha) == (sizeof(void*) == 8 ? 0x2c : 0x18));
static_assert(sizeof(NativePaintInfo) == (sizeof(void*) == 8 ? 0x40 : 0x28));
using DrawFullFn = void(STAGGER_MEMBER*)(HANDLE, NativePaintInfo*);
static DrawFullFn originalDrawFull;
static std::atomic<bool> alphaPaintLive{false};
namespace Removal {
static void Bind(HWND view);
static void Observe(void* item, HWND view);
static void ObservePaint(void* element);
static void Destroy(void* element);
static void CancelView(HWND view);
static void ContextGone();
static void Reclaim();
static bool Active();
static int Views(HWND* out, int max);
static bool NeedsAlpha(HANDLE node);
static bool Stage(HMODULE dui, HMODULE shell);
static void Enable(bool ready);
static void Uninit();
}
static DisplayFn originalDisplay;
static DestroyFn originalDestroy;
static DeleteFn originalDelete;
static GetClassFn getClass;
static ReleaseFn releaseValue;
static NodeFn getNode;
static ParentFn getParent;
static AlphaGetFn getAlpha;
static AlphaSetFn setAlpha;
static IdFn getId; // opcional: sem ele, pasta vazia espera o limite de conteudo
static GetPairFn getScale, getCenter;
static SetPairFn setScale, setCenter;
static RotationFn getRotation;
static RectFn getRect;
static HwndFn getHwnd;
static CreateActionFn createAction;
static InvalidateFn invalidate;
static GadgetStyleFn getGadgetStyle, getGadgetFlags;
static LayerRefFn addLayeredRef, releaseLayeredRef;
static GadgetVisualFn getGadgetVisual;
static bool NativeCompositionAvailable() {
    return originalDrawFull && getGadgetStyle && getGadgetFlags &&
        addLayeredRef && releaseLayeredRef && getGadgetVisual;
}
// O device nativo pertence ao contexto DUI. Chamadas e referencias do mod
// ficam no owner; o mutex tambem serializa o descarte de um owner encerrado.
static std::mutex compositionMutex;
struct CompositionState {
    Microsoft::WRL::ComPtr<IDCompositionDevice> device;
    Microsoft::WRL::ComPtr<IDCompositionVisual> parent, content, bridge;
    Microsoft::WRL::ComPtr<IDCompositionScaleTransform> transform;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> curve;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> opacityCurve;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> pageX, pageY;
    Microsoft::WRL::ComPtr<IDCompositionEffectGroup> opacity;
    bool layerRef = false, contentRemoved = false, contentInBridge = false, bridgeAttached = false;
    ULONGLONG curveStart = 0, finishAt = 0;
};
static void DeleteComposition(CompositionState* state) {
    std::lock_guard<std::mutex> lock(compositionMutex);
    delete state;
}
static HMODULE duiModule, duserModule, itemModule;
static void* nativeItemVtable;
static std::mutex setupMutex;
static std::atomic<bool> live{false}, stopping{false};
static std::atomic<bool> setupAttempted{false};
static std::mutex preparationMutex;
static std::condition_variable preparationCv;
// Apenas os callbacks de lifecycle do Windhawk criam/joinam este worker.
// O ponteiro trivial tambem evita abort no teardown CRT do processo.
static std::thread* preparationThread = nullptr;
static std::atomic<bool> preparationRunning{false}, preparationFailed{false};
static bool preparationStop = true, preparationRequested = false, preparationDone = true;
enum { Free, Reserved, Busy };
constexpr int kMembers = 192;
constexpr int kFirstBatchWaitMs = 750;
constexpr int kPreparationWaitMs = 1500;
// AddDeleteThemeTransition calls TAS_ADDTOGRID / TA_ADDTOGRID_ADDED even
// for list contexts (WinUI LayoutTransition_partial.cpp). ThemeGenerator
// obtains these transforms from the Windows "Animations" theme. The extra
// 300/600 ms collection-reflow choreography is not a folder page transition.
constexpr float kAddScale = .90f;
static int ThemeTime(int nativeMs, int totalMs) {
    return std::max(1, int((int64_t(nativeMs) * std::max(1, totalMs) + 225) / 450));
}
// Folder entrance already has a preparation hold. Keep the native curve,
// shortening only AddToGrid's additional 166 ms delay to 80 ms.
static int AddHoldDuration(int totalMs) { return ThemeTime(80, totalMs); }
static int GrowDuration(int totalMs) { return ThemeTime(333, totalMs); }
static int DeleteDuration(int totalMs) { return ThemeTime(100, totalMs); }
struct Member {
    void* element = nullptr;
    HANDLE node = nullptr;
    float last = 1.f, centerX = 0.f, centerY = 0.f;
    ULONGLONG start = 0;
    bool done = false;
    bool modified = false, centerModified = false;
    bool repaintPending = false;
    bool parentRepaintPending = false;
    int lastAlpha = 255;
    bool alphaModified = false;
    // Ponteiro trivial: o CRT do processo nao deve liberar interfaces de um
    // contexto DUI ja destruido. Lifecycle e OnDestroy descartam no owner.
    CompositionState* composition = nullptr;
    bool compositionTried = false;
    bool settled = false; // curva composta terminada, aguardando a devolucao do grupo
};
struct Group {
    std::atomic<int> state{Free};
    std::atomic<DWORD> owner{0};
    HANDLE ownerHandle = nullptr;
    // Apenas o owner acessa estes campos; o scanner le so state/ownerHandle.
    HANDLE action = nullptr;
    HWND view = nullptr;
    ULONGLONG reveal = 0, acceptUntil = 0;
    ULONGLONG firstBatch = 0, waitUntil = 0;
    ULONGLONG sequenceEnd = 0;
    ULONGLONG deadlineAt = 0;
    int growMs = 1, holdMs = 0, maxDelay = 0, stepMs = 20, count = 0, collectMs = 0;
    int scheduled = 0;
    float fromScale = kAddScale;
    bool frameDriven = false;
    bool useComposition = false, compositionFailureLogged = false;
    bool pageMotion = false;
    POINT pageOffset{};
    AnimationTimeline timeline;
    bool canceling = false;
    bool repairing = false;
    unsigned repairPass = 0;
    // Devolucao natural: cortina levantada uma vez e despromocao a partir de handoffAt (tick).
    bool curtain = false, handoffReady = false, awaitingHandoff = false;
    ULONGLONG handoffAt = 0;
    // Pagina: um item ja concluiu sua pintura nativa sob a ponte preparada.
    bool contentPainted = false;
    unsigned serial = 0;
    uint64_t curtainGeneration = 0;
    Member members[kMembers];
};
static Group groups[16];
static SRWLOCK ownerLock = SRWLOCK_INIT;
static std::atomic<int> busy{0};
static thread_local bool inPaint;
static bool Enabled() {
    AcquireSRWLockShared(&g_sLock);
    bool enabled = g_s.stagger;
    ReleaseSRWLockShared(&g_sLock);
    return enabled;
}
static bool IsItemClass(PCWSTR name) {
    // Classes do UIFILE ExplorerFrame 40960, nao filhos checkbox/syncstatus
    // nem cabecalhos UIGroupItem, que usam item.general.grouped.
    return name && (!wcscmp(name, L"item.icons") ||
        !wcscmp(name, L"item.details") || !wcscmp(name, L"item.smallicons") ||
        !wcscmp(name, L"item.list") || !wcscmp(name, L"item.list.grouped") ||
        !wcscmp(name, L"item.tiles") || !wcscmp(name, L"item.tiles.frequent") ||
        !wcscmp(name, L"item.tiles.recent") || !wcscmp(name, L"item.smart.alpha") ||
        !wcscmp(name, L"item.smart.beta") || !wcscmp(name, L"item.smart.gamma") ||
        !wcscmp(name, L"item.smart.delta"));
}

static void* FindPaintedItem(void* element, int* ancestorDepth) {
    // Um item sem fundo/conteudo proprio pode nao receber Paint. O icone,
    // texto ou checkbox ainda pinta: animar seu ancestral, nunca esse filho.
    for (int depth = 0; element && depth < 8; ++depth) {
        void* value = nullptr;
        PCWSTR name = getClass(element, &value);
        // UIItem na view de miniaturas pode nao ter Class visual. Comparar
        // somente o vptr ABI com a vtable exata resolvida por simbolos: sem
        // offsets de campos privados, sem aceitar UIGroupItem/UIRootItem.
        void* vtable = nullptr;
        memcpy(&vtable, element, sizeof(vtable));
        bool item = IsItemClass(name) || (nativeItemVtable && vtable == nativeItemVtable);
        if (value) releaseValue(value);
        if (item) {
            *ancestorDepth = depth;
            return element;
        }
        element = getParent(element);
    }
    return nullptr;
}

// ownerLock exclusivo, e nenhum callback do owner pode continuar neste slot.
static void ReleaseLocked(Group& group) {
    if (group.ownerHandle) CloseHandle(group.ownerHandle);
    group.ownerHandle = nullptr;
    for (int i = 0; i < group.count; ++i) {
        // Chamado sem Restore apenas depois da morte do contexto/owner:
        // liberar COM, nunca consultar gadgets que ja deixaram de existir.
        DeleteComposition(group.members[i].composition);
        group.members[i].composition = nullptr;
    }
    group.count = 0;
    // O reparo dos pais ja foi pedido: a cortina segura o hold e entao some.
    if (group.curtain) LowerCurtain(group.view, group.curtainGeneration);
    group.curtain = false;
    group.handoffReady = false;
    group.awaitingHandoff = false;
    group.handoffAt = 0;
    group.view = nullptr;
    group.action = nullptr;
    group.owner.store(0, std::memory_order_relaxed);
    group.state.store(Free, std::memory_order_seq_cst);
    busy.fetch_sub(1, std::memory_order_acq_rel);
}
static void Release(Group& group) {
    AcquireSRWLockExclusive(&ownerLock);
    ReleaseLocked(group);
    ReleaseSRWLockExclusive(&ownerLock);
}
static void ReclaimExitedOwners() {
    AcquireSRWLockExclusive(&ownerLock);
    for (Group& group : groups) {
        if (group.state.load(std::memory_order_acquire) == Busy &&
            WaitForSingleObject(group.ownerHandle, 0) == WAIT_OBJECT_0) {
            int expected = Busy;
            if (group.state.compare_exchange_strong(expected, Reserved))
                ReleaseLocked(group);
        }
    }
    ReleaseSRWLockExclusive(&ownerLock);
}
static bool Owned(const Group& group) {
    return group.state.load(std::memory_order_acquire) == Busy &&
        group.owner.load(std::memory_order_relaxed) == GetCurrentThreadId();
}
static void STAGGER_MEMBER DrawFullHook(HANDLE node, NativePaintInfo* info) {
    if (!originalDrawFull) return;
    if (info && !info->drawingLayer && alphaPaintLive.load(std::memory_order_acquire) &&
        Removal::NeedsAlpha(node)) {
        NativePaintInfo local = *info;
        local.useAlpha = TRUE;
        bool previous = inPaint;
        inPaint = true;
        originalDrawFull(node, &local);
        inPaint = previous;
        return;
    }
    if (info && !info->drawingLayer && alphaPaintLive.load(std::memory_order_acquire) &&
        busy.load(std::memory_order_acquire)) {
        for (Group& group : groups) if (Owned(group)) {
            for (int i = 0; i < group.count; ++i) {
                Member& member = group.members[i];
                if (member.node != node || !member.element || member.done || !member.composition ||
                    !member.composition->contentInBridge) continue;
                // Contexto local preserva o caller e propaga alfa ao paint dos
                // filhos. Nao alterar superficie/estilo nem invalidar no paint.
                NativePaintInfo local = *info;
                local.useAlpha = TRUE;
                bool previous = inPaint;
                inPaint = true;
                originalDrawFull(node, &local);
                inPaint = previous;
                return;
            }
        }
    }
    originalDrawFull(node, info);
}
static bool Near(float a, float b) { return std::abs(a - b) < .0001f; }
static bool NativeIdentity(HANDLE node) {
    float x = 0.f, y = 0.f, rotation = 0.f;
    // GetCenterPoint devolve (1,1) quando nao existe XForm; nao (0,0).
    return getScale(node, &x, &y) && Near(x, 1.f) && Near(y, 1.f) &&
        getCenter(node, &x, &y) && Near(x, 1.f) && Near(y, 1.f) &&
        getRotation(node, &rotation) && Near(rotation, 0.f);
}
static bool DetachComposition(Member& member, bool destroying = false) {
    CompositionState* state = member.composition;
    if (!state) return true;
    member.composition = nullptr;
    void* element = member.element;
    HANDLE node = member.node;
    bool okay = true;
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        auto check = [&](HRESULT hr) { okay = SUCCEEDED(hr) && okay; };
        if (state->bridge) check(state->bridge->SetTransform(static_cast<IDCompositionTransform*>(nullptr)));
        if (state->contentInBridge) {
            HRESULT hr = state->bridge->RemoveVisual(state->content.Get());
            check(hr);
            if (SUCCEEDED(hr)) state->contentInBridge = false;
        }
        if (state->contentRemoved && !state->contentInBridge) {
            HRESULT hr = state->parent->AddVisual(state->content.Get(), TRUE,
                state->bridgeAttached ? state->bridge.Get() : nullptr);
            check(hr);
            if (SUCCEEDED(hr)) state->contentRemoved = false;
        }
        if (state->bridgeAttached) {
            HRESULT hr = state->parent->RemoveVisual(state->bridge.Get());
            check(hr);
            if (SUCCEEDED(hr)) state->bridgeAttached = false;
        }
        if (state->device) check(state->device->Commit());
    }
    // OnDestroy vai destruir a referencia junto com o gadget. Nao disparar
    // mudancas de estilo/reentrancia na destruicao nem tocar depois nela.
    if (state->layerRef && !destroying && member.element == element && member.node == node)
        okay = !!releaseLayeredRef(node) && okay;
    member.parentRepaintPending = state->layerRef && !destroying;
    DeleteComposition(state);
    if (!okay) Wh_Log(L"[STAGGER] restauracao da composicao falhou; verifique device/contexto nativo");
    return okay;
}
static double EntranceSlope(double t) {
    constexpr double delta = .0001;
    double lo = std::max(0., t - delta), hi = std::min(1., t + delta);
    return (WinUIEntranceEase(hi) - WinUIEntranceEase(lo)) / (hi - lo);
}
constexpr int kCompositionSegments = 32;
struct CompositionCubic { float p, v, c2, c3; };
static CompositionCubic CompositionSegment(int index, double seconds, float from) {
    double h = seconds / kCompositionSegments;
    double t0 = double(index) / kCompositionSegments, t1 = double(index + 1) / kCompositionSegments;
    double p0 = from + (1. - from) * WinUIEntranceEase(t0);
    double p1 = from + (1. - from) * WinUIEntranceEase(t1);
    double v0 = (1. - from) * EntranceSlope(t0) / seconds;
    double v1 = (1. - from) * EntranceSlope(t1) / seconds;
    return {(float)p0, (float)v0,
        (float)((3. * (p1 - p0) / h - 2. * v0 - v1) / h),
        (float)((2. * (p0 - p1) / h + v0 + v1) / (h * h))};
}
// Hermite por trechos aproxima a spline temporal WinUI, sem trocar a curva
// por uma ease-out diferente. Os coeficientes sao calculados so ao armar.
static HRESULT BuildCompositionCurve(IDCompositionAnimation* curve, double delay, int growMs, float from) {
    HRESULT hr = curve->Reset();
    if (FAILED(hr)) return hr;
    if (delay > 0.) {
        hr = curve->AddCubic(0., from, 0.f, 0.f, 0.f);
        if (FAILED(hr)) return hr;
    }
    double seconds = growMs / 1000., h = seconds / kCompositionSegments;
    for (int i = 0; i < kCompositionSegments; ++i) {
        auto c = CompositionSegment(i, seconds, from);
        hr = curve->AddCubic(delay + i * h, c.p, c.v, c.c2, c.c3);
        if (FAILED(hr)) return hr;
    }
    return curve->End(delay + seconds, 1.f);
}
static bool PrepareComposition(Member& member, float from, float fromOpacity = 1.f,
                               const POINT* pageOffset = nullptr) {
    if (!alphaPaintLive.load(std::memory_order_acquire) || !NativeCompositionAvailable()) return false;
    // Um gadget que exige pintura opaca nao deve perder esse estado nativo.
    if (getGadgetFlags(member.node) & 0x200) return false;
    // Nao inserir uma ponte num gadget ja controlado por uma transicao nativa.
    if (getGadgetStyle(member.node) & 0x80000) return false;
    void* element = member.element;
    HANDLE node = member.node;
    RECT rect{};
    if (!getRect(node, &rect, 8) || rect.right <= rect.left || rect.bottom <= rect.top) return false;
    auto* state = new (std::nothrow) CompositionState;
    if (!state) return false;
    if (!addLayeredRef(node)) { DeleteComposition(state); return false; }
    if (member.element != element || member.node != node) { DeleteComposition(state); return false; }
    state->layerRef = true;
    member.composition = state;
    HRESULT hr = E_FAIL;
    // O getter nativo pode reentrar no DUI: nunca chama-lo sob o mutex COM.
    IUnknown *rawParent = nullptr, *rawContent = nullptr, *rawDevice = nullptr;
    BOOL visual = getGadgetVisual(0, node, reinterpret_cast<void**>(&rawParent),
        reinterpret_cast<void**>(&rawContent), reinterpret_cast<void**>(&rawDevice));
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        Microsoft::WRL::ComPtr<IUnknown> parent, content, device;
        parent.Attach(rawParent); content.Attach(rawContent); device.Attach(rawDevice);
        if (member.composition != state || member.element != element || member.node != node) return false;
        if (visual && parent && content && device) {
            constexpr IID visualIid{0x4d93059d,0x097b,0x4651,{0x9a,0x60,0xf0,0xf2,0x51,0x16,0xe2,0xf3}};
            hr = parent->QueryInterface(visualIid, reinterpret_cast<void**>(state->parent.GetAddressOf()));
            if (SUCCEEDED(hr)) hr = content->QueryInterface(visualIid, reinterpret_cast<void**>(state->content.GetAddressOf()));
            if (SUCCEEDED(hr)) hr = device.As(&state->device);
            if (SUCCEEDED(hr)) hr = state->device->CreateVisual(&state->bridge);
            if (SUCCEEDED(hr)) hr = state->device->CreateScaleTransform(&state->transform);
            if (SUCCEEDED(hr)) hr = state->device->CreateAnimation(&state->curve);
            if (SUCCEEDED(hr)) hr = state->device->CreateAnimation(&state->opacityCurve);
            if (SUCCEEDED(hr)) hr = state->device->CreateEffectGroup(&state->opacity);
            if (SUCCEEDED(hr)) hr = state->opacity->SetOpacity(fromOpacity);
            if (SUCCEEDED(hr)) hr = state->bridge->SetEffect(state->opacity.Get());
            if (SUCCEEDED(hr)) hr = state->transform->SetCenterX((rect.right - rect.left) * .5f);
            if (SUCCEEDED(hr)) hr = state->transform->SetCenterY((rect.bottom - rect.top) * .5f);
            if (SUCCEEDED(hr)) hr = state->transform->SetScaleX(from);
            if (SUCCEEDED(hr)) hr = state->transform->SetScaleY(from);
            if (SUCCEEDED(hr)) hr = state->bridge->SetTransform(state->transform.Get());
            if (SUCCEEDED(hr) && pageOffset) hr = state->bridge->SetOffsetX(float(pageOffset->x));
            if (SUCCEEDED(hr) && pageOffset) hr = state->bridge->SetOffsetY(float(pageOffset->y));
            // A ponte ocupa exatamente o lugar do conteudo na lista do pai.
            // Nenhuma transformacao, clip ou conteudo da Shell e substituido.
            if (SUCCEEDED(hr)) hr = state->parent->AddVisual(state->bridge.Get(), TRUE, state->content.Get());
            if (SUCCEEDED(hr)) state->bridgeAttached = true;
            if (SUCCEEDED(hr)) hr = state->parent->RemoveVisual(state->content.Get());
            if (SUCCEEDED(hr)) state->contentRemoved = true;
            if (SUCCEEDED(hr)) hr = state->bridge->AddVisual(state->content.Get(), TRUE, nullptr);
            if (SUCCEEDED(hr)) state->contentInBridge = true;
            if (SUCCEEDED(hr)) hr = state->device->Commit();
        }
    }
    if (FAILED(hr)) {
        if (!DetachComposition(member)) member.done = true;
        return false;
    }
    return true;
}
static void Restore(Member& member);
static bool ApplyComposition(Member& member, Group& group, double now) {
    if (!member.element || member.done || !member.node) return true;
    if (!member.compositionTried) {
        // Nao promover um lote atrasado que nao cabe mais na acao nativa.
        if (group.timeline.Now() + 16 + group.growMs + 32 > group.deadlineAt) {
            Restore(member);
            return true;
        }
        member.compositionTried = true;
        if (!PrepareComposition(member, group.fromScale, 0.f)) {
            if (!group.compositionFailureLogged) {
                group.compositionFailureLogged = true;
                Wh_Log(L"[STAGGER] composicao indisponivel para um item; usando fallback DirectUI");
            }
            return false;
        }
    }
    CompositionState* state = member.composition;
    if (!state || member.done) return false;
    if (state->curveStart && now >= state->finishAt) {
        // Na escala final a ponte e identica ao conteudo nativo: devolver o
        // grupo inteiro de uma vez, sob uma unica cortina.
        if (group.frameDriven && !group.handoffReady) {
            member.settled = true;
            return true;
        }
        void* element = member.element;
        HANDLE node = member.node;
        DetachComposition(member);
        if (member.element != element || member.node != node) return true;
        invalidate(node);
        member.done = true;
        member.node = nullptr;
        return true;
    }
    if (state->curveStart == member.start) return true;
    // Uma curva que ja iniciou nao e reiniciada pela chegada de outro lote.
    if (state->curveStart && now >= state->curveStart) return true;
    double actualNow = group.timeline.Now();
    member.start = std::max(member.start, (ULONGLONG)std::ceil(actualNow) + 16);
    if (member.start + group.growMs + 32 > group.deadlineAt) {
        Restore(member);
        return true;
    }
    double delay = std::max(0., (double(member.start) - actualNow) / 1000.);
    HRESULT hr;
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        hr = BuildCompositionCurve(state->curve.Get(), delay, group.growMs, group.fromScale);
        // Native linear opacity shares the scale's adapted hold and 333 ms
        // active interval, independently of Entrance's background crossfade.
        if (SUCCEEDED(hr)) hr = state->opacityCurve->Reset();
        if (SUCCEEDED(hr) && delay > 0.) hr = state->opacityCurve->AddCubic(0., 0.f, 0.f, 0.f, 0.f);
        if (SUCCEEDED(hr)) hr = state->opacityCurve->AddCubic(delay, 0.f, 1000.f / group.growMs, 0.f, 0.f);
        if (SUCCEEDED(hr)) hr = state->opacityCurve->End(delay + group.growMs / 1000., 1.f);
        if (SUCCEEDED(hr)) hr = state->opacity->SetOpacity(state->opacityCurve.Get());
        if (SUCCEEDED(hr)) hr = state->transform->SetScaleX(state->curve.Get());
        if (SUCCEEDED(hr)) hr = state->transform->SetScaleY(state->curve.Get());
        if (SUCCEEDED(hr)) hr = state->device->Commit();
    }
    if (FAILED(hr)) {
        if (!DetachComposition(member)) member.done = true;
        Wh_Log(L"[STAGGER] curva de composicao falhou: %08X; fallback DirectUI", (unsigned)hr);
        return false;
    }
    state->curveStart = member.start;
    state->finishAt = member.start + group.growMs + 32;
    member.modified = true;
    // Uma pintura inicial alimenta a superficie. A curva nao solicita paints
    // nem setters por frame; miniaturas/estados continuam atualizados pelo DUI.
    invalidate(member.node);
    if (member.element && member.node) if (void* parent = getParent(member.element)) {
        if (HANDLE parentNode = getNode(parent)) invalidate(parentNode);
    }
    member.repaintPending = false;
    return true;
}
static void Restore(Member& member) {
    void* element = member.element;
    if (!element || member.done || !member.node) return;
    HANDLE node = member.node;
    if (member.composition) {
        DetachComposition(member);
        if (member.element != element || member.node != node) return;
        invalidate(node);
        member.done = true;
        member.repaintPending = false;
        member.node = nullptr;
        return;
    }
    float x = 0.f, y = 0.f;
    // Somente transformacoes inicialmente ausentes sao elegiveis. Cada
    // componente so e restaurado se ainda contiver o ultimo valor do mod.
    if (member.modified && getScale(node, &x, &y) && member.element == element && member.node == node &&
        Near(x, member.last) && Near(y, member.last)) {
        if (!setScale(node, 1.f, 1.f))
            Wh_Log(L"[STAGGER] restauracao de escala falhou: %08X", GetLastError());
    }
    if (member.element != element || !member.node) return;
    if (member.centerModified && getCenter(node, &x, &y) &&
        member.element == element && member.node == node &&
        Near(x, member.centerX) && Near(y, member.centerY)) {
        // Centro zero + escala identidade remove o XForm nativo temporario.
        if (!setCenter(node, 0.f, 0.f))
            Wh_Log(L"[STAGGER] restauracao do centro falhou: %08X", GetLastError());
    }
    if (member.element != element || !member.node) return;
    if (member.alphaModified && getAlpha && setAlpha && getAlpha(element) == member.lastAlpha) {
        if (FAILED(setAlpha(element, 255)))
            Wh_Log(L"[STAGGER] restauracao de opacidade falhou");
    }
    if (member.element != element || !member.node) return;
    invalidate(node);
    member.done = true;
    member.repaintPending = false;
    member.node = nullptr;
}
static bool Apply(Member& member, double now, int growMs, float fromScale) {
    void* element = member.element;
    if (!element || member.done) return false;
    if (inPaint) { member.repaintPending = true; return false; }
    if (!member.centerModified && !NativeIdentity(member.node)) {
        Restore(member);
        return false;
    }
    float x = 0.f, y = 0.f;
    if (!getScale(member.node, &x, &y) || !Near(x, member.last) || !Near(y, member.last)) {
        Restore(member);
        return false;
    }
    if (member.centerModified && (!getCenter(member.node, &x, &y) ||
        !Near(x, member.centerX) || !Near(y, member.centerY))) {
        Restore(member);
        return false;
    }
    if (getAlpha && getAlpha(element) != member.lastAlpha) {
        Restore(member);
        return false;
    }
    if (now >= member.start + growMs) {
        Restore(member);
        return false;
    }
    if (!member.centerModified) {
        RECT rect{};
        // GMA_RECT_LOGICAL (8): dimensoes nao transformadas, locais ao item.
        if (!getRect(member.node, &rect, 8) || rect.right <= rect.left || rect.bottom <= rect.top) {
            Restore(member);
            return false;
        }
        member.centerX = (rect.right - rect.left) * .5f;
        member.centerY = (rect.bottom - rect.top) * .5f;
        member.centerModified = true;
        BOOL centered;
        {
            centered = setCenter(member.node, member.centerX, member.centerY);
        }
        if (!centered) {
            Wh_Log(L"[STAGGER] SetGadgetCenterPoint falhou: %08X", GetLastError());
            Restore(member);
            return false;
        }
        if (member.element != element || !member.node) return false;
    }
    double t = std::clamp((now - double(member.start)) / growMs, 0., 1.);
    if (getAlpha && setAlpha) {
        int alpha = int(std::lround(255. * t));
        if (alpha != member.lastAlpha) {
            int previous = member.lastAlpha;
            member.lastAlpha = alpha;
            member.alphaModified = true;
            HRESULT hr = setAlpha(element, alpha);
            if (member.element != element || !member.node) return false;
            if (FAILED(hr)) {
                member.lastAlpha = previous;
                Restore(member);
                return false;
            }
        }
    }
    float scale;
    {
        scale = fromScale + (1.f - fromScale) * (float)WinUIEntranceEase(t);
    }
    if (Near(scale, member.last)) return false;
    float previous = member.last;
    member.last = scale;
    member.modified = true;
    BOOL scaled;
    {
        scaled = setScale(member.node, scale, scale);
    }
    if (!scaled) {
        Wh_Log(L"[STAGGER] SetGadgetScale falhou: %08X", GetLastError());
        if (member.element == element) { member.last = previous; Restore(member); }
        return false;
    }
    if (member.element == element && member.node) {
        member.repaintPending = false;
        invalidate(member.node);
    }
    return true;
}
static void WINAPI ActionProc(void* info);
static void RepaintParents(Group& group) {
    HANDLE repainted[kMembers]{};
    int count = 0;
    for (int i = 0; i < group.count; ++i) {
        Member& member = group.members[i];
        if (!member.element || !member.parentRepaintPending) continue;
        member.parentRepaintPending = false;
        // OnDestroy conserva este ponteiro somente enquanto o item vive.
        if (void* parent = getParent(member.element)) {
            if (HANDLE node = getNode(parent)) {
                bool duplicate = false;
                for (int j = 0; j < count; ++j) duplicate = duplicate || repainted[j] == node;
                if (!duplicate) { repainted[count++] = node; invalidate(node); }
            }
        }
    }
}
static bool QueueParentRepaint(Group& group) {
    bool pending = false;
    for (int i = 0; i < group.count; ++i)
        pending = pending || (group.members[i].element && group.members[i].parentRepaintPending);
    if (!pending) return false;
    // ReleaseLayeredRef agenda a despromocao. Invalidar no mesmo ciclo ainda
    // pode compor os glifos sobre o backing antigo; deixar um ciclo nativo
    // completar antes de reconstruir o pai, sem repaints por frame.
    group.repairing = true;
    group.canceling = false;
    Action action{sizeof(Action), 0.f, .05f, 0.f, 0, 8u, ActionProc, &group};
    group.action = createAction(&action);
    if (group.action) return true;
    DWORD error = GetLastError();
    group.repairing = false;
    RepaintParents(group);
    Wh_Log(L"[STAGGER] nao foi possivel agendar repintura apos composicao: %08X", error);
    return false;
}
static void Cancel(Group& group) {
    if (!Owned(group) || group.canceling || group.repairing) return;
    group.canceling = true;
    HANDLE action = group.action;
    group.action = nullptr;
    // DeleteHandle pode chamar ActionProc sincronamente; canceling o bloqueia.
    bool deleted = !action || originalDelete(action);
    for (int i = 0; i < group.count; ++i) Restore(group.members[i]);
    if (!deleted) {
        group.action = action;
        group.canceling = false;
        group.acceptUntil = 0;
        group.waitUntil = 0;
        Wh_Log(L"[STAGGER] DeleteHandle falhou; aguardando fim natural");
        return;
    }
    if (QueueParentRepaint(group)) return;
    group.state.store(Reserved, std::memory_order_seq_cst);
    Release(group);
}
static void CancelView(HWND view) {
    Removal::CancelView(view);
    if (!busy.load(std::memory_order_acquire)) return;
    for (Group& group : groups)
        if (Owned(group) && group.view == view) Cancel(group);
}
static void Schedule(Group& group, ULONGLONG now) {
    if (group.scheduled == group.count) return;
    int first = 0;
    // Nunca atrasar um crescimento que ja comecou; so redistribuir a cauda.
    while (first < group.scheduled && group.members[first].start <= now) ++first;
    ULONGLONG begin = first ? std::max(now, group.members[first - 1].start + group.stepMs)
                            : std::max(group.reveal, now + 16) + group.holdMs;
    if (!group.sequenceEnd) group.sequenceEnd = begin + group.maxDelay;
    if (begin > group.sequenceEnd) {
        for (int i = group.scheduled; i < group.count; ++i) {
            Restore(group.members[i]);
        }
    } else {
        int intervals = group.count - first - 1;
        ULONGLONG span = std::min<ULONGLONG>(intervals * group.stepMs, group.sequenceEnd - begin);
        for (int i = first; i < group.count; ++i)
            group.members[i].start = begin + (intervals ? span * (i - first) / intervals : 0);
    }
    group.scheduled = group.count;
}
// Levanta a cortina na primeira chamada; true quando a despromocao pode comecar.
// A copia congelaria outro grupo ainda animando na mesma view (pagina e
// staggering): esperar todos chegarem ao fim e compartilhar uma cortina.
static bool HandoffDue(Group& group) {
    if (group.handoffReady) return true;
    group.awaitingHandoff = true;
    for (const Group& other : groups)
        if (&other != &group && Owned(other) && other.view == group.view &&
            !other.canceling && !other.repairing && !other.awaitingHandoff) return false;
    if (!group.handoffAt) {
        ULONGLONG readyAt = 0;
        group.curtain = RaiseCurtain(group.view, readyAt, group.curtainGeneration);
        group.handoffAt = group.curtain ? readyAt : 1;
    }
    if (GetTickCount64() < group.handoffAt) return false;
    group.handoffReady = true;
    return true;
}
static HRESULT BuildPageCurve(IDCompositionAnimation* curve, double delay, int duration, float offset) {
    HRESULT hr = curve->Reset();
    if (SUCCEEDED(hr) && delay > 0.) hr = curve->AddCubic(0., offset, 0.f, 0.f, 0.f);
    double seconds = duration / 1000., h = seconds / kCompositionSegments;
    for (int i = 0; SUCCEEDED(hr) && i < kCompositionSegments; ++i) {
        auto c = CompositionSegment(i, seconds, 0.f);
        hr = curve->AddCubic(delay + i * h, offset * (1.f - c.p),
            -offset * c.v, -offset * c.c2, -offset * c.c3);
    }
    if (SUCCEEDED(hr)) hr = curve->End(delay + seconds, 0.f);
    return hr;
}
// Clique ou tecla durante a animacao: concluir em pouco tempo, a partir do valor
// atual de cada curva, e devolver pelo caminho natural (cortina com o conteudo
// final). Despromover no meio deixaria a copia da cortina parada no meio-termo.
constexpr int kInterruptFinishMs = 100;
static bool Accelerate(HWND view) {
    if (!busy.load(std::memory_order_acquire) || inPaint || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire)) return false;
    bool accelerated = false;
    for (Group& group : groups) {
        if (!Owned(group) || group.view != view || !group.frameDriven || group.canceling ||
            group.repairing) continue;
        // Ja entregando sob a cortina: deixar terminar em vez de cancelar no meio.
        if (group.awaitingHandoff) { accelerated = true; continue; }
        double now = group.timeline.Now();
        ULONGLONG start = (ULONGLONG)std::ceil(now) + 16;
        double delay = (double(start) - now) / 1000.;
        int previous = std::max(1, group.growMs);
        if (!group.pageMotion && !group.firstBatch) {
            // Nenhum item ainda: sem camadas; o proximo frame cancela o grupo e
            // os itens que chegarem ficam nativos.
            group.waitUntil = std::min<ULONGLONG>(group.waitUntil, (ULONGLONG)now);
            continue;
        }
        group.growMs = std::min(group.growMs, kInterruptFinishMs);
        double seconds = group.growMs / 1000.;
        if (group.pageMotion) {
            CompositionState* state = group.count ? group.members[0].composition : nullptr;
            group.contentPainted = true; // sem curva ainda: o proximo frame a envia ja curta
            if (!state) continue;
            accelerated = true;
            if (!state->curveStart || now >= state->finishAt) continue;
            // A curva atual ja termina antes da curta: mante-la.
            if (start + group.growMs >= state->curveStart + previous) continue;
            float remain = float(1. - WinUIEntranceEase(std::clamp((now - double(state->curveStart)) / previous, 0., 1.)));
            HRESULT hr;
            {
                std::lock_guard<std::mutex> lock(compositionMutex);
                hr = BuildPageCurve(state->pageX.Get(), delay, group.growMs, group.pageOffset.x * remain);
                if (SUCCEEDED(hr)) hr = BuildPageCurve(state->pageY.Get(), delay, group.growMs, group.pageOffset.y * remain);
                if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetX(state->pageX.Get());
                if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetY(state->pageY.Get());
                if (SUCCEEDED(hr)) hr = state->device->Commit();
            }
            if (FAILED(hr)) { Wh_Log(L"[PAGE] aceleracao falhou: %08X", (unsigned)hr); continue; }
            group.members[0].start = start;
            state->curveStart = start;
            state->finishAt = start + group.growMs + 32;
            continue;
        }
        // Itens que chegarem depois ficam nativos; os pendentes comecam agora.
        group.acceptUntil = std::min<ULONGLONG>(group.acceptUntil, (ULONGLONG)now);
        group.scheduled = group.count;
        for (int i = 0; i < group.count; ++i) {
            Member& member = group.members[i];
            if (!member.element || member.done) continue;
            CompositionState* state = member.composition;
            if (!state || !state->curveStart) {
                // So camadas DComp tem a corrida com o GDI; o fallback cancela na hora.
                if (state) accelerated = true;
                if (!state && member.modified && member.start <= now) {
                    // Fallback DirectUI: conservar o progresso ao encurtar a duracao.
                    double t = std::clamp((now - double(member.start)) / previous, 0., 1.);
                    member.start = (ULONGLONG)std::max(0., now - t * group.growMs);
                } else {
                    member.start = start;
                }
                continue;
            }
            if (now >= state->finishAt) continue;
            accelerated = true;
            // A curva atual ja termina antes da curta: mante-la.
            if (start + group.growMs >= state->curveStart + previous) continue;
            double t = std::clamp((now - double(state->curveStart)) / previous, 0., 1.);
            float scale = group.fromScale + (1.f - group.fromScale) * float(WinUIEntranceEase(t));
            float opacity = float(t);
            HRESULT hr;
            {
                std::lock_guard<std::mutex> lock(compositionMutex);
                hr = BuildCompositionCurve(state->curve.Get(), delay, group.growMs, scale);
                if (SUCCEEDED(hr)) hr = state->opacityCurve->Reset();
                if (SUCCEEDED(hr) && delay > 0.) hr = state->opacityCurve->AddCubic(0., opacity, 0.f, 0.f, 0.f);
                if (SUCCEEDED(hr)) hr = state->opacityCurve->AddCubic(delay, opacity, float((1. - opacity) / seconds), 0.f, 0.f);
                if (SUCCEEDED(hr)) hr = state->opacityCurve->End(delay + seconds, 1.f);
                if (SUCCEEDED(hr)) hr = state->opacity->SetOpacity(state->opacityCurve.Get());
                if (SUCCEEDED(hr)) hr = state->transform->SetScaleX(state->curve.Get());
                if (SUCCEEDED(hr)) hr = state->transform->SetScaleY(state->curve.Get());
                if (SUCCEEDED(hr)) hr = state->device->Commit();
            }
            if (FAILED(hr)) { Wh_Log(L"[STAGGER] aceleracao falhou: %08X", (unsigned)hr); continue; }
            member.start = start;
            state->curveStart = start;
            state->finishAt = start + group.growMs + 32;
        }
    }
    return accelerated;
}
static void UpdatePage(Group& group, bool restore, double now) {
    if (!group.count) return;
    Member& member = group.members[0];
    bool finished = member.composition && member.composition->curveStart &&
        now >= member.composition->finishAt;
    if (!restore && finished && group.frameDriven && !HandoffDue(group)) return;
    if (restore || finished) {
        Restore(member);
        return;
    }
    if (!member.element || member.done || inPaint) return;
    if (!member.compositionTried) {
        // Nao iniciar apenas a cauda da curva depois de carregar miniaturas.
        if (now + group.growMs + 64 > group.deadlineAt) { Restore(member); return; }
        member.compositionTried = true;
        // A ponte nasce na posicao inicial: o conteudo que chegar pinta deslocado
        // e parado ate a curva comecar.
        if (!PrepareComposition(member, 1.f, 1.f, &group.pageOffset)) {
            Wh_Log(L"[PAGE] container DirectUI sem composicao elegivel; mantendo pagina estatica");
            Restore(member);
            return;
        }
        member.modified = true;
        member.repaintPending = false;
        invalidate(member.node);
        if (void* parent = getParent(member.element))
            if (HANDLE node = getNode(parent)) invalidate(node);
        if (!group.frameDriven) group.contentPainted = true;
        return;
    }
    auto* state = member.composition;
    if (!state || state->curveStart) return;
    // Em pastas pesadas o container pode pintar muito antes dos itens: mover so
    // depois do primeiro item pintado. Sem itens ate waitUntil (pasta vazia ou
    // lenta), move o que existir, como antes.
    if (!group.contentPainted && now <= group.waitUntil) return;
    if (now + group.growMs + 64 > group.deadlineAt) { Restore(member); return; }
    double actualNow = group.timeline.Now();
    member.start = std::max(group.reveal, (ULONGLONG)std::ceil(actualNow) + 16);
    double delay = std::max(0., (double(member.start) - actualNow) / 1000.);
    HRESULT hr;
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        hr = state->device->CreateAnimation(&state->pageX);
        if (SUCCEEDED(hr)) hr = state->device->CreateAnimation(&state->pageY);
        if (SUCCEEDED(hr)) hr = BuildPageCurve(state->pageX.Get(), delay, group.growMs, float(group.pageOffset.x));
        if (SUCCEEDED(hr)) hr = BuildPageCurve(state->pageY.Get(), delay, group.growMs, float(group.pageOffset.y));
        if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetX(state->pageX.Get());
        if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetY(state->pageY.Get());
        if (SUCCEEDED(hr)) hr = state->device->Commit();
    }
    if (FAILED(hr)) {
        Wh_Log(L"[PAGE] curva DirectUI falhou: %08X", (unsigned)hr);
        Restore(member);
        return;
    }
    state->curveStart = member.start;
    state->finishAt = member.start + group.growMs + 32;
}
static void UpdateGroup(Group* group, bool finished, double frameTime = -1.) {
    bool restore = finished || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || (!group->pageMotion && !Enabled());
    if (restore) group->acceptUntil = 0;
    double now = frameTime >= 0. ? frameTime : group->timeline.Now();
    if (group->pageMotion) { UpdatePage(*group, restore, now); return; }
    if (!restore && !inPaint) Schedule(*group, (ULONGLONG)now);
    for (int i = 0; i < group->count; ++i) {
        if (restore) Restore(group->members[i]);
        else if (!group->useComposition || inPaint ||
                 !ApplyComposition(group->members[i], *group, now))
            Apply(group->members[i], now, group->growMs, group->fromScale);
        Member& member = group->members[i];
        if (!restore && member.element && member.node && member.repaintPending) {
            member.repaintPending = false;
            invalidate(member.node);
        }
    }
}
static void FrameView(HWND view, double frameTime = -1.) {
    if (inPaint || !busy.load(std::memory_order_acquire)) return;
    bool cancel = stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire);
    for (Group& group : groups)
        if (Owned(group) && group.frameDriven && !group.canceling && !group.repairing && group.view == view) {
            if (cancel || (!group.pageMotion && !Enabled())) { Cancel(group); continue; }
            double now = frameTime >= 0. ? frameTime : group.timeline.Now();
            if (group.pageMotion) {
                UpdateGroup(&group, false, frameTime);
                if (now >= group.deadlineAt || (!group.count && now > group.waitUntil) ||
                    (group.count && (!group.members[0].element || group.members[0].done))) Cancel(group);
                continue;
            }
            ULONGLONG hardEnd = group.deadlineAt;
            double firstEnd = std::max(double(group.reveal), now + 16.) + group.holdMs + group.maxDelay + group.growMs;
            if (now >= hardEnd || (!group.scheduled && group.count && firstEnd > hardEnd)) {
                Cancel(group);
                continue;
            }
            UpdateGroup(&group, false, frameTime);
            bool settled = group.firstBatch && now > group.acceptUntil, composed = false;
            for (int i = 0; settled && i < group.count; ++i) {
                const Member& member = group.members[i];
                settled = !member.element || member.done || member.settled;
                composed = composed || (member.element && !member.done && member.settled);
            }
            if (settled && composed && HandoffDue(group)) UpdateGroup(&group, false, frameTime);
            bool done = group.firstBatch && now > group.acceptUntil;
            for (int i = 0; done && i < group.count; ++i)
                done = !group.members[i].element || group.members[i].done;
            // O cancelamento ocorre no owner, fora do callback da acao nativa.
            if (done) Cancel(group);
            else if (!group.firstBatch && now > group.waitUntil) Cancel(group);
        }
}
static bool WaitingForItems(HWND view, double now) {
    if (!busy.load(std::memory_order_acquire) || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || !Enabled()) return false;
    for (Group& group : groups)
        if (Owned(group) && !group.pageMotion && group.frameDriven && !group.canceling && !group.repairing && group.view == view) {
            if (!group.firstBatch) return now <= group.waitUntil;
            for (int i = 0; i < group.count; ++i) {
                const Member& member = group.members[i];
                if (member.element && !member.done && !member.modified) return true;
            }
            return false;
        }
    return false;
}
static bool ActiveView(HWND view) {
    if (!busy.load(std::memory_order_acquire)) return false;
    for (Group& group : groups)
        if (Owned(group) && !group.canceling && !group.repairing && group.view == view) return true;
    return false;
}
static void RebasePending(HWND view, ULONGLONG reveal) {
    if (!busy.load(std::memory_order_acquire)) return;
    for (Group& group : groups)
        if (Owned(group) && group.frameDriven && !group.canceling && !group.repairing &&
            group.view == view && !group.pageMotion && !group.scheduled) {
            if (reveal > group.waitUntil) { Cancel(group); continue; }
            group.reveal = reveal;
            if (group.firstBatch) {
                group.firstBatch = std::max(group.firstBatch, reveal);
                group.acceptUntil = group.firstBatch + group.collectMs;
            }
            // waitUntil e o limite absoluto armado com a acao: nao prolongar
            // indefinidamente uma pasta vazia nem exceder o lifetime nativo.
        }
}
static void WINAPI ActionProc(void* info) {
    BYTE* bytes = static_cast<BYTE*>(info);
    Group* group = *reinterpret_cast<Group**>(bytes + sizeof(void*));
    bool finished = *reinterpret_cast<BOOL*>(bytes +
        (sizeof(void*) == 8 ? 0x20 : 0x18)) != FALSE;
    if (!group || !Owned(*group) || group->canceling) return;
    if (group->repairing) {
        if (group->repairPass++) RepaintParents(*group);
        if (finished) {
            group->action = nullptr;
            // UI ocupada pode entregar so o callback final. Reagendar conserva
            // um ciclo entre a despromocao e a primeira repintura do pai.
            if (QueueParentRepaint(*group)) return;
            group->state.store(Reserved, std::memory_order_seq_cst);
            Release(*group);
        }
        return;
    }
    // O worker da view dirige os frames no owner. A acao DUI conserva o
    // lifetime/contexto; fora desse caminho, tambem fornece os frames.
    if (!group->frameDriven || finished || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || (!group->pageMotion && !Enabled()))
        UpdateGroup(group, finished);
    if (finished) {
        // O HACTION deixa de existir depois deste callback natural.
        group->action = nullptr;
        if (QueueParentRepaint(*group)) return;
        group->state.store(Reserved, std::memory_order_seq_cst);
        Release(*group);
    }
}
static void ForgetElement(void* element) {
    Removal::Destroy(element);
    if (busy.load(std::memory_order_acquire)) {
        for (Group& group : groups) if (Owned(group))
            for (int i = 0; i < group.count; ++i)
                if (group.members[i].element == element) {
                    DetachComposition(group.members[i], true);
                    group.members[i] = {};
                }
    }
}
static void STAGGER_MEMBER DestroyHook(void* element) {
    ForgetElement(element);
    originalDestroy(element);
}
static void AfterDeleteHandle() {
    if (!busy.load(std::memory_order_acquire) && !Removal::Active()) return;
    bool owned = false;
    for (Group& group : groups) owned = owned || Owned(group);
    if (!owned && !Removal::Active()) return;
    DWORD error = GetLastError();
    SetLastError(ERROR_SUCCESS);
    createAction(nullptr);
    bool gone = GetLastError() == 0x80040014; // DU_E_NOCONTEXT
    if (gone) {
        Removal::ContextGone();
        for (Group& group : groups) if (Owned(group)) {
            // O contexto ja destruiu os gadgets: nao tocar neles nem HACTION.
            group.state.store(Reserved, std::memory_order_seq_cst);
            Release(group);
        }
    }
    SetLastError(error);
}
static BOOL WINAPI DeleteHook(HANDLE handle) {
    BOOL result = originalDelete(handle);
    AfterDeleteHandle();
    return result;
}
static bool OnPagePaint(void* element) {
    if (!live.load(std::memory_order_acquire) || !busy.load(std::memory_order_acquire) ||
        stopping.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire)) return true;
    bool interested = false;
    for (Group& group : groups)
        interested = interested || (Owned(group) && group.pageMotion && !group.canceling && !group.repairing &&
            (!group.count || (group.members[0].element && !group.members[0].modified && !group.members[0].done)));
    if (!interested) return true;
    // Filho comum abaixo do HWNDElement, nunca a raiz/background do host.
    // Tambem funciona para pastas vazias, sem depender de um UIItem pintado.
    void* container = element;
    for (int depth = 0; container && depth < 64; ++depth) {
        void* parent = getParent(container);
        if (!parent || !getParent(parent)) break;
        container = parent;
    }
    if (!container || !getParent(container)) return true;
    HWND hwnd = nullptr;
    void* ancestor = container;
    for (int depth = 0; ancestor && !hwnd && depth < 64; ++depth) {
        hwnd = getHwnd(ancestor);
        ancestor = getParent(ancestor);
    }
    for (Group& group : groups) if (Owned(group) && group.pageMotion && !group.canceling &&
        !group.repairing && (hwnd == group.view || IsChild(group.view, hwnd))) {
        if (group.count) {
            auto& member = group.members[0];
            return member.element != container || member.modified || member.done;
        }
        if (group.timeline.Now() > group.waitUntil) return true;
        HANDLE node = getNode(container);
        if (!node || !NativeIdentity(node) || getAlpha(container) != 255) return true;
        auto& member = group.members[0];
        member = {};
        member.element = container;
        member.node = node;
        member.repaintPending = true;
        group.count = 1;
        // Somente registrar/gatear: promover e submeter curvas no proximo
        // frame do owner, nunca dentro do callback de pintura.
        return false;
    }
    return true;
}
static bool OnPaint(void* element) {
    if (!live.load(std::memory_order_acquire) || !busy.load(std::memory_order_acquire) ||
        stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || !Enabled()) return true;
    bool owned = false;
    for (Group& group : groups) if (Owned(group) && !group.pageMotion && !group.canceling && !group.repairing) {
        owned = true;
    }
    if (!owned) return true;
    int ancestorDepth = 0;
    void* item = FindPaintedItem(element, &ancestorDepth);
    if (!item) return true;
    element = item;
    HWND hwnd = nullptr;
    void* ancestor = element;
    for (int depth = 0; ancestor && !hwnd && depth < 64; ++depth) {
        hwnd = getHwnd(ancestor);
        ancestor = getParent(ancestor);
    }
    Group* target = nullptr;
    for (Group& group : groups) if (Owned(group) && !group.pageMotion && !group.canceling && !group.repairing &&
        (hwnd == group.view || IsChild(group.view, hwnd))) {
        target = &group;
        break;
    }
    if (!target) return true;
    Removal::Observe(element, target->view);
    ULONGLONG now = (ULONGLONG)target->timeline.Now();
    for (int i = 0; i < target->count; ++i)
        if (target->members[i].element == element) {
            bool ready = target->members[i].modified || target->members[i].done;
            return ready;
        }
    ULONGLONG acceptUntil = target->frameDriven && !target->firstBatch
        ? target->waitUntil : target->acceptUntil;
    if (now > acceptUntil) {
        return true;
    }
    if (target->count >= kMembers) {
        return true;
    }
    HANDLE node = getNode(element);
    // Nao disputar transformacoes preexistentes da Shell ou de outro mod.
    if (!node || !NativeIdentity(node) || (getAlpha && getAlpha(element) != 255)) return true;
    if (target->frameDriven && !target->firstBatch) {
        target->firstBatch = std::max(target->reveal, now);
        target->acceptUntil = target->firstBatch + target->collectMs;
        target->reveal = target->firstBatch;
    }
    Member& member = target->members[target->count++];
    member = {};
    member.element = element;
    member.node = node;
    // O primeiro tick distribui os inicios pelo lote completo e prepara a
    // escala fora do paint. Ate la, nem o item nem seus filhos sao pintados.
    member.repaintPending = true;
    return false;
}
// Texto "Esta pasta esta vazia": id EmptyTextFocusable do UIFILE 40960 do
// ExplorerFrame. O atomo local e o mesmo devolvido por Element::GetID, mas e
// recriado com outro valor quando a ultima view que o usa fecha: sem cache.
static void* EmptyFolderText(void* element) {
    if (!getId) return nullptr;
    ATOM id = FindAtomW(L"EmptyTextFocusable");
    if (!id) return nullptr;
    for (int depth = 0; element && depth < 3; ++depth, element = getParent(element))
        if (getId(element) == id) return element;
    return nullptr;
}
// Pagina preparada aguardando conteudo: o elemento e um item (ou filho dele),
// ou o texto de pasta vazia, dentro do seu container. Avaliado antes do
// callback, que pode destrui-lo.
static Group* PageAwaitingContent(void* element) {
    if (!busy.load(std::memory_order_acquire)) return nullptr;
    void* item = nullptr;
    bool searched = false;
    for (Group& group : groups) {
        if (!Owned(group) || !group.pageMotion || group.canceling || group.repairing ||
            group.contentPainted || !group.count) continue;
        const Member& page = group.members[0];
        if (!page.element || page.done || !page.composition || page.composition->curveStart) continue;
        if (!searched) {
            int depth = 0;
            item = FindPaintedItem(element, &depth);
            if (!item) item = EmptyFolderText(element);
            searched = true;
        }
        void* ancestor = item;
        for (int depth = 0; ancestor && depth < 64; ++depth, ancestor = getParent(ancestor))
            if (ancestor == page.element) return &group;
    }
    return nullptr;
}
static HRESULT WINAPI DisplayHook(HANDLE node, void* element, void* event) {
    // EventMsg: cbSize e mensagem sao DWORDs nas duas arquiteturas.
    // 0x8002 despacha o Paint virtual; 0x40001 e o resultado handled nativo.
    struct Header { DWORD size, message; } header{};
    if (event) memcpy(&header, event, sizeof(header));
    if (!element || header.size < sizeof(header) || header.message != 0x8002)
        return originalDisplay(node, element, event);
    bool previous = inPaint;
    inPaint = true;
    Removal::ObservePaint(element);
    Group* waiting = PageAwaitingContent(element);
    unsigned serial = waiting ? waiting->serial : 0;
    bool ready = OnPagePaint(element) && OnPaint(element);
    HRESULT result = ready ? originalDisplay(node, element, event) : HRESULT(0x40001);
    // Paint bloqueado pelo staggering ou com falha nao libera o movimento.
    if (waiting && ready && SUCCEEDED(result) && Owned(*waiting) && waiting->serial == serial)
        waiting->contentPainted = true;
    inPaint = previous;
    return result;
}
static FARPROC Export(HMODULE module, const char* x64, const char* x86) {
    return GetProcAddress(module, sizeof(void*) == 8 ? x64 : x86);
}
namespace Removal {
// IDefItem is queried, never obtained by subtracting an object offset. The
// private IID and 8-byte ITEMKEY were checked against CDefItem's QITAB and
// GetItemData. The x64 Shell notification path requires the exact symbols
// and capture APIs; unavailable dependencies keep entrance unaffected.
struct Key { DWORD slot, generation; };
static_assert(sizeof(Key) == 8);
struct DefItem : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE SetItemData(const Key*, IUnknown*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetItemData(Key*, IUnknown**) = 0;
};
constexpr IID defItemIid{0xb2334792,0x88cf,0x479c,{0xad,0x3b,0xe2,0xf3,0x0e,0x64,0x83,0x94}};
constexpr IID unknownIid{0,0,0,{0xc0,0,0,0,0,0,0,0x46}};
using ItemFn = HRESULT(STAGGER_MEMBER*)(void*, IUnknown**);
using EnumFn = HRESULT(STAGGER_MEMBER*)(void*, HRESULT(*)(void*, void*), void*);
using ViewFn = void*(*)(void*);
using BoundsFn = HRESULT(STAGGER_MEMBER*)(void*, RECT*);
using DeletedFn = void(STAGGER_MEMBER*)(void*, const Key*, ULONG);
using DetachFn = BOOL(WINAPI*)(UINT, HANDLE, DWORD*, IUnknown**, IUnknown**, IUnknown**, IUnknown**);
using ReleaseDetachedFn = BOOL(WINAPI*)(UINT, DWORD);
static ItemFn getItem;
static EnumFn enumItems;
static ViewFn getView;
static BoundsFn getBounds;
static DeletedFn originalDeleted;
static DetachFn detachVisuals;
static ReleaseDetachedFn releaseDetached;
static HMODULE storageModule;
static std::atomic<bool> ready{false};
static bool NativeDeletionAvailable() {
#ifdef _WIN64
    return NativeCompositionAvailable() && getItem && enumItems && getView &&
        getBounds && detachVisuals && releaseDetached && nativeItemVtable && originalDeleted;
#else
    return false;
#endif
}
struct Witness {
    DWORD owner = 0;
    HANDLE ownerHandle = nullptr;
    HWND view = nullptr;
    void* itemsView = nullptr;
};
static Witness witnesses[16]; // ownerLock protects reservation and reclamation.
struct Surface {
    Microsoft::WRL::ComPtr<IDCompositionDevice> device;
    Microsoft::WRL::ComPtr<IDCompositionVisual> placement, parent, content;
    Microsoft::WRL::ComPtr<IDCompositionEffectGroup> opacity;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> opacityCurve;
    DWORD ticket = ~0u;
    bool attached = false;
};
namespace Scene {
static bool Active();
static int Views(HWND* out,int max);
static bool NeedsAlpha(HANDLE node);
static void CancelView(HWND view);
static void Destroy(void* element);
static void ContextGone();
static void Reclaim();
}
static bool NeedsAlpha(HANDLE node) {return Scene::NeedsAlpha(node);}
static double DeleteEase(double t) {
    if (t <= 0.) return 0.;
    if (t >= 1.) return 1.;
    double lo = 0., hi = 1.;
    for (int i = 0; i < 24; ++i) {
        double u = (lo + hi) * .5, v = 1. - u;
        if (.33*v*v*u + .72*v*u*u + u*u*u < t) lo = u; else hi = u;
    }
    double u = (lo + hi) * .5, v = 1. - u;
    return 1.5*v*v*u + 2.88*v*u*u + u*u*u;
}
static HRESULT BuildScale(IDCompositionAnimation* curve, int ms) {
    HRESULT hr = curve->Reset();
    double seconds = ms / 1000., h = seconds / kCompositionSegments;
    auto value = [](double t) { return 1. - .1 * DeleteEase(t); };
    auto slope = [&](double t) {
        double a = std::max(0., t-.0001), b = std::min(1., t+.0001);
        return (value(b)-value(a))/(b-a)/seconds;
    };
    for (int i = 0; SUCCEEDED(hr) && i < kCompositionSegments; ++i) {
        double a = double(i)/kCompositionSegments, b = double(i+1)/kCompositionSegments;
        double p = value(a), q = value(b), v = slope(a), w = slope(b);
        hr = curve->AddCubic(i*h, (float)p, (float)v,
            (float)((3*(q-p)/h-2*v-w)/h), (float)((2*(p-q)/h+v+w)/(h*h)));
    }
    return FAILED(hr) ? hr : curve->End(seconds, kAddScale);
}
namespace Scene {
struct Item {
    Member member;
    Key key{};
    void* collection=nullptr; // identity token only; never dereferenced or released off-owner
    bool selected=false, started=false;
};
struct Data {
    HWND view=nullptr, hwnd=nullptr;
    SIZE viewport{};
    UINT_PTR subclassId=0;
    bool subclassed=false, inputRemoveFailed=false;
    void* parent=nullptr;
    HANDLE parentNode=nullptr, action=nullptr;
    Member root;
    Surface snapshot;
    Item items[kMembers];
    int count=0, durationMs=100;
    bool preparing=true, detached=false, canceled=false, canceling=false, contextDead=false;
    bool rootRef=false, retiring=false, backingReleased=false;
    bool curtain=false, curtainTried=false, coverCancel=false;
    uint64_t curtainGeneration=0;
    unsigned repairPass=0;
    unsigned nativeDepth=0;
    ULONGLONG created=0, swapAt=0, hiddenAt=0;
};
struct Slot {
    std::atomic<int> state{Free};
    std::atomic<DWORD> owner{0};
    HANDLE ownerHandle=nullptr;
    Data* data=nullptr; // trivial static storage; explicit owner/context cleanup
};
static Slot slots[16];
static std::atomic<unsigned> active{0};
static bool Owned(const Slot& s) {return s.state.load(std::memory_order_acquire)==Busy&&s.owner==GetCurrentThreadId();}
static bool Active() {
    // Include reservations: unload must not pass a slot still allocating its
    // owner data before the second stopping check below.
    for(auto& s:slots)if(s.state.load(std::memory_order_seq_cst)!=Free)return true;
    return false;
}
static bool NeedsAlpha(HANDLE node) {
    if(!active.load(std::memory_order_acquire)||!node)return false;
    for(auto& s:slots)if(Owned(s)&&s.data){
        auto& d=*s.data;
        if(d.root.element&&(d.rootRef||(d.root.composition&&d.root.composition->layerRef))&&d.root.node==node&&!(getGadgetFlags(node)&0x200))return true;
        if(!d.detached)for(int i=0;i<d.count;++i){auto& m=d.items[i].member;
            if(m.element&&m.node==node&&m.composition&&m.composition->layerRef)return true;
        }
    }
    return false;
}
static Slot* Find(HWND view) {for(auto& s:slots)if(Owned(s)&&s.data&&s.data->view==view)return &s;return nullptr;}
static LRESULT CALLBACK InputSubclass(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
static bool RemoveInput(Data& d) {
    if(!d.subclassed)return true;
    if(d.hwnd&&!RemoveWindowSubclass(d.hwnd,InputSubclass,d.subclassId)&&IsWindow(d.hwnd)){
        if(!d.inputRemoveFailed)Wh_Log(L"[STAGGER] falha ao remover protecao de entrada hwnd=%p",d.hwnd);
        d.inputRemoveFailed=true;return false;
    }
    d.subclassed=false;
    return true;
}
static void FreeSlot(Slot& s,bool ownerLockHeld=false) {
    // Native tickets/gadgets are owned by a dead context on this path, or
    // have already been retired by the owner. Only COM/handles remain.
    if(s.data&&s.owner==GetCurrentThreadId()&&WaitForSingleObject(s.ownerHandle,0)==WAIT_TIMEOUT&&!RemoveInput(*s.data)){
        s.data->contextDead=true;return; // Keep callback userdata alive until removal succeeds.
    }
    if(!ownerLockHeld)AcquireSRWLockExclusive(&ownerLock);
    if(s.data&&s.data->curtain){LowerCurtain(s.data->view,s.data->curtainGeneration);s.data->curtain=false;}
    {std::lock_guard<std::mutex> lock(compositionMutex);
        if(s.data){
            delete s.data->root.composition;
            for(int i=0;i<s.data->count;++i)delete s.data->items[i].member.composition;
            delete s.data;
        }
    }
    s.data=nullptr;
    if(s.ownerHandle)CloseHandle(s.ownerHandle);
    s.ownerHandle=nullptr;s.state.store(Free,std::memory_order_release);active.fetch_sub(1,std::memory_order_acq_rel);
    if(!ownerLockHeld)ReleaseSRWLockExclusive(&ownerLock);
}
static HRESULT SwapAt(Data& d,int delayMs) {
    // Two complementary step curves: native layout is never suspended.
    auto* live=d.root.composition;
    if(!live||!live->opacity)return E_FAIL;
    auto& snap=d.snapshot;
    HRESULT hr=live->opacityCurve->Reset();
    if(SUCCEEDED(hr))hr=snap.opacityCurve->Reset();
    if(SUCCEEDED(hr))hr=live->opacityCurve->AddCubic(0.,0.f,0.f,0.f,0.f);
    if(SUCCEEDED(hr))hr=live->opacityCurve->End(delayMs/1000.,1.f);
    if(SUCCEEDED(hr))hr=snap.opacityCurve->AddCubic(0.,1.f,0.f,0.f,0.f);
    if(SUCCEEDED(hr))hr=snap.opacityCurve->End(delayMs/1000.,0.f);
    if(SUCCEEDED(hr))hr=live->opacity->SetOpacity(live->opacityCurve.Get());
    if(SUCCEEDED(hr))hr=snap.opacity->SetOpacity(snap.opacityCurve.Get());
    if(SUCCEEDED(hr))hr=snap.device->Commit();
    if(SUCCEEDED(hr))d.swapAt=GetTickCount64()+delayMs;
    return hr;
}
static void Retire(Data& d) {
    if(d.retiring)return;
    d.retiring=true;
    RemoveInput(d);
    HRESULT hr=S_OK;
    {std::lock_guard<std::mutex> lock(compositionMutex);
        if(d.root.composition&&d.root.composition->opacity)hr=d.root.composition->opacity->SetOpacity(1.f);
        if(d.snapshot.opacity){HRESULT h=d.snapshot.opacity->SetOpacity(0.f);if(FAILED(h))hr=h;}
        if(d.snapshot.attached){HRESULT h=d.snapshot.parent->RemoveVisual(d.snapshot.placement.Get());
            if(SUCCEEDED(h))d.snapshot.attached=false;else hr=h;}
        if(d.snapshot.device){HRESULT h=d.snapshot.device->Commit();if(FAILED(h))hr=h;}
    }
    if(FAILED(hr))Wh_Log(L"[STAGGER] falha ao revelar layout apos remocao: %08X",(unsigned)hr);
    d.hiddenAt=GetTickCount64();
}
static LRESULT CALLBACK InputSubclass(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR,DWORD_PTR ref) {
    auto* slot=reinterpret_cast<Slot*>(ref);
    if(slot&&Owned(*slot)&&slot->data&&slot->data->hwnd==hwnd){auto& d=*slot->data;
        if(d.contextDead){FreeSlot(*slot);return DefSubclassProc(hwnd,msg,wp,lp);}
        bool resize=msg==WM_SIZE&&(LOWORD(lp)!=d.viewport.cx||HIWORD(lp)!=d.viewport.cy);
        // Movimento/hover nao cancelam: a reorganizacao do layout tambem gera
        // WM_MOUSEMOVE, e a copia retida vive so ~100 ms apos a exclusao.
        bool input=msg==WM_LBUTTONDOWN||msg==WM_LBUTTONDBLCLK||msg==WM_RBUTTONDOWN||
            msg==WM_MBUTTONDOWN||msg==WM_XBUTTONDOWN||msg==WM_MOUSEWHEEL||msg==WM_MOUSEHWHEEL||
            msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN||msg==WM_HSCROLL||msg==WM_VSCROLL;
        if(input||resize||msg==WM_NCDESTROY||(msg==WM_SHOWWINDOW&&!wp)){
            // Clique e tecla nao movem o conteudo: a devolucao ao GDI fica sob a cortina.
            d.coverCancel=input&&msg!=WM_MOUSEWHEEL&&msg!=WM_MOUSEHWHEEL&&msg!=WM_HSCROLL&&msg!=WM_VSCROLL;
            d.canceled=true;
            if(!d.preparing&&!inPaint)Retire(d);
            if(msg==WM_NCDESTROY){RemoveInput(d);d.hwnd=nullptr;}
        }
    }
    return DefSubclassProc(hwnd,msg,wp,lp);
}
static void ReleaseBacking(Data& d) {
    ++d.nativeDepth;
    if(d.snapshot.ticket!=~0u){
        if(!releaseDetached(0,d.snapshot.ticket))Wh_Log(L"[STAGGER] falha ao liberar layout de saida: %08X",GetLastError());
        d.snapshot.ticket=~0u;
    }
    if(d.root.composition&&!d.contextDead)DetachComposition(d.root,!d.root.element);
    if(!d.contextDead&&d.rootRef&&d.root.element&&d.root.node)releaseLayeredRef(d.root.node);
    d.rootRef=false;
    d.backingReleased=true;
    --d.nativeDepth;
}
static void Repair(Data& d) {
    if(d.parent&&d.parentNode)invalidate(d.parentNode);
    else if(d.root.element&&d.root.node)invalidate(d.root.node);
}
// O Explorer reaproveita o elemento do item excluido para outro item muito antes
// de destrui-lo (ate segundos depois). Outra chave na mesma colecao confirma a
// exclusao; falha na consulta nao confirma nada.
static bool Rebound(const Item& item) {
    void* element=item.member.element;
    if(!element||!item.collection)return false;
    Microsoft::WRL::ComPtr<IUnknown> native,collection,identity;
    Microsoft::WRL::ComPtr<DefItem> def;
    Key current{};
    if(FAILED(getItem(element,native.GetAddressOf()))||!native||
        FAILED(native->QueryInterface(defItemIid,reinterpret_cast<void**>(def.GetAddressOf())))||
        FAILED(def->GetItemData(&current,collection.GetAddressOf()))||!collection||FAILED(collection.As(&identity)))
        return false;
    return identity.Get()==item.collection&&(current.slot!=item.key.slot||current.generation!=item.key.generation);
}
static bool Start(Data& d,Item& item);
static void WINAPI Tick(void* info) {
    auto* bytes=static_cast<BYTE*>(info);
    auto* s=*reinterpret_cast<Slot**>(bytes+sizeof(void*));
    bool final=*reinterpret_cast<BOOL*>(bytes+(sizeof(void*)==8?0x20:0x18))!=FALSE;
    if(!s||!Owned(*s)||!s->data)return;
    auto& d=*s->data;
    if(d.canceling)return;
    if(d.preparing){if(final)d.action=nullptr;return;}
    if(d.contextDead){FreeSlot(*s);return;}
    if(d.detached&&!d.retiring&&!d.canceled&&!final)
        for(int i=0;i<d.count;++i){auto& item=d.items[i];
            if(item.selected&&!item.started&&Rebound(item))Start(d,item);
        }
    if(d.canceled||stopping.load()||g_unloading.load()||!Enabled()||GetTickCount64()>=d.swapAt+32||final)Retire(d);
    // Fim natural ou interrupcao por clique/tecla: cobrir a devolucao do container ao GDI.
    if(d.retiring&&!d.backingReleased&&!d.curtainTried&&!stopping.load()&&!g_unloading.load()&&Enabled()&&
        (d.coverCancel||(!d.canceled&&GetTickCount64()>=d.swapAt))){
        d.curtainTried=true;ULONGLONG ready=0;
        if((d.curtain=RaiseCurtain(d.view,ready,d.curtainGeneration)))d.hiddenAt=std::max(d.hiddenAt,ready-32);
    }
    if(d.retiring&&!d.backingReleased&&GetTickCount64()>=d.hiddenAt+32)ReleaseBacking(d);
    if(d.contextDead){FreeSlot(*s);return;}
    if(d.retiring&&d.backingReleased&&d.repairPass++==1)Repair(d);
    if(!final&&d.retiring&&d.backingReleased&&d.repairPass>=2){
        HANDLE action=d.action;d.canceling=true;
        bool deleted=!action||originalDelete(action);
        if(!Owned(*s)||s->data!=&d)return;
        d.canceling=false;
        if(deleted){d.action=nullptr;FreeSlot(*s);return;}
    }
    if(final){
        d.action=nullptr;
        if(!d.backingReleased||d.repairPass<2){
            Action repair{sizeof(Action),0.f,.1f,0.f,0,8,Tick,s};d.action=createAction(&repair);
            if(d.action)return;
        }
        if(!d.backingReleased)ReleaseBacking(d);
        if(d.repairPass<2)Repair(d);
        FreeSlot(*s);
    }
}
static void CancelView(HWND view) {
    for(auto& s:slots)if(Owned(s)&&s.data&&s.data->view==view){
        // Rolagem/navegacao: sem cortina, mesmo depois de clique ou tecla.
        s.data->coverCancel=false;
        s.data->canceled=true;
        if(!s.data->preparing&&!inPaint)Retire(*s.data);
    }
}
static int Views(HWND* out,int max) {
    int n=0;
    for(auto& s:slots)if(Owned(s)&&s.data&&s.data->view&&n<max){
        bool known=false;for(int i=0;i<n;++i)known=known||out[i]==s.data->view;
        if(!known)out[n++]=s.data->view;
    }
    return n;
}
static void ContextGone() {
    for(auto& s:slots)if(Owned(s)&&s.data){
        if(s.data->preparing||s.data->nativeDepth){
            s.data->contextDead=true;
            s.data->canceled=true;
        }else{
            FreeSlot(s);
        }
    }
}
static void Reclaim() {
    AcquireSRWLockExclusive(&ownerLock);
    for(auto& s:slots)if(s.state.load(std::memory_order_acquire)==Busy&&WaitForSingleObject(s.ownerHandle,0)==WAIT_OBJECT_0)FreeSlot(s,true);
    ReleaseSRWLockExclusive(&ownerLock);
}
static bool Start(Data& d,Item& item) {
    auto* c=item.member.composition;
    BOOL animations=TRUE;SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animations,0);
    if(!c||d.preparing||!d.detached||d.retiring||d.canceled||inPaint||!animations||!Enabled()||
        stopping.load()||g_unloading.load()||GetTickCount64()>=d.swapAt||GetTickCount64()>d.created+350)return false;
    HRESULT hr;
    {std::lock_guard<std::mutex> lock(compositionMutex);
        hr=BuildScale(c->curve.Get(),d.durationMs);
        if(SUCCEEDED(hr))hr=c->opacityCurve->Reset();
        if(SUCCEEDED(hr))hr=c->opacityCurve->AddCubic(0.,1.f,-1000.f/d.durationMs,0.f,0.f);
        if(SUCCEEDED(hr))hr=c->opacityCurve->End(d.durationMs/1000.,0.f);
        if(SUCCEEDED(hr))hr=c->transform->SetScaleX(c->curve.Get());
        if(SUCCEEDED(hr))hr=c->transform->SetScaleY(c->curve.Get());
        if(SUCCEEDED(hr))hr=c->opacity->SetOpacity(c->opacityCurve.Get());
        if(SUCCEEDED(hr))hr=SwapAt(d,d.durationMs);
    }
    if(FAILED(hr)){Wh_Log(L"[STAGGER] curva da saida retida falhou: %08X",(unsigned)hr);d.canceled=true;return false;}
    item.started=true;return true;
}
static void Destroy(void* element) {
    for(auto& s:slots)if(Owned(s)&&s.data){auto& d=*s.data;
        if(d.root.element==element){
            if(d.root.composition)DetachComposition(d.root,true);
            d.root.element=nullptr;d.root.node=nullptr;d.rootRef=false;d.canceled=true;
        }
        if(d.parent==element){d.parent=nullptr;d.parentNode=nullptr;d.canceled=true;}
        for(int i=0;i<d.count;++i){auto& item=d.items[i];auto& m=item.member;
            if(m.element!=element)continue;
            if(!d.detached){if(m.composition)DetachComposition(m,true);}
            else if(item.selected&&!item.started)Start(d,item);
            m.element=nullptr;m.node=nullptr;
        }
    }
}
static HRESULT Collect(void* element,void* context) {
    auto& d=*static_cast<Data*>(context);
    if(d.canceled||d.count>=kMembers||!element)return S_OK;
    void* vtable=nullptr;memcpy(&vtable,element,sizeof(vtable));
    if(!nativeItemVtable||vtable!=nativeItemVtable)return S_OK;
    RECT bounds{},client{},intersection{};
    if(FAILED(getBounds(element,&bounds))||IsRectEmpty(&bounds)||!GetClientRect(d.hwnd,&client)||!IntersectRect(&intersection,&bounds,&client))return S_OK;
    auto& item=d.items[d.count++];item.member.element=element;item.member.node=getNode(element);
    Microsoft::WRL::ComPtr<IUnknown> native,source;
    Microsoft::WRL::ComPtr<DefItem> def;
    if(SUCCEEDED(getItem(element,native.GetAddressOf()))&&native&&SUCCEEDED(native->QueryInterface(defItemIid,reinterpret_cast<void**>(def.GetAddressOf())))&&
        SUCCEEDED(def->GetItemData(&item.key,source.GetAddressOf()))&&source){
        Microsoft::WRL::ComPtr<IUnknown> identity;source.As(&identity);item.collection=identity.Get();
    }
    return S_OK;
}
static Slot* Begin(HWND view,void* nativeView,IUnknown* source=nullptr,const Key* key=nullptr) {
    if(Slot* existing=Find(view))return existing;
    if(inPaint||stopping.load(std::memory_order_seq_cst)||g_unloading.load()||!Enabled()||
        !originalDelete||!alphaPaintLive.load()||!enumItems||!getBounds)return nullptr;
    Slot* slot=nullptr;
    AcquireSRWLockExclusive(&ownerLock);
    for(auto& s:slots){int expected=Free;if(s.state.compare_exchange_strong(expected,Reserved)){slot=&s;break;}}
    if(slot){slot->ownerHandle=OpenThread(SYNCHRONIZE,FALSE,GetCurrentThreadId());
        if(!slot->ownerHandle){slot->state.store(Free);slot=nullptr;}}
    ReleaseSRWLockExclusive(&ownerLock);
    if(!slot)return nullptr;
    auto* data=new(std::nothrow) Data;
    if(!data){CloseHandle(slot->ownerHandle);slot->ownerHandle=nullptr;slot->state.store(Free);return nullptr;}
    slot->data=data;slot->owner=GetCurrentThreadId();active.fetch_add(1);slot->state.store(Busy,std::memory_order_seq_cst);
    if(stopping.load(std::memory_order_seq_cst)||g_unloading.load()||!Enabled()){FreeSlot(*slot);return nullptr;}
    auto& d=*data;d.view=view;d.hwnd=getHwnd(nativeView);d.created=GetTickCount64();d.swapAt=d.created+250;
    AcquireSRWLockShared(&g_sLock);d.durationMs=DeleteDuration(g_s.duration);ReleaseSRWLockShared(&g_sLock);
    bool okay=d.hwnd&&IsWindowVisible(d.hwnd)&&GetWindowThreadProcessId(d.hwnd,nullptr)==GetCurrentThreadId()&&
        nativeView;
    if(okay)okay=SUCCEEDED(enumItems(nativeView,Collect,&d))&&d.count>0&&!d.canceled;
    if(okay&&source&&key){
        bool matched=false;
        for(int i=0;i<d.count;++i){auto& item=d.items[i];
            item.selected=item.collection==source&&item.key.slot==key->slot&&item.key.generation==key->generation;
            matched=matched||item.selected;
        }
        // Offscreen/unrelated changes must not snapshot or hold this view.
        if(!matched){FreeSlot(*slot);return nullptr;}
    }
    // UIItemsView can itself be the HWND root. Never detach that root: its
    // backing/background and target attachment belong to the host. Freeze the
    // common child containing all realized items/sections, not the HWND backing.
    void* container=okay?d.items[0].member.element:nullptr;
    for(int depth=0;container&&depth<64;++depth){void* p=getParent(container);if(!p||!getParent(p))break;container=p;}
    d.root.element=container;d.root.node=container?getNode(container):nullptr;
    d.parent=container?getParent(container):nullptr;d.parentNode=d.parent?getNode(d.parent):nullptr;
    okay=okay&&d.parentNode&&d.root.node&&getAlpha(container)==255&&NativeIdentity(d.root.node)&&
        !(getGadgetStyle(d.root.node)&0x80000)&&!(getGadgetFlags(d.root.node)&0x200);
    for(int i=0;okay&&i<d.count;++i){
        void* ancestor=d.items[i].member.element;
        for(int depth=0;ancestor&&ancestor!=container&&depth<64;++depth)ancestor=getParent(ancestor);
        okay=ancestor==container;
    }
    if(!okay){FreeSlot(*slot);return nullptr;}
    Action action{sizeof(Action),0.f,(d.durationMs+1000)/1000.f,0.f,0,8,Tick,slot};
    if(!d.contextDead)d.action=createAction(&action);okay=okay&&d.action;
    if(okay){
        RECT client{};okay=!!GetClientRect(d.hwnd,&client);
        d.viewport={client.right-client.left,client.bottom-client.top};d.subclassId=reinterpret_cast<UINT_PTR>(slot);
        if(okay){d.subclassed=!!SetWindowSubclass(d.hwnd,InputSubclass,d.subclassId,reinterpret_cast<DWORD_PTR>(slot));okay=d.subclassed;}
    }
    // The snapshot is a sibling of live content INSIDE its own buffer. Never
    // promote the host root merely to obtain an attachment point for the copy.
    if(okay){d.rootRef=!!addLayeredRef(d.root.node);okay=d.rootRef;}
    for(int i=0;okay&&i<d.count;++i){auto& m=d.items[i].member;
        if(m.element&&m.node&&getAlpha(m.element)==255&&NativeIdentity(m.node))PrepareComposition(m,1.f);
        okay=!d.canceled&&!d.contextDead;
    }
    if(okay&&source){bool eligible=false;
        for(int i=0;i<d.count;++i)eligible=eligible||(d.items[i].selected&&d.items[i].member.composition);
        okay=eligible;
    }
    if(okay){
        invalidate(d.root.node);if(d.parentNode)invalidate(d.parentNode);
        RedrawWindow(d.hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_UPDATENOW);
        okay=!d.canceled&&!d.contextDead&&d.root.element&&!stopping.load()&&!g_unloading.load()&&Enabled();
    }
    if(okay){
        Microsoft::WRL::ComPtr<IUnknown> p,c,dev,b,co,pl,device;
        if(okay)okay=!!detachVisuals(0,d.root.node,&d.snapshot.ticket,b.GetAddressOf(),co.GetAddressOf(),pl.GetAddressOf(),device.GetAddressOf());
        if(okay){
            d.detached=true;
            for(int i=0;i<d.count;++i){auto& m=d.items[i].member;
                if(d.contextDead||!d.root.element)break;
                if(m.composition&&m.composition->layerRef){m.composition->layerRef=false;
                    if(m.element&&m.node)releaseLayeredRef(m.node);}
                m.node=nullptr;
            }
            if(d.canceled||d.contextDead||!d.root.element)okay=false;
            if(okay){
                auto* live=new(std::nothrow) CompositionState;
                d.root.composition=live;
                if(live){live->layerRef=true;d.rootRef=false;}
                HRESULT hr=live?S_OK:E_OUTOFMEMORY;
                if(SUCCEEDED(hr)&&!getGadgetVisual(0,d.root.node,reinterpret_cast<void**>(p.GetAddressOf()),reinterpret_cast<void**>(c.GetAddressOf()),reinterpret_cast<void**>(dev.GetAddressOf())))hr=E_FAIL;
                if(d.canceled||d.contextDead||!d.root.element)hr=E_ABORT;
                {
                    std::lock_guard<std::mutex> lock(compositionMutex);
                    constexpr IID vi{0x4d93059d,0x097b,0x4651,{0x9a,0x60,0xf0,0xf2,0x51,0x16,0xe2,0xf3}};
                    Microsoft::WRL::ComPtr<IDCompositionVisual> oldBuffer;
                    if(SUCCEEDED(hr))hr=b->QueryInterface(vi,reinterpret_cast<void**>(oldBuffer.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=co->QueryInterface(vi,reinterpret_cast<void**>(d.snapshot.content.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=device.As(&d.snapshot.device);
                    if(SUCCEEDED(hr))hr=p->QueryInterface(vi,reinterpret_cast<void**>(live->parent.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=c->QueryInterface(vi,reinterpret_cast<void**>(live->content.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=dev.As(&live->device);
                    if(SUCCEEDED(hr))d.snapshot.parent=live->parent;
                    if(SUCCEEDED(hr))hr=live->device->CreateVisual(&live->bridge);
                    if(SUCCEEDED(hr))hr=live->device->CreateEffectGroup(&live->opacity);
                    if(SUCCEEDED(hr))hr=live->device->CreateAnimation(&live->opacityCurve);
                    if(SUCCEEDED(hr))hr=live->opacity->SetOpacity(0.f);
                    if(SUCCEEDED(hr))hr=live->bridge->SetEffect(live->opacity.Get());
                    if(SUCCEEDED(hr))hr=live->parent->AddVisual(live->bridge.Get(),TRUE,live->content.Get());
                    if(SUCCEEDED(hr))live->bridgeAttached=true;
                    if(SUCCEEDED(hr))hr=live->parent->RemoveVisual(live->content.Get());
                    if(SUCCEEDED(hr))live->contentRemoved=true;
                    if(SUCCEEDED(hr))hr=live->bridge->AddVisual(live->content.Get(),TRUE,nullptr);
                    if(SUCCEEDED(hr))live->contentInBridge=true;
                    if(SUCCEEDED(hr))hr=live->device->CreateVisual(&d.snapshot.placement);
                    if(SUCCEEDED(hr))hr=live->device->CreateEffectGroup(&d.snapshot.opacity);
                    if(SUCCEEDED(hr))hr=live->device->CreateAnimation(&d.snapshot.opacityCurve);
                    if(SUCCEEDED(hr))hr=d.snapshot.placement->SetEffect(d.snapshot.opacity.Get());
                    // Old content is already in container-local coordinates.
                    // Its native placement/clip stays on the live container.
                    if(SUCCEEDED(hr))hr=oldBuffer->RemoveVisual(d.snapshot.content.Get());
                    if(SUCCEEDED(hr))hr=d.snapshot.placement->AddVisual(d.snapshot.content.Get(),TRUE,nullptr);
                    if(SUCCEEDED(hr))hr=d.snapshot.parent->AddVisual(d.snapshot.placement.Get(),TRUE,nullptr);
                    if(SUCCEEDED(hr))d.snapshot.attached=true;
                    if(SUCCEEDED(hr))hr=SwapAt(d,250);
                    okay=SUCCEEDED(hr);
                }
            }
        }
        // Native gadget APIs run on the owner, outside our COM lock. Dispose
        // their returned references explicitly under the common DComp lock.
        {std::lock_guard<std::mutex> lock(compositionMutex);
            p.Reset();c.Reset();dev.Reset();b.Reset();co.Reset();pl.Reset();device.Reset();
        }
    }
    // O detach entrega superficies novas ao container: repintar tudo enquanto a
    // ponte viva esta oculta, nao apenas os filhos que mudaram de posicao.
    if(okay&&d.detached&&!d.contextDead&&d.root.element&&d.root.node)invalidate(d.root.node);
    d.preparing=false;
    if(d.contextDead){FreeSlot(*slot);return nullptr;}
    d.created=GetTickCount64();
    if(okay&&!d.action){d.action=createAction(&action);okay=d.action!=nullptr;}
    if(!okay){
        if(!d.detached)for(int i=0;i<d.count;++i)if(d.items[i].member.composition)DetachComposition(d.items[i].member);
        d.canceled=true;Retire(d);
        if(!d.action){ReleaseBacking(d);Repair(d);FreeSlot(*slot);}
        return nullptr;
    }
    return slot;
}
static void Mark(Slot& slot,IUnknown* source,const Key& key) {
    if(!Owned(slot)||!slot.data)return;
    auto& d=*slot.data;
    if(d.retiring||d.canceled||GetTickCount64()>=d.swapAt||GetTickCount64()>d.created+350)return;
    for(int i=0;i<d.count;++i){auto& item=d.items[i];
        if(item.collection!=source||item.key.slot!=key.slot||item.key.generation!=key.generation||!item.member.element)continue;
        // UIItem recycling can change its binding without destroying the
        // element. Recheck the current identity before selecting the old copy.
        Microsoft::WRL::ComPtr<IUnknown> native,collection,identity;
        Microsoft::WRL::ComPtr<DefItem> def;
        Key current{};
        if(SUCCEEDED(getItem(item.member.element,native.GetAddressOf()))&&native&&
            SUCCEEDED(native->QueryInterface(defItemIid,reinterpret_cast<void**>(def.GetAddressOf())))&&
            SUCCEEDED(def->GetItemData(&current,collection.GetAddressOf()))&&collection&&
            SUCCEEDED(collection.As(&identity))&&identity.Get()==source&&current.slot==key.slot&&current.generation==key.generation)
            item.selected=true;
    }
}
} // namespace Scene
static void CancelView(HWND view) {Scene::CancelView(view);}
static int Views(HWND* out,int max) {return Scene::Views(out,max);}
static bool Active() {return Scene::Active();}
static void Reclaim() {
    Scene::Reclaim();
    AcquireSRWLockExclusive(&ownerLock);
    for(Witness& w:witnesses)if(w.ownerHandle&&(WaitForSingleObject(w.ownerHandle,0)==WAIT_OBJECT_0||
        !IsWindow(w.view)||GetWindowThreadProcessId(w.view,nullptr)!=w.owner)){
        CloseHandle(w.ownerHandle);w={};
    }
    ReleaseSRWLockExclusive(&ownerLock);
}
static void ContextGone() {
    Scene::ContextGone();
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses)if(w.owner==GetCurrentThreadId())w.itemsView=nullptr;
    ReleaseSRWLockShared(&ownerLock);
}
static void Bind(HWND view) {
    if (!ready.load(std::memory_order_acquire)) return;
    CancelView(view);
    Reclaim();
    AcquireSRWLockExclusive(&ownerLock);
    Witness* slot = nullptr;
    for (Witness& w : witnesses) if (w.owner == GetCurrentThreadId() && w.view == view) { slot=&w; break; }
    if (!slot) for (Witness& w : witnesses) if (!w.ownerHandle) { slot=&w; break; }
    if (slot) {
        if (!slot->ownerHandle) slot->ownerHandle = OpenThread(SYNCHRONIZE,FALSE,GetCurrentThreadId());
        if (slot->ownerHandle) {
            slot->owner=GetCurrentThreadId();slot->view=view;slot->itemsView=nullptr;
        }
    }
    ReleaseSRWLockExclusive(&ownerLock);
}
static void Observe(void* item, HWND view) {
    if (!ready.load(std::memory_order_acquire)) return;
    AcquireSRWLockShared(&ownerLock);
    for (Witness& w : witnesses) if (w.owner == GetCurrentThreadId() && w.view == view && !w.itemsView) {
        w.itemsView = getView(item);
        break;
    }
    ReleaseSRWLockShared(&ownerLock);
}
static void ObservePaint(void* element) {
    if (!ready.load(std::memory_order_acquire) || !Enabled() || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire)) return;
    bool needsBinding=false;
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses) if(w.owner==GetCurrentThreadId()&&!w.itemsView) needsBinding=true;
    ReleaseSRWLockShared(&ownerLock);
    if (!needsBinding) return;
    int depth=0;
    void* item=FindPaintedItem(element,&depth);
    void* nativeView=item?getView(item):nullptr;
    HWND hwnd=nativeView?getHwnd(nativeView):nullptr;
    if (!hwnd) return;
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses) if(w.owner==GetCurrentThreadId()&&!w.itemsView&&
        (hwnd==w.view||IsChild(w.view,hwnd))) w.itemsView=nativeView;
    ReleaseSRWLockShared(&ownerLock);
}
static void Destroy(void* element) {
    Scene::Destroy(element);
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses)if(w.owner==GetCurrentThreadId()&&w.itemsView==element)w.itemsView=nullptr;
    ReleaseSRWLockShared(&ownerLock);
}
static void STAGGER_MEMBER Deleted(void* collection, const Key* key, ULONG flags) {
    if (ready.load(std::memory_order_acquire) && key && !inPaint && Enabled() &&
        !stopping.load(std::memory_order_acquire) && !g_unloading.load(std::memory_order_acquire)) {
        // The notification is informational. Never wait for an owner thread
        // or slow a file operation to manufacture an exit animation.
        BOOL animations=TRUE;
        SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animations,0);
        Microsoft::WRL::ComPtr<IUnknown> identity;
        if (animations && SUCCEEDED(static_cast<IUnknown*>(collection)->QueryInterface(unknownIid,
                reinterpret_cast<void**>(identity.GetAddressOf())))) {
            Witness local[ARRAYSIZE(witnesses)]{};
            int count=0;
            AcquireSRWLockShared(&ownerLock);
            for (Witness& w : witnesses) if (w.owner==GetCurrentThreadId() && w.itemsView) local[count++]=w;
            ReleaseSRWLockShared(&ownerLock);
            for (int i=0;i<count;++i) {
                Witness& w=local[i];
                // Re-read the owner binding after each potentially reentrant
                // capture. A navigation/destruction invalidates old witnesses.
                bool valid=false;
                AcquireSRWLockShared(&ownerLock);
                for (Witness& current:witnesses) if(current.owner==w.owner && current.view==w.view &&
                    current.itemsView==w.itemsView) valid=true;
                ReleaseSRWLockShared(&ownerLock);
                bool groupBusy=false;
                for(Group& group:groups) if(ItemStagger::Owned(group)&&group.view==w.view) groupBusy=true;
                if (!valid || !IsWindow(w.view) || GetWindowThreadProcessId(w.view,nullptr)!=w.owner || groupBusy) continue;
                if(auto* scene=Scene::Begin(w.view,w.itemsView,identity.Get(),key))Scene::Mark(*scene,identity.Get(),*key);
            }
        }
    }
    originalDeleted(collection,key,flags);
}
static bool Stage(HMODULE, HMODULE shell) {
    getBounds=reinterpret_cast<BoundsFn>(Export(duiModule,
        "?GetRootRelativeBounds@Element@DirectUI@@QEAAJPEAUtagRECT@@@Z",
        "?GetRootRelativeBounds@Element@DirectUI@@QAEJPAUtagRECT@@@Z"));
    detachVisuals=reinterpret_cast<DetachFn>(GetProcAddress(duserModule,"DetachGadgetVisuals"));
    releaseDetached=reinterpret_cast<ReleaseDetachedFn>(GetProcAddress(duserModule,"ReleaseDetachedObjects"));
#ifdef _WIN64
    if (!GetModuleHandleExW(0,L"windows.storage.dll",&storageModule)) {
        Wh_Log(L"[DELETE] windows.storage.dll indisponivel: %08X; mantendo remocao nativa", GetLastError());
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK explorerframeDllHooks[]={
        {{L"public: virtual long __cdecl UIItem::GetItem(struct IItem * *)"},&getItem,nullptr,true},
        {{L"public: long __cdecl UIItemsView::EnumUIItems(long (__cdecl*)(class UIItem *,void *),void *)"},&enumItems,nullptr,true},
        {{L"class UIItemsView * __cdecl GetUIItemsView(class DirectUI::Element *)"},&getView,nullptr,true},
    };
    bool views=shell && WindhawkUtils::HookSymbols(shell,explorerframeDllHooks,ARRAYSIZE(explorerframeDllHooks)) && getItem && enumItems && getView;
    if (!views || !getBounds || !detachVisuals || !releaseDetached || !nativeItemVtable) {
        Wh_Log(L"[DELETE] dependencias indisponiveis: views=%d item=%d enum=%d view=%d bounds=%d detach=%d release=%d type=%d; mantendo remocao nativa",
            views, getItem != nullptr, enumItems != nullptr, getView != nullptr, getBounds != nullptr,
            detachVisuals != nullptr, releaseDetached != nullptr, nativeItemVtable != nullptr);
        return false;
    }
    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK hooks[]={
        {{L"private: void __cdecl CDefCollection::_OnDeleteViewItem(struct tagITEMKEY const *,unsigned long)"},&originalDeleted,Deleted,true},
    };
    bool hooked=WindhawkUtils::HookSymbols(storageModule,hooks,ARRAYSIZE(hooks)) && originalDeleted;
    if (!hooked)
        Wh_Log(L"[DELETE] CDefCollection::_OnDeleteViewItem indisponivel ou hook recusado; mantendo remocao nativa");
    return hooked && NativeDeletionAvailable();
#else
    return false;
#endif
}
static void Enable(bool value) {ready.store(value && NativeDeletionAvailable(),std::memory_order_release);}
static void Uninit() {
    ready.store(false,std::memory_order_release);
    AcquireSRWLockExclusive(&ownerLock);
    for (Witness& w : witnesses) {if(w.ownerHandle)CloseHandle(w.ownerHandle);w={};}
    ReleaseSRWLockExclusive(&ownerLock);
    if (storageModule) {FreeLibrary(storageModule);storageModule=nullptr;}
}
} // namespace Removal
// Views desta thread com camadas do mod (grupos ou cena de exclusao).
static int ScrollViews(HWND* out, int max) {
    int n = Removal::Views(out, max);
    if (!busy.load(std::memory_order_acquire)) return n;
    for (Group& group : groups) {
        if (!Owned(group) || !group.view || n >= max) continue;
        bool known = false;
        for (int i = 0; i < n; ++i) known = known || out[i] == group.view;
        if (!known) out[n++] = group.view;
    }
    return n;
}
// O Explorer sincroniza a barra com o conteudo a cada passo, venha a rolagem da
// roda, da barra, do teclado ou de UI Automation (Smooth Scroll): sinal unico.
using PositionChangedFn = void(STAGGER_MEMBER*)(void*, void*);
static PositionChangedFn originalPositionChanged;
static void STAGGER_MEMBER PositionChangedHook(void* bar, void* value) {
    if (!inPaint) HandleContentScroll();
    originalPositionChanged(bar, value);
}
static bool EnsureHooks() {
    if (live.load(std::memory_order_acquire)) return true;
    std::unique_lock<std::mutex> lock(setupMutex, std::try_to_lock);
    if (!lock.owns_lock()) return false;
    if (live.load(std::memory_order_acquire)) return true;
    if (setupAttempted.load(std::memory_order_acquire) || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire)) return false;
    if (!GetModuleHandleW(L"dui70.dll") || !GetModuleHandleW(L"duser.dll")) {
        return false;
    }
    setupAttempted.store(true, std::memory_order_release);
    if (!GetModuleHandleExW(0, L"dui70.dll", &duiModule) ||
        !GetModuleHandleExW(0, L"duser.dll", &duserModule)) return false;
#define STAGGER_EXPORT(type, x64, x86) reinterpret_cast<type>(Export(duiModule, x64, x86))
    getClass = STAGGER_EXPORT(GetClassFn, "?GetClass@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z", "?GetClass@Element@DirectUI@@QAEPBGPAPAVValue@2@@Z");
    releaseValue = STAGGER_EXPORT(ReleaseFn, "?Release@Value@DirectUI@@QEAAXXZ", "?Release@Value@DirectUI@@QAEXXZ");
    getNode = STAGGER_EXPORT(NodeFn, "?GetDisplayNode@Element@DirectUI@@QEAAPEAUHGADGET__@@XZ", "?GetDisplayNode@Element@DirectUI@@QAEPAUHGADGET__@@XZ");
    getParent = STAGGER_EXPORT(ParentFn, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ", "?GetParent@Element@DirectUI@@QAEPAV12@XZ");
    getAlpha = STAGGER_EXPORT(AlphaGetFn, "?GetAlpha@Element@DirectUI@@QEAAHXZ", "?GetAlpha@Element@DirectUI@@QAEHXZ");
    setAlpha = STAGGER_EXPORT(AlphaSetFn, "?SetAlpha@Element@DirectUI@@QEAAJH@Z", "?SetAlpha@Element@DirectUI@@QAEJH@Z");
    getId = STAGGER_EXPORT(IdFn, "?GetID@Element@DirectUI@@QEAAGXZ", "?GetID@Element@DirectUI@@QAEGXZ");
    // Opcional: builds/modos antigos ainda usam as classes do UIFILE.
    if (GetModuleHandleExW(0, L"ExplorerFrame.dll", &itemModule)) {
        WindhawkUtils::SYMBOL_HOOK explorerframeDllHooks[] = {
            {{L"const UIItem::`vftable'"}, &nativeItemVtable, nullptr, true},
        };
        WindhawkUtils::HookSymbols(itemModule, explorerframeDllHooks, ARRAYSIZE(explorerframeDllHooks));
        if (!nativeItemVtable)
            Wh_Log(L"[STAGGER] tipo UIItem indisponivel; usando apenas classes visuais");
    }
    // Este helper e simbolo privado, nao export. Ele verifica a classe antes
    // de consultar o HWND; um UIItem comum nao possui HWND proprio.
    if (!getHwnd) {
        WindhawkUtils::SYMBOL_HOOK dui70_dll_hooks[] = {
#ifdef _WIN64
            {{L"struct HWND__ * __cdecl DirectUI::GetHwndFromElement(class DirectUI::Element *)"}, &getHwnd, nullptr, true},
#else
            {{L"struct HWND__ * __stdcall DirectUI::GetHwndFromElement(class DirectUI::Element *)"}, &getHwnd, nullptr, true},
#endif
        };
        WindhawkUtils::HookSymbols(duiModule, dui70_dll_hooks, ARRAYSIZE(dui70_dll_hooks));
    }
    auto display = STAGGER_EXPORT(DisplayFn, "?_DisplayNodeCallback@Element@DirectUI@@SAJPEAUHGADGET__@@PEAXPEAUEventMsg@@@Z", "?_DisplayNodeCallback@Element@DirectUI@@SGJPAUHGADGET__@@PAXPAUEventMsg@@@Z");
    auto destroy = STAGGER_EXPORT(DestroyFn, "?OnDestroy@Element@DirectUI@@UEAAXXZ", "?OnDestroy@Element@DirectUI@@UAEXXZ");
#undef STAGGER_EXPORT
    createAction = reinterpret_cast<CreateActionFn>(GetProcAddress(duserModule, "CreateAction"));
    invalidate = reinterpret_cast<InvalidateFn>(GetProcAddress(duserModule, "InvalidateGadget"));
    getGadgetStyle = reinterpret_cast<GadgetStyleFn>(GetProcAddress(duserModule, "GetGadgetStyle"));
    getGadgetFlags = reinterpret_cast<GadgetStyleFn>(GetProcAddress(duserModule, "GetGadgetFlags"));
    addLayeredRef = reinterpret_cast<LayerRefFn>(GetProcAddress(duserModule, "AddLayeredRef"));
    releaseLayeredRef = reinterpret_cast<LayerRefFn>(GetProcAddress(duserModule, "ReleaseLayeredRef"));
    getGadgetVisual = reinterpret_cast<GadgetVisualFn>(GetProcAddress(duserModule, "GetGadgetVisual"));
    getScale = reinterpret_cast<GetPairFn>(GetProcAddress(duserModule, "GetGadgetScale"));
    setScale = reinterpret_cast<SetPairFn>(GetProcAddress(duserModule, "SetGadgetScale"));
    getCenter = reinterpret_cast<GetPairFn>(GetProcAddress(duserModule, "GetGadgetCenterPoint"));
    setCenter = reinterpret_cast<SetPairFn>(GetProcAddress(duserModule, "SetGadgetCenterPoint"));
    getRotation = reinterpret_cast<RotationFn>(GetProcAddress(duserModule, "GetGadgetRotation"));
    getRect = reinterpret_cast<RectFn>(GetProcAddress(duserModule, "GetGadgetRect"));
    auto deleteHandle = reinterpret_cast<DeleteFn>(GetProcAddress(duserModule, "DeleteHandle"));
    if (!getClass || !releaseValue || !getNode || !getParent || !getHwnd || !getAlpha || !setAlpha ||
        !getScale || !setScale || !getCenter || !setCenter || !getRotation || !getRect ||
        !createAction || !invalidate || !deleteHandle || !display || !destroy) {
        Wh_Log(L"[STAGGER] exports DUI indisponiveis; mantendo entrance normal");
        Wh_Log(L"[STAGGER] APIs host=%d display=%d destroy=%d action=%d delete=%d escala=%d centro=%d geometria=%d alpha=%d",
                getHwnd != nullptr, display != nullptr, destroy != nullptr, createAction != nullptr,
                deleteHandle != nullptr, getScale && setScale, getCenter && setCenter, getRotation && getRect, getAlpha && setAlpha);
        return false;
    }
    // Gates ficam fechados se um hook falhar, mesmo com staging parcial.
    if (stopping.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire))
        return false;
    bool ok = WindhawkUtils::SetFunctionHook(destroy, DestroyHook, &originalDestroy);
    ok = WindhawkUtils::SetFunctionHook(deleteHandle, DeleteHook, &originalDelete) && ok;
    ok = WindhawkUtils::SetFunctionHook(display, DisplayHook, &originalDisplay) && ok;
    // Resolver a assinatura exata tambem em DLLs atualizadas. Timestamp e
    // checksum da build nao determinam disponibilidade desta capacidade.
    WindhawkUtils::SYMBOL_HOOK duserDllHooks[] = {
#ifdef _WIN64
        {{L"private: void __cdecl DuVisual::xrDrawFull(struct PaintInfo *)"}, &originalDrawFull, DrawFullHook, true},
#else
        {{L"private: void __thiscall DuVisual::xrDrawFull(struct PaintInfo *)"}, &originalDrawFull, DrawFullHook, true},
#endif
    };
    bool alphaStaged = WindhawkUtils::HookSymbols(duserModule, duserDllHooks, ARRAYSIZE(duserDllHooks)) && originalDrawFull;
    if (!alphaStaged)
        Wh_Log(L"[STAGGER] DuVisual::xrDrawFull indisponivel ou hook recusado; usando fallback DirectUI");
    else if (!NativeCompositionAvailable())
        Wh_Log(L"[STAGGER] APIs de composicao nativa indisponiveis: style=%d flags=%d add=%d release=%d visual=%d",
            getGadgetStyle != nullptr, getGadgetFlags != nullptr, addLayeredRef != nullptr,
            releaseLayeredRef != nullptr, getGadgetVisual != nullptr);
    bool removalStaged = alphaStaged && NativeCompositionAvailable() && Removal::Stage(duiModule,itemModule);
    // Opcional: sem ele, so a entrada na propria janela retira as camadas.
    auto positionChanged = reinterpret_cast<PositionChangedFn>(Export(duiModule,
        "?OnPositionChanged@BaseScrollBar@DirectUI@@QEAAXPEAVValue@2@@Z",
        "?OnPositionChanged@BaseScrollBar@DirectUI@@QAEXPAVValue@2@@Z"));
    if (!positionChanged ||
        !WindhawkUtils::SetFunctionHook(positionChanged, PositionChangedHook, &originalPositionChanged))
        Wh_Log(L"[STAGGER] BaseScrollBar::OnPositionChanged indisponivel; rolagem pela barra mantem as camadas");
    bool applied = Wh_ApplyHookOperations();
    if (!applied) ok = false;
    alphaPaintLive.store(ok && applied && alphaStaged && NativeCompositionAvailable(), std::memory_order_release);
    Removal::Enable(ok && applied && removalStaged);
    live.store(ok, std::memory_order_release);
    if (!ok) Wh_Log(L"[STAGGER] hooks indisponiveis; mantendo entrance normal");
    return ok;
}
static void RequestPreparation() {
    if (!preparationRunning.load(std::memory_order_acquire) ||
        live.load(std::memory_order_acquire) || preparationFailed.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire)) return;
    {
        std::lock_guard<std::mutex> lock(preparationMutex);
        if (preparationStop) return;
        preparationRequested = true;
    }
    preparationCv.notify_one();
}
static void StartPreparation(bool (*prepare)() = EnsureHooks) {
    if (preparationThread || g_unloading.load(std::memory_order_acquire)) return;
    std::lock_guard<std::mutex> lock(preparationMutex);
    preparationStop = false;
    preparationRequested = true;
    preparationDone = false;
    try {
        preparationThread = new std::thread([prepare] {
            std::unique_lock<std::mutex> lock(preparationMutex);
            for (;;) {
                preparationCv.wait(lock, [] { return preparationStop || preparationRequested; });
                if (preparationStop) break;
                preparationRequested = false;
                lock.unlock();
                if (!g_unloading.load(std::memory_order_acquire) &&
                    !live.load(std::memory_order_acquire) && !preparationFailed.load(std::memory_order_acquire)) {
                    bool ready = false;
                    try { ready = prepare(); }
                    catch (...) {
                        preparationFailed.store(true, std::memory_order_release);
                        Wh_Log(L"[STAGGER] excecao durante preparo; mantendo entrance normal");
                    }
                    if (!ready && setupAttempted.load(std::memory_order_acquire))
                        preparationFailed.store(true, std::memory_order_release);
                }
                lock.lock();
            }
            preparationDone = true;
            lock.unlock();
            preparationCv.notify_all();
        });
        preparationRunning.store(true, std::memory_order_release);
    } catch (const std::system_error&) {
        preparationStop = true;
        preparationDone = true;
        preparationFailed.store(true, std::memory_order_release);
        Wh_Log(L"[STAGGER] nao foi possivel criar worker de preparo; mantendo entrance normal");
    }
}
static void StopPreparation() {
    {
        std::lock_guard<std::mutex> lock(preparationMutex);
        preparationStop = true;
        preparationRequested = false;
        preparationRunning.store(false, std::memory_order_release);
    }
    preparationCv.notify_all();
    if (preparationThread) {
        {
            std::unique_lock<std::mutex> lock(preparationMutex);
            if (!preparationCv.wait_for(lock, std::chrono::seconds(2), [] { return preparationDone; }))
                Wh_Log(L"[STAGGER] aguardando resolucao de simbolos antes do unload");
        }
        if (preparationThread->joinable()) preparationThread->join();
        delete preparationThread;
        preparationThread = nullptr;
    }
}
static BOOL CALLBACK DetectDirectUi(HWND child, LPARAM data) {
    wchar_t name[64]{};
    if (GetClassNameW(child, name, ARRAYSIZE(name)) && !wcscmp(name, L"DirectUIHWND")) {
        *reinterpret_cast<bool*>(data) = true;
        return FALSE;
    }
    return TRUE;
}
static bool HasDirectUiPayload(HWND view) {
    bool directUi = false;
    EnumChildWindows(view, DetectDirectUi, reinterpret_cast<LPARAM>(&directUi));
    return directUi;
}
static void TrackRemoval(HWND view) {
    if (!Removal::ready.load(std::memory_order_acquire) || !Enabled()) return;
    bool bound=false;
    AcquireSRWLockShared(&ownerLock);
    for(auto& w:Removal::witnesses) if(w.owner==GetCurrentThreadId()&&w.view==view) bound=true;
    ReleaseSRWLockShared(&ownerLock);
    if (!bound && IsExplorerContent(view) && HasDirectUiPayload(view)) Removal::Bind(view);
}
static ULONGLONG Arm(HWND view, ULONGLONG reveal, int totalMs, bool frameDriven = false,
                     const AnimationTimeline* timeline = nullptr,
                     bool composition = false, const POINT* pageOffset = nullptr, int pageMoveMs = 1,
                     ULONGLONG minEnd = 0) {
    if (!view || !IsWindow(view) ||
        GetWindowThreadProcessId(view, nullptr) != GetCurrentThreadId() ||
        stopping.load(std::memory_order_seq_cst) ||
        g_unloading.load(std::memory_order_acquire) || (!pageOffset && !Enabled())) return 0;
    if (!HasDirectUiPayload(view)) {
        return 0;
    }
    BOOL animations = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animations, 0);
    if (!animations) {
        return 0;
    }
    if (!live.load(std::memory_order_acquire)) {
        RequestPreparation();
        return 0;
    }
    // O grupo da pagina coexiste com o staggering ja armado nesta mesma view.
    if (!pageOffset) { CancelView(view); Removal::Bind(view); }
    ReclaimExitedOwners();
    for (Group& group : groups) {
        int expected = Free;
        if (!group.state.compare_exchange_strong(expected, Reserved,
                std::memory_order_seq_cst)) continue;
        busy.fetch_add(1, std::memory_order_acq_rel);
        group.count = 0;
        group.ownerHandle = stopping.load(std::memory_order_seq_cst)
            ? nullptr : OpenThread(SYNCHRONIZE, FALSE, GetCurrentThreadId());
        if (!group.ownerHandle) { Release(group); return 0; }
        group.owner.store(GetCurrentThreadId(), std::memory_order_relaxed);
        group.action = nullptr;
        group.view = view;
        if (timeline) group.timeline = *timeline;
        else group.timeline.Init();
        ULONGLONG now = (ULONGLONG)group.timeline.Now();
        group.reveal = reveal;
        group.collectMs = std::min(100, totalMs / 3);
        group.firstBatch = 0;
        group.waitUntil = std::max(reveal, now) + kFirstBatchWaitMs;
        group.acceptUntil = frameDriven ? 0 : reveal + group.collectMs;
        group.growMs = GrowDuration(totalMs);
        group.holdMs = AddHoldDuration(totalMs);
        group.fromScale = kAddScale;
        group.frameDriven = frameDriven;
        group.useComposition = composition;
        group.pageMotion = pageOffset != nullptr;
        group.pageOffset = pageOffset ? *pageOffset : POINT{};
        if (pageOffset) { group.growMs = std::max(1, pageMoveMs); group.holdMs = 0; }
        group.compositionFailureLogged = false;
        group.stepMs = 20;
        group.maxDelay = std::min(160, totalMs / 3);
        group.sequenceEnd = 0;
        group.scheduled = 0;
        group.canceling = false;
        group.repairing = false;
        group.repairPass = 0;
        group.curtain = false;
        group.handoffReady = false;
        group.awaitingHandoff = false;
        group.handoffAt = 0;
        group.contentPainted = false;
        ++group.serial;
        ULONGLONG end = frameDriven ? group.waitUntil + group.collectMs + 16 + group.holdMs + group.maxDelay + group.growMs
            : std::max(group.reveal, group.acceptUntil + 16) + group.holdMs + group.maxDelay + group.growMs;
        if (composition) end += 64 + CURTAIN_READY_MS; // curva, cortina e cleanup
        // A pagina aguarda o staggering da mesma view para devolver sob a mesma cortina.
        if (pageOffset) end = std::max(group.waitUntil + group.growMs + 96 + CURTAIN_READY_MS, minEnd);
        group.deadlineAt = end;
        int remaining = (int)std::clamp<ULONGLONG>(end > now ? end - now : 1, 1, 8000);
        group.state.store(Busy, std::memory_order_release);
        Action action{sizeof(Action), 0.f, remaining / 1000.f, 0.f, 0, frameDriven ? 50u : 8u, ActionProc, &group};
        group.action = createAction(&action);
        if (!group.action) {
            group.state.store(Reserved, std::memory_order_seq_cst);
            Release(group);
            Wh_Log(L"[STAGGER] CreateAction falhou: %08X", GetLastError());
            return 0;
        }
        return end;
    }
    Wh_Log(L"[STAGGER] limite de views simultaneas; mantendo entrance normal");
    return 0;
}
static void BeforeUninit() {
    StopPreparation();
    stopping.store(true, std::memory_order_seq_cst);
    // CancelView na subclass restaura os elementos no owner. O scanner so
    // libera threads encerrados; callbacks remanescentes precisam drenar.
    for (;;) {
        ReclaimExitedOwners();
        Removal::Reclaim();
        bool active = Removal::Active();
        for (Group& group : groups)
            active = active || group.state.load(std::memory_order_seq_cst) != Free;
        if (!active) break;
        Sleep(8);
    }
}
static void Uninit() {
    Removal::Uninit();
    if (duiModule && FreeLibrary(duiModule)) duiModule = nullptr;
    if (duserModule && FreeLibrary(duserModule)) duserModule = nullptr;
    if (itemModule && FreeLibrary(itemModule)) itemModule = nullptr;
    nativeItemVtable = nullptr;
}
#undef STAGGER_MEMBER
} // namespace ItemStagger

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
// Frames de slide aplicados no thread proprietario da view
// -----------------------------------------------

struct AnimInfo {
    AnimAxis axis;
    bool isPropPage;
    bool useFade;
    bool useDCompFade;
    int buttonRowTop;
    TransitionType transition;
    bool nativePage = false;
};

static constexpr UINT_PTR SLIDE_SUBCLASS_ID = 0xFECC;
static std::atomic<UINT_PTR> g_slideGeneration{1};

struct CompositorClock {
    using WaitFn = DWORD(WINAPI*)(UINT, const HANDLE*, DWORD);
    enum class Result { Frame, Canceled, Fallback };
    HMODULE module = nullptr;
    HANDLE wake = nullptr;
    WaitFn wait = nullptr;
    CompositorClock() = default;
    CompositorClock(const CompositorClock&) = delete;
    CompositorClock& operator=(const CompositorClock&) = delete;
    ~CompositorClock() {
        if (module) FreeLibrary(module);
        if (wake) CloseHandle(wake);
    }
    void Init() {
        wake = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!wake) return;
        module = LoadLibraryExW(L"dcomp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (module) wait = reinterpret_cast<WaitFn>(
            GetProcAddress(module, "DCompositionWaitForCompositorClock"));
    }
    void Cancel() { if (wake) SetEvent(wake); }
    Result Wait(DWORD timeoutMs) const {
        if (!wait || !wake) return Result::Fallback;
        // DOCUMENTED EXCEPTION: espera do compositor somente no worker,
        // limitada e cancelavel. Nao usa DwmFlush nem bloqueia o owner DUI.
        DWORD status = wait(1, &wake, timeoutMs);
        if (WaitForSingleObject(wake, 0) == WAIT_OBJECT_0) return Result::Canceled;
        if (status == WAIT_OBJECT_0 || status == WAIT_OBJECT_0 + 1 || status == WAIT_TIMEOUT)
            return Result::Frame;
        // Display desligado/ocluido pode retornar imediatamente: usar o
        // fallback temporizado para nao criar um loop ocupado.
        return Result::Fallback;
    }
};
static std::chrono::steady_clock::time_point NextFrameDeadline(
    std::chrono::steady_clock::time_point previous,
    std::chrono::steady_clock::time_point now, long long frameUs) {
    auto step = std::chrono::microseconds(std::max(1LL, frameUs));
    auto next = previous + step;
    if (next <= now) next += step * ((now - next) / step + 1);
    return next;
}

struct SlideAnimation {
    HWND hwnd = nullptr;
    HWND inputHost = nullptr;
    HWND parent = nullptr;
    UINT_PTR generation = 0;
    AnimInfo info{};
    Settings settings{};
    POINT base{};
    POINT offset{};
    POINT initialOffset{};
    HRGN originalRegion = nullptr;
    ULONGLONG startAt = 0;
    // startAt e imutavel para o worker; os inicios efetivos pertencem ao owner.
    double effectiveStartAt = 0., moveStartAt = 0.;
    double preparationDeadline = 0.;
    int holdMs = 0;
    int moveMs = 0;
    int totalMs = 0;
    bool started = false;
    bool moveStarted = false;
    bool staggerPending = false;
    bool positioned = false;
    bool applying = false;
    bool finishing = false;
    bool accelerated = false; // clique/tecla: grupos concluindo rapido, slide segue
    bool revealed = false;
    int lastClipH = -1;
    long long frameUs = 16667;
    CompositorClock clock;
    AnimationTimeline timeline;
    std::atomic<bool> stop{false};
    std::atomic<bool> frameQueued{false};
    // DOCUMENTED EXCEPTION: o CRT pode destruir g_slides sem unload no exit.
    // Nao destruir um thread joinable nesse caminho; o SO recupera o objeto.
    // FinishSlide sempre faz join/delete no fim normal, cancelamento e unload.
    std::thread* worker = nullptr;

    ~SlideAnimation() {
        if (originalRegion) DeleteObject(originalRegion);
    }
};

static std::shared_ptr<SlideAnimation> FindSlide(HWND hwnd) {
    std::lock_guard<std::mutex> lk(g_animMtx);
    auto it = g_slides.find(hwnd);
    return it != g_slides.end() ? it->second : nullptr;
}

// A rolagem move o conteudo sob a cortina congelada e sob camadas que nao recebem
// o clip do viewport: retirar a cortina e devolver as camadas desta thread.
static void HandleContentScroll() {
    if (g_unloading.load(std::memory_order_acquire)) return;
    HWND views[16], curtains[8];
    int curtainCount = CurtainViews(GetCurrentThreadId(), curtains, 8);
    for (int i = 0; i < curtainCount; ++i) DropOverlay(curtains[i]);
    int count = ItemStagger::ScrollViews(views, 16);
    for (int i = 0; i < count; ++i) ItemStagger::CancelView(views[i]);
}

static LRESULT CALLBACK SlideSubclassProc(
    HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
static LRESULT CALLBACK PageInputSubclassProc(
    HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

static void RestoreSlideRegion(const std::shared_ptr<SlideAnimation>& s) {
    // Preservar o original: cada SetWindowRgn recebe uma copia transferivel.
    HRGN copy = nullptr;
    if (s->originalRegion) {
        copy = CreateRectRgn(0, 0, 0, 0);
        if (!copy) return;
        if (CombineRgn(copy, s->originalRegion, nullptr, RGN_COPY) == ERROR) {
            DeleteObject(copy);
            return;
        }
    }
    if (!SetWindowRgn(s->hwnd, copy, TRUE) && copy)
        DeleteObject(copy);
}

static void FinishSlide(const std::shared_ptr<SlideAnimation>& s,
                        bool restore = true) {
    if (s->finishing) return;
    s->finishing = true;
    {
        // Sob o mutex do worker: entre o teste do predicado e o sono a
        // notificacao se perderia, e o join esperaria o timeout inteiro.
        std::lock_guard<std::mutex> lk(g_sleepMtx);
        s->stop.store(true, std::memory_order_release);
    }
    s->clock.Cancel();
    g_sleepCv.notify_all();
    // O worker apenas posta frames: pode ser aguardado sem depender da UI.
    if (s->worker && s->worker->joinable()) s->worker->join();
    delete s->worker;
    s->worker = nullptr;
    if (s->inputHost) RemoveWindowSubclass(s->inputHost, PageInputSubclassProc, SLIDE_SUBCLASS_ID);
    s->inputHost = nullptr;
    ItemStagger::CancelView(s->hwnd);
    if (restore && IsWindow(s->hwnd)) {
        if (GetParent(s->hwnd) == s->parent &&
            (s->offset.x || s->offset.y)) {
            s->applying = true;
            SetWindowPos(s->hwnd, nullptr, s->base.x, s->base.y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            s->applying = false;
        }
        RestoreSlideRegion(s);
        if (s->info.useFade && !s->info.useDCompFade)
            EndFade(s->hwnd);
    }
    RemoveWindowSubclass(s->hwnd, SlideSubclassProc, SLIDE_SUBCLASS_ID);
    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        auto it = g_slides.find(s->hwnd);
        if (it != g_slides.end() && it->second == s) {
            g_slides.erase(it);
            g_animating.erase(s->hwnd);
        }
    }
    g_animDoneCv.notify_all();
}

static LRESULT CALLBACK PageInputSubclassProc(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR view) {
    auto s = FindSlide(reinterpret_cast<HWND>(view));
    // Layout e hover fazem parte da abertura normal. Cancelar aqui tambem
    // descartaria o staggering antes de seu primeiro frame.
    bool interrupt = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN ||
        msg == WM_XBUTTONDOWN || msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN || msg == WM_CONTEXTMENU;
    // Clique e tecla concluem as camadas rapido; a rolagem as devolve na hora.
    if (s && interrupt && (s->accelerated || ItemStagger::Accelerate(s->hwnd))) {
        s->accelerated = true;
    } else if (s && (interrupt || msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL ||
        msg == WM_VSCROLL || msg == WM_HSCROLL ||
        msg == WM_NCDESTROY || (msg == WM_SHOWWINDOW && !wp))) {
        FinishSlide(s);
    }
    if (msg == WM_NCDESTROY) RemoveWindowSubclass(hwnd, PageInputSubclassProc, SLIDE_SUBCLASS_ID);
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void ApplySlideFrame(const std::shared_ptr<SlideAnimation>& s) {
    if (g_unloading.load(std::memory_order_acquire) ||
        GetParent(s->hwnd) != s->parent || !IsWindowVisible(s->hwnd)) {
        FinishSlide(s);
        return;
    }
    double now = s->timeline.Now();
    if (now < s->startAt) return;
    if (!s->started && s->settings.stagger && !s->info.isPropPage)
        ItemStagger::Removal::CancelView(s->hwnd);
    if (s->settings.stagger && !s->info.isPropPage) ItemStagger::TrackRemoval(s->hwnd);
    if (!s->started && s->staggerPending && !ItemStagger::live.load(std::memory_order_acquire)) {
        bool available = ItemStagger::preparationRunning.load(std::memory_order_acquire) &&
            !ItemStagger::preparationFailed.load(std::memory_order_acquire) && ItemStagger::Enabled();
        if (available && now < s->preparationDeadline) return;
        s->staggerPending = false;
    }
    if (!s->started && s->info.nativePage && !ItemStagger::live.load(std::memory_order_acquire) &&
        ItemStagger::preparationRunning.load(std::memory_order_acquire) &&
        !ItemStagger::preparationFailed.load(std::memory_order_acquire) && now < s->preparationDeadline) return;
    if (!s->started) {
        s->started = true;
        s->effectiveStartAt = s->info.isPropPage ? double(s->startAt) : now;
        ULONGLONG staggerEnd = 0;
        if (s->staggerPending) {
            s->staggerPending = false;
            staggerEnd = ItemStagger::Arm(s->hwnd, (ULONGLONG)now + s->holdMs, s->totalMs, true,
                &s->timeline, true);
        }
        if (s->info.nativePage) {
            ItemStagger::Arm(s->hwnd, (ULONGLONG)now + s->holdMs,
                s->totalMs, true, &s->timeline, true, &s->initialOffset, s->moveMs, staggerEnd);
        }
        ItemStagger::RebasePending(s->hwnd, (ULONGLONG)s->effectiveStartAt + s->holdMs);
        bool navEntrance = !s->info.isPropPage &&
            s->info.axis == AnimAxis::Horizontal &&
            s->info.transition == TransitionType::Entrance &&
            s->settings.animateNavPane;
        if (!s->info.isPropPage &&
            s->info.transition == TransitionType::Entrance &&
            (s->info.useFade || s->info.useDCompFade || navEntrance) &&
            s->settings.alphaFade &&
            g_dcDevice.load(std::memory_order_acquire)) {
            if (s->info.useFade && !s->info.useDCompFade)
                EndFade(s->hwnd);
            s->info.useFade = false;
            s->info.useDCompFade = true;
            // O painel tem um target proprio; o target da pasta continua no
            // CabinetWClass. Nenhum deles substitui o overlay do outro.
            HWND root = navEntrance ? s->hwnd : GetAncestor(s->hwnd, GA_ROOT);
            if (!HasDCompFade(root)) {
                CapturedFrame frame;
                if (CaptureContentFrame(s->hwnd, root, frame)) {
                    int hold = std::min(s->totalMs - 1,
                                        s->holdMs + ENTRANCE_DCOMP_HOLD_MS);
                    int fade = CalcFadeDuration(s->totalMs);
                    if (!StartDCompFade(root, frame, fade,
                                        hold, true))
                        Wh_Log(L"[DComp] fade de entrada falhou root=%p", root);
                }
            }
        }
    }

    ItemStagger::FrameView(s->hwnd, now);
    double elapsed = now - s->effectiveStartAt;
    bool reveal = elapsed >= s->holdMs;
    if (!s->info.isPropPage && reveal) {
        // A regiao sera liberada ainda na posicao inicial, para o primeiro
        // paint inscrever os itens sem mostrar seu tamanho nativo por um frame.
        if (!s->moveStarted && !ItemStagger::WaitingForItems(s->hwnd, now)) {
            s->moveStarted = true;
            s->moveStartAt = now;
        }
        elapsed = s->holdMs + (s->moveStarted ? now - s->moveStartAt : 0.);
    }
    // WinUI: keyframes em 0/150/450 ms; configuracoes escalam a timeline.
    double t = std::clamp((elapsed - s->holdMs) / s->moveMs, 0.0, 1.0);
    double remaining = 1.0 - WinUIEntranceEase(t);
    POINT offset = {
        (LONG)std::lround(s->initialOffset.x * remaining),
        (LONG)std::lround(s->initialOffset.y * remaining)
    };
    if (!s->info.nativePage && (!s->positioned || offset.x != s->offset.x || offset.y != s->offset.y)) {
        s->positioned = true;
        s->offset = offset;
        s->applying = true;
        BOOL moved = SetWindowPos(
            s->hwnd, nullptr, s->base.x + offset.x, s->base.y + offset.y,
            0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        s->applying = false;
        if (!moved) {
            FinishSlide(s);
            return;
        }
    }

    // A view nova permanece oculta durante a fase de saida de 150 ms.
    if (reveal) {
        if (!s->revealed) {
            RestoreSlideRegion(s);
            s->revealed = true;
        }
        if (s->info.buttonRowTop >= 0) {
            RECT rc{};
            if (GetClientRect(s->hwnd, &rc)) {
                int visible = s->info.buttonRowTop - s->base.y - s->offset.y;
                int clipH = std::clamp(visible, 0, (int)rc.bottom);
                if (clipH != s->lastClipH) {
                    s->lastClipH = clipH;
                    RestoreSlideRegion(s);
                    HRGN clip = CreateRectRgn(0, 0, rc.right, clipH);
                    if (clip && s->originalRegion)
                        CombineRgn(clip, clip, s->originalRegion, RGN_AND);
                    if (clip && !SetWindowRgn(s->hwnd, clip, TRUE))
                        DeleteObject(clip);
                }
            }
        }
    }

    if (s->info.useFade && !s->info.useDCompFade) {
        double fadeT = std::clamp(
            (elapsed - s->holdMs) / CalcFadeDuration(s->totalMs), 0.0, 1.0);
        SetAlpha(s->hwnd, (BYTE)std::clamp(
            (int)std::lround(WinUIEntranceEase(fadeT) * 255.0), 1, 255));
    }
    if (elapsed >= s->totalMs && !ItemStagger::ActiveView(s->hwnd))
        FinishSlide(s);
}

static LRESULT CALLBACK SlideSubclassProc(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR generation) {
    auto s = FindSlide(hwnd);
    if (!s || s->generation != generation)
        return DefSubclassProc(hwnd, msg, wp, lp);

    if (msg == WM_WH_ANIMATION_FRAME) {
        if (wp != generation) return 0;
        s->frameQueued.store(false, std::memory_order_release);
        ApplySlideFrame(s);
        return 0;
    }
    if (msg == WM_WH_REMOVE_SUBCLASS) {
        FinishSlide(s);
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        FinishSlide(s, false);
    } else if (s->info.nativePage &&
        (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN ||
         msg == WM_KEYDOWN || msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL)) {
        bool wheel = msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL;
        // A transformacao e somente visual: devolver a superficie nativa
        // antes de interagir, sem deslocar hit targets. Layout fica nativo.
        // Clique e tecla concluem as camadas rapido em vez de cortar no meio.
        if (!wheel && (s->accelerated || ItemStagger::Accelerate(s->hwnd))) s->accelerated = true;
        else FinishSlide(s);
    } else if (msg == WM_WINDOWPOSCHANGING && s->applying) {
        // Consumir so a chamada propria; um layout reentrante continua nativo.
        s->applying = false;
    } else if (msg == WM_WINDOWPOSCHANGING && !s->finishing) {
        auto pos = reinterpret_cast<WINDOWPOS*>(lp);
        if (pos->flags & SWP_HIDEWINDOW) {
            FinishSlide(s);
        } else if (!s->info.nativePage && !(pos->flags & SWP_NOMOVE)) {
            // O layout nativo fornece o novo destino; o offset e temporario.
            // Um reposicionamento para a posicao atual nao acumula o offset.
            if (pos->x != s->base.x + s->offset.x ||
                pos->y != s->base.y + s->offset.y)
                s->base = {pos->x, pos->y};
            pos->x = s->base.x + s->offset.x;
            pos->y = s->base.y + s->offset.y;
        }
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static bool PrepareSlide(HWND hwnd, AnimInfo info) {
    DWORD ownerThread = GetWindowThreadProcessId(hwnd, nullptr);
    if (ownerThread != GetCurrentThreadId() ||
        !(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) {
        Wh_Log(L"[ANIM] view ignorada hwnd=%p owner=%lu caller=%lu", hwnd,
               ownerThread, GetCurrentThreadId());
        return false;
    }
    auto s = std::make_shared<SlideAnimation>();
    s->timeline.Init();
    s->hwnd = hwnd;
    s->parent = GetParent(hwnd);
    s->generation = g_slideGeneration.fetch_add(1, std::memory_order_relaxed);
    s->info = info;
    AcquireSRWLockShared(&g_sLock);
    s->settings = g_s;
    ReleaseSRWLockShared(&g_sLock);

    RECT rect{};
    if (!s->parent || !GetWindowRect(hwnd, &rect)) return false;
    MapWindowPoints(nullptr, s->parent, reinterpret_cast<POINT*>(&rect), 2);
    s->base = {rect.left, rect.top};
    UINT dpi = GetDpiForWindow(hwnd);
    s->clock.Init();
    MONITORINFOEXW monitor{};
    monitor.cbSize = sizeof(monitor);
    if (GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitor)) {
        DEVMODEW mode{};
        mode.dmSize = sizeof(mode);
        if (EnumDisplaySettingsW(monitor.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
            mode.dmDisplayFrequency >= 24)
            s->frameUs = 1000000LL / mode.dmDisplayFrequency;
    }
    double distance = s->settings.distance * (dpi ? dpi : 96) / 96.0;
    if (info.transition == TransitionType::SlideForward)
        distance *= 200.0 / 140.0;
    else if (info.transition == TransitionType::SlideBack)
        distance *= 150.0 / 140.0;
    int offset = (int)std::lround(distance);
    if (info.axis == AnimAxis::Horizontal)
        s->initialOffset.x = info.transition == TransitionType::SlideForward
            ? offset : -offset;
    else
        s->initialOffset.y = s->settings.reverseDirection && !info.isPropPage
            ? -offset : offset;

    s->totalMs = s->settings.duration;
    s->holdMs = s->totalMs / 3;
    s->moveMs = std::max(1, s->totalMs - s->holdMs);
    s->startAt = info.isPropPage ? (ULONGLONG)s->timeline.Now() : GetBatchStartAt(
        GetAncestor(hwnd, GA_ROOT));
    if (info.axis == AnimAxis::Horizontal &&
        info.transition == TransitionType::Entrance)
        s->startAt += 16;

    s->originalRegion = CreateRectRgn(0, 0, 0, 0);
    if (s->originalRegion && GetWindowRgn(hwnd, s->originalRegion) == ERROR) {
        DeleteObject(s->originalRegion);
        s->originalRegion = nullptr;
    }
    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        if (g_unloading.load(std::memory_order_acquire) ||
            g_animating.count(hwnd))
            return false;
        g_animating.insert(hwnd);
        g_slides.emplace(hwnd, s);
    }
    if (!SetWindowSubclass(hwnd, SlideSubclassProc, SLIDE_SUBCLASS_ID,
                           s->generation)) {
        std::lock_guard<std::mutex> lk(g_animMtx);
        g_animating.erase(hwnd);
        g_slides.erase(hwnd);
        return false;
    }
    if (info.useFade && !info.useDCompFade)
        BeginFade(hwnd);
    HideViaRgn(hwnd);
    s->preparationDeadline = std::max(double(s->startAt), s->timeline.Now()) + ItemStagger::kPreparationWaitMs;
    if (info.nativePage) {
        ItemStagger::RequestPreparation();
        HWND host = FindFolderChild(hwnd, L"DirectUIHWND");
        if (host && SetWindowSubclass(host, PageInputSubclassProc, SLIDE_SUBCLASS_ID,
            reinterpret_cast<DWORD_PTR>(hwnd))) s->inputHost = host;
        else { FinishSlide(s); return false; }
    }
    if (s->settings.stagger && !info.isPropPage &&
        info.axis == AnimAxis::Vertical && info.transition == TransitionType::Entrance) {
        s->staggerPending = ItemStagger::HasDirectUiPayload(hwnd);
        s->preparationDeadline = std::max(double(s->startAt), s->timeline.Now()) + ItemStagger::kPreparationWaitMs;
        if (s->staggerPending) {
            BOOL animations = TRUE;
            SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animations, 0);
            s->staggerPending = animations != FALSE;
            if (s->staggerPending) ItemStagger::RequestPreparation();
        }
    }
    return true;
}

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

    if (age > PENDING_EXPIRE_MS) {
        g_pendingNav.erase(it);
        return TransitionType::Entrance;
    }

    if (it->second.consumed)
        return TransitionType::Entrance;

    // Manter direcionais brevemente apos o consumo. O Explorer pode chamar
    // BrowseObject de novo com um PIDL absoluto durante a mesma navegacao.
    if (type == TransitionType::SlideBack ||
        type == TransitionType::SlideForward) {
        it->second.consumed = true;
    } else {
        g_pendingNav.erase(it);
    }

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

    if (IsExplorerContent(hwnd) || IsFolderIsland(hwnd)) {
        if (ResolveAnimationTarget(hwnd) != hwnd) return false;
        TransitionType transition = ConsumePendingTransition(hwnd);
        if (!pageTransitions)
            transition = TransitionType::Entrance;

        bool dcompReady =
            g_dcDevice.load(std::memory_order_acquire) != nullptr;
        // Basear o caminho no device. Uma aba nova pode nao ter
        // frame capturado; usar WS_EX_LAYERED nesse caso deixa ItemsView vazio.
        bool useDCompFade = dcompReady && alphaFade;
        bool nativePage = !IsFolderIsland(hwnd) && ItemStagger::HasDirectUiPayload(hwnd);
        bool useCpuFade = !useDCompFade && alphaFade && !nativePage && !IsFolderIsland(hwnd);

        AnimAxis axis = AnimAxis::Vertical;
        if (transition == TransitionType::SlideBack   ||
            transition == TransitionType::SlideForward)
            axis = AnimAxis::Horizontal;

        info = {axis, false, useCpuFade, useDCompFade, -1, transition};
        info.nativePage = nativePage;
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
// Navegacao relativa e absoluta usa Entrance. Apenas voltar, avancar e subir
// recebem slide horizontal; o marcador por root cobre chamadas complementares.
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
static void AnimateCreatedPayload(HWND hwnd);

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
    if (hwnd) AnimateCreatedPayload(hwnd);
    return hwnd;
}

// Pesquisa recursiva pelo host de conteudo animavel do Explorer.
// visivel e correctamente dimensionado (w>1, h>1) na arvore de hwndParent.
// Funcao estatica: evita overhead e potenciais problemas do std::function.
static HWND FindVisibleContent(HWND hwndParent) {
    if (IsExplorerContent(hwndParent) || IsFolderIsland(hwndParent)) {
        HWND target = ResolveAnimationTarget(hwndParent);
        if (target && IsWindowVisible(target)) {
            RECT r{}; GetClientRect(target, &r);
            if (r.right > 1 && r.bottom > 1) return target;
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
    if (!pThis || !g_origBrowseObject) return E_FAIL;
    if (g_unloading.load(std::memory_order_acquire))
        return g_origBrowseObject(pThis, pidl, wFlags);

    TransitionType type = TransitionType::Entrance;
    if (wFlags & (SBSP_NAVIGATEBACK | SBSP_PARENT))
        type = TransitionType::SlideBack;
    else if (wFlags & SBSP_NAVIGATEFORWARD)
        type = TransitionType::SlideForward;

    Settings snap;
    AcquireSRWLockShared(&g_sLock);
    snap = g_s;
    ReleaseSRWLockShared(&g_sLock);

    Microsoft::WRL::ComPtr<IOleWindow> browserWindow;
    auto unknown = reinterpret_cast<IUnknown*>(pThis);
    HWND browser = nullptr;
    if (FAILED(unknown->QueryInterface(IID_PPV_ARGS(&browserWindow))) ||
        FAILED(browserWindow->GetWindow(&browser)) || !browser) {
        return g_origBrowseObject(pThis, pidl, wFlags);
    }

    HWND root = GetAncestor(browser, GA_ROOT);
    HWND content = FindVisibleContent(browser);
    if (!root || !content)
        return g_origBrowseObject(pThis, pidl, wFlags);
    // Uma nova navegacao cancela a cauda do staggering antes de reutilizar
    // os itens/host da pagina anterior.
    if (auto active = FindSlide(content)) FinishSlide(active);
    ItemStagger::CancelView(content);
    DropCurtain(content);

    bool prepared = false;
    {
        std::lock_guard<std::mutex> lk(g_pendingMtx);
        auto it = g_pendingNav.find(root);
        ULONGLONG now = GetTickCount64();
        bool preserve = type == TransitionType::Entrance &&
            it != g_pendingNav.end() &&
            it->second.type != TransitionType::Entrance &&
            now - it->second.timestamp < 200;
        if (preserve) {
            type = it->second.type;
            prepared = it->second.dcompPrepared;
            it->second.timestamp = now;
        } else {
            g_pendingNav[root] = {type, now};
        }
    }

    // Nenhum lock de navegacao atravessa COM, captura, DComp ou o original.
    if (snap.alphaFade && !prepared &&
        g_dcDevice.load(std::memory_order_acquire)) {
        CapturedFrame frame;
        if (CaptureContentFrame(content, root, frame)) {
            ULONGLONG start = GetBatchStartAt(root);
            ULONGLONG now = GetTickCount64();
            int delay = (int)std::min<ULONGLONG>(
                start > now ? start - now : 0, 5000) + snap.duration / 3 +
                DCOMP_FADE_SYNC_MARGIN_MS;
            bool started = StartDCompFade(
                root, frame, CalcFadeDuration(snap.duration), delay, false);
            if (started) {
                std::lock_guard<std::mutex> lk(g_pendingMtx);
                auto it = g_pendingNav.find(root);
                if (it != g_pendingNav.end() && it->second.type == type)
                    it->second.dcompPrepared = true;
            } else {
                Wh_Log(L"[DComp] fade de navegacao falhou root=%p", root);
            }
        }
    }

    return g_origBrowseObject(pThis, pidl, wFlags);
}

// -----------------------------------------------
// Despacho de animacao
// -----------------------------------------------

static void DispatchAnimation(HWND hwnd) {
    auto s = FindSlide(hwnd);
    if (!s) return;
    bool accepted;
    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        accepted = !g_unloading.load(std::memory_order_acquire);
        if (accepted) ++g_animationWorkers;
    }
    if (!accepted) {
        FinishSlide(s);
        return;
    }

    try {
        s->worker = new std::thread([s]() {
            ULONGLONG now = (ULONGLONG)s->timeline.Now();
            if (now < s->startAt) {
                std::unique_lock<std::mutex> lk(g_sleepMtx);
                g_sleepCv.wait_for(lk, std::chrono::milliseconds(s->startAt - now), [&] {
                    return g_unloading.load(std::memory_order_acquire) ||
                           s->stop.load(std::memory_order_acquire);
                });
            }
            auto nextFrame = std::chrono::steady_clock::now();
            for (;;) {
                if (g_unloading.load(std::memory_order_acquire) ||
                    s->stop.load(std::memory_order_acquire)) break;
                auto result = s->clock.Wait(50);
                if (result == CompositorClock::Result::Canceled) break;
                if (result == CompositorClock::Result::Fallback) {
                    nextFrame = NextFrameDeadline(nextFrame, std::chrono::steady_clock::now(), s->frameUs);
                    std::unique_lock<std::mutex> lk(g_sleepMtx);
                    g_sleepCv.wait_until(lk, nextFrame, [&] {
                        return g_unloading.load(std::memory_order_acquire) ||
                               s->stop.load(std::memory_order_acquire);
                    });
                }
                if (g_unloading.load(std::memory_order_acquire) ||
                    s->stop.load(std::memory_order_acquire)) break;
                ULONGLONG now = (ULONGLONG)s->timeline.Now();
                if (now >= s->startAt) {
                    if (!s->frameQueued.exchange(true, std::memory_order_acq_rel)) {
                        if (!PostMessageW(s->hwnd, WM_WH_ANIMATION_FRAME, s->generation, 0)) {
                            s->frameQueued.store(false, std::memory_order_release);
                        }
                    }
                }
                // Somente o owner sabe quando o movimento realmente comecou.
                // FinishSlide cancela a espera e encerra este worker sem
                // depender de mensagens adicionais da UI para fazer o join.
            }
            {
                std::lock_guard<std::mutex> lk(g_animMtx);
                --g_animationWorkers;
            }
            g_animDoneCv.notify_all();
        });
    } catch (const std::system_error&) {
        {
            std::lock_guard<std::mutex> lk(g_animMtx);
            --g_animationWorkers;
        }
        FinishSlide(s);
        g_animDoneCv.notify_all();
        Wh_Log(L"[ANIM] nao foi possivel criar o worker hwnd=%p", hwnd);
    }
}

// -- Hook 1: ShowWindow ----------------------------
// Intercept ShowWindow para janelas de conteudo que serao mostradas pela
// primeira vez (IsWindowVisible == false). So os comandos que revelam uma
// janela oculta sao tratados; variantes de minimize/hide sao ignoradas.
using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t      g_origSW = nullptr;
static thread_local bool g_inSW   = false;

static void AnimateCreatedPayload(HWND hwnd) {
    if (g_unloading.load(std::memory_order_acquire)) return;
    wchar_t name[64]{};
    GetClassNameW(hwnd, name, ARRAYSIZE(name));
    bool island = IsFolderIsland(hwnd);
    if (!island && wcscmp(name, L"DirectUIHWND")) return;
    HWND target = island ? hwnd : nullptr;
    for (HWND parent = GetParent(hwnd); parent; parent = GetParent(parent)) {
        if (!IsExplorerContent(parent)) continue;
        if (island) {
            if (auto previous = FindSlide(parent)) FinishSlide(previous);
        } else {
            target = ResolveAnimationTarget(parent);
        }
        break;
    }
    if (!target || FindSlide(target) || !IsWindowVisible(target)) return;
    AnimInfo info;
    if (Classify(target, info) && PrepareSlide(target, info)) DispatchAnimation(target);
}

BOOL WINAPI HookShowWindow(HWND hwnd, int cmd) {
    if (g_inSW || g_unloading.load(std::memory_order_acquire) ||
        IsWindowVisible(hwnd) || FindSlide(hwnd))
        return g_origSW(hwnd, cmd);
    if (cmd != SW_SHOW && cmd != SW_SHOWNA &&
        cmd != SW_SHOWNORMAL && cmd != SW_SHOWNOACTIVATE)
        return g_origSW(hwnd, cmd);

    HWND target = ResolveAnimationTarget(hwnd);
    AnimInfo info;
    if (!target || FindSlide(target) || !Classify(target, info) || !PrepareSlide(target, info))
        return g_origSW(hwnd, cmd);
    g_inSW = true;
    BOOL result = g_origSW(hwnd, cmd);
    g_inSW = false;
    DispatchAnimation(target);
    return result;
}

// -- Hook 2: SetWindowPos --------------------------
using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t g_origSWP = nullptr;
static thread_local bool g_inSWP = false;

BOOL WINAPI HookSetWindowPos(HWND hwnd, HWND hwndAfter,
                            int x, int y, int cx, int cy, UINT flags) {
    if (g_inSWP || g_unloading.load(std::memory_order_acquire) ||
        !(flags & SWP_SHOWWINDOW) || IsWindowVisible(hwnd) || FindSlide(hwnd))
        return g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);

    HWND target = ResolveAnimationTarget(hwnd);
    AnimInfo info;
    if (!target || FindSlide(target) || !Classify(target, info) || !PrepareSlide(target, info))
        return g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);
    g_inSWP = true;
    BOOL result = g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);
    g_inSWP = false;
    if (result)
        DispatchAnimation(target);
    else if (auto s = FindSlide(target))
        FinishSlide(s);
    return result;
}

// -----------------------------------------------
// Windhawk entrypoints
// -----------------------------------------------

// O Painel de Controle roda num explorer.exe proprio (/factory com
// CLSID_ControlPanelProcessExplorerHost), inclusive quando outra janela navega
// para ele; ali nao ha conteudo do Explorador de Arquivos para animar.
static bool IsControlPanelHostProcess() {
    std::wstring line = GetCommandLineW();
    for (wchar_t& c : line) c = (wchar_t)towupper(c);
    return line.find(L"5BD95610-9434-43C2-886C-57852CC8A120") != std::wstring::npos;
}

BOOL Wh_ModInit() {
    if (IsControlPanelHostProcess()) {
        Wh_Log(L"[INIT] processo do Painel de Controle; mod inativo nele");
        return FALSE;
    }
    LoadSettings();

    // Registar mensagens customizadas no range 0xC000-0xFFFF para evitar
    // colisao com WM_USER+N que CabinetWClass possa usar internamente.
    WM_WH_REMOVE_SUBCLASS = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_RemoveSubclass");
    if (!WM_WH_REMOVE_SUBCLASS) {
        Wh_Log(L"[INIT] RegisterWindowMessageW(RemoveSubclass) falhou: %lu",
               GetLastError());
        return FALSE;
    }
    WM_WH_TRY_BROWSER_HOOK = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_TryBrowserHook");
    if (!WM_WH_TRY_BROWSER_HOOK) {
        Wh_Log(L"[INIT] RegisterWindowMessageW(TryBrowserHook) falhou: %lu",
               GetLastError());
        return FALSE;
    }
    WM_WH_ANIMATION_FRAME = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_Frame");
    if (!WM_WH_ANIMATION_FRAME) {
        Wh_Log(L"[INIT] RegisterWindowMessageW(Frame) falhou: %lu", GetLastError());
        return FALSE;
    }

    // Registar hooks ANTES de iniciar threads.
    // Se qualquer Wh_SetFunctionHook falhar, retornamos FALSE e o Windhawk
    // nao chama Wh_ModUninit. Se as threads ja estivessem a correr,
    // ficariam orfas (sem join nem g_unloading).
    if (!WindhawkUtils::SetFunctionHook(
            ShowWindow, HookShowWindow, &g_origSW)) {
        Wh_Log(L"[INIT] hook ShowWindow falhou");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            SetWindowPos, HookSetWindowPos, &g_origSWP)) {
        Wh_Log(L"[INIT] hook SetWindowPos falhou");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            CreateWindowExW, HookCreateWindowExW, &g_origCWEW)) {
        Wh_Log(L"[INIT] hook CreateWindowExW falhou");
        return FALSE;
    }

    // Apenas simbolos de um modulo ja carregado: nao criar objetos Shell
    // no startup do processo nem enumerar outras janelas via COM.
    TryHookBrowseObjectSymbols();

    return TRUE;
}

// O modulo pode carregar depois de Wh_ModInit; a primeira janela tenta de novo.
static void TryHookBrowseObjectSymbols() {
    if (g_unloading.load(std::memory_order_acquire) ||
        g_browserHookInstalled.load(std::memory_order_acquire))
        return;
    std::unique_lock<std::mutex> lk(g_browseObjectHookMtx, std::try_to_lock);
    if (!lk.owns_lock() || g_browserHookInstalled.load(std::memory_order_acquire))
        return;
    HMODULE module = GetModuleHandleW(L"explorerframe.dll");
    if (!module) return;
    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {{
            LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const __unaligned *,unsigned int))",
            LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST const *,unsigned int))",
        }, &g_origBrowseObject, HookBrowseObject, true},
    };
    WindhawkUtils::HookSymbols(module, explorerframe_dll_hooks,
                               ARRAYSIZE(explorerframe_dll_hooks));
    g_browserHookInstalled.store(g_origBrowseObject != nullptr,
                                  std::memory_order_release);
    if (!g_origBrowseObject)
        Wh_Log(L"[INIT] BrowseObject indisponivel; usando entrance e APPCOMMAND");
}

static BOOL CALLBACK EnumExplorerWindowsProc(HWND hwnd, LPARAM) {
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
}

void Wh_ModAfterInit() {
    ItemStagger::StartPreparation();
    // Hooks ja aplicados pelo Windhawk neste ponto -- seguro iniciar
    // threads de suporte.

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
        HRESULT hrFactory = CreateDXGIFactory1(
            __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pFactory));
        if (SUCCEEDED(hrFactory) && pFactory) {
            IDXGIAdapter* pAdapter = nullptr;
            HRESULT hrAdapter = pFactory->EnumAdapters(0, &pAdapter);
            pFactory->Release();

            if (SUCCEEDED(hrAdapter) && pAdapter) {
                if (g_unloading.load(std::memory_order_relaxed)) {
                    pAdapter->Release();
                    return;
                }
                HRESULT hrDevice = D3D11CreateDevice(
                    pAdapter,
                    D3D_DRIVER_TYPE_UNKNOWN, // UNKNOWN quando adapter explicito
                    nullptr,
                    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                    nullptr, 0, D3D11_SDK_VERSION,
                    &g_d3dDevice, &level, nullptr);
                pAdapter->Release();
                if (FAILED(hrDevice)) {
                    Wh_Log(L"[DComp] D3D11CreateDevice(adapter) falhou: 0x%08X",
                           (unsigned)hrDevice);
                    if (g_d3dDevice) {
                        g_d3dDevice->Release();
                        g_d3dDevice = nullptr;
                    }
                }
            } else if (FAILED(hrAdapter)) {
                Wh_Log(L"[DComp] IDXGIFactory1::EnumAdapters falhou: 0x%08X",
                       (unsigned)hrAdapter);
            }
        } else {
            Wh_Log(L"[DComp] CreateDXGIFactory1 falhou: 0x%08X",
                   (unsigned)hrFactory);
        }

        if (g_unloading.load(std::memory_order_relaxed)) return;

        // Fallback: device WARP (software) — sempre disponivel, init instantanea.
        if (!g_d3dDevice) {
            HRESULT hrWarp = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP,
                nullptr,
                D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                nullptr, 0, D3D11_SDK_VERSION,
                &g_d3dDevice, &level, nullptr);
            if (FAILED(hrWarp)) {
                Wh_Log(L"[DComp] D3D11CreateDevice(WARP) falhou: 0x%08X",
                       (unsigned)hrWarp);
                if (g_d3dDevice) {
                    g_d3dDevice->Release();
                    g_d3dDevice = nullptr;
                }
            }
        }

        if (g_unloading.load(std::memory_order_relaxed)) return;

        if (g_d3dDevice) {
            HRESULT hrDxgi = g_d3dDevice->QueryInterface(
                __uuidof(IDXGIDevice),
                reinterpret_cast<void**>(&g_dxgiDevice));
            if (FAILED(hrDxgi)) {
                Wh_Log(L"[DComp] QueryInterface(IDXGIDevice) falhou: 0x%08X",
                       (unsigned)hrDxgi);
                if (g_dxgiDevice) g_dxgiDevice->Release();
                g_dxgiDevice = nullptr;
            }
        }
        if (g_dxgiDevice) {
            using DCompCreate2_t = HRESULT(WINAPI*)(IUnknown*, REFIID, void**);
            HMODULE hDComp = LoadLibraryExW(L"dcomp.dll", nullptr,
                                            LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (hDComp) {
                auto pfn = reinterpret_cast<DCompCreate2_t>(
                    GetProcAddress(hDComp, "DCompositionCreateDevice2"));
                if (pfn) {
                    IDCompositionDesktopDevice* pDC = nullptr;
                    HRESULT hrDComp = pfn(
                        g_dxgiDevice, __uuidof(IDCompositionDesktopDevice),
                        reinterpret_cast<void**>(&pDC));
                    if (SUCCEEDED(hrDComp) && pDC) {
                        g_dcompModule = hDComp;
                        hDComp = nullptr; // ownership transferido para o estado global
                        // Store com release publica devices e o modulo retido.
                        g_dcDevice.store(pDC, std::memory_order_release);
                    } else {
                        Wh_Log(L"[DComp] DCompositionCreateDevice2 falhou: "
                               L"0x%08X device=%p", (unsigned)hrDComp, pDC);
                        if (pDC) pDC->Release();
                    }
                } else {
                    Wh_Log(L"[DComp] DCompositionCreateDevice2 nao exportada: %lu",
                           GetLastError());
                }
                if (hDComp) FreeLibrary(hDComp);
            } else {
                Wh_Log(L"[DComp] LoadLibraryExW(dcomp.dll) falhou: %lu",
                       GetLastError());
            }
        }
    });

    // Subclassear janelas CabinetWClass ja abertas quando o mod e instalado
    // com o Explorer em execucao. Novas janelas sao capturadas pelo hook de
    // CreateWindowExW.
    EnumWindows(EnumExplorerWindowsProc, 0);
}

static void RemoveSubclassBeforeUnload(HWND hwnd) {
    for (int attempt = 0; attempt < 3 && IsWindow(hwnd); ++attempt) {
        if (SendMessageTimeoutW(hwnd, WM_WH_REMOVE_SUBCLASS, 0, 0,
                                SMTO_BLOCK | SMTO_ABORTIFHUNG, 1000, nullptr))
            return;
    }
    if (IsWindow(hwnd)) {
        Wh_Log(L"[UNLOAD] aguardando a UI para restaurar hwnd=%p", hwnd);
        SendMessageW(hwnd, WM_WH_REMOVE_SUBCLASS, 0, 0);
    }
}

void Wh_ModBeforeUninit() {
    // Sinaliza para todas as threads em voo que devem encerrar.
    g_unloading.store(true, std::memory_order_release);
    ItemStagger::StopPreparation();
    // Acorda imediatamente os workers aguardando o proximo frame. O lock vazio
    // ordena o store antes de um worker que esteja entre o predicado e o sono.
    { std::lock_guard<std::mutex> lk(g_sleepMtx); }
    g_sleepCv.notify_all();

    // Aguardar o init thread D3D/DComp terminar antes de prosseguir.
    // Se o mod for descarregado durante a inicializacao (cold driver ~100-300ms),
    // sem este join o thread continuaria a escrever g_d3dDevice/g_dxgiDevice
    // enquanto Wh_ModUninit os liberta -> UAF.
    if (g_initThread && g_initThread->joinable())
        g_initThread->join();
    delete g_initThread;
    g_initThread = nullptr;

    // Os workers so postam mensagens; restaurar e remover a subclass na UI
    // tambem faz join, sem nenhuma espera circular por chamadas Win32.
    {
        std::vector<HWND> windows;
        {
            std::lock_guard<std::mutex> lk(g_animMtx);
            for (const auto& [hwnd, slide] : g_slides) windows.push_back(hwnd);
        }
        for (HWND hwnd : windows) RemoveSubclassBeforeUnload(hwnd);
    }

    // Remover subclasses antes do DLL descarregar para evitar crash com
    // proc invalido. Envia WM_WH_REMOVE_SUBCLASS para o thread da janela
    // (unico thread que pode chamar RemoveWindowSubclass com seguranca).
    // Tentar com timeout primeiro; uma falha persistente exige esperar pela UI.
    {
        std::vector<HWND> toRemove;
        {
            std::lock_guard<std::mutex> lk(g_subclassMtx);
            toRemove.assign(g_subclassedWindows.begin(), g_subclassedWindows.end());
        }
        for (HWND hw : toRemove) {
            RemoveSubclassBeforeUnload(hw);
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
            RemoveSubclassBeforeUnload(hw);
        }
    }

    // A primeira espera limitada permite diagnosticar stalls antes de aguardar
    // todos os workers; nenhum thread pode sobreviver ao unload.
    {
        std::unique_lock<std::mutex> lk(g_animMtx);
        if (!g_animDoneCv.wait_for(lk, std::chrono::milliseconds(2000),
                [] { return g_animationWorkers == 0; })) {
            Wh_Log(L"[UNLOAD] aguardando workers de animacao");
            g_animDoneCv.wait(lk, [] { return g_animationWorkers == 0; });
        }
    }

    // Acordar e aguardar o worker de cleanup DComp.
    // Join aqui garante que todos os refs de device AddRef'd sao libertados
    // antes de Wh_ModUninit libertar g_dcDevice.
    g_cleanupCv.notify_one();
    if (g_cleanupThread && g_cleanupThread->joinable())
        g_cleanupThread->join();
    delete g_cleanupThread;
    g_cleanupThread = nullptr;
    ItemStagger::BeforeUninit();
}

void Wh_ModUninit() {
    ItemStagger::Uninit();
    // Limpar estado global. Neste ponto todas as threads ja terminaram
    // (garantido pelo Wh_ModBeforeUninit acima).

    // Limpar animacoes DComp em voo e libertar o device sob o MESMO lock que
    // StartDCompFade usa para ler g_dcDevice -- ver comentario em
    // StartDCompFade. Isso serializa contra qualquer chamada de HookBrowseObject
    // que ainda esteja em voo neste exato instante (o motor do Windhawk nao
    // garante que uma chamada de hook ja em curso termine antes de
    // Wh_ModBeforeUninit/Wh_ModUninit rodarem -- so bloqueia chamadas novas).
    {
        std::lock_guard<std::mutex> lkd(g_dcFadeMtx);
        for (auto& [hwnd, a] : g_dcFades) {
            if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
            if (a.pEffect)  a.pEffect->Release();
            if (a.pVisual)  a.pVisual->Release();
            if (a.pSurface) a.pSurface->Release();
        }
        g_dcFades.clear();
        // Referencias enfileiradas depois do join do worker (ex.: cortina
        // levantada durante o unload) so podem ser liberadas aqui.
        {
            std::lock_guard<std::mutex> lkq(g_cleanupMtx);
            for (auto& c : g_cleanupQueue) if (c.dev) c.dev->Release();
            g_cleanupQueue.clear();
        }

        IDCompositionDesktopDevice* dcDev = g_dcDevice.exchange(nullptr, std::memory_order_relaxed);
        if (dcDev)  { dcDev->Commit();  dcDev->Release(); }
    }
    if (g_dxgiDevice){ g_dxgiDevice->Release(); g_dxgiDevice = nullptr; }
    if (g_d3dDevice) { g_d3dDevice->Release();  g_d3dDevice  = nullptr; }
    if (g_dcompModule) {
        // So zera o ponteiro em sucesso -- se FreeLibrary falhar, o modulo
        // continua carregado e a variavel deve continuar a refletir isso.
        if (FreeLibrary(g_dcompModule))
            g_dcompModule = nullptr;
        else
            Wh_Log(L"[UNLOAD] FreeLibrary(dcomp.dll) falhou: %lu",
                   GetLastError());
    }

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
    ItemStagger::StartPreparation();
    ItemStagger::RequestPreparation();
}
