// ==WindhawkMod==
// @id              vector-screen-holder
// @name            Vector Screen Holder
// @name:es-ES       Mantenedor de pantalla vectorial
// @name:pt-BR       Mantenedor de tela vetorial
// @name:fr-FR       Maintien d'écran vectoriel
// @name:de-DE       Vektor-Bildschirmhalter
// @name:it-IT       Mantenitore schermo vettoriale
// @name:nl-NL       Vector schermhouder
// @name:pl-PL       Wektorowy strażnik ekranu
// @name:tr-TR       Vektörel ekran tutucu
// @name:ru-RU       Векторный хранитель экрана
// @name:uk-UA       Векторний хранитель екрана
// @name:zh-CN       矢量屏幕守护
// @name:zh-TW       向量螢幕守護
// @name:ja-JP       ベクター スクリーンホルダー
// @name:ko-KR       벡터 스크린 홀더
// @name:ar          حافظ الشاشة المتجهي
// @name:he          שומר מסך וקטורי
// @description     Fills a display you choose with generative line art and keeps the PC from idling while it runs
// @description:es-ES Llena la pantalla que elijas con arte lineal generativo y evita que el PC entre en reposo mientras se ejecuta
// @description:pt-BR Preenche a tela escolhida com arte de linhas generativa e impede que o PC entre em ociosidade enquanto roda
// @description:fr-FR Remplit l'écran de votre choix d'art linéaire génératif et empêche le PC de passer en veille pendant son exécution
// @description:de-DE Füllt einen gewählten Bildschirm mit generativer Linienkunst und hält den PC währenddessen aus dem Leerlauf
// @description:it-IT Riempie lo schermo scelto con arte lineare generativa e impedisce al PC di andare in inattività mentre è in esecuzione
// @description:nl-NL Vult een gekozen scherm met generatieve lijnkunst en voorkomt dat de pc inactief wordt zolang het draait
// @description:pl-PL Wypełnia wybrany ekran generatywną grafiką liniową i zapobiega przejściu komputera w stan bezczynności
// @description:tr-TR Seçtiğiniz ekranı üretken çizgi sanatıyla doldurur ve çalışırken bilgisayarın boşta kalmasını önler
// @description:ru-RU Заполняет выбранный экран генеративной линейной графикой и не даёт компьютеру уйти в простой
// @description:uk-UA Заповнює вибраний екран генеративною лінійною графікою й не дає комп'ютеру перейти в простій
// @description:zh-CN 用生成式线条艺术填满所选显示器，并在运行期间防止电脑进入闲置状态
// @description:zh-TW 以生成式線條藝術填滿所選螢幕，並在執行期間防止電腦進入閒置狀態
// @description:ja-JP 選んだディスプレイをジェネラティブなライン アートで満たし、動作中は PC がアイドルにならないようにします
// @description:ko-KR 선택한 디스플레이를 제너러티브 라인 아트로 채우고 실행 중에는 PC가 유휴 상태로 전환되지 않도록 합니다
// @description:ar   يملأ الشاشة التي تختارها بفن خطي توليدي ويمنع الكمبيوتر من الخمول أثناء تشغيله
// @description:he   ממלא מסך לבחירתך באמנות קווית גנרטיבית ומונע מהמחשב לעבור למצב סרק בזמן שהוא פועל
// @version         1.3.3
// @author          akilluminati47
// @github          https://github.com/akilluminati47
// @homepage        https://vector.akilluminati47.pages.dev/
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -ladvapi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vector Screen Holder

Fills a display with generative line art and holds the screen awake for as long
as it runs. **It runs on your primary display, on any single display you name,
or on all of them at once**, so one monitor is as well served as six.

Every frame is drawn as strokes through Direct2D on the GPU. There are no
images, no video file and no fixed resolution, so the artwork is generated for
whatever size the display you choose actually is: a 1080p monitor and a 4K
portrait panel each get correctly proportioned art.

Uploading a dataset, rendering out a model, updating a shelf of games,
seeding torrents overnight, a long build or backup, a download that will take
an hour, or a run of AI agents grinding through a queue. Anything where the
machine has to stay awake and stay signed in, but a black screen tells you
nothing. Start the Screen Holder, put it on whichever display you can spare,
and go to lunch. The work keeps running, the screen stays awake, and you can
still see progress at a glance from across the room.

