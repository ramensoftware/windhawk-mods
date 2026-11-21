// ==WindhawkMod==
// @id              nimbus-menu
// @name            Nimbus Menu
// @description     Replaces the Windows taskbar with a light dock and its own Start menu (also on the Windows key): smoked glass, macOS-like zoom, live window previews, one bar per monitor, file search with Everything
// @description:pt-BR Troca a barra de tarefas do Windows por uma dock leve com Iniciar próprio (também na tecla Windows): vidro fumê, zoom como no macOS, prévia das janelas ao vivo, uma barra por monitor, busca de arquivos com o Everything
// @version         2.0.0
// @author          igoredm
// @github          https://github.com/igoredm
// @homepage        https://github.com/igoredm/nimbus-menu
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lgdi32 -ld2d1 -ldwrite -lwindowscodecs -ldwmapi -lshell32 -lshlwapi -lshcore -lpropsys -ladvapi32 -lcomdlg32 -lwlanapi -lversion -lcomctl32 -lsetupapi -lruntimeobject -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Nimbus Menu

Replaces the Windows taskbar with a light dock, one per monitor, with its own Start menu.

![Dock and Start](https://raw.githubusercontent.com/igoredm/windhawk-mods/b8319aac0d112762c582a7a0b4778d6c1d72790a/start.png)

![Control center](https://raw.githubusercontent.com/igoredm/windhawk-mods/1635432cdfe302469b8963916e776999508ebee7/2.0-central.png)

![Player](https://raw.githubusercontent.com/igoredm/windhawk-mods/1635432cdfe302469b8963916e776999508ebee7/2.0-player.png)

![Start search, even with a typo](https://raw.githubusercontent.com/igoredm/windhawk-mods/1635432cdfe302469b8963916e776999508ebee7/2.0-busca.png)

![Nimbus notification](https://raw.githubusercontent.com/igoredm/windhawk-mods/1635432cdfe302469b8963916e776999508ebee7/2.0-aviso.png)

![Guided setup](https://raw.githubusercontent.com/igoredm/windhawk-mods/1635432cdfe302469b8963916e776999508ebee7/2.0-guiado.png)

* Floating dock or full-width bar; 4 looks (Névoa, Cristal, Grafite, Aurora).
* macOS-like zoom (or Lift / Focus), live window previews (hover a thumbnail to peek), drag icons to reorder, pin and unpin.
* Its own Start: pinned apps, all apps, recent downloads, app search, files (with [Everything](https://www.voidtools.com) or Windows Search) and Windows Settings pages. Compact or wide search.
* A search that understands you: no accents needed, initials ("vsc" finds Visual Studio Code), one typo ("spotfy"), common nicknames ("cmd", "calc"), and what you open most comes first.
* Keyboard in Start: arrows, Tab and Enter in every tab. Power: restart and shut down ask for a second click; right-click a power button for sign out, hibernate and restart to advanced options.
* Right-click menus like Windows': open file location, copy path, and End task on the dock (when it's on in Windows, or always).
* The Windows key opens Nimbus' Start over any window (administrator ones too) and in games. Ctrl+Esc still opens Windows' Start. An optional shortcut (Alt+Space or Ctrl+Space) can open it too.
* Status area at either end of the dock (or as a Start tab): clock and calendar, volume per app, network and Wi-Fi, battery, keyboard language. Each opens its own panel, or Windows' own if you prefer.
* Tray: the icons of the apps running in the background, beside the status area. Inline with arrows, behind one button (pin the ones you want outside), or in a grid or list panel. Clicks go to the app itself, like on the taskbar; the middle button pins.
* Control center: one dock button opens do not disturb, Bluetooth, night light, shortcuts (project, cast, captions, accessibility), each monitor's brightness, sound and power, and any other item you put there. Each item opens its full panel inside it, and it scrolls on small screens. Every item can be in the dock, the control center and Start at once, or off.
* Bluetooth: turn it on and off, see paired devices with their battery, connect and disconnect headsets and speakers.
* Notifications: Windows' notifications also show as Nimbus cards (in the corner you choose, paused while the mouse is on them, never over games), with their buttons; a bell with the count opens the list, newest first. Win+N can open it. Its own sounds (14 to pick, or Windows' own), one for notifications and one for reminders.
* Player: what any app is playing (Spotify, YouTube…): cover, title, time, controls, shuffle and repeat, that app's volume. In the dock as the cover and title, the cover, the title, bars moving with the sound, or an icon.
* Devices: the battery of wireless headsets, mice and Xbox controllers, with a warning at 20% and 10%. USB receivers too (Logitech, Razer…) with [BatteryHub](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor), a separate free program, if you turn it on (see below).
* Notes and reminders: notes stay until you delete them; a reminder (in 1, 7 or 14 days, a month, or a calendar day, at a time or as soon as Nimbus runs) comes as a Nimbus notification, late too if the PC was off.
* Shelf: drop files on the dock to keep them for a moment, then drag them anywhere. Ctrl+V turns a copied picture or text into a file you can drag.
* Its own settings window, with a live dock preview: right-click the dock → Settings, or the Nimbus button at the bottom left of Start. Windhawk's Settings tab is empty on purpose: everything is in that window (what was set there in 1.x carries over).
* Guided setup: a few questions set everything up (it opens by itself the first time; Settings → Get started brings it back). Everything can be changed later in Settings.
* Follows Windows' high contrast and "Animation effects" off.
* Per-monitor settings: format, auto-hide, what shows, only that monitor's windows. Each monitor keeps its own, even when the monitors change order.
* The dock runs in its own Windhawk process and hides when a game or video is in full screen. Explorer only gets three small hooks: the Windows key, Win+N, and passing the tray icons on to the dock (it never waits on the dock).
* The text is in English, or in Portuguese when Windows is in Portuguese.

### How to use

**Dock**
* Click an app to open it or bring it to the front; middle-click opens a new window.
* Drag an icon to reorder. Drag an open app to the pinned ones to pin it; drag a pinned one up out of the dock to unpin it.
* Right-click an app for its menu, or an empty spot for the dock's menu (settings are there).

**Status area**
* Each item, and the tray, can go to the dock, the control center, a "Status" tab in Start (one, several or all of them), or be turned off (Settings → Status area).
* Start's Status tab looks like the control center; its parts can be put in any order ("Organize": drag them, or Alt + arrows).
* Click an item for its panel. Scroll on the volume to change it; middle-click mutes.
* The keyboard item: a click switches the language, right-click lists them all.

**Tray**
* *Button* (default): one button holds the icons; click it to show or hide them. Middle-click an icon to pin it: pinned icons stay outside the button, always visible. Middle-click again to put it back.
* *Inline*: every icon is on the dock; pinned ones come first, with a dot.
* *Grid* / *List*: the button opens a panel with all the icons; middle-click pins one to the front.
* In Start, the icons show in their own part of the Status tab; clicks and the middle button work the same.
* Left and right clicks go to the app, like on the Windows taskbar (the menu you see is the app's own). Scroll with the arrows or the mouse wheel. "Icons shown" sets how many fit at once.
* Drag an icon onto another to change the order (in the dock, the panel or the control center).

**Shelf**
* Drag files over the dock: the shelf opens; drop them on it (it keeps only where they are, nothing is copied).
* Drag an item from the shelf to a folder or an app: it's copied (or moved, see Settings → Status area → Shelf). With the right button, Windows asks.
* Ctrl+V (or "Paste") with the shelf open: a copied picture becomes a .png, text becomes a .txt, copied files come in as they are. Those pasted files move when dragged out, and are deleted when the mod stops if still on the shelf.
* Right-click an item: Open, Show in folder, Copy, Remove.

**Help and suggestions:** [github.com/igoredm/nimbus-menu](https://github.com/igoredm/nimbus-menu) (ask for help, report a bug or suggest an idea).

### What it changes in Windows

Nimbus replaces part of Windows, so a few of its actions are Windows settings. Here is each one, when it happens, and how it comes back:

| What | When | How it comes back |
|---|---|---|
| The taskbar's own **"Automatically hide the taskbar"** option (saved by Windows in your profile) | While "Hide the Windows taskbar" is on (the default), so the hidden taskbar doesn't reserve screen space | Your original choice is kept and put back when the mod stops, on sign-out or shutdown, at the next start after a crash, and by the Explorer part when the mod is turned off. |
| Each app's **"Show notification banners"** switch (Settings › System › Notifications) | Only if you turn on "Hide Windows' own banners" and confirm (off by default) | Every app's original value is saved before anything is switched off (nothing is switched off if that backup can't be saved). Put back when the option goes off, when the mod stops, on sign-out or shutdown, and by the Explorer part when the mod is turned off. |
| Night light, power plan and mode, each monitor's brightness (DDC/CI), Bluetooth and Wi-Fi on or off, volume, keyboard language | Only when you click them, like in Windows' quick settings | They're your choices, so they stay as you set them (the same as when you change them in Windows). |

**BatteryHub** (optional, off by default): with "Devices › Use BatteryHub" on, Nimbus runs [BatteryHub](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor) (installed separately, version 1.1 or newer) hidden every 5 minutes with `--json`, only to read the batteries of USB receivers; it closes by itself, at most after 20 seconds. Turn it off in the Devices panel or in Settings › Status area.

Nimbus' own data (settings, notes, pins, the shelf's pasted files) stays in the mod's storage. If the Windows taskbar ever stays hidden, turn the mod off or restart Explorer.

---

**Português**

Troca a barra de tarefas do Windows por uma dock leve, uma por monitor, com Iniciar próprio.

* Dock flutuante ou barra fixa; 4 visuais (Névoa, Cristal, Grafite, Aurora).
* Zoom como no macOS (ou Elevar / Foco), prévia das janelas ao vivo (parar o mouse na miniatura mostra a janela), arrastar ícones pra reorganizar, fixar e desafixar.
* Iniciar próprio: fixados, todos os apps, baixados recentes, busca de apps, arquivos (com o [Everything](https://www.voidtools.com) ou a busca do Windows) e telas das Configurações do Windows. Busca compacta ou larga.
* Uma busca que entende: sem precisar de acento, por iniciais ("vsc" acha o Visual Studio Code), com uma letra errada ("spotfy"), apelidos comuns ("cmd", "calc", "zap"), e o que você mais abre vem primeiro.
* Teclado no Iniciar: setas, Tab e Enter em todas as abas. Energia: reiniciar e desligar pedem um segundo clique; o botão direito num botão de energia tem sair da conta, hibernar e reiniciar nas opções avançadas.
* Menus do botão direito como os do Windows: abrir o local do arquivo, copiar caminho e Finalizar tarefa na dock (quando está ligado no Windows, ou sempre).
* A tecla Windows abre o Iniciar do Nimbus em qualquer janela (até de administrador) e em jogos. Ctrl+Esc continua abrindo o Iniciar do Windows. Um atalho opcional (Alt+Espaço ou Ctrl+Espaço) também pode abrir.
* Área de status numa ponta da dock (ou numa aba do Iniciar): relógio e calendário, volume por app, rede e Wi-Fi, bateria, idioma do teclado. Cada item abre o seu painel, ou o do Windows se preferir.
* Bandeja: os ícones dos programas em segundo plano, ao lado da área de status. Lado a lado com setas, atrás de um botão (fixe fora os que quiser), ou num painel em grade ou lista. Os cliques vão pro próprio programa, como na barra; o botão do meio fixa.
* Central de controle: um botão na dock abre não incomodar, Bluetooth, luz noturna, atalhos (projetar, transmitir, legendas, acessibilidade), o brilho de cada monitor, som e energia, e qualquer outro item que você puser lá. Cada item abre o painel completo dentro dela, e ela rola em telas pequenas. Cada item pode ficar na dock, na central e no Iniciar ao mesmo tempo, ou desligado.
* Bluetooth: ligar e desligar, ver os aparelhos pareados com a bateria, conectar e desconectar fones e caixas de som.
* Notificações: os avisos do Windows também aparecem como cartões do Nimbus (no canto escolhido, pausam com o mouse em cima, nunca sobre jogos), com os botões deles; um sino com o contador abre a lista, da mais nova pra mais antiga. O Win+N pode abrir. Sons próprios (14 pra escolher, ou o do Windows), um pros avisos e outro pros lembretes.
* Player: o que qualquer app está tocando (Spotify, YouTube…): capa, título, tempo, controles, aleatório e repetir, o volume daquele app. Na dock como capa e título, só a capa, só o título, barras que se mexem com o som ou um ícone.
* Dispositivos: a bateria de fones, mouses e controles de Xbox sem fio, com aviso em 20% e 10%. Receptores USB também (Logitech, Razer…) com o [BatteryHub](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor), um programa à parte e gratuito, se você ligar (veja abaixo).
* Notas e lembretes: as notas ficam até você apagar; um lembrete (em 1, 7 ou 14 dias, 1 mês ou num dia do calendário, num horário ou assim que o Nimbus abrir) chega como notificação do Nimbus, atrasado também se o PC estava desligado.
* Prateleira: solte arquivos na dock pra guardar por um tempo, depois arraste pra onde quiser. O Ctrl+V transforma uma imagem ou texto copiado num arquivo pra arrastar.
* Janela de ajustes própria, com prévia da dock ao vivo: botão direito na dock → Ajustes, ou o botão do Nimbus no canto de baixo do Iniciar. A aba Configurações do Windhawk fica vazia de propósito: tudo está nessa janela (o que foi escolhido lá na 1.x é mantido).
* Modo guiado: umas perguntas rápidas deixam tudo pronto (abre sozinho na primeira vez; Ajustes → Começar traz de volta). Tudo pode ser mudado depois nos Ajustes.
* Segue o alto contraste do Windows e os "Efeitos de animação" desligados.
* Ajustes por monitor: formato, esconder sozinha, o que aparece, só as janelas daquele monitor. Cada monitor guarda os seus, mesmo se a ordem dos monitores mudar.
* A dock roda num processo próprio do Windhawk e some quando um jogo ou vídeo está em tela cheia. No Explorador entram só três ganchos pequenos: o da tecla Windows, o do Win+N e o que passa os ícones da bandeja pra dock (ele nunca espera a dock).
* Os textos ficam em inglês, ou em português quando o Windows está em português.

### Como usar

**Dock**
* Clique num app pra abrir ou trazer pra frente; o botão do meio abre uma janela nova.
* Arraste um ícone pra mudar a ordem. Arraste um app aberto pros fixados pra fixar; arraste um fixado pra cima, pra fora da dock, pra desafixar.
* Botão direito num app abre o menu dele; num espaço vazio, o menu da dock (os ajustes ficam lá).

**Área de status**
* Cada item, e a bandeja, pode ficar na dock, na central de controle, numa aba "Status" do Iniciar (num, em vários ou em todos), ou desligado (Ajustes → Área de status).
* Clique num item pra abrir o painel. Role o mouse no volume pra mudar; o botão do meio deixa mudo.
* Teclado: um clique troca o idioma; o botão direito mostra a lista.

**Bandeja**
* *Botão* (o padrão): um botão guarda os ícones; clique nele pra mostrar ou esconder. Botão do meio num ícone fixa ele: os fixados ficam fora do botão, sempre à mostra. Botão do meio de novo devolve pra dentro.
* *Lado a lado*: todos os ícones ficam na dock; os fixados vêm primeiro, com um pontinho.
* *Grade* / *Lista*: o botão abre um painel com todos os ícones; o botão do meio fixa no começo.
* No Iniciar, os ícones aparecem embaixo dos blocos de status; os cliques e o botão do meio funcionam igual.
* Os cliques esquerdo e direito vão pro programa, como na barra do Windows (o menu que aparece é o do próprio programa). Role com as setas ou a roda do mouse. "Ícones à mostra" define quantos cabem de uma vez.
* Arraste um ícone por cima de outro pra mudar a ordem (na dock, no painel ou na central).

**Prateleira**
* Arraste arquivos por cima da dock: a prateleira abre; solte nela (ela guarda só onde estão, nada é copiado).
* Arraste um item da prateleira pra uma pasta ou um app: ele é copiado (ou movido, veja Ajustes → Área de status → Prateleira). Com o botão direito, o Windows pergunta.
* Ctrl+V (ou "Colar") com a prateleira aberta: uma imagem copiada vira .png, um texto vira .txt, arquivos copiados entram como estão. Esses arquivos colados são movidos ao arrastar, e apagados quando o mod para se ainda estiverem na prateleira.
* Botão direito num item: Abrir, Mostrar na pasta, Copiar, Remover.

**Ajuda e sugestões:** [github.com/igoredm/nimbus-menu](https://github.com/igoredm/nimbus-menu) (peça ajuda, avise de um bug ou mande uma ideia).

### O que muda no Windows

O Nimbus substitui uma parte do Windows, então algumas ações dele são configurações do Windows. Aqui está cada uma, quando acontece e como volta:

| O quê | Quando | Como volta |
|---|---|---|
| A opção **"Ocultar automaticamente a barra de tarefas"** do próprio Windows (salva no seu perfil) | Enquanto "Esconder a barra do Windows" estiver ligado (o padrão), pra barra escondida não reservar espaço na tela | A sua escolha original fica guardada e volta quando o mod para, ao sair da conta ou desligar o PC, no próximo início depois de um travamento, e pela parte do Explorador quando o mod é desligado. |
| O botão **"Mostrar faixas de notificação"** de cada app (Configurações › Sistema › Notificações) | Só se você ligar "Esconder os avisos do Windows" e confirmar (desligado por padrão) | O valor original de cada app é salvo antes de qualquer coisa ser desligada (se o backup não puder ser salvo, nada é desligado). Volta quando a opção é desligada, quando o mod para, ao sair da conta ou desligar o PC, e pela parte do Explorador quando o mod é desligado. |
| Luz noturna, plano e modo de energia, brilho de cada monitor (DDC/CI), Bluetooth e Wi-Fi ligado ou desligado, volume, idioma do teclado | Só quando você clica, como nas Configurações rápidas do Windows | São escolhas suas, então ficam como você deixou (igual a mudar pelo Windows). |

**BatteryHub** (opcional, desligado por padrão): com "Dispositivos › Usar o BatteryHub" ligado, o Nimbus abre o [BatteryHub](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor) (instalado à parte, versão 1.1 ou mais nova) escondido a cada 5 minutos com `--json`, só pra ler as baterias de receptores USB; ele fecha sozinho, no máximo em 20 segundos. Desligue no painel Dispositivos ou em Ajustes › Área de status.

Os dados do próprio Nimbus (ajustes, notas, fixados, os arquivos colados na prateleira) ficam no armazenamento do mod. Se a barra do Windows ficar escondida, desligue o mod ou reinicie o Explorador.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <mmsystem.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <propsys.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <shellscalingapi.h>
#include <wrl/client.h>
#include <sddl.h>
#include <chrono>
#include <tlhelp32.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <functiondiscoverykeys_devpkey.h>
#include <wlanapi.h>
#include <netlistmgr.h>
#include <msctf.h>
#include <bluetoothapis.h>
#include <setupapi.h>
#include <devpropdef.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#ifndef TF_IPPMF_FORSESSION
#define TF_IPPMF_FORSESSION 0x20000000
#define TF_IPPMF_DONTCARECURRENTINPUTLANGUAGE 0x00000004
#endif
#include <windhawk_utils.h>

#include <algorithm>
#include <deque>
#include <functional>
#include <atomic>
#include <memory>
#include <cmath>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Microsoft::WRL::ComPtr;
static void MediaSetOn(bool on);
static std::atomic<bool> g_devHubOn{false};  // (the "Use BatteryHub" setting, read by the devices worker)
static void DevSetOn(bool on);

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    int iconSize = 60;
    int iconGap = 18;
    int magnification = 120;
    bool autoHide = true;
    int hideDelay = 400;
    int bottomGap = 12;
    COLORREF tint = RGB(5, 5, 6);
    bool customTint = false;
    int tintOpacity = 62;
    bool hideTaskbar = true;
    bool showStart = true;
    std::wstring style = L"nevoa";
    bool fixedBar = false;              // format: fixa
    bool nevoaStart = true;             // startMenu: nevoa
    bool winKey = false;                // winKeyMode: nevoa (the Windows key opens Nimbus' Start)
    std::wstring startHotkey = L"none";  // altSpace | ctrlSpace | none
    std::wstring endTask = L"windows";  // windows | on | off
    std::wstring fileSearch = L"everything";  // everything | windows | off
    bool wideSearch = false;            // searchLayout: larga
    std::wstring animation = L"zoom";   // zoom | elevar | foco | nenhuma
    std::wstring statusPos = L"direita";  // direita | esquerda | iniciar | off
    int statPlace[5] = {1, 1, 1, 1, 1};  // by StatId (keyboard, network, volume, battery, clock): 0 off, 1 dock, 2 Start
    bool statusWinPanels = false;       // statusPanels: windows (a click opens Windows' own panels)
    std::wstring trayStyle = L"botao";  // botao | lado | grade | lista | off
    int trayVisible = 5;                // icons shown at once in the strip
    int trayPlace = 1;                  // 0 off, 1 dock, 2 Start
    int shelfPlace = 1;                 // 0 off, 1 dock, 2 Start
    std::wstring shelfDrag = L"copiar"; // dragged out to a folder: copiar | mover | windows (Windows' default)
    bool shelfList = false;             // shelfStyle: lista
    int shelfLimit = 10;
    int notifPlace = 1;      // the bell: 0 off, 1 dock, 2 Start
    bool notifShow = true;   // Nimbus' own cards for Windows' notifications
    bool notifTop = false;   // notifV: cima
    int notifH = 2;          // notifH: 0 esquerda, 1 centro, 2 direita
    int notifSecs = 6;
    bool notifHideWin = false;  // Windows' own banners off (each app's "Show banners"), with the user's ok
    bool notifWinN = false;     // Win+N opens Nimbus' notifications instead of Windows' panel
    int notesPlace = 1;         // notes and reminders (places: 0 off, 1 dock, 2 Start, 3 the control center)
    bool notifDnd = false;      // do not disturb: no Nimbus cards (the bell's list still keeps them)
    int extraPlace[8] = {4, 4, 4, 4, 4, 4, 1, 1};  // do not disturb, Bluetooth, night light, shortcuts, brightness, power, player, devices
} g_set;

// Colors of the chosen look (Névoa, Cristal, Grafite, Aurora).
struct Theme {
    D2D1_COLOR_F tint;
    bool blur;
    bool edgeHorizontal;
    D2D1_COLOR_F edge[3];
    D2D1_COLOR_F dot, labelBg, labelText, startTile, startGlyph, sep, hover, text;
} g_theme;

static D2D1_COLOR_F Rgba(int r, int g, int b, float a) { return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, a); }

static bool TransparencyOn() {
    DWORD v = 1, size = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &v, &size);
    BOOL saver = FALSE;
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps) && ps.SystemStatusFlag == 1) saver = TRUE;
    return v != 0 && !saver;
}

static bool g_transparencyUsed = true;  // what the theme was built with (see WM_SETTINGCHANGE)
static bool g_contrastUsed = false;

// Windows' accessibility: high contrast (solid system colours, no glass) and "Animation effects" off
// (panels appear at once, nothing slides).
static bool HighContrastOn() {
    HIGHCONTRASTW hc = {sizeof(hc)};
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) && (hc.dwFlags & HCF_HIGHCONTRASTON);
}
static bool ReduceMotion() {
    BOOL on = TRUE;
    return SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &on, 0) && !on;
}

static Theme MakeTheme(const std::wstring& style, float op) {
    Theme t;
    if (style == L"cristal") {
        t = {Rgba(236, 238, 244, op * 0.9f), true, false,
             {Rgba(255, 255, 255, 0.95f), Rgba(255, 255, 255, 0.45f), Rgba(255, 255, 255, 0.6f)},
             Rgba(27, 27, 31, 0.9f), Rgba(245, 246, 250, 0.94f), Rgba(27, 27, 31, 1), Rgba(0, 0, 0, 0.08f),
             Rgba(27, 27, 31, 1), Rgba(0, 0, 0, 0.14f), Rgba(0, 0, 0, 0.07f), Rgba(27, 27, 31, 1)};
    } else if (style == L"grafite") {
        t = {Rgba(14, 14, 16, 1), false, false, {Rgba(38, 38, 43, 1), Rgba(38, 38, 43, 1), Rgba(38, 38, 43, 1)},
             Rgba(232, 232, 236, 0.9f), Rgba(14, 14, 16, 1), Rgba(232, 232, 236, 1), Rgba(29, 29, 33, 1),
             Rgba(232, 232, 236, 1), Rgba(42, 42, 48, 1), Rgba(255, 255, 255, 0.07f), Rgba(232, 232, 236, 1)};
    } else if (style == L"aurora") {
        t = {Rgba(14, 9, 24, std::min(1.0f, op * 1.05f)), true, true,
             {Rgba(255, 46, 136, 1), Rgba(255, 92, 80, 1), Rgba(255, 122, 26, 1)},
             Rgba(255, 122, 158, 1), Rgba(14, 9, 24, 0.92f), Rgba(255, 255, 255, 1), Rgba(255, 46, 136, 0.22f),
             Rgba(255, 255, 255, 1), Rgba(255, 255, 255, 0.14f), Rgba(255, 46, 136, 0.14f), Rgba(243, 238, 248, 1)};
    } else {
        t = {Rgba(5, 5, 6, op), true, false,
             {Rgba(255, 255, 255, 0.30f), Rgba(255, 255, 255, 0.06f), Rgba(255, 255, 255, 0.12f)},
             Rgba(245, 245, 247, 0.9f), Rgba(5, 5, 6, 0.88f), Rgba(245, 245, 247, 1), Rgba(255, 255, 255, 0.12f),
             Rgba(245, 245, 247, 1), Rgba(255, 255, 255, 0.14f), Rgba(255, 255, 255, 0.08f), Rgba(237, 237, 240, 1)};
    }
    return t;
}

static void BuildTheme() {
    Theme t = MakeTheme(g_set.style, g_set.tintOpacity / 100.0f);
    if (g_set.customTint && g_set.style != L"grafite") {
        t.tint = Rgba(GetRValue(g_set.tint), GetGValue(g_set.tint), GetBValue(g_set.tint), t.tint.a);
    }
    // Transparency effects off in Windows (or battery saver): its blur draws a black block, so the
    // glass becomes the solid tint.
    g_transparencyUsed = TransparencyOn();
    if (!g_transparencyUsed) {
        t.blur = false;
        t.tint.a = 1;
    }
    g_contrastUsed = HighContrastOn();
    if (g_contrastUsed) {
        auto sys = [](int i, float a) {
            COLORREF c = GetSysColor(i);
            return Rgba(GetRValue(c), GetGValue(c), GetBValue(c), a);
        };
        t.blur = false;
        t.tint = sys(COLOR_WINDOW, 1);
        t.text = sys(COLOR_WINDOWTEXT, 1);
        t.hover = sys(COLOR_HIGHLIGHT, 0.45f);
        t.sep = sys(COLOR_WINDOWTEXT, 0.6f);
        t.edge[0] = t.edge[1] = t.edge[2] = sys(COLOR_WINDOWTEXT, 1);
        t.labelBg = sys(COLOR_WINDOW, 1);
        t.labelText = sys(COLOR_WINDOWTEXT, 1);
        t.startTile = sys(COLOR_HIGHLIGHT, 1);
        t.startGlyph = sys(COLOR_HIGHLIGHTTEXT, 1);
        t.dot = sys(COLOR_HIGHLIGHT, 1);
    }
    g_theme = t;
}

static int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }


// ---------------------------------------------------------------------------
// Text shown by the mod: English, or Portuguese when Windows' display language is Portuguese.
// ---------------------------------------------------------------------------

enum StrId {
    S_Start, S_BarSettings, S_TaskManager, S_NewWindow, S_PinDock, S_UnpinDock, S_CloseAll, S_CloseWindow,
    S_PinStart, S_UnpinStart, S_SearchHint, S_Lock, S_Sleep, S_Restart, S_ShutDown, S_KeysHint, S_TabPinned,
    S_TabAll, S_CountPinned, S_CountApps, S_PinHint, S_Apps, S_NoApps, S_BestMatch, S_FilesFolders,
    S_SearchFilesPre, S_SearchFilesPost, S_OpenEverything, S_ExplorerSearch, S_GetEverything, S_TypeMore,
    S_NoFiles, S_Settings, S_KindSetting, S_KindFolder, S_KindFile, S_KindApp, S_Open, S_OpenLocation,
    S_EndTask, S_TabRecent, S_CountRecent, S_NoRecent, S_CopyPath, S_AgoNow, S_AgoMin, S_AgoHour,
    S_AgoDay, S_Count
};

static const wchar_t* const kStrEn[S_Count] = {
    L"Start", L"Nimbus settings", L"Task Manager", L"Open new window", L"Pin to dock",
    L"Unpin from dock", L"Close all windows", L"Close window", L"Pin to Start", L"Unpin from Start",
    L"Search apps and files", L"Lock", L"Sleep", L"Restart", L"Shut down",
    L"↑ ↓ to choose     Enter opens     Esc clears the search", L"Pinned", L"All apps", L"%d pinned",
    L"%d apps", L"Right-click an app in \"All apps\" to pin it here", L"Apps", L"No app with that name",
    L"Best match", L"Files and folders", L"Search files for “", L"” (File Explorer)",
    L"Open Everything to search files", L"Search files with File Explorer",
    L"Get Everything (free, instant search)", L"Type at least 2 letters", L"No files", L"Settings",
    L"Windows Settings", L"Folder", L"File", L"App", L"Open", L"Open file location",
    L"End task", L"Downloads", L"%d files", L"Nothing downloaded in the last 30 days", L"Copy path",
    L"just now", L"%d min ago", L"%d h ago", L"%d d ago",
};

static const wchar_t* const kStrPt[S_Count] = {
    L"Iniciar", L"Ajustes do Nimbus", L"Gerenciador de Tarefas", L"Abrir nova janela",
    L"Fixar na barra", L"Desafixar da barra", L"Fechar todas as janelas", L"Fechar janela", L"Fixar no Iniciar",
    L"Desafixar do Iniciar", L"Buscar apps e arquivos", L"Bloquear", L"Suspender", L"Reiniciar", L"Desligar",
    L"↑ ↓ pra escolher     Enter abre     Esc limpa a busca", L"Fixados", L"Todos os apps",
    L"%d fixados", L"%d apps", L"Clique com o botão direito num app em \"Todos os apps\" pra fixar aqui", L"Apps",
    L"Nenhum app com esse nome", L"Melhor resultado", L"Arquivos e pastas", L"Buscar “",
    L"” nos arquivos (Explorador)", L"Abrir o Everything pra buscar arquivos",
    L"Buscar nos arquivos pelo Explorador", L"Baixar o Everything (grátis, busca na hora)",
    L"Digite pelo menos 2 letras", L"Nenhum arquivo", L"Configurações", L"Configurações do Windows", L"Pasta",
    L"Arquivo", L"App", L"Abrir", L"Abrir o local do arquivo",
    L"Finalizar tarefa", L"Baixados", L"%d arquivos", L"Nada baixado nos últimos 30 dias",
    L"Copiar caminho", L"agora", L"há %d min", L"há %d h", L"há %d d",
};

static bool IsPortugueseUi() {
    static int pt = -1;
    if (pt < 0) pt = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE;
    return pt == 1;
}

static const wchar_t* Tr(StrId id) { return (IsPortugueseUi() ? kStrPt : kStrEn)[id]; }

// ---------------------------------------------------------------------------
// Settings storage. Every setting is in Nimbus' own settings window, saved in the mod's storage (per
// user, see UserPrefix). Windhawk's settings page is empty on purpose since 2.0; what was chosen there
// before (1.x) is read once as the starting value.
// ---------------------------------------------------------------------------

static const std::wstring& UserPrefix();

struct CfgKey {
    const wchar_t* name;
    bool isInt;
};
static const CfgKey kCfgKeys[] = {
    {L"style", false}, {L"format", false}, {L"animation", false}, {L"showStart", true}, {L"startMenu", false},
    {L"winKeyMode", false}, {L"startHotkey", false}, {L"endTask", false}, {L"searchLayout", false},
    {L"fileSearch", false}, {L"iconSize", true}, {L"iconGap", true}, {L"magnification", true}, {L"autoHide", true},
    {L"hideDelay", true}, {L"bottomGap", true}, {L"tintColor", false}, {L"tintOpacity", true},
    {L"hideWindowsTaskbar", true}, {L"statusPos", false},
};
// Settings added in 2.0 (never on Windhawk's page): name, default.
static const std::pair<const wchar_t*, const wchar_t*> kOwnKeys[] = {
    {L"statusClock", L"1"}, {L"statusVolume", L"1"}, {L"statusNetwork", L"1"}, {L"statusBattery", L"1"},
    {L"statusKeyboard", L"1"}, {L"statusPanels", L"nimbus"}, {L"trayStyle", L"botao"},
    {L"trayVisible", L"5"}, {L"trayPlace", L"1"}, {L"shelfPlace", L"dock"}, {L"shelfDrag", L"copiar"},
    {L"shelfStyle", L"grade"}, {L"shelfLimit", L"10"}, {L"notifPlace", L"dock"}, {L"notifShow", L"1"},
    {L"notifV", L"baixo"}, {L"notifH", L"direita"}, {L"notifSecs", L"6"}, {L"notifHideWin", L"0"},
    {L"notifSound", L"pingo"}, {L"remSound", L"bolha"},
    {L"notifWinN", L"0"}, {L"notesPlace", L"dock"}, {L"notifDnd", L"0"}, {L"dndPlace", L"central"}, {L"btPlace", L"central"}, {L"playerPlace", L"dock"}, {L"devPlace", L"dock"}, {L"devAlerts", L"1"}, {L"devHub", L"0"}, {L"playerDock", L"ambos"}, {L"playerHide", L"0"},
    {L"nightPlace", L"central"}, {L"shortPlace", L"central"}, {L"lightPlace", L"central"}, {L"powerPlace", L"central"},
};
static const wchar_t* const kMonGroups[] = {L"monPrimary", L"monLeft", L"monRight", L"monOther"};
static const CfgKey kMonFields[] = {{L"enabled", true}, {L"format", false}, {L"autoHide", false},
                                   {L"showStart", true}, {L"showPinned", true}, {L"windows", false}};

std::unordered_map<std::wstring, std::wstring> g_cfg;  // effective values, as text

// Values are saved as "=" + text, so an empty text (tintColor) differs from a missing value.
static bool ReadVal(const std::wstring& name, std::wstring& out) {
    wchar_t buf[512] = {};
    if (!Wh_GetStringValue(name.c_str(), buf, ARRAYSIZE(buf)) || buf[0] != L'=') return false;
    out = buf + 1;
    return true;
}
static void WriteVal(const std::wstring& name, const std::wstring& v) { Wh_SetStringValue(name.c_str(), (L"=" + v).c_str()); }

static std::wstring WhSetting(const std::wstring& name, bool isInt) {
    if (isInt) return std::to_wstring(Wh_GetIntSetting(name.c_str()));
    return WindhawkUtils::StringSetting::make(name.c_str()).get();
}

// Monitors with their own choices, saved by device (see MonitorIdentity): "id1,id2,...".
static std::vector<std::wstring> MonIdsWithChoices() {
    std::vector<std::wstring> ids;
    std::wstring list;
    if (!ReadVal(UserPrefix() + L"monIds", list)) return ids;
    size_t a = 0;
    while (a < list.size()) {
        size_t b = list.find(L',', a);
        if (b == std::wstring::npos) b = list.size();
        if (b > a) ids.push_back(list.substr(a, b - a));
        a = b + 1;
    }
    return ids;
}

static const wchar_t* CfgDefault(const std::wstring& name);

// One setting: the Nimbus window's choice; else what Windhawk's page last had (2.0 builds noted it, 1.x
// kept it as a Windhawk setting); else the default.
static void SyncCfgKey(const std::wstring& name, bool isInt) {
    std::wstring v;
    if (ReadVal(UserPrefix() + L"cfg." + name, v) || ReadVal(UserPrefix() + L"seen." + name, v)) {
        g_cfg[name] = v;
        return;
    }
    v = WhSetting(name, isInt);
    // Windhawk answers 0 both for "set to 0/off" and for "never set". Every 1.x user has the look stored,
    // so then a 0 is real. What came from there is written as Nimbus' own once (it no longer depends on
    // Windhawk keeping the old settings).
    static int legacy = -1;
    if (legacy < 0) legacy = !WhSetting(L"style", false).empty() ? 1 : 0;
    bool real = isInt ? (v != L"0" || legacy == 1) : !v.empty();
    g_cfg[name] = real ? v : CfgDefault(name);
    if (real && legacy == 1) WriteVal(UserPrefix() + L"cfg." + name, v);
}

static void LoadCfg() {
    g_cfg.clear();
    for (auto& k : kCfgKeys) SyncCfgKey(k.name, k.isInt);
    for (auto& k : kOwnKeys) {
        std::wstring v;
        g_cfg[k.first] = ReadVal(UserPrefix() + L"cfg." + k.first, v) ? v : k.second;
    }
    for (auto* g : kMonGroups)
        for (auto& f : kMonFields) SyncCfgKey(std::wstring(g) + L"." + f.name, f.isInt);
}

static int CfgInt(const wchar_t* name) {
    auto it = g_cfg.find(name);
    return it == g_cfg.end() ? 0 : _wtoi(it->second.c_str());
}
static std::wstring CfgStr(const wchar_t* name, const wchar_t* def = L"") {
    auto it = g_cfg.find(name);
    return it == g_cfg.end() || it->second.empty() ? def : it->second;
}

// A monitor's own choice for a field, else its position group's.
static std::wstring MonCfg(const std::wstring& id, const wchar_t* group, const wchar_t* field) {
    std::wstring v;
    if (!id.empty() && ReadVal(UserPrefix() + L"mon." + id + L"." + field, v)) return v;
    auto it = g_cfg.find(std::wstring(group) + L"." + field);
    return it == g_cfg.end() ? L"" : it->second;
}

// Where a status item (or the tray) goes: "dock", "central" (the control center), "iniciar" or "off".
// "1" (the default, and what 2.0's first builds saved) follows statusPos; "0" is off. The tray started in
// the dock whatever statusPos says.
// Where an item shows: any of the dock (1), Start (2) and the control center (4), or nowhere.
// Saved as words with commas ("dock,central"); one word (older settings) still reads.
constexpr int PlDock = 1, PlStart = 2, PlCentral = 4;
static int PlaceOf(const std::wstring& v, bool tray = false) {
    if (v == L"off" || v == L"0") return 0;
    {
        int m = 0;
        size_t a = 0;
        bool known = true;
        while (a <= v.size()) {
            size_t b = v.find(L',', a);
            std::wstring w = v.substr(a, b == std::wstring::npos ? std::wstring::npos : b - a);
            if (w == L"dock") m |= PlDock;
            else if (w == L"iniciar") m |= PlStart;
            else if (w == L"central") m |= PlCentral;
            else known = false;
            if (b == std::wstring::npos) break;
            a = b + 1;
        }
        if (known && m) return m;
    }
    if (tray) return 1;
    std::wstring pos = CfgStr(L"statusPos", L"direita");
    return pos == L"iniciar" ? 2 : pos == L"off" ? 0 : 1;
}
static std::wstring PlaceName(int p) {
    std::wstring s;
    if (p & PlDock) s += L"dock";
    if (p & PlCentral) s += std::wstring(s.empty() ? L"" : L",") + L"central";
    if (p & PlStart) s += std::wstring(s.empty() ? L"" : L",") + L"iniciar";
    return s.empty() ? L"off" : s;
}

void LoadSettings() {
    LoadCfg();
    g_set.iconSize = Clamp(CfgInt(L"iconSize"), 24, 128);
    g_set.iconGap = Clamp(CfgInt(L"iconGap"), 0, 64);
    g_set.magnification = Clamp(CfgInt(L"magnification"), 100, 250);
    g_set.autoHide = CfgInt(L"autoHide") != 0;
    g_set.hideDelay = Clamp(CfgInt(L"hideDelay"), 0, 5000);
    g_set.bottomGap = Clamp(CfgInt(L"bottomGap"), 0, 100);
    g_set.tintOpacity = Clamp(CfgInt(L"tintOpacity"), 0, 100);
    g_set.hideTaskbar = CfgInt(L"hideWindowsTaskbar") != 0;
    g_set.showStart = CfgInt(L"showStart") != 0;
    auto str = [](const wchar_t* name, const wchar_t* def) { return CfgStr(name, def); };
    g_set.style = str(L"style", L"nevoa");
    g_set.fixedBar = str(L"format", L"dock") == L"fixa";
    g_set.nevoaStart = str(L"startMenu", L"nevoa") != L"windows";
    g_set.winKey = str(L"winKeyMode", L"nevoa") == L"nevoa";
    g_set.startHotkey = str(L"startHotkey", L"none");
    g_set.endTask = str(L"endTask", L"windows");
    g_set.fileSearch = str(L"fileSearch", L"everything");
    g_set.wideSearch = str(L"searchLayout", L"larga") == L"larga";
    g_set.animation = str(L"animation", L"zoom");
    g_set.statusPos = str(L"statusPos", L"direita");
    const wchar_t* statKeys[5] = {L"statusKeyboard", L"statusNetwork", L"statusVolume", L"statusBattery", L"statusClock"};
    for (int i = 0; i < 5; i++) g_set.statPlace[i] = PlaceOf(CfgStr(statKeys[i], L"1"));
    g_set.statusWinPanels = str(L"statusPanels", L"nimbus") == L"windows";
    g_set.trayStyle = str(L"trayStyle", L"botao");
    g_set.trayVisible = std::clamp(CfgInt(L"trayVisible"), 3, 9);
    g_set.trayPlace = g_set.trayStyle == L"off" ? 0 : PlaceOf(CfgStr(L"trayPlace", L"1"), true);
    g_set.shelfPlace = PlaceOf(CfgStr(L"shelfPlace", L"dock"), true);
    g_set.shelfDrag = str(L"shelfDrag", L"copiar");
    g_set.shelfList = str(L"shelfStyle", L"grade") == L"lista";
    g_set.shelfLimit = std::clamp(CfgInt(L"shelfLimit"), 5, 20);
    g_set.notifPlace = PlaceOf(CfgStr(L"notifPlace", L"dock"), true);
    g_set.notifShow = CfgInt(L"notifShow") != 0;
    g_set.notifTop = str(L"notifV", L"baixo") == L"cima";
    {
        std::wstring h = str(L"notifH", L"direita");
        g_set.notifH = h == L"esquerda" ? 0 : h == L"centro" ? 1 : 2;
    }
    g_set.notifSecs = std::clamp(CfgInt(L"notifSecs"), 3, 12);
    g_set.notifHideWin = CfgInt(L"notifHideWin") != 0;
    g_set.notifWinN = CfgInt(L"notifWinN") != 0;
    g_set.notesPlace = PlaceOf(CfgStr(L"notesPlace", L"dock"), true);
    g_set.notifDnd = CfgInt(L"notifDnd") != 0;
    {
        const wchar_t* keys[8] = {L"dndPlace", L"btPlace", L"nightPlace", L"shortPlace", L"lightPlace", L"powerPlace", L"playerPlace", L"devPlace"};
        for (int k = 0; k < 8; k++) g_set.extraPlace[k] = PlaceOf(CfgStr(keys[k], k >= 6 ? L"dock" : L"central"), true);
    }
    MediaSetOn(g_set.extraPlace[6] != 0);
    g_devHubOn = CfgInt(L"devHub") != 0;
    DevSetOn(g_set.extraPlace[7] != 0);
    std::wstring tint = str(L"tintColor", L"");
    g_set.customTint = false;
    const wchar_t* s = tint.c_str();
    if (s[0] == L'#') s++;
    if (wcslen(s) == 6) {
        unsigned long v = wcstoul(s, nullptr, 16);
        g_set.tint = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
        g_set.customTint = true;
    }
    BuildTheme();
}

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

HINSTANCE g_module;
HANDLE g_uiThread;
HWND g_msgWnd;
UINT g_shellHookMsg;
UINT g_taskbarCreatedMsg;

ComPtr<ID2D1Factory> g_d2d;
ComPtr<IDWriteFactory> g_dw;
ComPtr<IWICImagingFactory> g_wic;

constexpr UINT WM_APP_SETTINGS = WM_APP + 1;
constexpr UINT WM_APP_QUIT = WM_APP + 2;
constexpr UINT_PTR TIMER_POLL = 1;
constexpr UINT_PTR TIMER_REBUILD = 2;
constexpr UINT_PTR TIMER_DOCKS = 3;
constexpr UINT_PTR TIMER_ANIM = 10;

static const PROPERTYKEY kPkeyAumid = {
    {0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 5};

static bool EqI(const std::wstring& a, const std::wstring& b) {
    return !a.empty() && !b.empty() && _wcsicmp(a.c_str(), b.c_str()) == 0;
}

static std::wstring Lower(std::wstring s) {
    CharLowerBuffW(s.data(), (DWORD)s.size());
    return s;
}

// ---------------------------------------------------------------------------
// Apps (Start, pinned, running)
// ---------------------------------------------------------------------------

struct Pin {
    std::wstring kind;   // lnk | aumid | exe
    std::wstring value;
    // resolved (lnk)
    std::wstring target, args, aumid;
    bool resolved = false;
};
std::vector<Pin> g_pins;

struct App {
    std::wstring key;
    std::wstring aumid, exe, lnk, args, label;
    bool start = false, pinned = false;
    int pinIndex = -1;
    std::vector<HWND> wins;
    bool flash = false;
    ComPtr<IWICBitmapSource> icon;
    bool iconTried = false;
};
std::vector<App> g_apps;
// The user's locale for text layout (line breaking, digits), not a fixed language.
wchar_t g_locale[LOCALE_NAME_MAX_LENGTH] = L"en-US";
std::unordered_map<HWND, bool> g_flashing;
// Apps being opened (key -> tick): their icon bounces until a window shows up.
std::unordered_map<std::wstring, DWORD> g_launch;
constexpr DWORD kLaunchBounceMs = 8000;

struct WinInfo {
    std::wstring aumid, exe;
};
std::unordered_map<HWND, WinInfo> g_winCache;

static std::wstring GetPropString(IPropertyStore* ps, const PROPERTYKEY& key) {
    std::wstring out;
    PROPVARIANT pv;
    PropVariantInit(&pv);
    if (SUCCEEDED(ps->GetValue(key, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) {
        out = pv.pwszVal;
    }
    PropVariantClear(&pv);
    return out;
}

static std::wstring ProcessPath(DWORD pid) {
    std::wstring out;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) {
        wchar_t buf[MAX_PATH * 2];
        DWORD len = ARRAYSIZE(buf);
        if (QueryFullProcessImageNameW(h, 0, buf, &len)) out.assign(buf, len);
        CloseHandle(h);
    }
    return out;
}

static std::wstring DirOf(const std::wstring& p);
static bool IsGenericDir(const std::wstring& dirLower);

// Some apps show their window from a helper in a subfolder (Steam: bin\cef\...\steamwebhelper.exe,
// started by steam.exe). Then the app is the parent program, whose folder contains the helper's.
static std::wstring MainProgramOf(DWORD pid, const std::wstring& exe) {
    if (exe.empty()) return exe;
    DWORD parent = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return exe;
    PROCESSENTRY32W pe = {sizeof(pe)};
    for (BOOL ok = Process32FirstW(snap, &pe); ok; ok = Process32NextW(snap, &pe)) {
        if (pe.th32ProcessID == pid) { parent = pe.th32ParentProcessID; break; }
    }
    CloseHandle(snap);
    if (!parent) return exe;
    std::wstring pexe = ProcessPath(parent);
    if (pexe.empty()) return exe;
    std::wstring dir = DirOf(exe), pdir = DirOf(pexe);
    if (pdir.size() < dir.size() && dir.compare(0, pdir.size(), pdir) == 0 && !IsGenericDir(pdir)) return pexe;
    return exe;
}

static WinInfo GetWinInfo(HWND hwnd) {
    auto it = g_winCache.find(hwnd);
    if (it != g_winCache.end()) return it->second;
    WinInfo info;
    ComPtr<IPropertyStore> ps;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&ps)))) {
        info.aumid = GetPropString(ps.Get(), kPkeyAumid);
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    info.exe = ProcessPath(pid);
    // Helpers that carry their app's AUMID (Steam's web helper) belong to the parent program. Games
    // started by a launcher from its own library folder (Steam, GOG, Ubisoft) set none and keep their exe.
    if (!info.aumid.empty()) info.exe = MainProgramOf(pid, info.exe);
    g_winCache[hwnd] = info;
    return info;
}

// Windhawk 1.x runs this process as 32-bit (WOW64): System32 paths would start the 32-bit copies, and
// %ProgramFiles% would be "Program Files (x86)".
static bool IsWow64() {
    static int w = -1;
    if (w < 0) {
        BOOL b = FALSE;
        w = IsWow64Process(GetCurrentProcess(), &b) && b ? 1 : 0;
    }
    return w == 1;
}
static std::wstring NativePath(const std::wstring& path) {
    if (!IsWow64()) return path;
    wchar_t win[MAX_PATH];
    if (!GetWindowsDirectoryW(win, MAX_PATH)) return path;
    std::wstring sys = std::wstring(win) + L"\\System32\\";
    if (path.size() >= sys.size() && _wcsnicmp(path.c_str(), sys.c_str(), sys.size()) == 0)
        return std::wstring(win) + L"\\Sysnative\\" + path.substr(sys.size());
    return path;
}

static void ResolvePin(Pin& p) {
    if (p.resolved) return;
    p.resolved = true;
    if (p.kind == L"aumid") { p.aumid = p.value; return; }
    if (p.kind == L"exe") { p.target = p.value; return; }
    ComPtr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)))) return;
    ComPtr<IPersistFile> pf;
    if (FAILED(link.As(&pf)) || FAILED(pf->Load(p.value.c_str(), STGM_READ))) return;
    wchar_t buf[MAX_PATH * 2] = {};
    if (SUCCEEDED(link->GetPath(buf, ARRAYSIZE(buf), nullptr, SLGP_RAWPATH))) {
        std::wstring raw = buf;
        if (IsWow64()) {  // (the 64-bit folders, like the apps' own windows report)
            auto swap = [&](const wchar_t* from, const wchar_t* to) {
                size_t k;
                while ((k = Lower(raw).find(Lower(from))) != std::wstring::npos) raw.replace(k, wcslen(from), to);
            };
            swap(L"%ProgramFiles%", L"%ProgramW6432%");
            swap(L"%CommonProgramFiles%", L"%CommonProgramW6432%");
        }
        wchar_t exp[MAX_PATH * 2] = {};
        ExpandEnvironmentStringsW(raw.c_str(), exp, ARRAYSIZE(exp));
        p.target = exp;
    }
    wchar_t args[1024] = {};
    if (SUCCEEDED(link->GetArguments(args, ARRAYSIZE(args)))) p.args = args;
    ComPtr<IPropertyStore> ps;
    if (SUCCEEDED(link.As(&ps))) p.aumid = GetPropString(ps.Get(), kPkeyAumid);
    // Shortcuts to shell places (File Explorer, This PC...) have no file target: they open in Explorer.
    if (p.target.empty() && (p.aumid.empty() || EqI(p.aumid, L"Microsoft.Windows.Explorer"))) {
        wchar_t win[MAX_PATH];
        if (GetWindowsDirectoryW(win, MAX_PATH)) p.target = std::wstring(win) + L"\\explorer.exe";
    }
}

static std::wstring DirOf(const std::wstring& p) {
    return Lower(p.substr(0, p.size() - wcslen(PathFindFileNameW(p.c_str()))));
}

// Folders shared by many apps: never enough to say two programs are the same app.
static bool IsGenericDir(const std::wstring& dirLower) {
    static std::vector<std::wstring> generic;
    if (generic.empty()) {
        auto add = [](const wchar_t* var, const wchar_t* sub) {
            wchar_t buf[MAX_PATH];
            if (GetEnvironmentVariableW(var, buf, MAX_PATH)) {
                std::wstring s = Lower(buf);
                if (s.back() != L'\\') s += L'\\';
                s += sub;
                generic.push_back(s);
            }
        };
        add(L"SystemRoot", L"");
        add(L"SystemRoot", L"system32\\");
        add(L"SystemRoot", L"syswow64\\");
        add(L"ProgramFiles", L"");
        add(L"ProgramFiles", L"windowsapps\\");
        add(L"ProgramFiles(x86)", L"");
        add(L"LOCALAPPDATA", L"");
        add(L"LOCALAPPDATA", L"programs\\");
        add(L"APPDATA", L"");
        add(L"SystemDrive", L"");
    }
    for (auto& g : generic) if (dirLower == g) return true;
    // Shared runtimes (python, node, java) installed per user are also generic.
    return dirLower.find(L"\\python") != std::wstring::npos || dirLower.find(L"\\nodejs") != std::wstring::npos;
}

static bool AppMatches(const App& a, const WinInfo& w) {
    if (a.start) return false;
    if (EqI(a.aumid, w.aumid)) return true;
    if (!a.aumid.empty() && !w.aumid.empty()) return false;
    if (EqI(a.exe, w.exe)) return true;
    // Launchers and helpers: the pinned target and the running program live in the same app folder
    // (one inside the other), e.g. Claude/Discord stubs, Docker's frontend, Python-based apps.
    if (a.pinned && !a.exe.empty() && !w.exe.empty()) {
        std::wstring da = DirOf(a.exe), dw = DirOf(w.exe);
        if (!da.empty() && !dw.empty() && !IsGenericDir(da) && !IsGenericDir(dw) &&
            (dw.rfind(da, 0) == 0 || da.rfind(dw, 0) == 0)) return true;
    }
    if (a.pinned && !a.args.empty() && !w.exe.empty()) {
        std::wstring exeName = Lower(PathFindFileNameW(w.exe.c_str()));
        if (Lower(a.args).find(exeName) != std::wstring::npos) return true;
    }
    return false;
}

static bool IsTaskWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) return false;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    HWND owner = GetWindow(hwnd, GW_OWNER);
    bool appWindow = (ex & WS_EX_APPWINDOW) != 0;
    if (!appWindow) {
        if (owner) return false;
        if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    }
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) return false;
    if (GetWindowTextLengthW(hwnd) == 0) return false;
    wchar_t cls[64];
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    static const wchar_t* skip[] = {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
                                    L"Windows.UI.Core.CoreWindow", L"NimbusMenu_Glass", L"NimbusMenu_Content"};
    for (auto s : skip) if (!wcscmp(cls, s)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) return !wcscmp(cls, L"NimbusMenu_Settings");  // only the settings window
    return true;
}

void LoadPins();
void SavePins();

static App MakeStartApp() {
    App a;
    a.start = true;
    a.key = L"#start";
    a.label = Tr(S_Start);
    return a;
}

void RebuildApps() {
    std::vector<App> next;
    next.push_back(MakeStartApp());  // each monitor decides whether to show it (DockCfg)
    for (int i = 0; i < (int)g_pins.size(); i++) {
        Pin& p = g_pins[i];
        ResolvePin(p);
        App a;
        a.pinned = true;
        a.pinIndex = i;
        a.aumid = p.aumid;
        a.exe = p.kind == L"exe" ? p.value : p.target;
        a.lnk = p.kind == L"lnk" ? p.value : L"";
        a.args = p.args;
        a.key = L"pin:" + Lower(p.value);
        if (!a.lnk.empty()) {
            std::wstring name = PathFindFileNameW(a.lnk.c_str());
            if (name.size() > 4 && EqI(name.substr(name.size() - 4), L".lnk")) name.resize(name.size() - 4);
            a.label = name;
        }
        next.push_back(a);
    }

    // Running windows in z-order (top first).
    std::vector<HWND> wins;
    EnumWindows([](HWND h, LPARAM lp) WINAPI -> BOOL {
        if (IsTaskWindow(h)) ((std::vector<HWND>*)lp)->push_back(h);
        return TRUE;
    }, (LPARAM)&wins);

    std::unordered_map<HWND, WinInfo> keep;
    for (HWND h : wins) {
        WinInfo w = GetWinInfo(h);
        keep[h] = w;
        App* found = nullptr;
        for (auto& a : next) {
            if (AppMatches(a, w)) { found = &a; break; }
        }
        if (!found) {
            std::wstring key = !w.aumid.empty() ? L"aumid:" + Lower(w.aumid) : L"exe:" + Lower(w.exe);
            for (auto& a : next) if (a.key == key) { found = &a; break; }
            if (!found) {
                App a;
                a.key = key;
                a.aumid = w.aumid;
                a.exe = w.exe;
                next.push_back(a);
                found = &next.back();
            }
        }
        found->wins.push_back(h);
        if (g_flashing.count(h)) found->flash = true;
    }
    g_winCache.swap(keep);

    // Keep icons and labels already loaded.
    for (auto& a : next) {
        for (auto& o : g_apps) {
            if (o.key == a.key) {
                a.icon = o.icon;
                a.iconTried = o.iconTried;
                if (!o.label.empty()) a.label = o.label;  // the shell's (translated) name, once known
                break;
            }
        }
    }
    g_apps.swap(next);
}

// Dock icons load on a worker thread: asking the shell for an icon can take a while (a slow drive, a
// cold cache), and the UI thread also runs the Start's mouse hook. The dock draws a plain tile meanwhile.
struct IconJob {
    std::wstring key, lnk, aumid, exe;
    bool wantLabel = false;
    int size = 128;
};
struct IconResult {
    std::wstring key, label;
    HBITMAP hbm = nullptr;
};
constexpr UINT WM_APP_ICON = WM_APP + 8;  // lParam = IconResult* (the receiver deletes it)

// Heap pointers handed over in messages: noted when posted, taken only if noted (a stray post with any
// value from another process is ignored instead of being freed).
static SRWLOCK g_ptrLock = SRWLOCK_INIT;
static std::unordered_set<void*> g_ptrs;
static bool PostPtr(HWND h, UINT msg, void* ptr) {
    AcquireSRWLockExclusive(&g_ptrLock);
    g_ptrs.insert(ptr);
    ReleaseSRWLockExclusive(&g_ptrLock);
    if (h && PostMessageW(h, msg, 0, (LPARAM)ptr)) return true;
    AcquireSRWLockExclusive(&g_ptrLock);
    g_ptrs.erase(ptr);
    ReleaseSRWLockExclusive(&g_ptrLock);
    return false;
}
static bool TakePtr(void* ptr) {
    AcquireSRWLockExclusive(&g_ptrLock);
    bool ok = g_ptrs.erase(ptr) > 0;
    ReleaseSRWLockExclusive(&g_ptrLock);
    return ok;
}
static SRWLOCK g_iconLock = SRWLOCK_INIT;
static CONDITION_VARIABLE g_iconWake = CONDITION_VARIABLE_INIT;
static std::deque<IconJob> g_iconJobs;
static bool g_iconStop = false;
static HANDLE g_iconThread = nullptr;

static DWORD WINAPI IconThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    for (;;) {
        IconJob job;
        AcquireSRWLockExclusive(&g_iconLock);
        while (g_iconJobs.empty() && !g_iconStop) SleepConditionVariableSRW(&g_iconWake, &g_iconLock, INFINITE, 0);
        bool stop = g_iconStop;
        if (!stop) {
            job = std::move(g_iconJobs.front());
            g_iconJobs.pop_front();
        }
        ReleaseSRWLockExclusive(&g_iconLock);
        if (stop) break;

        auto* r = new IconResult();
        r->key = job.key;
        ComPtr<IShellItem> item;
        if (!job.lnk.empty()) SHCreateItemFromParsingName(job.lnk.c_str(), nullptr, IID_PPV_ARGS(&item));
        if (!item && !job.aumid.empty()) {
            std::wstring p = L"shell:AppsFolder\\" + job.aumid;
            SHCreateItemFromParsingName(p.c_str(), nullptr, IID_PPV_ARGS(&item));
        }
        if (!item && !job.exe.empty()) SHCreateItemFromParsingName(job.exe.c_str(), nullptr, IID_PPV_ARGS(&item));
        if (item) {
            // The shell's display name is the translated one ("Explorador de Arquivos", not "File Explorer").
            PWSTR name = nullptr;
            if (job.wantLabel && SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name) {
                r->label = name;
                if (r->label.size() > 4 && EqI(r->label.substr(r->label.size() - 4), L".exe")) r->label.resize(r->label.size() - 4);
                CoTaskMemFree(name);
            }
            ComPtr<IShellItemImageFactory> factory;
            if (SUCCEEDED(item.As(&factory))) factory->GetImage(SIZE{job.size, job.size}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &r->hbm);
        }
        if (!PostPtr(g_msgWnd, WM_APP_ICON, r)) {
            if (r->hbm) DeleteObject(r->hbm);
            delete r;
        }
    }
    CoUninitialize();
    return 0;
}

constexpr wchar_t kSettingsAumid[] = L"igoredm.NimbusMenu.Settings";
static HBITMAP DrawNimbusIcon(int px);
static const wchar_t* SettingsTitle();

static void RequestIcon(const App& a) {
    if (EqI(a.aumid, kSettingsAumid)) {
        // The settings window: its icon is drawn here, not looked up in the shell.
        auto* r = new IconResult();
        r->key = a.key;
        r->label = SettingsTitle();
        r->hbm = DrawNimbusIcon(128);
        if (!PostPtr(g_msgWnd, WM_APP_ICON, r)) {
            if (r->hbm) DeleteObject(r->hbm);
            delete r;
        }
        return;
    }
    IconJob job;
    job.key = a.key;
    job.lnk = a.lnk;
    job.aumid = a.aumid;
    job.exe = a.exe;
    job.wantLabel = a.label.empty() || !a.lnk.empty();
    AcquireSRWLockExclusive(&g_iconLock);
    if (!g_iconThread && !g_iconStop) g_iconThread = CreateThread(nullptr, 0, IconThread, nullptr, 0, nullptr);
    g_iconJobs.push_back(std::move(job));
    ReleaseSRWLockExclusive(&g_iconLock);
    WakeConditionVariable(&g_iconWake);
}

static void StopIconThread() {
    AcquireSRWLockExclusive(&g_iconLock);
    g_iconStop = true;
    g_iconJobs.clear();
    ReleaseSRWLockExclusive(&g_iconLock);
    WakeAllConditionVariable(&g_iconWake);
    if (g_iconThread) {
        WaitForSingleObject(g_iconThread, 3000);
        CloseHandle(g_iconThread);
        g_iconThread = nullptr;
    }
}

static ComPtr<IWICBitmapSource> WicFromHBitmap(HBITMAP hbm) {
    ComPtr<IWICBitmap> wb;
    if (FAILED(g_wic->CreateBitmapFromHBITMAP(hbm, nullptr, WICBitmapUseAlpha, &wb))) return {};
    ComPtr<IWICFormatConverter> conv;
    if (FAILED(g_wic->CreateFormatConverter(&conv))) return {};
    if (FAILED(conv->Initialize(wb.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0,
                                WICBitmapPaletteTypeCustom))) return {};
    ComPtr<IWICBitmap> out;
    if (FAILED(g_wic->CreateBitmapFromSource(conv.Get(), WICBitmapCacheOnLoad, &out))) return {};
    return out;
}

// ---------------------------------------------------------------------------
// Pins storage
// ---------------------------------------------------------------------------

static std::vector<std::wstring> TaskbarPinnedLinks() {
    std::vector<std::wstring> files;
    wchar_t dir[MAX_PATH];
    if (!SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, dir))) return files;
    std::wstring base = std::wstring(dir) + L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\";
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((base + L"*.lnk").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return files;
    do files.push_back(base + fd.cFileName); while (FindNextFileW(h, &fd));
    FindClose(h);

    // Order them like the Windows taskbar: by where each name appears in Taskband\Favorites.
    std::vector<BYTE> blob;
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Taskband", 0,
                      KEY_READ, &key) == ERROR_SUCCESS) {
        DWORD size = 0;
        if (RegQueryValueExW(key, L"Favorites", nullptr, nullptr, nullptr, &size) == ERROR_SUCCESS && size) {
            blob.resize(size);
            RegQueryValueExW(key, L"Favorites", nullptr, nullptr, blob.data(), &size);
        }
        RegCloseKey(key);
    }
    auto pos = [&](const std::wstring& path) -> size_t {
        std::wstring name = PathFindFileNameW(path.c_str());
        if (blob.size() < name.size() * 2) return SIZE_MAX;
        for (size_t i = 0; i + name.size() * 2 <= blob.size(); i += 2) {
            if (_wcsnicmp((const wchar_t*)(blob.data() + i), name.c_str(), name.size()) == 0) return i;
        }
        return SIZE_MAX;
    };
    std::stable_sort(files.begin(), files.end(), [&](const std::wstring& a, const std::wstring& b) {
        return pos(a) < pos(b);
    });
    return files;
}

// Mod storage is shared by every user of the PC (and is an INI file in portable Windhawk), so each
// value name starts with the user's SID and lists are stored one value per item, with a count.
static const std::wstring& UserPrefix() {
    static std::wstring prefix;
    if (prefix.empty()) {
        HANDLE token;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            BYTE buf[256];
            DWORD len = 0;
            if (GetTokenInformation(token, TokenUser, buf, sizeof(buf), &len)) {
                LPWSTR sid = nullptr;
                if (ConvertSidToStringSidW(((TOKEN_USER*)buf)->User.Sid, &sid)) {
                    prefix = std::wstring(sid) + L"_";
                    LocalFree(sid);
                }
            }
            CloseHandle(token);
        }
        if (prefix.empty()) prefix = L"user_";
    }
    return prefix;
}

static std::wstring UKey(const wchar_t* fmt, int i = 0) {
    wchar_t name[64];
    swprintf_s(name, fmt, i);
    return UserPrefix() + name;
}

static std::wstring GetStr(const std::wstring& key) {
    wchar_t buf[2048] = {};
    Wh_GetStringValue(key.c_str(), buf, ARRAYSIZE(buf));
    return buf;
}

// A value of any length (the buffer grows until it fits; Windhawk returns nothing when it doesn't).
static bool ReadLong(const std::wstring& key, std::wstring& out) {
    // (empty, missing and too long all read as nothing: grow up to 131072 characters, the most any
    // value of this mod was ever written with)
    for (size_t n = 2048; n <= 131072 * 2; n *= 4) {
        std::vector<wchar_t> buf(n);
        size_t got = Wh_GetStringValue(key.c_str(), buf.data(), buf.size());
        if (got) {
            out.assign(buf.data(), got);
            return true;
        }
    }
    return false;
}

// A list kept as "<SID>_<name>.n" and one value per item, "<SID>_<name>.<i>" (no line breaks inside a
// value: the portable Windhawk keeps them in an INI file). Lists saved before as one string (with a
// separator) are read once from it and moved over. Returns false when nothing was ever saved.
// What each list was last saved or read as (only the entries that changed are written again: the
// portable Windhawk rewrites its INI file for every value).
static std::unordered_map<std::wstring, std::vector<std::wstring>> g_storedItems;
static bool StoreItems(const wchar_t* name, const std::vector<std::wstring>& items) {
    std::wstring base = UserPrefix() + name;
    auto known = g_storedItems.find(name);
    std::vector<std::wstring> before;
    int old;
    if (known != g_storedItems.end()) {
        before = known->second;
        old = (int)before.size();
    } else {
        old = Wh_GetIntValue((base + L".n").c_str(), 0);
    }
    bool ok = true;
    for (size_t i = 0; i < items.size(); i++)
        if (i >= before.size() || before[i] != items[i]) ok = Wh_SetStringValue((base + L"." + std::to_wstring(i)).c_str(), items[i].c_str()) && ok;
    for (int i = (int)items.size(); i < old; i++) Wh_DeleteValue((base + L"." + std::to_wstring(i)).c_str());
    if ((int)items.size() != old || known == g_storedItems.end()) ok = Wh_SetIntValue((base + L".n").c_str(), (int)items.size()) && ok;
    if (ok) g_storedItems[name] = items;
    else g_storedItems.erase(name);
    return ok;
}
static bool LoadItems(const wchar_t* name, wchar_t legacySep, std::vector<std::wstring>& out) {
    out.clear();
    std::wstring base = UserPrefix() + name;
    int n = Wh_GetIntValue((base + L".n").c_str(), -1);
    if (n >= 0) {
        bool whole = true;
        for (int i = 0; i < n; i++) {
            std::wstring v;
            if (ReadLong(base + L"." + std::to_wstring(i), v) && !v.empty()) out.push_back(v);
            else whole = false;
        }
        if (whole) g_storedItems[name] = out;  // (what's stored now, exactly)
        return true;
    }
    std::wstring all;
    if (!ReadLong(base, all)) return false;
    size_t a = 0;
    while (a < all.size()) {
        size_t b = all.find(legacySep, a);
        if (b == std::wstring::npos) b = all.size();
        if (b > a) out.push_back(all.substr(a, b - a));
        a = b + 1;
    }
    if (StoreItems(name, out)) Wh_DeleteValue(base.c_str());
    return true;
}

// Pairs of strings saved as <SID>_<list>Count, <SID>_<list>%dA, <SID>_<list>%dB.
static void SaveList(const wchar_t* list, const std::vector<std::pair<std::wstring, std::wstring>>& items) {
    std::wstring l = list;
    int old = Wh_GetIntValue(UKey((l + L"Count").c_str()).c_str(), 0);
    for (int i = 0; i < (int)items.size(); i++) {
        Wh_SetStringValue(UKey((l + L"%dA").c_str(), i).c_str(), items[i].first.c_str());
        Wh_SetStringValue(UKey((l + L"%dB").c_str(), i).c_str(), items[i].second.c_str());
    }
    for (int i = (int)items.size(); i < old; i++) {
        Wh_DeleteValue(UKey((l + L"%dA").c_str(), i).c_str());
        Wh_DeleteValue(UKey((l + L"%dB").c_str(), i).c_str());
    }
    Wh_SetIntValue(UKey((l + L"Count").c_str()).c_str(), (int)items.size());
}

// Returns false when the list was never saved for this user.
static bool LoadList(const wchar_t* list, std::vector<std::pair<std::wstring, std::wstring>>& items) {
    std::wstring l = list;
    int count = Wh_GetIntValue(UKey((l + L"Count").c_str()).c_str(), -1);
    if (count < 0) return false;
    for (int i = 0; i < count; i++) {
        std::wstring a = GetStr(UKey((l + L"%dA").c_str(), i)), b = GetStr(UKey((l + L"%dB").c_str(), i));
        if (!a.empty() && !b.empty()) items.push_back({a, b});
    }
    return true;
}

// Version 1.0.0 before the review kept each list as one string ("a\tb\n..."), without the SID.
static bool LoadLegacyList(const wchar_t* name, std::vector<std::pair<std::wstring, std::wstring>>& items) {
    std::vector<wchar_t> buf(65536);
    if (!Wh_GetStringValue(name, buf.data(), buf.size())) return false;
    std::wstring all(buf.data());
    size_t start = 0;
    while (start < all.size()) {
        size_t end = all.find(L'\n', start);
        if (end == std::wstring::npos) end = all.size();
        std::wstring line = all.substr(start, end - start);
        size_t tab = line.find(L'\t');
        if (tab != std::wstring::npos) items.push_back({line.substr(0, tab), line.substr(tab + 1)});
        start = end + 1;
    }
    Wh_DeleteValue(name);
    return true;
}

void LoadPins() {
    g_pins.clear();
    std::vector<std::pair<std::wstring, std::wstring>> items;
    if (!LoadList(L"pin", items)) {
        if (!LoadLegacyList(L"pins", items)) {
            for (auto& f : TaskbarPinnedLinks()) items.push_back({L"lnk", f});
        }
        Wh_DeleteValue(L"pinsInit");
        for (auto& it : items) g_pins.push_back({it.first, it.second});
        SavePins();
        return;
    }
    for (auto& it : items) g_pins.push_back({it.first, it.second});
}

void SavePins() {
    std::vector<std::pair<std::wstring, std::wstring>> items;
    for (auto& p : g_pins) items.push_back({p.kind, p.value});
    SaveList(L"pin", items);
}

// ---------------------------------------------------------------------------
// Windows taskbar
// ---------------------------------------------------------------------------

// Hiding the Windows taskbar: its windows are hidden, and its auto-hide is turned on so that it stops
// reserving screen space (maximized windows would stop above an invisible bar otherwise). Explorer
// saves auto-hide in the user's profile, so the original state is kept in mod storage the first time
// it is changed (<SID>_autoHideOrig) and given back when the mod stops, also after an unclean exit:
// by the next start of the bar, or by the explorer.exe part when the mod is turned off.

bool g_taskbarHidden = false;

static void ForEachTaskbar(void (*fn)(HWND)) {
    HWND h = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (h) fn(h);
    HWND s = nullptr;
    while ((s = FindWindowExW(nullptr, s, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) fn(s);
}

static void TurnOnTaskbarAutoHide() {
    APPBARDATA abd = {sizeof(abd)};
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    if (state & ABS_AUTOHIDE) return;  // already on (by the user, or by us before an unclean exit)
    std::wstring key = UKey(L"autoHideOrig");
    if (Wh_GetIntValue(key.c_str(), -1) < 0) Wh_SetIntValue(key.c_str(), (int)state);
    abd.lParam = state | ABS_AUTOHIDE;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}

// Gives the taskbar back its own auto-hide setting, if this mod changed it. Safe to call from any
// process of the user and more than once.
static void RestoreTaskbarAutoHide() {
    std::wstring key = UKey(L"autoHideOrig");
    int orig = Wh_GetIntValue(key.c_str(), -1);
    if (orig < 0) return;
    APPBARDATA abd = {sizeof(abd)};
    abd.lParam = (LPARAM)orig;
    SHAppBarMessage(ABM_SETSTATE, &abd);
    Wh_DeleteValue(key.c_str());
}

static void ShowTaskbarWindows() {
    ForEachTaskbar([](HWND h) { ShowWindowAsync(h, SW_SHOWNA); });
}

void HideTaskbars() {
    if (!g_taskbarHidden) {
        TurnOnTaskbarAutoHide();
        g_taskbarHidden = true;
    }
    ForEachTaskbar([](HWND h) {
        if (IsWindowVisible(h)) ShowWindowAsync(h, SW_HIDE);
    });
}

void RestoreTaskbars() {
    if (!g_taskbarHidden) return;
    ShowTaskbarWindows();
    RestoreTaskbarAutoHide();
    g_taskbarHidden = false;
}

// ---------------------------------------------------------------------------
// Docks
// ---------------------------------------------------------------------------

struct Elem {
    int app = -1;      // index in g_apps, -1 = separator
    std::wstring key;  // app key or separator id, to keep its state across rebuilds
    float x = 0, w = 0, m = 1, target = 1;
    float appear = 1;  // 0..1, grows in when an app opens
    float lift = 0, liftT = 0;  // "Elevar": raised icon
    float op = 1, opT = 1;      // "Foco": the others fade
};

// Each monitor's own choices (Windhawk settings monPrimary / monLeft / monRight / monOther).
struct DockCfg {
    bool enabled = true;
    bool fixed = false;
    bool autoHide = true;
    bool showStart = true;
    bool showPinned = true;
    bool onlyThis = false;  // open windows: only the ones on this monitor
};

static DockCfg LoadDockCfg(const wchar_t* group, const std::wstring& monId) {
    DockCfg c;
    auto v = [&](const wchar_t* field) { return MonCfg(monId, group, field); };
    c.enabled = _wtoi(v(L"enabled").c_str()) != 0;
    std::wstring f = v(L"format");
    c.fixed = f == L"fixa" || ((f.empty() || f == L"geral") && g_set.fixedBar);
    std::wstring h = v(L"autoHide");
    c.autoHide = h == L"sim" ? true : h == L"nao" ? false : g_set.autoHide;
    c.showStart = _wtoi(v(L"showStart").c_str()) != 0 && g_set.showStart;
    c.showPinned = _wtoi(v(L"showPinned").c_str()) != 0;
    c.onlyThis = v(L"windows") == L"deste";
    return c;
}

// A monitor as a device: its id survives changing the monitors' order or position (it comes from the
// monitor's device path, which names the monitor model and the port it's on), plus its friendly name.
struct MonIdentity {
    std::wstring id, name;
};

static std::wstring HashId(const std::wstring& s) {
    unsigned long long h = 1469598103934665603ULL;
    for (wchar_t c : s) {
        h ^= (unsigned long long)towlower(c);
        h *= 1099511628211ULL;
    }
    wchar_t buf[20];
    swprintf_s(buf, L"%016llx", h);
    return buf;
}

static MonIdentity MonitorIdentity(HMONITOR mon) {
    MonIdentity out;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) return out;
    UINT32 np = 0, nm = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &np, &nm) == ERROR_SUCCESS) {
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(np);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(nm);
        if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &np, paths.data(), &nm, modes.data(), nullptr) == ERROR_SUCCESS) {
            for (UINT32 i = 0; i < np; i++) {
                DISPLAYCONFIG_SOURCE_DEVICE_NAME src = {};
                src.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                src.header.size = sizeof(src);
                src.header.adapterId = paths[i].sourceInfo.adapterId;
                src.header.id = paths[i].sourceInfo.id;
                if (DisplayConfigGetDeviceInfo(&src.header) != ERROR_SUCCESS || wcscmp(src.viewGdiDeviceName, mi.szDevice))
                    continue;
                DISPLAYCONFIG_TARGET_DEVICE_NAME tgt = {};
                tgt.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
                tgt.header.size = sizeof(tgt);
                tgt.header.adapterId = paths[i].targetInfo.adapterId;
                tgt.header.id = paths[i].targetInfo.id;
                if (DisplayConfigGetDeviceInfo(&tgt.header) != ERROR_SUCCESS) continue;
                if (tgt.monitorDevicePath[0]) out.id = HashId(tgt.monitorDevicePath);
                out.name = tgt.monitorFriendlyDeviceName;
                break;
            }
        }
    }
    if (out.id.empty()) out.id = HashId(mi.szDevice);
    return out;
}

// Which settings group a monitor uses: by where it is next to the primary one.
static const wchar_t* MonitorGroup(HMONITOR mon, const RECT& rc) {
    HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (mon == primary) return L"monPrimary";
    MONITORINFO pi = {sizeof(pi)};
    GetMonitorInfoW(primary, &pi);
    LONG cx = (rc.left + rc.right) / 2;
    if (cx < pi.rcMonitor.left) return L"monLeft";
    if (cx >= pi.rcMonitor.right) return L"monRight";
    return L"monOther";
}

// ---------------------------------------------------------------------------
// Status area: clock, volume, network, battery and keyboard, at one end of the dock or in a Start tab.
// Each item opens a Nimbus panel (see the Flyout section) or, if the user prefers, Windows' own.
// ---------------------------------------------------------------------------

constexpr UINT WM_APP_STATUS = WM_APP + 10;  // wParam: StatId that changed; lParam: 1 = reload its source

static const wchar_t* L2(const wchar_t* pt, const wchar_t* en) { return IsPortugueseUi() ? pt : en; }

enum StatId { StKeyboard, StNetwork, StVolume, StBattery, StClock, StCount };
class StatAudioCb;

struct WifiNet {
    std::wstring ssid, profile;
    int signal = 0;
    bool secure = false, connected = false, known = false;
    DOT11_BSS_TYPE bss = dot11_BSS_type_infrastructure;
};

struct KbInfo {
    std::wstring code, name, layout, klid;
};

struct StatusData {
    // Clock
    int minute = -1;
    SYSTEMTIME now{};
    std::wstring time, date, dateLong;
    // Volume
    ComPtr<IMMDeviceEnumerator> mmde;
    ComPtr<IAudioEndpointVolume> ep;
    StatAudioCb* audioCb = nullptr;
    std::wstring device;
    float vol = 0;
    bool muted = false, audio = false;
    // Network
    ComPtr<INetworkListManager> nlm;
    int net = 0;  // 0 offline, 1 cable, 2 Wi-Fi
    bool internet = false;
    std::wstring wifiName;
    int signal = 0;
    HANDLE wlan = nullptr;
    GUID wifiIf{};
    bool hasWifi = false, wifiOn = true;
    std::vector<WifiNet> nets;
    // Battery
    bool battery = false, charging = false, plugged = false;
    int pct = 0;
    DWORD secsLeft = (DWORD)-1;
    // Keyboard
    std::vector<HKL> layouts;
    HKL cur = nullptr;
    HWND target = nullptr;  // the window a layout change goes to (the last one in front that isn't ours)
    ComPtr<ITfInputProcessorProfileMgr> tsf;
    HKL lastRead = nullptr;  // the target window's layout as last read
    HWND lastTarget = nullptr;
    std::unordered_map<HKL, KbInfo> kb;
} g_stat;

// The shelf: files kept for a moment (only their paths), and what was pasted into it (made into files
// in %TEMP%\Nimbus\Prateleira, removed when the mod stops unless dragged out first).
constexpr int StShelf = 60;  // its status item and panel
struct ShelfItem {
    std::wstring path, name, sub;
    bool gen = false;  // made from a paste
    ComPtr<IWICBitmapSource> icon;
};
std::vector<ShelfItem> g_shelf;

// Notifications: Windows' own (read from its notification database), shown as Nimbus cards.
constexpr int StNotif = 61;  // the bell's status item and panel
struct NotifAction {
    std::wstring label, args, type;
};
struct NotifItem {
    long long id = 0;
    std::wstring aumid, title, body, launch, launchType;
    std::vector<NotifAction> actions;
    long long arrival = 0;  // FILETIME
    bool silent = false;    // the app asked for no sound
    bool sticky = false;    // an alarm, a reminder or a call: the card stays until dismissed
    bool reminder = false;  // a Nimbus reminder (its own sound)
};
std::vector<NotifItem> g_notifs;  // newest first

// Notes (kept until deleted) and reminders (a Nimbus notification when due, also late).
constexpr int StNotes = 62;
struct Note {
    long long id = 0, when = 0;  // when: created or last edited (FILETIME, UTC)
    std::wstring text;
};
struct Reminder {
    long long id = 0, due = 0, firedAt = 0;  // FILETIME, UTC
    std::wstring text;
    bool onOpen = false;  // "as soon as Nimbus runs" that day (due = that day's midnight)
    bool late = true;     // still tell when the PC was off at that time
    bool fired = false, missed = false;
};
std::vector<Note> g_notes;
std::vector<Reminder> g_rems;
static int RemPending() {
    int n = 0;
    for (auto& r : g_rems)
        if (!r.fired && !r.missed) n++;
    return n;
}

// The control center and the items that came with it.
constexpr int StDnd = 70, StBt = 71, StNight = 72, StShort = 73, StLight = 74, StPower = 75, StPlayer = 76, StDevices = 77, StCentral = 80;
static bool IsExtra(int id) { return id >= StDnd && id <= StDevices; }

// Wireless devices' batteries (Bluetooth, Xbox controllers, and BatteryHub's receivers when installed).
struct DevBattery {
    std::wstring key, name, via, kind;  // kind: mouse, headset, keyboard, gamepad, phone, other
    int percent = -1;
    bool charging = false;
};
static std::vector<DevBattery> DevicesNow();

// What's playing (any app in Windows' media controls), read on a worker thread.
struct MediaInfo {
    bool has = false;
    std::wstring aumid, title, artist;
    int status = 0;  // 1 playing, 2 paused or stopped
    bool canPrev = false, canNext = false, canToggle = false, canShuffle = false, canRepeat = false, canSeek = false;
    bool shuffle = false;
    int repeat = 0;  // 0 off, 1 the track, 2 the list
    double pos = 0, dur = 0;  // seconds
    DWORD posAt = 0;          // when pos was read
    std::vector<std::wstring> sessions;
    int thumbGen = 0;
};
static MediaInfo MediaNow();
static void MediaSetOn(bool on);
static void DevSetOn(bool on);
static void MediaCmd(int cmd, double arg = 0);
static int ExtraPlace(int id) { return g_set.extraPlace[id - StDnd]; }
static bool BtPresent();

static bool StatOn(int id) {
    if (id == StBattery && !g_stat.battery) return false;
    return g_set.statPlace[id] != 0;
}
static bool StatOnLeft() { return g_set.statusPos == L"esquerda"; }

// Anything in the control center (then its button is in the dock).
static bool CentralOn() {
    for (int id = 0; id < StCount; id++)
        if (StatOn(id) && (g_set.statPlace[id] & PlCentral)) return true;
    if ((g_set.trayPlace & PlCentral) || (g_set.shelfPlace & PlCentral) || (g_set.notifPlace & PlCentral) || (g_set.notesPlace & PlCentral)) return true;
    for (int k = 0; k < 8; k++)
        if ((g_set.extraPlace[k] & PlCentral)) return true;
    return false;
}

// The items in the dock, left to right.
static std::vector<int> StatIds() {
    std::vector<int> ids;
    if ((g_set.notesPlace & PlDock)) ids.push_back(StNotes);
    if ((g_set.notifPlace & PlDock)) ids.push_back(StNotif);
    if ((g_set.shelfPlace & PlDock)) ids.push_back(StShelf);
    if ((ExtraPlace(StPlayer) & PlDock) && (!CfgInt(L"playerHide") || MediaNow().has)) ids.push_back(StPlayer);
    if ((ExtraPlace(StDevices) & PlDock) && !DevicesNow().empty()) ids.push_back(StDevices);
    for (int id : {StDnd, StNight, StBt, StShort, StLight, StPower})
        if ((ExtraPlace(id) & PlDock) && (id != StBt || BtPresent())) ids.push_back(id);
    for (int id : {StKeyboard, StNetwork, StVolume, StBattery})
        if (StatOn(id) && (g_set.statPlace[id] & PlDock)) ids.push_back(id);
    if (CentralOn()) ids.push_back(StCentral);
    if (StatOn(StClock) && (g_set.statPlace[StClock] & PlDock)) ids.push_back(StClock);
    if (StatOnLeft()) std::reverse(ids.begin(), ids.end());
    return ids;
}

static void PostStatus(int id, int reload = 0) {
    if (g_msgWnd) PostMessageW(g_msgWnd, WM_APP_STATUS, id, reload);
}

// ---- Clock ----

// True when the shown minute changed.
static bool UpdateClock() {
    GetLocalTime(&g_stat.now);
    int m = g_stat.now.wDay * 1440 + g_stat.now.wHour * 60 + g_stat.now.wMinute;
    if (m == g_stat.minute) return false;
    g_stat.minute = m;
    wchar_t b[128];
    if (GetTimeFormatEx(g_locale, TIME_NOSECONDS, &g_stat.now, nullptr, b, ARRAYSIZE(b))) g_stat.time = b;
    if (GetDateFormatEx(g_locale, DATE_SHORTDATE, &g_stat.now, nullptr, b, ARRAYSIZE(b), nullptr)) g_stat.date = b;
    if (GetDateFormatEx(g_locale, DATE_LONGDATE, &g_stat.now, nullptr, b, ARRAYSIZE(b), nullptr)) g_stat.dateLong = b;
    return true;
}

// ---- Volume ----

// Windows calls these on its own threads: they only post a message to the UI thread.
class StatAudioCb final : public IAudioEndpointVolumeCallback, public IMMNotificationClient {
    LONG ref_ = 1;

   public:
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&ref_); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG r = InterlockedDecrement(&ref_);
        if (!r) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** pp) override {
        if (iid == IID_IUnknown || iid == __uuidof(IAudioEndpointVolumeCallback))
            *pp = static_cast<IAudioEndpointVolumeCallback*>(this);
        else if (iid == __uuidof(IMMNotificationClient))
            *pp = static_cast<IMMNotificationClient*>(this);
        else {
            *pp = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        PostStatus(StVolume);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eMultimedia) PostStatus(StVolume, 1);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }
};

static void ReadVolume() {
    g_stat.audio = false;
    if (!g_stat.ep) return;
    BOOL mute = FALSE;
    if (SUCCEEDED(g_stat.ep->GetMasterVolumeLevelScalar(&g_stat.vol)) && SUCCEEDED(g_stat.ep->GetMute(&mute))) {
        g_stat.muted = mute != FALSE;
        g_stat.audio = true;
    }
}

// The default output device (again, after it changed).
static void OpenAudio() {
    StatAudioCb* cb = g_stat.audioCb;
    if (g_stat.ep && cb) g_stat.ep->UnregisterControlChangeNotify(cb);
    g_stat.ep.Reset();
    g_stat.device.clear();
    if (!g_stat.mmde) {
        CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&g_stat.mmde));
        if (!g_stat.mmde) return;
        if (!cb) {
            cb = new StatAudioCb();
            g_stat.audioCb = cb;
        }
        g_stat.mmde->RegisterEndpointNotificationCallback(cb);
    }
    ComPtr<IMMDevice> dev;
    if (SUCCEEDED(g_stat.mmde->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) && dev) {
        dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)g_stat.ep.GetAddressOf());
        ComPtr<IPropertyStore> ps;
        if (SUCCEEDED(dev->OpenPropertyStore(STGM_READ, &ps))) {
            PROPVARIANT v;
            PropVariantInit(&v);
            if (SUCCEEDED(ps->GetValue(PKEY_Device_DeviceDesc, &v)) && v.vt == VT_LPWSTR) g_stat.device = v.pwszVal;
            PropVariantClear(&v);
        }
    }
    if (g_stat.ep && cb) g_stat.ep->RegisterControlChangeNotify(cb);
    ReadVolume();
}

static void CloseAudio() {
    StatAudioCb* cb = g_stat.audioCb;
    if (g_stat.ep && cb) g_stat.ep->UnregisterControlChangeNotify(cb);
    if (g_stat.mmde && cb) g_stat.mmde->UnregisterEndpointNotificationCallback(cb);
    g_stat.ep.Reset();
    g_stat.mmde.Reset();
    if (cb) cb->Release();
    g_stat.audioCb = nullptr;
}

static void SetVolume(float v) {
    v = std::clamp(v, 0.0f, 1.0f);
    if (!g_stat.ep) return;
    g_stat.ep->SetMasterVolumeLevelScalar(v, nullptr);
    if (g_stat.muted && v > 0) g_stat.ep->SetMute(FALSE, nullptr);
    g_stat.vol = v;
    if (v > 0) g_stat.muted = false;
}

static void ToggleMute() {
    if (!g_stat.ep) return;
    g_stat.ep->SetMute(g_stat.muted ? FALSE : TRUE, nullptr);
    g_stat.muted = !g_stat.muted;
}

// One app's sound (all of its sessions on the default device).
struct AppVol {
    DWORD pid = 0;
    bool system = false;
    std::wstring name, path;
    std::vector<ComPtr<ISimpleAudioVolume>> vols;
    float level = 1;
    ComPtr<IWICBitmapSource> icon;
};
std::vector<AppVol> g_appVols;

static std::wstring FileDescription(const std::wstring& path) {
    DWORD h = 0, n = GetFileVersionInfoSizeW(path.c_str(), &h);
    if (!n) return L"";
    std::vector<BYTE> buf(n);
    if (!GetFileVersionInfoW(path.c_str(), 0, n, buf.data())) return L"";
    struct LangCp { WORD lang, cp; }* tr = nullptr;
    UINT len = 0;
    if (!VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (void**)&tr, &len) || len < sizeof(LangCp)) return L"";
    wchar_t key[64];
    swprintf_s(key, L"\\StringFileInfo\\%04x%04x\\FileDescription", tr[0].lang, tr[0].cp);
    wchar_t* val = nullptr;
    if (!VerQueryValueW(buf.data(), key, (void**)&val, &len) || !val || !*val) return L"";
    return val;
}

static void LoadAppVolumes() {
    g_appVols.clear();
    if (!g_stat.mmde) return;
    ComPtr<IMMDevice> dev;
    ComPtr<IAudioSessionManager2> mgr;
    ComPtr<IAudioSessionEnumerator> en;
    if (FAILED(g_stat.mmde->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) ||
        FAILED(dev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)mgr.GetAddressOf())) ||
        FAILED(mgr->GetSessionEnumerator(&en)))
        return;
    int n = 0;
    en->GetCount(&n);
    for (int i = 0; i < n; i++) {
        ComPtr<IAudioSessionControl> c;
        ComPtr<IAudioSessionControl2> c2;
        ComPtr<ISimpleAudioVolume> sv;
        if (FAILED(en->GetSession(i, &c)) || FAILED(c.As(&c2)) || FAILED(c.As(&sv))) continue;
        AudioSessionState st;
        if (FAILED(c->GetState(&st)) || st == AudioSessionStateExpired) continue;
        DWORD pid = 0;
        c2->GetProcessId(&pid);
        bool system = c2->IsSystemSoundsSession() == S_OK;
        auto it = std::find_if(g_appVols.begin(), g_appVols.end(),
                               [&](const AppVol& a) { return system ? a.system : (!a.system && a.pid == pid); });
        if (it == g_appVols.end()) {
            AppVol a;
            a.pid = pid;
            a.system = system;
            if (system) {
                a.name = L2(L"Sons do sistema", L"System sounds");
            } else {
                a.path = ProcessPath(pid);
                if (a.path.empty()) continue;
                a.name = FileDescription(a.path);
                if (a.name.empty()) {
                    a.name = PathFindFileNameW(a.path.c_str());
                    PathRemoveExtensionW(a.name.data());
                    a.name.resize(wcslen(a.name.c_str()));
                }
            }
            sv->GetMasterVolume(&a.level);
            g_appVols.push_back(a);
            it = g_appVols.end() - 1;
        }
        it->vols.push_back(sv);
    }
    // Apps first (by name), the system sounds last.
    std::stable_sort(g_appVols.begin(), g_appVols.end(), [](const AppVol& a, const AppVol& b) {
        if (a.system != b.system) return !a.system;
        return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
    });
    if (g_appVols.size() > 7) g_appVols.resize(7);
}

static void SetAppVolume(AppVol& a, float v) {
    a.level = std::clamp(v, 0.0f, 1.0f);
    for (auto& sv : a.vols) sv->SetMasterVolume(a.level, nullptr);
}

// ---- Network ----

static void CALLBACK WlanNotify(PWLAN_NOTIFICATION_DATA d, PVOID) {
    if (!d || d->NotificationSource != WLAN_NOTIFICATION_SOURCE_ACM) return;
    switch (d->NotificationCode) {
        case wlan_notification_acm_scan_complete:
        case wlan_notification_acm_scan_fail:
        case wlan_notification_acm_network_available:
            PostStatus(StNetwork, 2);  // the list of networks
            break;
        default:
            PostStatus(StNetwork);
            break;
    }
}

static std::wstring SsidText(const DOT11_SSID& s) {
    int n = MultiByteToWideChar(CP_UTF8, 0, (const char*)s.ucSSID, (int)s.uSSIDLength, nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (const char*)s.ucSSID, (int)s.uSSIDLength, w.data(), n);
    return w;
}

static void OpenWlan() {
    if (g_stat.wlan) return;
    DWORD ver = 0;
    if (WlanOpenHandle(2, nullptr, &ver, &g_stat.wlan) != ERROR_SUCCESS) {
        g_stat.wlan = nullptr;
        return;
    }
    PWLAN_INTERFACE_INFO_LIST list = nullptr;
    if (WlanEnumInterfaces(g_stat.wlan, nullptr, &list) == ERROR_SUCCESS && list) {
        if (list->dwNumberOfItems > 0) {
            g_stat.wifiIf = list->InterfaceInfo[0].InterfaceGuid;
            g_stat.hasWifi = true;
        }
        WlanFreeMemory(list);
    }
    if (g_stat.hasWifi) WlanRegisterNotification(g_stat.wlan, WLAN_NOTIFICATION_SOURCE_ACM, TRUE, WlanNotify, nullptr, nullptr, nullptr);
}

static void CloseWlan() {
    if (!g_stat.wlan) return;
    WlanRegisterNotification(g_stat.wlan, WLAN_NOTIFICATION_SOURCE_NONE, TRUE, nullptr, nullptr, nullptr, nullptr);
    WlanCloseHandle(g_stat.wlan, nullptr);
    g_stat.wlan = nullptr;
    g_stat.hasWifi = false;
}

// True when what the dock shows changed.
static bool ReadNetwork() {
    int net = 0, signal = 0;
    bool internet = false, wifiOn = g_stat.wifiOn;
    std::wstring name;
    if (!g_stat.nlm) CoCreateInstance(CLSID_NetworkListManager, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&g_stat.nlm));
    NLM_CONNECTIVITY c = NLM_CONNECTIVITY_DISCONNECTED;
    if (g_stat.nlm) g_stat.nlm->GetConnectivity(&c);
    internet = (c & (NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET)) != 0;
    bool connected = c != NLM_CONNECTIVITY_DISCONNECTED;
    if (g_stat.hasWifi) {
        DWORD size = 0;
        PWLAN_CONNECTION_ATTRIBUTES ca = nullptr;
        if (WlanQueryInterface(g_stat.wlan, &g_stat.wifiIf, wlan_intf_opcode_current_connection, nullptr, &size,
                               (PVOID*)&ca, nullptr) == ERROR_SUCCESS && ca) {
            if (ca->isState == wlan_interface_state_connected) {
                net = 2;
                name = SsidText(ca->wlanAssociationAttributes.dot11Ssid);
                signal = (int)ca->wlanAssociationAttributes.wlanSignalQuality;
            }
            WlanFreeMemory(ca);
        }
        PWLAN_RADIO_STATE rs = nullptr;
        if (WlanQueryInterface(g_stat.wlan, &g_stat.wifiIf, wlan_intf_opcode_radio_state, nullptr, &size, (PVOID*)&rs,
                               nullptr) == ERROR_SUCCESS && rs) {
            wifiOn = false;
            for (DWORD i = 0; i < rs->dwNumberOfPhys; i++)
                if (rs->PhyRadioState[i].dot11SoftwareRadioState == dot11_radio_state_on &&
                    rs->PhyRadioState[i].dot11HardwareRadioState == dot11_radio_state_on)
                    wifiOn = true;
            WlanFreeMemory(rs);
        }
    }
    if (!net && connected) net = 1;
    bool changed = net != g_stat.net || internet != g_stat.internet || name != g_stat.wifiName ||
                   signal / 25 != g_stat.signal / 25 || wifiOn != g_stat.wifiOn;
    g_stat.net = net;
    g_stat.internet = internet;
    g_stat.wifiName = name;
    g_stat.signal = signal;
    g_stat.wifiOn = wifiOn;
    return changed;
}

static void LoadWifiList() {
    g_stat.nets.clear();
    if (!g_stat.hasWifi) return;
    PWLAN_AVAILABLE_NETWORK_LIST list = nullptr;
    if (WlanGetAvailableNetworkList(g_stat.wlan, &g_stat.wifiIf, 0, nullptr, &list) != ERROR_SUCCESS || !list) return;
    for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
        const WLAN_AVAILABLE_NETWORK& n = list->Network[i];
        if (!n.dot11Ssid.uSSIDLength) continue;  // hidden network
        WifiNet w;
        w.ssid = SsidText(n.dot11Ssid);
        w.signal = (int)n.wlanSignalQuality;
        w.secure = n.bSecurityEnabled != FALSE;
        w.connected = (n.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) != 0;
        w.known = (n.dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE) != 0;
        w.profile = n.strProfileName;
        w.bss = n.dot11BssType;
        auto it = std::find_if(g_stat.nets.begin(), g_stat.nets.end(), [&](const WifiNet& x) { return x.ssid == w.ssid; });
        if (it == g_stat.nets.end()) {
            g_stat.nets.push_back(w);
        } else {
            // The same network twice (one entry per profile): keep what we know of both.
            it->signal = std::max(it->signal, w.signal);
            it->connected = it->connected || w.connected;
            if (w.known && !it->known) {
                it->known = true;
                it->profile = w.profile;
            }
        }
    }
    WlanFreeMemory(list);
    std::stable_sort(g_stat.nets.begin(), g_stat.nets.end(), [](const WifiNet& a, const WifiNet& b) {
        if (a.connected != b.connected) return a.connected;
        return a.signal > b.signal;
    });
    if (g_stat.nets.size() > 7) g_stat.nets.resize(7);
}

static void ScanWifi() {
    if (g_stat.hasWifi) WlanScan(g_stat.wlan, &g_stat.wifiIf, nullptr, nullptr, nullptr);
}

static void OpenUri(const wchar_t* uri) { ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL); }

static void ConnectWifi(const WifiNet& n) {
    if (n.connected) {
        WlanDisconnect(g_stat.wlan, &g_stat.wifiIf, nullptr);
        return;
    }
    if (!n.known) {
        // A new network: the password is typed in Windows' own list.
        OpenUri(L"ms-availablenetworks:");
        return;
    }
    WLAN_CONNECTION_PARAMETERS p = {};
    p.wlanConnectionMode = wlan_connection_mode_profile;
    p.strProfile = n.profile.c_str();
    p.dot11BssType = n.bss;
    if (WlanConnect(g_stat.wlan, &g_stat.wifiIf, &p, nullptr) != ERROR_SUCCESS) OpenUri(L"ms-availablenetworks:");
}

static void SetWifiRadio(bool on) {
    WLAN_PHY_RADIO_STATE st = {};
    st.dwPhyIndex = 0;
    st.dot11SoftwareRadioState = on ? dot11_radio_state_on : dot11_radio_state_off;
    if (WlanSetInterface(g_stat.wlan, &g_stat.wifiIf, wlan_intf_opcode_radio_state, sizeof(st), &st, nullptr) != ERROR_SUCCESS)
        OpenUri(L"ms-settings:network-wifi");
}

// ---- Battery ----

static bool ReadBattery() {
    SYSTEM_POWER_STATUS ps;
    if (!GetSystemPowerStatus(&ps)) return false;
    bool battery = ps.BatteryFlag != 255 && !(ps.BatteryFlag & 128) && ps.BatteryLifePercent <= 100;
    int pct = battery ? ps.BatteryLifePercent : 0;
    bool plugged = ps.ACLineStatus == 1, charging = battery && (ps.BatteryFlag & 8);
    bool changed = battery != g_stat.battery || pct != g_stat.pct || plugged != g_stat.plugged || charging != g_stat.charging;
    g_stat.battery = battery;
    g_stat.pct = pct;
    g_stat.plugged = plugged;
    g_stat.charging = charging;
    g_stat.secsLeft = ps.BatteryLifeTime;
    return changed;
}

// Windows 11's power mode ("overlay"): best efficiency, balanced, best performance.
static const GUID kPowerModes[3] = {
    {0x961cc777, 0x2547, 0x4f9d, {0x81, 0x74, 0x7d, 0x86, 0x18, 0x1b, 0x8a, 0x7a}},
    {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0xded574b5, 0x45a0, 0x4f42, {0x87, 0x37, 0x46, 0x34, 0x5c, 0x09, 0xc2, 0x38}},
};

// The power plan (Control Panel's): "High performance" or "Ultimate" counts as performance and
// "Power saver" as efficiency; on "Balanced", the mode (quick settings' slider) decides.
static const GUID kPlanBalanced = {0x381b4222, 0xf694, 0x41f0, {0x96, 0x85, 0xff, 0x5b, 0xb2, 0x60, 0xdf, 0x2e}};
static const GUID kPlanHigh = {0x8c5e7fda, 0xe8bf, 0x4a96, {0x9a, 0x85, 0xa6, 0xe2, 0x3a, 0x8c, 0x63, 0x5c}};
static const GUID kPlanUltimate = {0xe9a42b02, 0xd5df, 0x448d, {0xaa, 0x00, 0x03, 0xf1, 0x47, 0x49, 0xeb, 0x61}};
static const GUID kPlanSaver = {0xa1841308, 0x3541, 0x4fab, {0xbc, 0x81, 0xf7, 0x15, 0x56, 0xf2, 0x0b, 0x4a}};

static bool ActivePlan(GUID& out) {
    using Fn = DWORD(WINAPI*)(HKEY, GUID**);
    static Fn get = (Fn)GetProcAddress(LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32), "PowerGetActiveScheme");
    GUID* g = nullptr;
    if (!get || get(nullptr, &g) != ERROR_SUCCESS || !g) return false;
    out = *g;
    LocalFree(g);
    return true;
}

static int PowerMode() {
    GUID plan;
    if (ActivePlan(plan)) {
        if (IsEqualGUID(plan, kPlanHigh) || IsEqualGUID(plan, kPlanUltimate)) return 2;
        if (IsEqualGUID(plan, kPlanSaver)) return 0;
    }
    using Fn = DWORD(WINAPI*)(GUID*);
    static Fn get = (Fn)GetProcAddress(LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32), "PowerGetEffectiveOverlayScheme");
    GUID g = {};
    if (!get || get(&g) != ERROR_SUCCESS) return 1;
    for (int i = 0; i < 3; i++) if (IsEqualGUID(g, kPowerModes[i])) return i;
    return 1;
}

static int PowerModeCached();

// Each button picks the plan of its name when the PC has it ("Power saver", "Balanced", "High
// performance"); without it, "Balanced" with the matching mode (what quick settings change).
static bool SetPlan(const GUID& g) {
    using SetFn = DWORD(WINAPI*)(HKEY, const GUID*);
    static SetFn setPlan = (SetFn)GetProcAddress(LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32), "PowerSetActiveScheme");
    return setPlan && setPlan(nullptr, &g) == ERROR_SUCCESS;
}
static void SetPowerMode(int i) {
    GUID plan = {};
    bool known = ActivePlan(plan);
    if (i == 2) {
        if (known && (IsEqualGUID(plan, kPlanHigh) || IsEqualGUID(plan, kPlanUltimate))) return;
        if (SetPlan(kPlanHigh) || SetPlan(kPlanUltimate)) {
            Wh_Log(L"power: High performance plan");
            return;
        }
    } else if (i == 0) {
        if (known && IsEqualGUID(plan, kPlanSaver)) return;
        if (SetPlan(kPlanSaver)) {
            Wh_Log(L"power: Power saver plan");
            return;
        }
    }
    if (!known || !IsEqualGUID(plan, kPlanBalanced)) SetPlan(kPlanBalanced);
    using Fn = DWORD(WINAPI*)(REFGUID);
    static Fn set = (Fn)GetProcAddress(LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32), "PowerSetActiveOverlayScheme");
    if (set) set(kPowerModes[i]);
    Wh_Log(L"power: Balanced plan, mode %d", i);
}

// ---- Keyboard ----

static bool IsOurWindow(HWND h) {
    wchar_t cls[64] = {};
    GetClassNameW(GetAncestor(h, GA_ROOT), cls, ARRAYSIZE(cls));
    return !wcsncmp(cls, L"NimbusMenu_", 11);
}

static const KbInfo& KbOf(HKL hkl) {
    auto it = g_stat.kb.find(hkl);
    if (it != g_stat.kb.end()) return it->second;
    KbInfo k;
    LCID lcid = MAKELCID(LOWORD((UINT_PTR)hkl), SORT_DEFAULT);
    wchar_t b[128] = {}, loc[LOCALE_NAME_MAX_LENGTH] = {};
    if (GetLocaleInfoW(lcid, LOCALE_SISO639LANGNAME, b, ARRAYSIZE(b))) {
        k.code = b;
        for (auto& c : k.code) c = towupper(c);
    }
    if (LCIDToLocaleName(lcid, loc, ARRAYSIZE(loc), 0) && GetLocaleInfoEx(loc, LOCALE_SLOCALIZEDDISPLAYNAME, b, ARRAYSIZE(b)))
        k.name = b;
    // The layout's name ("ABNT2", "US") from its id, which only the active layout gives: activate it on this
    // thread for a moment (this thread only; nothing else sees it).
    HKL mine = GetKeyboardLayout(0);
    wchar_t klid[KL_NAMELENGTH] = {};
    if (ActivateKeyboardLayout(hkl, 0) && GetKeyboardLayoutNameW(klid)) {
        k.klid = klid;
        std::wstring key = std::wstring(L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\") + klid;
        HKEY h;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &h) == ERROR_SUCCESS) {
            wchar_t t[128] = {};
            if (RegLoadMUIStringW(h, L"Layout Display Name", t, sizeof(t), nullptr, 0, nullptr) == ERROR_SUCCESS ||
                [&] { DWORD sz = sizeof(t); return RegQueryValueExW(h, L"Layout Text", nullptr, nullptr, (BYTE*)t, &sz) == ERROR_SUCCESS; }())
                k.layout = t;
            RegCloseKey(h);
        }
    }
    ActivateKeyboardLayout(mine, 0);
    if (k.code.empty()) k.code = L"??";
    return g_stat.kb[hkl] = k;
}

// True when what the dock shows changed.
static bool ReadKeyboard() {
    HWND fg = GetForegroundWindow();
    if (fg && !IsOurWindow(fg)) g_stat.target = fg;
    if (g_stat.target && !IsWindow(g_stat.target)) g_stat.target = nullptr;
    HKL cur = g_stat.target ? GetKeyboardLayout(GetWindowThreadProcessId(g_stat.target, nullptr)) : GetKeyboardLayout(0);
    int n = GetKeyboardLayoutList(0, nullptr);
    std::vector<HKL> list(std::max(0, n));
    if (n > 0) GetKeyboardLayoutList(n, list.data());
    // After a switch made here, the window may not report it yet (a window run as administrator only
    // takes it with its next input): keep showing our choice until what is read really changes.
    if (cur == g_stat.lastRead && g_stat.target == g_stat.lastTarget && g_stat.cur) {
        cur = g_stat.cur;
    } else {
        g_stat.lastRead = cur;
        g_stat.lastTarget = g_stat.target;
    }
    bool changed = cur != g_stat.cur || list != g_stat.layouts;
    g_stat.cur = cur;
    g_stat.layouts = list;
    return changed;
}

// What the dock shows: the language ("PT"), or the layout's own short name when two layouts share the
// language (Portuguese ABNT and ABNT2 would both be "PT").
// The layout's own short name: the last word of its name ("ABNT2"), or, when that is long
// ("Internacional"), the country it was made for ("US").
static std::wstring KbLayoutShort(const KbInfo& k) {
    std::wstring t = k.layout;
    while (!t.empty() && (t.back() == L')' || t.back() == L' ')) t.pop_back();
    size_t cut = t.find_last_of(L" (-");
    if (cut != std::wstring::npos) t = t.substr(cut + 1);
    if (t.empty() || t.size() > 6) {
        t.clear();
        if (k.klid.size() == 8) {
            LANGID lid = (LANGID)wcstoul(k.klid.c_str() + 4, nullptr, 16);
            wchar_t c[16] = {};
            if (GetLocaleInfoW(MAKELCID(lid, SORT_DEFAULT), LOCALE_SISO3166CTRYNAME, c, ARRAYSIZE(c))) t = c;
        }
    }
    for (auto& c : t) c = towupper(c);
    return t;
}

static std::wstring KbShort(HKL hkl) {
    const KbInfo& k = KbOf(hkl);
    bool twin = false;
    for (HKL o : g_stat.layouts)
        if (o != hkl && KbOf(o).code == k.code) twin = true;
    if (!twin) return k.code;
    std::wstring t = KbLayoutShort(k);
    bool clash = t.empty();
    for (HKL o : g_stat.layouts)
        if (o != hkl && KbOf(o).code == k.code && KbLayoutShort(KbOf(o)) == t) clash = true;
    if (clash) {
        auto it = std::find(g_stat.layouts.begin(), g_stat.layouts.end(), hkl);
        return k.code + std::to_wstring(it - g_stat.layouts.begin() + 1);
    }
    return t;
}

// Switches through Windows' text input service, for the whole session (like Win+Space): it also reaches
// windows run as administrator, which ignore messages from Nimbus. The message is a fallback for setups
// where each window keeps its own layout.
static void SetKeyboard(HKL hkl) {
    if (!g_stat.tsf)
        CoCreateInstance(CLSID_TF_InputProcessorProfiles, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_stat.tsf));
    HRESULT hr = E_FAIL;
    if (g_stat.tsf)
        hr = g_stat.tsf->ActivateProfile(TF_PROFILETYPE_KEYBOARDLAYOUT, LOWORD((UINT_PTR)hkl), CLSID_NULL, GUID_NULL, hkl,
                                         TF_IPPMF_FORSESSION | TF_IPPMF_DONTCARECURRENTINPUTLANGUAGE);
    if (g_stat.target) PostMessageW(g_stat.target, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)hkl);
    Wh_Log(L"keyboard: switch to %p (tsf hr=0x%08lx, target %p)", hkl, (unsigned long)hr, g_stat.target);
    g_stat.cur = hkl;
}

static void NextKeyboard() {
    if (g_stat.layouts.size() < 2) return;
    auto it = std::find(g_stat.layouts.begin(), g_stat.layouts.end(), g_stat.cur);
    size_t i = it == g_stat.layouts.end() ? 0 : (size_t)(it - g_stat.layouts.begin() + 1) % g_stat.layouts.size();
    SetKeyboard(g_stat.layouts[i]);
}

// ---- Text ----

static std::wstring StatLabel(int id) {
    wchar_t b[256];
    switch (id) {
        case StKeyboard:
            swprintf_s(b, L2(L"Teclado: %s. Clique troca", L"Keyboard: %s. Click switches"),
                      g_stat.cur ? KbOf(g_stat.cur).name.c_str() : L"?");
            return b;
        case StNetwork:
            if (g_stat.net == 2)
                swprintf_s(b, L"Wi-Fi: %s%s", g_stat.wifiName.c_str(), g_stat.internet ? L"" : L2(L", sem internet", L", no internet"));
            else if (g_stat.net == 1)
                wcscpy_s(b, g_stat.internet ? L2(L"Rede: cabo, com internet", L"Network: cable, connected")
                                            : L2(L"Rede: cabo, sem internet", L"Network: cable, no internet"));
            else
                wcscpy_s(b, L2(L"Sem conexão", L"Not connected"));
            return b;
        case StVolume:
            if (!g_stat.audio) return L2(L"Sem saída de som", L"No sound output");
            if (g_stat.muted) return L2(L"Volume: mudo", L"Volume: muted");
            swprintf_s(b, L"Volume %d%%", (int)std::lround(g_stat.vol * 100));
            return b;
        case StBattery:
            swprintf_s(b, L2(L"Bateria %d%%%s", L"Battery %d%%%s"), g_stat.pct,
                       g_stat.charging ? L2(L", carregando", L", charging") : g_stat.plugged ? L2(L", na tomada", L", plugged in") : L"");
            return b;
        case StClock:
            return g_stat.dateLong;
        case StCentral:
            return L2(L"Central de controle", L"Control center");
        case StDnd:
            return g_set.notifDnd ? L2(L"Não incomodar: ligado", L"Do not disturb: on") : L2(L"Não incomodar: desligado", L"Do not disturb: off");
        case StBt:
            return L"Bluetooth";
        case StNight:
            return L2(L"Luz noturna", L"Night light");
        case StShort:
            return L2(L"Atalhos", L"Shortcuts");
        case StLight:
            return L2(L"Brilho", L"Brightness");
        case StPower:
            return L2(L"Energia", L"Power");
        case StDevices: {
            static std::wstring tip;
            tip.clear();
            for (auto& dv : DevicesNow())
                if (dv.percent >= 0) tip += (tip.empty() ? L"" : L" · ") + dv.name + L" " + std::to_wstring(dv.percent) + L"%";
            if (tip.empty()) tip = L2(L"Dispositivos: nenhuma bateria", L"Devices: no batteries");
            return tip.c_str();
        }
        case StPlayer: {
            MediaInfo m = MediaNow();
            if (!m.has) return L2(L"Nada tocando", L"Nothing playing");
            static std::wstring tip;
            tip = m.title + (m.artist.empty() ? L"" : L" — " + m.artist);
            return tip.c_str();
        }
        case StNotes:
            if (!RemPending()) return L2(L"Notas e lembretes", L"Notes and reminders");
            swprintf_s(b, L2(L"Notas e lembretes: %d lembretes", L"Notes and reminders: %d reminders"), RemPending());
            return b;
        case StNotif:
            if (g_notifs.empty()) return L2(L"Notificações", L"Notifications");
            swprintf_s(b, L2(L"Notificações: %d", L"Notifications: %d"), (int)g_notifs.size());
            return b;
        case StShelf:
            if (g_shelf.empty()) return L2(L"Prateleira", L"Shelf");
            swprintf_s(b, L2(L"Prateleira: %d", L"Shelf: %d"), (int)g_shelf.size());
            return b;
    }
    return L"";
}

// ---- Line icons (24x24 box, like the design) ----

static const wchar_t* WifiIcon(int signal) {
    if (signal >= 67) return L"M2.5 9a14 14 0 0 1 19 0 M5.5 12.5a9.5 9.5 0 0 1 13 0 M8.5 16a5 5 0 0 1 7 0 M12 19.5v.5";
    if (signal >= 34) return L"M5.5 12.5a9.5 9.5 0 0 1 13 0 M8.5 16a5 5 0 0 1 7 0 M12 19.5v.5";
    return L"M8.5 16a5 5 0 0 1 7 0 M12 19.5v.5";
}

static const wchar_t* StatIcon(int id) {
    switch (id) {
        case StShelf:
            return L"M3 13h18v6H3z M6 13V6h5v7 M13 13V9h5v4";
        case StNotes:
            return L"M6 3h9l4 4v14H6z M14 3v5h5 M9 12h7 M9 16h5";
        case StCentral:
            return L"M4 7h10 M18 7h2 M16 5v4 M4 17h4 M12 17h8 M10 15v4";
        case StDnd:
            return L"M20 14.5A8 8 0 1 1 9.5 4a6.5 6.5 0 0 0 10.5 10.5z";
        case StBt:
            return L"M7 7l10 10-5 4V3l5 4L7 17";
        case StNight:
            return L"M12 8a4 4 0 1 1 0 8a4 4 0 1 1 0-8z M12 2v2 M12 20v2 M4.9 4.9l1.4 1.4 M17.7 17.7l1.4 1.4 M2 12h2 M20 12h2 M4.9 19.1l1.4-1.4 M17.7 6.3l1.4-1.4";
        case StShort:
            return L"M4 4h6v6H4z M14 4h6v6h-6z M4 14h6v6H4z M17 14v6 M14 17h6";
        case StLight:
            return L"M12 7a5 5 0 1 1 0 10a5 5 0 1 1 0-10z M12 1v2 M12 21v2 M4.2 4.2l1.4 1.4 M18.4 18.4l1.4 1.4 M1 12h2 M21 12h2";
        case StPower:
            return L"M13 3L5 13h6l-1 8 8-10h-6z";
        case StPlayer:
            return L"M9 18V5l11-2v13 M9 18a3 3 0 1 1-6 0a3 3 0 1 1 6 0z M20 16a3 3 0 1 1-6 0a3 3 0 1 1 6 0z";
        case StDevices:
            return L"M12 3a5 5 0 0 1 5 5v8a5 5 0 0 1-10 0V8a5 5 0 0 1 5-5z M12 3v6 M7 10h10";
        case StNotif:
            return g_set.notifShow ? L"M6 16V11a6 6 0 0 1 12 0v5l1.5 2h-15z M10 20.5a2 2 0 0 0 4 0"
                                   : L"M6 16V11a6 6 0 0 1 12 0v5l1.5 2h-15z M10 20.5a2 2 0 0 0 4 0 M4 4l16 16";
        case StVolume:
            if (!g_stat.audio || g_stat.muted || g_stat.vol < 0.005f) return L"M4 9h4l5-4v14l-5-4H4z M17 9.5l5 5 M22 9.5l-5 5";
            if (g_stat.vol < 0.5f) return L"M4 9h4l5-4v14l-5-4H4z M16.5 8.5a5 5 0 0 1 0 7";
            return L"M4 9h4l5-4v14l-5-4H4z M16.5 8.5a5 5 0 0 1 0 7 M19 6a8.5 8.5 0 0 1 0 12";
        case StNetwork:
            if (g_stat.net == 1) return L"M3 5h18v11H3z M8 20h8 M12 16v4";
            if (g_stat.net == 0) return g_stat.hasWifi ? L"M3 3l18 18 M8.5 16a5 5 0 0 1 7 0 M12 19.5v.5 M5.5 12.5a9.5 9.5 0 0 1 4-2.4"
                                                       : L"M3 5h18v11H3z M8 20h8 M12 16v4 M9 8l6 5 M15 8l-6 5";
            return WifiIcon(g_stat.signal);
        case StBattery:
            return L"M3 8h15v8H3z M18 11h2v2h-2";
    }
    return L"";
}

// ---- Tray (the notification area's icons) ----
// Shell_NotifyIcon only reaches Explorer (Shell_TrayWnd gets each call as WM_COPYDATA): the explorer.exe
// part forwards every change here (see ExTraySubclass), and clicks go straight to the app's callback
// message, like the taskbar does.

constexpr ULONG_PTR kTrayCopy = 0x4E425452;  // an icon change (TrayWire)
constexpr ULONG_PTR kTrayRect = 0x4E425243;  // Shell_NotifyIconGetRect: where is this icon? (TrayRectWire)
constexpr DWORD kTrayKnown = 0x80;           // TrayWire.op: an icon's whole known state (a replay)
constexpr UINT_PTR TIMER_TRAY_SAVE = 5;
constexpr UINT_PTR TIMER_TRAY_ASK = 6;

#pragma pack(push, 1)
struct TrayWire {
    DWORD op;  // NIM_ADD, NIM_MODIFY, NIM_DELETE, NIM_SETVERSION or kTrayKnown
    DWORD hwnd;
    UINT uid, flags, cbMsg;
    DWORD hicon;
    DWORD state, stateMask;
    UINT version;
    GUID guid;
    WCHAR tip[128];
};
struct TrayRectWire {
    DWORD hwnd;
    UINT uid;
    GUID guid;
    DWORD which;  // 1 the top-left corner, 2 the bottom-right one (one corner fits any LRESULT)
};
#pragma pack(pop)

// 32-bit handle values sign-extend to the 64-bit ones.
static HWND WireHwnd(DWORD v) { return (HWND)(LONG_PTR)(LONG)v; }
static HICON WireIcon(DWORD v) { return (HICON)(LONG_PTR)(LONG)v; }

// Windows' own icons (volume, network, power...): the status area already has them.
static bool IsWindowsTrayGuid(const GUID& g) { return (g.Data1 & 0xFFFFFFF0) == 0x7820AE70 && g.Data2 == 0x23E3; }

// The dock's tray pieces (StatItem ids, after the status items').
enum { StTrayIcon = 50, StTrayBtn, StTrayPrev, StTrayNext, StTrayStrip };
static bool IsTrayKind(int id) { return id >= StTrayIcon && id <= StTrayStrip; }

struct TrayIcon {
    HWND hwnd = nullptr;
    UINT uid = 0, cbMsg = 0, version = 0;
    GUID guid{};
    bool hasGuid = false, hidden = false;
    ComPtr<IWICBitmapSource> icon;
    int iconVer = 0;
    std::wstring tip, path, name, key;
    DWORD pid = 0;
    UINT seq = 0;  // arrival order
};
std::vector<TrayIcon> g_tray;
std::vector<std::wstring> g_trayPinned;  // keys of the pinned icons, in the order they were pinned
struct TrayUi {
    int off = 0;        // first icon shown in the strip
    bool open = false;  // "button" style: the icons are out
    UINT seq = 0;
    int iconVer = 0;
    DWORD lastClick = 0;  // (double clicks)
    std::wstring lastKey;
    UINT replayMsg = 0;
    bool published = false;
} g_trayUi;

static void StatusChanged();

static bool TrayOn() { return g_set.trayPlace != 0; }  // listening (in the dock or in Start)
static bool TrayInDock() { return (g_set.trayPlace & PlDock); }
static bool TrayInStart() { return (g_set.trayPlace & PlStart); }
static bool TrayStyle(const wchar_t* st) { return g_set.trayStyle == st; }

static std::wstring TrayAppName(const std::wstring& path) {
    static std::unordered_map<std::wstring, std::wstring> names;
    auto it = names.find(path);
    if (it != names.end()) return it->second;
    std::wstring n = path.empty() ? L"" : FileDescription(path);
    if (n.empty()) {
        size_t a = path.find_last_of(L'\\');
        n = a == std::wstring::npos ? path : path.substr(a + 1);
        size_t dot = n.find_last_of(L'.');
        if (dot != std::wstring::npos) n = n.substr(0, dot);
    }
    names[path] = n;
    return n;
}

static std::wstring TrayKey(const TrayIcon& t) {
    size_t a = t.path.find_last_of(L'\\');
    std::wstring exe = Lower(a == std::wstring::npos ? t.path : t.path.substr(a + 1));
    if (t.hasGuid) {
        wchar_t g[40] = {};
        StringFromGUID2(t.guid, g, 40);
        return exe + L":" + g;
    }
    return exe + L":" + std::to_wstring(t.uid);
}

static ComPtr<IWICBitmapSource> TrayBitmap(HICON h) {
    ComPtr<IWICBitmap> wb;
    if (!h || !g_wic || FAILED(g_wic->CreateBitmapFromHICON(h, &wb))) return {};
    ComPtr<IWICFormatConverter> conv;
    if (FAILED(g_wic->CreateFormatConverter(&conv))) return {};
    if (FAILED(conv->Initialize(wb.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0,
                                WICBitmapPaletteTypeCustom))) return {};
    ComPtr<IWICBitmap> out;
    if (FAILED(g_wic->CreateBitmapFromSource(conv.Get(), WICBitmapCacheOnLoad, &out))) return {};
    return out;
}

static int TrayFind(HWND hwnd, UINT uid, const GUID* guid) {
    for (int i = 0; i < (int)g_tray.size(); i++) {
        const TrayIcon& t = g_tray[i];
        if (guid ? (t.hasGuid && IsEqualGUID(t.guid, *guid)) : (t.hwnd == hwnd && t.uid == uid)) return i;
    }
    return -1;
}

static int TrayByKey(const std::wstring& key) {
    for (int i = 0; i < (int)g_tray.size(); i++)
        if (g_tray[i].key == key) return i;
    return -1;
}

static TrayIcon NewTrayIcon(HWND h, UINT uid, const GUID* guid) {
    TrayIcon t;
    t.hwnd = h;
    t.uid = uid;
    if (guid) {
        t.hasGuid = true;
        t.guid = *guid;
    }
    GetWindowThreadProcessId(h, &t.pid);
    t.path = ProcessPath(t.pid);
    t.name = TrayAppName(t.path);
    t.key = TrayKey(t);
    t.seq = ++g_trayUi.seq;
    return t;
}

// One forwarded change; true when what the tray shows changed.
static bool TrayApply(const TrayWire& w) {
    const GUID* guid = (w.flags & NIF_GUID) ? &w.guid : nullptr;
    if (guid && IsWindowsTrayGuid(*guid)) return false;
    HWND h = WireHwnd(w.hwnd);
    int i = TrayFind(h, w.uid, guid);
    if (w.op == NIM_DELETE) {
        if (i < 0) return false;
        g_tray.erase(g_tray.begin() + i);
        return true;
    }
    if (w.op == NIM_SETVERSION) {
        if (i >= 0) g_tray[i].version = w.version;
        return false;
    }
    if (w.op != NIM_ADD && w.op != NIM_MODIFY && w.op != kTrayKnown) return false;
    if (i < 0) {
        // (An icon added before Nimbus was listening becomes known when it next changes its picture.)
        if (w.op == NIM_MODIFY && !(w.flags & NIF_ICON)) return false;
        if (!IsWindow(h)) return false;
        g_tray.push_back(NewTrayIcon(h, w.uid, guid));
        i = (int)g_tray.size() - 1;
    }
    TrayIcon& t = g_tray[i];
    if (w.flags & NIF_MESSAGE) t.cbMsg = w.cbMsg;
    if (w.flags & NIF_ICON) {
        t.icon = TrayBitmap(WireIcon(w.hicon));
        t.iconVer = ++g_trayUi.iconVer;
    }
    if (w.flags & NIF_TIP) t.tip.assign(w.tip, wcsnlen(w.tip, ARRAYSIZE(w.tip)));
    if ((w.flags & NIF_STATE) && (w.stateMask & NIS_HIDDEN)) t.hidden = (w.state & NIS_HIDDEN) != 0;
    if (w.op == kTrayKnown) t.version = w.version;
    return true;
}

static bool TrayIsPinned(const TrayIcon& t) {
    return std::find(g_trayPinned.begin(), g_trayPinned.end(), t.key) != g_trayPinned.end();
}

// The order dragged by hand (the icons not pinned), by key.
std::vector<std::wstring> g_trayOrderKeys;

// The icons to show: pinned first (in pin order), then in the dragged order, then by arrival.
static std::vector<int> TrayOrder() {
    std::vector<int> v;
    for (int i = 0; i < (int)g_tray.size(); i++)
        if (!g_tray[i].hidden && g_tray[i].icon) v.push_back(i);
    auto rank = [](const TrayIcon& t) {
        auto it = std::find(g_trayPinned.begin(), g_trayPinned.end(), t.key);
        return it == g_trayPinned.end() ? INT_MAX : (int)(it - g_trayPinned.begin());
    };
    auto pos = [](const TrayIcon& t) {
        auto it = std::find(g_trayOrderKeys.begin(), g_trayOrderKeys.end(), t.key);
        return it == g_trayOrderKeys.end() ? INT_MAX : (int)(it - g_trayOrderKeys.begin());
    };
    std::stable_sort(v.begin(), v.end(), [&](int a, int b) {
        int ra = rank(g_tray[a]), rb = rank(g_tray[b]);
        if (ra != rb) return ra < rb;
        int pa = pos(g_tray[a]), pb = pos(g_tray[b]);
        return pa != pb ? pa < pb : g_tray[a].seq < g_tray[b].seq;
    });
    return v;
}
// The icons in the scrolling strip ("button" style: the pinned ones stay outside it).
static std::vector<int> TrayStripList() {
    std::vector<int> v = TrayOrder();
    if (TrayStyle(L"botao")) v.erase(std::remove_if(v.begin(), v.end(), [](int i) { return TrayIsPinned(g_tray[i]); }), v.end());
    return v;
}
static std::vector<int> TrayFixedList() {
    std::vector<int> v;
    if (!TrayStyle(L"botao")) return v;
    for (int i : TrayOrder())
        if (TrayIsPinned(g_tray[i])) v.push_back(i);
    return v;
}
static int TrayMaxOff() { return std::max(0, (int)TrayStripList().size() - g_set.trayVisible); }

static void SaveTrayPins();
static void SaveTrayOrder() { StoreItems(L"tray.order", g_trayOrderKeys); }

// Dragging an icon onto another: it goes before it (after, past its middle), and joins its group
// (pinned or not).
static void TrayMove(const std::wstring& drag, const std::wstring& target, bool after) {
    if (drag == target) return;
    bool tPinned = std::find(g_trayPinned.begin(), g_trayPinned.end(), target) != g_trayPinned.end();
    auto& list = tPinned ? g_trayPinned : g_trayOrderKeys;
    if (!tPinned) {
        // (the whole visible order of the unpinned ones, so the move is exact)
        std::vector<std::wstring> keys;
        for (int i : TrayOrder())
            if (std::find(g_trayPinned.begin(), g_trayPinned.end(), g_tray[i].key) == g_trayPinned.end()) keys.push_back(g_tray[i].key);
        for (auto& k : g_trayOrderKeys)
            if (std::find(keys.begin(), keys.end(), k) == keys.end()) keys.push_back(k);
        g_trayOrderKeys = keys;
    }
    g_trayPinned.erase(std::remove(g_trayPinned.begin(), g_trayPinned.end(), drag), g_trayPinned.end());
    g_trayOrderKeys.erase(std::remove(g_trayOrderKeys.begin(), g_trayOrderKeys.end(), drag), g_trayOrderKeys.end());
    auto it = std::find(list.begin(), list.end(), target);
    if (it == list.end()) list.push_back(drag);
    else list.insert(after ? it + 1 : it, drag);
    if (g_trayOrderKeys.size() > 120) g_trayOrderKeys.resize(120);
    SaveTrayPins();
    SaveTrayOrder();
}

static void LoadTrayPins() {
    LoadItems(L"tray.order", L'*', g_trayOrderKeys);
    LoadItems(L"tray.pinned", L'*', g_trayPinned);
}
static void SaveTrayPins() { StoreItems(L"tray.pinned", g_trayPinned); }
static void TrayTogglePin(int i) {
    if (i < 0 || i >= (int)g_tray.size()) return;
    auto it = std::find(g_trayPinned.begin(), g_trayPinned.end(), g_tray[i].key);
    if (it != g_trayPinned.end()) g_trayPinned.erase(it);
    else g_trayPinned.push_back(g_tray[i].key);
    SaveTrayPins();
}

// ---- Clicks: to the icon's callback message, as the taskbar sends them ----

static void TraySend(const TrayIcon& t, UINT m, POINT pt) {
    if (!t.cbMsg) return;
    WPARAM wp;
    LPARAM lp;
    if (t.version >= 4) {
        wp = MAKEWPARAM((WORD)(SHORT)pt.x, (WORD)(SHORT)pt.y);
        lp = MAKELPARAM(m, t.uid);
    } else {
        wp = t.uid;
        lp = m;
    }
    PostMessageW(t.hwnd, t.cbMsg, wp, lp);
}

static void TrayHover(int i) {
    if (i < 0 || i >= (int)g_tray.size()) return;
    POINT pt;
    GetCursorPos(&pt);
    TraySend(g_tray[i], WM_MOUSEMOVE, pt);
}

// button: 0 left, 1 right.
static void TrayClick(int i, int button) {
    if (i < 0 || i >= (int)g_tray.size()) return;
    const TrayIcon& t = g_tray[i];
    POINT pt;
    GetCursorPos(&pt);
    // The app's menu or window comes to the front (Nimbus has the right: the click was on it).
    AllowSetForegroundWindow(t.pid);
    if (button == 1) {
        TraySend(t, WM_RBUTTONDOWN, pt);
        TraySend(t, WM_RBUTTONUP, pt);
        if (t.version >= 3) TraySend(t, WM_CONTEXTMENU, pt);
    } else {
        DWORD now = GetTickCount();
        bool dbl = g_trayUi.lastKey == t.key && now - g_trayUi.lastClick <= GetDoubleClickTime();
        g_trayUi.lastKey = dbl ? L"" : t.key;
        g_trayUi.lastClick = now;
        TraySend(t, dbl ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN, pt);
        TraySend(t, WM_LBUTTONUP, pt);
        if (t.version >= 3 && !dbl) TraySend(t, NIN_SELECT, pt);
    }
    Wh_Log(L"tray: %s click on %s (v%u, cb 0x%x)", button == 1 ? L"right" : g_trayUi.lastKey.empty() ? L"double" : L"left",
        t.key.c_str(), t.version, t.cbMsg);
}

// A left click on a tray icon just happened: a second one may follow (a double click), so the Start or
// the panel it's in stays open a moment, even if the app's window takes the focus meanwhile.
static bool TrayClickJustNow() { return GetTickCount() - g_trayUi.lastClick < GetDoubleClickTime() + 100; }

// ---- Kept between runs: the icons of apps already running when Nimbus (or the mod) restarts ----

// This user's folder in the mod's storage (that folder is shared by every account on the PC): named
// by the SID, like the stored values. Made when asked for.
static std::wstring UserDir(bool create) {
    wchar_t base[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(base, ARRAYSIZE(base)) || !base[0]) return L"";
    std::wstring sid = UserPrefix();
    if (!sid.empty() && sid.back() == L'_') sid.pop_back();
    std::wstring dir = base;
    if (dir.back() != L'\\') dir += L'\\';
    dir += sid;
    if (create) SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    return dir;
}

static std::wstring TrayFile() {
    std::wstring dir = UserDir(true);
    if (dir.empty()) return L"";
    std::wstring file = dir + L"\\tray.bin";
    // (earlier builds kept it in the shared folder: taken over once, by the first user who starts)
    wchar_t base[MAX_PATH] = {};
    if (GetFileAttributesW(file.c_str()) == INVALID_FILE_ATTRIBUTES && Wh_GetModStoragePath(base, ARRAYSIZE(base))) {
        std::wstring old = std::wstring(base) + L"\\tray.bin";
        MoveFileExW(old.c_str(), file.c_str(), 0);
    }
    return file;
}

static void SaveTray() {
    std::wstring file = TrayFile();
    if (file.empty()) return;
    std::vector<BYTE> out;
    auto put = [&](const void* p, size_t n) {
        if (n) out.insert(out.end(), (const BYTE*)p, (const BYTE*)p + n);
    };
    auto putU = [&](UINT32 v) { put(&v, 4); };
    auto putS = [&](const std::wstring& str) {
        putU((UINT32)str.size());
        put(str.data(), str.size() * 2);
    };
    putU(0x3154424E);
    putU((UINT32)g_tray.size());
    for (auto& t : g_tray) {
        UINT64 h = (UINT64)(ULONG_PTR)t.hwnd;
        put(&h, 8);
        putU(t.uid);
        put(&t.guid, sizeof(GUID));
        putU(t.hasGuid);
        putU(t.cbMsg);
        putU(t.version);
        putU(t.hidden);
        putU(t.seq);
        putS(t.tip);
        putS(t.path);
        UINT w = 0, hh = 0;
        std::vector<BYTE> px;
        if (t.icon && SUCCEEDED(t.icon->GetSize(&w, &hh)) && w && hh && w <= 256 && hh <= 256) {
            px.resize((size_t)w * hh * 4);
            if (FAILED(t.icon->CopyPixels(nullptr, w * 4, (UINT)px.size(), px.data()))) {
                w = hh = 0;
                px.clear();
            }
        } else {
            w = hh = 0;
        }
        putU(w);
        putU(hh);
        put(px.data(), px.size());
    }
    HANDLE f = CreateFileW(file.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD wr = 0;
    WriteFile(f, out.data(), (DWORD)out.size(), &wr, nullptr);
    CloseHandle(f);
}

static void LoadTray() {
    std::wstring file = TrayFile();
    if (file.empty()) return;
    HANDLE f = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    LARGE_INTEGER size = {};
    std::vector<BYTE> in;
    if (GetFileSizeEx(f, &size) && size.QuadPart > 0 && size.QuadPart < 16 * 1024 * 1024) {
        in.resize((size_t)size.QuadPart);
        DWORD rd = 0;
        if (!ReadFile(f, in.data(), (DWORD)in.size(), &rd, nullptr) || rd != in.size()) in.clear();
    }
    CloseHandle(f);
    size_t pos = 0;
    auto get = [&](void* p, size_t n) {
        if (pos + n > in.size()) return false;
        memcpy(p, in.data() + pos, n);
        pos += n;
        return true;
    };
    auto getU = [&](UINT32& v) { return get(&v, 4); };
    auto getS = [&](std::wstring& str) {
        UINT32 n = 0;
        if (!getU(n) || n > 4096 || pos + n * 2 > in.size()) return false;
        str.assign((const wchar_t*)(in.data() + pos), n);
        pos += n * 2;
        return true;
    };
    UINT32 magic = 0, count = 0;
    if (!getU(magic) || magic != 0x3154424E || !getU(count)) return;
    int restored = 0;
    for (UINT32 k = 0; k < count && k < 256; k++) {
        UINT64 h = 0;
        UINT32 uid, hasGuid, cbMsg, version, hidden, seq, w, hh;
        GUID guid;
        std::wstring tip, path;
        if (!get(&h, 8) || !getU(uid) || !get(&guid, sizeof(GUID)) || !getU(hasGuid) || !getU(cbMsg) || !getU(version) ||
            !getU(hidden) || !getU(seq) || !getS(tip) || !getS(path) || !getU(w) || !getU(hh))
            return;
        size_t bytes = (size_t)w * hh * 4;
        if (w > 256 || hh > 256 || pos + bytes > in.size()) return;
        const BYTE* px = in.data() + pos;
        pos += bytes;
        HWND hwnd = (HWND)(ULONG_PTR)h;
        // Only while that window still exists and belongs to the same program (handles are reused).
        if (!IsWindow(hwnd)) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (_wcsicmp(ProcessPath(pid).c_str(), path.c_str())) continue;
        if (TrayFind(hwnd, uid, hasGuid ? &guid : nullptr) >= 0) continue;
        TrayIcon t = NewTrayIcon(hwnd, uid, hasGuid ? &guid : nullptr);
        t.cbMsg = cbMsg;
        t.version = version;
        t.hidden = hidden != 0;
        t.tip = tip;
        if (bytes) {
            ComPtr<IWICBitmap> bmp;
            if (SUCCEEDED(g_wic->CreateBitmapFromMemory(w, hh, GUID_WICPixelFormat32bppPBGRA, w * 4, (UINT)bytes, (BYTE*)px, &bmp)))
                t.icon = bmp;
            t.iconVer = ++g_trayUi.iconVer;
        }
        g_tray.push_back(std::move(t));
        restored++;
    }
    Wh_Log(L"tray: %d icons kept from the last run", restored);
}

static void TrayChanged() {
    g_trayUi.off = std::min(g_trayUi.off, TrayMaxOff());
    StatusChanged();
    if (g_msgWnd) SetTimer(g_msgWnd, TIMER_TRAY_SAVE, 30000, nullptr);  // (icon pixels too: not after every change)
}

// Apps that quit without removing their icon.
static void TrayPrune() {
    size_t before = g_tray.size();
    g_tray.erase(std::remove_if(g_tray.begin(), g_tray.end(), [](const TrayIcon& t) { return !IsWindow(t.hwnd); }), g_tray.end());
    if (g_tray.size() != before) TrayChanged();
}

// Tell the explorer.exe part whether to forward the tray; when it starts to, it replays what it knows.
static void TrayPublish() {
    if (!g_msgWnd) return;
    if (TrayOn()) {
        SetPropW(g_msgWnd, L"NimbusTray", (HANDLE)1);
        if (!g_trayUi.published) {
            g_trayUi.published = true;
            if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) PostMessageW(tray, g_trayUi.replayMsg, 0, 0);
            SetTimer(g_msgWnd, TIMER_TRAY_ASK, 2500, nullptr);
        }
    } else {
        RemovePropW(g_msgWnd, L"NimbusTray");
        g_trayUi.published = false;
        g_trayUi.open = false;
    }
}

// Icons added before anything was listening (the mod just turned on): Windows lists the programs that
// have tray icons; the running ones Nimbus doesn't know yet get "TaskbarCreated" (what a restarted
// Explorer sends), so they add their icon again. Only to those programs, never to every window:
// a broadcast makes other tray mods add everything again, and Task Manager adds a new icon each time.
static void TrayAskMissing() {
    if (!TrayOn()) return;
    static std::unordered_set<std::wstring> asked;  // (once per program, while Nimbus runs)
    std::unordered_set<std::wstring> exes;
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\NotifyIconSettings", 0, KEY_READ, &k) == ERROR_SUCCESS) {
        wchar_t sub[128];
        for (DWORD i = 0;; i++) {
            DWORD n = ARRAYSIZE(sub);
            if (RegEnumKeyExW(k, i, sub, &n, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
            wchar_t path[1024] = {};
            DWORD bytes = sizeof(path);
            if (RegGetValueW(k, sub, L"ExecutablePath", RRF_RT_REG_SZ, nullptr, path, &bytes) != ERROR_SUCCESS) continue;
            const wchar_t* name = wcsrchr(path, L'\\');
            exes.insert(Lower(name ? name + 1 : path));
        }
        RegCloseKey(k);
    }
    for (const wchar_t* skip : {L"explorer.exe", L"taskmgr.exe", L"vgtray.exe"}) exes.erase(skip);
    struct Ctx {
        std::unordered_set<std::wstring>* exes;
        std::unordered_set<DWORD> known;
        std::unordered_map<DWORD, std::wstring> exeOf;
        std::unordered_set<std::wstring> hit;
        UINT msg;
        int posted = 0;
    } ctx;
    ctx.exes = &exes;
    for (auto& t : g_tray) ctx.known.insert(t.pid);
    ctx.msg = RegisterWindowMessageW(L"TaskbarCreated");
    EnumWindows(
        [](HWND h, LPARAM lp) WINAPI -> BOOL {
            auto& c = *(Ctx*)lp;
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (!pid || c.known.count(pid)) return TRUE;
            auto it = c.exeOf.find(pid);
            if (it == c.exeOf.end()) {
                std::wstring path = ProcessPath(pid);
                size_t a = path.find_last_of(L'\\');
                it = c.exeOf.emplace(pid, Lower(a == std::wstring::npos ? path : path.substr(a + 1))).first;
            }
            if (it->second.empty() || !c.exes->count(it->second) || asked.count(it->second)) return TRUE;
            PostMessageW(h, c.msg, 0, 0);
            c.hit.insert(it->second);
            c.posted++;
            return TRUE;
        },
        (LPARAM)&ctx);
    std::wstring names;
    for (auto& e : ctx.hit) {
        asked.insert(e);
        names += L" " + e;
    }
    Wh_Log(L"tray: asked %d windows to add their icon again:%s", ctx.posted, names.c_str());
}

static void TrayInit() {
    g_trayUi.replayMsg = RegisterWindowMessageW(L"NimbusMenu_TrayReplay");
    LoadTrayPins();
    LoadTray();
    TrayPublish();
}

static void TrayUninit() {
    if (g_msgWnd) {
        KillTimer(g_msgWnd, TIMER_TRAY_SAVE);
        KillTimer(g_msgWnd, TIMER_TRAY_ASK);
        RemovePropW(g_msgWnd, L"NimbusTray");
    }
    SaveTray();
    g_tray.clear();
}

struct StatItem {
    int id = 0;
    float x = 0, w = 0;
    int sub = -1;  // StTrayIcon: the g_tray index
};

struct Dock {
    DockCfg cfg;
    float fit = 1;  // < 1 when the icons don't fit this monitor's width: they shrink here only
    HMONITOR mon = nullptr;
    RECT rcMon{};
    float scale = 1;
    std::vector<HWND> bands;   // blur windows (see PlaceDock), bands[0] owns the content window
    std::vector<RECT> bandRc;
    HWND content = nullptr;
    int cw = 0, ch = 0;
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    ComPtr<ID2D1DCRenderTarget> rt;
    ComPtr<ID2D1DeviceContext> dc;
    ComPtr<IDWriteTextFormat> font;
    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> bmps;
    std::vector<Elem> elems;
    float mouseX = -1;   // content coords, -1 = outside
    int hoverElem = -1;
    float shown = 0;     // 0 hidden .. 1 shown
    bool wantShown = false;
    bool visible = false;
    bool animating = false;
    bool menuOpen = false;
    bool fullscreen = false;
    DWORD outsideSince = 0;
    DWORD bandsAt = 0;          // the blur bands were last moved (see PlaceDock)
    bool bandsPending = false;
    D2D1_RECT_F pill{};
    bool appbar = false;  // fixed bar, always shown: reserves its strip so maximized windows stop above it
    ComPtr<IDWriteTextFormat> fontBold;
    std::vector<StatItem> stat;  // the status area's items (see BuildStat)
    float statW = 0;
    int statHover = -1;
    ComPtr<IDWriteTextFormat> fontTiny;  // the tray button's count
    float trayPos = 0, trayVel = 0;      // the strip's scroll, in icons (a spring toward g_trayUi.off)
    int trayHot = -1;                    // g_tray index under the mouse
};
std::vector<Dock*> g_docks;

// Dragging an icon to reorder, pin (from the open apps) or unpin (dropped above the dock).
struct Drag {
    bool down = false, active = false;
    Dock* dock = nullptr;
    std::wstring key;
    int sx = 0, sy = 0;
    float x = 0, y = 0;
} g_drag;

static float S(const Dock& d) { return g_set.iconSize * d.scale * d.fit; }
static float Gap(const Dock& d) { return g_set.iconGap * d.scale * d.fit; }
static float PadY(const Dock& d) { return 10 * d.scale; }
static float DotRoom(const Dock& d) { return 7 * d.scale; }
static float PillH(const Dock& d) { return S(d) + PadY(d) * 2 + DotRoom(d); }
static float Zoom() { return g_set.magnification / 100.0f; }
static float PillRadius(const Dock& d) { return d.cfg.fixed ? 0.0f : std::min(PillH(d) / 2, S(d) * 0.42f); }
static float BottomGap(const Dock& d) { return d.cfg.fixed ? 0.0f : g_set.bottomGap * d.scale; }

// An app's windows as this dock shows them (all, or only the ones on its monitor).
static std::vector<HWND> DockWins(const Dock& d, const App& a) {
    if (!d.cfg.onlyThis) return a.wins;
    std::vector<HWND> out;
    for (HWND h : a.wins)
        if (MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) == d.mon) out.push_back(h);
    return out;
}
constexpr int kBandSteps = 10;  // blur bands per rounded end (2 * steps + 1 windows per dock)

// Where a dragged icon would land among the pinned ones (index in g_pins).
static int PinInsertIndexAt(Dock& d, float x) {
    int idx = 0;
    for (auto& el : d.elems) {
        if (el.app < 0 || !g_apps[el.app].pinned) continue;
        if (el.x + el.w / 2 < x) idx++;
    }
    return idx;
}

// Dropped well above the dock: unpin.
static bool DragRemovesAt(Dock& d, float y) { return y < d.pill.top - S(d) * 0.9f; }


// ---- The status items in the dock ----

static ComPtr<ID2D1PathGeometry> SvgPath(const wchar_t* d, bool filled);
std::unordered_map<std::wstring, ComPtr<ID2D1PathGeometry>> g_iconPaths;
ComPtr<ID2D1StrokeStyle> g_roundStroke;

static ID2D1StrokeStyle* RoundStroke() {
    if (!g_roundStroke)
        g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                             D2D1_LINE_JOIN_ROUND),
                                 nullptr, 0, &g_roundStroke);
    return g_roundStroke.Get();
}

// A line icon of the design (24x24 box), centered on (cx, cy), `size` px wide.
static void DrawLineIcon(ID2D1RenderTarget* rt, const wchar_t* d, float cx, float cy, float size, ID2D1Brush* b,
                         float stroke = 1.7f) {
    if (!d || !*d) return;
    auto& g = g_iconPaths[d];
    if (!g) g = SvgPath(d, false);
    if (!g) return;
    D2D1_MATRIX_3X2_F old;
    rt->GetTransform(&old);
    float k = size / 24;
    rt->SetTransform(D2D1::Matrix3x2F::Translation(-12, -12) * D2D1::Matrix3x2F::Scale(k, k) *
                     D2D1::Matrix3x2F::Translation(cx, cy) * old);
    rt->DrawGeometry(g.Get(), b, stroke, RoundStroke());
    rt->SetTransform(old);
}

// The battery: its outline and how full it is.
static void DrawBattery(ID2D1RenderTarget* rt, float cx, float cy, float size, ID2D1Brush* b) {
    DrawLineIcon(rt, StatIcon(StBattery), cx, cy, size, b);
    float k = size / 24;
    float fill = 11.0f * std::clamp(g_stat.pct, 0, 100) / 100.0f;
    if (fill > 0.3f)
        rt->FillRectangle(D2D1::RectF(cx + (5 - 12) * k, cy + (10.5f - 12) * k, cx + (5 + fill - 12) * k, cy + (13.5f - 12) * k), b);
}

static D2D1_SIZE_F TextSize(const std::wstring& s, IDWriteTextFormat* f) {
    ComPtr<IDWriteTextLayout> l;
    DWRITE_TEXT_METRICS m = {};
    if (f && SUCCEEDED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f, 4000, 400, &l))) {
        l->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        l->GetMetrics(&m);
    }
    return D2D1::SizeF(m.widthIncludingTrailingWhitespace, m.height);
}

// Text centered on (cx, cy).
static void DrawTextAt(ID2D1RenderTarget* rt, const std::wstring& s, IDWriteTextFormat* f, float cx, float cy, ID2D1Brush* b) {
    ComPtr<IDWriteTextLayout> l;
    if (!f || FAILED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f, 4000, 400, &l))) return;
    l->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    l->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    DWRITE_TEXT_METRICS m = {};
    l->GetMetrics(&m);
    rt->DrawTextLayout(D2D1::Point2F(cx - m.widthIncludingTrailingWhitespace / 2, cy - m.height / 2), l.Get(), b);
}

static float StatDiv(const Dock& d) { return 13 * d.scale; }

// What the player's dock item shows: the icon, the cover, the title, both, or bars moving with the sound.
// (Without anything playing: the icon.)
enum { PdIcon, PdCover, PdTitle, PdBoth, PdBars };
static int PlayerDockMode() {
    std::wstring v = CfgStr(L"playerDock", L"ambos");
    if (v == L"icone") return PdIcon;
    if (v == L"capa") return PdCover;
    if (v == L"titulo") return PdTitle;
    if (v == L"espectro") return PdBars;
    return PdBoth;
}
static int PlayerShows() { return MediaNow().has ? PlayerDockMode() : PdIcon; }

// The bars: the sound's level (the default output's peak meter), each bar moving at its own pace.
// (IAudioMeterInformation: only declared in this compiler's headers.)
struct NimbusMeter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 count, float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};
__CRT_UUID_DECL(NimbusMeter, 0xC02216F6, 0x8C67, 0x4B5B, 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64)
struct PlayerVis {
    float lv[5] = {};
    float t = 0;
    ComPtr<NimbusMeter> meter;
    DWORD meterAt = 0;
};
static PlayerVis g_vis;
static std::wstring PlayerLabel(const Dock& d) {
    // (cut once per title and size, not on every frame)
    static std::wstring lastTitle, lastLabel;
    static float lastScale = 0;
    static const void* lastFont = nullptr;
    std::wstring t = MediaNow().title;
    if (t == lastTitle && d.scale == lastScale && d.font.Get() == lastFont) return lastLabel;
    lastTitle = t;
    lastScale = d.scale;
    lastFont = d.font.Get();
    float max = 150 * d.scale;
    if (d.font && TextSize(t, d.font.Get()).width > max) {
        // (binary search for the longest start that fits)
        size_t lo = 1, hi = t.size();
        while (lo < hi) {
            size_t mid = (lo + hi + 1) / 2;
            if (TextSize(t.substr(0, mid) + L"…", d.font.Get()).width <= max) lo = mid;
            else hi = mid - 1;
        }
        t = t.substr(0, lo) + L"…";
    }
    lastLabel = t;
    return t;
}

static float StatWidth(const Dock& d, int id) {
    float base = 38 * d.scale;
    if (id == StPlayer) {
        switch (PlayerShows()) {
            case PdCover: return 44 * d.scale;
            case PdTitle: return TextSize(PlayerLabel(d), d.font.Get()).width + 20 * d.scale;
            case PdBoth: return 26 * d.scale + 8 * d.scale + TextSize(PlayerLabel(d), d.font.Get()).width + 18 * d.scale;
            case PdBars: return 42 * d.scale;
        }
    }
    if (id == StKeyboard) return std::max(base, TextSize(g_stat.cur ? KbShort(g_stat.cur) : L"??", d.fontBold.Get()).width + 20 * d.scale);
    if (id == StClock)
        return std::max(TextSize(g_stat.time, d.font.Get()).width, TextSize(g_stat.date, d.font.Get()).width) + 18 * d.scale;
    return base;
}

static float TraySlot(const Dock& d) { return 32 * d.scale; }

// The tray's pieces, left to right: [pinned icons][button][<][strip][>], by style.
static std::vector<StatItem> TrayItems(const Dock& d) {
    std::vector<StatItem> v;
    if (!TrayInDock()) return v;
    auto add = [&](int id, float w, int sub) {
        StatItem it;
        it.id = id;
        it.w = w;
        it.sub = sub;
        v.push_back(it);
    };
    float slot = TraySlot(d);
    if (TrayStyle(L"grade") || TrayStyle(L"lista")) {
        if (!TrayOrder().empty()) add(StTrayBtn, 40 * d.scale, -1);
        return v;
    }
    std::vector<int> strip = TrayStripList();
    bool button = TrayStyle(L"botao");
    if (button) {
        for (int i : TrayFixedList()) add(StTrayIcon, slot, i);
        if (!strip.empty()) add(StTrayBtn, 40 * d.scale, -1);
        if (!g_trayUi.open) return v;
    }
    if (strip.empty()) return v;
    int n = (int)strip.size(), vis = std::min(n, g_set.trayVisible);
    if (n > vis) add(StTrayPrev, 20 * d.scale, -1);
    add(StTrayStrip, vis * slot, -1);
    if (n > vis) add(StTrayNext, 20 * d.scale, -1);
    return v;
}

static void BuildStat(Dock& d) {
    std::vector<StatItem> old = d.stat;  // (positions stay until the next layout: a panel placed now uses them)
    d.stat.clear();
    d.statW = 0;
    for (int id : StatIds()) {
        StatItem it;
        it.id = id;
        it.w = StatWidth(d, id);
        d.stat.push_back(it);
    }
    // The tray sits next to the apps.
    std::vector<StatItem> tray = TrayItems(d);
    d.stat.insert(StatOnLeft() ? d.stat.end() : d.stat.begin(), tray.begin(), tray.end());
    for (auto& it : d.stat) {
        d.statW += it.w;
        for (auto& o : old)
            if (o.id == it.id && o.sub == it.sub) it.x = o.x;
    }
    if (d.stat.size() > 1) d.statW += 2 * d.scale * (d.stat.size() - 1);
    if (d.statHover >= (int)d.stat.size()) d.statHover = -1;
}

// Room the status items take next to the apps (a full-width bar puts them in its corner instead).
static float StatBlock(const Dock& d) { return d.stat.empty() ? 0 : d.statW + (d.cfg.fixed ? 0 : StatDiv(d)); }

// Where the apps start, for `appsW` of apps.
static float AppsLeft(const Dock& d, float appsW) {
    if (d.cfg.fixed || d.stat.empty()) return d.cw / 2.0f - appsW / 2.0f;
    float block = StatBlock(d), left = d.cw / 2.0f - (appsW + block) / 2.0f;
    return StatOnLeft() ? left + block : left;
}

static void LayoutStat(Dock& d, float appsLeft, float appsW) {
    if (d.stat.empty()) return;
    float x;
    if (d.cfg.fixed) x = StatOnLeft() ? 12 * d.scale : d.cw - 12 * d.scale - d.statW;
    else x = StatOnLeft() ? appsLeft - StatBlock(d) : appsLeft + appsW + StatDiv(d);
    for (auto& it : d.stat) {
        it.x = x;
        x += it.w + 2 * d.scale;
    }
}

static int StatAt(const Dock& d, float x, float y) {
    if (y < d.pill.top || y > d.pill.bottom) return -1;
    for (int i = 0; i < (int)d.stat.size(); i++)
        if (x >= d.stat[i].x && x < d.stat[i].x + d.stat[i].w) return i;
    return -1;
}

bool FlyoutShows(const Dock& d, int kind);
bool FlyoutOpenOn(const Dock& d);

static float StatBoxH(const Dock& d) { return std::min(S(d) * 0.92f, 46 * d.scale); }

// The tray icon (g_tray index) under x in status item si, or -1.
static int TrayIconAt(const Dock& d, int si, float x) {
    if (si < 0 || si >= (int)d.stat.size()) return -1;
    const StatItem& it = d.stat[si];
    if (it.id == StTrayIcon) return it.sub;
    if (it.id != StTrayStrip) return -1;
    std::vector<int> strip = TrayStripList();
    int j = (int)std::floor((x - it.x) / TraySlot(d) + d.trayPos);
    return j >= 0 && j < (int)strip.size() ? strip[j] : -1;
}

// A tray icon in the dock pressed (and maybe dragged to a new place).
struct TrayDockDrag {
    bool down = false, active = false;
    Dock* dock = nullptr;
    std::wstring key;
    int sx = 0, sy = 0;
    float mx = 0;
};
static TrayDockDrag g_trayDockDrag;
constexpr UINT_PTR TIMER_TRAY_DRAGSCROLL = 14;  // dragging at the strip's ends: it scrolls by itself

static void DrawTrayIcon(Dock& d, int ti, float cx, float cy, float alpha = 1) {
    TrayIcon& t = g_tray[ti];
    if (!t.icon) return;
    if (g_trayDockDrag.active && g_trayDockDrag.dock == &d && t.key == g_trayDockDrag.key && alpha == 1) alpha = 0.3f;  // (its old place)
    std::wstring key = L"#tray:" + t.key + L":" + std::to_wstring(t.iconVer);
    ID2D1Bitmap* bmp = nullptr;
    auto it = d.bmps.find(key);
    if (it != d.bmps.end()) {
        bmp = it->second.Get();
    } else {
        // (a new picture of this icon: its older ones go, or an animated icon fills the cache)
        std::wstring prefix = L"#tray:" + t.key + L":";
        for (auto o = d.bmps.begin(); o != d.bmps.end();) o = o->first.rfind(prefix, 0) == 0 ? d.bmps.erase(o) : std::next(o);
        ComPtr<ID2D1Bitmap> b;
        d.rt->CreateBitmapFromWicBitmap(t.icon.Get(), nullptr, &b);
        if (!b) return;
        d.bmps[key] = b;
        bmp = b.Get();
    }
    float h = 10 * d.scale;
    D2D1_RECT_F r = D2D1::RectF(cx - h, cy - h, cx + h, cy + h);
    if (d.dc) d.dc->DrawBitmap(bmp, r, alpha, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
    else d.rt->DrawBitmap(bmp, r, alpha);
}

// Where a dragged tray icon would land: the icon under the mouse and which side (false if none).
static bool TrayDropAt(const Dock& d, float mx, int& si, int& ti, bool& after, float& barX) {
    si = -1;
    for (int i = 0; i < (int)d.stat.size(); i++)
        if (mx >= d.stat[i].x && mx < d.stat[i].x + d.stat[i].w) si = i;
    if (si < 0) return false;
    const StatItem& it = d.stat[si];
    ti = TrayIconAt(d, si, mx);
    if (ti < 0 || ti >= (int)g_tray.size()) return false;
    if (it.id == StTrayIcon) {
        after = mx > it.x + it.w / 2;
        barX = after ? it.x + it.w : it.x;
    } else {
        float slot = TraySlot(d), rel = (mx - it.x) / slot + d.trayPos;
        int j = (int)std::floor(rel);
        after = rel - j > 0.5f;
        barX = it.x + (j + (after ? 1 : 0) - d.trayPos) * slot;
    }
    return true;
}

// While dragging: a bar where it would land, and the icon under the mouse.
static void DrawTrayDrag(Dock& d, float cy, ID2D1SolidColorBrush* ink) {
    auto& g = g_trayDockDrag;
    if (!g.active || g.dock != &d) return;
    int si, ti;
    bool after;
    float bx;
    if (TrayDropAt(d, g.mx, si, ti, after, bx) && g_tray[ti].key != g.key) {
        float bh = StatBoxH(d) * 0.7f;
        d.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(bx - 1.5f * d.scale, cy - bh / 2, bx + 1.5f * d.scale, cy + bh / 2), 1.5f * d.scale,
                                                     1.5f * d.scale),
                                   ink);
    }
    for (int i = 0; i < (int)g_tray.size(); i++)
        if (g_tray[i].key == g.key) {
            DrawTrayIcon(d, i, g.mx, cy - 8 * d.scale, 0.9f);
            break;
        }
}

static void DrawTrayItem(Dock& d, int i, float cy, ID2D1SolidColorBrush* ink, ID2D1SolidColorBrush* hov) {
    const StatItem& it = d.stat[i];
    float bh = StatBoxH(d), r = 9 * d.scale, cx = it.x + it.w / 2;
    bool hot = i == d.statHover;
    auto box = [&](float l, float rr, bool strong) {
        D2D1_COLOR_F hc = g_theme.hover;
        hc.a = std::min(1.0f, hc.a * (strong ? 1.7f : 1.0f));
        hov->SetColor(hc);
        d.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(l, cy - bh / 2, rr, cy + bh / 2), r, r), hov);
    };
    switch (it.id) {
        case StTrayIcon:
            if (hot) box(it.x, it.x + it.w, false);
            DrawTrayIcon(d, it.sub, cx, cy);
            break;
        case StTrayBtn: {
            bool open = TrayStyle(L"botao") ? g_trayUi.open : FlyoutShows(d, StTrayBtn);
            if (open || hot) box(it.x, it.x + it.w, open);
            const wchar_t* chev = !TrayStyle(L"botao") ? L"M6 15l6-6 6 6" : open ? L"M15 6l-6 6 6 6" : L"M9 6l6 6-6 6";
            DrawLineIcon(d.rt.Get(), chev, cx - 2 * d.scale, cy, 18 * d.scale, ink);
            int n = (int)(TrayStyle(L"botao") ? TrayStripList() : TrayOrder()).size();
            if (n > 0 && d.fontTiny) {
                std::wstring num = std::to_wstring(n);
                float tw = TextSize(num, d.fontTiny.Get()).width;
                float bhh = 15 * d.scale, bw = std::max(bhh, tw + 7 * d.scale);
                float right = it.x + it.w - 2 * d.scale, top = cy - bh / 2 + 4 * d.scale;
                D2D1_COLOR_F bc = g_theme.text;
                bc.a *= 0.18f;
                hov->SetColor(bc);
                d.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(right - bw, top, right, top + bhh), bhh / 2, bhh / 2), hov);
                DrawTextAt(d.rt.Get(), num, d.fontTiny.Get(), right - bw / 2, top + bhh / 2, ink);
            }
            break;
        }
        case StTrayPrev:
        case StTrayNext: {
            bool off = it.id == StTrayPrev ? g_trayUi.off <= 0 : g_trayUi.off >= TrayMaxOff();
            if (hot && !off) box(it.x, it.x + it.w, false);
            D2D1_COLOR_F c = g_theme.text;
            if (off) c.a *= 0.3f;
            ComPtr<ID2D1SolidColorBrush> b;
            d.rt->CreateSolidColorBrush(c, &b);
            DrawLineIcon(d.rt.Get(), it.id == StTrayPrev ? L"M15 6l-6 6 6 6" : L"M9 6l6 6-6 6", cx, cy, 14 * d.scale, b.Get());
            break;
        }
        case StTrayStrip: {
            std::vector<int> strip = TrayStripList();
            float slot = TraySlot(d);
            d.rt->PushAxisAlignedClip(D2D1::RectF(it.x, cy - bh / 2, it.x + it.w, cy + bh / 2), D2D1_ANTIALIAS_MODE_ALIASED);
            for (int j = 0; j < (int)strip.size(); j++) {
                float x = it.x + (j - d.trayPos) * slot;
                if (x + slot < it.x || x > it.x + it.w) continue;
                int ti = strip[j];
                if (ti == d.trayHot) box(x + 1 * d.scale, x + slot - 1 * d.scale, false);
                DrawTrayIcon(d, ti, x + slot / 2, cy);
                if (TrayIsPinned(g_tray[ti]))
                    d.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + slot - 7 * d.scale, cy - bh / 2 + 9 * d.scale), 2.5f * d.scale,
                                                    2.5f * d.scale), ink);
            }
            d.rt->PopAxisAlignedClip();
            break;
        }
    }
}

// The label above a tray piece: the icon's tooltip (else the app's name), or what the button does.
static std::wstring TrayLabel(const Dock& d, const StatItem& it, float& cx) {
    if (it.id == StTrayBtn) {
        if (!TrayStyle(L"botao")) return L2(L"Em segundo plano", L"In the background");
        return g_trayUi.open ? L2(L"Esconder os ícones", L"Hide the icons") : L2(L"Mostrar os ícones", L"Show the icons");
    }
    int ti = d.trayHot;
    if (ti < 0 || ti >= (int)g_tray.size() || (it.id != StTrayIcon && it.id != StTrayStrip)) return L"";
    if (it.id == StTrayStrip) {
        std::vector<int> strip = TrayStripList();
        auto f = std::find(strip.begin(), strip.end(), ti);
        if (f != strip.end()) cx = it.x + ((f - strip.begin()) - d.trayPos + 0.5f) * TraySlot(d);
    }
    const TrayIcon& t = g_tray[ti];
    return t.tip.empty() ? t.name : t.tip;
}

static bool DrawPlayerArt(Dock& d, D2D1_RECT_F r, float radius);
static bool DrawPlayerArtOn(ID2D1RenderTarget* rt, const void* who, D2D1_RECT_F r, float radius);
static void DrawTrayDrag(Dock& d, float cy, ID2D1SolidColorBrush* ink);
static void DrawStat(Dock& d, float cy) {
    if (d.stat.empty()) return;
    ComPtr<ID2D1SolidColorBrush> ink, sub, hov, sepB;
    d.rt->CreateSolidColorBrush(g_theme.text, &ink);
    D2D1_COLOR_F sc = g_theme.text;
    sc.a *= 0.72f;
    d.rt->CreateSolidColorBrush(sc, &sub);
    d.rt->CreateSolidColorBrush(g_theme.hover, &hov);
    d.rt->CreateSolidColorBrush(g_theme.sep, &sepB);
    if (!d.cfg.fixed) {
        float dx = StatOnLeft() ? d.stat.back().x + d.stat.back().w + StatDiv(d) / 2 : d.stat.front().x - StatDiv(d) / 2;
        float h = S(d) * 0.62f;
        d.rt->FillRectangle(D2D1::RectF(dx - 0.5f * d.scale, cy - h / 2, dx + 0.5f * d.scale, cy + h / 2), sepB.Get());
    }
    float bh = StatBoxH(d), r = 10 * d.scale;
    for (int i = 0; i < (int)d.stat.size(); i++) {
        const StatItem& it = d.stat[i];
        if (IsTrayKind(it.id)) {
            DrawTrayItem(d, i, cy, ink.Get(), hov.Get());
            continue;
        }
        bool open = FlyoutShows(d, it.id), hot = i == d.statHover;
        if (open || hot) {
            D2D1_COLOR_F hc = g_theme.hover;
            hc.a = std::min(1.0f, hc.a * (open ? 1.7f : 1.0f));
            hov->SetColor(hc);
            d.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(it.x, cy - bh / 2, it.x + it.w, cy + bh / 2), r, r), hov.Get());
        }
        float cx = it.x + it.w / 2;
        switch (it.id) {
            case StKeyboard:
                DrawTextAt(d.rt.Get(), g_stat.cur ? KbShort(g_stat.cur) : L"??", d.fontBold.Get(), cx, cy, ink.Get());
                break;
            case StClock: {
                float lh = TextSize(g_stat.time, d.font.Get()).height;
                DrawTextAt(d.rt.Get(), g_stat.time, d.font.Get(), cx, cy - lh * 0.48f, ink.Get());
                DrawTextAt(d.rt.Get(), g_stat.date, d.font.Get(), cx, cy + lh * 0.52f, sub.Get());
                break;
            }
            case StBattery:
                DrawBattery(d.rt.Get(), cx, cy, 21 * d.scale, ink.Get());
                break;
            case StDevices: {
                int low = 101;
                for (auto& dv : DevicesNow())
                    if (dv.percent >= 0) low = std::min(low, dv.percent);
                float rr = 9 * d.scale;
                D2D1_COLOR_F c = g_theme.text;
                c.a *= 0.22f;
                hov->SetColor(c);
                d.rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rr, rr), hov.Get(), 2.2f * d.scale);
                if (low <= 100) {
                    ComPtr<ID2D1PathGeometry> g;
                    ComPtr<ID2D1GeometrySink> k;
                    float a = 2 * 3.14159265f * low / 100.0f;
                    if (low >= 100) d.rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rr, rr), low <= 20 ? sub.Get() : ink.Get(), 2.2f * d.scale);
                    else if (SUCCEEDED(g_d2d->CreatePathGeometry(&g)) && SUCCEEDED(g->Open(&k))) {
                        k->BeginFigure(D2D1::Point2F(cx, cy - rr), D2D1_FIGURE_BEGIN_HOLLOW);
                        k->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + rr * std::sin(a), cy - rr * std::cos(a)), D2D1::SizeF(rr, rr), 0,
                                                   D2D1_SWEEP_DIRECTION_CLOCKWISE, a > 3.14159265f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
                        k->EndFigure(D2D1_FIGURE_END_OPEN);
                        k->Close();
                        ComPtr<ID2D1SolidColorBrush> arc;
                        d.rt->CreateSolidColorBrush(low <= 20 ? D2D1::ColorF(0.95f, 0.36f, 0.32f) : g_theme.text, &arc);
                        ComPtr<ID2D1StrokeStyle> round;
                        D2D1_STROKE_STYLE_PROPERTIES sp = D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND);
                        g_d2d->CreateStrokeStyle(sp, nullptr, 0, &round);
                        d.rt->DrawGeometry(g.Get(), arc.Get(), 2.2f * d.scale, round.Get());
                    }
                }
                DrawLineIcon(d.rt.Get(), L"M12 8.5a2 2 0 0 1 2 2v3a2 2 0 0 1-4 0v-3a2 2 0 0 1 2-2z", cx, cy, 13 * d.scale, ink.Get());
                break;
            }
            case StPlayer: {
                int mode = PlayerShows();
                if (mode == PdCover || mode == PdBoth) {
                    float a = (mode == PdCover ? 28 : 26) * d.scale, ax = mode == PdCover ? cx - a / 2 : it.x + 9 * d.scale;
                    D2D1_RECT_F ar = D2D1::RectF(ax, cy - a / 2, ax + a, cy + a / 2);
                    if (!DrawPlayerArt(d, ar, 7 * d.scale)) {
                        D2D1_COLOR_F c = g_theme.text;
                        c.a *= 0.14f;
                        hov->SetColor(c);
                        d.rt->FillRoundedRectangle(D2D1::RoundedRect(ar, 7 * d.scale, 7 * d.scale), hov.Get());
                        DrawLineIcon(d.rt.Get(), StatIcon(StPlayer), ax + a / 2, cy, 15 * d.scale, ink.Get());
                    }
                    if (mode == PdBoth) {
                        std::wstring t = PlayerLabel(d);
                        float tw = TextSize(t, d.font.Get()).width;
                        DrawTextAt(d.rt.Get(), t, d.font.Get(), ax + a + 8 * d.scale + tw / 2, cy, ink.Get());
                    }
                } else if (mode == PdTitle) {
                    DrawTextAt(d.rt.Get(), PlayerLabel(d), d.font.Get(), cx, cy, ink.Get());
                } else if (mode == PdBars) {
                    float bw = 3.2f * d.scale, gap = 3.2f * d.scale, maxH = 22 * d.scale, minH = bw;
                    float x0 = cx - (5 * bw + 4 * gap) / 2;
                    for (int k = 0; k < 5; k++) {
                        float h = minH + (maxH - minH) * std::clamp(g_vis.lv[k], 0.0f, 1.0f);
                        float x = x0 + k * (bw + gap);
                        d.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x, cy - h / 2, x + bw, cy + h / 2), bw / 2, bw / 2), ink.Get());
                    }
                } else {
                    DrawLineIcon(d.rt.Get(), StatIcon(it.id), cx, cy, 20 * d.scale, ink.Get());
                }
                break;
            }
            default:
                DrawLineIcon(d.rt.Get(), StatIcon(it.id), cx, cy, 20 * d.scale, ink.Get());
                break;
        }
        size_t badgeN = it.id == StShelf ? g_shelf.size() : it.id == StNotif ? g_notifs.size() : it.id == StNotes ? (size_t)RemPending() : 0;
        if (badgeN && d.fontTiny) {
            std::wstring num = badgeN > 99 ? L"99+" : std::to_wstring(badgeN);
            float tw = TextSize(num, d.fontTiny.Get()).width;
            float bhh = 15 * d.scale, bw = std::max(bhh, tw + 7 * d.scale);
            float right = it.x + it.w - 1 * d.scale, top = cy - bh / 2 + 4 * d.scale;
            D2D1_COLOR_F bc = g_theme.text;
            bc.a *= 0.18f;
            hov->SetColor(bc);
            d.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(right - bw, top, right, top + bhh), bhh / 2, bhh / 2), hov.Get());
            DrawTextAt(d.rt.Get(), num, d.fontTiny.Get(), right - bw / 2, top + bhh / 2, ink.Get());
        }
    }
    DrawTrayDrag(d, cy, ink.Get());
}

// Label above the status item under the mouse (like the apps' names).
static void DrawStatLabel(Dock& d, float cy, ID2D1Brush* bg, ID2D1Brush* fg) {
    if (d.statHover < 0 || d.statHover >= (int)d.stat.size() || d.menuOpen || FlyoutOpenOn(d) || !d.font) return;
    const StatItem& it = d.stat[d.statHover];
    float cx = it.x + it.w / 2;
    std::wstring text = IsTrayKind(it.id) ? TrayLabel(d, it, cx) : StatLabel(it.id);
    if (text.empty()) return;
    ComPtr<IDWriteTextLayout> lay;
    g_dw->CreateTextLayout(text.c_str(), (UINT32)text.size(), d.font.Get(), 600 * d.scale, 60 * d.scale, &lay);
    if (!lay) return;
    DWRITE_TEXT_METRICS tm;
    lay->GetMetrics(&tm);
    float px = 10 * d.scale, py = 5 * d.scale;
    float bottom = cy - StatBoxH(d) / 2 - 8 * d.scale;
    float left = std::clamp(cx - tm.width / 2 - px, 4 * d.scale, d.cw - tm.width - px * 2 - 4 * d.scale);
    D2D1_RECT_F lr = D2D1::RectF(left, bottom - tm.height - py * 2, left + tm.width + px * 2, bottom);
    d.rt->FillRoundedRectangle(D2D1::RoundedRect(lr, 8 * d.scale, 8 * d.scale), bg);
    d.rt->DrawTextLayout(D2D1::Point2F(lr.left + px, lr.top + py), lay.Get(), fg);
}

void RenderDock(Dock& d);
void PlaceDock(Dock& d);

static void Layout(Dock& d);

static void BuildElems(Dock& d, bool animateNew) {
    std::unordered_map<std::wstring, Elem> old;
    for (auto& el : d.elems) old[el.key] = el;
    std::vector<Elem> e;
    auto add = [&](int app, const std::wstring& key) {
        Elem el;
        el.app = app;
        el.key = key;
        auto it = old.find(key);
        if (it != old.end()) {
            el.x = it->second.x;  // (until the next layout: clicks and the preview use it)
            el.w = it->second.w;
            el.m = it->second.m;
            el.target = it->second.target;
            el.appear = it->second.appear;
        } else {
            el.appear = animateNew ? 0.0f : 1.0f;
        }
        e.push_back(el);
    };
    bool startShown = false, sepStart = false, anyPinnedShown = false, sepRunning = false;
    for (int i = 0; i < (int)g_apps.size(); i++) {
        const App& a = g_apps[i];
        bool show;
        if (a.start) show = d.cfg.showStart;
        else if (a.pinned) show = d.cfg.showPinned || !DockWins(d, a).empty();
        else show = !DockWins(d, a).empty();
        if (!show) continue;
        if (!a.start && startShown && !sepStart) {
            add(-1, L"#sep-start");
            sepStart = true;
        }
        if (!a.start && !a.pinned && !sepRunning) {
            if (anyPinnedShown) add(-1, L"#sep-running");
            sepRunning = true;
        }
        if (a.start) startShown = true;
        if (a.pinned) anyPinnedShown = true;
        add(i, a.key);
    }
    d.elems.swap(e);

    // Fit the monitor: at full size (with room for the zoomed icons), how wide would the dock be?
    float slot = (g_set.iconSize + g_set.iconGap) * d.scale, sep = g_set.iconGap * d.scale;
    float need = 24 * d.scale + slot * std::max(0.0f, Zoom() - 1) * 1.6f;
    for (auto& el : d.elems) need += el.app < 0 ? sep : slot;
    BuildStat(d);
    need += StatBlock(d);
    float avail = d.cw - 32 * d.scale;
    d.fit = need > avail ? std::max(0.4f, avail / need) : 1.0f;
    // Positions now, not at the next frame: a rebuild often redraws nothing, and a click or the preview
    // right after it would otherwise find every icon at x = 0 (the Start button "not opening").
    Layout(d);
}

static void ComputeTargets(Dock& d) {
    float slot = S(d) + Gap(d), sepW = Gap(d);
    float rest = 0;
    for (auto& el : d.elems) rest += el.app < 0 ? sepW : slot;
    float x = AppsLeft(d, rest);
    float range = slot * 2.4f;
    const std::wstring& anim = g_set.animation;
    // Icon under the mouse, by the resting layout (stable while icons grow).
    int hov = -1;
    {
        float hx = x;
        for (int i = 0; i < (int)d.elems.size(); i++) {
            float w = d.elems[i].app < 0 ? sepW : slot;
            if (d.elems[i].app >= 0 && d.mouseX >= hx && d.mouseX < hx + w) hov = i;
            hx += w;
        }
    }
    for (int i = 0; i < (int)d.elems.size(); i++) {
        Elem& el = d.elems[i];
        float w = el.app < 0 ? sepW : slot;
        float cx = x + w / 2;
        el.target = 1;
        el.liftT = 0;
        el.opT = 1;
        if (el.app >= 0 && d.mouseX >= 0) {
            if (anim == L"zoom" && Zoom() > 1.001f) {
                float dist = std::fabs(d.mouseX - cx) / range;
                if (dist < 1) el.target = 1 + (Zoom() - 1) * (0.5f + 0.5f * std::cos(3.14159265f * dist));
            } else if (anim == L"elevar") {
                if (i == hov) el.liftT = -S(d) * 0.22f;
            } else if (anim == L"foco") {
                if (i == hov) el.target = std::min(std::max(Zoom(), 1.08f), 1.16f);
                else if (hov >= 0) el.opT = 0.42f;
            }
        }
        x += w;
    }
}

static bool StepAnim(Dock& d) {
    bool moving = false;
    for (auto& el : d.elems) {
        float diff = el.target - el.m;
        if (std::fabs(diff) < 0.002f) el.m = el.target;
        else { el.m += diff * 0.35f; moving = true; }
        if (el.appear < 1) {
            el.appear = std::min(1.0f, el.appear + (1.0f - el.appear) * 0.22f + 0.01f);
            moving = true;
        }
        float dl = el.liftT - el.lift;
        if (std::fabs(dl) < 0.3f) el.lift = el.liftT;
        else { el.lift += dl * 0.3f; moving = true; }
        float dop = el.opT - el.op;
        if (std::fabs(dop) < 0.01f) el.op = el.opT;
        else { el.op += dop * 0.25f; moving = true; }
    }
    for (auto it = g_launch.begin(); it != g_launch.end();) it = GetTickCount() - it->second > kLaunchBounceMs ? g_launch.erase(it) : std::next(it);
    if (!g_launch.empty() && d.visible) moving = true;
    // The tray strip's scroll: a spring, so it slightly overshoots and settles.
    float tgoal = (float)g_trayUi.off, tdx = tgoal - d.trayPos;
    if (std::fabs(tdx) < 0.002f && std::fabs(d.trayVel) < 0.002f) {
        d.trayPos = tgoal;
        d.trayVel = 0;
    } else {
        d.trayVel = (d.trayVel + tdx * 0.16f) * 0.74f;
        d.trayPos += d.trayVel;
        moving = true;
    }
    float goal = d.wantShown ? 1.0f : 0.0f;
    float sd = goal - d.shown;
    if (std::fabs(sd) < 0.01f) d.shown = goal;
    else { d.shown += sd * 0.3f; moving = true; }
    return moving;
}

static void Layout(Dock& d) {
    float slot = S(d) + Gap(d), sepW = Gap(d);
    float total = 0;
    for (auto& el : d.elems) total += (el.app < 0 ? sepW : slot * el.m) * el.appear;
    float padX = 12 * d.scale;
    float left = AppsLeft(d, total);
    float x = left;
    for (auto& el : d.elems) {
        el.x = x;
        el.w = (el.app < 0 ? sepW : slot * el.m) * el.appear;
        x += el.w;
    }
    LayoutStat(d, left, total);
    float bottom = d.ch - BottomGap(d);
    float l0 = left - (StatOnLeft() && !d.cfg.fixed ? StatBlock(d) : 0);
    float l1 = left + total + (!StatOnLeft() && !d.cfg.fixed ? StatBlock(d) : 0);
    if (d.cfg.fixed) d.pill = D2D1::RectF(0, bottom - PillH(d), (float)d.cw, bottom);
    else d.pill = D2D1::RectF(l0 - padX, bottom - PillH(d), l1 + padX, bottom);
}

static void Animate(Dock& d) {
    if (!d.animating) {
        d.animating = true;
        SetTimer(d.content, TIMER_ANIM, 15, nullptr);
    }
}

static void Invalidate(Dock& d) {
    ComputeTargets(d);
    Animate(d);
}

static void EnsureTarget(Dock& d) {
    if (d.rt) return;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    g_d2d->CreateDCRenderTarget(&props, &d.rt);
    if (d.rt) d.rt.As(&d.dc);
    d.bmps.clear();
}

static ID2D1Bitmap* IconBitmap(Dock& d, App& a) {
    auto it = d.bmps.find(a.key);
    if (it != d.bmps.end()) return it->second.Get();
    if (!a.iconTried) {
        a.iconTried = true;
        RequestIcon(a);  // arrives as WM_APP_ICON; a plain tile until then
        return nullptr;
    }
    if (!a.icon) return nullptr;  // still loading, or no icon
    ComPtr<ID2D1Bitmap> bmp;
    d.rt->CreateBitmapFromWicBitmap(a.icon.Get(), nullptr, &bmp);
    if (bmp) d.bmps[a.key] = bmp;
    return bmp.Get();
}

void RenderDock(Dock& d) {
    EnsureTarget(d);
    if (!d.rt) return;
    Layout(d);
    RECT rc = {0, 0, d.cw, d.ch};
    if (FAILED(d.rt->BindDC(d.memDC, &rc))) return;
    d.rt->BeginDraw();
    d.rt->Clear(D2D1::ColorF(0, 0, 0, 0));

    float r = PillRadius(d);
    D2D1_ROUNDED_RECT pill = D2D1::RoundedRect(d.pill, r, r);

    // The tint, in the exact rounded shape (the blur bands sit under it). Never fully transparent,
    // so the whole pill keeps catching the mouse (layered windows ignore alpha 0).
    ComPtr<ID2D1SolidColorBrush> fill, white, ltext, label, dot, flash, tile, sep;
    D2D1_COLOR_F tint = g_theme.tint;
    tint.a = std::max(tint.a, 1.0f / 255);
    d.rt->CreateSolidColorBrush(tint, &fill);
    d.rt->FillRoundedRectangle(pill, fill.Get());

    // Edge: a thin light line (brighter on top, like glass) or the theme's gradient.
    D2D1_GRADIENT_STOP stops[] = {{0.0f, g_theme.edge[0]}, {0.55f, g_theme.edge[1]}, {1.0f, g_theme.edge[2]}};
    ComPtr<ID2D1GradientStopCollection> coll;
    d.rt->CreateGradientStopCollection(stops, 3, &coll);
    ComPtr<ID2D1LinearGradientBrush> edge;
    D2D1_POINT_2F e0 = g_theme.edgeHorizontal ? D2D1::Point2F(d.pill.left, 0) : D2D1::Point2F(0, d.pill.top);
    D2D1_POINT_2F e1 = g_theme.edgeHorizontal ? D2D1::Point2F(d.pill.right, 0) : D2D1::Point2F(0, d.pill.bottom);
    d.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(e0, e1), coll.Get(), &edge);
    float sw = std::max(1.0f, d.scale);
    D2D1_ROUNDED_RECT edgeRc = D2D1::RoundedRect(
        D2D1::RectF(d.pill.left + sw / 2, d.pill.top + sw / 2, d.pill.right - sw / 2, d.pill.bottom - sw / 2),
        r - sw / 2, r - sw / 2);
    if (d.cfg.fixed) {
        // Full-width bar: only a line on top.
        d.rt->FillRectangle(D2D1::RectF(d.pill.left, d.pill.top, d.pill.right, d.pill.top + sw), edge.Get());
    } else {
        d.rt->DrawRoundedRectangle(edgeRc, edge.Get(), sw);
    }

    d.rt->CreateSolidColorBrush(g_theme.startGlyph, &white);
    d.rt->CreateSolidColorBrush(g_theme.labelText, &ltext);
    d.rt->CreateSolidColorBrush(g_theme.labelBg, &label);
    d.rt->CreateSolidColorBrush(g_theme.dot, &dot);
    d.rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.55f, 0.2f, 1), &flash);
    d.rt->CreateSolidColorBrush(g_theme.startTile, &tile);
    d.rt->CreateSolidColorBrush(g_theme.sep, &sep);

    float iconBottom = d.pill.bottom - PadY(d) - DotRoom(d);
    int hoverElem = -1;
    for (int i = 0; i < (int)d.elems.size(); i++) {
        Elem& el = d.elems[i];
        if (el.app < 0) {
            float cx = el.x + el.w / 2;
            float h = S(d) * 0.62f;
            float cy = d.pill.top + PillH(d) / 2;
            d.rt->FillRectangle(D2D1::RectF(cx - 0.5f * d.scale, cy - h / 2, cx + 0.5f * d.scale, cy + h / 2), sep.Get());
            continue;
        }
        App& a = g_apps[el.app];
        float size = S(d) * el.m * el.appear;
        if (size < 1) continue;
        float cx = el.x + el.w / 2;
        float lift = el.lift;
        auto ln = g_launch.find(a.key);
        if (ln != g_launch.end()) {
            DWORD t = GetTickCount() - ln->second;
            if (!a.wins.empty() || t > kLaunchBounceMs) g_launch.erase(ln);
            else lift += -std::fabs(std::sin(t / 1000.0f * 3.14159265f * 2.2f)) * S(d) * 0.32f;
        }
        D2D1_RECT_F ir = D2D1::RectF(cx - size / 2, iconBottom - size + lift, cx + size / 2, iconBottom + lift);
        // "Elevar": a soft shadow under the raised icon.
        if (el.lift < -0.5f) {
            float k = std::min(1.0f, el.lift / (-S(d) * 0.22f));
            ComPtr<ID2D1SolidColorBrush> shadow;
            d.rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.18f * k), &shadow);
            for (int s = 0; s < 3; s++) {
                float rx = size * (0.30f + 0.06f * s), ry = d.scale * (2.5f + 1.5f * s);
                d.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, iconBottom + d.scale), rx, ry), shadow.Get());
            }
        }
        tile->SetOpacity(el.op);
        white->SetOpacity(el.op);
        if (d.mouseX >= el.x && d.mouseX < el.x + el.w) hoverElem = i;
        if (a.start) {
            float tr = size * 0.27f;
            d.rt->FillRoundedRectangle(D2D1::RoundedRect(ir, tr, tr), tile.Get());
            float q = size * 0.19f, g = size * 0.035f;
            float x0 = cx - q - g / 2, y0 = (ir.top + ir.bottom) / 2 - q - g / 2;
            for (int k = 0; k < 4; k++) {
                float qx = x0 + (k % 2) * (q + g), qy = y0 + (k / 2) * (q + g);
                d.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(qx, qy, qx + q, qy + q), q * 0.12f, q * 0.12f), white.Get());
            }
        } else if (ID2D1Bitmap* bmp = IconBitmap(d, a)) {
            float op = ((g_drag.active && g_drag.dock == &d && a.key == g_drag.key) ? 0.3f : 1.0f) * el.op;
            if (d.dc) d.dc->DrawBitmap(bmp, ir, op, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
            else d.rt->DrawBitmap(bmp, ir, op, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
            float tr = size * 0.27f;
            d.rt->FillRoundedRectangle(D2D1::RoundedRect(ir, tr, tr), tile.Get());
        }
        if (!DockWins(d, a).empty()) {
            float dr = 2.4f * d.scale;
            float dy = d.pill.bottom - PadY(d) - DotRoom(d) / 2 + 2 * d.scale;
            d.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, dy), dr, dr), a.flash ? flash.Get() : dot.Get());
        }
    }

    tile->SetOpacity(1);
    white->SetOpacity(1);
    DrawStat(d, iconBottom - S(d) / 2);

    // The icon being dragged follows the mouse; a line shows where it will land.
    if (g_drag.active && g_drag.dock == &d) {
        auto it = std::find_if(g_apps.begin(), g_apps.end(), [](const App& x) { return x.key == g_drag.key; });
        if (it != g_apps.end()) {
            bool removes = DragRemovesAt(d, g_drag.y);
            if (!removes) {
                int ins = PinInsertIndexAt(d, g_drag.x), k = 0;
                float mx = -1, lastRight = -1;
                for (auto& el : d.elems) {
                    if (el.app < 0 || !g_apps[el.app].pinned) continue;
                    if (k == ins) { mx = el.x; break; }
                    lastRight = el.x + el.w;
                    k++;
                }
                if (mx < 0) {
                    if (lastRight >= 0) mx = lastRight;
                    else for (auto& el : d.elems) if (el.app < 0) { mx = el.x + el.w; break; }
                }
                if (mx >= 0) {
                    float h = S(d) * 0.8f;
                    d.rt->FillRoundedRectangle(
                        D2D1::RoundedRect(D2D1::RectF(mx - 1.5f * d.scale, iconBottom - h, mx + 1.5f * d.scale, iconBottom),
                                          1.5f * d.scale, 1.5f * d.scale),
                        dot.Get());
                }
            }
            float size = S(d);
            float cy = std::clamp(g_drag.y, size / 2, d.ch - size / 2);
            D2D1_RECT_F fr = D2D1::RectF(g_drag.x - size / 2, cy - size / 2, g_drag.x + size / 2, cy + size / 2);
            if (ID2D1Bitmap* bmp = IconBitmap(d, *it)) {
                float op = removes ? 0.4f : 0.95f;
                if (d.dc) d.dc->DrawBitmap(bmp, fr, op, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
                else d.rt->DrawBitmap(bmp, fr, op, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }
    }

    // Name above the icon under the mouse.
    extern bool PreviewShowing(const std::wstring& key);
    if (hoverElem >= 0 && !d.menuOpen && d.font && !PreviewShowing(g_apps[d.elems[hoverElem].app].key)) {
        Elem& el = d.elems[hoverElem];
        App& a = g_apps[el.app];
        const std::wstring& text = a.label.empty() ? std::wstring(L"App") : a.label;
        ComPtr<IDWriteTextLayout> lay;
        g_dw->CreateTextLayout(text.c_str(), (UINT32)text.size(), d.font.Get(), 600 * d.scale, 60 * d.scale, &lay);
        if (lay) {
            DWRITE_TEXT_METRICS tm;
            lay->GetMetrics(&tm);
            float px = 10 * d.scale, py = 5 * d.scale;
            float size = S(d) * el.m;
            float cx = el.x + el.w / 2;
            float bottom = iconBottom - size - 8 * d.scale;
            D2D1_RECT_F lr = D2D1::RectF(cx - tm.width / 2 - px, bottom - tm.height - py * 2, cx + tm.width / 2 + px, bottom);
            d.rt->FillRoundedRectangle(D2D1::RoundedRect(lr, 8 * d.scale, 8 * d.scale), label.Get());
            d.rt->DrawTextLayout(D2D1::Point2F(lr.left + px, lr.top + py), lay.Get(), ltext.Get());
        }
    }
    DrawStatLabel(d, iconBottom - S(d) / 2, label.Get(), ltext.Get());

    if (d.rt->EndDraw() == D2DERR_RECREATE_TARGET) {
        d.rt.Reset();
        d.dc.Reset();
        d.bmps.clear();
        return;
    }
    PlaceDock(d);
}

// Windows' blur behind a window. It always covers the window's whole rectangle (a window region
// doesn't cut it), so the dock builds its rounded glass from bands (see PlaceGlass) and paints the
// tint itself, in the rounded shape. `tinted`: acrylic with the theme's tint (the preview, which
// uses Windows' own rounded corners instead).
static void SetAccent(HWND hwnd, bool on, bool tinted = false) {
    struct ACCENTPOLICY { int state; int flags; DWORD color; int anim; };
    struct WINCOMPATTRDATA { int attr; void* data; SIZE_T size; };
    using Fn = BOOL(WINAPI*)(HWND, WINCOMPATTRDATA*);
    static Fn fn = (Fn)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!fn) return;
    const D2D1_COLOR_F& c = g_theme.tint;
    DWORD abgr = ((DWORD)(c.a * 255) << 24) | ((DWORD)(c.b * 255) << 16) | ((DWORD)(c.g * 255) << 8) | (DWORD)(c.r * 255);
    ACCENTPOLICY pol = {on ? (tinted ? 4 : 3) : 0, 0, abgr, 0};
    WINCOMPATTRDATA data = {19 /* WCA_ACCENT_POLICY */, &pol, sizeof(pol)};
    fn(hwnd, &data);
}

// The blur behind a rounded rectangle (screen coords), in bands that follow its rounded ends from
// inside (Windows' blur is always a rectangle). The tint and the edge are painted by the content
// window in the exact shape, so the few pixels the bands leave at the curves only miss the blur,
// never show it outside. `bands` has 1 + 2 * kBandSteps windows.
static void PlaceBands(std::vector<HWND>& bands, std::vector<RECT>& cur, bool on, float L, float T, float R, float B,
                       float r) {
    std::vector<RECT> want(bands.size());
    if (on) {
        const int n = kBandSteps;
        auto insetAt = [&](float fromEdge) {
            float dy = r - fromEdge;
            return r - std::sqrt(std::max(0.0f, r * r - dy * dy));
        };
        for (int i = 0; i < n; i++) {
            float e0 = r * i / n, e1 = r * (i + 1) / n;
            LONG x0 = (LONG)std::ceil(L + insetAt(e0)), x1 = (LONG)std::floor(R - insetAt(e0));
            want[1 + i] = {x0, (LONG)std::lround(T + e0), x1, (LONG)std::lround(T + e1)};
            want[1 + n + i] = {x0, (LONG)std::lround(B - e1), x1, (LONG)std::lround(B - e0)};
        }
        want[0] = {(LONG)std::lround(L), (LONG)std::lround(T + r), (LONG)std::lround(R), (LONG)std::lround(B - r)};
    }
    HDWP dwp = BeginDeferWindowPos((int)bands.size());
    for (size_t i = 0; i < bands.size(); i++) {
        const RECT& w = want[i];
        bool empty = w.right <= w.left || w.bottom <= w.top;
        if (empty) {
            if (cur[i].right > cur[i].left && dwp)
                dwp = DeferWindowPos(dwp, bands[i], nullptr, 0, 0, 0, 0,
                                     SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        } else if (!EqualRect(&w, &cur[i]) && dwp) {
            UINT flags = SWP_NOACTIVATE | SWP_NOZORDER;
            if (cur[i].right <= cur[i].left) flags |= SWP_SHOWWINDOW;
            dwp = DeferWindowPos(dwp, bands[i], nullptr, w.left, w.top, w.right - w.left, w.bottom - w.top, flags);
        }
        cur[i] = empty ? RECT{} : w;
    }
    if (dwp) EndDeferWindowPos(dwp);
}

static void HideBands(std::vector<HWND>& bands, std::vector<RECT>& cur) {
    for (size_t i = 0; i < bands.size(); i++) {
        ShowWindow(bands[i], SW_HIDE);
        cur[i] = {};
    }
}

static HWND CreateBand() {
    HWND g = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, L"NimbusMenu_Glass", L"", WS_POPUP,
                             0, 0, 10, 10, nullptr, nullptr, g_module, nullptr);
    DWORD corner = 1; /* DWMWCP_DONOTROUND */
    DwmSetWindowAttribute(g, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corner, sizeof(corner));
    COLORREF none = 0xFFFFFFFE; /* DWMWA_COLOR_NONE */
    DwmSetWindowAttribute(g, 34 /* DWMWA_BORDER_COLOR */, &none, sizeof(none));
    SetAccent(g, true);
    return g;
}

constexpr UINT_PTR TIMER_DOCK_BANDS = 15;
void PlaceDock(Dock& d) {
    int slide = (int)std::lround((1 - d.shown) * (PillH(d) + BottomGap(d) + 8 * d.scale));
    int top = d.rcMon.bottom - d.ch + slide;

    if (d.shown <= 0.001f && !d.wantShown) {
        if (d.visible) {
            ShowWindow(d.content, SW_HIDE);
            HideBands(d.bands, d.bandRc);
            d.visible = false;
        }
        return;
    }

    POINT dst = {d.rcMon.left, top};
    SIZE size = {d.cw, d.ch};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(d.content, nullptr, &dst, &size, d.memDC, &src, 0, &bf, ULW_ALPHA);

    // The blur behind is ~20 small windows: moving them all takes Windows up to 30 ms. While the dock
    // zooms (a frame every 15 ms) they follow at most every 33 ms; the last position always arrives.
    DWORD now = GetTickCount();
    if (d.visible && now - d.bandsAt < 33) {
        if (!d.bandsPending) {
            d.bandsPending = true;
            SetTimer(d.content, TIMER_DOCK_BANDS, 34, nullptr);
        }
    } else {
        PlaceBands(d.bands, d.bandRc, g_theme.blur, d.rcMon.left + d.pill.left, top + d.pill.top,
                   d.rcMon.left + d.pill.right, top + d.pill.bottom, PillRadius(d));
        d.bandsAt = now;
        d.bandsPending = false;
    }
    if (!d.visible) {
        ShowWindow(d.content, SW_SHOWNOACTIVATE);
        SetWindowPos(d.content, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        d.visible = true;
    }
}

static Dock* DockFrom(HWND hwnd) { return (Dock*)GetWindowLongPtrW(hwnd, GWLP_USERDATA); }

static int ElemAt(Dock& d, float x, float y) {
    if (y < d.pill.top - S(d) * (Zoom() - 1) - 4 * d.scale || y > d.pill.bottom) return -1;
    for (int i = 0; i < (int)d.elems.size(); i++) {
        const Elem& el = d.elems[i];
        if (el.app >= 0 && x >= el.x && x < el.x + el.w) return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------

// The restore we asked for last. A second restore that lands after the first one has already brought a
// maximized window back un-maximizes it, so a fast double click must not send two.
static HWND g_restoring;
static DWORD g_restoringAt;

// The window the dock brought to the front last, and when: right after a click, Windows may not have
// made it the foreground one yet, and the next click must still minimize it.
static HWND g_activated;
static DWORD g_activatedAt;

static void Activate(HWND h) {
    g_activated = h;
    g_activatedAt = GetTickCount();
    // SwitchToThisWindow(h, TRUE) restores a minimized window itself (to its maximized size when it was
    // maximized). Adding our own SW_RESTORE on top could hit the window after that and shrink it.
    bool pending = h == g_restoring && GetTickCount() - g_restoringAt < 800;
    if (IsIconic(h) && !pending) {
        g_restoring = h;
        g_restoringAt = GetTickCount();
        SwitchToThisWindow(h, TRUE);
    } else {
        SwitchToThisWindow(h, !IsIconic(h));  // still minimized = our restore is on its way: only focus
    }
}

static void Animate(Dock& d);
static void Launch(const App& a) {
    g_launch[a.key] = GetTickCount();
    for (auto* d : g_docks) Animate(*d);
    std::wstring target;
    if (!a.lnk.empty()) target = a.lnk;
    else if (!a.aumid.empty()) target = L"shell:AppsFolder\\" + a.aumid;
    else target = NativePath(a.exe);
    if (!target.empty()) ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void OpenStart() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) PostMessageW(tray, WM_SYSCOMMAND, SC_TASKLIST, 0);
}

void ToggleStart(Dock& d);
static void CloseStart(bool animate);

static void ClickApp(Dock& d, App& a) {
    if (a.start) {
        if (g_set.nevoaStart) ToggleStart(d);
        else OpenStart();
        return;
    }
    CloseStart(true);  // an app from the dock: the Iniciar gets out of the way
    std::vector<HWND> wins = DockWins(d, a);
    if (wins.empty()) { Launch(a); return; }
    // In front: the foreground window, or one it belongs to (a dialog of it), or the one this dock just
    // brought up (Windows may still be bringing it).
    HWND fg = GetForegroundWindow();
    HWND fgRoot = fg ? GetAncestor(fg, GA_ROOTOWNER) : nullptr;
    HWND front = nullptr;
    for (HWND w : wins)
        if (w == fg || w == fgRoot) front = w;
    if (!front && g_activated && GetTickCount() - g_activatedAt < 1000 && !IsIconic(g_activated) &&
        std::find(wins.begin(), wins.end(), g_activated) != wins.end())
        front = g_activated;
    Wh_Log(L"dock: app click (%d windows, fg=%p front=%p iconic=%d)", (int)wins.size(), fg, front, front ? IsIconic(front) : -1);
    if (front) {
        if (wins.size() == 1) {
            // ShowWindowAsync also reaches windows run as administrator (a posted SC_MINIMIZE doesn't).
            g_activated = nullptr;
            ShowWindowAsync(front, SW_MINIMIZE);
        } else {
            // Several windows: go to the next one (the last in z-order, so it cycles).
            Activate(wins.back());
        }
        return;
    }
    Activate(wins.front());
}

// How to pin an open app. Its window's AppUserModelID only works when an app in Start has it (Steam
// sets Valve.Steam.Client on its window, but its shortcut doesn't): otherwise the program's path.
static bool AumidInStart(const std::wstring& aumid) {
    ComPtr<IShellItem> item;
    return SUCCEEDED(SHCreateItemFromParsingName((L"shell:AppsFolder\\" + aumid).c_str(), nullptr, IID_PPV_ARGS(&item)));
}

static bool PinForApp(const App& a, Pin& out) {
    if (!a.aumid.empty() && (a.exe.empty() || AumidInStart(a.aumid))) out = {L"aumid", a.aumid};
    else if (!a.exe.empty()) out = {L"exe", a.exe};
    else return false;
    return true;
}

static void TogglePin(const std::wstring& key) {
    for (auto& a : g_apps) {
        if (a.key != key || a.start) continue;
        if (a.pinned) {
            if (a.pinIndex >= 0 && a.pinIndex < (int)g_pins.size()) g_pins.erase(g_pins.begin() + a.pinIndex);
        } else {
            Pin p;
            if (PinForApp(a, p)) g_pins.push_back(p);
        }
        SavePins();
        break;
    }
    RebuildApps();
    for (auto* d : g_docks) { BuildElems(*d, true); Invalidate(*d); }
}

static void DropDrag(Dock& d, const std::wstring& key, float x, float y) {
    auto it = std::find_if(g_apps.begin(), g_apps.end(), [&](const App& a) { return a.key == key; });
    if (it == g_apps.end() || it->start) return;
    App& a = *it;
    if (DragRemovesAt(d, y)) {
        if (a.pinned && a.pinIndex >= 0 && a.pinIndex < (int)g_pins.size()) g_pins.erase(g_pins.begin() + a.pinIndex);
    } else {
        int ins = PinInsertIndexAt(d, x);
        Pin moving;
        if (a.pinned && a.pinIndex >= 0 && a.pinIndex < (int)g_pins.size()) {
            moving = g_pins[a.pinIndex];
            g_pins.erase(g_pins.begin() + a.pinIndex);
            if (ins > a.pinIndex) ins--;
        } else if (!PinForApp(a, moving)) {
            return;
        }
        ins = std::clamp(ins, 0, (int)g_pins.size());
        g_pins.insert(g_pins.begin() + ins, moving);
    }
    SavePins();
    RebuildApps();
    for (auto* x2 : g_docks) { BuildElems(*x2, true); Invalidate(*x2); }
}

static void OpenSettings();

// A dock menu runs a modal loop that still handles display changes, Explorer restarts and settings
// changes. Rebuilding the docks then would free the Dock the menu belongs to, so CreateDocks waits
// until the menu returns.
int g_dockMenuDepth = 0;
bool g_docksPending = false;

static int TrackDockMenu(Dock& d, HMENU m, POINT screen) {
    d.menuOpen = true;
    RenderDock(d);
    SetForegroundWindow(d.content);
    g_dockMenuDepth++;
    int cmd = TrackPopupMenuEx(m, TPM_RETURNCMD | TPM_BOTTOMALIGN | TPM_CENTERALIGN | TPM_NONOTIFY, screen.x,
                               screen.y, d.content, nullptr);
    g_dockMenuDepth--;
    DestroyMenu(m);
    d.menuOpen = false;
    if (!g_dockMenuDepth && g_docksPending) {
        g_docksPending = false;
        SetTimer(g_msgWnd, TIMER_DOCKS, 1, nullptr);
    }
    return cmd;
}

// Menu of the dock itself (Start button or empty part of the dock).
static void ShowDockMenu(Dock& d, POINT screen) {
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, Tr(S_BarSettings));
    AppendMenuW(m, MF_STRING, 2, Tr(S_TaskManager));
    int cmd = TrackDockMenu(d, m, screen);
    if (cmd == 1) OpenSettings();
    if (cmd == 2) ShellExecuteW(nullptr, L"open", L"taskmgr.exe", nullptr, nullptr, SW_SHOWNORMAL);
    d.outsideSince = GetTickCount();
    Invalidate(d);
}

// "End task", like the Windows taskbar's: only when it's turned on in Windows (Settings > System >
// For developers > End task). Ends the processes of the app's windows, even when they don't respond.
static bool EndTaskEnabled() {
    if (g_set.endTask == L"on") return true;
    if (g_set.endTask == L"off") return false;
    for (const wchar_t* key : {L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\TaskbarDeveloperSettings",
                               L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"}) {
        DWORD v = 0, size = sizeof(v);
        if (RegGetValueW(HKEY_CURRENT_USER, key, L"TaskbarEndTask", RRF_RT_REG_DWORD, nullptr, &v, &size) == ERROR_SUCCESS)
            return v != 0;
    }
    return false;
}

static void NotifPush(const std::wstring& title, const std::wstring& body, std::vector<NotifAction> actions, bool force, bool reminder);
static void EndTask(const std::vector<HWND>& wins) {
    // Never the shell: File Explorer windows usually belong to the explorer.exe of the desktop.
    DWORD shellPid = 0;
    GetWindowThreadProcessId(GetShellWindow(), &shellPid);
    std::vector<DWORD> pids;
    for (HWND h : wins) {
        HWND owner = h;
        wchar_t cls[64] = {};
        GetClassNameW(h, cls, ARRAYSIZE(cls));
        if (!wcscmp(cls, L"ApplicationFrameWindow")) {
            // Store apps: the frame belongs to ApplicationFrameHost.exe (shared by all of them); the
            // app itself owns the inner CoreWindow, which is detached while minimized or suspended.
            owner = FindWindowExW(h, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
            if (!owner) continue;
        }
        DWORD pid = 0;
        GetWindowThreadProcessId(owner, &pid);
        if (pid && pid != shellPid && pid != GetCurrentProcessId() &&
            std::find(pids.begin(), pids.end(), pid) == pids.end())
            pids.push_back(pid);
    }
    int failed = 0;
    for (DWORD pid : pids) {
        HANDLE p = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (!p) {
            Wh_Log(L"End task: can't open process %u (%u)", pid, GetLastError());
            failed++;
            continue;
        }
        if (!TerminateProcess(p, 1)) {
            Wh_Log(L"End task: TerminateProcess %u failed (%u)", pid, GetLastError());
            failed++;
        }
        CloseHandle(p);
    }
    // Usually a program run as administrator: say so instead of doing nothing.
    if (failed)
        NotifPush(L2(L"Não deu pra encerrar a tarefa", L"Couldn't end the task"),
                  L2(L"O Windows não deixou (o programa deve estar rodando como administrador). Tente pelo Gerenciador de Tarefas.",
                     L"Windows didn't allow it (the program is probably running as administrator). Try Task Manager."),
                  {}, true, false);
}

static void ShowMenu(Dock& d, int elem, POINT screen) {
    App& a = g_apps[d.elems[elem].app];
    if (a.start) { ShowDockMenu(d, screen); return; }
    std::wstring key = a.key;
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, Tr(S_NewWindow));
    AppendMenuW(m, MF_STRING, 2, a.pinned ? Tr(S_UnpinDock) : Tr(S_PinDock));
    std::vector<HWND> wins = DockWins(d, a);
    if (!wins.empty()) {
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, 3, wins.size() > 1 ? Tr(S_CloseAll) : Tr(S_CloseWindow));
        if (EndTaskEnabled()) AppendMenuW(m, MF_STRING, 4, Tr(S_EndTask));
    }
    int cmd = TrackDockMenu(d, m, screen);
    for (auto& x : g_apps) {
        if (x.key != key) continue;
        if (cmd == 1) Launch(x);
        if (cmd == 3) for (HWND h : wins) PostMessageW(h, WM_SYSCOMMAND, SC_CLOSE, 0);
        if (cmd == 4) EndTask(wins);
        break;
    }
    if (cmd == 2) TogglePin(key);
    d.outsideSince = GetTickCount();
    Invalidate(d);
}

// ---------------------------------------------------------------------------
// Fixed bar: reserve its strip (an "app bar", like the Windows taskbar)
// ---------------------------------------------------------------------------

constexpr UINT WM_APP_APPBAR = WM_APP + 5;

static void SetAppBarPos(Dock& d) {
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = d.content;
    abd.uEdge = ABE_BOTTOM;
    abd.rc = d.rcMon;
    LONG h = (LONG)std::lround(PillH(d));
    abd.rc.top = abd.rc.bottom - h;
    SHAppBarMessage(ABM_QUERYPOS, &abd);
    abd.rc.top = abd.rc.bottom - h;
    SHAppBarMessage(ABM_SETPOS, &abd);
}

static void RegisterAppBar(Dock& d) {
    if (d.appbar) return;
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = d.content;
    abd.uCallbackMessage = WM_APP_APPBAR;
    if (!SHAppBarMessage(ABM_NEW, &abd)) return;
    d.appbar = true;
    SetAppBarPos(d);
}

static void RemoveAppBar(Dock& d) {
    if (!d.appbar) return;
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = d.content;
    SHAppBarMessage(ABM_REMOVE, &abd);
    d.appbar = false;
}

// ---------------------------------------------------------------------------
// Window previews (live DWM thumbnails, like the Windows taskbar)
// ---------------------------------------------------------------------------

constexpr UINT_PTR TIMER_PREVIEW = 11;
constexpr int kMaxPreviews = 6;

struct Preview {
    HWND hwnd = nullptr;
    Dock* dock = nullptr;
    std::wstring key;
    std::vector<HWND> wins;
    std::vector<HTHUMBNAIL> thumbs;
    std::vector<D2D1_RECT_F> tiles, shots;
    std::vector<std::wstring> titles;
    int hover = -1;
    bool closeHover = false;
    bool visible = false;
    RECT rc{};  // screen
    float scale = 1;
    ComPtr<ID2D1HwndRenderTarget> rt;
    ComPtr<IDWriteTextFormat> font;
    DWORD outsideSince = 0;
    HWND peek = nullptr;
} g_prev;

constexpr UINT_PTR TIMER_PEEK = 12;

// "Peek" (the whole window shown, the others faded), like hovering a taskbar thumbnail.
// Undocumented dwmapi export #113, the same one other docks use.
using PeekFn = HRESULT(WINAPI*)(UINT, HWND, HWND, UINT, UINT_PTR);
static PeekFn GetPeek() {
    static PeekFn fn = []() -> PeekFn {
        HMODULE m = GetModuleHandleW(L"dwmapi.dll");
        return m ? (PeekFn)GetProcAddress(m, (LPCSTR)113) : nullptr;
    }();
    return fn;
}

static void StopPeek() {
    if (!g_prev.peek) return;
    if (auto fn = GetPeek()) fn(0, nullptr, nullptr, 1, 0);
    g_prev.peek = nullptr;
}

static void StartPeek(HWND w) {
    if (!w || IsIconic(w) || w == g_prev.peek) return;
    auto fn = GetPeek();
    if (!fn) return;
    HRESULT hr = fn(1, w, g_prev.hwnd, 1, 0);
    Wh_Log(L"peek hwnd=%p hr=0x%08lx", w, (unsigned long)hr);
    if (SUCCEEDED(hr)) g_prev.peek = w;
}

static float PS(float v) { return v * g_prev.scale; }

bool PreviewShowing(const std::wstring& key) { return g_prev.visible && g_prev.key == key; }

static D2D1_RECT_F CloseRect(int i) {
    const D2D1_RECT_F& t = g_prev.tiles[i];
    float s = PS(22);
    return D2D1::RectF(t.right - PS(8) - s, t.top + PS(6), t.right - PS(8), t.top + PS(6) + s);
}

void HidePreview() {
    if (g_prev.hwnd) KillTimer(g_prev.hwnd, TIMER_PEEK);
    StopPeek();
    for (auto t : g_prev.thumbs) if (t) DwmUnregisterThumbnail(t);
    g_prev.thumbs.clear();
    g_prev.wins.clear();
    g_prev.tiles.clear();
    g_prev.shots.clear();
    g_prev.titles.clear();
    g_prev.key.clear();
    g_prev.hover = -1;
    if (g_prev.visible && g_prev.hwnd) ShowWindow(g_prev.hwnd, SW_HIDE);
    g_prev.visible = false;
    if (g_prev.dock) Invalidate(*g_prev.dock);
    g_prev.dock = nullptr;
}

static void RenderPreview() {
    if (!g_prev.visible) return;
    int w = g_prev.rc.right - g_prev.rc.left, h = g_prev.rc.bottom - g_prev.rc.top;
    if (!g_prev.rt) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
        g_d2d->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(g_prev.hwnd, D2D1::SizeU(w, h)),
                                      &g_prev.rt);
        if (!g_prev.rt) return;
    } else {
        g_prev.rt->Resize(D2D1::SizeU(w, h));
    }
    auto* rt = g_prev.rt.Get();
    rt->BeginDraw();
    rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    float r = PS(8);
    if (!g_theme.blur) {
        ComPtr<ID2D1SolidColorBrush> solid;
        rt->CreateSolidColorBrush(g_theme.tint, &solid);
        rt->FillRectangle(D2D1::RectF(0, 0, (float)w, (float)h), solid.Get());
    }
    D2D1_GRADIENT_STOP stops[] = {{0.0f, g_theme.edge[0]}, {0.55f, g_theme.edge[1]}, {1.0f, g_theme.edge[2]}};
    ComPtr<ID2D1GradientStopCollection> coll;
    rt->CreateGradientStopCollection(stops, 3, &coll);
    ComPtr<ID2D1LinearGradientBrush> edge;
    rt->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), g_theme.edgeHorizontal ? D2D1::Point2F((float)w, 0)
                                                                                         : D2D1::Point2F(0, (float)h)),
        coll.Get(), &edge);
    float sw = std::max(1.0f, g_prev.scale);
    rt->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(sw / 2, sw / 2, w - sw / 2, h - sw / 2), r - sw / 2, r - sw / 2), edge.Get(), sw);

    ComPtr<ID2D1SolidColorBrush> text, hoverBg, xBg, xFg, empty;
    rt->CreateSolidColorBrush(g_theme.text, &text);
    rt->CreateSolidColorBrush(g_theme.hover, &hoverBg);
    rt->CreateSolidColorBrush(D2D1::ColorF(0.9f, 0.2f, 0.25f, g_prev.closeHover ? 1.0f : 0.0f), &xBg);
    rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.9f), &xFg);
    rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.05f), &empty);
    for (size_t i = 0; i < g_prev.tiles.size(); i++) {
        const D2D1_RECT_F& t = g_prev.tiles[i];
        if ((int)i == g_prev.hover) rt->FillRoundedRectangle(D2D1::RoundedRect(t, PS(12), PS(12)), hoverBg.Get());
        if (!g_prev.thumbs[i]) rt->FillRoundedRectangle(D2D1::RoundedRect(g_prev.shots[i], PS(8), PS(8)), empty.Get());
        D2D1_RECT_F tr = D2D1::RectF(t.left + PS(10), t.top + PS(8), t.right - PS(36), t.top + PS(30));
        rt->DrawTextW(g_prev.titles[i].c_str(), (UINT32)g_prev.titles[i].size(), g_prev.font.Get(), tr, text.Get(),
                      D2D1_DRAW_TEXT_OPTIONS_CLIP);
        if ((int)i == g_prev.hover) {
            D2D1_RECT_F c = CloseRect((int)i);
            rt->FillRoundedRectangle(D2D1::RoundedRect(c, PS(6), PS(6)), xBg.Get());
            float m = PS(7);
            rt->DrawLine(D2D1::Point2F(c.left + m, c.top + m), D2D1::Point2F(c.right - m, c.bottom - m), xFg.Get(), PS(1.4f));
            rt->DrawLine(D2D1::Point2F(c.right - m, c.top + m), D2D1::Point2F(c.left + m, c.bottom - m), xFg.Get(), PS(1.4f));
        }
    }
    if (rt->EndDraw() == D2DERR_RECREATE_TARGET) g_prev.rt.Reset();
}

void ShowPreview(Dock& d, int elem) {
    if (elem < 0 || elem >= (int)d.elems.size() || d.elems[elem].app < 0) { HidePreview(); return; }
    App& a = g_apps[d.elems[elem].app];
    std::vector<HWND> wins = DockWins(d, a);
    if (wins.empty() || a.start) { HidePreview(); return; }
    if (g_prev.visible && g_prev.key == a.key && g_prev.dock == &d) return;
    HidePreview();
    g_prev.dock = &d;
    g_prev.key = a.key;
    if (g_prev.font && g_prev.scale != d.scale) g_prev.font.Reset();  // another monitor's scale
    g_prev.scale = d.scale;
    if (!g_prev.font) {
        g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 12.5f * d.scale, g_locale, &g_prev.font);
        if (g_prev.font) {
            g_prev.font->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
            ComPtr<IDWriteInlineObject> dots;
            g_dw->CreateEllipsisTrimmingSign(g_prev.font.Get(), &dots);
            g_prev.font->SetTrimming(&trim, dots.Get());
        }
    }

    float tileW = PS(232), shotH = PS(140), titleH = PS(34), pad = PS(10), gap = PS(6);
    int n = std::min((int)wins.size(), kMaxPreviews);
    int w = (int)std::ceil(pad * 2 + n * tileW + (n - 1) * gap);
    int h = (int)std::ceil(pad * 2 + titleH + shotH + PS(8));
    for (int i = 0; i < n; i++) {
        HWND win = wins[i];
        g_prev.wins.push_back(win);
        wchar_t title[256] = {};
        GetWindowTextW(win, title, ARRAYSIZE(title));
        g_prev.titles.push_back(title);
        float x = pad + i * (tileW + gap);
        g_prev.tiles.push_back(D2D1::RectF(x, pad, x + tileW, h - pad));
        g_prev.shots.push_back(D2D1::RectF(x + PS(8), pad + titleH, x + tileW - PS(8), pad + titleH + shotH));
    }

    // Above the icon, inside the monitor.
    const Elem& el = d.elems[elem];
    int iconCx = d.rcMon.left + (int)(el.x + el.w / 2);
    int dockTop = d.rcMon.bottom - d.ch + (int)d.pill.top;
    int x = std::clamp(iconCx - w / 2, (int)d.rcMon.left + 8, (int)d.rcMon.right - w - 8);
    int y = dockTop - h - (int)PS(10);
    g_prev.rc = {x, y, x + w, y + h};

    SetWindowPos(g_prev.hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    g_prev.visible = true;
    g_prev.outsideSince = GetTickCount();

    for (int i = 0; i < n; i++) {
        HTHUMBNAIL th = nullptr;
        if (!IsIconic(g_prev.wins[i]) && SUCCEEDED(DwmRegisterThumbnail(g_prev.hwnd, g_prev.wins[i], &th))) {
            SIZE src = {};
            DwmQueryThumbnailSourceSize(th, &src);
            D2D1_RECT_F s = g_prev.shots[i];
            float bw = s.right - s.left, bh = s.bottom - s.top;
            float k = (src.cx > 0 && src.cy > 0) ? std::min(bw / src.cx, bh / src.cy) : 1.0f;
            float tw = src.cx * k, thh = src.cy * k;
            float ox = s.left + (bw - tw) / 2, oy = s.top + (bh - thh) / 2;
            DWM_THUMBNAIL_PROPERTIES p = {};
            p.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
            p.rcDestination = {(LONG)ox, (LONG)oy, (LONG)(ox + tw), (LONG)(oy + thh)};
            p.fVisible = TRUE;
            p.opacity = 255;
            p.fSourceClientAreaOnly = FALSE;
            DwmUpdateThumbnailProperties(th, &p);
        }
        g_prev.thumbs.push_back(th);
    }
    RenderPreview();
    Invalidate(d);
}

static bool CursorInPreview(POINT pt) {
    return g_prev.visible && pt.x >= g_prev.rc.left && pt.x < g_prev.rc.right && pt.y >= g_prev.rc.top &&
           pt.y < g_prev.rc.bottom + (LONG)PS(14);
}

void PollPreview() {
    if (!g_prev.visible || !g_prev.dock) return;
    POINT pt;
    GetCursorPos(&pt);
    Dock& d = *g_prev.dock;
    bool overDock = d.mouseX >= 0;
    if (CursorInPreview(pt) || overDock) {
        g_prev.outsideSince = GetTickCount();
    } else if (GetTickCount() - g_prev.outsideSince > 300) {
        HidePreview();
    }
}

static int PreviewTileAt(int x, int y) {
    for (int i = 0; i < (int)g_prev.tiles.size(); i++) {
        const D2D1_RECT_F& t = g_prev.tiles[i];
        if (x >= t.left && x < t.right && y >= t.top && y < t.bottom) return i;
    }
    return -1;
}

LRESULT CALLBACK PreviewProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            RenderPreview();
            return 0;
        }
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            int t = PreviewTileAt(x, y);
            bool ch = false;
            if (t >= 0) {
                D2D1_RECT_F c = CloseRect(t);
                ch = x >= c.left && x < c.right && y >= c.top && y < c.bottom;
            }
            if (t != g_prev.hover) {
                StopPeek();
                KillTimer(hwnd, TIMER_PEEK);
                if (t >= 0) SetTimer(hwnd, TIMER_PEEK, 350, nullptr);
            }
            if (t != g_prev.hover || ch != g_prev.closeHover) {
                g_prev.hover = t;
                g_prev.closeHover = ch;
                RenderPreview();
            }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            g_prev.outsideSince = GetTickCount();
            return 0;
        }
        case WM_TIMER:
            if (wp == TIMER_PEEK) {
                KillTimer(hwnd, TIMER_PEEK);
                if (g_prev.hover >= 0 && g_prev.hover < (int)g_prev.wins.size() && !g_prev.closeHover)
                    StartPeek(g_prev.wins[g_prev.hover]);
            }
            return 0;
        case WM_MOUSELEAVE:
            KillTimer(hwnd, TIMER_PEEK);
            StopPeek();
            g_prev.hover = -1;
            g_prev.closeHover = false;
            RenderPreview();
            return 0;
        case WM_LBUTTONUP: {
            int t = PreviewTileAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (t < 0) return 0;
            HWND win = g_prev.wins[t];
            StopPeek();
            if (g_prev.closeHover) {
                PostMessageW(win, WM_SYSCOMMAND, SC_CLOSE, 0);
            } else {
                Activate(win);
            }
            HidePreview();
            return 0;
        }
        case WM_MBUTTONUP: {
            int t = PreviewTileAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (t >= 0) PostMessageW(g_prev.wins[t], WM_SYSCOMMAND, SC_CLOSE, 0);
            HidePreview();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Nimbus' Start: search (apps + files via Everything), pinned, all apps, power
// ---------------------------------------------------------------------------

struct StartItem {
    std::wstring name, launch;  // launch: "shell:AppsFolder\<id>" or a file/shortcut path
    std::wstring path;          // files: the folder, shown under the name
    bool file = false, folder = false;
    bool setting = false;       // a Windows Settings page (ms-settings:)
    std::wstring keywords;      // settings: other words that find it
    ComPtr<IWICBitmapSource> icon;
    bool iconTried = false;
    ULONGLONG when = 0;         // Downloads tab: last change
    std::wstring fold, foldExtra;  // search: the name, and the app's id or the settings' words, lower case without accents
};

enum StartRowKind { RowApp, RowFile, RowTabPinned, RowTabAll, RowPower, RowAction, RowTabRecent, RowTabStatus, RowStatus, RowTray,
                    RowFn, RowSlide, RowSec, RowOrg };  // (Status tab: a button, a slider, a part being organized, the Organize button)

// The Start's Status tab shows when any status item, or the tray, goes to Start.
static bool StartHasStatus() {
    if (TrayInStart() || (g_set.shelfPlace & PlStart) || (g_set.notifPlace & PlStart) || (g_set.notesPlace & PlStart)) return true;
    for (int k = 0; k < 8; k++)
        if ((g_set.extraPlace[k] & PlStart)) return true;
    for (int id = 0; id < StCount; id++)
        if (StatOn(id) && (g_set.statPlace[id] & PlStart)) return true;
    return false;
}
bool FlyoutShowsKind(int kind);
static void StatClick(Dock& d, int id, bool right, bool fromStart);
bool FlyoutFromStartOpen();
void CloseFlyout();

// Without Everything running: what the files part offers instead.
enum EverythingState { EvOk, EvClosed, EvMissing };
enum StartAction { ActOpenEverything = 1, ActExplorerSearch, ActGetEverything };

struct StartRow {
    D2D1_RECT_F rc;
    StartRowKind kind;
    StartItem* item = nullptr;
    int power = 0;
    std::function<void()> fn;             // RowFn: what a click does
    std::function<void(float)> slide;     // RowSlide: set from 0..1
    float x0 = 0, x1 = 0;                 // RowSlide: the track
};

struct StartMenu {
    HWND hwnd = nullptr;
    std::vector<HWND> bands;
    std::vector<RECT> bandRc;
    bool open = false;
    Dock* dock = nullptr;
    float scale = 1;
    int w = 0, h = 0;
    RECT rc{};
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    int dibW = 0, dibH = 0;
    ComPtr<ID2D1DCRenderTarget> rt;
    ComPtr<ID2D1DeviceContext> dc;
    ComPtr<IDWriteTextFormat> font, small, section, glyphs, big, head;
    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> bmps;
    std::wstring query;
    size_t caret = 0, anchor = 0;  // text cursor and the other end of the selection (equal: no selection)
    float textScroll = 0;           // search text scrolled left when it doesn't fit
    D2D1_RECT_F textRc{};           // search text box (panel coordinates)
    bool selecting = false;         // mouse held down in the search text
    struct {
        bool down = false, active = false;
        int cur = -1;  // the dragged pin's index in g_startPins
        int sx = 0, sy = 0;
    } pinDrag;  // reordering the pinned apps
    bool org = false;  // Status tab: organizing its parts
    struct {
        bool down = false, active = false;
        int sec = -1;
        int sx = 0, sy = 0;
    } secDrag;  // a Status part being dragged
    bool sliding = false;  // a Status slider held down
    std::function<void(float)> slideFn;
    float slideX0 = 0, slideX1 = 1;
    DWORD lastTextClick = 0;
    int tab = 0;  // 0 pinned, 1 all apps, 2 downloads, 3 status (statusPos: iniciar)
    float scroll = 0, maxScroll = 0;
    int hover = -1, sel = 0;
    int kbFocus = -1;  // the row chosen with the keyboard (arrows, Tab) while nothing is typed
    std::vector<StartRow> rows;
    std::vector<StartItem*> appHits;
    std::vector<StartItem> files;
    EverythingState ev = EvOk;
    bool caretOn = true;
    bool menuOpen = false;
    DWORD openedAt = 0;
    // Open/close animation: rises from the dock, fading in and growing a little (with a soft
    // overshoot); closes faster, sinking and fading out.
    float anim = 0, animFrom = 0;
    bool closing = false;
    DWORD animStart = 0;
    // Wide search: the panel widens (both sides) while searching.
    float wide = 0, wideFrom = 0;
    bool wideTarget = false;
    DWORD wideStart = 0;
    float offX = 0;  // panel's left edge inside the window
    std::vector<StartItem*> setHits;
    int powerArm = -1;  // restart or shut down clicked once: a second click within 4 s does it
    DWORD powerArmAt = 0;
} g_start;

constexpr UINT_PTR TIMER_START_WIDE = 23;
constexpr UINT_PTR TIMER_START_TRAY = 26;  // after a click on a tray icon: close, unless a second click came
constexpr DWORD kStartWideMs = 170;

// Windows Settings pages the wide search can find (name, page, other words).
std::vector<StartItem> g_settingsItems;
static void LoadSettingsItems() {
    if (!g_settingsItems.empty()) return;
    // English name, Portuguese name, page, other words (both languages: a search in either finds it).
    static const wchar_t* table[][4] = {
        {L"Display", L"Tela", L"ms-settings:display", L"monitor resolution scale brightness hdr resolução escala brilho"},
        {L"Night light", L"Luz noturna", L"ms-settings:nightlight", L"blue light night filtro azul noite"},
        {L"Graphics", L"Gráficos", L"ms-settings:display-advancedgraphics", L"gpu graphics card games performance placa de vídeo jogos desempenho"},
        {L"Sound", L"Som", L"ms-settings:sound", L"audio volume microphone headphones speakers áudio microfone fone alto-falante"},
        {L"Volume mixer", L"Mixer de volume", L"ms-settings:apps-volume", L"app volume audio volume por app áudio"},
        {L"Notifications", L"Notificações", L"ms-settings:notifications", L"alerts avisos alertas"},
        {L"Focus", L"Modo de foco", L"ms-settings:quiethours", L"do not disturb não perturbe concentração"},
        {L"Power", L"Energia", L"ms-settings:powersleep", L"battery sleep screen off plan bateria suspensão desligar tela plano"},
        {L"Storage", L"Armazenamento", L"ms-settings:storagesense", L"disk space cleanup disco espaço limpeza"},
        {L"Multitasking", L"Multitarefa", L"ms-settings:multitasking", L"windows snap desktops janelas área de trabalho"},
        {L"Clipboard", L"Área de transferência", L"ms-settings:clipboard", L"copy paste history copiar colar histórico"},
        {L"About", L"Sobre", L"ms-settings:about", L"system pc name specs sistema nome especificações"},
        {L"Bluetooth & devices", L"Bluetooth e dispositivos", L"ms-settings:bluetooth", L"headphones controller device fone controle dispositivo"},
        {L"Printers", L"Impressoras", L"ms-settings:printers", L"printer scanner impressora"},
        {L"Mouse", L"Mouse", L"ms-settings:mousetouchpad", L"cursor pointer sensitivity ponteiro sensibilidade"},
        {L"Typing", L"Teclado e digitação", L"ms-settings:typing", L"keyboard autocorrect teclado digitar correção"},
        {L"Network & internet", L"Rede e Internet", L"ms-settings:network", L"internet ethernet connection conexão"},
        {L"Wi-Fi", L"Wi-Fi", L"ms-settings:network-wifi", L"wifi wireless network rede sem fio"},
        {L"VPN", L"VPN", L"ms-settings:network-vpn", L"vpn network rede"},
        {L"Proxy", L"Proxy", L"ms-settings:network-proxy", L"proxy network rede"},
        {L"Personalization", L"Personalização", L"ms-settings:personalization", L"look theme appearance visual tema aparência"},
        {L"Background", L"Plano de fundo", L"ms-settings:personalization-background", L"wallpaper papel de parede"},
        {L"Colors", L"Cores", L"ms-settings:colors", L"dark light mode accent color transparency modo escuro claro cor de destaque transparência"},
        {L"Themes", L"Temas", L"ms-settings:themes", L"theme tema"},
        {L"Lock screen", L"Tela de bloqueio", L"ms-settings:lockscreen", L"lock bloqueio"},
        {L"Fonts", L"Fontes", L"ms-settings:fonts", L"font typeface fonte letra"},
        {L"Taskbar", L"Barra de tarefas", L"ms-settings:taskbar", L"taskbar barra"},
        {L"Installed apps", L"Aplicativos instalados", L"ms-settings:appsfeatures", L"uninstall programs apps desinstalar programas"},
        {L"Default apps", L"Aplicativos padrão", L"ms-settings:defaultapps", L"default browser open with navegador padrão abrir com"},
        {L"Startup apps", L"Inicialização", L"ms-settings:startupapps", L"startup run at sign-in iniciar com o windows apps"},
        {L"Accounts", L"Contas", L"ms-settings:accounts", L"user account email usuário conta"},
        {L"Sign-in options", L"Opções de entrada", L"ms-settings:signinoptions", L"password pin windows hello senha"},
        {L"Date & time", L"Data e hora", L"ms-settings:dateandtime", L"time clock time zone date hora relógio fuso horário data"},
        {L"Language & region", L"Idioma e região", L"ms-settings:regionlanguage", L"language region format idioma região formato"},
        {L"Game Mode", L"Modo de jogo", L"ms-settings:gaming-gamemode", L"games gaming jogos game"},
        {L"Accessibility", L"Acessibilidade", L"ms-settings:easeofaccess", L"magnifier narrator contrast lupa narrador contraste"},
        {L"Privacy & security", L"Privacidade e segurança", L"ms-settings:privacy", L"privacy permissions camera location privacidade permissões câmera localização"},
        {L"Windows Security", L"Segurança do Windows", L"windowsdefender:", L"antivirus defender protection virus antivírus proteção vírus"},
        {L"Windows Update", L"Windows Update", L"ms-settings:windowsupdate", L"update updates atualização atualizar"},
        {L"Recovery", L"Recuperação", L"ms-settings:recovery", L"restore reset restaurar redefinir"},
    };
    bool pt = IsPortugueseUi();
    for (auto& row : table) {
        StartItem s;
        s.setting = true;
        s.name = pt ? row[1] : row[0];
        s.launch = row[2];
        s.keywords = std::wstring(pt ? row[0] : row[1]) + L" " + row[3];
        s.path = Tr(S_Settings);
        g_settingsItems.push_back(std::move(s));
    }
}

enum WideAction { ActOpenSel = 10, ActPinSel, ActLocationSel };
static StartItem* SelectedResult();
static void CloseStart(bool animate = false);

constexpr UINT_PTR TIMER_START_ANIM = 22;
constexpr UINT_PTR TIMER_START_POWER = 27;  // an armed power button goes back
constexpr DWORD kStartOpenMs = 220, kStartCloseMs = 140;

std::vector<StartItem> g_allApps;
std::vector<StartItem> g_startPins;
bool g_allAppsLoaded = false;

constexpr UINT_PTR TIMER_CARET = 20;
constexpr UINT_PTR TIMER_SEARCH = 21;
constexpr ULONG_PTR kEverythingReply = 0x4E455641;  // our id for Everything's answers

static ComPtr<IWICBitmapSource> IconFromItem(IShellItem* item, int size) {
    ComPtr<IShellItemImageFactory> factory;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&factory)))) return {};
    HBITMAP hbm = nullptr;
    SIZE sz = {size, size};
    if (FAILED(factory->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbm)) || !hbm) return {};
    ComPtr<IWICBitmap> wb;
    HRESULT hr = g_wic->CreateBitmapFromHBITMAP(hbm, nullptr, WICBitmapUseAlpha, &wb);
    DeleteObject(hbm);
    if (FAILED(hr)) return {};
    ComPtr<IWICFormatConverter> conv;
    if (FAILED(g_wic->CreateFormatConverter(&conv))) return {};
    if (FAILED(conv->Initialize(wb.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0,
                                WICBitmapPaletteTypeCustom))) return {};
    ComPtr<IWICBitmap> out;
    if (FAILED(g_wic->CreateBitmapFromSource(conv.Get(), WICBitmapCacheOnLoad, &out))) return {};
    return out;
}

static std::wstring ItemName(IShellItem* item) {
    std::wstring s;
    PWSTR n = nullptr;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &n)) && n) {
        s = n;
        CoTaskMemFree(n);
    }
    return s;
}

// A program installed or removed (its Start Menu shortcut comes or goes): All apps loads again the next
// time it's needed. Windows tells about changes in the two Start Menu folders.
static HANDLE g_appsWatch[2];
static void AppsWatchTick() {
    static bool started = false;
    if (!started) {
        started = true;
        int folders[2] = {CSIDL_PROGRAMS, CSIDL_COMMON_PROGRAMS};
        for (int k = 0; k < 2; k++) {
            wchar_t dir[MAX_PATH];
            g_appsWatch[k] = SUCCEEDED(SHGetFolderPathW(nullptr, folders[k], nullptr, 0, dir))
                                 ? FindFirstChangeNotificationW(dir, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME)
                                 : INVALID_HANDLE_VALUE;
        }
    }
    for (HANDLE& h : g_appsWatch) {
        if (h == INVALID_HANDLE_VALUE || !h || WaitForSingleObject(h, 0) != WAIT_OBJECT_0) continue;
        FindNextChangeNotification(h);
        g_allAppsLoaded = false;  // (loaded again when the Start opens or shows All apps)
        Wh_Log(L"start: the Start Menu folders changed, All apps reloads");
    }
}
static void AppsWatchStop() {
    for (HANDLE& h : g_appsWatch) {
        if (h && h != INVALID_HANDLE_VALUE) FindCloseChangeNotification(h);
        h = nullptr;
    }
}

static void LoadAllApps() {
    g_allApps.clear();
    ComPtr<IShellItem> folder;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder)))) return;
    ComPtr<IEnumShellItems> en;
    if (FAILED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&en)))) return;
    IShellItem* it = nullptr;
    while (en->Next(1, &it, nullptr) == S_OK) {
        StartItem s;
        s.name = ItemName(it);
        PWSTR id = nullptr;
        if (SUCCEEDED(it->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &id)) && id) {
            s.launch = std::wstring(L"shell:AppsFolder\\") + id;
            CoTaskMemFree(id);
        }
        it->Release();
        if (!s.name.empty() && !s.launch.empty()) g_allApps.push_back(std::move(s));
    }
    std::sort(g_allApps.begin(), g_allApps.end(), [](const StartItem& a, const StartItem& b) {
        return CompareStringEx(LOCALE_NAME_USER_DEFAULT, LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC, a.name.c_str(), -1,
                               b.name.c_str(), -1, nullptr, nullptr, 0) == CSTR_LESS_THAN;
    });
    g_allAppsLoaded = true;
}

static void SaveStartPins() {
    std::vector<std::pair<std::wstring, std::wstring>> items;
    for (auto& p : g_startPins) items.push_back({p.name, p.launch});
    SaveList(L"startPin", items);
}

static void LoadStartPins() {
    g_startPins.clear();
    std::vector<std::pair<std::wstring, std::wstring>> items;
    if (!LoadList(L"startPin", items)) {
        if (!LoadLegacyList(L"startPins", items)) {
            // First time: the same apps pinned on the taskbar.
            for (auto& lnk : TaskbarPinnedLinks()) {
                ComPtr<IShellItem> item;
                if (FAILED(SHCreateItemFromParsingName(lnk.c_str(), nullptr, IID_PPV_ARGS(&item)))) continue;
                std::wstring name = ItemName(item.Get());
                if (!name.empty()) items.push_back({name, lnk});
            }
        }
        Wh_DeleteValue(L"startPinsInit");
        for (auto& it : items) {
            StartItem s;
            s.name = it.first;
            s.launch = it.second;
            g_startPins.push_back(std::move(s));
        }
        SaveStartPins();
        return;
    }
    for (auto& it : items) {
        StartItem s;
        s.name = it.first;
        s.launch = it.second;
        g_startPins.push_back(std::move(s));
    }
}

static void RenderStart();

// Everything.exe, if installed (App Paths, then the usual folders).
static std::wstring EverythingExe() {
    wchar_t buf[MAX_PATH];
    DWORD size = sizeof(buf);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Everything.exe",
                     nullptr, RRF_RT_REG_SZ | RRF_SUBKEY_WOW6464KEY, nullptr, buf, &size) == ERROR_SUCCESS &&
        GetFileAttributesW(buf) != INVALID_FILE_ATTRIBUTES)
        return buf;
    for (const wchar_t* var : {L"ProgramW6432", L"ProgramFiles", L"ProgramFiles(x86)", L"LOCALAPPDATA"}) {
        wchar_t base[MAX_PATH];
        if (!GetEnvironmentVariableW(var, base, MAX_PATH)) continue;
        std::wstring p = std::wstring(base) + (wcscmp(var, L"LOCALAPPDATA") ? L"\\Everything\\Everything.exe"
                                                                            : L"\\Programs\\Everything\\Everything.exe");
        if (GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES) return p;
    }
    return L"";
}

static std::wstring g_evAskedFor;  // (the search the last question to Everything was for)
static void QueryEverything() {
    g_start.files.clear();
    g_evAskedFor = g_start.query;
    g_start.ev = EvOk;
    if (g_start.query.size() < 2 || g_set.fileSearch != L"everything") return;
    HWND ev = FindWindowW(L"EVERYTHING_TASKBAR_NOTIFICATION", nullptr);
    if (!ev) {
        g_start.ev = EverythingExe().empty() ? EvMissing : EvClosed;
        return;
    }
    // EVERYTHING_IPC_QUERYW: reply window, reply id, flags, offset, max results, search text.
    size_t len = g_start.query.size();
    std::vector<BYTE> buf(sizeof(DWORD) * 5 + (len + 1) * sizeof(WCHAR));
    DWORD* p = (DWORD*)buf.data();
    p[0] = (DWORD)(UINT_PTR)g_start.hwnd;
    p[1] = (DWORD)kEverythingReply;
    p[2] = 0;
    p[3] = 0;
    p[4] = 8;
    memcpy(buf.data() + sizeof(DWORD) * 5, g_start.query.c_str(), (len + 1) * sizeof(WCHAR));
    COPYDATASTRUCT cds = {2 /* EVERYTHING_IPC_COPYDATAQUERYW */, (DWORD)buf.size(), buf.data()};
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(ev, WM_COPYDATA, (WPARAM)g_start.hwnd, (LPARAM)&cds, SMTO_ABORTIFHUNG, 500, &result) ||
        !result) {
        g_start.ev = EvClosed;
    }
}

static void RunStartAction(int action) {
    switch (action) {
        case ActOpenEverything: {
            std::wstring exe = EverythingExe();
            if (!exe.empty()) ShellExecuteW(nullptr, L"open", exe.c_str(), L"-startup", nullptr, SW_SHOWNORMAL);
            // Ask again once it's up.
            SetTimer(g_start.hwnd, TIMER_SEARCH, 1500, nullptr);
            break;
        }
        case ActExplorerSearch: {
            // Windows' own search, in an Explorer window (slower, but needs nothing installed).
            std::wstring q;
            for (wchar_t c : g_start.query) {
                if (iswalnum(c) || c == L'.' || c == L'-' || c == L'_') q += c;
                else {
                    wchar_t hex[8];
                    swprintf_s(hex, L"%%%02X", (unsigned)(c & 0xFF));
                    q += c > 0xFF ? std::wstring(1, c) : std::wstring(hex);
                }
            }
            std::wstring uri = L"search-ms:query=" + q + L"&crumb=location:" +
                               std::wstring(_wgetenv(L"USERPROFILE") ? _wgetenv(L"USERPROFILE") : L"C:\\");
            ShellExecuteW(nullptr, L"open", uri.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
        }
        case ActGetEverything:
            ShellExecuteW(nullptr, L"open", L"https://www.voidtools.com/", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case ActOpenSel:
            if (StartItem* it = SelectedResult()) {
                std::wstring target = NativePath(it->launch);
                CloseStart(true);
                ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }
            break;
        case ActLocationSel:
            if (StartItem* it = SelectedResult()) {
                std::wstring args = L"/select,\"" + it->launch + L"\"";
                CloseStart(true);
                ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
            }
            break;
        case ActPinSel:
            if (StartItem* it = SelectedResult()) {
                std::wstring id = it->launch.rfind(L"shell:AppsFolder\\", 0) == 0 ? it->launch.substr(17) : L"";
                std::wstring value = !id.empty() ? id : it->launch;
                if (std::any_of(g_pins.begin(), g_pins.end(), [&](const Pin& p) { return EqI(p.value, value); })) break;  // (pinned already)
                if (!id.empty()) g_pins.push_back({L"aumid", id});
                else if (EqI(PathFindExtensionW(it->launch.c_str()), L".lnk")) g_pins.push_back({L"lnk", it->launch});
                else g_pins.push_back({L"exe", it->launch});
                SavePins();
                RebuildApps();
                for (auto* d : g_docks) { BuildElems(*d, true); Invalidate(*d); }
            }
            break;
    }
}

// Any process of this user can send this message: a string must end inside the data.
static bool ReplyString(const COPYDATASTRUCT* cds, DWORD off, std::wstring& out) {
    if (off >= cds->cbData || (off & 1)) return false;
    size_t maxChars = (cds->cbData - off) / sizeof(wchar_t);
    const wchar_t* p = (const wchar_t*)((const BYTE*)cds->lpData + off);
    size_t n = wcsnlen(p, maxChars);
    if (n == maxChars) return false;
    out.assign(p, n);
    return true;
}

static void OnEverythingReply(const COPYDATASTRUCT* cds) {
    if (!g_start.open || !cds->lpData || cds->cbData < sizeof(DWORD) * 7) return;
    if (g_start.query.size() < 2 || g_start.query != g_evAskedFor) return;  // (an answer to an older search)
    const BYTE* base = (const BYTE*)cds->lpData;
    const DWORD* list = (const DWORD*)base;
    DWORD num = list[5];
    struct ItemW { DWORD flags, nameOffset, pathOffset; };
    const ItemW* items = (const ItemW*)(list + 7);
    g_start.files.clear();
    for (DWORD i = 0; i < num && i < 8; i++) {
        if ((const BYTE*)(items + i + 1) > base + cds->cbData) break;
        StartItem f;
        f.file = true;
        f.folder = (items[i].flags & 1) != 0;
        if (!ReplyString(cds, items[i].nameOffset, f.name) || !ReplyString(cds, items[i].pathOffset, f.path)) break;
        f.launch = f.path + L"\\" + f.name;
        g_start.files.push_back(std::move(f));
    }
    RenderStart();
}

// ---- "Downloads" tab: files that came from the internet (any folder), newest first ----
// Windows marks them with a Zone.Identifier stream (the "this file came from another computer"
// warning). Everything lists the files changed in the last 30 days, newest first; the ones with that
// mark are shown. Without Everything: the Downloads folder.

constexpr ULONG_PTR kEverythingRecentReply = 0x4E455642;
constexpr size_t kRecentMax = 30;
std::vector<StartItem> g_recent;
bool g_recentLoaded = false;

static std::wstring AgoText(ULONGLONG when) {
    FILETIME nowFt;
    GetSystemTimeAsFileTime(&nowFt);
    ULONGLONG now = ((ULONGLONG)nowFt.dwHighDateTime << 32) | nowFt.dwLowDateTime;
    long long mins = now > when ? (long long)((now - when) / 600000000ULL) : 0;
    wchar_t buf[64];
    if (mins < 1) return Tr(S_AgoNow);
    if (mins < 60) swprintf_s(buf, Tr(S_AgoMin), (int)mins);
    else if (mins < 60 * 24) swprintf_s(buf, Tr(S_AgoHour), (int)(mins / 60));
    else swprintf_s(buf, Tr(S_AgoDay), (int)(mins / (60 * 24)));
    return buf;
}

static void AddRecent(const std::wstring& full, ULONGLONG when) {
    StartItem it;
    it.file = true;
    it.launch = full;
    it.name = PathFindFileNameW(full.c_str());
    std::wstring dir = full.substr(0, full.size() - it.name.size());
    if (dir.size() > 3 && dir.back() == L'\\') dir.pop_back();
    it.path = AgoText(when) + L"   " + dir;
    it.when = when;
    g_recent.push_back(std::move(it));
}

static bool CameFromInternet(const std::wstring& full) {
    return GetFileAttributesW((full + L":Zone.Identifier").c_str()) != INVALID_FILE_ATTRIBUTES;
}

static void LoadRecentFromDownloads() {
    PWSTR dl = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &dl)) || !dl) return;
    std::wstring dir = dl;
    CoTaskMemFree(dl);
    struct F { std::wstring path; ULONGLONG when; };
    std::vector<F> files;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW((dir + L"\\*").c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, 0);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) continue;
        ULONGLONG w = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime;
        ULONGLONG c = ((ULONGLONG)fd.ftCreationTime.dwHighDateTime << 32) | fd.ftCreationTime.dwLowDateTime;
        files.push_back({dir + L"\\" + fd.cFileName, std::max(w, c)});
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    std::sort(files.begin(), files.end(), [](const F& a, const F& b) { return a.when > b.when; });
    for (size_t i = 0; i < files.size() && i < kRecentMax; i++) AddRecent(files[i].path, files[i].when);
}

// Newest first, each file once, at most kRecentMax.
static void FinishRecent() {
    std::stable_sort(g_recent.begin(), g_recent.end(), [](const StartItem& a, const StartItem& b) { return a.when > b.when; });
    std::vector<StartItem> out;
    for (auto& it : g_recent) {
        if (out.size() >= kRecentMax) break;
        if (std::none_of(out.begin(), out.end(), [&](const StartItem& o) { return EqI(o.launch, it.launch); }))
            out.push_back(std::move(it));
    }
    g_recent = std::move(out);
}

static void LoadRecent() {
    g_recent.clear();
    g_recentLoaded = true;
    // Everything in the Downloads folder (many browsers and apps don't mark their files)...
    LoadRecentFromDownloads();
    // ...plus files with the "from the internet" mark anywhere else (Everything's answer adds them).
    HWND ev = g_set.fileSearch == L"everything" ? FindWindowW(L"EVERYTHING_TASKBAR_NOTIFICATION", nullptr) : nullptr;
    if (ev) {
        // EVERYTHING_IPC_QUERY2W: reply window, reply id, search flags, offset, max results, request
        // flags (full path 0x4, date modified 0x40, attributes 0x100), sort (date modified, newest first).
        const wchar_t* search =
            L"file: dm:last30days !\\AppData\\ !\\Windows\\ !\\$Recycle.Bin\\ !\\ProgramData\\ "
            L"!\"\\Program Files\" !\\node_modules\\ !\\.git\\";
        size_t len = wcslen(search);
        std::vector<BYTE> buf(sizeof(DWORD) * 7 + (len + 1) * sizeof(WCHAR));
        DWORD* p = (DWORD*)buf.data();
        p[0] = (DWORD)(UINT_PTR)g_start.hwnd;
        p[1] = (DWORD)kEverythingRecentReply;
        p[2] = 0;
        p[3] = 0;
        p[4] = 1500;
        p[5] = 0x4 | 0x40 | 0x100;
        p[6] = 14;  // EVERYTHING_SORT_DATE_MODIFIED_DESCENDING
        memcpy(buf.data() + sizeof(DWORD) * 7, search, (len + 1) * sizeof(WCHAR));
        COPYDATASTRUCT cds = {18 /* EVERYTHING_IPC_COPYDATA_QUERY2W */, (DWORD)buf.size(), buf.data()};
        DWORD_PTR result = 0;
        if (SendMessageTimeoutW(ev, WM_COPYDATA, (WPARAM)g_start.hwnd, (LPARAM)&cds, SMTO_ABORTIFHUNG, 500, &result) &&
            result)
            return;  // the answer comes as WM_COPYDATA (OnEverythingRecentReply)
        Wh_Log(L"downloads: Everything didn't take the query");
    }
    FinishRecent();
}

static void OnEverythingRecentReply(const COPYDATASTRUCT* cds) {
    // EVERYTHING_IPC_LIST2: total, count, offset, request flags, sort, then {flags, data offset} per
    // item; each item's data: full path (DWORD length, text, null), date modified (FILETIME), attributes.
    if (!cds->lpData || cds->cbData < sizeof(DWORD) * 5) return;
    const BYTE* base = (const BYTE*)cds->lpData;
    const DWORD* list = (const DWORD*)base;
    DWORD num = list[1], flags = list[3];
    if (flags != (0x4 | 0x40 | 0x100)) {
        Wh_Log(L"downloads: unexpected reply flags %X", flags);
        FinishRecent();
        if (g_start.open) RenderStart();
        return;
    }
    struct Item2 { DWORD flags, dataOffset; };
    const Item2* items = (const Item2*)(list + 5);
    // (Any process of this user can send this message: offsets are checked as integers.)
    size_t numItems = std::min<size_t>(num, (cds->cbData - sizeof(DWORD) * 5) / sizeof(Item2));
    int checked = 0, marked = 0;
    // The checks below run on the UI thread: stop after a short while, and only look at fixed drives
    // (a sleeping USB disk or a slow network share would freeze the dock).
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(150);
    BYTE fixedDrives[26] = {};  // 0 unknown, 1 fixed, 2 other
    for (size_t i = 0; i < numItems && marked < (int)kRecentMax; i++) {
        if (std::chrono::steady_clock::now() > deadline) break;
        DWORD off = items[i].dataOffset;
        if (off > cds->cbData || cds->cbData - off < sizeof(DWORD)) break;
        const BYTE* d = base + off;
        size_t len = *(const DWORD UNALIGNED*)d;
        d += sizeof(DWORD);
        // text, null, FILETIME (4 WCHARs), attributes (2 WCHARs); subtract, never add (32-bit size_t)
        size_t avail = (cds->cbData - off - sizeof(DWORD)) / sizeof(WCHAR);
        if (avail < 7 || len > avail - 7) break;
        std::wstring full((const wchar_t*)d, len);
        d += (len + 1) * sizeof(WCHAR);
        ULONGLONG when = *(const ULONGLONG UNALIGNED*)d;
        d += 8;
        DWORD attr = *(const DWORD UNALIGNED*)d;
        // Cloud placeholders (OneDrive "online only"): reading their streams could download them.
        if (attr & (FILE_ATTRIBUTE_OFFLINE | 0x00040000 /* RECALL_ON_OPEN */ | 0x00400000 /* RECALL_ON_DATA_ACCESS */))
            continue;
        if (full.size() < 3 || full[1] != L':' || !iswalpha(full[0])) continue;
        int drive = towupper(full[0]) - L'A';
        if (drive < 0 || drive >= 26) continue;
        if (!fixedDrives[drive]) {
            wchar_t root[4] = {full[0], L':', L'\\', 0};
            fixedDrives[drive] = GetDriveTypeW(root) == DRIVE_FIXED ? 1 : 2;
        }
        if (fixedDrives[drive] != 1) continue;
        checked++;
        if (CameFromInternet(full)) {
            AddRecent(full, when);
            marked++;
        }
    }
    FinishRecent();
    Wh_Log(L"downloads: %u files from Everything, %d checked, %d marked, %zu shown", num, checked, marked, g_recent.size());
    if (g_start.open) RenderStart();
}

// ---- Search ranking ----
// Lower case, no accents ("Configurações" → "configuracoes").
static std::wstring FoldText(const std::wstring& s) {
    std::wstring o;
    if (s.empty()) return o;
    int n = NormalizeString(NormalizationD, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring d(std::max(n, 0) + 1, L'\0');
    n = NormalizeString(NormalizationD, s.c_str(), (int)s.size(), d.data(), (int)d.size());
    if (n <= 0) d = s;
    else d.resize(n);
    for (wchar_t c : d) {
        if (c >= 0x300 && c <= 0x36F) continue;  // (the accents, split off)
        o += towlower(c);
    }
    return o;
}

static bool IsSep(wchar_t c) { return !iswalnum(c); }

// Edits between two short words (a letter wrong, missing, extra or swapped counts one).
static int EditDistance(const std::wstring& a, const std::wstring& b, int cap) {
    size_t n = a.size(), m = b.size();
    if ((int)(n > m ? n - m : m - n) > cap) return cap + 1;
    std::vector<int> pp(m + 1), p(m + 1), c(m + 1);
    for (size_t j = 0; j <= m; j++) p[j] = (int)j;
    for (size_t i = 1; i <= n; i++) {
        c[0] = (int)i;
        int best = c[0];
        for (size_t j = 1; j <= m; j++) {
            int cost = a[i - 1] == b[j - 1] ? 0 : 1;
            c[j] = std::min({p[j] + 1, c[j - 1] + 1, p[j - 1] + cost});
            if (i > 1 && j > 1 && a[i - 1] == b[j - 2] && a[i - 2] == b[j - 1]) c[j] = std::min(c[j], pp[j - 2] + 1);
            best = std::min(best, c[j]);
        }
        if (best > cap) return cap + 1;
        pp = p;
        p = c;
    }
    return p[m];
}

// How well a (folded) name answers a (folded) query: 0 not at all, higher is better.
//   the whole name 1000, its start 900, a word's start 800, the words' first letters ("vsc" → Visual
//   Studio Code) 750, inside a word 500, all of several words 450, one typo in a word's start 280.
static int MatchScore(const std::wstring& name, const std::wstring& q) {
    if (q.empty() || name.empty()) return 0;
    if (name == q) return 1000;
    if (name.rfind(q, 0) == 0) return 900;
    std::vector<size_t> starts;
    for (size_t i = 0; i < name.size(); i++)
        if (!IsSep(name[i]) && (i == 0 || IsSep(name[i - 1]) || (iswdigit(name[i]) && !iswdigit(name[i - 1])))) starts.push_back(i);
    for (size_t st : starts)
        if (name.compare(st, q.size(), q) == 0) return 800;
    // initials: each query letter starts the next word (a word's next letters may follow: "vscode")
    if (q.size() >= 2 && q.find(L' ') == std::wstring::npos) {
        std::function<bool(size_t, size_t)> init = [&](size_t qi, size_t wi) -> bool {
            if (qi == q.size()) return true;
            for (size_t w = wi; w < starts.size(); w++) {
                size_t k = 0;
                while (qi + k < q.size() && starts[w] + k < name.size() && !IsSep(name[starts[w] + k]) && name[starts[w] + k] == q[qi + k]) {
                    k++;
                    if (init(qi + k, w + 1)) return true;
                }
            }
            return false;
        };
        if (init(0, 0)) return 750;
    }
    if (name.find(q) != std::wstring::npos) return 500;
    // several query words: each one somewhere
    if (q.find(L' ') != std::wstring::npos) {
        bool all = true;
        size_t a = 0;
        while (all && a < q.size()) {
            size_t b = q.find(L' ', a);
            std::wstring w = q.substr(a, b == std::wstring::npos ? std::wstring::npos : b - a);
            if (!w.empty() && name.find(w) == std::wstring::npos) all = false;
            if (b == std::wstring::npos) break;
            a = b + 1;
        }
        if (all) return 450;
    }
    // a typo: the query against each word's start of the same length
    if (q.size() >= 4)
        for (size_t st : starts) {
            size_t end = st;
            while (end < name.size() && !IsSep(name[end])) end++;
            std::wstring w = name.substr(st, std::min(end - st, q.size() + 1));
            std::wstring w2 = name.substr(st, std::min(end - st, q.size()));
            if (EditDistance(q, w, 1) <= 1 || EditDistance(q, w2, 1) <= 1) return 280;
        }
    return 0;
}

// What people often type for an app whose name says something else.
static const wchar_t* const kSearchAliases[][2] = {
    {L"cmd", L"prompt de comando"}, {L"cmd", L"command prompt"}, {L"terminal", L"windows terminal"}, {L"ps", L"powershell"},
    {L"config", L"configuracoes"}, {L"settings", L"configuracoes"}, {L"control", L"painel de controle"}, {L"regedit", L"editor do registro"},
    {L"task", L"gerenciador de tarefas"}, {L"taskmgr", L"gerenciador de tarefas"}, {L"calc", L"calculadora"}, {L"notepad", L"bloco de notas"},
    {L"paint", L"paint"}, {L"explorer", L"explorador de arquivos"}, {L"word", L"word"}, {L"vscode", L"visual studio code"},
    {L"code", L"visual studio code"}, {L"zap", L"whatsapp"}, {L"wpp", L"whatsapp"}, {L"insta", L"instagram"}, {L"yt", L"youtube"}};

// How many times each thing was opened from the Start (it comes up first next time).
static std::unordered_map<std::wstring, int> g_usage;
static bool g_usageLoaded = false;
static void LoadUsage() {
    if (g_usageLoaded) return;
    g_usageLoaded = true;
    std::vector<std::wstring> lines;
    LoadItems(L"start.usage", L'\n', lines);
    for (auto& line : lines) {
        size_t bar = line.find(L'|');
        if (bar != std::wstring::npos) g_usage[line.substr(bar + 1)] = _wtoi(line.substr(0, bar).c_str());
    }
}
static void NoteUsage(const std::wstring& launch) {
    if (launch.empty()) return;
    LoadUsage();
    g_usage[launch]++;
    std::vector<std::pair<int, std::wstring>> v;
    for (auto& u : g_usage) v.push_back({u.second, u.first});
    std::sort(v.begin(), v.end(), [](auto& x, auto& y) { return x.first > y.first; });
    if (v.size() > 150) v.resize(150);
    std::vector<std::wstring> lines;
    for (auto& e : v) lines.push_back(std::to_wstring(e.first) + L"|" + e.second);
    StoreItems(L"start.usage", lines);
}

static int ItemScore(StartItem& it, const std::wstring& q) {
    if (it.fold.empty()) {
        it.fold = FoldText(it.name);
        std::wstring extra = it.setting ? it.keywords : it.launch;
        size_t bs = extra.find_last_of(L"\\!");
        if (!it.setting && bs != std::wstring::npos) extra = extra.substr(0, bs) + L" " + extra.substr(bs + 1);
        it.foldExtra = FoldText(extra);
    }
    int sc = MatchScore(it.fold, q);
    for (auto& al : kSearchAliases)
        if (q == al[0] && it.fold.find(al[1]) != std::wstring::npos) sc = std::max(sc, 850);
    if (!sc && q.size() >= 3 && it.foldExtra.find(q) != std::wstring::npos) sc = 400;  // the app's id or the setting's words
    if (!sc) return 0;
    auto u = g_usage.find(it.launch);
    if (u != g_usage.end()) sc += std::min(u->second * 12, 120);
    return sc;
}

static void PlaceStartBands();

static void UpdateSearch() {
    g_start.appHits.clear();
    g_start.setHits.clear();
    if (!g_start.query.empty()) {
        if (!g_allAppsLoaded) LoadAllApps();
        LoadUsage();
        std::wstring q = FoldText(g_start.query);
        while (!q.empty() && q.back() == L' ') q.pop_back();
        while (!q.empty() && q.front() == L' ') q.erase(0, 1);
        std::vector<std::pair<int, StartItem*>> found;
        for (auto& a : g_allApps)
            if (int sc = ItemScore(a, q)) found.push_back({sc, &a});
        std::stable_sort(found.begin(), found.end(), [](auto& x, auto& y) { return x.first > y.first; });
        for (size_t i = 0; i < found.size() && i < 6; i++) g_start.appHits.push_back(found[i].second);
        if (g_set.wideSearch) {
            LoadSettingsItems();
            std::vector<std::pair<int, StartItem*>> sf;
            for (auto& st : g_settingsItems)
                if (int sc = ItemScore(st, q)) sf.push_back({sc, &st});
            std::stable_sort(sf.begin(), sf.end(), [](auto& x, auto& y) { return x.first > y.first; });
            for (size_t i = 0; i < sf.size() && i < 3; i++) g_start.setHits.push_back(sf[i].second);
        }
    }
    // Wide search: widen while there's a search, back when it's cleared.
    bool want = g_set.wideSearch && !g_start.query.empty();
    if (want != g_start.wideTarget) {
        g_start.wideTarget = want;
        g_start.wideFrom = g_start.wide;
        g_start.wideStart = GetTickCount();
        HideBands(g_start.bands, g_start.bandRc);
        SetTimer(g_start.hwnd, TIMER_START_WIDE, 15, nullptr);
    }
    g_start.sel = 0;
    KillTimer(g_start.hwnd, TIMER_SEARCH);
    if (g_start.query.size() >= 2) SetTimer(g_start.hwnd, TIMER_SEARCH, 120, nullptr);
    else g_start.files.clear();
}

static void FinishCloseStart() {
    g_start.powerArm = -1;
    KillTimer(g_start.hwnd, TIMER_START_ANIM);
    KillTimer(g_start.hwnd, TIMER_START_WIDE);
    ShowWindow(g_start.hwnd, SW_HIDE);
    g_start.closing = false;
    g_start.anim = 0;
    g_start.wide = 0;
    g_start.wideTarget = false;
    g_start.query.clear();
    g_start.files.clear();
    g_start.appHits.clear();
    g_start.setHits.clear();
    g_start.scroll = 0;
    g_start.hover = -1;
}

static void CloseStart(bool animate) {
    if (!g_start.open) {
        if (!animate && g_start.closing) FinishCloseStart();
        return;
    }
    g_start.open = false;
    if (FlyoutFromStartOpen()) CloseFlyout();
    extern void StartMouseHookOff();
    StartMouseHookOff();
    KillTimer(g_start.hwnd, TIMER_CARET);
    KillTimer(g_start.hwnd, TIMER_SEARCH);
    HideBands(g_start.bands, g_start.bandRc);
    if (g_start.dock) {
        g_start.dock->outsideSince = GetTickCount();
        Invalidate(*g_start.dock);
    }
    g_start.dock = nullptr;
    if (animate) {
        g_start.closing = true;
        g_start.animFrom = g_start.anim;
        g_start.animStart = GetTickCount();
        SetTimer(g_start.hwnd, TIMER_START_ANIM, 15, nullptr);
    } else {
        FinishCloseStart();
    }
}

static void LaunchItem(const StartItem& it) {
    if (!it.file) NoteUsage(it.launch);
    std::wstring target = it.file ? NativePath(it.launch) : it.launch;
    ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void Power(int which) {
    switch (which) {
        case 4:
            OpenSettings();
            break;
        case 5:
            ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case 0:
            LockWorkStation();
            break;
        case 1: {
            using Fn = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
            if (HMODULE m = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32))
                if (auto fn = (Fn)GetProcAddress(m, "SetSuspendState")) fn(FALSE, FALSE, FALSE);
            break;
        }
        case 2:
            ShellExecuteW(nullptr, L"open", L"shutdown.exe", L"/r /t 0", nullptr, SW_HIDE);
            break;
        case 3:
            ShellExecuteW(nullptr, L"open", L"shutdown.exe", L"/s /hybrid /t 0", nullptr, SW_HIDE);
            break;
        case 6:
            if (!ExitWindowsEx(EWX_LOGOFF, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED)) Wh_Log(L"sign out failed: %u", GetLastError());
            break;
        case 7: {
            using Fn = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
            if (HMODULE m = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32))
                if (auto fn = (Fn)GetProcAddress(m, "SetSuspendState")) fn(TRUE, FALSE, FALSE);
            break;
        }
        case 8:
            ShellExecuteW(nullptr, L"open", L"shutdown.exe", L"/r /o /t 0", nullptr, SW_HIDE);
            break;
    }
}

static bool CanHibernate() {
    using Fn = BOOLEAN(WINAPI*)(PSYSTEM_POWER_CAPABILITIES);
    SYSTEM_POWER_CAPABILITIES c = {};
    if (HMODULE m = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32))
        if (auto fn = (Fn)GetProcAddress(m, "GetPwrCapabilities"))
            if (fn(&c)) return c.SystemS4 && c.HiberFilePresent;
    return false;
}

// Right-click on a power button: everything, each one at once (the menu is the confirmation).
static void PowerMenu(POINT pt) {
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1 + 0, Tr(S_Lock));
    AppendMenuW(m, MF_STRING, 1 + 6, L2(L"Sair da conta", L"Sign out"));
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 1 + 1, Tr(S_Sleep));
    if (CanHibernate()) AppendMenuW(m, MF_STRING, 1 + 7, L2(L"Hibernar", L"Hibernate"));
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 1 + 2, Tr(S_Restart));
    AppendMenuW(m, MF_STRING, 1 + 8, L2(L"Reiniciar nas opções avançadas", L"Restart to advanced options"));
    AppendMenuW(m, MF_STRING, 1 + 3, Tr(S_ShutDown));
    g_start.menuOpen = true;
    int cmd = TrackPopupMenuEx(m, TPM_RETURNCMD | TPM_NONOTIFY | TPM_BOTTOMALIGN, pt.x, pt.y, g_start.hwnd, nullptr);
    g_start.menuOpen = false;
    DestroyMenu(m);
    if (cmd > 0) {
        CloseStart();
        Power(cmd - 1);
    }
}

// Icons come from the shell, and some take a while (Store apps): each frame loads only what fits in
// a few milliseconds, and the rest comes in the next frames (TIMER_START_ICONS), so scrolling
// through all apps never stalls.
constexpr UINT_PTR TIMER_START_ICONS = 25;
static std::chrono::steady_clock::time_point g_iconBudgetEnd;
static bool g_iconsPending = false;
static bool g_iconLoadedThisFrame = false;

// Start icons by what they launch, as they arrive from the icon thread (newest asks first: what is on
// screen now). Until then the row shows no icon.
static std::unordered_map<std::wstring, ComPtr<IWICBitmapSource>> g_startIcons;
static std::unordered_set<std::wstring> g_startIconAsked;

static void RequestStartIcon(const std::wstring& launch) {
    IconJob job;
    job.key = L"#start:" + launch;
    job.lnk = launch;
    job.size = 64;
    AcquireSRWLockExclusive(&g_iconLock);
    if (!g_iconThread && !g_iconStop) g_iconThread = CreateThread(nullptr, 0, IconThread, nullptr, 0, nullptr);
    g_iconJobs.push_front(std::move(job));
    ReleaseSRWLockExclusive(&g_iconLock);
    WakeConditionVariable(&g_iconWake);
}

static ID2D1Bitmap* StartIcon(StartItem& it) {
    const std::wstring& key = it.launch;
    auto f = g_start.bmps.find(key);
    if (f != g_start.bmps.end()) return f->second.Get();
    auto c = g_startIcons.find(key);
    if (c == g_startIcons.end()) {
        if (g_startIconAsked.insert(key).second) RequestStartIcon(key);
        return nullptr;
    }
    ComPtr<ID2D1Bitmap> bmp;
    if (c->second) g_start.rt->CreateBitmapFromWicBitmap(c->second.Get(), nullptr, &bmp);
    g_start.bmps[key] = bmp;
    return bmp.Get();
}

static float SS(float v) { return v * g_start.scale; }

// The search text on one line (no wrapping), for drawing and for mouse hit tests.
static ComPtr<IDWriteTextLayout> QueryLayout() {
    auto& s = g_start;
    ComPtr<IDWriteTextLayout> lay;
    if (!g_dw || !s.font) return lay;
    g_dw->CreateTextLayout(s.query.c_str(), (UINT32)s.query.size(), s.font.Get(), 100000.0f,
                           s.textRc.bottom - s.textRc.top, &lay);
    if (lay) lay->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    return lay;
}

static void RenderStartFrame();

static void RenderStart() {
    g_iconBudgetEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(8);
    g_iconsPending = false;
    g_iconLoadedThisFrame = false;
    RenderStartFrame();
    if (g_iconsPending && g_start.open) SetTimer(g_start.hwnd, TIMER_START_ICONS, 16, nullptr);
}

struct StartPaint {
    ID2D1RenderTarget* rt;
    ID2D1Brush *text, *sub, *card, *cardHi, *hi, *line;
    D2D1_COLOR_F tintSolid;
    float W, pad, top, bottom;
};
static void StartStatusTab(const StartPaint& p);
static void StartOrgButton(const StartPaint& p, float listTop);
static void SaveStartSecOrder();
static void StartSecMoveTo(int sec, int target);
static void StartSecStep(int sec, int dir);
static void StartStatusRefresh();

static void RenderStartFrame() {
    if (!g_start.open && !g_start.closing) return;
    auto& s = g_start;
    if (!s.rt) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
        g_d2d->CreateDCRenderTarget(&props, &s.rt);
        if (!s.rt) return;
        s.rt.As(&s.dc);
        s.bmps.clear();
    }
    RECT rc = {0, 0, s.w, s.h};
    if (FAILED(s.rt->BindDC(s.memDC, &rc))) return;
    auto* rt = s.rt.Get();
    rt->BeginDraw();
    rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    s.rows.clear();

    const Theme& T = g_theme;
    float wideE = 1 - (1 - s.wide) * (1 - s.wide) * (1 - s.wide);
    float W = std::min((float)s.w, SS(580) + (g_set.wideSearch ? SS(200) * wideE : 0.0f));
    float H = (float)s.h, R = SS(28), pad = SS(20);
    s.offX = ((float)s.w - W) / 2;
    bool searching = !s.query.empty();

    float p = std::clamp(s.anim, 0.0f, 1.0f);
    float eased = 1 - (1 - p) * (1 - p) * (1 - p);
    float grow = eased;
    if (!s.closing) {
        const float c1 = 1.5f;  // a soft overshoot while opening
        grow = 1 + (c1 + 1) * std::pow(p - 1, 3.0f) + c1 * std::pow(p - 1, 2.0f);
    }
    float sc = 0.94f + 0.06f * grow;
    rt->SetTransform(D2D1::Matrix3x2F::Translation(s.offX, 0) *
                     D2D1::Matrix3x2F::Scale(sc, sc, D2D1::Point2F(s.w / 2.0f, H)));
    ComPtr<ID2D1SolidColorBrush> tint, text, sub, card, cardHi, line, hi;
    rt->CreateSolidColorBrush(T.tint, &tint);
    rt->CreateSolidColorBrush(T.text, &text);
    D2D1_COLOR_F subC = T.text;
    subC.a = 0.62f;
    rt->CreateSolidColorBrush(subC, &sub);
    D2D1_COLOR_F cardC = T.hover;
    cardC.a *= 0.55f;
    rt->CreateSolidColorBrush(cardC, &card);
    rt->CreateSolidColorBrush(T.hover, &cardHi);
    rt->CreateSolidColorBrush(T.sep, &line);
    D2D1_COLOR_F hiC = T.hover;
    hiC.a = std::min(1.0f, hiC.a * 1.8f);
    rt->CreateSolidColorBrush(hiC, &hi);

    D2D1_ROUNDED_RECT panel = D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), R, R);
    rt->FillRoundedRectangle(panel, tint.Get());
    D2D1_GRADIENT_STOP stops[] = {{0.0f, T.edge[0]}, {0.55f, T.edge[1]}, {1.0f, T.edge[2]}};
    ComPtr<ID2D1GradientStopCollection> coll;
    rt->CreateGradientStopCollection(stops, 3, &coll);
    ComPtr<ID2D1LinearGradientBrush> edge;
    rt->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), T.edgeHorizontal ? D2D1::Point2F(W, 0) : D2D1::Point2F(0, H)),
        coll.Get(), &edge);
    float sw = std::max(1.0f, s.scale);
    rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(sw / 2, sw / 2, W - sw / 2, H - sw / 2), R - sw / 2, R - sw / 2),
                             edge.Get(), sw);

    // Search pill.
    D2D1_RECT_F sr = D2D1::RectF(pad, pad, W - pad, pad + SS(44));
    rt->FillRoundedRectangle(D2D1::RoundedRect(sr, SS(22), SS(22)), card.Get());
    rt->DrawRoundedRectangle(D2D1::RoundedRect(sr, SS(22), SS(22)), line.Get(), sw);
    rt->DrawTextW(L"", 1, s.glyphs.Get(), D2D1::RectF(sr.left + SS(16), sr.top, sr.left + SS(40), sr.bottom), sub.Get());
    D2D1_RECT_F tr = D2D1::RectF(sr.left + SS(44), sr.top, sr.right - SS(16), sr.bottom);
    s.textRc = tr;
    s.caret = std::min(s.caret, s.query.size());
    s.anchor = std::min(s.anchor, s.query.size());
    if (s.query.empty()) {
        s.textScroll = 0;
        rt->DrawTextW(Tr(S_SearchHint), (UINT32)wcslen(Tr(S_SearchHint)), s.font.Get(), tr, sub.Get());
        if (s.caretOn)
            rt->FillRectangle(D2D1::RectF(tr.left, sr.top + SS(13), tr.left + sw, sr.bottom - SS(13)), text.Get());
    } else if (ComPtr<IDWriteTextLayout> lay = QueryLayout()) {
        float boxW = tr.right - tr.left;
        DWRITE_HIT_TEST_METRICS hm;
        FLOAT cx = 0, cy = 0;
        lay->HitTestTextPosition((UINT32)s.caret, FALSE, &cx, &cy, &hm);
        DWRITE_TEXT_METRICS tm;
        lay->GetMetrics(&tm);
        // Keep the cursor in view when the text is wider than the box.
        if (cx - s.textScroll > boxW - SS(2)) s.textScroll = cx - boxW + SS(2);
        if (cx - s.textScroll < 0) s.textScroll = cx;
        s.textScroll = std::clamp(s.textScroll, 0.0f, std::max(0.0f, tm.widthIncludingTrailingWhitespace + SS(2) - boxW));
        float ox = tr.left - s.textScroll;
        rt->PushAxisAlignedClip(D2D1::RectF(tr.left - SS(2), sr.top, tr.right, sr.bottom), D2D1_ANTIALIAS_MODE_ALIASED);
        if (s.caret != s.anchor) {
            D2D1_COLOR_F selC = T.text;
            selC.a = 0.26f;
            ComPtr<ID2D1SolidColorBrush> selB;
            rt->CreateSolidColorBrush(selC, &selB);
            UINT32 a = (UINT32)std::min(s.caret, s.anchor), n = (UINT32)(std::max(s.caret, s.anchor) - a);
            DWRITE_HIT_TEST_METRICS rg[8];
            UINT32 count = 0;
            if (SUCCEEDED(lay->HitTestTextRange(a, n, 0, 0, rg, ARRAYSIZE(rg), &count)) && selB) {
                for (UINT32 i = 0; i < count; i++)
                    rt->FillRectangle(D2D1::RectF(ox + rg[i].left, sr.top + SS(11), ox + rg[i].left + rg[i].width,
                                                  sr.bottom - SS(11)),
                                      selB.Get());
            }
        }
        rt->DrawTextLayout(D2D1::Point2F(ox, tr.top), lay.Get(), text.Get());
        if (s.caretOn)
            rt->FillRectangle(D2D1::RectF(ox + cx, sr.top + SS(13), ox + cx + sw, sr.bottom - SS(13)), text.Get());
        rt->PopAxisAlignedClip();
    }

    // Footer: power buttons.
    float footTop = H - pad - SS(40);
    bool showPower = !(searching && wideE > 0.5f);
    rt->FillRectangle(D2D1::RectF(pad, footTop - SS(12), W - pad, footTop - SS(12) + sw), line.Get());
    static const wchar_t* pg[] = {L"", L"", L"", L"", L"", L""};
    // 0-3 power (right), 5 Windows Settings (a little apart, left of them), 4 Nimbus settings (far left,
    // with Nimbus' own icon; the name of the button under the mouse shows beside it).
    for (int i = 0; i < (showPower ? 6 : 0); i++) {
        float x1 = W - pad - (3 - i) * SS(46), x0 = x1 - SS(40);
        if (i == 5) {
            x1 = W - pad - 4 * SS(46) - SS(14);
            x0 = x1 - SS(40);
        } else if (i == 4) {
            x0 = pad;
            x1 = pad + SS(40);
        }
        D2D1_RECT_F b = D2D1::RectF(x0, footTop, x1, footTop + SS(40));
        int idx = (int)s.rows.size();
        s.rows.push_back({b, RowPower, nullptr, i});
        bool armed = i == s.powerArm && GetTickCount() - s.powerArmAt < 4000;
        if (armed) {
            ComPtr<ID2D1SolidColorBrush> warn;
            rt->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.25f, 0.25f, 0.85f), &warn);
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F((x0 + x1) / 2, footTop + SS(20)), SS(20), SS(20)), warn.Get());
        } else
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F((x0 + x1) / 2, footTop + SS(20)), SS(20), SS(20)),
                        s.hover == idx ? cardHi.Get() : card.Get());
        if (i == 4) {
            auto f = s.bmps.find(L"#nimbus");
            if (f == s.bmps.end()) {
                ComPtr<ID2D1Bitmap> nb;
                if (HBITMAP hb = DrawNimbusIcon(64)) {
                    if (ComPtr<IWICBitmapSource> w = WicFromHBitmap(hb)) rt->CreateBitmapFromWicBitmap(w.Get(), nullptr, &nb);
                    DeleteObject(hb);
                }
                f = s.bmps.emplace(L"#nimbus", nb).first;
            }
            float cx = (x0 + x1) / 2, cy = footTop + SS(20), h = SS(12);
            if (f->second) {
                if (s.dc) s.dc->DrawBitmap(f->second.Get(), D2D1::RectF(cx - h, cy - h, cx + h, cy + h), 1,
                                           D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
                else rt->DrawBitmap(f->second.Get(), D2D1::RectF(cx - h, cy - h, cx + h, cy + h));
            }
            continue;
        }
        s.glyphs->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        rt->DrawTextW(pg[i], 1, s.glyphs.Get(), b, text.Get());
        s.glyphs->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    const wchar_t* pn[] = {Tr(S_Lock), Tr(S_Sleep), Tr(S_Restart), Tr(S_ShutDown), Tr(S_BarSettings),
                           L2(L"Configurações do Windows", L"Windows Settings")};
    bool armedNow = s.powerArm >= 0 && GetTickCount() - s.powerArmAt < 4000;
    if (armedNow && showPower) {
        const wchar_t* t = s.powerArm == 2 ? L2(L"Clique de novo pra reiniciar", L"Click again to restart")
                                           : L2(L"Clique de novo pra desligar", L"Click again to shut down");
        rt->DrawTextW(t, (UINT32)wcslen(t), s.small.Get(), D2D1::RectF(pad + SS(52), footTop, W / 2 + SS(60), footTop + SS(40)), text.Get());
    } else
    for (size_t i = 0; i < s.rows.size(); i++) {
        if (s.rows[i].kind == RowPower && s.hover == (int)i) {
            std::wstring name = pn[s.rows[i].power];
            if (s.rows[i].power <= 3) name += L2(L" · botão direito: mais", L" · right-click: more");
            rt->DrawTextW(name.c_str(), (UINT32)name.size(), s.small.Get(),
                          D2D1::RectF(pad + SS(52), footTop, W / 2 + SS(80), footTop + SS(40)), sub.Get());
        }
    }

    if (!showPower) {
        const wchar_t* hints = Tr(S_KeysHint);
        rt->DrawTextW(hints, (UINT32)wcslen(hints), s.small.Get(), D2D1::RectF(pad, footTop, W - pad, footTop + SS(40)), sub.Get());
    }

    // An icon: the item's own, or a gear for Settings pages.
    auto drawIcon = [&](StartItem& it, D2D1_RECT_F ir) {
        if (ID2D1Bitmap* bmp = it.setting ? nullptr : StartIcon(it)) {
            if (s.dc) s.dc->DrawBitmap(bmp, ir, 1.0f, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
            else rt->DrawBitmap(bmp, ir);
        } else if (it.setting) {
            float rr = (ir.right - ir.left) * 0.27f;
            rt->FillRoundedRectangle(D2D1::RoundedRect(ir, rr, rr), cardHi.Get());
            ComPtr<IDWriteTextFormat> gf;
            g_dw->CreateTextFormat(L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                   DWRITE_FONT_STRETCH_NORMAL, (ir.bottom - ir.top) * 0.55f, g_locale, &gf);
            if (gf) {
                gf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                gf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                const wchar_t gear[2] = {0xE713, 0};
                rt->DrawTextW(gear, 1, gf.Get(), ir, text.Get());
            }
        }
    };

    auto drawRow = [&](StartItem& it, D2D1_RECT_F r, bool highlight, bool showPath, int resultIndex = -1) {
        int idx = (int)s.rows.size();
        s.rows.push_back({r, it.file ? RowFile : RowApp, &it, resultIndex});
        bool on = highlight || s.hover == idx;
        rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(14), SS(14)), on ? cardHi.Get() : card.Get());
        float isz = SS(32);
        D2D1_RECT_F ir = D2D1::RectF(r.left + SS(10), (r.top + r.bottom - isz) / 2, r.left + SS(10) + isz, (r.top + r.bottom + isz) / 2);
        drawIcon(it, ir);
        float tx = ir.right + SS(12);
        if (showPath) {
            rt->DrawTextW(it.name.c_str(), (UINT32)it.name.size(), s.font.Get(),
                          D2D1::RectF(tx, r.top + SS(6), r.right - SS(10), r.top + SS(28)), text.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            rt->DrawTextW(it.path.c_str(), (UINT32)it.path.size(), s.small.Get(),
                          D2D1::RectF(tx, r.top + SS(27), r.right - SS(10), r.bottom - SS(4)), sub.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        } else {
            rt->DrawTextW(it.name.c_str(), (UINT32)it.name.size(), s.font.Get(),
                          D2D1::RectF(tx, r.top, r.right - SS(10), r.bottom), text.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    };

    float listTop = sr.bottom + SS(16), listBottom = footTop - SS(24);
    if (s.query.empty()) {
        // Tabs.
        int nTabs = StartHasStatus() ? 4 : 3;
        if (s.tab >= nTabs) s.tab = 0;
        D2D1_RECT_F tabs = D2D1::RectF(pad, listTop, pad + SS(nTabs == 4 ? 432 : 342), listTop + SS(36));
        rt->FillRoundedRectangle(D2D1::RoundedRect(tabs, SS(10), SS(10)), card.Get());
        const wchar_t* names[] = {Tr(S_TabPinned), Tr(S_TabAll), Tr(S_TabRecent), L"Status"};
        const StartRowKind kinds[] = {RowTabPinned, RowTabAll, RowTabRecent, RowTabStatus};
        const float widths[] = {SS(92), SS(128), SS(110), SS(87)};
        float tx = tabs.left + SS(3);
        for (int i = 0; i < nTabs; i++) {
            float tw = widths[i];
            D2D1_RECT_F t = D2D1::RectF(tx, tabs.top + SS(3), tx + tw, tabs.bottom - SS(3));
            s.rows.push_back({t, kinds[i], nullptr, 0});
            if (s.tab == i) rt->FillRoundedRectangle(D2D1::RoundedRect(t, SS(8), SS(8)), hi.Get());
            s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            rt->DrawTextW(names[i], (UINT32)wcslen(names[i]), s.small.Get(), t, s.tab == i ? text.Get() : sub.Get());
            s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            tx += tw + SS(3);
        }
        static std::vector<StartItem> none;
        std::vector<StartItem>& list = s.tab == 0 ? g_startPins : s.tab == 1 ? g_allApps : s.tab == 2 ? g_recent : none;
        D2D1_COLOR_F tintSolid = T.tint;
        tintSolid.a = 1;
        StartPaint sp = {rt, text.Get(), sub.Get(), card.Get(), cardHi.Get(), hi.Get(), line.Get(), tintSolid, W, pad, 0, 0};
        if (s.tab == 3) {
            StartOrgButton(sp, listTop);
        } else {
            wchar_t count[64];
            swprintf_s(count, s.tab == 0 ? Tr(S_CountPinned) : s.tab == 1 ? Tr(S_CountApps) : Tr(S_CountRecent), (int)list.size());
            s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            rt->DrawTextW(count, (UINT32)wcslen(count), s.small.Get(), D2D1::RectF(W / 2, listTop, W - pad, listTop + SS(36)), sub.Get());
            s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        }

        float gridTop = listTop + SS(48);
        // Pinned and all apps: two columns. Downloads: one, with when and where under the name.
        int cols = s.tab == 2 ? 1 : 2;
        float colW = (W - pad * 2 - SS(6) * (cols - 1)) / cols, rowH = SS(s.tab == 2 ? 50 : 48), gap = SS(6);
        int rowsN = ((int)list.size() + cols - 1) / cols;
        if (s.tab != 3) {
            s.maxScroll = std::max(0.0f, rowsN * (rowH + gap) - gap - (listBottom - gridTop));
            s.scroll = std::clamp(s.scroll, 0.0f, s.maxScroll);
        }
        rt->PushAxisAlignedClip(D2D1::RectF(0, gridTop, W, listBottom), D2D1_ANTIALIAS_MODE_ALIASED);
        for (size_t i = 0; i < list.size(); i++) {
            float x = pad + (i % cols) * (colW + gap);
            float y = gridTop + (i / cols) * (rowH + gap) - s.scroll;
            if (y + rowH < gridTop || y > listBottom) continue;
            drawRow(list[i], D2D1::RectF(x, y, x + colW, y + rowH), s.tab == 0 && s.pinDrag.active && (int)i == s.pinDrag.cur,
                    s.tab == 2);
        }
        rt->PopAxisAlignedClip();
        if (list.empty() && s.tab == 2 && g_recentLoaded) {
            const wchar_t* hint = Tr(S_NoRecent);
            rt->DrawTextW(hint, (UINT32)wcslen(hint), s.small.Get(), D2D1::RectF(pad, gridTop, W - pad, gridTop + SS(40)), sub.Get());
        }
        if (s.tab == 3) {
            sp.top = gridTop;
            sp.bottom = listBottom;
            StartStatusTab(sp);
        }
        if (list.empty() && s.tab == 0) {
            const wchar_t* hint = Tr(S_PinHint);
            rt->DrawTextW(hint, (UINT32)wcslen(hint), s.small.Get(), D2D1::RectF(pad, gridTop, W - pad, gridTop + SS(40)), sub.Get());
        }
    } else {
        float y = listTop;
        int k = 0;
        // Wide search: the list leaves room for the item panel on the right.
        float listRight = W - pad - wideE * SS(256);
        bool wideNow = g_set.wideSearch && wideE > 0.5f;
        auto sectionTitle = [&](const wchar_t* t) {
            rt->DrawTextW(t, (UINT32)wcslen(t), s.small.Get(), D2D1::RectF(pad + SS(4), y, listRight, y + SS(22)), sub.Get());
            y += SS(24);
        };
        if (s.appHits.empty()) {
            sectionTitle(Tr(S_Apps));
            rt->DrawTextW(Tr(S_NoApps), (UINT32)wcslen(Tr(S_NoApps)), s.small.Get(), D2D1::RectF(pad + SS(4), y, listRight, y + SS(24)), sub.Get());
            y += SS(30);
        }
        for (size_t i = 0; i < s.appHits.size(); i++) {
            if (i == 0) sectionTitle(wideNow ? Tr(S_BestMatch) : Tr(S_Apps));
            if (i == 1 && wideNow) {
                y += SS(4);
                sectionTitle(Tr(S_Apps));
            }
            if (y + SS(48) > listBottom) break;
            drawRow(*s.appHits[i], D2D1::RectF(pad, y, listRight, y + SS(46)), k == s.sel, false, k);
            y += SS(50);
            k++;
        }
        y += SS(6);
        if (y + SS(30) < listBottom && g_set.fileSearch != L"off") {
            sectionTitle(Tr(S_FilesFolders));
            if (g_set.fileSearch == L"windows") {
                // Windows' own search only opens in Explorer: one row that does it.
                D2D1_RECT_F r = D2D1::RectF(pad, y, listRight, y + SS(42));
                int idx = (int)s.rows.size();
                s.rows.push_back({r, RowAction, nullptr, ActExplorerSearch});
                rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(14), SS(14)), (s.hover == idx || k == s.sel) ? cardHi.Get() : card.Get());
                rt->DrawTextW(L"", 1, s.glyphs.Get(), D2D1::RectF(r.left + SS(16), r.top, r.left + SS(40), r.bottom), sub.Get());
                std::wstring label = Tr(S_SearchFilesPre) + s.query + Tr(S_SearchFilesPost);
                rt->DrawTextW(label.c_str(), (UINT32)label.size(), s.font.Get(),
                              D2D1::RectF(r.left + SS(48), r.top, r.right - SS(12), r.bottom), text.Get(),
                              D2D1_DRAW_TEXT_OPTIONS_CLIP);
            } else if (s.ev != EvOk) {
                auto actionRow = [&](const wchar_t* label, const wchar_t* glyph, int act) {
                    if (y + SS(44) > listBottom) return;
                    D2D1_RECT_F r = D2D1::RectF(pad, y, listRight, y + SS(42));
                    int idx = (int)s.rows.size();
                    s.rows.push_back({r, RowAction, nullptr, act});
                    rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(14), SS(14)), s.hover == idx ? cardHi.Get() : card.Get());
                    rt->DrawTextW(glyph, 1, s.glyphs.Get(), D2D1::RectF(r.left + SS(16), r.top, r.left + SS(40), r.bottom), sub.Get());
                    rt->DrawTextW(label, (UINT32)wcslen(label), s.font.Get(),
                                  D2D1::RectF(r.left + SS(48), r.top, r.right - SS(12), r.bottom), text.Get(),
                                  D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    y += SS(46);
                };
                if (s.ev == EvClosed) {
                    actionRow(Tr(S_OpenEverything), L"", ActOpenEverything);
                } else {
                    actionRow(Tr(S_ExplorerSearch), L"", ActExplorerSearch);
                    actionRow(Tr(S_GetEverything), L"", ActGetEverything);
                }
            } else if (s.files.empty()) {
                const wchar_t* m = s.query.size() < 2 ? Tr(S_TypeMore) : Tr(S_NoFiles);
                rt->DrawTextW(m, (UINT32)wcslen(m), s.small.Get(), D2D1::RectF(pad + SS(4), y, W - pad, y + SS(24)), sub.Get());
            }
            for (auto& f : s.files) {
                if (y + SS(50) > listBottom) break;
                drawRow(f, D2D1::RectF(pad, y, listRight, y + SS(48)), k == s.sel, true, k);
                y += SS(52);
                k++;
            }
        }
        // Wide search: Windows Settings pages.
        if (wideNow && !s.setHits.empty() && y + SS(80) < listBottom) {
            y += SS(6);
            sectionTitle(Tr(S_Settings));
            for (auto* st : s.setHits) {
                if (y + SS(48) > listBottom) break;
                drawRow(*st, D2D1::RectF(pad, y, listRight, y + SS(46)), k == s.sel, false, k);
                y += SS(50);
                k++;
            }
        }
        // Wide search: the chosen item, big, with what can be done with it.
        if (wideNow) {
            D2D1_RECT_F pr = D2D1::RectF(listRight + SS(16), listTop, W - pad, listBottom);
            rt->FillRoundedRectangle(D2D1::RoundedRect(pr, SS(20), SS(20)), card.Get());
            if (StartItem* it = SelectedResult()) {
                float cx = (pr.left + pr.right) / 2, isz = SS(72);
                drawIcon(*it, D2D1::RectF(cx - isz / 2, pr.top + SS(26), cx + isz / 2, pr.top + SS(26) + isz));
                float ty = pr.top + SS(26) + isz + SS(10);
                s.font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                rt->DrawTextW(it->name.c_str(), (UINT32)it->name.size(), s.font.Get(),
                              D2D1::RectF(pr.left + SS(12), ty, pr.right - SS(12), ty + SS(26)), text.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                const wchar_t* kind = it->setting ? Tr(S_KindSetting) : it->folder ? Tr(S_KindFolder) : it->file ? Tr(S_KindFile) : Tr(S_KindApp);
                rt->DrawTextW(kind, (UINT32)wcslen(kind), s.small.Get(),
                              D2D1::RectF(pr.left + SS(12), ty + SS(24), pr.right - SS(12), ty + SS(44)), sub.Get());
                s.font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                float by = ty + SS(60);
                auto button = [&](const wchar_t* label, int act, bool primary) {
                    D2D1_RECT_F b = D2D1::RectF(pr.left + SS(14), by, pr.right - SS(14), by + SS(38));
                    int idx = (int)s.rows.size();
                    s.rows.push_back({b, RowAction, nullptr, act});
                    bool on = s.hover == idx;
                    if (primary) {
                        ComPtr<ID2D1SolidColorBrush> pb;
                        D2D1_COLOR_F c = T.text;
                        c.a = on ? 1.0f : 0.92f;
                        rt->CreateSolidColorBrush(c, &pb);
                        rt->FillRoundedRectangle(D2D1::RoundedRect(b, SS(12), SS(12)), pb.Get());
                        ComPtr<ID2D1SolidColorBrush> pt;
                        D2D1_COLOR_F tc = T.tint;
                        tc.a = 1;
                        rt->CreateSolidColorBrush(tc, &pt);
                        rt->DrawTextW(label, (UINT32)wcslen(label), s.small.Get(), b, pt.Get());
                    } else {
                        rt->FillRoundedRectangle(D2D1::RoundedRect(b, SS(12), SS(12)), on ? cardHi.Get() : card.Get());
                        rt->DrawRoundedRectangle(D2D1::RoundedRect(b, SS(12), SS(12)), line.Get(), sw);
                        rt->DrawTextW(label, (UINT32)wcslen(label), s.small.Get(), b, text.Get());
                    }
                    by += SS(44);
                };
                button(Tr(S_Open), ActOpenSel, true);
                std::wstring id = it->launch.rfind(L"shell:AppsFolder\\", 0) == 0 ? it->launch.substr(17) : L"";
                bool pinnable = (!id.empty() && id.find(L'\\') == std::wstring::npos) ||
                                (it->file && !it->folder &&
                                 (EqI(PathFindExtensionW(it->launch.c_str()), L".exe") ||
                                  EqI(PathFindExtensionW(it->launch.c_str()), L".lnk")));
                if (pinnable) button(Tr(S_PinDock), ActPinSel, false);
                if (it->file) button(Tr(S_OpenLocation), ActLocationSel, false);
                s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            }
        }
    }

    // The keyboard's choice: a ring around it.
    if (s.kbFocus >= 0 && s.kbFocus < (int)s.rows.size() && s.query.empty()) {
        D2D1_RECT_F fr = s.rows[s.kbFocus].rc;
        fr = D2D1::RectF(fr.left - SS(2), fr.top - SS(2), fr.right + SS(2), fr.bottom + SS(2));
        float rad = s.rows[s.kbFocus].kind == RowPower ? (fr.bottom - fr.top) / 2 : SS(12);
        ComPtr<ID2D1SolidColorBrush> ring;
        rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.85f), &ring);
        rt->DrawRoundedRectangle(D2D1::RoundedRect(fr, rad, rad), ring.Get(), SS(2));
    }
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    if (rt->EndDraw() == D2DERR_RECREATE_TARGET) {
        s.rt.Reset();
        s.dc.Reset();
        s.bmps.clear();
        return;
    }
    POINT dst = {s.rc.left, s.rc.top + (LONG)std::lround((1 - eased) * SS(26))};
    SIZE size = {s.w, s.h};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)std::lround(255 * eased), AC_SRC_ALPHA};
    UpdateLayeredWindow(s.hwnd, nullptr, &dst, &size, s.memDC, &src, 0, &bf, ULW_ALPHA);
}

// Bring the Start to the front. Every way of opening it already has the foreground right (a click on
// the dock, the hotkey, or Explorer's AllowSetForegroundWindow for the Windows key); an empty
// SendInput covers the rest.
static void ForceForeground(HWND hwnd) {
    if (!SetForegroundWindow(hwnd)) {
        INPUT input{};
        SendInput(1, &input, sizeof(INPUT));
        SetForegroundWindow(hwnd);
    }
    SetFocus(hwnd);
}

// While the Iniciar is open: a click anywhere else closes it (also when it never got the foreground).
constexpr UINT WM_APP_START_OUTSIDE = WM_APP + 7;
bool CursorInFlyout(POINT pt);

// A click outside the Start closes it, also on windows run as administrator (which never take the
// activation from it in a way it sees). No low-level mouse hook (it would make every mouse move in the
// system wait for this thread): the buttons are checked every 40 ms while the Start is open.
constexpr UINT_PTR TIMER_START_WATCH = 28;
static bool g_startWasDown = false;
static HWND g_startFg = nullptr;  // the window in front when the Start opened (see StartWatchStep)
static HWINEVENTHOOK g_startFgHook = nullptr;
// Another window (not Nimbus', not a menu, not Windows' search box) came to the front while the Start
// is open. (Right after the Windows key, Windows moves the focus around by itself: not counted.)
static bool StartLostFront(HWND fg, bool changed) {
    extern DWORD g_lastWinAlone;
    if (!g_start.open || g_start.menuOpen || GetTickCount() - g_start.openedAt < 200 || GetTickCount() - g_lastWinAlone < 400) return false;
    // (polling: the window that was already in front isn't a change; told by Windows: it is)
    if (!fg || fg == g_start.hwnd || (!changed && fg == g_startFg)) return false;
    wchar_t fcls[64] = {};
    GetClassNameW(GetAncestor(fg, GA_ROOT), fcls, ARRAYSIZE(fcls));
    return wcsncmp(fcls, L"NimbusMenu_", 11) != 0 && wcscmp(fcls, L"#32768") != 0 && wcscmp(fcls, L"Windows.UI.Core.CoreWindow") != 0;
}
// Told by Windows the moment the window in front changes (a window run as administrator hides its
// clicks, but not that it came to the front).
static void CALLBACK StartFgEvent(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (StartLostFront(hwnd, true)) PostMessageW(g_start.hwnd, WM_APP_START_OUTSIDE, 0, 0);
}
static void StartWatchStep() {
    // A window run as administrator hides its clicks from this process (the buttons read as up). It
    // still comes to the front when clicked: a new window in front that isn't ours closes the Start.
    if (StartLostFront(GetForegroundWindow(), false)) {
        PostMessageW(g_start.hwnd, WM_APP_START_OUTSIDE, 0, 0);
        return;
    }
    SHORT l = GetAsyncKeyState(VK_LBUTTON), r = GetAsyncKeyState(VK_RBUTTON), m = GetAsyncKeyState(VK_MBUTTON);
    bool down = ((l | r | m) & 0x8000) != 0, tapped = ((l | r | m) & 1) != 0;  // (bit 0: pressed since the last check)
    if ((down || tapped) && !g_startWasDown && g_start.open && !g_start.menuOpen) {
        POINT pt;
        GetCursorPos(&pt);
        HWND under = WindowFromPoint(pt);
        wchar_t cls[64] = {};
        if (under) GetClassNameW(GetAncestor(under, GA_ROOT), cls, ARRAYSIZE(cls));
        // (The dock handles its own clicks: its Start button toggles, its apps close the Iniciar.)
        // (the panel itself, not the whole window: its clear sides, room for the wide search, let clicks
        // through to the windows behind)
        float W = std::min((float)g_start.w, SS(580) + SS(200) * g_start.wide);
        float L = g_start.rc.left + ((float)g_start.w - W) / 2;
        RECT panel = {(LONG)L, g_start.rc.top, (LONG)(L + W), g_start.rc.bottom};
        if (!PtInRect(&panel, pt) && wcscmp(cls, L"NimbusMenu_Content") != 0 && wcscmp(cls, L"#32768") != 0 && !CursorInFlyout(pt))
            PostMessageW(g_start.hwnd, WM_APP_START_OUTSIDE, 0, 0);
    }
    g_startWasDown = down;
}
static void StartMouseHook(bool on) {
    if (!g_start.hwnd) return;
    if (on) {
        g_startWasDown = true;  // (the click that opened it may still be down)
        g_startFg = GetForegroundWindow();
        if (!g_startFgHook)
            g_startFgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, StartFgEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
        SetTimer(g_start.hwnd, TIMER_START_WATCH, 40, nullptr);
    } else {
        KillTimer(g_start.hwnd, TIMER_START_WATCH);
        if (g_startFgHook) {
            UnhookWinEvent(g_startFgHook);
            g_startFgHook = nullptr;
        }
    }
}

void StartMouseHookOff() { StartMouseHook(false); }

void ToggleStart(Dock& d) {
    Wh_Log(L"start: toggle (open=%d closing=%d same dock=%d)", g_start.open, g_start.closing, g_start.dock == &d);
    CloseFlyout();
    if (g_start.open) {
        if (g_start.dock == &d) {
            CloseStart(true);
            return;
        }
        CloseStart(false);
    }
    if (g_start.closing) FinishCloseStart();
    auto& s = g_start;
    HidePreview();
    s.dock = &d;
    s.scale = d.scale;
    s.w = (int)std::lround(SS(g_set.wideSearch ? 780 : 580));  // room to widen into
    s.h = (int)std::lround(SS(560));
    s.wide = 0;
    s.wideTarget = false;
    KillTimer(s.hwnd, TIMER_START_WIDE);
    if (s.w != s.dibW || s.h != s.dibH || !s.memDC) {
        if (s.memDC) {
            SelectObject(s.memDC, s.oldBmp);
            DeleteDC(s.memDC);
            DeleteObject(s.dib);
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = s.w;
        bi.bmiHeader.biHeight = -s.h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        s.memDC = CreateCompatibleDC(nullptr);
        s.dib = CreateDIBSection(s.memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        s.oldBmp = SelectObject(s.memDC, s.dib);
        s.dibW = s.w;
        s.dibH = s.h;
    }
    s.font.Reset();
    s.small.Reset();
    s.glyphs.Reset();
    s.big.Reset();
    s.head.Reset();
    g_dw->CreateTextFormat(L"Segoe UI Variable Display", nullptr, DWRITE_FONT_WEIGHT_SEMI_LIGHT, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(34), g_locale, &s.big);
    g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(13.5f), g_locale, &s.head);
    g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(14.5f), g_locale, &s.font);
    g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(12.5f), g_locale, &s.small);
    g_dw->CreateTextFormat(L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(16), g_locale, &s.glyphs);
    for (auto* f : {s.font.Get(), s.small.Get(), s.glyphs.Get(), s.big.Get(), s.head.Get()}) {
        if (!f) continue;
        f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> dots;
        g_dw->CreateEllipsisTrimmingSign(f, &dots);
        f->SetTrimming(&trim, dots.Get());
    }
    if (!g_allAppsLoaded) LoadAllApps();

    int dockTop = d.rcMon.bottom - d.ch + (int)d.pill.top;
    int x = d.rcMon.left + (d.cw - s.w) / 2;
    int y = dockTop - s.h - (int)SS(12);
    y = std::max(y, (int)d.rcMon.top + 8);
    s.rc = {x, y, x + s.w, y + s.h};
    s.query.clear();
    s.tab = 0;
    s.org = false;
    s.scroll = 0;
    s.hover = -1;
    s.kbFocus = -1;
    s.caretOn = true;
    s.open = true;
    s.openedAt = GetTickCount();
    s.anim = 0;
    s.closing = false;
    s.animStart = GetTickCount();
    RenderStart();  // fully transparent, below its place
    SetWindowPos(s.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    ForceForeground(s.hwnd);
    StartMouseHook(true);
    {
        wchar_t cls[64] = {};
        GetClassNameW(GetForegroundWindow(), cls, ARRAYSIZE(cls));
        Wh_Log(L"start opened: foreground=%s ours=%d", cls, GetForegroundWindow() == s.hwnd);
    }
    SetTimer(s.hwnd, TIMER_CARET, 530, nullptr);
    SetTimer(s.hwnd, TIMER_START_ANIM, 15, nullptr);
}

// The blur goes in once the panel has landed (it can't fade or move with it).
static void StartAnimStep() {
    auto& s = g_start;
    DWORD t = ReduceMotion() ? 100000 : GetTickCount() - s.animStart;
    if (s.closing) {
        s.anim = s.animFrom * (1 - std::min(1.0f, t / (float)kStartCloseMs));
        if (s.anim <= 0) {
            FinishCloseStart();
            return;
        }
    } else {
        s.anim = std::min(1.0f, t / (float)kStartOpenMs);
        if (s.anim >= 1) {
            KillTimer(s.hwnd, TIMER_START_ANIM);
            PlaceStartBands();
        }
    }
    RenderStart();
}

// The blur under the panel at its current width (only once nothing is moving).
static void PlaceStartBands() {
    auto& s = g_start;
    if (!s.open || s.anim < 1 || (s.wide > 0 && s.wide < 1)) return;
    float W = SS(580) + (g_set.wideSearch && s.wideTarget ? SS(200) : 0.0f);
    W = std::min(W, (float)s.w);
    float L = s.rc.left + ((float)s.w - W) / 2;
    PlaceBands(s.bands, s.bandRc, g_theme.blur, L, (float)s.rc.top, L + W, (float)s.rc.bottom, SS(28));
    SetWindowPos(s.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void StartWideStep() {
    auto& s = g_start;
    float t = std::min(1.0f, (GetTickCount() - s.wideStart) / (float)kStartWideMs);
    float goal = s.wideTarget ? 1.0f : 0.0f;
    s.wide = s.wideFrom + (goal - s.wideFrom) * t;
    if (t >= 1) {
        s.wide = goal;
        KillTimer(s.hwnd, TIMER_START_WIDE);
        PlaceStartBands();
    }
    RenderStart();
}

bool StartOpenOn(const Dock& d) { return g_start.open && g_start.dock == &d; }

// ---- Keyboard in the Start: arrows go to the nearest item that way, Tab to the next, Enter clicks ----
static bool StartIsTabRow(StartRowKind k) { return k == RowTabPinned || k == RowTabAll || k == RowTabRecent || k == RowTabStatus; }
static void StartKeepVisible() {
    auto& s = g_start;
    if (s.kbFocus < 0 || s.kbFocus >= (int)s.rows.size()) return;
    const StartRow& row = s.rows[s.kbFocus];
    if (StartIsTabRow(row.kind) || row.kind == RowPower || row.kind == RowOrg) return;
    float top = 0, bottom = 1e9f;
    for (auto& r : s.rows) {
        if (StartIsTabRow(r.kind)) top = std::max(top, r.rc.bottom);
        if (r.kind == RowPower) bottom = std::min(bottom, r.rc.top);
    }
    float d = 0;
    if (row.rc.top < top) d = row.rc.top - top - 8;
    else if (row.rc.bottom > bottom - 16) d = row.rc.bottom - bottom + 24;
    if (d != 0) s.scroll = std::clamp(s.scroll + d, 0.0f, s.maxScroll);
}
static void StartNav(int dir) {  // 0 left, 1 right, 2 up, 3 down, 4 next, 5 previous
    auto& s = g_start;
    int n = (int)s.rows.size();
    if (!n) return;
    int cur = s.kbFocus >= 0 && s.kbFocus < n ? s.kbFocus : -1;
    if (cur < 0) {
        // the first item of the tab's content (not a tab, not the power buttons)
        for (int i = 0; i < n; i++)
            if (!StartIsTabRow(s.rows[i].kind) && s.rows[i].kind != RowPower && s.rows[i].kind != RowOrg) {
                cur = i;
                break;
            }
        s.kbFocus = cur >= 0 ? cur : 0;
        return;
    }
    if (dir == 4 || dir == 5) {
        s.kbFocus = (cur + (dir == 4 ? 1 : n - 1)) % n;
        return;
    }
    const D2D1_RECT_F& a = s.rows[cur].rc;
    float ax = (a.left + a.right) / 2, ay = (a.top + a.bottom) / 2;
    int best = -1;
    float bestScore = 1e18f;
    for (int i = 0; i < n; i++) {
        if (i == cur) continue;
        const D2D1_RECT_F& b = s.rows[i].rc;
        float bx = (b.left + b.right) / 2, by = (b.top + b.bottom) / 2;
        float dx = bx - ax, dy = by - ay, main, side;
        if (dir == 0) main = -dx, side = std::fabs(dy);
        else if (dir == 1) main = dx, side = std::fabs(dy);
        else if (dir == 2) main = -dy, side = std::fabs(dx);
        else main = dy, side = std::fabs(dx);
        if (main <= 2) continue;
        // (rows that overlap on the other axis first: a grid moves by its rows and columns)
        bool overlap = dir <= 1 ? (b.top < a.bottom && b.bottom > a.top) : (b.left < a.right && b.right > a.left);
        float score = main + side * 2 + (overlap ? 0 : 4000);
        if (score < bestScore) {
            bestScore = score;
            best = i;
        }
    }
    if (best >= 0) s.kbFocus = best;
}
static void StartActivateFocus(HWND hwnd) {
    auto& s = g_start;
    if (s.kbFocus < 0 || s.kbFocus >= (int)s.rows.size()) return;
    const D2D1_RECT_F& r = s.rows[s.kbFocus].rc;
    int x = (int)std::lround((r.left + r.right) / 2 + s.offX), y = (int)std::lround((r.top + r.bottom) / 2);
    bool tab = StartIsTabRow(s.rows[s.kbFocus].kind);
    SendMessageW(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));  // (as a click there)
    if (tab && g_start.open) {
        g_start.kbFocus = -1;
        StartNav(3);  // into the new tab's content
        RenderStart();
    }
}

static int StartRowAt(int x, int y) {
    x -= (int)std::lround(g_start.offX);
    for (int i = 0; i < (int)g_start.rows.size(); i++) {
        const auto& r = g_start.rows[i].rc;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}

// ---- Search text editing: cursor, selection (Shift + arrows, mouse), clipboard ----

static bool InSearchText(int x, int y) {
    auto& s = g_start;
    float lx = x - s.offX;
    return lx >= s.textRc.left - SS(44) && lx < s.textRc.right + SS(16) && y >= s.textRc.top && y < s.textRc.bottom;
}

static size_t QueryPosAt(int x) {
    auto& s = g_start;
    ComPtr<IDWriteTextLayout> lay = QueryLayout();
    if (!lay) return s.query.size();
    float lx = x - s.offX - s.textRc.left + s.textScroll;
    BOOL trailing = FALSE, inside = FALSE;
    DWRITE_HIT_TEST_METRICS hm{};
    lay->HitTestPoint(lx, (s.textRc.bottom - s.textRc.top) / 2, &trailing, &inside, &hm);
    return std::min(s.query.size(), (size_t)hm.textPosition + (trailing ? hm.length : 0));
}

static bool HasTextSel() { return g_start.caret != g_start.anchor; }

static void DeleteTextSel() {
    auto& s = g_start;
    size_t a = std::min(s.caret, s.anchor), b = std::max(s.caret, s.anchor);
    s.query.erase(a, b - a);
    s.caret = s.anchor = a;
}

static void InsertText(std::wstring t) {
    auto& s = g_start;
    t.erase(std::remove_if(t.begin(), t.end(), [](wchar_t c) { return c < 32; }), t.end());
    DeleteTextSel();
    t = t.substr(0, 200 - std::min<size_t>(200, s.query.size()));
    s.query.insert(s.caret, t);
    s.caret = s.anchor = s.caret + t.size();
}

static size_t WordLeft(size_t i) {
    const auto& q = g_start.query;
    while (i > 0 && q[i - 1] == L' ') i--;
    while (i > 0 && q[i - 1] != L' ') i--;
    return i;
}

static size_t WordRight(size_t i) {
    const auto& q = g_start.query;
    while (i < q.size() && q[i] != L' ') i++;
    while (i < q.size() && q[i] == L' ') i++;
    return i;
}

static void MoveTextCaret(size_t to, bool extend) {
    auto& s = g_start;
    s.caret = std::min(to, s.query.size());
    if (!extend) s.anchor = s.caret;
    s.caretOn = true;
    RenderStart();
}

static void SetClipboardText(HWND hwnd, const wchar_t* text, size_t n) {
    if (!OpenClipboard(hwnd)) return;
    EmptyClipboard();
    if (HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, (n + 1) * sizeof(wchar_t))) {
        if (auto* p = (wchar_t*)GlobalLock(h)) {
            memcpy(p, text, n * sizeof(wchar_t));
            p[n] = 0;
            GlobalUnlock(h);
            if (!SetClipboardData(CF_UNICODETEXT, h)) GlobalFree(h);
        } else {
            GlobalFree(h);
        }
    }
    CloseClipboard();
}

static void CopyTextSel(HWND hwnd) {
    auto& s = g_start;
    if (!HasTextSel()) return;
    size_t a = std::min(s.caret, s.anchor), n = std::max(s.caret, s.anchor) - a;
    SetClipboardText(hwnd, s.query.c_str() + a, n);
}

static StartItem* SelectedResult() {
    int k = 0;
    for (auto* a : g_start.appHits) if (k++ == g_start.sel) return a;
    for (auto& f : g_start.files) if (k++ == g_start.sel) return &f;
    for (auto* st : g_start.setHits) if (k++ == g_start.sel) return st;
    return nullptr;
}

// Where an app in Start lives on disk (for "Open file location"): its program, for desktop apps.
// Store apps have none.
static std::wstring AppFileLocation(const std::wstring& launch) {
    static const PROPERTYKEY kPkeyLinkTarget = {
        {0xB9B4B3FC, 0x2B51, 0x4A42, {0xB5, 0xD8, 0x32, 0x41, 0x46, 0xAF, 0xCF, 0x25}}, 2};
    std::wstring path;
    ComPtr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(launch.c_str(), nullptr, IID_PPV_ARGS(&item)))) {
        ComPtr<IShellItem2> item2;
        PWSTR s = nullptr;
        if (SUCCEEDED(item.As(&item2)) && SUCCEEDED(item2->GetString(kPkeyLinkTarget, &s)) && s) {
            path = s;
            CoTaskMemFree(s);
        }
    }
    // "shell:AppsFolder\{known folder}\sub\app.exe"
    std::wstring id = launch.rfind(L"shell:AppsFolder\\", 0) == 0 ? launch.substr(17) : L"";
    if (path.empty() && id.size() > 38 && id[0] == L'{' && id[37] == L'}' && id[38] == L'\\') {
        GUID g;
        PWSTR base = nullptr;
        if (SUCCEEDED(CLSIDFromString(id.substr(0, 38).c_str(), &g)) &&
            SUCCEEDED(SHGetKnownFolderPath(g, 0, nullptr, &base)) && base) {
            path = std::wstring(base) + id.substr(38);
            CoTaskMemFree(base);
        }
    }
    if (!path.empty() && GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) path.clear();
    return path;
}

// Right-click on a file (search results, Downloads tab): open, open its folder, copy its path.
static void FileItemMenu(StartItem& it, POINT pt) {
    std::wstring path = it.launch;
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, Tr(S_Open));
    AppendMenuW(m, MF_STRING, 2, Tr(S_OpenLocation));
    AppendMenuW(m, MF_STRING, 3, Tr(S_CopyPath));
    g_start.menuOpen = true;
    int cmd = TrackPopupMenuEx(m, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, g_start.hwnd, nullptr);
    g_start.menuOpen = false;
    DestroyMenu(m);
    if (cmd == 1 || cmd == 2) {
        CloseStart(true);
        if (cmd == 1) {
            ShellExecuteW(nullptr, L"open", NativePath(path).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        } else {
            std::wstring args = L"/select,\"" + path + L"\"";
            ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
        }
        return;
    }
    if (cmd == 3) SetClipboardText(g_start.hwnd, path.c_str(), path.size());
    SetForegroundWindow(g_start.hwnd);
    RenderStart();
}

static void StartItemMenu(StartItem& it, POINT pt) {
    if (it.file) return;
    bool pinned = std::any_of(g_startPins.begin(), g_startPins.end(), [&](const StartItem& p) { return EqI(p.launch, it.launch); });
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, pinned ? Tr(S_UnpinStart) : Tr(S_PinStart));
    std::wstring id = it.launch.rfind(L"shell:AppsFolder\\", 0) == 0 ? it.launch.substr(17) : L"";
    bool uwp = !id.empty() && id.find(L'\\') == std::wstring::npos;
    if (uwp) AppendMenuW(m, MF_STRING, 2, Tr(S_PinDock));
    std::wstring location = AppFileLocation(it.launch);
    if (!location.empty()) AppendMenuW(m, MF_STRING, 3, Tr(S_OpenLocation));
    g_start.menuOpen = true;
    int cmd = TrackPopupMenuEx(m, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, g_start.hwnd, nullptr);
    g_start.menuOpen = false;
    DestroyMenu(m);
    if (cmd == 1) {
        if (pinned) {
            g_startPins.erase(std::remove_if(g_startPins.begin(), g_startPins.end(),
                                             [&](const StartItem& p) { return EqI(p.launch, it.launch); }),
                              g_startPins.end());
        } else {
            StartItem p;
            p.name = it.name;
            p.launch = it.launch;
            g_startPins.push_back(p);
        }
        SaveStartPins();
    } else if (cmd == 2) {
        if (std::none_of(g_pins.begin(), g_pins.end(), [&](const Pin& p) { return EqI(p.value, id); })) g_pins.push_back({L"aumid", id});
        SavePins();
        RebuildApps();
        for (auto* d : g_docks) { BuildElems(*d, true); Invalidate(*d); }
    } else if (cmd == 3) {
        std::wstring args = L"/select,\"" + location + L"\"";
        CloseStart(true);
        ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
        return;
    }
    SetForegroundWindow(g_start.hwnd);
    RenderStart();
}

// Right after the Windows key, Windows' own search box (an invisible CoreWindow in SearchHost) can take
// the keyboard back after Nimbus' Start got it. For a short while, check that the Start keeps the
// foreground, and if a CoreWindow took it, make it let go (Esc) and come back.
constexpr UINT_PTR TIMER_WINKEY_GUARD = 24;
static DWORD g_winKeyGuardStart = 0;
static int g_winKeyGuardEsc = 0;
static DWORD g_startClosedBySearchAt = 0;  // the Start lost the focus to Windows' search box (a Windows key press)

static void StartWinKeyGuard() {
    g_winKeyGuardStart = GetTickCount();
    g_winKeyGuardEsc = 0;
    SetTimer(g_start.hwnd, TIMER_WINKEY_GUARD, 30, nullptr);
}

static void WinKeyGuardStep() {
    auto& s = g_start;
    DWORD since = GetTickCount() - g_winKeyGuardStart;
    if (!s.open || since > 1500) {
        KillTimer(s.hwnd, TIMER_WINKEY_GUARD);
        return;
    }
    HWND fg = GetForegroundWindow();
    if (fg == s.hwnd || s.menuOpen) return;
    wchar_t cls[64] = {};
    if (fg) GetClassNameW(fg, cls, ARRAYSIZE(cls));
    if (!wcscmp(cls, L"Windows.UI.Core.CoreWindow")) {
        // Windows' search box still has the keyboard. (The first 250 ms belong to the Explorer part,
        // which does the same; two Esc presses at once could reach the Start itself.)
        if (since < 250 || g_winKeyGuardEsc >= 3) return;
        g_winKeyGuardEsc++;
        INPUT in[2] = {};
        in[0].type = in[1].type = INPUT_KEYBOARD;
        in[0].ki.wVk = in[1].ki.wVk = VK_ESCAPE;
        in[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, in, sizeof(INPUT));
        Sleep(20);
        ForceForeground(s.hwnd);
        Wh_Log(L"start: took the keyboard back from Windows' search box (%d, ok=%d)", g_winKeyGuardEsc,
            GetForegroundWindow() == s.hwnd);
        return;
    }
    // Windows' search box let go, but the keyboard went back to the previous window (seen with
    // Chrome): the Start is still open, so bring it to the front.
    if (g_winKeyGuardEsc >= 6) return;
    g_winKeyGuardEsc++;
    ForceForeground(s.hwnd);
    Wh_Log(L"start: took the keyboard back from %s after %ums (%d, ok=%d)", cls, since, g_winKeyGuardEsc,
        GetForegroundWindow() == s.hwnd);
}
LRESULT CALLBACK StartProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto& s = g_start;
    switch (msg) {
        case WM_ACTIVATE: {
            // (Right after a Windows key press, Windows' own Start takes the focus for a moment: we
            // close it and come back, so that doesn't count as clicking away.)
            extern DWORD g_lastWinAlone;
            if (LOWORD(wp) == WA_INACTIVE && s.open && !s.menuOpen && GetTickCount() - s.openedAt > 150 &&
                GetTickCount() - g_lastWinAlone > 1500 && !TrayClickJustNow()) {
                // Windows' search box takes the focus as soon as the Windows key goes down: that press
                // is closing the Start, and the Explorer part must not open it again right after.
                HWND other = GetForegroundWindow();
                wchar_t cls[64] = {};
                if (other) GetClassNameW(other, cls, ARRAYSIZE(cls));
                if (!wcscmp(cls, L"Windows.UI.Core.CoreWindow")) g_startClosedBySearchAt = GetTickCount();
                Wh_Log(L"start: lost the focus to %s", cls);
                CloseStart(true);
            }
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;
        case WM_APP_START_OUTSIDE:
            Wh_Log(L"start: click outside (open=%d)", s.open);
            if (s.open && !s.menuOpen && GetTickCount() - s.openedAt > 150) CloseStart(true);
            return 0;
        case WM_TIMER:
            if (wp == TIMER_START_TRAY) {
                KillTimer(hwnd, TIMER_START_TRAY);
                if (TrayClickJustNow()) SetTimer(hwnd, TIMER_START_TRAY, 150, nullptr);
                else CloseStart(true);
            } else if (wp == TIMER_START_ICONS) {
                KillTimer(hwnd, TIMER_START_ICONS);
                RenderStart();
            } else if (wp == TIMER_START_WATCH) {
                StartWatchStep();
            } else if (wp == TIMER_START_POWER) {
                KillTimer(hwnd, TIMER_START_POWER);
                s.powerArm = -1;
                if (s.open) RenderStart();
            } else if (wp == TIMER_WINKEY_GUARD) {
                WinKeyGuardStep();
            } else if (wp == TIMER_START_ANIM) {
                StartAnimStep();
            } else if (wp == TIMER_START_WIDE) {
                StartWideStep();
            } else if (wp == TIMER_CARET) {
                s.caretOn = !s.caretOn;
                RenderStart();
            } else if (wp == TIMER_SEARCH) {
                KillTimer(hwnd, TIMER_SEARCH);
                QueryEverything();
                RenderStart();
            }
            return 0;
        case WM_COPYDATA: {
            auto* cds = (const COPYDATASTRUCT*)lp;
            if (cds && cds->dwData == kEverythingReply) OnEverythingReply(cds);
            if (cds && cds->dwData == kEverythingRecentReply) OnEverythingRecentReply(cds);
            return TRUE;
        }
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            if (!InSearchText(x, y)) {
                int r = StartRowAt(x, y);
                // Status tab: a slider follows the mouse; a part being organized may be dragged.
                if (r >= 0 && s.rows[r].kind == RowSlide && s.rows[r].slide) {
                    s.sliding = true;
                    s.slideFn = s.rows[r].slide;
                    s.slideX0 = s.rows[r].x0;
                    s.slideX1 = s.rows[r].x1;
                    s.slideFn(std::clamp((x - s.offX - s.slideX0) / std::max(1.0f, s.slideX1 - s.slideX0), 0.0f, 1.0f));
                    SetCapture(hwnd);
                    RenderStart();
                    return 0;
                }
                if (r >= 0 && s.rows[r].kind == RowSec) {
                    s.secDrag.down = true;
                    s.secDrag.active = false;
                    s.secDrag.sec = s.rows[r].power;
                    s.secDrag.sx = x;
                    s.secDrag.sy = y;
                    SetCapture(hwnd);
                    return 0;
                }
                // A pinned app: maybe the start of a drag to reorder.
                if (r >= 0 && s.query.empty() && s.tab == 0 && s.rows[r].kind == RowApp && s.rows[r].item && !g_startPins.empty() &&
                    s.rows[r].item >= &g_startPins.front() && s.rows[r].item <= &g_startPins.back()) {
                    s.pinDrag.down = true;
                    s.pinDrag.active = false;
                    s.pinDrag.cur = (int)(s.rows[r].item - &g_startPins.front());
                    s.pinDrag.sx = x;
                    s.pinDrag.sy = y;
                    SetCapture(hwnd);
                }
                return 0;
            }
            size_t pos = QueryPosAt(x);
            DWORD now = GetTickCount();
            if (now - s.lastTextClick < GetDoubleClickTime() && !s.query.empty()) {
                // Double click: the word under the mouse (a third click selects everything).
                size_t a = WordLeft(std::min(pos + 1, s.query.size())), b = WordRight(pos);
                while (b > a && s.query[b - 1] == L' ') b--;
                if (s.anchor == a && s.caret == b) a = 0, b = s.query.size();
                s.anchor = a;
                s.caret = b;
                s.lastTextClick = now;
                RenderStart();
                return 0;
            }
            s.lastTextClick = now;
            s.caret = pos;
            if (!(GetKeyState(VK_SHIFT) & 0x8000)) s.anchor = pos;
            s.selecting = true;
            s.caretOn = true;
            SetCapture(hwnd);
            RenderStart();
            return 0;
        }
        case WM_CAPTURECHANGED:
            s.selecting = false;
            if ((HWND)lp != hwnd) {
                s.sliding = false;
                if (s.secDrag.active) SaveStartSecOrder();
                s.secDrag = {};
            }
            if ((HWND)lp != hwnd && s.pinDrag.down) {
                if (s.pinDrag.active) SaveStartPins();
                s.pinDrag = {};
                RenderStart();
            }
            return 0;
        case WM_MOUSEMOVE: {
            if (s.selecting) {
                size_t pos = QueryPosAt(GET_X_LPARAM(lp));
                if (pos != s.caret) MoveTextCaret(pos, true);
                return 0;
            }
            if (s.sliding && s.slideFn) {
                s.slideFn(std::clamp((GET_X_LPARAM(lp) - s.offX - s.slideX0) / std::max(1.0f, s.slideX1 - s.slideX0), 0.0f, 1.0f));
                RenderStart();
                return 0;
            }
            if (s.secDrag.down) {
                int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
                if (!s.secDrag.active && (abs(x - s.secDrag.sx) > 6 || abs(y - s.secDrag.sy) > 6)) s.secDrag.active = true;
                if (s.secDrag.active) {
                    // Over another part: the dragged one takes its place (the others shift), as with the pinned apps.
                    for (auto& row : s.rows)
                        if (row.kind == RowSec && row.power != s.secDrag.sec && y >= row.rc.top && y < row.rc.bottom) {
                            StartSecMoveTo(s.secDrag.sec, row.power);
                            break;
                        }
                    RenderStart();
                }
                return 0;
            }
            if (s.pinDrag.down) {
                int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
                if (!s.pinDrag.active && (abs(x - s.pinDrag.sx) > 6 || abs(y - s.pinDrag.sy) > 6)) s.pinDrag.active = true;
                if (s.pinDrag.active) {
                    // Over another pinned app: the dragged one takes its place (the others shift).
                    int r = StartRowAt(x, y);
                    if (r >= 0 && s.rows[r].kind == RowApp && s.rows[r].item && s.rows[r].item >= &g_startPins.front() &&
                        s.rows[r].item <= &g_startPins.back()) {
                        int to = (int)(s.rows[r].item - &g_startPins.front()), from = s.pinDrag.cur;
                        if (to != from && from >= 0 && from < (int)g_startPins.size()) {
                            if (to > from) std::rotate(g_startPins.begin() + from, g_startPins.begin() + from + 1, g_startPins.begin() + to + 1);
                            else std::rotate(g_startPins.begin() + to, g_startPins.begin() + from, g_startPins.begin() + from + 1);
                            s.pinDrag.cur = to;
                        }
                    }
                    RenderStart();
                    return 0;
                }
            }
            int r = StartRowAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (r != s.hover) {
                s.hover = r;
                // A search result under the mouse becomes the chosen one (the wide panel shows it).
                if (r >= 0 && !s.query.empty() && (s.rows[r].kind == RowApp || s.rows[r].kind == RowFile) &&
                    s.rows[r].power >= 0)
                    s.sel = s.rows[r].power;
                RenderStart();
            }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            s.hover = -1;
            RenderStart();
            return 0;
        case WM_MOUSEWHEEL:
            if (s.query.empty()) {
                s.scroll -= GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * SS(54) * 1.5f;
                s.scroll = std::clamp(s.scroll, 0.0f, s.maxScroll);
                RenderStart();
            }
            return 0;
        case WM_LBUTTONUP: {
            if (s.selecting) {
                s.selecting = false;
                ReleaseCapture();
                return 0;
            }
            if (s.sliding) {
                s.sliding = false;
                ReleaseCapture();
                RenderStart();
                return 0;
            }
            if (s.secDrag.down) {
                bool moved = s.secDrag.active;
                if (moved) SaveStartSecOrder();
                s.secDrag = {};
                ReleaseCapture();
                RenderStart();
                return 0;
            }
            if (s.pinDrag.down) {
                bool moved = s.pinDrag.active;
                if (moved) SaveStartPins();
                s.pinDrag = {};
                ReleaseCapture();
                if (moved) {
                    RenderStart();
                    return 0;
                }
            }
            int r = StartRowAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (r < 0 || (s.rows[r].kind != RowStatus && s.rows[r].kind != RowFn)) CloseFlyout();
            if (r < 0) return 0;
            if (StartIsTabRow(s.rows[r].kind)) s.kbFocus = -1;
            StartRow row = s.rows[r];
            if (row.kind == RowStatus) {
                if (s.dock) StatClick(*s.dock, row.power, false, true);
                RenderStart();
            } else if (row.kind == RowFn) {
                if (row.fn) row.fn();
                if (g_start.open) RenderStart();
            } else if (row.kind == RowOrg) {
                s.org = !s.org;
                s.secDrag = {};
                CloseFlyout();
                RenderStart();
            } else if (row.kind == RowTray) {
                TrayClick(row.power, 0);
                SetTimer(hwnd, TIMER_START_TRAY, GetDoubleClickTime() + 120, nullptr);  // then the Start closes
            } else if (row.kind == RowTabStatus) {
                s.tab = 3;
                s.scroll = 0;
                StartStatusRefresh();
                RenderStart();
            } else if (row.kind == RowTabPinned || row.kind == RowTabAll || row.kind == RowTabRecent) {
                s.tab = row.kind == RowTabPinned ? 0 : row.kind == RowTabAll ? 1 : 2;
                s.org = false;
                if (s.tab == 2) LoadRecent();
                s.scroll = 0;
                RenderStart();
            } else if (row.kind == RowAction) {
                // (Opening Everything and pinning keep the Iniciar open.)
                if (row.power == ActOpenEverything || row.power == ActPinSel) {
                    RunStartAction(row.power);
                    RenderStart();
                } else {
                    RunStartAction(row.power);
                    CloseStart(true);
                }
            } else if (row.kind == RowPower) {
                DWORD since = GetTickCount() - s.powerArmAt;
                if ((row.power == 2 || row.power == 3) && s.powerArm == row.power && since < GetDoubleClickTime()) return 0;  // (a double-click isn't two)
                if ((row.power == 2 || row.power == 3) && !(s.powerArm == row.power && since < 4000)) {
                    s.powerArm = row.power;
                    s.powerArmAt = GetTickCount();
                    SetTimer(hwnd, TIMER_START_POWER, 4100, nullptr);  // (the button goes back by itself)
                    RenderStart();
                    return 0;
                }
                s.powerArm = -1;
                CloseStart();
                Power(row.power);
            } else if (row.item) {
                StartItem it = *row.item;
                CloseStart(true);
                LaunchItem(it);
            }
            return 0;
        }
        case WM_MBUTTONUP: {
            int r = StartRowAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (r >= 0 && s.rows[r].kind == RowTray) {
                TrayTogglePin(s.rows[r].power);
                TrayChanged();
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            int r = StartRowAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (r >= 0 && s.rows[r].kind == RowPower && s.rows[r].power <= 3) {
                POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                ClientToScreen(hwnd, &pt);
                PowerMenu(pt);
                return 0;
            }
            if (r >= 0 && s.rows[r].kind == RowTray) {
                // (The app's menu takes the focus, and the Start closes.)
                TrayClick(s.rows[r].power, 1);
                return 0;
            }
            if (r >= 0 && s.rows[r].item && (s.rows[r].kind == RowApp || s.rows[r].kind == RowFile)) {
                POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                ClientToScreen(hwnd, &pt);
                if (s.rows[r].kind == RowFile) FileItemMenu(*s.rows[r].item, pt);
                else StartItemMenu(*s.rows[r].item, pt);
            }
            return 0;
        }
        case WM_SYSKEYDOWN:
            if ((wp == VK_UP || wp == VK_DOWN) && s.org && s.tab == 3 && s.query.empty() && s.kbFocus >= 0 &&
                s.kbFocus < (int)s.rows.size() && s.rows[s.kbFocus].kind == RowSec) {
                int sec = s.rows[s.kbFocus].power;
                StartSecStep(sec, wp == VK_DOWN ? 1 : -1);
                SaveStartSecOrder();
                RenderStart();
                for (int i = 0; i < (int)s.rows.size(); i++)
                    if (s.rows[i].kind == RowSec && s.rows[i].power == sec) s.kbFocus = i;
                StartKeepVisible();
                RenderStart();
                return 0;
            }
            break;
        case WM_KEYDOWN:
            switch (wp) {
                case VK_ESCAPE:
                    if (!s.query.empty()) {
                        s.query.clear();
                        UpdateSearch();
                        RenderStart();
                    } else {
                        CloseStart(true);
                    }
                    return 0;
                case VK_TAB:
                    if (s.query.empty()) {
                        StartNav(GetKeyState(VK_SHIFT) & 0x8000 ? 5 : 4);
                        StartKeepVisible();
                        RenderStart();
                    }
                    return 0;
                case VK_RETURN:
                    if (lp & (1 << 30)) return 0;  // (held down: only the first press)
                    if (s.query.empty() && s.kbFocus >= 0) {
                        StartActivateFocus(hwnd);
                        return 0;
                    }
                    if (StartItem* it = SelectedResult()) {
                        StartItem copy = *it;
                        CloseStart(true);
                        LaunchItem(copy);
                    } else if (g_set.fileSearch == L"windows" && s.query.size() >= 2) {
                        RunStartAction(ActExplorerSearch);
                        CloseStart(true);
                    }
                    return 0;
                case VK_DOWN:
                case VK_UP: {
                    if (s.query.empty()) {
                        StartNav(wp == VK_DOWN ? 3 : 2);
                        StartKeepVisible();
                        RenderStart();
                        return 0;
                    }
                    int total = (int)(s.appHits.size() + s.files.size() + s.setHits.size());
                    if (total) {
                        s.sel = (s.sel + (wp == VK_DOWN ? 1 : total - 1)) % total;
                        RenderStart();
                    }
                    return 0;
                }
                case VK_LEFT:
                case VK_RIGHT:
                case VK_HOME:
                case VK_END: {
                    if (s.query.empty() && (wp == VK_LEFT || wp == VK_RIGHT)) {
                        StartNav(wp == VK_LEFT ? 0 : 1);
                        StartKeepVisible();
                        RenderStart();
                        return 0;
                    }
                    bool shift = GetKeyState(VK_SHIFT) & 0x8000, ctrl = GetKeyState(VK_CONTROL) & 0x8000;
                    size_t to;
                    if (wp == VK_HOME) to = 0;
                    else if (wp == VK_END) to = s.query.size();
                    else if (!shift && HasTextSel()) to = wp == VK_LEFT ? std::min(s.caret, s.anchor) : std::max(s.caret, s.anchor);
                    else if (wp == VK_LEFT) to = ctrl ? WordLeft(s.caret) : (s.caret ? s.caret - 1 : 0);
                    else to = ctrl ? WordRight(s.caret) : s.caret + 1;
                    MoveTextCaret(to, shift);
                    return 0;
                }
                case VK_DELETE:
                    if (HasTextSel()) {
                        DeleteTextSel();
                    } else if (s.caret < s.query.size()) {
                        size_t b = (GetKeyState(VK_CONTROL) & 0x8000) ? WordRight(s.caret) : s.caret + 1;
                        s.query.erase(s.caret, b - s.caret);
                    } else {
                        return 0;
                    }
                    s.caretOn = true;
                    UpdateSearch();
                    RenderStart();
                    return 0;
                case 'A':
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        s.anchor = 0;
                        s.caret = s.query.size();
                        RenderStart();
                    }
                    return 0;
                case 'C':
                case 'X':
                    if ((GetKeyState(VK_CONTROL) & 0x8000) && HasTextSel()) {
                        CopyTextSel(hwnd);
                        if (wp == 'X') {
                            DeleteTextSel();
                            UpdateSearch();
                            RenderStart();
                        }
                    }
                    return 0;
                case 'V':
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        if (OpenClipboard(hwnd)) {
                            if (HANDLE hd = GetClipboardData(CF_UNICODETEXT)) {
                                if (auto* t = (const wchar_t*)GlobalLock(hd)) {
                                    std::wstring add = t;
                                    GlobalUnlock(hd);
                                    InsertText(add);
                                    UpdateSearch();
                                    RenderStart();
                                }
                            }
                            CloseClipboard();
                        }
                    }
                    return 0;
            }
            break;
        case WM_CHAR: {
            wchar_t c = (wchar_t)wp;
            if (c == 8) {  // Backspace (Ctrl+Backspace arrives as 127)
                if (HasTextSel()) {
                    DeleteTextSel();
                } else if (s.caret > 0) {
                    s.query.erase(s.caret - 1, 1);
                    s.caret = s.anchor = s.caret - 1;
                }
            } else if (c == 127) {
                if (HasTextSel()) {
                    DeleteTextSel();
                } else {
                    size_t w = WordLeft(s.caret);
                    s.query.erase(w, s.caret - w);
                    s.caret = s.anchor = w;
                }
            } else if (c >= 32) {
                InsertText(std::wstring(1, c));
            } else {
                return 0;
            }
            s.caretOn = true;
            UpdateSearch();
            RenderStart();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Windows key and shortcut. The Windows key is caught inside Explorer (see the explorer.exe part at
// the end), which asks this process to open the Start; nothing here listens to the keyboard.
// ---------------------------------------------------------------------------

constexpr UINT WM_APP_WINKEY = WM_APP + 6;  // sent by the explorer.exe part; returns the open Start window
constexpr UINT WM_APP_WINN = WM_APP + 13;   // sent by the explorer.exe part: Win+N
DWORD g_lastWinAlone = 0;                   // last Windows key press (Windows' search box takes the focus briefly)

// Windows moving between monitors (only matters for "only this monitor's windows"): the shell sends
// nothing for that, so watch moves while some dock needs it, and not during full-screen games.
HWINEVENTHOOK g_moveEndHook, g_locationHook;
void ScheduleRebuild();

void CALLBACK OnWindowMoved(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) return;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return;
    if (!g_winCache.count(hwnd)) return;  // only the windows the docks show
    ScheduleRebuild();
}

static void UpdateMoveHooks(bool anyFullscreen) {
    bool need = !anyFullscreen && std::any_of(g_docks.begin(), g_docks.end(), [](Dock* d) { return d->cfg.onlyThis; });
    if (need && !g_moveEndHook) {
        g_moveEndHook = SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND, nullptr, OnWindowMoved, 0, 0,
                                        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        // (not EVENT_OBJECT_LOCATIONCHANGE: it would bring every caret and cursor move in the system here;
        // a window moved by the keyboard shows at the next periodic check)
        g_locationHook = SetWinEventHook(EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND, nullptr, OnWindowMoved, 0, 0,
                                         WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    } else if (!need && g_moveEndHook) {
        UnhookWinEvent(g_moveEndHook);
        if (g_locationHook) UnhookWinEvent(g_locationHook);
        g_moveEndHook = g_locationHook = nullptr;
    }
}

// The Start shortcut: a system hotkey (no keyboard hook; works over administrator windows).
static void RegisterStartHotkey() {
    UnregisterHotKey(g_msgWnd, 1);
    if (!g_set.nevoaStart || g_set.startHotkey == L"none") return;
    UINT mods = (g_set.startHotkey == L"ctrlSpace" ? MOD_CONTROL : MOD_ALT) | MOD_NOREPEAT;
    if (!RegisterHotKey(g_msgWnd, 1, mods, VK_SPACE))
        Wh_Log(L"Shortcut %s is taken by another program (%lu)", g_set.startHotkey.c_str(), GetLastError());
}

// Tell the explorer.exe part whether the Windows key should open our Start.
static void PublishWinKeyChoice() {
    if (!g_msgWnd) return;
    if (g_set.winKey && g_set.nevoaStart) SetPropW(g_msgWnd, L"NimbusWinKey", (HANDLE)1);
    else RemovePropW(g_msgWnd, L"NimbusWinKey");
    if (g_set.notifWinN) SetPropW(g_msgWnd, L"NimbusWinN", (HANDLE)1);
    else RemovePropW(g_msgWnd, L"NimbusWinN");
}


// ---------------------------------------------------------------------------
// Window procedures
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Status panels: open above the dock's status item (or beside the Start's Status tab), with the Start's
// glass and round corners. They never take the focus, so the window in front keeps the keyboard.
// ---------------------------------------------------------------------------

constexpr UINT_PTR TIMER_FLY_ANIM = 40;
constexpr UINT_PTR TIMER_FLY_PAGE = 45;  // going from one panel to another inside the control center
constexpr UINT_PTR TIMER_FLY_WIFI = 41;
constexpr UINT_PTR TIMER_FLY_WATCH = 42;
constexpr UINT_PTR TIMER_FLY_SHELF = 43;
constexpr UINT_PTR TIMER_FLY_TRAY = 44;  // after a click on a tray icon: close, unless a second click came  // after a drag out: a moved file leaves the shelf
constexpr UINT WM_APP_FLY_OUTSIDE = WM_APP + 11;
constexpr float kFlyW = 360, kFlyPad = 18, kFlyR = 16;

struct FlyHit {
    D2D1_RECT_F r{};
    std::function<void()> click;
    std::function<void(float)> slide;  // sliders: 0..1 from the mouse's x
    float x0 = 0, x1 = 0;
    std::function<void(int)> button;  // right (1) and middle (2) clicks
    std::function<void(bool)> dragOut;  // dragging it out of the panel (true: with the right button)
    std::wstring trayKey;               // a tray icon: dragging it onto another reorders them
};

struct Flyout {
    HWND hwnd = nullptr;
    std::vector<HWND> bands;
    std::vector<RECT> bandRc;
    bool open = false;
    int kind = -1;
    Dock* dock = nullptr;
    bool fromStart = false;
    float scale = 1;
    int w = 0, h = 0;
    RECT rc{};
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    int dibW = 0, dibH = 0;
    ComPtr<ID2D1DCRenderTarget> rt;
    ComPtr<ID2D1DeviceContext> dc;
    ComPtr<ID2D1SolidColorBrush> br;
    ComPtr<IDWriteTextFormat> fBig, fTitle, fText, fSmall, fCode, fWrap, fWrapS;
    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> bmps;
    std::vector<FlyHit> hits;
    D2D1_POINT_2F mouse{-1, -1};
    bool dragging = false;
    FlyHit drag;
    float anim = 1;
    DWORD animStart = 0;
    bool measuring = false;
    float maxH = 2000;  // the room the panel has (DIPs): from the top of the monitor to the dock
    float listScroll = 0, listMax = 0;  // the notifications list scrolls inside the panel
    int back = 0;                       // a panel opened inside another (the control center): where "back" goes
    std::wstring trayDrag;              // the tray icon being dragged to a new place
    float panelScroll = 0, panelMax = 0;  // a panel taller than the room above the dock scrolls (not the notifications: their list does)
    float page = 1;                     // 0..1: the new panel slides in and the height follows
    DWORD pageStart = 0;
    int pageDir = 1;                    // 1 going in (from the right), -1 going back (from the left)
    float fromH = 0, curH = 0, toH = 0;
    bool pageMeasured = false;
    DWORD tMeasure = 0, tDraw = 0, tPresent = 0;  // (timing of the last frame)
    ComPtr<ID2D1Layer> pageLayer;
    long long notifOpen = 0;            // the notification under the mouse shows all its text
    int month = 0;  // calendar: months away from today's
    int pick = -1;  // Wi-Fi: the network showing its button
    bool wasDown = false;
    DWORD openedAt = 0;
    int press = -1;  // the hit pressed (it may become a drag out)
    D2D1_POINT_2F pressPt{};
    bool pressRight = false;
    bool noFocus = false;  // opened by a drag passing over the dock: don't take the focus from it
} g_fly;

static void FlyRender();
void CloseFlyout();

// ---- Drawing (in DIPs; the render target is scaled) ----

static D2D1_COLOR_F FCol(D2D1_COLOR_F c, float a) {
    c.a *= a;
    return c;
}
static D2D1_COLOR_F FInk() { return g_theme.text; }
static D2D1_COLOR_F FSub() { return FCol(g_theme.text, 0.62f); }
static D2D1_COLOR_F FCard() { return FCol(g_theme.hover, 0.6f); }
static D2D1_COLOR_F FHov() { return g_theme.hover; }
static D2D1_COLOR_F FSolidTint() {
    D2D1_COLOR_F c = g_theme.tint;
    c.a = 1;
    return c;
}

static ID2D1SolidColorBrush* FBr(D2D1_COLOR_F c) {
    g_fly.br->SetColor(c);
    return g_fly.br.Get();
}
static bool FDraw() { return !g_fly.measuring && g_fly.rt; }
static bool FIn(const D2D1_RECT_F& r) {
    return g_fly.mouse.x >= r.left && g_fly.mouse.x < r.right && g_fly.mouse.y >= r.top && g_fly.mouse.y < r.bottom;
}
static bool FHot(const D2D1_RECT_F& r) { return !g_fly.dragging && FIn(r); }

static void FHitAdd(D2D1_RECT_F r, std::function<void()> click) {
    FlyHit h;
    h.r = r;
    h.click = std::move(click);
    g_fly.hits.push_back(std::move(h));
}

static void FFill(D2D1_RECT_F r, float radius, D2D1_COLOR_F c) {
    if (FDraw()) g_fly.rt->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), FBr(c));
}

// Text in a box: left, centered or right; vertically centered in [top, top + h].
static float FText(const std::wstring& s, IDWriteTextFormat* f, float x, float top, float w, float h, D2D1_COLOR_F c,
                   DWRITE_TEXT_ALIGNMENT al = DWRITE_TEXT_ALIGNMENT_LEADING) {
    ComPtr<IDWriteTextLayout> l;
    if (!f || FAILED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f, std::max(1.0f, w), 400, &l))) return 0;
    l->SetTextAlignment(al);
    DWRITE_TEXT_METRICS m = {};
    l->GetMetrics(&m);
    if (FDraw()) g_fly.rt->DrawTextLayout(D2D1::Point2F(x, h > 0 ? top + (h - m.height) / 2 : top), l.Get(), FBr(c),
                                          D2D1_DRAW_TEXT_OPTIONS_CLIP);
    return m.height;
}

static void FIcon(const wchar_t* d, float cx, float cy, float size, D2D1_COLOR_F c, float stroke = 1.7f) {
    if (FDraw()) DrawLineIcon(g_fly.rt.Get(), d, cx, cy, size, FBr(c), stroke);
}

static void FSep(float& y) {
    y += 10;
    if (FDraw()) g_fly.rt->FillRectangle(D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 1), FBr(g_theme.sep));
    y += 11;
}

// A link in the footer ("Open Windows' ...").
static void FLink(float& y, const wchar_t* label, std::function<void()> fn) {
    D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y, kFlyW - kFlyPad + 8, y + 32);
    if (FHot(r)) FFill(r, 8, FHov());
    FText(label, g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 32, FHot(r) ? FInk() : FCol(g_theme.text, 0.8f));
    FHitAdd(r, std::move(fn));
    y += 32;
}

static void FSlider(float x, float top, float w, float v, std::function<void(float)> set) {
    float cy = top + 16;
    v = std::clamp(v, 0.0f, 1.0f);
    if (FDraw()) {
        g_fly.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x, cy - 2, x + w, cy + 2), 2, 2), FBr(FCol(g_theme.text, 0.18f)));
        g_fly.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x, cy - 2, x + w * v, cy + 2), 2, 2), FBr(FInk()));
        D2D1_POINT_2F k = D2D1::Point2F(x + w * v, cy);
        g_fly.rt->FillEllipse(D2D1::Ellipse(k, 8, 8), FBr(FSolidTint()));
        g_fly.rt->FillEllipse(D2D1::Ellipse(k, 5.5f, 5.5f), FBr(FInk()));
    }
    FlyHit h;
    h.r = D2D1::RectF(x - 8, top, x + w + 8, top + 32);
    h.slide = std::move(set);
    h.x0 = x;
    h.x1 = x + w;
    g_fly.hits.push_back(std::move(h));
}

static void FButton(D2D1_RECT_F r, const wchar_t* label, std::function<void()> fn) {
    FFill(r, 8, FHot(r) ? FHov() : FCard());
    if (FDraw())
        g_fly.rt->DrawRoundedRectangle(D2D1::RoundedRect(r, 8, 8), FBr(g_theme.sep), 1);
    FText(label, g_fly.fSmall.Get(), r.left, r.top, r.right - r.left, r.bottom - r.top, FInk(), DWRITE_TEXT_ALIGNMENT_CENTER);
    FHitAdd(r, std::move(fn));
}

// Title on the left, a note on the right.
static void FHeader(float& y, const std::wstring& title, const std::wstring& note) {
    float tw = TextSize(title, g_fly.fTitle.Get()).width;
    FText(title, g_fly.fTitle.Get(), kFlyPad, y, tw + 4, 22, FInk());
    if (!note.empty())
        FText(note, g_fly.fSmall.Get(), kFlyPad + tw + 16, y, kFlyW - kFlyPad * 2 - tw - 16, 22, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
    y += 22 + 12;
}

static ID2D1Bitmap* FBitmap(const std::wstring& key, IWICBitmapSource* src) {
    if (!src || !FDraw()) return nullptr;
    auto it = g_fly.bmps.find(key);
    if (it != g_fly.bmps.end()) return it->second.Get();
    ComPtr<ID2D1Bitmap> b;
    g_fly.rt->CreateBitmapFromWicBitmap(src, nullptr, &b);
    g_fly.bmps[key] = b;
    return b.Get();
}

static void SendWinKey(WORD vk) {
    INPUT in[4] = {};
    for (auto& i : in) i.type = INPUT_KEYBOARD;
    in[0].ki.wVk = VK_LWIN;
    in[1].ki.wVk = vk;
    in[2].ki.wVk = vk;
    in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3].ki.wVk = VK_LWIN;
    in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, in, sizeof(INPUT));
}

// ---- Panels ----

static int DayOfWeek(int y, int m, int d) {  // 0 = Sunday
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if (m < 3) y -= 1;
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}
static int DaysIn(int y, int m) {
    static const int n[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
    return m == 2 && leap ? 29 : n[m - 1];
}

static float PanelClock(float y) {
    const SYSTEMTIME& now = g_stat.now;
    y += FText(g_stat.time, g_fly.fBig.Get(), kFlyPad, y, 300, 0, FInk());
    y += FText(g_stat.dateLong, g_fly.fSmall.Get(), kFlyPad, y + 2, kFlyW - kFlyPad * 2, 0, FSub()) + 2;
    FSep(y);

    int ym = now.wYear * 12 + (now.wMonth - 1) + g_fly.month;
    int year = ym / 12, month = ym % 12 + 1;
    SYSTEMTIME first = {};
    first.wYear = (WORD)year;
    first.wMonth = (WORD)month;
    first.wDay = 1;
    wchar_t b[96] = {};
    GetDateFormatEx(g_locale, DATE_YEARMONTH, &first, nullptr, b, ARRAYSIZE(b), nullptr);
    if (b[0]) b[0] = towupper(b[0]);
    FText(b, g_fly.fTitle.Get(), kFlyPad, y, 240, 32, FInk());
    for (int k = 0; k < 2; k++) {
        D2D1_RECT_F r = D2D1::RectF(kFlyW - kFlyPad - 68 + k * 36, y, kFlyW - kFlyPad - 36 + k * 36, y + 32);
        if (FHot(r)) FFill(r, 8, FHov());
        FIcon(k ? L"M9 6l6 6-6 6" : L"M15 6l-6 6 6 6", (r.left + r.right) / 2, y + 16, 16, FInk(), 2);
        FHitAdd(r, [k] { g_fly.month += k ? 1 : -1; });
    }
    y += 40;

    // Weekday letters, from the user's first day of the week.
    wchar_t fd[4] = L"6";
    GetLocaleInfoEx(g_locale, LOCALE_IFIRSTDAYOFWEEK, fd, ARRAYSIZE(fd));
    int firstSun = (_wtoi(fd) + 1) % 7;  // 0 = Sunday
    float cw = (kFlyW - kFlyPad * 2) / 7;
    for (int c = 0; c < 7; c++) {
        int dow = (firstSun + c) % 7;
        wchar_t n[32] = {};
        GetLocaleInfoEx(g_locale, LOCALE_SSHORTESTDAYNAME1 + (dow == 0 ? 6 : dow - 1), n, ARRAYSIZE(n));
        std::wstring s = n;
        if (s.size() > 2) s = s.substr(0, 1);
        if (!s.empty()) s[0] = towupper(s[0]);
        FText(s, g_fly.fSmall.Get(), kFlyPad + c * cw, y, cw, 24, FSub(), DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    y += 26;
    int lead = (DayOfWeek(year, month, 1) - firstSun + 7) % 7;
    int pm = month == 1 ? 12 : month - 1, py = month == 1 ? year - 1 : year;
    for (int i = 0; i < 42; i++) {
        int day = i - lead + 1;
        bool in = day >= 1 && day <= DaysIn(year, month);
        int shown = in ? day : day < 1 ? DaysIn(py, pm) + day : day - DaysIn(year, month);
        bool today = in && year == now.wYear && month == now.wMonth && day == now.wDay;
        float x = kFlyPad + (i % 7) * cw, top = y + (i / 7) * 36;
        float cx = x + cw / 2, cy = top + 18;
        if (today && FDraw()) g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 17, 17), FBr(FInk()));
        FText(std::to_wstring(shown), g_fly.fText.Get(), x, top, cw, 36,
              today ? FSolidTint() : in ? FInk() : FCol(g_theme.text, 0.38f), DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    y += 6 * 36;
    if (g_fly.month != 0) {
        y += 4;
        FLink(y, L2(L"Voltar pra hoje", L"Back to today"), [] { g_fly.month = 0; });
    }
    FSep(y);
    FLink(y, L2(L"Abrir o calendário do Windows", L"Open Windows' calendar"), [] {
        CloseFlyout();
        OpenUri(L"ms-actioncenter:");  // (not Win+N: that may be set to open Nimbus' panel)
    });
    return y;
}

static std::wstring PctText(float v) { return std::to_wstring((int)std::lround(v * 100)) + L"%"; }

static float PanelVolume(float y) {
    FHeader(y, L2(L"Som", L"Sound"), g_stat.device);
    if (!g_stat.audio) {
        FText(L2(L"Nenhuma saída de som ligada.", L"No sound output is on."), g_fly.fSmall.Get(), kFlyPad, y, 300, 32, FSub());
        y += 32;
    } else {
        D2D1_RECT_F mb = D2D1::RectF(kFlyPad - 4, y, kFlyPad + 28, y + 32);
        if (FHot(mb)) FFill(mb, 8, FHov());
        FIcon(StatIcon(StVolume), (mb.left + mb.right) / 2, y + 16, 19, FInk());
        FHitAdd(mb, [] { ToggleMute(); });
        float sx = kFlyPad + 40, sw = kFlyW - kFlyPad - 48 - sx;
        FSlider(sx, y, sw, g_stat.muted ? 0 : g_stat.vol, [](float v) { SetVolume(v); });
        FText(g_stat.muted ? L2(L"Mudo", L"Muted") : PctText(g_stat.vol), g_fly.fSmall.Get(), kFlyW - kFlyPad - 44, y, 44, 32,
              FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
        y += 32;
    }
    FSep(y);
    if (g_appVols.empty()) {
        FText(L2(L"Nenhum app está tocando som agora.", L"No app is playing sound now."), g_fly.fSmall.Get(), kFlyPad, y,
              300, 28, FSub());
        y += 28;
    } else {
        FText(L2(L"Por app", L"Apps"), g_fly.fSmall.Get(), kFlyPad, y, 200, 20, FSub());
        y += 24;
        for (size_t i = 0; i < g_appVols.size(); i++) {
            AppVol& a = g_appVols[i];
            float cy = y + 16;
            if (ID2D1Bitmap* bmp = FBitmap(a.path, a.icon.Get())) {
                D2D1_RECT_F ir = D2D1::RectF(kFlyPad, cy - 11, kFlyPad + 22, cy + 11);
                if (g_fly.dc) g_fly.dc->DrawBitmap(bmp, ir, 1, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
                else g_fly.rt->DrawBitmap(bmp, ir);
            } else {
                FIcon(a.system ? L"M4 9h4l5-4v14l-5-4H4z M16.5 8.5a5 5 0 0 1 0 7" : L"M4 5h16v14H4z M4 9h16", kFlyPad + 11, cy, 18,
                      FSub());
            }
            FText(a.name, g_fly.fSmall.Get(), kFlyPad + 32, y, 92, 32, FInk());
            float sx = kFlyPad + 134, sw = kFlyW - kFlyPad - 48 - sx;
            FSlider(sx, y, sw, a.level, [i](float v) {
                if (i < g_appVols.size()) SetAppVolume(g_appVols[i], v);
            });
            FText(PctText(a.level), g_fly.fSmall.Get(), kFlyW - kFlyPad - 44, y, 44, 32, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
            y += 36;
        }
    }
    FSep(y);
    FLink(y, L2(L"Abrir o mixer de volume do Windows", L"Open Windows' volume mixer"), [] {
        CloseFlyout();
        OpenUri(L"ms-settings:apps-volume");
    });
    return y;
}

static float PanelNetwork(float y) {
    FHeader(y, L2(L"Rede", L"Network"), L"");
    D2D1_RECT_F card = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 60);
    FFill(card, 12, FCard());
    if (FDraw()) g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(card.left + 30, y + 30), 18, 18), FBr(FInk()));
    FIcon(StatIcon(StNetwork), card.left + 30, y + 30, 18, FSolidTint(), 1.9f);
    std::wstring name = g_stat.net == 2 ? g_stat.wifiName : g_stat.net == 1 ? L2(L"Cabo de rede", L"Ethernet")
                                                                              : L2(L"Desconectado", L"Disconnected");
    std::wstring state = g_stat.net == 0     ? L2(L"Sem conexão", L"Not connected")
                         : g_stat.internet ? L2(L"Conectado, com internet", L"Connected, with internet")
                                           : L2(L"Conectado, sem internet", L"Connected, no internet");
    FText(name, g_fly.fText.Get(), card.left + 60, y + 11, card.right - card.left - 72, 0, FInk());
    FText(state, g_fly.fSmall.Get(), card.left + 60, y + 31, card.right - card.left - 72, 0, FSub());
    y += 60 + 12;
    if (g_stat.hasWifi) {
        FText(L2(L"Redes Wi-Fi", L"Wi-Fi networks"), g_fly.fSmall.Get(), kFlyPad, y, 200, 24, FSub());
        D2D1_RECT_F sw = D2D1::RectF(kFlyW - kFlyPad - 40, y + 1, kFlyW - kFlyPad, y + 23);
        bool on = g_stat.wifiOn;
        FFill(sw, 11, on ? FInk() : FCol(g_theme.text, 0.18f));
        if (FDraw())
            g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(on ? sw.right - 11 : sw.left + 11, (sw.top + sw.bottom) / 2), 8, 8),
                                  FBr(on ? FSolidTint() : FInk()));
        FHitAdd(sw, [] { SetWifiRadio(!g_stat.wifiOn); });
        y += 30;
        if (g_stat.wifiOn) {
            if (g_stat.nets.empty()) {
                FText(L2(L"Procurando redes…", L"Looking for networks…"), g_fly.fSmall.Get(), kFlyPad, y, 300, 30, FSub());
                y += 30;
            }
            for (int i = 0; i < (int)g_stat.nets.size(); i++) {
                const WifiNet& n = g_stat.nets[i];
                D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y, kFlyW - kFlyPad + 8, y + 42);
                if (FHot(r) || g_fly.pick == i) FFill(r, 10, FCard());
                FIcon(WifiIcon(n.signal), kFlyPad + 10, y + 21, 18, FInk());
                const wchar_t* note = n.connected ? L2(L"Conectada", L"Connected")
                                      : n.known   ? L2(L"Salva", L"Saved")
                                      : n.secure  ? L2(L"Protegida", L"Secured")
                                                  : L2(L"Aberta", L"Open");
                FText(n.ssid, g_fly.fText.Get(), kFlyPad + 32, y, 190, 42, FInk());
                FText(note, g_fly.fSmall.Get(), kFlyW - kFlyPad - 90, y, 90, 42, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
                FHitAdd(r, [i] { g_fly.pick = g_fly.pick == i ? -1 : i; });
                y += 44;
                if (g_fly.pick == i) {
                    if (!n.known && !n.connected && n.secure)
                        FText(L2(L"A senha é digitada na janela do Windows.", L"The password goes in Windows' own list."),
                              g_fly.fSmall.Get(), kFlyPad + 32, y, 190, 36, FSub());
                    WifiNet copy = n;
                    FButton(D2D1::RectF(kFlyW - kFlyPad - 104, y + 2, kFlyW - kFlyPad, y + 34),
                            n.connected ? L2(L"Desconectar", L"Disconnect") : L2(L"Conectar", L"Connect"), [copy] {
                                ConnectWifi(copy);
                                g_fly.pick = -1;
                            });
                    y += 42;
                }
            }
        }
    }
    FSep(y);
    FLink(y, L2(L"Configurações de rede", L"Network settings"), [] {
        CloseFlyout();
        OpenUri(L"ms-settings:network");
    });
    return y;
}

static float PanelKeyboard(float y) {
    FHeader(y, L2(L"Teclado", L"Keyboard"), L2(L"Win + Espaço também troca", L"Win + Space switches too"));
    for (HKL h : g_stat.layouts) {
        const KbInfo& k = KbOf(h);
        D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y, kFlyW - kFlyPad + 8, y + 48);
        if (FHot(r)) FFill(r, 10, FCard());
        FText(KbShort(h), g_fly.fCode.Get(), kFlyPad, y, 46, 48, FInk());
        FText(k.name, g_fly.fText.Get(), kFlyPad + 54, y + 5, 220, 0, FInk());
        FText(k.layout, g_fly.fSmall.Get(), kFlyPad + 54, y + 25, 220, 0, FSub());
        if (h == g_stat.cur) FIcon(L"M5 12l5 5 9-10", kFlyW - kFlyPad - 10, y + 24, 16, FInk(), 2.2f);
        FHitAdd(r, [h] { SetKeyboard(h); });
        y += 50;
    }
    FSep(y);
    FLink(y, L2(L"Idiomas e teclados", L"Languages and keyboards"), [] {
        CloseFlyout();
        OpenUri(L"ms-settings:regionlanguage");
    });
    return y;
}

static float PanelBattery(float y) {
    FHeader(y, L2(L"Bateria", L"Battery"), g_stat.plugged ? L2(L"Na tomada", L"Plugged in") : L2(L"Na bateria", L"On battery"));
    std::wstring pct = std::to_wstring(g_stat.pct) + L"%";
    float bw = TextSize(pct, g_fly.fBig.Get()).width;
    float bh = FText(pct, g_fly.fBig.Get(), kFlyPad, y, bw + 4, 0, FInk());
    std::wstring note;
    if (g_stat.charging) note = L2(L"carregando", L"charging");
    else if (!g_stat.plugged && g_stat.secsLeft != (DWORD)-1) {
        wchar_t b[64];
        swprintf_s(b, L2(L"%d h %02d min restantes", L"%d h %02d min left"), g_stat.secsLeft / 3600, g_stat.secsLeft / 60 % 60);
        note = b;
    }
    FText(note, g_fly.fSmall.Get(), kFlyPad + bw + 12, y, 200, bh, FSub());
    y += bh + 8;
    FFill(D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 6), 3, FCol(g_theme.text, 0.12f));
    FFill(D2D1::RectF(kFlyPad, y, kFlyPad + (kFlyW - kFlyPad * 2) * g_stat.pct / 100.0f, y + 6), 3, FInk());
    y += 6 + 16;
    FText(L2(L"Modo de energia", L"Power mode"), g_fly.fSmall.Get(), kFlyPad, y, 300, 20, FSub());
    y += 26;
    D2D1_RECT_F seg = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 36);
    FFill(seg, 9, FCard());
    const wchar_t* names[3] = {L2(L"Economia", L"Efficiency"), L2(L"Equilibrado", L"Balanced"), L2(L"Desempenho", L"Performance")};
    int cur = PowerModeCached();
    float ow = (seg.right - seg.left - 6) / 3;
    for (int i = 0; i < 3; i++) {
        D2D1_RECT_F o = D2D1::RectF(seg.left + 3 + i * ow, seg.top + 3, seg.left + 3 + (i + 1) * ow, seg.bottom - 3);
        if (i == cur) FFill(o, 7, FInk());
        else if (FHot(o)) FFill(o, 7, FHov());
        FText(names[i], g_fly.fSmall.Get(), o.left, o.top, o.right - o.left, o.bottom - o.top, i == cur ? FSolidTint() : FInk(),
              DWRITE_TEXT_ALIGNMENT_CENTER);
        FHitAdd(o, [i] { SetPowerMode(i); });
    }
    y += 36;
    FSep(y);
    FLink(y, L2(L"Opções de energia", L"Power options"), [] {
        CloseFlyout();
        OpenUri(L"ms-settings:powersleep");
    });
    return y;
}

// A click on a tray icon in the panel (by key: the list may change before the click).
static void TrayPanelClick(const std::wstring& key, int button) {
    int i = TrayByKey(key);
    if (i < 0) return;
    if (button == 2) {
        TrayTogglePin(i);
        TrayChanged();
        return;
    }
    if (button == 1) {
        CloseFlyout();
        TrayClick(i, button);
        return;
    }
    TrayClick(i, button);
    SetTimer(g_fly.hwnd, TIMER_FLY_TRAY, GetDoubleClickTime() + 120, nullptr);  // then the panel closes
}

static void TrayPanelHit(D2D1_RECT_F r, const std::wstring& key) {
    // While dragging: a bar where it would go.
    if (!g_fly.trayDrag.empty() && g_fly.trayDrag != key && FHot(r) && FDraw()) {
        bool after = g_fly.mouse.x > (r.left + r.right) / 2;
        float bx = after ? r.right - 1 : r.left - 1;
        g_fly.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(bx - 1, r.top + 4, bx + 1, r.bottom - 4), 1, 1), FBr(FInk()));
    }
    FlyHit h;
    h.r = r;
    h.trayKey = key;
    h.click = [key] { TrayPanelClick(key, 0); };
    h.button = [key](int b) { TrayPanelClick(key, b); };
    g_fly.hits.push_back(std::move(h));
}

static void FTrayIcon(int ti, float cx, float cy, float size) {
    TrayIcon& t = g_tray[ti];
    std::wstring key = L"#tray:" + t.key + L":" + std::to_wstring(t.iconVer);
    if (FDraw() && !g_fly.bmps.count(key)) {
        std::wstring prefix = L"#tray:" + t.key + L":";
        for (auto o = g_fly.bmps.begin(); o != g_fly.bmps.end();) o = o->first.rfind(prefix, 0) == 0 ? g_fly.bmps.erase(o) : std::next(o);
    }
    ID2D1Bitmap* bmp = FBitmap(key, t.icon.Get());
    if (!bmp) return;
    D2D1_RECT_F ir = D2D1::RectF(cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
    float alpha = g_fly.trayDrag == t.key ? 0.35f : 1.0f;  // (the one being dragged)
    if (g_fly.dc) g_fly.dc->DrawBitmap(bmp, ir, alpha, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
    else g_fly.rt->DrawBitmap(bmp, ir, alpha);
}

// The tray as a panel (grid and list styles).
static float PanelTray(float y) {
    std::vector<int> order = TrayOrder();
    FHeader(y, L2(L"Em segundo plano", L"In the background"), std::to_wstring(order.size()));
    if (order.empty()) {
        FText(L2(L"Nenhum programa com ícone agora.", L"No app has an icon now."), g_fly.fSmall.Get(), kFlyPad, y, 300, 28, FSub());
        y += 28;
    } else if (TrayStyle(L"lista")) {
        for (int ti : order) {
            const TrayIcon& t = g_tray[ti];
            D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y, kFlyW - kFlyPad + 8, y + 46);
            if (FHot(r)) FFill(r, 10, FHov());
            FTrayIcon(ti, kFlyPad + 11, y + 23, 22);
            bool pinned = TrayIsPinned(t);
            float tw = kFlyW - kFlyPad * 2 - 34 - (pinned ? 56 : 0);
            std::wstring sub = t.tip;
            size_t nl = sub.find_first_of(L"\r\n");
            if (nl != std::wstring::npos) sub = sub.substr(0, nl);
            if (sub == t.name) sub.clear();
            if (sub.empty()) {
                FText(t.name, g_fly.fText.Get(), kFlyPad + 34, y, tw, 46, FInk());
            } else {
                FText(t.name, g_fly.fText.Get(), kFlyPad + 34, y + 5, tw, 0, FInk());
                FText(sub, g_fly.fSmall.Get(), kFlyPad + 34, y + 25, tw, 0, FSub());
            }
            if (pinned)
                FText(L2(L"Fixado", L"Pinned"), g_fly.fSmall.Get(), kFlyW - kFlyPad - 56, y, 56, 46, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
            TrayPanelHit(r, t.key);
            y += 48;
        }
        y -= 2;
    } else {
        float cw = (kFlyW - kFlyPad * 2) / 5, ch = 52;
        for (size_t k = 0; k < order.size(); k++) {
            int ti = order[k];
            float x = kFlyPad + (k % 5) * cw, top = y + (k / 5) * (ch + 4);
            D2D1_RECT_F r = D2D1::RectF(x + 2, top, x + cw - 2, top + ch);
            if (FHot(r)) FFill(r, 10, FHov());
            FTrayIcon(ti, (r.left + r.right) / 2, top + ch / 2, 22);
            if (TrayIsPinned(g_tray[ti]) && FDraw())
                g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(r.right - 8, top + 8), 3, 3), FBr(FInk()));
            TrayPanelHit(r, g_tray[ti].key);
        }
        y += ((order.size() + 4) / 5) * (ch + 4) - 4;
    }
    FSep(y);
    FText(L2(L"O botão do meio fixa um ícone no começo.", L"The middle button pins an icon to the front."), g_fly.fSmall.Get(),
          kFlyPad, y, kFlyW - kFlyPad * 2, 24, FSub());
    y += 24;
    return y;
}

// ---- Shelf ----

static void OpenFlyout(Dock& d, int kind, bool fromStart);

struct ShelfUi {
    bool over = false;         // a drag is over the panel
    bool draggingOut = false;  // our own drag out (the panel mustn't take it back)
} g_shelfUi;
ComPtr<ID2D1StrokeStyle> g_dashStroke;

static bool PathExists(const std::wstring& p) { return GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES; }

// Pasted files live in this user's folder of the mod's storage, in "Shelf".
static std::wstring ShelfDirPath() {
    std::wstring dir = UserDir(false);
    return dir.empty() ? L"" : dir + L"\\Shelf";
}
static std::wstring ShelfDir() {  // (made when a file is pasted)
    std::wstring dir = ShelfDirPath();
    if (!dir.empty()) SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    return dir;
}
static bool InShelfDir(const std::wstring& path) {
    std::wstring dir = ShelfDirPath();
    if (dir.empty()) return false;
    wchar_t full[MAX_PATH * 2];
    if (!GetFullPathNameW(path.c_str(), ARRAYSIZE(full), full, nullptr)) return false;
    std::wstring f = full;
    dir += L'\\';
    return f.size() > dir.size() && _wcsnicmp(f.c_str(), dir.c_str(), dir.size()) == 0 && f.find(L"..") == std::wstring::npos;
}

static void ShelfFill(ShelfItem& it) {
    size_t a = it.path.find_last_of(L'\\');
    it.name = a == std::wstring::npos ? it.path : it.path.substr(a + 1);
    WIN32_FILE_ATTRIBUTE_DATA fa = {};
    if (GetFileAttributesExW(it.path.c_str(), GetFileExInfoStandard, &fa)) {
        if (fa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            it.sub = L2(L"Pasta", L"Folder");
        } else {
            wchar_t b[64] = {};
            StrFormatByteSizeW(((ULONGLONG)fa.nFileSizeHigh << 32) | fa.nFileSizeLow, b, ARRAYSIZE(b));
            it.sub = b;
        }
    }
    if (it.gen) it.sub = L2(L"colado · ", L"pasted · ") + it.sub;
    ComPtr<IShellItem> si;
    if (SUCCEEDED(SHCreateItemFromParsingName(it.path.c_str(), nullptr, IID_PPV_ARGS(&si)))) it.icon = IconFromItem(si.Get(), 64);
}

static void SaveShelf() {
    std::vector<std::wstring> lines;
    for (auto& it : g_shelf) lines.push_back(std::wstring(it.gen ? L"g|" : L"f|") + it.path);
    StoreItems(L"shelf.items", lines);
}

static bool InShelfDir(const std::wstring& path);
static void LoadShelf() {
    g_shelf.clear();
    std::vector<std::wstring> lines;
    LoadItems(L"shelf.items", L'\n', lines);
    for (auto& line : lines) {
        if (line.size() < 3 || line[1] != L'|') continue;
        ShelfItem it;
        // (a pasted file is ours only when it's in our own folder: nothing else is ever deleted)
        it.gen = line[0] == L'g' && InShelfDir(line.substr(2));
        it.path = line.substr(2);
        if (!PathExists(it.path)) continue;
        ShelfFill(it);
        g_shelf.push_back(std::move(it));
    }
}

static void ShelfChanged() {
    SaveShelf();
    StatusChanged();
}

static int ShelfFind(const std::wstring& path) {
    for (int i = 0; i < (int)g_shelf.size(); i++)
        if (!_wcsicmp(g_shelf[i].path.c_str(), path.c_str())) return i;
    return -1;
}

// Pasted files are ours: removing them from the shelf deletes them (unless they were dragged out).
static void ShelfDropItem(int i) {
    if (i < 0 || i >= (int)g_shelf.size()) return;
    if (g_shelf[i].gen && InShelfDir(g_shelf[i].path) && PathExists(g_shelf[i].path)) DeleteFileW(g_shelf[i].path.c_str());
    g_shelf.erase(g_shelf.begin() + i);
}

static bool ShelfAdd(const std::wstring& path, bool gen) {
    if (path.empty() || ShelfFind(path) >= 0 || !PathExists(path)) return false;
    ShelfItem it;
    it.path = path;
    it.gen = gen;
    ShelfFill(it);
    g_shelf.push_back(std::move(it));
    while ((int)g_shelf.size() > g_set.shelfLimit) ShelfDropItem(0);  // (the oldest goes)
    return true;
}

// Files moved or deleted elsewhere leave the shelf.
static bool ShelfPrune() {
    size_t before = g_shelf.size();
    g_shelf.erase(std::remove_if(g_shelf.begin(), g_shelf.end(), [](const ShelfItem& it) { return !PathExists(it.path); }),
                  g_shelf.end());
    return g_shelf.size() != before;
}

static void ShelfClear() {
    while (!g_shelf.empty()) ShelfDropItem(0);
    ShelfChanged();
}

static std::wstring PasteName(const wchar_t* pt, const wchar_t* en, const wchar_t* ext) {
    SYSTEMTIME t;
    GetLocalTime(&t);
    wchar_t b[128];
    swprintf_s(b, L"%s %04d-%02d-%02d %02d.%02d.%02d%s", L2(pt, en), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, ext);
    std::wstring dir = ShelfDir(), path = dir + L"\\" + b;
    for (int n = 2; PathExists(path) && n < 100; n++) {
        swprintf_s(b, L"%s %04d-%02d-%02d %02d.%02d.%02d (%d)%s", L2(pt, en), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, n, ext);
        path = dir + L"\\" + b;
    }
    return path;
}

static bool WriteBytes(const std::wstring& path, const void* data, DWORD n) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    DWORD w = 0;
    BOOL ok = WriteFile(f, data, n, &w, nullptr) && w == n;
    CloseHandle(f);
    if (!ok) DeleteFileW(path.c_str());
    return ok;
}

static bool SavePng(IWICBitmapSource* src, const std::wstring& path) {
    ComPtr<IWICStream> st;
    ComPtr<IWICBitmapEncoder> enc;
    ComPtr<IWICBitmapFrameEncode> fr;
    if (!src || FAILED(g_wic->CreateStream(&st)) || FAILED(st->InitializeFromFilename(path.c_str(), GENERIC_WRITE)) ||
        FAILED(g_wic->CreateEncoder(GUID_ContainerFormatPng, nullptr, &enc)) || FAILED(enc->Initialize(st.Get(), WICBitmapEncoderNoCache)) ||
        FAILED(enc->CreateNewFrame(&fr, nullptr)) || FAILED(fr->Initialize(nullptr)))
        return false;
    UINT w = 0, h = 0;
    src->GetSize(&w, &h);
    WICPixelFormatGUID fmt = GUID_WICPixelFormat24bppBGR;
    fr->SetSize(w, h);
    fr->SetPixelFormat(&fmt);
    ComPtr<IWICFormatConverter> conv;
    if (FAILED(g_wic->CreateFormatConverter(&conv)) ||
        FAILED(conv->Initialize(src, fmt, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom)))
        return false;
    return SUCCEEDED(fr->WriteSource(conv.Get(), nullptr)) && SUCCEEDED(fr->Commit()) && SUCCEEDED(enc->Commit());
}

// Ctrl+V (or "Paste"): files copied in Explorer come in as they are; a picture or text becomes a file.
// The clipboard is read only now, never watched, and what a password manager marks private is skipped.
static void ShelfPaste() {
    if (!OpenClipboard(g_fly.hwnd)) return;
    static UINT exclude = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    static UINT png = RegisterClipboardFormatW(L"PNG");
    int added = 0;
    if (!IsClipboardFormatAvailable(exclude)) {
        if (IsClipboardFormatAvailable(CF_HDROP)) {
            HDROP hd = (HDROP)GetClipboardData(CF_HDROP);
            UINT n = hd ? DragQueryFileW(hd, 0xFFFFFFFF, nullptr, 0) : 0;
            for (UINT i = 0; i < n; i++) {
                wchar_t f[MAX_PATH * 2];
                if (DragQueryFileW(hd, i, f, ARRAYSIZE(f)) && ShelfAdd(f, false)) added++;
            }
        } else if (IsClipboardFormatAvailable(png)) {
            HGLOBAL h = (HGLOBAL)GetClipboardData(png);
            if (void* p = h ? GlobalLock(h) : nullptr) {
                std::wstring path = PasteName(L"Imagem colada", L"Pasted image", L".png");
                if (WriteBytes(path, p, (DWORD)GlobalSize(h)) && ShelfAdd(path, true)) added++;
                GlobalUnlock(h);
            }
        } else if (IsClipboardFormatAvailable(CF_BITMAP)) {
            HBITMAP hb = (HBITMAP)GetClipboardData(CF_BITMAP);
            ComPtr<IWICBitmap> wb;
            if (hb && SUCCEEDED(g_wic->CreateBitmapFromHBITMAP(hb, nullptr, WICBitmapIgnoreAlpha, &wb))) {
                std::wstring path = PasteName(L"Imagem colada", L"Pasted image", L".png");
                if (SavePng(wb.Get(), path) && ShelfAdd(path, true)) added++;
                else DeleteFileW(path.c_str());
            }
        } else if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            HGLOBAL h = (HGLOBAL)GetClipboardData(CF_UNICODETEXT);
            if (const wchar_t* t = h ? (const wchar_t*)GlobalLock(h) : nullptr) {
                int n = WideCharToMultiByte(CP_UTF8, 0, t, -1, nullptr, 0, nullptr, nullptr);
                std::string u8(n > 0 ? n - 1 : 0, '\0');
                if (n > 1) WideCharToMultiByte(CP_UTF8, 0, t, -1, u8.data(), n, nullptr, nullptr);
                GlobalUnlock(h);
                if (!u8.empty()) {
                    std::wstring path = PasteName(L"Texto colado", L"Pasted text", L".txt");
                    if (WriteBytes(path, u8.data(), (DWORD)u8.size()) && ShelfAdd(path, true)) added++;
                }
            }
        }
    }
    CloseClipboard();
    Wh_Log(L"shelf: pasted %d", added);
    if (added) ShelfChanged();
}

static ComPtr<IDataObject> ShelfDataObject(const std::wstring& path) {
    ComPtr<IShellItem> si;
    ComPtr<IDataObject> obj;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&si))))
        si->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&obj));
    return obj;
}

// Dragging an item out (to a folder, an app, a chat...). The shelf keeps it unless it was moved.
static void ShelfDragOut(const std::wstring& path, bool right) {
    int i = ShelfFind(path);
    if (i < 0) return;
    bool gen = g_shelf[i].gen;
    ComPtr<IDataObject> obj = ShelfDataObject(path);
    if (!obj) return;
    // Pasted files always move (no leftovers in the temp folder); the right button lets Windows ask.
    DWORD allowed = gen ? DROPEFFECT_MOVE
                    : right ? (DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK)
                    : g_set.shelfDrag == L"mover" ? DROPEFFECT_MOVE
                    : g_set.shelfDrag == L"windows" ? (DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK)
                                                    : DROPEFFECT_COPY;
    DWORD eff = 0;
    g_shelfUi.draggingOut = true;
    HRESULT hr = SHDoDragDrop(g_fly.hwnd, obj.Get(), nullptr, allowed, &eff);
    g_shelfUi.draggingOut = false;
    Wh_Log(L"shelf: dragged out (hr=0x%08lx effect=%lu)", (unsigned long)hr, eff);
    // A move by Explorer may finish a moment later.
    SetTimer(g_fly.hwnd, TIMER_FLY_SHELF, 1500, nullptr);
    if (ShelfPrune()) ShelfChanged();
}

static void ShelfOpen(const std::wstring& path) {
    CloseFlyout();
    ShellExecuteW(nullptr, L"open", NativePath(path).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void ShelfMenu(const std::wstring& path) {
    int i = ShelfFind(path);
    if (i < 0) return;
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, L2(L"Abrir", L"Open"));
    AppendMenuW(m, MF_STRING, 2, L2(L"Mostrar na pasta", L"Show in folder"));
    AppendMenuW(m, MF_STRING, 3, L2(L"Copiar", L"Copy"));
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 4, L2(L"Remover da prateleira", L"Remove from the shelf"));
    POINT pt;
    GetCursorPos(&pt);
    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_fly.hwnd, nullptr);
    DestroyMenu(m);
    if (cmd == 1) {
        ShelfOpen(path);
    } else if (cmd == 2) {
        CloseFlyout();
        std::wstring args = L"/select,\"" + path + L"\"";
        ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
    } else if (cmd == 3) {
        if (ComPtr<IDataObject> obj = ShelfDataObject(path)) {
            OleSetClipboard(obj.Get());
            OleFlushClipboard();
        }
    } else if (cmd == 4) {
        ShelfDropItem(ShelfFind(path));
        ShelfChanged();
    }
}

static void ShelfPanelMenu() {
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, L2(L"Colar (Ctrl+V)", L"Paste (Ctrl+V)"));
    AppendMenuW(m, MF_STRING | (g_shelf.empty() ? MF_GRAYED : 0), 2, L2(L"Limpar a prateleira", L"Clear the shelf"));
    POINT pt;
    GetCursorPos(&pt);
    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_fly.hwnd, nullptr);
    DestroyMenu(m);
    if (cmd == 1) ShelfPaste();
    else if (cmd == 2) ShelfClear();
}

static void ShelfUninit() {
    // Pasted files still on the shelf go with it.
    for (int i = (int)g_shelf.size() - 1; i >= 0; i--)
        if (g_shelf[i].gen) ShelfDropItem(i);
    SaveShelf();
    std::wstring dir = ShelfDirPath();  // (not made here when it isn't there)
    if (!dir.empty()) RemoveDirectoryW(dir.c_str());  // (only when empty)
    g_shelf.clear();
}

// Files dragged over the dock or the panel (Windows' drag image keeps showing).
static void ShelfOpenFor(Dock& d) {
    if (g_fly.open && g_fly.kind == StShelf) return;
    g_fly.noFocus = true;
    CloseFlyout();
    OpenFlyout(d, StShelf, false);
    g_fly.noFocus = false;
}

class ShelfDrop final : public IDropTarget {
    LONG ref_ = 1;
    HWND hwnd_;
    Dock* dock_;  // nullptr: the panel itself
    ComPtr<IDropTargetHelper> helper_;
    bool ok_ = false;
    DWORD eff_ = DROPEFFECT_NONE;

   public:
    ShelfDrop(HWND h, Dock* d) : hwnd_(h), dock_(d) {
        CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&helper_));
    }
    STDMETHODIMP QueryInterface(REFIID riid, void** out) override {
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *out = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *out = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&ref_); }
    STDMETHODIMP_(ULONG) Release() override {
        LONG r = InterlockedDecrement(&ref_);
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP DragEnter(IDataObject* o, DWORD, POINTL pt, DWORD* e) override {
        FORMATETC fe = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        ok_ = g_set.shelfPlace != 0 && !g_shelfUi.draggingOut && o && o->QueryGetData(&fe) == S_OK;
        // A link: the shelf only keeps where the file is (the source mustn't think it was copied).
        eff_ = !ok_ ? DROPEFFECT_NONE : (*e & DROPEFFECT_LINK) ? DROPEFFECT_LINK : (*e & DROPEFFECT_COPY) ? DROPEFFECT_COPY : DROPEFFECT_NONE;
        *e = eff_;
        POINT p = {pt.x, pt.y};
        if (helper_) helper_->DragEnter(hwnd_, o, &p, *e);
        if (ok_) {
            if (dock_) {
                ShelfOpenFor(*dock_);
            } else {
                g_shelfUi.over = true;
                FlyRender();
            }
        }
        return S_OK;
    }
    STDMETHODIMP DragOver(DWORD, POINTL pt, DWORD* e) override {
        *e = eff_;
        POINT p = {pt.x, pt.y};
        if (helper_) helper_->DragOver(&p, *e);
        return S_OK;
    }
    STDMETHODIMP DragLeave() override {
        if (helper_) helper_->DragLeave();
        if (!dock_ && g_shelfUi.over) {
            g_shelfUi.over = false;
            FlyRender();
        }
        return S_OK;
    }
    STDMETHODIMP Drop(IDataObject* o, DWORD, POINTL pt, DWORD* e) override {
        POINT p = {pt.x, pt.y};
        if (helper_) helper_->Drop(o, &p, eff_);
        *e = eff_;
        g_shelfUi.over = false;
        if (!ok_) return S_OK;
        FORMATETC fe = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM sm = {};
        int added = 0;
        if (SUCCEEDED(o->GetData(&fe, &sm))) {
            if (HDROP hd = (HDROP)GlobalLock(sm.hGlobal)) {
                UINT n = DragQueryFileW(hd, 0xFFFFFFFF, nullptr, 0);
                for (UINT i = 0; i < n; i++) {
                    wchar_t f[MAX_PATH * 2];
                    if (DragQueryFileW(hd, i, f, ARRAYSIZE(f)) && ShelfAdd(f, false)) added++;
                }
                GlobalUnlock(sm.hGlobal);
            }
            ReleaseStgMedium(&sm);
        }
        Wh_Log(L"shelf: %d dropped in", added);
        if (dock_) ShelfOpenFor(*dock_);
        ShelfChanged();
        return S_OK;
    }
};

static void RegisterShelfDrop(HWND h, Dock* d) {
    auto* t = new ShelfDrop(h, d);
    RegisterDragDrop(h, t);
    t->Release();  // (RegisterDragDrop keeps its own reference)
}

static ID2D1StrokeStyle* ShelfDash() {
    if (!g_dashStroke && g_d2d)
        g_d2d->CreateStrokeStyle(D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                             D2D1_LINE_JOIN_ROUND, 10, D2D1_DASH_STYLE_DASH),
                                 nullptr, 0, &g_dashStroke);
    return g_dashStroke.Get();
}

static void FBmp(const std::wstring& key, IWICBitmapSource* src, float cx, float cy, float size) {
    ID2D1Bitmap* bmp = FBitmap(key, src);
    if (!bmp) return;
    D2D1_RECT_F ir = D2D1::RectF(cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
    if (g_fly.dc) g_fly.dc->DrawBitmap(bmp, ir, 1, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
    else g_fly.rt->DrawBitmap(bmp, ir);
}

static void ShelfItemHit(D2D1_RECT_F r, const std::wstring& path) {
    FlyHit h;
    h.r = r;
    h.click = [path] { ShelfOpen(path); };
    h.button = [path](int b) {
        if (b == 1) ShelfMenu(path);
    };
    h.dragOut = [path](bool right) { ShelfDragOut(path, right); };
    g_fly.hits.push_back(std::move(h));
}

// The × on the item under the mouse.
static void ShelfRemoveButton(float cx, float cy, const std::wstring& path) {
    D2D1_RECT_F xr = D2D1::RectF(cx - 10, cy - 10, cx + 10, cy + 10);
    FFill(xr, 10, FIn(xr) ? D2D1::ColorF(0, 0, 0, 0.75f) : D2D1::ColorF(0, 0, 0, 0.45f));
    FIcon(L"M8 8l8 8 M16 8l-8 8", cx, cy, 13, FInk(), 1.8f);
    FHitAdd(xr, [path] {
        ShelfDropItem(ShelfFind(path));
        ShelfChanged();
    });
}

static float PanelShelf(float y) {
    // Right-click on the panel itself: paste, clear.
    {
        FlyHit h;
        h.r = D2D1::RectF(0, 0, kFlyW, 4000);
        h.button = [](int b) {
            if (b == 1) ShelfPanelMenu();
        };
        g_fly.hits.push_back(std::move(h));
    }
    std::wstring title = L2(L"Prateleira", L"Shelf");
    float tw = TextSize(title, g_fly.fTitle.Get()).width;
    FText(title, g_fly.fTitle.Get(), kFlyPad, y, tw + 4, 22, FInk());
    FText(std::to_wstring(g_shelf.size()), g_fly.fSmall.Get(), kFlyPad + tw + 12, y, 40, 22, FSub());
    float rx = kFlyW - kFlyPad + 6;
    auto link = [&](const wchar_t* label, std::function<void()> fn) {
        float w = TextSize(label, g_fly.fSmall.Get()).width + 16;
        D2D1_RECT_F r = D2D1::RectF(rx - w, y - 2, rx, y + 24);
        if (FHot(r)) FFill(r, 7, FHov());
        FText(label, g_fly.fSmall.Get(), r.left, r.top, w, 26, FHot(r) ? FInk() : FCol(g_theme.text, 0.8f), DWRITE_TEXT_ALIGNMENT_CENTER);
        FHitAdd(r, std::move(fn));
        rx -= w + 2;
    };
    if (!g_shelf.empty()) link(L2(L"Limpar", L"Clear"), [] { ShelfClear(); });
    link(L2(L"Colar", L"Paste"), [] { ShelfPaste(); });
    y += 22 + 12;
    if (!g_shelf.empty()) {
        if (g_set.shelfList) {
            for (auto& it : g_shelf) {
                D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y, kFlyW - kFlyPad + 8, y + 46);
                bool hot = FHot(r);
                if (hot) FFill(r, 10, FHov());
                FBmp(L"#shelf:" + it.path, it.icon.Get(), kFlyPad + 12, y + 23, 28);
                float w = kFlyW - kFlyPad * 2 - 36 - (hot ? 30 : 0);
                FText(it.name, g_fly.fText.Get(), kFlyPad + 34, y + 5, w, 0, FInk());
                FText(it.sub, g_fly.fSmall.Get(), kFlyPad + 34, y + 25, w, 0, FSub());
                ShelfItemHit(r, it.path);
                if (hot) ShelfRemoveButton(r.right - 22, y + 23, it.path);
                y += 48;
            }
            y += 6;
        } else {
            float cw = (kFlyW - kFlyPad * 2 + 12) / 3, ch = 98;
            for (size_t i = 0; i < g_shelf.size(); i++) {
                auto& it = g_shelf[i];
                float x = kFlyPad - 6 + (i % 3) * cw, top = y + (i / 3) * (ch + 4);
                D2D1_RECT_F r = D2D1::RectF(x + 2, top, x + cw - 2, top + ch);
                bool hot = FHot(r);
                if (hot) FFill(r, 12, FHov());
                FBmp(L"#shelf:" + it.path, it.icon.Get(), (r.left + r.right) / 2, top + 30, 40);
                FText(it.name, g_fly.fSmall.Get(), r.left + 6, top + 56, r.right - r.left - 12, 0, FInk(), DWRITE_TEXT_ALIGNMENT_CENTER);
                FText(it.sub, g_fly.fSmall.Get(), r.left + 6, top + 74, r.right - r.left - 12, 0, FSub(), DWRITE_TEXT_ALIGNMENT_CENTER);
                ShelfItemHit(r, it.path);
                if (hot) ShelfRemoveButton(r.right - 14, top + 14, it.path);
            }
            y += ((g_shelf.size() + 2) / 3) * (ch + 4) + 6;
        }
    }
    D2D1_RECT_F dz = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 50);
    bool over = g_shelfUi.over;
    if (over) FFill(dz, 12, FHov());
    if (FDraw())
        g_fly.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(dz.left + 0.75f, dz.top + 0.75f, dz.right - 0.75f, dz.bottom - 0.75f), 12, 12),
                                       FBr(FCol(g_theme.text, over ? 0.55f : 0.2f)), 1.5f, ShelfDash());
    FText(over ? L2(L"Solte pra guardar aqui", L"Drop to keep it here")
               : L2(L"Solte arquivos aqui ou cole com Ctrl+V", L"Drop files here or paste with Ctrl+V"),
          g_fly.fSmall.Get(), dz.left, dz.top, dz.right - dz.left, 50, over ? FInk() : FSub(), DWRITE_TEXT_ALIGNMENT_CENTER);
    y += 50;
    FSep(y);
    const wchar_t* mode = g_set.shelfDrag == L"mover" ? L2(L"mover", L"move")
                          : g_set.shelfDrag == L"windows" ? L2(L"como no Windows", L"like Windows")
                                                          : L2(L"copiar", L"copy");
    FText(std::wstring(L2(L"Ao arrastar pra uma pasta: ", L"Dragged to a folder: ")) + mode, g_fly.fSmall.Get(), kFlyPad, y, 260, 20, FSub());
    FText(std::wstring(L2(L"até ", L"up to ")) + std::to_wstring(g_set.shelfLimit), g_fly.fSmall.Get(), kFlyW - kFlyPad - 80, y, 80, 20, FSub(),
          DWRITE_TEXT_ALIGNMENT_TRAILING);
    y += 20;
    return y;
}

// ---- Notifications ----
// Windows keeps every notification in %LOCALAPPDATA%\Microsoft\Windows\Notifications\wpndatabase.db
// (SQLite). Nimbus only reads it (Windows' own winsqlite3.dll, opened read-only), every 2 s while the
// feature is on, and draws its own cards. Clearing one here only hides it in Nimbus.

struct WinSql {
    HMODULE mod = nullptr;
    void* db = nullptr;
    int(WINAPI* open)(const char*, void**, int, const char*) = nullptr;
    int(WINAPI* prepare)(void*, const char*, int, void**, const char**) = nullptr;
    int(WINAPI* step)(void*) = nullptr;
    long long(WINAPI* colInt64)(void*, int) = nullptr;
    const void*(WINAPI* colText16)(void*, int) = nullptr;
    const void*(WINAPI* colBlob)(void*, int) = nullptr;
    int(WINAPI* colBytes)(void*, int) = nullptr;
    int(WINAPI* bindInt64)(void*, int, long long) = nullptr;
    int(WINAPI* finalize)(void*) = nullptr;
    int(WINAPI* close)(void*) = nullptr;
    int(WINAPI* busy)(void*, int) = nullptr;
    bool failed = false;
} g_sql;

struct NotifApp {
    std::wstring name;
    ComPtr<IWICBitmapSource> icon;
};
std::unordered_map<std::wstring, NotifApp> g_notifApps;
std::vector<long long> g_notifCleared;  // hidden in Nimbus (saved)
long long g_notifLast = 0;              // the newest id seen
bool g_notifReady = false;
static void CfgSet(const std::wstring& name, const std::wstring& v);

static bool SqlOpen() {
    if (g_sql.db) return true;
    if (g_sql.failed) return false;
    g_sql.mod = LoadLibraryExW(L"winsqlite3.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto get = [](const char* n) { return g_sql.mod ? (void*)GetProcAddress(g_sql.mod, n) : nullptr; };
    g_sql.open = (decltype(g_sql.open))get("sqlite3_open_v2");
    g_sql.prepare = (decltype(g_sql.prepare))get("sqlite3_prepare_v2");
    g_sql.step = (decltype(g_sql.step))get("sqlite3_step");
    g_sql.colInt64 = (decltype(g_sql.colInt64))get("sqlite3_column_int64");
    g_sql.colText16 = (decltype(g_sql.colText16))get("sqlite3_column_text16");
    g_sql.colBlob = (decltype(g_sql.colBlob))get("sqlite3_column_blob");
    g_sql.colBytes = (decltype(g_sql.colBytes))get("sqlite3_column_bytes");
    g_sql.bindInt64 = (decltype(g_sql.bindInt64))get("sqlite3_bind_int64");
    g_sql.finalize = (decltype(g_sql.finalize))get("sqlite3_finalize");
    g_sql.close = (decltype(g_sql.close))get("sqlite3_close");
    g_sql.busy = (decltype(g_sql.busy))get("sqlite3_busy_timeout");
    if (!g_sql.open || !g_sql.prepare || !g_sql.step || !g_sql.colInt64 || !g_sql.colText16 || !g_sql.colBlob ||
        !g_sql.colBytes || !g_sql.bindInt64 || !g_sql.finalize || !g_sql.close) {
        g_sql.failed = true;
        Wh_Log(L"notifications: winsqlite3 not usable");
        return false;
    }
    wchar_t path[MAX_PATH];
    if (!ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Microsoft\\Windows\\Notifications\\wpndatabase.db", path, MAX_PATH)) return false;
    char u8[MAX_PATH * 3];
    WideCharToMultiByte(CP_UTF8, 0, path, -1, u8, sizeof(u8), nullptr, nullptr);
    if (g_sql.open(u8, &g_sql.db, 1 /* SQLITE_OPEN_READONLY */, nullptr) != 0) {
        if (g_sql.db) g_sql.close(g_sql.db);
        g_sql.db = nullptr;
        Wh_Log(L"notifications: can't open the database");
        return false;
    }
    if (g_sql.busy) g_sql.busy(g_sql.db, 200);
    return true;
}

static void SqlClose() {
    if (g_sql.db) g_sql.close(g_sql.db);
    g_sql.db = nullptr;
}

static std::wstring XmlDecode(std::wstring s) {
    std::wstring o;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] != L'&') {
            o += s[i];
            continue;
        }
        size_t e = s.find(L';', i);
        if (e == std::wstring::npos || e - i > 10) {
            o += s[i];
            continue;
        }
        std::wstring ent = s.substr(i + 1, e - i - 1);
        if (ent == L"amp") o += L'&';
        else if (ent == L"lt") o += L'<';
        else if (ent == L"gt") o += L'>';
        else if (ent == L"quot") o += L'"';
        else if (ent == L"apos") o += L'\'';
        else if (!ent.empty() && ent[0] == L'#') o += (wchar_t)(ent.size() > 1 && (ent[1] == L'x' || ent[1] == L'X') ? wcstoul(ent.c_str() + 2, nullptr, 16) : wcstoul(ent.c_str() + 1, nullptr, 10));
        else o += L'&' + ent + L';';
        i = e;
    }
    return o;
}

static std::wstring XmlAttr(const std::wstring& tag, const wchar_t* name) {
    std::wstring key = std::wstring(L" ") + name + L"=";
    size_t a = tag.find(key);
    if (a == std::wstring::npos) return L"";
    a += key.size();
    if (a >= tag.size()) return L"";
    wchar_t q = tag[a];
    if (q != L'"' && q != L'\'') return L"";
    size_t b = tag.find(q, a + 1);
    return b == std::wstring::npos ? L"" : XmlDecode(tag.substr(a + 1, b - a - 1));
}

// The toast's texts (first one is the title) and buttons.
static void ParseToast(const std::wstring& x, NotifItem& n) {
    n.silent = x.find(L"silent=\"true\"") != std::wstring::npos;
    size_t t = x.find(L"<toast");
    if (t != std::wstring::npos) {
        std::wstring tag = x.substr(t, x.find(L'>', t) - t);
        n.launch = XmlAttr(tag, L"launch");
        n.launchType = XmlAttr(tag, L"activationType");
        std::wstring sc = XmlAttr(tag, L"scenario");
        n.sticky = sc == L"alarm" || sc == L"reminder" || sc == L"incomingCall" || sc == L"urgent";
    }
    size_t b = x.find(L"<binding"), be = x.find(L"</binding>", b);
    std::vector<std::wstring> texts;
    for (size_t p = b; b != std::wstring::npos && (p = x.find(L"<text", p)) != std::wstring::npos && p < be;) {
        size_t te = x.find(L'>', p);
        if (te == std::wstring::npos) break;
        if (x[te - 1] == L'/') {
            p = te;
            continue;
        }
        size_t ce = x.find(L"</text>", te);
        if (ce == std::wstring::npos) break;
        std::wstring raw = x.substr(te + 1, ce - te - 1);
        // <![CDATA[...]]> is text as it is (no entities).
        std::wstring v = raw.rfind(L"<![CDATA[", 0) == 0 && raw.size() >= 12 && raw.compare(raw.size() - 3, 3, L"]]>") == 0
                             ? raw.substr(9, raw.size() - 12)
                             : XmlDecode(raw);
        if (!v.empty()) texts.push_back(v);
        p = ce;
    }
    if (!texts.empty()) n.title = texts[0];
    for (size_t i = 1; i < texts.size(); i++) n.body += (n.body.empty() ? L"" : L"\n") + texts[i];
    for (size_t p = x.find(L"<actions"); p != std::wstring::npos && (p = x.find(L"<action ", p)) != std::wstring::npos;) {
        size_t te = x.find(L'>', p);
        if (te == std::wstring::npos) break;
        std::wstring tag = x.substr(p, te - p);
        if (XmlAttr(tag, L"placement") != L"contextMenu" && n.actions.size() < 2) {
            NotifAction a;
            a.label = XmlAttr(tag, L"content");
            a.args = XmlAttr(tag, L"arguments");
            a.type = XmlAttr(tag, L"activationType");
            if (!a.label.empty()) n.actions.push_back(a);
        }
        p = te;
    }
}

static const NotifApp& NotifAppOf(const std::wstring& aumid) {
    auto it = g_notifApps.find(aumid);
    if (it != g_notifApps.end()) return it->second;
    NotifApp a;
    if (aumid == L"#nimbus") {  // Nimbus' own (test, questions, reminders)
        a.name = L"Nimbus Menu";
        if (HBITMAP hb = DrawNimbusIcon(64)) {
            a.icon = WicFromHBitmap(hb);
            DeleteObject(hb);
        }
        return g_notifApps[aumid] = a;
    }
    ComPtr<IShellItem> si;
    if (SUCCEEDED(SHCreateItemFromParsingName((L"shell:AppsFolder\\" + aumid).c_str(), nullptr, IID_PPV_ARGS(&si)))) {
        PWSTR name = nullptr;
        if (SUCCEEDED(si->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name) {
            a.name = name;
            CoTaskMemFree(name);
        }
        a.icon = IconFromItem(si.Get(), 64);
    }
    if (a.name.empty()) {
        // "{...}\Folder\App.exe" or "Company.App": the last word.
        size_t c = aumid.find_last_of(L"\\.!");
        a.name = c == std::wstring::npos ? aumid : aumid.substr(c + 1);
        if (a.name == L"exe" && c != std::wstring::npos) {
            std::wstring rest = aumid.substr(0, c);
            size_t c2 = rest.find_last_of(L"\\");
            a.name = c2 == std::wstring::npos ? rest : rest.substr(c2 + 1);
        }
    }
    return g_notifApps[aumid] = a;
}

static bool NotifIsCleared(long long id) { return std::find(g_notifCleared.begin(), g_notifCleared.end(), id) != g_notifCleared.end(); }

static void SaveNotifCleared() {
    while (g_notifCleared.size() > 300) g_notifCleared.erase(g_notifCleared.begin());
    std::wstring all;
    for (long long id : g_notifCleared) all += (all.empty() ? L"" : L",") + std::to_wstring(id);
    Wh_SetStringValue((UserPrefix() + L"notif.cleared").c_str(), all.c_str());
}

static void LoadNotifCleared() {
    g_notifCleared.clear();
    std::vector<wchar_t> buf(8192);
    if (!Wh_GetStringValue((UserPrefix() + L"notif.cleared").c_str(), buf.data(), buf.size())) return;
    for (wchar_t* p = buf.data(); *p;) {
        wchar_t* e;
        long long v = wcstoll(p, &e, 10);
        if (e == p) break;
        g_notifCleared.push_back(v);
        p = *e == L',' ? e + 1 : e;
    }
}

static void NotifClear(long long id) {
    if (!NotifIsCleared(id)) g_notifCleared.push_back(id);
    g_notifs.erase(std::remove_if(g_notifs.begin(), g_notifs.end(), [&](const NotifItem& n) { return n.id == id; }), g_notifs.end());
    SaveNotifCleared();
}

static void NotifShowCard(const NotifItem& n, bool force = false);
static void NotifAskBanners();
static void NotifMuteNewApp(const std::wstring& aumid);

// New rows since the last look (and, every 10 s, the ones Windows removed).
static bool NotifPoll(bool first) {
    if (!SqlOpen()) return false;
    void* st = nullptr;
    const char* q =
        "SELECT n.Id, h.PrimaryId, n.Payload, n.ArrivalTime FROM Notification n JOIN NotificationHandler h ON h.RecordId = "
        "n.HandlerId WHERE n.Type = 'toast' AND n.Id > ?1 ORDER BY n.Id";
    if (g_sql.prepare(g_sql.db, q, -1, &st, nullptr) != 0 || !st) {
        SqlClose();  // (reopened next time)
        return false;
    }
    g_sql.bindInt64(st, 1, g_notifLast);
    bool changed = false;
    while (g_sql.step(st) == 100 /* SQLITE_ROW */) {
        NotifItem n;
        n.id = g_sql.colInt64(st, 0);
        g_notifLast = std::max(g_notifLast, n.id);
        if (NotifIsCleared(n.id)) continue;
        if (auto a = (const wchar_t*)g_sql.colText16(st, 1)) n.aumid = a;
        const char* blob = (const char*)g_sql.colBlob(st, 2);
        int bytes = g_sql.colBytes(st, 2);
        if (blob && bytes > 0) {
            int wn = MultiByteToWideChar(CP_UTF8, 0, blob, bytes, nullptr, 0);
            std::wstring x(wn, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, blob, bytes, x.data(), wn);
            ParseToast(x, n);
        }
        n.arrival = g_sql.colInt64(st, 3);
        if (n.title.empty() && n.body.empty()) continue;
        g_notifs.insert(g_notifs.begin(), n);
        if (!first) {
            NotifMuteNewApp(n.aumid);
            NotifShowCard(n);
            NotifAskBanners();
        }
        changed = true;
    }
    g_sql.finalize(st);
    while (g_notifs.size() > 60) g_notifs.pop_back();
    if (changed)
        for (auto& n : g_notifs) NotifAppOf(n.aumid);
    static DWORD lastSweep = 0;
    if (first || GetTickCount() - lastSweep > 10000) {
        lastSweep = GetTickCount();
        std::unordered_set<long long> alive;
        if (g_sql.prepare(g_sql.db, "SELECT Id FROM Notification WHERE Type = 'toast'", -1, &st, nullptr) == 0 && st) {
            while (g_sql.step(st) == 100) alive.insert(g_sql.colInt64(st, 0));
            g_sql.finalize(st);
            size_t before = g_notifs.size();
            g_notifs.erase(std::remove_if(g_notifs.begin(), g_notifs.end(), [&](const NotifItem& n) { return n.id > 0 && !alive.count(n.id); }),
                           g_notifs.end());
            if (g_notifs.size() != before) changed = true;
        }
    }
    if (changed) StatusChanged();
    return true;
}

// ---- Windows' own banners (only with the user's ok) ----
// Each app's "Show notification banners" switch in Windows' Settings is the ShowBanner value under
// HKCU\...\Notifications\Settings\<app>. Nimbus turns it off for every app (notifications still go to
// Windows' center), notes what each one was, and puts it all back when the option goes off or Nimbus
// stops.

static const wchar_t kNotifSettingsKey[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\Settings";
std::vector<std::pair<std::wstring, std::wstring>> g_banners;  // app, what ShowBanner was ("-" = no value)
bool g_bannersLoaded = false;

static bool SaveBanners() {
    std::vector<std::wstring> lines;
    for (auto& b : g_banners) lines.push_back(b.second + L"|" + b.first);
    return StoreItems(L"notif.banners", lines);
}

static void LoadBanners() {
    if (g_bannersLoaded) return;
    g_bannersLoaded = true;
    g_banners.clear();
    std::vector<std::wstring> lines;
    LoadItems(L"notif.banners", L'\n', lines);
    for (auto& line : lines) {
        size_t bar = line.find(L'|');
        if (bar != std::wstring::npos) g_banners.push_back({line.substr(bar + 1), line.substr(0, bar)});
    }
}

// What an app's ShowBanner is now: "-" when there's no value, "" when it's off already (the user's own
// choice: left alone) or the app has no settings key.
static std::wstring BannerNow(const std::wstring& app) {
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, (std::wstring(kNotifSettingsKey) + L"\\" + app).c_str(), 0, KEY_READ, &k) != ERROR_SUCCESS) return L"";
    DWORD v = 0, n = sizeof(v), type = 0;
    bool has = RegQueryValueExW(k, L"ShowBanner", nullptr, &type, (BYTE*)&v, &n) == ERROR_SUCCESS && type == REG_DWORD;
    RegCloseKey(k);
    if (has && v == 0) return L"";
    return has ? std::to_wstring(v) : L"-";
}
static void BannerOff(const std::wstring& app) {
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, (std::wstring(kNotifSettingsKey) + L"\\" + app).c_str(), 0, KEY_SET_VALUE, &k) != ERROR_SUCCESS) return;
    DWORD zero = 0;
    RegSetValueExW(k, L"ShowBanner", 0, REG_DWORD, (const BYTE*)&zero, sizeof(zero));
    RegCloseKey(k);
}

// Banners off for these apps: each one's value is written to the backup first; if the backup can't be
// saved, nothing is switched off.
static void MuteBannersOf(const std::vector<std::wstring>& apps) {
    size_t before = g_banners.size();
    for (auto& app : apps) {
        if (app.empty() || app == L"#nimbus") continue;
        if (std::any_of(g_banners.begin(), g_banners.end(), [&](auto& b) { return b.first == app; })) continue;
        std::wstring was = BannerNow(app);
        if (!was.empty()) g_banners.push_back({app, was});
    }
    if (g_banners.size() == before) return;
    if (!SaveBanners()) {
        g_banners.resize(before);
        Wh_Log(L"notifications: the backup couldn't be saved, Windows' banners left on");
        return;
    }
    for (size_t i = before; i < g_banners.size(); i++) BannerOff(g_banners[i].first);
}
static void MuteBanner(const std::wstring& app) { MuteBannersOf({app}); }

static void MuteBanners() {
    LoadBanners();
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kNotifSettingsKey, 0, KEY_READ, &k) != ERROR_SUCCESS) return;
    std::vector<std::wstring> apps;
    wchar_t sub[512];
    for (DWORD i = 0;; i++) {
        DWORD n = ARRAYSIZE(sub);
        if (RegEnumKeyExW(k, i, sub, &n, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
        apps.push_back(sub);
    }
    RegCloseKey(k);
    MuteBannersOf(apps);
    Wh_Log(L"notifications: Windows' banners off for %d apps", (int)g_banners.size());
}

static void RestoreBanners() {
    LoadBanners();
    for (auto& b : g_banners) {
        HKEY k;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, (std::wstring(kNotifSettingsKey) + L"\\" + b.first).c_str(), 0, KEY_SET_VALUE, &k) !=
            ERROR_SUCCESS)
            continue;
        if (b.second == L"-") {
            RegDeleteValueW(k, L"ShowBanner");
        } else {
            DWORD v = (DWORD)wcstoul(b.second.c_str(), nullptr, 10);
            RegSetValueExW(k, L"ShowBanner", 0, REG_DWORD, (const BYTE*)&v, sizeof(v));
        }
        RegCloseKey(k);
    }
    if (!g_banners.empty()) Wh_Log(L"notifications: Windows' banners back for %d apps", (int)g_banners.size());
    g_banners.clear();
    SaveBanners();
}

// The option and what's applied agree (at start, after a settings change).
static void SyncBanners() {
    LoadBanners();
    bool want = g_set.notifHideWin && g_set.notifShow;
    if (want) MuteBanners();
    else if (!g_banners.empty()) RestoreBanners();
}

static void NotifMuteNewApp(const std::wstring& aumid) {
    if (!g_set.notifHideWin || !g_set.notifShow) return;
    MuteBanner(aumid);
}

static long long g_notifSynth = 0;

// A notification from Nimbus itself (it goes to the list too).
static void NotifPush(const std::wstring& title, const std::wstring& body, std::vector<NotifAction> actions, bool force,
                      bool reminder = false) {
    NotifItem n;
    n.reminder = reminder;
    n.id = -(++g_notifSynth);
    n.aumid = L"#nimbus";
    n.title = title;
    n.body = body;
    n.actions = std::move(actions);
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    n.arrival = ((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    g_notifs.insert(g_notifs.begin(), n);
    NotifShowCard(g_notifs.front(), force);
    StatusChanged();
}

static void NotifTest() {
    NotifPush(L2(L"Teste do Nimbus", L"Nimbus test"),
              L2(L"É assim que os avisos vão aparecer. Passe o mouse em cima pra ele esperar.",
                 L"This is how notifications will look. Rest the mouse on it to make it wait."),
              {{L"Ok", L"ok", L"nimbus"}}, true);
}

// Once per run, with the first card: may Nimbus hide Windows' own banners (so there aren't two)?
static void NotifAskBanners() {
    static bool askedThisRun = false;
    if (askedThisRun || g_set.notifHideWin || !g_set.notifShow) return;
    wchar_t v[8] = {};
    if (Wh_GetStringValue((UserPrefix() + L"notif.askedBanners").c_str(), v, ARRAYSIZE(v)) && v[0] == L'1') return;
    askedThisRun = true;
    NotifPush(L2(L"Esconder os avisos do Windows?", L"Hide Windows' own banners?"),
              L2(L"Pra não aparecerem dois. O Nimbus desliga o balão de cada app no Windows; os avisos continuam na central e voltam ao normal quando você quiser.",
                 L"So there aren't two. Nimbus turns off each app's banner in Windows; notifications stay in its center, and it all comes back whenever you want."),
              {{L2(L"Esconder", L"Hide them"), L"banners-yes", L"nimbus"}, {L2(L"Agora não", L"Not now"), L"banners-no", L"nimbus"}}, false);
}

static void RemAction(const std::wstring& args);
static void NotifNimbusAction(const std::wstring& args) {
    if (!args.compare(0, 4, L"rem-")) {
        RemAction(args);
        return;
    }
    if (args == L"banners-yes") {
        Wh_SetStringValue((UserPrefix() + L"notif.askedBanners").c_str(), L"1");
        g_set.notifHideWin = true;
        CfgSet(L"notifHideWin", L"1");
        SyncBanners();
    } else if (args == L"banners-no") {
        Wh_SetStringValue((UserPrefix() + L"notif.askedBanners").c_str(), L"1");
    }
}

// ---- Clicks: the app's own way of opening a notification ----

struct INotifActivationCallback : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Activate(LPCWSTR appUserModelId, LPCWSTR invokedArgs, const void* data, ULONG count) = 0;
};
static const IID kIID_NotifActivation = {0x53E31837, 0x6600, 0x4A81, {0x93, 0x95, 0x75, 0xCF, 0xFE, 0x74, 0x6F, 0x94}};

// The COM class an app registered for its notifications: in its AppUserModelId key, or on its Start
// shortcut (System.AppUserModel.ToastActivatorCLSID).
static bool NotifActivator(const std::wstring& aumid, CLSID& out) {
    wchar_t s[64] = {};
    DWORD n = sizeof(s);
    if (RegGetValueW(HKEY_CURRENT_USER, (L"Software\\Classes\\AppUserModelId\\" + aumid).c_str(), L"CustomActivator", RRF_RT_REG_SZ,
                     nullptr, s, &n) == ERROR_SUCCESS &&
        SUCCEEDED(CLSIDFromString(s, &out)))
        return true;
    static const PROPERTYKEY kToastActivator = {{0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 26};
    ComPtr<IShellItem2> si;
    if (SUCCEEDED(SHCreateItemFromParsingName((L"shell:AppsFolder\\" + aumid).c_str(), nullptr, IID_PPV_ARGS(&si))) &&
        SUCCEEDED(si->GetCLSID(kToastActivator, &out)) && out != GUID_NULL)
        return true;
    return false;
}

// action: -1 the notification itself, else its button.
static void NotifActivate(const NotifItem& n, int action) {
    std::wstring args = action >= 0 && action < (int)n.actions.size() ? n.actions[action].args : n.launch;
    std::wstring type = action >= 0 && action < (int)n.actions.size() ? n.actions[action].type : n.launchType;
    if (n.aumid == L"#nimbus") {
        NotifNimbusAction(args);
        return;
    }
    if (type == L"system") return;  // (snooze, dismiss: the card is gone already)
    if (type == L"protocol") {
        // Only a "scheme:" URI, as Windows does: never a path, a UNC share or a file: URL.
        PARSEDURLW pu = {sizeof(pu)};
        bool ok = !args.empty() && SUCCEEDED(ParseURLW(args.c_str(), &pu)) && pu.cchProtocol > 1 && pu.nScheme != URL_SCHEME_FILE &&
                  pu.nScheme != URL_SCHEME_SHELL && pu.nScheme != URL_SCHEME_MSSHELLROOTED && pu.nScheme != URL_SCHEME_MSSHELLIDLIST &&
                  pu.nScheme != URL_SCHEME_KNOWNFOLDER;  // (shell:, knownfolder: and the like name files: not protocols)
        if (!ok) {
            Wh_Log(L"notifications: refused a protocol launch that isn't a URI");
            return;
        }
        AllowSetForegroundWindow(ASFW_ANY);
        ShellExecuteW(nullptr, L"open", args.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    AllowSetForegroundWindow(ASFW_ANY);
    CLSID clsid;
    if (NotifActivator(n.aumid, clsid)) {
        ComPtr<INotifActivationCallback> cb;
        HRESULT hr = CoCreateInstance(clsid, nullptr, CLSCTX_LOCAL_SERVER, kIID_NotifActivation, (void**)cb.GetAddressOf());
        if (SUCCEEDED(hr) && cb) hr = cb->Activate(n.aumid.c_str(), args.c_str(), nullptr, 0);
        Wh_Log(L"notifications: activator for %s hr=0x%08lx", n.aumid.c_str(), (unsigned long)hr);
        if (SUCCEEDED(hr)) return;
    }
    if (type == L"background") return;  // (meant for the app's background task, never a window)
    if (n.aumid.find(L'!') != std::wstring::npos) {
        ComPtr<IApplicationActivationManager> am;
        DWORD pid = 0;
        if (SUCCEEDED(CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&am))) &&
            SUCCEEDED(am->ActivateApplication(n.aumid.c_str(), args.c_str(), AO_NONE, &pid)))
            return;
    }
    ShellExecuteW(nullptr, L"open", (L"shell:AppsFolder\\" + n.aumid).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void NotifOpen(long long id, int action) {
    auto it = std::find_if(g_notifs.begin(), g_notifs.end(), [&](const NotifItem& n) { return n.id == id; });
    if (it == g_notifs.end()) return;
    NotifItem n = *it;
    NotifClear(id);
    StatusChanged();
    NotifActivate(n, action);
}

static void NotifInit() {
    SyncBanners();
    if (g_set.notifPlace == 0 && !g_set.notifShow) return;
    LoadNotifCleared();
    // (only once a poll worked: otherwise the next one would show every stored notification as new)
    g_notifReady = NotifPoll(true);
}

static void NotifTick() {
    static DWORD last = 0;
    if (g_set.notifPlace == 0 && !g_set.notifShow) {
        if (g_sql.db) SqlClose();
        g_notifReady = false;  // (turned on again later: what came meanwhile isn't shown as new)
        return;
    }
    if (GetTickCount() - last < 2000) return;
    last = GetTickCount();
    if (!g_notifReady) NotifInit();
    else NotifPoll(false);
}

// ---- The panel (the bell) ----

static std::wstring NotifTime(long long ft) {
    FILETIME f = {(DWORD)ft, (DWORD)(ft >> 32)}, lf;
    SYSTEMTIME s, now;
    if (!FileTimeToLocalFileTime(&f, &lf) || !FileTimeToSystemTime(&lf, &s)) return L"";
    GetLocalTime(&now);
    wchar_t b[64] = {};
    if (s.wYear == now.wYear && s.wMonth == now.wMonth && s.wDay == now.wDay)
        GetTimeFormatEx(g_locale, TIME_NOSECONDS, &s, nullptr, b, ARRAYSIZE(b));
    else
        GetDateFormatEx(g_locale, 0, &s, L"dd/MM", b, ARRAYSIZE(b), nullptr);
    return b;
}

// Line breaks (any kind) and tabs as spaces, runs of spaces as one.
static std::wstring OneLine(const std::wstring& s) {
    std::wstring o;
    for (wchar_t c : s) {
        if (c == L'\r' || c == L'\n' || c == L'\t' || c == 0x2028 || c == 0x2029) c = L' ';
        if (c == L' ' && (o.empty() || o.back() == L' ')) continue;
        o += c;
    }
    while (!o.empty() && o.back() == L' ') o.pop_back();
    return o;
}

static float PanelNotif(float y) {
    std::wstring title = L2(L"Notificações", L"Notifications");
    float tw = TextSize(title, g_fly.fTitle.Get()).width;
    FText(title, g_fly.fTitle.Get(), kFlyPad, y, tw + 4, 22, FInk());
    FText(std::to_wstring(g_notifs.size()), g_fly.fSmall.Get(), kFlyPad + tw + 12, y, 40, 22, FSub());
    if (!g_notifs.empty()) {
        const wchar_t* label = L2(L"Limpar tudo", L"Clear all");
        float w = TextSize(label, g_fly.fSmall.Get()).width + 16;
        D2D1_RECT_F r = D2D1::RectF(kFlyW - kFlyPad + 6 - w, y - 2, kFlyW - kFlyPad + 6, y + 24);
        if (FHot(r)) FFill(r, 7, FHov());
        FText(label, g_fly.fSmall.Get(), r.left, r.top, w, 26, FHot(r) ? FInk() : FCol(g_theme.text, 0.8f), DWRITE_TEXT_ALIGNMENT_CENTER);
        FHitAdd(r, [] {
            for (auto& n : std::vector<NotifItem>(g_notifs)) NotifClear(n.id);
            StatusChanged();
        });
    }
    y += 22 + 12;
    if (g_notifs.empty()) {
        FText(L2(L"Nada por aqui.", L"Nothing here."), g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 40, FSub(),
              DWRITE_TEXT_ALIGNMENT_CENTER);
        y += 40;
    }
    // By time, newest first, under the day it came. The list scrolls inside the panel (the wheel).
    float view = std::clamp(g_fly.maxH - y - 70, 140.0f, 480.0f);
    float listTop = y;
    D2D1_RECT_F viewR = D2D1::RectF(0, listTop, kFlyW, listTop + view);
    auto inView = [&](D2D1_RECT_F r) { return r.bottom > viewR.top && r.top < viewR.bottom; };
    auto clipHit = [&](D2D1_RECT_F r, std::function<void()> fn) {
        if (!inView(r)) return;
        r.top = std::max(r.top, viewR.top);
        r.bottom = std::min(r.bottom, viewR.bottom);
        FHitAdd(r, std::move(fn));
    };
    bool mouseIn = FIn(viewR);
    long long hoverNow = 0;
    y = listTop - g_fly.listScroll;
    if (FDraw()) g_fly.rt->PushAxisAlignedClip(viewR, D2D1_ANTIALIAS_MODE_ALIASED);
    int shown = 0;
    std::wstring lastDay;
    for (auto& n : g_notifs) {
        if (shown >= 60) break;
        shown++;
        FILETIME f = {(DWORD)n.arrival, (DWORD)(n.arrival >> 32)}, lf;
        SYSTEMTIME st = {}, now;
        GetLocalTime(&now);
        std::wstring day;
        if (FileTimeToLocalFileTime(&f, &lf) && FileTimeToSystemTime(&lf, &st)) {
            FILETIME nf, df;
            SystemTimeToFileTime(&now, &nf);
            SYSTEMTIME a = st, b = now;
            a.wHour = a.wMinute = a.wSecond = a.wMilliseconds = 0;
            b.wHour = b.wMinute = b.wSecond = b.wMilliseconds = 0;
            SystemTimeToFileTime(&a, &df);
            SystemTimeToFileTime(&b, &nf);
            long long days = ((((long long)nf.dwHighDateTime << 32) | nf.dwLowDateTime) - (((long long)df.dwHighDateTime << 32) | df.dwLowDateTime)) /
                              864000000000LL;
            if (days <= 0) day = L2(L"Hoje", L"Today");
            else if (days == 1) day = L2(L"Ontem", L"Yesterday");
            else {
                wchar_t b2[64] = {};
                GetDateFormatEx(g_locale, DATE_LONGDATE, &st, nullptr, b2, ARRAYSIZE(b2), nullptr);
                day = b2;
            }
        }
        if (day != lastDay) {
            lastDay = day;
            FText(day, g_fly.fSmall.Get(), kFlyPad, y, 300, 24, FSub());
            y += 26;
        }
        const NotifApp& app = NotifAppOf(n.aumid);
        // Under the mouse, the whole text (up to about 8 lines); otherwise one line each.
        bool open = n.id == g_fly.notifOpen;
        std::wstring fullBody;
        for (wchar_t c : n.body)
            if (c != L'\r') fullBody += c;
        float bodyH = 18;
        if (open && g_fly.fWrapS) {
            ComPtr<IDWriteTextLayout> l;
            DWRITE_TEXT_METRICS m = {};
            if (SUCCEEDED(g_dw->CreateTextLayout(fullBody.c_str(), (UINT32)fullBody.size(), g_fly.fWrapS.Get(), kFlyW - kFlyPad * 2 - 12, 140, &l)))
                l->GetMetrics(&m);
            bodyH = std::clamp(m.height, 18.0f, 140.0f);
        }
        D2D1_RECT_F r = D2D1::RectF(kFlyPad - 6, y, kFlyW - kFlyPad + 6, y + 54 + bodyH);
        bool hot = mouseIn && FHot(r);
        if (FDraw() && hot) hoverNow = n.id;
        FFill(r, 12, hot ? FHov() : FCol(g_theme.hover, 0.45f));
        FBmp(L"#notif:" + n.aumid, app.icon.Get(), r.left + 21, y + 19, 18);
        std::wstring time = NotifTime(n.arrival);
        float tmw = TextSize(time, g_fly.fSmall.Get()).width;
        FText(app.name, g_fly.fSmall.Get(), r.left + 36, y + 9, r.right - r.left - 60 - tmw, 20, FSub());
        if (!hot) FText(time, g_fly.fSmall.Get(), r.right - 12 - tmw, y + 9, tmw + 2, 20, FSub());
        FText(OneLine(n.title), g_fly.fText.Get(), r.left + 12, y + 31, r.right - r.left - 24, 0, FInk());
        if (open && g_fly.fWrapS) {
            ComPtr<IDWriteTextLayout> l;
            if (FDraw() &&
                SUCCEEDED(g_dw->CreateTextLayout(fullBody.c_str(), (UINT32)fullBody.size(), g_fly.fWrapS.Get(), r.right - r.left - 24, bodyH, &l))) {
                g_fly.rt->PushAxisAlignedClip(D2D1::RectF(r.left + 12, y + 50, r.right - 12, y + 50 + bodyH), D2D1_ANTIALIAS_MODE_ALIASED);
                g_fly.rt->DrawTextLayout(D2D1::Point2F(r.left + 12, y + 50), l.Get(), FBr(FSub()));
                g_fly.rt->PopAxisAlignedClip();
            }
        } else {
            FText(OneLine(n.body), g_fly.fSmall.Get(), r.left + 12, y + 50, r.right - r.left - 24, 0, FSub());
        }
        long long id = n.id;
        clipHit(r, [id] { NotifOpen(id, -1); });
        if (hot) {
            D2D1_RECT_F xr = D2D1::RectF(r.right - 30, y + 6, r.right - 8, y + 28);
            if (FIn(xr)) FFill(xr, 11, FHov());
            FIcon(L"M8 8l8 8 M16 8l-8 8", (xr.left + xr.right) / 2, (xr.top + xr.bottom) / 2, 12, FInk(), 1.8f);
            clipHit(xr, [id] {
                NotifClear(id);
                StatusChanged();
            });
        }
        y += r.bottom - r.top + 4;
    }
    if (FDraw()) g_fly.rt->PopAxisAlignedClip();
    if (FDraw()) g_fly.notifOpen = hoverNow;
    float content = y + g_fly.listScroll - listTop;
    g_fly.listMax = std::max(0.0f, content - view);
    g_fly.listScroll = std::clamp(g_fly.listScroll, 0.0f, g_fly.listMax);
    y = listTop + std::min(view, content);
    if (g_fly.listMax > 0 && FDraw()) {
        // A thin bar: where the list is.
        float bh = std::max(30.0f, view * view / content), by = listTop + (view - bh) * (g_fly.listScroll / g_fly.listMax);
        g_fly.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(kFlyW - 6, by, kFlyW - 3, by + bh), 1.5f, 1.5f), FBr(FCol(g_theme.text, 0.25f)));
    }
    FSep(y);
    FText(L2(L"Mostrar os avisos do Nimbus", L"Show Nimbus' notifications"), g_fly.fSmall.Get(), kFlyPad, y, 260, 24, FSub());
    D2D1_RECT_F sw = D2D1::RectF(kFlyW - kFlyPad - 40, y + 1, kFlyW - kFlyPad, y + 23);
    bool on = g_set.notifShow;
    FFill(sw, 11, on ? FInk() : FCol(g_theme.text, 0.18f));
    if (FDraw())
        g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(on ? sw.right - 11 : sw.left + 11, (sw.top + sw.bottom) / 2), 8, 8),
                              FBr(on ? FSolidTint() : FInk()));
    FHitAdd(sw, [] {
        g_set.notifShow = !g_set.notifShow;
        CfgSet(L"notifShow", g_set.notifShow ? L"1" : L"0");
        StatusChanged();
    });
    y += 26;
    return y;
}

// ---- The cards ----

constexpr UINT_PTR TIMER_TOAST = 1;
constexpr float kToastW = 360, kToastM = 22;  // card width and the window's margin around the stack (for the shadow)

struct ToastCard {
    long long id = 0;
    float y = 0, op = 0, h = 0;
    bool leaving = false, hover = false, placed = false;
    DWORD total = 6000, left = 6000;
};

struct ToastUi {
    HWND hwnd = nullptr;
    ComPtr<ID2D1DCRenderTarget> rt;
    ComPtr<ID2D1DeviceContext> dc;
    ComPtr<ID2D1SolidColorBrush> br;
    ComPtr<IDWriteTextFormat> fApp, fTitle, fBody, fBtn;
    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> bmps;
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    int dibW = 0, dibH = 0;
    float scale = 1;
    RECT rc{};
    std::vector<ToastCard> cards;
    std::vector<std::pair<D2D1_RECT_F, std::function<void()>>> hits;
    D2D1_POINT_2F mouse{-1, -1};
    DWORD lastTick = 0;
} g_toast;

static const NotifItem* NotifById(long long id) {
    for (auto& n : g_notifs)
        if (n.id == id) return &n;
    return nullptr;
}

static Dock* ToastDock() {
    HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    for (auto* d : g_docks)
        if (d->mon == primary) return d;
    return g_docks.empty() ? nullptr : g_docks[0];
}

static void ToastFonts() {
    auto& t = g_toast;
    if (t.fApp) return;
    auto make = [](DWRITE_FONT_WEIGHT wt, float size, bool wrap, ComPtr<IDWriteTextFormat>& out) {
        g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, wt, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, g_locale, &out);
        if (!out) return;
        out->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> dots;
        g_dw->CreateEllipsisTrimmingSign(out.Get(), &dots);
        out->SetTrimming(&trim, dots.Get());
    };
    make(DWRITE_FONT_WEIGHT_NORMAL, 12.5f, false, t.fApp);
    make(DWRITE_FONT_WEIGHT_SEMI_BOLD, 14, true, t.fTitle);
    make(DWRITE_FONT_WEIGHT_NORMAL, 13, true, t.fBody);
    make(DWRITE_FONT_WEIGHT_SEMI_BOLD, 13, false, t.fBtn);
    if (t.fBtn) t.fBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
}

static float ToastTextH(const std::wstring& s, IDWriteTextFormat* f, float w, float maxH) {
    ComPtr<IDWriteTextLayout> l;
    DWRITE_TEXT_METRICS m = {};
    if (s.empty() || !f || FAILED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f, w, maxH, &l))) return 0;
    l->GetMetrics(&m);
    return std::min(m.height, maxH);
}

static float ToastHeight(const NotifItem& n) {
    float w = kToastW - 28;
    float h = 14 + 24 + 8 + ToastTextH(OneLine(n.title), g_toast.fTitle.Get(), w, 38);
    if (!n.body.empty()) h += 3 + ToastTextH(n.body, g_toast.fBody.Get(), w, 56);
    if (!n.actions.empty()) h += 12 + 34;
    return h + 14;
}

static void ToastDismiss(long long id) {
    for (auto& c : g_toast.cards)
        if (c.id == id) c.leaving = true;
}

static void ToastRender() {
    auto& t = g_toast;
    Dock* d = ToastDock();
    if (!t.hwnd || !d) return;
    t.scale = d->scale;
    if (!t.rt) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
        g_d2d->CreateDCRenderTarget(&props, &t.rt);
        if (!t.rt) return;
        t.rt.As(&t.dc);
        t.rt->CreateSolidColorBrush(D2D1::ColorF(0, 0), &t.br);
        t.bmps.clear();
    }
    ToastFonts();
    const float W = kToastW + kToastM * 2, H = 660;
    int pw = (int)std::ceil(W * t.scale), ph = (int)std::ceil(H * t.scale);
    if (pw != t.dibW || ph != t.dibH || !t.memDC) {
        if (t.memDC) {
            SelectObject(t.memDC, t.oldBmp);
            DeleteDC(t.memDC);
            DeleteObject(t.dib);
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = pw;
        bi.bmiHeader.biHeight = -ph;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        t.memDC = CreateCompatibleDC(nullptr);
        t.dib = CreateDIBSection(t.memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        t.oldBmp = SelectObject(t.memDC, t.dib);
        t.dibW = pw;
        t.dibH = ph;
    }
    // Where: the chosen corner (or middle) of the main monitor; at the bottom, above the dock.
    const RECT& m = d->rcMon;
    int x = g_set.notifH == 0 ? m.left + (int)(8 * t.scale)
            : g_set.notifH == 1 ? (m.left + m.right - pw) / 2
                                : m.right - pw - (int)(8 * t.scale);
    int dockRoom = d->ch - (int)d->pill.top;
    int y = g_set.notifTop ? m.top + (int)(4 * t.scale) : m.bottom - dockRoom - ph + (int)(kToastM * t.scale) - (int)(4 * t.scale);
    t.rc = {x, y, x + pw, y + ph};

    // Layout: newest nearest the edge.
    float acc = 0;
    for (auto& c : t.cards) {
        const NotifItem* n = NotifById(c.id);
        if (!n) {
            c.leaving = true;
            continue;
        }
        c.h = ToastHeight(*n);
        float target = g_set.notifTop ? kToastM + acc : H - kToastM - acc - c.h;
        if (!c.placed) {
            c.y = target + (g_set.notifTop ? -22.0f : 22.0f);
            c.placed = true;
        }
        c.y += (target - c.y) * 0.28f;
        acc += c.h + 10;
    }

    RECT rc = {0, 0, pw, ph};
    if (FAILED(t.rt->BindDC(t.memDC, &rc))) return;
    t.rt->BeginDraw();
    t.rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    t.rt->SetTransform(D2D1::Matrix3x2F::Scale(t.scale, t.scale));
    t.hits.clear();
    auto in = [&](const D2D1_RECT_F& r) { return t.mouse.x >= r.left && t.mouse.x < r.right && t.mouse.y >= r.top && t.mouse.y < r.bottom; };
    auto col = [&](D2D1_COLOR_F c, float a) {
        c.a = std::min(1.0f, c.a * a);
        t.br->SetColor(c);
        return t.br.Get();
    };
    const Theme& T = g_theme;
    for (auto& c : t.cards) {
        const NotifItem* n = NotifById(c.id);
        if (!n || c.op <= 0.01f) continue;
        float a = c.op, x0 = kToastM, y0 = c.y, x1 = x0 + kToastW, y1 = y0 + c.h;
        // (no blur behind: a soft shadow and a nearly solid tint)
        for (int i = 3; i >= 1; i--)
            t.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x0 - i * 2.0f, y0 - i * 1.0f + 4, x1 + i * 2.0f, y1 + i * 3.0f), 16 + i * 2.0f, 16 + i * 2.0f),
                                       col(D2D1::ColorF(0, 0, 0, 1), 0.07f * a));
        D2D1_COLOR_F bg = T.tint;
        bg.a = std::max(bg.a, 0.985f) * a;
        t.br->SetColor(bg);
        D2D1_ROUNDED_RECT card = D2D1::RoundedRect(D2D1::RectF(x0, y0, x1, y1), 16, 16);
        t.rt->FillRoundedRectangle(card, t.br.Get());
        t.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x0 + .5f, y0 + .5f, x1 - .5f, y1 - .5f), 15.5f, 15.5f), col(T.text, 0.12f * a), 1);
        D2D1_RECT_F cardR = D2D1::RectF(x0, y0, x1, y1);
        long long id = c.id;
        if (!c.leaving) t.hits.push_back({cardR, [id] { NotifOpen(id, -1); }});
        // Header: icon, app, close.
        const NotifApp& app = NotifAppOf(n->aumid);
        if (app.icon) {
            auto f = t.bmps.find(n->aumid);
            if (f == t.bmps.end()) {
                ComPtr<ID2D1Bitmap> b;
                t.rt->CreateBitmapFromWicBitmap(app.icon.Get(), nullptr, &b);
                f = t.bmps.emplace(n->aumid, b).first;
            }
            if (f->second) t.rt->DrawBitmap(f->second.Get(), D2D1::RectF(x0 + 14, y0 + 14, x0 + 36, y0 + 36), a);
        }
        std::wstring head = app.name + L" · " + NotifTime(n->arrival);
        t.rt->DrawTextW(head.c_str(), (UINT32)head.size(), t.fApp.Get(), D2D1::RectF(x0 + 44, y0 + 14, x1 - 40, y0 + 36), col(T.text, 0.62f * a),
                        D2D1_DRAW_TEXT_OPTIONS_CLIP);
        D2D1_RECT_F xr = D2D1::RectF(x1 - 34, y0 + 10, x1 - 10, y0 + 34);
        if (in(xr)) t.rt->FillRoundedRectangle(D2D1::RoundedRect(xr, 12, 12), col(T.hover, a));
        DrawLineIcon(t.rt.Get(), L"M8 8l8 8 M16 8l-8 8", (xr.left + xr.right) / 2, (xr.top + xr.bottom) / 2, 12, col(T.text, (in(xr) ? 1 : 0.62f) * a), 1.8f);
        if (!c.leaving) t.hits.push_back({xr, [id] { ToastDismiss(id); }});
        // Texts.
        float ty = y0 + 46, w = kToastW - 28;
        std::wstring title1 = OneLine(n->title);
        float th = ToastTextH(title1, t.fTitle.Get(), w, 38);
        t.rt->DrawTextW(title1.c_str(), (UINT32)title1.size(), t.fTitle.Get(), D2D1::RectF(x0 + 14, ty, x0 + 14 + w, ty + th + 1), col(T.text, a),
                        D2D1_DRAW_TEXT_OPTIONS_CLIP);
        ty += th;
        if (!n->body.empty()) {
            float bh = ToastTextH(n->body, t.fBody.Get(), w, 56);
            t.rt->DrawTextW(n->body.c_str(), (UINT32)n->body.size(), t.fBody.Get(), D2D1::RectF(x0 + 14, ty + 3, x0 + 14 + w, ty + 3 + bh + 1),
                            col(T.text, 0.78f * a), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            ty += 3 + bh;
        }
        // Buttons.
        if (!n->actions.empty()) {
            ty += 12;
            float bw = (w - 8 * (n->actions.size() - 1)) / n->actions.size();
            for (size_t i = 0; i < n->actions.size(); i++) {
                D2D1_RECT_F br = D2D1::RectF(x0 + 14 + i * (bw + 8), ty, x0 + 14 + i * (bw + 8) + bw, ty + 34);
                bool primary = i == 0;
                D2D1_COLOR_F fill = primary ? T.text : T.hover;
                t.rt->FillRoundedRectangle(D2D1::RoundedRect(br, 9, 9), col(fill, (primary ? (in(br) ? 1.0f : 0.92f) : (in(br) ? 1.6f : 1.0f)) * a));
                D2D1_COLOR_F ink = primary ? D2D1::ColorF(0x141418) : T.text;
                const std::wstring& label = n->actions[i].label;
                t.rt->DrawTextW(label.c_str(), (UINT32)label.size(), t.fBtn.Get(), D2D1::RectF(br.left + 6, br.top + 7, br.right - 6, br.bottom - 5),
                                col(ink, a), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                int act = (int)i;
                if (!c.leaving) t.hits.push_back({br, [id, act] { NotifOpen(id, act); }});
            }
        }
        // Time left.
        float frac = c.total ? (float)c.left / c.total : 0;
        t.rt->FillRectangle(D2D1::RectF(x0 + 16, y1 - 2.5f, x0 + 16 + (kToastW - 32) * frac, y1 - 1), col(T.text, 0.45f * a));
    }
    t.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    if (t.rt->EndDraw() == D2DERR_RECREATE_TARGET) {
        t.rt.Reset();
        t.dc.Reset();
        t.br.Reset();
        t.bmps.clear();
        return;
    }
    POINT dst = {t.rc.left, t.rc.top}, src = {0, 0};
    SIZE size = {pw, ph};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(t.hwnd, nullptr, &dst, &size, t.memDC, &src, 0, &bf, ULW_ALPHA);
}

// One frame: time runs out (not while the mouse is on the card), cards fade in and out.
static void ToastStep() {
    auto& t = g_toast;
    DWORD now = GetTickCount(), dt = t.lastTick ? now - t.lastTick : 0;
    t.lastTick = now;
    bool moving = false;
    for (auto& c : t.cards) {
        if (!c.leaving && !c.hover) {
            c.left = c.left > dt ? c.left - dt : 0;
            if (!c.left) c.leaving = true;
        }
        float goal = c.leaving ? 0.0f : 1.0f;
        if (std::fabs(goal - c.op) > 0.01f) {
            c.op += (goal - c.op) * 0.24f + (goal > c.op ? 0.01f : -0.01f);
            c.op = std::clamp(c.op, 0.0f, 1.0f);
            moving = true;
        } else {
            c.op = goal;
        }
    }
    t.cards.erase(std::remove_if(t.cards.begin(), t.cards.end(), [](const ToastCard& c) { return c.leaving && c.op <= 0.01f; }), t.cards.end());
    if (t.cards.empty()) {
        KillTimer(t.hwnd, TIMER_TOAST);
        ShowWindow(t.hwnd, SW_HIDE);
        t.lastTick = 0;
        t.mouse = {-1, -1};
        return;
    }
    ToastRender();
    SetTimer(t.hwnd, TIMER_TOAST, moving ? 15 : 60, nullptr);
}

// ---- Notification sounds ----
// Nimbus' own: four made here (synthesized) and eleven from Kenney's "Interface Sounds" (CC0, kenney.nl).
// Kept as IMA ADPCM (4 bits a sample, 24 kHz, mono) in base64; made into a WAV in memory the first
// time each one plays. A setting's value: "0" none, "1" Windows' notification sound, or a sound's id
// (a number from before the ids: its place in the old list, see SoundValue).
struct NimbusSound {
    const wchar_t *id, *pt, *en;
    int samples;
    const char* data;
};
static const NimbusSound kSounds[] = {
    {L"bong", L"Bong", L"Bong", 26397,
     "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"},
    {L"bongseco", L"Bong seco", L"Dry bong", 2878,
     "////75mpmqu7y7qsuqurq5qJCDJUUzREQ0NDQzMlQzIkQzJDMjNDMjIjIxIRgKnMvM3Ly7zLy8u7vKzLuru8yqqruqqqmogRMjY1NTREMzQ0NENDQjIzQzMzJDMjMyISAJjKvb3My7zbu9u7vMu7y8u6u6y7q7ur"
     "qpoIIVNENDU0NERCMjQzRDIzNEMyMzNDIiIiAYCp29u8vby8vLzbury7vLu8rLu7rKu6qqqJCCFjQ0RTM0QzNDQ0QzM0MzRDMjMzJCIiEgGAqsy8vczLy8vLu7y8u7zLu8u6u8uqq6qpiBgyREQ0RDM1Q0NDMzRD"
     "M0MzNDJDIiMjIhEBmMrbvMy827u9u8y7y7u8y7u7vLq7rKqpmQkQQjQ1NTQ0RDM0Q0MzNEMyJDMzMyQjIxICgZnb27y928u7razLu8u7vMu6u6y7uruqqokIIkRENEQzNTRDJCQzNDM0QzJDMjIjIyMSAZC6zdu8"
     "zLu9y8vLu7y7vLzKurrLqquqm5qIEDJFUzQ0NEQzNEMkQzIkM0MjJDIyMjIiEQCp2tu8zMvLvLu9y7u8y7usu6y7q7u7u6qJCDE1RTQ0NUM0Q0MzUzIkMyQzQzIjMyMjEgGYyry+vL3bu7y8vMu7vMu6rLu7y7q6"
     "qqqZiCE0RUM0NUNDJCQzNEMzJEMyMzM0MjIiEgGQqr2+29u7vLy8vLvMusu6y6u7y6qrq6mZCSFDNTU0NVMzNENDQzIkMzQyQzIjMzIiEgCoy8y8vcy7zLu8y8u7vLu8u8u7u7vLmpoJGCJENEUzNUMkJEMzQzM0"
     "MzQzQzIyIyIhAJnbu8y8zLutrMu7rKy7y7usu7u6u6ubmRkyQ0NTQzRDQ0MzJTNDMzQyQzIyMiMxEQG5u9u7vL3Ly8u727u7vMury7q6qrqpmREiMzRDNFMzNEQyQzMkQzIzQzIyMiIRAZm7u7zby8q7vLy8u7ys"
     "u6y7u8uquZoKECEzMzUzNDVDQ0MzQyQzQzNDMjMjMyMSAJm7u627rbzLy8u7vMu7rMu6u7vLqqqqmQExMkMzNSQ0NEM0QiM0MyRDMjMzQyIiEhGQqru8y7y8vLy8y8u727q7vLq8uquru5oKECIzUzNTMzVTIzQz"
     "NTNTMjMkM0MyIiMiIQCZu7u9u73by8q7y8u7vMu7u7y7y7qrqqqZETEyUzNDNFNDMzQ0MzUzQzNDMzMkIyMiEgGZu7u828u7vbzLu7y8u7zLu8u7u7u7rKmZGSEyMzUzNUNTMjRDQzNDMzRCMjMzM0MSMQGQubrb"
     "usvL27u8vMu7vMu7y7usu6u7u7uaCREiMiRTMzQ0NDRDQzM0MzRDMzNDIzIzIhEBmau727rLy8vLu7y8y8u6u7y7y7qru7ubCgARIzNTMjM1JDQzNTM0NDNTMjMzQyMzMxIRkKm5u9u6u73Ly7u9u9u7u7y7vLq7"
     "ramampkQESIzMzRCMzUzNDRDMzQzNDNTIiEjIhERkZmau7u7vLu9urzLu7zLu7y726qqu7ubm5kJEBESIzMzMyQzUyMyUyIjQyMzMzMiEhIRAJCZqaqqu7vLqsur26qr26qqy6q7u7qampqZCQARERIiMTIyIzMz"
     "MzMzMzMjIxITEhIRAQCQmZmamququrq6u7u7u7u7u6qrqrmpqZmZmQkAARERIRESEhMiMTExMSESEhIREhEQEACQkJmZmZqampqbm6u5mpurqZqqqZmamZmZmQkJABAQEAERERERERIREREhEQEREBAAAZAAkAmZ"
     "mZmZmamZqZmaqZmaqZmZmpmZmZmZmZAACQAAAAABEAABEBAQEAABEAABAAAAAJAACZCQCZmQmQmZmZmZmZCZmZmZkJmQCZmQkJAACZAACQCQAACQAAAACQCQAAAJkACQkAAJCZCQkJCQkJCQkAkJkJCQkJCQAAk="},
    {L"vidro", L"Vidro", L"Glass", 28800,
     "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"},
    {L"cristal", L"Cristal", L"Crystal", 2136,
     "d3d3p/+KkAhSRhOwzasIQiIQEAGw3rsacyMCqauaqLoKdUMDuc2KGCEREUIS2M2rKEQigZmKmdurKWUzgtqrigAREFI0A/u8iiA0IoCJgMnNqzA3I5DLqomAiDhlM4HcrAkhMgEAIYHszIpBQxKYq5qIuqpxRSKg"
     "zKsIIhIQQTOh37sJQzQBmZqIuc0KUjUDucyKGBEIIDUkuM6rGDQTkZgAkOzLGWMzgrusiRCYiWI1E9rMihEzAYgQE9DdmzhEI6C6ixiovBlWNIDLvAgxEogYNATavopBQwGomgCA2pw4VSKo24oYEpCJQjWB3KwI"
     "QiKQqRgSuM+KQTSDyasJIYG7KmYjqM2aIDMCqgpSEvvLGVMjkKyaIRLKq1A1A9qsCTITqKtAJZHNqzBFAamrKDOxzYpTNJDMmiAkkcoJQiPJvQpTI6C8CkITyqwoNQPZrBlDErmsECWS25swNgHLqyA0gbyLYjOg"
     "votBQ4C7ikEjyb0ZRBSoy4oyI7m8IDcC26wZQyOpuyk0od2aQUQBuqwZI4G6G3QjqL6bMTQCuooyBPq8CEQUkLqbIBK5nGFEgsq8CTIzkJogJbDfmjBEEqmriRGgvCpHJJC8rAAjApgoNAP8vAlCJIGpigiQ26th"
     "NRO5vZoQIoAZczOwzpwIQyKAigiA6qwKczMByquJEICJUTYDya6bITMSgBgR0N2rKEUjgLuriIi7KWczguu7ijEjERBDA/u+mkBTEZCamYi7rEBGI5DMqgkhARBSQ5Hsu4lCJBGJiYi6r4tSRBKpu6sYgIBCRiO4"
     "3asYQiIAABCo3awJUyQCuauaiKoJZTQDyb2aKDISIDIT+c2rKEQjgbmambuuKFVDgbqtihACEUFTAdq9miE0I5CYmLq/q0BVIpDKqokAgDBUNJHcu4lBMhEAEJDcvZpCNROou6qZqYpyRROgvawIIiIQIiPB3rsa"
     "UyQCmaqJurwKZEQCuLyqABIREkUTyL6cGEMigZiYqMy8GGQzgcm7mQCAEGNEAsq9miEzI4ABEdu/nDhEI6G6q4moqzh3I5HauwohIxEhQwL8vIoxJhKYmpmgy6tRRRSYy6oZIREAUkORvrwJQzMBuAiYys2aUzQT"
     "ua2aEICIMVUjsM6qGCQhAAkRkb2vGVIjAaubiYC6m3NDA9m7mzEzgQAzQ7C/uzBFEpC6CAiwvQg0F4G6uxkiApARUxG5zZkxJIK5mRABuqwRJhKgq4wwEZAaMTOTu60ZMiOZmRARobqbMRURqqoQEZGpABMEmLub"
     "IQMBCRkQEZC7GSISkZoJEQCQmSkRA6mZmRIAAAkQEZGZqRECEZmQEAEAmQAQEZEJmRGRkQAAEAGQkBkQAZCRGRAAAAkAAQGQkBAQAAAJEACRAAkBEAAACRAAAAAAABCQAAAQAAAAAAAAAAAA"},
    {L"arpejo", L"Arpejo", L"Arpeggio", 26372,
     "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"},
    {L"bolha", L"Bolha", L"Bubble", 16800,
     "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"},
    {L"pingo", L"Pingo", L"Drop", 4406,
     "f3d3d3eAgJiZqaqrqooIMUQ0RDIiEoC53Mu8u6yaCRA0VDM0MiMBqNvMy6y7qooYMjY1QzMjEpHJ68vLu6ubiSFUUzM0MxMCqOvby7vLqokYQlM0QzMjEpHJzNu7vLqaiDFERDM0MyIBqMzMy7vLmooQQkRDQyMj"
     "EZDJvM3KqquaCDFENCUzMyIBqdzbu7y7q4kQY1NDMyQiEZi6zbysy5qKCCFENDRDMiGAqcy8zLqrqokhY1MzNCQSAZjKvL3Lu6qKGEJENDQzMhKAur69vMuqmgkRRFNDQiISgZjKzMu7y5qKAEJEQ0MjIxGQus28"
     "vKyqmggxRDQlMzMiALnc28u7upuJIEREQzNDEgGQy7y9vLurmRhSUzRDMyMSgLq+vby7u5sJMTY1NCQjIoGo68u8y6uqiRBDNTUzNCIRmMrM27u7u5oYQjU1NDMkEYCpzLy8vKqqiCFENDUzMxMCqcy9vLyrq5kg"
     "Q0VDQzIiEZDKvL28u5yaADFENDQkIxKAqczMu6y7mokhRDQ1MyQiAJjLzLzLu6qZEENUM0QiIwGAus28y7urqhhBRFMzJCMigKm9vbysq6oJIERTQzMkEgGYy8zburuqiihSNDUkMyMRkMrMvLy7u5oIQkREM0Mi"
     "EoCpvb28y6qaCSFTREMzIyOBqMzMy8uqqokQQ0Q0MzQiAZDLzLzLu7qZGEJENDQzMxKAysy8zLqqmggxRDQ1QiIRgai8vby8upqJIEQ0RDIzIwGo69vLu7urihBTRDQkMyMRkMvMvLy7q5oIQkREQjIiIYCq3MvL"
     "u7uaCTFUNDQ0IhOBqMy8zLq7m4kgY1NDMzMjEajr28vLuqqZEEJEUzIkEgKQuszMu8uqmQgyVEM0MjMSgMnry8u7upoJMVQ0NCQjEgGpzLzMuruqiCBENDUzJCIBqMu9zLrLqYkQQjQ1MzQiEZjLzLzLu7qZEEJE"
     "NDQzMxGAu87bu8uqiggxREQzJDMhgLncy7y7u6uIMUU0NTMzEwG43Ly8vKubiSBjU0MyJBIBqMrMy7vLmpkQQ0RDJCMjAaDKzLzLu6qKGENENDQzIxKIy729y7urmhgyRkNDMzISgLq+zMu6u6kIMUVTMzQyEoG5"
     "zMzLu7qbiTFERCQkIiIAqMu9vLy7qokgU0Q0MyQiAaDLzLzLu6qZEFNTNEMyIxGYy8y8vLq7mRhDNTVDMzICgMvMvLy7q5oIQ0REQiIjEZC5vb28u7ubCEJENDQkIxKAub29vMuqmogiRFNDMyMTgbnNy7ysq5oJ"
     "ITRFQzMzIgG5zMzLu6yaiSBTNDQ0IyMBqdvMy8urqokgQ0Q0QzMiAZjb67usu6uKIFM0NTQjExKo2tu8y6urmRBDRDQkMyMCoLu/vLy7nIoYMjUmMyQTEpjJ29u7u5yaGDI1NTQzFBKQycu8vKyrqQAyNTU0JBMS"
     "gLrcy8u7q5oIMkVTQzMyEoC6zcu8y6qagCJENDQ0IhIAuszbvLurmxkxVTM1M0MRAaq8zcurq6oZIUVDNEMiEoG5+7q8vKqqCTFzIzRDMhKBqfu6vLusqggRRFMzNDMTgqrMy8vLuqoJIXMiQzMzE5K5vLy9rKur"
     "iDFjMlMyMyORudu7vbusmokiQ0NTIzMjEbDLy8u7vJoJIVMyUzJDIQC5y8rLu7u7ChEkQ0NDIhICqbu9u7yru5khMyUzNDIxEaC7vMu7y5qZETMlMiQyIgGZu9u6u7u7mRAzJEIzMzMRoLq7vbm7q5kRMzM0MhUS"
     "AKm5u62quqsJIDEzNDIzMxGZurvLq7qbGyAiM0MiMyIBsbm6y6u5mxoQIyMzJBIjAJCruayqu5oJKTEzMzMzMQCQm7u7u7upACAxMTMzMxEBkJuqu7uqqQAgISIyMjIRAZCquau7uZmZESIxMiISEhCZmqqbu5qa"
     "GRESMTIxIhEBqZmau5upmZkRIiAiIiERAJmZqrmam5kJERIxEhMxEACZCrmpm6kJABEhERMTIQEAmZmaqrmZCRkRESETEiEAAJmZuampmZkBERESIRIRAZGZmampqZkJGRECIRExEQAAmZmZm6mZkBARERESERER"
     "mZCQmpmamQAQEQEREhERkZGZkJmamZkZABERERESEBCZCZAKCqkAkBEREBECIBkQmZCQCpmZkBkBEQEREREBkZCQkJmZmZkQEBEBEREREAmQAJmZmZkZABARABEREZEACQCZmZmZEAkRARAREBEZkACQmZmQAAkZ"
     "EQEAEQEQAJEJkAmZCZAAEBABABEQEACQAJAJmQmQAAEBAQARARCQAACQCZkJABkBEQAQERAQCQAAkQmZAAkBAQEBEBABAQkQAJAAmQkQABABAQARAZAQABCZAJCQAQARAAEAAQAQAAABCQmRAAAAERAAAAABAAAB"
     "AAkAAAAAABEAAAEAABAAAAAJAAAAABABABAAAAABAAAAGQkAAAABAQAQAAABAAAAAJABCQCREQAQAAABAAAQAJAAAAAAAAEBABAAAAABAACQABAJABEAAAEAABAAAAABCQAAAAABAQAAAQAAEAAAAJAQGQAQAAAQ"
     "AAAQAAAAEAAJAAAAAQABAAAQAAAAEAAACQAAEAEAAAABAAABAAAQAAkAAAEAAQAAEAAAEAAAABAJABAAAAEAAAABAAABAAAAAAABAAAAAQAAABAAAAAAAQAAAAABAAAAEAAAAAAQAAAAAAEAAAAQAAAAABAAAAAA"
     "AQAAAAABAAAAAQAAAAABAAAAEAAAAAABAAAAAAEAAAAAAQAAAAABAAAAAAEAAAAAAQAAAAABAAAAEAAAAAAQAAAAAAEAAAAQAAAAABAAAAAAAQAAAAABAAAAEAAAAAAQAAAAABAAAAAAEAAAAAAQAAAAABAAAAAA"
     "AQAAAAABAAAAEAAAAAAQAAAAABAAAAAAAQAAAAABAAAAEAAAAAAQAAAAAA=="},
    {L"pingo2", L"Pingo 2", L"Drop 2", 4373,
     "d3d3d3eAgJiZqbqqm4oIMjU1QzMzEojKzMy7y7qZiCJENEQyMiGAqczMu7y7qoggRDQ1QyMiAZi8zcu7u7uJIGM0RDMzIgKo69vLu6yamSBCRDQzJCIBqNrby8uqm4kgQ0Q0QzISAZm8zcu6u5sJIVRTMzQyIYCp"
     "vb28rKuaCCI1NTQzMyKQytzLy7uqmhBCVDNEIiIBmcvMy8uqmoghY0NDMzISkLq+vcu7upkQUzQ1QzISAajM28u6u5oIMUU0NDMzApDLzcusq6qJIFNEMzQjIoC5zby8y5qKADNFNEMyIQCpzNvLuquZGDI2NUMy"
     "IgGp68u8u7uaGFJjMzQzIgGp3Nu7rKuZGEJEQ0MyIYC468vLu7qJGFM0NTMkEoC5zMy7u6uJIEQ1NEMiEZC6zby7rJoJIUREMzMjAqjMzKysqpkAMlRDMzMTgbrNzLu7qwogRVNDMiIRmMvcu8urmQhCREMzJCGA"
     "ucy8vLuqCSE1RTMzIwK4zL28u5yZEEJEQzMyApDa26ysqpoAMUVDMzMTgLrOy6yrmgkxREQzMyKBudzMuqyaiCFTUzMkEgGp27zMqqqIIFM0NDMjAbjc27usmokgNDUlIyIBqcy8vLuqiTFUNDQyIoGpzdu7q5sJ"
     "MUU0NDMSgLm+zLurmwgzRkMzMxKAy83Lu6uaEFNEQzIjEajbzMu6qokhU0QzJCIAuevLrKuaCDFFQ0MiEZC6zcu7q4oQNDY0MyIBuNzbu7uqCEFEUzIzEYDLzLy7u5kRRERDMhIBqcy8vLuaGDI2NTMzApjMzLus"
     "mwkwRDQ0IxKAy8y8u6uKIFRDNCMiALrN26urmhBSNEQyEgGpzNu7u5oYQjVEIyMBqLy+y7qaCEFTNDMjAqjrvLy7mxlBRDQzMwKozMzLq5oIMUVDMyMCqMzMu7ubCEI1NTMjEam9zbq7mghDNUQyEgGpzLysq5oQ"
     "QzU0MyOAycy8u5yKEVNTMzMiiMvNu7yaiTFERCMjAqjbzLu7qgBSNDUzIoG5zdu6q5kgU0QzMxOQ28y7vJoJMUVDMyMRuczMu7uaEDQ2NCMikMrMy7ubiTJFUzITAqnry8uqihBSQzQyEYC7zrvLmggyVDM0IgC5"
     "3Mu7q4khNTU0IgKo68u7rIkYQ0QzMxOg28zLq5oIQkRDMhKBur68u6uJMjdTIyIBucy8vKqJIUQ0QxMCqevLu7uKMEREMzMCqMzMu7uZIEREMyMCqOvbu6uKIGNTMjMRqOvLrKuJEFNTMzMRqMy8vKuaEUQ0NCMR"
     "mczbu6uKIEREIzMBuMy8vJuKITVEMyMBqr3NupqJMUQ0JBKBucy8u6sIUlNDIxKAu768rJkAQlNDIhGgysy7u4owREQjIwK5zMyrm4kxNTVDEoC5vbysihlCU0MiAqDKzLu7iiFUQ0MSgam8vayaCTI1NTISoMq9"
     "rLuZIGNDQxIBqdu8y5oIMTU0JBKYytvLqooRUzRDIoGpzLy7qxhSU0MjEpi8zMuqiSFTNEMSgaq9vbqaIFJTMzMBqb29rJsIMjU0JAKYy9usq4ghUyUzEgDL28urmSBTUzMiAcnbvKubEFNTMzMRqs3bq5oZMkVD"
     "IxGp29u7qglCU1MiIZnL28uaiTE0NSQRkLu9rZuZIkRDMxKQvNy7qwlAQyUzEpDb27qriTE1JTMhmNu7vaoJMTQWMyGg27u8qwkyNUMzEZC827ucCTI0NDMToNu7y5oYUTJDMhG5rLy7uxBDQjMjEbm8y7uaKTM1"
     "IyMButu7ugowQ0IjEpG7vLudCSBDMyQCoLu8u7sJMSUyMhGZrNqaiilBMSQSkrnLuruaMTIkMhORusurqhAxMyQiEamr26kJGTMzJBEBubu8mZkhIzMkAKGru5yZGTIyQxGQuau7u5kRMzMzEqC5u7uaCRMzMyOR"
     "ubm7uwkRIzMjEZC5u6uZCTEyIxIBubm7mwkREzMjAZCbq6uZASIxIxGRubmqmZkxMSIRkqmgm5uZERIjMQGQmqu5mRExMSERkam5mpkZERITEpGpkKuZCRESMQIBmam5mRoQITEhAZmZmpsJGSEhEZOQmampCQkR"
     "EjEBkZmpqZkAEQISEZGQqZmaGREREhEBmaCZCgkBAiERAJCZqZmQERERIRmZkKkJCRARERERmZmZmZkRERERAZmQmZmZERERERCZkJmZCRERAREBmQCZmgARERERAZmQmakZIAEREZGQkJmpAREBEREAkJmZkJAR"
     "EREQAAmZmZAAERARAQAJkJmZEBABEQGQAJmZABAQEQEBkQCZmQAQERAREAkJmQkAEQEQEZAQkJkJEBABEQGQAZmQkAEREBABGQAJmRAAEQARkACRCQkBEAEAEQkQkAAJABEAEAAAEJkAEAABAQAAAJGQARkQEAAA"
     "AQCQAJABAQEAAQAACQAAEBAAAAEAkAAAAAEQABAAAAkAABAQAAAAAZAAAAABAQAAEAAJAAAQEAAAAACRAAAAARAAAAAQCQAAEAAAAQAAABkAAAEAEAAAAJABAAEAEAAAAACQARAAEAAAAAAAAAABAAEAAACRAAAB"
     "AAEAABAACQAQAAEAABAAkAAQABAAABAAkAAQABAZAAABAAkAAQABAAABAJABABAAAAABAAAAAAEAAAABAAAAAAEAAAAQAAAAAAEAAAAQAAAAAAEAAAAQAAAAEAAAAAABAAAAEAAAAAABAAAAEAAAAAABAAAAEAAA"
     "ABAAAAAAAQAAABAAAAAAEAAAAAAQAAAAAAEAAAAAAQAAAAABAAAAAAEAAAAAEAAAAAAQAAAAABAAAAAAAQAAAAABAAAAAAEAAAAQAAAAAAEAAAAAAQAAABAAAAAAEAAAAAAQAAAAAAEAAAAAAQAAAAABAAAAAAEA"
     "AAAQAAAAABAAAAAAEAAAAAABAAAAAAEAAAAA"},
    {L"pingo3", L"Pingo 3", L"Drop 3", 2612,
     "f3d3d0eqrKoIITUkEoDb26qZIUQzE6DMvJsJQzUjkdq8qhlSNBKQvL2ZMEQzkcqtmyBEI4HLvIogRSKYy7sKUzQBycuKIEQTsL2rMDUUsMyLMCaCuqwIQSSQ25owQwLJuwtzI6CuCjIkmLybUiSYrAoxJJC9ClIj"
     "yZ0oJIHbijE0kL4KUxK5rCg0kr0KRIHKikIT2ZsxFKirODXIrTAloK0pJKG7OSWwrTAkuKwhFMkKU5HMKTSQvCkksIxCgswKRJG6KBPbKjWwrTEEyxklsJ0ygrw4JMmLY6GsMYS6KBTaCjSgmjGxnjAUqiDBrlKD"
     "u0iDzTgTqkCivzAkqRixvlETqQm5C0eRqyCgGzbYChTJKRTJGJKbY7GLNNorNLgJoYxEqBoFyiihKifZGhKqUqGbE7xwAqsgulgDqiLsSQKLM+sZhIoz6iiRGgWsQKEZlKwioECzjgKrVLApsQ0zmiT7GJIZBash"
     "2kCREMENE5oz2jDQKaFJhawSmTLZQLAqskqEjIIMFKszyTD6QJA4wBuhcKIKsRsEKKMPkQsWioKbI50zuyS7FZ0TigWNA4yEC4WLA4uEG5Qrw0vCWcAgsEjAILgyyyScBAyUHJQroinAQKkijYUdkyyiOrEomIIc"
     "pTzTWsE4uBKLBAuRQMoyDKM+wymYBI6EKrhCyhMs0ki4IQu0StI4mZM8sSGNljvSMKqEK8M5mZRL0DKOhBuBCBqzWpmTW7gjD5MrGNJqsCGLpBgaxEqhoHjIIYmQIQ2kS5CxecARCYiCHKOQasAhG6AjD7MQLNMQ"
     "OtAiDJKIasABOckDOsuHKpiiQIyjIA2kgFq4AjqqlzsA0DGKmKdKoJBBjIGBW6mDAC6ziWCqoyIfoQE6mqcpKJyVCDrAkzkr8YM6C6SgcIyjklqKs4FJirICWwvUESor4qM4TLmlATuYwoNaHMKjOCyotINdCsKS"
     "ID2ptYJKK8CkIDwawbQRSwqhooJsG7G0gmoboKOhA18b0pMYOhvSo5EwP4rEkgA5PIrDs4J7KwnRthE7GhkJ0qMgPAoACfSkOC2YkgDRtEA9GpjDgikJOS2gEci3AZB5PaijgNOCKTo8irOyxQIakXs8qJMZ8qMB"
     "GSoZSiywpBjyozgLgWwrCSmos9S1ERoYXBuASRugw8Sjw7QBOhsYSxp7PYnCo4GxxKOyxaM4PQoAOjteLAkAGRmQsbOR9LSSkLMBsLURCTkbAUuKs2ksiEk9Gik8GilLTSsJWjwJKE0KAEs7KjsrWzxLPCtKLIlI"
     "PokQKxk6maKRowEJobOzo6LTtJKQsqMBkZIZGSkZOTs7OzsqGRCQkQGRoqOSAJERCRAZOSsZGQCRkaIREBkZKToqGRkJAZGRAZEBEBkQGQCRkaKSkQEQGRAZAJGRAQGRAQCRkQGRkREQGRkQkAGRAQEZKSoZGQAQ"
     "GSkqGQmRkQEBEBkQkAGRARAAEJABAQAQGQCRAQEQGZGRkQEBEBkAkQEQEBkAAAEZEAAQABAZGRAAkRAQGQABABAAEAAAEAAAAZEBAAEAEAAQAAAQABAAEBkZEBkAEAAQABAZGRAAEBkQGQAQAAAQABAAAAEAEAAQ"
     "AAABABAAAAEAAAEAAQAAAQABABAAABAAAAEAEAAAAQABAAABAAEAEAAAAQAQAAABABAAAAEAEAAAAQAQAAABABAAAAEAAQAQAAABABAAAAEAEAAAAQAQAAABABAAAAEAEAAAAQAQAAABAA=="},
    {L"pingo4", L"Pingo 4", L"Drop 4", 4390,
     "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"},
    {L"subida", L"Subida", L"Rise", 4389,
     "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"},
    {L"corda", L"Corda", L"Pluck", 2389,
     "/393R/Dzp06YgMIhmErCAqnlgJQAATscioH4o1CYAamBUA2BmEg7S5kjHEgfCkgYWpmAuDCMGRGVCJA7XMgDijmZpaBTiYGq2XgRiLSg0iqgHRAkiZAamMdJiYjDk6FYsgDgoSkGoYOaCQ+iqgJziICaogQuoZmC"
     "WQkqhxmIjBs0EViairwivRBwkgCpgUi8hJlAGaG4JwkAjLtzAjjAmOgoySowBgioGSnwIJkpooTIQqEg6chAEwCju+krs48RJJiQCgHDPLiKg2OKWZUIuKs9JwARm4uegp0BU5EImxESj6KZIFqAGScKiJ2bZAEg"
     "uInrELooUoSIqQgx+hG6IIGFmHOYgMq5cwMBkKvpKdAbMRWIiYoS5Cm4GoMyuXCkkcjZSBUAgbq5DZKNETSIiJsSAw+gm4JwgDkGGaqNHSUBEaqq2xGeEEGTCKkQIZ+BqiEgswt3iJGbnVICIaC6yin7GUETiMgA"
     "APgBuBABArpGooHK60EjIJHKyhvQizIXgJgaKLEtwAmDEchxAxjZ2ig0ERG6nayinTA1gZibABGNowwYOLF4FoCovQpFEiC4q70I6yhjgoipAADJAqswKJMdV4CQy5tyEyCgq+sJyBtSJIiZihHIGMgIEQOpdQII"
     "27toJBGBq9uLwZwyJoGYmhCgG7AMEkKocAaAuL0pJwIBubqtgLwwNgKYqhiBnJKsITGSWkeIoL0LVRIBqKu8CesZRAOIqQkRyhC7KDEEGmeBkOuaUSQQgLvbCsiLUxWAmIoRuBq4CyM0qnUEiMqsODcRAKvbmqCs"
     "QTWBiZoQkIyhjBEykHgXiKitGjYSAbnLu4i9OUYCmJkIgaqArCAig1pXgJjMimMjAaDLrInZGmMTiKmIEKkZ2hkSEghnAYjrqlEkEYC7vIvJjFI0gJmKAKAKwIoiIpB0BoDJrDk2IgC6zKqYrTE2AZiaGJCbkawx"
     "MpFwJ4CpvglFEwG4y7uKzClFE5ipCQGqCMsYMwI5dwKYzZpTJBGYu7ya2QpjFICZChCoCboKMjMZdxOY+5tAJRKAuryrqZ1RJYGZmRCYiaiMISMBcheIua4YRRIAuMu7mLxIRQKYmgiAmZi6MDIEYDaByb0LRRQR"
     "qLutmboqRiOIqokBuYjKKTISOHcDmNybUiQSkMu7m8qLVCSBmYoAoIq5C0MSAGcToOqsMDYSgMm7q7mtUTQCmZoYkKqgnSEyAXImkbq+GTYjAbnMq5nLOUYCkJkJgJmYujkzI1FXgrnNCmMjApi8rJq6G0YUkKgJ"
     "gJiZuRkyMzF3EqjMnFEzE5Dbu6u6jGMlAJmZAJCaqQoiQxFkJKDryyA2EwC6vbqpu2FEEZmZCICamaoxMxNzNIHavBpGExG5vKyZuik3I5iqGYGamrobJAExNxK4vptzFBKoyruauhtlE5GpiRCZmLoKMhIRNQSg"
     "65tQNBKAy8uquZpyJIGomRiYm5kLERMRMiSY26soNhSBucuqmrs5NhGYmQCQmZmaEBERISIBqboKMyWBsLu7m6oZQxKZmpkBAAmQCQEBEBERkJmZECIRkJmamZkAEREBCQAAAACQAAABAAAAAAAAAAAAAA=="},
    {L"pergunta", L"Pergunta", L"Question", 7980,
     "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"},
    {L"selecao", L"Seleção", L"Select", 5343,
     "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"},
    {L"clique", L"Clique", L"Click", 3659,
     "/3eUd/cPF54RgRiImAGCTpCqlwgpCQuE8CGALgGwgaI7YI0BspAgiWqooJYZOJmKFNEACmqRuAKhOBCPgrEiuAtxuAjEOYSdUpiLhqpCqCoA2ASRPQCcFNAIIgwQ4RgCiiGcQNOZIqg6KKqXkUmYjCPogYFbALoy"
     "o5wyu2qimhW5WJALBcghkBwSyyLSKhKdMdEIhJpBuCmVmjLJSpKcFLlJkowUuUCRDCPLMcIbFKxAsBoEu2KoKoWqM8g6hJwjyTmSjiLJOKMNI8pAogsky0CxCwW6QLArBatCsDsFq1O4OoSdIsgpk40iyTijjDS6"
     "WKILFbpAsAsUvEGwKxSsQrA6BatD0CmTq1LJKZKcM9k4owwkuViiCxTKMLENA9sxsBsGqkKgKgWbQsAZg50x2SmSnDPJSJILNLpQogwjyzjCDBO8QbArBapSsSkEukPAKgOfEbgZk51CuUiCCyW5QLIcI7w4wZsV"
     "u2CgKhO7U7EoFJwy2hmimxG5K7OaM7lAkQkzuTCTDJK6EJEdAboxsxszuTKyGRObMbkZkZsRsTqSmyKwOQOaMaEZk5oRmQmQmQGQGQEZEZARAQkRkAkBmQAAmRAJGQEAEAAAEAGQAZAAAAkAkJCRARkQkAEAEAAZ"
     "kQAAkAAACQGZAZEZAZABkQARmRCQAAAAAAAJAZABAAAAABCQAAEJAAAAAJABkBAAAAAAAAAAEAkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQCQpD2j0Gyiklug8lwI5SkZwgk6o4FOophpoYk5o5V3l39R8HrCgVujmUukORjiKwLCL5KCGtIZMOMMILQrkI"
     "I8tIk50ysY0z2DoT6ygTvBGTDSLhGSLJGBOdMKOOIbELQsk5A8xBgp4ioR0i2DoSyjARnxGjjCGhCjL5GDK7EAOMIeEaM/soA8sxgp8hkQwhwDgR6yEDnhGzjELICTPLGAWrQKKqYpiLFakqBMowgaxCkA0iuDsz"
     "+yACrDGzjUK4CiTKKYS6aJGqQqCLJbk6FMswk60ysg4iyBoUyiCDrEGyi1O4CiW6OoXKQJGrM7ANM9kpE8tAgqxCoYwz2BoEujiErDGynDS4Gxa5OQS7UJKsM8ALJMo6E7xYkqtSoYs0yCoUu0mDvEGynDPIGyTK"
     "OQTKMZOdQqALM+kZE9sgkqwywos0yCoku0iEq1Ghm0PIGyPqKQPLQJGbQ7EbJcgpI7xIgq0xsY0j2CoT2jCDrFKhizTIGiS7OgWsOJKtMrGMJLk6FatAo6tTsQtDyQkT2zmSy0GgG1OYGjO5KRO6MAGbMLCaE7kq"
     "A7k5kpoxopkxoBkSmhkBmQCRGhGgABGQEBGZEQGZEZGZEZkAAZkQAJkRkAABABkAABAACRCQAAAAAAAAAACQAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"},
};
constexpr int kSoundCount = (int)(sizeof(kSounds) / sizeof(kSounds[0]));

static std::vector<BYTE> SoundWav(int i) {
    static const short kStep[89] = {7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107,
                                    118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963,
                                    1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894,
                                    6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
                                    32767};
    static const int kIdx[8] = {-1, -1, -1, -1, 2, 4, 6, 8};
    const NimbusSound& snd = kSounds[i];
    // base64
    std::vector<BYTE> bytes;
    unsigned acc = 0;
    int bits = 0;
    for (const char* c = snd.data; *c; c++) {
        int v = *c >= 'A' && *c <= 'Z' ? *c - 'A' : *c >= 'a' && *c <= 'z' ? *c - 'a' + 26 : *c >= '0' && *c <= '9' ? *c - '0' + 52 : *c == '+' ? 62 : *c == '/' ? 63 : -1;
        if (v < 0) continue;
        acc = (acc << 6) | v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            bytes.push_back((BYTE)(acc >> bits));
        }
    }
    // ADPCM to 16-bit samples, behind a WAV header
    int n = snd.samples;
    std::vector<BYTE> wav(44 + n * 2);
    auto put32 = [&](size_t at, DWORD v) { memcpy(&wav[at], &v, 4); };
    auto put16 = [&](size_t at, WORD v) { memcpy(&wav[at], &v, 2); };
    memcpy(&wav[0], "RIFF", 4);
    put32(4, (DWORD)(36 + n * 2));
    memcpy(&wav[8], "WAVEfmt ", 8);
    put32(16, 16);
    put16(20, 1);
    put16(22, 1);
    put32(24, 24000);
    put32(28, 48000);
    put16(32, 2);
    put16(34, 16);
    memcpy(&wav[36], "data", 4);
    put32(40, (DWORD)(n * 2));
    int pred = 0, idx = 0;
    for (int k = 0; k < n && (size_t)(k / 2) < bytes.size(); k++) {
        int code = k % 2 ? bytes[k / 2] >> 4 : bytes[k / 2] & 15;
        int step = kStep[idx], dq = step >> 3;
        if (code & 4) dq += step;
        if (code & 2) dq += step >> 1;
        if (code & 1) dq += step >> 2;
        pred = std::clamp(code & 8 ? pred - dq : pred + dq, -32768, 32767);
        idx = std::clamp(idx + kIdx[code & 7], 0, 88);
        short v = (short)pred;
        memcpy(&wav[44 + k * 2], &v, 2);
    }
    return wav;
}

// The setting as a number to play: 0 none, 1 Windows', 2 + index in kSounds.
static int SoundValue(const std::wstring& s) {
    if (s.empty()) return 0;
    if (iswdigit(s[0])) {
        int n = _wtoi(s.c_str());
        if (n <= 1) return std::max(n, 0);
        // the list's old order, when the setting was its place in it
        static const wchar_t* const old[] = {L"bong", L"vidro", L"arpejo", L"bolha", L"pingo3", L"pingo2", L"pingo", L"pingo4",
                                             L"cristal", L"subida", L"corda", L"pergunta", L"selecao", L"clique", L"bongseco"};
        if (n - 2 >= (int)ARRAYSIZE(old)) return 0;
        return SoundValue(old[n - 2]);
    }
    for (int i = 0; i < kSoundCount; i++)
        if (s == kSounds[i].id) return 2 + i;
    return 0;
}
static std::wstring SoundSetting(int v) { return v <= 1 ? std::to_wstring(std::max(v, 0)) : std::wstring(kSounds[v - 2].id); }

static std::wstring SoundName(int v) {
    if (v <= 0) return L2(L"Nenhum", L"None");
    if (v == 1) return L2(L"O do Windows", L"Windows' own");
    if (v - 2 < kSoundCount) return L2(kSounds[v - 2].pt, kSounds[v - 2].en);
    return L"?";
}

// In a call (Discord, Teams…) Windows turns the other sounds down, by 80% unless the user changed it
// (Sound › Communications), which left Nimbus' notification sound all but silent. Its sounds play in the
// process' default audio session, so that session opts out (checked each time: the output may change).
static void SoundSkipDucking() {
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    {
        ComPtr<IMMDeviceEnumerator> en;
        ComPtr<IMMDevice> dev;
        ComPtr<IAudioSessionManager> mgr;
        ComPtr<IAudioSessionControl> ctl;
        ComPtr<IAudioSessionControl2> ctl2;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&en))) &&
            SUCCEEDED(en->GetDefaultAudioEndpoint(eRender, eConsole, &dev)) &&
            SUCCEEDED(dev->Activate(__uuidof(IAudioSessionManager), CLSCTX_ALL, nullptr, (void**)mgr.GetAddressOf())) &&
            SUCCEEDED(mgr->GetAudioSessionControl(nullptr, 0, &ctl)) && SUCCEEDED(ctl.As(&ctl2)))
            ctl2->SetDuckingPreference(TRUE);
    }
    if (SUCCEEDED(co)) CoUninitialize();
}

static void PlayNimbusSound(int v) {
    static std::vector<std::vector<BYTE>> cache(kSoundCount);  // (an async sound reads its buffer while it plays)
    if (v <= 0) return;
    SoundSkipDucking();
    if (v == 1) {
        PlaySoundW(L"Notification.Default", nullptr, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
        return;
    }
    int i = v - 2;
    if (i >= kSoundCount) return;
    if (cache[i].empty()) cache[i] = SoundWav(i);
    PlaySoundW((LPCWSTR)cache[i].data(), nullptr, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
}

// The sound for a card that just showed. Another app's notification plays only when Windows' banners
// are hidden by Nimbus (otherwise Windows already plays its own), and never when the app asked for none.
static void NotifSound(const NotifItem& n) {
    if (n.reminder) {
        PlayNimbusSound(SoundValue(CfgStr(L"remSound", L"bolha")));
        return;
    }
    if (n.silent) return;
    if (n.aumid != L"#nimbus" && !g_set.notifHideWin) return;
    PlayNimbusSound(SoundValue(CfgStr(L"notifSound", L"pingo")));
}

static void NotifShowCard(const NotifItem& n, bool force) {
    if (((!g_set.notifShow || g_set.notifDnd) && !force) || !g_toast.hwnd) return;
    // Nothing over a game, a full-screen video or a presentation.
    QUERY_USER_NOTIFICATION_STATE qs = QUNS_ACCEPTS_NOTIFICATIONS;
    if (!force && SUCCEEDED(SHQueryUserNotificationState(&qs)) && qs != QUNS_ACCEPTS_NOTIFICATIONS) return;
    auto& t = g_toast;
    ToastCard c;
    c.id = n.id;
    c.total = c.left = n.sticky ? 24 * 3600 * 1000 : g_set.notifSecs * 1000;
    t.cards.insert(t.cards.begin(), c);
    int live = 0;
    for (auto& x : t.cards)
        if (!x.leaving && ++live > 3) x.leaving = true;
    ToastRender();
    SetWindowPos(t.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    t.lastTick = GetTickCount();
    SetTimer(t.hwnd, TIMER_TOAST, 15, nullptr);
    NotifSound(n);
}

LRESULT CALLBACK ToastProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto& t = g_toast;
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_TIMER:
            if (wp == TIMER_TOAST) ToastStep();
            return 0;
        case WM_MOUSEMOVE: {
            bool was = t.mouse.x >= 0;
            t.mouse = D2D1::Point2F(GET_X_LPARAM(lp) / t.scale, GET_Y_LPARAM(lp) / t.scale);
            if (!was) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
            }
            for (auto& c : t.cards) c.hover = !c.leaving && t.mouse.x >= kToastM && t.mouse.x < kToastM + kToastW && t.mouse.y >= c.y && t.mouse.y < c.y + c.h;
            ToastRender();
            return 0;
        }
        case WM_MOUSELEAVE:
            t.mouse = {-1, -1};
            for (auto& c : t.cards) c.hover = false;
            ToastRender();
            return 0;
        case WM_LBUTTONUP: {
            D2D1_POINT_2F p = D2D1::Point2F(GET_X_LPARAM(lp) / t.scale, GET_Y_LPARAM(lp) / t.scale);
            for (int i = (int)t.hits.size() - 1; i >= 0; i--) {
                const D2D1_RECT_F& r = t.hits[i].first;
                if (p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom) {
                    auto fn = t.hits[i].second;
                    fn();
                    break;
                }
            }
            ToastRender();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---- Notes and reminders ----

static long long NowFt() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}
constexpr long long kFtMinute = 600000000LL;

static std::wstring Esc(const std::wstring& t) {
    std::wstring o;
    for (wchar_t c : t) {
        if (c == L'\\') o += L"\\\\";
        else if (c == L'\n') o += L"\\n";
        else if (c == L'\t') o += L"\\t";
        else if (c != L'\r') o += c;
    }
    return o;
}
static std::wstring Unesc(const std::wstring& t) {
    std::wstring o;
    for (size_t i = 0; i < t.size(); i++) {
        if (t[i] == L'\\' && i + 1 < t.size()) {
            wchar_t n = t[++i];
            o += n == L'n' ? L'\n' : n == L't' ? L'\t' : n;
        } else {
            o += t[i];
        }
    }
    return o;
}
static std::vector<std::wstring> SplitTabs(const std::wstring& line) {
    std::vector<std::wstring> v;
    size_t a = 0;
    for (;;) {
        size_t b = line.find(L'\t', a);
        v.push_back(line.substr(a, b == std::wstring::npos ? std::wstring::npos : b - a));
        if (b == std::wstring::npos) break;
        a = b + 1;
    }
    return v;
}
static std::vector<std::wstring> ReadLines(const wchar_t* key) {
    std::vector<std::wstring> out;
    LoadItems(key, L'\n', out);
    return out;
}

// (each note is one value, its line breaks escaped: no limit for the whole list, none lost on load)
static void SaveNotes() {
    std::vector<std::wstring> lines;
    for (auto& n : g_notes) lines.push_back(std::to_wstring(n.id) + L"\t" + std::to_wstring(n.when) + L"\t" + Esc(n.text));
    StoreItems(L"notes.list", lines);
}
static void SaveRems() {
    std::vector<std::wstring> lines;
    for (auto& r : g_rems) {
        int fl = (r.onOpen ? 1 : 0) | (r.late ? 2 : 0) | (r.fired ? 4 : 0) | (r.missed ? 8 : 0);
        lines.push_back(std::to_wstring(r.id) + L"\t" + std::to_wstring(r.due) + L"\t" + std::to_wstring(fl) + L"\t" + std::to_wstring(r.firedAt) +
                        L"\t" + Esc(r.text));
    }
    StoreItems(L"rems.list", lines);
}
static void LoadNotes() {
    g_notes.clear();
    for (auto& line : ReadLines(L"notes.list")) {
        auto f = SplitTabs(line);
        if (f.size() < 3) continue;
        Note n;
        n.id = _wtoi64(f[0].c_str());
        n.when = _wtoi64(f[1].c_str());
        n.text = Unesc(f[2]);
        g_notes.push_back(n);
    }
    g_rems.clear();
    for (auto& line : ReadLines(L"rems.list")) {
        auto f = SplitTabs(line);
        if (f.size() < 5) continue;
        Reminder r;
        r.id = _wtoi64(f[0].c_str());
        r.due = _wtoi64(f[1].c_str());
        int fl = _wtoi(f[2].c_str());
        r.onOpen = fl & 1;
        r.late = (fl & 2) != 0;
        r.fired = (fl & 4) != 0;
        r.missed = (fl & 8) != 0;
        r.firedAt = _wtoi64(f[3].c_str());
        r.text = Unesc(f[4]);
        g_rems.push_back(r);
    }
}

// Local date/time <-> FILETIME (UTC).
static long long LocalToFt(const SYSTEMTIME& local) {
    SYSTEMTIME utc;
    FILETIME ft;
    if (!TzSpecificLocalTimeToSystemTime(nullptr, &local, &utc) || !SystemTimeToFileTime(&utc, &ft)) return 0;
    return ((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}
static SYSTEMTIME FtToLocal(long long v) {
    FILETIME f = {(DWORD)v, (DWORD)(v >> 32)};
    SYSTEMTIME utc = {}, loc = {};
    FileTimeToSystemTime(&f, &utc);
    SystemTimeToTzSpecificLocalTime(nullptr, &utc, &loc);
    return loc;
}
static std::wstring HourText(const SYSTEMTIME& st) {
    wchar_t b[32] = {};
    GetTimeFormatEx(g_locale, TIME_NOSECONDS, &st, nullptr, b, ARRAYSIZE(b));
    return b;
}
// "Hoje", "Amanhã" or the date.
static std::wstring DayText(const SYSTEMTIME& st) {
    SYSTEMTIME now;
    GetLocalTime(&now);
    SYSTEMTIME a = st, b = now;
    a.wHour = a.wMinute = a.wSecond = a.wMilliseconds = 0;
    b.wHour = b.wMinute = b.wSecond = b.wMilliseconds = 0;
    long long d = (LocalToFt(a) - LocalToFt(b)) / (1440 * kFtMinute);
    if (d == 0) return L2(L"Hoje", L"Today");
    if (d == 1) return L2(L"Amanhã", L"Tomorrow");
    if (d == -1) return L2(L"Ontem", L"Yesterday");
    wchar_t buf[64] = {};
    GetDateFormatEx(g_locale, 0, &st, L"dd/MM", buf, ARRAYSIZE(buf), nullptr);
    return buf;
}

static long long g_remSince = 0;  // when Nimbus started watching reminders (before that: "late")
static void RemFire(Reminder& r, long long now) {
    SYSTEMTIME due = FtToLocal(r.due);
    std::wstring body = r.text;
    if (!r.onOpen && now - r.due > 2 * kFtMinute)
        body += L"\n" + std::wstring(L2(L"Era pra ", L"It was for ")) + DayText(due) + L", " + HourText(due) + L2(L" (o PC estava desligado)", L" (the PC was off)");
    r.fired = true;
    r.firedAt = now;
    std::wstring id = std::to_wstring(r.id);
    NotifPush(L2(L"Lembrete", L"Reminder"), body,
              {{L2(L"Concluir", L"Done"), L"rem-done:" + id, L"nimbus"}, {L2(L"Adiar 1 h", L"Snooze 1 h"), L"rem-snooze:" + id, L"nimbus"}},
              !g_set.notifShow && !g_set.notifDnd /* (a reminder shows even with the cards off) */, true);
}

// Each second: due reminders become notifications (never over a game: they wait for it to end).
static void RemTick() {
    if (g_rems.empty()) return;
    QUERY_USER_NOTIFICATION_STATE qs = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&qs)) && qs != QUNS_ACCEPTS_NOTIFICATIONS) return;
    long long now = NowFt();
    if (!g_remSince) g_remSince = now;
    bool changed = false;
    for (auto& r : g_rems) {
        if (r.fired || r.missed || now < r.due) continue;
        // Late (the PC or Nimbus was off at that time): tell anyway, unless the user said not to. Held
        // back by a game or the lock screen while Nimbus ran isn't late: it shows now.
        if (!r.onOpen && !r.late && r.due < g_remSince - 2 * kFtMinute) r.missed = true;
        else RemFire(r, now);
        changed = true;
    }
    if (changed) {
        SaveRems();
        StatusChanged();
    }
}

static void RemAction(const std::wstring& args) {
    bool done = !args.compare(0, 9, L"rem-done:"), snooze = !args.compare(0, 11, L"rem-snooze:");
    if (!done && !snooze) return;
    long long id = _wtoi64(args.c_str() + (done ? 9 : 11));
    for (size_t i = 0; i < g_rems.size(); i++) {
        if (g_rems[i].id != id) continue;
        if (done) {
            g_rems.erase(g_rems.begin() + i);
        } else {
            g_rems[i].due = NowFt() + 60 * kFtMinute;
            g_rems[i].onOpen = false;
            g_rems[i].fired = g_rems[i].missed = false;
        }
        break;
    }
    SaveRems();
    StatusChanged();
}

// ---- The text box (a real Windows edit box over the panel's field: the panel itself is a picture) ----

constexpr UINT WM_APP_NOTE_SAVE = WM_APP + 14;
constexpr UINT WM_APP_NOTE_CANCEL = WM_APP + 15;
constexpr UINT WM_APP_NOTE_TIME = WM_APP + 16;  // a time was typed: the chips follow

struct NoteEdit {
    HWND host = nullptr, edit = nullptr;
    HFONT font = nullptr;
    bool multi = false, time = false, want = false;
    D2D1_RECT_F rc{};  // in the panel, DIPs
    std::wstring initial;
};
NoteEdit g_ned;    // the note's or the reminder's text
NoteEdit g_ntime;  // the reminder's time (digits only, formatted as you type: 1632 -> 16:32)
HBRUSH g_nedBg;

// "16:32" from up to 4 digits; false when a digit can't be there (25:00, 16:75).
static bool TimeDigitsOk(const std::wstring& d) {
    if (d.size() >= 1 && d[0] > L'2') return false;
    if (d.size() >= 2 && d[0] == L'2' && d[1] > L'3') return false;
    if (d.size() >= 3 && d[2] > L'5') return false;
    return d.size() <= 4;
}
static std::wstring TimeFormat(const std::wstring& d) { return d.size() <= 2 ? d : d.substr(0, 2) + L":" + d.substr(2); }
static std::wstring TimeDigits(HWND e) {
    wchar_t b[16] = {};
    GetWindowTextW(e, b, ARRAYSIZE(b));
    std::wstring d;
    for (wchar_t* c = b; *c; c++)
        if (*c >= L'0' && *c <= L'9') d += *c;
    return d;
}
// Minutes from the time box, or -2 when it isn't a full time.
static int TimeBoxMinutes() {
    if (!g_ntime.edit) return -2;
    std::wstring d = TimeDigits(g_ntime.edit);
    if (d.size() == 3) d = L"0" + d;  // "932" -> 09:32
    if (d.size() != 4 || !TimeDigitsOk(d)) return -2;
    return _wtoi(d.substr(0, 2).c_str()) * 60 + _wtoi(d.substr(2).c_str());
}

static LRESULT CALLBACK NoteEditKeys(HWND h, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR ref) {
    NoteEdit& e = *(NoteEdit*)ref;
    if (msg == WM_KEYDOWN && wp == VK_ESCAPE) {
        PostMessageW(g_fly.hwnd, WM_APP_NOTE_CANCEL, 0, 0);
        return 0;
    }
    if (msg == WM_KEYDOWN && wp == VK_RETURN && (!e.multi || (GetKeyState(VK_CONTROL) & 0x8000))) {
        PostMessageW(g_fly.hwnd, WM_APP_NOTE_SAVE, 0, 0);
        return 0;
    }
    if (msg == WM_KEYDOWN && wp == VK_TAB) {
        // Between the reminder's text and its time.
        NoteEdit& other = &e == &g_ned ? g_ntime : g_ned;
        if (other.host) {
            SetForegroundWindow(other.host);
            SetFocus(other.edit);
            SendMessageW(other.edit, EM_SETSEL, 0, -1);
        }
        return 0;
    }
    if (msg == WM_CHAR && (wp == L'\r' || wp == 27 || wp == 10 || wp == L'\t') && (!e.multi || wp != L'\r' || (GetKeyState(VK_CONTROL) & 0x8000)))
        return 0;
    if (msg == WM_KEYDOWN && wp == 'A' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        SendMessageW(h, EM_SETSEL, 0, -1);
        return 0;
    }
    if (e.time) {
        // Digits only, up to the minute, formatted as you type.
        if (msg == WM_PASTE) return 0;
        if (msg == WM_CHAR) {
            std::wstring d = TimeDigits(h);
            DWORD a = 0, b = 0;
            SendMessageW(h, EM_GETSEL, (WPARAM)&a, (LPARAM)&b);
            if (b > a) d.clear();  // (typing over a selection starts again)
            if (wp == 8) {
                if (!d.empty()) d.pop_back();
            } else if (wp >= L'0' && wp <= L'9') {
                std::wstring n = d + (wchar_t)wp;
                if (!TimeDigitsOk(n)) {
                    MessageBeep(MB_OK);
                    return 0;
                }
                d = n;
            } else {
                return 0;
            }
            std::wstring t = TimeFormat(d);
            SetWindowTextW(h, t.c_str());
            SendMessageW(h, EM_SETSEL, t.size(), t.size());
            PostMessageW(g_fly.hwnd, WM_APP_NOTE_TIME, 0, 0);
            return 0;
        }
    }
    return DefSubclassProc(h, msg, wp, lp);
}

static LRESULT CALLBACK NoteHostProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CTLCOLOREDIT) {
        SetTextColor((HDC)wp, RGB(0xEC, 0xEC, 0xEE));
        SetBkColor((HDC)wp, RGB(0x1C, 0x1C, 0x21));
        return (LRESULT)g_nedBg;
    }
    if (msg == WM_ERASEBKGND) {
        RECT r;
        GetClientRect(h, &r);
        FillRect((HDC)wp, &r, g_nedBg);
        return 1;
    }
    if (msg == WM_SETFOCUS) {
        HWND e = h == g_ned.host ? g_ned.edit : h == g_ntime.host ? g_ntime.edit : nullptr;
        if (e) SetFocus(e);
        return 0;
    }
    // Clicked away (not on the panel or the other box): the panel closes, as it would.
    if (msg == WM_ACTIVATE && LOWORD(wp) == WA_INACTIVE && (HWND)lp != g_fly.hwnd && (HWND)lp != g_ned.host && (HWND)lp != g_ntime.host &&
        g_fly.open && !g_fly.fromStart)
        PostMessageW(g_fly.hwnd, WM_APP_FLY_OUTSIDE, 0, 0);
    return DefWindowProcW(h, msg, wp, lp);
}

static void NoteEditHide(NoteEdit& e) {
    if (e.host) DestroyWindow(e.host);
    e.host = e.edit = nullptr;
    e.want = false;
    if (e.font) DeleteObject(e.font);
    e.font = nullptr;
}
static void NoteEditHideAll() {
    NoteEditHide(g_ned);
    NoteEditHide(g_ntime);
}

static std::wstring NoteEditText(NoteEdit& e) {
    if (!e.edit) return e.initial;
    int n = GetWindowTextLengthW(e.edit);
    std::wstring t(n, L'\0');
    if (n) GetWindowTextW(e.edit, t.data(), n + 1);
    size_t a = t.find_first_not_of(L" \r\n\t"), b = t.find_last_not_of(L" \r\n\t");
    return a == std::wstring::npos ? L"" : t.substr(a, b - a + 1);
}

// After each frame of the panel: the box follows its field (or goes away).
static void NoteEditPlace(NoteEdit& e, bool focus) {
    auto& f = g_fly;
    if (!e.want || !f.open || f.anim < 1) {
        if (!e.want && e.host) NoteEditHide(e);
        return;
    }
    float k = f.scale;
    int padX = (int)std::lround(10 * k), padY = (int)std::lround((e.multi ? 9 : 8) * k);
    int x = f.rc.left + (int)std::lround(e.rc.left * k) + padX, y = f.rc.top + (int)std::lround(e.rc.top * k) + padY;
    int w = (int)std::lround((e.rc.right - e.rc.left) * k) - padX * 2, h = (int)std::lround((e.rc.bottom - e.rc.top) * k) - padY * 2;
    if (!e.host) {
        static bool reg = false;
        if (!reg) {
            WNDCLASSEXW wc = {sizeof(wc)};
            wc.lpfnWndProc = NoteHostProc;
            wc.hInstance = g_module;
            wc.hCursor = LoadCursorW(nullptr, IDC_IBEAM);
            wc.lpszClassName = L"NimbusMenu_NoteEdit";
            RegisterClassExW(&wc);
            reg = true;
        }
        if (!g_nedBg) g_nedBg = CreateSolidBrush(RGB(0x1C, 0x1C, 0x21));
        e.host = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"NimbusMenu_NoteEdit", L"", WS_POPUP, x, y, w, h, f.hwnd, nullptr, g_module,
                                 nullptr);
        // (No scroll bar: it would be a light one; the text scrolls with the caret.)
        DWORD es = WS_CHILD | WS_VISIBLE | (e.multi ? ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN : ES_AUTOHSCROLL) | (e.time ? ES_CENTER : 0);
        e.edit = CreateWindowExW(0, L"EDIT", e.initial.c_str(), es, 0, 0, w, h, e.host, nullptr, g_module, nullptr);
        e.font = CreateFontW(-(int)std::lround(13.5f * k), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                             L"Segoe UI Variable Text");
        SendMessageW(e.edit, WM_SETFONT, (WPARAM)e.font, TRUE);
        SendMessageW(e.edit, EM_SETLIMITTEXT, e.time ? 5 : 4000, 0);
        SetWindowSubclass(e.edit, NoteEditKeys, 1, (DWORD_PTR)&e);
        ShowWindow(e.host, SW_SHOWNA);
        if (focus) {
            SetForegroundWindow(e.host);
            SetFocus(e.edit);
            SendMessageW(e.edit, EM_SETSEL, e.initial.size(), e.initial.size());
        }
    } else {
        SetWindowPos(e.host, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
        MoveWindow(e.edit, 0, 0, w, h, TRUE);
    }
}
static void NoteEditPlaceAll() {
    bool first = !g_ned.host;
    NoteEditPlace(g_ned, first);
    NoteEditPlace(g_ntime, false);
}

// The panel asks for a box this frame.
static void NoteEditWant(NoteEdit& e, D2D1_RECT_F r, bool multi) {
    if (!FDraw()) return;
    e.want = true;
    e.multi = multi;
    e.rc = r;
}

struct NotesUi {
    int tab = 0;                // 0 notes, 1 reminders
    long long editing = -1;     // note being edited: -1 none, 0 a new one
    bool creating = false;      // the reminder form
    long long editRem = 0;      // the reminder being changed (0: a new one)
    int when = 0;               // 1 day, 7, 14, a month, the calendar
    int calY = 0, calM = 0, calD = 0;
    int minutes = 9 * 60;       // the reminder's time; -1 = as soon as Nimbus runs
    bool late = true;
    bool autoOpened = false;    // (the empty Notes tab opens a new note once)
    std::wstring draft;         // the reminder's text while the form shows
    DWORD emptyAt = 0;          // "Save" with nothing written: a hint shows for a moment
} g_notesUi;

static void NotesSave(bool closing = false) {
    std::wstring t = NoteEditText(g_ned);
    auto& u = g_notesUi;
    bool blank = t.find_first_not_of(L" \t\r\n") == std::wstring::npos;
    if (blank && !closing && ((u.tab == 0 && u.editing >= 0) || (u.tab == 1 && u.creating))) {
        u.emptyAt = GetTickCount();
        if (g_ned.edit) SetFocus(g_ned.edit);
        FlyRender();
        return;
    }
    if (blank) t.clear();
    if (u.tab == 0 && u.editing >= 0) {
        if (!t.empty()) {
            if (u.editing == 0) {
                Note n;
                n.id = NowFt();
                n.when = n.id;
                n.text = t;
                g_notes.insert(g_notes.begin(), n);
            } else {
                for (auto& n : g_notes)
                    if (n.id == u.editing) {
                        n.text = t;
                        n.when = NowFt();
                    }
            }
            SaveNotes();
        }
        u.editing = -1;
    } else if (u.tab == 1 && u.creating) {
        int typed = TimeBoxMinutes();
        if (typed >= 0) u.minutes = typed;
        if (!t.empty()) {
            SYSTEMTIME d;
            GetLocalTime(&d);
            if (u.when == 4 && u.calY) {
                d.wYear = (WORD)u.calY;
                d.wMonth = (WORD)u.calM;
                d.wDay = (WORD)u.calD;
            } else {
                d.wHour = d.wMinute = d.wSecond = d.wMilliseconds = 0;
                long long base = LocalToFt(d);
                if (u.when == 3) {
                    d.wMonth = d.wMonth == 12 ? 1 : d.wMonth + 1;
                    if (d.wMonth == 1) d.wYear++;
                    int dim = (d.wMonth == 2) ? ((d.wYear % 4 == 0 && (d.wYear % 100 || d.wYear % 400 == 0)) ? 29 : 28)
                              : (d.wMonth == 4 || d.wMonth == 6 || d.wMonth == 9 || d.wMonth == 11) ? 30 : 31;
                    if (d.wDay > dim) d.wDay = (WORD)dim;
                } else {
                    int days = u.when == 0 ? 1 : u.when == 1 ? 7 : 14;
                    d = FtToLocal(base + days * 1440 * kFtMinute + 120 * kFtMinute);  // (+2 h: daylight saving changes)
                }
            }
            Reminder r;
            r.id = u.editRem ? u.editRem : NowFt();
            r.text = t;
            r.onOpen = u.minutes < 0;
            r.late = u.late;
            d.wHour = (WORD)(r.onOpen ? 0 : u.minutes / 60);
            d.wMinute = (WORD)(r.onOpen ? 0 : u.minutes % 60);
            d.wSecond = d.wMilliseconds = 0;
            r.due = LocalToFt(d);
            if (u.editRem)
                g_rems.erase(std::remove_if(g_rems.begin(), g_rems.end(), [&](const Reminder& x) { return x.id == u.editRem; }), g_rems.end());
            g_rems.push_back(r);  // (fired and missed start again: a changed reminder tells again)
            std::sort(g_rems.begin(), g_rems.end(), [](const Reminder& a, const Reminder& b) { return a.due < b.due; });
            SaveRems();
        }
        u.creating = false;
        u.editRem = 0;
    }
    NoteEditHideAll();
    StatusChanged();
    FlyRender();
}

static void NotesCancel() {
    g_notesUi.editing = -1;
    g_notesUi.creating = false;
    g_notesUi.editRem = 0;
    NoteEditHideAll();
    FlyRender();
}

// A small round-ended choice.
static float FChip(float x, float y, const std::wstring& label, bool on, std::function<void()> fn) {
    float w = TextSize(label, g_fly.fSmall.Get()).width + 22;
    D2D1_RECT_F r = D2D1::RectF(x, y, x + w, y + 30);
    if (on) FFill(r, 15, FInk());
    else {
        if (FHot(r)) FFill(r, 15, FHov());
        if (FDraw()) g_fly.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + .5f, r.top + .5f, r.right - .5f, r.bottom - .5f), 14.5f, 14.5f),
                                                    FBr(FCol(g_theme.text, 0.16f)), 1);
    }
    FText(label, g_fly.fSmall.Get(), r.left, r.top, w, 30, on ? FSolidTint() : FCol(g_theme.text, 0.85f), DWRITE_TEXT_ALIGNMENT_CENTER);
    FHitAdd(r, std::move(fn));
    return w;
}

static float FWrapText(const std::wstring& s, float x, float top, float w, float maxH, D2D1_COLOR_F c) {
    ComPtr<IDWriteTextLayout> l;
    if (s.empty() || !g_fly.fWrap || FAILED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), g_fly.fWrap.Get(), w, maxH, &l))) return 0;
    DWRITE_TEXT_METRICS m = {};
    l->GetMetrics(&m);
    float h = std::min(m.height, maxH);
    if (FDraw()) {
        g_fly.rt->PushAxisAlignedClip(D2D1::RectF(x, top, x + w, top + h), D2D1_ANTIALIAS_MODE_ALIASED);
        g_fly.rt->DrawTextLayout(D2D1::Point2F(x, top), l.Get(), FBr(c));
        g_fly.rt->PopAxisAlignedClip();
    }
    return h;
}

static float FSwitch(float y, const wchar_t* label, bool on, std::function<void()> flip) {
    FText(label, g_fly.fSmall.Get(), kFlyPad, y, 270, 24, FSub());
    D2D1_RECT_F sw = D2D1::RectF(kFlyW - kFlyPad - 40, y + 1, kFlyW - kFlyPad, y + 23);
    FFill(sw, 11, on ? FInk() : FCol(g_theme.text, 0.18f));
    if (FDraw())
        g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(on ? sw.right - 11 : sw.left + 11, (sw.top + sw.bottom) / 2), 8, 8),
                              FBr(on ? FSolidTint() : FInk()));
    FHitAdd(sw, std::move(flip));
    return y + 28;
}

static std::wstring MinutesText(int m) {
    SYSTEMTIME st = {};
    st.wYear = 2026;
    st.wMonth = st.wDay = 1;
    st.wHour = (WORD)(m / 60);
    st.wMinute = (WORD)(m % 60);
    return HourText(st);
}

static float PanelNotes(float y) {
    auto& u = g_notesUi;
    // Tabs.
    {
        D2D1_RECT_F box = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 36);
        FFill(box, 10, FCol(g_theme.hover, 0.5f));
        const wchar_t* names[2] = {L2(L"Notas", L"Notes"), L2(L"Lembretes", L"Reminders")};
        float tw = (box.right - box.left - 6) / 2;
        for (int i = 0; i < 2; i++) {
            D2D1_RECT_F t = D2D1::RectF(box.left + 3 + i * tw, box.top + 3, box.left + 3 + (i + 1) * tw, box.bottom - 3);
            if (u.tab == i) FFill(t, 8, FHov());
            FText(names[i], g_fly.fSmall.Get(), t.left, t.top, t.right - t.left, t.bottom - t.top, u.tab == i ? FInk() : FSub(),
                  DWRITE_TEXT_ALIGNMENT_CENTER);
            FHitAdd(t, [i] {
                if (g_notesUi.tab != i) NotesCancel();
                g_notesUi.tab = i;
            });
        }
        y += 36 + 12;
    }
    SYSTEMTIME now;
    GetLocalTime(&now);
    if (u.tab == 0) {
        if (g_notes.empty() && u.editing < 0 && !u.autoOpened) {
            u.autoOpened = true;
            u.editing = 0;
            g_ned.initial.clear();
        }
        if (u.editing >= 0) {
            D2D1_RECT_F r = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 110);
            FFill(r, 10, D2D1::ColorF(0x1C1C21));
            if (FDraw()) g_fly.rt->DrawRoundedRectangle(D2D1::RoundedRect(r, 10, 10), FBr(FCol(g_theme.text, 0.3f)), 1);
            NoteEditWant(g_ned, r, true);
            y += 110 + 8;
            FButton(D2D1::RectF(kFlyW - kFlyPad - 196, y, kFlyW - kFlyPad - 104, y + 34), L2(L"Cancelar", L"Cancel"), [] { NotesCancel(); });
            FButton(D2D1::RectF(kFlyW - kFlyPad - 96, y, kFlyW - kFlyPad, y + 34), L2(L"Salvar", L"Save"), [] { NotesSave(); });
            y += 34 + 4;
            bool hint = g_notesUi.emptyAt && GetTickCount() - g_notesUi.emptyAt < 3000;
            FText(hint ? L2(L"Escreva alguma coisa pra salvar a nota.", L"Write something to save the note.")
                       : L2(L"Ctrl+Enter salva · Esc cancela", L"Ctrl+Enter saves · Esc cancels"),
                  g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 22, hint ? D2D1::ColorF(0.95f, 0.45f, 0.4f) : FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
            y += 22 + 8;
        } else {
            FLink(y, L2(L"+ Nova nota", L"+ New note"), [] {
                g_notesUi.editing = 0;
                g_ned.initial.clear();
            });
            y += 4;
        }
        int shown = 0;
        for (auto& n : g_notes) {
            if (shown++ >= 8) break;
            if (n.id == u.editing) continue;
            float th = 0;
            {
                bool saveDraw = g_fly.measuring;
                g_fly.measuring = true;
                th = FWrapText(n.text, 0, 0, kFlyW - kFlyPad * 2 - 34, 78, FInk());
                g_fly.measuring = saveDraw;
            }
            float h = 12 + th + 22;
            D2D1_RECT_F r = D2D1::RectF(kFlyPad - 4, y, kFlyW - kFlyPad + 4, y + h);
            bool hot = FHot(r);
            FFill(r, 12, hot ? FHov() : FCol(g_theme.hover, 0.45f));
            FWrapText(n.text, r.left + 12, y + 10, r.right - r.left - 44, 78, FInk());
            SYSTEMTIME w = FtToLocal(n.when);
            FText(DayText(w) + L", " + HourText(w), g_fly.fSmall.Get(), r.left + 12, y + 12 + th + 2, 200, 18, FSub());
            long long id = n.id;
            std::wstring text = n.text;
            FHitAdd(r, [id, text] {
                g_notesUi.editing = id;
                g_ned.initial = text;
            });
            if (hot) {
                D2D1_RECT_F xr = D2D1::RectF(r.right - 30, y + 6, r.right - 8, y + 28);
                if (FIn(xr)) FFill(xr, 11, FHov());
                FIcon(L"M8 8l8 8 M16 8l-8 8", (xr.left + xr.right) / 2, (xr.top + xr.bottom) / 2, 12, FInk(), 1.8f);
                FHitAdd(xr, [id] {
                    g_notes.erase(std::remove_if(g_notes.begin(), g_notes.end(), [&](const Note& x) { return x.id == id; }), g_notes.end());
                    SaveNotes();
                    StatusChanged();
                });
            }
            y += h + 6;
        }
        if (g_notes.empty() && u.editing < 0) {
            FText(L2(L"Nenhuma nota ainda.", L"No notes yet."), g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 30, FSub(), DWRITE_TEXT_ALIGNMENT_CENTER);
            y += 30;
        }
        FSep(y);
        FText(L2(L"As notas ficam salvas até você apagar, mesmo desligando o PC.", L"Notes stay until you delete them, even with the PC off."),
              g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 22, FSub());
        y += 22;
        return y;
    }
    // Reminders.
    if (u.creating) {
        D2D1_RECT_F r = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 38);
        FFill(r, 10, D2D1::ColorF(0x1C1C21));
        if (FDraw()) g_fly.rt->DrawRoundedRectangle(D2D1::RoundedRect(r, 10, 10), FBr(FCol(g_theme.text, 0.3f)), 1);
        NoteEditWant(g_ned, r, false);
        y += 38 + 12;
        FText(L2(L"Quando", L"When"), g_fly.fSmall.Get(), kFlyPad, y, 200, 20, FSub());
        y += 24;
        const wchar_t* whens[5] = {L2(L"1 dia", L"1 day"), L2(L"7 dias", L"7 days"), L2(L"14 dias", L"14 days"), L2(L"1 mês", L"1 month"),
                                   L2(L"Calendário", L"Calendar")};
        float x = kFlyPad;
        for (int i = 0; i < 5; i++) {
            float w = TextSize(whens[i], g_fly.fSmall.Get()).width + 22;
            if (x + w > kFlyW - kFlyPad) {
                x = kFlyPad;
                y += 36;
            }
            x += FChip(x, y, whens[i], u.when == i, [i] {
                g_notesUi.when = i;
                if (i == 4 && !g_notesUi.calY) {
                    SYSTEMTIME t;
                    GetLocalTime(&t);
                    g_notesUi.calY = t.wYear;
                    g_notesUi.calM = t.wMonth;
                    g_notesUi.calD = t.wDay;
                }
            }) + 6;
        }
        y += 36 + 4;
        if (u.when == 4 && u.calY) {
            // Month and days.
            SYSTEMTIME first = {};
            first.wYear = (WORD)u.calY;
            first.wMonth = (WORD)u.calM;
            first.wDay = 1;
            wchar_t mb[64] = {};
            GetDateFormatEx(g_locale, DATE_YEARMONTH, &first, nullptr, mb, ARRAYSIZE(mb), nullptr);
            FText(mb, g_fly.fCode.Get(), kFlyPad, y, 200, 28, FInk());
            auto nav = [&](float cx, const wchar_t* path, int dir) {
                D2D1_RECT_F nr = D2D1::RectF(cx - 14, y, cx + 14, y + 28);
                if (FHot(nr)) FFill(nr, 8, FHov());
                FIcon(path, cx, y + 14, 14, FInk());
                FHitAdd(nr, [dir] {
                    auto& v = g_notesUi;
                    v.calM += dir;
                    if (v.calM < 1) { v.calM = 12; v.calY--; }
                    if (v.calM > 12) { v.calM = 1; v.calY++; }
                    v.calD = 1;
                });
            };
            nav(kFlyW - kFlyPad - 46, L"M15 6l-6 6 6 6", -1);
            nav(kFlyW - kFlyPad - 14, L"M9 6l6 6-6 6", 1);
            y += 32;
            FILETIME ft;
            SystemTimeToFileTime(&first, &ft);
            FileTimeToSystemTime(&ft, &first);  // (fills the weekday)
            int lead = first.wDayOfWeek;
            int dim = (u.calM == 2) ? ((u.calY % 4 == 0 && (u.calY % 100 || u.calY % 400 == 0)) ? 29 : 28)
                      : (u.calM == 4 || u.calM == 6 || u.calM == 9 || u.calM == 11) ? 30 : 31;
            float cw = (kFlyW - kFlyPad * 2) / 7;
            for (int i = 0; i < lead + dim; i++) {
                if (i < lead) continue;
                int day = i - lead + 1;
                float cx = kFlyPad + (i % 7) * cw + cw / 2, cy = y + (i / 7) * 32 + 15;
                D2D1_RECT_F dr = D2D1::RectF(cx - 14, cy - 14, cx + 14, cy + 14);
                bool past = u.calY < now.wYear || (u.calY == now.wYear && (u.calM < now.wMonth || (u.calM == now.wMonth && day < now.wDay)));
                bool sel = day == u.calD;
                if (sel) FFill(dr, 14, FInk());
                else if (!past && FHot(dr)) FFill(dr, 14, FHov());
                FText(std::to_wstring(day), g_fly.fSmall.Get(), dr.left, dr.top, 28, 28, sel ? FSolidTint() : past ? FCol(g_theme.text, 0.3f) : FInk(),
                      DWRITE_TEXT_ALIGNMENT_CENTER);
                if (!past) FHitAdd(dr, [day] { g_notesUi.calD = day; });
            }
            y += ((lead + dim + 6) / 7) * 32 + 6;
        }
        FText(L2(L"Horário", L"Time"), g_fly.fSmall.Get(), kFlyPad, y, 200, 20, FSub());
        y += 24;
        x = kFlyPad;
        // (a chip clears the typed time)
        for (int m : {9 * 60, 12 * 60, 18 * 60})
            x += FChip(x, y, MinutesText(m), u.minutes == m, [m] {
                     g_notesUi.minutes = m;
                     if (g_ntime.edit) SetWindowTextW(g_ntime.edit, L"");
                 }) + 6;
        x += FChip(x, y, L2(L"Ao abrir o Nimbus", L"When Nimbus runs"), u.minutes < 0, [] {
                 g_notesUi.minutes = -1;
                 if (g_ntime.edit) SetWindowTextW(g_ntime.edit, L"");
             }) + 6;
        y += 36;
        {
            // Any other time, typed (digits only; 1632 becomes 16:32).
            FText(L2(L"Outro horário:", L"Other time:"), g_fly.fSmall.Get(), kFlyPad, y, 110, 34, FSub());
            D2D1_RECT_F tr = D2D1::RectF(kFlyPad + 112, y, kFlyPad + 200, y + 34);
            FFill(tr, 9, D2D1::ColorF(0x1C1C21));
            bool full = TimeBoxMinutes() >= 0;
            if (FDraw())
                g_fly.rt->DrawRoundedRectangle(D2D1::RoundedRect(tr, 9, 9), FBr(FCol(g_theme.text, full ? 0.45f : 0.22f)), 1);
            if (!g_ntime.host) {
                bool preset = u.minutes < 0 || u.minutes == 9 * 60 || u.minutes == 12 * 60 || u.minutes == 18 * 60;
                g_ntime.initial = preset ? L"" : TimeFormat(std::to_wstring(u.minutes / 60 + 100).substr(1) + std::to_wstring(u.minutes % 60 + 100).substr(1));
            }
            g_ntime.time = true;
            NoteEditWant(g_ntime, tr, false);
            FText(L2(L"ex.: 16:32", L"e.g. 16:32"), g_fly.fSmall.Get(), tr.right + 10, y, 120, 34, FCol(g_theme.text, 0.4f));
            y += 34 + 10;
        }
        y = FSwitch(y, L2(L"Avisar mesmo se o PC estava desligado na hora", L"Still tell me if the PC was off at that time"), u.late,
                    [] { g_notesUi.late = !g_notesUi.late; });
        y += 4;
        FButton(D2D1::RectF(kFlyW - kFlyPad - 236, y, kFlyW - kFlyPad - 144, y + 34), L2(L"Cancelar", L"Cancel"), [] { NotesCancel(); });
        FButton(D2D1::RectF(kFlyW - kFlyPad - 136, y, kFlyW - kFlyPad, y + 34),
                u.editRem ? L2(L"Salvar", L"Save") : L2(L"Criar lembrete", L"Add reminder"), [] { NotesSave(); });
        y += 34 + 4;
        if (u.emptyAt && GetTickCount() - u.emptyAt < 3000) {
            FText(L2(L"Escreva o lembrete antes de salvar.", L"Write the reminder before saving."), g_fly.fSmall.Get(), kFlyPad, y,
                  kFlyW - kFlyPad * 2, 22, D2D1::ColorF(0.95f, 0.45f, 0.4f), DWRITE_TEXT_ALIGNMENT_TRAILING);
            y += 22;
        }
        y += 6;
    } else {
        FLink(y, L2(L"+ Novo lembrete", L"+ New reminder"), [] {
            g_notesUi.creating = true;
            g_notesUi.editRem = 0;
            g_ned.initial.clear();
        });
        y += 4;
    }
    int shown = 0;
    for (auto& r : g_rems) {
        if (u.creating && r.id == u.editRem) continue;  // (it's in the form)
        if (shown++ >= 8) break;
        SYSTEMTIME d = FtToLocal(r.due);
        std::wstring when = DayText(d) + L", " + (r.onOpen ? std::wstring(L2(L"quando o Nimbus abrir", L"when Nimbus runs")) : HourText(d));
        bool lateFired = r.fired && !r.onOpen && r.firedAt - r.due > 2 * kFtMinute;
        if (r.missed) when += L2(L" · perdido (o PC estava desligado)", L" · missed (the PC was off)");
        else if (lateFired) when += std::wstring(L2(L" · avisado atrasado às ", L" · told late at ")) + HourText(FtToLocal(r.firedAt));
        else if (r.fired) when += L2(L" · avisado", L" · told");
        D2D1_RECT_F rr = D2D1::RectF(kFlyPad - 4, y, kFlyW - kFlyPad + 4, y + 54);
        bool hot = FHot(rr);
        FFill(rr, 12, hot ? FHov() : FCol(g_theme.hover, 0.45f));
        FText(r.text, g_fly.fText.Get(), rr.left + 12, y + 8, rr.right - rr.left - 44, 0, (r.fired || r.missed) ? FSub() : FInk());
        FText(when, g_fly.fSmall.Get(), rr.left + 12, y + 30, rr.right - rr.left - 24, 0,
              (lateFired || r.missed) ? D2D1::ColorF(0xE4B262) : FSub());
        long long id = r.id;
        // A click changes it: the form opens with its text, day and time.
        if (!u.creating) {
            Reminder copy = r;
            FHitAdd(rr, [copy] {
                auto& v = g_notesUi;
                v.creating = true;
                v.editRem = copy.id;
                g_ned.initial = copy.text;
                SYSTEMTIME dd = FtToLocal(copy.due);
                v.when = 4;
                v.calY = dd.wYear;
                v.calM = dd.wMonth;
                v.calD = dd.wDay;
                v.minutes = copy.onOpen ? -1 : dd.wHour * 60 + dd.wMinute;
                v.late = copy.late;
            });
        }
        if (hot) {
            D2D1_RECT_F xr = D2D1::RectF(rr.right - 30, y + 6, rr.right - 8, y + 28);
            if (FIn(xr)) FFill(xr, 11, FHov());
            FIcon(L"M8 8l8 8 M16 8l-8 8", (xr.left + xr.right) / 2, (xr.top + xr.bottom) / 2, 12, FInk(), 1.8f);
            FHitAdd(xr, [id] {
                g_rems.erase(std::remove_if(g_rems.begin(), g_rems.end(), [&](const Reminder& x) { return x.id == id; }), g_rems.end());
                SaveRems();
                StatusChanged();
            });
        }
        y += 54 + 6;
    }
    if (g_rems.empty() && !u.creating) {
        FText(L2(L"Nenhum lembrete.", L"No reminders."), g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 30, FSub(), DWRITE_TEXT_ALIGNMENT_CENTER);
        y += 30;
    }
    FSep(y);
    FText(L2(L"No dia, o aviso chega como notificação do Nimbus.", L"On the day, it comes as a Nimbus notification."), g_fly.fSmall.Get(), kFlyPad,
          y, kFlyW - kFlyPad * 2, 22, FSub());
    y += 22;
    return y;
}

// ---- Control center and its items ----

static void ToggleFlyout(Dock& d, int kind, bool fromStart);
static void SwSetPage(int p);

// ---- Bluetooth ----
// Everything runs on a worker thread (the radio, the device list, the batteries and connecting can
// each take a while); the panel draws what it found last.
//   - The radio: Windows.Devices.Radios (what Windows' quick settings use), on and off.
//   - Paired devices: bthprops (name, address, connected).
//   - Connecting a headset or speaker: the Bluetooth audio driver's one-shot reconnect/disconnect,
//     asked on the device's audio (kernel streaming) interface. Other devices only show their state.
//   - Battery: the level Windows' Settings shows, a property of the device's node (BTHENUM/BTHLE).
static HMODULE BtLib() {
    static HMODULE m = LoadLibraryExW(L"bthprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    return m;
}

struct BtDevice {
    std::wstring name, addr;  // addr: 12 hex digits, upper case
    bool connected = false, audio = false;
    int battery = -1;
};

struct BtState {
    SRWLOCK lock = SRWLOCK_INIT;
    HANDLE thread = nullptr, ev = nullptr;
    bool stop = false;
    // asked by the UI
    bool scan = false;
    int wantRadio = -1;
    std::wstring wantConnect, wantDisconnect;
    // found by the worker
    int radio = -1;  // -1 no radio, 0 off, 1 on
    std::vector<BtDevice> devs;
    std::wstring busy;  // an address being connected or disconnected, or "radio"
    bool scanned = false;
};
static BtState g_bts;
static HANDLE g_btQuit;  // (manual reset: ends the worker's pauses at once)
static DWORD g_btAskAt = 0;
constexpr UINT WM_APP_BTREAD = WM_APP + 19;

static int BtRadio(int set) {
    using namespace winrt::Windows::Devices::Radios;
    try {
        static bool asked = false;
        if (!asked) {
            Radio::RequestAccessAsync().get();
            asked = true;
        }
        for (auto const& r : Radio::GetRadiosAsync().get()) {
            if (r.Kind() != RadioKind::Bluetooth) continue;
            if (set >= 0) {
                RadioAccessStatus st = r.SetStateAsync(set ? RadioState::On : RadioState::Off).get();
                Wh_Log(L"bluetooth: radio %s (%d)", set ? L"on" : L"off", (int)st);
                if (st == RadioAccessStatus::Allowed) return set;
            }
            return r.State() == RadioState::On ? 1 : 0;
        }
    } catch (...) {
        Wh_Log(L"bluetooth: radio call failed");
    }
    return -1;
}

static std::vector<BtDevice> BtList() {
    std::vector<BtDevice> v;
    using First_t = HBLUETOOTH_DEVICE_FIND(WINAPI*)(const BLUETOOTH_DEVICE_SEARCH_PARAMS*, BLUETOOTH_DEVICE_INFO*);
    using Next_t = BOOL(WINAPI*)(HBLUETOOTH_DEVICE_FIND, BLUETOOTH_DEVICE_INFO*);
    using Close_t = BOOL(WINAPI*)(HBLUETOOTH_DEVICE_FIND);
    HMODULE m = BtLib();
    auto first = m ? (First_t)GetProcAddress(m, "BluetoothFindFirstDevice") : nullptr;
    auto next = m ? (Next_t)GetProcAddress(m, "BluetoothFindNextDevice") : nullptr;
    auto close = m ? (Close_t)GetProcAddress(m, "BluetoothFindDeviceClose") : nullptr;
    if (!first || !next || !close) return v;
    BLUETOOTH_DEVICE_SEARCH_PARAMS prm = {sizeof(prm)};
    prm.fReturnAuthenticated = TRUE;
    prm.fReturnRemembered = TRUE;
    prm.fReturnConnected = TRUE;
    BLUETOOTH_DEVICE_INFO info = {sizeof(info)};
    HBLUETOOTH_DEVICE_FIND f = first(&prm, &info);
    if (!f) return v;
    do {
        BtDevice d;
        d.name = info.szName;
        wchar_t a[16];
        swprintf_s(a, L"%012llX", (unsigned long long)info.Address.ullLong);
        d.addr = a;
        d.connected = info.fConnected != FALSE;
        if (std::none_of(v.begin(), v.end(), [&](const BtDevice& o) { return o.addr == d.addr; })) v.push_back(d);
        info.dwSize = sizeof(info);
    } while (v.size() < 16 && next(f, &info));
    close(f);
    return v;
}

static std::wstring Upper(std::wstring s) {
    for (auto& c : s) c = towupper(c);
    return s;
}

// The audio interfaces (kernel streaming) of a device, by its address in their instance id.
static const GUID kKsCategoryAudio = {0x6994AD04, 0x93EF, 0x11D0, {0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}};
static std::vector<std::wstring> BtAudioPaths(const std::wstring& addr) {
    std::vector<std::wstring> out;
    HDEVINFO set = SetupDiGetClassDevsW(&kKsCategoryAudio, nullptr, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (set == INVALID_HANDLE_VALUE) return out;
    SP_DEVICE_INTERFACE_DATA ifd = {sizeof(ifd)};
    for (DWORD k = 0; SetupDiEnumDeviceInterfaces(set, nullptr, &kKsCategoryAudio, k, &ifd); k++) {
        DWORD need = 0;
        SetupDiGetDeviceInterfaceDetailW(set, &ifd, nullptr, 0, &need, nullptr);
        if (!need) continue;
        std::vector<BYTE> buf(need);
        auto* det = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)buf.data();
        det->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        SP_DEVINFO_DATA dev = {sizeof(dev)};
        if (!SetupDiGetDeviceInterfaceDetailW(set, &ifd, det, need, nullptr, &dev)) continue;
        wchar_t id[512];
        if (!SetupDiGetDeviceInstanceIdW(set, &dev, id, ARRAYSIZE(id), nullptr)) continue;
        std::wstring sid = Upper(id);
        if (sid.rfind(L"BTHENUM\\", 0) == 0 && sid.find(addr) != std::wstring::npos) out.push_back(det->DevicePath);
    }
    SetupDiDestroyDeviceInfoList(set);
    return out;
}

// One-shot reconnect (0) or disconnect (1) of a Bluetooth audio device.
static bool BtAudioOneShot(const std::wstring& addr, ULONG what) {
    struct KsProp {
        GUID set;
        ULONG id, flags;
    };
    static const GUID kBtAudio = {0x7FA06C40, 0xB8F6, 0x4C7E, {0x85, 0x56, 0xE8, 0xC3, 0x3A, 0x12, 0xE5, 0x4D}};
    const DWORD kIoctlKsProperty = CTL_CODE(0x0000002F /* FILE_DEVICE_KS */, 0x000, METHOD_NEITHER, FILE_ANY_ACCESS);
    bool ok = false;
    for (auto& path : BtAudioPaths(addr)) {
        HANDLE h = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) continue;
        KsProp prop = {kBtAudio, what, 1 /* KSPROPERTY_TYPE_GET */};
        DWORD got = 0;
        if (DeviceIoControl(h, kIoctlKsProperty, &prop, sizeof(prop), nullptr, 0, &got, nullptr)) ok = true;
        CloseHandle(h);
    }
    Wh_Log(L"bluetooth: %s %s: %s", what ? L"disconnect" : L"connect", addr.c_str(), ok ? L"ok" : L"failed");
    return ok;
}

// Battery levels and which devices have audio, by address.
static void BtDetails(std::vector<BtDevice>& devs) {
    static const DEVPROPKEY kBattery = {{0x104EA319, 0x6EE2, 0x4701, {0xBD, 0x47, 0x8D, 0xDB, 0xF4, 0x25, 0xBB, 0xE5}}, 2};
    for (const wchar_t* en : {L"BTHENUM", L"BTHLE"}) {
        HDEVINFO set = SetupDiGetClassDevsW(nullptr, en, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
        if (set == INVALID_HANDLE_VALUE) continue;
        SP_DEVINFO_DATA dev = {sizeof(dev)};
        for (DWORD k = 0; SetupDiEnumDeviceInfo(set, k, &dev); k++) {
            wchar_t id[512];
            if (!SetupDiGetDeviceInstanceIdW(set, &dev, id, ARRAYSIZE(id), nullptr)) continue;
            std::wstring sid = Upper(id);
            for (auto& d : devs) {
                if (sid.find(d.addr) == std::wstring::npos) continue;
                DEVPROPTYPE type = 0;
                BYTE level = 0;
                if (SetupDiGetDevicePropertyW(set, &dev, &kBattery, &type, &level, sizeof(level), nullptr, 0) && type == DEVPROP_TYPE_BYTE &&
                    level <= 100)
                    d.battery = level;
                // (the A2DP sink / hands-free services: it plays sound)
                if (sid.find(L"{0000110B-") != std::wstring::npos || sid.find(L"{0000111E-") != std::wstring::npos) d.audio = true;
            }
        }
        SetupDiDestroyDeviceInfoList(set);
    }
}

static DWORD WINAPI BtWorker(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    for (;;) {
        WaitForSingleObject(g_bts.ev, INFINITE);
        AcquireSRWLockExclusive(&g_bts.lock);
        if (g_bts.stop) {
            ReleaseSRWLockExclusive(&g_bts.lock);
            break;
        }
        int wantRadio = g_bts.wantRadio;
        std::wstring con = g_bts.wantConnect, dis = g_bts.wantDisconnect;
        g_bts.wantRadio = -1;
        g_bts.wantConnect.clear();
        g_bts.wantDisconnect.clear();
        g_bts.scan = false;
        ReleaseSRWLockExclusive(&g_bts.lock);

        int radio = BtRadio(wantRadio);
        if (wantRadio >= 0) WaitForSingleObject(g_btQuit, 600);  // (the devices follow the radio)
        if (!con.empty()) BtAudioOneShot(con, 0);
        if (!dis.empty()) BtAudioOneShot(dis, 1);
        if (!con.empty() || !dis.empty()) WaitForSingleObject(g_btQuit, 1500);  // (connecting takes a moment to show)
        std::vector<BtDevice> devs = radio == 1 ? BtList() : std::vector<BtDevice>{};
        BtDetails(devs);
        std::stable_sort(devs.begin(), devs.end(), [](const BtDevice& a, const BtDevice& b) { return a.connected > b.connected; });

        AcquireSRWLockExclusive(&g_bts.lock);
        g_bts.radio = radio;
        g_bts.devs = std::move(devs);
        g_bts.busy.clear();
        g_bts.scanned = true;
        bool again = g_bts.scan || g_bts.wantRadio >= 0 || !g_bts.wantConnect.empty() || !g_bts.wantDisconnect.empty();
        ReleaseSRWLockExclusive(&g_bts.lock);
        if (g_msgWnd) PostMessageW(g_msgWnd, WM_APP_BTREAD, 0, 0);
        if (again) SetEvent(g_bts.ev);
    }
    winrt::uninit_apartment();
    return 0;
}

static void BtKick() {
    if (!g_btQuit) g_btQuit = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_bts.ev) g_bts.ev = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_bts.thread) g_bts.thread = CreateThread(nullptr, 0, BtWorker, nullptr, 0, nullptr);
    SetEvent(g_bts.ev);
}

// A fresh look at most every 4 s while something shows Bluetooth.
static void BtAsk() {
    if (g_btAskAt && GetTickCount() - g_btAskAt < 4000) return;
    g_btAskAt = GetTickCount();
    AcquireSRWLockExclusive(&g_bts.lock);
    g_bts.scan = true;
    ReleaseSRWLockExclusive(&g_bts.lock);
    BtKick();
}

static void BtRequest(int radio, const std::wstring& connect, const std::wstring& disconnect) {
    AcquireSRWLockExclusive(&g_bts.lock);
    if (radio >= 0) {
        g_bts.wantRadio = radio;
        g_bts.busy = L"radio";
    }
    if (!connect.empty()) g_bts.wantConnect = g_bts.busy = connect;
    if (!disconnect.empty()) g_bts.wantDisconnect = g_bts.busy = disconnect;
    ReleaseSRWLockExclusive(&g_bts.lock);
    BtKick();
}


// Joins a worker until a shared deadline. A worker that didn't end keeps its handles (it may still use
// them; the process exits right after).
static bool JoinWorker(HANDLE& thread, HANDLE& ev, DWORD deadline) {
    if (!thread) return true;
    DWORD now = GetTickCount();
    DWORD left = (LONG)(deadline - now) > 0 ? deadline - now : 0;
    if (WaitForSingleObject(thread, left) != WAIT_OBJECT_0) return false;
    CloseHandle(thread);
    if (ev) CloseHandle(ev);
    thread = ev = nullptr;
    return true;
}
static void BtSignalStop() {
    if (!g_bts.thread) return;
    AcquireSRWLockExclusive(&g_bts.lock);
    g_bts.stop = true;
    ReleaseSRWLockExclusive(&g_bts.lock);
    if (g_btQuit) SetEvent(g_btQuit);
    SetEvent(g_bts.ev);
}
static void BtJoin(DWORD deadline) {
    if (JoinWorker(g_bts.thread, g_bts.ev, deadline)) {
        g_bts.stop = false;
        if (g_btQuit) {
            CloseHandle(g_btQuit);
            g_btQuit = nullptr;
        }
    }
}

// What the panel needs (a copy, under the lock).
struct BtView {
    int radio = -1;
    std::vector<BtDevice> devs;
    std::wstring busy;
    bool scanned = false;
};
static BtView BtNow() {
    BtAsk();
    BtView v;
    AcquireSRWLockShared(&g_bts.lock);
    v.radio = g_bts.radio;
    v.devs = g_bts.devs;
    v.busy = g_bts.busy;
    v.scanned = g_bts.scanned;
    ReleaseSRWLockShared(&g_bts.lock);
    return v;
}

static bool BtPresent() {
    BtAsk();
    AcquireSRWLockShared(&g_bts.lock);
    bool on = g_bts.radio >= 0;
    ReleaseSRWLockShared(&g_bts.lock);
    return on;
}

// Night light. Windows has no call for it: Settings keeps its state in the registry (CloudStore) as a
// small binary record, and Windows applies a newer record written there. Layout:
//   43 42 01 00 0A 02 01 00 2A 06 | stamp (seconds since 1970, 7 bits a byte, high bit = more) |
//   2A 2B 0E | n | 43 42 01 00 [10 00 only when on] ... (n bytes) | the rest
// The record is checked before and after the change; anything else is left alone (and the switch just
// opens Windows' page). Newer builds also keep a per-device record: both are changed together.
static const wchar_t kCloudStore[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\CloudStore\\Store\\DefaultAccount\\Current";

struct NightRec {
    std::wstring sub;  // under kCloudStore
    std::vector<BYTE> d;
    size_t stamp = 0, stampLen = 0, lenAt = 0;
    bool on = false;
};

static bool NightParse(NightRec& r) {
    static const BYTE head[] = {0x43, 0x42, 0x01, 0x00, 0x0A, 0x02, 0x01, 0x00, 0x2A, 0x06};
    const auto& d = r.d;
    if (d.size() < 30 || memcmp(d.data(), head, sizeof(head))) return false;
    size_t k = sizeof(head);
    r.stamp = k;
    while (k < d.size() && (d[k] & 0x80) && k - r.stamp < 9) k++;
    if (k >= d.size() || (d[k] & 0x80)) return false;
    r.stampLen = ++k - r.stamp;
    if (k + 4 > d.size() || d[k] != 0x2A || d[k + 1] != 0x2B || d[k + 2] != 0x0E) return false;
    r.lenAt = k + 3;
    size_t st = r.lenAt + 1;
    if (d[r.lenAt] < 6 || st + d[r.lenAt] > d.size() || d[st] != 0x43 || d[st + 1] != 0x42 || d[st + 2] != 0x01 || d[st + 3] != 0x00) return false;
    r.on = d[st + 4] == 0x10 && d[st + 5] == 0x00;
    return true;
}

static std::vector<NightRec> NightRecords() {
    std::vector<NightRec> out;
    HKEY store;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kCloudStore, 0, KEY_READ, &store) != ERROR_SUCCESS) return out;
    const std::wstring state = L"windows.data.bluelightreduction.bluelightreductionstate";
    wchar_t name[300];
    for (DWORD k = 0;; k++) {
        DWORD n = ARRAYSIZE(name);
        if (RegEnumKeyExW(store, k, name, &n, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
        std::wstring key(name, n);
        size_t dollar = key.find(L'$');
        if (dollar == std::wstring::npos) continue;
        std::wstring val = key.substr(dollar + 1);
        if (val != state && val != state + L"perdevice") continue;
        NightRec r;
        r.sub = key + L"\\" + val;
        DWORD size = 0;
        if (RegGetValueW(store, r.sub.c_str(), L"Data", RRF_RT_REG_BINARY, nullptr, nullptr, &size) != ERROR_SUCCESS || !size || size > 4096)
            continue;
        r.d.resize(size);
        if (RegGetValueW(store, r.sub.c_str(), L"Data", RRF_RT_REG_BINARY, nullptr, r.d.data(), &size) != ERROR_SUCCESS) continue;
        r.d.resize(size);
        if (NightParse(r)) out.push_back(std::move(r));
    }
    RegCloseKey(store);
    return out;
}

// 1 on, 0 off, -1 unknown.
static int NightRead() {
    int v = -1;
    for (auto& r : NightRecords())
        if (v < 0 || r.sub.find(L"perdevice") != std::wstring::npos) v = r.on ? 1 : 0;
    return v;
}

// The control center's slow reads (night light: a big registry key; the power mode; the Bluetooth
// radio) run on a worker thread at most every 2 s. A frame only reads what it found last (reading on
// the UI thread made the panel stutter).
constexpr UINT WM_APP_CCREAD = WM_APP + 18;
static std::atomic<int> g_nightA{-1}, g_powerA{1};
static std::atomic<DWORD> g_ccClickAt{0};  // (a click just set a value: reads older than it are dropped)
static std::atomic<bool> g_ccBusy{false};
static DWORD g_ccAt = 0;
static DWORD WINAPI CcWorker(void*) {
    DWORD began = GetTickCount();
    int night = NightRead(), power = PowerMode();
    if ((LONG)(g_ccClickAt.load() - began) >= 0 && g_ccClickAt.load()) {  // (a click came during the read: its value stays)
        g_ccBusy = false;
        return 0;
    }
    bool changed = night != g_nightA || power != g_powerA;
    g_nightA = night;
    g_powerA = power;
    g_ccBusy = false;
    if (changed && g_msgWnd) PostMessageW(g_msgWnd, WM_APP_CCREAD, 0, 0);
    return 0;
}
static void CcRefresh() {
    if (g_ccBusy || (g_ccAt && GetTickCount() - g_ccAt < 2000)) return;
    g_ccAt = GetTickCount();
    g_ccBusy = true;
    if (HANDLE t = CreateThread(nullptr, 0, CcWorker, nullptr, 0, nullptr)) CloseHandle(t);
    else g_ccBusy = false;
}
static int NightState() {
    CcRefresh();
    return g_nightA;
}

static bool NightSet(bool on) {
    auto recs = NightRecords();
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    unsigned long long now = ((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) - 116444736000000000ULL) / 10000000ULL;
    bool done = false;
    for (auto& r : recs) {
        auto& d = r.d;
        unsigned long long was = 0;
        for (size_t k = 0; k < r.stampLen && k < 9; k++) was |= (unsigned long long)(d[r.stamp + k] & 0x7F) << (7 * k);
        // (a newer stamp than the one Windows has, or it ignores the record)
        unsigned long long v = std::max(now, was + 1);
        std::vector<BYTE> stamp;
        do {
            BYTE b = v & 0x7F;
            v >>= 7;
            stamp.push_back(v ? (BYTE)(b | 0x80) : b);
        } while (v);
        size_t st = r.lenAt + 1;
        if (on && !r.on) {
            d.insert(d.begin() + st + 4, {0x10, 0x00});
            d[r.lenAt] += 2;
        } else if (!on && r.on) {
            d.erase(d.begin() + st + 4, d.begin() + st + 6);
            d[r.lenAt] -= 2;
        }
        d.erase(d.begin() + r.stamp, d.begin() + r.stamp + r.stampLen);
        d.insert(d.begin() + r.stamp, stamp.begin(), stamp.end());
        NightRec check;
        check.d = d;
        if (!NightParse(check) || check.on != on) continue;  // (not as expected: left alone)
        if (RegSetKeyValueW(HKEY_CURRENT_USER, (std::wstring(kCloudStore) + L"\\" + r.sub).c_str(), L"Data", REG_BINARY, d.data(),
                            (DWORD)d.size()) == ERROR_SUCCESS)
            done = true;
    }
    if (done) {
        g_nightA = on ? 1 : 0;
        g_ccClickAt = GetTickCount();
    }
    Wh_Log(L"night light: %s (%d records, %s)", on ? L"on" : L"off", (int)recs.size(), done ? L"written" : L"not changed");
    return done;
}

// The power mode, as the worker last read it.
static int PowerModeCached() {
    CcRefresh();
    return g_powerA;
}

static void CfgSetQuiet(const std::wstring& name, const std::wstring& v);
static void SetDnd(bool on) {
    g_set.notifDnd = on;
    // (saved without reloading everything: a reload rebuilds the docks and closes the open panel)
    CfgSetQuiet(L"notifDnd", on ? L"1" : L"0");
    StatusChanged();
}

// Win+Ctrl+L (live captions) and the like.
static void SendWinCtrlKey(WORD vk) {
    INPUT in[6] = {};
    for (auto& i : in) i.type = INPUT_KEYBOARD;
    in[0].ki.wVk = VK_LWIN;
    in[1].ki.wVk = VK_CONTROL;
    in[2].ki.wVk = vk;
    in[3].ki.wVk = vk;
    in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    in[4].ki.wVk = VK_CONTROL;
    in[4].ki.dwFlags = KEYEVENTF_KEYUP;
    in[5].ki.wVk = VK_LWIN;
    in[5].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(6, in, sizeof(INPUT));
}

// ---- Brightness: each monitor's own (DDC/CI), on a worker thread (a monitor can take 50 ms to answer) ----

constexpr UINT WM_APP_BRIGHT = WM_APP + 17;  // lParam = std::vector<BrView>* (the receiver deletes it)
struct BrView {
    std::wstring name;
    bool ok = false;
    int pct = 0;
};
std::vector<BrView> g_brView;
bool g_brScanned = false;
struct BrShared {
    SRWLOCK lock = SRWLOCK_INIT;
    std::vector<int> want;
    bool scan = false, stop = false;
    HANDLE ev = nullptr, thread = nullptr;
} g_brs;

struct PhysMon {
    HANDLE h;
    WCHAR desc[128];
};

static DWORD WINAPI BrightThread(void*) {
    HMODULE m = LoadLibraryExW(L"dxva2.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    using Num_t = BOOL(WINAPI*)(HMONITOR, DWORD*);
    using Get_t = BOOL(WINAPI*)(HMONITOR, DWORD, PhysMon*);
    using Destroy_t = BOOL(WINAPI*)(DWORD, PhysMon*);
    using GetB_t = BOOL(WINAPI*)(HANDLE, DWORD*, DWORD*, DWORD*);
    using SetB_t = BOOL(WINAPI*)(HANDLE, DWORD);
    auto num = m ? (Num_t)GetProcAddress(m, "GetNumberOfPhysicalMonitorsFromHMONITOR") : nullptr;
    auto get = m ? (Get_t)GetProcAddress(m, "GetPhysicalMonitorsFromHMONITOR") : nullptr;
    auto destroy = m ? (Destroy_t)GetProcAddress(m, "DestroyPhysicalMonitors") : nullptr;
    auto getB = m ? (GetB_t)GetProcAddress(m, "GetMonitorBrightness") : nullptr;
    auto setB = m ? (SetB_t)GetProcAddress(m, "SetMonitorBrightness") : nullptr;
    struct One {
        std::vector<PhysMon> phys;
        DWORD mn = 0, mx = 100;
        bool ok = false;
    };
    std::vector<One> mons;
    auto release = [&] {
        for (auto& o : mons)
            if (destroy && !o.phys.empty()) destroy((DWORD)o.phys.size(), o.phys.data());
        mons.clear();
    };
    for (;;) {
        WaitForSingleObject(g_brs.ev, INFINITE);
        AcquireSRWLockExclusive(&g_brs.lock);
        bool stop = g_brs.stop, scan = g_brs.scan;
        std::vector<int> want = g_brs.want;
        for (auto& w : g_brs.want) w = -1;
        g_brs.scan = false;
        ReleaseSRWLockExclusive(&g_brs.lock);
        if (stop) break;
        if (scan && num && get && getB) {
            release();
            std::vector<HMONITOR> hm;
            EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR h, HDC, LPRECT, LPARAM lp) WINAPI -> BOOL {
                ((std::vector<HMONITOR>*)lp)->push_back(h);
                return TRUE;
            }, (LPARAM)&hm);
            auto* views = new std::vector<BrView>();
            for (HMONITOR h : hm) {
                One o;
                DWORD n = 0;
                if (num(h, &n) && n) {
                    o.phys.resize(n);
                    if (!get(h, n, o.phys.data())) o.phys.clear();
                }
                DWORD cur = 0;
                o.ok = !o.phys.empty() && getB(o.phys[0].h, &o.mn, &cur, &o.mx) && o.mx > o.mn;
                BrView v;
                v.name = MonitorIdentity(h).name;
                if (v.name.empty() && !o.phys.empty()) v.name = o.phys[0].desc;
                v.ok = o.ok;
                v.pct = o.ok ? (int)((cur - o.mn) * 100 / (o.mx - o.mn)) : 0;
                views->push_back(v);
                mons.push_back(std::move(o));
            }
            if (!PostPtr(g_msgWnd, WM_APP_BRIGHT, views)) delete views;
        }
        // (many monitors save each value to their memory: a drag writes ~4 times a second, the last
        // value always arrives)
        static std::vector<int> written;
        if (written.size() < mons.size()) written.resize(mons.size(), -1);
        bool wrote = false;
        for (size_t k = 0; k < want.size() && k < mons.size(); k++)
            if (want[k] >= 0 && want[k] != written[k] && mons[k].ok && setB) {
                for (auto& ph : mons[k].phys) setB(ph.h, mons[k].mn + (mons[k].mx - mons[k].mn) * (DWORD)want[k] / 100);
                written[k] = want[k];
                wrote = true;
            }
        if (scan) std::fill(written.begin(), written.end(), -1);
        if (wrote) Sleep(250);
    }
    release();
    return 0;
}

static void BrightKick() {
    if (!g_brs.ev) g_brs.ev = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_brs.thread) g_brs.thread = CreateThread(nullptr, 0, BrightThread, nullptr, 0, nullptr);
    SetEvent(g_brs.ev);
}
static void BrightScan() {
    AcquireSRWLockExclusive(&g_brs.lock);
    g_brs.scan = true;
    ReleaseSRWLockExclusive(&g_brs.lock);
    BrightKick();
}
static void BrightSet(size_t i, int pct) {
    if (i >= g_brView.size()) return;
    g_brView[i].pct = pct;
    AcquireSRWLockExclusive(&g_brs.lock);
    if (g_brs.want.size() < g_brView.size()) g_brs.want.resize(g_brView.size(), -1);
    g_brs.want[i] = pct;
    ReleaseSRWLockExclusive(&g_brs.lock);
    BrightKick();
}
static void BrightSignalStop() {
    if (!g_brs.thread) return;
    AcquireSRWLockExclusive(&g_brs.lock);
    g_brs.stop = true;
    ReleaseSRWLockExclusive(&g_brs.lock);
    SetEvent(g_brs.ev);
}
static void BrightJoin(DWORD deadline) {
    if (JoinWorker(g_brs.thread, g_brs.ev, deadline)) g_brs.stop = false;
}

// ---- Wireless batteries ----
// Bluetooth levels come from the Bluetooth worker. This one reads Xbox controllers (XInput) every 30 s
// and, when BatteryHub (formerly Wireless Battery Monitor, krisgfx9) is installed, runs it with
// "--json" every 5 minutes: it reads USB receivers (Logitech, Razer...) that Windows doesn't. Nothing
// of its code is here: it's a separate program, asked like on a command line.
struct DevState {
    SRWLOCK lock = SRWLOCK_INIT;
    HANDLE thread = nullptr, ev = nullptr;
    bool stop = false, on = false;
    std::vector<DevBattery> xbox, hub;
    std::wstring hubExe;
    bool hubTried = false;
    DWORD hubAt = 0;
};
static DevState g_devs;
constexpr UINT WM_APP_DEVREAD = WM_APP + 21;

// BatteryHub.exe, version 1.1 or newer (the first with "--json"; older versions would just start hidden).
static bool BatteryHubOk(const std::wstring& path) {
    if (_wcsicmp(PathFindFileNameW(path.c_str()), L"BatteryHub.exe") != 0) return false;
    DWORD h = 0, n = GetFileVersionInfoSizeW(path.c_str(), &h);
    if (!n) return false;
    std::vector<BYTE> data(n);
    VS_FIXEDFILEINFO* fi = nullptr;
    UINT len = 0;
    if (!GetFileVersionInfoW(path.c_str(), 0, n, data.data()) || !VerQueryValueW(data.data(), L"\\", (void**)&fi, &len) || !fi) return false;
    WORD major = HIWORD(fi->dwFileVersionMS), minor = LOWORD(fi->dwFileVersionMS);
    return major > 1 || (major == 1 && minor >= 1);
}
static std::wstring BatteryHubExe() {
    wchar_t buf[MAX_PATH];
    std::vector<std::wstring> cands;
    // (its "start with Windows" entry points at wherever it is, the portable one too)
    DWORD size = sizeof(buf);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"WirelessBattery", RRF_RT_REG_SZ, nullptr, buf, &size) ==
        ERROR_SUCCESS) {
        std::wstring v = buf;
        if (!v.empty() && v[0] == L'"') v = v.substr(1, v.find(L'"', 1) - 1);
        cands.push_back(v);
    }
    if (ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Programs\\BatteryHub\\BatteryHub.exe", buf, MAX_PATH)) cands.push_back(buf);
    if (ExpandEnvironmentStringsW(IsWow64() ? L"%ProgramW6432%\\BatteryHub\\BatteryHub.exe" : L"%ProgramFiles%\\BatteryHub\\BatteryHub.exe", buf, MAX_PATH))
        cands.push_back(buf);
    for (auto& c : cands)
        if (GetFileAttributesW(c.c_str()) != INVALID_FILE_ATTRIBUTES && BatteryHubOk(c)) return c;
    return L"";
}


// A small reader for its one-line JSON: an array of flat objects (strings, numbers, booleans).
static std::vector<DevBattery> ParseHubJson(const std::string& j) {
    std::vector<DevBattery> out;
    size_t i = 0;
    auto ws = [&] {
        while (i < j.size() && isspace((unsigned char)j[i])) i++;
    };
    auto str = [&]() -> std::wstring {
        std::string o;
        if (i >= j.size() || j[i] != '"') return L"";
        i++;
        while (i < j.size() && j[i] != '"') {
            if (j[i] == '\\' && i + 1 < j.size()) {
                char c = j[++i];
                if (c == 'n') o += '\n';
                else if (c == 't') o += '\t';
                else if (c == 'u' && i + 4 < j.size()) {
                    unsigned v = strtoul(j.substr(i + 1, 4).c_str(), nullptr, 16);
                    i += 4;
                    if (v < 0x80) o += (char)v;
                    else if (v < 0x800) {
                        o += (char)(0xC0 | (v >> 6));
                        o += (char)(0x80 | (v & 63));
                    } else {
                        o += (char)(0xE0 | (v >> 12));
                        o += (char)(0x80 | ((v >> 6) & 63));
                        o += (char)(0x80 | (v & 63));
                    }
                } else o += c;
            } else o += j[i];
            i++;
        }
        i++;
        int n = MultiByteToWideChar(CP_UTF8, 0, o.c_str(), (int)o.size(), nullptr, 0);
        std::wstring w(n, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, o.c_str(), (int)o.size(), w.data(), n);
        return w;
    };
    ws();
    if (i >= j.size() || j[i] != '[') return out;
    i++;
    while (i < j.size()) {
        ws();
        if (j[i] == ']') break;
        if (j[i] == ',') {
            i++;
            continue;
        }
        if (j[i] != '{') break;
        i++;
        DevBattery d;
        bool connected = true;
        std::wstring level;
        while (i < j.size()) {
            ws();
            if (j[i] == '}') {
                i++;
                break;
            }
            if (j[i] == ',') {
                i++;
                continue;
            }
            std::wstring k = str();
            ws();
            if (i < j.size() && j[i] == ':') i++;
            ws();
            if (i < j.size() && j[i] == '"') {
                std::wstring v = str();
                if (k == L"key") d.key = L"hub:" + v;
                else if (k == L"name") d.name = v;
                else if (k == L"via") d.via = v;
                else if (k == L"kind") d.kind = v;
                else if (k == L"level") level = v;
            } else {
                size_t st = i;
                while (i < j.size() && j[i] != ',' && j[i] != '}') i++;
                std::string v = j.substr(st, i - st);
                while (!v.empty() && isspace((unsigned char)v.back())) v.pop_back();
                if (k == L"percent") d.percent = atoi(v.c_str());
                else if (k == L"charging") d.charging = v == "true";
                else if (k == L"connected") connected = v == "true";
            }
        }
        if (d.percent < 0 && !level.empty()) {
            std::wstring l = level;
            for (auto& c : l) c = towlower(c);
            d.percent = l.find(L"full") != std::wstring::npos ? 100 : l.find(L"high") != std::wstring::npos ? 80 : l.find(L"med") != std::wstring::npos ? 50
                      : l.find(L"low") != std::wstring::npos ? 20 : l.find(L"crit") != std::wstring::npos ? 5 : -1;
        }
        if (connected && !d.name.empty()) out.push_back(d);
    }
    return out;
}

static std::vector<DevBattery> RunBatteryHub(const std::wstring& exe) {
    std::vector<DevBattery> out;
    SECURITY_ATTRIBUTES sa = {sizeof(sa), nullptr, TRUE};
    HANDLE rd = nullptr, wr = nullptr;
    if (!CreatePipe(&rd, &wr, &sa, 0)) return out;
    SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = wr;
    si.hStdError = wr;
    PROCESS_INFORMATION pi = {};
    std::wstring cmd = L"\"" + exe + L"\" --json";
    std::wstring dir = exe.substr(0, exe.find_last_of(L'\\'));
    BOOL ok = CreateProcessW(exe.c_str(), cmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW | BELOW_NORMAL_PRIORITY_CLASS, nullptr, dir.c_str(), &si, &pi);
    CloseHandle(wr);
    if (!ok) {
        CloseHandle(rd);
        Wh_Log(L"batteries: BatteryHub didn't start (%u)", GetLastError());
        return out;
    }
    std::string text;
    DWORD start = GetTickCount();
    for (;;) {
        // (checked every turn: a program that never stops writing still ends here)
        AcquireSRWLockShared(&g_devs.lock);
        bool stopping = g_devs.stop;
        ReleaseSRWLockShared(&g_devs.lock);
        if (stopping || GetTickCount() - start > 20000 || text.size() > 256 * 1024) {
            TerminateProcess(pi.hProcess, 1);
            break;
        }
        DWORD avail = 0;
        if (PeekNamedPipe(rd, nullptr, 0, nullptr, &avail, nullptr) && avail) {
            char buf[4096];
            DWORD got = 0;
            if (ReadFile(rd, buf, std::min<DWORD>(avail, sizeof(buf)), &got, nullptr) && got) text.append(buf, got);
            continue;
        }
        if (WaitForSingleObject(pi.hProcess, 100) == WAIT_OBJECT_0) {
            DWORD got = 0;
            char buf[4096];
            while (text.size() < 256 * 1024 && PeekNamedPipe(rd, nullptr, 0, nullptr, &avail, nullptr) && avail &&
                   ReadFile(rd, buf, std::min<DWORD>(avail, sizeof(buf)), &got, nullptr) && got)
                text.append(buf, got);
            break;
        }
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(rd);
    size_t a = text.find('[');
    if (a != std::string::npos) out = ParseHubJson(text.substr(a));
    Wh_Log(L"batteries: BatteryHub gave %d devices", (int)out.size());
    return out;
}

static std::vector<DevBattery> ReadXbox() {
    std::vector<DevBattery> out;
    struct XState {
        DWORD packet;
        WORD buttons;
        BYTE lt, rt;
        SHORT lx, ly, rx, ry;
    };
    struct XBattery {
        BYTE type, level;
    };
    static HMODULE m = LoadLibraryExW(L"xinput1_4.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    using St = DWORD(WINAPI*)(DWORD, XState*);
    using Bt = DWORD(WINAPI*)(DWORD, BYTE, XBattery*);
    auto state = m ? (St)GetProcAddress(m, "XInputGetState") : nullptr;
    auto bat = m ? (Bt)GetProcAddress(m, "XInputGetBatteryInformation") : nullptr;
    if (!state || !bat) return out;
    for (DWORD u = 0; u < 4; u++) {
        XState st;
        if (state(u, &st) != ERROR_SUCCESS) continue;
        XBattery b = {};
        if (bat(u, 0 /* the controller */, &b) != ERROR_SUCCESS) continue;
        if (b.type == 0 /* disconnected */ || b.type == 1 /* wired */) continue;
        DevBattery d;
        d.key = L"xbox:" + std::to_wstring(u);
        d.name = std::wstring(L2(L"Controle ", L"Controller ")) + std::to_wstring(u + 1);
        d.via = L"Xbox";
        d.kind = L"gamepad";
        d.percent = b.level == 0 ? 5 : b.level == 1 ? 25 : b.level == 2 ? 60 : 100;
        out.push_back(d);
    }
    return out;
}

static DWORD WINAPI DevWorker(void*) {
    for (;;) {
        AcquireSRWLockShared(&g_devs.lock);
        bool on = g_devs.on;
        ReleaseSRWLockShared(&g_devs.lock);
        WaitForSingleObject(g_devs.ev, on ? 30000 : INFINITE);
        AcquireSRWLockExclusive(&g_devs.lock);
        bool stop = g_devs.stop;
        on = g_devs.on;
        bool tried = g_devs.hubTried;
        std::wstring exe = g_devs.hubExe;
        DWORD hubAt = g_devs.hubAt;
        ReleaseSRWLockExclusive(&g_devs.lock);
        if (stop) break;
        if (!on) continue;
        std::vector<DevBattery> xbox = ReadXbox(), hub;
        bool runHub = false;
        if (!tried) {
            exe = BatteryHubExe();
            tried = true;
        }
        if (g_devHubOn && !exe.empty() && (!hubAt || GetTickCount() - hubAt > 5 * 60 * 1000)) runHub = true;
        if (!g_devHubOn) hub.clear();
        if (runHub) hub = RunBatteryHub(exe);
        AcquireSRWLockExclusive(&g_devs.lock);
        g_devs.xbox = xbox;
        g_devs.hubTried = tried;
        g_devs.hubExe = exe;
        if (!g_devHubOn) g_devs.hub.clear();
        if (runHub) {
            g_devs.hub = hub;
            g_devs.hubAt = GetTickCount();
        }
        ReleaseSRWLockExclusive(&g_devs.lock);
        if (g_msgWnd) PostMessageW(g_msgWnd, WM_APP_DEVREAD, 0, 0);
    }
    return 0;
}

static void DevSetOn(bool on) {
    AcquireSRWLockExclusive(&g_devs.lock);
    bool was = g_devs.on;
    g_devs.on = on;
    ReleaseSRWLockExclusive(&g_devs.lock);
    if (!on && !was) return;
    if (!g_devs.ev) g_devs.ev = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_devs.thread) g_devs.thread = CreateThread(nullptr, 0, DevWorker, nullptr, 0, nullptr);
    SetEvent(g_devs.ev);
}

static void DevRefreshHub() {
    AcquireSRWLockExclusive(&g_devs.lock);
    g_devs.hubAt = 0;
    g_devs.hubTried = false;
    ReleaseSRWLockExclusive(&g_devs.lock);
    if (g_devs.ev) SetEvent(g_devs.ev);
}

static void DevSignalStop() {
    if (!g_devs.thread) return;
    AcquireSRWLockExclusive(&g_devs.lock);
    g_devs.stop = true;
    ReleaseSRWLockExclusive(&g_devs.lock);
    SetEvent(g_devs.ev);
}
static void DevJoin(DWORD deadline) {
    if (JoinWorker(g_devs.thread, g_devs.ev, deadline)) g_devs.stop = false;
}

// Everything known now: BatteryHub's first (it knows receivers), then Bluetooth, then Xbox, one per name.
static std::vector<DevBattery> DevicesNow() {
    std::vector<DevBattery> v;
    if (!ExtraPlace(StDevices)) return v;
    AcquireSRWLockShared(&g_devs.lock);
    v = g_devs.hub;
    std::vector<DevBattery> xbox = g_devs.xbox;
    ReleaseSRWLockShared(&g_devs.lock);
    auto has = [&](const std::wstring& name) {
        return std::any_of(v.begin(), v.end(), [&](const DevBattery& d) { return _wcsicmp(d.name.c_str(), name.c_str()) == 0; });
    };
    AcquireSRWLockShared(&g_bts.lock);
    for (auto& b : g_bts.devs)
        if (b.connected && b.battery >= 0 && !has(b.name)) {
            DevBattery d;
            d.key = L"bt:" + b.addr;
            d.name = b.name;
            d.via = L"Bluetooth";
            d.kind = b.audio ? L"headset" : L"other";
            d.percent = b.battery;
            v.push_back(d);
        }
    ReleaseSRWLockShared(&g_bts.lock);
    for (auto& x : xbox)
        if (!has(x.name)) v.push_back(x);
    return v;
}

static bool BatteryHubFound() {
    AcquireSRWLockShared(&g_devs.lock);
    bool f = !g_devs.hubExe.empty();
    ReleaseSRWLockShared(&g_devs.lock);
    return f;
}

// A notification when one drops to 20% and again at 10% (once each, until it charges).
static std::unordered_map<std::wstring, int> g_devWarned;
static void DevAlerts() {
    if (!CfgInt(L"devAlerts")) return;
    for (auto& d : DevicesNow()) {
        if (d.percent < 0) continue;
        int& w = g_devWarned[d.key];
        if (d.charging || d.percent > 25) {
            w = 0;
            continue;
        }
        int step = d.percent <= 10 ? 2 : d.percent <= 20 ? 1 : 0;
        if (step > w) {
            w = step;
            NotifPush(std::wstring(L2(L"Bateria fraca: ", L"Battery low: ")) + d.name,
                      std::to_wstring(d.percent) + L"%" + (d.via.empty() ? L"" : L" · " + d.via), {}, false);
        }
    }
}

static const wchar_t* DevIcon(const std::wstring& kind) {
    if (kind == L"mouse") return L"M12 3a5 5 0 0 1 5 5v8a5 5 0 0 1-10 0V8a5 5 0 0 1 5-5z M12 3v6 M7 10h10";
    if (kind == L"headset") return L"M4 14v-2a8 8 0 0 1 16 0v2 M4 14h3v6H4z M17 14h3v6h-3z";
    if (kind == L"keyboard") return L"M3 7h18v10H3z M7 11h.01 M11 11h.01 M15 11h.01 M8 14h8";
    if (kind == L"gamepad") return L"M7 8h10a4 4 0 0 1 4 4l-1 5a2 2 0 0 1-3.5 1L15 16H9l-1.5 2A2 2 0 0 1 4 17l-1-5a4 4 0 0 1 4-4z M8 11v3 M6.5 12.5h3 M15 12h.01 M17 13.5h.01";
    if (kind == L"phone") return L"M8 3h8v18H8z M11 18h2";
    return L"M6 7h11v10H6z M17 10h2v4h-2";
}

static float PanelDevices(float y) {
    BtAsk();
    auto devs = DevicesNow();
    FHeader(y, L2(L"Dispositivos", L"Devices"), BatteryHubFound() ? L"+ BatteryHub" : L"");
    if (devs.empty()) {
        FText(L2(L"Nenhum aparelho sem fio informando bateria agora.", L"No wireless device reporting a battery now."), g_fly.fSmall.Get(), kFlyPad, y,
              kFlyW - kFlyPad * 2, 22, FSub());
        y += 28;
    }
    for (auto& d : devs) {
        FIcon(DevIcon(d.kind), kFlyPad + 10, y + 22, 18, FInk());
        FText(d.name, g_fly.fText.Get(), kFlyPad + 30, y + 4, 200, 0, FInk());
        FText(d.via + (d.charging ? std::wstring(L2(L" · carregando", L" · charging")) : L""), g_fly.fSmall.Get(), kFlyPad + 30, y + 24, 200, 0, FSub());
        if (d.percent >= 0) {
            float bw = 64, bx = kFlyW - kFlyPad - 44 - bw - 8;
            D2D1_RECT_F bar = D2D1::RectF(bx, y + 20, bx + bw, y + 25);
            FFill(bar, 2.5f, FCol(g_theme.text, 0.15f));
            D2D1_COLOR_F c = d.percent <= 20 ? D2D1::ColorF(0.95f, 0.36f, 0.32f) : FInk();
            FFill(D2D1::RectF(bar.left, bar.top, bar.left + bw * d.percent / 100.0f, bar.bottom), 2.5f, c);
            FText(std::to_wstring(d.percent) + L"%", g_fly.fSmall.Get(), kFlyW - kFlyPad - 44, y + 12, 44, 20, FInk(), DWRITE_TEXT_ALIGNMENT_TRAILING);
        }
        y += 48;
    }
    FSep(y);
    y = FSwitch(y, L2(L"Avisar com bateria fraca (20% e 10%)", L"Warn on low battery (20% and 10%)"), CfgInt(L"devAlerts") != 0,
                [] { CfgSetQuiet(L"devAlerts", CfgInt(L"devAlerts") ? L"0" : L"1"); }) + 6;
    if (BatteryHubFound()) {
        y = FSwitch(y, L2(L"Usar o BatteryHub (receptores USB)", L"Use BatteryHub (USB receivers)"), CfgInt(L"devHub") != 0, [] {
                CfgSetQuiet(L"devHub", CfgInt(L"devHub") ? L"0" : L"1");
                g_devHubOn = CfgInt(L"devHub") != 0;
                DevRefreshHub();
            }) + 2;
        FText(L2(L"Ligado, o Nimbus abre o BatteryHub escondido a cada 5 minutos só pra ler as baterias (ele fecha sozinho).",
                 L"On, Nimbus runs BatteryHub hidden every 5 minutes just to read the batteries (it closes by itself)."),
              g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 36, FSub());
        y += 40;
        if (CfgInt(L"devHub")) FLink(y, L2(L"Ler o BatteryHub agora", L"Read BatteryHub now"), [] { DevRefreshHub(); });
    } else {
        FText(L2(L"Receptores USB (Logitech, Razer…) aparecem aqui com o BatteryHub instalado, um programa à parte e gratuito.",
                 L"USB receivers (Logitech, Razer…) show here with BatteryHub installed, a separate free program."),
              g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 36, FSub());
        y += 40;
        FLink(y, L2(L"Conhecer o BatteryHub", L"About BatteryHub"), [] {
            CloseFlyout();
            OpenUri(L"https://github.com/krisgfx9/Wireless-Device-Battery-Monitor");
        });
    }
    return y;
}

// ---- Player ----
static void FlyEnter(int kind);
// Windows' media controls (GlobalSystemMediaTransportControls: what the volume flyout and the lock
// screen show) on a worker thread: once a second while the player is on, and right away after a
// button. The panel and the dock draw what it read last.
struct MediaState {
    SRWLOCK lock = SRWLOCK_INIT;
    HANDLE thread = nullptr, ev = nullptr;
    bool stop = false, on = false;
    MediaInfo info;
    std::vector<BYTE> thumb;
    int cmd = 0;
    double arg = 0;
    std::wstring pick;  // the session chosen in the panel
};
static MediaState g_media;
constexpr UINT WM_APP_MEDIA = WM_APP + 20;  // wParam 1: what shows changed (else only the time)

static DWORD WINAPI MediaWorker(void*) {
    using namespace winrt::Windows::Media::Control;
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    std::wstring lastKey;
    std::vector<BYTE> thumb;
    int gen = 0;
    for (;;) {
        AcquireSRWLockShared(&g_media.lock);
        bool on = g_media.on;
        ReleaseSRWLockShared(&g_media.lock);
        AcquireSRWLockShared(&g_media.lock);
        bool any = !g_media.info.sessions.empty();
        ReleaseSRWLockShared(&g_media.lock);
        WaitForSingleObject(g_media.ev, !on ? INFINITE : any ? 1000 : 4000);
        AcquireSRWLockExclusive(&g_media.lock);
        if (g_media.stop) {
            ReleaseSRWLockExclusive(&g_media.lock);
            break;
        }
        int cmd = g_media.cmd;
        double arg = g_media.arg;
        std::wstring pick = g_media.pick;
        g_media.cmd = 0;
        on = g_media.on;
        MediaInfo old = g_media.info;
        ReleaseSRWLockExclusive(&g_media.lock);
        if (!on) continue;

        MediaInfo mi;
        try {
            if (!mgr) mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = mgr.GetSessions();
            GlobalSystemMediaTransportControlsSession cur{nullptr};
            for (auto const& se : sessions) {
                std::wstring id(se.SourceAppUserModelId().c_str());
                mi.sessions.push_back(id);
                if (!pick.empty() && id == pick) cur = se;
            }
            if (!cur) cur = mgr.GetCurrentSession();
            if (!cur && sessions.Size()) cur = sessions.GetAt(0);
            if (cur) {
                if (cmd) {
                    auto pb0 = cur.GetPlaybackInfo();
                    if (cmd == 1) cur.TryTogglePlayPauseAsync().get();
                    else if (cmd == 2) cur.TrySkipNextAsync().get();
                    else if (cmd == 3) cur.TrySkipPreviousAsync().get();
                    else if (cmd == 4) cur.TryChangeShuffleActiveAsync(!(pb0.IsShuffleActive() && pb0.IsShuffleActive().Value())).get();
                    else if (cmd == 5) {
                        int r = pb0.AutoRepeatMode() ? (int)pb0.AutoRepeatMode().Value() : 0;
                        cur.TryChangeAutoRepeatModeAsync((winrt::Windows::Media::MediaPlaybackAutoRepeatMode)((r + 1) % 3)).get();
                    } else if (cmd == 6)
                        cur.TryChangePlaybackPositionAsync((int64_t)((cur.GetTimelineProperties().StartTime().count() / 1e7 + arg) * 10000000.0)).get();
                    Sleep(200);  // (the app takes a moment to say what changed)
                }
                auto pb = cur.GetPlaybackInfo();
                auto c = pb.Controls();
                mi.canPrev = c.IsPreviousEnabled();
                mi.canNext = c.IsNextEnabled();
                mi.canToggle = c.IsPlayPauseToggleEnabled() || c.IsPlayEnabled() || c.IsPauseEnabled();
                mi.canShuffle = c.IsShuffleEnabled();
                mi.canRepeat = c.IsRepeatEnabled();
                mi.canSeek = c.IsPlaybackPositionEnabled();
                mi.status = pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing ? 1 : 2;
                if (auto sh = pb.IsShuffleActive()) mi.shuffle = sh.Value();
                if (auto rp = pb.AutoRepeatMode()) mi.repeat = (int)rp.Value();
                auto tl = cur.GetTimelineProperties();
                double start = tl.StartTime().count() / 1e7, end = tl.EndTime().count() / 1e7, pos = tl.Position().count() / 1e7;
                if (mi.status == 1) {
                    double age = (winrt::clock::now() - tl.LastUpdatedTime()).count() / 1e7;
                    if (age > 0 && age < 6 * 3600) pos += age;
                }
                mi.dur = std::max(0.0, end - start);
                mi.pos = std::clamp(pos - start, 0.0, mi.dur > 0 ? mi.dur : 1e9);
                mi.posAt = GetTickCount();
                auto props = cur.TryGetMediaPropertiesAsync().get();
                mi.title = props.Title().c_str();
                mi.artist = props.Artist().c_str();
                if (mi.artist.empty()) mi.artist = props.AlbumArtist().c_str();
                mi.aumid = cur.SourceAppUserModelId().c_str();
                mi.has = !mi.title.empty();
                std::wstring key = mi.aumid + L"|" + mi.title + L"|" + mi.artist;
                if (key != lastKey) {
                    lastKey = key;
                    thumb.clear();
                    if (auto th = props.Thumbnail()) {
                        auto st = th.OpenReadAsync().get();
                        uint32_t size = (uint32_t)st.Size();
                        if (size && size < 8 * 1024 * 1024) {
                            winrt::Windows::Storage::Streams::Buffer buf(size);
                            auto got = st.ReadAsync(buf, size, winrt::Windows::Storage::Streams::InputStreamOptions::None).get();
                            thumb.assign(got.data(), got.data() + got.Length());
                        }
                    }
                    gen++;
                }
            }
        } catch (...) {
            mgr = nullptr;  // (asked again next time)
        }
        mi.thumbGen = gen;
        bool shows = mi.has != old.has || mi.title != old.title || mi.artist != old.artist || mi.aumid != old.aumid || mi.status != old.status ||
                     mi.thumbGen != old.thumbGen || mi.sessions != old.sessions || mi.shuffle != old.shuffle || mi.repeat != old.repeat ||
                     mi.canPrev != old.canPrev || mi.canNext != old.canNext;
        AcquireSRWLockExclusive(&g_media.lock);
        g_media.info = mi;
        if (shows) g_media.thumb = thumb;
        ReleaseSRWLockExclusive(&g_media.lock);
        if (g_msgWnd && (shows || mi.has)) PostMessageW(g_msgWnd, WM_APP_MEDIA, shows ? 1 : 0, 0);
    }
    mgr = nullptr;  // (an out-of-process proxy: released while the apartment is still there)
    winrt::uninit_apartment();
    return 0;
}

static void MediaKick() {
    if (!g_media.ev) g_media.ev = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_media.thread) g_media.thread = CreateThread(nullptr, 0, MediaWorker, nullptr, 0, nullptr);
    SetEvent(g_media.ev);
}

static void MediaSetOn(bool on) {
    AcquireSRWLockExclusive(&g_media.lock);
    bool was = g_media.on;
    g_media.on = on;
    if (!on) g_media.info = MediaInfo();
    ReleaseSRWLockExclusive(&g_media.lock);
    if (on || was) MediaKick();
}

static MediaInfo MediaNow() {
    AcquireSRWLockShared(&g_media.lock);
    MediaInfo m = g_media.info;
    ReleaseSRWLockShared(&g_media.lock);
    return m;
}

// 1 play/pause, 2 next, 3 previous, 4 shuffle, 5 repeat, 6 seek to arg seconds.
static void MediaCmd(int cmd, double arg) {
    AcquireSRWLockExclusive(&g_media.lock);
    g_media.cmd = cmd;
    g_media.arg = arg;
    if (cmd == 1 && g_media.info.has) g_media.info.status = g_media.info.status == 1 ? 2 : 1;  // (shows at once)
    if (cmd == 6) {
        g_media.info.pos = arg;
        g_media.info.posAt = GetTickCount();
    }
    ReleaseSRWLockExclusive(&g_media.lock);
    MediaKick();
}

static void MediaPick(const std::wstring& aumid) {
    AcquireSRWLockExclusive(&g_media.lock);
    g_media.pick = aumid;
    ReleaseSRWLockExclusive(&g_media.lock);
    MediaKick();
}

static void MediaSignalStop() {
    if (!g_media.thread) return;
    AcquireSRWLockExclusive(&g_media.lock);
    g_media.stop = true;
    ReleaseSRWLockExclusive(&g_media.lock);
    SetEvent(g_media.ev);
}
static void MediaJoin(DWORD deadline) {
    if (JoinWorker(g_media.thread, g_media.ev, deadline)) g_media.stop = false;
}

// The cover, decoded on the UI thread (square, from the middle of a wide one).
static ComPtr<IWICBitmapSource> g_mediaArt;
static int g_mediaArtGen = -1;
static void MediaArtUpdate() {
    MediaInfo m = MediaNow();
    if (m.thumbGen == g_mediaArtGen) return;
    g_mediaArtGen = m.thumbGen;
    g_mediaArt.Reset();
    AcquireSRWLockShared(&g_media.lock);
    std::vector<BYTE> b = g_media.thumb;
    ReleaseSRWLockShared(&g_media.lock);
    if (b.empty() || !g_wic) return;
    ComPtr<IStream> st;
    st.Attach(SHCreateMemStream(b.data(), (UINT)b.size()));
    ComPtr<IWICBitmapDecoder> dec;
    ComPtr<IWICBitmapFrameDecode> fr;
    if (!st || FAILED(g_wic->CreateDecoderFromStream(st.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &dec)) || FAILED(dec->GetFrame(0, &fr))) return;
    UINT w = 0, h = 0;
    fr->GetSize(&w, &h);
    if (!w || !h) return;
    UINT side = std::min(w, h);
    WICRect crop = {(INT)(w - side) / 2, (INT)(h - side) / 2, (INT)side, (INT)side};
    ComPtr<IWICBitmapClipper> clip;
    ComPtr<IWICBitmapScaler> sc;
    ComPtr<IWICFormatConverter> conv;
    ComPtr<IWICBitmap> out;
    if (FAILED(g_wic->CreateBitmapClipper(&clip)) || FAILED(clip->Initialize(fr.Get(), &crop)) || FAILED(g_wic->CreateBitmapScaler(&sc)) ||
        FAILED(sc->Initialize(clip.Get(), 192, 192, WICBitmapInterpolationModeFant)) || FAILED(g_wic->CreateFormatConverter(&conv)) ||
        FAILED(conv->Initialize(sc.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom)) ||
        FAILED(g_wic->CreateBitmapFromSource(conv.Get(), WICBitmapCacheOnLoad, &out)))
        return;
    g_mediaArt = out;
}

// The cover in a dock (each dock keeps its own copy for its render target).
struct DockArt {
    ComPtr<ID2D1RenderTarget> rt;
    int gen = -1;
    ComPtr<ID2D1Bitmap> bmp;
};
static std::unordered_map<const void*, DockArt> g_dockArt;
static void FillBitmapRounded(ID2D1RenderTarget* rt, ID2D1Bitmap* bmp, D2D1_RECT_F r, float radius) {
    ComPtr<ID2D1BitmapBrush> br;
    D2D1_SIZE_F sz = bmp->GetSize();
    if (FAILED(rt->CreateBitmapBrush(bmp, D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                                                                      D2D1_BITMAP_INTERPOLATION_MODE_LINEAR), &br)))
        return;
    br->SetTransform(D2D1::Matrix3x2F::Scale((r.right - r.left) / sz.width, (r.bottom - r.top) / sz.height) * D2D1::Matrix3x2F::Translation(r.left, r.top));
    rt->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), br.Get());
}
static bool DrawPlayerArtOn(ID2D1RenderTarget* rt, const void* who, D2D1_RECT_F r, float radius) {
    if (!g_mediaArt || !rt) return false;
    DockArt& a = g_dockArt[who];
    if (a.rt.Get() != rt || a.gen != g_mediaArtGen) {
        a.rt = rt;
        a.gen = g_mediaArtGen;
        a.bmp.Reset();
        rt->CreateBitmapFromWicBitmap(g_mediaArt.Get(), nullptr, &a.bmp);
    }
    if (!a.bmp) return false;
    FillBitmapRounded(rt, a.bmp.Get(), r, radius);
    return true;
}
static bool DrawPlayerArt(Dock& d, D2D1_RECT_F r, float radius) { return DrawPlayerArtOn(d.rt.Get(), &d, r, radius); }

// The bars' step (20 a second while they show and something plays).
constexpr UINT_PTR TIMER_PLAYER_VIS = 35;
static void PlayerVisStep() {
    MediaInfo m = MediaNow();
    float peak = 0;
    if (m.status == 1) {
        if (!g_vis.meter || GetTickCount() - g_vis.meterAt > 5000) {
            g_vis.meter.Reset();
            g_vis.meterAt = GetTickCount();
            ComPtr<IMMDevice> dev;
            if (g_stat.mmde && SUCCEEDED(g_stat.mmde->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)))
                dev->Activate(__uuidof(NimbusMeter), CLSCTX_ALL, nullptr, (void**)g_vis.meter.GetAddressOf());
        }
        if (g_vis.meter) g_vis.meter->GetPeakValue(&peak);
    }
    g_vis.t += 0.05f;
    static const float speed[5] = {7.1f, 9.7f, 6.3f, 11.3f, 8.2f}, phase[5] = {0, 1.7f, 3.1f, 0.9f, 2.4f}, shape[5] = {0.55f, 0.85f, 1.0f, 0.8f, 0.6f};
    float lvl = std::min(1.0f, std::sqrt(peak) * 1.15f);
    for (int k = 0; k < 5; k++) {
        float want = lvl * shape[k] * (0.55f + 0.45f * (0.5f + 0.5f * std::sin(g_vis.t * speed[k] + phase[k])));
        float& v = g_vis.lv[k];
        v += (want - v) * (want > v ? 0.6f : 0.25f);  // (up fast, down slow)
    }
    for (auto* d : g_docks)
        for (auto& it : d->stat)
            if (it.id == StPlayer) Invalidate(*d);
}
static void PlayerVisUpdate() {
    bool want = g_msgWnd && (ExtraPlace(StPlayer) & PlDock) && PlayerShows() == PdBars && MediaNow().status == 1;
    static bool on = false;
    if (want == on) return;
    on = want;
    if (on) SetTimer(g_msgWnd, TIMER_PLAYER_VIS, 50, nullptr);
    else {
        KillTimer(g_msgWnd, TIMER_PLAYER_VIS);
        for (auto& v : g_vis.lv) v = 0;
        g_vis.meter.Reset();
    }
}

// The playing app's own volume (its audio sessions, matched by the program's name).
static AppVol* MediaVol(const std::wstring& aumid, bool reload) {
    static DWORD at = 0;
    bool panel = g_fly.open && (g_fly.kind == StVolume || g_fly.kind == StPlayer);
    if (reload && !panel && (g_appVols.empty() || GetTickCount() - at > 3000)) {
        LoadAppVolumes();
        at = GetTickCount();
    }
    if (aumid.empty()) return nullptr;
    std::wstring id = aumid;
    for (auto& c : id) c = towlower(c);
    for (auto& a : g_appVols) {
        if (a.system || a.path.empty()) continue;
        std::wstring exe = PathFindFileNameW(a.path.c_str());
        size_t dot = exe.rfind(L'.');
        if (dot != std::wstring::npos) exe.resize(dot);
        for (auto& c : exe) c = towlower(c);
        if (exe.size() >= 3 && id.find(exe) != std::wstring::npos) return &a;
    }
    return nullptr;
}

static std::wstring TimeText(double s) {
    int t = (int)std::max(0.0, s);
    wchar_t b[24];
    if (t >= 3600) swprintf_s(b, L"%d:%02d:%02d", t / 3600, t / 60 % 60, t % 60);
    else swprintf_s(b, L"%d:%02d", t / 60, t % 60);
    return b;
}

static double MediaPos(const MediaInfo& m) {
    double p = m.pos + (m.status == 1 ? (GetTickCount() - m.posAt) / 1000.0 : 0);
    return m.dur > 0 ? std::min(p, m.dur) : p;
}

static void FPlayerArt(D2D1_RECT_F r, float radius) {
    ID2D1Bitmap* bmp = g_mediaArt ? FBitmap(L"media:" + std::to_wstring(g_mediaArtGen), g_mediaArt.Get()) : nullptr;
    if (bmp && FDraw()) {
        FillBitmapRounded(g_fly.rt.Get(), bmp, r, radius);
        return;
    }
    FFill(r, radius, FCard());
    FIcon(StatIcon(StPlayer), (r.left + r.right) / 2, (r.top + r.bottom) / 2, (r.right - r.left) * 0.36f, FSub());
}

// A round button with a filled icon (play, pause, next, previous), drawn with D2D shapes.
static void FFilledShape(int kind, float cx, float cy, float s, D2D1_COLOR_F c) {
    if (!FDraw()) return;
    ID2D1Brush* b = FBr(c);
    auto tri = [&](float x0, float y0, float x1, float y1, float x2, float y2) {
        ComPtr<ID2D1PathGeometry> g;
        ComPtr<ID2D1GeometrySink> k;
        if (FAILED(g_d2d->CreatePathGeometry(&g)) || FAILED(g->Open(&k))) return;
        k->BeginFigure(D2D1::Point2F(x0, y0), D2D1_FIGURE_BEGIN_FILLED);
        k->AddLine(D2D1::Point2F(x1, y1));
        k->AddLine(D2D1::Point2F(x2, y2));
        k->EndFigure(D2D1_FIGURE_END_CLOSED);
        k->Close();
        g_fly.rt->FillGeometry(g.Get(), b);
    };
    auto bar = [&](float x0, float y0, float x1, float y1) {
        g_fly.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x0, y0, x1, y1), s * 0.06f, s * 0.06f), b);
    };
    float h = s / 2;
    switch (kind) {
        case 0:  // play
            tri(cx - h * 0.55f, cy - h * 0.75f, cx + h * 0.8f, cy, cx - h * 0.55f, cy + h * 0.75f);
            break;
        case 1:  // pause
            bar(cx - h * 0.62f, cy - h * 0.72f, cx - h * 0.18f, cy + h * 0.72f);
            bar(cx + h * 0.18f, cy - h * 0.72f, cx + h * 0.62f, cy + h * 0.72f);
            break;
        case 2:  // next
            tri(cx - h * 0.7f, cy - h * 0.65f, cx + h * 0.4f, cy, cx - h * 0.7f, cy + h * 0.65f);
            bar(cx + h * 0.42f, cy - h * 0.65f, cx + h * 0.66f, cy + h * 0.65f);
            break;
        case 3:  // previous
            tri(cx + h * 0.7f, cy - h * 0.65f, cx - h * 0.4f, cy, cx + h * 0.7f, cy + h * 0.65f);
            bar(cx - h * 0.66f, cy - h * 0.65f, cx - h * 0.42f, cy + h * 0.65f);
            break;
    }
}

static void FMediaBtn(float cx, float cy, float rad, int shape, const wchar_t* line, bool solid, bool enabled, bool lit, std::function<void()> fn) {
    D2D1_RECT_F r = D2D1::RectF(cx - rad, cy - rad, cx + rad, cy + rad);
    bool hot = enabled && FHot(r);
    if (FDraw()) {
        if (solid) g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad), FBr(FInk()));
        else if (lit || hot) g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad), FBr(lit ? FCol(g_theme.text, 0.14f) : FHov()));
    }
    D2D1_COLOR_F c = solid ? FSolidTint() : enabled ? (line && !lit ? FSub() : FInk()) : FCol(g_theme.text, 0.3f);
    if (line) FIcon(line, cx, cy, rad * 0.95f, c, 1.8f);
    else FFilledShape(shape, cx, cy, rad * (solid ? 0.9f : 1.0f), c);
    if (enabled) FHitAdd(r, std::move(fn));
}

static float PanelPlayer(float y) {
    MediaArtUpdate();
    MediaInfo m = MediaNow();
    if (!m.has) {
        FHeader(y, L"Player", L"");
        FText(L2(L"Nada tocando agora. Funciona com qualquer app que aparece nos controles de mídia do Windows (Spotify, YouTube no navegador, players de vídeo).",
                 L"Nothing playing now. It works with any app in Windows' media controls (Spotify, YouTube in a browser, video players)."),
              g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 54, FSub());
        return y + 60;
    }
    const NotifApp& app = NotifAppOf(m.aumid);
    // Cover, title, artist, the app
    float a = 84;
    FPlayerArt(D2D1::RectF(kFlyPad, y, kFlyPad + a, y + a), 12);
    float tx = kFlyPad + a + 14, tw = kFlyW - kFlyPad - tx;
    float th = 22;
    if (g_fly.fWrap) {
        ComPtr<IDWriteTextLayout> l;
        DWRITE_TEXT_METRICS mt = {};
        if (SUCCEEDED(g_dw->CreateTextLayout(m.title.c_str(), (UINT32)m.title.size(), g_fly.fTitle.Get(), tw, 60, &l))) {
            l->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
            l->GetMetrics(&mt);
            th = std::min(mt.height, 42.0f);
            if (FDraw()) {
                g_fly.rt->PushAxisAlignedClip(D2D1::RectF(tx, y + 4, tx + tw, y + 4 + th), D2D1_ANTIALIAS_MODE_ALIASED);
                g_fly.rt->DrawTextLayout(D2D1::Point2F(tx, y + 4), l.Get(), FBr(FInk()));
                g_fly.rt->PopAxisAlignedClip();
            }
        }
    }
    float ay = y + 4 + th + 2;
    if (!m.artist.empty()) {
        FText(m.artist, g_fly.fSmall.Get(), tx, ay, tw, 18, FCol(g_theme.text, 0.72f));
        ay += 20;
    }
    {
        float cw = std::min(tw, TextSize(app.name, g_fly.fSmall.Get()).width + 30);
        D2D1_RECT_F chip = D2D1::RectF(tx, ay + 4, tx + cw, ay + 24);
        FFill(chip, 10, FCol(g_theme.text, 0.07f));
        if (ID2D1Bitmap* ib = FBitmap(L"app:" + m.aumid, app.icon.Get())) {
            if (FDraw()) g_fly.rt->DrawBitmap(ib, D2D1::RectF(tx + 5, ay + 7, tx + 19, ay + 21));
        } else {
            FIcon(StatIcon(StPlayer), tx + 12, ay + 14, 12, FSub());
        }
        FText(app.name, g_fly.fSmall.Get(), tx + 24, ay + 4, cw - 28, 20, FSub());
    }
    y += a + 16;
    // Time
    if (m.dur > 0) {
        double pos = MediaPos(m), dur = m.dur;
        if (m.canSeek) FSlider(kFlyPad, y - 8, kFlyW - kFlyPad * 2, (float)(pos / dur), [dur](float f) { MediaCmd(6, f * dur); });
        else {
            D2D1_RECT_F bar = D2D1::RectF(kFlyPad, y + 6, kFlyW - kFlyPad, y + 10);
            FFill(bar, 2, FCol(g_theme.text, 0.16f));
            FFill(D2D1::RectF(bar.left, bar.top, bar.left + (bar.right - bar.left) * (float)(pos / dur), bar.bottom), 2, FInk());
        }
        y += 22;
        FText(TimeText(pos), g_fly.fSmall.Get(), kFlyPad, y, 100, 16, FSub());
        FText(TimeText(dur), g_fly.fSmall.Get(), kFlyW - kFlyPad - 100, y, 100, 16, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
        y += 24;
    }
    // Buttons
    float cy = y + 28, cx = kFlyW / 2;
    FMediaBtn(cx - 118, cy, 19, 0, L"M3 7h4l10 10h4 M3 17h4l3-3 M14 10l3-3h4 M18 4l3 3-3 3 M18 14l3 3-3 3", false, m.canShuffle, m.shuffle,
              [] { MediaCmd(4); });
    FMediaBtn(cx - 62, cy, 21, 3, nullptr, false, m.canPrev, false, [] { MediaCmd(3); });
    FMediaBtn(cx, cy, 27, m.status == 1 ? 1 : 0, nullptr, true, true, false, [] { MediaCmd(1); });
    FMediaBtn(cx + 62, cy, 21, 2, nullptr, false, m.canNext, false, [] { MediaCmd(2); });
    FMediaBtn(cx + 118, cy, 19, 0, m.repeat == 1 ? L"M4 11V9a3 3 0 0 1 3-3h12 M16 3l3 3-3 3 M20 13v2a3 3 0 0 1-3 3H5 M8 21l-3-3 3-3 M11 10h1.5v5"
                                                : L"M4 11V9a3 3 0 0 1 3-3h12 M16 3l3 3-3 3 M20 13v2a3 3 0 0 1-3 3H5 M8 21l-3-3 3-3",
              false, m.canRepeat, m.repeat != 0, [] { MediaCmd(5); });
    y = cy + 28 + 14;
    // The app's volume
    if (AppVol* av = MediaVol(m.aumid, false)) {
        D2D1_RECT_F mb = D2D1::RectF(kFlyPad - 4, y, kFlyPad + 28, y + 32);
        FIcon(StatIcon(StVolume), (mb.left + mb.right) / 2, y + 16, 18, FInk());
        float sx = kFlyPad + 40, sw = kFlyW - kFlyPad - 48 - sx;
        std::wstring pid = std::to_wstring(av->pid);
        FSlider(sx, y, sw, av->level, [pid](float v) {
            for (auto& x : g_appVols)
                if (std::to_wstring(x.pid) == pid) SetAppVolume(x, v);
        });
        FText(PctText(av->level), g_fly.fSmall.Get(), kFlyW - kFlyPad - 44, y, 44, 32, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
        y += 32;
        FText(std::wstring(L2(L"volume do ", L"volume of ")) + app.name, g_fly.fSmall.Get(), kFlyPad + 40, y - 6, 260, 18, FCol(g_theme.text, 0.5f));
        y += 16;
    }
    // Other apps playing
    if (m.sessions.size() > 1) {
        FSep(y);
        FText(L2(L"Também tocando", L"Also playing"), g_fly.fSmall.Get(), kFlyPad, y, 200, 20, FSub());
        y += 26;
        float x = kFlyPad;
        for (auto& id : m.sessions) {
            const NotifApp& sa = NotifAppOf(id);
            float w = std::min(150.0f, TextSize(sa.name, g_fly.fSmall.Get()).width + 34);
            if (x + w > kFlyW - kFlyPad) {
                x = kFlyPad;
                y += 34;
            }
            D2D1_RECT_F r = D2D1::RectF(x, y, x + w, y + 28);
            bool on = id == m.aumid;
            FFill(r, 14, on ? FCol(g_theme.text, 0.14f) : FHot(r) ? FHov() : FCol(g_theme.text, 0.05f));
            if (ID2D1Bitmap* ib = FBitmap(L"app:" + id, sa.icon.Get())) {
                if (FDraw()) g_fly.rt->DrawBitmap(ib, D2D1::RectF(x + 7, y + 7, x + 21, y + 21));
            }
            FText(sa.name, g_fly.fSmall.Get(), x + 26, y, w - 30, 28, on ? FInk() : FSub());
            std::wstring pick = id;
            if (!on) FHitAdd(r, [pick] { MediaPick(pick); });
            x += w + 6;
        }
        y += 36;
    }
    FSep(y);
    std::wstring open = std::wstring(L2(L"Abrir o ", L"Open ")) + app.name;
    std::wstring aumid = m.aumid;
    FLink(y, open.c_str(), [aumid] {
        CloseFlyout();
        ShellExecuteW(nullptr, L"open", (L"shell:AppsFolder\\" + aumid).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    });
    return y;
}

// The control center's card: the cover, the title and two buttons; a click opens the whole player.
static float SecPlayerCard(float y) {
    MediaArtUpdate();
    MediaInfo m = MediaNow();
    if (!m.has) return y;
    D2D1_RECT_F card = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 68);
    FFill(card, 14, FHot(card) ? FCol(g_theme.text, 0.10f) : FCol(g_theme.text, 0.06f));
    FHitAdd(card, [] { FlyEnter(StPlayer); });
    FPlayerArt(D2D1::RectF(card.left + 12, y + 12, card.left + 56, y + 56), 10);
    float tx = card.left + 68, tw = card.right - 96 - 12 - tx;
    FText(m.title, g_fly.fText.Get(), tx, y + 14, tw, 20, FInk());
    FText(m.artist, g_fly.fSmall.Get(), tx, y + 36, tw, 18, FSub());
    FMediaBtn(card.right - 64, y + 34, 17, m.status == 1 ? 1 : 0, nullptr, false, true, false, [] { MediaCmd(1); });
    FMediaBtn(card.right - 26, y + 34, 17, 2, nullptr, false, m.canNext, false, [] { MediaCmd(2); });
    return y + 68 + 14;
}

// ---- Sections (the control center shows several; a dock item or a Start tile shows one) ----

static void FSectionHead(float& y, const wchar_t* title, const std::wstring& note, std::function<void()> more = nullptr) {
    float right = kFlyW - kFlyPad;
    if (more) {
        D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y - 5, kFlyW - kFlyPad + 8, y + 27);
        if (FHot(r)) FFill(r, 8, FHov());
        FIcon(L"M9 6l6 6-6 6", right - 6, y + 11, 13, FSub());
        right -= 20;
        FHitAdd(r, std::move(more));
    }
    FText(title, g_fly.fTitle.Get(), kFlyPad, y, 200, 22, FInk());
    if (!note.empty()) FText(note, g_fly.fSmall.Get(), kFlyPad + 120, y, right - kFlyPad - 120, 22, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
    y += 30;
}

static void FlyEnter(int kind);

static float SecLight(float y, bool head) {
    if (head) FSectionHead(y, L2(L"Brilho", L"Brightness"), L2(L"por monitor", L"each monitor"), [] { FlyEnter(StLight); });
    if (!g_brScanned) {
        FText(L2(L"Procurando os monitores…", L"Looking for the monitors…"), g_fly.fSmall.Get(), kFlyPad, y, 300, 28, FSub());
        return y + 28;
    }
    for (size_t k = 0; k < g_brView.size(); k++) {
        const BrView& v = g_brView[k];
        FText(v.name.empty() ? L2(L"Monitor", L"Monitor") : v.name, g_fly.fSmall.Get(), kFlyPad, y, 104, 32, FCol(g_theme.text, 0.85f));
        if (v.ok) {
            FSlider(kFlyPad + 112, y, kFlyW - kFlyPad * 2 - 112 - 46, v.pct / 100.0f, [k](float f) { BrightSet(k, (int)std::lround(f * 100)); });
            FText(std::to_wstring(v.pct) + L"%", g_fly.fSmall.Get(), kFlyW - kFlyPad - 40, y, 40, 32, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
        } else {
            FText(L2(L"não deixa ajustar (DDC/CI desligado no monitor)", L"can't be set (DDC/CI off on the monitor)"), g_fly.fSmall.Get(),
                  kFlyPad + 112, y, kFlyW - kFlyPad * 2 - 112, 32, FSub());
        }
        y += 36;
    }
    return y;
}

static float SecSound(float y) {
    FSectionHead(y, L2(L"Som", L"Sound"), g_stat.device, [] { FlyEnter(StVolume); });
    if (!g_stat.audio) {
        FText(L2(L"Nenhuma saída de som ligada.", L"No sound output is on."), g_fly.fSmall.Get(), kFlyPad, y, 300, 28, FSub());
        return y + 28;
    }
    D2D1_RECT_F mb = D2D1::RectF(kFlyPad - 4, y, kFlyPad + 28, y + 32);
    if (FHot(mb)) FFill(mb, 8, FHov());
    FIcon(StatIcon(StVolume), (mb.left + mb.right) / 2, y + 16, 19, FInk());
    FHitAdd(mb, [] { ToggleMute(); });
    float sx = kFlyPad + 40, sw = kFlyW - kFlyPad - 48 - sx;
    FSlider(sx, y, sw, g_stat.muted ? 0 : g_stat.vol, [](float v) { SetVolume(v); });
    FText(g_stat.muted ? L2(L"Mudo", L"Muted") : std::to_wstring((int)std::lround(g_stat.vol * 100)) + L"%", g_fly.fSmall.Get(), kFlyW - kFlyPad - 44, y,
          44, 32, FSub(), DWRITE_TEXT_ALIGNMENT_TRAILING);
    y += 34;
    FLink(y, L2(L"Volume de cada app ›", L"Each app's volume ›"), [] { FlyEnter(StVolume); });
    return y;
}

static float SecPower(float y, bool head) {
    if (head) FSectionHead(y, L2(L"Energia", L"Power"), L"");
    D2D1_RECT_F box = D2D1::RectF(kFlyPad, y, kFlyW - kFlyPad, y + 36);
    FFill(box, 10, FCol(g_theme.hover, 0.5f));
    const wchar_t* names[3] = {L2(L"Economia", L"Efficiency"), L2(L"Equilibrado", L"Balanced"), L2(L"Desempenho", L"Performance")};
    int cur = PowerModeCached();
    float w = (box.right - box.left - 6) / 3;
    for (int k = 0; k < 3; k++) {
        D2D1_RECT_F t = D2D1::RectF(box.left + 3 + k * w, box.top + 3, box.left + 3 + (k + 1) * w, box.bottom - 3);
        if (cur == k) FFill(t, 8, FHov());
        else if (FHot(t)) FFill(t, 8, FCol(g_theme.hover, 0.6f));
        FText(names[k], g_fly.fSmall.Get(), t.left, t.top, t.right - t.left, t.bottom - t.top, cur == k ? FInk() : FSub(), DWRITE_TEXT_ALIGNMENT_CENTER);
        FHitAdd(t, [k] {
            SetPowerMode(k);
            g_powerA = k;
            g_ccClickAt = GetTickCount();
        });
    }
    return y + 36 + 4;
}

static float SecShort(float y) {
    struct Sc {
        const wchar_t* name;
        const wchar_t* icon;
        int what;
    };
    Sc list[4] = {{L2(L"Projetar", L"Project"), L"M3 5h14v10H3z M7 19h6 M10 15v4 M17 9h4v8h-6", 0},
                  {L2(L"Transmitir", L"Cast"), L"M3 8V6a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2v12a2 2 0 0 1-2 2h-6 M3 12a8 8 0 0 1 8 8 M3 16a4 4 0 0 1 4 4 M3 20h.01", 1},
                  {L2(L"Legendas", L"Captions"), L"M3 5h18v14H3z M7 13h3 M14 13h3 M7 16h10", 2},
                  {L2(L"Acessibilidade", L"Accessibility"), L"M12 4a2 2 0 1 1 0 .01 M5 8l7 2 7-2 M12 10v5 M9 21l3-6 3 6", 3}};
    float x = kFlyPad;
    for (auto& sc : list) {
        float w = TextSize(sc.name, g_fly.fSmall.Get()).width + 40;
        if (x + w > kFlyW - kFlyPad) {
            x = kFlyPad;
            y += 36;
        }
        D2D1_RECT_F r = D2D1::RectF(x, y, x + w, y + 30);
        if (FHot(r)) FFill(r, 15, FHov());
        if (FDraw()) g_fly.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + .5f, r.top + .5f, r.right - .5f, r.bottom - .5f), 14.5f, 14.5f),
                                                    FBr(FCol(g_theme.text, 0.16f)), 1);
        FIcon(sc.icon, r.left + 17, y + 15, 15, FCol(g_theme.text, 0.85f), 1.8f);
        FText(sc.name, g_fly.fSmall.Get(), r.left + 30, y, w - 34, 30, FCol(g_theme.text, 0.85f));
        int what = sc.what;
        FHitAdd(r, [what] {
            CloseFlyout();
            if (what == 0) SendWinKey('P');
            else if (what == 1) SendWinKey('K');
            else if (what == 2) SendWinCtrlKey('L');
            else OpenUri(L"ms-settings:easeofaccess");
        });
        x += w + 6;
    }
    return y + 36;
}

// ---- Start, Status tab: the control center's look in parts the user can put in any order ----

enum StartSec { SsClock, SsPlayer, SsRings, SsSound, SsLight, SsTray, SsPower, SsShort, SsRows, SsCount };
static const wchar_t* const kStartSecKey[SsCount] = {L"clock", L"player", L"rings", L"sound", L"light", L"tray", L"power", L"short", L"rows"};
static std::vector<int> g_startSecOrder;
static bool g_startSecLoaded = false;

// The order is saved per account ("start.statusOrder", the parts' names with commas); a part it doesn't name goes last.
static void LoadStartSecOrder() {
    g_startSecLoaded = true;
    g_startSecOrder.clear();
    wchar_t buf[256] = {};
    Wh_GetStringValue((UserPrefix() + L"start.statusOrder").c_str(), buf, ARRAYSIZE(buf));
    std::wstring v = buf;
    size_t a = 0;
    while (a < v.size()) {
        size_t b = v.find(L',', a);
        if (b == std::wstring::npos) b = v.size();
        std::wstring w = v.substr(a, b - a);
        for (int k = 0; k < SsCount; k++)
            if (w == kStartSecKey[k] && std::find(g_startSecOrder.begin(), g_startSecOrder.end(), k) == g_startSecOrder.end())
                g_startSecOrder.push_back(k);
        a = b + 1;
    }
    for (int k = 0; k < SsCount; k++)
        if (std::find(g_startSecOrder.begin(), g_startSecOrder.end(), k) == g_startSecOrder.end()) g_startSecOrder.push_back(k);
}
static void SaveStartSecOrder() {
    std::wstring v;
    for (int k : g_startSecOrder) v += (v.empty() ? L"" : L",") + std::wstring(kStartSecKey[k]);
    Wh_SetStringValue((UserPrefix() + L"start.statusOrder").c_str(), v.c_str());
}
static void StartSecMoveTo(int sec, int target) {
    if (!g_startSecLoaded) LoadStartSecOrder();
    auto& o = g_startSecOrder;
    auto from = std::find(o.begin(), o.end(), sec), to = std::find(o.begin(), o.end(), target);
    if (from == o.end() || to == o.end() || from == to) return;
    if (from < to) std::rotate(from, from + 1, to + 1);
    else std::rotate(to, from, from + 1);
}
static bool StartSecShown(int k);
// One step up or down among the parts that show.
static void StartSecStep(int sec, int dir) {
    if (!g_startSecLoaded) LoadStartSecOrder();
    std::vector<int> shown;
    for (int k : g_startSecOrder)
        if (StartSecShown(k)) shown.push_back(k);
    auto it = std::find(shown.begin(), shown.end(), sec);
    if (it == shown.end()) return;
    int i = (int)(it - shown.begin()) + dir;
    if (i < 0 || i >= (int)shown.size()) return;
    StartSecMoveTo(sec, shown[i]);
}

static bool StartSecShown(int k) {
    auto inS = [](int id) { return StatOn(id) && (g_set.statPlace[id] & PlStart); };
    auto exS = [](int id) { return (ExtraPlace(id) & PlStart) != 0; };
    switch (k) {
        case SsClock: return inS(StClock);
        case SsPlayer: return exS(StPlayer);
        case SsRings:
            return inS(StNetwork) || inS(StKeyboard) || inS(StBattery) || exS(StDnd) || exS(StNight) || (exS(StBt) && BtPresent());
        case SsSound: return inS(StVolume);
        case SsLight: return exS(StLight);
        case SsTray: return TrayInStart();
        case SsPower: return exS(StPower);
        case SsShort: return exS(StShort);
        case SsRows:
            return (g_set.notifPlace & PlStart) || (g_set.notesPlace & PlStart) || (g_set.shelfPlace & PlStart) ||
                   (exS(StDevices) && !DevicesNow().empty());
    }
    return false;
}

// Opening the tab: what its parts show gets read (the monitors' brightness, the media, the clock).
static void StartStatusRefresh() {
    if (!g_startSecLoaded) LoadStartSecOrder();
    UpdateClock();
    if (StartSecShown(SsLight)) {
        CcRefresh();
        BrightScan();
    }
    if (StartSecShown(SsPlayer)) MediaKick();
}

// The Organize / Done button, on the tab row.
static void StartOrgButton(const StartPaint& p, float listTop) {
    auto& s = g_start;
    auto* rt = p.rt;
    if (!StartHasStatus()) return;
    const wchar_t* label = s.org ? L2(L"Pronto", L"Done") : L2(L"Organizar", L"Organize");
    float tw = TextSize(label, s.small.Get()).width;
    D2D1_RECT_F r = D2D1::RectF(p.W - p.pad - tw - SS(40), listTop + SS(3), p.W - p.pad, listTop + SS(33));
    int idx = (int)s.rows.size();
    s.rows.push_back({r, RowOrg});
    bool hot = s.hover == idx;
    ComPtr<ID2D1SolidColorBrush> on;
    rt->CreateSolidColorBrush(p.tintSolid, &on);
    if (s.org) rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(15), SS(15)), p.text);
    else if (hot) rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(15), SS(15)), p.cardHi);
    ID2D1Brush* ink = s.org ? (ID2D1Brush*)on.Get() : p.sub;
    DrawLineIcon(rt, s.org ? L"M5 12l5 5 9-10" : L"M4 7h16 M4 12h16 M4 17h16", r.left + SS(18), (r.top + r.bottom) / 2, SS(14), ink);
    rt->DrawTextW(label, (UINT32)wcslen(label), s.small.Get(), D2D1::RectF(r.left + SS(32), r.top, r.right - SS(8), r.bottom), ink);
}

static void StartStatusTab(const StartPaint& p) {
    auto& s = g_start;
    auto* rt = p.rt;
    if (!g_startSecLoaded) LoadStartSecOrder();
    bool live = !s.org;  // (organizing: the parts move, their buttons rest)
    float L = p.pad + (s.org ? SS(30) : 0), R = p.W - p.pad, gap = SS(14);
    ComPtr<ID2D1SolidColorBrush> tint;
    rt->CreateSolidColorBrush(p.tintSolid, &tint);
    auto text = [&](const std::wstring& t, IDWriteTextFormat* f, float x, float y, float w, float h, ID2D1Brush* b,
                    DWRITE_TEXT_ALIGNMENT al = DWRITE_TEXT_ALIGNMENT_LEADING) {
        if (!f || w <= 0) return;
        f->SetTextAlignment(al);
        rt->DrawTextW(t.c_str(), (UINT32)t.size(), f, D2D1::RectF(x, y, x + w, y + h), b, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        if (al != DWRITE_TEXT_ALIGNMENT_LEADING) f->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    };
    // A clickable spot; returns whether it's under the mouse or the keyboard.
    auto button = [&](D2D1_RECT_F r, std::function<void()> fn) {
        if (!live) return false;
        int idx = (int)s.rows.size();
        StartRow row{r, RowFn};
        row.fn = std::move(fn);
        s.rows.push_back(std::move(row));
        return s.hover == idx;
    };
    auto slider = [&](float x, float y, float w, float v, std::function<void(float)> set) {
        float cy = y + SS(16);
        v = std::clamp(v, 0.0f, 1.0f);
        ComPtr<ID2D1SolidColorBrush> track;
        D2D1_COLOR_F tc = g_theme.text;
        tc.a = 0.18f;
        rt->CreateSolidColorBrush(tc, &track);
        rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x, cy - SS(2), x + w, cy + SS(2)), SS(2), SS(2)), track.Get());
        rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x, cy - SS(2), x + w * v, cy + SS(2)), SS(2), SS(2)), p.text);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + w * v, cy), SS(8), SS(8)), tint.Get());
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + w * v, cy), SS(5.5f), SS(5.5f)), p.text);
        if (!live) return;
        StartRow row{D2D1::RectF(x - SS(8), y, x + w + SS(8), y + SS(32)), RowSlide};
        row.slide = std::move(set);
        row.x0 = x;
        row.x1 = x + w;
        s.rows.push_back(std::move(row));
    };
    // A section title with a note and ›, the whole line opening that item's panel.
    auto head = [&](float& y, const wchar_t* title, const std::wstring& note, int panel) {
        D2D1_RECT_F r = D2D1::RectF(L - SS(8), y - SS(4), R + SS(8), y + SS(26));
        bool hot = panel >= 0 && button(r, [panel] {
            if (g_start.dock) StatClick(*g_start.dock, panel, false, true);
        });
        if (hot) rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(8), SS(8)), p.cardHi);
        float right = R;
        if (panel >= 0) {
            DrawLineIcon(rt, L"M9 6l6 6-6 6", R - SS(6), y + SS(11), SS(13), p.sub);
            right -= SS(20);
        }
        text(title, s.head.Get(), L, y, SS(220), SS(22), p.text);
        if (!note.empty()) text(note, s.small.Get(), L + SS(200), y, right - L - SS(200), SS(22), p.sub, DWRITE_TEXT_ALIGNMENT_TRAILING);
        y += SS(32);
    };

    rt->PushAxisAlignedClip(D2D1::RectF(0, p.top, p.W, p.bottom), D2D1_ANTIALIAS_MODE_ALIASED);
    float y = p.top - s.scroll;
    if (s.org) {
        text(L2(L"Arraste uma parte pela alça pra mudar a ordem (ou Alt + setas).", L"Drag a part by its handle to change the order (or Alt + arrows)."),
             s.small.Get(), p.pad, y, R - p.pad - SS(130), SS(26), p.sub);
        const wchar_t* reset = L2(L"Voltar ao padrão", L"Back to default");
        D2D1_RECT_F rr = D2D1::RectF(R - SS(124), y, R, y + SS(26));
        int idx = (int)s.rows.size();
        StartRow row{rr, RowFn};
        row.fn = [] {
            Wh_SetStringValue((UserPrefix() + L"start.statusOrder").c_str(), L"");
            LoadStartSecOrder();
        };
        s.rows.push_back(std::move(row));
        if (s.hover == idx) rt->FillRoundedRectangle(D2D1::RoundedRect(rr, SS(8), SS(8)), p.cardHi);
        text(reset, s.small.Get(), rr.left, rr.top, rr.right - rr.left - SS(8), SS(26), p.text, DWRITE_TEXT_ALIGNMENT_TRAILING);
        y += SS(34);
    }
    bool first = true;
    for (int k : g_startSecOrder) {
        if (!StartSecShown(k)) continue;
        float top = y;
        if (!first) {
            rt->FillRectangle(D2D1::RectF(p.pad, y, R, y + std::max(1.0f, s.scale)), p.line);
            y += gap;
        }
        first = false;
        float inner = y;
        switch (k) {
            case SsClock: {
                UpdateClock();
                bool hot = button(D2D1::RectF(L - SS(8), y - SS(4), R + SS(8), y + SS(60)), [] {
                    if (g_start.dock) StatClick(*g_start.dock, StClock, false, true);
                });
                if (hot) rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(L - SS(8), y - SS(4), R + SS(8), y + SS(60)), SS(10), SS(10)), p.cardHi);
                text(g_stat.time, s.big.Get(), L, y, SS(260), SS(38), p.text);
                text(g_stat.dateLong, s.small.Get(), L, y + SS(40), SS(320), SS(18), p.sub);
                text(std::wstring(L2(L"Calendário", L"Calendar")) + L" ›", s.small.Get(), R - SS(140), y + SS(34), SS(140), SS(22), p.sub,
                     DWRITE_TEXT_ALIGNMENT_TRAILING);
                y += SS(58);
                break;
            }
            case SsPlayer: {
                MediaInfo m = MediaNow();
                D2D1_RECT_F card = D2D1::RectF(L, y, R, y + SS(64));
                bool hot = button(card, [] {
                    if (g_start.dock) StatClick(*g_start.dock, StPlayer, false, true);
                });
                rt->FillRoundedRectangle(D2D1::RoundedRect(card, SS(14), SS(14)), hot ? p.cardHi : p.card);
                D2D1_RECT_F ar = D2D1::RectF(card.left + SS(10), y + SS(10), card.left + SS(54), y + SS(54));
                if (!m.has || !DrawPlayerArtOn(rt, &s, ar, SS(9))) {
                    rt->FillRoundedRectangle(D2D1::RoundedRect(ar, SS(9), SS(9)), p.cardHi);
                    DrawLineIcon(rt, StatIcon(StPlayer), (ar.left + ar.right) / 2, (ar.top + ar.bottom) / 2, SS(18), p.text);
                }
                float tx = ar.right + SS(12), tw = card.right - SS(96) - tx;
                text(m.has ? m.title : std::wstring(L2(L"Nada tocando", L"Nothing playing")), s.font.Get(), tx, y + SS(12), tw, SS(22), p.text);
                if (m.has) text(m.artist, s.small.Get(), tx, y + SS(34), tw, SS(18), p.sub);
                if (m.has) {
                    auto mbtn = [&](float cx, const wchar_t* path, int cmd) {
                        D2D1_RECT_F r = D2D1::RectF(cx - SS(17), y + SS(15), cx + SS(17), y + SS(49));
                        bool h = button(r, [cmd] { MediaCmd(cmd); });
                        if (h) rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, y + SS(32)), SS(17), SS(17)), p.cardHi);
                        DrawLineIcon(rt, path, cx, y + SS(32), SS(16), p.text);
                    };
                    mbtn(card.right - SS(70), m.status == 1 ? L"M8 5v14 M16 5v14" : L"M8 5v14l11-7z", 1);
                    mbtn(card.right - SS(30), L"M6 5v14l9-7z M18 5v14", 2);
                }
                y += SS(64);
                break;
            }
            case SsRings: {
                struct Rg {
                    const wchar_t* icon;
                    std::wstring txt, name, state;
                    bool on;
                    std::function<void()> fn;
                };
                auto inS = [](int id) { return StatOn(id) && (g_set.statPlace[id] & PlStart); };
                auto exS = [](int id) { return (ExtraPlace(id) & PlStart) != 0; };
                auto open = [](int id) {
                    return [id] {
                        if (g_start.dock) StatClick(*g_start.dock, id, false, true);
                    };
                };
                std::vector<Rg> rings;
                if (inS(StNetwork))
                    rings.push_back({StatIcon(StNetwork), L"", L2(L"Rede", L"Network"),
                                     g_stat.net == 2 ? g_stat.wifiName : g_stat.net == 1 ? std::wstring(L2(L"Cabo", L"Cable")) : std::wstring(L2(L"Sem rede", L"Offline")),
                                     g_stat.net != 0, open(StNetwork)});
                if (exS(StBt) && BtPresent()) {
                    BtView bv = BtNow();
                    std::wstring st = bv.radio < 0 ? L2(L"Indisponível", L"Unavailable") : bv.radio == 0 ? L2(L"Desligado", L"Off") : L2(L"Ligado", L"On");
                    for (auto& dv : bv.devs)
                        if (bv.radio == 1 && dv.connected) {
                            st = dv.name;
                            break;
                        }
                    rings.push_back({StatIcon(StBt), L"", L"Bluetooth", st, bv.radio == 1, open(StBt)});
                }
                if (exS(StDnd))
                    rings.push_back({StatIcon(StDnd), L"", L2(L"Não incomodar", L"Do not disturb"),
                                     g_set.notifDnd ? std::wstring(L2(L"Ligado", L"On")) : std::wstring(L2(L"Desligado", L"Off")), g_set.notifDnd,
                                     [] { SetDnd(!g_set.notifDnd); }});
                if (exS(StNight)) {
                    int ns = NightState();
                    rings.push_back({StatIcon(StNight), L"", L2(L"Luz noturna", L"Night light"),
                                     ns < 0 ? std::wstring(L2(L"Configurar", L"Set up")) : ns ? std::wstring(L2(L"Ligada", L"On")) : std::wstring(L2(L"Desligada", L"Off")),
                                     ns == 1, [ns] {
                                         if (ns < 0 || !NightSet(ns == 0))
                                             if (g_start.dock) StatClick(*g_start.dock, StNight, false, true);
                                     }});
                }
                if (inS(StKeyboard))
                    rings.push_back({nullptr, g_stat.cur ? KbShort(g_stat.cur) : L"??", L2(L"Teclado", L"Keyboard"),
                                     g_stat.cur ? KbOf(g_stat.cur).name : std::wstring(L2(L"escolher", L"choose")), false, open(StKeyboard)});
                if (inS(StBattery))
                    rings.push_back({StatIcon(StBattery), L"", L2(L"Bateria", L"Battery"), std::to_wstring(g_stat.pct) + L"%", g_stat.charging, open(StBattery)});
                float cw = (R - L) / 6;
                for (size_t i = 0; i < rings.size(); i++) {
                    if (i && i % 6 == 0) y += SS(92);
                    auto& rg = rings[i];
                    float cx = L + (i % 6) * cw + cw / 2;
                    D2D1_RECT_F r = D2D1::RectF(cx - cw / 2, y, cx + cw / 2, y + SS(88));
                    bool hot = button(r, rg.fn);
                    D2D1_COLOR_F rc = g_theme.text;
                    rc.a = rg.on ? 1.0f : hot ? 0.16f : 0.09f;
                    ComPtr<ID2D1SolidColorBrush> ring;
                    rt->CreateSolidColorBrush(rc, &ring);
                    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, y + SS(24)), SS(23), SS(23)), ring.Get());
                    ID2D1Brush* ink = rg.on ? (ID2D1Brush*)tint.Get() : p.text;
                    if (rg.icon) DrawLineIcon(rt, rg.icon, cx, y + SS(24), SS(19), ink);
                    else text(rg.txt, s.head.Get(), cx - SS(23), y + SS(1), SS(46), SS(46), ink, DWRITE_TEXT_ALIGNMENT_CENTER);
                    text(rg.name, s.small.Get(), cx - cw / 2 + SS(2), y + SS(52), cw - SS(4), SS(18), p.text, DWRITE_TEXT_ALIGNMENT_CENTER);
                    text(rg.state, s.small.Get(), cx - cw / 2 + SS(2), y + SS(69), cw - SS(4), SS(16), p.sub, DWRITE_TEXT_ALIGNMENT_CENTER);
                }
                y += SS(88);
                break;
            }
            case SsSound: {
                head(y, L2(L"Som", L"Sound"), g_stat.device, StVolume);
                if (!g_stat.audio) {
                    text(L2(L"Nenhuma saída de som ligada.", L"No sound output is on."), s.small.Get(), L, y, R - L, SS(28), p.sub);
                    y += SS(28);
                    break;
                }
                D2D1_RECT_F mb = D2D1::RectF(L - SS(4), y, L + SS(28), y + SS(32));
                if (button(mb, [] { ToggleMute(); })) rt->FillRoundedRectangle(D2D1::RoundedRect(mb, SS(8), SS(8)), p.cardHi);
                DrawLineIcon(rt, StatIcon(StVolume), (mb.left + mb.right) / 2, y + SS(16), SS(18), p.text);
                slider(L + SS(42), y, R - SS(52) - (L + SS(42)), g_stat.muted ? 0 : g_stat.vol, [](float v) { SetVolume(v); });
                text(g_stat.muted ? std::wstring(L2(L"Mudo", L"Muted")) : std::to_wstring((int)std::lround(g_stat.vol * 100)) + L"%", s.small.Get(),
                     R - SS(46), y, SS(46), SS(32), p.sub, DWRITE_TEXT_ALIGNMENT_TRAILING);
                y += SS(32);
                break;
            }
            case SsLight: {
                head(y, L2(L"Brilho", L"Brightness"), L2(L"por monitor", L"each monitor"), StLight);
                if (!g_brScanned) {
                    text(L2(L"Procurando os monitores…", L"Looking for the monitors…"), s.small.Get(), L, y, R - L, SS(28), p.sub);
                    y += SS(28);
                    break;
                }
                for (size_t m = 0; m < g_brView.size(); m++) {
                    const BrView& v = g_brView[m];
                    text(v.name.empty() ? std::wstring(L2(L"Monitor", L"Monitor")) : v.name, s.small.Get(), L, y, SS(130), SS(32), p.text);
                    if (v.ok) {
                        slider(L + SS(140), y, R - SS(52) - (L + SS(140)), v.pct / 100.0f, [m](float f) { BrightSet(m, (int)std::lround(f * 100)); });
                        text(std::to_wstring(v.pct) + L"%", s.small.Get(), R - SS(46), y, SS(46), SS(32), p.sub, DWRITE_TEXT_ALIGNMENT_TRAILING);
                    } else {
                        text(L2(L"não deixa ajustar (DDC/CI desligado no monitor)", L"can't be set (DDC/CI off on the monitor)"), s.small.Get(), L + SS(140), y,
                             R - L - SS(140), SS(32), p.sub);
                    }
                    y += SS(36);
                }
                y -= SS(4);
                break;
            }
            case SsTray: {
                std::vector<int> order = TrayOrder();
                head(y, L2(L"Em segundo plano", L"In the background"), std::to_wstring(order.size()), -1);
                if (order.empty()) {
                    text(L2(L"Nenhum programa com ícone agora.", L"No app has an icon now."), s.small.Get(), L, y, R - L, SS(24), p.sub);
                    y += SS(24);
                    break;
                }
                float cell = SS(40);
                int cols = std::max(1, (int)((R - L + SS(4)) / cell));
                for (size_t t = 0; t < order.size(); t++) {
                    int ti = order[t];
                    float x = L - SS(4) + (t % cols) * cell, ty = y + (t / cols) * cell;
                    D2D1_RECT_F r = D2D1::RectF(x + SS(2), ty + SS(2), x + cell - SS(2), ty + cell - SS(2));
                    if (live) {
                        int idx = (int)s.rows.size();
                        s.rows.push_back({r, RowTray, nullptr, ti});
                        if (s.hover == idx) rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(9), SS(9)), p.cardHi);
                    }
                    ComPtr<ID2D1Bitmap> bmp;
                    if (g_tray[ti].icon) rt->CreateBitmapFromWicBitmap(g_tray[ti].icon.Get(), nullptr, &bmp);
                    float cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2, h = SS(10);
                    if (bmp) rt->DrawBitmap(bmp.Get(), D2D1::RectF(cx - h, cy - h, cx + h, cy + h));
                    if (TrayIsPinned(g_tray[ti])) rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(r.right - SS(6), r.top + SS(6)), SS(2.5f), SS(2.5f)), p.text);
                }
                y += ((order.size() + cols - 1) / cols) * cell;
                break;
            }
            case SsPower: {
                head(y, L2(L"Energia", L"Power"), StatOn(StBattery) && g_stat.battery ? std::wstring(L2(L"Bateria ", L"Battery ")) + std::to_wstring(g_stat.pct) + L"%" : L"", -1);
                D2D1_RECT_F box = D2D1::RectF(L, y, R, y + SS(36));
                rt->FillRoundedRectangle(D2D1::RoundedRect(box, SS(10), SS(10)), p.card);
                const wchar_t* names[3] = {L2(L"Economia", L"Efficiency"), L2(L"Equilibrado", L"Balanced"), L2(L"Desempenho", L"Performance")};
                int cur = PowerModeCached();
                float w = (box.right - box.left - SS(6)) / 3;
                for (int m = 0; m < 3; m++) {
                    D2D1_RECT_F t = D2D1::RectF(box.left + SS(3) + m * w, box.top + SS(3), box.left + SS(3) + (m + 1) * w, box.bottom - SS(3));
                    bool hot = button(t, [m] {
                        SetPowerMode(m);
                        g_powerA = m;
                        g_ccClickAt = GetTickCount();
                    });
                    if (cur == m) rt->FillRoundedRectangle(D2D1::RoundedRect(t, SS(8), SS(8)), p.hi);
                    else if (hot) rt->FillRoundedRectangle(D2D1::RoundedRect(t, SS(8), SS(8)), p.cardHi);
                    text(names[m], s.small.Get(), t.left, t.top, t.right - t.left, t.bottom - t.top, cur == m ? p.text : p.sub, DWRITE_TEXT_ALIGNMENT_CENTER);
                }
                y += SS(36);
                break;
            }
            case SsShort: {
                struct Sc {
                    const wchar_t* name;
                    const wchar_t* icon;
                    int what;
                };
                Sc list[4] = {{L2(L"Projetar", L"Project"), L"M3 5h14v10H3z M7 19h6 M10 15v4 M17 9h4v8h-6", 0},
                              {L2(L"Transmitir", L"Cast"), L"M3 8V6a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2v12a2 2 0 0 1-2 2h-6 M3 12a8 8 0 0 1 8 8 M3 16a4 4 0 0 1 4 4 M3 20h.01", 1},
                              {L2(L"Legendas", L"Captions"), L"M3 5h18v14H3z M7 13h3 M14 13h3 M7 16h10", 2},
                              {L2(L"Acessibilidade", L"Accessibility"), L"M12 4a2 2 0 1 1 0 .01 M5 8l7 2 7-2 M12 10v5 M9 21l3-6 3 6", 3}};
                float x = L;
                for (auto& sc : list) {
                    float w = TextSize(sc.name, s.small.Get()).width + SS(42);
                    if (x + w > R) {
                        x = L;
                        y += SS(38);
                    }
                    D2D1_RECT_F r = D2D1::RectF(x, y, x + w, y + SS(32));
                    int what = sc.what;
                    bool hot = button(r, [what] {
                        CloseStart(true);
                        if (what == 0) SendWinKey('P');
                        else if (what == 1) SendWinKey('K');
                        else if (what == 2) SendWinCtrlKey('L');
                        else OpenUri(L"ms-settings:easeofaccess");
                    });
                    if (hot) rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(16), SS(16)), p.cardHi);
                    rt->DrawRoundedRectangle(D2D1::RoundedRect(r, SS(16), SS(16)), p.line, std::max(1.0f, s.scale));
                    DrawLineIcon(rt, sc.icon, r.left + SS(18), y + SS(16), SS(15), p.text);
                    text(sc.name, s.small.Get(), r.left + SS(32), y, w - SS(36), SS(32), p.text);
                    x += w + SS(6);
                }
                y += SS(32);
                break;
            }
            case SsRows: {
                struct Ln {
                    int id;
                    std::wstring name, count;
                };
                std::vector<Ln> ls;
                if ((g_set.notifPlace & PlStart)) ls.push_back({StNotif, L2(L"Notificações", L"Notifications"), std::to_wstring(g_notifs.size())});
                if ((g_set.notesPlace & PlStart)) ls.push_back({StNotes, L2(L"Notas e lembretes", L"Notes and reminders"), std::to_wstring(RemPending())});
                if ((g_set.shelfPlace & PlStart)) ls.push_back({StShelf, L2(L"Prateleira", L"Shelf"), std::to_wstring(g_shelf.size())});
                if ((ExtraPlace(StDevices) & PlStart) && !DevicesNow().empty()) {
                    std::wstring c;
                    for (auto& dv : DevicesNow())
                        if (dv.percent >= 0) c += (c.empty() ? L"" : L" · ") + std::to_wstring(dv.percent) + L"%";
                    ls.push_back({StDevices, L2(L"Dispositivos", L"Devices"), c});
                }
                float cw = (R - L - SS(6)) / 2;
                for (size_t i = 0; i < ls.size(); i++) {
                    float x = L + (i % 2) * (cw + SS(6)), ry = y + (i / 2) * SS(48);
                    D2D1_RECT_F r = D2D1::RectF(x, ry, x + cw, ry + SS(42));
                    int id = ls[i].id;
                    bool hot = button(r, [id] {
                        if (g_start.dock) StatClick(*g_start.dock, id, false, true);
                    });
                    rt->FillRoundedRectangle(D2D1::RoundedRect(r, SS(12), SS(12)), hot ? p.cardHi : p.card);
                    DrawLineIcon(rt, StatIcon(id), r.left + SS(22), ry + SS(21), SS(17), p.text);
                    float cwid = TextSize(ls[i].count, s.small.Get()).width + SS(4);
                    text(ls[i].name, s.font.Get(), r.left + SS(42), ry, r.right - r.left - SS(42) - SS(30) - cwid, SS(42), p.text);
                    text(ls[i].count, s.small.Get(), r.right - SS(30) - cwid, ry, cwid, SS(42), p.sub, DWRITE_TEXT_ALIGNMENT_TRAILING);
                    DrawLineIcon(rt, L"M9 6l6 6-6 6", r.right - SS(16), ry + SS(21), SS(13), p.sub);
                }
                y += ((ls.size() + 1) / 2) * SS(48) - SS(6);
                break;
            }
        }
        y += gap;
        if (s.org) {
            // The part, with its handle (dots on the left), as one row to drag or to move with Alt + arrows.
            D2D1_RECT_F r = D2D1::RectF(p.pad - SS(6), top + (top == inner ? 0 : SS(2)), R + SS(6), y);
            int idx = (int)s.rows.size();
            StartRow row{r, RowSec};
            row.power = k;
            s.rows.push_back(std::move(row));
            bool dragged = s.secDrag.active && s.secDrag.sec == k;
            if (s.hover == idx || dragged) {
                ComPtr<ID2D1SolidColorBrush> edge;
                D2D1_COLOR_F ec = g_theme.text;
                ec.a = dragged ? 0.7f : 0.25f;
                rt->CreateSolidColorBrush(ec, &edge);
                rt->DrawRoundedRectangle(D2D1::RoundedRect(r, SS(12), SS(12)), edge.Get(), std::max(1.0f, s.scale) * (dragged ? 1.5f : 1));
            }
            float gx = p.pad + SS(8), gy = (inner + y - gap) / 2;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = 0; dx <= 1; dx++)
                    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(gx + dx * SS(6), gy + dy * SS(6)), SS(1.6f), SS(1.6f)), p.sub);
        }
    }
    rt->PopAxisAlignedClip();
    // The scroll range, from what was laid out (the next frame uses it).
    float contentH = y + s.scroll - p.top;
    s.maxScroll = std::max(0.0f, contentH - (p.bottom - p.top));
    s.scroll = std::clamp(s.scroll, 0.0f, s.maxScroll);
}

// A round switch with its name under it (the control center's top row).
static void FRound(float cx, float y, const wchar_t* icon, const std::wstring& text, const std::wstring& name, const std::wstring& state, bool on,
                   std::function<void()> fn, std::function<void()> more = nullptr) {
    D2D1_RECT_F r = D2D1::RectF(cx - 40, y, cx + 40, y + 88);
    bool hot = FHot(r);
    D2D1_COLOR_F ring = on ? FInk() : FCol(g_theme.text, hot ? 0.16f : 0.09f);
    if (FDraw()) g_fly.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, y + 24), 24, 24), FBr(ring));
    if (icon) FIcon(icon, cx, y + 24, 20, on ? FSolidTint() : FInk(), 1.8f);
    else FText(text, g_fly.fCode.Get(), cx - 24, y, 48, 48, on ? FSolidTint() : FInk(), DWRITE_TEXT_ALIGNMENT_CENTER);
    FText(name, g_fly.fSmall.Get(), cx - 40, y + 52, 80, 18, FInk(), DWRITE_TEXT_ALIGNMENT_CENTER);
    FText(state, g_fly.fSmall.Get(), cx - 40, y + 69, 80, 16, FCol(g_theme.text, 0.5f), DWRITE_TEXT_ALIGNMENT_CENTER);
    FHitAdd(r, std::move(fn));
    if (more) g_fly.hits.back().button = [more](int) { more(); };
}

static void OpenPanelFromCentral(int id) { FlyEnter(id); }

static float PanelCentral(float y) {
    // Header: the time, the date, edit and settings.
    UpdateClock();
    FText(g_stat.time, g_fly.fBig.Get(), kFlyPad, y - 4, 220, 0, FInk());
    FText(g_stat.dateLong, g_fly.fSmall.Get(), kFlyPad, y + 40, 260, 0, FSub());
    auto iconBtn = [&](float x, const wchar_t* path, std::function<void()> fn) {
        D2D1_RECT_F r = D2D1::RectF(x, y, x + 30, y + 30);
        if (FHot(r)) FFill(r, 8, FHov());
        FIcon(path, x + 15, y + 15, 16, FCol(g_theme.text, 0.85f), 1.8f);
        FHitAdd(r, std::move(fn));
    };
    iconBtn(kFlyW - kFlyPad - 64, L"M4 20h4L19 9l-4-4L4 16z M14 6l4 4", [] {
        CloseFlyout();
        OpenSettings();
        SwSetPage(3);  // (the status page)
    });
    iconBtn(kFlyW - kFlyPad - 30, L"M4 7h10 M18 7h2 M16 5v4 M4 17h4 M12 17h8 M10 15v4", [] {
        CloseFlyout();
        OpenSettings();
    });
    y += 66;
    if ((ExtraPlace(StPlayer) & PlCentral)) y = SecPlayerCard(y);

    // Round switches: network, Bluetooth, do not disturb, night light, keyboard, battery (those in here).
    struct Rd {
        const wchar_t* icon;
        std::wstring text, name, state;
        bool on;
        std::function<void()> fn;
        std::function<void()> more;
    };
    std::vector<Rd> rounds;
    if (StatOn(StNetwork) && (g_set.statPlace[StNetwork] & PlCentral))
        rounds.push_back({StatIcon(StNetwork), L"", L2(L"Rede", L"Network"),
                          g_stat.net == 2 ? g_stat.wifiName : g_stat.net == 1 ? std::wstring(L2(L"Cabo", L"Cable")) : std::wstring(L2(L"Sem rede", L"Offline")),
                          g_stat.net != 0, [] { OpenPanelFromCentral(StNetwork); }});
    if ((ExtraPlace(StBt) & PlCentral))
    {
        BtView bv = BtNow();
        std::wstring st = bv.radio < 0 ? L2(L"Indisponível", L"Unavailable") : bv.radio == 0 ? L2(L"Desligado", L"Off") : L2(L"Ligado", L"On");
        for (auto& dv : bv.devs)
            if (bv.radio == 1 && dv.connected) {
                st = dv.name;
                break;
            }
        rounds.push_back({StatIcon(StBt), L"", L"Bluetooth", st, bv.radio == 1, [] { OpenPanelFromCentral(StBt); }});
    }
    if ((ExtraPlace(StDnd) & PlCentral))
        rounds.push_back({StatIcon(StDnd), L"", L2(L"Não incomodar", L"Do not disturb"),
                          g_set.notifDnd ? std::wstring(L2(L"Ligado", L"On")) : std::wstring(L2(L"Desligado", L"Off")), g_set.notifDnd,
                          [] { SetDnd(!g_set.notifDnd); }, [] { FlyEnter(StDnd); }});
    if ((ExtraPlace(StNight) & PlCentral)) {
        int st = NightState();
        rounds.push_back({StatIcon(StNight), L"", L2(L"Luz noturna", L"Night light"),
                          st < 0 ? std::wstring(L2(L"Configurar", L"Set up")) : st ? std::wstring(L2(L"Ligada", L"On")) : std::wstring(L2(L"Desligada", L"Off")),
                          st == 1, [st] {
                              if (st < 0 || !NightSet(st == 0)) FlyEnter(StNight);
                          }, [] { FlyEnter(StNight); }});
    }
    if (StatOn(StKeyboard) && (g_set.statPlace[StKeyboard] & PlCentral))
        rounds.push_back({nullptr, g_stat.cur ? KbShort(g_stat.cur) : L"??", L2(L"Teclado", L"Keyboard"), L2(L"escolher", L"choose"), false,
                          [] { FlyEnter(StKeyboard); }});
    if (StatOn(StBattery) && (g_set.statPlace[StBattery] & PlCentral))
        rounds.push_back({StatIcon(StBattery), L"", L2(L"Bateria", L"Battery"), std::to_wstring(g_stat.pct) + L"%", g_stat.charging,
                          [] { OpenPanelFromCentral(StBattery); }});
    float cw = (kFlyW - kFlyPad * 2) / 4;
    for (size_t k = 0; k < rounds.size(); k++) {
        if (k && k % 4 == 0) y += 94;
        auto& r = rounds[k];
        FRound(kFlyPad + (k % 4) * cw + cw / 2, y, r.icon, r.text, r.name, r.state, r.on, r.fn, r.more);
    }
    if (!rounds.empty()) y += 94;
    if ((ExtraPlace(StShort) & PlCentral)) y = SecShort(y) + 2;
    if ((ExtraPlace(StLight) & PlCentral)) {
        FSep(y);
        y = SecLight(y, true);
    }
    if (StatOn(StVolume) && (g_set.statPlace[StVolume] & PlCentral)) {
        FSep(y);
        y = SecSound(y);
    }
    if ((g_set.trayPlace & PlCentral)) {
        FSep(y);
        FSectionHead(y, L2(L"Em segundo plano", L"In the background"), L"", [] { FlyEnter(StTrayBtn); });
        float x = kFlyPad - 4;
        for (int ti : TrayOrder()) {
            if (x + 34 > kFlyW - kFlyPad + 4) {
                x = kFlyPad - 4;
                y += 36;
            }
            D2D1_RECT_F r = D2D1::RectF(x, y, x + 34, y + 34);
            if (FHot(r)) FFill(r, 9, FHov());
            FTrayIcon(ti, x + 17, y + 17, 20);
            TrayPanelHit(r, g_tray[ti].key);
            x += 36;
        }
        y += 38;
    }
    if ((ExtraPlace(StPower) & PlCentral)) {
        FSep(y);
        y = SecPower(y, true);
    }
    // Items with their own panel: a row that opens it.
    struct Ln {
        int id;
        std::wstring text;
    };
    std::vector<Ln> links;
    if (StatOn(StClock) && (g_set.statPlace[StClock] & PlCentral)) links.push_back({StClock, L2(L"Calendário", L"Calendar")});
    if ((g_set.notifPlace & PlCentral)) links.push_back({StNotif, std::wstring(L2(L"Notificações", L"Notifications")) + L" · " + std::to_wstring(g_notifs.size())});
    if ((g_set.notesPlace & PlCentral)) links.push_back({StNotes, std::wstring(L2(L"Notas e lembretes", L"Notes and reminders")) + L" · " + std::to_wstring(RemPending())});
    if ((g_set.shelfPlace & PlCentral)) links.push_back({StShelf, std::wstring(L2(L"Prateleira", L"Shelf")) + L" · " + std::to_wstring(g_shelf.size())});
    if ((ExtraPlace(StDevices) & PlCentral) && !DevicesNow().empty()) {
        std::wstring t = L2(L"Dispositivos", L"Devices");
        for (auto& dv : DevicesNow())
            if (dv.percent >= 0) t += L" · " + std::to_wstring(dv.percent) + L"%";
        links.push_back({StDevices, t});
    }
    if (!links.empty()) {
        FSep(y);
        for (auto& l : links) {
            D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y, kFlyW - kFlyPad + 8, y + 36);
            if (FHot(r)) FFill(r, 8, FHov());
            FIcon(StatIcon(l.id == StClock ? StCentral : l.id), kFlyPad + 9, y + 18, 17, FCol(g_theme.text, 0.85f));
            if (l.id == StClock) FIcon(L"M4 6h16v14H4z M4 10h16 M8 3v5 M16 3v5", kFlyPad + 9, y + 18, 17, FCol(g_theme.text, 0.85f));
            FText(l.text, g_fly.fText.Get(), kFlyPad + 28, y, 260, 36, FInk());
            FIcon(L"M9 6l6 6-6 6", kFlyW - kFlyPad - 6, y + 18, 13, FSub());
            int id = l.id;
            FHitAdd(r, [id] { OpenPanelFromCentral(id); });
            y += 38;
        }
    }
    FSep(y);
    FLink(y, L2(L"Abrir os controles do Windows", L"Open Windows' controls"), [] {
        CloseFlyout();
        SendWinKey('A');
    });
    return y;
}

// A new item on its own (in the dock or from a Start tile).
static float PanelExtra(int id, float y) {
    switch (id) {
        case StDnd:
            FHeader(y, L2(L"Não incomodar", L"Do not disturb"), L"");
            FText(L2(L"Os avisos do Nimbus não aparecem; continuam guardados no sino.", L"Nimbus' notifications don't show; the bell still keeps them."),
                  g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 22, FSub());
            y += 28;
            y = FSwitch(y, L2(L"Ligado", L"On"), g_set.notifDnd, [] { SetDnd(!g_set.notifDnd); });
            return y;
        case StBt: {
            BtView bv = BtNow();
            FHeader(y, L"Bluetooth", bv.busy == L"radio" ? L2(L"um momento…", L"one moment…") : L"");
            if (!bv.scanned) {
                FText(L2(L"Procurando…", L"Looking…"), g_fly.fSmall.Get(), kFlyPad, y, 300, 28, FSub());
                y += 28;
            } else if (bv.radio < 0) {
                FText(L2(L"Este computador não tem Bluetooth (ou ele está desativado no Windows).", L"This computer has no Bluetooth (or it's off in Windows)."),
                      g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 22, FSub());
                y += 30;
            } else {
                y = FSwitch(y, L2(L"Ligado", L"On"), bv.radio == 1, [on = bv.radio == 1] { BtRequest(on ? 0 : 1, L"", L""); }) + 6;
                if (bv.radio == 1) {
                    FSep(y);
                    if (bv.devs.empty()) {
                        FText(L2(L"Nenhum aparelho pareado.", L"No paired devices."), g_fly.fSmall.Get(), kFlyPad, y, 300, 28, FSub());
                        y += 28;
                    }
                    for (auto& dv : bv.devs) {
                        D2D1_RECT_F row = D2D1::RectF(kFlyPad - 8, y, kFlyW - kFlyPad + 8, y + 48);
                        bool busy = bv.busy == dv.addr;
                        // icon: headphones for audio, a generic device otherwise
                        FIcon(dv.audio ? L"M4 14v-2a8 8 0 0 1 16 0v2 M4 14h3v6H4z M17 14h3v6h-3z" : L"M7 3h10v18H7z M11 18h2",
                              kFlyPad + 10, y + 24, 18, dv.connected ? FInk() : FSub());
                        FText(dv.name, g_fly.fText.Get(), kFlyPad + 30, y + 5, 190, 0, FInk());
                        std::wstring sub = busy ? (dv.connected ? L2(L"Desconectando…", L"Disconnecting…") : L2(L"Conectando…", L"Connecting…"))
                                                : dv.connected ? L2(L"Conectado", L"Connected") : L2(L"Pareado", L"Paired");
                        if (dv.battery >= 0) sub += L" · " + std::to_wstring(dv.battery) + L"%";
                        FText(sub, g_fly.fSmall.Get(), kFlyPad + 30, y + 25, 190, 0, FSub());
                        if (dv.audio && !busy) {
                            const wchar_t* label = dv.connected ? L2(L"Desconectar", L"Disconnect") : L2(L"Conectar", L"Connect");
                            float bw = TextSize(label, g_fly.fSmall.Get()).width + 22;
                            D2D1_RECT_F b = D2D1::RectF(kFlyW - kFlyPad - bw, y + 10, kFlyW - kFlyPad, y + 38);
                            FFill(b, 14, FHot(b) ? FHov() : FCol(g_theme.text, 0.08f));
                            FText(label, g_fly.fSmall.Get(), b.left, b.top, bw, 28, FInk(), DWRITE_TEXT_ALIGNMENT_CENTER);
                            std::wstring addr = dv.addr;
                            bool con = dv.connected;
                            FHitAdd(b, [addr, con] { BtRequest(-1, con ? L"" : addr, con ? addr : L""); });
                        }
                        (void)row;
                        y += 50;
                    }
                }
            }
            FSep(y);
            // (Windows' new "Add device" is on that page; the old wizard is the only one that opens on its own)
            FLink(y, L2(L"Adicionar aparelho e mais opções", L"Add a device and more"), [] {
                CloseFlyout();
                OpenUri(L"ms-settings:bluetooth");
            });
            return y;
        }
        case StNight: {
            int st = NightState();
            FHeader(y, L2(L"Luz noturna", L"Night light"), st < 0 ? L"" : st ? L2(L"Ligada", L"On") : L2(L"Desligada", L"Off"));
            if (st >= 0) y = FSwitch(y, L2(L"Ligada", L"On"), st == 1, [st] { NightSet(st == 0); }) + 4;
            else {
                FText(L2(L"Configure uma vez nas Configurações do Windows pra ligar daqui.", L"Set it up once in Windows' Settings to switch it here."),
                      g_fly.fSmall.Get(), kFlyPad, y, kFlyW - kFlyPad * 2, 22, FSub());
                y += 28;
            }
            FLink(y, L2(L"Abrir a luz noturna do Windows", L"Open Windows' night light"), [] {
                CloseFlyout();
                OpenUri(L"ms-settings:nightlight");
            });
            return y;
        }
        case StShort:
            FHeader(y, L2(L"Atalhos", L"Shortcuts"), L"");
            return SecShort(y);
        case StLight:
            FHeader(y, L2(L"Brilho", L"Brightness"), L2(L"por monitor", L"each monitor"));
            y = SecLight(y, false);
            FSep(y);
            FLink(y, L2(L"Abrir as configurações de tela", L"Open display settings"), [] {
                CloseFlyout();
                OpenUri(L"ms-settings:display");
            });
            return y;
        case StPower:
            FHeader(y, L2(L"Energia", L"Power"), L"");
            return SecPower(y, false);
    }
    return y;
}

static float FlyContent() {
    float y = kFlyPad - (g_fly.measuring ? 0 : g_fly.panelScroll);
    if (g_fly.back) {
        D2D1_RECT_F r = D2D1::RectF(kFlyPad - 8, y - 6, kFlyPad + 170, y + 24);
        bool hot = FHot(r);
        if (hot) FFill(r, 8, FHov());
        FIcon(L"M15 6l-6 6 6 6", kFlyPad + 3, y + 9, 13, hot ? FInk() : FSub());
        FText(L2(L"Central de controle", L"Control center"), g_fly.fSmall.Get(), kFlyPad + 16, y - 6, 150, 30, hot ? FInk() : FSub());
        int back = g_fly.back;
        FHitAdd(r, [back] { FlyEnter(back); });
        y += 34;
    }
    switch (g_fly.kind) {
        case StClock: y = PanelClock(y); break;
        case StVolume: y = PanelVolume(y); break;
        case StNetwork: y = PanelNetwork(y); break;
        case StKeyboard: y = PanelKeyboard(y); break;
        case StBattery: y = PanelBattery(y); break;
        case StTrayBtn: y = PanelTray(y); break;
        case StShelf: y = PanelShelf(y); break;
        case StNotif: y = PanelNotif(y); break;
        case StNotes: y = PanelNotes(y); break;
        case StCentral: y = PanelCentral(y); break;
        case StDnd:
        case StBt:
        case StNight:
        case StShort:
        case StLight:
        case StPower: y = PanelExtra(g_fly.kind, y); break;
        case StPlayer: y = PanelPlayer(y); break;
        case StDevices: y = PanelDevices(y); break;
    }
    return y + kFlyPad - 6;
}

// ---- The window ----

static void FlyPlace(int w, int h) {
    auto& f = g_fly;
    Dock& d = *f.dock;
    float k = f.scale;
    int x, y;
    if (f.fromStart && g_start.open) {
        x = g_start.rc.right + (int)(12 * k);
        if (x + w > d.rcMon.right - 8) x = g_start.rc.left - (int)(12 * k) - w;
        y = g_start.rc.bottom - h;
    } else {
        int dockTop = d.rcMon.bottom - d.ch + (int)d.pill.top;
        y = dockTop - (int)(10 * k) - h;
        if (d.cfg.fixed) x = StatOnLeft() ? d.rcMon.left + (int)(12 * k) : d.rcMon.right - (int)(12 * k) - w;
        else x = StatOnLeft() ? d.rcMon.left + (int)d.pill.left : d.rcMon.left + (int)d.pill.right - w;
        // The tray's, shelf's and notes' panels: above their item. Notifications: at the right edge,
        // like Windows' own.
        int pk = f.back ? f.back : f.kind;  // (a panel opened inside the control center stays where it is)
        if (pk == StTrayBtn || pk == StShelf || pk == StNotes || pk == StCentral || IsExtra(pk))
            for (auto& it : d.stat)
                if (it.id == pk) x = d.rcMon.left + (int)(it.x + it.w / 2) - w / 2;
        if (pk == StNotif) x = d.rcMon.right - (int)(12 * k) - w;
    }
    x = std::clamp(x, (int)d.rcMon.left + 8, (int)d.rcMon.right - w - 8);
    y = std::max(y, (int)d.rcMon.top + 8);
    f.rc = {x, y, x + w, y + h};
}

static void FlyRender() {
    auto& f = g_fly;
    if (!f.open || !f.dock) return;
    // (A slow frame is logged with its panel: they're drawn on every mouse move.)
    struct SlowFrame {
        DWORD t0 = GetTickCount();
        ~SlowFrame() {
            DWORD dt = GetTickCount() - t0;
            if (dt > 30) Wh_Log(L"panel %d: a frame took %lu ms", g_fly.kind, dt);
        }
    } slow;
    if (!f.rt) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
        g_d2d->CreateDCRenderTarget(&props, &f.rt);
        if (!f.rt) return;
        f.rt.As(&f.dc);
        f.rt->CreateSolidColorBrush(D2D1::ColorF(0, 0), &f.br);
        f.pageLayer.Reset();
        f.bmps.clear();
    }
    // First the height (the panel's content decides it), then the drawing.
    g_ned.want = g_ntime.want = false;
    f.measuring = true;
    f.hits.clear();
    {
        Dock& d = *f.dock;
        int dockTop = d.rcMon.bottom - d.ch + (int)d.pill.top;
        f.maxH = std::max(200.0f, (dockTop - d.rcMon.top - 30) / f.scale);
    }
    float H;
    bool sliding = f.page < 1 && f.fromH > 0;
    DWORD tm0 = GetTickCount();
    f.tMeasure = f.tDraw = f.tPresent = 0;
    if (sliding && f.pageMeasured) H = f.toH;
    else {
        H = FlyContent();
        // Taller than the room above the dock: as tall as the room, and the content scrolls.
        if (f.kind != StNotif && H > f.maxH + 0.5f) {
            f.panelMax = H - f.maxH;
            H = f.maxH;
        } else {
            f.panelMax = 0;
        }
        f.panelScroll = std::clamp(f.panelScroll, 0.0f, f.panelMax);
        f.toH = H;
        f.pageMeasured = true;
    }
    f.tMeasure = GetTickCount() - tm0;
    f.measuring = false;
    float pe = 1;
    if (f.page < 1 && f.fromH > 0) {
        float q = std::clamp(f.page, 0.0f, 1.0f);
        pe = 1 - (1 - q) * (1 - q) * (1 - q);
        H = f.fromH + (H - f.fromH) * pe;
    }
    f.curH = H;
    int w = (int)std::ceil(kFlyW * f.scale), h = (int)std::ceil(H * f.scale);
    int bh = sliding ? (int)std::ceil(std::max({f.fromH, f.toH, H}) * f.scale) : h;
    if (w != f.dibW || (sliding ? bh > f.dibH : h != f.dibH) || !f.memDC) {
        h = std::max(h, 1);
        bh = std::max(bh, h);
        if (f.memDC) {
            SelectObject(f.memDC, f.oldBmp);
            DeleteDC(f.memDC);
            DeleteObject(f.dib);
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -bh;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        f.memDC = CreateCompatibleDC(nullptr);
        f.dib = CreateDIBSection(f.memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        f.oldBmp = SelectObject(f.memDC, f.dib);
        f.dibW = w;
        f.dibH = bh;
    }
    f.w = w;
    f.h = h;
    FlyPlace(w, h);
    RECT rc = {0, 0, w, f.dibH};
    if (FAILED(f.rt->BindDC(f.memDC, &rc))) return;
    DWORD td0 = GetTickCount();
    f.rt->BeginDraw();
    f.rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    f.rt->SetTransform(D2D1::Matrix3x2F::Scale(f.scale, f.scale));
    const Theme& T = g_theme;
    D2D1_ROUNDED_RECT panel = D2D1::RoundedRect(D2D1::RectF(0, 0, kFlyW, H), kFlyR, kFlyR);
    f.rt->FillRoundedRectangle(panel, FBr(T.tint));
    D2D1_GRADIENT_STOP stops[] = {{0.0f, T.edge[0]}, {0.55f, T.edge[1]}, {1.0f, T.edge[2]}};
    ComPtr<ID2D1GradientStopCollection> coll;
    f.rt->CreateGradientStopCollection(stops, 3, &coll);
    ComPtr<ID2D1LinearGradientBrush> edge;
    f.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), T.edgeHorizontal ? D2D1::Point2F(kFlyW, 0)
                                                                                                                : D2D1::Point2F(0, H)),
                                    coll.Get(), &edge);
    float sw = 1 / f.scale * std::max(1.0f, f.scale);
    if (edge)
        f.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(sw / 2, sw / 2, kFlyW - sw / 2, H - sw / 2), kFlyR - sw / 2, kFlyR - sw / 2),
                                   edge.Get(), sw);
    f.hits.clear();
    if (pe < 1) {
        // The new panel: in from the side, fading in, cut to the panel's edges while the height moves.
        f.rt->PushAxisAlignedClip(D2D1::RectF(1, 1, kFlyW - 1, H - 1), D2D1_ANTIALIAS_MODE_ALIASED);
        f.rt->SetTransform(D2D1::Matrix3x2F::Translation((1 - pe) * 26 * f.pageDir, 0) * D2D1::Matrix3x2F::Scale(f.scale, f.scale));
        FlyContent();
        f.rt->SetTransform(D2D1::Matrix3x2F::Scale(f.scale, f.scale));
        f.rt->PopAxisAlignedClip();
        // (fading in: the panel's own colour over the content, lifting)
        D2D1_COLOR_F veil = T.tint;
        veil.a = std::max(veil.a, 0.85f) * (1 - pe) * 0.9f;
        f.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(1, 1, kFlyW - 1, H - 1), kFlyR - 1, kFlyR - 1), FBr(veil));
    } else if (f.panelMax > 0) {
        f.rt->PushAxisAlignedClip(D2D1::RectF(1, 1, kFlyW - 1, H - 1), D2D1_ANTIALIAS_MODE_ALIASED);
        FlyContent();
        f.rt->PopAxisAlignedClip();
        // A thin bar at the right edge: where the view is.
        float track = H - 2 * kFlyR, thumb = std::max(28.0f, track * H / (H + f.panelMax));
        float ty = kFlyR + (track - thumb) * (f.panelScroll / f.panelMax);
        D2D1_COLOR_F bc = T.text;
        bc.a *= 0.28f;
        f.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(kFlyW - 6, ty, kFlyW - 3, ty + thumb), 1.5f, 1.5f), FBr(bc));
    } else {
        FlyContent();
    }
    f.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    if (f.rt->EndDraw() == D2DERR_RECREATE_TARGET) {
        f.rt.Reset();
        f.pageLayer.Reset();
        f.dc.Reset();
        f.br.Reset();
        f.bmps.clear();
        return;
    }
    f.tDraw = GetTickCount() - td0;
    DWORD tp0 = GetTickCount();
    float p = std::clamp(f.anim, 0.0f, 1.0f), eased = 1 - (1 - p) * (1 - p) * (1 - p);
    POINT dst = {f.rc.left, f.rc.top + (LONG)std::lround((1 - eased) * 10 * f.scale)};
    SIZE size = {w, h};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)std::lround(255 * eased), AC_SRC_ALPHA};
    UpdateLayeredWindow(f.hwnd, nullptr, &dst, &size, f.memDC, &src, 0, &bf, ULW_ALPHA);
    NoteEditPlaceAll();
    // The blur only once the panel has landed (it can't fade or move with it).
    if (p >= 1)
        PlaceBands(f.bands, f.bandRc, T.blur, (float)f.rc.left, (float)f.rc.top, (float)f.rc.right, (float)f.rc.bottom,
                   kFlyR * f.scale);
    else
        HideBands(f.bands, f.bandRc);
    f.tPresent = GetTickCount() - tp0;
}


static void FlyFonts() {
    auto& f = g_fly;
    if (f.fText) return;
    auto make = [](const wchar_t* fam, DWRITE_FONT_WEIGHT wt, float size, ComPtr<IDWriteTextFormat>& out) {
        g_dw->CreateTextFormat(fam, nullptr, wt, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, g_locale, &out);
        if (!out) return;
        out->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> dots;
        g_dw->CreateEllipsisTrimmingSign(out.Get(), &dots);
        out->SetTrimming(&trim, dots.Get());
    };
    make(L"Segoe UI Variable Display", DWRITE_FONT_WEIGHT_LIGHT, 34, f.fBig);
    make(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_SEMI_BOLD, 14, f.fTitle);
    make(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL, 13.5f, f.fText);
    make(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL, 12.5f, f.fSmall);
    make(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_SEMI_BOLD, 12.5f, f.fCode);
    make(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL, 13.5f, f.fWrap);
    if (f.fWrap) f.fWrap->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    make(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL, 12.5f, f.fWrapS);
    if (f.fWrapS) f.fWrapS->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
}

static void FlyLoad(int kind);
static void OpenFlyout(Dock& d, int kind, bool fromStart) {
    auto& f = g_fly;
    HidePreview();
    FlyFonts();
    f.open = true;
    f.kind = kind;
    f.dock = &d;
    f.fromStart = fromStart;
    f.scale = fromStart ? g_start.scale : d.scale;
    f.month = 0;
    f.pick = -1;
    f.dragging = false;
    f.mouse = {-1, -1};
    f.back = 0;
    f.panelScroll = f.panelMax = 0;
    f.page = 1;
    KillTimer(f.hwnd, TIMER_FLY_PAGE);
    FlyLoad(kind);
    f.press = -1;
    f.listScroll = 0;
    f.anim = 0;
    f.animStart = GetTickCount();
    HideBands(f.bands, f.bandRc);
    FlyRender();
    SetWindowPos(f.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    f.openedAt = GetTickCount();
    if (!fromStart && !f.noFocus) ForceForeground(f.hwnd);
    SetTimer(f.hwnd, TIMER_FLY_ANIM, 15, nullptr);
    f.wasDown = true;  // (the click that opened it may still be down)
    SetTimer(f.hwnd, TIMER_FLY_WATCH, 40, nullptr);
    Invalidate(d);
    if (fromStart) RenderStart();
}

// What a panel reads when it opens.
static void FlyLoad(int kind) {
    auto& f = g_fly;
    if (kind == StVolume) {
        ReadVolume();
        LoadAppVolumes();
        for (auto& a : g_appVols) {
            if (a.path.empty()) continue;
            ComPtr<IShellItem> item;
            if (SUCCEEDED(SHCreateItemFromParsingName(a.path.c_str(), nullptr, IID_PPV_ARGS(&item)))) a.icon = IconFromItem(item.Get(), 32);
        }
    } else if (kind == StNetwork) {
        ReadNetwork();
        LoadWifiList();
        ScanWifi();
        SetTimer(f.hwnd, TIMER_FLY_WIFI, 4000, nullptr);
    } else if (kind == StBattery) {
        ReadBattery();
    } else if (kind == StKeyboard) {
        ReadKeyboard();
    } else if (kind == StClock) {
        UpdateClock();
    } else if ((kind == StCentral && (ExtraPlace(StLight) & PlCentral)) || kind == StLight) {
        CcRefresh();
        BrightScan();
    } else if (kind == StPlayer) {
        LoadAppVolumes();
        MediaKick();
    } else if (kind == StShelf) {
        if (ShelfPrune()) SaveShelf();
        g_shelfUi.over = false;
    }
}

// Another panel in the same window (the control center's items; "back" returns to it). The panel
// doesn't close: only a click outside it does.
static void FlyEnter(int kind) {
    auto& f = g_fly;
    if (!f.open) return;
    KillTimer(f.hwnd, TIMER_FLY_WIFI);
    NoteEditHideAll();
    g_notesUi.editing = -1;
    g_notesUi.autoOpened = false;
    g_notesUi.creating = false;
    g_appVols.clear();
    f.back = kind == StCentral ? 0 : StCentral;
    f.kind = kind;
    f.month = 0;
    f.pick = -1;
    f.press = -1;
    f.listScroll = 0;
    f.panelScroll = 0;
    f.pageDir = kind == StCentral ? -1 : 1;
    f.fromH = f.curH;
    f.page = ReduceMotion() ? 1.0f : 0.0f;
    f.pageMeasured = false;
    f.pageStart = GetTickCount();
    SetTimer(f.hwnd, TIMER_FLY_PAGE, 15, nullptr);
    FlyLoad(kind);
    FlyRender();
}

static void NotesSave(bool closing);
void CloseFlyout() {
    auto& f = g_fly;
    if (!f.open) return;
    f.open = false;
    KillTimer(f.hwnd, TIMER_FLY_ANIM);
    KillTimer(f.hwnd, TIMER_FLY_PAGE);
    f.page = 1;
    KillTimer(f.hwnd, TIMER_FLY_WIFI);
    KillTimer(f.hwnd, TIMER_FLY_WATCH);
    if (f.dragging || f.press >= 0) ReleaseCapture();
    f.press = -1;
    if (g_notesUi.tab == 0 && g_notesUi.editing >= 0) NotesSave(true);  // (a note being written isn't lost)
    NoteEditHideAll();
    g_notesUi.editing = -1;
    g_notesUi.autoOpened = false;
    g_notesUi.creating = false;
    f.dragging = false;
    ShowWindow(f.hwnd, SW_HIDE);
    HideBands(f.bands, f.bandRc);
    f.bmps.clear();
    g_appVols.clear();
    bool fromStart = f.fromStart;
    if (f.dock) Invalidate(*f.dock);
    f.dock = nullptr;
    if (fromStart && g_start.open) RenderStart();
}

static void ToggleFlyout(Dock& d, int kind, bool fromStart) {
    auto& f = g_fly;
    bool same = f.open && (f.kind == kind || f.back == kind) && f.dock == &d && f.fromStart == fromStart;
    CloseFlyout();
    if (!same) OpenFlyout(d, kind, fromStart);
}

bool FlyoutShows(const Dock& d, int kind) { return g_fly.open && !g_fly.fromStart && g_fly.dock == &d && g_fly.kind == kind; }
bool FlyoutOpenOn(const Dock& d) { return g_fly.open && g_fly.dock == &d; }
bool CursorInFlyout(POINT pt) { return g_fly.open && PtInRect(&g_fly.rc, pt); }
bool FlyoutShowsKind(int kind) { return g_fly.open && g_fly.kind == kind; }
bool FlyoutFromStartOpen() { return g_fly.open && g_fly.fromStart; }

static int FlyHitAt(D2D1_POINT_2F p) {
    for (int i = (int)g_fly.hits.size() - 1; i >= 0; i--) {
        const D2D1_RECT_F& r = g_fly.hits[i].r;
        if (p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom) return i;
    }
    return -1;
}

static void FlySlide(float mx) {
    auto& h = g_fly.drag;
    if (h.slide && h.x1 > h.x0) h.slide(std::clamp((mx - h.x0) / (h.x1 - h.x0), 0.0f, 1.0f));
}

LRESULT CALLBACK FlyProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto& f = g_fly;
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return f.fromStart ? MA_NOACTIVATE : MA_ACTIVATE;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && f.open && !f.fromStart && GetTickCount() - f.openedAt > 150 && !TrayClickJustNow() &&
                !(g_ned.host && (HWND)lp == g_ned.host) && !(g_ntime.host && (HWND)lp == g_ntime.host))
                CloseFlyout();
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) {
                if (f.back) FlyEnter(f.back);
                else CloseFlyout();
            }
            else if (wp == 'V' && (GetKeyState(VK_CONTROL) & 0x8000) && f.open && f.kind == StShelf) ShelfPaste();
            return 0;
        case WM_APP_NOTE_SAVE:
            NotesSave();
            return 0;
        case WM_APP_NOTE_TIME: {
            int m = TimeBoxMinutes();
            if (m >= 0) g_notesUi.minutes = m;
            FlyRender();
            return 0;
        }
        case WM_APP_NOTE_CANCEL:
            NotesCancel();
            return 0;
        case WM_APP_FLY_OUTSIDE:
            CloseFlyout();
            return 0;
        case WM_MOUSEMOVE: {
            if (!f.open) return 0;
            bool was = f.mouse.x >= 0;
            f.mouse = D2D1::Point2F(GET_X_LPARAM(lp) / f.scale, GET_Y_LPARAM(lp) / f.scale);
            if (!was) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
            }
            if (f.dragging) FlySlide(f.mouse.x);
            if (f.press >= 0 && (std::fabs(f.mouse.x - f.pressPt.x) > 4 || std::fabs(f.mouse.y - f.pressPt.y) > 4)) {
                int i = f.press;
                bool right = f.pressRight;
                f.press = -1;
                ReleaseCapture();
                if (i < (int)f.hits.size() && !f.hits[i].trayKey.empty() && !right) {
                    f.trayDrag = f.hits[i].trayKey;
                    SetCapture(hwnd);
                } else if (i < (int)f.hits.size() && f.hits[i].dragOut) {
                    auto out = f.hits[i].dragOut;
                    out(right);  // (modal until the drop)
                    f.mouse = {-1, -1};
                    if (f.open) FlyRender();
                    return 0;
                }
            }
            FlyRender();
            return 0;
        }
        case WM_MOUSELEAVE:
            f.mouse = {-1, -1};
            FlyRender();
            return 0;
        case WM_LBUTTONDOWN: {
            if (!f.open) return 0;
            f.mouse = D2D1::Point2F(GET_X_LPARAM(lp) / f.scale, GET_Y_LPARAM(lp) / f.scale);
            int i = FlyHitAt(f.mouse);
            if (i >= 0 && f.hits[i].slide) {
                f.drag = f.hits[i];
                f.dragging = true;
                SetCapture(hwnd);
                FlySlide(f.mouse.x);
                FlyRender();
            } else if (i >= 0 && (f.hits[i].dragOut || !f.hits[i].trayKey.empty())) {
                f.press = i;
                f.pressPt = f.mouse;
                f.pressRight = false;
                SetCapture(hwnd);
            }
            return 0;
        }
        case WM_RBUTTONDOWN: {
            if (!f.open) return 0;
            f.mouse = D2D1::Point2F(GET_X_LPARAM(lp) / f.scale, GET_Y_LPARAM(lp) / f.scale);
            int i = FlyHitAt(f.mouse);
            if (i >= 0 && f.hits[i].dragOut) {
                f.press = i;
                f.pressPt = f.mouse;
                f.pressRight = true;
                SetCapture(hwnd);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!f.open) return 0;
            if (!f.trayDrag.empty()) {
                std::wstring drag = f.trayDrag;
                f.trayDrag.clear();
                ReleaseCapture();
                D2D1_POINT_2F pt = D2D1::Point2F(GET_X_LPARAM(lp) / f.scale, GET_Y_LPARAM(lp) / f.scale);
                int i = FlyHitAt(pt);
                if (i >= 0 && !f.hits[i].trayKey.empty() && f.hits[i].trayKey != drag) {
                    const D2D1_RECT_F& r = f.hits[i].r;
                    TrayMove(drag, f.hits[i].trayKey, pt.x > (r.left + r.right) / 2);
                    TrayChanged();
                }
                FlyRender();
                return 0;
            }
            if (f.press >= 0) {
                f.press = -1;
                ReleaseCapture();
            }
            if (f.dragging) {
                f.dragging = false;
                ReleaseCapture();
                FlyRender();
                return 0;
            }
            int i = FlyHitAt(D2D1::Point2F(GET_X_LPARAM(lp) / f.scale, GET_Y_LPARAM(lp) / f.scale));
            if (i >= 0 && f.hits[i].click) {
                auto click = f.hits[i].click;
                click();
                if (f.open) FlyRender();
                for (auto* d : g_docks) Invalidate(*d);
            }
            return 0;
        }
        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            if (!f.open) return 0;
            if (f.press >= 0) {
                f.press = -1;
                ReleaseCapture();
            }
            int i = FlyHitAt(D2D1::Point2F(GET_X_LPARAM(lp) / f.scale, GET_Y_LPARAM(lp) / f.scale));
            if (i >= 0 && f.hits[i].button) {
                auto button = f.hits[i].button;
                button(msg == WM_RBUTTONUP ? 1 : 2);
                if (f.open) FlyRender();
            }
            return 0;
        }
        case WM_MOUSEWHEEL:
            if (f.open && f.panelMax > 0) {
                f.panelScroll = std::clamp(f.panelScroll - GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * 64, 0.0f, f.panelMax);
                FlyRender();
            } else if (f.open && f.kind == StVolume) {
                SetVolume(g_stat.vol + GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * 0.02f);
                FlyRender();
            } else if (f.open && f.kind == StClock) {
                f.month += GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1;
                FlyRender();
            } else if (f.open && f.kind == StNotif && f.listMax > 0) {
                f.listScroll = std::clamp(f.listScroll - GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * 76, 0.0f, f.listMax);
                FlyRender();
            }
            return 0;
        case WM_CAPTURECHANGED:
            if ((HWND)lp != hwnd) {
                f.dragging = false;
                f.press = -1;
            }
            return 0;
        case WM_TIMER:
            if (wp == TIMER_FLY_PAGE) {
                f.page = ReduceMotion() ? 1.0f : std::min(1.0f, (GetTickCount() - f.pageStart) / 220.0f);
                if (f.page >= 1) KillTimer(hwnd, TIMER_FLY_PAGE);
                FlyRender();
            } else if (wp == TIMER_FLY_ANIM) {
                f.anim = ReduceMotion() ? 1.0f : std::min(1.0f, (GetTickCount() - f.animStart) / 150.0f);
                if (f.anim >= 1) KillTimer(hwnd, TIMER_FLY_ANIM);
                FlyRender();
            } else if (wp == TIMER_FLY_WIFI) {
                KillTimer(hwnd, TIMER_FLY_WIFI);
                LoadWifiList();
                FlyRender();
            } else if (wp == TIMER_FLY_TRAY) {
                KillTimer(hwnd, TIMER_FLY_TRAY);
                if (TrayClickJustNow()) SetTimer(hwnd, TIMER_FLY_TRAY, 150, nullptr);
                else CloseFlyout();
            } else if (wp == TIMER_FLY_SHELF) {
                KillTimer(hwnd, TIMER_FLY_SHELF);
                if (ShelfPrune()) ShelfChanged();
            } else if (wp == TIMER_FLY_WATCH) {
                // A click on a window run as administrator: the mouse hook never sees it.
                SHORT l = GetAsyncKeyState(VK_LBUTTON), r = GetAsyncKeyState(VK_RBUTTON), m = GetAsyncKeyState(VK_MBUTTON);
                bool down = ((l | r | m) & 0x8000) != 0, tapped = ((l | r | m) & 1) != 0;  // (a quick tap between checks)
                if ((down || tapped) && !f.wasDown) {
                    POINT pt;
                    GetCursorPos(&pt);
                    if (!PtInRect(&f.rc, pt)) {
                        HWND under = WindowFromPoint(pt);
                        wchar_t cls[64] = {};
                        if (under) GetClassNameW(GetAncestor(under, GA_ROOT), cls, ARRAYSIZE(cls));
                        if (wcscmp(cls, L"NimbusMenu_Content") && wcscmp(cls, L"NimbusMenu_Start") && wcscmp(cls, L"#32768"))
                            CloseFlyout();
                    }
                }
                f.wasDown = down;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Something the status area shows changed: the docks, the open panel and the Start's Status tab follow.
static void StatusChanged() {
    for (auto* d : g_docks) {
        BuildElems(*d, false);
        Layout(*d);
        Invalidate(*d);
    }
    if (g_fly.open && !g_fly.dragging) FlyRender();
    if (g_start.open) RenderStart();
}

// A click on a status item (in the dock, or a tile in the Start's Status tab).
static void StatClick(Dock& d, int id, bool right, bool fromStart) {
    if (id == StShelf || id == StNotif || id == StNotes || id == StCentral || IsExtra(id)) {
        ToggleFlyout(d, id, fromStart);
        return;
    }
    if (id == StKeyboard && !right && !fromStart) {
        CloseFlyout();
        NextKeyboard();
        StatusChanged();
        return;
    }
    if (g_set.statusWinPanels) {
        CloseFlyout();
        if (fromStart) CloseStart(true);
        if (id == StClock) OpenUri(L"ms-actioncenter:");
        else if (id == StKeyboard) SendWinKey(VK_SPACE);
        else SendWinKey('A');
        return;
    }
    ToggleFlyout(d, id, fromStart);
}

static void Animate(Dock& d);

static void TrayScroll(int delta) {
    int o = std::clamp(g_trayUi.off + delta, 0, TrayMaxOff());
    if (o == g_trayUi.off) return;
    g_trayUi.off = o;
    for (auto* d : g_docks) Animate(*d);
}

// A click on a tray piece in the dock. button: 0 left, 1 right, 2 middle.
static void TrayDockClick(Dock& d, int si, float x, int button) {
    if (si < 0 || si >= (int)d.stat.size()) return;
    int id = d.stat[si].id;
    if (id == StTrayBtn) {
        if (button != 0) return;
        if (TrayStyle(L"botao")) {
            CloseFlyout();
            g_trayUi.open = !g_trayUi.open;
            StatusChanged();
        } else {
            ToggleFlyout(d, StTrayBtn, false);
        }
        return;
    }
    if (id == StTrayPrev || id == StTrayNext) {
        if (button == 0) TrayScroll(id == StTrayPrev ? -2 : 2);
        return;
    }
    int ti = TrayIconAt(d, si, x);
    if (ti < 0) return;
    CloseFlyout();
    if (button == 2) {
        TrayTogglePin(ti);
        TrayChanged();
        return;
    }
    TrayClick(ti, button);
}

// Once a second (and on every poll for the keyboard, which changes with the window in front).
static void StatusTick(bool second) {
    bool changed = ReadKeyboard();
    if (second) {
        static DWORD lastNet = 0, lastBat = 0;
        DWORD now = GetTickCount();
        if (UpdateClock()) changed = true;
        if (now - lastNet >= 5000) {
            lastNet = now;
            if (ReadNetwork()) changed = true;
        }
        if (now - lastBat >= 30000) {
            lastBat = now;
            if (ReadBattery()) changed = true;
        }
        static DWORD lastPrune = 0;
        if (now - lastPrune >= 5000) {
            lastPrune = now;
            TrayPrune();
        }
        NotifTick();
        RemTick();
        AppsWatchTick();
    }
    if (changed) StatusChanged();
}

constexpr UINT_PTR TIMER_WIZARD = 36;
static bool WizardFirstRun() {
    std::vector<wchar_t> b(16);
    if (Wh_GetStringValue((UserPrefix() + L"wizard.done").c_str(), b.data(), b.size())) return false;
    std::wstring v;
    for (const wchar_t* k : {L"style", L"format", L"statusClock", L"trayStyle", L"animation", L"winKeyMode"})
        if (ReadVal(UserPrefix() + L"cfg." + k, v)) return false;  // (already set up: a 1.x or earlier 2.0 user)
    return true;
}

static void StatusInit() {
    if (g_msgWnd && WizardFirstRun()) SetTimer(g_msgWnd, TIMER_WIZARD, 2500, nullptr);
    UpdateClock();
    OpenAudio();
    OpenWlan();
    ReadNetwork();
    ReadBattery();
    ReadKeyboard();
    TrayInit();
    LoadShelf();
    NotifInit();
    LoadNotes();
}

static void StatusUninit() {
    // What Windows keeps goes back first (if a worker below is slow, nothing is left changed).
    RestoreBanners();
    BrightSignalStop();
    BtSignalStop();
    MediaSignalStop();
    DevSignalStop();
    CloseFlyout();
    TrayUninit();
    ShelfUninit();
    {
        DWORD deadline = GetTickCount() + 3000;  // (Windhawk gives the whole unload 5 s)
        BrightJoin(deadline);
        BtJoin(deadline);
        MediaJoin(deadline);
        DevJoin(deadline);
        while (g_ccBusy && (LONG)(deadline - GetTickCount()) > 0) Sleep(10);
    }
    if (g_msgWnd) KillTimer(g_msgWnd, TIMER_PLAYER_VIS);
    g_vis.meter.Reset();
    g_dockArt.clear();
    g_mediaArt.Reset();
    g_mediaArtGen = -1;
    AppsWatchStop();
    if (g_toast.hwnd) KillTimer(g_toast.hwnd, TIMER_TOAST);
    g_toast.cards.clear();
    g_notifReady = false;
    g_notifLast = 0;
    RestoreBanners();
    SqlClose();
    g_notifs.clear();
    g_notifApps.clear();
    CloseAudio();
    CloseWlan();
    g_stat.nlm.Reset();
    g_stat.tsf.Reset();
    g_appVols.clear();
}

LRESULT CALLBACK GlassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            FillRect(dc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}


LRESULT CALLBACK ContentProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Dock* d = DockFrom(hwnd);
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE:
            if (d) {
                if (g_trayDockDrag.down && g_trayDockDrag.dock == d) {
                    int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
                    if (!g_trayDockDrag.active && (abs(x - g_trayDockDrag.sx) > 6 || abs(y - g_trayDockDrag.sy) > 6)) {
                        g_trayDockDrag.active = true;
                        d->statHover = -1;
                        d->trayHot = -1;
                        SetTimer(hwnd, TIMER_TRAY_DRAGSCROLL, 280, nullptr);
                    }
                    if (g_trayDockDrag.active) {
                        g_trayDockDrag.mx = (float)x;
                        SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
                        Invalidate(*d);
                        return 0;
                    }
                }
                if (g_drag.down && g_drag.dock == d) {
                    int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
                    if (!g_drag.active && (abs(x - g_drag.sx) > 6 || abs(y - g_drag.sy) > 6)) {
                        g_drag.active = true;
                        KillTimer(hwnd, TIMER_PREVIEW);
                        HidePreview();
                    }
                    if (g_drag.active) {
                        g_drag.x = (float)x;
                        g_drag.y = (float)y;
                        d->mouseX = -1;
                        Invalidate(*d);
                        return 0;
                    }
                }
                if (d->mouseX < 0) {
                    TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                    TrackMouseEvent(&tme);
                }
                d->mouseX = (float)GET_X_LPARAM(lp);
                d->statHover = StatAt(*d, d->mouseX, (float)GET_Y_LPARAM(lp));
                int th = TrayIconAt(*d, d->statHover, d->mouseX);
                if (th != d->trayHot) {
                    d->trayHot = th;
                    TrayHover(th);
                }
                int e = ElemAt(*d, d->mouseX, (float)GET_Y_LPARAM(lp));
                if (e != d->hoverElem) {
                    d->hoverElem = e;
                    if (e >= 0) {
                        if (g_prev.visible) ShowPreview(*d, e);
                        else SetTimer(hwnd, TIMER_PREVIEW, 450, nullptr);
                    }
                }
                Invalidate(*d);
            }
            return 0;
        case WM_MOUSELEAVE:
            if (d) {
                KillTimer(hwnd, TIMER_PREVIEW);
                d->hoverElem = -1;
                d->statHover = -1;
                d->trayHot = -1;
                d->mouseX = -1;
                d->outsideSince = GetTickCount();
                Invalidate(*d);
            }
            return 0;
        case WM_LBUTTONUP:
            if (d) {
                TrayDockDrag td = g_trayDockDrag;
                g_trayDockDrag = {};
                KillTimer(hwnd, TIMER_TRAY_DRAGSCROLL);
                if (td.down) ReleaseCapture();
                if (td.active) {
                    int si, ti;
                    bool after;
                    float bx;
                    if (TrayDropAt(*d, (float)GET_X_LPARAM(lp), si, ti, after, bx) && g_tray[ti].key != td.key) {
                        TrayMove(td.key, g_tray[ti].key, after);
                        TrayChanged();
                    }
                    Invalidate(*d);
                    return 0;
                }
                Drag drag = g_drag;
                g_drag = {};
                if (drag.down) ReleaseCapture();
                if (drag.active) {
                    DropDrag(*d, drag.key, (float)GET_X_LPARAM(lp), (float)GET_Y_LPARAM(lp));
                    return 0;
                }
                float mx = (float)GET_X_LPARAM(lp), my = (float)GET_Y_LPARAM(lp);
                int si = StatAt(*d, mx, my);
                if (si >= 0) {
                    if (IsTrayKind(d->stat[si].id)) TrayDockClick(*d, si, mx, 0);
                    else StatClick(*d, d->stat[si].id, false, false);
                    return 0;
                }
                CloseFlyout();
                int e = ElemAt(*d, mx, my);
                KillTimer(hwnd, TIMER_PREVIEW);
                HidePreview();
                if (e >= 0) ClickApp(*d, g_apps[d->elems[e].app]);
            }
            return 0;
        case WM_MBUTTONUP:
            if (d) {
                int si = StatAt(*d, (float)GET_X_LPARAM(lp), (float)GET_Y_LPARAM(lp));
                if (si >= 0) {
                    if (IsTrayKind(d->stat[si].id)) TrayDockClick(*d, si, (float)GET_X_LPARAM(lp), 2);
                    if (d->stat[si].id == StVolume) {
                        ToggleMute();
                        StatusChanged();
                    }
                    if (d->stat[si].id == StPlayer) MediaCmd(1);
                    return 0;
                }
                int e = ElemAt(*d, (float)GET_X_LPARAM(lp), (float)GET_Y_LPARAM(lp));
                if (e >= 0 && !g_apps[d->elems[e].app].start) Launch(g_apps[d->elems[e].app]);
            }
            return 0;
        case WM_RBUTTONUP:
            if (d) {
                float x = (float)GET_X_LPARAM(lp), y = (float)GET_Y_LPARAM(lp);
                int si = StatAt(*d, x, y);
                if (si >= 0 && IsTrayKind(d->stat[si].id)) {
                    TrayDockClick(*d, si, x, 1);
                    return 0;
                }
                if (si >= 0 && d->stat[si].id == StKeyboard) {
                    StatClick(*d, StKeyboard, true, false);
                    return 0;
                }
                CloseFlyout();
                int e = ElemAt(*d, x, y);
                POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                ClientToScreen(hwnd, &pt);
                pt.y = d->rcMon.bottom - d->ch + (LONG)d->pill.top - 6;
                HidePreview();
                if (e >= 0) ShowMenu(*d, e, pt);
                else if (x >= d->pill.left && x <= d->pill.right && y >= d->pill.top && y <= d->pill.bottom)
                    ShowDockMenu(*d, pt);
            }
            return 0;
        case WM_LBUTTONDOWN:
            if (d) {
                {
                    float mx = (float)GET_X_LPARAM(lp), my = (float)GET_Y_LPARAM(lp);
                    int si = StatAt(*d, mx, my);
                    int ti = si >= 0 ? TrayIconAt(*d, si, mx) : -1;
                    if (ti >= 0 && ti < (int)g_tray.size()) {
                        g_trayDockDrag = {};
                        g_trayDockDrag.down = true;
                        g_trayDockDrag.dock = d;
                        g_trayDockDrag.key = g_tray[ti].key;
                        g_trayDockDrag.sx = (int)mx;
                        g_trayDockDrag.sy = (int)my;
                        SetCapture(hwnd);
                        return 0;
                    }
                }
                int e = ElemAt(*d, (float)GET_X_LPARAM(lp), (float)GET_Y_LPARAM(lp));
                if (e >= 0 && !g_apps[d->elems[e].app].start) {
                    g_drag = {};
                    g_drag.down = true;
                    g_drag.dock = d;
                    g_drag.key = g_apps[d->elems[e].app].key;
                    g_drag.sx = GET_X_LPARAM(lp);
                    g_drag.sy = GET_Y_LPARAM(lp);
                    SetCapture(hwnd);
                }
            }
            return 0;
        case WM_MOUSEWHEEL:
            if (d) {
                // Over the volume item: change the volume.
                POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                ScreenToClient(hwnd, &pt);
                int si = StatAt(*d, (float)pt.x, (float)pt.y);
                if (si >= 0 && d->stat[si].id == StVolume) {
                    SetVolume(g_stat.vol + GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * 0.02f);
                    StatusChanged();
                }
                if (si >= 0 && d->stat[si].id == StPlayer) {
                    if (AppVol* a = MediaVol(MediaNow().aumid, true)) SetAppVolume(*a, a->level + GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * 0.04f);
                }
                if (si >= 0 && (d->stat[si].id == StTrayStrip || d->stat[si].id == StTrayPrev || d->stat[si].id == StTrayNext)) {
                    TrayScroll(GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1);
                    d->trayHot = -1;
                }
            }
            return 0;
        case WM_APP_APPBAR:
            if (d && wp == ABN_POSCHANGED) SetAppBarPos(*d);
            return 0;
        case WM_DPICHANGED:
            SetTimer(g_msgWnd, TIMER_DOCKS, 300, nullptr);  // new scale: the docks are rebuilt for it
            return 0;
        case WM_CAPTURECHANGED:
            if (g_drag.active && (HWND)lp != hwnd) {
                g_drag = {};
                if (d) Invalidate(*d);
            }
            return 0;
        case WM_TIMER:
            if (d && wp == TIMER_DOCK_BANDS) {
                KillTimer(hwnd, TIMER_DOCK_BANDS);
                d->bandsPending = false;
                PlaceDock(*d);
                return 0;
            }
            if (d && wp == TIMER_TRAY_DRAGSCROLL) {
                // Over an arrow, or near the strip's ends: one more icon that way.
                auto& g = g_trayDockDrag;
                if (!g.active || g.dock != d) {
                    KillTimer(hwnd, TIMER_TRAY_DRAGSCROLL);
                    return 0;
                }
                for (auto& it : d->stat) {
                    if (g.mx < it.x || g.mx >= it.x + it.w) continue;
                    float edge = 12 * d->scale;
                    if (it.id == StTrayPrev || (it.id == StTrayStrip && g.mx < it.x + edge)) TrayScroll(-1);
                    else if (it.id == StTrayNext || (it.id == StTrayStrip && g.mx > it.x + it.w - edge)) TrayScroll(1);
                }
                Invalidate(*d);
                return 0;
            }
            if (d && wp == TIMER_PREVIEW) {
                KillTimer(hwnd, TIMER_PREVIEW);
                if (d->mouseX >= 0 && d->hoverElem >= 0 && !d->menuOpen) ShowPreview(*d, d->hoverElem);
                return 0;
            }
            if (d && wp == TIMER_ANIM) {
                ComputeTargets(*d);
                bool moving = StepAnim(*d);
                RenderDock(*d);
                if (!moving) {
                    KillTimer(hwnd, TIMER_ANIM);
                    d->animating = false;
                }
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void DestroyDock(Dock* d) {
    g_dockArt.erase(d);
    RemoveAppBar(*d);
    if (d->content) RevokeDragDrop(d->content);
    if (d->content) DestroyWindow(d->content);
    for (HWND b : d->bands) DestroyWindow(b);
    d->rt.Reset();
    d->dc.Reset();
    d->bmps.clear();
    if (d->memDC) {
        SelectObject(d->memDC, d->oldBmp);
        DeleteDC(d->memDC);
    }
    if (d->dib) DeleteObject(d->dib);
    delete d;
}

void DestroyDocks() {
    CloseFlyout();
    HidePreview();
    CloseStart();
    for (auto* d : g_docks) DestroyDock(d);
    g_docks.clear();
}

void CreateDocks() {
    if (g_dockMenuDepth) {
        g_docksPending = true;  // rebuilt when the menu returns (see TrackDockMenu)
        return;
    }
    DestroyDocks();
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR mon, HDC, LPRECT, LPARAM) WINAPI -> BOOL {
        MONITORINFO mi = {sizeof(mi)};
        if (!GetMonitorInfoW(mon, &mi)) return TRUE;
        const wchar_t* group = MonitorGroup(mon, mi.rcMonitor);
        DockCfg cfg = LoadDockCfg(group, MonitorIdentity(mon).id);
        Wh_Log(L"monitor (%d,%d,%d,%d) -> %s enabled=%d fixed=%d autoHide=%d", mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right, mi.rcMonitor.bottom, group, cfg.enabled, cfg.fixed, cfg.autoHide);
        if (!cfg.enabled) return TRUE;
        Dock* d = new Dock();
        d->cfg = cfg;
        d->mon = mon;
        d->rcMon = mi.rcMonitor;
        UINT dx = 96, dy = 96;
        GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy);
        d->scale = dx / 96.0f;
        d->cw = mi.rcMonitor.right - mi.rcMonitor.left;
        d->ch = (int)std::ceil(PillH(*d) + S(*d) * (Zoom() - 1) + 48 * d->scale + BottomGap(*d));

        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = d->cw;
        bi.bmiHeader.biHeight = -d->ch;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        d->memDC = CreateCompatibleDC(nullptr);
        d->dib = CreateDIBSection(d->memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        d->oldBmp = SelectObject(d->memDC, d->dib);

        g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 12.5f * d->scale, g_locale, &d->font);
        g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 12.5f * d->scale, g_locale, &d->fontBold);
        g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 9.5f * d->scale, g_locale, &d->fontTiny);

        for (int b = 0; b < 1 + 2 * kBandSteps; b++) {
            d->bands.push_back(CreateBand());
            d->bandRc.push_back(RECT{});
        }

        d->content = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
                                     L"NimbusMenu_Content", L"Nimbus Menu", WS_POPUP, d->rcMon.left,
                                     d->rcMon.bottom - d->ch, d->cw, d->ch, d->bands[0], nullptr, g_module, nullptr);
        Wh_Log(L"dock mon=(%d,%d,%d,%d) scale=%.2f cw=%d ch=%d content=%p style=%s", d->rcMon.left, d->rcMon.top,
            d->rcMon.right, d->rcMon.bottom, d->scale, d->cw, d->ch, d->content, g_set.style.c_str());
        SetWindowLongPtrW(d->content, GWLP_USERDATA, (LONG_PTR)d);
        RegisterShelfDrop(d->content, d);  // files dragged over the dock open the shelf
        BuildElems(*d, false);
        d->wantShown = !d->cfg.autoHide;
        d->shown = d->wantShown ? 1 : 0;
        g_docks.push_back(d);
        if (d->cfg.fixed && !d->cfg.autoHide) RegisterAppBar(*d);
        ComputeTargets(*d);
        for (auto& el : d->elems) el.m = 1;
        RenderDock(*d);
        return TRUE;
    }, 0);
}

static bool IsFullscreen(Dock& d) {
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    wchar_t cls[64];
    GetClassNameW(fg, cls, ARRAYSIZE(cls));
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") || !wcscmp(cls, L"NimbusMenu_Content")) return false;
    if (MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST) != d.mon) return false;
    if (IsZoomed(fg)) return false;
    RECT r;
    if (!GetWindowRect(fg, &r)) return false;
    return r.left <= d.rcMon.left && r.top <= d.rcMon.top && r.right >= d.rcMon.right && r.bottom >= d.rcMon.bottom;
}

static void Poll() {
    POINT pt;
    GetCursorPos(&pt);
    DWORD now = GetTickCount();
    bool anyFs = false;
    for (auto* dp : g_docks) {
        Dock& d = *dp;
        bool fs = IsFullscreen(d);
        anyFs = anyFs || fs;
        bool want = d.wantShown;
        if (fs) {
            want = false;
        } else if (!d.cfg.autoHide) {
            want = true;
        } else {
            bool onMon = pt.x >= d.rcMon.left && pt.x < d.rcMon.right && pt.y >= d.rcMon.top && pt.y < d.rcMon.bottom;
            bool atEdge = onMon && pt.y >= d.rcMon.bottom - 2;
            float top = d.rcMon.bottom - d.ch + d.pill.top - S(d) * (Zoom() - 1);
            bool overDock = d.visible && onMon && pt.y >= top && pt.x >= d.rcMon.left + d.pill.left - 8 &&
                            pt.x <= d.rcMon.left + d.pill.right + 8;
            if (g_prev.visible && g_prev.dock == &d && CursorInPreview(pt)) overDock = true;
            if (StartOpenOn(d) || FlyoutOpenOn(d)) overDock = true;
            if (atEdge || overDock || d.menuOpen) {
                want = true;
                d.outsideSince = now;
            } else if (d.wantShown && now - d.outsideSince >= (DWORD)g_set.hideDelay) {
                want = false;
            }
        }
        d.fullscreen = fs;
        if (want != d.wantShown) {
            d.wantShown = want;
            if (want) d.outsideSince = now;
            Animate(d);
        }
    }
    UpdateMoveHooks(anyFs);
}

void ScheduleRebuild() { SetTimer(g_msgWnd, TIMER_REBUILD, 60, nullptr); }

// What a dock shows: its icons, which are open (and how many windows) and which flash.
static std::wstring DockSig(const Dock& d) {
    std::wstring sig;
    for (auto& el : d.elems) {
        sig += el.key;
        if (el.app >= 0 && el.app < (int)g_apps.size()) {
            const App& a = g_apps[el.app];
            sig += L'|' + std::to_wstring(DockWins(d, a).size()) + (a.flash ? L"f" : L"") + (a.icon ? L"i" : L"");
            sig += a.label;
        }
        sig += L';';
    }
    return sig;
}

// A monitor's scale changed without a display change (Settings > Display > Scale): rebuild the docks.
static bool DockScalesChanged() {
    for (auto* d : g_docks) {
        UINT dx = 96, dy = 96;
        if (SUCCEEDED(GetDpiForMonitor(d->mon, MDT_EFFECTIVE_DPI, &dx, &dy)) && std::fabs(dx / 96.0f - d->scale) > 0.001f)
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Nimbus' settings window: its own page instead of Windhawk's generic one. Each change is saved at
// once (CfgSet) and the dock follows it a moment later.
// ---------------------------------------------------------------------------

constexpr UINT_PTR TIMER_APPLY = 4;
constexpr UINT WM_APP_OPEN_SETTINGS = WM_APP + 9;  // wParam = page
constexpr UINT_PTR TIMER_SW_SCROLL = 30;
constexpr wchar_t kVersion[] = L"2.0.0";
constexpr wchar_t kHelpUrl[] = L"https://github.com/igoredm/nimbus-menu";

static const wchar_t* SettingsTitle() { return L2(L"Ajustes do Nimbus", L"Nimbus settings"); }

// The values the Windhawk settings above start with, for "Back to defaults".
static const wchar_t* CfgDefault(const std::wstring& name) {
    static const std::pair<const wchar_t*, const wchar_t*> defs[] = {
        {L"style", L"nevoa"}, {L"format", L"dock"}, {L"animation", L"zoom"}, {L"showStart", L"1"},
        {L"startMenu", L"nevoa"}, {L"winKeyMode", L"nevoa"}, {L"startHotkey", L"none"}, {L"endTask", L"windows"},
        {L"searchLayout", L"larga"}, {L"fileSearch", L"everything"}, {L"iconSize", L"60"}, {L"iconGap", L"18"},
        {L"magnification", L"150"}, {L"autoHide", L"1"}, {L"hideDelay", L"400"}, {L"bottomGap", L"12"},
        {L"tintColor", L""}, {L"tintOpacity", L"62"}, {L"hideWindowsTaskbar", L"1"}, {L"enabled", L"1"},
        {L"showPinned", L"1"}, {L"windows", L"todas"}, {L"statusPos", L"direita"},
    };
    std::wstring field = name.substr(name.find(L'.') + 1);  // monitor groups: "monLeft.format" -> "format"
    bool mon = field.size() != name.size();
    if (mon && (field == L"format" || field == L"autoHide")) return L"geral";
    for (auto& d : defs) if (field == d.first) return d.second;
    for (auto& d : kOwnKeys) if (field == d.first) return d.second;
    return L"";
}

static void ApplySoon() {
    if (g_msgWnd) SetTimer(g_msgWnd, TIMER_APPLY, 120, nullptr);
}

static void CfgSet(const std::wstring& name, const std::wstring& v) {
    g_cfg[name] = v;
    WriteVal(UserPrefix() + L"cfg." + name, v);
    ApplySoon();
}

static void SwInvalidate();
static void CfgSetQuiet(const std::wstring& name, const std::wstring& v) {
    g_cfg[name] = v;
    WriteVal(UserPrefix() + L"cfg." + name, v);
    SwInvalidate();
}

static void MonSet(const std::wstring& id, const wchar_t* field, const std::wstring& v) {
    WriteVal(UserPrefix() + L"mon." + id + L"." + field, v);
    auto ids = MonIdsWithChoices();
    if (std::find(ids.begin(), ids.end(), id) == ids.end()) {
        ids.push_back(id);
        std::wstring list;
        for (auto& x : ids) list += (list.empty() ? L"" : L",") + x;
        WriteVal(UserPrefix() + L"monIds", list);
    }
    ApplySoon();
}

static void ResetCfg() {
    for (auto& k : kCfgKeys) CfgSet(k.name, CfgDefault(k.name));
    for (auto& k : kOwnKeys) CfgSet(k.first, k.second);
    for (auto* g : kMonGroups)
        for (auto& f : kMonFields) {
            std::wstring n = std::wstring(g) + L"." + f.name;
            CfgSet(n, CfgDefault(n));
        }
    for (auto& id : MonIdsWithChoices())
        for (auto& f : kMonFields) Wh_DeleteValue((UserPrefix() + L"mon." + id + L"." + f.name).c_str());
    Wh_DeleteValue((UserPrefix() + L"monIds").c_str());
}

// ---- Drawing helpers -------------------------------------------------------

// A small SVG path reader (M L H V A Z, absolute and relative), for the line icons of the design.
static ComPtr<ID2D1PathGeometry> SvgPath(const wchar_t* d, bool filled = false) {
    ComPtr<ID2D1PathGeometry> g;
    ComPtr<ID2D1GeometrySink> s;
    if (FAILED(g_d2d->CreatePathGeometry(&g)) || FAILED(g->Open(&s))) return {};
    const wchar_t* p = d;
    float cx = 0, cy = 0, sx = 0, sy = 0;
    bool open = false;
    wchar_t cmd = 0;
    auto skip = [&] { while (*p == L' ' || *p == L',') p++; };
    auto isNum = [&] { skip(); return *p == L'-' || *p == L'+' || *p == L'.' || (*p >= L'0' && *p <= L'9'); };
    auto num = [&] {
        skip();
        wchar_t* e = nullptr;
        float v = wcstof(p, &e);
        if (e == p) e = (wchar_t*)p + 1;
        p = e;
        return v;
    };
    auto begin = [&] {
        if (!open) {
            s->BeginFigure(D2D1::Point2F(cx, cy), filled ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW);
            open = true;
        }
    };
    for (;;) {
        skip();
        if (!*p) break;
        if (!isNum()) cmd = *p++;
        bool rel = cmd >= L'a';
        switch (cmd) {
            case L'M': case L'm': {
                float x = num(), y = num();
                if (rel) { x += cx; y += cy; }
                if (open) s->EndFigure(D2D1_FIGURE_END_OPEN);
                open = false;
                cx = sx = x;
                cy = sy = y;
                begin();
                cmd = rel ? L'l' : L'L';
                break;
            }
            case L'L': case L'l': {
                float x = num(), y = num();
                if (rel) { x += cx; y += cy; }
                begin();
                s->AddLine(D2D1::Point2F(x, y));
                cx = x;
                cy = y;
                break;
            }
            case L'H': case L'h': {
                float x = num();
                if (rel) x += cx;
                begin();
                s->AddLine(D2D1::Point2F(x, cy));
                cx = x;
                break;
            }
            case L'V': case L'v': {
                float y = num();
                if (rel) y += cy;
                begin();
                s->AddLine(D2D1::Point2F(cx, y));
                cy = y;
                break;
            }
            case L'A': case L'a': {
                float rx = num(), ry = num(), rot = num(), large = num(), sweep = num(), x = num(), y = num();
                if (rel) { x += cx; y += cy; }
                begin();
                s->AddArc(D2D1::ArcSegment(D2D1::Point2F(x, y), D2D1::SizeF(rx, ry), rot,
                                           sweep != 0 ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                                           large != 0 ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
                cx = x;
                cy = y;
                break;
            }
            case L'Z': case L'z':
                if (open) s->EndFigure(D2D1_FIGURE_END_CLOSED);
                open = false;
                cx = sx;
                cy = sy;
                break;
            default:
                p = L"";  // not used by these icons
                break;
        }
    }
    if (open) s->EndFigure(D2D1_FIGURE_END_OPEN);
    s->Close();
    return g;
}

static const wchar_t kGlyphDock[] = L"M3 17h18v3H3z M6 12h3v3H6z M10.5 12h3v3h-3z M15 12h3v3h-3z";

// The Nimbus icon (dock glyph on a dark rounded tile), for the settings window and its dock icon.
// Nimbus' logo: a frosted cloud over a little dock, on a smoked-glass tile (assets/logo.png is the same
// design). Drawn as vectors into a square box on any target, so it stays sharp at every size.
static void DrawNimbusLogo(ID2D1RenderTarget* rt, D2D1_RECT_F box) {
    float u = (box.right - box.left) / 24.0f;
    D2D1_MATRIX_3X2_F old;
    rt->GetTransform(&old);
    rt->SetTransform(D2D1::Matrix3x2F::Scale(u, u) * D2D1::Matrix3x2F::Translation(box.left, box.top) * old);
    auto vgrad = [&](D2D1_COLOR_F a, D2D1_COLOR_F b, float y0, float y1) {
        D2D1_GRADIENT_STOP st[] = {{0, a}, {1, b}};
        ComPtr<ID2D1GradientStopCollection> sc;
        ComPtr<ID2D1LinearGradientBrush> g;
        rt->CreateGradientStopCollection(st, 2, &sc);
        if (sc) rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, y0), D2D1::Point2F(0, y1)), sc.Get(), &g);
        return g;
    };
    ComPtr<ID2D1SolidColorBrush> b;
    rt->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFF, 1), &b);
    // Tile, with light on its top edge.
    D2D1_ROUNDED_RECT tile = D2D1::RoundedRect(D2D1::RectF(1, 1, 23, 23), 5.5f, 5.5f);
    if (auto g = vgrad(D2D1::ColorF(0x3A3A41), D2D1::ColorF(0x101013), 1, 23)) rt->FillRoundedRectangle(tile, g.Get());
    if (auto g = vgrad(D2D1::ColorF(0xFFFFFF, 0.6f), D2D1::ColorF(0xFFFFFF, 0.06f), 1, 12))
        rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(1.25f, 1.25f, 22.75f, 22.75f), 5.25f, 5.25f), g.Get(),
                                 std::max(0.4f, 1.0f / u));
    if (b) {
        // A soft glow, then the cloud.
        b->SetColor(D2D1::ColorF(0xFFFFFF, 0.07f));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(12, 11.4f), 8, 5.6f), b.Get());
    }
    if (auto g = vgrad(D2D1::ColorF(0xF7F7F9), D2D1::ColorF(0xC4C5CB), 5.6f, 14.3f)) {
        rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(6.2f, 10.3f, 17.8f, 14.3f), 2, 2), g.Get());
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(9.3f, 11.2f), 2.9f, 2.9f), g.Get());
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(12.5f, 9.3f), 3.7f, 3.7f), g.Get());
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(15.5f, 11.5f), 2.5f, 2.5f), g.Get());
    }
    if (b) {
        // The dock under it, with four lit dots.
        D2D1_ROUNDED_RECT pill = D2D1::RoundedRect(D2D1::RectF(8.2f, 16.3f, 15.8f, 18.7f), 1.2f, 1.2f);
        b->SetColor(D2D1::ColorF(0xFFFFFF, 0.13f));
        rt->FillRoundedRectangle(pill, b.Get());
        b->SetColor(D2D1::ColorF(0xFFFFFF, 0.22f));
        rt->DrawRoundedRectangle(pill, b.Get(), std::max(0.25f, 0.6f / u));
        b->SetColor(D2D1::ColorF(0xFFFFFF, 0.95f));
        for (int i = 0; i < 4; i++) rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(10.0f + i * 1.333f, 17.5f), 0.4f, 0.4f), b.Get());
    }
    rt->SetTransform(old);
}

static HBITMAP DrawNimbusIcon(int px) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = px;
    bi.bmiHeader.biHeight = -px;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp) {
        DeleteDC(dc);
        return nullptr;
    }
    HGDIOBJ old = SelectObject(dc, bmp);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    ComPtr<ID2D1DCRenderTarget> rt;
    RECT rc = {0, 0, px, px};
    if (SUCCEEDED(g_d2d->CreateDCRenderTarget(&props, &rt)) && SUCCEEDED(rt->BindDC(dc, &rc))) {
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0));
        DrawNimbusLogo(rt.Get(), D2D1::RectF(0, 0, (float)px, (float)px));
        rt->EndDraw();
    }
    SelectObject(dc, old);
    DeleteDC(dc);
    return bmp;
}

static HICON IconFromBitmap(HBITMAP color, int px) {
    if (!color) return nullptr;
    HBITMAP mask = CreateBitmap(px, px, 1, 1, nullptr);
    ICONINFO ii = {TRUE, 0, 0, mask, color};
    HICON h = CreateIconIndirect(&ii);
    DeleteObject(mask);
    return h;
}

// ---- The window ------------------------------------------------------------

enum SwPage { PgGeral, PgDock, PgIniciar, PgStatus, PgMonitores, PgSobre, PgComecar, PgCount };

struct SwHit {
    D2D1_RECT_F r{};
    std::function<void()> click;
    bool slider = false;
    std::wstring key;  // slider: value key (see SwVal)
    float tx = 0, tw = 0;
    int min = 0, max = 0, step = 1;
    std::function<void(int)> commit;
};

struct SwMon {
    HMONITOR mon;
    MonIdentity who;
    const wchar_t* group;
    bool primary;
    int w, h;
};

struct SettingsWin {
    HWND hwnd = nullptr;
    ComPtr<ID2D1HwndRenderTarget> rt;
    ComPtr<ID2D1DeviceContext> dc;
    ComPtr<ID2D1SolidColorBrush> br;
    ComPtr<ID2D1StrokeStyle> round;
    ComPtr<IDWriteTextFormat> fH1, fSub, fLbl, fDesc, fSec, fSeg, fNav, fSmall, fBrand, fGlyph, fTitle;
    std::unordered_map<std::wstring, ComPtr<ID2D1PathGeometry>> paths;
    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> bmps;
    float scale = 1;
    int page = PgGeral;
    float scroll = 0, scrollTo = 0, contentH = 0, viewH = 0;
    D2D1_POINT_2F mouse{-1, -1};
    bool mouseIn = false;
    std::vector<SwHit> hits;
    int pressed = -1;
    bool dragging = false;
    SwHit drag;
    std::unordered_map<std::wstring, int> pending;  // slider values while dragging
    bool confirmReset = false;
    bool confirmBanners = false;
    int statusTab = 0;  // status page: items, tray, shelf, notifications
    int wizStep = 0;            // the guided setup: the question shown (past the last: done)
    std::vector<int> wizAns;    // what was picked in each (-1: skipped)
    HICON iconBig = nullptr, iconSmall = nullptr;
    D2D1_RECT_F preview{};
    bool inPreview = false;
    std::vector<SwMon> mons;
} g_sw;

static ID2D1SolidColorBrush* Br(UINT32 rgb, float a = 1) {
    g_sw.br->SetColor(D2D1::ColorF(rgb, a));
    return g_sw.br.Get();
}
static ID2D1SolidColorBrush* Br(const D2D1_COLOR_F& c) {
    g_sw.br->SetColor(c);
    return g_sw.br.Get();
}

static bool InRect(const D2D1_RECT_F& r, D2D1_POINT_2F p) { return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom; }
static bool Hot(const D2D1_RECT_F& r) { return g_sw.mouseIn && !g_sw.dragging && InRect(r, g_sw.mouse); }

static ComPtr<IDWriteTextLayout> Lay(const std::wstring& s, IDWriteTextFormat* f, float w, float h = 4000) {
    ComPtr<IDWriteTextLayout> l;
    g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f, std::max(1.0f, w), h, &l);
    return l;
}
static D2D1_SIZE_F Measure(const std::wstring& s, IDWriteTextFormat* f, float w = 4000) {
    DWRITE_TEXT_METRICS m = {};
    if (auto l = Lay(s, f, w)) l->GetMetrics(&m);
    return D2D1::SizeF(m.widthIncludingTrailingWhitespace, m.height);
}
static float Txt(const std::wstring& s, IDWriteTextFormat* f, float x, float y, float w, UINT32 rgb, float a = 1) {
    auto l = Lay(s, f, w);
    if (!l) return 0;
    g_sw.rt->DrawTextLayout(D2D1::Point2F(x, y), l.Get(), Br(rgb, a), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    DWRITE_TEXT_METRICS m = {};
    l->GetMetrics(&m);
    return m.height;
}
// Text centered in a box.
static void TxtC(const std::wstring& s, IDWriteTextFormat* f, D2D1_RECT_F r, UINT32 rgb, float a = 1) {
    auto l = Lay(s, f, r.right - r.left);
    if (!l) return;
    l->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    DWRITE_TEXT_METRICS m = {};
    l->GetMetrics(&m);
    g_sw.rt->DrawTextLayout(D2D1::Point2F(r.left, (r.top + r.bottom - m.height) / 2), l.Get(), Br(rgb, a));
}

// A line icon from the design (24x24 box, 1.7 stroke).
static void Icon(const wchar_t* d, float x, float y, float size, UINT32 rgb, float a = 1, float stroke = 1.7f) {
    auto& g = g_sw.paths[d];
    if (!g) g = SvgPath(d);
    if (!g) return;
    D2D1_MATRIX_3X2_F old;
    g_sw.rt->GetTransform(&old);
    float k = size / 24;
    g_sw.rt->SetTransform(D2D1::Matrix3x2F::Scale(k, k) * D2D1::Matrix3x2F::Translation(x, y) * old);
    g_sw.rt->DrawGeometry(g.Get(), Br(rgb, a), stroke, g_sw.round.Get());
    g_sw.rt->SetTransform(old);
}

static void Glyph(wchar_t ch, D2D1_RECT_F r, UINT32 rgb, float a = 1) {
    wchar_t s[2] = {ch, 0};
    g_sw.fGlyph->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    g_sw.fGlyph->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    g_sw.rt->DrawTextW(s, 1, g_sw.fGlyph.Get(), r, Br(rgb, a));
}

static SwHit& AddHit(D2D1_RECT_F r, std::function<void()> click) {
    g_sw.hits.push_back({});
    g_sw.hits.back().r = r;
    g_sw.hits.back().click = std::move(click);
    return g_sw.hits.back();
}

static void SwInvalidate() {
    if (g_sw.hwnd) InvalidateRect(g_sw.hwnd, nullptr, FALSE);
}

// A setting's number, or the slider's while it's dragged.
static int SwVal(const std::wstring& key) {
    auto it = g_sw.pending.find(key);
    return it != g_sw.pending.end() ? it->second : CfgInt(key.c_str());
}

static const UINT32 kInk = 0xECECEE, kSub = 0xA0A0A8, kIdle = 0xB4B4BB, kMuted = 0x8A8A92, kBg = 0x0E0E10,
                    kSide = 0x121215, kAccent = 0xECECEE;

// ---- Rows ------------------------------------------------------------------

enum SwCtl { CtlNone, CtlToggle, CtlSeg, CtlSlider, CtlButtons, CtlSwatches };

struct SwRow {
    std::wstring label, desc;
    SwCtl ctl = CtlNone;
    std::function<bool()> on;
    std::function<void()> flip;
    std::vector<std::pair<std::wstring, std::wstring>> opts;  // value, label
    std::function<std::wstring()> cur;
    std::function<void(const std::wstring&)> pick;
    std::wstring key;
    int min = 0, max = 0, step = 1;
    std::function<std::wstring(int)> fmt;
    std::vector<std::pair<std::wstring, std::function<void()>>> buttons;
    bool multi = false;  // a segmented row where several can be on (cur: values with commas)
};

static SwRow RToggle(const wchar_t* label, const wchar_t* desc, const wchar_t* key) {
    SwRow r;
    r.label = label;
    r.desc = desc ? desc : L"";
    r.ctl = CtlToggle;
    std::wstring k = key;
    r.on = [k] { return CfgInt(k.c_str()) != 0; };
    r.flip = [k] { CfgSet(k, CfgInt(k.c_str()) ? L"0" : L"1"); };
    return r;
}

static SwRow RSeg(const wchar_t* label, const wchar_t* desc, const wchar_t* key,
                  std::vector<std::pair<std::wstring, std::wstring>> opts) {
    SwRow r;
    r.label = label;
    r.desc = desc ? desc : L"";
    r.ctl = CtlSeg;
    r.opts = std::move(opts);
    std::wstring k = key, def = r.opts.empty() ? L"" : r.opts[0].first;
    r.cur = [k, def] { return CfgStr(k.c_str(), def.c_str()); };
    r.pick = [k](const std::wstring& v) { CfgSet(k, v); };
    return r;
}

// Dock, Start or off, for a status item or the tray.
static SwRow RPlace(const wchar_t* label, const wchar_t* desc, const wchar_t* key) {
    SwRow r = RSeg(label, desc, key,
                   {{L"dock", L"Dock"}, {L"central", L2(L"Central", L"Center")}, {L"iniciar", L2(L"Iniciar", L"Start")}, {L"off", L2(L"Desligado", L"Off")}});
    std::wstring k = key;
    r.multi = true;
    r.cur = [k] {
        if (k == L"trayPlace" && CfgStr(L"trayStyle") == L"off") return std::wstring(L"off");
        bool extra = k == L"dndPlace" || k == L"btPlace" || k == L"nightPlace" || k == L"shortPlace" || k == L"lightPlace" || k == L"powerPlace" ||
                     k == L"playerPlace" || k == L"devPlace";
        bool dockFirst = k == L"playerPlace" || k == L"devPlace";
        return std::wstring(
            PlaceName(PlaceOf(CfgStr(k.c_str(), dockFirst ? L"dock" : extra ? L"central" : L"1"), k == L"trayPlace" || extra)));
    };
    r.pick = [k](const std::wstring& v) {
        // A place switches on or off; "Desligado" clears them all.
        bool extra = k == L"dndPlace" || k == L"btPlace" || k == L"nightPlace" || k == L"shortPlace" || k == L"lightPlace" || k == L"powerPlace" ||
                     k == L"playerPlace" || k == L"devPlace";
        bool dockFirst = k == L"playerPlace" || k == L"devPlace";
        int m = PlaceOf(CfgStr(k.c_str(), dockFirst ? L"dock" : extra ? L"central" : L"1"), k == L"trayPlace" || extra);
        if (k == L"trayPlace" && CfgStr(L"trayStyle") == L"off") m = 0;
        int b = PlaceOf(v, true);
        m = v == L"off" ? 0 : (m ^ b);
        std::wstring nv = PlaceName(m);
        CfgSet(k, nv);
        if (k == L"trayPlace" && nv != L"off" && CfgStr(L"trayStyle") == L"off") CfgSet(L"trayStyle", L"botao");
    };
    return r;
}

static SwRow RSlider(const wchar_t* label, const wchar_t* desc, const wchar_t* key, int min, int max, int step,
                     std::function<std::wstring(int)> fmt) {
    SwRow r;
    r.label = label;
    r.desc = desc ? desc : L"";
    r.ctl = CtlSlider;
    r.key = key;
    r.min = min;
    r.max = max;
    r.step = step;
    r.fmt = std::move(fmt);
    return r;
}

static SwRow RButtons(const wchar_t* label, const wchar_t* desc,
                      std::vector<std::pair<std::wstring, std::function<void()>>> buttons) {
    SwRow r;
    r.label = label;
    r.desc = desc ? desc : L"";
    r.ctl = CtlButtons;
    r.buttons = std::move(buttons);
    return r;
}

// A sound to pick: ‹ and › go through them (each one plays), "Ouvir" plays it again.
static SwRow SoundRow(const wchar_t* label, const wchar_t* key, const wchar_t* note) {
    std::wstring k = key;
    int v = SoundValue(CfgStr(key));
    std::wstring desc = SoundName(v) + L" · " + note;
    auto step = [k](int d) {
        int n = kSoundCount + 2;
        int nv = ((SoundValue(CfgStr(k.c_str())) + d) % n + n) % n;
        CfgSet(k, SoundSetting(nv));
        PlayNimbusSound(nv);
    };
    return RButtons(label, desc.c_str(),
                    {{L"‹", [step] { step(-1); }}, {L"›", [step] { step(1); }},
                     {L2(L"Ouvir", L"Listen"), [k] { PlayNimbusSound(SoundValue(CfgStr(k.c_str()))); }}});
}

static std::wstring Px(int v) { return std::to_wstring(v) + L" px"; }
static std::wstring Pct(int v) { return std::to_wstring(v) + L"%"; }
static std::wstring Secs(int ms) {
    wchar_t b[16];
    swprintf_s(b, L"%.1f s", ms / 1000.0);
    if (IsPortugueseUi()) for (wchar_t* c = b; *c; c++) if (*c == L'.') *c = L',';
    return b;
}

static const float kSegH = 30, kSegPad = 3;

static float SegOptW(const std::wstring& s) { return Measure(s, g_sw.fSeg.Get()).width + 24; }

static const wchar_t* const kSwatches[] = {L"050506", L"0B1220", L"1A0A12", L"07140F", L"140A22", L"1E1408"};

static float ControlWidth(const SwRow& r) {
    switch (r.ctl) {
        case CtlToggle: return 40;
        case CtlSeg: {
            float w = kSegPad * 2;
            for (auto& o : r.opts) w += SegOptW(o.second) + 2;
            return w - 2;
        }
        case CtlSlider: return 200 + 12 + 60;
        case CtlButtons: {
            float w = 0;
            for (auto& b : r.buttons) w += Measure(b.first, g_sw.fSeg.Get()).width + 28 + 8;
            return w - 8;
        }
        case CtlSwatches: return (2 + (float)ARRAYSIZE(kSwatches)) * 30 - 8;
        default: return 0;
    }
}

static void DrawToggle(float x, float cy, bool on, std::function<void()> flip) {
    D2D1_RECT_F r = D2D1::RectF(x, cy - 11, x + 40, cy + 11);
    bool hot = Hot(r);
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 11, 11),
                                  on ? Br(kAccent, hot ? 0.88f : 1) : Br(0xFFFFFF, hot ? 0.22f : 0.16f));
    float kx = on ? x + 29 : x + 11;
    g_sw.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(kx, cy), 8, 8), on ? Br(kBg) : Br(0xBDBDC4));
    AddHit(r, std::move(flip));
}

static void DrawSeg(float x, float cy, const SwRow& row) {
    float w = ControlWidth(row);
    D2D1_RECT_F wrap = D2D1::RectF(x, cy - kSegH / 2 - kSegPad, x + w, cy + kSegH / 2 + kSegPad);
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(wrap, 9, 9), Br(0xFFFFFF, 0.05f));
    std::wstring cur = row.cur();
    float ox = x + kSegPad;
    for (auto& o : row.opts) {
        float ow = SegOptW(o.second);
        D2D1_RECT_F r = D2D1::RectF(ox, cy - kSegH / 2, ox + ow, cy + kSegH / 2);
        bool on = o.first == cur;
        if (row.multi) on = (L"," + cur + L",").find(L"," + o.first + L",") != std::wstring::npos;
        if (on) g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 7, 7), Br(0xFFFFFF, 0.14f));
        else if (Hot(r)) g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 7, 7), Br(0xFFFFFF, 0.06f));
        TxtC(o.second, g_sw.fSeg.Get(), r, on ? 0xFFFFFF : kIdle);
        auto pick = row.pick;
        std::wstring v = o.first;
        if (!on || row.multi) AddHit(r, [pick, v] { pick(v); });
        ox += ow + 2;
    }
}

static void DrawSlider(float x, float cy, const SwRow& row) {
    int v = SwVal(row.key);
    float t = row.max > row.min ? (float)(Clamp(v, row.min, row.max) - row.min) / (row.max - row.min) : 0;
    float tw = 200, tx = x;
    D2D1_RECT_F track = D2D1::RectF(tx, cy - 2, tx + tw, cy + 2);
    bool hot = Hot(D2D1::RectF(tx - 8, cy - 12, tx + tw + 8, cy + 12)) || (g_sw.dragging && g_sw.drag.key == row.key);
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(track, 2, 2), Br(0xFFFFFF, 0.16f));
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(tx, cy - 2, tx + tw * t, cy + 2), 2, 2), Br(kAccent));
    D2D1_POINT_2F c = D2D1::Point2F(tx + tw * t, cy);
    g_sw.rt->FillEllipse(D2D1::Ellipse(c, hot ? 10.5f : 9.5f, hot ? 10.5f : 9.5f), Br(0xFFFFFF, 0.25f));
    g_sw.rt->FillEllipse(D2D1::Ellipse(c, hot ? 9.5f : 8.5f, hot ? 9.5f : 8.5f), Br(0x18181B));
    g_sw.rt->FillEllipse(D2D1::Ellipse(c, hot ? 6.5f : 5.5f, hot ? 6.5f : 5.5f), Br(kAccent));
    std::wstring text = row.fmt ? row.fmt(v) : std::to_wstring(v);
    auto l = Lay(text, g_sw.fSeg.Get(), 60);
    if (l) {
        l->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        DWRITE_TEXT_METRICS m = {};
        l->GetMetrics(&m);
        g_sw.rt->DrawTextLayout(D2D1::Point2F(tx + tw + 12, cy - m.height / 2), l.Get(), Br(kSub));
    }
    SwHit& h = AddHit(D2D1::RectF(tx - 10, cy - 13, tx + tw + 10, cy + 13), nullptr);
    h.slider = true;
    h.key = row.key;
    h.tx = tx;
    h.tw = tw;
    h.min = row.min;
    h.max = row.max;
    h.step = row.step;
    std::wstring key = row.key;
    h.commit = [key](int nv) { CfgSet(key, std::to_wstring(nv)); };
}

static void DrawButton(D2D1_RECT_F r, const std::wstring& label, std::function<void()> click, bool strong = false) {
    bool hot = Hot(r);
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(r, 8, 8);
    if (strong) g_sw.rt->FillRoundedRectangle(rr, Br(kAccent, hot ? 0.88f : 1));
    else g_sw.rt->FillRoundedRectangle(rr, Br(0xFFFFFF, hot ? 0.1f : 0.06f));
    if (!strong) g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), 8, 8),
                                               Br(0xFFFFFF, 0.1f), 1);
    TxtC(label, g_sw.fSeg.Get(), r, strong ? kBg : kInk);
    AddHit(r, std::move(click));
}

static void DrawButtons(float x, float cy, const SwRow& row) {
    float ox = x;
    for (auto& b : row.buttons) {
        float bw = Measure(b.first, g_sw.fSeg.Get()).width + 28;
        DrawButton(D2D1::RectF(ox, cy - 17, ox + bw, cy + 17), b.first, b.second);
        ox += bw + 8;
    }
}

static COLORREF HexColor(const std::wstring& hex, COLORREF def) {
    const wchar_t* s = hex.c_str();
    if (*s == L'#') s++;
    if (wcslen(s) != 6) return def;
    unsigned long v = wcstoul(s, nullptr, 16);
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static void PickCustomTint() {
    static COLORREF custom[16] = {};
    CHOOSECOLORW cc = {sizeof(cc)};
    cc.hwndOwner = g_sw.hwnd;
    cc.rgbResult = HexColor(CfgStr(L"tintColor"), RGB(5, 5, 6));
    cc.lpCustColors = custom;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (!ChooseColorW(&cc)) return;
    wchar_t b[8];
    swprintf_s(b, L"%02X%02X%02X", GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
    CfgSet(L"tintColor", b);
}

// Glass color: the look's own, a few dark tints, or any color.
static void DrawSwatches(float x, float cy) {
    std::wstring cur = CfgStr(L"tintColor");
    for (auto& c : cur) c = towupper(c);
    if (!cur.empty() && cur[0] == L'#') cur = cur.substr(1);
    bool preset = false;
    for (auto* s : kSwatches) preset |= cur == s;
    auto swatch = [&](float sx, D2D1_COLOR_F fill, bool on, std::function<void()> click, bool plus = false) {
        D2D1_POINT_2F c = D2D1::Point2F(sx + 11, cy);
        D2D1_RECT_F r = D2D1::RectF(sx - 3, cy - 14, sx + 25, cy + 14);
        bool hot = Hot(r);
        if (on || hot) g_sw.rt->DrawEllipse(D2D1::Ellipse(c, 14, 14), Br(kAccent, on ? 1 : 0.35f), 1.6f);
        if (plus) {
            g_sw.rt->FillEllipse(D2D1::Ellipse(c, 11, 11), Br(0xFFFFFF, 0.06f));
            g_sw.rt->DrawEllipse(D2D1::Ellipse(c, 10.5f, 10.5f), Br(0xFFFFFF, 0.25f), 1);
            Icon(L"M12 7v10 M7 12h10", c.x - 8, c.y - 8, 16, kInk, 0.9f, 2);
        } else {
            fill.a = 1;
            g_sw.rt->FillEllipse(D2D1::Ellipse(c, 11, 11), Br(fill));
            g_sw.rt->DrawEllipse(D2D1::Ellipse(c, 10.5f, 10.5f), Br(0xFFFFFF, 0.22f), 1);
        }
        AddHit(r, std::move(click));
    };
    float sx = x;
    // the look's own color, drawn as a half and half disc
    {
        Theme t = MakeTheme(CfgStr(L"style", L"nevoa"), 1);
        D2D1_POINT_2F c = D2D1::Point2F(sx + 11, cy);
        D2D1_RECT_F r = D2D1::RectF(sx - 3, cy - 14, sx + 25, cy + 14);
        bool on = cur.empty();
        if (on || Hot(r)) g_sw.rt->DrawEllipse(D2D1::Ellipse(c, 14, 14), Br(kAccent, on ? 1 : 0.35f), 1.6f);
        D2D1_COLOR_F f = t.tint;
        f.a = 1;
        g_sw.rt->FillEllipse(D2D1::Ellipse(c, 11, 11), Br(f));
        TxtC(L"A", g_sw.fSmall.Get(), D2D1::RectF(c.x - 11, c.y - 11, c.x + 11, c.y + 11), (f.r + f.g + f.b) > 1.5f ? 0x1B1B1F : kInk);
        g_sw.rt->DrawEllipse(D2D1::Ellipse(c, 10.5f, 10.5f), Br(0xFFFFFF, 0.22f), 1);
        AddHit(r, [] { CfgSet(L"tintColor", L""); });
        sx += 30;
    }
    for (auto* s : kSwatches) {
        COLORREF c = HexColor(s, 0);
        std::wstring v = s;
        swatch(sx, Rgba(GetRValue(c), GetGValue(c), GetBValue(c), 1), cur == s, [v] { CfgSet(L"tintColor", v); });
        sx += 30;
    }
    if (!cur.empty() && !preset) {
        COLORREF c = HexColor(cur, 0);
        swatch(sx, Rgba(GetRValue(c), GetGValue(c), GetBValue(c), 1), true, [] { PickCustomTint(); });
    } else {
        swatch(sx, {}, false, [] { PickCustomTint(); }, true);
    }
}

// A card with rows (label and description on the left, the control on the right).
static void Card(std::vector<SwRow> rows, float& y, float x, float w) {
    std::vector<float> hs, ths;
    float total = 0;
    for (auto& r : rows) {
        float cw = ControlWidth(r);
        float tw = w - 36 - (cw > 0 ? cw + 20 : 0);
        float th = Measure(r.label, g_sw.fLbl.Get(), tw).height;
        if (!r.desc.empty()) th += 3 + Measure(r.desc, g_sw.fDesc.Get(), tw).height;
        float h = std::max(52.0f, th + 26);
        hs.push_back(h);
        ths.push_back(th);
        total += h;
    }
    D2D1_RECT_F card = D2D1::RectF(x, y, x + w, y + total);
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(card, 12, 12), Br(0xFFFFFF, 0.035f));
    g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x + 0.5f, y + 0.5f, x + w - 0.5f, y + total - 0.5f), 12, 12),
                                  Br(0xFFFFFF, 0.06f), 1);
    float ry = y;
    for (size_t i = 0; i < rows.size(); i++) {
        auto& r = rows[i];
        if (i) g_sw.rt->FillRectangle(D2D1::RectF(x + 1, ry, x + w - 1, ry + 1), Br(0xFFFFFF, 0.05f));
        float cw = ControlWidth(r);
        float tw = w - 36 - (cw > 0 ? cw + 20 : 0);
        float ty = ry + (hs[i] - ths[i]) / 2;
        float lh = Txt(r.label, g_sw.fLbl.Get(), x + 18, ty, tw, kInk);
        if (!r.desc.empty()) Txt(r.desc, g_sw.fDesc.Get(), x + 18, ty + lh + 3, tw, kSub);
        float cx = x + w - 18 - cw, cy = ry + hs[i] / 2;
        switch (r.ctl) {
            case CtlToggle: DrawToggle(cx, cy, r.on(), r.flip); break;
            case CtlSeg: DrawSeg(cx, cy, r); break;
            case CtlSlider: DrawSlider(cx, cy, r); break;
            case CtlButtons: DrawButtons(cx, cy, r); break;
            case CtlSwatches: DrawSwatches(cx, cy); break;
            default: break;
        }
        ry += hs[i];
    }
    y += total;
}

static void Heading(float& y, float x, float w, const wchar_t* title, const wchar_t* sub) {
    y += Txt(title, g_sw.fH1.Get(), x, y, w, kInk);
    if (sub) y += 6 + Txt(sub, g_sw.fSub.Get(), x, y + 6, w, kSub);
}

static void Section(float& y, float x, float w, const wchar_t* title) {
    y += 24;
    y += Txt(title, g_sw.fSec.Get(), x + 4, y, w, kSub);
    y += 9;
}

// ---- Illustrations ---------------------------------------------------------

// A dim wallpaper, so the glass shows what it does.
static void Wallpaper(D2D1_RECT_F r, float radius) {
    D2D1_GRADIENT_STOP stops[] = {{0, D2D1::ColorF(0x2E3446)}, {0.55f, D2D1::ColorF(0x1E2029)}, {1, D2D1::ColorF(0x121216)}};
    ComPtr<ID2D1GradientStopCollection> sc;
    ComPtr<ID2D1LinearGradientBrush> lin;
    g_sw.rt->CreateGradientStopCollection(stops, 3, &sc);
    if (sc) g_sw.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.right, r.bottom)),
                                               sc.Get(), &lin);
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(r, radius, radius);
    if (lin) g_sw.rt->FillRoundedRectangle(rr, lin.Get());
    else g_sw.rt->FillRoundedRectangle(rr, Br(0x08080A));
    D2D1_GRADIENT_STOP glow[] = {{0, D2D1::ColorF(0x5A6A8C, 0.45f)}, {1, D2D1::ColorF(0x5A6A8C, 0)}};
    ComPtr<ID2D1GradientStopCollection> gc;
    ComPtr<ID2D1RadialGradientBrush> rad;
    g_sw.rt->CreateGradientStopCollection(glow, 2, &gc);
    float rw = (r.right - r.left) * 0.45f;
    if (gc) g_sw.rt->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(r.left + (r.right - r.left) * 0.72f, r.top + 10), {}, rw, rw * 0.7f),
                                               gc.Get(), &rad);
    if (rad) g_sw.rt->FillRoundedRectangle(rr, rad.Get());
}

// A glass bar in a look's colors.
static void GlassBar(D2D1_RECT_F r, float radius, const Theme& t, float tintAlpha) {
    D2D1_COLOR_F tint = t.tint;
    if (t.blur) tint.a = tintAlpha;
    if (t.blur) g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), Br(0xFFFFFF, 0.05f));  // the blur's haze
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), Br(tint));
    D2D1_RECT_F e = D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f);
    if (t.edgeHorizontal) {
        D2D1_GRADIENT_STOP stops[] = {{0, t.edge[0]}, {0.5f, t.edge[1]}, {1, t.edge[2]}};
        ComPtr<ID2D1GradientStopCollection> sc;
        ComPtr<ID2D1LinearGradientBrush> lin;
        g_sw.rt->CreateGradientStopCollection(stops, 3, &sc);
        if (sc) g_sw.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, 0), D2D1::Point2F(r.right, 0)), sc.Get(), &lin);
        if (lin) g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(e, radius, radius), lin.Get(), 1);
    } else {
        D2D1_GRADIENT_STOP stops[] = {{0, t.edge[0]}, {1, t.edge[1]}};
        ComPtr<ID2D1GradientStopCollection> sc;
        ComPtr<ID2D1LinearGradientBrush> lin;
        g_sw.rt->CreateGradientStopCollection(stops, 2, &sc);
        if (sc) g_sw.rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)), sc.Get(), &lin);
        if (lin) g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(e, radius, radius), lin.Get(), 1);
    }
}

static ID2D1Bitmap* SwAppBitmap(const App& a) {
    if (!a.icon) return nullptr;
    auto& b = g_sw.bmps[a.key];
    if (!b) g_sw.rt->CreateBitmapFromWicBitmap(a.icon.Get(), &b);
    return b.Get();
}

static void DrawStartTile(D2D1_RECT_F r, const Theme& t) {
    float s = r.right - r.left;
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, s * 0.27f, s * 0.27f), Br(t.startTile));
    float q = s * 0.17f, gp = s * 0.05f, cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    for (int i = 0; i < 4; i++) {
        float x0 = i % 2 ? cx + gp / 2 : cx - gp / 2 - q, y0 = i / 2 ? cy + gp / 2 : cy - gp / 2 - q;
        g_sw.rt->FillRectangle(D2D1::RectF(x0, y0, x0 + q, y0 + q), Br(t.startGlyph));
    }
}

// The dock, as the current settings draw it, with the user's own apps. The mouse zooms it like the real one.
static void DockPreview(D2D1_RECT_F box) {
    g_sw.preview = box;
    Wallpaper(box, 12);
    g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(box.left + 0.5f, box.top + 0.5f, box.right - 0.5f, box.bottom - 0.5f), 12, 12),
                                  Br(0xFFFFFF, 0.06f), 1);
    g_sw.rt->PushAxisAlignedClip(box, D2D1_ANTIALIAS_MODE_ALIASED);
    std::wstring style = CfgStr(L"style", L"nevoa"), anim = CfgStr(L"animation", L"zoom");
    Theme t = MakeTheme(style, SwVal(L"tintOpacity") / 100.0f);
    std::wstring tc = CfgStr(L"tintColor");
    if (!tc.empty() && style != L"grafite") {
        COLORREF c = HexColor(tc, 0);
        t.tint = Rgba(GetRValue(c), GetGValue(c), GetBValue(c), t.tint.a);
    }
    bool fixed = CfgStr(L"format", L"dock") == L"fixa";
    float k = 0.62f;
    float isz = (float)Clamp(SwVal(L"iconSize"), 24, 128);
    if (isz * k > 58) k = 58 / isz;
    float S = isz * k, G = Clamp(SwVal(L"iconGap"), 0, 64) * k, padY = 10 * k, dotRoom = 7 * k;
    float pillH = S + padY * 2 + dotRoom, Z = Clamp(SwVal(L"magnification"), 100, 250) / 100.0f;

    std::vector<const App*> apps;
    for (auto& a : g_apps) {
        if (a.start && !CfgInt(L"showStart")) continue;
        if (!a.start && !a.pinned && a.wins.empty()) continue;
        if (apps.size() >= 10) break;
        apps.push_back(&a);
    }
    size_t n = apps.size();
    float padX = padY + G * 0.5f;
    float baseW = n * S + (n ? (n - 1) * G : 0) + padX * 2;
    float x0 = fixed ? box.left + 18 : (box.left + box.right - baseW) / 2 + padX;
    // magnification by the mouse distance, like the dock's wave
    std::vector<float> m(n, 1.0f);
    bool over = g_sw.mouseIn && InRect(box, g_sw.mouse);
    int nearest = -1;
    if (over) {
        float best = 1e9f;
        for (size_t i = 0; i < n; i++) {
            float c = x0 + i * (S + G) + S / 2, d = fabsf(g_sw.mouse.x - c);
            if (d < best && d < (S + G) * 0.8f) { best = d; nearest = (int)i; }
            if (anim == L"zoom") {
                float u = d / ((S + G) * 2.4f);
                if (u < 1) m[i] = 1 + (Z - 1) * (cosf(u * 3.14159f) + 1) / 2;
            }
        }
        if (anim == L"foco" && nearest >= 0) m[nearest] = std::min(Z, 1.2f);
    }
    float extra = 0;
    for (size_t i = 0; i < n; i++) extra += S * (m[i] - 1);
    float bottom = box.bottom - (fixed ? 0 : std::max(6.0f, Clamp(SwVal(L"bottomGap"), 0, 100) * k + 6));
    D2D1_RECT_F pill = fixed ? D2D1::RectF(box.left, bottom - pillH, box.right, bottom)
                             : D2D1::RectF(x0 - padX - extra / 2, bottom - pillH, x0 - padX + baseW + extra / 2, bottom);
    float radius = fixed ? 0 : std::min(pillH / 2, S * 0.42f);
    GlassBar(pill, radius, t, t.tint.a);

    float x = fixed ? x0 : x0 - extra / 2;
    for (size_t i = 0; i < n; i++) {
        const App& a = *apps[i];
        float s = S * m[i];
        float lift = (anim == L"elevar" && (int)i == nearest) ? S * 0.22f : 0;
        float iy = pill.bottom - padY - dotRoom - s - lift;
        D2D1_RECT_F ir = D2D1::RectF(x, iy, x + s, iy + s);
        float op = (anim == L"foco" && nearest >= 0 && (int)i != nearest) ? 0.45f : 1;
        if (a.start) {
            DrawStartTile(ir, t);
        } else if (ID2D1Bitmap* bmp = SwAppBitmap(a)) {
            if (g_sw.dc) g_sw.dc->DrawBitmap(bmp, ir, op, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
            else g_sw.rt->DrawBitmap(bmp, ir, op);
        } else {
            g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(ir, s * 0.27f, s * 0.27f), Br(0xFFFFFF, 0.14f * op));
        }
        if (!a.wins.empty())
            g_sw.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + s / 2, pill.bottom - padY * 0.55f - dotRoom * 0.35f), 1.8f, 1.8f), Br(t.dot));
        x += s + G;
    }
    g_sw.rt->PopAxisAlignedClip();
}

// Cards to choose one of a few things, each with a small picture.
struct SwPick {
    std::wstring id, name, hint;
};
static void PickGrid(float& y, float x, float w, const std::vector<SwPick>& items, const std::wstring& cur,
                     std::function<void(const SwPick&, D2D1_RECT_F)> draw, std::function<void(const std::wstring&)> pick) {
    int cols = (int)items.size();
    float gap = 10, cw = (w - gap * (cols - 1)) / cols;
    float h = 12 + 76 + 10 + 19 + 2 + 17 + 12;
    for (int i = 0; i < cols; i++) {
        auto& it = items[i];
        D2D1_RECT_F r = D2D1::RectF(x + i * (cw + gap), y, x + i * (cw + gap) + cw, y + h);
        bool on = it.id == cur, hot = Hot(r);
        g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 12, 12), Br(0xFFFFFF, on ? 0.06f : hot ? 0.045f : 0.025f));
        g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), 12, 12),
                                      on ? Br(kAccent) : Br(0xFFFFFF, hot ? 0.14f : 0.07f), 1);
        D2D1_RECT_F ill = D2D1::RectF(r.left + 12, r.top + 12, r.right - 12, r.top + 88);
        draw(it, ill);
        Txt(it.name, g_sw.fLbl.Get(), r.left + 12, ill.bottom + 10, cw - 24, kInk);
        Txt(it.hint, g_sw.fDesc.Get(), r.left + 12, ill.bottom + 31, cw - 24, kSub);
        std::wstring id = it.id;
        if (!on) AddHit(r, [pick, id] { pick(id); });
    }
    y += h;
}

// ---- Pages -----------------------------------------------------------------

static void WizStart();
static void PageGeral(float& y, float x, float w) {
    Heading(y, x, w, L2(L"Geral", L"General"), L2(L"O visual vale pra dock, o Iniciar e as prévias das janelas.",
                                                      L"The look applies to the dock, Start and the window previews."));
    y += 14;
    Card({RButtons(L2(L"Configurar com perguntas", L"Set up with questions"),
                   L2(L"Umas perguntas rápidas deixam tudo pronto. Dá pra mudar depois aqui nos Ajustes.",
                      L"A few quick questions set everything up. You can change it later here in Settings."),
                   {{L2(L"Começar", L"Start"), [] {
                         WizStart();
                         SwSetPage(PgComecar);
                     }}})},
         y, x, w);
    Section(y, x, w, L2(L"Visual", L"Look"));
    std::vector<SwPick> looks = {
        {L"nevoa", L"Névoa", L2(L"Vidro fumê", L"Smoked glass")},
        {L"cristal", L"Cristal", L2(L"Vidro claro", L"Light glass")},
        {L"grafite", L"Grafite", L2(L"Sólido, o mais leve", L"Solid, the lightest")},
        {L"aurora", L"Aurora", L2(L"Borda colorida", L"Colored edge")},
    };
    PickGrid(y, x, w, looks, CfgStr(L"style", L"nevoa"),
             [](const SwPick& p, D2D1_RECT_F ill) {
                 Wallpaper(ill, 8);
                 Theme t = MakeTheme(p.id, CfgInt(L"tintOpacity") / 100.0f);
                 float bw = (ill.right - ill.left) * 0.64f, cx = (ill.left + ill.right) / 2;
                 D2D1_RECT_F bar = D2D1::RectF(cx - bw / 2, ill.bottom - 30, cx + bw / 2, ill.bottom - 12);
                 GlassBar(bar, 9, t, t.tint.a);
                 for (int i = 0; i < 4; i++) {
                     float s = 10, ix = bar.left + 9 + i * 15;
                     D2D1_RECT_F ir = D2D1::RectF(ix, bar.top + 4, ix + s, bar.top + 4 + s);
                     g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(ir, 3, 3), Br(i ? t.text : t.startTile));
                 }
             },
             [](const std::wstring& id) { CfgSet(L"style", id); });
    y += 12;
    SwRow color;
    color.label = L2(L"Cor do vidro", L"Glass color");
    color.desc = L2(L"A: a do visual escolhido. O + escolhe qualquer cor.", L"A: the chosen look's own. + picks any color.");
    color.ctl = CtlSwatches;
    Card({color,
          RSlider(L2(L"Força da cor do vidro", L"Glass color strength"),
                  L2(L"Mais baixo deixa ver mais o fundo. O Grafite é sempre sólido.", L"Lower shows more of what's behind. Grafite is always solid."),
                  L"tintOpacity", 0, 100, 1, Pct),
          RSeg(L2(L"Animação ao passar o mouse", L"Hover animation"),
               L2(L"Zoom faz a onda do macOS; Elevar sobe só o ícone; Foco apaga os outros.",
                  L"Zoom makes the macOS wave; Lift raises the icon; Focus dims the others."),
               L"animation",
               {{L"zoom", L"Zoom"}, {L"elevar", L2(L"Elevar", L"Lift")}, {L"foco", L2(L"Foco", L"Focus")}, {L"nenhuma", L2(L"Nenhuma", L"None")}})},
         y, x, w);
    Section(y, x, w, L"Windows");
    Card({RToggle(L2(L"Esconder a barra do Windows", L"Hide the Windows taskbar"),
                  L2(L"Pra ela não reservar espaço, o Nimbus liga o \"Ocultar automaticamente a barra de tarefas\" do Windows. Tudo volta como era quando o Nimbus para (também ao sair da conta ou depois de um travamento).",
                     L"So it doesn't reserve space, Nimbus turns on Windows' \"Automatically hide the taskbar\". Everything goes back when Nimbus stops (also on sign-out or after a crash)."),
                  L"hideWindowsTaskbar")},
         y, x, w);
}

static void PageDock(float& y, float x, float w) {
    Heading(y, x, w, L"Dock", L2(L"Tamanho, formato e quando ela aparece. Passe o mouse na prévia.",
                                 L"Size, format and when it shows up. Hover the preview."));
    y += 18;
    DockPreview(D2D1::RectF(x, y, x + w, y + 160));
    y += 160;
    Section(y, x, w, L2(L"Formato", L"Format"));
    Card({RSeg(L2(L"Formato", L"Format"),
               L2(L"Flutuante é a pílula solta. A barra inteira cola embaixo e reserva o espaço.",
                  L"Floating is the loose pill. The full-width bar sits at the bottom and reserves its space."),
               L"format", {{L"dock", L2(L"Flutuante", L"Floating")}, {L"fixa", L2(L"Barra inteira", L"Full width")}}),
          RSlider(L2(L"Tamanho dos ícones", L"Icon size"), nullptr, L"iconSize", 24, 128, 1, Px),
          RSlider(L2(L"Espaço entre os ícones", L"Space between icons"), nullptr, L"iconGap", 0, 48, 1, Px),
          RSlider(L2(L"Zoom ao passar o mouse", L"Hover zoom"), L2(L"100% desliga.", L"100% turns it off."), L"magnification", 100, 250, 5, Pct)},
         y, x, w);
    Section(y, x, w, L2(L"Aparecer e esconder", L"Show and hide"));
    Card({RToggle(L2(L"Esconder sozinha", L"Auto-hide"),
                  L2(L"Aparece quando o mouse encosta na borda de baixo.", L"Shows up when the mouse touches the bottom edge."),
                  L"autoHide"),
          RSlider(L2(L"Tempo até esconder", L"Delay before hiding"), nullptr, L"hideDelay", 0, 3000, 50, Secs),
          RSlider(L2(L"Distância da borda", L"Distance from the edge"), L2(L"Só na dock flutuante.", L"Floating dock only."), L"bottomGap", 0, 60, 1, Px)},
         y, x, w);
}

static void PageIniciar(float& y, float x, float w) {
    Heading(y, x, w, L2(L"Iniciar", L"Start"), L2(L"O Iniciar do Nimbus, a tecla Windows e a busca.", L"Nimbus' Start, the Windows key and search."));
    Section(y, x, w, L2(L"Abrir", L"Opening"));
    std::vector<std::pair<std::wstring, std::wstring>> who = {{L"nevoa", L"Nimbus"}, {L"windows", L"Windows"}};
    Card({RToggle(L2(L"Botão Iniciar na dock", L"Start button on the dock"), nullptr, L"showStart"),
          RSeg(L2(L"O botão Iniciar abre", L"The Start button opens"), nullptr, L"startMenu", who),
          RSeg(L2(L"A tecla Windows abre", L"The Windows key opens"),
               L2(L"Ctrl+Esc sempre abre o Iniciar do Windows.", L"Ctrl+Esc always opens Windows' Start."), L"winKeyMode", who),
          RSeg(L2(L"Atalho extra", L"Extra shortcut"),
               L2(L"O atalho para de funcionar nos outros apps enquanto o Nimbus roda.",
                  L"The shortcut stops working in other apps while Nimbus runs."),
               L"startHotkey",
               {{L"none", L2(L"Nenhum", L"None")}, {L"altSpace", L2(L"Alt+Espaço", L"Alt+Space")}, {L"ctrlSpace", L2(L"Ctrl+Espaço", L"Ctrl+Space")}})},
         y, x, w);
    Section(y, x, w, L2(L"Busca", L"Search"));
    Card({RSeg(L2(L"Layout da busca", L"Search layout"),
               L2(L"A larga mostra o painel do item e as telas das Configurações ao lado.",
                  L"Wide shows the item panel and Windows Settings pages beside the results."),
               L"searchLayout", {{L"larga", L2(L"Larga", L"Wide")}, {L"compacta", L2(L"Compacta", L"Compact")}}),
          RSeg(L2(L"Busca de arquivos", L"File search"),
               L2(L"O Everything (grátis) acha qualquer arquivo na hora. Sem ele, abre a busca do Windows.",
                  L"Everything (free) finds any file instantly. Without it, Windows Search opens."),
               L"fileSearch",
               {{L"everything", L"Everything"}, {L"windows", L"Windows"}, {L"off", L2(L"Desligada", L"Off")}})},
         y, x, w);
    Section(y, x, w, L"Menus");
    Card({RSeg(L2(L"Finalizar tarefa no menu dos apps", L"End task in the apps' menu"),
               L2(L"\"Como no Windows\" segue a opção de Desenvolvedores do sistema.",
                  L"\"Like Windows\" follows the system's For developers option."),
               L"endTask",
               {{L"windows", L2(L"Como no Windows", L"Like Windows")}, {L"on", L2(L"Sempre", L"Always")}, {L"off", L2(L"Nunca", L"Never")}})},
         y, x, w);
}

static void PageStatus(float& y, float x, float w) {
    Heading(y, x, w, L2(L"Área de status", L"Status area"),
            L2(L"Cada item na dock, na central de controle (um botão na dock abre tudo junto), no Iniciar ou desligado.",
               L"Each item in the dock, in the control center (one dock button opens them all), in Start or off."));
    {
        SwRow tabs = RSeg(L2(L"Seção", L"Section"), nullptr, L"#statusTab",
                          {{L"0", L2(L"Itens", L"Items")}, {L"1", L2(L"Bandeja", L"Tray")}, {L"2", L2(L"Prateleira", L"Shelf")},
                           {L"3", L2(L"Notificações", L"Notifications")}, {L"4", L"Player"}});
        tabs.cur = [] { return std::to_wstring(g_sw.statusTab); };
        tabs.pick = [](const std::wstring& v) {
            g_sw.statusTab = _wtoi(v.c_str());
            SwInvalidate();
        };
        y += 14;
        Card({tabs}, y, x, w);
    }
    if (g_sw.statusTab == 0) {
    Section(y, x, w, L2(L"Onde fica cada um", L"Where each goes"));
    Card({RPlace(L2(L"Relógio e data", L"Clock and date"), L2(L"O clique abre o calendário do mês.", L"A click opens the month's calendar."),
                 L"statusClock"),
          RPlace(L"Volume",
                 L2(L"Rolar o mouse no ícone muda o volume; o botão do meio deixa mudo.",
                    L"Scroll on the icon to change the volume; the middle button mutes."),
                 L"statusVolume"),
          RPlace(L2(L"Rede", L"Network"), L2(L"Cabo ou Wi-Fi, com a lista de redes.", L"Cable or Wi-Fi, with the list of networks."),
                 L"statusNetwork"),
          RPlace(L2(L"Bateria", L"Battery"), L2(L"Só aparece em notebook.", L"Only shows on laptops."), L"statusBattery"),
          RPlace(L2(L"Idioma do teclado", L"Keyboard language"),
                 L2(L"Um clique troca; o botão direito abre a lista.", L"A click switches; right-click opens the list."),
                 L"statusKeyboard"),
          RPlace(L2(L"Bandeja", L"Tray"), L2(L"Os ícones dos programas em segundo plano.", L"The icons of the apps running in the background."),
                 L"trayPlace"),
          RPlace(L2(L"Não incomodar", L"Do not disturb"), L2(L"Pausa os avisos do Nimbus (o sino continua guardando).", L"Pauses Nimbus' notifications (the bell still keeps them)."),
                 L"dndPlace"),
          RPlace(L"Bluetooth", L2(L"Só aparece quando o PC tem Bluetooth.", L"Only shows when the PC has Bluetooth."), L"btPlace"),
          RPlace(L2(L"Luz noturna", L"Night light"), nullptr, L"nightPlace"),
          RPlace(L2(L"Atalhos", L"Shortcuts"), L2(L"Projetar, transmitir, legendas e acessibilidade.", L"Project, cast, captions and accessibility."), L"shortPlace"),
          RPlace(L2(L"Brilho", L"Brightness"), L2(L"De cada monitor (os que aceitam DDC/CI).", L"Of each monitor (those with DDC/CI)."), L"lightPlace"),
          RPlace(L2(L"Energia", L"Power"), L2(L"Economia, equilibrado ou desempenho.", L"Efficiency, balanced or performance."), L"powerPlace"),
          RPlace(L"Player", L2(L"O que está tocando em qualquer app. Mais opções na aba Player.", L"What any app is playing. More in the Player tab."),
                 L"playerPlace"),
          RPlace(L2(L"Dispositivos (baterias)", L"Devices (batteries)"),
                 L2(L"Fones, mouses e controles sem fio (Bluetooth e Xbox). Só aparece com algum aparelho.",
                    L"Wireless headsets, mice and controllers (Bluetooth and Xbox). Only shows with a device."),
                 L"devPlace"),
          RToggle(L2(L"Dispositivos: usar o BatteryHub", L"Devices: use BatteryHub"),
                  L2(L"Pra receptores USB (Logitech, Razer…). Desligado por padrão: ligado, o Nimbus abre o BatteryHub (instalado à parte) escondido a cada 5 minutos, só pra ler as baterias; ele fecha sozinho.",
                     L"For USB receivers (Logitech, Razer…). Off by default: on, Nimbus runs BatteryHub (installed separately) hidden every 5 minutes, just to read the batteries; it closes by itself."),
                  L"devHub"),
          RPlace(L2(L"Notas e lembretes", L"Notes and reminders"),
                 L2(L"Notas que ficam até você apagar, e lembretes que avisam no dia (mesmo atrasados).",
                    L"Notes that stay until you delete them, and reminders that tell you on the day (late too)."),
                 L"notesPlace"),
          RPlace(L2(L"Notificações", L"Notifications"),
                 L2(L"O sino com o contador; abre a lista, da mais nova pra mais antiga.", L"The bell with the count; it opens the list, newest first."),
                 L"notifPlace"),
          RPlace(L2(L"Prateleira", L"Shelf"),
                 L2(L"Guarda arquivos por um tempo: arraste pra dock, depois arraste pra onde quiser.",
                    L"Keeps files for a moment: drag them to the dock, then drag them where you want."),
                 L"shelfPlace")},
         y, x, w);
    Section(y, x, w, L2(L"Na dock", L"In the dock"));
    SwRow side = RSeg(L2(L"Lado", L"Side"),
                      L2(L"Na barra inteira, os itens ficam no canto.", L"On the full-width bar, the items go to the corner."), L"statusPos",
                      {{L"direita", L2(L"Direita", L"Right")}, {L"esquerda", L2(L"Esquerda", L"Left")}});
    side.pick = [](const std::wstring& v) {
        // Items still following the old "where" (Start tab, off) keep their place.
        for (const wchar_t* k : {L"statusClock", L"statusVolume", L"statusNetwork", L"statusBattery", L"statusKeyboard"})
            CfgSet(k, PlaceName(PlaceOf(CfgStr(k, L"1"))));
        CfgSet(L"statusPos", v);
    };
    Card({side}, y, x, w);
    Section(y, x, w, L2(L"Ao clicar", L"On click"));
    Card({RSeg(L2(L"Abrir", L"Open"),
               L2(L"O do Windows abre as Configurações rápidas ou o calendário do próprio Windows.",
                  L"Windows' opens its own Quick Settings or calendar."),
               L"statusPanels", {{L"nimbus", L2(L"Painel do Nimbus", L"Nimbus panel")}, {L"windows", L2(L"Painel do Windows", L"Windows panel")}})},
         y, x, w);
    }
    if (g_sw.statusTab == 1) {
    Section(y, x, w, L2(L"Bandeja", L"Tray"));
    Card({RSeg(L2(L"Estilo na dock", L"Style in the dock"),
               L2(L"O botão do meio fixa um ícone; no \"Botão\", os fixados ficam sempre à mostra.",
                  L"The middle button pins an icon; with \"Button\", pinned ones always show."),
               L"trayStyle",
               {{L"botao", L2(L"Botão", L"Button")}, {L"lado", L2(L"Lado a lado", L"Inline")}, {L"grade", L2(L"Grade", L"Grid")},
                {L"lista", L2(L"Lista", L"List")}}),
          RSlider(L2(L"Ícones à mostra", L"Icons shown"),
                  L2(L"Quantos cabem de uma vez; o resto rola com as setas ou a roda do mouse.",
                     L"How many fit at once; scroll the rest with the arrows or the mouse wheel."),
                  L"trayVisible", 3, 9, 1, [](int v) { return std::to_wstring(v); })},
         y, x, w);
    }
    if (g_sw.statusTab == 2) {
    Section(y, x, w, L2(L"Prateleira", L"Shelf"));
    Card({RSeg(L2(L"Ao arrastar pra uma pasta", L"Dragged to a folder"),
               L2(L"Arrastar com o botão direito sempre pergunta. O que foi colado (Ctrl+V) sempre é movido.",
                  L"Dragging with the right button always asks. What was pasted (Ctrl+V) is always moved."),
               L"shelfDrag",
               {{L"copiar", L2(L"Copiar", L"Copy")}, {L"mover", L2(L"Mover", L"Move")}, {L"windows", L2(L"Como no Windows", L"Like Windows")}}),
          RSeg(L2(L"Estilo", L"Style"), nullptr, L"shelfStyle", {{L"grade", L2(L"Grade", L"Grid")}, {L"lista", L2(L"Lista", L"List")}}),
          RSlider(L2(L"Itens no máximo", L"Most items"), L2(L"Passou disso, o mais antigo sai.", L"Past that, the oldest goes."),
                  L"shelfLimit", 5, 20, 1, [](int v) { return std::to_wstring(v); })},
         y, x, w);
    }
    if (g_sw.statusTab == 4) {
    Section(y, x, w, L"Player");
    Card({RPlace(L2(L"Onde fica", L"Where it goes"),
                 L2(L"Mostra o que qualquer app está tocando nos controles de mídia do Windows: Spotify, YouTube no navegador, players de vídeo.",
                    L"Shows what any app plays in Windows' media controls: Spotify, YouTube in a browser, video players."),
                 L"playerPlace"),
          RSeg(L2(L"Na dock", L"In the dock"), L2(L"Barras: se mexem com o som. Sem nada tocando, fica o ícone.", L"Bars: they move with the sound. With nothing playing, the icon."),
               L"playerDock",
               {{L"icone", L2(L"Ícone", L"Icon")}, {L"capa", L2(L"Capa", L"Cover")}, {L"titulo", L2(L"Título", L"Title")},
                {L"ambos", L2(L"Capa e título", L"Both")}, {L"espectro", L2(L"Barras", L"Bars")}}),
          RToggle(L2(L"Some quando nada está tocando", L"Hidden when nothing plays"), L2(L"Só na dock.", L"Only in the dock."), L"playerHide")},
         y, x, w);
    Section(y, x, w, L2(L"Atalhos do mouse", L"Mouse shortcuts"));
    Card({RButtons(L2(L"No item da dock", L"On the dock item"),
                   L2(L"Clique abre o player; botão do meio toca ou pausa; a roda muda o volume só desse app.",
                      L"A click opens the player; the middle button plays or pauses; the wheel changes only that app's volume."),
                   {})},
         y, x, w);
    }
    if (g_sw.statusTab == 3) {
    Section(y, x, w, L2(L"Notificações", L"Notifications"));
    Card({RToggle(L2(L"Mostrar os avisos com o visual do Nimbus", L"Show notifications in Nimbus' look"),
                  L2(L"Os avisos do Windows também aparecem como cartões do Nimbus. Em tela cheia ou jogo, nada aparece.",
                     L"Windows' notifications also show as Nimbus cards. Nothing shows over full-screen apps or games."),
                  L"notifShow"),
          RSeg(L2(L"Altura", L"Height"), nullptr, L"notifV", {{L"baixo", L2(L"Embaixo", L"Bottom")}, {L"cima", L2(L"Em cima", L"Top")}}),
          RSeg(L2(L"Lado", L"Side"), nullptr, L"notifH",
               {{L"direita", L2(L"Direita", L"Right")}, {L"centro", L2(L"Centro", L"Center")}, {L"esquerda", L2(L"Esquerda", L"Left")}}),
          RSlider(L2(L"Tempo na tela", L"Time on screen"), L2(L"Com o mouse em cima, o aviso espera.", L"With the mouse on it, it waits."),
                  L"notifSecs", 3, 12, 1, [](int v) { return std::to_wstring(v) + L" s"; }),
          [] {
              bool on = g_set.notifHideWin, confirm = g_sw.confirmBanners;
              return RButtons(L2(L"Esconder os avisos do Windows", L"Hide Windows' own banners"),
                              on ? L2(L"Escondidos: o balão de cada app no Windows está desligado. Os avisos continuam na central do Windows.",
                                      L"Hidden: each app's banner in Windows is off. Notifications still go to Windows' center.")
                              : confirm ? L2(L"Clique de novo pra confirmar: o Nimbus desliga o balão de cada app no Windows e liga de volta quando você desligar isso ou o Nimbus.",
                                             L"Click again to confirm: Nimbus turns off each app's banner in Windows and turns it back on when you turn this or Nimbus off.")
                                        : L2(L"Pra não aparecerem dois avisos. Nada some: eles continuam na central do Windows.",
                                             L"So there aren't two. Nothing is lost: they stay in Windows' center."),
                              {{on ? L2(L"Mostrar de novo", L"Show them again") : confirm ? L2(L"Confirmar", L"Confirm") : L2(L"Esconder", L"Hide"), [] {
                                    if (g_set.notifHideWin) {
                                        CfgSet(L"notifHideWin", L"0");
                                        g_sw.confirmBanners = false;
                                    } else if (g_sw.confirmBanners) {
                                        Wh_SetStringValue((UserPrefix() + L"notif.askedBanners").c_str(), L"1");
                                        CfgSet(L"notifHideWin", L"1");
                                        g_sw.confirmBanners = false;
                                    } else {
                                        g_sw.confirmBanners = true;
                                    }
                                }}});
          }(),
          RToggle(L2(L"Win+N abre as notificações do Nimbus", L"Win+N opens Nimbus' notifications"),
                  L2(L"No lugar do painel do Windows, em qualquer janela. Desligado, o Win+N continua sendo do Windows.",
                     L"Instead of Windows' panel, over any window. Off, Win+N stays Windows'."),
                  L"notifWinN"),
          SoundRow(L2(L"Som dos avisos", L"Notification sound"), L"notifSound",
                   L2(L"Os de outros apps só tocam com os avisos do Windows escondidos (senão o Windows já toca o dele).",
                      L"Other apps' only play with Windows' banners hidden (otherwise Windows plays its own).")),
          SoundRow(L2(L"Som dos lembretes", L"Reminder sound"), L"remSound", L2(L"Os lembretes das suas notas.", L"Your notes' reminders.")),
          RButtons(L2(L"Testar um aviso", L"Try a notification"), L2(L"Mostra um aviso de exemplo agora.", L"Shows a sample notification now."),
                   {{L2(L"Testar", L"Try it"), [] { NotifTest(); }}})},
         y, x, w);
    }
}

static void LoadSwMonitors() {
    g_sw.mons.clear();
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR mon, HDC, LPRECT, LPARAM) WINAPI -> BOOL {
        MONITORINFO mi = {sizeof(mi)};
        if (!GetMonitorInfoW(mon, &mi)) return TRUE;
        SwMon m;
        m.mon = mon;
        m.who = MonitorIdentity(mon);
        m.group = MonitorGroup(mon, mi.rcMonitor);
        m.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        MONITORINFOEXW mx = {};
        mx.cbSize = sizeof(mx);
        GetMonitorInfoW(mon, &mx);
        if (EnumDisplaySettingsW(mx.szDevice, ENUM_CURRENT_SETTINGS, &dm)) {
            m.w = dm.dmPelsWidth;
            m.h = dm.dmPelsHeight;
        } else {
            m.w = mi.rcMonitor.right - mi.rcMonitor.left;
            m.h = mi.rcMonitor.bottom - mi.rcMonitor.top;
        }
        g_sw.mons.push_back(m);
        return TRUE;
    }, 0);
    std::stable_sort(g_sw.mons.begin(), g_sw.mons.end(), [](const SwMon& a, const SwMon& b) { return a.primary > b.primary; });
}

static void PageMonitores(float& y, float x, float w) {
    Heading(y, x, w, L2(L"Monitores", L"Monitors"),
            L2(L"Cada monitor guarda os próprios ajustes, mesmo se você mudar a ordem ou a posição deles.",
               L"Each monitor keeps its own settings, even if you change their order or position."));
    if (g_sw.mons.empty()) LoadSwMonitors();
    int n = 0;
    for (auto& m : g_sw.mons) {
        n++;
        wchar_t title[200];
        std::wstring name = m.who.name.empty() ? std::wstring(L2(L"Monitor ", L"Monitor ")) + std::to_wstring(n) : m.who.name;
        swprintf_s(title, L"%s%s  ·  %d × %d", name.c_str(), m.primary ? L2(L" (principal)", L" (primary)") : L"", m.w, m.h);
        Section(y, x, w, title);
        std::wstring id = m.who.id;
        const wchar_t* group = m.group;
        auto tog = [&](const wchar_t* label, const wchar_t* desc, const wchar_t* field) {
            SwRow r;
            r.label = label;
            r.desc = desc ? desc : L"";
            r.ctl = CtlToggle;
            std::wstring f = field;
            r.on = [id, group, f] { return _wtoi(MonCfg(id, group, f.c_str()).c_str()) != 0; };
            r.flip = [id, group, f] { MonSet(id, f.c_str(), _wtoi(MonCfg(id, group, f.c_str()).c_str()) ? L"0" : L"1"); };
            return r;
        };
        auto seg = [&](const wchar_t* label, const wchar_t* field, std::vector<std::pair<std::wstring, std::wstring>> opts) {
            SwRow r;
            r.label = label;
            r.ctl = CtlSeg;
            r.opts = std::move(opts);
            std::wstring f = field, def = r.opts[0].first;
            r.cur = [id, group, f, def] {
                std::wstring v = MonCfg(id, group, f.c_str());
                return v.empty() ? def : v;
            };
            r.pick = [id, f](const std::wstring& v) { MonSet(id, f.c_str(), v); };
            return r;
        };
        Card({tog(L2(L"Mostrar a dock", L"Show the dock"), nullptr, L"enabled"),
              seg(L2(L"Formato", L"Format"), L"format",
                  {{L"geral", L2(L"Igual ao geral", L"Same as general")}, {L"dock", L2(L"Flutuante", L"Floating")}, {L"fixa", L2(L"Barra inteira", L"Full width")}}),
              seg(L2(L"Esconder sozinha", L"Auto-hide"), L"autoHide",
                  {{L"geral", L2(L"Igual ao geral", L"Same as general")}, {L"sim", L2(L"Sim", L"Yes")}, {L"nao", L2(L"Não", L"No")}}),
              seg(L2(L"Janelas abertas", L"Open windows"), L"windows",
                  {{L"todas", L2(L"Todas", L"All")}, {L"deste", L2(L"Só as deste monitor", L"This monitor's only")}}),
              tog(L2(L"Botão Iniciar", L"Start button"), nullptr, L"showStart"),
              tog(L2(L"Apps fixados", L"Pinned apps"), nullptr, L"showPinned")},
             y, x, w);
    }
}

static void PageSobre(float& y, float x, float w) {
    Heading(y, x, w, L2(L"Sobre", L"About"), nullptr);
    y += 18;
    float h = 100;
    D2D1_RECT_F card = D2D1::RectF(x, y, x + w, y + h);
    g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(card, 12, 12), Br(0xFFFFFF, 0.035f));
    g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x + 0.5f, y + 0.5f, x + w - 0.5f, y + h - 0.5f), 12, 12), Br(0xFFFFFF, 0.06f), 1);
    D2D1_RECT_F tile = D2D1::RectF(x + 22, y + 22, x + 78, y + 78);
    DrawNimbusLogo(g_sw.rt.Get(), tile);
    std::wstring bw = L2(L"Ajuda e sugestões", L"Help and ideas");
    float btnW = Measure(bw, g_sw.fSeg.Get()).width + 28;
    float tx = tile.right + 18, tw = w - (tx - x) - btnW - 40;
    float th = Txt(std::wstring(L"Nimbus Menu ") + kVersion, g_sw.fTitle.Get(), tx, y + 30, tw, kInk);
    Txt(L2(L"Dock e Iniciar pro Windows 11, no Windhawk. Feito por igoredm.", L"A dock and Start for Windows 11, on Windhawk. Made by igoredm."),
        g_sw.fDesc.Get(), tx, y + 30 + th + 3, tw, kSub);
    DrawButton(D2D1::RectF(x + w - 22 - btnW, y + h / 2 - 17, x + w - 22, y + h / 2 + 17), bw,
               [] { ShellExecuteW(nullptr, L"open", kHelpUrl, nullptr, nullptr, SW_SHOWNORMAL); });
    y += h;
    Section(y, x, w, L2(L"Ajustes", L"Settings"));
    bool confirm = g_sw.confirmReset;
    Card({RButtons(L2(L"Voltar ao padrão", L"Back to defaults"),
                   confirm ? L2(L"Clique de novo pra confirmar. Os fixados ficam.", L"Click again to confirm. Pinned apps stay.")
                           : L2(L"Os fixados ficam; só os ajustes voltam.", L"Pinned apps stay; only the settings go back."),
                   {{confirm ? L2(L"Confirmar", L"Confirm") : L2(L"Voltar ao padrão", L"Reset"), [] {
                         if (g_sw.confirmReset) {
                             g_sw.confirmReset = false;
                             ResetCfg();
                         } else {
                             g_sw.confirmReset = true;
                         }
                     }}})},
         y, x, w);
}

// ---- Guided setup: a few questions, each answer applied at once ----
struct WizOpt {
    const wchar_t *pt, *en, *dpt, *den;
    bool rec;
};
struct WizQ {
    const wchar_t *kpt, *ken, *qpt, *qen, *hpt, *hen;
    std::vector<WizOpt> opts;
};
static const std::vector<WizQ>& WizQuestions() {
    static const std::vector<WizQ> q = {
        {L"Visual", L"Look", L"Qual visual você prefere?", L"Which look do you prefer?", L"Vale pra dock, o Iniciar e os painéis.",
         L"It applies to the dock, Start and the panels.",
         {{L"Névoa", L"Névoa", L"Vidro fumê, escuro e suave.", L"Smoked glass, dark and soft.", true},
          {L"Cristal", L"Cristal", L"Vidro claro.", L"Light glass.", false},
          {L"Grafite", L"Grafite", L"Sólido, o mais leve pro PC.", L"Solid, the lightest for the PC.", false},
          {L"Aurora", L"Aurora", L"Vidro com borda colorida.", L"Glass with a colored edge.", false}}},
        {L"Dock", L"Dock", L"Como a dock fica na tela?", L"How does the dock sit on screen?", L"Na aba Dock tem prévia e tamanho dos ícones.",
         L"The Dock tab has a preview and the icon size.",
         {{L"Flutuante", L"Floating", L"Uma pílula solta no centro, como no macOS.", L"A loose pill in the middle, like macOS.", true},
          {L"Barra inteira", L"Full width", L"Cola embaixo de ponta a ponta, como a barra do Windows.", L"Across the bottom, like the Windows taskbar.", false}}},
        {L"Esconder", L"Auto-hide", L"Ela some quando você não usa?", L"Does it hide when you're not using it?",
         L"Escondida, aparece quando o mouse encosta embaixo.", L"Hidden, it shows when the mouse touches the bottom.",
         {{L"Sim, esconder sozinha", L"Yes, hide by itself", L"Mais espaço pras janelas.", L"More room for windows.", true},
          {L"Não, sempre à mostra", L"No, always visible", L"Sempre visível.", L"Always there.", false}}},
        {L"Animação", L"Animation", L"O que acontece ao passar o mouse nos ícones?", L"What happens when the mouse goes over the icons?", L"",
         L"",
         {{L"Zoom", L"Zoom", L"A onda do macOS.", L"The macOS wave.", true},
          {L"Elevar", L"Lift", L"Só o ícone sobe.", L"Only the icon rises.", false},
          {L"Foco", L"Focus", L"Os outros ícones apagam.", L"The other icons dim.", false},
          {L"Nada", L"Nothing", L"Quietinho.", L"Quiet.", false}}},
        {L"Tecla Windows", L"Windows key", L"A tecla Windows abre qual Iniciar?", L"Which Start does the Windows key open?",
         L"Ctrl+Esc sempre abre o do Windows.", L"Ctrl+Esc always opens Windows' own.",
         {{L"O do Nimbus", L"Nimbus'", L"Fixados, todos os apps, baixados e a busca esperta.", L"Pinned, all apps, downloads and the smart search.", true},
          {L"O do Windows", L"Windows'", L"O Nimbus fica só na dock.", L"Nimbus stays in the dock only.", false}}},
        {L"Área de status", L"Status area", L"Onde ficam relógio, volume, rede e o resto?", L"Where do the clock, volume, network and the rest go?",
         L"Depois, cada item pode ir pra um ou vários lugares nos Ajustes.", L"Later, each item can go to one or several places in Settings.",
         {{L"Na dock", L"In the dock", L"Tudo à vista, como na barra do Windows.", L"Everything in sight, like the Windows taskbar.", false},
          {L"Numa central", L"In a control center", L"A dock fica limpa: o relógio e um botão que abre o resto.",
           L"A clean dock: the clock and one button that opens the rest.", false},
          {L"Misto", L"Mixed", L"Relógio, volume, rede, bateria, bandeja e avisos na dock; o resto na central.",
           L"Clock, volume, network, battery, tray and notifications in the dock; the rest in the center.", true}}},
        {L"Bandeja", L"Tray", L"E os ícones em segundo plano (bandeja)?", L"And the background icons (tray)?", L"Discord, Steam, antivírus…",
         L"Discord, Steam, antivirus…",
         {{L"Num botão", L"In a button", L"Um ícone que abre e fecha os outros.", L"One icon that opens and closes the others.", true},
          {L"Lado a lado", L"Side by side", L"Todos à mostra, rolando com a roda.", L"All shown, scrolling with the wheel.", false},
          {L"Grade", L"Grid", L"Um painel com os ícones em grade.", L"A panel with the icons in a grid.", false},
          {L"Escondida", L"Hidden", L"Não aparece.", L"Doesn't show.", false}}},
        {L"Notificações", L"Notifications", L"Quer os avisos com o visual do Nimbus?", L"Do you want notifications in Nimbus' look?",
         L"Os do Windows continuam guardados na central dele.", L"Windows' own stay in its center.",
         {{L"Sim, com som", L"Yes, with sound", L"Cartões do Nimbus, o som Pingo nos avisos e Bolha nos lembretes.", L"Nimbus cards, the Drop sound for notifications and Bubble for reminders.", true},
          {L"Sim, sem som", L"Yes, silent", L"Só o cartão, nem os lembretes tocam.", L"Just the card; reminders are silent too.", false},
          {L"Não", L"No", L"Fica tudo com o Windows.", L"Everything stays with Windows.", false}}},
        {L"Player", L"Player", L"O que está tocando aparece na dock?", L"Does what's playing show in the dock?", L"Spotify, YouTube, qualquer player.",
         L"Spotify, YouTube, any player.",
         {{L"Capa e título", L"Cover and title", L"A música atual na dock.", L"The current song in the dock.", true},
          {L"Barras", L"Bars", L"Barrinhas que dançam com o som.", L"Little bars dancing with the sound.", false},
          {L"Só o ícone", L"Just the icon", L"Discreto.", L"Discreet.", false},
          {L"Não", L"No", L"Sem player.", L"No player.", false}}},
    };
    return q;
}

static void WizApply(int q, int o) {
    auto set = [](const wchar_t* k, const wchar_t* v) {
        g_cfg[k] = v;
        WriteVal(UserPrefix() + L"cfg." + k, v);
    };
    switch (q) {
        case 0: set(L"style", o == 0 ? L"nevoa" : o == 1 ? L"cristal" : o == 2 ? L"grafite" : L"aurora"); break;
        case 1: set(L"format", o == 0 ? L"dock" : L"fixa"); break;
        case 2: set(L"autoHide", o == 0 ? L"1" : L"0"); break;
        case 3: set(L"animation", o == 0 ? L"zoom" : o == 1 ? L"elevar" : o == 2 ? L"foco" : L"nenhuma"); break;
        case 4:
            set(L"winKeyMode", o == 0 ? L"nevoa" : L"windows");
            set(L"startMenu", o == 0 ? L"nevoa" : L"windows");
            break;
        case 5: {
            const wchar_t* core[] = {L"statusClock", L"statusVolume", L"statusNetwork", L"statusBattery", L"statusKeyboard"};
            const wchar_t* rest[] = {L"notesPlace", L"shelfPlace", L"dndPlace", L"btPlace", L"nightPlace", L"shortPlace", L"lightPlace", L"powerPlace"};
            if (o == 0) {
                for (auto k : core) set(k, L"dock");
                for (auto k : rest) set(k, L"dock");
                set(L"trayPlace", L"dock");
                set(L"notifPlace", L"dock");
            } else if (o == 1) {
                for (auto k : core) set(k, L"central");
                set(L"statusClock", L"dock");
                for (auto k : rest) set(k, L"central");
                set(L"trayPlace", L"central");
                set(L"notifPlace", L"central");
            } else {
                for (auto k : core) set(k, L"dock");
                set(L"statusKeyboard", L"central");
                for (auto k : rest) set(k, L"central");
                set(L"trayPlace", L"dock");
                set(L"notifPlace", L"dock");
            }
            break;
        }
        case 6:
            set(L"trayStyle", o == 0 ? L"botao" : o == 1 ? L"lado" : o == 2 ? L"grade" : L"off");
            break;
        case 7:
            set(L"notifShow", o == 2 ? L"0" : L"1");
            set(L"notifSound", o == 0 ? L"pingo" : L"0");
            set(L"remSound", o == 0 ? L"bolha" : L"0");
            break;
        case 8:
            if (o == 3) set(L"playerPlace", L"off");
            else {
                if (CfgStr(L"playerPlace", L"dock") == L"off") set(L"playerPlace", L"dock");
                set(L"playerDock", o == 0 ? L"ambos" : o == 1 ? L"espectro" : L"icone");
            }
            break;
    }
    ApplySoon();  // (the dock changes behind the window)
}

// The option that matches the current settings (-1: none of them).
static int WizCurrent(int q) {
    switch (q) {
        case 0: {
            std::wstring v = CfgStr(L"style", L"nevoa");
            return v == L"nevoa" ? 0 : v == L"cristal" ? 1 : v == L"grafite" ? 2 : v == L"aurora" ? 3 : -1;
        }
        case 1: return CfgStr(L"format", L"dock") == L"fixa" ? 1 : 0;
        case 2: return CfgInt(L"autoHide") ? 0 : 1;
        case 3: {
            std::wstring v = CfgStr(L"animation", L"zoom");
            return v == L"zoom" ? 0 : v == L"elevar" ? 1 : v == L"foco" ? 2 : 3;
        }
        case 4: return CfgStr(L"winKeyMode", L"nevoa") == L"windows" ? 1 : 0;
        case 6: {
            std::wstring v = CfgStr(L"trayStyle", L"botao");
            return v == L"botao" ? 0 : v == L"lado" ? 1 : v == L"grade" ? 2 : v == L"off" ? 3 : -1;
        }
        case 7: return !CfgInt(L"notifShow") ? 2 : SoundValue(CfgStr(L"notifSound")) ? 0 : 1;
        case 8: {
            if (CfgStr(L"playerPlace", L"dock") == L"off") return 3;
            std::wstring v = CfgStr(L"playerDock", L"ambos");
            return v == L"espectro" ? 1 : v == L"icone" ? 2 : v == L"ambos" ? 0 : -1;
        }
    }
    return -1;
}

static void WizStart() {
    g_sw.wizStep = 0;
    g_sw.wizAns.assign(WizQuestions().size(), -1);
    Wh_SetStringValue((UserPrefix() + L"wizard.done").c_str(), L"1");  // (it opens by itself only once)
}

static void PageComecar(float& y, float x, float w) {
    const auto& qs = WizQuestions();
    if (g_sw.wizAns.size() != qs.size()) g_sw.wizAns.assign(qs.size(), -1);
    int n = (int)qs.size(), i = g_sw.wizStep;
    auto later = [&] {
        // (said plainly: nothing here is final)
        y += 18;
        Txt(L2(L"Nada aqui é definitivo: tudo pode ser mudado depois nas outras abas dos Ajustes, do jeito que você preferir.",
               L"Nothing here is final: everything can be changed later in the other Settings tabs, the way you prefer."),
            g_sw.fDesc.Get(), x, y, w, kMuted);
        y += 20;
    };
    if (i >= n) {
        Heading(y, x, w, L2(L"O Nimbus ficou do seu jeito", L"Nimbus is set up your way"),
                L2(L"Pode mudar qualquer coisa depois, conforme preferir, nas outras abas, ou responder tudo de novo.",
                   L"You can change anything later, as you like, in the other tabs, or answer again."));
        y += 18;
        std::vector<SwRow> rows;
        for (int k = 0; k < n; k++) {
            SwRow r;
            r.label = L2(qs[k].kpt, qs[k].ken);
            int a = g_sw.wizAns[k];
            r.desc = a >= 0 ? std::wstring(L2(qs[k].opts[a].pt, qs[k].opts[a].en)) : std::wstring(L2(L"como estava", L"unchanged"));
            rows.push_back(r);
        }
        Card(rows, y, x, w);
        y += 22;
        DrawButton(D2D1::RectF(x, y, x + 150, y + 36), L2(L"Recomeçar", L"Start over"), [] {
            WizStart();
            SwInvalidate();
        });
        DrawButton(D2D1::RectF(x + w - 170, y, x + w, y + 36), L2(L"Ir pros Ajustes", L"Go to Settings"), [] { SwSetPage(PgGeral); }, true);
        y += 36;
        return;
    }
    const WizQ& q = qs[i];
    wchar_t step[64];
    swprintf_s(step, L2(L"Pergunta %d de %d", L"Question %d of %d"), i + 1, n);
    Txt(step, g_sw.fDesc.Get(), x, y, w, kMuted);
    y += 22;
    float dw = std::min(30.0f, (w - (n - 1) * 6) / n);
    for (int k = 0; k < n; k++)
        g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x + k * (dw + 6), y, x + k * (dw + 6) + dw, y + 4), 2, 2),
                                      Br(k <= i ? kInk : 0xFFFFFF, k <= i ? 1 : 0.12f));
    y += 26;
    Heading(y, x, w, L2(q.qpt, q.qen), *q.hpt ? L2(q.hpt, q.hen) : nullptr);
    y += 24;
    int cur = g_sw.wizAns[i] >= 0 ? g_sw.wizAns[i] : WizCurrent(i);
    float gap = 12, cw = (w - gap) / 2, ch = 86;
    for (int k = 0; k < (int)q.opts.size(); k++) {
        const WizOpt& o = q.opts[k];
        float ox = x + (k % 2) * (cw + gap), oy = y + (k / 2) * (ch + gap);
        D2D1_RECT_F r = D2D1::RectF(ox, oy, ox + cw, oy + ch);
        bool hot = Hot(r), on = k == cur;
        g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(r, 14, 14), Br(0xFFFFFF, hot ? 0.08f : on ? 0.07f : 0.04f));
        g_sw.rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), 14, 14),
                                      Br(on ? kInk : 0xFFFFFF, on ? 0.9f : 0.08f), on ? 1.5f : 1);
        float tx = r.left + 18;
        if (i == 0) {  // the looks: a little glass bar of each
            D2D1_RECT_F ill = D2D1::RectF(r.left + 14, r.top + 16, r.left + 98, r.bottom - 16);
            Wallpaper(ill, 9);
            static const wchar_t* ids[] = {L"nevoa", L"cristal", L"grafite", L"aurora"};
            Theme t = MakeTheme(ids[k], CfgInt(L"tintOpacity") / 100.0f);
            D2D1_RECT_F bar = D2D1::RectF(ill.left + 10, ill.bottom - 22, ill.right - 10, ill.bottom - 8);
            GlassBar(bar, 7, t, t.tint.a);
            tx = ill.right + 14;
        }
        float th = Txt(L2(o.pt, o.en), g_sw.fTitle.Get(), tx, r.top + 16, r.right - tx - 110, kInk);
        if (o.rec) {
            std::wstring rec = L2(L"recomendado", L"recommended");
            float rw = Measure(rec, g_sw.fSmall.Get()).width + 14;
            D2D1_RECT_F pr = D2D1::RectF(r.right - 14 - rw, r.top + 16, r.right - 14, r.top + 36);
            g_sw.rt->FillRoundedRectangle(D2D1::RoundedRect(pr, 10, 10), Br(kInk, 0.9f));
            TxtC(rec, g_sw.fSmall.Get(), pr, kBg);
        }
        Txt(L2(o.dpt, o.den), g_sw.fDesc.Get(), tx, r.top + 16 + th + 4, r.right - tx - 16, kSub);
        AddHit(r, [i, k] {
            g_sw.wizAns[i] = k;
            WizApply(i, k);
            g_sw.wizStep = i + 1;
            g_sw.scroll = g_sw.scrollTo = 0;
            SwInvalidate();
        });
    }
    y += ((int)q.opts.size() + 1) / 2 * (ch + gap);
    later();
    y += 10;
    DrawButton(D2D1::RectF(x, y, x + 110, y + 34), L2(L"‹ Voltar", L"‹ Back"), [i] {
        g_sw.wizStep = std::max(0, i - 1);
        SwInvalidate();
    });
    DrawButton(D2D1::RectF(x + w - 130, y, x + w, y + 34), L2(L"Pular esta", L"Skip this"), [i] {
        g_sw.wizStep = i + 1;
        SwInvalidate();
    });
    y += 34;
}

// ---- Frame -----------------------------------------------------------------

static const float kSideW = 236, kCapH = 36, kCapBtnW = 46;

struct SwNav {
    const wchar_t* pt;
    const wchar_t* en;
    const wchar_t* icon;
};
static const SwNav kNav[PgCount] = {
    {L"Geral", L"General", L"M4 7h9 M17 7h3 M17 7a2 2 0 1 1-4 0a2 2 0 1 1 4 0 M4 17h3 M11 17h9 M11 17a2 2 0 1 1-4 0a2 2 0 1 1 4 0"},
    {L"Dock", L"Dock", kGlyphDock},
    {L"Iniciar", L"Start", L"M4 4h7v7H4z M13 4h7v7h-7z M4 13h7v7H4z M13 13h7v7h-7z"},
    {L"Área de status", L"Status area", L"M4 9h4l5-4v14l-5-4H4z M16.5 8.5a5 5 0 0 1 0 7 M19 6a8.5 8.5 0 0 1 0 12"},
    {L"Monitores", L"Monitors", L"M3 4h18v12H3z M8 20h8 M12 16v4"},
    {L"Sobre", L"About", L"M12 3a9 9 0 1 1 0 18a9 9 0 1 1 0-18 M12 11v6 M12 7.5v.5"},
    {L"Começar", L"Get started", L"M12 3l2.2 5.6L20 9.3l-4.4 3.9 1.3 5.8L12 16l-4.9 3 1.3-5.8L4 9.3l5.8-.7z"},
};

static void SwSetPage(int p) {
    g_sw.page = p;
    g_sw.scroll = g_sw.scrollTo = 0;
    g_sw.confirmReset = false;
    if (p == PgMonitores) LoadSwMonitors();
    SwInvalidate();
}

static void SwCreateFonts() {
    if (g_sw.fH1) return;
    auto mk = [](const wchar_t* fam, float size, DWRITE_FONT_WEIGHT wt, ComPtr<IDWriteTextFormat>& f, bool wrap = true) {
        g_dw->CreateTextFormat(fam, nullptr, wt, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, g_locale, &f);
        if (f && !wrap) f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    };
    mk(L"Segoe UI Variable Display", 26, DWRITE_FONT_WEIGHT_SEMI_BOLD, g_sw.fH1);
    mk(L"Segoe UI Variable Text", 13.5f, DWRITE_FONT_WEIGHT_NORMAL, g_sw.fSub);
    mk(L"Segoe UI Variable Text", 14, DWRITE_FONT_WEIGHT_NORMAL, g_sw.fLbl);
    mk(L"Segoe UI Variable Text", 12.5f, DWRITE_FONT_WEIGHT_NORMAL, g_sw.fDesc);
    mk(L"Segoe UI Variable Text", 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, g_sw.fSec);
    mk(L"Segoe UI Variable Text", 13, DWRITE_FONT_WEIGHT_NORMAL, g_sw.fSeg, false);
    mk(L"Segoe UI Variable Text", 13.5f, DWRITE_FONT_WEIGHT_NORMAL, g_sw.fNav, false);
    mk(L"Segoe UI Variable Text", 11.5f, DWRITE_FONT_WEIGHT_NORMAL, g_sw.fSmall, false);
    mk(L"Segoe UI Variable Text", 14, DWRITE_FONT_WEIGHT_SEMI_BOLD, g_sw.fBrand, false);
    mk(L"Segoe UI Variable Display", 17, DWRITE_FONT_WEIGHT_SEMI_BOLD, g_sw.fTitle);
    mk(L"Segoe Fluent Icons", 10, DWRITE_FONT_WEIGHT_NORMAL, g_sw.fGlyph, false);
    D2D1_STROKE_STYLE_PROPERTIES sp = D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                                  D2D1_LINE_JOIN_ROUND);
    g_d2d->CreateStrokeStyle(sp, nullptr, 0, &g_sw.round);
}

static void SwRender() {
    if (!g_sw.hwnd) return;
    RECT rc;
    GetClientRect(g_sw.hwnd, &rc);
    D2D1_SIZE_U px = D2D1::SizeU(std::max(1L, rc.right), std::max(1L, rc.bottom));
    if (!g_sw.rt) {
        if (FAILED(g_d2d->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(g_sw.hwnd, px),
                                                 &g_sw.rt)))
            return;
        g_sw.rt.As(&g_sw.dc);
        g_sw.rt->CreateSolidColorBrush(D2D1::ColorF(0), &g_sw.br);
        g_sw.bmps.clear();
    } else {
        D2D1_SIZE_U cur = g_sw.rt->GetPixelSize();
        if (cur.width != px.width || cur.height != px.height) g_sw.rt->Resize(px);
    }
    g_sw.rt->SetDpi(96 * g_sw.scale, 96 * g_sw.scale);
    SwCreateFonts();
    D2D1_SIZE_F sz = g_sw.rt->GetSize();
    float W = sz.width, H = sz.height;
    g_sw.hits.clear();
    g_sw.preview = {};

    auto* rt = g_sw.rt.Get();
    rt->BeginDraw();
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    rt->Clear(D2D1::ColorF(kBg));

    // Sidebar
    rt->FillRectangle(D2D1::RectF(0, 0, kSideW, H), Br(kSide));
    rt->FillRectangle(D2D1::RectF(kSideW - 1, 0, kSideW, H), Br(0xFFFFFF, 0.06f));
    D2D1_RECT_F brand = D2D1::RectF(20, 22, 48, 50);
    DrawNimbusLogo(g_sw.rt.Get(), D2D1::RectF(brand.left - 2, brand.top - 2, brand.right + 2, brand.bottom + 2));
    Txt(L"Nimbus", g_sw.fBrand.Get(), 58, 20, 160, kInk);
    Txt(L2(L"Ajustes", L"Settings"), g_sw.fSmall.Get(), 58, 38, 160, kSub);
    float ny = 74;
    static const int kNavOrder[PgCount] = {PgComecar, PgGeral, PgDock, PgIniciar, PgStatus, PgMonitores, PgSobre};
    for (int oi = 0; oi < PgCount; oi++) {
        int i = kNavOrder[oi];
        D2D1_RECT_F r = D2D1::RectF(12, ny, kSideW - 12, ny + 38);
        bool on = g_sw.page == i, hot = Hot(r);
        if (on || hot) rt->FillRoundedRectangle(D2D1::RoundedRect(r, 8, 8), Br(0xFFFFFF, on ? 0.09f : 0.045f));
        if (on) rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left, r.top + 11, r.left + 3, r.bottom - 11), 1.5f, 1.5f), Br(kAccent));
        UINT32 c = on ? 0xFFFFFF : kIdle;
        Icon(kNav[i].icon, r.left + 12, r.top + 10, 18, c);
        Txt(L2(kNav[i].pt, kNav[i].en), g_sw.fNav.Get(), r.left + 42, r.top + 9, r.right - r.left - 50, c);
        int page = i;
        if (!on) AddHit(r, [page] { SwSetPage(page); });
        ny += 42;
    }
    Txt(std::wstring(L"Nimbus Menu ") + kVersion, g_sw.fSmall.Get(), 24, H - 34, kSideW - 30, kMuted);

    // Page (scrolls)
    float mainL = kSideW;
    float cw = std::min(720.0f, W - mainL - 80);
    float x = mainL + std::max(40.0f, (W - mainL - cw) / 2 > 40 ? 40.0f : (W - mainL - cw) / 2);
    g_sw.viewH = H;
    rt->PushAxisAlignedClip(D2D1::RectF(mainL, 0, W, H), D2D1_ANTIALIAS_MODE_ALIASED);
    float y = 40 - g_sw.scroll;
    switch (g_sw.page) {
        case PgGeral: PageGeral(y, x, cw); break;
        case PgDock: PageDock(y, x, cw); break;
        case PgIniciar: PageIniciar(y, x, cw); break;
        case PgStatus: PageStatus(y, x, cw); break;
        case PgMonitores: PageMonitores(y, x, cw); break;
        case PgSobre: PageSobre(y, x, cw); break;
        case PgComecar: PageComecar(y, x, cw); break;
    }
    g_sw.contentH = y + g_sw.scroll + 40;
    // fade under the caption buttons once scrolled
    if (g_sw.scroll > 1) {
        D2D1_GRADIENT_STOP stops[] = {{0, D2D1::ColorF(kBg, 1)}, {1, D2D1::ColorF(kBg, 0)}};
        ComPtr<ID2D1GradientStopCollection> sc;
        ComPtr<ID2D1LinearGradientBrush> lin;
        rt->CreateGradientStopCollection(stops, 2, &sc);
        if (sc) rt->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, 40)), sc.Get(), &lin);
        if (lin) rt->FillRectangle(D2D1::RectF(mainL, 0, W, 40), lin.Get());
    }
    float maxScroll = std::max(0.0f, g_sw.contentH - H);
    if (maxScroll > 0) {
        float th = std::max(40.0f, H * H / g_sw.contentH), ty = (H - th) * (g_sw.scroll / maxScroll);
        rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(W - 7, ty + 4, W - 4, ty + th - 4), 1.5f, 1.5f), Br(0xFFFFFF, 0.18f));
    }
    rt->PopAxisAlignedClip();

    // Caption buttons
    bool zoomed = IsZoomed(g_sw.hwnd);
    struct {
        wchar_t glyph;
        std::function<void()> fn;
        bool close;
    } caps[] = {
        {0xE921, [] { ShowWindow(g_sw.hwnd, SW_MINIMIZE); }, false},
        {(wchar_t)(zoomed ? 0xE923 : 0xE922), [zoomed] { ShowWindow(g_sw.hwnd, zoomed ? SW_RESTORE : SW_MAXIMIZE); }, false},
        {0xE8BB, [] { PostMessageW(g_sw.hwnd, WM_CLOSE, 0, 0); }, true},
    };
    for (int i = 0; i < 3; i++) {
        D2D1_RECT_F r = D2D1::RectF(W - kCapBtnW * (3 - i), 0, W - kCapBtnW * (2 - i), kCapH);
        bool hot = Hot(r);
        if (hot) rt->FillRectangle(r, caps[i].close ? Br(0xC42B1C) : Br(0xFFFFFF, 0.08f));
        Glyph(caps[i].glyph, r, hot && caps[i].close ? 0xFFFFFF : kInk, hot ? 1 : 0.85f);
        AddHit(r, caps[i].fn);
    }

    HRESULT hr = rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        g_sw.bmps.clear();
        g_sw.br.Reset();
        g_sw.dc.Reset();
        g_sw.rt.Reset();
    }
    // the content may have shrunk (another page, a smaller window)
    float maxS = std::max(0.0f, g_sw.contentH - H);
    if (g_sw.scrollTo > maxS || g_sw.scroll > maxS) {
        g_sw.scrollTo = std::min(g_sw.scrollTo, maxS);
        g_sw.scroll = std::min(g_sw.scroll, maxS);
        SwInvalidate();
    }
}

static int SwHitAt(D2D1_POINT_2F p) {
    for (int i = (int)g_sw.hits.size() - 1; i >= 0; i--)
        if (InRect(g_sw.hits[i].r, p)) return i;
    return -1;
}

static void SwDragTo(float mx) {
    SwHit& h = g_sw.drag;
    float t = h.tw > 0 ? (mx - h.tx) / h.tw : 0;
    t = std::max(0.0f, std::min(1.0f, t));
    int v = h.min + (int)std::lround(t * (h.max - h.min) / h.step) * h.step;
    v = Clamp(v, h.min, h.max);
    auto it = g_sw.pending.find(h.key);
    if (it == g_sw.pending.end() || it->second != v) {
        g_sw.pending[h.key] = v;
        SwInvalidate();
    }
}

static void SwEndDrag() {
    if (!g_sw.dragging) return;
    g_sw.dragging = false;
    auto it = g_sw.pending.find(g_sw.drag.key);
    if (it != g_sw.pending.end()) {
        int v = it->second;
        if (v != CfgInt(g_sw.drag.key.c_str()) && g_sw.drag.commit) g_sw.drag.commit(v);
        g_sw.pending.erase(it);
    }
    SwInvalidate();
}

static void SwScrollBy(float dy) {
    float maxS = std::max(0.0f, g_sw.contentH - g_sw.viewH);
    g_sw.scrollTo = std::max(0.0f, std::min(maxS, g_sw.scrollTo + dy));
    SetTimer(g_sw.hwnd, TIMER_SW_SCROLL, 10, nullptr);
}

static D2D1_POINT_2F SwPoint(LPARAM lp) {
    return D2D1::Point2F(GET_X_LPARAM(lp) / g_sw.scale, GET_Y_LPARAM(lp) / g_sw.scale);
}

LRESULT CALLBACK SettingsProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_NCCALCSIZE:
            if (wp) {
                // No Windows frame: the window draws its own caption. Maximized, keep it inside the screen.
                if (IsZoomed(hwnd)) {
                    auto* p = (NCCALCSIZE_PARAMS*)lp;
                    UINT dpi = GetDpiForWindow(hwnd);
                    int f = GetSystemMetricsForDpi(SM_CXFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
                    p->rgrc[0].left += f;
                    p->rgrc[0].top += f;
                    p->rgrc[0].right -= f;
                    p->rgrc[0].bottom -= f;
                }
                return 0;
            }
            break;
        case WM_NCHITTEST: {
            POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(hwnd, &pt);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (!IsZoomed(hwnd)) {
                int b = (int)(6 * g_sw.scale);
                bool l = pt.x < b, r = pt.x >= rc.right - b, t = pt.y < b, bt = pt.y >= rc.bottom - b;
                if (t && l) return HTTOPLEFT;
                if (t && r) return HTTOPRIGHT;
                if (bt && l) return HTBOTTOMLEFT;
                if (bt && r) return HTBOTTOMRIGHT;
                if (l) return HTLEFT;
                if (r) return HTRIGHT;
                if (t) return HTTOP;
                if (bt) return HTBOTTOM;
            }
            D2D1_POINT_2F p = D2D1::Point2F(pt.x / g_sw.scale, pt.y / g_sw.scale);
            if (SwHitAt(p) >= 0) return HTCLIENT;
            if (p.y < 40 || (p.x < kSideW && p.y < 64)) return HTCAPTION;
            return HTCLIENT;
        }
        case WM_GETMINMAXINFO: {
            auto* mm = (MINMAXINFO*)lp;
            mm->ptMinTrackSize.x = (LONG)(760 * g_sw.scale);
            mm->ptMinTrackSize.y = (LONG)(520 * g_sw.scale);
            return 0;
        }
        case WM_DPICHANGED: {
            g_sw.scale = HIWORD(wp) / 96.0f;
            auto* r = (RECT*)lp;
            SetWindowPos(hwnd, nullptr, r->left, r->top, r->right - r->left, r->bottom - r->top, SWP_NOZORDER | SWP_NOACTIVATE);
            SwInvalidate();
            return 0;
        }
        case WM_SIZE:
            SwInvalidate();
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            SwRender();
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                int h = SwHitAt(D2D1::Point2F(pt.x / g_sw.scale, pt.y / g_sw.scale));
                SetCursor(LoadCursorW(nullptr, h >= 0 && pt.y / g_sw.scale >= kCapH ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_MOUSEMOVE: {
            D2D1_POINT_2F p = SwPoint(lp);
            if (!g_sw.mouseIn) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                g_sw.mouseIn = true;
            }
            int before = SwHitAt(g_sw.mouse), now = SwHitAt(p);
            bool wasPreview = g_sw.inPreview;
            g_sw.mouse = p;
            g_sw.inPreview = InRect(g_sw.preview, p);
            if (g_sw.dragging) SwDragTo(p.x);
            else if (before != now || wasPreview || g_sw.inPreview) SwInvalidate();
            return 0;
        }
        case WM_MOUSELEAVE:
            g_sw.mouseIn = false;
            g_sw.inPreview = false;
            g_sw.mouse = D2D1::Point2F(-1, -1);
            SwInvalidate();
            return 0;
        case WM_LBUTTONDOWN: {
            D2D1_POINT_2F p = SwPoint(lp);
            g_sw.mouse = p;
            int h = SwHitAt(p);
            SetCapture(hwnd);
            if (h >= 0 && g_sw.hits[h].slider) {
                g_sw.dragging = true;
                g_sw.drag = g_sw.hits[h];
                SwDragTo(p.x);
            } else {
                g_sw.pressed = h;
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            D2D1_POINT_2F p = SwPoint(lp);
            if (g_sw.dragging) {
                SwEndDrag();
            } else {
                int h = SwHitAt(p);
                int pressed = g_sw.pressed;
                g_sw.pressed = -1;
                if (h >= 0 && h == pressed && g_sw.hits[h].click) {
                    auto fn = g_sw.hits[h].click;  // (the hits are rebuilt on the next paint)
                    bool wasConfirm = g_sw.confirmReset;
                    fn();
                    if (wasConfirm && g_sw.confirmReset) g_sw.confirmReset = false;  // any other click cancels it
                    SwInvalidate();
                }
            }
            if (GetCapture() == hwnd) ReleaseCapture();
            return 0;
        }
        case WM_LBUTTONDBLCLK:
            return 0;
        case WM_CAPTURECHANGED:
            if (g_sw.dragging) SwEndDrag();
            return 0;
        case WM_MOUSEWHEEL:
            SwScrollBy(-GET_WHEEL_DELTA_WPARAM(wp) / 120.0f * 80);
            return 0;
        case WM_KEYDOWN:
            switch (wp) {
                case VK_DOWN: SwScrollBy(48); return 0;
                case VK_UP: SwScrollBy(-48); return 0;
                case VK_NEXT: SwScrollBy(g_sw.viewH * 0.85f); return 0;
                case VK_PRIOR: SwScrollBy(-g_sw.viewH * 0.85f); return 0;
                case VK_HOME: SwScrollBy(-1e6f); return 0;
                case VK_END: SwScrollBy(1e6f); return 0;
                case VK_TAB:
                    SwSetPage((g_sw.page + (GetKeyState(VK_SHIFT) < 0 ? PgCount - 1 : 1)) % PgCount);
                    return 0;
            }
            break;
        case WM_TIMER:
            if (wp == TIMER_SW_SCROLL) {
                float d = g_sw.scrollTo - g_sw.scroll;
                if (fabsf(d) < 0.5f) {
                    g_sw.scroll = g_sw.scrollTo;
                    KillTimer(hwnd, TIMER_SW_SCROLL);
                } else {
                    g_sw.scroll += d * 0.28f;
                }
                SwInvalidate();
            }
            return 0;
        case WM_DISPLAYCHANGE:
            if (g_sw.page == PgMonitores) {
                LoadSwMonitors();
                SwInvalidate();
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY: {
            SwEndDrag();
            ComPtr<IPropertyStore> ps;
            if (SUCCEEDED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&ps)))) {
                PROPVARIANT pv;
                PropVariantInit(&pv);
                ps->SetValue(kPkeyAumid, pv);
                ps->Commit();
            }
            g_sw.bmps.clear();
            g_sw.br.Reset();
            g_sw.dc.Reset();
            g_sw.rt.Reset();
            g_sw.hits.clear();
            g_sw.hwnd = nullptr;
            ScheduleRebuild();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void OpenSettings() {
    if (g_sw.hwnd) {
        if (IsIconic(g_sw.hwnd)) ShowWindow(g_sw.hwnd, SW_RESTORE);
        SetForegroundWindow(g_sw.hwnd);
        return;
    }
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    UINT dx = 96, dy = 96;
    GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy);
    g_sw.scale = dx / 96.0f;
    RECT wa = mi.rcWork;
    int w = std::min((int)(1000 * g_sw.scale), (int)((wa.right - wa.left) * 0.92f));
    int h = std::min((int)(700 * g_sw.scale), (int)((wa.bottom - wa.top) * 0.9f));
    int x = wa.left + (wa.right - wa.left - w) / 2, y = wa.top + (wa.bottom - wa.top - h) / 2;
    g_sw.page = PgGeral;
    g_sw.scroll = g_sw.scrollTo = 0;
    g_sw.confirmReset = false;
    g_sw.hwnd = CreateWindowExW(WS_EX_APPWINDOW, L"NimbusMenu_Settings", SettingsTitle(), WS_OVERLAPPEDWINDOW, x, y, w, h,
                                nullptr, nullptr, g_module, nullptr);
    if (!g_sw.hwnd) return;
    g_sw.scale = GetDpiForWindow(g_sw.hwnd) / 96.0f;
    // Its own app id, so the dock shows it with the Nimbus icon (see RequestIcon).
    ComPtr<IPropertyStore> ps;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(g_sw.hwnd, IID_PPV_ARGS(&ps)))) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        pv.vt = VT_LPWSTR;
        pv.pwszVal = (LPWSTR)kSettingsAumid;
        ps->SetValue(kPkeyAumid, pv);
        ps->Commit();
    }
    BOOL dark = TRUE;
    DwmSetWindowAttribute(g_sw.hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
    DWORD corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(g_sw.hwnd, 33, &corner, sizeof(corner));
    COLORREF border = RGB(44, 44, 49);
    DwmSetWindowAttribute(g_sw.hwnd, 34, &border, sizeof(border));
    MARGINS m = {0, 0, 1, 0};
    DwmExtendFrameIntoClientArea(g_sw.hwnd, &m);
    if (!g_sw.iconBig) {
        g_sw.iconBig = IconFromBitmap(DrawNimbusIcon(GetSystemMetrics(SM_CXICON)), GetSystemMetrics(SM_CXICON));
        g_sw.iconSmall = IconFromBitmap(DrawNimbusIcon(GetSystemMetrics(SM_CXSMICON)), GetSystemMetrics(SM_CXSMICON));
    }
    SendMessageW(g_sw.hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_sw.iconBig);
    SendMessageW(g_sw.hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_sw.iconSmall);
    SetWindowPos(g_sw.hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(g_sw.hwnd, SW_SHOW);
    SetForegroundWindow(g_sw.hwnd);
    ScheduleRebuild();
}

static void FreeSettingsWin() {
    if (g_sw.hwnd) DestroyWindow(g_sw.hwnd);
    g_sw.paths.clear();
    g_sw.round.Reset();
    for (ComPtr<IDWriteTextFormat>* f : {std::addressof(g_sw.fH1), std::addressof(g_sw.fSub), std::addressof(g_sw.fLbl),
                                         std::addressof(g_sw.fDesc), std::addressof(g_sw.fSec), std::addressof(g_sw.fSeg),
                                         std::addressof(g_sw.fNav), std::addressof(g_sw.fSmall), std::addressof(g_sw.fBrand),
                                         std::addressof(g_sw.fGlyph), std::addressof(g_sw.fTitle)})
        f->Reset();
    if (g_sw.iconBig) DestroyIcon(g_sw.iconBig);
    if (g_sw.iconSmall) DestroyIcon(g_sw.iconSmall);
    g_sw.iconBig = g_sw.iconSmall = nullptr;
}

// Shell_NotifyIconGetRect (asked through Explorer): the icon's place in the dock, as 4 shorts
// (left, top, right, bottom); 0 when it isn't in a dock. A hidden one answers with its button or strip.
static LRESULT TrayRectOf(const TrayRectWire& q) {
    static const GUID none = {};
    bool byGuid = !IsEqualGUID(q.guid, none);
    int ti = TrayFind(WireHwnd(q.hwnd), q.uid, byGuid ? &q.guid : nullptr);
    if (ti < 0 && byGuid) ti = TrayFind(WireHwnd(q.hwnd), q.uid, nullptr);
    if (ti < 0) return 0;
    std::vector<int> strip = TrayStripList();
    for (auto* d : g_docks) {
        float top = d->rcMon.bottom - d->ch + d->pill.top, bottom = d->rcMon.bottom - d->ch + d->pill.bottom;
        float l = -1, r = -1;
        for (auto& it : d->stat) {
            if (it.id == StTrayIcon && it.sub == ti) {
                l = it.x;
                r = it.x + it.w;
            } else if (it.id == StTrayStrip) {
                auto f = std::find(strip.begin(), strip.end(), ti);
                float x = f == strip.end() ? it.x : it.x + ((f - strip.begin()) - d->trayPos) * TraySlot(*d);
                l = std::clamp(x, it.x, it.x + it.w - TraySlot(*d));
                r = l + TraySlot(*d);
            } else if (it.id == StTrayBtn && l < 0) {
                l = it.x;
                r = it.x + it.w;
            }
        }
        if (l < 0) continue;
        auto s16 = [](float v) { return (SHORT)std::lround(v); };
        if (q.which == 2) return (LRESULT)(DWORD)MAKELONG(s16(d->rcMon.left + r), s16(bottom));
        return (LRESULT)(DWORD)MAKELONG(s16(d->rcMon.left + l), s16(top));
    }
    return 0;
}

LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_shellHookMsg && g_shellHookMsg) {
        HWND h = (HWND)lp;
        switch (wp & 0x7FFF) {
            case HSHELL_WINDOWCREATED:
            case HSHELL_WINDOWDESTROYED:
                g_winCache.erase(h);
                g_flashing.erase(h);
                ScheduleRebuild();
                break;
            case HSHELL_WINDOWACTIVATED:
                g_flashing.erase(h);
                ScheduleRebuild();
                break;
            case HSHELL_REDRAW:
                if (wp & 0x8000) g_flashing[h] = true;  // HSHELL_FLASH
                ScheduleRebuild();
                break;
        }
        return 0;
    }
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg) {
        // Always rebuild: a fixed bar must register its app bar again with the new Explorer.
        SetTimer(hwnd, TIMER_DOCKS, 1500, nullptr);
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wp == TIMER_POLL) {
                static DWORD lastSecond = 0, lastRebuild = 0, lastLaunch = 0;
                static UINT interval = 100;
                DWORD now = GetTickCount();
                Poll();
                bool second = now - lastSecond >= 1000;
                StatusTick(second);
                if (second) {
                    lastSecond = now;
                    if (g_set.hideTaskbar) HideTaskbars();
                    if (DockScalesChanged()) SetTimer(hwnd, TIMER_DOCKS, 300, nullptr);
                }
                if (now - lastRebuild >= 3000 || (!g_launch.empty() && now - lastLaunch >= 500)) {
                    lastRebuild = lastLaunch = now;
                    ScheduleRebuild();
                }
                PollPreview();
                // Fast only while the mouse matters (auto-hide edge, preview, Start, animations); otherwise
                // a slow check is enough for full-screen apps.
                bool fast = g_prev.visible || g_start.open || !g_launch.empty() ||
                            std::any_of(g_docks.begin(), g_docks.end(), [](Dock* d) { return d->cfg.autoHide || d->animating; });
                UINT want = fast ? 100 : 400;
                if (want != interval) {
                    interval = want;
                    SetTimer(hwnd, TIMER_POLL, interval, nullptr);
                }
            } else if (wp == TIMER_REBUILD) {
                KillTimer(hwnd, TIMER_REBUILD);
                std::vector<std::wstring> before;
                for (auto* d : g_docks) before.push_back(DockSig(*d));  // (before g_apps changes: elems index it)
                RebuildApps();
                if (g_prev.visible) {
                    auto it = std::find_if(g_apps.begin(), g_apps.end(), [](const App& a) { return a.key == g_prev.key; });
                    std::vector<HWND> now;
                    if (it != g_apps.end() && g_prev.dock) now = DockWins(*g_prev.dock, *it);
                    size_t n = std::min(now.size(), (size_t)kMaxPreviews);
                    bool same = n == g_prev.wins.size() && std::equal(g_prev.wins.begin(), g_prev.wins.end(), now.begin());
                    if (!same) HidePreview();
                }
                for (size_t i = 0; i < g_docks.size(); i++) {
                    // Redraw only when something the dock shows changed (the periodic check usually finds nothing).
                    Dock* d = g_docks[i];
                    BuildElems(*d, true);
                    if (DockSig(*d) != before[i] || !g_launch.empty()) Invalidate(*d);
                }
            } else if (wp == TIMER_DOCKS) {
                KillTimer(hwnd, TIMER_DOCKS);
                if (g_set.hideTaskbar) HideTaskbars();
                CreateDocks();
            } else if (wp == TIMER_TRAY_ASK) {
                KillTimer(hwnd, TIMER_TRAY_ASK);
                TrayAskMissing();
            } else if (wp == TIMER_TRAY_SAVE) {
                KillTimer(hwnd, TIMER_TRAY_SAVE);
                SaveTray();
            } else if (wp == TIMER_WIZARD) {
                KillTimer(hwnd, TIMER_WIZARD);
                WizStart();
                OpenSettings();
                SwSetPage(PgComecar);
            } else if (wp == TIMER_PLAYER_VIS) {
                PlayerVisStep();
            } else if (wp == TIMER_APPLY) {
                // A change in the settings window (several quick ones apply once).
                KillTimer(hwnd, TIMER_APPLY);
                SendMessageW(hwnd, WM_APP_SETTINGS, 0, 0);
            }
            return 0;
        case WM_DISPLAYCHANGE:
            // (Not on work-area changes: the fixed bar's own app bar causes those.)
            SetTimer(hwnd, TIMER_DOCKS, 800, nullptr);
            return 0;
        case WM_SETTINGCHANGE:
            // Transparency effects turned on or off (see BuildTheme).
            // (Also sent for light/dark and accent changes: reload only when the glass would change.)
            if (lp && !wcscmp((LPCWSTR)lp, L"ImmersiveColorSet") && TransparencyOn() != g_transparencyUsed)
                PostMessageW(hwnd, WM_APP_SETTINGS, 0, 0);
            else if (HighContrastOn() != g_contrastUsed)  // (high contrast on or off)
                PostMessageW(hwnd, WM_APP_SETTINGS, 0, 0);
            break;
        case WM_APP_CCREAD:
        case WM_APP_BTREAD:
            StatusChanged();
            return 0;
        case WM_APP_DEVREAD:
            BtAsk();  // (Bluetooth levels too, every 30 s)
            DevAlerts();
            StatusChanged();
            return 0;
        case WM_APP_MEDIA:
            MediaArtUpdate();
            PlayerVisUpdate();
            if (wp) StatusChanged();
            else if (g_fly.open && !g_fly.dragging && (g_fly.kind == StPlayer || g_fly.kind == StCentral)) FlyRender();
            return 0;
        case WM_APP_STATUS:
            if (wp == StVolume) {
                if (lp == 1) OpenAudio();
                else ReadVolume();
                if (g_fly.open && g_fly.kind == StVolume && !g_fly.dragging) {
                    for (auto& a : g_appVols)
                        if (!a.vols.empty()) a.vols[0]->GetMasterVolume(&a.level);
                }
            } else if (wp == StNetwork) {
                if (lp == 2) LoadWifiList();
                else ReadNetwork();
            }
            StatusChanged();
            return 0;
        case WM_APP_BRIGHT: {
            if (!TakePtr((void*)lp)) return 0;
            std::unique_ptr<std::vector<BrView>> v((std::vector<BrView>*)lp);
            g_brView = *v;
            g_brScanned = true;
            if (g_fly.open) FlyRender();
            if (g_start.open) RenderStart();
            return 0;
        }
        case WM_POWERBROADCAST:
            if (wp == PBT_APMPOWERSTATUSCHANGE && ReadBattery()) StatusChanged();
            return TRUE;
        case WM_COPYDATA: {
            // From the explorer.exe part (see ExTraySubclass).
            auto* cds = (COPYDATASTRUCT*)lp;
            if (cds && cds->dwData == kTrayCopy && cds->cbData >= sizeof(TrayWire) && cds->lpData) {
                TrayWire w;
                memcpy(&w, cds->lpData, sizeof(w));
                bool changed = TrayApply(w);  // (reads the icon while Explorer still holds it)
                ReplyMessage(TRUE);
                if (changed) TrayChanged();
                return TRUE;
            }
            if (cds && cds->dwData == kTrayRect && cds->cbData >= sizeof(TrayRectWire) && cds->lpData) {
                TrayRectWire q;
                memcpy(&q, cds->lpData, sizeof(q));
                return TrayRectOf(q);
            }
            break;
        }
        case WM_APP_OPEN_SETTINGS:
            OpenSettings();
            if (wp < PgCount) SwSetPage((int)wp);
            return 0;
        case WM_APP_ICON: {
            if (!TakePtr((void*)lp)) return 0;
            std::unique_ptr<IconResult> r((IconResult*)lp);
            ComPtr<IWICBitmapSource> icon;
            if (r->hbm) {
                icon = WicFromHBitmap(r->hbm);
                DeleteObject(r->hbm);
            }
            if (!r->key.compare(0, 7, L"#start:")) {
                std::wstring k = r->key.substr(7);
                g_startIcons[k] = icon;
                g_start.bmps.erase(k);
                if (g_start.open) SetTimer(g_start.hwnd, TIMER_START_ICONS, 30, nullptr);  // (several at once: one frame)
                return 0;
            }
            for (auto& a : g_apps) {
                if (a.key != r->key) continue;
                a.icon = icon;
                if (!r->label.empty()) a.label = r->label;
            }
            for (auto* d : g_docks) {
                d->bmps.erase(r->key);
                Invalidate(*d);
            }
            return 0;
        }
        case WM_APP_SETTINGS:
            LoadSettings();
            RegisterStartHotkey();
            PublishWinKeyChoice();
            TrayPublish();
            SyncBanners();
            HidePreview();
            SetAccent(g_prev.hwnd, true, true);
            RebuildApps();
            if (g_set.hideTaskbar) {
                HideTaskbars();
            } else {
                RestoreTaskbars();
                RestoreTaskbarAutoHide();  // also when this process never hid it (see UiThread)
            }
            CreateDocks();
            SwInvalidate();
            return 0;
        case WM_APP_WINKEY: {
            if (!g_start.open && GetTickCount() - g_startClosedBySearchAt < 700) {
                // This press already closed the Start (Windows' search box took the focus).
                g_startClosedBySearchAt = 0;
                Wh_Log(L"Windows key: Start already closed by this press");
                return 1;  // (handled: 0 would make the explorer.exe part open Windows' Start)
            }
            g_lastWinAlone = GetTickCount();
            POINT pt;
            GetCursorPos(&pt);
            HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
            Dock* target = nullptr;
            for (auto* d : g_docks) if (d->mon == mon) target = d;
            if (!target && !g_docks.empty()) target = g_docks[0];
            if (target) ToggleStart(*target);
            // (The explorer.exe part brings the open Start to the front itself.)
            if (g_start.open) StartWinKeyGuard();
            return g_start.open ? (LRESULT)g_start.hwnd : 1;
        }
        case WM_APP_WINN: {
            // Win+N (from the explorer.exe part): the notifications panel on the monitor with the mouse.
            POINT pt;
            GetCursorPos(&pt);
            HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
            Dock* target = nullptr;
            for (auto* d : g_docks)
                if (d->mon == mon) target = d;
            if (!target && !g_docks.empty()) target = g_docks[0];
            if (target) ToggleFlyout(*target, StNotif, false);
            return 0;
        }
        case WM_HOTKEY:
            if (wp == 1) {
                POINT pt;
                GetCursorPos(&pt);
                HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
                Dock* target = nullptr;
                for (auto* d : g_docks) if (d->mon == mon) target = d;
                if (!target && !g_docks.empty()) target = g_docks[0];
                if (target) ToggleStart(*target);
            }
            return 0;
        case WM_ENDSESSION:
            if (wp) {
                RestoreBanners();
                RestoreTaskbarAutoHide();
            }
            return 0;
        case WM_APP_QUIT:
            if (g_dockMenuDepth > 0) {  // (a dock's menu is open: the docks can't go from inside it)
                EndMenu();
                PostMessageW(hwnd, WM_APP_QUIT, 0, 0);
                return 0;
            }
            UnregisterHotKey(hwnd, 1);
            RemovePropW(hwnd, L"NimbusWinKey");
            RemovePropW(hwnd, L"NimbusWinN");
            if (g_moveEndHook) UnhookWinEvent(g_moveEndHook);
            if (g_locationHook) UnhookWinEvent(g_locationHook);
            g_moveEndHook = g_locationHook = nullptr;
            KillTimer(hwnd, TIMER_POLL);
            KillTimer(hwnd, TIMER_APPLY);
            // The Windows taskbar and the banners first: whatever is slow after this, they're back.
            RestoreTaskbars();
            RestoreBanners();
            StopIconThread();
            FreeSettingsWin();
            DestroyDocks();
            StatusUninit();
            DeregisterShellHookWindow(hwnd);
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI UiThread(LPVOID) {
    OleInitialize(nullptr);  // (COM too; drag and drop needs OLE)
    if (!GetUserDefaultLocaleName(g_locale, LOCALE_NAME_MAX_LENGTH)) wcscpy_s(g_locale, L"en-US");
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)g_dw.GetAddressOf());
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_wic));
    if (!g_d2d || !g_dw || !g_wic) {
        Wh_Log(L"Could not create the graphics factories");
        OleUninitialize();
        return 1;
    }

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.hInstance = g_module;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpfnWndProc = GlassProc;
    wc.lpszClassName = L"NimbusMenu_Glass";
    RegisterClassExW(&wc);
    wc.lpfnWndProc = ContentProc;
    wc.lpszClassName = L"NimbusMenu_Content";
    RegisterClassExW(&wc);
    wc.lpfnWndProc = MsgProc;
    wc.lpszClassName = L"NimbusMenu_Msg";
    RegisterClassExW(&wc);
    wc.lpfnWndProc = PreviewProc;
    wc.lpszClassName = L"NimbusMenu_Preview";
    RegisterClassExW(&wc);
    wc.lpfnWndProc = StartProc;
    wc.lpszClassName = L"NimbusMenu_Start";
    RegisterClassExW(&wc);
    wc.lpfnWndProc = SettingsProc;
    wc.lpszClassName = L"NimbusMenu_Settings";
    RegisterClassExW(&wc);
    wc.lpfnWndProc = FlyProc;
    wc.lpszClassName = L"NimbusMenu_Flyout";
    RegisterClassExW(&wc);
    g_fly.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"NimbusMenu_Flyout",
                                 L"Nimbus", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, g_module, nullptr);
    RegisterShelfDrop(g_fly.hwnd, nullptr);
    wc.lpfnWndProc = ToastProc;
    wc.lpszClassName = L"NimbusMenu_Toast";
    RegisterClassExW(&wc);
    g_toast.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"NimbusMenu_Toast", L"Nimbus",
                                   WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, g_module, nullptr);
    SoundSkipDucking();  // (before any call starts: Windows decides what to turn down when the call begins)
    for (int b = 0; b < 1 + 2 * kBandSteps; b++) {
        g_fly.bands.push_back(CreateBand());
        g_fly.bandRc.push_back(RECT{});
    }
    g_start.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"NimbusMenu_Start", Tr(S_Start),
                                   WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, g_module, nullptr);
    for (int b = 0; b < 1 + 2 * kBandSteps; b++) {
        g_start.bands.push_back(CreateBand());
        g_start.bandRc.push_back(RECT{});
    }
    LoadStartPins();
    g_prev.hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST, L"NimbusMenu_Preview", L"",
                                  WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, g_module, nullptr);
    {
        // Windows' own rounded corners (they do cut the blur, unlike a window region).
        DWORD corner = 2; /* DWMWCP_ROUND */
        DwmSetWindowAttribute(g_prev.hwnd, 33, &corner, sizeof(corner));
        COLORREF none = 0xFFFFFFFE;
        DwmSetWindowAttribute(g_prev.hwnd, 34, &none, sizeof(none));
        SetAccent(g_prev.hwnd, true, true);
    }

    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    g_msgWnd = CreateWindowExW(WS_EX_TOOLWINDOW, L"NimbusMenu_Msg", L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                               g_module, nullptr);
    RegisterShellHookWindow(g_msgWnd);

    LoadPins();
    RebuildApps();
    Wh_Log(L"start: pins=%d apps=%d", (int)g_pins.size(), (int)g_apps.size());
    for (auto& a : g_apps) Wh_Log(L"  app key=%s wins=%d pinned=%d", a.key.c_str(), (int)a.wins.size(), a.pinned);
    if (g_set.hideTaskbar) {
        HideTaskbars();
    } else {
        // The option may have been turned off while the bar wasn't running (after a crash): give the
        // taskbar back its own auto-hide and show it. Nothing happens if this mod never changed it.
        RestoreTaskbarAutoHide();
        ShowTaskbarWindows();
    }
    StatusInit();
    CreateDocks();
    SetTimer(g_msgWnd, TIMER_POLL, 100, nullptr);
    RegisterStartHotkey();
    PublishWinKeyChoice();

    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    // Release every COM/D2D object here, on this thread, before CoUninitialize (not at ExitProcess).
    g_apps.clear();
    g_allApps.clear();
    g_startIcons.clear();
    g_recent.clear();
    g_startPins.clear();
    g_settingsItems.clear();
    g_start.appHits.clear();
    g_start.setHits.clear();
    g_start.rows.clear();
    g_start.files.clear();
    g_start.bmps.clear();
    g_start.font.Reset();
    g_start.small.Reset();
    g_start.section.Reset();
    g_start.glyphs.Reset();
    g_start.big.Reset();
    g_start.head.Reset();
    g_start.dc.Reset();
    g_start.rt.Reset();
    g_prev.font.Reset();
    g_prev.rt.Reset();
    g_fly.bmps.clear();
    g_fly.br.Reset();
    g_fly.dc.Reset();
    g_fly.pageLayer.Reset();
    g_fly.rt.Reset();
    g_fly.fBig.Reset();
    g_fly.fTitle.Reset();
    g_fly.fText.Reset();
    g_fly.fSmall.Reset();
    g_fly.fCode.Reset();
    g_fly.fWrap.Reset();
    g_fly.fWrapS.Reset();
    g_iconPaths.clear();
    g_roundStroke.Reset();
    g_dashStroke.Reset();
    if (g_fly.hwnd) RevokeDragDrop(g_fly.hwnd);
    g_toast.bmps.clear();
    g_toast.br.Reset();
    g_toast.dc.Reset();
    g_toast.rt.Reset();
    g_toast.fApp.Reset();
    g_toast.fTitle.Reset();
    g_toast.fBody.Reset();
    g_toast.fBtn.Reset();
    if (g_toast.memDC) {
        SelectObject(g_toast.memDC, g_toast.oldBmp);
        DeleteDC(g_toast.memDC);
        DeleteObject(g_toast.dib);
    }
    g_d2d.Reset();
    g_dw.Reset();
    g_wic.Reset();
    OleUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// explorer.exe part: the Windows key. When Windows is about to show its Start
// (XamlLauncherState::ShowStartView, called by the Windows key), ask the bar to open Nimbus' Start
// instead. Only while the bar runs and wants the key; Ctrl+Esc always gets Windows' Start.
// ---------------------------------------------------------------------------

bool g_isExplorer;

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    const WCHAR* name = n ? wcsrchr(path, L'\\') : nullptr;
    return name && _wcsicmp(name + 1, L"explorer.exe") == 0;
}

static HWND BarWantingKey() {
    HWND bar = FindWindowW(L"NimbusMenu_Msg", nullptr);
    if (!bar || !GetPropW(bar, L"NimbusWinKey")) return nullptr;
    return bar;
}

using ShowStartView_t = long(__cdecl*)(void*, int, int);
ShowStartView_t ShowStartView_orig;

long __cdecl ShowStartView_hook(void* self, int method, int flags) {
    // Ctrl+Esc (still held when this runs): Windows' Start, on purpose.
    bool ctrlEsc = (GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_ESCAPE) & 0x8000);
    if (!ctrlEsc) {
        if (HWND bar = BarWantingKey()) {
            // Explorer may bring a window to the front right now (it would for its own Start): it waits
            // for the bar to open Nimbus' Start (a few ms) and brings it to the front itself, so typing
            // goes straight to the search.
            DWORD barPid = 0;
            GetWindowThreadProcessId(bar, &barPid);
            if (barPid) AllowSetForegroundWindow(barPid);
            DWORD_PTR opened = 0;
            BOOL sent = SendMessageTimeoutW(bar, WM_APP_WINKEY, 1, 0, SMTO_ABORTIFHUNG, 600, &opened);
            if (!sent || !opened) {
                // (hung, busy or closing: Windows' own Start rather than nothing)
                Wh_Log(L"Windows key: the bar didn't answer, Windows' Start");
                return ShowStartView_orig(self, method, flags);
            }
            // Windows' search box (SearchHost, invisible here) took the keyboard when the key went down.
            // An Esc makes it let go.
            HWND fg = GetForegroundWindow();
            wchar_t cls[64] = {};
            GetClassNameW(fg, cls, ARRAYSIZE(cls));
            HWND w = (sent && opened && IsWindow((HWND)opened)) ? (HWND)opened : nullptr;
            BOOL fgOk = w ? SetForegroundWindow(w) : FALSE;
            Wh_Log(L"Windows key: sent=%d start=%p foreground=%s set=%d", sent, w, cls, fgOk);
            if (fgOk) return S_OK;
            if (!wcscmp(cls, L"Windows.UI.Core.CoreWindow")) {
                INPUT in[2] = {};
                in[0].type = in[1].type = INPUT_KEYBOARD;
                in[0].ki.wVk = VK_ESCAPE;
                in[1].ki.wVk = VK_ESCAPE;
                in[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, in, sizeof(INPUT));
            }
            if (w) {
                for (int i = 0; i < 4 && GetForegroundWindow() != w; i++) {  // (60 ms at most inside Explorer)
                    Sleep(15);
                    SetForegroundWindow(w);
                }
                if (GetForegroundWindow() != w) Wh_Log(L"Could not bring Nimbus' Start to the front");
            }
            return S_OK;
        }
    }
    return ShowStartView_orig(self, method, flags);
}

// Win+N: Windows shows its notification center through CActionCenterExperienceManager::Show (seen on
// 26200). When the user chose Nimbus' notifications, the bar opens its own panel instead. As for the
// Windows key, this works over windows run as administrator and in games.
using ActionCenterShow_t = long(__cdecl*)(void*);
ActionCenterShow_t ActionCenterShow_orig;
long __cdecl ActionCenterShow_hook(void* self) {
    HWND bar = FindWindowW(L"NimbusMenu_Msg", nullptr);
    if (bar && GetPropW(bar, L"NimbusWinN")) {
        DWORD pid = 0;
        GetWindowThreadProcessId(bar, &pid);
        if (pid) AllowSetForegroundWindow(pid);
        PostMessageW(bar, WM_APP_WINN, 0, 0);
        Wh_Log(L"Win+N: Nimbus' notifications");
        return S_OK;
    }
    return ActionCenterShow_orig(self);
}

// Folder windows can run in their own explorer.exe processes, where Start never opens.
static bool IsFolderWindowsProcess() {
    std::wstring cmd = Lower(GetCommandLineW());
    return cmd.find(L"/factory") != std::wstring::npos || cmd.find(L"-embedding") != std::wstring::npos ||
           cmd.find(L"/separate") != std::wstring::npos;
}

// ---------------------------------------------------------------------------
// explorer.exe part: the tray. Shell_TrayWnd gets every Shell_NotifyIcon call as WM_COPYDATA; each one
// is kept here (with a copy of its icon) and forwarded to the bar, which replays what it missed by
// posting NimbusMenu_TrayReplay. Windows' own handling is untouched (it runs first).
// ---------------------------------------------------------------------------

#pragma pack(push, 1)
struct NotifyIconData32 {  // NOTIFYICONDATAW as the shell sends it (32-bit handles)
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
#pragma pack(pop)

struct ExTrayIcon {
    TrayWire w{};  // the merged known state
    HICON own = nullptr;
};
std::vector<ExTrayIcon> g_exTray;  // (only touched by the tray window's thread)
HWND g_exTrayWnd;
UINT g_exReplayMsg;
HANDLE g_exTrayStop;
HANDLE g_exTrayThread;
bool g_exSubclassed;

// What goes to the bar waits here: Explorer's tray thread only queues (it never waits on the bar's
// process); ExTrayThread sends. Each item has its own copy of the icon, destroyed once sent.
struct ExTrayOut {
    TrayWire w;
    HICON icon;
};
std::vector<ExTrayOut> g_exOut;
SRWLOCK g_exOutLock = SRWLOCK_INIT;
HANDLE g_exOutEvent;

static HWND BarWantingTray() {
    HWND bar = FindWindowW(L"NimbusMenu_Msg", nullptr);
    return bar && GetPropW(bar, L"NimbusTray") ? bar : nullptr;
}

static void SendTrayWire(HWND bar, const TrayWire& w) {
    COPYDATASTRUCT cds = {kTrayCopy, sizeof(TrayWire), (void*)&w};
    DWORD_PTR r = 0;
    SendMessageTimeoutW(bar, WM_COPYDATA, 0, (LPARAM)&cds, SMTO_ABORTIFHUNG | SMTO_BLOCK, 150, &r);
}

// Queue one for the bar (icon: a handle of Explorer's own, copied again for the queue).
static int ExTrayFind(const TrayWire& w);
static void ExTrayQueue(TrayWire w, HICON icon) {
    // One pending entry per icon: a newer one replaces it, as the icon's whole known state (so nothing
    // an older change carried is lost), or as its removal. The queue never holds more than the tray.
    bool del = w.op == NIM_DELETE;
    if (!del) {
        int k = ExTrayFind(w);
        if (k >= 0) {
            TrayWire full = g_exTray[k].w;
            full.op = kTrayKnown;
            w = full;
            icon = g_exTray[k].own;
        }
    }
    HICON copy = icon ? CopyIcon(icon) : nullptr;
    w.hicon = (DWORD)(ULONG_PTR)copy;
    bool byGuid = (w.flags & NIF_GUID) != 0;
    AcquireSRWLockExclusive(&g_exOutLock);
    auto same = [&](const ExTrayOut& o) {
        return byGuid ? ((o.w.flags & NIF_GUID) && IsEqualGUID(o.w.guid, w.guid)) : (o.w.hwnd == w.hwnd && o.w.uid == w.uid);
    };
    auto it = std::find_if(g_exOut.begin(), g_exOut.end(), same);
    if (it != g_exOut.end()) {
        if (it->icon) DestroyIcon(it->icon);
        g_exOut.erase(it);
    }
    g_exOut.push_back({w, copy});
    ReleaseSRWLockExclusive(&g_exOutLock);
    if (g_exOutEvent) SetEvent(g_exOutEvent);
}

static int ExTrayFind(const TrayWire& w) {
    bool byGuid = (w.flags & NIF_GUID) != 0;
    for (int i = 0; i < (int)g_exTray.size(); i++) {
        const TrayWire& k = g_exTray[i].w;
        if (byGuid ? ((k.flags & NIF_GUID) && IsEqualGUID(k.guid, w.guid)) : (k.hwnd == w.hwnd && k.uid == w.uid)) return i;
    }
    return -1;
}

static void ExTrayKeep(const TrayWire& w) {
    int i = ExTrayFind(w);
    if (w.op == NIM_DELETE) {
        if (i >= 0) {
            if (g_exTray[i].own) DestroyIcon(g_exTray[i].own);
            g_exTray.erase(g_exTray.begin() + i);
        }
        return;
    }
    if (w.op == NIM_SETVERSION) {
        if (i >= 0) g_exTray[i].w.version = w.version;
        return;
    }
    if (w.op != NIM_ADD && w.op != NIM_MODIFY) return;
    if (i < 0) {
        if (w.op == NIM_MODIFY && !(w.flags & NIF_ICON)) return;
        ExTrayIcon e;
        e.w.hwnd = w.hwnd;
        e.w.uid = w.uid;
        if (w.flags & NIF_GUID) {
            e.w.guid = w.guid;
            e.w.flags |= NIF_GUID;
        }
        g_exTray.push_back(e);
        i = (int)g_exTray.size() - 1;
    }
    ExTrayIcon& e = g_exTray[i];
    if (w.flags & NIF_MESSAGE) {
        e.w.cbMsg = w.cbMsg;
        e.w.flags |= NIF_MESSAGE;
    }
    if (w.flags & NIF_ICON) {
        if (e.own) DestroyIcon(e.own);
        e.own = w.hicon ? CopyIcon(WireIcon(w.hicon)) : nullptr;
        e.w.flags |= NIF_ICON;
    }
    if (w.flags & NIF_TIP) {
        memcpy(e.w.tip, w.tip, sizeof(e.w.tip));
        e.w.flags |= NIF_TIP;
    }
    if ((w.flags & NIF_STATE) && (w.stateMask & NIS_HIDDEN)) {
        e.w.state = (e.w.state & ~NIS_HIDDEN) | (w.state & NIS_HIDDEN);
        e.w.stateMask |= NIS_HIDDEN;
        e.w.flags |= NIF_STATE;
    }
}

static void ExTrayReplay() {
    if (!BarWantingTray()) return;
    for (auto it = g_exTray.begin(); it != g_exTray.end();) {
        if (!IsWindow(WireHwnd(it->w.hwnd))) {
            if (it->own) DestroyIcon(it->own);
            it = g_exTray.erase(it);
        } else {
            ++it;
        }
    }
    // (queued, not sent here: nothing is sent while the list could change under the loop)
    for (auto& e : g_exTray) {
        TrayWire w = e.w;
        w.op = kTrayKnown;
        ExTrayQueue(w, e.own);
    }
    Wh_Log(L"tray: replayed %d icons", (int)g_exTray.size());
}

static LRESULT CALLBACK ExTraySubclass(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == g_exReplayMsg && g_exReplayMsg) {
        ExTrayReplay();
        return 0;
    }
    if (msg != WM_COPYDATA) return DefSubclassProc(hWnd, msg, wp, lp);
    auto* cds = (COPYDATASTRUCT*)lp;
    if (!cds || !cds->lpData || cds->cbData < 8 || *(const DWORD*)cds->lpData != 0x34753423)
        return DefSubclassProc(hWnd, msg, wp, lp);
    const BYTE* raw = (const BYTE*)cds->lpData;
    if (cds->dwData == 1 && cds->cbData >= 8 + offsetof(NotifyIconData32, szTip)) {
        LRESULT res = DefSubclassProc(hWnd, msg, wp, lp);
        NotifyIconData32 nid = {};
        size_t n = std::min<size_t>(cds->cbData - 8, sizeof(nid));
        memcpy(&nid, raw + 8, n);
        TrayWire w = {};
        w.op = *(const DWORD*)(raw + 4);
        w.hwnd = nid.hWnd;
        w.uid = nid.uID;
        w.flags = nid.uFlags;
        w.cbMsg = nid.uCallbackMessage;
        w.hicon = nid.hIcon;
        // Older, shorter structures: only what they hold.
        bool v2 = n >= offsetof(NotifyIconData32, guidItem), v3 = n >= offsetof(NotifyIconData32, hBalloonIcon);
        size_t tipMax = v2 ? 128 : 64;
        size_t tipLen = wcsnlen(nid.szTip, tipMax);
        memcpy(w.tip, nid.szTip, std::min<size_t>(tipLen, 127) * sizeof(WCHAR));
        if (v2) {
            w.state = nid.dwState;
            w.stateMask = nid.dwStateMask;
            w.version = nid.uVersion;
        } else {
            w.flags &= ~(NIF_STATE | NIF_INFO);
        }
        if (v3) w.guid = nid.guidItem;
        else w.flags &= ~NIF_GUID;
        if (res || w.op == NIM_ADD) {  // (what Explorer accepted; a repeated add is refused but complete)
            ExTrayKeep(w);
            if (BarWantingTray()) {
                // (Explorer's own copy of the icon goes out: the app may destroy its handle any time)
                int k = ExTrayFind(w);
                ExTrayQueue(w, (w.flags & NIF_ICON) && k >= 0 ? g_exTray[k].own : nullptr);
            }
        }
        return res;
    }
    // Shell_NotifyIconGetRect: 1 asks for the top-left corner, 2 for the bottom-right one.
    if (cds->dwData == 3 && cds->cbData >= 40) {
        DWORD which = *(const DWORD*)(raw + 4);
        TrayRectWire q = {};
        q.hwnd = *(const DWORD*)(raw + 16);
        q.uid = *(const UINT*)(raw + 20);
        memcpy(&q.guid, raw + 24, sizeof(GUID));
        q.which = which;
        if (HWND bar = BarWantingTray(); bar && (which == 1 || which == 2)) {
            // (this one is asked by an app waiting for the answer: a short wait, never on a hung bar)
            COPYDATASTRUCT c = {kTrayRect, sizeof(q), &q};
            DWORD_PTR r = 0;
            if (SendMessageTimeoutW(bar, WM_COPYDATA, 0, (LPARAM)&c, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &r) && r) return (LRESULT)(DWORD)r;
        }
    }
    return DefSubclassProc(hWnd, msg, wp, lp);
}

// The taskbar window may not exist yet when Explorer starts: wait for it here. Then this thread sends
// the queued icons to the bar (the tray thread never waits on it).
static DWORD WINAPI ExTrayThread(void*) {
    bool found = false;
    for (int i = 0; i < 1200 && !found; i++) {
        if (HWND h = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid != GetCurrentProcessId()) return 0;  // (another Explorer owns the taskbar)
            g_exTrayWnd = h;
            g_exSubclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(h, ExTraySubclass, 0);
            Wh_Log(L"tray: %s", g_exSubclassed ? L"listening" : L"subclass failed");
            found = true;
            break;
        }
        if (WaitForSingleObject(g_exTrayStop, 250) == WAIT_OBJECT_0) return 0;
    }
    if (!g_exSubclassed) return 0;
    HANDLE waits[2] = {g_exTrayStop, g_exOutEvent};
    for (;;) {
        if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) == WAIT_OBJECT_0) break;
        for (;;) {
            AcquireSRWLockExclusive(&g_exOutLock);
            std::vector<ExTrayOut> batch;
            batch.swap(g_exOut);
            ReleaseSRWLockExclusive(&g_exOutLock);
            if (batch.empty()) break;
            HWND bar = BarWantingTray();
            for (auto& o : batch) {
                if (bar && WaitForSingleObject(g_exTrayStop, 0) != WAIT_OBJECT_0) SendTrayWire(bar, o.w);
                if (o.icon) DestroyIcon(o.icon);
            }
        }
    }
    return 0;
}

static void ExTrayStart() {
    g_exReplayMsg = RegisterWindowMessageW(L"NimbusMenu_TrayReplay");
    g_exTrayStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_exOutEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_exTrayThread = CreateThread(nullptr, 0, ExTrayThread, nullptr, 0, nullptr);
}

static void ExTrayStop() {
    // The subclass goes first (nothing more is queued), then the thread: it only waits on these events,
    // on SetWindowSubclassFromAnyThread, or on a send of at most 150 ms, so it always ends.
    if (g_exSubclassed && IsWindow(g_exTrayWnd)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_exTrayWnd, ExTraySubclass);
    if (g_exTrayStop) SetEvent(g_exTrayStop);
    if (g_exTrayThread) {
        WaitForSingleObject(g_exTrayThread, INFINITE);
        CloseHandle(g_exTrayThread);
        g_exTrayThread = nullptr;
    }
    // (the thread may have subclassed the window while stopping)
    if (g_exSubclassed && IsWindow(g_exTrayWnd)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_exTrayWnd, ExTraySubclass);
    g_exSubclassed = false;
    for (auto& e : g_exTray)
        if (e.own) DestroyIcon(e.own);
    g_exTray.clear();
    for (auto& o : g_exOut)
        if (o.icon) DestroyIcon(o.icon);
    g_exOut.clear();
    if (g_exTrayStop) {
        CloseHandle(g_exTrayStop);
        g_exTrayStop = nullptr;
    }
    if (g_exOutEvent) {
        CloseHandle(g_exOutEvent);
        g_exOutEvent = nullptr;
    }
}

static BOOL ExplorerInit() {
    if (IsFolderWindowsProcess()) return FALSE;
    ExTrayStart();
    // Stays loaded even when the hook can't be set, so that ExplorerUninit always runs and can give the
    // taskbar back (see RestoreTaskbarAutoHide).
    HMODULE twinui = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!twinui) {
        Wh_Log(L"twinui.pcshell.dll not loaded: the Windows key keeps opening Windows' Start");
        return TRUE;
    }
    // Both resolved in one call (HookSymbols caches per module); recent builds (seen on 26200) go through
    // XamlLauncherState, others have only XamlLauncher::ShowStartView.
    void* xamlLauncherState = nullptr;
    void* xamlLauncher = nullptr;
    void* actionCenterShow = nullptr;
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"public: virtual long __cdecl XamlLauncherState::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum "
          L"IMMERSIVELAUNCHERSHOWFLAGS)"},
         &xamlLauncherState, nullptr, true},
        {{L"public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum "
          L"IMMERSIVELAUNCHERSHOWFLAGS)"},
         &xamlLauncher, nullptr, true},
        {{L"private: long __cdecl CActionCenterExperienceManager::Show(void)"}, &actionCenterShow, nullptr, true},
    };
    WindhawkUtils::HookSymbols(twinui, hooks, ARRAYSIZE(hooks));
    if (void* target = xamlLauncherState ? xamlLauncherState : xamlLauncher) {
        WindhawkUtils::SetFunctionHook((ShowStartView_t)target, ShowStartView_hook, &ShowStartView_orig);
    } else {
        Wh_Log(L"ShowStartView not found: the Windows key keeps opening Windows' Start");
    }
    if (actionCenterShow) {
        WindhawkUtils::SetFunctionHook((ActionCenterShow_t)actionCenterShow, ActionCenterShow_hook, &ActionCenterShow_orig);
    } else {
        Wh_Log(L"CActionCenterExperienceManager::Show not found: Win+N stays Windows'");
    }
    return TRUE;
}

// The mod is being turned off (or updated): once the bar's process is gone, make sure the Windows
// taskbar is back, also when that process crashed or was ended before it could restore it.
static void RestoreBanners();
static void ExplorerUninit() {
    ExTrayStop();
    // The tool process holds its mutex until it exits: wait for that (it restores everything itself),
    // at most 10 s.
    bool gone = true;
    if (HANDLE m = OpenMutexW(SYNCHRONIZE, FALSE, L"windhawk-tool-mod_" WH_MOD_ID)) {
        DWORD r = WaitForSingleObject(m, 10000);
        gone = r == WAIT_OBJECT_0 || r == WAIT_ABANDONED;
        if (gone) ReleaseMutex(m);
        CloseHandle(m);
    }
    if (!gone || FindWindowW(L"NimbusMenu_Msg", nullptr)) return;  // still running: it restores things itself
    RestoreTaskbarAutoHide();
    ShowTaskbarWindows();
    RestoreBanners();  // (what a crashed or ended bar left off; nothing when it restored them itself)
}

// ---------------------------------------------------------------------------
// Tool mod callbacks
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&WhTool_ModInit, &g_module);
    LoadSettings();
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    return g_uiThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_msgWnd) PostMessageW(g_msgWnd, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    if (g_msgWnd) PostMessageW(g_msgWnd, WM_APP_QUIT, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
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
//
// Nimbus Menu: the only change is the explorer.exe dispatch at the top of each
// callback (the Windows key hook, see ExplorerInit).

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    if (IsExplorerProcess()) {
        g_isExplorer = true;
        return ExplorerInit();
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
    if (g_isExplorer) {
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

void Wh_ModSettingsChanged() {
    if (g_isExplorer) {
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        ExplorerUninit();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
