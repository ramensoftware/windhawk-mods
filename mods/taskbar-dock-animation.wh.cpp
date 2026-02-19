// ==WindhawkMod==
// @id              taskbar-dock-animation
// @name              Taskbar Dock Animation
// @name:zh-CN        任务栏 Dock 动画
// @name:ja-JP        タスクバー Dock アニメーション
// @name:ko-KR        작업 표시줄 Dock 애니메이션
// @name:pt-BR        Animação Dock da Barra de Tarefas
// @name:it-IT        Animazione Dock della Barra delle applicazioni
// @description       Animates taskbar icons on mouse hover like in macOS
// @description:uk-UA Анімація іконок панелі завдань, як в macOS, при наведенні
// @description:zh-CN 类似 macOS 的任务栏图标悬停动画
// @description:ja-JP macOS のように、マウスホバーでタスクバーアイコンをアニメーション表示します
// @description:ko-KR macOS처럼 마우스를 올리면 작업 표시줄 아이콘이 애니메이션됩니다
// @description:pt-BR Anima os ícones da barra de tarefas ao passar o mouse, como no macOS
// @description:it-IT Anima le icone della barra delle applicazioni al passaggio del mouse, come su macOS
// @version           1.9.2
// @author            Ph0en1x-dev
// @github            https://github.com/Ph0en1x-dev
// @include           explorer.exe
// @architecture      x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Dock Animation

This mod adds a macOS-like taskbar animation.
The current version is **Beta-Testing** and not yet fully stable.

**Requires Windows 11 or newer to function.**

---

### ⚠️ Known Issues & Limitations
* Icons are sometimes clipped by the taskbar.
* Upscaled icons may appear slightly blurry.

"Taskbar height and icon size" Users: If you are using this mod to create a minimalistic taskbar, please keep the Max Scale at or below 130% to avoid clipping issues. This recommendation applies to other scaling mods as well.
Dell / Alienware Users: If you encounter specific issues on these devices, please reach out to me directly or open an issue so we can investigate together.
StartAllBack Compatibility: This mod does not and will not support StartAllBack. Supporting it would require a complete rewrite of the codebase, which is currently out of scope.
Feedback: Feel free to suggest new animation ideas or curves in the mod topic or via direct message!

You can experiment with the **radius** value to achieve the best result for your setup until proper fixes for the issues above are found.

---

