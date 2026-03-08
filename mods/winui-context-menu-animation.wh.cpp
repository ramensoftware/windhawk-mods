// ==WindhawkMod==
// @id              winui-context-menu-animation
// @name            WinUI Context Menu Animation
// @name:pt         Menus de Contexto com Animação WinUI
// @name:es         Menús contextuales con animación WinUI
// @description     Adds smooth WinUI-style vertical slide animations to classic Win32 context menus in Windows 11
// @description:pt  Adiciona animações suaves de deslizamento vertical no estilo WinUI aos menus de contexto Win32 clássicos no Windows 11
// @description:es  Agrega animaciones de deslizamiento verticales suaves al estilo WinUI a los menús contextuales clásicos de Win32 en Windows 11
// @version         1.0.1
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         *
// @compilerOptions -lgdi32 -lcomctl32 -lmsimg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- animationDuration: 320
  $name: Animation duration (ms)
  $name:pt: Duração da animação (ms)
  $name:es: Duración de la animación (ms)
  $description: Total duration of the menu opening animation in milliseconds
  $description:pt: Tempo total da animação de abertura do menu em milissegundos
  $description:es: Tempo total de la animación de apertura del menú en milissegundos

- fadeDuration: 80
  $name: Fade-in duration (%)
  $name:pt: Duração do fade-in (%)
  $name:es: Duración del desvanecimiento (%)
  $description: Percentage of the animation used for the fade-in effect (0 = no fade, 100 = fade for the entire animation)
  $description:pt: Porcentagem da animação usada para o efeito de fade-in (0 = sem fade, 100 = fade durante toda a animação)
  $description:es: Porcentaje de animación utilizada para el efecto de aparición gradual (0 = sin desvanecimiento, 100 = desvanecimiento durante toda la animación)

- timerInterval: 6
  $name: Frame interval (ms)
  $name:pt: Intervalo de frame (ms)
  $name:es: Intervalo de fotograma (ms)
  $description: "Interval in milliseconds between animation frames. Recommended: 6 for 120 Hz or higher displays, 16 for 60 Hz displays. Lower values produce smoother motion but increase CPU usage, as each frame composites a bitmap per frame (via UpdateLayeredWindow in default mode, or via AlphaBlend in compatibility mode)."
  $description:pt: "Intervalo em milissegundos entre os frames da animação. Recomendado: 6 para telas de 120 Hz ou superior, 16 para telas de 60 Hz. Valores menores produzem movimento mais suave, mas aumentam o uso de CPU, pois cada frame compõe um bitmap por frame (via UpdateLayeredWindow no modo padrão, ou via AlphaBlend no modo de compatibilidade)."
  $description:es: "Intervalo en milisegundos entre fotogramas de animación. Recomendado: 6 para pantallas de 120 Hz o superiores, 16 para pantallas de 60 Hz. Valores menores producen un movimiento más suave, pero aumentan el uso de CPU, ya que cada fotograma compone un mapa de bits por fotograma (mediante UpdateLayeredWindow en el modo predeterminado, o mediante AlphaBlend en el modo de compatibilidad)."

- translucentWindowsCompat: false
  $name: Translucent Windows compatibility mode
  $name:pt: Modo de compatibilidade com Translucent Windows
  $name:es: Modo de compatibilidad con Translucent Windows
  $description: Enable if you use the Translucent Windows mod. Reverts to the legacy compositing path (AlphaBlend), disabling per-pixel alpha rendering and background styling changes. This restores the behavior of version 1.0.
  $description:pt: Ative se você usa o mod Translucent Windows. Reverte para o caminho de composição legado (AlphaBlend), desativando a renderização alfa por pixel e as alterações de estilo de fundo. Isso restaura o comportamento da versão 1.0.
  $description:es: Actívalo si usas el mod Translucent Windows. Revierte al camino de composición heredado (AlphaBlend), desactivando la composición alfa por píxel y los cambios de estilo de fondo. Esto restaura el comportamiento de la versión 1.0.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
![Preview](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/Menu%20de%20Contexto%20com%20Anima%C3%A7%C3%A3o%20WinUI.gif)
![Preview](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/Menu%20de%20Contexto%202%20com%20Anima%C3%A7%C3%A3o%20WinUI.gif)
# WinUI Context Menu Animation

## English

This mod brings the smooth vertical slide animation used in modern **WinUI applications**
to classic **Win32 context menus and popup menus** in Windows 11.

Instead of appearing instantly, menus smoothly slide into view using a **WinUI-style
cubic-bezier easing curve** combined with a subtle fade-in.

Each menu and submenu independently determines whether it should animate **upward or
downward** based on its actual position on the screen. This ensures consistent behavior
with how Windows naturally flips menus near screen edges.

### Features

• Smooth WinUI-style vertical slide animation  
• Configurable animation and fade times  
• Accurate cubic-bezier easing matching modern Windows apps  
• Independent direction detection for menus and submenus  
• Compatible with context menus, menu bars, and nested submenus  
• Lightweight implementation using native Win32 hooks  
• Optional compatibility mode for the Translucent Windows mod

### Translucent Windows Compatibility

If you use the **Translucent Windows** mod alongside this one, enable the
**Translucent Windows compatibility mode** setting. This reverts the animation
rendering to the legacy compositing path (AlphaBlend), disabling per-pixel alpha
transparency and background styling changes. The slide and fade animations remain
fully functional — only the compositing method changes.

### Credits

This mod is based on the excellent work by **aubymori**:

Menu/Tooltip Slide Animation  
https://windhawk.net/mods/menu-tooltip-slide-animation

The original mod restored the classic Windows 98/XP menu animation system.
This fork adapts the animation engine to replicate the **modern WinUI motion style**
used throughout Windows 11.

Changes in this version include:

• Replacing the classic animation with a WinUI-style vertical easing motion  
• Removing tooltip animation logic  
• Removing horizontal animation  
• Improving submenu direction detection  
• Implementing an accurate cubic-bezier solver for smooth motion  
• (1.0.1) Migrating to per-pixel alpha compositing via UpdateLayeredWindow and added Translucent Windows compatibility mode

---

## Português (Brasil)

Este mod traz a animação suave de **deslizamento vertical usada em aplicativos
modernos com WinUI** para os **menus de contexto e menus popup clássicos do Win32**
no Windows 11.

Em vez de aparecerem instantaneamente, os menus passam a surgir com um movimento
suave utilizando uma **curva de easing cubic-bezier semelhante à do WinUI**,
acompanhada por um leve efeito de fade-in.

Cada menu e submenu determina **de forma independente** se deve animar para cima
ou para baixo com base em sua posição real na tela. Isso garante um comportamento
natural quando o menu precisa inverter sua direção próximo às bordas da tela.

### Recursos

• Animação vertical suave inspirada no WinUI  
• Tempo de animação e fade configuráveis  
• Curva de easing cubic-bezier equivalente à usada no Windows moderno  
• Detecção independente de direção para menus e submenus  
• Compatível com menus de contexto, barras de menu e submenus aninhados  
• Implementação leve baseada em hooks nativos do Win32  
• Modo de compatibilidade opcional para o mod Translucent Windows

