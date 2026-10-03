// ==WindhawkMod==
// @id              alt-tab-flip-3d
// @name            Alt+Tab Flip 3D (Vista Style)
// @name:pt-BR      Alt+Tab Flip 3D (estilo Vista)
// @description     Replaces Alt+Tab with a fluid, Windows Vista-style Flip 3D stack of live windows
// @description:pt-BR Substitui o Alt+Tab por uma pilha 3D fluida de janelas ao vivo, no estilo Flip 3D do Windows Vista
// @version         1.5.0
// @author          caliberda
// @github          https://github.com/cesarkali
// @homepage        https://caliberda.com.br
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -ld3d11 -ldxgi -ld2d1 -ldcomp -ldwrite -ldwmapi -lole32 -loleaut32 -luuid -lruntimeobject -lwindowscodecs -lshcore -lshell32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+Tab Flip 3D (Vista Style)

![Flip 3D style](https://i.imgur.com/mRzfHq2.gif)

**English** | [Português](#português)

Brings back the Windows Vista **Flip 3D** window switcher on Windows 11, with
smooth, spring-based animations inspired by macOS.

Press **Alt+Tab** and your windows fly from their real positions into a 3D
stack, one behind the other, showing their **live content**. Keep Alt held and
flip through them; release Alt and the chosen window flies back into place.

## Animation styles

![All animation styles](https://i.imgur.com/ul1d5ca.jpeg)

* **Flip 3D**: the Windows Vista stack, receding up and to the left.
* **Cascade**: a deck of windows tilted backwards, receding upwards.
* **Cover Flow**: the selected window faces you, the others are turned on
  both sides.
* **Carousel**: a 3D ring of windows that rotates around its center.
* **Grid**: every window side by side, like Mission Control.
* **Helix**: a 3D spiral that turns and rises as you flip.
* **Fan**: windows spread like playing cards held in a hand.
* **Panorama**: a row of windows on a wall curving around you.
* **Tunnel**: windows sinking into a twisting tunnel.
* **Rolodex**: a vertical wheel, windows roll over the top and under the
  bottom.

Simpler 2D styles, for those who want something closer to the default:

* **Windows 11 (enhanced)**: the native layout, with windows flying into their
  thumbnails and a selection that slides smoothly.
* **Thumbnails with titles below**: squared panel, titles under the
  thumbnails.
* **Icons and titles**: a row of large icons with their titles.
* **Vertical list**: a compact list of icons and titles.
* **Classic (Windows XP)**: the old icon grid with the title in a box below.
* **Native Windows Alt+Tab**: the mod leaves Alt+Tab alone.

The switcher opens on the monitor where the mouse cursor is (or, optionally,
where the active window is).

## Controls

| Key / action              | What it does                                  |
|---------------------------|-----------------------------------------------|
| Alt+Tab                   | Open the stack / next window                  |
| Alt+Shift+Tab             | Previous window                               |
| Arrow keys, mouse wheel   | Flip forward / backward                       |
| Release Alt, Enter, Space | Switch to the window in front                 |
| Click a window            | Switch to that window                         |
| Esc                       | Cancel and go back to where you were          |

A quick Alt+Tab tap (shorter than the *show delay*) switches instantly to the
previous window without showing the stack, just like the native switcher.

## How it compares with similar mods

* **[Aero Flip 3D Recreation](https://windhawk.net/mods/aero-flip3d-recreation)**
  brings Flip 3D back on **Win+Tab**. This mod replaces **Alt+Tab** instead,
  draws the windows with real perspective from their live content, and adds
  nine more 3D layouts. Both can be installed together, since they use
  different shortcuts.
* **[Simple Window Switcher](https://windhawk.net/mods/simple-window-switcher)**
  and **[Legacy Alt+Tab dialog](https://windhawk.net/mods/legacy-alt-tab)**
  also replace Alt+Tab. The 2D styles here are close to them, and are included
  so that every style, 3D or 2D, can be picked from one place and share the
  same animations, keys and settings.

**Don't combine this mod with another Alt+Tab replacement**, such as the two
above. Only one of them can take over Alt+Tab.

## Notes

* Windows 11 only (uses Windows Graphics Capture without the yellow border).
* Ctrl+Alt+Tab and Win+Tab are not changed.
* The switcher runs in its own background process, so a problem in it can't
  affect the taskbar or the desktop. Only the small part that handles the case
  below runs inside explorer.
* Also works when a program running as administrator is in front: the mod
  catches the Alt+Tab hotkey inside explorer before the native switcher opens,
  and hands it to the switcher.
* Minimized windows show their last content, like the taskbar previews (this
  can be turned off). Windows that refuse to be captured are shown as a card
  with the app icon.
* With the *Dim only* background, the real windows stay visible behind the
  stack. The wallpaper backgrounds hide them, like the original Flip 3D.
* If the switcher can't start (e.g. no GPU device), the native Alt+Tab keeps
  working.

## Credits

* Mod by **caliberda** ([caliberda.com.br](https://caliberda.com.br),
  Instagram [@cesar.kali](https://www.instagram.com/cesar.kali)).
* Inspired by **Windows Flip 3D** from Windows Vista and Windows 7 (Microsoft).

---

## Português

Traz de volta o alternador de janelas **Flip 3D** do Windows Vista para o
Windows 11, com animações suaves baseadas em molas, inspiradas no macOS.

Pressione **Alt+Tab** e suas janelas voam de suas posições reais para uma pilha
3D, uma atrás da outra, mostrando o **conteúdo ao vivo**. Segure o Alt e vá
passando por elas; solte o Alt e a janela escolhida volta voando para o lugar.

### Estilos de animação

![Todos os estilos de animação](https://i.imgur.com/ul1d5ca.jpeg)

* **Flip 3D**: a pilha do Windows Vista, indo para cima e para a esquerda.
* **Cascata**: um baralho de janelas inclinado para trás, subindo.
* **Cover Flow**: a janela escolhida fica de frente, as outras viradas dos
  dois lados.
* **Carrossel**: um anel 3D de janelas que gira em volta do centro.
* **Grade**: todas as janelas lado a lado, como o Mission Control.
* **Hélice**: uma espiral 3D que gira e sobe enquanto você troca.
* **Leque**: janelas abertas como cartas de baralho na mão.
* **Panorama**: uma fileira de janelas numa parede curva em volta de você.
* **Túnel**: janelas afundando num túnel que gira.
* **Rolodex**: uma roda vertical, as janelas rolam por cima e por baixo.

Estilos 2D mais simples, para quem quer algo mais perto do padrão:

* **Windows 11 (aprimorado)**: o layout nativo, com as janelas voando até as
  miniaturas e uma seleção que desliza suave.
* **Miniaturas com título embaixo**: painel quadrado, títulos sob as
  miniaturas.
* **Ícones e títulos**: uma fileira de ícones grandes com os títulos.
* **Lista vertical**: uma lista compacta de ícones e títulos.
* **Clássico (Windows XP)**: a grade de ícones antiga com o título numa caixa
  embaixo.
* **Alt+Tab nativo do Windows**: o mod deixa o Alt+Tab em paz.

O alternador abre no monitor onde está o cursor do mouse (ou, se preferir,
onde está a janela ativa).

### Controles

| Tecla / ação                 | O que faz                                  |
|------------------------------|--------------------------------------------|
| Alt+Tab                      | Abre a pilha / próxima janela              |
| Alt+Shift+Tab                | Janela anterior                            |
| Setas, roda do mouse         | Avança / volta na pilha                    |
| Soltar Alt, Enter, Espaço    | Muda para a janela da frente               |
| Clicar numa janela           | Muda para essa janela                      |
| Esc                          | Cancela e volta para onde você estava      |

Um toque rápido no Alt+Tab (menor que o *atraso para exibir*) troca na hora
para a janela anterior sem mostrar a pilha, igual ao alternador nativo.

### Comparação com mods parecidos

* O **[Aero Flip 3D Recreation](https://windhawk.net/mods/aero-flip3d-recreation)**
  traz o Flip 3D de volta no **Win+Tab**. Este mod substitui o **Alt+Tab**,
  desenha as janelas com perspectiva real a partir do conteúdo ao vivo e tem
  mais nove layouts 3D. Os dois podem ficar instalados juntos, porque usam
  atalhos diferentes.
* O **[Simple Window Switcher](https://windhawk.net/mods/simple-window-switcher)**
  e o **[Legacy Alt+Tab dialog](https://windhawk.net/mods/legacy-alt-tab)**
  também substituem o Alt+Tab. Os estilos 2D daqui são parecidos com eles e
  estão incluídos para que todos os estilos, 3D ou 2D, possam ser escolhidos
  num lugar só, com as mesmas animações, teclas e configurações.

**Não use este mod junto com outro substituto do Alt+Tab**, como os dois
acima. Só um deles consegue assumir o Alt+Tab.

### Observações

* Somente Windows 11 (usa o Windows Graphics Capture sem a borda amarela).
* Ctrl+Alt+Tab e Win+Tab não são alterados.
* O alternador roda num processo próprio, em segundo plano, então um problema
  nele não afeta a barra de tarefas nem a área de trabalho. Só a pequena parte
  que trata o caso abaixo roda dentro do explorer.
* Funciona também com um programa rodando como administrador na frente: o mod
  pega o atalho Alt+Tab dentro do explorer antes do alternador nativo abrir e
  repassa para o alternador.
* Janelas minimizadas mostram o último conteúdo, como as miniaturas da barra de
  tarefas (dá para desligar). Janelas que bloqueiam captura aparecem como um
  cartão com o ícone do app.
* Com o fundo *Apenas escurecer*, as janelas reais continuam visíveis atrás da
  pilha. Os fundos com papel de parede as escondem, como no Flip 3D original.
* Se o alternador não conseguir iniciar (ex.: sem dispositivo de GPU), o Alt+Tab
  nativo continua funcionando.

### Créditos

* Mod feito por **caliberda** ([caliberda.com.br](https://caliberda.com.br),
  Instagram [@cesar.kali](https://www.instagram.com/cesar.kali)).
* Inspirado no **Windows Flip 3D** do Windows Vista e Windows 7 (Microsoft).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: flip3d
  $name: Animation style
  $name:pt-BR: Estilo da animação
  $description: How the windows are presented
  $description:pt-BR: Como as janelas são apresentadas
  $options:
  - flip3d: Flip 3D (Vista stack)
  - cascade: Cascade
  - coverflow: Cover Flow
  - carousel: Carousel
  - grid: Grid (Mission Control)
  - helix: Helix
  - fan: Fan
  - panorama: Panorama
  - tunnel: Tunnel
  - rolodex: Rolodex
  - windows11: Windows 11 (enhanced)
  - thumbnails: Thumbnails with titles below
  - icons: Icons and titles
  - list: Vertical list
  - classic: Classic (Windows XP)
  - native: Native Windows Alt+Tab
  $options:pt-BR:
  - flip3d: Flip 3D (pilha do Vista)
  - cascade: Cascata
  - coverflow: Cover Flow
  - carousel: Carrossel
  - grid: Grade (Mission Control)
  - helix: Hélice
  - fan: Leque
  - panorama: Panorama
  - tunnel: Túnel
  - rolodex: Rolodex
  - windows11: Windows 11 (aprimorado)
  - thumbnails: Miniaturas com título embaixo
  - icons: Ícones e títulos
  - list: Lista vertical
  - classic: Clássico (Windows XP)
  - native: Alt+Tab nativo do Windows
- monitor: cursor
  $name: Monitor
  $name:pt-BR: Monitor
  $description: Which monitor the switcher opens on
  $description:pt-BR: Em qual monitor o alternador abre
  $options:
  - cursor: Where the mouse cursor is
  - activeWindow: Where the active window is
  $options:pt-BR:
  - cursor: Onde está o cursor do mouse
  - activeWindow: Onde está a janela ativa
- animationDuration: 420
  $name: Open/close animation duration (ms)
  $name:pt-BR: Duração da animação de abrir/fechar (ms)
  $description: How long the windows take to fly into the stack and back
  $description:pt-BR: Quanto tempo as janelas levam para voar até a pilha e voltar
- flipSpeed: 100
  $name: Flip speed (%)
  $name:pt-BR: Velocidade da troca (%)
  $description: Speed of the spring animation when flipping between windows (25-400)
  $description:pt-BR: Velocidade da animação de mola ao passar entre as janelas (25-400)
- showDelay: 90
  $name: Show delay (ms)
  $name:pt-BR: Atraso para exibir (ms)
  $description: A quick Alt+Tab shorter than this switches instantly without showing the stack
  $description:pt-BR: Um Alt+Tab mais rápido que isso troca na hora, sem mostrar a pilha
- tiltAngle: 30
  $name: Tilt angle (degrees)
  $name:pt-BR: Ângulo de inclinação (graus)
  $description: How much the windows are turned (0-70). Used by Flip 3D, Cascade and Cover Flow
  $description:pt-BR: Quanto as janelas ficam viradas (0-70). Usado pelo Flip 3D, Cascata e Cover Flow
- stackSpacing: 100
  $name: Stack spacing (%)
  $name:pt-BR: Espaçamento da pilha (%)
  $description: Distance between the windows in the stack (40-250)
  $description:pt-BR: Distância entre as janelas na pilha (40-250)
- maxWindows: 20
  $name: Maximum number of windows
  $name:pt-BR: Número máximo de janelas
  $description: Windows beyond this number (by recent use) are not shown (2-40)
  $description:pt-BR: Janelas além desse número (por uso recente) não são mostradas (2-40)
- background: blur
  $name: Background
  $name:pt-BR: Fundo
  $description: What is shown behind the stack
  $description:pt-BR: O que aparece atrás da pilha
  $options:
  - blur: Blurred wallpaper
  - wallpaper: Wallpaper
  - dim: Dim only
  $options:pt-BR:
  - blur: Papel de parede desfocado
  - wallpaper: Papel de parede
  - dim: Apenas escurecer
- blurAmount: 20
  $name: Background blur strength
  $name:pt-BR: Intensidade do desfoque do fundo
  $description: Used by the blurred wallpaper background (1-100)
  $description:pt-BR: Usado pelo fundo com papel de parede desfocado (1-100)
- dimOpacity: 25
  $name: Background dimming (%)
  $name:pt-BR: Escurecimento do fundo (%)
  $description: How dark the background gets (0-90). For "Dim only", around 60 looks best
  $description:pt-BR: Quanto o fundo escurece (0-90). Para "Apenas escurecer", algo perto de 60 fica melhor
- showTitle: true
  $name: Show window title
  $name:pt-BR: Mostrar título da janela
  $description: Shows the icon and title of the window in front
  $description:pt-BR: Mostra o ícone e o título da janela da frente
- shadows: true
  $name: Window shadows
  $name:pt-BR: Sombras das janelas
  $description: Soft shadows under the windows in the stack
  $description:pt-BR: Sombras suaves sob as janelas da pilha
- includeMinimized: true
  $name: Include minimized windows
  $name:pt-BR: Incluir janelas minimizadas
- minimizedContent: true
  $name: Show the content of minimized windows
  $name:pt-BR: Mostrar o conteúdo das janelas minimizadas
  $description: Shows the last content of minimized windows, like the taskbar previews. When off, they're shown as a card with the app icon
  $description:pt-BR: Mostra o último conteúdo das janelas minimizadas, como as miniaturas da barra de tarefas. Desligado, elas aparecem como um cartão com o ícone do app
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <d3d11_4.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi1_3.h>
#include <inspectable.h>
#include <propsys.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>
#include <wincodec.h>
#include <windows.graphics.capture.interop.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Security.Authorization.AppCapabilityAccess.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include <windhawk_utils.h>

namespace wgc = winrt::Windows::Graphics::Capture;
namespace wgdx = winrt::Windows::Graphics::DirectX;
namespace wgd3d = winrt::Windows::Graphics::DirectX::Direct3D11;
using winrt::com_ptr;

namespace {

// The llvm-mingw SDK doesn't ship windows.graphics.directx.direct3d11.interop.h,
// so the interop pieces needed here are declared manually.
struct IDirect3DDxgiInterfaceAccess : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetInterface(REFIID iid, void** p) = 0;
};
constexpr GUID kIID_IDirect3DDxgiInterfaceAccess = {
    0xa9b3d012,
    0x3df2,
    0x4ee3,
    {0xb8, 0xd1, 0x86, 0x95, 0xf4, 0x57, 0xd3, 0xc1}};
constexpr GUID kIID_IGraphicsCaptureItemInterop = {
    0x3628e81b,
    0x3cac,
    0x4c60,
    {0xb7, 0xf4, 0x23, 0xce, 0x0e, 0x0c, 0x33, 0x56}};
constexpr GUID kCLSID_DesktopWallpaper = {
    0xc2cf3110,
    0x460e,
    0x4fc1,
    {0xb9, 0xd0, 0x8a, 0x1c, 0x0c, 0x9c, 0xc4, 0xbd}};
// Used to find the icons of packaged (UWP) apps.
constexpr GUID kFOLDERID_AppsFolder = {
    0x1e87508d,
    0x89c2,
    0x42f0,
    {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};
constexpr PROPERTYKEY kPKEY_AppUserModel_ID = {
    {0x9f4c2855,
     0x9f79,
     0x4b39,
     {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}},
    5};

using CreateDirect3D11DeviceFromDXGIDevice_t = HRESULT(WINAPI*)(IDXGIDevice*,
                                                                ::IInspectable**);

// Effect property values missing from the llvm-mingw d2d1effects.h, as
// documented in the Windows SDK.
constexpr UINT32 kTransform3DPropInterpolationMode = 0;
constexpr UINT32 kTransform3DPropBorderMode = 1;
constexpr UINT32 kTransform3DPropTransformMatrix = 2;
constexpr UINT32 kTransform3DInterpolationAnisotropic = 4;
constexpr UINT32 kShadowOptimizationSpeed = 0;
constexpr UINT32 kGaussianBlurOptimizationQuality = 2;

// Messages posted from the hook thread to the overlay window.
constexpr UINT WM_APP_START = WM_APP + 1;   // wParam: kStart* flags
constexpr UINT WM_APP_STEP = WM_APP + 2;    // wParam: signed step
constexpr UINT WM_APP_WHEEL = WM_APP + 3;   // wParam: signed wheel delta
constexpr UINT WM_APP_COMMIT = WM_APP + 4;  // switch to the front window
constexpr UINT WM_APP_CANCEL = WM_APP + 5;  // go back without switching
constexpr UINT WM_APP_SETTINGS = WM_APP + 6;

constexpr WPARAM kStartBackwards = 1;
// Started from explorer's own Alt+Tab hotkey: the keyboard hook didn't see
// the keys, usually because an elevated window has the focus.
constexpr WPARAM kStartFromHotkey = 2;

// Explorer looks for new GUI threads to hook until it has seen the Alt+Tab
// hotkey (see ExplorerThreadProc).
constexpr DWORD kRescanThreadsIntervalMs = 10000;
// How often explorer checks whether the taskbar has been created yet.
constexpr DWORD kWaitForTaskbarIntervalMs = 1000;

// Thread messages handled by the hook thread.
constexpr UINT WM_HOOK_ENGAGE = WM_APP + 20;
constexpr UINT WM_HOOK_DISENGAGE = WM_APP + 21;
// Installs or removes the keyboard hook, following g_takeOver.
constexpr UINT WM_HOOK_UPDATE = WM_APP + 22;

// Marks input injected by this mod, so the hooks can ignore it.
constexpr ULONG_PTR kInjectedInputTag = 0x46334453;
// Unassigned virtual key, used to stop Alt from activating menu bars.
constexpr WORD kDummyVk = 0xE8;

constexpr WCHAR kOverlayClassName[] = L"WindhawkFlip3DSwitcherOverlay";
constexpr WCHAR kProxyClassName[] = L"WindhawkFlip3DSwitcherProxy";
// The overlay's title tells explorer whether the switcher can take over
// Alt+Tab right now. FindWindow compares titles without sending messages.
constexpr WCHAR kOverlayTitleIdle[] = L"Flip 3D";
constexpr WCHAR kOverlayTitleReady[] = L"Flip 3D (ready)";
// Posted by explorer to the overlay when it receives the Alt+Tab hotkey.
// wParam: kStartBackwards or 0.
constexpr WCHAR kHotkeyMessageName[] = L"WindhawkFlip3DSwitcher_Hotkey";

enum class AnimationStyle {
    Flip3D,
    Cascade,
    CoverFlow,
    Carousel,
    Grid,
    Helix,
    Fan,
    Panorama,
    Tunnel,
    Rolodex,
    Windows11,
    Thumbnails,
    IconRow,
    List,
    Classic,
    // The mod leaves Alt+Tab alone, and keeps no hooks or graphics devices.
    Native,
};
enum class MonitorMode { Cursor, ActiveWindow };
enum class BackgroundMode { BlurredWallpaper, Wallpaper, Dim };

struct Settings {
    AnimationStyle style;
    MonitorMode monitor;
    int animationDurationMs;
    float flipSpeed;
    int showDelayMs;
    float tiltRadians;
    float stackSpacing;
    int maxWindows;
    BackgroundMode background;
    float blurAmount;
    float dimOpacity;
    bool showTitle;
    bool shadows;
    bool includeMinimized;
    bool minimizedContent;
};

Settings LoadSettings() {
    Settings s;

    constexpr struct {
        PCWSTR name;
        AnimationStyle style;
    } kStyles[] = {
        {L"cascade", AnimationStyle::Cascade},
        {L"coverflow", AnimationStyle::CoverFlow},
        {L"carousel", AnimationStyle::Carousel},
        {L"grid", AnimationStyle::Grid},
        {L"helix", AnimationStyle::Helix},
        {L"fan", AnimationStyle::Fan},
        {L"panorama", AnimationStyle::Panorama},
        {L"tunnel", AnimationStyle::Tunnel},
        {L"rolodex", AnimationStyle::Rolodex},
        {L"windows11", AnimationStyle::Windows11},
        {L"thumbnails", AnimationStyle::Thumbnails},
        {L"icons", AnimationStyle::IconRow},
        {L"list", AnimationStyle::List},
        {L"classic", AnimationStyle::Classic},
        {L"native", AnimationStyle::Native},
    };
    const auto style = WindhawkUtils::StringSetting::make(L"style");
    s.style = AnimationStyle::Flip3D;
    for (const auto& entry : kStyles) {
        if (wcscmp(style, entry.name) == 0) {
            s.style = entry.style;
        }
    }

    const auto monitor = WindhawkUtils::StringSetting::make(L"monitor");
    s.monitor = wcscmp(monitor, L"activeWindow") == 0
                    ? MonitorMode::ActiveWindow
                    : MonitorMode::Cursor;

    s.animationDurationMs =
        std::clamp(Wh_GetIntSetting(L"animationDuration"), 50, 3000);
    s.flipSpeed = std::clamp(Wh_GetIntSetting(L"flipSpeed"), 25, 400) / 100.0f;
    s.showDelayMs = std::clamp(Wh_GetIntSetting(L"showDelay"), 0, 1000);
    s.tiltRadians = std::clamp(Wh_GetIntSetting(L"tiltAngle"), 0, 70) *
                    3.14159265f / 180.0f;
    s.stackSpacing =
        std::clamp(Wh_GetIntSetting(L"stackSpacing"), 40, 250) / 100.0f;
    s.maxWindows = std::clamp(Wh_GetIntSetting(L"maxWindows"), 2, 40);

    const auto background = WindhawkUtils::StringSetting::make(L"background");
    if (wcscmp(background, L"wallpaper") == 0) {
        s.background = BackgroundMode::Wallpaper;
    } else if (wcscmp(background, L"dim") == 0) {
        s.background = BackgroundMode::Dim;
    } else {
        s.background = BackgroundMode::BlurredWallpaper;
    }

    s.blurAmount = (float)std::clamp(Wh_GetIntSetting(L"blurAmount"), 1, 100);
    s.dimOpacity = std::clamp(Wh_GetIntSetting(L"dimOpacity"), 0, 90) / 100.0f;
    s.showTitle = Wh_GetIntSetting(L"showTitle") != 0;
    s.shadows = Wh_GetIntSetting(L"shadows") != 0;
    s.includeMinimized = Wh_GetIntSetting(L"includeMinimized") != 0;
    s.minimizedContent = Wh_GetIntSetting(L"minimizedContent") != 0;
    return s;
}

HMODULE g_module;
// See kHotkeyMessageName.
UINT g_hotkeyMessage;
HANDLE g_uiThread;
DWORD g_uiThreadId;
HANDLE g_hookThread;
DWORD g_hookThreadId;
HWND g_overlayWnd;
HHOOK g_keyboardHook;  // Hook thread only.
HHOOK g_mouseHook;     // Hook thread only.

// The switcher is initialized and can take over Alt+Tab.
std::atomic<bool> g_ready;
// Alt+Tab is taken over (false with the native switcher style).
std::atomic<bool> g_takeOver;
// An Alt+Tab session is in progress (Alt is still held).
std::atomic<bool> g_switching;

bool IsPanelStyle(AnimationStyle style) {
    return style == AnimationStyle::Windows11 ||
           style == AnimationStyle::Thumbnails ||
           style == AnimationStyle::IconRow || style == AnimationStyle::List ||
           style == AnimationStyle::Classic;
}

double NowSeconds() {
    static const double frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return (double)f.QuadPart;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart / frequency;
}

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float PositiveMod(float value, float modulus) {
    float result = std::fmod(value, modulus);
    return result < 0 ? result + modulus : result;
}

int PositiveMod(long long value, int modulus) {
    long long result = value % modulus;
    return (int)(result < 0 ? result + modulus : result);
}

bool IsKeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

// Injects an unassigned key press. Sent right after the swallowed Tab, it stops
// the foreground app from treating the Alt release as a menu activation. It
// also makes this process the source of the last input, which allows it to
// call SetForegroundWindow.
void SendDummyKeyPress() {
    INPUT inputs[2] = {};
    for (INPUT& input : inputs) {
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = kDummyVk;
        input.ki.dwExtraInfo = kInjectedInputTag;
    }
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

////////////////////////////////////////////////////////////////////////////////
// Low-level hooks (hook thread).

// Ends the session from any thread. Returns false if it was already ended.
bool EndSwitching() {
    if (!g_switching.exchange(false)) {
        return false;
    }
    PostThreadMessageW(g_hookThreadId, WM_HOOK_DISENGAGE, 0, 0);
    return true;
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    if (kb->dwExtraInfo == kInjectedInputTag) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    const DWORD vk = kb->vkCode;

    if (!g_switching) {
        if (keyDown && vk == VK_TAB && (kb->flags & LLKHF_ALTDOWN) &&
            g_ready && g_takeOver && !IsKeyDown(VK_CONTROL) && !IsKeyDown(VK_LWIN) &&
            !IsKeyDown(VK_RWIN)) {
            g_switching = true;
            PostMessageW(g_overlayWnd, WM_APP_START,
                         IsKeyDown(VK_SHIFT) ? kStartBackwards : 0, 0);
            PostThreadMessageW(g_hookThreadId, WM_HOOK_ENGAGE, 0, 0);
            return 1;
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    switch (vk) {
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            if (!keyDown && EndSwitching()) {
                PostMessageW(g_overlayWnd, WM_APP_COMMIT, 0, 0);
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);

        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_LWIN:
        case VK_RWIN:
            return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    // While switching, every other key belongs to the switcher. Otherwise the
    // foreground app would receive them as Alt+key accelerators.
    if (keyDown) {
        int step = 0;
        switch (vk) {
            case VK_TAB:
                step = IsKeyDown(VK_SHIFT) ? -1 : 1;
                break;
            case VK_RIGHT:
            case VK_DOWN:
                step = 1;
                break;
            case VK_LEFT:
            case VK_UP:
                step = -1;
                break;
            case VK_ESCAPE:
                if (EndSwitching()) {
                    PostMessageW(g_overlayWnd, WM_APP_CANCEL, 0, 0);
                }
                break;
            case VK_RETURN:
            case VK_SPACE:
                if (EndSwitching()) {
                    PostMessageW(g_overlayWnd, WM_APP_COMMIT, 0, 0);
                }
                break;
        }
        if (step) {
            PostMessageW(g_overlayWnd, WM_APP_STEP, (WPARAM)(INT_PTR)step, 0);
        }
    }
    return 1;
}

LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == WM_MOUSEWHEEL && g_switching) {
        const auto* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        if (ms->dwExtraInfo != kInjectedInputTag) {
            const short delta = (short)HIWORD(ms->mouseData);
            PostMessageW(g_overlayWnd, WM_APP_WHEEL, (WPARAM)(INT_PTR)delta,
                         0);
            return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// The keyboard hook is only installed while Alt+Tab is taken over, so the
// native style leaves no system-wide hook behind.
void UpdateKeyboardHook() {
    if (g_takeOver && !g_keyboardHook) {
        g_keyboardHook =
            SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, g_module, 0);
        if (!g_keyboardHook) {
            Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) failed: %u",
                   GetLastError());
        }
    } else if (!g_takeOver && g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}

DWORD WINAPI HookThreadProc(LPVOID parameter) {
    MSG msg;
    // Create the message queue before the creator continues.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    UpdateKeyboardHook();
    SetEvent((HANDLE)parameter);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd) {
            DispatchMessageW(&msg);
            continue;
        }

        switch (msg.message) {
            case WM_HOOK_UPDATE:
                UpdateKeyboardHook();
                break;

            case WM_HOOK_ENGAGE:
                SendDummyKeyPress();
                if (!g_mouseHook) {
                    g_mouseHook =
                        SetWindowsHookExW(WH_MOUSE_LL, MouseProc, g_module, 0);
                }
                break;

            case WM_HOOK_DISENGAGE:
                if (g_mouseHook && !g_switching) {
                    UnhookWindowsHookEx(g_mouseHook);
                    g_mouseHook = nullptr;
                }
                break;
        }
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Explorer's Alt+Tab hotkey (explorer.exe).
//
// The native switcher is started by a hotkey that explorer registers for
// itself. Normally the keyboard hook swallows the Tab before hotkeys are
// processed, but low-level hooks don't see the keys when an elevated window
// has the focus, and physical key presses can slip past them. Hotkeys are
// delivered in every case, so explorer's message retrieval is watched too, and
// the hotkey is handed to the switcher instead of opening the native one.
//
// This is the only part of the mod that runs inside explorer. The hotkey
// always arrives on the same explorer thread, but which one isn't known in
// advance, so every GUI thread is hooked until the hotkey has been seen. Then
// the other hooks are removed and no more threads are looked for.

struct MessageHook {
    DWORD threadId;
    HHOOK hook;
};

std::vector<MessageHook> g_messageHooks;  // Explorer thread only.
std::atomic<int> g_messageHookCalls;
// The thread that received the Alt+Tab hotkey, 0 until then.
std::atomic<DWORD> g_hotkeyThreadId;
HANDLE g_hotkeySeenEvent;
HANDLE g_explorerStopEvent;
HANDLE g_explorerThread;

// Hands the hotkey to the switcher. Returns false if the switcher isn't running
// or can't take over Alt+Tab right now; the native switcher then opens.
bool ForwardHotkey(bool backwards) {
    HWND overlay = FindWindowW(kOverlayClassName, kOverlayTitleReady);
    if (!overlay) {
        return false;
    }

    // Explorer has just received the hotkey, so it may pass the foreground
    // on. The switcher needs it to take the keyboard focus.
    DWORD processId = 0;
    GetWindowThreadProcessId(overlay, &processId);
    AllowSetForegroundWindow(processId);

    return PostMessageW(overlay, g_hotkeyMessage,
                        backwards ? kStartBackwards : 0, 0) != FALSE;
}

LRESULT CALLBACK GetMessageProc(int code, WPARAM wParam, LPARAM lParam) {
    g_messageHookCalls++;

    auto* msg = reinterpret_cast<MSG*>(lParam);
    if (code == HC_ACTION && wParam == PM_REMOVE &&
        msg->message == WM_HOTKEY && HIWORD(msg->lParam) == VK_TAB) {
        const UINT modifiers = LOWORD(msg->lParam);
        if ((modifiers & MOD_ALT) && !(modifiers & (MOD_CONTROL | MOD_WIN))) {
            if (!g_hotkeyThreadId) {
                g_hotkeyThreadId = GetCurrentThreadId();
                SetEvent(g_hotkeySeenEvent);
            }
            if (ForwardHotkey((modifiers & MOD_SHIFT) != 0)) {
                Wh_Log(L"Alt+Tab reached explorer's hotkey, handing it over");
                // Explorer gets an empty message instead of the hotkey.
                msg->message = WM_NULL;
            }
        }
    }

    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    g_messageHookCalls--;
    return result;
}

bool IsThreadAlive(DWORD threadId) {
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread) {
        return false;
    }
    const bool alive = WaitForSingleObject(thread, 0) == WAIT_TIMEOUT;
    CloseHandle(thread);
    return alive;
}

// Installs a WH_GETMESSAGE hook on every GUI thread of this process that
// doesn't have one yet. These are thread hooks inside explorer itself, so no
// code is injected anywhere else.
void HookMessageThreads() {
    const DWORD processId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    // Forget hooks of threads that ended, their IDs can be reused.
    std::erase_if(g_messageHooks, [](const MessageHook& entry) {
        if (IsThreadAlive(entry.threadId)) {
            return false;
        }
        UnhookWindowsHookEx(entry.hook);
        return true;
    });

    THREADENTRY32 entry{sizeof(entry)};
    for (BOOL more = Thread32First(snapshot, &entry); more;
         more = Thread32Next(snapshot, &entry)) {
        const DWORD threadId = entry.th32ThreadID;
        if (entry.th32OwnerProcessID != processId ||
            threadId == GetCurrentThreadId()) {
            continue;
        }
        if (std::any_of(g_messageHooks.begin(), g_messageHooks.end(),
                        [threadId](const MessageHook& hook) {
                            return hook.threadId == threadId;
                        })) {
            continue;
        }

        // Only threads with a message queue can receive the hotkey.
        GUITHREADINFO info{sizeof(info)};
        if (!GetGUIThreadInfo(threadId, &info)) {
            continue;
        }
        if (HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, GetMessageProc,
                                           g_module, threadId)) {
            g_messageHooks.push_back({threadId, hook});
        }
    }

    CloseHandle(snapshot);
}

// Keeps only the hook of the thread that receives the hotkey.
void PruneMessageHooks(DWORD keepThreadId) {
    std::erase_if(g_messageHooks, [keepThreadId](const MessageHook& entry) {
        if (entry.threadId == keepThreadId) {
            return false;
        }
        UnhookWindowsHookEx(entry.hook);
        return true;
    });
}

void UnhookMessageThreads() {
    for (const MessageHook& entry : g_messageHooks) {
        UnhookWindowsHookEx(entry.hook);
    }
    g_messageHooks.clear();

    // Let calls already running on other threads leave the module before it
    // gets unloaded. No new calls can start after the hooks are removed, so
    // this always ends.
    const ULONGLONG start = GetTickCount64();
    bool logged = false;
    while (g_messageHookCalls > 0) {
        if (!logged && GetTickCount64() - start > 1000) {
            Wh_Log(L"Still waiting for message hook calls to return");
            logged = true;
        }
        Sleep(1);
    }
}

// Returns false if the stop event was signaled while waiting.
bool WaitOrStop(HANDLE event, DWORD timeout) {
    HANDLE events[] = {g_explorerStopEvent, event};
    const DWORD count = event ? 2 : 1;
    return WaitForMultipleObjects(count, events, FALSE, timeout) !=
           WAIT_OBJECT_0;
}

DWORD WINAPI ExplorerThreadProc(LPVOID) {
    // Only the shell process (the one with the taskbar) receives the hotkey.
    // With "Launch folder windows in a separate process", folder windows run
    // in other explorer processes, which have nothing to do.
    for (;;) {
        if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            DWORD processId = 0;
            GetWindowThreadProcessId(taskbar, &processId);
            if (processId != GetCurrentProcessId()) {
                Wh_Log(L"Not the shell process, nothing to do");
                return 0;
            }
            break;
        }
        // Explorer is still starting up.
        if (!WaitOrStop(nullptr, kWaitForTaskbarIntervalMs)) {
            return 0;
        }
    }

    bool stopped = false;
    while (!stopped) {
        // Look for GUI threads until the hotkey has been seen.
        while (!g_hotkeyThreadId) {
            HookMessageThreads();
            if (!WaitOrStop(g_hotkeySeenEvent, kRescanThreadsIntervalMs)) {
                stopped = true;
                break;
            }
        }
        if (stopped) {
            break;
        }

        const DWORD hotkeyThreadId = g_hotkeyThreadId;
        PruneMessageHooks(hotkeyThreadId);

        // If that thread ever ends, look for the new one.
        HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, hotkeyThreadId);
        stopped = !WaitOrStop(thread, INFINITE);
        if (thread) {
            CloseHandle(thread);
        }
        g_hotkeyThreadId = 0;
        ResetEvent(g_hotkeySeenEvent);
    }

    UnhookMessageThreads();
    return 0;
}

void StopExplorerPart() {
    if (g_explorerThread) {
        SetEvent(g_explorerStopEvent);
        WaitForSingleObject(g_explorerThread, INFINITE);
        CloseHandle(g_explorerThread);
        g_explorerThread = nullptr;
    }
    if (g_explorerStopEvent) {
        CloseHandle(g_explorerStopEvent);
        g_explorerStopEvent = nullptr;
    }
    if (g_hotkeySeenEvent) {
        CloseHandle(g_hotkeySeenEvent);
        g_hotkeySeenEvent = nullptr;
    }
    g_hotkeyThreadId = 0;
}

// Doesn't wait for anything, so explorer's startup isn't delayed.
void StartExplorerPart() {
    if (LoadSettings().style == AnimationStyle::Native) {
        // Nothing to hand over.
        return;
    }

    g_explorerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hotkeySeenEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_explorerStopEvent || !g_hotkeySeenEvent) {
        StopExplorerPart();
        return;
    }
    g_explorerThread =
        CreateThread(nullptr, 0, ExplorerThreadProc, nullptr, 0, nullptr);
    if (!g_explorerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        StopExplorerPart();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window list.

bool IsSwitchableWindow(HWND hwnd) {
    if (hwnd == g_overlayWnd || !IsWindowVisible(hwnd) ||
        hwnd == GetShellWindow()) {
        return false;
    }

    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_APPWINDOW)) {
        if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
            return false;
        }
        if (GetWindow(hwnd, GW_OWNER)) {
            return false;
        }
    }

    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    WCHAR className[64];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        for (PCWSTR excluded : {L"Progman", L"WorkerW", L"Shell_TrayWnd",
                                L"Shell_SecondaryTrayWnd"}) {
            if (wcscmp(className, excluded) == 0) {
                return false;
            }
        }
    }
    return true;
}

// Window bounds in physical pixels. For minimized windows, the restored bounds.
bool GetWindowBounds(HWND hwnd, bool minimized, RECT* rect) {
    if (minimized) {
        WINDOWPLACEMENT placement{sizeof(placement)};
        if (!GetWindowPlacement(hwnd, &placement)) {
            return false;
        }
        // rcNormalPosition is in workspace coordinates.
        *rect = placement.rcNormalPosition;
        HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{sizeof(info)};
        if (GetMonitorInfoW(monitor, &info)) {
            if (placement.flags & WPF_RESTORETOMAXIMIZED) {
                *rect = info.rcWork;
            } else if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) &
                         WS_EX_TOOLWINDOW)) {
                OffsetRect(rect, info.rcWork.left - info.rcMonitor.left,
                           info.rcWork.top - info.rcMonitor.top);
            }
        }
    } else if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                            rect, sizeof(*rect)))) {
        if (!GetWindowRect(hwnd, rect)) {
            return false;
        }
    }
    return rect->right - rect->left > 0 && rect->bottom - rect->top > 0;
}

HICON GetWindowIcon(HWND hwnd) {
    DWORD_PTR result = 0;
    for (WPARAM type : {(WPARAM)ICON_BIG, (WPARAM)ICON_SMALL2}) {
        if (SendMessageTimeoutW(hwnd, WM_GETICON, type, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &result) &&
            result) {
            return (HICON)result;
        }
    }
    if (HICON icon = (HICON)GetClassLongPtrW(hwnd, GCLP_HICON)) {
        return icon;
    }
    return (HICON)GetClassLongPtrW(hwnd, GCLP_HICONSM);
}

////////////////////////////////////////////////////////////////////////////////
// Geometry.

struct Pose {
    float x = 0, y = 0, z = 0;  // Card center, relative to the screen center.
    float scale = 1;
    float angleY = 0;  // Turn around the vertical axis, in radians.
    float angleX = 0;  // Tilt around the horizontal axis, in radians.
    float angleZ = 0;  // Roll in the screen plane, in radians.
    float opacity = 1;
    float brightness = 1;
    float highlight = 0;  // Selection outline, used by the grid.
};

Pose LerpPose(const Pose& a, const Pose& b, float t) {
    return {Lerp(a.x, b.x, t),
            Lerp(a.y, b.y, t),
            Lerp(a.z, b.z, t),
            Lerp(a.scale, b.scale, t),
            Lerp(a.angleY, b.angleY, t),
            Lerp(a.angleX, b.angleX, t),
            Lerp(a.angleZ, b.angleZ, t),
            Lerp(a.opacity, b.opacity, t),
            Lerp(a.brightness, b.brightness, t),
            Lerp(a.highlight, b.highlight, t)};
}

// Corners in order: top-left, top-right, bottom-right, bottom-left.
struct Quad {
    D2D1_POINT_2F p[4];
};

bool QuadContains(const Quad& quad, D2D1_POINT_2F pt) {
    bool hasPositive = false;
    bool hasNegative = false;
    for (int i = 0; i < 4; i++) {
        const D2D1_POINT_2F& a = quad.p[i];
        const D2D1_POINT_2F& b = quad.p[(i + 1) % 4];
        const float cross =
            (b.x - a.x) * (pt.y - a.y) - (b.y - a.y) * (pt.x - a.x);
        hasPositive |= cross > 0;
        hasNegative |= cross < 0;
    }
    return !(hasPositive && hasNegative);
}

// Projective transform mapping the (0,0)-(width,height) rectangle onto a quad
// (Heckbert's square-to-quad mapping), as a row-vector 4x4 matrix for D2D.
D2D1_MATRIX_4X4_F RectToQuadMatrix(float width, float height, const Quad& q) {
    const float x0 = q.p[0].x, y0 = q.p[0].y;
    const float x1 = q.p[1].x, y1 = q.p[1].y;
    const float x2 = q.p[2].x, y2 = q.p[2].y;
    const float x3 = q.p[3].x, y3 = q.p[3].y;

    const float sx = x0 - x1 + x2 - x3;
    const float sy = y0 - y1 + y2 - y3;

    float a, b, d, e, g, h;
    if (std::fabs(sx) < 1e-3f && std::fabs(sy) < 1e-3f) {
        a = x1 - x0;
        b = x3 - x0;
        d = y1 - y0;
        e = y3 - y0;
        g = 0;
        h = 0;
    } else {
        const float dx1 = x1 - x2, dx2 = x3 - x2;
        const float dy1 = y1 - y2, dy2 = y3 - y2;
        float den = dx1 * dy2 - dx2 * dy1;
        if (std::fabs(den) < 1e-6f) {
            den = den < 0 ? -1e-6f : 1e-6f;
        }
        g = (sx * dy2 - dx2 * sy) / den;
        h = (dx1 * sy - sx * dy1) / den;
        a = x1 - x0 + g * x1;
        b = x3 - x0 + h * x3;
        d = y1 - y0 + g * y1;
        e = y3 - y0 + h * y3;
    }

    D2D1_MATRIX_4X4_F m{};
    m._11 = a / width;
    m._12 = d / width;
    m._14 = g / width;
    m._21 = b / height;
    m._22 = e / height;
    m._24 = h / height;
    m._33 = 1;
    m._41 = x0;
    m._42 = y0;
    m._44 = 1;
    return m;
}

float EaseOutQuart(float t) {
    const float u = 1 - t;
    return 1 - u * u * u * u;
}

float EaseInOutCubic(float t) {
    const float u = -2 * t + 2;
    return t < 0.5f ? 4 * t * t * t : 1 - u * u * u / 2;
}

struct Tween {
    float from = 0;
    float to = 0;
    double start = 0;
    double duration = 0;
    bool easeInOut = false;

    float Progress(double now) const {
        if (duration <= 0) {
            return 1;
        }
        return (float)std::clamp((now - start) / duration, 0.0, 1.0);
    }
    float Value(double now) const {
        const float p = Progress(now);
        return Lerp(from, to, easeInOut ? EaseInOutCubic(p) : EaseOutQuart(p));
    }
    bool Done(double now) const { return Progress(now) >= 1; }
};

////////////////////////////////////////////////////////////////////////////////
// The switcher (UI thread).

struct EffectChain {
    com_ptr<ID2D1Effect> transform;
    com_ptr<ID2D1Effect> color;
    com_ptr<ID2D1Effect> shadow;
    ID2D1Image* input = nullptr;
};

struct Item {
    HWND hwnd = nullptr;
    std::wstring title;
    RECT rect{};
    bool minimized = false;
    com_ptr<ID2D1Bitmap1> icon;
    com_ptr<ID2D1Bitmap1> placeholder;

    // Minimized windows can't be captured directly. Their DWM thumbnail (the
    // last content, as in the taskbar previews) is shown in an off-screen
    // proxy window, which is captured instead.
    HWND proxy = nullptr;
    HTHUMBNAIL thumbnail = nullptr;

    wgc::GraphicsCaptureItem captureItem{nullptr};
    wgc::Direct3D11CaptureFramePool framePool{nullptr};
    wgc::GraphicsCaptureSession session{nullptr};
    winrt::Windows::Graphics::SizeInt32 poolSize{};
    com_ptr<ID3D11Texture2D> texture;
    com_ptr<ID2D1Bitmap1> content;

    // While an item wraps from the front to the back of the stack, it's drawn
    // twice (flying out, and fading in at the back), so it needs two chains.
    EffectChain chains[2];

    // Title for the panel styles.
    com_ptr<IDWriteTextLayout> label;
    float labelWidth = 0;

    float Width() const { return (float)(rect.right - rect.left); }
    float Height() const { return (float)(rect.bottom - rect.top); }
};

struct DrawEntry {
    Item* item;
    int chain;
    Pose pose;
    float order;  // Larger is farther away, drawn first.
    Quad quad;
};

class Switcher {
   public:
    bool CreateOverlay();
    void Activate();
    void Run();
    void Shutdown();

    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   private:
    enum class State { Idle, Pending, Open, Closing };

    void Deactivate();
    void PublishState();
    bool CreateDeviceResources();
    void ReleaseDeviceResources();
    void HandleDeviceLost();
    bool EnsureSwapChain(UINT width, UINT height);

    void OnHotkey(bool backwards);
    void OnStart(WPARAM flags);
    void OnStep(int step);
    void OnCommit(int index);
    void OnCancel();
    void OnClick(POINT pt);

    void CollectWindows();
    bool CreateProxy(Item& item);
    void StartCaptures();
    bool FirstFramesReady();
    void StartCapture(Item& item);
    void StopCapture(Item& item);
    void PollFrames(Item& item);
    void Teardown();

    void TakeFocus();
    void ShowOverlay();
    void HideOverlay();
    bool AltReleased(double now);
    void BeginClose(bool commit);
    void FinishClose();
    void ActivateWindow(HWND hwnd);

    void ComputeLayout();
    Pose FlatPose(const Item& item) const;
    Pose LayoutPose(const Item& item, int index, float rel) const;
    Quad Project(const Pose& pose, float width, float height) const;
    void BuildDrawList(float t);

    void RenderFrame();
    void DrawCard(const DrawEntry& entry, float t);
    void DrawTitle(float t);
    void ComputePanelLayout();
    D2D1_RECT_F SelectionCell(float* opacity) const;
    void DrawPanelBack(float t);
    void DrawPanelFront(float t);
    bool EnsureLabelFormat();
    void DrawLabel(Item& item,
                   const D2D1_RECT_F& rect,
                   bool centered,
                   D2D1_COLOR_F color);
    bool EnsureChain(EffectChain& chain);

    com_ptr<ID2D1Bitmap1> GetIconBitmap(HWND hwnd);
    com_ptr<ID2D1Bitmap1> CreateIconBitmap(HICON icon);
    com_ptr<ID2D1Bitmap1> CreateAppIconBitmap(HWND hwnd);
    com_ptr<ID2D1Bitmap1> CreatePlaceholder(const Item& item);
    com_ptr<ID2D1Bitmap1> CreateTargetBitmap(UINT width, UINT height);
    void UpdateBackground();

    int SelectedIndex() const;

    Settings m_settings{};
    HWND m_hwnd = nullptr;

    // Device resources.
    com_ptr<ID3D11Device> m_d3dDevice;
    com_ptr<ID3D11DeviceContext> m_d3dContext;
    com_ptr<IDXGIDevice> m_dxgiDevice;
    wgd3d::IDirect3DDevice m_captureDevice{nullptr};
    com_ptr<ID2D1Factory1> m_d2dFactory;
    com_ptr<ID2D1Device> m_d2dDevice;
    com_ptr<ID2D1DeviceContext> m_ctx;
    com_ptr<IDCompositionDevice> m_dcompDevice;
    com_ptr<IDCompositionTarget> m_dcompTarget;
    com_ptr<IDCompositionVisual> m_dcompVisual;
    com_ptr<IDXGISwapChain1> m_swapChain;
    com_ptr<ID2D1Bitmap1> m_targetBitmap;
    com_ptr<ID2D1SolidColorBrush> m_brush;
    com_ptr<IDWriteFactory> m_dwriteFactory;
    com_ptr<IDWriteTextFormat> m_titleFormat;
    com_ptr<IWICImagingFactory> m_wicFactory;
    UINT m_swapWidth = 0;
    UINT m_swapHeight = 0;
    bool m_borderlessRequested = false;

    // Background.
    com_ptr<ID2D1Bitmap1> m_background;
    std::wstring m_backgroundKey;

    // Session.
    State m_state = State::Idle;
    std::vector<std::unique_ptr<Item>> m_items;
    std::vector<DrawEntry> m_drawList;
    RECT m_monitor{};
    // Monitor DPI / 96, for text sizes.
    float m_dpiScale = 1;
    double m_showAt = 0;
    // Captures start once the show delay has passed. The overlay is shown
    // when their first frames arrived, or at this time at the latest.
    bool m_capturing = false;
    double m_captureDeadline = 0;
    double m_lastFrame = 0;
    double m_altReleasedAt = 0;
    Tween m_open;
    bool m_commit = false;
    bool m_selectedOnTopWhenFlat = false;
    // The overlay took the keyboard focus (sessions started from the hotkey).
    bool m_hasFocus = false;
    HWND m_previousForeground = nullptr;
    long long m_target = 0;  // Index of the front item, not wrapped.
    double m_scroll = 0;     // Animated towards m_target.
    double m_velocity = 0;
    int m_wheelRemainder = 0;
    int m_titleIndex = -1;
    com_ptr<IDWriteTextLayout> m_titleLayout;

    // Layout, in pixels.
    float m_width = 0, m_height = 0;
    float m_focal = 0;
    float m_boxWidth = 0, m_boxHeight = 0;
    float m_frontX = 0, m_frontY = 0;
    float m_stepX = 0, m_stepY = 0, m_stepZ = 0;
    int m_gridColumns = 1, m_gridRows = 1;

    // Panel styles: each window's cell and the parts inside it, the panel
    // around them, and the classic style's title box.
    std::vector<D2D1_RECT_F> m_cells;
    std::vector<D2D1_RECT_F> m_thumbnails;
    std::vector<D2D1_RECT_F> m_iconRects;
    std::vector<D2D1_RECT_F> m_labelRects;
    D2D1_RECT_F m_panel{};
    D2D1_RECT_F m_caption{};
    float m_labelFontSize = 0;
    com_ptr<IDWriteTextFormat> m_labelFormat;
};

Switcher* g_switcher;

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    if (g_switcher) {
        return g_switcher->HandleMessage(hwnd, msg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Creates the overlay window only. It's quick, so the mod finishes loading
// right away; the graphics devices are created afterwards, in Activate.
bool Switcher::CreateOverlay() {
    m_settings = LoadSettings();
    g_takeOver = m_settings.style != AnimationStyle::Native;

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = g_module;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kOverlayClassName;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassEx failed: %u", GetLastError());
        return false;
    }

    // Proxy windows only host a DWM thumbnail, they draw nothing themselves.
    WNDCLASSEXW proxyClass{sizeof(proxyClass)};
    proxyClass.lpfnWndProc = DefWindowProcW;
    proxyClass.hInstance = g_module;
    proxyClass.lpszClassName = kProxyClassName;
    if (!RegisterClassExW(&proxyClass)) {
        Wh_Log(L"RegisterClassEx failed: %u", GetLastError());
    }

    m_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                 WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
                             kOverlayClassName, kOverlayTitleIdle, WS_POPUP, 0,
                             0, 0, 0, nullptr, nullptr, g_module, nullptr);
    if (!m_hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return false;
    }
    g_overlayWnd = m_hwnd;

    // Explorer posts the hotkey message. Allow it even if this process runs
    // at a higher integrity level than explorer.
    ChangeWindowMessageFilterEx(m_hwnd, g_hotkeyMessage, MSGFLT_ALLOW,
                                nullptr);

    BOOL disableTransitions = TRUE;
    DwmSetWindowAttribute(m_hwnd, DWMWA_TRANSITIONS_FORCEDISABLED,
                          &disableTransitions, sizeof(disableTransitions));
    DWM_WINDOW_CORNER_PREFERENCE corners = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(m_hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners,
                          sizeof(corners));
    return true;
}

// Gets ready to take over Alt+Tab. With the native style, nothing is created.
void Switcher::Activate() {
    if (!g_takeOver || m_d3dDevice) {
        PublishState();
        return;
    }

    if (!CreateDeviceResources()) {
        // Alt+Tab isn't taken over, so the native switcher keeps working.
        ReleaseDeviceResources();
        PublishState();
        return;
    }

    if (!m_borderlessRequested) {
        m_borderlessRequested = true;
        // Unpackaged processes are granted borderless capture without a
        // prompt. Without it, Windows draws a yellow border around captured
        // windows.
        try {
            const auto status =
                wgc::GraphicsCaptureAccess::RequestAccessAsync(
                    wgc::GraphicsCaptureAccessKind::Borderless)
                    .get();
            if (status != winrt::Windows::Security::Authorization::
                              AppCapabilityAccess::AppCapabilityAccessStatus::
                                  Allowed) {
                Wh_Log(L"Borderless capture not allowed: %d", (int)status);
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"RequestAccessAsync failed: 0x%08X", (UINT)e.code());
        }
    }

    g_ready = true;
    PublishState();
}

// Switched to the native style: let go of everything.
void Switcher::Deactivate() {
    EndSwitching();
    HideOverlay();
    Teardown();
    m_state = State::Idle;
    ReleaseDeviceResources();
    PublishState();
}

// Tells explorer, through the overlay's title, whether to hand the Alt+Tab
// hotkey over (see ForwardHotkey).
void Switcher::PublishState() {
    if (m_hwnd) {
        SetWindowTextW(m_hwnd, g_ready && g_takeOver ? kOverlayTitleReady
                                                     : kOverlayTitleIdle);
    }
}

bool Switcher::CreateDeviceResources() {
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
        m_d3dDevice.put(), nullptr, m_d3dContext.put());
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: 0x%08X", (UINT)hr);
        return false;
    }

    // Capture frame pools use the device from their own threads.
    if (auto multithread = m_d3dDevice.try_as<ID3D11Multithread>()) {
        multithread->SetMultithreadProtected(TRUE);
    }

    m_dxgiDevice = m_d3dDevice.as<IDXGIDevice>();

    auto createCaptureDevice =
        (CreateDirect3D11DeviceFromDXGIDevice_t)GetProcAddress(
            GetModuleHandleW(L"d3d11.dll"),
            "CreateDirect3D11DeviceFromDXGIDevice");
    if (!createCaptureDevice) {
        Wh_Log(L"CreateDirect3D11DeviceFromDXGIDevice not found");
        return false;
    }
    com_ptr<::IInspectable> inspectable;
    hr = createCaptureDevice(m_dxgiDevice.get(), inspectable.put());
    if (FAILED(hr)) {
        Wh_Log(L"CreateDirect3D11DeviceFromDXGIDevice failed: 0x%08X",
               (UINT)hr);
        return false;
    }
    hr = inspectable->QueryInterface(winrt::guid_of<wgd3d::IDirect3DDevice>(),
                                     winrt::put_abi(m_captureDevice));
    if (FAILED(hr)) {
        return false;
    }

    D2D1_FACTORY_OPTIONS options{};
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                           __uuidof(ID2D1Factory1), &options,
                           m_d2dFactory.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"D2D1CreateFactory failed: 0x%08X", (UINT)hr);
        return false;
    }
    hr = m_d2dFactory->CreateDevice(m_dxgiDevice.get(), m_d2dDevice.put());
    if (FAILED(hr)) {
        return false;
    }
    hr = m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                          m_ctx.put());
    if (FAILED(hr)) {
        return false;
    }
    // Everything is in physical pixels.
    m_ctx->SetDpi(96, 96);
    m_ctx->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
    hr = m_ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White),
                                      m_brush.put());
    if (FAILED(hr)) {
        return false;
    }

    hr = DCompositionCreateDevice(m_dxgiDevice.get(),
                                  IID_PPV_ARGS(m_dcompDevice.put()));
    if (FAILED(hr)) {
        Wh_Log(L"DCompositionCreateDevice failed: 0x%08X", (UINT)hr);
        return false;
    }
    hr = m_dcompDevice->CreateTargetForHwnd(m_hwnd, TRUE, m_dcompTarget.put());
    if (FAILED(hr)) {
        return false;
    }
    hr = m_dcompDevice->CreateVisual(m_dcompVisual.put());
    if (FAILED(hr)) {
        return false;
    }
    m_dcompTarget->SetRoot(m_dcompVisual.get());

    if (!m_dwriteFactory) {
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(m_dwriteFactory.put()));
        if (FAILED(hr)) {
            return false;
        }
    }
    if (!m_wicFactory) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                              CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(m_wicFactory.put()));
        if (FAILED(hr)) {
            return false;
        }
    }
    return true;
}

