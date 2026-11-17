// ==WindhawkMod==
// @id              windowpilot
// @name            WindowPilot
// @description     Configurable window shortcuts with game protection and optional double-press confirmation
// @description:pt-BR Atalhos de janela com proteção para jogos e confirmação opcional por duas pressões
// @description:es-ES Atajos de ventana con protección para juegos y confirmación opcional de doble pulsación
// @version         1.1.0
// @author          ghostzpy
// @github          https://github.com/ghostzpy
// @license         MIT
// @include         explorer.exe
// @include         ctfmon.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -ldwmapi -lshlwapi -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WindowPilot

## English

Control the foreground window with configurable shortcuts:

| Default shortcut | Action |
| --- | --- |
| Win + Q | Close normally, preserving the application's save prompts |
| Win + Z | Minimize |
| Win + N | Maximize; optionally restore when already maximized |
| Win + Shift + N | Open the native Windows Notification Center |
| Win + S | Original Windows Search, unchanged |

Each window action has independent enable, shortcut, game protection and
double-press confirmation settings. Game protection is enabled by default;
double-press confirmation is disabled by default. Shortcuts support Win, Ctrl,
Alt, Shift, A–Z, 0–9, F1–F24 and common named keys. Examples: `Win+Q`,
`Ctrl+Alt+F10`, `Win+Shift+N`. Only the exact configured combination is consumed.
Changing or disabling a shortcut immediately releases its old combination.
Win+S and Win+L are reserved; duplicate or invalid bindings show a warning.

With confirmation enabled, press and release the shortcut twice in **less than
3 seconds**. The hint disappears after 1.5 seconds by default; its disappearance
does not shorten the confirmation deadline. Holding a key never confirms.
Switching windows, choosing another action, pressing Esc or saving settings
cancels a pending confirmation. Timings are configurable for each action.

**Hint appearance:** automatically follows the Windows display language in
English, Portuguese or Spanish, with English fallback for other languages.
You can also select a language manually. Dark and light themes can follow
Windows or be selected explicitly. **Always on top** is configurable and
enabled by default. The hint never steals keyboard focus or intercepts clicks.
Settings labels follow the language selected in the Windhawk interface.

**Elevated windows:** the mod runs a standard-user fallback in Explorer and an
additional keyboard worker in the user's `ctfmon.exe` text-input host, which
normally has UIAccess. This covers elevated applications such as Windhawk's
editor. Keep both executable names in the mod's inclusion list. It does not
restart either process, create a scheduled task, inject into games or change
Windows security settings. Secure-desktop prompts are outside its scope.
If your Windhawk configuration excludes `ctfmon.exe`, normal windows continue
to work, but elevated-window coverage may be unavailable.

**Games:** detection uses editable executable/path patterns, Unity/Unreal
window classes and optional fullscreen geometry. It is conservative: browser
F11 fullscreen can be protected too; ordinary maximized windows are excluded
from the fullscreen heuristic. Add unrecognized windowed games to the list.
Protected shortcuts do nothing, including their original Windows action.

Notifications use the original Win+N after the alternate shortcut's modifiers
are released. Tagged synthetic input prevents recursive maximization. If an
existing Windows policy disables Notification Center, the hint explains the
restriction; the mod does not change that policy. Desktop, taskbar, menus and
windows that do not support an action are preserved. No forced process termination.

Windows 11 x64; Windhawk 1.7 or later.

## Português

Atalhos padrão: **Win+Q** fecha, **Win+Z** minimiza, **Win+N** maximiza/restaura,
**Win+Shift+N** abre notificações e **Win+S** continua sendo a pesquisa nativa.
Cada ação tem opções independentes de ativação, atalho, proteção para jogos e
duas pressões. A proteção para jogos vem ativada e a confirmação desativada.

Com a confirmação ligada, solte a tecla e pressione novamente em menos de
3 segundos. O aviso some após 1,5 segundo, mas o prazo continua válido.
Trocar de janela, usar outra ação, pressionar Esc ou salvar configurações cancela.

Em **Aparência do aviso**, escolha idioma automático/inglês/português/espanhol,
tema automático/escuro/claro e **Sempre no topo**. O modo automático acompanha
o idioma de exibição e o tema do Windows; o aviso não captura foco nem cliques.
Os rótulos das configurações seguem o idioma da interface do Windhawk.

Explorer e `ctfmon.exe` atendem às janelas comuns e elevadas. Mantenha os dois
na lista de inclusão do mod. A detecção de jogos usa heurísticas e listas
editáveis; acrescente jogos em janela não reconhecidos. O mod não é injetado
nos jogos, não encerra processos à força e não altera políticas do Windows.
Se a central de notificações estiver bloqueada por política, um aviso informa.

## Español

Atajos predeterminados: **Win+Q** cierra, **Win+Z** minimiza, **Win+N** maximiza/restaura,
**Win+Shift+N** abre notificaciones y **Win+S** conserva la búsqueda de Windows.
Cada acción tiene controles independientes para activar, cambiar el atajo,
ignorar juegos y exigir dos pulsaciones. La protección para juegos está activada;
la confirmación está desactivada de forma predeterminada.

Con la confirmación activada, suelta la tecla y pulsa de nuevo en menos de
3 segundos. El aviso desaparece después de 1,5 segundos, pero el plazo sigue
vigente. Cambiar de ventana, usar otra acción, pulsar Esc o guardar ajustes cancela.

En **Apariencia del aviso**, elige idioma automático/inglés/portugués/español,
tema automático/oscuro/claro y **Siempre visible**. El modo automático sigue el
idioma de visualización y el tema de Windows. El aviso no roba foco ni clics.
Los ajustes siguen el idioma elegido en la interfaz de Windhawk.

Explorer y `ctfmon.exe` cubren ventanas normales y elevadas. Conserva ambos en
la lista de inclusión. La detección de juegos usa heurísticas y listas editables;
añade los juegos en ventana que no se reconozcan. El mod no se inyecta en juegos,
no fuerza el cierre de procesos y no cambia las directivas de Windows.
Si una directiva bloquea las notificaciones, un aviso explica la restricción.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable WindowPilot
  $description: Turn off to restore the original Windows behavior for every shortcut.
  $name:pt-BR: Ativar WindowPilot
  $name:es-ES: Activar WindowPilot
  $description:pt-BR: Desative para devolver todos os atalhos ao comportamento original do Windows.
  $description:es-ES: Desactiva para restaurar el comportamiento original de todos los atajos de Windows.