![The overlay running on a portrait monitor while a note is typed in Notepad](https://raw.githubusercontent.com/akilluminati47/vector-screen-holder/main/assets/typing.gif)

Above: a note being written on the side monitor while the art keeps running
underneath it. The overlay is below the window, not over it.

## Controls

| Input | What it does |
| --- | --- |
| **Esc** | Close the overlay (focus it first, or see below) |
| **Left click** | Cycle to the next enabled style |
| **Right click** | Step the amount: how much information is on screen |
| **Mouse wheel** | Adjust the current style's parameter |
| **Space** | Step to the next palette |
| **Ctrl+Alt+H** | Toggle the overlay on and off (configurable below) |

![The readout naming the style, the wheel value, the amount notch and the palette](https://raw.githubusercontent.com/akilluminati47/vector-screen-holder/main/assets/readout.gif)

Above: the mouse wheel taking vigor from 0 to 100 across a single growth cycle.

The overlay opens clean, with no labels and no chrome, nothing on screen but
the art. Adjusting anything raises one line along the bottom naming the style,
its parameter value, the amount notch and the palette, which fades after about
two seconds. That line is the only text the mod draws, and only your own input
raises it: the rotation timer changes style in silence.

That line is set in a pixel font carried inside the mod, so there is nothing to
install and it looks identical on every machine. The wording is translated into
sixteen languages and follows your Windows display language, unless you pick
one in the settings.

The font carries the Latin, Greek and Cyrillic alphabets. Scripts it does not
carry, Chinese, Japanese, Korean, Arabic, Hebrew and the rest, are drawn in
your own system interface font, size matched so the line still reads as a
single typeface. Either way the size is worked out by measuring against your
display before anything is drawn, so the line never wraps and never runs past
the edge. The font is published on its own as
[a TrueType release](https://github.com/akilluminati47/fusion-pixel-font/releases/tag/vsh-subset-2026.09.01)
if you want it elsewhere.

Every control except the hotkey needs the overlay focused. Click it once and it
takes them, without ever coming to the front. That first click, the one that
moves focus, only raises the readout: it confirms the click landed and the
overlay is listening, without changing what is on screen. Clicks after that
cycle the style as usual.

**Ctrl+Alt+H works from anywhere**, so you can always close the overlay even
when something else has focus.

If you turn on **Click through to the desktop**, none of the mouse controls
apply: every click goes to the desktop instead, which is the point of it. The
hotkey still shows and hides the overlay, the global key still changes the
palette if you have it on, and style, amount and the wheel parameter come from
the settings.

**Start active** brings the overlay up as soon as the mod loads. Windhawk loads
its mods when you sign in, so that means it is waiting on your chosen display
after every reboot, not just the time you ticked the box.

The mod also listens on a named event, `Local\WindhawkVectorScreenHolderToggle`,
in the per-session namespace, so an ordinary Windows shortcut can toggle the
overlay without opening Windhawk at all. Save this as
`toggle-screen-holder.vbs` and make a shortcut to it:

```vbs
CreateObject("WScript.Shell").Run "powershell -nop -w hidden -c ""[Threading.EventWaitHandle]::OpenExisting('Local\WindhawkVectorScreenHolderToggle').Set()""", 0, False
```

The mod has to be **enabled in Windhawk** for that to do anything, because
the event only exists while the mod is loaded. The overlay itself does not
have to be on screen.

If you would rather **Esc** and the palette key reached the overlay from any
application, turn on **Global Esc and Ctrl+Shift+Space** in the settings. The
palette key is a chord there on purpose: plain Space from anywhere would step
the palette on every space you type. The whole setting is off by default
because Esc is a heavily used key, and a reflexive press meant for a dialog or
a search box in another window would end the session and release the
keep-awake with nothing on screen to say it had happened.

The overlay sits above your wallpaper but *below* your windows: anything you
open covers it normally, and it never steals focus by itself or appears in
Alt+Tab.

It does cover the desktop icons on the display it runs on, and by default a
click there goes to the overlay rather than the desktop. Turn on **Click
through to the desktop** and every click passes to the desktop instead, so the
icons keep working with the artwork drawn over them. The overlay is then shown
and hidden by the toggle hotkey rather than by clicking it, which is the trade
that setting makes.

Drawing *behind* the icons rather than over them is possible, and two mods in
this catalog do it, but only by injecting into `explorer.exe` and rendering
into the window between the wallpaper and the icon view. This mod is built not
to load into another process at all, so that route is closed to it by design.

The **Display** setting defaults to your primary screen, which is the right
choice on a single monitor machine; on more than one, point it at whichever
display you are not working on. Either way the overlay only goes for good when
you press Esc or toggle it off.

## Color

Seven palettes: aurora, ember, ocean, neon, forest, mono, and custom, which
takes its colors from the two custom settings. **Space** steps to the next one
while the overlay has focus, and **Ctrl+Shift+Space** does the same from
anywhere if the global key setting is on. The one you land on is remembered,
so you can pick a palette by eye instead of by name. Changing the palette in
the settings overrides whatever you stepped to, so the setting is never a dead
control.

**Hue shift** rotates the whole palette by a fixed number of degrees. It is a
dial across the color wheel rather than a switch: 180 of its 359 degrees lands
on the opposite side and reads as a flip of the palette you picked, and values
between shift it part of the way there. **Leave it at 0, the default**, to get
each palette exactly as it was designed; reach for it when you want a variation
without writing a custom palette.

Turn on the **automatic color ramp** and the hue rotates continuously from
whatever the hue shift is set to. Turning the ramp back off returns the colors
to that value, rather than leaving them wherever the rotation happened to
stop.

One thing to expect when you step the palette mid-composition: flow field,
growth and harmonograph build up into a buffer, so the new palette takes the
background and everything drawn from that moment on, while the strokes already
laid down keep the colors they were drawn in until the next scene begins.
Contours redraw every frame, so they change over wholesale.

## The four styles

Each has its own **parameter** (the wheel) and its own **amount** (right click).
The parameter changes the character of the art; the amount changes how much of
it there is. Amount has five notches: minimal, sparse, balanced, dense, maximal.

| Style | Parameter (wheel) | Amount (right click) |
| --- | --- | --- |
| **Flow field** | turbulence of the underlying field | how tightly ribbons pack: a few broad ones through to many fine ones |
| **Contours** | relief (how rough the terrain is) | number of contour levels, 6 through 46 |
| **Differential growth** | vigor (how hard the colony pushes outward) | number of colonies, 1 through 6 |
| **Harmonograph** | tempo (how fast the figure is drawn) | number of overlaid figures, 1 through 6 |

On flow field and growth the wheel is a pace control as well as a character
one: turning turbulence or vigor up draws the piece as much as five times
faster on its way to the same kind of result. Harmonograph's tempo is only
speed, and contours is paced by the clock alone.

![The four styles: flow field, contours, differential growth, harmonograph](https://raw.githubusercontent.com/akilluminati47/vector-screen-holder/main/assets/styles.png)

Left to right: flow field, contours, differential growth, harmonograph.

All four draw themselves in progressively. Contours and growth keep moving
once the picture is full, holding it a while before fading out; flow field and
harmonograph are one-shot, and begin to fade the moment they finish. Either
way a new piece follows.

The wheel reaches flow field a little differently from the other three. Each
ribbon traces its whole path at the moment it spawns, so a turn of the wheel
steers the ribbons drawn from then on and leaves the ones already on screen as
they were drawn. The other three answer the wheel immediately.

## Keeping the PC awake

While the overlay is up the mod calls `SetThreadExecutionState` with
`ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED`, which is the
supported way to tell Windows the display and system should stay on. The flags
are cleared the moment you close it.

It does **not** fake keystrokes or mouse movement. Some corporate presence
tools (Teams, Slack) track real input rather than display state and will still
mark you away.

The overlay is an ordinary window, so it belongs to the virtual desktop it was
opened on. Switch desktops and it goes with the rest of them; the screen is
still held awake, you just will not see the art until you switch back. Windows
offers no supported way to pin a window across desktops.

For the same reason it will not hold a managed workstation open. Suppressing
the screen saver also suppresses the lock that rides on it, which covers the
consumer default, but a machine-inactivity policy measures real input idle
time and will lock on schedule regardless.

## What it costs

The overlay is real work on the GPU and the CPU, and the whole point is to run
it *while* something else is busy, so it is worth being plain about the price.

Measured, rather than guessed at: on a 1440x2560 portrait display driven at
**180** frames per second, which is three times the default, differential
growth at the *balanced* amount costs about **0.88 of one core**, and stepping
it to *maximal* takes that to about **0.95**. On a sixteen core machine that
is under 6% of the whole CPU. At the default 60 the same work comes to roughly
a third of those figures.

The default is 60 on purpose rather than something lower. How smoothly the
art moves is most of the first impression, and the default amount is
*balanced* rather than maximal, so nobody lands on the expensive end by
accident. **Frames per second** is yours to set anywhere from 10 to 240.

Contours is the heaviest of the four, because it re-marches the field and
rebuilds every contour level on every frame, where flow field, growth and
harmonograph draw themselves in and then idle on the finished picture. If you
want the mod further out of the way of a long job:

- drop **Frames per second**. The artwork is paced against the clock rather
  than the frame count, so a lower rate stays smooth instead of becoming
  choppy.
- step the **amount** down a notch or two with right click.
- prefer flow field or harmonograph over contours.

Nothing is simulated at all while the overlay is genuinely occluded. That is
narrower than it sounds: with desktop composition on, which is always on these
days, an ordinary maximized window over the overlay is still composited and
does not count. What does count is the workstation locked, or a fullscreen
exclusive window on that display.

On more than one display the overlays are presented without waiting on vsync,
so they do not divide a single refresh between them. The cost is some tearing
in the artwork, which single display users do not get.

## Source and credits

The mod lives at
[github.com/akilluminati47/vector-screen-holder](https://github.com/akilluminati47/vector-screen-holder),
along with the browser prototype the four algorithms were developed in before
the Direct2D port. There is a short showcase of all four styles at
[vector.akilluminati47.pages.dev](https://vector.akilluminati47.pages.dev/).

Originally created by [**akilluminati47**](https://akilluminati47.github.io/akilluminati47/),
developed with the help of the AI pair-programmers Claude and Big-Pickle
(opencode).

None of the four techniques is original to this mod. They are well-trodden
generative art, and these are the implementations and write-ups each scene was
built from:

| Style | Drawn after |
| --- | --- |
| **Flow field** | [moistkitteh/Flowfield_Generative_Art](https://github.com/moistkitteh/Flowfield_Generative_Art), itself after Tyler Hobbs' *Fidenza* |
| **Contours** | [arthurxavierx/contour-lines](https://github.com/arthurxavierx/contour-lines) by Arthur Xavier |
| **Differential growth** | [inconvergent/differential-line](https://github.com/inconvergent/differential-line) by inconvergent |
| **Harmonograph** | [Paul Bourke's harmonograph notes](https://paulbourke.net/geometry/harmonograph/) |

The readout is set in [Fusion Pixel Font](https://github.com/TakWolf/fusion-pixel-font)
by [TakWolf](https://takwolf.com), used under the SIL Open Font License 1.1.
The 717 glyph subset the mod carries, and the script that cuts it, are
published at
[akilluminati47/fusion-pixel-font](https://github.com/akilluminati47/fusion-pixel-font/releases/tag/vsh-subset-2026.09.01).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: Ctrl+Alt+H
  $name: Toggle hotkey
  $name:es-ES: Atajo para alternar
  $name:pt-BR: Atalho para alternar
  $name:fr-FR: Raccourci de bascule
  $name:de-DE: Tastenkürzel zum Umschalten
  $name:it-IT: Scorciatoia di attivazione
  $name:nl-NL: Sneltoets
  $name:pl-PL: Skrót przełączający
  $name:tr-TR: Açma kapama kısayolu
  $name:ru-RU: Горячая клавиша
  $name:uk-UA: Гаряча клавіша
  $name:zh-CN: 开关快捷键
  $name:zh-TW: 開關快速鍵
  $name:ja-JP: 切り替えのホットキー
  $name:ko-KR: 전환 단축키
  $name:ar: مفتاح التبديل
  $name:he: מקש קיצור להחלפה
  $description: >-
    Global hotkey to show and hide the overlay. Modifiers are Ctrl, Alt, Shift
    and Win, joined with "+". The key itself can be A-Z, 0-9 or F1-F24; other
    keys such as arrows, numpad and punctuation are not supported and the mod
    logs that no hotkey was registered. Leave empty to disable.

    A letter or digit needs at least one modifier. A registered hotkey is taken
    before any window sees it and stays registered while the mod is loaded, so
    a bare "A" would cost you that letter everywhere; the mod refuses those and
    logs why. Bare F1-F24 are accepted.
- monitor: primary
  $name: Display
  $name:es-ES: Pantalla
  $name:pt-BR: Tela
  $name:fr-FR: Écran
  $name:de-DE: Bildschirm
  $name:it-IT: Schermo
  $name:nl-NL: Scherm
  $name:pl-PL: Ekran
  $name:tr-TR: Ekran
  $name:ru-RU: Экран
  $name:uk-UA: Екран
  $name:zh-CN: 显示器
  $name:zh-TW: 螢幕
  $name:ja-JP: ディスプレイ
  $name:ko-KR: 디스플레이
  $name:ar: الشاشة
  $name:he: מסך
  $description: >-
    Which display to hold. Any resolution and orientation works, because the
    art is generated to fit whatever the display actually is. The numbered
    choices come from the \\.\DISPLAYn device names. Those usually match the
    numbers Windows Settings shows, but the two are produced by different
    parts of Windows and can disagree after displays are re-arranged. The mod
    log lists every display with its number, resolution and position when the
    mod loads; use the resolution to confirm which is which. The list stops at
    Display 8; beyond that, use All displays.
  $options:
  - primary: Primary display
  - all: All displays
  - "1": Display 1
  - "2": Display 2
  - "3": Display 3
  - "4": Display 4
  - "5": Display 5
  - "6": Display 6
  - "7": Display 7
  - "8": Display 8
  $options:es-ES:
  - primary: Pantalla principal
  - all: Todas las pantallas
  - "1": Pantalla 1
  - "2": Pantalla 2
  - "3": Pantalla 3
  - "4": Pantalla 4
  - "5": Pantalla 5
  - "6": Pantalla 6
  - "7": Pantalla 7
  - "8": Pantalla 8
  $options:pt-BR:
  - primary: Tela principal
  - all: Todas as telas
  - "1": Tela 1
  - "2": Tela 2
  - "3": Tela 3
  - "4": Tela 4
  - "5": Tela 5
  - "6": Tela 6
  - "7": Tela 7
  - "8": Tela 8
  $options:fr-FR:
  - primary: Écran principal
  - all: Tous les écrans
  - "1": Écran 1
  - "2": Écran 2
  - "3": Écran 3
  - "4": Écran 4
  - "5": Écran 5
  - "6": Écran 6
  - "7": Écran 7
  - "8": Écran 8
  $options:de-DE:
  - primary: Hauptbildschirm
  - all: Alle Bildschirme
  - "1": Bildschirm 1
  - "2": Bildschirm 2
  - "3": Bildschirm 3
  - "4": Bildschirm 4
  - "5": Bildschirm 5
  - "6": Bildschirm 6
  - "7": Bildschirm 7
  - "8": Bildschirm 8
  $options:it-IT:
  - primary: Schermo principale
  - all: Tutti gli schermi
  - "1": Schermo 1
  - "2": Schermo 2
  - "3": Schermo 3
  - "4": Schermo 4
  - "5": Schermo 5
  - "6": Schermo 6
  - "7": Schermo 7
  - "8": Schermo 8
  $options:nl-NL:
  - primary: Hoofdscherm
  - all: Alle schermen
  - "1": Scherm 1
  - "2": Scherm 2
  - "3": Scherm 3
  - "4": Scherm 4
  - "5": Scherm 5
  - "6": Scherm 6
  - "7": Scherm 7
  - "8": Scherm 8
  $options:pl-PL:
  - primary: Ekran główny
  - all: Wszystkie ekrany
  - "1": Ekran 1
  - "2": Ekran 2
  - "3": Ekran 3
  - "4": Ekran 4
  - "5": Ekran 5
  - "6": Ekran 6
  - "7": Ekran 7
  - "8": Ekran 8
  $options:tr-TR:
  - primary: Birincil ekran
  - all: Tüm ekranlar
  - "1": Ekran 1
  - "2": Ekran 2
  - "3": Ekran 3
  - "4": Ekran 4
  - "5": Ekran 5
  - "6": Ekran 6
  - "7": Ekran 7
  - "8": Ekran 8
  $options:ru-RU:
  - primary: Основной экран
  - all: Все экраны
  - "1": Экран 1
  - "2": Экран 2
  - "3": Экран 3
  - "4": Экран 4
  - "5": Экран 5
  - "6": Экран 6
  - "7": Экран 7
  - "8": Экран 8
  $options:uk-UA:
  - primary: Основний екран
  - all: Усі екрани
  - "1": Екран 1
  - "2": Екран 2
  - "3": Екран 3
  - "4": Екран 4
  - "5": Екран 5
  - "6": Екран 6
  - "7": Екран 7
  - "8": Екран 8
  $options:zh-CN:
  - primary: 主显示器
  - all: 所有显示器
  - "1": 显示器 1
  - "2": 显示器 2
  - "3": 显示器 3
  - "4": 显示器 4
  - "5": 显示器 5
  - "6": 显示器 6
  - "7": 显示器 7
  - "8": 显示器 8
  $options:zh-TW:
  - primary: 主螢幕
  - all: 所有螢幕
  - "1": 螢幕 1
  - "2": 螢幕 2
  - "3": 螢幕 3
  - "4": 螢幕 4
  - "5": 螢幕 5
  - "6": 螢幕 6
  - "7": 螢幕 7
  - "8": 螢幕 8
  $options:ja-JP:
  - primary: メイン ディスプレイ
  - all: すべてのディスプレイ
  - "1": ディスプレイ 1
  - "2": ディスプレイ 2
  - "3": ディスプレイ 3
  - "4": ディスプレイ 4
  - "5": ディスプレイ 5
  - "6": ディスプレイ 6
  - "7": ディスプレイ 7
  - "8": ディスプレイ 8
  $options:ko-KR:
  - primary: 주 디스플레이
  - all: 모든 디스플레이
  - "1": 디스플레이 1
  - "2": 디스플레이 2
  - "3": 디스플레이 3
  - "4": 디스플레이 4
  - "5": 디스플레이 5
  - "6": 디스플레이 6
  - "7": 디스플레이 7
  - "8": 디스플레이 8
  $options:ar:
  - primary: الشاشة الرئيسية
  - all: كل الشاشات
  - "1": الشاشة 1
  - "2": الشاشة 2
  - "3": الشاشة 3
  - "4": الشاشة 4
  - "5": الشاشة 5
  - "6": الشاشة 6
  - "7": الشاشة 7
  - "8": الشاشة 8
  $options:he:
  - primary: המסך הראשי
  - all: כל המסכים
  - "1": מסך 1
  - "2": מסך 2
  - "3": מסך 3
  - "4": מסך 4
  - "5": מסך 5
  - "6": מסך 6
  - "7": מסך 7
  - "8": מסך 8
- fps: 60
  $name: Frames per second
  $name:es-ES: Fotogramas por segundo
  $name:pt-BR: Quadros por segundo
  $name:fr-FR: Images par seconde
  $name:de-DE: Bilder pro Sekunde
  $name:it-IT: Fotogrammi al secondo
  $name:nl-NL: Beelden per seconde
  $name:pl-PL: Klatki na sekundę
  $name:tr-TR: Saniyedeki kare sayısı
  $name:ru-RU: Кадров в секунду
  $name:uk-UA: Кадрів на секунду
  $name:zh-CN: 每秒帧数
  $name:zh-TW: 每秒影格數
  $name:ja-JP: フレームレート
  $name:ko-KR: 초당 프레임 수
  $name:ar: الإطارات في الثانية
  $name:he: פריימים לשנייה
  $description: >-
    Render rate of the overlay. Higher is smoother; the artwork itself is paced
    by wall-clock time, so raising this makes the motion finer without making
    anything draw faster. Clamped to 10-240.
- language: auto
  $name: Readout language
  $name:es-ES: Idioma del rótulo
  $name:pt-BR: Idioma do rótulo
  $name:fr-FR: Langue de l'affichage
  $name:de-DE: Sprache der Anzeige
  $name:it-IT: Lingua della scritta
  $name:nl-NL: Taal van de tekstregel
  $name:pl-PL: Język opisu
  $name:tr-TR: Bilgi satırı dili
  $name:ru-RU: Язык подписи
  $name:uk-UA: Мова підпису
  $name:zh-CN: 信息栏语言
  $name:zh-TW: 資訊列語言
  $name:ja-JP: 表示の言語
  $name:ko-KR: 표시 언어
  $name:ar: لغة السطر المعروض
  $name:he: שפת השורה המוצגת
  $description: >-
    Language for the line the overlay draws when you change something.
    Automatic follows the Windows display language and falls back to English
    when that language is not one of the ones listed. This page itself follows
    Windhawk's own language setting, not this one.
  $options:
  - auto: Automatic (match Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:es-ES:
  - auto: Automático (igual que Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:pt-BR:
  - auto: Automático (igual ao Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:fr-FR:
  - auto: Automatique (comme Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:de-DE:
  - auto: Automatisch (wie Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:it-IT:
  - auto: Automatica (come Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:nl-NL:
  - auto: Automatisch (zoals Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:pl-PL:
  - auto: Automatycznie (jak Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:tr-TR:
  - auto: Otomatik (Windows ile aynı)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:ru-RU:
  - auto: Автоматически (как в Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:uk-UA:
  - auto: Автоматично (як у Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:zh-CN:
  - auto: 自动（跟随 Windows）
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:zh-TW:
  - auto: 自動（跟隨 Windows）
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:ja-JP:
  - auto: 自動 (Windows に合わせる)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:ko-KR:
  - auto: 자동(Windows에 맞춤)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:ar:
  - auto: تلقائي (حسب Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
  $options:he:
  - auto: אוטומטי (לפי Windows)
  - en: English
  - es-ES: Español
  - pt-BR: Português (Brasil)
  - fr-FR: Français
  - de-DE: Deutsch
  - it-IT: Italiano
  - nl-NL: Nederlands
  - pl-PL: Polski
  - tr-TR: Türkçe
  - ru-RU: Русский
  - uk-UA: Українська
  - zh-CN: 简体中文
  - zh-TW: 繁體中文
  - ja-JP: 日本語
  - ko-KR: 한국어
  - ar: العربية
  - he: עברית
- enableFlow: true
  $name: "Style: flow field"
  $name:es-ES: "Estilo: campo de flujo"
  $name:pt-BR: "Estilo: campo de fluxo"
  $name:fr-FR: "Style : champ de flux"
  $name:de-DE: "Stil: Strömungsfeld"
  $name:it-IT: "Stile: campo di flusso"
  $name:nl-NL: "Stijl: stromingsveld"
  $name:pl-PL: "Styl: pole przepływu"
  $name:tr-TR: "Stil: akış alanı"
  $name:ru-RU: "Стиль: поле потока"
  $name:uk-UA: "Стиль: поле потоку"
  $name:zh-CN: 样式：流场
  $name:zh-TW: 樣式：流場
  $name:ja-JP: "スタイル: フローフィールド"
  $name:ko-KR: "스타일: 흐름장"
  $name:ar: "النمط: حقل التدفق"
  $name:he: "סגנון: שדה זרימה"
  $description: Include this style when cycling with a click or on the rotation timer.
- enableContour: true
  $name: "Style: contours"
  $name:es-ES: "Estilo: curvas de nivel"
  $name:pt-BR: "Estilo: curvas de nível"
  $name:fr-FR: "Style : courbes de niveau"
  $name:de-DE: "Stil: Höhenlinien"
  $name:it-IT: "Stile: curve di livello"
  $name:nl-NL: "Stijl: hoogtelijnen"
  $name:pl-PL: "Styl: poziomice"
  $name:tr-TR: "Stil: eş yükselti eğrileri"
  $name:ru-RU: "Стиль: изолинии"
  $name:uk-UA: "Стиль: ізолінії"
  $name:zh-CN: 样式：等高线
  $name:zh-TW: 樣式：等高線
  $name:ja-JP: "スタイル: 等高線"
  $name:ko-KR: "스타일: 등고선"
  $name:ar: "النمط: خطوط الكنتور"
  $name:he: "סגנון: קווי גובה"
- enableGrowth: true
  $name: "Style: differential growth"
  $name:es-ES: "Estilo: crecimiento diferencial"
  $name:pt-BR: "Estilo: crescimento diferencial"
  $name:fr-FR: "Style : croissance différentielle"
  $name:de-DE: "Stil: differenzielles Wachstum"
  $name:it-IT: "Stile: crescita differenziale"
  $name:nl-NL: "Stijl: differentiële groei"
  $name:pl-PL: "Styl: wzrost różnicowy"
  $name:tr-TR: "Stil: diferansiyel büyüme"
  $name:ru-RU: "Стиль: дифференциальный рост"
  $name:uk-UA: "Стиль: диференційний ріст"
  $name:zh-CN: 样式：差异生长
  $name:zh-TW: 樣式：差異生長
  $name:ja-JP: "スタイル: 微分成長"
  $name:ko-KR: "스타일: 미분 성장"
  $name:ar: "النمط: النمو التفاضلي"
  $name:he: "סגנון: צמיחה דיפרנציאלית"
- enableHarmonograph: true
  $name: "Style: harmonograph"
  $name:es-ES: "Estilo: armonógrafo"
  $name:pt-BR: "Estilo: harmonógrafo"
  $name:fr-FR: "Style : harmonographe"
  $name:de-DE: "Stil: Harmonograph"
  $name:it-IT: "Stile: armonografo"
  $name:nl-NL: "Stijl: harmonograaf"
  $name:pl-PL: "Styl: harmonograf"
  $name:tr-TR: "Stil: harmonograf"
  $name:ru-RU: "Стиль: гармонограф"
  $name:uk-UA: "Стиль: гармонограф"
  $name:zh-CN: 样式：谐波图
  $name:zh-TW: 樣式：諧波圖
  $name:ja-JP: "スタイル: ハーモノグラフ"
  $name:ko-KR: "스타일: 하모노그래프"
  $name:ar: "النمط: هارمونوغراف"
  $name:he: "סגנון: הרמונוגרף"
- rotate: false
  $name: Rotate through styles
  $name:es-ES: Rotar entre estilos
  $name:pt-BR: Alternar entre estilos
  $name:fr-FR: Faire défiler les styles
  $name:de-DE: Stile durchwechseln
  $name:it-IT: Ruota tra gli stili
  $name:nl-NL: Door stijlen roteren
  $name:pl-PL: Przełączaj style
  $name:tr-TR: Stiller arasında geçiş yap
  $name:ru-RU: Менять стили по кругу
  $name:uk-UA: Змінювати стилі по колу
  $name:zh-CN: 轮换样式
  $name:zh-TW: 輪換樣式
  $name:ja-JP: スタイルを自動で切り替える
  $name:ko-KR: 스타일 순환
  $name:ar: التنقل بين الأنماط
  $name:he: מעבר אוטומטי בין סגנונות
  $description: Automatically move to the next enabled style on a timer.
- rotateSeconds: 300
  $name: Rotation interval (seconds)
  $name:es-ES: Intervalo de rotación (segundos)
  $name:pt-BR: Intervalo de alternância (segundos)
  $name:fr-FR: Intervalle de rotation (secondes)
  $name:de-DE: Wechselintervall (Sekunden)
  $name:it-IT: Intervallo di rotazione (secondi)
  $name:nl-NL: Rotatie-interval (seconden)
  $name:pl-PL: Odstęp przełączania (sekundy)
  $name:tr-TR: Geçiş aralığı (saniye)
  $name:ru-RU: Интервал смены (секунды)
  $name:uk-UA: Інтервал зміни (секунди)
  $name:zh-CN: 轮换间隔（秒）
  $name:zh-TW: 輪換間隔（秒）
  $name:ja-JP: 切り替え間隔 (秒)
  $name:ko-KR: 순환 간격(초)
  $name:ar: فترة التنقل (ثوانٍ)
  $name:he: מרווח המעבר (שניות)
  $description: Clamped to 10-7200.
- amount: balanced
  $name: Amount
  $name:es-ES: Cantidad
  $name:pt-BR: Quantidade
  $name:fr-FR: Quantité
  $name:de-DE: Menge
  $name:it-IT: Quantità
  $name:nl-NL: Hoeveelheid
  $name:pl-PL: Ilość
  $name:tr-TR: Miktar
  $name:ru-RU: Количество
  $name:uk-UA: Кількість
  $name:zh-CN: 数量
  $name:zh-TW: 數量
  $name:ja-JP: 量
  $name:ko-KR: 양
  $name:ar: الكمية
  $name:he: כמות
  $description: >-
    Starting amount notch, meaning how much information is on screen. Right
    click the
    overlay to step it.
  $options:
  - minimal: Minimal
  - sparse: Sparse
  - balanced: Balanced
  - dense: Dense
  - maximal: Maximal
  $options:es-ES:
  - minimal: Mínimo
  - sparse: Escaso
  - balanced: Equilibrado
  - dense: Denso
  - maximal: Máximo
  $options:pt-BR:
  - minimal: Mínimo
  - sparse: Esparso
  - balanced: Equilibrado
  - dense: Denso
  - maximal: Máximo
  $options:fr-FR:
  - minimal: Minimal
  - sparse: Épars
  - balanced: Équilibré
  - dense: Dense
  - maximal: Maximal
  $options:de-DE:
  - minimal: Minimal
  - sparse: Spärlich
  - balanced: Ausgewogen
  - dense: Dicht
  - maximal: Maximal
  $options:it-IT:
  - minimal: Minimo
  - sparse: Rado
  - balanced: Equilibrato
  - dense: Denso
  - maximal: Massimo
  $options:nl-NL:
  - minimal: Minimaal
  - sparse: Schaars
  - balanced: Gebalanceerd
  - dense: Dicht
  - maximal: Maximaal
  $options:pl-PL:
  - minimal: Minimalna
  - sparse: Rzadka
  - balanced: Zrównoważona
  - dense: Gęsta
  - maximal: Maksymalna
  $options:tr-TR:
  - minimal: En az
  - sparse: Seyrek
  - balanced: Dengeli
  - dense: Yoğun
  - maximal: En çok
  $options:ru-RU:
  - minimal: Минимум
  - sparse: Разреженно
  - balanced: Сбалансированно
  - dense: Плотно
  - maximal: Максимум
  $options:uk-UA:
  - minimal: Мінімум
  - sparse: Розріджено
  - balanced: Збалансовано
  - dense: Щільно
  - maximal: Максимум
  $options:zh-CN:
  - minimal: 极简
  - sparse: 稀疏
  - balanced: 均衡
  - dense: 密集
  - maximal: 极密
  $options:zh-TW:
  - minimal: 極簡
  - sparse: 稀疏
  - balanced: 均衡
  - dense: 密集
  - maximal: 極密
  $options:ja-JP:
  - minimal: 最小
  - sparse: まばら
  - balanced: 標準
  - dense: 密
  - maximal: 最大
  $options:ko-KR:
  - minimal: 최소
  - sparse: 성김
  - balanced: 균형
  - dense: 조밀
  - maximal: 최대
  $options:ar:
  - minimal: أدنى
  - sparse: متناثر
  - balanced: متوازن
  - dense: كثيف
  - maximal: أقصى
  $options:he:
  - minimal: מינימלי
  - sparse: דליל
  - balanced: מאוזן
  - dense: צפוף
  - maximal: מקסימלי
- parameter: 50
  $name: Parameter (%)
  $name:es-ES: Parámetro (%)
  $name:pt-BR: Parâmetro (%)
  $name:fr-FR: Paramètre (%)
  $name:de-DE: Parameter (%)
  $name:it-IT: Parametro (%)
  $name:nl-NL: Parameter (%)
  $name:pl-PL: Parametr (%)
  $name:tr-TR: Parametre (%)
  $name:ru-RU: Параметр (%)
  $name:uk-UA: Параметр (%)
  $name:zh-CN: 参数（%）
  $name:zh-TW: 參數（%）
  $name:ja-JP: パラメーター (%)
  $name:ko-KR: 매개변수(%)
  $name:ar: المعامل (%)
  $name:he: פרמטר (%)
  $description: >-
    Starting value for the parameter the mouse wheel adjusts. It is a single
    value carried across the styles rather than one per style, so the notch you
    set for one style is the notch the next one starts from. Clamped to 0-100.
- palette: aurora
  $name: Palette
  $name:es-ES: Paleta
  $name:pt-BR: Paleta
  $name:fr-FR: Palette
  $name:de-DE: Palette
  $name:it-IT: Palette
  $name:nl-NL: Palet
  $name:pl-PL: Paleta
  $name:tr-TR: Palet
  $name:ru-RU: Палитра
  $name:uk-UA: Палітра
  $name:zh-CN: 配色
  $name:zh-TW: 配色
  $name:ja-JP: パレット
  $name:ko-KR: 팔레트
  $name:ar: لوحة الألوان
  $name:he: פלטה
  $options:
  - aurora: Aurora (teal / blue / violet)
  - ember: Ember (orange / red / gold)
  - ocean: Ocean (cyan / blue)
  - neon: Neon (magenta / cyan / lime)
  - forest: Forest (green / lime)
  - mono: Monochrome
  - custom: Custom (see below)
  $options:es-ES:
  - aurora: Aurora (turquesa / azul / violeta)
  - ember: Brasa (naranja / rojo / oro)
  - ocean: Océano (cian / azul)
  - neon: Neón (magenta / cian / lima)
  - forest: Bosque (verde / lima)
  - mono: Monocromo
  - custom: Personalizada (ver abajo)
  $options:pt-BR:
  - aurora: Aurora (verde-azulado / azul / violeta)
  - ember: Brasa (laranja / vermelho / dourado)
  - ocean: Oceano (ciano / azul)
  - neon: Néon (magenta / ciano / lima)
  - forest: Floresta (verde / lima)
  - mono: Monocromático
  - custom: Personalizada (veja abaixo)
  $options:fr-FR:
  - aurora: Aurore (sarcelle / bleu / violet)
  - ember: Braise (orange / rouge / or)
  - ocean: Océan (cyan / bleu)
  - neon: Néon (magenta / cyan / vert lime)
  - forest: Forêt (vert / vert lime)
  - mono: Monochrome
  - custom: Personnalisée (voir ci-dessous)
  $options:de-DE:
  - aurora: Aurora (Petrol / Blau / Violett)
  - ember: Glut (Orange / Rot / Gold)
  - ocean: Ozean (Cyan / Blau)
  - neon: Neon (Magenta / Cyan / Limette)
  - forest: Wald (Grün / Limette)
  - mono: Monochrom
  - custom: Benutzerdefiniert (siehe unten)
  $options:it-IT:
  - aurora: Aurora (foglia di tè / blu / viola)
  - ember: Brace (arancione / rosso / oro)
  - ocean: Oceano (ciano / blu)
  - neon: Neon (magenta / ciano / lime)
  - forest: Foresta (verde / lime)
  - mono: Monocromatica
  - custom: Personalizzata (vedi sotto)
  $options:nl-NL:
  - aurora: Aurora (blauwgroen / blauw / violet)
  - ember: Gloed (oranje / rood / goud)
  - ocean: Oceaan (cyaan / blauw)
  - neon: Neon (magenta / cyaan / limoen)
  - forest: Bos (groen / limoen)
  - mono: Monochroom
  - custom: Aangepast (zie hieronder)
  $options:pl-PL:
  - aurora: Zorza (morski / niebieski / fioletowy)
  - ember: Żar (pomarańczowy / czerwony / złoty)
  - ocean: Ocean (cyjan / niebieski)
  - neon: Neon (magenta / cyjan / limonka)
  - forest: Las (zielony / limonka)
  - mono: Monochromatyczna
  - custom: Własna (patrz niżej)
  $options:tr-TR:
  - aurora: Kutup ışığı (deniz mavisi / mavi / mor)
  - ember: Kor (turuncu / kırmızı / altın)
  - ocean: Okyanus (camgöbeği / mavi)
  - neon: Neon (macenta / camgöbeği / limon)
  - forest: Orman (yeşil / limon)
  - mono: Tek renk
  - custom: Özel (aşağıya bakın)
  $options:ru-RU:
  - aurora: Сияние (бирюзовый / синий / фиолетовый)
  - ember: Угли (оранжевый / красный / золотой)
  - ocean: Океан (голубой / синий)
  - neon: Неон (пурпурный / голубой / лайм)
  - forest: Лес (зелёный / лайм)
  - mono: Монохром
  - custom: Своя (см. ниже)
  $options:uk-UA:
  - aurora: Сяйво (бірюзовий / синій / фіолетовий)
  - ember: Жар (оранжевий / червоний / золотий)
  - ocean: Океан (блакитний / синій)
  - neon: Неон (пурпуровий / блакитний / лайм)
  - forest: Ліс (зелений / лайм)
  - mono: Монохром
  - custom: Власна (див. нижче)
  $options:zh-CN:
  - aurora: 极光（青绿 / 蓝 / 紫）
  - ember: 余烬（橙 / 红 / 金）
  - ocean: 海洋（青 / 蓝）
  - neon: 霓虹（品红 / 青 / 柠檬绿）
  - forest: 森林（绿 / 柠檬绿）
  - mono: 单色
  - custom: 自定义（见下）
  $options:zh-TW:
  - aurora: 極光（青綠 / 藍 / 紫）
  - ember: 餘燼（橙 / 紅 / 金）
  - ocean: 海洋（青 / 藍）
  - neon: 霓虹（洋紅 / 青 / 檸檬綠）
  - forest: 森林（綠 / 檸檬綠）
  - mono: 單色
  - custom: 自訂（見下）
  $options:ja-JP:
  - aurora: オーロラ (ティール / ブルー / バイオレット)
  - ember: 残り火 (オレンジ / レッド / ゴールド)
  - ocean: オーシャン (シアン / ブルー)
  - neon: ネオン (マゼンタ / シアン / ライム)
  - forest: フォレスト (グリーン / ライム)
  - mono: モノクロ
  - custom: カスタム (下を参照)
  $options:ko-KR:
  - aurora: 오로라(청록 / 파랑 / 보라)
  - ember: 잔불(주황 / 빨강 / 금색)
  - ocean: 오션(청록 / 파랑)
  - neon: 네온(마젠타 / 청록 / 라임)
  - forest: 포레스트(초록 / 라임)
  - mono: 흑백
  - custom: 사용자 지정(아래 참조)
  $options:ar:
  - aurora: شفق (أزرق مخضر / أزرق / بنفسجي)
  - ember: جمر (برتقالي / أحمر / ذهبي)
  - ocean: محيط (سماوي / أزرق)
  - neon: نيون (أرجواني / سماوي / ليموني)
  - forest: غابة (أخضر / ليموني)
  - mono: أحادي اللون
  - custom: مخصص (انظر أدناه)
  $options:he:
  - aurora: זוהר (טורקיז / כחול / סגול)
  - ember: גחלים (כתום / אדום / זהב)
  - ocean: אוקיינוס (ציאן / כחול)
  - neon: ניאון (מג'נטה / ציאן / ליים)
  - forest: יער (ירוק / ליים)
  - mono: מונוכרום
  - custom: מותאם (ראו למטה)
- customColors: "#29d0a5,#3aa0ff,#7b5cff,#ff5ec4,#e8f3ff"
  $name: Custom colors
  $name:es-ES: Colores personalizados
  $name:pt-BR: Cores personalizadas
  $name:fr-FR: Couleurs personnalisées
  $name:de-DE: Eigene Farben
  $name:it-IT: Colori personalizzati
  $name:nl-NL: Aangepaste kleuren
  $name:pl-PL: Własne kolory
  $name:tr-TR: Özel renkler
  $name:ru-RU: Свои цвета
  $name:uk-UA: Власні кольори
  $name:zh-CN: 自定义颜色
  $name:zh-TW: 自訂顏色
  $name:ja-JP: カスタムの色
  $name:ko-KR: 사용자 지정 색
  $name:ar: ألوان مخصصة
  $name:he: צבעים מותאמים
  $description: Two to six #rrggbb values, comma separated. Used when the palette is Custom.
- customBackground: "#05070d"
  $name: Custom background
  $name:es-ES: Fondo personalizado
  $name:pt-BR: Fundo personalizado
  $name:fr-FR: Arrière-plan personnalisé
  $name:de-DE: Eigener Hintergrund
  $name:it-IT: Sfondo personalizzato
  $name:nl-NL: Aangepaste achtergrond
  $name:pl-PL: Własne tło
  $name:tr-TR: Özel arka plan
  $name:ru-RU: Свой фон
  $name:uk-UA: Власний фон
  $name:zh-CN: 自定义背景
  $name:zh-TW: 自訂背景
  $name:ja-JP: カスタムの背景
  $name:ko-KR: 사용자 지정 배경
  $name:ar: خلفية مخصصة
  $name:he: רקע מותאם
  $description: Background #rrggbb. Used when the palette is Custom.
- hueOffset: 0
  $name: Hue shift (degrees)
  $name:es-ES: Desplazamiento de tono (grados)
  $name:pt-BR: Deslocamento de matiz (graus)
  $name:fr-FR: Décalage de teinte (degrés)
  $name:de-DE: Farbtonverschiebung (Grad)
  $name:it-IT: Spostamento tonalità (gradi)
  $name:nl-NL: Tintverschuiving (graden)
  $name:pl-PL: Przesunięcie barwy (stopnie)
  $name:tr-TR: Renk tonu kaydırma (derece)
  $name:ru-RU: Сдвиг оттенка (градусы)
  $name:uk-UA: Зсув відтінку (градуси)
  $name:zh-CN: 色相偏移（度）
  $name:zh-TW: 色相偏移（度）
  $name:ja-JP: 色相のずらし (度)
  $name:ko-KR: 색조 이동(도)
  $name:ar: إزاحة تدرج اللون (درجات)
  $name:he: הסטת גוון (מעלות)
  $description: >-
    Rotates the palette by a fixed amount, so you can tune the colors without
    editing a custom palette. 0 leaves the palette exactly as defined. This is
    the color you get whenever the automatic ramp is off, and the color the
    artwork returns to the moment you turn the ramp off. Clamped to 0-359.
- colorRamp: false
  $name: Automatic color ramp
  $name:es-ES: Rampa de color automática
  $name:pt-BR: Rampa de cor automática
  $name:fr-FR: Dégradé de couleur automatique
  $name:de-DE: Automatischer Farbverlauf
  $name:it-IT: Variazione automatica del colore
  $name:nl-NL: Automatisch kleurverloop
  $name:pl-PL: Automatyczna zmiana barwy
  $name:tr-TR: Otomatik renk geçişi
  $name:ru-RU: Автоматическая смена оттенка
  $name:uk-UA: Автоматична зміна відтінку
  $name:zh-CN: 自动色相渐变
  $name:zh-TW: 自動色相漸變
  $name:ja-JP: 色相の自動変化
  $name:ko-KR: 자동 색조 변화
  $name:ar: تدرج لوني تلقائي
  $name:he: שינוי גוון אוטומטי
  $description: >-
    Continuously rotate the hue of the artwork, starting from the hue shift
    above. While this is on, the hue shift setting is the starting point
    rather than a fixed value. Turn it off and the colors return to the hue
    shift you set, instead of stopping wherever the rotation happened to be.
- rampSpeed: 12
  $name: Ramp speed (degrees/sec)
  $name:es-ES: Velocidad de la rampa (grados/s)
  $name:pt-BR: Velocidade da rampa (graus/s)
  $name:fr-FR: Vitesse du dégradé (degrés/s)
  $name:de-DE: Verlaufsgeschwindigkeit (Grad/s)
  $name:it-IT: Velocità della variazione (gradi/s)
  $name:nl-NL: Snelheid van het verloop (graden/s)
  $name:pl-PL: Szybkość zmiany (stopnie/s)
  $name:tr-TR: Geçiş hızı (derece/sn)
  $name:ru-RU: Скорость смены (градусы/с)
  $name:uk-UA: Швидкість зміни (градуси/с)
  $name:zh-CN: 渐变速度（度/秒）
  $name:zh-TW: 漸變速度（度/秒）
  $name:ja-JP: 変化の速さ (度/秒)
  $name:ko-KR: 변화 속도(도/초)
  $name:ar: سرعة التدرج (درجة/ثانية)
  $name:he: מהירות השינוי (מעלות/שנייה)
  $description: >-
    How fast the automatic ramp rotates the hue. Only used while the ramp is
    on. Clamped to 1-360.
- opacity: 100
  $name: Opacity (%)
  $name:es-ES: Opacidad (%)
  $name:pt-BR: Opacidade (%)
  $name:fr-FR: Opacité (%)
  $name:de-DE: Deckkraft (%)
  $name:it-IT: Opacità (%)
  $name:nl-NL: Dekking (%)
  $name:pl-PL: Krycie (%)
  $name:tr-TR: Saydamlık (%)
  $name:ru-RU: Непрозрачность (%)
  $name:uk-UA: Непрозорість (%)
  $name:zh-CN: 不透明度（%）
  $name:zh-TW: 不透明度（%）
  $name:ja-JP: 不透明度 (%)
  $name:ko-KR: 불투명도(%)
  $name:ar: العتامة (%)
  $name:he: אטימות (%)
  $description: Below 100 the desktop shows through the overlay. Clamped to 10-100.
- globalKeys: false
  $name: Global Esc and Ctrl+Shift+Space
  $name:es-ES: Esc y Ctrl+Mayús+Espacio globales
  $name:pt-BR: Esc e Ctrl+Shift+Espaço globais
  $name:fr-FR: Échap et Ctrl+Maj+Espace globaux
  $name:de-DE: Esc und Strg+Umschalt+Leertaste global
  $name:it-IT: Esc e Ctrl+Maiusc+Spazio globali
  $name:nl-NL: Esc en Ctrl+Shift+spatie overal
  $name:pl-PL: Globalne Esc i Ctrl+Shift+spacja
  $name:tr-TR: Genel Esc ve Ctrl+Shift+Boşluk
  $name:ru-RU: Глобальные Esc и Ctrl+Shift+пробел
  $name:uk-UA: Глобальні Esc і Ctrl+Shift+пробіл
  $name:zh-CN: 全局 Esc 和 Ctrl+Shift+空格
  $name:zh-TW: 全域 Esc 與 Ctrl+Shift+空白鍵
  $name:ja-JP: Esc と Ctrl+Shift+スペースを全体で有効
  $name:ko-KR: 전역 Esc 및 Ctrl+Shift+스페이스
  $name:ar: Esc و Ctrl+Shift+المسافة بشكل عام
  $name:he: Esc ו-Ctrl+Shift+רווח גלובליים
  $description: >-
    Let Esc close the overlay and Ctrl+Shift+Space change the palette from any
    application, not just when the overlay has focus. The palette key is a
    chord on purpose: plain Space from anywhere would step the palette on every
    space you type. Off by default because Esc is a heavily used key, and a
    reflexive press in another window would end the session and release the
    keep-awake without any visible sign. Plain Space still changes the palette
    when the overlay itself has focus, and the toggle hotkey above always works
    regardless of this setting.
- clickThrough: false
  $name: Click through to the desktop
  $name:es-ES: Clics hacia el escritorio
  $name:pt-BR: Cliques passam para a área de trabalho
  $name:fr-FR: Clics traversants vers le bureau
  $name:de-DE: Klicks zum Desktop durchlassen
  $name:it-IT: Clic trasparenti verso il desktop
  $name:nl-NL: Klikken doorlaten naar bureaublad
  $name:pl-PL: Przepuszczaj kliknięcia na pulpit
  $name:tr-TR: Tıklamalar masaüstüne geçsin
  $name:ru-RU: Пропускать клики на рабочий стол
  $name:uk-UA: Пропускати кліки на робочий стіл
  $name:zh-CN: 鼠标点击穿透到桌面
  $name:zh-TW: 滑鼠點擊穿透至桌面
  $name:ja-JP: クリックをデスクトップに透過
  $name:ko-KR: 클릭을 바탕 화면으로 통과
  $name:ar: تمرير النقرات إلى سطح المكتب
  $name:he: העברת לחיצות לשולחן העבודה
  $description: >-
    Pass every click straight through to the desktop, so the icons under the
    overlay stay usable. Worth turning on if you run the overlay on your only
    display. The trade is that the overlay can no longer be clicked or
    scrolled. The toggle hotkey still shows and hides it and the global key
    above still changes the palette, but the amount and the wheel parameter
    come from the settings above, and the style is whichever one you last left
    it on, or the rotation if you have it on. To pin it to one style, leave
    only that style ticked below.
- keepAwake: true
  $name: Keep the PC awake
  $name:es-ES: Mantener el PC despierto
  $name:pt-BR: Manter o PC acordado
  $name:fr-FR: Empêcher la mise en veille
  $name:de-DE: PC wach halten
  $name:it-IT: Mantieni il PC sveglio
  $name:nl-NL: Pc wakker houden
  $name:pl-PL: Nie pozwól komputerowi zasnąć
  $name:tr-TR: Bilgisayarı uyanık tut
  $name:ru-RU: Не давать ПК уснуть
  $name:uk-UA: Не давати ПК заснути
  $name:zh-CN: 保持电脑唤醒
  $name:zh-TW: 保持電腦喚醒
  $name:ja-JP: PC をスリープさせない
  $name:ko-KR: PC를 깨어 있게 유지
  $name:ar: إبقاء الكمبيوتر مستيقظًا
  $name:he: למנוע מהמחשב להירדם
  $description: >-
    Hold the display and system out of idle while the overlay is up, via
    SetThreadExecutionState.
- startActive: false
  $name: Start active
  $name:es-ES: Iniciar activo
  $name:pt-BR: Iniciar ativo
  $name:fr-FR: Démarrer actif
  $name:de-DE: Aktiv starten
  $name:it-IT: Avvia attivo
  $name:nl-NL: Actief starten
  $name:pl-PL: Uruchom aktywny
  $name:tr-TR: Etkin başlat
  $name:ru-RU: Запускать активным
  $name:uk-UA: Запускати активним
  $name:zh-CN: 启动即显示
  $name:zh-TW: 啟動即顯示
  $name:ja-JP: 起動時に表示する
  $name:ko-KR: 시작할 때 켜기
  $name:ar: البدء نشطًا
  $name:he: להתחיל פעיל
  $description: >-
    Show the overlay as soon as the mod loads, and open it straight away when
    you tick it here. Windhawk loads its mods when you sign in, so leaving this
    on means the overlay is already up on the display you chose after every
    reboot, not only the time you ticked it.
- workAreaOnly: false
  $name: Stay inside the work area
  $name:es-ES: Permanecer en el área de trabajo
  $name:pt-BR: Ficar dentro da área de trabalho
  $name:fr-FR: Rester dans la zone de travail
  $name:de-DE: Im Arbeitsbereich bleiben
  $name:it-IT: Resta nell'area di lavoro
  $name:nl-NL: Binnen het werkgebied blijven
  $name:pl-PL: Pozostań w obszarze roboczym
  $name:tr-TR: Çalışma alanının içinde kal
  $name:ru-RU: Оставаться в рабочей области
  $name:uk-UA: Залишатися в робочій області
  $name:zh-CN: 限制在工作区内
  $name:zh-TW: 限制在工作區內
  $name:ja-JP: 作業領域の中に収める
  $name:ko-KR: 작업 영역 안에 유지
  $name:ar: البقاء داخل منطقة العمل
  $name:he: להישאר בתוך אזור העבודה
  $description: >-
    Keep the overlay inside each display's work area instead of covering the
    whole display, so it never draws over the taskbar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwrite_3.h>
#include <windhawk_utils.h>
#include <sddl.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cwctype>
#include <cwchar>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// Constants and small helpers
// ---------------------------------------------------------------------------

// Declared by hand so the mod never depends on __uuidof or on the import
// library exporting the GUID. Both are toolchain sensitive.
static const GUID kIID_ID2D1Factory = {
    0x06152247, 0x6f50, 0x465a,
    {0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07}};

static const GUID kIID_IDWriteFactory = {
    0xb859ee5a, 0xd838, 0x4b5b,
    {0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48}};

static IDWriteFactory* g_dwrite = nullptr;

// Global hue offset in degrees, shared by every overlay so the displays stay
// in step with one another.
static float g_hue = 0;

static const WCHAR kWindowClass[] = L"WindhawkVectorScreenHolderWnd";

// The mod's own image, which owns the window class and the window procedure.
static HINSTANCE g_modInstance = nullptr;

// True only once SetWindowsHookEx has actually returned a hook. The window
// proc uses it to stay off keys the hook has already handled, so it has to
// follow the hook itself: gating on the setting, or on the hook thread merely
// starting, would leave the key dead in both places whenever the hook failed
// to install.
static std::atomic<bool> g_kbdHookLive{false};

// Set when more than one display is being driven. The worker renders the
// overlays one after another inside a single loop iteration, and a vsynced
// EndDraw blocks until the next refresh, so three displays would each get a
// third of the refresh rate whatever the frame rate setting says. Presenting
// immediately decouples them; the waitable timer is what paces frames anyway.
// A single display keeps vsync, because there is nothing to decouple and
// tearing is a real cost.
static bool g_presentImmediately = false;
static const WCHAR kEventLocal[] = L"Local\\WindhawkVectorScreenHolderToggle";

// Thread hot keys must use 0x0000-0xBFFF; 0xC000+ is reserved for atoms.
static const int kHotkeyId = 0x4A21;

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
static const float kPi = 3.14159265358979f;

enum StyleId {
    kStyleFlow = 0,
    kStyleContour,
    kStyleGrowth,
    kStyleHarmonograph,
    kStyleCount,
};

static const int kAmountCount = 5;

// The words the overlay can put on screen, one row per language. Order within
// each row follows the enums above: the four styles, the wheel parameter that
// belongs to each, the five amount notches, the seven palettes.
//
// English is first and is the fallback for any language not listed. Adding a
// language is a row here plus the matching $name and $options entries in the
// settings block; nothing else needs to change, because the readout is sized
// by measuring whatever is in this table.
static const int kPaletteNames = 7;

struct Strings {
    const wchar_t* tag;
    bool rtl;
    const wchar_t* styles[kStyleCount];
    const wchar_t* params[kStyleCount];
    const wchar_t* amounts[kAmountCount];
    const wchar_t* palettes[kPaletteNames];
    const wchar_t* amount;
};

static const Strings kStrings[] = {
    {L"en", false,
     {L"flow field", L"contours", L"differential growth", L"harmonograph"},
     {L"turbulence", L"relief", L"vigor", L"tempo"},
     {L"minimal", L"sparse", L"balanced", L"dense", L"maximal"},
     {L"aurora", L"ember", L"ocean", L"neon", L"forest", L"mono", L"custom"},
     L"amount"},
    {L"es-ES", false,
     {L"campo de flujo", L"curvas de nivel", L"crecimiento diferencial", L"armonógrafo"},
     {L"turbulencia", L"relieve", L"vigor", L"tempo"},
     {L"mínimo", L"escaso", L"equilibrado", L"denso", L"máximo"},
     {L"aurora", L"brasa", L"océano", L"neón", L"bosque", L"monocromo", L"personalizada"},
     L"cantidad"},
    {L"pt-BR", false,
     {L"campo de fluxo", L"curvas de nível", L"crescimento diferencial", L"harmonógrafo"},
     {L"turbulência", L"relevo", L"vigor", L"tempo"},
     {L"mínimo", L"esparso", L"equilibrado", L"denso", L"máximo"},
     {L"aurora", L"brasa", L"oceano", L"néon", L"floresta", L"monocromático", L"personalizada"},
     L"quantidade"},
    {L"fr-FR", false,
     {L"champ de flux", L"courbes de niveau", L"croissance différentielle", L"harmonographe"},
     {L"turbulence", L"relief", L"vigueur", L"tempo"},
     {L"minimal", L"épars", L"équilibré", L"dense", L"maximal"},
     {L"aurore", L"braise", L"océan", L"néon", L"forêt", L"monochrome", L"personnalisée"},
     L"quantité"},
    {L"de-DE", false,
     {L"Strömungsfeld", L"Höhenlinien", L"differenzielles Wachstum", L"Harmonograph"},
     {L"Turbulenz", L"Relief", L"Wuchskraft", L"Tempo"},
     {L"minimal", L"spärlich", L"ausgewogen", L"dicht", L"maximal"},
     {L"Aurora", L"Glut", L"Ozean", L"Neon", L"Wald", L"Monochrom", L"benutzerdefiniert"},
     L"Menge"},
    {L"it-IT", false,
     {L"campo di flusso", L"curve di livello", L"crescita differenziale", L"armonografo"},
     {L"turbolenza", L"rilievo", L"vigore", L"tempo"},
     {L"minimo", L"rado", L"equilibrato", L"denso", L"massimo"},
     {L"aurora", L"brace", L"oceano", L"neon", L"foresta", L"monocromatica", L"personalizzata"},
     L"quantità"},
    {L"nl-NL", false,
     {L"stromingsveld", L"hoogtelijnen", L"differentiële groei", L"harmonograaf"},
     {L"turbulentie", L"reliëf", L"groeikracht", L"tempo"},
     {L"minimaal", L"schaars", L"gebalanceerd", L"dicht", L"maximaal"},
     {L"aurora", L"gloed", L"oceaan", L"neon", L"bos", L"monochroom", L"aangepast"},
     L"hoeveelheid"},
    {L"pl-PL", false,
     {L"pole przepływu", L"poziomice", L"wzrost różnicowy", L"harmonograf"},
     {L"turbulencja", L"rzeźba", L"wigor", L"tempo"},
     {L"minimalna", L"rzadka", L"zrównoważona", L"gęsta", L"maksymalna"},
     {L"zorza", L"żar", L"ocean", L"neon", L"las", L"monochromatyczna", L"własna"},
     L"ilość"},
    {L"tr-TR", false,
     {L"akış alanı", L"eş yükselti eğrileri", L"diferansiyel büyüme", L"harmonograf"},
     {L"türbülans", L"kabartma", L"canlılık", L"tempo"},
     {L"en az", L"seyrek", L"dengeli", L"yoğun", L"en çok"},
     {L"kutup ışığı", L"kor", L"okyanus", L"neon", L"orman", L"tek renk", L"özel"},
     L"miktar"},
    {L"ru-RU", false,
     {L"поле потока", L"изолинии", L"дифференциальный рост", L"гармонограф"},
     {L"турбулентность", L"рельеф", L"энергия", L"темп"},
     {L"минимум", L"разреженно", L"сбалансированно", L"плотно", L"максимум"},
     {L"сияние", L"угли", L"океан", L"неон", L"лес", L"монохром", L"своя"},
     L"количество"},
    {L"uk-UA", false,
     {L"поле потоку", L"ізолінії", L"диференційний ріст", L"гармонограф"},
     {L"турбулентність", L"рельєф", L"енергія", L"темп"},
     {L"мінімум", L"розріджено", L"збалансовано", L"щільно", L"максимум"},
     {L"сяйво", L"жар", L"океан", L"неон", L"ліс", L"монохром", L"власна"},
     L"кількість"},
    {L"zh-CN", false,
     {L"流场", L"等高线", L"差异生长", L"谐波图"},
     {L"湍流", L"起伏", L"生长力", L"节奏"},
     {L"极简", L"稀疏", L"均衡", L"密集", L"极密"},
     {L"极光", L"余烬", L"海洋", L"霓虹", L"森林", L"单色", L"自定义"},
     L"数量"},
    {L"zh-TW", false,
     {L"流場", L"等高線", L"差異生長", L"諧波圖"},
     {L"湍流", L"起伏", L"生長力", L"節奏"},
     {L"極簡", L"稀疏", L"均衡", L"密集", L"極密"},
     {L"極光", L"餘燼", L"海洋", L"霓虹", L"森林", L"單色", L"自訂"},
     L"數量"},
    {L"ja-JP", false,
     {L"フローフィールド", L"等高線", L"微分成長", L"ハーモノグラフ"},
     {L"乱流", L"起伏", L"成長力", L"テンポ"},
     {L"最小", L"まばら", L"標準", L"密", L"最大"},
     {L"オーロラ", L"残り火", L"オーシャン", L"ネオン", L"フォレスト", L"モノクロ", L"カスタム"},
     L"量"},
    {L"ko-KR", false,
     {L"흐름장", L"등고선", L"미분 성장", L"하모노그래프"},
     {L"난류", L"기복", L"성장력", L"템포"},
     {L"최소", L"성김", L"균형", L"조밀", L"최대"},
     {L"오로라", L"잔불", L"오션", L"네온", L"포레스트", L"흑백", L"사용자 지정"},
     L"양"},
    {L"ar", true,
     {L"حقل التدفق", L"خطوط الكنتور", L"النمو التفاضلي", L"هارمونوغراف"},
     {L"اضطراب", L"تضاريس", L"حيوية", L"إيقاع"},
     {L"أدنى", L"متناثر", L"متوازن", L"كثيف", L"أقصى"},
     {L"شفق", L"جمر", L"محيط", L"نيون", L"غابة", L"أحادي اللون", L"مخصص"},
     L"الكمية"},
    {L"he", true,
     {L"שדה זרימה", L"קווי גובה", L"צמיחה דיפרנציאלית", L"הרמונוגרף"},
     {L"מערבולת", L"תבליט", L"מרץ", L"קצב"},
     {L"מינימלי", L"דליל", L"מאוזן", L"צפוף", L"מקסימלי"},
     {L"זוהר", L"גחלים", L"אוקיינוס", L"ניאון", L"יער", L"מונוכרום", L"מותאם"},
     L"כמות"},
};

static const Strings* g_strings = &kStrings[0];

static size_t PrimaryLen(const wchar_t* tag) {
    size_t n = 0;
    while (tag[n] && tag[n] != L'-') {
        n++;
    }
    return n;
}

static bool TagEquals(const wchar_t* a, const wchar_t* b) {
    while (*a && *b && towlower(*a) == towlower(*b)) {
        a++;
        b++;
    }
    return *a == *b;
}

// Does the tag carry this subtag, as in the "Hant" of "zh-Hant-HK".
static bool HasSubtag(const wchar_t* tag, const wchar_t* sub) {
    for (const wchar_t* p = tag; *p; p++) {
        if (p != tag && p[-1] != L'-') {
            continue;
        }
        size_t i = 0;
        while (sub[i] && towlower(p[i]) == towlower(sub[i])) {
            i++;
        }
        if (!sub[i] && (p[i] == 0 || p[i] == L'-')) {
            return true;
        }
    }
    return false;
}

static const Strings* FindStrings(const wchar_t* tag) {
    if (!tag || !*tag) {
        return nullptr;
    }
    for (size_t i = 0; i < ARRAYSIZE(kStrings); i++) {
        if (TagEquals(tag, kStrings[i].tag)) {
            return &kStrings[i];
        }
    }
    // Chinese needs the script and not just the language: Hong Kong and Macau
    // are written in traditional characters, the same as Taiwan.
    size_t primary = PrimaryLen(tag);
    if (primary == 2 && towlower(tag[0]) == L'z' && towlower(tag[1]) == L'h') {
        bool trad = HasSubtag(tag, L"Hant") || HasSubtag(tag, L"TW") ||
                    HasSubtag(tag, L"HK") || HasSubtag(tag, L"MO");
        return FindStrings(trad ? L"zh-TW" : L"zh-CN");
    }
    // Otherwise the language alone is enough: en-GB reads the English row and
    // pt-PT the Portuguese one.
    for (size_t i = 0; i < ARRAYSIZE(kStrings); i++) {
        const wchar_t* cand = kStrings[i].tag;
        if (PrimaryLen(cand) != primary) {
            continue;
        }
        size_t j = 0;
        while (j < primary && towlower(tag[j]) == towlower(cand[j])) {
            j++;
        }
        if (j == primary) {
            return &kStrings[i];
        }
    }
    return nullptr;
}

// The languages the user actually asked Windows for, in their order of
// preference, so someone running an English install with Russian second still
// gets Russian if English were ever removed from the table.
static const Strings* StringsFromSystem() {
    ULONG num = 0, chars = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num, nullptr, &chars) &&
        chars > 0) {
        std::vector<wchar_t> buf(chars);
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num, buf.data(),
                                        &chars)) {
            const wchar_t* p = buf.data();
            while (*p) {
                const Strings* hit = FindStrings(p);
                if (hit) {
                    return hit;
                }
                p += wcslen(p) + 1;
            }
        }
    }
    WCHAR name[LOCALE_NAME_MAX_LENGTH];
    if (LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT),
                         name, ARRAYSIZE(name), 0)) {
        return FindStrings(name);
    }
    return nullptr;
}

// Kept in preference to std::clamp, which returns a reference to one of its
// arguments, so the result must never be bound to one: with a temporary
// argument, const auto& r = std::clamp(x + 1, 0, 9) dangles. Assigning by
// value is perfectly safe there, so this is a preference rather than a fix;
// returning by value just removes the trap.
template <typename T>
static T ClampT(T v, T lo, T hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

template <typename T>
static void SafeRelease(T** pp) {
    if (*pp) {
        (*pp)->Release();
        *pp = nullptr;
    }
}

// Deterministic LCG, so each display gets its own scene without <random>.
struct Rng {
    unsigned s;
    explicit Rng(unsigned seed) : s(seed ? seed : 1u) {}
    float Next() {
        s = s * 1664525u + 1013904223u;
        return (float)(s >> 8) * (1.0f / 16777216.0f);
    }
    float Range(float a, float b) { return a + (b - a) * Next(); }
    int Int(int a, int b) {
        if (b <= a) {
            return a;
        }
        // Next() is strictly below 1, so the cast already lands in [0, b-a]
        // and the modulo this used to carry never had anything to do.
        return a + (int)(Next() * (float)(b - a + 1));
    }
    float Sign() { return Next() < 0.5f ? -1.0f : 1.0f; }
};

// ---------------------------------------------------------------------------
// Perlin noise (Ken Perlin's improved noise), identical to the prototype
// ---------------------------------------------------------------------------
struct Noise {
    unsigned char p[512];

    void Seed(unsigned seed) {
        unsigned char t[256];
        for (int i = 0; i < 256; i++) {
            t[i] = (unsigned char)i;
        }
        Rng r(seed);
        for (int i = 255; i > 0; i--) {
            int j = r.Int(0, i);
            unsigned char tmp = t[i];
            t[i] = t[j];
            t[j] = tmp;
        }
        for (int i = 0; i < 512; i++) {
            p[i] = t[i & 255];
        }
    }

    static float Fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    static float Lerp(float t, float a, float b) { return a + t * (b - a); }

    static float Grad(int h, float x, float y, float z) {
        switch (h & 15) {
            case 0: return x + y;
            case 1: return -x + y;
            case 2: return x - y;
            case 3: return -x - y;
            case 4: return x + z;
            case 5: return -x + z;
            case 6: return x - z;
            case 7: return -x - z;
            case 8: return y + z;
            case 9: return -y + z;
            case 10: return y - z;
            case 11: return -y - z;
            case 12: return y + x;
            case 13: return -y + z;
            case 14: return y - x;
            default: return -y - z;
        }
    }

    float N3(float x, float y, float z) const {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;
        int Z = (int)std::floor(z) & 255;
        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);
        float u = Fade(x), v = Fade(y), w = Fade(z);
        int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
        int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
        return Lerp(w,
                    Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
                                 Grad(p[BA], x - 1, y, z)),
                         Lerp(u, Grad(p[AB], x, y - 1, z),
                              Grad(p[BB], x - 1, y - 1, z))),
                    Lerp(v,
                         Lerp(u, Grad(p[AA + 1], x, y, z - 1),
                              Grad(p[BA + 1], x - 1, y, z - 1)),
                         Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
                              Grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }

    float Fbm(float x, float y, float z, int oct) const {
        float a = 0.5f, f = 1.0f, sum = 0.0f, norm = 0.0f;
        for (int i = 0; i < oct; i++) {
            sum += a * N3(x * f, y * f, z * f);
            norm += a;
            a *= 0.5f;
            f *= 2.0f;
        }
        return norm > 0 ? sum / norm : 0.0f;
    }
};

// ---------------------------------------------------------------------------
// Color
// ---------------------------------------------------------------------------
struct Rgb {
    float r, g, b;
};

static Rgb RgbFromHex(unsigned v) {
    Rgb c;
    c.r = (float)((v >> 16) & 0xFF) / 255.0f;
    c.g = (float)((v >> 8) & 0xFF) / 255.0f;
    c.b = (float)(v & 0xFF) / 255.0f;
    return c;
}

// Hue rotation applied at stroke time. Direct2D 1.0 has no hue-rotate effect,
// and doing it per stroke is the better look anyway: styles that redraw every
// frame slide through color wholesale, while styles that accumulate lay a
// gradient through the artwork as the hue drifts.
static Rgb ShiftHue(const Rgb& in, float deg) {
    if (deg == 0.0f) {
        return in;
    }
    float mx = std::max(in.r, std::max(in.g, in.b));
    float mn = std::min(in.r, std::min(in.g, in.b));
    float l = (mx + mn) * 0.5f;
    float h = 0.0f, s = 0.0f;
    if (mx != mn) {
        float d = mx - mn;
        s = l > 0.5f ? d / (2.0f - mx - mn) : d / (mx + mn);
        if (mx == in.r) {
            h = (in.g - in.b) / d + (in.g < in.b ? 6.0f : 0.0f);
        } else if (mx == in.g) {
            h = (in.b - in.r) / d + 2.0f;
        } else {
            h = (in.r - in.g) / d + 4.0f;
        }
        h /= 6.0f;
    }
    h += deg / 360.0f;
    h -= std::floor(h);

    Rgb out;
    if (s == 0.0f) {
        out.r = out.g = out.b = l;
        return out;
    }
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    float ch[3] = {h + 1.0f / 3.0f, h, h - 1.0f / 3.0f};
    float res[3];
    for (int i = 0; i < 3; i++) {
        float t = ch[i];
        if (t < 0) t += 1.0f;
        if (t > 1) t -= 1.0f;
        if (t < 1.0f / 6.0f) {
            res[i] = p + (q - p) * 6.0f * t;
        } else if (t < 0.5f) {
            res[i] = q;
        } else if (t < 2.0f / 3.0f) {
            res[i] = p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        } else {
            res[i] = p;
        }
    }
    out.r = res[0];
    out.g = res[1];
    out.b = res[2];
    return out;
}

struct Palette {
    Rgb bg;
    std::vector<Rgb> ink;
};

static D2D1_COLOR_F ToColorF(const Rgb& c, float a) {
    D2D1_COLOR_F o;
    o.r = c.r;
    o.g = c.g;
    o.b = c.b;
    o.a = a;
    return o;
}

static D2D1_POINT_2F Pt(float x, float y) {
    D2D1_POINT_2F p;
    p.x = x;
    p.y = y;
    return p;
}

// ---------------------------------------------------------------------------
// Direct2D geometry helpers. Struct parameters are always passed by pointer
// and points by value, which is the intersection of the MSVC and mingw-w64
// declarations of these interfaces.
// ---------------------------------------------------------------------------
static ID2D1PathGeometry* MakePolyline(ID2D1Factory* factory,
                                       const D2D1_POINT_2F* pts,
                                       UINT32 n,
                                       bool closed) {
    if (!factory || n < 2) {
        return nullptr;
    }
    ID2D1PathGeometry* geom = nullptr;
    if (FAILED(factory->CreatePathGeometry(&geom)) || !geom) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geom->Open(&sink)) || !sink) {
        geom->Release();
        return nullptr;
    }
    sink->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_HOLLOW);
    for (UINT32 i = 1; i < n; i++) {
        sink->AddLine(pts[i]);
    }
    sink->EndFigure(closed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
    sink->Close();
    sink->Release();
    return geom;
}

// One geometry holding many disconnected two-point figures. Used by the
// contour renderer so a whole iso level is a single DrawGeometry call.
static ID2D1PathGeometry* MakeSegments(ID2D1Factory* factory,
                                       const std::vector<D2D1_POINT_2F>& pts) {
    if (!factory || pts.size() < 2) {
        return nullptr;
    }
    ID2D1PathGeometry* geom = nullptr;
    if (FAILED(factory->CreatePathGeometry(&geom)) || !geom) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geom->Open(&sink)) || !sink) {
        geom->Release();
        return nullptr;
    }
    for (size_t i = 0; i + 1 < pts.size(); i += 2) {
        sink->BeginFigure(pts[i], D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddLine(pts[i + 1]);
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
    }
    sink->Close();
    sink->Release();
    return geom;
}

// ---------------------------------------------------------------------------
// Scene interface
// ---------------------------------------------------------------------------
struct SceneCtx {
    ID2D1Factory* factory = nullptr;
    ID2D1RenderTarget* target = nullptr;      // accumulation buffer
    ID2D1SolidColorBrush* brush = nullptr;
    float hue = 0;
    float param = 0.5f;                       // 0..1, the wheel
    int amount = 2;                           // 0..4, right click
    float dt = 1.0f / 60.0f;                  // seconds since the last frame
};

// Converts a per-second rate into whole steps, carrying the remainder across
// frames so the simulation advances at the same speed at any frame rate.
class StepClock {
   public:
    int Take(float ratePerSec, float dt) {
        acc_ += ratePerSec * dt;
        if (acc_ > 240.0f) {
            acc_ = 240.0f;   // never spiral after a stall
        }
        int n = (int)acc_;
        acc_ -= (float)n;
        return n;
    }

   private:
    float acc_ = 0;
};

class Scene {
   public:
    virtual ~Scene() {}
    // Advance the simulation and paint into ctx.target. Returns true when the
    // composition is finished and should be held.
    virtual bool Step(SceneCtx& ctx) = 0;
    // Optional bright overlay drawn straight to the window each frame.
    virtual void PaintCrisp(SceneCtx& ctx, ID2D1RenderTarget* rt) {
        (void)ctx;
        (void)rt;
    }
    // When true the artwork keeps animating through the hold phase. When
    // false the finished composition fades out immediately instead of
    // freezing, so the screen is never a static picture.
    virtual bool Continuous() const { return false; }
};

static void SetInk(SceneCtx& ctx, const Rgb& base, float alpha) {
    Rgb c = ShiftHue(base, ctx.hue);
    D2D1_COLOR_F col = ToColorF(c, alpha);
    ctx.brush->SetColor(&col);
}

// ---------------------------------------------------------------------------
// Style 1: flow field ribbons
//
// Curves are traced up front against a packing grid; a curve that dies before
// a minimum length has the cells it claimed rolled back and is discarded, so
// the field never fills with short stubs. Accepted curves are then drawn in
// progressively as parallel ribbons of fine lines.
// ---------------------------------------------------------------------------
class FlowScene : public Scene {
   public:
    FlowScene(float w, float h, unsigned seed, const Palette& pal, int amount)
        : rng_(seed) {
        noise_.Seed(seed);
        w_ = w;
        h_ = h;
        diag_ = std::sqrt(w * w + h * h);

        static const float kSepMul[kAmountCount] = {2.20f, 1.50f, 1.00f, 0.72f,
                                                    0.54f};
        int amt = ClampT(amount, 0, kAmountCount - 1);

        scale_ = rng_.Range(0.0016f, 0.0032f);
        zt_ = rng_.Range(0.0f, 100.0f);
        turns_ = 2.0f;
        sep_ = diag_ * rng_.Range(0.010f, 0.021f) * kSepMul[amt];
        step_ = std::max(2.0f, diag_ * 0.0035f);
        minSteps_ = 32;
        cell_ = sep_ * 0.5f;

        gw_ = (int)(w_ / cell_) + 3;
        gh_ = (int)(h_ / cell_) + 3;
        grid_.assign((size_t)gw_ * gh_, -1);

        // Seed lattice scales with the packing density, so the number of
        // candidate start points is right for any display size.
        int nx = ClampT((int)(w_ / (sep_ * 0.8f)), 16, 200);
        int ny = ClampT((int)(h_ / (sep_ * 0.8f)), 16, 200);
        seeds_.reserve((size_t)nx * ny);
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                D2D1_POINT_2F p =
                    Pt(((float)i + rng_.Range(0.1f, 0.9f)) / nx * w_,
                       ((float)j + rng_.Range(0.1f, 0.9f)) / ny * h_);
                seeds_.push_back(p);
            }
        }
        for (size_t i = seeds_.size(); i > 1; i--) {
            size_t j = (size_t)rng_.Int(0, (int)i - 1);
            std::swap(seeds_[i - 1], seeds_[j]);
        }
        maxCurves_ = ClampT((int)(seeds_.size() / 3), 40, 1200);
        pal_ = &pal;
    }

    bool Step(SceneCtx& ctx) override {
        turns_ = 1.0f + ctx.param * 3.5f;
        // the 30x keeps the wall-clock pacing identical to the prototype
        int steps = clock_.Take((1.0f + ctx.param * 4.0f) * 30.0f, ctx.dt);
        for (int i = 0; i < steps; i++) {
            AdvanceAll(ctx);
        }
        return done_;
    }

   private:
    struct Curve {
        std::vector<D2D1_POINT_2F> spine;
        size_t at = 1;
        int nLines = 3;
        float width = 4;
        float lw = 1;
        float alpha = 0.9f;
        Rgb color;
        std::vector<D2D1_POINT_2F> prev;
        bool hasPrev = false;
    };

    float AngleAt(float x, float y) const {
        return noise_.Fbm(x * scale_, y * scale_, zt_, 3) * kPi * 2.0f * turns_;
    }

    bool Markable(float x, float y, int id) const {
        int cx = (int)(x / cell_) + 1;
        int cy = (int)(y / cell_) + 1;
        if (cx < 1 || cy < 1 || cx >= gw_ - 1 || cy >= gh_ - 1) {
            return false;
        }
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                int v = grid_[(size_t)(cy + j) * gw_ + (cx + i)];
                if (v != -1 && v != id) {
                    return false;
                }
            }
        }
        return true;
    }

    bool Spawn() {
        while (spawnCursor_ < seeds_.size()) {
            D2D1_POINT_2F s = seeds_[spawnCursor_++];
            int id = (int)curves_.size();
            if (!Markable(s.x, s.y, id)) {
                continue;
            }
            float dir = rng_.Sign();
            int maxLife = rng_.Int(260, 900);

            std::vector<D2D1_POINT_2F> spine;
            std::vector<int> claimed;
            spine.push_back(s);
            float x = s.x, y = s.y;
            for (int i = 0; i < maxLife; i++) {
                float a = AngleAt(x, y);
                float nx = x + std::cos(a) * step_ * dir;
                float ny = y + std::sin(a) * step_ * dir;
                if (!Markable(nx, ny, id)) {
                    break;
                }
                int cx = (int)(nx / cell_) + 1;
                int cy = (int)(ny / cell_) + 1;
                int idx = cy * gw_ + cx;
                if (grid_[idx] == -1) {
                    claimed.push_back(idx);
                }
                grid_[idx] = id;
                spine.push_back(Pt(nx, ny));
                x = nx;
                y = ny;
            }
            if ((int)spine.size() < minSteps_) {
                for (size_t i = 0; i < claimed.size(); i++) {
                    grid_[claimed[i]] = -1;   // roll back, try another seed
                }
                continue;
            }

            Curve c;
            c.spine.swap(spine);
            c.nLines = rng_.Int(2, 4);
            c.width = sep_ * rng_.Range(0.55f, 0.92f);
            c.lw = rng_.Range(0.75f, 1.25f);
            c.alpha = rng_.Range(0.72f, 1.0f);
            c.color = pal_->ink[(size_t)rng_.Int(0, (int)pal_->ink.size() - 1)];
            curves_.push_back(c);
            active_.push_back((int)curves_.size() - 1);
            return true;
        }
        return false;
    }

    void AdvanceAll(SceneCtx& ctx) {
        while ((int)active_.size() < 26 && (int)curves_.size() < maxCurves_) {
            if (!Spawn()) {
                break;
            }
        }
        if (active_.empty()) {
            done_ = true;
            return;
        }
        for (int k = (int)active_.size() - 1; k >= 0; k--) {
            Curve& c = curves_[active_[k]];
            if (c.at >= c.spine.size()) {
                active_.erase(active_.begin() + k);
                continue;
            }
            D2D1_POINT_2F n = c.spine[c.at];
            D2D1_POINT_2F p = c.spine[c.at - 1];
            float dx = n.x - p.x, dy = n.y - p.y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len < 1e-4f) {
                len = 1.0f;
            }
            float ox = -dy / len, oy = dx / len;
            float taper =
                std::sin(((float)c.at / (float)c.spine.size()) * kPi);
            float halfW = c.width * (0.35f + 0.65f * taper);

            std::vector<D2D1_POINT_2F> pts;
            pts.reserve(c.nLines);
            for (int i = 0; i < c.nLines; i++) {
                float t = c.nLines == 1
                              ? 0.0f
                              : ((float)i / (c.nLines - 1) - 0.5f) * 2.0f;
                pts.push_back(Pt(n.x + ox * halfW * t, n.y + oy * halfW * t));
            }

            if (c.hasPrev) {
                SetInk(ctx, c.color, c.alpha);
                for (size_t i = 0; i < pts.size() && i < c.prev.size(); i++) {
                    ctx.target->DrawLine(c.prev[i], pts[i], ctx.brush, c.lw,
                                         nullptr);
                }
            }
            c.prev = pts;
            c.hasPrev = true;
            c.at++;
        }
    }

    Rng rng_;
    Noise noise_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0, diag_ = 0;
    float scale_ = 0, zt_ = 0, turns_ = 2, sep_ = 10, step_ = 3, cell_ = 5;
    int minSteps_ = 32;
    int gw_ = 0, gh_ = 0;
    int maxCurves_ = 400;
    std::vector<int> grid_;
    std::vector<D2D1_POINT_2F> seeds_;
    size_t spawnCursor_ = 0;
    std::vector<Curve> curves_;
    std::vector<int> active_;
    StepClock clock_;
    bool done_ = false;
};

// ---------------------------------------------------------------------------
// Style 2: topographic contours (marching squares over a warped fbm field)
//
// The sampling grid is area budgeted rather than tied to pixel count, so the
// per-frame cost is the same on a 1080p panel and a 4K one; only the cell size
// changes. The field is resampled every third frame because it drifts slowly.
// ---------------------------------------------------------------------------
class ContourScene : public Scene {
   public:
    static int LevelsFor(int amount) {
        static const int kLevels[kAmountCount] = {6, 12, 21, 32, 46};
        return kLevels[ClampT(amount, 0, kAmountCount - 1)];
    }

    ContourScene(float w, float h, unsigned seed, const Palette& pal, int amount)
        : rng_(seed ^ 0x9e37u) {
        noise_.Seed(seed ^ 0x9e37u);
        w_ = w;
        h_ = h;
        pal_ = &pal;

        // Fixed sample budget -> resolution independent cost.
        const float kBudget = 18000.0f;
        float aspect = (h > 0) ? (w / h) : 1.0f;
        cols_ = ClampT((int)std::sqrt(kBudget * aspect), 40, 260);
        rows_ = ClampT((int)(kBudget / std::max(1, cols_)), 30, 200);
        cw_ = w_ / cols_;
        ch_ = h_ / rows_;
        field_.assign((size_t)(cols_ + 1) * (rows_ + 1), 0.0f);

        levels_ = LevelsFor(amount);
        scale_ = rng_.Range(2.2f, 3.6f);
        z_ = rng_.Range(0.0f, 50.0f);
        warp_ = 0.3f;

        fieldA_.assign(field_.size(), 0.0f);
        fieldB_.assign(field_.size(), 0.0f);
        zA_ = z_;
        zB_ = z_ + kDz;
        SampleInto(fieldA_, zA_);
        SampleInto(fieldB_, zB_);
        blend_ = 0.0f;
    }

    bool Step(SceneCtx& ctx) override {
        warp_ = 0.10f + ctx.param * 0.80f;
        levels_ = LevelsFor(ctx.amount);

        Advance(ctx.dt);
        age_ += ctx.dt;
        MarchAll();

        // Contours are a full redraw each frame.
        D2D1_COLOR_F clear = ToColorF(pal_->bg, 0.0f);
        ctx.target->Clear(&clear);

        const size_t inkN = pal_->ink.size();
        for (int k = 0; k < levels_; k++) {
            float t = levels_ > 1 ? (float)k / (levels_ - 1) : 0.0f;
            bool emph = (k % 5 == 0);

            // blend across the palette by depth so the map reads as one system
            float fi = t * (float)(inkN - 1);
            int i0 = ClampT((int)fi, 0, (int)inkN - 1);
            int i1 = ClampT(i0 + 1, 0, (int)inkN - 1);
            float ft = fi - i0;
            Rgb c;
            c.r = pal_->ink[i0].r + (pal_->ink[i1].r - pal_->ink[i0].r) * ft;
            c.g = pal_->ink[i0].g + (pal_->ink[i1].g - pal_->ink[i0].g) * ft;
            c.b = pal_->ink[i0].b + (pal_->ink[i1].b - pal_->ink[i0].b) * ft;

            const std::vector<D2D1_POINT_2F>& segs = segsByLevel_[k];
            if (segs.empty()) {
                continue;
            }
            SetInk(ctx, c, emph ? 0.95f : 0.38f);
            ID2D1PathGeometry* g = MakeSegments(ctx.factory, segs);
            if (g) {
                ctx.target->DrawGeometry(g, ctx.brush, emph ? 1.15f : 0.6f,
                                         nullptr);
                g->Release();
            }
        }
        return age_ > kLifeSecs;
    }

    bool Continuous() const override { return true; }

   private:
    void SampleInto(std::vector<float>& dst, float z) {
        float s = scale_;
        for (int j = 0; j <= rows_; j++) {
            float v = (float)j / rows_;
            for (int i = 0; i <= cols_; i++) {
                float u = (float)i / cols_;
                float wx = noise_.Fbm(u * s + 5.2f, v * s + 1.3f, z, 2) * warp_;
                float wy = noise_.Fbm(u * s + 9.7f, v * s + 4.1f, z, 2) * warp_;
                dst[(size_t)j * (cols_ + 1) + i] =
                    noise_.Fbm(u * s + wx, v * s + wy, z, 4);
            }
        }
    }

    // Blend between the two sampled fields every frame, rolling forward when
    // the blend completes. Noise is evaluated twice a second; the contours
    // themselves move continuously.
    void Advance(float dt) {
        blend_ += dt / kCycleSecs;
        while (blend_ >= 1.0f) {
            blend_ -= 1.0f;
            fieldA_.swap(fieldB_);
            zA_ = zB_;
            zB_ = zA_ + kDz;
            SampleInto(fieldB_, zB_);
        }
        float t = ClampT(blend_, 0.0f, 1.0f);
        for (size_t i = 0; i < field_.size(); i++) {
            field_[i] = fieldA_[i] + (fieldB_[i] - fieldA_[i]) * t;
        }
    }

    float At(int i, int j) const {
        return field_[(size_t)j * (cols_ + 1) + i];
    }

    static void Emit(std::vector<D2D1_POINT_2F>& out,
                     float ax, float ay, float bx, float by) {
        out.push_back(Pt(ax, ay));
        out.push_back(Pt(bx, by));
    }

    // One pass over the grid for every level at once.
    //
    // The levels are evenly spaced, so a cell can only cross the ones that
    // fall between the minimum and maximum of its four corners. Bracketing
    // against that leaves most cells touching one or two levels instead of
    // being re-tested against all of them, which at the maximal setting is
    // the difference between about 830k cell tests per frame and roughly one
    // pass over the grid. The output is identical.
    void MarchAll() {
        if ((int)segsByLevel_.size() != levels_) {
            // Not just grow: stepping the amount back down used to leave this
            // at the largest it had ever been for the life of the scene.
            segsByLevel_.resize(levels_);
        }
        for (int k = 0; k < levels_; k++) {
            segsByLevel_[k].clear();
        }
        if (levels_ < 2) {
            return;
        }

        const float kBase = -0.42f, kSpan = 0.84f;
        const float step = kSpan / (levels_ - 1);

        for (int j = 0; j < rows_; j++) {
            for (int i = 0; i < cols_; i++) {
                float v0 = At(i, j), v1 = At(i + 1, j);
                float v2 = At(i + 1, j + 1), v3 = At(i, j + 1);

                float vmin = std::min(std::min(v0, v1), std::min(v2, v3));
                float vmax = std::max(std::max(v0, v1), std::max(v2, v3));

                // Widen by one notch each way: the level is recomputed below
                // the same way the color ramp does it, and that expression
                // can land a hair either side of base + k * step. A spare
                // level costs one rejected code test; a missed one would drop
                // a segment.
                int kFrom = (int)std::floor((vmin - kBase) / step) - 1;
                int kTo = (int)std::ceil((vmax - kBase) / step) + 1;
                if (kFrom < 0) {
                    kFrom = 0;
                }
                if (kTo > levels_ - 1) {
                    kTo = levels_ - 1;
                }
                if (kFrom > kTo) {
                    continue;
                }

                float x0 = i * cw_, y0 = j * ch_;
                float x1 = x0 + cw_, y1 = y0 + ch_;

                for (int k = kFrom; k <= kTo; k++) {
                    float level =
                        kBase + ((float)k / (levels_ - 1)) * kSpan;
                    int code = 0;
                    if (v0 > level) code |= 1;
                    if (v1 > level) code |= 2;
                    if (v2 > level) code |= 4;
                    if (v3 > level) code |= 8;
                    if (code == 0 || code == 15) {
                        continue;
                    }
                    std::vector<D2D1_POINT_2F>& out = segsByLevel_[k];
                    // linear interpolation along each crossed edge
                    float dT = (v1 - v0) != 0 ? (v1 - v0) : 1e-6f;
                    float dR = (v2 - v1) != 0 ? (v2 - v1) : 1e-6f;
                    float dB = (v2 - v3) != 0 ? (v2 - v3) : 1e-6f;
                    float dL = (v3 - v0) != 0 ? (v3 - v0) : 1e-6f;
                    float Tx = x0 + (x1 - x0) * ((level - v0) / dT), Ty = y0;
                    float Rx = x1, Ry = y0 + (y1 - y0) * ((level - v1) / dR);
                    float Bx = x0 + (x1 - x0) * ((level - v3) / dB), By = y1;
                    float Lx = x0, Ly = y0 + (y1 - y0) * ((level - v0) / dL);
                    switch (code) {
                        case 1: case 14: Emit(out, Lx, Ly, Tx, Ty); break;
                        case 2: case 13: Emit(out, Tx, Ty, Rx, Ry); break;
                        case 3: case 12: Emit(out, Lx, Ly, Rx, Ry); break;
                        case 4: case 11: Emit(out, Rx, Ry, Bx, By); break;
                        case 6: case 9:  Emit(out, Tx, Ty, Bx, By); break;
                        case 7: case 8:  Emit(out, Lx, Ly, Bx, By); break;
                        case 5:
                            Emit(out, Lx, Ly, Tx, Ty);
                            Emit(out, Rx, Ry, Bx, By);
                            break;
                        case 10:
                            Emit(out, Tx, Ty, Rx, Ry);
                            Emit(out, Lx, Ly, Bx, By);
                            break;
                        default: break;
                    }
                }
            }
        }
    }

    Rng rng_;
    Noise noise_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0, cw_ = 1, ch_ = 1;
    int cols_ = 0, rows_ = 0, levels_ = 21;
    float scale_ = 3, z_ = 0, warp_ = 0.3f;
    float age_ = 0;
    static constexpr float kLifeSecs = 300.0f;   // wall clock, not frames
    static constexpr float kCycleSecs = 0.5f;
    static constexpr float kDz = 0.015f;
    float zA_ = 0, zB_ = 0, blend_ = 0;
    std::vector<float> field_, fieldA_, fieldB_;
    std::vector<std::vector<D2D1_POINT_2F>> segsByLevel_;
};

// ---------------------------------------------------------------------------
// Style 3: differential growth
//
// Closed loops of nodes that attract along the curve and repel through a
// shared spatial hash, subdividing as they stretch. Every step is stamped into
// the buffer at low alpha so the history of the growth accumulates, with the
// live outline stroked bright on top.
// ---------------------------------------------------------------------------
class GrowthScene : public Scene {
   public:
    static int LoopsFor(int amount) {
        static const int kLoops[kAmountCount] = {1, 2, 3, 4, 6};
        return kLoops[ClampT(amount, 0, kAmountCount - 1)];
    }

    GrowthScene(float w, float h, unsigned seed, const Palette& pal, int amount)
        : rng_(seed ^ 0x51edu) {
        noise_.Seed(seed ^ 0x51edu);
        w_ = w;
        h_ = h;
        pal_ = &pal;
        float m = std::min(w, h);

        maxLen_ = m * 0.0085f;
        baseRepel_ = maxLen_ * 2.4f;
        repel_ = baseRepel_;
        // Step scales repel_ up to baseRepel_ * 1.9, and a 3x3 bucket search
        // only reaches cellSize_. Sizing for the smaller figure meant that
        // above about three quarters of the wheel some neighbours inside the
        // radius fell in non-adjacent cells and were skipped, so "vigor"
        // stopped responding and the form spaced unevenly rather than simply
        // less. The bucket count barely changes.
        cellSize_ = baseRepel_ * 1.9f * 1.05f;
        zt_ = rng_.Range(0.0f, 40.0f);

        int nLoops = LoopsFor(amount);
        // Budget per colony, so one colony is a small clean form rather than
        // spending the whole budget in one place and saturating.
        maxNodes_ = std::min(7000, 1500 * nLoops);

        for (int k = 0; k < nLoops; k++) {
            float cx = 0, cy = 0;
            float minSep = m * (0.62f / std::max(1, nLoops - 1));
            for (int tries = 0; tries < 40; tries++) {
                cx = rng_.Range(w_ * 0.16f, w_ * 0.84f);
                cy = rng_.Range(h_ * 0.18f, h_ * 0.82f);
                bool ok = true;
                for (size_t i = 0; i < loops_.size(); i++) {
                    float dx = cx - loops_[i].cx, dy = cy - loops_[i].cy;
                    if (std::sqrt(dx * dx + dy * dy) < minSep) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    break;
                }
            }
            float rad = m * rng_.Range(0.045f, 0.075f);
            int n = std::max(24, (int)std::ceil(2 * kPi * rad /
                                                (maxLen_ * 1.35f)));
            Loop L;
            L.cx = cx;
            L.cy = cy;
            L.color = pal_->ink[(size_t)rng_.Int(0, (int)pal_->ink.size() - 1)];
            L.lw = rng_.Range(0.5f, 0.9f);
            L.nodes.resize(n);
            for (int i = 0; i < n; i++) {
                float a = (float)i / n * 2 * kPi;
                L.nodes[i].x = cx + std::cos(a) * rad;
                L.nodes[i].y = cy + std::sin(a) * rad;
                L.nodes[i].vx = 0;
                L.nodes[i].vy = 0;
            }
            loops_.push_back(L);
        }
        gw_ = std::max(1, (int)(w_ / cellSize_) + 2);
        gh_ = std::max(1, (int)(h_ / cellSize_) + 2);
    }

    bool Step(SceneCtx& ctx) override {
        repel_ = baseRepel_ * (0.70f + ctx.param * 1.20f);
        int steps = clock_.Take((1.0f + ctx.param * 4.0f) * 30.0f, ctx.dt);
        for (int i = 0; i < steps; i++) {
            Simulate();
        }
        // Stamp the trail on the step budget rather than once per frame, so
        // the accumulated density is the same at any frame rate.
        if (steps > 0) {
            Paint(ctx, ctx.target, 0.05f * (float)steps / 5.0f);
        }
        return Total() >= maxNodes_;
    }

    void PaintCrisp(SceneCtx& ctx, ID2D1RenderTarget* rt) override {
        Paint(ctx, rt, 0.85f);
    }

    bool Continuous() const override { return true; }

   private:
    struct Node {
        float x, y, vx, vy;
    };
    struct Loop {
        std::vector<Node> nodes;
        float cx = 0, cy = 0, lw = 0.7f;
        Rgb color;
    };

    int Total() const {
        int t = 0;
        for (size_t i = 0; i < loops_.size(); i++) {
            t += (int)loops_[i].nodes.size();
        }
        return t;
    }

    void Simulate() {
        // Rebuild a shared bucket grid so loops repel each other as well as
        // themselves. Linked-list buckets avoid per-step allocation.
        int total = Total();
        flat_.clear();
        flat_.reserve(total);
        for (size_t li = 0; li < loops_.size(); li++) {
            for (size_t ni = 0; ni < loops_[li].nodes.size(); ni++) {
                flat_.push_back(&loops_[li].nodes[ni]);
            }
        }
        head_.assign((size_t)gw_ * gh_, -1);
        next_.assign(flat_.size(), -1);
        for (size_t i = 0; i < flat_.size(); i++) {
            int cx = ClampT((int)(flat_[i]->x / cellSize_), 0, gw_ - 1);
            int cy = ClampT((int)(flat_[i]->y / cellSize_), 0, gh_ - 1);
            int c = cy * gw_ + cx;
            next_[i] = head_[c];
            head_[c] = (int)i;
        }

        float r2 = repel_ * repel_;
        for (size_t li = 0; li < loops_.size(); li++) {
            std::vector<Node>& N = loops_[li].nodes;
            int n = (int)N.size();
            if (n < 3) {
                continue;
            }
            for (int i = 0; i < n; i++) {
                Node& a = N[i];
                const Node& prev = N[(i - 1 + n) % n];
                const Node& nx = N[(i + 1) % n];
                float fx = (prev.x + nx.x - 2 * a.x) * 0.20f;
                float fy = (prev.y + nx.y - 2 * a.y) * 0.20f;

                int bx = ClampT((int)(a.x / cellSize_), 0, gw_ - 1);
                int by = ClampT((int)(a.y / cellSize_), 0, gh_ - 1);
                for (int dy = -1; dy <= 1; dy++) {
                    int yy = by + dy;
                    if (yy < 0 || yy >= gh_) {
                        continue;
                    }
                    for (int dx = -1; dx <= 1; dx++) {
                        int xx = bx + dx;
                        if (xx < 0 || xx >= gw_) {
                            continue;
                        }
                        for (int idx = head_[(size_t)yy * gw_ + xx]; idx != -1;
                             idx = next_[idx]) {
                            const Node* o = flat_[idx];
                            if (o == &a) {
                                continue;
                            }
                            float ox = a.x - o->x, oy = a.y - o->y;
                            float d2 = ox * ox + oy * oy;
                            if (d2 < 1e-6f || d2 > r2) {
                                continue;
                            }
                            float d = std::sqrt(d2);
                            float f = (1.0f - d / repel_) * 1.35f;
                            fx += ox / d * f;
                            fy += oy / d * f;
                        }
                    }
                }
                float nz = noise_.Fbm(a.x * 0.0045f, a.y * 0.0045f, zt_, 2);
                fx += std::cos(nz * 14.0f) * 0.16f;
                fy += std::sin(nz * 14.0f) * 0.16f;
                a.vx = (a.vx + fx) * 0.60f;
                a.vy = (a.vy + fy) * 0.60f;
            }
            for (int i = 0; i < n; i++) {
                N[i].x = ClampT(N[i].x + N[i].vx, 6.0f, w_ - 6.0f);
                N[i].y = ClampT(N[i].y + N[i].vy, 6.0f, h_ - 6.0f);
            }
        }

        // total, from the top of this step: nothing between there and here
        // adds or removes a node, and Total() is a walk of every loop.
        if (total < maxNodes_) {
            int grew = 0;
            for (size_t li = 0; li < loops_.size(); li++) {
                std::vector<Node>& N = loops_[li].nodes;
                for (int i = (int)N.size() - 1; i >= 0; i--) {
                    const Node& a = N[i];
                    const Node& b = N[(i + 1) % (int)N.size()];
                    float dx = b.x - a.x, dy = b.y - a.y;
                    if (std::sqrt(dx * dx + dy * dy) > maxLen_) {
                        Node m;
                        m.x = (a.x + b.x) * 0.5f;
                        m.y = (a.y + b.y) * 0.5f;
                        m.vx = 0;
                        m.vy = 0;
                        N.insert(N.begin() + i + 1, m);
                        grew++;
                    }
                }
            }
            // Never let a colony sit at equilibrium and stall: force the
            // longest segment open if nothing subdivided this step.
            if (grew == 0) {
                for (size_t li = 0; li < loops_.size(); li++) {
                    std::vector<Node>& N = loops_[li].nodes;
                    if (N.size() < 3) {
                        continue;
                    }
                    int best = -1;
                    float bestD = -1;
                    for (int i = 0; i < (int)N.size(); i++) {
                        const Node& a = N[i];
                        const Node& b = N[(i + 1) % (int)N.size()];
                        float dx = b.x - a.x, dy = b.y - a.y;
                        float d = std::sqrt(dx * dx + dy * dy);
                        if (d > bestD) {
                            bestD = d;
                            best = i;
                        }
                    }
                    if (best >= 0) {
                        const Node& a = N[best];
                        const Node& b = N[(best + 1) % (int)N.size()];
                        Node m;
                        m.x = (a.x + b.x) * 0.5f;
                        m.y = (a.y + b.y) * 0.5f;
                        m.vx = 0;
                        m.vy = 0;
                        N.insert(N.begin() + best + 1, m);
                    }
                }
            }
        }
        zt_ += 0.0025f;
    }

    void Paint(SceneCtx& ctx, ID2D1RenderTarget* rt, float alpha) {
        for (size_t li = 0; li < loops_.size(); li++) {
            const std::vector<Node>& N = loops_[li].nodes;
            if (N.size() < 3) {
                continue;
            }
            pts_.clear();
            pts_.reserve(N.size());
            for (size_t i = 0; i < N.size(); i++) {
                pts_.push_back(Pt(N[i].x, N[i].y));
            }
            SetInk(ctx, loops_[li].color, alpha);
            ID2D1PathGeometry* g =
                MakePolyline(ctx.factory, pts_.data(), (UINT32)pts_.size(), true);
            if (g) {
                rt->DrawGeometry(g, ctx.brush, loops_[li].lw, nullptr);
                g->Release();
            }
        }
    }

    Rng rng_;
    Noise noise_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0;
    float maxLen_ = 4, repel_ = 8, baseRepel_ = 8, cellSize_ = 12, zt_ = 0;
    int maxNodes_ = 4500;
    int gw_ = 1, gh_ = 1;
    std::vector<Loop> loops_;
    std::vector<Node*> flat_;
    std::vector<int> head_, next_;
    std::vector<D2D1_POINT_2F> pts_;
    StepClock clock_;
};

// ---------------------------------------------------------------------------
// Style 4: harmonograph
//
// Two decaying pendulums per axis. Integer frequency ratios read as deliberate
// figures; a single shared decay per figure makes it spiral inward
// self-similarly instead of collapsing to a flat lens.
// ---------------------------------------------------------------------------
class HarmonographScene : public Scene {
   public:
    static int FiguresFor(int amount) {
        static const int kFigs[kAmountCount] = {1, 2, 3, 4, 6};
        return kFigs[ClampT(amount, 0, kAmountCount - 1)];
    }

    HarmonographScene(float w,
                      float h,
                      unsigned seed,
                      const Palette& pal,
                      int amount)
        : rng_(seed ^ 0x2f19u) {
        w_ = w;
        h_ = h;
        pal_ = &pal;
        float m = std::min(w, h);
        int count = FiguresFor(amount);
        for (int k = 0; k < count; k++) {
            Fig f;
            int fx = rng_.Int(1, 5);
            int fy = rng_.Int(1, 5);
            if (fy == fx) {
                fy = (fx % 5) + 1;   // equal ratios collapse to a flat lens
            }
            float detune = rng_.Range(0.002f, 0.010f) * rng_.Sign();
            f.a[0] = rng_.Range(0.26f, 0.40f) * m;
            f.a[1] = rng_.Range(0.10f, 0.22f) * m;
            f.a[2] = rng_.Range(0.26f, 0.40f) * m;
            f.a[3] = rng_.Range(0.10f, 0.22f) * m;
            f.f[0] = (float)fx;
            f.f[1] = (float)fx + detune;
            f.f[2] = (float)fy;
            f.f[3] = (float)fy + detune * rng_.Sign();
            for (int i = 0; i < 4; i++) {
                f.p[i] = rng_.Range(0.0f, 2 * kPi);
            }
            float dd = rng_.Range(0.0006f, 0.0012f);
            for (int i = 0; i < 4; i++) {
                f.d[i] = dd;
            }
            f.color = pal_->ink[(size_t)rng_.Int(0, (int)pal_->ink.size() - 1)];
            f.lw = rng_.Range(0.45f, 0.75f);
            f.alpha = rng_.Range(0.22f, 0.42f);
            f.t = 0;
            f.tEnd = rng_.Range(2000.0f, 2800.0f);
            f.hasLast = false;
            figs_.push_back(f);
        }
    }

    bool Step(SceneCtx& ctx) override {
        int steps = clock_.Take((4.0f + ctx.param * 60.0f) * 30.0f, ctx.dt);
        int alive = 0;
        for (size_t k = 0; k < figs_.size(); k++) {
            Fig& f = figs_[k];
            if (f.t >= f.tEnd) {
                continue;
            }
            alive++;
            pts_.clear();
            if (f.hasLast) {
                pts_.push_back(f.last);
            }
            for (int i = 0; i < steps && f.t < f.tEnd; i++, f.t += 0.22f) {
                D2D1_POINT_2F p = PointAt(f, f.t);
                pts_.push_back(p);
                f.last = p;
                f.hasLast = true;
            }
            if (pts_.size() < 2) {
                continue;
            }
            SetInk(ctx, f.color, f.alpha);
            ID2D1PathGeometry* g = MakePolyline(ctx.factory, pts_.data(),
                                                (UINT32)pts_.size(), false);
            if (g) {
                ctx.target->DrawGeometry(g, ctx.brush, f.lw, nullptr);
                g->Release();
            }
        }
        return alive == 0;
    }

   private:
    struct Fig {
        float a[4], f[4], p[4], d[4];
        Rgb color;
        float lw, alpha, t, tEnd;
        D2D1_POINT_2F last;
        bool hasLast;
    };

    D2D1_POINT_2F PointAt(const Fig& f, float t) const {
        float x = f.a[0] * std::sin(f.f[0] * t * 0.05f + f.p[0]) *
                      std::exp(-f.d[0] * t) +
                  f.a[1] * std::sin(f.f[1] * t * 0.05f + f.p[1]) *
                      std::exp(-f.d[1] * t);
        float y = f.a[2] * std::sin(f.f[2] * t * 0.05f + f.p[2]) *
                      std::exp(-f.d[2] * t) +
                  f.a[3] * std::sin(f.f[3] * t * 0.05f + f.p[3]) *
                      std::exp(-f.d[3] * t);
        return Pt(w_ * 0.5f + x, h_ * 0.5f + y);
    }

    Rng rng_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0;
    std::vector<Fig> figs_;
    std::vector<D2D1_POINT_2F> pts_;
    StepClock clock_;
};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
struct Settings {
    std::wstring monitor = L"primary";
    bool enable[kStyleCount] = {true, true, true, true};
    bool rotate = false;
    int rotateSeconds = 300;
    int amount = 2;
    int parameter = 50;
    std::wstring palette = L"aurora";
    std::wstring customColors;
    std::wstring customBackground;
    int hueOffset = 0;
    bool colorRamp = false;
    int rampSpeed = 12;
    int fps = 60;
    int opacity = 100;
    bool globalKeys = false;
    bool clickThrough = false;
    bool keepAwake = true;
    bool startActive = false;
    bool workAreaOnly = false;
    std::wstring hotkey = L"Ctrl+Alt+H";
    std::wstring language = L"auto";
};

static Settings g_settings;
static Palette g_palette;

static unsigned ParseHex(const std::wstring& s) {
    unsigned v = 0;
    for (size_t i = 0; i < s.size(); i++) {
        wchar_t c = s[i];
        int d = -1;
        if (c >= L'0' && c <= L'9') d = c - L'0';
        else if (c >= L'a' && c <= L'f') d = 10 + (c - L'a');
        else if (c >= L'A' && c <= L'F') d = 10 + (c - L'A');
        else continue;
        v = v * 16 + d;
    }
    return v;
}

struct Preset {
    const wchar_t* name;
    unsigned bg;
    unsigned ink[5];
};

// "custom" is the last entry so stepping with Space reaches it too; its
// colors come from the two custom settings rather than from this table.
static const Preset kPresets[] = {
        {L"aurora", 0x05070d, {0x7fe7cf, 0x5fb3ff, 0xa68bff, 0xff7fd0, 0xe8f3ff}},
        {L"ember",  0x0d0603, {0xffb066, 0xff6a3d, 0xffd98a, 0xe0503a, 0xfff0d8}},
        {L"ocean",  0x02080f, {0x4fd1f5, 0x59a5ff, 0x8fe9ff, 0x3b6fd4, 0xdff6ff}},
        {L"neon",   0x05010a, {0xff2ec4, 0x00f0ff, 0xb026ff, 0x39ff14, 0xffffff}},
        {L"forest", 0x030a06, {0x34d399, 0xa3e635, 0x059669, 0xd9f99d, 0xecfccb}},
    {L"mono",   0x07070a, {0xe8e9ee, 0xb9bcc6, 0x8a8f9c, 0x5e636f, 0xffffff}},
    {L"custom", 0x05070d, {0x29d0a5, 0x3aa0ff, 0x7b5cff, 0xff5ec4, 0xe8f3ff}},
};
static const int kPaletteCount =
    (int)(sizeof(kPresets) / sizeof(kPresets[0]));

// Which preset is live. Space steps it, the setting seeds it, and it is
// remembered across restarts like the style and amount are.
static_assert(kPaletteCount == kPaletteNames,
              "add the new palette to every row of kStrings");

static int g_paletteIndex = 0;

static int PaletteIndexFromName(const std::wstring& name) {
    for (int i = 0; i < kPaletteCount; i++) {
        if (name == kPresets[i].name) {
            return i;
        }
    }
    return 0;
}

static void BuildPalette() {
    const Preset& chosen = kPresets[ClampT(g_paletteIndex, 0, kPaletteCount - 1)];
    g_palette.ink.clear();

    if (wcscmp(chosen.name, L"custom") == 0) {
        g_palette.bg = RgbFromHex(g_settings.customBackground.empty()
                                      ? 0x05070d
                                      : ParseHex(g_settings.customBackground));
        std::wstring s = g_settings.customColors;
        size_t start = 0;
        while (start <= s.size() && g_palette.ink.size() < 6) {
            size_t comma = s.find(L',', start);
            std::wstring tok = s.substr(
                start, comma == std::wstring::npos ? std::wstring::npos
                                                   : comma - start);
            if (!tok.empty()) {
                g_palette.ink.push_back(RgbFromHex(ParseHex(tok)));
            }
            if (comma == std::wstring::npos) {
                break;
            }
            start = comma + 1;
        }
        if (g_palette.ink.size() >= 2) {
            return;
        }
        // an unusable custom list falls back to this preset's own colors
        g_palette.ink.clear();
    }

    g_palette.bg = RgbFromHex(chosen.bg);
    for (int i = 0; i < 5; i++) {
        g_palette.ink.push_back(RgbFromHex(chosen.ink[i]));
    }
}

// ---------------------------------------------------------------------------
// Monitors
// ---------------------------------------------------------------------------
struct MonitorEntry {
    RECT rect;              // full monitor bounds
    RECT work;              // bounds minus the taskbar / work area
    bool primary;
    std::wstring device;        // \\.\DISPLAY2
    std::wstring deviceName;    // friendly name, e.g. DELL U2720Q
    int winNum = 0;             // the number in \\.\DISPLAY<n>, as Windows shows it
};

static int DisplayNumberFromDevice(const std::wstring& dev) {
    // "\\.\DISPLAY2" -> 2; the number Windows Settings shows.
    size_t p = dev.rfind(L"DISPLAY");
    if (p == std::wstring::npos) {
        return 0;
    }
    return _wtoi(dev.c_str() + p + 7);
}

static BOOL CALLBACK EnumMonProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    std::vector<MonitorEntry>* out = (std::vector<MonitorEntry>*)lParam;
    MONITORINFOEXW mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMon, &mi)) {
        MonitorEntry e;
        e.rect = mi.rcMonitor;
        e.work = mi.rcWork;
        e.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        e.device = mi.szDevice;
        e.winNum = DisplayNumberFromDevice(e.device);
        DISPLAY_DEVICEW dd;
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        if (EnumDisplayDevicesW(mi.szDevice, 0, &dd, 0)) {
            e.deviceName = dd.DeviceString;
        }
        out->push_back(e);
    }
    return TRUE;
}

static std::vector<MonitorEntry> EnumerateMonitors() {
    std::vector<MonitorEntry> list;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonProc, (LPARAM)&list);
    std::sort(list.begin(), list.end(),
              [](const MonitorEntry& a, const MonitorEntry& b) {
                  if (a.rect.left != b.rect.left) {
                      return a.rect.left < b.rect.left;
                  }
                  return a.rect.top < b.rect.top;
              });
    return list;
}

// ---------------------------------------------------------------------------
// Overlay: one per held display
// ---------------------------------------------------------------------------
enum Phase { kPhaseIn, kPhaseBuild, kPhaseHold, kPhaseOut };

// How long the readout stays up, and how much of that is the fade.
static const float kHudSecs = 2.2f;
static const float kHudFade = 0.6f;

// ---------------------------------------------------------------------------
// Embedded readout font
// ---------------------------------------------------------------------------
//
// Fusion Pixel Font, the 12px monospaced Latin build, subset to the 717
// glyphs of the alphabetic scripts a readout can plausibly be written in:
// ASCII, Latin-1, Latin Extended-A and B, spacing and combining diacritics,
// Greek, Cyrillic and Cyrillic Supplement, general punctuation. That is every
// language written in a Latin, Greek or Cyrillic alphabet, in the pixel
// design, for 8424 bytes. Anything outside it, and that means CJK, Arabic,
// Hebrew, Thai, the Indic scripts, is drawn by the fallback below instead.
//
// Reproduce with fontTools, from the release named in the URL:
//   subset fusion-pixel-12px-monospaced-latin.ttf
//     --unicodes=20-7E,A0-FF,100-17F,180-24F,2B0-2FF,300-36F,370-3FF,
//                400-4FF,500-52F,2000-206F,20A0-20BF
//     --flavor=woff2 --no-glyph-names --notdef-outline
//     --name-IDs=0,1,2,3,4,5,6,13,14 --layout-features=
//
// It is stored as WOFF2 and unpacked by DirectWrite on the way in, which is
// what keeps seven hundred glyphs down to the size a hundred would take raw.
//
//   Fusion Pixel Font   https://github.com/TakWolf/fusion-pixel-font
//   Copyright (c) 2022, TakWolf (https://takwolf.com)
//
// The font data below is licensed under the SIL Open Font License 1.1, which
// is reproduced in full. It covers the font data only: the mod itself is MIT
// as declared at the top of this file.

/*
Fusion Pixel Font
https://github.com/TakWolf/fusion-pixel-font

Copyright (c) 2022, TakWolf (https://takwolf.com).

This Font Software is licensed under the SIL Open Font License, Version 1.1.
This license is copied below, and is also available with a FAQ at:
https://openfontlicense.org


-----------------------------------------------------------
SIL OPEN FONT LICENSE Version 1.1 - 26 February 2007
-----------------------------------------------------------

PREAMBLE
The goals of the Open Font License (OFL) are to stimulate worldwide
development of collaborative font projects, to support the font creation
efforts of academic and linguistic communities, and to provide a free and
open framework in which fonts may be shared and improved in partnership
with others.

The OFL allows the licensed fonts to be used, studied, modified and
redistributed freely as long as they are not sold by themselves. The
fonts, including any derivative works, can be bundled, embedded,
redistributed and/or sold with any software provided that any reserved
names are not used by derivative works. The fonts and derivatives,
however, cannot be released under any other type of license. The
requirement for fonts to remain under this license does not apply
to any document created using the fonts or their derivatives.

DEFINITIONS
"Font Software" refers to the set of files released by the Copyright
Holder(s) under this license and clearly marked as such. This may
include source files, build scripts and documentation.

"Reserved Font Name" refers to any names specified as such after the
copyright statement(s).

"Original Version" refers to the collection of Font Software components as
distributed by the Copyright Holder(s).

"Modified Version" refers to any derivative made by adding to, deleting,
or substituting -- in part or in whole -- any of the components of the
Original Version, by changing formats or by porting the Font Software to a
new environment.

"Author" refers to any designer, engineer, programmer, technical
writer or other person who contributed to the Font Software.

PERMISSION & CONDITIONS
Permission is hereby granted, free of charge, to any person obtaining
a copy of the Font Software, to use, study, copy, merge, embed, modify,
redistribute, and sell modified and unmodified copies of the Font
Software, subject to the following conditions:

1) Neither the Font Software nor any of its individual components,
in Original or Modified Versions, may be sold by itself.

2) Original or Modified Versions of the Font Software may be bundled,
redistributed and/or sold with any software, provided that each copy
contains the above copyright notice and this license. These can be
included either as stand-alone text files, human-readable headers or
in the appropriate machine-readable metadata fields within text or
binary files as long as those fields can be easily viewed by the user.

3) No Modified Version of the Font Software may use the Reserved Font
Name(s) unless explicit written permission is granted by the corresponding
Copyright Holder. This restriction only applies to the primary font name as
presented to the users.

4) The name(s) of the Copyright Holder(s) or the Author(s) of the Font
Software shall not be used to promote, endorse or advertise any
Modified Version, except to acknowledge the contribution(s) of the
Copyright Holder(s) and the Author(s) or with their explicit written
permission.

5) The Font Software, modified or unmodified, in part or in whole,
must be distributed entirely under this license, and must not be
distributed under any other license. The requirement for fonts to
remain under this license does not apply to any document created
using the Font Software.

TERMINATION
This license becomes null and void if any of the above conditions are
not met.

DISCLAIMER
THE FONT SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
OF COPYRIGHT, PATENT, TRADEMARK, OR OTHER RIGHT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
INCLUDING ANY GENERAL, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF THE USE OR INABILITY TO USE THE FONT SOFTWARE OR FROM
OTHER DEALINGS IN THE FONT SOFTWARE.
*/

static const wchar_t kEmbeddedFontFamily[] = L"Fusion Pixel 12px Mono latin";
static const int kEmbeddedFontGrid = 12;

static const GUID kIID_IDWriteFactory5 = {
    0x958db99a, 0xbe2a, 0x4f09,
    {0xaf, 0x7d, 0x65, 0x18, 0x98, 0x03, 0xd1, 0xd3}};

static const GUID kIID_IDWriteFactory2 = {
    0x0439fc60, 0xca44, 0x4994,
    {0x8d, 0xee, 0x3a, 0x9a, 0xf7, 0xb7, 0x32, 0xec}};

static const GUID kIID_IDWriteTextFormat1 = {
    0x5f174b49, 0x0d8b, 0x4cfb,
    {0x8b, 0xca, 0xf1, 0xcc, 0xe9, 0xd0, 0x6c, 0x67}};

// clang-format off
static const char kEmbeddedFontB64[] =
    "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";
// clang-format on

// Registered for the life of the process and torn down beside the factory.
static IDWriteFontFileLoader* g_fontLoader = nullptr;
static IDWriteFontCollection* g_embeddedFonts = nullptr;
static bool g_embeddedFontsTried = false;
static IDWriteFontFallback* g_hudFallback = nullptr;
static std::vector<BYTE> g_embeddedFontBytes;

static std::vector<BYTE> DecodeBase64(const char* s, size_t len) {
    std::vector<BYTE> out;
    out.reserve(len / 4 * 3);
    unsigned acc = 0;
    int bits = 0;
    for (size_t i = 0; i < len; i++) {
        char c = s[i];
        int v;
        if (c >= 'A' && c <= 'Z') {
            v = c - 'A';
        } else if (c >= 'a' && c <= 'z') {
            v = c - 'a' + 26;
        } else if (c >= '0' && c <= '9') {
            v = c - '0' + 52;
        } else if (c == '+') {
            v = 62;
        } else if (c == '/') {
            v = 63;
        } else {
            continue;   // padding, and any whitespace that creeps in
        }
        acc = (acc << 6) | (unsigned)v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((BYTE)((acc >> bits) & 0xFF));
        }
    }
    return out;
}

// Turns the bytes above into a private font collection. This needs the
// Windows 10 in memory font loader; on anything older it returns null and the
// readout falls back to a stock face, which is why none of it is fatal.
static IDWriteFontCollection* BuildEmbeddedFonts() {
    IDWriteFactory5* f5 = nullptr;
    if (!g_dwrite ||
        FAILED(g_dwrite->QueryInterface(kIID_IDWriteFactory5, (void**)&f5)) ||
        !f5) {
        return nullptr;
    }

    g_embeddedFontBytes =
        DecodeBase64(kEmbeddedFontB64, ARRAYSIZE(kEmbeddedFontB64) - 1);

    // The blob is WOFF2, so hand it to DirectWrite to unpack before anything
    // else looks at it. If that fails for any reason the raw bytes are passed
    // through and the reference below simply refuses them.
    const void* fontData = g_embeddedFontBytes.data();
    UINT32 fontSize = (UINT32)g_embeddedFontBytes.size();
    IDWriteFontFileStream* unpacked = nullptr;
    const void* fragment = nullptr;
    void* fragmentCtx = nullptr;
    DWRITE_CONTAINER_TYPE container = f5->AnalyzeContainerType(fontData, fontSize);
    if (container != DWRITE_CONTAINER_TYPE_UNKNOWN &&
        SUCCEEDED(f5->UnpackFontFile(container, fontData, fontSize, &unpacked))) {
        UINT64 unpackedSize = 0;
        if (SUCCEEDED(unpacked->GetFileSize(&unpackedSize)) && unpackedSize > 0 &&
            unpackedSize <= 0x7FFFFFFFull &&
            SUCCEEDED(unpacked->ReadFileFragment(&fragment, 0, unpackedSize,
                                                 &fragmentCtx))) {
            fontData = fragment;
            fontSize = (UINT32)unpackedSize;
        }
    }

    IDWriteInMemoryFontFileLoader* loader = nullptr;
    IDWriteFontFile* file = nullptr;
    IDWriteFontSetBuilder1* builder = nullptr;
    IDWriteFontSet* set = nullptr;
    IDWriteFontCollection1* collection = nullptr;
    IDWriteFontCollection* result = nullptr;

    if (SUCCEEDED(f5->CreateInMemoryFontFileLoader(&loader)) &&
        SUCCEEDED(g_dwrite->RegisterFontFileLoader(loader))) {
        // The loader has to outlive every font file it hands out, so the
        // teardown below owns it rather than this function.
        g_fontLoader = loader;
        // A null owner means DirectWrite takes its own copy, so the unpacked
        // fragment and the base64 bytes are both free to go afterwards.
        if (SUCCEEDED(loader->CreateInMemoryFontFileReference(
                g_dwrite, fontData, fontSize, nullptr, &file)) &&
            SUCCEEDED(f5->CreateFontSetBuilder(&builder)) &&
            SUCCEEDED(builder->AddFontFile(file)) &&
            SUCCEEDED(builder->CreateFontSet(&set)) &&
            SUCCEEDED(f5->CreateFontCollectionFromFontSet(set, &collection))) {
            result = collection;
            collection = nullptr;
        }
    } else {
        SafeRelease(&loader);
    }

    SafeRelease(&collection);
    SafeRelease(&set);
    SafeRelease(&builder);
    SafeRelease(&file);
    // CreateInMemoryFontFileReference was given a null owner, which is the
    // case where DirectWrite keeps its own copy, so nothing below needs these.
    g_embeddedFontBytes.clear();
    g_embeddedFontBytes.shrink_to_fit();
    if (unpacked) {
        if (fragmentCtx) {
            unpacked->ReleaseFileFragment(fragmentCtx);
        }
        unpacked->Release();
    }
    f5->Release();
    return result;
}

static void ReleaseEmbeddedFonts() {
    SafeRelease(&g_hudFallback);
    SafeRelease(&g_embeddedFonts);
    if (g_fontLoader) {
        if (g_dwrite) {
            g_dwrite->UnregisterFontFileLoader(g_fontLoader);
        }
        g_fontLoader->Release();
        g_fontLoader = nullptr;
    }
    g_embeddedFontBytes.clear();
    g_embeddedFontBytes.shrink_to_fit();
    g_embeddedFontsTried = false;
}

// The readout text is built in one place so the fitting pass below can measure
// every combination the mod is ever able to show.
static std::wstring HudTextFor(int style, int amount, float param,
                               int paletteIdx) {
    const Strings* t = g_strings;
    int st = ClampT(style, 0, kStyleCount - 1);
    WCHAR buf[256];
    swprintf_s(buf, ARRAYSIZE(buf), L"%s     %s %d%%     %s %s     %s",
               t->styles[st], t->params[st],
               (int)(param * 100.0f + 0.5f), t->amount,
               t->amounts[ClampT(amount, 0, kAmountCount - 1)],
               t->palettes[ClampT(paletteIdx, 0, kPaletteNames - 1)]);
    return buf;
}

// Fusion Pixel Font (https://github.com/TakWolf/fusion-pixel-font, OFL-1.1)
// draws Latin and CJK off one pixel grid, so the readout keeps its shape
// whatever the system locale is. The project ships a family per pixel size,
// width mode and language rather than a single font, so take whichever of them
// is installed, preferring the largest grid and the monospaced cut. None of
// this is required: with none of them present the mod falls back to the copy
// embedded below, and to a stock face if even that cannot be loaded.
struct HudFont {
    std::wstring family = L"Consolas";
    int pixelSize = 0;   // 0 when the fallback face is in use
    // null for anything the system already knows about
    IDWriteFontCollection* collection = nullptr;
};

static bool FindPixelFamily(IDWriteFontCollection* sys, HudFont* out) {
    static const int kGrids[] = {12, 10, 8};
    static const wchar_t* kModes[] = {L"Mono", L"Prop"};
    static const wchar_t* kLangs[] = {L"latin", L"zh_hans", L"zh_hant", L"ja",
                                      L"ko"};
    for (size_t g = 0; g < ARRAYSIZE(kGrids); g++) {
        for (size_t m = 0; m < ARRAYSIZE(kModes); m++) {
            for (size_t l = 0; l < ARRAYSIZE(kLangs); l++) {
                WCHAR name[64];
                swprintf_s(name, ARRAYSIZE(name), L"Fusion Pixel %dpx %s %s",
                           kGrids[g], kModes[m], kLangs[l]);
                UINT32 index = 0;
                BOOL exists = FALSE;
                if (SUCCEEDED(sys->FindFamilyName(name, &index, &exists)) &&
                    exists) {
                    out->family = name;
                    out->pixelSize = kGrids[g];
                    return true;
                }
            }
        }
    }
    return false;
}

// The font the user's own system uses for its interface, which Windows has
// already chosen to suit their locale: Segoe UI on an English install, Yu
// Gothic UI on a Japanese one, Malgun Gothic on a Korean one. It is the right
// partner for the embedded font precisely because it is, by definition, a face
// their machine can write their language in.
static std::wstring SystemUiFontFamily() {
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(ncm);
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0) &&
        ncm.lfMessageFont.lfFaceName[0]) {
        return ncm.lfMessageFont.lfFaceName;
    }
    return L"Segoe UI";
}

// The regular face of a family, from the given collection or from the system
// one when that is null. Caller releases.
static IDWriteFont* FirstFont(IDWriteFontCollection* collection,
                              const wchar_t* family) {
    IDWriteFontCollection* owned = nullptr;
    if (!collection) {
        if (!g_dwrite ||
            FAILED(g_dwrite->GetSystemFontCollection(&owned, FALSE)) || !owned) {
            return nullptr;
        }
        collection = owned;
    }
    UINT32 index = 0;
    BOOL exists = FALSE;
    IDWriteFontFamily* fam = nullptr;
    IDWriteFont* fnt = nullptr;
    if (SUCCEEDED(collection->FindFamilyName(family, &index, &exists)) && exists &&
        SUCCEEDED(collection->GetFontFamily(index, &fam))) {
        if (FAILED(fam->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_NORMAL,
                                             DWRITE_FONT_STRETCH_NORMAL,
                                             DWRITE_FONT_STYLE_NORMAL, &fnt))) {
            fnt = nullptr;
        }
    }
    SafeRelease(&fam);
    SafeRelease(&owned);
    return fnt;
}

// x height as a fraction of the em, which is what the eye actually compares
// when two faces sit on the same line.
static float XHeightRatio(IDWriteFontCollection* collection,
                          const wchar_t* family) {
    IDWriteFont* fnt = FirstFont(collection, family);
    if (!fnt) {
        return 0;
    }
    float ratio = 0;
    IDWriteFontFace* face = nullptr;
    if (SUCCEEDED(fnt->CreateFontFace(&face))) {
        DWRITE_FONT_METRICS fm;
        face->GetMetrics(&fm);
        if (fm.designUnitsPerEm > 0 && fm.xHeight > 0) {
            ratio = (float)fm.xHeight / (float)fm.designUnitsPerEm;
        }
    }
    SafeRelease(&face);
    fnt->Release();
    return ratio;
}

// Can this face write every word the active language can put on screen? A
// half pixel, half system font line looks worse than one drawn entirely in the
// system font, so this decides which of the two the readout gets rather than
// leaving the fallback to fill gaps mid word.
static bool CoversActiveLanguage(IDWriteFontCollection* collection,
                                 const wchar_t* family) {
    IDWriteFont* fnt = FirstFont(collection, family);
    if (!fnt) {
        return false;
    }
    const Strings* t = g_strings;
    std::wstring all = t->amount;
    all += L"0123456789% ";
    for (int i = 0; i < kStyleCount; i++) {
        all += t->styles[i];
        all += t->params[i];
    }
    for (int i = 0; i < kAmountCount; i++) {
        all += t->amounts[i];
    }
    for (int i = 0; i < kPaletteNames; i++) {
        all += t->palettes[i];
    }
    bool ok = true;
    for (size_t i = 0; i < all.size() && ok; i++) {
        BOOL has = FALSE;
        if (FAILED(fnt->HasCharacter((UINT32)all[i], &has)) || !has) {
            ok = false;
        }
    }
    fnt->Release();
    return ok;
}

// Everything the embedded font cannot draw goes to the user's interface font,
// sized so its lowercase matches rather than sitting visibly larger, and then
// to the system's own fallback chain for anything even that lacks. The result
// is that no readout can ever come out blank or boxed, in any language.
static IDWriteFontFallback* BuildFallbackChain(const HudFont& font) {
    IDWriteFactory2* f2 = nullptr;
    if (!g_dwrite ||
        FAILED(g_dwrite->QueryInterface(kIID_IDWriteFactory2, (void**)&f2)) ||
        !f2) {
        return nullptr;   // pre Windows 8.1: the stock chain still applies
    }

    std::wstring ui = SystemUiFontFamily();
    float mine = XHeightRatio(font.collection, font.family.c_str());
    float theirs = XHeightRatio(nullptr, ui.c_str());
    float scale = (mine > 0 && theirs > 0) ? mine / theirs : 1.0f;
    scale = ClampT(scale, 0.7f, 1.5f);

    IDWriteFontFallbackBuilder* builder = nullptr;
    IDWriteFontFallback* system = nullptr;
    IDWriteFontFallback* result = nullptr;
    DWRITE_UNICODE_RANGE all = {0, 0x10FFFF};
    const WCHAR* names[1] = {ui.c_str()};
    if (SUCCEEDED(f2->CreateFontFallbackBuilder(&builder)) &&
        SUCCEEDED(builder->AddMapping(&all, 1, names, 1, nullptr, nullptr,
                                      nullptr, scale)) &&
        SUCCEEDED(f2->GetSystemFontFallback(&system)) &&
        SUCCEEDED(builder->AddMappings(system))) {
        if (FAILED(builder->CreateFontFallback(&result))) {
            result = nullptr;
        }
    }
    Wh_Log(L"readout fallback: %s at %.2f", ui.c_str(), scale);
    SafeRelease(&system);
    SafeRelease(&builder);
    f2->Release();
    return result;
}

// Arabic and Hebrew read the other way, which also puts the line in the other
// bottom corner. DirectWrite handles the digits and the percent sign inside it.
static void ApplyDirection(IDWriteTextFormat* fmt) {
    if (fmt && g_strings->rtl) {
        fmt->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
        fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    }
}

static void ApplyFallback(IDWriteTextFormat* fmt) {
    if (!fmt || !g_hudFallback) {
        return;
    }
    IDWriteTextFormat1* fmt1 = nullptr;
    if (SUCCEEDED(fmt->QueryInterface(kIID_IDWriteTextFormat1, (void**)&fmt1)) &&
        fmt1) {
        fmt1->SetFontFallback(g_hudFallback);
        fmt1->Release();
    }
}

// Resolved once. Only the worker thread builds overlays, so a plain flag is
// enough, and a font installed mid-session is not worth a rescan per frame.
static HudFont g_hudFont;
static bool g_hudFontResolved = false;

// The widest readout the mod can produce, measured once at the reference size.
// It depends on the resolved face and the active language, so it is the same
// for every display; only the fitting arithmetic below is per overlay. Driving
// six displays would otherwise lay out the same 140 strings six times on every
// show.
static float g_hudWidestAtRef = 0;

// Called when the language changes: the words are different, so the fitted
// size and possibly the face itself have to be worked out again.
static void ResetHudFontChoice();

static const HudFont& HudFontOnce() {
    if (g_hudFontResolved) {
        return g_hudFont;
    }
    g_hudFontResolved = true;
    IDWriteFontCollection* sys = nullptr;
    if (g_dwrite && SUCCEEDED(g_dwrite->GetSystemFontCollection(&sys, FALSE)) &&
        sys) {
        FindPixelFamily(sys, &g_hudFont);
        sys->Release();
    }
    if (g_hudFont.pixelSize == 0) {
        // Built at most once. Changing the readout language sends us back
        // through here, and building a second time would register another font
        // file loader against the factory and strand the first collection.
        if (!g_embeddedFontsTried) {
            g_embeddedFontsTried = true;
            g_embeddedFonts = BuildEmbeddedFonts();
        }
        if (g_embeddedFonts) {
            g_hudFont.family = kEmbeddedFontFamily;
            g_hudFont.pixelSize = kEmbeddedFontGrid;
            g_hudFont.collection = g_embeddedFonts;
        }
    }
    if (g_hudFont.pixelSize > 0 &&
        !CoversActiveLanguage(g_hudFont.collection, g_hudFont.family.c_str())) {
        // The embedded subset is Latin, Greek and Cyrillic. Installing the
        // full family puts CJK back on the pixel grid and this check passes.
        Wh_Log(L"pixel font cannot write %s; using the system font",
               g_strings->tag);
        g_hudFont = HudFont();
        g_hudFont.family = SystemUiFontFamily();
    }
    Wh_Log(L"readout font: %s%s", g_hudFont.family.c_str(),
           g_hudFont.collection ? L" (embedded)" : L"");
    g_hudFallback = BuildFallbackChain(g_hudFont);
    return g_hudFont;
}

class Overlay {
   public:
    Overlay(ID2D1Factory* factory, const RECT& rc, unsigned seed)
        : factory_(factory), rect_(rc), seed_(seed) {}

    ~Overlay() {
        Destroy();
        SafeRelease(&hudFormat_);
    }

    bool Create();
    void Destroy();
    void Render(float dtSec);
    void NewScene();
    bool Occluded() const { return occluded_; }
    const RECT& Rect() const { return rect_; }
    // Briefly show what just changed. The overlay is otherwise completely
    // clean, and this is the only text it ever draws.
    void FlashHud();
    // Arms on the click that activates an inactive overlay, and is spent by
    // that same click: it reports the current state instead of changing it,
    // which is the only sign the overlay has taken focus.
    void ArmFocusClick() { focusClickArmed_ = true; }
    // Called on every button down. It spends the arming, so a press that is
    // dragged off the window and released elsewhere cannot leave it set for
    // the next click to trip over: that next press simply finds nothing armed.
    void BeginClick() {
        reportOnUp_ = focusClickArmed_;
        focusClickArmed_ = false;
    }
    bool TakeReportClick() {
        bool report = reportOnUp_;
        reportOnUp_ = false;
        return report;
    }
    void ClearFocusClick() {
        focusClickArmed_ = false;
        reportOnUp_ = false;
    }

    int style = kStyleFlow;
    int amount = 2;
    float param = 0.5f;

    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

   private:
    bool CreateDeviceResources();
    void CreateHudFormat();
    void DiscardDeviceResources();

    ID2D1Factory* factory_ = nullptr;
    HWND hwnd_ = nullptr;
    RECT rect_;
    unsigned seed_ = 1;

    ID2D1HwndRenderTarget* rt_ = nullptr;
    ID2D1BitmapRenderTarget* buf_ = nullptr;
    ID2D1Bitmap* bufBitmap_ = nullptr;
    bool occluded_ = false;
    ID2D1SolidColorBrush* brush_ = nullptr;

    std::unique_ptr<Scene> scene_;
    IDWriteTextFormat* hudFormat_ = nullptr;
    std::wstring hudText_;
    float hudT_ = 0;
    float hudPx_ = 15.0f;
    float hudMarginX_ = 24.0f;
    float retryWait_ = 0.0f;
    bool hudCrisp_ = false;
    bool focusClickArmed_ = false;
    bool reportOnUp_ = false;

    Phase phase_ = kPhaseIn;
    float phaseT_ = 0;
    float artAlpha_ = 0;
    float holdT_ = 0;
};

// forward declarations for the controller hooks the window proc calls into
static void Controller_CycleStyle(Overlay* ov);
static void Controller_StepAmount(Overlay* ov);
static void Controller_Wheel(Overlay* ov, int delta);
static void Controller_RequestRebuild();
static void Controller_RequestClose();
static void Controller_RequestPalette();
static void Controller_CyclePalette();

bool Overlay::Create() {
    // Bottom of the z-order: it sits above the wallpaper but under every
    // application window. Focusable on click so keyboard input reaches it, but
    // it is never raised.
    //
    // WS_EX_TRANSPARENT hands every click straight through to whatever is
    // below, which at the bottom of the z-order is the desktop, so the icons
    // the overlay is drawn over stay usable. WS_EX_NOACTIVATE goes with it:
    // there is no point taking the focus for keys that can no longer be aimed
    // at the window. The overlay is then driven by the toggle hotkey and, if
    // it is on, the global key, which is what the setting says.
    //
    // WS_EX_LAYERED has to travel with them. Measured, not assumed: with
    // WS_EX_TRANSPARENT alone a synthetic click still landed on the upper
    // window, and only with both styles did it reach the one underneath. The
    // pass through lives in the layered composition path, so at full opacity
    // this is a layered window at alpha 255 rather than a plain one.
    DWORD exStyle = WS_EX_TOOLWINDOW;
    if (g_settings.clickThrough) {
        exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
    }
    hwnd_ = CreateWindowExW(
        exStyle, kWindowClass, L"",
        WS_POPUP, rect_.left, rect_.top, rect_.right - rect_.left,
        rect_.bottom - rect_.top, nullptr, nullptr,
        g_modInstance, nullptr);
    if (!hwnd_) {
        Wh_Log(L"CreateWindowEx failed (%u)", GetLastError());
        return false;
    }
    SetWindowLongPtrW(hwnd_, GWLP_USERDATA, (LONG_PTR)this);

    // Verified rather than assumed: a WS_EX_LAYERED window that an
    // ID2D1HwndRenderTarget presents to does honour the constant alpha on
    // Windows 11, so this is a live control and not a dead one.
    if (g_settings.opacity < 100 || g_settings.clickThrough) {
        SetWindowLongPtrW(hwnd_, GWL_EXSTYLE,
                          GetWindowLongPtrW(hwnd_, GWL_EXSTYLE) |
                              WS_EX_LAYERED);
        // 255 at full opacity, which is what a click through window at 100
        // gets: layered for the hit testing, unchanged on screen.
        BYTE a = (BYTE)(255 * g_settings.opacity / 100);
        SetLayeredWindowAttributes(hwnd_, 0, a, LWA_ALPHA);
    }

    if (!CreateDeviceResources()) {
        return false;
    }
    NewScene();

    // ShowWindow goes through WM_WINDOWPOSCHANGING, which forces
    // hwndInsertAfter to HWND_BOTTOM, so no separate sink is needed.
    ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
    return true;
}

void Overlay::Destroy() {
    DiscardDeviceResources();
    scene_.reset();
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

bool Overlay::CreateDeviceResources() {
    if (rt_ && buf_ && brush_) {
        return true;
    }
    DiscardDeviceResources();   // never leave a half-built set behind
    D2D1_RENDER_TARGET_PROPERTIES props;
    ZeroMemory(&props, sizeof(props));
    props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    // 96 dpi so that one device independent pixel is one physical pixel,
    // whatever the display's scaling is.
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

    D2D1_HWND_RENDER_TARGET_PROPERTIES hprops;
    ZeroMemory(&hprops, sizeof(hprops));
    hprops.hwnd = hwnd_;
    hprops.pixelSize.width = (UINT32)(rect_.right - rect_.left);
    hprops.pixelSize.height = (UINT32)(rect_.bottom - rect_.top);
    hprops.presentOptions = g_presentImmediately
                                ? D2D1_PRESENT_OPTIONS_IMMEDIATELY
                                : D2D1_PRESENT_OPTIONS_NONE;

    if (FAILED(factory_->CreateHwndRenderTarget(&props, &hprops, &rt_))) {
        Wh_Log(L"CreateHwndRenderTarget failed");
        DiscardDeviceResources();
        return false;
    }
    rt_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    // The accumulation buffer must carry alpha even though the window does
    // not, so the artwork can be composited at a fade opacity.
    D2D1_PIXEL_FORMAT fmt;
    fmt.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    fmt.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    if (FAILED(rt_->CreateCompatibleRenderTarget(
            nullptr, nullptr, &fmt,
            D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &buf_))) {
        Wh_Log(L"CreateCompatibleRenderTarget failed");
        DiscardDeviceResources();
        return false;
    }
    buf_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    // GetBitmap hands back the same object with an AddRef each time, so hold
    // one reference for the life of the buffer rather than one per frame.
    if (FAILED(buf_->GetBitmap(&bufBitmap_))) {
        Wh_Log(L"GetBitmap failed");
        DiscardDeviceResources();
        return false;
    }

    // hudFormat_ deliberately outlives a device loss: IDWriteTextFormat is
    // device independent, so it is built once per overlay and released in the
    // destructor rather than rebuilt on every recreate.
    if (g_dwrite && !hudFormat_) {
        CreateHudFormat();
    }

    D2D1_COLOR_F white = {1, 1, 1, 1};
    if (FAILED(rt_->CreateSolidColorBrush(&white, nullptr, &brush_))) {
        Wh_Log(L"CreateSolidColorBrush failed");
        DiscardDeviceResources();
        return false;
    }
    return true;
}

static void ResetHudFontChoice() {
    SafeRelease(&g_hudFallback);
    g_hudWidestAtRef = 0;
    g_hudFontResolved = false;
    g_hudFont = HudFont();
}

// Picks the language table, from the setting or from Windows when the setting
// is automatic.
static void SelectStrings(const std::wstring& setting) {
    const Strings* chosen = nullptr;
    if (setting.empty() || setting == L"auto") {
        chosen = StringsFromSystem();
    } else {
        chosen = FindStrings(setting.c_str());
    }
    if (!chosen) {
        chosen = &kStrings[0];
    }
    if (chosen != g_strings) {
        g_strings = chosen;
        Wh_Log(L"readout language: %s", g_strings->tag);
        ResetHudFontChoice();
    }
}

// The largest size at which the widest line still fits the width given,
// snapped down to a whole multiple of a pixel font's grid.
static float FitReadoutSize(float avail, float widest, float refSize,
                            float target, int grid, bool* crisp) {
    float fit = widest > 0 ? refSize * avail / widest : target;
    float px = std::min(target, fit);
    *crisp = false;
    if (grid > 0) {
        int maxSteps = (int)(fit / (float)grid);
        int steps = (int)(target / (float)grid + 0.5f);
        if (steps < 1) {
            steps = 1;
        }
        if (maxSteps >= 1) {
            if (steps > maxSteps) {
                steps = maxSteps;
            }
            px = (float)(steps * grid);
            *crisp = true;
        }
    }
    return std::max(8.0f, px);
}

// Sizes the readout to the largest it can be while the widest line the mod is
// able to produce still fits this display, so it never wraps and never runs
// off the edge whatever happens to be showing.
void Overlay::CreateHudFormat() {
    if (!g_dwrite) {
        return;
    }
    const HudFont& font = HudFontOnce();
    float w = (float)(rect_.right - rect_.left);
    float h = (float)(rect_.bottom - rect_.top);
    // The side gutter comes from the width and the bottom one from the height.
    // Taking both from the height put a gutter the size of a portrait screen's
    // height down its narrow sides, which squeezed the text for no reason.
    float marginX = std::max(24.0f, w * 0.025f);
    float avail = std::max(80.0f, w - marginX * 2.0f);
    // A pixel font has one weight; asking for a heavier one makes DirectWrite
    // embolden it algorithmically, which smears the grid.
    DWRITE_FONT_WEIGHT weight = font.pixelSize > 0
                                    ? DWRITE_FONT_WEIGHT_NORMAL
                                    : DWRITE_FONT_WEIGHT_SEMI_BOLD;

    // Measure every combination once at a reference size. Advance widths scale
    // linearly with the font size, so one pass gives the exact size at which
    // the worst case fits, without a search.
    const float kRef = 32.0f;
    float widest = g_hudWidestAtRef;
    IDWriteTextFormat* probe = nullptr;
    if (widest <= 0 && SUCCEEDED(g_dwrite->CreateTextFormat(
            font.family.c_str(), font.collection, weight,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, kRef, L"",
            &probe))) {
        // The measuring pass has to see exactly what the drawing pass will,
        // fallback and direction included, or the fitted size would be a guess.
        ApplyFallback(probe);
        ApplyDirection(probe);
        for (int st = 0; st < kStyleCount; st++) {
            for (int am = 0; am < kAmountCount; am++) {
                for (int pal = 0; pal < kPaletteCount; pal++) {
                    // 100% is the widest the parameter ever reads.
                    std::wstring line = HudTextFor(st, am, 1.0f, pal);
                    IDWriteTextLayout* layout = nullptr;
                    if (FAILED(g_dwrite->CreateTextLayout(
                            line.c_str(), (UINT32)line.size(), probe, 100000.0f,
                            kRef * 4.0f, &layout))) {
                        continue;
                    }
                    DWRITE_TEXT_METRICS tm;
                    if (SUCCEEDED(layout->GetMetrics(&tm))) {
                        widest = std::max(widest, tm.width);
                    }
                    layout->Release();
                }
            }
        }
        probe->Release();
        g_hudWidestAtRef = widest;
    }

    // A pixel design only lands on whole pixels at whole multiples of the grid
    // it was drawn for, so the size comes down in steps rather than smoothly.
    float target = std::max(15.0f, h * 0.018f);
    float px = FitReadoutSize(avail, widest, kRef, target, font.pixelSize,
                              &hudCrisp_);

    // Losing a whole grid step halves the text, which is a steep price for a
    // wide gutter. Try the narrowest gutter before paying it: the gutter is
    // decoration, the line is the point. Longer languages on narrow displays
    // are exactly where this bites.
    const float kTightMargin = 24.0f;
    if (font.pixelSize > 0 && marginX > kTightMargin) {
        bool tightCrisp = false;
        float tightAvail = std::max(80.0f, w - kTightMargin * 2.0f);
        float tightPx = FitReadoutSize(tightAvail, widest, kRef, target,
                                       font.pixelSize, &tightCrisp);
        if (tightPx > px) {
            px = tightPx;
            hudCrisp_ = tightCrisp;
            marginX = kTightMargin;
        }
    }
    hudMarginX_ = marginX;

    if (FAILED(g_dwrite->CreateTextFormat(
            font.family.c_str(), font.collection, weight,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, px, L"",
            &hudFormat_))) {
        hudFormat_ = nullptr;   // the readout is optional, never fatal
        return;
    }
    ApplyFallback(hudFormat_);
    ApplyDirection(hudFormat_);
    // Belt and braces behind the measuring above: whatever face DirectWrite
    // ends up resolving to, the line stays on one line.
    hudFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    hudPx_ = px;
}

void Overlay::DiscardDeviceResources() {
    SafeRelease(&brush_);
    SafeRelease(&bufBitmap_);
    SafeRelease(&buf_);
    SafeRelease(&rt_);
}

void Overlay::NewScene() {
    float w = (float)(rect_.right - rect_.left);
    float h = (float)(rect_.bottom - rect_.top);
    seed_ = seed_ * 1664525u + 1013904223u;

    switch (style) {
        case kStyleContour:
            scene_.reset(new ContourScene(w, h, seed_, g_palette, amount));
            break;
        case kStyleGrowth:
            scene_.reset(new GrowthScene(w, h, seed_, g_palette, amount));
            break;
        case kStyleHarmonograph:
            scene_.reset(
                new HarmonographScene(w, h, seed_, g_palette, amount));
            break;
        case kStyleFlow:
        default:
            scene_.reset(new FlowScene(w, h, seed_, g_palette, amount));
            break;
    }
    phase_ = kPhaseIn;
    phaseT_ = 0;
    artAlpha_ = 0;
    holdT_ = 0;
    if (buf_) {
        D2D1_COLOR_F clear = {0, 0, 0, 0};
        buf_->BeginDraw();
        buf_->Clear(&clear);
        buf_->EndDraw(nullptr, nullptr);
    }
}

void Overlay::FlashHud() {
    hudText_ = HudTextFor(style, amount, param, g_paletteIndex);
    hudT_ = kHudSecs;
}

static float EaseInOut(float t) {
    return t < 0.5f ? 2 * t * t : 1 - (2 - 2 * t) * (2 - 2 * t) * 0.5f;
}

void Overlay::Render(float dtSec) {
    if (!rt_ || !buf_ || !brush_) {
        // A driver reset or a remote session detaching can leave the device
        // unavailable for a long time, and retrying every frame means sixty
        // failed CreateHwndRenderTarget calls a second per display for the
        // whole outage. Once it has failed, try roughly twice a second.
        if (retryWait_ > 0.0f) {
            retryWait_ -= dtSec;
            return;
        }
        if (!CreateDeviceResources()) {
            retryWait_ = 0.5f;
            return;
        }
        retryWait_ = 0.0f;
        // The accumulation buffer went with the device while the scene kept
        // its progress, so resuming would paint only the strokes that were
        // still to come onto an empty buffer. Start the piece again. This is
        // the single place every recreate path passes through.
        NewScene();
    }

    // The premise of the mod is that it runs for hours beside a heavy job, and
    // the two cases where it is invisible are exactly the ones that job
    // creates: the workstation locked, or a fullscreen window over the display
    // it holds, which always hides it because it sits at HWND_BOTTOM. Direct2D
    // reports both. Left unchecked, contours at maximal would spend about a
    // core on frames nobody can see, competing with the work being waited on.
    // The state reflects the last present, so it is re-read every frame rather
    // than latched; the keep-awake is unaffected and the display stays on.
    // CheckWindowState reports the state as of the last EndDraw, because
    // Direct2D only learns the window is visible again by presenting. Bailing
    // out here without presenting would latch occluded_ on for good, so the
    // frame is still drawn and presented; what is skipped is the simulation
    // below. The worker's slow poll is what keeps the cost down.
    occluded_ = (rt_->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED) != 0;

    const float kFadeIn = 0.75f, kFadeOut = 2.2f;
    // A fixed hold. This used to borrow rotateSeconds when rotation was on,
    // giving two independent timers the same period so they raced each other
    // around the rotation boundary.
    const float holdSecs = 8.0f;

    const float vw = (float)(rect_.right - rect_.left);
    const float vh = (float)(rect_.bottom - rect_.top);

    SceneCtx ctx;
    ctx.factory = factory_;
    ctx.target = buf_;
    ctx.brush = brush_;
    ctx.hue = g_hue;
    ctx.param = param;
    ctx.amount = amount;
    ctx.dt = dtSec;

    buf_->BeginDraw();
    bool done = false;
    // Step in every phase, fade-out included, so contours and growth keep
    // moving all the way through the transition instead of freezing.
    //
    // Nothing is simulated while the window is occluded: the frame is still
    // presented, which is what lets Direct2D notice the window is back, but
    // advancing the art for a screen nobody can see is the part worth saving.
    if (scene_ && !occluded_) {
        done = scene_->Step(ctx);
    } else if (!scene_) {
        // Nothing to draw at all is finished by definition. Occluded is not:
        // that piece is paused, not over.
        done = true;
    }
    if (buf_->EndDraw(nullptr, nullptr) == D2DERR_RECREATE_TARGET) {
        DiscardDeviceResources();
        return;
    }

    // The phase clock belongs to the artwork, so it stops when the artwork
    // does. Advancing it while the overlay is covered would fade the piece
    // out and begin another that nobody asked for and nobody saw, and under
    // a fullscreen window it would cycle scenes for as long as that window
    // was up, rebuilding a contour field every time. The composite below
    // still runs, so the frozen picture is presented and Direct2D can see
    // the window come back.
    if (!occluded_) {
        phaseT_ += dtSec;
        if (phase_ == kPhaseIn) {
            artAlpha_ = EaseInOut(ClampT(phaseT_ / kFadeIn, 0.0f, 1.0f));
            if (phaseT_ >= kFadeIn) {
                phase_ = kPhaseBuild;
                phaseT_ = 0;
                artAlpha_ = 1;
            }
        } else if (phase_ == kPhaseBuild) {
            if (done) {
                if (scene_ && scene_->Continuous()) {
                    // Contours and growth keep animating through the hold so they
                    // never sit still.
                    phase_ = kPhaseHold;
                    phaseT_ = 0;
                } else {
                    // One-shot styles fade away the moment they fill the screen.
                    phase_ = kPhaseOut;
                    phaseT_ = 0;
                }
            }
        } else if (phase_ == kPhaseHold) {
            if (phaseT_ >= holdSecs) {
                phase_ = kPhaseOut;
                phaseT_ = 0;
            }
        } else {
            artAlpha_ = 1 - EaseInOut(ClampT(phaseT_ / kFadeOut, 0.0f, 1.0f));
            if (phaseT_ >= kFadeOut) {
                NewScene();
                return;
            }
        }
    }

    // Composite: background, then the accumulation buffer at the fade opacity.
    // A true linear fade. Repeatedly blending a translucent background over
    // the artwork instead plateaus once the per-frame delta rounds below one
    // 8-bit step.
    rt_->BeginDraw();
    D2D1_COLOR_F bg = ToColorF(g_palette.bg, 1.0f);
    rt_->Clear(&bg);

    if (bufBitmap_) {
        D2D1_RECT_F dst;
        dst.left = 0;
        dst.top = 0;
        dst.right = vw;
        dst.bottom = vh;
        rt_->DrawBitmap(bufBitmap_, &dst, artAlpha_,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);
    }

    if (scene_ && phase_ != kPhaseOut) {
        scene_->PaintCrisp(ctx, rt_);
    }

    // The readout, drawn last so it sits over the art.
    if (hudT_ > 0.0f) {
        hudT_ -= dtSec;
        if (hudFormat_ && !hudText_.empty()) {
            float a = ClampT(hudT_ / kHudFade, 0.0f, 1.0f);
            // The side gutter is whatever the fitting pass settled on.
            float marginX = hudMarginX_;
            float marginY = std::max(24.0f, vh * 0.035f);
            D2D1_RECT_F box;
            box.left = marginX;
            box.top = vh - marginY * 2.0f;
            box.right = vw - marginX;
            // Tall enough for the line whatever size it was fitted to, since
            // DrawText clips to this rectangle.
            box.bottom = box.top + std::max(marginY * 1.5f, hudPx_ * 2.0f);

            D2D1_TEXT_ANTIALIAS_MODE prevAA = rt_->GetTextAntialiasMode();
            if (hudCrisp_) {
                // A pixel font sits on whole pixels; antialiasing would soften
                // the grid it was drawn for.
                rt_->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
            }

            const Rgb& fg = g_palette.ink[g_palette.ink.size() - 1];
            Rgb shifted = ShiftHue(fg, g_hue);

            // a soft dark pass underneath keeps it legible over pale art
            D2D1_COLOR_F shadow = {0, 0, 0, 0.55f * a};
            brush_->SetColor(&shadow);
            D2D1_RECT_F sbox = box;
            sbox.left += 1.5f;
            sbox.top += 1.5f;
            rt_->DrawTextW(hudText_.c_str(), (UINT32)hudText_.size(),
                           hudFormat_, &sbox, brush_,
                           D2D1_DRAW_TEXT_OPTIONS_NONE,
                           DWRITE_MEASURING_MODE_NATURAL);

            D2D1_COLOR_F col = ToColorF(shifted, 0.92f * a);
            brush_->SetColor(&col);
            rt_->DrawTextW(hudText_.c_str(), (UINT32)hudText_.size(),
                           hudFormat_, &box, brush_,
                           D2D1_DRAW_TEXT_OPTIONS_NONE,
                           DWRITE_MEASURING_MODE_NATURAL);

            if (hudCrisp_) {
                rt_->SetTextAntialiasMode(prevAA);
            }
        }
    }

    HRESULT hr = rt_->EndDraw(nullptr, nullptr);
    if (hr == D2DERR_RECREATE_TARGET) {
        // The next frame rebuilds and restarts through the entry path.
        DiscardDeviceResources();
    }
}

LRESULT CALLBACK Overlay::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Overlay* self = (Overlay*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_WINDOWPOSCHANGING: {
            // Re-sink on every position change. ORing SWP_NOZORDER here would
            // also neutralise our own HWND_BOTTOM call and leave the overlay
            // above the user's windows.
            WINDOWPOS* wpos = (WINDOWPOS*)lp;
            wpos->hwndInsertAfter = HWND_BOTTOM;
            wpos->flags &= ~SWP_NOZORDER;
            return 0;
        }
        case WM_SETTINGCHANGE:
            // The taskbar can move or resize without any display change, so
            // the work area shifts under a window sized to it.
            if (g_settings.workAreaOnly && wp == SPI_SETWORKAREA) {
                Controller_RequestRebuild();
            }
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            // Monitor geometry moved under us; rebuild every overlay.
            Controller_RequestRebuild();
            return 0;
        case WM_MOUSEACTIVATE:
            // Focusable on click, but never raised. This message only arrives
            // while the overlay is inactive, so the click carrying it is the
            // one taking focus: arm it to report rather than act.
            if (self) {
                self->ArmFocusClick();
            }
            return MA_ACTIVATE;
        case WM_KILLFOCUS:
            // Focus went elsewhere without the click ever landing here, so the
            // arming is stale.
            if (self) {
                self->ClearFocusClick();
            }
            return 0;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            if (self) {
                self->BeginClick();
            }
            return 0;
        case WM_LBUTTONUP:
            if (self) {
                if (self->TakeReportClick()) {
                    self->FlashHud();
                } else {
                    Controller_CycleStyle(self);
                }
            }
            return 0;
        case WM_RBUTTONUP:
            if (self) {
                if (self->TakeReportClick()) {
                    self->FlashHud();
                } else {
                    Controller_StepAmount(self);
                }
            }
            return 0;
        case WM_CONTEXTMENU:
            return 0;
        case WM_MOUSEWHEEL:
            if (self) {
                Controller_Wheel(self, GET_WHEEL_DELTA_WPARAM(wp));
            }
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) {
                // Post rather than calling HideOverlays directly: that deletes
                // this very Overlay while its WndProc is on the stack.
                Controller_RequestClose();
                return 0;
            }
            if (wp == VK_SPACE) {
                // bit 30 is the previous key state: ignore auto-repeat so a
                // held Space does not race through every palette.
                //
                // Ctrl+Shift+Space belongs to the global hook. If that is live
                // it has already handled the press, and handling it here too
                // would step the palette twice for one press.
                bool chord = (GetKeyState(VK_CONTROL) & 0x8000) != 0 &&
                             (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                if (!(lp & (1 << 30)) && !(chord && g_kbdHookLive)) {
                    Controller_RequestPalette();
                }
                return 0;
            }
            break;
        case WM_CLOSE:
            // Alt+F4 with the overlay focused would otherwise reach
            // DefWindowProc and destroy the window without the controller
            // knowing. Route it the same way Esc goes.
            Controller_RequestClose();
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Controller
// ---------------------------------------------------------------------------
static std::vector<Overlay*> g_overlays;
static std::atomic<bool> g_active{false};
static HANDLE g_toggleEvent = nullptr;

// Teardown cannot depend on a message arriving. PostThreadMessage fails once a
// thread queue is full, and the worker's wait is INFINITE whenever the overlay
// is hidden, so a failed post would leave Wh_ModUninit joining a thread that
// nothing will ever wake. This event sits in the same wait set and setting it
// cannot fail.
static HANDLE g_quitEvent = nullptr;
static std::atomic<DWORD> g_workerThreadId{0};
// The HHOOK deliberately does not live here. Bounding the join means a thread
// can in principle be abandoned, and a straggler that still owned this global
// would unhook whatever hook the next one had installed. It is a local in
// KbdHookThread instead, so an abandoned thread can only ever unhook its own.
static bool g_hotkeyRegistered = false;
static float g_rotateTimer = 0;
// True only while every overlay reports itself hidden, which drops the loop to
// a slow poll instead of stopping it, so the state can clear again.
static bool g_allOccluded = false;

static const UINT WM_VSH_SETTINGS = WM_APP + 1;
static const UINT WM_VSH_QUIT = WM_APP + 2;

static const UINT WM_VSH_CLOSE = WM_APP + 3;
static const UINT WM_VSH_REBUILD = WM_APP + 4;
static const UINT WM_VSH_PALETTE = WM_APP + 5;

static int NextEnabledStyle(int from) {
    for (int i = 1; i <= kStyleCount; i++) {
        int c = (from + i) % kStyleCount;
        if (g_settings.enable[c]) {
            return c;
        }
    }
    return from;
}

static int FirstEnabledStyle() {
    for (int i = 0; i < kStyleCount; i++) {
        if (g_settings.enable[i]) {
            return i;
        }
    }
    return kStyleFlow;
}

// A wheel notch used to write three values straight through, so a fast
// scroll hammered the store dozens of times a second. Mark dirty instead and
// flush on hide and teardown.
static std::atomic<bool> g_stateDirty{false};
static int g_pendingStyle = 0, g_pendingAmount = 2, g_pendingParam = 500;

// The palette gets its own flag rather than sharing g_stateDirty. The pending
// style, amount and parameter above start at the hardcoded defaults and are
// only filled in by SaveState, so a palette step flushing the shared flag
// would write those defaults over whatever the user had actually set.
static std::atomic<bool> g_paletteDirty{false};
static int g_pendingPalette = 0, g_pendingPaletteFrom = 0;

// Seconds since the last change, counted only while something is unsaved. The
// flush used to happen on hide alone, so a sign-out or a reboot that took the
// process down without an unload lost whatever style, amount and parameter the
// user had settled on, which is the opposite of what the readme promises about
// the one you land on being remembered. Waiting out a short quiet period keeps
// this to one write per burst of adjustment instead of one per wheel notch.
static float g_stateQuiet = 0;
static const float kStateFlushDelay = 2.0f;

static void SaveState(const Overlay* ov) {
    g_pendingStyle = ov->style;
    g_pendingAmount = ov->amount;
    g_pendingParam = (int)(ov->param * 1000.0f);
    g_stateDirty = true;
    g_stateQuiet = 0;   // each change restarts the quiet period
}

static void SavePalette(int index, int from) {
    g_pendingPalette = index;
    g_pendingPaletteFrom = from;
    g_paletteDirty = true;
    g_stateQuiet = 0;
}

static bool StatePending() {
    return g_stateDirty || g_paletteDirty;
}

static void FlushState() {
    if (g_stateDirty.exchange(false)) {
        Wh_SetIntValue(L"state.style", g_pendingStyle);
        Wh_SetIntValue(L"state.amount", g_pendingAmount);
        Wh_SetIntValue(L"state.param", g_pendingParam);
        // Stamp which setting values this state was derived from.
        Wh_SetIntValue(L"state.amountFrom", g_settings.amount);
        Wh_SetIntValue(L"state.paramFrom", g_settings.parameter);
    }
    if (g_paletteDirty.exchange(false)) {
        Wh_SetIntValue(L"state.palette", g_pendingPalette);
        Wh_SetIntValue(L"state.paletteFrom", g_pendingPaletteFrom);
    }
}

// Clicks fan out to every overlay, the way the rotation timer already does,
// so multiple displays stay in step instead of drifting apart.
static void Controller_CycleStyle(Overlay* ov) {
    int next = NextEnabledStyle(ov->style);
    // Choosing a style by hand restarts the rotation clock, so a click that
    // lands near the end of an interval is not rotated away a moment later.
    g_rotateTimer = 0;
    for (size_t i = 0; i < g_overlays.size(); i++) {
        g_overlays[i]->style = next;
        g_overlays[i]->NewScene();
        g_overlays[i]->FlashHud();
    }
    SaveState(ov);
}

static void Controller_StepAmount(Overlay* ov) {
    int next = (ov->amount + 1) % kAmountCount;
    for (size_t i = 0; i < g_overlays.size(); i++) {
        Overlay* o = g_overlays[i];
        o->amount = next;
        // Contours re-level in place; the others are structural, so rebuild.
        if (o->style != kStyleContour) {
            o->NewScene();
        }
        o->FlashHud();
    }
    SaveState(ov);
}

static void Controller_Wheel(Overlay* ov, int delta) {
    // Only the worker thread reaches this, so a plain static is enough.
    static int wheelAcc = 0;
    wheelAcc += delta;
    int notches = wheelAcc / WHEEL_DELTA;
    if (notches == 0) {
        return;
    }
    wheelAcc -= notches * WHEEL_DELTA;
    float next = ClampT(ov->param + 0.04f * notches, 0.0f, 1.0f);
    for (size_t i = 0; i < g_overlays.size(); i++) {
        g_overlays[i]->param = next;
        g_overlays[i]->FlashHud();
    }
    SaveState(ov);
}

// Space steps through the palettes, live, the way click steps the style.
// It used to slide the hue while held, which left the colors stranded at an
// arbitrary rotation with no way to get back to a named palette.
static void Controller_CyclePalette() {
    g_paletteIndex = (g_paletteIndex + 1) % kPaletteCount;
    BuildPalette();
    // Deferred, the way a wheel notch is. Stepped quickly this used to be two
    // writes per press going straight to the store, which is the sort of thing
    // that has no business happening several times a second.
    SavePalette(g_paletteIndex, PaletteIndexFromName(g_settings.palette));
    for (size_t i = 0; i < g_overlays.size(); i++) {
        g_overlays[i]->FlashHud();
    }
}



// The low level keyboard hook cannot call Controller_CyclePalette itself.
// BuildPalette clears and refills g_palette while the worker thread is inside
// Render reading it, g_overlays is owned by the worker, and a hook callback
// holds up every keystroke on the system until it returns, with Windows
// silently dropping the hook past LowLevelHooksTimeout. So the hook posts and
// the worker does the work, the same way Esc already does.
static void Controller_RequestPalette() {
    DWORD tid = g_workerThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_VSH_PALETTE, 0, 0)) {
        Wh_Log(L"PostThreadMessage(PALETTE) failed (%u)", GetLastError());
    }
}

static void Controller_RequestClose() {
    DWORD tid = g_workerThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_VSH_CLOSE, 0, 0)) {
        Wh_Log(L"PostThreadMessage(CLOSE) failed (%u)", GetLastError());
    }
}

// Monitor geometry changed; ask the worker to tear the overlays down and
// re-run the display selection from scratch.
static std::atomic<bool> g_rebuildQueued{false};

static void Controller_RequestRebuild() {
    // Every overlay gets its own WM_DISPLAYCHANGE, and each rebuild tears the
    // hook thread down and back up; collapse the burst into one.
    if (g_rebuildQueued.exchange(true)) {
        return;
    }
    DWORD tid = g_workerThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_VSH_REBUILD, 0, 0)) {
        g_rebuildQueued = false;
        Wh_Log(L"PostThreadMessage(REBUILD) failed (%u)", GetLastError());
    }
}