void Switcher::ReleaseDeviceResources() {
    g_ready = false;
    m_titleLayout = nullptr;
    m_titleFormat = nullptr;
    m_titleIndex = -1;
    m_background = nullptr;
    m_backgroundKey.clear();
    if (m_ctx) {
        m_ctx->SetTarget(nullptr);
    }
    m_targetBitmap = nullptr;
    m_swapChain = nullptr;
    m_swapWidth = m_swapHeight = 0;
    m_dcompVisual = nullptr;
    m_dcompTarget = nullptr;
    m_dcompDevice = nullptr;
    m_brush = nullptr;
    m_ctx = nullptr;
    m_d2dDevice = nullptr;
    m_d2dFactory = nullptr;
    m_captureDevice = nullptr;
    m_dxgiDevice = nullptr;
    m_d3dContext = nullptr;
    m_d3dDevice = nullptr;
}

void Switcher::HandleDeviceLost() {
    Wh_Log(L"Graphics device lost, recreating");
    // The Alt release will reach the foreground app normally.
    EndSwitching();
    HideOverlay();
    Teardown();
    m_state = State::Idle;
    ReleaseDeviceResources();
    if (CreateDeviceResources()) {
        g_ready = true;
    } else {
        ReleaseDeviceResources();
    }
    PublishState();
}