### Compatibilidade com Translucent Windows

Se você usa o mod **Translucent Windows** junto com este, ative a configuração
**Modo de compatibilidade com Translucent Windows**. Isso reverte a renderização
da animação para o caminho de composição legado (AlphaBlend), desativando a
transparência alfa por pixel e as alterações de estilo de fundo. As animações
de slide e fade continuam funcionando normalmente — apenas o método de composição
muda.

### Créditos

Este mod é baseado no trabalho de **aubymori**:

Menu/Tooltip Slide Animation  
https://windhawk.net/mods/menu-tooltip-slide-animation

O mod original restaurava as animações clássicas de menus do Windows 98/XP.
Esta versão adapta o mecanismo de animação para reproduzir o **estilo de
movimento moderno do WinUI presente no Windows 11**.

Principais mudanças nesta versão:

• Substituição da animação clássica por movimento vertical estilo WinUI  
• Remoção da animação de tooltips  
• Remoção da animação horizontal  
• Melhoria na detecção de direção de submenus  
• Implementação de um solver cubic-bezier preciso para animação suave  
• (1.0.1) Migração para composição alfa por pixel via UpdateLayeredWindow e adição do modo de compatibilidade com Translucent Windows

---

## Español

Este mod añade la animación vertical suave utilizada en las **aplicaciones modernas
basadas en WinUI** a los **menús de contexto y menús emergentes clásicos de Win32**
en Windows 11.

En lugar de aparecer instantáneamente, los menús se deslizan suavemente utilizando
una **curva de easing cubic-bezier similar a la del WinUI**, combinada con un
ligero efecto de aparición gradual (fade-in).

Cada menú y submenú determina **de forma independiente** si debe animarse hacia
arriba o hacia abajo según su posición real en la pantalla, manteniendo un
comportamiento natural cuando Windows invierte la dirección cerca de los bordes.

### Características

• Animación vertical suave inspirada en WinUI  
• Tiempos de animación y desvanecimiento configurables  
• Curva de easing cubic-bezier equivalente a la usada en Windows moderno  
• Detección independiente de dirección para menús y submenús  
• Compatible con menús de contexto, barras de menú y submenús anidados  
• Implementación ligera basada en hooks nativos de Win32  
• Modo de compatibilidad opcional para el mod Translucent Windows

### Compatibilidad con Translucent Windows

Si usas el mod **Translucent Windows** junto con este, activa la opción
**Modo de compatibilidad con Translucent Windows**. Esto revierte la composición
de la animación al camino heredado (AlphaBlend), desactivando la transparencia
alfa por píxel y los cambios de estilo de fondo. Las animaciones de deslizamiento
y desvanecimiento siguen funcionando con normalidad — solo cambia el método de
composición.

### Créditos

Este mod está basado en el trabajo de **aubymori**:

Menu/Tooltip Slide Animation  
https://windhawk.net/mods/menu-tooltip-slide-animation

El mod original restauraba las animaciones clásicas de menús de Windows 98/XP.
Esta versión adapta el motor de animación para reproducir el **estilo de
movimiento moderno de WinUI utilizado en Windows 11**.

Cambios principales en esta versión:

• Reemplazo de la animación clásica por movimiento vertical estilo WinUI  
• Eliminación de la animación de tooltips  
• Eliminación de la animación horizontal  
• Mejora en la detección de dirección de submenús  
• Implementación precisa de la curva cubic-bezier  
• (1.0.1) Migración a composición alfa por píxel mediante UpdateLayeredWindow y adición del modo de compatibilidad con Translucent Windows
*/
// ==/WindhawkModReadme==

#include <wingdi.h>
#include <windhawk_utils.h>
#include <commctrl.h>
#include <cmath>

// ============================================================
//  Parâmetros do sistema
// ============================================================

static inline bool UIEffectsEnabled()
{
    BOOL v = FALSE;
    SystemParametersInfoW(SPI_GETUIEFFECTS, 0, &v, 0);
    return v != FALSE;
}

static inline bool MenuAnimationEnabled()
{
    return Wh_GetIntValue(L"AnimateMenus", 1) != 0;
}

static inline bool MenuFadeEnabled()
{
    return Wh_GetIntValue(L"FadeMenus", 0) != 0;
}

using SystemParametersInfo_t = decltype(&SystemParametersInfoW);
SystemParametersInfo_t SystemParametersInfoW_orig;
SystemParametersInfo_t SystemParametersInfoA_orig;

#define USPF_ININIT     0x1   // Chamado em Wh_ModInit   (usa SPI real, sem hook)
#define USPF_INUNINIT   0x2   // Chamado em Wh_ModUninit (restaura estado salvo)

static void UpdateSystemParameters(DWORD dwFlags)
{
    // Durante init/uninit ainda não instalamos o hook (ou já removemos),
    // então chamamos a função real diretamente.
    SystemParametersInfo_t pfnSPI = (dwFlags & (USPF_ININIT | USPF_INUNINIT))
        ? SystemParametersInfoW
        : SystemParametersInfoW_orig;

    bool fAnimEnabled = (dwFlags & USPF_INUNINIT)
        ? (Wh_GetIntValue(L"AnimateMenus", 0) != 0)
        : (MenuAnimationEnabled() && MenuFadeEnabled());

    pfnSPI(SPI_SETMENUANIMATION, 0, (LPVOID)(UINT_PTR)(BOOL)fAnimEnabled, SPIF_UPDATEINIFILE);

    // O bit de fade deve estar sempre ativo enquanto o mod estiver rodando.
    // Sem ele, componentes kernel-mode voltam para o slide legado, que está
    // quebrado nos drivers de GPU modernos (aparece instantâneo com fundo sólido).
    pfnSPI(SPI_SETMENUFADE, 0, (LPVOID)(UINT_PTR)(BOOL)TRUE, SPIF_UPDATEINIFILE);
}

static bool HandleSPICall(UINT uiAction, LPVOID pvParam, UINT fWinIni, BOOL *pfResult)
{
    switch (uiAction)
    {
        case SPI_GETMENUANIMATION:
        case SPI_GETMENUFADE:
        {
            if (!pvParam) { *pfResult = FALSE; return true; }
            BOOL value = UIEffectsEnabled()
                ? (uiAction == SPI_GETMENUANIMATION ? MenuAnimationEnabled() : MenuFadeEnabled())
                : FALSE;
            *(BOOL *)pvParam = value;
            *pfResult = TRUE;
            return true;
        }
        case SPI_SETMENUANIMATION:
        {
            Wh_SetIntValue(L"AnimateMenus", pvParam != 0);
            UpdateSystemParameters(0);
            if (fWinIni & SPIF_SENDCHANGE)
            {
                ULONG_PTR r;
                SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE,
                    uiAction, (LPARAM)L"", SMTO_NORMAL, 100, &r);
            }
            *pfResult = TRUE;
            return true;
        }
        case SPI_SETMENUFADE:
        {
            Wh_SetIntValue(L"FadeMenus", pvParam != 0);
            UpdateSystemParameters(0);
            if (fWinIni & SPIF_SENDCHANGE)
            {
                ULONG_PTR r;
                SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE,
                    uiAction, (LPARAM)L"", SMTO_NORMAL, 100, &r);
            }
            *pfResult = TRUE;
            return true;
        }
        default:
            return false;
    }
}