// A global low-level keyboard hook so the overlay can be reached when it does
// not own the keyboard focus. It never swallows anything, so normal typing is
// completely unaffected.
//
// Esc is taken plain: it is a one shot with a visible result, and an escape
// hatch that needs a chord is not much of an escape hatch. The palette key is
// not taken plain, because plain Space from anywhere means every space in
// every sentence you type steps the palette, silently and hundreds of times an
// hour. Globally it is Ctrl+Shift+Space; plain Space still works when the
// overlay itself has the focus, where you asked for it by clicking.
// The hook gets no repeat flag, so the key down is latched here to step once
// per physical press. It lives outside the callback because the hook can be
// torn down and reinstalled with the key still held.
static std::atomic<bool> g_spaceHeld{false};

static LRESULT CALLBACK LowLevelKbdProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_active) {
        KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;
        bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
        if (k->vkCode == VK_ESCAPE) {
            if (down) {
                Controller_RequestClose();
            }
        } else if (k->vkCode == VK_SPACE) {
            // The hook gets no repeat flag, so latch the key down ourselves
            // and step once per physical press.
            bool chord = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 &&
                         (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            if (down && chord && !g_spaceHeld) {
                g_spaceHeld = true;
                Controller_RequestPalette();
            } else if (up) {
                g_spaceHeld = false;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static HANDLE g_hookThread = nullptr;
static HANDLE g_hookReady = nullptr;
static std::atomic<DWORD> g_hookThreadId{0};

static DWORD WINAPI KbdHookThread(LPVOID) {
    // Force the message queue into existence before publishing the id:
    // PostThreadMessageW fails until the thread owns one.
    MSG seed;
    PeekMessageW(&seed, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_hookThreadId = GetCurrentThreadId();
    if (g_hookReady) {
        SetEvent(g_hookReady);   // id published and queue exists
    }

    HMODULE mod = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&LowLevelKbdProc, &mod);
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKbdProc, mod, 0);
    // Published here rather than beside CreateThread: the thread starting says
    // nothing about whether the hook itself took. Setting it early meant that
    // if this call failed, the window proc would still stand aside for a hook
    // that was not there, and Space would be dead in both places.
    g_kbdHookLive = hook != nullptr;
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed (%u)", GetLastError());
    }

    // Nothing but the pump lives on this thread, so the hook is always
    // serviced immediately no matter how long a frame takes.
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_VSH_QUIT) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    // The thread id is cleared by whoever joined this thread, not here: a
    // straggler clearing it would zero the id of the thread that replaced it.
    return 0;
}

static void InstallKbdHook() {
    if (g_hookThread) {
        return;
    }
    // A press held across a hide would otherwise leave this latched, and the
    // first press after the next show would be swallowed.
    g_spaceHeld = false;
    g_hookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookReady) {
        // Without it the uninstall path cannot know when the id is published,
        // so it could post nothing and then wait forever. Better to run
        // without the global keys than to risk hanging teardown.
        Wh_Log(L"CreateEvent failed (%u); global keys disabled",
               GetLastError());
        return;
    }
    g_hookThread = CreateThread(nullptr, 0, KbdHookThread, nullptr, 0, nullptr);
    if (!g_hookThread) {
        Wh_Log(L"Could not start the keyboard hook thread (%u)",
               GetLastError());
        CloseHandle(g_hookReady);
        g_hookReady = nullptr;
    }
}

