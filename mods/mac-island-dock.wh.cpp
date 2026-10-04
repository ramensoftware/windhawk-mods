// ==WindhawkMod==
// @id              mac-island-dock
// @name            Mac Island & Dock
// @name:pt-BR      Ilha e Dock estilo Mac
// @description     An island at the top of the screen like the macOS menu bar and the iPhone's Dynamic Island (clock, control center, notifications, media, tray icons), and a macOS-like dock instead of the taskbar (or the Windows taskbar, transparent or as it is)
// @description:pt-BR Uma ilha no topo da tela como a barra de menus do macOS e a Dynamic Island do iPhone (relógio, central de controle, notificações, mídia, ícones da bandeja) e uma dock como a do macOS no lugar da barra de tarefas (ou a barra do Windows, transparente ou como ela é)
// @version         0.42.3
// @author          caliberda
// @github          https://github.com/cesarkali
// @homepage        https://caliberda.com.br
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -ldwmapi -lcomctl32 -lgdi32 -lmsimg32 -ld2d1 -ldwrite -lwlanapi -luuid -lwindowscodecs -lversion -lbthprops -ldxva2 -lshlwapi -lwinhttp
// @license         GPL-3.0
// ==/WindhawkMod==

// The code that reaches the taskbar's XAML tree is based on "Taskbar tray
// auto-hide (show on hover)" by m417z, published under the GNU General Public
// License v3.0, so this mod uses the same license.

// ==WindhawkModReadme==
/*
# Mac Island & Dock

**English** | [Português](#português)

The top of macOS and the iPhone's Dynamic Island on Windows 11: an island at
the top of the screen with the clock, a control center, notifications and
what's playing; and, if you want it, a dock like the one on macOS instead of
the taskbar.

| The control center and the settings | The calendar and a notification |
| :---: | :---: |
| ![The control center, editing it, and the island's settings](https://i.imgur.com/x2JTfH9.gif) | ![The calendar, and a new notification answered from the pill](https://i.imgur.com/tSf46GE.gif) |

## The island

* **Always at the top**: the clock, network, volume, battery and
  notifications, always visible (except in full screen): a black pill at the
  top center, which can be minimized to a thin line, or a full-width bar
  like the macOS menu bar, which keeps maximized windows below it. Apps with unread notifications show up on its left, with
  a red dot, even while the taskbar is hidden. The mouse wheel on the volume
  changes it, and the bell turns into a moon while "Do not disturb" is on.
* **Panels like the Dynamic Island**: the pill itself stretches into a panel,
  sideways and then down, with a little bounce:
  * **Calendar** (the clock).
  * **Control center** (the network, volume or battery), like the one on
    macOS: Wi-Fi (on/off, the networks in reach, joining with a password),
    Bluetooth (on/off, the paired devices), "Do not disturb", night light,
    project (the four display modes), cast, live captions, accessibility
    (magnifier, narrator, color filters, sticky keys), the display's
    brightness (monitors with DDC/CI), the volume, the output device and the
    volume of each app, the battery, the mobile hotspot and airplane mode.
  * **Notifications** (the bell): the latest notifications Windows showed;
    clicking one opens its app.
  * **What's playing** (the music note): the cover, the song, a timeline to
    jump in it, previous / play / next, and the volume (the mouse wheel too).
    The buttons, and the cover with the title, can also sit on the pill.
  * The control center's controls can be chosen and ordered right in it
    ("Edit", like on the iPhone: drag one to move it, "-" removes
    it, and the others are listed below to add), with shortcuts like dark
    mode, screenshot, lock, emoji, clipboard history, Task Manager,
    Calculator and File Explorer.
  * The calendar keeps events, with reminders that show in the pill.
* **Colors**: black, graphite, light, the Windows accent color, or glass.
* **The other apps' tray icons** on the island: the pinned ones always, the
  others opening sideways from the "⋯" button. Clicking, double-clicking and
  right-clicking them works like on the taskbar (a double click opens the app,
  the right button shows its menu); the pin that shows after the mouse
  stays two seconds on an icon (or the middle button) pins it.
* **New notifications in the pill**: the pill opens with the app, the title
  and the text for a few seconds, like the iPhone's Dynamic Island (not with
  "Do not disturb"), with the sender's picture when the notification has one
  on this computer (like a contact's photo). Messaging apps that offer a reply box in their
  Windows notifications (like WhatsApp) can be answered right there.
* **Notifications** can be cleared from the island's list (all, or one by
  one), and "Do not disturb" can be switched there.
* **Anywhere on the screen**: choose its place in the gear (or turn on
  dragging the pill, which then snaps to the middle and the corners of each
  edge); some apps can have a place of their own (for example, lower, so it
  doesn't cover a browser's tabs), and it slides there while they're in
  front. On the lower half of the screen, its panels and banners open
  upwards; on the left and right edges it stands upright, and they open
  towards the middle.
* **The island's options are in the island**: the gear at the bottom of the
  control center chooses its style (pill or bar), its colors, its size, what
  it shows and how it shows new notifications. Windhawk's settings only turn
  the island on and set up the taskbar.

### Screenshots

| What's playing | The control center |
| :---: | :---: |
| ![What's playing](https://i.imgur.com/MUbasD3.gif) | ![The control center](https://i.imgur.com/v7dzSiz.png) |
| **The calendar** | **The island's settings** |
| ![The calendar](https://i.imgur.com/s6vQr9b.png) | ![The island's settings](https://i.imgur.com/gUoeHWi.png) |

The bar style, at the top of the screen:

![The bar style](https://i.imgur.com/GMNJ6Fj.png)

| Colors | |
| :--- | :---: |
| Black | ![Black](https://i.imgur.com/W6RuZzU.png) |
| Graphite | ![Graphite](https://i.imgur.com/0Ku4wWx.png) |
| Light | ![Light](https://i.imgur.com/iqZRqtX.png) |
| The Windows accent color | ![Accent color](https://i.imgur.com/KDjmAiE.png) |
| Glass | ![Glass](https://i.imgur.com/3Rh5YTQ.png) |

## The dock (optional)

The "Taskbar" setting turns the Windows taskbar into a dock like the one on
macOS; or keeps the Windows taskbar, transparent with only its apps (with a
soft shadow rising from the bottom edge, the magnification and the bounces,
sliding in and out with the mod's animation; it only comes up and takes
clicks around its apps, so the rest of the bottom of the screen stays
usable), or as it is (all of it, or only its apps).

* **Compact and floating**: only the app icons, centered, in a rounded pill
  that floats a little above the bottom edge.
* **Blurred background**: the windows and the wallpaper behind it show
  through, blurred.
* **Magnification**: the icons grow in a wave that follows the mouse.
* **Bouncing icons**: an app's icon bounces while it opens, and when the app
  needs attention (a new message, for example). After clicking an app, the
  dock waits for it to open and bounce before hiding.
* **Notifications without the whole dock**: when an app needs attention while
  the dock is hidden (a new count on its icon, or a flashing button), only its
  icon peeks out from the bottom edge, bounces, and goes away again. Clicking it opens the app. The whole dock, or nothing
  at all, can be chosen instead.
* **Dots under open apps**, instead of the Windows line.
* **Hidden until you need it**: the taskbar auto-hides, and springs in when
  the mouse reaches the bottom edge of the screen.
* **Out of the way in full screen**: games and apps in full screen (exclusive
  or borderless) don't bring it up when the mouse hits the bottom edge.

## Notes

* Windows 11 only.
* The dock and the transparent taskbar are for a taskbar at the bottom. With
  the taskbar on the left, the right or the top (newer Windows 11 versions),
  it stays as Windows has it, and the island keeps working; they come back
  when it's at the bottom again.
* **The island without the dock**: the "Taskbar" setting keeps the Windows
  taskbar as it is, or with only its apps (the island shows the clock, tray
  and notifications), to combine the island with other taskbar mods.
* The mod turns on the taskbar's auto-hide while it's enabled, and turns it
  back off when the mod is disabled (if it was off before). This can be
  changed in the settings.
* Without the island, the system tray (clock, network, volume) stays at the
  right end of the taskbar, in its own pill, unless it's hidden in the
  settings.
* **Bigger magnification**: the magnified icons can only grow up to the top
  of the taskbar. For room above the dock, make the taskbar taller with the
  [Taskbar height and icon size](https://windhawk.net/mods/taskbar-icon-size)
  mod (for example, a height of 72 and the default icon size). The dock keeps
  its own height, and the space above it is left for the icons.
* Don't combine this mod with other mods that restyle the taskbar or change
  its auto-hide animation, such as Windows 11 Taskbar Styler themes or
  Taskbar Auto-Hide Instant Show, or with other dock mods. With the Windows
  taskbar "only the apps", Windows 11 Taskbar Styler can bring the tray back
  and make Explorer stop responding; for a transparent taskbar, use this
  mod's own "transparent" option instead.
* **Two processes**: the dock (and what only the taskbar knows: the apps'
  badges and the tray icons the apps send it) runs in `explorer.exe`, since
  it changes the taskbar itself; the island runs in a `windhawk.exe` process
  of its own, so a problem in it can't take the taskbar and the desktop down.
  They tell each other what changed with window messages.
* **How some features work**: Windows has no public API for a few of them, so
  the mod uses what Windows itself uses, and leaves a feature out when it
  doesn't find it as expected:
  * New notifications (also when an app's count doesn't change, like a new
    message in a chat that's already unread) and the notification list are
    read, read-only, from the Windows notification database.
  * Night light is switched through its state in the registry (CloudStore),
    checked before it's changed.
  * The output device and airplane mode use interfaces of Windows (the same
    ones the volume tools and the quick settings use).
  * Cast, live captions, the narrator and color filters are switched with
    their Windows shortcuts.
  * "Do not disturb" is switched with the interface the Windows notification
    center uses.
  * The tray icons are read as the apps send them to the taskbar. When the
    mod starts, it asks the apps to send them again, with the message Windows
    sends when the taskbar restarts. Clearing notifications on the island only
    hides them there (the Windows notification database is only read).
  * While a new notification shows in the pill, the Windows banner is moved
    off the screen (it can be kept in the gear); Windows places it again for
    the next notification.
  * Replies go to the app the way Windows sends them from its own banners:
    through the notification handler the app registered.
  * The mod only goes online if "Download pictures from the web" is turned
    on in the gear (it's off by default): then a notification's picture
    that's on the web (like WhatsApp's contact photos) is downloaded over
    https from the address the app put in it, like Windows does, and only
    for apps allowed to go online. Otherwise the app's icon is shown.
* **Similar mods**: [Dynamic Island for
  Windows](https://windhawk.net/mods/dynamic-island-for-windows) is a pill
  overlay that reacts to media, downloads and the clipboard; [Island Media
  Controls](https://windhawk.net/mods/island-media-controls) puts media
  controls on the taskbar; [TopBar for
  Windows](https://windhawk.net/mods/windhawk-topbar) adds a top bar with
  flyouts; [Taskbar Dock
  Animation](https://windhawk.net/mods/taskbar-dock-animation) magnifies the
  taskbar icons. This mod brings the island (with the control center,
  notifications and tray icons) and the dock together, as one macOS-like top
  and bottom, without other mods.

## Credits

* Mod by **caliberda** ([caliberda.com.br](https://caliberda.com.br),
  Instagram [@cesar.kali](https://www.instagram.com/cesar.kali)).
* Requests and bug reports:
  [julio@caliberda.com.br](mailto:julio@caliberda.com.br).
* The code that reaches the taskbar's XAML is based on **Taskbar tray
  auto-hide (show on hover)** by m417z (GPL-3.0).
* Inspired by the **macOS** menu bar, Control Center and Dock, and the
  iPhone's **Dynamic Island** (Apple).

---

## Português

O topo do macOS e a Dynamic Island do iPhone no Windows 11: uma ilha no topo
da tela com o relógio, uma central de controle, as notificações e o que está
tocando; e, se quiser, uma dock como a do macOS no lugar da barra de tarefas.

| A central de controle e as configurações | O calendário e uma notificação |
| :---: | :---: |
| ![A central de controle, editando, e as configurações da ilha](https://i.imgur.com/x2JTfH9.gif) | ![O calendário, e uma notificação nova respondida pela pílula](https://i.imgur.com/tSf46GE.gif) |

### A ilha

* **Sempre no topo**: o relógio, a rede, o volume, a bateria e as
  notificações, sempre visíveis (menos em tela cheia): uma pílula preta no
  centro do topo, que pode ser minimizada para uma linha fina, ou uma
  barra na largura toda, como a barra de menus do macOS, que deixa as janelas
  maximizadas abaixo dela. Apps com notificações não lidas aparecem à esquerda
  dela, com um pontinho vermelho, mesmo com a barra escondida. A roda do mouse
  no volume muda o volume, e o sino vira uma lua quando o "Não incomodar" está
  ligado.
* **Painéis como a Dynamic Island**: a própria pílula se estica até virar um
  painel, para os lados e depois para baixo, com um leve balanço:
  * **Calendário** (o relógio).
  * **Central de controle** (a rede, o volume ou a bateria), como a do macOS:
    Wi-Fi (ligar/desligar, as redes ao alcance, conectar com senha),
    Bluetooth (ligar/desligar, os dispositivos pareados), "Não incomodar",
    luz noturna, projetar (os quatro modos de tela), transmitir, legendas ao
    vivo, acessibilidade (lupa, narrador, filtros de cor, teclas de
    aderência), o brilho da tela (monitores com DDC/CI), o volume, o
    dispositivo de saída e o volume de cada app, a bateria, o hotspot móvel e
    o modo avião.
  * **Notificações** (o sino): as últimas notificações que o Windows mostrou;
    clicar numa abre o app dela.
  * **O que está tocando** (a nota musical): a capa, a música, uma linha do
    tempo para pular partes, voltar / tocar / avançar e o volume (a roda do
    mouse também). Os botões, e a capa com o título, também podem ficar na
    pílula.
  * Os controles da central de controle podem ser escolhidos e ordenados nela
    mesma ("Editar", como no iPhone: arraste um para mudar de
    lugar, o "-" tira, e os outros ficam listados embaixo para adicionar), com
    atalhos como modo escuro, captura de tela, bloquear, emojis, histórico da
    área de transferência, Gerenciador de Tarefas, Calculadora e Explorador de
    Arquivos.
  * O calendário guarda eventos, com lembretes que aparecem na pílula.
* **Cores**: preto, grafite, claro, a cor de destaque do Windows, ou vidro.
* **Os ícones dos outros apps na bandeja**, na ilha: os fixados sempre, os
  outros abrindo para o lado pelo botão "⋯". Clicar, clicar duas vezes e
  clicar com o botão direito funcionam como na barra (o duplo clique abre o
  app, o botão direito mostra o menu dele); o alfinete que aparece depois de
  dois segundos com o mouse no ícone (ou o botão do meio) fixa o ícone.
* **Notificações novas na pílula**: a pílula se abre com o app, o título e o
  texto por alguns segundos, como a Dynamic Island do iPhone (não com o "Não
  incomodar" ligado), com a foto de quem mandou quando a notificação tem uma
  neste computador (como a foto de um contato). Apps de mensagem que oferecem resposta nas
  notificações do Windows (como o WhatsApp) podem ser respondidos ali mesmo.
* **Notificações** podem ser limpas na lista da ilha (todas, ou uma por uma),
  e o "Não incomodar" pode ser ligado e desligado ali.
* **Em qualquer lugar da tela**: escolha a posição na engrenagem (ou ligue
  mover a pílula arrastando; ela gruda no meio e nos cantos de cada borda);
  alguns apps podem ter uma posição só deles (por exemplo, mais abaixo, para
  não cobrir as abas do navegador), e ela desliza para lá enquanto eles estão
  na frente. Na metade de baixo da tela, os painéis e os avisos abrem para
  cima; nas laterais ela fica em pé, e eles abrem para o meio.
* **As opções da ilha ficam na ilha**: a engrenagem no fim da central de
  controle escolhe o estilo (pílula ou barra), as cores, o tamanho, o que ela
  mostra e como ela mostra as notificações novas. As configurações do Windhawk só ligam
  a ilha e ajustam a barra de tarefas.

#### Imagens

| O que está tocando | A central de controle |
| :---: | :---: |
| ![O que está tocando](https://i.imgur.com/MUbasD3.gif) | ![A central de controle](https://i.imgur.com/v7dzSiz.png) |
| **O calendário** | **As configurações da ilha** |
| ![O calendário](https://i.imgur.com/s6vQr9b.png) | ![As configurações da ilha](https://i.imgur.com/gUoeHWi.png) |

O estilo barra, no topo da tela:

![O estilo barra](https://i.imgur.com/GMNJ6Fj.png)

| Cores | |
| :--- | :---: |
| Preto | ![Preto](https://i.imgur.com/W6RuZzU.png) |
| Grafite | ![Grafite](https://i.imgur.com/0Ku4wWx.png) |
| Claro | ![Claro](https://i.imgur.com/iqZRqtX.png) |
| A cor de destaque do Windows | ![Cor de destaque](https://i.imgur.com/KDjmAiE.png) |
| Vidro | ![Vidro](https://i.imgur.com/3Rh5YTQ.png) |

### A dock (opcional)

A configuração "Barra de tarefas" transforma a barra do Windows numa dock como
a do macOS; ou mantém a barra do Windows, transparente só com os apps (com uma
sombra suave subindo da borda de baixo, a ampliação e os pulos, entrando e
saindo com a animação do mod; ela só aparece e só pega os cliques em volta dos
apps, então o resto da parte de baixo da tela continua clicável), ou como ela
é (inteira, ou só com os apps).

* **Compacta e flutuante**: só os ícones dos apps, centralizados, numa pílula
  arredondada que flutua um pouco acima da borda de baixo.
* **Fundo desfocado**: as janelas e o papel de parede aparecem atrás dela,
  desfocados.
* **Ampliação**: os ícones crescem numa onda que acompanha o mouse.
* **Ícones que pulam**: o ícone de um app pula enquanto ele abre, e quando o
  app pede atenção (uma mensagem nova, por exemplo). Depois de clicar num app,
  a dock espera ele abrir e pular antes de se esconder.
* **Notificações sem a dock inteira**: quando um app pede atenção com a dock
  escondida (um número novo no ícone, ou o botão piscando), só o ícone dele
  aparece na borda de baixo, pula e some de novo.
  Clicar nele abre o app. Dá para escolher a dock inteira, ou nada, no lugar.
* **Pontinhos embaixo dos apps abertos**, no lugar do traço do Windows.
* **Escondida até você precisar**: a barra se oculta sozinha e entra com efeito
  de mola quando o mouse chega na borda de baixo da tela.
* **Fora do caminho em tela cheia**: jogos e apps em tela cheia (exclusiva ou
  sem bordas) não fazem ela aparecer quando o mouse encosta embaixo.

### Observações

* Somente Windows 11.
* A dock e a barra transparente são para a barra embaixo. Com a barra na
  esquerda, na direita ou no topo (versões novas do Windows 11), ela fica como
  o Windows a deixa, e a ilha continua funcionando; elas voltam quando a barra
  volta para baixo.
* **A ilha sem a dock**: a configuração "Barra de tarefas" mantém a barra do
  Windows como ela é, ou só com os apps (a ilha mostra o relógio, a bandeja e
  as notificações), para combinar a ilha com outros mods de barra.
* O mod liga o ocultar automaticamente da barra enquanto está ativo, e desliga
  de novo quando o mod é desativado (se estava desligado antes). Dá para mudar
  isso nas configurações.
* Sem a ilha, a bandeja do sistema (relógio, rede, volume) fica na ponta
  direita da barra, numa pílula própria, a não ser que seja escondida nas
  configurações.
* **Ampliação maior**: os ícones ampliados só crescem até o topo da barra de
  tarefas. Para ter espaço acima da dock, deixe a barra mais alta com o mod
  [Taskbar height and icon size](https://windhawk.net/mods/taskbar-icon-size)
  (por exemplo, altura 72 e o tamanho de ícone padrão). A dock mantém a altura
  dela, e o espaço acima fica para os ícones.
* Não use este mod junto com outros que mudam o visual da barra ou a animação
  de ocultar, como os temas do Windows 11 Taskbar Styler ou o Taskbar
  Auto-Hide Instant Show, nem com outros mods de dock. Com a barra do Windows
  "só com os apps", o Windows 11 Taskbar Styler pode trazer a bandeja de volta
  e fazer o Explorer parar de responder; para uma barra transparente, use a
  opção "transparente" deste mod.
* **Dois processos**: a dock (e o que só a barra sabe: os contadores dos apps e
  os ícones que os apps mandam para a bandeja) roda no `explorer.exe`, porque
  mexe na própria barra; a ilha roda num processo `windhawk.exe` só dela, então
  um problema nela não derruba a barra e a área de trabalho. As duas partes
  avisam uma à outra o que mudou com mensagens de janela.
* **Como algumas funções funcionam**: o Windows não tem API pública para
  algumas delas, então o mod usa o que o próprio Windows usa, e deixa a função
  de fora quando não encontra o que espera:
  * As notificações novas (também quando o número de um app não muda, como uma
    mensagem nova numa conversa que já estava não lida) e a lista de
    notificações são lidas, somente leitura, do banco de notificações do
    Windows.
  * A luz noturna é ligada e desligada pelo estado dela no registro
    (CloudStore), conferido antes de mudar.
  * O dispositivo de saída e o modo avião usam interfaces do Windows (as mesmas
    que os programas de volume e as configurações rápidas usam).
  * Transmitir, legendas ao vivo, narrador e filtros de cor são ligados pelos
    atalhos do Windows.
  * O "Não incomodar" é ligado e desligado pela interface que a central de
    notificações do Windows usa.
  * Os ícones da bandeja são lidos quando os apps os mandam para a barra.
    Quando o mod começa, ele pede aos apps que os mandem de novo, com a
    mensagem que o Windows manda quando a barra reinicia. Limpar as
    notificações na ilha só as esconde ali (o banco de notificações do Windows
    é só lido).
  * Enquanto uma notificação nova aparece na pílula, o aviso do Windows é
    tirado da tela (dá para mantê-lo na engrenagem); o Windows o coloca de
    novo na próxima notificação.
  * As respostas chegam ao app do jeito que o Windows as manda pelos avisos
    dele: pelo receptor de notificações que o app registrou.
  * O mod só acessa a internet se "Baixar fotos da internet" for ligado na
    engrenagem (vem desligado): aí a imagem de uma notificação que está na
    internet (como as fotos dos contatos do WhatsApp) é baixada por https do
    endereço que o app colocou nela, como o Windows faz, e só para apps com
    permissão de internet. Senão, aparece o ícone do app.
* **Mods parecidos**: o [Dynamic Island for
  Windows](https://windhawk.net/mods/dynamic-island-for-windows) é uma pílula
  que reage a mídia, downloads e área de transferência; o [Island Media
  Controls](https://windhawk.net/mods/island-media-controls) põe controles de
  mídia na barra; o [TopBar for
  Windows](https://windhawk.net/mods/windhawk-topbar) adiciona uma barra no
  topo com painéis; o [Taskbar Dock
  Animation](https://windhawk.net/mods/taskbar-dock-animation) amplia os
  ícones da barra. Este mod junta a ilha (com a central de controle, as
  notificações e os ícones da bandeja) e a dock, num topo e numa base como os
  do macOS, sem precisar de outros mods.

### Créditos

* Mod feito por **caliberda** ([caliberda.com.br](https://caliberda.com.br),
  Instagram [@cesar.kali](https://www.instagram.com/cesar.kali)).
* Pedidos e relatos de bugs:
  [julio@caliberda.com.br](mailto:julio@caliberda.com.br).
* O código que chega no XAML da barra de tarefas é baseado no **Taskbar tray
  auto-hide (show on hover)**, do m417z (GPL-3.0).
* Inspirado na barra de menus, na central de controle e na Dock do
  **macOS**, e na **Dynamic Island** do iPhone (Apple).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarMode: dock
  $name: Taskbar
  $name:pt-BR: Barra de tarefas
  $description: What happens to the Windows taskbar. Each group below says which of these it applies to
  $description:pt-BR: O que acontece com a barra de tarefas do Windows. Cada grupo abaixo diz para quais destas opções ele vale
  $options:
  - dock: "Dock: compact, blurred and floating, like on macOS"
  - windowsTransparent: "Windows taskbar, transparent: only the apps, with a soft shadow, hiding itself"
  - windowsApps: "Windows taskbar, only the apps (the island shows the rest)"
  - windows: "Windows taskbar, unchanged (to use other taskbar mods)"
  $options:pt-BR:
  - dock: "Dock: compacta, desfocada e flutuante, como no macOS"
  - windowsTransparent: "Barra do Windows transparente: só os apps, com uma sombra suave, se escondendo sozinha"
  - windowsApps: "Barra do Windows só com os apps (a ilha mostra o resto)"
  - windows: "Barra do Windows sem mudanças (para usar outros mods de barra)"
- showIsland: true
  $name: Island at the top
  $name:pt-BR: Ilha no topo
  $description: The clock, the control center, notifications, what's playing and the other apps' tray icons, at the top of the screen. Its own options (style, colors, what it shows) are in it, in the gear at the bottom of its control center
  $description:pt-BR: O relógio, a central de controle, as notificações, o que está tocando e os ícones dos outros apps, no topo da tela. As opções dela (estilo, cores, o que ela mostra) ficam nela mesma, na engrenagem no fim da central de controle
- hiding:
  - autoHide: true
    $name: Hide the taskbar until the mouse reaches the bottom edge
    $name:pt-BR: Esconder a barra até o mouse chegar na borda de baixo
    $description: Turns on the Windows auto-hide while the mod is on, and back off when it's turned off (if it was off before). The other options of this group need it
    $description:pt-BR: Liga o ocultar automaticamente do Windows enquanto o mod está ativo, e desliga de novo quando ele é desativado (se estava desligado antes). As outras opções deste grupo precisam dela
  - showDelay: 0
    $name: Wait before showing (ms)
    $name:pt-BR: Espera para aparecer (ms)
    $description: How long the mouse has to stay at the bottom edge (0-2000)
    $description:pt-BR: Quanto tempo o mouse precisa ficar na borda de baixo (0-2000)
  - hideDelay: 400
    $name: Wait before hiding (ms)
    $name:pt-BR: Espera para esconder (ms)
    $description: How long it stays after the mouse leaves it (0-5000)
    $description:pt-BR: Quanto tempo ela fica depois que o mouse sai (0-5000)
  - showDuration: 260
    $name: Showing animation (ms)
    $name:pt-BR: Animação de aparecer (ms)
    $description: How long it takes to slide in, with a little spring (0-1000; 0 is instant)
    $description:pt-BR: Quanto tempo ela leva para entrar, com um leve efeito de mola (0-1000; 0 é na hora)
  - hideDuration: 220
    $name: Hiding animation (ms)
    $name:pt-BR: Animação de esconder (ms)
    $description: How long it takes to slide out (0-1000; 0 is instant)
    $description:pt-BR: Quanto tempo ela leva para sair (0-1000; 0 é na hora)
  - fullscreenGuard: true
    $name: Not over games and videos in full screen
    $name:pt-BR: Não aparecer sobre jogos e vídeos em tela cheia
    $description: The mouse at the bottom edge doesn't bring it up over them (the Windows key still does)
    $description:pt-BR: O mouse na borda de baixo não faz ela aparecer sobre eles (a tecla Windows continua funcionando)
  $name: Hiding and showing (dock and transparent taskbar)
  $name:pt-BR: Esconder e mostrar (dock e barra transparente)
  $description: Only for the "Dock" and "Windows taskbar, transparent" choices above. The Windows taskbar unchanged or with only the apps hides the Windows way
  $description:pt-BR: Só para as opções "Dock" e "Barra do Windows transparente" acima. A barra do Windows sem mudanças ou só com os apps se esconde do jeito do Windows
- icons:
  - magnification: 150
    $name: Icon magnification (%)
    $name:pt-BR: Ampliação dos ícones (%)
    $description: How big the icon under the mouse gets, in a wave like on macOS (100 turns it off, up to 200). On the transparent taskbar the icons can only grow up to its top
    $description:pt-BR: O quanto o ícone embaixo do mouse cresce, numa onda como no macOS (100 desliga, até 200). Na barra transparente os ícones só crescem até o topo dela
  - magnificationRange: 120
    $name: Magnification reach (px)
    $name:pt-BR: Alcance da ampliação (px)
    $description: How far from the mouse the neighboring icons still grow (40-300). Only with magnification above 100
    $description:pt-BR: Até que distância do mouse os ícones vizinhos também crescem (40-300). Só com ampliação acima de 100
  - bounceOnLaunch: true
    $name: Bounce while an app opens
    $name:pt-BR: Pular enquanto um app abre
  - bounceOnAttention: true
    $name: Bounce when an app needs attention
    $name:pt-BR: Pular quando um app pede atenção
    $description: For example, a new message in a chat app (3 bounces)
    $description:pt-BR: Por exemplo, uma mensagem nova num app de conversa (3 pulos)
  - keepOpenOnLaunch: true
    $name: Stay up while a clicked app opens
    $name:pt-BR: Ficar aberta enquanto um app clicado abre
    $description: After clicking an app that isn't open, the taskbar waits for it to open (and bounce) before hiding. Only with the hiding of the group above
    $description:pt-BR: Depois de clicar num app que não está aberto, a barra espera ele abrir (e pular) antes de se esconder. Só com o esconder do grupo acima
  - keepOpenDuration: 1500
    $name: How long it stays after the app opens (ms)
    $name:pt-BR: Quanto tempo ela fica depois que o app abre (ms)
    $description: Enough to see the bounces (0-5000). Only with the option above on
    $description:pt-BR: O suficiente para ver os pulos (0-5000). Só com a opção acima ligada
  $name: App icons (dock and transparent taskbar)
  $name:pt-BR: Ícones dos apps (dock e barra transparente)
  $description: Only for the "Dock" and "Windows taskbar, transparent" choices above
  $description:pt-BR: Só para as opções "Dock" e "Barra do Windows transparente" acima
- dock:
  - dockHeight: 38
    $name: Height (px)
    $name:pt-BR: Altura (px)
    $description: Of the dock itself (28-64). If the taskbar is taller, the room above the dock is left for the magnified icons
    $description:pt-BR: Da própria dock (28-64). Se a barra de tarefas for mais alta, o espaço acima da dock fica para os ícones ampliados
  - bottomGap: 6
    $name: Gap at the bottom (px)
    $name:pt-BR: Espaço embaixo (px)
    $description: Between the dock and the bottom edge of the screen (0-16)
    $description:pt-BR: Entre a dock e a borda de baixo da tela (0-16)
  - cornerRadius: 14
    $name: Corner roundness (px)
    $name:pt-BR: Arredondamento dos cantos (px)
    $description: 0 is square, 24 is very round
    $description:pt-BR: 0 é quadrado, 24 é bem arredondado
  - backgroundOpacity: 55
    $name: Background tint (%)
    $name:pt-BR: Cor do fundo (%)
    $description: How much color over the blur (0 is only blur, 100 is solid)
    $description:pt-BR: Quanto de cor por cima do desfoque (0 é só desfoque, 100 é sólido)
  - dotIndicator: true
    $name: Dots under open apps
    $name:pt-BR: Pontinhos embaixo dos apps abertos
    $description: Instead of the Windows line
    $description:pt-BR: No lugar do traço do Windows
  - attentionMode: icon
    $name: When an app needs attention with the dock hidden
    $name:pt-BR: Quando um app pede atenção com a dock escondida
    $options:
    - icon: Only the app's icon peeks out, bouncing
    - dock: The whole dock shows up
    - none: Nothing
    $options:pt-BR:
    - icon: Só o ícone do app aparece, pulando
    - dock: A dock inteira aparece
    - none: Nada
  - hideTray: false
    $name: Hide the clock and the tray (without the island)
    $name:pt-BR: Esconder o relógio e a bandeja (sem a ilha)
    $description: Leaves only the apps in the dock. With the island on, they're always hidden from the dock (the island shows them)
    $description:pt-BR: Deixa só os apps na dock. Com a ilha ligada, eles já ficam sempre fora da dock (a ilha mostra)
  $name: Dock look (dock only)
  $name:pt-BR: Visual da dock (só para a dock)
  $description: Only for the "Dock" choice above
  $description:pt-BR: Só para a opção "Dock" acima
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <appmodel.h>
#include <audiopolicy.h>
#include <bluetoothapis.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <endpointvolume.h>
#include <highlevelmonitorconfigurationapi.h>
#include <mmdeviceapi.h>
#include <netlistmgr.h>
#include <physicalmonitorenumerationapi.h>
#include <propsys.h>
#include <wincodec.h>
#include <winhttp.h>
#include <wlanapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Networking.NetworkOperators.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

using namespace winrt::Windows::UI::Xaml;

enum class AttentionMode { Icon, Dock, None };

struct {
    // What the taskbar is: the mod's dock, or the Windows taskbar (all of it,
    // or only its apps).
    enum class TaskbarMode {
        Dock,
        Windows,
        WindowsApps,
        WindowsTransparent,
    } taskbarMode;
    bool autoHide;
    double cornerRadius;
    double dockHeight;
    double bottomGap;
    double backgroundOpacity;
    int showDuration;
    int hideDuration;
    int showDelay;
    int hideDelay;
    bool fullscreenGuard;
    bool hideTray;
    bool showIsland;
    bool islandBar;
    // What the island shows.
    bool islandTray;
    bool islandTrayOpen;
    bool islandNetwork;
    bool islandVolume;
    bool islandBattery;
    bool islandBell;
    bool islandMedia;
    bool islandMediaButtons;
    bool islandBanner;
    bool islandBannerMinimized;
    bool islandHideWindowsBanners;
    // Pictures of notifications that are on the web (like WhatsApp's contact
    // photos) are downloaded: off unless chosen, since it goes online.
    bool islandWebPictures;
    // The island's clock: seconds (-1 like the Windows clock), the day of the
    // week, the day and month, the year.
    int clockSeconds;
    bool clockWeekday;
    bool clockDate;
    bool clockYear;
    // Moving the pill by dragging it, and standing it upright on the sides.
    bool islandFreeDrag;
    bool islandVerticalSides;
    // The island's size (its text, icons and width), in percent.
    int islandSize;
    double magnification;
    double magnificationRange;
    bool bounceOnLaunch;
    bool keepOpenOnLaunch;
    int keepOpenDuration;
    bool bounceOnAttention;
    bool dotIndicator;
    AttentionMode attentionMode;
} g_settings;

// The island's colors: its background, and its foreground (texts, icons and
// the light fills over the background, drawn with some transparency).
struct IslandTheme {
    float background[3] = {0, 0, 0};
    // How opaque the background is (glass lets the windows show through).
    float backgroundAlpha = 1;
    float foreground[3] = {1, 1, 1};
    // Dark text on a light background needs more of it to read well.
    bool light = false;
};
IslandTheme g_theme;

// The foreground with an opacity.
D2D1_COLOR_F Fg(float alpha) {
    // On the light theme, texts and icons (drawn at 0.35 and up) get darker;
    // the light fills under them stay as they are.
    if (g_theme.light && alpha >= 0.35f) {
        alpha = std::min(1.0f, 0.55f + alpha * 0.5f);
    }
    return {g_theme.foreground[0], g_theme.foreground[1],
            g_theme.foreground[2], alpha};
}

// The background with an opacity (times the theme's own).
D2D1_COLOR_F Bg(float alpha) {
    return {g_theme.background[0], g_theme.background[1],
            g_theme.background[2], alpha * g_theme.backgroundAlpha};
}

// The themes, in the order the island's settings show them.
enum class ThemeId { Black, Graphite, Light, Accent, Glass, Count };
int g_themeIndex;

void LoadTheme() {
    g_themeIndex =
        std::clamp(Wh_GetIntValue(L"islandTheme", 0), 0, (int)ThemeId::Count - 1);
    const ThemeId theme = (ThemeId)g_themeIndex;
    g_theme = {};
    if (theme == ThemeId::Graphite) {
        g_theme.background[0] = 0.17f;
        g_theme.background[1] = 0.17f;
        g_theme.background[2] = 0.19f;
    } else if (theme == ThemeId::Light) {
        g_theme.background[0] = 0.96f;
        g_theme.background[1] = 0.96f;
        g_theme.background[2] = 0.97f;
        g_theme.foreground[0] = 0.02f;
        g_theme.foreground[1] = 0.02f;
        g_theme.foreground[2] = 0.03f;
        g_theme.light = true;
    } else if (theme == ThemeId::Accent) {
        // Darkened, so white reads well on it.
        DWORD color = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
            g_theme.background[0] = ((color >> 16) & 0xFF) / 255.0f * 0.55f;
            g_theme.background[1] = ((color >> 8) & 0xFF) / 255.0f * 0.55f;
            g_theme.background[2] = (color & 0xFF) / 255.0f * 0.55f;
        }
    } else if (theme == ThemeId::Glass) {
        // Frosted: a gray haze over what's behind, thick enough to read on
        // any window (a layered window can't blur what's behind it).
        g_theme.background[0] = 0.19f;
        g_theme.background[1] = 0.19f;
        g_theme.background[2] = 0.21f;
        g_theme.backgroundAlpha = 0.94f;
    }
}

// Whether the taskbar was at the bottom when the settings were read.
std::atomic<bool> g_taskbarAtBottom{true};

// The mod's dock (otherwise, the Windows taskbar, and the mod only adds the
// island).
bool IsDock() {
    return g_settings.taskbarMode == decltype(g_settings.taskbarMode)::Dock;
}

// The Windows taskbar, transparent, with only its apps.
bool IsTransparentTaskbar() {
    return g_settings.taskbarMode ==
           decltype(g_settings.taskbarMode)::WindowsTransparent;
}

// The taskbar hides itself, and slides in and out with the mod's animation:
// the dock, and the transparent Windows taskbar.
bool SlidesItself() {
    return IsDock() || IsTransparentTaskbar();
}

// The icons grow under the mouse and bounce: the dock, and the transparent
// Windows taskbar too.
bool AnimatesIcons() {
    return IsDock() || IsTransparentTaskbar();
}

// The transparent taskbar's soft shadow, rising from the bottom edge: a bit
// stronger over light windows (measured as it shows) and under the mouse.
std::atomic<double> g_backdropLight{0.5};

std::atomic<bool> g_unloading;

// Runs `f`, keeping its errors from reaching the code that called it: XAML
// calls throw when an element isn't ready (the taskbar hiding for a
// full-screen app, for one), and an exception left uncaught in code that
// Windows calls (hooks, window procedures, XAML events) ends Explorer.
template <typename F>
void Safely(PCWSTR what, F&& f) {
    try {
        f();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"%s failed: %08X", what, (unsigned)e.code());
    } catch (...) {
        Wh_Log(L"%s failed", what);
    }
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

// When a taskbar button last started flashing (an app asking for attention),
// in NowSeconds() time.
std::atomic<double> g_lastFlashTime{0};
// Where the button of the app that last asked for attention is, horizontally,
// in screen pixels, and when that was measured (see RecordAttentionButton).
std::atomic<int> g_attentionButtonX{0};
std::atomic<double> g_attentionButtonTime{0};

// Until when the dock stays up no matter what, in NowSeconds() time, while an
// app clicked in it opens (see KeepDockShown).
std::atomic<double> g_keepShownUntil{0};

// Bounces come in groups of three, each one a little lower, like on macOS.
// Opening an app and asking for attention are one group each.
constexpr double kBounceTime = 0.42;
constexpr double kBounceGroupPause = 0.25;
constexpr int kBouncesPerGroup = 3;
constexpr int kLaunchBounceGroups = 1;
constexpr int kAttentionBounceGroups = 1;

// Height of a bounce sequence `elapsed` seconds into it, from 0 (resting) to
// 1 (highest). Each bounce is a parabola, like a thrown object. Sets `done`
// when the sequence is over.
double BounceSequence(double elapsed, int groups, bool* done) {
    constexpr double kGroupLength =
        kBouncesPerGroup * kBounceTime + kBounceGroupPause;
    *done = false;
    const int group = (int)(elapsed / kGroupLength);
    if (group >= groups) {
        *done = true;
        return 0;
    }
    const double inGroup = elapsed - group * kGroupLength;
    const int bounce = (int)(inGroup / kBounceTime);
    if (bounce >= kBouncesPerGroup) {
        return 0;  // The pause between groups.
    }
    const double phase = (inGroup - bounce * kBounceTime) / kBounceTime;
    return (1.0 - 0.18 * bounce) * 4 * phase * (1 - phase);
}

// Whether the mod turned auto-hide on, so it's turned off again on unload.
bool g_turnedAutoHideOn;

// The taskbar's internal timers: hiding after the mouse leaves, and showing
// after the mouse reaches the edge.
constexpr UINT_PTR kHideTimerId = 2;
constexpr UINT_PTR kUnhideTimerId = 3;

BOOL CALLBACK FindTaskbarProc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId;
    WCHAR className[32];
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId() &&
        GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Finding the taskbar's XAML elements.

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        FindTaskbarProc,
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

// There can be more than one explorer.exe (folders can open in a process of
// their own): only the one with the taskbar runs the watchers and the
// auto-hide setting.
bool OwnsTaskbar() {
    return FindCurrentProcessTaskbarWnd() != nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// The mod runs in two processes (see the tool mod implementation at the end):
// in explorer.exe, the dock and what only the taskbar knows (the apps' badges,
// the tray icons the apps send it); in a windhawk.exe process of its own, the
// island, so that nothing in it can take the shell down. They tell each other
// what changed with WM_COPYDATA, tagged, never waiting long for each other.

constexpr WCHAR kIslandClassName[] = L"WindhawkMacIslandDockIsland";
constexpr WCHAR kPanelClassName[] = L"WindhawkMacIslandDockPanel";

enum IpcKind : ULONG_PTR {
    // Explorer to the island: the apps with a badge ('\n'-separated IDs).
    kIpcBadgeApps = 0x4D494401,
    // Explorer to the island: the tray icons (see SendTrayIcons).
    kIpcTrayIcons,
    // The island to explorer: it started, send everything.
    kIpcHello,
    // The island to explorer: where the tray icons are on the screen.
    kIpcTrayPlaces,
    // The island to explorer: apps with a new notification ('\n'-separated
    // IDs), so their icons bounce.
    kIpcNotifiedApps,
    // The island to explorer: an app's icon (an int size, then its ID). The
    // answer is the icon (an HICON), kept by explorer: the island copies it.
    // The shell gives packaged apps' icons in explorer, not always in the
    // island's process.
    kIpcAppIcon,
};
constexpr UINT kIpcTimeoutMs = 400;

bool SendIpc(HWND to, HWND from, IpcKind kind, const void* data, DWORD size) {
    if (!to) {
        return false;
    }
    COPYDATASTRUCT copyData{kind, size, (PVOID)data};
    DWORD_PTR result = 0;
    return SendMessageTimeout(to, WM_COPYDATA, (WPARAM)from, (LPARAM)&copyData,
                              SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT,
                              kIpcTimeoutMs, &result) != 0;
}

// Whether a WM_COPYDATA came from the other part of the mod: the island's
// window (in explorer), or the taskbar (in the island's process).
bool IsIpcSender(HWND sender, PCWSTR className) {
    WCHAR name[64];
    return sender && GetClassName(sender, name, ARRAYSIZE(name)) &&
           wcscmp(name, className) == 0;
}

HWND FindIslandWnd() {
    return FindWindow(kIslandClassName, nullptr);
}

// The main taskbar, from any process.
HWND FindTaskbarWnd() {
    return FindWindow(L"Shell_TrayWnd", nullptr);
}

// Whether the taskbar is at the bottom of its monitor (Windows 11 can also
// put it on the left, the right or the top). Hidden, it's mostly off its
// monitor, on the same side.
bool IsTaskbarAtBottom() {
    HWND taskbar = FindTaskbarWnd();
    RECT rect;
    MONITORINFO info{sizeof(info)};
    if (!taskbar || !GetWindowRect(taskbar, &rect) ||
        !GetMonitorInfo(MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST),
                        &info)) {
        return true;
    }
    if (rect.right - rect.left < rect.bottom - rect.top) {
        return false;
    }
    return rect.top + rect.bottom > info.rcMonitor.top + info.rcMonitor.bottom;
}

std::wstring JoinLines(const std::vector<std::wstring>& lines) {
    std::wstring joined;
    for (const auto& line : lines) {
        joined += line + L"\n";
    }
    return joined;
}

std::vector<std::wstring> SplitLines(const COPYDATASTRUCT* copyData) {
    std::vector<std::wstring> lines;
    const auto* text = (const WCHAR*)copyData->lpData;
    const size_t length = copyData->cbData / sizeof(WCHAR);
    size_t start = 0;
    for (size_t i = 0; i < length; i++) {
        if (text[i] == L'\n') {
            if (i > start) {
                lines.emplace_back(text + start, i - start);
            }
            start = i + 1;
        }
    }
    return lines;
}

bool SendLines(HWND to, HWND from, IpcKind kind,
               const std::vector<std::wstring>& lines) {
    const std::wstring joined = JoinLines(lines);
    return SendIpc(to, from, kind, joined.data(),
                   (DWORD)(joined.size() * sizeof(WCHAR)));
}

// Plain values and strings, one after another.
struct IpcWriter {
    std::vector<BYTE> bytes;
    template <typename T>
    void Put(const T& value) {
        const BYTE* data = (const BYTE*)&value;
        bytes.insert(bytes.end(), data, data + sizeof(T));
    }
    void PutString(const std::wstring& text) {
        Put<UINT32>((UINT32)text.size());
        const BYTE* data = (const BYTE*)text.data();
        bytes.insert(bytes.end(), data, data + text.size() * sizeof(WCHAR));
    }
};

struct IpcReader {
    const BYTE* at;
    const BYTE* end;
    explicit IpcReader(const COPYDATASTRUCT* copyData)
        : at((const BYTE*)copyData->lpData), end(at + copyData->cbData) {}
    template <typename T>
    bool Get(T* value) {
        if (end - at < (ptrdiff_t)sizeof(T)) {
            return false;
        }
        memcpy(value, at, sizeof(T));
        at += sizeof(T);
        return true;
    }
    bool GetString(std::wstring* text) {
        UINT32 length = 0;
        if (!Get(&length) ||
            (size_t)(end - at) < (size_t)length * sizeof(WCHAR)) {
            return false;
        }
        text->assign((const WCHAR*)at, length);
        at += length * sizeof(WCHAR);
        return true;
    }
};

// Breadth-first search, so the shallowest match wins.
FrameworkElement FindDescendant(
    FrameworkElement root,
    std::function<bool(FrameworkElement const&)> predicate,
    int maxDepth = 12) {
    std::vector<FrameworkElement> level{root};
    for (int depth = 0; depth < maxDepth && !level.empty(); depth++) {
        std::vector<FrameworkElement> next;
        for (const auto& element : level) {
            const int count = Media::VisualTreeHelper::GetChildrenCount(element);
            for (int i = 0; i < count; i++) {
                auto child = Media::VisualTreeHelper::GetChild(element, i)
                                 .try_as<FrameworkElement>();
                if (!child) {
                    continue;
                }
                if (predicate(child)) {
                    return child;
                }
                next.push_back(child);
            }
        }
        level = std::move(next);
    }
    return nullptr;
}

FrameworkElement FindByClassName(FrameworkElement root, PCWSTR className) {
    return FindDescendant(root, [className](FrameworkElement const& element) {
        return winrt::get_class_name(element) == className;
    });
}

FrameworkElement FindByName(FrameworkElement root, PCWSTR name) {
    return FindDescendant(root, [name](FrameworkElement const& element) {
        return element.Name() == name;
    });
}

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

////////////////////////////////////////////////////////////////////////////////
// The dock style (taskbar thread).

// Every property the mod sets, with the value it had before, so that it can
// be put back.
struct StyledProperty {
    winrt::weak_ref<DependencyObject> element;
    DependencyProperty property{nullptr};
    // DependencyProperty::UnsetValue() if it had no local value.
    winrt::Windows::Foundation::IInspectable original{nullptr};
};
// The globals below hold XAML objects. They're released on the taskbar's
// thread when the mod unloads (see ApplySettingsFromTaskbarThread), and never
// destroyed at process exit, when XAML is already gone (Windhawk wiki: "Global
// objects and process shutdown").
[[clang::no_destroy]] std::vector<StyledProperty> g_styledProperties;

// Keeps the dock's width automatic, because the taskbar sets the frame's width
// itself.
struct WidthWatch {
    winrt::weak_ref<FrameworkElement> element;
    int64_t token;
};
[[clang::no_destroy]] std::vector<WidthWatch> g_widthWatches;

void SetStyled(DependencyObject const& element,
               DependencyProperty const& property,
               winrt::Windows::Foundation::IInspectable const& value) {
    const bool known =
        std::any_of(g_styledProperties.begin(), g_styledProperties.end(),
                    [&](const StyledProperty& entry) {
                        return entry.property == property &&
                               entry.element.get() == element;
                    });
    if (!known) {
        g_styledProperties.push_back({winrt::make_weak(element), property,
                                      element.ReadLocalValue(property)});
    }
    element.SetValue(property, value);
}

void ResetDockBehavior();

void ClearStyles() {
    ResetDockBehavior();

    for (const auto& watch : g_widthWatches) {
        if (auto element = watch.element.get()) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::WidthProperty(), watch.token);
        }
    }
    g_widthWatches.clear();

    // In reverse, in case a property was recorded twice.
    for (auto it = g_styledProperties.rbegin(); it != g_styledProperties.rend();
         ++it) {
        auto element = it->element.get();
        if (!element) {
            continue;
        }
        if (it->original == DependencyProperty::UnsetValue()) {
            element.ClearValue(it->property);
        } else {
            element.SetValue(it->property, it->original);
        }
    }
    g_styledProperties.clear();
}

bool IsLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValue(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                    L"Personalize",
                    L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                    &size) != ERROR_SUCCESS) {
        return false;
    }
    return value != 0;
}

Media::Brush CreateDockBackground() {
    const bool light = IsLightTheme();
    const winrt::Windows::UI::Color tint =
        light ? winrt::Windows::UI::Color{255, 243, 243, 243}
              : winrt::Windows::UI::Color{255, 32, 32, 32};

    Media::AcrylicBrush brush;
    // Blurs what's behind the taskbar window: other windows and the desktop.
    brush.BackgroundSource(Media::AcrylicBackgroundSource::HostBackdrop);
    brush.TintColor(tint);
    brush.TintOpacity(g_settings.backgroundOpacity);
    brush.FallbackColor(tint);
    return brush;
}

Media::Brush CreateDockBorder() {
    const bool light = IsLightTheme();
    return Media::SolidColorBrush(
        light ? winrt::Windows::UI::Color{0x40, 0, 0, 0}
              : winrt::Windows::UI::Color{0x30, 255, 255, 255});
}

// Styles a Grid or StackPanel as a rounded, blurred pill.
void StylePill(FrameworkElement const& element, Thickness margin) {
    const auto radius =
        CornerRadiusHelper::FromUniformRadius(g_settings.cornerRadius);
    const auto border = ThicknessHelper::FromUniformLength(1);

    SetStyled(element, Controls::Panel::BackgroundProperty(),
              CreateDockBackground());
    SetStyled(element, FrameworkElement::MarginProperty(),
              winrt::box_value(margin));

    if (element.try_as<Controls::Grid>()) {
        SetStyled(element, Controls::Grid::CornerRadiusProperty(),
                  winrt::box_value(radius));
        SetStyled(element, Controls::Grid::BorderThicknessProperty(),
                  winrt::box_value(border));
        SetStyled(element, Controls::Grid::BorderBrushProperty(),
                  CreateDockBorder());
    } else if (element.try_as<Controls::StackPanel>()) {
        SetStyled(element, Controls::StackPanel::CornerRadiusProperty(),
                  winrt::box_value(radius));
        SetStyled(element, Controls::StackPanel::BorderThicknessProperty(),
                  winrt::box_value(border));
        SetStyled(element, Controls::StackPanel::BorderBrushProperty(),
                  CreateDockBorder());
    }
}

// The dock's background: a rounded, blurred pill, like on current macOS.
void StyleDockBackground(Shapes::Rectangle const& fill) {
    const double radius = g_settings.cornerRadius;
    SetStyled(fill, UIElement::VisibilityProperty(),
              winrt::box_value(Visibility::Visible));
    SetStyled(fill, Shapes::Rectangle::RadiusXProperty(),
              winrt::box_value(radius));
    SetStyled(fill, Shapes::Rectangle::RadiusYProperty(),
              winrt::box_value(radius));
    SetStyled(fill, Shapes::Shape::StrokeThicknessProperty(),
              winrt::box_value(1.0));

    SetStyled(fill, Shapes::Shape::FillProperty(), CreateDockBackground());
    SetStyled(fill, Shapes::Shape::StrokeProperty(), CreateDockBorder());
}

// Gives an element the dock's height, at the bottom of the taskbar.
void SetDockGeometry(FrameworkElement const& element, double rightMargin) {
    SetStyled(element, FrameworkElement::HeightProperty(),
              winrt::box_value(g_settings.dockHeight));
    SetStyled(element, FrameworkElement::VerticalAlignmentProperty(),
              winrt::box_value(VerticalAlignment::Bottom));
    SetStyled(element, FrameworkElement::MarginProperty(),
              winrt::box_value(ThicknessHelper::FromLengths(
                  0, 0, rightMargin, g_settings.bottomGap)));
}

void WatchFrameWidth(FrameworkElement const& frame) {
    const bool watched =
        std::any_of(g_widthWatches.begin(), g_widthWatches.end(),
                    [&](const WidthWatch& watch) {
                        return watch.element.get() == frame;
                    });
    if (watched) {
        return;
    }
    const int64_t token = frame.RegisterPropertyChangedCallback(
        FrameworkElement::WidthProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            if (g_unloading) {
                return;
            }
            auto element = sender.as<FrameworkElement>();
            if (!std::isnan(element.Width())) {
                element.Width(std::numeric_limits<double>::quiet_NaN());
            }
        });
    g_widthWatches.push_back({winrt::make_weak(frame), token});
}

////////////////////////////////////////////////////////////////////////////////
// Dock behavior: magnification, bouncing and dots (taskbar thread).

struct DockIcon {
    // The button grows and bounces as a whole.
    winrt::weak_ref<FrameworkElement> button;
    // The icon image inside it, used to line the icon up with the dock.
    winrt::weak_ref<FrameworkElement> image;
    Media::ScaleTransform scale{nullptr};
    Media::TranslateTransform lift{nullptr};
    // Eased towards the size the mouse position asks for.
    double magnification = 1;
    // Bouncing: when it started (0 when not bouncing) and how many groups of
    // bounces it has (see BounceSequence).
    double bounceStart = 0;
    int bounceGroups = 0;
    // Whether the app is open, and whether it's asking for attention, to
    // notice when that changes.
    bool open = false;
    bool attention = false;
    // The app's notification badge (the count on the icon), once it exists
    // (see CheckBadge): the one watched, its text and whether it was shown
    // when last checked, for which app (buttons are reused for other apps),
    // and when it last counted as a notification.
    winrt::weak_ref<FrameworkElement> badge;
    std::wstring badgeText;
    bool badgeShown = false;
    std::wstring badgeAppId;
    double lastNotified = 0;
    // The running indicators turned into dots (see UpdateDots).
    struct Dot {
        winrt::weak_ref<Shapes::Rectangle> rectangle;
        Media::ScaleTransform scale{nullptr};
        Media::TranslateTransform move{nullptr};
    };
    std::vector<Dot> dots;
};

[[clang::no_destroy]] std::vector<DockIcon> g_dockIcons;
HWND g_dockWnd;
[[clang::no_destroy]] winrt::weak_ref<FrameworkElement> g_dockFrame;
[[clang::no_destroy]] winrt::weak_ref<FrameworkElement> g_dockRepeater;
// The dock's visible background, which the icons are lined up with.
[[clang::no_destroy]] winrt::weak_ref<FrameworkElement> g_dockBackground;
// The transparent taskbar's shadow, and how strong it's shown.
[[clang::no_destroy]] winrt::weak_ref<UIElement> g_shadowFill;
double g_shadowStrength = 0.6;
// The transparent taskbar only takes the mouse around its apps: its window is
// cut down to them (a window region), so clicks elsewhere reach the windows
// behind it, and the bottom edge brings it up only there. In the window's
// pixels, and on the screen for the bottom edge (see EdgeThreadProc).
RECT g_appsRegion{};
// The apps' extent it was made for (XAML coordinates), to make it again.
double g_appsLeft = 0;
double g_appsRight = 0;
std::atomic<int> g_appsAreaLeft{0};
std::atomic<int> g_appsAreaRight{0};
// Room around the apps that still counts as theirs, and the shadow's (which
// fades out before its edges).
constexpr double kAppsAreaMargin = 34;
constexpr double kShadowMargin = 30;
// The shadow's visual, drawn on its element.
[[clang::no_destroy]] winrt::Windows::UI::Composition::SpriteVisual
    g_shadowVisual{nullptr};
int g_dockItemCount = -1;
double g_dockSince;
double g_lastRenderTime;

// The boxed handler, kept to remove it again.
[[clang::no_destroy]] winrt::Windows::Foundation::IInspectable
    g_pointerHandler{nullptr};
[[clang::no_destroy]] winrt::Windows::Foundation::IInspectable
    g_pressedHandler{nullptr};
[[clang::no_destroy]] winrt::weak_ref<FrameworkElement> g_pointerTarget;
[[clang::no_destroy]] FrameworkElement::LayoutUpdated_revoker g_layoutRevoker;
[[clang::no_destroy]] Media::CompositionTarget::Rendering_revoker
    g_renderingRevoker;
[[clang::no_destroy]] DispatcherTimer g_badgeTimer{nullptr};
// The whole dock shown for a notification (see ShowDockForNotification).
[[clang::no_destroy]] DispatcherTimer g_notificationTimer{nullptr};
[[clang::no_destroy]] std::vector<
    VisualStateGroup::CurrentStateChanged_revoker>
    g_stateRevokers;
[[clang::no_destroy]] std::vector<FrameworkElement::SizeChanged_revoker>
    g_dotRevokers;
[[clang::no_destroy]] std::vector<winrt::weak_ref<FrameworkElement>>
    g_watchedButtons;

// Property callbacks on the notification badges, to remove them again.
struct BadgeWatch {
    winrt::weak_ref<DependencyObject> element;
    DependencyProperty property{nullptr};
    int64_t token;
};
[[clang::no_destroy]] std::vector<BadgeWatch> g_badgeWatches;
double g_lastBadgeScan;

// Defined further down.
bool IsTaskbarHidden(HWND hTaskbarWnd, const RECT& monitorRect);
std::wstring GetWindowAppId(HWND hWnd);
std::wstring WindowAppKey(HWND hWnd);
void PostAttentionRequest(HWND app, std::wstring appId);
void ScheduleTaskbarHide(HWND hTaskbarWnd, UINT delayMs);
void SlideTo(HWND hWnd, const RECT& target, int durationMs, bool show);

// Whether the taskbar itself has the dock shown: it calls SlideWindow to
// show and hide it (see TrayUI_SlideWindow_Hook).
bool g_trayShown;
// The mod showed the dock for a notification, without the taskbar knowing,
// and where to put it back (see ShowDockForNotification).
bool g_shownForNotification;
RECT g_notificationHiddenRect;

// How long the dock waits at most for a clicked app to open, in seconds.
constexpr double kLaunchWaitSeconds = 6;

// Keeps the dock up for `seconds` from now, replacing any earlier wait. The
// taskbar asks PermitAutoHide before hiding (see
// CTaskListWnd_PermitAutoHide_Hook), and its hide timer is set to try again
// right after.
void KeepDockShown(double seconds) {
    if (!g_dockWnd) {
        return;
    }
    g_keepShownUntil = NowSeconds() + seconds;
    ScheduleTaskbarHide(g_dockWnd, (UINT)(seconds * 1000) + 100);
}

// How fast the magnification follows the mouse (higher is snappier).
constexpr double kMagnificationFollow = 18;
// Diameter of the open-app dot, and its distance below where the running
// indicator line was, in pixels.
constexpr double kDotSize = 4;
constexpr double kDotGap = 3;

void OnRendering(winrt::Windows::Foundation::IInspectable const&,
                 winrt::Windows::Foundation::IInspectable const&);

void StartRendering() {
    if (!g_renderingRevoker && !g_unloading) {
        g_lastRenderTime = NowSeconds();
        Safely(L"Starting to draw", [] {
            g_renderingRevoker = Media::CompositionTarget::Rendering(
                winrt::auto_revoke, OnRendering);
        });
    }
}

DockIcon* FindDockIcon(FrameworkElement const& button) {
    for (auto& icon : g_dockIcons) {
        if (icon.button.get() == button) {
            return &icon;
        }
    }
    return nullptr;
}

// Collects every element of the given classes under `root`, without looking
// inside the ones found.
void FindAllByClassName(FrameworkElement const& root,
                        std::initializer_list<PCWSTR> classNames,
                        std::vector<FrameworkElement>& found,
                        int depth = 0) {
    if (depth > 14) {
        return;
    }
    const int count = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(root, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        const auto className = winrt::get_class_name(child);
        if (std::any_of(classNames.begin(), classNames.end(),
                        [&](PCWSTR name) { return className == name; })) {
            found.push_back(child);
        } else {
            FindAllByClassName(child, classNames, found, depth + 1);
        }
    }
}

// Remembers where an app's button is on the screen, for the icon that peeks
// out while the dock is hidden (see RunAttentionIcon).
void RecordAttentionButton(FrameworkElement const& button) {
    auto parent =
        Media::VisualTreeHelper::GetParent(button).try_as<UIElement>();
    if (!parent || !g_dockWnd) {
        return;
    }
    const auto offset = button.ActualOffset();
    const double center =
        parent.TransformToVisual(nullptr)
            .TransformPoint({offset.x, offset.y})
            .X +
        button.ActualWidth() / 2;
    POINT pt{MulDiv((int)std::lround(center), GetDpiForWindow(g_dockWnd), 96),
             0};
    ClientToScreen(g_dockWnd, &pt);
    g_attentionButtonX = pt.x;
    g_attentionButtonTime = NowSeconds();
}

// The running state comes from the button's "RunningIndicatorStates" visual
// states: NoRunningIndicator (closed), Active/InactiveRunningIndicator (open)
// and RequestingAttention... (asking for attention).
void OnRunningStateChanged(winrt::weak_ref<FrameworkElement> weakButton,
                           std::wstring_view oldState,
                           std::wstring_view newState) {
    auto button = weakButton.get();
    DockIcon* icon = button ? FindDockIcon(button) : nullptr;
    if (!icon) {
        return;
    }

    const bool wasClosed = oldState == L"NoRunningIndicator" ||
                           // A new button for an app that wasn't pinned.
                           (oldState.empty() && NowSeconds() - g_dockSince > 5);
    const bool attention =
        newState.find(L"RequestingAttention") != std::wstring_view::npos;
    const bool open = !newState.empty() && newState != L"NoRunningIndicator";

    if (attention && !icon->attention) {
        RecordAttentionButton(button);
        if (g_settings.bounceOnAttention) {
            icon->bounceStart = NowSeconds();
            icon->bounceGroups = kAttentionBounceGroups;
            StartRendering();
        }
    }
    icon->attention = attention;

    icon->open = open;

    if (wasClosed && open && !attention) {
        if (g_settings.bounceOnLaunch) {
            icon->bounceStart = NowSeconds();
            icon->bounceGroups = kLaunchBounceGroups;
            StartRendering();
        }
        // Only if it was clicked in the dock (see OnDockPressed), so the
        // dock is up.
        if (g_settings.keepOpenOnLaunch && NowSeconds() < g_keepShownUntil) {
            KeepDockShown(g_settings.keepOpenDuration / 1000.0);
        }
    }
}

// A click on a closed app's button: the dock waits for the app to open.
void OnDockPressed(Input::PointerRoutedEventArgs const& args) {
    if (!AnimatesIcons()) {
        return;
    }
    if (!g_settings.keepOpenOnLaunch) {
        return;
    }
    auto element = args.OriginalSource().try_as<DependencyObject>();
    while (element) {
        auto frameworkElement = element.try_as<FrameworkElement>();
        if (frameworkElement && winrt::get_class_name(frameworkElement) ==
                                    L"Taskbar.TaskListButton") {
            DockIcon* icon = FindDockIcon(frameworkElement);
            if (icon && !icon->open) {
                KeepDockShown(kLaunchWaitSeconds);
            }
            return;
        }
        element = Media::VisualTreeHelper::GetParent(element);
    }
}

// How long the whole dock stays up for a notification, when that's the
// setting, in milliseconds.
constexpr UINT kNotificationShowMs = 3500;

// The app a taskbar button belongs to: its AppUserModelID, which the button
// carries as its automation ID (with an "Appid: " prefix on some builds).
std::wstring GetButtonAppId(FrameworkElement const& button) {
    std::wstring id{Automation::AutomationProperties::GetAutomationId(button)};
    constexpr std::wstring_view kPrefix = L"Appid: ";
    if (id.starts_with(kPrefix)) {
        id.erase(0, kPrefix.size());
    }
    return id;
}

// Shows the whole dock for a moment, for a notification. The taskbar only
// shows itself for the mouse at the edge, so the mod slides it in itself, and
// back out after a while unless the mouse is on it. If the taskbar shows
// itself meanwhile (the mouse went there), it takes over.
void ShowDockForNotification() {
    if (g_shownForNotification || g_trayShown || !g_dockWnd) {
        return;
    }
    RECT rect;
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetWindowRect(g_dockWnd, &rect) ||
        !GetMonitorInfo(MonitorFromWindow(g_dockWnd, MONITOR_DEFAULTTONEAREST),
                        &monitor)) {
        return;
    }
    g_notificationHiddenRect = rect;
    RECT shown = rect;
    OffsetRect(&shown, 0, monitor.rcMonitor.bottom - rect.bottom);
    g_shownForNotification = true;
    SlideTo(g_dockWnd, shown, g_settings.showDuration, true);

    if (g_notificationTimer) {
        g_notificationTimer.Stop();
    }
    g_notificationTimer = DispatcherTimer();
    g_notificationTimer.Interval(std::chrono::milliseconds(kNotificationShowMs));
    g_notificationTimer.Tick(
        [](winrt::Windows::Foundation::IInspectable const& sender,
           winrt::Windows::Foundation::IInspectable const&) {
            auto timer = sender.as<DispatcherTimer>();
            if (!g_shownForNotification || g_trayShown || g_unloading ||
                !g_dockWnd) {
                timer.Stop();
                g_shownForNotification = false;
                return;
            }
            // Stays while the mouse is on it; asked again on the next tick.
            POINT pt;
            RECT rect;
            if (GetCursorPos(&pt) && GetWindowRect(g_dockWnd, &rect) &&
                PtInRect(&rect, pt)) {
                return;
            }
            timer.Stop();
            g_shownForNotification = false;
            SlideTo(g_dockWnd, g_notificationHiddenRect,
                    g_settings.hideDuration, false);
        });
    g_notificationTimer.Start();
}

// An app has something new: a new count on its badge, or its button started
// flashing. The icon bounces in the dock if it's on screen. Otherwise, it
// depends on the setting: only the icon peeks out, the whole dock shows up,
// or nothing happens.
void OnAppNotified(FrameworkElement const& button) {
    DockIcon* icon = FindDockIcon(button);
    const double now = NowSeconds();
    if (!icon || !g_dockWnd || now - icon->lastNotified < 1.0) {
        return;
    }
    icon->lastNotified = now;

    const std::wstring appId = GetButtonAppId(button);
    // On the Windows taskbar, the button shows it itself (on the transparent
    // one, it also bounces while the taskbar is up).
    if (!AnimatesIcons()) {
        return;
    }
    // Not for the app the user is looking at.
    if (!appId.empty() && GetWindowAppId(GetForegroundWindow()) == appId) {
        return;
    }
    RecordAttentionButton(button);

    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfo(MonitorFromWindow(g_dockWnd, MONITOR_DEFAULTTONEAREST),
                   &monitor);
    const bool hidden = IsTaskbarHidden(g_dockWnd, monitor.rcMonitor);
    if (!IsDock()) {
        if (!hidden && g_settings.bounceOnAttention) {
            icon->bounceStart = now;
            icon->bounceGroups = kAttentionBounceGroups;
            StartRendering();
        }
        return;
    }

    if (hidden && g_settings.attentionMode == AttentionMode::Icon) {
        PostAttentionRequest(nullptr, appId);
        return;
    }
    if (hidden && g_settings.attentionMode == AttentionMode::None) {
        return;
    }
    if (g_settings.bounceOnAttention) {
        icon->bounceStart = now;
        icon->bounceGroups = kAttentionBounceGroups;
        StartRendering();
    }
    if (hidden) {
        ShowDockForNotification();
    }
}

bool IsBadgeShown(const DockIcon& icon) {
    auto badge = icon.badge.get();
    return badge && badge.Visibility() == Visibility::Visible &&
           badge.Opacity() > 0;
}

// The apps with a notification badge right now, by AppUserModelID, for the
// island (see Island::UpdateApps). Written on the taskbar thread.
SRWLOCK g_badgeAppsLock = SRWLOCK_INIT;
std::vector<std::wstring> g_badgeApps;
// The island's window, told when they change.
std::atomic<HWND> g_islandWnd;
// The message that tells it.
constexpr UINT WM_APP_BADGES = WM_APP + 51;

// To the island, in its own process.
void SendBadgeApps() {
    HWND island = FindIslandWnd();
    if (!island) {
        return;
    }
    AcquireSRWLockShared(&g_badgeAppsLock);
    const std::vector<std::wstring> apps = g_badgeApps;
    ReleaseSRWLockShared(&g_badgeAppsLock);
    SendLines(island, FindCurrentProcessTaskbarWnd(), kIpcBadgeApps, apps);
}

void PublishBadgeApps() {
    std::vector<std::wstring> apps;
    for (const auto& icon : g_dockIcons) {
        auto button = icon.button.get();
        if (!button || !icon.badgeShown) {
            continue;
        }
        std::wstring appId = GetButtonAppId(button);
        if (!appId.empty() &&
            std::find(apps.begin(), apps.end(), appId) == apps.end()) {
            apps.push_back(std::move(appId));
        }
    }
    AcquireSRWLockExclusive(&g_badgeAppsLock);
    const bool changed = apps != g_badgeApps;
    if (changed) {
        g_badgeApps = std::move(apps);
    }
    ReleaseSRWLockExclusive(&g_badgeAppsLock);
    if (changed) {
        SendBadgeApps();
    }
}

// The badge (Taskbar.Badge#BadgeControl, with TextBlock#BadgeText inside) is
// only created with an app's first notification.
FrameworkElement FindBadge(FrameworkElement const& button) {
    return FindDescendant(button, [](FrameworkElement const& e) {
        return std::wstring_view{winrt::get_class_name(e)}.find(L"Badge") !=
                   std::wstring_view::npos ||
               std::wstring_view{e.Name()}.find(L"Badge") !=
                   std::wstring_view::npos;
    });
}

Controls::TextBlock FindBadgeText(FrameworkElement const& badge) {
    if (auto found = FindDescendant(badge, [](FrameworkElement const& e) {
            return (bool)e.try_as<Controls::TextBlock>();
        })) {
        return found.as<Controls::TextBlock>();
    }
    return nullptr;
}

void CheckBadgeOf(winrt::weak_ref<FrameworkElement> const& weakButton);

// Asks to be told when the badge's text, visibility or opacity change.
void WatchBadge(DockIcon& icon,
                FrameworkElement const& button,
                FrameworkElement const& badge,
                Controls::TextBlock const& text) {
    icon.badge = winrt::make_weak(badge);
    const auto weakButton = winrt::make_weak(button);
    auto watch = [&](DependencyObject const& element,
                     DependencyProperty const& property) {
        g_badgeWatches.push_back(
            {winrt::make_weak(element), property,
             element.RegisterPropertyChangedCallback(
                 property, [weakButton](DependencyObject const&,
                                        DependencyProperty const&) {
                     CheckBadgeOf(weakButton);
                 })});
    };
    watch(text, Controls::TextBlock::TextProperty());
    watch(badge, UIElement::VisibilityProperty());
    watch(badge, UIElement::OpacityProperty());
}

// Compares the badge with how it was last seen. News: it appeared with a
// count, or the count changed (but not to a smaller number: fewer unread
// items isn't news; "99+" reads as 99). Nothing is news for a moment after the
// mod starts (the badges already there), or when the button now shows another
// app.
void CheckBadge(DockIcon& icon) {
    auto button = icon.button.get();
    if (!button) {
        return;
    }
    auto badge = FindBadge(button);
    Controls::TextBlock text = badge ? FindBadgeText(badge) : nullptr;
    if (badge && text && badge != icon.badge.get()) {
        WatchBadge(icon, button, badge, text);
    }

    const std::wstring appId = GetButtonAppId(button);
    const std::wstring newText = text ? std::wstring{text.Text()} : L"";
    const bool shown = text && !newText.empty() &&
                       badge.Visibility() == Visibility::Visible &&
                       badge.Opacity() > 0;

    bool news = false;
    if (appId == icon.badgeAppId && shown) {
        if (!icon.badgeShown) {
            news = true;
        } else if (newText != icon.badgeText) {
            const int count = _wtoi(newText.c_str());
            const int oldCount = _wtoi(icon.badgeText.c_str());
            news = !(count > 0 && oldCount > 0 && count < oldCount);
        }
    }
    icon.badgeAppId = appId;
    icon.badgeText = newText;
    icon.badgeShown = shown;
    if (news && NowSeconds() - g_dockSince > 5) {
        OnAppNotified(button);
    }
}

void CheckBadgeOf(winrt::weak_ref<FrameworkElement> const& weakButton) {
    auto button = weakButton.get();
    if (DockIcon* icon = button ? FindDockIcon(button) : nullptr) {
        CheckBadge(*icon);
        PublishBadgeApps();
    }
}

// The badges are created, replaced, faded in and out by the taskbar, so they're
// checked again as it changes, and every second.
void WatchBadges() {
    for (auto& icon : g_dockIcons) {
        CheckBadge(icon);
    }
}

void WatchBadgesAndPublish() {
    WatchBadges();
    PublishBadgeApps();
}


void WatchRunningState(FrameworkElement const& button) {
    auto panel = FindByName(button, L"IconPanel");
    if (!panel) {
        return;
    }
    for (const auto& group : VisualStateManager::GetVisualStateGroups(panel)) {
        if (group.Name() != L"RunningIndicatorStates") {
            continue;
        }
        if (DockIcon* icon = FindDockIcon(button)) {
            const auto state = group.CurrentState();
            icon->open = state && state.Name() != L"NoRunningIndicator";
        }
        g_stateRevokers.push_back(group.CurrentStateChanged(
            winrt::auto_revoke,
            [weakButton = winrt::make_weak(button)](
                winrt::Windows::Foundation::IInspectable const&,
                VisualStateChangedEventArgs const& args) {
                const auto oldState =
                    args.OldState() ? args.OldState().Name() : winrt::hstring{};
                const auto newState =
                    args.NewState() ? args.NewState().Name() : winrt::hstring{};
                OnRunningStateChanged(weakButton, oldState, newState);
            }));
    }
}

void FindAllByName(FrameworkElement const& root,
                   PCWSTR name,
                   std::vector<FrameworkElement>& found,
                   int depth = 0) {
    if (depth > 14) {
        return;
    }
    const int count = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(root, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            found.push_back(child);
        }
        FindAllByName(child, name, found, depth + 1);
    }
}

// Where a button is drawn this frame, in the taskbar's coordinates, for
// UpdateDots.
struct IconPlacement {
    double buttonTop;   // Layout position, before the mod's transforms.
    double originY;     // Scaling origin, in the button's coordinates.
    double offsetY;     // The button's vertical move (alignment + bounce).
    double iconBottom;  // The resting icon's bottom edge.
    double dockBottom;  // The dock background's bottom edge.
};

// Keeps the icon's dots round, small, just below the icon, and still while
// the icon grows and bounces, like on macOS. The dots are inside the button
// that grows and bounces, so they're moved and scaled back.
void UpdateDots(DockIcon& icon, const IconPlacement& place) {
    auto button = icon.button.get();
    if (!button || g_unloading) {
        return;
    }
    const double magnification = std::max(icon.magnification, 0.01);
    const double dotCenter =
        std::min(place.iconBottom + kDotGap + kDotSize / 2,
                 place.dockBottom - kDotSize / 2 - 1);

    for (auto& dot : icon.dots) {
        auto rectangle = dot.rectangle.get();
        if (!rectangle) {
            continue;
        }
        const double width = rectangle.ActualWidth();
        const double height = rectangle.ActualHeight();
        auto parent =
            Media::VisualTreeHelper::GetParent(rectangle).try_as<UIElement>();
        if (width <= 0 || height <= 0 || !parent) {
            continue;
        }

        // Rounded fully before scaling, so it's a circle after. Only set when
        // it changes: a new radius makes the taskbar lay itself out again,
        // which would render another frame, and so on.
        if (rectangle.RadiusX() != width / 2 ||
            rectangle.RadiusY() != height / 2) {
            SetStyled(rectangle, Shapes::Rectangle::RadiusXProperty(),
                      winrt::box_value(width / 2));
            SetStyled(rectangle, Shapes::Rectangle::RadiusYProperty(),
                      winrt::box_value(height / 2));
        }
        dot.scale.ScaleX(kDotSize / (width * magnification));
        dot.scale.ScaleY(kDotSize / (height * magnification));

        // The line's center, in the button's coordinates.
        const auto offset = rectangle.ActualOffset();
        const double center = parent.TransformToVisual(button)
                                  .TransformPoint({offset.x, offset.y})
                                  .Y +
                              height / 2;
        // A point `y` of the button is drawn at
        //   buttonTop + originY + (y - originY) * magnification + offsetY.
        // This is the move that puts the line's center at `dotCenter`.
        dot.move.Y((dotCenter - place.buttonTop - place.originY -
                    place.offsetY) /
                       magnification +
                   place.originY - center);
    }
}

void MakeDots(DockIcon& icon, FrameworkElement const& button) {
    std::vector<FrameworkElement> indicators;
    FindAllByName(button, L"RunningIndicator", indicators);

    for (const auto& element : indicators) {
        auto rectangle = element.try_as<Shapes::Rectangle>();
        if (!rectangle) {
            continue;
        }
        DockIcon::Dot dot;
        dot.rectangle = winrt::make_weak(rectangle);
        Media::TransformGroup transforms;
        transforms.Children().Append(dot.scale = Media::ScaleTransform());
        transforms.Children().Append(dot.move = Media::TranslateTransform());
        SetStyled(rectangle, UIElement::RenderTransformProperty(), transforms);
        SetStyled(rectangle, UIElement::RenderTransformOriginProperty(),
                  winrt::box_value(winrt::Windows::Foundation::Point{0.5f, 0.5f}));
        icon.dots.push_back(dot);

        // The line changes size with the app's state.
        g_dotRevokers.push_back(rectangle.SizeChanged(
            winrt::auto_revoke,
            [weakButton = winrt::make_weak(button)](
                winrt::Windows::Foundation::IInspectable const&,
                SizeChangedEventArgs const&) {
                if (weakButton.get()) {
                    StartRendering();
                }
            }));
    }
}

// Finds the dock's buttons and prepares the ones it hasn't seen yet. Called
// again whenever the number of buttons changes.
void RescanDockIcons() {
    auto frame = g_dockFrame.get();
    if (!frame) {
        return;
    }

    std::erase_if(g_dockIcons, [](const DockIcon& icon) {
        return !icon.button.get();
    });

    std::vector<FrameworkElement> buttons;
    FindAllByClassName(frame,
                       {L"Taskbar.TaskListButton",
                        L"Taskbar.ExperienceToggleButton",
                        L"Taskbar.SearchBoxButton",
                        L"Taskbar.AugmentedEntryPointButton"},
                       buttons);

    for (const auto& button : buttons) {
        if (FindDockIcon(button)) {
            continue;
        }

        // The whole button grows and bounces. The icon image inside it isn't
        // touched, because the taskbar animates it itself when it's pressed,
        // and replacing its transform breaks that.
        DockIcon icon;
        icon.button = winrt::make_weak(button);
        if (auto image = FindByName(button, L"Icon")) {
            icon.image = winrt::make_weak(image);
        }
        Media::TransformGroup transforms;
        transforms.Children().Append(icon.scale = Media::ScaleTransform());
        transforms.Children().Append(icon.lift = Media::TranslateTransform());
        if (AnimatesIcons()) {
            SetStyled(button, UIElement::RenderTransformProperty(), transforms);
        }
        // The scaling origin is set in pixels on the scale itself (see
        // OnRendering), so the relative origin stays at the corner.
        if (AnimatesIcons()) {
            SetStyled(button, UIElement::RenderTransformOriginProperty(),
                      winrt::box_value(winrt::Windows::Foundation::Point{0, 0}));
        }
        g_dockIcons.push_back(icon);
        DockIcon& added = g_dockIcons.back();

        const bool watched =
            std::any_of(g_watchedButtons.begin(), g_watchedButtons.end(),
                        [&](const auto& weak) { return weak.get() == button; });
        if (!watched &&
            winrt::get_class_name(button) == L"Taskbar.TaskListButton") {
            g_watchedButtons.push_back(winrt::make_weak(button));
            WatchRunningState(button);
        }
        if (g_settings.dotIndicator && IsDock()) {
            MakeDots(added, button);
        }

        // No highlight behind the icon on hover or for the active app, like
        // on macOS. It's hidden rather than recolored, because the button's
        // visual states set its color.
        std::vector<FrameworkElement> highlights;
        if (AnimatesIcons()) {
            FindAllByName(button, L"BackgroundElement", highlights);
        }
        for (const auto& highlight : highlights) {
            SetStyled(highlight, UIElement::VisibilityProperty(),
                      winrt::box_value(Visibility::Collapsed));
        }
    }
}

// Height of the icon's bounce right now, in pixels (negative is up), for a
// bounce at most `maxHeight` high.
double BounceOffset(DockIcon& icon, double now, double maxHeight) {
    if (!icon.bounceStart) {
        return 0;
    }
    bool done;
    const double height =
        BounceSequence(now - icon.bounceStart, icon.bounceGroups, &done);
    if (done) {
        icon.bounceStart = 0;
        return 0;
    }
    return -std::max(maxHeight, 0.0) * height;
}

// Cuts the transparent taskbar's window down to its apps (and a margin), and
// puts its shadow behind them. `left` and `right` are the apps' extent, in
// the taskbar's XAML coordinates. Only done when it changes (an app opened or
// closed): the shadow's size is part of the layout.
// The span the shadow was last put behind.
double g_shadowLeft = 0;
double g_shadowRight = 0;

void PlaceShadow(double left, double right) {
    if (left == g_shadowLeft && right == g_shadowRight) {
        return;
    }
    Safely(L"Placing the shadow", [&] {
        auto fill = g_shadowFill.get().try_as<FrameworkElement>();
        if (!fill) {
            return;
        }
        double parentLeft = 0;
        if (auto parent =
                Media::VisualTreeHelper::GetParent(fill).try_as<UIElement>()) {
            parentLeft = parent.TransformToVisual(nullptr)
                             .TransformPoint({0, 0})
                             .X;
        }
        SetStyled(fill, FrameworkElement::HorizontalAlignmentProperty(),
                  winrt::box_value(HorizontalAlignment::Left));
        SetStyled(fill, FrameworkElement::MarginProperty(),
                  winrt::box_value(ThicknessHelper::FromLengths(
                      std::max(0.0, left - kShadowMargin - parentLeft), 0, 0,
                      0)));
        SetStyled(fill, FrameworkElement::WidthProperty(),
                  winrt::box_value(right - left + kShadowMargin * 2));
        g_shadowLeft = left;
        g_shadowRight = right;
    });
}

// `withShadow`: also puts the shadow behind the apps (XAML, so only from the
// taskbar's own drawing, not while Windows moves it).
void UpdateAppsArea(double left, double right, bool withShadow) {
    if (!g_dockWnd) {
        return;
    }
    if (withShadow) {
        PlaceShadow(left, right);
    }
    const double scale = GetDpiForWindow(g_dockWnd) / 96.0;
    RECT window{};
    if (!GetWindowRect(g_dockWnd, &window)) {
        return;
    }
    RECT region{(LONG)std::floor((left - kAppsAreaMargin) * scale), 0,
                (LONG)std::ceil((right + kAppsAreaMargin) * scale),
                window.bottom - window.top};
    region.left = std::max(region.left, 0L);
    region.right = std::min(region.right, window.right - window.left);
    if (region.right <= region.left) {
        return;
    }
    g_appsLeft = left;
    g_appsRight = right;
    // Already in place (Windows sometimes sets the taskbar's region itself,
    // which drops this one: then it's made again).
    RECT current{};
    const int kind = GetWindowRgnBox(g_dockWnd, &current);
    if (EqualRect(&region, &g_appsRegion) && kind != ERROR &&
        kind != NULLREGION && EqualRect(&current, &region)) {
        return;
    }
    if (g_appsRegion.right > g_appsRegion.left) {
        Wh_Log(L"The taskbar's region was changed (%d), setting it again", kind);
    }
    HRGN handle = CreateRectRgnIndirect(&region);
    if (!handle) {
        return;
    }
    // The system owns the region once it's set.
    if (!SetWindowRgn(g_dockWnd, handle, TRUE)) {
        DeleteObject(handle);
        return;
    }
    g_appsRegion = region;
    g_appsAreaLeft = window.left + region.left;
    g_appsAreaRight = window.left + region.right;
}

// The shadow: an ellipse centered on the bottom edge, dark in its middle and
// fading out to nothing at its edges, so it has no corners.
void MakeShadow(UIElement const& element) {
    namespace Composition = winrt::Windows::UI::Composition;
    try {
        auto compositor =
            Hosting::ElementCompositionPreview::GetElementVisual(element)
                .Compositor();
        auto gradient = compositor.CreateRadialGradientBrush();
        gradient.EllipseCenter({0.5f, 1.0f});
        gradient.EllipseRadius({0.5f, 1.0f});
        const std::pair<float, uint8_t> kStops[] = {
            {0.0f, 0xC4}, {0.35f, 0x98}, {0.65f, 0x4C}, {0.85f, 0x1A},
            {1.0f, 0x00}};
        for (const auto& [offset, alpha] : kStops) {
            gradient.ColorStops().Append(compositor.CreateColorGradientStop(
                offset, winrt::Windows::UI::Color{alpha, 0, 0, 0}));
        }
        auto visual = compositor.CreateSpriteVisual();
        visual.Brush(gradient);
        // As big as the element, whatever its size.
        visual.RelativeSizeAdjustment({1.0f, 1.0f});
        Hosting::ElementCompositionPreview::SetElementChildVisual(element,
                                                                 visual);
        g_shadowVisual = visual;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"The shadow couldn't be made: %08X", (unsigned)e.code());
    }
}

void ResetAppsArea() {
    if (g_dockWnd && (g_appsRegion.right > g_appsRegion.left)) {
        SetWindowRgn(g_dockWnd, nullptr, TRUE);
    }
    g_appsRegion = {};
    g_appsLeft = g_appsRight = 0;
    g_shadowLeft = g_shadowRight = 0;
    g_appsAreaLeft = 0;
    g_appsAreaRight = 0;
}

void OnRenderingUnsafe();

void OnRendering(winrt::Windows::Foundation::IInspectable const&,
                 winrt::Windows::Foundation::IInspectable const&) {
    bool failed = true;
    Safely(L"Drawing the icons", [&] {
        OnRenderingUnsafe();
        failed = false;
    });
    // Tried again on the next change, not every frame.
    if (failed) {
        g_renderingRevoker.revoke();
    }
}

void OnRenderingUnsafe() {
    const double now = NowSeconds();
    const double dt = std::clamp(now - g_lastRenderTime, 0.0, 0.05);
    g_lastRenderTime = now;

    auto frame = g_dockFrame.get();
    if (!frame || g_unloading || !AnimatesIcons()) {
        g_renderingRevoker.revoke();
        return;
    }

    // The mouse, in the taskbar's XAML coordinates.
    POINT pt;
    double mouseX = 0, mouseY = -1;
    if (GetCursorPos(&pt) && ScreenToClient(g_dockWnd, &pt)) {
        const UINT dpi = GetDpiForWindow(g_dockWnd);
        mouseX = pt.x * 96.0 / dpi;
        mouseY = pt.y * 96.0 / dpi;
    }
    RECT client{};
    GetClientRect(g_dockWnd, &client);
    const double clientHeight =
        client.bottom * 96.0 / GetDpiForWindow(g_dockWnd);

    // The dock background's edges: the icons rest on its middle line.
    double dockTop = 0;
    double dockBottom = clientHeight;
    if (auto background = g_dockBackground.get()) {
        const auto bounds =
            background.TransformToVisual(nullptr).TransformBounds(
                {0, 0, (float)background.ActualWidth(),
                 (float)background.ActualHeight()});
        dockTop = bounds.Y;
        dockBottom = bounds.Y + bounds.Height;
    }
    const double dockMiddle = (dockTop + dockBottom) / 2;

    struct Placed {
        DockIcon* icon;
        double center;
        double buttonTop;
        double buttonWidth;
        // The icon image, in the button's coordinates.
        double iconTop;
        double iconHeight;
        // The highest part drawn with the icon: its top, or the top of the
        // notification badge on it. In the button's coordinates.
        double contentTop;
    };
    std::vector<Placed> placed;
    double left = std::numeric_limits<double>::max();
    double right = std::numeric_limits<double>::lowest();
    for (auto& icon : g_dockIcons) {
        auto button = icon.button.get();
        if (!button || button.ActualWidth() <= 0) {
            continue;
        }
        auto parent =
            Media::VisualTreeHelper::GetParent(button).try_as<UIElement>();
        if (!parent) {
            continue;
        }
        // The layout position, without the mod's own transforms.
        const auto offset = button.ActualOffset();
        const auto origin = parent.TransformToVisual(nullptr).TransformPoint(
            {offset.x, offset.y});
        const double width = button.ActualWidth();
        const double height = button.ActualHeight();
        // Windows keeps some buttons far off the window (like a hidden
        // Snipping Tool one): not among the apps.
        const double clientWidth =
            client.right * 96.0 / GetDpiForWindow(g_dockWnd);
        if (origin.X + width < 0 || origin.X > clientWidth ||
            button.Visibility() != Visibility::Visible) {
            continue;
        }

        // Buttons without an icon image (like Start) use their middle part.
        double iconTop = height * 0.2;
        double iconHeight = height * 0.6;
        // The layout position, not the drawn one: the taskbar's own press
        // animation shrinks the image, which mustn't move the icon around.
        if (auto image = icon.image.get(); image && image.ActualHeight() > 0) {
            if (auto imageParent = Media::VisualTreeHelper::GetParent(image)
                                       .try_as<UIElement>()) {
                const auto imageOffset = image.ActualOffset();
                iconTop = imageParent.TransformToVisual(button)
                              .TransformPoint({imageOffset.x, imageOffset.y})
                              .Y;
                iconHeight = image.ActualHeight();
            }
        }

        left = std::min(left, (double)origin.X);
        right = std::max(right, origin.X + width);
        // The badge sits on the icon's top corner and grows with it.
        double contentTop = iconTop;
        if (auto badge = icon.badge.get(); badge && IsBadgeShown(icon)) {
            if (auto badgeParent = Media::VisualTreeHelper::GetParent(badge)
                                       .try_as<UIElement>()) {
                const auto badgeOffset = badge.ActualOffset();
                contentTop = std::min(
                    contentTop,
                    (double)badgeParent.TransformToVisual(button)
                        .TransformPoint({badgeOffset.x, badgeOffset.y})
                        .Y);
            }
        }

        placed.push_back({&icon, origin.X + width / 2, origin.Y, width,
                          iconTop, iconHeight, contentTop});
    }

    const bool hovering = !placed.empty() && mouseY >= 0 &&
                          mouseY <= clientHeight + 2 && mouseX >= left - 16 &&
                          mouseX <= right + 16;
    const double follow = 1 - std::exp(-dt * kMagnificationFollow);

    bool busy = hovering;

    if (IsTransparentTaskbar() && !placed.empty()) {
        UpdateAppsArea(left, right, true);
    }

    // The transparent taskbar's shadow: stronger over light windows, and
    // with the mouse on the taskbar.
    if (auto fill = g_shadowFill.get()) {
        const bool over = mouseY >= 0 && mouseY <= clientHeight + 2 &&
                          mouseX >= 0 &&
                          mouseX <= client.right * 96.0 /
                                        GetDpiForWindow(g_dockWnd);
        const double target =
            std::min(1.0, 0.66 + 0.28 * g_backdropLight + (over ? 0.12 : 0.0));
        g_shadowStrength += (target - g_shadowStrength) * (1 - std::exp(-dt * 8));
        if (std::fabs(target - g_shadowStrength) < 0.003) {
            g_shadowStrength = target;
        } else {
            busy = true;
        }
        if (std::fabs(fill.Opacity() - g_shadowStrength) > 0.002) {
            fill.Opacity(g_shadowStrength);
        }
        if (over) {
            busy = true;
        }
    }
    for (auto& item : placed) {
        DockIcon& icon = *item.icon;
        if (item.iconHeight <= 0) {
            continue;
        }

        // Moves the icon to the middle of the dock, wherever the taskbar put
        // it (a taller taskbar centers the icons in its whole height).
        const double align =
            dockMiddle - (item.buttonTop + item.iconTop + item.iconHeight / 2);
        const double iconBottom =
            item.buttonTop + item.iconTop + item.iconHeight + align;

        // Never taller than the space up to the top of the taskbar, badge
        // included. `reach` is how far the content goes above the icon's
        // bottom edge, which stays put.
        const double reach =
            item.iconTop + item.iconHeight - item.contentTop;
        double maxScale = std::max(
            1.0, std::min(g_settings.magnification, (iconBottom - 1) / reach));
        // With their names shown ("Combine taskbar buttons" off), buttons
        // are wide and grow with their text: only a few pixels, so they
        // don't run into the next one.
        if (item.buttonWidth > item.iconHeight * 3) {
            maxScale = std::min(maxScale, 1 + 12 / item.buttonWidth);
        }

        // A bell curve around the mouse: the icon under it grows the most.
        double target = 1;
        if (hovering && maxScale > 1) {
            const double distance =
                std::fabs(mouseX - item.center) / g_settings.magnificationRange;
            if (distance < 1) {
                const double bell = 0.5 + 0.5 * std::cos(3.14159265 * distance);
                target = 1 + (maxScale - 1) * bell;
            }
        }
        icon.magnification += (target - icon.magnification) * follow;
        if (std::fabs(target - icon.magnification) < 0.002) {
            icon.magnification = target;
        } else {
            busy = true;
        }

        // Grows upwards from the icon's bottom edge, around the button's
        // middle, like on macOS.
        const double originY = item.iconTop + item.iconHeight;
        icon.scale.CenterX(item.buttonWidth / 2);
        icon.scale.CenterY(originY);
        icon.scale.ScaleX(icon.magnification);
        icon.scale.ScaleY(icon.magnification);

        // The bounce uses what's left above the (possibly magnified) icon.
        const double bounceRoom = iconBottom - 1 - reach * icon.magnification;
        const double bounceHeight =
            std::min({item.iconHeight * 0.6, 14.0, bounceRoom});
        const double bounce = BounceOffset(icon, now, bounceHeight);
        icon.lift.Y(align + bounce);
        if (icon.bounceStart) {
            busy = true;
        }

        UpdateDots(icon, {item.buttonTop, originY, align + bounce, iconBottom,
                          dockBottom});
    }

    if (!busy) {
        g_renderingRevoker.revoke();
    }
}

void CheckDockItems() {
    auto repeater = g_dockRepeater.get();
    if (!repeater) {
        return;
    }
    const int count = Media::VisualTreeHelper::GetChildrenCount(repeater);
    if (count != g_dockItemCount) {
        g_dockItemCount = count;
        RescanDockIcons();
        WatchBadgesAndPublish();
    }
}

void SetupDockBehavior(HWND hWnd,
                       FrameworkElement const& frame,
                       FrameworkElement const& background) {
    g_dockWnd = hWnd;
    g_dockFrame = winrt::make_weak(frame);
    g_dockBackground = winrt::make_weak(background);
    auto repeater = FindByName(frame, L"TaskbarFrameRepeater");
    g_dockRepeater = winrt::make_weak(repeater ? repeater : frame);
    g_dockItemCount = -1;
    g_dockSince = NowSeconds();

    // Pointer events reach the frame even when a button handles them.
    g_pointerHandler = winrt::box_value(Input::PointerEventHandler(
        [](winrt::Windows::Foundation::IInspectable const&,
           Input::PointerRoutedEventArgs const&) { StartRendering(); }));
    frame.AddHandler(UIElement::PointerMovedEvent(), g_pointerHandler, true);
    g_pressedHandler = winrt::box_value(Input::PointerEventHandler(
        [](winrt::Windows::Foundation::IInspectable const&,
           Input::PointerRoutedEventArgs const& args) {
            Safely(L"A click on the taskbar", [&] { OnDockPressed(args); });
        }));
    frame.AddHandler(UIElement::PointerPressedEvent(), g_pressedHandler, true);
    g_pointerTarget = winrt::make_weak(frame);

    // New and closed apps change the number of buttons.
    g_layoutRevoker = frame.LayoutUpdated(
        winrt::auto_revoke,
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::Foundation::IInspectable const&) {
            Safely(L"Following the layout", [] {
                CheckDockItems();
                // Positions may have changed: line the icons up again.
                // Moving them doesn't cause another layout pass.
                StartRendering();
                // Looking for new badges, at most twice a second.
                if (NowSeconds() - g_lastBadgeScan > 0.5) {
                    g_lastBadgeScan = NowSeconds();
                    WatchBadgesAndPublish();
                }
            });
        });
    CheckDockItems();
    StartRendering();

    // Badges fade in and out, so they're also checked every second.
    g_badgeTimer = DispatcherTimer();
    g_badgeTimer.Interval(std::chrono::seconds(1));
    g_badgeTimer.Tick([](winrt::Windows::Foundation::IInspectable const&,
                         winrt::Windows::Foundation::IInspectable const&) {
        if (!g_unloading) {
            Safely(L"Watching the badges", [] { WatchBadgesAndPublish(); });
        }
    });
    g_badgeTimer.Start();

}

void ResetDockBehavior() {
    if (g_badgeTimer) {
        g_badgeTimer.Stop();
        g_badgeTimer = nullptr;
    }
    // The dock shown for a notification goes back down.
    if (g_notificationTimer) {
        g_notificationTimer.Stop();
        g_notificationTimer = nullptr;
    }
    if (g_shownForNotification) {
        g_shownForNotification = false;
        if (g_dockWnd) {
            const RECT& hidden = g_notificationHiddenRect;
            SetWindowPos(g_dockWnd, nullptr, hidden.left, hidden.top,
                         hidden.right - hidden.left,
                         hidden.bottom - hidden.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    AcquireSRWLockExclusive(&g_badgeAppsLock);
    g_badgeApps.clear();
    ReleaseSRWLockExclusive(&g_badgeAppsLock);
    g_renderingRevoker.revoke();
    g_layoutRevoker.revoke();
    g_stateRevokers.clear();
    g_dotRevokers.clear();
    for (const auto& watch : g_badgeWatches) {
        if (auto element = watch.element.get()) {
            element.UnregisterPropertyChangedCallback(watch.property,
                                                      watch.token);
        }
    }
    g_badgeWatches.clear();
    if (auto target = g_pointerTarget.get()) {
        if (g_pointerHandler) {
            target.RemoveHandler(UIElement::PointerMovedEvent(),
                                 g_pointerHandler);
        }
        if (g_pressedHandler) {
            target.RemoveHandler(UIElement::PointerPressedEvent(),
                                 g_pressedHandler);
        }
    }
    g_pointerHandler = nullptr;
    g_pressedHandler = nullptr;
    g_pointerTarget = nullptr;
    g_watchedButtons.clear();
    g_dockIcons.clear();
    g_dockFrame = nullptr;
    g_dockRepeater = nullptr;
    g_dockBackground = nullptr;
    if (auto fill = g_shadowFill.get(); fill && g_shadowVisual) {
        Hosting::ElementCompositionPreview::SetElementChildVisual(fill,
                                                                 nullptr);
    }
    g_shadowVisual = nullptr;
    g_shadowFill = nullptr;
    ResetAppsArea();
    g_dockItemCount = -1;
    g_dockWnd = nullptr;
}

bool ApplyStyle(XamlRoot xamlRoot, HWND hWnd, bool mainTaskbar) {
    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) {
        return false;
    }

    auto frame = FindByClassName(content, L"Taskbar.TaskbarFrame");
    if (!frame) {
        Wh_Log(L"TaskbarFrame not found");
        return false;
    }
    auto rootGrid = FindByName(frame, L"RootGrid");
    if (!rootGrid) {
        Wh_Log(L"RootGrid not found");
        return false;
    }

    // The Windows taskbar: as it is, or without what the island shows (the
    // tray on the right, the widgets on the left). The apps' badges are still
    // watched, for the island.
    if (!IsDock()) {
        if (g_settings.taskbarMode ==
                decltype(g_settings.taskbarMode)::WindowsApps ||
            IsTransparentTaskbar()) {
            if (auto tray =
                    FindByClassName(content, L"SystemTray.SystemTrayFrame")) {
                SetStyled(tray, UIElement::VisibilityProperty(),
                          winrt::box_value(Visibility::Collapsed));
            }
            std::vector<FrameworkElement> widgets;
            FindAllByClassName(frame, {L"Taskbar.AugmentedEntryPointButton"},
                               widgets);
            for (const auto& widget : widgets) {
                SetStyled(widget, UIElement::VisibilityProperty(),
                          winrt::box_value(Visibility::Collapsed));
            }
        }
        // Transparent: no background and no line on top, only a soft shadow
        // rising from the bottom edge, so the icons still stand out over
        // light windows.
        if (IsTransparentTaskbar()) {
            if (auto background =
                    FindByClassName(rootGrid, L"Taskbar.TaskbarBackground")) {
                if (auto fill = FindByName(background, L"BackgroundFill")) {
                    SetStyled(fill, Shapes::Shape::FillProperty(),
                              Media::SolidColorBrush(
                                  winrt::Windows::UI::Colors::Transparent()));
                    SetStyled(fill, UIElement::OpacityProperty(),
                              winrt::box_value(g_shadowStrength));
                    if (mainTaskbar) {
                        g_shadowFill = winrt::make_weak(fill.as<UIElement>());
                        MakeShadow(fill);
                    }
                }
                if (auto stroke = FindByName(background, L"BackgroundStroke")) {
                    SetStyled(stroke, UIElement::VisibilityProperty(),
                              winrt::box_value(Visibility::Collapsed));
                }
            } else {
                Wh_Log(L"TaskbarBackground not found");
            }
        }
        if (mainTaskbar) {
            SetupDockBehavior(hWnd, frame, nullptr);
        }
        return true;
    }

    // Only as wide as the icons, centered.
    SetStyled(frame, FrameworkElement::HorizontalAlignmentProperty(),
              winrt::box_value(HorizontalAlignment::Center));
    SetStyled(frame, FrameworkElement::WidthProperty(),
              winrt::box_value(std::numeric_limits<double>::quiet_NaN()));
    WatchFrameWidth(frame);

    // The dock has its own height, at the bottom of the taskbar. Whatever the
    // taskbar has above it stays transparent: room for the magnified icons,
    // and the strip that stays on screen while the taskbar is hidden.
    SetStyled(rootGrid, Controls::Grid::PaddingProperty(),
              winrt::box_value(ThicknessHelper::FromLengths(8, 0, 8, 0)));

    // The dock's look goes on the taskbar's own background rectangle, which
    // lies behind the icons. Rounding the panel that holds the icons would
    // also cut off the magnified icons at its edges.
    Shapes::Rectangle fill{nullptr};
    if (auto background =
            FindByClassName(rootGrid, L"Taskbar.TaskbarBackground")) {
        fill = FindByName(background, L"BackgroundFill")
                   .try_as<Shapes::Rectangle>();
        if (auto stroke = FindByName(background, L"BackgroundStroke")) {
            SetStyled(stroke, UIElement::VisibilityProperty(),
                      winrt::box_value(Visibility::Collapsed));
        }
    }
    // Only the background gets the dock's height. The buttons keep theirs,
    // and their icons are lined up with the background (see OnRendering).
    FrameworkElement dockBackground{nullptr};
    if (fill) {
        StyleDockBackground(fill);
        SetDockGeometry(fill, 0);
        dockBackground = fill;
    } else {
        Wh_Log(L"BackgroundFill not found, rounding the panel instead");
        StylePill(rootGrid, ThicknessHelper::FromLengths(
                                0, 0, 0, g_settings.bottomGap));
        SetDockGeometry(rootGrid, 0);
        dockBackground = rootGrid;
    }

    if (auto tray = FindByClassName(content, L"SystemTray.SystemTrayFrame")) {
        if (g_settings.hideTray || g_settings.showIsland) {
            SetStyled(tray, UIElement::VisibilityProperty(),
                      winrt::box_value(Visibility::Collapsed));
        } else if (auto trayGrid = FindByName(tray, L"SystemTrayFrameGrid")) {
            StylePill(trayGrid, ThicknessHelper::FromLengths(
                                    0, 0, 6, g_settings.bottomGap));
            SetDockGeometry(trayGrid, 6);
        }
    }

    // Magnification and bouncing follow the mouse on the main taskbar.
    if (mainTaskbar) {
        SetupDockBehavior(hWnd, frame, dockBackground);
    }
    return true;
}

// Whether the main taskbar got its style (it may not be ready yet when the
// mod loads with Explorer; see LateStartThreadProc).
std::atomic<bool> g_mainTaskbarStyled;

void ApplyToTaskbar(HWND hWnd, XamlRoot xamlRoot, bool mainTaskbar) {
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed for %p", hWnd);
        return;
    }
    bool applied = false;
    Safely(L"Styling the taskbar",
           [&] { applied = ApplyStyle(xamlRoot, hWnd, mainTaskbar); });
    if (!applied) {
        Wh_Log(L"ApplyStyle failed for %p", hWnd);
        return;
    }
    if (mainTaskbar) {
        g_mainTaskbarStyled = true;
    }
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Runs a function on a window's thread (with a hook on that thread for the
// message sent to the window), and returns once it ran.
const UINT g_runFromWindowThreadMessage =
    RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runFromWindowThreadMessage) {
            auto* param = (RunFromWindowThreadParam*)cwp->lParam;
            param->proc(param->procParam);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, RunFromWindowThreadHookProc,
                                  nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParam param{proc, procParam};
    SendMessage(hWnd, g_runFromWindowThreadMessage, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

// Styles a taskbar window of this thread.
BOOL CALLBACK ApplyToTaskbarWindowProc(HWND hWnd, LPARAM) {
    WCHAR className[32];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        ApplyToTaskbar(hWnd, GetTaskbarXamlRoot(hWnd), true);
    } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        ApplyToTaskbar(hWnd, GetSecondaryTaskbarXamlRoot(hWnd), false);
    }
    return TRUE;
}

// Restyles every taskbar from the taskbar thread, or removes the style while
// unloading.
// Unloading: what the XAML globals still hold goes, on the taskbar's thread
// (their destructors never run, see g_styledProperties).
void FreeXamlGlobals() {
    std::vector<StyledProperty>().swap(g_styledProperties);
    std::vector<WidthWatch>().swap(g_widthWatches);
    std::vector<DockIcon>().swap(g_dockIcons);
    std::vector<VisualStateGroup::CurrentStateChanged_revoker>().swap(
        g_stateRevokers);
    std::vector<FrameworkElement::SizeChanged_revoker>().swap(g_dotRevokers);
    std::vector<winrt::weak_ref<FrameworkElement>>().swap(g_watchedButtons);
    std::vector<BadgeWatch>().swap(g_badgeWatches);
    g_layoutRevoker = {};
    g_renderingRevoker = {};
}

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");

    ClearStyles();
    g_mainTaskbarStyled = false;
    if (g_unloading) {
        FreeXamlGlobals();
        return;
    }

    EnumThreadWindows(GetCurrentThreadId(), ApplyToTaskbarWindowProc, 0);
}

void WINAPI ApplySettingsProc(void*) {
    ApplySettingsFromTaskbarThread();
}

void ApplySettings() {
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTaskbarWnd, ApplySettingsProc, nullptr);
    }
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;
void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L">");
    TrayUI_StartTaskbar_Original(pThis);
    ApplySettingsFromTaskbarThread();
}

using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void* pThis);
CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original;

using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis,
                                                        void* taskbarModel);
CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original;
// Defined further down.
void SubclassTaskbars();

void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis,
                                                 void* taskbarModel) {
    Wh_Log(L">");
    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);
    if (g_unloading) {
        return;
    }
    HWND taskbarWnd = CSecondaryTray_GetTrayWindow_Original(pThis);
    ApplyToTaskbar(taskbarWnd, GetSecondaryTaskbarXamlRoot(taskbarWnd), false);
    SubclassTaskbars();
}

////////////////////////////////////////////////////////////////////////////////
// Showing and hiding (taskbar thread).

// A slightly springy settle, like the macOS dock: fast at first, a hint of
// overshoot, then at rest. `t` goes from 0 to 1 over the animation.
double SpringOut(double t) {
    constexpr double kDamping = 0.78;
    // Settles within the duration.
    constexpr double kOmega = 5.2 / kDamping;
    const double dampedOmega = kOmega * std::sqrt(1 - kDamping * kDamping);
    return 1 - std::exp(-kDamping * kOmega * t) *
                   (std::cos(dampedOmega * t) +
                    kDamping * kOmega / dampedOmega * std::sin(dampedOmega * t));
}

double EaseInOutCubic(double t) {
    const double u = -2 * t + 2;
    return t < 0.5 ? 4 * t * t * t : 1 - u * u * u / 2;
}

int LerpInt(int a, int b, double t) {
    return a + (int)std::lround((b - a) * t);
}

// Slides the window from where it is to `target`, one step per display frame,
// timed by the clock so a slow frame doesn't slow the whole animation down.
// Showing settles like a spring; hiding eases in and out.
void SlideTo(HWND hWnd, const RECT& target, int durationMs, bool show) {
    RECT start;
    if (durationMs <= 0 || !GetWindowRect(hWnd, &start) ||
        EqualRect(&start, &target)) {
        return;
    }

    const double begin = NowSeconds();
    const double duration = durationMs / 1000.0;
    for (;;) {
        const double t =
            std::min((NowSeconds() - begin) / duration, 1.0);
        const double p = t >= 1 ? 1 : show ? SpringOut(t) : EaseInOutCubic(t);
        SetWindowPos(hWnd, nullptr, LerpInt(start.left, target.left, p),
                     LerpInt(start.top, target.top, p),
                     LerpInt(start.right - start.left,
                             target.right - target.left, p),
                     LerpInt(start.bottom - start.top,
                             target.bottom - target.top, p),
                     SWP_NOZORDER | SWP_NOACTIVATE);
        if (t >= 1) {
            break;
        }
        // Waits for the next frame.
        DwmFlush();
    }
}

// How long after an app asks for attention a "show" counts as the taskbar
// showing itself for it, in seconds.
constexpr double kAttentionShowWindow = 1.0;

// Whether the user is reaching for the taskbar right now: the mouse on it or
// at the bottom edge, or the Windows key held.
bool UserIsReachingForTaskbar(HWND hWnd) {
    if ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) {
        return true;
    }
    POINT pt;
    RECT rect;
    if (!GetCursorPos(&pt) || !GetWindowRect(hWnd, &rect)) {
        return true;
    }
    if (PtInRect(&rect, pt)) {
        return true;
    }
    MONITORINFO info{sizeof(info)};
    return GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                          &info) &&
           pt.y >= info.rcMonitor.bottom - 2;
}

// How light the screen is where the taskbar is about to show (0 dark, 1
// light), for the transparent taskbar's shadow: the area, shrunk to a few
// pixels, averaged.
void SampleBackdrop(const RECT& rect) {
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return;
    }
    constexpr int kSampleWidth = 16;
    constexpr int kSampleHeight = 2;
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return;
    }
    HDC memory = CreateCompatibleDC(screen);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = kSampleWidth;
    info.bmiHeader.biHeight = -kSampleHeight;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap =
        memory ? CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits,
                                  nullptr, 0)
               : nullptr;
    if (bitmap && bits) {
        HGDIOBJ old = SelectObject(memory, bitmap);
        SetStretchBltMode(memory, HALFTONE);
        if (StretchBlt(memory, 0, 0, kSampleWidth, kSampleHeight, screen,
                       rect.left, rect.top, width, height, SRCCOPY)) {
            GdiFlush();
            double total = 0;
            const auto* pixels = (const BYTE*)bits;
            for (int i = 0; i < kSampleWidth * kSampleHeight; i++) {
                const BYTE* pixel = pixels + i * 4;
                total += (0.0722 * pixel[0] + 0.7152 * pixel[1] +
                          0.2126 * pixel[2]) /
                         255;
            }
            g_backdropLight = total / (kSampleWidth * kSampleHeight);
        }
        SelectObject(memory, old);
    }
    if (bitmap) {
        DeleteObject(bitmap);
    }
    if (memory) {
        DeleteDC(memory);
    }
    ReleaseDC(nullptr, screen);
    StartRendering();
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rect,
                                           HMONITOR monitor,
                                           bool show,
                                           bool animate);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
// Defined further down.
bool IsFullscreenAppOnMonitor(HWND hTaskbarWnd);
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rect,
                                    HMONITOR monitor,
                                    bool show,
                                    bool animate) {
    if (!SlidesItself()) {
        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);
        return;
    }
    // Unless the whole dock is wanted, the taskbar doesn't show itself when an
    // app asks for attention: only the app's icon peeks out (or nothing).
    if (show && !g_unloading && IsDock() &&
        g_settings.attentionMode != AttentionMode::Dock &&
        NowSeconds() - g_lastFlashTime < kAttentionShowWindow &&
        !UserIsReachingForTaskbar(hWnd)) {
        Wh_Log(L"Not showing the taskbar for an app asking for attention");
        // The taskbar now takes itself for shown. Its hide timer puts that
        // right, without moving anything.
        PostMessage(hWnd, WM_TIMER, kHideTimerId, 0);
        return;
    }

    if (hWnd == g_dockWnd) {
        g_trayShown = show;
        // The taskbar takes over from a dock shown for a notification.
        g_shownForNotification = false;
    }

    if (show && IsTransparentTaskbar()) {
        SampleBackdrop(*rect);
    }

    // After a full screen app (like a video in a browser), Windows sometimes
    // leaves the taskbar below other windows, so it would show behind them.
    if (show && !g_unloading &&
        !(GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) &&
        !IsFullscreenAppOnMonitor(hWnd)) {
        Wh_Log(L"The taskbar wasn't on top, putting it back");
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    if (!animate || g_unloading) {
        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);
        if (!g_unloading && IsTransparentTaskbar() && hWnd == g_dockWnd &&
            g_appsRight > g_appsLeft) {
            UpdateAppsArea(g_appsLeft, g_appsRight, false);
        }
        return;
    }

    // The window is moved by the mod's own animation, then the taskbar
    // finishes the move without its animation, which keeps its state right.
    SlideTo(hWnd, *rect,
            show ? g_settings.showDuration : g_settings.hideDuration, show);
    TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, false);
    // Still cut down to its apps.
    if (IsTransparentTaskbar() && hWnd == g_dockWnd &&
        g_appsRight > g_appsLeft) {
        UpdateAppsArea(g_appsLeft, g_appsRight, false);
    }
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    if (hWnd && !g_unloading && SlidesItself() &&
        (nIDEvent == kHideTimerId || nIDEvent == kUnhideTimerId)) {
        WCHAR className[32];
        if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
            (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
             _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
            int delay = nIDEvent == kUnhideTimerId ? g_settings.showDelay
                                                   : g_settings.hideDelay;
            // While the dock is kept up, hiding is tried right after that.
            const double keep = g_keepShownUntil - NowSeconds();
            if (nIDEvent == kHideTimerId && keep > 0) {
                delay = std::max(delay, (int)(keep * 1000) + 100);
            }
            // A timer can't be shorter than 10 ms anyway.
            uElapse = (UINT)std::max(delay, 10);
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// Starts the taskbar's own hide timer with a delay of the mod's choosing.
void ScheduleTaskbarHide(HWND hTaskbarWnd, UINT delayMs) {
    SetTimer_Original(hTaskbarWnd, kHideTimerId, delayMs, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Full-screen apps.

bool IsShellWindow(HWND hWnd) {
    WCHAR className[32];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    for (PCWSTR shellClass : {L"Progman", L"WorkerW", L"Shell_TrayWnd",
                              L"Shell_SecondaryTrayWnd"}) {
        if (_wcsicmp(className, shellClass) == 0) {
            return true;
        }
    }
    return false;
}

// Whether the foreground window covers the whole monitor of this taskbar,
// like games in exclusive or borderless full screen, and videos in full
// screen.
// Whether a window covers the whole monitor like a full-screen app.
// `windowsSaysBusy`: Windows itself takes a full-screen app to be running (a
// browser's video in full screen, for one), so a window with a title bar
// counts even if its size isn't exactly the screen's.
bool CoversMonitor(HWND hWnd,
                   HMONITOR monitor,
                   const RECT& monitorRect,
                   bool windowsSaysBusy = false) {
    if (!hWnd || !IsWindowVisible(hWnd) || IsShellWindow(hWnd) ||
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL) != monitor) {
        return false;
    }
    DWORD processId = 0;
    // Not the mod's own windows (the island, its panels).
    if (GetWindowThreadProcessId(hWnd, &processId) &&
        processId == GetCurrentProcessId()) {
        return false;
    }
    RECT rect;
    if (!GetWindowRect(hWnd, &rect) || rect.left > monitorRect.left ||
        rect.top > monitorRect.top || rect.right < monitorRect.right ||
        rect.bottom < monitorRect.bottom) {
        return false;
    }
    // Exactly the screen: full screen, whatever its style (a browser keeps
    // its window "maximized" while it plays a video in full screen).
    if (EqualRect(&rect, &monitorRect)) {
        return true;
    }
    // Bigger than the screen: a maximized window (with the taskbar hidden,
    // its borders go past the screen's edges) isn't full screen, and neither
    // is one with a title bar stretched over it (like File Explorer), unless
    // Windows itself takes it for a full-screen app.
    if (IsZoomed(hWnd)) {
        return false;
    }
    if ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION) {
        return windowsSaysBusy;
    }
    return true;
}

// Whether a full-screen app (a game, a video) is on this taskbar's monitor:
// the window in front, or the one at the top of the screen (in front may be
// something small, like a browser's "press Esc" hint).
bool IsFullscreenAppOnMonitor(HWND hTaskbarWnd) {
    // Exclusive full screen (Direct3D) and presentations, as reported by
    // Windows.
    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
        (state == QUNS_RUNNING_D3D_FULL_SCREEN ||
         state == QUNS_PRESENTATION_MODE)) {
        return true;
    }
    // "Busy": Windows sees a full-screen app (not only Direct3D ones).
    const bool busy = state == QUNS_BUSY;

    HMONITOR monitor = MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfo(monitor, &info)) {
        return false;
    }
    HWND foreground = GetForegroundWindow();
    if (CoversMonitor(GetAncestor(foreground, GA_ROOT), monitor,
                      info.rcMonitor, busy)) {
        return true;
    }
    // Only when the window in front is that one's (a hint over a video),
    // not any window behind the one being used.
    const POINT top{(info.rcMonitor.left + info.rcMonitor.right) / 2,
                    info.rcMonitor.top + 1};
    HWND atTop = GetAncestor(WindowFromPoint(top), GA_ROOT);
    if (atTop && atTop != foreground) {
        DWORD topProcess = 0, foregroundProcess = 0;
        GetWindowThreadProcessId(atTop, &topProcess);
        GetWindowThreadProcessId(foreground, &foregroundProcess);
        if (topProcess == foregroundProcess &&
            CoversMonitor(atTop, monitor, info.rcMonitor, busy)) {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// The other apps' tray icons, for the island.
//
// Apps add, change and remove their tray icons (Shell_NotifyIcon) by sending
// the data to the taskbar window (WM_COPYDATA). The mod reads it there, as it
// passes, keeps its own copy of each icon, and the island draws them. Clicks
// on the island are passed on to the app the same way the taskbar does: as
// the icon's callback message. To learn about the icons added before the mod
// started, it asks the apps to add them again, with the message Windows sends
// them when the taskbar is restarted ("TaskbarCreated").

// The data the taskbar gets (with 32-bit handles, from 32 and 64-bit apps).
#pragma pack(push, 4)
struct NotifyIconData32 {
    DWORD cbSize;
    DWORD hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    DWORD hIcon;
    WCHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[256];
    UINT uVersion;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    DWORD hBalloonIcon;
};
struct TrayNotifyData {
    DWORD signature;
    DWORD message;
    NotifyIconData32 nid;
};
#pragma pack(pop)
constexpr DWORD kTrayNotifySignature = 0x34753423;

// An icon handle the mod owns, destroyed when the last copy goes.
struct OwnedIcon {
    HICON icon;
    explicit OwnedIcon(HICON icon) : icon(icon) {}
    ~OwnedIcon() {
        if (icon) {
            DestroyIcon(icon);
        }
    }
    OwnedIcon(const OwnedIcon&) = delete;
    OwnedIcon& operator=(const OwnedIcon&) = delete;
};

struct TrayIcon {
    HWND owner = nullptr;
    UINT id = 0;
    GUID guid{};
    bool hasGuid = false;
    UINT callback = 0;
    UINT version = 0;
    std::shared_ptr<OwnedIcon> icon;
    // Changes with the icon, so the island draws it again.
    UINT serial = 0;
    std::wstring tip;
    bool hidden = false;
    DWORD processId = 0;
    // Stays the same across restarts, to remember it pinned: the program
    // and the icon's ID.
    std::wstring key;
};

SRWLOCK g_trayIconsLock = SRWLOCK_INIT;
std::vector<TrayIcon> g_trayIcons;
UINT g_trayIconSerial;
// Tells the island they changed.
constexpr UINT WM_APP_TRAY = WM_APP + 53;

std::wstring TrayIconKey(const TrayIcon& icon) {
    std::wstring key;
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                     icon.processId)) {
        WCHAR path[MAX_PATH];
        DWORD length = ARRAYSIZE(path);
        if (QueryFullProcessImageName(process, 0, path, &length)) {
            key = path;
        }
        CloseHandle(process);
    }
    for (auto& c : key) {
        c = towlower(c);
    }
    if (icon.hasGuid) {
        WCHAR guid[64];
        StringFromGUID2(icon.guid, guid, ARRAYSIZE(guid));
        key += L"|";
        key += guid;
    } else {
        key += L"|" + std::to_wstring(icon.id);
    }
    return key;
}

// On the taskbar's thread, as an app sends it (see TaskbarSubclassProc).
void SendTrayIcons();

// The island gets the tray icons a moment after they change (see
// TaskbarSubclassProc).
const UINT g_sendTrayIconsMessage =
    RegisterWindowMessage(L"WindhawkMacIslandDock_SendTrayIcons");
std::atomic<bool> g_trayIconsSendPending;

void OnTrayNotify(const COPYDATASTRUCT* copyData) {
    if (!copyData ||
        copyData->dwData != 1 ||
        copyData->cbData < offsetof(TrayNotifyData, nid) +
                               offsetof(NotifyIconData32, szTip)) {
        return;
    }
    const auto* data = (const TrayNotifyData*)copyData->lpData;
    if (data->signature != kTrayNotifySignature) {
        return;
    }
    NotifyIconData32 nid{};
    memcpy(&nid, &data->nid,
           std::min<size_t>(copyData->cbData - offsetof(TrayNotifyData, nid),
                            sizeof(nid)));
    nid.szTip[ARRAYSIZE(nid.szTip) - 1] = L'\0';
    const bool hasGuid = (nid.uFlags & NIF_GUID) &&
                         copyData->cbData >= offsetof(TrayNotifyData, nid) +
                                                 offsetof(NotifyIconData32,
                                                          hBalloonIcon);
    const HWND owner = (HWND)(LONG_PTR)(LONG)nid.hWnd;
    // Windows' own icons (volume, network, power, microphone...), which the
    // island shows its own way: {7820AE7x-23E3-4229-82C1-E41CB67D5B9C}.
    static const BYTE kSystemIconTail[] = {0x82, 0xC1, 0xE4, 0x1C,
                                           0xB6, 0x7D, 0x5B, 0x9C};
    if (hasGuid && (nid.guidItem.Data1 & 0xFFFFFFF0) == 0x7820AE70 &&
        nid.guidItem.Data2 == 0x23E3 && nid.guidItem.Data3 == 0x4229 &&
        memcmp(nid.guidItem.Data4, kSystemIconTail, 8) == 0) {
        return;
    }

    bool changed = false;
    AcquireSRWLockExclusive(&g_trayIconsLock);
    auto it = std::find_if(g_trayIcons.begin(), g_trayIcons.end(),
                           [&](const TrayIcon& icon) {
                               return hasGuid ? icon.hasGuid &&
                                                    icon.guid == nid.guidItem
                                              : !icon.hasGuid &&
                                                    icon.owner == owner &&
                                                    icon.id == nid.uID;
                           });
    switch (data->message) {
        case NIM_ADD:
        case NIM_MODIFY: {
            if (it == g_trayIcons.end()) {
                if (data->message == NIM_MODIFY) {
                    break;
                }
                TrayIcon icon;
                icon.owner = owner;
                icon.id = nid.uID;
                icon.hasGuid = hasGuid;
                icon.guid = nid.guidItem;
                GetWindowThreadProcessId(owner, &icon.processId);
                icon.key = TrayIconKey(icon);
                g_trayIcons.push_back(std::move(icon));
                it = g_trayIcons.end() - 1;
            }
            it->owner = owner;
            if (nid.uFlags & NIF_MESSAGE) {
                it->callback = nid.uCallbackMessage;
            }
            if ((nid.uFlags & NIF_ICON) && nid.hIcon) {
                // The app may destroy its handle right after: a copy.
                if (HICON copy = CopyIcon((HICON)(LONG_PTR)(LONG)nid.hIcon)) {
                    it->icon = std::make_shared<OwnedIcon>(copy);
                    it->serial = ++g_trayIconSerial;
                }
            }
            if (nid.uFlags & NIF_TIP) {
                it->tip = nid.szTip;
            }
            if ((nid.uFlags & NIF_STATE) && (nid.dwStateMask & NIS_HIDDEN)) {
                it->hidden = (nid.dwState & NIS_HIDDEN) != 0;
            }
            changed = true;
            break;
        }
        case NIM_DELETE:
            if (it != g_trayIcons.end()) {
                g_trayIcons.erase(it);
                changed = true;
            }
            break;
        case NIM_SETVERSION:
            if (it != g_trayIcons.end()) {
                it->version = nid.uVersion;
                changed = true;
            }
            break;
    }
    // Icons whose window is gone (the app closed without removing them).
    if (std::erase_if(g_trayIcons, [](const TrayIcon& icon) {
            return !IsWindow(icon.owner);
        })) {
        changed = true;
    }
    ReleaseSRWLockExclusive(&g_trayIconsLock);
    // Sent a moment later, once for all the changes until then: the app
    // that changed its icon is waiting for the taskbar meanwhile.
    if (changed && !g_trayIconsSendPending.exchange(true)) {
        if (HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr)) {
            PostMessage(taskbar, g_sendTrayIconsMessage, 0, 0);
        } else {
            g_trayIconsSendPending = false;
        }
    }
}

// To the island, in its own process: each icon's data, and its icon handle
// (icons can be used from any process of the session; the island makes its
// own copy while handling the message, while this one is kept alive).
//
// Nothing is locked while sending: waiting for the island, this thread still
// handles messages sent to it, like another app's tray icon (OnTrayNotify),
// which takes the lock.
void SendTrayIcons() {
    HWND island = FindIslandWnd();
    if (!island) {
        return;
    }
    IpcWriter writer;
    std::vector<std::shared_ptr<OwnedIcon>> keptAlive;
    AcquireSRWLockShared(&g_trayIconsLock);
    writer.Put<UINT32>((UINT32)g_trayIcons.size());
    for (const auto& icon : g_trayIcons) {
        writer.Put<UINT64>((UINT64)(ULONG_PTR)icon.owner);
        writer.Put<UINT32>(icon.id);
        writer.Put<GUID>(icon.guid);
        writer.Put<BYTE>(icon.hasGuid);
        writer.Put<UINT32>(icon.callback);
        writer.Put<UINT32>(icon.version);
        writer.Put<UINT64>(
            (UINT64)(ULONG_PTR)(icon.icon ? icon.icon->icon : nullptr));
        writer.Put<UINT32>(icon.serial);
        writer.Put<BYTE>(icon.hidden);
        writer.Put<UINT32>(icon.processId);
        writer.PutString(icon.tip);
        writer.PutString(icon.key);
        keptAlive.push_back(icon.icon);
    }
    ReleaseSRWLockShared(&g_trayIconsLock);
    SendIpc(island, FindCurrentProcessTaskbarWnd(), kIpcTrayIcons,
            writer.bytes.data(), (DWORD)writer.bytes.size());
}

// In the island's process: the icons explorer sent, each with the island's
// own copy of its icon (kept while it doesn't change).
void ReceiveTrayIcons(const COPYDATASTRUCT* copyData) {
    IpcReader reader(copyData);
    UINT32 count = 0;
    if (!reader.Get(&count)) {
        return;
    }
    std::vector<TrayIcon> icons;
    for (UINT32 i = 0; i < count && i < 512; i++) {
        TrayIcon icon;
        UINT64 owner = 0, handle = 0;
        BYTE hasGuid = 0, hidden = 0;
        if (!reader.Get(&owner) || !reader.Get(&icon.id) ||
            !reader.Get(&icon.guid) || !reader.Get(&hasGuid) ||
            !reader.Get(&icon.callback) || !reader.Get(&icon.version) ||
            !reader.Get(&handle) || !reader.Get(&icon.serial) ||
            !reader.Get(&hidden) || !reader.Get(&icon.processId) ||
            !reader.GetString(&icon.tip) || !reader.GetString(&icon.key)) {
            return;
        }
        icon.owner = (HWND)(ULONG_PTR)owner;
        icon.hasGuid = hasGuid != 0;
        icon.hidden = hidden != 0;
        icons.push_back(std::move(icon));
        // The copy made before, if it's the same icon.
        TrayIcon& added = icons.back();
        AcquireSRWLockShared(&g_trayIconsLock);
        for (const auto& old : g_trayIcons) {
            if (old.key == added.key && old.serial == added.serial) {
                added.icon = old.icon;
            }
        }
        ReleaseSRWLockShared(&g_trayIconsLock);
        if (!added.icon && handle) {
            if (HICON copy = CopyIcon((HICON)(ULONG_PTR)handle)) {
                added.icon = std::make_shared<OwnedIcon>(copy);
            }
        }
    }
    AcquireSRWLockExclusive(&g_trayIconsLock);
    g_trayIcons = std::move(icons);
    g_trayIconSerial++;
    ReleaseSRWLockExclusive(&g_trayIconsLock);
}

// Where each icon is on the island (or its arrow, when it's in the closed
// list), on the screen, as the island last drew them.
struct TrayIconPlace {
    HWND owner;
    UINT id;
    GUID guid;
    bool hasGuid;
    RECT rect;
};
SRWLOCK g_trayPlacesLock = SRWLOCK_INIT;
std::vector<TrayIconPlace> g_trayPlaces;

// An app asking where its icon is (Shell_NotifyIconGetRect): the request
// that comes to the taskbar window. Returns the answer, or 0 to leave it to
// the taskbar.
#pragma pack(push, 4)
struct NotifyIconIdentifier32 {
    DWORD signature;
    // 1: the top left corner, 2: the size.
    DWORD request;
    DWORD cbSize;
    DWORD hWndHigh;
    DWORD hWndLow;
    UINT uID;
    GUID guidItem;
};
#pragma pack(pop)

LRESULT OnTrayIconRectRequest(const COPYDATASTRUCT* copyData) {
    if (!g_settings.showIsland || !copyData ||
        copyData->dwData != 3 ||
        copyData->cbData < sizeof(NotifyIconIdentifier32)) {
        return 0;
    }
    const auto* request = (const NotifyIconIdentifier32*)copyData->lpData;
    if (request->signature != kTrayNotifySignature ||
        (request->request != 1 && request->request != 2)) {
        return 0;
    }
    static const GUID kNoGuid{};
    const bool byGuid = request->guidItem != kNoGuid;
    LRESULT result = 0;
    AcquireSRWLockShared(&g_trayPlacesLock);
    for (const auto& place : g_trayPlaces) {
        const DWORD owner = (DWORD)(ULONG_PTR)place.owner;
        const bool matches =
            byGuid ? place.hasGuid && place.guid == request->guidItem
                   : (owner == request->hWndLow || owner == request->hWndHigh) &&
                         place.id == request->uID;
        if (matches) {
            const RECT& r = place.rect;
            result = request->request == 1
                         ? MAKELONG((short)r.left, (short)r.top)
                         : MAKELONG((short)(r.right - r.left),
                                    (short)(r.bottom - r.top));
            break;
        }
    }
    ReleaseSRWLockShared(&g_trayPlacesLock);
    return result;
}

// Asks the apps to add their icons again (the message Windows sends when the
// taskbar restarts), at most once while Explorer runs: the icons are kept
// from then on, for the island to ask for them (kIpcHello). Only once the
// taskbar receives them (subclassed), and only for an island that shows them.
std::atomic<bool> g_trayIconsRequested;
// The taskbars subclassed (see SubclassTaskbars), defined further down.
extern std::vector<HWND> g_subclassedTaskbars;

void RequestTrayIcons() {
    if (g_trayIconsRequested || !g_settings.showIsland ||
        !g_settings.islandTray || !OwnsTaskbar() ||
        g_subclassedTaskbars.empty()) {
        return;
    }
    g_trayIconsRequested = true;
    static const UINT taskbarCreated = RegisterWindowMessage(L"TaskbarCreated");
    PostMessage(HWND_BROADCAST, taskbarCreated, 0, 0);
}

// Tells the taskbar's thread to send the island everything (see
// TaskbarSubclassProc).
const UINT g_sendToIslandMessage =
    RegisterWindowMessage(L"WindhawkMacIslandDock_SendToIsland");
const UINT g_requestTrayIconsMessage =
    RegisterWindowMessage(L"WindhawkMacIslandDock_RequestTrayIcons");

// Where the island drew the tray icons, sent by it.
void ReceiveTrayPlaces(const COPYDATASTRUCT* copyData) {
    IpcReader reader(copyData);
    UINT32 count = 0;
    if (!reader.Get(&count)) {
        return;
    }
    std::vector<TrayIconPlace> places;
    for (UINT32 i = 0; i < count && i < 512; i++) {
        UINT64 owner = 0;
        TrayIconPlace place{};
        BYTE hasGuid = 0;
        if (!reader.Get(&owner) || !reader.Get(&place.id) ||
            !reader.Get(&place.guid) || !reader.Get(&hasGuid) ||
            !reader.Get(&place.rect)) {
            return;
        }
        place.owner = (HWND)(ULONG_PTR)owner;
        place.hasGuid = hasGuid != 0;
        places.push_back(place);
    }
    AcquireSRWLockExclusive(&g_trayPlacesLock);
    g_trayPlaces = std::move(places);
    ReleaseSRWLockExclusive(&g_trayPlacesLock);
}

void WINAPI NotifyToastApps(void* parameter);

// Defined further down.
HICON AppIconForIsland(const COPYDATASTRUCT* copyData);
void WatchTaskbarPlace();
void StopWatchingTaskbarPlace();

// Also swallows the "show" timer while a full-screen app is in front, so the
// mouse at the bottom edge doesn't bring the taskbar up. Other ways of showing
// it, like the Windows key, aren't affected.
LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR) {
    if (uMsg == WM_TIMER && wParam == kUnhideTimerId && SlidesItself() &&
        g_settings.fullscreenGuard && IsFullscreenAppOnMonitor(hWnd)) {
        static ULONGLONG lastLogged;
        if (GetTickCount64() - lastLogged > 5000) {
            lastLogged = GetTickCount64();
            Wh_Log(L"Full-screen app in front, not showing the taskbar");
        }
        return 0;
    }
    // The other apps' tray icons, for the island (the taskbar still gets
    // them).
    // From the island (in its own process): send everything later (not
    // while it waits for the answer), where the tray icons are, and apps with
    // a new notification.
    if (uMsg == g_sendTrayIconsMessage && g_sendTrayIconsMessage) {
        g_trayIconsSendPending = false;
        SendTrayIcons();
        return 0;
    }
    if (uMsg == g_requestTrayIconsMessage && g_requestTrayIconsMessage) {
        RequestTrayIcons();
        return 0;
    }
    if (uMsg == g_sendToIslandMessage && g_sendToIslandMessage) {
        SendBadgeApps();
        SendTrayIcons();
        return 0;
    }
    // Moved to another edge of the screen (or back to the bottom).
    if (uMsg == WM_WINDOWPOSCHANGED && lParam && !g_unloading &&
        (((const WINDOWPOS*)lParam)->flags & (SWP_NOMOVE | SWP_NOSIZE)) !=
            (SWP_NOMOVE | SWP_NOSIZE) &&
        IsTaskbarAtBottom() != g_taskbarAtBottom) {
        WatchTaskbarPlace();
    }
    if (uMsg == WM_COPYDATA && lParam &&
        IsIpcSender((HWND)wParam, kIslandClassName)) {
        const auto* copyData = (const COPYDATASTRUCT*)lParam;
        switch (copyData->dwData) {
            case kIpcHello:
                // The island shows the tray icons now: they're asked for if
                // they haven't been yet.
                g_settings.islandTray = Wh_GetIntValue(L"islandTray", 1) != 0;
                g_settings.showIsland = true;
                PostMessage(hWnd, g_requestTrayIconsMessage, 0, 0);
                PostMessage(hWnd, g_sendToIslandMessage, 0, 0);
                return TRUE;
            case kIpcTrayPlaces:
                ReceiveTrayPlaces(copyData);
                return TRUE;
            case kIpcNotifiedApps: {
                std::vector<std::wstring> apps = SplitLines(copyData);
                NotifyToastApps(&apps);
                return TRUE;
            }
            case kIpcAppIcon:
                return (LRESULT)AppIconForIsland(copyData);
        }
    }
    if (uMsg == WM_COPYDATA) {
        // Where an icon is: on the island.
        if (const LRESULT place =
                OnTrayIconRectRequest((const COPYDATASTRUCT*)lParam)) {
            return place;
        }
        const LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        OnTrayNotify((const COPYDATASTRUCT*)lParam);
        return result;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// The shell hook message, and its code for a flashing window (lParam is the
// window).
UINT g_shellHookMessage;
constexpr WPARAM kShellHookFlash = 0x8006;  // HSHELL_FLASH

void OnAppFlashed(HWND hWnd);

LRESULT CALLBACK TaskbandSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR) {
    // Recorded before the taskbar handles it, since that's when it decides to
    // show itself (see TrayUI_SlideWindow_Hook).
    if (g_shellHookMessage && uMsg == g_shellHookMessage &&
        wParam == kShellHookFlash) {
        g_lastFlashTime = NowSeconds();
        OnAppFlashed((HWND)lParam);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

std::vector<HWND> g_subclassedTaskbars;
std::vector<HWND> g_subclassedTaskbands;

BOOL CALLBACK SubclassTaskbarProc(HWND hWnd, LPARAM) {
    DWORD processId = 0;
    WCHAR className[32] = L"";
    if (GetWindowThreadProcessId(hWnd, &processId) &&
        processId == GetCurrentProcessId() &&
        GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
         _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) &&
        std::find(g_subclassedTaskbars.begin(),
                  g_subclassedTaskbars.end(),
                  hWnd) == g_subclassedTaskbars.end()) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWnd, TaskbarSubclassProc, 0)) {
            g_subclassedTaskbars.push_back(hWnd);
        }
    }
    // The main taskbar's buttons live in its taskband window (made a
    // moment after the taskbar, when Explorer starts).
    if (processId == GetCurrentProcessId() &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        HWND taskband = (HWND)GetProp(hWnd, L"TaskbandHWND");
        if (taskband &&
            std::find(g_subclassedTaskbands.begin(),
                      g_subclassedTaskbands.end(),
                      taskband) == g_subclassedTaskbands.end() &&
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                taskband, TaskbandSubclassProc, 0)) {
            g_subclassedTaskbands.push_back(taskband);
        }
    }
    return TRUE;
}

void SubclassTaskbars() {
    EnumWindows(
        SubclassTaskbarProc,
        0);
}

void UnsubclassTaskbars() {
    for (HWND hWnd : g_subclassedTaskbars) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, TaskbarSubclassProc);
        }
    }
    g_subclassedTaskbars.clear();

    for (HWND hWnd : g_subclassedTaskbands) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, TaskbandSubclassProc);
        }
    }
    g_subclassedTaskbands.clear();
}

// Loaded with Explorer (or while it restarts), the taskbar isn't ready yet:
// tries again every second, for up to two minutes, until it's styled, then
// asks the apps for their tray icons again (they sent them before the mod
// could see them).
HANDLE g_lateStartThread;
HANDLE g_lateStartStop;

// The parts that need the taskbar's process (see OwnsTaskbar): started with
// the mod, or once the taskbar is ready.
void StartAttentionThread();
void StartEdgeWatch();
void StartTaskbarParts() {
    StartAttentionThread();
    StartEdgeWatch();
}

void UpdateAutoHide();

DWORD WINAPI LateStartThreadProc(LPVOID) {
    for (int tries = 0; tries < 120; tries++) {
        if (WaitForSingleObject(g_lateStartStop, 1000) != WAIT_TIMEOUT) {
            return 0;
        }
        if (!FindCurrentProcessTaskbarWnd()) {
            continue;
        }
        SubclassTaskbars();
        if (!g_mainTaskbarStyled) {
            ApplySettings();
        }
        if (g_mainTaskbarStyled) {
            Wh_Log(L"Taskbar ready after %d s", tries + 1);
            UpdateAutoHide();
            StartTaskbarParts();
            RequestTrayIcons();
            return 0;
        }
    }
    Wh_Log(L"Taskbar still not ready, giving up");
    return 0;
}

// Started when the taskbar wasn't ready.
void StartLateStart() {
    if (g_lateStartThread || g_mainTaskbarStyled) {
        return;
    }
    g_lateStartStop = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_lateStartStop) {
        return;
    }
    g_lateStartThread =
        CreateThread(nullptr, 0, LateStartThreadProc, nullptr, 0, nullptr);
    if (!g_lateStartThread) {
        CloseHandle(g_lateStartStop);
        g_lateStartStop = nullptr;
    }
}

void StopLateStart() {
    if (g_lateStartThread) {
        SetEvent(g_lateStartStop);
        WaitForSingleObject(g_lateStartThread, INFINITE);
        CloseHandle(g_lateStartThread);
        g_lateStartThread = nullptr;
    }
    if (g_lateStartStop) {
        CloseHandle(g_lateStartStop);
        g_lateStartStop = nullptr;
    }
}

// Whether the taskbar may hide now. A button asking for attention keeps it
// up; unless the whole dock is wanted for that, it hides as usual. An app
// clicked in the dock that's still opening keeps it up too.
using CTaskListWnd_PermitAutoHide_t = BOOL(WINAPI*)(void* pThis);
CTaskListWnd_PermitAutoHide_t CTaskListWnd_PermitAutoHide_Original;
BOOL WINAPI CTaskListWnd_PermitAutoHide_Hook(void* pThis) {
    if (!SlidesItself()) {
        return CTaskListWnd_PermitAutoHide_Original(pThis);
    }
    // An app clicked in the dock is opening (see KeepDockShown).
    if (!g_unloading && NowSeconds() < g_keepShownUntil) {
        return FALSE;
    }
    if (!g_unloading && IsDock() &&
        g_settings.attentionMode != AttentionMode::Dock &&
        std::any_of(g_dockIcons.begin(), g_dockIcons.end(),
                    [](const DockIcon& icon) { return icon.attention; })) {
        return TRUE;
    }
    return CTaskListWnd_PermitAutoHide_Original(pThis);
}

////////////////////////////////////////////////////////////////////////////////
// Bottom edge.
//
// While hidden, the taskbar keeps a thin strip on screen to notice the mouse.
// With the dock style that strip is transparent, and transparent parts don't
// always receive the mouse, so the bottom edge is watched here as well.
// Only while the mod slides the taskbar itself, and only often when the mouse
// is near the bottom (10 times a second otherwise).

HANDLE g_edgeThread;
HANDLE g_edgeStopEvent;

constexpr DWORD kEdgePollMs = 15;
// Far from the bottom edge, the mouse is looked at less often: it takes more
// than this long to get there.
constexpr DWORD kEdgeIdlePollMs = 100;
constexpr int kEdgeNearPixels = 300;
// While the mouse stays at the edge, the request is repeated in case it got
// lost.
constexpr DWORD kEdgeRetryMs = 150;

// Looking for the taskbar of a monitor.
struct TaskbarSearch {
    HMONITOR monitor;
    HWND result;
};

BOOL CALLBACK FindTaskbarForMonitorProc(HWND hWnd, LPARAM lParam) {
    auto* context = reinterpret_cast<TaskbarSearch*>(lParam);
    DWORD processId = 0;
    WCHAR className[32];
    if (GetWindowThreadProcessId(hWnd, &processId) &&
        processId == GetCurrentProcessId() &&
        GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
         _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) &&
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) ==
            context->monitor) {
        context->result = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindTaskbarForMonitor(HMONITOR monitor) {
    TaskbarSearch context{monitor, nullptr};
    EnumWindows(
        FindTaskbarForMonitorProc,
        reinterpret_cast<LPARAM>(&context));
    return context.result;
}

// Hidden: less than half of the taskbar is on its monitor.
bool IsTaskbarHidden(HWND hTaskbarWnd, const RECT& monitorRect) {
    RECT rect, visible;
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        return false;
    }
    if (!IntersectRect(&visible, &rect, &monitorRect)) {
        return true;
    }
    return (visible.bottom - visible.top) < (rect.bottom - rect.top) / 2;
}

DWORD WINAPI EdgeThreadProc(LPVOID) {
    ULONGLONG atEdgeSince = 0;
    ULONGLONG lastRequest = 0;

    DWORD wait = kEdgeIdlePollMs;
    while (WaitForSingleObject(g_edgeStopEvent, wait) == WAIT_TIMEOUT) {
        wait = kEdgeIdlePollMs;
        POINT pt;
        if (!GetCursorPos(&pt)) {
            continue;
        }
        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{sizeof(info)};
        if (!GetMonitorInfo(monitor, &info)) {
            atEdgeSince = 0;
            continue;
        }
        if (pt.y >= info.rcMonitor.bottom - kEdgeNearPixels) {
            wait = kEdgePollMs;
        }
        if (pt.y < info.rcMonitor.bottom - 1) {
            atEdgeSince = 0;
            continue;
        }
        // The transparent taskbar: only under its apps.
        if (IsTransparentTaskbar() && g_appsAreaRight > g_appsAreaLeft &&
            (pt.x < g_appsAreaLeft || pt.x >= g_appsAreaRight)) {
            atEdgeSince = 0;
            continue;
        }

        const ULONGLONG now = GetTickCount64();
        if (!atEdgeSince) {
            atEdgeSince = now;
        }
        if (now - atEdgeSince < (ULONGLONG)g_settings.showDelay ||
            now - lastRequest < kEdgeRetryMs) {
            continue;
        }

        HWND hTaskbarWnd = FindTaskbarForMonitor(monitor);
        if (!hTaskbarWnd || !IsTaskbarHidden(hTaskbarWnd, info.rcMonitor) ||
            (g_settings.fullscreenGuard &&
             IsFullscreenAppOnMonitor(hTaskbarWnd))) {
            continue;
        }

        // The same message the taskbar's own "show" timer sends.
        PostMessage(hTaskbarWnd, WM_TIMER, kUnhideTimerId, 0);
        lastRequest = now;
    }
    return 0;
}

void StartEdgeWatch() {
    // A transparent taskbar, hidden, doesn't get the mouse either.
    if (g_edgeThread || !SlidesItself() || !OwnsTaskbar()) {
        return;
    }
    g_edgeStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_edgeStopEvent) {
        return;
    }
    g_edgeThread = CreateThread(nullptr, 0, EdgeThreadProc, nullptr, 0, nullptr);
    if (!g_edgeThread) {
        CloseHandle(g_edgeStopEvent);
        g_edgeStopEvent = nullptr;
    }
}

void StopEdgeWatch() {
    if (g_edgeThread) {
        SetEvent(g_edgeStopEvent);
        WaitForSingleObject(g_edgeThread, INFINITE);
        CloseHandle(g_edgeThread);
        g_edgeThread = nullptr;
    }
    if (g_edgeStopEvent) {
        CloseHandle(g_edgeStopEvent);
        g_edgeStopEvent = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// New notifications (their own thread).
//
// A new message in a chat that's already unread doesn't change WhatsApp's
// badge, so the badge alone misses it. Windows keeps the notifications it shows
// in its notification database, and updates a shell state (WNF) with each one.
// When that state changes, the mod reads the new notifications from the
// database (read-only, with the system's SQLite, winsqlite3.dll), and each new
// one of an app on the dock counts as news (see OnAppNotified).

// Changes with each notification shown (and dismissed). Undocumented: if it
// doesn't exist on a Windows build, subscribing fails and only the badges are
// watched.
constexpr ULONGLONG kWnfNotificationsChanged = 0x0D83063EA3BC1035;

void StopToastWatch();

HANDLE g_toastThread;
HANDLE g_toastStopEvent;
HANDLE g_toastChangedEvent;
void* g_toastSubscription;

LONG NTAPI ToastStateCallback(ULONGLONG, ULONG, void*, void*, const void*,
                              ULONG) {
    if (HANDLE event = g_toastChangedEvent) {
        SetEvent(event);
    }
    return 0;
}

// The few functions of winsqlite3.dll that are used.
struct Sqlite {
    using open_v2_t = int(WINAPI*)(const char*, void**, int, const char*);
    using prepare_v2_t =
        int(WINAPI*)(void*, const char*, int, void**, const char**);
    using bind_int64_t = int(WINAPI*)(void*, int, long long);
    using step_t = int(WINAPI*)(void*);
    using column_int64_t = long long(WINAPI*)(void*, int);
    using column_text16_t = const void*(WINAPI*)(void*, int);
    using column_blob_t = const void*(WINAPI*)(void*, int);
    using column_bytes_t = int(WINAPI*)(void*, int);
    using finalize_t = int(WINAPI*)(void*);
    using close_t = int(WINAPI*)(void*);
    using busy_timeout_t = int(WINAPI*)(void*, int);

    static constexpr int kOpenReadOnly = 0x00000001;
    static constexpr int kRow = 100;

    HMODULE module = nullptr;
    open_v2_t open_v2 = nullptr;
    prepare_v2_t prepare_v2 = nullptr;
    bind_int64_t bind_int64 = nullptr;
    step_t step = nullptr;
    column_int64_t column_int64 = nullptr;
    column_text16_t column_text16 = nullptr;
    column_blob_t column_blob = nullptr;
    column_bytes_t column_bytes = nullptr;
    finalize_t finalize = nullptr;
    close_t close = nullptr;
    busy_timeout_t busy_timeout = nullptr;

    bool Load() {
        module = LoadLibraryEx(L"winsqlite3.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            return false;
        }
        open_v2 = (open_v2_t)GetProcAddress(module, "sqlite3_open_v2");
        prepare_v2 = (prepare_v2_t)GetProcAddress(module, "sqlite3_prepare_v2");
        bind_int64 = (bind_int64_t)GetProcAddress(module, "sqlite3_bind_int64");
        step = (step_t)GetProcAddress(module, "sqlite3_step");
        column_int64 =
            (column_int64_t)GetProcAddress(module, "sqlite3_column_int64");
        column_text16 =
            (column_text16_t)GetProcAddress(module, "sqlite3_column_text16");
        column_blob = (column_blob_t)GetProcAddress(module, "sqlite3_column_blob");
        column_bytes =
            (column_bytes_t)GetProcAddress(module, "sqlite3_column_bytes");
        finalize = (finalize_t)GetProcAddress(module, "sqlite3_finalize");
        close = (close_t)GetProcAddress(module, "sqlite3_close");
        busy_timeout =
            (busy_timeout_t)GetProcAddress(module, "sqlite3_busy_timeout");
        return open_v2 && prepare_v2 && bind_int64 && step && column_int64 &&
               column_text16 && column_blob && column_bytes && finalize &&
               close && busy_timeout;
    }

    void Unload() {
        if (module) {
            FreeLibrary(module);
            module = nullptr;
        }
    }
};

// Whether notifications show their title and text (on the island), or only
// their app. Switched in the island's notifications, and remembered.
std::atomic<bool> g_showNotificationContent{true};
// New notifications in the pill (instead of the Windows banner), and also
// with the pill minimized. Their settings, switched in the island too.
std::atomic<bool> g_bannerEnabled{true};
std::atomic<bool> g_bannerWhenMinimized{false};

// What a notification offers besides its text: opening it (its own
// arguments, for the app to open the right place, like a chat), and a reply
// box with the button that sends it (like messaging apps' notifications).
struct ToastReply {
    std::wstring launch;
    std::wstring inputId;
    std::wstring placeholder;
    // The button that sends the reply.
    std::wstring arguments;
    std::wstring activationType;
    std::wstring label;
    bool CanReply() const { return !inputId.empty() && !arguments.empty(); }
};

// Defined further down.
ToastReply ParseToastReply(const std::wstring& appId,
                           const char* xml,
                           int size);

// A new notification, for the island to show (see Island::ShowBanner).
struct IslandToast {
    std::wstring appId;
    std::wstring title;
    std::wstring text;
    ToastReply reply;
    // Stays until closed or clicked (a reminder).
    bool sticky = false;
    // The picture replacing the app's icon (like a contact's photo), as a
    // file, and whether it's cut round.
    std::wstring image;
    bool imageCircle = false;
    // A picture on the web, downloaded (see islandWebPictures).
    std::vector<BYTE> imageData;
};
SRWLOCK g_islandToastsLock = SRWLOCK_INIT;
std::vector<IslandToast> g_islandToasts;
// Tells the island there are new ones.
constexpr UINT WM_APP_TOAST = WM_APP + 52;

// Defined further down.
std::vector<std::wstring> ToastTexts(const char* xml, int size);
bool ToastImage(const std::wstring& appId,
                const char* xml,
                int size,
                std::wstring* path,
                bool* circle);
bool DownloadPicture(const std::wstring& url, std::vector<BYTE>* data);
bool PackageGoesOnline(const std::wstring& appId);

// The toasts newer than `lastId`: the AppUserModelIDs of their apps, and the
// toasts themselves; moves `lastId` past them. Returns false if the database
// can't be read.
bool ReadNewToasts(Sqlite& sqlite,
                   const std::string& path,
                   long long& lastId,
                   std::vector<std::wstring>& apps,
                   std::vector<IslandToast>& toasts) {
    void* db = nullptr;
    if (sqlite.open_v2(path.c_str(), &db, Sqlite::kOpenReadOnly, nullptr) !=
        0) {
        if (db) {
            sqlite.close(db);
        }
        return false;
    }
    sqlite.busy_timeout(db, 500);
    void* statement = nullptr;
    const bool ok =
        sqlite.prepare_v2(db,
                          "SELECT n.Id, h.PrimaryId, n.Payload FROM "
                          "Notification n JOIN NotificationHandler h ON "
                          "h.RecordId = n.HandlerId WHERE n.Id > ?1 AND "
                          "n.Type = 'toast' ORDER BY n.Id",
                          -1, &statement, nullptr) == 0;
    if (ok) {
        sqlite.bind_int64(statement, 1, lastId);
        while (sqlite.step(statement) == Sqlite::kRow) {
            lastId = std::max(lastId, sqlite.column_int64(statement, 0));
            auto app = (PCWSTR)sqlite.column_text16(statement, 1);
            if (!app) {
                continue;
            }
            if (std::find(apps.begin(), apps.end(), app) == apps.end()) {
                apps.push_back(app);
            }
            IslandToast toast{app};
            const auto payload = (const char*)sqlite.column_blob(statement, 2);
            const auto texts =
                payload ? ToastTexts(payload, sqlite.column_bytes(statement, 2))
                        : std::vector<std::wstring>{};
            if (!texts.empty()) {
                toast.title = texts[0];
            }
            for (size_t i = 1; i < texts.size(); i++) {
                toast.text += (i > 1 ? L" " : L"") + texts[i];
            }
            if (payload) {
                toast.reply = ParseToastReply(
                    app, payload, sqlite.column_bytes(statement, 2));
                ToastImage(app, payload, sqlite.column_bytes(statement, 2),
                           &toast.image, &toast.imageCircle);
                if (toast.image.starts_with(L"https://") &&
                    !DownloadPicture(toast.image, &toast.imageData)) {
                    toast.image.clear();
                }
            }
            if (!toast.title.empty() || !toast.text.empty()) {
                toasts.push_back(std::move(toast));
            }
        }
        sqlite.finalize(statement);
    }
    sqlite.close(db);
    return ok;
}

// The newest notification's ID, to start after it.
bool ReadLastToastId(Sqlite& sqlite, const std::string& path, long long& id) {
    void* db = nullptr;
    if (sqlite.open_v2(path.c_str(), &db, Sqlite::kOpenReadOnly, nullptr) !=
        0) {
        if (db) {
            sqlite.close(db);
        }
        return false;
    }
    sqlite.busy_timeout(db, 500);
    void* statement = nullptr;
    bool ok = sqlite.prepare_v2(db, "SELECT MAX(Id) FROM Notification", -1,
                                &statement, nullptr) == 0;
    if (ok) {
        id = sqlite.step(statement) == Sqlite::kRow
                 ? sqlite.column_int64(statement, 0)
                 : 0;
        sqlite.finalize(statement);
    }
    sqlite.close(db);
    return ok;
}

// On the taskbar thread: the apps' buttons count it as news.
void WINAPI NotifyToastApps(void* parameter) {
    const auto& apps = *(const std::vector<std::wstring>*)parameter;
    if (g_unloading) {
        return;
    }
    for (const auto& app : apps) {
        for (auto& icon : g_dockIcons) {
            auto button = icon.button.get();
            if (button && _wcsicmp(GetButtonAppId(button).c_str(),
                                   app.c_str()) == 0) {
                OnAppNotified(button);
                break;
            }
        }
    }
}

// The notification database's path, in UTF-8 for SQLite.
std::string GetNotificationDatabasePath() {
    WCHAR widePath[MAX_PATH];
    std::string path;
    if (ExpandEnvironmentStrings(
            L"%LOCALAPPDATA%\\Microsoft\\Windows\\Notifications\\"
            L"wpndatabase.db",
            widePath, ARRAYSIZE(widePath))) {
        const int size = WideCharToMultiByte(CP_UTF8, 0, widePath, -1,
                                             nullptr, 0, nullptr, nullptr);
        if (size > 0) {
            path.resize(size);
            WideCharToMultiByte(CP_UTF8, 0, widePath, -1, path.data(), size,
                                nullptr, nullptr);
            path.resize(size - 1);
        }
    }
    return path;
}

// New notifications are being read (the pill can show them).
std::atomic<bool> g_toastWatchReady;

DWORD WINAPI ToastThreadProc(LPVOID) {
    Sqlite sqlite;
    const std::string path = GetNotificationDatabasePath();
    long long lastId = 0;
    if (path.empty() || !sqlite.Load() ||
        !ReadLastToastId(sqlite, path, lastId)) {
        Wh_Log(L"The notification database can't be read");
        sqlite.Unload();
        return 0;
    }
    Wh_Log(L"Watching new notifications after %lld", lastId);
    g_toastWatchReady = true;

    const HANDLE events[] = {g_toastStopEvent, g_toastChangedEvent};
    while (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE,
                                  INFINITE) == WAIT_OBJECT_0 + 1) {
        // Written right after the state changes; a moment for it to land.
        if (WaitForSingleObject(g_toastStopEvent, 150) == WAIT_OBJECT_0) {
            break;
        }
        std::vector<std::wstring> apps;
        std::vector<IslandToast> toasts;
        if (!ReadNewToasts(sqlite, path, lastId, apps, toasts) ||
            apps.empty()) {
            continue;
        }
        // The island shows the latest one.
        if (!toasts.empty() && g_bannerEnabled) {
            AcquireSRWLockExclusive(&g_islandToastsLock);
            g_islandToasts = std::move(toasts);
            ReleaseSRWLockExclusive(&g_islandToastsLock);
            if (HWND island = g_islandWnd) {
                PostMessage(island, WM_APP_TOAST, 0, 0);
            }
        }
        // Explorer makes their icons bounce.
        SendLines(FindTaskbarWnd(), g_islandWnd, kIpcNotifiedApps, apps);
    }
    sqlite.Unload();
    return 0;
}

void StartToastWatch() {
    if (g_toastThread) {
        return;
    }
    HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
    using Subscribe_t = LONG(NTAPI*)(void**, ULONGLONG, ULONG, void*, void*,
                                     const void*, ULONG, ULONG);
    using Query_t = LONG(NTAPI*)(const ULONGLONG*, const void*, const void*,
                                 ULONG*, void*, ULONG*);
    auto subscribe = (Subscribe_t)GetProcAddress(
        ntdll, "RtlSubscribeWnfStateChangeNotification");
    auto query = (Query_t)GetProcAddress(ntdll, "NtQueryWnfStateData");
    if (!subscribe || !query) {
        return;
    }
    g_toastStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_toastChangedEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_toastStopEvent || !g_toastChangedEvent) {
        StopToastWatch();
        return;
    }
    g_toastThread =
        CreateThread(nullptr, 0, ToastThreadProc, nullptr, 0, nullptr);
    if (!g_toastThread) {
        StopToastWatch();
        return;
    }
    // Told of changes after the current one only.
    ULONG changeStamp = 0;
    BYTE buffer[64];
    ULONG size = sizeof(buffer);
    query(&kWnfNotificationsChanged, nullptr, nullptr, &changeStamp, buffer,
          &size);
    if (subscribe(&g_toastSubscription, kWnfNotificationsChanged, changeStamp,
                  (void*)ToastStateCallback, nullptr, nullptr, 0, 0) < 0) {
        g_toastSubscription = nullptr;
        Wh_Log(L"Can't watch new notifications");
        StopToastWatch();
    }
}

void StopToastWatch() {
    g_toastWatchReady = false;
    if (g_toastSubscription) {
        using Unsubscribe_t = LONG(NTAPI*)(void*);
        if (auto unsubscribe = (Unsubscribe_t)GetProcAddress(
                GetModuleHandle(L"ntdll.dll"),
                "RtlUnsubscribeWnfStateChangeNotification")) {
            unsubscribe(g_toastSubscription);
        }
        g_toastSubscription = nullptr;
    }
    if (g_toastThread) {
        SetEvent(g_toastStopEvent);
        WaitForSingleObject(g_toastThread, INFINITE);
        CloseHandle(g_toastThread);
        g_toastThread = nullptr;
    }
    for (HANDLE* event : {&g_toastStopEvent, &g_toastChangedEvent}) {
        if (*event) {
            CloseHandle(*event);
            *event = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// The attention icon (its own thread).
//
// When an app asks for attention while the dock is hidden, only its icon peeks
// out from the bottom edge, bounces, and goes away again. The dock's buttons
// are off screen then, so the icon is drawn in a small window of its own.

HANDLE g_attentionThread;
DWORD g_attentionThreadId;
HMODULE g_module;
std::atomic<bool> g_attentionClicked;

constexpr UINT WM_APP_ATTENTION = WM_APP + 30;  // lParam: the app's window.
constexpr WCHAR kAttentionClassName[] = L"WindhawkMacDockAttentionIcon";

// Sizes in pixels at 100% scaling: the icon, how high it bounces, and how far
// above the bottom edge it rests.
constexpr int kAttentionIconSize = 48;
constexpr int kAttentionBounceHeight = 22;
constexpr int kAttentionRestGap = 10;
// How long it takes to come out and to go back, in seconds.
constexpr double kAttentionRiseTime = 0.28;
constexpr double kAttentionSinkTime = 0.24;
// The same app doesn't bounce again this soon after, in seconds.
constexpr double kAttentionRepeatDelay = 4;

constexpr GUID kFOLDERID_AppsFolder = {
    0x1e87508d,
    0x89c2,
    0x42f0,
    {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};
constexpr GUID kFOLDERID_LocalAppData = {
    0xf1b32785,
    0x6fba,
    0x4fcf,
    {0x9d, 0x55, 0x7b, 0x8e, 0x7f, 0x15, 0x70, 0x91}};
constexpr PROPERTYKEY kPKEY_AppUserModel_ID = {
    {0x9f4c2855,
     0x9f79,
     0x4b39,
     {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}},
    5};

// What RunAttentionIcon shows: the app's window (a flashing button), or its
// AppUserModelID (a badge, see OnAppNotified).
struct AttentionRequest {
    HWND app;
    std::wstring appId;
};

void PostAttentionRequest(HWND app, std::wstring appId) {
    if (!g_attentionThreadId) {
        return;
    }
    auto* request = new AttentionRequest{app, std::move(appId)};
    if (!PostThreadMessage(g_attentionThreadId, WM_APP_ATTENTION, 0,
                           (LPARAM)request)) {
        delete request;
    }
}

void OnAppFlashed(HWND hWnd) {
    if (IsDock() && g_settings.attentionMode == AttentionMode::Icon) {
        PostAttentionRequest(hWnd, {});
    }
}

std::wstring GetWindowAppId(HWND hWnd) {
    std::wstring appId;
    winrt::com_ptr<IPropertyStore> store;
    if (hWnd && SUCCEEDED(SHGetPropertyStoreForWindow(
                    hWnd, IID_PPV_ARGS(store.put())))) {
        PROPVARIANT value;
        PropVariantInit(&value);
        if (SUCCEEDED(store->GetValue(kPKEY_AppUserModel_ID, &value)) &&
            value.vt == VT_LPWSTR && value.pwszVal) {
            appId = value.pwszVal;
        }
        PropVariantClear(&value);
    }
    // Packaged apps (installed from the Store, like WhatsApp) have theirs on
    // their process, not on their windows.
    if (appId.empty() && hWnd) {
        DWORD processId = 0;
        GetWindowThreadProcessId(hWnd, &processId);
        if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                         FALSE, processId)) {
            // An AppUserModelID has at most 130 characters.
            WCHAR id[256] = L"";
            UINT32 length = ARRAYSIZE(id);
            if (GetApplicationUserModelId(process, &length, id) ==
                ERROR_SUCCESS) {
                appId = id;
            }
            CloseHandle(process);
        }
    }
    return appId;
}

// An app to remember things for (its place): its ID, or its program's path
// when it has none.
std::wstring WindowAppKey(HWND hWnd) {
    std::wstring key = GetWindowAppId(hWnd);
    if (!key.empty() || !hWnd) {
        return key;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (HANDLE process =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId)) {
        WCHAR path[MAX_PATH];
        DWORD length = ARRAYSIZE(path);
        if (QueryFullProcessImageName(process, 0, path, &length)) {
            key = path;
        }
        CloseHandle(process);
    }
    return key;
}

// The app's icon as a 32-bit bitmap with premultiplied alpha, from the shell:
// by the app's AppUserModelID if it has one, by its program file otherwise.
HBITMAP LoadAppIconBitmap(HWND hWnd, std::wstring appId, int size) {
    winrt::com_ptr<IShellItemImageFactory> factory;

    if (appId.empty()) {
        appId = GetWindowAppId(hWnd);
    }
    if (!appId.empty()) {
        SHCreateItemInKnownFolder(kFOLDERID_AppsFolder, 0, appId.c_str(),
                                  IID_PPV_ARGS(factory.put()));
    }
    // Notifications of a packaged app may come under an ID of their own (like
    // Phone Link's, one for each phone app): its package's app then.
    if (const size_t bang = appId.find(L'!');
        !factory && bang != std::wstring::npos) {
        const std::wstring packageApp = appId.substr(0, bang) + L"!App";
        if (packageApp != appId) {
            SHCreateItemInKnownFolder(kFOLDERID_AppsFolder, 0,
                                      packageApp.c_str(),
                                      IID_PPV_ARGS(factory.put()));
        }
    }

    if (!factory && hWnd) {
        DWORD processId = 0;
        GetWindowThreadProcessId(hWnd, &processId);
        HANDLE process =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (process) {
            WCHAR path[MAX_PATH];
            DWORD length = ARRAYSIZE(path);
            if (QueryFullProcessImageName(process, 0, path, &length)) {
                SHCreateItemFromParsingName(path, nullptr,
                                            IID_PPV_ARGS(factory.put()));
            }
            CloseHandle(process);
        }
    }

    HBITMAP bitmap = nullptr;
    if (factory &&
        FAILED(factory->GetImage({size, size},
                                 SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                                 &bitmap))) {
        bitmap = nullptr;
    }
    return bitmap;
}

// The icons given to the island (see kIpcAppIcon), by app and size. Only used
// on the taskbar's thread.
std::vector<std::pair<std::wstring, HICON>> g_islandAppIcons;

HICON AppIconForIsland(const COPYDATASTRUCT* copyData) {
    IpcReader reader(copyData);
    int size = 0;
    std::wstring appId;
    if (!reader.Get(&size) || !reader.GetString(&appId) || appId.empty() ||
        size <= 0 || size > 256) {
        return nullptr;
    }
    const std::wstring key = std::to_wstring(size) + L"|" + appId;
    for (const auto& [id, icon] : g_islandAppIcons) {
        if (id == key) {
            return icon;
        }
    }
    HICON icon = nullptr;
    if (HBITMAP bitmap = LoadAppIconBitmap(nullptr, appId, size)) {
        // An icon's colors aren't premultiplied by their alpha.
        BITMAP info{};
        GetObject(bitmap, sizeof(info), &info);
        const int width = info.bmWidth;
        const int height = std::abs(info.bmHeight);
        BITMAPINFO header{};
        header.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32,
                            BI_RGB};
        void* bits = nullptr;
        HDC screen = GetDC(nullptr);
        HBITMAP color = width > 0 && height > 0
                            ? CreateDIBSection(screen, &header, DIB_RGB_COLORS,
                                               &bits, nullptr, 0)
                            : nullptr;
        if (color &&
            GetDIBits(screen, bitmap, 0, height, bits, &header,
                      DIB_RGB_COLORS) == height) {
            auto* pixel = (BYTE*)bits;
            for (int i = 0; i < width * height; i++, pixel += 4) {
                if (const BYTE alpha = pixel[3]; alpha && alpha < 255) {
                    for (int c = 0; c < 3; c++) {
                        pixel[c] = (BYTE)std::min(255, pixel[c] * 255 / alpha);
                    }
                }
            }
            HBITMAP mask = CreateBitmap(width, height, 1, 1, nullptr);
            ICONINFO iconInfo{TRUE, 0, 0, mask, color};
            icon = CreateIconIndirect(&iconInfo);
            if (mask) {
                DeleteObject(mask);
            }
        }
        if (color) {
            DeleteObject(color);
        }
        ReleaseDC(nullptr, screen);
        DeleteObject(bitmap);
    }
    // Remembered even when missing, so it isn't looked up again; the oldest
    // go past a limit (the island has copied them already).
    constexpr size_t kMaxIslandAppIcons = 128;
    if (g_islandAppIcons.size() >= kMaxIslandAppIcons) {
        for (size_t i = 0; i < kMaxIslandAppIcons / 2; i++) {
            if (g_islandAppIcons[i].second) {
                DestroyIcon(g_islandAppIcons[i].second);
            }
        }
        g_islandAppIcons.erase(g_islandAppIcons.begin(),
                               g_islandAppIcons.begin() +
                                   kMaxIslandAppIcons / 2);
    }
    g_islandAppIcons.push_back({key, icon});
    return icon;
}

void FreeIslandAppIcons() {
    for (const auto& [id, icon] : g_islandAppIcons) {
        if (icon) {
            DestroyIcon(icon);
        }
    }
    g_islandAppIcons.clear();
}

void ActivateApp(HWND hWnd) {
    if (!IsWindow(hWnd)) {
        return;
    }
    if (IsIconic(hWnd)) {
        ShowWindowAsync(hWnd, SW_RESTORE);
    }
    HWND popup = GetLastActivePopup(hWnd);
    SetForegroundWindow(popup && IsWindowVisible(popup) ? popup : hWnd);
}

// Whether a window belongs to the app with this AppUserModelID. Apps without
// one of their own are told apart by their program file, which the taskbar
// then uses as their ID.
bool WindowMatchesApp(HWND hWnd, const std::wstring& appId) {
    const std::wstring id = GetWindowAppId(hWnd);
    if (!id.empty()) {
        return _wcsicmp(id.c_str(), appId.c_str()) == 0;
    }
    if (appId.find(L'\\') == std::wstring::npos) {
        return false;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }
    WCHAR path[MAX_PATH];
    DWORD length = ARRAYSIZE(path);
    const bool matches =
        QueryFullProcessImageName(process, 0, path, &length) &&
        _wcsicmp(path, appId.c_str()) == 0;
    CloseHandle(process);
    return matches;
}

// Looking for an app's window.
struct AppWindowSearch {
    const std::wstring* appId;
    HWND found;
};

BOOL CALLBACK FindAppWindowProc(HWND hWnd, LPARAM lParam) {
    auto* context = reinterpret_cast<AppWindowSearch*>(lParam);
    if (!IsWindowVisible(hWnd) || GetWindow(hWnd, GW_OWNER) ||
        (GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) {
        return TRUE;
    }
    if (WindowMatchesApp(hWnd, *context->appId)) {
        context->found = hWnd;
        return FALSE;
    }
    return TRUE;
}

// Brings an app to the front by its AppUserModelID: its window if it has one
// (restored if minimized), otherwise it's started, or brought back from the
// tray, through the shell.
void OpenApp(const std::wstring& appId) {
    if (appId.empty()) {
        return;
    }
    AppWindowSearch context{&appId, nullptr};
    EnumWindows(
        FindAppWindowProc,
        (LPARAM)&context);
    if (context.found) {
        ActivateApp(context.found);
        return;
    }
    const std::wstring target = appId.find(L'\\') != std::wstring::npos
                                    ? appId
                                    : L"shell:AppsFolder\\" + appId;
    ShellExecute(nullptr, L"open", target.c_str(), nullptr, nullptr,
                 SW_SHOWNORMAL);
}

LRESULT CALLBACK AttentionWndProc(HWND hWnd,
                                  UINT uMsg,
                                  WPARAM wParam,
                                  LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_LBUTTONDOWN:
            g_attentionClicked = true;
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Whether the app is the one in front, so it doesn't need the icon.
bool IsAppInFront(const AttentionRequest& request) {
    HWND foreground = GetForegroundWindow();
    if (request.app) {
        return foreground == request.app;
    }
    return !request.appId.empty() &&
           GetWindowAppId(foreground) == request.appId;
}

// Shows the app's icon peeking out and bouncing, then hides it. Returns false
// if the thread should quit.
bool RunAttentionIcon(HWND window, const AttentionRequest& request) {
    static std::wstring lastKey;
    static double lastEnd;

    const HWND app = request.app;
    const std::wstring key =
        app ? L"#" + std::to_wstring((ULONG_PTR)app) : request.appId;
    if (g_settings.attentionMode != AttentionMode::Icon ||
        (app && !IsWindow(app)) || (!app && request.appId.empty()) ||
        IsAppInFront(request) ||
        (key == lastKey && NowSeconds() - lastEnd < kAttentionRepeatDelay)) {
        return true;
    }

    HWND taskbar = FindCurrentProcessTaskbarWnd();
    MONITORINFO monitor{sizeof(monitor)};
    if (!taskbar ||
        !GetMonitorInfo(MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST),
                        &monitor)) {
        return true;
    }
    // With the dock on screen, the icon bounces in it instead. Full-screen
    // apps aren't disturbed.
    if (!IsTaskbarHidden(taskbar, monitor.rcMonitor) ||
        (g_settings.fullscreenGuard && IsFullscreenAppOnMonitor(taskbar))) {
        return true;
    }

    // Above the app's button, measured by the taskbar when the app started
    // asking for attention. That can come a little after this.
    const double asked = NowSeconds();
    int centerX = (monitor.rcMonitor.left + monitor.rcMonitor.right) / 2;
    for (int i = 0; i < 30; i++) {
        if (g_attentionButtonTime >= asked - 1.0) {
            centerX = g_attentionButtonX;
            break;
        }
        Sleep(10);
    }

    const UINT dpi = GetDpiForWindow(taskbar);
    const int iconSize = MulDiv(kAttentionIconSize, dpi, 96);
    const int bounceHeight = MulDiv(kAttentionBounceHeight, dpi, 96);
    const int restGap = MulDiv(kAttentionRestGap, dpi, 96);

    HBITMAP icon = LoadAppIconBitmap(app, request.appId, iconSize);
    if (!icon) {
        return true;
    }
    BITMAP iconInfo{};
    GetObject(icon, sizeof(iconInfo), &iconInfo);

    const SIZE size{iconSize, iconSize + bounceHeight + restGap};
    const POINT position{centerX - size.cx / 2,
                         monitor.rcMonitor.bottom - size.cy};
    // The icon's top edge, in the window: hidden below it, and resting.
    const double hiddenTop = size.cy;
    const double restTop = size.cy - restGap - iconSize;

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = size.cx;
    info.bmiHeader.biHeight = -size.cy;  // Top-down.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC canvas = CreateCompatibleDC(screen);
    HDC iconDc = CreateCompatibleDC(screen);
    HBITMAP canvasBitmap =
        CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!canvas || !iconDc || !canvasBitmap) {
        if (canvasBitmap) {
            DeleteObject(canvasBitmap);
        }
        if (canvas) {
            DeleteDC(canvas);
        }
        if (iconDc) {
            DeleteDC(iconDc);
        }
        DeleteObject(icon);
        return true;
    }
    HGDIOBJ oldCanvas = SelectObject(canvas, canvasBitmap);
    HGDIOBJ oldIcon = SelectObject(iconDc, icon);

    const BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    g_attentionClicked = false;
    bool keepRunning = true;
    bool shown = false;
    const double start = NowSeconds();
    // When it starts going back, once the bounces are over or it's not needed
    // anymore.
    double sinkStart = 0;
    // The app in front is only looked up when the foreground window changes.
    HWND lastForeground = nullptr;
    bool appInFront = false;

    for (;;) {
        const double now = NowSeconds();
        if (HWND foreground = GetForegroundWindow();
            foreground != lastForeground) {
            lastForeground = foreground;
            appInFront = IsAppInFront(request);
        }
        if (!sinkStart &&
            (g_attentionClicked || appInFront ||
             !IsTaskbarHidden(taskbar, monitor.rcMonitor) || g_unloading)) {
            if (g_attentionClicked) {
                if (app) {
                    ActivateApp(app);
                } else {
                    OpenApp(request.appId);
                }
            }
            sinkStart = now;
        }

        double top;
        const double elapsed = now - start;
        if (!sinkStart && elapsed < kAttentionRiseTime) {
            top = hiddenTop + (restTop - hiddenTop) *
                                  SpringOut(elapsed / kAttentionRiseTime);
        } else if (!sinkStart) {
            bool done;
            const double height =
                BounceSequence(elapsed - kAttentionRiseTime,
                               kAttentionBounceGroups, &done);
            top = restTop - bounceHeight * height;
            if (done) {
                sinkStart = now;
            }
        } else {
            const double t = std::min((now - sinkStart) / kAttentionSinkTime, 1.0);
            top = restTop + (hiddenTop - restTop) * EaseInOutCubic(t);
            if (t >= 1) {
                break;
            }
        }

        ZeroMemory(bits, size.cx * size.cy * 4);
        AlphaBlend(canvas, 0, (int)std::lround(top), iconSize, iconSize, iconDc,
                   0, 0, iconInfo.bmWidth, std::abs(iconInfo.bmHeight), blend);
        POINT source{0, 0};
        UpdateLayeredWindow(window, nullptr, (POINT*)&position, (SIZE*)&size,
                            canvas, &source, 0, (BLENDFUNCTION*)&blend,
                            ULW_ALPHA);
        if (!shown) {
            SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_SHOWWINDOW);
            shown = true;
        }

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                keepRunning = false;
                break;
            }
            // Another app asking meanwhile is skipped.
            if (msg.message == WM_APP_ATTENTION && !msg.hwnd) {
                delete (AttentionRequest*)msg.lParam;
                continue;
            }
            DispatchMessage(&msg);
        }
        if (!keepRunning) {
            break;
        }
        // Waits for the next frame.
        DwmFlush();
    }

    ShowWindow(window, SW_HIDE);
    SelectObject(iconDc, oldIcon);
    SelectObject(canvas, oldCanvas);
    DeleteObject(canvasBitmap);
    DeleteDC(iconDc);
    DeleteDC(canvas);
    DeleteObject(icon);

    lastKey = key;
    lastEnd = NowSeconds();
    return keepRunning;
}

DWORD WINAPI AttentionThreadProc(LPVOID parameter) {
    // Create the message queue before the creator continues.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent((HANDLE)parameter);

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT comResult =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASS windowClass{};
    windowClass.lpfnWndProc = AttentionWndProc;
    windowClass.hInstance = g_module;
    windowClass.hCursor = LoadCursor(nullptr, IDC_HAND);
    windowClass.lpszClassName = kAttentionClassName;
    RegisterClass(&windowClass);

    HWND window = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kAttentionClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        g_module, nullptr);
    if (!window) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
    }

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_ATTENTION && !msg.hwnd) {
            std::unique_ptr<AttentionRequest> request{
                (AttentionRequest*)msg.lParam};
            if (window && !RunAttentionIcon(window, *request)) {
                break;
            }
            continue;
        }
        DispatchMessage(&msg);
    }

    // Requests that came in after the end.
    while (PeekMessage(&msg, nullptr, WM_APP_ATTENTION, WM_APP_ATTENTION,
                       PM_REMOVE)) {
        if (!msg.hwnd) {
            delete (AttentionRequest*)msg.lParam;
        }
    }

    if (window) {
        DestroyWindow(window);
    }
    UnregisterClass(kAttentionClassName, g_module);
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return 0;
}

void StartAttentionThread() {
    if (g_attentionThread || !OwnsTaskbar()) {
        return;
    }
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&AttentionWndProc, &g_module);
    HANDLE started = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!started) {
        return;
    }
    g_attentionThread = CreateThread(nullptr, 0, AttentionThreadProc, started,
                                     0, &g_attentionThreadId);
    if (g_attentionThread) {
        WaitForSingleObject(started, INFINITE);
    } else {
        g_attentionThreadId = 0;
    }
    CloseHandle(started);
}

void StopAttentionThread() {
    if (!g_attentionThread) {
        return;
    }
    PostThreadMessage(g_attentionThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_attentionThread, INFINITE);
    CloseHandle(g_attentionThread);
    g_attentionThread = nullptr;
    g_attentionThreadId = 0;
}

////////////////////////////////////////////////////////////////////////////////
// The island at the top (its own thread).
//
// The clock, network, volume, battery and notifications at the top of the
// screen: a black pill at the top center (which can be minimized to a thin
// line), or a full-width bar like the macOS menu bar, which reserves its strip
// so maximized windows start below it. The taskbar is at the bottom, so the
// island is a window of its own, drawn with Direct2D. Clicking the clock opens
// a calendar under it; clicking the network, volume or battery opens a
// control center, like on macOS.

HANDLE g_islandThread;
DWORD g_islandThreadId;

constexpr UINT_PTR kIslandTimerId = 1;
// The clock is checked this often; the rest every few ticks.
// The island wakes up on each new second, for the clock; the network,
// volume and "Do not disturb" are checked every few seconds (and right away
// when changed from the island).
// Network, volume, battery and "Do not disturb" come as events (see
// Island::StartStatusWatch); in case one is missed, they're read again this
// often (in clock ticks, one a second).
constexpr int kIslandStatusTicks = 60;
// Every couple of seconds, the island and its open panel are put back on top
// (other always-on-top windows can cover them), and an open panel's contents
// refreshed.
constexpr int kIslandRaiseTicks = 2;
// What changed, posted to the island (wParam).
constexpr UINT WM_APP_STATUS = WM_APP + 55;
enum StatusChange : WPARAM {
    kStatusVolume = 1,
    kStatusAudioDevice,
    kStatusNetwork,
    kStatusBattery,
    kStatusDoNotDisturb,
};

// The bar's messages from the shell, as a registered app bar.
constexpr UINT WM_APP_APPBAR = WM_APP + 50;

// Sizes in pixels at 100% scaling.
constexpr float kPillHeight = 30;
// Upright on a side, the pill is this wide.
constexpr float kVerticalPillWidth = 46;
constexpr float kPillTopGap = 6;
constexpr float kBarHeight = 28;
constexpr float kIslandPadding = 12;
constexpr float kIslandSpacing = 6;
constexpr float kIslandIconSlot = 26;
constexpr float kIslandSmallSlot = 20;
constexpr float kIslandIconSize = 15;
constexpr float kIslandSmallIconSize = 11;
constexpr float kIslandTextSize = 13;
// The minimized pill: a thin line at the top, a bit bigger under the mouse.
constexpr float kLineWidth = 72;
constexpr float kLineHoverWidth = 96;
constexpr float kLineHeight = 5;
constexpr float kLineTopGap = 0;
// Under the mouse, the line grows this much taller.
constexpr float kLineHoverGrowth = 3;
// The panels under the island.
constexpr float kPanelGap = 8;
constexpr float kNotificationsWidth = 360;
constexpr float kNotificationHeight = 74;
constexpr int kMaxNotificationsShown = 5;
constexpr int kMaxNotifications = 50;
constexpr float kSwitchRowHeight = 50;
constexpr float kPanelPadding = 16;
constexpr float kPanelRadius = 16;
constexpr float kCalendarCell = 36;
constexpr float kControlWidth = 340;
constexpr float kControlPadding = 12;
constexpr float kControlGap = 10;
constexpr float kTileHeight = 62;
constexpr float kSoundHeight = 92;
constexpr float kDisplayHeight = 72;
constexpr float kShortcutHeight = 64;
constexpr float kBatteryHeight = 50;
constexpr float kFooterHeight = 30;

// Icons from the Segoe Fluent Icons font.
constexpr WCHAR kGlyphWifi[] = {0xE872, 0xE873, 0xE874, 0xE701};
constexpr WCHAR kGlyphEthernet = 0xE839;
constexpr WCHAR kGlyphNoInternet = 0xF384;
constexpr WCHAR kGlyphMute = 0xE74F;
constexpr WCHAR kGlyphVolume[] = {0xE992, 0xE993, 0xE994, 0xE995};
constexpr WCHAR kGlyphBell = 0xEA8F;
constexpr WCHAR kGlyphMoon = 0xE708;
constexpr WCHAR kGlyphCollapse = 0xE70E;
constexpr WCHAR kGlyphPrevious = 0xE76B;
constexpr WCHAR kGlyphNext = 0xE76C;
constexpr WCHAR kGlyphBluetooth = 0xE702;
constexpr WCHAR kGlyphSettings = 0xE713;
constexpr WCHAR kGlyphPin = 0xE718;
constexpr WCHAR kGlyphMusic = 0xE8D6;
constexpr WCHAR kGlyphCalendar = 0xE787;
constexpr WCHAR kGlyphPlay = 0xE768;
constexpr WCHAR kGlyphPause = 0xE769;
constexpr WCHAR kGlyphPreviousTrack = 0xE892;
constexpr WCHAR kGlyphNextTrack = 0xE893;
constexpr WCHAR kGlyphSpeaker = 0xE767;
constexpr WCHAR kGlyphSend = 0xE724;

// What's at each spot of the island.
enum class IslandItem {
    None,
    Overflow,
    Network,
    Volume,
    Battery,
    Clock,
    Bell,
    Minimize,
    Line,
    // An app with a notification badge (the slot's text is its ID).
    App,
    // What's playing, and its buttons on the pill.
    Media,
    MediaPrevious,
    MediaPlayPause,
    MediaNext,
    // A new notification shown in the pill, its reply box and "Open".
    Banner,
    BannerReply,
    BannerOpen,
    BannerClose,
    // Another app's tray icon (the slot's text is its key), and its pin.
    TrayIcon,
    TrayPin,
    // A faint line between the open list and the pinned icons.
    TraySeparator,
};

struct IslandSlot {
    IslandItem item;
    D2D1_RECT_F rect;
    std::wstring text;
    // 0: text, 1: icon, 2: small icon, 3: the app's icon (see App), 4: a
    // tray icon (see TrayIcon).
    int font;
    // How far an app's icon has grown in (see Island::AppEntry).
    float scale = 1;
};

constexpr GUID kCLSID_MMDeviceEnumerator = {
    0xbcde0395,
    0xe52f,
    0x467c,
    {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e}};
constexpr GUID kIID_IMMDeviceEnumerator = {
    0xa95664d2,
    0x9614,
    0x4f35,
    {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6}};
constexpr GUID kIID_IAudioEndpointVolume = {
    0x5cdf2c82,
    0x841e,
    0x4546,
    {0x97, 0x22, 0x0c, 0xf7, 0x40, 0x78, 0x22, 0x9a}};
constexpr GUID kCLSID_NetworkListManager = {
    0xdcb00c01,
    0x570f,
    0x4a9b,
    {0x8d, 0x69, 0x19, 0x9f, 0xdb, 0xa5, 0x72, 0x3b}};
constexpr GUID kIID_INetworkListManager = {
    0xdcb00000,
    0x570f,
    0x4a9b,
    {0x8d, 0x69, 0x19, 0x9f, 0xdb, 0xa5, 0x72, 0x3b}};
constexpr PROPERTYKEY kPKEY_Device_FriendlyName = {
    {0xa45c254e,
     0xdf1c,
     0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};

// The island's own texts, in English or Portuguese like the Windows display
// language.
PCWSTR Tr(PCWSTR english, PCWSTR portuguese) {
    static const bool kPortuguese =
        PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE;
    return kPortuguese ? portuguese : english;
}

// Presses Windows+key, to open a Windows panel: A for the quick settings, N
// for the notifications.
void PressWindowsKey(WORD key) {
    INPUT inputs[4] = {};
    for (INPUT& input : inputs) {
        input.type = INPUT_KEYBOARD;
    }
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].ki.wVk = key;
    inputs[2].ki.wVk = key;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].ki.wVk = VK_LWIN;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

// Presses keys together (a shortcut), then lets them go in reverse order.
void PressKeys(std::initializer_list<WORD> keys) {
    std::vector<INPUT> inputs;
    for (WORD key : keys) {
        INPUT input{INPUT_KEYBOARD};
        input.ki.wVk = key;
        inputs.push_back(input);
    }
    for (auto it = std::rbegin(keys); it != std::rend(keys); ++it) {
        INPUT input{INPUT_KEYBOARD};
        input.ki.wVk = *it;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        inputs.push_back(input);
    }
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

////////////////////////////////////////////////////////////////////////////////
// Night light. Windows has no API for it: it keeps its state in the registry
// (CloudStore) as a small binary record, and applies a new record written there,
// which is how scripts and tools switch it. The record is checked strictly
// before changing anything; if it doesn't look as expected, the mod leaves it
// alone and the control center shows the tile as unavailable. There's a record
// for the whole account and, on newer builds, one per device; both are kept the
// same.
//
// The record: 43 42 01 00 0A 02 01 00 2A 06 <time, varint> 2A 2B 0E <length>
// then the state, 43 42 01 00 [10 00 when on] ..., then 00 00 00 (00).

constexpr WCHAR kCloudStoreKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\CloudStore\\Store\\"
    L"DefaultAccount\\Current";
constexpr WCHAR kNightLightState[] =
    L"windows.data.bluelightreduction.bluelightreductionstate";

struct NightLightRecord {
    // The value's key, under kCloudStoreKey.
    std::wstring key;
    std::vector<BYTE> data;
    size_t stampAt = 0;
    size_t stampLength = 0;
    size_t lengthAt = 0;
    bool on = false;
};

bool ParseNightLight(NightLightRecord& record) {
    static const BYTE kHeader[] = {0x43, 0x42, 0x01, 0x00, 0x0A,
                                   0x02, 0x01, 0x00, 0x2A, 0x06};
    static const BYTE kState[] = {0x43, 0x42, 0x01, 0x00};
    const auto& d = record.data;
    if (d.size() < 30 || memcmp(d.data(), kHeader, sizeof(kHeader)) != 0) {
        return false;
    }
    size_t i = sizeof(kHeader);
    record.stampAt = i;
    while (i < d.size() && (d[i] & 0x80) && i - record.stampAt < 9) {
        i++;
    }
    if (i >= d.size() || (d[i] & 0x80)) {
        return false;
    }
    i++;
    record.stampLength = i - record.stampAt;
    if (i + 4 > d.size() || d[i] != 0x2A || d[i + 1] != 0x2B ||
        d[i + 2] != 0x0E) {
        return false;
    }
    record.lengthAt = i + 3;
    const size_t state = record.lengthAt + 1;
    if (state + d[record.lengthAt] > d.size() || d[record.lengthAt] < 6 ||
        memcmp(&d[state], kState, sizeof(kState)) != 0) {
        return false;
    }
    record.on = d[state + 4] == 0x10 && d[state + 5] == 0x00;
    return true;
}

// The records there are, read and checked.
std::vector<NightLightRecord> ReadNightLightRecords() {
    std::vector<NightLightRecord> records;
    HKEY store;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, kCloudStoreKey, 0, KEY_READ,
                     &store) != ERROR_SUCCESS) {
        return records;
    }
    const std::wstring_view state{kNightLightState};
    WCHAR name[256];
    for (DWORD index = 0;; index++) {
        DWORD length = ARRAYSIZE(name);
        if (RegEnumKeyEx(store, index, name, &length, nullptr, nullptr,
                         nullptr, nullptr) != ERROR_SUCCESS) {
            break;
        }
        // "<scope>$windows...state" or "<device>$windows...stateperdevice".
        const std::wstring_view key{name, length};
        const size_t dollar = key.find(L'$');
        if (dollar == std::wstring_view::npos) {
            continue;
        }
        const std::wstring_view value = key.substr(dollar + 1);
        if (value != state && value != std::wstring(state) + L"perdevice") {
            continue;
        }
        NightLightRecord record;
        record.key = std::wstring(key) + L"\\" + std::wstring(value);
        DWORD size = 0;
        if (RegGetValue(store, record.key.c_str(), L"Data", RRF_RT_REG_BINARY,
                        nullptr, nullptr, &size) != ERROR_SUCCESS ||
            size == 0 || size > 4096) {
            continue;
        }
        record.data.resize(size);
        if (RegGetValue(store, record.key.c_str(), L"Data", RRF_RT_REG_BINARY,
                        nullptr, record.data.data(),
                        &size) == ERROR_SUCCESS &&
            ParseNightLight(record)) {
            record.data.resize(size);
            records.push_back(std::move(record));
        }
    }
    RegCloseKey(store);
    return records;
}

// 1 on, 0 off, -1 unknown (no record, or not as expected).
int GetNightLight() {
    const auto records = ReadNightLightRecords();
    if (records.empty()) {
        return -1;
    }
    // The device's own record wins.
    for (const auto& record : records) {
        if (record.key.find(L"perdevice") != std::wstring::npos) {
            return record.on;
        }
    }
    return records[0].on;
}

bool SetNightLight(bool on) {
    auto records = ReadNightLightRecords();
    bool changed = false;
    // Now, in seconds since 1970, as a varint.
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    ULONGLONG now = ((((ULONGLONG)fileTime.dwHighDateTime << 32) |
                      fileTime.dwLowDateTime) -
                     116444736000000000ULL) /
                    10000000;
    std::vector<BYTE> stamp;
    do {
        BYTE byte = now & 0x7F;
        now >>= 7;
        stamp.push_back(now ? byte | 0x80 : byte);
    } while (now);

    for (auto& record : records) {
        auto& d = record.data;
        const size_t state = record.lengthAt + 1;
        if (on && !record.on) {
            d.insert(d.begin() + state + 4, {0x10, 0x00});
            d[record.lengthAt] += 2;
        } else if (!on && record.on) {
            d.erase(d.begin() + state + 4, d.begin() + state + 6);
            d[record.lengthAt] -= 2;
        }
        d.erase(d.begin() + record.stampAt,
                d.begin() + record.stampAt + record.stampLength);
        d.insert(d.begin() + record.stampAt, stamp.begin(), stamp.end());
        // Checked again before it's written.
        NightLightRecord check{record.key, d};
        if (!ParseNightLight(check) || check.on != on) {
            continue;
        }
        const std::wstring path =
            std::wstring(kCloudStoreKey) + L"\\" + record.key;
        if (RegSetKeyValue(HKEY_CURRENT_USER, path.c_str(), L"Data",
                           REG_BINARY, d.data(),
                           (DWORD)d.size()) == ERROR_SUCCESS) {
            changed = true;
        }
    }
    return changed;
}

////////////////////////////////////////////////////////////////////////////////
// The monitors' brightness, over DDC/CI (documented, dxva2). Each call talks
// to the monitor and takes tens of milliseconds, so they run on threads of
// their own (see ControlPanel).

// Collects the monitors (see ForEachPhysicalMonitor).
BOOL CALLBACK CollectMonitorProc(HMONITOR monitor,
                                 HDC,
                                 LPRECT,
                                 LPARAM parameter) {
    ((std::vector<HMONITOR>*)parameter)->push_back(monitor);
    return TRUE;
}

template <typename F>
void ForEachPhysicalMonitor(F&& f) {
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        CollectMonitorProc,
        (LPARAM)&monitors);
    for (HMONITOR monitor : monitors) {
        DWORD count = 0;
        if (!GetNumberOfPhysicalMonitorsFromHMONITOR(monitor, &count) ||
            !count) {
            continue;
        }
        std::vector<PHYSICAL_MONITOR> physical(count);
        if (!GetPhysicalMonitorsFromHMONITOR(monitor, count,
                                             physical.data())) {
            continue;
        }
        for (const auto& one : physical) {
            f(one.hPhysicalMonitor);
        }
        DestroyPhysicalMonitors(count, physical.data());
    }
}

// From 0 to 100, -1 if no monitor answers.
int ReadBrightness() {
    int level = -1;
    ForEachPhysicalMonitor([&](HANDLE monitor) {
        DWORD minimum, current, maximum;
        if (level < 0 &&
            GetMonitorBrightness(monitor, &minimum, &current, &maximum) &&
            maximum > minimum) {
            level = (int)std::lround(100.0 * (current - minimum) /
                                     (maximum - minimum));
        }
    });
    return level;
}

void WriteBrightness(int level) {
    ForEachPhysicalMonitor([&](HANDLE monitor) {
        DWORD minimum, current, maximum;
        if (GetMonitorBrightness(monitor, &minimum, &current, &maximum) &&
            maximum > minimum) {
            SetMonitorBrightness(
                monitor, minimum + (DWORD)std::lround((maximum - minimum) *
                                                      level / 100.0));
        }
    });
}

void OpenUri(PCWSTR uri) {
    ShellExecute(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
}

// Whether "Do not disturb" is on. Windows publishes it as a notification
// state (WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED): 0 when off, otherwise
// the active profile (priority only, alarms only).
bool IsDoNotDisturbOn() {
    using NtQueryWnfStateData_t = LONG(NTAPI*)(
        const ULONGLONG* stateName, const void* typeId,
        const void* explicitScope, ULONG* changeStamp, void* buffer,
        ULONG* bufferSize);
    static const auto query = (NtQueryWnfStateData_t)GetProcAddress(
        GetModuleHandle(L"ntdll.dll"), "NtQueryWnfStateData");
    if (!query) {
        return false;
    }
    constexpr ULONGLONG kQuietHoursState = 0x0D83063EA3BF1C75;
    DWORD profile = 0;
    ULONG size = sizeof(profile);
    ULONG changeStamp;
    return query(&kQuietHoursState, nullptr, nullptr, &changeStamp, &profile,
                 &size) >= 0 &&
           size == sizeof(profile) && profile != 0;
}

// Turns "Do not disturb" on or off, with the interface the Windows
// notification center uses (the quiet hours settings). It isn't documented;
// "Do not disturb" on is the "priority only" profile. Returns whether it
// worked.
struct IQuietHoursSettings : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE get_UserSelectedProfile(LPWSTR* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UserSelectedProfile(LPCWSTR id) = 0;
};

constexpr GUID kCLSID_QuietHoursSettings = {
    0xf53321fa,
    0x34f8,
    0x4b7f,
    {0xb9, 0xa3, 0x36, 0x18, 0x77, 0xcb, 0x94, 0xcf}};
constexpr GUID kIID_IQuietHoursSettings = {
    0x6bff4732,
    0x81ec,
    0x4ffb,
    {0xae, 0x67, 0xb6, 0xc1, 0xbc, 0x29, 0x63, 0x1f}};

bool SetDoNotDisturb(bool on) {
    winrt::com_ptr<IQuietHoursSettings> settings;
    if (FAILED(CoCreateInstance(kCLSID_QuietHoursSettings, nullptr,
                                CLSCTX_LOCAL_SERVER, kIID_IQuietHoursSettings,
                                settings.put_void()))) {
        Wh_Log(L"The quiet hours settings aren't available");
        return false;
    }
    const HRESULT result = settings->put_UserSelectedProfile(
        on ? L"Microsoft.QuietHoursProfile.PriorityOnly"
           : L"Microsoft.QuietHoursProfile.Unrestricted");
    if (FAILED(result)) {
        Wh_Log(L"Switching \"Do not disturb\" failed: %08X", (unsigned)result);
    }
    return SUCCEEDED(result);
}

// Moves `value` towards `target` like a spring, over about `seconds`.
// `damping` 1 settles without overshooting; lower values bounce a little, like
// the iPhone's Dynamic Island. Returns true while it's still moving.
bool SpringTowards(double& value, double& velocity, double target,
                   double seconds, double dt, double damping = 1.0) {
    const double omega = 8.0 / seconds;
    constexpr double kStep = 1.0 / 480;
    for (double remaining = dt; remaining > 0; remaining -= kStep) {
        const double h = std::min(remaining, kStep);
        const double acceleration = omega * omega * (target - value) -
                                    2 * damping * omega * velocity;
        velocity += acceleration * h;
        value += velocity * h;
    }
    if (std::fabs(target - value) < 0.001 && std::fabs(velocity) < 0.01) {
        value = target;
        velocity = 0;
        return false;
    }
    return true;
}

// A button pressed: it goes down quickly (staying down while held), and once
// all the way down, springs back with a little overshoot. Timed rather than a
// spring, so even a quick click shows the whole press.
constexpr double kPressDown = 0.09;
constexpr double kPressBack = 0.42;

struct PressAnimation {
    // What's pressed (an item or a part, as a number), or 0.
    int target = 0;
    bool held = false;
    double start = 0;
    double releaseAt = 0;

    void Press(int what) {
        target = what;
        held = true;
        start = NowSeconds();
    }
    void Release() {
        if (!held) {
            return;
        }
        held = false;
        releaseAt = std::max(NowSeconds(), start + kPressDown);
    }
    // From 0 (up) to 1 (down); a bit under 0 while springing back.
    double Amount(double now) const {
        if (!target) {
            return 0;
        }
        const double down = std::min((now - start) / kPressDown, 1.0);
        const double eased = 1 - (1 - down) * (1 - down) * (1 - down);
        if (held || now < releaseAt) {
            return eased;
        }
        const double back = (now - releaseAt) / kPressBack;
        return back >= 1 ? 0 : 1 - SpringOut(back);
    }
    // Returns true while it moves (and forgets it when done).
    bool Animate(double now) {
        if (!target) {
            return false;
        }
        if (!held && now >= releaseAt + kPressBack) {
            target = 0;
            return true;
        }
        return true;
    }
};

// Opening things bounces a little; closing doesn't.
constexpr double kBounceDamping = 0.72;
constexpr double kOpenSeconds = 0.42;
constexpr double kCloseSeconds = 0.26;
// A panel opening out of the pill: sideways quick and a bit bouncy, then
// downwards a moment later, a bit softer. Closing goes back without bouncing.
constexpr double kPanelWidthSeconds = 0.55;
constexpr double kPanelWidthDamping = 0.62;
constexpr double kPanelHeightSeconds = 0.65;
constexpr double kPanelHeightDamping = 0.70;
constexpr double kPanelHeightDelay = 0.05;
constexpr double kPanelCloseSeconds = 0.42;
// The island opening out of its dot, and closing back into it.
constexpr double kAppearSeconds = 1.25;
constexpr double kDisappearSeconds = 0.95;
// The pill's corners, opened into a panel.
constexpr float kIslandPanelRadius = 26;
// A new notification in the pill: its size, and how long it stays.
constexpr float kBannerWidth = 380;
constexpr float kBannerHeight = 70;
// The row with the reply box and "Open", shown under the mouse.
constexpr float kBannerReplyHeight = 46;
constexpr UINT kBannerShownMs = 5000;
constexpr UINT_PTR kBannerTimerId = 3;
constexpr UINT_PTR kTrayCloseTimerId = 4;
constexpr UINT_PTR kTipTimerId = 5;
constexpr UINT_PTR kTrayAppTimerId = 6;
constexpr UINT_PTR kMediaTimerId = 7;
// A tray icon's pin shows after the mouse stays on it this long, so it isn't
// clicked by accident; it then grows in.
constexpr UINT_PTR kPinTimerId = 8;
// A Windows banner hidden right away is put back if the pill hasn't shown the
// notification by then (see OnWindowsBanner).
constexpr UINT_PTR kBannerRestoreTimerId = 9;
constexpr UINT kBannerRestoreMs = 1500;
constexpr UINT kPinDelayMs = 2000;
constexpr double kPinGrowSeconds = 0.22;
// What's playing stays on the pill at least this long, and this long after it
// stops (seconds).
constexpr double kMediaMinShown = 2.0;
// The title on the pill: a short one gets its artist next to it (so the
// island doesn't shrink to a sliver), and a long one is cut (with "...").
constexpr float kMediaTitleShortWidth = 90;
constexpr float kMediaTitleMinWidth = 60;
constexpr float kMediaTitleMaxWidth = 150;
constexpr double kMediaLinger = 1.5;
// The uncloaking event (not in older headers).
constexpr DWORD kEventObjectUncloaked = 0x8018;

bool PointInRect(POINT pt, const D2D1_RECT_F& rect) {
    return pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top &&
           pt.y < rect.bottom;
}

// A layered window drawn with Direct2D into a 32-bit bitmap.
class LayeredCanvas {
   public:
    bool Create(ID2D1Factory* factory) {
        const D2D1_RENDER_TARGET_PROPERTIES properties =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96);
        if (FAILED(factory->CreateDCRenderTarget(&properties, target.put()))) {
            return false;
        }
        // ClearType needs an opaque background.
        target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        m_dc = CreateCompatibleDC(nullptr);
        return m_dc != nullptr;
    }

    void Destroy() {
        if (m_dc) {
            if (m_oldBitmap) {
                SelectObject(m_dc, m_oldBitmap);
            }
            DeleteDC(m_dc);
            m_dc = nullptr;
        }
        if (m_bitmap) {
            DeleteObject(m_bitmap);
            m_bitmap = nullptr;
            m_bits = nullptr;
        }
    }

    // What was last drawn: premultiplied BGRA rows, top first.
    const BYTE* Bits() const { return (const BYTE*)m_bits; }
    SIZE Size() const { return m_size; }

    // Starts drawing on a canvas of `size`, cleared.
    bool Begin(SIZE size) {
        if (!m_bitmap || m_size.cx != size.cx || m_size.cy != size.cy) {
            if (m_bitmap) {
                SelectObject(m_dc, m_oldBitmap);
                DeleteObject(m_bitmap);
                m_bitmap = nullptr;
            }
            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(info.bmiHeader);
            info.bmiHeader.biWidth = size.cx;
            info.bmiHeader.biHeight = -size.cy;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;
            m_bitmap = CreateDIBSection(m_dc, &info, DIB_RGB_COLORS, &m_bits,
                                        nullptr, 0);
            if (!m_bitmap) {
                m_bits = nullptr;
                return false;
            }
            m_oldBitmap = SelectObject(m_dc, m_bitmap);
            m_size = size;
        }
        const RECT bounds{0, 0, size.cx, size.cy};
        if (FAILED(target->BindDC(m_dc, &bounds))) {
            return false;
        }
        target->BeginDraw();
        target->SetTransform(D2D1::Matrix3x2F::Identity());
        target->Clear(D2D1::ColorF(0, 0, 0, 0));
        return true;
    }

    // Finishes drawing and puts it on the window, with an overall opacity.
    void End(HWND hWnd, POINT position, float opacity) {
        if (FAILED(target->EndDraw())) {
            return;
        }
        POINT source{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0,
                            (BYTE)std::clamp((int)(opacity * 255), 0, 255),
                            AC_SRC_ALPHA};
        UpdateLayeredWindow(hWnd, nullptr, &position, &m_size, m_dc, &source, 0,
                            &blend, ULW_ALPHA);
    }

    winrt::com_ptr<ID2D1DCRenderTarget> target;

   private:
    HDC m_dc = nullptr;
    HBITMAP m_bitmap = nullptr;
    void* m_bits = nullptr;
    HGDIOBJ m_oldBitmap = nullptr;
    SIZE m_size{};
};

// What the control center and the island show about the network.
struct NetworkInfo {
    bool connected = false;
    bool internet = false;
    // -1 when not on Wi-Fi.
    int signal = -1;
    std::wstring wifiName;
};

class Island;
Island* g_island;

// A panel's timer for the screenshot (see ControlPanel).
constexpr UINT_PTR kPanelScreenshotTimerId = 7;

// Runs the island's animation frames (see IslandThreadProc).
void StartIslandAnimation();

class Panel;
// Gives a panel a new snapshot of the pill (see Island::CapturePill).
void CapturePillFor(Panel& panel);

////////////////////////////////////////////////////////////////////////////////
// Panels: windows that open out of the island and close when clicking anywhere
// else. Out of the pill, the pill itself stretches into the panel, like the
// iPhone's Dynamic Island: sideways first, then down, its contents shrinking
// away as the panel's fade in. On the bar, the panel drops out of the clicked
// item.

class Panel {
   public:
    virtual ~Panel() = default;
    bool Create(ID2D1Factory* factory, IDWriteFactory* dwrite);
    void Destroy();
    // `source` is the part of the island it grows out of, on the screen, and
    // `anchor` the middle of the panel's top. `fromPill`: the pill becomes the
    // panel (otherwise it drops out of an item of the bar).
    void Open(POINT anchor, float scale, D2D1_RECT_F source, bool fromPill);
    // How the pill looks, drawn shrinking away as it stretches.
    void SetPillSnapshot(const BYTE* bits, int stride, RECT rect);
    void Close();
    bool IsOpen() const { return m_target > 0; }
    // Which way it opens out of the island: down, up (on the lower half of
    // the screen), right or left (upright on the left or right edge).
    enum class Direction { Down, Up, Right, Left };
    void SetDirection(Direction direction) { m_direction = direction; }
    // Keeps it above the island, which is put back on top now and then.
    void Raise() {
        if (m_hwnd && (m_open > 0 || m_openX > 0)) {
            SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
    bool Animate(double dt);
    void Render();
    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

   protected:
    // Sets up the fonts and the size for this scale.
    virtual void Prepare(float scale) = 0;
    // Called when opening, before the first frame.
    virtual void OnOpen() {}
    virtual void Draw(ID2D1RenderTarget* target,
                      ID2D1SolidColorBrush* brush) = 0;
    // These return true to draw a new frame.
    virtual bool OnMouseMove(POINT) { return false; }
    virtual bool OnMouseLeave() { return false; }
    virtual bool OnMouseDown(POINT) { return false; }
    virtual bool OnMouseUp(POINT) { return false; }
    virtual bool OnWheel(int) { return false; }
    // Escape closes the panel unless this returns true.
    virtual bool OnKey(WPARAM) { return false; }
    virtual bool OnChar(WCHAR) { return false; }
    // Messages from WM_APP up, posted to the panel's window.
    virtual bool OnAppMessage(UINT, WPARAM, LPARAM) { return false; }
    // Before the window goes away.
    virtual void OnDestroy() {}
    // Animates the contents while open; returns true while it moves.
    virtual bool AnimateContents(double) { return false; }
    // Set by AnimateContents when only something that never stops moves
    // (scrolling text): drawn at a lower frame rate then, which
    // looks the same and costs much less.
    bool m_idleMotion = false;
    double m_lastIdleFrame = 0;

    winrt::com_ptr<IDWriteTextFormat> MakeFormat(
        PCWSTR family,
        DWRITE_FONT_WEIGHT weight,
        float size,
        DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_CENTER);

    HWND m_hwnd = nullptr;
    IDWriteFactory* m_dwrite = nullptr;
    LayeredCanvas m_canvas;
    float m_scale = 0;
    // The panel's place on the screen when open, and its size.
    POINT m_position{};
    SIZE m_size{};
    bool m_tracking = false;
    // The island part it grows out of. The window covers both, and the panel
    // is drawn at `m_contentOffset` in it.
    D2D1_RECT_F m_source{};
    POINT m_contentOffset{};

    // Opening: 0 (closed) to 1 (open), animated downwards (m_open) and
    // sideways (m_openX); downwards starts a moment later.
    double m_open = 0;
    double m_velocity = 0;
    double m_openX = 0;
    double m_velocityX = 0;
    double m_heightDelay = 0;
    double m_target = 0;
    bool m_fromPill = false;
    Direction m_direction = Direction::Down;
    // Out of the pill, at least as wide as it.
    float m_minWidth = 0;
    POINT m_anchor{};
    RECT m_monitorRect{};
    // The size shown, following m_size when it changes (another view), so the
    // panel stretches to it.
    double m_shownWidth = 0;
    double m_shownHeight = 0;
    double m_widthVelocity = 0;
    double m_heightVelocity = 0;
    // The pill's snapshot: its pixels, and the bitmap made from them.
    std::vector<BYTE> m_snapshotPixels;
    SIZE m_snapshotSize{};
    bool m_snapshotChanged = false;
    winrt::com_ptr<ID2D1Bitmap> m_snapshot;
};

LRESULT CALLBACK PanelWndProc(HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam) {
    if (auto* panel = (Panel*)GetWindowLongPtr(hWnd, GWLP_USERDATA)) {
        return panel->HandleMessage(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Panel::Create(ID2D1Factory* factory, IDWriteFactory* dwrite) {
    m_dwrite = dwrite;
    // Takes the focus when open, so clicking anywhere else closes it.
    m_hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                            kPanelClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
                            nullptr, g_module, nullptr);
    if (!m_hwnd) {
        return false;
    }
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
    return m_canvas.Create(factory);
}

void Panel::Destroy() {
    OnDestroy();
    m_snapshot = nullptr;
    if (m_hwnd) {
        SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    m_canvas.Destroy();
}

winrt::com_ptr<IDWriteTextFormat> Panel::MakeFormat(
    PCWSTR family,
    DWRITE_FONT_WEIGHT weight,
    float size,
    DWRITE_TEXT_ALIGNMENT alignment) {
    winrt::com_ptr<IDWriteTextFormat> format;
    if (SUCCEEDED(m_dwrite->CreateTextFormat(
            family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, size, L"", format.put()))) {
        format->SetTextAlignment(alignment);
        format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        // Too long texts end with "...".
        const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER,
                                       0, 0};
        winrt::com_ptr<IDWriteInlineObject> ellipsis;
        m_dwrite->CreateEllipsisTrimmingSign(format.get(), ellipsis.put());
        format->SetTrimming(&trimming, ellipsis.get());
    }
    return format;
}

void Panel::Open(POINT anchor,
                 float scale,
                 D2D1_RECT_F source,
                 bool fromPill) {
    if (!m_hwnd) {
        return;
    }
    m_source = source;
    m_fromPill = fromPill;
    m_minWidth = fromPill ? source.right - source.left : 0;
    m_scale = scale;
    Prepare(scale);
    OnOpen();

    m_anchor = anchor;
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfo(MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST),
                   &monitor);
    m_monitorRect = monitor.rcMonitor;

    // Closed (not still closing): starts from the island part, at its size.
    if (m_open <= 0.01 && m_openX <= 0.01) {
        m_open = m_openX = 0;
        m_velocity = m_velocityX = 0;
        m_shownWidth = std::max((float)m_size.cx, m_minWidth);
        m_shownHeight = m_size.cy;
        m_widthVelocity = m_heightVelocity = 0;
        m_heightDelay = fromPill ? kPanelHeightDelay : 0;
    }

    m_target = 1;
    Render();
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(m_hwnd);
    StartIslandAnimation();
}

void Panel::SetPillSnapshot(const BYTE* bits, int stride, RECT rect) {
    m_snapshotPixels.clear();
    m_snapshotSize = {};
    m_snapshotChanged = true;
    if (!bits || rect.right <= rect.left || rect.bottom <= rect.top) {
        return;
    }
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    m_snapshotPixels.resize((size_t)width * height * 4);
    for (int y = 0; y < height; y++) {
        memcpy(&m_snapshotPixels[(size_t)y * width * 4],
               bits + (size_t)(rect.top + y) * stride + rect.left * 4,
               (size_t)width * 4);
    }
    m_snapshotSize = {width, height};
}

void Panel::Close() {
    if (m_target > 0) {
        m_target = 0;
        // The pill may have changed (the clock) since it opened.
        if (m_fromPill) {
            CapturePillFor(*this);
        }
        StartIslandAnimation();
    }
    if (GetCapture() == m_hwnd) {
        ReleaseCapture();
    }
}

// Returns true while it's still moving.
// Frames of motion that never stops (see m_idleMotion): 30 a second.
constexpr double kIdleFrameSeconds = 1.0 / 30;

bool Panel::Animate(double dt) {
    if (!m_hwnd) {
        return false;
    }
    if (m_open <= 0 && m_openX <= 0 && m_target <= 0) {
        return false;
    }
    bool moving;
    if (m_target > 0) {
        const bool sideways =
            SpringTowards(m_openX, m_velocityX, 1, kPanelWidthSeconds, dt,
                          kPanelWidthDamping);
        bool downwards = true;
        if (m_heightDelay > 0) {
            m_heightDelay -= dt;
        } else {
            downwards = SpringTowards(m_open, m_velocity, 1,
                                      kPanelHeightSeconds, dt,
                                      kPanelHeightDamping);
        }
        const bool contents = AnimateContents(dt);
        // Another view, another size.
        const bool width = SpringTowards(
            m_shownWidth, m_widthVelocity,
            std::max((float)m_size.cx, m_minWidth), kOpenSeconds, dt,
            kBounceDamping);
        const bool height =
            SpringTowards(m_shownHeight, m_heightVelocity, m_size.cy,
                          kOpenSeconds, dt, kBounceDamping);
        moving = sideways || downwards || width || height || contents;
        if (m_idleMotion && !sideways && !downwards && !width && !height) {
            const double now = NowSeconds();
            if (now - m_lastIdleFrame < kIdleFrameSeconds) {
                return true;
            }
            m_lastIdleFrame = now;
        }
    } else {
        m_idleMotion = false;
        m_heightDelay = 0;
        const bool sideways = SpringTowards(m_openX, m_velocityX, 0,
                                            kPanelCloseSeconds, dt);
        const bool downwards = SpringTowards(m_open, m_velocity, 0,
                                             kPanelCloseSeconds, dt);
        moving = sideways || downwards;
    }
    if (!moving && m_open <= 0 && m_openX <= 0) {
        ShowWindow(m_hwnd, SW_HIDE);
        return false;
    }
    Render();
    return moving;
}

// The island part stretches into the panel: a black shape going from the
// island part's rect to the panel's, sideways and downwards on their own
// springs. Out of the pill, the pill's contents shrink and fade away first,
// then the panel's contents fade in, sliding down a little. Closing goes back
// the same way.
void Panel::Render() {
    if (!m_hwnd) {
        return;
    }
    // Another view, another size: it stretches to it.
    if (m_target > 0 &&
        (std::fabs(m_shownWidth - std::max((float)m_size.cx, m_minWidth)) >
             0.5 ||
         std::fabs(m_shownHeight - m_size.cy) > 0.5)) {
        StartIslandAnimation();
    }
    const float width = (float)std::max(m_shownWidth, 1.0);
    const float height = (float)std::max(m_shownHeight, 1.0);
    // Centered under (or over) the anchor, or beside it and centered on it,
    // inside the monitor.
    const bool sideways =
        m_direction == Direction::Right || m_direction == Direction::Left;
    const float left =
        sideways ? (m_direction == Direction::Right ? (float)m_anchor.x
                                                    : (float)m_anchor.x - width)
                 : std::clamp((float)m_anchor.x - width / 2,
                              (float)m_monitorRect.left + 8,
                              std::max((float)m_monitorRect.left + 8,
                                       (float)m_monitorRect.right - width - 8));
    const float panelTop =
        sideways ? std::clamp((float)m_anchor.y - height / 2,
                              (float)m_monitorRect.top + 8,
                              std::max((float)m_monitorRect.top + 8,
                                       (float)m_monitorRect.bottom - height - 8))
        : m_direction == Direction::Up ? (float)m_anchor.y - height
                                       : (float)m_anchor.y;
    const D2D1_RECT_F panel{std::round(left), panelTop,
                            std::round(left) + width, panelTop + height};
    m_position = {(LONG)panel.left, (LONG)panel.top};

    // The window covers the island part and the panel, and the bounce.
    const float margin = std::round(12 * m_scale);
    const float sideMargin =
        margin + 0.08f * std::max(std::fabs(panel.left - m_source.left),
                                  std::fabs(panel.right - m_source.right));
    const float bottomMargin =
        margin + 0.08f * std::fabs(panel.bottom - m_source.bottom);
    const float topMargin =
        m_direction != Direction::Down
            ? margin + 0.08f * std::fabs(panel.top - m_source.top)
            : 2;
    const POINT windowPosition{
        (LONG)std::floor(std::min(panel.left, m_source.left) - sideMargin),
        (LONG)std::floor(std::min(panel.top, m_source.top) - topMargin)};
    const SIZE windowSize{
        (LONG)std::ceil(std::max(panel.right, m_source.right) + sideMargin -
                        windowPosition.x),
        (LONG)std::ceil(std::max(panel.bottom, m_source.bottom) +
                        bottomMargin - windowPosition.y)};
    if (!m_canvas.Begin(windowSize)) {
        return;
    }
    auto* target = m_canvas.target.get();

    if (m_snapshotChanged) {
        m_snapshotChanged = false;
        m_snapshot = nullptr;
        if (!m_snapshotPixels.empty()) {
            target->CreateBitmap(
                D2D1::SizeU(m_snapshotSize.cx, m_snapshotSize.cy),
                m_snapshotPixels.data(), m_snapshotSize.cx * 4,
                D2D1::BitmapProperties(
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                      D2D1_ALPHA_MODE_PREMULTIPLIED)),
                m_snapshot.put());
        }
    }

    auto local = [&](const D2D1_RECT_F& r) {
        return D2D1_RECT_F{r.left - windowPosition.x, r.top - windowPosition.y,
                           r.right - windowPosition.x,
                           r.bottom - windowPosition.y};
    };
    const D2D1_RECT_F from = local(m_source);
    const D2D1_RECT_F to = local(panel);

    const float tx = (float)std::clamp(m_openX, -0.05, 1.08);
    const float ty = (float)std::clamp(m_open, -0.05, 1.08);
    auto lerp = [](float a, float b, float t) { return a + (b - a) * t; };
    const D2D1_RECT_F shape{lerp(from.left, to.left, tx),
                            lerp(from.top, to.top, ty),
                            lerp(from.right, to.right, tx),
                            lerp(from.bottom, to.bottom, ty)};
    // How far down it has stretched, which leads the contents.
    const float progress = std::clamp(ty, 0.0f, 1.0f);
    const float fromRadius = (from.bottom - from.top) / 2;
    const float toRadius =
        (m_fromPill ? kIslandPanelRadius : kPanelRadius) * m_scale;
    const float radius = std::clamp(
        fromRadius + (toRadius - fromRadius) * progress, 0.0f,
        std::max(0.0f, std::min(shape.right - shape.left,
                                shape.bottom - shape.top) /
                           2));

    // Opaque out of the pill, so the pill under it doesn't show through.
    winrt::com_ptr<ID2D1SolidColorBrush> brush;
    target->CreateSolidColorBrush(
        Bg(m_fromPill ? 1.0f : 0.94f), brush.put());
    target->FillRoundedRectangle({shape, radius, radius}, brush.get());
    brush->SetColor(Fg(0.10f * (m_fromPill ? 1.0f : std::clamp(progress * 2, 0.0f, 1.0f))));
    target->DrawRoundedRectangle(
        {{shape.left + 0.5f, shape.top + 0.5f, shape.right - 0.5f,
          shape.bottom - 0.5f},
         radius,
         radius},
        brush.get(), 1);

    // Clipped to the rounded shape.
    winrt::com_ptr<ID2D1Factory> factory;
    target->GetFactory(factory.put());
    winrt::com_ptr<ID2D1RoundedRectangleGeometry> clip;
    factory->CreateRoundedRectangleGeometry({shape, radius, radius},
                                            clip.put());

    // The pill's contents, shrinking and fading away in the middle of the
    // stretching shape.
    const float pillOpacity = std::clamp(1 - progress * 4, 0.0f, 1.0f);
    if (m_fromPill && m_snapshot && pillOpacity > 0.01f && clip) {
        const float shrink = 1 - 0.15f * progress;
        const float w = m_snapshotSize.cx * shrink;
        const float h = m_snapshotSize.cy * shrink;
        const float centerX = (shape.left + shape.right) / 2;
        const float centerY = from.top + m_snapshotSize.cy / 2.0f;
        winrt::com_ptr<ID2D1Layer> layer;
        target->CreateLayer(nullptr, layer.put());
        target->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), clip.get()),
            layer.get());
        target->DrawBitmap(m_snapshot.get(),
                           {centerX - w / 2, centerY - h / 2, centerX + w / 2,
                            centerY + h / 2},
                           pillOpacity);
        target->PopLayer();
    }

    // The panel's contents (laid out at m_size), centered in the shape as
    // it changes size, fading in and sliding down into place.
    const float contents =
        std::clamp((progress - 0.45f) / 0.55f, 0.0f, 1.0f);
    const float contentLeft =
        std::round((to.left + to.right) / 2 - m_size.cx / 2.0f);
    const float contentTop = to.top;
    m_contentOffset = {(LONG)contentLeft, (LONG)contentTop};
    if (contents > 0.01f && clip) {
        winrt::com_ptr<ID2D1Layer> layer;
        target->CreateLayer(nullptr, layer.put());
        target->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), clip.get(),
                                  D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                  D2D1::IdentityMatrix(), contents),
            layer.get());
        target->SetTransform(D2D1::Matrix3x2F::Translation(
            contentLeft,
            contentTop + (m_direction == Direction::Up     ? 1.0f
                          : m_direction == Direction::Down ? -1.0f
                                                           : 0.0f) *
                             (1 - progress) * 10 * m_scale));
        Draw(target, brush.get());
        target->SetTransform(D2D1::Matrix3x2F::Identity());
        target->PopLayer();
    }

    m_canvas.End(m_hwnd, windowPosition, 1);
}

LRESULT Panel::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam,
                             LPARAM lParam) {
    const POINT pt{(short)LOWORD(lParam) - m_contentOffset.x,
                   (short)HIWORD(lParam) - m_contentOffset.y};
    bool redraw = false;
    switch (msg) {
        case WM_ACTIVATE:
            // Clicked somewhere else.
            if (LOWORD(wParam) == WA_INACTIVE) {
                Close();
            }
            break;

        case WM_KEYDOWN:
            redraw = OnKey(wParam);
            if (wParam == VK_ESCAPE && !redraw) {
                Close();
            }
            break;

        case WM_CHAR:
            redraw = OnChar((WCHAR)wParam);
            break;

        case WM_TIMER:
            KillTimer(hWnd, wParam);
            if (wParam == kPanelScreenshotTimerId) {
                redraw = OnAppMessage(WM_APP + 64, 0, 0);
            }
            break;

        case WM_MOUSEMOVE:
            if (!m_tracking) {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hWnd, 0};
                m_tracking = TrackMouseEvent(&track);
            }
            redraw = OnMouseMove(pt);
            break;

        case WM_MOUSELEAVE:
            m_tracking = false;
            redraw = OnMouseLeave();
            break;

        case WM_LBUTTONDOWN:
            redraw = OnMouseDown(pt);
            break;

        case WM_LBUTTONUP:
            redraw = OnMouseUp(pt);
            break;

        case WM_MOUSEWHEEL:
            redraw = OnWheel(GET_WHEEL_DELTA_WPARAM(wParam));
            break;

        default:
            if (msg >= WM_APP && msg <= 0xBFFF) {
                redraw = OnAppMessage(msg, wParam, lParam);
                break;
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    if (redraw && m_open > 0) {
        Render();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// The calendar, like the one in the macOS notification center: the month,
// today marked, arrows (or the mouse wheel) to change the month.

int DaysInMonth(int year, int month) {
    static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    return month == 2 && leap ? 29 : kDays[month - 1];
}

// 0 is Sunday.
int DayOfWeek(int year, int month, int day) {
    SYSTEMTIME time{(WORD)year, (WORD)month, 0, (WORD)day};
    FILETIME file;
    SystemTimeToFileTime(&time, &file);
    FileTimeToSystemTime(&file, &time);
    return time.wDayOfWeek;
}

// Events and reminders, kept by the mod (no accounts): a day, an optional
// time, a title, and whether to be reminded (in the pill, with a sound).
struct CalendarEvent {
    long long id = 0;
    int year = 0;
    int month = 0;
    int day = 0;
    // Minutes into the day, or -1 for the whole day.
    int minute = -1;
    std::wstring title;
    bool remind = true;
    bool fired = false;
};

std::vector<CalendarEvent> g_events;
bool g_eventsLoaded;
constexpr size_t kMaxEvents = 200;
// Whole-day events remind at this hour.
constexpr int kAllDayReminderMinute = 9 * 60;

void LoadEvents() {
    if (g_eventsLoaded) {
        return;
    }
    g_eventsLoaded = true;
    std::vector<WCHAR> value(64 * 1024);
    Wh_GetStringValue(L"events", value.data(), (size_t)value.size());
    // One event per line: id, year, month, day, minute, remind, fired, title
    // (tab-separated).
    for (PCWSTR line = value.data(); *line;) {
        PCWSTR end = wcschr(line, L'\n');
        const std::wstring text =
            end ? std::wstring(line, end) : std::wstring(line);
        std::vector<std::wstring> fields;
        size_t at = 0;
        for (int i = 0; i < 7; i++) {
            const size_t tab = text.find(L'\t', at);
            if (tab == std::wstring::npos) {
                break;
            }
            fields.push_back(text.substr(at, tab - at));
            at = tab + 1;
        }
        if (fields.size() == 7) {
            CalendarEvent event;
            event.id = _wtoi64(fields[0].c_str());
            event.year = _wtoi(fields[1].c_str());
            event.month = _wtoi(fields[2].c_str());
            event.day = _wtoi(fields[3].c_str());
            event.minute = _wtoi(fields[4].c_str());
            event.remind = fields[5] == L"1";
            event.fired = fields[6] == L"1";
            event.title = text.substr(at);
            if (event.month >= 1 && event.month <= 12 && event.day >= 1 &&
                event.day <= 31) {
                g_events.push_back(std::move(event));
            }
        }
        if (!end) {
            break;
        }
        line = end + 1;
    }
}

void SaveEvents() {
    std::wstring value;
    for (const auto& event : g_events) {
        value += std::to_wstring(event.id) + L"\t" +
                 std::to_wstring(event.year) + L"\t" +
                 std::to_wstring(event.month) + L"\t" +
                 std::to_wstring(event.day) + L"\t" +
                 std::to_wstring(event.minute) + L"\t" +
                 (event.remind ? L"1" : L"0") + L"\t" +
                 (event.fired ? L"1" : L"0") + L"\t" + event.title + L"\n";
    }
    Wh_SetStringValue(L"events", value.c_str());
}

bool HasEvents(int year, int month, int day) {
    return std::any_of(g_events.begin(), g_events.end(),
                       [&](const CalendarEvent& event) {
                           return event.year == year && event.month == month &&
                                  event.day == day;
                       });
}

// A local time as minutes since 1601, to compare.
long long LocalMinutes(int year, int month, int day, int minute) {
    SYSTEMTIME time{(WORD)year, (WORD)month, 0, (WORD)day,
                    (WORD)(minute / 60), (WORD)(minute % 60)};
    FILETIME file;
    if (!SystemTimeToFileTime(&time, &file)) {
        return 0;
    }
    return (((long long)file.dwHighDateTime << 32) | file.dwLowDateTime) /
           (60LL * 10000000);
}

// The reminders due now (marked as done). Missed ones from long ago are
// marked done without reminding.
std::vector<CalendarEvent> TakeDueReminders() {
    LoadEvents();
    SYSTEMTIME now;
    GetLocalTime(&now);
    const long long current = LocalMinutes(now.wYear, now.wMonth, now.wDay,
                                           now.wHour * 60 + now.wMinute);
    std::vector<CalendarEvent> due;
    bool changed = false;
    for (auto& event : g_events) {
        if (!event.remind || event.fired) {
            continue;
        }
        const long long at = LocalMinutes(
            event.year, event.month, event.day,
            event.minute >= 0 ? event.minute : kAllDayReminderMinute);
        if (current < at) {
            continue;
        }
        event.fired = true;
        changed = true;
        if (current - at < 12 * 60) {
            due.push_back(event);
        }
    }
    if (changed) {
        SaveEvents();
    }
    return due;
}

// "14:30", or "All day".
std::wstring EventTimeText(int minute) {
    if (minute < 0) {
        return Tr(L"All day", L"Dia todo");
    }
    SYSTEMTIME time{2000, 1, 0, 1, (WORD)(minute / 60), (WORD)(minute % 60)};
    WCHAR text[32] = L"";
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &time, nullptr,
                    text, ARRAYSIZE(text));
    return text;
}

class CalendarPanel : public Panel {
   protected:
    void Prepare(float scale) override;
    void OnOpen() override;
    void Draw(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush) override;
    bool OnMouseMove(POINT pt) override;
    bool OnMouseLeave() override;
    bool OnMouseUp(POINT pt) override;
    bool OnWheel(int delta) override;
    bool OnKey(WPARAM key) override;
    bool OnChar(WCHAR c) override;

   public:
    // Called every second while open, for the clock.
    void Refresh() {
        if (IsOpen()) {
            Render();
        }
    }
    // Opens on a day (a reminder's).
    void SelectDay(int year, int month, int day) {
        m_openOn = {year, month, day};
    }

   protected:
    bool AnimateContents(double dt) override;

   private:
    enum class Part {
        None,
        Previous,
        Next,
        Title,
        Day,          // A day of the month (value).
        DeleteEvent,  // An event's "x" (id).
        EventRow,     // An event, to edit it (id).
        Add,
        TitleField,
        // The value: 0 in the box (to type), +1/-1 on its arrows.
        Hour,
        Minute,
        AllDay,
        Remind,
        Save,
        Cancel,
    };
    struct Hit {
        Part part = Part::None;
        long long value = 0;
        bool operator==(const Hit& other) const {
            return part == other.part && value == other.value;
        }
    };
    struct DayOpen {
        int year = 0;
        int month = 0;
        int day = 0;
    };

    void ChangeMonth(int delta);
    void Layout();
    Hit HitTest(POINT pt) const;
    // A month's title and days, moved sideways by `offset` pixels.
    void DrawMonth(ID2D1RenderTarget* target,
                   ID2D1SolidColorBrush* brush,
                   int year,
                   int month,
                   float offset,
                   float opacity);
    void DrawField(ID2D1RenderTarget* target,
                   ID2D1SolidColorBrush* brush,
                   const D2D1_RECT_F& rect,
                   const std::wstring& text,
                   PCWSTR placeholder,
                   bool focused);
    std::vector<const CalendarEvent*> DayEvents() const;
    void SaveNewEvent();
    void StartEditing(const CalendarEvent* event);
    void StopEditing();
    void TypeDigit(int digit);

    winrt::com_ptr<IDWriteTextFormat> m_titleFormat;
    winrt::com_ptr<IDWriteTextFormat> m_dayFormat;
    winrt::com_ptr<IDWriteTextFormat> m_iconFormat;
    winrt::com_ptr<IDWriteTextFormat> m_clockFormat;
    winrt::com_ptr<IDWriteTextFormat> m_dateFormat;
    winrt::com_ptr<IDWriteTextFormat> m_textFormat;
    winrt::com_ptr<IDWriteTextFormat> m_smallFormat;
    winrt::com_ptr<IDWriteTypography> m_tabular;
    // Changing months: the one before slides out, the new one in (m_slide
    // from 1 to 0), in the direction of m_slideDirection.
    int m_fromYear = 0;
    int m_fromMonth = 0;
    double m_slide = 0;
    double m_slideVelocity = 0;
    int m_slideDirection = 0;
    float m_preparedScale = 0;
    D2D1_RECT_F m_previous{}, m_next{}, m_title{};
    Hit m_hover;
    int m_year = 0;
    int m_month = 0;
    // The day chosen (its events below the month).
    int m_selectedYear = 0;
    int m_selectedMonth = 0;
    int m_selectedDay = 0;
    DayOpen m_openOn;
    // Where things are.
    std::vector<std::pair<int, D2D1_RECT_F>> m_dayRects;
    std::vector<std::pair<long long, D2D1_RECT_F>> m_eventRows;
    D2D1_RECT_F m_eventsHeader{}, m_addButton{}, m_titleField{}, m_hourBox{},
        m_minuteBox{}, m_allDayButton{}, m_remindButton{}, m_saveButton{},
        m_cancelButton{}, m_status{};
    // A new event being written (or one being changed, m_editingId): its
    // title, its time (or the whole day), and the reminder.
    bool m_adding = false;
    long long m_editingId = 0;
    // Where typing goes; in the hour and the minute, how many figures were
    // typed (two move on).
    enum class Field { Title, Hour, Minute } m_focus = Field::Title;
    int m_digits = 0;
    // The fields fading and sliding in (0 to 1).
    double m_form = 0;
    double m_formVelocity = 0;
    std::wstring m_newTitle;
    int m_newHour = 9;
    int m_newMinute = 0;
    bool m_newAllDay = false;
    bool m_newRemind = true;
    void StepTime(Part part, int steps);
    std::wstring m_statusText;
};

// The clock on top of the calendar; the events' rows under it.
constexpr float kCalendarClockHeight = 74;
constexpr float kEventRowHeight = 30;
constexpr int kEventsShown = 4;
constexpr WCHAR kGlyphEventDelete = 0xE711;
constexpr WCHAR kGlyphChevronDownCalendar = 0xE70D;

void CalendarPanel::Prepare(float scale) {
    if (scale != m_preparedScale) {
        m_preparedScale = scale;
        m_clockFormat = MakeFormat(L"Segoe UI Variable Display",
                                   DWRITE_FONT_WEIGHT_SEMI_BOLD, 34 * scale,
                                   DWRITE_TEXT_ALIGNMENT_LEADING);
        m_dateFormat = MakeFormat(L"Segoe UI Variable Text",
                                  DWRITE_FONT_WEIGHT_NORMAL, 13 * scale,
                                  DWRITE_TEXT_ALIGNMENT_LEADING);
        if (!m_tabular &&
            SUCCEEDED(m_dwrite->CreateTypography(m_tabular.put()))) {
            m_tabular->AddFontFeature(
                {DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES, 1});
        }
        m_titleFormat = MakeFormat(L"Segoe UI Variable Display",
                                   DWRITE_FONT_WEIGHT_BOLD, 15 * scale,
                                   DWRITE_TEXT_ALIGNMENT_LEADING);
        m_dayFormat = MakeFormat(L"Segoe UI Variable Text",
                                 DWRITE_FONT_WEIGHT_SEMI_BOLD, 13 * scale);
        m_iconFormat = MakeFormat(L"Segoe Fluent Icons",
                                  DWRITE_FONT_WEIGHT_NORMAL, 12 * scale);
        m_textFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       12.5f * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_smallFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       11.5f * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    LoadEvents();
    Layout();
}

void CalendarPanel::OnOpen() {
    SYSTEMTIME today;
    GetLocalTime(&today);
    m_selectedYear = m_year = today.wYear;
    m_selectedMonth = m_month = today.wMonth;
    m_selectedDay = today.wDay;
    // A reminder's day.
    if (m_openOn.year) {
        m_selectedYear = m_year = m_openOn.year;
        m_selectedMonth = m_month = m_openOn.month;
        m_selectedDay = m_openOn.day;
        m_openOn = {};
    }
    m_hover = {};
    m_slide = 0;
    m_slideVelocity = 0;
    m_adding = false;
    m_editingId = 0;
    m_form = 0;
    m_formVelocity = 0;
    m_statusText.clear();
    Layout();
}

bool CalendarPanel::AnimateContents(double dt) {
    const bool slide =
        SpringTowards(m_slide, m_slideVelocity, 0, 0.45, dt, 0.85);
    const bool form = SpringTowards(m_form, m_formVelocity, m_adding ? 1 : 0,
                                    m_adding ? 0.4 : 0.25, dt,
                                    m_adding ? kBounceDamping : 1.0);
    return slide || form;
}

// Writing an event: a new one (nullptr) for the chosen day, or one to change.
void CalendarPanel::StartEditing(const CalendarEvent* event) {
    m_adding = true;
    m_statusText.clear();
    m_focus = Field::Title;
    m_digits = 0;
    if (event) {
        m_editingId = event->id;
        m_newTitle = event->title;
        m_newRemind = event->remind;
        m_newAllDay = event->minute < 0;
        const int minute = event->minute < 0 ? 9 * 60 : event->minute;
        m_newHour = minute / 60;
        m_newMinute = minute % 60;
    } else {
        m_editingId = 0;
        m_newTitle.clear();
        m_newRemind = true;
        m_newAllDay = false;
        // The next full hour.
        SYSTEMTIME now;
        GetLocalTime(&now);
        m_newHour = (now.wHour + 1) % 24;
        m_newMinute = 0;
    }
    m_form = 0;
    m_formVelocity = 0;
    StartIslandAnimation();
    Layout();
}

void CalendarPanel::StopEditing() {
    m_adding = false;
    m_editingId = 0;
    m_statusText.clear();
    StartIslandAnimation();
    Layout();
}

// A figure typed in the hour or the minute: two make the value (the hour
// then moves on to the minute), like typing a time on a phone.
void CalendarPanel::TypeDigit(int digit) {
    m_newAllDay = false;
    if (m_focus == Field::Hour) {
        if (m_digits == 0) {
            m_newHour = digit;
            m_digits = 1;
            // No hour starts with 3 or more and has a second figure.
            if (digit > 2) {
                m_focus = Field::Minute;
                m_digits = 0;
            }
        } else {
            const int value = m_newHour * 10 + digit;
            m_newHour = value <= 23 ? value : digit;
            m_focus = Field::Minute;
            m_digits = 0;
        }
    } else if (m_focus == Field::Minute) {
        if (m_digits == 0) {
            m_newMinute = digit;
            m_digits = digit > 5 ? 0 : 1;
        } else {
            m_newMinute = std::min(m_newMinute * 10 + digit, 59);
            m_digits = 0;
        }
    }
}

void CalendarPanel::ChangeMonth(int delta) {
    // The month shown slides out (from where it is, if still sliding).
    if (m_slide < 0.5) {
        m_fromYear = m_year;
        m_fromMonth = m_month;
    }
    m_slideDirection = delta > 0 ? 1 : -1;
    m_slide = 1;
    m_slideVelocity = 0;
    StartIslandAnimation();
    m_month += delta;
    while (m_month < 1) {
        m_month += 12;
        m_year--;
    }
    while (m_month > 12) {
        m_month -= 12;
        m_year++;
    }
    Layout();
}

std::vector<const CalendarEvent*> CalendarPanel::DayEvents() const {
    std::vector<const CalendarEvent*> events;
    for (const auto& event : g_events) {
        if (event.year == m_selectedYear && event.month == m_selectedMonth &&
            event.day == m_selectedDay) {
            events.push_back(&event);
        }
    }
    std::stable_sort(events.begin(), events.end(),
                     [](const CalendarEvent* a, const CalendarEvent* b) {
                         return a->minute < b->minute;
                     });
    return events;
}

void CalendarPanel::Layout() {
    const float scale = m_scale;
    const float padding = std::round(kPanelPadding * scale);
    const float cell = std::round(kCalendarCell * scale);
    const float width = padding * 2 + cell * 7;
    const float top = padding + std::round(kCalendarClockHeight * scale);
    const float titleHeight = cell * 1.2f;
    const float gridTop = top + titleHeight + cell * 0.8f;

    // The days of the month shown, where they are.
    m_dayRects.clear();
    WCHAR firstDayText[4] = L"6";
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK,
                    firstDayText, ARRAYSIZE(firstDayText));
    const int firstDay = (_wtoi(firstDayText) + 1) % 7;
    const int lead = m_year ? (DayOfWeek(m_year, m_month, 1) - firstDay + 7) % 7
                            : 0;
    const int days = m_year ? DaysInMonth(m_year, m_month) : 0;
    for (int day = 1; day <= days; day++) {
        const int index = day - 1 + lead;
        m_dayRects.push_back(
            {day,
             {padding + (index % 7) * cell, gridTop + (index / 7) * cell,
              padding + (index % 7 + 1) * cell,
              gridTop + (index / 7 + 1) * cell}});
    }

    // The chosen day's events, then adding one.
    float y = gridTop + cell * 6 + std::round(6 * scale);
    const float row = std::round(kEventRowHeight * scale);
    m_eventsHeader = {padding, y, width - padding, y + row};
    y += row;
    m_eventRows.clear();
    const auto events = DayEvents();
    for (size_t i = 0; i < events.size() && (int)i < kEventsShown; i++) {
        m_eventRows.push_back(
            {events[i]->id, {padding, y, width - padding, y + row}});
        y += row;
    }
    if (events.empty()) {
        // "No events".
        y += row;
    }
    y += std::round(4 * scale);
    const float field = std::round(32 * scale);
    if (!m_adding) {
        m_addButton = {padding, y, width - padding, y + field};
        m_titleField = m_hourBox = m_minuteBox = m_allDayButton =
            m_remindButton = m_saveButton = m_cancelButton = m_status = {};
        y += field;
    } else {
        // The title; under it, the time and the reminder; then "Cancel" and
        // "Add" (or "Save").
        m_addButton = {};
        const float gap = std::round(6 * scale);
        m_titleField = {padding, y, width - padding, y + field};
        y += field + gap;
        // From the right: the bell, the minute, ":", the hour; "All day"
        // takes what's left.
        const float box = std::round(50 * scale);
        const float time = std::round(40 * scale);
        const float colon = std::round(12 * scale);
        m_remindButton = {width - padding - time, y, width - padding,
                          y + time};
        m_minuteBox = {m_remindButton.left - gap * 2 - box, y,
                       m_remindButton.left - gap * 2, y + time};
        m_hourBox = {m_minuteBox.left - colon - box, y,
                     m_minuteBox.left - colon, y + time};
        m_allDayButton = {padding, y, m_hourBox.left - gap * 2, y + time};
        y += time;
        m_status = {padding, y, width - padding, y + row * 0.8f};
        if (!m_statusText.empty()) {
            y += row * 0.8f;
        }
        y += gap * 1.5f;
        const float buttonWidth =
            std::floor((width - padding * 2 - gap) / 2);
        m_cancelButton = {padding, y, padding + buttonWidth, y + field};
        m_saveButton = {width - padding - buttonWidth, y, width - padding,
                        y + field};
        y += field;
    }
    y += padding;
    m_size = {(LONG)width, (LONG)std::ceil(y)};
}

CalendarPanel::Hit CalendarPanel::HitTest(POINT pt) const {
    if (PointInRect(pt, m_previous)) {
        return {Part::Previous};
    }
    if (PointInRect(pt, m_next)) {
        return {Part::Next};
    }
    if (PointInRect(pt, m_title)) {
        return {Part::Title};
    }
    for (const auto& [day, rect] : m_dayRects) {
        if (PointInRect(pt, rect)) {
            return {Part::Day, day};
        }
    }
    for (const auto& [id, rect] : m_eventRows) {
        // The "x" at the end of the row; the rest of it edits the event.
        if (PointInRect(pt, rect)) {
            if (pt.x >= rect.right - std::round(28 * m_scale)) {
                return {Part::DeleteEvent, id};
            }
            return {Part::EventRow, id};
        }
    }
    if (PointInRect(pt, m_addButton)) {
        return {Part::Add};
    }
    if (PointInRect(pt, m_titleField)) {
        return {Part::TitleField};
    }
    // The hour and the minute: their arrows (at the right end, up adds,
    // down takes away), or the box itself, to type.
    for (Part part : {Part::Hour, Part::Minute}) {
        const D2D1_RECT_F& box = part == Part::Hour ? m_hourBox : m_minuteBox;
        if (!PointInRect(pt, box)) {
            continue;
        }
        if (pt.x >= box.right - std::round(16 * m_scale)) {
            return {part, pt.y < (box.top + box.bottom) / 2 ? 1 : -1};
        }
        return {part, 0};
    }
    if (PointInRect(pt, m_allDayButton)) {
        return {Part::AllDay};
    }
    if (PointInRect(pt, m_remindButton)) {
        return {Part::Remind};
    }
    if (PointInRect(pt, m_saveButton)) {
        return {Part::Save};
    }
    if (PointInRect(pt, m_cancelButton)) {
        return {Part::Cancel};
    }
    return {};
}

void CalendarPanel::DrawField(ID2D1RenderTarget* target,
                              ID2D1SolidColorBrush* brush,
                              const D2D1_RECT_F& rect,
                              const std::wstring& text,
                              PCWSTR placeholder,
                              bool focused) {
    const float s = m_scale;
    const float radius = std::round(8 * s);
    brush->SetColor(Fg(focused ? 0.14f : 0.09f));
    target->FillRoundedRectangle({rect, radius, radius}, brush);
    if (focused) {
        brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
        target->DrawRoundedRectangle(
            {{rect.left + 0.75f, rect.top + 0.75f, rect.right - 0.75f,
              rect.bottom - 0.75f},
             radius,
             radius},
            brush, 1.5f);
    }
    const D2D1_RECT_F textRect{rect.left + std::round(10 * s), rect.top,
                               rect.right - std::round(8 * s), rect.bottom};
    const bool empty = text.empty();
    const std::wstring shown = empty ? std::wstring(placeholder) : text;
    float caretX = textRect.left;
    winrt::com_ptr<IDWriteTextLayout> layout;
    if (SUCCEEDED(m_dwrite->CreateTextLayout(
            shown.c_str(), (UINT32)shown.size(), m_textFormat.get(), 10000,
            textRect.bottom - textRect.top, layout.put()))) {
        DWRITE_TEXT_METRICS metrics;
        layout->GetMetrics(&metrics);
        const float overflow =
            empty ? 0
                  : std::max(0.0f, metrics.widthIncludingTrailingWhitespace -
                                       (textRect.right - textRect.left));
        target->PushAxisAlignedClip(textRect, D2D1_ANTIALIAS_MODE_ALIASED);
        brush->SetColor(Fg(empty ? 0.4f : 0.95f));
        target->DrawTextLayout({textRect.left - overflow, textRect.top},
                               layout.get(), brush);
        target->PopAxisAlignedClip();
        if (!empty) {
            caretX = textRect.left + metrics.widthIncludingTrailingWhitespace -
                     overflow;
        }
    }
    if (focused) {
        const float middle = (rect.top + rect.bottom) / 2;
        brush->SetColor(Fg(0.9f));
        target->FillRectangle({caretX + 1, middle - std::round(8 * s),
                               caretX + 2.5f, middle + std::round(8 * s)},
                              brush);
    }
}

void CalendarPanel::Draw(ID2D1RenderTarget* target,
                         ID2D1SolidColorBrush* brush) {
    if (!m_titleFormat || !m_dayFormat || !m_iconFormat || !m_textFormat ||
        !m_smallFormat) {
        return;
    }
    const float scale = m_scale;
    const float width = (float)m_size.cx;
    const float padding = std::round(kPanelPadding * scale);
    const float cell = std::round(kCalendarCell * scale);

    // The time, big, and the date ("sexta-feira, 3 de outubro").
    SYSTEMTIME now;
    GetLocalTime(&now);
    const float clockHeight = std::round(kCalendarClockHeight * scale);
    if (m_clockFormat && m_dateFormat) {
        WCHAR time[32] = L"";
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, nullptr, time,
                        ARRAYSIZE(time));
        const D2D1_RECT_F timeRect{padding + cell * 0.2f, padding,
                                   width - padding,
                                   padding + clockHeight * 0.62f};
        brush->SetColor(Fg(0.97f));
        winrt::com_ptr<IDWriteTextLayout> layout;
        if (SUCCEEDED(m_dwrite->CreateTextLayout(
                time, (UINT32)wcslen(time), m_clockFormat.get(),
                timeRect.right - timeRect.left,
                timeRect.bottom - timeRect.top, layout.put()))) {
            if (m_tabular) {
                layout->SetTypography(m_tabular.get(),
                                      {0, (UINT32)wcslen(time)});
            }
            target->DrawTextLayout({timeRect.left, timeRect.top}, layout.get(),
                                   brush);
        }
        WCHAR date[96] = L"";
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &now, nullptr,
                        date, ARRAYSIZE(date), nullptr);
        if (date[0]) {
            date[0] = towupper(date[0]);
        }
        brush->SetColor(Fg(0.6f));
        target->DrawText(date, (UINT32)wcslen(date), m_dateFormat.get(),
                         {timeRect.left, timeRect.bottom, width - padding,
                          padding + clockHeight - std::round(8 * scale)},
                         brush);
        // A line under it.
        brush->SetColor(Fg(0.1f));
        target->FillRectangle({padding, padding + clockHeight - 1,
                               width - padding, padding + clockHeight},
                              brush);
    }
    const float top = padding + clockHeight;

    // The arrows; the title slides with the days.
    const float titleHeight = cell * 1.2f;
    m_title = {padding + cell * 0.2f, top, width - padding - cell * 2,
               top + titleHeight};
    m_previous = {width - padding - cell * 2, top, width - padding - cell,
                  top + titleHeight};
    m_next = {width - padding - cell, top, width - padding,
              top + titleHeight};
    for (Part arrow : {Part::Previous, Part::Next}) {
        const D2D1_RECT_F& rect = arrow == Part::Previous ? m_previous : m_next;
        if (m_hover.part == arrow) {
            const float inset = cell * 0.18f;
            const D2D1_RECT_F back{rect.left + inset, rect.top + inset,
                                   rect.right - inset, rect.bottom - inset};
            brush->SetColor(Fg(0.16f));
            target->FillRoundedRectangle(
                {back, (back.bottom - back.top) / 2,
                 (back.bottom - back.top) / 2},
                brush);
        }
        const WCHAR glyph = arrow == Part::Previous ? kGlyphPrevious : kGlyphNext;
        brush->SetColor(Fg(0.9f));
        target->DrawText(&glyph, 1, m_iconFormat.get(), rect, brush);
    }

    // Weekday initials, starting on the locale's first day of the week.
    WCHAR firstDayText[4] = L"6";
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK,
                    firstDayText, ARRAYSIZE(firstDayText));
    // LOCALE_IFIRSTDAYOFWEEK: 0 is Monday. Here, 0 is Sunday.
    const int firstDay = (_wtoi(firstDayText) + 1) % 7;
    const float headerTop = top + titleHeight;
    for (int column = 0; column < 7; column++) {
        const int weekday = (firstDay + column) % 7;
        // LOCALE_SSHORTESTDAYNAME1 is Monday.
        WCHAR name[16] = L"";
        GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                        LOCALE_SSHORTESTDAYNAME1 + (weekday + 6) % 7, name,
                        ARRAYSIZE(name));
        const D2D1_RECT_F rect{padding + column * cell, headerTop,
                               padding + (column + 1) * cell,
                               headerTop + cell * 0.8f};
        brush->SetColor(Fg(0.5f));
        target->DrawText(name, (UINT32)wcslen(name), m_dayFormat.get(), rect,
                         brush);
    }

    // The months: the new one slides in, the one before out, both fading,
    // clipped to the calendar.
    const float slide = (float)std::clamp(m_slide, 0.0, 1.0);
    target->PushAxisAlignedClip(
        {padding, top, width - padding, m_eventsHeader.top},
        D2D1_ANTIALIAS_MODE_ALIASED);
    if (slide > 0.001f && m_fromMonth) {
        DrawMonth(target, brush, m_fromYear, m_fromMonth,
                  -m_slideDirection * (1 - slide) * width * 0.6f, slide);
    }
    DrawMonth(target, brush, m_year, m_month,
              m_slideDirection * slide * width * 0.6f, 1 - slide);
    target->PopAxisAlignedClip();

    // The chosen day's events.
    SYSTEMTIME chosen{(WORD)m_selectedYear, (WORD)m_selectedMonth, 0,
                      (WORD)m_selectedDay};
    WCHAR day[64] = L"";
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &chosen, L"d MMMM", day,
                    ARRAYSIZE(day), nullptr);
    brush->SetColor(Fg(0.1f));
    target->FillRectangle({padding, m_eventsHeader.top, width - padding,
                           m_eventsHeader.top + 1},
                          brush);
    const std::wstring header =
        std::wstring(Tr(L"Events, ", L"Eventos, ")) + day;
    brush->SetColor(Fg(0.6f));
    target->DrawText(header.c_str(), (UINT32)header.size(),
                     m_smallFormat.get(), m_eventsHeader, brush);
    const auto events = DayEvents();
    if (events.empty()) {
        PCWSTR none = Tr(L"No events", L"Nenhum evento");
        brush->SetColor(Fg(0.4f));
        target->DrawText(none, (UINT32)wcslen(none), m_textFormat.get(),
                         {padding, m_eventsHeader.bottom, width - padding,
                          m_eventsHeader.bottom +
                              std::round(kEventRowHeight * scale)},
                         brush);
    }
    for (size_t i = 0; i < m_eventRows.size() && i < events.size(); i++) {
        const CalendarEvent& event = *events[i];
        const D2D1_RECT_F& row = m_eventRows[i].second;
        const bool hover = (m_hover.part == Part::DeleteEvent) &&
                           m_hover.value == event.id;
        // The row under the mouse (or being edited) lights up: a click edits
        // it.
        if ((m_hover.value == event.id &&
             (m_hover.part == Part::EventRow ||
              m_hover.part == Part::DeleteEvent)) ||
            (m_adding && m_editingId == event.id)) {
            const float rowRadius = std::round(7 * scale);
            brush->SetColor(Fg(m_adding && m_editingId == event.id ? 0.12f
                                                                   : 0.07f));
            target->FillRoundedRectangle({row, rowRadius, rowRadius}, brush);
        }
        // A colored bar, the time, the title, the bell, the "x".
        brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, 0.9f));
        target->FillRoundedRectangle(
            {{row.left, row.top + std::round(6 * scale),
              row.left + std::round(3 * scale),
              row.bottom - std::round(6 * scale)},
             1.5f,
             1.5f},
            brush);
        const float timeWidth = std::round(64 * scale);
        const std::wstring time = EventTimeText(event.minute);
        brush->SetColor(Fg(0.55f));
        target->DrawText(time.c_str(), (UINT32)time.size(),
                         m_smallFormat.get(),
                         {row.left + std::round(10 * scale), row.top,
                          row.left + timeWidth, row.bottom},
                         brush);
        brush->SetColor(Fg(0.95f));
        target->DrawText(event.title.c_str(), (UINT32)event.title.size(),
                         m_textFormat.get(),
                         {row.left + timeWidth, row.top,
                          row.right - std::round(48 * scale), row.bottom},
                         brush);
        if (event.remind) {
            brush->SetColor(Fg(event.fired ? 0.3f : 0.6f));
            target->DrawText(&kGlyphBell, 1, m_iconFormat.get(),
                             {row.right - std::round(48 * scale), row.top,
                              row.right - std::round(28 * scale), row.bottom},
                             brush);
        }
        brush->SetColor(Fg(hover ? 0.95f : 0.4f));
        target->DrawText(&kGlyphEventDelete, 1, m_iconFormat.get(),
                         {row.right - std::round(28 * scale), row.top,
                          row.right, row.bottom},
                         brush);
    }

    // "+ New event", or its fields.
    if (!m_adding) {
        const float radius = std::round(8 * scale);
        brush->SetColor(Fg(m_hover.part == Part::Add ? 0.16f : 0.09f));
        target->FillRoundedRectangle({m_addButton, radius, radius}, brush);
        PCWSTR add = Tr(L"+  New event", L"+  Novo evento");
        brush->SetColor(Fg(0.85f));
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        target->DrawText(add, (UINT32)wcslen(add), m_textFormat.get(),
                         m_addButton, brush);
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    } else {
        // The fields fade in, sliding down a little.
        const float form = (float)std::clamp(m_form, 0.0, 1.0);
        winrt::com_ptr<ID2D1Layer> formLayer;
        if (form < 0.999f) {
            target->CreateLayer(nullptr, formLayer.put());
        }
        if (formLayer) {
            target->PushLayer(
                D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                      D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                      D2D1::IdentityMatrix(), form),
                formLayer.get());
        }
        // On top of the panel's own placement.
        D2D1_MATRIX_3X2_F base;
        target->GetTransform(&base);
        target->SetTransform(
            D2D1::Matrix3x2F::Translation(
                0, -std::round(8 * scale) *
                       (1 - (float)std::clamp(m_form, 0.0, 1.2))) *
            *D2D1::Matrix3x2F::ReinterpretBaseType(&base));
        DrawField(target, brush, m_titleField, m_newTitle,
                  Tr(L"Title", L"Título"), m_focus == Field::Title);
        // "All day", blue when chosen.
        const float chipRadius =
            (m_allDayButton.bottom - m_allDayButton.top) / 2;
        brush->SetColor(m_newAllDay ? D2D1::ColorF(0.04f, 0.52f, 1.0f, 1)
                                    : Fg(m_hover.part == Part::AllDay ? 0.18f
                                                                       : 0.1f));
        target->FillRoundedRectangle({m_allDayButton, chipRadius, chipRadius},
                                     brush);
        PCWSTR allDay = Tr(L"All day", L"Dia todo");
        brush->SetColor(m_newAllDay ? D2D1::ColorF(1, 1, 1, 1) : Fg(0.9f));
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        target->DrawText(allDay, (UINT32)wcslen(allDay), m_textFormat.get(),
                         m_allDayButton, brush);
        // The hour and the minute: click to type them (blue outline), or
        // use the arrows at their right end (or the wheel, or ↑ ↓).
        const float timeOpacity = m_newAllDay ? 0.35f : 1.0f;
        for (Part part : {Part::Hour, Part::Minute}) {
            const D2D1_RECT_F& box = part == Part::Hour ? m_hourBox : m_minuteBox;
            const bool focused =
                !m_newAllDay && m_focus == (part == Part::Hour ? Field::Hour
                                                               : Field::Minute);
            const float radius = std::round(8 * scale);
            brush->SetColor(Fg((m_hover.part == part || focused ? 0.16f : 0.1f) *
                               timeOpacity));
            target->FillRoundedRectangle({box, radius, radius}, brush);
            if (focused) {
                brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
                target->DrawRoundedRectangle(
                    {{box.left + 0.75f, box.top + 0.75f, box.right - 0.75f,
                      box.bottom - 0.75f},
                     radius,
                     radius},
                    brush, 1.5f);
            }
            WCHAR value[8];
            swprintf_s(value, L"%02d",
                       part == Part::Hour ? m_newHour : m_newMinute);
            // Half typed: only the figure typed so far.
            if (focused && m_digits == 1) {
                swprintf_s(value, L"%d_",
                           part == Part::Hour ? m_newHour : m_newMinute);
            }
            const float arrow = std::round(14 * scale);
            brush->SetColor(Fg(0.95f * timeOpacity));
            target->DrawText(value, (UINT32)wcslen(value), m_dayFormat.get(),
                             {box.left, box.top, box.right - arrow / 2,
                              box.bottom},
                             brush);
            if ((m_hover.part == part || focused) && !m_newAllDay) {
                const float half = (box.bottom - box.top) / 2;
                const bool onArrows = m_hover.part == part;
                brush->SetColor(
                    Fg(onArrows && m_hover.value > 0 ? 0.95f : 0.4f));
                target->DrawText(&kGlyphCollapse, 1, m_iconFormat.get(),
                                 {box.right - arrow, box.top + 2,
                                  box.right - 2, box.top + half},
                                 brush);
                brush->SetColor(
                    Fg(onArrows && m_hover.value < 0 ? 0.95f : 0.4f));
                target->DrawText(&kGlyphChevronDownCalendar, 1,
                                 m_iconFormat.get(),
                                 {box.right - arrow, box.top + half,
                                  box.right - 2, box.bottom - 2},
                                 brush);
            }
        }
        brush->SetColor(Fg(0.7f * timeOpacity));
        target->DrawText(L":", 1, m_dayFormat.get(),
                         {m_hourBox.right, m_hourBox.top, m_minuteBox.left,
                          m_hourBox.bottom},
                         brush);
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        // The reminder's bell: blue when on.
        const float radius = (m_remindButton.bottom - m_remindButton.top) / 2;
        brush->SetColor(m_newRemind ? D2D1::ColorF(0.04f, 0.52f, 1.0f, 1)
                                    : Fg(0.12f));
        target->FillRoundedRectangle({m_remindButton, radius, radius}, brush);
        brush->SetColor(Fg(0.95f));
        target->DrawText(&kGlyphBell, 1, m_iconFormat.get(), m_remindButton,
                         brush);
        const float saveRadius = (m_saveButton.bottom - m_saveButton.top) / 2;
        brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f,
                                     m_hover.part == Part::Save ? 0.85f : 1.0f));
        target->FillRoundedRectangle({m_saveButton, saveRadius, saveRadius},
                                     brush);
        PCWSTR save = m_editingId ? Tr(L"Save", L"Salvar")
                                  : Tr(L"Add", L"Adicionar");
        brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        target->DrawText(save, (UINT32)wcslen(save), m_textFormat.get(),
                         m_saveButton, brush);
        // "Cancel", gray.
        const float cancelRadius =
            (m_cancelButton.bottom - m_cancelButton.top) / 2;
        brush->SetColor(Fg(m_hover.part == Part::Cancel ? 0.18f : 0.1f));
        target->FillRoundedRectangle(
            {m_cancelButton, cancelRadius, cancelRadius}, brush);
        PCWSTR cancel = Tr(L"Cancel", L"Cancelar");
        brush->SetColor(Fg(0.9f));
        target->DrawText(cancel, (UINT32)wcslen(cancel), m_textFormat.get(),
                         m_cancelButton, brush);
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        if (!m_statusText.empty()) {
            brush->SetColor(D2D1::ColorF(1.0f, 0.55f, 0.5f, 1));
            target->DrawText(m_statusText.c_str(),
                             (UINT32)m_statusText.size(), m_smallFormat.get(),
                             m_status, brush);
        }
        target->SetTransform(base);
        if (formLayer) {
            target->PopLayer();
        }
    }
}

void CalendarPanel::DrawMonth(ID2D1RenderTarget* target,
                              ID2D1SolidColorBrush* brush,
                              int year,
                              int month,
                              float offset,
                              float opacity) {
    const float scale = m_scale;
    const float padding = std::round(kPanelPadding * scale);
    const float cell = std::round(kCalendarCell * scale);
    const float top = padding + std::round(kCalendarClockHeight * scale);
    const float titleHeight = cell * 1.2f;

    // Title ("Outubro de 2026").
    SYSTEMTIME first{(WORD)year, (WORD)month, 0, 1};
    WCHAR title[64] = L"";
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_YEARMONTH, &first, nullptr,
                    title, ARRAYSIZE(title), nullptr);
    if (title[0]) {
        title[0] = towupper(title[0]);
    }
    brush->SetColor(Fg(0.95f * opacity));
    target->DrawText(title, (UINT32)wcslen(title), m_titleFormat.get(),
                     {m_title.left + offset, m_title.top, m_title.right + offset,
                      m_title.bottom},
                     brush);

    WCHAR firstDayText[4] = L"6";
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK,
                    firstDayText, ARRAYSIZE(firstDayText));
    const int firstDay = (_wtoi(firstDayText) + 1) % 7;

    // Six weeks of days: today in a red circle like on macOS, the chosen day
    // in a ring, a dot under days with events.
    SYSTEMTIME today;
    GetLocalTime(&today);
    const int lead = (DayOfWeek(year, month, 1) - firstDay + 7) % 7;
    const int days = DaysInMonth(year, month);
    const int previousMonth = month == 1 ? 12 : month - 1;
    const int previousDays =
        DaysInMonth(month == 1 ? year - 1 : year, previousMonth);
    const float gridTop = top + titleHeight + cell * 0.8f;
    for (int index = 0; index < 42; index++) {
        int day = index - lead + 1;
        bool inMonth = true;
        if (day < 1) {
            day += previousDays;
            inMonth = false;
        } else if (day > days) {
            day -= days;
            inMonth = false;
        }
        const D2D1_RECT_F rect{padding + (index % 7) * cell + offset,
                               gridTop + (index / 7) * cell,
                               padding + (index % 7 + 1) * cell + offset,
                               gridTop + (index / 7 + 1) * cell};
        const D2D1_POINT_2F center{(rect.left + rect.right) / 2,
                                   (rect.top + rect.bottom) / 2};
        const float circle = cell * 0.4f;
        const bool isToday = inMonth && day == today.wDay &&
                             month == today.wMonth && year == today.wYear;
        const bool selected = inMonth && day == m_selectedDay &&
                              month == m_selectedMonth &&
                              year == m_selectedYear;
        const bool hover = inMonth && m_hover.part == Part::Day &&
                           m_hover.value == day && year == m_year &&
                           month == m_month;
        if (isToday) {
            brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, opacity));
            target->FillEllipse({center, circle, circle}, brush);
        } else if (hover) {
            brush->SetColor(Fg(0.1f * opacity));
            target->FillEllipse({center, circle, circle}, brush);
        }
        if (selected && !isToday) {
            brush->SetColor(Fg(0.6f * opacity));
            target->DrawEllipse({center, circle, circle}, brush, 1.5f);
        }
        const std::wstring text = std::to_wstring(day);
        // White on today's red circle; the theme's color elsewhere.
        brush->SetColor(isToday ? D2D1::ColorF(1, 1, 1, opacity)
                                : Fg((inMonth ? 0.9f : 0.3f) * opacity));
        target->DrawText(text.c_str(), (UINT32)text.size(), m_dayFormat.get(),
                         rect, brush);
        if (inMonth && HasEvents(year, month, day)) {
            const float dot = 1.8f * scale;
            brush->SetColor(isToday
                                ? D2D1::ColorF(1, 1, 1, opacity)
                                : D2D1::ColorF(1.0f, 0.27f, 0.23f, opacity));
            target->FillEllipse(
                {{center.x, center.y + circle - 3 * scale}, dot, dot}, brush);
        }
    }
}

bool CalendarPanel::OnMouseMove(POINT pt) {
    const Hit hover = HitTest(pt);
    if (hover == m_hover) {
        return false;
    }
    m_hover = hover;
    return true;
}

bool CalendarPanel::OnMouseLeave() {
    if (m_hover.part == Part::None) {
        return false;
    }
    m_hover = {};
    return true;
}

void CalendarPanel::SaveNewEvent() {
    const int minute = m_newAllDay ? -1 : m_newHour * 60 + m_newMinute;
    auto edited = std::find_if(
        g_events.begin(), g_events.end(),
        [&](const CalendarEvent& e) { return m_editingId && e.id == m_editingId; });
    if (edited == g_events.end() && g_events.size() >= kMaxEvents) {
        m_statusText = Tr(L"Too many events", L"Eventos demais");
        Layout();
        return;
    }
    CalendarEvent event;
    if (edited != g_events.end()) {
        event = *edited;
    } else {
        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        event.id = ((long long)now.dwHighDateTime << 32) | now.dwLowDateTime;
        event.year = m_selectedYear;
        event.month = m_selectedMonth;
        event.day = m_selectedDay;
    }
    event.minute = minute;
    event.title = m_newTitle.empty()
                      ? std::wstring(Tr(L"Event", L"Evento"))
                      : m_newTitle;
    event.remind = m_newRemind;
    // A reminder for a time already gone isn't due; moved later, it is again.
    SYSTEMTIME local;
    GetLocalTime(&local);
    event.fired =
        LocalMinutes(event.year, event.month, event.day,
                     minute >= 0 ? minute : kAllDayReminderMinute) <=
        LocalMinutes(local.wYear, local.wMonth, local.wDay,
                     local.wHour * 60 + local.wMinute);
    if (edited != g_events.end()) {
        *edited = std::move(event);
    } else {
        g_events.push_back(std::move(event));
    }
    SaveEvents();
    StopEditing();
}

bool CalendarPanel::OnMouseUp(POINT pt) {
    const Hit hit = HitTest(pt);
    switch (hit.part) {
        case Part::Previous:
            ChangeMonth(-1);
            return true;
        case Part::Next:
            ChangeMonth(1);
            return true;
        case Part::Title: {
            // The title goes back to this month (and today).
            SYSTEMTIME today;
            GetLocalTime(&today);
            const int delta =
                (today.wYear - m_year) * 12 + (today.wMonth - m_month);
            if (delta) {
                ChangeMonth(delta);
            }
            m_selectedYear = today.wYear;
            m_selectedMonth = today.wMonth;
            m_selectedDay = today.wDay;
            Layout();
            return true;
        }
        case Part::Day:
            m_selectedYear = m_year;
            m_selectedMonth = m_month;
            m_selectedDay = (int)hit.value;
            Layout();
            return true;
        case Part::DeleteEvent:
            if (m_adding && m_editingId == hit.value) {
                StopEditing();
            }
            std::erase_if(g_events, [&](const CalendarEvent& event) {
                return event.id == hit.value;
            });
            SaveEvents();
            Layout();
            return true;
        case Part::EventRow:
            for (const auto& event : g_events) {
                if (event.id == hit.value) {
                    StartEditing(&event);
                    break;
                }
            }
            return true;
        case Part::Add:
            StartEditing(nullptr);
            return true;
        case Part::TitleField:
            m_focus = Field::Title;
            return true;
        case Part::Hour:
        case Part::Minute:
            if (hit.value) {
                StepTime(hit.part, (int)hit.value);
            } else {
                m_newAllDay = false;
            }
            m_focus = hit.part == Part::Hour ? Field::Hour : Field::Minute;
            m_digits = 0;
            return true;
        case Part::Cancel:
            StopEditing();
            return true;
        case Part::AllDay:
            m_newAllDay = !m_newAllDay;
            return true;
        case Part::Remind:
            m_newRemind = !m_newRemind;
            return true;
        case Part::Save:
            SaveNewEvent();
            return true;
        case Part::None:
            break;
    }
    return false;
}

// By one, around the clock.
void CalendarPanel::StepTime(Part part, int steps) {
    m_digits = 0;
    if (m_newAllDay) {
        m_newAllDay = false;
        return;
    }
    if (part == Part::Hour) {
        m_newHour = ((m_newHour + steps) % 24 + 24) % 24;
    } else {
        m_newMinute = ((m_newMinute + steps) % 60 + 60) % 60;
    }
}

bool CalendarPanel::OnWheel(int delta) {
    // Over the hour or the minute: changes it.
    if (m_adding &&
        (m_hover.part == Part::Hour || m_hover.part == Part::Minute)) {
        StepTime(m_hover.part, delta > 0 ? 1 : -1);
        return true;
    }
    ChangeMonth(delta > 0 ? -1 : 1);
    return true;
}

bool CalendarPanel::OnKey(WPARAM key) {
    if (m_adding) {
        // Escape leaves the event; the arrows change the hour or the minute.
        if (key == VK_ESCAPE) {
            StopEditing();
            return true;
        }
        if ((key == VK_UP || key == VK_DOWN) && m_focus != Field::Title) {
            StepTime(m_focus == Field::Hour ? Part::Hour : Part::Minute,
                     key == VK_UP ? 1 : -1);
            return true;
        }
        return false;
    }
    if (key == VK_LEFT || key == VK_PRIOR) {
        ChangeMonth(-1);
        return true;
    }
    if (key == VK_RIGHT || key == VK_NEXT) {
        ChangeMonth(1);
        return true;
    }
    return false;
}

// Typing the event's title, or its time; Tab goes to the next field, Enter
// adds it.
bool CalendarPanel::OnChar(WCHAR c) {
    if (!m_adding) {
        return false;
    }
    if (c == L'\t') {
        const bool back = GetKeyState(VK_SHIFT) < 0;
        m_focus = m_focus == Field::Title  ? (back ? Field::Minute : Field::Hour)
                  : m_focus == Field::Hour ? (back ? Field::Title : Field::Minute)
                                           : (back ? Field::Hour : Field::Title);
        m_digits = 0;
        return true;
    }
    if (m_focus != Field::Title && c != L'\r') {
        if (c >= L'0' && c <= L'9') {
            TypeDigit(c - L'0');
        } else if (c == L'\b') {
            // Back to the hour from an empty minute.
            if (m_focus == Field::Minute && m_digits == 0 && m_newMinute == 0) {
                m_focus = Field::Hour;
            } else if (m_focus == Field::Hour) {
                m_newHour /= 10;
            } else {
                m_newMinute /= 10;
            }
            m_digits = 0;
        } else if (c == L':' || c == L'h' || c == L'H') {
            m_focus = Field::Minute;
            m_digits = 0;
        } else {
            return false;
        }
        return true;
    }
    std::wstring& text = m_newTitle;
    if (c == L'\r') {
        SaveNewEvent();
    } else if (c == L'\b') {
        if (!text.empty()) {
            text.pop_back();
        }
    } else if (c == 0x16) {
        if (OpenClipboard(m_hwnd)) {
            if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
                if (auto pasted = (PCWSTR)GlobalLock(data)) {
                    for (PCWSTR p = pasted; *p && text.size() < 120; p++) {
                        if (*p >= 0x20 && *p != L'\t') {
                            text += *p;
                        }
                    }
                    GlobalUnlock(data);
                }
            }
            CloseClipboard();
        }
    } else if (c >= 0x20 && text.size() < 120) {
        text += c;
    } else {
        return false;
    }
    m_statusText.clear();
    Layout();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// The control center, like on macOS: tiles for the network, Bluetooth, "Do
// not disturb" and the settings, the volume as a big slider, and the battery.
// The sound has two more screens, like the Windows quick settings: the output
// device, and the volume of each app.

// Windows' own interface for the default audio device. It isn't documented,
// but it has been stable since Windows 7, and the volume tools use it.
struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64,
                                                          PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR,
                                                       const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR,
                                                       const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId,
                                                         ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

constexpr GUID kCLSID_PolicyConfig = {
    0x870af99c,
    0x171d,
    0x4f9e,
    {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
constexpr GUID kIID_IPolicyConfig = {
    0xf8679f50,
    0x850a,
    0x41cf,
    {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};
constexpr GUID kIID_IAudioSessionManager2 = {
    0x77aa99a0,
    0x1bd6,
    0x484f,
    {0x8b, 0xc7, 0x2c, 0x65, 0x4c, 0x9a, 0x9b, 0x6f}};
constexpr PROPERTYKEY kPKEY_AudioEndpoint_FormFactor = {
    {0x1da5d803,
     0xd492,
     0x4edd,
     {0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e}},
    0};

constexpr WCHAR kGlyphHeadphones = 0xE7F6;
constexpr WCHAR kGlyphSpeakers = 0xE7F5;
constexpr WCHAR kGlyphMonitor = 0xE7F4;
constexpr WCHAR kGlyphCheck = 0xE73E;
constexpr WCHAR kGlyphBack = 0xE72B;

constexpr WCHAR kGlyphLock = 0xE72E;
constexpr WCHAR kGlyphEdit = 0xE70F;
constexpr WCHAR kGlyphClose = 0xE711;
constexpr WCHAR kGlyphBrightness = 0xE706;
constexpr WCHAR kGlyphNightLight = 0xF08C;
constexpr WCHAR kGlyphProject = 0xEBC6;
constexpr WCHAR kGlyphHotspot = 0xE88A;
constexpr WCHAR kGlyphAirplane = 0xE709;
constexpr WCHAR kGlyphMagnifier = 0xE71E;
constexpr WCHAR kGlyphNarrator = 0xEBC5;
constexpr WCHAR kGlyphColorFilters = 0xE790;
constexpr WCHAR kGlyphShow = 0xE7B3;
constexpr WCHAR kGlyphHide = 0xED1A;
constexpr WCHAR kGlyphKeyboard = 0xE765;
constexpr WCHAR kGlyphMouse = 0xE962;

// The controls the control center can show: big tiles (two per row), small
// buttons (four per row) and full-width sections. Which ones, and their
// order, are chosen in it ("Edit").
enum class ControlId {
    Network,
    Bluetooth,
    DoNotDisturb,
    NightLight,
    Hotspot,
    Airplane,
    Project,
    Cast,
    Captions,
    Accessibility,
    DarkMode,
    Screenshot,
    Lock,
    Emoji,
    Clipboard,
    TaskManager,
    Calculator,
    Explorer,
    Display,
    Sound,
    Battery,
};

enum class ControlKind { Tile, Button, Section };

struct ControlInfo {
    ControlId id;
    ControlKind kind;
    WCHAR glyph;
    PCWSTR english;
    PCWSTR portuguese;
    // Remembered by this name.
    PCWSTR key;
    bool shownByDefault;
};

const ControlInfo kControls[] = {
    {ControlId::Network, ControlKind::Tile, 0xE701, L"Wi-Fi / network",
     L"Wi-Fi / rede", L"network", true},
    {ControlId::Bluetooth, ControlKind::Tile, 0xE702, L"Bluetooth",
     L"Bluetooth", L"bluetooth", true},
    {ControlId::DoNotDisturb, ControlKind::Tile, 0xE708, L"Do not disturb",
     L"Não incomodar", L"dnd", true},
    {ControlId::NightLight, ControlKind::Tile, 0xF08C, L"Night light",
     L"Luz noturna", L"nightLight", true},
    {ControlId::Hotspot, ControlKind::Tile, 0xE88A, L"Mobile hotspot",
     L"Hotspot móvel", L"hotspot", false},
    {ControlId::Airplane, ControlKind::Tile, 0xE709, L"Airplane mode",
     L"Modo avião", L"airplane", false},
    {ControlId::Project, ControlKind::Button, 0xEBC6, L"Project", L"Projetar",
     L"project", true},
    {ControlId::Cast, ControlKind::Button, 0xEC15, L"Cast", L"Transmitir",
     L"cast", true},
    {ControlId::Captions, ControlKind::Button, 0xE7F0, L"Captions",
     L"Legendas", L"captions", true},
    {ControlId::Accessibility, ControlKind::Button, 0xE776, L"Accessibility",
     L"Acessibilidade", L"accessibility", true},
    {ControlId::DarkMode, ControlKind::Button, 0xE793, L"Dark mode",
     L"Modo escuro", L"darkMode", false},
    {ControlId::Screenshot, ControlKind::Button, 0xE722, L"Screenshot",
     L"Captura de tela", L"screenshot", false},
    {ControlId::Lock, ControlKind::Button, 0xE72E, L"Lock", L"Bloquear",
     L"lock", false},
    {ControlId::Emoji, ControlKind::Button, 0xE76E, L"Emoji", L"Emojis",
     L"emoji", false},
    {ControlId::Clipboard, ControlKind::Button, 0xF0E3, L"Clipboard",
     L"Área de transferência", L"clipboard", false},
    {ControlId::TaskManager, ControlKind::Button, 0xE9D9, L"Task Manager",
     L"Gerenciador de Tarefas", L"taskManager", false},
    {ControlId::Calculator, ControlKind::Button, 0xE8EF, L"Calculator",
     L"Calculadora", L"calculator", false},
    {ControlId::Explorer, ControlKind::Button, 0xEC50, L"File Explorer",
     L"Explorador de Arquivos", L"explorer", false},
    {ControlId::Display, ControlKind::Section, 0xE706, L"Display brightness",
     L"Brilho da tela", L"display", true},
    {ControlId::Sound, ControlKind::Section, 0xE767, L"Sound", L"Som",
     L"sound", true},
    {ControlId::Battery, ControlKind::Section, 0xE83F, L"Battery",
     L"Bateria", L"battery", true},
};

const ControlInfo& GetControlInfo(ControlId id) {
    for (const auto& info : kControls) {
        if (info.id == id) {
            return info;
        }
    }
    return kControls[0];
}

// At most this many of each, for a control center that fits on the screen.
constexpr int kMaxTiles = 8;
constexpr int kMaxButtons = 8;

// The island's own options, chosen in it (the gear in the control center)
// rather than in Windhawk, and kept with Wh_SetIntValue.
enum class IslandPref {
    Bar,
    Theme,
    Tray,
    TrayOpen,
    Network,
    Volume,
    Battery,
    Bell,
    Media,
    MediaButtons,
    MediaTitle,
    Banner,
    BannerMinimized,
    HideWindowsBanners,
    WebPictures,
    ClockSeconds,
    ClockWeekday,
    ClockDate,
    ClockYear,
    FreeDrag,
    VerticalSides,
};

struct IslandPrefInfo {
    IslandPref pref;
    PCWSTR key;
    WCHAR glyph;
    PCWSTR english;
    PCWSTR portuguese;
};

const IslandPrefInfo kIslandPrefs[] = {
    {IslandPref::Bar, L"islandBar", 0xE7C4, L"Style", L"Estilo"},
    {IslandPref::Theme, L"islandTheme", 0xE790, L"Colors", L"Cores"},
    {IslandPref::Tray, L"islandTray", 0xE712, L"Other apps' tray icons",
     L"Ícones dos outros apps"},
    {IslandPref::TrayOpen, L"trayKeepOpen", 0xE718,
     L"Keep their list open", L"Manter a lista aberta"},
    {IslandPref::Network, L"islandNetwork", 0xE701, L"Network", L"Rede"},
    {IslandPref::Volume, L"islandVolume", 0xE767, L"Volume", L"Volume"},
    {IslandPref::Battery, L"islandBattery", 0xE83F, L"Battery", L"Bateria"},
    {IslandPref::Bell, L"islandBell", 0xEA8F, L"Notifications (bell)",
     L"Notificações (sino)"},
    {IslandPref::Media, L"islandMedia", 0xE8D6, L"What's playing",
     L"O que está tocando"},
    {IslandPref::MediaButtons, L"mediaButtons", 0xE768,
     L"Media buttons on the pill", L"Botões de mídia na pílula"},
    {IslandPref::MediaTitle, L"mediaTitle", 0xE93C,
     L"Cover and title on the pill", L"Capa e título na pílula"},
    {IslandPref::Banner, L"bannerEnabled", 0xE8BD,
     L"New notifications in the pill", L"Avisos na pílula"},
    {IslandPref::BannerMinimized, L"bannerMinimized", 0xE921,
     L"Also with the pill minimized", L"Também com a pílula minimizada"},
    {IslandPref::HideWindowsBanners, L"hideWindowsBanners", 0xED1A,
     L"Hide the Windows banners", L"Esconder os avisos do Windows"},
    {IslandPref::WebPictures, L"bannerWebPictures", 0xE774,
     L"Download pictures from the web (goes online)",
     L"Baixar fotos da internet (acessa a internet)"},
    {IslandPref::ClockSeconds, L"clockSeconds", 0xE916, L"Seconds",
     L"Segundos"},
    {IslandPref::ClockWeekday, L"clockWeekday", 0xE787, L"Day of the week",
     L"Dia da semana"},
    {IslandPref::ClockDate, L"clockDate", 0xE8BF, L"Day and month",
     L"Dia e mês"},
    {IslandPref::ClockYear, L"clockYear", 0xE787, L"Year", L"Ano"},
    {IslandPref::FreeDrag, L"islandFreeDrag", 0xE7C2,
     L"Move the pill by dragging it", L"Mover a pílula arrastando"},
    {IslandPref::VerticalSides, L"islandVerticalSides", 0xE8B4,
     L"Upright on the left and right edges", L"Em pé nas laterais"},
};

// The island's size, in percent: the slider's ends, and the size it's
// restored to.
constexpr int kIslandSizeMin = 70;
constexpr int kIslandSizeMax = 150;
constexpr int kIslandSizeDefault = 100;
constexpr int kIslandSizeStep = 5;

// The settings view's rows, in order: a label (negative), or an option.
constexpr int kPrefLabelItems = -1;
constexpr int kPrefLabelNotifications = -2;
constexpr int kPrefLabelClock = -3;
constexpr int kPrefLabelPlace = -4;
const int kPrefRows[] = {
    kPrefLabelPlace,
    (int)IslandPref::FreeDrag,
    (int)IslandPref::VerticalSides,
    kPrefLabelItems,
    (int)IslandPref::Tray,
    (int)IslandPref::TrayOpen,
    (int)IslandPref::Network,
    (int)IslandPref::Volume,
    (int)IslandPref::Battery,
    (int)IslandPref::Bell,
    (int)IslandPref::Media,
    (int)IslandPref::MediaButtons,
    (int)IslandPref::MediaTitle,
    kPrefLabelClock,
    (int)IslandPref::ClockSeconds,
    (int)IslandPref::ClockWeekday,
    (int)IslandPref::ClockDate,
    (int)IslandPref::ClockYear,
    kPrefLabelNotifications,
    (int)IslandPref::Banner,
    (int)IslandPref::BannerMinimized,
    (int)IslandPref::HideWindowsBanners,
    (int)IslandPref::WebPictures,
};

int GetIslandPref(IslandPref pref);
void SetIslandPref(IslandPref pref, int value);
void SetIslandSize(int percent, bool keep);

// Where the pill sits on its monitor: fractions of the room it can move in
// (x: 0 left, 0.5 middle, 1 right; y: 0 top, 0.5 middle, 1 bottom). One for
// all apps, and others for some apps, used while they're in front.
struct IslandPlace {
    std::wstring appId;
    double x = 0.5;
    double y = 0;
};
IslandPlace g_islandPlace;
std::vector<IslandPlace> g_appPlaces;
constexpr size_t kMaxAppPlaces = 24;

IslandPlace* FindAppPlace(const std::wstring& appId) {
    if (appId.empty()) {
        return nullptr;
    }
    for (auto& place : g_appPlaces) {
        if (_wcsicmp(place.appId.c_str(), appId.c_str()) == 0) {
            return &place;
        }
    }
    return nullptr;
}

void LoadPlaces() {
    g_islandPlace.x = std::clamp(Wh_GetIntValue(L"placeX", 500), 0, 1000) / 1000.0;
    g_islandPlace.y = std::clamp(Wh_GetIntValue(L"placeY", 0), 0, 1000) / 1000.0;
    g_appPlaces.clear();
    std::vector<WCHAR> value(16 * 1024);
    Wh_GetStringValue(L"appPlaces", value.data(), (size_t)value.size());
    // One per line: x, y (thousandths), the app's ID (tab-separated).
    for (PCWSTR line = value.data(); *line;) {
        PCWSTR end = wcschr(line, L'\n');
        const std::wstring text =
            end ? std::wstring(line, end) : std::wstring(line);
        const size_t tab1 = text.find(L'\t');
        const size_t tab2 =
            tab1 == std::wstring::npos ? tab1 : text.find(L'\t', tab1 + 1);
        if (tab2 != std::wstring::npos && tab2 + 1 < text.size() &&
            g_appPlaces.size() < kMaxAppPlaces) {
            IslandPlace place;
            place.x = std::clamp(_wtoi(text.c_str()), 0, 1000) / 1000.0;
            place.y =
                std::clamp(_wtoi(text.c_str() + tab1 + 1), 0, 1000) / 1000.0;
            place.appId = text.substr(tab2 + 1);
            g_appPlaces.push_back(std::move(place));
        }
        if (!end) {
            break;
        }
        line = end + 1;
    }
}

void SavePlaces() {
    Wh_SetIntValue(L"placeX", (int)std::lround(g_islandPlace.x * 1000));
    Wh_SetIntValue(L"placeY", (int)std::lround(g_islandPlace.y * 1000));
    std::wstring value;
    for (const auto& place : g_appPlaces) {
        value += std::to_wstring(std::lround(place.x * 1000)) + L"\t" +
                 std::to_wstring(std::lround(place.y * 1000)) + L"\t" +
                 place.appId + L"\n";
    }
    Wh_SetStringValue(L"appPlaces", value.c_str());
}

// Changed in the settings, or by dragging the pill: kept, and the pill
// slides there (see Island::OnPlacesChanged).
void SetIslandPlace(const std::wstring& appId, double x, double y);

// Windows' dark mode (apps and the system), switched like its settings do:
// the two registry values, then telling the windows.
bool IsDarkMode() {
    DWORD light = 1;
    DWORD size = sizeof(light);
    RegGetValue(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                L"Personalize",
                L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    return light == 0;
}

DWORD WINAPI DarkModeThreadProc(LPVOID parameter) {
    const DWORD light = parameter ? 0 : 1;
    for (PCWSTR name : {L"AppsUseLightTheme", L"SystemUsesLightTheme"}) {
        RegSetKeyValue(HKEY_CURRENT_USER,
                       L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                       L"Personalize",
                       name, REG_DWORD, &light, sizeof(light));
    }
    // Windows that don't answer are skipped.
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                       (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 500,
                       nullptr);
    return 0;
}

constexpr float kHeaderHeight = 40;
constexpr float kRowHeight = 40;
constexpr float kMixerRowHeight = 46;
constexpr size_t kMaxMixerApps = 8;
constexpr size_t kMaxNetworks = 8;
constexpr size_t kMaxBluetoothDevices = 8;

// The control center's messages: the radios' state (wParam Wi-Fi, lParam
// Bluetooth, see RadioThreadProc), and Wi-Fi events (wParam the event, lParam
// the reason of a failed connection).
constexpr UINT WM_APP_RADIOS = WM_APP + 60;
constexpr UINT WM_APP_WLAN = WM_APP + 61;
// The mobile hotspot's and airplane mode's state (wParam and lParam, like a
// radio's).
constexpr UINT WM_APP_HOTSPOT = WM_APP + 62;
// The monitors' brightness (wParam, 0 to 100, or -1).
constexpr UINT WM_APP_BRIGHTNESS = WM_APP + 63;
// The screenshot shortcut, once the control center is gone.
constexpr UINT WM_APP_SCREENSHOT = WM_APP + 64;
constexpr UINT_PTR kScreenshotTimerId = kPanelScreenshotTimerId;

// A radio's state for the control center.
constexpr int kRadioUnknown = -1;
constexpr int kRadioMissing = -2;

// Turns the Wi-Fi or Bluetooth radio on or off, like the quick settings do
// (Windows.Devices.Radios), then reports both radios' state to `notify`. The
// calls wait, so they run on a thread of their own.
struct RadioRequest {
    HWND notify;
    // 0: Wi-Fi, 1: Bluetooth, 2: the mobile hotspot, 3: airplane mode, -1:
    // only report.
    int kind;
    bool on;
};

// Airplane mode: the interface the Windows quick settings use (the radio
// management service). It isn't documented, but it has been the same since
// Windows 8 and other tools use it; if it's missing, the switch isn't shown.
struct IRadioManager : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsRMSupported(DWORD* state) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUIRadioInstances(
        IUnknown** instances) = 0;
    // `enabled`: the radios are on (airplane mode is off).
    virtual HRESULT STDMETHODCALLTYPE GetSystemRadioState(int* enabled,
                                                          int* unknown,
                                                          int* reason) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSystemRadioState(int enabled) = 0;
};

constexpr GUID kCLSID_RadioManagementAPI = {
    0x581333F6,
    0x28DB,
    0x41BE,
    {0xBC, 0x7A, 0xFF, 0x20, 0x1F, 0x12, 0xF3, 0xF6}};
constexpr GUID kIID_IRadioManager = {
    0xDB3AFBFB,
    0x08E6,
    0x46C6,
    {0xAA, 0x70, 0xBF, 0x9A, 0x34, 0xC3, 0x0A, 0xB7}};

DWORD WINAPI RadioThreadProc(LPVOID parameter) {
    std::unique_ptr<RadioRequest> request((RadioRequest*)parameter);
    using namespace winrt::Windows::Devices::Radios;
    int states[2] = {kRadioMissing, kRadioMissing};
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    // Airplane mode first: it switches the radios.
    int airplane = kRadioMissing;
    winrt::com_ptr<IRadioManager> radioManager;
    if (SUCCEEDED(CoCreateInstance(kCLSID_RadioManagementAPI, nullptr,
                                   CLSCTX_ALL, kIID_IRadioManager,
                                   radioManager.put_void()))) {
        if (request->kind == 3) {
            const HRESULT result =
                radioManager->SetSystemRadioState(request->on ? 0 : 1);
            if (FAILED(result)) {
                Wh_Log(L"Airplane mode: %08X", (unsigned)result);
            }
        }
        int enabled = 1;
        int unknown = 0;
        int reason = 0;
        if (SUCCEEDED(radioManager->GetSystemRadioState(&enabled, &unknown,
                                                        &reason))) {
            airplane = enabled ? 0 : 1;
        }
        radioManager = nullptr;
    }

    try {
        if (Radio::RequestAccessAsync().get() == RadioAccessStatus::Allowed) {
            for (const auto& radio : Radio::GetRadiosAsync().get()) {
                const int index = radio.Kind() == RadioKind::WiFi ? 0
                                  : radio.Kind() == RadioKind::Bluetooth
                                      ? 1
                                      : -1;
                if (index < 0) {
                    continue;
                }
                if (index == request->kind) {
                    radio
                        .SetStateAsync(request->on ? RadioState::On
                                                   : RadioState::Off)
                        .get();
                }
                states[index] = std::max(
                    states[index], radio.State() == RadioState::On ? 1 : 0);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Radios: %08X %s", (unsigned)e.code(), e.message().c_str());
    }
    PostMessage(request->notify, WM_APP_RADIOS, (WPARAM)(INT_PTR)states[0],
                (LPARAM)(INT_PTR)states[1]);

    // The mobile hotspot shares the internet connection over Wi-Fi.
    int hotspot = kRadioMissing;
    try {
        using namespace winrt::Windows::Networking;
        auto profile =
            Connectivity::NetworkInformation::GetInternetConnectionProfile();
        using NetworkOperators::NetworkOperatorTetheringManager;
        if (profile &&
            NetworkOperatorTetheringManager::
                    GetTetheringCapabilityFromConnectionProfile(profile) ==
                NetworkOperators::TetheringCapability::Enabled) {
            auto manager =
                NetworkOperatorTetheringManager::CreateFromConnectionProfile(
                    profile);
            if (request->kind == 2) {
                auto result = (request->on ? manager.StartTetheringAsync()
                                           : manager.StopTetheringAsync())
                                  .get();
                if (result.Status() !=
                    NetworkOperators::TetheringOperationStatus::Success) {
                    Wh_Log(L"Hotspot: status %d", (int)result.Status());
                }
            }
            hotspot = manager.TetheringOperationalState() ==
                              NetworkOperators::TetheringOperationalState::On
                          ? 1
                          : 0;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Hotspot: %08X %s", (unsigned)e.code(), e.message().c_str());
    }
    PostMessage(request->notify, WM_APP_HOTSPOT, (WPARAM)(INT_PTR)hotspot,
                (LPARAM)(INT_PTR)airplane);
    winrt::uninit_apartment();
    return 0;
}

// Brightness: reads it (BrightnessQueryProc), or sets the latest level asked
// for until no newer one comes (BrightnessThreadProc), so dragging the slider
// doesn't queue up slow calls.
struct BrightnessJob {
    HWND notify = nullptr;
    std::atomic<int> target{-1};
    std::atomic<bool> running{false};
};

DWORD WINAPI BrightnessQueryProc(LPVOID parameter) {
    auto* job = (BrightnessJob*)parameter;
    PostMessage(job->notify, WM_APP_BRIGHTNESS,
                (WPARAM)(INT_PTR)ReadBrightness(), 0);
    return 0;
}

DWORD WINAPI BrightnessThreadProc(LPVOID parameter) {
    auto* job = (BrightnessJob*)parameter;
    int applied = -1;
    for (;;) {
        const int level = job->target;
        if (level != applied) {
            WriteBrightness(level);
            applied = level;
            continue;
        }
        job->running = false;
        // A level asked for meanwhile starts it again.
        if (job->target != applied && !job->running.exchange(true)) {
            continue;
        }
        break;
    }
    return 0;
}

// Wi-Fi events, on a system thread: passed on to the control center.
void WINAPI WlanNotificationCallback(PWLAN_NOTIFICATION_DATA data,
                                     PVOID context) {
    if (!data || data->NotificationSource != WLAN_NOTIFICATION_SOURCE_ACM) {
        return;
    }
    DWORD reason = 0;
    if ((data->NotificationCode == wlan_notification_acm_connection_complete ||
         data->NotificationCode ==
             wlan_notification_acm_connection_attempt_fail) &&
        data->pData &&
        data->dwDataSize >= sizeof(WLAN_CONNECTION_NOTIFICATION_DATA)) {
        reason =
            ((PWLAN_CONNECTION_NOTIFICATION_DATA)data->pData)->wlanReasonCode;
    }
    PostMessage((HWND)context, WM_APP_WLAN, data->NotificationCode, reason);
}

std::wstring EscapeXml(const std::wstring& text) {
    std::wstring escaped;
    for (WCHAR c : text) {
        switch (c) {
            case L'&':
                escaped += L"&amp;";
                break;
            case L'<':
                escaped += L"&lt;";
                break;
            case L'>':
                escaped += L"&gt;";
                break;
            case L'"':
                escaped += L"&quot;";
                break;
            case L'\'':
                escaped += L"&apos;";
                break;
            default:
                escaped += c;
        }
    }
    return escaped;
}

std::wstring SsidName(const DOT11_SSID& ssid) {
    const int length = (int)std::min<ULONG>(ssid.uSSIDLength, 32);
    WCHAR name[64] = L"";
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                             (LPCCH)ssid.ucSSID, length, name,
                             ARRAYSIZE(name) - 1)) {
        MultiByteToWideChar(CP_ACP, 0, (LPCCH)ssid.ucSSID, length, name,
                            ARRAYSIZE(name) - 1);
    }
    return name;
}

// A program's name, as its file describes it ("Google Chrome"), or its file
// name.
std::wstring GetProgramName(PCWSTR path) {
    std::wstring name;
    DWORD handle = 0;
    const DWORD size = GetFileVersionInfoSize(path, &handle);
    if (size) {
        std::vector<BYTE> data(size);
        struct Translation {
            WORD language;
            WORD codePage;
        }* translations = nullptr;
        UINT length = 0;
        if (GetFileVersionInfo(path, 0, size, data.data()) &&
            VerQueryValue(data.data(), L"\\VarFileInfo\\Translation",
                          (void**)&translations, &length) &&
            length >= sizeof(Translation)) {
            WCHAR query[64];
            swprintf_s(query, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                       translations[0].language, translations[0].codePage);
            PCWSTR description = nullptr;
            if (VerQueryValue(data.data(), query, (void**)&description,
                              &length) &&
                description && *description) {
                name = description;
            }
        }
    }
    if (name.empty()) {
        PCWSTR file = wcsrchr(path, L'\\');
        name = file ? file + 1 : path;
        if (name.size() > 4 &&
            _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0) {
            name.resize(name.size() - 4);
        }
    }
    return name;
}

class ControlPanel : public Panel {
   public:
    ~ControlPanel() {
        ClearApps();
        StopWifi();
        WaitForWorkers();
    }

    // Called while open, to show changes made elsewhere.
    void Refresh() {
        if (IsOpen() && !m_dragging && m_dragId < 0) {
            Update();
            Layout();
            Render();
        }
    }

   protected:
    void Prepare(float scale) override;
    void OnOpen() override;
    void Draw(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush) override;
    bool OnMouseMove(POINT pt) override;
    bool OnMouseLeave() override;
    bool OnMouseDown(POINT pt) override;
    bool OnMouseUp(POINT pt) override;
    bool OnWheel(int delta) override;
    bool OnKey(WPARAM key) override;
    bool OnChar(WCHAR c) override;
    bool OnAppMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
    void OnDestroy() override;

   private:
    enum class View {
        Main,
        Output,
        Mixer,
        Wifi,
        Password,
        Bluetooth,
        Project,
        Accessibility,
        // The island's own options (the gear).
        Settings,
        // A place for some apps (from the island's settings).
        AppPlaces,
    };

    enum class Part {
        None,
        Network,
        Bluetooth,
        DoNotDisturb,
        NightLight,
        VolumeIcon,
        Slider,
        Device,
        MixerButton,
        Battery,
        More,
        Back,
        Output,      // A device in the output list (index).
        AppIcon,     // An app's icon in the mixer: mutes it (index).
        AppSlider,   // An app's volume in the mixer (index).
        WifiToggle,  // Wi-Fi on/off: the tile's icon, or the list's switch.
        BluetoothToggle,
        WifiRow,       // A Wi-Fi network (index).
        Disconnect,    // "Disconnect" on the connected network (index).
        BluetoothRow,  // A Bluetooth device (index).
        PasswordField,
        ShowPassword,
        Connect,
        Shortcut,          // Project, cast, captions, accessibility (index).
        BrightnessSlider,
        HotspotToggle,
        AirplaneToggle,
        DndToggle,
        ProjectRow,        // A display mode (index).
        AccessibilityRow,  // An accessibility feature (index).
        EditButton,        // "Edit" at the bottom.
        SettingsButton,    // The gear at the bottom.
        EditControl,       // A control, while editing (its ControlId).
        EditRemove,        // Its "-" (its ControlId).
        EditAdd,           // A control to add (its ControlId).
        EditDone,          // "Done".
        SettingToggle,     // An island option's switch (its IslandPref).
        StyleSegment,      // The pill (0) or the bar (1).
        ThemeSwatch,       // A theme (its index).
        PlaceDot,          // A ready place for all apps (0-8).
        AppPlacesRow,      // "Place per app".
        AppPlaceDot,       // An app's place (entry * 9 + place).
        AppPlaceRemove,    // An app's "x" (entry).
        AppPlaceAdd,       // An open app to give a place (its index).
        SizeSlider,        // The island's size.
        SizeReset,         // Back to its usual size.
    };

    struct Hit {
        Part part = Part::None;
        int index = -1;
        bool operator==(const Hit& other) const {
            return part == other.part && index == other.index;
        }
    };

    struct OutputDevice {
        std::wstring id;
        std::wstring name;
        WCHAR glyph;
        bool isDefault;
    };

    // The sessions of one program playing sound.
    struct MixerApp {
        DWORD processId = 0;
        bool system = false;
        std::wstring name;
        float volume = 1;
        bool muted = false;
        std::vector<winrt::com_ptr<ISimpleAudioVolume>> volumes;
        HICON icon = nullptr;
        winrt::com_ptr<ID2D1Bitmap> bitmap;
    };

    // A Wi-Fi network in reach.
    struct WifiNetwork {
        std::wstring name;
        DOT11_SSID ssid{};
        int signal = 0;
        bool secure = false;
        bool connected = false;
        // Its saved profile, if any.
        std::wstring profile;
        DOT11_AUTH_ALGORITHM auth = DOT11_AUTH_ALGO_80211_OPEN;
        DOT11_CIPHER_ALGORITHM cipher = DOT11_CIPHER_ALGO_NONE;
    };

    struct BluetoothDevice {
        std::wstring name;
        bool connected;
        WCHAR glyph;
    };

    // An accessibility feature with an on/off switch.
    enum class Feature { Magnifier, Narrator, ColorFilters, StickyKeys };
    struct AccessibilityItem {
        Feature feature;
        WCHAR glyph;
        PCWSTR name;
        bool on;
    };

    void Update();
    void UpdateOutputs();
    void UpdateApps();
    void ClearApps();
    void StartWifi();
    void StopWifi();
    void UpdateWifi();
    void UpdateBluetooth();
    bool ConnectWifi(const WifiNetwork& network, const std::wstring* key);
    void JoinWifi(int index);
    void SubmitPassword();
    void SwitchRadio(int kind, bool on);
    void StartWorker(LPTHREAD_START_ROUTINE proc, void* parameter);
    void WaitForWorkers();
    void UpdateDisplayMode();
    void UpdateAccessibility();
    void ToggleAccessibility(int index);
    void SetBrightnessAt(POINT pt);
    void SetBrightness(int level);
    std::wstring StatusText() const;
    void Layout();
    void SetView(View view);
    Hit HitTest(POINT pt) const;
    void SetVolumeAt(POINT pt);
    void SetAppVolumeAt(int index, POINT pt);
    void SetAppVolume(int index, float volume);
    void DrawTile(ID2D1RenderTarget* target,
                  ID2D1SolidColorBrush* brush,
                  const D2D1_RECT_F& rect,
                  WCHAR glyph,
                  bool active,
                  PCWSTR title,
                  const std::wstring& subtitle,
                  bool hover);
    void DrawSlider(ID2D1RenderTarget* target,
                    ID2D1SolidColorBrush* brush,
                    const D2D1_RECT_F& rect,
                    float level,
                    WCHAR glyph,
                    bool hoverIcon);
    void DrawHeader(ID2D1RenderTarget* target,
                    ID2D1SolidColorBrush* brush,
                    PCWSTR title,
                    int toggle = kRadioMissing);
    // `on` from 0 (off) to 1 (on), animated (see KnobFor).
    void DrawToggle(ID2D1RenderTarget* target,
                    ID2D1SolidColorBrush* brush,
                    const D2D1_RECT_F& rect,
                    float on);
    // A switch's knob, sliding to where it's now (a bit springy).
    float KnobFor(int key, bool on);
    void DrawStatus(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawWifi(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawPassword(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawBluetooth(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawProject(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawSettings(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    // One control of the main view, at its place.
    void DrawControl(ID2D1RenderTarget* target,
                     ID2D1SolidColorBrush* brush,
                     ControlId id,
                     const D2D1_RECT_F& rect);
    // Editing: each control drawn once into a picture, which then only
    // moves (sliding to its place, dragged); drawing them all again on every
    // frame costs a lot. While editing they don't change.
    struct ControlTile {
        ControlId id;
        D2D1_SIZE_F size;
        winrt::com_ptr<ID2D1Bitmap> bitmap;
    };
    std::vector<ControlTile> m_tiles;
    ID2D1Bitmap* GetControlTile(ID2D1RenderTarget* target,
                                ID2D1SolidColorBrush* brush,
                                ControlId id,
                                const D2D1_RECT_F& rect,
                                float margin);
    // Editing: the "-" at a control's corner.
    void DrawRemoveBadge(ID2D1RenderTarget* target,
                         ID2D1SolidColorBrush* brush,
                         const D2D1_RECT_F& rect,
                         float amount);
    void SetEditing(bool editing);
    // An option that depends on another one being on.
    static bool PrefDisabled(IslandPref pref) {
        return (pref == IslandPref::TrayOpen &&
                !GetIslandPref(IslandPref::Tray)) ||
               ((pref == IslandPref::BannerMinimized ||
                 pref == IslandPref::HideWindowsBanners ||
                 pref == IslandPref::WebPictures) &&
                !GetIslandPref(IslandPref::Banner));
    }
    void DrawShortcut(ID2D1RenderTarget* target,
                      ID2D1SolidColorBrush* brush,
                      const D2D1_RECT_F& rect,
                      WCHAR glyph,
                      PCWSTR name,
                      bool active,
                      bool hover);
    // Text that scrolls back and forth when it doesn't fit (see
    // AnimateContents).
    void DrawMarquee(ID2D1RenderTarget* target,
                     ID2D1SolidColorBrush* brush,
                     const std::wstring& text,
                     IDWriteTextFormat* format,
                     const D2D1_RECT_F& rect,
                     bool centered = false);
    bool AnimateContents(double dt) override;
    // The controls shown, in order, and remembering them.
    void LoadControls();
    void SaveControls();
    void DrawAccessibility(ID2D1RenderTarget* target,
                           ID2D1SolidColorBrush* brush);
    void DrawRowToggle(ID2D1RenderTarget* target,
                       ID2D1SolidColorBrush* brush,
                       const D2D1_RECT_F& row,
                       WCHAR glyph,
                       PCWSTR name,
                       bool on,
                       bool hover);
    void DrawMain(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawView(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawOutputs(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    void DrawMixer(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    ID2D1Bitmap* GetAppBitmap(ID2D1RenderTarget* target, MixerApp& app);

    winrt::com_ptr<IDWriteTextFormat> m_iconFormat;
    winrt::com_ptr<IDWriteTextFormat> m_titleFormat;
    winrt::com_ptr<IDWriteTextFormat> m_subtitleFormat;
    winrt::com_ptr<IDWriteTextFormat> m_sectionFormat;
    winrt::com_ptr<IDWriteTextFormat> m_percentFormat;
    winrt::com_ptr<IDWriteTextFormat> m_linkFormat;
    winrt::com_ptr<IDWriteTextFormat> m_smallFormat;
    winrt::com_ptr<IWICImagingFactory> m_wic;
    float m_preparedScale = 0;

    View m_view = View::Main;
    // Changing views: the new one slides in and fades in (0 to 1).
    double m_viewIn = 1;
    double m_viewInVelocity = 0;
    int m_viewDirection = 1;

    // State shown.
    NetworkInfo m_network;
    float m_volume = 0;
    bool m_muted = false;
    bool m_hasVolume = false;
    std::wstring m_device;
    bool m_doNotDisturb = false;
    bool m_hasBattery = false;
    int m_batteryPercent = 0;
    bool m_charging = false;
    std::vector<OutputDevice> m_outputs;
    std::vector<MixerApp> m_apps;

    // Wi-Fi, with a WLAN handle of its own that's told of scans and
    // connections.
    HANDLE m_wlan = nullptr;
    GUID m_wifiInterface{};
    bool m_hasWifi = false;
    std::vector<WifiNetwork> m_networks;
    // Joining a network with a password.
    WifiNetwork m_joining;
    std::wstring m_password;
    bool m_showPassword = false;
    // A profile made to join, removed if joining fails.
    std::wstring m_createdProfile;
    bool m_connecting = false;
    std::wstring m_wifiStatus;
    // The radios: on (1), off (0), kRadioUnknown or kRadioMissing.
    int m_wifiRadio = kRadioUnknown;
    int m_bluetoothRadio = kRadioUnknown;
    std::vector<BluetoothDevice> m_bluetoothDevices;
    // The radio and brightness threads (see StartWorker).
    std::vector<HANDLE> m_workers;
    // The mobile hotspot and airplane mode: like a radio.
    int m_hotspot = kRadioUnknown;
    int m_airplane = kRadioUnknown;
    // Night light: 1 on, 0 off, -1 unavailable.
    int m_nightLight = -1;
    // The monitors' brightness, 0 to 100, or -1 (not known or not
    // adjustable).
    int m_brightness = -1;
    BrightnessJob m_brightnessJob;
    ULONGLONG m_brightnessReadAt = 0;
    // The display mode (DISPLAYCONFIG_TOPOLOGY_*).
    int m_displayMode = 0;
    std::vector<AccessibilityItem> m_accessibility;

    // Layout.
    D2D1_RECT_F m_networkTile{}, m_bluetoothTile{}, m_dndTile{},
        m_sound{}, m_volumeIcon{}, m_slider{}, m_deviceRow{},
        m_mixerButton{}, m_batteryTile{}, m_more{}, m_back{}, m_toggle{},
        m_networkIcon{}, m_bluetoothIcon{}, m_field{}, m_eye{}, m_connect{},
        m_status{}, m_nightTile{}, m_display{}, m_brightnessSlider{},
        m_hotspotRow{}, m_airplaneRow{}, m_dndIcon{};
    // Where each control shown is.
    std::vector<std::pair<ControlId, D2D1_RECT_F>> m_controlRects;
    std::vector<ControlId> m_controls;
    bool m_controlsLoaded = false;
    D2D1_RECT_F m_edit{}, m_gear{}, m_done{}, m_addHeader{};
    // Editing in place, like the iPhone's control center: the controls
    // each have a "-"; they're dragged to another place, and the
    // others (listed below) are added with a click. m_editAmount (0 to 1)
    // fades the "-" in and out.
    bool m_editing = false;
    double m_editAmount = 0;
    double m_editAmountVelocity = 0;
    std::vector<std::pair<ControlId, D2D1_RECT_F>> m_addRects;
    std::wstring m_editStatus;
    // Where each control is shown: it follows its place with a spring, so
    // the others slide out of the way, and an added one grows in.
    struct Motion {
        ControlId id;
        double x;
        double y;
        double vx = 0;
        double vy = 0;
        double scale = 1;
        double vscale = 0;
    };
    std::vector<Motion> m_motions;
    Motion* FindMotion(ControlId id);
    // A removed control shrinking away.
    struct Ghost {
        ControlId id;
        D2D1_RECT_F rect;
        double amount = 1;
        double velocity = 0;
    };
    std::vector<Ghost> m_ghosts;
    // A control being dragged: where the mouse took it, and where it is.
    int m_dragId = -1;
    bool m_dragMoved = false;
    POINT m_dragStart{};
    POINT m_dragPoint{};
    D2D1_POINT_2F m_dragGrab{};
    winrt::com_ptr<IDWriteTextFormat> m_badgeFormat;
    // The island's settings view.
    D2D1_RECT_F m_styleRow{}, m_themeRow{};
    // The place for all apps (a little screen with its 9 ready places), and
    // the places per app.
    D2D1_RECT_F m_placeRow{}, m_placeGrid{}, m_appPlacesRow{};
    // The island's size: its row, slider and reset button.
    D2D1_RECT_F m_sizeRow{}, m_sizeSlider{}, m_sizeReset{};
    void SetIslandSizeAt(POINT pt, bool keep);
    std::vector<D2D1_RECT_F> m_appPlaceRows;
    std::vector<D2D1_RECT_F> m_appPlaceGrids;
    std::vector<D2D1_RECT_F> m_appPlaceRemoves;
    D2D1_RECT_F m_addAppsHeader{};
    std::vector<std::wstring> m_openApps;
    std::vector<D2D1_RECT_F> m_openAppChips;
    void ListOpenApps();
    // The ready place under a point of a grid (0-8), or -1.
    static int PlaceDotAt(const D2D1_RECT_F& grid, POINT pt);
    void DrawPlaceGrid(ID2D1RenderTarget* target,
                       ID2D1SolidColorBrush* brush,
                       const D2D1_RECT_F& grid,
                       double x,
                       double y,
                       int hover,
                       float opacity);
    void DrawAppPlaces(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush);
    D2D1_RECT_F m_styleSegments[2]{};
    D2D1_RECT_F m_swatches[(int)ThemeId::Count]{};
    std::vector<std::pair<int, D2D1_RECT_F>> m_prefRows;
    double m_styleKnob = -1;
    double m_styleKnobVelocity = 0;
    // The switches' knobs, by key.
    struct Knob {
        int key;
        double value;
        double velocity;
        double target;
    };
    std::vector<Knob> m_knobs;
    // Scrolling texts: since when, and whether one scrolls now.
    double m_marqueeStart = 0;
    bool m_marqueeActive = false;
    // Their layouts, made once instead of on every frame.
    struct MarqueeLayout {
        std::wstring text;
        IDWriteTextFormat* format;
        float fontSize;
        float height;
        winrt::com_ptr<IDWriteTextLayout> layout;
        float width;
    };
    std::vector<MarqueeLayout> m_marqueeLayouts;
    bool m_darkMode = false;
    std::vector<D2D1_RECT_F> m_rows;
    std::vector<D2D1_RECT_F> m_rowIcons;
    std::vector<D2D1_RECT_F> m_rowSliders;
    Hit m_hover;
    // Dragging the main slider (index -1), an app's (its index), or the
    // brightness.
    static constexpr int kDragBrightness = -2;
    static constexpr int kDragIslandSize = -3;
    bool m_dragging = false;
    int m_dragIndex = -1;
};

////////////////////////////////////////////////////////////////////////////////
// The notifications, like the macOS notification center: the latest ones
// Windows showed, newest first, read (read-only) from its notification
// database, as cards with the app's icon and name, the time, the title and the
// text. Clicking one opens the app; the wheel scrolls. Clearing them is left to
// the Windows notification center (a link at the bottom), since the database
// isn't written to.

class NotificationPanel : public Panel {
   public:
    // Called while open, to show new notifications.
    void Refresh() {
        if (IsOpen()) {
            Load();
            Layout();
            Render();
        }
    }

   protected:
    void Prepare(float scale) override;
    void OnOpen() override;
    void Draw(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush) override;
    bool OnMouseMove(POINT pt) override;
    bool OnMouseLeave() override;
    bool OnMouseUp(POINT pt) override;
    bool OnWheel(int delta) override;
    void OnDestroy() override { m_sqlite.Unload(); }
    bool AnimateContents(double dt) override;
    bool OnChar(WCHAR c) override;
    bool OnKey(WPARAM key) override;

   private:
    struct Notification {
        long long id;
        std::wstring appId;
        std::wstring title;
        std::wstring text;
        long long arrival;
        ToastReply reply;
    };

    enum class Part {
        None,
        Card,
        Dismiss,
        ClearAll,
        DndToggle,
        ContentToggle,
        BannerToggle,
        MinimizedToggle,
        ReplyButton,  // "Reply" on a card (index).
        ReplyField,   // The box being typed in (index).
        ReplySend,    // Its send button (index).
        Link
    };
    struct Hit {
        Part part = Part::None;
        int index = -1;
        bool operator==(const Hit& other) const {
            return part == other.part && index == other.index;
        }
    };

    void Load();
    void LoadCleared();
    void SaveCleared();
    void Layout();
    Hit HitTest(POINT pt) const;
    const std::wstring& AppName(const std::wstring& appId);
    ID2D1Bitmap* AppBitmap(ID2D1RenderTarget* target,
                           const std::wstring& appId);
    std::wstring TimeText(long long arrival) const;

    Sqlite m_sqlite;
    bool m_sqliteLoaded = false;
    std::vector<Notification> m_notifications;
    // Cleared here (the Windows notification database is only read): all
    // that arrived up to `m_clearedUntil`, and the ones dismissed one by one.
    long long m_clearedUntil = 0;
    std::vector<long long> m_dismissed;
    bool m_clearedLoaded = false;
    bool m_doNotDisturb = false;
    int m_first = 0;
    Hit m_hover;
    std::vector<D2D1_RECT_F> m_cards;
    D2D1_RECT_F m_header{}, m_clearAll{}, m_dndRow{}, m_dndToggle{},
        m_contentRow{}, m_contentToggle{}, m_link{};
    // The switches' knobs, sliding (0 off, 1 on).
    double m_dndKnob = -1;
    double m_dndKnobVelocity = 0;
    double m_contentKnob = -1;
    double m_contentKnobVelocity = 0;
    // "In the pill" and "With the pill minimized".
    D2D1_RECT_F m_bannerRow{}, m_bannerToggle{}, m_minimizedRow{},
        m_minimizedToggle{};
    double m_bannerKnob = -1;
    double m_bannerKnobVelocity = 0;
    double m_minimizedKnob = -1;
    double m_minimizedKnobVelocity = 0;
    // Cards moving: dragged sideways, sliding away (then their room closing),
    // or back.
    struct CardMotion {
        long long id;
        double offset = 0;
        double velocity = 0;
        double target = 0;
        double room = 1;
        double roomVelocity = 0;
        // Before it starts sliding ("Clear all" sends them one after
        // another).
        double delay = 0;
        bool removing = false;
    };
    std::vector<CardMotion> m_motions;
    CardMotion* Motion(long long id, bool create);
    void Remove(long long id, int direction, double delay = 0);
    // Dragging a card: which, from where, and whether it moved yet.
    long long m_dragId = -1;
    int m_dragStartX = 0;
    bool m_dragMoved = false;
    bool m_clearAllPending = false;
    bool OnMouseDown(POINT pt) override;
    // The notification being answered (by its ID), and what's typed.
    long long m_replyId = -1;
    std::wstring m_replyText;
    void SendReply();
    // A card's "Reply" button, and the reply box with its send button.
    void CardParts(const D2D1_RECT_F& card,
                   D2D1_RECT_F* replyButton,
                   D2D1_RECT_F* field,
                   D2D1_RECT_F* send) const;
    void DrawSwitchRow(ID2D1RenderTarget* target,
                       ID2D1SolidColorBrush* brush,
                       const D2D1_RECT_F& row,
                       const D2D1_RECT_F& toggle,
                       WCHAR glyph,
                       const D2D1_COLOR_F& glyphColor,
                       PCWSTR name,
                       PCWSTR description,
                       double knob,
                       bool hover,
                       bool enabled = true);
    std::vector<std::pair<std::wstring, std::wstring>> m_names;
    std::vector<std::pair<std::wstring, winrt::com_ptr<ID2D1Bitmap>>>
        m_bitmaps;
    winrt::com_ptr<IWICImagingFactory> m_wic;
    winrt::com_ptr<IDWriteTextFormat> m_headerFormat;
    winrt::com_ptr<IDWriteTextFormat> m_appFormat;
    winrt::com_ptr<IDWriteTextFormat> m_timeFormat;
    winrt::com_ptr<IDWriteTextFormat> m_titleFormat;
    winrt::com_ptr<IDWriteTextFormat> m_textFormat;
    winrt::com_ptr<IDWriteTextFormat> m_linkFormat;
    winrt::com_ptr<IDWriteTextFormat> m_iconFormat;
    float m_preparedScale = 0;
};

////////////////////////////////////////////////////////////////////////////////
// What's playing (music, videos), from the media controls Windows shows for
// apps (Windows.Media.Control, documented). A thread of its own is told when
// the playing app, the song or its state changes, reads it, and hands it to
// the island; the island's buttons are sent back to it.

struct MediaState {
    bool present = false;
    std::wstring appId;
    std::wstring title;
    std::wstring artist;
    bool playing = false;
    bool canPrevious = false;
    bool canNext = false;
    bool canPlayPause = false;
    bool canSeek = false;
    // In seconds; the position as of `updatedAt` (NowSeconds()).
    double position = 0;
    double duration = 0;
    double updatedAt = 0;
    // The cover (an image file's bytes), and a number that changes with it.
    std::vector<BYTE> thumbnail;
    UINT thumbnailSerial = 0;

    // Where it is now, moving while it plays.
    double Now() const {
        const double now =
            position + (playing ? NowSeconds() - updatedAt : 0);
        return duration > 0 ? std::clamp(now, 0.0, duration) : now;
    }
};

enum class MediaCommand { PlayPause, Previous, Next, Seek };

// The media buttons on the pill (the setting, switched in the media panel).
std::atomic<bool> g_mediaButtonsInPill{false};
// What's playing on the pill: its cover and its title, instead of the note.
std::atomic<bool> g_mediaTitleInPill{false};

SRWLOCK g_mediaLock = SRWLOCK_INIT;
MediaState g_media;
std::vector<std::pair<MediaCommand, double>> g_mediaCommands;
HANDLE g_mediaThread;
HANDLE g_mediaStopEvent;
HANDLE g_mediaChangedEvent;
HANDLE g_mediaCommandEvent;
// Tells the island it changed.
constexpr UINT WM_APP_MEDIA = WM_APP + 54;

void SendMediaCommand(MediaCommand command, double position = 0) {
    AcquireSRWLockExclusive(&g_mediaLock);
    g_mediaCommands.push_back({command, position});
    ReleaseSRWLockExclusive(&g_mediaLock);
    if (g_mediaCommandEvent) {
        SetEvent(g_mediaCommandEvent);
    }
}

DWORD WINAPI MediaThreadProc(LPVOID) {
    using namespace winrt::Windows::Media::Control;
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    {
        GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
        try {
            manager =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync()
                    .get();
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Media controls: %08X", (unsigned)e.code());
        }
        if (manager) {
            auto changed = [](auto&&...) { SetEvent(g_mediaChangedEvent); };
            auto sessionRevoker =
                manager.CurrentSessionChanged(winrt::auto_revoke, changed);
            auto sessionsRevoker =
                manager.SessionsChanged(winrt::auto_revoke, changed);
            GlobalSystemMediaTransportControlsSession session{nullptr};
            GlobalSystemMediaTransportControlsSession::
                MediaPropertiesChanged_revoker propertiesRevoker;
            GlobalSystemMediaTransportControlsSession::
                PlaybackInfoChanged_revoker playbackRevoker;
            GlobalSystemMediaTransportControlsSession::
                TimelinePropertiesChanged_revoker timelineRevoker;
            std::wstring lastTitle;
            double titleChangedAt = 0;
            UINT thumbnailSerial = 0;
            SetEvent(g_mediaChangedEvent);

            const HANDLE events[] = {g_mediaStopEvent, g_mediaChangedEvent,
                                     g_mediaCommandEvent};
            for (;;) {
                const DWORD wait = WaitForMultipleObjects(
                    ARRAYSIZE(events), events, FALSE, INFINITE);
                if (wait == WAIT_OBJECT_0) {
                    break;
                }
                try {
                    if (wait == WAIT_OBJECT_0 + 2) {
                        // The island's buttons.
                        AcquireSRWLockExclusive(&g_mediaLock);
                        auto commands = std::move(g_mediaCommands);
                        g_mediaCommands.clear();
                        ReleaseSRWLockExclusive(&g_mediaLock);
                        for (const auto& [command, position] : commands) {
                            if (!session) {
                                break;
                            }
                            switch (command) {
                                case MediaCommand::PlayPause:
                                    session.TryTogglePlayPauseAsync().get();
                                    break;
                                case MediaCommand::Previous:
                                    session.TrySkipPreviousAsync().get();
                                    break;
                                case MediaCommand::Next:
                                    session.TrySkipNextAsync().get();
                                    break;
                                case MediaCommand::Seek:
                                    session
                                        .TryChangePlaybackPositionAsync(
                                            (int64_t)(position * 10000000))
                                        .get();
                                    break;
                            }
                        }
                        continue;
                    }

                    // The playing app, its song and its state.
                    auto current = manager.GetCurrentSession();
                    if (current != session) {
                        session = current;
                        propertiesRevoker = {};
                        playbackRevoker = {};
                        timelineRevoker = {};
                        if (session) {
                            propertiesRevoker = session.MediaPropertiesChanged(
                                winrt::auto_revoke, changed);
                            playbackRevoker = session.PlaybackInfoChanged(
                                winrt::auto_revoke, changed);
                            timelineRevoker = session.TimelinePropertiesChanged(
                                winrt::auto_revoke, changed);
                        }
                    }
                    MediaState state;
                    if (session) {
                        state.present = true;
                        state.appId = session.SourceAppUserModelId();
                        // Any of these may be missing (a browser changing
                        // pages, or going full screen).
                        auto properties =
                            session.TryGetMediaPropertiesAsync().get();
                        if (properties) {
                            state.title = properties.Title();
                            state.artist = properties.Artist();
                        }
                        if (auto playback = session.GetPlaybackInfo()) {
                            state.playing =
                                playback.PlaybackStatus() ==
                                GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                    Playing;
                            if (auto controls = playback.Controls()) {
                                state.canPrevious = controls.IsPreviousEnabled();
                                state.canNext = controls.IsNextEnabled();
                                state.canPlayPause =
                                    controls.IsPlayPauseToggleEnabled() ||
                                    controls.IsPauseEnabled() ||
                                    controls.IsPlayEnabled();
                                state.canSeek =
                                    controls.IsPlaybackPositionEnabled();
                            }
                        }
                        using seconds = std::chrono::duration<double>;
                        auto timeline = session.GetTimelineProperties();
                        if (timeline) {
                            state.duration =
                                std::chrono::duration_cast<seconds>(
                                    timeline.EndTime() - timeline.StartTime())
                                    .count();
                            state.position =
                                std::chrono::duration_cast<seconds>(
                                    timeline.Position() - timeline.StartTime())
                                    .count();
                        }
                        // Since the app last said where it was.
                        if (state.playing && timeline) {
                            const double since =
                                std::chrono::duration_cast<seconds>(
                                    winrt::clock::now() -
                                    timeline.LastUpdatedTime())
                                    .count();
                            if (since > 0 && since < 3600) {
                                state.position += since;
                            }
                        }
                        state.updatedAt = NowSeconds();
                        // The cover, read again when the song changes. Some
                        // apps (like Spotify) give the new title first and
                        // the new cover a moment later: it's read again for
                        // a few seconds after a change, and while missing.
                        const std::wstring key =
                            state.appId + L"|" + state.title + L"|" +
                            state.artist;
                        if (key != lastTitle) {
                            lastTitle = key;
                            titleChangedAt = NowSeconds();
                        }
                        AcquireSRWLockShared(&g_mediaLock);
                        state.thumbnail = g_media.thumbnail;
                        ReleaseSRWLockShared(&g_mediaLock);
                        if (NowSeconds() - titleChangedAt < 6 ||
                            state.thumbnail.empty()) {
                            std::vector<BYTE> cover;
                            auto thumbnail =
                                properties ? properties.Thumbnail() : nullptr;
                            auto stream =
                                thumbnail ? thumbnail.OpenReadAsync().get()
                                          : nullptr;
                            if (stream) {
                                const uint32_t size =
                                    (uint32_t)std::min<uint64_t>(stream.Size(),
                                                                 8 << 20);
                                winrt::Windows::Storage::Streams::DataReader
                                    reader(stream);
                                reader.LoadAsync(size).get();
                                cover.resize(size);
                                reader.ReadBytes(cover);
                            }
                            if (cover != state.thumbnail) {
                                state.thumbnail = std::move(cover);
                                thumbnailSerial++;
                            }
                        }
                        state.thumbnailSerial = thumbnailSerial;
                    }
                    AcquireSRWLockExclusive(&g_mediaLock);
                    g_media = std::move(state);
                    ReleaseSRWLockExclusive(&g_mediaLock);
                    if (HWND island = g_islandWnd) {
                        PostMessage(island, WM_APP_MEDIA, 0, 0);
                    }
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"Media: %08X", (unsigned)e.code());
                }
            }
        }
    }
    winrt::uninit_apartment();
    return 0;
}

void StartMediaWatch() {
    if (g_mediaThread) {
        return;
    }
    g_mediaStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_mediaChangedEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_mediaCommandEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (g_mediaStopEvent && g_mediaChangedEvent && g_mediaCommandEvent) {
        g_mediaThread =
            CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    }
}

void StopMediaWatch() {
    if (g_mediaThread) {
        SetEvent(g_mediaStopEvent);
        WaitForSingleObject(g_mediaThread, INFINITE);
        CloseHandle(g_mediaThread);
        g_mediaThread = nullptr;
    }
    for (HANDLE* event :
         {&g_mediaStopEvent, &g_mediaChangedEvent, &g_mediaCommandEvent}) {
        if (*event) {
            CloseHandle(*event);
            *event = nullptr;
        }
    }
    AcquireSRWLockExclusive(&g_mediaLock);
    g_media = {};
    g_mediaCommands.clear();
    ReleaseSRWLockExclusive(&g_mediaLock);
}

std::wstring AppDisplayName(const std::wstring& appId);

// "3:07", or "1:02:45".
std::wstring FormatMediaTime(double seconds) {
    const int total = (int)std::max(0.0, seconds);
    WCHAR text[32];
    if (total >= 3600) {
        swprintf_s(text, L"%d:%02d:%02d", total / 3600, total / 60 % 60,
                   total % 60);
    } else {
        swprintf_s(text, L"%d:%02d", total / 60, total % 60);
    }
    return text;
}

////////////////////////////////////////////////////////////////////////////////
// The media controls, under the island: the cover, the song, a timeline to
// jump in it, previous / play / next, and the volume (the wheel too).

class MediaPanel : public Panel {
   public:
    void SetState(const MediaState& state) {
        // Play turning into pause (and back): it shrinks and grows again.
        if (m_state.present && state.present &&
            m_state.playing != state.playing) {
            m_playSwap = 1;
            m_playSwapVelocity = 0;
            StartIslandAnimation();
        }
        m_state = state;
        if (IsOpen()) {
            Layout();
            Render();
        }
    }
    // Called every second while open, for the timeline.
    void Refresh() {
        if (IsOpen() && m_state.playing) {
            Render();
        }
    }

   protected:
    void Prepare(float scale) override;
    void OnOpen() override;
    void Draw(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush) override;
    bool OnMouseMove(POINT pt) override;
    bool OnMouseLeave() override;
    bool OnMouseDown(POINT pt) override;
    bool OnMouseUp(POINT pt) override;
    bool OnWheel(int delta) override;
    bool AnimateContents(double dt) override;

   private:
    enum class Part {
        None,
        Previous,
        PlayPause,
        Next,
        Timeline,
        Volume,
        PillButtons,
        PillTitle,
    };
    // A button pressed shrinks, and springs back when released.
    PressAnimation m_press;
    double m_playSwap = 0;
    double m_playSwapVelocity = 0;
    // The switches' knobs, sliding (-1 until first drawn).
    double m_pillKnob = -1;
    double m_pillKnobVelocity = 0;
    double m_titleKnob = -1;
    double m_titleKnobVelocity = 0;
    // The timeline and the volume grow thicker under the mouse.
    double m_timelineHover = 0;
    double m_timelineHoverVelocity = 0;
    double m_volumeHover = 0;
    double m_volumeHoverVelocity = 0;

    void Layout();
    Part HitTest(POINT pt) const;
    ID2D1Bitmap* Thumbnail(ID2D1RenderTarget* target);
    void ReadVolume();
    void SetVolume(float level);
    double PositionAt(POINT pt) const;

    MediaState m_state;
    winrt::com_ptr<IWICImagingFactory> m_wic;
    winrt::com_ptr<ID2D1Bitmap> m_thumbnail;
    UINT m_thumbnailSerial = (UINT)-1;
    winrt::com_ptr<IDWriteTextFormat> m_titleFormat;
    winrt::com_ptr<IDWriteTextFormat> m_textFormat;
    winrt::com_ptr<IDWriteTextFormat> m_timeFormat;
    winrt::com_ptr<IDWriteTextFormat> m_timeRightFormat;
    winrt::com_ptr<IDWriteTextFormat> m_iconFormat;
    winrt::com_ptr<IDWriteTextFormat> m_bigIconFormat;
    float m_preparedScale = 0;
    D2D1_RECT_F m_cover{}, m_timeline{}, m_previous{}, m_playPause{}, m_next{},
        m_volume{}, m_volumeIcon{}, m_pillButtons{}, m_pillTitle{};
    Part m_hover = Part::None;
    // Dragging the timeline (to a position) or the volume.
    Part m_dragging = Part::None;
    double m_dragPosition = 0;
    float m_level = 0;
    bool m_muted = false;
};

// The Windows banner and the hook that follows it (see BannerWinEventProc).
extern HWND g_windowsBanner;
void CALLBACK BannerWinEventProc(HWINEVENTHOOK,
                                 DWORD,
                                 HWND,
                                 LONG,
                                 LONG,
                                 DWORD,
                                 DWORD);

class StatusEvents;

class Island {
   public:
    bool Create();
    void Destroy();
    void Tick();
    bool Animating() const { return m_animating; }
    void AnimationFrame();
    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void StartAnimating();

    // Shared with the control center.
    IMMDeviceEnumerator* Devices() { return m_devices.get(); }
    winrt::com_ptr<IAudioEndpointVolume> GetVolume(std::wstring* deviceName);
    NetworkInfo QueryNetwork();
    void OnVolumeChanged();
    void OnNetworkChanged();
    void OnDoNotDisturbChanged();
    void OpenNotifications();
    void OnPreferencesChanged();
    // Tells explorer the island is there (it sends what it has).
    void SayHello() {
        SendIpc(FindTaskbarWnd(), m_hwnd, kIpcHello, nullptr, 0);
    }
    // Another place (or one changed): it slides there.
    void OnPlacesChanged();
    // The app in front changed (its place may be another).
    void OnForegroundChanged();
    float TaskbarRoom() const;
    // On the lower half of the screen: panels and banners open upwards.
    bool AtBottom() const { return !m_bar && !m_vertical && m_placeY > 0.5; }
    // At the very top, where it can be minimized to a line.
    bool AtTop() const { return m_bar || m_placeY < 0.001; }
    // Lays it out again (an item shown or hidden), animated.
    void Relayout() {
        StartAnimating();
        Render();
    }

   private:
    void UpdateClock();
    void UpdateStatus();
    void UpdateApps();
    void ScheduleTick();
    std::wstring NetworkGlyph();
    std::wstring VolumeGlyph();
    std::wstring BatteryGlyph();
    void UpdateFormats(float scale);
    bool IsCovered() const;
    // The screen's scaling (m_scale also has the island's size in it).
    float m_dpiScale = 1;
    void Layout();
    void Render(bool force = false);
    void SetAppBarPosition();
    IslandItem HitTest(POINT pt, std::wstring* appId = nullptr) const;
    void OnClick(IslandItem item, const std::wstring& appId);
    ID2D1Bitmap* GetAppBitmap(const std::wstring& appId);
    void TogglePanel(Panel& panel, IslandItem item);
    void SetMinimized(bool minimized);

   public:
    void CapturePill(Panel& panel);

   private:
    void ShowBanner();
    void HideBanner();
    void ShowReminder(const CalendarEvent& event);
    // The day of the reminder shown (its banner opens the calendar there).
    struct {
        int year = 0;
        int month = 0;
        int day = 0;
    } m_reminderDay;
    D2D1_RECT_F BannerRect() const;
    // The reply box (with the send button at its end) and "Open".
    void BannerParts(D2D1_RECT_F* field,
                     D2D1_RECT_F* send,
                     D2D1_RECT_F* open) const;
    // The "x" at its top right, under the mouse.
    D2D1_RECT_F BannerCloseRect() const;
    bool BannerHovered() const {
        return m_hover == IslandItem::Banner ||
               m_hover == IslandItem::BannerReply ||
               m_hover == IslandItem::BannerOpen ||
               m_hover == IslandItem::BannerClose;
    }
    void StartBannerTyping();
    void EndBannerTyping();
    void SendBannerReply();
    bool OnBannerChar(WCHAR c);
    void UpdateTrayIcons();
    const TrayIcon* FindTrayIcon(const std::wstring& key) const;
    ID2D1Bitmap* GetTrayBitmap(const TrayIcon& icon);
    bool IsTrayPinned(const std::wstring& key) const;
    void SetTrayPinned(const std::wstring& key, bool pinned);
    void SendToTrayIcon(const std::wstring& key, UINT message);
    void ShowTrayTip(const std::wstring& key);
    void WatchTrayAppWindows(const TrayIcon& icon);
    void StopWatchingTrayAppWindows();

   public:
    void OnWindowsBanner(HWND banner);
    void WatchWindowsBannerMoves(DWORD processId);
    void OnTrayAppWindow(HWND hWnd);

   private:
    D2D1_RECT_F TrayPinRect(const D2D1_RECT_F& slot) const;
    void OnMouseButton(UINT msg, LPARAM lParam);

    HWND m_hwnd = nullptr;
    bool m_bar = false;
    winrt::com_ptr<ID2D1Factory> m_d2d;
    winrt::com_ptr<IDWriteFactory> m_dwrite;
    LayeredCanvas m_canvas;
    winrt::com_ptr<IDWriteTextFormat> m_iconFormat;
    winrt::com_ptr<IDWriteTextFormat> m_smallIconFormat;
    winrt::com_ptr<IDWriteTextFormat> m_textFormat;
    // Figures of equal width ("1" as wide as "0"), for the clock.
    winrt::com_ptr<IDWriteTypography> m_tabular;
    winrt::com_ptr<IMMDeviceEnumerator> m_devices;
    winrt::com_ptr<INetworkListManager> m_network;
    HANDLE m_wlan = nullptr;
    CalendarPanel m_calendar;
    ControlPanel m_control;
    NotificationPanel m_notifications;
    MediaPanel m_mediaPanel;
    // What's playing, from the media thread.
    MediaState m_mediaState;
    // What the pill shows: it grows in and out (m_mediaAmount from 0 to 1, a
    // bit more while bouncing), and stays a moment after the media starts or
    // stops, so it doesn't flash. Kept as it last was while it goes away.
    MediaState m_mediaShown;
    bool m_mediaWanted = false;
    double m_mediaSince = 0;
    double m_mediaGoneAt = 0;
    double m_mediaAmount = 0;
    double m_mediaVelocity = 0;
    // Its buttons on the pill, growing in and out the same way.
    double m_mediaButtonsAmount = 0;
    double m_mediaButtonsVelocity = 0;
    // A button pressed on the pill shrinks, and springs back when released;
    // play turning into pause (and back) shrinks and grows again.
    PressAnimation m_press;
    // How much room they take: grows without bouncing (the icons bounce on
    // their own), so the pill widens smoothly.
    double m_mediaRoom = 0;
    double m_mediaRoomVelocity = 0;
    double m_mediaButtonsRoom = 0;
    double m_mediaButtonsRoomVelocity = 0;
    // The cover and the title instead of the note (0 to 1, animated), the
    // cover's bitmap, and the title's width, measured once per title.
    double m_mediaTitleRoom = 0;
    double m_mediaTitleRoomVelocity = 0;
    winrt::com_ptr<ID2D1Bitmap> m_mediaCover;
    UINT m_mediaCoverSerial = (UINT)-1;
    std::wstring m_mediaTitleMeasured;
    // The title as shown (with its artist, when short).
    std::wstring m_mediaTitleShown;
    float m_mediaTitleWidth = 0;
    ID2D1Bitmap* MediaCover();
    std::wstring MediaTitleText() const;
    double m_playSwap = 0;
    double m_playSwapVelocity = 0;
    void UpdateMediaShown();
    bool m_appBar = false;

    float m_scale = 0;
    RECT m_monitor{};
    POINT m_position{};
    SIZE m_size{};
    // The expanded pill's rect, in the window.
    D2D1_RECT_F m_pill{};
    std::vector<IslandSlot> m_slots;
    IslandItem m_hover = IslandItem::None;
    // The app under the mouse, when m_hover is App.
    std::wstring m_hoverApp;
    bool m_tracking = false;
    bool m_visible = false;
    // Where it is (fractions of the room it moves in, see IslandPlace),
    // springing to its place, and the room in screen pixels (from Layout).
    double m_placeX = 0.5;
    double m_placeY = 0;
    double m_placeVelocityX = 0;
    double m_placeVelocityY = 0;
    float m_roomLeft = 0;
    float m_roomTop = 0;
    float m_roomWidth = 0;
    float m_roomHeight = 0;
    // Upright on the left or right edge (its items one under the other; its
    // panels and banners open towards the screen's middle).
    bool m_vertical = false;
    bool m_sideLeft = true;
    // The clock's two lines when upright: the time and a short date.
    std::wstring m_clockTime;
    std::wstring m_clockDate;
    // The app in front whose place is used (empty: the one for all apps).
    std::wstring m_placeApp;
    HWINEVENTHOOK m_foregroundHook = nullptr;
    const IslandPlace& CurrentPlace() const {
        static const IslandPlace kTop{L"", 0.5, 0};
        if (m_bar) {
            return kTop;
        }
        if (const IslandPlace* place = FindAppPlace(m_placeApp)) {
            return *place;
        }
        return g_islandPlace;
    }
    // Dragging it: pressed (it may become a drag), moved (it is one), where
    // the mouse holds the pill.
    bool m_dragArmed = false;
    bool m_dragMoved = false;
    POINT m_dragFrom{};
    POINT m_dragGrab{};
    void FinishDrag();
    // Faded away (over a full-screen app): kept off the screen rather than
    // hidden, since Windows' own animation for showing a window drew it as a
    // plain rectangle.
    bool m_away = true;
    bool m_shownOnce = false;
    // Showing and hiding (over full-screen apps): a dot fades in at the
    // pill's middle and opens into it; hiding, it closes back into the dot,
    // which fades away. 0 hidden, 1 shown (a bit more while bouncing).
    double m_appear = 0;
    double m_appearVelocity = 0;
    // The dot's opacity, and how far it has opened, from m_appear.
    float AppearFade() const {
        return (float)std::clamp(m_appear / 0.22, 0.0, 1.0);
    }
    float AppearOpen() const {
        return (float)std::clamp((m_appear - 0.18) / 0.82, 0.0, 1.12);
    }
    int m_ticks = 0;

    // Apps with a notification badge (from the taskbar), and their icons.
    // Each grows in and out (`shown` from 0 to 1).
    std::vector<std::wstring> m_badgeApps;
    struct AppEntry {
        std::wstring id;
        double shown = 0;
        double velocity = 0;
        bool present = true;
    };
    std::vector<AppEntry> m_appEntries;
    // The other apps' tray icons (from the taskbar), the ones pinned to show
    // always, and the others, shown when the list is open (m_trayAmount from
    // 0 to 1, animated).
    std::vector<TrayIcon> m_trayIcons;
    std::vector<std::wstring> m_trayPinned;
    // How pinned each icon is shown (0 in the list, 1 pinned), animated.
    struct PinState {
        std::wstring key;
        double amount;
        double velocity;
    };
    std::vector<PinState> m_pinStates;
    float PinAmount(const std::wstring& key) const;
    // The icon whose pin shows (see kPinDelayMs), and since when.
    std::wstring m_pinShownKey;
    double m_pinShownAt = 0;
    float PinShown(double now) const;
    bool m_trayOpen = false;
    // Kept open (the setting, or the arrow's right click).
    bool m_trayKeepOpen = false;
    double m_trayAmount = 0;
    double m_trayVelocity = 0;
    double m_trayBounce = 0;
    double m_trayBounceVelocity = 0;
    // The arrow turning from › to ‹ (0 to 1).
    double m_trayArrow = 0;
    double m_trayArrowVelocity = 0;
    // The tooltip waits a moment, like the taskbar's.
    std::wstring m_pendingTipKey;
    // An app's window opened from its tray icon, moved under the island.
    HWINEVENTHOOK m_trayAppHooks[2]{};
    POINT m_trayAppAnchor{-1, -1};
    // The Windows banner, moved away while the pill shows it.
    HWINEVENTHOOK m_bannerHook = nullptr;
    HWINEVENTHOOK m_bannerMoveHook = nullptr;
    void PublishTrayPlaces();
    // What was last sent to explorer (see PublishTrayPlaces).
    std::vector<BYTE> m_sentTrayPlaces;
    std::vector<std::tuple<std::wstring, UINT, winrt::com_ptr<ID2D1Bitmap>>>
        m_trayBitmaps;
    HWND m_tooltip = nullptr;
    std::wstring m_tooltipKey;
    // A new notification shown in the pill: it stretches into a banner
    // (m_bannerAmount from 0 to 1, a bit more while bouncing), stays a few
    // seconds (longer under the mouse), and goes back.
    IslandToast m_banner;
    bool m_bannerWanted = false;
    // When the pill last took a notification, when the Windows banner was
    // hidden ahead of it and where it was, and when it was put back (see
    // OnWindowsBanner).
    double m_bannerTakenAt = -100;
    double m_bannerHiddenAt = -100;
    double m_bannerRestoredAt = -100;
    LONG m_bannerHomeTop = 0;
    bool CanTakeBanners() const;
    void RestoreWindowsBanner();
    // The reply row: 0 hidden, 1 shown (under the mouse, or while typing).
    double m_bannerOpen = 0;
    double m_bannerOpenVelocity = 0;
    bool m_bannerTyping = false;
    std::wstring m_bannerReplyText;
    double m_bannerAmount = 0;
    double m_bannerVelocity = 0;
    winrt::com_ptr<IDWriteTextFormat> m_bannerAppFormat;
    winrt::com_ptr<IDWriteTextFormat> m_bannerTimeFormat;
    winrt::com_ptr<IDWriteTextFormat> m_bannerTitleFormat;
    winrt::com_ptr<IDWriteTextFormat> m_bannerTextFormat;
    // An app that just got a badge while the pill is minimized peeks out.
    std::wstring m_peekApp;
    double m_peekStart = 0;
    float PeekAmount(double now);
    std::vector<std::pair<std::wstring, winrt::com_ptr<ID2D1Bitmap>>>
        m_appBitmaps;
    winrt::com_ptr<IWICImagingFactory> m_wic;
    // The banner's picture (see IslandToast::image), loaded for it.
    ID2D1Bitmap* GetBannerImage();
    std::wstring m_bannerImagePath;
    winrt::com_ptr<ID2D1Bitmap> m_bannerImage;

    // Minimizing (pill only): 1 expanded, 0 a thin line, animated.
    bool m_minimized = false;
    // The line growing under the mouse (0 to 1).
    double m_lineHover = 0;
    double m_lineHoverVelocity = 0;
    double m_expand = 1;
    double m_expandVelocity = 0;
    bool m_animating = false;
    double m_lastFrame = 0;

    // Told of changes (see StartStatusWatch).
    void StartStatusWatch();
    void StopStatusWatch();
    void WatchVolume();
    StatusEvents* m_statusEvents = nullptr;
    winrt::com_ptr<IAudioEndpointVolume> m_watchedVolume;
    DWORD m_networkCookie = 0;
    winrt::com_ptr<IConnectionPoint> m_networkPoint;
    bool m_wlanWatched = false;
    HPOWERNOTIFY m_powerNotifications[2]{};
    void* m_dndSubscription = nullptr;
    int m_raiseTicks = 0;

    // What's shown; a new frame is drawn only when it changes.
    std::wstring m_clock;
    std::wstring m_networkGlyph;
    std::wstring m_volumeGlyph;
    std::wstring m_batteryGlyph;
    std::wstring m_bellGlyph;
    std::wstring m_drawnKey;
};

LRESULT CALLBACK IslandWndProc(HWND hWnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
    if (g_island) {
        return g_island->HandleMessage(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// The control center.

void ControlPanel::Prepare(float scale) {
    if (!m_wic) {
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(m_wic.put()));
    }
    Update();
    Layout();
    if (scale == m_preparedScale) {
        return;
    }
    m_preparedScale = scale;
    m_iconFormat = MakeFormat(L"Segoe Fluent Icons", DWRITE_FONT_WEIGHT_NORMAL,
                              15 * scale);
    m_titleFormat = MakeFormat(L"Segoe UI Variable Text",
                               DWRITE_FONT_WEIGHT_SEMI_BOLD, 13 * scale,
                               DWRITE_TEXT_ALIGNMENT_LEADING);
    m_subtitleFormat =
        MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                   11.5f * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
    m_sectionFormat = MakeFormat(L"Segoe UI Variable Text",
                                 DWRITE_FONT_WEIGHT_SEMI_BOLD, 13 * scale,
                                 DWRITE_TEXT_ALIGNMENT_LEADING);
    m_percentFormat =
        MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                   12 * scale, DWRITE_TEXT_ALIGNMENT_TRAILING);
    m_linkFormat = MakeFormat(L"Segoe UI Variable Text",
                              DWRITE_FONT_WEIGHT_NORMAL, 12 * scale);
    m_smallFormat = MakeFormat(L"Segoe UI Variable Text",
                               DWRITE_FONT_WEIGHT_NORMAL, 10.5f * scale);
    m_badgeFormat = MakeFormat(L"Segoe Fluent Icons", DWRITE_FONT_WEIGHT_NORMAL,
                               9 * scale);
}

void ControlPanel::OnOpen() {
    m_view = View::Main;
    m_hover = {};
    m_dragging = false;
    m_editing = false;
    m_editAmount = 0;
    m_editAmountVelocity = 0;
    m_dragId = -1;
    m_motions.clear();
    m_ghosts.clear();
    m_styleKnob = -1;
    m_viewIn = 1;
    m_viewInVelocity = 0;
    Layout();
    m_wifiStatus.clear();
    m_marqueeStart = NowSeconds();
    m_darkMode = IsDarkMode();
    StartWifi();
    // The radios' and the brightness's state come a moment later.
    SwitchRadio(-1, false);
    // The monitor is asked once, and again only after a while: asking over
    // DDC/CI can make some monitors flicker.
    m_brightnessJob.notify = m_hwnd;
    const ULONGLONG now = GetTickCount64();
    if (!m_brightnessJob.running &&
        (!m_brightnessReadAt || now - m_brightnessReadAt > 10 * 60 * 1000)) {
        m_brightnessReadAt = now;
        StartWorker(BrightnessQueryProc, &m_brightnessJob);
    }
}

void ControlPanel::OnDestroy() {
    StopWifi();
}

void ControlPanel::StartWifi() {
    if (m_wlan || !m_hwnd) {
        return;
    }
    DWORD version;
    if (WlanOpenHandle(2, nullptr, &version, &m_wlan) != ERROR_SUCCESS) {
        m_wlan = nullptr;
        return;
    }
    PWLAN_INTERFACE_INFO_LIST interfaces = nullptr;
    if (WlanEnumInterfaces(m_wlan, nullptr, &interfaces) == ERROR_SUCCESS) {
        if (interfaces->dwNumberOfItems > 0) {
            m_wifiInterface = interfaces->InterfaceInfo[0].InterfaceGuid;
            m_hasWifi = true;
        }
        WlanFreeMemory(interfaces);
    }
    WlanRegisterNotification(m_wlan, WLAN_NOTIFICATION_SOURCE_ACM, TRUE,
                             WlanNotificationCallback, m_hwnd, nullptr,
                             nullptr);
}

void ControlPanel::StopWifi() {
    if (!m_wlan) {
        return;
    }
    WlanRegisterNotification(m_wlan, WLAN_NOTIFICATION_SOURCE_NONE, TRUE,
                             nullptr, nullptr, nullptr, nullptr);
    WlanCloseHandle(m_wlan, nullptr);
    m_wlan = nullptr;
    m_hasWifi = false;
}

// The networks in reach, one per name: the connected one first, then by
// signal.
void ControlPanel::UpdateWifi() {
    m_networks.clear();
    PWLAN_AVAILABLE_NETWORK_LIST list = nullptr;
    if (!m_wlan || !m_hasWifi ||
        WlanGetAvailableNetworkList(m_wlan, &m_wifiInterface, 0, nullptr,
                                    &list) != ERROR_SUCCESS) {
        return;
    }
    for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
        const WLAN_AVAILABLE_NETWORK& available = list->Network[i];
        // Hidden networks have no name to show.
        if (available.dot11Ssid.uSSIDLength == 0 ||
            available.dot11BssType != dot11_BSS_type_infrastructure) {
            continue;
        }
        WifiNetwork network;
        network.ssid = available.dot11Ssid;
        network.name = SsidName(available.dot11Ssid);
        network.signal = (int)available.wlanSignalQuality;
        network.secure = available.bSecurityEnabled;
        network.connected =
            (available.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) != 0;
        if (available.dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE) {
            network.profile = available.strProfileName;
        }
        network.auth = available.dot11DefaultAuthAlgorithm;
        network.cipher = available.dot11DefaultCipherAlgorithm;

        auto same = std::find_if(
            m_networks.begin(), m_networks.end(), [&](const WifiNetwork& n) {
                return n.ssid.uSSIDLength == network.ssid.uSSIDLength &&
                       memcmp(n.ssid.ucSSID, network.ssid.ucSSID,
                              network.ssid.uSSIDLength) == 0;
            });
        if (same == m_networks.end()) {
            m_networks.push_back(std::move(network));
            continue;
        }
        same->signal = std::max(same->signal, network.signal);
        same->connected = same->connected || network.connected;
        if (same->profile.empty()) {
            same->profile = network.profile;
        }
    }
    WlanFreeMemory(list);
    std::stable_sort(m_networks.begin(), m_networks.end(),
                     [](const WifiNetwork& a, const WifiNetwork& b) {
                         if (a.connected != b.connected) {
                             return a.connected;
                         }
                         return a.signal > b.signal;
                     });
    if (m_networks.size() > kMaxNetworks) {
        m_networks.resize(kMaxNetworks);
    }
}

// The paired devices, the connected ones first. Only what Windows already
// knows: no search for new devices.
void ControlPanel::UpdateBluetooth() {
    m_bluetoothDevices.clear();
    if (m_bluetoothRadio != 1) {
        return;
    }
    BLUETOOTH_DEVICE_SEARCH_PARAMS params{sizeof(params)};
    params.fReturnAuthenticated = TRUE;
    params.fReturnRemembered = TRUE;
    params.fReturnConnected = TRUE;
    BLUETOOTH_DEVICE_INFO info{sizeof(info)};
    HBLUETOOTH_DEVICE_FIND find = BluetoothFindFirstDevice(&params, &info);
    if (!find) {
        return;
    }
    do {
        WCHAR glyph = kGlyphBluetooth;
        const ULONG major = GET_COD_MAJOR(info.ulClassofDevice);
        if (major == COD_MAJOR_AUDIO) {
            glyph = kGlyphHeadphones;
        } else if (major == COD_MAJOR_PERIPHERAL) {
            glyph = (GET_COD_MINOR(info.ulClassofDevice) & 0x20) ? kGlyphMouse
                                                                 : kGlyphKeyboard;
        }
        m_bluetoothDevices.push_back(
            {info.szName, info.fConnected != FALSE, glyph});
        info.dwSize = sizeof(info);
    } while (BluetoothFindNextDevice(find, &info));
    BluetoothFindDeviceClose(find);
    std::stable_sort(m_bluetoothDevices.begin(), m_bluetoothDevices.end(),
                     [](const BluetoothDevice& a, const BluetoothDevice& b) {
                         return a.connected && !b.connected;
                     });
    if (m_bluetoothDevices.size() > kMaxBluetoothDevices) {
        m_bluetoothDevices.resize(kMaxBluetoothDevices);
    }
}

// Joins a network: with its saved profile, or with a new one (open, or with
// the password `key`). WPA/WPA2/WPA3 personal only; the others need the
// Windows network list.
bool ControlPanel::ConnectWifi(const WifiNetwork& network,
                               const std::wstring* key) {
    if (!m_wlan || !m_hasWifi) {
        return false;
    }
    std::wstring profile = network.profile;
    if (profile.empty() || key) {
        PCWSTR authentication;
        switch (network.auth) {
            case DOT11_AUTH_ALGO_80211_OPEN:
                authentication = L"open";
                break;
            case DOT11_AUTH_ALGO_WPA_PSK:
                authentication = L"WPAPSK";
                break;
            case DOT11_AUTH_ALGO_RSNA_PSK:
                authentication = L"WPA2PSK";
                break;
            case DOT11_AUTH_ALGO_WPA3_SAE:
                authentication = L"WPA3SAE";
                break;
            default:
                return false;
        }
        const bool open = network.auth == DOT11_AUTH_ALGO_80211_OPEN;
        if (!open && !key) {
            return false;
        }
        PCWSTR encryption = open ? L"none"
                            : network.cipher == DOT11_CIPHER_ALGO_TKIP
                                ? L"TKIP"
                                : L"AES";
        std::wstring hex;
        for (ULONG i = 0; i < network.ssid.uSSIDLength && i < 32; i++) {
            WCHAR byte[3];
            swprintf_s(byte, L"%02X", network.ssid.ucSSID[i]);
            hex += byte;
        }
        const std::wstring name = EscapeXml(network.name);
        std::wstring xml =
            L"<?xml version=\"1.0\"?>"
            L"<WLANProfile "
            L"xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">"
            L"<name>" +
            name +
            L"</name><SSIDConfig><SSID><hex>" + hex +
            L"</hex><name>" + name +
            L"</name></SSID></SSIDConfig>"
            L"<connectionType>ESS</connectionType>"
            L"<connectionMode>auto</connectionMode>"
            L"<MSM><security><authEncryption><authentication>" +
            authentication + L"</authentication><encryption>" + encryption +
            L"</encryption><useOneX>false</useOneX></authEncryption>";
        if (!open) {
            xml += L"<sharedKey><keyType>passPhrase</keyType>"
                   L"<protected>false</protected><keyMaterial>" +
                   EscapeXml(*key) + L"</keyMaterial></sharedKey>";
        }
        xml += L"</security></MSM></WLANProfile>";
        DWORD reason = 0;
        const DWORD result =
            WlanSetProfile(m_wlan, &m_wifiInterface, 0, xml.c_str(), nullptr,
                           TRUE, nullptr, &reason);
        if (result != ERROR_SUCCESS) {
            Wh_Log(L"WlanSetProfile failed: %u, reason %u", result, reason);
            return false;
        }
        profile = network.name;
        if (network.profile.empty()) {
            m_createdProfile = profile;
        }
    }
    WLAN_CONNECTION_PARAMETERS parameters{};
    parameters.wlanConnectionMode = wlan_connection_mode_profile;
    parameters.strProfile = profile.c_str();
    parameters.dot11BssType = dot11_BSS_type_infrastructure;
    const DWORD result =
        WlanConnect(m_wlan, &m_wifiInterface, &parameters, nullptr);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"WlanConnect failed: %u", result);
        return false;
    }
    m_connecting = true;
    m_wifiStatus = Tr(L"Connecting...", L"Conectando...");
    return true;
}

// A network clicked in the list: joins it, asking for its password if needed.
void ControlPanel::JoinWifi(int index) {
    if (index < 0 || index >= (int)m_networks.size()) {
        return;
    }
    const WifiNetwork network = m_networks[index];
    if (network.connected) {
        return;
    }
    m_createdProfile.clear();
    if (!network.profile.empty() || !network.secure) {
        if (!ConnectWifi(network, nullptr)) {
            m_wifiStatus = Tr(L"Couldn't connect.",
                              L"Não foi possível conectar.");
        }
        return;
    }
    switch (network.auth) {
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_RSNA_PSK:
        case DOT11_AUTH_ALGO_WPA3_SAE:
            m_joining = network;
            m_password.clear();
            m_showPassword = false;
            m_wifiStatus.clear();
            SetView(View::Password);
            break;
        default:
            // Company networks (a user name and a certificate): the Windows
            // network list asks for them.
            Close();
            OpenUri(L"ms-availablenetworks:");
            break;
    }
}

void ControlPanel::SubmitPassword() {
    if (m_connecting) {
        return;
    }
    if (m_password.size() < 8) {
        m_wifiStatus = Tr(L"The password has at least 8 characters.",
                          L"A senha tem pelo menos 8 caracteres.");
        return;
    }
    m_createdProfile.clear();
    if (!ConnectWifi(m_joining, &m_password)) {
        m_wifiStatus =
            Tr(L"Couldn't connect.", L"Não foi possível conectar.");
    }
}

// kind: 0 Wi-Fi, 1 Bluetooth, -1 only to know their state.
void ControlPanel::SwitchRadio(int kind, bool on) {
    // Forgets the threads that are done.
    std::erase_if(m_workers, [](HANDLE thread) {
        if (WaitForSingleObject(thread, 0) == WAIT_OBJECT_0) {
            CloseHandle(thread);
            return true;
        }
        return false;
    });
    if (!m_hwnd) {
        return;
    }
    auto* request = new RadioRequest{m_hwnd, kind, on};
    if (HANDLE thread =
            CreateThread(nullptr, 0, RadioThreadProc, request, 0, nullptr)) {
        m_workers.push_back(thread);
    } else {
        delete request;
    }
}

void ControlPanel::StartWorker(LPTHREAD_START_ROUTINE proc, void* parameter) {
    std::erase_if(m_workers, [](HANDLE thread) {
        if (WaitForSingleObject(thread, 0) == WAIT_OBJECT_0) {
            CloseHandle(thread);
            return true;
        }
        return false;
    });
    if (HANDLE thread = CreateThread(nullptr, 0, proc, parameter, 0, nullptr)) {
        m_workers.push_back(thread);
    }
}

// The display mode, like Windows+P: the PC's screen only, duplicated,
// extended, or the second screen only.
void ControlPanel::UpdateDisplayMode() {
    UINT32 paths = 0;
    UINT32 modes = 0;
    m_displayMode = 0;
    if (GetDisplayConfigBufferSizes(QDC_DATABASE_CURRENT, &paths, &modes) !=
        ERROR_SUCCESS) {
        return;
    }
    std::vector<DISPLAYCONFIG_PATH_INFO> pathInfo(paths);
    std::vector<DISPLAYCONFIG_MODE_INFO> modeInfo(modes);
    DISPLAYCONFIG_TOPOLOGY_ID topology{};
    if (QueryDisplayConfig(QDC_DATABASE_CURRENT, &paths, pathInfo.data(),
                           &modes, modeInfo.data(),
                           &topology) == ERROR_SUCCESS) {
        m_displayMode = (int)topology;
    }
}

// The accessibility features with a switch in the Windows quick settings
// that can be read and switched: the magnifier, the narrator, color filters
// (only when their shortcut is on, which is how they're switched) and sticky
// keys.
void ControlPanel::UpdateAccessibility() {
    m_accessibility.clear();
    m_accessibility.push_back(
        {Feature::Magnifier, kGlyphMagnifier, Tr(L"Magnifier", L"Lupa"),
         FindWindow(L"MagUIClass", nullptr) != nullptr});
    BOOL narrator = FALSE;
    SystemParametersInfo(SPI_GETSCREENREADER, 0, &narrator, 0);
    m_accessibility.push_back({Feature::Narrator, kGlyphNarrator,
                               Tr(L"Narrator", L"Narrador"),
                               narrator != FALSE});
    DWORD hotkey = 0;
    DWORD active = 0;
    DWORD size = sizeof(hotkey);
    RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\ColorFiltering",
                L"HotkeyEnabled", RRF_RT_REG_DWORD, nullptr, &hotkey, &size);
    size = sizeof(active);
    RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\ColorFiltering",
                L"Active", RRF_RT_REG_DWORD, nullptr, &active, &size);
    if (hotkey) {
        m_accessibility.push_back({Feature::ColorFilters, kGlyphColorFilters,
                                   Tr(L"Color filters", L"Filtros de cor"),
                                   active != 0});
    }
    STICKYKEYS sticky{sizeof(sticky)};
    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(sticky), &sticky, 0);
    m_accessibility.push_back({Feature::StickyKeys, kGlyphKeyboard,
                               Tr(L"Sticky keys", L"Teclas de aderência"),
                               (sticky.dwFlags & SKF_STICKYKEYSON) != 0});
}

void ControlPanel::ToggleAccessibility(int index) {
    if (index < 0 || index >= (int)m_accessibility.size()) {
        return;
    }
    AccessibilityItem& item = m_accessibility[index];
    switch (item.feature) {
        case Feature::Magnifier:
            if (HWND magnifier = FindWindow(L"MagUIClass", nullptr)) {
                PostMessage(magnifier, WM_CLOSE, 0, 0);
            } else {
                ShellExecute(nullptr, L"open", L"magnify.exe", nullptr,
                             nullptr, SW_SHOWNORMAL);
            }
            break;
        case Feature::Narrator:
            PressKeys({VK_LWIN, VK_CONTROL, VK_RETURN});
            break;
        case Feature::ColorFilters:
            PressKeys({VK_LWIN, VK_CONTROL, 'C'});
            break;
        case Feature::StickyKeys: {
            STICKYKEYS sticky{sizeof(sticky)};
            if (SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(sticky),
                                     &sticky, 0)) {
                sticky.dwFlags ^= SKF_STICKYKEYSON;
                SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(sticky),
                                     &sticky,
                                     SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
            }
            break;
        }
    }
    // Shown right away; the next update confirms it.
    item.on = !item.on;
}

void ControlPanel::SetBrightness(int level) {
    m_brightness = std::clamp(level, 0, 100);
    m_brightnessJob.notify = m_hwnd;
    m_brightnessJob.target = m_brightness;
    if (!m_brightnessJob.running.exchange(true)) {
        const size_t before = m_workers.size();
        StartWorker(BrightnessThreadProc, &m_brightnessJob);
        if (m_workers.size() == before) {
            m_brightnessJob.running = false;
        }
    }
}

void ControlPanel::SetBrightnessAt(POINT pt) {
    const D2D1_RECT_F& rect = m_brightnessSlider;
    const float knob = rect.bottom - rect.top;
    SetBrightness((int)std::lround(
        100 * std::clamp((pt.x - rect.left - knob / 2) /
                             (rect.right - rect.left - knob),
                         0.0f, 1.0f)));
}

void ControlPanel::WaitForWorkers() {
    for (HANDLE thread : m_workers) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    m_workers.clear();
}

// The line under a list: why it's empty, or how joining went.
std::wstring ControlPanel::StatusText() const {
    if (m_view == View::Wifi) {
        if (m_wifiRadio == 0) {
            return Tr(L"Wi-Fi is off", L"Wi-Fi desligado");
        }
        if (!m_hasWifi) {
            return Tr(L"No Wi-Fi adapter", L"Sem adaptador de Wi-Fi");
        }
        if (!m_wifiStatus.empty()) {
            return m_wifiStatus;
        }
        if (m_networks.empty()) {
            return Tr(L"Looking for networks...", L"Procurando redes...");
        }
    } else if (m_view == View::Bluetooth) {
        if (m_bluetoothRadio == kRadioMissing) {
            return Tr(L"No Bluetooth", L"Sem Bluetooth");
        }
        if (m_bluetoothRadio == 0) {
            return Tr(L"Bluetooth is off", L"Bluetooth desligado");
        }
        if (m_bluetoothDevices.empty()) {
            return Tr(L"No paired devices", L"Nenhum dispositivo pareado");
        }
    } else if (m_view == View::Password) {
        return m_wifiStatus;
    }
    return {};
}

void ControlPanel::Update() {
    if (!g_island) {
        return;
    }
    m_network = g_island->QueryNetwork();
    m_device.clear();
    m_hasVolume = false;
    if (auto volume = g_island->GetVolume(&m_device)) {
        BOOL muted = FALSE;
        m_hasVolume = SUCCEEDED(volume->GetMasterVolumeLevelScalar(&m_volume)) &&
                      SUCCEEDED(volume->GetMute(&muted));
        m_muted = muted;
    }
    m_doNotDisturb = IsDoNotDisturbOn();
    m_nightLight = GetNightLight();

    SYSTEM_POWER_STATUS power;
    m_hasBattery = GetSystemPowerStatus(&power) && !(power.BatteryFlag & 128) &&
                   power.BatteryFlag != 255 && power.BatteryLifePercent <= 100;
    if (m_hasBattery) {
        m_batteryPercent = power.BatteryLifePercent;
        m_charging = power.ACLineStatus == 1;
    }

    if (m_view == View::Output) {
        UpdateOutputs();
    } else if (m_view == View::Mixer) {
        UpdateApps();
    } else if (m_view == View::Wifi) {
        UpdateWifi();
    } else if (m_view == View::Bluetooth) {
        UpdateBluetooth();
    } else if (m_view == View::Project) {
        UpdateDisplayMode();
    } else if (m_view == View::Accessibility) {
        UpdateAccessibility();
    }
}

// The active output devices, the default one marked.
void ControlPanel::UpdateOutputs() {
    m_outputs.clear();
    IMMDeviceEnumerator* devices = g_island ? g_island->Devices() : nullptr;
    if (!devices) {
        return;
    }
    std::wstring defaultId;
    winrt::com_ptr<IMMDevice> defaultDevice;
    if (SUCCEEDED(devices->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                   defaultDevice.put()))) {
        LPWSTR id = nullptr;
        if (SUCCEEDED(defaultDevice->GetId(&id))) {
            defaultId = id;
            CoTaskMemFree(id);
        }
    }

    winrt::com_ptr<IMMDeviceCollection> collection;
    UINT count = 0;
    if (FAILED(devices->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                           collection.put())) ||
        FAILED(collection->GetCount(&count))) {
        return;
    }
    for (UINT i = 0; i < count; i++) {
        winrt::com_ptr<IMMDevice> device;
        LPWSTR id = nullptr;
        if (FAILED(collection->Item(i, device.put())) ||
            FAILED(device->GetId(&id))) {
            continue;
        }
        OutputDevice output{id, L"", kGlyphSpeakers, defaultId == id};
        CoTaskMemFree(id);

        winrt::com_ptr<IPropertyStore> properties;
        if (SUCCEEDED(device->OpenPropertyStore(STGM_READ,
                                                properties.put()))) {
            PROPVARIANT value;
            PropVariantInit(&value);
            if (SUCCEEDED(properties->GetValue(kPKEY_Device_FriendlyName,
                                               &value)) &&
                value.vt == VT_LPWSTR && value.pwszVal) {
                output.name = value.pwszVal;
            }
            PropVariantClear(&value);
            PropVariantInit(&value);
            // Form factor: headphones (3) and headsets (5), displays (9).
            if (SUCCEEDED(properties->GetValue(kPKEY_AudioEndpoint_FormFactor,
                                               &value)) &&
                value.vt == VT_UI4) {
                if (value.ulVal == 3 || value.ulVal == 5) {
                    output.glyph = kGlyphHeadphones;
                } else if (value.ulVal == 9) {
                    output.glyph = kGlyphMonitor;
                }
            }
            PropVariantClear(&value);
        }
        m_outputs.push_back(std::move(output));
    }
}

void ControlPanel::ClearApps() {
    for (auto& app : m_apps) {
        if (app.icon) {
            DestroyIcon(app.icon);
        }
    }
    m_apps.clear();
}

// The programs playing sound on the default device, one entry per program.
void ControlPanel::UpdateApps() {
    // Icons and bitmaps are kept for the programs still there.
    std::vector<MixerApp> previous = std::move(m_apps);
    m_apps.clear();

    IMMDeviceEnumerator* devices = g_island ? g_island->Devices() : nullptr;
    winrt::com_ptr<IMMDevice> device;
    winrt::com_ptr<IAudioSessionManager2> manager;
    winrt::com_ptr<IAudioSessionEnumerator> sessions;
    int count = 0;
    if (devices &&
        SUCCEEDED(devices->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                   device.put())) &&
        SUCCEEDED(device->Activate(kIID_IAudioSessionManager2, CLSCTX_ALL,
                                   nullptr, manager.put_void())) &&
        SUCCEEDED(manager->GetSessionEnumerator(sessions.put())) &&
        SUCCEEDED(sessions->GetCount(&count))) {
        for (int i = 0; i < count; i++) {
            winrt::com_ptr<IAudioSessionControl> control;
            if (FAILED(sessions->GetSession(i, control.put()))) {
                continue;
            }
            AudioSessionState state;
            if (FAILED(control->GetState(&state)) ||
                state == AudioSessionStateExpired) {
                continue;
            }
            auto control2 = control.try_as<IAudioSessionControl2>();
            auto volume = control.try_as<ISimpleAudioVolume>();
            if (!control2 || !volume) {
                continue;
            }
            const bool system = control2->IsSystemSoundsSession() == S_OK;
            DWORD processId = 0;
            control2->GetProcessId(&processId);

            auto it = std::find_if(m_apps.begin(), m_apps.end(),
                                   [&](const MixerApp& app) {
                                       return app.system == system &&
                                              app.processId == processId;
                                   });
            if (it == m_apps.end()) {
                if (m_apps.size() >= kMaxMixerApps) {
                    continue;
                }
                MixerApp app;
                app.processId = processId;
                app.system = system;
                float level = 1;
                BOOL muted = FALSE;
                volume->GetMasterVolume(&level);
                volume->GetMute(&muted);
                app.volume = level;
                app.muted = muted;
                m_apps.push_back(std::move(app));
                it = m_apps.end() - 1;
            }
            it->volumes.push_back(volume);
        }
    }

    for (auto& app : m_apps) {
        auto old = std::find_if(previous.begin(), previous.end(),
                                [&](const MixerApp& other) {
                                    return other.system == app.system &&
                                           other.processId == app.processId;
                                });
        if (old != previous.end()) {
            app.name = old->name;
            app.icon = std::exchange(old->icon, nullptr);
            app.bitmap = old->bitmap;
            continue;
        }
        if (app.system) {
            app.name = Tr(L"System sounds (notifications)",
                          L"Sons do sistema (notificações)");
            continue;
        }
        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                     app.processId);
        if (process) {
            WCHAR path[MAX_PATH];
            DWORD length = ARRAYSIZE(path);
            if (QueryFullProcessImageName(process, 0, path, &length)) {
                app.name = GetProgramName(path);
                ExtractIconEx(path, 0, &app.icon, nullptr, 1);
            }
            CloseHandle(process);
        }
        if (app.name.empty()) {
            app.name = Tr(L"App", L"App");
        }
    }
    for (auto& old : previous) {
        if (old.icon) {
            DestroyIcon(old.icon);
        }
    }
}

void ControlPanel::SetView(View view) {
    // The new view slides in: from the right going in, from the left coming
    // back to the main one (or the Wi-Fi list from the password).
    if (view != m_view) {
        const bool back = view == View::Main ||
                          (view == View::Wifi && m_view == View::Password) ||
                          (view == View::Settings && m_view == View::AppPlaces);
        m_viewDirection = back ? -1 : 1;
        m_viewIn = 0;
        m_viewInVelocity = 0;
        StartIslandAnimation();
    }
    m_view = view;
    m_hover = {};
    if (view == View::Wifi && m_wlan && m_hasWifi) {
        // The list is updated when the scan is done (WM_APP_WLAN).
        WlanScan(m_wlan, &m_wifiInterface, nullptr, nullptr, nullptr);
    }
    Update();
    Layout();
}

void ControlPanel::Layout() {
    const float s = m_scale;
    const float width = std::round(kControlWidth * s);
    const float padding = std::round(kControlPadding * s);
    const float gap = std::round(kControlGap * s);
    float y = padding;

    m_rows.clear();
    m_rowIcons.clear();
    m_rowSliders.clear();

    if (m_view != View::Main) {
        // "<  Title" header (with an on/off switch for the radios), then a row
        // per device, app or network.
        const float header = std::round(kHeaderHeight * s);
        m_back = {padding, y, padding + header, y + header};
        const float toggleWidth = std::round(36 * s);
        const float toggleHeight = std::round(20 * s);
        const float toggleRight = width - padding - std::round(4 * s);
        m_toggle = {toggleRight - toggleWidth,
                    y + std::round((header - toggleHeight) / 2),
                    toggleRight,
                    y + std::round((header - toggleHeight) / 2) +
                        toggleHeight};
        y += header;
        if (m_view == View::Wifi || m_view == View::Bluetooth) {
            const size_t count = m_view == View::Wifi
                                     ? m_networks.size()
                                     : m_bluetoothDevices.size();
            const float row = std::round(kRowHeight * s);
            for (size_t i = 0; i < count; i++) {
                m_rows.push_back({padding, y, width - padding, y + row});
                y += row;
            }
            m_status = {padding, y, width - padding, y + row};
            if (!StatusText().empty()) {
                y += row;
            }
            // The mobile hotspot and airplane mode, under the networks.
            m_hotspotRow = m_airplaneRow = {};
            if (m_view == View::Wifi &&
                (m_hotspot >= 0 || m_airplane >= 0)) {
                y += std::round(4 * s);
                if (m_hotspot >= 0) {
                    m_hotspotRow = {padding, y, width - padding, y + row};
                    y += row;
                }
                if (m_airplane >= 0) {
                    m_airplaneRow = {padding, y, width - padding, y + row};
                    y += row;
                }
            }
        } else if (m_view == View::Settings) {
            // The style (pill or bar), the colors, then the switches, under
            // their labels.
            const float choice = std::round(44 * s);
            const float row = std::round(34 * s);
            const float label = std::round(26 * s);
            m_styleRow = {padding, y, width - padding, y + choice};
            const float segment = std::round(74 * s);
            const float segmentTop = y + std::round(7 * s);
            const float segmentBottom = y + choice - std::round(7 * s);
            m_styleSegments[1] = {width - padding - std::round(4 * s) - segment,
                                  segmentTop, width - padding - std::round(4 * s),
                                  segmentBottom};
            m_styleSegments[0] = {m_styleSegments[1].left - segment, segmentTop,
                                  m_styleSegments[1].left, segmentBottom};
            y += choice;
            m_themeRow = {padding, y, width - padding, y + choice};
            const float swatch = std::round(22 * s);
            const float swatchGap = std::round(8 * s);
            float swatchRight = width - padding - std::round(6 * s);
            for (int i = (int)ThemeId::Count - 1; i >= 0; i--) {
                const float middle = y + choice / 2;
                m_swatches[i] = {swatchRight - swatch, middle - swatch / 2,
                                 swatchRight, middle + swatch / 2};
                swatchRight -= swatch + swatchGap;
            }
            y += choice;
            // The place: a little screen with its 9 ready places, then the
            // places per app.
            m_placeRow = {padding, y, width - padding, y + choice};
            const float gridWidth = std::round(54 * s);
            const float gridHeight = std::round(32 * s);
            m_placeGrid = {width - padding - std::round(6 * s) - gridWidth,
                           y + (choice - gridHeight) / 2,
                           width - padding - std::round(6 * s),
                           y + (choice + gridHeight) / 2};
            y += choice;
            m_appPlacesRow = {padding, y, width - padding, y + row};
            y += row;
            // The size: a slider, and a button to restore it.
            m_sizeRow = {padding, y, width - padding, y + choice};
            const float reset = std::round(28 * s);
            m_sizeReset = {width - padding - std::round(2 * s) - reset,
                           y + (choice - reset) / 2,
                           width - padding - std::round(2 * s),
                           y + (choice + reset) / 2};
            m_sizeSlider = {padding + std::round((width - padding * 2) * 0.48f),
                            y, m_sizeReset.left - std::round(10 * s),
                            y + choice};
            y += choice;
            m_prefRows.clear();
            for (int item : kPrefRows) {
                const float height = item < 0 ? label : row;
                m_prefRows.push_back(
                    {item, {padding, y, width - padding, y + height}});
                y += height;
            }
        } else if (m_view == View::AppPlaces) {
            // A row per app with its place and its "x", then the open apps
            // to add, two by two.
            const float row = std::round(44 * s);
            const float gridWidth = std::round(50 * s);
            const float gridHeight = std::round(30 * s);
            const float remove = std::round(28 * s);
            m_appPlaceRows.clear();
            m_appPlaceGrids.clear();
            m_appPlaceRemoves.clear();
            for (size_t i = 0; i < g_appPlaces.size(); i++) {
                const D2D1_RECT_F rect{padding, y, width - padding, y + row};
                m_appPlaceRows.push_back(rect);
                m_appPlaceRemoves.push_back(
                    {rect.right - remove, y, rect.right, y + row});
                m_appPlaceGrids.push_back(
                    {rect.right - remove - std::round(6 * s) - gridWidth,
                     y + (row - gridHeight) / 2,
                     rect.right - remove - std::round(6 * s),
                     y + (row + gridHeight) / 2});
                y += row;
            }
            const float label = std::round(28 * s);
            m_addAppsHeader = {padding, y, width - padding, y + label};
            y += label;
            const float chip = std::round(34 * s);
            const float chipGap = std::round(6 * s);
            const float chipWidth = (width - padding * 2 - chipGap) / 2;
            m_openAppChips.clear();
            for (size_t i = 0; i < m_openApps.size(); i++) {
                const float left = padding + (chipWidth + chipGap) * (i % 2);
                m_openAppChips.push_back(
                    {std::round(left), y, std::round(left + chipWidth), y + chip});
                if (i % 2 == 1 || i + 1 == m_openApps.size()) {
                    y += chip + chipGap;
                }
            }
            m_status = {padding, y, width - padding, y + label * 1.5f};
            y += label * 1.5f;
        } else if (m_view == View::Project ||
                   m_view == View::Accessibility) {
            const size_t count =
                m_view == View::Project ? 4 : m_accessibility.size();
            const float row = std::round(kRowHeight * s);
            for (size_t i = 0; i < count; i++) {
                m_rows.push_back({padding, y, width - padding, y + row});
                y += row;
            }
        } else if (m_view == View::Password) {
            y += std::round(4 * s);
            const float field = std::round(36 * s);
            m_field = {padding, y, width - padding, y + field};
            m_eye = {m_field.right - field, m_field.top, m_field.right,
                     m_field.bottom};
            y += field + gap;
            const float button = std::round(32 * s);
            m_connect = {width - padding - std::round(100 * s), y,
                         width - padding, y + button};
            m_status = {padding, y, m_connect.left - gap, y + button};
            y += button;
        } else if (m_view == View::Output) {
            const float row = std::round(kRowHeight * s);
            for (size_t i = 0; i < m_outputs.size(); i++) {
                m_rows.push_back({padding, y, width - padding, y + row});
                y += row;
            }
        } else {
            const float row = std::round(kMixerRowHeight * s);
            const float icon = std::round(30 * s);
            const float slider = std::round(18 * s);
            // The device's volume first, then one row per app.
            for (size_t i = 0; i < m_apps.size() + 1; i++) {
                const D2D1_RECT_F rect{padding, y, width - padding, y + row};
                m_rows.push_back(rect);
                const float middle = (rect.top + rect.bottom) / 2;
                m_rowIcons.push_back({rect.left + std::round(4 * s),
                                      middle - icon / 2,
                                      rect.left + std::round(4 * s) + icon,
                                      middle + icon / 2});
                const float sliderLeft =
                    rect.left + std::round(4 * s) + icon + std::round(12 * s);
                const float sliderTop = middle + std::round(1 * s);
                m_rowSliders.push_back({sliderLeft, sliderTop,
                                        rect.right - std::round(6 * s),
                                        sliderTop + slider});
                y += row;
            }
        }
        y += padding;
        m_size = {(LONG)width, (LONG)std::ceil(y)};
        return;
    }

    // The chosen controls, in order: tiles two by two, buttons four by four,
    // sections across.
    LoadControls();
    m_controlRects.clear();
    m_networkTile = m_bluetoothTile = m_dndTile = m_nightTile = {};
    m_networkIcon = m_bluetoothIcon = m_dndIcon = {};
    m_display = m_brightnessSlider = m_sound = m_slider = m_volumeIcon =
        m_deviceRow = m_mixerButton = m_batteryTile = {};
    const float tile = std::round(kTileHeight * s);
    const float half = (width - padding * 2 - gap) / 2;
    const float shortcut = std::round(kShortcutHeight * s);
    const float shortcutWidth = (width - padding * 2 - gap * 3) / 4;
    // The tiles' round icons switch them (see DrawTile).
    auto iconOf = [s](const D2D1_RECT_F& rect) {
        const float circle = std::round(17 * s);
        const float x = rect.left + std::round(12 * s) + circle;
        const float y = (rect.top + rect.bottom) / 2;
        return D2D1_RECT_F{x - circle, y - circle, x + circle, y + circle};
    };
    ControlKind rowKind = ControlKind::Section;
    int column = 0;
    auto endRow = [&]() {
        if (column > 0) {
            y += (rowKind == ControlKind::Tile ? tile : shortcut) + gap;
        }
        column = 0;
    };
    for (ControlId id : m_controls) {
        const ControlInfo& info = GetControlInfo(id);
        if (info.kind != rowKind) {
            endRow();
            rowKind = info.kind;
        }
        D2D1_RECT_F rect{};
        if (info.kind == ControlKind::Tile) {
            const float left = padding + (half + gap) * column;
            rect = {std::round(left), y, std::round(left + half), y + tile};
            if (++column == 2) {
                endRow();
            }
        } else if (info.kind == ControlKind::Button) {
            const float left = padding + (shortcutWidth + gap) * column;
            rect = {std::round(left), y, std::round(left + shortcutWidth),
                    y + shortcut};
            if (++column == 4) {
                endRow();
            }
        } else if (id == ControlId::Display) {
            // Only when the monitor lets it be changed (while editing, its
            // place is shown anyway).
            if (m_brightness < 0 && !m_editing) {
                continue;
            }
            const float display = std::round(kDisplayHeight * s);
            rect = {padding, y, width - padding, y + display};
            if (m_brightness >= 0) {
                m_display = rect;
                const float inner = std::round(12 * s);
                const float sliderTop = y + std::round(34 * s);
                m_brightnessSlider = {padding + inner, sliderTop,
                                      width - padding - inner,
                                      sliderTop + std::round(26 * s)};
            }
            y += display + gap;
        } else if (id == ControlId::Sound) {
            const float sound = std::round(kSoundHeight * s);
            rect = m_sound = {padding, y, width - padding, y + sound};
            const float inner = std::round(12 * s);
            const float sliderHeight = std::round(26 * s);
            const float sliderTop = y + std::round(34 * s);
            m_slider = {padding + inner, sliderTop, width - padding - inner,
                        sliderTop + sliderHeight};
            m_volumeIcon = {m_slider.left, m_slider.top,
                            m_slider.left + sliderHeight * 1.2f,
                            m_slider.bottom};
            const float bottomTop = m_slider.bottom + std::round(4 * s);
            const float bottomBottom = y + sound - std::round(4 * s);
            const float mixerWidth = std::round(64 * s);
            m_mixerButton = {width - padding - inner - mixerWidth, bottomTop,
                             width - padding - inner, bottomBottom};
            m_deviceRow = {padding + inner, bottomTop,
                           m_mixerButton.left - std::round(8 * s),
                           bottomBottom};
            y += sound + gap;
        } else if (id == ControlId::Battery) {
            if (!m_hasBattery && !m_editing) {
                continue;
            }
            const float battery = std::round(kBatteryHeight * s);
            rect = {padding, y, width - padding, y + battery};
            if (m_hasBattery) {
                m_batteryTile = rect;
            }
            y += battery + gap;
        }
        switch (id) {
            case ControlId::Network:
                m_networkTile = rect;
                m_networkIcon = iconOf(rect);
                break;
            case ControlId::Bluetooth:
                m_bluetoothTile = rect;
                m_bluetoothIcon = iconOf(rect);
                break;
            case ControlId::DoNotDisturb:
                m_dndTile = rect;
                m_dndIcon = iconOf(rect);
                break;
            case ControlId::NightLight:
                m_nightTile = rect;
                break;
            default:
                break;
        }
        m_controlRects.push_back({id, rect});
    }
    endRow();
    y -= gap / 2;

    const float footer = std::round(kFooterHeight * s);
    m_addRects.clear();
    if (m_editing) {
        // The controls not shown, to add, three by three; then "Done".
        const float label = std::round(26 * s);
        m_addHeader = {padding, y, width - padding, y + label};
        y += label;
        const float chip = std::round(34 * s);
        const float chipGap = std::round(6 * s);
        const float chipWidth = (width - padding * 2 - chipGap * 2) / 3;
        int index = 0;
        for (const auto& info : kControls) {
            if (std::find(m_controls.begin(), m_controls.end(), info.id) !=
                m_controls.end()) {
                continue;
            }
            const float left = padding + (chipWidth + chipGap) * (index % 3);
            m_addRects.push_back(
                {info.id,
                 {std::round(left), y, std::round(left + chipWidth), y + chip}});
            if (++index % 3 == 0) {
                y += chip + chipGap;
            }
        }
        if (index % 3) {
            y += chip + chipGap;
        }
        m_status = {padding, y, width - padding, y + label};
        if (!m_editStatus.empty()) {
            y += label;
        }
        y += std::round(4 * s);
        const float doneWidth = std::round(110 * s);
        m_done = {width - padding - doneWidth, y, width - padding, y + footer};
        m_more = m_edit = m_gear = {};
    } else {
        // "More Windows controls", the gear and "Edit".
        m_done = m_addHeader = {};
        const float editWidth = std::round(84 * s);
        m_edit = {width - padding - editWidth, y, width - padding, y + footer};
        m_gear = {m_edit.left - std::round(6 * s) - footer, y,
                  m_edit.left - std::round(6 * s), y + footer};
        m_more = {padding, y, m_gear.left - gap, y + footer};
    }
    y += footer + padding / 2;

    // Each control is shown where it is, following it with a spring (see
    // AnimateContents); a new one starts at its place.
    for (const auto& [id, rect] : m_controlRects) {
        if (!FindMotion(id)) {
            m_motions.push_back({id, rect.left, rect.top});
        }
    }
    std::erase_if(m_motions, [this](const Motion& motion) {
        return std::none_of(m_controlRects.begin(), m_controlRects.end(),
                            [&](const auto& entry) {
                                return entry.first == motion.id;
                            });
    });
    // Something moved (a section shown, another order): it slides there.
    for (const auto& [id, rect] : m_controlRects) {
        const Motion* motion = FindMotion(id);
        if (motion && (std::fabs(motion->x - rect.left) > 0.5 ||
                       std::fabs(motion->y - rect.top) > 0.5)) {
            StartIslandAnimation();
            break;
        }
    }

    m_size = {(LONG)width, (LONG)std::ceil(y)};
}

ControlPanel::Motion* ControlPanel::FindMotion(ControlId id) {
    for (auto& motion : m_motions) {
        if (motion.id == id) {
            return &motion;
        }
    }
    return nullptr;
}

void ControlPanel::SetEditing(bool editing) {
    m_editing = editing;
    m_editStatus.clear();
    m_dragId = -1;
    m_dragMoved = false;
    m_hover = {};
    Layout();
    StartIslandAnimation();
}

// The controls chosen, or the usual ones.
void ControlPanel::LoadControls() {
    if (m_controlsLoaded) {
        return;
    }
    m_controlsLoaded = true;
    m_controls.clear();
    WCHAR value[1024] = L"";
    Wh_GetStringValue(L"controls", value, ARRAYSIZE(value));
    if (!value[0]) {
        for (const auto& info : kControls) {
            if (info.shownByDefault) {
                m_controls.push_back(info.id);
            }
        }
        return;
    }
    for (PCWSTR p = value; *p;) {
        PCWSTR end = wcschr(p, L',');
        const std::wstring key = end ? std::wstring(p, end) : std::wstring(p);
        for (const auto& info : kControls) {
            if (key == info.key &&
                std::find(m_controls.begin(), m_controls.end(), info.id) ==
                    m_controls.end()) {
                m_controls.push_back(info.id);
            }
        }
        if (!end) {
            break;
        }
        p = end + 1;
    }
}

void ControlPanel::SaveControls() {
    std::wstring value;
    for (ControlId id : m_controls) {
        value += (value.empty() ? L"" : L",") +
                 std::wstring(GetControlInfo(id).key);
    }
    // Nothing chosen is still something chosen.
    Wh_SetStringValue(L"controls", value.empty() ? L"none" : value.c_str());
}

ControlPanel::Hit ControlPanel::HitTest(POINT pt) const {
    if (m_view == View::Wifi || m_view == View::Bluetooth) {
        if (PointInRect(pt, m_back)) {
            return {Part::Back};
        }
        const bool wifi = m_view == View::Wifi;
        if ((wifi ? m_wifiRadio : m_bluetoothRadio) >= 0 &&
            PointInRect(pt, m_toggle)) {
            return {wifi ? Part::WifiToggle : Part::BluetoothToggle};
        }
        for (size_t i = 0; i < m_rows.size(); i++) {
            if (!PointInRect(pt, m_rows[i])) {
                continue;
            }
            if (!wifi) {
                return {Part::BluetoothRow, (int)i};
            }
            // The connected network has "Disconnect" on its right.
            if (i < m_networks.size() && m_networks[i].connected &&
                pt.x >= m_rows[i].right - std::round(100 * m_scale)) {
                return {Part::Disconnect, (int)i};
            }
            return {Part::WifiRow, (int)i};
        }
        if (wifi && m_hotspot >= 0 && PointInRect(pt, m_hotspotRow)) {
            return {Part::HotspotToggle};
        }
        if (wifi && m_airplane >= 0 && PointInRect(pt, m_airplaneRow)) {
            return {Part::AirplaneToggle};
        }
        return {};
    }
    if (m_view == View::Settings) {
        if (PointInRect(pt, m_back)) {
            return {Part::Back};
        }
        for (int i = 0; i < 2; i++) {
            if (PointInRect(pt, m_styleSegments[i])) {
                return {Part::StyleSegment, i};
            }
        }
        const float grow = std::round(3 * m_scale);
        for (int i = 0; i < (int)ThemeId::Count; i++) {
            const D2D1_RECT_F& r = m_swatches[i];
            if (PointInRect(pt, {r.left - grow, r.top - grow, r.right + grow,
                                 r.bottom + grow})) {
                return {Part::ThemeSwatch, i};
            }
        }
        if (PointInRect(pt, m_sizeReset)) {
            return {Part::SizeReset};
        }
        if (PointInRect(pt, m_sizeSlider)) {
            return {Part::SizeSlider};
        }
        if (!g_settings.islandBar) {
            const int dot = PlaceDotAt(m_placeGrid, pt);
            if (dot >= 0) {
                return {Part::PlaceDot, dot};
            }
            if (PointInRect(pt, m_appPlacesRow)) {
                return {Part::AppPlacesRow};
            }
        }
        for (const auto& [item, rect] : m_prefRows) {
            if (item >= 0 && PointInRect(pt, rect) &&
                !PrefDisabled((IslandPref)item)) {
                return {Part::SettingToggle, item};
            }
        }
        return {};
    }
    if (m_view == View::AppPlaces) {
        if (PointInRect(pt, m_back)) {
            return {Part::Back};
        }
        for (size_t i = 0; i < m_appPlaceRows.size(); i++) {
            if (PointInRect(pt, m_appPlaceRemoves[i])) {
                return {Part::AppPlaceRemove, (int)i};
            }
            const int dot = PlaceDotAt(m_appPlaceGrids[i], pt);
            if (dot >= 0) {
                return {Part::AppPlaceDot, (int)i * 9 + dot};
            }
        }
        for (size_t i = 0; i < m_openAppChips.size(); i++) {
            if (PointInRect(pt, m_openAppChips[i])) {
                return {Part::AppPlaceAdd, (int)i};
            }
        }
        return {};
    }
    if (m_view == View::Project || m_view == View::Accessibility) {
        if (PointInRect(pt, m_back)) {
            return {Part::Back};
        }
        for (size_t i = 0; i < m_rows.size(); i++) {
            if (PointInRect(pt, m_rows[i])) {
                return {m_view == View::Project ? Part::ProjectRow
                                                : Part::AccessibilityRow,
                        (int)i};
            }
        }
        return {};
    }
    if (m_view == View::Password) {
        if (PointInRect(pt, m_back)) {
            return {Part::Back};
        }
        if (PointInRect(pt, m_eye)) {
            return {Part::ShowPassword};
        }
        if (PointInRect(pt, m_field)) {
            return {Part::PasswordField};
        }
        if (PointInRect(pt, m_connect)) {
            return {Part::Connect};
        }
        return {};
    }
    if (m_view != View::Main) {
        if (PointInRect(pt, m_back)) {
            return {Part::Back};
        }
        for (size_t i = 0; i < m_rows.size(); i++) {
            if (!PointInRect(pt, m_rows[i])) {
                continue;
            }
            if (m_view == View::Output) {
                return {Part::Output, (int)i};
            }
            // Row 0 is the device's volume (index -1).
            const int index = (int)i - 1;
            if (PointInRect(pt, m_rowIcons[i])) {
                return index < 0 ? Hit{Part::VolumeIcon} : Hit{Part::AppIcon,
                                                                index};
            }
            return {Part::AppSlider, index};
        }
        return {};
    }
    // Editing: the "-" first (it sticks out of the corner), the controls, the
    // ones to add, "Done".
    if (m_editing) {
        const float badge = std::round(11 * m_scale);
        for (const auto& [id, rect] : m_controlRects) {
            if (PointInRect(pt, {rect.left - badge, rect.top - badge,
                                 rect.left + badge * 1.4f,
                                 rect.top + badge * 1.4f})) {
                return {Part::EditRemove, (int)id};
            }
        }
        for (const auto& [id, rect] : m_controlRects) {
            if (PointInRect(pt, rect)) {
                return {Part::EditControl, (int)id};
            }
        }
        for (const auto& [id, rect] : m_addRects) {
            if (PointInRect(pt, rect)) {
                return {Part::EditAdd, (int)id};
            }
        }
        if (PointInRect(pt, m_done)) {
            return {Part::EditDone};
        }
        return {};
    }
    for (const auto& [id, rect] : m_controlRects) {
        if (!PointInRect(pt, rect)) {
            continue;
        }
        switch (id) {
            case ControlId::Network:
                return {m_wifiRadio >= 0 && PointInRect(pt, m_networkIcon)
                            ? Part::WifiToggle
                            : Part::Network};
            case ControlId::Bluetooth:
                return {m_bluetoothRadio >= 0 &&
                                PointInRect(pt, m_bluetoothIcon)
                            ? Part::BluetoothToggle
                            : Part::Bluetooth};
            case ControlId::DoNotDisturb:
                return {PointInRect(pt, m_dndIcon) ? Part::DndToggle
                                                   : Part::DoNotDisturb};
            case ControlId::NightLight:
                return {Part::NightLight};
            case ControlId::Hotspot:
                return {m_hotspot >= 0 ? Part::HotspotToggle : Part::None};
            case ControlId::Airplane:
                return {m_airplane >= 0 ? Part::AirplaneToggle : Part::None};
            case ControlId::Display:
                return {Part::BrightnessSlider};
            case ControlId::Sound:
                if (PointInRect(pt, m_volumeIcon)) {
                    return {Part::VolumeIcon};
                }
                if (PointInRect(pt, m_slider)) {
                    return {Part::Slider};
                }
                if (PointInRect(pt, m_deviceRow)) {
                    return {Part::Device};
                }
                if (PointInRect(pt, m_mixerButton)) {
                    return {Part::MixerButton};
                }
                return {};
            case ControlId::Battery:
                return {Part::Battery};
            default:
                return {Part::Shortcut, (int)id};
        }
    }
    if (PointInRect(pt, m_edit)) {
        return {Part::EditButton};
    }
    if (PointInRect(pt, m_gear)) {
        return {Part::SettingsButton};
    }
    if (PointInRect(pt, m_more)) {
        return {Part::More};
    }
    return {};
}

void ControlPanel::DrawTile(ID2D1RenderTarget* target,
                            ID2D1SolidColorBrush* brush,
                            const D2D1_RECT_F& rect,
                            WCHAR glyph,
                            bool active,
                            PCWSTR title,
                            const std::wstring& subtitle,
                            bool hover) {
    const float s = m_scale;
    const float radius = std::round(14 * s);
    brush->SetColor(Fg(hover ? 0.14f : 0.08f));
    target->FillRoundedRectangle({rect, radius, radius}, brush);

    // The icon in a circle, blue when it's on.
    const float circle = std::round(17 * s);
    const D2D1_POINT_2F center{rect.left + std::round(12 * s) + circle,
                               (rect.top + rect.bottom) / 2};
    brush->SetColor(active ? D2D1::ColorF(0.04f, 0.52f, 1.0f, 1)
                           : Fg(0.18f));
    target->FillEllipse({center, circle, circle}, brush);
    brush->SetColor(Fg(0.95f));
    target->DrawText(&glyph, 1, m_iconFormat.get(),
                     {center.x - circle, center.y - circle, center.x + circle,
                      center.y + circle},
                     brush);

    const float textLeft = center.x + circle + std::round(9 * s);
    const float middle = (rect.top + rect.bottom) / 2;
    // Long names scroll instead of being cut.
    DrawMarquee(target, brush, title, m_titleFormat.get(),
                {textLeft, middle - std::round(17 * s),
                 rect.right - std::round(8 * s), middle});
    brush->SetColor(Fg(0.55f));
    DrawMarquee(target, brush, subtitle, m_subtitleFormat.get(),
                {textLeft, middle, rect.right - std::round(8 * s),
                 middle + std::round(16 * s)});
}

// Text that doesn't fit scrolls to its end and back, with a pause at each
// end, like on macOS; text that fits is drawn as it is. Uses the brush's
// color.
void ControlPanel::DrawMarquee(ID2D1RenderTarget* target,
                               ID2D1SolidColorBrush* brush,
                               const std::wstring& text,
                               IDWriteTextFormat* format,
                               const D2D1_RECT_F& rect,
                               bool centered) {
    if (text.empty() || !format) {
        return;
    }
    const float width = rect.right - rect.left;
    const float height = rect.bottom - rect.top;
    winrt::com_ptr<IDWriteTextLayout> layout;
    float textWidth = 0;
    for (const auto& cached : m_marqueeLayouts) {
        // The size too: a format made again (another scaling) can have the
        // same address.
        if (cached.format == format && cached.height == height &&
            cached.fontSize == format->GetFontSize() && cached.text == text) {
            layout = cached.layout;
            textWidth = cached.width;
            break;
        }
    }
    if (!layout) {
        if (FAILED(m_dwrite->CreateTextLayout(text.c_str(),
                                              (UINT32)text.size(), format,
                                              10000, height, layout.put()))) {
            return;
        }
        layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        DWRITE_TEXT_METRICS metrics;
        layout->GetMetrics(&metrics);
        textWidth = metrics.widthIncludingTrailingWhitespace;
        // Texts change (like a network's name): the old ones go.
        if (m_marqueeLayouts.size() >= 96) {
            m_marqueeLayouts.clear();
        }
        m_marqueeLayouts.push_back(
            {text, format, format->GetFontSize(), height, layout, textWidth});
    }
    float x = rect.left;
    if (textWidth <= width) {
        if (centered) {
            x += (width - textWidth) / 2;
        }
    } else {
        // Pause, scroll to the end, pause, scroll back.
        m_marqueeActive = true;
        const double distance = textWidth - width;
        const double speed = 30 * m_scale;
        const double pause = 1.5;
        const double travel = distance / speed;
        const double cycle = (pause + travel) * 2;
        const double t = std::fmod(NowSeconds() - m_marqueeStart, cycle);
        double shift;
        if (t < pause) {
            shift = 0;
        } else if (t < pause + travel) {
            shift = (t - pause) * speed;
        } else if (t < pause * 2 + travel) {
            shift = distance;
        } else {
            shift = distance - (t - pause * 2 - travel) * speed;
        }
        x -= (float)std::clamp(shift, 0.0, distance);
    }
    target->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_ALIASED);
    target->DrawTextLayout({x, rect.top}, layout.get(), brush);
    target->PopAxisAlignedClip();
}

// Scrolling texts move while the control center is open; so do switches,
// the controls while editing (sliding to their places), and
// removed ones shrinking away.
bool ControlPanel::AnimateContents(double dt) {
    // Scrolling text never stops (see m_idleMotion).
    const bool idle = m_marqueeActive;
    bool moving = false;
    if (SpringTowards(m_viewIn, m_viewInVelocity, 1, 0.42, dt, 0.82)) {
        moving = true;
    }
    if (SpringTowards(m_editAmount, m_editAmountVelocity, m_editing ? 1 : 0,
                      0.32, dt)) {
        moving = true;
    }
    for (auto& knob : m_knobs) {
        if (SpringTowards(knob.value, knob.velocity, knob.target, 0.3, dt,
                          kBounceDamping)) {
            moving = true;
        }
    }
    const double style = g_settings.islandBar ? 1 : 0;
    if (m_styleKnob < 0) {
        m_styleKnob = style;
    }
    if (SpringTowards(m_styleKnob, m_styleKnobVelocity, style, 0.35, dt,
                      kBounceDamping)) {
        moving = true;
    }
    for (auto& motion : m_motions) {
        if (m_dragMoved && (int)motion.id == m_dragId) {
            // Under the mouse.
            motion.x = m_dragPoint.x - m_dragGrab.x;
            motion.y = m_dragPoint.y - m_dragGrab.y;
            motion.vx = motion.vy = 0;
            moving = true;
            continue;
        }
        for (const auto& [id, rect] : m_controlRects) {
            if (id != motion.id) {
                continue;
            }
            if (SpringTowards(motion.x, motion.vx, rect.left, 0.42, dt, 0.8)) {
                moving = true;
            }
            if (SpringTowards(motion.y, motion.vy, rect.top, 0.42, dt, 0.8)) {
                moving = true;
            }
        }
        if (SpringTowards(motion.scale, motion.vscale, 1, 0.38, dt,
                          kBounceDamping)) {
            moving = true;
        }
    }
    for (auto& ghost : m_ghosts) {
        if (SpringTowards(ghost.amount, ghost.velocity, 0, 0.26, dt)) {
            moving = true;
        }
    }
    std::erase_if(m_ghosts,
                  [](const Ghost& ghost) { return ghost.amount <= 0; });
    m_idleMotion = idle && !moving;
    return moving || idle;
}

float ControlPanel::KnobFor(int key, bool on) {
    const double target = on ? 1 : 0;
    for (auto& knob : m_knobs) {
        if (knob.key == key) {
            if (knob.target != target) {
                knob.target = target;
                StartIslandAnimation();
            }
            return (float)knob.value;
        }
    }
    m_knobs.push_back({key, target, 0, target});
    return (float)target;
}

// A small button: an icon in a circle (blue when it's on), its name under it.
void ControlPanel::DrawShortcut(ID2D1RenderTarget* target,
                                ID2D1SolidColorBrush* brush,
                                const D2D1_RECT_F& rect,
                                WCHAR glyph,
                                PCWSTR name,
                                bool active,
                                bool hover) {
    const float s = m_scale;
    const float tileRadius = std::round(14 * s);
    brush->SetColor(Fg(hover ? 0.14f : 0.08f));
    target->FillRoundedRectangle({rect, tileRadius, tileRadius}, brush);
    const float circle = std::round(14 * s);
    const D2D1_POINT_2F center{(rect.left + rect.right) / 2,
                               rect.top + std::round(8 * s) + circle};
    brush->SetColor(active ? D2D1::ColorF(0.04f, 0.52f, 1.0f, 1)
                           : Fg(0.18f));
    target->FillEllipse({center, circle, circle}, brush);
    brush->SetColor(Fg(0.95f));
    target->DrawText(&glyph, 1, m_iconFormat.get(),
                     {center.x - circle, center.y - circle, center.x + circle,
                      center.y + circle},
                     brush);
    brush->SetColor(Fg(0.75f));
    DrawMarquee(target, brush, name, m_smallFormat.get(),
                {rect.left + std::round(4 * s), center.y + circle,
                 rect.right - std::round(4 * s),
                 rect.bottom - std::round(2 * s)},
                true);
}

// A thick slider like on macOS: filled up to the level, with a round knob,
// and optionally an icon inside its start.
void ControlPanel::DrawSlider(ID2D1RenderTarget* target,
                              ID2D1SolidColorBrush* brush,
                              const D2D1_RECT_F& rect,
                              float level,
                              WCHAR glyph,
                              bool hoverIcon) {
    const float radius = (rect.bottom - rect.top) / 2;
    brush->SetColor(Fg(0.16f));
    target->FillRoundedRectangle({rect, radius, radius}, brush);
    const float knob = radius * 2;
    const float fillRight =
        rect.left + knob + (rect.right - rect.left - knob) *
                               std::clamp(level, 0.0f, 1.0f);
    brush->SetColor(Fg(0.92f));
    target->FillRoundedRectangle(
        {{rect.left, rect.top, fillRight, rect.bottom}, radius, radius}, brush);
    brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
    target->FillEllipse(
        {{fillRight - radius, rect.top + radius}, radius - 1, radius - 1},
        brush);
    if (glyph) {
        brush->SetColor(Bg(hoverIcon ? 0.95f : 0.7f));
        target->DrawText(&glyph, 1, m_iconFormat.get(),
                         {rect.left, rect.top, rect.left + knob * 1.2f,
                          rect.bottom},
                         brush);
    }
}

// A switch like on macOS: blue with the knob on the right when on.
void ControlPanel::DrawToggle(ID2D1RenderTarget* target,
                              ID2D1SolidColorBrush* brush,
                              const D2D1_RECT_F& rect,
                              float on) {
    const float radius = (rect.bottom - rect.top) / 2;
    // The track goes from gray to blue; the knob slides, a bit springy.
    const float t = std::clamp(on, 0.0f, 1.0f);
    const D2D1_COLOR_F off = Fg(0.22f);
    brush->SetColor(D2D1::ColorF(off.r + (0.04f - off.r) * t,
                                 off.g + (0.52f - off.g) * t,
                                 off.b + (1.0f - off.b) * t,
                                 off.a + (1.0f - off.a) * t));
    target->FillRoundedRectangle({rect, radius, radius}, brush);
    const float knob = radius - std::round(2 * m_scale);
    const float travel = rect.right - rect.left - radius * 2;
    brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
    target->FillEllipse({{rect.left + radius + travel * std::clamp(on, -0.1f, 1.1f),
                          rect.top + radius},
                         knob,
                         knob},
                        brush);
}

void ControlPanel::DrawStatus(ID2D1RenderTarget* target,
                              ID2D1SolidColorBrush* brush) {
    const std::wstring status = StatusText();
    if (status.empty()) {
        return;
    }
    brush->SetColor(Fg(0.55f));
    target->DrawText(status.c_str(), (UINT32)status.size(),
                     m_linkFormat.get(), m_status, brush);
}

void ControlPanel::DrawHeader(ID2D1RenderTarget* target,
                              ID2D1SolidColorBrush* brush,
                              PCWSTR title,
                              int toggle) {
    if (m_hover.part == Part::Back) {
        const float inset = std::round(6 * m_scale);
        const D2D1_RECT_F back{m_back.left + inset, m_back.top + inset,
                               m_back.right - inset, m_back.bottom - inset};
        const float radius = (back.bottom - back.top) / 2;
        brush->SetColor(Fg(0.14f));
        target->FillRoundedRectangle({back, radius, radius}, brush);
    }
    brush->SetColor(Fg(0.95f));
    target->DrawText(&kGlyphBack, 1, m_iconFormat.get(), m_back, brush);
    const float titleRight =
        toggle >= 0 ? m_toggle.left - std::round(8 * m_scale)
                    : (float)m_size.cx - m_back.left;
    target->DrawText(title, (UINT32)wcslen(title), m_sectionFormat.get(),
                     {m_back.right + std::round(4 * m_scale), m_back.top,
                      titleRight, m_back.bottom},
                     brush);
    if (toggle >= 0) {
        DrawToggle(target, brush, m_toggle,
                   KnobFor(1000 + (int)m_view, toggle == 1));
    }
}

void ControlPanel::DrawWifi(ID2D1RenderTarget* target,
                            ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    DrawHeader(target, brush, L"Wi-Fi", m_wifiRadio);
    const float icon = std::round(36 * s);
    for (size_t i = 0; i < m_networks.size() && i < m_rows.size(); i++) {
        const WifiNetwork& network = m_networks[i];
        const D2D1_RECT_F& row = m_rows[i];
        const bool hover =
            (m_hover.part == Part::WifiRow || m_hover.part == Part::Disconnect) &&
            m_hover.index == (int)i;
        if (hover || network.connected) {
            const float radius = std::round(10 * s);
            brush->SetColor(Fg(hover ? 0.14f : 0.08f));
            target->FillRoundedRectangle({row, radius, radius}, brush);
        }
        // The signal, blue when connected.
        const WCHAR glyph = kGlyphWifi[std::clamp(network.signal / 25, 0, 3)];
        brush->SetColor(network.connected ? D2D1::ColorF(0.04f, 0.52f, 1.0f, 1)
                                          : Fg(0.95f));
        target->DrawText(&glyph, 1, m_iconFormat.get(),
                         {row.left, row.top, row.left + icon, row.bottom},
                         brush);
        const float right =
            row.right - (network.connected ? std::round(100 * s) : icon);
        brush->SetColor(Fg(0.95f));
        target->DrawText(network.name.c_str(), (UINT32)network.name.size(),
                         m_subtitleFormat.get(),
                         {row.left + icon, row.top, right, row.bottom}, brush);
        if (network.connected) {
            PCWSTR disconnect = Tr(L"Disconnect", L"Desconectar");
            brush->SetColor(Fg(m_hover.part == Part::Disconnect && m_hover.index == (int)i
                    ? 0.95f
                    : 0.5f));
            target->DrawText(disconnect, (UINT32)wcslen(disconnect),
                             m_linkFormat.get(),
                             {right, row.top, row.right - std::round(8 * s),
                              row.bottom},
                             brush);
        } else if (network.secure && network.profile.empty()) {
            brush->SetColor(Fg(0.5f));
            target->DrawText(&kGlyphLock, 1, m_iconFormat.get(),
                             {row.right - icon, row.top, row.right,
                              row.bottom},
                             brush);
        }
    }
    DrawStatus(target, brush);
    if (m_hotspot >= 0 && m_hotspotRow.bottom > m_hotspotRow.top) {
        DrawRowToggle(target, brush, m_hotspotRow, kGlyphHotspot,
                      Tr(L"Mobile hotspot", L"Hotspot móvel"), m_hotspot == 1,
                      m_hover.part == Part::HotspotToggle);
    }
    if (m_airplane >= 0 && m_airplaneRow.bottom > m_airplaneRow.top) {
        DrawRowToggle(target, brush, m_airplaneRow, kGlyphAirplane,
                      Tr(L"Airplane mode", L"Modo avião"), m_airplane == 1,
                      m_hover.part == Part::AirplaneToggle);
    }
}

// A row with an icon, a name and a switch on the right.
void ControlPanel::DrawRowToggle(ID2D1RenderTarget* target,
                                 ID2D1SolidColorBrush* brush,
                                 const D2D1_RECT_F& row,
                                 WCHAR glyph,
                                 PCWSTR name,
                                 bool on,
                                 bool hover) {
    const float s = m_scale;
    if (hover) {
        const float radius = std::round(10 * s);
        brush->SetColor(Fg(0.08f));
        target->FillRoundedRectangle({row, radius, radius}, brush);
    }
    const float icon = std::round(36 * s);
    brush->SetColor(Fg(0.95f));
    target->DrawText(&glyph, 1, m_iconFormat.get(),
                     {row.left, row.top, row.left + icon, row.bottom}, brush);
    const float toggleWidth = std::round(36 * s);
    const float toggleHeight = std::round(20 * s);
    const D2D1_RECT_F toggle{
        row.right - std::round(8 * s) - toggleWidth,
        std::round((row.top + row.bottom - toggleHeight) / 2),
        row.right - std::round(8 * s),
        std::round((row.top + row.bottom - toggleHeight) / 2) + toggleHeight};
    brush->SetColor(Fg(0.95f));
    target->DrawText(name, (UINT32)wcslen(name), m_subtitleFormat.get(),
                     {row.left + icon, row.top, toggle.left - std::round(8 * s),
                      row.bottom},
                     brush);
    DrawToggle(target, brush, toggle, KnobFor(2000 + glyph, on));
}

void ControlPanel::DrawProject(ID2D1RenderTarget* target,
                               ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    DrawHeader(target, brush, Tr(L"Project", L"Projetar"));
    static const int kModes[] = {
        DISPLAYCONFIG_TOPOLOGY_INTERNAL, DISPLAYCONFIG_TOPOLOGY_CLONE,
        DISPLAYCONFIG_TOPOLOGY_EXTEND, DISPLAYCONFIG_TOPOLOGY_EXTERNAL};
    const PCWSTR names[] = {
        Tr(L"PC screen only", L"Somente tela do PC"),
        Tr(L"Duplicate", L"Duplicar"), Tr(L"Extend", L"Estender"),
        Tr(L"Second screen only", L"Somente segunda tela")};
    const float icon = std::round(36 * s);
    for (size_t i = 0; i < 4 && i < m_rows.size(); i++) {
        const D2D1_RECT_F& row = m_rows[i];
        const bool hover =
            m_hover.part == Part::ProjectRow && m_hover.index == (int)i;
        const bool current = m_displayMode == kModes[i];
        if (hover || current) {
            const float radius = std::round(10 * s);
            brush->SetColor(Fg(hover ? 0.14f : 0.08f));
            target->FillRoundedRectangle({row, radius, radius}, brush);
        }
        brush->SetColor(Fg(0.95f));
        target->DrawText(&kGlyphProject, 1, m_iconFormat.get(),
                         {row.left, row.top, row.left + icon, row.bottom},
                         brush);
        target->DrawText(names[i], (UINT32)wcslen(names[i]),
                         m_subtitleFormat.get(),
                         {row.left + icon, row.top, row.right - icon,
                          row.bottom},
                         brush);
        if (current) {
            brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
            target->DrawText(&kGlyphCheck, 1, m_iconFormat.get(),
                             {row.right - icon, row.top, row.right,
                              row.bottom},
                             brush);
        }
    }
}

// A theme's color, for its swatch.
D2D1_COLOR_F ThemeSwatchColor(int index) {
    switch ((ThemeId)index) {
        case ThemeId::Graphite:
            return D2D1::ColorF(0.17f, 0.17f, 0.19f, 1);
        case ThemeId::Light:
            return D2D1::ColorF(0.96f, 0.96f, 0.97f, 1);
        case ThemeId::Accent: {
            DWORD color = 0;
            BOOL opaque = FALSE;
            if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
                return D2D1::ColorF(((color >> 16) & 0xFF) / 255.0f * 0.55f,
                                    ((color >> 8) & 0xFF) / 255.0f * 0.55f,
                                    (color & 0xFF) / 255.0f * 0.55f, 1);
            }
            return D2D1::ColorF(0.1f, 0.25f, 0.45f, 1);
        }
        case ThemeId::Glass:
            return D2D1::ColorF(0.19f, 0.19f, 0.21f, 0.6f);
        default:
            return D2D1::ColorF(0, 0, 0, 1);
    }
}

PCWSTR ThemeName(int index) {
    switch ((ThemeId)index) {
        case ThemeId::Graphite:
            return Tr(L"Graphite", L"Grafite");
        case ThemeId::Light:
            return Tr(L"Light", L"Claro");
        case ThemeId::Accent:
            return Tr(L"Accent color", L"Cor de destaque");
        case ThemeId::Glass:
            return Tr(L"Glass", L"Vidro");
        default:
            return Tr(L"Black", L"Preto");
    }
}

// A little screen with the 9 ready places as dots: the chosen one blue, or,
// for a place of its own (dragged there), a blue dot where it is.
void ControlPanel::DrawPlaceGrid(ID2D1RenderTarget* target,
                                 ID2D1SolidColorBrush* brush,
                                 const D2D1_RECT_F& grid,
                                 double x,
                                 double y,
                                 int hover,
                                 float opacity) {
    const float s = m_scale;
    const float radius = std::round(5 * s);
    brush->SetColor(Fg(0.1f * opacity));
    target->FillRoundedRectangle({grid, radius, radius}, brush);
    brush->SetColor(Fg(0.25f * opacity));
    target->DrawRoundedRectangle(
        {{grid.left + 0.5f, grid.top + 0.5f, grid.right - 0.5f,
          grid.bottom - 0.5f},
         radius,
         radius},
        brush, 1);
    const float inset = std::round(6 * s);
    auto at = [&](double fx, double fy) {
        return D2D1_POINT_2F{
            grid.left + inset + (float)fx * (grid.right - grid.left - inset * 2),
            grid.top + inset + (float)fy * (grid.bottom - grid.top - inset * 2)};
    };
    bool ready = false;
    for (int i = 0; i < 9; i++) {
        const double fx = (i % 3) / 2.0;
        const double fy = (i / 3) / 2.0;
        const bool chosen = std::fabs(fx - x) < 0.01 && std::fabs(fy - y) < 0.01;
        ready = ready || chosen;
        const float dot = (chosen ? 3.2f : 2.2f) * s *
                          (hover == i ? 1.35f : 1.0f);
        brush->SetColor(chosen ? D2D1::ColorF(0.04f, 0.52f, 1.0f, opacity)
                               : Fg((hover == i ? 0.8f : 0.4f) * opacity));
        target->FillEllipse({at(fx, fy), dot, dot}, brush);
    }
    // Somewhere of its own.
    if (!ready) {
        brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, opacity));
        target->FillEllipse({at(x, y), 3.2f * s, 3.2f * s}, brush);
    }
}

int ControlPanel::PlaceDotAt(const D2D1_RECT_F& grid, POINT pt) {
    if (!PointInRect(pt, grid)) {
        return -1;
    }
    const int column = std::clamp(
        (int)((pt.x - grid.left) * 3 / (grid.right - grid.left)), 0, 2);
    const int row = std::clamp(
        (int)((pt.y - grid.top) * 3 / (grid.bottom - grid.top)), 0, 2);
    return row * 3 + column;
}

BOOL CALLBACK ListOpenAppsProc(HWND hWnd, LPARAM lParam) {
    auto* apps = reinterpret_cast<std::vector<std::wstring>*>(lParam);
    if (apps->size() >= 20 || !IsWindowVisible(hWnd) ||
        GetWindow(hWnd, GW_OWNER) ||
        (GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
        !GetWindowTextLength(hWnd)) {
        return TRUE;
    }
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                          sizeof(cloaked));
    if (cloaked) {
        return TRUE;
    }
    // Not the island's own windows.
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    WCHAR className[64] = L"";
    GetClassName(hWnd, className, ARRAYSIZE(className));
    if (processId == GetCurrentProcessId() &&
        (_wcsicmp(className, kIslandClassName) == 0 ||
         _wcsicmp(className, kPanelClassName) == 0)) {
        return TRUE;
    }
    std::wstring appId = WindowAppKey(hWnd);
    if (appId.empty() || FindAppPlace(appId) ||
        std::find(apps->begin(), apps->end(), appId) != apps->end()) {
        return TRUE;
    }
    apps->push_back(std::move(appId));
    return TRUE;
}

// The apps open now that have no place of their own yet.
void ControlPanel::ListOpenApps() {
    m_openApps.clear();
    EnumWindows(
        ListOpenAppsProc,
        (LPARAM)&m_openApps);
}

// The apps with a place of their own, and the open ones to add.
void ControlPanel::DrawAppPlaces(ID2D1RenderTarget* target,
                                 ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    DrawHeader(target, brush, Tr(L"Place per app", L"Posição por app"));
    for (size_t i = 0; i < m_appPlaceRows.size() && i < g_appPlaces.size();
         i++) {
        const D2D1_RECT_F& row = m_appPlaceRows[i];
        const IslandPlace& place = g_appPlaces[i];
        const bool hover = (m_hover.part == Part::AppPlaceDot &&
                            m_hover.index / 9 == (int)i) ||
                           (m_hover.part == Part::AppPlaceRemove &&
                            m_hover.index == (int)i);
        if (hover) {
            const float radius = std::round(10 * s);
            brush->SetColor(Fg(0.06f));
            target->FillRoundedRectangle({row, radius, radius}, brush);
        }
        brush->SetColor(Fg(0.92f));
        DrawMarquee(target, brush, AppDisplayName(place.appId),
                    m_subtitleFormat.get(),
                    {row.left + std::round(10 * s), row.top,
                     m_appPlaceGrids[i].left - std::round(8 * s), row.bottom});
        DrawPlaceGrid(target, brush, m_appPlaceGrids[i], place.x, place.y,
                      m_hover.part == Part::AppPlaceDot &&
                              m_hover.index / 9 == (int)i
                          ? m_hover.index % 9
                          : -1,
                      1);
        const WCHAR close = kGlyphEventDelete;
        brush->SetColor(Fg(m_hover.part == Part::AppPlaceRemove &&
                                   m_hover.index == (int)i
                               ? 0.95f
                               : 0.45f));
        target->DrawText(&close, 1, m_iconFormat.get(), m_appPlaceRemoves[i],
                         brush);
    }
    PCWSTR add = m_openApps.empty()
                     ? Tr(L"Open an app to give it a place",
                          L"Abra um app para dar uma posição a ele")
                     : Tr(L"Add an open app", L"Adicionar um app aberto");
    brush->SetColor(Fg(0.5f));
    target->DrawText(add, (UINT32)wcslen(add), m_smallFormat.get(),
                     m_addAppsHeader, brush);
    for (size_t i = 0; i < m_openAppChips.size() && i < m_openApps.size();
         i++) {
        const D2D1_RECT_F& chip = m_openAppChips[i];
        const bool hover =
            m_hover.part == Part::AppPlaceAdd && m_hover.index == (int)i;
        const float radius = std::round(10 * s);
        brush->SetColor(Fg(hover ? 0.14f : 0.07f));
        target->FillRoundedRectangle({chip, radius, radius}, brush);
        brush->SetColor(Fg(0.85f));
        const std::wstring name = L"+  " + AppDisplayName(m_openApps[i]);
        DrawMarquee(target, brush, name, m_smallFormat.get(),
                    {chip.left + std::round(10 * s), chip.top,
                     chip.right - std::round(8 * s), chip.bottom});
    }
    PCWSTR hint =
        g_settings.islandFreeDrag
            ? Tr(L"The pill moves there while the app is in front. "
                 L"Dragging it with the app in front changes it too.",
                 L"A pílula vai para lá enquanto o app está na frente. "
                 L"Arrastar a pílula com o app na frente também muda.")
            : Tr(L"The pill moves there while the app is in front.",
                 L"A pílula vai para lá enquanto o app está na frente.");
    brush->SetColor(Fg(0.45f));
    DrawMarquee(target, brush, hint, m_smallFormat.get(), m_status, true);
}

// The island's own options: its style, its colors, and what it shows.
void ControlPanel::SetIslandSizeAt(POINT pt, bool keep) {
    const float inset = std::round(10 * m_scale);
    const float left = m_sizeSlider.left + inset;
    const float right = m_sizeSlider.right - inset;
    if (right <= left) {
        return;
    }
    const float fraction = std::clamp((pt.x - left) / (right - left), 0.0f, 1.0f);
    const int steps = (kIslandSizeMax - kIslandSizeMin) / kIslandSizeStep;
    SetIslandSize(kIslandSizeMin + (int)std::lround(fraction * steps) *
                                       kIslandSizeStep,
                  keep);
}

void ControlPanel::DrawSettings(ID2D1RenderTarget* target,
                                ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    DrawHeader(target, brush, Tr(L"Island settings", L"Configurações da ilha"));
    const float icon = std::round(36 * s);
    auto label = [&](const D2D1_RECT_F& row, WCHAR glyph, PCWSTR name,
                     float right, float opacity) {
        brush->SetColor(Fg(0.95f * opacity));
        target->DrawText(&glyph, 1, m_iconFormat.get(),
                         {row.left, row.top, row.left + icon, row.bottom},
                         brush);
        DrawMarquee(target, brush, name, m_subtitleFormat.get(),
                    {row.left + icon, row.top, right - std::round(8 * s),
                     row.bottom});
    };

    // The style: the pill or the bar, the chosen one lit (sliding).
    {
        const IslandPrefInfo& info = kIslandPrefs[(int)IslandPref::Bar];
        label(m_styleRow, info.glyph, Tr(info.english, info.portuguese),
              m_styleSegments[0].left, 1);
        const D2D1_RECT_F track{m_styleSegments[0].left, m_styleSegments[0].top,
                                m_styleSegments[1].right,
                                m_styleSegments[1].bottom};
        const float radius = (track.bottom - track.top) / 2;
        brush->SetColor(Fg(0.08f));
        target->FillRoundedRectangle({track, radius, radius}, brush);
        const float knob = (float)std::clamp(
            m_styleKnob < 0 ? (g_settings.islandBar ? 1.0 : 0.0) : m_styleKnob,
            -0.05, 1.05);
        const float segmentWidth =
            m_styleSegments[0].right - m_styleSegments[0].left;
        const float inset = std::round(2 * s);
        const D2D1_RECT_F lit{
            m_styleSegments[0].left + segmentWidth * knob + inset,
            track.top + inset,
            m_styleSegments[0].right + segmentWidth * knob - inset,
            track.bottom - inset};
        const float litRadius = (lit.bottom - lit.top) / 2;
        brush->SetColor(Fg(m_hover.part == Part::StyleSegment ? 0.26f : 0.2f));
        target->FillRoundedRectangle({lit, litRadius, litRadius}, brush);
        for (int i = 0; i < 2; i++) {
            PCWSTR name = i == 0 ? Tr(L"Pill", L"Pílula") : Tr(L"Bar", L"Barra");
            const bool chosen = (g_settings.islandBar ? 1 : 0) == i;
            brush->SetColor(Fg(chosen ? 0.95f : 0.55f));
            target->DrawText(name, (UINT32)wcslen(name), m_linkFormat.get(),
                             m_styleSegments[i], brush);
        }
    }

    // The colors: a swatch each, the chosen one ringed in blue.
    {
        const IslandPrefInfo& info = kIslandPrefs[(int)IslandPref::Theme];
        const std::wstring name = std::wstring(Tr(info.english, info.portuguese)) +
                                  L" \u00B7 " + ThemeName(g_themeIndex);
        label(m_themeRow, info.glyph, name.c_str(), m_swatches[0].left, 1);
        for (int i = 0; i < (int)ThemeId::Count; i++) {
            const D2D1_RECT_F& r = m_swatches[i];
            const D2D1_POINT_2F center{(r.left + r.right) / 2,
                                       (r.top + r.bottom) / 2};
            const float radius = (r.right - r.left) / 2;
            const bool hover =
                m_hover.part == Part::ThemeSwatch && m_hover.index == i;
            const float grow = hover ? std::round(1.5f * s) : 0;
            brush->SetColor(ThemeSwatchColor(i));
            target->FillEllipse({center, radius + grow, radius + grow}, brush);
            brush->SetColor(Fg(0.3f));
            target->DrawEllipse({center, radius + grow, radius + grow}, brush,
                                1);
            if (i == g_themeIndex) {
                brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
                target->DrawEllipse(
                    {center, radius + std::round(3.5f * s),
                     radius + std::round(3.5f * s)},
                    brush, 2);
            }
        }
    }

    // The place, for all apps (not for the bar, always at the top).
    {
        const bool bar = g_settings.islandBar;
        const float opacity = bar ? 0.4f : 1.0f;
        label(m_placeRow, 0xE81E,
              bar ? Tr(L"Place (the bar is at the top)",
                       L"Posição (a barra fica no topo)")
                  : g_settings.islandFreeDrag
                      ? Tr(L"Place (or drag the pill)",
                           L"Posição (ou arraste a pílula)")
                      : Tr(L"Place", L"Posição"),
              m_placeGrid.left, opacity);
        DrawPlaceGrid(target, brush, m_placeGrid, g_islandPlace.x,
                      g_islandPlace.y,
                      m_hover.part == Part::PlaceDot ? m_hover.index : -1,
                      opacity);
        if (!bar && m_hover.part == Part::AppPlacesRow) {
            const float radius = std::round(10 * s);
            brush->SetColor(Fg(0.08f));
            target->FillRoundedRectangle({m_appPlacesRow, radius, radius},
                                         brush);
        }
        const std::wstring perApp =
            std::wstring(Tr(L"Place per app", L"Posição por app")) +
            (g_appPlaces.empty()
                 ? std::wstring()
                 : L"  (" + std::to_wstring(g_appPlaces.size()) + L")");
        label(m_appPlacesRow, 0xE8A9, perApp.c_str(),
              m_appPlacesRow.right - std::round(24 * s), opacity);
        brush->SetColor(Fg(0.5f * opacity));
        const WCHAR chevron = kGlyphNext;
        target->DrawText(&chevron, 1, m_iconFormat.get(),
                         {m_appPlacesRow.right - std::round(28 * s),
                          m_appPlacesRow.top, m_appPlacesRow.right,
                          m_appPlacesRow.bottom},
                         brush);
    }

    // The size: its percentage, a slider (the knob in steps), and a button
    // to restore it, dimmed while it's the usual size.
    {
        const int size = g_settings.islandSize;
        const std::wstring name = std::wstring(Tr(L"Size", L"Tamanho")) +
                                  L" \u00B7 " + std::to_wstring(size) + L"%";
        label(m_sizeRow, 0xE740, name.c_str(), m_sizeSlider.left, 1);
        const float inset = std::round(10 * s);
        const float middle = (m_sizeSlider.top + m_sizeSlider.bottom) / 2;
        const float thickness = std::round(4 * s);
        const D2D1_RECT_F track{m_sizeSlider.left + inset,
                                middle - thickness / 2,
                                m_sizeSlider.right - inset,
                                middle + thickness / 2};
        const float fraction = (float)(size - kIslandSizeMin) /
                               (kIslandSizeMax - kIslandSizeMin);
        const float knobX = track.left + (track.right - track.left) * fraction;
        brush->SetColor(Fg(0.15f));
        target->FillRoundedRectangle(
            {track, thickness / 2, thickness / 2}, brush);
        brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
        target->FillRoundedRectangle(
            {{track.left, track.top, knobX, track.bottom}, thickness / 2,
             thickness / 2},
            brush);
        // The usual size, marked on the track.
        const float usual =
            track.left + (track.right - track.left) *
                             (float)(kIslandSizeDefault - kIslandSizeMin) /
                             (kIslandSizeMax - kIslandSizeMin);
        brush->SetColor(Fg(0.35f));
        target->FillRectangle({usual - std::round(1 * s),
                               middle - std::round(6 * s),
                               usual + std::round(1 * s),
                               middle + std::round(6 * s)},
                              brush);
        const bool sliding = m_dragging && m_dragIndex == kDragIslandSize;
        const float knob = std::round(
            (sliding || m_hover.part == Part::SizeSlider ? 9 : 8) * s);
        brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
        target->FillEllipse({{knobX, middle}, knob, knob}, brush);
        brush->SetColor(D2D1::ColorF(0, 0, 0, 0.25f));
        target->DrawEllipse({{knobX, middle}, knob, knob}, brush, 1);

        const bool usualSize = size == kIslandSizeDefault;
        if (!usualSize && m_hover.part == Part::SizeReset) {
            const float radius = (m_sizeReset.right - m_sizeReset.left) / 2;
            brush->SetColor(Fg(0.12f));
            target->FillRoundedRectangle({m_sizeReset, radius, radius}, brush);
        }
        brush->SetColor(Fg(usualSize ? 0.25f : 0.85f));
        const WCHAR restore = 0xE72C;
        target->DrawText(&restore, 1, m_iconFormat.get(), m_sizeReset, brush);
    }

    // The switches, under their labels; one that depends on another being
    // on is dimmed while it's off.
    for (const auto& [item, row] : m_prefRows) {
        if (item < 0) {
            PCWSTR text = item == kPrefLabelItems
                              ? Tr(L"On the island", L"Na ilha")
                          : item == kPrefLabelClock
                              ? Tr(L"Clock (the calendar shows it all)",
                                   L"Relógio (o calendário mostra tudo)")
                          : item == kPrefLabelPlace
                              ? Tr(L"Moving it", L"Mover")
                              : Tr(L"Notifications", L"Notificações");
            brush->SetColor(Fg(0.5f));
            target->DrawText(text, (UINT32)wcslen(text), m_smallFormat.get(),
                             {row.left + std::round(6 * s), row.top + std::round(6 * s),
                              row.right, row.bottom},
                             brush);
            continue;
        }
        const IslandPref pref = (IslandPref)item;
        const IslandPrefInfo& info = kIslandPrefs[item];
        const bool disabled = PrefDisabled(pref);
        const bool hover = !disabled && m_hover.part == Part::SettingToggle &&
                           m_hover.index == item;
        if (hover) {
            const float radius = std::round(10 * s);
            brush->SetColor(Fg(0.08f));
            target->FillRoundedRectangle({row, radius, radius}, brush);
        }
        const float toggleWidth = std::round(36 * s);
        const float toggleHeight = std::round(20 * s);
        const D2D1_RECT_F toggle{
            row.right - std::round(8 * s) - toggleWidth,
            std::round((row.top + row.bottom - toggleHeight) / 2),
            row.right - std::round(8 * s),
            std::round((row.top + row.bottom - toggleHeight) / 2) +
                toggleHeight};
        const float opacity = disabled ? 0.4f : 1.0f;
        label(row, info.glyph, Tr(info.english, info.portuguese), toggle.left,
              opacity);
        winrt::com_ptr<ID2D1Layer> layer;
        if (disabled) {
            target->CreateLayer(nullptr, layer.put());
            target->PushLayer(
                D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                      D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                      D2D1::IdentityMatrix(), opacity),
                layer.get());
        }
        DrawToggle(target, brush, toggle,
                   KnobFor(100 + item, GetIslandPref(pref) != 0));
        if (layer) {
            target->PopLayer();
        }
    }
}

void ControlPanel::DrawAccessibility(ID2D1RenderTarget* target,
                                     ID2D1SolidColorBrush* brush) {
    DrawHeader(target, brush, Tr(L"Accessibility", L"Acessibilidade"));
    for (size_t i = 0; i < m_accessibility.size() && i < m_rows.size(); i++) {
        const AccessibilityItem& item = m_accessibility[i];
        DrawRowToggle(target, brush, m_rows[i], item.glyph, item.name, item.on,
                      m_hover.part == Part::AccessibilityRow &&
                          m_hover.index == (int)i);
    }
}

void ControlPanel::DrawPassword(ID2D1RenderTarget* target,
                                ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    DrawHeader(target, brush, m_joining.name.c_str());

    // The field: dots (or the text), a caret, and the eye to show it.
    const float radius = std::round(10 * s);
    brush->SetColor(Fg(0.10f));
    target->FillRoundedRectangle({m_field, radius, radius}, brush);
    brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
    target->DrawRoundedRectangle(
        {{m_field.left + 0.5f, m_field.top + 0.5f, m_field.right - 0.5f,
          m_field.bottom - 0.5f},
         radius,
         radius},
        brush, 1.5f);
    const D2D1_RECT_F textRect{m_field.left + std::round(12 * s), m_field.top,
                               m_eye.left, m_field.bottom};
    const std::wstring shown =
        m_showPassword ? m_password
                       : std::wstring(m_password.size(), L'\u2022');
    float caretX = textRect.left;
    if (shown.empty()) {
        PCWSTR placeholder = Tr(L"Password", L"Senha");
        brush->SetColor(Fg(0.4f));
        target->DrawText(placeholder, (UINT32)wcslen(placeholder),
                         m_subtitleFormat.get(), textRect, brush);
    } else {
        winrt::com_ptr<IDWriteTextLayout> layout;
        if (SUCCEEDED(m_dwrite->CreateTextLayout(
                shown.c_str(), (UINT32)shown.size(), m_subtitleFormat.get(),
                textRect.right - textRect.left,
                textRect.bottom - textRect.top, layout.put()))) {
            brush->SetColor(Fg(0.95f));
            target->DrawTextLayout({textRect.left, textRect.top},
                                   layout.get(), brush);
            DWRITE_TEXT_METRICS metrics;
            layout->GetMetrics(&metrics);
            caretX = std::min(textRect.left +
                                  metrics.widthIncludingTrailingWhitespace,
                              textRect.right);
        }
    }
    brush->SetColor(Fg(0.9f));
    const float middle = (m_field.top + m_field.bottom) / 2;
    target->FillRectangle({caretX + 1, middle - std::round(8 * s), caretX + 2.5f,
                           middle + std::round(8 * s)},
                          brush);
    const WCHAR eye = m_showPassword ? kGlyphHide : kGlyphShow;
    brush->SetColor(Fg(m_hover.part == Part::ShowPassword ? 0.95f : 0.55f));
    target->DrawText(&eye, 1, m_iconFormat.get(), m_eye, brush);

    // "Connect", blue.
    const float buttonRadius = (m_connect.bottom - m_connect.top) / 2;
    brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f,
                                 m_connecting                     ? 0.5f
                                 : m_hover.part == Part::Connect ? 0.85f
                                                                 : 1.0f));
    target->FillRoundedRectangle({m_connect, buttonRadius, buttonRadius},
                                 brush);
    PCWSTR connect = Tr(L"Connect", L"Conectar");
    brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
    target->DrawText(connect, (UINT32)wcslen(connect), m_linkFormat.get(),
                     m_connect, brush);
    DrawStatus(target, brush);
}

void ControlPanel::DrawBluetooth(ID2D1RenderTarget* target,
                                 ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    DrawHeader(target, brush, L"Bluetooth", m_bluetoothRadio);
    const float icon = std::round(36 * s);
    for (size_t i = 0; i < m_bluetoothDevices.size() && i < m_rows.size();
         i++) {
        const BluetoothDevice& device = m_bluetoothDevices[i];
        const D2D1_RECT_F& row = m_rows[i];
        const bool hover =
            m_hover.part == Part::BluetoothRow && m_hover.index == (int)i;
        if (hover) {
            const float radius = std::round(10 * s);
            brush->SetColor(Fg(0.08f));
            target->FillRoundedRectangle({row, radius, radius}, brush);
        }
        brush->SetColor(device.connected ? D2D1::ColorF(0.04f, 0.52f, 1.0f, 1)
                                         : Fg(0.95f));
        target->DrawText(&device.glyph, 1, m_iconFormat.get(),
                         {row.left, row.top, row.left + icon, row.bottom},
                         brush);
        const float right = row.right - std::round(96 * s);
        brush->SetColor(Fg(0.95f));
        target->DrawText(device.name.c_str(), (UINT32)device.name.size(),
                         m_subtitleFormat.get(),
                         {row.left + icon, row.top, right, row.bottom}, brush);
        if (device.connected) {
            PCWSTR connected = Tr(L"Connected", L"Conectado");
            brush->SetColor(Fg(0.5f));
            target->DrawText(connected, (UINT32)wcslen(connected),
                             m_linkFormat.get(),
                             {right, row.top, row.right - std::round(8 * s),
                              row.bottom},
                             brush);
        }
    }
    DrawStatus(target, brush);
}

WCHAR VolumeGlyphFor(float level, bool muted) {
    if (muted) {
        return kGlyphMute;
    }
    if (level <= 0.001f) {
        return kGlyphVolume[0];
    }
    return kGlyphVolume[std::clamp((int)(level * 3) + 1, 1, 3)];
}

// One control of the main view, at its place (sections at theirs).
ID2D1Bitmap* ControlPanel::GetControlTile(ID2D1RenderTarget* target,
                                          ID2D1SolidColorBrush* brush,
                                          ControlId id,
                                          const D2D1_RECT_F& rect,
                                          float margin) {
    const D2D1_SIZE_F size{
        std::ceil(rect.right - rect.left + margin * 2),
        std::ceil(rect.bottom - rect.top + margin * 2)};
    ControlTile* tile = nullptr;
    for (auto& existing : m_tiles) {
        if (existing.id == id) {
            tile = &existing;
        }
    }
    if (tile && tile->size.width == size.width &&
        tile->size.height == size.height) {
        return tile->bitmap.get();
    }
    winrt::com_ptr<ID2D1BitmapRenderTarget> tileTarget;
    if (FAILED(target->CreateCompatibleRenderTarget(size, tileTarget.put()))) {
        return nullptr;
    }
    tileTarget->SetTextAntialiasMode(target->GetTextAntialiasMode());
    tileTarget->BeginDraw();
    tileTarget->Clear(D2D1::ColorF(0, 0, 0, 0));
    tileTarget->SetTransform(D2D1::Matrix3x2F::Translation(
        margin - rect.left, margin - rect.top));
    DrawControl(tileTarget.get(), brush, id, rect);
    winrt::com_ptr<ID2D1Bitmap> bitmap;
    if (FAILED(tileTarget->EndDraw()) ||
        FAILED(tileTarget->GetBitmap(bitmap.put()))) {
        return nullptr;
    }
    if (!tile) {
        tile = &m_tiles.emplace_back();
        tile->id = id;
    }
    tile->size = size;
    tile->bitmap = std::move(bitmap);
    return tile->bitmap.get();
}

void ControlPanel::DrawControl(ID2D1RenderTarget* target,
                               ID2D1SolidColorBrush* brush,
                               ControlId id,
                               const D2D1_RECT_F& rect) {
    const float s = m_scale;
    const ControlInfo& info = GetControlInfo(id);
    const PCWSTR name = Tr(info.english, info.portuguese);
    // While editing, a section that isn't available here (no brightness
    // control, no battery) still shows its place.
    const bool missing =
        (id == ControlId::Display && m_display.right <= m_display.left) ||
        (id == ControlId::Sound && m_sound.right <= m_sound.left) ||
        (id == ControlId::Battery &&
         m_batteryTile.right <= m_batteryTile.left);
    if (missing) {
        const float radius = std::round(14 * s);
        brush->SetColor(Fg(0.08f));
        target->FillRoundedRectangle({rect, radius, radius}, brush);
        const float icon = std::round(40 * s);
        brush->SetColor(Fg(0.6f));
        target->DrawText(&info.glyph, 1, m_iconFormat.get(),
                         {rect.left, rect.top, rect.left + icon, rect.bottom},
                         brush);
        // Said only when it really isn't (a removed one shrinking away has
        // no place either).
        const bool unavailable =
            (id == ControlId::Display && m_brightness < 0) ||
            (id == ControlId::Battery && !m_hasBattery);
        const std::wstring text =
            unavailable ? std::wstring(name) + L"  ·  " +
                              Tr(L"Not available here", L"Indisponível aqui")
                        : std::wstring(name);
        brush->SetColor(Fg(0.6f));
        target->DrawText(text.c_str(), (UINT32)text.size(),
                         m_subtitleFormat.get(),
                         {rect.left + icon, rect.top, rect.right, rect.bottom},
                         brush);
        return;
    }
    switch (id) {
        case ControlId::Network: {
            // Wi-Fi with its name, or the cable.
            WCHAR networkGlyph = kGlyphEthernet;
            std::wstring networkName = Tr(L"Ethernet", L"Cabo");
            if (m_network.signal >= 0) {
                networkGlyph =
                    kGlyphWifi[std::clamp(m_network.signal / 25, 0, 3)];
                networkName = m_network.wifiName.empty()
                                  ? L"Wi-Fi"
                                  : m_network.wifiName;
            } else if (!m_network.connected) {
                networkGlyph = kGlyphNoInternet;
                networkName = Tr(L"Network", L"Rede");
            }
            std::wstring networkState =
                m_network.internet    ? Tr(L"Connected", L"Conectado")
                : m_network.connected ? Tr(L"No internet", L"Sem internet")
                                      : Tr(L"Disconnected", L"Desconectado");
            if (m_wifiRadio == 0 && m_network.signal < 0 &&
                !m_network.connected) {
                networkGlyph = kGlyphWifi[3];
                networkName = L"Wi-Fi";
                networkState = Tr(L"Off", L"Desligado");
            }
            DrawTile(target, brush, rect, networkGlyph,
                     m_network.internet ||
                         (m_wifiRadio == 1 && m_network.signal >= 0),
                     networkName.c_str(), networkState,
                     m_hover.part == Part::Network ||
                         m_hover.part == Part::WifiToggle);
            break;
        }
        case ControlId::Bluetooth:
            DrawTile(target, brush, rect, kGlyphBluetooth,
                     m_bluetoothRadio == 1, L"Bluetooth",
                     m_bluetoothRadio == 1   ? Tr(L"On", L"Ligado")
                     : m_bluetoothRadio == 0 ? Tr(L"Off", L"Desligado")
                     : m_bluetoothRadio == kRadioMissing
                         ? Tr(L"Unavailable", L"Indisponível")
                         : Tr(L"Devices", L"Dispositivos"),
                     m_hover.part == Part::Bluetooth ||
                         m_hover.part == Part::BluetoothToggle);
            break;
        case ControlId::DoNotDisturb:
            DrawTile(target, brush, rect, kGlyphMoon, m_doNotDisturb, name,
                     m_doNotDisturb ? Tr(L"On", L"Ligado")
                                    : Tr(L"Off", L"Desligado"),
                     m_hover.part == Part::DoNotDisturb ||
                         m_hover.part == Part::DndToggle);
            break;
        case ControlId::NightLight:
            DrawTile(target, brush, rect, kGlyphNightLight,
                     m_nightLight == 1, name,
                     m_nightLight == 1   ? Tr(L"On", L"Ligada")
                     : m_nightLight == 0 ? Tr(L"Off", L"Desligada")
                                         : Tr(L"Unavailable",
                                              L"Indisponível"),
                     m_hover.part == Part::NightLight && m_nightLight >= 0);
            break;
        case ControlId::Hotspot:
            DrawTile(target, brush, rect, kGlyphHotspot, m_hotspot == 1,
                     name,
                     m_hotspot == 1   ? Tr(L"On", L"Ligado")
                     : m_hotspot == 0 ? Tr(L"Off", L"Desligado")
                                      : Tr(L"Unavailable", L"Indisponível"),
                     m_hover.part == Part::HotspotToggle);
            break;
        case ControlId::Airplane:
            DrawTile(target, brush, rect, kGlyphAirplane, m_airplane == 1,
                     name,
                     m_airplane == 1   ? Tr(L"On", L"Ligado")
                     : m_airplane == 0 ? Tr(L"Off", L"Desligado")
                                       : Tr(L"Unavailable", L"Indisponível"),
                     m_hover.part == Part::AirplaneToggle);
            break;
        case ControlId::Display:
        case ControlId::Sound:
        case ControlId::Battery:
            // Below.
            break;
        default:
            DrawShortcut(target, brush, rect, info.glyph, name,
                         id == ControlId::DarkMode && m_darkMode,
                         m_hover.part == Part::Shortcut &&
                             m_hover.index == (int)id);
            break;
    }
    // The display: its brightness, like the sound.
    if (id == ControlId::Display && m_display.right > m_display.left) {
        const float sectionRadius = std::round(14 * s);
        brush->SetColor(Fg(0.08f));
        target->FillRoundedRectangle({m_display, sectionRadius, sectionRadius},
                                     brush);
        const float sectionInner = std::round(12 * s);
        const D2D1_RECT_F displayTitle{
            m_display.left + sectionInner, m_display.top + 6 * s,
            m_display.right - sectionInner, m_brightnessSlider.top - 4 * s};
        PCWSTR title = Tr(L"Display", L"Tela");
        brush->SetColor(Fg(0.95f));
        target->DrawText(title, (UINT32)wcslen(title), m_sectionFormat.get(),
                         displayTitle, brush);
        const std::wstring level = std::to_wstring(m_brightness) + L"%";
        brush->SetColor(Fg(0.55f));
        target->DrawText(level.c_str(), (UINT32)level.size(),
                         m_percentFormat.get(), displayTitle, brush);
        DrawSlider(target, brush, m_brightnessSlider, m_brightness / 100.0f,
                   kGlyphBrightness, false);
    }

    // Sound: title, percentage, the slider, the output device and the mixer.
    if (id == ControlId::Sound && m_sound.right > m_sound.left) {
        const float radius = std::round(14 * s);
        brush->SetColor(Fg(0.08f));
        target->FillRoundedRectangle({m_sound, radius, radius}, brush);
        const float inner = std::round(12 * s);
        brush->SetColor(Fg(0.95f));
        const D2D1_RECT_F titleRect{m_sound.left + inner, m_sound.top + 6 * s,
                                    m_sound.right - inner, m_slider.top - 4 * s};
        PCWSTR soundTitle = Tr(L"Sound", L"Som");
        target->DrawText(soundTitle, (UINT32)wcslen(soundTitle),
                         m_sectionFormat.get(), titleRect, brush);
        const std::wstring percent =
            m_hasVolume ? (m_muted ? std::wstring(Tr(L"Muted", L"Mudo"))
                                   : std::to_wstring((int)std::lround(
                                         m_volume * 100)) +
                                         L"%")
                        : L"";
        brush->SetColor(Fg(0.55f));
        target->DrawText(percent.c_str(), (UINT32)percent.size(),
                         m_percentFormat.get(), titleRect, brush);

        DrawSlider(target, brush, m_slider,
                   m_hasVolume && !m_muted ? m_volume : 0,
                   m_hasVolume ? VolumeGlyphFor(m_volume, m_muted) : kGlyphMute,
                   m_hover.part == Part::VolumeIcon);

        // The output device, opening the list of devices.
        const std::wstring device =
            (m_device.empty() ? std::wstring(Tr(L"Output", L"Saída"))
                              : m_device) +
            L"  ›";
        brush->SetColor(Fg(m_hover.part == Part::Device ? 0.9f : 0.55f));
        target->DrawText(device.c_str(), (UINT32)device.size(),
                         m_subtitleFormat.get(), m_deviceRow, brush);

        // The mixer button.
        const float buttonRadius = (m_mixerButton.bottom - m_mixerButton.top) / 2;
        brush->SetColor(Fg(m_hover.part == Part::MixerButton ? 0.2f : 0.1f));
        target->FillRoundedRectangle({m_mixerButton, buttonRadius, buttonRadius},
                                     brush);
        PCWSTR mixer = L"Mixer";
        brush->SetColor(Fg(0.9f));
        target->DrawText(mixer, (UINT32)wcslen(mixer), m_linkFormat.get(),
                         m_mixerButton, brush);
    }

    if (id == ControlId::Battery &&
        m_batteryTile.right > m_batteryTile.left) {
        const int tenth = (m_batteryPercent + 5) / 10;
        const WCHAR glyph =
            tenth >= 10 ? (m_charging ? (WCHAR)0xEA93 : (WCHAR)0xE83F)
                        : (WCHAR)((m_charging ? 0xE85A : 0xE850) + tenth);
        DrawTile(target, brush, m_batteryTile, glyph, m_charging,
                 (std::to_wstring(m_batteryPercent) + L"%").c_str(),
                 m_charging ? Tr(L"Charging", L"Carregando")
                            : Tr(L"On battery", L"Na bateria"),
                 m_hover.part == Part::Battery);
    }
}

// The "-" on a control's corner while editing, like on the iPhone.
void ControlPanel::DrawRemoveBadge(ID2D1RenderTarget* target,
                                   ID2D1SolidColorBrush* brush,
                                   const D2D1_RECT_F& rect,
                                   float amount) {
    if (amount <= 0.01f || !m_badgeFormat) {
        return;
    }
    const float s = m_scale;
    const float radius = std::round(9 * s) * std::clamp(amount, 0.0f, 1.1f);
    const D2D1_POINT_2F center{rect.left + std::round(3 * s),
                               rect.top + std::round(3 * s)};
    const float opacity = std::clamp(amount, 0.0f, 1.0f);
    const bool hover = m_hover.part == Part::EditRemove;
    brush->SetColor(D2D1::ColorF(hover ? 0.55f : 0.42f, hover ? 0.55f : 0.42f,
                                 hover ? 0.57f : 0.44f, opacity));
    target->FillEllipse({center, radius, radius}, brush);
    brush->SetColor(D2D1::ColorF(1, 1, 1, opacity));
    const WCHAR minus = 0xE738;
    target->DrawText(&minus, 1, m_badgeFormat.get(),
                     {center.x - radius, center.y - radius, center.x + radius,
                      center.y + radius},
                     brush);
}

// The chosen controls, each where it's shown (sliding to its place); while
// editing each has a "-", and the dragged one follows the mouse,
// a bit bigger. Below: the footer, or the controls to add and "Done".
void ControlPanel::DrawMain(ID2D1RenderTarget* target,
                           ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    const float edit = (float)std::clamp(m_editAmount, 0.0, 1.0);
    if (edit <= 0.001f) {
        m_tiles.clear();
    }
    D2D1_MATRIX_3X2_F base;
    target->GetTransform(&base);
    const D2D1::Matrix3x2F& baseMatrix =
        *D2D1::Matrix3x2F::ReinterpretBaseType(&base);

    // Removed ones, shrinking and fading away.
    for (const auto& ghost : m_ghosts) {
        const float amount = (float)std::clamp(ghost.amount, 0.0, 1.0);
        const D2D1_POINT_2F center{(ghost.rect.left + ghost.rect.right) / 2,
                                   (ghost.rect.top + ghost.rect.bottom) / 2};
        winrt::com_ptr<ID2D1Layer> layer;
        target->CreateLayer(nullptr, layer.put());
        target->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                  D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                  D2D1::IdentityMatrix(), amount),
            layer.get());
        const float scale = 0.6f + 0.4f * amount;
        target->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale, center) *
                             baseMatrix);
        DrawControl(target, brush, ghost.id, ghost.rect);
        target->SetTransform(base);
        target->PopLayer();
    }

    auto drawAt = [&](ControlId id, const D2D1_RECT_F& rect, bool dragged) {
        const Motion* motion = FindMotion(id);
        const float dx = motion ? (float)motion->x - rect.left : 0;
        const float dy = motion ? (float)motion->y - rect.top : 0;
        float scale = motion ? (float)motion->scale : 1;
        if (dragged) {
            scale *= 1.06f;
        }
        const D2D1_POINT_2F center{(rect.left + rect.right) / 2,
                                   (rect.top + rect.bottom) / 2};
        target->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale, center) *
                             D2D1::Matrix3x2F::Translation(dx, dy) *
                             baseMatrix);
        const float margin = std::round(4 * s);
        if (ID2D1Bitmap* tile = edit > 0.001f ? GetControlTile(target, brush,
                                                               id, rect, margin)
                                              : nullptr) {
            const D2D1_SIZE_F size = tile->GetSize();
            target->DrawBitmap(tile,
                               {rect.left - margin, rect.top - margin,
                                rect.left - margin + size.width,
                                rect.top - margin + size.height});
        } else {
            DrawControl(target, brush, id, rect);
        }
        DrawRemoveBadge(target, brush, rect, edit);
        target->SetTransform(base);
    };
    for (const auto& [id, rect] : m_controlRects) {
        if (m_dragMoved && (int)id == m_dragId) {
            continue;
        }
        drawAt(id, rect, false);
    }
    // The dragged one, on top.
    for (const auto& [id, rect] : m_controlRects) {
        if (m_dragMoved && (int)id == m_dragId) {
            drawAt(id, rect, true);
        }
    }

    if (m_editing) {
        // The controls to add.
        if (!m_addRects.empty()) {
            PCWSTR add = Tr(L"Add controls", L"Adicionar controles");
            brush->SetColor(Fg(0.5f * edit));
            target->DrawText(add, (UINT32)wcslen(add), m_smallFormat.get(),
                             m_addHeader, brush);
        }
        for (const auto& [id, rect] : m_addRects) {
            const ControlInfo& info = GetControlInfo(id);
            const bool hover =
                m_hover.part == Part::EditAdd && m_hover.index == (int)id;
            const float radius = std::round(10 * s);
            brush->SetColor(Fg((hover ? 0.14f : 0.07f) * edit));
            target->FillRoundedRectangle({rect, radius, radius}, brush);
            const float icon = std::round(28 * s);
            brush->SetColor(Fg(0.85f * edit));
            target->DrawText(&info.glyph, 1, m_iconFormat.get(),
                             {rect.left + std::round(2 * s), rect.top,
                              rect.left + icon, rect.bottom},
                             brush);
            brush->SetColor(Fg(0.85f * edit));
            DrawMarquee(target, brush, Tr(info.english, info.portuguese),
                        m_smallFormat.get(),
                        {rect.left + icon, rect.top,
                         rect.right - std::round(6 * s), rect.bottom});
        }
        if (!m_editStatus.empty()) {
            brush->SetColor(D2D1::ColorF(1.0f, 0.55f, 0.5f, edit));
            DrawMarquee(target, brush, m_editStatus, m_linkFormat.get(),
                        m_status, true);
        }
        // "Done", blue.
        const float doneRadius = (m_done.bottom - m_done.top) / 2;
        brush->SetColor(D2D1::ColorF(
            0.04f, 0.52f, 1.0f, (m_hover.part == Part::EditDone ? 0.85f : 1.0f)));
        target->FillRoundedRectangle({m_done, doneRadius, doneRadius}, brush);
        PCWSTR done = Tr(L"Done", L"Concluído");
        brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
        target->DrawText(done, (UINT32)wcslen(done), m_linkFormat.get(),
                         m_done, brush);
        return;
    }

    PCWSTR more = Tr(L"More Windows controls", L"Mais controles do Windows");
    brush->SetColor(Fg(m_hover.part == Part::More ? 0.9f : 0.55f));
    DrawMarquee(target, brush, more, m_linkFormat.get(), m_more, true);
    // "Edit", with a pencil.
    const float editRadius = (m_edit.bottom - m_edit.top) / 2 - 2;
    const D2D1_RECT_F editBack{m_edit.left, m_edit.top + 2, m_edit.right,
                               m_edit.bottom - 2};
    brush->SetColor(Fg(m_hover.part == Part::EditButton ? 0.2f : 0.1f));
    target->FillRoundedRectangle({editBack, editRadius, editRadius}, brush);
    const std::wstring editText =
        std::wstring(1, kGlyphEdit) + L"  " + Tr(L"Edit", L"Editar");
    brush->SetColor(Fg(0.9f));
    // The gear: the island's own options.
    {
        const float gearRadius = (m_gear.bottom - m_gear.top) / 2 - 2;
        brush->SetColor(Fg(m_hover.part == Part::SettingsButton ? 0.2f : 0.1f));
        target->FillEllipse({{(m_gear.left + m_gear.right) / 2,
                              (m_gear.top + m_gear.bottom) / 2},
                             gearRadius,
                             gearRadius},
                            brush);
        brush->SetColor(Fg(0.9f));
        target->DrawText(&kGlyphSettings, 1, m_iconFormat.get(), m_gear,
                         brush);
    }
    winrt::com_ptr<IDWriteTextLayout> editLayout;
    if (SUCCEEDED(m_dwrite->CreateTextLayout(
            editText.c_str(), (UINT32)editText.size(), m_linkFormat.get(),
            m_edit.right - m_edit.left, m_edit.bottom - m_edit.top,
            editLayout.put()))) {
        // The pencil in the icon font.
        editLayout->SetFontFamilyName(L"Segoe Fluent Icons", {0, 1});
        target->DrawTextLayout({m_edit.left, m_edit.top}, editLayout.get(),
                               brush);
    }
}

void ControlPanel::DrawOutputs(ID2D1RenderTarget* target,
                              ID2D1SolidColorBrush* brush) {
    const float s = m_scale;
    DrawHeader(target, brush, Tr(L"Sound output", L"Saída de som"));
    for (size_t i = 0; i < m_outputs.size() && i < m_rows.size(); i++) {
        const auto& output = m_outputs[i];
        const D2D1_RECT_F& row = m_rows[i];
        const bool hover = m_hover.part == Part::Output && m_hover.index == (int)i;
        if (hover || output.isDefault) {
            const float radius = std::round(10 * s);
            brush->SetColor(Fg(hover ? 0.14f : 0.08f));
            target->FillRoundedRectangle({row, radius, radius}, brush);
        }
        const float icon = std::round(36 * s);
        brush->SetColor(Fg(0.95f));
        target->DrawText(&output.glyph, 1, m_iconFormat.get(),
                         {row.left, row.top, row.left + icon, row.bottom},
                         brush);
        target->DrawText(output.name.c_str(), (UINT32)output.name.size(),
                         m_subtitleFormat.get(),
                         {row.left + icon, row.top, row.right - icon,
                          row.bottom},
                         brush);
        if (output.isDefault) {
            brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
            target->DrawText(&kGlyphCheck, 1, m_iconFormat.get(),
                             {row.right - icon, row.top, row.right,
                              row.bottom},
                             brush);
        }
    }
}

// The app's icon as a Direct2D bitmap, made once.
ID2D1Bitmap* ControlPanel::GetAppBitmap(ID2D1RenderTarget* target,
                                        MixerApp& app) {
    if (app.bitmap || !app.icon || !m_wic) {
        return app.bitmap.get();
    }
    winrt::com_ptr<IWICBitmap> wicBitmap;
    winrt::com_ptr<IWICFormatConverter> converter;
    if (SUCCEEDED(m_wic->CreateBitmapFromHICON(app.icon, wicBitmap.put())) &&
        SUCCEEDED(m_wic->CreateFormatConverter(converter.put())) &&
        SUCCEEDED(converter->Initialize(
            wicBitmap.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        target->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                          app.bitmap.put());
    }
    return app.bitmap.get();
}

void ControlPanel::DrawMixer(ID2D1RenderTarget* target,
                             ID2D1SolidColorBrush* brush) {
    DrawHeader(target, brush, Tr(L"Volume mixer", L"Mixer de volume"));
    for (size_t i = 0; i < m_rows.size(); i++) {
        const int index = (int)i - 1;
        const D2D1_RECT_F& iconRect = m_rowIcons[i];
        const D2D1_RECT_F& sliderRect = m_rowSliders[i];
        const bool hoverIcon =
            (index < 0 && m_hover.part == Part::VolumeIcon) ||
            (m_hover.part == Part::AppIcon && m_hover.index == index);

        float level;
        bool muted;
        std::wstring name;
        if (index < 0) {
            level = m_volume;
            muted = m_muted;
            name = m_device.empty() ? std::wstring(Tr(L"Device", L"Dispositivo"))
                                    : m_device;
        } else {
            const MixerApp& app = m_apps[index];
            level = app.volume;
            muted = app.muted;
            name = app.name;
        }

        // The icon: the app's, or a glyph for the device and system sounds.
        if (hoverIcon) {
            const float radius = (iconRect.bottom - iconRect.top) / 2;
            brush->SetColor(Fg(0.14f));
            target->FillRoundedRectangle({iconRect, radius, radius}, brush);
        }
        ID2D1Bitmap* bitmap =
            index >= 0 ? GetAppBitmap(target, m_apps[index]) : nullptr;
        if (bitmap) {
            const float inset = std::round(5 * m_scale);
            target->DrawBitmap(bitmap,
                               {iconRect.left + inset, iconRect.top + inset,
                                iconRect.right - inset,
                                iconRect.bottom - inset},
                               muted ? 0.4f : 1.0f);
        } else {
            const WCHAR glyph = index < 0 ? kGlyphHeadphones : kGlyphSpeakers;
            brush->SetColor(Fg(muted ? 0.4f : 0.9f));
            target->DrawText(&glyph, 1, m_iconFormat.get(), iconRect, brush);
        }
        if (muted) {
            brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, 1));
            target->DrawText(&kGlyphMute, 1, m_iconFormat.get(),
                             {iconRect.right - std::round(14 * m_scale),
                              iconRect.bottom - std::round(14 * m_scale),
                              iconRect.right + std::round(2 * m_scale),
                              iconRect.bottom + std::round(2 * m_scale)},
                             brush);
        }

        // The name above the slider.
        brush->SetColor(Fg(0.6f));
        target->DrawText(name.c_str(), (UINT32)name.size(),
                         m_subtitleFormat.get(),
                         {sliderRect.left, iconRect.top - std::round(2 * m_scale),
                          sliderRect.right, sliderRect.top},
                         brush);
        DrawSlider(target, brush, sliderRect, muted ? 0 : level, 0, false);
    }
}

void ControlPanel::Draw(ID2D1RenderTarget* target,
                        ID2D1SolidColorBrush* brush) {
    // Found again while drawing (see DrawMarquee). Not cleared between
    // drawings: frames skipped for the lower frame rate don't draw.
    m_marqueeActive = false;
    if (!m_iconFormat || !m_titleFormat || !m_subtitleFormat ||
        !m_sectionFormat || !m_percentFormat || !m_linkFormat ||
        !m_smallFormat) {
        return;
    }
    // A view coming in: faded and moved aside a little, on top of the
    // panel's own placement.
    const float in = (float)std::clamp(m_viewIn, 0.0, 1.05);
    winrt::com_ptr<ID2D1Layer> viewLayer;
    D2D1_MATRIX_3X2_F base;
    target->GetTransform(&base);
    if (in < 0.999f) {
        target->CreateLayer(nullptr, viewLayer.put());
        target->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                  D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                  D2D1::IdentityMatrix(),
                                  std::clamp(in * 1.3f, 0.0f, 1.0f)),
            viewLayer.get());
        target->SetTransform(
            D2D1::Matrix3x2F::Translation(
                m_viewDirection * (1 - in) * std::round(28 * m_scale), 0) *
            *D2D1::Matrix3x2F::ReinterpretBaseType(&base));
    }
    DrawView(target, brush);
    if (viewLayer) {
        target->SetTransform(base);
        target->PopLayer();
    }
}

void ControlPanel::DrawView(ID2D1RenderTarget* target,
                            ID2D1SolidColorBrush* brush) {
    switch (m_view) {
        case View::Main:
            DrawMain(target, brush);
            break;
        case View::Output:
            DrawOutputs(target, brush);
            break;
        case View::Mixer:
            DrawMixer(target, brush);
            break;
        case View::Wifi:
            DrawWifi(target, brush);
            break;
        case View::Password:
            DrawPassword(target, brush);
            break;
        case View::Bluetooth:
            DrawBluetooth(target, brush);
            break;
        case View::Project:
            DrawProject(target, brush);
            break;
        case View::Accessibility:
            DrawAccessibility(target, brush);
            break;
        case View::Settings:
            DrawSettings(target, brush);
            break;
        case View::AppPlaces:
            DrawAppPlaces(target, brush);
            break;
    }
}

void ControlPanel::SetVolumeAt(POINT pt) {
    if (!g_island) {
        return;
    }
    const D2D1_RECT_F& rect =
        m_view == View::Mixer && !m_rowSliders.empty() ? m_rowSliders[0]
                                                       : m_slider;
    const float knob = rect.bottom - rect.top;
    const float level = std::clamp(
        (pt.x - rect.left - knob / 2) / (rect.right - rect.left - knob), 0.0f,
        1.0f);
    if (auto volume = g_island->GetVolume(nullptr)) {
        volume->SetMasterVolumeLevelScalar(level, nullptr);
        volume->SetMute(level <= 0.001f, nullptr);
    }
    m_volume = level;
    m_muted = level <= 0.001f;
    g_island->OnVolumeChanged();
}

void ControlPanel::SetAppVolume(int index, float volume) {
    if (index < 0 || index >= (int)m_apps.size()) {
        return;
    }
    MixerApp& app = m_apps[index];
    app.volume = std::clamp(volume, 0.0f, 1.0f);
    app.muted = false;
    for (auto& session : app.volumes) {
        session->SetMasterVolume(app.volume, nullptr);
        session->SetMute(FALSE, nullptr);
    }
}

void ControlPanel::SetAppVolumeAt(int index, POINT pt) {
    if (index < 0 || index + 1 >= (int)m_rowSliders.size()) {
        return;
    }
    const D2D1_RECT_F& rect = m_rowSliders[index + 1];
    const float knob = rect.bottom - rect.top;
    SetAppVolume(index, (pt.x - rect.left - knob / 2) /
                            (rect.right - rect.left - knob));
}

bool ControlPanel::OnMouseMove(POINT pt) {
    // Dragging a control while editing: it follows the mouse, and takes the
    // place of the one under it (the others slide out of the way).
    if (m_dragId >= 0 && (GetKeyState(VK_LBUTTON) & 0x8000)) {
        const int threshold = (int)std::round(5 * m_scale);
        if (!m_dragMoved && (std::abs(pt.x - m_dragStart.x) > threshold ||
                             std::abs(pt.y - m_dragStart.y) > threshold)) {
            m_dragMoved = true;
        }
        if (!m_dragMoved) {
            return false;
        }
        m_dragPoint = pt;
        for (const auto& [id, rect] : m_controlRects) {
            if ((int)id == m_dragId || !PointInRect(pt, rect)) {
                continue;
            }
            auto from = std::find(m_controls.begin(), m_controls.end(),
                                  (ControlId)m_dragId);
            auto to = std::find(m_controls.begin(), m_controls.end(), id);
            if (from != m_controls.end() && to != m_controls.end()) {
                const ControlId moved = *from;
                const int toIndex = (int)(to - m_controls.begin());
                m_controls.erase(from);
                m_controls.insert(m_controls.begin() + toIndex, moved);
                Layout();
            }
            break;
        }
        // Drawn by the animation, once per frame: not on every mouse move
        // too (a mouse can send hundreds a second).
        StartIslandAnimation();
        return false;
    }
    if (m_dragging) {
        if (m_dragIndex == kDragIslandSize) {
            SetIslandSizeAt(pt, false);
        } else if (m_dragIndex == kDragBrightness) {
            SetBrightnessAt(pt);
        } else if (m_dragIndex < 0) {
            SetVolumeAt(pt);
        } else {
            SetAppVolumeAt(m_dragIndex, pt);
        }
        return true;
    }
    const Hit hover = HitTest(pt);
    if (hover == m_hover) {
        return false;
    }
    m_hover = hover;
    return true;
}

bool ControlPanel::OnMouseLeave() {
    if (m_hover.part == Part::None) {
        return false;
    }
    m_hover = {};
    return true;
}

bool ControlPanel::OnMouseDown(POINT pt) {
    const Hit hit = HitTest(pt);
    // A control while editing: maybe dragged.
    if (m_view == View::Main && m_editing && hit.part == Part::EditControl) {
        for (const auto& [id, rect] : m_controlRects) {
            if ((int)id == hit.index) {
                m_dragId = hit.index;
                m_dragMoved = false;
                m_dragStart = m_dragPoint = pt;
                m_dragGrab = {pt.x - rect.left, pt.y - rect.top};
                SetCapture(m_hwnd);
            }
        }
        return false;
    }
    if (hit.part == Part::SizeSlider) {
        m_dragging = true;
        m_dragIndex = kDragIslandSize;
        SetCapture(m_hwnd);
        SetIslandSizeAt(pt, false);
        return true;
    }
    if (hit.part == Part::BrightnessSlider &&
        PointInRect(pt, m_brightnessSlider)) {
        m_dragging = true;
        m_dragIndex = kDragBrightness;
        SetCapture(m_hwnd);
        SetBrightnessAt(pt);
        return true;
    }
    if (hit.part == Part::Slider ||
        (hit.part == Part::AppSlider && hit.index < 0)) {
        m_dragging = true;
        m_dragIndex = -1;
        SetCapture(m_hwnd);
        SetVolumeAt(pt);
        return true;
    }
    if (hit.part == Part::AppSlider) {
        m_dragging = true;
        m_dragIndex = hit.index;
        SetCapture(m_hwnd);
        SetAppVolumeAt(hit.index, pt);
        return true;
    }
    return false;
}

bool ControlPanel::OnMouseUp(POINT pt) {
    if (m_dragId >= 0) {
        const bool moved = m_dragMoved;
        // Dropped: it springs into its place from there.
        if (Motion* motion = FindMotion((ControlId)m_dragId); motion && moved) {
            motion->x = m_dragPoint.x - m_dragGrab.x;
            motion->y = m_dragPoint.y - m_dragGrab.y;
            motion->vx = motion->vy = 0;
            motion->scale = 1.06;
            motion->vscale = 0;
        }
        m_dragId = -1;
        m_dragMoved = false;
        if (GetCapture() == m_hwnd) {
            ReleaseCapture();
        }
        if (moved) {
            SaveControls();
            Layout();
            StartIslandAnimation();
            return true;
        }
    }
    if (m_dragging) {
        m_dragging = false;
        ReleaseCapture();
        if (m_dragIndex == kDragIslandSize) {
            SetIslandSizeAt(pt, true);
        }
        return true;
    }
    const Hit hit = HitTest(pt);
    switch (hit.part) {
        case Part::Network:
            SetView(View::Wifi);
            return true;
        case Part::Bluetooth:
            SetView(View::Bluetooth);
            return true;
        case Part::WifiToggle:
            // Shown right away; the radio's thread confirms it.
            m_wifiRadio = m_wifiRadio == 1 ? 0 : 1;
            SwitchRadio(0, m_wifiRadio == 1);
            if (m_wifiRadio == 0) {
                m_networks.clear();
            }
            Layout();
            return true;
        case Part::BluetoothToggle:
            m_bluetoothRadio = m_bluetoothRadio == 1 ? 0 : 1;
            SwitchRadio(1, m_bluetoothRadio == 1);
            UpdateBluetooth();
            Layout();
            return true;
        case Part::WifiRow:
            JoinWifi(hit.index);
            Layout();
            return true;
        case Part::Disconnect:
            if (m_wlan && m_hasWifi) {
                WlanDisconnect(m_wlan, &m_wifiInterface, nullptr);
            }
            return true;
        case Part::ShowPassword:
            m_showPassword = !m_showPassword;
            return true;
        case Part::Connect:
            SubmitPassword();
            Layout();
            return true;
        case Part::DoNotDisturb:
            // The notifications, with its switch.
            if (g_island) {
                g_island->OpenNotifications();
            }
            break;
        case Part::DndToggle:
            if (SetDoNotDisturb(!m_doNotDisturb)) {
                m_doNotDisturb = !m_doNotDisturb;
                if (g_island) {
                    g_island->OnDoNotDisturbChanged();
                }
            }
            return true;
        case Part::NightLight:
            if (m_nightLight >= 0 && SetNightLight(m_nightLight != 1)) {
                m_nightLight = m_nightLight == 1 ? 0 : 1;
            }
            return true;
        case Part::Shortcut:
            switch ((ControlId)hit.index) {
                case ControlId::Project:
                    UpdateDisplayMode();
                    SetView(View::Project);
                    return true;
                case ControlId::Cast:
                    // Casting looks for wireless displays: the Windows panel.
                    Close();
                    PressKeys({VK_LWIN, 'K'});
                    break;
                case ControlId::Captions:
                    // Live captions switch on and off with their shortcut.
                    PressKeys({VK_LWIN, VK_CONTROL, 'L'});
                    break;
                case ControlId::Accessibility:
                    UpdateAccessibility();
                    SetView(View::Accessibility);
                    return true;
                case ControlId::DarkMode:
                    m_darkMode = !m_darkMode;
                    StartWorker(DarkModeThreadProc,
                                m_darkMode ? (void*)1 : nullptr);
                    return true;
                case ControlId::Screenshot:
                    // After the panel is gone, so it isn't in the picture.
                    Close();
                    SetTimer(m_hwnd, kScreenshotTimerId, 450, nullptr);
                    break;
                case ControlId::Lock:
                    Close();
                    LockWorkStation();
                    break;
                case ControlId::Emoji:
                    Close();
                    PressKeys({VK_LWIN, VK_OEM_PERIOD});
                    break;
                case ControlId::Clipboard:
                    Close();
                    PressKeys({VK_LWIN, 'V'});
                    break;
                case ControlId::TaskManager:
                    Close();
                    PressKeys({VK_CONTROL, VK_SHIFT, VK_ESCAPE});
                    break;
                case ControlId::Calculator:
                    Close();
                    ShellExecute(nullptr, L"open", L"calc.exe", nullptr,
                                 nullptr, SW_SHOWNORMAL);
                    break;
                case ControlId::Explorer:
                    Close();
                    ShellExecute(nullptr, L"open", L"explorer.exe", nullptr,
                                 nullptr, SW_SHOWNORMAL);
                    break;
                default:
                    break;
            }
            break;
        case Part::EditButton:
            SetEditing(true);
            return true;
        case Part::EditDone:
            SetEditing(false);
            return true;
        case Part::SettingsButton:
            SetView(View::Settings);
            return true;
        case Part::EditRemove: {
            const ControlId id = (ControlId)hit.index;
            auto it = std::find(m_controls.begin(), m_controls.end(), id);
            if (it == m_controls.end()) {
                return true;
            }
            // It shrinks away where it's shown; the others close the gap.
            for (const auto& [shown, rect] : m_controlRects) {
                if (shown == id) {
                    D2D1_RECT_F at = rect;
                    if (const Motion* motion = FindMotion(id)) {
                        const float dx = (float)motion->x - rect.left;
                        const float dy = (float)motion->y - rect.top;
                        at = {rect.left + dx, rect.top + dy, rect.right + dx,
                              rect.bottom + dy};
                    }
                    m_ghosts.push_back({id, at});
                }
            }
            m_controls.erase(it);
            m_editStatus.clear();
            SaveControls();
            Layout();
            StartIslandAnimation();
            return true;
        }
        case Part::EditAdd: {
            const ControlId id = (ControlId)hit.index;
            // Within the limits.
            const ControlKind kind = GetControlInfo(id).kind;
            const int count = (int)std::count_if(
                m_controls.begin(), m_controls.end(), [&](ControlId other) {
                    return GetControlInfo(other).kind == kind;
                });
            const int limit = kind == ControlKind::Tile     ? kMaxTiles
                              : kind == ControlKind::Button ? kMaxButtons
                                                            : 99;
            if (count >= limit) {
                m_editStatus =
                    kind == ControlKind::Tile
                        ? Tr(L"At most 8 tiles", L"No máximo 8 blocos")
                        : Tr(L"At most 8 buttons", L"No máximo 8 botões");
                Layout();
                return true;
            }
            D2D1_RECT_F from{};
            for (const auto& [chip, rect] : m_addRects) {
                if (chip == id) {
                    from = rect;
                }
            }
            m_controls.push_back(id);
            m_editStatus.clear();
            SaveControls();
            Layout();
            // It grows in from where it was listed.
            if (Motion* motion = FindMotion(id)) {
                motion->x = from.left;
                motion->y = from.top;
                motion->scale = 0.4;
                motion->vscale = 0;
            }
            StartIslandAnimation();
            return true;
        }
        case Part::SettingToggle: {
            const IslandPref pref = (IslandPref)hit.index;
            SetIslandPref(pref, GetIslandPref(pref) ? 0 : 1);
            return true;
        }
        case Part::SizeReset:
            SetIslandSize(kIslandSizeDefault, true);
            return true;
        case Part::StyleSegment:
            if (GetIslandPref(IslandPref::Bar) != hit.index) {
                SetIslandPref(IslandPref::Bar, hit.index);
                StartIslandAnimation();
            }
            return true;
        case Part::PlaceDot:
            SetIslandPlace({}, (hit.index % 3) / 2.0, (hit.index / 3) / 2.0);
            return true;
        case Part::AppPlacesRow:
            ListOpenApps();
            SetView(View::AppPlaces);
            return true;
        case Part::AppPlaceDot: {
            const size_t entry = hit.index / 9;
            if (entry < g_appPlaces.size()) {
                SetIslandPlace(g_appPlaces[entry].appId,
                               (hit.index % 9 % 3) / 2.0,
                               (hit.index % 9 / 3) / 2.0);
            }
            return true;
        }
        case Part::AppPlaceRemove:
            if (hit.index >= 0 && hit.index < (int)g_appPlaces.size()) {
                g_appPlaces.erase(g_appPlaces.begin() + hit.index);
                SavePlaces();
                if (g_island) {
                    g_island->OnPlacesChanged();
                }
                ListOpenApps();
                Layout();
            }
            return true;
        case Part::AppPlaceAdd:
            if (hit.index >= 0 && hit.index < (int)m_openApps.size() &&
                g_appPlaces.size() < kMaxAppPlaces) {
                // Where the pill is for all apps, to start with.
                IslandPlace place = g_islandPlace;
                place.appId = m_openApps[hit.index];
                g_appPlaces.push_back(std::move(place));
                SavePlaces();
                ListOpenApps();
                Layout();
            }
            return true;
        case Part::ThemeSwatch:
            SetIslandPref(IslandPref::Theme, hit.index);
            return true;
        case Part::HotspotToggle:
            m_hotspot = m_hotspot == 1 ? 0 : 1;
            SwitchRadio(2, m_hotspot == 1);
            return true;
        case Part::AirplaneToggle:
            m_airplane = m_airplane == 1 ? 0 : 1;
            SwitchRadio(3, m_airplane == 1);
            return true;
        case Part::ProjectRow: {
            static const UINT32 kTopologies[] = {
                SDC_TOPOLOGY_INTERNAL, SDC_TOPOLOGY_CLONE,
                SDC_TOPOLOGY_EXTEND, SDC_TOPOLOGY_EXTERNAL};
            if (hit.index >= 0 && hit.index < 4) {
                const LONG result =
                    SetDisplayConfig(0, nullptr, 0, nullptr,
                                     SDC_APPLY | kTopologies[hit.index]);
                if (result != ERROR_SUCCESS) {
                    Wh_Log(L"SetDisplayConfig failed: %ld", result);
                }
                UpdateDisplayMode();
            }
            return true;
        }
        case Part::AccessibilityRow:
            ToggleAccessibility(hit.index);
            return true;
        case Part::VolumeIcon:
            if (auto volume = g_island ? g_island->GetVolume(nullptr) : nullptr) {
                volume->SetMute(!m_muted, nullptr);
                m_muted = !m_muted;
                g_island->OnVolumeChanged();
            }
            return true;
        case Part::Device:
            SetView(View::Output);
            return true;
        case Part::MixerButton:
            SetView(View::Mixer);
            return true;
        case Part::Back:
            SetView(m_view == View::Password    ? View::Wifi
                    : m_view == View::AppPlaces ? View::Settings
                                                : View::Main);
            return true;
        case Part::Output:
            if (hit.index >= 0 && hit.index < (int)m_outputs.size()) {
                winrt::com_ptr<IPolicyConfig> policy;
                if (SUCCEEDED(CoCreateInstance(kCLSID_PolicyConfig, nullptr,
                                               CLSCTX_ALL, kIID_IPolicyConfig,
                                               policy.put_void()))) {
                    const auto& id = m_outputs[hit.index].id;
                    for (ERole role : {eConsole, eMultimedia, eCommunications}) {
                        policy->SetDefaultEndpoint(id.c_str(), role);
                    }
                }
                Update();
                if (g_island) {
                    g_island->OnVolumeChanged();
                }
            }
            return true;
        case Part::AppIcon:
            if (hit.index >= 0 && hit.index < (int)m_apps.size()) {
                MixerApp& app = m_apps[hit.index];
                app.muted = !app.muted;
                for (auto& session : app.volumes) {
                    session->SetMute(app.muted, nullptr);
                }
            }
            return true;
        case Part::Battery:
            Close();
            OpenUri(L"ms-settings:batterysaver");
            break;
        case Part::More:
            Close();
            PressWindowsKey('A');
            break;
        default:
            break;
    }
    return false;
}

bool ControlPanel::OnWheel(int delta) {
    // Not the volume while choosing options or editing.
    if (m_view == View::Settings || (m_view == View::Main && m_editing)) {
        return false;
    }
    if (!g_island) {
        return false;
    }
    const float step = 0.02f * delta / WHEEL_DELTA;
    // Over the display, the wheel changes the brightness, 5% a notch.
    if (m_view == View::Main && m_hover.part == Part::BrightnessSlider &&
        m_brightness >= 0) {
        SetBrightness(m_brightness + 5 * delta / WHEEL_DELTA);
        return true;
    }
    // In the mixer, the wheel over an app's row changes that app.
    if (m_view == View::Mixer && m_hover.part != Part::None &&
        m_hover.index >= 0 && m_hover.index < (int)m_apps.size()) {
        SetAppVolume(m_hover.index, m_apps[m_hover.index].volume + step);
        return true;
    }
    if (auto volume = g_island->GetVolume(nullptr)) {
        const float level = std::clamp(m_volume + step, 0.0f, 1.0f);
        volume->SetMasterVolumeLevelScalar(level, nullptr);
        if (level > 0) {
            volume->SetMute(FALSE, nullptr);
        }
        m_volume = level;
        m_muted = false;
        g_island->OnVolumeChanged();
    }
    return true;
}

bool ControlPanel::OnKey(WPARAM key) {
    // Escape and Backspace go back, like in Windows (Backspace types in the
    // password).
    // Escape leaves editing.
    if (m_view == View::Main) {
        if (m_editing && key == VK_ESCAPE) {
            SetEditing(false);
            return true;
        }
        return false;
    }
    if (key == VK_BACK && m_view == View::Password) {
        return false;
    }
    if (key == VK_ESCAPE || key == VK_BACK) {
        SetView(m_view == View::Password    ? View::Wifi
                : m_view == View::AppPlaces ? View::Settings
                                            : View::Main);
        return true;
    }
    return false;
}

// Typing the password: Enter connects, Ctrl+V pastes.
bool ControlPanel::OnChar(WCHAR c) {
    if (m_view != View::Password) {
        return false;
    }
    constexpr size_t kMaxPassword = 63;
    if (c == L'\r') {
        SubmitPassword();
        Layout();
        return true;
    }
    if (c == L'\b') {
        if (!m_password.empty()) {
            m_password.pop_back();
        }
    } else if (c == 0x16) {
        if (OpenClipboard(m_hwnd)) {
            if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
                if (auto text = (PCWSTR)GlobalLock(data)) {
                    for (PCWSTR p = text; *p; p++) {
                        if (*p >= 0x20 && m_password.size() < kMaxPassword) {
                            m_password += *p;
                        }
                    }
                    GlobalUnlock(data);
                }
            }
            CloseClipboard();
        }
    } else if (c >= 0x20 && m_password.size() < kMaxPassword) {
        m_password += c;
    } else {
        return false;
    }
    if (!m_connecting) {
        m_wifiStatus.clear();
    }
    Layout();
    return true;
}

bool ControlPanel::OnAppMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_APP_SCREENSHOT) {
        PressKeys({VK_LWIN, VK_SHIFT, 'S'});
        return false;
    }
    if (msg == WM_APP_RADIOS) {
        m_wifiRadio = (int)(INT_PTR)wParam;
        m_bluetoothRadio = (int)(INT_PTR)lParam;
        Update();
        Layout();
        return true;
    }
    if (msg == WM_APP_HOTSPOT) {
        m_hotspot = (int)(INT_PTR)wParam;
        m_airplane = (int)(INT_PTR)lParam;
        Layout();
        return true;
    }
    if (msg == WM_APP_BRIGHTNESS) {
        // Not while it's being changed here.
        if (!m_brightnessJob.running) {
            m_brightness = (int)(INT_PTR)wParam;
        }
        Layout();
        return true;
    }
    if (msg != WM_APP_WLAN) {
        return false;
    }
    switch (wParam) {
        case wlan_notification_acm_scan_complete:
        case wlan_notification_acm_disconnected:
            break;
        case wlan_notification_acm_connection_complete:
        case wlan_notification_acm_connection_attempt_fail:
            if (!m_connecting) {
                break;
            }
            m_connecting = false;
            if (wParam == wlan_notification_acm_connection_complete &&
                lParam == WLAN_REASON_CODE_SUCCESS) {
                m_createdProfile.clear();
                m_wifiStatus.clear();
                if (m_view == View::Password) {
                    m_password.clear();
                    SetView(View::Wifi);
                }
            } else {
                Wh_Log(L"Joining Wi-Fi failed, reason %u", (unsigned)lParam);
                // A wrong password isn't kept.
                if (!m_createdProfile.empty() && m_wlan) {
                    WlanDeleteProfile(m_wlan, &m_wifiInterface,
                                      m_createdProfile.c_str(), nullptr);
                }
                m_createdProfile.clear();
                m_wifiStatus =
                    m_view == View::Password
                        ? Tr(L"Couldn't connect. Check the password.",
                             L"Não foi possível conectar. Confira a senha.")
                        : Tr(L"Couldn't connect.",
                             L"Não foi possível conectar.");
            }
            break;
        default:
            return false;
    }
    if (m_view == View::Wifi) {
        UpdateWifi();
    }
    if (g_island) {
        g_island->OnNetworkChanged();
    }
    Layout();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// The notifications.

// The texts of a toast's XML: <text> elements, in order, with entities
// turned back into characters.
// XML text (or an attribute's value) as a wide string, with its entities
// ("&amp;", "&#233;"...) turned back into characters.
std::wstring DecodeXml(std::string_view raw) {
    std::string text;
    for (size_t i = 0; i < raw.size(); i++) {
        if (raw[i] != '&') {
            text += raw[i];
            continue;
        }
        const size_t semicolon = raw.find(';', i);
        if (semicolon == std::string_view::npos || semicolon - i > 10) {
            text += raw[i];
            continue;
        }
        const std::string_view entity = raw.substr(i + 1, semicolon - i - 1);
        unsigned code = 0;
        if (entity == "amp") {
            code = '&';
        } else if (entity == "lt") {
            code = '<';
        } else if (entity == "gt") {
            code = '>';
        } else if (entity == "quot") {
            code = '"';
        } else if (entity == "apos") {
            code = '\'';
        } else if (entity.size() > 1 && entity[0] == '#') {
            code = entity[1] == 'x'
                       ? strtoul(std::string(entity.substr(2)).c_str(),
                                 nullptr, 16)
                       : strtoul(std::string(entity.substr(1)).c_str(),
                                 nullptr, 10);
        }
        if (!code) {
            text += raw[i];
            continue;
        }
        // As UTF-8.
        if (code < 0x80) {
            text += (char)code;
        } else if (code < 0x800) {
            text += (char)(0xC0 | (code >> 6));
            text += (char)(0x80 | (code & 0x3F));
        } else if (code < 0x10000) {
            text += (char)(0xE0 | (code >> 12));
            text += (char)(0x80 | ((code >> 6) & 0x3F));
            text += (char)(0x80 | (code & 0x3F));
        } else {
            text += (char)(0xF0 | (code >> 18));
            text += (char)(0x80 | ((code >> 12) & 0x3F));
            text += (char)(0x80 | ((code >> 6) & 0x3F));
            text += (char)(0x80 | (code & 0x3F));
        }
        i = semicolon;
    }
    const int length = MultiByteToWideChar(CP_UTF8, 0, text.data(),
                                           (int)text.size(), nullptr, 0);
    std::wstring wide(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(),
                        wide.data(), length);
    return wide;
}

// An attribute's value in a tag ("<action content="Reply" ...").
std::wstring XmlAttribute(std::string_view tag, std::string_view name) {
    for (size_t at = tag.find(name); at != std::string_view::npos;
         at = tag.find(name, at + 1)) {
        const size_t equals = at + name.size();
        if ((at > 0 && !isspace((unsigned char)tag[at - 1])) ||
            equals + 1 >= tag.size() || tag[equals] != '=') {
            continue;
        }
        const char quote = tag[equals + 1];
        if (quote != '"' && quote != '\'') {
            continue;
        }
        const size_t end = tag.find(quote, equals + 2);
        if (end == std::string_view::npos) {
            return {};
        }
        return DecodeXml(tag.substr(equals + 2, end - equals - 2));
    }
    return {};
}

// The picture a notification shows instead of its app's icon (its
// "appLogoOverride" image, like a contact's photo), as a file on this
// computer, or on the web when chosen (islandWebPictures), only for an app
// allowed to go online, like Windows does (see PackageGoesOnline).
bool ToastImage(const std::wstring& appId,
                const char* xml,
                int size,
                std::wstring* path,
                bool* circle) {
    const std::string_view view{xml, (size_t)std::max(size, 0)};
    std::wstring src;
    for (size_t at = view.find("<image"); at != std::string_view::npos;
         at = view.find("<image", at + 1)) {
        const size_t close = view.find('>', at);
        if (close == std::string_view::npos) {
            break;
        }
        const std::string_view tag = view.substr(at, close - at);
        if (XmlAttribute(tag, "placement") == L"appLogoOverride") {
            src = XmlAttribute(tag, "src");
            *circle = XmlAttribute(tag, "hint-crop") == L"circle";
            break;
        }
    }
    if (src.empty()) {
        return false;
    }
    auto startsWith = [&](std::wstring_view prefix) {
        return src.size() > prefix.size() &&
               _wcsnicmp(src.c_str(), prefix.data(), prefix.size()) == 0;
    };
    // A packaged app's own files (ms-appdata:///local/...).
    std::wstring packageFolder;
    PWSTR localAppData = nullptr;
    if (const size_t bang = appId.find(L'!');
        bang != std::wstring::npos &&
        SUCCEEDED(SHGetKnownFolderPath(kFOLDERID_LocalAppData, 0, nullptr,
                                       &localAppData))) {
        packageFolder = std::wstring(localAppData) + L"\\Packages\\" +
                        appId.substr(0, bang) + L"\\";
    }
    CoTaskMemFree(localAppData);
    std::wstring file;
    if (startsWith(L"file:///")) {
        file = src.substr(8);
    } else if (!packageFolder.empty() && startsWith(L"ms-appdata:///local/")) {
        file = packageFolder + L"LocalState\\" + src.substr(20);
    } else if (!packageFolder.empty() &&
               startsWith(L"ms-appdata:///roaming/")) {
        file = packageFolder + L"RoamingState\\" + src.substr(22);
    } else if (!packageFolder.empty() && startsWith(L"ms-appdata:///temp/")) {
        file = packageFolder + L"TempState\\" + src.substr(19);
    } else if (src.size() > 2 && src[1] == L':' && src[2] == L'\\') {
        file = src;
    } else if (startsWith(L"https://") && g_settings.islandWebPictures &&
               PackageGoesOnline(appId)) {
        // Downloaded (see DownloadPicture).
        *path = src;
        return true;
    } else {
        return false;
    }
    // "%20" and the like, which are UTF-8 bytes.
    std::string bytes;
    for (size_t i = 0; i < file.size(); i++) {
        if (file[i] == L'%' && i + 2 < file.size() && iswxdigit(file[i + 1]) &&
            iswxdigit(file[i + 2])) {
            bytes += (char)wcstol(file.substr(i + 1, 2).c_str(), nullptr, 16);
            i += 2;
        } else if (file[i] < 0x80) {
            bytes += file[i] == L'/' ? '\\' : (char)file[i];
        } else {
            char utf8[8];
            const int length =
                WideCharToMultiByte(CP_UTF8, 0, &file[i], 1, utf8,
                                    sizeof(utf8), nullptr, nullptr);
            bytes.append(utf8, std::max(length, 0));
        }
    }
    const int length = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                           (int)bytes.size(), nullptr, 0);
    std::wstring decoded(std::max(length, 0), L' ');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        decoded.data(), length);
    const DWORD attributes = GetFileAttributes(decoded.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return false;
    }
    *path = std::move(decoded);
    return true;
}

// A notification's picture on the web, only when chosen in the island's
// settings (off by default): downloaded like Windows does to show it, from
// the address the app put in its notification. Only over https, small, not
// waiting long, and kept in memory only.
bool DownloadPicture(const std::wstring& url, std::vector<BYTE>* data) {
    constexpr size_t kMaxSize = 4 * 1024 * 1024;
    URL_COMPONENTS parts{sizeof(parts)};
    parts.dwHostNameLength = (DWORD)-1;
    parts.dwUrlPathLength = (DWORD)-1;
    parts.dwExtraInfoLength = (DWORD)-1;
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts) ||
        parts.nScheme != INTERNET_SCHEME_HTTPS || !parts.lpszHostName) {
        return false;
    }
    const std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    // The path with its query, which follows it.
    const std::wstring path =
        parts.lpszUrlPath
            ? std::wstring(parts.lpszUrlPath,
                           parts.dwUrlPathLength + parts.dwExtraInfoLength)
            : std::wstring(L"/");
    bool ok = false;
    HINTERNET session =
        WinHttpOpen(L"Windhawk", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET connection =
        session ? WinHttpConnect(session, host.c_str(), parts.nPort, 0)
                : nullptr;
    HINTERNET request =
        connection
            ? WinHttpOpenRequest(connection, L"GET", path.c_str(), nullptr,
                                 WINHTTP_NO_REFERER,
                                 WINHTTP_DEFAULT_ACCEPT_TYPES,
                                 WINHTTP_FLAG_SECURE)
            : nullptr;
    if (request) {
        WinHttpSetTimeouts(request, 2000, 2000, 2000, 3000);
        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        if (WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                               WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
            WinHttpReceiveResponse(request, nullptr) &&
            WinHttpQueryHeaders(
                request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                WINHTTP_NO_HEADER_INDEX) &&
            status == 200) {
            ok = true;
            BYTE buffer[16 * 1024];
            DWORD read = 0;
            while (WinHttpReadData(request, buffer, sizeof(buffer), &read) &&
                   read > 0) {
                if (data->size() + read > kMaxSize) {
                    ok = false;
                    break;
                }
                data->insert(data->end(), buffer, buffer + read);
            }
        }
        WinHttpCloseHandle(request);
    }
    if (connection) {
        WinHttpCloseHandle(connection);
    }
    if (session) {
        WinHttpCloseHandle(session);
    }
    if (!ok || data->empty()) {
        data->clear();
        Wh_Log(L"Notification picture not downloaded: error %u", GetLastError());
        return false;
    }
    return true;
}

// No "..." at the end of a text box's text (it scrolls instead).
const DWRITE_TRIMMING kNoTrimming{DWRITE_TRIMMING_GRANULARITY_NONE, 0, 0};

// Defined further down.
bool FindToastActivator(const std::wstring& appId, CLSID* clsid);

// The notification's own arguments, its text box and the button sending it.
// No reply box when the app can't be answered from here (like Phone Link,
// which has no notification activator).
ToastReply ParseToastReply(const std::wstring& appId,
                           const char* xml,
                           int size) {
    ToastReply reply;
    const std::string_view view{xml, (size_t)std::max(size, 0)};
    std::vector<std::string_view> actions;
    for (size_t at = view.find('<'); at != std::string_view::npos;
         at = view.find('<', at + 1)) {
        const size_t close = view.find('>', at);
        if (close == std::string_view::npos) {
            break;
        }
        const std::string_view tag = view.substr(at, close - at);
        auto is = [&](std::string_view name) {
            return tag.size() > name.size() + 1 &&
                   tag.substr(1, name.size()) == name &&
                   (isspace((unsigned char)tag[name.size() + 1]) ||
                    tag[name.size() + 1] == '/');
        };
        if (is("toast")) {
            reply.launch = XmlAttribute(tag, "launch");
        } else if (is("input") && reply.inputId.empty() &&
                   XmlAttribute(tag, "type") == L"text") {
            reply.inputId = XmlAttribute(tag, "id");
            reply.placeholder = XmlAttribute(tag, "placeHolderContent");
        } else if (is("action")) {
            actions.push_back(tag);
        }
    }
    for (const auto& tag : actions) {
        if (!reply.inputId.empty() &&
            XmlAttribute(tag, "hint-inputId") == reply.inputId) {
            reply.arguments = XmlAttribute(tag, "arguments");
            reply.activationType = XmlAttribute(tag, "activationType");
            reply.label = XmlAttribute(tag, "content");
            break;
        }
    }
    CLSID clsid;
    if (reply.CanReply() && reply.activationType != L"protocol" &&
        !FindToastActivator(appId, &clsid)) {
        reply.inputId.clear();
    }
    return reply;
}

// Notifications are answered like Windows does: through the app's
// notification activator (a COM class it registers for that), with the
// button's arguments and what was typed.
struct NotificationUserInputData {
    LPCWSTR Key;
    LPCWSTR Value;
};

struct INotificationActivationCallback : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE
    Activate(LPCWSTR appUserModelId,
             LPCWSTR invokedArgs,
             const NotificationUserInputData* data,
             ULONG count) = 0;
};

constexpr GUID kIID_INotificationActivationCallback = {
    0x53e31837,
    0x6600,
    0x4a81,
    {0x93, 0x95, 0x75, 0xcf, 0xfe, 0x74, 0x6f, 0x94}};

// The activator of a packaged app, from its manifest
// (ToastActivatorCLSID); remembered.
// Looked up from the notification threads and the island's.
SRWLOCK g_toastActivatorsLock = SRWLOCK_INIT;

// A packaged app's AppxManifest.xml (empty for other apps).
std::string ReadAppxManifest(const std::wstring& appId) {
    std::string xml;
    const size_t bang = appId.find(L'!');
    if (bang == std::wstring::npos) {
        return xml;
    }
    const std::wstring family = appId.substr(0, bang);
    UINT32 count = 0;
    UINT32 length = 0;
    if (GetPackagesByPackageFamily(family.c_str(), &count, nullptr, &length,
                                   nullptr) != ERROR_INSUFFICIENT_BUFFER ||
        !count) {
        return xml;
    }
    std::vector<PWSTR> names(count);
    std::vector<WCHAR> buffer(length);
    UINT32 pathLength = 0;
    if (GetPackagesByPackageFamily(family.c_str(), &count, names.data(),
                                   &length, buffer.data()) != ERROR_SUCCESS ||
        GetPackagePathByFullName(names[0], &pathLength, nullptr) !=
            ERROR_INSUFFICIENT_BUFFER) {
        return xml;
    }
    std::wstring path(pathLength, L'\0');
    if (GetPackagePathByFullName(names[0], &pathLength, path.data()) !=
        ERROR_SUCCESS) {
        return xml;
    }
    path.resize(wcslen(path.c_str()));
    path += L"\\AppxManifest.xml";
    HANDLE file = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                             nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return xml;
    }
    const DWORD size = GetFileSize(file, nullptr);
    xml.resize(size < 4 * 1024 * 1024 ? size : 0);
    DWORD read = 0;
    ReadFile(file, xml.data(), (DWORD)xml.size(), &read, nullptr);
    xml.resize(read);
    CloseHandle(file);
    return xml;
}

// Whether a packaged app may go online (it declares the "internetClient"
// capability): Windows only shows its notifications' pictures from the web
// then. Remembered.
SRWLOCK g_onlineAppsLock = SRWLOCK_INIT;

bool PackageGoesOnline(const std::wstring& appId) {
    static std::vector<std::pair<std::wstring, bool>> cache;
    AcquireSRWLockShared(&g_onlineAppsLock);
    for (const auto& [id, online] : cache) {
        if (id == appId) {
            ReleaseSRWLockShared(&g_onlineAppsLock);
            return online;
        }
    }
    ReleaseSRWLockShared(&g_onlineAppsLock);
    const std::string xml = ReadAppxManifest(appId);
    const bool online =
        xml.find("Name=\"internetClient\"") != std::string::npos ||
        xml.find("Name=\"internetClientServer\"") != std::string::npos;
    AcquireSRWLockExclusive(&g_onlineAppsLock);
    cache.push_back({appId, online});
    ReleaseSRWLockExclusive(&g_onlineAppsLock);
    return online;
}

bool FindToastActivator(const std::wstring& appId, CLSID* clsid) {
    static std::vector<std::pair<std::wstring, std::wstring>> cache;
    std::wstring found;
    bool cached = false;
    AcquireSRWLockShared(&g_toastActivatorsLock);
    for (const auto& [id, value] : cache) {
        if (id == appId) {
            found = value;
            cached = true;
        }
    }
    ReleaseSRWLockShared(&g_toastActivatorsLock);
    const size_t bang = appId.find(L'!');
    if (!cached && bang != std::wstring::npos) {
        const std::wstring family = appId.substr(0, bang);
        UINT32 count = 0;
        UINT32 length = 0;
        if (GetPackagesByPackageFamily(family.c_str(), &count, nullptr, &length,
                                       nullptr) == ERROR_INSUFFICIENT_BUFFER &&
            count) {
            std::vector<PWSTR> names(count);
            std::vector<WCHAR> buffer(length);
            UINT32 pathLength = 0;
            if (GetPackagesByPackageFamily(family.c_str(), &count,
                                           names.data(), &length,
                                           buffer.data()) == ERROR_SUCCESS &&
                GetPackagePathByFullName(names[0], &pathLength, nullptr) ==
                    ERROR_INSUFFICIENT_BUFFER) {
                std::wstring path(pathLength, L'\0');
                if (GetPackagePathByFullName(names[0], &pathLength,
                                             path.data()) == ERROR_SUCCESS) {
                    path.resize(wcslen(path.c_str()));
                    path += L"\\AppxManifest.xml";
                    HANDLE file = CreateFile(path.c_str(), GENERIC_READ,
                                             FILE_SHARE_READ, nullptr,
                                             OPEN_EXISTING, 0, nullptr);
                    if (file != INVALID_HANDLE_VALUE) {
                        const DWORD size = GetFileSize(file, nullptr);
                        std::string xml(size < 4 * 1024 * 1024 ? size : 0,
                                        '\0');
                        DWORD read = 0;
                        ReadFile(file, xml.data(), (DWORD)xml.size(), &read,
                                 nullptr);
                        CloseHandle(file);
                        const size_t at = xml.find("ToastActivatorCLSID=\"");
                        if (at != std::string::npos) {
                            const size_t start = at + 21;
                            const size_t end = xml.find('"', start);
                            if (end != std::string::npos && end - start < 40) {
                                found = L"{" +
                                        std::wstring(xml.begin() + start,
                                                     xml.begin() + end) +
                                        L"}";
                            }
                        }
                    }
                }
            }
        }
        AcquireSRWLockExclusive(&g_toastActivatorsLock);
        cache.push_back({appId, found});
        ReleaseSRWLockExclusive(&g_toastActivatorsLock);
    }
    return !found.empty() && SUCCEEDED(CLSIDFromString(found.c_str(), clsid));
}

struct ToastActivation {
    CLSID clsid;
    std::wstring appId;
    std::wstring arguments;
    std::wstring inputId;
    std::wstring text;
};

// The app's activator is out of process and may have to start: called on a
// thread of its own.
DWORD WINAPI ToastActivationThreadProc(LPVOID parameter) {
    std::unique_ptr<ToastActivation> activation((ToastActivation*)parameter);
    const HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    winrt::com_ptr<INotificationActivationCallback> callback;
    HRESULT result = CoCreateInstance(
        activation->clsid, nullptr, CLSCTX_LOCAL_SERVER,
        kIID_INotificationActivationCallback, callback.put_void());
    if (SUCCEEDED(result)) {
        const NotificationUserInputData data{activation->inputId.c_str(),
                                             activation->text.c_str()};
        result = callback->Activate(activation->appId.c_str(),
                                    activation->arguments.c_str(),
                                    activation->inputId.empty() ? nullptr
                                                                : &data,
                                    activation->inputId.empty() ? 0 : 1);
    }
    Wh_Log(L"Notification activation of \"%s\": %08X",
        activation->appId.c_str(), (unsigned)result);
    callback = nullptr;
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return 0;
}

std::vector<HANDLE> g_toastActivationThreads;

void WaitForToastActivations() {
    for (HANDLE thread : g_toastActivationThreads) {
        WaitForSingleObject(thread, 10000);
        CloseHandle(thread);
    }
    g_toastActivationThreads.clear();
}

// Presses a notification's button (or the notification itself): the app
// gets its arguments, and what was typed for `inputId`. Returns false if the
// app can't be reached this way.
bool ActivateToast(const std::wstring& appId,
                   const std::wstring& arguments,
                   const std::wstring& activationType,
                   const std::wstring& inputId = {},
                   const std::wstring& text = {}) {
    // The app (or the page it opens) can come to the front.
    AllowSetForegroundWindow(ASFW_ANY);
    if (activationType == L"protocol") {
        // Only a URI with a scheme (like "https:" or an app's own), never a
        // file or a path.
        const size_t colon = arguments.find(L':');
        const bool uri =
            colon != std::wstring::npos && colon >= 2 &&
            std::all_of(arguments.begin(), arguments.begin() + colon,
                        [](WCHAR c) {
                            return iswalnum(c) || c == L'+' || c == L'-' ||
                                   c == L'.';
                        }) &&
            _wcsnicmp(arguments.c_str(), L"file:", 5) != 0;
        return uri && (INT_PTR)ShellExecute(nullptr, L"open",
                                            arguments.c_str(), nullptr,
                                            nullptr, SW_SHOWNORMAL) > 32;
    }
    CLSID clsid;
    if (!FindToastActivator(appId, &clsid)) {
        return false;
    }
    std::erase_if(g_toastActivationThreads, [](HANDLE thread) {
        if (WaitForSingleObject(thread, 0) == WAIT_OBJECT_0) {
            CloseHandle(thread);
            return true;
        }
        return false;
    });
    auto* activation =
        new ToastActivation{clsid, appId, arguments, inputId, text};
    HANDLE thread = CreateThread(nullptr, 0, ToastActivationThreadProc,
                                 activation, 0, nullptr);
    if (!thread) {
        delete activation;
        return false;
    }
    g_toastActivationThreads.push_back(thread);
    return true;
}

// Opens a notification: the app at the right place (a chat), or just the app.
void OpenNotification(const std::wstring& appId, const ToastReply& reply) {
    if (reply.launch.empty() ||
        !ActivateToast(appId, reply.launch, L"foreground")) {
        OpenApp(appId);
    }
}

std::vector<std::wstring> ToastTexts(const char* xml, int size) {
    std::vector<std::wstring> texts;
    const std::string_view view{xml, (size_t)std::max(size, 0)};
    size_t at = 0;
    while ((at = view.find("<text", at)) != std::string_view::npos) {
        const size_t open = at + 5;
        at = open;
        if (open >= view.size() || (view[open] != '>' && view[open] != ' ')) {
            continue;
        }
        const size_t close = view.find('>', open);
        if (close == std::string_view::npos) {
            break;
        }
        const std::string_view tag = view.substr(open, close - open);
        if (!tag.empty() && tag.back() == '/') {
            continue;
        }
        // The app's name the toast sometimes adds isn't part of the message.
        if (tag.find("placement=\"attribution\"") != std::string_view::npos) {
            continue;
        }
        const size_t end = view.find("</text>", close);
        if (end == std::string_view::npos) {
            break;
        }
        const std::string_view raw = view.substr(close + 1, end - close - 1);
        at = end + 7;

        std::wstring wide = DecodeXml(raw);
        // One line: new lines become spaces.
        for (auto& c : wide) {
            if (c == L'\r' || c == L'\n' || c == L'\t') {
                c = L' ';
            }
        }
        if (!wide.empty()) {
            texts.push_back(std::move(wide));
        }
    }
    return texts;
}

// "now", "5 min", the time today, "yesterday", or the date, for a
// notification's arrival time (a FILETIME, in UTC).
std::wstring NotificationTimeText(long long arrival) {
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    const long long current =
        ((long long)now.dwHighDateTime << 32) | now.dwLowDateTime;
    const long long minutes = (current - arrival) / (60LL * 10000000);
    if (minutes < 1) {
        return Tr(L"now", L"agora");
    }
    if (minutes < 60) {
        return std::to_wstring(minutes) + L" min";
    }
    const ULARGE_INTEGER value{.QuadPart = (ULONGLONG)arrival};
    const FILETIME utc{value.LowPart, value.HighPart};
    FILETIME localFile;
    SYSTEMTIME local;
    SYSTEMTIME today;
    if (!FileTimeToLocalFileTime(&utc, &localFile) ||
        !FileTimeToSystemTime(&localFile, &local)) {
        return {};
    }
    GetLocalTime(&today);
    WCHAR text[64] = L"";
    if (local.wYear == today.wYear && local.wMonth == today.wMonth &&
        local.wDay == today.wDay) {
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &local,
                        nullptr, text, ARRAYSIZE(text));
        return text;
    }
    if (minutes < 48 * 60) {
        return Tr(L"yesterday", L"ontem");
    }
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"d MMM", text,
                    ARRAYSIZE(text), nullptr);
    return text;
}

void NotificationPanel::Prepare(float scale) {
    if (!m_wic) {
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(m_wic.put()));
    }
    if (scale != m_preparedScale) {
        m_preparedScale = scale;
        m_bitmaps.clear();
        m_headerFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_SEMI_BOLD,
                       14 * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_appFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       11.5f * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_timeFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       11.5f * scale, DWRITE_TEXT_ALIGNMENT_TRAILING);
        m_titleFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_SEMI_BOLD,
                       13 * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_textFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       12.5f * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_linkFormat = MakeFormat(L"Segoe UI Variable Text",
                                  DWRITE_FONT_WEIGHT_NORMAL, 12 * scale);
        m_iconFormat = MakeFormat(L"Segoe Fluent Icons",
                                  DWRITE_FONT_WEIGHT_NORMAL, 14 * scale);
    }
    Load();
    Layout();
}

void NotificationPanel::OnOpen() {
    m_first = 0;
    m_hover = {};
    m_replyId = -1;
    m_replyText.clear();
    // The switches start where they are.
    m_dndKnob = IsDoNotDisturbOn() ? 1 : 0;
    m_contentKnob = g_showNotificationContent ? 1 : 0;
    m_bannerKnob = g_bannerEnabled ? 1 : 0;
    m_minimizedKnob = g_bannerWhenMinimized ? 1 : 0;
    m_dndKnobVelocity = m_contentKnobVelocity = 0;
    m_bannerKnobVelocity = m_minimizedKnobVelocity = 0;
}

// What was cleared, kept across restarts.
void NotificationPanel::LoadCleared() {
    if (m_clearedLoaded) {
        return;
    }
    m_clearedLoaded = true;
    WCHAR value[4096] = L"";
    Wh_GetStringValue(L"notificationsClearedUntil", value, ARRAYSIZE(value));
    m_clearedUntil = _wcstoi64(value, nullptr, 10);
    value[0] = L'\0';
    Wh_GetStringValue(L"notificationsDismissed", value, ARRAYSIZE(value));
    for (PCWSTR p = value; *p;) {
        PWSTR end = nullptr;
        const long long id = _wcstoi64(p, &end, 10);
        if (end == p) {
            break;
        }
        m_dismissed.push_back(id);
        p = *end == L',' ? end + 1 : end;
    }
}

void NotificationPanel::SaveCleared() {
    // Only the latest ones dismissed one by one are worth keeping.
    constexpr size_t kMaxDismissed = 200;
    if (m_dismissed.size() > kMaxDismissed) {
        m_dismissed.erase(m_dismissed.begin(),
                          m_dismissed.end() - kMaxDismissed);
    }
    Wh_SetStringValue(L"notificationsClearedUntil",
                      std::to_wstring(m_clearedUntil).c_str());
    std::wstring dismissed;
    for (long long id : m_dismissed) {
        dismissed += (dismissed.empty() ? L"" : L",") + std::to_wstring(id);
    }
    Wh_SetStringValue(L"notificationsDismissed", dismissed.c_str());
}

// The latest toasts, newest first, without the ones cleared here.
void NotificationPanel::Load() {
    LoadCleared();
    m_doNotDisturb = IsDoNotDisturbOn();
    m_notifications.clear();
    if (!m_sqliteLoaded) {
        m_sqliteLoaded = true;
        if (!m_sqlite.Load()) {
            m_sqlite.Unload();
        }
    }
    if (!m_sqlite.module || !m_sqlite.open_v2) {
        return;
    }
    const std::string path = GetNotificationDatabasePath();
    void* db = nullptr;
    if (path.empty() ||
        m_sqlite.open_v2(path.c_str(), &db, Sqlite::kOpenReadOnly, nullptr) !=
            0) {
        if (db) {
            m_sqlite.close(db);
        }
        return;
    }
    m_sqlite.busy_timeout(db, 500);
    void* statement = nullptr;
    if (m_sqlite.prepare_v2(
            db,
            "SELECT n.Id, h.PrimaryId, n.Payload, n.ArrivalTime FROM "
            "Notification n JOIN NotificationHandler h ON h.RecordId = "
            "n.HandlerId WHERE n.Type = 'toast' AND n.ArrivalTime > ?1 "
            "ORDER BY n.ArrivalTime DESC LIMIT ?2",
            -1, &statement, nullptr) == 0) {
        m_sqlite.bind_int64(statement, 1, m_clearedUntil);
        m_sqlite.bind_int64(statement, 2,
                            kMaxNotifications + (long long)m_dismissed.size());
        while (m_sqlite.step(statement) == Sqlite::kRow &&
               (int)m_notifications.size() < kMaxNotifications) {
            Notification notification;
            notification.id = m_sqlite.column_int64(statement, 0);
            if (std::find(m_dismissed.begin(), m_dismissed.end(),
                          notification.id) != m_dismissed.end()) {
                continue;
            }
            if (auto app = (PCWSTR)m_sqlite.column_text16(statement, 1)) {
                notification.appId = app;
            }
            const auto payload = (const char*)m_sqlite.column_blob(statement, 2);
            const int size = m_sqlite.column_bytes(statement, 2);
            notification.arrival = m_sqlite.column_int64(statement, 3);
            const auto texts = payload ? ToastTexts(payload, size)
                                       : std::vector<std::wstring>{};
            if (!texts.empty()) {
                notification.title = texts[0];
            }
            for (size_t i = 1; i < texts.size(); i++) {
                notification.text += (i > 1 ? L" " : L"") + texts[i];
            }
            if (payload) {
                notification.reply =
                    ParseToastReply(notification.appId, payload, size);
            }
            if (!notification.title.empty() || !notification.text.empty()) {
                m_notifications.push_back(std::move(notification));
            }
        }
        m_sqlite.finalize(statement);
    }
    m_sqlite.close(db);
    m_first = std::clamp(
        m_first, 0,
        std::max(0, (int)m_notifications.size() - kMaxNotificationsShown));
}

void NotificationPanel::Layout() {
    const float s = m_scale;
    const float width = std::round(kNotificationsWidth * s);
    const float padding = std::round(12 * s);
    const float gap = std::round(8 * s);
    float y = padding;
    // "Notifications"  ...  "Clear all".
    const float header = std::round(kHeaderHeight * s);
    m_header = {padding + std::round(4 * s), y,
                width - padding - std::round(4 * s), y + header};
    m_clearAll = {m_header.right - std::round(110 * s), y, m_header.right,
                  y + header};
    y += header;
    // "Do not disturb" with its switch.
    // Taller rows: a name and what it does.
    const float row = std::round(kSwitchRowHeight * s);
    m_dndRow = {padding, y, width - padding, y + row};
    const float toggleWidth = std::round(36 * s);
    const float toggleHeight = std::round(20 * s);
    m_dndToggle = {m_dndRow.right - std::round(10 * s) - toggleWidth,
                   std::round((m_dndRow.top + m_dndRow.bottom - toggleHeight) /
                              2),
                   m_dndRow.right - std::round(10 * s),
                   std::round((m_dndRow.top + m_dndRow.bottom - toggleHeight) /
                              2) +
                       toggleHeight};
    // The other switches, under it.
    auto nextRow = [&](D2D1_RECT_F* rowRect, D2D1_RECT_F* toggle) {
        y += row + std::round(4 * s);
        *rowRect = {padding, y, width - padding, y + row};
        *toggle = {m_dndToggle.left, m_dndToggle.top + (y - m_dndRow.top),
                   m_dndToggle.right, m_dndToggle.bottom + (y - m_dndRow.top)};
    };
    nextRow(&m_bannerRow, &m_bannerToggle);
    nextRow(&m_minimizedRow, &m_minimizedToggle);
    nextRow(&m_contentRow, &m_contentToggle);
    y += row + gap;

    m_cards.clear();
    const int shown = std::min((int)m_notifications.size() - m_first,
                               kMaxNotificationsShown);
    const float card = std::round(kNotificationHeight * s);
    const float replyRow = std::round(kBannerReplyHeight * s);
    for (int i = 0; i < shown; i++) {
        // The one being answered has its reply box under it; one being
        // removed closes its room.
        const long long id = m_notifications[m_first + i].id;
        float room = 1;
        for (const auto& motion : m_motions) {
            if (motion.id == id) {
                room = (float)std::clamp(motion.room, 0.0, 1.0);
            }
        }
        const float height = (card + (id == m_replyId ? replyRow : 0)) * room;
        m_cards.push_back({padding, y, width - padding, y + height});
        y += (height + gap) * room;
    }
    if (shown <= 0) {
        // "No notifications".
        y += std::round(40 * s);
    }
    const float link = std::round(kFooterHeight * s);
    m_link = {padding, y, width - padding, y + link};
    y += link + padding / 2;
    m_size = {(LONG)width, (LONG)std::ceil(y)};
}

void NotificationPanel::CardParts(const D2D1_RECT_F& card,
                                  D2D1_RECT_F* replyButton,
                                  D2D1_RECT_F* field,
                                  D2D1_RECT_F* send) const {
    const float s = m_scale;
    const float inner = std::round(12 * s);
    const float base = card.top + std::round(kNotificationHeight * s);
    const float buttonHeight = std::round(24 * s);
    *replyButton = {card.right - inner - std::round(84 * s),
                    base - inner - buttonHeight + std::round(4 * s),
                    card.right - inner, base - inner + std::round(4 * s)};
    const float fieldHeight = std::round(32 * s);
    *field = {card.left + inner, base, card.right - inner,
              base + fieldHeight};
    *send = {field->right - fieldHeight, field->top, field->right,
             field->bottom};
}

NotificationPanel::Hit NotificationPanel::HitTest(POINT pt) const {
    for (size_t i = 0; i < m_cards.size(); i++) {
        if (!PointInRect(pt, m_cards[i])) {
            continue;
        }
        const Notification& n = m_notifications[m_first + i];
        if (n.reply.CanReply()) {
            D2D1_RECT_F replyButton, field, send;
            CardParts(m_cards[i], &replyButton, &field, &send);
            if (n.id == m_replyId) {
                if (PointInRect(pt, send) && !m_replyText.empty()) {
                    return {Part::ReplySend, m_first + (int)i};
                }
                if (PointInRect(pt, field)) {
                    return {Part::ReplyField, m_first + (int)i};
                }
            } else if (PointInRect(pt, replyButton)) {
                return {Part::ReplyButton, m_first + (int)i};
            }
        }
        // The "x" at the top right of the card.
        const float size = std::round(28 * m_scale);
        if (pt.x >= m_cards[i].right - size && pt.y < m_cards[i].top + size) {
            return {Part::Dismiss, m_first + (int)i};
        }
        return {Part::Card, m_first + (int)i};
    }
    if (!m_notifications.empty() && PointInRect(pt, m_clearAll)) {
        return {Part::ClearAll};
    }
    if (PointInRect(pt, m_dndRow)) {
        return {Part::DndToggle};
    }
    if (PointInRect(pt, m_contentRow)) {
        return {Part::ContentToggle};
    }
    if (PointInRect(pt, m_bannerRow)) {
        return {Part::BannerToggle};
    }
    if (PointInRect(pt, m_minimizedRow)) {
        return {Part::MinimizedToggle};
    }
    if (PointInRect(pt, m_link)) {
        return {Part::Link};
    }
    return {};
}

// The app's name, as the Start menu shows it.
// An app's name, as the Start menu shows it, by its AppUserModelID;
// remembered.
std::wstring AppDisplayName(const std::wstring& appId) {
    static std::vector<std::pair<std::wstring, std::wstring>> names;
    for (const auto& [id, name] : names) {
        if (id == appId) {
            return name;
        }
    }
    std::wstring name;
    winrt::com_ptr<IShellItem> item;
    if (!appId.empty() &&
        SUCCEEDED(SHCreateItemInKnownFolder(kFOLDERID_AppsFolder, 0,
                                            appId.c_str(),
                                            IID_PPV_ARGS(item.put())))) {
        PWSTR display = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &display))) {
            name = display;
            CoTaskMemFree(display);
        }
    }
    if (name.empty()) {
        // "Company.App_hash!App" or a program's path: the readable part.
        name = appId;
        if (size_t slash = name.find_last_of(L'\\'); slash != name.npos) {
            name = name.substr(slash + 1);
            // "notepad.exe": the program's name.
            if (size_t dot = name.find_last_of(L'.'); dot != name.npos) {
                name = name.substr(0, dot);
            }
            if (!name.empty()) {
                name[0] = towupper(name[0]);
            }
            names.push_back({appId, name});
            return name;
        }
        if (size_t bang = name.find(L'!'); bang != name.npos) {
            name = name.substr(0, bang);
        }
        if (size_t underscore = name.find(L'_'); underscore != name.npos) {
            name = name.substr(0, underscore);
        }
        if (size_t dot = name.find_last_of(L'.'); dot != name.npos &&
                                                  dot + 1 < name.size()) {
            name = name.substr(dot + 1);
        }
    }
    names.push_back({appId, name});
    return name;
}

const std::wstring& NotificationPanel::AppName(const std::wstring& appId) {
    for (const auto& [id, name] : m_names) {
        if (id == appId) {
            return name;
        }
    }
    m_names.push_back({appId, AppDisplayName(appId)});
    return m_names.back().second;
}

// An app's icon for the island and its panels, at `size` pixels: from
// explorer, which has the shell give the icons of packaged apps too (not
// always given in the island's process), or loaded here if it can't.
winrt::com_ptr<ID2D1Bitmap> LoadAppIcon(ID2D1RenderTarget* target,
                                        IWICImagingFactory* wic,
                                        HWND from,
                                        const std::wstring& appId,
                                        int size) {
    winrt::com_ptr<ID2D1Bitmap> bitmap;
    if (!target || !wic) {
        return bitmap;
    }
    auto convert = [&](IWICBitmapSource* source) {
        winrt::com_ptr<IWICFormatConverter> converter;
        if (SUCCEEDED(wic->CreateFormatConverter(converter.put())) &&
            SUCCEEDED(converter->Initialize(
                source, GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone, nullptr, 0,
                WICBitmapPaletteTypeMedianCut))) {
            target->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                              bitmap.put());
        }
    };
    HICON taskbarIcon = nullptr;
    if (HWND taskbar = FindTaskbarWnd()) {
        IpcWriter writer;
        writer.Put(size);
        writer.PutString(appId);
        COPYDATASTRUCT copyData{kIpcAppIcon, (DWORD)writer.bytes.size(),
                                writer.bytes.data()};
        DWORD_PTR result = 0;
        // From the island's window (explorer only takes requests from it).
        if (SendMessageTimeout(taskbar, WM_COPYDATA,
                               (WPARAM)(g_islandWnd ? (HWND)g_islandWnd : from),
                               (LPARAM)&copyData,
                               SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 2000,
                               &result) &&
            result) {
            taskbarIcon = CopyIcon((HICON)result);
        }
    }
    if (taskbarIcon) {
        winrt::com_ptr<IWICBitmap> wicBitmap;
        if (SUCCEEDED(wic->CreateBitmapFromHICON(taskbarIcon,
                                                 wicBitmap.put()))) {
            convert(wicBitmap.get());
        }
        DestroyIcon(taskbarIcon);
    }
    if (!bitmap) {
        if (HBITMAP icon = LoadAppIconBitmap(nullptr, appId, size)) {
            winrt::com_ptr<IWICBitmap> wicBitmap;
            if (SUCCEEDED(wic->CreateBitmapFromHBITMAP(
                    icon, nullptr, WICBitmapUsePremultipliedAlpha,
                    wicBitmap.put()))) {
                convert(wicBitmap.get());
            }
            DeleteObject(icon);
        }
    }
    return bitmap;
}

ID2D1Bitmap* NotificationPanel::AppBitmap(ID2D1RenderTarget* target,
                                          const std::wstring& appId) {
    for (const auto& [id, bitmap] : m_bitmaps) {
        if (id == appId) {
            return bitmap.get();
        }
    }
    winrt::com_ptr<ID2D1Bitmap> bitmap = LoadAppIcon(
        target, m_wic.get(), m_hwnd, appId, (int)std::round(32 * m_scale));
    m_bitmaps.push_back({appId, bitmap});
    return bitmap.get();
}

// "now", "5 min", the time today, "yesterday", or the date.
std::wstring NotificationPanel::TimeText(long long arrival) const {
    return NotificationTimeText(arrival);
}

void NotificationPanel::Draw(ID2D1RenderTarget* target,
                             ID2D1SolidColorBrush* brush) {
    if (!m_headerFormat || !m_appFormat || !m_timeFormat || !m_titleFormat ||
        !m_textFormat || !m_linkFormat || !m_iconFormat) {
        return;
    }
    const float s = m_scale;
    const float padding = std::round(12 * s);

    PCWSTR title = Tr(L"Notifications", L"Notificações");
    brush->SetColor(Fg(0.95f));
    target->DrawText(title, (UINT32)wcslen(title), m_headerFormat.get(),
                     m_header, brush);
    if (!m_notifications.empty()) {
        PCWSTR clear = Tr(L"Clear all", L"Limpar tudo");
        brush->SetColor(Fg(m_hover.part == Part::ClearAll ? 0.95f : 0.55f));
        target->DrawText(clear, (UINT32)wcslen(clear), m_timeFormat.get(),
                         m_clearAll, brush);
    }

    // "Do not disturb", with the moon, and "Show content", with the eye.
    DrawSwitchRow(target, brush, m_dndRow, m_dndToggle, kGlyphMoon,
                  D2D1::ColorF(0.55f, 0.45f, 1.0f, 1),
                  Tr(L"Do not disturb", L"Não incomodar"),
                  Tr(L"Windows' own: no notification sounds",
                     L"O do Windows: sem os sons das notificações"),
                  m_dndKnob, m_hover.part == Part::DndToggle);
    DrawSwitchRow(target, brush, m_bannerRow, m_bannerToggle, kGlyphBell,
                  D2D1::ColorF(0.04f, 0.52f, 1.0f, 1),
                  Tr(L"New ones in the pill", L"Avisos na pílula"),
                  Tr(L"The pill opens with each new one",
                     L"A pílula se abre com cada notificação nova"),
                  m_bannerKnob, m_hover.part == Part::BannerToggle);
    DrawSwitchRow(target, brush, m_minimizedRow, m_minimizedToggle,
                  kGlyphCollapse, D2D1::ColorF(0.04f, 0.52f, 1.0f, 1),
                  Tr(L"Also with the pill minimized",
                     L"Também com a pílula minimizada"),
                  Tr(L"Otherwise, only the app's icon peeks out",
                     L"Senão, só o ícone do app aparece na linha"),
                  m_minimizedKnob, m_hover.part == Part::MinimizedToggle,
                  g_bannerEnabled);
    DrawSwitchRow(target, brush, m_contentRow, m_contentToggle, kGlyphShow,
                  D2D1::ColorF(0.04f, 0.52f, 1.0f, 1),
                  Tr(L"Show their content", L"Mostrar o conteúdo"),
                  Tr(L"The title and the text, in the pill and here",
                     L"O título e o texto, na pílula e aqui"),
                  m_contentKnob, m_hover.part == Part::ContentToggle);

    if (m_cards.empty()) {
        PCWSTR empty = Tr(L"No notifications", L"Nenhuma notificação");
        brush->SetColor(Fg(0.5f));
        const float top = m_contentRow.bottom + std::round(8 * s);
        target->DrawText(empty, (UINT32)wcslen(empty), m_linkFormat.get(),
                         {padding, top, m_size.cx - padding,
                          top + std::round(40 * s)},
                         brush);
    }

    for (size_t i = 0; i < m_cards.size(); i++) {
        const Notification& n = m_notifications[m_first + i];
        const D2D1_RECT_F& card = m_cards[i];
        // Moved sideways, fading the farther it goes.
        float offset = 0;
        float room = 1;
        for (const auto& motion : m_motions) {
            if (motion.id == n.id) {
                offset = (float)motion.offset;
                room = (float)std::clamp(motion.room, 0.0, 1.0);
            }
        }
        if (room < 0.02f) {
            continue;
        }
        D2D1_MATRIX_3X2_F transform;
        target->GetTransform(&transform);
        target->SetTransform(D2D1::Matrix3x2F::Translation(offset, 0) *
                             transform);
        winrt::com_ptr<ID2D1Layer> cardLayer;
        const float fade =
            std::clamp(1 - std::fabs(offset) / (card.right - card.left), 0.0f,
                       1.0f) *
            room;
        target->CreateLayer(nullptr, cardLayer.put());
        target->PushLayer(D2D1::LayerParameters(
                              {card.left - 1, card.top - 1, card.right + 1,
                               card.bottom + 1},
                              nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                              D2D1::IdentityMatrix(), fade),
                          cardLayer.get());
        // The card stays under the mouse over its buttons too.
        const bool hover = (m_hover.part == Part::Card ||
                            m_hover.part == Part::Dismiss ||
                            m_hover.part == Part::ReplyButton) &&
                           m_hover.index == m_first + (int)i;
        const float radius = std::round(16 * s);
        brush->SetColor(Fg(hover ? 0.15f : 0.09f));
        target->FillRoundedRectangle({card, radius, radius}, brush);

        const float inner = std::round(12 * s);
        const float icon = std::round(18 * s);
        const float top = card.top + std::round(10 * s);
        const D2D1_RECT_F iconRect{card.left + inner, top,
                                   card.left + inner + icon, top + icon};
        if (ID2D1Bitmap* bitmap = AppBitmap(target, n.appId)) {
            target->DrawBitmap(bitmap, iconRect);
        } else {
            brush->SetColor(Fg(0.3f));
            target->FillRoundedRectangle({iconRect, icon / 4, icon / 4},
                                         brush);
        }
        // The time, or the "x" under the mouse.
        const float right = card.right - inner;
        const D2D1_RECT_F appLine{iconRect.right + std::round(8 * s), top,
                                  right, top + icon};
        const std::wstring& name = AppName(n.appId);
        brush->SetColor(Fg(0.6f));
        target->DrawText(name.c_str(), (UINT32)name.size(), m_appFormat.get(),
                         {appLine.left, appLine.top,
                          appLine.right - std::round(70 * s), appLine.bottom},
                         brush);
        if (hover) {
            const float circle = std::round(9 * s);
            const D2D1_POINT_2F center{right - circle + std::round(2 * s),
                                       top + icon / 2};
            brush->SetColor(Fg(m_hover.part == Part::Dismiss ? 0.35f : 0.18f));
            target->FillEllipse({center, circle, circle}, brush);
            brush->SetColor(Fg(0.95f));
            target->DrawText(&kGlyphClose, 1, m_iconFormat.get(),
                             {center.x - circle, center.y - circle,
                              center.x + circle, center.y + circle},
                             brush);
        } else {
            const std::wstring time = TimeText(n.arrival);
            target->DrawText(time.c_str(), (UINT32)time.size(),
                             m_timeFormat.get(), appLine, brush);
        }

        const float line = std::round(19 * s);
        const float textTop = top + icon + std::round(4 * s);
        // Only the app, when the content is hidden.
        const bool content = g_showNotificationContent;
        const std::wstring cardTitle =
            content ? n.title : std::wstring(Tr(L"Notification", L"Notificação"));
        const std::wstring cardText =
            content ? n.text
                    : std::wstring(Tr(L"Content hidden", L"Conteúdo oculto"));
        brush->SetColor(Fg(0.95f));
        target->DrawText(cardTitle.c_str(), (UINT32)cardTitle.size(),
                         m_titleFormat.get(),
                         {card.left + inner, textTop, card.right - inner,
                          textTop + line},
                         brush);
        // "Reply" on the card under the mouse; the box when answering.
        if (n.reply.CanReply()) {
            D2D1_RECT_F replyButton, field, send;
            CardParts(card, &replyButton, &field, &send);
            if (n.id == m_replyId) {
                const float fieldRadius = (field.bottom - field.top) / 2;
                brush->SetColor(Fg(0.14f));
                target->FillRoundedRectangle({field, fieldRadius, fieldRadius},
                                             brush);
                brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, 1));
                target->DrawRoundedRectangle(
                    {{field.left + 0.75f, field.top + 0.75f,
                      field.right - 0.75f, field.bottom - 0.75f},
                     fieldRadius,
                     fieldRadius},
                    brush, 1.5f);
                const D2D1_RECT_F textRect{field.left + inner, field.top,
                                           send.left, field.bottom};
                const bool empty = m_replyText.empty();
                const std::wstring shown =
                    empty ? (n.reply.placeholder.empty()
                                 ? std::wstring(Tr(L"Reply", L"Responder"))
                                 : n.reply.placeholder)
                          : m_replyText;
                float caretX = textRect.left;
                winrt::com_ptr<IDWriteTextLayout> layout;
                if (SUCCEEDED(m_dwrite->CreateTextLayout(
                        shown.c_str(), (UINT32)shown.size(), m_textFormat.get(),
                        textRect.right - textRect.left,
                        textRect.bottom - textRect.top, layout.put()))) {
                    DWRITE_TEXT_METRICS metrics;
                    layout->GetMetrics(&metrics);
                    const float overflow = std::max(
                        0.0f, metrics.widthIncludingTrailingWhitespace -
                                  (textRect.right - textRect.left));
                    layout->SetTrimming(
                        &kNoTrimming,
                        nullptr);
                    target->PushAxisAlignedClip(textRect,
                                                D2D1_ANTIALIAS_MODE_ALIASED);
                    brush->SetColor(
                        Fg(empty ? 0.45f : 0.95f));
                    target->DrawTextLayout(
                        {textRect.left - (empty ? 0 : overflow), textRect.top},
                        layout.get(), brush);
                    target->PopAxisAlignedClip();
                    if (!empty) {
                        caretX = std::min(
                            textRect.left +
                                metrics.widthIncludingTrailingWhitespace -
                                overflow,
                            textRect.right);
                    }
                }
                const float caretMiddle = (field.top + field.bottom) / 2;
                brush->SetColor(Fg(0.9f));
                target->FillRectangle(
                    {caretX + 1, caretMiddle - std::round(8 * s), caretX + 2.5f,
                     caretMiddle + std::round(8 * s)},
                    brush);
                if (!empty) {
                    const float sendRadius =
                        (send.bottom - send.top) / 2 - std::round(3 * s);
                    const D2D1_POINT_2F sendCenter{(send.left + send.right) / 2,
                                                   (send.top + send.bottom) / 2};
                    brush->SetColor(D2D1::ColorF(
                        0.04f, 0.52f, 1.0f,
                        m_hover.part == Part::ReplySend ? 0.85f : 1.0f));
                    target->FillEllipse({sendCenter, sendRadius, sendRadius},
                                        brush);
                    brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
                    target->DrawText(&kGlyphSend, 1, m_iconFormat.get(), send,
                                     brush);
                }
            } else if (hover) {
                const float buttonRadius =
                    (replyButton.bottom - replyButton.top) / 2;
                brush->SetColor(Fg(m_hover.part == Part::ReplyButton ? 0.3f : 0.2f));
                target->FillRoundedRectangle(
                    {replyButton, buttonRadius, buttonRadius}, brush);
                // "Reply" (the app's own button is "Send", inside the box).
                const std::wstring label = Tr(L"Reply", L"Responder");
                brush->SetColor(Fg(0.95f));
                m_appFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                target->DrawText(label.c_str(), (UINT32)label.size(),
                                 m_appFormat.get(), replyButton, brush);
                m_appFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            }
        }
        brush->SetColor(Fg(content ? 0.7f : 0.45f));
        target->DrawText(cardText.c_str(), (UINT32)cardText.size(),
                         m_textFormat.get(),
                         {card.left + inner, textTop + line,
                          card.right - inner, textTop + line * 2},
                         brush);
        target->PopLayer();
        target->SetTransform(transform);
    }

    if ((int)m_notifications.size() > kMaxNotificationsShown) {
        const std::wstring more =
            std::to_wstring(m_first + 1) + L"–" +
            std::to_wstring(std::min((int)m_notifications.size(),
                                     m_first + kMaxNotificationsShown)) +
            Tr(L" of ", L" de ") + std::to_wstring(m_notifications.size()) +
            L"  ·  ";
        PCWSTR link = Tr(L"Windows notification center",
                         L"Central do Windows");
        const std::wstring footer = more + link;
        brush->SetColor(Fg(m_hover.part == Part::Link ? 0.9f : 0.5f));
        target->DrawText(footer.c_str(), (UINT32)footer.size(),
                         m_linkFormat.get(), m_link, brush);
    } else {
        PCWSTR link = Tr(L"Open the Windows notification center",
                         L"Abrir a central de notificações do Windows");
        brush->SetColor(Fg(m_hover.part == Part::Link ? 0.9f : 0.5f));
        target->DrawText(link, (UINT32)wcslen(link), m_linkFormat.get(),
                         m_link, brush);
    }
}

// The switches slide to their state (they start there).
bool NotificationPanel::AnimateContents(double dt) {
    const double dnd = m_doNotDisturb ? 1 : 0;
    const double content = g_showNotificationContent ? 1 : 0;
    if (m_dndKnob < 0) {
        m_dndKnob = dnd;
    }
    if (m_contentKnob < 0) {
        m_contentKnob = content;
    }
    const bool a = SpringTowards(m_dndKnob, m_dndKnobVelocity, dnd, 0.3, dt,
                                 kBounceDamping);
    const bool b = SpringTowards(m_contentKnob, m_contentKnobVelocity, content,
                                 0.3, dt, kBounceDamping);
    const double banner = g_bannerEnabled ? 1 : 0;
    const double minimized = g_bannerWhenMinimized ? 1 : 0;
    if (m_bannerKnob < 0) {
        m_bannerKnob = banner;
    }
    if (m_minimizedKnob < 0) {
        m_minimizedKnob = minimized;
    }
    const bool c = SpringTowards(m_bannerKnob, m_bannerKnobVelocity, banner,
                                 0.3, dt, kBounceDamping);
    const bool d = SpringTowards(m_minimizedKnob, m_minimizedKnobVelocity,
                                 minimized, 0.3, dt, kBounceDamping);

    // The cards: sliding, then closing their room, then gone.
    bool cards = false;
    std::vector<long long> removed;
    for (auto& motion : m_motions) {
        if (motion.id == m_dragId && m_dragMoved) {
            cards = true;
            continue;
        }
        if (motion.delay > 0) {
            motion.delay -= dt;
            cards = true;
            continue;
        }
        if (SpringTowards(motion.offset, motion.velocity, motion.target,
                          motion.removing ? 0.35 : 0.4, dt,
                          motion.removing ? 1.0 : kBounceDamping)) {
            cards = true;
        }
        if (motion.removing &&
            std::fabs(motion.offset) >= std::fabs(motion.target) * 0.85) {
            if (SpringTowards(motion.room, motion.roomVelocity, 0, 0.3, dt)) {
                cards = true;
            } else {
                removed.push_back(motion.id);
            }
        }
    }
    // Settled back in place: no longer moving.
    std::erase_if(m_motions, [&](const CardMotion& motion) {
        return !motion.removing && motion.offset == 0 &&
               motion.velocity == 0 && motion.id != m_dragId;
    });
    if (m_clearAllPending) {
        // All gone: everything that arrived until now is cleared.
        if (std::all_of(m_motions.begin(), m_motions.end(),
                        [](const CardMotion& motion) {
                            return motion.removing && motion.room <= 0 &&
                                   motion.roomVelocity == 0;
                        })) {
            m_clearAllPending = false;
            for (const auto& n : m_notifications) {
                m_clearedUntil = std::max(m_clearedUntil, n.arrival);
            }
            m_dismissed.clear();
            m_motions.clear();
            SaveCleared();
            Load();
            cards = true;
        }
    } else if (!removed.empty()) {
        std::erase_if(m_motions, [&](const CardMotion& motion) {
            return std::find(removed.begin(), removed.end(), motion.id) !=
                   removed.end();
        });
        m_dismissed.insert(m_dismissed.end(), removed.begin(), removed.end());
        SaveCleared();
        Load();
        cards = true;
    }
    if (cards || !removed.empty()) {
        Layout();
    }
    return a || b || c || d || cards;
}

// A row with an icon (colored when on), a name, and a switch on the right
// whose knob is at `knob` (0 off, 1 on).
void NotificationPanel::DrawSwitchRow(ID2D1RenderTarget* target,
                                      ID2D1SolidColorBrush* brush,
                                      const D2D1_RECT_F& row,
                                      const D2D1_RECT_F& toggle,
                                      WCHAR glyph,
                                      const D2D1_COLOR_F& glyphColor,
                                      PCWSTR name,
                                      PCWSTR description,
                                      double knob,
                                      bool hover,
                                      bool enabled) {
    const float s = m_scale;
    const float on = (float)std::clamp(knob < 0 ? 0.0 : knob, 0.0, 1.0);
    // Dimmed while it doesn't apply.
    winrt::com_ptr<ID2D1Layer> layer;
    if (!enabled) {
        target->CreateLayer(nullptr, layer.put());
        target->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                  D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                  D2D1::IdentityMatrix(), 0.4f),
            layer.get());
        hover = false;
    }
    const float radius = std::round(12 * s);
    brush->SetColor(Fg(hover ? 0.13f : 0.08f));
    target->FillRoundedRectangle({row, radius, radius}, brush);
    const float icon = std::round(36 * s);
    auto mix = [on](float a, float b) { return a + (b - a) * on; };
    const D2D1_COLOR_F ink = Fg(0.95f);
    brush->SetColor(D2D1::ColorF(mix(ink.r, glyphColor.r),
                                 mix(ink.g, glyphColor.g),
                                 mix(ink.b, glyphColor.b), ink.a));
    target->DrawText(&glyph, 1, m_iconFormat.get(),
                     {row.left, row.top, row.left + icon, row.bottom}, brush);
    const float middle = (row.top + row.bottom) / 2;
    const float textRight = toggle.left - std::round(8 * s);
    brush->SetColor(Fg(0.95f));
    target->DrawText(name, (UINT32)wcslen(name), m_textFormat.get(),
                     {row.left + icon, middle - std::round(19 * s), textRight,
                      middle + std::round(1 * s)},
                     brush);
    brush->SetColor(Fg(0.5f));
    target->DrawText(description, (UINT32)wcslen(description),
                     m_appFormat.get(),
                     {row.left + icon, middle + std::round(1 * s), textRight,
                      middle + std::round(18 * s)},
                     brush);
    // The track goes from gray to blue; the knob slides, a bit springy.
    const float trackRadius = (toggle.bottom - toggle.top) / 2;
    const D2D1_COLOR_F track = Fg(0.22f);
    brush->SetColor(D2D1::ColorF(mix(track.r, 0.04f), mix(track.g, 0.52f),
                                 mix(track.b, 1), mix(track.a, 1)));
    target->FillRoundedRectangle({toggle, trackRadius, trackRadius}, brush);
    const float knobRadius = trackRadius - std::round(2 * s);
    const float travel = toggle.right - toggle.left - trackRadius * 2;
    const float x = toggle.left + trackRadius +
                    travel * (float)std::clamp(knob < 0 ? 0.0 : knob, -0.1, 1.1);
    brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
    target->FillEllipse({{x, toggle.top + trackRadius}, knobRadius, knobRadius},
                        brush);
    if (layer) {
        target->PopLayer();
    }
}

bool NotificationPanel::OnMouseMove(POINT pt) {
    // Dragging a card sideways: it follows the mouse.
    if (m_dragId >= 0 && (GetKeyState(VK_LBUTTON) & 0x8000)) {
        const int dx = pt.x - m_dragStartX;
        if (!m_dragMoved && std::abs(dx) > std::round(6 * m_scale)) {
            m_dragMoved = true;
        }
        if (m_dragMoved) {
            CardMotion* motion = Motion(m_dragId, true);
            motion->offset = motion->target = dx;
            motion->velocity = 0;
            return true;
        }
        return false;
    }
    const Hit hover = HitTest(pt);
    if (hover == m_hover) {
        return false;
    }
    m_hover = hover;
    return true;
}

bool NotificationPanel::OnMouseLeave() {
    if (m_hover.part == Part::None) {
        return false;
    }
    m_hover = {};
    return true;
}

NotificationPanel::CardMotion* NotificationPanel::Motion(long long id,
                                                       bool create) {
    for (auto& motion : m_motions) {
        if (motion.id == id) {
            return &motion;
        }
    }
    if (!create) {
        return nullptr;
    }
    m_motions.push_back({id});
    return &m_motions.back();
}

// Slides a card away (right for 1, left for -1), then closes its room.
void NotificationPanel::Remove(long long id, int direction, double delay) {
    CardMotion* motion = Motion(id, true);
    motion->removing = true;
    motion->delay = delay;
    motion->target = direction * (kNotificationsWidth * m_scale * 1.1);
    if (id == m_replyId) {
        m_replyId = -1;
        m_replyText.clear();
    }
    StartIslandAnimation();
}

bool NotificationPanel::OnMouseDown(POINT pt) {
    const Hit hit = HitTest(pt);
    if (hit.part != Part::Card || hit.index < 0 ||
        hit.index >= (int)m_notifications.size()) {
        return false;
    }
    // Maybe a drag: decided once the mouse moves.
    m_dragId = m_notifications[hit.index].id;
    m_dragStartX = pt.x;
    m_dragMoved = false;
    SetCapture(m_hwnd);
    return false;
}

bool NotificationPanel::OnMouseUp(POINT pt) {
    // The end of a drag: away if far enough, otherwise back.
    if (m_dragId >= 0) {
        const long long id = m_dragId;
        m_dragId = -1;
        if (GetCapture() == m_hwnd) {
            ReleaseCapture();
        }
        if (m_dragMoved) {
            CardMotion* motion = Motion(id, true);
            if (std::fabs(motion->offset) > kNotificationsWidth * m_scale * 0.3) {
                Remove(id, motion->offset > 0 ? 1 : -1);
            } else {
                motion->target = 0;
                StartIslandAnimation();
            }
            return true;
        }
    }
    const Hit hit = HitTest(pt);
    switch (hit.part) {
        case Part::Link:
            Close();
            PressKeys({VK_LWIN, 'N'});
            return false;
        case Part::Card:
            if (hit.index >= 0 && hit.index < (int)m_notifications.size()) {
                const Notification n = m_notifications[hit.index];
                Close();
                OpenNotification(n.appId, n.reply);
            }
            return false;
        case Part::ReplyButton:
            if (hit.index >= 0 && hit.index < (int)m_notifications.size()) {
                m_replyId = m_notifications[hit.index].id;
                m_replyText.clear();
                Layout();
            }
            return true;
        case Part::ReplyField:
            return false;
        case Part::ReplySend:
            SendReply();
            return true;
        case Part::Dismiss:
            if (hit.index >= 0 && hit.index < (int)m_notifications.size()) {
                Remove(m_notifications[hit.index].id, 1);
                m_hover = {};
            }
            return true;
        case Part::ClearAll: {
            // The cards slide away one after the other; then everything
            // that arrived until now is cleared.
            double delay = 0;
            for (int i = m_first;
                 i < (int)m_notifications.size() &&
                 i < m_first + kMaxNotificationsShown;
                 i++) {
                Remove(m_notifications[i].id, 1, delay);
                delay += 0.045;
            }
            m_clearAllPending = true;
            m_hover = {};
            if (m_motions.empty()) {
                StartIslandAnimation();
            }
            return true;
        }
        case Part::DndToggle:
            if (SetDoNotDisturb(!m_doNotDisturb)) {
                m_doNotDisturb = !m_doNotDisturb;
                if (g_island) {
                    g_island->OnDoNotDisturbChanged();
                }
            }
            StartIslandAnimation();
            return true;
        case Part::ContentToggle:
            g_showNotificationContent = !g_showNotificationContent;
            Wh_SetIntValue(L"notificationContent", g_showNotificationContent);
            StartIslandAnimation();
            return true;
        case Part::BannerToggle:
            g_bannerEnabled = !g_bannerEnabled;
            Wh_SetIntValue(L"bannerEnabled", g_bannerEnabled);
            StartIslandAnimation();
            return true;
        case Part::MinimizedToggle:
            if (!g_bannerEnabled) {
                return false;
            }
            g_bannerWhenMinimized = !g_bannerWhenMinimized;
            Wh_SetIntValue(L"bannerMinimized", g_bannerWhenMinimized);
            StartIslandAnimation();
            return true;
        case Part::None:
            break;
    }
    return false;
}

void NotificationPanel::SendReply() {
    for (const auto& n : m_notifications) {
        if (n.id != m_replyId || m_replyText.empty()) {
            continue;
        }
        const bool sent =
            ActivateToast(n.appId, n.reply.arguments, n.reply.activationType,
                          n.reply.inputId, m_replyText);
        Wh_Log(L"Reply from the list: %s", sent ? L"sent" : L"no activator");
        break;
    }
    m_replyId = -1;
    m_replyText.clear();
    Layout();
}

// Typing a reply: Enter sends, Ctrl+V pastes.
bool NotificationPanel::OnChar(WCHAR c) {
    if (m_replyId < 0) {
        return false;
    }
    constexpr size_t kMaxReply = 2000;
    if (c == L'\r') {
        SendReply();
    } else if (c == L'\b') {
        if (!m_replyText.empty()) {
            m_replyText.pop_back();
        }
    } else if (c == 0x16) {
        if (OpenClipboard(m_hwnd)) {
            if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
                if (auto text = (PCWSTR)GlobalLock(data)) {
                    for (PCWSTR p = text; *p; p++) {
                        if (m_replyText.size() < kMaxReply) {
                            m_replyText +=
                                (*p == L'\r' || *p == L'\n') ? L' ' : *p;
                        }
                    }
                    GlobalUnlock(data);
                }
            }
            CloseClipboard();
        }
    } else if (c >= 0x20 && m_replyText.size() < kMaxReply) {
        m_replyText += c;
    } else {
        return false;
    }
    return true;
}

// Escape leaves the reply box first.
bool NotificationPanel::OnKey(WPARAM key) {
    if (key == VK_ESCAPE && m_replyId >= 0) {
        m_replyId = -1;
        m_replyText.clear();
        Layout();
        return true;
    }
    return false;
}

bool NotificationPanel::OnWheel(int delta) {
    const int last =
        std::max(0, (int)m_notifications.size() - kMaxNotificationsShown);
    const int first = std::clamp(m_first - delta / WHEEL_DELTA, 0, last);
    if (first == m_first) {
        return false;
    }
    m_first = first;
    m_hover = {};
    Layout();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// The media controls.

constexpr float kMediaWidth = 340;

void MediaPanel::Prepare(float scale) {
    if (!m_wic) {
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(m_wic.put()));
    }
    if (scale != m_preparedScale) {
        m_preparedScale = scale;
        m_titleFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_SEMI_BOLD,
                       14 * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_textFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       12.5f * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_timeFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       11 * scale, DWRITE_TEXT_ALIGNMENT_LEADING);
        m_timeRightFormat =
            MakeFormat(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL,
                       11 * scale, DWRITE_TEXT_ALIGNMENT_TRAILING);
        m_iconFormat = MakeFormat(L"Segoe Fluent Icons",
                                  DWRITE_FONT_WEIGHT_NORMAL, 18 * scale);
        m_bigIconFormat = MakeFormat(L"Segoe Fluent Icons",
                                     DWRITE_FONT_WEIGHT_NORMAL, 26 * scale);
    }
    Layout();
}

void MediaPanel::OnOpen() {
    m_hover = Part::None;
    m_dragging = Part::None;
    m_press = {};
    m_pillKnob = -1;
    m_titleKnob = -1;
    m_timelineHover = m_volumeHover = 0;
    ReadVolume();
}

bool MediaPanel::AnimateContents(double dt) {
    bool moving = m_press.Animate(NowSeconds());
    if (SpringTowards(m_playSwap, m_playSwapVelocity, 0, 0.4, dt,
                      kBounceDamping)) {
        moving = true;
    }
    const double knob = g_mediaButtonsInPill ? 1 : 0;
    if (m_pillKnob < 0) {
        m_pillKnob = knob;
    }
    if (SpringTowards(m_pillKnob, m_pillKnobVelocity, knob, 0.3, dt,
                      kBounceDamping)) {
        moving = true;
    }
    const double titleKnob = g_mediaTitleInPill ? 1 : 0;
    if (m_titleKnob < 0) {
        m_titleKnob = titleKnob;
    }
    if (SpringTowards(m_titleKnob, m_titleKnobVelocity, titleKnob, 0.3, dt,
                      kBounceDamping)) {
        moving = true;
    }
    const bool timeline =
        m_hover == Part::Timeline || m_dragging == Part::Timeline;
    if (SpringTowards(m_timelineHover, m_timelineHoverVelocity,
                      timeline ? 1 : 0, 0.25, dt)) {
        moving = true;
    }
    const bool volume = m_hover == Part::Volume || m_dragging == Part::Volume;
    if (SpringTowards(m_volumeHover, m_volumeHoverVelocity, volume ? 1 : 0,
                      0.25, dt)) {
        moving = true;
    }
    return moving;
}

void MediaPanel::ReadVolume() {
    m_level = 0;
    m_muted = false;
    if (auto volume = g_island ? g_island->GetVolume(nullptr) : nullptr) {
        BOOL muted = FALSE;
        volume->GetMasterVolumeLevelScalar(&m_level);
        volume->GetMute(&muted);
        m_muted = muted;
    }
}

void MediaPanel::SetVolume(float level) {
    m_level = std::clamp(level, 0.0f, 1.0f);
    if (auto volume = g_island ? g_island->GetVolume(nullptr) : nullptr) {
        volume->SetMasterVolumeLevelScalar(m_level, nullptr);
        volume->SetMute(m_level <= 0.001f, nullptr);
        m_muted = m_level <= 0.001f;
    }
    if (g_island) {
        g_island->OnVolumeChanged();
    }
}

void MediaPanel::Layout() {
    const float s = m_scale;
    const float width = std::round(kMediaWidth * s);
    const float padding = std::round(16 * s);
    float y = padding;
    const float cover = std::round(64 * s);
    m_cover = {padding, y, padding + cover, y + cover};
    y += cover + std::round(14 * s);
    // The timeline, when the app says how long it is.
    if (m_state.duration > 0) {
        m_timeline = {padding, y, width - padding, y + std::round(6 * s)};
        y += std::round(6 * s) + std::round(22 * s);
    } else {
        m_timeline = {};
    }
    // Previous, play, next, in the middle.
    const float button = std::round(44 * s);
    const float big = std::round(52 * s);
    const float middle = width / 2;
    m_playPause = {middle - big / 2, y, middle + big / 2, y + big};
    m_previous = {m_playPause.left - std::round(18 * s) - button,
                  y + (big - button) / 2, m_playPause.left - std::round(18 * s),
                  y + (big + button) / 2};
    m_next = {m_playPause.right + std::round(18 * s), y + (big - button) / 2,
              m_playPause.right + std::round(18 * s) + button,
              y + (big + button) / 2};
    y += big + std::round(12 * s);
    // The volume.
    const float volumeHeight = std::round(24 * s);
    m_volumeIcon = {padding, y, padding + volumeHeight, y + volumeHeight};
    m_volume = {m_volumeIcon.right + std::round(8 * s),
                y + volumeHeight / 2 - std::round(3 * s), width - padding,
                y + volumeHeight / 2 + std::round(3 * s)};
    y += volumeHeight + std::round(10 * s);
    // "Buttons on the pill", with its switch.
    const float row = std::round(34 * s);
    m_pillButtons = {padding, y, width - padding, y + row};
    y += row + std::round(4 * s);
    m_pillTitle = {padding, y, width - padding, y + row};
    y += row + padding;
    m_size = {(LONG)width, (LONG)std::ceil(y)};
}

MediaPanel::Part MediaPanel::HitTest(POINT pt) const {
    auto around = [&](const D2D1_RECT_F& r) {
        const float grow = std::round(8 * m_scale);
        return PointInRect(pt, {r.left - 2, r.top - grow, r.right + 2,
                                r.bottom + grow});
    };
    if (PointInRect(pt, m_previous)) {
        return Part::Previous;
    }
    if (PointInRect(pt, m_playPause)) {
        return Part::PlayPause;
    }
    if (PointInRect(pt, m_next)) {
        return Part::Next;
    }
    if (m_state.canSeek && m_timeline.right > m_timeline.left &&
        around(m_timeline)) {
        return Part::Timeline;
    }
    if (PointInRect(pt, m_pillButtons)) {
        return Part::PillButtons;
    }
    if (PointInRect(pt, m_pillTitle)) {
        return Part::PillTitle;
    }
    if (around(m_volume)) {
        return Part::Volume;
    }
    return Part::None;
}

double MediaPanel::PositionAt(POINT pt) const {
    const float width = m_timeline.right - m_timeline.left;
    return width > 0 ? std::clamp((pt.x - m_timeline.left) / width, 0.0f,
                                  1.0f) *
                           m_state.duration
                     : 0;
}

// The cover, decoded once per song.
ID2D1Bitmap* MediaPanel::Thumbnail(ID2D1RenderTarget* target) {
    if (m_thumbnailSerial == m_state.thumbnailSerial) {
        return m_thumbnail.get();
    }
    m_thumbnailSerial = m_state.thumbnailSerial;
    m_thumbnail = nullptr;
    if (m_state.thumbnail.empty() || !m_wic) {
        return nullptr;
    }
    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream(m_state.thumbnail.data(),
                                    (UINT)m_state.thumbnail.size()));
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    winrt::com_ptr<IWICFormatConverter> converter;
    if (stream &&
        SUCCEEDED(m_wic->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnLoad,
            decoder.put())) &&
        SUCCEEDED(decoder->GetFrame(0, frame.put())) &&
        SUCCEEDED(m_wic->CreateFormatConverter(converter.put())) &&
        SUCCEEDED(converter->Initialize(
            frame.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        target->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                          m_thumbnail.put());
    }
    return m_thumbnail.get();
}

void MediaPanel::Draw(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush) {
    if (!m_titleFormat || !m_textFormat || !m_timeFormat ||
        !m_timeRightFormat || !m_iconFormat || !m_bigIconFormat) {
        return;
    }
    const float s = m_scale;
    const float width = (float)m_size.cx;
    const float padding = std::round(16 * s);

    // The cover (or a note), rounded.
    const float radius = std::round(10 * s);
    if (ID2D1Bitmap* cover = Thumbnail(target)) {
        winrt::com_ptr<ID2D1Factory> factory;
        target->GetFactory(factory.put());
        winrt::com_ptr<ID2D1RoundedRectangleGeometry> clip;
        factory->CreateRoundedRectangleGeometry({m_cover, radius, radius},
                                                clip.put());
        winrt::com_ptr<ID2D1Layer> layer;
        target->CreateLayer(nullptr, layer.put());
        target->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), clip.get()),
            layer.get());
        // Filled, keeping its proportions.
        const D2D1_SIZE_F size = cover->GetSize();
        const float coverSize = m_cover.right - m_cover.left;
        const float fill =
            std::max(coverSize / std::max(size.width, 1.0f),
                     coverSize / std::max(size.height, 1.0f));
        const float w = size.width * fill;
        const float h = size.height * fill;
        const float cx = (m_cover.left + m_cover.right) / 2;
        const float cy = (m_cover.top + m_cover.bottom) / 2;
        target->DrawBitmap(cover, {cx - w / 2, cy - h / 2, cx + w / 2,
                                   cy + h / 2});
        target->PopLayer();
    } else {
        brush->SetColor(Fg(0.1f));
        target->FillRoundedRectangle({m_cover, radius, radius}, brush);
        brush->SetColor(Fg(0.6f));
        target->DrawText(&kGlyphMusic, 1, m_bigIconFormat.get(), m_cover,
                         brush);
    }

    // The song, the artist, the app.
    const float textLeft = m_cover.right + std::round(14 * s);
    const float textRight = width - padding;
    const float line = std::round(20 * s);
    float textTop = m_cover.top + std::round(2 * s);
    const std::wstring title =
        m_state.present ? m_state.title
                        : std::wstring(Tr(L"Nothing playing", L"Nada tocando"));
    brush->SetColor(Fg(0.95f));
    target->DrawText(title.c_str(), (UINT32)title.size(), m_titleFormat.get(),
                     {textLeft, textTop, textRight, textTop + line}, brush);
    textTop += line;
    brush->SetColor(Fg(0.65f));
    target->DrawText(m_state.artist.c_str(), (UINT32)m_state.artist.size(),
                     m_textFormat.get(),
                     {textLeft, textTop, textRight, textTop + line}, brush);
    textTop += line;
    const std::wstring app = AppDisplayName(m_state.appId);
    brush->SetColor(Fg(0.4f));
    target->DrawText(app.c_str(), (UINT32)app.size(), m_timeFormat.get(),
                     {textLeft, textTop, textRight, textTop + line}, brush);

    // The timeline: elapsed and remaining.
    if (m_timeline.right > m_timeline.left) {
        const double position = m_dragging == Part::Timeline
                                    ? m_dragPosition
                                    : m_state.Now();
        const float done = (float)std::clamp(
            position / std::max(m_state.duration, 0.001), 0.0, 1.0);
        D2D1_RECT_F bar = m_timeline;
        {
            const float middle = (bar.top + bar.bottom) / 2;
            const float half =
                2 * s + (bar.bottom - bar.top - 4 * s) / 2 *
                            (float)std::clamp(m_timelineHover, 0.0, 1.0);
            bar.top = middle - half;
            bar.bottom = middle + half;
        }
        const float barRadius = (bar.bottom - bar.top) / 2;
        brush->SetColor(Fg(0.18f));
        target->FillRoundedRectangle({bar, barRadius, barRadius}, brush);
        brush->SetColor(Fg(0.9f));
        target->FillRoundedRectangle(
            {{bar.left, bar.top, bar.left + (bar.right - bar.left) * done,
              bar.bottom},
             barRadius,
             barRadius},
            brush);
        const std::wstring elapsed = FormatMediaTime(position);
        const std::wstring remaining =
            L"-" + FormatMediaTime(m_state.duration - position);
        const D2D1_RECT_F times{m_timeline.left,
                                m_timeline.bottom + std::round(4 * s),
                                m_timeline.right,
                                m_timeline.bottom + std::round(20 * s)};
        brush->SetColor(Fg(0.5f));
        target->DrawText(elapsed.c_str(), (UINT32)elapsed.size(),
                         m_timeFormat.get(), times, brush);
        target->DrawText(remaining.c_str(), (UINT32)remaining.size(),
                         m_timeRightFormat.get(), times, brush);
    }

    // Previous, play/pause, next: dimmed when the app doesn't allow them.
    // Pressed, a button and its circle shrink; play turning into pause
    // shrinks and grows again.
    auto button = [&](const D2D1_RECT_F& rect, WCHAR glyph, bool enabled,
                      Part part, IDWriteTextFormat* format) {
        const D2D1_POINT_2F center{(rect.left + rect.right) / 2,
                                   (rect.top + rect.bottom) / 2};
        float size = 1;
        const bool pressed = m_press.target == (int)part;
        const float press =
            pressed ? (float)m_press.Amount(NowSeconds()) : 0.0f;
        if (pressed) {
            size *= 1 - 0.18f * std::clamp(press, -0.3f, 1.0f);
        }
        const bool scaled = std::fabs(size - 1) > 0.001f;
        // On top of the panel's own placement.
        D2D1_MATRIX_3X2_F base;
        target->GetTransform(&base);
        const D2D1::Matrix3x2F& baseMatrix =
            *D2D1::Matrix3x2F::ReinterpretBaseType(&base);
        if (scaled) {
            target->SetTransform(D2D1::Matrix3x2F::Scale(size, size, center) *
                                 baseMatrix);
        }
        if ((m_hover == part || pressed) && enabled) {
            const float r = (rect.right - rect.left) / 2;
            brush->SetColor(
                Fg(0.12f + 0.1f * std::clamp(press, 0.0f, 1.0f)));
            target->FillEllipse({center, r, r}, brush);
        }
        float glyphSize = 1;
        if (part == Part::PlayPause) {
            glyphSize -= 0.55f * (float)std::clamp(m_playSwap, 0.0, 1.0);
        }
        if (glyphSize < 1) {
            target->SetTransform(D2D1::Matrix3x2F::Scale(glyphSize, glyphSize,
                                                         center) *
                                 D2D1::Matrix3x2F::Scale(size, size, center) *
                                 baseMatrix);
        }
        brush->SetColor(Fg(enabled ? 0.95f : 0.3f));
        target->DrawText(&glyph, 1, format, rect, brush);
        if (scaled || glyphSize < 1) {
            target->SetTransform(base);
        }
    };
    button(m_previous, kGlyphPreviousTrack,
           m_state.present && m_state.canPrevious, Part::Previous,
           m_iconFormat.get());
    button(m_playPause, m_state.playing ? kGlyphPause : kGlyphPlay,
           m_state.present && m_state.canPlayPause, Part::PlayPause,
           m_bigIconFormat.get());
    button(m_next, kGlyphNextTrack, m_state.present && m_state.canNext,
           Part::Next, m_iconFormat.get());

    // The volume.
    brush->SetColor(Fg(0.7f));
    const WCHAR speaker = m_muted ? kGlyphMute : kGlyphSpeaker;
    target->DrawText(&speaker, 1, m_iconFormat.get(), m_volumeIcon, brush);
    D2D1_RECT_F bar = m_volume;
    {
        const float middle = (bar.top + bar.bottom) / 2;
        const float half = 2 * s + (bar.bottom - bar.top - 4 * s) / 2 *
                                       (float)std::clamp(m_volumeHover, 0.0, 1.0);
        bar.top = middle - half;
        bar.bottom = middle + half;
    }
    const float barRadius = (bar.bottom - bar.top) / 2;
    brush->SetColor(Fg(0.18f));
    target->FillRoundedRectangle({bar, barRadius, barRadius}, brush);
    brush->SetColor(Fg(0.9f));
    target->FillRoundedRectangle(
        {{bar.left, bar.top,
          bar.left + (bar.right - bar.left) * (m_muted ? 0 : m_level),
          bar.bottom},
         barRadius,
         barRadius},
        brush);

    // "Buttons on the pill" and "Cover and title on the pill", with their
    // switches: the track goes from gray to blue; the knob slides, a bit
    // springy.
    auto switchRow = [&](const D2D1_RECT_F& row, Part part, PCWSTR label,
                         double knob, bool value) {
        const float rowRadius = std::round(10 * s);
        brush->SetColor(Fg(m_hover == part ? 0.12f : 0.07f));
        target->FillRoundedRectangle({row, rowRadius, rowRadius}, brush);
        brush->SetColor(Fg(0.9f));
        target->DrawText(label, (UINT32)wcslen(label), m_textFormat.get(),
                         {row.left + std::round(12 * s), row.top,
                          row.right - std::round(56 * s), row.bottom},
                         brush);
        const double at = knob < 0 ? (value ? 1.0 : 0.0) : knob;
        const float on = (float)std::clamp(at, 0.0, 1.0);
        const float knobAt = (float)std::clamp(at, -0.1, 1.1);
        const float toggleWidth = std::round(36 * s);
        const float toggleHeight = std::round(20 * s);
        const D2D1_RECT_F toggle{
            row.right - std::round(10 * s) - toggleWidth,
            std::round((row.top + row.bottom - toggleHeight) / 2),
            row.right - std::round(10 * s),
            std::round((row.top + row.bottom - toggleHeight) / 2) +
                toggleHeight};
        const float trackRadius = toggleHeight / 2;
        const D2D1_COLOR_F off = Fg(0.22f);
        brush->SetColor(D2D1::ColorF(off.r + (0.04f - off.r) * on,
                                     off.g + (0.52f - off.g) * on,
                                     off.b + (1.0f - off.b) * on,
                                     off.a + (1.0f - off.a) * on));
        target->FillRoundedRectangle({toggle, trackRadius, trackRadius},
                                     brush);
        brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
        const float travel = toggle.right - toggle.left - trackRadius * 2;
        target->FillEllipse(
            {{toggle.left + trackRadius + travel * knobAt,
              toggle.top + trackRadius},
             trackRadius - std::round(2 * s),
             trackRadius - std::round(2 * s)},
            brush);
    };
    switchRow(m_pillButtons, Part::PillButtons,
              Tr(L"Buttons on the pill", L"Botões na pílula"), m_pillKnob,
              g_mediaButtonsInPill);
    switchRow(m_pillTitle, Part::PillTitle,
              Tr(L"Cover and title on the pill", L"Capa e título na pílula"),
              m_titleKnob, g_mediaTitleInPill);
}

bool MediaPanel::OnMouseMove(POINT pt) {
    // Moved off a pressed button: it lets go.
    if (m_press.held && (int)HitTest(pt) != m_press.target) {
        m_press.Release();
        StartIslandAnimation();
    }
    if (m_dragging == Part::Timeline) {
        m_dragPosition = PositionAt(pt);
        return true;
    }
    if (m_dragging == Part::Volume) {
        SetVolume((pt.x - m_volume.left) / (m_volume.right - m_volume.left));
        return true;
    }
    const Part hover = HitTest(pt);
    if (hover == m_hover) {
        return false;
    }
    m_hover = hover;
    StartIslandAnimation();
    return true;
}

bool MediaPanel::OnMouseLeave() {
    if (m_press.held) {
        m_press.Release();
        StartIslandAnimation();
    }
    if (m_hover == Part::None) {
        return false;
    }
    m_hover = Part::None;
    StartIslandAnimation();
    return true;
}

bool MediaPanel::OnMouseDown(POINT pt) {
    const Part hit = HitTest(pt);
    if (hit == Part::Previous || hit == Part::PlayPause || hit == Part::Next) {
        m_press.Press((int)hit);
        StartIslandAnimation();
        return true;
    }
    if (hit == Part::Timeline) {
        m_dragging = Part::Timeline;
        m_dragPosition = PositionAt(pt);
        SetCapture(m_hwnd);
        return true;
    }
    if (hit == Part::Volume) {
        m_dragging = Part::Volume;
        SetCapture(m_hwnd);
        SetVolume((pt.x - m_volume.left) / (m_volume.right - m_volume.left));
        return true;
    }
    return false;
}

bool MediaPanel::OnMouseUp(POINT pt) {
    if (m_dragging != Part::None) {
        if (m_dragging == Part::Timeline) {
            SendMediaCommand(MediaCommand::Seek, m_dragPosition);
            // Shown there until the app says where it is.
            m_state.position = m_dragPosition;
            m_state.updatedAt = NowSeconds();
        }
        m_dragging = Part::None;
        ReleaseCapture();
        StartIslandAnimation();
        return true;
    }
    if (m_press.held) {
        m_press.Release();
        StartIslandAnimation();
    }
    switch (HitTest(pt)) {
        case Part::PillButtons:
            SetIslandPref(IslandPref::MediaButtons, !g_mediaButtonsInPill);
            StartIslandAnimation();
            return true;
        case Part::PillTitle:
            SetIslandPref(IslandPref::MediaTitle, !g_mediaTitleInPill);
            StartIslandAnimation();
            return true;
        case Part::Previous:
            SendMediaCommand(MediaCommand::Previous);
            break;
        case Part::PlayPause:
            SendMediaCommand(MediaCommand::PlayPause);
            break;
        case Part::Next:
            SendMediaCommand(MediaCommand::Next);
            break;
        default:
            break;
    }
    return false;
}

// The wheel anywhere changes the volume, 2% a notch.
bool MediaPanel::OnWheel(int delta) {
    ReadVolume();
    SetVolume(m_level + 0.02f * delta / WHEEL_DELTA);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// The island's own options.

// Restarting the island (another style) is done from a thread of its own,
// since the island's thread can't wait for itself to end.
HANDLE g_islandRestartThread;

void StopIslandThread();
void StartIslandThread();

// Restarting the island, from Windhawk's settings change or from the island
// itself (another style), one at a time.
SRWLOCK g_islandLifeLock = SRWLOCK_INIT;

DWORD WINAPI IslandRestartThreadProc(LPVOID) {
    AcquireSRWLockExclusive(&g_islandLifeLock);
    StopIslandThread();
    StartIslandThread();
    ReleaseSRWLockExclusive(&g_islandLifeLock);
    return 0;
}

void RestartIslandSoon() {
    if (g_islandRestartThread) {
        if (WaitForSingleObject(g_islandRestartThread, 0) != WAIT_OBJECT_0) {
            return;
        }
        CloseHandle(g_islandRestartThread);
    }
    g_islandRestartThread =
        CreateThread(nullptr, 0, IslandRestartThreadProc, nullptr, 0, nullptr);
}

// Before stopping the island elsewhere (unloading, settings changed).
void WaitForIslandRestart() {
    if (g_islandRestartThread) {
        WaitForSingleObject(g_islandRestartThread, INFINITE);
        CloseHandle(g_islandRestartThread);
        g_islandRestartThread = nullptr;
    }
}

// Seconds on the island's clock: as chosen, or like the Windows clock.
bool ClockShowsSeconds() {
    if (g_settings.clockSeconds >= 0) {
        return g_settings.clockSeconds != 0;
    }
    DWORD seconds = 0;
    DWORD size = sizeof(seconds);
    RegGetValue(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                L"Advanced",
                L"ShowSecondsInSystemClock", RRF_RT_REG_DWORD, nullptr,
                &seconds, &size);
    return seconds != 0;
}

int GetIslandPref(IslandPref pref) {
    switch (pref) {
        case IslandPref::Bar:
            return g_settings.islandBar;
        case IslandPref::Theme:
            return g_themeIndex;
        case IslandPref::Tray:
            return g_settings.islandTray;
        case IslandPref::TrayOpen:
            return g_settings.islandTrayOpen;
        case IslandPref::Network:
            return g_settings.islandNetwork;
        case IslandPref::Volume:
            return g_settings.islandVolume;
        case IslandPref::Battery:
            return g_settings.islandBattery;
        case IslandPref::Bell:
            return g_settings.islandBell;
        case IslandPref::Media:
            return g_settings.islandMedia;
        case IslandPref::MediaButtons:
            return g_mediaButtonsInPill;
        case IslandPref::MediaTitle:
            return g_mediaTitleInPill;
        case IslandPref::Banner:
            return g_bannerEnabled;
        case IslandPref::BannerMinimized:
            return g_bannerWhenMinimized;
        case IslandPref::HideWindowsBanners:
            return g_settings.islandHideWindowsBanners;
        case IslandPref::WebPictures:
            return g_settings.islandWebPictures;
        case IslandPref::ClockSeconds:
            return ClockShowsSeconds();
        case IslandPref::ClockWeekday:
            return g_settings.clockWeekday;
        case IslandPref::ClockDate:
            return g_settings.clockDate;
        case IslandPref::ClockYear:
            return g_settings.clockYear;
        case IslandPref::FreeDrag:
            return g_settings.islandFreeDrag;
        case IslandPref::VerticalSides:
            return g_settings.islandVerticalSides;
    }
    return 0;
}

void SetIslandPlace(const std::wstring& appId, double x, double y) {
    IslandPlace* place = appId.empty() ? &g_islandPlace : FindAppPlace(appId);
    if (!place) {
        return;
    }
    place->x = std::clamp(x, 0.0, 1.0);
    place->y = std::clamp(y, 0.0, 1.0);
    SavePlaces();
    if (g_island) {
        g_island->OnPlacesChanged();
    }
}

// Changed in the island's settings (its thread): shown right away, and kept
// when `keep` (once the slider is let go).
void SetIslandSize(int percent, bool keep) {
    percent = std::clamp(percent, kIslandSizeMin, kIslandSizeMax);
    if (keep) {
        Wh_SetIntValue(L"islandSize", percent);
    }
    if (percent == g_settings.islandSize) {
        return;
    }
    g_settings.islandSize = percent;
    if (g_island) {
        g_island->OnPreferencesChanged();
    }
}

// Changed in the island (its thread): kept, and shown right away.
void SetIslandPref(IslandPref pref, int value) {
    for (const auto& info : kIslandPrefs) {
        if (info.pref == pref) {
            Wh_SetIntValue(info.key, value);
        }
    }
    const bool on = value != 0;
    switch (pref) {
        case IslandPref::Bar:
            if (g_settings.islandBar != on) {
                g_settings.islandBar = on;
                // Another kind of window: the island starts again.
                RestartIslandSoon();
            }
            return;
        case IslandPref::Theme:
            LoadTheme();
            break;
        case IslandPref::Tray:
            g_settings.islandTray = on;
            // Explorer asks the apps for their icons if it hasn't yet.
            if (on && g_island) {
                g_island->SayHello();
            }
            break;
        case IslandPref::TrayOpen:
            g_settings.islandTrayOpen = on;
            break;
        case IslandPref::Network:
            g_settings.islandNetwork = on;
            break;
        case IslandPref::Volume:
            g_settings.islandVolume = on;
            break;
        case IslandPref::Battery:
            g_settings.islandBattery = on;
            break;
        case IslandPref::Bell:
            g_settings.islandBell = on;
            break;
        case IslandPref::Media:
            g_settings.islandMedia = on;
            break;
        case IslandPref::MediaButtons:
            g_settings.islandMediaButtons = on;
            g_mediaButtonsInPill = on;
            break;
        case IslandPref::MediaTitle:
            g_mediaTitleInPill = on;
            break;
        case IslandPref::Banner:
            g_settings.islandBanner = on;
            g_bannerEnabled = on;
            break;
        case IslandPref::BannerMinimized:
            g_settings.islandBannerMinimized = on;
            g_bannerWhenMinimized = on;
            break;
        case IslandPref::HideWindowsBanners:
            g_settings.islandHideWindowsBanners = on;
            break;
        case IslandPref::WebPictures:
            g_settings.islandWebPictures = on;
            break;
        case IslandPref::ClockSeconds:
            g_settings.clockSeconds = on;
            break;
        case IslandPref::ClockWeekday:
            g_settings.clockWeekday = on;
            break;
        case IslandPref::ClockDate:
            g_settings.clockDate = on;
            break;
        case IslandPref::ClockYear:
            g_settings.clockYear = on;
            break;
        case IslandPref::FreeDrag:
            g_settings.islandFreeDrag = on;
            break;
        case IslandPref::VerticalSides:
            g_settings.islandVerticalSides = on;
            break;
    }
    if (g_island) {
        g_island->OnPreferencesChanged();
    }
}

////////////////////////////////////////////////////////////////////////////////
// The island.

////////////////////////////////////////////////////////////////////////////////
// The island is told when what it shows changes, instead of asking: the
// volume and the default output device (Core Audio), the network (the network
// list manager, and Wi-Fi's own notifications for its signal), the battery
// (power setting notifications) and "Do not disturb" (its WNF state, see
// IsDoNotDisturbOn). Each posts WM_APP_STATUS to the island's window, from
// whatever thread it comes on.

constexpr GUID kIID_IAudioEndpointVolumeCallback = {
    0x657804fa,
    0xd6ad,
    0x4496,
    {0x8a, 0x60, 0x35, 0x27, 0x52, 0xaf, 0x4f, 0x89}};
constexpr GUID kIID_IMMNotificationClient = {
    0x7991eec9,
    0x7e89,
    0x4d85,
    {0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0}};
constexpr GUID kIID_INetworkListManagerEvents = {
    0xdcb00001,
    0x570f,
    0x4a9b,
    {0x8d, 0x69, 0x19, 0x9f, 0xdb, 0xa5, 0x72, 0x3b}};
constexpr GUID kIID_IConnectionPointContainer = {
    0xb196b284,
    0xbab4,
    0x101a,
    {0xb6, 0x9c, 0x00, 0xaa, 0x00, 0x34, 0x1d, 0x07}};
constexpr GUID kGUID_BatteryPercentageRemaining = {
    0xa7ad8041,
    0xb45a,
    0x4cae,
    {0x87, 0xa3, 0xee, 0xcb, 0xb4, 0x68, 0xa9, 0xe1}};
constexpr GUID kGUID_AcDcPowerSource = {
    0x5d3e9a59,
    0xe9d5,
    0x4b00,
    {0xa6, 0xbd, 0xff, 0x34, 0xff, 0x51, 0x65, 0x48}};
constexpr ULONGLONG kWnfQuietHoursState = 0x0D83063EA3BF1C75;

class StatusEvents final : public IAudioEndpointVolumeCallback,
                           public IMMNotificationClient,
                           public INetworkListManagerEvents {
   public:
    explicit StatusEvents(HWND hwnd) : m_hwnd(hwnd) {}

    // IUnknown (the three interfaces share it).
    IFACEMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (riid == IID_IUnknown || riid == kIID_IAudioEndpointVolumeCallback) {
            *object = static_cast<IAudioEndpointVolumeCallback*>(this);
        } else if (riid == kIID_IMMNotificationClient) {
            *object = static_cast<IMMNotificationClient*>(this);
        } else if (riid == kIID_INetworkListManagerEvents) {
            *object = static_cast<INetworkListManagerEvents*>(this);
        } else {
            *object = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    IFACEMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_refs);
    }
    IFACEMETHODIMP_(ULONG) Release() override {
        const ULONG refs = InterlockedDecrement(&m_refs);
        if (!refs) {
            delete this;
        }
        return refs;
    }

    // The volume or the mute changed.
    IFACEMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        Post(kStatusVolume);
        return S_OK;
    }

    // Another default output device, or one added or removed.
    IFACEMETHODIMP OnDefaultDeviceChanged(EDataFlow flow,
                                          ERole role,
                                          LPCWSTR) override {
        if (flow == eRender && role == eMultimedia) {
            Post(kStatusAudioDevice);
        }
        return S_OK;
    }
    IFACEMETHODIMP OnDeviceAdded(LPCWSTR) override { return S_OK; }
    IFACEMETHODIMP OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    IFACEMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) override {
        return S_OK;
    }
    IFACEMETHODIMP OnPropertyValueChanged(LPCWSTR,
                                          const PROPERTYKEY) override {
        return S_OK;
    }

    // Connected, disconnected, with or without internet.
    IFACEMETHODIMP ConnectivityChanged(NLM_CONNECTIVITY) override {
        Post(kStatusNetwork);
        return S_OK;
    }

    // From the island's thread before it closes its window.
    void Detach() { m_hwnd = nullptr; }

   private:
    void Post(StatusChange change) {
        if (HWND hwnd = m_hwnd) {
            PostMessage(hwnd, WM_APP_STATUS, change, 0);
        }
    }

    LONG m_refs = 1;
    std::atomic<HWND> m_hwnd;
};

// Wi-Fi: connected, disconnected, or its signal changed.
void WINAPI IslandWlanCallback(PWLAN_NOTIFICATION_DATA data, PVOID context) {
    if (data->NotificationSource == WLAN_NOTIFICATION_SOURCE_MSM &&
        (data->NotificationCode == wlan_notification_msm_signal_quality_change ||
         data->NotificationCode == wlan_notification_msm_connected ||
         data->NotificationCode == wlan_notification_msm_disconnected)) {
        PostMessage((HWND)context, WM_APP_STATUS, kStatusNetwork, 0);
    }
}

// "Do not disturb" switched (on any thread).
LONG NTAPI IslandDndCallback(ULONGLONG, ULONG, void*, void*, const void*,
                             ULONG) {
    if (HWND island = g_islandWnd) {
        PostMessage(island, WM_APP_STATUS, kStatusDoNotDisturb, 0);
    }
    return 0;
}

void Island::StartStatusWatch() {
    m_statusEvents = new StatusEvents(m_hwnd);

    // The volume, and the default output device changing.
    if (m_devices) {
        m_devices->RegisterEndpointNotificationCallback(m_statusEvents);
        WatchVolume();
    }

    // The network.
    winrt::com_ptr<IConnectionPointContainer> container;
    if (m_network &&
        SUCCEEDED(m_network->QueryInterface(kIID_IConnectionPointContainer,
                                            container.put_void())) &&
        SUCCEEDED(container->FindConnectionPoint(
            kIID_INetworkListManagerEvents, m_networkPoint.put())) &&
        FAILED(m_networkPoint->Advise(
            static_cast<INetworkListManagerEvents*>(m_statusEvents),
            &m_networkCookie))) {
        m_networkPoint = nullptr;
        m_networkCookie = 0;
    }
    if (m_wlan &&
        WlanRegisterNotification(m_wlan, WLAN_NOTIFICATION_SOURCE_MSM, TRUE,
                                 IslandWlanCallback, m_hwnd, nullptr,
                                 nullptr) == ERROR_SUCCESS) {
        m_wlanWatched = true;
    }

    // The battery, if there's one.
    if (!BatteryGlyph().empty()) {
        m_powerNotifications[0] = RegisterPowerSettingNotification(
            m_hwnd, &kGUID_BatteryPercentageRemaining,
            DEVICE_NOTIFY_WINDOW_HANDLE);
        m_powerNotifications[1] = RegisterPowerSettingNotification(
            m_hwnd, &kGUID_AcDcPowerSource, DEVICE_NOTIFY_WINDOW_HANDLE);
    }

    // "Do not disturb".
    using Subscribe_t = LONG(NTAPI*)(void**, ULONGLONG, ULONG, void*, void*,
                                     const void*, ULONG, ULONG);
    using Query_t = LONG(NTAPI*)(const ULONGLONG*, const void*, const void*,
                                 ULONG*, void*, ULONG*);
    HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
    auto subscribe = (Subscribe_t)GetProcAddress(
        ntdll, "RtlSubscribeWnfStateChangeNotification");
    auto query = (Query_t)GetProcAddress(ntdll, "NtQueryWnfStateData");
    if (subscribe && query) {
        ULONG changeStamp = 0;
        BYTE buffer[16];
        ULONG size = sizeof(buffer);
        query(&kWnfQuietHoursState, nullptr, nullptr, &changeStamp, buffer,
              &size);
        if (subscribe(&m_dndSubscription, kWnfQuietHoursState, changeStamp,
                      (void*)IslandDndCallback, nullptr, nullptr, 0, 0) < 0) {
            m_dndSubscription = nullptr;
        }
    }
}

// The default output device's volume (again when the device changes).
void Island::WatchVolume() {
    if (m_watchedVolume) {
        m_watchedVolume->UnregisterControlChangeNotify(m_statusEvents);
        m_watchedVolume = nullptr;
    }
    if (auto volume = GetVolume(nullptr);
        volume && SUCCEEDED(volume->RegisterControlChangeNotify(m_statusEvents))) {
        m_watchedVolume = volume;
    }
}

void Island::StopStatusWatch() {
    if (m_dndSubscription) {
        using Unsubscribe_t = LONG(NTAPI*)(void*);
        if (auto unsubscribe = (Unsubscribe_t)GetProcAddress(
                GetModuleHandle(L"ntdll.dll"),
                "RtlUnsubscribeWnfStateChangeNotification")) {
            unsubscribe(m_dndSubscription);
        }
        m_dndSubscription = nullptr;
    }
    for (HPOWERNOTIFY& notification : m_powerNotifications) {
        if (notification) {
            UnregisterPowerSettingNotification(notification);
            notification = nullptr;
        }
    }
    if (m_wlanWatched && m_wlan) {
        WlanRegisterNotification(m_wlan, WLAN_NOTIFICATION_SOURCE_NONE, TRUE,
                                 nullptr, nullptr, nullptr, nullptr);
        m_wlanWatched = false;
    }
    if (m_networkPoint) {
        m_networkPoint->Unadvise(m_networkCookie);
        m_networkPoint = nullptr;
        m_networkCookie = 0;
    }
    if (!m_statusEvents) {
        return;
    }
    if (m_watchedVolume) {
        m_watchedVolume->UnregisterControlChangeNotify(m_statusEvents);
        m_watchedVolume = nullptr;
    }
    if (m_devices) {
        m_devices->UnregisterEndpointNotificationCallback(m_statusEvents);
    }
    m_statusEvents->Detach();
    m_statusEvents->Release();
    m_statusEvents = nullptr;
}

// What's playing, as the pill shows it: its title, or its app's name.
std::wstring Island::MediaTitleText() const {
    if (!m_mediaShown.title.empty()) {
        return m_mediaShown.title;
    }
    return m_mediaShown.appId.empty() ? std::wstring()
                                      : AppDisplayName(m_mediaShown.appId);
}

// The cover of what's playing, decoded once per cover.
ID2D1Bitmap* Island::MediaCover() {
    if (m_mediaCoverSerial == m_mediaShown.thumbnailSerial) {
        return m_mediaCover.get();
    }
    m_mediaCoverSerial = m_mediaShown.thumbnailSerial;
    m_mediaCover = nullptr;
    if (m_mediaShown.thumbnail.empty() || !m_wic || !m_canvas.target) {
        return nullptr;
    }
    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream(m_mediaShown.thumbnail.data(),
                                    (UINT)m_mediaShown.thumbnail.size()));
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    winrt::com_ptr<IWICFormatConverter> converter;
    if (stream &&
        SUCCEEDED(m_wic->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnLoad,
            decoder.put())) &&
        SUCCEEDED(decoder->GetFrame(0, frame.put())) &&
        SUCCEEDED(m_wic->CreateFormatConverter(converter.put())) &&
        SUCCEEDED(converter->Initialize(
            frame.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        m_canvas.target->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                   m_mediaCover.put());
    }
    return m_mediaCover.get();
}

// The room the taskbar takes at the bottom (where it shows, even hidden): the
// pill stays above it.
float Island::TaskbarRoom() const {
    RECT rect;
    HWND taskbar = FindTaskbarWnd();
    if (!taskbar || !GetWindowRect(taskbar, &rect)) {
        return 0;
    }
    return (float)(rect.bottom - rect.top);
}

void Island::OnPlacesChanged() {
    // Away from the top, it can't stay a line.
    if (m_minimized && CurrentPlace().y > 0.001) {
        SetMinimized(false);
    }
    StartAnimating();
}

// The app in front, for its own place. The island's own windows (and the
// taskbar) don't count.
void CALLBACK IslandForegroundProc(HWINEVENTHOOK,
                                   DWORD,
                                   HWND,
                                   LONG,
                                   LONG,
                                   DWORD,
                                   DWORD) {
    if (g_island) {
        g_island->OnForegroundChanged();
    }
}

void Island::OnForegroundChanged() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(foreground, &processId);
    if (processId == GetCurrentProcessId()) {
        WCHAR className[64] = L"";
        GetClassName(foreground, className, ARRAYSIZE(className));
        if (_wcsicmp(className, kIslandClassName) == 0 ||
            _wcsicmp(className, kPanelClassName) == 0 ||
            _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
            return;
        }
    }
    const std::wstring appId = WindowAppKey(foreground);
    if (appId == m_placeApp) {
        return;
    }
    const IslandPlace* before = FindAppPlace(m_placeApp);
    m_placeApp = appId;
    if (before || FindAppPlace(appId)) {
        OnPlacesChanged();
    }
}

// Let go after dragging: it snaps to a ready place nearby (the middle or the
// corners of each edge), or stays where it is; kept for the app in front if
// it has its own place.
void Island::FinishDrag() {
    double x = m_placeX;
    double y = m_placeY;
    const double snap = 64.0 * m_scale;
    double best = snap;
    for (double px : {0.0, 0.5, 1.0}) {
        for (double py : {0.0, 0.5, 1.0}) {
            const double distance = std::hypot((m_placeX - px) * m_roomWidth,
                                               (m_placeY - py) * m_roomHeight);
            if (distance < best) {
                best = distance;
                x = px;
                y = py;
            }
        }
    }
    SetIslandPlace(FindAppPlace(m_placeApp) ? m_placeApp : std::wstring(), x,
                   y);
}

// Another option chosen in the island: shown right away.
void Island::OnPreferencesChanged() {
    UpdateClock();
    m_trayKeepOpen = g_settings.islandTrayOpen;
    m_trayOpen = m_trayKeepOpen;
    KillTimer(m_hwnd, kTrayCloseTimerId);
    if (g_settings.islandMedia || g_mediaButtonsInPill || g_mediaTitleInPill) {
        StartMediaWatch();
    }
    StartAnimating();
    Render(true);
}

bool Island::Create() {
    m_bar = g_settings.islandBar;
    m_placeApp = WindowAppKey(GetForegroundWindow());
    m_placeX = m_bar ? 0.5 : CurrentPlace().x;
    m_placeY = m_bar ? 0 : CurrentPlace().y;
    m_minimized = !m_bar && m_placeY < 0.001 &&
                  Wh_GetIntValue(L"islandMinimized", 0) != 0;
    m_expand = m_minimized ? 0 : 1;

    WNDCLASS windowClass{};
    windowClass.hInstance = g_module;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpfnWndProc = IslandWndProc;
    windowClass.lpszClassName = kIslandClassName;
    // Double clicks are passed on to the tray icons.
    windowClass.style = CS_DBLCLKS;
    RegisterClass(&windowClass);
    windowClass.style = 0;
    windowClass.lpfnWndProc = PanelWndProc;
    windowClass.lpszClassName = kPanelClassName;
    RegisterClass(&windowClass);

    m_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kIslandClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_module,
        nullptr);
    if (!m_hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return false;
    }

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 m_d2d.put())) ||
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                   __uuidof(IDWriteFactory),
                                   (IUnknown**)m_dwrite.put())) ||
        !m_canvas.Create(m_d2d.get())) {
        Wh_Log(L"Direct2D/DirectWrite failed");
        return false;
    }
    if (!m_calendar.Create(m_d2d.get(), m_dwrite.get()) ||
        !m_control.Create(m_d2d.get(), m_dwrite.get()) ||
        !m_notifications.Create(m_d2d.get(), m_dwrite.get()) ||
        !m_mediaPanel.Create(m_d2d.get(), m_dwrite.get())) {
        Wh_Log(L"A panel couldn't be created");
    }

    CoCreateInstance(kCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                     kIID_IMMDeviceEnumerator, m_devices.put_void());
    CoCreateInstance(kCLSID_NetworkListManager, nullptr, CLSCTX_ALL,
                     kIID_INetworkListManager, m_network.put_void());
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(m_wic.put()));
    DWORD version;
    if (WlanOpenHandle(2, nullptr, &version, &m_wlan) != ERROR_SUCCESS) {
        m_wlan = nullptr;
    }

    UpdateClock();
    UpdateStatus();
    Layout();
    StartStatusWatch();

    // The bar reserves its strip at the top of the screen, like the taskbar
    // does at the bottom.
    if (m_bar) {
        APPBARDATA data{sizeof(data)};
        data.hWnd = m_hwnd;
        data.uCallbackMessage = WM_APP_APPBAR;
        m_appBar = SHAppBarMessage(ABM_NEW, &data) != FALSE;
        SetAppBarPosition();
    }

    // The tray icons' names, under them.
    m_tooltip = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, TOOLTIPS_CLASS,
                               nullptr, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                               0, 0, 0, 0, m_hwnd, nullptr, g_module, nullptr);
    if (m_tooltip) {
        TOOLINFO tool{sizeof(tool)};
        tool.uFlags = TTF_TRACK | TTF_ABSOLUTE;
        tool.hwnd = m_hwnd;
        tool.uId = 1;
        tool.lpszText = (LPWSTR)L"";
        SendMessage(m_tooltip, TTM_ADDTOOL, 0, (LPARAM)&tool);
        SendMessage(m_tooltip, TTM_SETMAXTIPWIDTH, 0, 400);
    }
    m_trayKeepOpen = Wh_GetIntValue(L"trayKeepOpen",
                                    g_settings.islandTrayOpen ? 1 : 0) != 0;
    g_bannerEnabled =
        Wh_GetIntValue(L"bannerEnabled", g_settings.islandBanner ? 1 : 0) != 0;
    g_bannerWhenMinimized =
        Wh_GetIntValue(L"bannerMinimized",
                       g_settings.islandBannerMinimized ? 1 : 0) != 0;
    m_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        IslandForegroundProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    // The Windows banner: when it's uncloaked and as it moves.
    m_bannerHook = SetWinEventHook(kEventObjectUncloaked,
                                   kEventObjectUncloaked, nullptr,
                                   BannerWinEventProc, 0, 0,
                                   WINEVENT_OUTOFCONTEXT);
    m_trayOpen = m_trayKeepOpen;
    m_trayAmount = m_trayBounce = m_trayArrow = m_trayOpen ? 1 : 0;
    g_showNotificationContent =
        Wh_GetIntValue(L"notificationContent", 1) != 0;
    WCHAR pinned[4096] = L"";
    Wh_GetStringValue(L"trayPinned", pinned, ARRAYSIZE(pinned));
    for (PCWSTR p = pinned; *p;) {
        PCWSTR end = wcschr(p, L'\n');
        m_trayPinned.push_back(end ? std::wstring(p, end) : std::wstring(p));
        if (!end) {
            break;
        }
        p = end + 1;
    }

    Render(true);
    // Explorer sends the badges and the tray icons when it hears from it
    // (and again whenever it restarts, see "TaskbarCreated").
    SayHello();
    // Shown right away, still off the screen and transparent: Windows' own
    // animation for showing a window draws it as a plain rectangle, so it
    // runs where it can't be seen (see m_away).
    ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
    m_shownOnce = true;
    g_islandWnd = m_hwnd;
    UpdateApps();
    UpdateTrayIcons();
    g_mediaButtonsInPill =
        Wh_GetIntValue(L"mediaButtons",
                       g_settings.islandMediaButtons ? 1 : 0) != 0;
    m_mediaButtonsAmount = m_mediaButtonsRoom = g_mediaButtonsInPill ? 1 : 0;
    g_mediaTitleInPill = Wh_GetIntValue(L"mediaTitle", 0) != 0;
    m_mediaTitleRoom = g_mediaTitleInPill ? 1 : 0;
    if (g_settings.islandMedia || g_mediaButtonsInPill || g_mediaTitleInPill) {
        StartMediaWatch();
    }
    ScheduleTick();
    return true;
}

// The next tick, right after the clock's next second.
void Island::ScheduleTick() {
    SYSTEMTIME now;
    GetLocalTime(&now);
    SetTimer(m_hwnd, kIslandTimerId, 1000 - now.wMilliseconds + 15, nullptr);
}

void Island::Destroy() {
    g_islandWnd = nullptr;
    WaitForToastActivations();
    if (m_hwnd) {
        KillTimer(m_hwnd, kBannerTimerId);
        KillTimer(m_hwnd, kMediaTimerId);
    }
    StopStatusWatch();
    StopMediaWatch();
    m_calendar.Destroy();
    m_control.Destroy();
    m_notifications.Destroy();
    m_mediaPanel.Destroy();
    if (m_tooltip) {
        DestroyWindow(m_tooltip);
        m_tooltip = nullptr;
    }
    for (HWINEVENTHOOK* hook :
         {&m_bannerHook, &m_bannerMoveHook, &m_foregroundHook}) {
        if (*hook) {
            UnhookWinEvent(*hook);
            *hook = nullptr;
        }
    }
    g_windowsBanner = nullptr;
    StopWatchingTrayAppWindows();
    m_trayIcons.clear();
    m_trayBitmaps.clear();
    if (m_appBar) {
        APPBARDATA data{sizeof(data)};
        data.hWnd = m_hwnd;
        SHAppBarMessage(ABM_REMOVE, &data);
        m_appBar = false;
    }
    if (m_hwnd) {
        KillTimer(m_hwnd, kIslandTimerId);
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    m_canvas.Destroy();
    if (m_wlan) {
        WlanCloseHandle(m_wlan, nullptr);
        m_wlan = nullptr;
    }
    UnregisterClass(kIslandClassName, g_module);
    UnregisterClass(kPanelClassName, g_module);
}

void Island::SetAppBarPosition() {
    if (!m_appBar) {
        return;
    }
    APPBARDATA data{sizeof(data)};
    data.hWnd = m_hwnd;
    data.uEdge = ABE_TOP;
    data.rc = {m_monitor.left, m_monitor.top, m_monitor.right,
               m_monitor.top + m_size.cy};
    SHAppBarMessage(ABM_QUERYPOS, &data);
    data.rc.bottom = data.rc.top + m_size.cy;
    SHAppBarMessage(ABM_SETPOS, &data);
}

// Like on macOS: "sex 3 out 23:59:12", with seconds when the taskbar clock
// shows them.
// "sáb 3 out 2026  20:01:50": the parts chosen in the island's settings.
void Island::UpdateClock() {
    SYSTEMTIME now;
    GetLocalTime(&now);

    std::wstring format;
    auto add = [&](PCWSTR part) {
        if (!format.empty()) {
            format += L" ";
        }
        format += part;
    };
    if (g_settings.clockWeekday) {
        add(L"ddd");
    }
    if (g_settings.clockDate) {
        add(L"d MMM");
    }
    if (g_settings.clockYear) {
        add(L"yyyy");
    }
    WCHAR date[64] = L"";
    if (!format.empty()) {
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, format.c_str(),
                        date, ARRAYSIZE(date), nullptr);
    }
    WCHAR time[64] = L"";
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT,
                    ClockShowsSeconds() ? 0 : TIME_NOSECONDS, &now, nullptr,
                    time, ARRAYSIZE(time));
    m_clock = date[0] ? std::wstring(date) + L"  " + time : std::wstring(time);
    // Upright: the time without seconds, and the day and month under it.
    WCHAR shortTime[32] = L"";
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &now, nullptr,
                    shortTime, ARRAYSIZE(shortTime));
    WCHAR shortDate[32] = L"";
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, L"d MMM", shortDate,
                    ARRAYSIZE(shortDate), nullptr);
    m_clockTime = shortTime;
    m_clockDate = shortDate;
}

// The default device can change (headphones), so it's looked up each time.
winrt::com_ptr<IAudioEndpointVolume> Island::GetVolume(
    std::wstring* deviceName) {
    winrt::com_ptr<IMMDevice> device;
    winrt::com_ptr<IAudioEndpointVolume> volume;
    if (!m_devices ||
        FAILED(m_devices->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                  device.put())) ||
        FAILED(device->Activate(kIID_IAudioEndpointVolume, CLSCTX_ALL, nullptr,
                                volume.put_void()))) {
        return nullptr;
    }
    if (deviceName) {
        winrt::com_ptr<IPropertyStore> properties;
        if (SUCCEEDED(device->OpenPropertyStore(STGM_READ,
                                                properties.put()))) {
            PROPVARIANT value;
            PropVariantInit(&value);
            if (SUCCEEDED(properties->GetValue(kPKEY_Device_FriendlyName,
                                               &value)) &&
                value.vt == VT_LPWSTR && value.pwszVal) {
                *deviceName = value.pwszVal;
            }
            PropVariantClear(&value);
        }
    }
    return volume;
}

NetworkInfo Island::QueryNetwork() {
    NetworkInfo info;
    NLM_CONNECTIVITY connectivity;
    if (m_network && SUCCEEDED(m_network->GetConnectivity(&connectivity))) {
        info.internet = connectivity & (NLM_CONNECTIVITY_IPV4_INTERNET |
                                        NLM_CONNECTIVITY_IPV6_INTERNET);
        info.connected = connectivity != NLM_CONNECTIVITY_DISCONNECTED;
    }

    // A connected Wi-Fi network shows its name and signal.
    PWLAN_INTERFACE_INFO_LIST interfaces = nullptr;
    if (m_wlan && WlanEnumInterfaces(m_wlan, nullptr, &interfaces) ==
                      ERROR_SUCCESS) {
        for (DWORD i = 0; i < interfaces->dwNumberOfItems && info.signal < 0;
             i++) {
            const auto& adapter = interfaces->InterfaceInfo[i];
            if (adapter.isState != wlan_interface_state_connected) {
                continue;
            }
            DWORD size = 0;
            PWLAN_CONNECTION_ATTRIBUTES attributes = nullptr;
            if (WlanQueryInterface(m_wlan, &adapter.InterfaceGuid,
                                   wlan_intf_opcode_current_connection, nullptr,
                                   &size, (PVOID*)&attributes,
                                   nullptr) == ERROR_SUCCESS) {
                info.signal =
                    attributes->wlanAssociationAttributes.wlanSignalQuality;
                const auto& ssid = attributes->wlanAssociationAttributes.dot11Ssid;
                WCHAR name[64] = L"";
                MultiByteToWideChar(CP_UTF8, 0, (LPCCH)ssid.ucSSID,
                                    (int)std::min<ULONG>(ssid.uSSIDLength, 32),
                                    name, ARRAYSIZE(name) - 1);
                info.wifiName = name;
                WlanFreeMemory(attributes);
            }
        }
        WlanFreeMemory(interfaces);
    }
    return info;
}

std::wstring Island::NetworkGlyph() {
    const NetworkInfo info = QueryNetwork();
    if (!info.internet && !info.connected) {
        return {kGlyphNoInternet};
    }
    if (info.signal >= 0) {
        return {info.internet ? kGlyphWifi[std::clamp(info.signal / 25, 0, 3)]
                              : kGlyphNoInternet};
    }
    return {info.internet ? kGlyphEthernet : kGlyphNoInternet};
}

std::wstring Island::VolumeGlyph() {
    auto volume = GetVolume(nullptr);
    float level = 0;
    BOOL muted = FALSE;
    if (!volume || FAILED(volume->GetMasterVolumeLevelScalar(&level)) ||
        FAILED(volume->GetMute(&muted))) {
        return {};
    }
    if (muted) {
        return {kGlyphMute};
    }
    if (level <= 0.001f) {
        return {kGlyphVolume[0]};
    }
    return {kGlyphVolume[std::clamp((int)(level * 3) + 1, 1, 3)]};
}

void Island::OnDoNotDisturbChanged() {
    UpdateStatus();
    Render();
}

void Island::OpenNotifications() {
    if (!m_notifications.IsOpen()) {
        TogglePanel(m_notifications, IslandItem::Bell);
    }
}

void Island::OnNetworkChanged() {
    m_networkGlyph = NetworkGlyph();
    Render();
}

void Island::OnVolumeChanged() {
    m_volumeGlyph = VolumeGlyph();
    Render();
}

// Nothing on a computer without a battery.
std::wstring Island::BatteryGlyph() {
    SYSTEM_POWER_STATUS power;
    if (!GetSystemPowerStatus(&power) || (power.BatteryFlag & 128) ||
        power.BatteryFlag == 255 || power.BatteryLifePercent > 100) {
        return {};
    }
    const int tenth = (power.BatteryLifePercent + 5) / 10;
    const bool charging = power.ACLineStatus == 1;
    if (tenth >= 10) {
        return {charging ? (WCHAR)0xEA93 : (WCHAR)0xE83F};
    }
    return {(WCHAR)((charging ? 0xE85A : 0xE850) + tenth)};
}

void Island::UpdateStatus() {
    m_networkGlyph = NetworkGlyph();
    m_volumeGlyph = VolumeGlyph();
    m_batteryGlyph = BatteryGlyph();
    // A moon while "Do not disturb" is on, like the macOS Focus icon.
    m_bellGlyph = {IsDoNotDisturbOn() ? kGlyphMoon : kGlyphBell};
}

void Island::UpdateFormats(float scale) {
    if (scale == m_scale && m_iconFormat && m_smallIconFormat &&
        m_textFormat) {
        return;
    }
    m_scale = scale;
    m_iconFormat = nullptr;
    m_smallIconFormat = nullptr;
    m_textFormat = nullptr;
    m_dwrite->CreateTextFormat(L"Segoe Fluent Icons", nullptr,
                               DWRITE_FONT_WEIGHT_NORMAL,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL,
                               kIslandIconSize * scale, L"",
                               m_iconFormat.put());
    m_dwrite->CreateTextFormat(L"Segoe Fluent Icons", nullptr,
                               DWRITE_FONT_WEIGHT_NORMAL,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL,
                               kIslandSmallIconSize * scale, L"",
                               m_smallIconFormat.put());
    m_dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr,
                               DWRITE_FONT_WEIGHT_SEMI_BOLD,
                               DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL,
                               kIslandTextSize * scale, L"",
                               m_textFormat.put());
    for (auto* format : {m_iconFormat.get(), m_smallIconFormat.get(),
                         m_textFormat.get()}) {
        if (format) {
            format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
    }
    if (!m_tabular &&
        SUCCEEDED(m_dwrite->CreateTypography(m_tabular.put()))) {
        m_tabular->AddFontFeature({DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES, 1});
    }

    // The banner's texts: one line each, ending with "..." when too long.
    auto make = [&](DWRITE_FONT_WEIGHT weight, float size,
                    DWRITE_TEXT_ALIGNMENT alignment) {
        winrt::com_ptr<IDWriteTextFormat> format;
        if (SUCCEEDED(m_dwrite->CreateTextFormat(
                L"Segoe UI Variable Text", nullptr, weight,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                size * scale, L"", format.put()))) {
            format->SetTextAlignment(alignment);
            format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            const DWRITE_TRIMMING trimming{
                DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
            winrt::com_ptr<IDWriteInlineObject> ellipsis;
            m_dwrite->CreateEllipsisTrimmingSign(format.get(), ellipsis.put());
            format->SetTrimming(&trimming, ellipsis.get());
        }
        return format;
    };
    m_bannerAppFormat = make(DWRITE_FONT_WEIGHT_NORMAL, 11.5f,
                             DWRITE_TEXT_ALIGNMENT_LEADING);
    m_bannerTimeFormat = make(DWRITE_FONT_WEIGHT_NORMAL, 11.5f,
                              DWRITE_TEXT_ALIGNMENT_TRAILING);
    m_bannerTitleFormat = make(DWRITE_FONT_WEIGHT_SEMI_BOLD, 13,
                               DWRITE_TEXT_ALIGNMENT_LEADING);
    m_bannerTextFormat = make(DWRITE_FONT_WEIGHT_NORMAL, 12.5f,
                              DWRITE_TEXT_ALIGNMENT_LEADING);
}

// Places the items and sizes the island around them: centered in the pill,
// at the right end of the bar (like the macOS menu bar's status items).
void Island::Layout() {
    HWND taskbar = FindTaskbarWnd();
    HMONITOR monitor =
        taskbar ? MonitorFromWindow(taskbar, MONITOR_DEFAULTTOPRIMARY)
                : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO info{sizeof(info)};
    GetMonitorInfo(monitor, &info);
    m_monitor = info.rcMonitor;
    // The screen's scaling, and the island's own size on top of it (the
    // panels keep the screen's).
    m_dpiScale = (taskbar ? GetDpiForWindow(taskbar) : 96) / 96.0f;
    const float scale = m_dpiScale * g_settings.islandSize / 100.0f;
    UpdateFormats(scale);

    // Upright on the left or right edge, if chosen (not for the bar).
    {
        const IslandPlace& place = CurrentPlace();
        m_vertical = !m_bar && g_settings.islandVerticalSides &&
                     (place.x < 0.001 || place.x > 0.999) && place.y > 0.001 &&
                     place.y < 0.999;
        m_sideLeft = place.x < 0.5;
    }
    // Across the pill (its height, or its width when upright).
    const float height = std::round(
        (m_bar ? kBarHeight : m_vertical ? kVerticalPillWidth : kPillHeight) *
        scale);
    const float padding = std::round(kIslandPadding * scale);
    const float spacing = std::round(kIslandSpacing * scale);
    const float iconSlot = std::round(kIslandIconSlot * scale);
    const float smallSlot = std::round(kIslandSmallSlot * scale);

    // The clock's width, measured with every figure as a 0, so that it stays
    // the same as the time changes.
    float clockWidth = 100 * scale;
    std::wstring widest = m_clock;
    for (auto& c : widest) {
        if (c >= L'0' && c <= L'9') {
            c = L'0';
        }
    }
    winrt::com_ptr<IDWriteTextLayout> clockLayout;
    if (m_textFormat &&
        SUCCEEDED(m_dwrite->CreateTextLayout(
            widest.c_str(), (UINT32)widest.size(), m_textFormat.get(),
            1000 * scale, height, clockLayout.put()))) {
        if (m_tabular) {
            clockLayout->SetTypography(m_tabular.get(),
                                       {0, (UINT32)widest.size()});
        }
        DWRITE_TEXT_METRICS metrics;
        clockLayout->GetMetrics(&metrics);
        clockWidth = std::ceil(metrics.widthIncludingTrailingWhitespace) +
                     12 * scale;
    }
    // Upright: two short lines.
    if (m_vertical) {
        clockWidth = std::round(40 * scale);
    }

    m_slots.clear();
    float x = 0;
    auto addIcon = [&](IslandItem item, const std::wstring& glyph,
                       bool small = false) {
        if (glyph.empty()) {
            return;
        }
        const float slot = small ? smallSlot : iconSlot;
        m_slots.push_back(
            {item, {x, 0, x + slot, height}, glyph, small ? 2 : 1});
        x += slot + spacing;
    };
    // Apps with notifications first, like the icons on the left of a phone's
    // status bar. They take room as they grow in, so the island widens with
    // them.
    float appsShown = 0;
    for (const auto& entry : m_appEntries) {
        const float shown = (float)std::clamp(entry.shown, 0.0, 1.15);
        const float room = std::min(shown, 1.0f);
        IslandSlot slot{IslandItem::App, {x, 0, x + iconSlot * room, height},
                        entry.id, 3};
        slot.scale = shown;
        m_slots.push_back(slot);
        x += (iconSlot + spacing) * room;
        appsShown = std::max(appsShown, room);
    }
    x += spacing * appsShown;
    // The tray: the arrow (when some icons aren't pinned), the others
    // opening sideways after it, then the pinned ones.
    if (g_settings.islandTray) {
        bool unpinned = false;
        for (const auto& icon : m_trayIcons) {
            unpinned = unpinned || !IsTrayPinned(icon.key);
        }
        if (unpinned) {
            addIcon(IslandItem::Overflow, {kGlyphNext});
        }
        // The room grows without going past it (no jolt at the end); the
        // icons bounce in on their own.
        const float room = (float)std::clamp(m_trayAmount, 0.0, 1.0);
        const float open = (float)std::clamp(m_trayBounce, 0.0, 1.15);
        // An icon being pinned shrinks out of the list as it grows among the
        // pinned ones (and back when unpinned).
        for (const auto& icon : m_trayIcons) {
            const float unpinned = 1 - PinAmount(icon.key);
            const float iconRoom = room * unpinned;
            if (iconRoom <= 0.001f) {
                continue;
            }
            IslandSlot slot{IslandItem::TrayIcon,
                            {x, 0, x + iconSlot * iconRoom, height},
                            icon.key,
                            4};
            slot.scale = open * unpinned;
            m_slots.push_back(slot);
            x += (iconSlot + spacing) * iconRoom;
        }
        // With the list open, a faint line between it and the pinned ones.
        bool anyPinned = false;
        for (const auto& icon : m_trayIcons) {
            anyPinned = anyPinned || PinAmount(icon.key) > 0.001f;
        }
        if (unpinned && anyPinned && room > 0.001f) {
            const float line = std::round(6 * scale);
            IslandSlot slot{IslandItem::TraySeparator,
                            {x, 0, x + line * room, height},
                            L"",
                            5};
            slot.scale = room;
            m_slots.push_back(slot);
            x += (line + spacing) * room;
        }
        for (const auto& icon : m_trayIcons) {
            const float pinned = PinAmount(icon.key);
            if (pinned <= 0.001f) {
                continue;
            }
            IslandSlot slot{IslandItem::TrayIcon,
                            {x, 0, x + iconSlot * pinned, height},
                            icon.key,
                            4};
            slot.scale = pinned;
            m_slots.push_back(slot);
            x += (iconSlot + spacing) * pinned;
        }
    }
    if (g_settings.islandNetwork) {
        addIcon(IslandItem::Network, m_networkGlyph);
    }
    if (g_settings.islandVolume) {
        addIcon(IslandItem::Volume, m_volumeGlyph);
    }
    if (g_settings.islandBattery) {
        addIcon(IslandItem::Battery, m_batteryGlyph);
    }
    x += spacing;
    m_slots.push_back(
        {IslandItem::Clock, {x, 0, x + clockWidth, height}, m_clock, 0});
    x += clockWidth + spacing;
    if (g_settings.islandBell) {
        addIcon(IslandItem::Bell, m_bellGlyph);
    }
    // What's playing, next to the bell, and its buttons: they take room as
    // they grow in, so the pill widens with them.
    auto addGrowing = [&](IslandItem item, const std::wstring& glyph,
                          bool small, float room, float amount) {
        if (room <= 0.001f && amount <= 0.001f) {
            return;
        }
        room = std::clamp(room, 0.0f, 1.0f);
        const float slot = small ? smallSlot : iconSlot;
        IslandSlot entry{item, {x, 0, x + slot * room, height}, glyph,
                         small ? 2 : 1};
        entry.scale = std::max(amount, 0.0f);
        m_slots.push_back(entry);
        x += (slot + spacing) * room;
    };
    const float mediaRoom = (float)std::clamp(m_mediaRoom, 0.0, 1.0);
    const float media = (float)std::clamp(m_mediaAmount, 0.0, 1.15);
    const float titleRoom = (float)std::clamp(m_mediaTitleRoom, 0.0, 1.0);
    if (!m_vertical && titleRoom > 0.001f &&
        (mediaRoom > 0.001f || media > 0.001f)) {
        // The cover and the title, as wide as the title (up to a limit),
        // opening out of the note.
        const std::wstring measuredKey = MediaTitleText() + L"\n" +
                                         m_mediaShown.artist + L"\n" +
                                         std::to_wstring(scale);
        if (measuredKey != m_mediaTitleMeasured) {
            m_mediaTitleMeasured = measuredKey;
            auto measure = [&](const std::wstring& text) {
                winrt::com_ptr<IDWriteTextLayout> layout;
                if (!m_bannerTextFormat ||
                    FAILED(m_dwrite->CreateTextLayout(
                        text.c_str(), (UINT32)text.size(),
                        m_bannerTextFormat.get(), 10000, height,
                        layout.put()))) {
                    return 0.0f;
                }
                DWRITE_TEXT_METRICS metrics;
                layout->GetMetrics(&metrics);
                return metrics.widthIncludingTrailingWhitespace;
            };
            m_mediaTitleShown = MediaTitleText();
            m_mediaTitleWidth = measure(m_mediaTitleShown);
            if (m_mediaTitleWidth < kMediaTitleShortWidth * scale &&
                !m_mediaShown.title.empty() && !m_mediaShown.artist.empty()) {
                m_mediaTitleShown += L" \u00B7 " + m_mediaShown.artist;
                m_mediaTitleWidth = measure(m_mediaTitleShown);
            }
        }
        const std::wstring& title = m_mediaTitleShown;
        const float full =
            std::round(18 * scale) + std::round(8 * scale) +
            std::clamp(m_mediaTitleWidth, std::round(kMediaTitleMinWidth * scale),
                       std::round(kMediaTitleMaxWidth * scale)) +
            std::round(4 * scale);
        const float width =
            (iconSlot + (std::max(full, iconSlot) - iconSlot) * titleRoom) *
            mediaRoom;
        IslandSlot entry{IslandItem::Media, {x, 0, x + width, height}, title, 6};
        entry.scale = std::max(media, 0.0f);
        m_slots.push_back(entry);
        x += width + spacing * mediaRoom;
    } else if (g_settings.islandMedia) {
        addGrowing(IslandItem::Media, {kGlyphMusic}, false, mediaRoom, media);
    }
    const float buttonsRoom =
        mediaRoom * (float)std::clamp(m_mediaButtonsRoom, 0.0, 1.0);
    const float buttons =
        std::min(media, 1.0f) *
        (float)std::clamp(m_mediaButtonsAmount, 0.0, 1.15);
    addGrowing(IslandItem::MediaPrevious, {kGlyphPreviousTrack}, true,
               buttonsRoom, buttons);
    addGrowing(IslandItem::MediaPlayPause,
               {m_mediaShown.playing ? kGlyphPause : kGlyphPlay}, true,
               buttonsRoom, buttons);
    addGrowing(IslandItem::MediaNext, {kGlyphNextTrack}, true, buttonsRoom,
               buttons);
    // Minimized to a line at the very top only.
    if (!m_bar && AtTop()) {
        addIcon(IslandItem::Minimize, {kGlyphCollapse}, true);
    }
    const float contentWidth = x - spacing;

    if (m_bar) {
        const float width = (float)(m_monitor.right - m_monitor.left);
        const float shift = width - padding - contentWidth;
        for (auto& slot : m_slots) {
            slot.rect.left += shift;
            slot.rect.right += shift;
        }
        m_size = {(LONG)width, (LONG)height};
        m_position = {m_monitor.left, m_monitor.top};
        m_pill = {0, 0, width, height};
        return;
    }

    if (m_vertical) {
        // Upright: the items laid out along its length (so far along x) go
        // from top to bottom; it sits against its edge, and the window is
        // big enough for a banner opening towards the middle.
        const float length = contentWidth + padding * 2;
        float windowWidth = height;
        float windowHeight = std::ceil(length / 2) * 2 + 2;
        if (m_bannerWanted || m_bannerAmount > 0) {
            const float bannerHeight =
                kBannerHeight +
                (m_banner.reply.CanReply() ? kBannerReplyHeight : 0);
            windowWidth = std::max(windowWidth,
                                   std::ceil(kBannerWidth * scale * 1.08f + 2));
            windowHeight = std::max(
                windowHeight,
                std::ceil(bannerHeight * scale * 1.08f / 2) * 2 + 2);
        }
        const float pillLeft = m_sideLeft ? 0 : windowWidth - height;
        const float shift = (windowHeight - length) / 2;
        for (auto& slot : m_slots) {
            slot.rect = {pillLeft, slot.rect.left + padding + shift,
                         pillLeft + height, slot.rect.right + padding + shift};
        }
        m_size = {(LONG)windowWidth, (LONG)windowHeight};
        m_pill = {pillLeft, shift, pillLeft + height, shift + length};
        const float margin = std::round(kPillTopGap * scale);
        m_roomLeft = (float)m_monitor.left + margin;
        m_roomTop = (float)m_monitor.top + margin;
        m_roomWidth = std::max(
            0.0f, (float)(m_monitor.right - m_monitor.left) - margin * 2 -
                      height);
        m_roomHeight = std::max(0.0f, (float)(m_monitor.bottom - m_monitor.top) -
                                          margin * 2 - length - TaskbarRoom());
        const float pillX = m_roomLeft + (float)m_placeX * m_roomWidth;
        const float pillY = m_roomTop + (float)m_placeY * m_roomHeight;
        m_position = {(LONG)std::lround(pillX - pillLeft),
                      (LONG)std::lround(pillY - shift)};
        return;
    }

    for (auto& slot : m_slots) {
        slot.rect.left += padding;
        slot.rect.right += padding;
    }
    const float pillWidth = contentWidth + padding * 2;
    // The window is as big as the expanded pill (plus the gap above it, where
    // the minimized line is), or the banner with its bounce; its transparent
    // parts let clicks through.
    const float top = std::round(kPillTopGap * scale);
    float windowWidth = std::ceil(pillWidth / 2) * 2 + 2;
    float windowHeight = top + height;
    if (m_bannerWanted || m_bannerAmount > 0) {
        windowWidth = std::max(
            windowWidth, std::ceil(kBannerWidth * scale * 1.08f / 2) * 2 + 2);
        const float bannerHeight =
            kBannerHeight +
            (m_banner.reply.CanReply() ? kBannerReplyHeight : 0);
        windowHeight = std::max(
            windowHeight, std::ceil(top + bannerHeight * scale * 1.08f + 2));
    }
    const float shift = (windowWidth - pillWidth) / 2;
    // On the lower half, the pill is at the window's bottom (banners open
    // upwards).
    const float pillTop = AtBottom() ? windowHeight - height : top;
    for (auto& slot : m_slots) {
        slot.rect.left += shift;
        slot.rect.right += shift;
        slot.rect.top += pillTop;
        slot.rect.bottom += pillTop;
    }
    m_size = {(LONG)windowWidth, (LONG)windowHeight};
    m_pill = {shift, pillTop, shift + pillWidth, pillTop + height};

    // Its place: from the top (where it's always been) to the bottom, left to
    // right, inside the monitor.
    const float margin = top;
    m_roomLeft = (float)m_monitor.left + margin;
    m_roomTop = (float)m_monitor.top + top;
    m_roomWidth =
        std::max(0.0f, (float)(m_monitor.right - m_monitor.left) - margin * 2 -
                           pillWidth);
    m_roomHeight = std::max(0.0f, (float)(m_monitor.bottom - m_monitor.top) -
                                      top - margin - height - TaskbarRoom());
    const float pillX = m_roomLeft + (float)m_placeX * m_roomWidth;
    const float pillY = m_roomTop + (float)m_placeY * m_roomHeight;
    m_position = {(LONG)std::lround(pillX - shift),
                  (LONG)std::lround(pillY - pillTop)};
}

// Tells the taskbar where each tray icon is (see OnTrayIconRectRequest): its
// slot, or the arrow while it's in the closed list.
void Island::PublishTrayPlaces() {
    D2D1_RECT_F arrow = m_pill;
    for (const auto& slot : m_slots) {
        if (slot.item == IslandItem::Overflow) {
            arrow = slot.rect;
        }
    }
    auto toScreen = [this](const D2D1_RECT_F& r) {
        return RECT{m_position.x + (LONG)r.left, m_position.y + (LONG)r.top,
                    m_position.x + (LONG)r.right,
                    m_position.y + (LONG)r.bottom};
    };
    std::vector<TrayIconPlace> places;
    for (const auto& icon : m_trayIcons) {
        RECT rect = toScreen(arrow);
        for (const auto& slot : m_slots) {
            if (slot.item == IslandItem::TrayIcon && slot.text == icon.key &&
                slot.rect.right - slot.rect.left > 4) {
                rect = toScreen(slot.rect);
            }
        }
        places.push_back(
            {icon.owner, icon.id, icon.guid, icon.hasGuid, rect});
    }
    // To explorer (only when they changed: this runs with every frame).
    IpcWriter writer;
    writer.Put<UINT32>((UINT32)places.size());
    for (const auto& place : places) {
        writer.Put<UINT64>((UINT64)(ULONG_PTR)place.owner);
        writer.Put<UINT32>(place.id);
        writer.Put<GUID>(place.guid);
        writer.Put<BYTE>(place.hasGuid);
        writer.Put<RECT>(place.rect);
    }
    if (writer.bytes != m_sentTrayPlaces) {
        m_sentTrayPlaces = writer.bytes;
        SendIpc(FindTaskbarWnd(), m_hwnd, kIpcTrayPlaces,
                writer.bytes.data(), (DWORD)writer.bytes.size());
    }
}

// The banner, open, in the window.
D2D1_RECT_F Island::BannerRect() const {
    const float width =
        std::max(std::round(kBannerWidth * m_scale), m_pill.right - m_pill.left);
    const float height = std::round(
        (kBannerHeight +
         kBannerReplyHeight * (float)std::clamp(m_bannerOpen, 0.0, 1.05)) *
        m_scale);
    // Upright: towards the middle of the screen, from the pill's edge.
    if (m_vertical) {
        const float bannerWidth = std::round(kBannerWidth * m_scale);
        const float left = m_sideLeft ? m_pill.left : m_pill.right - bannerWidth;
        const float top = std::clamp(
            std::round((m_pill.top + m_pill.bottom - height) / 2), 0.0f,
            std::max(0.0f, (float)m_size.cy - height));
        return {left, top, left + bannerWidth, top + height};
    }
    const float centerX = (m_pill.left + m_pill.right) / 2;
    // Upwards on the lower half of the screen.
    const float top = AtBottom() ? m_pill.bottom - height : m_pill.top;
    return {std::round(centerX - width / 2), top,
            std::round(centerX - width / 2) + width, top + height};
}

void Island::BannerParts(D2D1_RECT_F* field,
                         D2D1_RECT_F* send,
                         D2D1_RECT_F* open) const {
    const D2D1_RECT_F banner = BannerRect();
    const float inner = std::round(14 * m_scale);
    const float top = banner.top + std::round(kBannerHeight * m_scale);
    const float height = std::round(32 * m_scale);
    const float openWidth = std::round(76 * m_scale);
    *open = {banner.right - inner - openWidth, top, banner.right - inner,
             top + height};
    *field = {banner.left + inner, top,
              open->left - std::round(8 * m_scale), top + height};
    *send = {field->right - height, field->top, field->right, field->bottom};
}

D2D1_RECT_F Island::BannerCloseRect() const {
    const D2D1_RECT_F banner = BannerRect();
    const float size = std::round(20 * m_scale);
    const float inner = std::round(12 * m_scale);
    return {banner.right - inner - size, banner.top + inner,
            banner.right - inner, banner.top + inner + size};
}

// Typing a reply: the island takes the keyboard, and the banner stays until
// clicking somewhere else.
void Island::StartBannerTyping() {
    if (m_bannerTyping || !m_banner.reply.CanReply()) {
        return;
    }
    m_bannerTyping = true;
    m_bannerReplyText.clear();
    KillTimer(m_hwnd, kBannerTimerId);
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE,
                     GetWindowLongPtr(m_hwnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
    StartAnimating();
    Render(true);
}

void Island::EndBannerTyping() {
    if (!m_bannerTyping) {
        return;
    }
    m_bannerTyping = false;
    m_bannerReplyText.clear();
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE,
                     GetWindowLongPtr(m_hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
    StartAnimating();
}

void Island::SendBannerReply() {
    if (m_bannerReplyText.empty()) {
        return;
    }
    const bool sent =
        ActivateToast(m_banner.appId, m_banner.reply.arguments,
                      m_banner.reply.activationType, m_banner.reply.inputId,
                      m_bannerReplyText);
    Wh_Log(L"Reply from the pill: %s", sent ? L"sent" : L"no activator");
    EndBannerTyping();
    HideBanner();
}

// Typing in the reply box: Enter sends, Escape cancels, Ctrl+V pastes.
bool Island::OnBannerChar(WCHAR c) {
    if (!m_bannerTyping) {
        return false;
    }
    constexpr size_t kMaxReply = 2000;
    if (c == L'\r') {
        SendBannerReply();
    } else if (c == 0x1B) {
        EndBannerTyping();
        HideBanner();
    } else if (c == L'\b') {
        if (!m_bannerReplyText.empty()) {
            m_bannerReplyText.pop_back();
        }
    } else if (c == 0x16) {
        if (OpenClipboard(m_hwnd)) {
            if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
                if (auto text = (PCWSTR)GlobalLock(data)) {
                    for (PCWSTR p = text; *p; p++) {
                        if (m_bannerReplyText.size() < kMaxReply) {
                            m_bannerReplyText +=
                                (*p == L'\r' || *p == L'\n') ? L' ' : *p;
                        }
                    }
                    GlobalUnlock(data);
                }
            }
            CloseClipboard();
        }
    } else if (c >= 0x20 && m_bannerReplyText.size() < kMaxReply) {
        m_bannerReplyText += c;
    }
    Render(true);
    return true;
}

// Takes the latest new notification and shows it: in the pill, or peeking out
// of the line when minimized. Not with "Do not disturb", in the bar, or over
// an open panel.
void Island::ShowBanner() {
    AcquireSRWLockExclusive(&g_islandToastsLock);
    std::vector<IslandToast> toasts = std::move(g_islandToasts);
    g_islandToasts.clear();
    ReleaseSRWLockExclusive(&g_islandToastsLock);
    if (toasts.empty() || m_bannerTyping) {
        return;
    }
    if (m_bar || !m_visible || m_calendar.IsOpen() || m_control.IsOpen() ||
        m_notifications.IsOpen() || m_mediaPanel.IsOpen()) {
        return;
    }
    if (m_minimized && !g_bannerWhenMinimized &&
        !toasts.back().appId.empty()) {
        m_peekApp = toasts.back().appId;
        m_peekStart = NowSeconds();
        StartAnimating();
        return;
    }
    m_banner = std::move(toasts.back());
    m_bannerWanted = true;
    // The Windows banner can go now (see OnWindowsBanner).
    m_bannerTakenAt = NowSeconds();
    if (g_windowsBanner) {
        OnWindowsBanner(g_windowsBanner);
    }
    if (!m_banner.sticky) {
        SetTimer(m_hwnd, kBannerTimerId, kBannerShownMs, nullptr);
    } else {
        KillTimer(m_hwnd, kBannerTimerId);
    }
    StartAnimating();
}

void Island::HideBanner() {
    KillTimer(m_hwnd, kBannerTimerId);
    if (m_bannerWanted) {
        m_bannerWanted = false;
        StartAnimating();
    }
}

void Island::Render(bool force) {
    if (!m_hwnd) {
        return;
    }
    Layout();

    // Everything drawn, as a key: nothing to do if it didn't change.
    std::wstring key = std::to_wstring(m_size.cx) + L"|" +
                       std::to_wstring(m_position.x) + L"|" +
                       std::to_wstring((int)m_hover) + m_hoverApp + L"|" +
                       std::to_wstring((int)(m_expand * 1000)) + L"|" +
                       std::to_wstring((int)(m_appear * 1000)) +
                       std::to_wstring(m_away) + L"|" +
                       std::to_wstring(m_position.y) + L"|" +
                       std::to_wstring((int)(PeekAmount(NowSeconds()) * 1000)) +
                       L"|" + std::to_wstring((int)(m_bannerAmount * 1000)) +
                       L"|" + m_banner.appId + m_banner.title + m_banner.text +
                       L"|" + std::to_wstring((int)(m_bannerOpen * 1000)) +
                       L"|" + std::to_wstring(m_bannerTyping) +
                       m_bannerReplyText + L"|" + std::to_wstring(g_trayIconSerial) + L"|" +
                       std::to_wstring((int)(m_trayAmount * 1000)) + L"|" +
                       std::to_wstring((int)(m_trayBounce * 1000)) + L"|" +
                       std::to_wstring((int)(m_trayArrow * 1000)) + L"|" +
                       std::to_wstring((int)(m_lineHover * 1000)) + L"|" +
                       std::to_wstring(m_pinStates.size()) + L"|" +
                       m_pinShownKey +
                       std::to_wstring((int)(PinShown(NowSeconds()) * 100)) +
                       L"|" +
                       std::to_wstring(m_trayKeepOpen) +
                       std::to_wstring((int)g_showNotificationContent.load()) +
                       L"|" + std::to_wstring(m_press.target) + L"|" +
                       std::to_wstring(
                           (int)(m_press.Amount(NowSeconds()) * 1000)) +
                       L"|" +
                       std::to_wstring((int)(m_playSwap * 1000)) + L"|" +
                       std::to_wstring(m_mediaShown.thumbnailSerial) + L"|" +
                       std::to_wstring(m_vertical) + m_clockTime + L"|";
    for (const auto& slot : m_slots) {
        key += slot.text + L"|" + std::to_wstring((int)(slot.scale * 1000)) +
               L"|";
    }
    if (!force && key == m_drawnKey) {
        return;
    }
    if (!m_canvas.Begin(m_size)) {
        return;
    }
    m_drawnKey = key;
    auto* target = m_canvas.target.get();
    const float scale = m_scale;

    winrt::com_ptr<ID2D1SolidColorBrush> brush;
    target->CreateSolidColorBrush(Bg(0.94f), brush.put());

    if (m_bar) {
        target->FillRectangle(m_pill, brush.get());
        brush->SetColor(Fg(0.08f));
        target->FillRectangle({m_pill.left, m_pill.bottom - 1, m_pill.right,
                               m_pill.bottom},
                              brush.get());
    } else {
        // Between the thin line (0) and the pill (1).
        const float e = (float)std::clamp(m_expand, 0.0, 1.1);
        const float centerX = (m_pill.left + m_pill.right) / 2;
        const float hover = (float)std::clamp(m_lineHover, 0.0, 1.2);
        const float lineWidth =
            (kLineWidth + (kLineHoverWidth - kLineWidth) * hover) * scale;
        const float lineTop = kLineTopGap * scale;
        const float lineHeight = (kLineHeight + kLineHoverGrowth * hover) * scale;
        // An invisible strip from the top edge, so the mouse pushed up
        // reaches the line rather than the window behind it.
        if (e < 0.5f && m_bannerAmount <= 0) {
            const float stripWidth = (kLineHoverWidth + 24) * scale;
            brush->SetColor(D2D1::ColorF(0, 0, 0, 1.0f / 255));
            target->FillRectangle(
                {centerX - stripWidth / 2, 0, centerX + stripWidth / 2,
                 (kLineHeight + kLineHoverGrowth + 8) * scale},
                brush.get());
        }
        const float width =
            lineWidth + (m_pill.right - m_pill.left - lineWidth) * e;
        const float top = lineTop + (m_pill.top - lineTop) * e;
        const float height =
            lineHeight + (m_pill.bottom - m_pill.top - lineHeight) * e;
        D2D1_RECT_F rect{centerX - width / 2, top, centerX + width / 2,
                         top + height};
        float lineMix = 1 - std::clamp(e * 2.5f, 0.0f, 1.0f);
        // A new app peeking out: the line becomes a small black pill.
        const float peek = PeekAmount(NowSeconds());
        if (peek > 0) {
            const float peekWidth = 46 * scale;
            const float peekHeight = 30 * scale;
            const D2D1_RECT_F small{centerX - peekWidth / 2, m_pill.top,
                                    centerX + peekWidth / 2,
                                    m_pill.top + peekHeight};
            rect = {rect.left + (small.left - rect.left) * peek,
                    rect.top + (small.top - rect.top) * peek,
                    rect.right + (small.right - rect.right) * peek,
                    rect.bottom + (small.bottom - rect.bottom) * peek};
            lineMix *= 1 - std::clamp(peek, 0.0f, 1.0f);
        }
        // A new notification: the pill stretches into the banner.
        const float banner = (float)std::clamp(m_bannerAmount, -0.05, 1.08);
        if (banner > 0) {
            const D2D1_RECT_F open = BannerRect();
            rect = {rect.left + (open.left - rect.left) * banner,
                    rect.top + (open.top - rect.top) * banner,
                    rect.right + (open.right - rect.right) * banner,
                    rect.bottom + (open.bottom - rect.bottom) * banner};
        }
        // Black, not the line's gray, as the banner opens.
        lineMix *= 1 - std::clamp(banner * 3, 0.0f, 1.0f);
        // Showing or hiding: out of (or into) a dot in its middle.
        const float open = AppearOpen();
        if (open < 1.12f && std::fabs(open - 1) > 0.0005f) {
            const float dot = std::round(10 * scale);
            const float centerY = (rect.top + rect.bottom) / 2;
            const D2D1_RECT_F from{centerX - dot / 2, centerY - dot / 2,
                                   centerX + dot / 2, centerY + dot / 2};
            rect = {from.left + (rect.left - from.left) * open,
                    from.top + (rect.top - from.top) * open,
                    from.right + (rect.right - from.right) * open,
                    from.bottom + (rect.bottom - from.bottom) * open};
            // Not smaller than the dot while bouncing back.
            if (rect.right - rect.left < dot) {
                rect.left = centerX - dot / 2;
                rect.right = centerX + dot / 2;
            }
        }
        // Round ends, whichever way it stands.
        const float pillRadius =
            std::min(rect.bottom - rect.top, rect.right - rect.left) / 2;
        const float bannerRadius = kIslandPanelRadius * scale;
        const float radius = std::min(
            pillRadius,
            pillRadius + (bannerRadius - pillRadius) *
                             std::clamp(banner, 0.0f, 1.0f) *
                             std::clamp(open, 0.0f, 1.0f));
        // The line is light, to be seen over dark and light windows alike.
        const D2D1_COLOR_F background = Bg(0.94f);
        brush->SetColor(D2D1::ColorF(
            background.r + (0.55f - background.r) * lineMix,
            background.g + (0.55f - background.g) * lineMix,
            background.b + (0.55f - background.b) * lineMix,
            background.a + (0.74f - background.a) * lineMix));
        target->FillRoundedRectangle({rect, radius, radius}, brush.get());
        brush->SetColor(Fg(0.10f + 0.3f * lineMix));
        target->DrawRoundedRectangle(
            {{rect.left + 0.5f, rect.top + 0.5f, rect.right - 0.5f,
              rect.bottom - 0.5f},
             radius,
             radius},
            brush.get(), 1);
        if (peek > 0.05f && !m_peekApp.empty() && banner < 0.01f) {
            const float size = 18 * scale * std::clamp(peek, 0.0f, 1.1f);
            const float middle = (rect.top + rect.bottom) / 2;
            const D2D1_RECT_F icon{centerX - size / 2, middle - size / 2,
                                   centerX + size / 2, middle + size / 2};
            const float opacity = std::clamp((peek - 0.3f) / 0.5f, 0.0f, 1.0f);
            if (ID2D1Bitmap* bitmap = GetAppBitmap(m_peekApp)) {
                target->DrawBitmap(bitmap, icon, opacity);
            }
            const float dot = 3.5f * scale * std::clamp(peek, 0.0f, 1.0f);
            brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, opacity));
            target->FillEllipse({{icon.right - dot / 2, icon.top + dot / 2},
                                 dot,
                                 dot},
                                brush.get());
        }
    }

    // The contents fade in near the end of expanding, and away as the banner
    // opens.
    // The contents come at the end of opening out of the dot.
    const float appearContents =
        m_bar ? 1.0f : std::clamp((AppearOpen() - 0.7f) / 0.3f, 0.0f, 1.0f);
    const float contentOpacity =
        appearContents *
        (m_bar ? 1.0f
               : (float)std::clamp((m_expand - 0.6) / 0.4, 0.0, 1.0) *
                     std::clamp(1.0f - (float)m_bannerAmount * 3, 0.0f, 1.0f));

    // The banner's contents: the app's icon, its name and the time, the
    // title and the text.
    const float bannerContents =
        (float)std::clamp((m_bannerAmount - 0.45) / 0.55, 0.0, 1.0) *
        appearContents;
    if (!m_bar && bannerContents > 0.01f && m_bannerAppFormat &&
        m_bannerTimeFormat && m_bannerTitleFormat && m_bannerTextFormat) {
        const D2D1_RECT_F open = BannerRect();
        const float inner = std::round(16 * scale);
        const float iconSize = std::round(38 * scale);
        const float middle = open.top + std::round(kBannerHeight * scale) / 2;
        const D2D1_RECT_F icon{open.left + inner, middle - iconSize / 2,
                               open.left + inner + iconSize,
                               middle + iconSize / 2};
        if (m_banner.appId.empty()) {
            // A reminder: a calendar in a red circle.
            brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, bannerContents));
            target->FillEllipse({{(icon.left + icon.right) / 2,
                                  (icon.top + icon.bottom) / 2},
                                 iconSize / 2,
                                 iconSize / 2},
                                brush.get());
            brush->SetColor(Fg(bannerContents));
            target->DrawText(&kGlyphCalendar, 1, m_iconFormat.get(), icon,
                             brush.get());
        } else if (ID2D1Bitmap* image = GetBannerImage()) {
            // The picture, filling its place (round, or with round corners),
            // and the app's icon small at its corner, like Windows shows it.
            const D2D1_SIZE_F imageSize = image->GetSize();
            const float fill =
                iconSize / std::max(1.0f, std::min(imageSize.width,
                                                   imageSize.height));
            winrt::com_ptr<ID2D1BitmapBrush> imageBrush;
            if (SUCCEEDED(
                    target->CreateBitmapBrush(image, imageBrush.put()))) {
                imageBrush->SetOpacity(bannerContents);
                imageBrush->SetTransform(
                    D2D1::Matrix3x2F::Scale(fill, fill) *
                    D2D1::Matrix3x2F::Translation(
                        (icon.left + icon.right - imageSize.width * fill) / 2,
                        (icon.top + icon.bottom - imageSize.height * fill) /
                            2));
                const float radius = m_banner.imageCircle
                                         ? iconSize / 2
                                         : std::round(8 * scale);
                target->FillRoundedRectangle({icon, radius, radius},
                                             imageBrush.get());
            }
            if (ID2D1Bitmap* bitmap = GetAppBitmap(m_banner.appId)) {
                const float small = std::round(iconSize * 0.45f);
                const float outside = std::round(3 * scale);
                const D2D1_RECT_F corner{icon.right - small + outside,
                                         icon.bottom - small + outside,
                                         icon.right + outside,
                                         icon.bottom + outside};
                target->DrawBitmap(bitmap, corner, bannerContents);
            }
        } else if (ID2D1Bitmap* bitmap = GetAppBitmap(m_banner.appId)) {
            target->DrawBitmap(bitmap, icon, bannerContents);
        }
        const float left = icon.right + std::round(12 * scale);
        const float right = open.right - inner - std::round(4 * scale);
        const float line = std::round(18 * scale);
        const float top = middle - line * 1.5f;
        // The time, or the "x" under the mouse.
        if (BannerHovered() || m_bannerTyping) {
            const D2D1_RECT_F close = BannerCloseRect();
            const float closeRadius = (close.right - close.left) / 2;
            brush->SetColor(Fg((m_hover == IslandItem::BannerClose ? 0.32f : 0.16f) *
                    bannerContents));
            target->FillEllipse({{(close.left + close.right) / 2,
                                  (close.top + close.bottom) / 2},
                                 closeRadius,
                                 closeRadius},
                                brush.get());
            brush->SetColor(Fg(0.95f * bannerContents));
            target->DrawText(&kGlyphClose, 1, m_smallIconFormat.get(), close,
                             brush.get());
        } else {
            brush->SetColor(Fg(0.55f * bannerContents));
            PCWSTR now = Tr(L"now", L"agora");
            target->DrawText(now, (UINT32)wcslen(now),
                             m_bannerTimeFormat.get(),
                             {left, top, right, top + line}, brush.get());
        }
        // Only that there's one, when the content is hidden.
        const bool content = g_showNotificationContent;
        const std::wstring title =
            content ? m_banner.title
                    : std::wstring(Tr(L"New notification", L"Nova notificação"));
        const std::wstring text =
            content ? m_banner.text
                    : std::wstring(Tr(L"Content hidden", L"Conteúdo oculto"));
        brush->SetColor(Fg(0.95f * bannerContents));
        target->DrawText(title.c_str(), (UINT32)title.size(),
                         m_bannerTitleFormat.get(),
                         {left, top, right - std::round(50 * scale),
                          top + line},
                         brush.get());
        brush->SetColor(
            Fg((content ? 0.75f : 0.45f) * bannerContents));
        target->DrawText(text.c_str(), (UINT32)text.size(),
                         m_bannerTextFormat.get(),
                         {left, top + line, right, top + line * 2},
                         brush.get());

        // The reply box and "Open", under the mouse.
        const float row = bannerContents *
                          (float)std::clamp((m_bannerOpen - 0.3) / 0.7, 0.0, 1.0);
        if (row > 0.01f && m_banner.reply.CanReply()) {
            D2D1_RECT_F field, send, openButton;
            BannerParts(&field, &send, &openButton);
            const float fieldRadius = (field.bottom - field.top) / 2;
            brush->SetColor(Fg((m_bannerTyping || m_hover == IslandItem::BannerReply ? 0.18f
                                                                      : 0.12f) *
                    row));
            target->FillRoundedRectangle({field, fieldRadius, fieldRadius},
                                         brush.get());
            if (m_bannerTyping) {
                brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, row));
                target->DrawRoundedRectangle(
                    {{field.left + 0.75f, field.top + 0.75f,
                      field.right - 0.75f, field.bottom - 0.75f},
                     fieldRadius,
                     fieldRadius},
                    brush.get(), 1.5f);
            }
            const D2D1_RECT_F textRect{field.left + std::round(12 * scale),
                                       field.top, send.left, field.bottom};
            const bool empty = m_bannerReplyText.empty();
            const std::wstring shown =
                empty ? (m_banner.reply.placeholder.empty()
                             ? std::wstring(Tr(L"Reply", L"Responder"))
                             : m_banner.reply.placeholder)
                      : m_bannerReplyText;
            float caretX = textRect.left;
            winrt::com_ptr<IDWriteTextLayout> layout;
            if (SUCCEEDED(m_dwrite->CreateTextLayout(
                    shown.c_str(), (UINT32)shown.size(),
                    m_bannerTextFormat.get(), textRect.right - textRect.left,
                    textRect.bottom - textRect.top, layout.put()))) {
                // The end of what's typed stays in view.
                DWRITE_TEXT_METRICS metrics;
                layout->GetMetrics(&metrics);
                const float overflow = std::max(
                    0.0f, metrics.widthIncludingTrailingWhitespace -
                              (textRect.right - textRect.left));
                target->PushAxisAlignedClip(textRect,
                                            D2D1_ANTIALIAS_MODE_ALIASED);
                brush->SetColor(Fg((empty ? 0.45f : 0.95f) * row));
                layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                layout->SetTrimming(
                    &kNoTrimming,
                    nullptr);
                target->DrawTextLayout(
                    {textRect.left - (empty ? 0 : overflow), textRect.top},
                    layout.get(), brush.get());
                target->PopAxisAlignedClip();
                if (!empty) {
                    caretX = std::min(
                        textRect.left + metrics.widthIncludingTrailingWhitespace -
                            overflow,
                        textRect.right);
                }
            }
            if (m_bannerTyping) {
                const float caretMiddle = (field.top + field.bottom) / 2;
                brush->SetColor(Fg(0.9f * row));
                target->FillRectangle(
                    {caretX + 1, caretMiddle - std::round(8 * scale),
                     caretX + 2.5f, caretMiddle + std::round(8 * scale)},
                    brush.get());
            }
            // Send, blue, once something is typed.
            if (!empty) {
                const float sendRadius = (send.bottom - send.top) / 2 -
                                         std::round(3 * scale);
                const D2D1_POINT_2F sendCenter{(send.left + send.right) / 2,
                                               (send.top + send.bottom) / 2};
                brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, row));
                target->FillEllipse({sendCenter, sendRadius, sendRadius},
                                    brush.get());
                brush->SetColor(Fg(row));
                target->DrawText(&kGlyphSend, 1, m_smallIconFormat.get(), send,
                                 brush.get());
            }
            const float openRadius = (openButton.bottom - openButton.top) / 2;
            brush->SetColor(Fg((m_hover == IslandItem::BannerOpen ? 0.26f : 0.16f) * row));
            target->FillRoundedRectangle(
                {openButton, openRadius, openRadius}, brush.get());
            PCWSTR openText = Tr(L"Open", L"Abrir");
            brush->SetColor(Fg(0.95f * row));
            m_bannerTimeFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            target->DrawText(openText, (UINT32)wcslen(openText),
                             m_bannerTimeFormat.get(), openButton,
                             brush.get());
            m_bannerTimeFormat->SetTextAlignment(
                DWRITE_TEXT_ALIGNMENT_TRAILING);
        }
    }
    if (contentOpacity > 0.01f) {
        for (const auto& slot : m_slots) {
            const bool hovered =
                (slot.item == m_hover ||
                 (slot.item == IslandItem::TrayIcon &&
                  m_hover == IslandItem::TrayPin)) &&
                ((slot.item != IslandItem::App &&
                  slot.item != IslandItem::TrayIcon) ||
                 slot.text == m_hoverApp);
            if (hovered || (slot.item == IslandItem::Overflow && m_trayOpen)) {
                const float inset = std::round(4 * scale);
                const D2D1_RECT_F rect =
                    m_vertical
                        ? D2D1_RECT_F{slot.rect.left + inset,
                                      slot.rect.top - inset / 2,
                                      slot.rect.right - inset,
                                      slot.rect.bottom + inset / 2}
                        : D2D1_RECT_F{slot.rect.left - inset / 2,
                                      slot.rect.top + inset,
                                      slot.rect.right + inset / 2,
                                      slot.rect.bottom - inset};
                const float radius = (rect.bottom - rect.top) / 2;
                brush->SetColor(Fg(0.16f * contentOpacity));
                target->FillRoundedRectangle({rect, radius, radius},
                                             brush.get());
            }
            if (slot.item == IslandItem::Overflow && m_iconFormat) {
                // › turning into ‹ as the list opens.
                const D2D1_POINT_2F center{
                    (slot.rect.left + slot.rect.right) / 2,
                    (slot.rect.top + slot.rect.bottom) / 2};
                // Pointing down when upright.
                target->SetTransform(D2D1::Matrix3x2F::Rotation(
                    (float)(m_trayArrow * 180) + (m_vertical ? 90.0f : 0.0f),
                    center));
                brush->SetColor(Fg(0.95f * contentOpacity));
                target->DrawText(slot.text.c_str(), (UINT32)slot.text.size(),
                                 m_smallIconFormat.get(), slot.rect,
                                 brush.get());
                target->SetTransform(D2D1::Matrix3x2F::Identity());
                // A dot under it while it's kept open.
                if (m_trayKeepOpen) {
                    const float dot = 1.6f * scale;
                    brush->SetColor(
                        Fg(0.7f * contentOpacity));
                    target->FillEllipse(
                        {{center.x, slot.rect.bottom - 5 * scale}, dot, dot},
                        brush.get());
                }
                continue;
            }
            if (slot.font == 6) {
                // What's playing: the note turning into the cover, and the
                // title next to it.
                const float shown =
                    contentOpacity * std::clamp(slot.scale, 0.0f, 1.0f);
                const float title = (float)std::clamp(m_mediaTitleRoom, 0.0, 1.0);
                const float press =
                    m_press.target == (int)IslandItem::Media
                        ? (float)m_press.Amount(NowSeconds())
                        : 0.0f;
                const float size = std::round(18 * scale) *
                                   (1 - 0.15f * std::clamp(press, -0.3f, 1.0f));
                const float middle = (slot.rect.top + slot.rect.bottom) / 2;
                // Where the cover sits: from the note's middle to the left.
                const float noteCenter =
                    slot.rect.left + std::round(kIslandIconSlot * scale) / 2;
                const float coverCenter =
                    noteCenter + (slot.rect.left + std::round(2 * scale) +
                                  size / 2 - noteCenter) *
                                     title;
                const D2D1_RECT_F cover{coverCenter - size / 2, middle - size / 2,
                                        coverCenter + size / 2,
                                        middle + size / 2};
                target->PushAxisAlignedClip(slot.rect,
                                            D2D1_ANTIALIAS_MODE_ALIASED);
                ID2D1Bitmap* bitmap = MediaCover();
                if (bitmap && title > 0.01f) {
                    winrt::com_ptr<ID2D1RoundedRectangleGeometry> clip;
                    m_d2d->CreateRoundedRectangleGeometry(
                        {cover, std::round(4 * scale), std::round(4 * scale)},
                        clip.put());
                    winrt::com_ptr<ID2D1Layer> layer;
                    target->CreateLayer(nullptr, layer.put());
                    target->PushLayer(
                        D2D1::LayerParameters(D2D1::InfiniteRect(), clip.get(),
                                              D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                              D2D1::IdentityMatrix(),
                                              shown * title),
                        layer.get());
                    // Filled, keeping its proportions.
                    const D2D1_SIZE_F bitmapSize = bitmap->GetSize();
                    const float fill = std::max(
                        size / std::max(bitmapSize.width, 1.0f),
                        size / std::max(bitmapSize.height, 1.0f));
                    const float w = bitmapSize.width * fill;
                    const float h = bitmapSize.height * fill;
                    target->DrawBitmap(bitmap,
                                       {coverCenter - w / 2, middle - h / 2,
                                        coverCenter + w / 2, middle + h / 2});
                    target->PopLayer();
                }
                // The note, fading as the cover comes (or staying, without a
                // cover).
                const float note = bitmap ? 1 - title : 1.0f;
                if (note > 0.01f && m_iconFormat) {
                    brush->SetColor(Fg(0.95f * shown * note));
                    target->DrawText(&kGlyphMusic, 1, m_iconFormat.get(), cover,
                                     brush.get());
                }
                if (title > 0.01f && m_bannerTextFormat) {
                    const float textLeft = slot.rect.left + std::round(2 * scale) +
                                           size + std::round(8 * scale);
                    brush->SetColor(Fg(0.92f * shown * title));
                    target->DrawText(slot.text.c_str(), (UINT32)slot.text.size(),
                                     m_bannerTextFormat.get(),
                                     {textLeft, slot.rect.top,
                                      std::max(textLeft,
                                               slot.rect.right -
                                                   std::round(4 * scale)),
                                      slot.rect.bottom},
                                     brush.get());
                }
                target->PopAxisAlignedClip();
                continue;
            }
            if (slot.font == 5) {
                // The line between the open list and the pinned icons.
                const float lineX = std::round((slot.rect.left + slot.rect.right) / 2);
                const float half = std::round(8 * scale);
                const float middle = std::round((slot.rect.top + slot.rect.bottom) / 2);
                brush->SetColor(
                    Fg(0.2f * contentOpacity * std::clamp(slot.scale, 0.0f, 1.0f)));
                // Across the pill, whichever way it stands.
                target->FillRectangle(
                    m_vertical ? D2D1_RECT_F{lineX - half, middle - 0.5f,
                                             lineX + half, middle + 0.5f}
                               : D2D1_RECT_F{lineX - 0.5f, middle - half,
                                             lineX + 0.5f, middle + half},
                    brush.get());
                continue;
            }
            if (slot.font == 4) {
                const TrayIcon* icon = FindTrayIcon(slot.text);
                const float shown = std::max(slot.scale, 0.0f);
                const float size = 16 * scale * shown;
                const float centerX = (slot.rect.left + slot.rect.right) / 2;
                const float centerY = (slot.rect.top + slot.rect.bottom) / 2;
                const D2D1_RECT_F rect{centerX - size / 2, centerY - size / 2,
                                       centerX + size / 2, centerY + size / 2};
                const float opacity =
                    contentOpacity * std::clamp(shown, 0.0f, 1.0f);
                if (ID2D1Bitmap* bitmap = icon ? GetTrayBitmap(*icon) : nullptr) {
                    target->DrawBitmap(bitmap, rect, opacity);
                }
                // The pin, on the icon under the mouse after a moment
                // (growing in): filled when pinned.
                const float pinShown =
                    hovered && slot.text == m_pinShownKey
                        ? PinShown(NowSeconds())
                        : 0.0f;
                if (pinShown > 0.01f && opacity > 0.5f) {
                    const D2D1_RECT_F fullPin = TrayPinRect(slot.rect);
                    const D2D1_POINT_2F pinCenter{
                        (fullPin.left + fullPin.right) / 2,
                        (fullPin.top + fullPin.bottom) / 2};
                    const float pinRadius =
                        (fullPin.right - fullPin.left) / 2 * pinShown;
                    const bool pinned = IsTrayPinned(slot.text);
                    brush->SetColor(
                        pinned ? D2D1::ColorF(0.04f, 0.52f, 1.0f, opacity)
                               : D2D1::ColorF(
                                     0.25f, 0.25f, 0.25f,
                                     (m_hover == IslandItem::TrayPin ? 1.0f
                                                                     : 0.9f) *
                                         opacity));
                    target->FillEllipse({pinCenter, pinRadius, pinRadius},
                                        brush.get());
                    // The glyph grows with it.
                    target->SetTransform(D2D1::Matrix3x2F::Scale(
                        pinShown, pinShown, pinCenter));
                    brush->SetColor(Fg(opacity));
                    target->DrawText(&kGlyphPin, 1, m_smallIconFormat.get(),
                                     fullPin, brush.get());
                    target->SetTransform(D2D1::Matrix3x2F::Identity());
                }
                continue;
            }
            if (slot.font == 3) {
                const float shown = std::max(slot.scale, 0.0f);
                const float size = 18 * scale * shown;
                const float centerX = (slot.rect.left + slot.rect.right) / 2;
                const float centerY = (slot.rect.top + slot.rect.bottom) / 2;
                const D2D1_RECT_F icon{centerX - size / 2, centerY - size / 2,
                                       centerX + size / 2, centerY + size / 2};
                const float opacity =
                    contentOpacity * std::clamp(shown, 0.0f, 1.0f);
                if (ID2D1Bitmap* bitmap = GetAppBitmap(slot.text)) {
                    target->DrawBitmap(bitmap, icon, opacity);
                } else {
                    brush->SetColor(Fg(0.3f * opacity));
                    target->FillRoundedRectangle(
                        {icon, size / 4, size / 4}, brush.get());
                }
                const float dot = 3.5f * scale * shown;
                brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, opacity));
                target->FillEllipse(
                    {{icon.right - dot / 2, icon.top + dot / 2}, dot, dot},
                    brush.get());
                continue;
            }
            auto* format = slot.font == 1   ? m_iconFormat.get()
                           : slot.font == 2 ? m_smallIconFormat.get()
                                            : m_textFormat.get();
            if (!format) {
                continue;
            }
            // Growing in or out, pressed, or play turning into pause.
            float glyphScale = std::max(slot.scale, 0.0f);
            if (m_press.target && (int)slot.item == m_press.target) {
                glyphScale *=
                    1 - 0.24f * (float)std::clamp(
                                    m_press.Amount(NowSeconds()), -0.3, 1.0);
            }
            if (slot.item == IslandItem::MediaPlayPause) {
                glyphScale *=
                    1 - 0.55f * (float)std::clamp(m_playSwap, 0.0, 1.0);
            }
            const float glyphOpacity = std::clamp(slot.scale, 0.0f, 1.0f);
            const bool scaled = std::fabs(glyphScale - 1) > 0.001f;
            if (scaled) {
                target->SetTransform(D2D1::Matrix3x2F::Scale(
                    glyphScale, glyphScale,
                    {(slot.rect.left + slot.rect.right) / 2,
                     (slot.rect.top + slot.rect.bottom) / 2}));
            }
            brush->SetColor(
                Fg((slot.item == IslandItem::Minimize ? 0.6f : 0.95f) *
                   contentOpacity * glyphOpacity));
            // Upright, the clock is two lines: the time, the date under it.
            if (slot.font == 0 && m_vertical && m_bannerTimeFormat) {
                const float middle = (slot.rect.top + slot.rect.bottom) / 2;
                const D2D1_RECT_F timeRect{slot.rect.left, slot.rect.top,
                                           slot.rect.right, middle + 2 * scale};
                const D2D1_RECT_F dateRect{slot.rect.left, middle,
                                           slot.rect.right, slot.rect.bottom};
                target->DrawText(m_clockTime.c_str(),
                                 (UINT32)m_clockTime.size(), format, timeRect,
                                 brush.get());
                brush->SetColor(Fg(0.6f * contentOpacity));
                m_bannerTimeFormat->SetTextAlignment(
                    DWRITE_TEXT_ALIGNMENT_CENTER);
                target->DrawText(m_clockDate.c_str(),
                                 (UINT32)m_clockDate.size(),
                                 m_bannerTimeFormat.get(), dateRect,
                                 brush.get());
                m_bannerTimeFormat->SetTextAlignment(
                    DWRITE_TEXT_ALIGNMENT_TRAILING);
                if (scaled) {
                    target->SetTransform(D2D1::Matrix3x2F::Identity());
                }
                continue;
            }
            // Text with figures of equal width; icons as they are.
            winrt::com_ptr<IDWriteTextLayout> layout;
            if (slot.font == 0 && m_tabular &&
                SUCCEEDED(m_dwrite->CreateTextLayout(
                    slot.text.c_str(), (UINT32)slot.text.size(), format,
                    slot.rect.right - slot.rect.left,
                    slot.rect.bottom - slot.rect.top, layout.put()))) {
                layout->SetTypography(m_tabular.get(),
                                      {0, (UINT32)slot.text.size()});
                target->DrawTextLayout({slot.rect.left, slot.rect.top},
                                       layout.get(), brush.get());
                if (scaled) {
                    target->SetTransform(D2D1::Matrix3x2F::Identity());
                }
                continue;
            }
            // Icons growing in are drawn at their full size (scaled), not
            // squeezed into their room.
            D2D1_RECT_F glyphRect = slot.rect;
            if (slot.scale < 1) {
                const float full =
                    std::round((slot.font == 2 ? kIslandSmallSlot
                                               : kIslandIconSlot) *
                               scale);
                const float centerX = (slot.rect.left + slot.rect.right) / 2;
                glyphRect.left = centerX - full / 2;
                glyphRect.right = centerX + full / 2;
            }
            target->DrawText(slot.text.c_str(), (UINT32)slot.text.size(),
                             format, glyphRect, brush.get());
            if (scaled) {
                target->SetTransform(D2D1::Matrix3x2F::Identity());
            }
        }
    }

    // Off the screen while away (over a full-screen app).
    m_canvas.End(m_hwnd, m_away ? POINT{-32000, -32000} : m_position,
                 m_away ? 0.0f
                 : m_bar ? (float)std::clamp(m_appear, 0.0, 1.0)
                         : AppearFade());
    PublishTrayPlaces();
}

// How far the peeking app's little pill is out, from 0 to 1 (a bit more
// while bouncing): it springs out, stays a moment, and goes back.
constexpr double kPeekIn = 0.45;
constexpr double kPeekHold = 1.9;
constexpr double kPeekOut = 0.45;

float Island::PeekAmount(double now) {
    if (!m_peekStart) {
        return 0;
    }
    const double t = now - m_peekStart;
    if (t < kPeekIn) {
        return (float)SpringOut(t / kPeekIn);
    }
    if (t < kPeekHold) {
        return 1;
    }
    if (t < kPeekHold + kPeekOut) {
        return 1 - (float)EaseInOutCubic((t - kPeekHold) / kPeekOut);
    }
    return 0;
}

ID2D1Bitmap* Island::GetBannerImage() {
    if (m_banner.image.empty() || !m_wic || !m_canvas.target) {
        return nullptr;
    }
    if (m_bannerImagePath == m_banner.image) {
        return m_bannerImage.get();
    }
    m_bannerImagePath = m_banner.image;
    m_bannerImage = nullptr;
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    winrt::com_ptr<IWICFormatConverter> converter;
    winrt::com_ptr<IWICStream> stream;
    const bool decoded =
        m_banner.imageData.empty()
            ? SUCCEEDED(m_wic->CreateDecoderFromFilename(
                  m_banner.image.c_str(), nullptr, GENERIC_READ,
                  WICDecodeMetadataCacheOnDemand, decoder.put()))
            : SUCCEEDED(m_wic->CreateStream(stream.put())) &&
                  SUCCEEDED(stream->InitializeFromMemory(
                      m_banner.imageData.data(),
                      (DWORD)m_banner.imageData.size())) &&
                  SUCCEEDED(m_wic->CreateDecoderFromStream(
                      stream.get(), nullptr, WICDecodeMetadataCacheOnLoad,
                      decoder.put()));
    if (decoded && SUCCEEDED(decoder->GetFrame(0, frame.put())) &&
        SUCCEEDED(m_wic->CreateFormatConverter(converter.put())) &&
        SUCCEEDED(converter->Initialize(
            frame.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        m_canvas.target->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                   m_bannerImage.put());
    }
    return m_bannerImage.get();
}

ID2D1Bitmap* Island::GetAppBitmap(const std::wstring& appId) {
    for (const auto& [id, bitmap] : m_appBitmaps) {
        if (id == appId) {
            return bitmap.get();
        }
    }
    winrt::com_ptr<ID2D1Bitmap> bitmap =
        LoadAppIcon(m_canvas.target.get(), m_wic.get(), m_hwnd, appId,
                    (int)std::round(32 * m_scale));
    // Remembered even when missing, so it isn't looked up every frame.
    m_appBitmaps.push_back({appId, bitmap});
    return bitmap.get();
}

void Island::StartAnimating() {
    if (!m_animating) {
        m_animating = true;
        m_lastFrame = NowSeconds();
    }
}

void Island::AnimationFrame() {
    const double now = NowSeconds();
    const double dt = std::clamp(now - m_lastFrame, 0.0, 0.05);
    m_lastFrame = now;

    bool expanding =
        m_minimized ? SpringTowards(m_expand, m_expandVelocity, 0,
                                    kCloseSeconds * 1.3, dt)
                    : SpringTowards(m_expand, m_expandVelocity, 1,
                                    kOpenSeconds, dt, kBounceDamping);
    // Out of the dot (bouncing a little), or back into it.
    if (m_visible ? SpringTowards(m_appear, m_appearVelocity, 1,
                                  kAppearSeconds, dt, kPanelWidthDamping)
                  : SpringTowards(m_appear, m_appearVelocity, 0,
                                  kDisappearSeconds, dt)) {
        expanding = true;
    } else if (!m_visible && !m_away) {
        m_away = true;
        Render(true);
    }
    bool apps = false;
    for (auto& entry : m_appEntries) {
        if (SpringTowards(entry.shown, entry.velocity,
                          entry.present ? 1.0 : 0.0, kOpenSeconds, dt,
                          entry.present ? kBounceDamping : 1.0)) {
            apps = true;
        }
    }
    std::erase_if(m_appEntries, [](const AppEntry& entry) {
        return !entry.present && entry.shown <= 0;
    });
    // The minimized line under the mouse.
    if (SpringTowards(m_lineHover, m_lineHoverVelocity,
                      m_minimized && m_hover == IslandItem::Line ? 1 : 0,
                      kOpenSeconds * 0.8, dt, kBounceDamping)) {
        apps = true;
    }
    // The tray icons' list: out like a panel, back without bouncing.
    const double trayTarget = m_trayOpen ? 1 : 0;
    const bool trayRoom =
        SpringTowards(m_trayAmount, m_trayVelocity, trayTarget,
                      m_trayOpen ? kPanelWidthSeconds * 0.8 : kPanelCloseSeconds,
                      dt);
    const bool trayBounce =
        m_trayOpen ? SpringTowards(m_trayBounce, m_trayBounceVelocity, 1,
                                   kPanelWidthSeconds, dt, kPanelWidthDamping)
                   : SpringTowards(m_trayBounce, m_trayBounceVelocity, 0,
                                   kPanelCloseSeconds, dt);
    const bool trayArrow =
        SpringTowards(m_trayArrow, m_trayArrowVelocity, trayTarget,
                      kOpenSeconds, dt, kBounceDamping);
    if (trayRoom || trayBounce || trayArrow) {
        apps = true;
    }
    for (auto& state : m_pinStates) {
        if (SpringTowards(state.amount, state.velocity,
                          IsTrayPinned(state.key) ? 1 : 0, kOpenSeconds, dt)) {
            apps = true;
        }
    }
    std::erase_if(m_pinStates, [this](const PinState& state) {
        return state.velocity == 0 &&
               state.amount == (IsTrayPinned(state.key) ? 1 : 0);
    });
    // Sliding to its place (unless it's being dragged).
    if (!m_dragMoved) {
        const IslandPlace& place = CurrentPlace();
        if (SpringTowards(m_placeX, m_placeVelocityX, place.x,
                          kOpenSeconds * 1.6, dt, kBounceDamping)) {
            apps = true;
        }
        if (SpringTowards(m_placeY, m_placeVelocityY, place.y,
                          kOpenSeconds * 1.6, dt, kBounceDamping)) {
            apps = true;
        }
    }
    // What's playing, and its buttons, growing in (bouncing) and out.
    if (SpringTowards(m_mediaAmount, m_mediaVelocity, m_mediaWanted ? 1 : 0,
                      m_mediaWanted ? kOpenSeconds * 1.25 : kCloseSeconds * 1.8,
                      dt, m_mediaWanted ? kBounceDamping : 1.0)) {
        apps = true;
    }
    const bool buttonsWanted = g_mediaButtonsInPill.load();
    if (SpringTowards(m_mediaButtonsAmount, m_mediaButtonsVelocity,
                      buttonsWanted ? 1 : 0,
                      buttonsWanted ? kOpenSeconds * 1.25 : kCloseSeconds * 1.8,
                      dt, buttonsWanted ? kBounceDamping : 1.0)) {
        apps = true;
    }
    // A pressed button.
    if (m_press.Animate(now)) {
        apps = true;
    }
    // The room for what's playing and its buttons, without bouncing.
    if (SpringTowards(m_mediaRoom, m_mediaRoomVelocity, m_mediaWanted ? 1 : 0,
                      m_mediaWanted ? kOpenSeconds * 1.1 : kCloseSeconds * 1.8,
                      dt)) {
        apps = true;
    }
    if (SpringTowards(m_mediaTitleRoom, m_mediaTitleRoomVelocity,
                      g_mediaTitleInPill ? 1 : 0,
                      g_mediaTitleInPill ? kOpenSeconds * 1.2 : kCloseSeconds * 1.8,
                      dt)) {
        apps = true;
    }
    if (SpringTowards(m_mediaButtonsRoom, m_mediaButtonsRoomVelocity,
                      buttonsWanted ? 1 : 0,
                      buttonsWanted ? kOpenSeconds * 1.1 : kCloseSeconds * 1.8,
                      dt)) {
        apps = true;
    }
    if (SpringTowards(m_playSwap, m_playSwapVelocity, 0, 0.4, dt,
                      kBounceDamping)) {
        apps = true;
    }
    // The banner's reply row, under the mouse or while typing.
    const bool wantRow = m_bannerWanted && m_banner.reply.CanReply() &&
                         (m_bannerTyping || BannerHovered());
    if (SpringTowards(m_bannerOpen, m_bannerOpenVelocity, wantRow ? 1 : 0,
                      kOpenSeconds, dt, wantRow ? kBounceDamping : 1.0)) {
        apps = true;
    }
    // The banner: out like a panel, back without bouncing.
    const bool bannerMoving =
        m_bannerWanted
            ? SpringTowards(m_bannerAmount, m_bannerVelocity, 1,
                            kPanelWidthSeconds, dt, kPanelWidthDamping)
            : SpringTowards(m_bannerAmount, m_bannerVelocity, 0,
                            kPanelCloseSeconds, dt);
    if (bannerMoving) {
        apps = true;
    } else if (!m_bannerWanted && !m_banner.appId.empty()) {
        m_banner = {};
        m_bannerOpen = 0;
        m_bannerOpenVelocity = 0;
    }
    if (m_peekStart) {
        if (now - m_peekStart >= kPeekHold + kPeekOut || !m_minimized) {
            m_peekStart = 0;
            m_peekApp.clear();
        } else {
            apps = true;
        }
    }
    // A pin growing in.
    if (!m_pinShownKey.empty() && now - m_pinShownAt < kPinGrowSeconds) {
        apps = true;
    }
    Render();
    const bool calendar = m_calendar.Animate(dt);
    const bool control = m_control.Animate(dt);
    const bool notifications = m_notifications.Animate(dt);
    const bool mediaPanel = m_mediaPanel.Animate(dt);
    m_animating = expanding || calendar || control || notifications ||
                  mediaPanel || apps;
}

void Island::SetMinimized(bool minimized) {
    m_minimized = minimized;
    Wh_SetIntValue(L"islandMinimized", minimized);
    if (minimized) {
        m_calendar.Close();
        m_control.Close();
        m_notifications.Close();
        m_mediaPanel.Close();
    }
    StartAnimating();
}

// What's playing shows on the pill a moment longer than it plays, so media
// starting and stopping quickly (or a page changing songs) doesn't flash.
void Island::UpdateMediaShown() {
    const double now = NowSeconds();
    if (m_mediaState.present) {
        KillTimer(m_hwnd, kMediaTimerId);
        m_mediaGoneAt = 0;
        if (m_mediaShown.present && m_mediaShown.playing != m_mediaState.playing) {
            // Play turning into pause: it shrinks and grows again.
            m_playSwap = 1;
            m_playSwapVelocity = 0;
        }
        m_mediaShown = m_mediaState;
        if (!m_mediaWanted) {
            m_mediaWanted = true;
            m_mediaSince = now;
        }
        StartAnimating();
        return;
    }
    if (!m_mediaWanted) {
        return;
    }
    if (!m_mediaGoneAt) {
        m_mediaGoneAt = now;
    }
    const double until =
        std::max(m_mediaGoneAt + kMediaLinger, m_mediaSince + kMediaMinShown);
    if (now + 0.01 >= until) {
        m_mediaWanted = false;
        m_mediaShown.present = false;
        // Nothing playing anymore: its panel closes.
        m_mediaPanel.Close();
        StartAnimating();
        return;
    }
    SetTimer(m_hwnd, kMediaTimerId, (UINT)((until - now) * 1000) + 10, nullptr);
}

// Whether a visible window of another app is above the island where it is.
bool Island::IsCovered() const {
    RECT island;
    if (!GetWindowRect(m_hwnd, &island)) {
        return false;
    }
    const DWORD self = GetCurrentProcessId();
    for (HWND above = GetWindow(m_hwnd, GW_HWNDPREV); above;
         above = GetWindow(above, GW_HWNDPREV)) {
        DWORD processId = 0;
        GetWindowThreadProcessId(above, &processId);
        RECT rect;
        RECT overlap;
        int cloaked = 0;
        if (processId != self && IsWindowVisible(above) &&
            (FAILED(DwmGetWindowAttribute(above, DWMWA_CLOAKED, &cloaked,
                                          sizeof(cloaked))) ||
             !cloaked) &&
            GetWindowRect(above, &rect) &&
            IntersectRect(&overlap, &rect, &island)) {
            return true;
        }
    }
    return false;
}

void Island::Tick() {
    // Not over a full-screen app (a game or a video).
    HWND taskbar = FindTaskbarWnd();
    const bool show = !(taskbar && IsFullscreenAppOnMonitor(taskbar));
    if (show != m_visible) {
        m_visible = show;
        // Hidden once it has closed into the dot and faded (see
        // AnimationFrame).
        if (show) {
            m_away = false;
            // Shown once, still fully transparent; never hidden again.
            if (!m_shownOnce) {
                m_shownOnce = true;
                ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
            }
        } else {
            m_calendar.Close();
            m_control.Close();
            m_notifications.Close();
            m_mediaPanel.Close();
        }
        StartAnimating();
    }
    if (!show) {
        return;
    }
    if (++m_ticks >= kIslandStatusTicks) {
        m_ticks = 0;
        UpdateStatus();
    }
    if (++m_raiseTicks >= kIslandRaiseTicks) {
        m_raiseTicks = 0;
        m_control.Refresh();
        m_notifications.Refresh();
        // Other always-on-top windows can cover it: raised again only then (and
        // an open panel stays above it), so it doesn't fight over the top.
        if (IsCovered()) {
            SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            m_calendar.Raise();
            m_control.Raise();
            m_notifications.Raise();
            m_mediaPanel.Raise();
        }
    }
    UpdateClock();
    m_calendar.Refresh();
    m_mediaPanel.Refresh();
    // Reminders due now.
    for (const auto& event : TakeDueReminders()) {
        ShowReminder(event);
    }
    Render();
}

// A reminder in the pill, like a notification (on the calendar's day when
// clicked), with the reminder sound.
void Island::ShowReminder(const CalendarEvent& event) {
    MessageBeep(MB_ICONASTERISK);
    IslandToast toast;
    toast.title = event.title;
    toast.text = EventTimeText(event.minute);
    toast.sticky = true;
    m_reminderDay = {event.year, event.month, event.day};
    AcquireSRWLockExclusive(&g_islandToastsLock);
    g_islandToasts.push_back(std::move(toast));
    ReleaseSRWLockExclusive(&g_islandToastsLock);
    ShowBanner();
}

// Takes the apps with a badge from the taskbar: new ones grow in (and peek
// out of the line when the pill is minimized), the others shrink away.
void Island::UpdateApps() {
    AcquireSRWLockShared(&g_badgeAppsLock);
    m_badgeApps = g_badgeApps;
    ReleaseSRWLockShared(&g_badgeAppsLock);
    // New apps grow in; apps without a badge anymore shrink away.
    bool changed = false;
    for (const auto& id : m_badgeApps) {
        auto it = std::find_if(m_appEntries.begin(), m_appEntries.end(),
                               [&](const AppEntry& e) { return e.id == id; });
        if (it == m_appEntries.end()) {
            m_appEntries.push_back({id});
            changed = true;
            if (m_minimized && !m_bar) {
                m_peekApp = id;
                m_peekStart = NowSeconds();
            }
        } else if (!it->present) {
            it->present = true;
            changed = true;
        }
    }
    for (auto& entry : m_appEntries) {
        const bool present = std::find(m_badgeApps.begin(), m_badgeApps.end(),
                                       entry.id) != m_badgeApps.end();
        if (entry.present && !present) {
            entry.present = false;
            changed = true;
        }
    }
    if (changed) {
        StartAnimating();
    }
}

// The Windows banner for a new notification is a window of the shell
// experience host that's uncloaked at the bottom right, starting with no
// height. The island shows notifications its own way (in the pill, or only
// its icons and the list), so it's moved off the screen as it shows (its
// sound still plays, and it still goes to the notification center). Windows
// places it again for each notification, so nothing stays changed if the mod
// stops.
HWND g_windowsBanner;
DWORD g_shellExperienceHostId;

bool IsShellExperienceHost(DWORD processId) {
    if (processId == g_shellExperienceHostId) {
        return true;
    }
    bool is = false;
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                     processId)) {
        WCHAR path[MAX_PATH];
        DWORD length = ARRAYSIZE(path);
        if (QueryFullProcessImageName(process, 0, path, &length)) {
            PCWSTR name = wcsrchr(path, L'\\');
            is = _wcsicmp(name ? name + 1 : path, L"ShellExperienceHost.exe") ==
                 0;
        }
        CloseHandle(process);
    }
    if (is) {
        g_shellExperienceHostId = processId;
    }
    return is;
}

void CALLBACK BannerWinEventProc(HWINEVENTHOOK,
                                 DWORD event,
                                 HWND hWnd,
                                 LONG idObject,
                                 LONG idChild,
                                 DWORD,
                                 DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hWnd ||
        !g_island) {
        return;
    }
    if (event == kEventObjectUncloaked && hWnd != g_windowsBanner) {
        WCHAR className[64];
        RECT rect;
        DWORD processId = 0;
        // Recognized when it first shows: no height yet.
        // Not a flyout the user opened (the notification center, the quick
        // settings): those come to the front.
        if (GetForegroundWindow() != hWnd &&
            GetClassName(hWnd, className, ARRAYSIZE(className)) &&
            wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 &&
            GetWindowRect(hWnd, &rect) && rect.top == rect.bottom &&
            GetWindowThreadProcessId(hWnd, &processId) &&
            IsShellExperienceHost(processId)) {
            g_windowsBanner = hWnd;
            g_island->WatchWindowsBannerMoves(processId);
        }
    }
    if (hWnd == g_windowsBanner) {
        g_island->OnWindowsBanner(hWnd);
    }
}

// Once the banner is known, its moves (only that process's windows).
void Island::WatchWindowsBannerMoves(DWORD processId) {
    if (m_bannerMoveHook) {
        UnhookWinEvent(m_bannerMoveHook);
    }
    m_bannerMoveHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        BannerWinEventProc, processId, 0, WINEVENT_OUTOFCONTEXT);
}

// Whether the pill shows new notifications now (see ShowBanner): not in the
// bar style, with its banners off, minimized without them, while a panel is
// open, in full screen, or when new notifications can't be read.
bool Island::CanTakeBanners() const {
    return g_settings.islandHideWindowsBanners && g_bannerEnabled &&
           g_toastWatchReady && !m_bar && m_visible && !m_bannerTyping &&
           !(m_minimized && !g_bannerWhenMinimized) && !m_calendar.IsOpen() &&
           !m_control.IsOpen() && !m_notifications.IsOpen() &&
           !m_mediaPanel.IsOpen();
}

// Moves the Windows banner off the screen when the pill shows the
// notification instead. It's moved as soon as it shows, when the pill can
// show notifications, so it doesn't flash; if the pill doesn't take the
// notification shortly after (one it can't read), it's put back. Never a
// window that's in front (a flyout the user opened).
void Island::OnWindowsBanner(HWND banner) {
    const double now = NowSeconds();
    const bool taken =
        now - m_bannerTakenAt < kBannerShownMs / 1000.0 + 3;
    const bool ahead = !taken && CanTakeBanners() &&
                       now - m_bannerRestoredAt > kBannerShownMs / 1000.0 + 3;
    if (!g_settings.islandHideWindowsBanners || (!taken && !ahead) ||
        GetForegroundWindow() == banner) {
        return;
    }
    int cloaked = 0;
    RECT rect;
    if (FAILED(DwmGetWindowAttribute(banner, DWMWA_CLOAKED, &cloaked,
                                     sizeof(cloaked))) ||
        cloaked || !GetWindowRect(banner, &rect)) {
        return;
    }
    const int below = GetSystemMetrics(SM_YVIRTUALSCREEN) +
                      GetSystemMetrics(SM_CYVIRTUALSCREEN) + 200;
    if (rect.top >= below) {
        return;
    }
    if (ahead && now - m_bannerHiddenAt > kBannerRestoreMs / 1000.0) {
        m_bannerHiddenAt = now;
        m_bannerHomeTop = rect.top;
        SetTimer(m_hwnd, kBannerRestoreTimerId, kBannerRestoreMs, nullptr);
    }
    SetWindowPos(banner, nullptr, rect.left, below, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// The pill didn't take the notification it hid the Windows banner for: the
// banner comes back where it was.
void Island::RestoreWindowsBanner() {
    if (m_bannerTakenAt >= m_bannerHiddenAt || !g_windowsBanner ||
        !IsWindow(g_windowsBanner)) {
        return;
    }
    RECT rect;
    const int below = GetSystemMetrics(SM_YVIRTUALSCREEN) +
                      GetSystemMetrics(SM_CYVIRTUALSCREEN) + 200;
    if (GetWindowRect(g_windowsBanner, &rect) && rect.top >= below) {
        m_bannerRestoredAt = NowSeconds();
        SetWindowPos(g_windowsBanner, nullptr, rect.left, m_bannerHomeTop, 0,
                     0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Moves an app's small window, opened from its tray icon at the bottom of
// the screen (where it thinks the icon is), under the island.
void CALLBACK TrayAppWinEventProc(HWINEVENTHOOK,
                                  DWORD,
                                  HWND hWnd,
                                  LONG idObject,
                                  LONG idChild,
                                  DWORD,
                                  DWORD) {
    if (idObject == OBJID_WINDOW && idChild == CHILDID_SELF && hWnd &&
        g_island) {
        g_island->OnTrayAppWindow(hWnd);
    }
}

void Island::WatchTrayAppWindows(const TrayIcon& icon) {
    StopWatchingTrayAppWindows();
    m_trayAppAnchor = {-1, -1};
    // On the lower half of the screen (or a side), the app's window already
    // opens near the island.
    if (AtBottom() || m_vertical) {
        return;
    }
    for (const auto& slot : m_slots) {
        if (slot.item == IslandItem::TrayIcon && slot.text == icon.key) {
            m_trayAppAnchor = {
                m_position.x + (LONG)((slot.rect.left + slot.rect.right) / 2),
                m_position.y + (LONG)(m_pill.bottom + kPanelGap * m_scale)};
        }
    }
    if (m_trayAppAnchor.x < 0) {
        return;
    }
    // Shown or brought to the front within a moment.
    m_trayAppHooks[0] = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, TrayAppWinEventProc,
        icon.processId, 0, WINEVENT_OUTOFCONTEXT);
    m_trayAppHooks[1] = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        TrayAppWinEventProc, icon.processId, 0, WINEVENT_OUTOFCONTEXT);
    SetTimer(m_hwnd, kTrayAppTimerId, 2000, nullptr);
}

void Island::StopWatchingTrayAppWindows() {
    for (auto& hook : m_trayAppHooks) {
        if (hook) {
            UnhookWinEvent(hook);
            hook = nullptr;
        }
    }
    if (m_hwnd) {
        KillTimer(m_hwnd, kTrayAppTimerId);
    }
}

void Island::OnTrayAppWindow(HWND hWnd) {
    if (GetAncestor(hWnd, GA_ROOT) != hWnd || !IsWindowVisible(hWnd) ||
        IsZoomed(hWnd) || m_trayAppAnchor.x < 0) {
        return;
    }
    RECT rect;
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetWindowRect(hWnd, &rect) ||
        !GetMonitorInfo(MonitorFromPoint(m_trayAppAnchor,
                                         MONITOR_DEFAULTTONEAREST),
                        &monitor)) {
        return;
    }
    const RECT& screen = monitor.rcMonitor;
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    // Only a small window in the lower part of the screen (a flyout placed
    // next to where the taskbar's icon would be), not the app's own window.
    if (width <= 0 || height <= 0 ||
        width > (screen.right - screen.left) * 6 / 10 ||
        height > (screen.bottom - screen.top) * 7 / 10 ||
        rect.top < screen.top + (screen.bottom - screen.top) / 3) {
        return;
    }
    const int x = std::clamp((int)m_trayAppAnchor.x - width / 2,
                             (int)screen.left + 8,
                             std::max((int)screen.left + 8,
                                      (int)screen.right - width - 8));
    SetWindowPos(hWnd, nullptr, x, m_trayAppAnchor.y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    StopWatchingTrayAppWindows();
}

// Takes the tray icons from the taskbar.
void Island::UpdateTrayIcons() {
    AcquireSRWLockShared(&g_trayIconsLock);
    m_trayIcons.clear();
    for (const auto& icon : g_trayIcons) {
        if (!icon.hidden && icon.icon) {
            m_trayIcons.push_back(icon);
        }
    }
    ReleaseSRWLockShared(&g_trayIconsLock);
    // Bitmaps of icons that are gone.
    std::erase_if(m_trayBitmaps, [&](const auto& entry) {
        return !FindTrayIcon(std::get<0>(entry));
    });
}

const TrayIcon* Island::FindTrayIcon(const std::wstring& key) const {
    for (const auto& icon : m_trayIcons) {
        if (icon.key == key) {
            return &icon;
        }
    }
    return nullptr;
}

ID2D1Bitmap* Island::GetTrayBitmap(const TrayIcon& icon) {
    for (auto& [key, serial, bitmap] : m_trayBitmaps) {
        if (key == icon.key) {
            if (serial == icon.serial) {
                return bitmap.get();
            }
            break;
        }
    }
    std::erase_if(m_trayBitmaps, [&](const auto& entry) {
        return std::get<0>(entry) == icon.key;
    });
    winrt::com_ptr<ID2D1Bitmap> bitmap;
    winrt::com_ptr<IWICBitmap> wicBitmap;
    winrt::com_ptr<IWICFormatConverter> converter;
    if (m_wic && icon.icon &&
        SUCCEEDED(m_wic->CreateBitmapFromHICON(icon.icon->icon,
                                               wicBitmap.put())) &&
        SUCCEEDED(m_wic->CreateFormatConverter(converter.put())) &&
        SUCCEEDED(converter->Initialize(
            wicBitmap.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        m_canvas.target->CreateBitmapFromWicBitmap(converter.get(), nullptr,
                                                   bitmap.put());
    }
    m_trayBitmaps.push_back({icon.key, icon.serial, bitmap});
    return bitmap.get();
}

float Island::PinAmount(const std::wstring& key) const {
    for (const auto& state : m_pinStates) {
        if (state.key == key) {
            return (float)std::clamp(state.amount, 0.0, 1.0);
        }
    }
    return IsTrayPinned(key) ? 1.0f : 0.0f;
}

bool Island::IsTrayPinned(const std::wstring& key) const {
    return std::find(m_trayPinned.begin(), m_trayPinned.end(), key) !=
           m_trayPinned.end();
}

void Island::SetTrayPinned(const std::wstring& key, bool pinned) {
    // It slides from where it is now.
    if (std::none_of(m_pinStates.begin(), m_pinStates.end(),
                     [&](const PinState& state) { return state.key == key; })) {
        m_pinStates.push_back({key, IsTrayPinned(key) ? 1.0 : 0.0, 0});
    }
    std::erase(m_trayPinned, key);
    if (pinned) {
        m_trayPinned.push_back(key);
    }
    StartAnimating();
    std::wstring value;
    for (const auto& one : m_trayPinned) {
        value += (value.empty() ? L"" : L"\n") + one;
    }
    Wh_SetStringValue(L"trayPinned", value.c_str());
    Render();
}

// The pin's circle, at the top right of a tray icon's slot.
float Island::PinShown(double now) const {
    if (m_pinShownKey.empty()) {
        return 0;
    }
    const double t = (now - m_pinShownAt) / kPinGrowSeconds;
    return t >= 1 ? 1.0f : (float)SpringOut(std::max(t, 0.0));
}

D2D1_RECT_F Island::TrayPinRect(const D2D1_RECT_F& slot) const {
    const float size = std::round(11 * m_scale);
    const float right = slot.right - std::round(1 * m_scale);
    const float top = slot.top + std::round(2 * m_scale);
    return {right - size, top, right, top + size};
}

// Passes a mouse message on to the app, as the taskbar does: newer apps
// (version 4) get the position and the message with the icon's ID; older ones
// the ID and the message.
void Island::SendToTrayIcon(const std::wstring& key, UINT message) {
    const TrayIcon* icon = FindTrayIcon(key);
    if (!icon || !icon->callback || !IsWindow(icon->owner)) {
        return;
    }
    // Its menu or window can come to the front.
    AllowSetForegroundWindow(icon->processId);
    POINT pt;
    GetCursorPos(&pt);
    if (icon->version >= 4) {
        PostMessage(icon->owner, icon->callback, MAKEWPARAM(pt.x, pt.y),
                    MAKELPARAM(message, icon->id));
    } else {
        PostMessage(icon->owner, icon->callback, icon->id, message);
    }
}

// The icon's name under it, or nothing.
void Island::ShowTrayTip(const std::wstring& key) {
    if (!m_tooltip || key == m_tooltipKey) {
        return;
    }
    m_tooltipKey = key;
    TOOLINFO tool{sizeof(tool)};
    tool.hwnd = m_hwnd;
    tool.uId = 1;
    const TrayIcon* icon = key.empty() ? nullptr : FindTrayIcon(key);
    if (!icon || icon->tip.empty()) {
        SendMessage(m_tooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&tool);
        return;
    }
    tool.lpszText = (LPWSTR)icon->tip.c_str();
    SendMessage(m_tooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&tool);
    for (const auto& slot : m_slots) {
        if (slot.item == IslandItem::TrayIcon && slot.text == key) {
            // Under the icon; beside it when upright (on the left edge; on
            // the right one, roughly a name's width to the left).
            const LONG gap = (LONG)std::round(6 * m_scale);
            const POINT at =
                !m_vertical  ? POINT{m_position.x + (LONG)slot.rect.left,
                                     m_position.y + (LONG)slot.rect.bottom + gap}
                : m_sideLeft ? POINT{m_position.x + (LONG)slot.rect.right + gap,
                                     m_position.y + (LONG)slot.rect.top}
                             : POINT{m_position.x + (LONG)slot.rect.left - gap -
                                         (LONG)std::round(160 * m_scale),
                                     m_position.y + (LONG)slot.rect.top};
            SendMessage(m_tooltip, TTM_TRACKPOSITION, 0, MAKELPARAM(at.x, at.y));
        }
    }
    SendMessage(m_tooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&tool);
}

// Mouse buttons: on a tray icon, passed on to its app (a double click opens
// it, the right button shows its menu, like on the taskbar); the middle button
// pins it. Elsewhere, a click (on release) is the island's own.
void Island::OnMouseButton(UINT msg, LPARAM lParam) {
    std::wstring key;
    const IslandItem item =
        HitTest({(short)LOWORD(lParam), (short)HIWORD(lParam)}, &key);
    if (item == IslandItem::TrayIcon) {
        const TrayIcon* icon = FindTrayIcon(key);
        const bool newer = icon && icon->version >= 4;
        ShowTrayTip({});
        switch (msg) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
                SendToTrayIcon(key, msg);
                break;
            case WM_LBUTTONUP:
                if (icon) {
                    WatchTrayAppWindows(*icon);
                }
                SendToTrayIcon(key, WM_LBUTTONUP);
                if (newer) {
                    SendToTrayIcon(key, NIN_SELECT);
                }
                break;
            case WM_RBUTTONUP:
                SendToTrayIcon(key, WM_RBUTTONUP);
                if (newer) {
                    SendToTrayIcon(key, WM_CONTEXTMENU);
                }
                break;
            case WM_MBUTTONUP:
                SetTrayPinned(key, !IsTrayPinned(key));
                break;
        }
        return;
    }
    const bool pressable = item == IslandItem::MediaPrevious ||
                           item == IslandItem::MediaPlayPause ||
                           item == IslandItem::MediaNext ||
                           item == IslandItem::Media;
    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) {
        if (pressable) {
            m_press.Press((int)item);
            StartAnimating();
        }
    }
    if (msg == WM_LBUTTONUP) {
        if (m_press.held) {
            m_press.Release();
            StartAnimating();
        }
        if (item == IslandItem::TrayPin) {
            SetTrayPinned(key, !IsTrayPinned(key));
            return;
        }
        OnClick(item, key);
    }
    // The arrow's right click keeps the list open, or not.
    if (msg == WM_RBUTTONUP && item == IslandItem::Overflow) {
        m_trayKeepOpen = !m_trayKeepOpen;
        g_settings.islandTrayOpen = m_trayKeepOpen;
        Wh_SetIntValue(L"trayKeepOpen", m_trayKeepOpen);
        m_trayOpen = m_trayKeepOpen;
        KillTimer(m_hwnd, kTrayCloseTimerId);
        StartAnimating();
    }
}

IslandItem Island::HitTest(POINT pt, std::wstring* appId) const {
    if (!m_bar && m_bannerAmount > 0.5 && PointInRect(pt, BannerRect())) {
        if (PointInRect(pt, BannerCloseRect())) {
            return IslandItem::BannerClose;
        }
        if (m_bannerOpen > 0.5 && m_banner.reply.CanReply()) {
            D2D1_RECT_F field, send, open;
            BannerParts(&field, &send, &open);
            if (PointInRect(pt, field)) {
                return IslandItem::BannerReply;
            }
            if (PointInRect(pt, open)) {
                return IslandItem::BannerOpen;
            }
        }
        return IslandItem::Banner;
    }
    if (!m_bar && m_minimized) {
        return IslandItem::Line;
    }
    for (const auto& slot : m_slots) {
        if (pt.x >= slot.rect.left && pt.x < slot.rect.right &&
            pt.y >= slot.rect.top && pt.y < slot.rect.bottom) {
            if (appId && (slot.item == IslandItem::App ||
                          slot.item == IslandItem::TrayIcon)) {
                *appId = slot.text;
            }
            if (slot.item == IslandItem::TrayIcon &&
                slot.text == m_pinShownKey &&
                PointInRect(pt, TrayPinRect(slot.rect))) {
                return IslandItem::TrayPin;
            }
            return slot.item;
        }
    }
    return IslandItem::None;
}

// Gives the panel the pill as it's drawn now (nothing on the bar).
void Island::CapturePill(Panel& panel) {
    const BYTE* bits = m_canvas.Bits();
    const SIZE size = m_canvas.Size();
    if (m_bar || !bits) {
        panel.SetPillSnapshot(nullptr, 0, {});
        return;
    }
    // Drawing may still be queued.
    GdiFlush();
    RECT rect{(LONG)std::floor(m_pill.left), (LONG)std::floor(m_pill.top),
              (LONG)std::ceil(m_pill.right), (LONG)std::ceil(m_pill.bottom)};
    rect.left = std::max(rect.left, 0L);
    rect.top = std::max(rect.top, 0L);
    rect.right = std::min(rect.right, size.cx);
    rect.bottom = std::min(rect.bottom, size.cy);
    panel.SetPillSnapshot(bits, size.cx * 4, rect);
}

void CapturePillFor(Panel& panel) {
    if (g_island) {
        g_island->CapturePill(panel);
    }
}

// Opens a panel out of the island, or closes it if it's open. Only one panel
// is open at a time. The pill itself becomes the panel; on the bar, the panel
// drops out of the clicked item.
void Island::TogglePanel(Panel& panel, IslandItem item) {
    EndBannerTyping();
    if (m_bannerWanted || m_bannerAmount > 0) {
        HideBanner();
        m_bannerAmount = 0;
        m_bannerVelocity = 0;
        m_banner = {};
        Render(true);
    }
    for (Panel* other : {(Panel*)&m_calendar, (Panel*)&m_control,
                         (Panel*)&m_notifications, (Panel*)&m_mediaPanel}) {
        if (other != &panel) {
            other->Close();
        }
    }
    if (panel.IsOpen()) {
        panel.Close();
    } else if (!m_bar) {
        // From the pill's top (growing down), its bottom (growing up, on the
        // lower half of the screen), or its outer edge (growing towards the
        // middle, upright on a side).
        const POINT anchor =
            m_vertical
                ? POINT{m_position.x +
                            (LONG)(m_sideLeft ? m_pill.left : m_pill.right),
                        m_position.y + (LONG)((m_pill.top + m_pill.bottom) / 2)}
                : POINT{m_position.x + (LONG)((m_pill.left + m_pill.right) / 2),
                        m_position.y +
                            (LONG)(AtBottom() ? m_pill.bottom : m_pill.top)};
        panel.SetDirection(m_vertical ? (m_sideLeft ? Panel::Direction::Right
                                                    : Panel::Direction::Left)
                           : AtBottom() ? Panel::Direction::Up
                                        : Panel::Direction::Down);
        CapturePill(panel);
        panel.Open(anchor, m_dpiScale,
                   {m_position.x + m_pill.left, m_position.y + m_pill.top,
                    m_position.x + m_pill.right, m_position.y + m_pill.bottom},
                   true);
    } else {
        // Out of the item; out of the clock if the item isn't shown.
        const IslandSlot* from = nullptr;
        for (const auto& slot : m_slots) {
            if (slot.item == item ||
                (!from && slot.item == IslandItem::Clock)) {
                from = &slot;
            }
        }
        if (from) {
            const POINT anchor{
                m_position.x + (LONG)((from->rect.left + from->rect.right) / 2),
                m_position.y + (LONG)(m_pill.bottom + kPanelGap * m_scale)};
            panel.SetPillSnapshot(nullptr, 0, {});
            panel.SetDirection(Panel::Direction::Down);
            panel.Open(anchor, m_dpiScale,
                       {m_position.x + from->rect.left,
                        m_position.y + from->rect.top,
                        m_position.x + from->rect.right,
                        m_position.y + from->rect.bottom},
                       false);
        }
    }
    StartAnimating();
}

void Island::OnClick(IslandItem item, const std::wstring& appId) {
    switch (item) {
        case IslandItem::App:
            OpenApp(appId);
            break;
        case IslandItem::Banner:
        case IslandItem::BannerOpen:
            if (!m_banner.appId.empty()) {
                OpenNotification(m_banner.appId, m_banner.reply);
                EndBannerTyping();
                HideBanner();
            } else {
                // A reminder: the calendar, on its day.
                m_calendar.SelectDay(m_reminderDay.year, m_reminderDay.month,
                                     m_reminderDay.day);
                HideBanner();
                TogglePanel(m_calendar, IslandItem::Clock);
            }
            break;
        case IslandItem::BannerClose:
            EndBannerTyping();
            HideBanner();
            break;
        case IslandItem::BannerReply: {
            // In the box: typing; on the send button: sending.
            D2D1_RECT_F field, send, open;
            BannerParts(&field, &send, &open);
            POINT pt;
            GetCursorPos(&pt);
            pt.x -= m_position.x;
            pt.y -= m_position.y;
            if (m_bannerTyping && !m_bannerReplyText.empty() &&
                PointInRect(pt, send)) {
                SendBannerReply();
            } else {
                StartBannerTyping();
            }
            break;
        }
        case IslandItem::Overflow:
            // The other icons open sideways (and stay, if kept open).
            m_trayOpen = !m_trayOpen;
            if (!m_trayOpen) {
                m_trayKeepOpen = false;
                Wh_SetIntValue(L"trayKeepOpen", 0);
            }
            StartAnimating();
            break;
        case IslandItem::TrayIcon:
        case IslandItem::TrayPin:
            break;
        case IslandItem::Network:
        case IslandItem::Volume:
        case IslandItem::Battery:
            TogglePanel(m_control, item);
            break;
        case IslandItem::Clock:
            TogglePanel(m_calendar, item);
            break;
        case IslandItem::Media:
            m_mediaPanel.SetState(m_mediaState.present ? m_mediaState
                                                       : m_mediaShown);
            TogglePanel(m_mediaPanel, item);
            break;
        case IslandItem::MediaPrevious:
            SendMediaCommand(MediaCommand::Previous);
            break;
        case IslandItem::MediaPlayPause:
            SendMediaCommand(MediaCommand::PlayPause);
            break;
        case IslandItem::MediaNext:
            SendMediaCommand(MediaCommand::Next);
            break;
        case IslandItem::Bell:
            TogglePanel(m_notifications, item);
            break;
        case IslandItem::Minimize:
            SetMinimized(true);
            break;
        case IslandItem::Line:
            SetMinimized(false);
            break;
        case IslandItem::TraySeparator:
        case IslandItem::None:
            break;
    }
}

LRESULT Island::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam) {
    // Explorer restarted (or asked the apps for their tray icons): it's told
    // the island is here.
    static const UINT taskbarCreated = RegisterWindowMessage(L"TaskbarCreated");
    if (msg == taskbarCreated && taskbarCreated) {
        SayHello();
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wParam == kIslandTimerId) {
                Tick();
                ScheduleTick();
                return 0;
            }
            if (wParam == kTrayAppTimerId) {
                StopWatchingTrayAppWindows();
                return 0;
            }
            if (wParam == kMediaTimerId) {
                KillTimer(m_hwnd, kMediaTimerId);
                UpdateMediaShown();
                return 0;
            }
            if (wParam == kBannerRestoreTimerId) {
                KillTimer(m_hwnd, kBannerRestoreTimerId);
                RestoreWindowsBanner();
                return 0;
            }
            if (wParam == kPinTimerId) {
                KillTimer(m_hwnd, kPinTimerId);
                if (m_hover == IslandItem::TrayIcon && !m_hoverApp.empty()) {
                    m_pinShownKey = m_hoverApp;
                    m_pinShownAt = NowSeconds();
                    StartAnimating();
                }
                return 0;
            }
            if (wParam == kTipTimerId) {
                KillTimer(m_hwnd, kTipTimerId);
                if (!m_pendingTipKey.empty() &&
                    m_hover == IslandItem::TrayIcon &&
                    m_hoverApp == m_pendingTipKey) {
                    ShowTrayTip(m_pendingTipKey);
                }
                m_pendingTipKey.clear();
                return 0;
            }
            if (wParam == kTrayCloseTimerId) {
                KillTimer(m_hwnd, kTrayCloseTimerId);
                if (m_trayOpen) {
                    m_trayOpen = false;
                    StartAnimating();
                }
                return 0;
            }
            if (wParam == kBannerTimerId) {
                // Stays while the mouse is on it, or while typing.
                if (m_bannerTyping || BannerHovered()) {
                    return 0;
                }
                HideBanner();
                return 0;
            }
            break;

        case WM_APP_BADGES:
            UpdateApps();
            Render();
            return 0;

        // From explorer: the apps with a badge, and the tray icons.
        case WM_COPYDATA: {
            const auto* copyData = (const COPYDATASTRUCT*)lParam;
            if (!copyData || !IsIpcSender((HWND)wParam, L"Shell_TrayWnd")) {
                break;
            }
            // Taken now (the icons are copied while explorer waits), shown
            // a moment later: this may arrive while the island is itself
            // sending explorer something.
            if (copyData->dwData == kIpcBadgeApps) {
                std::vector<std::wstring> apps = SplitLines(copyData);
                AcquireSRWLockExclusive(&g_badgeAppsLock);
                g_badgeApps = std::move(apps);
                ReleaseSRWLockExclusive(&g_badgeAppsLock);
                PostMessage(hWnd, WM_APP_BADGES, 0, 0);
                return TRUE;
            }
            if (copyData->dwData == kIpcTrayIcons) {
                ReceiveTrayIcons(copyData);
                PostMessage(hWnd, WM_APP_TRAY, 0, 0);
                return TRUE;
            }
            break;
        }

        case WM_APP_STATUS:
            switch (wParam) {
                case kStatusAudioDevice:
                    WatchVolume();
                    [[fallthrough]];
                case kStatusVolume:
                    OnVolumeChanged();
                    m_control.Refresh();
                    break;
                case kStatusNetwork:
                    OnNetworkChanged();
                    m_control.Refresh();
                    break;
                case kStatusBattery:
                    m_batteryGlyph = BatteryGlyph();
                    Render();
                    m_control.Refresh();
                    break;
                case kStatusDoNotDisturb:
                    OnDoNotDisturbChanged();
                    m_control.Refresh();
                    m_notifications.Refresh();
                    break;
            }
            return 0;

        case WM_POWERBROADCAST:
            if (wParam == PBT_POWERSETTINGCHANGE) {
                PostMessage(hWnd, WM_APP_STATUS, kStatusBattery, 0);
                return TRUE;
            }
            break;

        case WM_APP_TOAST:
            ShowBanner();
            return 0;

        case WM_APP_MEDIA:
            AcquireSRWLockShared(&g_mediaLock);
            m_mediaState = g_media;
            ReleaseSRWLockShared(&g_mediaLock);
            m_mediaPanel.SetState(m_mediaState);
            UpdateMediaShown();
            Render();
            return 0;

        case WM_APP_APPBAR:
            if (wParam == ABN_POSCHANGED) {
                SetAppBarPosition();
            }
            return 0;

        case WM_MOUSEACTIVATE:
            return m_bannerTyping ? MA_ACTIVATE : MA_NOACTIVATE;

        case WM_ACTIVATE:
            // Clicked somewhere else while typing a reply.
            if (LOWORD(wParam) == WA_INACTIVE && m_bannerTyping) {
                EndBannerTyping();
                HideBanner();
            }
            return 0;

        case WM_CHAR:
            if (OnBannerChar((WCHAR)wParam)) {
                return 0;
            }
            break;

        case WM_MOUSEMOVE: {
            if (!m_tracking) {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hWnd, 0};
                m_tracking = TrackMouseEvent(&track);
            }
            // Dragging the pill: it follows the mouse once it has moved a
            // little.
            if (m_dragArmed && (wParam & MK_LBUTTON)) {
                POINT cursor;
                GetCursorPos(&cursor);
                const int threshold = (int)std::round(6 * m_scale);
                if (!m_dragMoved &&
                    (std::abs(cursor.x - m_dragFrom.x) > threshold ||
                     std::abs(cursor.y - m_dragFrom.y) > threshold)) {
                    m_dragMoved = true;
                    m_calendar.Close();
                    m_control.Close();
                    m_notifications.Close();
                    m_mediaPanel.Close();
                    ShowTrayTip({});
                    m_press.Release();
                    m_hover = IslandItem::None;
                    m_hoverApp.clear();
                }
                if (m_dragMoved) {
                    if (m_roomWidth > 0) {
                        m_placeX = std::clamp(
                            (cursor.x - m_dragGrab.x - m_roomLeft) / m_roomWidth,
                            0.0f, 1.0f);
                    }
                    if (m_roomHeight > 0) {
                        m_placeY = std::clamp(
                            (cursor.y - m_dragGrab.y - m_roomTop) / m_roomHeight,
                            0.0f, 1.0f);
                    }
                    m_placeVelocityX = m_placeVelocityY = 0;
                    Render();
                    return 0;
                }
            }
            std::wstring hoverApp;
            const IslandItem hover = HitTest(
                {(short)LOWORD(lParam), (short)HIWORD(lParam)}, &hoverApp);
            if (hover != m_hover || hoverApp != m_hoverApp) {
                m_hover = hover;
                m_hoverApp = hoverApp;
                // Another icon (or none): its pin waits.
                const bool onTrayIcon = hover == IslandItem::TrayIcon ||
                                        hover == IslandItem::TrayPin;
                if (!onTrayIcon || hoverApp != m_pinShownKey) {
                    m_pinShownKey.clear();
                    KillTimer(m_hwnd, kPinTimerId);
                    if (onTrayIcon) {
                        SetTimer(m_hwnd, kPinTimerId, kPinDelayMs, nullptr);
                    }
                }
                // The name shows after a moment, or right away when moving
                // from one icon to the next.
                const std::wstring tipKey =
                    hover == IslandItem::TrayIcon ? hoverApp : std::wstring{};
                KillTimer(m_hwnd, kTipTimerId);
                if (tipKey.empty() || !m_tooltipKey.empty()) {
                    m_pendingTipKey.clear();
                    ShowTrayTip(tipKey);
                } else {
                    m_pendingTipKey = tipKey;
                    SetTimer(m_hwnd, kTipTimerId, GetDoubleClickTime(),
                             nullptr);
                }
                Render();
            }
            KillTimer(m_hwnd, kTrayCloseTimerId);
            // The line and the banner's reply row follow the mouse.
            if (m_bannerWanted || m_minimized) {
                StartAnimating();
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            m_tracking = false;
            // The banner goes a moment after the mouse leaves it, and the
            // tray icons' list a bit later.
            if (m_bannerWanted && !m_bannerTyping && !m_banner.sticky) {
                SetTimer(m_hwnd, kBannerTimerId, 2000, nullptr);
                StartAnimating();
            }
            if (m_trayOpen && !m_trayKeepOpen) {
                SetTimer(m_hwnd, kTrayCloseTimerId, 4000, nullptr);
            }
            KillTimer(m_hwnd, kTipTimerId);
            m_pendingTipKey.clear();
            ShowTrayTip({});
            if (m_press.held) {
                m_press.Release();
                StartAnimating();
            }
            if (m_hover != IslandItem::None) {
                m_hover = IslandItem::None;
                m_hoverApp.clear();
                StartAnimating();
                Render();
            }
            return 0;

        case WM_LBUTTONDOWN: {
            // The pill can be dragged from anywhere but the tray icons (their
            // apps get the click) and the line; a click still clicks.
            const IslandItem item =
                HitTest({(short)LOWORD(lParam), (short)HIWORD(lParam)});
            if (g_settings.islandFreeDrag && !m_bar && !m_bannerTyping &&
                m_bannerAmount <= 0.01 && !m_minimized &&
                item != IslandItem::TrayIcon &&
                item != IslandItem::TrayPin) {
                m_dragArmed = true;
                m_dragMoved = false;
                GetCursorPos(&m_dragFrom);
                m_dragGrab = {
                    m_dragFrom.x - (m_position.x + (LONG)std::lround(m_pill.left)),
                    m_dragFrom.y - (m_position.y + (LONG)std::lround(m_pill.top))};
                SetCapture(m_hwnd);
            }
            OnMouseButton(msg, lParam);
            return 0;
        }

        case WM_LBUTTONUP:
            if (m_dragArmed) {
                m_dragArmed = false;
                const bool moved = m_dragMoved;
                if (GetCapture() == m_hwnd) {
                    ReleaseCapture();
                }
                if (moved) {
                    m_dragMoved = false;
                    FinishDrag();
                    return 0;
                }
            }
            OnMouseButton(msg, lParam);
            return 0;

        case WM_CAPTURECHANGED:
            if (m_dragArmed && (HWND)lParam != m_hwnd) {
                m_dragArmed = false;
                if (m_dragMoved) {
                    m_dragMoved = false;
                    FinishDrag();
                }
            }
            break;

        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            OnMouseButton(msg, lParam);
            return 0;

        case WM_APP_TRAY:
            UpdateTrayIcons();
            Render();
            return 0;

        case WM_MOUSEWHEEL:
            // The wheel on the volume changes it, 2% a notch.
            if (m_hover == IslandItem::Volume || m_hover == IslandItem::Media ||
                m_hover == IslandItem::MediaPrevious ||
                m_hover == IslandItem::MediaPlayPause ||
                m_hover == IslandItem::MediaNext) {
                if (auto volume = GetVolume(nullptr)) {
                    float level = 0;
                    volume->GetMasterVolumeLevelScalar(&level);
                    const float step =
                        0.02f * GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
                    volume->SetMasterVolumeLevelScalar(
                        std::clamp(level + step, 0.0f, 1.0f), nullptr);
                    if (level + step > 0) {
                        volume->SetMute(FALSE, nullptr);
                    }
                }
                OnVolumeChanged();
                m_control.Refresh();
            }
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void StartIslandAnimation() {
    if (g_island) {
        g_island->StartAnimating();
    }
}

// Handles messages, and returns false on WM_QUIT.
bool PumpMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

DWORD WINAPI IslandThreadProc(LPVOID parameter) {
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent((HANDLE)parameter);

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT comResult =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    {
        Island island;
        g_island = &island;
        if (island.Create()) {
            island.Tick();
            // While animating, a frame per display refresh; otherwise, only
            // when there's something to do.
            for (;;) {
                if (island.Animating()) {
                    if (!PumpMessages()) {
                        break;
                    }
                    island.AnimationFrame();
                    DwmFlush();
                    continue;
                }
                if (GetMessage(&msg, nullptr, 0, 0) <= 0) {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        island.Destroy();
        g_island = nullptr;
    }

    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return 0;
}

void StartIslandThread() {
    if (g_islandThread || !g_settings.showIsland) {
        return;
    }
    HANDLE started = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!started) {
        return;
    }
    g_islandThread = CreateThread(nullptr, 0, IslandThreadProc, started, 0,
                                  &g_islandThreadId);
    if (g_islandThread) {
        WaitForSingleObject(started, INFINITE);
    } else {
        g_islandThreadId = 0;
    }
    CloseHandle(started);
}

void StopIslandThread() {
    if (!g_islandThread) {
        return;
    }
    PostThreadMessage(g_islandThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_islandThread, INFINITE);
    CloseHandle(g_islandThread);
    g_islandThread = nullptr;
    g_islandThreadId = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Auto-hide.

void SetAutoHide(bool on) {
    APPBARDATA data{sizeof(data)};
    const UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &data);
    const bool isOn = (state & ABS_AUTOHIDE) != 0;
    if (isOn == on) {
        return;
    }
    data.lParam = on ? (state | ABS_AUTOHIDE) : (state & ~ABS_AUTOHIDE);
    SHAppBarMessage(ABM_SETSTATE, &data);
}

// Auto-hide is a Windows setting, kept when Explorer stops: the mod
// remembers that it turned it on (also across a crash), so that it's turned
// back off when the mod is turned off or no longer needs it.
void UpdateAutoHide() {
    // A Windows setting: only from the explorer.exe with the taskbar.
    if (!OwnsTaskbar()) {
        return;
    }
    if (g_settings.autoHide && SlidesItself() && !g_unloading) {
        APPBARDATA data{sizeof(data)};
        if (!(SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE)) {
            SetAutoHide(true);
            g_turnedAutoHideOn = true;
            Wh_SetIntValue(L"turnedAutoHideOn", 1);
        }
    } else if (g_turnedAutoHideOn) {
        SetAutoHide(false);
        g_turnedAutoHideOn = false;
        Wh_SetIntValue(L"turnedAutoHideOn", 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifetime.

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CSecondaryTray::GetTrayWindow(void))"},
            &CSecondaryTray_GetTrayWindow_Original,
        },
        {
            {LR"(public: virtual void __cdecl CSecondaryTray::InitModelAndHost(struct winrt::WindowsUdk::UI::Shell::TaskbarModel))"},
            &CSecondaryTray_InitModelAndHost_Original,
            CSecondaryTray_InitModelAndHost_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListWnd::PermitAutoHide(void))"},
            &CTaskListWnd_PermitAutoHide_Original,
            CTaskListWnd_PermitAutoHide_Hook,
            // Without it, the taskbar just shows itself for attention as
            // usual.
            true,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}

void LoadSettings() {
    LoadTheme();
    LoadPlaces();
    const auto taskbarMode = WindhawkUtils::StringSetting::make(L"taskbarMode");
    g_settings.taskbarMode =
        wcscmp(taskbarMode, L"windows") == 0
            ? decltype(g_settings.taskbarMode)::Windows
        : wcscmp(taskbarMode, L"windowsApps") == 0
            ? decltype(g_settings.taskbarMode)::WindowsApps
        : wcscmp(taskbarMode, L"windowsTransparent") == 0
            ? decltype(g_settings.taskbarMode)::WindowsTransparent
            : decltype(g_settings.taskbarMode)::Dock;
    // The dock and the transparent taskbar are made for the bottom: with the
    // taskbar elsewhere, it stays as Windows has it (the island still works),
    // until it's back at the bottom (see WatchTaskbarPlace).
    g_taskbarAtBottom = IsTaskbarAtBottom();
    if (!g_taskbarAtBottom &&
        (g_settings.taskbarMode == decltype(g_settings.taskbarMode)::Dock ||
         g_settings.taskbarMode ==
             decltype(g_settings.taskbarMode)::WindowsTransparent)) {
        Wh_Log(L"The taskbar isn't at the bottom: leaving it as it is");
        g_settings.taskbarMode = decltype(g_settings.taskbarMode)::Windows;
    }
    g_settings.autoHide = Wh_GetIntSetting(L"hiding.autoHide") != 0;
    g_settings.cornerRadius =
        std::clamp(Wh_GetIntSetting(L"dock.cornerRadius"), 0, 24);
    g_settings.dockHeight = std::clamp(Wh_GetIntSetting(L"dock.dockHeight"), 28, 64);
    g_settings.bottomGap = std::clamp(Wh_GetIntSetting(L"dock.bottomGap"), 0, 16);
    g_settings.backgroundOpacity =
        std::clamp(Wh_GetIntSetting(L"dock.backgroundOpacity"), 0, 100) / 100.0;
    g_settings.showDuration =
        std::clamp(Wh_GetIntSetting(L"hiding.showDuration"), 0, 1000);
    g_settings.hideDuration =
        std::clamp(Wh_GetIntSetting(L"hiding.hideDuration"), 0, 1000);
    g_settings.showDelay = std::clamp(Wh_GetIntSetting(L"hiding.showDelay"), 0, 2000);
    g_settings.hideDelay = std::clamp(Wh_GetIntSetting(L"hiding.hideDelay"), 0, 5000);
    g_settings.fullscreenGuard = Wh_GetIntSetting(L"hiding.fullscreenGuard") != 0;
    g_settings.hideTray = Wh_GetIntSetting(L"dock.hideTray") != 0;
    g_settings.showIsland = Wh_GetIntSetting(L"showIsland") != 0;
    // The island's own options, chosen in it (see kIslandPrefs).
    g_settings.islandBar = Wh_GetIntValue(L"islandBar", 0) != 0;
    g_settings.islandTray = Wh_GetIntValue(L"islandTray", 1) != 0;
    g_settings.islandTrayOpen = Wh_GetIntValue(L"trayKeepOpen", 0) != 0;
    g_settings.islandNetwork = Wh_GetIntValue(L"islandNetwork", 1) != 0;
    g_settings.islandVolume = Wh_GetIntValue(L"islandVolume", 1) != 0;
    g_settings.islandBattery = Wh_GetIntValue(L"islandBattery", 1) != 0;
    g_settings.islandBell = Wh_GetIntValue(L"islandBell", 1) != 0;
    g_settings.islandMedia = Wh_GetIntValue(L"islandMedia", 1) != 0;
    g_settings.islandMediaButtons = Wh_GetIntValue(L"mediaButtons", 0) != 0;
    g_settings.islandBanner = Wh_GetIntValue(L"bannerEnabled", 1) != 0;
    g_settings.islandBannerMinimized =
        Wh_GetIntValue(L"bannerMinimized", 0) != 0;
    g_settings.islandHideWindowsBanners =
        Wh_GetIntValue(L"hideWindowsBanners", 1) != 0;
    g_settings.islandWebPictures =
        Wh_GetIntValue(L"bannerWebPictures", 0) != 0;
    g_settings.clockSeconds = Wh_GetIntValue(L"clockSeconds", -1);
    g_settings.clockWeekday = Wh_GetIntValue(L"clockWeekday", 1) != 0;
    g_settings.clockDate = Wh_GetIntValue(L"clockDate", 1) != 0;
    g_settings.clockYear = Wh_GetIntValue(L"clockYear", 0) != 0;
    g_settings.islandFreeDrag = Wh_GetIntValue(L"islandFreeDrag", 0) != 0;
    g_settings.islandVerticalSides =
        Wh_GetIntValue(L"islandVerticalSides", 1) != 0;
    g_settings.islandSize = std::clamp(
        Wh_GetIntValue(L"islandSize", kIslandSizeDefault), kIslandSizeMin,
        kIslandSizeMax);
    g_settings.magnification =
        std::clamp(Wh_GetIntSetting(L"icons.magnification"), 100, 200) / 100.0;
    g_settings.magnificationRange =
        std::clamp(Wh_GetIntSetting(L"icons.magnificationRange"), 40, 300);
    g_settings.bounceOnLaunch = Wh_GetIntSetting(L"icons.bounceOnLaunch") != 0;
    g_settings.keepOpenOnLaunch = Wh_GetIntSetting(L"icons.keepOpenOnLaunch") != 0;
    g_settings.keepOpenDuration =
        std::clamp(Wh_GetIntSetting(L"icons.keepOpenDuration"), 0, 5000);
    g_settings.bounceOnAttention = Wh_GetIntSetting(L"icons.bounceOnAttention") != 0;
    g_settings.dotIndicator = Wh_GetIntSetting(L"dock.dotIndicator") != 0;
    // Options that only work with another one: as if off without it.
    if (!g_settings.autoHide) {
        g_settings.keepOpenOnLaunch = false;
    }
    const auto attentionMode =
        WindhawkUtils::StringSetting::make(L"dock.attentionMode");
    g_settings.attentionMode = wcscmp(attentionMode, L"dock") == 0
                                   ? AttentionMode::Dock
                               : wcscmp(attentionMode, L"none") == 0
                                   ? AttentionMode::None
                                   : AttentionMode::Icon;
}

////////////////////////////////////////////////////////////////////////////////
// The explorer.exe part: the dock, and what the taskbar knows for the island.

BOOL ExplorerModInit() {
    Wh_Log(L">");

    LoadSettings();
    // Turned on by the mod before Explorer last stopped (maybe a crash).
    g_turnedAutoHideOn = Wh_GetIntValue(L"turnedAutoHideOn", 0) != 0;

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(SetTimer, SetTimer_Hook,
                                   &SetTimer_Original);
    return TRUE;
}

void ExplorerModAfterInit() {
    Wh_Log(L">");

    g_shellHookMessage = RegisterWindowMessage(L"SHELLHOOK");
    StartAttentionThread();
    ApplySettings();
    SubclassTaskbars();
    UpdateAutoHide();
    StartEdgeWatch();
    RequestTrayIcons();
    StartLateStart();
}

void ExplorerModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
    StopWatchingTaskbarPlace();
    StopLateStart();
    StopEdgeWatch();
    StopAttentionThread();
    ApplySettings();
    UnsubclassTaskbars();
    FreeIslandAppIcons();
    UpdateAutoHide();
}

// The taskbar moved to another edge: once it has settled, the settings
// apply again (the dock goes away, or comes back at the bottom). On a thread
// of its own, since applying them waits for the taskbar's thread.
HANDLE g_taskbarPlaceThread;
HANDLE g_taskbarPlaceStop;

void ExplorerModSettingsChanged();

DWORD WINAPI TaskbarPlaceThreadProc(LPVOID) {
    if (WaitForSingleObject(g_taskbarPlaceStop, 800) == WAIT_TIMEOUT &&
        !g_unloading && IsTaskbarAtBottom() != g_taskbarAtBottom) {
        ExplorerModSettingsChanged();
    }
    return 0;
}

void StopWatchingTaskbarPlace() {
    if (g_taskbarPlaceThread) {
        SetEvent(g_taskbarPlaceStop);
        WaitForSingleObject(g_taskbarPlaceThread, INFINITE);
        CloseHandle(g_taskbarPlaceThread);
        g_taskbarPlaceThread = nullptr;
    }
    if (g_taskbarPlaceStop) {
        CloseHandle(g_taskbarPlaceStop);
        g_taskbarPlaceStop = nullptr;
    }
}

// On the taskbar's thread.
void WatchTaskbarPlace() {
    if (g_taskbarPlaceThread) {
        if (WaitForSingleObject(g_taskbarPlaceThread, 0) != WAIT_OBJECT_0) {
            return;
        }
        StopWatchingTaskbarPlace();
    }
    g_taskbarPlaceStop = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_taskbarPlaceStop) {
        return;
    }
    g_taskbarPlaceThread = CreateThread(nullptr, 0, TaskbarPlaceThreadProc,
                                        nullptr, 0, nullptr);
}

// Windhawk's settings change and the taskbar moving to another edge (see
// TaskbarPlaceThreadProc) apply them one at a time.
SRWLOCK g_explorerSettingsLock = SRWLOCK_INIT;

void ExplorerModSettingsChanged() {
    Wh_Log(L">");

    AcquireSRWLockExclusive(&g_explorerSettingsLock);

    StopLateStart();
    LoadSettings();
    ApplySettings();
    SubclassTaskbars();
    UpdateAutoHide();
    // The bottom edge is only watched for the dock.
    StopEdgeWatch();
    StartEdgeWatch();
    RequestTrayIcons();
    StartLateStart();
    ReleaseSRWLockExclusive(&g_explorerSettingsLock);
}

////////////////////////////////////////////////////////////////////////////////
// The island's process (windhawk.exe): the island, and the new notifications
// (for the island, and for the dock's icons to bounce).

// Keeps the multithreaded apartment alive while the island's process runs.
// Its threads (media, radios, notifications) come and go, and when the last
// one leaves it, COM unloads Windows' DLLs while C++/WinRT still has their
// factories cached: the next call (opening the control center after the
// island restarted) would run code that's gone.
// Looked up at run time (not in every import library).
CO_MTA_USAGE_COOKIE g_mtaUsage;

FARPROC GetComBaseProc(PCSTR name) {
    HMODULE combase = GetModuleHandle(L"combase.dll");
    return combase ? GetProcAddress(combase, name) : nullptr;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    using CoIncrementMTAUsage_t = HRESULT(WINAPI*)(CO_MTA_USAGE_COOKIE*);
    if (auto increment = (CoIncrementMTAUsage_t)GetComBaseProc(
            "CoIncrementMTAUsage");
        !increment || FAILED(increment(&g_mtaUsage))) {
        g_mtaUsage = nullptr;
    }
    LoadSettings();
    StartToastWatch();
    StartIslandThread();
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;
    WaitForIslandRestart();
    StopIslandThread();
    StopToastWatch();
    winrt::clear_factory_cache();
    using CoDecrementMTAUsage_t = HRESULT(WINAPI*)(CO_MTA_USAGE_COOKIE);
    if (auto decrement = (CoDecrementMTAUsage_t)GetComBaseProc(
            "CoDecrementMTAUsage");
        g_mtaUsage && decrement) {
        decrement(g_mtaUsage);
    }
    g_mtaUsage = nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    WaitForIslandRestart();
    AcquireSRWLockExclusive(&g_islandLifeLock);
    StopIslandThread();
    LoadSettings();
    StartIslandThread();
    ReleaseSRWLockExclusive(&g_islandLifeLock);
}

bool g_isExplorer;

bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    const DWORD length = GetModuleFileName(nullptr, path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) {
        return false;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
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
    // The mod's own module, for its window classes (both processes).
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&Wh_ModInit, &g_module);

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
    // explorer.exe part.
    if (g_isExplorer) {
        ExplorerModAfterInit();
        return;
    }

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

// explorer.exe part (not part of the tool mod snippet).
void Wh_ModBeforeUninit() {
    if (g_isExplorer) {
        ExplorerModBeforeUninit();
    }
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
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
