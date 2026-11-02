// ==WindhawkMod==
// @id              quick-launch-dock
// @name            Quick Launch Dock
// @name:ru-RU      Панель быстрого запуска
// @description     A slim animated dock on the screen edge: drop a shortcut or pick a program with "+", then launch it in one click
// @description:ru-RU Тонкая анимированная панель у края экрана: перетащи ярлык или выбери программу через «+» и запускай в один клик
// @version         1.1.0
// @author          cheliks1123
// @github          https://github.com/cheliks1123
// @include         windhawk.exe
// @compilerOptions -lshell32 -lcomdlg32 -lgdi32 -luser32 -lole32 -loleaut32 -ldwmapi -lcomctl32 -ladvapi32 -lwininet
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Launch Dock

![Quick Launch Dock demo - dragging a shortcut onto the dock](https://raw.githubusercontent.com/cheliks1123/Quick-launch-dock/main/quck-lauch-dock.gif)

*Drag a shortcut onto the dock and it stays there. Click an icon to launch it.*

A slim, smoothly animated dock that lives on the edge of your screen and gives you
one-click access to your favourite programs, files and folders. Think of it as a
tiny launcher bar with "+" slots: drop a shortcut onto it, or click "+" and pick a
program, and it stays there until you remove it.

> ⚠️ **Website icons, please note:** not every website provides an icon, and the ones that do are not always in good quality. Many sites only offer a tiny 16×16 or 32×32 icon that can look blurry on a big dock, and some offer none at all. In that case a gray globe 🌐 stays in its place. You can always pick your own image with "Change icon" 🖼️.

## ✨ Features

- ➕ **Add programs your way**
  - Drag and drop shortcuts, executables, files or folders onto the dock.
  - Click the **+** slot to choose a program in a file dialog (shortcuts are kept as shortcuts, so their icons stay correct).
  - Right-click the **+** slot to choose a program, add a website, or paste a path or link from the clipboard (quotes and `%ENVIRONMENT%` variables are handled).
- 🌐 **Websites as slots** — right-click any icon and pick "Add to the dock", then "Add a website..." (or paste a link with "Paste a path or link"). A click opens the address in your default browser. Every site gets a gray globe icon. The optional "Download site icons" setting (off by default) lets the mod request the site's own /favicon.ico in the background and use it instead. A spinner is shown while the icon is downloading, and the globe stays if it cannot be downloaded. You can also pick your own image with "Change icon".
- 🚀 **One-click launch** — click an icon to start it. The working directory is set automatically.
- 🖱️ **Right-click menu** on any icon: Open, Run as administrator, Show in folder, Change icon / Reset icon, Move up / Move down, Add to the dock (choose a program, add a website, paste a path or link), Remove. For websites: Open, Change icon / Reset icon, Refresh site icon, Move up / Move down, Add to the dock, Remove.
- ↕️ **Reorder by dragging** — press and hold an icon, drag it along the dock and drop it where you want. The other icons move aside smoothly. You can also drop it onto another panel: while you drag, all panels slide out, so you can move an icon between panels. "Move up / Move down" in the menu still works too.
- 📐 **Dock size** — thickness of the dock, the space around every icon and the inner padding of the dock are set separately from the icon size.
- 🎨 **Custom icons** — pick any image (PNG, JPG, BMP, GIF or ICO) for an item from the right-click menu. The choice is remembered, and "Reset icon" brings back the original.
- 💾 **Persistent list** — your slots are saved by Windhawk and restored after a restart.
- 🙈 **Auto-hide** — the dock collapses into a thin strip and smoothly slides out when you move the mouse to the screen edge.
- 🎬 **Smooth animations** — slide-in/out, hover highlight fade, icon zoom on hover, a small press effect when you click an icon, appear/disappear and re-ordering animations. Frames are synchronized with your monitor's refresh rate, so it stays smooth on 60/144/240 Hz displays. You can also pick a fixed rate (120 to 360 Hz) in the settings.
- 💎 **High-quality rendering** — anti-aliased rounded corners and high-quality icon scaling (GDI+), per-pixel transparent window, DPI-aware sizes.
- 🎮 **Fullscreen aware** — optionally hides itself while a fullscreen app or game is active.
- 🌈 **Fully themeable** — background, hover, accent (plus sign and handle) and border colors are configurable (`#RRGGBB`).
- 🔮 **Panel opacity and glass style** — make the panel background translucent (icons stay fully opaque) and switch on a frosted-glass look with a soft highlight and a light border.
- 🌗 **Follows the Windows theme** — optionally takes the light or dark colors and the accent color from Windows, and updates when you switch the theme.
- 🔵 **Running indicator** — a small dot on the screen-edge side of an icon shows that its program is running.
- ✨ **Appear animation** — the dock slides in and fades in when it starts, and the icons pop in one after another when the dock slides out.
- 🎭 **Seven animation styles** — Slide and pop (default), Jelly (the dock overshoots and wobbles like jelly while the icons lag behind and squash), Ripple (a wave runs along the dock and lifts the icons as it passes), Fade (calm, no movement), Genie (the dock flows out of a point at the screen edge through a smooth funnel, like the macOS window effect), Oval (a small oval pops out of the edge, then unfolds into the dock) and Droplet (a drop leaves the edge on a thin neck, the neck snaps and the drop opens into the dock). Choose one in the settings.
- 🔎 **Icon magnification** — an optional macOS-style effect, independent of the animation style: icons grow smoothly around the cursor and the effect glides along the dock as you move the mouse.
- 🗂️ **Up to four panels** — turn on any combination of the right, left, top and bottom screen edges. Every panel keeps its own list of programs (for example one for apps and one for games), while appearance and animation settings are shared. Adjustable icon size, tooltips with the item name.
- 🛡️ Does not use the taskbar, does not hook any functions and does not modify system files. Disabling the mod removes the dock completely.

## 🆚 How it differs from existing mods

- **Quick Launch & Media Panel** is a collapsible panel at the top of the screen with shortcut pages and media controls. This mod is a minimal dock on any screen edge (left, right, top or bottom, in any combination): a row or column of icons and a "+" slot, with no pages and no media controls.
- **Left Taskbar Quick Pin Dock** is attached to the taskbar next to the Start button. This mod floats on the screen edge independently of the taskbar.
- What is specific to this mod: auto-hide into a thin strip that smoothly slides out on hover, hide-in-fullscreen, per-item "Run as administrator" and "Show in folder", and configurable colors.

## 📖 How to use

1. Enable the mod. A dock appears at the right edge of the primary monitor (vertically centered). Turn on more edges in the settings to get more panels.
2. Drag a shortcut/program/folder onto it, or click **+**.
3. Click an icon to launch. Right-click an icon for more actions. Hold an icon and drag it to change the order.
4. With auto-hide enabled, move the cursor to the screen edge to reveal the dock.

## ⚙️ Settings

| Setting | Description |
| --- | --- |
| Panel on the right / left / top / bottom edge | Turn on any combination of edges. Every panel keeps its own list of programs. If all four are off, the right panel is used. |
| Icon size | Icon size in pixels at 100% DPI (16–96). Scales with system DPI. |
| Panel thickness | Thickness of the dock in pixels at 100% DPI (0–300). 0 means automatic: just enough for the icon plus the paddings. A larger value makes the dock thicker and keeps the icons centered. A value that is too small is raised to the automatic size. |
| Space around each icon | Padding around every icon in pixels at 100% DPI (0–40). Sets the distance between icons and the size of the hover highlight. Default 8. |
| Panel inner padding | Padding between the dock border and its icons in pixels at 100% DPI (0–40). Default 4. |
| Corner radius | Roundness of the panel corners in pixels at 100% DPI (0-40). 0 makes the panel square. Default 12. |
| Download site icons | Off by default. When on, the mod requests /favicon.ico from the website of every website slot, in the background, and caches the file in the mod's storage folder. Only the sites you added are contacted and nothing is sent to third parties. If a site has no usable icon, it is not asked again for a week. If it just does not answer, the mod tries once more right away and again on the next start. |
| Auto-hide | Collapse to a thin strip until the cursor touches the edge. |
| Hide when a fullscreen app is active | Hides the dock above fullscreen windows/games. |
| Animations | Turns all animations on or off. |
| Animation style | How the dock appears and slides out of its strip: Slide and pop (default), Jelly, Ripple, Fade, Genie, Oval or Droplet. Jelly and Ripple play when the dock expands; it folds back with the plain slide. |
| Animation speed | Speed of all animations in percent. 100 is the original speed; the default is 25, which makes the differences between styles easy to see. |
| Animation refresh rate | Auto (default) draws one frame per monitor refresh. Pick 120, 144, 180, 240 or 360 Hz to draw at a fixed rate, for example on a laptop whose refresh rate changes or when animations look choppy. Higher values use more CPU and battery. |
| Show the "+" slot | Turn off to hide the "+" slot. Programs can still be added by dragging them onto the dock or from the "Add to the dock" item of the right-click menu of any icon. |
| Show a dot on running programs | Shows a small dot on the screen edge side of an icon whose program is running. Programs are matched by the file path of the program, so programs started through a launcher or an updater may not be detected. The process list is checked every few seconds while the dock is open. On by default. |
| Take the icon from the shortcut's target | Shows the icon of the program a shortcut points to instead of the icon stored in the shortcut. An icon chosen with "Change icon" always takes priority. |
| Icon magnification | macOS-style magnification of the icons around the cursor. Works with any animation style. Off by default. |
| Panel opacity | Opacity of the panel background, 10-100 percent. Icons always stay fully opaque. |
| Glass style | Frosted-glass look: tinted translucent panel, soft highlight and a light border. Works best with a lower panel opacity. This is a visual style, it does not blur what is behind the dock. |
| Background / Hover / Accent / Border color | Colors in `#RRGGBB` format. Invalid values fall back to the defaults. |

## 📝 Notes and limitations

- The mod runs as a standalone tool in its own dedicated process (it is not injected into Explorer and hooks nothing), so a problem in the mod can never affect the Windows shell, and only one instance is ever running.
- Drag and drop from windows running **as administrator** is blocked by Windows (UIPI). Use the **+** button in that case.
- The dock is placed on the **primary** monitor. With auto-hide and another monitor attached on the same side, the cursor may pass the edge too quickly to reveal the dock — disable auto-hide there.
- The number of slots is limited by the length of the screen edge, and room is left at the corners so that panels on neighbouring edges do not overlap.
- Slots are stored as file paths or web addresses (`http://` and `https://`). If a file is moved or deleted, its icon becomes generic until you remove it.
- The lists are stored separately for every Windows account. The first account that starts this version takes over the list saved by earlier versions.
- The mod makes no network requests unless "Download site icons" is on. In that case only the sites you added are contacted, one request each, and the icons are kept in the mod's storage folder, which Windhawk removes together with the mod.
- The list saved by earlier versions stays on the right-edge panel. Turning a panel off keeps its list, so it comes back when you turn the panel on again.

---

# Панель быстрого запуска

Тонкая плавно анимированная панель у края экрана для быстрого запуска любимых
программ, файлов и папок. По сути — мини-лаунчер с «плюсиками»: перетащи на него
ярлык или нажми «+» и выбери программу — она останется на панели, пока ты сам её не уберёшь.

> ⚠️ **Про иконки сайтов:** не все сайты отдают иконку, а те, что отдают, не всегда в хорошем качестве. У многих сайтов есть только крошечная иконка 16×16 или 32×32, которая на большой панели выглядит размыто, а у некоторых её нет совсем. Тогда на месте иконки остаётся серый глобус 🌐. Свою картинку всегда можно выбрать через «Сменить иконку» 🖼️.

## ✨ Возможности

- ➕ **Добавление как удобно**
  - Перетаскивание ярлыков, exe-файлов, файлов и папок прямо на панель.
  - Клик по **+** — выбор программы через диалог (ярлыки сохраняются как ярлыки, поэтому иконки остаются правильными).
  - ПКМ по **+** — выбрать программу, добавить сайт или вставить путь или ссылку из буфера обмена (кавычки и переменные вида `%APPDATA%` обрабатываются).
- 🌐 **Сайты в слотах** — ПКМ по любой иконке → «Добавить на панель» → «Добавить сайт…» (или вставь ссылку через «Вставить путь или ссылку»). Клик открывает адрес в браузере по умолчанию. Каждый сайт получает иконку в виде серого глобуса. Необязательная настройка «Скачивать иконки сайтов» (по умолчанию выключена) позволяет моду в фоне запросить /favicon.ico самого сайта и использовать его вместо глобуса. Пока иконка скачивается, показывается крутящийся кружок, а если скачать не удалось, остаётся глобус. Свою картинку можно выбрать через «Сменить иконку».
- 🚀 **Запуск в один клик** — рабочая папка выставляется автоматически.
- 🖱️ **Контекстное меню** на любой иконке: Открыть, Запуск от имени администратора, Показать в папке, Сменить иконку / Сбросить иконку, Переместить выше / ниже, Добавить на панель (выбрать программу, добавить сайт, вставить путь или ссылку), Удалить. Для сайтов: Открыть, Сменить иконку / Сбросить иконку, Обновить иконку сайта, Переместить выше / ниже, Добавить на панель, Удалить.
- ↕️ **Перестановка перетаскиванием** — зажми иконку, перетащи вдоль панели и отпусти там, где нужно. Остальные иконки плавно раздвигаются. Можно отпустить и на другой панели: пока тащишь, все панели выезжают, так что иконку можно переносить между панелями. «Переместить выше / ниже» в меню тоже остаются.
- 📐 **Размер панели** — толщина панели, отступ вокруг каждой иконки и внутренний отступ панели настраиваются отдельно от размера иконок.
- 🎨 **Свои иконки** — для любого элемента через меню можно выбрать картинку (PNG, JPG, BMP, GIF или ICO). Выбор запоминается, а «Сбросить иконку» возвращает исходную.
- 💾 **Список сохраняется** — слоты хранятся в Windhawk и восстанавливаются после перезагрузки.
- 🙈 **Автоскрытие** — в покое панель сворачивается в тонкую полоску и плавно выезжает, когда подводишь курсор к краю экрана.
- 🎬 **Плавные анимации** — выезд и сворачивание, плавная подсветка, увеличение иконки при наведении, лёгкий эффект нажатия на иконку, анимации появления, удаления и перестановки. Кадры синхронизированы с частотой монитора, поэтому всё плавно на 60/144/240 Гц. В настройках можно выбрать и фиксированную частоту (от 120 до 360 Гц).
- 💎 **Качественная отрисовка** — сглаженные скруглённые углы и качественное масштабирование иконок (GDI+), окно с попиксельной прозрачностью, учёт DPI.
- 🎮 **Полноэкранные приложения** — при желании панель сама прячется, пока открыта полноэкранная программа или игра.
- 🌈 **Настраиваемые цвета** — фон, подсветка, акцент (плюс и полоска) и рамка задаются в формате `#RRGGBB`.
- 🔮 **Прозрачность панели и стеклянный стиль** — фон панели можно сделать полупрозрачным (иконки остаются полностью непрозрачными) и включить вид матового стекла с мягким бликом и светлой рамкой.
- 🌗 **Тема Windows** — по желанию берёт светлые или тёмные цвета и цвет акцента из Windows и обновляется при смене темы.
- 🔵 **Индикатор запуска** — маленькая точка со стороны края экрана показывает, что программа запущена.
- ✨ **Анимация появления** — при запуске панель выезжает с плавным проявлением, а при выезде из полоски иконки по очереди «выпрыгивают».
- 🎭 **Семь стилей анимации** — Slide and pop (по умолчанию), Jelly (панель «перелетает» и дрожит как желе, а иконки запаздывают и сплющиваются), Ripple (по панели пробегает волна и приподнимает иконки), Fade (спокойное проявление без движения), Genie (панель вытекает из точки у края экрана плавной воронкой, как эффект окон в macOS), Oval (у края выскакивает небольшой овал, который затем раскрывается в панель) и Droplet (капля отрывается от края на тонкой «ножке», ножка рвётся, и капля раскрывается в панель). Выбирается в настройках.
- 🔎 **Увеличение иконок при наведении** — необязательный эффект как в macOS, не зависящий от стиля анимации: иконки плавно увеличиваются вокруг курсора, и эффект скользит вдоль панели за мышью.
- 🗂️ **До четырёх панелей** — включай любое сочетание правого, левого, верхнего и нижнего краёв экрана. У каждой панели свой список программ (например, одна для приложений, другая для игр), а внешний вид и анимации общие. Настраиваемый размер иконок, подсказки с названием элемента.
- 🛡️ Не использует панель задач, не перехватывает функции и не меняет системные файлы. При отключении мода панель полностью исчезает.

## 🆚 Чем отличается от существующих модов

- **Quick Launch & Media Panel** — сворачиваемая панель у верхнего края экрана со страницами ярлыков и управлением медиа. Этот мод — минимальная панель у любого края экрана (левого, правого, верхнего или нижнего, в любом сочетании): ряд или колонка иконок и слот «+», без страниц и без медиа-кнопок.
- **Left Taskbar Quick Pin Dock** — панель, прикреплённая к панели задач рядом с кнопкой «Пуск». Этот мод свободно располагается у края экрана и от панели задач не зависит.
- Особенности этого мода: автоскрытие в тонкую полоску с плавным выездом при наведении, скрытие в полноэкранных приложениях, «Запуск от имени администратора» и «Показать в папке» для каждого элемента, настраиваемые цвета.

## 📖 Как пользоваться

1. Включи мод. У правого края основного монитора по центру по вертикали появится панель. Чтобы получить больше панелей, включи другие края в настройках.
2. Перетащи на неё ярлык, программу или папку либо нажми **+**.
3. Клик по иконке — запуск. ПКМ по иконке — дополнительные действия. Зажми иконку и перетащи, чтобы поменять порядок.
4. При включённом автоскрытии подведи курсор к краю экрана, чтобы показать панель.

## ⚙️ Настройки

| Настройка | Описание |
| --- | --- |
| Панель у правого / левого / верхнего / нижнего края | Включай любое сочетание краёв. У каждой панели свой список программ. Если выключены все четыре, используется правая панель. |
| Размер иконок | Размер в пикселях при 100% DPI (16–96), масштабируется вместе с DPI системы. |
| Толщина панели | Толщина панели в пикселях при 100% DPI (0–300). 0 — автоматически: ровно под иконку и отступы. Большее значение делает панель толще, иконки остаются по центру. Слишком маленькое значение повышается до автоматического. |
| Отступ вокруг иконки | Отступ вокруг каждой иконки в пикселях при 100% DPI (0–40). Задаёт расстояние между иконками и размер подсветки при наведении. По умолчанию 8. |
| Внутренний отступ панели | Отступ между рамкой панели и иконками в пикселях при 100% DPI (0–40). По умолчанию 4. |
| Скругление углов | Скругление углов панели в пикселях при 100% DPI (0–40). 0 — квадратные углы. По умолчанию 12. |
| Скачивать иконки сайтов | По умолчанию выключено. Если включить, мод в фоне запрашивает /favicon.ico у сайта каждого слота-сайта и кэширует файл в папке хранилища мода. Обращения идут только к добавленным вами сайтам, третьим лицам ничего не отправляется. Если у сайта нет подходящей иконки, повторный запрос будет не раньше чем через неделю. Если сайт просто не ответил, мод сразу пробует ещё раз и снова при следующем запуске. |
| Автоскрытие | Сворачивать в тонкую полоску, пока курсор не коснётся края. |
| Прятать в полноэкранных приложениях | Скрывает панель поверх полноэкранных окон и игр. |
| Анимации | Включает и выключает все анимации. |
| Стиль анимации | Как панель появляется и выезжает из полоски: Slide and pop (по умолчанию), Jelly, Ripple, Fade, Genie, Oval или Droplet. Jelly и Ripple проигрываются при выезде панели, сворачивается она обычным скольжением. |
| Скорость анимации | Скорость всех анимаций в процентах. 100 — исходная скорость; по умолчанию 25, так разницу между стилями хорошо видно. |
| Частота обновления анимации | Авто (по умолчанию) рисует один кадр на каждое обновление монитора. Выбери 120, 144, 180, 240 или 360 Гц, чтобы рисовать с фиксированной частотой, например на ноутбуке с меняющейся частотой обновления или если анимации дёргаются. Чем выше значение, тем больше нагрузка на процессор и батарею. |
| Показывать слот «+» | Выключи, чтобы скрыть слот «+». Программы по-прежнему можно добавлять перетаскиванием на панель или через пункт «Добавить на панель» в меню правого клика на любой иконке. |
| Точка у запущенных программ | Показывает маленькую точку со стороны края экрана у иконки запущенной программы. Программы определяются по пути к файлу, поэтому запущенные через лаунчер или апдейтер могут не определиться. Пока панель открыта, список процессов проверяется раз в несколько секунд. По умолчанию включено. |
| Брать иконку из цели ярлыка | Показывает иконку программы, на которую указывает ярлык, а не иконку, сохранённую в самом ярлыке. Иконка, выбранная через «Сменить иконку», всегда в приоритете. |
| Увеличение иконок | Увеличение иконок вокруг курсора как в macOS. Работает с любым стилем анимации. По умолчанию выключено. |
| Прозрачность панели | Прозрачность фона панели, от 10 до 100 процентов. Иконки всегда остаются полностью непрозрачными. |
| Стеклянный стиль | Вид матового стекла: тонированная полупрозрачная панель, мягкий блик и светлая рамка. Лучше всего сочетается с пониженной прозрачностью. Это визуальный стиль, он не размывает то, что находится за панелью. |
| Цвета фона / подсветки / акцента / рамки | Формат `#RRGGBB`. При неверном значении используется цвет по умолчанию. |

## Примечания и ограничения

- Мод работает как самостоятельный инструмент в собственном процессе (он не внедряется в проводник и ничего не перехватывает), поэтому сбой мода не может повлиять на оболочку Windows, а одновременно запущена всегда только одна копия.
- Перетаскивание из окон, запущенных **от имени администратора**, блокируется Windows (UIPI). В этом случае используй кнопку **+**.
- Панель располагается на **основном** мониторе. При автоскрытии и втором мониторе с той же стороны курсор может проскакивать край слишком быстро — в таком случае отключи автоскрытие.
- Количество слотов ограничено длиной края экрана, а у углов оставлено место, чтобы панели на соседних краях не перекрывались.
- Слоты хранятся как пути к файлам или веб-адреса (`http://` и `https://`). Если файл перенесён или удалён, иконка станет стандартной, пока ты не уберёшь слот.
- Списки хранятся отдельно для каждой учётной записи Windows. Список, сохранённый прошлыми версиями, забирает первая учётная запись, которая запустит эту версию.
- Мод не делает сетевых запросов, пока не включена настройка «Скачивать иконки сайтов». Если она включена, обращения идут только к добавленным вами сайтам, по одному запросу на сайт, а иконки хранятся в папке хранилища мода, которую Windhawk удаляет вместе с модом.
- Список, сохранённый прежними версиями, остаётся на панели у правого края. При выключении панели её список сохраняется и возвращается при повторном включении.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- panelRight: true
  $name: Panel on the right edge
  $name:ru-RU: Панель у правого края
  $description: Turn on any combination of edges. Every panel keeps its own list of programs
  $description:ru-RU: Можно включить любое сочетание краёв. У каждой панели свой список программ
- panelLeft: false
  $name: Panel on the left edge
  $name:ru-RU: Панель у левого края
- panelTop: false
  $name: Panel on the top edge
  $name:ru-RU: Панель у верхнего края
- panelBottom: false
  $name: Panel on the bottom edge
  $name:ru-RU: Панель у нижнего края
- iconSize: 36
  $name: Icon size (px at 100% DPI)
  $name:ru-RU: Размер иконок (px при 100% DPI)
- panelThickness: 0
  $name: Panel thickness (px at 100% DPI, 0 = automatic)
  $name:ru-RU: Толщина панели (px при 100% DPI, 0 = авто)
  $description: Thickness of the dock. 0 fits the icons and paddings exactly. A larger value makes the dock thicker and keeps the icons centered
  $description:ru-RU: Толщина панели. 0 - ровно под иконки и отступы. Большее значение делает панель толще, иконки остаются по центру
- iconPadding: 8
  $name: Space around each icon (px at 100% DPI)
  $name:ru-RU: Отступ вокруг иконки (px при 100% DPI)
  $description: Sets the distance between icons and the size of the hover highlight
  $description:ru-RU: Задаёт расстояние между иконками и размер подсветки при наведении
- panelPadding: 4
  $name: Panel inner padding (px at 100% DPI)
  $name:ru-RU: Внутренний отступ панели (px при 100% DPI)
  $description: Padding between the dock border and its icons
  $description:ru-RU: Отступ между рамкой панели и иконками
- cornerRadius: 12
  $name: Corner radius (px at 100% DPI)
  $name:ru-RU: Скругление углов (px при 100% DPI)
  $description: 0 makes the panel square, larger values make it rounder
  $description:ru-RU: 0 - квадратные углы, чем больше значение, тем круглее
- fetchFavicons: false
  $name: Download site icons
  $name:ru-RU: Скачивать иконки сайтов
  $description: Off by default. When enabled, the mod requests /favicon.ico from the website of each website slot in the background and caches it in the mod storage folder. Only the sites you added are contacted. Without it, a gray globe is used
  $description:ru-RU: По умолчанию выключено. Если включить, мод в фоне запрашивает /favicon.ico у сайта каждого слота-сайта и кэширует его в папке хранилища мода. Обращения идут только к добавленным вами сайтам. Без этого используется серый глобус
- autoHide: true
  $name: Auto-hide (thin strip until hovered)
  $name:ru-RU: Автоскрытие (тонкая полоска до наведения)
- hideInFullscreen: true
  $name: Hide when a fullscreen app is active
  $name:ru-RU: Прятать в полноэкранных приложениях
- animations: true
  $name: Animations
  $name:ru-RU: Анимации
- followTheme: false
  $name: Follow the Windows theme
  $name:ru-RU: Следовать теме Windows
  $description: Takes the light or dark colors and the accent color from Windows and updates when you switch the theme. The color settings below are ignored while this is on
  $description:ru-RU: Берёт светлые или тёмные цвета и цвет акцента из Windows и обновляется при смене темы. Пока включено, настройки цветов ниже не действуют
- bgColor: "#202020"
  $name: Background color (#RRGGBB)
  $name:ru-RU: Цвет фона (#RRGGBB)
- hoverColor: "#424242"
  $name: Hover highlight color (#RRGGBB)
  $name:ru-RU: Цвет подсветки при наведении (#RRGGBB)
- accentColor: "#BEBEBE"
  $name: Accent color - "+" sign and handle (#RRGGBB)
  $name:ru-RU: Акцентный цвет - плюс и полоска (#RRGGBB)
- borderColor: "#464646"
  $name: Border color (#RRGGBB)
  $name:ru-RU: Цвет рамки (#RRGGBB)
- animationStyle: slide
  $name: Animation style
  $name:ru-RU: Стиль анимации
  $description: How the dock appears and slides out of its strip
  $description:ru-RU: Как панель появляется и выезжает из полоски
  $options:
  - slide: Slide and pop
  - jelly: Jelly
  - ripple: Ripple
  - fade: Fade
  - genie: Genie
  - oval: Oval
  - drop: Droplet
- opacity: 100
  $name: Panel opacity (10-100 percent)
  $name:ru-RU: Прозрачность панели (10-100 процентов)
  $description: Opacity of the panel background. Icons always stay fully opaque
  $description:ru-RU: Прозрачность фона панели. Иконки всегда остаются полностью непрозрачными
- glass: false
  $name: Glass style
  $name:ru-RU: Стеклянный стиль
  $description: Frosted-glass look - a tinted translucent panel with a soft highlight and a light border. Works best together with a lower panel opacity
  $description:ru-RU: Вид матового стекла - тонированная полупрозрачная панель с мягким бликом и светлой рамкой. Лучше всего сочетается с пониженной непрозрачностью панели
- animationSpeed: 25
  $name: Animation speed (percent)
  $name:ru-RU: Скорость анимации (в процентах)
  $description: 100 is the original speed, lower is slower. Applies to every animation style
  $description:ru-RU: 100 - исходная скорость, меньше - медленнее. Действует на все стили анимации
- refreshRate: auto
  $name: Animation refresh rate
  $name:ru-RU: Частота обновления анимации
  $description: Auto draws one frame per monitor refresh. Pick a fixed rate if animations look choppy, for example on a laptop whose refresh rate changes. Higher values use more CPU and battery
  $description:ru-RU: Авто рисует один кадр на каждое обновление монитора. Выбери фиксированную частоту, если анимации дёргаются, например на ноутбуке с меняющейся частотой обновления. Чем выше значение, тем больше нагрузка на процессор и батарею
  $options:
  - auto: Auto
  - hz120: 120 Hz
  - hz144: 144 Hz
  - hz180: 180 Hz
  - hz240: 240 Hz
  - hz360: 360 Hz
- showPlus: true
  $name: Show the "+" slot
  $name:ru-RU: Показывать слот «+»
  $description: Turn off to hide it. Programs can still be added by dragging them onto the dock or from the Add to the dock item of the right-click menu of any icon. With an empty list the slot is always shown
  $description:ru-RU: Выключи, чтобы скрыть. Программы по-прежнему можно добавлять перетаскиванием на панель или через пункт «Добавить на панель» в меню правого клика на любой иконке. При пустом списке слот показывается всегда
- showRunning: true
  $name: Show a dot on running programs
  $name:ru-RU: Точка у запущенных программ
  $description: Shows a small dot on the screen edge side of an icon whose program is running. The process list is checked every few seconds while the dock is open
  $description:ru-RU: Показывает маленькую точку со стороны края экрана у иконки запущенной программы. Пока панель открыта, список процессов проверяется раз в несколько секунд
- useTargetIcon: true
  $name: Take the icon from the shortcut's target
  $name:ru-RU: Брать иконку из цели ярлыка
  $description: Shows the icon of the program a shortcut points to instead of the icon stored in the shortcut itself. An icon you choose with "Change icon" always takes priority
  $description:ru-RU: Показывает иконку программы, на которую указывает ярлык, а не иконку, сохранённую в самом ярлыке. Иконка, выбранная через «Сменить иконку», всегда в приоритете
- magnify: false
  $name: Icon magnification on hover
  $name:ru-RU: Увеличение иконок при наведении
  $description: macOS-style magnification - the icons grow smoothly around the cursor and the effect follows it along the dock. Independent of the animation style
  $description:ru-RU: Как в macOS - иконки плавно увеличиваются вокруг курсора, эффект следует за ним вдоль панели. Не зависит от стиля анимации
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shldisp.h>
#include <servprov.h>
#include <commdlg.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <sddl.h>
#include <tlhelp32.h>
#include <wininet.h>
#include <math.h>
#include <stddef.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

struct GpPointF {
    float x, y;
};

struct GpStartupInput {
    UINT32 version;
    void* debugCallback;
    BOOL suppressBackgroundThread;
    BOOL suppressExternalCodecs;
};

static struct {
    HMODULE mod = nullptr;
    ULONG_PTR token = 0;
    bool ok = false;

    int(WINAPI* Startup)(ULONG_PTR*, const GpStartupInput*, void*);
    void(WINAPI* Shutdown)(ULONG_PTR);
    int(WINAPI* CreateBitmapFromScan0)(int, int, int, int, BYTE*, void**);
    int(WINAPI* GetImageGraphicsContext)(void*, void**);
    int(WINAPI* DeleteGraphics)(void*);
    int(WINAPI* SetSmoothing)(void*, int);
    int(WINAPI* SetInterp)(void*, int);
    int(WINAPI* SetPixOff)(void*, int);
    int(WINAPI* Clear)(void*, UINT32);
    int(WINAPI* CreateFill)(UINT32, void**);
    int(WINAPI* DelBrush)(void*);
    int(WINAPI* CreatePath)(int, void**);
    int(WINAPI* DeletePath)(void*);
    int(WINAPI* AddArc)(void*, float, float, float, float, float, float);
    int(WINAPI* ClosePathFig)(void*);
    int(WINAPI* FillPath)(void*, void*, void*);
    int(WINAPI* DrawPath)(void*, void*, void*);
    int(WINAPI* CreatePen)(UINT32, float, int, void**);
    int(WINAPI* DelPen)(void*);
    int(WINAPI* SetPenStartCap)(void*, int);
    int(WINAPI* SetPenEndCap)(void*, int);
    int(WINAPI* DrawLine)(void*, void*, float, float, float, float);
    int(WINAPI* BitmapFromHICON)(HICON, void**);
    int(WINAPI* DisposeImage)(void*);
    int(WINAPI* DrawImageRect)(void*, void*, float, float, float, float);
    int(WINAPI* GetImageWidth)(void*, UINT*);
    int(WINAPI* GetImageHeight)(void*, UINT*);
    int(WINAPI* SetClipPath)(void*, void*, int);
    int(WINAPI* AddPathLine)(void*, float, float, float, float);
    int(WINAPI* AddPathBezier)(void*, float, float, float, float, float, float, float, float);
    int(WINAPI* AddPathEllipse)(void*, float, float, float, float);
    int(WINAPI* ResetClip)(void*);
    int(WINAPI* TranslateWorld)(void*, float, float, int);
    int(WINAPI* ResetWorld)(void*);
    int(WINAPI* DrawImageRectRect)(void*, void*, float, float, float, float, float, float, float,
                                   float, int, void*, void*, void*);
    int(WINAPI* CreateImageAttributes)(void**);
    int(WINAPI* DisposeImageAttributes)(void*);
    int(WINAPI* SetImageAttrColorMatrix)(void*, int, BOOL, const void*, const void*, int);
    int(WINAPI* SetImageAttrWrapMode)(void*, int, UINT32, BOOL);
    int(WINAPI* BitmapFromFile)(PCWSTR, void**);
    int(WINAPI* BitmapGetPixel)(void*, int, int, UINT32*);
    int(WINAPI* CreateLineBrush)(const void*, const void*, UINT32, UINT32, int, void**);
} Gp;

#define GP_LOAD(field, sym)                                                                  \
    Gp.field = reinterpret_cast<decltype(Gp.field)>((void*)GetProcAddress(Gp.mod, sym));     \
    if (!Gp.field) return false;

static bool GpInit() {
    Gp.mod = LoadLibraryW(L"gdiplus.dll");
    if (!Gp.mod) return false;
    GP_LOAD(Startup, "GdiplusStartup");
    GP_LOAD(Shutdown, "GdiplusShutdown");
    GP_LOAD(CreateBitmapFromScan0, "GdipCreateBitmapFromScan0");
    GP_LOAD(GetImageGraphicsContext, "GdipGetImageGraphicsContext");
    GP_LOAD(DeleteGraphics, "GdipDeleteGraphics");
    GP_LOAD(SetSmoothing, "GdipSetSmoothingMode");
    GP_LOAD(SetInterp, "GdipSetInterpolationMode");
    GP_LOAD(SetPixOff, "GdipSetPixelOffsetMode");
    GP_LOAD(Clear, "GdipGraphicsClear");
    GP_LOAD(CreateFill, "GdipCreateSolidFill");
    GP_LOAD(DelBrush, "GdipDeleteBrush");
    GP_LOAD(CreatePath, "GdipCreatePath");
    GP_LOAD(DeletePath, "GdipDeletePath");
    GP_LOAD(AddArc, "GdipAddPathArc");
    GP_LOAD(ClosePathFig, "GdipClosePathFigure");
    GP_LOAD(FillPath, "GdipFillPath");
    GP_LOAD(DrawPath, "GdipDrawPath");
    GP_LOAD(CreatePen, "GdipCreatePen1");
    GP_LOAD(DelPen, "GdipDeletePen");
    GP_LOAD(SetPenStartCap, "GdipSetPenStartCap");
    GP_LOAD(SetPenEndCap, "GdipSetPenEndCap");
    GP_LOAD(DrawLine, "GdipDrawLine");
    GP_LOAD(BitmapFromHICON, "GdipCreateBitmapFromHICON");
    GP_LOAD(DisposeImage, "GdipDisposeImage");
    GP_LOAD(DrawImageRect, "GdipDrawImageRect");
    GP_LOAD(GetImageWidth, "GdipGetImageWidth");
    GP_LOAD(GetImageHeight, "GdipGetImageHeight");
    GP_LOAD(SetClipPath, "GdipSetClipPath");
    GP_LOAD(AddPathLine, "GdipAddPathLine");
    GP_LOAD(AddPathBezier, "GdipAddPathBezier");
    GP_LOAD(AddPathEllipse, "GdipAddPathEllipse");
    GP_LOAD(ResetClip, "GdipResetClip");
    GP_LOAD(TranslateWorld, "GdipTranslateWorldTransform");
    GP_LOAD(ResetWorld, "GdipResetWorldTransform");
    GP_LOAD(DrawImageRectRect, "GdipDrawImageRectRect");
    GP_LOAD(CreateImageAttributes, "GdipCreateImageAttributes");
    GP_LOAD(DisposeImageAttributes, "GdipDisposeImageAttributes");
    GP_LOAD(SetImageAttrColorMatrix, "GdipSetImageAttributesColorMatrix");
    GP_LOAD(SetImageAttrWrapMode, "GdipSetImageAttributesWrapMode");
    GP_LOAD(BitmapFromFile, "GdipCreateBitmapFromFile");
    GP_LOAD(BitmapGetPixel, "GdipBitmapGetPixel");
    GP_LOAD(CreateLineBrush, "GdipCreateLineBrush");

    GpStartupInput in{1, nullptr, FALSE, FALSE};
    if (Gp.Startup(&Gp.token, &in, nullptr) != 0) return false;
    Gp.ok = true;
    return true;
}

static UINT32 Argb(COLORREF c, int a) {
    a = std::clamp(a, 0, 255);
    return ((UINT32)a << 24) | ((UINT32)GetRValue(c) << 16) | ((UINT32)GetGValue(c) << 8) |
           (UINT32)GetBValue(c);
}

static COLORREF Mix(COLORREF a, COLORREF b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    int r = (int)(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t);
    int g = (int)(GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t);
    int bl = (int)(GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t);
    return RGB(r, g, bl);
}

static float SmoothStep(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

static float JellySpring(float x) {
    x = std::max(0.0f, x);
    return 1.0f - expf(-5.5f * x) * cosf(15.0f * x);
}

static float LinearT(float p) {
    p = std::clamp(p, 0.0f, 0.9955f);
    return std::clamp(-logf(1.0f - p) / 5.4f, 0.0f, 1.0f);
}

struct Item {
    std::wstring path;
    std::wstring name;
    void* img = nullptr;
    void* imgBig = nullptr;
    std::wstring iconPath;
    float pos = 0;
    float hover = 0;
    float appear = 1;
    bool loading = false;
    std::wstring exeKey;
    std::wstring exeName;
    bool running = false;
    float runA = 0;
    float pressA = 0;
};

struct PlusAnim {
    float pos = 0;
    float hover = 0;
};

struct Surf {
    HDC dc = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ old = nullptr;
    int w = 0, h = 0;
    void* bmp = nullptr;
    void* gfx = nullptr;
};

struct Panel {
    int edge = 0;
    bool horiz = false;
    bool atMax = true;

    std::vector<Item> items;
    PlusAnim plus;
    HWND hwnd = nullptr;
    HWND tip = nullptr;

    float intro = 1.0f;
    float magY = 0.0f;
    float magAmt = 0.0f;
    float mouseY = -1.0f;
    bool expanded = true;
    bool tracking = false;
    int hover = -1;
    int pressIdx = -1;
    POINT pressPt{0, 0};
    int outsideTicks = 0;
    int toolCount = 0;

    double curW = 0, curH = 0, tgtW = 0, tgtH = 0;
    bool animRunning = false;
    double lastT = 0;

    HDC memDc = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    int dibW = 0, dibH = 0;
    void* surf = nullptr;
    void* gfx = nullptr;
    Surf warp;
};

static HANDLE g_thread = nullptr;
static volatile bool g_stop = false;
static DWORD g_threadId = 0;
static bool g_modal = false;
static int g_dpi = 96;
static std::atomic<HWND> g_ctrl{nullptr};
static std::vector<Panel*> g_panels;
static Panel* P = nullptr;
static bool g_dragActive = false;
static Panel* g_dragOwner = nullptr;
static int g_dragIdx = -1;
static std::wstring g_userSuffix;
static std::vector<std::wstring> g_favFailed;
static std::vector<std::wstring> g_favBusy;
static std::vector<std::pair<std::wstring, std::wstring>> g_favQueue;
static std::mutex g_favMutex;
static HANDLE g_favThread = nullptr;
static HANDLE g_favWake = nullptr;
static std::atomic<bool> g_favStop{false};
static HINTERNET g_favNet = nullptr;
static HINTERNET g_favUrl = nullptr;

struct PanelScope {
    Panel* saved;
    explicit PanelScope(Panel* p) : saved(P) { P = p; }
    ~PanelScope() { P = saved; }
};

static struct {
    bool panels[4] = {true, false, false, false};
    int icon = 36;
    bool autoHide = true;
    bool hideFs = true;
    bool anim = true;
    int opacity = 100;
    bool glass = false;
    int style = 0;
    bool magnify = false;
    float speed = 0.25f;
    int fps = 0;
    int radius = 12;
    bool followTheme = false;
    bool showRunning = true;
    bool showPlus = true;
    bool useTargetIcon = true;
    bool fetchFavicons = false;
    int thickness = 0;
    int iconPad = 8;
    int panelPad = 4;
    COLORREF bg = RGB(0x20, 0x20, 0x20);
    COLORREF hover = RGB(0x42, 0x42, 0x42);
    COLORREF accent = RGB(0xBE, 0xBE, 0xBE);
    COLORREF border = RGB(0x46, 0x46, 0x46);
} g_cfg;

static float StyleTau() {
    switch (g_cfg.style) {
        case 3: return 0.085f;
        case 4: return 0.06f;
        default: return 0.055f;
    }
}

static float ExpandTau() {
    switch (g_cfg.style) {
        case 1:
        case 2: return 0.2f;
        default: return StyleTau();
    }
}

constexpr UINT WM_RELOAD = WM_APP + 1;
constexpr UINT WM_FAVICON = WM_APP + 2;
constexpr UINT_PTR kTickId = 1;
constexpr wchar_t kClass[] = L"WhQuickLaunchDockWnd";

enum {
    ID_OPEN = 1, ID_ADMIN, ID_FOLDER, ID_REMOVE, ID_UP, ID_DOWN, ID_PICK, ID_PASTE, ID_ICON,
    ID_ICON_RESET, ID_URL, ID_FAV_REFRESH
};

static int Sc(int v) { return MulDiv(v, g_dpi, 96); }
static int IconPx() { return Sc(g_cfg.icon); }
static int BigPx() { return (int)(IconPx() * (g_cfg.magnify ? 1.4f : 1.15f) + 0.5f); }
static int Pad() { return Sc(g_cfg.iconPad); }
static int Margin() { return Sc(g_cfg.panelPad); }
static int StripW() { return Sc(6); }
static int SlotSize() { return IconPx() + Pad() * 2; }
static int FullW() { return std::max(SlotSize() + Margin() * 2, Sc(g_cfg.thickness)); }
static int CrossOff() { return (FullW() - SlotSize()) / 2; }
static bool PlusVisible() { return g_cfg.showPlus || P->items.empty(); }
static int TotalSlots() { return (int)P->items.size() + (PlusVisible() ? 1 : 0); }
static bool Expanded() { return !g_cfg.autoHide || P->expanded; }

static bool IsRu() { return (GetUserDefaultUILanguage() & 0x3FF) == LANG_RUSSIAN; }
static PCWSTR Tr(PCWSTR en, PCWSTR ru) { return IsRu() ? ru : en; }

static HMONITOR PrimaryMon() {
    return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

static RECT WorkArea() {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(PrimaryMon(), &mi);
    return mi.rcWork;
}

static int MaxItems() {
    RECT wa = WorkArea();
    int len = P->horiz ? (wa.right - wa.left) : (wa.bottom - wa.top);
    int avail = len - Sc(40) - FullW() * 2 - Margin() * 2;
    return std::max(1, avail / SlotSize() - (g_cfg.showPlus ? 1 : 0));
}

static RECT SlotRect(int i) {
    int s = SlotSize(), m = Margin(), c = CrossOff();
    RECT r{c, m + i * s, c + s, m + (i + 1) * s};
    if (P->horiz) return RECT{r.top, r.left, r.bottom, r.right};
    return r;
}

static int HitTest(POINT pt) {
    if (!Expanded()) return -1;
    int s = SlotSize(), m = Margin(), c = CrossOff();
    int lx = P->horiz ? pt.y : pt.x;
    int ly = P->horiz ? pt.x : pt.y;
    if (lx < c || lx >= c + s || ly < m) return -1;
    int i = (ly - m) / s;
    return i < TotalSlots() ? i : -1;
}

static double NowSec() {
    static double freq = 0;
    if (freq == 0) {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        freq = (double)f.QuadPart;
    }
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / freq;
}

static COLORREF ParseColor(PCWSTR s, COLORREF def) {
    if (!s) return def;
    if (*s == L'#') s++;
    if (wcslen(s) != 6) return def;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (!end || *end) return def;
    return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
}

static COLORREF ColorSetting(PCWSTR name, COLORREF def) {
    PCWSTR s = Wh_GetStringSetting(name);
    COLORREF c = ParseColor(s, def);
    if (s) Wh_FreeStringSetting(s);
    return c;
}

static void ApplySystemTheme() {
    DWORD light = 0, sz = sizeof(light);
    bool dark = true;
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &sz) == ERROR_SUCCESS) {
        dark = light == 0;
    }
    COLORREF neutral;
    if (dark) {
        g_cfg.bg = RGB(0x20, 0x20, 0x20);
        g_cfg.hover = RGB(0x42, 0x42, 0x42);
        g_cfg.border = RGB(0x46, 0x46, 0x46);
        neutral = RGB(0xBE, 0xBE, 0xBE);
    } else {
        g_cfg.bg = RGB(0xF3, 0xF3, 0xF3);
        g_cfg.hover = RGB(0xDA, 0xDA, 0xDA);
        g_cfg.border = RGB(0xC8, 0xC8, 0xC8);
        neutral = RGB(0x5F, 0x5F, 0x5F);
    }
    g_cfg.accent = neutral;
    DWORD col = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&col, &opaque))) {
        COLORREF c = RGB((col >> 16) & 255, (col >> 8) & 255, col & 255);
        auto lum = [](COLORREF x) {
            return (0.2126 * GetRValue(x) + 0.7152 * GetGValue(x) + 0.0722 * GetBValue(x)) / 255.0;
        };
        if (fabs(lum(c) - lum(g_cfg.bg)) >= 0.25) g_cfg.accent = c;
    }
}

static void LoadSettings() {
    g_cfg.panels[0] = Wh_GetIntSetting(L"panelRight") != 0;
    g_cfg.panels[1] = Wh_GetIntSetting(L"panelLeft") != 0;
    g_cfg.panels[2] = Wh_GetIntSetting(L"panelTop") != 0;
    g_cfg.panels[3] = Wh_GetIntSetting(L"panelBottom") != 0;
    if (!g_cfg.panels[0] && !g_cfg.panels[1] && !g_cfg.panels[2] && !g_cfg.panels[3]) {
        g_cfg.panels[0] = true;
    }
    g_cfg.icon = std::clamp((int)Wh_GetIntSetting(L"iconSize"), 16, 96);
    g_cfg.autoHide = Wh_GetIntSetting(L"autoHide") != 0;
    g_cfg.hideFs = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    g_cfg.anim = Wh_GetIntSetting(L"animations") != 0;
    g_cfg.opacity = std::clamp((int)Wh_GetIntSetting(L"opacity"), 10, 100);
    g_cfg.glass = Wh_GetIntSetting(L"glass") != 0;
    g_cfg.magnify = Wh_GetIntSetting(L"magnify") != 0;
    g_cfg.speed = std::clamp((int)Wh_GetIntSetting(L"animationSpeed"), 20, 300) / 100.0f;
    g_cfg.showPlus = Wh_GetIntSetting(L"showPlus") != 0;
    g_cfg.useTargetIcon = Wh_GetIntSetting(L"useTargetIcon") != 0;
    g_cfg.fetchFavicons = Wh_GetIntSetting(L"fetchFavicons") != 0;
    g_favFailed.clear();
    g_cfg.thickness = std::clamp((int)Wh_GetIntSetting(L"panelThickness"), 0, 300);
    g_cfg.iconPad = std::clamp((int)Wh_GetIntSetting(L"iconPadding"), 0, 40);
    g_cfg.panelPad = std::clamp((int)Wh_GetIntSetting(L"panelPadding"), 0, 40);
    g_cfg.style = 0;
    PCWSTR st = Wh_GetStringSetting(L"animationStyle");
    if (st) {
        if (wcscmp(st, L"jelly") == 0) g_cfg.style = 1;
        else if (wcscmp(st, L"ripple") == 0) g_cfg.style = 2;
        else if (wcscmp(st, L"fade") == 0) g_cfg.style = 3;
        else if (wcscmp(st, L"genie") == 0) g_cfg.style = 4;
        else if (wcscmp(st, L"oval") == 0) g_cfg.style = 5;
        else if (wcscmp(st, L"drop") == 0) g_cfg.style = 6;
        Wh_FreeStringSetting(st);
    }
    g_cfg.radius = std::clamp((int)Wh_GetIntSetting(L"cornerRadius"), 0, 40);
    g_cfg.followTheme = Wh_GetIntSetting(L"followTheme") != 0;
    g_cfg.showRunning = Wh_GetIntSetting(L"showRunning") != 0;
    g_cfg.fps = 0;
    PCWSTR fr = Wh_GetStringSetting(L"refreshRate");
    if (fr) {
        if (wcscmp(fr, L"hz120") == 0) g_cfg.fps = 120;
        else if (wcscmp(fr, L"hz144") == 0) g_cfg.fps = 144;
        else if (wcscmp(fr, L"hz180") == 0) g_cfg.fps = 180;
        else if (wcscmp(fr, L"hz240") == 0) g_cfg.fps = 240;
        else if (wcscmp(fr, L"hz360") == 0) g_cfg.fps = 360;
        Wh_FreeStringSetting(fr);
    }
    g_cfg.bg = ColorSetting(L"bgColor", RGB(0x20, 0x20, 0x20));
    g_cfg.hover = ColorSetting(L"hoverColor", RGB(0x42, 0x42, 0x42));
    g_cfg.accent = ColorSetting(L"accentColor", RGB(0xBE, 0xBE, 0xBE));
    g_cfg.border = ColorSetting(L"borderColor", RGB(0x46, 0x46, 0x46));
    if (g_cfg.followTheme) ApplySystemTheme();
}

typedef HRESULT(WINAPI* SHGetImageList_t)(int, REFIID, void**);

static HICON LoadIconFor(const std::wstring& path) {
    SHFILEINFOW sfi{};
    DWORD_PTR r = SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
    if (r) {
        static const GUID kIID_IImageList = {
            0x46EB5926, 0x582E, 0x4017, {0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x20}};
        static SHGetImageList_t pGet = reinterpret_cast<SHGetImageList_t>(
            (void*)GetProcAddress(GetModuleHandleW(L"shell32.dll"), "SHGetImageList"));
        int big = BigPx();
        int shil = big <= 40 ? 0  : (big <= 52 ? 2  : 4 );
        void* pil = nullptr;
        if (pGet && SUCCEEDED(pGet(shil, kIID_IImageList, &pil)) && pil) {
            HICON h = ImageList_GetIcon((HIMAGELIST)pil, sfi.iIcon, ILD_NORMAL);
            ((IUnknown*)pil)->Release();
            if (h) return h;
        }
    }
    SHFILEINFOW sfi2{};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi2, sizeof(sfi2), SHGFI_ICON | SHGFI_LARGEICON) &&
        sfi2.hIcon) {
        return sfi2.hIcon;
    }
    return CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
}

static void* Resample(void* src, int size) {
    void* bmp = nullptr;
    if (Gp.CreateBitmapFromScan0(size, size, 0, 0x000E200B, nullptr, &bmp) != 0 || !bmp) {
        return nullptr;
    }
    void* g = nullptr;
    if (Gp.GetImageGraphicsContext(bmp, &g) != 0 || !g) {
        Gp.DisposeImage(bmp);
        return nullptr;
    }
    Gp.SetInterp(g, 7);
    Gp.SetPixOff(g, 2);
    Gp.Clear(g, 0x00000000);
    Gp.DrawImageRect(g, src, 0.0f, 0.0f, (float)size, (float)size);
    Gp.DeleteGraphics(g);
    return bmp;
}

static void* ScaleTo(void* src, int dst) {
    UINT w = 0;
    Gp.GetImageWidth(src, &w);
    int cur = (int)w;
    void* tmp = nullptr;
    void* from = src;
    while (cur / 2 >= dst) {
        void* nb = Resample(from, cur / 2);
        if (!nb) break;
        if (tmp) Gp.DisposeImage(tmp);
        tmp = nb;
        from = nb;
        cur /= 2;
    }
    void* res = Resample(from, dst);
    if (tmp) Gp.DisposeImage(tmp);
    return res;
}

static void FreeImg(Item& it) {
    if (it.img) {
        Gp.DisposeImage(it.img);
        it.img = nullptr;
    }
    if (it.imgBig) {
        Gp.DisposeImage(it.imgBig);
        it.imgBig = nullptr;
    }
}

static void* BitmapFromHicon(HICON h, std::vector<UINT32>& pix) {
    ICONINFO ii{};
    if (!GetIconInfo(h, &ii)) return nullptr;
    BITMAP bm{};
    int w = 0, hgt = 0;
    if (ii.hbmColor && GetObjectW(ii.hbmColor, sizeof(bm), &bm)) {
        w = bm.bmWidth;
        hgt = bm.bmHeight;
    } else if (ii.hbmMask && GetObjectW(ii.hbmMask, sizeof(bm), &bm)) {
        w = bm.bmWidth;
        hgt = bm.bmHeight / 2;
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    if (w <= 0 || hgt <= 0 || w > 1024 || hgt > 1024) return nullptr;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -hgt;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC dc = CreateCompatibleDC(nullptr);
    void* bitsB = nullptr;
    void* bitsW = nullptr;
    HBITMAP dibB = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bitsB, nullptr, 0);
    HBITMAP dibW = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bitsW, nullptr, 0);
    void* bmp = nullptr;
    if (dc && dibB && dibW && bitsB && bitsW) {
        size_t n = (size_t)w * (size_t)hgt;
        HGDIOBJ old = SelectObject(dc, dibB);
        memset(bitsB, 0x00, n * 4);
        DrawIconEx(dc, 0, 0, h, w, hgt, 0, nullptr, DI_NORMAL);
        GdiFlush();
        SelectObject(dc, dibW);
        memset(bitsW, 0xFF, n * 4);
        DrawIconEx(dc, 0, 0, h, w, hgt, 0, nullptr, DI_NORMAL);
        GdiFlush();
        SelectObject(dc, old);

        pix.resize(n);
        const BYTE* pb = (const BYTE*)bitsB;
        const BYTE* pw = (const BYTE*)bitsW;
        for (size_t i = 0; i < n; i++) {
            int db = pw[i * 4 + 0] - pb[i * 4 + 0];
            int dg = pw[i * 4 + 1] - pb[i * 4 + 1];
            int dr = pw[i * 4 + 2] - pb[i * 4 + 2];
            int a = std::clamp(255 - (db + dg + dr) / 3, 0, 255);
            int bl = std::min((int)pb[i * 4 + 0], a);
            int gr = std::min((int)pb[i * 4 + 1], a);
            int rd = std::min((int)pb[i * 4 + 2], a);
            pix[i] = ((UINT32)a << 24) | ((UINT32)rd << 16) | ((UINT32)gr << 8) | (UINT32)bl;
        }
        if (Gp.CreateBitmapFromScan0(w, hgt, w * 4, 0x000E200B, (BYTE*)pix.data(), &bmp) != 0) {
            bmp = nullptr;
        }
    }
    if (dibB) DeleteObject(dibB);
    if (dibW) DeleteObject(dibW);
    if (dc) DeleteDC(dc);
    return bmp;
}

static void* MakeSquare(void* src) {
    UINT w = 0, h = 0;
    Gp.GetImageWidth(src, &w);
    Gp.GetImageHeight(src, &h);
    if (w == 0 || h == 0) return nullptr;
    int side = (int)std::max(w, h);
    if (w == h && side <= 1024) return nullptr;
    int canvas = std::min(side, 1024);
    float sc = (float)canvas / (float)side;
    void* bmp = nullptr;
    if (Gp.CreateBitmapFromScan0(canvas, canvas, 0, 0x000E200B, nullptr, &bmp) != 0 || !bmp) {
        return nullptr;
    }
    void* g = nullptr;
    if (Gp.GetImageGraphicsContext(bmp, &g) != 0 || !g) {
        Gp.DisposeImage(bmp);
        return nullptr;
    }
    Gp.SetInterp(g, 7);
    Gp.SetPixOff(g, 2);
    Gp.Clear(g, 0x00000000);
    float dw = w * sc, dh = h * sc;
    Gp.DrawImageRect(g, src, (canvas - dw) / 2.0f, (canvas - dh) / 2.0f, dw, dh);
    Gp.DeleteGraphics(g);
    return bmp;
}

static const CLSID kCLSID_ShellLink = {
    0x00021401, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const IID kIID_IShellLinkW = {
    0x000214F9, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const IID kIID_IPersistFile = {
    0x0000010B, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static std::wstring ResolveShortcutTarget(const std::wstring& lnk) {
    std::wstring out;
    IShellLinkW* psl = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, kIID_IShellLinkW,
                                (void**)&psl)) ||
        !psl) {
        return out;
    }
    IPersistFile* ppf = nullptr;
    if (SUCCEEDED(psl->QueryInterface(kIID_IPersistFile, (void**)&ppf)) && ppf) {
        if (SUCCEEDED(ppf->Load(lnk.c_str(), STGM_READ))) {
            wchar_t raw[MAX_PATH * 2] = L"";
            if (SUCCEEDED(psl->GetPath(raw, ARRAYSIZE(raw), nullptr, SLGP_RAWPATH)) && raw[0]) {
                wchar_t exp[MAX_PATH * 2];
                DWORD n = ExpandEnvironmentStringsW(raw, exp, ARRAYSIZE(exp));
                out = (n && n <= ARRAYSIZE(exp)) ? exp : raw;
            }
        }
        ppf->Release();
    }
    psl->Release();
    return out;
}

static std::wstring IconSourceFor(const std::wstring& path) {
    if (!g_cfg.useTargetIcon || path.size() < 4) return path;
    if (_wcsicmp(path.c_str() + path.size() - 4, L".lnk") != 0) return path;
    std::wstring target = ResolveShortcutTarget(path);
    if (target.empty() || GetFileAttributesW(target.c_str()) == INVALID_FILE_ATTRIBUTES) return path;
    return target;
}

static HICON ExtractIconFromFile(const std::wstring& file, int index) {
    using Fn = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
    static Fn fn = reinterpret_cast<Fn>(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "PrivateExtractIconsW"));
    if (!fn) return nullptr;
    int big = BigPx();
    int want = big <= 40 ? 32 : (big <= 52 ? 48 : 256);
    HICON h = nullptr;
    UINT id = 0;
    fn(file.c_str(), index, want, want, &h, &id, 1, 0);
    return h;
}

static HICON LoadLinkIcon(const std::wstring& lnk) {
    HICON result = nullptr;
    IShellLinkW* psl = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, kIID_IShellLinkW,
                                (void**)&psl)) ||
        !psl) {
        return nullptr;
    }
    IPersistFile* ppf = nullptr;
    if (SUCCEEDED(psl->QueryInterface(kIID_IPersistFile, (void**)&ppf)) && ppf) {
        if (SUCCEEDED(ppf->Load(lnk.c_str(), STGM_READ))) {
            auto expand = [](const wchar_t* raw) {
                wchar_t exp[MAX_PATH * 2];
                DWORD n = ExpandEnvironmentStringsW(raw, exp, ARRAYSIZE(exp));
                return std::wstring((n && n <= ARRAYSIZE(exp)) ? exp : raw);
            };
            wchar_t raw[MAX_PATH * 2] = L"";
            if (SUCCEEDED(psl->GetPath(raw, ARRAYSIZE(raw), nullptr, SLGP_RAWPATH)) && raw[0]) {
                std::wstring target = expand(raw);
                DWORD attr = GetFileAttributesW(target.c_str());
                if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                    result = ExtractIconFromFile(target, 0);
                }
            }
            if (!result) {
                wchar_t loc[MAX_PATH * 2] = L"";
                int idx = 0;
                if (SUCCEEDED(psl->GetIconLocation(loc, ARRAYSIZE(loc), &idx)) && loc[0]) {
                    std::wstring f = expand(loc);
                    if (GetFileAttributesW(f.c_str()) != INVALID_FILE_ATTRIBUTES) {
                        result = ExtractIconFromFile(f, idx);
                    }
                }
            }
        }
        ppf->Release();
    }
    psl->Release();
    return result;
}

