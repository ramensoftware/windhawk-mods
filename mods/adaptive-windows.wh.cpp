// ==WindhawkMod==
// @id              adaptive-windows
// @name            Adaptive Windows
// @name:pt-BR      Adaptive Windows
// @description     Automatic window grid with fluid animations (Nava OS style), animated focus border, and compatible with minimize/restore animation mods.
// @description:pt-BR Grade automática de janelas com animações fluidas (estilo Nava OS), borda animada e compatível com mods de minimizar/restaurar.
// @version         1.0.0
// @author          Thiago Silva
// @github          https://github.com/brideinflame-rgb
// @include         windhawk.exe
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lshell32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Adaptive Windows

*(Português do Brasil logo abaixo / Brazilian Portuguese below)*

![Adaptive Windows demo](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/overview.gif)

## English

An automatic tiling window manager for Windows 11, inspired by the **Nava OS** concept videos
and by four tools: **tacky-borders** (focus border), **komorebi** (tiling), **AltSnap** (moving
and resizing windows with Alt + mouse) and **Loop** (radial menu). Here you get all of it in one
place. It arranges the windows of each monitor in a grid, animates every change with fluid transitions,
and works together with minimize / restore / open animation mods: a window is animated
**straight to the new place assigned by the grid**, not to where it used to be.

The mod runs in its own `windhawk.exe` process (a Windhawk "tool mod"), not inside Explorer, so a
problem in it cannot take down the taskbar or File Explorer.

### Features
- **Automatic grid.** A new window takes a slot and the others make room for it; when a window
  closes or is minimized, the rest take over the free space.
- **Respects each app's minimum size.** The grid reshapes itself so no window ends up smaller than
  its app allows or overlaps another one (for example Outlook and Spotify together on a short
  screen). If a chosen layout does not fit, the automatic layout is used.
- **Free resizing.** Drag a window edge and the grid adapts to the width or height you chose: the
  other windows give way while you drag.
  Dragging a window by its title and dropping it over another one swaps the two; dropping it
  anywhere else sends it back to its slot.
- **Move windows with the keyboard.** Win + Alt + arrows swap the focused window with its neighbor
  on that side.
- **Move windows with the mouse (radial menu, optional).** Hold the right button (or another one
  you choose) over a window and a ring appears at the cursor. Move the mouse towards a side: the
  slot of the neighbor on that side is highlighted. Release and the two windows swap places.
- **Maximized windows** that return to or enter the grid go back to normal mode and take their slot.
  Turn on *Keep maximized windows maximized* to leave them alone instead: they stay maximized,
  outside the grid, and take their slot again when they stop being maximized. A maximized window
  never gets the drawn border, and the drawn border and its glow are clipped to the window's
  monitor, so they do not spill onto the monitor next to it.
- **Animated border on the focused window**: color, effect (solid, pulse, rainbow, gradient), line
  width, glow, corner radius and offset, so it can match your rounded corners mod.