## 🖼️ Preview
![Taskbar Dock Animation Preview](https://raw.githubusercontent.com/Ph0en1x-dev/Hlam/refs/heads/main/Screen-recording-2025-11-06-155028.gif)
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- AnimationType: 0
  $name: Animation Curve
  $name:uk-UA: Крива анімації
  $name:zh-CN: 动画曲线
  $name:ja-JP: アニメーション曲線
  $name:ko-KR: 애니메이션 곡선
  $name:pt-BR: Curva de animação
  $name:it-IT: Curva di animazione
  $description: Choose the feel of the scaling (0 = Cosine (Smooth), 1 = Linear, 2 = Exponential (Snappy)
  $description:uk-UA: Оберіть тип анімації (0 = Косинус (Плавно), 1 = Лінійна, 2 = Експоненціальна (Швидко)
  $description:zh-CN: 选择缩放的动画感觉（0=余弦（平滑），1=线性，2=指数（更灵敏））
  $description:ja-JP: 拡大縮小の動き方を選択（0=コサイン（滑らか）、1=線形、2=指数（キビキビ））
  $description:ko-KR: 확대/축소 애니메이션 느낌 선택 (0=코사인(부드러움), 1=선형, 2=지수(빠릿함))
  $description:pt-BR: Escolha o tipo de escala (0 = Cosseno (suave), 1 = Linear, 2 = Exponencial (rápido))
  $description:it-IT: Scegli il tipo di scala (0 = Coseno (fluida), 1 = Lineare, 2 = Esponenziale (reattiva))
- MaxScale: 130
  $name: Maximum scale (%)
  $name:uk-UA: Максимальний розмір (%)
  $name:zh-CN: 最大缩放（%）
  $name:ja-JP: 最大倍率（%）
  $name:ko-KR: 최대 확대 비율(%)
  $name:pt-BR: Escala máxima (%)
  $name:it-IT: Scala massima (%)
  $description: How large an icon can grow (e.g., 160 = +60%)
  $description:uk-UA: Наскільки сильно іконка може збільшитись (напр., 160 = +60%)
  $description:zh-CN: 图标最大可放大到多少（例如 160 = 增加 60%）
  $description:ja-JP: "アイコンの最大拡大率（例: 160 = +60%）"
  $description:ko-KR: "아이콘이 커질 수 있는 최대 크기 (예: 160 = +60%)"
  $description:pt-BR: "Até quanto o ícone pode aumentar (ex.: 160 = +60%)"
  $description:it-IT: "Quanto può ingrandirsi un’icona (es.: 160 = +60%)"
- EffectRadius: 100
  $name: Effect radius (px)
  $name:uk-UA: Радіус ефекту (пкс)
  $name:zh-CN: 效果半径（像素）
  $name:ja-JP: 効果半径（px）
  $name:ko-KR: 효과 반경(px)
  $name:pt-BR: Raio do efeito (px)
  $name:it-IT: Raggio dell’effetto (px)
  $description: Distance from the cursor where the animation is applied
  $description:uk-UA: Дистанція від курсора, на якій застосовується анімація
  $description:zh-CN: 距离光标多远范围内应用动画
  $description:ja-JP: カーソルからの距離に応じてアニメーションを適用します
  $description:ko-KR: 커서로부터 애니메이션이 적용되는 거리
  $description:pt-BR: Distância do cursor em que a animação é aplicada
  $description:it-IT: Distanza dal cursore entro cui si applica l’animazione
- SpacingFactor: 50
  $name: Spacing sensitivity (%)
  $name:uk-UA: Чутливість до проміжку (%)
  $name:zh-CN: 间距灵敏度（%）
  $name:ja-JP: 間隔の感度（%）
  $name:ko-KR: 간격 민감도(%)
  $name:pt-BR: Sensibilidade do espaçamento (%)
  $name:it-IT: Sensibilità spaziatura (%)
  $description: How much neighboring icons move apart during animation (default 50%)
  $description:uk-UA: Наскільки сусідні іконки розсуваються під час анімації (за замовчуванням 50%)
  $description:zh-CN: 动画时相邻图标拉开的程度（默认 50%）
  $description:ja-JP: アニメーション中に隣のアイコンが離れる度合い（既定 50%）
  $description:ko-KR: 애니메이션 중 인접 아이콘이 벌어지는 정도(기본 50%)
  $description:pt-BR: Quanto os ícones vizinhos se afastam durante a animação (padrão 50%)
  $description:it-IT: Quanto si allontanano le icone vicine durante l’animazione (predef. 50%)
- BounceDelay: 500
  $name: Bounce start delay (ms)
  $name:uk-UA: Затримка початку "дихання" (мс)
  $name:zh-CN: 弹跳开始延迟（毫秒）
  $name:ja-JP: バウンス開始遅延（ms）
  $name:ko-KR: 바운스 시작 지연(ms)
  $name:pt-BR: Atraso para iniciar o bounce (ms)
  $name:it-IT: Ritardo di inizio bounce (ms)
  $description: How long to wait after the cursor stops before starting the idle "breathing" animation (default 100ms)
  $description:uk-UA: Скільки чекати після зупинки курсора перед початком анімації "дихання" (за замовчуванням 100мс)
  $description:zh-CN: 光标停止后等待多久开始“呼吸”动画（默认 100ms）
  $description:ja-JP: カーソル停止後、待機時の「呼吸」アニメを開始するまでの遅延（既定 100ms）
  $description:ko-KR: 커서가 멈춘 뒤 ‘호흡’ 애니메이션을 시작하기까지 대기 시간(기본 100ms)
  $description:pt-BR: Quanto esperar após o cursor parar antes de iniciar a animação “respirar” (padrão 100ms)
  $description:it-IT: Quanto attendere dopo che il cursore si ferma prima di avviare l’animazione “respiro” (predef. 100ms)
- FocusDuration: 150
  $name: Focus animation duration (ms)
  $name:uk-UA: Тривалість анімації фокусу (мс)
  $name:zh-CN: 聚焦动画时长（毫秒）
  $name:ja-JP: フォーカスアニメ時間（ms）
  $name:ko-KR: 포커스 애니메이션 지속 시간(ms)
  $name:pt-BR: Duração da animação de foco (ms)
  $name:it-IT: Durata animazione focus (ms)
  $description: Duration of the fade-in and fade-out animation (default 150ms)
  $description:uk-UA: Тривалість анімації плавного з'явлення та згасання (за замовчуванням 150мс)
  $description:zh-CN: 淡入/淡出动画时长（默认 150ms）
  $description:ja-JP: フェードイン/アウトの時間（既定 150ms）
  $description:ko-KR: 페이드 인/아웃 애니메이션 지속 시간(기본 150ms)
  $description:pt-BR: Duração do fade-in e fade-out (padrão 150ms)
  $description:it-IT: Durata della dissolvenza in entrata/uscita (predef. 150ms)
- MirrorForTopTaskbar: false
  $name: Mirror animation for top taskbar
  $name:uk-UA: Дзеркальна анімація для верхньої панелі
  $name:zh-CN: 顶部任务栏镜像动画
  $name:ja-JP: 上部タスクバー用に反転
  $name:ko-KR: 상단 작업 표시줄용 반전
  $name:pt-BR: Espelhar animação para barra no topo
  $name:it-IT: Specchia animazione per barra in alto
  $description: Enable this if your taskbar is at the top of the screen to make icons animate downwards.
  $description:uk-UA: Увімкніть, якщо ваша панель завдань знаходиться вгорі, щоб іконки анімувалися вниз.
  $description:zh-CN: 如果任务栏在屏幕顶部，启用此项让图标向下动画。
  $description:ja-JP: タスクバーが画面上部にある場合、アイコンを下方向に動かすために有効化します。
  $description:ko-KR: 작업 표시줄이 화면 상단에 있을 때 아이콘이 아래로 움직이도록 합니다.
  $description:pt-BR: Ative se a barra de tarefas estiver no topo para animar os ícones para baixo.
  $description:it-IT: Attiva se la barra è in alto per animare le icone verso il basso.
- DisableVerticalBounce: false
  $name: Disable vertical "bounce" animation
  $name:uk-UA: Вимкнути вертикальне "дихання"
  $name:zh-CN: 禁用垂直“弹跳”动画
  $name:ja-JP: 垂直バウンスを無効化
  $name:ko-KR: 수직 바운스 애니메이션 비활성화
  $name:pt-BR: Desativar bounce vertical
  $name:it-IT: Disattiva bounce verticale
  $description: Disables the up/down "breathing" animation. Useful for vertical taskbars.
  $description:uk-UA: Вимикає анімацію "дихання" вгору/вниз. Корисно для вертикальних панелей.
  $description:zh-CN: 禁用上下“呼吸”动画。对垂直任务栏有用。
  $description:ja-JP: 上下の「呼吸」アニメを無効化します。縦型タスクバーに便利です。
  $description:ko-KR: 위/아래 ‘호흡’ 애니메이션을 비활성화합니다. 세로 작업 표시줄에 유용합니다.
  $description:pt-BR: Desativa a animação de “respirar” para cima/baixo. Útil para barras verticais.
  $description:it-IT: Disattiva l’animazione “respiro” su/giù. Utile per barre verticali.
- TaskbarLabelsMode: false
  $name: Taskbar Labels Compatibility
  $name:uk-UA: Сумісність із підписами (Labels)
  $name:zh-CN: 任务栏标签兼容模式
  $name:ja-JP: タスクバーラベル互換
  $name:ko-KR: 작업 표시줄 라벨 호환
  $name:pt-BR: Compatibilidade com rótulos
  $name:it-IT: Compatibilità etichette
  $description: Enable this if your taskbar shows window titles (rectangular icons) to fix animation centering.
  $description:uk-UA: Увімкніть, якщо ваша панель завдань відображає назви вікон (прямокутні кнопки), щоб виправити центрування.
  $description:zh-CN: 若任务栏显示窗口标题（矩形按钮），启用此项以修正动画居中。
  $description:ja-JP: タスクバーにウィンドウ名（矩形ボタン）が表示される場合、中央揃えを修正するために有効化します。
  $description:ko-KR: 작업 표시줄에 창 제목(직사각형 버튼)이 표시될 때 애니메이션 중앙 정렬을 수정합니다.
  $description:pt-BR: Ative se a barra mostrar títulos de janelas (ícones retangulares) para corrigir o alinhamento.
  $description:it-IT: Abilita se la barra mostra i titoli delle finestre (pulsanti rettangolari) per correggere la centratura.
- ExcludeSystemButtonsMode: 0
  $name: Exclude system buttons
  $name:uk-UA: Ігнорувати системні кнопки
  $name:zh-CN: 排除系统按钮
  $name:ja-JP: システムボタンを除外
  $name:ko-KR: 시스템 버튼 제외
  $name:pt-BR: Excluir botões do sistema
  $name:it-IT: Escludi pulsanti di sistema
  $description: 0=Animate all, 1=Exclude Start, 2=Exclude Start/Search/TaskView/Widgets, 3=Animate apps only
  $description:uk-UA: 0=Анімація всього, 1=Без анімації Start, 2=Без Start/Search/TaskView/Widgets, 3=Анімація тільки іконок застосунків
  $description:zh-CN: 0=全部动画，1=排除开始，2=排除开始/搜索/任务视图/小组件，3=仅应用图标动画
  $description:ja-JP: 0=すべてアニメ、1=スタート除外、2=スタート/検索/タスクビュー/ウィジェット除外、3=アプリのみ
  $description:ko-KR: 0=전체, 1=시작 제외, 2=시작/검색/작업 보기/위젯 제외, 3=앱만
  $description:pt-BR: 0=Animar tudo, 1=Excluir Iniciar, 2=Excluir Iniciar/Pesquisa/Visão de tarefas/Widgets, 3=Somente apps
  $description:it-IT: 0=Anima tutto, 1=Escludi Start, 2=Escludi Start/Ricerca/Task View/Widget, 3=Solo app
- LerpSpeed: 60
  $name: Smoothing (Lerp speed)
  $name:uk-UA: Плавність (швидкість Lerp)
  $name:zh-CN: 平滑度（Lerp 速度）
  $name:ja-JP: スムージング（Lerp 速度）
  $name:ko-KR: 부드러움(Lerp 속도)
  $name:pt-BR: Suavização (velocidade do Lerp)
  $name:it-IT: Smussamento (velocità Lerp)
  $description: Higher = snappier, lower = smoother. 0 = disabled (direct)
  $description:uk-UA: Більше = швидше, менше = плавніше. 0 = вимкнути (напряму)
  $description:zh-CN: 越高越灵敏，越低越平滑。0=禁用（直接应用）
  $description:ja-JP: 大きいほどキビキビ、小さいほど滑らか。0=無効（直接適用）
  $description:ko-KR: 높을수록 빠릿함, 낮을수록 부드러움. 0=비활성화(직접)
  $description:pt-BR: Maior = mais rápido, menor = mais suave. 0 = desativado (direto)
  $description:it-IT: Più alto = più reattivo, più basso = più fluido. 0 = disattivato (diretto)
- DisableBounce: false
  $name: Disable bounce effect
  $name:uk-UA: Вимкнути bounce ефект
  $name:zh-CN: 禁用弹跳效果
  $name:ja-JP: バウンス効果を無効化
  $name:ko-KR: 바운스 효과 비활성화
  $name:pt-BR: Desativar efeito bounce
  $name:it-IT: Disattiva effetto bounce
  $description: Disables the idle "breathing" bounce entirely
  $description:uk-UA: Повністю вимикає bounce ("дихання")
  $description:zh-CN: 完全禁用空闲时的“呼吸”弹跳
  $description:ja-JP: 待機時の「呼吸」バウンスを完全に無効化します
  $description:ko-KR: 대기 상태 ‘호흡’ 바운스를 완전히 비활성화합니다
  $description:pt-BR: Desativa totalmente o bounce ocioso (“respirar”)
  $description:it-IT: Disattiva completamente il bounce inattivo (“respiro”)
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#undef GetCurrentTime
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Input.h>
#include <vector>
#include <atomic>
#include <functional>
#include <cmath>
#include <map>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <winrt/Windows.UI.Xaml.Automation.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Automation;

// Settings container
struct {
    int animationType;
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
    double focusDuration;
    bool mirrorAnimation;
    bool disableVerticalBounce;
    bool taskbarLabelsMode;
    int excludeSystemButtonsMode;
    double lerpSpeed;
    bool disableBounce;
} g_settings;


// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

// Animation Loop Globals
winrt::event_token g_renderingToken;
std::atomic<bool> g_isRenderingHooked = false;
std::atomic<void*> g_activeContextKey = nullptr;
std::chrono::steady_clock::time_point g_lastSignificantMoveTime;
const double MOUSE_STOP_THRESHOLD = 0.5;

// Bounce Animation Globals
std::atomic<double> g_lastMouseX = -1.0;
std::atomic<bool> g_isBouncing = false;
std::chrono::steady_clock::time_point g_bounceStartTime;
const double BOUNCE_PERIOD_MS = 1200.0;
const double BOUNCE_SCALE_AMOUNT = 0.05;
const double BOUNCE_TRANSLATE_Y = -4.0;

// Focus/Fade Animation Globals
std::atomic<bool> g_isMouseInside = false;
std::atomic<double> g_animationIntensity = 0.0;
std::chrono::steady_clock::time_point g_lastRenderTime;

struct IconAnimState {
    double waveScale = 1.0;
    double waveTranslateX = 0.0;
};

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    double elementWidth = 0.0;
    IconAnimState state;
};

struct HostSignature {
    int count = -1;
    double width = -1.0;
    uint64_t orderHash = 0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    winrt::weak_ref<FrameworkElement> iconHost;
    std::vector<TaskbarIconInfo> icons;

    HostSignature lastSig;
    std::chrono::steady_clock::time_point lastDirtyCheck{};
};

// One context per monitor (keyed by XAML instance pointer)
std::map<void*, DockAnimationContext> g_contexts;

// Prototypes
void LoadSettings();
void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec);
void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame);
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args);
void OnTaskbarPointerExited(void* pThis_key);
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons);
void RefreshIconPositions(DockAnimationContext& ctx);
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&);