BOOL WINAPI SystemParametersInfoW_hook(UINT uiAction, UINT uiParam, LPVOID pvParam, UINT fWinIni)
{
    BOOL fResult = FALSE;
    if (HandleSPICall(uiAction, pvParam, fWinIni, &fResult))
        return fResult;
    return SystemParametersInfoW_orig(uiAction, uiParam, pvParam, fWinIni);
}

BOOL WINAPI SystemParametersInfoA_hook(UINT uiAction, UINT uiParam, LPVOID pvParam, UINT fWinIni)
{
    BOOL fResult = FALSE;
    if (HandleSPICall(uiAction, pvParam, fWinIni, &fResult))
        return fResult;
    return SystemParametersInfoA_orig(uiAction, uiParam, pvParam, fWinIni);
}

// ============================================================
//  Easing WinUI — cubic-bezier(0.1, 0.9, 0.2, 1.0)
// ============================================================
//
//  Avalia a função CSS cubic-bezier com precisão, usando o método de Newton
//  para inverter a curva X paramétrica e depois ler o valor Y.
//  P0=(0,0) e P3=(1,1) são implícitos; P1 e P2 são os pontos de controle.

static double CubicBezierX(double u, double p1x, double p2x)
{
    double inv = 1.0 - u;
    return 3.0*inv*inv*u*p1x + 3.0*inv*u*u*p2x + u*u*u;
}

static double CubicBezierXDeriv(double u, double p1x, double p2x)
{
    double inv = 1.0 - u;
    return 3.0*(inv*inv*p1x + 2.0*inv*u*(p2x - p1x) + u*u*(1.0 - p2x));
}

static double CubicBezierY(double u, double p1y, double p2y)
{
    double inv = 1.0 - u;
    return 3.0*inv*inv*u*p1y + 3.0*inv*u*u*p2y + u*u*u;
}

static double WinUIEase(double t)
{
    // Curva WinUI de expansão/exibição: cubic-bezier(0.1, 0.9, 0.2, 1.0)
    constexpr double p1x = 0.1, p1y = 0.9, p2x = 0.2, p2y = 1.0;

    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;

    // Resolve Bx(u) = t pelo método de Newton (6 iterações são suficientes)
    double u = t;
    for (int i = 0; i < 6; i++)
    {
        double dx = CubicBezierX(u, p1x, p2x) - t;
        double d  = CubicBezierXDeriv(u, p1x, p2x);
        if (fabs(d) < 1e-9) break;
        u -= dx / d;
        if (u < 0.0) u = 0.0;
        if (u > 1.0) u = 1.0;
    }
    return CubicBezierY(u, p1y, p2y);
}

// Parâmetros de animação configuráveis (lidos em Wh_ModSettingsChanged)
static double g_dAnimDuration     = 320.0; // duração total em ms
static double g_dFadeWindow       = 0.80;  // fração do tempo usada para fade-in
static UINT   g_uTimerInterval    = 6;     // intervalo do timer em ms
static bool   g_bTranslucentCompat = false; // modo de compatibilidade com Translucent Windows

// Alta precisão para cálculo de tempo da animação
static LARGE_INTEGER g_qpcFreq;

// ============================================================
//  Animação de menus
// ============================================================

#define MENUCLASS       MAKEINTATOM(0x8000)
#define WM_UAHINITMENU  0x0093
#define MFISPOPUP       0x00000001
#define IDWH_MNANIMATE  0x00574DB1
#define DCX_USESTYLE    0x00010000L

// Flags de direção da animação (somente vertical)
#define MNA_UP          0x4
#define MNA_DOWN        0x8

// Parâmetro do WM_UAHINITMENU
typedef struct tagUAHMENU {
    HMENU hmenu;
    HDC   hdc;
    DWORD dwFlags;
} UAHMENU, *PUAHMENU;

struct MNANIMATEINFO
{
    HWND      hwndAni;
    LARGE_INTEGER qpcStart;
    int       iDropDir;         // MNA_UP ou MNA_DOWN
    HDC       hdcWndAni;        // DC da janela do menu — usado para WM_PRINT e compat rendering
    HBITMAP   hbmAni;
    HGDIOBJ   hOldBmp;
    HDC       hdcAni;           // Fonte off-screen: bitmap opaco capturado via WM_PRINT
    // Campos exclusivos do modo ULW (não usados em modo de compatibilidade)
    HDC       hdcFrame;         // Destino 32-bit ARGB para UpdateLayeredWindow
    HBITMAP   hbmFrame;         // DIB section 32-bit associado a hdcFrame
    HGDIOBJ   hOldBmpFrame;     // Bitmap anterior em hdcFrame (para restauração)
    void     *pFrameBits;       // Ponteiro direto aos pixels de hbmFrame (pré-multiplicado)
    POINT     ptWindowPos;      // Posição da janela na tela (para UpdateLayeredWindow)
    int       cxAni;            // Largura total do menu
    int       cyAni;            // Altura total do menu
    int       cyVisible;        // Altura visível atual (cresce de 0 até cyAni)
    bool      fMouseMoved;
    POINT     ptInitialMousePos;
    bool      fLayoutRTL;
};

thread_local bool           g_fMenuAnimating = false;
thread_local ULONG          g_uMenuDepth     = 0;

thread_local bool           g_fIsTpm  = false;
thread_local POINT          g_ptTpm   = {};

thread_local HHOOK          g_hMouseHook    = NULL;
thread_local HHOOK          g_hKeyboardHook = NULL;

thread_local MNANIMATEINFO  g_mnAnimInfo    = {};

// Declaração antecipada
void MNAnimate(bool fIterate);

// ---- Hooks de janelas do Windows (mouse / teclado) ----

LRESULT WINAPI MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && g_fMenuAnimating && g_mnAnimInfo.hwndAni)
    {
        POINT pt = reinterpret_cast<MOUSEHOOKSTRUCT *>(lParam)->pt;

        // Se o mouse não se moveu desde que o menu abriu, não cancela a animação.
        // Isso cobre o caso em que o usuário abriu o menu pelo teclado.
        if (!g_mnAnimInfo.fMouseMoved
            && pt.x == g_mnAnimInfo.ptInitialMousePos.x
            && pt.y == g_mnAnimInfo.ptInitialMousePos.y)
        {
            goto done;
        }

        g_mnAnimInfo.fMouseMoved = true;

        // Obtém o rect atual da janela em animação para teste de interseção
        RECT rcMenu;
        GetWindowRect(g_mnAnimInfo.hwndAni, &rcMenu);

        bool fInsideMenu = (pt.x > rcMenu.left && pt.x < rcMenu.right
                         && pt.y > rcMenu.top  && pt.y < rcMenu.bottom);

        // Cancela a animação apenas se o usuário clicar dentro do menu.
        // Mover o cursor para dentro não interrompe — a animação continua
        // até o fim naturalmente, como no WinUI original.
        if (fInsideMenu && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN))
        {
            MNAnimate(false);
        }
    }

