// ==WindhawkMod==
// @id              osd-custom-pill
// @name            OSD Custom Pill
// @description     Replaces the Windows volume, brightness, and virtual desktop OSD with an animated spring pill
// @description:fr-FR Remplace les indicateurs Windows de volume, de luminosité et de bureau virtuel par une pilule animée à ressort
// @description:de-DE Ersetzt die Windows-Anzeigen für Lautstärke, Helligkeit und virtuelle Desktops durch eine animierte, federnde Pillenanzeige
// @description:es-ES Sustituye los indicadores de volumen, brillo y escritorio virtual de Windows por una píldora animada con efecto de resorte
// @description:it-IT Sostituisce gli indicatori di volume, luminosità e desktop virtuali di Windows con una pillola animata ed elastica
// @description:pt-BR Substitui os indicadores de volume, brilho e área de trabalho virtual do Windows por uma pílula animada com efeito de mola
// @description:pl-PL Zastępuje wskaźniki głośności, jasności i pulpitów wirtualnych systemu Windows animowanym, sprężystym paskiem
// @description:ru-RU Заменяет индикаторы громкости, яркости и виртуальных рабочих столов Windows анимированной упругой панелью
// @description:ja-JP Windows の音量、明るさ、仮想デスクトップの表示を、ばねのように動くアニメーション付きピル型表示に置き換えます
// @description:ko-KR Windows 볼륨, 밝기, 가상 데스크톱 표시를 스프링 애니메이션이 적용된 막대로 바꿉니다
// @description:zh-CN 将 Windows 音量、亮度和虚拟桌面提示替换为带有弹性动画的胶囊形提示条
// @version         4.9.11
// @author          yelloxygene
// @github          https://github.com/yelloxygene
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lgdi32 -luser32 -ldxva2 -luuid -ldwmapi -ladvapi32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# OSD Custom Pill

An animated on-screen indicator for volume, brightness and virtual desktops on Windows 11.

