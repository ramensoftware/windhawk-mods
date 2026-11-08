// ==WindhawkMod==
// @id              adaptive-windows
// @name            Adaptive Windows
// @description     Grade automática de janelas com animações fluidas (estilo Nava OS), borda animada e compatível com mods de minimizar/restaurar.
// @version         1.0.0
// @author          Thiago Silva
// @github          https://github.com/brideinflame-rgb
// @include         explorer.exe
// @compilerOptions -ldwmapi -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Adaptive Windows

*(Português do Brasil logo abaixo / Brazilian Portuguese below)*

![Adaptive Windows demo](https://raw.githubusercontent.com/brideinflame-rgb/Adaptive-Windows/main/screenshot/adaptive-windows-demo.gif)

## English

An automatic tiling window manager for Windows 11, inspired by the **Nava OS** concept videos.
It arranges the windows of each monitor in a grid, animates every change with fluid transitions,
and works together with minimize / restore / open animation mods: a window is animated
**straight to the new place assigned by the grid**, not to where it used to be.

### Features
- **Automatic grid.** A new window takes a slot and the others make room for it; when a window
  closes or is minimized, the rest take over the free space.
- **Respects each app's minimum size.** The grid reshapes itself so no window ends up smaller than
  its app allows or overlaps another one (for example Outlook and Spotify together on a short
  screen). If a chosen layout does not fit, the automatic layout is used.
- **Free resizing.** Drag a window edge and the grid adapts to the width or height you chose.
  Dragging a window by its title and dropping it over another one swaps the two; dropping it
  anywhere else sends it back to its slot.
- **Maximized windows** that return to or enter the grid go back to normal mode and take their slot.
- **Animated border on the focused window** (replaces tacky-border): color, effect (solid, pulse,
  rainbow, gradient), line width, glow, corner radius and offset, so it can match your rounded
  corners mod.
- **Multiple monitors.** Each monitor has its own grid, and windows can be dragged between them,
  including monitors with different display scales. It works within the limits each window or app
  imposes: the minimum size is decided by the app (and grows with the monitor's scale), so an app
  whose minimum is wider than the monitor will overflow its right / bottom edge.

### Hotkeys
| Hotkey | Action |
|--------|--------|
| Alt + G (default) | Cycle the layout: auto, columns, rows, grid, main left, main top |
| Alt + T (default) | Take the focused window out of the grid (it stays where it is and the others use the space); press again to put it back |
| Win + Alt + F | Focus mode: every window on the monitor fills the screen |
| Win + Alt + ← / → | Swap the focused window with the previous / next one |
| Win + Alt + ↓ | Re-arrange everything and put all windows back in the grid |

The Win + Alt hotkeys can use another modifier in the settings (Ctrl+Alt, Ctrl+Alt+Shift or
Alt+Shift) if another program already uses them; restart the mod after changing. Alt+G and Alt+T
are the defaults and can be changed in the settings (text fields such as "Ctrl+Alt+G" or
"Win+T"; leave empty to turn the hotkey off). Unavailable hotkeys are reported in the Windhawk
log (error 1409 = already in use).

### Works with other animation mods
Adaptive Windows works together with these two mods:

- **MacOS Minimize Animation** (genie effect for minimize, restore and open).
- **Windows Animations**.

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
The frame rate can follow the DWM (recommended) or be fixed.

### Taskbar and dock
The usable area is the Windows work area. The screen-edge margin applies to all four sides. If the
taskbar is hidden, windows only respect the screen edge. The bottom spacing setting (0 = automatic)
is meant for docks drawn by another mod: enter the distance from the screen edge to the top of
the dock.

### Corners
Uses the Windows 11 corner preference (default, rounded, small, square). Windows does not allow an
arbitrary radius this way. If you already use a corner mod (for example "Custom Window Corner
Radius"), leave this option on "do not touch" so the two do not fight over the corners.

### Ignored windows and limits
- Tool windows, always-on-top, non-focusable and non-resizable windows (panels, widgets) stay out
  of the grid, as do the ones in the **windows to ignore** list (part of the title or class,
  separated by semicolons). The list is empty by default.
- Windows that refuse to be positioned (a likely case is programs running as administrator) float
  after a few attempts; Win + Alt + ↓ tries again.
- The settings screen is currently in Brazilian Portuguese only.

---

## Português (Brasil)

Gerenciador de janelas em grade automática para o Windows 11, inspirado nos vídeos de conceito
do **Nava OS**. Ele organiza as janelas de cada monitor, anima as mudanças com transições fluidas
e funciona junto com os mods de animação de minimizar, restaurar e abrir janelas: a janela
é animada **já para o novo lugar definido pela grade**, e não para onde ela estava antes.

### O que ele faz
- **Grade automática.** Cada janela nova ocupa um espaço da grade e as outras abrem espaço para
  ela; ao fechar ou minimizar uma janela, as demais ocupam o espaço livre.
- **Respeita o tamanho mínimo de cada app.** O formato da grade se ajusta sozinho para que nenhuma
  janela fique menor do que o app aceita nem sobreponha outra (por exemplo, Outlook e Spotify
  juntos numa tela de pouca altura). Se um layout escolhido não couber, ele usa o automático.
- **Redimensionamento livre.** Arraste a borda de uma janela e a grade se adapta à largura ou à
  altura que você escolheu. Arrastar uma janela pelo meio e soltá-la sobre outra troca as duas
  de lugar; soltá-la em qualquer outro ponto a devolve ao seu espaço.
- **Janelas maximizadas** que voltam ou entram na grade passam ao modo normal e assumem o espaço.
- **Borda animada na janela em foco** (substitui o uso do tacky-border): cor, efeito (fixa,
  pulsar, arco-íris, degradê), largura da linha, brilho, raio dos cantos e deslocamento, para
  alinhar com o seu mod de cantos arredondados.
- **Vários monitores.** Cada monitor tem a sua própria grade, e as janelas podem ser arrastadas
  entre eles, inclusive entre monitores de escalas diferentes. Funciona dentro dos limites
  impostos por cada janela ou app: o tamanho mínimo é decidido pelo app (e cresce com a escala do
  monitor), então um app cujo mínimo é maior que a largura do monitor vai passar da borda direita
  / de baixo.

### Atalhos
| Atalho | Ação |
|--------|------|
| Alt + G (padrão) | Troca o layout: automático, colunas, linhas, grade, principal à esquerda, principal em cima |
| Alt + T (padrão) | Tira a janela em foco da grade (ela fica onde está e as outras ocupam o espaço); apertar de novo devolve à grade |
| Win + Alt + F | Modo foco: todas as janelas do monitor ocupam a tela toda |
| Win + Alt + ← / → | Troca a janela em foco com a anterior / a próxima |
| Win + Alt + ↓ | Reorganiza tudo e devolve todas as janelas à grade |

Os atalhos com Win + Alt podem trocar de modificador nas configurações (Ctrl+Alt, Ctrl+Alt+Shift
ou Alt+Shift) caso outro programa já os use; reinicie o mod depois de mudar. Alt+G e Alt+T são
os padrões e podem ser trocados nas configurações (campos de texto como "Ctrl+Alt+G" ou "Win+T";
deixe vazio para desligar o atalho). Atalhos indisponíveis aparecem no log do Windhawk
(erro 1409 = já em uso).

### Funciona junto com outros mods de animação
O Adaptive Windows funciona junto com estes dois mods:

- **MacOS Minimize Animation** (genie de minimizar, restaurar e abrir).
- **Windows Animations**.

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
sobreposição. A taxa de quadros pode seguir o DWM (recomendado) ou ser fixada.

### Barra de tarefas e dock
A área útil é a área de trabalho do Windows. A **Margem da borda da tela** vale para os quatro
lados. Se a barra de tarefas estiver escondida, as janelas respeitam apenas a borda da tela.
**Espaço embaixo** (0 = automático) serve para docks desenhados por outro mod: informe a distância
da borda da tela até o topo do dock.

### Cantos
Usa a preferência de cantos do próprio Windows 11 (padrão, arredondado, pequeno, quadrado).
O Windows não permite raio arbitrário por esta via. Se você já usa um mod de cantos (por exemplo,
"Custom Window Corner Radius"), deixe esta opção em "Não mexer" para os dois não disputarem
os cantos.

### Janelas ignoradas e limites
- Ficam fora da grade janelas de ferramenta, sempre no topo, sem foco, sem moldura redimensionável
  (por exemplo, painéis e widgets), e as da lista **Janelas a ignorar** (parte do título ou da
  classe, separadas por ponto e vírgula). A lista vem vazia por padrão.
- Janelas que não aceitam ser posicionadas (um caso provável são programas executados como
  administrador) passam a flutuar depois de algumas tentativas; Win + Alt + ↓ tenta de novo.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gap: 10
  $name: Espaço entre janelas (px)
- padding: 12
  $name: Margem da borda da tela (px)
- marginBottom: 0
  $name: "Espaço embaixo (px, a partir da borda da tela)"
  $description: "0 = automático: acompanha a barra de tarefas/dock do Windows; se ela estiver escondida, só respeita a borda da tela. Use um valor (distância da borda da tela até o topo do seu dock) só se o dock for desenhado por outro mod e as janelas ficarem longe ou por cima dele. Vale o maior entre este valor e a barra de tarefas."
- layout: auto
  $name: Layout da grade
  $description: "Alt+G troca de layout na hora. Se os tamanhos mínimos dos apps não couberem, o mod ajusta o formato sozinho."
  $options:
  - auto: Automático (colunas até 3 janelas, depois grade)
  - columns: Colunas lado a lado
  - rows: Linhas empilhadas
  - grid: Grade quadrada
  - mainleft: Principal à esquerda, o resto empilhado à direita
  - maintop: Principal em cima, o resto em colunas embaixo
- corners: default
  $name: Cantos das janelas organizadas
  $options:
  - default: Não mexer (padrão do Windows)
  - round: Arredondado
  - small: Arredondado pequeno
  - square: Quadrado
- preset: nava
  $name: Estilo de animação
  $options:
  - nava: Nava OS (base do vídeo)
  - smooth: Suave
  - snappy: Rápido
  - playful: Elástico (overshoot)
- fps: dwm
  $name: Taxa de quadros da animação
  $description: >-
    "Sincronizar com o DWM" acompanha a atualização do compositor do Windows (recomendado).
    As taxas fixas usam um temporizador de alta resolução: limitar a taxa (por exemplo 60 Hz
    num monitor de 144 Hz) reduz a carga em apps pesados. "Maior taxa dos monitores" lê a
    taxa de atualização atual de cada monitor e usa a mais alta.
  $options:
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
- launchSync: true
  $name: "Sincronizar com animações de abrir/fechar (ex.: MacOS Minimize Animation, Windows Animations)"
  $description: "Se uma janela nova aparece escondida por outro mod (cloak do DWM), ela é posicionada no lugar final na hora, sem animação própria, enquanto as outras abrem espaço ao mesmo tempo. Assim a animação do outro mod acontece já no lugar certo."
- layoutHotkey: "Alt+G"
  $name: Atalho para trocar de layout (reinicie o mod ao mudar)
  $description: "Escreva modificadores e tecla, como Alt+G, Ctrl+Alt+L ou Win+Shift+G. Modificadores aceitos: Ctrl, Alt, Shift, Win. Deixe vazio para desligar. Se aparecer como indisponível no log (erro 1409), outro programa já usa essa combinação."
- floatHotkey: "Alt+T"
  $name: Atalho para soltar/prender a janela em foco (reinicie o mod ao mudar)
  $description: "Mesmo formato do atalho de layout. Deixe vazio para desligar."
- hotkeys: winalt
  $name: Modificadores dos atalhos (reinicie o mod ao mudar)
  $description: "Se os atalhos aparecerem como indisponíveis no log (erro 1409), outro programa já usa essas teclas. Troque aqui."
  $options:
  - winalt: Win+Alt
  - ctrlalt: Ctrl+Alt
  - ctrlaltshift: Ctrl+Alt+Shift
  - altshift: Alt+Shift
- debugLog: false
  $name: Registrar informações detalhadas no log do Windhawk
- borderColor: "7B8CFF"
  $name: Cor da borda da janela em foco (RGB hex; vazio = desligado)
- borderStyle: static
  $name: Efeito da borda da janela em foco
  $description: "A borda é a do próprio Windows (1 px, acompanha os cantos). Pulsar, arco-íris e degradê animam a cor continuamente."
  $options:
  - static: Fixa
  - pulse: Pulsar (luz que respira)
  - rainbow: Arco-íris
  - gradient: Degradê entre duas cores
- borderColor2: "FF5CE1"
  $name: Segunda cor do degradê (RGB hex)
- borderSpeed: 4
  $name: Segundos por ciclo da animação da borda
- borderWidth: 1
  $name: Largura da linha da borda (px)
  $description: "1 = borda nativa do Windows (fina). Acima de 1, o mod desenha a borda por cima, fora da janela, com a espessura escolhida e o efeito girando em volta da janela. Use até o valor do espaço entre janelas."
- borderGlow: 0
  $name: Brilho em volta da borda (px, 0 = sem brilho)
- borderRadius: 8
  $name: Raio dos cantos da borda (px)
  $description: "Só vale com a borda desenhada por cima (largura acima de 1 ou brilho). Ajuste para ficar igual ao raio do seu mod de cantos arredondados (por exemplo 8 é o padrão do Windows 11; 0 = cantos retos)."
- borderOffset: 0
  $name: Deslocamento da borda (px)
  $description: "0 = colada na borda da janela, por fora. Positivo afasta para fora; negativo puxa para dentro da janela. Serve para alinhar com o mod de cantos."
- ignoreList: ""
  $name: "Janelas a ignorar (separe por ponto e vírgula; vale para parte do título ou da classe)"
  $description: "Essas janelas ficam livres, fora da grade. Útil para barras e painéis de outros mods. Exemplo: Finder Bar;minha-janela. Vazio por padrão."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <cmath>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>
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

enum Entry { EN_NONE, EN_RIGHT, EN_LEFT, EN_UP, EN_DOWN };

struct Bezier { double x1 = 0.22, y1 = 1.0, x2 = 0.36, y2 = 1.0; };

struct Config {
    int gap = 10;
    int padding = 12;
    int mTop = 0, mBottom = 0, mLeft = 0, mRight = 0;
    bool reserveTaskbar = true;
    bool unmaximize = true;
    int duration = 450;
    Bezier bez;
    Entry entry = EN_RIGHT;
    bool dropFloat = false;
    int layout = 0;  // 0 auto, 1 colunas, 2 linhas, 3 grade, 4 principal esq., 5 principal em cima
    int hz = 0;    // 0 = sincronizar com o DWM; -1 = maior taxa dos monitores; senão Hz fixo
    bool debugLog = false;
    bool launchSync = true;
    int corner = -1;  // -1 = não mexer; senão valor de DWM_WINDOW_CORNER_PREFERENCE
    bool borderOn = true;
    COLORREF borderColor = RGB(0x7B, 0x8C, 0xFF);
    COLORREF borderColor2 = RGB(0xFF, 0x5C, 0xE1);
    int borderStyle = 0;  // 0 fixa, 1 pulsar, 2 arco-íris, 3 degradê
    double borderSpeed = 4.0;
    bool keepSizes = true;
    int borderWidth = 1;
    int borderGlow = 0;
    int borderRadius = 8;
    int borderOffset = 0;
};

static Config g_cfg;
static int g_layoutRt = -1;
static double g_mainFrac = 0;  // fração da janela principal ajustada pelo usuário (0 = padrão)  // layout escolhido pelo atalho (-1 = usar o das configurações)

static std::wstring GetStr(PCWSTR name) {
    std::wstring out;
    PCWSTR v = Wh_GetStringSetting(name);
    if (v) { out = v; Wh_FreeStringSetting(v); }
    return out;
}

static std::mutex g_ignMx;
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
    std::lock_guard<std::mutex> lk(g_ignMx);
    g_ignore = std::move(out);
}

static bool IsIgnored(HWND h, PCWSTR cls) {
    wchar_t title[256] = L"";
    GetWindowTextW(h, title, 256);
    std::wstring t = Lower(title), c = Lower(cls);
    std::lock_guard<std::mutex> lk(g_ignMx);
    for (auto& k : g_ignore)
        if (t.find(k) != std::wstring::npos || c.find(k) != std::wstring::npos) return true;
    return false;
}

static void LoadSettings() {
    LoadIgnoreList();
    Config c;
    c.gap = std::max(0, Wh_GetIntSetting(L"gap"));
    c.padding = std::max(0, Wh_GetIntSetting(L"padding"));
    c.mTop = 0;
    c.mBottom = std::max(0, Wh_GetIntSetting(L"marginBottom"));
    c.mLeft = c.mRight = 0;
    c.reserveTaskbar = false;  // área útil = área de trabalho do Windows + "Espaço embaixo"
    c.unmaximize = true;

    c.dropFloat = false;  // soltar uma janela sempre a devolve ao layout; para deixá-la solta use Alt+T
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
        c.debugLog = Wh_GetIntSetting(L"debugLog") != 0;
        c.launchSync = Wh_GetIntSetting(L"launchSync") != 0;
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

    std::wstring bc = GetStr(L"borderColor");
    c.borderOn = false;
    size_t k = 0;
    while (k < bc.size() && (bc[k] == L'#' || iswspace(bc[k]))) k++;
    if (bc.size() - k >= 6) {
        unsigned long v = wcstoul(bc.c_str() + k, nullptr, 16);
        c.borderColor = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
        c.borderOn = true;
    }
    {
        std::wstring st = GetStr(L"borderStyle");
        c.borderStyle = st == L"pulse" ? 1 : st == L"rainbow" ? 2 : st == L"gradient" ? 3 : 0;
        c.borderSpeed = std::max(0.5, (double)Wh_GetIntSetting(L"borderSpeed"));
        c.keepSizes = true;
        c.borderWidth = std::min(40, std::max(1, Wh_GetIntSetting(L"borderWidth")));
        c.borderGlow = std::min(60, std::max(0, Wh_GetIntSetting(L"borderGlow")));
        c.borderRadius = std::min(80, std::max(0, Wh_GetIntSetting(L"borderRadius")));
        c.borderOffset = std::min(30, std::max(-30, Wh_GetIntSetting(L"borderOffset")));
        std::wstring b2 = GetStr(L"borderColor2");
        size_t q = 0;
        while (q < b2.size() && (b2[q] == L'#' || iswspace(b2[q]))) q++;
        if (b2.size() - q >= 6) {
            unsigned long v = wcstoul(b2.c_str() + q, nullptr, 16);
            c.borderColor2 = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
        }
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
};

static std::vector<Item> g_items;
struct Pend { HWND first; ULONGLONG second; int tries = 0; };
static std::vector<Pend> g_pending;  // janelas novas aguardando "assentar"
static bool g_monocle = false;
static HWND g_dragHwnd = nullptr;

// Relógio de alta precisão (GetTickCount64 tem passo de ~15 ms e causa "degraus" na animação).
static double NowMs() {
    static LARGE_INTEGER freq = [] { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f; }();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)freq.QuadPart;
}  // janela sendo arrastada pelo usuário
static HWND g_borderHwnd = nullptr;

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static volatile bool g_stop = false;

static const UINT WM_APP_RELOAD = WM_APP + 1;
static const int HK_FLOAT = 1, HK_MONOCLE = 2, HK_LEFT = 3, HK_RIGHT = 4, HK_RETILE = 5, HK_LAYOUT = 6;

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
        if (!g_cfg.debugLog || GetWindowTextLengthW(h) == 0) return;
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

static void DrawOverlay(HWND t) {
    DWORD cl = 0;
    if (!t || !IsWindow(t) || !IsWindowVisible(t) || IsIconic(t) ||
        (SUCCEEDED(DwmGetWindowAttribute(t, DWMWA_CLOAKED, &cl, sizeof(cl))) && cl)) {
        HideOverlay();
        return;
    }
    RECT fr{};
    if (FAILED(DwmGetWindowAttribute(t, DWMWA_EXTENDED_FRAME_BOUNDS, &fr, sizeof(fr)))) GetWindowRect(t, &fr);
    int fw = fr.right - fr.left, fh = fr.bottom - fr.top;
    if (fw < 20 || fh < 20) { HideOverlay(); return; }

    const int w = g_cfg.borderWidth, glow = g_cfg.borderGlow;
    const int off = g_cfg.borderOffset;
    const int m = w + glow + 2 + std::abs(off);
    const int W = fw + 2 * m, H = fh + 2 * m;

    bool animated = g_cfg.borderStyle != 0;
    bool same = EqualRect(&fr, &g_ovLast) && t == g_ovLastTarget;
    if (same && !animated && IsWindowVisible(g_ov)) return;  // nada mudou
    if (!EnsureOverlay(W, H)) return;
    g_ovLast = fr;
    g_ovLastTarget = t;

    double r = (double)g_cfg.borderRadius;
    double ph = std::fmod(NowMs() / (g_cfg.borderSpeed * 1000.0), 1.0);
    double pulse = 1.0;
    if (g_cfg.borderStyle == 1) pulse = 0.35 + 0.65 * (0.5 - 0.5 * std::cos(ph * 6.283185307));
    const double cx = W / 2.0, cy = H / 2.0, hx = fw / 2.0, hy = fh / 2.0;
    const int sB = m + (int)r + 3;  // faixas que contêm a borda; o miolo da janela fica transparente

    auto px = [&](int x, int y) {
        double dx = std::fabs(x + 0.5 - cx) - (hx - r), dy = std::fabs(y + 0.5 - cy) - (hy - r);
        double ox = std::max(dx, 0.0), oy = std::max(dy, 0.0);
        double d = std::sqrt(ox * ox + oy * oy) + std::min(std::max(dx, dy), 0.0) - r - off;  // >0 = fora da linha de referência
        double a = 0;
        if (d > -1.0) {
            double cov1 = std::min(1.0, std::max(0.0, d + 0.5));
            double cov2 = std::min(1.0, std::max(0.0, w - d + 0.5));
            a = cov1 * cov2;
            if (glow > 0 && d > w) {
                double g = 1.0 - (d - w) / glow;
                if (g > 0) a = std::max(a, 0.45 * g * g * cov1);
            }
        }
        uint32_t out = 0;
        if (a > 0.002) {
            COLORREF c;
            double ang = std::atan2(y + 0.5 - cy, x + 0.5 - cx) / 6.283185307 + 0.5;  // 0..1 em volta da janela
            if (g_cfg.borderStyle == 2) c = HsvToRgb((ang + ph) * 360.0, 0.8, 1.0);
            else if (g_cfg.borderStyle == 3) {
                double k = 0.5 - 0.5 * std::cos((ang + ph) * 6.283185307 * 2.0);
                c = Lerp(g_cfg.borderColor, g_cfg.borderColor2, k);
            } else c = g_cfg.borderColor;
            a *= pulse;
            uint32_t A = (uint32_t)std::lround(a * 255.0);
            uint32_t R = (GetRValue(c) * A + 127) / 255, G = (GetGValue(c) * A + 127) / 255, B = (GetBValue(c) * A + 127) / 255;
            out = (A << 24) | (R << 16) | (G << 8) | B;
        }
        g_ovBits[(size_t)y * W + x] = out;
    };
    for (int y = 0; y < H; y++) {
        if (y < sB || y >= H - sB) {
            for (int x = 0; x < W; x++) px(x, y);
        } else {
            for (int x = 0; x < m + 3; x++) px(x, y);
            for (int x = W - m - 3; x < W; x++) px(x, y);
        }
    }

    POINT dst{fr.left - m, fr.top - m}, src{0, 0};
    SIZE sz{W, H};
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

static bool BorderAnimated() {
    if (!g_cfg.borderOn || !g_borderHwnd || !IsWindow(g_borderHwnd)) return false;
    return UseOverlay() || g_cfg.borderStyle != 0;
}

static void TickBorder() {
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
        a.left += g_cfg.padding; a.top += g_cfg.padding;
        a.right -= g_cfg.padding; a.bottom -= g_cfg.padding;
    }

    int gap = g_cfg.gap;
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
    if (it.minRead) return;
    it.minRead = true;
    MINMAXINFO mmi{};
    mmi.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
    mmi.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
    DWORD_PTR res = 0;
    if (SendMessageTimeoutW(it.h, WM_GETMINMAXINFO, 0, (LPARAM)&mmi, SMTO_ABORTIFHUNG | SMTO_BLOCK, 20, &res)) {
        it.minW = std::max<LONG>(it.minW, mmi.ptMinTrackSize.x);
        it.minH = std::max<LONG>(it.minH, mmi.ptMinTrackSize.y);
        if (g_cfg.debugLog) {
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
        if (g_cfg.debugLog) Wh_Log(L"Escala do monitor mudou (%u -> %u DPI): minimos convertidos para %ldx%ld", it.mDpi, cur, (long)std::max(it.minW, it.lW), (long)std::max(it.minH, it.lH));
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
            case EN_LEFT: OffsetRect(&from, -w / 6, 0); break;
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

// Área útil do monitor: a área de trabalho do Windows, menos a barra de tarefas (se ela não
// reservou espaço, como quando está escondida ou flutuante) e menos as margens configuradas.
static RECT UsableArea(HMONITOR m, const MONITORINFO& mi) {
    RECT w = mi.rcWork;
    if (g_cfg.reserveTaskbar) {
        auto consider = [&](HWND tb) {
            if (!tb || !IsWindowVisible(tb) || MonitorFromWindow(tb, MONITOR_DEFAULTTONULL) != m) return;
            RECT r;
            if (!GetWindowRect(tb, &r)) return;
            RECT x;
            if (!IntersectRect(&x, &r, &w)) return;
            LONG tw = r.right - r.left, th = r.bottom - r.top;
            LONG cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
            LONG mx = (mi.rcMonitor.left + mi.rcMonitor.right) / 2, my = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
            if (tw >= th) {
                if (cy > my) w.bottom = std::min(w.bottom, r.top);
                else w.top = std::max(w.top, r.bottom);
            } else {
                if (cx > mx) w.right = std::min(w.right, r.left);
                else w.left = std::max(w.left, r.right);
            }
        };
        consider(FindWindowW(L"Shell_TrayWnd", nullptr));
        HWND sec = nullptr;
        while ((sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) consider(sec);
    }
    // margens medidas da borda da tela: vale o maior entre elas e a barra de tarefas (não soma)
    w.left = std::max(w.left, mi.rcMonitor.left + g_cfg.mLeft);
    w.top = std::max(w.top, mi.rcMonitor.top + g_cfg.mTop);
    w.right = std::min(w.right, mi.rcMonitor.right - g_cfg.mRight);
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
        if (g_cfg.debugLog) {
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
            full.left += g_cfg.padding; full.top += g_cfg.padding;
            full.right -= g_cfg.padding; full.bottom -= g_cfg.padding;
            for (auto* it : list) if (it->h != g_dragHwnd) SetTarget(*it, full);
        } else {
            std::vector<SIZE> mins;
            for (auto* it : list) mins.push_back(SlotMin(*it));
            std::vector<std::pair<double, double>> wts;
            for (auto* it : list) wts.push_back({it->wx, it->wy});
            auto slots = ComputeSlots((int)list.size(), UsableArea(m, mi), mins, &wts);
            for (size_t i = 0; i < list.size(); i++) {
                if (g_cfg.debugLog) {
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
// Se não, reaplica (até 3 vezes) e, com o log ligado, registra o que ocorreu
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
        const RECT& t = it.to;
        bool bad = std::abs(r.left - t.left) > 4 || std::abs(r.top - t.top) > 4 ||
                   std::abs(r.right - t.right) > 4 || std::abs(r.bottom - t.bottom) > 4;
        if (!bad) { it.verifyAt = 0; continue; }
        if (g_cfg.debugLog) {
            wchar_t cls[64] = L"";
            GetClassNameW(it.h, cls, 64);
            MINMAXINFO mm{};
            mm.ptMaxTrackSize.x = mm.ptMaxTrackSize.y = -1;
            DWORD_PTR rr = 0;
            SendMessageTimeoutW(it.h, WM_GETMINMAXINFO, 0, (LPARAM)&mm, SMTO_ABORTIFHUNG | SMTO_BLOCK, 20, &rr);
            Wh_Log(L"Conferencia (%d) [%s]: esperado %ldx%ld em (%ld,%ld), real %ldx%ld em (%ld,%ld)%s (maximo do app %ldx%ld)",
                   it.verifyTries + 1, cls, t.right - t.left, t.bottom - t.top, t.left, t.top,
                   r.right - r.left, r.bottom - r.top, r.left, r.top,
                   IsZoomed(it.h) ? L" [janela maximizada]" : L"", (long)mm.ptMaxTrackSize.x, (long)mm.ptMaxTrackSize.y);
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
                if (g_cfg.debugLog) Wh_Log(L"Janela nao aceita ser posicionada: passa a flutuar (use o atalho de reorganizar para tentar de novo)");
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

// Detecta janelas que mudaram entre maximizada e normal e refaz a grade.
static void PollZoom() {
    bool changed = false;
    for (auto& it : g_items) {
        bool z = IsZoomed(it.h) != 0;
        if (z != it.zoomed) {
            it.zoomed = z;
            it.to = RECT{0, 0, 0, 0};
            changed = true;
        }
    }
    if (changed) Retile();
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
    if (g_cfg.debugLog) {
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
        it.zoomed = true;
        it.fresh = false;
        it.snap = false;
        if (g_cfg.unmaximize) ShowWindowAsync(h, SW_RESTORE);
    }
    ResetWeights();
    for (size_t i = 0; i < g_minimized.size();)
        if (g_minimized[i].h == h) g_minimized.erase(g_minimized.begin() + i);
        else i++;
    g_items.push_back(it);
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
    if (g_borderHwnd == h) g_borderHwnd = nullptr;
    if (g_dragHwnd == h) g_dragHwnd = nullptr;
    g_items.erase(g_items.begin() + i);
    ResetWeights();
    Retile();
}

// Troca duas janelas de posição na lista SEM carregar o estado de animação junto:
// cada janela recomeça a animação a partir de onde ela está de fato na tela.
static void SwapItems(size_t i, size_t j) {
    std::swap(g_items[i], g_items[j]);
    for (size_t k : {i, j}) {
        Item& it = g_items[k];
        GetWindowRect(it.h, &it.cur);
        it.anim = false;
        it.to = RECT{0, 0, 0, 0};
    }
}

static RECT g_dragStart{};
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
        if (IsZoomed(h) && g_cfg.unmaximize) {
            // o arrasto até a borda de cima maximizou a janela: devolve ao modo normal para ela voltar à grade
            if (g_cfg.debugLog) Wh_Log(L"Arrasto maximizou a janela: restaurando para voltar a grade");
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
    Retile();
}

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
        if (g_cfg.debugLog) {
            wchar_t cls[64] = L"", ttl[48] = L"";
            GetClassNameW(h, cls, 64);
            GetWindowTextW(h, ttl, 48);
            Wh_Log(L"Arrasto iniciado [%s] \"%s\" (%s) em (%ld,%ld) %ldx%ld%s", cls, ttl, g_dragEdge ? L"pela borda" : L"pelo meio", (long)g_dragStart.left, (long)g_dragStart.top,
                   (long)(g_dragStart.right - g_dragStart.left), (long)(g_dragStart.bottom - g_dragStart.top),
                   g_items[i].floating ? L" [flutuante]" : L"");
        }
        g_items[i].anim = false;
        g_dragHwnd = h;
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
    int gap = g_cfg.gap;
    RECT bb{LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN};
    for (size_t j : idx) {
        bb.left = std::min(bb.left, g_items[j].to.left); bb.top = std::min(bb.top, g_items[j].to.top);
        bb.right = std::max(bb.right, g_items[j].to.right); bb.bottom = std::max(bb.bottom, g_items[j].to.bottom);
    }
    const RECT t = me.to;
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
            const RECT& r = g_items[j].to;
            bool adj = rightEdge ? closeTo(r.left, t.right + gap, 8) : closeTo(r.right + gap, t.left, 8);
            if (!adj || !overlapV(r, t)) continue;
            if (master && layout == 4 && (j == mainIdx || i == mainIdx)) continue;
            double d = (double)delta * (rightEdge ? 1 : -1);
            // a largura é da coluna inteira: todas as janelas empilhadas nela mudam juntas
            for (size_t q : idx) {
                if (closeTo(g_items[q].to.left, t.left, 4)) g_items[q].wx = std::max(kMin, g_items[q].wx + d);
                else if (closeTo(g_items[q].to.left, r.left, 4)) g_items[q].wx = std::max(kMin, g_items[q].wx - d);
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
            const RECT& r = g_items[j].to;
            if (!overlapH(r, t)) continue;
            if (bottomEdge ? closeTo(r.top, t.bottom + gap, 8) : closeTo(r.bottom + gap, t.top, 8)) { nb = j; break; }
        }
        if (nb == (size_t)-1) {
            // layout de linhas (itens lado a lado): cai no comportamento por linha
            std::vector<size_t> mine, other;
            for (size_t j : idx) {
                const RECT& r = g_items[j].to;
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

static void OnMoveEnd(HWND h) {
    g_dragHwnd = nullptr;
    int i = FindItem(h);
    wchar_t cls[64] = L"", ttl[48] = L"";
    if (g_cfg.debugLog) {
        GetClassNameW(h, cls, 64);
        GetWindowTextW(h, ttl, 48);
    }
    if (i < 0) {
        // janela que ainda não estava na grade (ex.: já existia ao carregar o mod, ou foi ignorada antes):
        // se ela é elegível agora, passa a fazer parte
        bool ok = GetAncestor(h, GA_ROOT) == h && IsManageable(h) && OnAdd(h);
        if (g_cfg.debugLog) Wh_Log(L"Arrasto terminou em janela fora da grade [%s] \"%s\": %s", cls, ttl, ok ? L"adotada pela grade" : L"nao elegivel (veja 'Ignorada' acima)");
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
    if (g_cfg.debugLog && !g_dragEdge) {
        LONG dw = (nowR.right - nowR.left) - (g_dragStart.right - g_dragStart.left);
        LONG dh = (nowR.bottom - nowR.top) - (g_dragStart.bottom - g_dragStart.top);
        if (std::abs(dw) > 6 || std::abs(dh) > 6)
            Wh_Log(L"ATENCAO: o tamanho mudou durante um arrasto pelo meio (%ldx%ld -> %ldx%ld): outra coisa redimensionou a janela",
                   (long)(g_dragStart.right - g_dragStart.left), (long)(g_dragStart.bottom - g_dragStart.top),
                   (long)(nowR.right - nowR.left), (long)(nowR.bottom - nowR.top));
    }
    auto logEnd = [&](const wchar_t* decisao) {
        if (g_cfg.debugLog)
            Wh_Log(L"Arrasto terminou [%s] \"%s\" em (%ld,%ld) %ldx%ld%s: %s", cls, ttl, (long)nowR.left, (long)nowR.top,
                   (long)(nowR.right - nowR.left), (long)(nowR.bottom - nowR.top), IsZoomed(h) ? L" [maximizada]" : L"", decisao);
    };

    if (!g_items[i].floating && g_cfg.keepSizes && g_dragEdge) {
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
        // modo "deixar solta": se mexeu de verdade, vira flutuante
        if (g_cfg.dropFloat) {
            const RECT& t = g_items[i].to;
            int d = std::max(std::max(std::abs(nowR.left - t.left), std::abs(nowR.top - t.top)),
                             std::max(std::abs(nowR.right - t.right), std::abs(nowR.bottom - t.bottom)));
            if (d > 12) {
                g_items[i].floating = true;
                logEnd(L"ficou solta (flutuante)");
                return;
            }
        }
        logEnd(L"voltando ao layout");
    } else {
        logEnd(L"janela flutuante: ignorada pelo layout (Alt+T ou 'reorganizar tudo' a prende de novo)");
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
        case EVENT_OBJECT_HIDE:
        case EVENT_OBJECT_CLOAKED:
            OnRemove(hwnd);
            break;
        case EVENT_SYSTEM_FOREGROUND:
            UpdateBorder(hwnd);
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

static void SwapFocused(int dir) {
    HWND fg = GetForegroundWindow();
    int i = FindItem(fg);
    if (i < 0 || g_items[i].floating) return;
    int j = i;
    for (;;) {
        j += dir;
        if (j < 0 || j >= (int)g_items.size()) return;
        if (!g_items[j].floating && g_items[j].mon == g_items[i].mon) break;
    }
    SwapItems((size_t)i, (size_t)j);
    Retile();
}

static void OnHotkey(int id) {
    HWND fg = GetForegroundWindow();
    switch (id) {
        case HK_FLOAT: {
            int i = FindItem(fg);
            if (g_cfg.debugLog) {
                wchar_t cls[64] = L"", ttl[48] = L"";
                GetClassNameW(fg, cls, 64);
                GetWindowTextW(fg, ttl, 48);
                Wh_Log(L"Atalho soltar/prender: janela em foco [%s] \"%s\" %s", cls, ttl, i < 0 ? L"(fora da grade)" : (g_items[i].floating ? L"(solta)" : L"(na grade)"));
            }
            if (i < 0) {
                OnAdd(fg);
                i = FindItem(fg);
                if (i < 0) {
                    if (g_cfg.debugLog) Wh_Log(L"Atalho soltar/prender: essa janela nao e elegivel para a grade (veja 'Ignorada' no log)");
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
        case HK_LEFT: SwapFocused(-1); break;
        case HK_RIGHT: SwapFocused(+1); break;
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

// Estatísticas (só gravadas no log com a opção "Registrar estatísticas" ligada)
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
    if (g_stats.frames > 1 && g_cfg.debugLog) {
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
            if (g_cfg.unmaximize) ShowWindowAsync(h, SW_RESTORE);
        }
        if (g_cfg.debugLog) {
            wchar_t cls[64] = L"", ttl[48] = L"";
            GetClassNameW(h, cls, 64);
            GetWindowTextW(h, ttl, 48);
            Wh_Log(L"Janela ja aberta [%s] \"%s\"%s", cls, ttl, it.zoomed ? L" (estava maximizada)" : L"");
        }
        g_items.push_back(it);
    }
    return TRUE;
}

static DWORD WINAPI Worker(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // cria a fila de mensagens

    // só a instância do explorer.exe que é dona da barra de tarefas deve rodar
    DWORD pid = 0;
    for (int i = 0; i < 120 && !g_stop; i++) {
        HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tb) { GetWindowThreadProcessId(tb, &pid); break; }
        Sleep(500);
    }
    if (g_stop || pid != GetCurrentProcessId()) return 0;

    HWINEVENTHOOK hooks[5] = {};
    hooks[0] = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[1] = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[2] = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[3] = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);  // DESTROY, SHOW, HIDE
    hooks[4] = SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    std::wstring hk = GetStr(L"hotkeys");
    UINT mods = MOD_WIN | MOD_ALT | MOD_NOREPEAT;
    if (hk == L"ctrlalt") mods = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
    else if (hk == L"altshift") mods = MOD_ALT | MOD_SHIFT | MOD_NOREPEAT;
    else if (hk == L"ctrlaltshift") mods = MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT;
    struct HK { int id; UINT mods, vk; };
    std::vector<HK> keys = {{HK_MONOCLE, mods, 'F'}, {HK_LEFT, mods, VK_LEFT}, {HK_RIGHT, mods, VK_RIGHT}, {HK_RETILE, mods, VK_DOWN}};
    {
        UINT m2, k2;
        if (ParseHotkey(GetStr(L"layoutHotkey"), &m2, &k2)) keys.push_back({HK_LAYOUT, m2, k2});
        else if (!GetStr(L"layoutHotkey").empty()) Wh_Log(L"Atalho de layout invalido: use algo como Alt+G");
        if (ParseHotkey(GetStr(L"floatHotkey"), &m2, &k2)) keys.push_back({HK_FLOAT, m2, k2});
        else if (!GetStr(L"floatHotkey").empty()) Wh_Log(L"Atalho de soltar janela invalido: use algo como Alt+T");
    }
    for (auto& k : keys) {
        if (RegisterHotKey(nullptr, k.id, k.mods, k.vk)) {
            if (g_cfg.debugLog) Wh_Log(L"Atalho registrado (id %d, tecla %u, modificadores %x)", k.id, k.vk, k.mods & 0xff);
        } else
            Wh_Log(L"Atalho (tecla %u) indisponivel (erro %lu; 1409 = ja registrado por outro programa/instancia)",
                   k.vk, GetLastError());
    }

    UpdateInterval();
    g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!g_frameTimer) g_frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);

    EnumWindows(EnumProc, 0);
    Retile();
    UpdateBorder(GetForegroundWindow());

    // Loop: quando há animação, cada quadro é sincronizado com a atualização da tela (DwmFlush);
    // quando não há, a thread dorme até chegar uma mensagem.
    while (!g_stop) {
        double tA = NowMs();
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { g_stop = true; break; }
            if (msg.message == WM_HOTKEY) {
                OnHotkey((int)msg.wParam);
            } else if (msg.message == WM_APP_RELOAD) {
                UpdateInterval();
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

        static double lastPoll = 0;
        if (NowMs() - lastPoll > 250) { lastPoll = NowMs(); PollZoom(); }
        double tB = NowMs();
        ProcessPending();
        double tC = NowMs();
        bool verifyPending = VerifyItems();
        double tD = NowMs();
        TickBorder();
        RunScheduledRetile();
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
            DWORD wait = (g_pending.empty() && !verifyPending) ? (g_items.empty() ? INFINITE : 250) : 20;
            if (BorderAnimated()) wait = std::min<DWORD>(wait, 16);
            if (g_retileAt) wait = std::min<DWORD>(wait, 15);
            MsgWaitForMultipleObjectsEx(0, nullptr, wait, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        }
    }

    // limpeza
    if (g_frameTimer) { CloseHandle(g_frameTimer); g_frameTimer = nullptr; }
    for (auto& k : keys) UnregisterHotKey(nullptr, k.id);
    for (auto h : hooks) if (h) UnhookWinEvent(h);
    for (auto& it : g_items) {
        SetBorder(it.h, false);
        if (it.cornerApplied >= 0 && it.cornerApplied != it.origCorner) {
            DWORD v = (DWORD)it.origCorner;
            DwmSetWindowAttribute(it.h, 33, &v, sizeof(v));
        }
    }
    DestroyOverlay();
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
// Pontos de entrada do Windhawk
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Adaptive Windows 1.0.0 carregado");
    LoadSettings();
    g_stop = false;
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_thread) PostThreadMessageW(g_threadId, WM_APP_RELOAD, 0, 0);
}

void Wh_ModUninit() {
    g_stop = true;
    if (g_thread) {
        for (int i = 0; i < 30; i++) {
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
            if (WaitForSingleObject(g_thread, 100) == WAIT_OBJECT_0) break;
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}