static std::wstring LinkTarget(const std::wstring& lnk) {
    std::wstring out;
    IShellLinkW* psl = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, kIID_IShellLinkW,
                                (void**)&psl)) ||
        !psl) {
        return out;
    }
    IPersistFile* ppf = nullptr;
    if (SUCCEEDED(psl->QueryInterface(kIID_IPersistFile, (void**)&ppf)) && ppf) {
        if (SUCCEEDED(ppf->Load(lnk.c_str(), STGM_READ))) {
            wchar_t raw[MAX_PATH * 2] = L"";
            if (SUCCEEDED(psl->GetPath(raw, ARRAYSIZE(raw), nullptr, SLGP_RAWPATH)) && raw[0]) {
                wchar_t exp[MAX_PATH * 2];
                DWORD n = ExpandEnvironmentStringsW(raw, exp, ARRAYSIZE(exp));
                out = (n && n <= ARRAYSIZE(exp)) ? exp : raw;
            }
        }
        ppf->Release();
    }
    psl->Release();
    return out;
}

static std::wstring LowerStr(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

static bool IsUrl(const std::wstring& s) {
    return _wcsnicmp(s.c_str(), L"http://", 7) == 0 || _wcsnicmp(s.c_str(), L"https://", 8) == 0;
}

static void ComputeExeKey(Item& it) {
    it.exeKey.clear();
    it.exeName.clear();
    if (IsUrl(it.path)) return;
    std::wstring target = it.path;
    size_t n = target.size();
    if (n >= 4 && _wcsicmp(target.c_str() + n - 4, L".lnk") == 0) target = LinkTarget(target);
    n = target.size();
    if (n < 4 || _wcsicmp(target.c_str() + n - 4, L".exe") != 0) return;
    std::wstring low = LowerStr(target);
    size_t sl = low.find_last_of(L"\\/");
    std::wstring name = sl == std::wstring::npos ? low : low.substr(sl + 1);
    if (name == L"explorer.exe") return;
    it.exeKey = low;
    it.exeName = name;
}

static std::wstring HostOf(const std::wstring& url) {
    size_t p = url.find(L"://");
    if (p == std::wstring::npos) return L"";
    size_t b = p + 3;
    size_t e = url.find_first_of(L"/?#", b);
    std::wstring a = url.substr(b, e == std::wstring::npos ? std::wstring::npos : e - b);
    size_t at = a.rfind(L'@');
    if (at != std::wstring::npos) a = a.substr(at + 1);
    if (!a.empty() && a[0] != L'[') {
        size_t c = a.find(L':');
        if (c != std::wstring::npos) a.resize(c);
    }
    for (auto& ch : a) ch = (wchar_t)towlower(ch);
    return a;
}

static std::wstring BareHost(const std::wstring& host) {
    if (host.size() > 4 && _wcsnicmp(host.c_str(), L"www.", 4) == 0) return host.substr(4);
    return host;
}

static std::wstring TrimUrl(std::wstring s) {
    auto bad = [](wchar_t c) { return iswspace(c) || c == L'"' || c == L'<' || c == L'>'; };
    while (!s.empty() && bad(s.front())) s.erase(s.begin());
    while (!s.empty() && bad(s.back())) s.pop_back();
    return s;
}

static bool LooksLikeUrl(const std::wstring& raw) {
    std::wstring s = TrimUrl(raw);
    return IsUrl(s) || _wcsnicmp(s.c_str(), L"www.", 4) == 0;
}

static std::wstring NormalizeUrl(const std::wstring& raw) {
    std::wstring s = TrimUrl(raw);
    if (s.empty() || s.size() > 2000) return L"";
    for (wchar_t c : s) {
        if (iswspace(c) || c < 0x20) return L"";
    }
    bool hadScheme = IsUrl(s);
    if (!hadScheme) {
        if (s.find(L"://") != std::wstring::npos) return L"";
        size_t ae = s.find_first_of(L"/?#");
        std::wstring auth = s.substr(0, ae);
        if (auth.find(L'@') != std::wstring::npos) return L"";
        size_t colon = auth.rfind(L':');
        if (colon != std::wstring::npos) {
            if (colon + 1 >= auth.size()) return L"";
            for (size_t i = colon + 1; i < auth.size(); i++) {
                if (auth[i] < L'0' || auth[i] > L'9') return L"";
            }
        }
        s = L"https://" + s;
    }
    std::wstring host = HostOf(s);
    if (host.empty()) return L"";
    if (!hadScheme && host.find(L'.') == std::wstring::npos && host != L"localhost") return L"";
    for (size_t p; (p = s.find(L'|')) != std::wstring::npos;) s.replace(p, 1, L"%7C");
    return s;
}

static void* RoundPath(float x, float y, float w, float h, float r);

static const unsigned char kGlobeRuns[] = {
    117,22,225,40,211,50,201,60,192,68,185,74,179,80,173,86,167,92,161,98,156,102,152,106,148,110,144,114,140,118,136,122,132,54,1,16,1,54,128,54,
    4,14,4,54,125,53,5,16,5,53,122,53,7,15,8,53,119,52,9,15,10,52,116,53,10,15,11,53,113,52,12,15,13,52,110,31,1,21,13,15,
    14,21,1,31,107,30,3,20,14,15,15,20,3,30,104,29,5,20,15,15,16,20,5,29,101,28,6,20,16,15,17,20,6,28,99,27,7,20,17,15,
    18,20,7,27,96,27,9,19,18,15,19,19,9,27,93,27,9,19,19,15,20,19,9,27,91,26,10,19,20,15,21,19,10,26,89,25,12,18,21,15,
    22,18,12,25,87,25,12,18,22,15,23,18,12,25,85,24,14,18,22,15,23,18,14,24,83,24,14,18,23,15,24,18,14,24,81,23,15,18,24,15,
    25,18,15,23,79,23,16,17,25,15,26,17,16,23,76,23,17,18,25,15,26,18,17,23,74,22,18,17,26,15,27,17,18,22,73,22,18,17,27,15,
    28,17,18,22,71,22,19,17,27,15,28,17,19,22,69,21,20,17,28,15,29,17,20,21,67,21,21,17,28,15,29,17,21,21,65,21,21,17,29,15,
    30,17,21,21,63,21,22,16,30,15,31,16,22,21,61,21,22,17,30,15,31,17,22,21,59,21,23,16,31,15,32,16,23,21,57,21,23,17,31,15,
    32,17,23,21,56,20,24,16,32,15,33,16,24,20,55,20,24,17,32,15,33,17,24,20,53,20,25,16,33,15,34,16,25,20,51,20,26,16,33,15,
    34,16,26,20,50,19,26,17,33,15,34,17,26,19,49,19,27,16,34,15,35,16,27,19,47,19,27,17,34,15,35,17,27,19,46,18,28,16,35,15,
    36,16,28,18,45,19,28,16,35,15,36,16,28,19,43,22,25,16,36,15,37,16,28,19,42,26,21,16,36,15,37,16,24,23,41,31,17,16,36,15,
    37,17,19,28,39,36,12,16,37,15,38,16,15,33,38,41,7,16,37,15,38,16,10,38,37,47,1,16,38,15,39,16,3,45,35,65,38,15,39,65,
    34,65,38,15,39,65,33,73,31,16,34,70,32,84,21,14,23,82,31,226,30,226,29,228,28,17,2,209,27,17,7,185,4,17,26,17,11,178,7,17,
    25,17,15,170,13,17,24,16,21,161,18,16,23,17,25,152,23,17,22,16,32,141,29,16,21,17,35,133,34,17,20,16,36,132,36,16,19,17,36,132,
    36,17,18,16,37,15,10,84,8,15,37,16,18,16,36,16,22,60,20,16,36,16,17,17,36,16,43,16,43,16,36,17,16,16,37,15,45,14,45,15,
    37,16,16,16,37,15,44,16,44,15,37,16,15,16,38,15,44,16,44,15,38,16,14,16,37,16,44,16,44,16,37,16,14,15,38,16,44,16,44,16,
    38,15,13,16,38,15,45,16,45,15,38,16,12,16,38,15,45,16,45,15,38,16,12,15,39,15,45,16,45,15,39,15,11,16,39,15,45,16,45,15,
    39,16,10,16,38,16,45,16,45,16,38,16,10,15,39,15,46,16,46,15,39,15,9,16,39,15,46,16,46,15,39,16,8,16,39,15,46,16,46,15,
    39,16,8,15,40,15,46,16,46,15,40,15,8,15,40,15,46,16,46,15,40,15,7,16,40,15,46,16,46,15,40,16,6,16,39,16,46,16,46,16,
    39,16,6,15,40,15,47,16,47,15,40,15,6,15,40,15,47,16,47,15,40,15,6,15,40,15,47,16,47,15,40,15,5,16,40,15,47,16,47,15,
    40,16,4,16,40,15,47,16,47,15,40,16,4,15,41,15,47,16,47,15,41,15,4,15,41,15,47,16,47,15,41,15,4,15,41,15,47,16,47,15,
    41,15,3,16,40,16,47,16,47,16,40,16,2,16,40,15,48,16,48,15,40,16,2,15,41,15,48,16,48,15,41,15,2,15,41,15,48,16,48,15,
    41,15,2,15,41,15,48,16,48,15,41,15,2,15,41,15,48,16,48,15,41,15,2,15,41,15,48,16,48,15,41,15,2,15,41,15,48,16,48,15,
    41,15,2,15,41,15,48,16,48,15,41,15,1,16,41,15,48,16,48,15,41,31,42,15,48,16,48,15,42,30,42,15,49,14,49,15,42,31,1,1,
    36,1,1,63,1,16,1,63,1,38,1,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,46,
    1,1,36,1,1,63,1,16,1,63,1,38,1,31,42,15,49,14,49,15,42,30,42,15,48,16,48,15,42,31,41,15,49,15,48,15,41,16,1,15,
    41,15,49,15,48,15,41,15,2,15,41,15,49,15,48,15,41,15,2,15,41,15,49,15,48,15,41,15,2,15,41,15,49,15,48,15,41,15,2,15,
    41,15,49,15,48,15,41,15,2,15,41,15,49,15,48,15,41,15,2,15,41,15,49,15,48,15,41,15,2,16,40,16,48,15,47,16,40,16,2,16,
    41,15,48,15,47,15,41,16,3,15,41,15,48,15,47,15,41,15,4,15,41,15,48,15,47,15,41,15,4,15,41,15,48,15,47,15,41,15,4,16,
    40,15,48,15,47,15,40,16,4,16,40,15,48,15,47,15,40,16,5,15,40,15,48,15,47,15,40,15,6,15,40,15,48,15,47,15,40,15,6,15,
    40,16,47,15,46,16,40,15,6,16,40,15,47,15,46,15,40,16,6,16,40,15,47,15,46,15,40,16,7,15,40,15,47,15,46,15,40,15,8,15,
    40,15,47,15,46,15,40,15,8,16,39,15,47,15,46,15,39,16,8,16,39,15,47,15,46,15,39,16,9,15,39,16,46,15,45,16,39,15,10,16,
    39,15,46,15,45,15,39,16,10,16,39,15,46,15,45,15,39,16,11,15,39,15,46,15,45,15,39,15,12,16,38,15,46,15,45,15,38,16,12,16,
    38,15,46,15,45,15,38,16,13,16,37,16,45,15,44,16,38,15,14,16,38,15,45,15,44,15,38,16,14,16,38,15,44,16,44,15,38,16,15,16,
    37,15,45,14,45,15,37,16,16,16,37,16,43,16,43,16,37,16,16,17,36,16,24,52,26,16,36,17,17,16,37,15,10,79,13,15,37,16,18,16,
    37,114,3,15,37,16,18,17,36,132,36,17,19,16,36,132,36,16,20,17,30,139,33,17,21,16,25,150,27,16,22,17,19,159,22,17,23,16,15,168,
    17,16,24,17,10,176,12,17,25,17,5,184,7,17,26,17,1,191,4,17,27,228,28,228,29,226,30,86,19,16,15,90,31,73,31,16,29,75,32,66,
    38,16,38,66,33,65,38,16,38,65,34,47,2,16,38,16,38,65,35,41,8,16,37,16,37,16,6,43,37,34,14,16,37,16,37,16,11,37,38,30,
    18,17,36,16,36,17,16,32,39,25,23,16,36,16,36,16,21,27,41,20,27,16,36,16,36,16,25,22,42,19,29,16,35,16,35,17,28,19,43,19,
    28,16,35,16,35,16,28,19,45,18,28,16,35,16,35,16,28,18,46,19,28,16,34,16,34,16,28,19,47,19,27,16,34,16,34,16,27,19,49,19,
    26,17,33,16,33,17,26,19,50,20,26,16,33,16,33,16,26,20,51,20,25,17,32,16,32,17,25,20,53,20,25,16,32,16,32,16,25,20,55,20,
    24,17,31,16,31,17,24,20,56,21,24,16,31,16,31,16,24,21,57,21,23,17,30,16,30,17,23,21,59,21,23,16,30,16,30,16,23,21,61,21,
    22,17,29,16,29,17,22,21,63,21,22,17,28,16,28,17,22,21,65,21,21,17,28,16,28,17,21,21,67,21,21,17,27,16,27,17,21,21,69,22,
    19,17,27,16,27,17,19,22,71,22,19,17,26,16,26,17,19,22,73,22,18,18,25,16,25,18,18,22,74,23,18,17,25,16,25,17,18,23,76,23,
    16,18,24,16,24,18,16,23,79,23,16,18,23,16,23,18,16,23,81,24,14,19,22,16,22,19,14,24,83,24,14,18,22,16,22,18,14,24,85,25,
    13,18,21,16,21,18,13,25,87,25,12,19,20,16,20,19,12,25,89,26,11,19,19,16,19,19,11,26,91,27,10,19,18,16,18,19,10,27,93,27,
    9,20,17,16,17,20,9,27,96,27,8,20,16,16,16,20,8,27,99,28,7,20,15,16,15,20,7,28,101,29,5,21,14,16,14,21,5,29,104,30,
    3,21,13,16,13,21,3,30,107,31,2,21,12,16,12,21,2,31,110,53,11,16,11,53,113,54,9,16,9,54,116,53,8,16,8,53,119,54,6,16,
    6,54,122,54,4,16,4,54,125,56,1,16,1,56,128,126,132,122,136,118,140,114,144,110,148,106,152,102,156,98,161,92,167,86,173,80,179,74,185,68,
    192,60,201,50,211,40,225,22,117,
};

static void* MakeGlobeIcon(std::vector<UINT32>& pix) {
    const int S = 256;
    const size_t total = (size_t)S * S;
    pix.assign(total, 0);
    size_t pos = 0;
    bool on = false;
    for (unsigned char run : kGlobeRuns) {
        size_t n = std::min((size_t)run, total - pos);
        if (on) {
            for (size_t i = 0; i < n; i++) pix[pos + i] = 0xFF9A9A9A;
        }
        pos += n;
        on = !on;
        if (pos >= total) break;
    }
    void* bmp = nullptr;
    if (Gp.CreateBitmapFromScan0(S, S, S * 4, 0x000E200B, (BYTE*)pix.data(), &bmp) != 0 || !bmp) {
        pix.clear();
        return nullptr;
    }
    return bmp;
}

static std::wstring OriginOf(const std::wstring& url) {
    size_t p = url.find(L"://");
    if (p == std::wstring::npos) return L"";
    size_t b = p + 3;
    size_t e = url.find_first_of(L"/?#", b);
    std::wstring a = url.substr(b, e == std::wstring::npos ? std::wstring::npos : e - b);
    size_t at = a.rfind(L'@');
    if (at != std::wstring::npos) a = a.substr(at + 1);
    if (a.empty()) return L"";
    return url.substr(0, b) + a;
}

static std::wstring FaviconDir() {
    wchar_t base[MAX_PATH] = L"";
    if (!Wh_GetModStoragePath(base, ARRAYSIZE(base)) || !base[0]) return L"";
    return std::wstring(base) + L"\\favicons";
}

static std::wstring FaviconFile(const std::wstring& bare, const wchar_t* ext) {
    std::wstring d = FaviconDir();
    if (d.empty() || bare.empty()) return L"";
    std::wstring n;
    for (wchar_t c : bare) n += (iswalnum(c) || c == L'.' || c == L'-') ? c : L'_';
    if (n.size() > 100) n.resize(100);
    return d + L"\\" + n + ext;
}

static bool InList(const std::vector<std::wstring>& v, const std::wstring& s) {
    for (auto& x : v) {
        if (_wcsicmp(x.c_str(), s.c_str()) == 0) return true;
    }
    return false;
}

static void EraseFromList(std::vector<std::wstring>& v, const std::wstring& s) {
    v.erase(std::remove_if(v.begin(), v.end(),
                           [&](const std::wstring& x) { return _wcsicmp(x.c_str(), s.c_str()) == 0; }),
            v.end());
}

static bool RecentlyFailed(const std::wstring& marker) {
    WIN32_FILE_ATTRIBUTE_DATA fa{};
    if (!GetFileAttributesExW(marker.c_str(), GetFileExInfoStandard, &fa)) return false;
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER a, b;
    a.LowPart = now.dwLowDateTime;
    a.HighPart = now.dwHighDateTime;
    b.LowPart = fa.ftLastWriteTime.dwLowDateTime;
    b.HighPart = fa.ftLastWriteTime.dwHighDateTime;
    const ULONGLONG week = 7ULL * 24ULL * 3600ULL * 10000000ULL;
    return a.QuadPart >= b.QuadPart && a.QuadPart - b.QuadPart < week;
}

static void WriteMarker(const std::wstring& path) {
    if (path.empty()) return;
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) CloseHandle(f);
}