- **Multiple monitors.** Each monitor has its own grid, and windows can be dragged between them,
  including monitors with different display scales. It works within the limits each window or app
  imposes: the minimum size is decided by the app (and grows with the monitor's scale), so an app
  whose minimum is wider than the monitor will overflow its right / bottom edge.
- **Display scale aware.** The gap, the screen-edge margin and the border sizes (width, glow, offset,
  corner radius) are set in pixels at 100% scale and scaled for each monitor, so they look the same
  on a 100% and on a 200% monitor.
- **Virtual desktops.** When you switch desktops, the windows of the desktop you come back to return
  to the same slots and order, without replaying the entrance animation.

### Free resizing: when it works
![Resizing with Alt + right button](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/mouse-resize.gif)

Dragging a window border adapts the grid only when all of these are true:
- The window is in the grid: not taken out with the float hotkey, not maximized and not minimized.
- There are at least two windows in the grid on the same monitor.
- You drag an edge that faces another window. An edge against the screen border has no neighbor to
  take space from, so dragging it does nothing and the window goes back to its slot.
- You hold the window by its border (within about 12 px of the left, right or bottom edge, or the
  top 5 px) and change its size by more than 6 px. Holding it by the middle or the title bar moves
  the window instead (drop it over another window to swap, anywhere else to send it back).

You do not have to aim at the border: with *Resize a window with Alt + right button* on, hold Alt
and the right button anywhere over the window and drag sideways or up/down. The side nearest to the
cursor follows the mouse (the right side if you hold the right half of the window, and so on):
drag outward to enlarge the window, inward to shrink it. The other windows move out of the way
while you drag, so nothing overlaps.
A side against the screen border has no neighbor, so the opposite side is used instead; if neither
side has a neighbor, the window goes back to its slot. Esc cancels. AltSnap / AltDrag use the same
Alt + right button gesture: if you run one of them too, change the gesture there or leave this
option off, otherwise windows of the grid are resized by this mod and the other windows by them.

In the "main left" / "main top" layouts, dragging the divider of the main window changes how much of
the screen it takes. Each app's minimum size still applies. Your sizes are discarded when a window
is opened, closed or minimized, when the layout changes and when you re-arrange everything
(Win + Alt + Home).

### Hotkeys
![Moving windows with the keyboard](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/keyboard-move.gif)

![Layouts](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/layouts.gif)

| Hotkey | Action |
|--------|--------|
| Win + Ctrl + G | Cycle the layout: auto, columns, rows, grid, main left, main top |
| Win + Ctrl + T | Take the focused window out of the grid (it stays where it is and the others use the space); press again to put it back |
| Win + Alt + F | Focus mode: every window on the monitor fills the screen; press again to leave |
| Win + Alt + ← / → | Move the focused window left / right: swaps it with the window next to it on that side, whatever the layout (does nothing if there is none) |
| Win + Alt + ↑ / ↓ | Move the focused window up / down: swaps it with the window directly above / below it (does nothing if there is none) |
| Win + Alt + Home | Re-arrange everything and put all windows back in the grid |

Every hotkey can be changed in the settings, and the change applies as soon as you save them:
- **Layout, float and re-arrange** have their own text fields (for example `Ctrl+Alt+G`, `Win+T` or
  `Alt+Home`). Write modifiers (Ctrl, Alt, Shift, Win) and one key: a letter, a digit, or Up, Down,
  Left, Right, Home or End. Leave a field empty to turn that hotkey off.
- **Focus mode and the four arrows** share one modifier choice: Win+Alt (default), Ctrl+Alt,
  Ctrl+Alt+Shift or Alt+Shift.
- Avoid combinations without Win or Ctrl: they take the key away from every other app.

Hotkeys that cannot be registered are reported in the Windhawk log (error 1409 = already in use).
Win + Alt + ↑ is one of them unless you turn on *Take over hotkeys* (see below).
Win + Ctrl + T is also the default Always On Top shortcut of PowerToys: if you use both, change one
of them.

#### Taking over shortcuts that are already in use
Windows itself uses some of these combinations: Win + Alt + ↑ snaps the window to the top half of the
screen, so with the default settings it is unavailable to the mod (the log reports error 1409) and
Windows keeps handling it. The mod never overrides another program's shortcut unless you ask it to.
To use Win + Alt + ↑, turn on the *Take over hotkeys* option (off by default) or pick another set
of modifiers for the arrows. With the option on, whenever a hotkey is already taken the mod
catches that combination with a low-level keyboard hook, so the other program or Windows no longer
receives it. The hook runs in a thread of its own, exists only while the mod runs, and only for the
combinations that were already taken.

### Radial menu: moving a window with the mouse
![Radial menu](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/radial-menu-glass.gif)

Off by default. In the settings, choose which button opens it: the right button, the middle button,
or Alt / Win + the left button. Then press and hold that button over a window of the grid. A ring
appears where the cursor is (after a brief hold, so a quick click does not flash it). Move the mouse out of the ring to the left, right, up or down: that
slice of the ring lights up and the slot the window will go to is highlighted. Release the button
to swap the window with the one in that slot. Esc cancels. If there is no window on that side, the
slice turns gray and nothing happens. It only considers windows on the same monitor.

The ring has two themes: **Classic** (a flat ring) and **Glass** (a glass tube showing the blurred
screen behind it, with highlights). The selection color paints the chosen slice
and the highlight over the window that will receive the one you are moving. By default the colors
are the focused window border ones, following its effect (pulse, rainbow, gradient); choose
*Custom* to set the ring color and the selection color yourself. The ring thickness can be set too.

If you release without leaving the ring, the click is passed on to the app when the button is
released, so a plain right click still opens the context menu. Over a window of the grid,
dragging with that button no longer works while the option is on; anywhere else (the desktop, the
taskbar, windows outside the grid) the click is not touched at all. It needs a low-level mouse hook, which only exists while this option
(or the Alt + right button resize) is on.

### Works with other animation mods
Adaptive Windows works together with these two mods:

- **MacOS Minimize Animation** (genie effect for minimize, restore and open).

![Adaptive Windows with MacOS Minimize Animation](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/macos-minimize-animation.gif)

- **Windows Animations**.

![Adaptive Windows with Windows Animations](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/windows-animations.gif)

Those mods hide the window and play their own animation. With the *sync with open/close
animations* option on, Adaptive Windows notices this, puts the window in its final slot while it
is hidden, and only animates the other windows making room, at the same time. Each window has a
single animation, so nothing fights.

For the genie to land in the right place, while a window is minimized the mod writes into Windows
the slot it will occupy when it comes back, and keeps that value up to date as the grid changes.
This way the restore animation ends where the window will really be.

Programs you exclude in the other mod (for example Word in the genie mod) are not animated by it;
in that case Adaptive Windows uses its own entrance animation.

### Animations
The **Nava OS** style is the base (strong ease-out curve, new windows slide in from the right).
There are also Smooth (slides in from the top), Snappy (no entrance effect) and Playful (enters from the
bottom with a slight overshoot). While moving, a window keeps a fixed size (like when dragging):
shrinking windows shrink at the start and growing ones only grow at the end, so nothing overlaps.
The frame rate can follow the DWM (recommended), follow the highest refresh rate among your
monitors, or be fixed (30 to 240 Hz).

### Taskbar and dock
The usable area is the Windows work area. The screen-edge margin applies to all four sides. If the
taskbar is hidden, windows only respect the screen edge. The bottom spacing setting (0 = automatic)
is meant for docks drawn by another mod: enter the distance from the screen edge to the top of
the dock. The taskbar can be on any edge of any monitor: the grid follows the Windows work
area, and is re-computed when a monitor is connected or removed, the resolution changes, or the
taskbar is moved or toggles auto-hide.

### Corners
Uses the Windows 11 corner preference (default, rounded, small, square). Windows does not allow an
arbitrary radius this way. If you already use a corner mod (for example "Custom Window Corner
Radius"), leave this option on "do not touch" so the two do not fight over the corners.

### Ignored windows and limits
- Settings apply as soon as you save them; there is no need to restart the mod. Changing the
  **apps that always stay out of the grid** list also takes windows out of the grid (or brings them in)
  right away.
- To take one window out of the grid for now, use the float hotkey (Win + Ctrl + T). To keep an
  app out of the grid always, write its name in that list (for example `spotify;chrome`). A name
  works if it is the program's name or if it appears in the window title. The list is empty by
  default.
- Tool windows, always-on-top, non-focusable and non-resizable windows (panels, widgets) always
  stay out of the grid. The Windhawk log says why each window was ignored.
- Windows that refuse to be positioned (a likely case is programs running as administrator) float
  after a few attempts; Win + Alt + Home tries again.
- Moving windows with the keyboard only considers windows on the same monitor.

---

## Português (Brasil)

Gerenciador de janelas em grade automática para o Windows 11, inspirado nos vídeos de conceito
do **Nava OS** e em quatro ferramentas: **tacky-borders** (borda na janela em foco), **komorebi**
(grade de janelas), **AltSnap** (mover e redimensionar janelas com Alt + mouse) e **Loop** (menu
radial). Aqui você tem tudo isso em um único lugar. Ele organiza as janelas de cada monitor, anima as mudanças com transições fluidas
e funciona junto com os mods de animação de minimizar, restaurar e abrir janelas: a janela
é animada **já para o novo lugar definido pela grade**, e não para onde ela estava antes.

O mod roda em um processo `windhawk.exe` próprio (um "tool mod" do Windhawk), e não dentro do
Explorer, então um problema nele não derruba a barra de tarefas nem o Explorador de Arquivos.

### O que ele faz
- **Grade automática.** Cada janela nova ocupa um espaço da grade e as outras abrem espaço para
  ela; ao fechar ou minimizar uma janela, as demais ocupam o espaço livre.
- **Respeita o tamanho mínimo de cada app.** O formato da grade se ajusta sozinho para que nenhuma
  janela fique menor do que o app aceita nem sobreponha outra (por exemplo, Outlook e Spotify
  juntos numa tela de pouca altura). Se um layout escolhido não couber, ele usa o automático.
- **Redimensionamento livre.** Arraste a borda de uma janela e a grade se adapta à largura ou à
  altura que você escolheu: as outras janelas abrem espaço enquanto você arrasta. Arrastar uma janela pelo meio e soltá-la sobre outra troca as duas
  de lugar; soltá-la em qualquer outro ponto a devolve ao seu espaço.
- **Mover janelas pelo teclado.** Win + Alt + setas trocam a janela em foco com a vizinha daquele
  lado.
- **Mover janelas com o mouse (menu radial, opcional).** Segure o botão direito (ou outro que você
  escolher) sobre uma janela e um anel aparece no cursor. Mova o mouse para um lado: o espaço da
  vizinha daquele lado é destacado. Solte e as duas janelas trocam de lugar.
- **Janelas maximizadas** que voltam ou entram na grade passam ao modo normal e assumem o espaço.
  Ligue *Manter janelas maximizadas* para deixá-las em paz: elas continuam maximizadas, fora da
  grade, e assumem o espaço de novo quando deixam de estar maximizadas. Uma janela maximizada
  nunca recebe a borda desenhada, e a borda desenhada e o brilho são recortados no monitor da
  janela, para não passarem para o monitor ao lado.
- **Borda animada na janela em foco**: cor, efeito (fixa, pulsar, arco-íris, degradê), largura da
  linha, brilho, raio dos cantos e deslocamento, para alinhar com o seu mod de cantos arredondados.
- **Vários monitores.** Cada monitor tem a sua própria grade, e as janelas podem ser arrastadas
  entre eles, inclusive entre monitores de escalas diferentes. Funciona dentro dos limites
  impostos por cada janela ou app: o tamanho mínimo é decidido pelo app (e cresce com a escala do
  monitor), então um app cujo mínimo é maior que a largura do monitor vai passar da borda direita
  / de baixo.
- **Respeita a escala de cada monitor.** O espaço entre janelas, a margem da borda da tela e as
  medidas da borda (largura, brilho, deslocamento, raio dos cantos) são configurados em pixels a
  100% de escala e ajustados a cada monitor, então ficam iguais num monitor de 100% e num de 200%.
- **Áreas de trabalho virtuais.** Ao trocar de área, as janelas da área para a qual você volta
  retomam os mesmos espaços e a mesma ordem, sem repetir a animação de entrada.

### Redimensionamento livre: quando funciona
![Redimensionar com Alt + botão direito](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/mouse-resize.gif)

Arrastar a borda de uma janela adapta a grade apenas quando todas estas condições são verdadeiras:
- A janela está na grade: não foi tirada com o atalho de soltar, nem está maximizada ou minimizada.
- Há pelo menos duas janelas na grade no mesmo monitor.
- Você arrasta uma borda que encosta em outra janela. Uma borda junto da borda da tela não tem
  vizinha para ceder espaço, então arrastá-la não faz nada e a janela volta ao seu espaço.
- Você segura a janela pela borda (a cerca de 12 px da lateral esquerda, direita ou de baixo, ou
  nos 5 px de cima) e muda o tamanho em mais de 6 px. Segurar pelo meio ou pela barra de título
  move a janela (solte sobre outra para trocar, ou em qualquer outro ponto para devolvê-la).

Não é preciso mirar na borda: com *Redimensionar a janela com Alt + botão direito* ligado, segure
Alt e o botão direito em qualquer ponto da janela e arraste para os lados ou para cima/baixo. A
lateral mais próxima do cursor acompanha o mouse (a direita, se você segurar a metade direita da
janela, e assim por diante): arraste para fora para aumentar a janela, para dentro para diminuir.
As outras janelas abrem espaço enquanto você arrasta, sem sobreposição. Uma lateral junto da borda da tela não tem vizinha,
então vale a lateral oposta; se nenhuma das duas tiver vizinha, a janela volta ao seu espaço. Esc
cancela. O AltSnap / AltDrag usam o mesmo gesto de Alt + botão direito: se você também usa um deles,
troque o gesto lá ou deixe esta opção desligada; senão as janelas da grade são redimensionadas por
este mod e as demais por eles.

Nos layouts "principal à esquerda" / "principal em cima", arrastar a divisória da janela principal
muda a parte da tela que ela ocupa. O tamanho mínimo de cada app continua valendo. Os tamanhos
escolhidos são descartados quando uma janela é aberta, fechada ou minimizada, quando o layout muda
e quando você reorganiza tudo (Win + Alt + Home).

### Atalhos
![Mover janelas pelo teclado](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/keyboard-move.gif)

![Layouts](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/layouts.gif)

| Atalho | Ação |
|--------|------|
| Win + Ctrl + G | Troca o layout: automático, colunas, linhas, grade, principal à esquerda, principal em cima |
| Win + Ctrl + T | Tira a janela em foco da grade (ela fica onde está e as outras ocupam o espaço); apertar de novo devolve à grade |
| Win + Alt + F | Modo foco: todas as janelas do monitor ocupam a tela toda; apertar de novo sai do modo |
| Win + Alt + ← / → | Move a janela em foco para a esquerda / direita: troca de lugar com a janela vizinha daquele lado, qualquer que seja o layout (não faz nada se não houver) |
| Win + Alt + ↑ / ↓ | Sobe / desce a janela em foco: troca de lugar com a janela logo acima / abaixo dela (não faz nada se não houver) |
| Win + Alt + Home | Reorganiza tudo e devolve todas as janelas à grade |

Todos os atalhos podem ser trocados nas configurações, e a mudança vale assim que você salva:
- **Layout, soltar/prender e reorganizar** têm campos de texto próprios (por exemplo `Ctrl+Alt+G`,
  `Win+T` ou `Alt+Home`). Escreva os modificadores (Ctrl, Alt, Shift, Win) e uma tecla: uma letra,
  um dígito, ou Up, Down, Left, Right, Home ou End. Deixe o campo vazio para desligar o atalho.
- **Modo foco e as quatro setas** compartilham uma única escolha de modificadores: Win+Alt
  (padrão), Ctrl+Alt, Ctrl+Alt+Shift ou Alt+Shift.
- Evite combinações sem Win ou Ctrl: elas tiram a tecla de todos os outros apps.

Atalhos que não puderem ser registrados aparecem no log do Windhawk (erro 1409 = já em uso).
Win + Alt + ↑ é um deles, a menos que você ligue *Assumir atalhos* (veja abaixo).
Win + Ctrl + T também é o atalho padrão do Always On Top do PowerToys: se você usa os dois, troque
um deles.

#### Assumir atalhos que já estão em uso
O próprio Windows usa algumas dessas combinações: Win + Alt + ↑ encaixa a janela na metade de cima
da tela, então com as configurações padrão ela fica indisponível para o mod (o log mostra o erro
1409) e o Windows continua tratando-a. O mod nunca sobrescreve o atalho de outro programa se você
não pedir. Para usar Win + Alt + ↑, ligue a opção *Assumir atalhos* (desligada por padrão) ou escolha
outros modificadores para as setas. Com a opção ligada, sempre que um atalho já estiver ocupado o
mod captura essa combinação com um gancho de teclado de baixo nível, e o outro programa ou o
Windows deixa de recebê-la. O gancho roda numa thread própria, só existe enquanto o mod está ligado,
e só para as combinações que já estavam ocupadas.

### Menu radial: mover a janela com o mouse
![Menu radial](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/radial-menu-glass.gif)

Desligado por padrão. Nas configurações, escolha o botão que o abre: o botão direito, o do meio, ou
Alt / Win + botão esquerdo. Depois pressione e segure esse botão sobre uma janela da grade. Um anel
aparece onde o cursor está (depois de um instante, para não piscar num clique rápido). Mova o mouse para fora do anel, para a esquerda, direita, cima ou
baixo: aquela fatia do anel acende e o espaço para onde a janela vai é destacado. Solte o botão
para trocar a janela de lugar com a que ocupa aquele espaço. Esc cancela. Se não houver janela
daquele lado, a fatia fica cinza e nada acontece. Só considera janelas do mesmo monitor.

O anel tem dois temas: **Clássico** (anel liso) e **Vidro** (um tubo de vidro que mostra a tela
desfocada atrás dele, com brilhos). A cor da seleção pinta a fatia escolhida e o
destaque sobre a janela que vai receber a que você está movendo. Por padrão as cores são as da
borda da janela em foco, acompanhando o efeito dela (pulsar, arco-íris, degradê); escolha
*Personalizadas* para definir você mesmo a cor do anel e a cor da seleção. A espessura do anel
também pode ser escolhida.

Se você soltar sem sair do anel, o clique é repassado ao app no momento em que o botão é solto,
então um clique direito comum continua abrindo o menu de contexto. Sobre uma janela da grade,
arrastar com esse botão deixa de funcionar enquanto a opção está ligada; em qualquer outro lugar (a
área de trabalho, a barra de tarefas, janelas fora da grade) o clique nem é tocado. Ela precisa de um gancho de mouse de baixo nível, que só
existe enquanto esta opção (ou o redimensionar com Alt + botão direito) está ligada.

### Funciona junto com outros mods de animação
O Adaptive Windows funciona junto com estes dois mods:

- **MacOS Minimize Animation** (efeito genie ao minimizar, restaurar e abrir).

![Adaptive Windows com o MacOS Minimize Animation](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/macos-minimize-animation.gif)

- **Windows Animations**.

![Adaptive Windows com o Windows Animations](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/windows-animations.gif)

Esses mods escondem a janela e tocam a animação sozinhos. Com *Sincronizar com animações de
abrir/fechar* ligado, o Adaptive Windows percebe isso, posiciona a janela no espaço final enquanto
ela está escondida e anima só as outras janelas abrindo espaço, ao mesmo tempo. Cada janela tem
uma única animação, sem briga.

Para o genie voltar ao lugar certo, enquanto uma janela está minimizada o mod grava no Windows o
espaço que ela vai ocupar quando voltar e mantém esse valor atualizado conforme a grade muda.
Assim a animação de restaurar termina onde a janela de fato vai ficar.

Programas que você excluir do outro mod (por exemplo, o Word no genie) não são animados por ele;
nesse caso o Adaptive Windows usa a própria animação de entrada.

### Animações
O estilo **Nava OS** é a base (curva de saída forte, janela nova entra deslizando da direita).
Há também os estilos Suave (entra deslizando de cima), Rápido (sem efeito de entrada) e Elástico
(entra de baixo com leve ressalto). Durante o movimento a janela mantém o tamanho (como ao
arrastar): as que diminuem encolhem no início e as que crescem só crescem no final, sem
sobreposição. A taxa de quadros pode seguir o DWM (recomendado), seguir a maior taxa de
atualização entre os monitores, ou ser fixa (de 30 a 240 Hz).

### Barra de tarefas e dock
A área útil é a área de trabalho do Windows. A **Margem da borda da tela** vale para os quatro
lados. Se a barra de tarefas estiver escondida, as janelas respeitam apenas a borda da tela.
**Espaço embaixo** (0 = automático) serve para docks desenhados por outro mod: informe a distância
da borda da tela até o topo do dock. A barra de tarefas pode ficar em qualquer lado de qualquer
monitor: a grade segue a área de trabalho do Windows e é refeita quando um monitor é conectado ou
removido, a resolução muda, ou a barra é movida ou muda a opção de ocultar automaticamente.

### Cantos
Usa a preferência de cantos do próprio Windows 11 (padrão, arredondado, pequeno, quadrado).
O Windows não permite raio arbitrário por esta via. Se você já usa um mod de cantos (por exemplo,
"Custom Window Corner Radius"), deixe esta opção em "Não mexer" para os dois não disputarem
os cantos.

### Janelas ignoradas e limites
- As configurações valem assim que você as salva, sem reiniciar o mod. Mudar a lista de
  **Aplicativos que ficam sempre fora da grade** também tira janelas da grade (ou as traz) na hora.
- Para tirar uma janela da grade por enquanto, use o atalho de soltar (Win + Ctrl + T). Para um
  aplicativo ficar sempre fora da grade, escreva o nome dele nessa lista (por exemplo
  `spotify;chrome`). Um nome vale se for o nome do programa ou se aparecer no título da janela. A
  lista vem vazia por padrão.
- Janelas de ferramenta, sempre no topo, sem foco ou sem moldura redimensionável (por exemplo,
  painéis e widgets) ficam sempre fora da grade. O log do Windhawk diz por que cada janela foi
  ignorada.
- Janelas que não aceitam ser posicionadas (um caso provável são programas executados como
  administrador) passam a flutuar depois de algumas tentativas; Win + Alt + Home tenta de novo.
- Mover janelas pelo teclado só considera janelas do mesmo monitor.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gap: 10
  $name: Gap between windows (px at 100% scale)
  $name:pt-BR: Espaço entre janelas (px a 100% de escala)
  $description: "Scaled for each monitor's display scale, so the gap looks the same on every monitor."
  $description:pt-BR: "Ajustado à escala de cada monitor, então o espaço parece o mesmo em todos."
- padding: 12
  $name: Screen edge margin (px at 100% scale)
  $name:pt-BR: Margem da borda da tela (px a 100% de escala)
  $description: "Scaled for each monitor's display scale."
  $description:pt-BR: "Ajustada à escala de cada monitor."
- marginBottom: 0
  $name: "Bottom spacing (px, from the screen edge)"
  $name:pt-BR: "Espaço embaixo (px, a partir da borda da tela)"
  $description: "0 = automatic: follows the Windows taskbar/dock; if the taskbar is hidden, only the screen edge is respected. Use a value (distance from the screen edge to the top of your dock) only if the dock is drawn by another mod and windows end up far from it or on top of it. The larger of this value and the taskbar applies."
  $description:pt-BR: "0 = automático: acompanha a barra de tarefas/dock do Windows; se ela estiver escondida, só respeita a borda da tela. Use um valor (distância da borda da tela até o topo do seu dock) só se o dock for desenhado por outro mod e as janelas ficarem longe ou por cima dele. Vale o maior entre este valor e a barra de tarefas."
- layout: auto
  $name: Grid layout
  $name:pt-BR: Layout da grade
  $description: "Win+Ctrl+G (default hotkey) switches the layout instantly. If the apps' minimum sizes don't fit, the mod adjusts the shape by itself."
  $description:pt-BR: "Win+Ctrl+G (atalho padrão) troca de layout na hora. Se os tamanhos mínimos dos apps não couberem, o mod ajusta o formato sozinho."
  $options:
  - auto: Automatic (columns up to 3 windows, then grid)
  - columns: Columns side by side
  - rows: Stacked rows
  - grid: Square grid
  - mainleft: Main window on the left, the rest stacked on the right
  - maintop: Main window on top, the rest in columns below
  $options:pt-BR:
  - auto: Automático (colunas até 3 janelas, depois grade)
  - columns: Colunas lado a lado
  - rows: Linhas empilhadas
  - grid: Grade quadrada
  - mainleft: Principal à esquerda, o resto empilhado à direita
  - maintop: Principal em cima, o resto em colunas embaixo
- corners: default
  $name: Corners of arranged windows
  $name:pt-BR: Cantos das janelas organizadas
  $options:
  - default: Don't touch (Windows default)
  - round: Rounded
  - small: Small rounded
  - square: Square
  $options:pt-BR:
  - default: Não mexer (padrão do Windows)
  - round: Arredondado
  - small: Arredondado pequeno
  - square: Quadrado
- preset: nava
  $name: Animation style
  $name:pt-BR: Estilo de animação
  $options:
  - nava: Nava OS (based on the video)
  - smooth: Smooth
  - snappy: Snappy
  - playful: Playful (overshoot)
  $options:pt-BR:
  - nava: Nava OS (base do vídeo)
  - smooth: Suave
  - snappy: Rápido
  - playful: Elástico (overshoot)
- fps: dwm
  $name: Animation frame rate
  $name:pt-BR: Taxa de quadros da animação
  $description: >-
    "Sync with DWM" follows the Windows compositor refresh (recommended).
    Fixed rates use a high-resolution timer: capping the rate (for example 60 Hz
    on a 144 Hz monitor) reduces the load on heavy apps. "Highest monitor rate"
    reads the current refresh rate of each monitor and uses the highest one.
  $description:pt-BR: >-
    "Sincronizar com o DWM" acompanha a atualização do compositor do Windows (recomendado).
    As taxas fixas usam um temporizador de alta resolução: limitar a taxa (por exemplo 60 Hz
    num monitor de 144 Hz) reduz a carga em apps pesados. "Maior taxa dos monitores" lê a
    taxa de atualização atual de cada monitor e usa a mais alta.
  $options:
  - dwm: Sync with DWM (recommended)
  - auto: Highest refresh rate among the monitors
  - hz30: 30 Hz
  - hz60: 60 Hz
  - hz75: 75 Hz
  - hz90: 90 Hz
  - hz120: 120 Hz
  - hz144: 144 Hz
  - hz165: 165 Hz
  - hz240: 240 Hz
  $options:pt-BR:
  - dwm: Sincronizar com o DWM (recomendado)
  - auto: Maior taxa de atualização entre os monitores
  - hz30: 30 Hz
  - hz60: 60 Hz
  - hz75: 75 Hz
  - hz90: 90 Hz
  - hz120: 120 Hz
  - hz144: 144 Hz
  - hz165: 165 Hz
  - hz240: 240 Hz
- keepMaximized: false
  $name: Keep maximized windows maximized
  $name:pt-BR: Manter janelas maximizadas
  $description: "Off (default): a maximized window is restored and takes its slot in the grid. On: maximized windows are left alone and stay maximized, outside the grid; the others keep their slots."
  $description:pt-BR: "Desligado (padrão): a janela maximizada volta ao normal e assume o seu espaço na grade. Ligado: janelas maximizadas ficam como estão, fora da grade; as outras mantêm seus espaços."
- launchSync: true
  $name: "Sync with open/close animations (e.g. MacOS Minimize Animation, Windows Animations)"
  $name:pt-BR: "Sincronizar com animações de abrir/fechar (ex.: MacOS Minimize Animation, Windows Animations)"
  $description: "If a new window appears hidden by another mod (DWM cloak), it is placed in its final slot right away, without its own animation, while the other windows make room at the same time. This way the other mod's animation already lands in the right place."
  $description:pt-BR: "Se uma janela nova aparece escondida por outro mod (cloak do DWM), ela é posicionada no lugar final na hora, sem animação própria, enquanto as outras abrem espaço ao mesmo tempo. Assim a animação do outro mod acontece já no lugar certo."
- layoutHotkey: "Win+Ctrl+G"
  $name: Hotkey to cycle the layout
  $name:pt-BR: Atalho para trocar de layout
  $description: "Write modifiers and a key, such as Win+Ctrl+G, Ctrl+Alt+L or Win+Shift+G. Accepted modifiers: Ctrl, Alt, Shift, Win. Leave empty to turn it off. If it shows as unavailable in the log (error 1409), another program already uses that combination. Avoid combinations without Win or Ctrl: they take the key away from every other app."
  $description:pt-BR: "Escreva modificadores e tecla, como Win+Ctrl+G, Ctrl+Alt+L ou Win+Shift+G. Modificadores aceitos: Ctrl, Alt, Shift, Win. Deixe vazio para desligar. Se aparecer como indisponível no log (erro 1409), outro programa já usa essa combinação. Evite combinações sem Win ou Ctrl: elas tiram a tecla de todos os outros apps."
- floatHotkey: "Win+Ctrl+T"
  $name: Hotkey to float/re-tile the focused window
  $name:pt-BR: Atalho para soltar/prender a janela em foco
  $description: "Same format as the layout hotkey. Leave empty to turn it off."
  $description:pt-BR: "Mesmo formato do atalho de layout. Deixe vazio para desligar."
- retileHotkey: "Win+Alt+Home"
  $name: Hotkey to re-arrange all windows
  $name:pt-BR: Atalho para reorganizar todas as janelas
  $description: "Re-arranges everything and puts all windows back in the grid. Same format as the other hotkeys (for example Alt+Home or Win+Alt+Home); the key can also be Up, Down, Left, Right, Home or End. Leave empty to turn it off."
  $description:pt-BR: "Reorganiza tudo e devolve todas as janelas à grade. Mesmo formato dos outros atalhos (por exemplo Alt+Home ou Win+Alt+Home); a tecla também pode ser Up, Down, Left, Right, Home ou End. Deixe vazio para desligar."
- radialMouse: off
  $name: Radial menu (move a window with the mouse)
  $name:pt-BR: Menu radial (mover a janela com o mouse)
  $description: "Press and hold the chosen button over a window of the grid: a ring appears at the cursor. Move the mouse left, right, up or down to pick the neighboring window on that side (its slot is highlighted) and release to swap the two. Esc cancels. If you release without choosing a side, the click goes to the app as usual (when the button is released). Outside the windows of the grid the button works normally. Uses a low-level mouse hook, which only exists while this option is on."
  $description:pt-BR: "Pressione e segure o botão escolhido sobre uma janela da grade: um anel aparece no cursor. Mova o mouse para a esquerda, direita, cima ou baixo para escolher a janela vizinha daquele lado (o espaço dela é destacado) e solte para trocar as duas de lugar. Esc cancela. Se soltar sem escolher um lado, o clique vai para o app normalmente (no momento em que o botão é solto). Fora das janelas da grade o botão funciona normalmente. Usa um gancho de mouse de baixo nível, que só existe enquanto esta opção está ligada."
  $options:
  - off: Off
  - right: Hold the right button
  - middle: Hold the middle button
  - altleft: Alt + hold the left button
  - winleft: Win + hold the left button
  $options:pt-BR:
  - off: Desligado
  - right: Segurar o botão direito
  - middle: Segurar o botão do meio
  - altleft: Alt + segurar o botão esquerdo
  - winleft: Win + segurar o botão esquerdo
- radialTheme: classic
  $name: Radial menu ring theme
  $name:pt-BR: Tema do anel do menu radial
  $description: "Glass draws the ring as a glass tube that shows the blurred screen behind it, with highlights and a tinted slice for the selection. The background behind the ring is captured once, when the ring opens."
  $description:pt-BR: "Vidro desenha o anel como um tubo de vidro que mostra a tela desfocada atrás dele, com brilhos e a fatia da seleção tingida. O fundo atrás do anel é capturado uma vez, quando o anel abre."
  $options:
  - classic: Classic (flat ring)
  - glass: Glass
  $options:pt-BR:
  - classic: Clássico (anel liso)
  - glass: Vidro
- radialThickness: 0
  $name: Ring thickness (px at 100% scale, 0 = the theme's own)
  $name:pt-BR: Espessura do anel (px a 100% de escala, 0 = a do tema)
  $description: "0 uses the theme's thickness (10 for classic, 20 for glass). Otherwise from 4 to 48. The ring grows outward; the hole in the middle keeps its size."
  $description:pt-BR: "0 usa a espessura do tema (10 no clássico, 20 no vidro). Senão, de 4 a 48. O anel cresce para fora; o furo do meio mantém o tamanho."
- radialColors: border
  $name: Radial menu colors
  $name:pt-BR: Cores do menu radial
  $description: "The selection color is used for the chosen slice of the ring and for the highlight over the window that will receive the one being moved. With the border colors, the selection follows the focused window border color and its effect (pulse, rainbow, gradient)."
  $description:pt-BR: "A cor da seleção vale para a fatia escolhida do anel e para o destaque sobre a janela que vai receber a que está sendo movida. Com as cores da borda, a seleção acompanha a cor da borda da janela em foco e o efeito dela (pulsar, arco-íris, degradê)."
  $options:
  - border: Same as the focused window border
  - custom: Custom (the two colors below)
  $options:pt-BR:
  - border: As mesmas da borda da janela em foco
  - custom: Personalizadas (as duas cores abaixo)
- radialRingColor: "FFFFFF"
  $name: Custom ring color (RGB hex)
  $name:pt-BR: Cor personalizada do anel (RGB hex)
- radialSelColor: "7B8CFF"
  $name: Custom selection color (RGB hex)
  $name:pt-BR: Cor personalizada da seleção (RGB hex)
  $description: "Also the color of the highlight over the destination window."
  $description:pt-BR: "Também é a cor do destaque sobre a janela de destino."
- resizeMouse: false
  $name: Resize a window with Alt + right button
  $name:pt-BR: Redimensionar a janela com Alt + botão direito
  $description: "Hold Alt and the right button anywhere over a window and drag sideways or up/down: the side of the window nearest to the cursor follows the mouse, so dragging outward enlarges the window and dragging inward shrinks it. The other windows make room as you drag. If that side is against the screen edge, the opposite side is used. Esc cancels. An Alt + right click without dragging still reaches the app. Uses the same low-level mouse hook as the radial menu."
  $description:pt-BR: "Segure Alt e o botão direito em qualquer ponto sobre uma janela e arraste para os lados ou para cima/baixo: a lateral da janela mais próxima do cursor acompanha o mouse, então arrastar para fora aumenta a janela e arrastar para dentro diminui. As outras janelas abrem espaço enquanto você arrasta. Se essa lateral estiver junto da borda da tela, vale a lateral oposta. Esc cancela. Um Alt + clique direito sem arrastar continua chegando ao app. Usa o mesmo gancho de mouse de baixo nível do menu radial."
- hotkeys: winalt
  $name: Modifiers of the other hotkeys
  $name:pt-BR: Modificadores dos atalhos
  $description: "If the hotkeys show as unavailable in the log (error 1409), another program already uses these keys. Change them here."
  $description:pt-BR: "Se os atalhos aparecerem como indisponíveis no log (erro 1409), outro programa já usa essas teclas. Troque aqui."
  $options:
  - winalt: Win+Alt
  - ctrlalt: Ctrl+Alt
  - ctrlaltshift: Ctrl+Alt+Shift
  - altshift: Alt+Shift
- forceHotkeys: false
  $name: Take over hotkeys that Windows or another program already uses (keyboard hook)
  $name:pt-BR: Assumir atalhos que o Windows ou outro programa já usa (gancho de teclado)
  $description: "If a hotkey cannot be registered because it is already in use (error 1409 in the log, which happens with Win+Alt+Up), the mod catches that key combination itself with a low-level keyboard hook and keeps it from the other program. Off by default, so the mod never overrides other programs' shortcuts unless you ask it to."
  $description:pt-BR: "Se um atalho não puder ser registrado porque já está em uso (erro 1409 no log, o que acontece com Win+Alt+Seta para cima), o mod captura essa combinação sozinho com um gancho de teclado e não deixa o outro programa recebê-la. Desligada por padrão, para o mod nunca sobrescrever atalhos de outros programas sem você pedir."
- borderColor: "7B8CFF"
  $name: Focused window border color (RGB hex; empty = off)
  $name:pt-BR: Cor da borda da janela em foco (RGB hex; vazio = desligado)
- borderStyle: static
  $name: Focused window border effect
  $name:pt-BR: Efeito da borda da janela em foco
  $description: "The border is Windows' own (1 px, follows the corners). Pulse, rainbow and gradient animate the color continuously."
  $description:pt-BR: "A borda é a do próprio Windows (1 px, acompanha os cantos). Pulsar, arco-íris e degradê animam a cor continuamente."
  $options:
  - static: Solid
  - pulse: Pulse (breathing light)
  - rainbow: Rainbow
  - gradient: Gradient between two colors
  $options:pt-BR:
  - static: Fixa
  - pulse: Pulsar (luz que respira)
  - rainbow: Arco-íris
  - gradient: Degradê entre duas cores
- borderColor2: "FF5CE1"
  $name: Second gradient color (RGB hex)
  $name:pt-BR: Segunda cor do degradê (RGB hex)
- borderSpeed: 4
  $name: Seconds per border animation cycle
  $name:pt-BR: Segundos por ciclo da animação da borda
- borderWidth: 1
  $name: Border line width (px at 100% scale)
  $name:pt-BR: Largura da linha da borda (px a 100% de escala)
  $description: "1 = native Windows border (thin). Above 1, the mod draws the border on top, outside the window, with the chosen thickness and the effect running around the window. Use up to the value of the gap between windows."
  $description:pt-BR: "1 = borda nativa do Windows (fina). Acima de 1, o mod desenha a borda por cima, fora da janela, com a espessura escolhida e o efeito girando em volta da janela. Use até o valor do espaço entre janelas."
- borderGlow: 0
  $name: Glow around the border (px at 100% scale, 0 = no glow)
  $name:pt-BR: Brilho em volta da borda (px a 100% de escala, 0 = sem brilho)
- borderRadius: 8
  $name: Border corner radius (px at 100% scale)
  $name:pt-BR: Raio dos cantos da borda (px a 100% de escala)
  $description: "Only applies to the drawn border (width above 1, or glow). Match it to the radius of your rounded corners mod (for example 8 is the Windows 11 default; 0 = square corners)."
  $description:pt-BR: "Só vale com a borda desenhada por cima (largura acima de 1 ou brilho). Ajuste para ficar igual ao raio do seu mod de cantos arredondados (por exemplo 8 é o padrão do Windows 11; 0 = cantos retos)."
- borderOffset: 0
  $name: Border offset (px at 100% scale)
  $name:pt-BR: Deslocamento da borda (px a 100% de escala)
  $description: "0 = flush against the window edge, outside. Positive moves it outward; negative pulls it inside the window. Useful to align it with the corners mod."
  $description:pt-BR: "0 = colada na borda da janela, por fora. Positivo afasta para fora; negativo puxa para dentro da janela. Serve para alinhar com o mod de cantos."
- ignoreList: ""
  $name: "Apps that always stay out of the grid"
  $name:pt-BR: "Aplicativos que ficam sempre fora da grade"
  $description: "Write the app names separated by semicolons, for example: spotify;chrome. To take just one window out for now, use the float hotkey (Win+Ctrl+T) instead; this list is for apps you never want in the grid."
  $description:pt-BR: "Escreva os nomes dos aplicativos separados por ponto e vírgula, por exemplo: spotify;chrome. Para tirar só uma janela por enquanto, use o atalho de soltar (Win+Ctrl+T); esta lista é para aplicativos que você nunca quer na grade."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <cstdint>

#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif
#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

// ---------------------------------------------------------------------------
// Configuração
// ---------------------------------------------------------------------------

enum Entry { EN_NONE, EN_RIGHT, EN_UP, EN_DOWN };

struct Bezier { double x1 = 0.22, y1 = 1.0, x2 = 0.36, y2 = 1.0; };

struct Config {
    int gap = 10;
    int padding = 12;
    int mBottom = 0;
    int duration = 450;
    Bezier bez;
    Entry entry = EN_RIGHT;
    int layout = 0;  // 0 auto, 1 colunas, 2 linhas, 3 grade, 4 principal esq., 5 principal em cima
    int hz = 0;    // 0 = sincronizar com o DWM; -1 = maior taxa dos monitores; senão Hz fixo
    bool launchSync = true;
    bool keepMaximized = false;
    int corner = -1;  // -1 = não mexer; senão valor de DWM_WINDOW_CORNER_PREFERENCE
    bool borderOn = true;
    COLORREF borderColor = RGB(0x7B, 0x8C, 0xFF);
    COLORREF borderColor2 = RGB(0xFF, 0x5C, 0xE1);
    int borderStyle = 0;  // 0 fixa, 1 pulsar, 2 arco-íris, 3 degradê
    double borderSpeed = 4.0;
    int borderWidth = 1;
    int borderGlow = 0;
    int borderRadius = 8;
    int borderOffset = 0;
    int radialTheme = 0;       // anel do menu radial: 0 = clássico, 1 = vidro
    bool radialCustom = false; // false = cores da borda; true = cores próprias do anel e da seleção
    COLORREF radialRing = RGB(0xFF, 0xFF, 0xFF);
    COLORREF radialSel = RGB(0x7B, 0x8C, 0xFF);
    int radialThickness = 0;   // espessura do anel em px a 100% de escala; 0 = a do tema
};

static Config g_cfg;
static int g_layoutRt = -1;  // layout escolhido pelo atalho (-1 = usar o das configurações)
static double g_mainFrac = 0;  // fração da janela principal ajustada pelo usuário (0 = padrão)

static std::wstring GetStr(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring out = v;
    Wh_FreeStringSetting(v);
    return out;
}

static std::vector<std::wstring> g_ignore;

static std::wstring Lower(std::wstring v) {
    for (auto& ch : v) ch = (wchar_t)towlower(ch);
    return v;
}

// "Ctrl+Alt+G" -> modificadores + tecla. Vazio ou inválido = false (atalho desligado).
static bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    UINT m = MOD_NOREPEAT, key = 0;
    std::wstring t = Lower(text), tok;
    t += L"+";
    for (wchar_t ch : t) {
        if (ch == L'+') {
            size_t a = tok.find_first_not_of(L" \t"), b = tok.find_last_not_of(L" \t");
            tok = a == std::wstring::npos ? L"" : tok.substr(a, b - a + 1);
            if (tok == L"ctrl" || tok == L"control") m |= MOD_CONTROL;
            else if (tok == L"alt") m |= MOD_ALT;
            else if (tok == L"shift") m |= MOD_SHIFT;
            else if (tok == L"win") m |= MOD_WIN;
            else if (tok == L"up") key = VK_UP;
            else if (tok == L"home") key = VK_HOME;
            else if (tok == L"end") key = VK_END;
            else if (tok == L"down") key = VK_DOWN;
            else if (tok == L"left") key = VK_LEFT;
            else if (tok == L"right") key = VK_RIGHT;
            else if (tok.size() == 1 && iswalnum(tok[0])) key = (UINT)towupper(tok[0]);
            else if (!tok.empty()) return false;
            tok.clear();
        } else tok += ch;
    }
    if (!key || m == MOD_NOREPEAT) return false;  // exige tecla e ao menos um modificador
    *mods = m; *vk = key;
    return true;
}

static void LoadIgnoreList() {
    std::wstring raw = Lower(GetStr(L"ignoreList"));
    std::vector<std::wstring> out;
    size_t i = 0;
    while (i <= raw.size()) {
        size_t j = raw.find_first_of(L";\n", i);
        if (j == std::wstring::npos) j = raw.size();
        std::wstring t = raw.substr(i, j - i);
        size_t a = t.find_first_not_of(L" \t\r"), b = t.find_last_not_of(L" \t\r");
        if (a != std::wstring::npos) out.push_back(t.substr(a, b - a + 1));
        i = j + 1;
    }
    g_ignore = std::move(out);
}

// Nome do arquivo do programa de um processo, em minúsculas (ex.: "chrome.exe"); vazio se não der para ler.
static std::wstring ProcessExeName(DWORD pid) {
    std::wstring name;
    HANDLE p = pid ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid) : nullptr;
    if (!p) return name;
    wchar_t path[MAX_PATH] = L"";
    DWORD len = MAX_PATH;
    if (QueryFullProcessImageNameW(p, 0, path, &len)) {
        const wchar_t* base = wcsrchr(path, L'\\');
        name = Lower(base ? base + 1 : path);
    }
    CloseHandle(p);
    return name;
}

// Programa dono da janela. Apps da Microsoft Store (Configurações, Calculadora...) têm a moldura no
// ApplicationFrameHost.exe e o conteúdo no processo do app: nesse caso vale o processo do conteúdo.
// O resultado fica guardado por janela, porque a consulta é feita a cada evento de janela.
static std::wstring WindowExeName(HWND h) {
    struct Entry { HWND h; DWORD pid; std::wstring name; };
    static std::vector<Entry> cache;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    for (const auto& e : cache)
        if (e.h == h && e.pid == pid && e.name != L"applicationframehost.exe") return e.name;
    std::wstring name = ProcessExeName(pid);
    if (name == L"applicationframehost.exe") {
        HWND core = FindWindowExW(h, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
        DWORD cpid = 0;
        if (core) GetWindowThreadProcessId(core, &cpid);
        if (cpid && cpid != pid) {
            std::wstring inner = ProcessExeName(cpid);
            if (!inner.empty()) name = inner;
        }
    }
    cache.erase(std::remove_if(cache.begin(), cache.end(), [&](const Entry& e) { return e.h == h; }), cache.end());
    if (cache.size() >= 128) cache.erase(cache.begin());
    cache.push_back({h, pid, name});
    return name;
}

// Lista de aplicativos que ficam fora da grade. Um item vale se for o nome do programa (com ou sem ".exe":
// "spotify" = spotify.exe) ou se aparecer no título da janela (ou na classe dela).
static bool IsIgnored(HWND h, PCWSTR cls) {
    if (g_ignore.empty()) return false;
    const std::wstring exe = WindowExeName(h);
    wchar_t title[256] = L"";
    GetWindowTextW(h, title, 256);
    const std::wstring t = Lower(title), c = Lower(cls);
    for (const auto& k : g_ignore) {
        const bool hasExt = k.size() > 4 && k.compare(k.size() - 4, 4, L".exe") == 0;
        if (!exe.empty() && exe == (hasExt ? k : k + L".exe")) return true;
        if (!hasExt && (t.find(k) != std::wstring::npos || c.find(k) != std::wstring::npos)) return true;
    }
    return false;
}

// "#RRGGBB" ou "RRGGBB" -> cor. Retorna false (e não mexe em out) se o texto não tiver 6 dígitos.
static bool ParseHexColor(const std::wstring& text, COLORREF* out) {
    size_t k = 0;
    while (k < text.size() && (text[k] == L'#' || iswspace(text[k]))) k++;
    if (text.size() - k < 6) return false;
    unsigned long v = wcstoul(text.c_str() + k, nullptr, 16);
    *out = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
    return true;
}

static void LoadSettings() {
    LoadIgnoreList();
    Config c;
    c.gap = std::max(0, Wh_GetIntSetting(L"gap"));
    c.padding = std::max(0, Wh_GetIntSetting(L"padding"));
    c.mBottom = std::max(0, Wh_GetIntSetting(L"marginBottom"));

    {
        std::wstring l = GetStr(L"layout");
        c.layout = l == L"columns" ? 1 : l == L"rows" ? 2 : l == L"grid" ? 3 : l == L"mainleft" ? 4 : l == L"maintop" ? 5 : 0;
        g_layoutRt = -1;
    }
    {
        std::wstring f = GetStr(L"fps");
        if (f == L"auto") c.hz = -1;
        else if (f.rfind(L"hz", 0) == 0) c.hz = _wtoi(f.c_str() + 2);
        else c.hz = 0;
        c.launchSync = Wh_GetIntSetting(L"launchSync") != 0;
        c.keepMaximized = Wh_GetIntSetting(L"keepMaximized") != 0;
    }

    std::wstring corners = GetStr(L"corners");
    if (corners == L"round") c.corner = 2;
    else if (corners == L"small") c.corner = 3;
    else if (corners == L"square") c.corner = 1;
    else c.corner = -1;

    // Presets. Os valores do "nava" são ESTIMATIVAS feitas a partir dos vídeos de conceito.
    std::wstring preset = GetStr(L"preset");
    // (a janela mantém o tamanho durante o movimento, então a entrada é sempre um deslize, não uma escala)
    if (preset == L"smooth") {
        c.duration = 300; c.bez = {0.25, 0.1, 0.25, 1.0}; c.entry = EN_UP;
    } else if (preset == L"snappy") {
        c.duration = 180; c.bez = {0.16, 1.0, 0.3, 1.0}; c.entry = EN_NONE;
    } else if (preset == L"playful") {
        c.duration = 420; c.bez = {0.34, 1.56, 0.64, 1.0}; c.entry = EN_DOWN;
    } else {  // nava (padrão)
        c.duration = 450; c.bez = {0.22, 1.0, 0.36, 1.0}; c.entry = EN_RIGHT;
    }

    c.borderOn = ParseHexColor(GetStr(L"borderColor"), &c.borderColor);  // vazio = borda desligada
    {
        std::wstring st = GetStr(L"borderStyle");
        c.borderStyle = st == L"pulse" ? 1 : st == L"rainbow" ? 2 : st == L"gradient" ? 3 : 0;
        c.borderSpeed = std::max(0.5, (double)Wh_GetIntSetting(L"borderSpeed"));
        c.borderWidth = std::min(40, std::max(1, Wh_GetIntSetting(L"borderWidth")));
        c.borderGlow = std::min(60, std::max(0, Wh_GetIntSetting(L"borderGlow")));
        c.borderRadius = std::min(80, std::max(0, Wh_GetIntSetting(L"borderRadius")));
        c.borderOffset = std::min(30, std::max(-30, Wh_GetIntSetting(L"borderOffset")));
        ParseHexColor(GetStr(L"borderColor2"), &c.borderColor2);
    }
    c.radialTheme = GetStr(L"radialTheme") == L"glass" ? 1 : 0;
    c.radialCustom = GetStr(L"radialColors") == L"custom";
    ParseHexColor(GetStr(L"radialRingColor"), &c.radialRing);
    ParseHexColor(GetStr(L"radialSelColor"), &c.radialSel);
    {
        int th = Wh_GetIntSetting(L"radialThickness");
        c.radialThickness = th <= 0 ? 0 : std::min(48, std::max(4, th));
    }
    g_cfg = c;
}

// ---------------------------------------------------------------------------
// Curva cubic-bezier (igual ao CSS)
// ---------------------------------------------------------------------------

static double Ease(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    const Bezier& b = g_cfg.bez;
    auto X = [&](double s) { return 3 * b.x1 * s * (1 - s) * (1 - s) + 3 * b.x2 * s * s * (1 - s) + s * s * s; };
    auto Y = [&](double s) { return 3 * b.y1 * s * (1 - s) * (1 - s) + 3 * b.y2 * s * s * (1 - s) + s * s * s; };
    double lo = 0.0, hi = 1.0, s = t;
    for (int i = 0; i < 28; i++) {
        s = (lo + hi) / 2;
        if (X(s) < t) lo = s; else hi = s;
    }
    return Y(s);
}

// ---------------------------------------------------------------------------
// Estado (tocado apenas pela thread de trabalho)
// ---------------------------------------------------------------------------

struct Item {
    HWND h = nullptr;
    bool floating = false;
    bool fresh = true;  // acabou de entrar: anima vindo da direita
    bool zoomed = false;  // último estado de "maximizada" visto (para detectar quando volta ao normal)
    RECT applied{};  // último retângulo enviado ao app durante a animação (evita chamadas repetidas)
    LONG minW = 0, minH = 0;  // tamanho mínimo da janela (retângulo externo) que o app aceita
    bool minRead = false;
    int minTries = 0;  // consultas ao app já tentadas (a consulta é repetida se o app não respondeu a tempo)
    int learned = 0;
    UINT mDpi = 0;  // DPI em que minW/minH/lW/lH foram medidos (monitores com escala diferente mudam o mínimo em pixels)
    LONG lW = 0, lH = 0;  // mínimo aprendido na conferência (separado do informado pelo app, para poder ser descartado)
    double verifyAt = 0;  // quando conferir se a janela chegou ao tamanho final
    int verifyTries = 0;
    bool snap = false;  // entrou escondida por outro mod: vai direto ao slot final, sem animar
    bool anim = false;
    RECT cur{}, from{}, to{};  // from/to: retângulo inicial e FINAL da janela
    RECT mfrom{}, mto{};       // trajetória de movimento (pode ter tamanho fixo)
    double t0 = 0;
    HMONITOR mon = nullptr;
    int origCorner = 0;
    bool origRead = false;
    int cornerApplied = -1;
    RECT lastInset{};  // diferença entre o retângulo da janela e a área visível (última medida)
    double wx = 1, wy = 1;  // proporção de largura/altura escolhida pelo usuário ao redimensionar
    long seq = 0;  // posição na ordem da grade; g_items fica sempre ordenada por ele (ver SwapItems e OnAdd)
};

static std::vector<Item> g_items;
struct Pend { HWND first; ULONGLONG second; int tries = 0; };
static std::vector<Pend> g_pending;  // janelas novas aguardando "assentar"
static bool g_monocle = false;
static HWND g_dragHwnd = nullptr;  // janela sendo arrastada pelo usuário

// Relógio de alta precisão (GetTickCount64 tem passo de ~15 ms e causa "degraus" na animação).
static double NowMs() {
    static LARGE_INTEGER freq = [] { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f; }();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)freq.QuadPart;
}
static HWND g_borderHwnd = nullptr;

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static std::atomic<bool> g_stop{false};

static const UINT WM_APP_RELOAD = WM_APP + 1;
static const int HK_FLOAT = 1, HK_MONOCLE = 2, HK_LEFT = 3, HK_RIGHT = 4, HK_RETILE = 5, HK_LAYOUT = 6, HK_UP = 7, HK_DOWN = 8, HK_ESC = 9;

static int FindItem(HWND h) {
    for (size_t i = 0; i < g_items.size(); i++)
        if (g_items[i].h == h) return (int)i;
    return -1;
}

// ---------------------------------------------------------------------------
// Janelas gerenciáveis
// ---------------------------------------------------------------------------

static bool AppCloaked(HWND h) {
    DWORD c = 0;
    return SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &c, sizeof(c))) && c == 1;  // DWM_CLOAKED_APP
}