![OSD Custom Pill preview](https://raw.githubusercontent.com/yelloxygene/osd-custom-pill-assets/main/osd%20custom%20pill%20GIF.gif)

## Volume and brightness

Shows an icon, a progress bar and the current value. The indicator expands when it appears and retracts when it closes.

## Elastic deformation

Increasing a value moves and stretches the pill to the right; decreasing it moves the pill to the left. Repeated changes build up the deformation. When you release the key, the pill returns with a spring effect.

The **movement**, **stretching**, **spring damping** and **return strength** can be adjusted independently in the mod settings.

## Compact mode

After several changes in a row, the indicator can become smaller to take up less space. You can choose how many changes activate this mode and when it returns to normal.

## Size and position

Adjust the pill's width, height and distance from the bottom of the screen. Opening and closing delays can also be changed.

## Virtual desktops

Displays the current virtual desktop name or number when switching desktops. This indicator has its own size, position, animation and duration settings.

## Settings

All options are available in Windhawk's **Settings** tab, with translated labels in supported languages. The main groups are:

**Appearance and position** — dimensions and screen placement.

**Opening and closing** — delays and spring response.

**Stretching and rebound** — movement distance, deformation and elastic return.

**Compact mode** — activation threshold and return timing.

**Virtual desktop indicator** — label, dimensions, timing and animation.

Timing values are in milliseconds. Size values are based on 100% display scaling.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- startDelayMs: 90
  $name: "Delay before the pill appears (ms)"
  $name:fr-FR: "Délai avant affichage de la pilule (ms)"
  $name:de-DE: "Verzögerung bis zur Anzeige der Pillenanzeige (ms)"
  $name:es-ES: "Retraso antes de mostrar la píldora (ms)"
  $name:it-IT: "Ritardo prima della comparsa della pillola (ms)"
  $name:pt-BR: "Atraso antes de exibir a pílula (ms)"
  $name:pl-PL: "Opóźnienie przed pojawieniem się wskaźnika (ms)"
  $name:ru-RU: "Задержка перед появлением индикатора (мс)"
  $name:ja-JP: "ピル表示までの遅延 (ms)"
  $name:ko-KR: "표시 막대가 나타나기 전 지연 시간(ms)"
  $name:zh-CN: "显示胶囊前的延迟（毫秒）"
- decideMs: 300
  $name: "Time to distinguish volume/brightness from virtual desktop changes (ms)"
  $name:fr-FR: "Attente pour savoir si c'est volume/luminosité ou un changement de bureau (ms)"
  $name:de-DE: "Wartezeit zur Unterscheidung von Lautstärke-/Helligkeits- und Desktopwechseln (ms)"
  $name:es-ES: "Tiempo para distinguir cambios de volumen/brillo de cambios de escritorio virtual (ms)"
  $name:it-IT: "Tempo per distinguere le modifiche di volume/luminosità dai cambi di desktop virtuale (ms)"
  $name:pt-BR: "Tempo para distinguir alterações de volume/brilho de trocas de área de trabalho virtual (ms)"
  $name:pl-PL: "Czas rozróżniania zmiany głośności/jasności od zmiany pulpitu wirtualnego (ms)"
  $name:ru-RU: "Время для различения изменения громкости/яркости и смены виртуального рабочего стола (мс)"
  $name:ja-JP: "音量・明るさの変更と仮想デスクトップ切り替えを判別する時間 (ms)"
  $name:ko-KR: "볼륨/밝기 변경과 가상 데스크톱 전환을 구분하는 시간(ms)"
  $name:zh-CN: "区分音量/亮度变化与虚拟桌面切换的时间（毫秒）"
- holdMs: 120
  $name: "Delay before expanding while the circle rises (ms)"
  $name:fr-FR: "Délai avant le dépliage, pendant que le rond monte (ms)"
  $name:de-DE: "Verzögerung vor dem Aufklappen, während der Kreis nach oben gleitet (ms)"
  $name:es-ES: "Retraso antes de expandirse mientras el círculo asciende (ms)"
  $name:it-IT: "Ritardo prima dell'espansione mentre il cerchio sale (ms)"
  $name:pt-BR: "Atraso antes de expandir enquanto o círculo sobe (ms)"
  $name:pl-PL: "Opóźnienie przed rozwinięciem podczas unoszenia się kółka (ms)"
  $name:ru-RU: "Задержка перед разворачиванием при подъёме круга (мс)"
  $name:ja-JP: "円が上昇してからピルが展開するまでの遅延 (ms)"
  $name:ko-KR: "원이 위로 올라가는 동안 막대가 펼쳐지기 전 지연 시간(ms)"
  $name:zh-CN: "圆形上升时，胶囊展开前的延迟（毫秒）"
- closeHoldMs: 140
  $name: "Delay before sliding down while collapsing (ms)"
  $name:fr-FR: "Délai avant la sortie par le bas, pendant que ça se replie (ms)"
  $name:de-DE: "Verzögerung vor dem Abgleiten nach unten beim Einklappen (ms)"
  $name:es-ES: "Retraso antes de deslizarse hacia abajo al contraerse (ms)"
  $name:it-IT: "Ritardo prima dello scorrimento verso il basso durante la chiusura (ms)"
  $name:pt-BR: "Atraso antes de deslizar para baixo durante a retração (ms)"
  $name:pl-PL: "Opóźnienie przed zsunięciem w dół podczas zwijania (ms)"
  $name:ru-RU: "Задержка перед сдвигом вниз при сворачивании (мс)"
  $name:ja-JP: "縮小中に下へスライドし始めるまでの遅延 (ms)"
  $name:ko-KR: "접히면서 아래로 내려가기 전 지연 시간(ms)"
  $name:zh-CN: "收起时向下滑动前的延迟（毫秒）"
- responseMs: 450
  $name: "Spring response time (ms; lower = snappier)"
  $name:fr-FR: "Vitesse du ressort (ms, plus petit = plus nerveux)"
  $name:de-DE: "Reaktionszeit der Feder (ms; kleiner = schneller)"
  $name:es-ES: "Tiempo de respuesta del resorte (ms; menor = más rápido)"
  $name:it-IT: "Tempo di risposta della molla (ms; minore = più rapido)"
  $name:pt-BR: "Tempo de resposta da mola (ms; menor = mais rápido)"
  $name:pl-PL: "Czas reakcji sprężyny (ms; mniej = szybciej)"
  $name:ru-RU: "Время отклика пружины (мс; меньше = быстрее)"
  $name:ja-JP: "ばねの応答時間 (ms、小さいほど素早い)"
  $name:ko-KR: "스프링 반응 시간(ms, 작을수록 빠름)"
  $name:zh-CN: "弹簧响应时间（毫秒，越小越灵敏）"
- dampingPct: 62
  $name: "Spring damping (%; lower = more bouncing, 100 = none)"
  $name:fr-FR: "Amortissement (%, plus petit = plus de rebonds, 100 = aucun)"
  $name:de-DE: "Federdämpfung (%; kleiner = mehr Nachschwingen, 100 = keines)"
  $name:es-ES: "Amortiguación del resorte (%; menor = más rebote, 100 = ninguno)"
  $name:it-IT: "Smorzamento della molla (%; minore = più rimbalzi, 100 = nessuno)"
  $name:pt-BR: "Amortecimento da mola (%; menor = mais oscilação, 100 = nenhuma)"
  $name:pl-PL: "Tłumienie sprężyny (%; mniej = więcej odbić, 100 = brak)"
  $name:ru-RU: "Затухание пружины (%; меньше = больше колебаний, 100 = без колебаний)"
  $name:ja-JP: "ばねの減衰率 (%、小さいほど弾む、100 = 弾みなし)"
  $name:ko-KR: "스프링 감쇠율(%, 작을수록 더 튕김, 100 = 튕김 없음)"
  $name:zh-CN: "弹簧阻尼（%，越小回弹越明显，100 = 无回弹）"
- widthPx: 220
  $name: "Pill width (px at 100% scaling)"
  $name:fr-FR: "Largeur de la pilule (px à 100%)"
  $name:de-DE: "Breite der Pillenanzeige (px bei 100 % Skalierung)"
  $name:es-ES: "Anchura de la píldora (px con escala del 100 %)"
  $name:it-IT: "Larghezza della pillola (px con scala al 100%)"
  $name:pt-BR: "Largura da pílula (px com escala de 100%)"
  $name:pl-PL: "Szerokość wskaźnika (px przy skalowaniu 100%)"
  $name:ru-RU: "Ширина индикатора (пикс. при масштабе 100%)"
  $name:ja-JP: "ピルの幅 (表示倍率100%でのpx)"
  $name:ko-KR: "막대 너비(배율 100%에서 px)"
  $name:zh-CN: "胶囊宽度（缩放比例 100% 时的像素）"
- heightPx: 44
  $name: "Pill height (px at 100% scaling)"
  $name:fr-FR: "Hauteur de la pilule (px à 100%)"
  $name:de-DE: "Höhe der Pillenanzeige (px bei 100 % Skalierung)"
  $name:es-ES: "Altura de la píldora (px con escala del 100 %)"
  $name:it-IT: "Altezza della pillola (px con scala al 100%)"
  $name:pt-BR: "Altura da pílula (px com escala de 100%)"
  $name:pl-PL: "Wysokość wskaźnika (px przy skalowaniu 100%)"
  $name:ru-RU: "Высота индикатора (пикс. при масштабе 100%)"
  $name:ja-JP: "ピルの高さ (表示倍率100%でのpx)"
  $name:ko-KR: "막대 높이(배율 100%에서 px)"
  $name:zh-CN: "胶囊高度（缩放比例 100% 时的像素）"
- bottomOffsetPx: 70
  $name: "Distance from pill bottom to screen bottom (px at 100% scaling)"
  $name:fr-FR: "Distance entre le bas de la pilule et le bas de l'écran (px à 100%)"
  $name:de-DE: "Abstand vom unteren Rand der Anzeige zum unteren Bildschirmrand (px bei 100 %)"
  $name:es-ES: "Distancia desde la parte inferior de la píldora hasta el borde inferior de la pantalla (px al 100 %)"
  $name:it-IT: "Distanza dal bordo inferiore della pillola a quello dello schermo (px al 100%)"
  $name:pt-BR: "Distância da parte inferior da pílula até a borda inferior da tela (px a 100%)"
  $name:pl-PL: "Odległość od dolnej krawędzi wskaźnika do dołu ekranu (px przy 100%)"
  $name:ru-RU: "Расстояние от нижнего края индикатора до низа экрана (пикс. при 100%)"
  $name:ja-JP: "ピルの下端から画面下端までの距離 (倍率100%でのpx)"
  $name:ko-KR: "막대 아래쪽에서 화면 아래쪽까지 거리(배율 100%에서 px)"
  $name:zh-CN: "胶囊底部与屏幕底部的距离（缩放比例 100% 时的像素）"
- shakePx: 18
  $name: "Directional motion: horizontal displacement (px at 100%, 0 = disabled)"
  $name:fr-FR: "Impulsion directionnelle : déplacement horizontal (px à 100%, 0 = désactivé)"
  $name:de-DE: "Richtungsbewegung: horizontale Verschiebung (px bei 100 %, 0 = aus)"
  $name:es-ES: "Movimiento direccional: desplazamiento horizontal (px al 100 %, 0 = desactivado)"
  $name:it-IT: "Movimento direzionale: spostamento orizzontale (px al 100%, 0 = disattivato)"
  $name:pt-BR: "Movimento direcional: deslocamento horizontal (px a 100%, 0 = desativado)"
  $name:pl-PL: "Ruch kierunkowy: przesunięcie poziome (px przy 100%, 0 = wyłączone)"
  $name:ru-RU: "Направленное движение: смещение по горизонтали (пикс. при 100%, 0 = выкл.)"
  $name:ja-JP: "方向移動: 水平方向の移動量 (倍率100%でのpx、0 = 無効)"
  $name:ko-KR: "방향 이동: 수평 이동 거리(배율 100%에서 px, 0 = 끔)"
  $name:zh-CN: "方向运动：水平位移（缩放比例 100% 时的像素，0 = 禁用）"
- shakeMs: 750
  $name: "Directional motion: spring duration (ms; higher = slower)"
  $name:fr-FR: "Impulsion directionnelle : durée du ressort (ms, plus grand = plus lent)"
  $name:de-DE: "Richtungsbewegung: Federdauer (ms; größer = langsamer)"
  $name:es-ES: "Movimiento direccional: duración del resorte (ms; mayor = más lento)"
  $name:it-IT: "Movimento direzionale: durata della molla (ms; maggiore = più lento)"
  $name:pt-BR: "Movimento direcional: duração da mola (ms; maior = mais lento)"
  $name:pl-PL: "Ruch kierunkowy: czas działania sprężyny (ms; więcej = wolniej)"
  $name:ru-RU: "Направленное движение: длительность пружины (мс; больше = медленнее)"
  $name:ja-JP: "方向移動: ばねの動作時間 (ms、大きいほど遅い)"
  $name:ko-KR: "방향 이동: 스프링 지속 시간(ms, 클수록 느림)"
  $name:zh-CN: "方向运动：弹簧持续时间（毫秒，越大越慢）"
- shakeDampingPct: 32
  $name: "Directional motion: damping (%; lower = more bouncing)"
  $name:fr-FR: "Impulsion directionnelle : amortissement (%, plus petit = plus de rebonds)"
  $name:de-DE: "Richtungsbewegung: Dämpfung (%; kleiner = mehr Nachschwingen)"
  $name:es-ES: "Movimiento direccional: amortiguación (%; menor = más rebote)"
  $name:it-IT: "Movimento direzionale: smorzamento (%; minore = più rimbalzi)"
  $name:pt-BR: "Movimento direcional: amortecimento (%; menor = mais oscilação)"
  $name:pl-PL: "Ruch kierunkowy: tłumienie (%; mniej = więcej odbić)"
  $name:ru-RU: "Направленное движение: затухание (%; меньше = больше колебаний)"
  $name:ja-JP: "方向移動: 減衰率 (%、小さいほど弾む)"
  $name:ko-KR: "방향 이동: 감쇠율(%, 작을수록 더 튕김)"
  $name:zh-CN: "方向运动：阻尼（%，越小回弹越明显）"
- valueShakePct: 70
  $name: "Motion per value change (% intensity, 0 = disabled)"
  $name:fr-FR: "Impulsion à chaque changement de valeur (% de l'amplitude, 0 = désactivée)"
  $name:de-DE: "Bewegung je Wertänderung (Intensität in %, 0 = aus)"
  $name:es-ES: "Movimiento por cambio de valor (intensidad en %, 0 = desactivado)"
  $name:it-IT: "Movimento per variazione del valore (intensità %, 0 = disattivato)"
  $name:pt-BR: "Movimento por mudança de valor (intensidade em %, 0 = desativado)"
  $name:pl-PL: "Ruch przy każdej zmianie wartości (intensywność %, 0 = wyłączone)"
  $name:ru-RU: "Движение при изменении значения (интенсивность %, 0 = выкл.)"
  $name:ja-JP: "値の変更ごとの動き (強さ%、0 = 無効)"
  $name:ko-KR: "값 변경당 움직임(강도 %, 0 = 끔)"
  $name:zh-CN: "每次更改数值时的运动强度（%，0 = 禁用）"
- shakeWidthPct: 32
  $name: "Directional motion: maximum horizontal stretch (%)"
  $name:fr-FR: "Impulsion directionnelle : élargissement horizontal maximal (%)"
  $name:de-DE: "Richtungsbewegung: maximale horizontale Dehnung (%)"
  $name:es-ES: "Movimiento direccional: estiramiento horizontal máximo (%)"
  $name:it-IT: "Movimento direzionale: allungamento orizzontale massimo (%)"
  $name:pt-BR: "Movimento direcional: alongamento horizontal máximo (%)"
  $name:pl-PL: "Ruch kierunkowy: maksymalne rozciągnięcie w poziomie (%)"
  $name:ru-RU: "Направленное движение: максимальное растяжение по горизонтали (%)"
  $name:ja-JP: "方向移動: 最大の横方向の伸び (%)"
  $name:ko-KR: "방향 이동: 최대 수평 늘어남(%)"
  $name:zh-CN: "方向运动：最大水平拉伸（%）"
- returnPowerPct: 155
  $name: "Elastic return: rebound strength (%, 100 = standard, 155 = stronger, 0 = soft)"
  $name:fr-FR: "Retour élastique : puissance du rebond (%, 100 = normal, 155 = plus marqué, 0 = retour doux)"
  $name:de-DE: "Elastische Rückkehr: Rückfederstärke (%, 100 = normal, 155 = stärker, 0 = sanft)"
  $name:es-ES: "Retorno elástico: fuerza del rebote (%, 100 = normal, 155 = más fuerte, 0 = suave)"
  $name:it-IT: "Ritorno elastico: intensità del rimbalzo (%, 100 = normale, 155 = più forte, 0 = delicato)"
  $name:pt-BR: "Retorno elástico: força do rebote (%, 100 = padrão, 155 = mais forte, 0 = suave)"
  $name:pl-PL: "Sprężysty powrót: siła odbicia (%, 100 = standardowo, 155 = mocniej, 0 = łagodnie)"
  $name:ru-RU: "Упругий возврат: сила отскока (%, 100 = стандартный, 155 = сильнее, 0 = плавно)"
  $name:ja-JP: "弾性復帰: 跳ね返りの強さ (%、100 = 標準、155 = 強め、0 = 穏やか)"
  $name:ko-KR: "탄성 복귀: 반동 강도(%, 100 = 기본, 155 = 더 강함, 0 = 부드럽게)"
  $name:zh-CN: "弹性复位：回弹力度（%，100 = 标准，155 = 更强，0 = 柔和）"
- compactEnabled: true
  $name: "Compact mode: enable compact mode"
  $name:fr-FR: "Mode compact : activer le mode compact"
  $name:de-DE: "Kompaktmodus: aktivieren"
  $name:es-ES: "Modo compacto: activar"
  $name:it-IT: "Modalità compatta: attiva"
  $name:pt-BR: "Modo compacto: ativar"
  $name:pl-PL: "Tryb kompaktowy: włącz"
  $name:ru-RU: "Компактный режим: включить"
  $name:ja-JP: "コンパクトモード: 有効化"
  $name:ko-KR: "컴팩트 모드: 사용"
  $name:zh-CN: "紧凑模式：启用"
- compactTriggerCount: 2
  $name: "Compact mode: number of presses before activation"
  $name:fr-FR: "Mode compact : nombre de pressions avant activation"
  $name:de-DE: "Kompaktmodus: Anzahl der Tastendrücke bis zur Aktivierung"
  $name:es-ES: "Modo compacto: número de pulsaciones para activarse"
  $name:it-IT: "Modalità compatta: numero di pressioni per l'attivazione"
  $name:pt-BR: "Modo compacto: quantidade de pressionamentos para ativar"
  $name:pl-PL: "Tryb kompaktowy: liczba naciśnięć potrzebnych do aktywacji"
  $name:ru-RU: "Компактный режим: число нажатий для активации"
  $name:ja-JP: "コンパクトモード: 有効化までのキー操作回数"
  $name:ko-KR: "컴팩트 모드: 활성화까지 필요한 키 누름 횟수"
  $name:zh-CN: "紧凑模式：激活所需按键次数"
- compactReturnToNormal: true
  $name: "Compact mode: return to normal after a pause (off = compact until hidden)"
  $name:fr-FR: "Mode compact : revenir au mode normal après une pause (décoché = reste compact jusqu'à sa disparition)"
  $name:de-DE: "Kompaktmodus: nach einer Pause zur Normalgröße zurückkehren (aus = kompakt bis zum Ausblenden)"
  $name:es-ES: "Modo compacto: volver al tamaño normal tras una pausa (desactivado = compacto hasta ocultarse)"
  $name:it-IT: "Modalità compatta: torna normale dopo una pausa (disattivato = compatta fino alla scomparsa)"
  $name:pt-BR: "Modo compacto: voltar ao tamanho normal após uma pausa (desativado = compacto até sumir)"
  $name:pl-PL: "Tryb kompaktowy: powrót do normalnego rozmiaru po przerwie (wyłączone = kompaktowy do ukrycia)"
  $name:ru-RU: "Компактный режим: возвращаться к обычному размеру после паузы (выкл. = до скрытия)"
  $name:ja-JP: "コンパクトモード: 一時停止後に通常サイズへ戻す (オフ = 非表示までコンパクト)"
  $name:ko-KR: "컴팩트 모드: 잠시 멈추면 원래 크기로 복귀(끔 = 숨겨질 때까지 컴팩트 유지)"
  $name:zh-CN: "紧凑模式：暂停后恢复正常大小（关闭 = 一直保持紧凑直到隐藏）"
- compactReleaseMs: 450
  $name: "Compact mode: max gap between presses and return delay (ms; 0 = no limit/automatic return)"
  $name:fr-FR: "Mode compact : fenêtre entre les pressions et délai de retour (ms, 0 = aucune limite / aucun retour automatique)"
  $name:de-DE: "Kompaktmodus: maximales Intervall zwischen Tastendrücken und Rückkehrverzögerung (ms; 0 = unbegrenzt/keine automatische Rückkehr)"
  $name:es-ES: "Modo compacto: intervalo máximo entre pulsaciones y retraso del retorno (ms; 0 = sin límite/sin retorno automático)"
  $name:it-IT: "Modalità compatta: intervallo massimo tra pressioni e ritardo di ritorno (ms; 0 = senza limite/nessun ritorno automatico)"
  $name:pt-BR: "Modo compacto: intervalo máximo entre pressionamentos e atraso de retorno (ms; 0 = sem limite/sem retorno automático)"
  $name:pl-PL: "Tryb kompaktowy: maksymalny odstęp między naciśnięciami i opóźnienie powrotu (ms; 0 = bez limitu/bez automatycznego powrotu)"
  $name:ru-RU: "Компактный режим: максимальный интервал между нажатиями и задержка возврата (мс; 0 = без ограничений/без автовозврата)"
  $name:ja-JP: "コンパクトモード: 連続操作の最大間隔と復帰までの遅延 (ms、0 = 制限なし/自動復帰なし)"
  $name:ko-KR: "컴팩트 모드: 키 입력 사이 최대 간격 및 복귀 지연(ms, 0 = 제한 없음/자동 복귀 없음)"
  $name:zh-CN: "紧凑模式：按键间最大间隔和复位延迟（毫秒，0 = 无限制/不自动复位）"
- deskLabel: Desktop
  $name: "Label before virtual desktop number when no name is set"
  $name:fr-FR: "Mot affiché devant le numéro quand le bureau n'a pas de nom"
  $name:de-DE: "Text vor der Nummer eines unbenannten virtuellen Desktops"
  $name:es-ES: "Texto delante del número de un escritorio virtual sin nombre"
  $name:it-IT: "Testo prima del numero di un desktop virtuale senza nome"
  $name:pt-BR: "Texto antes do número de uma área de trabalho virtual sem nome"
  $name:pl-PL: "Tekst przed numerem pulpitu wirtualnego bez nazwy"
  $name:ru-RU: "Текст перед номером виртуального рабочего стола без имени"
  $name:ja-JP: "名前のない仮想デスクトップ番号の前に表示する文字"
  $name:ko-KR: "이름 없는 가상 데스크톱 번호 앞에 표시할 텍스트"
  $name:zh-CN: "未命名虚拟桌面编号前显示的文字"
- deskWidthPx: 200
  $name: "Virtual desktop: pill width (px at 100% scaling)"
  $name:fr-FR: "Bureau : largeur de la pilule (px à 100%)"
  $name:de-DE: "Virtueller Desktop: Breite der Pillenanzeige (px bei 100 %)"
  $name:es-ES: "Escritorio virtual: anchura de la píldora (px al 100 %)"
  $name:it-IT: "Desktop virtuale: larghezza della pillola (px al 100%)"
  $name:pt-BR: "Área de trabalho virtual: largura da pílula (px a 100%)"
  $name:pl-PL: "Pulpit wirtualny: szerokość wskaźnika (px przy 100%)"
  $name:ru-RU: "Виртуальный рабочий стол: ширина индикатора (пикс. при 100%)"
  $name:ja-JP: "仮想デスクトップ: ピルの幅 (倍率100%でのpx)"
  $name:ko-KR: "가상 데스크톱: 막대 너비(배율 100%에서 px)"
  $name:zh-CN: "虚拟桌面：胶囊宽度（缩放比例 100% 时的像素）"
- deskHeightPx: 44
  $name: "Virtual desktop: pill height (px at 100% scaling)"
  $name:fr-FR: "Bureau : hauteur de la pilule (px à 100%)"
  $name:de-DE: "Virtueller Desktop: Höhe der Pillenanzeige (px bei 100 %)"
  $name:es-ES: "Escritorio virtual: altura de la píldora (px al 100 %)"
  $name:it-IT: "Desktop virtuale: altezza della pillola (px al 100%)"
  $name:pt-BR: "Área de trabalho virtual: altura da pílula (px a 100%)"
  $name:pl-PL: "Pulpit wirtualny: wysokość wskaźnika (px przy 100%)"
  $name:ru-RU: "Виртуальный рабочий стол: высота индикатора (пикс. при 100%)"
  $name:ja-JP: "仮想デスクトップ: ピルの高さ (倍率100%でのpx)"
  $name:ko-KR: "가상 데스크톱: 막대 높이(배율 100%에서 px)"
  $name:zh-CN: "虚拟桌面：胶囊高度（缩放比例 100% 时的像素）"
- deskBottomOffsetPx: 70
  $name: "Virtual desktop: distance from screen bottom (px at 100% scaling)"
  $name:fr-FR: "Bureau : distance au bas de l'écran (px à 100%)"
  $name:de-DE: "Virtueller Desktop: Abstand zum unteren Bildschirmrand (px bei 100 %)"
  $name:es-ES: "Escritorio virtual: distancia al borde inferior de la pantalla (px al 100 %)"
  $name:it-IT: "Desktop virtuale: distanza dal bordo inferiore dello schermo (px al 100%)"
  $name:pt-BR: "Área de trabalho virtual: distância da borda inferior da tela (px a 100%)"
  $name:pl-PL: "Pulpit wirtualny: odległość od dołu ekranu (px przy 100%)"
  $name:ru-RU: "Виртуальный рабочий стол: расстояние до низа экрана (пикс. при 100%)"
  $name:ja-JP: "仮想デスクトップ: 画面下端からの距離 (倍率100%でのpx)"
  $name:ko-KR: "가상 데스크톱: 화면 아래쪽으로부터 거리(배율 100%에서 px)"
  $name:zh-CN: "虚拟桌面：距屏幕底部的距离（缩放比例 100% 时的像素）"
- deskHoldMs: 120
  $name: "Virtual desktop: delay before expansion (ms)"
  $name:fr-FR: "Bureau : délai avant le dépliage (ms)"
  $name:de-DE: "Virtueller Desktop: Verzögerung vor dem Aufklappen (ms)"
  $name:es-ES: "Escritorio virtual: retraso antes de expandirse (ms)"
  $name:it-IT: "Desktop virtuale: ritardo prima dell'espansione (ms)"
  $name:pt-BR: "Área de trabalho virtual: atraso antes de expandir (ms)"
  $name:pl-PL: "Pulpit wirtualny: opóźnienie przed rozwinięciem (ms)"
  $name:ru-RU: "Виртуальный рабочий стол: задержка перед разворачиванием (мс)"
  $name:ja-JP: "仮想デスクトップ: 展開前の遅延 (ms)"
  $name:ko-KR: "가상 데스크톱: 펼쳐지기 전 지연 시간(ms)"
  $name:zh-CN: "虚拟桌面：展开前的延迟（毫秒）"
- deskCloseHoldMs: 140
  $name: "Virtual desktop: delay before sliding down (ms)"
  $name:fr-FR: "Bureau : délai avant la sortie par le bas (ms)"
  $name:de-DE: "Virtueller Desktop: Verzögerung vor dem Abgleiten nach unten (ms)"
  $name:es-ES: "Escritorio virtual: retraso antes de deslizarse hacia abajo (ms)"
  $name:it-IT: "Desktop virtuale: ritardo prima dello scorrimento verso il basso (ms)"
  $name:pt-BR: "Área de trabalho virtual: atraso antes de deslizar para baixo (ms)"
  $name:pl-PL: "Pulpit wirtualny: opóźnienie przed zsunięciem w dół (ms)"
  $name:ru-RU: "Виртуальный рабочий стол: задержка перед сдвигом вниз (мс)"
  $name:ja-JP: "仮想デスクトップ: 下へスライドするまでの遅延 (ms)"
  $name:ko-KR: "가상 데스크톱: 아래로 내려가기 전 지연 시간(ms)"
  $name:zh-CN: "虚拟桌面：向下滑动前的延迟（毫秒）"
- deskResponseMs: 450
  $name: "Virtual desktop: spring response time (ms)"
  $name:fr-FR: "Bureau : vitesse du ressort (ms)"
  $name:de-DE: "Virtueller Desktop: Reaktionszeit der Feder (ms)"
  $name:es-ES: "Escritorio virtual: tiempo de respuesta del resorte (ms)"
  $name:it-IT: "Desktop virtuale: tempo di risposta della molla (ms)"
  $name:pt-BR: "Área de trabalho virtual: tempo de resposta da mola (ms)"
  $name:pl-PL: "Pulpit wirtualny: czas reakcji sprężyny (ms)"
  $name:ru-RU: "Виртуальный рабочий стол: время отклика пружины (мс)"
  $name:ja-JP: "仮想デスクトップ: ばねの応答時間 (ms)"
  $name:ko-KR: "가상 데스크톱: 스프링 반응 시간(ms)"
  $name:zh-CN: "虚拟桌面：弹簧响应时间（毫秒）"
- deskDampingPct: 62
  $name: "Virtual desktop: damping (%; lower = more bouncing)"
  $name:fr-FR: "Bureau : amortissement (%, plus petit = plus de rebonds)"
  $name:de-DE: "Virtueller Desktop: Dämpfung (%; kleiner = mehr Nachschwingen)"
  $name:es-ES: "Escritorio virtual: amortiguación (%; menor = más rebote)"
  $name:it-IT: "Desktop virtuale: smorzamento (%; minore = più rimbalzi)"
  $name:pt-BR: "Área de trabalho virtual: amortecimento (%; menor = mais oscilação)"
  $name:pl-PL: "Pulpit wirtualny: tłumienie (%; mniej = więcej odbić)"
  $name:ru-RU: "Виртуальный рабочий стол: затухание (%; меньше = больше колебаний)"
  $name:ja-JP: "仮想デスクトップ: 減衰率 (%、小さいほど弾む)"
  $name:ko-KR: "가상 데스크톱: 감쇠율(%, 작을수록 더 튕김)"
  $name:zh-CN: "虚拟桌面：阻尼（%，越小回弹越明显）"
- deskDurationMs: 0
  $name: "Virtual desktop: display duration (ms; 0 = follow Windows)"
  $name:fr-FR: "Bureau : durée d'affichage (ms, 0 = suit Windows)"
  $name:de-DE: "Virtueller Desktop: Anzeigedauer (ms; 0 = Windows folgen)"
  $name:es-ES: "Escritorio virtual: duración de visualización (ms; 0 = seguir Windows)"
  $name:it-IT: "Desktop virtuale: durata della visualizzazione (ms; 0 = segue Windows)"
  $name:pt-BR: "Área de trabalho virtual: duração da exibição (ms; 0 = seguir o Windows)"
  $name:pl-PL: "Pulpit wirtualny: czas wyświetlania (ms; 0 = zgodnie z Windows)"
  $name:ru-RU: "Виртуальный рабочий стол: время отображения (мс; 0 = как в Windows)"
  $name:ja-JP: "仮想デスクトップ: 表示時間 (ms、0 = Windowsに従う)"
  $name:ko-KR: "가상 데스크톱: 표시 시간(ms, 0 = Windows 설정 따름)"
  $name:zh-CN: "虚拟桌面：显示时长（毫秒，0 = 跟随 Windows）"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <atomic>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <cmath>
#include <cwchar>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <mutex>
#include <vector>

// ---------------------------------------------------------------- état global
struct {
    int startDelayMs, decideMs, holdMs, closeHoldMs, responseMs, dampingPct, widthPx, heightPx, bottomOffset,
        shakePx, shakeMs, shakeDampPct, valueShakePct, shakeWidthPct, returnPowerPct, compactReleaseMs,
        compactEnabled, compactTriggerCount, compactReturnToNormal;
} g;
struct {
    int widthPx, heightPx, bottomOffset, holdMs, closeHoldMs, responseMs, dampingPct, durationMs;
} dk;
static wchar_t g_deskLabel[32] = L"Desktop";

#define MSG_SHOW   (WM_APP + 1)
#define MSG_HIDE   (WM_APP + 2)
#define MSG_VALUES (WM_APP + 3)
#define MSG_RELOAD (WM_APP + 4)
#define MSG_DESKTOP (WM_APP + 5)

static HANDLE g_stopEvent = nullptr;
static HANDLE g_pollWakeEvent = nullptr;
static std::atomic<bool> g_rebindAudio{false};
static SRWLOCK g_labelLock = SRWLOCK_INIT;
static volatile LONG g_vol = -1, g_mute = 0, g_bri = -1;
static volatile DWORD g_volTick = 0, g_briTick = 0;
static volatile DWORD g_deskTick = 0;   // dernier changement de bureau détecté
static volatile DWORD g_wantDdcUntil = 0;
static int g_limitDir = 0;              // luminosité en butée : +1 max, -1 min
static int g_bumpDir = 0;               // sens de la secousse à jouer à l'ouverture
static HWND g_ui = nullptr;
static HWND g_osd = nullptr; // dernier OSD natif pour l'etat visuel
// Chaque OSD masque doit etre restaure, meme si un autre s'affiche ensuite.
static std::mutex g_hiddenOsdMutex;
static std::vector<HWND> g_hiddenOsdWindows;

static bool IsTrackedHiddenOsd(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_hiddenOsdMutex);
    return std::find(g_hiddenOsdWindows.begin(), g_hiddenOsdWindows.end(), hwnd) !=
           g_hiddenOsdWindows.end();
}