static void UninstallKbdHook() {
    g_kbdHookLive = false;
    if (!g_hookThread) {
        return;
    }
    // Wait for the thread to publish its id before posting; otherwise the
    // post is skipped and the wait below never ends.
    if (g_hookReady) {
        WaitForSingleObject(g_hookReady, INFINITE);
    }
    if (!PostThreadMessageW(g_hookThreadId.load(), WM_VSH_QUIT, 0, 0)) {
        Wh_Log(L"PostThreadMessage to the hook thread failed (%u)",
               GetLastError());
    }
    // Bounded on purpose. Abandoning the thread leaves the hook installed, so
    // the wait is generous; but waiting forever on a post that may have failed
    // would hang the unload, and a hung unload takes Windhawk with it. Its
    // remaining work is only UnhookWindowsHookEx and a return, so five seconds
    // is far more than it can honestly need.
    if (WaitForSingleObject(g_hookThread, 5000) != WAIT_OBJECT_0) {
        // Deliberately leave g_hookThread set. Clearing it would let the next
        // InstallKbdHook start a second thread and a second WH_KEYBOARD_LL
        // hook while this one's is still live, and every press would then be
        // seen twice: Space would step two palettes at a time. Refusing to
        // install another is the safe failure, and the global keys are the
        // only thing lost until the process restarts.
        Wh_Log(L"Keyboard hook thread did not exit in time; leaving it in "
               L"place, global keys stay off for this session");
        return;
    }
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
    g_hookThreadId = 0;
    if (g_hookReady) {
        CloseHandle(g_hookReady);
        g_hookReady = nullptr;
    }
}