static bool IsManageable(HWND h, bool allowAppCloak = false) {
    if (!IsWindow(h) || !IsWindowVisible(h) || IsIconic(h)) return false;
    if (GetAncestor(h, GA_ROOT) != h || GetWindow(h, GW_OWNER)) return false;

    wchar_t cls[64] = L"";
    GetClassNameW(h, cls, 64);
    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    LONG_PTR st = GetWindowLongPtrW(h, GWL_STYLE);

    // diagnóstico: por que uma janela de navegador/app ficou fora da grade (uma vez por janela e motivo)
    auto why = [&](const wchar_t* motivo) {
        if (GetWindowTextLengthW(h) == 0) return;
        static HWND lastH = nullptr;
        static const wchar_t* lastM = nullptr;
        if (lastH == h && lastM == motivo) return;
        lastH = h; lastM = motivo;
        wchar_t t[80] = L"";
        GetWindowTextW(h, t, 80);
        Wh_Log(L"Ignorada [%s] \"%s\": %s (estilo=%08lX, estilo ex=%08lX)", cls, t, motivo,
               (unsigned long)st, (unsigned long)ex);
    };

    if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST)) { why(L"janela de ferramenta/topmost/sem foco"); return false; }
    // precisa poder ser redimensionada; a barra de título própria (WS_CAPTION) não é exigida,
    // porque navegadores e apps com moldura customizada não a têm
    if (!(st & WS_THICKFRAME)) { why(L"não redimensionável"); return false; }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked &&
        !(allowAppCloak && cloaked == 1)) {
        why(L"escondida (cloaked)");
        return false;
    }

    if (GetWindowTextLengthW(h) == 0) return false;

    static const wchar_t* skip[] = {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"Progman",
                                    L"WorkerW", L"Windows.UI.Core.CoreWindow",
                                    L"TopLevelWindowForOverflowXamlIsland"};
    for (auto sk : skip)
        if (!wcscmp(cls, sk)) return false;

    if (IsIgnored(h, cls)) { why(L"na lista de ignoradas"); return false; }
    return true;
}

// Diferença entre o retângulo "real" da janela e a parte visível (bordas invisíveis do DWM).
static RECT FrameInset(HWND h) {
    RECT w{}, f{};
    GetWindowRect(h, &w);
    if (FAILED(DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &f, sizeof(f))))
        return RECT{0, 0, 0, 0};
    return RECT{f.left - w.left, f.top - w.top, w.right - f.right, w.bottom - f.bottom};
}

// ---------------------------------------------------------------------------
// Borda de foco
// ---------------------------------------------------------------------------

static COLORREF HsvToRgb(double h, double s, double v) {
    h = std::fmod(h, 360.0);
    if (h < 0) h += 360.0;
    double c = v * s, x = c * (1 - std::fabs(std::fmod(h / 60.0, 2.0) - 1)), m = v - c;
    double r = 0, g = 0, b = 0;
    if (h < 60) { r = c; g = x; }
    else if (h < 120) { r = x; g = c; }
    else if (h < 180) { g = c; b = x; }
    else if (h < 240) { g = x; b = c; }
    else if (h < 300) { r = x; b = c; }
    else { r = c; b = x; }
    return RGB((int)std::lround((r + m) * 255), (int)std::lround((g + m) * 255), (int)std::lround((b + m) * 255));
}

static COLORREF Lerp(COLORREF a, COLORREF b, double t) {
    auto ch = [&](int sh) { return (int)std::lround(((a >> sh) & 0xFF) * (1 - t) + ((b >> sh) & 0xFF) * t); };
    return RGB(ch(0), ch(8), ch(16));
}

// cor da borda no instante atual, conforme o efeito escolhido
static COLORREF BorderColorNow() {
    if (g_cfg.borderStyle == 0) return g_cfg.borderColor;
    double ph = std::fmod(NowMs() / (g_cfg.borderSpeed * 1000.0), 1.0);
    switch (g_cfg.borderStyle) {
        case 1: {  // pulsar: vai e volta entre a cor e uma versão escura dela
            double k = 0.5 - 0.5 * std::cos(ph * 6.283185307);
            return Lerp(Lerp(g_cfg.borderColor, RGB(0, 0, 0), 0.7), g_cfg.borderColor, k);
        }
        case 2: return HsvToRgb(ph * 360.0, 0.75, 1.0);
        default: {  // degradê: vai e volta entre as duas cores
            double k = 0.5 - 0.5 * std::cos(ph * 6.283185307);
            return Lerp(g_cfg.borderColor, g_cfg.borderColor2, k);
        }
    }
}


// ---- borda desenhada por cima (largura e brilho personalizados) ----
static HWND g_ov = nullptr;
static HDC g_ovDC = nullptr;
static HBITMAP g_ovBmp = nullptr;
static uint32_t* g_ovBits = nullptr;
static int g_ovW = 0, g_ovH = 0;
static RECT g_ovLast{};
static HWND g_ovLastTarget = nullptr;