bool Switcher::EnsureSwapChain(UINT width, UINT height) {
    if (m_swapChain && m_swapWidth == width && m_swapHeight == height) {
        return true;
    }

    m_ctx->SetTarget(nullptr);
    m_targetBitmap = nullptr;
    m_swapChain = nullptr;
    m_swapWidth = m_swapHeight = 0;

    com_ptr<IDXGIAdapter> adapter;
    com_ptr<IDXGIFactory2> factory;
    if (FAILED(m_dxgiDevice->GetAdapter(adapter.put())) ||
        FAILED(adapter->GetParent(IID_PPV_ARGS(factory.put())))) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    HRESULT hr = factory->CreateSwapChainForComposition(
        m_d3dDevice.get(), &desc, nullptr, m_swapChain.put());
    if (FAILED(hr)) {
        Wh_Log(L"CreateSwapChainForComposition failed: 0x%08X", (UINT)hr);
        return false;
    }

    com_ptr<IDXGISurface> surface;
    hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(surface.put()));
    if (SUCCEEDED(hr)) {
        D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED));
        hr = m_ctx->CreateBitmapFromDxgiSurface(surface.get(), &props,
                                                m_targetBitmap.put());
    }
    if (SUCCEEDED(hr)) {
        hr = m_dcompVisual->SetContent(m_swapChain.get());
    }
    if (SUCCEEDED(hr)) {
        hr = m_dcompDevice->Commit();
    }
    if (FAILED(hr)) {
        Wh_Log(L"Swap chain setup failed: 0x%08X", (UINT)hr);
        m_targetBitmap = nullptr;
        m_swapChain = nullptr;
        return false;
    }

    m_swapWidth = width;
    m_swapHeight = height;
    return true;
}