done:
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT WINAPI KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Qualquer tecla pressionada durante a animação a cancela imediatamente
    if (nCode >= 0 && g_fMenuAnimating)
        MNAnimate(false);

    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

static void RegisterWindowsHooks()
{
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE, MouseProc, NULL, GetCurrentThreadId());

    // Hook de teclado apenas necessário para popups de primeiro nível;
    // submenus são encerrados via WM_WINDOWPOSCHANGED do menu seguinte.
    if (g_uMenuDepth == 1)
        g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD, KeyboardProc, NULL, GetCurrentThreadId());
}

static void UnregisterWindowsHooks()
{
    if (g_hMouseHook)    { UnhookWindowsHookEx(g_hMouseHook);    g_hMouseHook    = NULL; }
    if (g_hKeyboardHook) { UnhookWindowsHookEx(g_hKeyboardHook); g_hKeyboardHook = NULL; }
}

// ---- Gerenciamento de recursos GDI ----

static void ResetMNAnimateInfo()
{
    // Salva hwnd antes de ZeroMemory — necessário para repintar após liberação.
    HWND hwndReset = g_mnAnimInfo.hwndAni;

    if (g_mnAnimInfo.hOldBmp)
        SelectObject(g_mnAnimInfo.hdcAni, g_mnAnimInfo.hOldBmp);

    if (g_mnAnimInfo.hOldBmpFrame)
        SelectObject(g_mnAnimInfo.hdcFrame, g_mnAnimInfo.hOldBmpFrame);

    if (g_mnAnimInfo.hdcWndAni)
        ReleaseDC(g_mnAnimInfo.hwndAni, g_mnAnimInfo.hdcWndAni);

    if (g_mnAnimInfo.hbmAni)
        DeleteObject(g_mnAnimInfo.hbmAni);

    if (g_mnAnimInfo.hdcAni)
        DeleteDC(g_mnAnimInfo.hdcAni);

    if (g_mnAnimInfo.hbmFrame)
        DeleteObject(g_mnAnimInfo.hbmFrame);   // pFrameBits inválido após este ponto

    if (g_mnAnimInfo.hdcFrame)
        DeleteDC(g_mnAnimInfo.hdcFrame);

    ZeroMemory(&g_mnAnimInfo, sizeof(g_mnAnimInfo));
    UnregisterWindowsHooks();
    g_fMenuAnimating = false;

    if (hwndReset)
    {
        DWORD dwEx = GetWindowLongW(hwndReset, GWL_EXSTYLE);
        if (dwEx & WS_EX_LAYERED)
        {
            if (g_bTranslucentCompat)
            {
                // Modo de compatibilidade (legado): restaura opacidade total via SLWA
                // antes de remover WS_EX_LAYERED, assim o sistema volta a desenhar
                // normalmente e não é necessário forçar RedrawWindow.
                SetLayeredWindowAttributes(hwndReset, 0, 255, LWA_ALPHA);
            }
            SetWindowLongW(hwndReset, GWL_EXSTYLE, dwEx & ~WS_EX_LAYERED);
        }

        if (!g_bTranslucentCompat)
        {
            // Modo ULW: durante a animação suprimimos todos os WM_PAINT e pintamos
            // exclusivamente via UpdateLayeredWindow. Ao sair do modo ULW (removendo
            // WS_EX_LAYERED), o conteúdo da janela está inválido — o sistema nunca
            // a pintou via GDI. Sem RedrawWindow, o Windows exibe o fundo padrão
            // branco e os itens do menu só aparecem ao passar o mouse.
            //
            // RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME instrui o sistema
            // a apagar e repintar toda a janela — inclusive a moldura não-cliente —
            // de forma síncrona antes de retornar.
            RedrawWindow(hwndReset, NULL, NULL,
                RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
        }
    }
}

// fNaturalEnd=true: animação terminou no tempo — o último tick já enviou
// o frame completo; só liberamos recursos.
// fNaturalEnd=false: cancelamento explícito — exibimos o menu completo
// instantaneamente antes de liberar recursos.
static void MNAnimateExit(bool fNaturalEnd)
{
    KillTimer(g_mnAnimInfo.hwndAni, IDWH_MNANIMATE);

    if (!fNaturalEnd)
    {
        if (g_bTranslucentCompat)
        {
            // Modo de compatibilidade (legado): blita o menu completo direto
            // no DC da janela para exibição instantânea.
            if (g_mnAnimInfo.hdcAni)
            {
                DWORD dwOldLayout = SetLayout(g_mnAnimInfo.hdcWndAni, 0);
                BitBlt(g_mnAnimInfo.hdcWndAni, 0, 0,
                       g_mnAnimInfo.cxAni, g_mnAnimInfo.cyAni,
                       g_mnAnimInfo.hdcAni, 0, 0,
                       SRCCOPY | NOMIRRORBITMAP);
                SetLayout(g_mnAnimInfo.hdcWndAni, dwOldLayout);
            }
        }
        else
        {
            // Modo ULW: envia um frame 100% opaco via UpdateLayeredWindow antes
            // de remover WS_EX_LAYERED — garante que o menu apareça completo
            // ao sair do modo ULW, sem depender do RedrawWindow subsequente.
            if (g_mnAnimInfo.hdcFrame && g_mnAnimInfo.pFrameBits)
            {
                BitBlt(g_mnAnimInfo.hdcFrame, 0, 0,
                       g_mnAnimInfo.cxAni, g_mnAnimInfo.cyAni,
                       g_mnAnimInfo.hdcAni, 0, 0,
                       SRCCOPY | NOMIRRORBITMAP);

                DWORD *px    = (DWORD *)g_mnAnimInfo.pFrameBits;
                int    count = g_mnAnimInfo.cxAni * g_mnAnimInfo.cyAni;
                for (int i = 0; i < count; i++)
                    px[i] |= 0xFF000000u;

                POINT         ptSrc = {};
                SIZE          sz    = { g_mnAnimInfo.cxAni, g_mnAnimInfo.cyAni };
                BLENDFUNCTION bf    = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                UpdateLayeredWindow(g_mnAnimInfo.hwndAni, NULL,
                    &g_mnAnimInfo.ptWindowPos, &sz,
                    g_mnAnimInfo.hdcFrame, &ptSrc, 0, &bf, ULW_ALPHA);
            }
        }
    }

    ResetMNAnimateInfo();
}

// ---- Tick principal da animação ----

void MNAnimate(bool fIterate)
{
    if (!g_fMenuAnimating)
        return;

    double kDuration   = g_dAnimDuration;
    double kFadeWindow = g_dFadeWindow;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double elapsed = (double)(now.QuadPart - g_mnAnimInfo.qpcStart.QuadPart)
                   / (double)g_qpcFreq.QuadPart * 1000.0;
    double t = elapsed / kDuration;
    if (t > 1.0) t = 1.0;

    if (!fIterate)
    {
        MNAnimateExit(false); // cancelamento explícito
        return;
    }
    if (t >= 1.0)
    {
        MNAnimateExit(true); // tempo esgotado — fim natural
        return;
    }

    double smooth = WinUIEase(t);
    double motion = smooth + smooth * smooth * 0.02;

    // Usa floor em vez de round: a fração sobrante alimenta a linha
    // de borda sub-pixel, evitando saltos de 2px nos ticks lentos.
    double cyFloat = g_mnAnimInfo.cyAni * motion;
    int    cyFull  = (int)cyFloat;
    if (cyFull > g_mnAnimInfo.cyAni) cyFull = g_mnAnimInfo.cyAni;
    double frac = (cyFull < g_mnAnimInfo.cyAni) ? (cyFloat - (double)cyFull) : 0.0;

    int cyLast = g_mnAnimInfo.cyVisible;
    g_mnAnimInfo.cyVisible = cyFull;

    // Alpha global do frame para o fade-in — calculado com easing WinUI.
    BYTE alpha = 255;
    if (kFadeWindow > 0.0 && t < kFadeWindow)
    {
        double fadeT = t / kFadeWindow;
        if (fadeT > 1.0) fadeT = 1.0;
        alpha = (BYTE)(255.0 * WinUIEase(fadeT));
    }

    if (g_bTranslucentCompat)
    {
        // Modo de compatibilidade (legado): atualiza o alpha da janela inteira
        // via SetLayeredWindowAttributes antes do early-return, garantindo que
        // o fade seja sempre contínuo mesmo quando a altura não mudou.
        SetLayeredWindowAttributes(g_mnAnimInfo.hwndAni, 0, alpha, LWA_ALPHA);
    }

    // Evita repintura redundante quando nem a altura nem a fração mudaram
    if (g_mnAnimInfo.cyVisible == cyLast && frac < 0.004)
        return;

    // Coordenadas de destino no frame e offset na fonte.
    int y, yOff;
    if (g_mnAnimInfo.iDropDir & MNA_UP)
    {
        // Borda superior sobe; conteúdo entra pela borda de baixo.
        y    = g_mnAnimInfo.cyAni - cyFull;
        yOff = 0;
    }
    else
    {
        // Borda inferior desce; conteúdo entra pela borda de cima.
        y    = 0;
        yOff = g_mnAnimInfo.cyAni - cyFull;
    }

    if (g_bTranslucentCompat)
    {
        // ----------------------------------------------------------------
        //  Modo de compatibilidade (legado) — composição via AlphaBlend
        //
        //  Pinta diretamente no DC da janela (hdcWndAni) usando AlphaBlend
        //  para a transparência e BitBlt quando o alpha está próximo de 255.
        //  Compatível com mods que modificam o fundo da janela, como
        //  Translucent Windows.
        // ----------------------------------------------------------------

        DWORD dwOldLayout = SetLayout(g_mnAnimInfo.hdcWndAni, 0);

        // Região inteira visível
        if (cyFull > 0)
        {
            IntersectClipRect(g_mnAnimInfo.hdcWndAni,
                0, y, g_mnAnimInfo.cxAni, y + cyFull);

            if (alpha < 240)
            {
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, 0 };
                AlphaBlend(
                    g_mnAnimInfo.hdcWndAni, 0, y,   g_mnAnimInfo.cxAni, cyFull,
                    g_mnAnimInfo.hdcAni,   0, yOff, g_mnAnimInfo.cxAni, cyFull, bf);
            }
            else
            {
                BitBlt(
                    g_mnAnimInfo.hdcWndAni, 0, y,   g_mnAnimInfo.cxAni, cyFull,
                    g_mnAnimInfo.hdcAni,   0, yOff,
                    SRCCOPY | NOMIRRORBITMAP);
            }
            SelectClipRgn(g_mnAnimInfo.hdcWndAni, NULL);
        }

        // Linha de borda sub-pixel via AlphaBlend direto no DC da janela
        if (frac > 0.004 && cyFull < g_mnAnimInfo.cyAni)
        {
            int yDest, ySrc;
            if (g_mnAnimInfo.iDropDir & MNA_UP)
            {
                yDest = y - 1;
                ySrc  = cyFull;
            }
            else
            {
                yDest = cyFull;
                ySrc  = g_mnAnimInfo.cyAni - cyFull - 1;
            }
            if (yDest >= 0 && yDest < g_mnAnimInfo.cyAni)
            {
                BYTE ba = (BYTE)(frac * (double)alpha + 0.5);
                if (ba > 0)
                {
                    BLENDFUNCTION bf = { AC_SRC_OVER, 0, ba, 0 };
                    AlphaBlend(
                        g_mnAnimInfo.hdcWndAni, 0, yDest, g_mnAnimInfo.cxAni, 1,
                        g_mnAnimInfo.hdcAni,   0, ySrc,  g_mnAnimInfo.cxAni, 1, bf);
                }
            }
        }

        SetLayout(g_mnAnimInfo.hdcWndAni, dwOldLayout);
    }
    else
    {
        // ----------------------------------------------------------------
        //  Modo ULW — composição via UpdateLayeredWindow com ARGB 32-bit
        //
        //  UpdateLayeredWindow exige um bitmap 32-bit com alpha pré-multiplicado:
        //  cada canal (R, G, B) deve ser multiplicado pelo alpha antes de armazenar.
        //  Pixels transparentes (regiões ainda não reveladas) ficam em 0x00000000,
        //  tornando-se genuinamente transparentes — sem fundo branco ou preto.
        // ----------------------------------------------------------------

        // Zera o frame inteiro → todas as regiões começam transparentes
        memset(g_mnAnimInfo.pFrameBits, 0,
               (size_t)g_mnAnimInfo.cxAni * (size_t)g_mnAnimInfo.cyAni * 4);

        // Copia a região visível do bitmap opaco (hdcAni) para o frame.
        // BitBlt para um DIB 32-bit escreve os bytes de cor corretamente e
        // deixa o byte de alpha em 0 — fixaremos o alpha no loop abaixo.
        if (cyFull > 0)
        {
            BitBlt(g_mnAnimInfo.hdcFrame, 0, y,   g_mnAnimInfo.cxAni, cyFull,
                   g_mnAnimInfo.hdcAni,   0, yOff, SRCCOPY | NOMIRRORBITMAP);

            // Aplica alpha pré-multiplicado aos pixels da região visível.
            // Formato do DWORD no DIB (little-endian): 0xAARRGGBB
            //   byte[0]=B, byte[1]=G, byte[2]=R, byte[3]=A
            DWORD *px    = (DWORD *)g_mnAnimInfo.pFrameBits + (size_t)y * g_mnAnimInfo.cxAni;
            int    count = cyFull * g_mnAnimInfo.cxAni;
            for (int i = 0; i < count; i++)
            {
                DWORD c = px[i];
                BYTE  b = (BYTE)(c        & 0xFF);
                BYTE  g_= (BYTE)((c >> 8) & 0xFF);
                BYTE  r = (BYTE)((c >>16) & 0xFF);
                px[i] = ((DWORD)(b * alpha / 255))
                      | ((DWORD)(g_* alpha / 255) << 8)
                      | ((DWORD)(r * alpha / 255) << 16)
                      | ((DWORD)alpha             << 24);
            }
        }

        // Linha de borda sub-pixel: a próxima linha com alpha proporcional
        // à fração de pixel sobrante — garante movimento visualmente contínuo.
        if (frac > 0.004 && cyFull < g_mnAnimInfo.cyAni)
        {
            int yDest, ySrc;
            if (g_mnAnimInfo.iDropDir & MNA_UP)
            {
                yDest = y - 1;
                ySrc  = cyFull;
            }
            else
            {
                yDest = cyFull;
                ySrc  = g_mnAnimInfo.cyAni - cyFull - 1;
            }

            if (yDest >= 0 && yDest < g_mnAnimInfo.cyAni)
            {
                BYTE ba = (BYTE)(frac * (double)alpha + 0.5);
                if (ba > 0)
                {
                    BitBlt(g_mnAnimInfo.hdcFrame, 0, yDest, g_mnAnimInfo.cxAni, 1,
                           g_mnAnimInfo.hdcAni,   0, ySrc,  SRCCOPY | NOMIRRORBITMAP);

                    DWORD *pxSub = (DWORD *)g_mnAnimInfo.pFrameBits
                                 + (size_t)yDest * g_mnAnimInfo.cxAni;
                    for (int i = 0; i < g_mnAnimInfo.cxAni; i++)
                    {
                        DWORD c  = pxSub[i];
                        BYTE  b  = (BYTE)(c        & 0xFF);
                        BYTE  g_ = (BYTE)((c >> 8) & 0xFF);
                        BYTE  r  = (BYTE)((c >>16) & 0xFF);
                        pxSub[i] = ((DWORD)(b * ba / 255))
                                  | ((DWORD)(g_* ba / 255) << 8)
                                  | ((DWORD)(r * ba / 255) << 16)
                                  | ((DWORD)ba             << 24);
                    }
                }
            }
        }

        // Envia o frame ARGB composto ao DWM via UpdateLayeredWindow.
        // O compositor usa o alpha de cada pixel individualmente — as regiões
        // com alpha=0 são genuinamente transparentes (see-through para o desktop).
        {
            POINT         ptSrc = {};
            SIZE          sz    = { g_mnAnimInfo.cxAni, g_mnAnimInfo.cyAni };
            BLENDFUNCTION bf    = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            UpdateLayeredWindow(g_mnAnimInfo.hwndAni, NULL,
                &g_mnAnimInfo.ptWindowPos, &sz,
                g_mnAnimInfo.hdcFrame, &ptSrc, 0, &bf, ULW_ALPHA);
        }
    }

    if (g_mnAnimInfo.cyVisible == g_mnAnimInfo.cyAni)
        MNAnimateExit(true); // último pixel pintado — fim natural
}