// Espaço, margem e borda são configurados em pixels a 100% de escala e ajustados à escala de cada monitor.
// g_scale vale para o monitor cujo layout está sendo calculado agora (definido por quem chama ComputeSlots).
static double g_scale = 1.0;
static double MonScale(HMONITOR m) {
    UINT dx = 96, dy = 96;
    if (m && SUCCEEDED(GetDpiForMonitor(m, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx) return dx / 96.0;
    return 1.0;
}
static int Px(int v) { return (int)std::lround(v * g_scale); }

static bool UseOverlay() { return g_cfg.borderOn && (g_cfg.borderWidth > 1 || g_cfg.borderGlow > 0); }

static void HideOverlay() {
    if (g_ov && IsWindowVisible(g_ov)) ShowWindow(g_ov, SW_HIDE);
    g_ovLastTarget = nullptr;
}

static void DestroyOverlay() {
    if (g_ov) { DestroyWindow(g_ov); g_ov = nullptr; }
    if (g_ovBmp) { DeleteObject(g_ovBmp); g_ovBmp = nullptr; }
    if (g_ovDC) { DeleteDC(g_ovDC); g_ovDC = nullptr; }
    g_ovBits = nullptr;
    g_ovW = g_ovH = 0;
}

static bool EnsureOverlay(int W, int H) {
    if (!g_ov) {
        static bool reg = false;
        if (!reg) {
            WNDCLASSW wc{};
            wc.lpfnWndProc = DefWindowProcW;
            wc.hInstance = GetModuleHandleW(nullptr);
            wc.lpszClassName = L"AdaptiveWindowsBorder";
            RegisterClassW(&wc);
            reg = true;
        }
        g_ov = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                               L"AdaptiveWindowsBorder", L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                               GetModuleHandleW(nullptr), nullptr);
        if (!g_ov) return false;
    }
    if (!g_ovDC) g_ovDC = CreateCompatibleDC(nullptr);
    if (!g_ovBmp || W != g_ovW || H != g_ovH) {
        if (g_ovBmp) { DeleteObject(g_ovBmp); g_ovBmp = nullptr; }
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = W;
        bi.bmiHeader.biHeight = -H;  // de cima para baixo
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        g_ovBmp = CreateDIBSection(g_ovDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!g_ovBmp || !bits) return false;
        g_ovBits = (uint32_t*)bits;  // começa zerado (transparente)
        g_ovW = W; g_ovH = H;
        SelectObject(g_ovDC, g_ovBmp);
    }
    return true;
}

struct CovPx { uint32_t idx; float a; float ang; };
struct CovKey {
    int W, H, fw, fh, w, glow, off, rad;
    bool operator==(const CovKey& o) const {
        return W == o.W && H == o.H && fw == o.fw && fh == o.fh && w == o.w && glow == o.glow && off == o.off && rad == o.rad;
    }
};
static std::vector<CovPx> g_cov;
static CovKey g_covKey{};
static bool g_covValid = false;

static void DrawOverlay(HWND t) {
    DWORD cl = 0;
    // Uma janela maximizada ocupa o monitor inteiro: a borda, que é desenhada por FORA da janela, cairia
    // toda no monitor vizinho. Ela não recebe a borda desenhada.
    if (!t || !IsWindow(t) || !IsWindowVisible(t) || IsIconic(t) || IsZoomed(t) ||
        (SUCCEEDED(DwmGetWindowAttribute(t, DWMWA_CLOAKED, &cl, sizeof(cl))) && cl)) {
        HideOverlay();
        return;
    }
    RECT fr{};
    if (FAILED(DwmGetWindowAttribute(t, DWMWA_EXTENDED_FRAME_BOUNDS, &fr, sizeof(fr)))) GetWindowRect(t, &fr);
    int fw = fr.right - fr.left, fh = fr.bottom - fr.top;
    if (fw < 20 || fh < 20) { HideOverlay(); return; }

    // largura, brilho, deslocamento e raio são configurados a 100% de escala: ajusta à escala do monitor da janela
    const double sc = MonScale(MonitorFromWindow(t, MONITOR_DEFAULTTONEAREST));
    auto sPx = [&](int v) { return (int)std::lround(v * sc); };
    const int w = std::max(1, sPx(g_cfg.borderWidth)), glow = sPx(g_cfg.borderGlow);
    const int off = sPx(g_cfg.borderOffset);
    const int m = w + glow + 2 + std::abs(off);
    const int W = fw + 2 * m, H = fh + 2 * m;

    bool animated = g_cfg.borderStyle != 0;
    bool same = EqualRect(&fr, &g_ovLast) && t == g_ovLastTarget;
    if (same && !animated && IsWindowVisible(g_ov)) return;  // nada mudou
    if (!EnsureOverlay(W, H)) return;
    g_ovLast = fr;
    g_ovLastTarget = t;

    const int rad = sPx(g_cfg.borderRadius);
    const double r = (double)rad;
    double ph = std::fmod(NowMs() / (g_cfg.borderSpeed * 1000.0), 1.0);
    double pulse = 1.0;
    if (g_cfg.borderStyle == 1) pulse = 0.35 + 0.65 * (0.5 - 0.5 * std::cos(ph * 6.283185307));
    const double cx = W / 2.0, cy = H / 2.0, hx = fw / 2.0, hy = fh / 2.0;
    const int sB = m + rad + 3;  // faixas que contêm a borda; o miolo da janela fica transparente

    // A cobertura (transparência) e o ângulo de cada pixel só dependem da geometria; mudam apenas quando o
    // tamanho da janela ou as medidas da borda mudam. Ficam guardados, e a cada quadro só as cores são refeitas.
    const CovKey key{W, H, fw, fh, w, glow, off, rad};
    if (!g_covValid || !(key == g_covKey)) {
        g_covKey = key;
        g_covValid = true;
        g_cov.clear();
        memset(g_ovBits, 0, (size_t)W * H * sizeof(uint32_t));
        auto px = [&](int x, int y) {
            double dx = std::fabs(x + 0.5 - cx) - (hx - r), dy = std::fabs(y + 0.5 - cy) - (hy - r);
            double ox = std::max(dx, 0.0), oy = std::max(dy, 0.0);
            double d = std::sqrt(ox * ox + oy * oy) + std::min(std::max(dx, dy), 0.0) - r - off;  // >0 = fora da linha de referência
            if (d <= -1.0) return;
            double cov1 = std::min(1.0, std::max(0.0, d + 0.5));
            double cov2 = std::min(1.0, std::max(0.0, w - d + 0.5));
            double a = cov1 * cov2;
            if (glow > 0 && d > w) {
                double g = 1.0 - (d - w) / glow;
                if (g > 0) a = std::max(a, 0.45 * g * g * cov1);
            }
            if (a <= 0.002) return;
            double ang = std::atan2(y + 0.5 - cy, x + 0.5 - cx) / 6.283185307 + 0.5;  // 0..1 em volta da janela
            g_cov.push_back({(uint32_t)((size_t)y * W + x), (float)a, (float)ang});
        };
        for (int y = 0; y < H; y++) {
            if (y < sB || y >= H - sB) {
                for (int x = 0; x < W; x++) px(x, y);
            } else {
                for (int x = 0; x < m + 3; x++) px(x, y);
                for (int x = W - m - 3; x < W; x++) px(x, y);
            }
        }
    }
    for (const CovPx& c : g_cov) {
        COLORREF col;
        if (g_cfg.borderStyle == 2) col = HsvToRgb((c.ang + ph) * 360.0, 0.8, 1.0);
        else if (g_cfg.borderStyle == 3) {
            double k = 0.5 - 0.5 * std::cos((c.ang + ph) * 6.283185307 * 2.0);
            col = Lerp(g_cfg.borderColor, g_cfg.borderColor2, k);
        } else col = g_cfg.borderColor;
        uint32_t A = (uint32_t)std::lround(c.a * pulse * 255.0);
        uint32_t R = (GetRValue(col) * A + 127) / 255, G = (GetGValue(col) * A + 127) / 255, B = (GetBValue(col) * A + 127) / 255;
        g_ovBits[c.idx] = (A << 24) | (R << 16) | (G << 8) | B;
    }

    POINT dst{fr.left - m, fr.top - m}, src{0, 0};
    SIZE sz{W, H};
    // A borda e o brilho ficam por fora da janela; junto da borda da tela eles passariam para o monitor
    // vizinho. Recorta o desenho no monitor da janela (menos durante um arrasto, quando a janela pode
    // estar de propósito entre dois monitores).
    MONITORINFO omi{sizeof(omi)};
    if (t != g_dragHwnd && GetMonitorInfoW(MonitorFromWindow(t, MONITOR_DEFAULTTONEAREST), &omi)) {
        RECT full{dst.x, dst.y, dst.x + W, dst.y + H}, clip{};
        if (!IntersectRect(&clip, &full, &omi.rcMonitor)) {
            HideOverlay();
            return;
        }
        src.x = clip.left - full.left;
        src.y = clip.top - full.top;
        dst.x = clip.left;
        dst.y = clip.top;
        sz.cx = clip.right - clip.left;
        sz.cy = clip.bottom - clip.top;
    }
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_ov, nullptr, &dst, &sz, g_ovDC, &src, 0, &bf, ULW_ALPHA);
    // fica logo acima da janela ativa (e não por cima de tudo)
    HWND prev = GetWindow(t, GW_HWNDPREV);
    if (prev != g_ov)
        SetWindowPos(g_ov, prev ? prev : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    else if (!IsWindowVisible(g_ov))
        ShowWindow(g_ov, SW_SHOWNOACTIVATE);
}

static void SetBorder(HWND h, bool on) {
    if (!h || !IsWindow(h)) return;
    if (UseOverlay()) {
        if (!on) {
            COLORREF d = 0xFFFFFFFF;
            DwmSetWindowAttribute(h, DWMWA_BORDER_COLOR, &d, sizeof(d));
        }
        return;  // a borda desenhada por cima é atualizada em TickBorder
    }
    COLORREF c = on ? BorderColorNow() : 0xFFFFFFFF;  // 0xFFFFFFFF = DWMWA_COLOR_DEFAULT
    DwmSetWindowAttribute(h, DWMWA_BORDER_COLOR, &c, sizeof(c));
}

// Only the animated styles (pulse, rainbow, gradient) need a periodic tick. A static border, drawn by
// the mod or by DWM, is updated from events (focus change, EVENT_OBJECT_LOCATIONCHANGE).
static bool BorderAnimated() {
    if (!g_cfg.borderOn || !g_borderHwnd || !IsWindow(g_borderHwnd)) return false;
    return g_cfg.borderStyle != 0;
}

static void TickBorder() {
    if (g_cfg.borderStyle == 0) return;
    static double last = 0;
    double n = NowMs();
    if (n - last < 15) return;  // ~60 atualizações por segundo bastam para uma cor
    last = n;
    if (UseOverlay()) {
        if (g_borderHwnd && IsWindow(g_borderHwnd)) DrawOverlay(g_borderHwnd);
        else HideOverlay();
        return;
    }
    if (g_ov) HideOverlay();
    if (BorderAnimated()) SetBorder(g_borderHwnd, true);
}

static void UpdateBorder(HWND focus) {
    if (g_borderHwnd && g_borderHwnd != focus) SetBorder(g_borderHwnd, false);
    g_borderHwnd = nullptr;
    if (g_ov) HideOverlay();
    if (g_cfg.borderOn && focus && FindItem(focus) >= 0) {
        SetBorder(focus, true);
        g_borderHwnd = focus;
        if (UseOverlay()) DrawOverlay(focus);  // static overlay: no tick, so draw it here
    }
}

// ---------------------------------------------------------------------------
// Layout adaptativo
// ---------------------------------------------------------------------------

// Divide "total" em partes iguais, mas nenhuma parte fica abaixo do mínimo pedido (req).
// Se os mínimos não couberem, as partes que pedem mais ficam com o mínimo e as outras dividem o resto.
static std::vector<double> Distribute(double total, const std::vector<double>& req, const std::vector<double>* wt = nullptr) {
    size_t n = req.size();
    std::vector<double> reqF(req);
    for (auto& v : reqF) v = std::max(v, 120.0);  // nenhuma janela some (largura/altura 0) quando os mínimos dos outros estouram o espaço
    const std::vector<double>& req_ = reqF;
    auto W = [&](size_t i) { return (wt && i < wt->size() && (*wt)[i] > 0) ? (*wt)[i] : 1.0; };
    std::vector<double> h(n, 0.0);
    {
        double sw = 0;
        for (size_t i = 0; i < n; i++) sw += W(i);
        for (size_t i = 0; i < n; i++) h[i] = sw > 0 ? total * W(i) / sw : 0.0;
    }
    std::vector<bool> fixed(n, false);
    for (size_t iter = 0; iter <= n; iter++) {
        double fixedSum = 0;
        size_t freeCnt = 0;
        for (size_t i = 0; i < n; i++) {
            if (fixed[i]) fixedSum += h[i];
            else freeCnt++;
        }
        if (!freeCnt) break;
        double freeW = 0;
        for (size_t i = 0; i < n; i++)
            if (!fixed[i]) freeW += W(i);
        bool changed = false;
        for (size_t i = 0; i < n; i++) {
            if (fixed[i]) continue;
            double share = (total - fixedSum) * W(i) / freeW;
            if (req_[i] > share) { h[i] = req_[i]; fixed[i] = true; changed = true; }
            else h[i] = share;
        }
        if (!changed) break;
    }
    return h;
}

// mins[i] = tamanho mínimo (da área visível) que a i-ésima janela aceita; 0 = sem mínimo.
// Empurra o espaço para dentro da área, mas nunca para fora do lado esquerdo/de cima dela (não invade o
// monitor vizinho); se for maior que a área, o excesso sai pela borda direita/de baixo. Soma o excesso em "over".
static void ClampIntoArea(RECT& r, const RECT& a, double* over) {
    if (r.bottom > a.bottom) {
        LONG d = r.bottom - a.bottom;
        LONG m = std::max<LONG>(0, std::min(d, r.top - a.top));
        r.top -= m;
        r.bottom -= m;
        if (over) *over += d;
    }
    if (r.right > a.right) {
        LONG d = r.right - a.right;
        LONG m = std::max<LONG>(0, std::min(d, r.left - a.left));
        r.left -= m;
        r.right -= m;
        if (over) *over += d;
    }
}

static std::vector<RECT> ComputeSlotsEx(int n, RECT a, const std::vector<SIZE>& mins, int layout, bool pad, double* overflow = nullptr,
                                         const std::vector<std::pair<double, double>>* wts = nullptr) {
    std::vector<RECT> out;
    if (overflow) *overflow = 0;
    if (n <= 0) return out;
    if (pad) {
        const int pd = Px(g_cfg.padding);
        a.left += pd; a.top += pd;
        a.right -= pd; a.bottom -= pd;
    }

    int gap = Px(g_cfg.gap);
    double availW = a.right - a.left, availH = a.bottom - a.top;
    auto WT = [&](int i) { return (wts && i < (int)wts->size()) ? (*wts)[i] : std::pair<double, double>(1.0, 1.0); };

    // Principal + resto: o resto usa o mesmo cálculo de grade (respeitando mínimos), então
    // se não couber empilhado ele se divide em mais colunas/linhas em vez de sobrepor.
    if ((layout == 4 || layout == 5) && n >= 2) {
        bool left = (layout == 4);
        double total = left ? availW : availH;
        double mainReq = mins.empty() ? 0.0 : (left ? mins[0].cx : mins[0].cy);
        std::vector<SIZE> restMins(mins.size() > 1 ? mins.begin() + 1 : mins.end(), mins.end());
        std::vector<RECT> best;
        double bestMain = 0, bestOver = 1e18;
        std::vector<double> fracs = {0.6, 0.5, 0.42, 0.34, 0.28};
        if (g_mainFrac > 0.05 && g_mainFrac < 0.95) fracs.insert(fracs.begin(), g_mainFrac);
        std::vector<std::pair<double, double>> restW;
        if (wts && wts->size() > 1) restW.assign(wts->begin() + 1, wts->end());
        for (double frac : fracs) {
            double mainSz = std::max(mainReq, std::round((total - gap) * frac));
            mainSz = std::min(mainSz, total - gap - 1);
            RECT ra = left ? RECT{(LONG)std::lround(a.left + mainSz + gap), a.top, a.right, a.bottom}
                           : RECT{a.left, (LONG)std::lround(a.top + mainSz + gap), a.right, a.bottom};
            double over = 0;
            auto rest = ComputeSlotsEx(n - 1, ra, restMins, left ? 2 : 1, false, &over, restW.empty() ? nullptr : &restW);
            if (over < bestOver) { bestOver = over; best = rest; bestMain = mainSz; }
            if (over <= 0) break;
        }
        if (bestOver > 0) {
            // principal + resto não cabem juntos (ex.: Outlook 648 + Spotify 715 de altura mínima numa tela de 1250):
            // em vez de sobrepor janelas, usa o layout automático em colunas, que respeita os mínimos
            return ComputeSlotsEx(n, a, mins, 0, false, overflow, wts);
        }
        RECT m = left ? RECT{a.left, a.top, (LONG)std::lround(a.left + bestMain), a.bottom}
                      : RECT{a.left, a.top, a.right, (LONG)std::lround(a.top + bestMain)};
        out.push_back(m);
        for (auto& r : best) out.push_back(r);
        return out;
    }

    // Colunas com pilhas independentes: cada coluna divide a própria altura entre as janelas dela
    // (muito mais fácil de caber do que linhas compartilhadas, porque os mínimos de altura só
    // precisam caber dentro da coluna).
    if (layout != 2) {
        auto groupsOf = [&](int c) {
            std::vector<std::vector<int>> g(c);
            int base = n / c, extra = n % c, k = 0;
            for (int col = 0; col < c; col++) {
                int cnt = base + (col < extra ? 1 : 0);
                for (int j = 0; j < cnt; j++) g[col].push_back(k++);
            }
            return g;
        };
        auto overOf = [&](int c) {
            auto g = groupsOf(c);
            double needW = gap * (c - 1), over = 0;
            for (auto& col : g) {
                double needH = gap * ((double)col.size() - 1), cw = 0;
                for (int i : col)
                    if (i < (int)mins.size()) { needH += mins[i].cy; cw = std::max(cw, (double)mins[i].cx); }
                needW += cw;
                over += std::max(0.0, needH - availH);
            }
            return over + std::max(0.0, needW - availW);
        };
        int dc = (layout == 1) ? n : (layout == 3 ? (int)std::ceil(std::sqrt((double)n))
                                                  : (n <= 3 ? n : (n <= 6 ? (n + 1) / 2 : (int)std::ceil(std::sqrt((double)n)))));
        dc = std::max(1, std::min(n, dc));
        int cols = dc;
        double bestScore = 1e18;
        for (int c = 1; c <= n; c++) {
            double over = overOf(c);
            double score = over * 1000.0 + std::abs(c - dc);
            if (c == dc && over <= 0) { cols = c; break; }
            if (score < bestScore) { bestScore = score; cols = c; }
        }
        auto g = groupsOf(cols);
        std::vector<double> reqW(cols, 0.0), wC(cols, 0.0);
        for (int col = 0; col < cols; col++) {
            for (int i : g[col]) {
                if (i < (int)mins.size()) reqW[col] = std::max(reqW[col], (double)mins[i].cx);
                wC[col] += WT(i).first;
            }
            wC[col] /= std::max<size_t>(1, g[col].size());
        }
        std::vector<double> colW = Distribute(availW - gap * (cols - 1), reqW, &wC);
        out.assign(n, RECT{0, 0, 0, 0});
        double x = a.left;
        for (int col = 0; col < cols; col++) {
            std::vector<double> reqH(g[col].size(), 0.0), wR(g[col].size(), 1.0);
            for (size_t k = 0; k < g[col].size(); k++) {
                int i = g[col][k];
                if (i < (int)mins.size()) reqH[k] = mins[i].cy;
                wR[k] = WT(i).second;
            }
            std::vector<double> hh = Distribute(availH - gap * ((double)g[col].size() - 1), reqH, &wR);
            double y = a.top;
            for (size_t k = 0; k < g[col].size(); k++) {
                RECT r;
                r.left = (LONG)std::lround(x); r.right = (LONG)std::lround(x + colW[col]);
                r.top = (LONG)std::lround(y); r.bottom = (LONG)std::lround(y + hh[k]);
                out[g[col][k]] = r;
                y += hh[k] + gap;
            }
            x += colW[col] + gap;
        }
        for (auto& r : out) {
            ClampIntoArea(r, a, overflow);
        }
        return out;
    }

    int defCols;
    if (layout == 1) defCols = n;
    else if (layout == 2) defCols = 1;
    else if (layout == 3) defCols = (int)std::ceil(std::sqrt((double)n));
    else if (n <= 3) defCols = n;
    else if (n <= 6) defCols = (n + 1) / 2;
    else defCols = (int)std::ceil(std::sqrt((double)n));

    // Escolhe o formato (colunas x linhas): o padrão, se couber respeitando os tamanhos mínimos;
    // senão o formato que cabe mais parecido com o padrão; senão o que sobra menos.
    int cols = defCols;
    {
        double bestScore = 1e18;
        for (int c = 1; c <= n; c++) {
            int rws = (n + c - 1) / c;
            double needH = gap * (rws - 1), needW = 0;
            for (int r = 0; r < rws; r++) {
                double rh = 0, rw = gap * (std::min(c, n - r * c) - 1);
                for (int k = 0; k < c && r * c + k < n; k++) {
                    int i = r * c + k;
                    if (i < (int)mins.size()) { rh = std::max(rh, (double)mins[i].cy); rw += mins[i].cx; }
                }
                needH += rh;
                needW = std::max(needW, rw);
            }
            double over = std::max(0.0, needH - availH) + std::max(0.0, needW - availW);
            double score = over * 1000.0 + std::abs(c - defCols);  // sobra pesa mais que parecer com o padrão
            if (c == defCols && over <= 0) { cols = c; bestScore = -1; break; }
            if (score < bestScore) { bestScore = score; cols = c; }
        }
    }
    int rows = (n + cols - 1) / cols;
    // alturas das linhas respeitando o mínimo de cada janela da linha
    std::vector<double> reqH(rows, 0.0), wRow(rows, 0.0);
    std::vector<int> cRow(rows, 0);
    for (int i = 0; i < n; i++) {
        int r = i / cols;
        if (i < (int)mins.size()) reqH[r] = std::max(reqH[r], (double)mins[i].cy);
        wRow[r] += WT(i).second;
        cRow[r]++;
    }
    for (int r = 0; r < rows; r++) wRow[r] = cRow[r] ? wRow[r] / cRow[r] : 1.0;
    std::vector<double> rowH = Distribute((double)((a.bottom - a.top) - gap * (rows - 1)), reqH, &wRow);

    int placed = 0;
    double y = a.top;
    for (int r = 0; r < rows; r++) {
        int count = std::min(cols, n - placed);
        std::vector<double> reqW(count, 0.0), wCol(count, 1.0);
        for (int c = 0; c < count; c++) {
            if (placed + c < (int)mins.size()) reqW[c] = (double)mins[placed + c].cx;
            wCol[c] = WT(placed + c).first;
        }
        std::vector<double> colW = Distribute((double)((a.right - a.left) - gap * (count - 1)), reqW, &wCol);
        double x = a.left;
        for (int c = 0; c < count; c++) {
            RECT s;
            s.left = (LONG)std::lround(x);
            s.right = (LONG)std::lround(x + colW[c]);
            s.top = (LONG)std::lround(y);
            s.bottom = (LONG)std::lround(y + rowH[r]);
            out.push_back(s);
            x += colW[c] + gap;
        }
        y += rowH[r] + gap;
        placed += count;
    }
    // se os mínimos não couberem, a janela sobrepõe a de cima em vez de ficar fora da área útil
    double localOver = 0;
    for (auto& r : out) {
        ClampIntoArea(r, a, &localOver);
    }
    // "Linhas" compartilha a altura entre janelas da mesma linha e por isso estoura com facilidade
    // (ex.: Outlook 648 + Spotify 715 de altura mínima numa tela de 1250). No layout de nível superior,
    // se não couber, usa o automático em colunas com pilhas, que respeita os mínimos, em vez de sobrepor.
    if (layout == 2 && pad && localOver > 0) return ComputeSlotsEx(n, a, mins, 0, false, overflow, wts);
    if (overflow) *overflow += localOver;
    return out;
}

static std::vector<RECT> ComputeSlots(int n, RECT a, const std::vector<SIZE>& mins,
                                      const std::vector<std::pair<double, double>>* wts = nullptr) {
    return ComputeSlotsEx(n, a, mins, g_layoutRt >= 0 ? g_layoutRt : g_cfg.layout, true, nullptr, wts);
}

// Pergunta ao app o menor tamanho de janela que ele aceita (WM_GETMINMAXINFO).
static void QueryMin(Item& it) {
    if (it.minRead || it.minTries >= 4) return;  // app ocupado: tenta de novo na próxima vez, até 4 vezes
    it.minTries++;
    MINMAXINFO mmi{};
    mmi.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
    mmi.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
    DWORD_PTR res = 0;
    if (SendMessageTimeoutW(it.h, WM_GETMINMAXINFO, 0, (LPARAM)&mmi, SMTO_ABORTIFHUNG | SMTO_BLOCK, 20, &res)) {
        it.minRead = true;
        it.minW = std::max<LONG>(it.minW, mmi.ptMinTrackSize.x);
        it.minH = std::max<LONG>(it.minH, mmi.ptMinTrackSize.y);
        {
            wchar_t cls[64] = L"";
            GetClassNameW(it.h, cls, 64);
            Wh_Log(L"Tamanho minimo [%s]: %ldx%ld", cls, (long)it.minW, (long)it.minH);
        }
    }
}

// Os mínimos são medidos em pixels de um monitor; se a janela está em outro com escala diferente
// (ex.: 100% -> 125%), o mínimo em pixels muda na mesma proporção. Converte antes de usar.
static void ScaleMinsToDpi(Item& it) {
    UINT cur = GetDpiForWindow(it.h);
    if (!cur) return;
    if (it.mDpi && it.mDpi != cur) {
        auto sc = [&](LONG v) { return (LONG)std::lround((double)v * cur / it.mDpi); };
        it.minW = sc(it.minW); it.minH = sc(it.minH); it.lW = sc(it.lW); it.lH = sc(it.lH);
        Wh_Log(L"Escala do monitor mudou (%u -> %u DPI): minimos convertidos para %ldx%ld", it.mDpi, cur, (long)std::max(it.minW, it.lW), (long)std::max(it.minH, it.lH));
    }
    it.mDpi = cur;
}

// converte o mínimo da janela (retângulo externo) no mínimo da área visível (slot)
static SIZE SlotMin(Item& it) {
    QueryMin(it);
    ScaleMinsToDpi(it);
    RECT in = FrameInset(it.h);
    SIZE z;
    z.cx = std::max<LONG>(0, std::max(it.minW, it.lW) - (in.left + in.right));
    z.cy = std::max<LONG>(0, std::max(it.minH, it.lH) - (in.top + in.bottom));
    return z;
}

// Preferência de cantos do Win11 (DWMWA_WINDOW_CORNER_PREFERENCE = 33). Só vale para janelas organizadas.
static void ApplyCorner(Item& it) {
    if (!it.origRead) {
        DWORD v = 0;
        if (FAILED(DwmGetWindowAttribute(it.h, 33, &v, sizeof(v)))) v = 0;
        it.origCorner = (int)v;
        it.origRead = true;
    }
    if (g_cfg.corner < 0 && it.cornerApplied < 0) return;  // nunca mexemos nesta janela
    int want = (it.floating || g_cfg.corner < 0) ? it.origCorner : g_cfg.corner;
    if (want == it.cornerApplied) return;
    DWORD v = (DWORD)want;
    DwmSetWindowAttribute(it.h, 33, &v, sizeof(v));
    it.cornerApplied = want;
}

static void SetTarget(Item& it, RECT slot) {
    RECT in = FrameInset(it.h);
    it.lastInset = in;
    RECT to{slot.left - in.left, slot.top - in.top, slot.right + in.right, slot.bottom + in.bottom};

    if (it.snap) {
        // janela escondida por outro mod (animação de abrir): vai direto ao slot final
        it.snap = false;
        it.fresh = false;
        it.anim = false;
        it.from = it.cur = it.to = to;
        SetWindowPos(it.h, nullptr, to.left, to.top, to.right - to.left, to.bottom - to.top,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
        return;
    }
    if (!it.fresh && !it.anim && EqualRect(&to, &it.to)) return;

    RECT from;
    if (it.fresh) {
        from = to;
        LONG w = to.right - to.left, h = to.bottom - to.top;
        switch (g_cfg.entry) {
            case EN_RIGHT: OffsetRect(&from, w / 6, 0); break;
            case EN_UP: OffsetRect(&from, 0, -h / 6); break;
            case EN_DOWN: OffsetRect(&from, 0, h / 6); break;
            default: break;
        }
        it.fresh = false;
    } else if (it.anim) {
        from = it.cur;
    } else {
        GetWindowRect(it.h, &from);
    }

    it.from = from;
    it.cur = from;
    it.to = to;

    LONG fw = from.right - from.left, fh = from.bottom - from.top;
    LONG tw = to.right - to.left, th = to.bottom - to.top;
    // tamanho fixo durante o movimento (como ao arrastar): usa o menor entre o inicial e o final,
    // então a janela que diminui encolhe já no começo e a que cresce só cresce no fim (sem sobrepor)
    LONG mw = std::min(fw, tw), mh = std::min(fh, th);
    if (mw != fw || mh != fh) {
        SetWindowPos(it.h, nullptr, from.left, from.top, mw, mh,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
    }
    it.mfrom = RECT{from.left, from.top, from.left + mw, from.top + mh};
    it.mto = RECT{to.left, to.top, to.left + mw, to.top + mh};
    it.t0 = NowMs();
    it.anim = true;
    it.applied = RECT{0, 0, 0, 0};
}

// Área útil do monitor: a área de trabalho do Windows (que já desconta a barra de tarefas quando ela
// reserva espaço) e, embaixo, o "Espaço embaixo" configurado (vale o maior entre os dois, não soma).
static RECT UsableArea(HMONITOR, const MONITORINFO& mi) {
    RECT w = mi.rcWork;
    w.bottom = std::min(w.bottom, mi.rcMonitor.bottom - g_cfg.mBottom);
    return w;
}

// Janelas minimizadas: o mod do genie anima a janela voltando para o retângulo em que ela está no
// instante em que o app pede para restaurar, e esse retângulo não muda durante a animação. Como o mod
// só descobre a janela depois (e a coloca num slot novo, no fim da lista), a animação terminava no
// lugar antigo e a janela pulava. Por isso, enquanto a janela está minimizada, gravamos no Windows a
// posição "normal" dela como o slot que ela vai ocupar ao voltar: o genie já anima até o lugar certo.
struct MinEntry { HWND h; HMONITOR mon; LONG minW, minH; RECT inset; RECT last; RECT orig{}; bool haveOrig = false; };
static std::vector<MinEntry> g_minimized;

static void UpdateMinimizedPlacements() {
    if (g_minimized.empty()) return;
    RECT pw{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &pw, 0);  // coordenadas de "workspace" partem da área de trabalho primária
    for (size_t i = 0; i < g_minimized.size();) {
        MinEntry& e = g_minimized[i];
        if (!IsWindow(e.h)) { g_minimized.erase(g_minimized.begin() + i); continue; }
        UINT cl = 0;
        DwmGetWindowAttribute(e.h, DWMWA_CLOAKED, &cl, sizeof(cl));
        // já voltou ao normal (visível, sem cloak): não é mais minimizada
        if (!IsIconic(e.h) && IsWindowVisible(e.h) && !cl) { g_minimized.erase(g_minimized.begin() + i); continue; }
        i++;
        // só mexe na posição de janelas de fato minimizadas (SetWindowPlacement em outra janela a mostraria)
        if (!IsIconic(e.h)) continue;
        MONITORINFO mi{sizeof(mi)};
        if (!GetMonitorInfoW(e.mon, &mi)) continue;
        if (g_monocle) continue;
        std::vector<SIZE> mins;
        for (auto& it : g_items)
            if (!it.floating && !IsZoomed(it.h) && !IsIconic(it.h) && it.mon == e.mon) mins.push_back(SlotMin(it));
        size_t n = mins.size();
        RECT inset = e.inset;
        SIZE z;
        z.cx = std::max<LONG>(0, e.minW - (inset.left + inset.right));
        z.cy = std::max<LONG>(0, e.minH - (inset.top + inset.bottom));
        mins.push_back(z);
        g_scale = MonScale(e.mon);
        auto slots = ComputeSlots((int)n + 1, UsableArea(e.mon, mi), mins, nullptr);
        if (slots.size() <= n) continue;
        RECT sl = slots[n];
        RECT to{sl.left - inset.left, sl.top - inset.top, sl.right + inset.right, sl.bottom + inset.bottom};
        if (EqualRect(&to, &e.last)) continue;
        WINDOWPLACEMENT wp{sizeof(wp)};
        if (!GetWindowPlacement(e.h, &wp)) continue;
        if (!e.haveOrig) { e.orig = wp.rcNormalPosition; e.haveOrig = true; }  // para devolver ao descarregar o mod
        wp.rcNormalPosition = RECT{to.left - pw.left, to.top - pw.top, to.right - pw.left, to.bottom - pw.top};
        wp.flags |= WPF_ASYNCWINDOWPLACEMENT;
        wp.showCmd = SW_SHOWMINNOACTIVE;  // continua minimizada (o genie ignora janelas já minimizadas)
        SetWindowPlacement(e.h, &wp);
        e.last = to;
        {
            wchar_t cls[64] = L"";
            GetClassNameW(e.h, cls, 64);
            Wh_Log(L"Minimizada [%s]: ao voltar vai para %ldx%ld em (%ld,%ld)", cls, to.right - to.left, to.bottom - to.top, to.left, to.top);
        }
    }
}

static void Retile() {
    // agrupa por monitor, mantendo a ordem do vetor
    std::vector<HMONITOR> mons;
    for (auto& it : g_items) {
        if (it.floating || IsZoomed(it.h) || IsIconic(it.h)) continue;
        if (std::find(mons.begin(), mons.end(), it.mon) == mons.end()) mons.push_back(it.mon);
    }
    for (HMONITOR m : mons) {
        MONITORINFO mi{sizeof(mi)};
        if (!GetMonitorInfoW(m, &mi)) continue;
        std::vector<Item*> list;
        for (auto& it : g_items)
            if (!it.floating && !IsZoomed(it.h) && !IsIconic(it.h) && it.mon == m) list.push_back(&it);

        if (g_monocle) {
            RECT full = UsableArea(m, mi);
            g_scale = MonScale(m);
            const int pd = Px(g_cfg.padding);
            full.left += pd; full.top += pd;
            full.right -= pd; full.bottom -= pd;
            for (auto* it : list) if (it->h != g_dragHwnd) SetTarget(*it, full);
        } else {
            g_scale = MonScale(m);
            std::vector<SIZE> mins;
            for (auto* it : list) mins.push_back(SlotMin(*it));
            std::vector<std::pair<double, double>> wts;
            for (auto* it : list) wts.push_back({it->wx, it->wy});
            auto slots = ComputeSlots((int)list.size(), UsableArea(m, mi), mins, &wts);
            for (size_t i = 0; i < list.size(); i++) {
                {
                    wchar_t cls[64] = L"", ttl[40] = L"";
                    GetClassNameW(list[i]->h, cls, 64);
                    GetWindowTextW(list[i]->h, ttl, 40);
                    Wh_Log(L"Grade %zu/%zu [%s] \"%s\": %ldx%ld em (%ld,%ld)", i + 1, list.size(), cls, ttl,
                           slots[i].right - slots[i].left, slots[i].bottom - slots[i].top, slots[i].left, slots[i].top);
                }
                if (list[i]->h != g_dragHwnd) SetTarget(*list[i], slots[i]);
            }
        }
    }
    for (auto& it : g_items) ApplyCorner(it);
    UpdateMinimizedPlacements();
}

// ---------------------------------------------------------------------------
// Animação
// ---------------------------------------------------------------------------

static void StepAnimations() {
    double now = NowMs();
    const UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS;
    for (auto& it : g_items) {
        if (!it.anim) continue;
        // maximizada no meio da animação: para de movê-la (SetWindowPos numa janela maximizada a deixaria
        // "maximizada" fora do lugar, sobrando para o monitor vizinho)
        if (IsZoomed(it.h)) {
            it.anim = false;
            it.verifyAt = 0;
            continue;
        }
        double t = g_cfg.duration <= 0 ? 1.0 : (now - it.t0) / (double)g_cfg.duration;
        if (t >= 1.0) {
            // fim: posição E tamanho finais (é aqui que as janelas que crescem crescem)
            it.anim = false;
            it.cur = it.to;
            SetWindowPos(it.h, nullptr, it.to.left, it.to.top, it.to.right - it.to.left,
                         it.to.bottom - it.to.top, flags);
            it.verifyAt = now + 250;  // apps lentos (UWP/Explorer) levam um tempo para crescer
            it.verifyTries = 0;
            continue;
        }
        double e = Ease(t);
        auto L = [&](LONG a, LONG b) { return a + (LONG)std::lround((b - a) * e); };
        it.cur = RECT{L(it.mfrom.left, it.mto.left), L(it.mfrom.top, it.mto.top),
                      L(it.mfrom.right, it.mto.right), L(it.mfrom.bottom, it.mto.bottom)};
        // a cauda da curva muda menos de 1 px por quadro: não vale chamar o app de novo
        if (EqualRect(&it.cur, &it.applied)) continue;
        it.applied = it.cur;
        SetWindowPos(it.h, nullptr, it.cur.left, it.cur.top, it.cur.right - it.cur.left,
                     it.cur.bottom - it.cur.top, flags);
    }
}

// Depois que a animação termina, confere se o app realmente aceitou o tamanho final.
// Se não, reaplica (até 8 vezes) e registra o que ocorreu no log
// (janela maximizada, tamanho mínimo do app, etc.). Retorna true se ainda há conferências pendentes.
static bool VerifyItems() {
    double now = NowMs();
    bool pending = false, relayout = false;
    const UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS;
    for (auto& it : g_items) {
        if (it.verifyAt <= 0) continue;
        if (it.anim || it.floating || it.h == g_dragHwnd) { it.verifyAt = 0; continue; }
        if (now < it.verifyAt) { pending = true; continue; }
        // outro mod (animação de abrir/restaurar) ainda está mexendo na janela: espera terminar
        if (AppCloaked(it.h) && it.verifyTries < 40) { it.verifyTries++; it.verifyAt = now + 100; pending = true; continue; }
        RECT r;
        if (!GetWindowRect(it.h, &r)) { it.verifyAt = 0; continue; }
        // maximizada depois de organizada: está fora da grade, não é reposicionada
        if (IsZoomed(it.h)) { it.verifyAt = 0; continue; }
        const RECT& t = it.to;
        bool bad = std::abs(r.left - t.left) > 4 || std::abs(r.top - t.top) > 4 ||
                   std::abs(r.right - t.right) > 4 || std::abs(r.bottom - t.bottom) > 4;
        if (!bad) { it.verifyAt = 0; continue; }
        {
            wchar_t cls[64] = L"";
            GetClassNameW(it.h, cls, 64);
            Wh_Log(L"Conferencia (%d) [%s]: esperado %ldx%ld em (%ld,%ld), real %ldx%ld em (%ld,%ld)%s",
                   it.verifyTries + 1, cls, t.right - t.left, t.bottom - t.top, t.left, t.top,
                   r.right - r.left, r.bottom - r.top, r.left, r.top,
                   IsZoomed(it.h) ? L" [janela maximizada]" : L"");
        }
        // o app não aceitou: aprende o tamanho mínimo dele e refaz o layout respeitando-o
        // só aprende o mínimo se a janela ficou NO lugar esperado (senão foi movida/restaurada pelo usuário ou pelo app)
        bool samePos = std::abs(r.left - t.left) <= 2 && std::abs(r.top - t.top) <= 2;
        if (samePos && it.learned < 2 && it.verifyTries >= 1 && (r.right - r.left > (t.right - t.left) + 2 || r.bottom - r.top > (t.bottom - t.top) + 2)) {
            it.learned++;
            ScaleMinsToDpi(it);
            it.lW = std::max<LONG>(it.lW, (r.right - r.left > (t.right - t.left) + 2) ? r.right - r.left : 0);
            it.lH = std::max<LONG>(it.lH, (r.bottom - r.top > (t.bottom - t.top) + 2) ? r.bottom - r.top : 0);
            it.verifyAt = 0;
            relayout = true;
            continue;
        }
        if (it.verifyTries >= 8) {
            it.verifyAt = 0;
            // nem a posição foi aceita (ex.: app rodando como administrador): solta a janela da grade
            if (!samePos) {
                it.floating = true;
                Wh_Log(L"Janela nao aceita ser posicionada: passa a flutuar (use o atalho de reorganizar para tentar de novo)");
                relayout = true;
            }
            continue;
        }
        it.verifyTries++;
        // sempre assíncrono: uma chamada síncrona a um app ocupado ou travado bloquearia a thread do mod
        // inteira (e com ela os atalhos); apps que não respondem são simplesmente ignorados
        if (!IsHungAppWindow(it.h))
            SetWindowPos(it.h, nullptr, t.left, t.top, t.right - t.left, t.bottom - t.top, flags);
        it.verifyAt = now + 80 + 120 * it.verifyTries;  // reaplica com intervalos crescentes
        pending = true;
    }
    if (relayout) {
        for (auto& it : g_items) it.to = RECT{0, 0, 0, 0};
        Retile();
        pending = true;
    }
    return pending;
}

static bool AnyAnimating() {
    for (auto& it : g_items)
        if (it.anim) return true;
    return false;
}

static bool OnAdd(HWND h);

// Detecta uma janela que mudou entre maximizada e normal e refaz a grade.
// Chamada a partir de EVENT_OBJECT_LOCATIONCHANGE (que também informa maximizar/restaurar).
static void CheckZoom(HWND h) {
    int i = FindItem(h);
    if (i < 0) return;
    Item& it = g_items[i];
    bool z = IsZoomed(h) != 0;
    if (z == it.zoomed) return;
    it.zoomed = z;
    it.to = RECT{0, 0, 0, 0};
    if (z) {
        // passou a maximizada: sai da grade; cancela o que estava pendente para ela
        it.anim = false;
        it.verifyAt = 0;
    }
    Retile();
    if (h == g_borderHwnd && UseOverlay()) DrawOverlay(h);  // esconde/mostra a borda desenhada
}

// processa janelas novas que já "assentaram" (título/estilo definidos)
static void ProcessPending() {
    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_pending.size();) {
        if (g_pending[i].second <= now) {
            Pend p = g_pending[i];
            g_pending.erase(g_pending.begin() + i);
            // janela escondida por outro mod ainda sem título/estilo: tenta de novo em breve
            if (!OnAdd(p.first) && p.tries < 6 && g_cfg.launchSync && IsWindow(p.first) && AppCloaked(p.first)) {
                p.tries++;
                p.second = GetTickCount64() + 30;
                g_pending.push_back(p);
            }
        } else {
            i++;
        }
    }
}

// ---------------------------------------------------------------------------
// Eventos
// ---------------------------------------------------------------------------

// Ordem das janelas na grade: cada janela tem um 'seq' e g_items fica sempre ordenada por ele.
// Ao trocar de área de trabalho virtual o Windows esconde (cloak) as janelas da outra área; elas saem da grade
// e, na volta, reentram juntas. Guardamos o 'seq' (e se estavam soltas) para que voltem na mesma ordem e
// no mesmo lugar, sem repetir a animação de entrada.
static long g_seqNext = 0;
struct VDeskMem { HWND h; long seq; bool floating; };
static std::vector<VDeskMem> g_vdesk;

static void RememberVDesk(HWND h) {
    DWORD c = 0;
    if (FAILED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &c, sizeof(c))) || !(c & 2)) return;  // 2 = DWM_CLOAKED_SHELL (outra área de trabalho)
    int i = FindItem(h);
    if (i < 0) return;
    g_vdesk.erase(std::remove_if(g_vdesk.begin(), g_vdesk.end(), [&](const VDeskMem& e) { return e.h == h || !IsWindow(e.h); }), g_vdesk.end());
    g_vdesk.push_back({h, g_items[i].seq, g_items[i].floating});
}