void Switcher::Shutdown() {
    g_ready = false;
    EndSwitching();
    if (m_hwnd) {
        HideOverlay();
    }
    Teardown();
    ReleaseDeviceResources();
    m_dwriteFactory = nullptr;
    m_wicFactory = nullptr;
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    UnregisterClassW(kOverlayClassName, g_module);
    UnregisterClassW(kProxyClassName, g_module);
}

void Switcher::Run() {
    MSG msg;
    for (;;) {
        if (m_state == State::Idle || m_state == State::Pending) {
            DWORD timeout = INFINITE;
            if (m_state == State::Pending) {
                // Wake up regularly to watch the Alt key (see AltReleased),
                // and often while waiting for the first frames.
                const double remaining = m_showAt - NowSeconds();
                timeout = m_capturing
                              ? 4
                              : (DWORD)std::clamp(std::ceil(remaining * 1000),
                                                  0.0, 15.0);
            }
            MsgWaitForMultipleObjectsEx(0, nullptr, timeout, QS_ALLINPUT,
                                        MWMO_INPUTAVAILABLE);
        }

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (m_state == State::Pending) {
            const double now = NowSeconds();
            if (AltReleased(now) && EndSwitching()) {
                OnCommit(-1);
            } else if (now >= m_showAt) {
                // A quick Alt+Tab never gets here, so it doesn't pay for
                // captures it wouldn't show.
                if (!m_capturing) {
                    StartCaptures();
                    m_captureDeadline = now + 0.15;
                }
                if (FirstFramesReady() || now >= m_captureDeadline) {
                    ShowOverlay();
                }
            }
        }

        if (m_state == State::Open || m_state == State::Closing) {
            // Blocks until the next vertical blank.
            RenderFrame();
        }
    }
}

LRESULT Switcher::HandleMessage(HWND hwnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    if (msg == g_hotkeyMessage && g_hotkeyMessage) {
        OnHotkey((wParam & kStartBackwards) != 0);
        return 0;
    }

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_LBUTTONDOWN:
            OnClick({(short)LOWORD(lParam), (short)HIWORD(lParam)});
            return 0;

        case WM_APP_START:
            OnStart(wParam);
            return 0;

        case WM_APP_STEP:
            OnStep((int)(INT_PTR)wParam);
            return 0;

        case WM_APP_WHEEL:
            m_wheelRemainder += (int)(INT_PTR)wParam;
            while (std::abs(m_wheelRemainder) >= WHEEL_DELTA) {
                // Wheel up brings the previous window to the front.
                const int direction = m_wheelRemainder > 0 ? 1 : -1;
                m_wheelRemainder -= direction * WHEEL_DELTA;
                OnStep(-direction);
            }
            return 0;

        case WM_APP_COMMIT:
            OnCommit(-1);
            return 0;

        case WM_APP_CANCEL:
            OnCancel();
            return 0;

        case WM_APP_SETTINGS:
            m_settings = LoadSettings();
            g_takeOver = m_settings.style != AnimationStyle::Native;
            PostThreadMessageW(g_hookThreadId, WM_HOOK_UPDATE, 0, 0);
            m_backgroundKey.clear();
            m_titleFormat = nullptr;
            if (!g_takeOver) {
                Deactivate();
            } else if (!m_d3dDevice) {
                Activate();
            } else {
                if (m_state != State::Idle) {
                    ComputeLayout();
                }
                PublishState();
            }
            return 0;

        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
            m_backgroundKey.clear();
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int Switcher::SelectedIndex() const {
    if (m_items.empty()) {
        return -1;
    }
    return PositiveMod(m_target, (int)m_items.size());
}

// Explorer received the Alt+Tab hotkey and handed it over: the keyboard hook
// didn't see the keys, usually because an elevated window has the focus.
void Switcher::OnHotkey(bool backwards) {
    if (!g_ready || !g_takeOver) {
        return;
    }
    if (g_switching.exchange(true)) {
        // Tab pressed again while Alt is held.
        OnStep(backwards ? -1 : 1);
        return;
    }
    Wh_Log(L"Alt+Tab reached explorer's hotkey, taking it over");
    PostThreadMessageW(g_hookThreadId, WM_HOOK_ENGAGE, 0, 0);
    OnStart(kStartFromHotkey | (backwards ? kStartBackwards : 0));
}

void Switcher::OnStart(WPARAM flags) {
    if (!g_ready) {
        EndSwitching();
        return;
    }
    const bool backwards = (flags & kStartBackwards) != 0;
    const bool fromHotkey = (flags & kStartFromHotkey) != 0;

    // A new Alt+Tab while the previous one is still animating out.
    if (m_state == State::Closing) {
        FinishClose();
    } else if (m_state != State::Idle) {
        HideOverlay();
        Teardown();
        m_state = State::Idle;
    }

    m_previousForeground = GetForegroundWindow();

    HMONITOR monitor;
    POINT cursor;
    if (m_settings.monitor == MonitorMode::Cursor && GetCursorPos(&cursor)) {
        monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY);
    } else {
        monitor =
            MonitorFromWindow(m_previousForeground, MONITOR_DEFAULTTOPRIMARY);
    }
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(monitor, &info)) {
        EndSwitching();
        return;
    }
    m_monitor = info.rcMonitor;

    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiY = 96;
    }
    if (m_dpiScale != dpiY / 96.0f) {
        m_dpiScale = dpiY / 96.0f;
        m_titleFormat = nullptr;
    }

    CollectWindows();
    if (m_items.empty()) {
        // Nothing to switch to. The rest of the keys go to the apps again.
        EndSwitching();
        return;
    }

    const int count = (int)m_items.size();
    m_target = count > 1 ? (backwards ? count - 1 : 1) : 0;
    m_scroll = (double)m_target;
    m_velocity = 0;
    m_wheelRemainder = 0;
    m_altReleasedAt = 0;
    m_titleIndex = -1;
    m_commit = false;
    m_selectedOnTopWhenFlat = false;

    ComputeLayout();

    m_showAt = NowSeconds() + m_settings.showDelayMs / 1000.0;
    m_capturing = false;
    m_state = State::Pending;

    if (fromHotkey) {
        TakeFocus();
    }
}

// Used when the keyboard hook can't see the keys (e.g. an elevated window has
// the focus). Like the native switcher, the overlay then takes the keyboard
// focus, so the rest of the session goes through the hook again. It stays
// fully transparent until the stack is shown.
void Switcher::TakeFocus() {
    const UINT width = (UINT)(m_monitor.right - m_monitor.left);
    const UINT height = (UINT)(m_monitor.bottom - m_monitor.top);
    if (!EnsureSwapChain(width, height)) {
        return;
    }

    m_ctx->SetTarget(m_targetBitmap.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));
    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    if (SUCCEEDED(hr)) {
        hr = m_swapChain->Present(0, 0);
    }
    if (FAILED(hr)) {
        return;
    }

    SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE,
                      GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE) &
                          ~WS_EX_NOACTIVATE);
    SetWindowPos(m_hwnd, HWND_TOPMOST, m_monitor.left, m_monitor.top, width,
                 height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    // Receiving the hotkey allows this process to change the foreground.
    m_hasFocus = SetForegroundWindow(m_hwnd) != FALSE;
    if (!m_hasFocus) {
        Wh_Log(L"Couldn't take the focus");
    }
}