static void TrackHiddenOsd(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_hiddenOsdMutex);
    if (std::find(g_hiddenOsdWindows.begin(), g_hiddenOsdWindows.end(), hwnd) ==
        g_hiddenOsdWindows.end()) {
        g_hiddenOsdWindows.push_back(hwnd);
    }
}

static bool UntrackHiddenOsd(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_hiddenOsdMutex);
    auto it = std::find(g_hiddenOsdWindows.begin(), g_hiddenOsdWindows.end(), hwnd);
    if (it == g_hiddenOsdWindows.end()) return false;
    g_hiddenOsdWindows.erase(it);
    return true;
}

static void RestoreAllHiddenOsdWindows() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_hiddenOsdMutex);
        windows.swap(g_hiddenOsdWindows);
    }
    // Ne pas garder le mutex durant SetWindowRgn : Windows envoie des messages
    // synchrones sur le thread de la fenetre.
    for (HWND hwnd : windows) {
        if (IsWindow(hwnd)) SetWindowRgn(hwnd, nullptr, TRUE);
    }
}
static HANDLE g_uiThread = nullptr, g_pollThread = nullptr;
static float g_fontScale = 0.0f;

static inline float Clamp(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

static bool IsKnownNonOsdFlyout(HWND hwnd) {
    // Snap Layouts, Snap Assist and Task View can live in different Z-bands
    // depending on the Windows build and the display scale. Never gate this
    // exclusion on GetWindowBand.
    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (!tid) return false;
    HANDLE th = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!th) return false;
    PWSTR description = nullptr;
    bool excluded = false;
    if (SUCCEEDED(GetThreadDescription(th, &description)) && description) {
        excluded = _wcsicmp(description, L"MultitaskingView") == 0 ||
                   wcsstr(description, L"Snap") != nullptr ||
                   wcsstr(description, L"snap") != nullptr;
        LocalFree(description);
    }
    CloseHandle(th);
    return excluded;
}

// Learned native-OSD signature. A desktop-registry notification alone must
// never teach us the signature: Snap and Task View use the same XAML class.
// A non-empty thread description is preferred; the Z-band is a fallback.
static std::mutex g_nativeSignatureMutex;
static wchar_t g_nativeThreadDescription[128] = L"";
static DWORD g_nativeWindowBand = 0;
static bool g_nativeSignatureReady = false;
static volatile DWORD g_lastNativeCandidateTick = 0;

using GetWindowBandFunc = BOOL (WINAPI*)(HWND, PDWORD);
static DWORD NativeWindowBand(HWND hwnd) {
    static auto fn = reinterpret_cast<GetWindowBandFunc>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetWindowBand"));
    DWORD band = 0;
    if (fn) fn(hwnd, &band);
    return band;
}

static void NativeThreadDescription(HWND hwnd, wchar_t (&out)[128]) {
    out[0] = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (!tid) return;
    HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!thread) return;
    PWSTR description = nullptr;
    if (SUCCEEDED(GetThreadDescription(thread, &description)) && description) {
        wcsncpy_s(out, description, _TRUNCATE);
        LocalFree(description);
    }
    CloseHandle(thread);
}

static bool MatchesLearnedNativeOsd(HWND hwnd) {
    wchar_t description[128];
    NativeThreadDescription(hwnd, description);
    DWORD band = NativeWindowBand(hwnd);
    std::lock_guard<std::mutex> lock(g_nativeSignatureMutex);
    if (!g_nativeSignatureReady) return true; // Bootstrap before first genuine event.
    if (g_nativeThreadDescription[0])
        return wcscmp(description, g_nativeThreadDescription) == 0;
    return !g_nativeWindowBand || band == g_nativeWindowBand;
}

