// ==WindhawkMod==
// @id              nimbus-menu
// @name            Nimbus Menu
// @description     Replaces the Windows taskbar with a light dock and its own Start menu (also on the Windows key): smoked glass, macOS-like zoom, live window previews, one bar per monitor, file search with Everything
// @description:pt-BR Troca a barra de tarefas do Windows por uma dock leve com Iniciar próprio (também na tecla Windows): vidro fumê, zoom como no macOS, prévia das janelas ao vivo, uma barra por monitor, busca de arquivos com o Everything
// @version         1.0.0
// @author          igoredm
// @github          https://github.com/igoredm
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lgdi32 -ld2d1 -ldwrite -lwindowscodecs -ldwmapi -lshell32 -lshlwapi -lshcore -lpropsys -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Nimbus Menu

Replaces the Windows taskbar with a light dock, one per monitor, with its own Start menu.

* Floating dock or full-width bar; 4 looks (Névoa, Cristal, Grafite, Aurora).
* macOS-like zoom (or Lift / Focus), live window previews (hover a thumbnail to peek), drag icons to reorder, pin and unpin.
* Its own Start: pinned apps, all apps, app search, files (with [Everything](https://www.voidtools.com) or Windows Search) and Windows Settings pages. Compact or wide search.
* The Windows key opens Nimbus' Start over any window (administrator ones too) and in games. Ctrl+Esc still opens Windows' Start. An optional shortcut (Alt+Space or Ctrl+Space) can open it too.
* Per-monitor settings: format, auto-hide, what shows, only that monitor's windows.
* The dock runs in its own Windhawk process and hides when a game or video is in full screen. Explorer only gets one small hook, for the Windows key.
* The text is in English, or in Portuguese when Windows is in Portuguese.

**What it changes in Windows.** With "Hide the Windows taskbar" on (the default), the whole Windows taskbar is hidden, so its tray, clock and quick settings are gone too: use another mod or app for those if you need them. To stop the hidden taskbar from reserving screen space, the mod turns on the taskbar's own "Automatically hide the taskbar" option, which Windows saves in your profile. The mod remembers your original choice and puts it back when it's turned off, also after a crash or a shutdown with the mod on. If the Windows taskbar ever stays hidden, turn the mod off or restart Explorer.

---

**Português**

Troca a barra de tarefas do Windows por uma dock leve, uma por monitor, com Iniciar próprio.

* Dock flutuante ou barra fixa; 4 visuais (Névoa, Cristal, Grafite, Aurora).
* Zoom como no macOS (ou Elevar / Foco), prévia das janelas ao vivo (parar o mouse na miniatura mostra a janela), arrastar ícones pra reorganizar, fixar e desafixar.
* Iniciar próprio: fixados, todos os apps, busca de apps, arquivos (com o [Everything](https://www.voidtools.com) ou a busca do Windows) e telas das Configurações do Windows. Busca compacta ou larga.
* A tecla Windows abre o Iniciar do Nimbus em qualquer janela (até de administrador) e em jogos. Ctrl+Esc continua abrindo o Iniciar do Windows. Um atalho opcional (Alt+Espaço ou Ctrl+Espaço) também pode abrir.
* Ajustes por monitor: formato, esconder sozinha, o que aparece, só as janelas daquele monitor.
* A dock roda num processo próprio do Windhawk e some quando um jogo ou vídeo está em tela cheia. No Explorador entra só um gancho pequeno, o da tecla Windows.
* Os textos ficam em inglês, ou em português quando o Windows está em português.

**O que muda no Windows.** Com "Esconder a barra do Windows" ligado (o padrão), a barra do Windows some inteira, então a bandeja, o relógio e as configurações rápidas somem junto: use outro mod ou app pra isso se precisar. Pra barra escondida não reservar espaço na tela, o mod liga a opção "Ocultar automaticamente a barra de tarefas" do próprio Windows, que fica salva no seu perfil. O mod guarda a sua escolha original e devolve quando é desligado, até depois de um travamento ou de desligar o PC com o mod ligado. Se a barra do Windows ficar escondida, desligue o mod ou reinicie o Explorador.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: nevoa
  $name: Look
  $name:pt-BR: Visual
  $options:
  - nevoa: Névoa (smoked glass)
  - cristal: Cristal (light glass)
  - grafite: Grafite (solid black, the lightest)
  - aurora: Aurora (magenta to orange edge)
  $options:pt-BR:
  - nevoa: Névoa (vidro fumê)
  - cristal: Cristal (vidro claro)
  - grafite: Grafite (preto sólido, o mais leve)
  - aurora: Aurora (borda magenta → laranja)
- format: dock
  $name: Format
  $name:pt-BR: Formato
  $options:
  - dock: Floating dock (a pill off the screen edge)
  - fixa: Full-width bar (along the bottom)
  $options:pt-BR:
  - dock: Dock flutuante (pílula solta da borda)
  - fixa: Barra fixa (largura toda, colada embaixo)
- animation: zoom
  $name: Hover animation
  $name:pt-BR: Animação ao passar o mouse
  $options:
  - zoom: macOS zoom
  - elevar: Lift (the icon rises with a shadow)
  - foco: Focus (the icon grows, the others dim)
  - nenhuma: None
  $options:pt-BR:
  - zoom: Zoom do macOS
  - elevar: Elevar (o ícone sobe com uma sombra)
  - foco: Foco (o ícone cresce, os outros apagam)
  - nenhuma: Nenhuma
- showStart: true
  $name: Show the Start button
  $name:pt-BR: Mostrar o botão Iniciar
- startMenu: nevoa
  $name: Start button opens
  $name:pt-BR: Iniciar ao clicar no botão
  $options:
  - nevoa: Nimbus' Start (search with Everything)
  - windows: Windows' Start
  $options:pt-BR:
  - nevoa: Iniciar do Nimbus (busca com o Everything)
  - windows: Iniciar do Windows
- winKeyMode: nevoa
  $name: Windows key opens
  $name:pt-BR: Tecla Windows abre
  $description: Ctrl+Esc always opens Windows' Start
  $description:pt-BR: Ctrl+Esc sempre abre o Iniciar do Windows
  $options:
  - nevoa: Nimbus' Start
  - windows: Windows' Start
  $options:pt-BR:
  - nevoa: Iniciar do Nimbus
  - windows: Iniciar do Windows
- startHotkey: none
  $name: Extra shortcut for Nimbus' Start
  $name:pt-BR: Atalho extra do Iniciar do Nimbus
  $description: >-
    The Windows key already opens it. Each shortcut is taken from every app while the mod runs: Alt+Space replaces the window menu (Move, Size, Minimize, Close), Ctrl+Space the input method switch and many editors' autocomplete
  $description:pt-BR: >-
    A tecla Windows já abre. O atalho deixa de funcionar em todos os apps enquanto o mod roda: Alt+Espaço substitui o menu da janela (Mover, Tamanho, Minimizar, Fechar), Ctrl+Espaço a troca de método de entrada e o autocompletar de muitos editores
  $options:
  - altSpace: Alt + Space
  - ctrlSpace: Ctrl + Space
  - none: None
  $options:pt-BR:
  - altSpace: Alt + Espaço
  - ctrlSpace: Ctrl + Espaço
  - none: Nenhum
- searchLayout: larga
  $name: Search in Start
  $name:pt-BR: Busca no Iniciar
  $options:
  - compacta: Compact (a list inside Start)
  - larga: Wide (Start widens, with an item panel and Windows Settings pages)
  $options:pt-BR:
  - compacta: Compacta (lista no próprio Iniciar)
  - larga: Larga (o Iniciar se alarga, com painel do item e configurações do Windows)
- fileSearch: everything
  $name: File search in Start
  $name:pt-BR: Busca de arquivos no Iniciar
  $description: Everything (free, voidtools.com) finds any file instantly. Without it, Windows Search opens in Explorer
  $description:pt-BR: O Everything (grátis, voidtools.com) acha qualquer arquivo na hora. Sem ele, a busca do Windows abre no Explorador
  $options:
  - everything: Everything
  - windows: Windows Search (in Explorer)
  - off: "Off (apps only)"
  $options:pt-BR:
  - everything: Everything
  - windows: Busca do Windows (no Explorador)
  - off: Desligada (só apps)
- iconSize: 60
  $name: Icon size (px)
  $name:pt-BR: Tamanho dos ícones (px)
- iconGap: 18
  $name: Space between icons (px)
  $name:pt-BR: Espaço entre os ícones (px)
- magnification: 150
  $name: Hover zoom (%)
  $name:pt-BR: Zoom ao passar o mouse (%)
  $description: 100 turns the zoom off
  $description:pt-BR: 100 desliga o zoom
- autoHide: true
  $name: Auto-hide
  $name:pt-BR: Esconder sozinha
  $description: Shows up when the mouse touches the bottom edge
  $description:pt-BR: Aparece quando o mouse encosta na borda de baixo
- hideDelay: 400
  $name: Delay before hiding (ms)
  $name:pt-BR: Tempo até esconder (ms)
- bottomGap: 12
  $name: Distance from the bottom edge (px)
  $name:pt-BR: Distância da borda de baixo (px)
- tintColor: ""
  $name: Glass color (RRGGBB)
  $name:pt-BR: Cor do vidro (RRGGBB)
  $description: Empty uses the chosen look's color
  $description:pt-BR: Vazio usa a cor do visual escolhido
- tintOpacity: 62
  $name: Glass color strength (%)
  $name:pt-BR: Força da cor do vidro (%)
  $description: Not used by Grafite, which is solid
  $description:pt-BR: Não vale pro Grafite, que é sólido
- hideWindowsTaskbar: true
  $name: Hide the Windows taskbar
  $name:pt-BR: Esconder a barra do Windows
  $description: Also turns on its auto-hide while the mod runs, so it doesn't reserve screen space (your original choice comes back when the mod is turned off)
  $description:pt-BR: Também liga o ocultar automático dela enquanto o mod roda, pra não reservar espaço na tela (sua escolha original volta quando o mod é desligado)
- monPrimary:
  - enabled: true
    $name: Show the bar on this monitor
    $name:pt-BR: Mostrar a barra neste monitor
  - format: geral
    $name: Format
    $name:pt-BR: Formato
    $options:
    - geral: Same as the general format
    - dock: Floating dock
    - fixa: Full-width bar
    $options:pt-BR:
    - geral: Igual ao formato geral
    - dock: Dock flutuante
    - fixa: Barra fixa
  - autoHide: geral
    $name: Auto-hide
    $name:pt-BR: Esconder sozinha
    $options:
    - geral: Same as the general setting
    - sim: "Yes"
    - nao: "No"
    $options:pt-BR:
    - geral: Igual ao ajuste geral
    - sim: Sim
    - nao: Não
  - showStart: true
    $name: Start button
    $name:pt-BR: Botão Iniciar
  - showPinned: true
    $name: Pinned apps
    $name:pt-BR: Apps fixados
  - windows: todas
    $name: Open windows
    $name:pt-BR: Janelas abertas
    $options:
    - todas: All of them
    - deste: Only the ones on this monitor
    $options:pt-BR:
    - todas: Todas
    - deste: Só as deste monitor
  $name: Primary monitor
  $name:pt-BR: Monitor principal
- monLeft:
  - enabled: true
    $name: Show the bar on this monitor
    $name:pt-BR: Mostrar a barra neste monitor
  - format: geral
    $name: Format
    $name:pt-BR: Formato
    $options:
    - geral: Same as the general format
    - dock: Floating dock
    - fixa: Full-width bar
    $options:pt-BR:
    - geral: Igual ao formato geral
    - dock: Dock flutuante
    - fixa: Barra fixa
  - autoHide: geral
    $name: Auto-hide
    $name:pt-BR: Esconder sozinha
    $options:
    - geral: Same as the general setting
    - sim: "Yes"
    - nao: "No"
    $options:pt-BR:
    - geral: Igual ao ajuste geral
    - sim: Sim
    - nao: Não
  - showStart: true
    $name: Start button
    $name:pt-BR: Botão Iniciar
  - showPinned: true
    $name: Pinned apps
    $name:pt-BR: Apps fixados
  - windows: todas
    $name: Open windows
    $name:pt-BR: Janelas abertas
    $options:
    - todas: All of them
    - deste: Only the ones on this monitor
    $options:pt-BR:
    - todas: Todas
    - deste: Só as deste monitor
  $name: Monitor left of the primary one
  $name:pt-BR: Monitor à esquerda do principal
- monRight:
  - enabled: true
    $name: Show the bar on this monitor
    $name:pt-BR: Mostrar a barra neste monitor
  - format: geral
    $name: Format
    $name:pt-BR: Formato
    $options:
    - geral: Same as the general format
    - dock: Floating dock
    - fixa: Full-width bar
    $options:pt-BR:
    - geral: Igual ao formato geral
    - dock: Dock flutuante
    - fixa: Barra fixa
  - autoHide: geral
    $name: Auto-hide
    $name:pt-BR: Esconder sozinha
    $options:
    - geral: Same as the general setting
    - sim: "Yes"
    - nao: "No"
    $options:pt-BR:
    - geral: Igual ao ajuste geral
    - sim: Sim
    - nao: Não
  - showStart: true
    $name: Start button
    $name:pt-BR: Botão Iniciar
  - showPinned: true
    $name: Pinned apps
    $name:pt-BR: Apps fixados
  - windows: todas
    $name: Open windows
    $name:pt-BR: Janelas abertas
    $options:
    - todas: All of them
    - deste: Only the ones on this monitor
    $options:pt-BR:
    - todas: Todas
    - deste: Só as deste monitor
  $name: Monitor right of the primary one
  $name:pt-BR: Monitor à direita do principal
- monOther:
  - enabled: true
    $name: Show the bar on this monitor
    $name:pt-BR: Mostrar a barra neste monitor
  - format: geral
    $name: Format
    $name:pt-BR: Formato
    $options:
    - geral: Same as the general format
    - dock: Floating dock
    - fixa: Full-width bar
    $options:pt-BR:
    - geral: Igual ao formato geral
    - dock: Dock flutuante
    - fixa: Barra fixa
  - autoHide: geral
    $name: Auto-hide
    $name:pt-BR: Esconder sozinha
    $options:
    - geral: Same as the general setting
    - sim: "Yes"
    - nao: "No"
    $options:pt-BR:
    - geral: Igual ao ajuste geral
    - sim: Sim
    - nao: Não
  - showStart: true
    $name: Start button
    $name:pt-BR: Botão Iniciar
  - showPinned: true
    $name: Pinned apps
    $name:pt-BR: Apps fixados
  - windows: todas
    $name: Open windows
    $name:pt-BR: Janelas abertas
    $options:
    - todas: All of them
    - deste: Only the ones on this monitor
    $options:pt-BR:
    - todas: Todas
    - deste: Só as deste monitor
  $name: Monitors above or below the primary one
  $name:pt-BR: Monitores acima ou abaixo do principal
*/
// ==/WindhawkModSettings==
#include <windows.h>
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
#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;

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
    std::wstring fileSearch = L"everything";  // everything | windows | off
    bool wideSearch = false;            // searchLayout: larga
    std::wstring animation = L"zoom";   // zoom | elevar | foco | nenhuma
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

static void BuildTheme() {
    float op = g_set.tintOpacity / 100.0f;
    Theme t;
    if (g_set.style == L"cristal") {
        t = {Rgba(236, 238, 244, op * 0.9f), true, false,
             {Rgba(255, 255, 255, 0.95f), Rgba(255, 255, 255, 0.45f), Rgba(255, 255, 255, 0.6f)},
             Rgba(27, 27, 31, 0.9f), Rgba(245, 246, 250, 0.94f), Rgba(27, 27, 31, 1), Rgba(0, 0, 0, 0.08f),
             Rgba(27, 27, 31, 1), Rgba(0, 0, 0, 0.14f), Rgba(0, 0, 0, 0.07f), Rgba(27, 27, 31, 1)};
    } else if (g_set.style == L"grafite") {
        t = {Rgba(14, 14, 16, 1), false, false, {Rgba(38, 38, 43, 1), Rgba(38, 38, 43, 1), Rgba(38, 38, 43, 1)},
             Rgba(232, 232, 236, 0.9f), Rgba(14, 14, 16, 1), Rgba(232, 232, 236, 1), Rgba(29, 29, 33, 1),
             Rgba(232, 232, 236, 1), Rgba(42, 42, 48, 1), Rgba(255, 255, 255, 0.07f), Rgba(232, 232, 236, 1)};
    } else if (g_set.style == L"aurora") {
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
    if (g_set.customTint && g_set.style != L"grafite") {
        t.tint = Rgba(GetRValue(g_set.tint), GetGValue(g_set.tint), GetBValue(g_set.tint), t.tint.a);
    }
    g_theme = t;
}

static int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

#define Log(...) Wh_Log(__VA_ARGS__)

// ---------------------------------------------------------------------------
// Text shown by the mod: English, or Portuguese when Windows' display language is Portuguese.
// ---------------------------------------------------------------------------

enum StrId {
    S_Start, S_BarSettings, S_TaskManager, S_NewWindow, S_PinDock, S_UnpinDock, S_CloseAll, S_CloseWindow,
    S_PinStart, S_UnpinStart, S_SearchHint, S_Lock, S_Sleep, S_Restart, S_ShutDown, S_KeysHint, S_TabPinned,
    S_TabAll, S_CountPinned, S_CountApps, S_PinHint, S_Apps, S_NoApps, S_BestMatch, S_FilesFolders,
    S_SearchFilesPre, S_SearchFilesPost, S_OpenEverything, S_ExplorerSearch, S_GetEverything, S_TypeMore,
    S_NoFiles, S_Settings, S_KindSetting, S_KindFolder, S_KindFile, S_KindApp, S_Open, S_OpenLocation,
    S_Count
};

static const wchar_t* const kStrEn[S_Count] = {
    L"Start", L"Dock settings (Windhawk)", L"Task Manager", L"Open new window", L"Pin to dock",
    L"Unpin from dock", L"Close all windows", L"Close window", L"Pin to Start", L"Unpin from Start",
    L"Search apps and files", L"Lock", L"Sleep", L"Restart", L"Shut down",
    L"↑ ↓ to choose     Enter opens     Esc clears the search", L"Pinned", L"All apps", L"%d pinned",
    L"%d apps", L"Right-click an app in \"All apps\" to pin it here", L"Apps", L"No app with that name",
    L"Best match", L"Files and folders", L"Search files for “", L"” (File Explorer)",
    L"Open Everything to search files", L"Search files with File Explorer",
    L"Get Everything (free, instant search)", L"Type at least 2 letters", L"No files", L"Settings",
    L"Windows Settings", L"Folder", L"File", L"App", L"Open", L"Open file location",
};

static const wchar_t* const kStrPt[S_Count] = {
    L"Iniciar", L"Configurações da barra (Windhawk)", L"Gerenciador de Tarefas", L"Abrir nova janela",
    L"Fixar na barra", L"Desafixar da barra", L"Fechar todas as janelas", L"Fechar janela", L"Fixar no Iniciar",
    L"Desafixar do Iniciar", L"Buscar apps e arquivos", L"Bloquear", L"Suspender", L"Reiniciar", L"Desligar",
    L"↑ ↓ pra escolher     Enter abre     Esc limpa a busca", L"Fixados", L"Todos os apps",
    L"%d fixados", L"%d apps", L"Clique com o botão direito num app em \"Todos os apps\" pra fixar aqui", L"Apps",
    L"Nenhum app com esse nome", L"Melhor resultado", L"Arquivos e pastas", L"Buscar “",
    L"” nos arquivos (Explorador)", L"Abrir o Everything pra buscar arquivos",
    L"Buscar nos arquivos pelo Explorador", L"Baixar o Everything (grátis, busca na hora)",
    L"Digite pelo menos 2 letras", L"Nenhum arquivo", L"Configurações", L"Configurações do Windows", L"Pasta",
    L"Arquivo", L"App", L"Abrir", L"Abrir o local do arquivo",
};

static bool IsPortugueseUi() {
    static int pt = -1;
    if (pt < 0) pt = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE;
    return pt == 1;
}

static const wchar_t* Tr(StrId id) { return (IsPortugueseUi() ? kStrPt : kStrEn)[id]; }

void LoadSettings() {
    g_set.iconSize = Clamp(Wh_GetIntSetting(L"iconSize"), 24, 128);
    g_set.iconGap = Clamp(Wh_GetIntSetting(L"iconGap"), 0, 64);
    g_set.magnification = Clamp(Wh_GetIntSetting(L"magnification"), 100, 250);
    g_set.autoHide = Wh_GetIntSetting(L"autoHide") != 0;
    g_set.hideDelay = Clamp(Wh_GetIntSetting(L"hideDelay"), 0, 5000);
    g_set.bottomGap = Clamp(Wh_GetIntSetting(L"bottomGap"), 0, 100);
    g_set.tintOpacity = Clamp(Wh_GetIntSetting(L"tintOpacity"), 0, 100);
    g_set.hideTaskbar = Wh_GetIntSetting(L"hideWindowsTaskbar") != 0;
    g_set.showStart = Wh_GetIntSetting(L"showStart") != 0;
    using WindhawkUtils::StringSetting;
    auto str = [](const wchar_t* name, const wchar_t* def) {
        auto s = StringSetting::make(name);
        return std::wstring(*s.get() ? s.get() : def);
    };
    g_set.style = str(L"style", L"nevoa");
    g_set.fixedBar = str(L"format", L"dock") == L"fixa";
    g_set.nevoaStart = str(L"startMenu", L"nevoa") != L"windows";
    g_set.winKey = str(L"winKeyMode", L"nevoa") == L"nevoa";
    g_set.startHotkey = str(L"startHotkey", L"none");
    g_set.fileSearch = str(L"fileSearch", L"everything");
    g_set.wideSearch = str(L"searchLayout", L"larga") == L"larga";
    g_set.animation = str(L"animation", L"zoom");
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
    g_winCache[hwnd] = info;
    return info;
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
        wchar_t exp[MAX_PATH * 2] = {};
        ExpandEnvironmentStringsW(buf, exp, ARRAYSIZE(exp));
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
        add(L"SystemDrive", L"\\");
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
    if (pid == GetCurrentProcessId()) return false;
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

static ComPtr<IWICBitmapSource> LoadAppIcon(App& a) {
    ComPtr<IShellItem> item;
    if (!a.lnk.empty()) SHCreateItemFromParsingName(a.lnk.c_str(), nullptr, IID_PPV_ARGS(&item));
    if (!item && !a.aumid.empty()) {
        std::wstring p = L"shell:AppsFolder\\" + a.aumid;
        SHCreateItemFromParsingName(p.c_str(), nullptr, IID_PPV_ARGS(&item));
    }
    if (!item && !a.exe.empty()) SHCreateItemFromParsingName(a.exe.c_str(), nullptr, IID_PPV_ARGS(&item));
    if (!item) return {};
    // The shell's display name is the translated one ("Explorador de Arquivos", not "File Explorer").
    if (a.label.empty() || !a.lnk.empty()) {
        PWSTR name = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && name) {
            a.label = name;
            if (a.label.size() > 4 && EqI(a.label.substr(a.label.size() - 4), L".exe")) a.label.resize(a.label.size() - 4);
            CoTaskMemFree(name);
        }
    }
    ComPtr<IShellItemImageFactory> factory;
    if (FAILED(item.As(&factory))) return {};
    HBITMAP hbm = nullptr;
    SIZE sz = {128, 128};
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

static DockCfg LoadDockCfg(const wchar_t* group) {
    DockCfg c;
    c.enabled = Wh_GetIntSetting(L"%s.enabled", group) != 0;
    PCWSTR f = Wh_GetStringSetting(L"%s.format", group);
    c.fixed = (f && !wcscmp(f, L"fixa")) || ((!f || !*f || !wcscmp(f, L"geral")) && g_set.fixedBar);
    if (f) Wh_FreeStringSetting(f);
    PCWSTR h = Wh_GetStringSetting(L"%s.autoHide", group);
    c.autoHide = (h && !wcscmp(h, L"sim")) ? true : (h && !wcscmp(h, L"nao")) ? false : g_set.autoHide;
    if (h) Wh_FreeStringSetting(h);
    c.showStart = Wh_GetIntSetting(L"%s.showStart", group) != 0 && g_set.showStart;
    c.showPinned = Wh_GetIntSetting(L"%s.showPinned", group) != 0;
    PCWSTR w = Wh_GetStringSetting(L"%s.windows", group);
    c.onlyThis = w && !wcscmp(w, L"deste");
    if (w) Wh_FreeStringSetting(w);
    return c;
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
    D2D1_RECT_F pill{};
    bool appbar = false;  // fixed bar, always shown: reserves its strip so maximized windows stop above it
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
constexpr int kBandSteps = 5;  // blur bands per rounded end (2 * steps + 1 windows per dock)

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

void RenderDock(Dock& d);
void PlaceDock(Dock& d);

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
    float avail = d.cw - 32 * d.scale;
    d.fit = need > avail ? std::max(0.4f, avail / need) : 1.0f;
}

static void ComputeTargets(Dock& d) {
    float slot = S(d) + Gap(d), sepW = Gap(d);
    float rest = 0;
    for (auto& el : d.elems) rest += el.app < 0 ? sepW : slot;
    float x = d.cw / 2.0f - rest / 2.0f;
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
    if (!g_launch.empty() && d.visible) moving = true;
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
    float x = d.cw / 2.0f - total / 2.0f;
    for (auto& el : d.elems) {
        el.x = x;
        el.w = (el.app < 0 ? sepW : slot * el.m) * el.appear;
        x += el.w;
    }
    float bottom = d.ch - BottomGap(d);
    if (d.cfg.fixed) d.pill = D2D1::RectF(0, bottom - PillH(d), (float)d.cw, bottom);
    else d.pill = D2D1::RectF(d.cw / 2.0f - total / 2.0f - padX, bottom - PillH(d), d.cw / 2.0f + total / 2.0f + padX, bottom);
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
        a.icon = LoadAppIcon(a);
        a.iconTried = true;
    }
    ComPtr<ID2D1Bitmap> bmp;
    if (a.icon) d.rt->CreateBitmapFromWicBitmap(a.icon.Get(), nullptr, &bmp);
    d.bmps[a.key] = bmp;
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

    PlaceBands(d.bands, d.bandRc, g_theme.blur, d.rcMon.left + d.pill.left, top + d.pill.top,
               d.rcMon.left + d.pill.right, top + d.pill.bottom, PillRadius(d));
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

static void Activate(HWND h) {
    if (IsIconic(h)) ShowWindowAsync(h, SW_RESTORE);
    SwitchToThisWindow(h, TRUE);
}

static void Animate(Dock& d);
static void Launch(const App& a) {
    g_launch[a.key] = GetTickCount();
    for (auto* d : g_docks) Animate(*d);
    std::wstring target;
    if (!a.lnk.empty()) target = a.lnk;
    else if (!a.aumid.empty()) target = L"shell:AppsFolder\\" + a.aumid;
    else target = a.exe;
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
    HWND fg = GetForegroundWindow();
    auto it = std::find(wins.begin(), wins.end(), fg);
    if (it != wins.end()) {
        if (wins.size() == 1) PostMessageW(fg, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        else {
            // Several windows: go to the next one (the last in z-order, so it cycles).
            Activate(wins.back());
        }
        return;
    }
    Activate(wins.front());
}

static void TogglePin(const std::wstring& key) {
    for (auto& a : g_apps) {
        if (a.key != key || a.start) continue;
        if (a.pinned) {
            if (a.pinIndex >= 0 && a.pinIndex < (int)g_pins.size()) g_pins.erase(g_pins.begin() + a.pinIndex);
        } else if (!a.aumid.empty()) {
            g_pins.push_back({L"aumid", a.aumid});
        } else if (!a.exe.empty()) {
            g_pins.push_back({L"exe", a.exe});
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
        } else if (!a.aumid.empty()) {
            moving = {L"aumid", a.aumid};
        } else if (!a.exe.empty()) {
            moving = {L"exe", a.exe};
        } else {
            return;
        }
        ins = std::clamp(ins, 0, (int)g_pins.size());
        g_pins.insert(g_pins.begin() + ins, moving);
    }
    SavePins();
    RebuildApps();
    for (auto* x2 : g_docks) { BuildElems(*x2, true); Invalidate(*x2); }
}

static void OpenWindhawk() {
    wchar_t path[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (!n || n == MAX_PATH) return;
    PathRemoveFileSpecW(path);
    PathAppendW(path, L"windhawk.exe");
    ShellExecuteW(nullptr, L"open", path, nullptr, nullptr, SW_SHOWNORMAL);
}

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
    if (cmd == 1) OpenWindhawk();
    if (cmd == 2) ShellExecuteW(nullptr, L"open", L"taskmgr.exe", nullptr, nullptr, SW_SHOWNORMAL);
    d.outsideSince = GetTickCount();
    Invalidate(d);
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
    }
    int cmd = TrackDockMenu(d, m, screen);
    for (auto& x : g_apps) {
        if (x.key != key) continue;
        if (cmd == 1) Launch(x);
        if (cmd == 3) for (HWND h : wins) PostMessageW(h, WM_SYSCOMMAND, SC_CLOSE, 0);
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
    Log(L"peek hwnd=%p hr=0x%08lx", w, (unsigned long)hr);
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
    g_prev.scale = d.scale;
    if (!g_prev.font) {
        g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, 12.5f * d.scale, L"pt-BR", &g_prev.font);
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
};

enum StartRowKind { RowApp, RowFile, RowTabPinned, RowTabAll, RowPower, RowAction };

// Without Everything running: what the files part offers instead.
enum EverythingState { EvOk, EvClosed, EvMissing };
enum StartAction { ActOpenEverything = 1, ActExplorerSearch, ActGetEverything };

struct StartRow {
    D2D1_RECT_F rc;
    StartRowKind kind;
    StartItem* item = nullptr;
    int power = 0;
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
    ComPtr<IDWriteTextFormat> font, small, section, glyphs;
    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> bmps;
    std::wstring query;
    int tab = 0;  // 0 pinned, 1 all apps
    float scroll = 0, maxScroll = 0;
    int hover = -1, sel = 0;
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
} g_start;

constexpr UINT_PTR TIMER_START_WIDE = 23;
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

static bool Contains(const std::wstring& hay, const std::wstring& needle) {
    if (needle.empty()) return true;
    return FindNLSStringEx(LOCALE_NAME_USER_DEFAULT, FIND_FROMSTART | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC, hay.c_str(),
                           (int)hay.size(), needle.c_str(), (int)needle.size(), nullptr, nullptr, nullptr, 0) >= 0;
}

static void RenderStart();

// Everything.exe, if installed (App Paths, then the usual folders).
static std::wstring EverythingExe() {
    wchar_t buf[MAX_PATH];
    DWORD size = sizeof(buf);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Everything.exe",
                     nullptr, RRF_RT_REG_SZ, nullptr, buf, &size) == ERROR_SUCCESS &&
        GetFileAttributesW(buf) != INVALID_FILE_ATTRIBUTES)
        return buf;
    for (const wchar_t* var : {L"ProgramFiles", L"ProgramFiles(x86)", L"LOCALAPPDATA"}) {
        wchar_t base[MAX_PATH];
        if (!GetEnvironmentVariableW(var, base, MAX_PATH)) continue;
        std::wstring p = std::wstring(base) + (wcscmp(var, L"LOCALAPPDATA") ? L"\\Everything\\Everything.exe"
                                                                            : L"\\Programs\\Everything\\Everything.exe");
        if (GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES) return p;
    }
    return L"";
}

static void QueryEverything() {
    g_start.files.clear();
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
                std::wstring target = it->launch;
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

static void PlaceStartBands();

static void UpdateSearch() {
    g_start.appHits.clear();
    g_start.setHits.clear();
    if (!g_start.query.empty()) {
        if (!g_allAppsLoaded) LoadAllApps();
        for (auto& a : g_allApps) {
            if (Contains(a.name, g_start.query)) {
                g_start.appHits.push_back(&a);
                if (g_start.appHits.size() >= 6) break;
            }
        }
        if (g_set.wideSearch) {
            LoadSettingsItems();
            for (auto& st : g_settingsItems) {
                if (Contains(st.name, g_start.query) || Contains(st.keywords, g_start.query)) {
                    g_start.setHits.push_back(&st);
                    if (g_start.setHits.size() >= 3) break;
                }
            }
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
    ShellExecuteW(nullptr, L"open", it.launch.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static void Power(int which) {
    switch (which) {
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
    }
}

static ID2D1Bitmap* StartIcon(StartItem& it) {
    std::wstring key = it.launch;
    auto f = g_start.bmps.find(key);
    if (f != g_start.bmps.end()) return f->second.Get();
    if (!it.iconTried) {
        it.iconTried = true;
        ComPtr<IShellItem> item;
        if (SUCCEEDED(SHCreateItemFromParsingName(it.launch.c_str(), nullptr, IID_PPV_ARGS(&item))))
            it.icon = IconFromItem(item.Get(), 64);
    }
    ComPtr<ID2D1Bitmap> bmp;
    if (it.icon) g_start.rt->CreateBitmapFromWicBitmap(it.icon.Get(), nullptr, &bmp);
    g_start.bmps[key] = bmp;
    return bmp.Get();
}

static float SS(float v) { return v * g_start.scale; }

static void RenderStart() {
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
    if (s.query.empty()) {
        rt->DrawTextW(Tr(S_SearchHint), (UINT32)wcslen(Tr(S_SearchHint)), s.font.Get(), tr, sub.Get());
    } else {
        ComPtr<IDWriteTextLayout> lay;
        g_dw->CreateTextLayout(s.query.c_str(), (UINT32)s.query.size(), s.font.Get(), tr.right - tr.left, tr.bottom - tr.top, &lay);
        if (lay) {
            rt->DrawTextLayout(D2D1::Point2F(tr.left, tr.top), lay.Get(), text.Get());
            DWRITE_TEXT_METRICS tm;
            lay->GetMetrics(&tm);
            if (s.caretOn) {
                float cx = tr.left + tm.widthIncludingTrailingWhitespace + SS(1);
                rt->FillRectangle(D2D1::RectF(cx, sr.top + SS(13), cx + sw, sr.bottom - SS(13)), text.Get());
            }
        }
    }
    if (s.query.empty() && s.caretOn) {
        rt->FillRectangle(D2D1::RectF(tr.left, sr.top + SS(13), tr.left + sw, sr.bottom - SS(13)), text.Get());
    }

    // Footer: power buttons.
    float footTop = H - pad - SS(40);
    bool showPower = !(searching && wideE > 0.5f);
    rt->FillRectangle(D2D1::RectF(pad, footTop - SS(12), W - pad, footTop - SS(12) + sw), line.Get());
    static const wchar_t* pg[] = {L"", L"", L"", L""};
    for (int i = 0; i < (showPower ? 4 : 0); i++) {
        float x1 = W - pad - (3 - i) * SS(46), x0 = x1 - SS(40);
        D2D1_RECT_F b = D2D1::RectF(x0, footTop, x1, footTop + SS(40));
        int idx = (int)s.rows.size();
        s.rows.push_back({b, RowPower, nullptr, i});
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F((x0 + x1) / 2, footTop + SS(20)), SS(20), SS(20)),
                        s.hover == idx ? cardHi.Get() : card.Get());
        s.glyphs->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        rt->DrawTextW(pg[i], 1, s.glyphs.Get(), b, text.Get());
        s.glyphs->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    const wchar_t* pn[] = {Tr(S_Lock), Tr(S_Sleep), Tr(S_Restart), Tr(S_ShutDown)};
    for (size_t i = 0; i < s.rows.size(); i++) {
        if (s.rows[i].kind == RowPower && s.hover == (int)i) {
            rt->DrawTextW(pn[s.rows[i].power], (UINT32)wcslen(pn[s.rows[i].power]), s.small.Get(),
                          D2D1::RectF(pad, footTop, W / 2, footTop + SS(40)), sub.Get());
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
                                   DWRITE_FONT_STRETCH_NORMAL, (ir.bottom - ir.top) * 0.55f, L"pt-BR", &gf);
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
        D2D1_RECT_F tabs = D2D1::RectF(pad, listTop, pad + SS(236), listTop + SS(36));
        rt->FillRoundedRectangle(D2D1::RoundedRect(tabs, SS(10), SS(10)), card.Get());
        const wchar_t* names[] = {Tr(S_TabPinned), Tr(S_TabAll)};
        float tx = tabs.left + SS(3);
        for (int i = 0; i < 2; i++) {
            float tw = i == 0 ? SS(92) : SS(138);
            D2D1_RECT_F t = D2D1::RectF(tx, tabs.top + SS(3), tx + tw, tabs.bottom - SS(3));
            s.rows.push_back({t, i == 0 ? RowTabPinned : RowTabAll, nullptr, 0});
            if (s.tab == i) rt->FillRoundedRectangle(D2D1::RoundedRect(t, SS(8), SS(8)), hi.Get());
            s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            rt->DrawTextW(names[i], (UINT32)wcslen(names[i]), s.small.Get(), t, s.tab == i ? text.Get() : sub.Get());
            s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            tx += tw + SS(3);
        }
        std::vector<StartItem>& list = s.tab == 0 ? g_startPins : g_allApps;
        wchar_t count[64];
        swprintf_s(count, s.tab == 0 ? Tr(S_CountPinned) : Tr(S_CountApps), (int)list.size());
        s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        rt->DrawTextW(count, (UINT32)wcslen(count), s.small.Get(), D2D1::RectF(W / 2, listTop, W - pad, listTop + SS(36)), sub.Get());
        s.small->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

        float gridTop = listTop + SS(48);
        float colW = (W - pad * 2 - SS(6)) / 2, rowH = SS(48), gap = SS(6);
        int rowsN = ((int)list.size() + 1) / 2;
        s.maxScroll = std::max(0.0f, rowsN * (rowH + gap) - gap - (listBottom - gridTop));
        s.scroll = std::clamp(s.scroll, 0.0f, s.maxScroll);
        rt->PushAxisAlignedClip(D2D1::RectF(0, gridTop, W, listBottom), D2D1_ANTIALIAS_MODE_ALIASED);
        for (size_t i = 0; i < list.size(); i++) {
            float x = pad + (i % 2) * (colW + gap);
            float y = gridTop + (i / 2) * (rowH + gap) - s.scroll;
            if (y + rowH < gridTop || y > listBottom) continue;
            drawRow(list[i], D2D1::RectF(x, y, x + colW, y + rowH), false, false);
        }
        rt->PopAxisAlignedClip();
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
HHOOK g_startMouseHook;

LRESULT CALLBACK StartMouseProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && (wp == WM_LBUTTONDOWN || wp == WM_RBUTTONDOWN || wp == WM_MBUTTONDOWN) &&
        g_start.open && !g_start.menuOpen) {
        auto* m = (MSLLHOOKSTRUCT*)lp;
        HWND under = WindowFromPoint(m->pt);
        wchar_t cls[64] = {};
        if (under) GetClassNameW(GetAncestor(under, GA_ROOT), cls, ARRAYSIZE(cls));
        // (The dock handles its own clicks: its Start button toggles, its apps close the Iniciar.)
        if (!PtInRect(&g_start.rc, m->pt) && wcscmp(cls, L"NimbusMenu_Content") != 0)
            PostMessageW(g_start.hwnd, WM_APP_START_OUTSIDE, 0, 0);
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static void StartMouseHook(bool on) {
    if (on && !g_startMouseHook) g_startMouseHook = SetWindowsHookExW(WH_MOUSE_LL, StartMouseProc, g_module, 0);
    if (!on && g_startMouseHook) {
        UnhookWindowsHookEx(g_startMouseHook);
        g_startMouseHook = nullptr;
    }
}

void StartMouseHookOff() { StartMouseHook(false); }

void ToggleStart(Dock& d) {
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
    g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(14.5f), L"pt-BR", &s.font);
    g_dw->CreateTextFormat(L"Segoe UI Variable Text", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(12.5f), L"pt-BR", &s.small);
    g_dw->CreateTextFormat(L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                           DWRITE_FONT_STRETCH_NORMAL, SS(16), L"pt-BR", &s.glyphs);
    for (auto* f : {s.font.Get(), s.small.Get(), s.glyphs.Get()}) {
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
    s.scroll = 0;
    s.hover = -1;
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
        Log(L"start opened: foreground=%s ours=%d mousehook=%p", cls, GetForegroundWindow() == s.hwnd, g_startMouseHook);
    }
    SetTimer(s.hwnd, TIMER_CARET, 530, nullptr);
    SetTimer(s.hwnd, TIMER_START_ANIM, 15, nullptr);
}

// The blur goes in once the panel has landed (it can't fade or move with it).
static void StartAnimStep() {
    auto& s = g_start;
    DWORD t = GetTickCount() - s.animStart;
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

static int StartRowAt(int x, int y) {
    x -= (int)std::lround(g_start.offX);
    for (int i = 0; i < (int)g_start.rows.size(); i++) {
        const auto& r = g_start.rows[i].rc;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}

static StartItem* SelectedResult() {
    int k = 0;
    for (auto* a : g_start.appHits) if (k++ == g_start.sel) return a;
    for (auto& f : g_start.files) if (k++ == g_start.sel) return &f;
    for (auto* st : g_start.setHits) if (k++ == g_start.sel) return st;
    return nullptr;
}

static void StartItemMenu(StartItem& it, POINT pt) {
    if (it.file) return;
    bool pinned = std::any_of(g_startPins.begin(), g_startPins.end(), [&](const StartItem& p) { return EqI(p.launch, it.launch); });
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, pinned ? Tr(S_UnpinStart) : Tr(S_PinStart));
    std::wstring id = it.launch.rfind(L"shell:AppsFolder\\", 0) == 0 ? it.launch.substr(17) : L"";
    bool uwp = !id.empty() && id.find(L'\\') == std::wstring::npos;
    if (uwp) AppendMenuW(m, MF_STRING, 2, Tr(S_PinDock));
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
        g_pins.push_back({L"aumid", id});
        SavePins();
        RebuildApps();
        for (auto* d : g_docks) { BuildElems(*d, true); Invalidate(*d); }
    }
    SetForegroundWindow(g_start.hwnd);
    RenderStart();
}

LRESULT CALLBACK StartProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto& s = g_start;
    switch (msg) {
        case WM_ACTIVATE: {
            // (Right after a Windows key press, Windows' own Start takes the focus for a moment: we
            // close it and come back, so that doesn't count as clicking away.)
            extern DWORD g_lastWinAlone;
            if (LOWORD(wp) == WA_INACTIVE && s.open && !s.menuOpen && GetTickCount() - s.openedAt > 150 &&
                GetTickCount() - g_lastWinAlone > 1500)
                CloseStart(true);
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;
        case WM_APP_START_OUTSIDE:
            Log(L"start: click outside (open=%d)", s.open);
            if (s.open && !s.menuOpen && GetTickCount() - s.openedAt > 150) CloseStart(true);
            return 0;
        case WM_TIMER:
            if (wp == TIMER_START_ANIM) {
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
            return TRUE;
        }
        case WM_MOUSEMOVE: {
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
            int r = StartRowAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (r < 0) return 0;
            StartRow row = s.rows[r];
            if (row.kind == RowTabPinned || row.kind == RowTabAll) {
                s.tab = row.kind == RowTabPinned ? 0 : 1;
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
                CloseStart();
                Power(row.power);
            } else if (row.item) {
                StartItem it = *row.item;
                CloseStart(true);
                LaunchItem(it);
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            int r = StartRowAt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (r >= 0 && s.rows[r].item && s.rows[r].kind == RowApp) {
                POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                ClientToScreen(hwnd, &pt);
                StartItemMenu(*s.rows[r].item, pt);
            }
            return 0;
        }
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
                case VK_RETURN:
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
                    int total = (int)(s.appHits.size() + s.files.size() + s.setHits.size());
                    if (total) {
                        s.sel = (s.sel + (wp == VK_DOWN ? 1 : total - 1)) % total;
                        RenderStart();
                    }
                    return 0;
                }
                case 'V':
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        if (OpenClipboard(hwnd)) {
                            if (HANDLE hd = GetClipboardData(CF_UNICODETEXT)) {
                                if (auto* t = (const wchar_t*)GlobalLock(hd)) {
                                    std::wstring add = t;
                                    GlobalUnlock(hd);
                                    add.erase(std::remove_if(add.begin(), add.end(), [](wchar_t c) { return c < 32; }), add.end());
                                    s.query += add.substr(0, 200);
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
                if (!s.query.empty()) s.query.pop_back();
            } else if (c == 127) {
                while (!s.query.empty() && s.query.back() == L' ') s.query.pop_back();
                while (!s.query.empty() && s.query.back() != L' ') s.query.pop_back();
            } else if (c >= 32 && s.query.size() < 200) {
                s.query += c;
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
        g_locationHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, OnWindowMoved,
                                         0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
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
        Log(L"Shortcut %s is taken by another program (%lu)", g_set.startHotkey.c_str(), GetLastError());
}

// Tell the explorer.exe part whether the Windows key should open our Start.
static void PublishWinKeyChoice() {
    if (!g_msgWnd) return;
    if (g_set.winKey && g_set.nevoaStart) SetPropW(g_msgWnd, L"NimbusWinKey", (HANDLE)1);
    else RemovePropW(g_msgWnd, L"NimbusWinKey");
}


// ---------------------------------------------------------------------------
// Window procedures
// ---------------------------------------------------------------------------

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
                d->mouseX = -1;
                d->outsideSince = GetTickCount();
                Invalidate(*d);
            }
            return 0;
        case WM_LBUTTONUP:
            if (d) {
                Drag drag = g_drag;
                g_drag = {};
                if (drag.down) ReleaseCapture();
                if (drag.active) {
                    DropDrag(*d, drag.key, (float)GET_X_LPARAM(lp), (float)GET_Y_LPARAM(lp));
                    return 0;
                }
                int e = ElemAt(*d, (float)GET_X_LPARAM(lp), (float)GET_Y_LPARAM(lp));
                KillTimer(hwnd, TIMER_PREVIEW);
                HidePreview();
                if (e >= 0) ClickApp(*d, g_apps[d->elems[e].app]);
            }
            return 0;
        case WM_MBUTTONUP:
            if (d) {
                int e = ElemAt(*d, (float)GET_X_LPARAM(lp), (float)GET_Y_LPARAM(lp));
                if (e >= 0 && !g_apps[d->elems[e].app].start) Launch(g_apps[d->elems[e].app]);
            }
            return 0;
        case WM_RBUTTONUP:
            if (d) {
                float x = (float)GET_X_LPARAM(lp), y = (float)GET_Y_LPARAM(lp);
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
        case WM_APP_APPBAR:
            if (d && wp == ABN_POSCHANGED) SetAppBarPos(*d);
            return 0;
        case WM_CAPTURECHANGED:
            if (g_drag.active && (HWND)lp != hwnd) {
                g_drag = {};
                if (d) Invalidate(*d);
            }
            return 0;
        case WM_TIMER:
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
    RemoveAppBar(*d);
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
        DockCfg cfg = LoadDockCfg(group);
        Log(L"monitor (%d,%d,%d,%d) -> %s enabled=%d fixed=%d autoHide=%d", mi.rcMonitor.left, mi.rcMonitor.top,
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
                               DWRITE_FONT_STRETCH_NORMAL, 12.5f * d->scale, L"pt-BR", &d->font);

        for (int b = 0; b < 1 + 2 * kBandSteps; b++) {
            d->bands.push_back(CreateBand());
            d->bandRc.push_back(RECT{});
        }

        d->content = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
                                     L"NimbusMenu_Content", L"Nimbus Menu", WS_POPUP, d->rcMon.left,
                                     d->rcMon.bottom - d->ch, d->cw, d->ch, d->bands[0], nullptr, g_module, nullptr);
        Log(L"dock mon=(%d,%d,%d,%d) scale=%.2f cw=%d ch=%d content=%p style=%s", d->rcMon.left, d->rcMon.top,
            d->rcMon.right, d->rcMon.bottom, d->scale, d->cw, d->ch, d->content, g_set.style.c_str());
        SetWindowLongPtrW(d->content, GWLP_USERDATA, (LONG_PTR)d);
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
            if (StartOpenOn(d)) overDock = true;
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
                static int tick = 0;
                tick++;
                Poll();
                if (g_set.hideTaskbar && tick % 10 == 0) HideTaskbars();
                if (tick % 30 == 0 || (!g_launch.empty() && tick % 5 == 0)) ScheduleRebuild();
                PollPreview();
            } else if (wp == TIMER_REBUILD) {
                KillTimer(hwnd, TIMER_REBUILD);
                RebuildApps();
                if (g_prev.visible) {
                    auto it = std::find_if(g_apps.begin(), g_apps.end(), [](const App& a) { return a.key == g_prev.key; });
                    std::vector<HWND> now;
                    if (it != g_apps.end() && g_prev.dock) now = DockWins(*g_prev.dock, *it);
                    size_t n = std::min(now.size(), (size_t)kMaxPreviews);
                    bool same = n == g_prev.wins.size() && std::equal(g_prev.wins.begin(), g_prev.wins.end(), now.begin());
                    if (!same) HidePreview();
                }
                for (auto* d : g_docks) {
                    BuildElems(*d, true);
                    Invalidate(*d);
                }
            } else if (wp == TIMER_DOCKS) {
                KillTimer(hwnd, TIMER_DOCKS);
                if (g_set.hideTaskbar) HideTaskbars();
                CreateDocks();
            }
            return 0;
        case WM_DISPLAYCHANGE:
            // (Not on work-area changes: the fixed bar's own app bar causes those.)
            SetTimer(hwnd, TIMER_DOCKS, 800, nullptr);
            return 0;
        case WM_APP_SETTINGS:
            LoadSettings();
            RegisterStartHotkey();
            PublishWinKeyChoice();
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
            return 0;
        case WM_APP_WINKEY: {
            g_lastWinAlone = GetTickCount();
            POINT pt;
            GetCursorPos(&pt);
            HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
            Dock* target = nullptr;
            for (auto* d : g_docks) if (d->mon == mon) target = d;
            if (!target && !g_docks.empty()) target = g_docks[0];
            if (target) ToggleStart(*target);
            // (The explorer.exe part brings the open Start to the front itself.)
            return g_start.open ? (LRESULT)g_start.hwnd : 0;
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
        case WM_APP_QUIT:
            UnregisterHotKey(hwnd, 1);
            RemovePropW(hwnd, L"NimbusWinKey");
            if (g_moveEndHook) UnhookWinEvent(g_moveEndHook);
            if (g_locationHook) UnhookWinEvent(g_locationHook);
            g_moveEndHook = g_locationHook = nullptr;
            KillTimer(hwnd, TIMER_POLL);
            DestroyDocks();
            RestoreTaskbars();
            DeregisterShellHookWindow(hwnd);
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)g_dw.GetAddressOf());
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_wic));
    if (!g_d2d || !g_dw || !g_wic) {
        Wh_Log(L"Could not create the graphics factories");
        CoUninitialize();
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
    Log(L"start: pins=%d apps=%d", (int)g_pins.size(), (int)g_apps.size());
    for (auto& a : g_apps) Log(L"  app key=%s wins=%d pinned=%d", a.key.c_str(), (int)a.wins.size(), a.pinned);
    if (g_set.hideTaskbar) {
        HideTaskbars();
    } else {
        // The option may have been turned off while the bar wasn't running (after a crash): give the
        // taskbar back its own auto-hide and show it. Nothing happens if this mod never changed it.
        RestoreTaskbarAutoHide();
        ShowTaskbarWindows();
    }
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
    g_start.dc.Reset();
    g_start.rt.Reset();
    g_prev.font.Reset();
    g_prev.rt.Reset();
    g_d2d.Reset();
    g_dw.Reset();
    g_wic.Reset();
    CoUninitialize();
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
            // Windows' search box (SearchHost, invisible here) took the keyboard when the key went down.
            // An Esc makes it let go.
            HWND fg = GetForegroundWindow();
            wchar_t cls[64] = {};
            GetClassNameW(fg, cls, ARRAYSIZE(cls));
            HWND w = (sent && opened && IsWindow((HWND)opened)) ? (HWND)opened : nullptr;
            if (w && SetForegroundWindow(w)) return S_OK;
            if (!wcscmp(cls, L"Windows.UI.Core.CoreWindow")) {
                INPUT in[2] = {};
                in[0].type = in[1].type = INPUT_KEYBOARD;
                in[0].ki.wVk = VK_ESCAPE;
                in[1].ki.wVk = VK_ESCAPE;
                in[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, in, sizeof(INPUT));
            }
            if (w) {
                for (int i = 0; i < 8 && GetForegroundWindow() != w; i++) {
                    Sleep(25);
                    SetForegroundWindow(w);
                }
                if (GetForegroundWindow() != w) Wh_Log(L"Could not bring Nimbus' Start to the front");
            }
            return S_OK;
        }
    }
    return ShowStartView_orig(self, method, flags);
}

// Folder windows can run in their own explorer.exe processes, where Start never opens.
static bool IsFolderWindowsProcess() {
    std::wstring cmd = Lower(GetCommandLineW());
    return cmd.find(L"/factory") != std::wstring::npos || cmd.find(L"-embedding") != std::wstring::npos ||
           cmd.find(L"/separate") != std::wstring::npos;
}

static BOOL ExplorerInit() {
    if (IsFolderWindowsProcess()) return FALSE;
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
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellDllHooks[] = {
        {{L"public: virtual long __cdecl XamlLauncherState::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum "
          L"IMMERSIVELAUNCHERSHOWFLAGS)"},
         &xamlLauncherState, nullptr, true},
        {{L"public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum "
          L"IMMERSIVELAUNCHERSHOWFLAGS)"},
         &xamlLauncher, nullptr, true},
    };
    WindhawkUtils::HookSymbols(twinui, twinuiPcshellDllHooks, ARRAYSIZE(twinuiPcshellDllHooks));
    if (void* target = xamlLauncherState ? xamlLauncherState : xamlLauncher) {
        Wh_SetFunctionHook(target, (void*)ShowStartView_hook, (void**)&ShowStartView_orig);
    } else {
        Wh_Log(L"ShowStartView not found: the Windows key keeps opening Windows' Start");
    }
    return TRUE;
}

// The mod is being turned off (or updated): once the bar's process is gone, make sure the Windows
// taskbar is back, also when that process crashed or was ended before it could restore it.
static void ExplorerUninit() {
    for (int i = 0; i < 100 && FindWindowW(L"NimbusMenu_Msg", nullptr); i++) Sleep(50);
    if (FindWindowW(L"NimbusMenu_Msg", nullptr)) return;  // still running: it restores the taskbar itself
    RestoreTaskbarAutoHide();
    ShowTaskbarWindows();
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