void Switcher::HideOverlay() {
    ShowWindow(m_hwnd, SW_HIDE);
    SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE,
                      GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE) |
                          WS_EX_NOACTIVATE);
    m_hasFocus = false;
}

// Fallback for a missed Alt release (e.g. a secure desktop took the input, or
// the keys never reached the hook). The hook normally reports it first, so
// it only counts once Alt stays released for a moment.
bool Switcher::AltReleased(double now) {
    if (!g_switching || IsKeyDown(VK_MENU)) {
        m_altReleasedAt = 0;
        return false;
    }
    if (!m_altReleasedAt) {
        m_altReleasedAt = now;
    }
    return now - m_altReleasedAt > 0.15;
}

void Switcher::OnStep(int step) {
    if ((m_state != State::Pending && m_state != State::Open) ||
        m_items.size() < 2) {
        return;
    }
    m_target += step;
    if (m_state == State::Pending) {
        m_scroll = (double)m_target;
    }
}

void Switcher::OnCommit(int index) {
    if (m_state == State::Idle || m_state == State::Closing) {
        return;
    }

    if (index >= 0 && !m_items.empty()) {
        // Bring the chosen item to the front the shortest way.
        const int count = (int)m_items.size();
        int delta = index - SelectedIndex();
        if (delta > count / 2) {
            delta -= count;
        } else if (delta < -count / 2) {
            delta += count;
        }
        m_target += delta;
    }

    if (m_state == State::Pending) {
        // Quick Alt+Tab: switch right away, without showing anything.
        const int selected = SelectedIndex();
        HWND target = selected >= 0 ? m_items[selected]->hwnd : nullptr;
        if (target) {
            ActivateWindow(target);
        }
        HideOverlay();
        Teardown();
        m_state = State::Idle;
        return;
    }

    BeginClose(true);
}

void Switcher::OnCancel() {
    if (m_state == State::Pending) {
        // If the overlay took the focus, give it back.
        if (m_hasFocus && m_previousForeground) {
            ActivateWindow(m_previousForeground);
        }
        HideOverlay();
        Teardown();
        m_state = State::Idle;
    } else if (m_state == State::Open) {
        BeginClose(false);
    }
}

void Switcher::OnClick(POINT pt) {
    if (m_state != State::Open) {
        return;
    }
    const D2D1_POINT_2F point{(float)pt.x, (float)pt.y};
    if (IsPanelStyle(m_settings.style)) {
        for (size_t i = 0; i < m_cells.size(); i++) {
            const D2D1_RECT_F& cell = m_cells[i];
            if (point.x >= cell.left && point.x < cell.right &&
                point.y >= cell.top && point.y < cell.bottom) {
                EndSwitching();
                OnCommit((int)i);
                return;
            }
        }
        return;
    }
    for (auto it = m_drawList.rbegin(); it != m_drawList.rend(); ++it) {
        if (it->pose.opacity < 0.5f || !QuadContains(it->quad, point)) {
            continue;
        }
        for (size_t i = 0; i < m_items.size(); i++) {
            if (m_items[i].get() == it->item) {
                EndSwitching();
                OnCommit((int)i);
                return;
            }
        }
    }
}

struct EnumWindowsContext {
    std::vector<HWND> windows;
    size_t max;
    bool includeMinimized;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<EnumWindowsContext*>(lParam);
    if (IsSwitchableWindow(hwnd) &&
        (context->includeMinimized || !IsIconic(hwnd))) {
        context->windows.push_back(hwnd);
    }
    return context->windows.size() < context->max;
}

void Switcher::CollectWindows() {
    EnumWindowsContext context{
        {}, (size_t)m_settings.maxWindows, m_settings.includeMinimized};
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&context));

    // EnumWindows returns the Z order, which puts topmost windows first. The
    // foreground window must come first, like in the native switcher.
    HWND foreground = GetForegroundWindow();
    auto it =
        std::find(context.windows.begin(), context.windows.end(), foreground);
    if (it != context.windows.end()) {
        std::rotate(context.windows.begin(), it, it + 1);
    }

    m_items.clear();
    for (HWND hwnd : context.windows) {
        auto item = std::make_unique<Item>();
        item->hwnd = hwnd;
        item->minimized = IsIconic(hwnd);
        if (!GetWindowBounds(hwnd, item->minimized, &item->rect)) {
            continue;
        }

        WCHAR title[256];
        const int length = GetWindowTextW(hwnd, title, ARRAYSIZE(title));
        item->title.assign(title, std::max(length, 0));

        item->icon = GetIconBitmap(hwnd);
        item->placeholder = CreatePlaceholder(*item);
        if (!item->placeholder) {
            continue;
        }
        m_items.push_back(std::move(item));
    }
}

bool Switcher::CreateProxy(Item& item) {
    item.proxy = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
        kProxyClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, g_module,
        nullptr);
    if (!item.proxy) {
        return false;
    }

    BOOL disableTransitions = TRUE;
    DwmSetWindowAttribute(item.proxy, DWMWA_TRANSITIONS_FORCEDISABLED,
                          &disableTransitions, sizeof(disableTransitions));

    SIZE size{};
    if (FAILED(DwmRegisterThumbnail(item.proxy, item.hwnd, &item.thumbnail)) ||
        FAILED(DwmQueryThumbnailSourceSize(item.thumbnail, &size)) ||
        size.cx <= 0 || size.cy <= 0) {
        return false;
    }

    DWM_THUMBNAIL_PROPERTIES properties{};
    properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
                         DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
    properties.rcDestination = {0, 0, size.cx, size.cy};
    properties.fVisible = TRUE;
    properties.opacity = 255;
    properties.fSourceClientAreaOnly = FALSE;
    if (FAILED(DwmUpdateThumbnailProperties(item.thumbnail, &properties))) {
        return false;
    }

    // To the left of all monitors, where nobody can see or click it.
    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN) - size.cx - 100;
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    return SetWindowPos(item.proxy, HWND_BOTTOM, x, y, size.cx, size.cy,
                        SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void Switcher::StartCaptures() {
    m_capturing = true;
    for (auto& item : m_items) {
        StartCapture(*item);
    }
}

// Whether every capture delivered its first frame, so the stack can open with
// the windows' content instead of placeholders.
bool Switcher::FirstFramesReady() {
    bool ready = true;
    for (auto& item : m_items) {
        PollFrames(*item);
        if (item->framePool && !item->content) {
            ready = false;
        }
    }
    return ready;
}

void Switcher::StartCapture(Item& item) {
    HWND source = item.hwnd;
    if (item.minimized) {
        if (!m_settings.minimizedContent) {
            return;
        }
        if (!CreateProxy(item)) {
            Wh_Log(L"Proxy failed for %p", item.hwnd);
            StopCapture(item);
            return;
        }
        source = item.proxy;
    }

    try {
        auto factory =
            winrt::get_activation_factory<wgc::GraphicsCaptureItem>();
        com_ptr<IGraphicsCaptureItemInterop> interop;
        winrt::check_hresult(winrt::get_unknown(factory)->QueryInterface(
            kIID_IGraphicsCaptureItemInterop, interop.put_void()));
        winrt::check_hresult(interop->CreateForWindow(
            source, winrt::guid_of<wgc::GraphicsCaptureItem>(),
            winrt::put_abi(item.captureItem)));

        const auto size = item.captureItem.Size();
        if (size.Width <= 0 || size.Height <= 0) {
            StopCapture(item);
            return;
        }

        item.framePool = wgc::Direct3D11CaptureFramePool::CreateFreeThreaded(
            m_captureDevice, wgdx::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2, size);
        item.poolSize = size;
        item.session = item.framePool.CreateCaptureSession(item.captureItem);

        // Both are optional: older builds lack them, and the border can be
        // enforced by policy.
        try {
            item.session.IsCursorCaptureEnabled(false);
        } catch (...) {
        }
        try {
            item.session.IsBorderRequired(false);
        } catch (...) {
        }

        item.session.StartCapture();
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Capture failed for %p: 0x%08X", item.hwnd, (UINT)e.code());
        StopCapture(item);
    }
}

void Switcher::StopCapture(Item& item) {
    try {
        if (item.session) {
            item.session.Close();
        }
        if (item.framePool) {
            item.framePool.Close();
        }
    } catch (...) {
    }
    item.session = nullptr;
    item.framePool = nullptr;
    item.captureItem = nullptr;

    if (item.thumbnail) {
        DwmUnregisterThumbnail(item.thumbnail);
        item.thumbnail = nullptr;
    }
    if (item.proxy) {
        DestroyWindow(item.proxy);
        item.proxy = nullptr;
    }
}

void Switcher::PollFrames(Item& item) {
    if (!item.framePool) {
        return;
    }

    try {
        wgc::Direct3D11CaptureFrame latest{nullptr};
        while (auto frame = item.framePool.TryGetNextFrame()) {
            if (latest) {
                latest.Close();
            }
            latest = frame;
        }
        if (!latest) {
            return;
        }

        const auto contentSize = latest.ContentSize();
        com_ptr<IDirect3DDxgiInterfaceAccess> access;
        winrt::check_hresult(
            winrt::get_unknown(latest.Surface())
                ->QueryInterface(kIID_IDirect3DDxgiInterfaceAccess,
                                 access.put_void()));
        com_ptr<ID3D11Texture2D> source;
        winrt::check_hresult(access->GetInterface(IID_PPV_ARGS(source.put())));

        D3D11_TEXTURE2D_DESC sourceDesc;
        source->GetDesc(&sourceDesc);
        const UINT width = std::min((UINT)std::max(contentSize.Width, 1),
                                    sourceDesc.Width);
        const UINT height = std::min((UINT)std::max(contentSize.Height, 1),
                                     sourceDesc.Height);

        D3D11_TEXTURE2D_DESC desc{};
        if (item.texture) {
            item.texture->GetDesc(&desc);
        }
        if (!item.texture || desc.Width != width || desc.Height != height) {
            desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            com_ptr<ID3D11Texture2D> texture;
            com_ptr<ID2D1Bitmap1> content;
            winrt::check_hresult(
                m_d3dDevice->CreateTexture2D(&desc, nullptr, texture.put()));
            D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_NONE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED));
            winrt::check_hresult(m_ctx->CreateBitmapFromDxgiSurface(
                texture.as<IDXGISurface>().get(), &props, content.put()));
            item.texture = texture;
            item.content = content;
        }

        const D3D11_BOX box{0, 0, 0, width, height, 1};
        m_d3dContext->CopySubresourceRegion(item.texture.get(), 0, 0, 0, 0,
                                            source.get(), 0, &box);
        latest.Close();

        if (contentSize.Width != item.poolSize.Width ||
            contentSize.Height != item.poolSize.Height) {
            item.framePool.Recreate(
                m_captureDevice,
                wgdx::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2,
                contentSize);
            item.poolSize = contentSize;
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Frame update failed for %p: 0x%08X", item.hwnd,
               (UINT)e.code());
        StopCapture(item);
    }
}

void Switcher::Teardown() {
    for (auto& item : m_items) {
        StopCapture(*item);
    }
    m_drawList.clear();
    m_items.clear();
    m_titleLayout = nullptr;
    m_titleIndex = -1;
    m_capturing = false;
}

void Switcher::ShowOverlay() {
    const UINT width = (UINT)(m_monitor.right - m_monitor.left);
    const UINT height = (UINT)(m_monitor.bottom - m_monitor.top);
    if (!EnsureSwapChain(width, height)) {
        HandleDeviceLost();
        return;
    }
    UpdateBackground();
    if (!m_capturing) {
        StartCaptures();
    }

    m_open = {0, 1, NowSeconds(), m_settings.animationDurationMs / 1000.0,
              false};
    m_lastFrame = NowSeconds();
    m_state = State::Open;

    // Present the first frame (identical to the desktop) before showing the
    // window, so nothing stale ever appears.
    RenderFrame();
    if (m_state != State::Open) {
        return;
    }
    SetWindowPos(m_hwnd, HWND_TOPMOST, m_monitor.left, m_monitor.top, width,
                 height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void Switcher::BeginClose(bool commit) {
    const double now = NowSeconds();
    const float current = m_open.Value(now);
    m_open = {current, 0, now,
              m_settings.animationDurationMs * 0.85 / 1000.0 * current, true};
    m_commit = commit;
    m_selectedOnTopWhenFlat = commit;
    m_state = State::Closing;
}

void Switcher::FinishClose() {
    HWND target = nullptr;
    if (m_commit) {
        const int selected = SelectedIndex();
        if (selected >= 0) {
            target = m_items[selected]->hwnd;
        }
    } else if (m_hasFocus) {
        // Canceled after the overlay took the focus: give it back.
        target = m_previousForeground;
    }

    if (target) {
        ActivateWindow(target);
        // Give DWM a frame to bring the window up before revealing it.
        DwmFlush();
    }
    HideOverlay();
    Teardown();
    m_state = State::Idle;
}

void Switcher::ActivateWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }

    HWND popup = GetLastActivePopup(hwnd);
    if (!popup || !IsWindowVisible(popup) || !IsWindowEnabled(popup)) {
        popup = hwnd;
    }

    if (IsIconic(hwnd)) {
        ShowWindowAsync(hwnd, SW_RESTORE);
    }

    SendDummyKeyPress();
    if (!SetForegroundWindow(popup)) {
        Wh_Log(L"SetForegroundWindow failed for %p", popup);
    }
}

void Switcher::ComputeLayout() {
    m_width = (float)(m_monitor.right - m_monitor.left);
    m_height = (float)(m_monitor.bottom - m_monitor.top);
    m_focal = m_width * 0.95f;

    const int count = std::max((int)m_items.size(), 1);
    const float spacing = m_settings.stackSpacing;
    // Long stacks are compressed so that the last window stays on screen.
    const float density = std::min(1.0f, 7.0f / std::max(count - 1, 1)) * spacing;

    switch (m_settings.style) {
        case AnimationStyle::Native:
            // Never laid out: nothing is shown with this style.
            break;

        case AnimationStyle::Flip3D:
            // Like Flip 3D, the stack recedes up and to the left.
            m_boxWidth = m_width * 0.48f;
            m_boxHeight = m_height * 0.50f;
            m_frontX = m_width * 0.12f;
            m_frontY = m_height * 0.05f;
            m_stepX = -m_width * 0.065f * density;
            m_stepY = -m_height * 0.04f * density;
            m_stepZ = m_width * 0.07f * density;
            break;

        case AnimationStyle::Cascade:
            // A deck tilted backwards, receding upwards.
            m_boxWidth = m_width * 0.52f;
            m_boxHeight = m_height * 0.50f;
            m_frontX = 0;
            m_frontY = m_height * 0.10f;
            m_stepX = 0;
            m_stepY = -m_height * 0.075f * density;
            m_stepZ = m_width * 0.09f * density;
            break;

        case AnimationStyle::CoverFlow:
            m_boxWidth = m_width * 0.42f;
            m_boxHeight = m_height * 0.55f;
            m_frontX = m_width * 0.27f;  // Distance of the first side window.
            m_frontY = -m_height * 0.02f;
            m_stepX = m_width * 0.075f * spacing;
            m_stepY = 0;
            m_stepZ = m_width * 0.22f;  // How far back the sides are.
            break;

        case AnimationStyle::Carousel:
            m_boxWidth = m_width * 0.38f;
            m_boxHeight = m_height * 0.45f;
            m_frontX = 0;
            m_frontY = m_height * 0.08f;
            m_stepX = m_width * 0.30f * spacing;  // Ring radius.
            m_stepY = 0;
            m_stepZ = 2 * 3.14159265f / std::max(count, 5);  // Angle per window.
            break;

        case AnimationStyle::Grid: {
            // As many columns as make the cells closest to a 16:10 window.
            const float areaWidth = m_width * 0.92f;
            const float areaHeight = m_height * 0.78f;
            m_gridColumns = std::clamp(
                (int)std::ceil(std::sqrt(count * areaWidth / areaHeight / 1.6f)),
                1, count);
            m_gridRows = (count + m_gridColumns - 1) / m_gridColumns;
            m_boxWidth = areaWidth / m_gridColumns;
            m_boxHeight = areaHeight / m_gridRows;
            m_frontX = 0;
            m_frontY = m_height * 0.05f - m_height / 2;  // Top of the grid.
            m_stepX = m_stepY = m_stepZ = 0;
            break;
        }

        case AnimationStyle::Helix:
            m_boxWidth = m_width * 0.36f;
            m_boxHeight = m_height * 0.40f;
            m_frontX = 0;
            m_frontY = m_height * 0.02f;
            m_stepX = m_width * 0.28f * spacing;   // Radius.
            m_stepY = m_height * 0.09f * spacing;  // Rise per window.
            m_stepZ = 0.75f;                       // Angle per window.
            break;

        case AnimationStyle::Fan:
            m_boxWidth = m_width * 0.34f;
            m_boxHeight = m_height * 0.42f;
            m_frontX = 0;
            m_frontY = m_height * 0.95f;           // Pivot, below the screen.
            m_stepX = m_height * 0.95f;            // Distance to the pivot.
            m_stepY = 0;
            m_stepZ = 0.16f * spacing;             // Angle per window.
            break;

        case AnimationStyle::Panorama:
            m_boxWidth = m_width * 0.26f;
            m_boxHeight = m_height * 0.36f;
            m_frontX = 0;
            m_frontY = -m_height * 0.02f;
            m_stepX = m_width * 1.4f;              // Wall radius.
            m_stepY = 0;
            m_stepZ = m_width * 0.29f * spacing / m_stepX;  // Angle per window.
            break;

        case AnimationStyle::Tunnel:
            m_boxWidth = m_width * 0.42f;
            m_boxHeight = m_height * 0.44f;
            m_frontX = 0;
            m_frontY = m_height * 0.08f;
            m_stepX = 0;
            m_stepY = -m_height * 0.06f * density;  // Deeper ones peek above.
            m_stepZ = m_width * 0.12f * density;
            break;

        case AnimationStyle::Windows11:
        case AnimationStyle::Thumbnails:
        case AnimationStyle::IconRow:
        case AnimationStyle::List:
        case AnimationStyle::Classic:
            ComputePanelLayout();
            break;

        case AnimationStyle::Rolodex:
            m_boxWidth = m_width * 0.36f;
            m_boxHeight = m_height * 0.30f;
            m_frontX = 0;
            m_frontY = -m_height * 0.04f;
            m_stepX = m_height * 0.42f * spacing;  // Wheel radius.
            m_stepY = 0;
            m_stepZ = 0.75f;                       // Angle per window.
            break;
    }
}