static void LearnNativeOsd(HWND hwnd, const wchar_t* reason) {
    if (!hwnd || !IsWindow(hwnd) || !IsTrackedHiddenOsd(hwnd) ||
        IsKnownNonOsdFlyout(hwnd)) return;
    wchar_t description[128];
    NativeThreadDescription(hwnd, description);
    DWORD band = NativeWindowBand(hwnd);
    bool learned = false;
    {
        std::lock_guard<std::mutex> lock(g_nativeSignatureMutex);
        if (!g_nativeSignatureReady && (description[0] || band)) {
            wcsncpy_s(g_nativeThreadDescription, description, _TRUNCATE);
            g_nativeWindowBand = band;
            g_nativeSignatureReady = true;
            learned = true;
        }
    }
    if (learned)
        Wh_Log(L"Learned native OSD signature from %s: thread=\"%s\" band=%lu",
               reason, description, (unsigned long)band);
}

static bool IsOsd(HWND h) {
    wchar_t b[128];
    return GetClassNameW(h, b, 128) && wcscmp(b, L"XamlExplorerHostIslandWindow") == 0;
}

// ---------------------------------------------------------------- ressorts
struct Spring { float x = 0, v = 0, t = 0; };
static float g_k = 100, g_c = 10;
static float g_kv = 400, g_cv = 30;
static float g_kc = 200, g_cc = 20;     // ressort du mode compact

static void MakeSpringParams(float responseSec, float zeta, float* k, float* c) {
    float w = 6.2831853f / responseSec;
    *k = w * w;
    *c = 2.0f * zeta * w;
}

static void Step(Spring& s, float dt, float k, float c) {
    int n = (int)ceilf(dt / 0.004f);
    if (n < 1) n = 1;
    float h = dt / n;
    for (int i = 0; i < n; i++) {
        float a = k * (s.t - s.x) - c * s.v;
        s.v += a * h;
        s.x += s.v * h;
    }
}

static void ComputeSprings(int responseMs, int dampingPct) {
    float zeta = Clamp(dampingPct / 100.0f, 0.2f, 1.2f);
    MakeSpringParams(responseMs / 1000.0f, zeta, &g_k, &g_c);
    MakeSpringParams(0.22f, 0.75f, &g_kv, &g_cv);
    // Ressort du mode compact : amortissement plus faible pour un rebond visible.
    // La période un peu plus longue rend le mouvement souple, sans être trop lent.
    MakeSpringParams(0.36f, 0.48f, &g_kc, &g_cc);
}

// ---------------------------------------------------------------- nom du bureau virtuel
static bool RegBin(const wchar_t* path, const wchar_t* val, BYTE* buf, DWORD* sz) {
    return RegGetValueW(HKEY_CURRENT_USER, path, val, RRF_RT_REG_BINARY, nullptr, buf, sz) == ERROR_SUCCESS;
}

static void ReadDesktopName(wchar_t* out, int n) {
    const wchar_t* base = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops";
    wchar_t sessPath[300];
    DWORD sess = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sess);
    _snwprintf(sessPath, 300,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops", (unsigned long)sess);

    BYTE cur[16]; DWORD cs = 16;
    bool ok = RegBin(sessPath, L"CurrentVirtualDesktop", cur, &cs);
    if (!ok) { cs = 16; ok = RegBin(base, L"CurrentVirtualDesktop", cur, &cs); }

    BYTE ids[16 * 64]; DWORD is = sizeof(ids);
    bool okIds = RegBin(base, L"VirtualDesktopIDs", ids, &is);

    int index = 0;
    if (ok && okIds) {
        for (DWORD i = 0; i + 16 <= is; i += 16)
            if (memcmp(ids + i, cur, 16) == 0) { index = (int)(i / 16) + 1; break; }
    }

    if (ok) {
        GUID gd; memcpy(&gd, cur, 16);
        wchar_t gs[64];
        if (StringFromGUID2(gd, gs, 64)) {
            wchar_t p[400];
            _snwprintf(p, 400, L"%s\\Desktops\\%s", base, gs);
            wchar_t nm[64]; DWORD ns = sizeof(nm);
            if (RegGetValueW(HKEY_CURRENT_USER, p, L"Name", RRF_RT_REG_SZ, nullptr, nm, &ns) == ERROR_SUCCESS && nm[0]) {
                wcsncpy(out, nm, n - 1); out[n - 1] = 0;
                return;
            }
        }
    }
    wchar_t label[32];
    AcquireSRWLockShared(&g_labelLock);
    wcsncpy(label, g_deskLabel, 31); label[31] = 0;
    ReleaseSRWLockShared(&g_labelLock);
    if (index > 0) _snwprintf(out, n, L"%s %d", label, index);
    else _snwprintf(out, n, L"%s", label);
    out[n - 1] = 0;
}

// Read the current desktop ID instead of relying only on the displayed name.
// Two desktops may share the same name, and registry notifications may arrive
// after the native flyout has already been shown.
static bool ReadCurrentDesktopId(GUID* out) {
    if (!out) return false;
    wchar_t path[300];
    DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    _snwprintf(path, 300,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops",
        (unsigned long)session);
    BYTE bytes[16];
    DWORD bytesSize = sizeof(bytes);
    bool ok = RegBin(path, L"CurrentVirtualDesktop", bytes, &bytesSize);
    if (!ok) {
        bytesSize = sizeof(bytes);
        ok = RegBin(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops",
                    L"CurrentVirtualDesktop", bytes, &bytesSize);
    }
    if (!ok || bytesSize != sizeof(bytes)) return false;
    memcpy(out, bytes, sizeof(bytes));
    return true;
}

// ---------------------------------------------------------------- lecture des valeurs
struct DispBrightness { UCHAR policy, ac, dc; };
#define IOCTL_QUERY_BRIGHTNESS CTL_CODE(0x23, 0x126, METHOD_BUFFERED, FILE_ANY_ACCESS)

static int ReadLcd(HANDLE h) {
    if (h == INVALID_HANDLE_VALUE) return -1;
    DispBrightness d{}; DWORD ret = 0;
    if (!DeviceIoControl(h, IOCTL_QUERY_BRIGHTNESS, nullptr, 0, &d, sizeof(d), &ret, nullptr)) return -1;
    SYSTEM_POWER_STATUS ps{};
    bool ac = GetSystemPowerStatus(&ps) ? (ps.ACLineStatus == 1) : true;
    return ac ? d.ac : d.dc;
}

static int ReadDdc() {
    HMONITOR hm = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    DWORD n = 0;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hm, &n) || n == 0) return -1;
    PHYSICAL_MONITOR* pm = (PHYSICAL_MONITOR*)calloc(n, sizeof(PHYSICAL_MONITOR));
    if (!pm) return -1;
    int res = -1;
    if (GetPhysicalMonitorsFromHMONITOR(hm, n, pm)) {
        DWORD mn = 0, cur = 0, mx = 0;
        if (GetMonitorBrightness(pm[0].hPhysicalMonitor, &mn, &cur, &mx) && mx > mn)
            res = (int)((cur - mn) * 100 / (mx - mn));
        DestroyPhysicalMonitors(n, pm);
    }
    free(pm);
    return res;
}

static void PostValue(int kind, int v, bool mute) {
    if (g_ui) PostMessageW(g_ui, MSG_VALUES, (WPARAM)kind, (LPARAM)(v | (mute ? 0x100 : 0)));
}

// Windows provides event notifications for endpoint volume and default-device
// changes. There is no need to activate a new audio endpoint every 50 ms.
class VolumeNotification final : public IAudioEndpointVolumeCallback {
    LONG refs = 1;
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (iid != __uuidof(IUnknown) && iid != __uuidof(IAudioEndpointVolumeCallback))
            return E_NOINTERFACE;
        *out = static_cast<IAudioEndpointVolumeCallback*>(this); AddRef(); return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&refs); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = InterlockedDecrement(&refs); if (!n) delete this; return n;
    }
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA data) override {
        if (!data || !g_stopEvent || WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0)
            return S_OK;
        int newVol = (int)(data->fMasterVolume * 100.0f + 0.5f);
        int newMute = data->bMuted ? 1 : 0;
        bool changed = newVol != g_vol || newMute != g_mute;
        bool initial = g_vol < 0;
        g_vol = newVol; g_mute = newMute;
        if (changed && !initial) {
            g_volTick = GetTickCount();
            PostValue(0, newVol, newMute != 0);
        }
        return S_OK;
    }
};
class AudioDeviceNotification final : public IMMNotificationClient {
    LONG refs = 1;
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (iid != __uuidof(IUnknown) && iid != __uuidof(IMMNotificationClient))
            return E_NOINTERFACE;
        *out = static_cast<IMMNotificationClient*>(this); AddRef(); return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&refs); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = InterlockedDecrement(&refs); if (!n) delete this; return n;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eMultimedia) {
            g_rebindAudio.store(true);
            if (g_pollWakeEvent) SetEvent(g_pollWakeEvent);
        }
        return S_OK;
    }
};

static DWORD WINAPI PollThread(LPVOID) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool comOk = SUCCEEDED(hr);
    IMMDeviceEnumerator* en = nullptr;
    IAudioEndpointVolume* ep = nullptr;
    IMMDevice* dev = nullptr;
    VolumeNotification* volumeCb = new VolumeNotification();
    AudioDeviceNotification* deviceCb = new AudioDeviceNotification();
    if (comOk && FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&en))) en = nullptr;
    if (en) en->RegisterEndpointNotificationCallback(deviceCb);

    auto Unbind = [&]() {
        if (ep) { ep->UnregisterControlChangeNotify(volumeCb); ep->Release(); ep = nullptr; }
        if (dev) { dev->Release(); dev = nullptr; }
    };
    auto Bind = [&]() {
        Unbind();
        if (!en || FAILED(en->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev)) || !dev) return;
        if (FAILED(dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
             (void**)&ep)) || !ep) return;
        float lv = 0; BOOL mute = FALSE;
        if (SUCCEEDED(ep->GetMasterVolumeLevelScalar(&lv))) {
            g_vol = (LONG)(lv * 100.0f + 0.5f);
            if (SUCCEEDED(ep->GetMute(&mute))) g_mute = mute ? 1 : 0;
        }
        ep->RegisterControlChangeNotify(volumeCb);
    };
    Bind();

    HANDLE hLcd = CreateFileW(L"\\\\.\\LCD", GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    bool lcdOk = ReadLcd(hLcd) >= 0;
    int pb = lcdOk ? ReadLcd(hLcd) : ReadDdc();
    g_bri = pb;
    DWORD lastDdc = GetTickCount();

    // Registry change notifications replace continuous desktop-name polling.
    wchar_t sessPath[300]; DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    _snwprintf(sessPath, 300,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops",
        (unsigned long)session);
    HKEY desktopKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, sessPath, 0, KEY_NOTIFY, &desktopKey) != ERROR_SUCCESS)
        RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops",
            0, KEY_NOTIFY, &desktopKey);
    HANDLE desktopEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    auto ArmDesktop = [&]() {
        if (desktopKey && desktopEvent)
            RegNotifyChangeKeyValue(desktopKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                    desktopEvent, TRUE);
    };
    ArmDesktop();
    wchar_t lastName[64] = L"";
    ReadDesktopName(lastName, 64);
    GUID lastDesktopId{};
    bool lastDesktopValid = ReadCurrentDesktopId(&lastDesktopId);

    // During an active OSD, sample laptop brightness. Otherwise wait for
    // notifications, using no 50ms background polling loop.
    while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        DWORD now = GetTickCount();
        bool active = (LONG)(g_wantDdcUntil - now) > 0;
        HANDLE waits[3] = {g_stopEvent, g_pollWakeEvent, desktopEvent};
        DWORD count = desktopKey && desktopEvent ? 3 : 2;
        DWORD result = WaitForMultipleObjects(count, waits, FALSE, active ? 80 : INFINITE);
        if (result == WAIT_OBJECT_0) break;
        if (g_rebindAudio.exchange(false)) Bind();
        if (result == WAIT_OBJECT_0 + 2) {
            wchar_t name[64]; ReadDesktopName(name, 64);
            // A registry notification may mean a rename or an unrelated write.
            // Only a change of the current desktop GUID is a desktop switch.
            if (wcscmp(lastName, name) != 0)
                wcscpy(lastName, name);
            GUID current{};
            if (ReadCurrentDesktopId(&current)) {
                if (lastDesktopValid && memcmp(&lastDesktopId, &current, sizeof(current)) != 0) {
                    g_deskTick = GetTickCount();
                    if (!g_deskTick) g_deskTick = 1;
                    HWND ui = g_ui;
                    if (ui) PostMessageW(ui, MSG_DESKTOP, 0, 0);
                }
                lastDesktopId = current;
                lastDesktopValid = true;
            }
            ArmDesktop();
        }
        now = GetTickCount();
        if ((LONG)(g_wantDdcUntil - now) > 0) {
            int b = -1;
            if (lcdOk) b = ReadLcd(hLcd);
            else if (now - lastDdc >= 250) { b = ReadDdc(); lastDdc = now; }
            if (b >= 0 && b != pb) {
                pb = b; g_bri = b; g_briTick = GetTickCount();
                PostValue(1, b, false);
            }
        }
    }
    // Unregister before releasing callbacks and COM objects.
    Unbind();
    if (en) { en->UnregisterEndpointNotificationCallback(deviceCb); en->Release(); }
    volumeCb->Release(); deviceCb->Release();
    if (desktopKey) RegCloseKey(desktopKey);
    if (desktopEvent) CloseHandle(desktopEvent);
    if (hLcd != INVALID_HANDLE_VALUE) CloseHandle(hLcd);
    if (comOk) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------- rendu