- appearance:
  - language: auto
    $name: Hint language
    $name:pt-BR: Idioma do aviso
    $name:es-ES: Idioma del aviso
    $description: Automatic follows the Windows display language. English is used for other languages.
    $description:pt-BR: Automático segue o idioma de exibição do Windows. Outros idiomas usam inglês.
    $description:es-ES: Automático sigue el idioma de visualización de Windows. Para otros idiomas se usa inglés.
    $options:
    - auto: Automatic (Windows)
    - en: English
    - pt: Português
    - es: Español
    $options:pt-BR:
    - auto: Automático (Windows)
    - en: English
    - pt: Português
    - es: Español
    $options:es-ES:
    - auto: Automático (Windows)
    - en: English
    - pt: Português
    - es: Español
  - theme: auto
    $name: Hint theme
    $name:pt-BR: Tema do aviso
    $name:es-ES: Tema del aviso
    $options:
    - auto: Automatic (Windows)
    - dark: Dark
    - light: Light
    $options:pt-BR:
    - auto: Automático (Windows)
    - dark: Escuro
    - light: Claro
    $options:es-ES:
    - auto: Automático (Windows)
    - dark: Oscuro
    - light: Claro
  - alwaysOnTop: true
    $name: Always on top
    $name:pt-BR: Sempre no topo
    $name:es-ES: Siempre visible
    $description: Keeps the hint above other windows. Disable for normal window stacking. Never steals keyboard
      focus.
    $description:pt-BR: Mantém o aviso acima das outras janelas. Desative para usar a ordem normal. Não captura
      o foco do teclado.
    $description:es-ES: Mantiene el aviso sobre otras ventanas. Desactiva para usar el orden normal. No roba el
      foco del teclado.
  $name: Hint appearance
  $name:pt-BR: Aparência do aviso
  $name:es-ES: Apariencia del aviso
- close:
  - enabled: true
    $name: Enable close shortcut
    $name:pt-BR: Ativar atalho para fechar
    $name:es-ES: Activar atajo para cerrar
  - hotkey: Win+Q
    $name: Keyboard shortcut
    $description: 'Examples: Win+Q, Ctrl+Alt+F10. Win+S remains reserved for Search.'
    $name:pt-BR: Atalho
    $name:es-ES: Atajo de teclado
    $description:pt-BR: 'Exemplos: Win+Q, Ctrl+Alt+F10. Win+S permanece reservado à pesquisa.'
    $description:es-ES: 'Ejemplos: Win+Q, Ctrl+Alt+F10. Win+S sigue reservado para Buscar.'
  - ignoreGames: true
    $name: Ignore games
    $name:pt-BR: Ignorar jogos
    $name:es-ES: Ignorar juegos
  - doublePress: false
    $name: Require two presses to close
    $name:pt-BR: Exigir duas pressões para fechar
    $name:es-ES: Exigir dos pulsaciones para cerrar
  - confirmationMs: 3000
    $name: Maximum interval between presses (ms)
    $description: Press again before this deadline. Default 3000; allowed range 500–10000.
    $name:pt-BR: Intervalo máximo entre pressões (ms)
    $name:es-ES: Intervalo máximo entre pulsaciones (ms)
    $description:pt-BR: A segunda pressão deve ocorrer antes deste prazo. Padrão 3000; intervalo permitido de 500
      a 10000.
    $description:es-ES: Pulsa de nuevo antes de este plazo. Predeterminado 3000; rango permitido 500–10000.
  - hintMs: 1500
    $name: Hint duration (ms)
    $description: The hint may disappear before the confirmation deadline. Default 1500.
    $name:pt-BR: Duração do aviso (ms)
    $name:es-ES: Duración del aviso (ms)
    $description:pt-BR: O aviso pode sumir antes de terminar o prazo de confirmação. Padrão 1500.
    $description:es-ES: El aviso puede desaparecer antes de vencer el plazo de confirmación. Predeterminado 1500.
  $name: Close window
  $name:pt-BR: Fechar janela
  $name:es-ES: Cerrar ventana
- minimize:
  - enabled: true
    $name: Enable minimize shortcut
    $name:pt-BR: Ativar atalho para minimizar
    $name:es-ES: Activar atajo para minimizar
  - hotkey: Win+Z
    $name: Keyboard shortcut
    $name:pt-BR: Atalho
    $name:es-ES: Atajo de teclado
  - ignoreGames: true
    $name: Ignore games
    $name:pt-BR: Ignorar jogos
    $name:es-ES: Ignorar juegos
  - doublePress: false
    $name: Require two presses to minimize
    $name:pt-BR: Exigir duas pressões para minimizar
    $name:es-ES: Exigir dos pulsaciones para minimizar
  - confirmationMs: 3000
    $name: Maximum interval between presses (ms)
    $name:pt-BR: Intervalo máximo entre pressões (ms)
    $name:es-ES: Intervalo máximo entre pulsaciones (ms)
  - hintMs: 1500
    $name: Hint duration (ms)
    $name:pt-BR: Duração do aviso (ms)
    $name:es-ES: Duración del aviso (ms)
  $name: Minimize window
  $name:pt-BR: Minimizar janela
  $name:es-ES: Minimizar ventana
- maximize:
  - enabled: true
    $name: Enable maximize shortcut
    $name:pt-BR: Ativar atalho para maximizar
    $name:es-ES: Activar atajo para maximizar
  - hotkey: Win+N
    $name: Keyboard shortcut
    $name:pt-BR: Atalho
    $name:es-ES: Atajo de teclado
  - ignoreGames: true
    $name: Ignore games
    $name:pt-BR: Ignorar jogos
    $name:es-ES: Ignorar juegos
  - doublePress: false
    $name: Require two presses to maximize or restore
    $name:pt-BR: Exigir duas pressões para maximizar ou restaurar
    $name:es-ES: Exigir dos pulsaciones para maximizar o restaurar
  - confirmationMs: 3000
    $name: Maximum interval between presses (ms)
    $name:pt-BR: Intervalo máximo entre pressões (ms)
    $name:es-ES: Intervalo máximo entre pulsaciones (ms)
  - hintMs: 1500
    $name: Hint duration (ms)
    $name:pt-BR: Duração do aviso (ms)
    $name:es-ES: Duración del aviso (ms)
  - restoreIfMaximized: true
    $name: Restore when pressed again on a maximized window
    $name:pt-BR: Restaurar ao pressionar novamente em uma janela maximizada
    $name:es-ES: Restaurar al pulsar de nuevo en una ventana maximizada
  $name: Maximize and restore window
  $name:pt-BR: Maximizar e restaurar janela
  $name:es-ES: Maximizar y restaurar ventana