Pose Switcher::FlatPose(const Item& item) const {
    Pose pose;
    pose.x = (item.rect.left + item.rect.right) / 2.0f - m_monitor.left -
             m_width / 2;
    pose.y = (item.rect.top + item.rect.bottom) / 2.0f - m_monitor.top -
             m_height / 2;
    pose.scale = item.minimized ? 0.85f : 1.0f;
    pose.opacity = item.minimized ? 0.0f : 1.0f;
    return pose;
}

// The pose of a window in the switcher. For the stacked styles, `rel` is the
// slot (0 is the front, negative is leaving the front). For the others, it's
// the signed distance from the selected window.
Pose Switcher::LayoutPose(const Item& item, int index, float rel) const {
    Pose pose;
    const float fit =
        std::min({m_boxWidth / item.Width(), m_boxHeight / item.Height(), 1.0f});
    pose.scale = fit;
    const float tilt = m_settings.tiltRadians;
    const float distance = std::fabs(rel);
    const float side = rel < 0 ? -1.0f : 1.0f;

    switch (m_settings.style) {
        case AnimationStyle::Native:
            break;

        case AnimationStyle::Flip3D:
        case AnimationStyle::Cascade: {
            const bool flip = m_settings.style != AnimationStyle::Cascade;
            if (flip) {
                pose.angleY = tilt;
            } else {
                pose.angleX = tilt;
            }
            if (rel >= 0) {
                pose.x = m_frontX + rel * m_stepX;
                pose.y = m_frontY + rel * m_stepY;
                pose.z = rel * m_stepZ;
                pose.brightness = 1 - std::min(rel * 0.07f, 0.55f);
            } else if (flip) {
                // Leaving the front: fly towards the viewer and fade out.
                pose.x = m_frontX - rel * m_width * 0.22f;
                pose.y = m_frontY - rel * m_height * 0.10f;
                pose.z = rel * m_focal * 0.45f;
                pose.opacity = (1 + rel) * (1 + rel);
            } else {
                // Leaving the front: drop down and fade out.
                pose.x = m_frontX;
                pose.y = m_frontY - rel * m_height * 0.45f;
                pose.z = rel * m_focal * 0.25f;
                pose.opacity = (1 + rel) * (1 + rel);
            }
            break;
        }

        case AnimationStyle::CoverFlow: {
            // The selected window faces the viewer; the others are turned
            // towards it on both sides.
            const float inner = std::min(distance, 1.0f);
            const float outer = std::max(distance - 1, 0.0f);
            pose.x = side * (inner * m_frontX + outer * m_stepX);
            pose.y = m_frontY;
            pose.z = inner * m_stepZ + outer * m_width * 0.01f;
            pose.angleY =
                std::clamp(rel, -1.0f, 1.0f) * std::min(tilt * 2.2f, 1.3f);
            pose.brightness = 1 - inner * 0.2f - std::min(outer * 0.04f, 0.3f);
            pose.opacity = std::clamp(8 - distance, 0.0f, 1.0f);
            break;
        }

        case AnimationStyle::Carousel: {
            // A ring seen slightly from above, rotating around its center.
            const float angle = rel * m_stepZ;
            pose.x = m_stepX * std::sin(angle);
            pose.z = m_stepX * (1 - std::cos(angle));
            pose.y = m_frontY - pose.z * 0.18f;
            pose.angleY = angle * 0.55f;
            pose.brightness = 1 - (1 - std::cos(angle)) * 0.22f;
            pose.opacity = std::clamp(3.5f - std::fabs(angle), 0.0f, 1.0f);
            break;
        }

        case AnimationStyle::Grid: {
            const int row = index / m_gridColumns;
            const int column = index % m_gridColumns;
            const int inRow = std::min(
                m_gridColumns, (int)m_items.size() - row * m_gridColumns);
            const float selected = std::max(0.0f, 1 - distance);
            pose.scale = std::min({m_boxWidth * 0.88f / item.Width(),
                                   m_boxHeight * 0.84f / item.Height(), 1.0f}) *
                         (1 + selected * 0.05f);
            pose.x = (column - (inRow - 1) / 2.0f) * m_boxWidth;
            pose.y = m_frontY + (row + 0.5f) * m_boxHeight;
            pose.z = -selected * m_width * 0.02f;
            pose.brightness = 0.72f + selected * 0.28f;
            pose.highlight = selected;
            break;
        }

        case AnimationStyle::Helix: {
            // A spiral around a vertical axis: earlier windows go up and
            // around, later ones down and around.
            const float angle = rel * m_stepZ;
            pose.x = m_stepX * std::sin(angle);
            pose.z = m_stepX * (1 - std::cos(angle));
            pose.y = m_frontY + rel * m_stepY;
            pose.angleY = angle * 0.6f;
            pose.brightness = 1 - (1 - std::cos(angle)) * 0.22f;
            pose.opacity = std::clamp(3.6f - std::fabs(angle), 0.0f, 1.0f);
            break;
        }

        case AnimationStyle::Fan: {
            // Like playing cards held in a hand, around a pivot below.
            const float angle = rel * m_stepZ;
            const float selected = std::max(0.0f, 1 - distance);
            pose.x = m_stepX * std::sin(angle);
            pose.y = m_frontY - m_stepX * std::cos(angle) -
                     selected * m_height * 0.05f;
            pose.z = distance * m_width * 0.004f - selected * m_width * 0.02f;
            pose.angleZ = angle;
            pose.brightness = 0.8f + selected * 0.2f;
            pose.opacity = std::clamp((1.45f - std::fabs(angle)) * 3, 0.0f, 1.0f);
            break;
        }

        case AnimationStyle::Panorama: {
            // A row of windows on a wall curving around the viewer.
            const float angle = rel * m_stepZ;
            const float selected = std::max(0.0f, 1 - distance);
            pose.x = m_stepX * std::sin(angle);
            pose.y = m_frontY;
            pose.z = -m_stepX * (1 - std::cos(angle)) * 0.25f -
                     selected * m_width * 0.02f;
            pose.angleY = -angle * 0.8f;
            pose.scale = fit * (1 + selected * 0.08f);
            pose.brightness = 0.7f + selected * 0.3f;
            pose.highlight = selected;
            pose.opacity = std::clamp(4.5f - distance, 0.0f, 1.0f);
            break;
        }

        case AnimationStyle::Tunnel:
            // Windows sink into a twisting tunnel.
            pose.x = m_frontX;
            pose.y = m_frontY;
            if (rel >= 0) {
                pose.y += rel * m_stepY;
                pose.z = rel * m_stepZ;
                pose.angleZ = rel * 0.3f;
                pose.brightness = 1 - std::min(rel * 0.09f, 0.6f);
            } else {
                // Leaving the front: spin towards the viewer and fade out.
                pose.z = rel * m_focal * 0.4f;
                pose.angleZ = rel * 0.6f;
                pose.opacity = (1 + rel) * (1 + rel);
            }
            break;

        case AnimationStyle::Windows11:
        case AnimationStyle::Thumbnails: {
            // The window shrinks into its thumbnail.
            const D2D1_RECT_F& thumbnail = m_thumbnails[index];
            const float selected = std::max(0.0f, 1 - distance);
            pose.x = (thumbnail.left + thumbnail.right) / 2 - m_width / 2;
            pose.y = (thumbnail.top + thumbnail.bottom) / 2 - m_height / 2;
            pose.z = -selected * m_width * 0.002f;
            pose.scale =
                std::min((thumbnail.right - thumbnail.left) / item.Width(),
                         (thumbnail.bottom - thumbnail.top) / item.Height()) *
                (1 + selected * 0.03f);
            break;
        }

        case AnimationStyle::IconRow:
        case AnimationStyle::List:
        case AnimationStyle::Classic:
            // No thumbnails: the windows fade out where they are.
            pose = FlatPose(item);
            pose.opacity = 0;
            break;

        case AnimationStyle::Rolodex: {
            // A wheel around a horizontal axis: earlier windows roll over the
            // top, later ones under the bottom.
            const float angle = rel * m_stepZ;
            pose.x = m_frontX;
            pose.y = m_frontY + m_stepX * std::sin(angle);
            pose.z = m_stepX * (1 - std::cos(angle));
            pose.angleX = -angle;
            pose.brightness = 1 - (1 - std::cos(angle)) * 0.45f;
            // Fade out before the back side of the wheel comes around.
            pose.opacity = std::clamp((1.9f - std::fabs(angle)) * 2, 0.0f, 1.0f);
            break;
        }
    }
    return pose;
}

Quad Switcher::Project(const Pose& pose, float width, float height) const {
    const float halfWidth = width * pose.scale / 2;
    const float halfHeight = height * pose.scale / 2;
    const float cosY = std::cos(pose.angleY), sinY = std::sin(pose.angleY);
    const float cosX = std::cos(pose.angleX), sinX = std::sin(pose.angleX);
    const float cosZ = std::cos(pose.angleZ), sinZ = std::sin(pose.angleZ);
    const float corners[4][2] = {{-halfWidth, -halfHeight},
                                 {halfWidth, -halfHeight},
                                 {halfWidth, halfHeight},
                                 {-halfWidth, halfHeight}};

    Quad quad;
    for (int i = 0; i < 4; i++) {
        // Roll in the screen plane, turn around the vertical axis, then tilt
        // (positive tilts the top away from the viewer).
        const float localX = corners[i][0] * cosZ - corners[i][1] * sinZ;
        const float localY = corners[i][0] * sinZ + corners[i][1] * cosZ;
        const float turnedZ = localX * sinY;
        const float x = pose.x + localX * cosY;
        const float y = pose.y + localY * cosX + turnedZ * sinX;
        const float z = pose.z + turnedZ * cosX - localY * sinX;
        const float k = m_focal / std::max(m_focal + z, m_focal * 0.05f);
        quad.p[i] = {m_width / 2 + x * k, m_height / 2 + y * k};
    }
    return quad;
}

void Switcher::BuildDrawList(float t) {
    m_drawList.clear();
    const int count = (int)m_items.size();
    if (!count) {
        return;
    }
    const int selected = SelectedIndex();

    auto add = [&](int index, int chain, float rel, float fade) {
        Item* item = m_items[index].get();
        Pose layout = LayoutPose(*item, index, rel);
        layout.opacity *= fade;

        // Drawing order: the real Z order while flat, depth in the switcher.
        float flatOrder = (float)index;
        if (m_selectedOnTopWhenFlat && index == selected) {
            flatOrder = -1;
        }
        const float depthOrder = layout.z / (m_width * 0.05f);

        DrawEntry entry;
        entry.item = item;
        entry.chain = chain;
        entry.pose = LerpPose(FlatPose(*item), layout, t);
        entry.order = Lerp(flatOrder, depthOrder, t);
        entry.quad = Project(entry.pose, item->Width(), item->Height());
        if (entry.pose.opacity > 0.003f) {
            m_drawList.push_back(entry);
        }
    };

    const bool stacked = m_settings.style == AnimationStyle::Flip3D ||
                         m_settings.style == AnimationStyle::Cascade ||
                         m_settings.style == AnimationStyle::Tunnel;
    for (int i = 0; i < count; i++) {
        if (count == 1) {
            add(i, 0, 0, 1);
            continue;
        }
        const float slot = PositiveMod((float)(i - m_scroll), (float)count);
        if (!stacked) {
            // Signed distance from the selected window, wrapping around.
            add(i, 0, slot >= count / 2.0f ? slot - count : slot, 1);
        } else if (slot <= count - 1) {
            add(i, 0, slot, 1);
        } else {
            // Wrapping from the front to the back of the stack.
            const float leaving = slot - count;  // In (-1, 0).
            add(i, 0, leaving, 1);
            add(i, 1, slot, -leaving);
        }
    }

    std::stable_sort(m_drawList.begin(), m_drawList.end(),
                     [](const DrawEntry& a, const DrawEntry& b) {
                         return a.order > b.order;
                     });
}