static bool IsIconOrPng(const std::vector<BYTE>& d) {
    if (d.size() < 16) return false;
    if (d[0] == 0x89 && d[1] == 0x50 && d[2] == 0x4E && d[3] == 0x47) return true;
    if (d[0] == 0x00 && d[1] == 0x00 && d[2] == 0x01 && d[3] == 0x00) return true;
    return false;
}

static void FavCloseHandles() {
    if (g_favUrl) {
        InternetCloseHandle(g_favUrl);
        g_favUrl = nullptr;
    }
    if (g_favNet) {
        InternetCloseHandle(g_favNet);
        g_favNet = nullptr;
    }
}

enum { FAV_OK = 0, FAV_RETRY = 1, FAV_NONE = 2 };

static int FetchFavicon(const std::wstring& origin, const std::wstring& outFile) {
    HINTERNET hi = InternetOpenW(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) QuickLaunchDock/1.1",
        INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hi) return FAV_RETRY;
    DWORD to = 8000;
    InternetSetOptionW(hi, INTERNET_OPTION_CONNECT_TIMEOUT, &to, sizeof(to));
    InternetSetOptionW(hi, INTERNET_OPTION_RECEIVE_TIMEOUT, &to, sizeof(to));
    InternetSetOptionW(hi, INTERNET_OPTION_SEND_TIMEOUT, &to, sizeof(to));
    {
        std::lock_guard<std::mutex> lk(g_favMutex);
        if (g_favStop) {
            InternetCloseHandle(hi);
            return FAV_RETRY;
        }
        g_favNet = hi;
    }
    std::wstring url = origin + L"/favicon.ico";
    HINTERNET hu = InternetOpenUrlW(hi, url.c_str(), L"Accept: image/png,image/x-icon,image/*;q=0.8\r\n",
                                    (DWORD)-1,
                                    INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                                        INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                                        INTERNET_FLAG_RELOAD,
                                    0);
    {
        std::lock_guard<std::mutex> lk(g_favMutex);
        if (hu) {
            if (g_favNet) {
                g_favUrl = hu;
            } else {
                hu = nullptr;
            }
        }
    }
    int result = FAV_RETRY;
    if (hu) {
        DWORD status = 0, sl = sizeof(status);
        if (HttpQueryInfoW(hu, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &sl, nullptr)) {
            if (status == 200) {
                std::vector<BYTE> data;
                BYTE buf[4096];
                DWORD got = 0;
                bool tooBig = false, readOk = true;
                for (;;) {
                    if (g_favStop || !InternetReadFile(hu, buf, sizeof(buf), &got)) {
                        readOk = false;
                        break;
                    }
                    if (!got) break;
                    data.insert(data.end(), buf, buf + got);
                    if (data.size() > 256 * 1024) {
                        tooBig = true;
                        break;
                    }
                }
                if (tooBig) {
                    result = FAV_NONE;
                } else if (readOk) {
                    if (!IsIconOrPng(data)) {
                        result = FAV_NONE;
                    } else {
                        std::wstring tmp = outFile + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";
                        HANDLE f = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                               FILE_ATTRIBUTE_NORMAL, nullptr);
                        if (f != INVALID_HANDLE_VALUE) {
                            DWORD wr = 0;
                            bool w = WriteFile(f, data.data(), (DWORD)data.size(), &wr, nullptr) &&
                                     wr == data.size();
                            CloseHandle(f);
                            if (w && MoveFileExW(tmp.c_str(), outFile.c_str(), MOVEFILE_REPLACE_EXISTING)) {
                                result = FAV_OK;
                            } else {
                                DeleteFileW(tmp.c_str());
                            }
                        }
                    }
                }
            } else if (status == 404 || status == 410) {
                result = FAV_NONE;
            }
        }
    }
    {
        std::lock_guard<std::mutex> lk(g_favMutex);
        FavCloseHandles();
    }
    return result;
}