- notifications:
  - enabled: true
    $name: Enable alternate notification shortcut
    $name:pt-BR: Ativar atalho alternativo das notificações
    $name:es-ES: Activar atajo alternativo de notificaciones
  - hotkey: Win+Shift+N
    $name: Keyboard shortcut
    $description: Sends native Win+N after releasing the keys. Opens Notification Center, not Quick Settings.
    $name:pt-BR: Atalho
    $name:es-ES: Atajo de teclado
    $description:pt-BR: Abre a central pelo Win+N nativo após soltar as teclas. Não é um atalho para configurações
      rápidas.
    $description:es-ES: Envía Win+N nativo al soltar las teclas. Abre el centro de notificaciones, no la configuración
      rápida.
  $name: Notification Center
  $name:pt-BR: Central de notificações
  $name:es-ES: Centro de notificaciones
- games:
  - protectFullscreen: true
    $name: Also protect fullscreen windows
    $description: Conservative detection; includes browsers in F11. Ordinary maximized windows are not considered
      fullscreen.
    $name:pt-BR: Proteger também janelas em tela cheia
    $name:es-ES: Proteger también las ventanas a pantalla completa
    $description:pt-BR: Detecção conservadora; pode incluir navegador em F11. Janelas normalmente maximizadas não
      contam como tela cheia.
    $description:es-ES: Detección conservadora; incluye navegadores en F11. Las ventanas maximizadas normales no
      cuentan como pantalla completa.
  - processes:
    - acs.exe
    - Cities2.exe
    - RustClient.exe
    - cs2.exe
    - VALORANT-Win64-Shipping.exe
    - Minecraft.Windows.exe
    - RobloxPlayerBeta.exe
    $name: Additional game executables
    $description: Supports wildcards such as *-Win64-Shipping.exe. Add unrecognized windowed games.
    $name:pt-BR: Executáveis adicionais de jogos
    $name:es-ES: Ejecutables adicionales de juegos
    $description:pt-BR: Aceita curingas, como *-Win64-Shipping.exe. Adicione jogos em janela que não sejam reconhecidos.
    $description:es-ES: Admite comodines como *-Win64-Shipping.exe. Añade juegos en ventana que no se reconozcan.
  - paths:
    - '*\steamapps\common\*'
    - '*\Epic Games\*'
    - '*\XboxGames\*'
    - '*\GOG Games\*'
    - '*\Riot Games\*'
    $name: Game paths
    $description: Patterns matched against the full executable path. Supports * and ?.
    $name:pt-BR: Caminhos de jogos
    $name:es-ES: Rutas de juegos
    $description:pt-BR: Padrões comparados com o caminho completo do executável. Aceita * e ?.
    $description:es-ES: Patrones comparados con la ruta completa del ejecutable. Admite * y ?.
  $name: Game detection
  $name:pt-BR: Detecção de jogos
  $name:es-ES: Detección de juegos
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