static void ApplyExecutionState() {
    if (g_active && g_settings.keepAwake) {
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED |
                                ES_DISPLAY_REQUIRED);
    } else {
        SetThreadExecutionState(ES_CONTINUOUS);
    }
}

static ID2D1Factory* g_factory = nullptr;

// The Display setting tells people to identify their screens from this list,
// so it is written at startup and again whenever the layout changes.
static void LogDisplays() {
    std::vector<MonitorEntry> mons = EnumerateMonitors();
    for (size_t i = 0; i < mons.size(); i++) {
        Wh_Log(L"Windows display %d: %s [%s], %dx%d at (%d,%d)%s",
               mons[i].winNum,
               mons[i].deviceName.empty() ? L"Unknown" : mons[i].deviceName.c_str(),
               mons[i].device.c_str(),
               (int)(mons[i].rect.right - mons[i].rect.left),
               (int)(mons[i].rect.bottom - mons[i].rect.top),
               (int)mons[i].rect.left, (int)mons[i].rect.top,
               mons[i].primary ? L" [primary]" : L"");
    }
}

// Which rectangles the overlay should cover, given the current settings and
// the displays actually attached. Pulled out of ShowOverlays so the rebuild
// path can ask the question without tearing anything down to find out.
static std::vector<RECT> ComputeTargetRects() {
    std::vector<RECT> targets;
    std::vector<MonitorEntry> mons = EnumerateMonitors();
    if (mons.empty()) {
        return targets;
    }

    auto targetRect = [&](const MonitorEntry& m) -> RECT {
        if (g_settings.workAreaOnly) {
            return m.work;
        }
        // One pixel short of the bottom edge, and here is why.
        //
        // The shell decides a fullscreen application is running by looking for
        // a foreground window whose rectangle covers the monitor, and Focus
        // Assist silences notifications when it finds one. Clicking the
        // overlay makes it the foreground window, so on the default settings
        // a single click would quietly turn the user's notifications off
        // until something else took focus.
        //
        // Measured on Windows 11 rather than assumed: a WS_POPUP window with
        // WS_EX_TOOLWINDOW covering the primary monitor exactly, once it is
        // foreground, moves SHQueryUserNotificationState from
        // QUNS_ACCEPTS_NOTIFICATIONS to QUNS_BUSY. One pixel short of the
        // monitor it stays at QUNS_ACCEPTS_NOTIFICATIONS. The tool window
        // style does not exempt it, and only the primary monitor counts.
        //
        // A pixel of wallpaper along the bottom edge is not something anyone
        // will notice in a piece of generative line art. Losing notifications
        // without being told is.
        RECT r = m.rect;
        if (r.bottom > r.top) {
            r.bottom -= 1;
        }
        return r;
    };
    if (g_settings.monitor == L"all") {
        for (size_t i = 0; i < mons.size(); i++) {
            targets.push_back(targetRect(mons[i]));
        }
    } else if (g_settings.monitor == L"primary") {
        for (size_t i = 0; i < mons.size(); i++) {
            if (mons[i].primary) {
                targets.push_back(targetRect(mons[i]));
                break;
            }
        }
        if (targets.empty()) {
            targets.push_back(targetRect(mons[0]));
        }
    } else {
        int win = _wtoi(g_settings.monitor.c_str());
        for (size_t i = 0; i < mons.size(); i++) {
            if (mons[i].winNum == win) {
                targets.push_back(targetRect(mons[i]));
                break;
            }
        }
        if (targets.empty()) {
            Wh_Log(L"Windows display %d not connected, falling back to primary",
                   win);
            for (size_t i = 0; i < mons.size(); i++) {
                if (mons[i].primary) {
                    targets.push_back(targetRect(mons[i]));
                    break;
                }
            }
            if (targets.empty()) {
                targets.push_back(targetRect(mons[0]));
            }
        }
    }
    return targets;
}