static void ResetWeights() {
    for (auto& it : g_items) it.wx = it.wy = 1.0;
    g_mainFrac = 0;
}

static bool OnAdd(HWND h) {
    if (FindItem(h) >= 0) return true;
    bool ac = g_cfg.launchSync && AppCloaked(h);
    if (!IsManageable(h, ac)) return false;
    Item it;
    it.h = h;
    it.snap = ac;
    it.fresh = !ac;
    {
        wchar_t cls[64] = L"", ttl[48] = L"";
        GetClassNameW(h, cls, 64);
        GetWindowTextW(h, ttl, 48);
        Wh_Log(L"Janela nova [%s] \"%s\": escondida por outro mod=%d (%s)", cls, ttl, (int)ac,
               ac ? L"vai direto ao slot" : L"anima entrada");
    }
    it.mon = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
    GetWindowRect(h, &it.cur);
    it.to = it.cur;
    if (IsZoomed(h)) {
        // janela que volta/entra maximizada: devolve ao modo normal para ela assumir a grade
        // (com "manter maximizadas" ligado, ela fica como está, fora da grade, até deixar de estar maximizada)
        it.zoomed = true;
        it.fresh = false;
        it.snap = false;
        if (!g_cfg.keepMaximized) ShowWindowAsync(h, SW_RESTORE);
    }
    it.seq = ++g_seqNext;
    for (size_t k = 0; k < g_vdesk.size(); k++) {
        if (g_vdesk[k].h != h) continue;
        // voltou de outra área de trabalho virtual: retoma o lugar na ordem e o estado de solta, sem animar a entrada
        it.seq = g_vdesk[k].seq;
        it.floating = g_vdesk[k].floating;
        it.fresh = false;
        it.snap = false;
        g_vdesk.erase(g_vdesk.begin() + k);
        break;
    }
    ResetWeights();
    for (size_t i = 0; i < g_minimized.size();)
        if (g_minimized[i].h == h) g_minimized.erase(g_minimized.begin() + i);
        else i++;
    g_items.insert(std::upper_bound(g_items.begin(), g_items.end(), it.seq, [](long v, const Item& x) { return v < x.seq; }), it);
    Retile();
    if (GetForegroundWindow() == h) UpdateBorder(h);
    return true;
}

static void OnRemove(HWND h) {
    for (size_t i = 0; i < g_pending.size();)
        if (g_pending[i].first == h) g_pending.erase(g_pending.begin() + i);
        else i++;

    int i = FindItem(h);
    if (i < 0) return;
    // saiu da grade por ter sido minimizada: lembra onde ela vai entrar ao voltar (ver UpdateMinimizedPlacements)
    // (o genie esconde a janela com "cloak" ANTES de minimizá-la de verdade, então o evento de remoção chega
    // com a janela ainda não minimizada: por isso basta a janela existir, e a posição só é gravada quando ela já estiver minimizada)
    if (!g_items[i].floating && IsWindow(h)) {
        bool have = false;
        for (auto& e : g_minimized) if (e.h == h) have = true;
        if (!have)
            g_minimized.push_back(MinEntry{h, g_items[i].mon, std::max(g_items[i].minW, g_items[i].lW), std::max(g_items[i].minH, g_items[i].lH), g_items[i].lastInset, RECT{0, 0, 0, 0}});
    }
    // the window leaves the grid: give back everything the mod changed on it (focus border, corner preference)
    if (g_borderHwnd == h) {
        SetBorder(h, false);
        g_borderHwnd = nullptr;
        if (g_ov) HideOverlay();
    }
    {
        Item& gone = g_items[i];
        if (gone.cornerApplied >= 0 && gone.cornerApplied != gone.origCorner) {
            DWORD v = (DWORD)gone.origCorner;
            DwmSetWindowAttribute(h, 33, &v, sizeof(v));
        }
    }
    if (g_dragHwnd == h) g_dragHwnd = nullptr;
    g_items.erase(g_items.begin() + i);
    ResetWeights();
    Retile();
}

// Troca duas janelas de posição na lista SEM carregar o estado de animação junto:
// cada janela recomeça a animação a partir de onde ela está de fato na tela.
static void SwapItems(size_t i, size_t j) {
    std::swap(g_items[i], g_items[j]);
    std::swap(g_items[i].seq, g_items[j].seq);  // o 'seq' acompanha a posição na grade, não a janela
    for (size_t k : {i, j}) {
        Item& it = g_items[k];
        GetWindowRect(it.h, &it.cur);
        it.anim = false;
        it.to = RECT{0, 0, 0, 0};
    }
}

static RECT g_dragStart{};
static bool g_retileResetAll = false;  // o próximo "retile" agendado recalcula o destino de todas as janelas
static bool g_dragEdge = false;  // o arrasto começou na borda da janela (redimensionar) e não na barra de título (mover)
static ULONGLONG g_retileAt = 0;  // quando refazer o layout depois de um arrasto
static HWND g_retileHwnd = nullptr;

// Depois de soltar uma janela, espera um instante antes de reorganizar: o Windows pode ainda estar
// aplicando o "snap" de borda (meia tela/maximizar) e sobrescreveria a posição que o mod acabou de pedir.
static void ScheduleRetile(HWND h, ULONGLONG ms) {
    g_retileHwnd = h;
    g_retileAt = GetTickCount64() + ms;
}

static void RunScheduledRetile() {
    if (!g_retileAt || GetTickCount64() < g_retileAt) return;
    g_retileAt = 0;
    HWND h = g_retileHwnd;
    g_retileHwnd = nullptr;
    int i = FindItem(h);
    if (i >= 0) {
        Item& it = g_items[i];
        if (IsZoomed(h) && !g_cfg.keepMaximized) {
            // o arrasto até a borda de cima maximizou a janela: devolve ao modo normal para ela voltar à grade
            Wh_Log(L"Arrasto maximizou a janela: restaurando para voltar a grade");
            ShowWindowAsync(h, SW_RESTORE);
            it.zoomed = false;
            g_retileAt = GetTickCount64() + 150;
            g_retileHwnd = h;
            return;
        }
        it.anim = false;
        GetWindowRect(h, &it.cur);
        it.to = RECT{0, 0, 0, 0};
    }
    if (g_retileResetAll) {
        g_retileResetAll = false;
        for (auto& o : g_items) o.to = RECT{0, 0, 0, 0};
    }
    Retile();
}

static void LiveBegin(HWND h, const RECT& start, bool native);

static void OnMoveStart(HWND h) {
    int i = FindItem(h);
    if (i >= 0) {
        GetWindowRect(h, &g_dragStart);
        {
            POINT c{};
            GetCursorPos(&c);
            RECT in = FrameInset(h);
            RECT vis{g_dragStart.left + in.left, g_dragStart.top + in.top, g_dragStart.right - in.right, g_dragStart.bottom - in.bottom};
            const LONG e = 12;
            g_dragEdge = c.x <= vis.left + e || c.x >= vis.right - e || c.y >= vis.bottom - e || c.y <= vis.top + 5;
        }
        {
            wchar_t cls[64] = L"", ttl[48] = L"";
            GetClassNameW(h, cls, 64);
            GetWindowTextW(h, ttl, 48);
            Wh_Log(L"Arrasto iniciado [%s] \"%s\" (%s) em (%ld,%ld) %ldx%ld%s", cls, ttl, g_dragEdge ? L"pela borda" : L"pelo meio", (long)g_dragStart.left, (long)g_dragStart.top,
                   (long)(g_dragStart.right - g_dragStart.left), (long)(g_dragStart.bottom - g_dragStart.top),
                   g_items[i].floating ? L" [flutuante]" : L"");
        }
        g_items[i].anim = false;
        g_dragHwnd = h;
        // arrasto pela borda de uma janela da grade: as vizinhas acompanham enquanto ela muda de tamanho
        if (g_dragEdge && !g_items[i].floating) LiveBegin(h, g_dragStart, true);
    }
}

// O usuário puxou a borda de uma janela organizada: transforma o novo tamanho em proporções
// (pesos) para que o grid se ajuste, em vez de desfazer o redimensionamento.
static bool ApplyResize(size_t i, const RECT& now) {
    const RECT st = g_dragStart;
    const LONG thr = 6;
    LONG dL = now.left - st.left, dR = now.right - st.right, dT = now.top - st.top, dB = now.bottom - st.bottom;
    if (std::abs((now.right - now.left) - (st.right - st.left)) <= thr &&
        std::abs((now.bottom - now.top) - (st.bottom - st.top)) <= thr)
        return false;  // só mudou de lugar

    Item& me = g_items[i];
    if (me.to.right <= me.to.left) return false;
    std::vector<size_t> idx;
    for (size_t j = 0; j < g_items.size(); j++)
        if (!g_items[j].floating && !IsZoomed(g_items[j].h) && !IsIconic(g_items[j].h) && g_items[j].mon == me.mon &&
            g_items[j].to.right > g_items[j].to.left)
            idx.push_back(j);
    if (idx.size() < 2) return true;

    int layout = g_layoutRt >= 0 ? g_layoutRt : g_cfg.layout;
    bool master = (layout == 4 || layout == 5) && idx.size() >= 2;
    size_t mainIdx = master ? idx[0] : (size_t)-1;
    g_scale = MonScale(me.mon);
    int gap = Px(g_cfg.gap);
    RECT bb{LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN};
    for (size_t j : idx) {
        bb.left = std::min(bb.left, g_items[j].to.left); bb.top = std::min(bb.top, g_items[j].to.top);
        bb.right = std::max(bb.right, g_items[j].to.right); bb.bottom = std::max(bb.bottom, g_items[j].to.bottom);
    }
    // as janelas têm bordas invisíveis (a área de destino 'to' as inclui); vizinhança e alinhamento
    // precisam ser medidos na área visível, onde o espaço entre janelas é exatamente o 'gap'
    auto vis = [&](size_t j) {
        RECT r = g_items[j].to;
        const RECT& in = g_items[j].lastInset;
        r.left += in.left; r.top += in.top; r.right -= in.right; r.bottom -= in.bottom;
        return r;
    };
    const RECT t = vis(i);
    auto closeTo = [](LONG a, LONG b, LONG tol) { return std::abs(a - b) <= tol; };
    auto overlapV = [](const RECT& a, const RECT& b) { return std::min(a.bottom, b.bottom) - std::max(a.top, b.top) > 0; };
    auto overlapH = [](const RECT& a, const RECT& b) { return std::min(a.right, b.right) - std::max(a.left, b.left) > 0; };

    // pesos de partida = tamanhos atuais em pixels (assim só o que foi arrastado muda)
    for (size_t j : idx) {
        g_items[j].wx = (double)(g_items[j].to.right - g_items[j].to.left);
        g_items[j].wy = (double)(g_items[j].to.bottom - g_items[j].to.top);
    }
    const double kMin = 80.0;

    // --- horizontal ---
    auto horiz = [&](LONG delta, bool rightEdge) {
        if (std::abs(delta) <= thr) return;
        if (master && layout == 4) {
            double W = (double)(bb.right - bb.left) - gap;
            if (i == mainIdx && rightEdge) g_mainFrac = ((double)(now.right - bb.left)) / W;
            else if (i != mainIdx && !rightEdge) g_mainFrac = ((double)(now.left - gap - bb.left)) / W;
            if (i == mainIdx || !rightEdge) return;
        }
        for (size_t j : idx) {
            if (j == i) continue;
            const RECT r = vis(j);
            bool adj = rightEdge ? closeTo(r.left, t.right + gap, 8) : closeTo(r.right + gap, t.left, 8);
            if (!adj || !overlapV(r, t)) continue;
            if (master && layout == 4 && (j == mainIdx || i == mainIdx)) continue;
            double d = (double)delta * (rightEdge ? 1 : -1);
            // a largura é da coluna inteira: todas as janelas empilhadas nela mudam juntas
            for (size_t q : idx) {
                if (closeTo(vis(q).left, t.left, 4)) g_items[q].wx = std::max(kMin, g_items[q].wx + d);
                else if (closeTo(vis(q).left, r.left, 4)) g_items[q].wx = std::max(kMin, g_items[q].wx - d);
            }
        }
    };
    horiz(dR, true);
    horiz(dL, false);

    // --- vertical ---
    auto vert = [&](LONG delta, bool bottomEdge) {
        if (std::abs(delta) <= thr) return;
        if (master && layout == 5) {
            double H = (double)(bb.bottom - bb.top) - gap;
            if (i == mainIdx && bottomEdge) g_mainFrac = ((double)(now.bottom - bb.top)) / H;
            else if (i != mainIdx && !bottomEdge) g_mainFrac = ((double)(now.top - gap - bb.top)) / H;
            if (i == mainIdx || !bottomEdge) return;
        }
        // vizinha = a janela logo abaixo/acima (na mesma coluna); só as duas trocam altura
        size_t nb = (size_t)-1;
        for (size_t j : idx) {
            if (j == i) continue;
            const RECT r = vis(j);
            if (!overlapH(r, t)) continue;
            if (bottomEdge ? closeTo(r.top, t.bottom + gap, 8) : closeTo(r.bottom + gap, t.top, 8)) { nb = j; break; }
        }
        if (nb == (size_t)-1) {
            // layout de linhas (itens lado a lado): cai no comportamento por linha
            std::vector<size_t> mine, other;
            for (size_t j : idx) {
                const RECT r = vis(j);
                if (closeTo(r.top, t.top, 4)) mine.push_back(j);
                else if (bottomEdge && closeTo(r.top, t.bottom + gap, 8)) other.push_back(j);
                else if (!bottomEdge && closeTo(r.bottom + gap, t.top, 8)) other.push_back(j);
            }
            if (other.empty()) return;
            double d2 = (double)delta * (bottomEdge ? 1 : -1);
            for (size_t j : mine) g_items[j].wy = std::max(kMin, g_items[j].wy + d2);
            for (size_t j : other) g_items[j].wy = std::max(kMin, g_items[j].wy - d2);
            return;
        }
        if (master && layout == 5 && nb == mainIdx) return;
        double d = (double)delta * (bottomEdge ? 1 : -1);
        g_items[i].wy = std::max(kMin, g_items[i].wy + d);
        g_items[nb].wy = std::max(kMin, g_items[nb].wy - d);
    };
    vert(dB, true);
    vert(dT, false);
    return true;
}

// ---------------------------------------------------------------------------
// Redimensionamento ao vivo
// ---------------------------------------------------------------------------
// Enquanto uma janela da grade muda de tamanho (pela borda ou com Alt + botão direito), as outras são
// reposicionadas na hora para ceder ou ocupar o espaço, em vez de só se ajustarem quando o botão é solto.
//
// Apps demoram tempos diferentes para se redesenhar (o Explorador de Arquivos e os apps da Microsoft Store
// são bem mais lentos que os outros). Mandar posições novas sem esperar deixa o app lento cada vez mais
// para trás e abre buracos entre as janelas. Por isso a grade avança em passos:
//   1. as janelas que crescem recebem a posição nova;
//   2. quando elas chegam lá (ou passa um tempo-limite), as que diminuem recebem a delas;
//   3. só então começa o passo seguinte, já com a posição mais recente do cursor.
// Assim todas andam no ritmo da mais lenta, e no meio de um passo as janelas se sobrepõem um pouco em vez
// de deixar um vão.

struct LiveResize {
    bool active = false;
    bool native = false;  // true = arrasto pela borda (o Windows move a própria janela); false = Alt + botão direito
    HWND h = nullptr;     // janela sendo redimensionada
    RECT start{};         // retângulo dela no início
    RECT lastNow{};       // último retângulo dela visto (arrasto pela borda)
    // A grade como estava no início. A cada passo os pesos são recalculados a partir dela (e não do passo
    // anterior), então o resultado depende só do tamanho atual da janela arrastada.
    struct Base { HWND h; RECT to; double wx, wy; };
    std::vector<Base> base;
    double baseMainFrac = 0;
    struct Want { HWND h; RECT r; };
    std::vector<Want> want;     // onde cada janela deve ficar agora
    std::vector<Want> later;    // as que diminuem neste passo: enviadas depois que as que crescem chegarem
    std::vector<HWND> waiting;  // janelas cuja chegada estamos esperando
    int phase = 0;              // 0 = livre, 1 = esperando as que crescem, 2 = esperando as que diminuem
    double sentAt = 0;
};
static LiveResize g_live;

static void LiveBegin(HWND h, const RECT& start, bool native) {
    LiveResize& L = g_live;
    if (native && L.active) return;  // um redimensionamento com Alt + botão direito já está em curso
    L = LiveResize{};
    L.active = true;
    L.native = native;
    L.h = h;
    L.start = start;
    L.lastNow = start;
    for (auto& o : g_items) {
        o.anim = false;
        o.applied = RECT{0, 0, 0, 0};
        L.base.push_back({o.h, o.to, o.wx, o.wy});
    }
    L.baseMainFrac = g_mainFrac;
}

static void LiveEnd() { g_live = LiveResize{}; }

// Devolve a grade ao estado do início (destinos e pesos). Retorna false se alguma janela entrou ou saiu da
// grade nesse meio tempo: o estado guardado não vale mais.
static bool LiveRestoreBase() {
    LiveResize& L = g_live;
    if (L.base.size() != g_items.size()) return false;
    for (const auto& b : L.base) {
        int k = FindItem(b.h);
        if (k < 0) return false;
        g_items[k].to = b.to;
        g_items[k].wx = b.wx;
        g_items[k].wy = b.wy;
    }
    g_mainFrac = L.baseMainFrac;
    return true;
}