// Lays out the 2D panel styles: every window gets a cell, which may hold a
// thumbnail, an icon and a title, and the cells flow in rows (or, for the
// list, in columns) inside a panel.
void Switcher::ComputePanelLayout() {
    const int count = (int)m_items.size();
    m_cells.assign(count, {});
    m_thumbnails.assign(count, {});
    m_iconRects.assign(count, {});
    m_labelRects.assign(count, {});
    m_caption = {};
    m_labelFormat = nullptr;
    for (auto& item : m_items) {
        item->label = nullptr;
    }
    if (!count) {
        return;
    }

    const AnimationStyle style = m_settings.style;
    const bool vertical = style == AnimationStyle::List;
    const float spacing = m_settings.stackSpacing;
    // Follows the monitor's scaling, like the system's own text.
    const float font = std::round(14 * m_dpiScale);
    m_labelFontSize = font;

    // Cell geometry relative to the cell's top-left corner, for one window.
    struct CellLayout {
        D2D1_SIZE_F size;
        D2D1_RECT_F thumbnail, icon, label;
    };
    auto cellFor = [&](int index, float scale) {
        const Item& item = *m_items[index];
        CellLayout cell{};
        switch (style) {
            case AnimationStyle::Windows11:
            case AnimationStyle::Thumbnails: {
                const float height = std::round(m_height * 0.17f * scale);
                const float width = std::round(
                    std::clamp(height * item.Width() / item.Height(),
                               height * 0.75f, height * 1.9f));
                const float pad = std::round(height * 0.07f);
                const float label = std::round(font * 2.1f);
                const float icon = std::round(font * 1.35f);
                cell.size = {width + 2 * pad, label + height + 2 * pad};
                const float thumbnailTop =
                    style == AnimationStyle::Windows11 ? pad + label : pad;
                const float labelTop =
                    style == AnimationStyle::Windows11 ? pad : pad + height;
                cell.thumbnail = {pad, thumbnailTop, pad + width,
                                  thumbnailTop + height};
                const float iconTop = std::round(labelTop + (label - icon) / 2);
                cell.icon = {pad, iconTop, pad + icon, iconTop + icon};
                cell.label = {pad + icon + std::round(font * 0.55f), labelTop,
                              pad + width, labelTop + label};
                break;
            }

            case AnimationStyle::IconRow: {
                const float icon = std::round(font * 3 * scale);
                const float width = std::round(font * 11 * scale);
                const float pad = std::round(font * 0.9f * scale);
                const float label = std::round(font * 2.1f);
                const float iconLeft = std::round((width - icon) / 2);
                cell.size = {width, pad + icon + font * 0.5f + label + pad};
                cell.icon = {iconLeft, pad, iconLeft + icon, pad + icon};
                cell.label = {pad, pad + icon + font * 0.5f, width - pad,
                              pad + icon + font * 0.5f + label};
                break;
            }

            case AnimationStyle::List: {
                const float height = std::round(font * 2.7f * scale);
                const float width =
                    std::round(std::min(m_width * 0.34f, font * 30));
                const float icon = std::round(font * 1.45f);
                const float pad = std::round(font * 0.8f);
                const float iconTop = std::round((height - icon) / 2);
                cell.size = {width, height};
                cell.icon = {pad, iconTop, pad + icon, iconTop + icon};
                cell.label = {pad + icon + std::round(font * 0.7f), 0,
                              width - pad, height};
                break;
            }

            case AnimationStyle::Classic:
            default: {
                const float size = std::round(font * 3.6f * scale);
                const float icon = std::round(font * 2.4f * scale);
                const float offset = std::round((size - icon) / 2);
                cell.size = {size, size};
                cell.icon = {offset, offset, offset + icon, offset + icon};
                break;
            }
        }
        return cell;
    };

    float gap;
    switch (style) {
        case AnimationStyle::Windows11:
        case AnimationStyle::Thumbnails:
            gap = std::round(m_height * 0.01f * spacing);
            break;
        case AnimationStyle::IconRow:
            gap = std::round(font * 0.4f * spacing);
            break;
        case AnimationStyle::List:
            gap = std::round(font * 0.25f * spacing);
            break;
        default:
            gap = std::round(font * 0.15f * spacing);
            break;
    }
    // The classic switcher shows at most 7 icons per row.
    const int maxPerLine = style == AnimationStyle::Classic ? 7 : count;
    const float maxLine = vertical ? m_height * 0.78f : m_width * 0.86f;
    const float maxAcross = vertical ? m_width * 0.86f : m_height * 0.78f;

    // Flow the cells into lines, shrinking them until everything fits.
    std::vector<CellLayout> cells(count);
    std::vector<int> lineOf(count);
    std::vector<float> lineLengths, lineThickness;
    float scale = 1;
    for (int attempt = 0; attempt < 10; attempt++) {
        lineLengths.clear();
        lineThickness.clear();
        int inLine = 0;
        for (int i = 0; i < count; i++) {
            cells[i] = cellFor(i, scale);
            const float along = vertical ? cells[i].size.height
                                         : cells[i].size.width;
            const float across = vertical ? cells[i].size.width
                                          : cells[i].size.height;
            if (lineLengths.empty() || inLine >= maxPerLine ||
                lineLengths.back() + gap + along > maxLine) {
                lineLengths.push_back(along);
                lineThickness.push_back(across);
                inLine = 1;
            } else {
                lineLengths.back() += gap + along;
                lineThickness.back() = std::max(lineThickness.back(), across);
                inLine++;
            }
            lineOf[i] = (int)lineLengths.size() - 1;
        }
        float total = gap * (lineThickness.size() - 1);
        for (float thickness : lineThickness) {
            total += thickness;
        }
        if (total <= maxAcross) {
            break;
        }
        scale *= 0.85f;
    }

    // Classic and list lines start at the same edge; the others are centered.
    const bool centerLines = style != AnimationStyle::Classic &&
                             style != AnimationStyle::List;
    float totalAcross = gap * (lineThickness.size() - 1);
    for (float thickness : lineThickness) {
        totalAcross += thickness;
    }
    const float longest =
        *std::max_element(lineLengths.begin(), lineLengths.end());

    float lineStart = 0, along = 0;
    float across = std::round(
        ((vertical ? m_width : m_height) - totalAcross) / 2);
    for (int i = 0; i < count; i++) {
        const int line = lineOf[i];
        if (i > 0 && lineOf[i - 1] != line) {
            across += lineThickness[line - 1] + gap;
        }
        if (i == 0 || lineOf[i - 1] != line) {
            const float length = centerLines ? lineLengths[line] : longest;
            lineStart = std::round(
                ((vertical ? m_height : m_width) - length) / 2);
            along = lineStart;
        }

        const CellLayout& cell = cells[i];
        const float x = vertical ? across : along;
        const float y = vertical ? along : across;
        auto offset = [x, y](const D2D1_RECT_F& r) {
            return D2D1_RECT_F{x + r.left, y + r.top, x + r.right,
                               y + r.bottom};
        };
        m_cells[i] = {x, y, x + cell.size.width, y + cell.size.height};
        m_thumbnails[i] = offset(cell.thumbnail);
        m_iconRects[i] = offset(cell.icon);
        m_labelRects[i] = offset(cell.label);
        along += (vertical ? cell.size.height : cell.size.width) + gap;
    }

    m_panel = m_cells[0];
    for (const D2D1_RECT_F& cell : m_cells) {
        m_panel.left = std::min(m_panel.left, cell.left);
        m_panel.top = std::min(m_panel.top, cell.top);
        m_panel.right = std::max(m_panel.right, cell.right);
        m_panel.bottom = std::max(m_panel.bottom, cell.bottom);
    }
    const float margin = std::round(font * (style == AnimationStyle::Classic
                                                ? 0.9f
                                                : 1.1f));
    m_panel = {m_panel.left - margin, m_panel.top - margin,
               m_panel.right + margin, m_panel.bottom + margin};

    if (style == AnimationStyle::Classic) {
        // The title of the selected window goes in a box under the icons.
        const float height = std::round(font * 2.2f);
        m_caption = {m_panel.left + margin, m_panel.bottom,
                     m_panel.right - margin, m_panel.bottom + height};
        m_panel.bottom = m_caption.bottom + margin;
    }
}

// The selection slides between neighboring cells. When it wraps around from
// the last window to the first, it fades instead of crossing the panel.
D2D1_RECT_F Switcher::SelectionCell(float* opacity) const {
    const int count = (int)m_cells.size();
    const double base = std::floor(m_scroll);
    const float fraction = (float)(m_scroll - base);
    const int from = PositiveMod((long long)base, count);
    const int to = PositiveMod((long long)base + 1, count);

    if (to == 0 && count > 2) {
        *opacity = std::fabs(fraction - 0.5f) * 2;
        return m_cells[fraction < 0.5f ? from : to];
    }

    *opacity = 1;
    const D2D1_RECT_F& a = m_cells[from];
    const D2D1_RECT_F& b = m_cells[to];
    return {Lerp(a.left, b.left, fraction), Lerp(a.top, b.top, fraction),
            Lerp(a.right, b.right, fraction),
            Lerp(a.bottom, b.bottom, fraction)};
}

D2D1_COLOR_F GetAccentColor(float alpha) {
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (FAILED(DwmGetColorizationColor(&color, &opaque))) {
        return D2D1::ColorF(0.30f, 0.76f, 1.0f, alpha);
    }
    return D2D1::ColorF(((color >> 16) & 0xff) / 255.0f,
                        ((color >> 8) & 0xff) / 255.0f, (color & 0xff) / 255.0f,
                        alpha);
}

// Panel and selection background, drawn under the thumbnails.
void Switcher::DrawPanelBack(float t) {
    if (m_cells.empty()) {
        return;
    }

    if (m_settings.style == AnimationStyle::Classic) {
        // Windows XP: a beige panel with a blue frame.
        m_brush->SetColor(D2D1::ColorF(0.925f, 0.914f, 0.847f, t));
        m_ctx->FillRectangle(m_panel, m_brush.get());
        m_brush->SetColor(D2D1::ColorF(0.0f, 0.24f, 0.65f, t));
        m_ctx->DrawRectangle(m_panel, m_brush.get(), 2);
        return;
    }

    const float radius = m_settings.style == AnimationStyle::Thumbnails
                             ? 0.0f
                             : std::round(m_height / 135);
    m_brush->SetColor(D2D1::ColorF(0.11f, 0.11f, 0.12f, 0.82f * t));
    m_ctx->FillRoundedRectangle({m_panel, radius, radius}, m_brush.get());
    m_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.08f * t));
    m_ctx->DrawRoundedRectangle({m_panel, radius, radius}, m_brush.get(), 1);

    float opacity;
    const D2D1_RECT_F cell = SelectionCell(&opacity);
    m_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.07f * t * opacity));
    m_ctx->FillRoundedRectangle({cell, radius, radius}, m_brush.get());
}

bool Switcher::EnsureLabelFormat() {
    if (m_labelFormat) {
        return true;
    }
    const bool classic = m_settings.style == AnimationStyle::Classic;
    if (FAILED(m_dwriteFactory->CreateTextFormat(
            classic ? L"Tahoma" : L"Segoe UI Variable Text", nullptr,
            classic ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            m_labelFontSize, L"", m_labelFormat.put()))) {
        return false;
    }
    m_labelFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    m_labelFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0,
                                   0};
    com_ptr<IDWriteInlineObject> ellipsis;
    m_dwriteFactory->CreateEllipsisTrimmingSign(m_labelFormat.get(),
                                                ellipsis.put());
    m_labelFormat->SetTrimming(&trimming, ellipsis.get());
    return true;
}

// Draws a window title in a rectangle, vertically centered.
void Switcher::DrawLabel(Item& item,
                         const D2D1_RECT_F& rect,
                         bool centered,
                         D2D1_COLOR_F color) {
    const float width = rect.right - rect.left;
    const float height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0 || !EnsureLabelFormat()) {
        return;
    }
    if (!item.label || item.labelWidth != width) {
        item.label = nullptr;
        item.labelWidth = width;
        if (FAILED(m_dwriteFactory->CreateTextLayout(
                item.title.c_str(), (UINT32)item.title.size(),
                m_labelFormat.get(), width, height, item.label.put()))) {
            return;
        }
    }
    item.label->SetTextAlignment(centered ? DWRITE_TEXT_ALIGNMENT_CENTER
                                          : DWRITE_TEXT_ALIGNMENT_LEADING);
    item.label->SetMaxHeight(height);
    m_brush->SetColor(color);
    m_ctx->DrawTextLayout({rect.left, rect.top}, item.label.get(),
                          m_brush.get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

// Icons, titles and the selection border, drawn over the thumbnails.
void Switcher::DrawPanelFront(float t) {
    if (m_cells.empty()) {
        return;
    }
    const AnimationStyle style = m_settings.style;
    const bool classic = style == AnimationStyle::Classic;

    float opacity;
    const D2D1_RECT_F cell = SelectionCell(&opacity);
    if (classic) {
        m_brush->SetColor(D2D1::ColorF(0.19f, 0.42f, 0.77f, 0.25f * t * opacity));
        m_ctx->FillRectangle(cell, m_brush.get());
        m_brush->SetColor(D2D1::ColorF(0.19f, 0.42f, 0.77f, t * opacity));
        m_ctx->DrawRectangle(cell, m_brush.get(), 2);
    } else {
        const float radius = style == AnimationStyle::Thumbnails
                                 ? 0.0f
                                 : std::round(m_height / 135);
        const float border = std::max(2.0f, std::round(m_height / 360));
        const D2D1_RECT_F rect{cell.left + border / 2, cell.top + border / 2,
                               cell.right - border / 2,
                               cell.bottom - border / 2};
        m_brush->SetColor(GetAccentColor(t * opacity));
        m_ctx->DrawRoundedRectangle({rect, radius, radius}, m_brush.get(),
                                    border);
    }

    const float labelOpacity = t * t;
    for (size_t i = 0; i < m_items.size(); i++) {
        Item& item = *m_items[i];
        const D2D1_RECT_F& icon = m_iconRects[i];
        if (item.icon && icon.right > icon.left) {
            m_ctx->DrawBitmap(item.icon.get(), icon, t,
                              D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
        }
        if (!classic && labelOpacity > 0.01f) {
            DrawLabel(item, m_labelRects[i], style == AnimationStyle::IconRow,
                      D2D1::ColorF(1, 1, 1, 0.92f * labelOpacity));
        }
    }

    if (classic && labelOpacity > 0.01f) {
        // Sunken box with the title of the window under the selection.
        m_brush->SetColor(D2D1::ColorF(0.98f, 0.98f, 0.96f, t));
        m_ctx->FillRectangle(m_caption, m_brush.get());
        m_brush->SetColor(D2D1::ColorF(0.5f, 0.5f, 0.5f, t));
        m_ctx->DrawRectangle(m_caption, m_brush.get(), 1);

        const int index = PositiveMod((long long)std::round(m_scroll),
                                      (int)m_items.size());
        const float pad = std::round(m_labelFontSize * 0.5f);
        DrawLabel(*m_items[index],
                  {m_caption.left + pad, m_caption.top, m_caption.right - pad,
                   m_caption.bottom},
                  false, D2D1::ColorF(0, 0, 0, labelOpacity));
    }
}

bool Switcher::EnsureChain(EffectChain& chain) {
    if (chain.transform) {
        return true;
    }
    if (FAILED(m_ctx->CreateEffect(CLSID_D2D13DTransform,
                                   chain.transform.put())) ||
        FAILED(m_ctx->CreateEffect(CLSID_D2D1ColorMatrix,
                                   chain.color.put())) ||
        FAILED(m_ctx->CreateEffect(CLSID_D2D1Shadow, chain.shadow.put()))) {
        chain = {};
        return false;
    }

    chain.transform->SetValue(kTransform3DPropInterpolationMode,
                              kTransform3DInterpolationAnisotropic);
    chain.transform->SetValue(kTransform3DPropBorderMode,
                              (UINT32)D2D1_BORDER_MODE_SOFT);
    chain.color->SetInputEffect(0, chain.transform.get());
    chain.shadow->SetInputEffect(0, chain.color.get());
    chain.shadow->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION,
                           kShadowOptimizationSpeed);
    return true;
}

void Switcher::DrawCard(const DrawEntry& entry, float t) {
    Item& item = *entry.item;
    EffectChain& chain = item.chains[entry.chain];
    if (!EnsureChain(chain)) {
        return;
    }

    ID2D1Bitmap1* bitmap =
        item.content ? item.content.get() : item.placeholder.get();
    if (chain.input != bitmap) {
        chain.transform->SetInput(0, bitmap);
        chain.input = bitmap;
    }

    const D2D1_SIZE_F size = bitmap->GetSize();
    chain.transform->SetValue(
        kTransform3DPropTransformMatrix,
        RectToQuadMatrix(size.width, size.height, entry.quad));

    const float b = entry.pose.brightness;
    const D2D1_MATRIX_5X4_F colorMatrix =
        D2D1::Matrix5x4F(b, 0, 0, 0,                   //
                         0, b, 0, 0,                   //
                         0, 0, b, 0,                   //
                         0, 0, 0, entry.pose.opacity,  //
                         0, 0, 0, 0);
    chain.color->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, colorMatrix);

    if (m_settings.shadows && t > 0.01f) {
        const float depth =
            m_focal / std::max(m_focal + entry.pose.z, m_focal * 0.05f);
        chain.shadow->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION,
                               14.0f * depth);
        chain.shadow->SetValue(D2D1_SHADOW_PROP_COLOR,
                               D2D1::Vector4F(0, 0, 0, 0.55f * t));
        const D2D1_POINT_2F offset{0, 10.0f * depth * t};
        m_ctx->DrawImage(chain.shadow.get(), &offset);
    }
    m_ctx->DrawImage(chain.color.get());

    // Selection outline.
    const float highlight = entry.pose.highlight * entry.pose.opacity * t;
    if (highlight > 0.01f) {
        com_ptr<ID2D1PathGeometry> outline;
        com_ptr<ID2D1GeometrySink> sink;
        if (SUCCEEDED(m_d2dFactory->CreatePathGeometry(outline.put())) &&
            SUCCEEDED(outline->Open(sink.put()))) {
            sink->BeginFigure(entry.quad.p[0], D2D1_FIGURE_BEGIN_HOLLOW);
            sink->AddLines(&entry.quad.p[1], 3);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            if (SUCCEEDED(sink->Close())) {
                m_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.9f * highlight));
                m_ctx->DrawGeometry(outline.get(), m_brush.get(),
                                    std::max(2.0f, m_height / 360));
            }
        }
    }
}

void Switcher::DrawTitle(float t) {
    if (!m_settings.showTitle || m_items.empty()) {
        return;
    }

    // Visible while settled, faded out while flipping.
    const double nearest = std::round(m_scroll);
    const float settle =
        1 - std::clamp((float)std::fabs(m_scroll - nearest) * 3, 0.0f, 1.0f);
    const float opacity = t * t * settle;
    if (opacity < 0.01f) {
        return;
    }

    const int index = PositiveMod((long long)nearest, (int)m_items.size());
    const Item& item = *m_items[index];

    const float fontSize = std::round(21 * m_dpiScale);
    if (!m_titleFormat) {
        if (FAILED(m_dwriteFactory->CreateTextFormat(
                L"Segoe UI Variable Display", nullptr,
                DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, fontSize, L"",
                m_titleFormat.put()))) {
            return;
        }
        m_titleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER,
                                       0, 0};
        com_ptr<IDWriteInlineObject> ellipsis;
        m_dwriteFactory->CreateEllipsisTrimmingSign(m_titleFormat.get(),
                                                    ellipsis.put());
        m_titleFormat->SetTrimming(&trimming, ellipsis.get());
        m_titleLayout = nullptr;
    }

    if (index != m_titleIndex || !m_titleLayout) {
        m_titleLayout = nullptr;
        m_titleIndex = index;
        if (FAILED(m_dwriteFactory->CreateTextLayout(
                item.title.c_str(), (UINT32)item.title.size(),
                m_titleFormat.get(), m_width * 0.6f, fontSize * 2,
                m_titleLayout.put()))) {
            return;
        }
    }

    DWRITE_TEXT_METRICS metrics;
    m_titleLayout->GetMetrics(&metrics);
    const float iconSize = item.icon ? std::round(fontSize * 1.6f) : 0;
    const float gap = item.icon ? std::round(fontSize * 0.6f) : 0;
    const float left =
        std::round((m_width - (iconSize + gap + metrics.width)) / 2);
    const float centerY = std::round(m_height * 0.91f);

    if (item.icon) {
        const D2D1_RECT_F iconRect{left, centerY - iconSize / 2,
                                   left + iconSize, centerY + iconSize / 2};
        m_ctx->DrawBitmap(item.icon.get(), iconRect, opacity,
                          D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
    }

    const D2D1_POINT_2F origin{left + iconSize + gap - metrics.left,
                               centerY - metrics.height / 2 - metrics.top};
    m_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.55f * opacity));
    m_ctx->DrawTextLayout({origin.x, origin.y + 2}, m_titleLayout.get(),
                          m_brush.get());
    m_brush->SetColor(D2D1::ColorF(1, 1, 1, opacity));
    m_ctx->DrawTextLayout(origin, m_titleLayout.get(), m_brush.get());
}

