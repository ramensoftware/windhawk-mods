// ==WindhawkMod==
// @id              alt-tab-flip-3d
// @name            Alt+Tab Flip 3D (Vista Style)
// @name:pt-BR      Alt+Tab Flip 3D (estilo Vista)
// @description     Replaces Alt+Tab with a fluid, Windows Vista-style Flip 3D stack of live windows
// @description:pt-BR Substitui o Alt+Tab por uma pilha 3D fluida de janelas ao vivo, no estilo Flip 3D do Windows Vista
// @version         1.7.0
// @author          caliberda
// @github          https://github.com/cesarkali
// @homepage        https://caliberda.com.br
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -ld3d11 -ldxgi -ld2d1 -ldcomp -ldwrite -ldwmapi -lole32 -loleaut32 -luuid -lruntimeobject -lwindowscodecs -lshcore -lshell32 -lgdi32 -ladvapi32
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

![All animation styles](https://i.imgur.com/WtbqDRW.png)

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
* **Cube**: each window is a face of a cube that turns to the next one.
* **Sphere**: windows spread over a ball, in bands that spiral around it;
  the ball turns to bring the chosen one to the middle of its front.
* **Shuffle**: a deck of windows; the front one is lifted over the deck and
  slid in at the back.
* **Domino**: windows standing in a row; the front one falls over.

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
| Ctrl+Alt+Tab              | Open the stack and keep it open (optional)    |
| Arrow keys, mouse wheel   | Flip forward / backward                       |
| Release Alt, Enter, Space | Switch to the window in front                 |
| Click a window            | Switch to that window                         |
| Middle-click a window     | Close that window (optional)                  |
| Click the X on a window   | Close that window (optional)                  |
| Esc                       | Cancel and go back to where you were          |

Win+Tab can open the switcher too, instead of Alt+Tab or together with it
(see the *Shortcut* setting). Release Win to switch. With both, Win+Tab can
use a style of its own (*Win+Tab animation style*).

A quick Alt+Tab tap (shorter than the *show delay*) switches instantly to the
previous window without showing the stack, just like the native switcher.

## How it compares with similar mods

* **[Aero Flip 3D Recreation](https://windhawk.net/mods/aero-flip3d-recreation)**
  brings Flip 3D back on **Win+Tab**. This mod replaces **Alt+Tab** instead,
  draws the windows with real perspective from their live content, and adds
  thirteen more 3D layouts. Both can be installed together, since they use
  different shortcuts, unless the *Shortcut* setting here includes Win+Tab.
* **[Simple Window Switcher](https://windhawk.net/mods/simple-window-switcher)**
  and **[Legacy Alt+Tab dialog](https://windhawk.net/mods/legacy-alt-tab)**
  also replace Alt+Tab. The 2D styles here are close to them, and are included
  so that every style, 3D or 2D, can be picked from one place and share the
  same animations, keys and settings.

**Don't combine this mod with another Alt+Tab replacement**, such as the two
above. Only one of them can take over Alt+Tab.

## Notes

* Windows 11 only (uses Windows Graphics Capture without the yellow border).
* Bars and docks that stay on top of the screen, such as YASB and WindowSill,
  are left out of the switcher. Other apps can be left out with the
  *Excluded apps* setting.
* If the wallpaper can't be read (for example a format Windows shows but
  can't decode, or Windows Spotlight), the copy Windows keeps of the current
  wallpaper is used. If there's none, the desktop shows through, dimmed.
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
  working. If the graphics driver is updated or reset, the switcher starts
  over by itself.
* On laptops with two GPUs, the switcher draws on the one the screen is
  connected to. On slower PCs, the *Frame rate limit* and *Live window
  content* settings make it lighter.

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

![Todos os estilos de animação](https://i.imgur.com/WtbqDRW.png)

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
* **Cubo**: cada janela é uma face de um cubo que gira para a próxima.
* **Esfera**: janelas espalhadas numa bola, em faixas que dão a volta nela
  em espiral; a bola gira para trazer a escolhida para o meio da frente.
* **Embaralhar**: um baralho de janelas; a da frente é levantada por cima e
  colocada no fundo.
* **Dominó**: janelas em pé numa fileira; a da frente cai.

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
| Ctrl+Alt+Tab                 | Abre a pilha e a mantém aberta (opcional)  |
| Setas, roda do mouse         | Avança / volta na pilha                    |
| Soltar Alt, Enter, Espaço    | Muda para a janela da frente               |
| Clicar numa janela           | Muda para essa janela                      |
| Clique do meio numa janela   | Fecha essa janela (opcional)               |
| Clicar no X de uma janela    | Fecha essa janela (opcional)               |
| Esc                          | Cancela e volta para onde você estava      |

O Win+Tab também pode abrir o alternador, no lugar do Alt+Tab ou junto com
ele (veja a configuração *Atalho*). Solte o Win para trocar. Com os dois, o
Win+Tab pode ter um estilo próprio (*Estilo da animação no Win+Tab*).

Um toque rápido no Alt+Tab (menor que o *atraso para exibir*) troca na hora
para a janela anterior sem mostrar a pilha, igual ao alternador nativo.

### Comparação com mods parecidos

* O **[Aero Flip 3D Recreation](https://windhawk.net/mods/aero-flip3d-recreation)**
  traz o Flip 3D de volta no **Win+Tab**. Este mod substitui o **Alt+Tab**,
  desenha as janelas com perspectiva real a partir do conteúdo ao vivo e tem
  mais treze layouts 3D. Os dois podem ficar instalados juntos, porque usam
  atalhos diferentes, a menos que a configuração *Atalho* daqui inclua o
  Win+Tab.
* O **[Simple Window Switcher](https://windhawk.net/mods/simple-window-switcher)**
  e o **[Legacy Alt+Tab dialog](https://windhawk.net/mods/legacy-alt-tab)**
  também substituem o Alt+Tab. Os estilos 2D daqui são parecidos com eles e
  estão incluídos para que todos os estilos, 3D ou 2D, possam ser escolhidos
  num lugar só, com as mesmas animações, teclas e configurações.

**Não use este mod junto com outro substituto do Alt+Tab**, como os dois
acima. Só um deles consegue assumir o Alt+Tab.

### Observações

* Somente Windows 11 (usa o Windows Graphics Capture sem a borda amarela).
* Barras e docks que ficam sempre por cima na tela, como o YASB e o
  WindowSill, ficam fora do alternador. Outros apps podem ser tirados com a
  configuração *Apps excluídos*.
* Se o papel de parede não puder ser lido (por exemplo, um formato que o
  Windows mostra mas não decodifica, ou o Windows Spotlight), é usada a cópia
  que o Windows guarda do papel de parede atual. Se não houver, a área de
  trabalho aparece por trás, escurecida.
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
  nativo continua funcionando. Se o driver de vídeo for atualizado ou
  reiniciado, o alternador recomeça sozinho.
* Em notebooks com duas placas de vídeo, o alternador desenha na placa ligada
  à tela. Em PCs mais fracos, as configurações *Limite de quadros por segundo*
  e *Conteúdo ao vivo das janelas* deixam ele mais leve.

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
  - cube: Cube
  - sphere: Sphere
  - shuffle: Shuffle
  - domino: Domino
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
  - cube: Cubo
  - sphere: Esfera
  - shuffle: Embaralhar
  - domino: Dominó
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
- shortcut: altTab
  $name: Shortcut
  $name:pt-BR: Atalho
  $description: Which shortcut opens the switcher
  $description:pt-BR: Qual atalho abre o alternador
  $options:
  - altTab: Alt+Tab
  - winTab: Win+Tab (Alt+Tab stays native)
  - both: Alt+Tab and Win+Tab
  $options:pt-BR:
  - altTab: Alt+Tab
  - winTab: Win+Tab (o Alt+Tab fica nativo)
  - both: Alt+Tab e Win+Tab
- winTabStyle: same
  $name: Win+Tab animation style
  $name:pt-BR: Estilo da animação no Win+Tab
  $description: Lets Win+Tab use a different style from Alt+Tab. Only when the shortcut includes Win+Tab. The "Native Windows Alt+Tab" style turns the whole mod off, Win+Tab too; to keep Alt+Tab native and use the mod on Win+Tab only, choose the "Win+Tab" shortcut instead
  $description:pt-BR: Deixa o Win+Tab usar um estilo diferente do Alt+Tab. Só quando o atalho inclui o Win+Tab. O estilo "Alt+Tab nativo do Windows" desliga o mod inteiro, o Win+Tab também; para deixar o Alt+Tab nativo e usar o mod só no Win+Tab, escolha o atalho "Win+Tab"
  $options:
  - same: Same as Alt+Tab
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
  - cube: Cube
  - sphere: Sphere
  - shuffle: Shuffle
  - domino: Domino
  - windows11: Windows 11 (enhanced)
  - thumbnails: Thumbnails with titles below
  - icons: Icons and titles
  - list: Vertical list
  - classic: Classic (Windows XP)
  $options:pt-BR:
  - same: O mesmo do Alt+Tab
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
  - cube: Cubo
  - sphere: Esfera
  - shuffle: Embaralhar
  - domino: Dominó
  - windows11: Windows 11 (aprimorado)
  - thumbnails: Miniaturas com título embaixo
  - icons: Ícones e títulos
  - list: Lista vertical
  - classic: Clássico (Windows XP)
- stickyShortcut: true
  $name: Ctrl+Alt+Tab keeps the switcher open
  $name:pt-BR: Ctrl+Alt+Tab mantém o alternador aberto
  $description: Opens the switcher and keeps it open after the keys are released. Pick a window with the arrows, Tab or the mouse wheel, then press Enter or click it. Esc or a click outside the windows closes it. Only when the shortcut includes Alt+Tab
  $description:pt-BR: Abre o alternador e o mantém aberto depois de soltar as teclas. Escolha a janela com as setas, o Tab ou a roda do mouse e aperte Enter ou clique nela. Esc ou um clique fora das janelas fecha. Só quando o atalho inclui o Alt+Tab
- animationDuration: 420
  $name: Open/close animation duration (ms)
  $name:pt-BR: Duração da animação de abrir/fechar (ms)
  $description: How long the windows take to fly into the stack and back (50-3000, default 420)
  $description:pt-BR: Quanto tempo as janelas levam para voar até a pilha e voltar (50-3000, padrão 420)
- flipSpeed: 100
  $name: Flip speed (%)
  $name:pt-BR: Velocidade da troca (%)
  $description: Speed of the spring animation when flipping between windows (25-400, default 100)
  $description:pt-BR: Velocidade da animação de mola ao passar entre as janelas (25-400, padrão 100)
- showDelay: 90
  $name: Show delay (ms)
  $name:pt-BR: Atraso para exibir (ms)
  $description: A quick Alt+Tab shorter than this switches instantly without showing the stack (0-1000, default 90)
  $description:pt-BR: Um Alt+Tab mais rápido que isso troca na hora, sem mostrar a pilha (0-1000, padrão 90)
- tiltAngle: 30
  $name: Tilt angle (degrees)
  $name:pt-BR: Ângulo de inclinação (graus)
  $description: How much the windows are turned (0-70, default 30). Used by Flip 3D, Cascade and Cover Flow
  $description:pt-BR: Quanto as janelas ficam viradas (0-70, padrão 30). Usado pelo Flip 3D, Cascata e Cover Flow
- stackSpacing: 100
  $name: Stack spacing (%)
  $name:pt-BR: Espaçamento da pilha (%)
  $description: Distance between the windows in the stack (40-250, default 100)
  $description:pt-BR: Distância entre as janelas na pilha (40-250, padrão 100)
- windowSize: 100
  $name: Window size (%)
  $name:pt-BR: Tamanho das janelas (%)
  $description: Makes the windows in the switcher bigger, as if closer, or smaller, as if farther away (50-200, default 100). In the grid, windows only grow up to the size of their cell. Not used by the 2D styles
  $description:pt-BR: Deixa as janelas do alternador maiores, como se estivessem mais perto, ou menores, como se estivessem mais longe (50-200, padrão 100). Na grade, as janelas só crescem até o tamanho da célula. Não vale para os estilos 2D
- depth: 100
  $name: Depth (%)
  $name:pt-BR: Profundidade (%)
  $description: How far back the windows behind the front one go (0-300, default 100). Lower values bring them closer. Not used by the 2D styles
  $description:pt-BR: O quanto as janelas de trás vão para o fundo (0-300, padrão 100). Valores menores as trazem para mais perto. Não vale para os estilos 2D
- maxWindows: 20
  $name: Maximum number of windows
  $name:pt-BR: Número máximo de janelas
  $description: Windows beyond this number (by recent use) are not shown (2-40, default 20)
  $description:pt-BR: Janelas além desse número (por uso recente) não são mostradas (2-40, padrão 20)
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
  $description: Used by the blurred wallpaper background (1-100, default 20)
  $description:pt-BR: Usado pelo fundo com papel de parede desfocado (1-100, padrão 20)
- dimOpacity: 25
  $name: Background dimming (%)
  $name:pt-BR: Escurecimento do fundo (%)
  $description: How dark the background gets (0-90, default 25). For "Dim only", around 60 looks best
  $description:pt-BR: Quanto o fundo escurece (0-90, padrão 25). Para "Apenas escurecer", algo perto de 60 fica melhor
- showTitle: true
  $name: Show window title
  $name:pt-BR: Mostrar título da janela
  $description: Shows the icon and title of the window in front
  $description:pt-BR: Mostra o ícone e o título da janela da frente
- titlePosition: bottom
  $name: Title position
  $name:pt-BR: Posição do título
  $description: Where the title of the window in front is shown. Not used by the 2D styles
  $description:pt-BR: Onde o título da janela da frente aparece. Não vale para os estilos 2D
  $options:
  - bottom: Bottom of the screen
  - belowWindow: Right below the selected window
  $options:pt-BR:
  - bottom: Parte de baixo da tela
  - belowWindow: Logo abaixo da janela escolhida
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
- liveContent: all
  $name: Live window content
  $name:pt-BR: Conteúdo ao vivo das janelas
  $description: Which windows keep updating while the switcher is open. The others show a picture taken when it opened, which is lighter on slower PCs
  $description:pt-BR: Quais janelas continuam se atualizando com o alternador aberto. As outras mostram uma imagem tirada ao abrir, o que pesa menos em PCs mais fracos
  $options:
  - all: All windows
  - selected: Only the selected window
  - none: None (lightest)
  $options:pt-BR:
  - all: Todas as janelas
  - selected: Só a janela escolhida
  - none: Nenhuma (mais leve)
- frameRate: screen
  $name: Frame rate limit
  $name:pt-BR: Limite de quadros por segundo
  $description: Caps the animation's frame rate, which lightens the load on slower PCs and laptops
  $description:pt-BR: Limita os quadros por segundo da animação, o que alivia PCs mais fracos e notebooks
  $options:
  - screen: Screen refresh rate
  - "90": About 90 FPS
  - "60": About 60 FPS
  - "30": About 30 FPS
  $options:pt-BR:
  - screen: Taxa de atualização da tela
  - "90": Cerca de 90 FPS
  - "60": Cerca de 60 FPS
  - "30": Cerca de 30 FPS
- middleClickClose: true
  $name: Middle-click closes a window
  $name:pt-BR: Clique do meio fecha a janela
  $description: Clicking a window in the switcher with the middle mouse button (pressing the scroll wheel) closes it
  $description:pt-BR: Clicar numa janela do alternador com o botão do meio do mouse (apertando a rodinha) fecha essa janela
- closeButton: true
  $name: Close button
  $name:pt-BR: Botão de fechar
  $description: Shows an X on the window under the mouse. Clicking it closes that window
  $description:pt-BR: Mostra um X na janela embaixo do mouse. Clicar nele fecha essa janela
- hideBars: true
  $name: Leave out bars and docks
  $name:pt-BR: Deixar de fora barras e docks
  $description: Skips long, thin windows that stay on top or sit outside the work area, like status bars and docks (YASB, WindowSill and similar)
  $description:pt-BR: Ignora janelas compridas e finas que ficam sempre por cima ou fora da área de trabalho, como barras de status e docks (YASB, WindowSill e parecidos)
- excludedApps:
  - ""
  $name: Excluded apps
  $name:pt-BR: Apps excluídos
  $description: Windows of these apps never appear in the switcher. Process names, like yasb.exe, or window class names
  $description:pt-BR: Janelas desses apps nunca aparecem no alternador. Nomes de processo, como yasb.exe, ou nomes de classe da janela
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
#include <unordered_map>
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
constexpr UINT WM_APP_REFRESH_ICONS = WM_APP + 7;  // Posted to itself.
constexpr UINT WM_APP_MOVE_DEVICE = WM_APP + 8;    // Posted to itself.

constexpr WPARAM kStartBackwards = 1;
// Started from explorer's own Alt+Tab hotkey: the keyboard hook didn't see
// the keys, usually because an elevated window has the focus.
constexpr WPARAM kStartFromHotkey = 2;
// Started with Ctrl+Alt+Tab: stays open after the keys are released.
constexpr WPARAM kStartSticky = 4;
// Started with Win+Tab: ends when Win is released.
constexpr WPARAM kStartWinKey = 8;

// The explorer thread that receives the Alt+Tab hotkey, found by its name.
// Until it's found, it's looked for again, less and less often (see
// ExplorerThreadProc).
constexpr PCWSTR kAltTabThreadName = L"Immersive Shell";
constexpr DWORD kFindAltTabThreadIntervalMs = 2000;
constexpr DWORD kFindAltTabThreadMaxIntervalMs = 60000;
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

// Closing a window from the switcher: it shrinks and fades out, and the
// others slide to their new places.
constexpr double kCloseAnimationSeconds = 0.22;
constexpr double kSlideAnimationSeconds = 0.32;
// Smaller close buttons (on far-away cards) aren't shown.
constexpr float kMinCloseButtonRadius = 4;

// Overlay window timers.
// Checks that a minimized window switched to was restored (see CheckRestored).
constexpr UINT_PTR kRestoreTimerId = 1;
constexpr UINT kRestoreCheckMs = 60;
constexpr int kMaxRestoreTries = 3;
// Tries again to create the graphics device after it was lost and couldn't be
// recreated (e.g. while a graphics driver is being installed).
constexpr UINT_PTR kDeviceRetryTimerId = 2;
constexpr UINT kDeviceRetryMs = 2000;

constexpr WCHAR kOverlayClassName[] = L"WindhawkFlip3DSwitcherOverlay";
constexpr WCHAR kProxyClassName[] = L"WindhawkFlip3DSwitcherProxy";
// The overlay's title tells explorer whether the switcher can take over
// Alt+Tab right now. FindWindow compares titles without sending messages.
constexpr WCHAR kOverlayTitleIdle[] = L"Flip 3D";
constexpr WCHAR kOverlayTitleReady[] = L"Flip 3D (ready)";
// Posted by explorer to the overlay when it receives the Alt+Tab hotkey.
// wParam: kStartBackwards, kStartSticky and kStartWinKey flags.
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
    Cube,
    Sphere,
    Shuffle,
    Domino,
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
enum class TitlePosition { Bottom, BelowWindow };
enum class LiveContent { All, Selected, None };

struct Settings {
    // The style of the current session: altTabStyle or winTabStyle.
    AnimationStyle style;
    AnimationStyle altTabStyle;
    AnimationStyle winTabStyle;
    MonitorMode monitor;
    bool altTab;
    bool winTab;
    bool stickyShortcut;
    int animationDurationMs;
    float flipSpeed;
    int showDelayMs;
    float tiltRadians;
    float stackSpacing;
    float windowSize;
    float depth;
    int maxWindows;
    BackgroundMode background;
    float blurAmount;
    float dimOpacity;
    bool showTitle;
    TitlePosition titlePosition;
    bool shadows;
    bool includeMinimized;
    bool minimizedContent;
    LiveContent liveContent;
    // 0: the screen's refresh rate.
    int maxFps;
    bool middleClickClose;
    bool closeButton;
    bool hideBars;
    // Lowercase process or window class names.
    std::vector<std::wstring> excludedApps;
};

std::wstring ToLower(std::wstring text) {
    if (!text.empty()) {
        CharLowerBuffW(text.data(), (DWORD)text.size());
    }
    return text;
}

Settings LoadSettings() {
    Settings s;

    constexpr struct {
        PCWSTR name;
        AnimationStyle style;
    } kStyles[] = {
        {L"flip3d", AnimationStyle::Flip3D},
        {L"cascade", AnimationStyle::Cascade},
        {L"coverflow", AnimationStyle::CoverFlow},
        {L"carousel", AnimationStyle::Carousel},
        {L"grid", AnimationStyle::Grid},
        {L"helix", AnimationStyle::Helix},
        {L"fan", AnimationStyle::Fan},
        {L"panorama", AnimationStyle::Panorama},
        {L"tunnel", AnimationStyle::Tunnel},
        {L"rolodex", AnimationStyle::Rolodex},
        {L"cube", AnimationStyle::Cube},
        {L"sphere", AnimationStyle::Sphere},
        {L"shuffle", AnimationStyle::Shuffle},
        {L"domino", AnimationStyle::Domino},
        {L"windows11", AnimationStyle::Windows11},
        {L"thumbnails", AnimationStyle::Thumbnails},
        {L"icons", AnimationStyle::IconRow},
        {L"list", AnimationStyle::List},
        {L"classic", AnimationStyle::Classic},
        {L"native", AnimationStyle::Native},
    };
    auto readStyle = [&](PCWSTR name, AnimationStyle fallback) {
        const auto value = WindhawkUtils::StringSetting::make(name);
        AnimationStyle result = fallback;
        for (const auto& entry : kStyles) {
            if (wcscmp(value, entry.name) == 0) {
                result = entry.style;
            }
        }
        return result;
    };
    s.altTabStyle = readStyle(L"style", AnimationStyle::Flip3D);
    s.style = s.altTabStyle;
    // "same", or a style that can't be used here, follows Alt+Tab.
    s.winTabStyle = readStyle(L"winTabStyle", s.altTabStyle);
    if (s.winTabStyle == AnimationStyle::Native) {
        s.winTabStyle = s.altTabStyle;
    }

    const auto monitor = WindhawkUtils::StringSetting::make(L"monitor");
    s.monitor = wcscmp(monitor, L"activeWindow") == 0
                    ? MonitorMode::ActiveWindow
                    : MonitorMode::Cursor;

    const auto shortcut = WindhawkUtils::StringSetting::make(L"shortcut");
    s.altTab = wcscmp(shortcut, L"winTab") != 0;
    s.winTab =
        wcscmp(shortcut, L"winTab") == 0 || wcscmp(shortcut, L"both") == 0;
    // Part of the Alt+Tab family: native too when only Win+Tab is taken over.
    s.stickyShortcut = s.altTab && Wh_GetIntSetting(L"stickyShortcut") != 0;

    s.animationDurationMs =
        std::clamp(Wh_GetIntSetting(L"animationDuration"), 50, 3000);
    s.flipSpeed = std::clamp(Wh_GetIntSetting(L"flipSpeed"), 25, 400) / 100.0f;
    s.showDelayMs = std::clamp(Wh_GetIntSetting(L"showDelay"), 0, 1000);
    s.tiltRadians = std::clamp(Wh_GetIntSetting(L"tiltAngle"), 0, 70) *
                    3.14159265f / 180.0f;
    s.stackSpacing =
        std::clamp(Wh_GetIntSetting(L"stackSpacing"), 40, 250) / 100.0f;
    s.windowSize =
        std::clamp(Wh_GetIntSetting(L"windowSize"), 50, 200) / 100.0f;
    s.depth = std::clamp(Wh_GetIntSetting(L"depth"), 0, 300) / 100.0f;
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
    const auto titlePosition =
        WindhawkUtils::StringSetting::make(L"titlePosition");
    s.titlePosition = wcscmp(titlePosition, L"belowWindow") == 0
                          ? TitlePosition::BelowWindow
                          : TitlePosition::Bottom;
    s.shadows = Wh_GetIntSetting(L"shadows") != 0;
    s.includeMinimized = Wh_GetIntSetting(L"includeMinimized") != 0;
    s.minimizedContent = Wh_GetIntSetting(L"minimizedContent") != 0;
    const auto liveContent = WindhawkUtils::StringSetting::make(L"liveContent");
    s.liveContent = wcscmp(liveContent, L"selected") == 0 ? LiveContent::Selected
                    : wcscmp(liveContent, L"none") == 0   ? LiveContent::None
                                                          : LiveContent::All;
    const auto frameRate = WindhawkUtils::StringSetting::make(L"frameRate");
    s.maxFps = std::clamp(_wtoi(frameRate), 0, 240);
    s.middleClickClose = Wh_GetIntSetting(L"middleClickClose") != 0;
    s.closeButton = Wh_GetIntSetting(L"closeButton") != 0;
    s.hideBars = Wh_GetIntSetting(L"hideBars") != 0;

    for (int i = 0; i < 100; i++) {
        std::wstring name =
            WindhawkUtils::StringSetting::make(L"excludedApps[%d]", i).get();
        if (name.empty()) {
            break;
        }
        // Full paths are accepted too: only the file name is compared.
        if (const size_t slash = name.find_last_of(L"\\/");
            slash != std::wstring::npos) {
            name.erase(0, slash + 1);
        }
        name.erase(0, name.find_first_not_of(L" \t\""));
        name.erase(name.find_last_not_of(L" \t\"") + 1);
        if (!name.empty()) {
            s.excludedApps.push_back(ToLower(name));
        }
    }
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

// The key whose release ends the session and switches.
enum class HoldKey { Alt, Win, None };
std::atomic<HoldKey> g_holdKey{HoldKey::Alt};

// The shortcuts that open the switcher, read by the keyboard hook.
std::atomic<bool> g_altTabShortcut{true};
std::atomic<bool> g_winTabShortcut;
std::atomic<bool> g_stickyShortcut;

void ApplyShortcutSettings(const Settings& settings) {
    g_altTabShortcut = settings.altTab;
    g_winTabShortcut = settings.winTab;
    g_stickyShortcut = settings.stickyShortcut;
}

bool IsPanelStyle(AnimationStyle style) {
    return style == AnimationStyle::Windows11 ||
           style == AnimationStyle::Thumbnails ||
           style == AnimationStyle::IconRow || style == AnimationStyle::List ||
           style == AnimationStyle::Classic;
}

// Styles whose windows form one solid shape (a cube, a ball). The window size
// setting scales the whole shape, and the depth setting doesn't apply, so it
// keeps its form.
bool IsSolidStyle(AnimationStyle style) {
    return style == AnimationStyle::Cube || style == AnimationStyle::Sphere;
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

// Ctrl and Win keys pressed on the real keyboard, as seen by the hook (hook
// thread only). Input injected by other programs is left out: AutoHotkey
// scripts, for example, tap Ctrl or release and press Alt again to mask their
// own hotkeys, and that must not block or end a session.
constexpr int kLeftCtrlBit = 1;
constexpr int kRightCtrlBit = 2;
constexpr int kLeftWinBit = 4;
constexpr int kRightWinBit = 8;
int g_physicalModifiers;

int ModifierBit(DWORD vk) {
    switch (vk) {
        case VK_LCONTROL:
            return kLeftCtrlBit;
        case VK_RCONTROL:
            return kRightCtrlBit;
        case VK_LWIN:
            return kLeftWinBit;
        case VK_RWIN:
            return kRightWinBit;
    }
    return 0;
}

void ResetPhysicalModifiers() {
    g_physicalModifiers = 0;
    for (DWORD vk : {VK_LCONTROL, VK_RCONTROL, VK_LWIN, VK_RWIN}) {
        if (IsKeyDown(vk)) {
            g_physicalModifiers |= ModifierBit(vk);
        }
    }
}

// A modifier counts as held only if both the real keyboard and the system
// say so: the system state includes keys injected by other programs, and the
// hook can miss a real release (e.g. while an elevated window has the focus).
bool CtrlHeld() {
    return (g_physicalModifiers & (kLeftCtrlBit | kRightCtrlBit)) &&
           IsKeyDown(VK_CONTROL);
}

bool WinHeld() {
    return (g_physicalModifiers & (kLeftWinBit | kRightWinBit)) &&
           (IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN));
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
    const bool injected = (kb->flags & LLKHF_INJECTED) != 0;
    const DWORD vk = kb->vkCode;

    if (const int bit = ModifierBit(vk); bit && !injected) {
        if (keyDown) {
            g_physicalModifiers |= bit;
        } else {
            g_physicalModifiers &= ~bit;
        }
    }

    if (!g_switching) {
        if (keyDown && vk == VK_TAB && g_ready && g_takeOver) {
            const bool alt = (kb->flags & LLKHF_ALTDOWN) != 0;
            const bool ctrl = CtrlHeld();
            const bool win = WinHeld();
            bool start = false;
            HoldKey hold = HoldKey::Alt;
            if (alt && !win) {
                if (ctrl) {
                    start = g_stickyShortcut;
                    hold = HoldKey::None;
                } else {
                    start = g_altTabShortcut;
                }
            } else if (win && !alt && !ctrl) {
                start = g_winTabShortcut;
                hold = HoldKey::Win;
            }
            if (start) {
                g_holdKey = hold;
                g_switching = true;
                PostMessageW(g_overlayWnd, WM_APP_START,
                             IsKeyDown(VK_SHIFT) ? kStartBackwards : 0, 0);
                PostThreadMessageW(g_hookThreadId, WM_HOOK_ENGAGE, 0, 0);
                return 1;
            }
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    switch (vk) {
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN: {
            // An injected release is left to the UI thread, which switches
            // once the key stays released for a moment (see HoldReleased).
            const HoldKey key = (vk == VK_LWIN || vk == VK_RWIN)
                                    ? HoldKey::Win
                                    : HoldKey::Alt;
            if (!keyDown && !injected && g_holdKey == key && EndSwitching()) {
                PostMessageW(g_overlayWnd, WM_APP_COMMIT, 0, 0);
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }

        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
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
        ResetPhysicalModifiers();
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
// arrives on explorer's "Immersive Shell" thread (see kAltTabThreadName),
// which is found by its name: only that thread is hooked, with a thread hook
// inside explorer itself.

// Explorer thread only.
HHOOK g_messageHook;
HANDLE g_messageHookThread;  // To know when it ends.
std::atomic<int> g_messageHookCalls;
HANDLE g_explorerStopEvent;
HANDLE g_explorerThread;
// Which hotkeys are handed over. Set before the hook is installed.
bool g_forwardAltTab;
bool g_forwardWinTab;
bool g_forwardSticky;

// Hands the hotkey to the switcher. Returns false if the switcher isn't running
// or can't take over Alt+Tab right now; the native switcher then opens.
bool ForwardHotkey(WPARAM flags) {
    HWND overlay = FindWindowW(kOverlayClassName, kOverlayTitleReady);
    if (!overlay) {
        return false;
    }

    // Explorer has just received the hotkey, so it may pass the foreground
    // on. The switcher needs it to take the keyboard focus.
    DWORD processId = 0;
    GetWindowThreadProcessId(overlay, &processId);
    AllowSetForegroundWindow(processId);

    return PostMessageW(overlay, g_hotkeyMessage, flags, 0) != FALSE;
}

LRESULT CALLBACK GetMessageProc(int code, WPARAM wParam, LPARAM lParam) {
    g_messageHookCalls++;

    auto* msg = reinterpret_cast<MSG*>(lParam);
    if (code == HC_ACTION && wParam == PM_REMOVE &&
        msg->message == WM_HOTKEY && HIWORD(msg->lParam) == VK_TAB) {
        const UINT modifiers = LOWORD(msg->lParam);
        const bool alt = (modifiers & MOD_ALT) != 0;
        const bool ctrl = (modifiers & MOD_CONTROL) != 0;
        const bool win = (modifiers & MOD_WIN) != 0;
        bool forward = false;
        WPARAM flags = (modifiers & MOD_SHIFT) ? kStartBackwards : 0;
        if (alt && !ctrl && !win) {
            forward = g_forwardAltTab;
        } else if (alt && ctrl && !win) {
            forward = g_forwardSticky;
            flags |= kStartSticky;
        } else if (win && !alt && !ctrl) {
            forward = g_forwardWinTab;
            flags |= kStartWinKey;
        }
        if (forward && ForwardHotkey(flags)) {
            Wh_Log(L"Hotkey 0x%X+Tab reached explorer, handing it over",
                   modifiers);
            // Explorer gets an empty message instead of the hotkey.
            msg->message = WM_NULL;
        }
    }

    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    g_messageHookCalls--;
    return result;
}

// The ID of explorer's thread that receives the hotkey, or 0 if it doesn't
// exist now.
DWORD FindAltTabThread() {
    DWORD found = 0;
    const DWORD processId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return found;
    }

    THREADENTRY32 entry{sizeof(entry)};
    for (BOOL more = Thread32First(snapshot, &entry); more && !found;
         more = Thread32Next(snapshot, &entry)) {
        if (entry.th32OwnerProcessID != processId) {
            continue;
        }
        HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE,
                                   entry.th32ThreadID);
        if (!thread) {
            continue;
        }
        PWSTR name = nullptr;
        if (SUCCEEDED(GetThreadDescription(thread, &name)) && name &&
            wcscmp(name, kAltTabThreadName) == 0) {
            found = entry.th32ThreadID;
        }
        if (name) {
            LocalFree(name);
        }
        CloseHandle(thread);
    }

    CloseHandle(snapshot);
    return found;
}

// Hooks the thread that receives the hotkey, if it exists now. Returns true if
// it's hooked.
bool HookAltTabThread() {
    const DWORD threadId = FindAltTabThread();
    if (!threadId) {
        return false;
    }
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread) {
        return false;
    }
    // A thread of this process: no module (SetWindowsHookEx docs).
    HHOOK hook =
        SetWindowsHookExW(WH_GETMESSAGE, GetMessageProc, nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %u", GetLastError());
        CloseHandle(thread);
        return false;
    }
    Wh_Log(L"Hooked explorer's thread %u (%s)", threadId, kAltTabThreadName);
    g_messageHook = hook;
    g_messageHookThread = thread;
    return true;
}

void UnhookAltTabThread() {
    if (g_messageHook) {
        UnhookWindowsHookEx(g_messageHook);
        g_messageHook = nullptr;
    }
    if (g_messageHookThread) {
        CloseHandle(g_messageHookThread);
        g_messageHookThread = nullptr;
    }

    // Let calls already running on explorer's thread leave the module before
    // it gets unloaded. No new calls can start after the hook is removed, so
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
        if (WaitForSingleObject(g_explorerStopEvent,
                                kWaitForTaskbarIntervalMs) != WAIT_TIMEOUT) {
            return 0;
        }
    }

    // Hooks the thread, and again if it's ever made anew. Until it's hooked,
    // it's looked for again, less and less often.
    DWORD interval = kFindAltTabThreadIntervalMs;
    bool loggedMissing = false;
    for (;;) {
        if (!g_messageHook && !HookAltTabThread() && !loggedMissing) {
            Wh_Log(L"Explorer's \"%s\" thread wasn't found yet",
                   kAltTabThreadName);
            loggedMissing = true;
        }

        const HANDLE events[] = {g_explorerStopEvent, g_messageHookThread};
        const DWORD result = WaitForMultipleObjects(
            g_messageHook ? 2 : 1, events, FALSE,
            g_messageHook ? INFINITE : interval);
        if (result == WAIT_TIMEOUT) {
            interval = std::min(interval * 2, kFindAltTabThreadMaxIntervalMs);
        } else if (result == WAIT_OBJECT_0 + 1) {
            // The thread ended (its ID can be reused).
            UnhookAltTabThread();
            interval = kFindAltTabThreadIntervalMs;
            loggedMissing = false;
        } else {
            break;
        }
    }

    UnhookAltTabThread();
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
}

// Doesn't wait for anything, so explorer's startup isn't delayed.
void StartExplorerPart() {
    const Settings settings = LoadSettings();
    if (settings.style == AnimationStyle::Native) {
        // Nothing to hand over.
        return;
    }
    g_forwardAltTab = settings.altTab;
    g_forwardWinTab = settings.winTab;
    g_forwardSticky = settings.stickyShortcut;

    g_explorerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_explorerStopEvent) {
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

// Whether the window belongs to one of the excluded apps, by process name or
// window class name.
bool IsExcludedApp(HWND hwnd, const std::vector<std::wstring>& excluded) {
    if (excluded.empty()) {
        return false;
    }

    WCHAR className[256];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        const std::wstring name = ToLower(className);
        if (std::find(excluded.begin(), excluded.end(), name) !=
            excluded.end()) {
            return true;
        }
        // Packaged apps are hosted by ApplicationFrameHost.exe; the app's
        // own process owns the CoreWindow inside the frame.
        if (name == L"applicationframewindow") {
            if (HWND core = FindWindowExW(hwnd, nullptr,
                                          L"Windows.UI.Core.CoreWindow",
                                          nullptr)) {
                hwnd = core;
            }
        }
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    const bool gotPath =
        QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!gotPath) {
        return false;
    }
    PCWSTR fileName = wcsrchr(path, L'\\');
    const std::wstring name = ToLower(fileName ? fileName + 1 : path);
    return std::find(excluded.begin(), excluded.end(), name) != excluded.end();
}

// Bars and docks (YASB, WindowSill and similar) look like normal windows to
// the system. They're long and thin, and stay on top or sit outside the work
// area (the space reserved for app bars).
bool IsBarWindow(HWND hwnd) {
    RECT rect;
    if (IsIconic(hwnd) || !GetWindowRect(hwnd, &rect)) {
        return false;
    }
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST),
                         &info)) {
        return false;
    }
    const RECT& monitor = info.rcMonitor;
    const LONG width = rect.right - rect.left;
    const LONG height = rect.bottom - rect.top;
    const LONG monitorWidth = monitor.right - monitor.left;
    const LONG monitorHeight = monitor.bottom - monitor.top;
    const bool horizontal =
        width * 10 >= monitorWidth * 3 && height * 100 <= monitorHeight * 15;
    const bool vertical =
        height * 10 >= monitorHeight * 3 && width * 100 <= monitorWidth * 15;
    if (!horizontal && !vertical) {
        return false;
    }

    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) {
        return true;
    }
    RECT inWorkArea;
    if (!IntersectRect(&inWorkArea, &rect, &info.rcWork)) {
        return true;
    }
    // Mostly outside the work area.
    const LONG visible = (inWorkArea.right - inWorkArea.left) *
                         (inWorkArea.bottom - inWorkArea.top);
    return visible * 2 < width * height;
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
    float squeeze = 1;    // Height, on top of the scale (cube faces).
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
            Lerp(a.highlight, b.highlight, t),
            Lerp(a.squeeze, b.squeeze, t)};
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

// The GPU the monitor is connected to. On laptops with two GPUs, rendering on
// the other one means every frame is copied between them.
com_ptr<IDXGIAdapter1> FindMonitorAdapter(HMONITOR monitor) {
    com_ptr<IDXGIFactory1> factory;
    if (!monitor || FAILED(CreateDXGIFactory1(IID_PPV_ARGS(factory.put())))) {
        return nullptr;
    }
    com_ptr<IDXGIAdapter1> adapter;
    for (UINT i = 0; SUCCEEDED(factory->EnumAdapters1(i, adapter.put()));
         i++) {
        com_ptr<IDXGIOutput> output;
        for (UINT j = 0; SUCCEEDED(adapter->EnumOutputs(j, output.put()));
             j++) {
            DXGI_OUTPUT_DESC desc;
            if (SUCCEEDED(output->GetDesc(&desc)) && desc.Monitor == monitor) {
                return adapter;
            }
            output = nullptr;
        }
        adapter = nullptr;
    }
    return nullptr;
}

bool GetMonitorAdapterLuid(HMONITOR monitor, LUID* luid) {
    com_ptr<IDXGIAdapter1> adapter = FindMonitorAdapter(monitor);
    DXGI_ADAPTER_DESC1 desc;
    if (!adapter || FAILED(adapter->GetDesc1(&desc))) {
        return false;
    }
    *luid = desc.AdapterLuid;
    return true;
}

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

    // When the window was asked to close (see CloseItem), or 0.
    double closeRequestedAt = 0;

    // The pose it was last drawn with, and the one it slides from after
    // another window closed (see RemoveClosedItems).
    Pose lastPose;
    bool hasLastPose = false;
    Pose slideFrom;
    bool slides = false;

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

struct CloseButton {
    D2D1_POINT_2F center{};
    float radius = 0;

    bool Contains(D2D1_POINT_2F point) const {
        const float dx = point.x - center.x;
        const float dy = point.y - center.y;
        return radius > 0 && dx * dx + dy * dy <= radius * radius;
    }
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
    bool CreateDeviceResources(HMONITOR monitor);
    void ReleaseDeviceResources();
    bool DeviceRemoved() const;
    void RecreateDevice(HMONITOR monitor);
    void MoveDeviceToMonitor();
    void HandleDeviceLost();
    void UpdateFrameRate();
    void StopRestoreCheck();
    void CheckRestored();
    bool EnsureSwapChain(UINT width, UINT height);

    void OnHotkey(WPARAM flags);
    void OnStart(WPARAM flags);
    void OnStep(int step);
    void OnCommit(int index);
    void OnCancel();
    void OnClick(POINT pt);
    void OnMiddleClick(POINT pt);
    int HitTest(D2D1_POINT_2F point, CloseButton* button) const;
    CloseButton CloseButtonForQuad(const Quad& quad) const;
    CloseButton CloseButtonForCell(const D2D1_RECT_F& cell) const;
    void CloseItem(int index);
    void RemoveClosedItems(double now);
    void UpdateHover();
    void DrawCloseButton(float t);

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
    bool HoldReleased(double now);
    void BeginClose(bool commit);
    void FinishClose();
    void EndSession();
    void ActivateWindow(HWND hwnd);
    HWND CommitTarget() const;

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
    void RefreshIcons();
    com_ptr<IWICBitmap> ReadIconPixels(HICON icon);
    com_ptr<IWICBitmap> ReadAppIconPixels(HWND hwnd);
    com_ptr<ID2D1Bitmap1> CreateIconBitmap(IWICBitmap* pixels);
    com_ptr<ID2D1Bitmap1> CreatePlaceholder(const Item& item);
    com_ptr<ID2D1Bitmap1> CreateTargetBitmap(UINT width, UINT height);
    com_ptr<ID2D1Bitmap1> DecodeWallpaper(const std::wstring& path,
                                          UINT width,
                                          UINT height,
                                          DESKTOP_WALLPAPER_POSITION position,
                                          D2D1_RECT_F* rect);
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
    // The GPU the device was created on.
    LUID m_adapterLuid{};
    // The monitor whose GPU was last compared with it.
    HMONITOR m_checkedMonitor = nullptr;
    // Signaled when the device is lost (e.g. a graphics driver update), so
    // it's recreated right away instead of at the next Alt+Tab.
    HANDLE m_deviceRemovedEvent = nullptr;
    DWORD m_deviceRemovedCookie = 0;
    // The frame rate limit (see RenderFrame): the time between frames (0 for
    // every refresh), the time between refreshes, and when the next frame is
    // due.
    double m_frameInterval = 0;
    double m_refreshInterval = 1.0 / 60;
    double m_nextFrameAt = 0;
    // The last frame's result, so a repeating error is logged once.
    HRESULT m_lastRenderError = S_OK;

    // A minimized window being switched to, checked until it's restored (see
    // CheckRestored).
    HWND m_restoreWnd = nullptr;
    int m_restoreTries = 0;

    // Window icons, kept across sessions (see GetIconBitmap). The pixels are
    // null for windows without an icon. They're kept when the device changes;
    // only the bitmap belongs to the device, and is made again from them.
    struct CachedIcon {
        HICON source = nullptr;  // The window icon it was made from, if any.
        com_ptr<IWICBitmap> pixels;
        com_ptr<ID2D1Bitmap1> bitmap;
    };
    std::unordered_map<HWND, CachedIcon> m_iconCache;

    // Background.
    com_ptr<ID2D1Bitmap1> m_background;
    std::wstring m_backgroundKey;

    // Session.
    State m_state = State::Idle;
    std::vector<std::unique_ptr<Item>> m_items;
    // Closed windows, shrinking and fading out where they were.
    struct ClosingItem {
        std::unique_ptr<Item> item;
        double start;
    };
    std::vector<ClosingItem> m_closingItems;
    // When the remaining windows started sliding to their new places.
    double m_slideStart = 0;
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
    // The chosen window was activated when the closing animation started.
    bool m_activatedEarly = false;
    // The session was opened with Win+Tab, so it uses winTabStyle.
    bool m_winTabSession = false;
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
    // The item under the mouse, which shows a close button (see UpdateHover).
    int m_hoverIndex = -1;
    CloseButton m_hoverButton;
    bool m_hoverOnButton = false;

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
    g_takeOver = m_settings.altTabStyle != AnimationStyle::Native;
    ApplyShortcutSettings(m_settings);

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

    if (!CreateDeviceResources(
            MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY))) {
        // Alt+Tab isn't taken over, so the native switcher keeps working.
        // Tried again regularly (e.g. a graphics driver is being installed).
        ReleaseDeviceResources();
        SetTimer(m_hwnd, kDeviceRetryTimerId, kDeviceRetryMs, nullptr);
        PublishState();
        return;
    }
    KillTimer(m_hwnd, kDeviceRetryTimerId);

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
    m_iconCache.clear();
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

bool Switcher::CreateDeviceResources(HMONITOR monitor) {
    // On the monitor's GPU when it's known, else on the default one.
    com_ptr<IDXGIAdapter1> adapter = FindMonitorAdapter(monitor);
    HRESULT hr = D3D11CreateDevice(
        adapter.get(),
        adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
        m_d3dDevice.put(), nullptr, m_d3dContext.put());
    if (FAILED(hr) && adapter) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                               D3D11_SDK_VERSION, m_d3dDevice.put(), nullptr,
                               m_d3dContext.put());
    }
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: 0x%08X", (UINT)hr);
        return false;
    }

    if (auto device4 = m_d3dDevice.try_as<ID3D11Device4>()) {
        m_deviceRemovedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (m_deviceRemovedEvent &&
            FAILED(device4->RegisterDeviceRemovedEvent(
                m_deviceRemovedEvent, &m_deviceRemovedCookie))) {
            CloseHandle(m_deviceRemovedEvent);
            m_deviceRemovedEvent = nullptr;
        }
    }

    // Capture frame pools use the device from their own threads.
    if (auto multithread = m_d3dDevice.try_as<ID3D11Multithread>()) {
        multithread->SetMultithreadProtected(TRUE);
    }

    m_dxgiDevice = m_d3dDevice.as<IDXGIDevice>();
    // At most one frame waits for the screen, so a key press shows on the
    // next one, and the last frames of the closing animation aren't dropped
    // when the overlay is hidden.
    if (auto device1 = m_dxgiDevice.try_as<IDXGIDevice1>()) {
        device1->SetMaximumFrameLatency(1);
    }
    com_ptr<IDXGIAdapter> deviceAdapter;
    DXGI_ADAPTER_DESC adapterDesc;
    if (SUCCEEDED(m_dxgiDevice->GetAdapter(deviceAdapter.put())) &&
        SUCCEEDED(deviceAdapter->GetDesc(&adapterDesc))) {
        m_adapterLuid = adapterDesc.AdapterLuid;
        Wh_Log(L"Rendering on %s", adapterDesc.Description);
    }

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
    // The bitmaps belong to the device, the icons' pixels don't.
    for (auto& [hwnd, entry] : m_iconCache) {
        entry.bitmap = nullptr;
    }
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
    if (m_deviceRemovedEvent) {
        if (auto device4 = m_d3dDevice.try_as<ID3D11Device4>()) {
            device4->UnregisterDeviceRemoved(m_deviceRemovedCookie);
        }
        CloseHandle(m_deviceRemovedEvent);
        m_deviceRemovedEvent = nullptr;
    }
    m_d3dDevice = nullptr;
    m_adapterLuid = {};
}

bool Switcher::DeviceRemoved() const {
    return m_d3dDevice && m_d3dDevice->GetDeviceRemovedReason() != S_OK;
}

// Starts over with new graphics devices. If that fails (e.g. while a graphics
// driver is being installed), Alt+Tab is left to Windows meanwhile, and it's
// tried again regularly.
void Switcher::RecreateDevice(HMONITOR monitor) {
    ReleaseDeviceResources();
    if (CreateDeviceResources(monitor)) {
        g_ready = true;
        KillTimer(m_hwnd, kDeviceRetryTimerId);
    } else {
        ReleaseDeviceResources();
        SetTimer(m_hwnd, kDeviceRetryTimerId, kDeviceRetryMs, nullptr);
    }
    PublishState();
}

// Moves the device to the GPU of the monitor last switched on (or of the
// primary one, if that monitor is gone) when it's on another GPU, so frames
// aren't copied between GPUs. Only while idle, since it takes a moment. The
// monitor's GPU is only looked up again when the monitor or the displays
// change.
void Switcher::MoveDeviceToMonitor() {
    if (m_state != State::Idle || !m_d3dDevice) {
        return;
    }
    HMONITOR monitor = MonitorFromRect(&m_monitor, MONITOR_DEFAULTTOPRIMARY);
    if (monitor == m_checkedMonitor) {
        return;
    }
    m_checkedMonitor = monitor;
    LUID adapter{};
    if (GetMonitorAdapterLuid(monitor, &adapter) &&
        (adapter.LowPart != m_adapterLuid.LowPart ||
         adapter.HighPart != m_adapterLuid.HighPart)) {
        Wh_Log(L"Moving to the monitor's GPU");
        RecreateDevice(monitor);
    }
}

void Switcher::HandleDeviceLost() {
    Wh_Log(L"Graphics device lost, recreating");
    // The Alt release will reach the foreground app normally.
    EndSwitching();
    HideOverlay();
    Teardown();
    m_state = State::Idle;
    RecreateDevice(MonitorFromRect(&m_monitor, MONITOR_DEFAULTTOPRIMARY));
}

// Sets up the frame rate limit (see RenderFrame) for the switcher's monitor.
// A limit at or above its refresh rate changes nothing.
void Switcher::UpdateFrameRate() {
    m_frameInterval = 0;
    m_nextFrameAt = 0;
    if (m_settings.maxFps <= 0) {
        return;
    }
    // The refresh rate of the switcher's monitor, which can differ from the
    // others'. 0 and 1 mean the hardware's default rate.
    double refresh = 0;
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    DEVMODEW mode{};
    mode.dmSize = sizeof(mode);
    if (GetMonitorInfoW(MonitorFromRect(&m_monitor, MONITOR_DEFAULTTOPRIMARY),
                        &info) &&
        EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
        mode.dmDisplayFrequency > 1) {
        refresh = mode.dmDisplayFrequency;
    } else {
        DWM_TIMING_INFO timing{sizeof(timing)};
        if (FAILED(DwmGetCompositionTimingInfo(nullptr, &timing)) ||
            !timing.rateRefresh.uiDenominator) {
            return;
        }
        refresh = (double)timing.rateRefresh.uiNumerator /
                  timing.rateRefresh.uiDenominator;
    }
    if (refresh <= 0 || m_settings.maxFps >= refresh * 0.95) {
        return;
    }
    m_frameInterval = 1.0 / m_settings.maxFps;
    m_refreshInterval = 1.0 / refresh;
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
    m_iconCache.clear();
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
            // Also wakes up when the graphics device is lost.
            const DWORD count = m_deviceRemovedEvent ? 1 : 0;
            if (MsgWaitForMultipleObjectsEx(count, &m_deviceRemovedEvent,
                                            timeout, QS_ALLINPUT,
                                            MWMO_INPUTAVAILABLE) ==
                    WAIT_OBJECT_0 &&
                count && DeviceRemoved()) {
                if (m_state == State::Idle) {
                    Wh_Log(L"Graphics device lost, recreating");
                    RecreateDevice(
                        MonitorFromRect(&m_monitor, MONITOR_DEFAULTTOPRIMARY));
                } else {
                    HandleDeviceLost();
                }
            }
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
            if (HoldReleased(now) && EndSwitching()) {
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
        OnHotkey(wParam);
        return 0;
    }

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_LBUTTONDOWN:
            OnClick({(short)LOWORD(lParam), (short)HIWORD(lParam)});
            return 0;

        case WM_MBUTTONDOWN:
            OnMiddleClick({(short)LOWORD(lParam), (short)HIWORD(lParam)});
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

        case WM_APP_REFRESH_ICONS:
            // Skipped if a new session started meanwhile; it posts this again
            // when it ends.
            if (m_state == State::Idle && m_ctx) {
                RefreshIcons();
            }
            return 0;

        case WM_APP_MOVE_DEVICE:
            MoveDeviceToMonitor();
            return 0;

        case WM_APP_SETTINGS:
            m_settings = LoadSettings();
            // A session in progress keeps the style of its shortcut.
            if (m_state != State::Idle && m_winTabSession) {
                m_settings.style = m_settings.winTabStyle;
            }
            g_takeOver = m_settings.altTabStyle != AnimationStyle::Native;
            ApplyShortcutSettings(m_settings);
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

        case WM_TIMER:
            if (wParam == kRestoreTimerId) {
                CheckRestored();
                return 0;
            }
            if (wParam == kDeviceRetryTimerId) {
                if (!g_takeOver || m_d3dDevice) {
                    KillTimer(m_hwnd, kDeviceRetryTimerId);
                } else if (m_state == State::Idle) {
                    if (m_borderlessRequested) {
                        RecreateDevice(MonitorFromRect(
                            &m_monitor, MONITOR_DEFAULTTOPRIMARY));
                    } else {
                        // The first start failed.
                        Activate();
                    }
                }
                return 0;
            }
            break;

        case WM_DISPLAYCHANGE:
            // E.g. an external monitor on the other GPU was unplugged: move
            // the device back, so that GPU can power down.
            m_checkedMonitor = nullptr;
            m_backgroundKey.clear();
            PostMessageW(m_hwnd, WM_APP_MOVE_DEVICE, 0, 0);
            break;

        case WM_SETTINGCHANGE:
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
void Switcher::OnHotkey(WPARAM flags) {
    if (!g_ready || !g_takeOver) {
        return;
    }
    const bool backwards = (flags & kStartBackwards) != 0;
    if (g_switching) {
        // Tab pressed again while Alt is held.
        OnStep(backwards ? -1 : 1);
        return;
    }
    g_holdKey = (flags & kStartSticky)   ? HoldKey::None
                : (flags & kStartWinKey) ? HoldKey::Win
                                         : HoldKey::Alt;
    if (g_switching.exchange(true)) {
        OnStep(backwards ? -1 : 1);
        return;
    }
    Wh_Log(L"Hotkey reached explorer, taking it over");
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

    // A previous window may still be on its way back from minimized.
    StopRestoreCheck();

    // The device is gone (e.g. the graphics driver was updated or reset):
    // start over with a new one. If it's only on another GPU than this
    // monitor's, this session keeps it, and it moves once the session is over
    // (see MoveDeviceToMonitor), so Alt+Tab never waits for that.
    if (DeviceRemoved()) {
        RecreateDevice(monitor);
        if (!g_ready) {
            EndSwitching();
            return;
        }
    }

    m_winTabSession = g_holdKey == HoldKey::Win;
    m_settings.style =
        m_winTabSession ? m_settings.winTabStyle : m_settings.altTabStyle;

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
    m_hoverIndex = -1;
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

// Fallback for a missed Alt (or Win) release (e.g. a secure desktop took the
// input, or the keys never reached the hook), and the way injected releases
// are handled. The hook normally reports a real release first, so this only
// counts once the key stays released for a moment. Sessions opened with
// Ctrl+Alt+Tab don't end here.
bool Switcher::HoldReleased(double now) {
    bool held = true;
    switch (g_holdKey.load()) {
        case HoldKey::Alt:
            held = IsKeyDown(VK_MENU);
            break;
        case HoldKey::Win:
            held = IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN);
            break;
        case HoldKey::None:
            break;
    }
    if (!g_switching || held) {
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
        EndSession();
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
        EndSession();
    } else if (m_state == State::Open) {
        BeginClose(false);
    }
}

void Switcher::OnClick(POINT pt) {
    if (m_state != State::Open) {
        return;
    }
    const D2D1_POINT_2F point{(float)pt.x, (float)pt.y};
    CloseButton button;
    const int index = HitTest(point, &button);
    if (index < 0) {
        // In a Ctrl+Alt+Tab session, the mouse alone can dismiss it. In the
        // 2D styles, only outside the panel: a click between two thumbnails
        // is just a near miss.
        const bool onPanel = IsPanelStyle(m_settings.style) &&
                             point.x >= m_panel.left &&
                             point.x < m_panel.right &&
                             point.y >= m_panel.top && point.y < m_panel.bottom;
        if (g_holdKey == HoldKey::None && !onPanel && EndSwitching()) {
            OnCancel();
        }
        return;
    }
    if (m_settings.closeButton && button.Contains(point)) {
        CloseItem(index);
        return;
    }
    EndSwitching();
    OnCommit(index);
}

void Switcher::OnMiddleClick(POINT pt) {
    if (m_state != State::Open || !m_settings.middleClickClose) {
        return;
    }
    CloseButton button;
    const int index = HitTest({(float)pt.x, (float)pt.y}, &button);
    if (index >= 0) {
        CloseItem(index);
    }
}

// The item under a point (overlay coordinates), or -1. Also returns where that
// item's close button is.
int Switcher::HitTest(D2D1_POINT_2F point, CloseButton* button) const {
    if (IsPanelStyle(m_settings.style)) {
        for (size_t i = 0; i < m_cells.size(); i++) {
            const D2D1_RECT_F& cell = m_cells[i];
            if (point.x >= cell.left && point.x < cell.right &&
                point.y >= cell.top && point.y < cell.bottom) {
                *button = CloseButtonForCell(cell);
                return (int)i;
            }
        }
        return -1;
    }
    for (auto it = m_drawList.rbegin(); it != m_drawList.rend(); ++it) {
        if (it->pose.opacity < 0.5f || !QuadContains(it->quad, point)) {
            continue;
        }
        for (size_t i = 0; i < m_items.size(); i++) {
            if (m_items[i].get() == it->item) {
                *button = CloseButtonForQuad(it->quad);
                return (int)i;
            }
        }
    }
    return -1;
}

// Inside the top-right corner of a card, whatever its perspective.
CloseButton Switcher::CloseButtonForQuad(const Quad& quad) const {
    auto length = [](D2D1_POINT_2F a, D2D1_POINT_2F b) {
        return std::hypot(b.x - a.x, b.y - a.y);
    };
    const float shortest = std::min(length(quad.p[0], quad.p[1]),
                                    length(quad.p[1], quad.p[2]));
    CloseButton button;
    button.radius = std::min(std::round(13 * m_dpiScale), shortest * 0.12f);
    if (button.radius < kMinCloseButtonRadius) {
        // Too small to see or hit: no button.
        button.radius = 0;
    }

    const D2D1_POINT_2F corner = quad.p[1];
    const float centerX =
        (quad.p[0].x + quad.p[1].x + quad.p[2].x + quad.p[3].x) / 4;
    const float centerY =
        (quad.p[0].y + quad.p[1].y + quad.p[2].y + quad.p[3].y) / 4;
    const float toCenter =
        std::max(length(corner, {centerX, centerY}), 1.0f);
    const float inset = std::min(button.radius * 1.35f * 1.414f, toCenter);
    button.center = {corner.x + (centerX - corner.x) / toCenter * inset,
                     corner.y + (centerY - corner.y) / toCenter * inset};
    return button;
}

CloseButton Switcher::CloseButtonForCell(const D2D1_RECT_F& cell) const {
    CloseButton button;
    button.radius =
        std::min(std::round(11 * m_dpiScale),
                 std::min(cell.right - cell.left, cell.bottom - cell.top) *
                     0.16f);
    if (button.radius < kMinCloseButtonRadius) {
        button.radius = 0;
    }
    const float inset = std::round(button.radius * 1.3f);
    button.center = {cell.right - inset, cell.top + inset};
    return button;
}

// Asks the window to close, like its own close button. The item goes away
// once the window does (see RemoveClosedItems); if the app asks something
// first (e.g. to save), the window stays in the switcher.
void Switcher::CloseItem(int index) {
    Item& item = *m_items[index];
    if (item.closeRequestedAt) {
        return;
    }
    item.closeRequestedAt = NowSeconds();
    if (!PostMessageW(item.hwnd, WM_SYSCOMMAND, SC_CLOSE, 0)) {
        // E.g. an elevated window, which this process can't post to.
        Wh_Log(L"Couldn't close %p: %u", item.hwnd, GetLastError());
        item.closeRequestedAt = 0;
    }
}

void Switcher::RemoveClosedItems(double now) {
    if (!m_items.empty()) {
        // m_target isn't wrapped, and the item count may change below: wrap
        // it so that SelectedIndex() keeps pointing to the same item. The
        // shift is a multiple of the count, so nothing moves on screen.
        const int selected = SelectedIndex();
        m_scroll -= (double)(m_target - selected);
        m_target = selected;
    }
    bool removed = false;
    for (size_t i = 0; i < m_items.size();) {
        Item& item = *m_items[i];
        if (!item.closeRequestedAt) {
            i++;
            continue;
        }
        if (IsWindow(item.hwnd) && IsSwitchableWindow(item.hwnd)) {
            // Still there after a while: the app kept it open.
            if (now - item.closeRequestedAt > 3) {
                item.closeRequestedAt = 0;
            }
            i++;
            continue;
        }

        // Keep the selected window in front: removing an item before it
        // shifts the indexes.
        if ((int)i < SelectedIndex()) {
            m_target--;
            m_scroll -= 1;
        }
        // Its last frame stays, for the animation.
        StopCapture(item);
        if (item.hasLastPose) {
            m_closingItems.push_back({std::move(m_items[i]), now});
        }
        m_items.erase(m_items.begin() + i);
        removed = true;
    }
    if (!removed) {
        return;
    }

    // The others slide from where they are now to their new places.
    for (auto& item : m_items) {
        item->slideFrom = item->lastPose;
        item->slides = item->hasLastPose;
    }
    m_slideStart = now;

    // The draw list points to the items.
    m_drawList.clear();
    m_hoverIndex = -1;
    m_titleIndex = -1;
    m_titleLayout = nullptr;
    // With nothing left, this also clears the panel cells.
    ComputeLayout();
    if (m_items.empty()) {
        EndSwitching();
        BeginClose(false);
    }
}

// Finds the item under the mouse, which shows the close button.
void Switcher::UpdateHover() {
    m_hoverIndex = -1;
    m_hoverOnButton = false;
    if (!m_settings.closeButton || m_state != State::Open) {
        return;
    }
    POINT cursor;
    if (!GetCursorPos(&cursor) || !PtInRect(&m_monitor, cursor)) {
        return;
    }
    const D2D1_POINT_2F point{(float)(cursor.x - m_monitor.left),
                              (float)(cursor.y - m_monitor.top)};
    m_hoverIndex = HitTest(point, &m_hoverButton);
    m_hoverOnButton = m_hoverIndex >= 0 && m_hoverButton.Contains(point);
}

void Switcher::DrawCloseButton(float t) {
    if (m_hoverIndex < 0 || m_hoverIndex >= (int)m_items.size() ||
        m_items[m_hoverIndex]->closeRequestedAt || t < 0.01f) {
        return;
    }
    const CloseButton& button = m_hoverButton;
    const float r = button.radius;
    if (r <= 0) {
        return;
    }
    const D2D1_ELLIPSE circle{button.center, r, r};
    if (m_hoverOnButton) {
        // Red, like the close buttons of Windows.
        m_brush->SetColor(D2D1::ColorF(0.77f, 0.17f, 0.11f, 0.95f * t));
    } else {
        m_brush->SetColor(D2D1::ColorF(0.12f, 0.12f, 0.13f, 0.88f * t));
    }
    m_ctx->FillEllipse(circle, m_brush.get());
    m_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.22f * t));
    m_ctx->DrawEllipse(circle, m_brush.get(), 1);

    const float arm = r * 0.36f;
    const float stroke = std::max(1.5f, r * 0.14f);
    const D2D1_POINT_2F c = button.center;
    m_brush->SetColor(D2D1::ColorF(1, 1, 1, t));
    m_ctx->DrawLine({c.x - arm, c.y - arm}, {c.x + arm, c.y + arm},
                    m_brush.get(), stroke);
    m_ctx->DrawLine({c.x - arm, c.y + arm}, {c.x + arm, c.y - arm},
                    m_brush.get(), stroke);
}

struct EnumWindowsContext {
    std::vector<HWND> windows;
    const Settings* settings;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<EnumWindowsContext*>(lParam);
    const Settings& settings = *context->settings;
    if (IsSwitchableWindow(hwnd) &&
        (settings.includeMinimized || !IsIconic(hwnd)) &&
        !(settings.hideBars && IsBarWindow(hwnd)) &&
        !IsExcludedApp(hwnd, settings.excludedApps)) {
        context->windows.push_back(hwnd);
    }
    return context->windows.size() < (size_t)settings.maxWindows;
}

void Switcher::CollectWindows() {
    EnumWindowsContext context{{}, &m_settings};
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
    m_closingItems.clear();
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
    UpdateFrameRate();
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

    // Bring the chosen window up right away, behind the overlay, so the
    // desktop revealed as the switcher fades out already matches where the
    // windows are flying to. Otherwise the old front window shows through
    // while they move (a double image when frames come slowly). Minimized
    // windows wait for the end, so their restore animation stays hidden, and
    // so do always-on-top ones, which would come up above the overlay.
    m_activatedEarly = false;
    HWND target = CommitTarget();
    if (target && !IsIconic(target) &&
        !(GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        ActivateWindow(target);
        m_activatedEarly = true;
    }
}

// The window to switch to when the session ends, if any.
HWND Switcher::CommitTarget() const {
    if (m_commit) {
        const int selected = SelectedIndex();
        return selected >= 0 ? m_items[selected]->hwnd : nullptr;
    }
    // Canceled after the overlay took the focus: give it back.
    return m_hasFocus ? m_previousForeground : nullptr;
}

void Switcher::FinishClose() {
    HWND target = m_activatedEarly ? nullptr : CommitTarget();
    m_activatedEarly = false;
    if (target) {
        ActivateWindow(target);
        // Give DWM a frame to bring the window up before revealing it.
        DwmFlush();
    }
    EndSession();
}

void Switcher::EndSession() {
    HideOverlay();
    Teardown();
    m_state = State::Idle;
    // Nobody is waiting now, so check whether the device should move to this
    // monitor's GPU, and the cached icons (see GetIconBitmap).
    PostMessageW(m_hwnd, WM_APP_MOVE_DEVICE, 0, 0);
    PostMessageW(m_hwnd, WM_APP_REFRESH_ICONS, 0, 0);
}

void Switcher::ActivateWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }

    HWND popup = GetLastActivePopup(hwnd);
    if (!popup || !IsWindowVisible(popup) || !IsWindowEnabled(popup)) {
        popup = hwnd;
    }

    // Input first, which allows this process to change the foreground. A
    // restore asked for before that can leave the window activated but still
    // minimized (console windows do).
    SendDummyKeyPress();
    if (IsIconic(hwnd)) {
        ShowWindowAsync(hwnd, SW_RESTORE);
        // Some windows still ignore it: check, and try other ways.
        m_restoreWnd = hwnd;
        m_restoreTries = 0;
        SetTimer(m_hwnd, kRestoreTimerId, kRestoreCheckMs, nullptr);
    }

    if (!SetForegroundWindow(popup)) {
        Wh_Log(L"SetForegroundWindow failed for %p", popup);
    }
}

void Switcher::StopRestoreCheck() {
    if (m_restoreWnd) {
        KillTimer(m_hwnd, kRestoreTimerId);
        m_restoreWnd = nullptr;
    }
}

// A minimized window switched to is still minimized: ask again, the way the
// taskbar does it, then directly (without waiting for its app, which may be
// busy).
void Switcher::CheckRestored() {
    HWND hwnd = m_restoreWnd;
    if (!hwnd || !IsWindow(hwnd) || !IsIconic(hwnd) ||
        m_restoreTries >= kMaxRestoreTries) {
        if (hwnd && IsWindow(hwnd) && IsIconic(hwnd)) {
            Wh_Log(L"Couldn't restore %p", hwnd);
        }
        StopRestoreCheck();
        return;
    }

    m_restoreTries++;
    Wh_Log(L"%p is still minimized, trying again (%d)", hwnd, m_restoreTries);
    SendDummyKeyPress();
    if (m_restoreTries == 1) {
        PostMessageW(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    } else {
        ShowWindowAsync(hwnd, SW_RESTORE);
    }

    HWND popup = GetLastActivePopup(hwnd);
    if (!popup || !IsWindowVisible(popup) || !IsWindowEnabled(popup)) {
        popup = hwnd;
    }
    SetForegroundWindow(popup);
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

        case AnimationStyle::Cube:
            // Each window is a face of a cube turning around its vertical
            // axis. All faces are as wide as the cube, so the edges meet.
            m_boxWidth = m_width * 0.40f;
            m_boxHeight = m_height * 0.46f;
            m_frontX = 0;
            m_frontY = -m_height * 0.02f;
            m_stepX = m_boxWidth / 2;  // Half the cube's side.
            m_stepY = 0;
            m_stepZ = 3.14159265f / 2;  // Angle per window.
            break;

        case AnimationStyle::Sphere: {
            // Windows on a spiral around a ball, from the selected one in
            // the middle of its face. Each window is a step further around
            // and a little lower; one turn later, the windows are a step
            // lower, in the next band. Steps get smaller with more windows,
            // so all of them fit on the ball once, from top to bottom.
            m_boxWidth = m_width * 0.25f;
            m_boxHeight = m_height * 0.30f;
            m_frontX = 0;
            m_frontY = -m_height * 0.01f;
            m_stepX = m_height * 0.40f * spacing;  // Radius.
            // Longitude per window, at most about a window's width. Over
            // count windows, the bands then reach count * step^2 / (4 pi)
            // up and down, which stays within kMaxLatitude.
            constexpr float kMaxLatitude = 1.15f;
            m_stepZ = std::min(
                std::clamp(m_boxWidth * 0.95f / m_stepX, 0.3f, 1.0f),
                2 * std::sqrt(3.14159265f * kMaxLatitude / count));
            // Latitude per window: one step per turn.
            m_stepY = m_stepZ * m_stepZ / (2 * 3.14159265f);
            break;
        }

        case AnimationStyle::Shuffle:
            // A deck of cards: the front one is lifted over the deck and
            // slid in at the back.
            m_boxWidth = m_width * 0.46f;
            m_boxHeight = m_height * 0.48f;
            m_frontX = 0;
            m_frontY = m_height * 0.12f;
            m_stepX = 0;
            m_stepY = -m_height * 0.02f * density;
            m_stepZ = m_width * 0.035f * density;
            break;

        case AnimationStyle::Domino:
            // Windows standing in a row: the front one falls over.
            m_boxWidth = m_width * 0.30f;
            m_boxHeight = m_height * 0.42f;
            m_frontX = -m_width * 0.16f;
            m_frontY = m_height * 0.06f;
            m_stepX = m_width * 0.10f * density;
            m_stepY = 0;
            m_stepZ = m_width * 0.15f * density;
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
            // The window size setting only resizes the window inside its
            // cell, so the grid always fits on the screen.
            const float cellFit = std::min(m_boxWidth * 0.88f / item.Width(),
                                           m_boxHeight * 0.84f / item.Height());
            pose.scale = std::min(std::min(cellFit, 1.0f) *
                                      m_settings.windowSize,
                                  cellFit) *
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

        case AnimationStyle::Cube: {
            // Faces turn around the cube's center; only the front one and,
            // while turning, the one coming in are seen.
            const float angle = rel * m_stepZ;
            const float half = m_stepX;
            // Whatever the window's shape: as wide as the cube, and windows
            // taller than the cube are squeezed to its height.
            pose.scale = 2 * half / item.Width();
            pose.squeeze =
                std::min(1.0f, m_boxHeight / (item.Height() * pose.scale));
            pose.x = half * std::sin(angle);
            pose.y = m_frontY;
            pose.z = half * (1 - std::cos(angle)) + half * 0.35f;
            pose.angleY = angle;
            pose.brightness = 0.45f + 0.55f * std::max(std::cos(angle), 0.0f);
            pose.opacity = std::clamp((1 - distance) * 30, 0.0f, 1.0f);
            break;
        }

        case AnimationStyle::Sphere: {
            // See ComputeLayout. Earlier windows go left and up, later ones
            // right and down. Only the front of the ball is shown.
            const float selected = std::max(0.0f, 1 - distance);
            const float latitude = -rel * m_stepY;
            const float longitude = rel * m_stepZ;
            const float facing = std::cos(longitude) * std::cos(latitude);
            pose.x = m_stepX * std::sin(longitude) * std::cos(latitude);
            pose.y = m_frontY - m_stepX * std::sin(latitude);
            pose.z = m_stepX * (1 - facing) - selected * m_stepX * 0.2f;
            pose.angleY = longitude;
            pose.angleX = latitude;
            // Small enough for a step between neighbours, both along a band
            // and between bands, and smaller towards the top and bottom,
            // where the bands are shorter.
            const float cell = m_stepX * m_stepZ;
            pose.scale = std::min({fit, cell * 0.92f / item.Width(),
                                   cell * 0.85f / item.Height()}) *
                         std::max(std::cos(latitude), 0.5f) *
                         (1 + selected * 0.25f);
            pose.brightness = 0.45f + 0.55f * std::max(facing, 0.0f);
            pose.highlight = selected;
            pose.opacity = std::clamp((facing - 0.3f) * 4, 0.0f, 1.0f);
            // Also faded out at both ends of the spiral, where a window
            // jumps from one end to the other. Not with a few windows: the
            // ends are then next to the selected one.
            if (m_items.size() > 4) {
                const float ends = m_items.size() / 2.0f - distance;
                pose.opacity *= std::clamp(ends * 1.5f, 0.0f, 1.0f);
            }
            break;
        }

        case AnimationStyle::Shuffle: {
            const int count = (int)m_items.size();
            if (rel >= 0) {
                // A slightly untidy deck.
                pose.x = m_frontX;
                pose.y = m_frontY + rel * m_stepY;
                pose.z = rel * m_stepZ;
                pose.angleZ = ((index % 3) - 1) * 0.035f * std::min(rel, 1.0f);
                pose.brightness = 1 - std::min(rel * 0.06f, 0.5f);
            } else {
                // Leaving the front: lifted over the deck in an arc, then
                // slid in at the back.
                const float progress = -rel;
                const float arc = std::sin(progress * 3.14159265f);
                const float back = (float)std::max(count - 1, 1);
                pose.x = m_frontX + arc * m_width * 0.16f;
                pose.y = m_frontY + progress * back * m_stepY -
                         arc * m_height * 0.62f;
                pose.z = progress * back * m_stepZ;
                pose.angleX = arc * 0.8f;
                pose.angleZ = -arc * 0.35f;
                pose.brightness = 1 - std::min(progress * back * 0.06f, 0.5f);
            }
            break;
        }

        case AnimationStyle::Domino: {
            const float halfHeight = item.Height() * fit / 2;
            if (rel >= 0) {
                pose.x = m_frontX + rel * m_stepX;
                pose.y = m_frontY;
                pose.z = rel * m_stepZ;
                pose.angleY = -0.35f;
                pose.brightness = 1 - std::min(rel * 0.08f, 0.55f);
            } else {
                // Leaving the front: falls towards the viewer, around its
                // bottom edge, and fades out.
                const float progress = -rel;
                const float fall = progress * 1.45f;
                pose.x = m_frontX;
                pose.y = m_frontY + halfHeight - halfHeight * std::cos(fall);
                // Unlike x and y, z isn't scaled by the window size later.
                pose.z = -halfHeight * m_settings.windowSize * std::sin(fall);
                pose.angleX = -fall;
                pose.angleY = -0.35f * (1 - progress);
                pose.opacity = 1 - progress * progress;
            }
            break;
        }
    }
    return pose;
}

Quad Switcher::Project(const Pose& pose, float width, float height) const {
    const float halfWidth = width * pose.scale / 2;
    const float halfHeight = height * pose.scale * pose.squeeze / 2;
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
    const int selected = SelectedIndex();

    const bool panel = IsPanelStyle(m_settings.style);
    const double now = NowSeconds();
    const float slide = (float)std::clamp(
        (now - m_slideStart) / kSlideAnimationSeconds, 0.0, 1.0);
    auto add = [&](int index, int chain, float rel, float fade) {
        Item* item = m_items[index].get();
        Pose layout = LayoutPose(*item, index, rel);
        layout.opacity *= fade;
        if (!panel) {
            // Window size: zoom the whole arrangement around the screen
            // center (the grid does it in LayoutPose). Depth: how far back
            // the windows behind go.
            if (m_settings.style != AnimationStyle::Grid) {
                layout.x *= m_settings.windowSize;
                layout.y *= m_settings.windowSize;
                layout.scale *= m_settings.windowSize;
            }
            if (IsSolidStyle(m_settings.style)) {
                layout.z *= m_settings.windowSize;
            } else if (layout.z > 0) {
                layout.z *= m_settings.depth;
            }
        }

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
        if (chain == 0) {
            if (item->slides && slide < 1) {
                entry.pose = LerpPose(item->slideFrom, entry.pose,
                                      EaseOutQuart(slide));
            }
            item->lastPose = entry.pose;
            item->hasLastPose = true;
        }
        entry.order = Lerp(flatOrder, depthOrder, t);
        entry.quad = Project(entry.pose, item->Width(), item->Height());
        if (entry.pose.opacity > 0.003f) {
            m_drawList.push_back(entry);
        }
    };

    const bool stacked = m_settings.style == AnimationStyle::Flip3D ||
                         m_settings.style == AnimationStyle::Cascade ||
                         m_settings.style == AnimationStyle::Tunnel ||
                         m_settings.style == AnimationStyle::Shuffle ||
                         m_settings.style == AnimationStyle::Domino;
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
        } else if (m_settings.style == AnimationStyle::Shuffle) {
            // The card itself travels to the back of the deck.
            add(i, 0, slot - count, 1);
        } else {
            // Wrapping from the front to the back of the stack.
            const float leaving = slot - count;  // In (-1, 0).
            add(i, 0, leaving, 1);
            add(i, 1, slot, -leaving);
        }
    }

    // Closed windows shrink and fade out where they were.
    std::erase_if(m_closingItems, [now](const ClosingItem& closing) {
        return now - closing.start >= kCloseAnimationSeconds;
    });
    for (ClosingItem& closing : m_closingItems) {
        const float progress = (float)std::clamp(
            (now - closing.start) / kCloseAnimationSeconds, 0.0, 1.0);
        Item* item = closing.item.get();
        DrawEntry entry;
        entry.item = item;
        entry.chain = 0;
        entry.pose = item->lastPose;
        entry.pose.scale *= 1 - 0.25f * EaseOutQuart(progress);
        entry.pose.opacity *= (1 - progress) * (1 - progress);
        entry.pose.highlight = 0;
        entry.order = entry.pose.z / (m_width * 0.05f) - 0.01f;
        entry.quad = Project(entry.pose, item->Width(), item->Height());
        m_drawList.push_back(entry);
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
    if (m_cells.empty() || m_items.empty()) {
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

    // Dimmed while its app is closing it.
    const float b =
        entry.pose.brightness * (item.closeRequestedAt ? 0.6f : 1.0f);
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
    const float totalWidth = iconSize + gap + metrics.width;

    float centerX = m_width / 2;
    float centerY = std::round(m_height * 0.91f);
    if (m_settings.titlePosition == TitlePosition::BelowWindow) {
        // Under the selected window, following it as it moves.
        const DrawEntry* entry = nullptr;
        for (const DrawEntry& candidate : m_drawList) {
            if (candidate.item == &item &&
                (!entry || candidate.pose.opacity > entry->pose.opacity)) {
                entry = &candidate;
            }
        }
        if (entry) {
            float minX = entry->quad.p[0].x, maxX = minX;
            float bottom = entry->quad.p[0].y;
            for (const D2D1_POINT_2F& corner : entry->quad.p) {
                minX = std::min(minX, corner.x);
                maxX = std::max(maxX, corner.x);
                bottom = std::max(bottom, corner.y);
            }
            centerX = (minX + maxX) / 2;
            centerY = std::round(std::min(bottom + iconSize / 2 + fontSize,
                                          m_height - fontSize * 1.5f));
        }
    }
    const float margin = std::round(fontSize);
    const float left = std::round(std::clamp(
        centerX - totalWidth / 2, margin,
        std::max(margin, m_width - totalWidth - margin)));

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
    // Under the frame rate limit, a refresh is only waited for until the next
    // frame is due. Frames then come on the refreshes nearest to their time,
    // so on average exactly as often as the limit, whatever the screen's
    // refresh rate. If the compositor can't be waited on (it's restarting),
    // a moment passes instead, so the loop doesn't spin.
    if (m_frameInterval > 0) {
        if (now < m_nextFrameAt - m_refreshInterval / 2) {
            if (FAILED(DwmFlush())) {
                MsgWaitForMultipleObjects(0, nullptr, FALSE, 1, QS_ALLINPUT);
            }
            return;
        }
        // After a long frame, the next one is due a whole interval later.
        m_nextFrameAt =
            std::max(m_nextFrameAt, now - m_frameInterval) + m_frameInterval;
    }
    const double dt = std::clamp(now - m_lastFrame, 0.0, 0.05);
    m_lastFrame = now;

    if (m_state == State::Open && HoldReleased(now) && EndSwitching()) {
        BeginClose(true);
    }
    RemoveClosedItems(now);

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

    // Without new frames taken from it, a capture stops producing them, so
    // windows that aren't kept live cost nothing once they have a picture.
    const int selected = SelectedIndex();
    for (int i = 0; i < (int)m_items.size(); i++) {
        Item& item = *m_items[i];
        const bool live =
            m_settings.liveContent == LiveContent::All ||
            (m_settings.liveContent == LiveContent::Selected && i == selected);
        if (live || !item.content) {
            PollFrames(item);
        }
    }
    BuildDrawList(t);
    UpdateHover();

    m_ctx->SetTarget(m_targetBitmap.get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    const D2D1_RECT_F screen{0, 0, m_width, m_height};
    if (m_background) {
        // Covers the real windows early while opening, and uncovers them only
        // near the end while closing, when the cards are almost in place.
        const float cover = 1 - (1 - t) * (1 - t) * (1 - t);
        m_ctx->DrawBitmap(m_background.get(), screen, cover,
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
    }
    DrawCloseButton(t);
    if (!panel) {
        DrawTitle(t);
    }

    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    if (SUCCEEDED(hr)) {
        hr = m_swapChain->Present(1, 0);
    }
    if (FAILED(hr)) {
        if (hr == D2DERR_RECREATE_TARGET || hr == DXGI_ERROR_DEVICE_REMOVED ||
            hr == DXGI_ERROR_DEVICE_RESET || DeviceRemoved()) {
            Wh_Log(L"Rendering failed: 0x%08X", (UINT)hr);
            HandleDeviceLost();
            return;
        }
        // Anything else only loses this frame; the session goes on.
        if (hr != m_lastRenderError) {
            Wh_Log(L"Frame dropped: 0x%08X", (UINT)hr);
        }
    }
    m_lastRenderError = hr;

    if (m_state == State::Closing && m_open.Done(now)) {
        FinishClose();
    }
}

// Looking an icon up can wait on the window's app (WM_GETICON) or the shell,
// which would delay even a quick Alt+Tab. So the icons are kept across
// sessions, and RefreshIcons checks them after a session.
com_ptr<ID2D1Bitmap1> Switcher::GetIconBitmap(HWND hwnd) {
    if (auto it = m_iconCache.find(hwnd); it != m_iconCache.end()) {
        CachedIcon& entry = it->second;
        if (!entry.bitmap && entry.pixels) {
            // The device changed since: only the bitmap is made again.
            entry.bitmap = CreateIconBitmap(entry.pixels.get());
        }
        return entry.bitmap;
    }

    CachedIcon entry;
    if (HICON icon = GetWindowIcon(hwnd)) {
        entry.pixels = ReadIconPixels(icon);
        if (entry.pixels) {
            entry.source = icon;
        }
    }
    if (!entry.pixels) {
        entry.pixels = ReadAppIconPixels(hwnd);
    }
    if (entry.pixels) {
        entry.bitmap = CreateIconBitmap(entry.pixels.get());
    }
    m_iconCache[hwnd] = entry;
    return entry.bitmap;
}

// Forgets closed windows and picks up icons that changed.
void Switcher::RefreshIcons() {
    std::erase_if(m_iconCache,
                  [](const auto& entry) { return !IsWindow(entry.first); });

    for (auto& [hwnd, entry] : m_iconCache) {
        HICON icon = GetWindowIcon(hwnd);
        if (icon && icon != entry.source) {
            if (auto pixels = ReadIconPixels(icon)) {
                entry = {icon, pixels, CreateIconBitmap(pixels.get())};
            }
        } else if (!icon && !entry.pixels) {
            // Still no window icon; the app may have finished starting.
            entry.pixels = ReadAppIconPixels(hwnd);
            if (entry.pixels) {
                entry.bitmap = CreateIconBitmap(entry.pixels.get());
            }
        }
    }
}

// Copies an image into memory, in the format of the bitmaps. Unlike a bitmap,
// the copy doesn't belong to the device.
com_ptr<IWICBitmap> CopyPixels(IWICImagingFactory* factory,
                               IWICBitmapSource* source) {
    com_ptr<IWICFormatConverter> converter;
    com_ptr<IWICBitmap> pixels;
    if (FAILED(factory->CreateFormatConverter(converter.put())) ||
        FAILED(converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeMedianCut)) ||
        FAILED(factory->CreateBitmapFromSource(
            converter.get(), WICBitmapCacheOnLoad, pixels.put()))) {
        return nullptr;
    }
    return pixels;
}

com_ptr<IWICBitmap> Switcher::ReadIconPixels(HICON icon) {
    com_ptr<IWICBitmap> wicBitmap;
    if (FAILED(m_wicFactory->CreateBitmapFromHICON(icon, wicBitmap.put()))) {
        return nullptr;
    }
    return CopyPixels(m_wicFactory.get(), wicBitmap.get());
}

// Packaged (UWP) apps, hosted in an ApplicationFrameWindow, usually have no
// window icon. Their icon comes from the shell, by the app's AppUserModelID.
com_ptr<IWICBitmap> Switcher::ReadAppIconPixels(HWND hwnd) {
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
    com_ptr<IWICBitmap> pixels;
    if (SUCCEEDED(m_wicFactory->CreateBitmapFromHBITMAP(
            hbitmap, nullptr, WICBitmapUsePremultipliedAlpha,
            wicBitmap.put()))) {
        pixels = CopyPixels(m_wicFactory.get(), wicBitmap.get());
    }
    DeleteObject(hbitmap);
    return pixels;
}

com_ptr<ID2D1Bitmap1> Switcher::CreateIconBitmap(IWICBitmap* pixels) {
    com_ptr<ID2D1Bitmap1> bitmap;
    if (FAILED(m_ctx->CreateBitmapFromWicBitmap(pixels, nullptr,
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

// Decodes a wallpaper image, scaled and placed the way Windows shows it on a
// width x height monitor. Returns null if the file can't be read.
com_ptr<ID2D1Bitmap1> Switcher::DecodeWallpaper(
    const std::wstring& path,
    UINT width,
    UINT height,
    DESKTOP_WALLPAPER_POSITION position,
    D2D1_RECT_F* rect) {
    com_ptr<IWICBitmapDecoder> decoder;
    com_ptr<IWICBitmapFrameDecode> frame;
    UINT imageWidth = 0, imageHeight = 0;
    if (path.empty() ||
        FAILED(m_wicFactory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnDemand, decoder.put())) ||
        FAILED(decoder->GetFrame(0, frame.put())) ||
        FAILED(frame->GetSize(&imageWidth, &imageHeight)) || !imageWidth ||
        !imageHeight) {
        return nullptr;
    }

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
    *rect = {(width - drawWidth) / 2, (height - drawHeight) / 2,
             (width + drawWidth) / 2, (height + drawHeight) / 2};

    com_ptr<IWICBitmapScaler> scaler;
    com_ptr<IWICFormatConverter> converter;
    com_ptr<ID2D1Bitmap1> image;
    const UINT scaledWidth = (UINT)std::max(std::lround(drawWidth), 1L);
    const UINT scaledHeight = (UINT)std::max(std::lround(drawHeight), 1L);
    if (FAILED(m_wicFactory->CreateBitmapScaler(scaler.put())) ||
        FAILED(scaler->Initialize(frame.get(), scaledWidth, scaledHeight,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(m_wicFactory->CreateFormatConverter(converter.put())) ||
        FAILED(converter->Initialize(scaler.get(),
                                     GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeMedianCut)) ||
        FAILED(m_ctx->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                image.put()))) {
        return nullptr;
    }
    return image;
}

ULONGLONG GetFileWriteTime(const std::wstring& path) {
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if (path.empty() || !GetFileAttributesExW(path.c_str(),
                                              GetFileExInfoStandard,
                                              &attributes)) {
        return 0;
    }
    return ((ULONGLONG)attributes.ftLastWriteTime.dwHighDateTime << 32) |
           attributes.ftLastWriteTime.dwLowDateTime;
}

// The copy of the current wallpaper that Windows keeps, as a JPEG. It exists
// even when the original can't be read here, e.g. a format WIC can't decode
// (HEIC, AVIF, JPEG XL), a deleted file, or some Windows Spotlight images.
std::wstring GetTranscodedWallpaperPath() {
    WCHAR path[MAX_PATH];
    const DWORD length = ExpandEnvironmentStringsW(
        L"%APPDATA%\\Microsoft\\Windows\\Themes\\TranscodedWallpaper",
        path, ARRAYSIZE(path));
    if (!length || length > ARRAYSIZE(path)) {
        return {};
    }
    return path;
}

// Whether the desktop background is a solid color, with no picture.
bool IsSolidColorBackground() {
    DWORD type = 0;
    DWORD size = sizeof(type);
    return RegGetValueW(HKEY_CURRENT_USER,
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\"
                        L"Explorer\\Wallpapers",
                        L"BackgroundType", RRF_RT_REG_DWORD, nullptr, &type,
                        &size) == ERROR_SUCCESS &&
           type == 1;
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

    if (path.empty()) {
        WCHAR desktopPath[MAX_PATH] = L"";
        if (SystemParametersInfoW(SPI_GETDESKWALLPAPER, ARRAYSIZE(desktopPath),
                                  desktopPath, 0)) {
            path = desktopPath;
        }
    }
    // Live wallpaper apps (e.g. Wallpaper Engine) mark the background as a
    // solid color, but still set a picture of their wallpaper for other apps.
    // So a picture, when there's one, wins over the solid color.
    const bool solidColor = path.empty() && IsSolidColorBackground();
    const std::wstring transcoded =
        solidColor ? std::wstring() : GetTranscodedWallpaperPath();

    // The wallpaper files are often replaced in place, so include their time.
    const std::wstring key =
        path + L"|" + std::to_wstring(GetFileWriteTime(path)) + L"|" +
        transcoded + L"|" + std::to_wstring(GetFileWriteTime(transcoded)) +
        L"|" + std::to_wstring(width) + L"x" + std::to_wstring(height) + L"|" +
        std::to_wstring(color) + L"|" + std::to_wstring((int)position) + L"|" +
        std::to_wstring((int)m_settings.background) + L"|" +
        std::to_wstring(m_settings.blurAmount);
    if (key == m_backgroundKey) {
        return;
    }
    m_background = nullptr;
    m_backgroundKey = key;

    // Decode the image, scaled to the size it's displayed at.
    com_ptr<ID2D1Bitmap1> image;
    D2D1_RECT_F imageRect{};
    if (!solidColor) {
        image = DecodeWallpaper(path, width, height, position, &imageRect);
        if (!image) {
            image = DecodeWallpaper(transcoded, width, height, position,
                                    &imageRect);
            Wh_Log(L"Wallpaper \"%s\" can't be read, %s", path.c_str(),
                   image ? L"using Windows' copy of it"
                         : L"and Windows' copy can't either");
        }
        if (!image) {
            // Better than a black screen: the desktop shows through, dimmed,
            // like with the "Dim only" background.
            return;
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