// Recalcula onde cada janela do monitor deve ficar para o tamanho atual (now) da janela arrastada.
// Retorna false se a grade mudou e o redimensionamento ao vivo deve ser encerrado.
static bool LiveCompute(const RECT& now) {
    LiveResize& L = g_live;
    int i = FindItem(L.h);
    if (i < 0 || !LiveRestoreBase()) return false;
    g_dragStart = L.start;
    if (!ApplyResize((size_t)i, now)) LiveRestoreBase();  // mudança pequena demais: fica como no início
    L.want.clear();
    HMONITOR m = g_items[i].mon;
    MONITORINFO mi{sizeof(mi)};
    if (g_monocle || !GetMonitorInfoW(m, &mi)) return true;
    std::vector<Item*> list;
    for (auto& it : g_items)
        if (!it.floating && !IsZoomed(it.h) && !IsIconic(it.h) && it.mon == m) list.push_back(&it);
    g_scale = MonScale(m);
    std::vector<SIZE> mins;
    std::vector<std::pair<double, double>> wts;
    for (auto* it : list) {
        mins.push_back(SlotMin(*it));
        wts.push_back({it->wx, it->wy});
    }
    auto slots = ComputeSlots((int)list.size(), UsableArea(m, mi), mins, &wts);
    for (size_t k = 0; k < list.size() && k < slots.size(); k++) {
        if (L.native && list[k]->h == L.h) continue;  // no arrasto pela borda, o Windows é quem move a janela
        const RECT& in = list[k]->lastInset;
        L.want.push_back({list[k]->h, RECT{slots[k].left - in.left, slots[k].top - in.top, slots[k].right + in.right, slots[k].bottom + in.bottom}});
    }
    return true;
}

static void LiveSend(const LiveResize::Want& w) {
    int k = FindItem(w.h);
    if (k < 0) return;
    Item& it = g_items[k];
    it.anim = false;
    it.verifyAt = 0;
    it.applied = w.r;
    it.cur = w.r;
    g_live.waiting.push_back(w.h);
    if (IsHungAppWindow(w.h)) return;
    SetWindowPos(w.h, nullptr, w.r.left, w.r.top, w.r.right - w.r.left, w.r.bottom - w.r.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
}

// As janelas do passo atual já chegaram onde foram mandadas? (ou passou o tempo-limite)
static bool LiveSettled() {
    LiveResize& L = g_live;
    if (NowMs() - L.sentAt > 120.0) return true;  // app que não responde ou que recusou o tamanho: segue em frente
    for (HWND h : L.waiting) {
        int k = FindItem(h);
        RECT r{};
        if (k < 0 || !GetWindowRect(h, &r)) continue;
        const RECT& a = g_items[k].applied;
        if (std::abs(r.left - a.left) > 2 || std::abs(r.top - a.top) > 2 || std::abs(r.right - a.right) > 2 ||
            std::abs(r.bottom - a.bottom) > 2)
            return false;
    }
    return true;
}

// Avança o redimensionamento ao vivo. Chamada a cada volta do laço da thread de trabalho.
static void LivePump() {
    LiveResize& L = g_live;
    if (!L.active) return;
    if (L.phase != 0) {
        if (!LiveSettled()) return;
        L.waiting.clear();
        if (L.phase == 1 && !L.later.empty()) {
            for (const auto& w : L.later) LiveSend(w);
            L.later.clear();
            L.phase = 2;
            L.sentAt = NowMs();
            return;
        }
        L.phase = 0;
    }
    // novo passo: primeiro as janelas que crescem, depois (na fase seguinte) as que diminuem
    std::vector<LiveResize::Want> grow;
    for (const auto& w : L.want) {
        int k = FindItem(w.h);
        if (k < 0) continue;
        RECT cur = g_items[k].applied;
        if (cur.right <= cur.left) GetWindowRect(w.h, &cur);
        if (EqualRect(&cur, &w.r)) continue;
        const LONGLONG areaNow = (LONGLONG)(cur.right - cur.left) * (cur.bottom - cur.top);
        const LONGLONG areaNew = (LONGLONG)(w.r.right - w.r.left) * (w.r.bottom - w.r.top);
        if (areaNew >= areaNow) grow.push_back(w);
        else L.later.push_back(w);
    }
    if (grow.empty() && L.later.empty()) return;
    if (grow.empty()) {
        for (const auto& w : L.later) LiveSend(w);
        L.later.clear();
        L.phase = 2;
    } else {
        for (const auto& w : grow) LiveSend(w);
        L.phase = 1;
    }
    L.sentAt = NowMs();
}

// Arrasto pela borda: acompanha o tamanho que o Windows está dando à janela e ajusta as vizinhas.
static void NativeLiveTick() {
    LiveResize& L = g_live;
    if (!L.active || !L.native) return;
    RECT r{};
    if (g_dragHwnd != L.h || !GetWindowRect(L.h, &r)) {
        LiveEnd();
        return;
    }
    if (!EqualRect(&r, &L.lastNow)) {
        L.lastNow = r;
        if (!LiveCompute(r)) {
            LiveEnd();
            return;
        }
    }
    LivePump();
}

static void OnMoveEnd(HWND h) {
    g_dragHwnd = nullptr;
    int i = FindItem(h);
    if (g_live.active && g_live.native) {
        // fim do arrasto pela borda com as vizinhas acompanhando: volta à grade do início para que o
        // cálculo final abaixo parta dela, e pede que todas as janelas sejam conferidas no fim
        const bool ok = LiveRestoreBase();
        LiveEnd();
        g_retileResetAll = true;
        if (!ok) {
            ScheduleRetile(h, 60);
            return;
        }
    }
    wchar_t cls[64] = L"", ttl[48] = L"";
    {
        GetClassNameW(h, cls, 64);
        GetWindowTextW(h, ttl, 48);
    }
    if (i < 0) {
        // janela que ainda não estava na grade (ex.: já existia ao carregar o mod, ou foi ignorada antes):
        // se ela é elegível agora, passa a fazer parte
        bool ok = GetAncestor(h, GA_ROOT) == h && IsManageable(h) && OnAdd(h);
        Wh_Log(L"Arrasto terminou em janela fora da grade [%s] \"%s\": %s", cls, ttl, ok ? L"adotada pela grade" : L"nao elegivel (veja 'Ignorada' acima)");
        return;
    }
    {
        HMONITOR nm = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
        if (nm != g_items[i].mon) {
            g_items[i].mon = nm;  // os mínimos em pixels são reconvertidos para a escala do novo monitor (ScaleMinsToDpi)
        }
    }
    RECT nowR{};
    GetWindowRect(h, &nowR);
    if (!g_dragEdge) {
        LONG dw = (nowR.right - nowR.left) - (g_dragStart.right - g_dragStart.left);
        LONG dh = (nowR.bottom - nowR.top) - (g_dragStart.bottom - g_dragStart.top);
        if (std::abs(dw) > 6 || std::abs(dh) > 6)
            Wh_Log(L"ATENCAO: o tamanho mudou durante um arrasto pelo meio (%ldx%ld -> %ldx%ld): outra coisa redimensionou a janela",
                   (long)(g_dragStart.right - g_dragStart.left), (long)(g_dragStart.bottom - g_dragStart.top),
                   (long)(nowR.right - nowR.left), (long)(nowR.bottom - nowR.top));
    }
    auto logEnd = [&](const wchar_t* decisao) {
        Wh_Log(L"Arrasto terminou [%s] \"%s\" em (%ld,%ld) %ldx%ld%s: %s", cls, ttl, (long)nowR.left, (long)nowR.top,
                   (long)(nowR.right - nowR.left), (long)(nowR.bottom - nowR.top), IsZoomed(h) ? L" [maximizada]" : L"", decisao);
    };

    if (!g_items[i].floating && g_dragEdge) {
        if (ApplyResize((size_t)i, nowR)) {
            RECT st = g_dragStart;
            bool resized = std::abs((nowR.right - nowR.left) - (st.right - st.left)) > 6 ||
                           std::abs((nowR.bottom - nowR.top) - (st.bottom - st.top)) > 6;
            if (resized) {
                for (auto& it : g_items) it.to = RECT{0, 0, 0, 0};
                g_items[i].anim = false;
                g_items[i].cur = nowR;
                logEnd(L"redimensionada: grid ajustado");
                ScheduleRetile(h, 60);
                return;
            }
        }
    }
    if (!g_items[i].floating) {
        // soltou com o cursor sobre outra janela organizada? troca de lugar
        POINT pt{};
        GetCursorPos(&pt);
        for (size_t j = 0; j < g_items.size(); j++) {
            if ((int)j == i || g_items[j].floating) continue;
            RECT vis = g_items[j].to;  // retângulo de destino da outra janela, sem as bordas invisíveis
            RECT in = FrameInset(g_items[j].h);
            vis.left += in.left; vis.top += in.top; vis.right -= in.right; vis.bottom -= in.bottom;
            if (PtInRect(&vis, pt)) {
                SwapItems((size_t)i, j);
                logEnd(L"trocou de lugar com outra janela");
                ScheduleRetile(h, 60);
                return;
            }
        }
        logEnd(L"voltando ao layout");
    } else {
        logEnd(L"janela flutuante: ignorada pelo layout (Win+Ctrl+T ou 'reorganizar tudo' a prende de novo)");
        return;
    }
    // padrão: volta ao layout animando a partir de onde foi solta
    ScheduleRetile(h, 120);
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD ev, HWND hwnd, LONG idObj, LONG idChild, DWORD, DWORD) {
    if (idObj != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) return;
    switch (ev) {
        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_UNCLOAKED:
        case EVENT_SYSTEM_MINIMIZEEND:
            if (FindItem(hwnd) < 0) {
                // só janelas de nível superior sem dono interessam (evita a enxurrada de eventos de janelas filhas)
                if (GetAncestor(hwnd, GA_ROOT) != hwnd || GetWindow(hwnd, GW_OWNER)) break;
                // escondida por outro mod (genie): posiciona já; senão espera assentar
                ULONGLONG d = (g_cfg.launchSync && AppCloaked(hwnd)) ? 0 : 150;
                g_pending.push_back({hwnd, GetTickCount64() + d, 0});
            } else if (ev == EVENT_OBJECT_UNCLOAKED) {
                // fim da animação do outro mod: confere se a janela ficou onde deveria
                Item& it = g_items[FindItem(hwnd)];
                RECT r;
                if (!it.anim && !it.floating && GetWindowRect(hwnd, &r) &&
                    (std::abs(r.left - it.to.left) > 2 || std::abs(r.top - it.to.top) > 2 ||
                     std::abs(r.right - it.to.right) > 2 || std::abs(r.bottom - it.to.bottom) > 2)) {
                    it.to = RECT{0, 0, 0, 0};
                    Retile();
                }
            }
            break;
        case EVENT_SYSTEM_MINIMIZESTART:
            OnRemove(hwnd);
            UpdateMinimizedPlacements();  // a janela do genie chega aqui já removida da grade e agora minimizada
            break;
        case EVENT_OBJECT_DESTROY:
            for (size_t k = 0; k < g_minimized.size();)
                if (g_minimized[k].h == hwnd) g_minimized.erase(g_minimized.begin() + k);
                else k++;
            OnRemove(hwnd);
            break;
        case EVENT_OBJECT_CLOAKED:
            RememberVDesk(hwnd);
            OnRemove(hwnd);
            break;
        case EVENT_OBJECT_HIDE:
            OnRemove(hwnd);
            break;
        case EVENT_SYSTEM_FOREGROUND:
            UpdateBorder(hwnd);
            break;
        case EVENT_OBJECT_LOCATIONCHANGE:
            // a static drawn border follows its window from here; animated styles redraw on their tick
            if (hwnd == g_borderHwnd && g_cfg.borderStyle == 0 && UseOverlay()) DrawOverlay(hwnd);
            CheckZoom(hwnd);
            break;
        case EVENT_SYSTEM_MOVESIZESTART:
            OnMoveStart(hwnd);
            break;
        case EVENT_SYSTEM_MOVESIZEEND:
            OnMoveEnd(hwnd);
            break;
    }
}

// ---------------------------------------------------------------------------
// Atalhos
// ---------------------------------------------------------------------------

// Janela vizinha da i-ésima no lado pedido, procurada pela posição real na tela (mesmo monitor), qualquer
// que seja o layout. dir: 0 = cima, 1 = esquerda, 2 = direita, 3 = baixo. Retorna -1 se não houver.
static int FindNeighbor(int i, int dir) {
    if (i < 0 || g_items[i].floating) return -1;
    if (g_items[i].to.right <= g_items[i].to.left) return -1;  // ainda sem posição final calculada
    auto vis = [&](const Item& it) {
        RECT r = it.to;
        r.left += it.lastInset.left; r.top += it.lastInset.top;
        r.right -= it.lastInset.right; r.bottom -= it.lastInset.bottom;
        return r;
    };
    const RECT t = vis(g_items[i]);
    int best = -1;
    LONG bestGap = LONG_MAX, bestOv = 0;
    for (size_t j = 0; j < g_items.size(); j++) {
        if ((int)j == i) continue;
        const Item& o = g_items[j];
        if (o.floating || o.mon != g_items[i].mon || IsZoomed(o.h) || IsIconic(o.h) || o.to.right <= o.to.left) continue;
        const RECT r = vis(o);
        LONG ov, gap;
        if (dir == 0) {  // cima: sobreposta na horizontal e acima
            ov = std::min(r.right, t.right) - std::max(r.left, t.left);
            if (r.top >= t.top) continue;
            gap = t.top - r.bottom;
        } else if (dir == 1) {  // esquerda: sobreposta na vertical e à esquerda
            ov = std::min(r.bottom, t.bottom) - std::max(r.top, t.top);
            if (r.left >= t.left) continue;
            gap = t.left - r.right;
        } else if (dir == 2) {  // direita
            ov = std::min(r.bottom, t.bottom) - std::max(r.top, t.top);
            if (r.left <= t.left) continue;
            gap = r.left - t.right;
        } else {  // baixo: sobreposta na horizontal e abaixo
            ov = std::min(r.right, t.right) - std::max(r.left, t.left);
            if (r.top <= t.top) continue;
            gap = r.top - t.bottom;
        }
        if (ov <= 0 || gap < -4) continue;  // não é vizinha daquele lado (gap negativo = sobrepõe)
        if (gap < bestGap || (gap == bestGap && ov > bestOv)) { best = (int)j; bestGap = gap; bestOv = ov; }
    }
    return best;
}

// Move a janela em foco na direção da seta: troca de lugar com a janela vizinha naquele lado.
static void SwapFocusedDir(int dir) {
    int i = FindItem(GetForegroundWindow());
    int best = FindNeighbor(i, dir);
    if (best < 0) {
        Wh_Log(L"Atalho mover janela (%s): nao ha janela desse lado da que esta em foco",
               dir == 0 ? L"cima" : dir == 1 ? L"esquerda" : dir == 2 ? L"direita" : L"baixo");
        return;
    }
    SwapItems((size_t)i, (size_t)best);
    Retile();
}

// ---------------------------------------------------------------------------
// Menu radial: mover a janela com o mouse
// ---------------------------------------------------------------------------
// Enquanto o botão do mouse escolhido fica pressionado sobre uma janela, um anel aparece no cursor. Mover o
// mouse para um lado escolhe a janela vizinha daquele lado (o espaço de destino é destacado na tela); soltar o
// botão troca as duas de lugar. Soltar sem sair do centro do anel devolve o clique ao app; Esc cancela.
// O gancho de mouse só avisa que o botão foi pressionado/solto; a posição do cursor é lida a cada quadro.

struct Radial {
    bool active = false;
    HWND target = nullptr;  // janela que será movida
    POINT origin{};         // onde o cursor estava quando o menu abriu (centro do anel)
    int dir = -1;           // direção escolhida (-1 = nenhuma)
    int nb = -1;            // índice da vizinha naquela direção (-1 = não há)
    double scale = 1.0;
    HWND ring = nullptr;
    HWND prev = nullptr;
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    uint32_t* bits = nullptr;
    int size = 0;
    int drawnDir = -2;
    bool drawnOk = false;
    bool shown = false;     // o anel já apareceu (ele espera um instante, para não piscar num clique rápido)
    double beganAt = 0;
    std::vector<uint32_t> back;  // tema vidro: o que estava na tela sob o anel quando ele abriu, já desfocado
    int backSize = 0;
};
static Radial g_radial;

// Cores do anel: as da borda da janela em foco (acompanhando o efeito dela) ou as escolhidas pelo usuário.
// A cor da seleção é também a do destaque sobre a janela que vai receber a que está sendo movida.
static COLORREF RadialSelColor() { return g_cfg.radialCustom ? g_cfg.radialSel : BorderColorNow(); }
static COLORREF RadialRingColor() { return g_cfg.radialCustom ? g_cfg.radialRing : RGB(255, 255, 255); }
static bool RadialColorsAnimated() { return !g_cfg.radialCustom && g_cfg.borderStyle != 0; }
// Medidas do anel. O furo do meio tem tamanho fixo; a espessura (do tema ou a escolhida) cresce para fora.
static double RadialHalf(double scale) {
    const int th = g_cfg.radialThickness > 0 ? g_cfg.radialThickness : (g_cfg.radialTheme == 1 ? 20 : 10);
    return th * scale / 2.0;
}
static double RadialRad(double scale) { return 28.0 * scale + RadialHalf(scale); }  // raio do meio do anel
static int RadialSize(double scale) {
    const double margin = (g_cfg.radialTheme == 1 ? 10.0 : 8.0) * scale;  // sombra / disco de fundo em volta
    return std::max(48, (int)std::lround(2.0 * (RadialRad(scale) + RadialHalf(scale) + margin)) + 2);
}

// Botão que abre o menu radial (opcional): 0 = desligado, 1 = botão do meio, 2 = Alt + botão esquerdo,
// 3 = Win + botão esquerdo, 4 = botão direito. Um gancho de mouse de baixo nível, numa thread própria, segura o clique e avisa
// a thread de trabalho; se nada for escolhido, o clique é devolvido ao app.
static int g_mouseMode = 0;
static bool g_mouseCanceled = false;  // o menu foi cancelado com Esc: o clique não é devolvido
static bool g_resizeMouse = false;    // Alt + botão direito redimensiona a janela (opcional, mesmo gancho)
// O clique não foi sobre uma janela da grade: a thread de trabalho já devolveu o "pressionar" ao app e pede
// ao gancho que deixe passar o "soltar" verdadeiro. Assim, fora da grade, clicar e arrastar com o botão
// continuam funcionando normalmente.
static std::atomic<bool> g_mhPassUp{false};
static bool g_mouseDownInjected = false;  // só a thread de trabalho usa

static LRESULT CALLBACK RadialPrevProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_ERASEBKGND) {
        RECT rc{};
        GetClientRect(h, &rc);
        HBRUSH br = CreateSolidBrush(RadialSelColor());
        FillRect((HDC)w, &rc, br);
        DeleteObject(br);
        return 1;
    }
    return DefWindowProcW(h, m, w, l);
}

static bool RadialEnsureWindows() {
    static bool reg = false;
    HINSTANCE inst = GetModuleHandleW(nullptr);
    if (!reg) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = DefWindowProcW;
        wc.hInstance = inst;
        wc.lpszClassName = L"AdaptiveWindowsRadialRing";
        RegisterClassW(&wc);
        WNDCLASSW wp{};
        wp.style = CS_HREDRAW | CS_VREDRAW;
        wp.lpfnWndProc = RadialPrevProc;
        wp.hInstance = inst;
        wp.lpszClassName = L"AdaptiveWindowsRadialPreview";
        RegisterClassW(&wp);
        reg = true;
    }
    const DWORD ex = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST;
    if (!g_radial.ring)
        g_radial.ring = CreateWindowExW(ex, L"AdaptiveWindowsRadialRing", L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, inst, nullptr);
    if (!g_radial.prev) {
        g_radial.prev = CreateWindowExW(ex, L"AdaptiveWindowsRadialPreview", L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, inst, nullptr);
        if (g_radial.prev) {
            SetLayeredWindowAttributes(g_radial.prev, 0, 80, LWA_ALPHA);
            DWORD round = 2;  // DWMWCP_ROUND
            DwmSetWindowAttribute(g_radial.prev, 33, &round, sizeof(round));
        }
    }
    return g_radial.ring && g_radial.prev;
}

static void RadialDestroyWindows() {
    if (g_radial.ring) DestroyWindow(g_radial.ring);
    if (g_radial.prev) DestroyWindow(g_radial.prev);
    if (g_radial.bmp) DeleteObject(g_radial.bmp);
    if (g_radial.dc) DeleteDC(g_radial.dc);
    g_radial = Radial{};
}

// Tema vidro: copia o que está na tela sob o anel (antes de ele aparecer) e desfoca. O anel fica parado
// onde abriu, então uma cópia só basta; se a cópia falhar, usa um cinza neutro.
static void RadialCaptureBackdrop() {
    Radial& R = g_radial;
    const int S = RadialSize(R.scale);
    R.back.assign((size_t)S * S, 0x00808080);
    R.backSize = S;
    HDC scr = GetDC(nullptr);
    if (!scr) return;
    HDC mem = CreateCompatibleDC(scr);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = S;
    bi.bmiHeader.biHeight = -S;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = mem ? CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    if (bmp && bits) {
        HGDIOBJ old = SelectObject(mem, bmp);
        if (BitBlt(mem, 0, 0, S, S, scr, R.origin.x - S / 2, R.origin.y - S / 2, SRCCOPY)) {
            GdiFlush();
            memcpy(R.back.data(), bits, (size_t)S * S * sizeof(uint32_t));
        }
        SelectObject(mem, old);
    }
    if (bmp) DeleteObject(bmp);
    if (mem) DeleteDC(mem);
    ReleaseDC(nullptr, scr);

    // desfoque: média móvel na horizontal e na vertical, duas vezes (aproxima um desfoque gaussiano)
    const int rad = std::max(2, (int)std::lround(5 * R.scale));
    std::vector<uint32_t> tmp((size_t)S * S);
    auto pass = [&](const std::vector<uint32_t>& src, std::vector<uint32_t>& dst, bool horizontal) {
        for (int a = 0; a < S; a++) {
            int sr = 0, sg = 0, sb = 0, cnt = 0;
            auto at = [&](int k) { return horizontal ? src[(size_t)a * S + k] : src[(size_t)k * S + a]; };
            for (int k = 0; k <= rad && k < S; k++) {
                uint32_t v = at(k);
                sr += (v >> 16) & 0xFF;
                sg += (v >> 8) & 0xFF;
                sb += v & 0xFF;
                cnt++;
            }
            for (int k = 0; k < S; k++) {
                uint32_t o = ((uint32_t)(sr / cnt) << 16) | ((uint32_t)(sg / cnt) << 8) | (uint32_t)(sb / cnt);
                if (horizontal) dst[(size_t)a * S + k] = o;
                else dst[(size_t)k * S + a] = o;
                int in = k + rad + 1, out = k - rad;
                if (in < S) {
                    uint32_t v = at(in);
                    sr += (v >> 16) & 0xFF;
                    sg += (v >> 8) & 0xFF;
                    sb += v & 0xFF;
                    cnt++;
                }
                if (out >= 0) {
                    uint32_t v = at(out);
                    sr -= (v >> 16) & 0xFF;
                    sg -= (v >> 8) & 0xFF;
                    sb -= v & 0xFF;
                    cnt--;
                }
            }
        }
    };
    for (int rep = 0; rep < 2; rep++) {
        pass(R.back, tmp, true);
        pass(tmp, R.back, false);
    }
}

// Um pixel do anel, com as cores já multiplicadas pela transparência (formato do UpdateLayeredWindow).
static uint32_t RadialPack(double r, double g, double b, double a) {
    auto c8 = [](double v) { return (uint32_t)std::lround(std::min(255.0, std::max(0.0, v))); };
    return (c8(a * 255.0) << 24) | (c8(r * a) << 16) | (c8(g * a) << 8) | c8(b * a);
}

// Tema clássico: anel liso sobre um disco escuro. sel = o pixel está na fatia escolhida.
static uint32_t RadialPixelClassic(double d, bool sel, COLORREF ringCol, COLORREF selCol, double scale) {
    const double rad = RadialRad(scale), half = RadialHalf(scale);
    const double disc = rad + half + 6.0 * scale;  // disco escuro de fundo, para o anel aparecer sobre qualquer janela
    const double aBack = 0.38 * std::min(1.0, std::max(0.0, disc - d + 0.5));
    double aRing = std::min(1.0, std::max(0.0, half - std::fabs(d - rad) + 0.5));
    const COLORREF col = sel ? selCol : ringCol;
    aRing *= sel ? 1.0 : 0.80;
    const double A = aRing + aBack * (1.0 - aRing);
    if (A <= 0) return 0;
    // anel por cima do disco preto
    const double k = aRing / A;
    return RadialPack(GetRValue(col) * k, GetGValue(col) * k, GetBValue(col) * k, A);
}