enum { S_HIDDEN, S_PENDING, S_OPEN, S_CLOSING };
static int st = S_HIDDEN;
static DWORD stStart = 0, lastTick = 0, lastDeskRead = 0;
static int g_kind = 0;                   // 0 volume, 1 luminosité, 2 bureau
static int g_pendKind = 0;               // 0 incertain, 1 touche volume, 2 Win+Ctrl
static DWORD g_showTick = 0;
static bool g_muted = false;
static bool g_unfolded = false, g_phase2 = false;
static wchar_t g_deskName[64] = L"";
// Classification fallback for keys which do not produce a new value at a limit.
// All of these fields are accessed exclusively on the UI thread.
static int g_lastConfirmedKind = 0; // 0 volume, 1 brightness, 2 desktop
static DWORD g_lastConfirmedTick = 0;
static GUID g_observedDesktopId{};
static bool g_observedDesktopValid = false;
static bool g_pendingDesktopChange = false;
static bool g_pendingHide = false;
static DWORD g_nativeHideDeadline = 0; // avoids losing a short native OSD cycle
static DWORD g_brightnessHideDeadline = 0; // merge fast repeat presses into one continuous brightness OSD


static bool DesktopIdChanged() {
    GUID current{};
    if (!ReadCurrentDesktopId(&current)) return false;
    if (!g_observedDesktopValid) {
        g_observedDesktopId = current;
        g_observedDesktopValid = true;
        return false;
    }
    if (memcmp(&current, &g_observedDesktopId, sizeof(current)) == 0)
        return false;
    g_observedDesktopId = current;
    return true;
}


static Spring sp_p, sp_y, sp_v;
static float g_curV = 0, g_p = 0, g_ga = 0, g_yOff = 0, g_startOff = 0;

static float g_s = 1.0f, g_W = 220, g_H = 44;
static int g_pad = 16, g_padY = 16, g_cw = 0, g_ch = 0;
static POINT g_dst{0, 0};
static LONG g_baseY = 0;

static HDC g_scrDC = nullptr, g_dc = nullptr, g_maskDC = nullptr;
static HBITMAP g_bmp = nullptr, g_maskBmp = nullptr, g_oldBmp = nullptr, g_oldMask = nullptr;
static DWORD* g_bits = nullptr; static BYTE* g_maskBits = nullptr;
static HFONT g_fIcon = nullptr, g_fText = nullptr;

// valeurs actives (volume/luminosité ou bureau)
static int a_w = 220, a_h = 44, a_bottom = 70, a_hold = 120, a_closeHold = 140, g_fontH = 0;
static DWORD g_deskT0 = 0;
static bool g_hidePending = false;

// Impulsion directionnelle : déplacement horizontal + compression verticale.
static Spring sp_sh;
static Spring sp_shape;                 // enveloppe de déformation indépendante du déplacement
static float g_shakeK = 100, g_shakeC = 5, g_shakeW = 18;
static float g_shapeK = 100, g_shapeC = 20; // ressort d'attaque (compression progressive)
static float g_shapeReleaseK = 100, g_shapeReleaseC = 10; // ressort de retour sous-amorti
static float g_squash = 0, g_stretch = 0;
static volatile LONG g_keyDir = 0;       // +1 volume +, -1 volume -, 0 autre
static bool g_pendBump = false;          // secousse à jouer une fois la pilule dépliée
static DWORD g_lastBump = 0;
static bool g_volumeKeyWasHeld = false; // permet un retour au relâchement physique de la touche
static bool g_shapeReturning = false; // garantit un vrai retour, sans temporisation répétée

// mode compact (enchaînement de changements)
static Spring sp_c;                      // 0 = pilule normale, 1 = compacte autour de la barre
static DWORD g_lastChg = 0;
static int g_chain = 0;                  // nombre de changements rapprochés

static HBITMAP MakeDib(HDC dc, int w, int h, void** bits) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(dc, &bi, DIB_RGB_COLORS, bits, nullptr, 0);
}

static void FreeCanvas() {
    if (g_dc && g_oldBmp) SelectObject(g_dc, g_oldBmp);
    if (g_maskDC && g_oldMask) SelectObject(g_maskDC, g_oldMask);
    if (g_bmp) DeleteObject(g_bmp);
    if (g_maskBmp) DeleteObject(g_maskBmp);
    g_bmp = g_maskBmp = nullptr; g_bits = nullptr; g_maskBits = nullptr;
    g_oldBmp = g_oldMask = nullptr;
}

static void EnsureCanvas() {
    g_W = a_w * g_s; g_H = a_h * g_s;

    // La pilule peut devenir beaucoup plus large qu'au repos. Le bitmap doit
    // pouvoir contenir l'etirement maximal ET le depassement du ressort
    // d'ouverture (g_p jusqu'a 1.12), sinon son contour est coupe aux bords.
    const float stretchMax = (g_kind == 2) ? 0.0f :
        1.65f * Clamp(g.shakeWidthPct / 100.0f, 0.0f, 1.0f);
    const float maxBodyW = g_W * (1.0f + stretchMax) * 1.12f;
    const int neededPad = (int)ceilf((maxBodyW - g_W) * 0.5f + 20.0f * g_s);
    g_pad = (int)ceilf(Clamp((float)neededPad, 24.0f * g_s, 1000.0f * g_s));

    // Une marge verticale independante evite de dessiner inutilement un bitmap
    // gigantesque lorsque seul l'etirement horizontal augmente.
    g_padY = (int)ceilf(fmaxf(20.0f * g_s, g_H * 0.25f));
    int cw = (int)ceilf(g_W) + 2 * g_pad;
    int ch = (int)ceilf(g_H) + 2 * g_padY;
    if (g_bmp && cw == g_cw && ch == g_ch) return;
    FreeCanvas();
    g_cw = cw; g_ch = ch;
    g_bmp = MakeDib(g_scrDC, cw, ch, (void**)&g_bits);
    g_maskBmp = MakeDib(g_scrDC, cw, ch, (void**)&g_maskBits);
    g_oldBmp = (HBITMAP)SelectObject(g_dc, g_bmp);
    g_oldMask = (HBITMAP)SelectObject(g_maskDC, g_maskBmp);
}

static float CapDist(float px, float py, float xa, float xb, float cy, float r) {
    if (xb < xa) xa = xb = (xa + xb) * 0.5f;
    float qx = Clamp(px, xa, xb), dx = px - qx, dy = py - cy;
    return sqrtf(dx * dx + dy * dy) - r;
}

static wchar_t IconGlyph() {
    if (g_kind == 2) return 0xE7F4;                 // écran / bureau
    if (g_kind == 1) return 0xE706;                 // luminosité
    if (g_muted) return 0xE74F;                     // muet
    int v = (int)(g_curV + 0.5f);
    if (v <= 0) return 0xE992;
    if (v < 34) return 0xE993;
    if (v < 67) return 0xE994;
    return 0xE995;
}

