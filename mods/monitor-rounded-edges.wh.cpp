// ==WindhawkMod==
// @id              monitor-rounded-edges
// @name            Monitor Rounded Edges
// @name:pt         Bordas Arredondadas do Monitor
// @name:es         Bordes Redondeados del Monitor
// @description     Applies smooth, anti-aliased rounded corners to all monitors via Direct2D. Supports circular and squircle (superellipse) styles, with an optional inner accent border and fullscreen awareness.
// @description:pt  Aplica cantos arredondados suaves em todos os monitores via Direct2D. Suporta estilos circular e squircle (superelipse), com borda interna opcional e suporte a tela cheia.
// @description:es  Aplica esquinas redondeadas suaves en todos los monitores via Direct2D. Soporta estilos circular y squircle (superelipse), con borde interior opcional y soporte de pantalla completa.
// @version         1.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         windhawk.exe
// @compilerOptions -lUser32 -lGdi32 -ld2d1 -lShell32 -lOle32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Monitor Rounded Edges

Applies smooth, anti-aliased rounded corners to all connected monitors using Direct2D.

![Monitor Rounded Edges](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/Monitor%20Rounded%20Edges%201.png)
![Monitor Rounded Edges](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/Monitor%20Rounded%20Edges%202.png)

## Features

- Two corner styles -- Circle (classic arc) and Squircle (smooth superellipse).
- Inner accent border -- optional outline with configurable thickness and alpha color (#AARRGGBB).
- Fullscreen awareness -- uses SHQueryUserNotificationState to reliably detect fullscreen games.
- Minimal CPU overhead -- a continuous polling timer (750 ms) is only active when "Hide in
  Fullscreen" is enabled. "Dynamic Inner Border" uses a brief transition timer (5 checks over
  1.5 s) after each foreground change to catch delayed fullscreen detection, then stops
  automatically. With neither feature active, only a lightweight WinEvent hook runs.
- Multi-monitor support -- each monitor gets its own independent set of corner overlays.
- Single tool process -- the rendering process is only created once; reopening Windhawk from the
  system tray does not spawn additional instances.

## Squircle style

The squircle uses a cubic Bezier with separate control points interpolated toward the inner arc
center (k=0.70). This produces:
- Tangents parallel to the screen edges at both endpoints (no kink, smooth transition)
- Slightly more diagonal coverage than a circular arc at the same radius
- A softer, more organic corner appearance compared to a circle

## Settings