static DWORD WINAPI FavThread(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    for (;;) {
        std::pair<std::wstring, std::wstring> job;
        bool have = false;
        {
            std::lock_guard<std::mutex> lk(g_favMutex);
            if (g_favStop) break;
            if (!g_favQueue.empty()) {
                job = g_favQueue.front();
                g_favQueue.erase(g_favQueue.begin());
                have = true;
            }
        }
        if (!have) {
            WaitForSingleObject(g_favWake, INFINITE);
            continue;
        }
        std::wstring file = FaviconFile(job.second, L".img");
        bool ok = false;
        if (!file.empty()) {
            SHCreateDirectoryExW(nullptr, FaviconDir().c_str(), nullptr);
            int r = FAV_RETRY;
            for (int attempt = 0; attempt < 2 && r == FAV_RETRY && !g_favStop; attempt++) {
                if (attempt) {
                    for (int i = 0; i < 15 && !g_favStop; i++) Sleep(100);
                    if (g_favStop) break;
                }
                r = FetchFavicon(job.first, file);
            }
            ok = r == FAV_OK;
            if (r == FAV_NONE && !g_favStop) WriteMarker(FaviconFile(job.second, L".none"));
        }
        HWND ctrl = g_ctrl.load();
        auto* host = new std::wstring(job.second);
        if (!ctrl || !PostMessageW(ctrl, WM_FAVICON, ok ? 1 : 0, (LPARAM)host)) delete host;
    }
    return 0;
}