// ============================================================
//  Subclasse da janela de menu
// ============================================================

LRESULT CALLBACK MenuSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIDSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    // -----------------------------------------------------------------
    case WM_DESTROY:
    // -----------------------------------------------------------------
        if (g_uMenuDepth > 0) g_uMenuDepth--;

        // Ao navegar pela barra de menus via hover, o Windows destrói o popup
        // anterior antes de criar o novo. Só resetamos se for a janela animando.
        if (g_mnAnimInfo.hwndAni == hwnd)
        {
            // Restaura RTL antes de ResetMNAnimateInfo — após o reset o struct está zerado
            if (g_mnAnimInfo.fLayoutRTL)
            {
                DWORD dwEx = GetWindowLongW(hwnd, GWL_EXSTYLE);
                SetWindowLongW(hwnd, GWL_EXSTYLE, dwEx | WS_EX_LAYOUTRTL);
            }
            ResetMNAnimateInfo();
        }

        goto DWP;

    // -----------------------------------------------------------------
    case WM_WINDOWPOSCHANGING:
    // -----------------------------------------------------------------
    {
        LPWINDOWPOS pwpC = (LPWINDOWPOS)lParam;
        if (!(pwpC->flags & SWP_SHOWWINDOW))
            goto DWP;
        if (!UIEffectsEnabled() || !MenuAnimationEnabled() || MenuFadeEnabled())
            goto DWP;

        // Desativa RTL já aqui — antes de o DWM compor a janela — para que
        // o conteúdo nunca apareça espelhado para o lado oposto.
        // Aproveitamos a mesma leitura para adicionar WS_EX_LAYERED e definir
        // alpha=0, tornando a janela invisível ao compositor até o primeiro tick.
        // WS_EX_LAYERED é mantido durante toda a animação e removido
        // em ResetMNAnimateInfo ao fim (natural ou por cancelamento).
        {
            DWORD dwEx = GetWindowLongW(hwnd, GWL_EXSTYLE);
            dwEx &= ~WS_EX_LAYOUTRTL;   // remove RTL antes da composição
            dwEx |=  WS_EX_LAYERED;     // habilita transparência por alpha
            SetWindowLongW(hwnd, GWL_EXSTYLE, dwEx);
            SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
        }

        goto DWP;
    }

    // -----------------------------------------------------------------
    case WM_NCPAINT:
    // -----------------------------------------------------------------
        // Suprime a pintura da borda não-cliente durante a animação ULW.
        // A borda do menu Win32 é desenhada pelo sistema via WM_NCPAINT —
        // não pelo DWM. Sem supressão, ela aparece com tamanho total desde
        // o primeiro frame enquanto o conteúdo ainda está crescendo.
        // RDW_FRAME no RedrawWindow de ResetMNAnimateInfo repinta o NC
        // corretamente quando a animação termina.
        //
        // Não suprimimos no modo de compatibilidade: nesse caminho o DC
        // da janela (hdcWndAni) é pincelado diretamente pelo AlphaBlend e
        // a borda NC precisa estar presente normalmente.
        if (!g_bTranslucentCompat && g_fMenuAnimating && g_mnAnimInfo.hwndAni == hwnd)
            return 0;
        goto DWP;

    // -----------------------------------------------------------------
    case WM_PAINT:
    // -----------------------------------------------------------------
        // Suprime o repaint normal durante a animação;
        // toda pintura é feita por nós via DC off-screen.
        if (g_fMenuAnimating && g_mnAnimInfo.hwndAni == hwnd)
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        goto DWP;

    // -----------------------------------------------------------------
    case WM_TIMER:
    // -----------------------------------------------------------------
        if (wParam == IDWH_MNANIMATE)
        {
            MNAnimate(true);
            return 0;
        }
        goto DWP;

    // -----------------------------------------------------------------
    case WM_WINDOWPOSCHANGED:
    // -----------------------------------------------------------------
    {
        LPWINDOWPOS pwp = (LPWINDOWPOS)lParam;
        if (!(pwp->flags & SWP_SHOWWINDOW))
            goto DWP;

        // Se a animação não vai iniciar (UIEffects off, FadeEnabled, etc.),
        // remove WS_EX_LAYERED para não deixar a janela presa invisível.
        if (!UIEffectsEnabled() || !MenuAnimationEnabled() || MenuFadeEnabled())
        {
            DWORD dwExL = GetWindowLongW(hwnd, GWL_EXSTYLE);
            if (dwExL & WS_EX_LAYERED)
                SetWindowLongW(hwnd, GWL_EXSTYLE, dwExL & ~WS_EX_LAYERED);
            goto DWP;
        }

        // Encerra animação anterior (ex: submenu abrindo durante animação do pai,
        // ou hover na barra antes que a animação anterior termine).
        MNAnimate(false);

        g_uMenuDepth++;

        // -----------------------------------------------------------------
        //  Detecção de direção — cada menu é avaliado de forma independente.
        //
        //  Caso especial depth=1 via TrackPopupMenu/Ex: o ponto de invocação
        //  exato (g_ptTpm) é conhecido. Se o menu abriu acima desse ponto,
        //  o Windows o inverteu — animamos para cima (MNA_UP).
        //
        //  Demais casos (submenus, barra de menus): usamos a posição do
        //  cursor como referência. Se o topo do menu ficou acima do meio
        //  do cursor, o Windows inverteu o menu — animamos para cima.
        // -----------------------------------------------------------------

        int iDropDir;

        if (g_fIsTpm && g_uMenuDepth == 1)
        {
            // Menu de contexto principal
            iDropDir = (pwp->y >= g_ptTpm.y - 4) ? MNA_DOWN : MNA_UP;
        }
        else
        {
            POINT ptCursor;
            GetCursorPos(&ptCursor);

            // Distância entre cursor e topo do menu
            int dy = ptCursor.y - pwp->y;

            // Se o topo do menu ficou acima do cursor,
            // o Windows inverteu o submenu
            if (dy > (pwp->cy / 2))
                iDropDir = MNA_UP;
            else
                iDropDir = MNA_DOWN;
        }

        // -----------------------------------------------------------------
        //  Inicializa estado GDI da animação
        // -----------------------------------------------------------------
        g_fMenuAnimating               = true;
        g_mnAnimInfo.hwndAni           = hwnd;
        g_mnAnimInfo.iDropDir          = iDropDir;
        g_mnAnimInfo.cxAni             = pwp->cx;
        g_mnAnimInfo.cyAni             = pwp->cy;
        g_mnAnimInfo.cyVisible         = 0;
        g_mnAnimInfo.ptWindowPos       = { pwp->x, pwp->y };
        g_mnAnimInfo.hdcWndAni         = GetDCEx(hwnd, NULL, DCX_WINDOW | DCX_USESTYLE);
        g_mnAnimInfo.hdcAni            = CreateCompatibleDC(g_mnAnimInfo.hdcWndAni);
        g_mnAnimInfo.hbmAni            = CreateCompatibleBitmap(g_mnAnimInfo.hdcWndAni, pwp->cx, pwp->cy);
        g_mnAnimInfo.fLayoutRTL        = !!(GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL);
        GetCursorPos(&g_mnAnimInfo.ptInitialMousePos);

        if (!g_bTranslucentCompat)
        {
            // Modo ULW: cria o DC e o DIB section 32-bit ARGB usado por UpdateLayeredWindow.
            // biHeight negativo = DIB top-down, coordenada 0 no topo — igual ao
            // nosso sistema de coordenadas de animação.
            g_mnAnimInfo.hdcFrame = CreateCompatibleDC(g_mnAnimInfo.hdcWndAni);
            if (g_mnAnimInfo.hdcFrame)
            {
                BITMAPINFOHEADER bmi  = {};
                bmi.biSize            = sizeof(bmi);
                bmi.biWidth           = pwp->cx;
                bmi.biHeight          = -pwp->cy;  // top-down
                bmi.biPlanes          = 1;
                bmi.biBitCount        = 32;
                bmi.biCompression     = BI_RGB;
                g_mnAnimInfo.hbmFrame = CreateDIBSection(
                    g_mnAnimInfo.hdcFrame, (BITMAPINFO *)&bmi, DIB_RGB_COLORS,
                    &g_mnAnimInfo.pFrameBits, NULL, 0);
                if (g_mnAnimInfo.hbmFrame)
                    g_mnAnimInfo.hOldBmpFrame = SelectObject(
                        g_mnAnimInfo.hdcFrame, g_mnAnimInfo.hbmFrame);
            }
        }

        // Verifica falha de alocação GDI (OOM extremo) antes de SelectObject.
        // ResetMNAnimateInfo libera o que foi alocado e remove WS_EX_LAYERED.
        {
            bool fOOM = (!g_mnAnimInfo.hdcWndAni || !g_mnAnimInfo.hdcAni || !g_mnAnimInfo.hbmAni);
            if (!g_bTranslucentCompat)
                fOOM = fOOM || (!g_mnAnimInfo.hdcFrame || !g_mnAnimInfo.hbmFrame || !g_mnAnimInfo.pFrameBits);
            if (fOOM)
            {
                ResetMNAnimateInfo();
                goto DWP;
            }
        }

        // Captura o menu no estado totalmente renderizado (fonte da animação).
        // hdcAni é um DDB opaco — os pixels são copiados e transformados a cada tick.
        g_mnAnimInfo.hOldBmp = SelectObject(g_mnAnimInfo.hdcAni, g_mnAnimInfo.hbmAni);
        SendMessageW(hwnd, WM_PRINT, (WPARAM)g_mnAnimInfo.hdcAni,
            PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);

        RegisterWindowsHooks();

        if (!g_bTranslucentCompat)
        {
            // Modo ULW: alterna WS_EX_LAYERED para sair do modo SLWA e entrar em modo ULW.
            //
            // Em WM_WINDOWPOSCHANGING chamamos SetLayeredWindowAttributes(alpha=0)
            // para ocultar a janela ao compositor antes da primeira pintura.
            // Isso coloca a janela em "modo SLWA". Uma janela em modo SLWA rejeita
            // chamadas a UpdateLayeredWindow (retorna FALSE sem efeito), causando o
            // fundo branco a cada tick.
            //
            // A solução: remover e re-adicionar WS_EX_LAYERED limpa o modo SLWA.
            // Isso ocorre enquanto a janela ainda está invisível (alpha=0), de modo
            // que o ciclo é imperceptível ao usuário.
            // Após o ciclo, a próxima chamada — MNAnimate(true) abaixo — será a
            // primeira UpdateLayeredWindow, estabelecendo o modo ULW corretamente.
            {
                DWORD dwExMode = GetWindowLongW(hwnd, GWL_EXSTYLE);
                SetWindowLongW(hwnd, GWL_EXSTYLE, dwExMode & ~WS_EX_LAYERED);
                SetWindowLongW(hwnd, GWL_EXSTYLE, dwExMode |  WS_EX_LAYERED);
            }

            // Desabilita a decoração NC do DWM (borda + sombra) durante a animação.
            // O DWM compõe a borda como uma camada separada com o tamanho total da
            // janela desde o início — independente da nossa superfície ULW — dando
            // a impressão de que o conteúdo "se encaixa" na borda durante o slide.
            // A borda NC é suprimida via WM_NCPAINT durante a animação e restaurada
            // pelo RDW_FRAME do RedrawWindow em ResetMNAnimateInfo.
        }

        QueryPerformanceCounter(&g_mnAnimInfo.qpcStart);
        SetTimer(hwnd, IDWH_MNANIMATE, g_uTimerInterval, NULL);
        MNAnimate(true);
        goto DWP;
    }

    default:
        goto DWP;
    }