// What the overlays are covering right now.
static std::vector<RECT> CurrentOverlayRects() {
    std::vector<RECT> rects;
    for (size_t i = 0; i < g_overlays.size(); i++) {
        rects.push_back(g_overlays[i]->Rect());
    }
    return rects;
}

static bool SameRects(const std::vector<RECT>& a, const std::vector<RECT>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].left != b[i].left || a[i].top != b[i].top ||
            a[i].right != b[i].right || a[i].bottom != b[i].bottom) {
            return false;
        }
    }
    return true;
}

static void ShowOverlays() {
    if (g_active) {
        return;
    }
    if (!g_factory) {
        D2D1_FACTORY_OPTIONS opts;
        ZeroMemory(&opts, sizeof(opts));
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                     kIID_ID2D1Factory, &opts,
                                     (void**)&g_factory))) {
            Wh_Log(L"D2D1CreateFactory failed");
            return;
        }
    }
    if (!g_dwrite) {
        // Optional: without it the art still runs, just with no readout.
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                       kIID_IDWriteFactory,
                                       (IUnknown**)&g_dwrite))) {
            Wh_Log(L"DWriteCreateFactory failed; readout disabled");
            g_dwrite = nullptr;
        }
    }

    std::vector<RECT> targets = ComputeTargetRects();
    if (targets.empty()) {
        return;
    }

    g_presentImmediately = targets.size() > 1;

    int style = ClampT(Wh_GetIntValue(L"state.style", FirstEnabledStyle()), 0,
                       kStyleCount - 1);
    if (!g_settings.enable[style]) {
        style = FirstEnabledStyle();
    }
    // If the setting changed since the state was saved, the setting wins;
    // otherwise "Starting amount notch" and "Starting parameter" would be
    // silently ignored forever after the first scroll or right click.
    // Same rule as amount and parameter: a live change sticks, but editing
    // the setting overrides it, so the settings UI is never a dead control.
    int fromSetting = PaletteIndexFromName(g_settings.palette);
    g_paletteIndex = fromSetting;
    if (Wh_GetIntValue(L"state.paletteFrom", -1) == fromSetting) {
        g_paletteIndex =
            ClampT(Wh_GetIntValue(L"state.palette", fromSetting), 0,
                   kPaletteCount - 1);
    }
    BuildPalette();

    int amount = g_settings.amount;
    if (Wh_GetIntValue(L"state.amountFrom", -1) == g_settings.amount) {
        amount = Wh_GetIntValue(L"state.amount", g_settings.amount);
    }
    amount = ClampT(amount, 0, kAmountCount - 1);

    int paramMilli = g_settings.parameter * 10;
    if (Wh_GetIntValue(L"state.paramFrom", -1) == g_settings.parameter) {
        paramMilli = Wh_GetIntValue(L"state.param", paramMilli);
    }
    float param = ClampT(paramMilli, 0, 1000) / 1000.0f;

    unsigned seed = (unsigned)GetTickCount64();
    for (size_t i = 0; i < targets.size(); i++) {
        Overlay* ov = new Overlay(g_factory, targets[i], seed + (unsigned)i * 7919u);
        ov->style = style;
        ov->amount = amount;
        ov->param = param;
        if (ov->Create()) {
            g_overlays.push_back(ov);
        } else {
            delete ov;
        }
    }

    if (g_overlays.empty()) {
        return;
    }
    g_active = true;
    g_rotateTimer = 0;
    // Left set by a session that ended while something covered the overlay,
    // this would start the next one on the half second poll with its rotation
    // frozen, until the first render cleared it.
    g_allOccluded = false;
    if (g_settings.globalKeys) {
        InstallKbdHook();
    }
    ApplyExecutionState();
    Wh_Log(L"Screen Holder shown on %d display(s)", (int)g_overlays.size());
}