static bool RequestFavicon(const std::wstring& url) {
    if (!g_cfg.fetchFavicons || g_favStop) return false;
    std::wstring origin = OriginOf(url);
    std::wstring bare = BareHost(HostOf(url));
    if (origin.empty() || bare.empty()) return false;
    std::wstring none = FaviconFile(bare, L".none");
    if (none.empty() || InList(g_favFailed, bare) || RecentlyFailed(none)) return false;
    if (!g_favWake) g_favWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_favWake) return false;
    {
        std::lock_guard<std::mutex> lk(g_favMutex);
        if (InList(g_favBusy, bare)) return true;
        g_favBusy.push_back(bare);
        g_favQueue.emplace_back(origin, bare);
        if (!g_favThread) {
            g_favThread = CreateThread(nullptr, 0, FavThread, nullptr, 0, nullptr);
            if (!g_favThread) {
                g_favQueue.pop_back();
                EraseFromList(g_favBusy, bare);
                return false;
            }
        }
    }
    SetEvent(g_favWake);
    return true;
}

static void MarkFavFailed(const std::wstring& bare) {
    if (!InList(g_favFailed, bare)) g_favFailed.push_back(bare);
    WriteMarker(FaviconFile(bare, L".none"));
}

static void StopFavicons() {
    g_favStop = true;
    {
        std::lock_guard<std::mutex> lk(g_favMutex);
        FavCloseHandles();
        g_favQueue.clear();
    }
    if (g_favWake) SetEvent(g_favWake);
    if (g_favThread) {
        WaitForSingleObject(g_favThread, 3000);
        CloseHandle(g_favThread);
        g_favThread = nullptr;
    }
}

static void* LoadImageFileSquare(const std::wstring& file) {
    void* src = nullptr;
    if (Gp.BitmapFromFile(file.c_str(), &src) != 0) return nullptr;
    if (!src) return nullptr;
    void* sq = MakeSquare(src);
    if (sq) {
        Gp.DisposeImage(src);
        src = sq;
    }
    return src;
}

static bool NeedsBacking(void* src, bool& darkGlyph) {
    void* small = Resample(src, 16);
    if (!small) return false;
    double sumA = 0.0, sumL = 0.0, sumS = 0.0;
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            UINT32 c = 0;
            if (Gp.BitmapGetPixel(small, x, y, &c) != 0) continue;
            double a = (double)(c >> 24) / 255.0;
            if (a <= 0.0) continue;
            double r = (double)((c >> 16) & 255) / 255.0;
            double g = (double)((c >> 8) & 255) / 255.0;
            double b = (double)(c & 255) / 255.0;
            double mx = std::max(r, std::max(g, b)), mn = std::min(r, std::min(g, b));
            sumA += a;
            sumL += a * (0.2126 * r + 0.7152 * g + 0.0722 * b);
            sumS += a * (mx <= 0.0 ? 0.0 : (mx - mn) / mx);
        }
    }
    Gp.DisposeImage(small);
    if (sumA < 256.0 * 0.04) return false;
    double lg = sumL / sumA;
    double sg = sumS / sumA;
    double lb = (0.2126 * GetRValue(g_cfg.bg) + 0.7152 * GetGValue(g_cfg.bg) +
                 0.0722 * GetBValue(g_cfg.bg)) / 255.0;
    darkGlyph = lg < 0.5;
    return fabs(lg - lb) < 0.30 && sg < 0.35;
}

static void* BackedIcon(void* src, bool darkGlyph, std::vector<UINT32>& pix) {
    const int S = 128;
    pix.assign((size_t)S * S, 0);
    void* bmp = nullptr;
    if (Gp.CreateBitmapFromScan0(S, S, S * 4, 0x000E200B, (BYTE*)pix.data(), &bmp) != 0 || !bmp) {
        pix.clear();
        return nullptr;
    }
    void* g = nullptr;
    if (Gp.GetImageGraphicsContext(bmp, &g) != 0 || !g) {
        Gp.DisposeImage(bmp);
        pix.clear();
        return nullptr;
    }
    Gp.SetSmoothing(g, 4);
    Gp.SetInterp(g, 7);
    Gp.SetPixOff(g, 2);
    const float bw = 6.0f;
    void* path = RoundPath(bw / 2.0f, bw / 2.0f, (float)S - bw, (float)S - bw, 28.0f);
    void* br = nullptr;
    void* pen = nullptr;
    Gp.CreateFill(darkGlyph ? 0xFFF2F2F2 : 0xFF000000, &br);
    if (!darkGlyph) Gp.CreatePen(0xFFFFFFFF, bw, 2, &pen);
    if (path && br) Gp.FillPath(g, br, path);
    if (path && pen) Gp.DrawPath(g, pen, path);
    if (pen) Gp.DelPen(pen);
    if (br) Gp.DelBrush(br);
    if (path) Gp.DeletePath(path);
    const float inset = 22.0f;
    Gp.DrawImageRect(g, src, inset, inset, (float)S - inset * 2.0f, (float)S - inset * 2.0f);
    Gp.DeleteGraphics(g);
    return bmp;
}

static void LoadImages(Item& it) {
    FreeImg(it);
    it.loading = false;
    ComputeExeKey(it);
    void* src = nullptr;
    HICON h = nullptr;
    std::vector<UINT32> pix;

    const bool isUrl = IsUrl(it.path);
    if (!it.iconPath.empty()) src = LoadImageFileSquare(it.iconPath);

    if (!src && isUrl) {
        std::wstring bare = BareHost(HostOf(it.path));
        std::wstring fav = FaviconFile(bare, L".img");
        bool failed = false;
        if (!fav.empty() && GetFileAttributesW(fav.c_str()) != INVALID_FILE_ATTRIBUTES) {
            src = LoadImageFileSquare(fav);
            if (!src) {
                DeleteFileW(fav.c_str());
                MarkFavFailed(bare);
                failed = true;
            } else {
                bool darkGlyph = false;
                if (NeedsBacking(src, darkGlyph)) {
                    void* backed = BackedIcon(src, darkGlyph, pix);
                    if (backed) {
                        Gp.DisposeImage(src);
                        src = backed;
                    }
                }
            }
        } else if (g_cfg.fetchFavicons) {
            std::wstring none = FaviconFile(bare, L".none");
            failed = InList(g_favFailed, bare) || (!none.empty() && RecentlyFailed(none));
            if (!failed) it.loading = RequestFavicon(it.path);
        }
        if (!src) src = MakeGlobeIcon(pix);
    }

    if (!src) {
        if (g_cfg.useTargetIcon && it.path.size() >= 4 &&
            _wcsicmp(it.path.c_str() + it.path.size() - 4, L".lnk") == 0) {
            h = LoadLinkIcon(it.path);
        }
        if (!h) h = LoadIconFor(IconSourceFor(it.path));
        if (!h) return;
        src = BitmapFromHicon(h, pix);
        if (!src) {
            pix.clear();
            if (Gp.BitmapFromHICON(h, &src) != 0 || !src) {
                DestroyIcon(h);
                return;
            }
        }
    }

    it.img = ScaleTo(src, IconPx());
    it.imgBig = ScaleTo(src, BigPx());
    if (!it.img && !it.imgBig && pix.empty()) {
        it.img = src;
    } else {
        Gp.DisposeImage(src);
    }
    if (h) DestroyIcon(h);
}

static std::wstring TitleOf(const std::wstring& p) {
    if (IsUrl(p)) {
        std::wstring h = BareHost(HostOf(p));
        if (!h.empty()) return h;
    }
    SHFILEINFOW sfi{};
    if (SHGetFileInfoW(p.c_str(), 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME) &&
        sfi.szDisplayName[0]) {
        return sfi.szDisplayName;
    }
    size_t pos = p.find_last_of(L"\\/");
    return pos == std::wstring::npos ? p : p.substr(pos + 1);
}

static void ReloadIcons() {
    for (auto& it : P->items) LoadImages(it);
}

static std::wstring EdgeSuffix() {
    static const wchar_t* sfx[4] = {L"", L"_left", L"_top", L"_bottom"};
    return sfx[P->edge];
}
static std::wstring ItemsKey() { return L"items" + EdgeSuffix() + g_userSuffix; }
static std::wstring IconsKey() { return L"customIcons" + EdgeSuffix() + g_userSuffix; }

static std::wstring CurrentUserSid() {
    std::wstring sid;
    HANDLE tok = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok)) {
        DWORD n = 0;
        GetTokenInformation(tok, TokenUser, nullptr, 0, &n);
        if (n) {
            std::vector<BYTE> buf(n);
            if (GetTokenInformation(tok, TokenUser, buf.data(), n, &n)) {
                LPWSTR str = nullptr;
                if (ConvertSidToStringSidW(((TOKEN_USER*)buf.data())->User.Sid, &str) && str) {
                    sid = str;
                    LocalFree(str);
                }
            }
        }
        CloseHandle(tok);
    }
    return sid;
}

static std::wstring ReadValue(const std::wstring& key) {
    std::vector<wchar_t> buf(32768, 0);
    Wh_GetStringValue(key.c_str(), buf.data(), buf.size());
    return buf.data();
}

static void ClaimSharedValue(const std::wstring& base) {
    if (g_userSuffix.empty()) return;
    std::wstring mine = base + g_userSuffix;
    if (!ReadValue(mine).empty()) return;
    std::wstring shared = ReadValue(base);
    if (shared.empty()) return;
    Wh_SetStringValue(mine.c_str(), shared.c_str());
    Wh_DeleteValue(base.c_str());
}

static void InitUserStorage() {
    std::wstring sid = CurrentUserSid();
    g_userSuffix = sid.empty() ? L"" : L"_" + sid;
    static const wchar_t* const sfx[4] = {L"", L"_left", L"_top", L"_bottom"};
    for (auto e : sfx) {
        ClaimSharedValue(std::wstring(L"items") + e);
        ClaimSharedValue(std::wstring(L"customIcons") + e);
    }
}

static void SaveItems() {
    std::wstring s;
    for (auto& it : P->items) {
        if (!s.empty()) s += L'|';
        s += it.path;
    }
    Wh_SetStringValue(ItemsKey().c_str(), s.c_str());

    std::wstring ci;
    for (auto& it : P->items) {
        if (it.iconPath.empty()) continue;
        if (!ci.empty()) ci += L'|';
        ci += it.path;
        ci += L'\n';
        ci += it.iconPath;
    }
    Wh_SetStringValue(IconsKey().c_str(), ci.c_str());
}

static std::vector<std::pair<std::wstring, std::wstring>> LoadCustomIconMap() {
    std::vector<std::pair<std::wstring, std::wstring>> out;
    std::vector<wchar_t> buf(32768, 0);
    Wh_GetStringValue(IconsKey().c_str(), buf.data(), buf.size());
    std::wstring all = buf.data();
    size_t pos = 0;
    while (pos < all.size()) {
        size_t e = all.find(L'|', pos);
        if (e == std::wstring::npos) e = all.size();
        std::wstring entry = all.substr(pos, e - pos);
        size_t nl = entry.find(L'\n');
        if (nl != std::wstring::npos) out.emplace_back(entry.substr(0, nl), entry.substr(nl + 1));
        pos = e + 1;
    }
    return out;
}

static void LoadItems() {
    for (auto& it : P->items) FreeImg(it);
    P->items.clear();

    auto customIcons = LoadCustomIconMap();

    std::vector<wchar_t> buf(32768, 0);
    Wh_GetStringValue(ItemsKey().c_str(), buf.data(), buf.size());
    std::wstring all = buf.data();
    size_t pos = 0;
    while (pos < all.size()) {
        size_t e = all.find(L'|', pos);
        if (e == std::wstring::npos) e = all.size();
        std::wstring p = all.substr(pos, e - pos);
        if (!p.empty()) {
            Item it;
            it.path = p;
            it.name = TitleOf(p);
            for (auto& kv : customIcons) {
                if (_wcsicmp(kv.first.c_str(), p.c_str()) == 0) {
                    it.iconPath = kv.second;
                    break;
                }
            }
            LoadImages(it);
            it.pos = (float)P->items.size();
            P->items.push_back(std::move(it));
        }
        pos = e + 1;
    }
    P->plus.pos = (float)P->items.size();
    P->plus.hover = 0;
}

static void UpdateTooltips() {
    if (!P->tip) return;
    for (int i = 0; i < P->toolCount; i++) {
        TTTOOLINFOW ti{};
        ti.cbSize = (UINT)offsetof(TTTOOLINFOW, lParam);
        ti.hwnd = P->hwnd;
        ti.uId = i;
        SendMessageW(P->tip, TTM_DELTOOLW, 0, (LPARAM)&ti);
    }
    P->toolCount = 0;
    if (!Expanded()) return;

    for (int i = 0; i < TotalSlots(); i++) {
        TTTOOLINFOW ti{};
        ti.cbSize = (UINT)offsetof(TTTOOLINFOW, lParam);
        ti.uFlags = TTF_SUBCLASS;
        ti.hwnd = P->hwnd;
        ti.uId = i;
        ti.rect = SlotRect(i);
        ti.lpszText = i < (int)P->items.size() ? (LPWSTR)P->items[i].name.c_str()
                                              : (LPWSTR)Tr(L"Add a program", L"Добавить программу");
        if (SendMessageW(P->tip, TTM_ADDTOOLW, 0, (LPARAM)&ti)) P->toolCount = i + 1;
    }
}

constexpr float kGenieTail = 0.9f;
constexpr float kGenieSqueeze = 0.12f;


static void FreeSurf(Surf& s) {
    if (s.gfx) {
        Gp.DeleteGraphics(s.gfx);
        s.gfx = nullptr;
    }
    if (s.bmp) {
        Gp.DisposeImage(s.bmp);
        s.bmp = nullptr;
    }
    if (s.dc && s.old) SelectObject(s.dc, s.old);
    if (s.dib) DeleteObject(s.dib);
    if (s.dc) DeleteDC(s.dc);
    s.dib = nullptr;
    s.dc = nullptr;
    s.old = nullptr;
    s.w = s.h = 0;
}

static void SurfCaps(int& capW, int& capH) {
    RECT wa = WorkArea();
    if (P->horiz) {
        capW = wa.right - wa.left;
        capH = FullW() * 3 / 2 + Sc(8);
    } else {
        capW = FullW() * 3 / 2 + Sc(8);
        capH = wa.bottom - wa.top;
    }
}

static bool EnsureSurf(Surf& s, int w, int h) {
    if (s.dib && s.gfx && w <= s.w && h <= s.h) return true;

    int capW, capH;
    SurfCaps(capW, capH);
    int nw = std::max(std::max(w, s.w), capW);
    int nh = std::max(std::max(h, s.h), capH);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = nw;
    bi.bmiHeader.biHeight = -nh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP nb = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!nb || !bits) {
        if (nb) DeleteObject(nb);
        return false;
    }

    if (s.gfx) {
        Gp.DeleteGraphics(s.gfx);
        s.gfx = nullptr;
    }
    if (s.bmp) {
        Gp.DisposeImage(s.bmp);
        s.bmp = nullptr;
    }
    if (!s.dc) s.dc = CreateCompatibleDC(nullptr);
    HGDIOBJ old = SelectObject(s.dc, nb);
    if (!s.old) s.old = old;
    if (s.dib) DeleteObject(s.dib);
    s.dib = nb;
    s.w = nw;
    s.h = nh;

    const int kPixelFormat32bppPARGB = 0x000E200B;
    if (Gp.CreateBitmapFromScan0(nw, nh, nw * 4, kPixelFormat32bppPARGB, (BYTE*)bits, &s.bmp) != 0 ||
        !s.bmp) {
        return false;
    }
    if (Gp.GetImageGraphicsContext(s.bmp, &s.gfx) != 0 || !s.gfx) return false;
    Gp.SetSmoothing(s.gfx, 4);
    Gp.SetInterp(s.gfx, 3);
    Gp.SetPixOff(s.gfx, 2);
    return true;
}

static void FreeSurface() {
    if (P->gfx) {
        Gp.DeleteGraphics(P->gfx);
        P->gfx = nullptr;
    }
    if (P->surf) {
        Gp.DisposeImage(P->surf);
        P->surf = nullptr;
    }
}

static void FreeDib() {
    FreeSurf(P->warp);
    FreeSurface();
    if (P->memDc && P->oldBmp) SelectObject(P->memDc, P->oldBmp);
    if (P->dib) DeleteObject(P->dib);
    if (P->memDc) DeleteDC(P->memDc);
    P->dib = nullptr;
    P->memDc = nullptr;
    P->oldBmp = nullptr;
    P->dibW = P->dibH = 0;
}

static bool EnsureSurface(int w, int h) {
    if (P->dib && P->gfx && w <= P->dibW && h <= P->dibH) return true;

    int capW, capH;
    SurfCaps(capW, capH);
    int nw = std::max(std::max(w, P->dibW), capW);
    int nh = std::max(std::max(h, P->dibH), capH);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = nw;
    bi.bmiHeader.biHeight = -nh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP nb = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!nb || !bits) {
        if (nb) DeleteObject(nb);
        return false;
    }

    FreeSurface();
    if (!P->memDc) P->memDc = CreateCompatibleDC(nullptr);
    HGDIOBJ old = SelectObject(P->memDc, nb);
    if (!P->oldBmp) P->oldBmp = old;
    if (P->dib) DeleteObject(P->dib);
    P->dib = nb;
    P->dibW = nw;
    P->dibH = nh;

    const int kPixelFormat32bppPARGB = 0x000E200B;
    if (Gp.CreateBitmapFromScan0(nw, nh, nw * 4, kPixelFormat32bppPARGB, (BYTE*)bits, &P->surf) != 0 ||
        !P->surf) {
        return false;
    }
    if (Gp.GetImageGraphicsContext(P->surf, &P->gfx) != 0 || !P->gfx) return false;
    Gp.SetSmoothing(P->gfx, 4);
    Gp.SetInterp(P->gfx, 7);
    Gp.SetPixOff(P->gfx, 2);
    return true;
}

static void* RoundPath(float x, float y, float w, float h, float r) {
    void* p = nullptr;
    if (Gp.CreatePath(0, &p) != 0 || !p) return nullptr;
    r = std::max(0.5f, std::min(r, std::min(w, h) / 2.0f));
    float d = r * 2;
    Gp.AddArc(p, x, y, d, d, 180, 90);
    Gp.AddArc(p, x + w - d, y, d, d, 270, 90);
    Gp.AddArc(p, x + w - d, y + h - d, d, d, 0, 90);
    Gp.AddArc(p, x, y + h - d, d, d, 90, 90);
    Gp.ClosePathFig(p);
    return p;
}

static void DrawSpinner(void* g, float cx, float cy, float size) {
    if (size < 6.0f) return;
    float r = size * 0.28f;
    float w = std::max(2.0f, size * 0.09f);
    float angle = (float)fmod(NowSec() * 420.0, 360.0);
    for (int pass = 0; pass < 2; pass++) {
        void* path = nullptr;
        if (Gp.CreatePath(0, &path) != 0 || !path) continue;
        Gp.AddArc(path, cx - r, cy - r, r * 2.0f, r * 2.0f, pass == 0 ? 0.0f : angle,
                  pass == 0 ? 359.9f : 100.0f);
        void* pen = nullptr;
        if (Gp.CreatePen(Argb(g_cfg.accent, pass == 0 ? 70 : 255), w, 2, &pen) == 0 && pen) {
            Gp.SetPenStartCap(pen, 2);
            Gp.SetPenEndCap(pen, 2);
            Gp.DrawPath(g, pen, path);
            Gp.DelPen(pen);
        }
        Gp.DeletePath(path);
    }
}

static void FillRound(void* g, float x, float y, float w, float h, float r, COLORREF c, int a) {
    void* path = RoundPath(x, y, w, h, r);
    if (!path) return;
    void* br = nullptr;
    if (Gp.CreateFill(Argb(c, a), &br) == 0 && br) {
        Gp.FillPath(g, br, path);
        Gp.DelBrush(br);
    }
    Gp.DeletePath(path);
}

static float ExpandFactor() {
    if (!g_cfg.autoHide) return 1.0f;
    double full = FullW(), strip = StripW();
    return std::clamp((float)((P->curW - strip) / (full - strip)), 0.0f, 1.0f);
}