// Tema vidro: o anel é um tubo de vidro. Dentro dele aparece o fundo desfocado e
// deslocado (refração), com brilho do lado da luz (em cima, à esquerda), sombra do lado oposto e um
// filete claro nas bordas. A fatia escolhida é tingida com a cor da seleção.
static uint32_t RadialPixelGlass(int x, int y, double dx, double dy, double d, bool sel, COLORREF ringCol, COLORREF selCol) {
    const Radial& R = g_radial;
    const int S = R.size;
    const double sc = R.scale;
    const double rad = RadialRad(sc), half = RadialHalf(sc);
    const double off = std::fabs(d - rad);
    const double cov = std::min(1.0, std::max(0.0, half - off + 0.5));
    // sombra suave em volta do tubo
    const double sh = off > half ? 0.26 * std::max(0.0, 1.0 - (off - half) / (8.0 * sc)) : 0.26;
    if (cov <= 0) return sh > 0.003 ? RadialPack(0, 0, 0, sh * sh / 0.26) : 0;
    const double ux = d > 0.001 ? dx / d : 0.0, uy = d > 0.001 ? dy / d : 0.0;
    const double t = std::min(1.0, std::max(-1.0, (d - rad) / half));  // -1 = borda interna, +1 = borda externa
    // refração: o vidro curvo mostra o fundo puxado em direção ao meio do tubo
    const double bend = -t * std::fabs(t) * 0.9 * half;
    const int sx = std::min(S - 1, std::max(0, (int)std::lround(x + ux * bend)));
    const int sy = std::min(S - 1, std::max(0, (int)std::lround(y + uy * bend)));
    const uint32_t bk = (R.backSize == S && !R.back.empty()) ? R.back[(size_t)sy * S + sx] : 0x00808080;
    double c[3] = {(double)((bk >> 16) & 0xFF), (double)((bk >> 8) & 0xFF), (double)(bk & 0xFF)};
    const double ring[3] = {(double)GetRValue(ringCol), (double)GetGValue(ringCol), (double)GetBValue(ringCol)};
    const double tint[3] = {(double)GetRValue(selCol), (double)GetGValue(selCol), (double)GetBValue(selCol)};
    // luz vinda de cima, à esquerda; a "inclinação" do vidro cresce do meio do tubo para as bordas
    const double facing = -(ux * t * 0.6 + uy * t * 0.8);
    const double hl = 0.60 * std::pow(std::max(0.0, facing), 1.5);
    const double sd = 0.30 * std::pow(std::max(0.0, -facing), 1.5);
    const double edge = 0.50 * std::min(1.0, std::max(0.0, 1.0 - (1.0 - std::fabs(t)) * half / (1.4 * sc)));
    for (int k = 0; k < 3; k++) {
        double v = c[k] * 0.80 + ring[k] * 0.20;          // vidro levemente fosco, na cor do anel
        if (sel) v = v * 0.36 + tint[k] * 0.64;           // fatia escolhida
        v *= 1.0 - sd;
        v += (255.0 - v) * hl;
        v += (255.0 - v) * edge;
        c[k] = v;
    }
    const double A = cov + sh * (1.0 - cov);
    const double k = cov / A;  // o vidro por cima da sombra preta
    return RadialPack(c[0] * k, c[1] * k, c[2] * k, A);
}

// Desenha o anel. dir = fatia destacada (-1 = nenhuma); ok = há uma janela vizinha naquela direção.
static void RadialDrawRing(int dir, bool ok) {
    Radial& R = g_radial;
    const int S = RadialSize(R.scale);
    if (!R.dc) R.dc = CreateCompatibleDC(nullptr);
    if (!R.bmp || R.size != S) {
        if (R.bmp) DeleteObject(R.bmp);
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = S;
        bi.bmiHeader.biHeight = -S;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        R.bmp = CreateDIBSection(R.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!R.bmp || !bits) return;
        R.bits = (uint32_t*)bits;
        R.size = S;
        SelectObject(R.dc, R.bmp);
    }
    const double c = S / 2.0;
    // ângulo central de cada direção (atan2 com y para baixo): cima, esquerda, direita, baixo
    static const double kCenter[4] = {-1.5707963, 3.1415927, 0.0, 1.5707963};
    const COLORREF ringCol = RadialRingColor();
    const COLORREF selCol = ok ? RadialSelColor() : RGB(150, 150, 150);
    const bool glass = g_cfg.radialTheme == 1;
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            const double dx = x + 0.5 - c, dy = y + 0.5 - c;
            const double d = std::sqrt(dx * dx + dy * dy);
            bool sel = false;
            if (dir >= 0) {
                double diff = std::fabs(std::atan2(dy, dx) - kCenter[dir]);
                if (diff > 3.1415927) diff = 6.2831853 - diff;
                sel = diff <= 0.7853982;
            }
            R.bits[(size_t)y * S + x] = glass ? RadialPixelGlass(x, y, dx, dy, d, sel, ringCol, selCol)
                                              : RadialPixelClassic(d, sel, ringCol, selCol, R.scale);
        }
    }
    POINT dst{R.origin.x - S / 2, R.origin.y - S / 2}, src{0, 0};
    SIZE sz{S, S};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(R.ring, nullptr, &dst, &sz, R.dc, &src, 0, &bf, ULW_ALPHA);
    SetWindowPos(R.ring, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    R.drawnDir = dir;
    R.drawnOk = ok;
}

// Fecha o menu. Retorna true se a janela foi movida.
static bool RadialEnd(bool apply) {
    Radial& R = g_radial;
    if (!R.active) return false;
    R.active = false;
    if (R.ring) ShowWindow(R.ring, SW_HIDE);
    if (R.prev) ShowWindow(R.prev, SW_HIDE);
    int i = FindItem(R.target);
    if (!apply || i < 0 || R.dir < 0) {
        Wh_Log(L"Menu radial: cancelado");
        return false;
    }
    int nb = FindNeighbor(i, R.dir);
    if (nb < 0) {
        Wh_Log(L"Menu radial: nao ha janela desse lado");
        return false;
    }
    Wh_Log(L"Menu radial: janela movida para %s", R.dir == 0 ? L"cima" : R.dir == 1 ? L"a esquerda" : R.dir == 2 ? L"a direita" : L"baixo");
    SwapItems((size_t)i, (size_t)nb);
    Retile();
    return true;
}