namespace windowpilot {
constexpr UINT kActionMessage = WM_APP + 101;
constexpr UINT kReloadMessage = WM_APP + 102;
constexpr UINT kCancelMessage = WM_APP + 103;
constexpr UINT_PTR kHideHint = 1, kExpireConfirmation = 2, kNotifyTimer = 3;
constexpr ULONG_PTR kOwnInput = 0x57504C54;  // WPLT; no logging of keyboard input.
constexpr wchar_t kHostClass[] = L"WindowPilot.MessageHost.v1";
constexpr wchar_t kToastClass[] = L"WindowPilot.Confirmation.v1";
enum Action : size_t { Close, Minimize, Maximize, Notifications, ActionCount };
enum class Language { English, Portuguese, Spanish };

Language ResolveLanguage(const std::wstring& setting, LANGID systemLanguage) {
    if (setting == L"en") return Language::English;
    if (setting == L"pt") return Language::Portuguese;
    if (setting == L"es") return Language::Spanish;
    switch (PRIMARYLANGID(systemLanguage)) {
        case LANG_PORTUGUESE: return Language::Portuguese;
        case LANG_SPANISH: return Language::Spanish;
        default: return Language::English;
    }
}
const wchar_t* Text(Language language, const wchar_t* en, const wchar_t* pt, const wchar_t* es) {
    return language == Language::Portuguese ? pt : language == Language::Spanish ? es : en;
}
bool ResolveLightTheme(const std::wstring& setting, bool systemLight) {
    return setting == L"light" || (setting == L"auto" && systemLight);
}

struct Hotkey {
    UINT modifiers{};
    UINT key{};
    bool valid{};
    bool operator==(const Hotkey& other) const {
        return modifiers == other.modifiers && key == other.key;
    }
};
struct ActionSettings {
    bool enabled = true;
    std::wstring shortcut;
    Hotkey hotkey;
    bool ignoreGames = true;
    bool doublePress = false;
    UINT confirmationMs = 3000;
    UINT hintMs = 1500;
};
struct Config {
    bool enabled = true;
    bool restoreIfMaximized = true;
    bool protectFullscreen = true;
    Language language = ResolveLanguage(L"auto", GetUserDefaultUILanguage());
    std::wstring theme = L"auto";
    bool alwaysOnTop = true;
    std::array<ActionSettings, ActionCount> actions;
    std::vector<std::wstring> gameProcesses, gamePaths, errors;
};

std::wstring Trim(std::wstring text) {
    auto start = text.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return {};
    return text.substr(start, text.find_last_not_of(L" \t\r\n") - start + 1);
}
std::wstring Upper(std::wstring text) {
    std::transform(text.begin(), text.end(), text.begin(), towupper);
    return text;
}
Hotkey ParseHotkey(const std::wstring& text) {
    Hotkey result;
    size_t position = 0;
    bool hasKey = false;
    while (position <= text.size()) {
        auto end = text.find(L'+', position);
        auto token = Upper(Trim(text.substr(position, end - position)));
        if (token.empty()) return {};
        UINT modifier = 0;
        if (token == L"WIN" || token == L"WINDOWS") modifier = MOD_WIN;
        else if (token == L"CTRL" || token == L"CONTROL") modifier = MOD_CONTROL;
        else if (token == L"ALT") modifier = MOD_ALT;
        else if (token == L"SHIFT") modifier = MOD_SHIFT;
        if (modifier) {
            if ((result.modifiers & modifier) || hasKey) return {};
            result.modifiers |= modifier;
        } else {
            if (hasKey) return {};
            if (token.size() == 1 && ((token[0] >= L'A' && token[0] <= L'Z') ||
                                      (token[0] >= L'0' && token[0] <= L'9'))) {
                result.key = token[0];
            } else if (token.size() >= 2 && token[0] == L'F' &&
                       token.size() <= 3 && token[1] >= L'1' && token[1] <= L'9' &&
                       (token.size() == 2 || (token[2] >= L'0' && token[2] <= L'9'))) {
                auto number = wcstol(token.c_str() + 1, nullptr, 10);
                if (number < 1 || number > 24) return {};
                result.key = VK_F1 + static_cast<UINT>(number) - 1;
            } else {
                static const std::pair<const wchar_t*, UINT> names[] = {
                    {L"ESC", VK_ESCAPE}, {L"ESCAPE", VK_ESCAPE}, {L"TAB", VK_TAB},
                    {L"SPACE", VK_SPACE}, {L"ENTER", VK_RETURN}, {L"RETURN", VK_RETURN},
                    {L"BACKSPACE", VK_BACK}, {L"DELETE", VK_DELETE}, {L"DEL", VK_DELETE},
                    {L"INSERT", VK_INSERT}, {L"INS", VK_INSERT}, {L"HOME", VK_HOME},
                    {L"END", VK_END}, {L"PAGEUP", VK_PRIOR}, {L"PAGEDOWN", VK_NEXT},
                    {L"LEFT", VK_LEFT}, {L"RIGHT", VK_RIGHT}, {L"UP", VK_UP}, {L"DOWN", VK_DOWN}
                };
                for (auto [name, code] : names) if (token == name) result.key = code;
                if (!result.key) return {};
            }
            hasKey = true;
        }
        if (end == std::wstring::npos) break;
        position = end + 1;
    }
    if (!hasKey || !result.modifiers) return {};
    if (result.modifiers == MOD_WIN && (result.key == L'S' || result.key == L'L')) return {};
    if ((result.modifiers & (MOD_CONTROL | MOD_ALT)) == (MOD_CONTROL | MOD_ALT) &&
        result.key == VK_DELETE) return {};
    result.valid = true;
    return result;
}
std::wstring DisplayHotkey(const std::wstring& input) {
    std::wstring result;
    for (size_t pos = 0; pos <= input.size();) {
        auto end = input.find(L'+', pos);
        if (!result.empty()) result += L" + ";
        result += Trim(input.substr(pos, end - pos));
        if (end == std::wstring::npos) break;
        pos = end + 1;
    }
    return result;
}
const wchar_t* ActionName(Action action, Language language = Language::English) {
    switch (action) {
        case Close: return Text(language, L"Close", L"Fechar", L"Cerrar");
        case Minimize: return Text(language, L"Minimize", L"Minimizar", L"Minimizar");
        case Maximize: return Text(language, L"Maximize/restore", L"Maximizar/restaurar", L"Maximizar/restaurar");
        default: return Text(language, L"Notifications", L"Notificações", L"Notificaciones");
    }
}
void ValidateConfig(Config& config) {
    config.errors.clear();
    for (size_t i = 0; i < ActionCount; i++) {
        auto& action = config.actions[i];
        action.hotkey = ParseHotkey(action.shortcut);
        if (action.enabled && !action.hotkey.valid)
            config.errors.push_back(std::wstring(ActionName(static_cast<Action>(i), config.language)) +
                Text(config.language, L": invalid shortcut", L": atalho inválido", L": atajo no válido"));
        action.confirmationMs = std::clamp(action.confirmationMs, 500u, 10000u);
        action.hintMs = std::clamp(action.hintMs, 500u, 5000u);
    }
    // Check all pairs before invalidating; three-way duplicates must disable all three.
    std::array<bool, ActionCount> duplicate{};
    for (size_t i = 0; i < ActionCount; i++) {
        for (size_t j = i + 1; j < ActionCount; j++) {
            if (config.actions[i].enabled && config.actions[j].enabled &&
                config.actions[i].hotkey.valid && config.actions[j].hotkey.valid &&
                config.actions[i].hotkey == config.actions[j].hotkey) duplicate[i] = duplicate[j] = true;
        }
    }
    for (size_t i = 0; i < ActionCount; i++) if (duplicate[i]) {
        config.actions[i].hotkey.valid = false;
        config.errors.push_back(std::wstring(ActionName(static_cast<Action>(i), config.language)) +
            Text(config.language, L": duplicate shortcut", L": atalho duplicado", L": atajo duplicado"));
    }
}
int FindAction(const Config& config, UINT key, UINT modifiers) {
    if (!config.enabled) return -1;
    for (size_t i = 0; i < ActionCount; i++) {
        const auto& a = config.actions[i];
        if (a.enabled && a.hotkey.valid && a.hotkey.key == key && a.hotkey.modifiers == modifiers)
            return static_cast<int>(i);
    }
    return -1;
}

struct ConfirmationGate {
    HWND window{};
    DWORD pid{}, tid{};
    Action action = Close;
    ULONGLONG pressedAt{};
    bool armed{};
    void Reset() { armed = false; window = nullptr; }
    bool Press(Action next, HWND target, DWORD process, DWORD thread, ULONGLONG now, UINT interval) {
        bool confirmed = armed && action == next && window == target && pid == process &&
                         tid == thread && now >= pressedAt && now - pressedAt < interval;
        Reset();
        if (confirmed) return true;
        action = next; window = target; pid = process; tid = thread; pressedAt = now; armed = true;
        return false;
    }
};

bool IsGameImage(const Config& config, const std::wstring& path, const std::wstring& className) {
    auto name = PathFindFileNameW(path.c_str());
    for (const auto& pattern : config.gameProcesses)
        if (!pattern.empty() && PathMatchSpecW(name, pattern.c_str())) return true;
    for (const auto& pattern : config.gamePaths)
        if (!pattern.empty() && PathMatchSpecW(path.c_str(), pattern.c_str())) return true;
    return className == L"UnityWndClass" || className == L"UnrealWindow" ||
           PathMatchSpecW(name, L"*-Win64-Shipping.exe") ||
           PathMatchSpecW(name, L"*-WinGDK-Shipping.exe");
}
bool IsFullscreenRect(const RECT& window, const RECT& monitor, bool maximized) {
    return !maximized && window.left <= monitor.left + 1 && window.top <= monitor.top + 1 &&
           window.right >= monitor.right - 1 && window.bottom >= monitor.bottom - 1;
}
bool IsGame(HWND window, const Config& config) {
    DWORD pid{};
    GetWindowThreadProcessId(window, &pid);
    std::wstring path;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (process) {
        wchar_t buffer[32768]; DWORD length = ARRAYSIZE(buffer);
        if (QueryFullProcessImageNameW(process, 0, buffer, &length)) path.assign(buffer, length);
        CloseHandle(process);
    }
    wchar_t className[256]{};
    GetClassNameW(window, className, ARRAYSIZE(className));
    if (IsGameImage(config, path, className)) return true;
    if (!config.protectFullscreen) return false;
    RECT rect{}; MONITORINFO monitor{sizeof(monitor)};
    if (!GetWindowRect(window, &rect) ||
        !GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)) return false;
    return IsFullscreenRect(rect, monitor.rcMonitor, IsZoomed(window));
}