HMODULE GetTaskbarViewModuleHandle();
bool HookTaskbarViewDllSymbols(HMODULE module);
static bool RebaseIconGeometryFast(DockAnimationContext& ctx);

// Simple visual tree helpers
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                          PCWSTR className) {
    if (!element) return nullptr;
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Animation math (cosine falloff)
double CalculateScale(double distance, double radius, double maxScale) {
    if (distance > radius) return 1.0;
    
    double t = distance / radius;
    double factor = 0.0;
    
    switch (g_settings.animationType) {
        case 1: // Linear
            factor = 1.0 - t;
            break;
        case 2: // Exponential
            factor = pow(1.0 - t, 3);
            break;
        case 0: // Cosine
        default:
            factor = (cos(t * 3.14159) + 1.0) / 2.0;
            break;
    }
    if (factor < 0) factor = 0;
    return (maxScale - 1.0) * factor + 1.0;
}

void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec) {
    const double spacingFactor = g_settings.spacingFactor;
    std::vector<double> scales(ctx.icons.size());
    std::vector<double> extraWidths(ctx.icons.size());
    double totalExpansion = 0.0;
    size_t closestIconIndex = (size_t)-1;
    double minDistance = g_settings.effectRadius + 1.0;
    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;
    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        double distance = 0.0;
        if (g_settings.taskbarLabelsMode) {
            double iconStart = ctx.icons[i].originalCenterX; 
            double iconEnd = iconStart + ctx.icons[i].elementWidth; 
            if (mouseX < iconStart) {
                distance = iconStart - mouseX; 
            } else if (mouseX > iconEnd) {
                distance = mouseX - iconEnd; 
            } else {
                distance = 0.0; 
            }
        } else {
            distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        }
        scales[i] = CalculateScale(distance, (double)g_settings.effectRadius, g_settings.maxScale);
        extraWidths[i] = (scales[i] - 1.0) * element.ActualWidth() * spacingFactor;
        totalExpansion += extraWidths[i];

        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateY = 0.0;

    if (g_isBouncing && closestIconIndex != (size_t)-1 &&
        minDistance < (g_settings.effectRadius / 2.0)) {       
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - g_bounceStartTime)
                            .count();
        double t = std::fmod(elapsed_ms, BOUNCE_PERIOD_MS) / BOUNCE_PERIOD_MS;
        double normalizedWave = std::sin(t * 3.14159);
        bounceScaleFactor = 1.0 + (normalizedWave * BOUNCE_SCALE_AMOUNT);
        bounceTranslateY = normalizedWave * BOUNCE_TRANSLATE_Y;       
        if (g_settings.mirrorAnimation) {
            bounceTranslateY *= -1.0;
        }
    }

    double alpha = 1.0;
    if (g_settings.lerpSpeed > 0.0) {
        alpha = 1.0 - std::exp(-g_settings.lerpSpeed * dtSec);
    }
    alpha = std::clamp(alpha, 0.0, 1.0);


    double cumulativeShift = 0.0;

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        auto tg = element.RenderTransform().try_as<TransformGroup>();
        if (!tg || tg.Children().Size() < 4) continue;
        auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
        auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
        auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
        auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

        if (!waveScale || !waveTranslate || !bounceScale || !bounceTranslate) continue;
        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double finalShift = cumulativeShift + selfShift - centerOffset;
        cumulativeShift += extraWidths[i];
        double targetScale = 1.0 + (scales[i] - 1.0) * intensity;
        double targetTranslateX = finalShift * intensity;

        if (g_settings.lerpSpeed > 0.0) {
            ctx.icons[i].state.waveScale += (targetScale - ctx.icons[i].state.waveScale) * alpha;
            ctx.icons[i].state.waveTranslateX += (targetTranslateX - ctx.icons[i].state.waveTranslateX) * alpha;
        } else {
            ctx.icons[i].state.waveScale = targetScale;
            ctx.icons[i].state.waveTranslateX = targetTranslateX;
        }

        waveScale.ScaleX(ctx.icons[i].state.waveScale);
        waveScale.ScaleY(ctx.icons[i].state.waveScale);
        waveTranslate.X(ctx.icons[i].state.waveTranslateX);

        if (i == closestIconIndex && g_isBouncing) {
            bounceScale.ScaleX(bounceScaleFactor);
            bounceScale.ScaleY(bounceScaleFactor);

            if (!g_settings.disableVerticalBounce) {
                bounceTranslate.Y(bounceTranslateY);
            } else {
                bounceTranslate.Y(0.0);
            }
        } else {
            bounceScale.ScaleX(1.0);
            bounceScale.ScaleY(1.0);
            bounceTranslate.Y(0.0);
        }
    }
}