static void Render() {
    if (!g_bits || !g_maskBits || !g_ui) return;
    const int CW = g_cw, CH = g_ch;
    const bool desk = (g_kind == 2);

    // Progression du ressort compact. On conserve un peu de dépassement
    // au-dessus et en dessous de la cible pour rendre le rebond visible.
    const float cm = desk ? 0.0f : Clamp(sp_c.x, -0.18f, 1.22f);

    // Le contour rétrécit progressivement de 5 % en mode compact.
    // L'impulsion comprime aussi brièvement la pilule sur son axe vertical.
    const float compactScale = Lerp(1.0f, 0.95f, cm);
    const float s = g_s;
    // Chaque impulsion comprime la hauteur et élargit légèrement la pilule.
    // L'intensité suit la vitesse du ressort : les impulsions rapprochées
    // s'additionnent et produisent un déplacement/une déformation plus marqués.
    const float Hf = g_H * compactScale * (1.0f - g_squash);
    const float Wf = g_W * compactScale * (1.0f + g_stretch);
    const float vShown = Clamp(g_curV, 0, 100);
    float cx = CW * 0.5f, cy = CH * 0.5f, r = Hf * 0.5f;
    float curW = Hf + (Wf - Hf) * g_p;
    float x0 = cx - curW * 0.5f, x1 = cx + curW * 0.5f;
    float ca = Clamp((g_p - 0.55f) / 0.45f, 0, 1);
    ca = ca * ca * (3 - 2 * ca);
    float iconCx = x0 + r;
    float textR = x1 - 12 * s, textL = textR - 36 * s;
    float bx0 = x0 + Hf - 4 * s, bx1 = textL - 8 * s;
    if (bx1 < bx0 + 2) bx1 = bx0 + 2;
    float tr = 3 * s * (1.0f - g_squash);

    // ---- mode compact : le contour se resserre autour de la barre, qui s'étire
    const float trC = 4 * s * (1.0f - g_squash), cRad = trC + 4 * s * (1.0f - g_squash); // barre et rayon compact
    const float Lc = Wf * 0.78f;                         // longueur de la barre en mode compact
    float rr0 = Lerp(r, cRad, cm);
    float hw = Lerp(curW * 0.5f, Lc * 0.5f + 4 * s, cm);
    float sa = cx - hw + rr0, sb = cx + hw - rr0;        // centres des extrémités du contour
    bx0 = Lerp(bx0, cx - Lc * 0.5f, cm);
    bx1 = Lerp(bx1, cx + Lc * 0.5f, cm);
    tr  = Lerp(tr, trC, cm);
    const float fadeC = 1.0f - Clamp(cm * 2.2f, 0, 1);   // icône et valeur disparaissent

    memset(g_maskBits, 0, (size_t)CW * CH * 4);
    SetBkMode(g_maskDC, TRANSPARENT);
    wchar_t glyph[2] = { IconGlyph(), 0 };
    SelectObject(g_maskDC, g_fIcon);
    SetTextColor(g_maskDC, RGB(255, 0, 0));
    RECT ir = { (LONG)(iconCx - r), (LONG)(cy - r), (LONG)(iconCx + r), (LONG)(cy + r) };
    DrawTextW(g_maskDC, glyph, 1, &ir, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
    if (ca > 0.01f) {
        SelectObject(g_maskDC, g_fText);
        SetTextColor(g_maskDC, RGB(0, 255, 0));
        if (desk) {
            RECT tr2 = { (LONG)(x0 + Hf - 2 * s), (LONG)(cy - r), (LONG)textR, (LONG)(cy + r) };
            DrawTextW(g_maskDC, g_deskName, -1, &tr2,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        } else {
            wchar_t num[8];
            _snwprintf(num, 8, L"%d", (int)(vShown + 0.5f));
            RECT tr2 = { (LONG)textL, (LONG)(cy - r), (LONG)textR, (LONG)(cy + r) };
            DrawTextW(g_maskDC, num, -1, &tr2, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        }
    }
    GdiFlush();

    float accR = g_muted ? 150 : 96, accG = g_muted ? 150 : 205, accB = g_muted ? 150 : 255;
    float fillW = (bx1 - bx0) * vShown / 100.0f;
    float fa = bx0 + tr, fb = bx0 + (fillW > 2 * tr ? fillW : 2 * tr) - tr;

    for (int y = 0; y < CH; y++) {
        for (int x = 0; x < CW; x++) {
            float px = x + 0.5f, py = y + 0.5f;
            float d = CapDist(px, py, sa, sb, cy, rr0);
            float cov = Clamp(0.5f - d, 0, 1);
            if (cov <= 0) { g_bits[y * CW + x] = 0; continue; }

            float R = 32, G = 32, B = 32;
            float bf = Clamp((d + 1.5f) / 1.5f, 0, 1) * 0.8f;
            R = Lerp(R, 95, bf); G = Lerp(G, 95, bf); B = Lerp(B, 95, bf);

            if (!desk && ca > 0.001f) {
                float td = CapDist(px, py, bx0 + tr, bx1 - tr, cy, tr);
                float tc = Clamp(0.5f - td, 0, 1) * ca * 0.28f;
                R = Lerp(R, 255, tc); G = Lerp(G, 255, tc); B = Lerp(B, 255, tc);
                if (vShown > 0.2f) {
                    float fd = CapDist(px, py, fa, fb, cy, tr);
                    float fc = Clamp(0.5f - fd, 0, 1) * ca;
                    R = Lerp(R, accR, fc); G = Lerp(G, accG, fc); B = Lerp(B, accB, fc);
                }
            }
            BYTE* m = (BYTE*)&g_maskBits[(y * CW + x) * 4];     // B G R A
            float ic = m[2] / 255.0f * fadeC, tx = m[1] / 255.0f * ca * fadeC;
            R = Lerp(R, 255, ic); G = Lerp(G, 255, ic); B = Lerp(B, 255, ic);
            R = Lerp(R, 255, tx); G = Lerp(G, 255, tx); B = Lerp(B, 255, tx);

            float A = 0.94f * cov * g_ga;
            DWORD a = (DWORD)(A * 255.0f + 0.5f);
            DWORD rr = (DWORD)(R * A + 0.5f), gg = (DWORD)(G * A + 0.5f), bb = (DWORD)(B * A + 0.5f);
            g_bits[y * CW + x] = (a << 24) | (rr << 16) | (gg << 8) | bb;
        }
    }

    BLENDFUNCTION bf{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT src{0, 0}; SIZE sz{ CW, CH };
    // Le ressort pousse la pilule horizontalement dans le sens du changement.
    // Valeur positive = droite ; valeur négative = gauche.
    POINT dst{
        g_dst.x + (LONG)lroundf(sp_sh.x),
        g_baseY + (LONG)lroundf(g_yOff) + (LONG)lroundf(8.0f * g_s * cm)
    };
    UpdateLayeredWindow(g_ui, g_scrDC, &dst, &sz, g_dc, &src, 0, &bf, ULW_ALPHA);
}

// ---------------------------------------------------------------- profils
static void MakeFonts(int h) {
    if (g_fIcon && g_fText && h == g_fontH && fabsf(g_fontScale - g_s) < 0.001f) return;
    if (g_maskDC) SelectObject(g_maskDC, GetStockObject(SYSTEM_FONT));   // libère les anciennes polices
    if (g_fIcon) DeleteObject(g_fIcon);
    if (g_fText) DeleteObject(g_fText);
    g_fIcon = CreateFontW(-(int)(h * 0.33f * g_s), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          DEFAULT_PITCH, L"Segoe Fluent Icons");
    g_fText = CreateFontW(-(int)(h * 0.28f * g_s), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          DEFAULT_PITCH, L"Segoe UI");
    g_fontH = h;
    g_fontScale = g_s;
}

static void UseProfile(bool desk) {
    if (desk) {
        a_w = dk.widthPx; a_h = dk.heightPx; a_bottom = dk.bottomOffset;
        a_hold = dk.holdMs; a_closeHold = dk.closeHoldMs;
        ComputeSprings(dk.responseMs, dk.dampingPct);
    } else {
        a_w = g.widthPx; a_h = g.heightPx; a_bottom = g.bottomOffset;
        a_hold = g.holdMs; a_closeHold = g.closeHoldMs;
        ComputeSprings(g.responseMs, g.dampingPct);
    }
    MakeFonts(a_h);
}

static void StartClose() {
    g_brightnessHideDeadline = 0;
    st = S_CLOSING; stStart = GetTickCount();
    g_phase2 = false; g_hidePending = false;
    sp_p.t = 0;
    // Si le retour automatique est désactivé, conserver la forme compacte
    // pendant toute l'animation de disparition.
    if (g.compactReturnToNormal) sp_c.t = 0;
}

// ---------------------------------------------------------------- secousse
static void ComputeShake() {
    float period = g.shakeMs / 1000.0f;
    g_shakeW = 6.2831853f / period;
    MakeSpringParams(period, Clamp(g.shakeDampPct / 100.0f, 0.1f, 1.0f), &g_shakeK, &g_shakeC);

    // Deux réponses indépendantes : montée rapide et sans à-coups pendant
    // le maintien, retour lent et élastique au relâchement.
    MakeSpringParams(0.32f, 0.86f, &g_shapeK, &g_shapeC);
    // 100 % conserve exactement les reglages du ressort de la v4.4.
    // Une puissance plus grande laisse davantage de rebond (moins d'amortissement).
    const float power = Clamp(g.returnPowerPct / 100.0f, 0.0f, 3.0f);
    const float releaseDamping = Clamp(0.32f / sqrtf(fmaxf(power, 0.35f)), 0.18f, 0.80f);
    MakeSpringParams(0.70f, releaseDamping, &g_shapeReleaseK, &g_shapeReleaseC);
}

static void Bump(int dir, float scale) {
    if (g.shakePx <= 0 || scale <= 0) return;

    // Chaque changement décale doucement la cible dans la direction correspondante.
    // Les changements successifs s'accumulent, sans ajouter de coups de vitesse brusques.
    const float maxOffset = g.shakePx * g_s * 1.8f;
    sp_sh.t = Clamp(sp_sh.t + dir * scale * g.shakePx * g_s * 0.35f,
                    -maxOffset, maxOffset);

    // La compression augmente progressivement tant que les valeurs s'enchaînent.
    // La cible est plafonnée pour éviter que la pilule ne disparaisse presque.
    sp_shape.t = Clamp(sp_shape.t + scale * 0.22f, 0.0f, 0.95f);
    g_shapeReturning = false;
    g_lastBump = GetTickCount();
}

// Appui sur volume +/- alors que le niveau est déjà au max / min
static void CheckLimitKeys(DWORD now) {
    if (g_kind != 0 || g_muted) return;
    bool up = (GetAsyncKeyState(VK_VOLUME_UP) & 0x8000) != 0;
    bool dn = (GetAsyncKeyState(VK_VOLUME_DOWN) & 0x8000) != 0;
    bool atMax = up && g_vol >= 100;
    bool atMin = dn && g_vol <= 0;
    if ((atMax || atMin) && now - g_lastBump > 260)      // se répète si la touche reste enfoncée
        Bump(atMax ? 1 : -1, 1.0f);
}

// ---------------------------------------------------------------- machine d'états
static void LoadValues() {
    g_muted = false;
    float v = 0;
    if (g_kind == 2) { ReadDesktopName(g_deskName, 64); lastDeskRead = GetTickCount(); }
    else if (g_kind == 0) { v = (float)(g_vol < 0 ? 0 : g_vol); g_muted = g_mute != 0; }
    else { v = (float)(g_bri < 0 ? 50 : g_bri); }
    sp_v.x = sp_v.t = v; sp_v.v = 0;
    g_curV = v;
}

static void BeginShow() {
    g_brightnessHideDeadline = 0;
    HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(primary, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
        dpiX = GetDpiForSystem();
    g_s = (dpiX ? dpiX : 96) / 96.0f;
    UseProfile(g_kind == 2);
    EnsureCanvas();
    MONITORINFO mi{}; mi.cbSize = sizeof(mi);
    GetMonitorInfoW(primary, &mi);
    g_dst.x = (mi.rcMonitor.left + mi.rcMonitor.right) / 2 - g_cw / 2;
    g_dst.y = mi.rcMonitor.bottom - (LONG)(a_bottom * g_s) - (LONG)g_H - g_padY;
    g_baseY = g_dst.y;
    g_startOff = (float)(mi.rcMonitor.bottom - g_baseY) + 4;

    sp_y.x = g_startOff; sp_y.v = 0; sp_y.t = 0;
    sp_p.x = 0; sp_p.v = 0; sp_p.t = 0;
    g_unfolded = false; g_phase2 = false;

    LoadValues();
    g_p = 0; g_ga = 0; g_yOff = g_startOff;
    sp_sh.x = 0; sp_sh.v = 0; sp_sh.t = 0;
    sp_shape.x = 0; sp_shape.v = 0; sp_shape.t = 0;
    g_lastBump = GetTickCount();
    g_volumeKeyWasHeld = false;
    g_shapeReturning = false;
    g_squash = 0; g_stretch = 0;

    g_bumpDir = 0;
    if (g_kind == 0 && !g_muted) {
        if (g_keyDir > 0 && g_vol >= 100) g_bumpDir = 1;
        else if (g_keyDir < 0 && g_vol <= 0) g_bumpDir = -1;
    } else if (g_kind == 1 && g_limitDir) g_bumpDir = g_limitDir;
    g_pendBump = g_bumpDir != 0;

    sp_c.x = 0; sp_c.v = 0; sp_c.t = 0;
    g_chain = g.compactEnabled ? 1 : 0;
    g_lastChg = GetTickCount();
    // Le premier appui est celui qui a déclenché l'affichage.
    if (g.compactEnabled && g.compactTriggerCount <= 1) sp_c.t = 1;

    Render();
    g_deskT0 = GetTickCount(); g_hidePending = false;
    st = S_OPEN; stStart = GetTickCount(); lastTick = stStart;
    SetWindowPos(g_ui, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

// Only use events associated with this OSD appearance. Older poll results
// must not classify a volume-at-limit press as brightness or desktop.
static bool EventNearShow(DWORD tick, DWORD now, DWORD earlyMs) {
    return tick != 0 &&
        (LONG)(tick - (g_showTick - earlyMs)) >= 0 &&
        (LONG)(now - tick) >= 0;
}

static void Tick() {
    DWORD now = GetTickCount();
    float dt = Clamp((now - lastTick) / 1000.0f, 0.001f, 0.05f);
    lastTick = now;

    if (st == S_PENDING) {
        DWORD el = now - stStart;
        const bool volumeKeyHeld =
            ((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN) |
              GetAsyncKeyState(VK_VOLUME_MUTE)) & 0x8000) != 0;
        const bool desktopKeysHeld =
            ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) &&
            (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        const bool volumeEvent = EventNearShow(g_volTick, now, 110);
        const bool brightnessEvent = EventNearShow(g_briTick, now, 110);
        // Registry writes alone are not sufficient to identify a desktop switch.
        // They can also happen when the desktop list or a label is updated.
        const bool desktopIntent = g_pendKind == 2 || desktopKeysHeld;
        if ((desktopIntent || (!volumeKeyHeld && !volumeEvent && !brightnessEvent)) &&
            DesktopIdChanged())
            g_pendingDesktopChange = true;

        int decidedKind = -1;
        // Physical volume keys and actual value notifications outrank cached
        // desktop information, including while a desktop pill is still visible.
        if (g_pendKind == 1 || volumeKeyHeld) {
            decidedKind = 0;
        } else if (g_pendingDesktopChange && !volumeEvent && !brightnessEvent) {
            decidedKind = 2;
        } else if (desktopIntent && !volumeEvent && !brightnessEvent) {
            decidedKind = 2;
        } else if (volumeEvent || brightnessEvent) {
            if (volumeEvent && brightnessEvent)
                decidedKind = (LONG)(g_briTick - g_volTick) > 0 ? 1 : 0;
            else decidedKind = brightnessEvent ? 1 : 0;
        } else if ((int)el >= g.decideMs) {
            // On many laptops the brightness keys don't produce a standard
            // virtual-key event. At 100% (or 0%), pressing the brightness key
            // also produces NO value-change notification. Previously this
            // branch always fell back to volume, even when brightness was at
            // its limit and no volume input was observed.
            //
            // Only infer brightness at its limit after actual volume-key,
            // volume-notification and desktop-change checks above have failed.
            // A recent confirmed brightness event is stronger evidence, but a
            // known brightness limit should also work for the first press.
            const bool recentBrightness = g_lastConfirmedKind == 1 &&
                (now - g_lastConfirmedTick) < 30000;
            const bool knownBrightnessLimit = (g_bri >= 0 && g_bri <= 2) || g_bri >= 98;
            const bool noVolumeEvidence = g_pendKind != 1 &&
                !volumeKeyHeld && !volumeEvent;
            const bool noDesktopEvidence = !g_pendingDesktopChange &&
                !desktopIntent;
            decidedKind = noVolumeEvidence && noDesktopEvidence &&
                (recentBrightness || knownBrightnessLimit) ? 1 : 0;
        }

        if (decidedKind >= 0 && (int)el >= g.startDelayMs) {
            g_kind = decidedKind;
            if (decidedKind == 1 && brightnessEvent) {
                g_lastConfirmedKind = 1;
                g_lastConfirmedTick = now;
            } else if (decidedKind == 0 && (g_pendKind == 1 || volumeEvent)) {
                g_lastConfirmedKind = 0;
                g_lastConfirmedTick = now;
            }
            const bool shortNativeCycle = g_pendingHide;
            g_pendingHide = false;
            g_pendingDesktopChange = false;
            BeginShow();
            if (shortNativeCycle) {
                if (decidedKind == 2)
                    g_hidePending = true; // Native OSD hid before desktop classification ended.
                else
                    g_nativeHideDeadline = now + 650;
            }
        }
        return;
    }
    if (st == S_OPEN) {
        // Brightness Fn keys can briefly hide/re-show the native OSD for
        // every auto-repeat, even when brightness remains at 100%. Do not
        // close our pill until that sequence actually stops.
        if (g_kind == 1 && g_brightnessHideDeadline &&
            (LONG)(now - g_brightnessHideDeadline) >= 0) {
            g_brightnessHideDeadline = 0;
            StartClose();
        }
        if (st != S_OPEN) return;
        if (!g_unfolded && (int)(now - stStart) >= a_hold) {
            g_unfolded = true; sp_p.t = 1;
            if (g_pendBump) { Bump(g_bumpDir, 1.0f); g_pendBump = false; }
        }
        CheckLimitKeys(now);
        if (g_nativeHideDeadline &&
            (LONG)(now - g_nativeHideDeadline) >= 0 &&
            !((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN)) & 0x8000)) {
            g_nativeHideDeadline = 0;
            StartClose();
        }
        // Native OSD sometimes disappears without passing through ShowWindow.
        // Ensure the custom window cannot stay visible indefinitely.
        const bool volumeButtonHeld = g_kind == 0 &&
            ((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN)) & 0x8000);
        if (g_kind != 2 && !volumeButtonHeld && now - stStart > 6500 &&
            now - g_lastBump > 1200 && now - g_volTick > 1200 &&
            now - g_briTick > 1200) StartClose();
        if (!g.compactEnabled) {
            // Désactivation immédiate du mode compact si l'option est décochée.
            sp_c.t = 0;
            g_chain = 0;
        } else if (g.compactReturnToNormal && sp_c.t > 0.5f) {
            // Le retour au mode normal est optionnel.
            bool held = g_kind == 0 &&
                (((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN)) & 0x8000) != 0);
            if (held) g_lastChg = now;
            else if (g.compactReleaseMs > 0 &&
                     (int)(now - g_lastChg) > g.compactReleaseMs) {
                sp_c.t = 0;
                g_chain = 0;
            }
        }
        g_ga = Clamp(g_ga + dt / 0.09f, 0, 1);
        g_wantDdcUntil = now + 1500;
        if (g_kind == 2) {
            if (now - lastDeskRead > 120) {
                wchar_t nn[64];
                ReadDesktopName(nn, 64);
                if (wcscmp(nn, g_deskName) != 0) {        // nouveau changement de bureau
                    wcscpy(g_deskName, nn);
                    g_deskT0 = now; g_hidePending = false;
                }
                lastDeskRead = now;
            }
            bool closeNow = dk.durationMs > 0 ? (int)(now - g_deskT0) >= dk.durationMs
                                              : g_hidePending || (int)(now - g_deskT0) >= 3000;
            if (closeNow) StartClose();
        }
    } else if (st == S_CLOSING) {
        g_ga = 1;
        if (!g_phase2 && (int)(now - stStart) >= a_closeHold) { g_phase2 = true; sp_y.t = g_startOff; }
        if ((g_phase2 && sp_y.x >= g_startOff - 2) || (int)(now - stStart) > 3000) {
            ShowWindow(g_ui, SW_HIDE);
            st = S_HIDDEN;
            return;
        }
    } else return;

    Step(sp_p, dt, g_k, g_c);
    Step(sp_y, dt, g_k, g_c);
    Step(sp_v, dt, g_kv, g_cv);
    // Les événements audio Windows n'arrivent pas nécessairement à intervalles
    // réguliers. Ne JAMAIS relâcher le ressort entre deux événements tant que
    // la touche volume est physiquement maintenue.
    const bool volumeHeld = g_kind == 0 &&
        (((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN)) & 0x8000) != 0);
    const bool releasedNow = g_volumeKeyWasHeld && !volumeHeld;
    g_volumeKeyWasHeld = volumeHeld;

    // Pour les touches volume visibles via GetAsyncKeyState : retour immédiat
    // dès le relâchement. Pour la luminosité, les touches Fn non détectables
    // et les contrôles logiciels : petit délai de regroupement des événements,
    // pour préserver l'accumulation durant un maintien.
    const bool eventsFinished = g_lastBump && (now - g_lastBump >= 170);
    if (!g_shapeReturning && (releasedNow || (!volumeHeld && eventsFinished))) {
        sp_sh.t = 0;
        sp_shape.t = 0;
        g_shapeReturning = true;

        // Petite impulsion supplementaire au relachement : elle accelere le
        // retour vers le repos et genere une surcompensation plus visible.
        // 100 % = base rebound; lower values soften the return.
        const float extra = Clamp(g.returnPowerPct / 100.0f - 1.0f, -1.0f, 2.0f);
        if (extra > 0.0f) {
            sp_shape.v -= extra * 0.72f * sqrtf(g_shapeReleaseK) * fmaxf(sp_shape.x, 0.0f);
            sp_sh.v -= extra * 0.24f * g_shakeW * sp_sh.x;
        }
    }
    Step(sp_sh, dt, g_shakeK, g_shakeC);
    Step(sp_shape, dt,
         g_shapeReturning ? g_shapeReleaseK : g_shapeK,
         g_shapeReturning ? g_shapeReleaseC : g_shapeC);
    Step(sp_c, dt, g_kc, g_cc);

    g_p = Clamp(sp_p.x, 0, 1.12f);
    g_yOff = sp_y.x;
    g_curV = sp_v.x;
    {
        // L'enveloppe de forme dure plus longtemps que la simple impulsion de vitesse.
        // Elle permet une compression/extension visible, puis une légère surcompensation
        // avant le retour au repos. Les impulsions rapprochées se cumulent dans ce ressort.
        // Autorise une surcompensation légèrement plus ample au retour,
        // mais garde la même compression maximale que la version appréciée.
        const float intensity = Clamp(sp_shape.x / 0.58f, -0.65f, 1.65f);
        // Valeur positive : compression + élargissement. Valeur négative :
        // légère surcompensation (la pilule grandit et se resserre) au rebond.
        g_squash = intensity * 0.34f;
        g_stretch = intensity * (g.shakeWidthPct / 100.0f);
    }
    Render();
}

static void LoadSettings();
static LRESULT CALLBACK UiProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
    case MSG_SHOW: {
        DWORD now = GetTickCount();
        g_wantDdcUntil = now + 2000;
        if (g_pollWakeEvent) SetEvent(g_pollWakeEvent);
        const int keyHint = (int)w;
        const bool volumeKeyHeld = keyHint == 1 ||
            (((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN) |
               GetAsyncKeyState(VK_VOLUME_MUTE)) & 0x8000) != 0);
        const bool recentVolume = EventNearShow(g_volTick, now, 110);
        const bool recentBrightness = EventNearShow(g_briTick, now, 110);
        // Do not interpret an old registry tick as another desktop switch.
        // Keep the current desktop ID untouched if the volume key is held.
        const bool desktopSwitch = !volumeKeyHeld && !recentVolume &&
            !recentBrightness && DesktopIdChanged();
        const bool carryDesktop = st == S_PENDING && g_pendingDesktopChange &&
            (now - g_showTick) < 400 && !volumeKeyHeld &&
            !recentVolume && !recentBrightness;

        // If ShowWindow is signaled repeatedly for the same native cycle,
        // do not postpone classification indefinitely.
        if (st == S_PENDING && now - g_showTick < 65 &&
            (g_pendKind == keyHint || (carryDesktop && keyHint == 0)))
            return 0;

        // Repeated presses of the SAME kind should extend the current pill,
        // not restart its animation. A different kind must be reclassified.
        if (st == S_OPEN && g_kind == 0 &&
            (volumeKeyHeld || (recentVolume && !recentBrightness && !desktopSwitch))) {
            g_nativeHideDeadline = 0;
            g_hidePending = false;
            return 0;
        }
        if (st == S_OPEN && g_kind == 2 && (keyHint == 2 || desktopSwitch) &&
            !volumeKeyHeld && !recentVolume && !recentBrightness) {
            BeginShow(); // next virtual desktop, same visual profile
            return 0;
        }
        if (st == S_OPEN && g_kind == 2 && keyHint == 0 &&
            !volumeKeyHeld && !recentVolume && !recentBrightness && !desktopSwitch) {
            // Repeated ShowWindow for an unchanged desktop must not suddenly
            // replace the desktop pill by an unrelated volume pill.
            g_deskT0 = now;
            return 0;
        }
        // Brightness Fn keys may hide/show the native XAML host on every
        // repeat or when reversing direction. A new ShowWindow can arrive
        // BEFORE the brightness poll has observed the new value. If our
        // brightness pill is already visible, treat this as its continuation
        // unless a physical volume key or an actual desktop switch is seen.
        // A genuine volume callback is handled in MSG_VALUES and can still
        // replace the brightness pill immediately.
        const bool continueBrightness = !desktopSwitch && !volumeKeyHeld &&
            keyHint != 1 && keyHint != 2;
        if ((st == S_OPEN || st == S_CLOSING) && g_kind == 1 &&
            continueBrightness) {
            g_brightnessHideDeadline = 0;
            g_nativeHideDeadline = 0;
            g_hidePending = false;
            if (st == S_CLOSING) {
                // Reverse the ongoing close smoothly, preserving spring state.
                st = S_OPEN;
                stStart = now - (DWORD)a_hold;
                g_unfolded = true;
                g_phase2 = false;
                sp_p.t = 1.0f;
                sp_y.t = 0.0f;
            }
            return 0;
        }

        // This also handles a new volume/brightness OSD while the virtual
        // desktop pill is open or an older pill is closing.
        g_brightnessHideDeadline = 0;
        g_pendingDesktopChange = desktopSwitch || carryDesktop;
        g_pendKind = carryDesktop && keyHint == 0 ? 2 : keyHint;
        g_showTick = now;
        g_pendingHide = false;
        g_nativeHideDeadline = 0;
        st = S_PENDING;
        stStart = now;
        lastTick = now;
        return 0;
    }
    case MSG_HIDE:
        if (st == S_PENDING) {
            // Windows may hide its original OSD before our decision delay
            // finishes. Never discard the queued show in this case.
            g_pendingHide = true;
        } else if (st == S_OPEN) {
            if (g_kind == 2) g_hidePending = true;
            else if (g_kind == 1) {
                // A short grace period avoids a close/open flicker between
                // consecutive brightness-key repeats at the hardware limit.
                // A new MSG_SHOW clears the deadline without restarting.
                g_brightnessHideDeadline = GetTickCount() + 330;
            } else StartClose();
        }
        return 0;
    case MSG_DESKTOP:
        // A desktop event is valid only if the GUID has really changed.
        // This may arrive before or after the native XAML ShowWindow event.
        if (DesktopIdChanged()) {
            DWORD now = GetTickCount();
            const bool volumeHeld =
                ((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN) |
                  GetAsyncKeyState(VK_VOLUME_MUTE)) & 0x8000) != 0;
            // If another control was just used, its live notification wins.
            if (volumeHeld || EventNearShow(g_volTick, now, 180) ||
                EventNearShow(g_briTick, now, 180))
                return 0;
            g_pendingDesktopChange = true;
            g_pendKind = 2;
            g_showTick = now;
            g_pendingHide = false;
            g_nativeHideDeadline = 0;
            if (st == S_OPEN && g_kind == 2) {
                BeginShow();
            } else {
                st = S_PENDING;
                stStart = now;
                lastTick = now;
            }
        }
        return 0;
    case MSG_VALUES: {
        const int kind = (int)w;
        if (kind != 0 && kind != 1) return 0;
        const int v = (int)(l & 0xFF);
        const bool mute = (l & 0x100) != 0;
        const DWORD now = GetTickCount();
        // Brightness polling can report the new value after the native host
        // appeared. Allow that genuine notification to teach the signature.
        // Do not learn from a desktop-only notification or an old HWND.
        const bool desktopShortcutHeld =
            ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) &&
            (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        if (g_lastNativeCandidateTick && now - g_lastNativeCandidateTick <= 650 &&
            !(g_deskTick && now - g_deskTick <= 650) &&
            !desktopShortcutHeld && !g_pendingDesktopChange && g_pendKind != 2)
            LearnNativeOsd(g_osd, kind == 0 ? L"volume callback" : L"brightness callback");
        g_lastConfirmedKind = kind;
        g_lastConfirmedTick = now;

        if (st == S_PENDING) {
            // The latest real audio/brightness event resolves any ambiguity.
            // The pending classifier will pick up its timestamp immediately.
            if (kind == 0) g_volTick = now;
            else g_briTick = now;
            return 0;
        }
        if (st == S_OPEN || st == S_CLOSING) {
            g_nativeHideDeadline = 0;
            if (kind != g_kind) {
                // A change of indicator type replaces the old pill. Keep
                // this behavior for volume/brightness/desktop transitions.
                g_kind = kind;
                g_pendingDesktopChange = false;
                g_pendKind = kind == 0 ? 1 : 0;
                BeginShow();
            } else {
                // Same kind while closing: reverse the closing spring in
                // place, rather than resetting the opening animation.
                if (st == S_CLOSING) {
                    st = S_OPEN;
                    stStart = now - (DWORD)a_hold;
                    g_unfolded = true;
                    g_phase2 = false;
                    sp_p.t = 1.0f;
                    sp_y.t = 0.0f;
                }
                if (kind == 1) g_brightnessHideDeadline = 0;
            }
            if ((float)v != sp_v.t && g_unfolded) {
                Bump(v > sp_v.t ? 1 : -1, g.valueShakePct / 100.0f);
                if (g.compactEnabled) {
                    bool sameSequence = g.compactReleaseMs <= 0 ||
                        (int)(now - g_lastChg) <= g.compactReleaseMs;
                    g_chain = sameSequence ? g_chain + 1 : 1;
                    g_lastChg = now;
                    if (g_chain >= g.compactTriggerCount) sp_c.t = 1;
                } else {
                    g_chain = 0;
                    sp_c.t = 0;
                }
            }
            sp_v.t = (float)v;
            g_muted = mute;
            g_hidePending = false;
        }
        return 0;
    }
    case MSG_RELOAD:
        LoadSettings();
        UseProfile(g_kind == 2);
        EnsureCanvas();
        if (g_pollWakeEvent) SetEvent(g_pollWakeEvent);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(h, msg, w, l);
}