static void DrawHandle(void* g, int pw, int ph, float hf, bool horiz) {
    if (hf >= 0.98f) return;
    void* pen = nullptr;
    if (Gp.CreatePen(Argb(g_cfg.accent, (int)(150 * (1.0f - hf))), (float)std::max(2, Sc(2)), 2,
                     &pen) == 0 && pen) {
        Gp.SetPenStartCap(pen, 2);
        Gp.SetPenEndCap(pen, 2);
        float cx = pw / 2.0f, cy = ph / 2.0f, half = (float)Sc(18);
        if (horiz) {
            Gp.DrawLine(g, pen, cx - half, cy, cx + half, cy);
        } else {
            Gp.DrawLine(g, pen, cx, cy - half, cx, cy + half);
        }
        Gp.DelPen(pen);
    }
}

static bool WarpTo(int wOut, int h, float p, float eReal) {
    const bool horiz = P->horiz;
    int pw = horiz ? h : wOut;
    int ph = horiz ? wOut : h;
    if (!EnsureSurf(P->warp, pw, ph)) return false;
    void* gw = P->warp.gfx;
    Gp.Clear(gw, 0x00000000);

    void* attrs = nullptr;
    Gp.CreateImageAttributes(&attrs);
    if (attrs) {
        Gp.SetImageAttrWrapMode(attrs, 3, 0, FALSE);
        float a = SmoothStep(p / 0.35f);
        if (a < 0.999f) {
            float m[5][5] = {{1, 0, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0},
                             {0, 0, 0, a, 0}, {0, 0, 0, 0, 1}};
            Gp.SetImageAttrColorMatrix(attrs, 1, TRUE, m, nullptr, 0);
        }
    }

    float sw = (float)FullW() / (float)wOut;
    for (int c = 0; c < wOut; c++) {
        float t = (c + 0.5f) / (float)wOut;
        float u = P->atMax ? 1.0f - t : t;
        float q = std::clamp(p * (1.0f + kGenieTail) - (1.0f - u) * kGenieTail, 0.0f, 1.0f);
        float sc = kGenieSqueeze + (1.0f - kGenieSqueeze) * q * q * (3.0f - 2.0f * q);
        float dh = sc * (float)h;
        float off = ((float)h - dh) / 2.0f;
        if (!horiz) {
            Gp.DrawImageRectRect(gw, P->surf, (float)c, off, 1.2f, dh, c * sw, 0.0f, sw, (float)h,
                                 2, attrs, nullptr, nullptr);
        } else {
            Gp.DrawImageRectRect(gw, P->surf, off, (float)c, dh, 1.2f, 0.0f, c * sw, (float)h, sw,
                                 2, attrs, nullptr, nullptr);
        }
    }
    if (attrs) Gp.DisposeImageAttributes(attrs);
    DrawHandle(gw, pw, ph, eReal, horiz);
    return true;
}

static void Render() {
    if (!P->hwnd || !Gp.ok || P->curW <= 0) return;

    const bool horiz = P->horiz;
    const bool atMax = P->atMax;
    const int style = ((g_cfg.style == 1 || g_cfg.style == 2) && !Expanded()) ? 0 : g_cfg.style;
    const bool jelly = style == 1;
    const bool ripple = style == 2;
    const bool genie = style == 4;
    const bool oval = style == 5;
    const bool drop = style == 6;

    int hFull = (int)(P->curH + 0.5);
    float eReal = ExpandFactor();
    float introK = std::clamp(P->intro, 0.0f, 1.0f);
    float p = std::min(eReal, introK);

    int w = (int)(P->curW + 0.5), h = hFull;
    float ovalR = 0.0f;
    float shapeX = 0.0f, shapeY = 0.0f, shapeW = 0.0f, shapeH = 0.0f;
    bool dropNeck = false;
    float dropXa = 0.0f, dropXb = 0.0f, dropRa = 0.0f, dropRb = 0.0f, dropHn = 0.0f, dropCy = 0.0f,
          dropCx = 0.0f;
    float jT = 0.0f, jSp = 1.0f, jHs = 1.0f, rFy = 0.0f, rF = 0.0f;
    if (jelly || ripple) jT = LinearT(p);
    if (jelly) {
        jSp = jT >= 0.999f ? 1.0f : JellySpring(jT / 0.9f);
        float jig = jSp - 1.0f;
        float sw = (float)StripW() + (float)(FullW() - StripW()) * jSp;
        w = std::max(2, (int)(std::max(sw, 0.0f) + 0.5f));
        jHs = std::clamp(1.0f - jig * 0.5f, 0.6f, 1.12f);
        h = std::max(2, (int)((float)hFull * jHs + 0.5f));
    } else if (ripple) {
        float pe = 1.0f - powf(1.0f - std::clamp(jT / 0.3f, 0.0f, 1.0f), 3.0f);
        float sw = (float)StripW() + (float)(FullW() - StripW()) * pe;
        w = std::max(2, (int)(sw + 0.5f));
        rF = std::clamp((jT - 0.1f) / 0.75f, 0.0f, 1.0f);
        rFy = -(float)Sc(40) + ((float)hFull + (float)Sc(170)) * rF;
    } else if (genie) {
        w = FullW();
    } else if (oval) {
        float u1 = SmoothStep(p / 0.45f);
        float u2 = SmoothStep((p - 0.35f) / 0.65f);
        float sw = (float)StripW() + ((float)Sc(28) - (float)StripW()) * u1;
        float sh = (float)Sc(40) + ((float)Sc(48) - (float)Sc(40)) * u1;
        sw += ((float)FullW() - sw) * u2;
        sh += ((float)hFull - sh) * u2;
        w = std::max(2, (int)(sw + 0.5f));
        h = std::max(2, (int)(sh + 0.5f));
        ovalR = (std::min(sw, sh) / 2.0f) * (1.0f - u2) + (float)Sc(g_cfg.radius) * u2;
    } else if (drop) {
        float q1 = SmoothStep(p / 0.5f);
        float q2 = SmoothStep((p - 0.45f) / 0.55f);
        float fullW = (float)FullW();
        float rB = (float)Sc(10) + (float)Sc(4) * q1;
        float dB = (float)Sc(6) + q1 * (fullW * 0.62f + (float)Sc(4));
        float dr = (dB - rB) * (1.0f - q2);
        float dl = (dB + rB) + (fullW - (dB + rB)) * q2;
        float hh = rB + ((float)hFull / 2.0f - rB) * q2;
        float ra = (float)Sc(9);
        w = std::max(2, (int)(dl + 0.5f) + 1);
        h = std::max(2, (int)(std::max(hh, ra) * 2.0f + 0.5f) + 2);
        auto X = [&](float d) { return atMax ? (float)w - d : d; };
        float x1 = X(dr), x2 = X(dl);
        shapeX = std::min(x1, x2);
        shapeW = fabsf(x1 - x2);
        dropCy = h / 2.0f;
        shapeY = dropCy - hh;
        shapeH = hh * 2.0f;
        ovalR = rB + ((float)Sc(g_cfg.radius) - rB) * q2;
        dropNeck = q2 < 0.25f && q1 < 0.98f;
        dropXa = X(-1.0f);
        dropXb = X(dB);
        dropRa = ra;
        dropRb = rB;
        dropHn = std::max(0.6f, (1.0f - q1) * (float)Sc(6));
        dropCx = (x1 + x2) / 2.0f;
    }
    if (!drop) {
        shapeX = 0.0f;
        shapeY = 0.0f;
        shapeW = (float)w;
        shapeH = (float)h;
    }
    if (w < 2 || h < 2) return;

    int pw = horiz ? h : w;
    int ph = horiz ? w : h;
    if (!EnsureSurface(pw, ph)) return;

    float qx = horiz ? shapeY : shapeX;
    float qy = horiz ? shapeX : shapeY;
    float qw = horiz ? shapeH : shapeW;
    float qh = horiz ? shapeW : shapeH;

    void* g = P->gfx;
    Gp.Clear(g, 0x00000000);

    int bgA = std::clamp(g_cfg.opacity, 10, 100) * 255 / 100;
    int borderA = std::max(bgA, 90);

    const float baseR = (float)Sc(g_cfg.radius);
    float R = std::min(baseR, std::min(w, h) / 2.0f);
    if (oval || drop) R = ovalR;
    if (jelly) {
        R = std::min(std::clamp(baseR + (jSp - 1.0f) * (float)Sc(22), std::min((float)Sc(4), baseR), std::max((float)Sc(40), baseR)),
                     std::min(w, h) / 2.0f);
    }

    auto Pt = [&](float lx, float ly) { return horiz ? GpPointF{ly, lx} : GpPointF{lx, ly}; };

    if (drop && dropNeck) {
        void* nb = nullptr;
        if (Gp.CreateFill(Argb(g_cfg.bg, bgA), &nb) == 0 && nb) {
            float cy = dropCy, xm = (dropXa + dropXb) / 2.0f;
            auto quad = [&](void* path, float x0, float y0, float qx2, float qy2, float x1, float y1) {
                GpPointF a = Pt(x0, y0);
                GpPointF b = Pt(x0 + 2.0f / 3.0f * (qx2 - x0), y0 + 2.0f / 3.0f * (qy2 - y0));
                GpPointF c = Pt(x1 + 2.0f / 3.0f * (qx2 - x1), y1 + 2.0f / 3.0f * (qy2 - y1));
                GpPointF d = Pt(x1, y1);
                Gp.AddPathBezier(path, a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y);
            };
            void* np = nullptr;
            if (Gp.CreatePath(0, &np) == 0 && np) {
                quad(np, dropXa, cy - dropRa * 0.8f, xm, cy - dropHn, dropXb, cy - dropRb * 0.8f);
                GpPointF l1 = Pt(dropXb, cy - dropRb * 0.8f), l2 = Pt(dropXb, cy + dropRb * 0.8f);
                Gp.AddPathLine(np, l1.x, l1.y, l2.x, l2.y);
                quad(np, dropXb, cy + dropRb * 0.8f, xm, cy + dropHn, dropXa, cy + dropRa * 0.8f);
                Gp.ClosePathFig(np);
                Gp.FillPath(g, nb, np);
                Gp.DeletePath(np);
            }
            void* ep = nullptr;
            if (Gp.CreatePath(0, &ep) == 0 && ep) {
                GpPointF ec = Pt(dropXa, cy);
                Gp.AddPathEllipse(ep, ec.x - dropRa, ec.y - dropRa, dropRa * 2.0f, dropRa * 2.0f);
                Gp.FillPath(g, nb, ep);
                Gp.DeletePath(ep);
            }
            Gp.DelBrush(nb);
        }
    }

    if (ripple) {
        float cl = std::max(rFy, 1.0f);
        void* rc = RoundPath(0.0f, 0.0f, horiz ? cl : (float)pw, horiz ? (float)ph : cl, 0.5f);
        if (rc) {
            Gp.SetClipPath(g, rc, 0);
            Gp.DeletePath(rc);
        }
    }

    void* bgPath = RoundPath(qx + 0.5f, qy + 0.5f, qw - 1.0f, qh - 1.0f, R);
    if (bgPath) {
        void* br = nullptr;
        if (g_cfg.glass) {
            GpPointF p1{0.0f, qy}, p2{0.0f, qy + qh};
            Gp.CreateLineBrush(&p1, &p2, Argb(Mix(g_cfg.bg, RGB(255, 255, 255), 0.14f), bgA),
                               Argb(g_cfg.bg, bgA), 0, &br);
        } else {
            Gp.CreateFill(Argb(g_cfg.bg, bgA), &br);
        }
        if (br) {
            Gp.FillPath(g, br, bgPath);
            Gp.DelBrush(br);
        }
        Gp.DeletePath(bgPath);
    }

    if (g_cfg.glass) {
        float sheenH = qh * 0.5f;
        void* sheen = RoundPath(qx + 1.0f, qy + 1.0f, qw - 2.0f, sheenH, R);
        if (sheen) {
            GpPointF p1{0.0f, qy}, p2{0.0f, qy + sheenH};
            void* sb = nullptr;
            if (Gp.CreateLineBrush(&p1, &p2, Argb(RGB(255, 255, 255), 46), Argb(RGB(255, 255, 255), 0),
                                   0, &sb) == 0 && sb) {
                Gp.FillPath(g, sb, sheen);
                Gp.DelBrush(sb);
            }
            Gp.DeletePath(sheen);
        }
    }

    void* borderPath = RoundPath(qx + 0.5f, qy + 0.5f, qw - 1.0f, qh - 1.0f, R);
    if (borderPath) {
        void* pen = nullptr;
        UINT32 bc = g_cfg.glass ? Argb(RGB(255, 255, 255), 70) : Argb(g_cfg.border, borderA);
        if (Gp.CreatePen(bc, 1.0f, 2, &pen) == 0 && pen) {
            Gp.DrawPath(g, pen, borderPath);
            Gp.DelPen(pen);
        }
        Gp.DeletePath(borderPath);
    }

    if (ripple) {
        Gp.ResetClip(g);
        float la = (1.0f - rF) * std::clamp(jT * 6.0f, 0.0f, 1.0f);
        if (la > 0.02f && rFy > 0.0f && rFy < (float)hFull + (float)Sc(10)) {
            void* lp = nullptr;
            if (Gp.CreatePen(Argb(g_cfg.accent, (int)(190.0f * la)), (float)std::max(2, Sc(2)), 2, &lp) == 0 &&
                lp) {
                Gp.SetPenStartCap(lp, 2);
                Gp.SetPenEndCap(lp, 2);
                GpPointF a = horiz ? GpPointF{rFy, (float)Sc(4)} : GpPointF{(float)Sc(4), rFy};
                GpPointF b = horiz ? GpPointF{rFy, (float)w - Sc(4)} : GpPointF{(float)w - Sc(4), rFy};
                Gp.DrawLine(g, lp, a.x, a.y, b.x, b.y);
                Gp.DelPen(lp);
            }
        }
    }

    float e = (genie || oval || drop) ? 1.0f : eReal;

    if (!genie && !drop) {
        DrawHandle(g, pw, ph, oval ? std::clamp(p * 3.0f, 0.0f, 1.0f) : eReal, horiz);
    }

    if (oval || drop) {
        void* clip = RoundPath(qx + 1.0f, qy + 1.0f, qw - 2.0f, qh - 2.0f, std::max(0.5f, R - 1.0f));
        if (clip) {
            Gp.SetClipPath(g, clip, 0);
            Gp.DeletePath(clip);
        }
        float tx = drop ? dropCx - FullW() / 2.0f : (w - FullW()) / 2.0f;
        float ty = (h - hFull) / 2.0f;
        if (horiz) {
            Gp.TranslateWorld(g, ty, tx, 0);
        } else {
            Gp.TranslateWorld(g, tx, ty, 0);
        }
    }

    if (e > 0.02f && (!(oval || drop) || p > (oval ? 0.3f : 0.55f))) {
        float s = (float)SlotSize(), m = (float)Margin(), px = (float)IconPx();

        float offX = (atMax || oval || drop) ? 0.0f : (float)(w - FullW());
        if (ripple) offX = atMax ? (float)(w - FullW()) : 0.0f;
        float left = jelly ? ((float)w - s) / 2.0f : (float)CrossOff() + offX;
        int nItems = (int)P->items.size();

        for (int i = 0; i < TotalSlots(); i++) {
            bool isPlus = i == nItems;
            float pos = isPlus ? P->plus.pos : P->items[i].pos;
            float hov = isPlus ? P->plus.hover : P->items[i].hover;
            float app = isPlus ? 1.0f : P->items[i].appear;

            float top = (m + pos * s) * jHs;
            float sA = s * jHs;
            float cx = left + s / 2.0f, cy = top + sA / 2.0f;

            if (hov > 0.01f) {
                if (horiz) {
                    FillRound(g, top + 1, left + 1, sA - 2, s - 2, std::min((float)Sc(10), (float)Sc(g_cfg.radius)), g_cfg.hover,
                              (int)(hov * 255));
                } else {
                    FillRound(g, left + 1, top + 1, s - 2, sA - 2, std::min((float)Sc(10), (float)Sc(g_cfg.radius)), g_cfg.hover,
                              (int)(hov * 255));
                }
            }

            float aspX = 1.0f, aspY = 1.0f;
            float prog = e;
            float delay = std::min(i, 6) * 0.06f;
            float pop = std::clamp((prog - delay) / (1.0f - delay), 0.0f, 1.0f);

            float zoom = 1.0f + 0.15f * hov;
            if (g_cfg.magnify) {
                float d = (cy - P->magY) / s;
                zoom = 1.0f + 0.4f * P->magAmt * expf(-d * d / 1.1f);
            }

            float scale;
            if (jelly) {
                float u = (jT - (float)std::min(i, 6) * 0.05f) / 0.9f;
                float s2 = jT >= 0.999f ? 1.0f : JellySpring(u);
                float j = s2 - 1.0f;
                scale = zoom * app * std::clamp(s2, 0.0f, 1.4f);
                aspX = std::max(0.3f, 1.0f + j * 0.35f);
                aspY = std::max(0.3f, 1.0f - j * 0.35f);
            } else if (ripple) {
                float d = (rFy - cy) / (float)Sc(60);
                float ap = std::clamp((d + 0.3f) / 0.8f, 0.0f, 1.0f);
                float bump = (jT >= 0.999f || d <= -0.3f) ? 0.0f : expf(-powf((d - 0.5f) / 0.55f, 2.0f));
                scale = zoom * app * SmoothStep(ap) * (1.0f + 0.4f * bump);
                float dx = (float)Sc(12) * bump;
                cx += atMax ? -dx : dx;
            } else if (style == 3) {
                scale = zoom * app;
            } else if (oval || drop) {
                float from = oval ? 0.3f : 0.55f;
                float po = std::clamp(((p - from) / (1.0f - from) - delay) / (1.0f - delay), 0.0f, 1.0f);
                scale = zoom * app * (1.0f - (1.0f - po) * (1.0f - po));
            } else {
                scale = zoom * app * (1.0f - (1.0f - pop) * (1.0f - pop));
            }

            if (!isPlus && i < nItems) {
                float pa = P->items[i].pressA;
                if (pa > 0.001f) {
                    bool down = P->pressIdx == i && !g_dragActive;
                    float bounce = (!down && pa < 0.999f) ? 0.10f * sinf(3.14159265f * (1.0f - pa)) : 0.0f;
                    scale *= 1.0f - 0.12f * pa + bounce;
                }
            }

            float pcx = horiz ? cy : cx;
            float pcy = horiz ? cx : cy;

            if (isPlus) {
                float arm = px * 0.25f * scale;
                void* pen = nullptr;
                if (Gp.CreatePen(Argb(g_cfg.accent, 255), (float)std::max(2, Sc(2)), 2, &pen) == 0 &&
                    pen) {
                    Gp.SetPenStartCap(pen, 2);
                    Gp.SetPenEndCap(pen, 2);
                    Gp.DrawLine(g, pen, pcx - arm, pcy, pcx + arm, pcy);
                    Gp.DrawLine(g, pen, pcx, pcy - arm, pcx, pcy + arm);
                    Gp.DelPen(pen);
                }
            } else {
                float sz = px * scale;

                void* im = (scale > 1.02f && P->items[i].imgBig) ? P->items[i].imgBig : P->items[i].img;
                if (!im) im = P->items[i].imgBig;
                float szA = sz * aspX, szB = sz * aspY;
                float dw = horiz ? szB : szA, dh = horiz ? szA : szB;
                if (P->items[i].loading) {
                    DrawSpinner(g, pcx, pcy, sz);
                } else if (std::min(dw, dh) >= 2.0f && im) {
                    Gp.DrawImageRect(g, im, pcx - dw / 2, pcy - dh / 2, dw, dh);
                }
                float ra = P->items[i].runA * std::clamp(scale, 0.0f, 1.0f);
                if (g_cfg.showRunning && ra > 0.02f) {
                    float dr = std::max(1.5f, (float)Sc(2));
                    float edgeCross = atMax ? left + s - (float)Sc(4) : left + (float)Sc(4);
                    float dcx = horiz ? cy : edgeCross;
                    float dcy = horiz ? edgeCross : cy;
                    void* dpath = nullptr;
                    void* dbr = nullptr;
                    if (Gp.CreatePath(0, &dpath) == 0 && dpath &&
                        Gp.CreateFill(Argb(g_cfg.accent, (int)(230.0f * ra)), &dbr) == 0 && dbr) {
                        Gp.AddArc(dpath, dcx - dr, dcy - dr, dr * 2.0f, dr * 2.0f, 0.0f, 359.9f);
                        Gp.FillPath(g, dbr, dpath);
                    }
                    if (dbr) Gp.DelBrush(dbr);
                    if (dpath) Gp.DeletePath(dpath);
                }
            }
        }
    }

    if (oval || drop) {
        Gp.ResetWorld(g);
        Gp.ResetClip(g);
    }

    GdiFlush();

    HDC srcDc = P->memDc;
    int outW = w;
    if (genie && p < 0.999f) {
        int wOut = std::max(2, (int)((float)StripW() + (float)(FullW() - StripW()) * p + 0.5f));
        if (WarpTo(wOut, h, p, eReal)) {
            srcDc = P->warp.dc;
            outW = wOut;
        }
    }

    RECT wa = WorkArea();
    float introAlpha = introK;
    int slideOff = 0;
    if (style == 0) {
        slideOff = (int)((1.0f - introK) * (float)Sc(26));
    } else if (jelly || ripple) {
        introAlpha = std::clamp(introK * 3.0f, 0.0f, 1.0f);
    } else if (genie) {
        introAlpha = 1.0f;
    } else if (oval || drop) {
        introAlpha = std::clamp(introK * 3.0f, 0.0f, 1.0f);
    }

    int physW = horiz ? h : outW;
    int physH = horiz ? outW : h;
    POINT dst;
    if (!horiz) {
        dst.x = atMax ? wa.right - physW + slideOff : wa.left - slideOff;
        dst.y = wa.top + ((wa.bottom - wa.top) - physH) / 2;
    } else {
        dst.x = wa.left + ((wa.right - wa.left) - physW) / 2;
        dst.y = atMax ? wa.bottom - physH + slideOff : wa.top - slideOff;
    }
    SIZE sz{physW, physH};
    POINT src{0, 0};
    BYTE introA = (BYTE)(255.0f * introAlpha + 0.5f);
    BLENDFUNCTION bf{AC_SRC_OVER, 0, introA, AC_SRC_ALPHA};
    UpdateLayeredWindow(P->hwnd, nullptr, &dst, &sz, srcDc, &src, 0, &bf, ULW_ALPHA);
}