void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
    try {
        for (auto& iconInfo : icons) {
            auto element = iconInfo.element.get();
            if (!element) continue;
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) continue;
            auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
            auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
            auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
            auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

            if (waveScale) {
                waveScale.ScaleX(1.0);
                waveScale.ScaleY(1.0);
            }
            if (waveTranslate) {
                waveTranslate.X(0.0);
            }
            if (bounceScale) {
                bounceScale.ScaleX(1.0);
                bounceScale.ScaleY(1.0);
            }
            if (bounceTranslate) {
                bounceTranslate.Y(0.0);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error in ResetAllIconScales: %s", e.message().c_str());
    }
}

// Event handlers
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args) {
    try {
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;
        auto& ctx = it->second;
        auto frame = ctx.taskbarFrame.get();
        if (!frame) return;       
        g_isMouseInside = true;
        g_activeContextKey = pThis_key;
        double mouseX = args.GetCurrentPoint(frame).Position().X;
        if (std::abs(mouseX - g_lastMouseX.load()) > MOUSE_STOP_THRESHOLD) {
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_lastMouseX = mouseX;
        if (!g_isRenderingHooked.exchange(true)) {
            g_lastRenderTime = std::chrono::steady_clock::now();
            g_renderingToken = Media::CompositionTarget::Rendering(OnCompositionTargetRendering);
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
            Wh_Log(L"DockAnimation: Started render loop (FocusIn).");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(void* pThis_key) {
    try {
        if (g_activeContextKey.load() == pThis_key) {
            g_isMouseInside = false;
            g_isBouncing = false;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

FrameworkElement FindChildByClassNamePartial(FrameworkElement element, PCWSTR partialName) {
    if (!element) return nullptr;
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) != std::wstring_view::npos;
    });
}

static FrameworkElement FindIconHost(FrameworkElement const& taskbarFrame) {
    FrameworkElement host = FindChildByClassNamePartial(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
    return host;
}

static uint64_t Fnv1aMix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

static HostSignature ComputeHostSignature(FrameworkElement const& host) {
    HostSignature sig;
    sig.count = VisualTreeHelper::GetChildrenCount(host);
    sig.width = host.ActualWidth();

    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < sig.count; i++) {
        auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
        if (!child) continue;
        h = Fnv1aMix(h, (uint64_t)winrt::get_abi(child));
        h = Fnv1aMix(h, (uint64_t)(child.ActualWidth() * 10.0));
    }
    sig.orderHash = h;
    return sig;
}

static bool SigDifferent(HostSignature const& a, HostSignature const& b) {
    if (a.count != b.count) return true;
    if (std::abs(a.width - b.width) > 0.5) return true;
    if (a.orderHash != b.orderHash) return true;
    return false;
}

enum class ButtonKind {
    App,
    Start,
    Search,
    TaskView,
    Widgets,
    Weather,
    SystemOther
};

static std::wstring W(winrt::hstring const& s) {
    return std::wstring(s.c_str());
}

static std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static ButtonKind ClassifyButton(FrameworkElement const& e) {
    if (!e) return ButtonKind::SystemOther;

    std::wstring cn     = W(winrt::get_class_name(e));
    std::wstring feName = W(e.Name());
    std::wstring aid    = W(AutomationProperties::GetAutomationId(e));
    std::wstring nm     = W(AutomationProperties::GetName(e));

    std::wstring hay = ToLower(cn + L"|" + feName + L"|" + aid + L"|" + nm);

    auto has = [&](std::wstring_view s) {
        return hay.find(std::wstring(s)) != std::wstring::npos;
    };

    if (has(L"startbutton") || has(L"start")) return ButtonKind::Start;
    if (has(L"searchbutton") || has(L"search")) return ButtonKind::Search;
    if (has(L"taskviewbutton") || has(L"task view") || has(L"taskview")) return ButtonKind::TaskView;
    if (has(L"widgetsbutton") || has(L"widgets")) return ButtonKind::Widgets;
    if (has(L"weather")) return ButtonKind::Weather;

    if (has(L"appid:")) return ButtonKind::App;

    {
        std::wstring cnL = ToLower(cn);
        if (cnL == L"taskbar.tasklistbutton" ||
            (cnL.size() >= 13 && cnL.rfind(L".tasklistbutton") == cnL.size() - 13)) {
            return ButtonKind::App;
        }
    }

    return ButtonKind::SystemOther;
}


static bool ShouldAnimateElement(FrameworkElement const& e) {
    const int mode = g_settings.excludeSystemButtonsMode;
    if (mode == 0) return true;

    ButtonKind k = ClassifyButton(e);

    if (mode == 3) return k == ButtonKind::App;
    if (mode == 2) return k == ButtonKind::App;
    if (mode == 1) return k != ButtonKind::Start;

    return true;
}

static void ResetElementTransforms(FrameworkElement const& element) {
    auto tg = element.RenderTransform().try_as<TransformGroup>();
    if (!tg || tg.Children().Size() < 4) return;

    auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
    auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
    auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
    auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

    if (waveScale) { waveScale.ScaleX(1.0); waveScale.ScaleY(1.0); }
    if (waveTranslate) { waveTranslate.X(0.0); }
    if (bounceScale) { bounceScale.ScaleX(1.0); bounceScale.ScaleY(1.0); }
    if (bounceTranslate) { bounceTranslate.Y(0.0); }
}

static bool RebaseIconGeometryFast(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return false;

    bool changed = false;

    for (auto it = ctx.icons.begin(); it != ctx.icons.end(); ) {
        if (!it->element.get()) {
            it = ctx.icons.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    for (auto& info : ctx.icons) {
        auto e = info.element.get();
        if (!e) continue;

        try {
            double oldX = info.originalCenterX;
            double oldW = info.elementWidth;

            auto t = e.TransformToVisual(frame);
            auto pt = t.TransformPoint({0, 0});
            double w = e.ActualWidth();
            info.elementWidth = w;

            float yOrigin = g_settings.disableVerticalBounce
                ? 0.5f
                : (g_settings.mirrorAnimation ? 0.0f : 1.0f);

            if (g_settings.taskbarLabelsMode) {
                double newStartX = pt.X;
                info.originalCenterX = newStartX;

                float iconCenterPx = 24.0f;
                float iconCenterProportion = (w > 0.0) ? (iconCenterPx / (float)w) : 0.5f;
                e.RenderTransformOrigin({ iconCenterProportion, yOrigin });

                if (std::abs(newStartX - oldX) > 0.5) changed = true;
            } else {
                double newCenterX = pt.X + (w / 2.0);
                info.originalCenterX = newCenterX;

                e.RenderTransformOrigin({ 0.5f, yOrigin });

                if (std::abs(newCenterX - oldX) > 0.5) changed = true;
            }

            if (std::abs(w - oldW) > 0.5) changed = true;
        } catch (...) {
        }
    }

    if (changed && ctx.icons.size() > 1) {
        std::sort(ctx.icons.begin(), ctx.icons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });
    }

    return changed;
}

// Builds or refreshes the icon list and sets up transforms for animation (scale + translate).
void RefreshIconPositions(DockAnimationContext& ctx) {
    // preserve state
    std::unordered_map<void*, IconAnimState> prevStates;
    std::unordered_set<void*> prevElements;

    for (auto& old : ctx.icons) {
        if (auto e = old.element.get()) {
            void* key = winrt::get_abi(e);
            prevStates[key] = old.state;
            prevElements.insert(key);
        }
    }

    std::vector<TaskbarIconInfo> newIcons;

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

    auto IsTaskListButtonExact = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButton";
    };

    auto IsTaskListButtonPanel = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButtonPanel";
    };

    std::function<FrameworkElement(FrameworkElement)> FindFirstTaskListButton =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;
            if (IsTaskListButtonExact(root)) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (IsTaskListButtonExact(ch)) return ch;
                auto deep = FindFirstTaskListButton(ch);
                if (deep) return deep;
            }
            return nullptr;
        };

    // classifier node
    std::function<FrameworkElement(FrameworkElement)> FindClassifierNode =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;

            auto aid = W(AutomationProperties::GetAutomationId(root));
            auto nm  = W(AutomationProperties::GetName(root));
            if (!aid.empty() || !nm.empty()) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                auto hit = FindClassifierNode(ch);
                if (hit) return hit;
            }
            return nullptr;
        };

    // candidate in fallback: TaskListButton + TaskListButtonPanel
    auto IsFallbackCandidate = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;

        if (IsTaskListButtonExact(e) || IsTaskListButtonPanel(e)) return true;

        auto cn = winrt::get_class_name(e);
        if (std::wstring_view(cn).find(L"Taskbar.") != std::wstring_view::npos) {
            auto aid = W(AutomationProperties::GetAutomationId(e));
            auto nm  = W(AutomationProperties::GetName(e));
            if (!aid.empty() || !nm.empty()) return true;
        }

        return false;
    };

    // Setup & add element (transform + icon geometry)
    auto SetupAndAddElement = [&](FrameworkElement const& element) {
        try {
            if (!element) return;

            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());       // 0 waveScale
                tg.Children().Append(TranslateTransform());   // 1 waveTranslate
                tg.Children().Append(ScaleTransform());       // 2 bounceScale
                tg.Children().Append(TranslateTransform());   // 3 bounceTranslate
                element.RenderTransform(tg);
            }

            float yOrigin = g_settings.disableVerticalBounce
                ? 0.5f
                : (g_settings.mirrorAnimation ? 0.0f : 1.0f);

            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({0, 0});

            TaskbarIconInfo info;
            info.element = element;
            info.elementWidth = element.ActualWidth();

            if (g_settings.taskbarLabelsMode) {
                float iconCenterPx = 24.0f;
                float iconCenterProportion =
                    (info.elementWidth > 0) ? (iconCenterPx / (float)info.elementWidth) : 0.5f;

                element.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                info.originalCenterX = point.X;
            } else {
                element.RenderTransformOrigin({ 0.5f, yOrigin });
                info.originalCenterX = point.X + (info.elementWidth / 2.0);
            }

            void* key = winrt::get_abi(element);

            auto it = prevStates.find(key);
            if (it != prevStates.end()) {
                info.state = it->second;

                auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
                auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
                if (waveScale) {
                    waveScale.ScaleX(info.state.waveScale);
                    waveScale.ScaleY(info.state.waveScale);
                }
                if (waveTranslate) {
                    waveTranslate.X(info.state.waveTranslateX);
                }
            }

            newIcons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in SetupAndAddElement: %s", e.message().c_str());
        }
    };

    // SMART host find (structure-based)
    auto FindSmartHost = [&]() -> FrameworkElement {
        std::vector<FrameworkElement> nodes;
        nodes.reserve(512);

        std::vector<FrameworkElement> stack;
        stack.reserve(512);
        stack.push_back(taskbarFrame);

        while (!stack.empty()) {
            auto cur = stack.back();
            stack.pop_back();
            if (!cur) continue;

            nodes.push_back(cur);

            int c = VisualTreeHelper::GetChildrenCount(cur);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(cur, i).try_as<FrameworkElement>();
                if (ch) stack.push_back(ch);
            }
        }

        std::function<bool(FrameworkElement,int)> SubtreeHasIconLike =
            [&](FrameworkElement root, int depth) -> bool {
                if (!root || depth <= 0) return false;
                if (IsTaskListButtonExact(root) || IsTaskListButtonPanel(root)) return true;

                int c = VisualTreeHelper::GetChildrenCount(root);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                    if (!ch) continue;
                    if (SubtreeHasIconLike(ch, depth - 1)) return true;
                }
                return false;
            };

        FrameworkElement best = nullptr;
        int bestHits = 0;
        double bestWidth = 0.0;

        for (auto const& n : nodes) {
            int c = VisualTreeHelper::GetChildrenCount(n);
            if (c < 3 || c > 120) continue;

            int hits = 0;
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(n, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (SubtreeHasIconLike(ch, 5)) hits++;
            }

            if (hits >= 3) {
                double w = (double)n.ActualWidth();
                if (hits > bestHits || (hits == bestHits && w > bestWidth)) {
                    best = n;
                    bestHits = hits;
                    bestWidth = w;
                }
            }
        }

        return best;
    };

    // host selection
    auto host = ctx.iconHost.get();
    if (!host) {
        host = FindSmartHost();
        if (host) ctx.iconHost = host;
    }

    try {
        if (host) {
            // MAIN: enumerate host children
            int count = VisualTreeHelper::GetChildrenCount(host);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
                if (!child) continue;

                FrameworkElement classifier = FindClassifierNode(child);
                FrameworkElement base = classifier ? classifier : child;

                bool animate = (g_settings.excludeSystemButtonsMode == 0)
                    ? true
                    : ShouldAnimateElement(base);

                // target: prefer TaskListButton inside, else child
                FrameworkElement target = FindFirstTaskListButton(child);
                if (!target) target = child;

                if (!animate) {
                    void* kT = winrt::get_abi(target);
                    if (prevElements.contains(kT)) ResetElementTransforms(target);

                    void* kC = winrt::get_abi(child);
                    if (prevElements.contains(kC)) ResetElementTransforms(child);
                    continue;
                }

                SetupAndAddElement(target);
            }

            ctx.lastSig = ComputeHostSignature(host);
        } else {
            // FALLBACK: recurse and include TaskListButtonPanel too
            std::function<void(FrameworkElement)> recurse = [&](FrameworkElement element) {
                if (!element) return;

                if (IsFallbackCandidate(element)) {
                    FrameworkElement classifier = FindClassifierNode(element);
                    FrameworkElement base = classifier ? classifier : element;

                    bool animate = (g_settings.excludeSystemButtonsMode == 0)
                        ? true
                        : ShouldAnimateElement(base);

                    FrameworkElement target = FindFirstTaskListButton(element);
                    if (!target) target = element;

                    if (!animate) {
                        void* k = winrt::get_abi(target);
                        if (prevElements.contains(k)) ResetElementTransforms(target);
                    } else {
                        SetupAndAddElement(target);
                    }
                }

                int c = VisualTreeHelper::GetChildrenCount(element);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
                    if (ch) recurse(ch);
                }
            };

            recurse(taskbarFrame);
        }

        std::sort(newIcons.begin(), newIcons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });

        ctx.icons = std::move(newIcons);

    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during icon search: %s", e.message().c_str());
    }
}