static DWORD WINAPI UiThread(LPVOID) {
    HMODULE hi = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&UiProc, &hi);
    if (!hi) return 1;
    WNDCLASSW wc{};
    // Avoid reusing a possibly stale class procedure from an older version.
    wc.lpfnWndProc = UiProc; wc.hInstance = hi; wc.lpszClassName = L"WhOsdCustomPill497";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed (error %lu)", GetLastError());
        return 1;
    }

    g_scrDC = GetDC(nullptr);
    g_dc = CreateCompatibleDC(g_scrDC);
    g_maskDC = CreateCompatibleDC(g_scrDC);
    g_s = 1.0f;  // refreshed from the primary monitor in BeginShow
    UseProfile(false);
    EnsureCanvas();

    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
                                WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                                wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1,
                                nullptr, nullptr, hi, nullptr);
    g_ui = hwnd;
    // Cache the initial desktop so the first switch can be detected by GUID.
    g_observedDesktopValid = ReadCurrentDesktopId(&g_observedDesktopId);
    if (!hwnd) {
        Wh_Log(L"OSD Custom Pill: CreateWindowExW failed (error %lu)", GetLastError());
        FreeCanvas();
        if (g_dc) DeleteDC(g_dc);
        if (g_maskDC) DeleteDC(g_maskDC);
        if (g_scrDC) ReleaseDC(nullptr, g_scrDC);
        UnregisterClassW(wc.lpszClassName, hi);
        return 1;
    }

    bool quit = false;
    while (!quit) {
        MSG m;
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) {
            if (m.message == WM_QUIT) { quit = true; break; }
            TranslateMessage(&m); DispatchMessageW(&m);
        }
        if (quit || WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
        if (st != S_HIDDEN) {
            DWORD t = GetTickCount();
            DwmFlush();
            if (GetTickCount() - t < 4) Sleep(8);
            Tick();
        } else {
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        }
    }

    g_ui = nullptr;
    if (hwnd) DestroyWindow(hwnd);
    FreeCanvas();
    if (g_maskDC) SelectObject(g_maskDC, GetStockObject(SYSTEM_FONT));
    if (g_fIcon) DeleteObject(g_fIcon);
    if (g_fText) DeleteObject(g_fText);
    g_fIcon = g_fText = nullptr;
    if (g_dc) DeleteDC(g_dc);
    if (g_maskDC) DeleteDC(g_maskDC);
    if (g_scrDC) ReleaseDC(nullptr, g_scrDC);
    UnregisterClassW(wc.lpszClassName, hi);
    return 0;
}