static void HideOverlays() {
    if (!g_active) {
        return;
    }
    UninstallKbdHook();
    FlushState();
    for (size_t i = 0; i < g_overlays.size(); i++) {
        delete g_overlays[i];
    }
    g_overlays.clear();
    g_active = false;
    ApplyExecutionState();
    Wh_Log(L"Screen Holder hidden");
}

static void ToggleOverlays() {
    if (g_active) {
        HideOverlays();
    } else {
        ShowOverlays();
    }
}

// ---------------------------------------------------------------------------
// Hotkey parsing
// ---------------------------------------------------------------------------
static bool ParseHotkey(const std::wstring& s, UINT* mods, UINT* vk) {
    if (s.empty()) {
        return false;
    }
    UINT m = 0;
    UINT key = 0;
    size_t start = 0;
    while (start <= s.size()) {
        size_t plus = s.find(L'+', start);
        std::wstring tok = s.substr(
            start, plus == std::wstring::npos ? std::wstring::npos
                                              : plus - start);
        // trim
        while (!tok.empty() && (tok.front() == L' ' || tok.front() == L'\t')) {
            tok.erase(tok.begin());
        }
        while (!tok.empty() && (tok.back() == L' ' || tok.back() == L'\t')) {
            tok.pop_back();
        }
        std::wstring low;
        for (size_t i = 0; i < tok.size(); i++) {
            low.push_back((wchar_t)towlower(tok[i]));
        }

        if (low == L"ctrl" || low == L"control") {
            m |= MOD_CONTROL;
        } else if (low == L"alt") {
            m |= MOD_ALT;
        } else if (low == L"shift") {
            m |= MOD_SHIFT;
        } else if (low == L"win" || low == L"windows") {
            m |= MOD_WIN;
        } else if (low.size() == 1) {
            wchar_t c = low[0];
            if (c >= L'a' && c <= L'z') {
                key = (UINT)(L'A' + (c - L'a'));
            } else if (c >= L'0' && c <= L'9') {
                key = (UINT)c;
            }
        } else if (low.size() >= 2 && low[0] == L'f') {
            int n = _wtoi(low.c_str() + 1);
            if (n >= 1 && n <= 24) {
                key = VK_F1 + (n - 1);
            }
        }

        if (plus == std::wstring::npos) {
            break;
        }
        start = plus + 1;
    }
    if (!key) {
        return false;
    }
    // A registered hotkey is consumed before the focused window ever sees it,
    // and this one stays registered for as long as the mod is loaded rather
    // than only while the overlay is up. So "A" on its own would cost the user
    // that letter everywhere, all session, with nothing on screen to say why.
    // Function keys are a fair thing to take bare; letters and digits are not.
    if (m == 0 && !(key >= VK_F1 && key <= VK_F24)) {
        // The caller reports the failure, with the rule, for every way of
        // getting here. Logging it again from the parser was the same line
        // twice for one bad string.
        return false;
    }
    *mods = m | MOD_NOREPEAT;
    *vk = key;
    return true;
}