static float Approach(float cur, float tgt, float k) {
    if (k >= 0.999f) return tgt;
    float d = tgt - cur;
    if (fabsf(d) < 0.003f) return tgt;
    return cur + d * k;
}

static void AnimStep() {
    double now = NowSec();
    float dt = (float)std::min(now - P->lastT, 0.05);
    P->lastT = now;
    float k = g_cfg.anim ? 1.0f - expf(-dt * g_cfg.speed / StyleTau()) : 1.0f;
    bool busy = false;

    auto stepF = [&](float& v, float t) {
        v = Approach(v, t, k);
        if (v != t) busy = true;
    };
    auto stepD = [&](double& v, double t, float kk) {
        double d = t - v;
        if (kk >= 0.999f || fabs(d) < 0.25) {
            v = t;
        } else {
            v += d * kk;
            busy = true;
        }
    };

    float kE = k;
    if ((g_cfg.style == 1 || g_cfg.style == 2) && g_cfg.anim && P->tgtW >= P->curW) {
        kE = 1.0f - expf(-dt * g_cfg.speed / ExpandTau());
    }

    stepD(P->curW, P->tgtW, kE);
    stepD(P->curH, P->tgtH, k);

    int n = (int)P->items.size();
    for (int i = 0; i < n; i++) {
        if (!(g_dragActive && P == g_dragOwner && i == g_dragIdx)) stepF(P->items[i].pos, (float)i);
        stepF(P->items[i].hover, P->hover == i ? 1.0f : 0.0f);
        stepF(P->items[i].appear, 1.0f);
        stepF(P->items[i].runA, (g_cfg.showRunning && P->items[i].running) ? 1.0f : 0.0f);
        {
            float target = (P->pressIdx == i && !g_dragActive) ? 1.0f : 0.0f;
            float kp = g_cfg.anim ? 1.0f - expf(-dt * 12.0f) : 1.0f;
            P->items[i].pressA = Approach(P->items[i].pressA, target, kp);
            if (P->items[i].pressA != target) busy = true;
        }
    }
    stepF(P->plus.pos, (float)n);
    if (g_dragActive) busy = true;
    stepF(P->plus.hover, P->hover == n ? 1.0f : 0.0f);
    P->intro = Approach(P->intro, 1.0f, kE);
    if (P->intro != 1.0f) busy = true;
    if (g_cfg.magnify) {
        stepF(P->magAmt, P->mouseY >= 0.0f ? 1.0f : 0.0f);
        if (P->mouseY >= 0.0f) stepF(P->magY, P->mouseY);
    }

    bool spinOnly = false;
    if (P->curW > StripW() + 1) {
        for (auto& it : P->items) {
            if (it.loading) {
                spinOnly = !busy;
                busy = true;
                break;
            }
        }
    }

    static unsigned spinTick = 0;
    if (!spinOnly || (spinTick++ & 1) == 0) Render();

    if (!busy) P->animRunning = false;
}

static void StartAnim() {
    if (!P->hwnd || P->animRunning) return;
    P->lastT = NowSec() - 0.016;
    P->animRunning = true;
}

static void Relayout() {
    if (!P->hwnd) return;
    P->tgtW = Expanded() ? FullW() : StripW();
    P->tgtH = TotalSlots() * SlotSize() + Margin() * 2;
    if (P->curW <= 0) {
        P->curW = P->tgtW;
        P->curH = P->tgtH;
        P->intro = g_cfg.anim ? 0.0f : 1.0f;
        Render();
    }
    UpdateTooltips();
    StartAnim();
}

static void Msg(const std::wstring& text) {
    bool prev = g_modal;
    g_modal = true;
    MessageBoxW(P->hwnd, text.c_str(), L"Quick Launch Dock", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
    g_modal = prev;
}

static bool AddPath(std::wstring p) {
    while (!p.empty() && (p.front() == L'"' || iswspace(p.front()))) p.erase(p.begin());
    while (!p.empty() && (p.back() == L'"' || iswspace(p.back()))) p.pop_back();
    if (p.empty()) return false;

    wchar_t exp[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(p.c_str(), exp, ARRAYSIZE(exp));
    if (n && n <= ARRAYSIZE(exp)) p = exp;

    if (GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Msg(std::wstring(Tr(L"Path not found:", L"Путь не найден:")) + L"\n" + p);
        return false;
    }
    if ((int)P->items.size() >= MaxItems()) {
        Msg(Tr(L"There is no more room on the dock.", L"На панели больше нет места."));
        return false;
    }
    for (auto& it : P->items)
        if (_wcsicmp(it.path.c_str(), p.c_str()) == 0) return false;

    Item it;
    it.path = p;
    it.name = TitleOf(p);
    LoadImages(it);
    it.pos = (float)P->items.size();
    it.appear = g_cfg.anim ? 0.0f : 1.0f;
    P->items.push_back(std::move(it));
    return true;
}

static bool AddUrl(const std::wstring& raw) {
    std::wstring u = NormalizeUrl(raw);
    if (u.empty()) {
        Msg(Tr(L"This does not look like a web address.", L"Это не похоже на адрес сайта."));
        return false;
    }
    if ((int)P->items.size() >= MaxItems()) {
        Msg(Tr(L"There is no more room on the dock.", L"На панели больше нет места."));
        return false;
    }
    for (auto& it : P->items)
        if (_wcsicmp(it.path.c_str(), u.c_str()) == 0) return false;

    Item it;
    it.path = u;
    it.name = TitleOf(u);
    LoadImages(it);
    it.pos = (float)P->items.size();
    it.appear = g_cfg.anim ? 0.0f : 1.0f;
    P->items.push_back(std::move(it));
    return true;
}

static void Commit() {
    SaveItems();
    Relayout();
}

static void DragStart() {
    g_dragActive = true;
    g_dragOwner = P;
    g_dragIdx = P->pressIdx;
    for (Panel* pn : std::vector<Panel*>(g_panels)) {
        PanelScope sc(pn);
        pn->outsideTicks = 0;
        if (!pn->expanded) {
            pn->expanded = true;
            Relayout();
        }
    }
}

static bool PanelHasPath(Panel* pn, const std::wstring& path) {
    for (auto& it : pn->items) {
        if (_wcsicmp(it.path.c_str(), path.c_str()) == 0) return true;
    }
    return false;
}

static bool PanelAlive(Panel* pn) {
    return pn && std::find(g_panels.begin(), g_panels.end(), pn) != g_panels.end();
}

static void DragUpdate(POINT sp) {
    Panel* owner = g_dragOwner;
    if (!PanelAlive(owner) || g_dragIdx < 0 || g_dragIdx >= (int)owner->items.size()) return;

    Panel* target = owner;
    RECT r{};
    bool inOwner = owner->hwnd && GetWindowRect(owner->hwnd, &r) && PtInRect(&r, sp);
    if (!inOwner) {
        for (Panel* pn : g_panels) {
            if (pn == owner || !pn->hwnd || !IsWindowVisible(pn->hwnd)) continue;
            RECT wr{};
            if (GetWindowRect(pn->hwnd, &wr) && PtInRect(&wr, sp)) {
                target = pn;
                break;
            }
        }
    }

    if (target != owner) {
        bool blocked = false;
        Item moved = owner->items[g_dragIdx];
        {
            PanelScope st(target);
            blocked = PanelHasPath(target, moved.path) || (int)target->items.size() >= MaxItems();
        }
        if (!blocked) {
            owner->items.erase(owner->items.begin() + g_dragIdx);
            {
                PanelScope so(owner);
                owner->hover = -1;
                owner->mouseY = -1.0f;
                Commit();
            }
            PanelScope st(target);
            POINT cp = sp;
            ScreenToClient(target->hwnd, &cp);
            float along = target->horiz ? (float)cp.x : (float)cp.y;
            float s = (float)SlotSize(), m = (float)Margin();
            int idx = std::clamp((int)floorf((along - m) / s), 0, (int)target->items.size());
            moved.appear = 1.0f;
            moved.hover = 0.0f;
            target->items.insert(target->items.begin() + idx, moved);
            Commit();
            g_dragOwner = target;
            g_dragIdx = idx;
            owner = target;
        }
    }

    {
        PanelScope sc(owner);
        POINT cp = sp;
        ScreenToClient(owner->hwnd, &cp);
        float along = owner->horiz ? (float)cp.x : (float)cp.y;
        int n = (int)owner->items.size();
        float s = (float)SlotSize(), m = (float)Margin();
        int idx = std::clamp((int)floorf((along - m) / s), 0, n - 1);
        if (idx != g_dragIdx) {
            Item moved = owner->items[g_dragIdx];
            owner->items.erase(owner->items.begin() + g_dragIdx);
            owner->items.insert(owner->items.begin() + idx, moved);
            g_dragIdx = idx;
        }
        owner->items[g_dragIdx].pos = std::clamp((along - m) / s - 0.5f, 0.0f, (float)(n - 1));
        owner->hover = g_dragIdx;
        if (g_cfg.magnify) {
            if (owner->magAmt < 0.05f) owner->magY = along;
            owner->mouseY = along;
        }
        StartAnim();
    }
    for (Panel* pn : std::vector<Panel*>(g_panels)) {
        if (pn == owner) continue;
        if (pn->hover != -1 || pn->mouseY >= 0.0f) {
            PanelScope so(pn);
            pn->hover = -1;
            pn->mouseY = -1.0f;
            StartAnim();
        }
    }
}

static void DragEnd() {
    Panel* owner = g_dragOwner;
    g_dragActive = false;
    g_dragOwner = nullptr;
    g_dragIdx = -1;
    for (Panel* pn : std::vector<Panel*>(g_panels)) {
        PanelScope sc(pn);
        pn->hover = -1;
        pn->mouseY = -1.0f;
        pn->outsideTicks = 0;
        StartAnim();
    }
    if (PanelAlive(owner)) {
        PanelScope so(owner);
        Commit();
    }
}

constexpr int IDC_URL_LABEL = 1101;
constexpr int IDC_URL_EDIT = 1102;

static INT_PTR CALLBACK UrlDlgProc(HWND dlg, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_INITDIALOG: {
            SetWindowLongPtrW(dlg, GWLP_USERDATA, lp);
            auto* text = (std::wstring*)lp;
            SetWindowTextW(dlg, Tr(L"Add a website", L"Добавить сайт"));
            SetDlgItemTextW(dlg, IDC_URL_LABEL,
                            Tr(L"Web address (for example https://example.com):",
                               L"Адрес сайта (например, https://example.com):"));
            SetDlgItemTextW(dlg, IDOK, Tr(L"Add", L"Добавить"));
            SetDlgItemTextW(dlg, IDCANCEL, Tr(L"Cancel", L"Отмена"));
            HWND ed = GetDlgItem(dlg, IDC_URL_EDIT);
            SendMessageW(ed, EM_SETLIMITTEXT, 2000, 0);
            SetWindowTextW(ed, text->c_str());
            SetForegroundWindow(dlg);
            SetFocus(ed);
            SendMessageW(ed, EM_SETSEL, 0, -1);
            return FALSE;
        }
        case WM_COMMAND:
            if (LOWORD(wp) == IDOK) {
                auto* text = (std::wstring*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
                wchar_t buf[2100] = L"";
                GetDlgItemTextW(dlg, IDC_URL_EDIT, buf, ARRAYSIZE(buf));
                if (text) *text = buf;
                EndDialog(dlg, 1);
                return TRUE;
            }
            if (LOWORD(wp) == IDCANCEL) {
                EndDialog(dlg, 0);
                return TRUE;
            }
            break;
    }
    return FALSE;
}

static std::wstring ClipboardText() {
    std::wstring t;
    if (OpenClipboard(P->hwnd)) {
        HANDLE h = GetClipboardData(CF_UNICODETEXT);
        if (h) {
            auto p = (const wchar_t*)GlobalLock(h);
            if (p) {
                t = p;
                GlobalUnlock(h);
            }
        }
        CloseClipboard();
    }
    size_t nl = t.find_first_of(L"\r\n");
    if (nl != std::wstring::npos) t.resize(nl);
    return t;
}

static void AddSite() {
    std::wstring text = ClipboardText();
    if (!LooksLikeUrl(text)) text.clear();

    std::vector<WORD> t;
    auto putStr = [&](PCWSTR s) {
        do {
            t.push_back((WORD)*s);
        } while (*s++);
    };
    auto putD = [&](DWORD d) {
        t.push_back(LOWORD(d));
        t.push_back(HIWORD(d));
    };
    auto align = [&]() {
        if (t.size() & 1) t.push_back(0);
    };
    putD((DWORD)(DS_MODALFRAME | DS_CENTER | DS_SETFONT | DS_SETFOREGROUND | WS_POPUP | WS_CAPTION |
                 WS_SYSMENU));
    putD((DWORD)WS_EX_TOPMOST);
    t.push_back(4);
    t.push_back(0);
    t.push_back(0);
    t.push_back(270);
    t.push_back(62);
    t.push_back(0);
    t.push_back(0);
    putStr(L"");
    t.push_back(9);
    putStr(L"Segoe UI");
    auto item = [&](DWORD style, int x, int y, int cx, int cy, WORD id, WORD cls) {
        align();
        putD(style | WS_CHILD | WS_VISIBLE);
        putD(0);
        t.push_back((WORD)x);
        t.push_back((WORD)y);
        t.push_back((WORD)cx);
        t.push_back((WORD)cy);
        t.push_back(id);
        t.push_back(0xFFFF);
        t.push_back(cls);
        putStr(L"");
        t.push_back(0);
    };
    item(SS_LEFT, 10, 8, 250, 10, IDC_URL_LABEL, 0x0082);
    item(ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP, 10, 20, 250, 14, IDC_URL_EDIT, 0x0081);
    item(BS_DEFPUSHBUTTON | WS_TABSTOP, 148, 41, 56, 14, IDOK, 0x0080);
    item(BS_PUSHBUTTON | WS_TABSTOP, 208, 41, 52, 14, IDCANCEL, 0x0080);

    g_modal = true;
    INT_PTR r = DialogBoxIndirectParamW(GetModuleHandleW(nullptr), (LPCDLGTEMPLATEW)t.data(),
                                        P->hwnd, UrlDlgProc, (LPARAM)&text);
    g_modal = false;
    if (r == 1 && !TrimUrl(text).empty() && AddUrl(text)) Commit();
}

static void PickAndAdd() {
    wchar_t file[MAX_PATH * 2] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = P->hwnd;
    ofn.lpstrFilter = IsRu() ? L"Программы и ярлыки\0*.exe;*.lnk;*.bat;*.cmd;*.url\0Все файлы\0*.*\0"
                            : L"Programs and shortcuts\0*.exe;*.lnk;*.bat;*.cmd;*.url\0All files\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.lpstrTitle = Tr(L"Choose a program", L"Выберите программу");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR |
                OFN_DONTADDTORECENT | OFN_NODEREFERENCELINKS;
    g_modal = true;
    BOOL ok = GetOpenFileNameW(&ofn);
    g_modal = false;
    if (ok && AddPath(file)) Commit();
}

static void ChangeIcon(int idx) {
    if (idx < 0 || idx >= (int)P->items.size()) return;
    wchar_t file[MAX_PATH * 2] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = P->hwnd;
    ofn.lpstrFilter = IsRu() ? L"Изображения\0*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.ico\0Все файлы\0*.*\0"
                            : L"Images\0*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.ico\0All files\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.lpstrTitle = Tr(L"Choose an icon image", L"Выберите картинку для иконки");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
    g_modal = true;
    BOOL ok = GetOpenFileNameW(&ofn);
    g_modal = false;
    if (!ok) return;

    void* test = nullptr;
    if (Gp.BitmapFromFile(file, &test) != 0 || !test) {
        Msg(Tr(L"Could not load this image.", L"Не удалось загрузить это изображение."));
        return;
    }
    Gp.DisposeImage(test);

    P->items[idx].iconPath = file;
    LoadImages(P->items[idx]);
    Commit();
}

static void PastePath() {
    std::wstring t = ClipboardText();
    if (t.empty()) {
        Msg(Tr(L"The clipboard does not contain a path or link.",
               L"В буфере обмена нет пути или ссылки."));
        return;
    }
    if (LooksLikeUrl(t)) {
        if (AddUrl(t)) Commit();
        return;
    }
    if (AddPath(t)) Commit();
}

static const CLSID kCLSID_ShellWindows = {
    0x9BA05972, 0xF6A8, 0x11CF, {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};
static const GUID kSID_STopLevelBrowser = {
    0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

static bool LaunchViaExplorer(const std::wstring& file, const std::wstring& args,
                              const std::wstring& dir, PCWSTR verb) {
    IShellWindows* psw = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                __uuidof(IShellWindows), (void**)&psw)) ||
        !psw) {
        return false;
    }

    VARIANT vtLoc, vtEmpty;
    VariantInit(&vtLoc);
    VariantInit(&vtEmpty);
    vtLoc.vt = VT_I4;
    vtLoc.lVal = CSIDL_DESKTOP;
    long hwnd = 0;
    IDispatch* pdisp = nullptr;
    HRESULT hr = psw->FindWindowSW(&vtLoc, &vtEmpty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH, &pdisp);
    psw->Release();
    if (hr != S_OK || !pdisp) return false;

    IServiceProvider* psp = nullptr;
    hr = pdisp->QueryInterface(__uuidof(IServiceProvider), (void**)&psp);
    pdisp->Release();
    if (FAILED(hr) || !psp) return false;

    IShellBrowser* psb = nullptr;
    hr = psp->QueryService(kSID_STopLevelBrowser, __uuidof(IShellBrowser), (void**)&psb);
    psp->Release();
    if (FAILED(hr) || !psb) return false;

    IShellView* psv = nullptr;
    hr = psb->QueryActiveShellView(&psv);
    psb->Release();
    if (FAILED(hr) || !psv) return false;

    IDispatch* pbg = nullptr;
    hr = psv->GetItemObject(SVGIO_BACKGROUND, __uuidof(IDispatch), (void**)&pbg);
    psv->Release();
    if (FAILED(hr) || !pbg) return false;

    IShellFolderViewDual* pfvd = nullptr;
    hr = pbg->QueryInterface(__uuidof(IShellFolderViewDual), (void**)&pfvd);
    pbg->Release();
    if (FAILED(hr) || !pfvd) return false;

    IDispatch* papp = nullptr;
    hr = pfvd->get_Application(&papp);
    pfvd->Release();
    if (FAILED(hr) || !papp) return false;

    IShellDispatch2* psd = nullptr;
    hr = papp->QueryInterface(__uuidof(IShellDispatch2), (void**)&psd);
    papp->Release();
    if (FAILED(hr) || !psd) return false;

    BSTR bFile = SysAllocString(file.c_str());
    VARIANT vArgs, vDir, vVerb, vShow;
    VariantInit(&vArgs);
    VariantInit(&vDir);
    VariantInit(&vVerb);
    VariantInit(&vShow);
    if (!args.empty()) {
        vArgs.vt = VT_BSTR;
        vArgs.bstrVal = SysAllocString(args.c_str());
    }
    if (!dir.empty()) {
        vDir.vt = VT_BSTR;
        vDir.bstrVal = SysAllocString(dir.c_str());
    }
    vVerb.vt = VT_BSTR;
    vVerb.bstrVal = SysAllocString(verb);
    vShow.vt = VT_I4;
    vShow.lVal = SW_SHOWNORMAL;

    hr = psd->ShellExecute(bFile, vArgs, vDir, vVerb, vShow);

    SysFreeString(bFile);
    VariantClear(&vArgs);
    VariantClear(&vDir);
    VariantClear(&vVerb);
    psd->Release();
    return SUCCEEDED(hr);
}

static void RunShell(const std::wstring& file, const std::wstring& args, const std::wstring& dir,
                     PCWSTR verb) {
    AllowSetForegroundWindow(ASFW_ANY);
    if (LaunchViaExplorer(file, args, dir, verb)) return;
    ShellExecuteW(P->hwnd, verb, file.c_str(), args.empty() ? nullptr : args.c_str(),
                  dir.empty() ? nullptr : dir.c_str(), SW_SHOWNORMAL);
}

static void Launch(const std::wstring& path, bool admin) {
    if (IsUrl(path)) {
        RunShell(path, L"", L"", L"open");
        return;
    }
    std::wstring dir;
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos) dir = path.substr(0, pos);
    RunShell(path, L"", dir, admin ? L"runas" : L"open");
}