// ---------------------------------------------------------------- hook ShowWindow
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Orig;

BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd) {
    if (!g_ui || !IsOsd(hwnd)) return ShowWindow_Orig(hwnd, cmd);

    if (cmd == SW_HIDE) {
        BOOL r = ShowWindow_Orig(hwnd, cmd);
        // Restaurer CHAQUE fenetre native que le mod a masquee, pas seulement
        // la derniere dans g_osd. Retirer l'entree sous verrou, puis restaurer
        // la region hors verrou pour eviter un blocage avec le thread Explorer.
        const bool wasHidden = UntrackHiddenOsd(hwnd);
        if (wasHidden && IsWindow(hwnd)) SetWindowRgn(hwnd, nullptr, FALSE);
        if (hwnd == g_osd) PostMessageW(g_ui, MSG_HIDE, 0, 0);
        return r;
    }
    if (IsWindowVisible(hwnd)) {
        // Windows may re-use an already-visible XAML host. In that case
        // the native OSD has been given an empty region by this mod, so
        // still refresh the corresponding custom pill on a new show call.
        if (hwnd == g_osd || IsTrackedHiddenOsd(hwnd)) {
            g_osd = hwnd;
            int hint = 0;
            if ((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN) |
                 GetAsyncKeyState(VK_VOLUME_MUTE)) & 0x8000) hint = 1;
            else if (((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) &&
                     (GetAsyncKeyState(VK_CONTROL) & 0x8000)) hint = 2;
            PostMessageW(g_ui, MSG_SHOW, (WPARAM)hint, 0);
            if (g_pollWakeEvent) SetEvent(g_pollWakeEvent);
        }
        return ShowWindow_Orig(hwnd, cmd);
    }

    RECT rc{};
    if (!GetWindowRect(hwnd, &rc)) return ShowWindow_Orig(hwnd, cmd);
    const int physicalWidth = rc.right - rc.left;
    const int physicalHeight = rc.bottom - rc.top;

    // XAML host sizes vary across Windows 11 builds and display scales.
    // A tight fixed-height test misses the native OSD at 100% and 125%.
    // Match the small host as a candidate; reject known Snap / Task View
    // flyouts by their thread description, regardless of the Z-band.
    // The larger limit also allows the same OSD to be detected at 200%.
    if (physicalWidth <= 0 || physicalHeight <= 0 ||
        physicalWidth >= 700 || physicalHeight >= 250 ||
        IsKnownNonOsdFlyout(hwnd))
        return ShowWindow_Orig(hwnd, cmd);

    // A real virtual-desktop shortcut/switch is a separate supported OSD.
    // Keep the previous desktop behavior even if Windows hosts that indicator
    // on a different thread from the volume/brightness flyout.
    const bool desktopShortcutHeld =
        ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) &&
        (GetAsyncKeyState(VK_CONTROL) & 0x8000);
    const DWORD checkNow = GetTickCount();
    const bool recentDesktopSwitch = g_deskTick &&
        (LONG)(checkNow - g_deskTick) >= 0 && checkNow - g_deskTick <= 650;
    if (!desktopShortcutHeld && !recentDesktopSwitch &&
        !MatchesLearnedNativeOsd(hwnd)) {
        Wh_Log(L"Ignoring XAML flyout: native OSD signature mismatch");
        return ShowWindow_Orig(hwnd, cmd);
    }
    Wh_Log(L"Native OSD candidate: size=%dx%d dpi=%u", physicalWidth,
           physicalHeight, GetDpiForWindow(hwnd));

    int pend = 0;
    if ((GetAsyncKeyState(VK_VOLUME_UP) | GetAsyncKeyState(VK_VOLUME_DOWN) |
         GetAsyncKeyState(VK_VOLUME_MUTE)) & 0x8000) pend = 1;
    else if (((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) &&
             (GetAsyncKeyState(VK_CONTROL) & 0x8000)) pend = 2;

    g_keyDir = 0;
    if (pend == 1) {
        if (GetAsyncKeyState(VK_VOLUME_UP) & 0x8000) g_keyDir = 1;
        else if (GetAsyncKeyState(VK_VOLUME_DOWN) & 0x8000) g_keyDir = -1;
    }

    g_osd = hwnd;
    HRGN emptyRegion = CreateRectRgn(0, 0, 0, 0);
    if (emptyRegion) {
        // Apres succes, Windows possede la region; en cas d'echec, la liberer.
        if (SetWindowRgn(hwnd, emptyRegion, FALSE)) {
            TrackHiddenOsd(hwnd);
        } else {
            DeleteObject(emptyRegion);
        }
    }
    BOOL r = ShowWindow_Orig(hwnd, cmd);
    g_lastNativeCandidateTick = GetTickCount();
    // Learn only from a real value notification, not from the desktop hint
    // or the brightness-at-limit guess. Some notifications arrive shortly
    // AFTER ShowWindow, and MSG_VALUES handles that later case.
    const DWORD candidateNow = GetTickCount();
    const bool recentVolumeValue = g_volTick &&
        (LONG)(candidateNow - g_volTick) >= 0 && candidateNow - g_volTick <= 650;
    const bool recentBrightnessValue = g_briTick &&
        (LONG)(candidateNow - g_briTick) >= 0 && candidateNow - g_briTick <= 650;
    const bool recentDesktopChange = g_deskTick &&
        (LONG)(candidateNow - g_deskTick) >= 0 && candidateNow - g_deskTick <= 650;
    if ((recentVolumeValue || recentBrightnessValue) &&
        !recentDesktopChange && !desktopShortcutHeld && pend != 2)
        LearnNativeOsd(hwnd, L"recent volume/brightness value change");
    PostMessageW(g_ui, MSG_SHOW, (WPARAM)pend, 0);
    if (g_pollWakeEvent) SetEvent(g_pollWakeEvent);
    return r;
}

// ---------------------------------------------------------------- init
static void LoadSettings() {
    g.startDelayMs  = Wh_GetIntSetting(L"startDelayMs");
    g.decideMs      = Wh_GetIntSetting(L"decideMs");
    g.holdMs        = Wh_GetIntSetting(L"holdMs");
    g.closeHoldMs   = Wh_GetIntSetting(L"closeHoldMs");
    g.responseMs    = Wh_GetIntSetting(L"responseMs");
    g.dampingPct    = Wh_GetIntSetting(L"dampingPct");
    g.widthPx       = Wh_GetIntSetting(L"widthPx");
    g.heightPx      = Wh_GetIntSetting(L"heightPx");
    g.bottomOffset  = Wh_GetIntSetting(L"bottomOffsetPx");
    g.shakePx       = Wh_GetIntSetting(L"shakePx");
    g.shakeMs       = Wh_GetIntSetting(L"shakeMs");
    g.shakeDampPct  = Wh_GetIntSetting(L"shakeDampingPct");
    g.valueShakePct = Wh_GetIntSetting(L"valueShakePct");
    g.shakeWidthPct = Wh_GetIntSetting(L"shakeWidthPct");
    g.returnPowerPct = Wh_GetIntSetting(L"returnPowerPct");
    g.compactEnabled = Wh_GetIntSetting(L"compactEnabled");
    g.compactTriggerCount = Wh_GetIntSetting(L"compactTriggerCount");
    g.compactReturnToNormal = Wh_GetIntSetting(L"compactReturnToNormal");
    g.compactReleaseMs = Wh_GetIntSetting(L"compactReleaseMs");

    dk.widthPx      = Wh_GetIntSetting(L"deskWidthPx");
    dk.heightPx     = Wh_GetIntSetting(L"deskHeightPx");
    dk.bottomOffset = Wh_GetIntSetting(L"deskBottomOffsetPx");
    dk.holdMs       = Wh_GetIntSetting(L"deskHoldMs");
    dk.closeHoldMs  = Wh_GetIntSetting(L"deskCloseHoldMs");
    dk.responseMs   = Wh_GetIntSetting(L"deskResponseMs");
    dk.dampingPct   = Wh_GetIntSetting(L"deskDampingPct");
    dk.durationMs   = Wh_GetIntSetting(L"deskDurationMs");

    PCWSTR lbl = Wh_GetStringSetting(L"deskLabel");
    AcquireSRWLockExclusive(&g_labelLock);
    wcsncpy(g_deskLabel, lbl[0] ? lbl : L"Desktop", 31);
    g_deskLabel[31] = 0;
    ReleaseSRWLockExclusive(&g_labelLock);
    Wh_FreeStringSetting(lbl);

    if (g.startDelayMs < 0) g.startDelayMs = 0;
    if (g.decideMs < 100) g.decideMs = 100;
    if (g.decideMs > 700) g.decideMs = 700;
    if (g.startDelayMs > 700) g.startDelayMs = 700;
    if (g.holdMs < 0) g.holdMs = 0;
    if (g.closeHoldMs < 0) g.closeHoldMs = 0;
    if (g.responseMs < 100) g.responseMs = 100;
    if (g.heightPx < 30) g.heightPx = 30;
    if (g.heightPx > 160) g.heightPx = 160;
    if (g.widthPx < g.heightPx + 120) g.widthPx = g.heightPx + 120;
    if (g.widthPx > 900) g.widthPx = 900;
    if (g.shakePx < 0) g.shakePx = 0;
    if (g.shakePx > 150) g.shakePx = 150;
    if (g.shakeMs < 100) g.shakeMs = 100;
    if (g.shakeDampPct < 10) g.shakeDampPct = 10;
    if (g.valueShakePct < 0) g.valueShakePct = 0;
    if (g.shakeWidthPct < 0) g.shakeWidthPct = 0;
    if (g.shakeWidthPct > 45) g.shakeWidthPct = 45;
    if (g.returnPowerPct < 0) g.returnPowerPct = 0;
    if (g.returnPowerPct > 300) g.returnPowerPct = 300;
    if (g.compactTriggerCount < 1) g.compactTriggerCount = 1;
    if (g.compactTriggerCount > 100) g.compactTriggerCount = 100;
    if (g.compactReleaseMs < 0) g.compactReleaseMs = 0;

    if (dk.heightPx < 30) dk.heightPx = 30;
    if (dk.heightPx > 160) dk.heightPx = 160;
    if (dk.widthPx < dk.heightPx + 80) dk.widthPx = dk.heightPx + 80;
    if (dk.widthPx > 900) dk.widthPx = 900;
    if (dk.holdMs < 0) dk.holdMs = 0;
    if (dk.closeHoldMs < 0) dk.closeHoldMs = 0;
    if (dk.responseMs < 100) dk.responseMs = 100;
    if (dk.durationMs < 0) dk.durationMs = 0;

    ComputeShake();
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_pollWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_pollWakeEvent) {
        if (g_stopEvent) CloseHandle(g_stopEvent);
        if (g_pollWakeEvent) CloseHandle(g_pollWakeEvent);
        g_stopEvent = g_pollWakeEvent = nullptr;
        return FALSE;
    }
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    g_pollThread = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
    if (!g_uiThread || !g_pollThread) {
        SetEvent(g_stopEvent);
        if (g_uiThread) { WaitForSingleObject(g_uiThread, INFINITE); CloseHandle(g_uiThread); }
        if (g_pollThread) { WaitForSingleObject(g_pollThread, INFINITE); CloseHandle(g_pollThread); }
        CloseHandle(g_stopEvent); CloseHandle(g_pollWakeEvent);
        g_stopEvent = g_pollWakeEvent = nullptr;
        return FALSE;
    }
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig);
    return TRUE;
}

// Hooks have been removed before Wh_ModUninit. Do not restore the native
// window region while a ShowWindow hook invocation could still hide it again.
void Wh_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_uiThread) { WaitForSingleObject(g_uiThread, INFINITE); CloseHandle(g_uiThread); g_uiThread = nullptr; }
    if (g_pollThread) { WaitForSingleObject(g_pollThread, INFINITE); CloseHandle(g_pollThread); g_pollThread = nullptr; }
    // Les hooks sont deja retires : restaurer toutes les regions modifiees.
    RestoreAllHiddenOsdWindows();
    g_osd = nullptr;
    if (g_pollWakeEvent) CloseHandle(g_pollWakeEvent);
    if (g_stopEvent) CloseHandle(g_stopEvent);
    g_pollWakeEvent = g_stopEvent = nullptr;
}

void Wh_ModSettingsChanged() {
    HWND hwnd = g_ui;
    if (hwnd) PostMessageW(hwnd, MSG_RELOAD, 0, 0);
    else LoadSettings();
}