// ---------------------------------------------------------------------------
// Named toggle event, so a desktop shortcut can drive the mod
// ---------------------------------------------------------------------------
static HANDLE CreateToggleEvent() {
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    PSECURITY_DESCRIPTOR psd = nullptr;

    // EVENT_MODIFY_STATE | SYNCHRONIZE (0x100002). SetEvent alone only needs
    // EVENT_MODIFY_STATE, but the documented shortcut calls
    // EventWaitHandle.OpenExisting(name), which asks for Modify | Synchronize
    // and is refused without the latter. Still far short of GENERIC_ALL, which
    // would also hand out DELETE / WRITE_DAC / WRITE_OWNER. The Medium
    // integrity label still lets an ordinary shortcut signal it when Windhawk
    // is elevated, while keeping sandboxed processes below that level out.
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:(A;;0x100002;;;IU)S:(ML;;NW;;;ME)", SDDL_REVISION_1, &psd,
            nullptr)) {
        sa.lpSecurityDescriptor = psd;
    }

    // Local\ (per session) so a second logged-in user's shortcut cannot
    // toggle this session's overlay.
    HANDLE h = CreateEventW(psd ? &sa : nullptr, FALSE, FALSE, kEventLocal);
    if (psd) {
        LocalFree(psd);
    }
    return h;
}

// ---------------------------------------------------------------------------
// Settings loading
// ---------------------------------------------------------------------------
static std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

// The amount notch is stored by name so the setting can carry $options.
static int AmountFromName(const std::wstring& name) {
    static const wchar_t* kNames[kAmountCount] = {L"minimal", L"sparse",
                                                  L"balanced", L"dense",
                                                  L"maximal"};
    for (int i = 0; i < kAmountCount; i++) {
        if (name == kNames[i]) {
            return i;
        }
    }
    return 2;   // balanced
}

static void LoadSettings() {
    g_settings.monitor = GetStringSetting(L"monitor");
    g_settings.enable[kStyleFlow] = Wh_GetIntSetting(L"enableFlow") != 0;
    g_settings.enable[kStyleContour] = Wh_GetIntSetting(L"enableContour") != 0;
    g_settings.enable[kStyleGrowth] = Wh_GetIntSetting(L"enableGrowth") != 0;
    g_settings.enable[kStyleHarmonograph] =
        Wh_GetIntSetting(L"enableHarmonograph") != 0;
    bool any = false;
    for (int i = 0; i < kStyleCount; i++) {
        any = any || g_settings.enable[i];
    }
    if (!any) {
        g_settings.enable[kStyleFlow] = true;   // never leave nothing to draw
    }

    g_settings.rotate = Wh_GetIntSetting(L"rotate") != 0;
    g_settings.rotateSeconds =
        ClampT(Wh_GetIntSetting(L"rotateSeconds"), 10, 7200);
    g_settings.amount = AmountFromName(GetStringSetting(L"amount"));
    g_settings.parameter = ClampT(Wh_GetIntSetting(L"parameter"), 0, 100);
    g_settings.palette = GetStringSetting(L"palette");
    g_settings.customColors = GetStringSetting(L"customColors");
    g_settings.customBackground = GetStringSetting(L"customBackground");
    g_settings.hueOffset = ClampT(Wh_GetIntSetting(L"hueOffset"), 0, 359);
    g_settings.colorRamp = Wh_GetIntSetting(L"colorRamp") != 0;
    g_settings.rampSpeed = ClampT(Wh_GetIntSetting(L"rampSpeed"), 1, 360);
    g_settings.fps = ClampT(Wh_GetIntSetting(L"fps"), 10, 240);
    g_settings.opacity = ClampT(Wh_GetIntSetting(L"opacity"), 10, 100);
    g_settings.globalKeys = Wh_GetIntSetting(L"globalKeys") != 0;
    g_settings.clickThrough = Wh_GetIntSetting(L"clickThrough") != 0;
    g_settings.keepAwake = Wh_GetIntSetting(L"keepAwake") != 0;
    g_settings.startActive = Wh_GetIntSetting(L"startActive") != 0;
    g_settings.workAreaOnly = Wh_GetIntSetting(L"workAreaOnly") != 0;
    g_settings.hotkey = GetStringSetting(L"hotkey");
    g_settings.language = GetStringSetting(L"language");
    SelectStrings(g_settings.language);

    BuildPalette();
}

static void RegisterHotkeyFromSettings() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    UINT mods = 0, vk = 0;
    if (ParseHotkey(g_settings.hotkey, &mods, &vk)) {
        if (RegisterHotKey(nullptr, kHotkeyId, mods, vk)) {
            g_hotkeyRegistered = true;
            Wh_Log(L"Hotkey registered: %s", g_settings.hotkey.c_str());
        } else {
            Wh_Log(L"RegisterHotKey failed for '%s' (%u)",
                   g_settings.hotkey.c_str(), GetLastError());
        }
    } else if (!g_settings.hotkey.empty()) {
        // An empty value is the documented way to turn the hotkey off, so
        // only a non-empty string that will not parse is worth reporting.
        Wh_Log(L"Could not parse the hotkey '%s'; no hotkey is registered. "
               L"Letters and digits need a modifier such as Ctrl or Alt; "
               L"function keys can stand on their own",
               g_settings.hotkey.c_str());
    }
}

// ---------------------------------------------------------------------------
// Worker thread: owns the windows, the render loop and the execution state
// ---------------------------------------------------------------------------
static std::atomic<bool> g_running{true};

static DWORD WINAPI WorkerThread(LPVOID) {
    // Same as the hook thread: the queue must exist before anyone can post.
    MSG seed;
    PeekMessageW(&seed, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_workerThreadId = GetCurrentThreadId();

    // Per-monitor DPI awareness on this thread only, so monitor rectangles are
    // reported in physical pixels regardless of the host's awareness.
    typedef HANDLE(WINAPI * SetThreadDpiAwarenessContext_t)(HANDLE);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        SetThreadDpiAwarenessContext_t fn =
            (SetThreadDpiAwarenessContext_t)GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (fn) {
            fn((HANDLE)-4);   // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        }
    }

    // The window procedure lives in the mod image, not in windhawk.exe, so
    // the class is registered against the mod's own instance and given back in
    // the teardown below rather than left behind.
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&Overlay::WndProc, &g_modInstance);
    if (!g_modInstance) {
        g_modInstance = GetModuleHandleW(nullptr);
    }

    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = Overlay::WndProc;
    wc.hInstance = g_modInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClass;
    bool classRegistered = RegisterClassExW(&wc) != 0;
    if (!classRegistered) {
        Wh_Log(L"RegisterClassEx failed (%u); no overlay can be created",
               GetLastError());
    }

    LoadSettings();

    LogDisplays();

    g_toggleEvent = CreateToggleEvent();
    if (!g_toggleEvent) {
        Wh_Log(L"Could not create the toggle event (%u)", GetLastError());
    }

    RegisterHotkeyFromSettings();

    if (g_settings.startActive) {
        ShowOverlays();
    }

    LARGE_INTEGER freq, lastRender;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastRender);

    // High resolution needs Windows 10 1803; fall back to an ordinary timer,
    // which is still far better than a quantized timeout.
    HANDLE frameTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!frameTimer) {
        frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    }
    if (!frameTimer) {
        Wh_Log(L"CreateWaitableTimer failed (%u); pacing will be coarse",
               GetLastError());
    }

    while (g_running) {
        HANDLE handles[3];
        DWORD count = 0;
        DWORD toggleIdx = (DWORD)-1;
        if (g_quitEvent) {
            handles[count++] = g_quitEvent;   // wakes the wait, nothing more
        }
        if (g_toggleEvent) {
            toggleIdx = count;
            handles[count++] = g_toggleEvent;
        }
        if (g_active && frameTimer) {
            LARGE_INTEGER nowW;
            QueryPerformanceCounter(&nowW);
            float sinceW = (float)(nowW.QuadPart - lastRender.QuadPart) /
                           (float)freq.QuadPart;
            float period = g_allOccluded ? 0.5f : 1.0f / (float)g_settings.fps;
            float remain = period - sinceW;
            LARGE_INTEGER due;
            // negative is relative, in 100 ns units
            due.QuadPart = remain <= 0.0f
                               ? -1LL
                               : -(LONGLONG)(remain * 10000000.0f);
            SetWaitableTimer(frameTimer, &due, 0, nullptr, nullptr, FALSE);
            // The wall-clock gate below decides whether to draw; the timer is
            // only here to make the wait end at the right moment.
            handles[count++] = frameTimer;
        }

        // Without a timer the wait must still end on its own, or the overlay
        // would advance only when a message happened to arrive.
        DWORD waitMs = (g_active && !frameTimer) ? 1 : INFINITE;
        DWORD r = MsgWaitForMultipleObjects(count, count ? handles : nullptr,
                                            FALSE, waitMs, QS_ALLINPUT);

        if (toggleIdx != (DWORD)-1 && r == WAIT_OBJECT_0 + toggleIdx) {
            ToggleOverlays();
        }

        // MsgWaitForMultipleObjects reports only the lowest signaled index and
        // the queue is always last, so any signaled handle hides pending
        // messages for that iteration. The frame timer is armed to fire
        // immediately whenever the loop is behind schedule, which is the
        // steady state as soon as a frame costs more than its budget: on a
        // single display EndDraw waits for the refresh, so any frame rate
        // above the panel's does it, and so does a heavy contour scene. The
        // queue is therefore drained unconditionally; the wait result decides
        // what woke us, not whether input is read at all.
        {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
                    ToggleOverlays();
                    continue;
                }
                if (msg.message == WM_VSH_SETTINGS) {
                    bool wasActive = g_active;
                    bool hadStartActive = g_settings.startActive;
                    HideOverlays();
                    LoadSettings();
                    RegisterHotkeyFromSettings();
                    // Whatever the user left on screen is what they get back.
                    // Carrying startActive into every settings change meant
                    // dismissing the overlay with Esc and then editing an
                    // unrelated setting brought it back. Ticking the box
                    // itself is the one case that should still open it.
                    if (wasActive || (g_settings.startActive && !hadStartActive)) {
                        ShowOverlays();
                    }
                    continue;
                }
                if (msg.message == WM_VSH_QUIT) {
                    g_running = false;
                    break;
                }
                if (msg.message == WM_VSH_CLOSE) {
                    HideOverlays();
                    continue;
                }
                if (msg.message == WM_VSH_PALETTE) {
                    Controller_CyclePalette();
                    continue;
                }
                if (msg.message == WM_VSH_REBUILD) {
                    g_rebuildQueued = false;
                    // Displays added, removed or resized. Rebuild so the
                    // overlay follows the new geometry rather than sitting at
                    // the old size on a display that may no longer exist.
                    //
                    // Only when something actually moved, though. A DPI change
                    // cannot move these rectangles at all, because the worker
                    // is per-monitor aware and the targets are already
                    // physical pixels; nor can a monitor waking, a game
                    // switching mode and back, or a taskbar toggle with the
                    // work area unused. This runs for hours, so a piece
                    // restarting for no visible reason is exactly what gets
                    // noticed.
                    if (g_active) {
                        // Re-logged because the Display setting tells people
                        // to identify their screens from this list, which is
                        // no help if it only ever describes the old layout.
                        LogDisplays();
                        if (!SameRects(ComputeTargetRects(),
                                       CurrentOverlayRects())) {
                            HideOverlays();
                            ShowOverlays();
                        }
                    }
                    continue;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        if (!g_running) {
            break;   // the quit message was drained above
        }

        if (!g_active) {
            QueryPerformanceCounter(&lastRender);
            continue;
        }

        // Only render when the frame clock says so. MsgWaitForMultipleObjects
        // wakes for input as well as for the timeout, so rendering on every
        // wake-up ignored the fps setting entirely and spun the GPU whenever
        // the mouse crossed the overlay.
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float since = (float)(now.QuadPart - lastRender.QuadPart) /
                      (float)freq.QuadPart;
        float gate = g_allOccluded ? 0.5f : 1.0f / (float)g_settings.fps;
        if (since < gate) {
            continue;
        }
        lastRender = now;
        float dt = since > 0.25f ? 0.25f : since;

        if (StatePending()) {
            g_stateQuiet += dt;
            if (g_stateQuiet >= kStateFlushDelay) {
                FlushState();
            }
        }

        // hue
        if (g_settings.colorRamp) {
            g_hue += (float)g_settings.rampSpeed * dt;
            g_hue = std::fmod(g_hue, 360.0f);
            if (g_hue < 0) {
                g_hue += 360.0f;
            }
        } else {
            // With the ramp off the color is whatever the user set, not
            // wherever a previous rotation happened to stop.
            g_hue = (float)g_settings.hueOffset;
        }

        // Frozen with everything else while nobody can see the display: this
        // clock used to keep running and build whole new scenes, contour
        // fields included, behind a fullscreen window.
        if (g_settings.rotate && !g_allOccluded) {
            g_rotateTimer += dt;
            if (g_rotateTimer >= (float)g_settings.rotateSeconds) {
                g_rotateTimer = 0;
                // No readout here on purpose: the overlay is meant to be
                // clean while it runs by itself, and only something you do
                // brings the line up.
                for (size_t i = 0; i < g_overlays.size(); i++) {
                    g_overlays[i]->style =
                        NextEnabledStyle(g_overlays[i]->style);
                    g_overlays[i]->NewScene();
                }
                if (!g_overlays.empty()) {
                    SaveState(g_overlays[0]);
                }
            }
        }

        bool allOccluded = !g_overlays.empty();
        for (size_t i = 0; i < g_overlays.size(); i++) {
            g_overlays[i]->Render(dt);
            allOccluded = allOccluded && g_overlays[i]->Occluded();
        }
        g_allOccluded = allOccluded;
    }

    HideOverlays();
    if (frameTimer) {
        CancelWaitableTimer(frameTimer);
        CloseHandle(frameTimer);
    }
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    if (g_toggleEvent) {
        CloseHandle(g_toggleEvent);
        g_toggleEvent = nullptr;
    }
    ReleaseEmbeddedFonts();
    g_hudFontResolved = false;
    g_hudFont = HudFont();
    if (classRegistered) {
        UnregisterClassW(kWindowClass, g_modInstance);
    }
    SafeRelease(&g_dwrite);
    SafeRelease(&g_factory);
    SetThreadExecutionState(ES_CONTINUOUS);
    return 0;
}

static HANDLE g_workerThread = nullptr;

BOOL WhTool_ModInit() {
    Wh_Log(L"Vector Screen Holder starting");
    g_running = true;
    g_quitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_quitEvent) {
        // Not fatal: the quit message is still posted below, this only removes
        // the fallback if that post ever fails.
        Wh_Log(L"CreateEvent for quit failed (%u)", GetLastError());
    }
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    DWORD tid = g_workerThreadId.load();
    if (!tid || !PostThreadMessageW(tid, WM_VSH_SETTINGS, 0, 0)) {
        Wh_Log(L"Could not deliver the settings change (%u)", GetLastError());
    }
}

void WhTool_ModUninit() {
    g_running = false;
    // The event first, because it cannot fail. The message is only for
    // promptness: it wakes the wait the same way but also drains cleanly.
    if (g_quitEvent) {
        SetEvent(g_quitEvent);
    }
    DWORD tid = g_workerThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_VSH_QUIT, 0, 0)) {
        Wh_Log(L"PostThreadMessage(QUIT) failed (%u)", GetLastError());
    }
    // Wh_ModUninit calls ExitProcess right after this. Without the join that
    // would terminate the worker inside Direct2D, which can wedge DLL
    // teardown and leave the tool-mod mutex held, making the next enable
    // fail with "Tool mod already running".
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_quitEvent) {
        CloseHandle(g_quitEvent);
        g_quitEvent = nullptr;
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