DWP:
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// ============================================================
//  Hooks de DefWindowProc / DefDlgProc / etc.
//  — interceptam WM_UAHINITMENU para instalar a subclasse do menu
//
//  WM_UAHINITMENU (0x0093) é uma mensagem UAH (User API Hook) interna
//  enviada à janela proprietária imediatamente antes de cada popup ser
//  exibido — inclusive durante navegação hover na barra de menus.
//  Neste ponto a janela do menu já existe e FindWindowExW localiza o
//  HWND de forma confiável.
// ============================================================

static void SlideAnimWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg != WM_UAHINITMENU)
        return;

    if (!UIEffectsEnabled() || !MenuAnimationEnabled() || MenuFadeEnabled())
        return;

    PUAHMENU puim = (PUAHMENU)lParam;
    if (!(puim->dwFlags & MFISPOPUP))
        return;

    // Localiza o HWND da janela de menu que exibe este HMENU.
    // SetWindowSubclass é idempotente para o mesmo (hwnd, uIDSubclass):
    // chamá-lo novamente apenas atualiza os dados, sem duplicar o hook.
    HWND hwndMenu = NULL;
    while ((hwndMenu = FindWindowExW(NULL, hwndMenu, MENUCLASS, nullptr)) != NULL)
    {
        HMENU hm = (HMENU)SendMessageW(hwndMenu, MN_GETHMENU, 0, 0);
        if (hm == puim->hmenu)
            break;
    }

    if (hwndMenu)
        SetWindowSubclass(hwndMenu, MenuSubclassProc, 0, (DWORD_PTR)puim->hmenu);
}