static void ShowMenu(POINT sp, int idx) {
    HMENU m = CreatePopupMenu();
    bool isItem = idx < (int)P->items.size();
    bool isUrl = isItem && IsUrl(P->items[idx].path);
    if (isItem) {
        AppendMenuW(m, MF_STRING, ID_OPEN, Tr(L"Open", L"Открыть"));
        if (!isUrl) {
            AppendMenuW(m, MF_STRING, ID_ADMIN, Tr(L"Run as administrator", L"Запуск от имени администратора"));
            AppendMenuW(m, MF_STRING, ID_FOLDER, Tr(L"Show in folder", L"Показать в папке"));
        }
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, ID_ICON, Tr(L"Change icon...", L"Сменить иконку…"));
        AppendMenuW(m, MF_STRING | (P->items[idx].iconPath.empty() ? MF_GRAYED : 0), ID_ICON_RESET,
                    Tr(L"Reset icon", L"Сбросить иконку"));
        if (isUrl) {
            AppendMenuW(m, MF_STRING | (g_cfg.fetchFavicons ? 0 : MF_GRAYED), ID_FAV_REFRESH,
                        Tr(L"Refresh site icon", L"Обновить иконку сайта"));
        }
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING | (idx == 0 ? MF_GRAYED : 0), ID_UP, Tr(L"Move up", L"Переместить выше"));
        AppendMenuW(m, MF_STRING | (idx + 1 >= (int)P->items.size() ? MF_GRAYED : 0), ID_DOWN,
                    Tr(L"Move down", L"Переместить ниже"));
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        HMENU add = CreatePopupMenu();
        AppendMenuW(add, MF_STRING, ID_PICK, Tr(L"Choose a program...", L"Выбрать программу…"));
        AppendMenuW(add, MF_STRING, ID_URL, Tr(L"Add a website...", L"Добавить сайт…"));
        AppendMenuW(add, MF_STRING, ID_PASTE, Tr(L"Paste a path or link from the clipboard", L"Вставить путь или ссылку из буфера обмена"));
        AppendMenuW(m, MF_POPUP, (UINT_PTR)add, Tr(L"Add to the dock", L"Добавить на панель"));
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, ID_REMOVE, Tr(L"Remove from the dock", L"Удалить с панели"));
    } else {
        AppendMenuW(m, MF_STRING, ID_PICK, Tr(L"Choose a program...", L"Выбрать программу…"));
        AppendMenuW(m, MF_STRING, ID_URL, Tr(L"Add a website...", L"Добавить сайт…"));
        AppendMenuW(m, MF_STRING, ID_PASTE, Tr(L"Paste a path or link from the clipboard", L"Вставить путь или ссылку из буфера обмена"));
    }

    g_modal = true;
    SetForegroundWindow(P->hwnd);
    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON |
                                    (P->horiz ? (P->atMax ? TPM_BOTTOMALIGN : TPM_TOPALIGN) : (P->atMax ? TPM_RIGHTALIGN : TPM_LEFTALIGN)),
                             sp.x, sp.y, 0, P->hwnd, nullptr);
    PostMessageW(P->hwnd, WM_NULL, 0, 0);
    DestroyMenu(m);
    g_modal = false;
    P->outsideTicks = 0;

    switch (cmd) {
        case ID_OPEN:
            Launch(P->items[idx].path, false);
            break;
        case ID_ADMIN:
            Launch(P->items[idx].path, true);
            break;
        case ID_FOLDER: {
            std::wstring args = L"/select,\"" + P->items[idx].path + L"\"";
            RunShell(L"explorer.exe", args, L"", L"open");
            break;
        }
        case ID_ICON:
            ChangeIcon(idx);
            break;
        case ID_ICON_RESET:
            P->items[idx].iconPath.clear();
            LoadImages(P->items[idx]);
            Commit();
            break;
        case ID_REMOVE:
            FreeImg(P->items[idx]);
            P->items.erase(P->items.begin() + idx);
            P->hover = -1;
            Commit();
            break;
        case ID_UP:
            if (idx > 0) std::swap(P->items[idx], P->items[idx - 1]);
            Commit();
            break;
        case ID_DOWN:
            if (idx + 1 < (int)P->items.size()) std::swap(P->items[idx], P->items[idx + 1]);
            Commit();
            break;
        case ID_FAV_REFRESH: {
            Item& it = P->items[idx];
            std::wstring bare = BareHost(HostOf(it.path));
            FreeImg(it);
            for (const wchar_t* e : {L".img", L".none"}) {
                std::wstring f = FaviconFile(bare, e);
                if (!f.empty()) DeleteFileW(f.c_str());
            }
            EraseFromList(g_favFailed, bare);
            LoadImages(it);
            Commit();
            break;
        }
        case ID_URL:
            AddSite();
            break;
        case ID_PICK:
            PickAndAdd();
            break;
        case ID_PASTE:
            PastePath();
            break;
    }
}

static DWORD g_lastRunScan = 0;

static void RefreshRunning() {
    if (!g_cfg.showRunning) return;
    bool anyOpen = false;
    for (Panel* pn : g_panels) {
        if (pn->hwnd && pn->curW > StripW() + 1) anyOpen = true;
    }
    if (!anyOpen) return;
    DWORD now = GetTickCount();
    if (g_lastRunScan && now - g_lastRunScan < 1000) return;
    g_lastRunScan = now;

    std::vector<std::wstring> paths, names;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            wchar_t buf[MAX_PATH * 2];
            DWORD n = ARRAYSIZE(buf);
            bool got = false;
            HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
            if (h) {
                got = QueryFullProcessImageNameW(h, 0, buf, &n) != 0;
                CloseHandle(h);
            }
            if (got) {
                paths.push_back(LowerStr(buf));
            } else {
                names.push_back(LowerStr(pe.szExeFile));
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    std::sort(paths.begin(), paths.end());
    std::sort(names.begin(), names.end());

    for (Panel* pn : std::vector<Panel*>(g_panels)) {
        PanelScope sc(pn);
        bool changed = false;
        for (auto& it : P->items) {
            bool r = !it.exeKey.empty() &&
                     (std::binary_search(paths.begin(), paths.end(), it.exeKey) ||
                      std::binary_search(names.begin(), names.end(), it.exeName));
            if (r != it.running) {
                it.running = r;
                changed = true;
            }
        }
        if (changed) StartAnim();
    }
}

static void OnTick() {
    if (g_modal || !P->hwnd) return;

    bool hide = false;
    if (g_cfg.hideFs) {
        HWND fg = GetForegroundWindow();
        if (fg && fg != P->hwnd) {
            wchar_t cls[64] = {};
            GetClassNameW(fg, cls, ARRAYSIZE(cls));
            if (wcscmp(cls, L"Progman") && wcscmp(cls, L"WorkerW") &&
                wcscmp(cls, L"Shell_TrayWnd") && wcscmp(cls, L"Shell_SecondaryTrayWnd")) {
                HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
                if (mon == PrimaryMon()) {
                    RECT r;
                    GetWindowRect(fg, &r);
                    MONITORINFO mi{};
                    mi.cbSize = sizeof(mi);
                    GetMonitorInfoW(mon, &mi);
                    hide = r.left <= mi.rcMonitor.left && r.top <= mi.rcMonitor.top &&
                           r.right >= mi.rcMonitor.right && r.bottom >= mi.rcMonitor.bottom;
                }
            }
        }
    }

    bool vis = IsWindowVisible(P->hwnd);
    if (hide && vis) ShowWindow(P->hwnd, SW_HIDE);
    if (!hide && !vis) {
        P->intro = g_cfg.anim ? 0.0f : 1.0f;
        Render();
        ShowWindow(P->hwnd, SW_SHOWNOACTIVATE);
        StartAnim();
    }
    if (hide) return;

    if (!g_cfg.autoHide) return;

    POINT pt;
    GetCursorPos(&pt);
    RECT wr;
    GetWindowRect(P->hwnd, &wr);
    if (g_cfg.style == 5 || g_cfg.style == 6) {
        RECT wa = WorkArea();
        int zl = (int)(P->curH + 0.5), zw = Sc(10);
        RECT edge;
        if (!P->horiz) {
            int zy = wa.top + ((wa.bottom - wa.top) - zl) / 2;
            edge = P->atMax ? RECT{wa.right - zw, zy, wa.right, zy + zl}
                            : RECT{wa.left, zy, wa.left + zw, zy + zl};
        } else {
            int zx = wa.left + ((wa.right - wa.left) - zl) / 2;
            edge = P->atMax ? RECT{zx, wa.bottom - zw, zx + zl, wa.bottom}
                            : RECT{zx, wa.top, zx + zl, wa.top + zw};
        }
        UnionRect(&wr, &wr, &edge);
    }
    if (PtInRect(&wr, pt) || P->pressIdx >= 0 || g_dragActive) {
        P->outsideTicks = 0;
        if (!P->expanded) {
            P->expanded = true;
            Relayout();
            RefreshRunning();
        }
    } else if (P->expanded && ++P->outsideTicks >= 6) {
        P->expanded = false;
        P->hover = -1;
        Relayout();
    }
}

static LRESULT PanelProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            P->hwnd = hwnd;
            P->tip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                     WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, CW_USEDEFAULT,
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, nullptr,
                                     GetModuleHandleW(nullptr), nullptr);
            SetTimer(hwnd, kTickId, 100, nullptr);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_LBUTTONDOWN: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            int hit = HitTest(pt);
            if (hit >= 0 && hit < (int)P->items.size()) {
                P->pressIdx = hit;
                P->pressPt = pt;
                SetCapture(hwnd);
                StartAnim();
            }
            return 0;
        }
        case WM_CAPTURECHANGED: {
            if ((HWND)lp != hwnd) {
                if (g_dragActive) DragEnd();
                P->pressIdx = -1;
                StartAnim();
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            if (P->pressIdx >= 0 && (wp & MK_LBUTTON)) {
                if (!g_dragActive && P->pressIdx < (int)P->items.size()) {
                    int dx = pt.x - P->pressPt.x, dy = pt.y - P->pressPt.y;
                    int th = Sc(6);
                    if (dx * dx + dy * dy >= th * th) DragStart();
                }
                if (g_dragActive) {
                    POINT sp = pt;
                    ClientToScreen(hwnd, &sp);
                    DragUpdate(sp);
                    return 0;
                }
            }
            int hit = HitTest(pt);
            if (g_cfg.magnify) {
                float along = P->horiz ? (float)pt.x : (float)pt.y;
                if (P->magAmt < 0.05f) P->magY = along;
                P->mouseY = along;
                StartAnim();
            }
            if (hit != P->hover) {
                P->hover = hit;
                StartAnim();
            }
            if (!P->tracking) {
                TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&t);
                P->tracking = true;
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            P->tracking = false;
            if (g_dragActive) return 0;
            P->hover = -1;
            P->mouseY = -1.0f;
            StartAnim();
            return 0;
        case WM_LBUTTONUP: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            if (P->pressIdx >= 0) {
                bool wasDrag = g_dragActive;
                int idx = P->pressIdx;
                P->pressIdx = -1;
                StartAnim();
                if (wasDrag) DragEnd();
                if (GetCapture() == hwnd) ReleaseCapture();
                if (!wasDrag && idx < (int)P->items.size() && HitTest(pt) == idx) {
                    Launch(P->items[idx].path, false);
                }
                return 0;
            }
            int hit = HitTest(pt);
            if (hit >= 0) {
                if (hit < (int)P->items.size())
                    Launch(P->items[hit].path, false);
                else
                    PickAndAdd();
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            int hit = HitTest(pt);
            if (hit < 0 && !PlusVisible() && Expanded()) hit = (int)P->items.size();
            if (hit >= 0) {
                ClientToScreen(hwnd, &pt);
                ShowMenu(pt, hit);
            }
            return 0;
        }
        case WM_DROPFILES: {
            HDROP hd = (HDROP)wp;
            UINT n = DragQueryFileW(hd, 0xFFFFFFFF, nullptr, 0);
            bool changed = false;
            for (UINT i = 0; i < n; i++) {
                wchar_t buf[MAX_PATH * 2];
                if (DragQueryFileW(hd, i, buf, ARRAYSIZE(buf))) changed |= AddPath(buf);
            }
            DragFinish(hd);
            if (changed) Commit();
            return 0;
        }
        case WM_TIMER:
            if (wp == kTickId) OnTick();
            return 0;
        case 0x0320:
            if (g_cfg.followTheme) {
                HWND c = g_ctrl.load();
                if (c) PostMessageW(c, WM_RELOAD, 0, 0);
            }
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            if (msg == WM_SETTINGCHANGE && g_cfg.followTheme && lp &&
                wcscmp((LPCWSTR)lp, L"ImmersiveColorSet") == 0) {
                HWND c = g_ctrl.load();
                if (c) PostMessageW(c, WM_RELOAD, 0, 0);
            }
            Relayout();
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kTickId);
            P->animRunning = false;
            P->tip = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto* cs = (CREATESTRUCTW*)lp;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
    }
    Panel* me = (Panel*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!me) return DefWindowProcW(hwnd, msg, wp, lp);
    PanelScope scope(me);
    return PanelProc(hwnd, msg, wp, lp);
}

static Panel* CreatePanel(int edge) {
    Panel* pn = new Panel();
    pn->edge = edge;
    pn->horiz = edge >= 2;
    pn->atMax = edge == 0 || edge == 3;

    PanelScope scope(pn);
    LoadItems();
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_ACCEPTFILES, kClass,
        L"QuickLaunchDock", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, GetModuleHandleW(nullptr), pn);
    if (!hwnd) {
        for (auto& it : pn->items) FreeImg(it);
        delete pn;
        return nullptr;
    }
    pn->hwnd = hwnd;
    g_panels.push_back(pn);
    Relayout();
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    return pn;
}

static void DestroyPanel(Panel* pn) {
    if (g_dragOwner == pn) {
        g_dragActive = false;
        g_dragOwner = nullptr;
        g_dragIdx = -1;
    }
    {
        PanelScope scope(pn);
        if (pn->hwnd) DestroyWindow(pn->hwnd);
        pn->hwnd = nullptr;
        for (auto& it : pn->items) FreeImg(it);
        pn->items.clear();
        FreeDib();
    }
    g_panels.erase(std::remove(g_panels.begin(), g_panels.end(), pn), g_panels.end());
    delete pn;
}

static void SyncPanels() {
    for (int edge = 0; edge < 4; edge++) {
        Panel* existing = nullptr;
        for (Panel* pn : g_panels) {
            if (pn->edge == edge) existing = pn;
        }
        if (g_cfg.panels[edge] && !existing) {
            CreatePanel(edge);
        } else if (!g_cfg.panels[edge] && existing) {
            DestroyPanel(existing);
        }
    }
}

static void ReloadPanels() {
    for (Panel* pn : std::vector<Panel*>(g_panels)) {
        PanelScope scope(pn);
        P->hover = -1;
        ReloadIcons();
        P->expanded = true;
        FreeDib();
        Relayout();
    }
}

static void FavDone(const std::wstring& bare, bool ok) {
    {
        std::lock_guard<std::mutex> lk(g_favMutex);
        EraseFromList(g_favBusy, bare);
    }
    if (!ok && !InList(g_favFailed, bare)) g_favFailed.push_back(bare);
    for (Panel* pn : std::vector<Panel*>(g_panels)) {
        PanelScope scope(pn);
        bool changed = false;
        for (auto& it : P->items) {
            if (!IsUrl(it.path) || !it.iconPath.empty()) continue;
            if (_wcsicmp(BareHost(HostOf(it.path)).c_str(), bare.c_str()) != 0) continue;
            LoadImages(it);
            changed = true;
        }
        if (changed) StartAnim();
    }
}

constexpr wchar_t kCtlClass[] = L"WhQuickLaunchDockCtl";
constexpr UINT_PTR kCtlRetryTimer = 1;
constexpr UINT_PTR kCtlRunTimer = 2;

static LRESULT CALLBACK CtrlProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_FAVICON) {
        auto* host = (std::wstring*)lp;
        if (host) {
            FavDone(*host, wp != 0);
            delete host;
        }
        return 0;
    }
    if (msg == WM_RELOAD) {
        if (g_modal) {
            SetTimer(hwnd, kCtlRetryTimer, 300, nullptr);
            return 0;
        }
        LoadSettings();
        SyncPanels();
        ReloadPanels();
        return 0;
    }
    if (msg == WM_TIMER && wp == kCtlRunTimer) {
        RefreshRunning();
        return 0;
    }
    if (msg == WM_TIMER && wp == kCtlRetryTimer) {
        KillTimer(hwnd, kCtlRetryTimer);
        PostMessageW(hwnd, WM_RELOAD, 0, 0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static HANDLE g_frameTimer = nullptr;
static LONGLONG g_qpcFreq = 0;
static LONGLONG g_nextFrame = 0;

static void PaceFrame(int fps) {
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    LONGLONG period = g_qpcFreq / fps;
    if (g_nextFrame == 0 || t.QuadPart - g_nextFrame > period * 4) g_nextFrame = t.QuadPart;
    g_nextFrame += period;
    for (;;) {
        QueryPerformanceCounter(&t);
        LONGLONG left = g_nextFrame - t.QuadPart;
        if (left <= 0) break;
        double ms = (double)left * 1000.0 / (double)g_qpcFreq;
        if (g_frameTimer && ms > 0.5) {
            LARGE_INTEGER due;
            due.QuadPart = -(LONGLONG)(ms * 10000.0);
            if (SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr, FALSE)) {
                WaitForSingleObject(g_frameTimer, 50);
                continue;
            }
        }
        if (ms > 3.0) {
            Sleep(1);
        } else {
            YieldProcessor();
        }
    }
}

static DWORD WINAPI UiThread(LPVOID) {
    MSG queueInit;
    PeekMessageW(&queueInit, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    if (g_stop) return 0;

    using SetDpiCtx_t = HANDLE(WINAPI*)(HANDLE);
    auto setDpiCtx = reinterpret_cast<SetDpiCtx_t>(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"));
    if (setDpiCtx) setDpiCtx((HANDLE)(INT_PTR)-4);

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (!GpInit()) {
        Wh_Log(L"Quick Launch Dock: GDI+ init failed");
        CoUninitialize();
        return 0;
    }

    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);

    HDC dc = GetDC(nullptr);
    g_dpi = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(nullptr, dc);

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClass;
    RegisterClassW(&wc);

    WNDCLASSW cc{};
    cc.lpfnWndProc = CtrlProc;
    cc.hInstance = hInst;
    cc.lpszClassName = kCtlClass;
    RegisterClassW(&cc);

    LoadSettings();
    InitUserStorage();
    g_ctrl = CreateWindowExW(0, kCtlClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);
    SyncPanels();
    SetTimer(g_ctrl, kCtlRunTimer, 3000, nullptr);
    RefreshRunning();

    LARGE_INTEGER qf;
    QueryPerformanceFrequency(&qf);
    g_qpcFreq = qf.QuadPart;
    g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0x00000002, TIMER_ALL_ACCESS);

    MSG msg;
    bool quit = false;
    while (!quit) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) break;

        bool any = false;
        for (Panel* pn : g_panels) {
            if (pn->animRunning) any = true;
        }
        if (any) {
            if (g_cfg.fps > 0) {
                PaceFrame(g_cfg.fps);
            } else if (FAILED(DwmFlush())) {
                Sleep(8);
            }
            for (Panel* pn : std::vector<Panel*>(g_panels)) {
                if (!pn->animRunning) continue;
                PanelScope scope(pn);
                AnimStep();
            }
        } else {
            g_nextFrame = 0;
            WaitMessage();
        }
    }
    if (g_frameTimer) {
        CloseHandle(g_frameTimer);
        g_frameTimer = nullptr;
    }

    for (Panel* pn : std::vector<Panel*>(g_panels)) DestroyPanel(pn);
    HWND ctrl = g_ctrl.exchange(nullptr);
    if (ctrl) DestroyWindow(ctrl);
    UnregisterClassW(kClass, hInst);
    UnregisterClassW(kCtlClass, hInst);
    if (Gp.ok && Gp.Shutdown) Gp.Shutdown(Gp.token);
    Gp.ok = false;
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_stop = false;
    g_favStop = false;
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    HWND h = g_ctrl.load();
    if (h) PostMessageW(h, WM_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    StopFavicons();
    g_stop = true;
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
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