void Switcher::RenderFrame() {
    const double now = NowSeconds();
    const double dt = std::clamp(now - m_lastFrame, 0.0, 0.05);
    m_lastFrame = now;

    if (m_state == State::Open && AltReleased(now) && EndSwitching()) {
        BeginClose(true);
    }

    // Critically damped spring towards the target.
    const double omega = 15.0 * m_settings.flipSpeed;
    constexpr double kStep = 1.0 / 480;
    for (double remaining = dt; remaining > 0; remaining -= kStep) {
        const double h = std::min(remaining, kStep);
        const double acceleration =
            omega * omega * (m_target - m_scroll) - 2 * omega * m_velocity;
        m_velocity += acceleration * h;
        m_scroll += m_velocity * h;
    }
    if (std::fabs(m_target - m_scroll) < 1e-4 &&
        std::fabs(m_velocity) < 1e-3) {
        m_scroll = (double)m_target;
        m_velocity = 0;
    }
    // Keep the numbers small; only their value modulo the count matters.
    if (const int count = (int)m_items.size();
        count && std::llabs(m_target) > count * 1000LL) {
        const long long shift = m_target - PositiveMod(m_target, count);
        m_target -= shift;
        m_scroll -= (double)shift;
    }

    const float t = m_open.Value(now);

    for (auto& item : m_items) {
        PollFrames(*item);
    }
    BuildDrawList(t);

    m_ctx->SetTarget(m_targetBitmap.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    const D2D1_RECT_F screen{0, 0, m_width, m_height};
    if (m_background) {
        m_ctx->DrawBitmap(m_background.get(), screen, t,
                          D2D1_INTERPOLATION_MODE_LINEAR);
    }
    if (m_settings.dimOpacity > 0) {
        m_brush->SetColor(D2D1::ColorF(0, 0, 0, m_settings.dimOpacity * t));
        m_ctx->FillRectangle(screen, m_brush.get());
    }

    const bool panel = IsPanelStyle(m_settings.style);
    if (panel) {
        DrawPanelBack(t);
    }
    for (const DrawEntry& entry : m_drawList) {
        DrawCard(entry, t);
    }
    if (panel) {
        DrawPanelFront(t);
    } else {
        DrawTitle(t);
    }

    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    if (SUCCEEDED(hr)) {
        hr = m_swapChain->Present(1, 0);
    }
    if (FAILED(hr)) {
        Wh_Log(L"Rendering failed: 0x%08X", (UINT)hr);
        HandleDeviceLost();
        return;
    }

    if (m_state == State::Closing && m_open.Done(now)) {
        FinishClose();
    }
}

com_ptr<ID2D1Bitmap1> Switcher::GetIconBitmap(HWND hwnd) {
    if (HICON icon = GetWindowIcon(hwnd)) {
        if (auto bitmap = CreateIconBitmap(icon)) {
            return bitmap;
        }
    }
    return CreateAppIconBitmap(hwnd);
}

// Packaged (UWP) apps, hosted in an ApplicationFrameWindow, usually have no
// window icon. Their icon comes from the shell, by the app's AppUserModelID.
com_ptr<ID2D1Bitmap1> Switcher::CreateAppIconBitmap(HWND hwnd) {
    com_ptr<IPropertyStore> store;
    if (FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(store.put())))) {
        return nullptr;
    }
    std::wstring appId;
    PROPVARIANT value;
    PropVariantInit(&value);
    if (SUCCEEDED(store->GetValue(kPKEY_AppUserModel_ID, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        appId = value.pwszVal;
    }
    PropVariantClear(&value);
    if (appId.empty()) {
        return nullptr;
    }

    com_ptr<IShellItemImageFactory> imageFactory;
    if (FAILED(SHCreateItemInKnownFolder(kFOLDERID_AppsFolder, 0,
                                         appId.c_str(),
                                         IID_PPV_ARGS(imageFactory.put())))) {
        return nullptr;
    }
    const int size = (int)std::round(64 * m_dpiScale);
    HBITMAP hbitmap = nullptr;
    if (FAILED(imageFactory->GetImage({size, size}, SIIGBF_ICONONLY,
                                      &hbitmap))) {
        return nullptr;
    }

    com_ptr<IWICBitmap> wicBitmap;
    com_ptr<IWICFormatConverter> converter;
    com_ptr<ID2D1Bitmap1> bitmap;
    const bool ok =
        SUCCEEDED(m_wicFactory->CreateBitmapFromHBITMAP(
            hbitmap, nullptr, WICBitmapUsePremultipliedAlpha,
            wicBitmap.put())) &&
        SUCCEEDED(m_wicFactory->CreateFormatConverter(converter.put())) &&
        SUCCEEDED(converter->Initialize(
            wicBitmap.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut)) &&
        SUCCEEDED(m_ctx->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                   bitmap.put()));
    DeleteObject(hbitmap);
    return ok ? bitmap : nullptr;
}

com_ptr<ID2D1Bitmap1> Switcher::CreateIconBitmap(HICON icon) {
    com_ptr<IWICBitmap> wicBitmap;
    com_ptr<IWICFormatConverter> converter;
    com_ptr<ID2D1Bitmap1> bitmap;
    if (FAILED(m_wicFactory->CreateBitmapFromHICON(icon, wicBitmap.put())) ||
        FAILED(m_wicFactory->CreateFormatConverter(converter.put())) ||
        FAILED(converter->Initialize(wicBitmap.get(),
                                     GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeMedianCut)) ||
        FAILED(m_ctx->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                bitmap.put()))) {
        return nullptr;
    }
    return bitmap;
}

com_ptr<ID2D1Bitmap1> Switcher::CreateTargetBitmap(UINT width, UINT height) {
    com_ptr<ID2D1Bitmap1> bitmap;
    const D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(m_ctx->CreateBitmap({width, height}, nullptr, 0, &props,
                                   bitmap.put()))) {
        return nullptr;
    }
    return bitmap;
}

// Card shown until the first frame arrives, and for minimized windows.
com_ptr<ID2D1Bitmap1> Switcher::CreatePlaceholder(const Item& item) {
    const float scale =
        std::min(1.0f, 640 / std::max(item.Width(), item.Height()));
    const UINT width = (UINT)std::max(std::lround(item.Width() * scale), 1L);
    const UINT height = (UINT)std::max(std::lround(item.Height() * scale), 1L);

    com_ptr<ID2D1Bitmap1> bitmap = CreateTargetBitmap(width, height);
    if (!bitmap) {
        return nullptr;
    }

    m_ctx->SetTarget(bitmap.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    const D2D1_ROUNDED_RECT card{{0.5f, 0.5f, width - 0.5f, height - 0.5f},
                                 8,
                                 8};
    m_brush->SetColor(D2D1::ColorF(0.13f, 0.13f, 0.15f, 0.96f));
    m_ctx->FillRoundedRectangle(card, m_brush.get());
    m_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.14f));
    m_ctx->DrawRoundedRectangle(card, m_brush.get(), 1);

    if (item.icon) {
        const float iconSize =
            std::round(std::min({width * 0.3f, height * 0.3f, 96.0f}));
        const float x = std::round((width - iconSize) / 2);
        const float y = std::round((height - iconSize) / 2);
        m_ctx->DrawBitmap(item.icon.get(), {x, y, x + iconSize, y + iconSize},
                          1, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
    }

    const HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    return SUCCEEDED(hr) ? bitmap : nullptr;
}

void Switcher::UpdateBackground() {
    if (m_settings.background == BackgroundMode::Dim) {
        m_background = nullptr;
        m_backgroundKey.clear();
        return;
    }

    const UINT width = m_swapWidth;
    const UINT height = m_swapHeight;

    std::wstring path;
    COLORREF color = 0;
    DESKTOP_WALLPAPER_POSITION position = DWPOS_FILL;

    com_ptr<IDesktopWallpaper> wallpaper;
    if (SUCCEEDED(CoCreateInstance(kCLSID_DesktopWallpaper, nullptr,
                                   CLSCTX_ALL,
                                   IID_PPV_ARGS(wallpaper.put())))) {
        wallpaper->GetBackgroundColor(&color);
        wallpaper->GetPosition(&position);

        // Use the wallpaper of the monitor closest to ours.
        UINT count = 0;
        wallpaper->GetMonitorDevicePathCount(&count);
        const LONG centerX = (m_monitor.left + m_monitor.right) / 2;
        const LONG centerY = (m_monitor.top + m_monitor.bottom) / 2;
        long long bestDistance = LLONG_MAX;
        for (UINT i = 0; i < count; i++) {
            LPWSTR monitorId = nullptr;
            if (FAILED(wallpaper->GetMonitorDevicePathAt(i, &monitorId))) {
                continue;
            }
            RECT rect;
            LPWSTR monitorPath = nullptr;
            if (SUCCEEDED(wallpaper->GetMonitorRECT(monitorId, &rect)) &&
                SUCCEEDED(wallpaper->GetWallpaper(monitorId, &monitorPath))) {
                const long long dx = (rect.left + rect.right) / 2 - centerX;
                const long long dy = (rect.top + rect.bottom) / 2 - centerY;
                if (dx * dx + dy * dy < bestDistance) {
                    bestDistance = dx * dx + dy * dy;
                    path = monitorPath ? monitorPath : L"";
                }
            }
            CoTaskMemFree(monitorPath);
            CoTaskMemFree(monitorId);
        }
    }

    // The wallpaper file is often replaced in place, so include its time.
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if (!path.empty() && !GetFileAttributesExW(
                             path.c_str(), GetFileExInfoStandard, &attributes)) {
        path.clear();
    }
    const ULONGLONG fileTime =
        ((ULONGLONG)attributes.ftLastWriteTime.dwHighDateTime << 32) |
        attributes.ftLastWriteTime.dwLowDateTime;
    const std::wstring key =
        path + L"|" + std::to_wstring(fileTime) + L"|" +
        std::to_wstring(width) + L"x" + std::to_wstring(height) + L"|" +
        std::to_wstring(color) + L"|" + std::to_wstring((int)position) + L"|" +
        std::to_wstring((int)m_settings.background) + L"|" +
        std::to_wstring(m_settings.blurAmount);
    if (m_background && key == m_backgroundKey) {
        return;
    }
    m_background = nullptr;
    m_backgroundKey = key;

    // Decode the image, scaled to the size it's displayed at.
    com_ptr<ID2D1Bitmap1> image;
    D2D1_RECT_F imageRect{};
    com_ptr<IWICBitmapDecoder> decoder;
    com_ptr<IWICBitmapFrameDecode> frame;
    UINT imageWidth = 0, imageHeight = 0;
    if (!path.empty() &&
        SUCCEEDED(m_wicFactory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnDemand, decoder.put())) &&
        SUCCEEDED(decoder->GetFrame(0, frame.put())) &&
        SUCCEEDED(frame->GetSize(&imageWidth, &imageHeight)) && imageWidth &&
        imageHeight) {
        float drawWidth = (float)width;
        float drawHeight = (float)height;
        const float scaleX = width / (float)imageWidth;
        const float scaleY = height / (float)imageHeight;
        switch (position) {
            case DWPOS_FIT:
                drawWidth = imageWidth * std::min(scaleX, scaleY);
                drawHeight = imageHeight * std::min(scaleX, scaleY);
                break;
            case DWPOS_STRETCH:
                break;
            case DWPOS_CENTER:
                drawWidth = (float)imageWidth;
                drawHeight = (float)imageHeight;
                break;
            default:  // Fill, span and tile.
                drawWidth = imageWidth * std::max(scaleX, scaleY);
                drawHeight = imageHeight * std::max(scaleX, scaleY);
                break;
        }
        imageRect = {(width - drawWidth) / 2, (height - drawHeight) / 2,
                     (width + drawWidth) / 2, (height + drawHeight) / 2};

        com_ptr<IWICBitmapScaler> scaler;
        com_ptr<IWICFormatConverter> converter;
        const UINT scaledWidth = (UINT)std::max(std::lround(drawWidth), 1L);
        const UINT scaledHeight = (UINT)std::max(std::lround(drawHeight), 1L);
        if (SUCCEEDED(m_wicFactory->CreateBitmapScaler(scaler.put())) &&
            SUCCEEDED(scaler->Initialize(frame.get(), scaledWidth,
                                         scaledHeight,
                                         WICBitmapInterpolationModeFant)) &&
            SUCCEEDED(m_wicFactory->CreateFormatConverter(converter.put())) &&
            SUCCEEDED(converter->Initialize(
                scaler.get(), GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone, nullptr, 0,
                WICBitmapPaletteTypeMedianCut))) {
            m_ctx->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                             image.put());
        }
    }

    com_ptr<ID2D1Bitmap1> composed = CreateTargetBitmap(width, height);
    if (!composed) {
        return;
    }
    m_ctx->SetTarget(composed.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(GetRValue(color) / 255.0f,
                              GetGValue(color) / 255.0f,
                              GetBValue(color) / 255.0f, 1));
    if (image) {
        m_ctx->DrawBitmap(image.get(), imageRect, 1,
                          D2D1_INTERPOLATION_MODE_LINEAR);
    }
    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    if (FAILED(hr)) {
        return;
    }

    if (m_settings.background != BackgroundMode::BlurredWallpaper) {
        m_background = composed;
        return;
    }

    com_ptr<ID2D1Effect> blur;
    com_ptr<ID2D1Bitmap1> blurred = CreateTargetBitmap(width, height);
    if (!blurred ||
        FAILED(m_ctx->CreateEffect(CLSID_D2D1GaussianBlur, blur.put()))) {
        m_background = composed;
        return;
    }
    blur->SetInput(0, composed.get());
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
                   m_settings.blurAmount);
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
                   (UINT32)D2D1_BORDER_MODE_HARD);
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION,
                   kGaussianBlurOptimizationQuality);

    m_ctx->SetTarget(blurred.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 1));
    m_ctx->DrawImage(blur.get());
    hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    m_background = SUCCEEDED(hr) ? blurred : composed;
}

DWORD WINAPI UiThreadProc(LPVOID parameter) {
    // Work in physical pixels everywhere.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"init_apartment failed: 0x%08X", (UINT)e.code());
        SetEvent((HANDLE)parameter);
        return 1;
    }

    {
        Switcher switcher;
        g_switcher = &switcher;
        const bool created = switcher.CreateOverlay();
        // The creator only waits for the window.
        SetEvent((HANDLE)parameter);
        if (created) {
            switcher.Activate();
            switcher.Run();
        }
        switcher.Shutdown();
        g_switcher = nullptr;
    }

    winrt::uninit_apartment();
    return 0;
}

bool StartThread(LPTHREAD_START_ROUTINE proc, HANDLE* thread, DWORD* threadId) {
    HANDLE started = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!started) {
        return false;
    }
    *thread = CreateThread(nullptr, 0, proc, started, 0, threadId);
    if (*thread) {
        WaitForSingleObject(started, INFINITE);
    }
    CloseHandle(started);
    return *thread != nullptr;
}

void StopThread(HANDLE* thread, DWORD threadId) {
    if (!*thread) {
        return;
    }
    PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    WaitForSingleObject(*thread, INFINITE);
    CloseHandle(*thread);
    *thread = nullptr;
}

void InitModuleGlobals() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&InitModuleGlobals),
                       &g_module);
    g_hotkeyMessage = RegisterWindowMessageW(kHotkeyMessageName);
}

bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    const DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) {
        return false;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// The switcher runs in a dedicated windhawk.exe process (see the tool mod
// implementation below), not in explorer.

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    InitModuleGlobals();

    if (!StartThread(UiThreadProc, &g_uiThread, &g_uiThreadId) ||
        !g_overlayWnd) {
        Wh_Log(L"Failed to start the UI thread");
        StopThread(&g_uiThread, g_uiThreadId);
        return FALSE;
    }

    if (!StartThread(HookThreadProc, &g_hookThread, &g_hookThreadId)) {
        Wh_Log(L"Failed to start the hook thread");
        StopThread(&g_uiThread, g_uiThreadId);
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    g_ready = false;
    StopThread(&g_hookThread, g_hookThreadId);
    g_switching = false;
    StopThread(&g_uiThread, g_uiThreadId);
    g_overlayWnd = nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_overlayWnd) {
        PostMessageW(g_overlayWnd, WM_APP_SETTINGS, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// The part loaded in explorer.exe only hands the Alt+Tab hotkey over (see
// StartExplorerPart).

bool g_isExplorer;

BOOL ExplorerModInit() {
    Wh_Log(L">");

    InitModuleGlobals();
    StartExplorerPart();
    return TRUE;
}

void ExplorerModUninit() {
    Wh_Log(L">");

    StopExplorerPart();
}

void ExplorerModSettingsChanged() {
    Wh_Log(L">");

    StopExplorerPart();
    StartExplorerPart();
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
//
// This mod also targets explorer.exe: there, each callback calls the explorer
// part instead (the lines marked "explorer.exe part").

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    // explorer.exe part.
    if (IsExplorerProcess()) {
        g_isExplorer = true;
        return ExplorerModInit();
    }

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
    // explorer.exe part.
    if (g_isExplorer) {
        ExplorerModSettingsChanged();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    // explorer.exe part.
    if (g_isExplorer) {
        ExplorerModUninit();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