#define DWP_HOOK_(name, defArgs, callArgs)               \
LRESULT (CALLBACK *name##_orig) defArgs;                 \
LRESULT CALLBACK   name##_hook  defArgs                  \
{                                                         \
    SlideAnimWndProc(hWnd, uMsg, wParam, lParam);        \
    return name##_orig callArgs;                         \
}

#define DWP_HOOK(name, defArgs, callArgs)  \
    DWP_HOOK_(name##A, defArgs, callArgs)  \
    DWP_HOOK_(name##W, defArgs, callArgs)

DWP_HOOK(DefWindowProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(DefFrameProc,
    (HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, hWndMDIClient, uMsg, wParam, lParam))
DWP_HOOK(DefMDIChildProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(DefDlgProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))

// ============================================================
//  Hooks de TrackPopupMenu / TrackPopupMenuEx
//  — registram o ponto de invocação para detecção de direção
// ============================================================

using TrackPopupMenu_t   = decltype(&TrackPopupMenu);
using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);

TrackPopupMenu_t   TrackPopupMenu_orig;
TrackPopupMenuEx_t TrackPopupMenuEx_orig;

BOOL WINAPI TrackPopupMenu_hook(
    HMENU hMenu, UINT uFlags, int x, int y,
    int nReserved, HWND hWnd, const RECT *prcRect)
{
    g_fIsTpm = true;
    g_ptTpm  = { x, y };
    BOOL fResult = TrackPopupMenu_orig(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    g_fIsTpm = false;
    g_ptTpm  = {};
    return fResult;
}

BOOL WINAPI TrackPopupMenuEx_hook(
    HMENU hMenu, UINT uFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
{
    g_fIsTpm = true;
    g_ptTpm  = { x, y };
    BOOL fResult = TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hwnd, lptpm);
    g_fIsTpm = false;
    g_ptTpm  = {};
    return fResult;
}

// ============================================================
//  Ciclo de vida do mod
// ============================================================

// Macro auxiliar de dois níveis para converter o nome da função em wide string.
// O operador # gera uma string estreita (char); L#func não é válido em C/C++.
// A solução é passar por um macro intermediário que força a expansão antes
// de concatenar o prefixo L.
#define WH_WIDEN2(x) L ## x
#define WH_WIDEN(x)  WH_WIDEN2(x)

#define HOOK(func)                                                                               \
    if (!Wh_SetFunctionHook((void *)func, (void *)func##_hook, (void **)&func##_orig)) {       \
        Wh_Log(L"Falha ao hookar %s", WH_WIDEN(#func));                                        \
        return FALSE;                                                                            \
    }

#define HOOK_A_W(func) HOOK(func##A) HOOK(func##W)

void Wh_ModSettingsChanged()
{
    // Duração em ms — mínimo de 50ms para evitar flash invisível
    int iDuration = Wh_GetIntSetting(L"animationDuration");
    g_dAnimDuration = (iDuration >= 50) ? (double)iDuration : 50.0;

    // Fade em % (0–100) convertido para fração (0.0–1.0)
    int iFade = Wh_GetIntSetting(L"fadeDuration");
    if (iFade < 0)   iFade = 0;
    if (iFade > 100) iFade = 100;
    g_dFadeWindow = iFade / 100.0;

    // Intervalo do timer em ms — clampado entre 1 e 100
    int iInterval = Wh_GetIntSetting(L"timerInterval");
    if (iInterval < 1)   iInterval = 1;
    if (iInterval > 100) iInterval = 100;
    g_uTimerInterval = (UINT)iInterval;

    // Modo de compatibilidade com Translucent Windows
    g_bTranslucentCompat = Wh_GetIntSetting(L"translucentWindowsCompat") != 0;
}

BOOL Wh_ModInit()
{
    Wh_ModSettingsChanged();
    QueryPerformanceFrequency(&g_qpcFreq);

    // Salva o estado atual de animação para restaurá-lo fielmente no uninit
    BOOL fEnabled = FALSE;
    if (SystemParametersInfoW(SPI_GETMENUANIMATION, 0, &fEnabled, 0) && fEnabled)
        Wh_SetIntValue(L"AnimateMenus", 1);

    UpdateSystemParameters(USPF_ININIT);

    HOOK_A_W(DefWindowProc)
    HOOK_A_W(DefFrameProc)
    HOOK_A_W(DefMDIChildProc)
    HOOK_A_W(DefDlgProc)
    HOOK(TrackPopupMenu)
    HOOK(TrackPopupMenuEx)
    HOOK_A_W(SystemParametersInfo)

    return TRUE;
}

void Wh_ModUninit()
{
    UpdateSystemParameters(USPF_INUNINIT);
}