void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;
        g_contexts[pThis] = std::move(ctx);
        Wh_Log(L"DockAnimation: Monitor %p registered (hook-based).", pThis);
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to initialize context for %p: %s",
               pThis, e.message().c_str());
    }
}

// Main animation loop (pulse)
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&) {
    try {
        auto now = std::chrono::steady_clock::now();

        double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                               now - g_lastRenderTime)
                               .count();
        g_lastRenderTime = now;

        double dtSec = std::max(0.0, deltaTime) / 1000.0;

        double intensityChange = deltaTime / g_settings.focusDuration;
        double currentIntensity = g_animationIntensity.load();

        if (g_isMouseInside) {
            // FocusIn
            currentIntensity = std::min(1.0, currentIntensity + intensityChange);
        } else {
            // FocusOut
            currentIntensity = std::max(0.0, currentIntensity - intensityChange);
        }

        g_animationIntensity = currentIntensity;

        void* pThis_key = g_activeContextKey.load();
        if (pThis_key == nullptr) {
            if (currentIntensity <= 0.0) {
                Media::CompositionTarget::Rendering(g_renderingToken);
                g_isRenderingHooked = false;
                Wh_Log(L"DockAnimation: Stopped render loop (Key=null, Intensity=0).");
            }
            return;
        }

        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;

        auto& ctx = it->second;

        if (!g_isMouseInside && currentIntensity <= 0.0) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            g_isRenderingHooked = false;

            ResetAllIconScales(ctx.icons);
            ctx.isInitialized = false;
            ctx.icons.clear();

            g_activeContextKey = nullptr;
            g_lastMouseX = -1.0;
            g_isBouncing = false;

            Wh_Log(L"DockAnimation: Stopped render loop (FocusOut complete).");
            return;
        }

        if (!ctx.isInitialized) {
            RefreshIconPositions(ctx);
            ctx.isInitialized = true;
        }

        // Dirty-check
        if (g_isMouseInside) {
            auto sinceCheck = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - ctx.lastDirtyCheck).count();

            if (sinceCheck > 120) {
                ctx.lastDirtyCheck = now;

                auto frame = ctx.taskbarFrame.get();
                if (frame) {
                    auto host = ctx.iconHost.get();
                    if (!host) {
                        host = FindIconHost(frame);
                        if (host) ctx.iconHost = host;
                    }

                    if (host) {
                        HostSignature sig = ComputeHostSignature(host);

                        if (SigDifferent(sig, ctx.lastSig)) {
                            RefreshIconPositions(ctx);
                        } else {
                            RebaseIconGeometryFast(ctx);
                        }

                        ctx.lastSig = sig;

                        double mx = g_lastMouseX.load();
                        if (mx == -1.0) mx = 0.0;
                        ApplyAnimation(mx, ctx, currentIntensity, dtSec);
                    }
                }
            }
        }

        if (ctx.icons.empty()) {
            if (ctx.isInitialized) {
                RefreshIconPositions(ctx);
            }
            if (ctx.icons.empty()) return;
        }

        if (g_settings.disableBounce) {
            g_isBouncing = false;
        } else {
            auto elapsedSinceMove =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - g_lastSignificantMoveTime)
                    .count();

            if (g_isMouseInside && elapsedSinceMove > g_settings.bounceDelay) {
                if (!g_isBouncing.exchange(true)) {
                    g_bounceStartTime = now;
                }
            } else {
                g_isBouncing = false;
            }
        }

        double mouseX = g_lastMouseX.load();
        if (mouseX == -1.0) mouseX = 0.0;

        ApplyAnimation(mouseX, ctx, currentIntensity, dtSec);
    } catch (...) {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
        }
    }
}