| Setting | Description |
|---|---|
| Corner Radius | Rounding radius in pixels (1-500). |
| Corner Style | circle = standard arc; squircle = smooth superellipse. |
| Background Color | Hex fill color of the corners (#RRGGBB). |
| Inner Border Thickness | Width of the inner accent line in pixels (0 = disabled). |
| Inner Border Color | Hex color of the inner line, with alpha support (#AARRGGBB). |
| Dynamic Inner Border | Hides only the inner border in fullscreen; keeps the rounding. |
| Hide in Fullscreen | Hides everything in fullscreen apps and games. |

## Notes

The taskbar and Start menu may briefly appear above the corner overlays when they receive focus
(e.g. opening Start, the Action Center, or right-clicking the desktop). The corners return to the
top of the Z-order as soon as those elements lose focus.

Fullscreen detection via `SHQueryUserNotificationState` is **system-wide**, not per-monitor: a
fullscreen app on one monitor will affect the corners on all monitors. Additionally, `QUNS_BUSY`
and `QUNS_PRESENTATION_MODE` can be set by non-game apps (e.g. Windows Presentation Mode or apps
that request it), which may trigger hide/dynamic-border behavior unexpectedly.

---

# Bordas Arredondadas do Monitor

Aplica cantos arredondados suaves em todos os monitores conectados via Direct2D.

## Recursos

- Dois estilos de canto -- Circulo (arco classico) e Squircle (superelipse suave).
- Borda interna de destaque -- contorno opcional com espessura e cor com alpha (#AARRGGBB).
- Deteccao de tela cheia -- usa SHQueryUserNotificationState para detectar jogos em fullscreen.
- Overhead minimo de CPU -- o timer de polling continuo (750 ms) so e ativado quando "Ocultar em
  Tela Cheia" esta ligado. "Borda Interna Dinamica" usa um timer de transicao curto (5 checks em
  1,5 s) apos cada troca de foco para capturar deteccoes atrasadas, e para automaticamente. Com
  nenhum recurso ativo, apenas o WinEvent hook leve permanece em execucao.
- Suporte a multiplos monitores -- cada monitor tem seu proprio conjunto independente de overlays.
- Processo unico -- o processo de renderizacao e criado uma unica vez; reabrir o Windhawk pela
  bandeja do sistema nao cria instancias adicionais.

## Estilo Squircle

O squircle usa uma curva de Bezier cubica com pontos de controle interpolados em direcao ao centro
interno do arco (k=0,70). Isso produz:
- Tangentes paralelas as bordas da tela em ambos os extremos (sem kink, transicao suave)
- Cobertura diagonal ligeiramente maior do que um arco circular no mesmo raio
- Uma aparencia de canto mais suave e organica em comparacao ao circulo

## Configuracoes

| Configuracao | Descricao |
|---|---|
| Raio do Arredondamento | Raio do arredondamento em pixels (1-500). |
| Estilo do Canto | circle = arco padrao; squircle = superelipse suave. |
| Cor do Fundo | Cor de preenchimento dos cantos em hex (#RRGGBB). |
| Espessura da Linha Interna | Espessura da borda interna em pixels (0 para desativar). |
| Cor da Linha Interna | Cor da linha interna em hex, com suporte a alpha (#AARRGGBB). |
| Borda Interna Dinamica | Oculta apenas a borda interna em tela cheia, mantém o arredondamento. |
| Ocultar em Tela Cheia | Oculta tudo em jogos e aplicativos em tela cheia. |

## Observacoes

A barra de tarefas e o menu Iniciar podem aparecer sobre as bordas arredondadas enquanto estao
em foco (ex: ao abrir o Iniciar, a Central de Acoes ou ao clicar com o botao direito na area de
trabalho). As bordas voltam ao topo da ordem Z assim que esses elementos perdem o foco.

A deteccao de tela cheia via `SHQueryUserNotificationState` e **global** (nao por monitor): um
app em fullscreen em um monitor afeta as bordas de todos os monitores. Alem disso, `QUNS_BUSY` e
`QUNS_PRESENTATION_MODE` podem ser ativados por apps que nao sao jogos (ex: Modo de Apresentacao
do Windows), o que pode disparar o comportamento de ocultar/borda dinamica inesperadamente.

---

# Bordes Redondeados del Monitor

Aplica esquinas redondeadas suaves en todos los monitores conectados via Direct2D.

## Caracteristicas

- Dos estilos de esquina -- Circulo (arco clasico) y Squircle (superelipse suave).
- Borde interior de acento -- contorno opcional con grosor y color con alpha (#AARRGGBB).
- Deteccion de pantalla completa -- usa SHQueryUserNotificationState para detectar juegos en fullscreen.
- Overhead minimo de CPU -- el temporizador de polling continuo (750 ms) solo esta activo cuando
  "Ocultar en Pantalla Completa" esta habilitado. "Borde Interior Dinamico" usa un temporizador de
  transicion breve (5 verificaciones en 1,5 s) tras cada cambio de foco para detectar cambios
  tardios, y se detiene automaticamente. Sin ninguna funcion activa, solo el WinEvent hook ligero
  permanece en ejecucion.
- Soporte multi-monitor -- cada monitor tiene su propio conjunto independiente de overlays.
- Proceso unico -- el proceso de renderizado se crea una sola vez; reabrir Windhawk desde la
  bandeja del sistema no crea instancias adicionales.

## Estilo Squircle

El squircle usa una curva de Bezier cubica con puntos de control interpolados hacia el centro
interno del arco (k=0,70). Esto produce:
- Tangentes paralelas a los bordes de la pantalla en ambos extremos (sin kink, transicion suave)
- Cobertura diagonal ligeramente mayor que un arco circular al mismo radio
- Una apariencia de esquina mas suave y organica comparada con el circulo

## Configuracion

| Configuracion | Descripcion |
|---|---|
| Radio de las Esquinas | Radio de redondeo en pixeles (1-500). |
| Estilo de Esquina | circle = arco estandar; squircle = superelipse suave. |
| Color de Fondo | Color de relleno de las esquinas en hex (#RRGGBB). |
| Grosor del Borde Interior | Grosor del borde interior en pixeles (0 para desactivar). |
| Color del Borde Interior | Color del borde interior en hex, con soporte de alpha (#AARRGGBB). |
| Borde Interior Dinamico | Oculta solo el borde interior en pantalla completa, mantiene el redondeo. |
| Ocultar en Pantalla Completa | Oculta todo en juegos y aplicaciones a pantalla completa. |

## Notas

La barra de tareas y el menu Inicio pueden aparecer sobre las esquinas redondeadas mientras
reciben foco (ej: al abrir Inicio, el Centro de Notificaciones o al hacer clic derecho en el
escritorio). Las esquinas vuelven al frente del orden Z en cuanto esos elementos pierden el foco.

La deteccion de pantalla completa via `SHQueryUserNotificationState` es **global** (no por
monitor): una app en pantalla completa en un monitor afecta las esquinas de todos los monitores.
Ademas, `QUNS_BUSY` y `QUNS_PRESENTATION_MODE` pueden ser activados por apps que no son juegos
(ej: Modo de Presentacion de Windows), lo que puede disparar el comportamiento de ocultar/borde
dinamico de forma inesperada.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- radius: 10
  $name: Corner Radius
  $name:pt: Raio do Arredondamento
  $name:es: Radio de las Esquinas
  $description: "Corner rounding radius in pixels. Maximum: 500."
  $description:pt: "O raio (em pixels) para o arredondamento dos cantos. Maximo: 500."
  $description:es: "Radio de redondeo de esquinas en pixeles. Maximo: 500."
- squircle: false
  $name: Squircle Style
  $name:pt: Estilo Squircle
  $name:es: Estilo Squircle
  $description: "When enabled, uses a smooth superellipse (squircle) instead of a circular arc. Slightly softer appearance at the same radius."
  $description:pt: "Se ativo, usa superelipse (squircle) no lugar do arco circular. Aparencia levemente mais suave no mesmo raio."
  $description:es: "Cuando esta activo, usa una superelipse suave (squircle) en lugar del arco circular."
- color: "#000000"
  $name: Background Color
  $name:pt: Cor do Fundo
  $name:es: Color de Fondo
  $description: "Hex fill color of the corners (#RRGGBB)."
  $description:pt: "Codigo Hex da cor dos cantos (#RRGGBB)."
  $description:es: "Color Hex de relleno de las esquinas (#RRGGBB)."
- border_thickness: 1
  $name: Inner Border Thickness
  $name:pt: Espessura da Linha Interna
  $name:es: Grosor del Borde Interior
  $description: "Inner border thickness in pixels (0 to disable)."
  $description:pt: "Espessura (pixels) do contorno interno (0 para desativar)."
  $description:es: "Grosor del borde interior en pixeles (0 para desactivar)."
- border_color: "#33FFFFFF"
  $name: Inner Border Color
  $name:pt: Cor da Linha Interna
  $name:es: Color del Borde Interior
  $description: "Hex color of the inner border. Supports alpha: #AARRGGBB."
  $description:pt: "Codigo Hex da linha interna. Suporta alpha: #AARRGGBB."
  $description:es: "Color Hex del borde interior. Soporta alpha: #AARRGGBB."
- dynamic_border: false
  $name: Dynamic Inner Border
  $name:pt: Borda Interna Dinamica
  $name:es: Borde Interior Dinamico
  $description: "When enabled, hides only the inner border in fullscreen while keeping rounding."
  $description:pt: "Se ativo, oculta apenas a borda interna em tela cheia, mas mantém o arredondamento."
  $description:es: "Cuando esta activo, oculta solo el borde interior en pantalla completa."
- disable_fullscreen: true
  $name: Hide in Fullscreen
  $name:pt: Ocultar Mod em Tela Cheia
  $name:es: Ocultar en Pantalla Completa
  $description: "Hides everything in fullscreen apps and games. The polling timer is only active when this option is enabled. Note: always-on-top layered overlays may cause exclusive (flip-model) fullscreen games to flicker or drop out of exclusive mode; enable this option if that occurs."
  $description:pt: "Oculta TUDO em jogos e aplicativos em tela cheia. O timer de polling so e ativo quando esta opcao estiver ligada. Aviso: overlays sempre no topo podem causar flickering em jogos fullscreen exclusivo (flip-model); ative esta opcao se isso ocorrer."
  $description:es: "Oculta todo en aplicaciones y juegos a pantalla completa. El temporizador solo esta activo cuando esta opcion esta habilitada. Nota: los overlays siempre visibles pueden causar parpadeo en juegos fullscreen exclusivo (flip-model); activa esta opcion si ocurre."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <d2d1.h>
#include <wrl/client.h>
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>
#include <windhawk_utils.h>

using namespace Microsoft::WRL;

// ---------------------------------------------------------------------------
// Constantes
// ---------------------------------------------------------------------------

// Fator de interpolacao do squircle: CP = lerp(endpoint -> pC, k).
// k=0       -> sem curva (linha reta)
// k=0.5523  -> aproximacao do circulo
// k=0.70    -> squircle: midpoint em ~r*(4-2.1)/8 = 0.2375r, mais proximo de pC
//              que o circulo (0.293r), curva mais fechada, efeito squircle visivel
static constexpr float k_squircle = 0.70f;

// ---------------------------------------------------------------------------
// Estruturas e Globais
// ---------------------------------------------------------------------------

struct ModSettings {
    int          radius;
    bool         squircle;
    D2D1_COLOR_F bgColor;
    float        borderThickness;
    D2D1_COLOR_F borderColor;
    bool         dynamicBorder;
    bool         disableFullscreen;
} g_cfg;

// Armazena HWND + metadados de cada overlay para re-renderizar sem destruir.
struct EdgeWindow {
    HWND    hwnd    = NULL;
    int     side    = 0;   // 0=topo 1=rodape 2=esquerda 3=direita
    int     w       = 0;
    int     h       = 0;
    // Cache GDI: criados uma vez em CreateEdge, destruidos em SyncEdges.
    HDC     hdcMem  = NULL;  // DC em memoria com hBmp permanentemente selecionado
    HBITMAP hBmp    = NULL;  // DIB 32bpp (w*h*4 bytes) onde o D2D escreve
    // Cache D2D: geometrias de preenchimento, independentes de bt.
    // Validas enquanto radius/squircle/dimensoes nao mudam (ou seja, entre SyncEdges).
    ComPtr<ID2D1PathGeometry> fillGeoLeft;   // canto esquerdo (side 0/1 apenas)
    ComPtr<ID2D1PathGeometry> fillGeoRight;  // canto direito  (side 0/1 apenas)
};

HWND                    g_hwndMain             = NULL;
HANDLE                  g_hThread              = NULL;
HWINEVENTHOOK           g_hHookFG              = NULL;
std::atomic<bool>       g_pendingReassert      {false};
std::vector<EdgeWindow> g_edgeWindows;
bool                    g_isHidden             = false;
bool                    g_isAppFullscreen      = false;
int                     g_dynBorderTicks       = 0;

ComPtr<ID2D1Factory>         g_pFactory;
ComPtr<ID2D1DCRenderTarget>  g_pRT;
ComPtr<ID2D1SolidColorBrush> g_pBrushBG;
ComPtr<ID2D1SolidColorBrush> g_pBrushBorder;

// IDs de mensagem interna
static constexpr UINT WM_SETTINGS_CHANGED   = WM_APP + 1;
static constexpr UINT WM_FOREGROUND_CHANGED = WM_APP + 2;

// IDs de timer
static constexpr UINT TIMER_POLLING    = 1;  // disable_fullscreen: polling continuo 750ms
static constexpr UINT TIMER_TRANSITION = 2;  // dynamicBorder: curto pos-transicao de foreground

// ---------------------------------------------------------------------------
// Utilitarios
// ---------------------------------------------------------------------------

/// @brief Converte uma string hex para D2D1_COLOR_F.
///        Formatos suportados: #RRGGBB ou #AARRGGBB (com '#' opcional).
/// @param hex  Ponteiro para a string de cor.
/// @return     D2D1_COLOR_F correspondente, ou preto opaco em caso de erro.
static D2D1_COLOR_F HexToColor(PCWSTR hex) {
    if (!hex || hex[0] == L'\0') return D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);

    std::wstring s = (hex[0] == L'#') ? (hex + 1) : hex;
    try {
        unsigned long c = std::stoul(s, nullptr, 16);

        if (s.length() == 6) {
            // Formato RRGGBB -- extrair por shift, NAO usar GetRValue/GetBValue:
            // essas macros esperam COLORREF (0x00BBGGRR), nao 0x00RRGGBB.
            return D2D1::ColorF(
                ((c >> 16) & 0xFF) / 255.0f,
                ((c >>  8) & 0xFF) / 255.0f,
                ( c        & 0xFF) / 255.0f,
                1.0f
            );
        }
        if (s.length() == 8) {
            // Formato AARRGGBB
            return D2D1::ColorF(
                ((c >> 16) & 0xFF) / 255.0f,
                ((c >>  8) & 0xFF) / 255.0f,
                ( c        & 0xFF) / 255.0f,
                ((c >> 24) & 0xFF) / 255.0f
            );
        }
    } catch (...) {}

    return D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
}

static void LoadSettings() {
    g_cfg.radius   = (std::max)(1, (std::min)(500, Wh_GetIntSetting(L"radius")));
    g_cfg.squircle = Wh_GetIntSetting(L"squircle") != 0;

    g_cfg.bgColor = HexToColor(WindhawkUtils::StringSetting::make(L"color"));

    g_cfg.borderThickness = (float)(std::max)(0, Wh_GetIntSetting(L"border_thickness"));

    g_cfg.borderColor = HexToColor(WindhawkUtils::StringSetting::make(L"border_color"));

    g_cfg.dynamicBorder     = Wh_GetIntSetting(L"dynamic_border")    != 0;
    g_cfg.disableFullscreen = Wh_GetIntSetting(L"disable_fullscreen") != 0;
}

// ---------------------------------------------------------------------------
// Deteccao de Tela Cheia
// ---------------------------------------------------------------------------

/// @brief Verifica se o sistema esta em estado de tela cheia real.
///
///        Usa SHQueryUserNotificationState -- API do shell do Windows criada
///        para distinguir jogos/apps fullscreen de overlays do sistema.
///        Requer COM inicializado no thread chamador (CoInitializeEx).
///
/// @return true se um jogo ou app esta em tela cheia.
static bool CheckFullscreen() {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_NOT_PRESENT;
    SHQueryUserNotificationState(&state);
    return state == QUNS_RUNNING_D3D_FULL_SCREEN
        || state == QUNS_PRESENTATION_MODE
        || state == QUNS_BUSY;
}

// ---------------------------------------------------------------------------
// Renderizacao Direct2D
// ---------------------------------------------------------------------------

static void ReleaseResources() {
    g_pBrushBorder.Reset();
    g_pBrushBG.Reset();
    g_pRT.Reset();
}

/// @brief Garante que o RenderTarget e os brushes estao validos.
///        Em caso de falha parcial, desfaz tudo para estado limpo.
/// @return S_OK se pronto, codigo de erro caso contrario.
static HRESULT EnsureResources() {
    if (g_pRT) return S_OK;

    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );

    HRESULT hr = g_pFactory->CreateDCRenderTarget(&props, &g_pRT);
    if (FAILED(hr)) return hr;

    // Persistente no render target: nao precisa ser repetido a cada BeginDraw.
    g_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    hr = g_pRT->CreateSolidColorBrush(g_cfg.bgColor, &g_pBrushBG);
    if (FAILED(hr)) { ReleaseResources(); return hr; }

    hr = g_pRT->CreateSolidColorBrush(g_cfg.borderColor, &g_pBrushBorder);
    if (FAILED(hr)) { ReleaseResources(); return hr; }

    return S_OK;
}

/// @brief Calcula o primeiro ponto de controle do Bezier squircle.
///
///        CP1 = lerp(p1 -> pC, k).
///        B'(0) = 3*(CP1-p1) = 3k*(pC-p1) -> paralelo a borda de p1 (sem kink).
///        A curva parte de p1 em direcao a pC (canto externo), criando a
///        concavidade correta para o mascaramento do canto da tela.
///
/// @param p1  Ponto de inicio da curva.
/// @param pC  Canto externo (= canto fisico da tela).
/// @return    CP1 para o Bezier squircle.
static D2D1_POINT_2F SqCP1(D2D1_POINT_2F p1, D2D1_POINT_2F pC) {
    return { p1.x + k_squircle * (pC.x - p1.x), p1.y + k_squircle * (pC.y - p1.y) };
}

/// @brief Calcula o segundo ponto de controle do Bezier squircle.
///
///        CP2 = lerp(p2 -> pC, k).
///        B'(1) = 3*(p2-CP2) = 3k*(p2-pC) -> paralelo a borda de p2 (sem kink).
///
/// @param p2  Ponto de fim da curva.
/// @param pC  Canto externo (= canto fisico da tela).
/// @return    CP2 para o Bezier squircle.
static D2D1_POINT_2F SqCP2(D2D1_POINT_2F p2, D2D1_POINT_2F pC) {
    return { p2.x + k_squircle * (pC.x - p2.x), p2.y + k_squircle * (pC.y - p2.y) };
}

/// @brief Constroi a geometria de preenchimento de um canto para armazenar em cache.
///        Depende apenas de dimensoes fixas e configuracoes estruturais (raio, squircle);
///        nao e afetada por bt (border thickness), que varia no dynamicBorder.
static ComPtr<ID2D1PathGeometry> BuildFillGeometry(
    int w, int h, float r, int side, bool isRight)
{
    bool isTop = (side == 0);
    D2D1_POINT_2F pC = { isRight ? (float)w : 0.0f,  isTop ? 0.0f : r    };
    D2D1_POINT_2F p1 = { isRight ? (float)w - r : r, isTop ? 0.0f : r    };
    D2D1_POINT_2F p2 = { isRight ? (float)w : 0.0f,  isTop ? r    : 0.0f };
    D2D1_SWEEP_DIRECTION dir = isTop
        ? (isRight ? D2D1_SWEEP_DIRECTION_CLOCKWISE         : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE)
        : (isRight ? D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE : D2D1_SWEEP_DIRECTION_CLOCKWISE);

    ComPtr<ID2D1PathGeometry> pg;
    if (FAILED(g_pFactory->CreatePathGeometry(&pg))) return nullptr;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(pg->Open(&sink))) return nullptr;
    sink->BeginFigure(pC, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(p1);
    if (!g_cfg.squircle) {
        sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(r, r), 0.0f, dir, D2D1_ARC_SIZE_SMALL));
    } else {
        sink->AddBezier(D2D1::BezierSegment(SqCP1(p1, pC), SqCP2(p2, pC), p2));
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    return pg;
}

/// @brief Renderiza e aplica o overlay de borda em uma janela layered.
///        Usa o DC e o DIB em cache do EdgeWindow; sem alocacoes GDI por chamada.
static void RenderEdge(EdgeWindow& ew) {
    if (FAILED(EnsureResources())) return;
    if (!ew.hdcMem) return;

    RECT rb = {0, 0, ew.w, ew.h};
    g_pRT->BindDC(ew.hdcMem, &rb);
    g_pRT->BeginDraw();
    g_pRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

    float r    = (float)g_cfg.radius;
    float bt   = (g_cfg.dynamicBorder && g_isAppFullscreen) ? 0.0f : g_cfg.borderThickness;
    float half = bt / 2.0f;

    if (ew.side == 0 || ew.side == 1) {
        bool isTop = (ew.side == 0);

        // Preenchimento: geometrias em cache (nao dependem de bt).
        if (ew.fillGeoLeft)  g_pRT->FillGeometry(ew.fillGeoLeft.Get(),  g_pBrushBG.Get());
        if (ew.fillGeoRight) g_pRT->FillGeometry(ew.fillGeoRight.Get(), g_pBrushBG.Get());

        // Borda: recriada a cada render pois depende de bt (dinamico no dynamicBorder).
        if (bt > 0.0f) {
            auto DrawBorderCorner = [&](bool isRight) {
                // pC necessario para SqCP1/SqCP2 no modo squircle.
                D2D1_POINT_2F pC = { isRight ? (float)ew.w : 0.0f,  isTop ? 0.0f : r    };
                D2D1_POINT_2F p1 = { isRight ? (float)ew.w - r : r, isTop ? 0.0f : r    };
                D2D1_POINT_2F p2 = { isRight ? (float)ew.w : 0.0f,  isTop ? r    : 0.0f };
                D2D1_SWEEP_DIRECTION dir = isTop
                    ? (isRight ? D2D1_SWEEP_DIRECTION_CLOCKWISE         : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE)
                    : (isRight ? D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE : D2D1_SWEEP_DIRECTION_CLOCKWISE);

                D2D1_POINT_2F bp1 = p1;
                D2D1_POINT_2F bp2 = p2;
                if (isTop) bp1.y += half; else bp1.y -= half;
                bp2.x += (isRight ? -half : half);

                ComPtr<ID2D1PathGeometry> bpg;
                g_pFactory->CreatePathGeometry(&bpg);
                ComPtr<ID2D1GeometrySink> bsink;
                bpg->Open(&bsink);
                bsink->BeginFigure(bp1, D2D1_FIGURE_BEGIN_HOLLOW);
                if (!g_cfg.squircle) {
                    bsink->AddArc(D2D1::ArcSegment(bp2, D2D1::SizeF(r - half, r - half), 0.0f, dir, D2D1_ARC_SIZE_SMALL));
                } else {
                    bsink->AddBezier(D2D1::BezierSegment(SqCP1(bp1, pC), SqCP2(bp2, pC), bp2));
                }
                bsink->EndFigure(D2D1_FIGURE_END_OPEN);
                bsink->Close();
                g_pRT->DrawGeometry(bpg.Get(), g_pBrushBorder.Get(), bt);
            };

            DrawBorderCorner(false);
            DrawBorderCorner(true);

            float ly = isTop ? half : (r - half);
            g_pRT->DrawLine({r, ly}, {(float)ew.w - r, ly}, g_pBrushBorder.Get(), bt);
        }

    } else if (bt > 0.0f) {
        float lx = (ew.side == 2) ? half : (r - half);
        g_pRT->DrawLine({lx, r}, {lx, (float)ew.h - r}, g_pBrushBorder.Get(), bt);
    }

    HRESULT hr = g_pRT->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) ReleaseResources();

    if (SUCCEEDED(hr)) {
        SIZE          sz = {ew.w, ew.h};
        POINT         ps = {0, 0};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        // hdcDst=NULL: usa screen DC interna. pptDst=NULL: janela nao se move.
        UpdateLayeredWindow(ew.hwnd, NULL, NULL, &sz, ew.hdcMem, &ps, 0, &bf, ULW_ALPHA);
    }
}

// ---------------------------------------------------------------------------
// Sistema -- Janelas de Borda
// ---------------------------------------------------------------------------

static BOOL CALLBACK EnumMons(HMONITOR, HDC, LPRECT lr, LPARAM dw) {
    reinterpret_cast<std::vector<RECT>*>(dw)->push_back(*lr);
    return TRUE;
}

/// @brief Re-renderiza os overlays existentes sem destrui-los.
///        Chamada quando apenas o conteudo visual muda (dynamic_border),
///        evitando o flicker causado por destroy/recreate.
static void RefreshEdges() {
    for (auto& ew : g_edgeWindows)
        RenderEdge(ew);
}

/// @brief Destroi e recria todas as janelas de overlay.
///        Chamada apenas quando configuracoes estruturais mudam (raio, cor, estilo).
static void SyncEdges() {
    for (auto& ew : g_edgeWindows) {
        if (ew.hdcMem) DeleteDC(ew.hdcMem);
        if (ew.hBmp)   DeleteObject(ew.hBmp);
        DestroyWindow(ew.hwnd);
    }
    g_edgeWindows.clear();

    std::vector<RECT> monitors;
    EnumDisplayMonitors(NULL, NULL, EnumMons, reinterpret_cast<LPARAM>(&monitors));

    for (const auto& rc : monitors) {
        int mw = rc.right  - rc.left;
        int mh = rc.bottom - rc.top;

        auto CreateEdge = [&](int ex, int ey, int ew, int eh, int side) {
            HWND hwn = CreateWindowEx(
                WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                L"WindhawkCorner", L"", WS_POPUP,
                ex, ey, ew, eh,
                NULL, NULL, GetModuleHandle(NULL), NULL
            );
            if (!hwn) return;

            BITMAPINFO bmi              = {};
            bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth       = ew;
            bmi.bmiHeader.biHeight      = -eh;  // top-down
            bmi.bmiHeader.biPlanes      = 1;
            bmi.bmiHeader.biBitCount    = 32;
            bmi.bmiHeader.biCompression = BI_RGB;

            void*   bits = nullptr;
            HBITMAP hBmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
            if (!hBmp) { DestroyWindow(hwn); return; }

            HDC hdcMem = CreateCompatibleDC(NULL);
            if (!hdcMem) { DeleteObject(hBmp); DestroyWindow(hwn); return; }
            SelectObject(hdcMem, hBmp);

            EdgeWindow edgeWin;
            edgeWin.hwnd   = hwn;
            edgeWin.side   = side;
            edgeWin.w      = ew;
            edgeWin.h      = eh;
            edgeWin.hdcMem = hdcMem;
            edgeWin.hBmp   = hBmp;

            if (side == 0 || side == 1) {
                float r = (float)g_cfg.radius;
                edgeWin.fillGeoLeft  = BuildFillGeometry(ew, eh, r, side, false);
                edgeWin.fillGeoRight = BuildFillGeometry(ew, eh, r, side, true);
            }

            RenderEdge(edgeWin);
            if (!g_isHidden) ShowWindow(hwn, SW_SHOWNOACTIVATE);
            g_edgeWindows.push_back(std::move(edgeWin));
        };

        // Topo (0), Rodape (1), Esquerda (2), Direita (3)
        CreateEdge(rc.left,                  rc.top,                   mw,           g_cfg.radius, 0);
        CreateEdge(rc.left,                  rc.bottom - g_cfg.radius, mw,           g_cfg.radius, 1);
        CreateEdge(rc.left,                  rc.top,                   g_cfg.radius, mh,           2);
        CreateEdge(rc.right  - g_cfg.radius, rc.top,                   g_cfg.radius, mh,           3);
    }
}

/// @brief Re-asserta WS_EX_TOPMOST em todos os overlays visiveis.
///        Chamada quando uma janela topmost pode ter assumido o topo da pilha Z.
static void ReassertTopmost() {
    if (g_isHidden) return;
    for (auto& ew : g_edgeWindows)
        SetWindowPos(ew.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// ---------------------------------------------------------------------------
// Logica de Fullscreen
// ---------------------------------------------------------------------------

/// @brief Avalia o estado de fullscreen e aplica as acoes correspondentes.
static void ApplyFullscreenState() {
    bool currentFS = CheckFullscreen();

    if (currentFS != g_isAppFullscreen) {
        g_isAppFullscreen = currentFS;
        if (g_cfg.dynamicBorder) {
            RefreshEdges();
        }
    }

    bool shouldHide = g_cfg.disableFullscreen && g_isAppFullscreen;
    if (shouldHide != g_isHidden) {
        g_isHidden = shouldHide;
        for (auto& ew : g_edgeWindows)
            ShowWindow(ew.hwnd, g_isHidden ? SW_HIDE : SW_SHOWNOACTIVATE);
    }
}

// ---------------------------------------------------------------------------
// WinEvent Hook
// ---------------------------------------------------------------------------

/// @brief Callback de mudanca de janela em primeiro plano.
///        Com WINEVENT_OUTOFCONTEXT: postado na fila do ModThread --
///        sem injecao de DLL, custo zero entre eventos.
static void CALLBACK HookForeground(
    HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD)
{
    // Coalescing: so uma mensagem pendente por vez. exchange(true) retorna o valor
    // anterior; se ja era true, ha uma mensagem na fila e nao precisamos postar outra.
    if (g_hwndMain && !g_pendingReassert.exchange(true))
        PostMessage(g_hwndMain, WM_FOREGROUND_CHANGED, 0, 0);
}

/// @brief Instala o WinEvent hook e o timer de polling de acordo com as
///        configuracoes ativas.
///
///        Hook (EVENT_SYSTEM_FOREGROUND): SEMPRE ativo.
///        - Re-asserta TOPMOST quando outra janela assume o foco.
///        - Aciona ApplyFullscreenState quando funcionalidades de fullscreen ativas.
///        - Custo zero entre eventos (WINEVENT_OUTOFCONTEXT).
///
///        Timer: SOMENTE quando disable_fullscreen = true.
///        - Detecta jogos que entram em fullscreen sem mudar o foreground.
///        - Quando inativo: zero overhead periodico.
///
/// @param hw  Handle da janela de mensagens.
static void UpdateHookAndTimer(HWND hw) {
    if (!g_hHookFG) {
        g_hHookFG = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            NULL, HookForeground, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
        );
    }

    if (g_cfg.disableFullscreen) {
        SetTimer(hw, TIMER_POLLING, 750, NULL);
    } else {
        KillTimer(hw, TIMER_POLLING);
    }
}

// ---------------------------------------------------------------------------
// Message Loop Principal (ModThread)
// ---------------------------------------------------------------------------

static LRESULT CALLBACK MasterWndProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_SETTINGS_CHANGED:
        LoadSettings();
        ReleaseResources();

        // Resetar estado antes de SyncEdges: garante que os novos overlays
        // sejam criados visiveis. ApplyFullscreenState reavalia logo apos.
        g_isHidden        = false;
        g_isAppFullscreen = false;

        KillTimer(hw, TIMER_TRANSITION);
        g_dynBorderTicks = 0;
        SyncEdges();
        ApplyFullscreenState();
        UpdateHookAndTimer(hw);
        return 0;

    case WM_FOREGROUND_CHANGED:
        g_pendingReassert.store(false);
        if (g_cfg.disableFullscreen || g_cfg.dynamicBorder) {
            ApplyFullscreenState();
        }
        ReassertTopmost();
        // Timer de transicao: cobre o delay do SHQueryUserNotificationState apos
        // troca de foreground, sem polling continuo quando nenhum jogo esta rodando.
        // Nao necessario quando disableFullscreen ja tem TIMER_POLLING ativo (750ms).
        if (g_cfg.dynamicBorder && !g_cfg.disableFullscreen) {
            g_dynBorderTicks = 5;
            SetTimer(hw, TIMER_TRANSITION, 300, NULL);
        }
        return 0;

    case WM_TIMER:
        if (wp == TIMER_TRANSITION) {
            ApplyFullscreenState();
            ReassertTopmost();
            if (--g_dynBorderTicks <= 0) KillTimer(hw, TIMER_TRANSITION);
        } else {
            // TIMER_POLLING: ativo somente quando disable_fullscreen = true.
            ApplyFullscreenState();
            ReassertTopmost();
        }
        return 0;

    case WM_DISPLAYCHANGE:
        // Monitor conectado/desconectado ou resolucao alterada.
        SyncEdges();
        ApplyFullscreenState();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hw, msg, wp, lp);
    }
}

static DWORD WINAPI ModThread(LPVOID) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // SHQueryUserNotificationState requer COM inicializado no thread chamador.
    // S_FALSE = ja inicializado neste thread, mas ainda incrementa refcount --
    // CoUninitialize deve ser chamado em S_OK e S_FALSE, mas NAO em caso de erro.
    const HRESULT hrCom = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory),
        nullptr,
        reinterpret_cast<void**>(g_pFactory.GetAddressOf())
    );
    if (FAILED(hr)) {
        if (SUCCEEDED(hrCom)) CoUninitialize();
        return 1;
    }

    WNDCLASS wc      = {};
    wc.lpfnWndProc   = MasterWndProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkMaster";
    RegisterClass(&wc);

    WNDCLASS cc      = {};
    cc.lpfnWndProc   = DefWindowProc;
    cc.hInstance     = GetModuleHandle(NULL);
    cc.lpszClassName = L"WindhawkCorner";
    RegisterClass(&cc);

    // Top-level oculta (nao message-only): necessario para receber WM_DISPLAYCHANGE,
    // que e enviado via broadcast e nao chega a janelas message-only (HWND_MESSAGE).
    g_hwndMain = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"WindhawkMaster", L"Master", WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, wc.hInstance, NULL
    );

    if (g_hwndMain) {
        LoadSettings();
        SyncEdges();
        ApplyFullscreenState();
        UpdateHookAndTimer(g_hwndMain);

        MSG m = {};
        while (GetMessage(&m, NULL, 0, 0)) DispatchMessage(&m);

        // Limpeza ordenada apos WM_DESTROY -> PostQuitMessage -> GetMessage = FALSE.
        if (g_hHookFG) {
            UnhookWinEvent(g_hHookFG);
            g_hHookFG = NULL;
        }
        KillTimer(g_hwndMain, TIMER_POLLING);
        KillTimer(g_hwndMain, TIMER_TRANSITION);
        for (auto& ew : g_edgeWindows) {
            if (ew.hdcMem) DeleteDC(ew.hdcMem);
            if (ew.hBmp)   DeleteObject(ew.hBmp);
            DestroyWindow(ew.hwnd);
        }
        g_edgeWindows.clear();
    }

    ReleaseResources();
    g_pFactory.Reset();
    if (SUCCEEDED(hrCom)) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Ponto de Entrada Windhawk
// ---------------------------------------------------------------------------

static BOOL WhTool_ModInit() {
    g_hThread = CreateThread(NULL, 0, ModThread, NULL, 0, NULL);
    return (g_hThread != NULL) ? TRUE : FALSE;
}

static void WhTool_ModSettingsChanged() {
    if (g_hwndMain) PostMessage(g_hwndMain, WM_SETTINGS_CHANGED, 0, 0);
}

static void WhTool_ModUninit() {
    if (g_hwndMain) {
        PostMessage(g_hwndMain, WM_CLOSE, 0, 0);
        g_hwndMain = NULL;
    }
    if (g_hThread) {
        // TerminateThread removido: Wh_ModUninit chama ExitProcess(0) logo apos,
        // portanto um timeout aqui e inofensivo -- o processo sera encerrado de qualquer forma.
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