// Abre o menu para a janela da grade que está sob o cursor. Retorna true se abriu.
static bool RadialBegin() {
    Radial& R = g_radial;
    if (R.active) return false;
    POINT pt{};
    GetCursorPos(&pt);
    HWND under = WindowFromPoint(pt);
    if (under) under = GetAncestor(under, GA_ROOT);
    int i = FindItem(under);
    if (i < 0 || g_items[i].floating || IsZoomed(g_items[i].h)) return false;
    if (!RadialEnsureWindows()) return false;
    R.target = g_items[i].h;
    R.origin = pt;
    R.dir = -1;
    R.nb = -1;
    R.scale = MonScale(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
    R.active = true;
    R.shown = false;  // o anel só aparece se o botão continuar pressionado ou o mouse se mover (RadialTick)
    R.beganAt = NowMs();
    R.drawnDir = -2;
    return true;
}

static bool KeyIsDown(int vk);

// Chamada a cada volta do laço da thread de trabalho enquanto o menu está aberto.
static void RadialTick() {
    Radial& R = g_radial;
    if (!R.active) return;
    int i = FindItem(R.target);
    if (i < 0 || KeyIsDown(VK_ESCAPE)) {
        g_mouseCanceled = true;
        RadialEnd(false);
        return;
    }
    POINT pt{};
    GetCursorPos(&pt);
    const double dx = pt.x - R.origin.x, dy = pt.y - R.origin.y;
    if (!R.shown) {
        // Um clique rápido (para abrir o menu de contexto, por exemplo) não mostra o anel: ele aparece
        // depois de o botão ficar pressionado por um instante, ou assim que o mouse começa a se mover.
        const double moved = 6.0 * R.scale;
        if (NowMs() - R.beganAt < 180.0 && dx * dx + dy * dy < moved * moved) return;
        R.shown = true;
        if (g_cfg.radialTheme == 1) RadialCaptureBackdrop();  // antes de o anel aparecer na tela
        RadialDrawRing(-1, false);
    }
    const double dead = 22.0 * R.scale;  // dentro do centro do anel nada é escolhido
    int dir = -1;
    if (dx * dx + dy * dy >= dead * dead) {
        if (std::fabs(dx) >= std::fabs(dy)) dir = dx < 0 ? 1 : 2;
        else dir = dy < 0 ? 0 : 3;
    }
    int nb = dir >= 0 ? FindNeighbor(i, dir) : -1;
    R.dir = dir;
    const bool animated = RadialColorsAnimated();  // cores da borda com efeito (pulsar, arco-íris, degradê)
    if (dir != R.drawnDir || (nb >= 0) != R.drawnOk || (animated && dir >= 0)) RadialDrawRing(dir, nb >= 0);
    if (animated && nb >= 0 && nb == R.nb) InvalidateRect(R.prev, nullptr, TRUE);
    if (nb == R.nb) return;
    R.nb = nb;
    if (nb < 0) {
        ShowWindow(R.prev, SW_HIDE);
        return;
    }
    // destaca o espaço para onde a janela vai (a área visível da vizinha)
    const Item& o = g_items[nb];
    RECT v = o.to;
    v.left += o.lastInset.left;
    v.top += o.lastInset.top;
    v.right -= o.lastInset.right;
    v.bottom -= o.lastInset.bottom;
    SetWindowPos(R.prev, HWND_TOPMOST, v.left, v.top, v.right - v.left, v.bottom - v.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetWindowPos(R.ring, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);  // o anel fica por cima do destaque
}

// Devolve ao app um evento de botão que o gancho de mouse segurou. btn: 0 = esquerdo, 1 = meio, 2 = direito.
// Eventos injetados são ignorados pelo gancho.
static void InjectButton(int btn, bool down) {
    static const DWORD kDown[3] = {MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_RIGHTDOWN};
    static const DWORD kUp[3] = {MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTUP};
    INPUT in{};
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = down ? kDown[btn] : kUp[btn];
    SendInput(1, &in, sizeof(INPUT));
}

static void ReinjectClick(int btn) {
    InjectButton(btn, true);
    InjectButton(btn, false);
}

// O botão foi pressionado fora de uma janela da grade: devolve o "pressionar" ao app já, para que clicar e
// arrastar funcionem como sempre, e pede ao gancho que deixe passar o "soltar".
static void MousePassThrough(int btn) {
    InjectButton(btn, true);
    g_mouseDownInjected = true;
    g_mhPassUp = true;
}

// O gancho avisou do "soltar" depois de um clique devolvido ao app. Retorna true se o caso foi tratado aqui
// (o gancho segurou o "soltar" antes de ver o pedido para deixá-lo passar: devolve-o agora).
static bool MouseFinishPassThrough(int btn) {
    if (!g_mouseDownInjected) return false;
    g_mouseDownInjected = false;
    g_mhPassUp = false;
    InjectButton(btn, false);
    return true;
}

// O gancho de mouse avisou que o botão do menu radial foi pressionado (down) ou solto.
// O clique foi segurado pelo gancho; se o menu não moveu nada, ele é devolvido ao app aqui.
static void OnRadialMouse(bool down) {
    const int btn = g_mouseMode == 1 ? 1 : g_mouseMode == 4 ? 2 : 0;
    if (down) {
        g_mouseCanceled = false;
        g_mouseDownInjected = false;
        if (!RadialBegin()) MousePassThrough(btn);
        return;
    }
    if (MouseFinishPassThrough(btn)) return;
    const bool wasActive = g_radial.active;
    const bool chose = wasActive && g_radial.dir >= 0;
    const bool moved = RadialEnd(true);
    // não devolve o clique se a janela foi movida, se uma direção foi escolhida (mesmo sem vizinha) ou se foi cancelado com Esc
    if (moved || chose || g_mouseCanceled) return;
    ReinjectClick(btn);
}

// ---------------------------------------------------------------------------
// Redimensionar com Alt + botão direito
// ---------------------------------------------------------------------------
// Segurar Alt + botão direito sobre uma janela e arrastar: a lateral mais próxima do cursor (no eixo do
// arrasto) acompanha o mouse, para aumentar ou diminuir a janela. Ao soltar, a grade se ajusta como quando a borda da janela é arrastada (ApplyResize).

struct MouseResize {
    bool active = false;
    HWND h = nullptr;
    POINT origin{};
    RECT start{};   // retângulo da janela quando o botão foi pressionado
    RECT last{};    // retângulo pedido pelo cursor (a grade pode limitar o que a janela de fato ocupa)
    int edge = -1;  // lateral em movimento: 0 = cima, 1 = esquerda, 2 = direita, 3 = baixo; -1 = ainda não escolhida
    LONG minW = 0, minH = 0;
    double scale = 1.0;
};
static MouseResize g_mres;
static bool g_mresCanceled = false;

static void MouseResizeBegin() {
    MouseResize& R = g_mres;
    if (R.active || g_live.active) return;
    POINT pt{};
    GetCursorPos(&pt);
    HWND under = WindowFromPoint(pt);
    if (under) under = GetAncestor(under, GA_ROOT);
    int i = FindItem(under);
    if (i < 0 || IsZoomed(under)) return;
    Item& it = g_items[i];
    R = MouseResize{};
    R.h = under;
    R.origin = pt;
    GetWindowRect(under, &R.start);
    R.last = R.start;
    R.minW = std::max<LONG>(120, std::max(it.minW, it.lW));
    R.minH = std::max<LONG>(80, std::max(it.minH, it.lH));
    R.scale = MonScale(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
    R.active = true;
}

// Fecha o modo. apply = ajustar a grade ao novo tamanho; senão a janela volta ao espaço dela.
static void MouseResizeFinish(bool apply) {
    MouseResize& R = g_mres;
    if (!R.active) return;
    R.active = false;
    if (R.edge < 0) return;  // não chegou a arrastar: nada mudou
    g_dragHwnd = nullptr;
    int i = FindItem(R.h);
    if (i < 0) {
        LiveEnd();
        return;
    }
    Item& it = g_items[i];
    if (it.floating) {
        if (!apply) {
            SetWindowPos(R.h, nullptr, R.start.left, R.start.top, R.start.right - R.start.left, R.start.bottom - R.start.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
        }
        return;
    }
    // pesos finais calculados a partir da grade do início do arrasto (ou os do início, se cancelado)
    g_dragStart = R.start;
    if (LiveRestoreBase() && apply && ApplyResize((size_t)i, R.last)) {
        Wh_Log(L"Alt + botao direito: janela redimensionada, grade ajustada");
    } else {
        LiveRestoreBase();
    }
    LiveEnd();
    // as janelas já estão praticamente no lugar: a grade só confere e acerta o que faltar
    for (auto& o : g_items) {
        o.anim = false;
        o.to = RECT{0, 0, 0, 0};
    }
    ScheduleRetile(R.h, 30);
}

// Chamada a cada volta do laço da thread de trabalho enquanto o botão está pressionado.
static void MouseResizeTick() {
    MouseResize& R = g_mres;
    if (!R.active) return;
    int i = FindItem(R.h);
    if (i < 0) {
        R.active = false;
        g_dragHwnd = nullptr;
        LiveEnd();
        return;
    }
    if (KeyIsDown(VK_ESCAPE)) {
        g_mresCanceled = true;
        MouseResizeFinish(false);
        return;
    }
    const bool floating = g_items[i].floating;
    POINT pt{};
    GetCursorPos(&pt);
    const LONG dx = pt.x - R.origin.x, dy = pt.y - R.origin.y;
    if (R.edge < 0) {
        // O primeiro movimento escolhe o eixo (horizontal ou vertical); a lateral é a da metade da janela onde
        // o cursor estava. Assim, arrastar para fora aumenta a janela e arrastar para dentro diminui.
        const LONG thr = (LONG)std::lround(8 * R.scale);
        if (std::abs(dx) < thr && std::abs(dy) < thr) return;
        const bool horizontal = std::abs(dx) >= std::abs(dy);
        int edge, other;
        if (horizontal) {
            const bool rightHalf = R.origin.x >= (R.start.left + R.start.right) / 2;
            edge = rightHalf ? 2 : 1;
            other = rightHalf ? 1 : 2;
        } else {
            const bool bottomHalf = R.origin.y >= (R.start.top + R.start.bottom) / 2;
            edge = bottomHalf ? 3 : 0;
            other = bottomHalf ? 0 : 3;
        }
        // na grade, uma lateral junto da borda da tela não tem vizinha para ceder ou receber espaço:
        // usa a lateral oposta, se ela tiver vizinha
        if (!floating && FindNeighbor(i, edge) < 0 && FindNeighbor(i, other) >= 0) edge = other;
        R.edge = edge;
        g_items[i].anim = false;
        g_dragHwnd = R.h;  // outros eventos de grade não animam a janela enquanto ela é redimensionada
        if (!floating) LiveBegin(R.h, R.start, false);
    }
    RECT r = R.start;
    if (R.edge == 1) r.left = std::min(R.start.left + dx, r.right - R.minW);
    else if (R.edge == 2) r.right = std::max(R.start.right + dx, r.left + R.minW);
    else if (R.edge == 0) r.top = std::min(R.start.top + dy, r.bottom - R.minH);
    else r.bottom = std::max(R.start.bottom + dy, r.top + R.minH);
    const bool changed = !EqualRect(&r, &R.last);
    R.last = r;
    if (floating) {
        // janela solta: só ela muda de tamanho
        if (changed && !IsHungAppWindow(R.h)) {
            SetWindowPos(R.h, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
        }
        return;
    }
    // janela da grade: recalcula a grade para a posição do cursor e avança no ritmo das janelas
    if (changed && !LiveCompute(r)) {
        // a grade mudou durante o arrasto (janela aberta/fechada): encerra, e a grade se refaz sozinha
        R.active = false;
        g_dragHwnd = nullptr;
        LiveEnd();
        for (auto& o : g_items) o.to = RECT{0, 0, 0, 0};
        ScheduleRetile(R.h, 30);
        return;
    }
    LivePump();
}

// O gancho de mouse avisou que Alt + botão direito foi pressionado (down) ou solto.
static void OnResizeMouse(bool down) {
    if (down) {
        g_mresCanceled = false;
        g_mouseDownInjected = false;
        MouseResizeBegin();
        if (!g_mres.active) MousePassThrough(2);
        return;
    }
    if (MouseFinishPassThrough(2)) return;
    const bool dragged = g_mres.active && g_mres.edge >= 0;
    MouseResizeFinish(true);
    g_mres.active = false;
    // sem arrasto, foi só um Alt + clique direito: devolve o clique ao app
    if (!dragged && !g_mresCanceled) ReinjectClick(2);
}

struct HotkeyDef { int id; UINT mods, vk; };
static std::vector<HotkeyDef> g_keys;

static void OnHotkey(int id) {
    if (id == HK_ESC) {
        // Esc com o menu radial ou o redimensionamento pelo mouse em curso: cancela (a tecla não chega ao app)
        if (g_radial.active) {
            g_mouseCanceled = true;
            RadialEnd(false);
        }
        if (g_mres.active) {
            g_mresCanceled = true;
            MouseResizeFinish(false);
        }
        return;
    }
    HWND fg = GetForegroundWindow();
    switch (id) {
        case HK_FLOAT: {
            int i = FindItem(fg);
            {
                wchar_t cls[64] = L"", ttl[48] = L"";
                GetClassNameW(fg, cls, 64);
                GetWindowTextW(fg, ttl, 48);
                Wh_Log(L"Atalho soltar/prender: janela em foco [%s] \"%s\" %s", cls, ttl, i < 0 ? L"(fora da grade)" : (g_items[i].floating ? L"(solta)" : L"(na grade)"));
            }
            if (i < 0) {
                OnAdd(fg);
                i = FindItem(fg);
                if (i < 0) {
                    Wh_Log(L"Atalho soltar/prender: essa janela nao e elegivel para a grade (veja 'Ignorada' no log)");
                    return;
                }
            } else {
                g_items[i].floating = !g_items[i].floating;
            }
            if (!g_items[i].floating) g_items[i].to = RECT{0, 0, 0, 0};
            Retile();
            break;
        }
        case HK_MONOCLE:
            g_monocle = !g_monocle;
            for (auto& it : g_items) it.to = RECT{0, 0, 0, 0};
            Retile();
            break;
        case HK_LAYOUT: {
            int cur = g_layoutRt >= 0 ? g_layoutRt : g_cfg.layout;
            g_layoutRt = (cur + 1) % 6;
            ResetWeights();
            for (auto& it : g_items) { it.lW = it.lH = 0; it.learned = 0; }  // descarta mínimos aprendidos (podem ter sido tamanhos transitórios)
            for (auto& it : g_items) it.to = RECT{0, 0, 0, 0};
            Retile();
            break;
        }
        case HK_UP: SwapFocusedDir(0); break;
        case HK_LEFT: SwapFocusedDir(1); break;
        case HK_RIGHT: SwapFocusedDir(2); break;
        case HK_DOWN: SwapFocusedDir(3); break;
        case HK_RETILE:
            ResetWeights();
            for (auto& it : g_items) { it.lW = it.lH = 0; it.learned = 0; }
            for (auto& it : g_items) { it.floating = false; it.to = RECT{0, 0, 0, 0}; }
            Retile();
            break;
    }
}

// ---------------------------------------------------------------------------
// Thread de trabalho
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Ritmo dos quadros
// ---------------------------------------------------------------------------

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

static HANDLE g_frameTimer = nullptr;
static double g_nextFrame = 0;

static BOOL CALLBACK MaxHzProc(HMONITOR m, HDC, LPRECT, LPARAM lp) {
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(m, &mi)) return TRUE;
    DEVMODEW dm{};
    dm.dmSize = sizeof(dm);
    if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency > 1) {
        int* best = (int*)lp;
        *best = std::max<int>(*best, (int)dm.dmDisplayFrequency);
    }
    return TRUE;
}

// Intervalo entre quadros em ms; 0 = sincronizar com o DWM.
static double FrameIntervalMs() {
    int hz = g_cfg.hz;
    if (hz < 0) {
        int best = 0;
        EnumDisplayMonitors(nullptr, nullptr, MaxHzProc, (LPARAM)&best);
        hz = best >= 30 ? best : 60;
        Wh_Log(L"Maior taxa de atualizacao detectada: %d Hz", hz);
    }
    return hz > 0 ? 1000.0 / hz : 0.0;
}

static double g_intervalMs = 0;  // calculado ao iniciar e quando as configurações mudam
static void UpdateInterval() { g_intervalMs = FrameIntervalMs(); }

static void WaitFrame() {
    double interval = g_intervalMs;
    if (interval <= 0.0) {
        g_nextFrame = 0;
        if (FAILED(DwmFlush())) Sleep(8);  // DWM indisponível
        return;
    }
    double now = NowMs();
    if (g_nextFrame < now - interval * 4) g_nextFrame = now;  // atrasou muito: ressincroniza
    g_nextFrame += interval;
    double wait = g_nextFrame - NowMs();
    if (wait <= 0.2) return;
    if (g_frameTimer) {
        LARGE_INTEGER due;
        due.QuadPart = -(LONGLONG)(wait * 10000.0);  // 100 ns, relativo
        if (SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr, FALSE)) {
            WaitForSingleObject(g_frameTimer, 100);
            return;
        }
    }
    Sleep((DWORD)wait);
}

// Estatísticas (só aparecem no log do Windhawk, quando o registro está ligado)
static struct {
    int frames = 0;
    double start = 0, last = 0, maxDt = 0, maxCost = 0;
    double maxPump = 0, maxPend = 0, maxVerify = 0, maxWait = 0;
} g_stats;

static void StatsFrame(double begin, double cost) {
    if (g_stats.frames == 0) g_stats.start = begin;
    else g_stats.maxDt = std::max(g_stats.maxDt, begin - g_stats.last);
    g_stats.last = begin;
    g_stats.maxCost = std::max(g_stats.maxCost, cost);
    g_stats.frames++;
}

static void StatsFlush() {
    if (g_stats.frames > 1) {
        double total = g_stats.last - g_stats.start;
        Wh_Log(L"Animacao: %d quadros em %.0f ms (media %.1f ms/quadro, pior intervalo %.1f ms, pior custo de mover janelas %.1f ms; fases mais lentas: mensagens %.1f, novas %.1f, conferencia %.1f, espera %.1f ms)",
               g_stats.frames, total, total / (g_stats.frames - 1), g_stats.maxDt, g_stats.maxCost,
               g_stats.maxPump, g_stats.maxPend, g_stats.maxVerify, g_stats.maxWait);
    }
    g_stats = {};
}

static BOOL CALLBACK EnumProc(HWND h, LPARAM) {
    if (IsManageable(h) && FindItem(h) < 0) {
        Item it;
        it.h = h;
        it.fresh = false;  // janelas que já existiam não "entram"; só se ajustam
        it.mon = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
        GetWindowRect(h, &it.cur);
        it.to = RECT{0, 0, 0, 0};
        if (IsZoomed(h)) {
            // já estava maximizada quando o mod carregou: devolve ao modo normal para entrar na grade
            it.zoomed = true;
            if (!g_cfg.keepMaximized) ShowWindowAsync(h, SW_RESTORE);
        }
        {
            wchar_t cls[64] = L"", ttl[48] = L"";
            GetClassNameW(h, cls, 64);
            GetWindowTextW(h, ttl, 48);
            Wh_Log(L"Janela ja aberta [%s] \"%s\"%s", cls, ttl, it.zoomed ? L" (estava maximizada)" : L"");
        }
        it.seq = ++g_seqNext;
        g_items.push_back(it);
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// Mudanças de tela (monitor conectado/removido, resolução, barra de tarefas movida ou com ocultar
// automaticamente): o Windows avisa por mensagens enviadas a janelas de nível superior, então a
// thread de trabalho tem uma janela invisível só para recebê-las e refazer a grade.
// ---------------------------------------------------------------------------

static ULONGLONG g_displayAt = 0;  // quando refazer a grade depois de uma mudança de tela (0 = nada pendente)
static HWND g_watcher = nullptr;

static LRESULT CALLBACK WatcherProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DISPLAYCHANGE || (m == WM_SETTINGCHANGE && w == SPI_SETWORKAREA)) {
        g_displayAt = GetTickCount64() + 400;  // espera o Windows terminar de reposicionar as janelas
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void CreateWatcher() {
    WNDCLASSW wc{};
    wc.lpfnWndProc = WatcherProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"AdaptiveWindowsDisplayWatcher";
    RegisterClassW(&wc);
    g_watcher = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, wc.hInstance, nullptr);
    if (!g_watcher) Wh_Log(L"Nao foi possivel criar a janela que observa mudancas de tela (erro %lu)", GetLastError());
}

static void DestroyWatcher() {
    if (g_watcher) { DestroyWindow(g_watcher); g_watcher = nullptr; }
    UnregisterClassW(L"AdaptiveWindowsDisplayWatcher", GetModuleHandleW(nullptr));
}

static void RunDisplayRetile() {
    if (!g_displayAt || GetTickCount64() < g_displayAt) return;
    g_displayAt = 0;
    // o monitor guardado em cada janela pode ter deixado de existir: relê de onde a janela está agora
    for (auto& it : g_items) {
        it.mon = MonitorFromWindow(it.h, MONITOR_DEFAULTTONEAREST);
        it.to = RECT{0, 0, 0, 0};
    }
    for (auto& e : g_minimized) {
        MONITORINFO mi{sizeof(mi)};
        if (!GetMonitorInfoW(e.mon, &mi)) e.mon = MonitorFromWindow(e.h, MONITOR_DEFAULTTOPRIMARY);
        e.last = RECT{0, 0, 0, 0};
    }
    Wh_Log(L"Mudanca de tela detectada: refazendo a grade");
    Retile();
}

// Atalhos que o RegisterHotKey recusou (erro 1409: o Windows ou outro programa já os registrou, como o
// Win+Alt+Seta para cima). Quando a opção está ligada, um gancho de teclado de baixo nível os captura
// antes do sistema e avisa a thread de trabalho.
// O gancho roda numa thread própria que só bombeia mensagens: cada tecla do sistema espera o retorno do
// callback, e a thread de trabalho passa longos trechos sem bombear (animação, SendMessageTimeout...).
static const UINT WM_APP_LLHK = WM_APP + 2;
static std::vector<HotkeyDef> g_hookKeys;  // só a thread do gancho lê; quem as escreve é a de trabalho, com o gancho parado
static DWORD g_hookTarget = 0;             // thread de trabalho, que recebe WM_APP_LLHK
static int g_hookHeldId = 0;               // só a thread do gancho usa (evita repetir ao segurar a tecla)
static HANDLE g_hookThreadH = nullptr;
static DWORD g_hookThreadId = 0;

static bool KeyIsDown(int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; }

static LRESULT CALLBACK LowLevelKbProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT* k = (const KBDLLHOOKSTRUCT*)lp;
        if (!(k->flags & LLKHF_INJECTED)) {  // ignora o que o próprio mod injeta
            const bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
            const bool up = wp == WM_KEYUP || wp == WM_SYSKEYUP;
            for (const auto& hk : g_hookKeys) {
                if (k->vkCode != hk.vk) continue;
                if (down) {
                    UINT now = 0;
                    if (KeyIsDown(VK_CONTROL)) now |= MOD_CONTROL;
                    if (KeyIsDown(VK_MENU)) now |= MOD_ALT;
                    if (KeyIsDown(VK_SHIFT)) now |= MOD_SHIFT;
                    if (KeyIsDown(VK_LWIN) || KeyIsDown(VK_RWIN)) now |= MOD_WIN;
                    if (now != (hk.mods & 0xF)) continue;  // só vale com exatamente esses modificadores
                    if (g_hookHeldId != hk.id) {
                        g_hookHeldId = hk.id;
                        PostThreadMessageW(g_hookTarget, WM_APP_LLHK, (WPARAM)hk.id, 0);
                        // tecla "vazia" no meio da combinação: impede que soltar Win/Alt abra o menu Iniciar ou a barra de menus
                        INPUT in[2] = {};
                        in[0].type = in[1].type = INPUT_KEYBOARD;
                        in[0].ki.wVk = in[1].ki.wVk = 0xFF;
                        in[1].ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(2, in, sizeof(INPUT));
                    }
                    return 1;  // engole a tecla: o Windows não a recebe
                }
                if (up && g_hookHeldId == hk.id) { g_hookHeldId = 0; return 1; }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

struct HookStart { HANDLE ready; bool ok; };

static DWORD WINAPI HookThread(LPVOID param) {
    HookStart* st = (HookStart*)param;
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // cria a fila de mensagens antes de avisar
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKbProc, GetModuleHandleW(nullptr), 0);
    if (!hook) Wh_Log(L"Nao foi possivel instalar o gancho de teclado (erro %lu)", GetLastError());
    st->ok = hook != nullptr;
    SetEvent(st->ready);  // st deixa de valer a partir daqui
    if (!hook) return 0;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(hook);
    return 0;
}

// Chamadas só pela thread de trabalho. g_hookKeys já deve estar preenchida.
static void StartHookThread() {
    g_hookTarget = GetCurrentThreadId();
    g_hookHeldId = 0;
    HookStart st{CreateEventW(nullptr, TRUE, FALSE, nullptr), false};
    if (!st.ready) { g_hookKeys.clear(); return; }
    g_hookThreadH = CreateThread(nullptr, 0, HookThread, &st, 0, &g_hookThreadId);
    if (g_hookThreadH) WaitForSingleObject(st.ready, INFINITE);  // a thread só instala o gancho e sinaliza
    CloseHandle(st.ready);
    if (!g_hookThreadH || !st.ok) {
        if (g_hookThreadH) { WaitForSingleObject(g_hookThreadH, INFINITE); CloseHandle(g_hookThreadH); }
        g_hookThreadH = nullptr;
        g_hookKeys.clear();
    }
}

static void StopHookThread() {
    if (g_hookThreadH) {
        do {
            PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        } while (WaitForSingleObject(g_hookThreadH, 100) == WAIT_TIMEOUT);
        CloseHandle(g_hookThreadH);
        g_hookThreadH = nullptr;
    }
    g_hookKeys.clear();
    g_hookHeldId = 0;
}

// ---- gancho de mouse do menu radial (só existe quando a opção está ligada) ----
static const UINT WM_APP_MOUSE = WM_APP + 3;
// Retângulos das janelas que o mod administra, para o gancho decidir na hora: um clique fora de todos eles
// passa intocado (sem ser segurado e reenviado). A thread de trabalho atualiza a lista; o gancho só lê.
static std::mutex g_mhRectsLock;
static std::vector<RECT> g_mhRects;

static bool MousePointOverManaged(POINT pt) {
    std::lock_guard<std::mutex> lock(g_mhRectsLock);
    for (const RECT& r : g_mhRects)
        if (PtInRect(&r, pt)) return true;
    return false;
}

static int g_mhMode = 0;         // cópias de g_mouseMode / g_resizeMouse lidas pela thread do gancho
static bool g_mhResize = false;  // (escritas com o gancho parado)
static WPARAM g_mhUpMsg = 0;     // só a thread do gancho usa: mensagem de "soltar" do botão que ela segurou (0 = nenhum)
static WPARAM g_mhUpCode = 0;    // o que avisar à thread de trabalho quando esse botão for solto
static DWORD g_mhTarget = 0;
static HANDLE g_mhThreadH = nullptr;
static DWORD g_mhThreadId = 0;

// Alt ou Win pressionado e solto sem nenhuma outra tecla abre a barra de menus do app ou o menu Iniciar.
// Como o clique que viria no meio foi segurado, injeta uma tecla "vazia" para o Windows não ver a tecla sozinha.
static void MouseHookDummyKey() {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD;
    in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = 0xFF;
    in[1].ki.wVk = 0xFF;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

// Avisos à thread de trabalho (wParam de WM_APP_MOUSE): 1/0 = menu radial pressionado/solto, 3/2 = redimensionar.
static LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION) return CallNextHookEx(nullptr, code, wp, lp);
    const MSLLHOOKSTRUCT* m = (const MSLLHOOKSTRUCT*)lp;
    if (m->flags & LLMHF_INJECTED) return CallNextHookEx(nullptr, code, wp, lp);
    if (g_mhUpMsg && wp == g_mhUpMsg) {
        g_mhUpMsg = 0;
        // clique fora da grade, já devolvido ao app: o "soltar" verdadeiro segue normalmente
        if (g_mhPassUp.exchange(false)) return CallNextHookEx(nullptr, code, wp, lp);
        PostThreadMessageW(g_mhTarget, WM_APP_MOUSE, g_mhUpCode, 0);
        return 1;
    }
    if (g_mhUpMsg || (wp != WM_LBUTTONDOWN && wp != WM_MBUTTONDOWN && wp != WM_RBUTTONDOWN)) {
        return CallNextHookEx(nullptr, code, wp, lp);
    }
    // fora das janelas administradas (área de trabalho, barra de tarefas, outros apps) o clique segue intocado
    if (!MousePointOverManaged(m->pt)) return CallNextHookEx(nullptr, code, wp, lp);
    UINT now = 0;
    if (KeyIsDown(VK_CONTROL)) now |= MOD_CONTROL;
    if (KeyIsDown(VK_MENU)) now |= MOD_ALT;
    if (KeyIsDown(VK_SHIFT)) now |= MOD_SHIFT;
    if (KeyIsDown(VK_LWIN) || KeyIsDown(VK_RWIN)) now |= MOD_WIN;
    if (g_mhResize && wp == WM_RBUTTONDOWN && now == MOD_ALT) {
        g_mhUpMsg = WM_RBUTTONUP;
        g_mhUpCode = 2;
        PostThreadMessageW(g_mhTarget, WM_APP_MOUSE, 3, 0);
        MouseHookDummyKey();
        return 1;  // segura o clique; ele é devolvido depois se não houver arrasto
    }
    WPARAM wantDown = 0, wantUp = 0;
    UINT wantMods = 0;
    if (g_mhMode == 1) {
        wantDown = WM_MBUTTONDOWN;
        wantUp = WM_MBUTTONUP;
    } else if (g_mhMode == 2 || g_mhMode == 3) {
        wantDown = WM_LBUTTONDOWN;
        wantUp = WM_LBUTTONUP;
        wantMods = g_mhMode == 2 ? MOD_ALT : MOD_WIN;
    } else if (g_mhMode == 4) {
        wantDown = WM_RBUTTONDOWN;
        wantUp = WM_RBUTTONUP;
    }
    if (wantDown && wp == wantDown && now == wantMods) {
        g_mhUpMsg = wantUp;
        g_mhUpCode = 0;
        PostThreadMessageW(g_mhTarget, WM_APP_MOUSE, 1, 0);
        if (wantMods) MouseHookDummyKey();
        return 1;  // segura o clique; ele é devolvido depois se nada for escolhido
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static DWORD WINAPI MouseHookThread(LPVOID param) {
    HookStart* st = (HookStart*)param;
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    HHOOK hook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandleW(nullptr), 0);
    if (!hook) Wh_Log(L"Nao foi possivel instalar o gancho de mouse (erro %lu)", GetLastError());
    st->ok = hook != nullptr;
    SetEvent(st->ready);  // st deixa de valer a partir daqui
    if (!hook) return 0;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(hook);
    return 0;
}

static void StopMouseHook() {
    if (g_mhThreadH) {
        do {
            PostThreadMessageW(g_mhThreadId, WM_QUIT, 0, 0);
        } while (WaitForSingleObject(g_mhThreadH, 100) == WAIT_TIMEOUT);
        CloseHandle(g_mhThreadH);
        g_mhThreadH = nullptr;
    }
    g_mhUpMsg = 0;
    g_mhMode = 0;
    g_mhResize = false;
    g_mhPassUp = false;
}

// Atualiza a lista de retângulos lida pelo gancho de mouse: onde cada janela administrada está e para onde
// está indo. Chamada pela thread de trabalho a cada volta do laço (no máximo ~20 vezes por segundo).
static void RefreshHookRects(bool force = false) {
    if (!force && !g_mhThreadH) return;
    static double last = 0;
    const double now = NowMs();
    if (!force && now - last < 50.0) return;
    last = now;
    std::vector<RECT> rects;
    rects.reserve(g_items.size());
    for (const auto& it : g_items) {
        RECT r{};
        if (IsIconic(it.h) || !GetWindowRect(it.h, &r)) continue;
        if (it.to.right > it.to.left) UnionRect(&r, &r, &it.to);
        rects.push_back(r);
    }
    std::lock_guard<std::mutex> lock(g_mhRectsLock);
    g_mhRects.swap(rects);
}

// Chamada só pela thread de trabalho, com o gancho parado.
static void StartMouseHook() {
    if (g_mouseMode == 0 && !g_resizeMouse) return;
    RefreshHookRects(true);
    g_mhMode = g_mouseMode;
    g_mhResize = g_resizeMouse;
    g_mhTarget = GetCurrentThreadId();
    HookStart st{CreateEventW(nullptr, TRUE, FALSE, nullptr), false};
    if (!st.ready) return;
    g_mhThreadH = CreateThread(nullptr, 0, MouseHookThread, &st, 0, &g_mhThreadId);
    if (g_mhThreadH) WaitForSingleObject(st.ready, INFINITE);
    CloseHandle(st.ready);
    if (g_mhThreadH && !st.ok) {
        WaitForSingleObject(g_mhThreadH, INFINITE);
        CloseHandle(g_mhThreadH);
        g_mhThreadH = nullptr;
    }
    if (g_mhThreadH) Wh_Log(L"Gancho de mouse ligado (menu radial: modo %d; redimensionar com Alt + botao direito: %d)", g_mouseMode, (int)g_resizeMouse);
}

static void UnregisterHotkeys() {
    for (auto& k : g_keys) UnregisterHotKey(nullptr, k.id);
    g_keys.clear();
    StopHookThread();
    StopMouseHook();
}

// Registra os atalhos a partir das configurações; pode ser chamada de novo quando elas mudam.
static void RegisterHotkeys() {
    UnregisterHotkeys();
    std::wstring hk = GetStr(L"hotkeys");
    UINT mods = MOD_WIN | MOD_ALT | MOD_NOREPEAT;
    if (hk == L"ctrlalt") mods = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
    else if (hk == L"altshift") mods = MOD_ALT | MOD_SHIFT | MOD_NOREPEAT;
    else if (hk == L"ctrlaltshift") mods = MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT;
    g_keys = {{HK_MONOCLE, mods, 'F'}, {HK_LEFT, mods, VK_LEFT}, {HK_RIGHT, mods, VK_RIGHT},
              {HK_UP, mods, VK_UP}, {HK_DOWN, mods, VK_DOWN}};
    {
        UINT m2, k2;
        if (ParseHotkey(GetStr(L"layoutHotkey"), &m2, &k2)) g_keys.push_back({HK_LAYOUT, m2, k2});
        else if (!GetStr(L"layoutHotkey").empty()) Wh_Log(L"Atalho de layout invalido: use algo como Win+Ctrl+G");
        if (ParseHotkey(GetStr(L"retileHotkey"), &m2, &k2)) g_keys.push_back({HK_RETILE, m2, k2});
        else if (!GetStr(L"retileHotkey").empty()) Wh_Log(L"Atalho de reorganizar invalido: use algo como Alt+Home");
        if (ParseHotkey(GetStr(L"floatHotkey"), &m2, &k2)) g_keys.push_back({HK_FLOAT, m2, k2});
        else if (!GetStr(L"floatHotkey").empty()) Wh_Log(L"Atalho de soltar janela invalido: use algo como Win+Ctrl+T");
    }
    const bool force = Wh_GetIntSetting(L"forceHotkeys") != 0;
    for (auto& k : g_keys) {
        if (RegisterHotKey(nullptr, k.id, k.mods, k.vk)) {
            Wh_Log(L"Atalho registrado (id %d, tecla %u, modificadores %x)", k.id, k.vk, k.mods & 0xff);
        } else {
            DWORD err = GetLastError();
            if (err == ERROR_HOTKEY_ALREADY_REGISTERED && force) {
                g_hookKeys.push_back(k);
                Wh_Log(L"Atalho (id %d, tecla %u, modificadores %x) ja usado por outro programa: assumido pelo gancho de teclado",
                       k.id, k.vk, k.mods & 0xff);
            } else
                Wh_Log(L"Atalho (tecla %u) indisponivel (erro %lu; 1409 = ja registrado por outro programa/instancia; "
                       L"a opcao 'Assumir atalhos' permite ao mod assumi-lo)",
                       k.vk, err);
        }
    }
    if (!g_hookKeys.empty()) StartHookThread();
    {
        std::wstring rm = GetStr(L"radialMouse");
        g_mouseMode = rm == L"middle" ? 1 : rm == L"altleft" ? 2 : rm == L"winleft" ? 3 : rm == L"right" ? 4 : 0;
        g_resizeMouse = Wh_GetIntSetting(L"resizeMouse") != 0;
        StartMouseHook();
    }
}

static DWORD WINAPI Worker(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // cria a fila de mensagens

    HWINEVENTHOOK hooks[6] = {};
    hooks[0] = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[1] = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[2] = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[3] = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);  // DESTROY, SHOW, HIDE
    hooks[4] = SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[5] = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    RegisterHotkeys();

    UpdateInterval();
    g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!g_frameTimer) g_frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);

    CreateWatcher();
    EnumWindows(EnumProc, 0);
    Retile();
    UpdateBorder(GetForegroundWindow());

    // Loop: quando há animação, cada quadro é sincronizado com a atualização da tela (DwmFlush);
    // quando não há, a thread dorme até chegar uma mensagem.
    while (!g_stop) {
        double tA = NowMs();
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { g_stop = true; break; }
            if (msg.message == WM_HOTKEY || msg.message == WM_APP_LLHK) {
                OnHotkey((int)msg.wParam);
            } else if (msg.message == WM_APP_MOUSE) {
                if (msg.wParam >= 2) OnResizeMouse(msg.wParam == 3);
                else OnRadialMouse(msg.wParam == 1);
            } else if (msg.message == WM_APP_RELOAD) {
                Wh_Log(L"Configuracoes alteradas: recarregando");
                RadialEnd(false);
                MouseResizeFinish(false);
                if (g_live.active) {
                    LiveRestoreBase();
                    LiveEnd();
                }
                const bool keptMaximized = g_cfg.keepMaximized;
                LoadSettings();  // g_cfg and friends are only touched by this thread after startup
                UpdateInterval();
                RegisterHotkeys();  // atalhos novos valem na hora, sem reiniciar o mod
                {
                    // a lista de janelas ignoradas mudou: tira da grade as que passaram a constar nela...
                    std::vector<HWND> drop;
                    for (const auto& it : g_items) {
                        wchar_t cls[64] = L"";
                        GetClassNameW(it.h, cls, 64);
                        if (IsIgnored(it.h, cls)) drop.push_back(it.h);
                    }
                    for (HWND h : drop) {
                        OnRemove(h);  // (registra a janela como "minimizada"; aqui ela só foi ignorada)
                        g_minimized.erase(std::remove_if(g_minimized.begin(), g_minimized.end(),
                                                         [&](const MinEntry& e) { return e.h == h; }),
                                          g_minimized.end());
                    }
                    EnumWindows(EnumProc, 0);  // ...e traz as que deixaram de constar
                    // só devolve as maximizadas à grade se foi essa a opção que o usuário acabou de desligar;
                    // salvar qualquer outra configuração não mexe nelas
                    if (keptMaximized && !g_cfg.keepMaximized)
                        for (const auto& it : g_items)
                            if (!it.floating && IsZoomed(it.h)) ShowWindowAsync(it.h, SW_RESTORE);
                }
                ResetWeights();
                for (auto& it : g_items) it.to = RECT{0, 0, 0, 0};
                UpdateBorder(GetForegroundWindow());
                Retile();
            } else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        if (g_stop) break;

        double tB = NowMs();
        ProcessPending();
        double tC = NowMs();
        bool verifyPending = VerifyItems();
        double tD = NowMs();
        TickBorder();
        RadialTick();
        MouseResizeTick();
        {
            // Enquanto o menu radial ou o redimensionamento pelo mouse está em curso, Esc é registrado como
            // atalho global: assim ele cancela a ação sem chegar ao app que está sob o cursor.
            static bool wanted = false, grabbed = false;
            const bool need = g_radial.active || g_mres.active;
            if (need != wanted) {
                wanted = need;
                if (need) {
                    grabbed = RegisterHotKey(nullptr, HK_ESC, 0, VK_ESCAPE) != 0;
                } else if (grabbed) {
                    UnregisterHotKey(nullptr, HK_ESC);
                    grabbed = false;
                }
            }
        }
        RefreshHookRects();
        NativeLiveTick();
        RunScheduledRetile();
        RunDisplayRetile();
        if (AnyAnimating()) {
            double t0 = NowMs();
            StepAnimations();
            StatsFrame(t0, NowMs() - t0);
            g_stats.maxPump = std::max(g_stats.maxPump, tB - tA);
            g_stats.maxPend = std::max(g_stats.maxPend, tC - tB);
            g_stats.maxVerify = std::max(g_stats.maxVerify, tD - tC);
            double tW = NowMs();
            WaitFrame();
            g_stats.maxWait = std::max(g_stats.maxWait, NowMs() - tW);
        } else {
            StatsFlush();
            g_nextFrame = 0;
            DWORD wait = (g_pending.empty() && !verifyPending) ? INFINITE : 20;
            if (BorderAnimated()) wait = std::min<DWORD>(wait, 16);
            if (g_retileAt) wait = std::min<DWORD>(wait, 15);
            if (g_displayAt) wait = std::min<DWORD>(wait, 50);
            if (g_radial.active || g_mres.active) wait = std::min<DWORD>(wait, 16);
            if (g_live.active) wait = std::min<DWORD>(wait, 8);
            MsgWaitForMultipleObjectsEx(0, nullptr, wait, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        }
    }

    // limpeza
    if (g_frameTimer) { CloseHandle(g_frameTimer); g_frameTimer = nullptr; }
    UnregisterHotkeys();
    for (auto h : hooks) if (h) UnhookWinEvent(h);
    for (auto& it : g_items) {
        SetBorder(it.h, false);
        if (it.cornerApplied >= 0 && it.cornerApplied != it.origCorner) {
            DWORD v = (DWORD)it.origCorner;
            DwmSetWindowAttribute(it.h, 33, &v, sizeof(v));
        }
    }
    DestroyOverlay();
    RadialEnd(false);
    MouseResizeFinish(false);
    RadialDestroyWindows();
    DestroyWatcher();
    // janelas ainda minimizadas voltam a ter a posição normal que tinham antes do mod
    for (auto& e : g_minimized) {
        WINDOWPLACEMENT wp{sizeof(wp)};
        if (e.haveOrig && IsWindow(e.h) && IsIconic(e.h) && GetWindowPlacement(e.h, &wp)) {
            wp.rcNormalPosition = e.orig;
            wp.flags |= WPF_ASYNCWINDOWPLACEMENT;
            wp.showCmd = SW_SHOWMINNOACTIVE;
            SetWindowPlacement(e.h, &wp);
        }
    }
    g_minimized.clear();
    g_items.clear();
    g_pending.clear();
    return 0;
}

// ---------------------------------------------------------------------------
// Pontos de entrada da mod (tool mod: roda no processo windhawk.exe dedicado)
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Adaptive Windows " WH_MOD_VERSION L" carregado");
    LoadSettings();
    g_stop = false;
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    // settings are re-read on the worker thread (WM_APP_RELOAD), so no state is shared between threads
    if (g_thread) PostThreadMessageW(g_threadId, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    g_stop = true;
    if (g_thread) {
        // Repete até a fila de mensagens da thread existir; depois espera ela terminar, sem limite de tempo
        // (descarregar a DLL com a thread ainda rodando derrubaria o processo).
        do {
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        } while (WaitForSingleObject(g_thread, 100) == WAIT_TIMEOUT);
        CloseHandle(g_thread);
        g_thread = nullptr;
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