std::wstring ReadString(const wchar_t* key) {
    auto value = Wh_GetStringSetting(key);
    std::wstring result = value ? value : L"";
    if (value) Wh_FreeStringSetting(value);
    return result;
}
Config LoadConfig() {
    Config config;
    config.enabled = Wh_GetIntSetting(L"enabled") != 0;
    config.language = ResolveLanguage(ReadString(L"appearance.language"), GetUserDefaultUILanguage());
    config.theme = ReadString(L"appearance.theme");
    if (config.theme != L"dark" && config.theme != L"light") config.theme = L"auto";
    config.alwaysOnTop = Wh_GetIntSetting(L"appearance.alwaysOnTop") != 0;
    const wchar_t* groups[] = {L"close", L"minimize", L"maximize", L"notifications"};
    for (size_t i = 0; i < ActionCount; i++) {
        auto prefix = std::wstring(groups[i]) + L".";
        auto& action = config.actions[i];
        action.enabled = Wh_GetIntSetting((prefix + L"enabled").c_str()) != 0;
        action.shortcut = ReadString((prefix + L"hotkey").c_str());
        if (i == Notifications) continue;
        action.ignoreGames = Wh_GetIntSetting((prefix + L"ignoreGames").c_str()) != 0;
        action.doublePress = Wh_GetIntSetting((prefix + L"doublePress").c_str()) != 0;
        action.confirmationMs = static_cast<UINT>(std::clamp(
            Wh_GetIntSetting((prefix + L"confirmationMs").c_str()), 500, 10000));
        action.hintMs = static_cast<UINT>(std::clamp(
            Wh_GetIntSetting((prefix + L"hintMs").c_str()), 500, 5000));
    }
    config.restoreIfMaximized = Wh_GetIntSetting(L"maximize.restoreIfMaximized") != 0;
    config.protectFullscreen = Wh_GetIntSetting(L"games.protectFullscreen") != 0;
    for (auto pair : {std::pair{L"games.processes", &config.gameProcesses},
                      std::pair{L"games.paths", &config.gamePaths}}) {
        for (int i = 0; i < 256; i++) {
            auto value = Wh_GetStringSetting(L"%s[%d]", pair.first, i);
            std::wstring item = value ? value : L"";
            if (value) Wh_FreeStringSetting(value);
            if (item.empty()) break;
            pair.second->push_back(std::move(item));
        }
    }
    ValidateConfig(config);
    return config;
}

[[clang::no_destroy]] std::mutex g_configMutex;
[[clang::no_destroy]] Config g_config;
HANDLE g_stopEvent{}, g_readyEvent{}, g_workerThread{};
DWORD g_workerThreadId{};
std::atomic<bool> g_startupOk{false};
std::atomic<HWND> g_messageWindow{nullptr};
#ifdef WINDOWPILOT_TEST
std::atomic<HWND> g_testOnlyWindow{nullptr};
#endif

struct State {
    Config config;
    HINSTANCE module{};
    HWND host{}, toast{};
    HHOOK keyboard{};
    HWINEVENTHOOK foreground{};
    std::array<bool, 256> down{}, suppressed{};
    ConfirmationGate confirmation;
    bool pendingNotification{};
    ULONGLONG notificationAt{};
    std::wstring primaryText, secondaryText;
    wchar_t icon = L'\xE946';
    HFONT mainFont{}, detailFont{}, iconFont{};
    UINT dpi = 96;
    bool light{};
    bool hostClassRegistered{}, toastClassRegistered{};
    bool errorHint{};
};
thread_local State* t_state{};

