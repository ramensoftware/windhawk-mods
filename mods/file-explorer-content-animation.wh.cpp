// ==WindhawkMod==
// @id              file-explorer-content-animation
// @name            File Explorer WinUI-Like Content Animation
// @name:pt    Animação de Conteúdo do Explorador de Arquivos estilo WinUI
// @name:es     Animación de Contenido del Explorador de Archivos estilo WinUI
// @description     Smooth WinUI-like slide animation for the File Explorer content area.
// @description:pt Animação suave de deslizamento inspirada no WinUI para a área de conteúdo do Explorador de Arquivos.
// @description:es Animación suave de deslizamiento inspirada en WinUI para el área de contenido del Explorador de Archivos.
// @version         1.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

![Preview](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/file-explorer-content-animation.gif)

---

## English

A smooth, WinUI-inspired slide animation for the File Explorer content area on Windows 11.

This mod enhances folder navigation with a subtle motion design approach, similar to modern WinUI transitions, while preserving system stability and performance.

### ✨ Features

- Smooth slide animation with refined easing
- Per-window first open delay (optional)
- Navigation delay to hide the "Working on it..." message
- Zero flicker
- No black flashes
- No interference with thumbnails
- No DirectComposition breakage
- Lightweight and CPU-friendly
- Compatible with the Translucent Windows mod

### 🧠 How It Works

- The mod temporarily offsets the content view outside the visible area
- Waits for layout stabilization
- Restores position
- Plays a smooth eased slide animation

This approach keeps the DirectComposition pipeline intact, ensuring stability even on recent Windows 11 builds (including 25H2).

### ⚙️ Settings

- **Animation Duration** – Controls how long the slide lasts.
- **Slide Distance** – Controls how far the content moves before settling.
- **First Open Delay** – Applies only when a new Explorer window is opened.
- **Navigation Delay** – Brief delay during folder switching to hide loading text.

---

## Português

Uma animação suave de deslizamento inspirada no WinUI para a área de conteúdo do Explorador de Arquivos no Windows 11.

Este mod aprimora a navegação entre pastas com um movimento sutil e moderno, mantendo estabilidade total do sistema e ótimo desempenho.

### ✨ Recursos

- Animação suave com easing refinado
- Delay opcional na primeira abertura de cada janela
- Delay durante navegação para esconder o "Trabalhando nisso..."
- Sem flicker
- Sem flash preto
- Não interfere em miniaturas
- Não quebra o DirectComposition
- Leve e eficiente
- Compatível com o mod Translucent Windows

### 🧠 Como Funciona

- O mod move temporariamente a área de conteúdo para fora da área visível
- Aguarda estabilização do layout
- Restaura a posição original
- Executa uma animação suave com easing inspirado no WinUI

Isso mantém o pipeline de renderização intacto e estável mesmo nas builds mais recentes do Windows 11 (incluindo 25H2).

### ⚙️ Configurações

- **Duração da Animação** – Define quanto tempo a animação leva.
- **Distância do Slide** – Define o deslocamento inicial.
- **Delay na Primeira Abertura** – Aplicado apenas ao abrir uma nova janela.
- **Delay ao Trocar de Pasta** – Pequeno atraso para esconder o texto de carregamento.

---

## Español

Una animación suave de deslizamiento inspirada en WinUI para el área de contenido del Explorador de Archivos en Windows 11.

Este mod mejora la navegación entre carpetas con un movimiento moderno y sutil, manteniendo total estabilidad del sistema y un excelente rendimiento.

### ✨ Características

- Animación suave con easing refinado
- Retraso opcional en la primera apertura de cada ventana
- Retraso durante la navegación para ocultar "Trabajando en ello..."
- Sin parpadeos
- Sin flashes negros
- No interfiere con miniaturas
- No rompe DirectComposition
- Ligero y eficiente
- Compatible con el mod Translucent Windows

### 🧠 Cómo Funciona

- El mod mueve temporalmente el área de contenido fuera del área visible
- Espera a que el layout se estabilice
- Restaura la posición original
- Ejecuta una animación suave inspirada en WinUI

Esto mantiene intacto el pipeline de renderizado incluso en builds recientes de Windows 11 (incluyendo 25H2).

### ⚙️ Configuración

- **Duración de la Animación** – Controla cuánto dura el efecto.
- **Distancia del Deslizamiento** – Define el desplazamiento inicial.
- **Retraso en la Primera Apertura** – Solo al abrir una nueva ventana.
- **Retraso al Cambiar de Carpeta** – Pequeño retraso para ocultar el texto de carga.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration: 420
  $name: Duração da animação (ms)
  $name:en: Animation duration (ms)
  $name:es: Duración de la animación (ms)

- distance: 32
  $name: Distância do slide (px)
  $name:en: Slide distance (px)
  $name:es: Distancia del deslizamiento (px)

- firstOpenDelay: 500
  $name: Delay na primeira abertura (ms)
  $name:en: First open delay (ms)
  $name:es: Retraso en la primera apertura (ms)

  $description: Atraso aplicado apenas na primeira animação de cada janela do Explorer.
  $description:en: Delay applied only to the first animation of each Explorer window.
  $description:es: Retraso aplicado solo en la primera animación de cada ventana del Explorador.

- navigationDelay: 140
  $name: Delay ao trocar de pasta (ms)
  $name:en: Folder navigation delay (ms)
  $name:es: Retraso al cambiar de carpeta (ms)

  $description: Tempo para esconder o "Trabalhando nisso..." durante navegação.
  $description:en: Time used to hide the "Working on it..." message during navigation.
  $description:es: Tiempo utilizado para ocultar el mensaje "Trabajando en ello..." durante la navegación.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <dwmapi.h>