// Hooking
// Pointer event hooks (new approach: activates mod immediately)
using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(), winrt::put_abi(args));
    if (!args)
        return original();
    // Initialize animation hooks only once
    if (g_contexts.find(pThis) == g_contexts.end()) {
        InitializeAnimationHooks(pThis, element);
        Wh_Log(L"DockAnimation: Initialized via OnPointerMoved (%s)", className.c_str());
    }
    // Forward event to existing logic
    OnTaskbarPointerMoved(pThis, args);
    return original();
}

using TaskbarFrame_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;

int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    OnTaskbarPointerExited(pThis);
    return original();
}

void LoadSettings() {
    int rawScale = Wh_GetIntSetting(L"MaxScale");

    if (rawScale < 101) {
        rawScale = 101;
    } else if (rawScale > 220) {
        rawScale = 220;
    }

    g_settings.animationType = Wh_GetIntSetting(L"AnimationType");
    g_settings.maxScale = (double)rawScale / 100.0;

    g_settings.effectRadius = Wh_GetIntSetting(L"EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;

    g_settings.spacingFactor = (double)Wh_GetIntSetting(L"SpacingFactor") / 100.0;
    if (g_settings.spacingFactor < 0.0) g_settings.spacingFactor = 0.5;

    g_settings.bounceDelay = Wh_GetIntSetting(L"BounceDelay");
    if (g_settings.bounceDelay < 0) g_settings.bounceDelay = 100;

    g_settings.focusDuration = (double)Wh_GetIntSetting(L"FocusDuration");
    if (g_settings.focusDuration <= 0) g_settings.focusDuration = 150.0;

    g_settings.mirrorAnimation = (bool)Wh_GetIntSetting(L"MirrorForTopTaskbar");
    g_settings.disableVerticalBounce = (bool)Wh_GetIntSetting(L"DisableVerticalBounce");
    g_settings.taskbarLabelsMode = (bool)Wh_GetIntSetting(L"TaskbarLabelsMode");
    g_settings.excludeSystemButtonsMode = Wh_GetIntSetting(L"ExcludeSystemButtonsMode");

    g_settings.lerpSpeed = (double)Wh_GetIntSetting(L"LerpSpeed");
    if (g_settings.lerpSpeed < 0) g_settings.lerpSpeed = 0;
    if (g_settings.lerpSpeed > 60) g_settings.lerpSpeed = 60;

    g_settings.disableBounce = (bool)Wh_GetIntSetting(L"DisableBounce");

    Wh_Log(L"DockAnimation: Settings loaded.");
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"
            },
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
        },
    };
    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"DockAnimation: HookSymbols failed.");
        return false;
    }
    Wh_Log(L"DockAnimation: HookSymbols succeeded (Pointer events only).");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// Lazy hook path (if Taskbar.View.dll loads later)
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"DockAnimation: Taskbar.View.dll loaded, hooking symbols...");
            if (HookTaskbarViewDllSymbols(module)) {
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                    Wh_Log(L"DockAnimation: Hooks applied (slow path).");
                }
            }
        }
    }
    return module;
}