Config SnapshotConfig() {
    std::lock_guard lock(g_configMutex);
    return g_config;
}
void HideToast(State& state) {
    KillTimer(state.host, kHideHint);
    ShowWindow(state.toast, SW_HIDE);
}
void CancelPending(State& state) {
    state.confirmation.Reset();
    state.pendingNotification = false;
    KillTimer(state.host, kExpireConfirmation);
    KillTimer(state.host, kNotifyTimer);
    HideToast(state);
}
int Scaled(const State& state, int value) { return MulDiv(value, state.dpi, 96); }
void DeleteFonts(State& state) {
    if (state.mainFont) DeleteObject(state.mainFont);
    if (state.detailFont) DeleteObject(state.detailFont);
    if (state.iconFont) DeleteObject(state.iconFont);
    state.mainFont = state.detailFont = state.iconFont = nullptr;
}
void UpdateFonts(State& state, UINT dpi) {
    DeleteFonts(state);
    state.dpi = dpi ? dpi : 96;
    state.mainFont = CreateFontW(-Scaled(state, 14), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH, L"Segoe UI");
    state.detailFont = CreateFontW(-Scaled(state, 12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH, L"Segoe UI");
    state.iconFont = CreateFontW(-Scaled(state, 20), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH, L"Segoe Fluent Icons");
}
void PaintToast(State& state, HDC dc, const RECT& rect) {
    auto fill = CreateSolidBrush(state.light ? RGB(250,250,250) : RGB(32,34,39));
    FillRect(dc, &rect, fill); DeleteObject(fill);
    auto stroke = CreatePen(PS_SOLID, 1, state.light ? RGB(204,206,210) : RGB(76,80,89));
    auto oldPen = SelectObject(dc, stroke);
    auto oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    RoundRect(dc, 0, 0, rect.right, rect.bottom, Scaled(state, 16), Scaled(state, 16));
    SelectObject(dc, oldPen); SelectObject(dc, oldBrush); DeleteObject(stroke);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, state.errorHint ? (state.light ? RGB(145,39,32) : RGB(255,190,183)) :
                                     (state.light ? RGB(0,88,155) : RGB(143,205,255)));
    auto oldFont = SelectObject(dc, state.iconFont);
    RECT icon{Scaled(state,16), 0, Scaled(state,44), rect.bottom};
    DrawTextW(dc, &state.icon, 1, &icon, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SetTextColor(dc, state.light ? RGB(24,26,30) : RGB(245,246,249));
    SelectObject(dc, state.mainFont);
    RECT main{Scaled(state,56), Scaled(state,14), rect.right-Scaled(state,18), Scaled(state,37)};
    DrawTextW(dc, state.primaryText.c_str(), -1, &main, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    SelectObject(dc, state.detailFont);
    SetTextColor(dc, state.light ? RGB(80,84,92) : RGB(195,200,210));
    RECT detail{Scaled(state,56), Scaled(state,42), rect.right-Scaled(state,18), Scaled(state,63)};
    DrawTextW(dc, state.secondaryText.c_str(), -1, &detail, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    SelectObject(dc, oldFont);
}
void ShowToast(State& state, HWND target, std::wstring primary, std::wstring secondary,
               wchar_t icon, UINT duration, bool error = false) {
    if (!state.toast) return;
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST), &monitor);
    RECT targetRect = monitor.rcWork;
    if (target) GetWindowRect(target, &targetRect);
    UpdateFonts(state, target ? GetDpiForWindow(target) : 96);
    DWORD light = 0, size = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    state.light = ResolveLightTheme(state.config.theme, light != 0);
    state.primaryText = std::move(primary); state.secondaryText = std::move(secondary);
    state.icon = icon; state.errorHint = error;
    int available = monitor.rcWork.right - monitor.rcWork.left - Scaled(state,24);
    int width = std::min(Scaled(state,620), std::max(1, available));
    int height = Scaled(state,78);
    int x = targetRect.left + (targetRect.right-targetRect.left-width)/2;
    int y = targetRect.top + Scaled(state,16);
    x = std::clamp(x, static_cast<int>(monitor.rcWork.left+Scaled(state,12)),
                     static_cast<int>(monitor.rcWork.right-width-Scaled(state,12)));
    y = std::clamp(y, static_cast<int>(monitor.rcWork.top+Scaled(state,12)),
                     static_cast<int>(monitor.rcWork.bottom-height-Scaled(state,12)));
    SetWindowTextW(state.toast, (L"WindowPilot: " + state.primaryText + L". " + state.secondaryText).c_str());
    // Layered + transparent + no-activate: the hint never steals clicks or keyboard focus.
    SetLayeredWindowAttributes(state.toast, 0, 255, LWA_ALPHA);
    HRGN region = CreateRoundRectRgn(0, 0, width+1, height+1, Scaled(state,16), Scaled(state,16));
    if (region && !SetWindowRgn(state.toast, region, FALSE)) DeleteObject(region);
    if (!state.config.alwaysOnTop)
        SetWindowPos(state.toast, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetWindowPos(state.toast, state.config.alwaysOnTop ? HWND_TOPMOST : HWND_TOP,
                 x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(state.toast, nullptr, FALSE);
    UpdateWindow(state.toast);
    SetTimer(state.host, kHideHint, duration, nullptr);
    NotifyWinEvent(EVENT_SYSTEM_ALERT, state.toast, OBJID_CLIENT, CHILDID_SELF);
}
void ShowError(State& state, HWND target, const std::wstring& detail) {
    ShowToast(state, target, Text(state.config.language,
        L"WindowPilot could not perform the action", L"WindowPilot não executou a ação",
        L"WindowPilot no pudo ejecutar la acción"), detail, L'\xE7BA', 3000, true);
}

bool IsActionWindow(HWND window) {
    if (!window || !IsWindow(window) || !IsWindowVisible(window) || !IsWindowEnabled(window) ||
        window == GetShellWindow() || window == GetDesktopWindow()) return false;
    wchar_t cls[256]{}; GetClassNameW(window, cls, ARRAYSIZE(cls));
    for (auto skip : {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
                      L"#32768", kToastClass, kHostClass}) if (wcscmp(cls, skip) == 0) return false;
    auto ex = GetWindowLongPtrW(window, GWL_EXSTYLE);
    return !(ex & WS_EX_NOACTIVATE) && !(GetWindowLongPtrW(window, GWL_STYLE) & WS_CHILD);
}
void ExecuteAction(State& state, Action action, HWND window) {
    if (!IsActionWindow(window) || GetAncestor(GetForegroundWindow(), GA_ROOT) != window) return;
    auto style = GetWindowLongPtrW(window, GWL_STYLE);
    WPARAM command = SC_CLOSE;
    if (action == Minimize) {
        if (!(style & WS_MINIMIZEBOX)) { ShowError(state, window, Text(state.config.language,
            L"This window cannot be minimized.", L"Esta janela não permite minimizar.", L"Esta ventana no se puede minimizar.")); return; }
        command = SC_MINIMIZE;
    } else if (action == Maximize) {
        if (IsZoomed(window)) {
            if (!state.config.restoreIfMaximized) return;
            command = SC_RESTORE;
        } else {
            if (!(style & WS_MAXIMIZEBOX)) { ShowError(state, window, Text(state.config.language,
                L"This window cannot be maximized.", L"Esta janela não permite maximizar.", L"Esta ventana no se puede maximizar.")); return; }
            command = SC_MAXIMIZE;
        }
    } else {
        HMENU menu = GetSystemMenu(window, FALSE);
        if (menu && (GetMenuState(menu, SC_CLOSE, MF_BYCOMMAND) & (MF_DISABLED | MF_GRAYED))) {
            ShowError(state, window, Text(state.config.language,
                L"The application disabled closing this window.", L"O aplicativo desativou o fechamento desta janela.",
                L"La aplicación desactivó el cierre de esta ventana.")); return;
        }
    }
    // Normal window request, never TerminateProcess, taskkill, or a forced close.
    if (!PostMessageW(window, WM_SYSCOMMAND, command, 0)) {
        DWORD error = GetLastError();
        Wh_Log(L"Window action %u failed, error=%lu", static_cast<UINT>(action), error);
        ShowError(state, window, error == ERROR_ACCESS_DENIED ? Text(state.config.language,
            L"Windows blocked the command because of permission levels.",
            L"O Windows bloqueou o comando por diferença de permissões.",
            L"Windows bloqueó el comando por una diferencia de permisos.") : Text(state.config.language,
            L"The window did not accept the command.", L"A janela não aceitou o comando.", L"La ventana no aceptó el comando."));
    }
}
void HandleAction(State& state, Action action, HWND window) {
    if (!state.config.enabled || action >= ActionCount || !state.config.actions[action].enabled ||
        !state.config.actions[action].hotkey.valid) return;
    if (GetAncestor(GetForegroundWindow(), GA_ROOT) != window) { CancelPending(state); return; }
    if (action == Notifications) {
        CancelPending(state);
        state.pendingNotification = true; state.notificationAt = GetTickCount64();
        SetTimer(state.host, kNotifyTimer, 30, nullptr);
        return;
    }
    state.pendingNotification = false; KillTimer(state.host, kNotifyTimer);
    if (!IsActionWindow(window)) { CancelPending(state); return; }
    const auto& options = state.config.actions[action];
    if (options.ignoreGames && IsGame(window, state.config)) { CancelPending(state); return; }
    DWORD pid{}; DWORD tid = GetWindowThreadProcessId(window, &pid);
    if (options.doublePress && !state.confirmation.Press(action, window, pid, tid, GetTickCount64(), options.confirmationMs)) {
        wchar_t title[512]{}; GetWindowTextW(window, title, ARRAYSIZE(title));
        auto lang = state.config.language;
        const wchar_t* verb = action == Close ? Text(lang, L"close", L"fechar", L"cerrar") :
            action == Minimize ? Text(lang, L"minimize", L"minimizar", L"minimizar") :
            IsZoomed(window) && state.config.restoreIfMaximized ? Text(lang, L"restore", L"restaurar", L"restaurar") :
            Text(lang, L"maximize", L"maximizar", L"maximizar");
        auto text = std::wstring(Text(lang, L"Press ", L"Pressione ", L"Pulsa ")) + DisplayHotkey(options.shortcut) +
            Text(lang, L" once more to ", L" mais uma vez para ", L" otra vez para ") + verb;
        std::wstring detail = title[0] ? title : Text(lang, L"Current window", L"Janela atual", L"Ventana actual");
        detail += Text(lang, L"  ·  Esc cancels", L"  ·  Esc cancela", L"  ·  Esc cancela");
        ShowToast(state, window, text, detail, action == Close ? L'\xE8BB' :
                  action == Minimize ? L'\xE921' : L'\xE922', options.hintMs);
        SetTimer(state.host, kExpireConfirmation, options.confirmationMs, nullptr);
        return;
    }
    CancelPending(state);
    ExecuteAction(state, action, window);
}

UINT CurrentModifiers(const State& state) {
    const auto& d = state.down;
    return ((d[VK_LWIN] || d[VK_RWIN]) ? MOD_WIN : 0) |
           ((d[VK_LCONTROL] || d[VK_RCONTROL] || d[VK_CONTROL]) ? MOD_CONTROL : 0) |
           ((d[VK_LMENU] || d[VK_RMENU] || d[VK_MENU]) ? MOD_ALT : 0) |
           ((d[VK_LSHIFT] || d[VK_RSHIFT] || d[VK_SHIFT]) ? MOD_SHIFT : 0);
}
INPUT KeyInput(WORD key, bool up) {
    INPUT input{}; input.type = INPUT_KEYBOARD;
    input.ki.wVk = key; input.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0; input.ki.dwExtraInfo = kOwnInput;
    return input;
}
void MaskModifierMenu() {
    // An unassigned virtual key marks a consumed Win/Alt chord as used; it
    // prevents the Start menu/menu bar from opening when the modifier is released.
    INPUT mask[] = {KeyInput(0xE8, false), KeyInput(0xE8, true)};
    SendInput(ARRAYSIZE(mask), mask, sizeof(INPUT));
}
LRESULT CALLBACK KeyboardProc(int code, WPARAM message, LPARAM parameter) {
    if (code != HC_ACTION || !t_state) return CallNextHookEx(nullptr, code, message, parameter);
    const auto& event = *reinterpret_cast<KBDLLHOOKSTRUCT*>(parameter);
    if (event.dwExtraInfo == kOwnInput || event.vkCode >= 256)
        return CallNextHookEx(nullptr, code, message, parameter);
    auto& state = *t_state;
    UINT key = event.vkCode;
    bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
    bool wasDown = state.down[key];
    state.down[key] = !up;
    if (state.suppressed[key]) {
        if (up) state.suppressed[key] = false;
        return 1;  // Repeats cannot execute or confirm, and the matching key-up is consumed.
    }
    if (up || wasDown) return CallNextHookEx(nullptr, code, message, parameter);
    if (key == VK_ESCAPE) PostMessageW(state.host, kCancelMessage, 0, 0);
    auto modifiers = CurrentModifiers(state);
    int action = FindAction(state.config, key, modifiers);
    if (action < 0) return CallNextHookEx(nullptr, code, message, parameter);
    HWND target = GetAncestor(GetForegroundWindow(), GA_ROOT);
#ifdef WINDOWPILOT_TEST
    // Standalone tests must never control a user's application.
    if (target != g_testOnlyWindow.load()) return CallNextHookEx(nullptr, code, message, parameter);
#endif
    if (!target || !PostMessageW(state.host, kActionMessage, action, reinterpret_cast<LPARAM>(target)))
        return CallNextHookEx(nullptr, code, message, parameter);
    state.suppressed[key] = true;
    if (modifiers & (MOD_WIN | MOD_ALT)) MaskModifierMenu();
    return 1;
}
void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND window, LONG, LONG, DWORD, DWORD) {
    if (t_state && t_state->confirmation.armed && window != t_state->confirmation.window)
        CancelPending(*t_state);
}
bool AnyModifierDown() {
    for (auto key : {VK_LWIN, VK_RWIN, VK_CONTROL, VK_MENU, VK_SHIFT})
        if (GetAsyncKeyState(key) & 0x8000) return true;
    return false;
}
bool NotificationCenterDisabled() {
    for (HKEY hive : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        DWORD disabled{}, size = sizeof(disabled);
        if (RegGetValueW(hive, L"Software\\Policies\\Microsoft\\Windows\\Explorer",
                         L"DisableNotificationCenter", RRF_RT_REG_DWORD, nullptr,
                         &disabled, &size) == ERROR_SUCCESS && disabled != 0) return true;
    }
    return false;
}
void SendNativeNotifications(State& state) {
    if (!state.pendingNotification) return;
    if (GetTickCount64() - state.notificationAt > 5000) {
        CancelPending(state); return;
    }
    if (AnyModifierDown()) return;
    state.pendingNotification = false; KillTimer(state.host, kNotifyTimer);
    if (NotificationCenterDisabled()) {
        ShowError(state, GetForegroundWindow(), Text(state.config.language,
            L"Notification Center is disabled by a Windows policy.",
            L"A central de notificações está desativada por uma política do Windows.",
            L"El centro de notificaciones está desactivado por una directiva de Windows."));
        return;
    }
    INPUT input[] = {KeyInput(VK_LWIN, false), KeyInput(L'N', false),
                     KeyInput(L'N', true), KeyInput(VK_LWIN, true)};
    if (SendInput(ARRAYSIZE(input), input, sizeof(INPUT)) != ARRAYSIZE(input))
        ShowError(state, GetForegroundWindow(), Text(state.config.language,
            L"Notifications could not be opened over this window.", L"Não foi possível abrir as notificações nesta janela.",
            L"No se pudieron abrir las notificaciones sobre esta ventana."));
}
void ReportConfigErrors(State& state) {
    for (const auto& error : state.config.errors) Wh_Log(L"Configuration: %s", error.c_str());
    if (state.config.enabled && !state.config.errors.empty())
        ShowToast(state, GetForegroundWindow(), Text(state.config.language,
            L"Review your WindowPilot shortcuts", L"Revise os atalhos do WindowPilot", L"Revisa los atajos de WindowPilot"),
            state.config.errors.front() + Text(state.config.language,
                L". This binding was disabled.", L". A combinação foi desativada.", L". Se desactivó esta combinación."), L'\xE7BA', 5000, true);
}
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<State*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        state = static_cast<State*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
    }
    if (!state) return DefWindowProcW(window, message, wParam, lParam);
    if (window == state->toast) {
        if (message == WM_NCHITTEST) return HTTRANSPARENT;
        if (message == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
        if (message == WM_ERASEBKGND) return 1;
        if (message == WM_PRINTCLIENT) {
            RECT rect{}; GetClientRect(window, &rect);
            PaintToast(*state, reinterpret_cast<HDC>(wParam), rect); return 0;
        }
        if (message == WM_PAINT) {
            PAINTSTRUCT ps{}; HDC dc = BeginPaint(window, &ps); RECT rect{};
            GetClientRect(window, &rect); PaintToast(*state, dc, rect); EndPaint(window, &ps); return 0;
        }
    }
    if (message == kActionMessage) {
        if (wParam < ActionCount) HandleAction(*state, static_cast<Action>(wParam), reinterpret_cast<HWND>(lParam));
        return 0;
    }
    if (message == kCancelMessage) { CancelPending(*state); return 0; }
    if (message == kReloadMessage) {
        CancelPending(*state); state->config = SnapshotConfig(); ReportConfigErrors(*state); return 0;
    }
    if (message == WM_TIMER) {
        if (wParam == kHideHint) HideToast(*state);
        else if (wParam == kExpireConfirmation) {
            state->confirmation.Reset(); KillTimer(window, kExpireConfirmation);
        } else if (wParam == kNotifyTimer) SendNativeNotifications(*state);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
bool RegisterWindowClass(State& state, const wchar_t* name, bool& registered) {
    WNDCLASSEXW cls{sizeof(cls)};
    cls.lpfnWndProc = WindowProc; cls.hInstance = state.module; cls.lpszClassName = name;
    registered = RegisterClassExW(&cls) != 0;
    return registered;
}
void Cleanup(State& state) {
    g_messageWindow = nullptr;
    if (state.keyboard) UnhookWindowsHookEx(state.keyboard);
    if (state.foreground) UnhookWinEvent(state.foreground);
    if (state.host) CancelPending(state);
    if (state.toast) DestroyWindow(state.toast);
    if (state.host) DestroyWindow(state.host);
    DeleteFonts(state);
    if (state.toastClassRegistered) UnregisterClassW(kToastClass, state.module);
    if (state.hostClassRegistered) UnregisterClassW(kHostClass, state.module);
    t_state = nullptr;
}
bool HasElevatedInputAccess() {
    HANDLE token{};
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return false;
    DWORD uiAccess{}, bytes{};
    TOKEN_ELEVATION elevation{};
    bool access = GetTokenInformation(token, TokenUIAccess, &uiAccess, sizeof(uiAccess), &bytes) && uiAccess;
    access |= GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &bytes) && elevation.TokenIsElevated;
    CloseHandle(token);
    return access;
}
DWORD WINAPI WorkerMain(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG message{}; PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    DWORD session{}; ProcessIdToSessionId(GetCurrentProcessId(), &session);
    auto mutexName = L"Local\\WindowPilot.Keyboard." + std::to_wstring(session);
    // The standard-user hook cannot observe input over elevated windows. Keep
    // one fallback in Explorer and one UIAccess/elevated hook in the user's
    // text-input host. Consuming a key stops the hook chain, preventing duplicates.
    mutexName += HasElevatedInputAccess() ? L".Accessible" : L".Standard";
#ifdef WINDOWPILOT_TEST
    mutexName += L".Tests";
#endif
    HANDLE owner = CreateMutexW(nullptr, FALSE, mutexName.c_str());
    if (!owner) { SetEvent(g_readyEvent); return 1; }
    // Only one Explorer process owns the global hook; a standby takes over if
    // that process exits. The stop event also wakes standby threads on unload.
    HANDLE waits[] = {g_stopEvent, owner};
    DWORD acquired = WaitForMultipleObjects(2, waits, FALSE, 0);
    bool announced = false;
    if (acquired == WAIT_TIMEOUT) {
        g_startupOk = true; SetEvent(g_readyEvent); announced = true;
        acquired = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    }
    if (acquired != WAIT_OBJECT_0+1 && acquired != WAIT_ABANDONED_0+1) {
        CloseHandle(owner); if (!announced) SetEvent(g_readyEvent); return 0;
    }
    State state; t_state = &state; state.config = SnapshotConfig();
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<LPCWSTR>(&WorkerMain), &state.module);
    bool success = RegisterWindowClass(state, kHostClass, state.hostClassRegistered) &&
                   RegisterWindowClass(state, kToastClass, state.toastClassRegistered);
    if (success) {
        state.host = CreateWindowExW(0, kHostClass, L"WindowPilot", 0, 0,0,0,0, HWND_MESSAGE,
                                     nullptr, state.module, &state);
        state.toast = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                                      WS_EX_LAYERED | WS_EX_TRANSPARENT,
            kToastClass, L"WindowPilot", WS_POPUP, 0,0,0,0, nullptr, nullptr, state.module, &state);
        success = state.host && state.toast;
    }
    if (success) {
        for (auto key : {VK_LWIN, VK_RWIN, VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU})
            state.down[key] = (GetAsyncKeyState(key) & 0x8000) != 0;
        state.foreground = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                          nullptr, ForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
        state.keyboard = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, state.module, 0);
        success = state.keyboard && state.foreground;
    }
    if (success) {
        g_messageWindow = state.host;
        ReportConfigErrors(state);
        Wh_Log(L"WindowPilot keyboard worker active");
    } else Wh_Log(L"WindowPilot initialization failed: %lu", GetLastError());
    g_startupOk = success;
    if (!announced) SetEvent(g_readyEvent);
    if (success) {
        bool run = true;
        while (run && WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
            DWORD wake = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
            if (wake == WAIT_OBJECT_0 || wake == WAIT_FAILED) break;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) { run = false; break; }
                TranslateMessage(&message); DispatchMessageW(&message);
                if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
            }
        }
    }
    Cleanup(state); ReleaseMutex(owner); CloseHandle(owner);
    return success ? 0 : 1;
}
void StopWorker() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread); g_workerThread = nullptr;
    }
    if (g_readyEvent) { CloseHandle(g_readyEvent); g_readyEvent = nullptr; }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
}
}  // namespace windowpilot

BOOL Wh_ModInit() {
    using namespace windowpilot;
    { std::lock_guard lock(g_configMutex); g_config = LoadConfig(); }
    g_startupOk = false;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_readyEvent) { StopWorker(); return FALSE; }
    g_workerThread = CreateThread(nullptr, 0, WorkerMain, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread || WaitForSingleObject(g_readyEvent, 5000) != WAIT_OBJECT_0 || !g_startupOk) {
        StopWorker(); return FALSE;
    }
    return TRUE;
}
void Wh_ModUninit() { windowpilot::StopWorker(); }
void Wh_ModSettingsChanged() {
    using namespace windowpilot;
    { std::lock_guard lock(g_configMutex); g_config = LoadConfig(); }
    if (auto window = g_messageWindow.load()) PostMessageW(window, kReloadMessage, 0, 0);
}