struct Settings {
    int duration;
    int distance;
    int firstOpenDelay;
    int navigationDelay;
};

static Settings g_s{420, 32, 500, 140};

static void LoadSettings() {
    g_s.duration = std::clamp(Wh_GetIntSetting(L"duration"), 60, 800);
    g_s.distance = std::clamp(Wh_GetIntSetting(L"distance"), 0, 120);
    g_s.firstOpenDelay = std::clamp(Wh_GetIntSetting(L"firstOpenDelay"), 0, 5000);
    g_s.navigationDelay = std::clamp(Wh_GetIntSetting(L"navigationDelay"), 0, 500);
}

static inline float EaseOutWithFade(float t)
{
    // Pequena zona morta inicial (~12%)
    const float fadeZone = 0.12f;

    if (t < fadeZone)
    {
        float local = t / fadeZone;
        return local * local * 0.05f; 
        // movimenta só 5% no começo
    }

    float adjusted = (t - fadeZone) / (1.f - fadeZone);

    float inv = 1.f - adjusted;
    return 0.05f + (1.f - inv * inv * inv * inv) * 0.95f;
}

static std::unordered_map<HWND,bool> g_animating;
static std::mutex g_animMtx;
static std::unordered_map<HWND,bool> g_rootDelayApplied;
static std::mutex g_rootDelayMtx;

static bool IsExplorerContent(HWND hwnd)
{
    wchar_t c[128];
    GetClassNameW(hwnd, c, 128);

    if (wcscmp(c, L"UIItemsView") &&
        wcscmp(c, L"ItemsView") &&
        wcscmp(c, L"SHELLDLL_DefView"))
        return false;

    wchar_t rc[64];
    GetClassNameW(GetAncestor(hwnd, GA_ROOT), rc, 64);

    return !wcscmp(rc, L"CabinetWClass");
}

static void RunSlideIn(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return;

    HWND parent = GetParent(hwnd);
    if (!parent)
        return;

    RECT wrc{};
    GetWindowRect(hwnd, &wrc);

    POINT pt{ wrc.left, wrc.top };
    ScreenToClient(parent, &pt);

    int origX = pt.x;
    int origY = pt.y;

    int startY = origY + g_s.distance;
    int endY   = origY;

    SetWindowPos(hwnd, nullptr,
        origX, startY,
        0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    auto t0 = std::chrono::steady_clock::now();

    for (;;)
    {
        float elapsed = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - t0
        ).count();

        float t = std::min(elapsed / (float)g_s.duration, 1.f);
        float e = EaseOutWithFade(t);

        int cy = startY - (int)std::round(e * (startY - endY));

        SetWindowPos(hwnd, nullptr,
            origX, cy,
            0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

        if (t >= 1.f)
            break;

        DwmFlush();
        Sleep(1);
    }

    SetWindowPos(hwnd, nullptr,
        origX, endY,
        0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    std::lock_guard<std::mutex> lk(g_animMtx);
    g_animating.erase(hwnd);
}

// ─────────────────────────────────────────
// Hook ShowWindow (gatilho principal)
// ─────────────────────────────────────────

using ShowWindow_t = BOOL(WINAPI*)(HWND,int);
static ShowWindow_t g_origSW = nullptr;
static thread_local bool g_inSW = false;

BOOL WINAPI HookShowWindow(HWND hwnd, int cmd)
{
    if (!g_inSW &&
        !IsWindowVisible(hwnd) &&
        (cmd == SW_SHOW || cmd == SW_SHOWNA ||
         cmd == SW_SHOWNORMAL || cmd == SW_SHOWNOACTIVATE) &&
        IsExplorerContent(hwnd))
    {
        {
    std::lock_guard<std::mutex> lk(g_animMtx);
    if (g_animating.count(hwnd))
        return g_origSW(hwnd, cmd);

    g_animating[hwnd] = true;
}

HWND cap = hwnd;
HWND root = GetAncestor(cap, GA_ROOT);

bool applyFirstDelay = false;

{
    std::lock_guard<std::mutex> lk(g_rootDelayMtx);

    for (auto it = g_rootDelayApplied.begin(); it != g_rootDelayApplied.end(); ) {
        if (!IsWindow(it->first))
            it = g_rootDelayApplied.erase(it);
        else
            ++it;
    }

    if (!g_rootDelayApplied[root]) {
        g_rootDelayApplied[root] = true;
        applyFirstDelay = true;
    }
}

HWND parent = GetParent(cap);
RECT wrc{};
POINT pt{};

int origX = 0;
int origY = 0;

if (parent && GetWindowRect(cap, &wrc)) {
    pt.x = wrc.left;
    pt.y = wrc.top;
    ScreenToClient(parent, &pt);

    origX = pt.x;
    origY = pt.y;

    // Move imediatamente para fora antes de mostrar
    SetWindowPos(cap, nullptr,
        origX, origY + 3000,
        0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

g_inSW = true;
BOOL r = g_origSW(cap, cmd);
g_inSW = false;

std::thread([cap, origX, origY, applyFirstDelay]() {

    int delayToUse = applyFirstDelay ?
        g_s.firstOpenDelay :
        g_s.navigationDelay;

    if (delayToUse > 0)
        Sleep(delayToUse);

    if (!IsWindow(cap))
        return;

    // Restaurar posição real
    SetWindowPos(cap, nullptr,
        origX, origY,
        0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    RunSlideIn(cap);

}).detach();

return r;
    }

    return g_origSW(hwnd, cmd);
}

// ─────────────────────────────────────────
// Windhawk entrypoints
// ─────────────────────────────────────────

BOOL Wh_ModInit()
{
    LoadSettings();

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(ShowWindow),
        reinterpret_cast<void*>(HookShowWindow),
        reinterpret_cast<void**>(&g_origSW));

    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
}