// Windhawk entry points
BOOL Wh_ModInit() {
    Wh_Log(L"DockAnimation: Wh_ModInit");
    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(
            pKernelBaseLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"DockAnimation: Wh_ModAfterInit");

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
        Wh_Log(L"DockAnimation: Hooks already applied by Windhawk (fast path).");
    }
}

typedef void (*RunFromWindowThreadProc_t)(PVOID);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"DockAnimation: Wh_ModBeforeUninit (safe cleanup)");

    g_activeContextKey = nullptr;
    g_isBouncing = false;
    g_isMouseInside = false;
    g_animationIntensity = 0.0;

    try {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            Wh_Log(L"DockAnimation: Unhooked CompositionTarget::Rendering.");
        }
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error unhooking rendering: %s",
               e.message().c_str());
    }

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        RunFromWindowThread(
            hTaskbar,
            [](PVOID) {
                try {
                    for (auto& pair : g_contexts) {
                        auto& ctx = pair.second;
                        ResetAllIconScales(ctx.icons);
                    }
                    g_contexts.clear();
                    Wh_Log(L"DockAnimation: UI contexts cleaned up.");
                }
                catch (...) {}
            },
            nullptr);
    }
    else {
        g_contexts.clear();
    }
    g_taskbarViewDllLoaded = false;
    g_hooksApplied = false;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"DockAnimation: Settings changed.");
    LoadSettings();
    try {
        for (auto& pair : g_contexts) {
            auto& ctx = pair.second;
            ResetAllIconScales(ctx.icons);
            ctx.isInitialized = false;
            ctx.icons.clear();
        }
        g_isBouncing = false;
    } catch (...) {
    }
}

