// ==WindhawkMod==
// @id              classic-webview-pane
// @name            Classic WebView Pane
// @description     Brings back the Windows 2000 WebView - the pane left of the file list with the icon and name of the folder or the selected item, a divider line, a description and See also links
// @name:ru         Панель WebView как в Windows 2000
// @description:ru  Возвращает панель WebView из Windows 2000 - панель слева от списка файлов со значком и именем папки или выбранного объекта, линией-разделителем, описанием и ссылками «Перейти к»
// @version         2.16
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -lole32 -lshlwapi -luuid -lwindowscodecs
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- paneWidth: 200
  $name: Pane width
  $name:ru: Ширина панели
  $description: Width of the pane in pixels, at 100% scaling. Windows 2000 used about 200.
  $description:ru: >-
    Ширина панели в пикселях при масштабе 100%. В Windows 2000 она была около 200.
- minListWidth: 200
  $name: Room kept for the file list
  $name:ru: Место для списка файлов
  $description: >-
    When the window gets so narrow that the file list would be left with less
    than this many pixels (at 100% scaling), the pane gives way and the list
    gets its room, as in Windows 2000. 0 keeps the pane at any width.
  $description:ru: >-
    Когда окно становится настолько узким, что списку файлов осталось бы
    меньше этого числа пикселей (при масштабе 100%), панель уступает место
    списку, как в Windows 2000. 0 - панель остаётся при любой ширине.
- position: left
  $name: Position
  $name:ru: Расположение
  $description: >-
    Which side of the file list the pane is put on. Applies to folders opened
    after the change.
  $description:ru: >-
    С какой стороны от списка файлов размещается панель. Действует для папок,
    открытых после изменения.
  $options:
  - left: Left of the file list
  - right: Right of the file list
  $options:ru:
  - left: Слева от списка файлов
  - right: Справа от списка файлов
- showHeader: true
  $name: Icon and name at the top
  $name:ru: Значок и имя сверху
  $description: >-
    Show the icon and the name of the selected item at the top of the pane, or
    of the folder itself when nothing is selected.
  $description:ru: >-
    Показывать сверху значок и имя выбранного объекта, а когда ничего не
    выделено - самой папки.
- iconSize: 32
  $name: Icon size
  $name:ru: Размер значка
  $description: Size of the icon in the header, in pixels at 100% scaling.
  $description:ru: Размер значка в шапке, в пикселях при масштабе 100%.
- showItemType: true
  $name: Type and size under the name
  $name:ru: Тип и размер под именем
  $description: >-
    Add the kind of the item, and its size when it has one, under its name -
    the line Windows 2000 put there for a selected file.
  $description:ru: >-
    Добавлять под именем тип объекта и, если он есть, размер - строку, которую
    Windows 2000 показывала для выбранного файла.
- showDriveSpace: true
  $name: Drive space chart
  $name:ru: Диаграмма занятого места
  $description: >-
    For a drive, show the used/free legend, capacity and a three-dimensional
    pie chart under the name, the way the Windows 2000 WebView did.
  $description:ru: >-
    Показывать для диска ёмкость, легенду «Занято/Свободно» и объёмную круговую
    диаграмму под именем, как это делала панель WebView в Windows 2000.
- usedLabel: 'Used:'
  $name: Caption of the used space
  $name:ru: Подпись занятого места
  $description: The default text is shown in Russian on a Russian Windows.
  $description:ru: Текст по умолчанию в русской Windows показывается по-русски.
- freeLabel: 'Free:'
  $name: Caption of the free space
  $name:ru: Подпись свободного места
  $description: The default text is shown in Russian on a Russian Windows.
  $description:ru: Текст по умолчанию в русской Windows показывается по-русски.
- totalLabel: 'Capacity:'
  $name: Caption of the capacity
  $name:ru: Подпись ёмкости
  $description: The default text is shown in Russian on a Russian Windows.
  $description:ru: Текст по умолчанию в русской Windows показывается по-русски.
- barColorSource: theme
  $name: Bar colour source
  $name:ru: Источник цвета индикатора
  $description: >-
    Use the selection colour supplied by the current Windows theme, or the
    custom colour below.
  $description:ru: >-
    Использовать цвет выделения, установленный текущей темой Windows, либо
    заданный ниже собственный цвет.
  $options:
  - theme: Windows theme
  - custom: Custom colour
  $options:ru:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorBar: '#000080'
  $name: Custom colour of the chart
  $name:ru: Собственный цвет диаграммы
  $description: Used only when "Custom colour" is selected above.
  $description:ru: Используется только при выборе «Собственный цвет» выше.
- descriptionText: Select an item to view its description.
  $name: Text under the divider
  $name:ru: Текст под разделителем
  $description: >-
    The line Windows 2000 showed while nothing was selected. The default text is
    shown in the wording of the Russian Windows 2000 on a Russian Windows.
    Leave empty to omit it.
  $description:ru: >-
    Строка, которую Windows 2000 показывала, пока ничего не выделено. Текст по
    умолчанию в русской Windows показывается в формулировке русской Windows
    2000. Оставьте пустым, чтобы убрать её.
- showFolderDescription: true
  $name: Description of a system folder
  $name:ru: Описание системной папки
  $description: >-
    Show a Windows 2000-style information box whose text is chosen for the
    current system folder, such as Documents, This PC or Recycle Bin.
  $description:ru: >-
    Показывать информационное поле в стиле Windows 2000 с текстом, подходящим
    текущей системной папке: «Документы», «Мой компьютер», «Корзина» и другим.
- showSeeAlso: true
  $name: See also links
  $name:ru: Ссылки «Перейти к»
  $description: Show the list of links at the bottom of the pane.
  $description:ru: Показывать список ссылок внизу панели.
- seeAlsoTitle: 'See also:'
  $name: See also caption
  $name:ru: Заголовок списка ссылок
  $description: >-
    The caption over the links. The default text is shown in the wording of the
    Russian Windows 2000 on a Russian Windows.
  $description:ru: >-
    Заголовок над ссылками. Текст по умолчанию в русской Windows показывается в
    формулировке русской Windows 2000 - «Перейти к:».
- seeAlso:
  - - label: My Documents
      $name: Text
      $name:ru: Текст
      $description: >-
        The text of the link. The default names are shown in Russian on a
        Russian Windows.
      $description:ru: >-
        Текст ссылки. Имена по умолчанию в русской Windows показываются
        по-русски.
    - target: shell:Personal
      $name: Target
      $name:ru: Куда ведёт
      $description: >-
        Where the link goes. Anything Explorer can navigate to - a path, a
        shell: folder name or a shell:::{CLSID}.
      $description:ru: >-
        Куда ведёт ссылка. Всё, куда умеет переходить проводник - путь, имя
        папки вида shell: или shell:::{CLSID}.
  - - label: My Network Places
    - target: shell:NetworkPlacesFolder
  - - label: My Computer
    - target: shell:MyComputerFolder
  $name: Links
  $name:ru: Ссылки
  $description: The links under the See also caption.
  $description:ru: Ссылки под заголовком «Перейти к».
- underlineLinks: true
  $name: Underline the links
  $name:ru: Подчёркивать ссылки
  $description: >-
    Keep the links underlined the way the web view of Windows 2000 did, rather
    than only under the pointer.
  $description:ru: >-
    Держать ссылки подчёркнутыми, как в веб-виде Windows 2000, а не только под
    курсором.
- picture: win2000
  $name: Picture
  $name:ru: Картинка
  $description: >-
    The picture in the corner of the pane. Both pictures of the Windows 2000
    web view - the clouds and the coloured squares - come built into the mod.
    A file of your own can be used instead, or no picture at all.
  $description:ru: >-
    Картинка в углу панели. Обе картинки веб-вида Windows 2000 - облака и
    цветные квадраты - встроены в мод. Вместо них можно взять свой файл или
    обойтись без картинки.
  $options:
  - win2000: Windows 2000 clouds
  - squares: Windows 2000 coloured squares
  - file: The picture file below
  - none: No picture
  $options:ru:
  - win2000: Облака Windows 2000
  - squares: Цветные квадраты Windows 2000
  - file: Файл картинки ниже
  - none: Без картинки
- imagePath: ''
  $name: Picture file
  $name:ru: Файл картинки
  $description: >-
    Used when the picture is set to a file. BMP, GIF, PNG, JPEG and ICO all
    work, and the path may contain environment variables.
  $description:ru: >-
    Используется, если для картинки выбран файл. Подходят BMP, GIF, PNG, JPEG и
    ICO, в пути можно использовать переменные среды.
- imagePosition: background
  $name: Where the picture goes
  $name:ru: Где картинка
  $description: >-
    Windows 2000 had it in the top left corner of the pane, with the icon and
    the name drawn over it.
  $description:ru: >-
    В Windows 2000 она была в левом верхнем углу панели, а значок и имя
    рисовались поверх неё.
  $options:
  - background: In the top left corner, behind the icon and the name
  - top: Above the icon and the name
  - bottom: At the bottom of the pane
  $options:ru:
  - background: В левом верхнем углу, под значком и именем
  - top: Над значком и именем
  - bottom: Внизу панели
- imageWidth: 0
  $name: Picture width
  $name:ru: Ширина картинки
  $description: >-
    Width in pixels at 100% scaling. 0 stretches the picture across the pane
    instead, the way the one of Windows 2000 spans the top of its web view.
    Either way the height follows the proportions of the picture.
  $description:ru: >-
    Ширина в пикселях при масштабе 100%. 0 - растянуть картинку на всю ширину
    панели, как картинка Windows 2000 растянута по верху её веб-вида. И так и
    так высота берётся по пропорциям картинки.
- imageSmooth: false
  $name: Smooth the picture when it is scaled
  $name:ru: Сглаживать картинку при масштабировании
  $description: >-
    Off keeps the pixels as they are, which is what a picture out of Windows
    2000 wants. On smooths them, which suits a photograph.
  $description:ru: >-
    Выключено - пиксели остаются как есть, что и нужно картинке из Windows 2000.
    Включено - сглаживать, что подходит фотографии.
- imageBlendWhite: true
  $name: Let the picture's white background through
  $name:ru: Растворять белый фон картинки
  $description: >-
    Pictures like the Windows 2000 one are drawn on white, which shows as a
    white block on a pane that is not white. With this on, a picture without an
    alpha channel of its own is multiplied into the background instead: white
    leaves it untouched, everything else tints it. Pictures that do carry alpha
    are drawn by it either way.
  $description:ru: >-
    Картинки вроде той, что была в Windows 2000, нарисованы на белом, и на
    небелой панели белое лезет прямоугольником. Если включено, картинка без
    своего альфа-канала умножается на фон: белое фон не трогает, остальное его
    подкрашивает. Картинки с альфа-каналом рисуются по нему в любом случае.
- titleFontSize: 12
  $name: Size of the name
  $name:ru: Размер имени
  $description: >-
    Size of the folder or item name in points, at 100% scaling. Windows 2000
    set it well above the rest of the pane. 0 keeps the size of the other text.
  $description:ru: >-
    Размер имени папки или объекта в пунктах при масштабе 100%. В Windows 2000
    оно заметно крупнее остального текста в панели. 0 - как остальной текст.
- paneBorder: true
  $name: Take the pane inside the border of the file list
  $name:ru: Панель внутри рамки списка файлов
  $description: >-
    Windows 2000 had one sunken frame around the pane and the file list
    together, starting at the splitter of the folder tree. The pane draws the
    left, top and bottom of that frame and covers the left edge of the frame the
    list draws, so that the two read as one and no line is left between them.
  $description:ru: >-
    В Windows 2000 панель и список файлов были в одной утопленной рамке,
    начинавшейся от разделителя дерева папок. Панель дорисовывает эту рамку
    слева, сверху и снизу и закрывает левую грань рамки списка, так что они
    читаются как одна и линии между ними не остаётся.
- removeViewBorder: false
  $name: No sunken border on the file list
  $name:ru: Убрать рамку у списка файлов
  $description: >-
    Off, the file list keeps the sunken border Explorer gives it. On, the border
    is taken off - but only in the windows the pane is in, not in Explorer as a
    whole - and it is put back the moment the mod is turned off.
  $description:ru: >-
    Выключено - список файлов сохраняет утопленную рамку, которую ему рисует
    проводник. Включено - рамка убирается, но только в окнах с панелью, а не во
    всём проводнике, и возвращается, как только мод выключен.
- hideDetailsPane: false
  $name: Hide the details pane
  $name:ru: Скрыть панель сведений
  $description: >-
    Take the Windows details pane on the right out of the layout, so the new
    pane is the only one. Windows 2000 had no details pane. Applies to
    folders opened after the change.
  $description:ru: >-
    Убрать из раскладки панель сведений Windows справа, чтобы новая панель
    осталась единственной. В Windows 2000 панели сведений не было. Действует
    для папок, открытых после изменения.
- skipControlPanel: true
  $name: Leave the pages of Control Panel items alone
  $name:ru: Не показывать на страницах элементов панели управления
  $description: >-
    The page of a single Control Panel item - Programs and Features, Appearance
    and Personalization - draws a column of task links of its own in the very
    place the pane goes, and neither knows about the other, so the two end up
    drawn on top of each other. With this on, the pane stays off those pages.
    The Control Panel folder itself has no such column and keeps the pane.
  $description:ru: >-
    Страница отдельного элемента панели управления - «Программы и компоненты»,
    «Оформление и персонализация» - рисует на месте панели собственную колонку
    ссылок, и они накладываются друг на друга. Если включено, на таких
    страницах панель не появляется. В самой папке панели управления такой
    колонки нет, и панель там остаётся.
- backgroundColorSource: theme
  $name: Background colour source
  $name:ru: Источник цвета фона
  $options:
  - theme: Windows theme
  - custom: Custom colour
  $options:ru:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorBackground: window
  $name: Custom background colour
  $name:ru: Собственный цвет фона
  $description: >-
    A system colour name - window, buttonface, infobackground, appworkspace and
    the rest - or #RRGGBB.
  $description:ru: >-
    Имя системного цвета - window, buttonface, infobackground, appworkspace и
    так далее - либо #RRGGBB.
- titleColorSource: theme
  $name: Name colour source
  $name:ru: Источник цвета имени
  $options:
  - theme: Windows theme
  - custom: Custom colour
  $options:ru:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorTitle: windowtext
  $name: Custom name colour
  $name:ru: Собственный цвет имени
- textColorSource: theme
  $name: Text colour source
  $name:ru: Источник цвета текста
  $options:
  - theme: Windows theme
  - custom: Custom colour
  $options:ru:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorText: windowtext
  $name: Custom text colour
  $name:ru: Собственный цвет текста
- linkColorSource: custom
  $name: Link colour source
  $name:ru: Источник цвета ссылок
  $options:
  - theme: Windows theme
  - custom: Custom colour
  $options:ru:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorLink: '#0000FF'
  $name: Custom link colour
  $name:ru: Собственный цвет ссылок
  $description: >-
    The blue of a link, as the web view had it. It is a colour of its own rather
    than the system hotlight colour, which classic colour schemes are free to
    set to anything and often do.
  $description:ru: >-
    Синий цвет ссылки, как в веб-виде. Задан отдельным цветом, а не системным
    hotlight: классические схемы вольны ставить туда что угодно и часто ставят.
- dividerColorSource: custom
  $name: Divider colour source
  $name:ru: Источник цвета разделителя
  $options:
  - theme: Windows theme
  - custom: Custom colour
  $options:ru:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorDivider: '#0000FF'
  $name: Custom divider colour
  $name:ru: Собственный цвет разделителя
  $description: Only used when the divider is drawn as a line.
  $description:ru: Используется, только если разделитель рисуется линией.
- divider: win2000
  $name: Divider
  $name:ru: Разделитель
  $description: >-
    The line under the name. Windows 2000 drew it from a picture, a bar in the
    four colours of the Windows logo, which comes built into the mod.
  $description:ru: >-
    Линия под именем. Windows 2000 рисовала её из картинки - полоски в четырёх
    цветах логотипа Windows; она встроена в мод.
  $options:
  - win2000: Windows 2000 colour bar
  - file: The divider picture file below
  - drawn: A line in the divider colour
  $options:ru:
  - win2000: Цветная полоска Windows 2000
  - file: Файл картинки разделителя ниже
  - drawn: Линия цветом разделителя
- dividerImagePath: ''
  $name: Divider picture file
  $name:ru: Файл картинки разделителя
  $description: >-
    Used when the divider is set to a file. It is stretched across the pane and
    keeps its own height.
  $description:ru: >-
    Используется, если для разделителя выбран файл. Картинка растягивается на
    всю ширину панели и сохраняет свою высоту.
- dividerGradient: true
  $name: Fade the divider out
  $name:ru: Разделитель с переходом в фон
  $description: >-
    The line under the name of Windows 2000 was a gradient that started in
    colour on the left and faded into the background on the right, not a line
    of one colour all the way across.
  $description:ru: >-
    Линия под именем в Windows 2000 была градиентом: слева цветная, справа
    уходила в фон, а не сплошной одноцветной через всю панель.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Classic WebView Pane

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

Windows 2000 showed a pane on the left of the file list - the *WebView* - with
the icon and the name of whatever was selected, a divider line under it, a line
of text and a short list of *See also* links. Windows Vista dropped it, and
Windows 11 has no trace of it left.

This mod puts it back. The icon and the name follow the selection, and fall
back to the folder itself when nothing is selected; the links navigate the same
window the way they used to. On top of that, the pane brings back two things
Windows 2000 showed there:

* authentic descriptions of the system folders - This PC, Documents, Recycle
  Bin and the others;
* for a drive, its capacity, used and free space and the three-dimensional pie
  chart of how full it is.

![This PC, with the description of the folder](https://raw.githubusercontent.com/appEW/images/main/classic-webview-pane/my-computer.png)

![A drive, with its capacity and how full it is](https://raw.githubusercontent.com/appEW/images/main/classic-webview-pane/drive.png)

![The Recycle Bin](https://raw.githubusercontent.com/appEW/images/main/classic-webview-pane/recycle-bin.png)

## How it works

The content area of an Explorer window is not a set of child windows, it is a
DirectUI tree. `shell32` keeps its layout in `UIFILE` resources - one per folder
type - and hands the whole thing to `DirectUI::DUIXmlParser::SetXML` in
`dui70.dll` as one XML document, which is the single place where the layout of
every Explorer window can be seen and changed.

The mod hooks that function and inserts one element next to the one that hosts
the file list:

```
<Element layoutpos="Client" layout="shellborderlayout()">
  <BannerContainer .../>
  <Element id="atom(ClassicWebViewPane)" layoutpos="left" .../>   <-- inserted
  <Element id="atom(ViewHostContainer)" ...>
  <DetailsContainer layoutpos="right"/>
```

That element is only a spacer. Everything inside the pane - the icon, the name,
the divider, the text and the links - is drawn by the mod into a child window
of its own, which it keeps over the gap the spacer opens up between the folder
tree and the file list.

It is done that way because the parts of the shell the pane would otherwise be
built out of are gone. The Windows 2000 recreations for Windows 10 fill their
header with `PreviewThumbnail` and `PreviewMetadata` elements bound to
`PreviewThumbnailModule` and `PreviewMetadataModule`. On Windows 11 24H2 those
module names do not appear in `shell32` at all any more: the element classes
still exist and still take up space, but nothing ever fills them, so a header
built that way comes out empty. Drawing it directly also means the pane needs no
patched `shellstyle.dll` and looks right on the Classic theme, where the theme
colour functions the shell's own style sheets use return nothing.

The contents come from the shell view itself: the mod asks the
`SHELLDLL_DefView` window of the folder for its `IShellBrowser`, and through it
for the current folder and the selection, refreshing whenever the list reports
that the selection changed.

## Notes

- In a narrow window the pane gives way first, as the WebView did in Windows
  2000: once the file list would be left with less than *Room kept for the file
  list*, the mod collapses the DirectUI spacer to nothing (through the element
  itself - `HWNDElement` is in the first pointer of the `DirectUIHWND`'s extra
  bytes, the spacer is found under it by its id) and hides the pane, and gives
  both back when the window is wide enough again. The pane also follows the
  folder tree and the file list whenever DirectUI moves, resizes or hides
  either, and keeps clear of the splitter next to the tree.

- The pictures of the Windows 2000 web view - the clouds or the coloured
  squares in the corner, and the bar in the colours of the Windows logo under
  the name - are built into the mod, and the clouds and the bar are shown by
  default, so the pane looks right straight after installing. Both can be
  switched off or come from a file of your own instead: BMP, GIF, PNG, JPEG and
  ICO are all read - `LoadImage` takes the BMPs and icons, the shell takes the
  rest. The picture is stretched across the pane at its own proportions with the
  pixels left alone by default, and it can also be given a width of its own, put
  above the name or at the bottom instead, and smoothed when scaled.
- Two things in DirectUI markup take the whole window down with them - the file
  list, the folder tree and all - while `SetXML` still reports success: an
  attribute the element class does not know, and a value the parser cannot
  read. Colours from the settings are checked against the names DirectUI
  actually accepts for that reason.
- The pane sits between the folder tree and the file list. In Windows 2000 the
  tree and the WebView replaced each other; here they can both be open.
- If you also inject a WebView pane through patched `shell32` resources - the
  WinClassic recreation with Resource Redirect, say - turn that off, or you get
  two panes.

- The texts of the pane follow the Windows display language. The defaults are
  the wording of the English Windows 2000 - *Select an item to view its
  description.*, *See also:*, *My Documents*, *Capacity:* and so on - and on a
  Russian Windows they are shown in the wording of the Russian Windows 2000
  instead. A text you change yourself is shown exactly as you typed it. The
  short description of a system folder (Documents, This PC, Recycle Bin and the
  others) is written for this mod and is shown in English or Russian the same
  way.

---

## По-русски

Windows 2000 показывала слева от списка файлов панель WebView: значок и имя
выбранного объекта (или самой папки, если ничего не выделено), линию-разделитель
под ними, строку описания и короткий список ссылок «Перейти к». В Windows Vista
её убрали, в Windows 11 от неё не осталось ничего.

Мод возвращает эту панель. Значок и имя следуют за выделением, а ссылки
открывают папку в том же окне. Кроме того, панель возвращает то, что Windows
2000 показывала в ней:

* аутентичные описания системных папок - «Мой компьютер», «Документы»,
  «Корзина» и других;
* для диска - ёмкость, занятое и свободное место и объёмную круговую диаграмму
  его заполненности.

В узком окне панель, как и в Windows 2000, первой уступает место списку файлов.
Скриншоты - выше, в английской части.

Картинки веб-вида Windows 2000 - облака или цветные квадраты в углу и полоска
в цветах логотипа Windows под именем - встроены в мод; облака и полоска
включены по умолчанию, так что панель выглядит как надо сразу после установки.
Вместо них можно указать свои файлы или выключить их.

Внешний вид настраивается: ширина и сторона панели, размер значка, картинки,
цвета, подписи и ссылки. Тексты по умолчанию берутся в формулировке
Windows 2000 на языке интерфейса Windows: в русской Windows - «Выберите объект
для просмотра его описания.», «Перейти к:», «Мои документы» и так далее.
Изменённый вами текст показывается как есть.

Если вы уже добавляете панель WebView через изменённые ресурсы `shell32`
(например, WinClassic с Resource Redirect), отключите это, иначе панелей будет
две.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>

#include <commctrl.h>
#include <exdisp.h>
#include <objbase.h>
#include <servprov.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>

#include <mutex>
#include <string>
#include <vector>

// The message SHELLDLL_DefView answers with the IShellBrowser of its folder.
#define WM_GETISHELLBROWSER (WM_USER + 7)

// Set first thing on unload. From then on no pane, spacer or subclass is added:
// closing the panes resizes their views, and a view that is resized would
// otherwise build its pane again.
std::atomic<bool> g_unloading{false};


// -----------------------------------------------------------------------------
// Colours
// -----------------------------------------------------------------------------

// DirectUI hands an element that carries an attribute it cannot make sense of -
// an unknown name, or a value it fails to parse - back to the shell as a broken
// subtree, and the shell then gives up on the whole layout: the file list, the
// folder tree and everything else in the window stay blank, while SetXML still
// reports success. A colour out of the settings is the one part of the markup
// the mod does not write itself, so it is checked against what the parser
// actually accepts instead of being passed through.
struct ColorName {
    PCWSTR name;
    int sysColor;
};

constexpr ColorName kColorNames[] = {
    {L"activecaption", COLOR_ACTIVECAPTION},
    {L"appworkspace", COLOR_APPWORKSPACE},
    {L"background", COLOR_BACKGROUND},
    {L"buttonface", COLOR_BTNFACE},
    {L"buttonhighlight", COLOR_BTNHIGHLIGHT},
    {L"buttonshadow", COLOR_BTNSHADOW},
    {L"buttontext", COLOR_BTNTEXT},
    {L"captiontext", COLOR_CAPTIONTEXT},
    {L"graytext", COLOR_GRAYTEXT},
    {L"highlight", COLOR_HIGHLIGHT},
    {L"highlighttext", COLOR_HIGHLIGHTTEXT},
    {L"hotlight", COLOR_HOTLIGHT},
    {L"inactivecaption", COLOR_INACTIVECAPTION},
    {L"infobackground", COLOR_INFOBK},
    {L"infotext", COLOR_INFOTEXT},
    {L"menu", COLOR_MENU},
    {L"menutext", COLOR_MENUTEXT},
    {L"scrollbar", COLOR_SCROLLBAR},
    {L"threeddarkshadow", COLOR_3DDKSHADOW},
    {L"threedhighlight", COLOR_3DHIGHLIGHT},
    {L"threedlightshadow", COLOR_3DLIGHT},
    {L"threedshadow", COLOR_3DSHADOW},
    {L"window", COLOR_WINDOW},
    {L"windowframe", COLOR_WINDOWFRAME},
    {L"windowtext", COLOR_WINDOWTEXT},
};

// A colour the mod can both hand to DirectUI and paint with.
struct PaneColor {
    std::wstring dui = L"window";
    int sysColor = COLOR_WINDOW;  // -1 when the colour is a literal
    COLORREF rgb = 0;

    COLORREF Get() const {
        return sysColor >= 0 ? GetSysColor(sysColor) : rgb;
    }
};

static bool ParseHexColor(PCWSTR text, COLORREF* rgb) {
    if (*text != L'#' || wcslen(text) != 7) {
        return false;
    }

    unsigned value = 0;
    for (int i = 1; i <= 6; i++) {
        WCHAR c = towlower(text[i]);
        int digit;
        if (c >= L'0' && c <= L'9') {
            digit = c - L'0';
        } else if (c >= L'a' && c <= L'f') {
            digit = 10 + (c - L'a');
        } else {
            return false;
        }
        value = value * 16 + digit;
    }

    *rgb = RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
    return true;
}

static PaneColor ParseColor(PCWSTR text, PCWSTR fallbackName) {
    std::wstring trimmed = text ? text : L"";
    size_t first = trimmed.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        trimmed = fallbackName;
    } else {
        trimmed = trimmed.substr(first, trimmed.find_last_not_of(L" \t") - first + 1);
    }

    for (int attempt = 0; attempt < 2; attempt++) {
        for (const ColorName& known : kColorNames) {
            if (_wcsicmp(trimmed.c_str(), known.name) == 0) {
                PaneColor color;
                color.dui = known.name;
                color.sysColor = known.sysColor;
                return color;
            }
        }

        COLORREF rgb;
        if (ParseHexColor(trimmed.c_str(), &rgb)) {
            PaneColor color;
            WCHAR dui[64];
            swprintf(dui, ARRAYSIZE(dui), L"argb(255,%u,%u,%u)", GetRValue(rgb),
                     GetGValue(rgb), GetBValue(rgb));
            color.dui = dui;
            color.sysColor = -1;
            color.rgb = rgb;
            return color;
        }

        if (attempt == 0) {
            Wh_Log(L"'%s' is not a colour DirectUI knows, using '%s'",
                   trimmed.c_str(), fallbackName);
            trimmed = fallbackName;
        }
    }

    return PaneColor();
}

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

struct SeeAlsoLink {
    std::wstring label;
    std::wstring target;
};

struct {
    int paneWidth;
    int minListWidth;
    bool onRight;
    bool showHeader;
    int iconSize;
    bool showItemType;
    bool showDriveSpace;
    std::wstring usedLabel;
    std::wstring freeLabel;
    std::wstring totalLabel;
    PaneColor bar;
    std::wstring descriptionText;
    bool showFolderDescription;
    bool showSeeAlso;
    std::wstring seeAlsoTitle;
    std::vector<SeeAlsoLink> links;
    bool underlineLinks;
    std::wstring imagePath;
    enum class ImagePlace { Background, Top, Bottom } imagePlace;
    int imageWidth;
    bool imageSmooth;
    bool imageBlendWhite;
    int titleFontSize;
    bool paneBorder;
    bool removeViewBorder;
    bool hideDetailsPane;
    bool skipControlPanel;
    PaneColor background;
    PaneColor title;
    PaneColor text;
    PaneColor link;
    PaneColor divider;
    bool dividerGradient;
    std::wstring dividerImagePath;
    std::wstring spacerXml;  // the element handed to DirectUI, built once
} g_settings;

static std::wstring Px(int value) {
    return std::to_wstring(value) + L"rp";
}

static bool IsRussianUi() {
    static const bool russian =
        PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_RUSSIAN;
    return russian;
}

// English or Russian, following the Windows display language.
static const wchar_t* UiText(const wchar_t* english, const wchar_t* russian) {
    return IsRussianUi() ? russian : english;
}

// The texts in the settings default to the wording of the English Windows
// 2000. On a Russian Windows a text still left at that default is shown in
// the wording of the Russian Windows 2000 instead; anything the user typed
// is shown as it is.
static std::wstring LocalizedSetting(PCWSTR value) {
    static const struct {
        const wchar_t* english;
        const wchar_t* russian;
    } kDefaults[] = {
        {L"Used:", L"Занято:"},
        {L"Free:", L"Свободно:"},
        {L"Capacity:", L"Емкость:"},
        {L"Select an item to view its description.",
         L"Выберите объект для просмотра его описания."},
        {L"See also:", L"Перейти к:"},
        {L"My Documents", L"Мои документы"},
        {L"My Network Places", L"Сетевое окружение"},
        {L"My Computer", L"Мой компьютер"},
    };
    if (IsRussianUi()) {
        for (const auto& text : kDefaults) {
            if (wcscmp(value, text.english) == 0) {
                return text.russian;
            }
        }
    }
    return value;
}

// A path out of the settings, with any environment variables in it filled in.
// The pictures built into the mod are asked for by these names in place of a
// path; a real path cannot start with an asterisk.
constexpr WCHAR kBuiltInClouds[] = L"*win2000-clouds";
constexpr WCHAR kBuiltInSquares[] = L"*win2000-squares";
constexpr WCHAR kBuiltInDivider[] = L"*win2000-divider";

static std::wstring PathSetting(PCWSTR name) {
    WindhawkUtils::StringSetting setting =
        WindhawkUtils::StringSetting::make(name);
    if (!*setting.get()) {
        return L"";
    }

    WCHAR expanded[MAX_PATH * 2];
    DWORD length =
        ExpandEnvironmentStringsW(setting.get(), expanded, ARRAYSIZE(expanded));
    if (length > 0 && length <= ARRAYSIZE(expanded)) {
        return expanded;
    }

    return setting.get();
}

// A colour can either follow a stable system colour supplied by the current
// theme, or use the companion free-form setting. PaneColor keeps the system
// colour index rather than taking a snapshot, so GetSysColor is evaluated at
// paint time and a theme change is reflected without rebuilding the pane.
static PaneColor ThemedOrCustomColor(PCWSTR sourceSetting,
                                     PCWSTR colorSetting,
                                     PCWSTR themeColor,
                                     PCWSTR customFallback,
                                     bool themeByDefault) {
    WindhawkUtils::StringSetting source =
        WindhawkUtils::StringSetting::make(sourceSetting);
    bool fromTheme = *source.get()
                         ? _wcsicmp(source.get(), L"custom") != 0
                         : themeByDefault;
    if (fromTheme) {
        return ParseColor(themeColor, themeColor);
    }

    WindhawkUtils::StringSetting custom =
        WindhawkUtils::StringSetting::make(colorSetting);
    return ParseColor(custom.get(), customFallback);
}

static bool CanSyncSpacer();

static void LoadSettings() {
    g_settings.paneWidth = Wh_GetIntSetting(L"paneWidth");
    if (g_settings.paneWidth < 40) {
        g_settings.paneWidth = 40;
    } else if (g_settings.paneWidth > 2000) {
        g_settings.paneWidth = 2000;
    }

    g_settings.minListWidth = Wh_GetIntSetting(L"minListWidth");
    if (g_settings.minListWidth < 0) {
        g_settings.minListWidth = 0;
    } else if (g_settings.minListWidth > 2000) {
        g_settings.minListWidth = 2000;
    }

    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"position");
    g_settings.onRight = wcscmp(position.get(), L"right") == 0;

    g_settings.showHeader = Wh_GetIntSetting(L"showHeader");

    g_settings.iconSize = Wh_GetIntSetting(L"iconSize");
    if (g_settings.iconSize < 8) {
        g_settings.iconSize = 8;
    } else if (g_settings.iconSize > 256) {
        g_settings.iconSize = 256;
    }

    g_settings.showItemType = Wh_GetIntSetting(L"showItemType");
    g_settings.showDriveSpace = Wh_GetIntSetting(L"showDriveSpace");

    WindhawkUtils::StringSetting usedLabel =
        WindhawkUtils::StringSetting::make(L"usedLabel");
    WindhawkUtils::StringSetting freeLabel =
        WindhawkUtils::StringSetting::make(L"freeLabel");
    WindhawkUtils::StringSetting totalLabel =
        WindhawkUtils::StringSetting::make(L"totalLabel");
    g_settings.usedLabel = LocalizedSetting(usedLabel.get());
    g_settings.freeLabel = LocalizedSetting(freeLabel.get());
    g_settings.totalLabel = LocalizedSetting(totalLabel.get());

    g_settings.bar = ThemedOrCustomColor(
        L"barColorSource", L"colorBar", L"highlight", L"#000080", true);

    WindhawkUtils::StringSetting descriptionText =
        WindhawkUtils::StringSetting::make(L"descriptionText");
    g_settings.descriptionText = LocalizedSetting(descriptionText.get());
    g_settings.showFolderDescription =
        Wh_GetIntSetting(L"showFolderDescription");

    g_settings.showSeeAlso = Wh_GetIntSetting(L"showSeeAlso");

    WindhawkUtils::StringSetting seeAlsoTitle =
        WindhawkUtils::StringSetting::make(L"seeAlsoTitle");
    g_settings.seeAlsoTitle = LocalizedSetting(seeAlsoTitle.get());

    g_settings.links.clear();
    for (int i = 0;; i++) {
        WindhawkUtils::StringSetting label =
            WindhawkUtils::StringSetting::make(L"seeAlso[%d].label", i);
        WindhawkUtils::StringSetting target =
            WindhawkUtils::StringSetting::make(L"seeAlso[%d].target", i);
        if (!*label.get() && !*target.get()) {
            break;
        }
        if (*label.get() && *target.get()) {
            g_settings.links.push_back({LocalizedSetting(label.get()), target.get()});
        }
    }

    g_settings.underlineLinks = Wh_GetIntSetting(L"underlineLinks");

    WindhawkUtils::StringSetting picture =
        WindhawkUtils::StringSetting::make(L"picture");
    if (wcscmp(picture.get(), L"file") == 0) {
        g_settings.imagePath = PathSetting(L"imagePath");
    } else if (wcscmp(picture.get(), L"none") == 0) {
        g_settings.imagePath.clear();
    } else if (wcscmp(picture.get(), L"squares") == 0) {
        g_settings.imagePath = kBuiltInSquares;
    } else {
        g_settings.imagePath = kBuiltInClouds;
    }

    WindhawkUtils::StringSetting divider =
        WindhawkUtils::StringSetting::make(L"divider");
    if (wcscmp(divider.get(), L"file") == 0) {
        g_settings.dividerImagePath = PathSetting(L"dividerImagePath");
    } else if (wcscmp(divider.get(), L"drawn") == 0) {
        g_settings.dividerImagePath.clear();
    } else {
        g_settings.dividerImagePath = kBuiltInDivider;
    }

    WindhawkUtils::StringSetting imagePosition =
        WindhawkUtils::StringSetting::make(L"imagePosition");
    if (wcscmp(imagePosition.get(), L"top") == 0) {
        g_settings.imagePlace = decltype(g_settings.imagePlace)::Top;
    } else if (wcscmp(imagePosition.get(), L"bottom") == 0) {
        g_settings.imagePlace = decltype(g_settings.imagePlace)::Bottom;
    } else {
        g_settings.imagePlace = decltype(g_settings.imagePlace)::Background;
    }

    g_settings.imageWidth = Wh_GetIntSetting(L"imageWidth");
    if (g_settings.imageWidth < 0) {
        g_settings.imageWidth = 0;
    } else if (g_settings.imageWidth > 2000) {
        g_settings.imageWidth = 2000;
    }

    g_settings.imageSmooth = Wh_GetIntSetting(L"imageSmooth");
    g_settings.imageBlendWhite = Wh_GetIntSetting(L"imageBlendWhite");

    g_settings.titleFontSize = Wh_GetIntSetting(L"titleFontSize");
    if (g_settings.titleFontSize < 0) {
        g_settings.titleFontSize = 0;
    } else if (g_settings.titleFontSize > 72) {
        g_settings.titleFontSize = 72;
    }

    g_settings.paneBorder = Wh_GetIntSetting(L"paneBorder");
    g_settings.removeViewBorder = Wh_GetIntSetting(L"removeViewBorder");
    g_settings.hideDetailsPane = Wh_GetIntSetting(L"hideDetailsPane");
    g_settings.skipControlPanel = Wh_GetIntSetting(L"skipControlPanel");

    g_settings.background = ThemedOrCustomColor(
        L"backgroundColorSource", L"colorBackground", L"window", L"window",
        true);
    g_settings.title = ThemedOrCustomColor(
        L"titleColorSource", L"colorTitle", L"windowtext", L"windowtext",
        true);
    g_settings.text = ThemedOrCustomColor(
        L"textColorSource", L"colorText", L"windowtext", L"windowtext",
        true);
    g_settings.link = ThemedOrCustomColor(
        L"linkColorSource", L"colorLink", L"hotlight", L"#0000FF", false);
    g_settings.divider = ThemedOrCustomColor(
        L"dividerColorSource", L"colorDivider", L"highlight", L"#0000FF",
        false);
    g_settings.dividerGradient = Wh_GetIntSetting(L"dividerGradient");

    g_settings.spacerXml =
        L"<Element id=\"atom(ClassicWebViewPane)\" layoutpos=\"" +
        std::wstring(g_settings.onRight ? L"right" : L"left") + L"\" width=\"" +
        Px(CanSyncSpacer() ? 0 : g_settings.paneWidth) +
        L"\" background=\"" + g_settings.background.dui +
        L"\"/>";
}

// -----------------------------------------------------------------------------
// The layout hook
// -----------------------------------------------------------------------------

// The id of the spacer, also used to recognise a document the mod has already
// been through and one that carries somebody else's WebView.
constexpr WCHAR kPaneAtom[] = L"atom(ClassicWebViewPane)";

// The element that hosts the file list. The spacer goes right before it.
constexpr WCHAR kViewHostContainer[] = L"<Element id=\"atom(ViewHostContainer)\"";

constexpr WCHAR kDetailsContainer[] = L"<DetailsContainer layoutpos=\"right\"/>";
constexpr WCHAR kDetailsContainerHidden[] =
    L"<DetailsContainer layoutpos=\"none\" width=\"0\" height=\"0\"/>";

using SetXML_t = HRESULT(WINAPI*)(void* pThis,
                                  const WCHAR* pszXML,
                                  HINSTANCE hInst,
                                  HINSTANCE hResInst);
SetXML_t SetXML_Original;

HRESULT WINAPI SetXML_Hook(void* pThis,
                           const WCHAR* pszXML,
                           HINSTANCE hInst,
                           HINSTANCE hResInst) {
    if (!pszXML || g_unloading) {
        return SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    // Only folder layouts, and only ones the mod has not been through.
    const WCHAR* viewHost = wcsstr(pszXML, kViewHostContainer);
    if (!viewHost || wcsstr(pszXML, kPaneAtom)) {
        return SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    std::wstring modified(pszXML, viewHost);
    modified += g_settings.spacerXml;
    modified += viewHost;

    if (g_settings.hideDetailsPane) {
        size_t details = modified.find(kDetailsContainer);
        if (details != std::wstring::npos) {
            modified.replace(details, wcslen(kDetailsContainer),
                             kDetailsContainerHidden);
        }
    }

    HRESULT hr = SetXML_Original(pThis, modified.c_str(), hInst, hResInst);
    if (FAILED(hr)) {
        Wh_Log(L"The parser rejected the pane (%08X), using the original layout",
               hr);
        hr = SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    return hr;
}

// -----------------------------------------------------------------------------
// The pane window
// -----------------------------------------------------------------------------

constexpr PCWSTR kPaneClassName = L"ClassicWebViewPane";

constexpr UINT_PTR kRefreshTimer = 1;
// Retries finding the spacer, see OnSyncSpacer.
constexpr UINT_PTR kSpacerTimer = 2;
constexpr int kSpacerRetries = 20;
constexpr UINT WM_PANE_CLOSE = WM_APP + 1;
// Posted to the pane to widen or collapse its DirectUI spacer, see SyncSpacer.
constexpr UINT WM_PANE_SPACER = WM_APP + 2;
// Posted to every pane after a settings change. The pane's own thread is the
// only one that may use its state, and the only one its timers work on.
constexpr UINT WM_PANE_SETTINGS = WM_APP + 3;

// Sent to a folder view to make it build its pane on its own thread. Registered
// rather than WM_APP based - it goes to a window of the shell, not of the mod.
UINT g_adoptMessage;

struct PaneLink {
    RECT rect;
    size_t index;
};

struct Pane {
    HWND hwnd = nullptr;
    HWND host = nullptr;     // the DirectUIHWND the pane lives in
    HWND defView = nullptr;  // the folder view the contents come from

    std::wstring title;
    std::wstring subtitle;
    std::wstring folderDescription;
    bool folderDescriptionTooltip = false;
    HICON icon = nullptr;
    bool showingSelection = false;
    // Set on a Control Panel page, where the pane is kept hidden.
    bool suppressed = false;

    // The width of the splitter between the folder tree and the pane, as seen
    // while there is room for the whole pane (and the dpi it was seen at).
    int splitterGap = -1;
    int splitterGapDpi = 0;

    // The spacer was last seen collapsed, i.e. the pane has given way.
    bool spacerCollapsed = true;
    // No pane is shown until its spacer has been found and synchronized.
    bool spacerReady = false;
    // Set while OnSyncSpacer resizes the spacer.
    bool spacerTransition = false;
    // How many more times the spacer is looked for before giving up.
    int spacerRetries = kSpacerRetries;

    bool hasSpace = false;
    ULONGLONG capacity = 0;
    ULONGLONG freeSpace = 0;

    std::vector<PaneLink> linkRects;
    int hotLink = -1;
    bool tracking = false;

    HFONT font = nullptr;
    HFONT fontBold = nullptr;
    HFONT fontTitle = nullptr;
    HFONT fontUnderline = nullptr;
    HFONT fontTooltip = nullptr;
    int dpi = 96;
};

std::mutex g_panesMutex;
std::vector<HWND> g_panes;

struct Subclass {
    HWND hWnd;
    WindhawkUtils::WH_SUBCLASSPROC proc;
};

std::mutex g_subclassesMutex;
std::vector<Subclass> g_subclasses;

static bool AddSubclass(HWND hWnd,
                        WindhawkUtils::WH_SUBCLASSPROC proc,
                        DWORD_PTR data) {
    if (g_unloading) {
        return false;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, proc, data)) {
        return false;
    }

    std::lock_guard<std::mutex> guard(g_subclassesMutex);
    g_subclasses.push_back({hWnd, proc});
    return true;
}

static void DropSubclass(HWND hWnd, WindhawkUtils::WH_SUBCLASSPROC proc) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, proc);

    std::lock_guard<std::mutex> guard(g_subclassesMutex);
    std::erase_if(g_subclasses, [hWnd, proc](const Subclass& subclass) {
        return subclass.hWnd == hWnd && subclass.proc == proc;
    });
}

static void DropAllSubclasses() {
    std::vector<Subclass> subclasses;
    {
        std::lock_guard<std::mutex> guard(g_subclassesMutex);
        subclasses.swap(g_subclasses);
    }

    for (const Subclass& subclass : subclasses) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(subclass.hWnd,
                                                         subclass.proc);
    }
}

static bool IsClassName(HWND hWnd, PCWSTR name) {
    WCHAR className[64];
    if (!hWnd || !GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return _wcsicmp(className, name) == 0;
}

// The file list of Explorer carries a sunken border, and on a classic theme
// that border draws a line between the list and the pane, where Windows 2000
// had none. It is taken off the lists of the windows the pane is in and nowhere
// else, and every list it was taken off is remembered so that it can be given
// back when the mod stops.
std::mutex g_bordersMutex;
std::vector<HWND> g_borderless;

static void RestoreViewBorders() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> guard(g_bordersMutex);
        windows.swap(g_borderless);
    }

    for (HWND window : windows) {
        if (!IsWindow(window)) {
            continue;
        }

        LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
        SetWindowLongPtrW(window, GWL_EXSTYLE, exStyle | WS_EX_CLIENTEDGE);
        SetWindowPos(window, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);
    }
}

static void ApplyViewBorder(HWND defView) {
    if (!g_settings.removeViewBorder || !defView) {
        return;
    }

    for (HWND child = GetWindow(defView, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        LONG_PTR exStyle = GetWindowLongPtrW(child, GWL_EXSTYLE);
        if (!(exStyle & WS_EX_CLIENTEDGE)) {
            continue;
        }

        SetWindowLongPtrW(child, GWL_EXSTYLE, exStyle & ~WS_EX_CLIENTEDGE);
        SetWindowPos(child, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);

        std::lock_guard<std::mutex> guard(g_bordersMutex);
        g_borderless.push_back(child);
    }
}

static int Scale(const Pane* pane, int value) {
    return MulDiv(value, pane->dpi, 96);
}

// -----------------------------------------------------------------------------
// What the pane shows
// -----------------------------------------------------------------------------

// The browser behind a folder view is the way to both the folder and the
// selection. WM_GETISHELLBROWSER is answered by the window that hosts the view,
// not by the view itself: the tab of an Explorer window (every tab has a
// window, and a view, of its own, which keeps tabs apart), or the dialog for
// the file dialogs Explorer shows. The pointer carries no reference of its
// own; the browser outlives the view it belongs to.
static IShellBrowser* GetShellBrowser(HWND defView) {
    if (!defView || !IsWindow(defView)) {
        return nullptr;
    }

    for (HWND owner = GetParent(defView); owner; owner = GetParent(owner)) {
        if (!IsClassName(owner, L"ShellTabWindowClass") &&
            !IsClassName(owner, L"#32770")) {
            continue;
        }

        DWORD_PTR result = 0;
        if (SendMessageTimeoutW(owner, WM_GETISHELLBROWSER, 0, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
            result) {
            return reinterpret_cast<IShellBrowser*>(result);
        }
        return nullptr;
    }

    return nullptr;
}

static IFolderView* GetFolderView(HWND defView) {
    IShellBrowser* browser = GetShellBrowser(defView);
    if (!browser) {
        return nullptr;
    }

    IShellView* view = nullptr;
    HRESULT hr = browser->QueryActiveShellView(&view);
    if (FAILED(hr) || !view) {
        return nullptr;
    }

    IFolderView* folderView = nullptr;
    hr = view->QueryInterface(IID_IFolderView, (void**)&folderView);
    view->Release();
    return folderView;
}

static std::wstring GetItemText(IShellItem* item, SIGDN form) {
    PWSTR name = nullptr;
    if (FAILED(item->GetDisplayName(form, &name)) || !name) {
        return L"";
    }
    std::wstring result = name;
    CoTaskMemFree(name);
    return result;
}

static std::wstring FormatSize(ULONGLONG bytes) {
    WCHAR text[64];
    if (!StrFormatByteSizeW(bytes, text, ARRAYSIZE(text))) {
        return L"";
    }
    return text;
}

// System.ItemTypeText and System.Size, both out of the storage property set.
constexpr PROPERTYKEY kPropertyItemTypeText = {
    {0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
    4};
constexpr PROPERTYKEY kPropertySize = {
    {0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
    12};

// The kind of the item and, for a file, its size - the second line Windows 2000
// put under the name.
static std::wstring DescribeItem(IShellItem* item) {
    std::wstring description;

    IShellItem2* item2 = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_IShellItem2, (void**)&item2)) && item2) {
        PWSTR kind = nullptr;
        if (SUCCEEDED(item2->GetString(kPropertyItemTypeText, &kind)) && kind) {
            description = kind;
            CoTaskMemFree(kind);
        }

        SFGAOF attributes = 0;
        bool isFolder = SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, &attributes)) &&
                        (attributes & SFGAO_FOLDER);
        if (!isFolder) {
            ULONGLONG size = 0;
            if (SUCCEEDED(item2->GetUInt64(kPropertySize, &size)) && size) {
                std::wstring sizeText = FormatSize(size);
                if (!sizeText.empty()) {
                    description += description.empty() ? L"" : L", ";
                    description += sizeText;
                }
            }
        }

        item2->Release();
    }

    return description;
}

// System.Capacity and System.FreeSpace, out of the volume property set - what a
// drive answers with, and what Windows 2000 drew its bar of used space from.
constexpr PROPERTYKEY kPropertyFreeSpace = {
    {0x9B174B35, 0x40FF, 0x11D2, {0xA2, 0x7E, 0x00, 0xC0, 0x4F, 0xC3, 0x08, 0x71}},
    2};
constexpr PROPERTYKEY kPropertyCapacity = {
    {0x9B174B35, 0x40FF, 0x11D2, {0xA2, 0x7E, 0x00, 0xC0, 0x4F, 0xC3, 0x08, 0x71}},
    3};

static bool GetItemSpace(IShellItem* item,
                         ULONGLONG* capacity,
                         ULONGLONG* freeSpace) {
    *capacity = 0;
    *freeSpace = 0;

    // Only a drive gets the bar. The shell answers the volume properties for
    // any folder on the volume, so the item has to be the root of one.
    PWSTR path = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) || !path) {
        return false;
    }

    std::wstring root = path;
    CoTaskMemFree(path);

    if (!PathIsRootW(root.c_str())) {
        return false;
    }

    IShellItem2* item2 = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_IShellItem2, (void**)&item2)) &&
        item2) {
        ULONGLONG total = 0;
        ULONGLONG free = 0;
        if (SUCCEEDED(item2->GetUInt64(kPropertyCapacity, &total)) &&
            SUCCEEDED(item2->GetUInt64(kPropertyFreeSpace, &free)) && total > 0) {
            *capacity = total;
            *freeSpace = free;
        }
        item2->Release();
    }

    if (*capacity > 0) {
        return true;
    }

    // A drive that keeps no such properties - a network one, say - still
    // answers the plain question about its path.
    ULARGE_INTEGER available = {};
    ULARGE_INTEGER total = {};
    ULARGE_INTEGER free = {};
    if (GetDiskFreeSpaceExW(root.c_str(), &available, &total, &free) &&
        total.QuadPart > 0) {
        *capacity = total.QuadPart;
        *freeSpace = free.QuadPart;
    }

    return *capacity > 0;
}

static HICON GetItemIcon(IShellItem* item, int size) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHGetIDListFromObject(item, &pidl)) || !pidl) {
        return nullptr;
    }

    SHFILEINFOW info = {};
    UINT flags = SHGFI_PIDL | SHGFI_ICON |
                 (size <= 16 ? SHGFI_SMALLICON : SHGFI_LARGEICON);
    HICON icon = nullptr;
    if (SHGetFileInfoW((PCWSTR)pidl, 0, &info, sizeof(info), flags)) {
        icon = info.hIcon;
    }

    CoTaskMemFree(pidl);
    return icon;
}

static IShellItem* GetPaneItem(HWND defView, bool* isSelection) {
    *isSelection = false;

    IFolderView* folderView = GetFolderView(defView);
    if (!folderView) {
        return nullptr;
    }

    IShellItem* item = nullptr;

    int selected = 0;
    if (SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &selected)) &&
        selected == 1) {
        IShellItemArray* array = nullptr;
        if (SUCCEEDED(folderView->Items(SVGIO_SELECTION, IID_IShellItemArray,
                                        (void**)&array)) &&
            array) {
            if (SUCCEEDED(array->GetItemAt(0, &item)) && item) {
                *isSelection = true;
            }
            array->Release();
        }
    }

    if (!item) {
        IPersistFolder2* folder = nullptr;
        if (SUCCEEDED(folderView->GetFolder(IID_IPersistFolder2, (void**)&folder)) &&
            folder) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(folder->GetCurFolder(&pidl)) && pidl) {
                SHCreateItemFromIDList(pidl, IID_IShellItem, (void**)&item);
                CoTaskMemFree(pidl);
            }
            folder->Release();
        }
    }

    folderView->Release();
    return item;
}

struct SystemFolderDescription {
    std::wstring text;
    // Ordinary folder summaries in the Windows 2000 WebView were displayed in
    // an info-tip coloured box. Recycle Bin had its own plain paragraph.
    bool tooltip = true;
};

static bool IsKnownFolder(PCIDLIST_ABSOLUTE itemPidl,
                          REFKNOWNFOLDERID folderId) {
    PIDLIST_ABSOLUTE knownPidl = nullptr;
    bool equal = SUCCEEDED(SHGetKnownFolderIDList(
                     folderId, KF_FLAG_DEFAULT, nullptr, &knownPidl)) &&
                 knownPidl && ILIsEqual(itemPidl, knownPidl);
    CoTaskMemFree(knownPidl);
    return equal;
}

// What tells a page that draws the task column from the Control Panel folder
// itself is not where it sits - a page opened straight by its own CLSID,
// shell:::{7b81be6a-...} for Programs and Features, carries no trace of the
// Control Panel in its parsing name and still gets the column - but whether the
// folder being shown is registered as a Control Panel item. The folder of the
// Control Panel, its all-items view included, is not one of those, which is why
// it keeps the pane.
static bool IsRegisteredControlPanelItem(PCWSTR parsing) {
    // Control Panel items always appear as "::{GUID}"; the last one is the
    // folder itself. A plain directory that happens to be named after a GUID
    // has no "::" in front of it and is left alone.
    const WCHAR* moniker = nullptr;
    for (const WCHAR* p = parsing; p[0] && p[1] && p[2]; p++) {
        if (p[0] == L':' && p[1] == L':' && p[2] == L'{') {
            moniker = p + 2;
        }
    }
    if (!moniker) {
        return false;
    }

    const WCHAR* end = wcschr(moniker, L'}');
    if (!end) {
        return false;
    }

    std::wstring key =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"
        L"\\ControlPanel\\NameSpace\\";
    key.append(moniker, end - moniker + 1);

    HKEY handle = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &handle) !=
        ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(handle);
    return true;
}

// The page of a Control Panel item puts a column of task links of its own
// exactly where the pane goes. That column is drawn by DirectUI rather than
// being a window of its own, so the layout pass that keeps the pane clear of the
// folder tree and the details pane cannot see it, and the two are drawn one over
// the other. The pane keeps off those pages instead.
static bool IsControlPanelFolder(HWND defView) {
    IFolderView* folderView = GetFolderView(defView);
    if (!folderView) {
        return false;
    }

    bool isControlPanel = false;

    IPersistFolder2* folder = nullptr;
    if (SUCCEEDED(folderView->GetFolder(IID_IPersistFolder2, (void**)&folder)) &&
        folder) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(folder->GetCurFolder(&pidl)) && pidl) {
            PWSTR parsing = nullptr;
            if (SUCCEEDED(SHGetNameFromIDList(
                    pidl, SIGDN_DESKTOPABSOLUTEPARSING, &parsing)) &&
                parsing) {
                isControlPanel = IsRegisteredControlPanelItem(parsing);
                CoTaskMemFree(parsing);
            }
            CoTaskMemFree(pidl);
        }
        folder->Release();
    }

    folderView->Release();
    return isControlPanel;
}

static int GetFolderItemCount(HWND defView) {
    IFolderView* folderView = GetFolderView(defView);
    if (!folderView) {
        return -1;
    }

    int count = -1;
    folderView->ItemCount(SVGIO_ALLVIEW, &count);
    folderView->Release();
    return count;
}

// A short summary of each system folder for the classic WebView, rather than
// one generic sentence for every shell namespace folder.
static SystemFolderDescription DescribeSystemFolder(IShellItem* item,
                                                     HWND defView) {
    PIDLIST_ABSOLUTE itemPidl = nullptr;
    if (FAILED(SHGetIDListFromObject(item, &itemPidl)) || !itemPidl) {
        return {};
    }

    SystemFolderDescription description;
    if (IsKnownFolder(itemPidl, FOLDERID_RecycleBinFolder)) {
        description.tooltip = false;
        description.text = UiText(
            L"This folder contains files and folders that you have deleted "
            L"from your computer.",
            L"Эта папка содержит файлы и папки, которые были удалены с "
            L"компьютера.");
        int count = GetFolderItemCount(defView);
        if (count == 0) {
            description.text += UiText(L"\n\nThe Recycle Bin is empty.",
                                       L"\n\nКорзина пуста.");
        } else if (count > 0) {
            description.text +=
                UiText(L"\n\nThe Recycle Bin contains deleted items.",
                       L"\n\nВ корзине находятся удалённые элементы.");
        }
    } else if (IsKnownFolder(itemPidl, FOLDERID_Documents)) {
        description.text = UiText(L"Stores and manages documents.",
                                  L"Хранит документы и управляет ими.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_ComputerFolder)) {
        description.text =
            UiText(L"Displays the files and folders on this computer.",
                   L"Отображает файлы и папки на этом компьютере.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_NetworkFolder)) {
        description.text =
            UiText(L"Displays the computers and devices on the network.",
                   L"Отображает компьютеры и устройства в сети.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_ControlPanelFolder)) {
        description.text =
            UiText(L"Changes the settings of your computer.",
                   L"Изменяет параметры и настройки компьютера.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_Downloads)) {
        description.text =
            UiText(L"Stores the files downloaded from the Internet.",
                   L"Хранит файлы, загруженные из Интернета.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_Pictures)) {
        description.text = UiText(L"Stores and manages pictures.",
                                  L"Хранит изображения и управляет ими.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_Music)) {
        description.text =
            UiText(L"Stores music and other sound files.",
                   L"Хранит музыку и другие звуковые файлы.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_Videos)) {
        description.text = UiText(L"Stores and manages video files.",
                                  L"Хранит видеофайлы и управляет ими.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_Desktop)) {
        description.text = UiText(L"Displays the items on the desktop.",
                                  L"Отображает объекты рабочего стола.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_Profile)) {
        description.text =
            UiText(L"Contains your personal files and folders.",
                   L"Содержит ваши личные файлы и папки.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_ConnectionsFolder)) {
        description.text =
            UiText(L"Displays the network connections of this computer.",
                   L"Отображает сетевые подключения компьютера.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_PrintersFolder)) {
        description.text =
            UiText(L"Displays and manages the installed printers.",
                   L"Отображает установленные принтеры и управляет ими.");
    } else if (IsKnownFolder(itemPidl, FOLDERID_Libraries)) {
        description.text =
            UiText(L"Gathers files from different folders into libraries.",
                   L"Собирает файлы из разных папок в библиотеки.");
    }

    CoTaskMemFree(itemPidl);
    return description;
}

static void LayOutPane(HWND paneWindow);

static void RefreshPane(Pane* pane) {
    if (g_settings.skipControlPanel && IsControlPanelFolder(pane->defView)) {
        if (!pane->suppressed) {
            pane->suppressed = true;
            ShowWindow(pane->hwnd, SW_HIDE);
        }
        return;
    }

    if (pane->suppressed) {
        // Navigated back out of the Control Panel - put the pane back.
        pane->suppressed = false;
        LayOutPane(pane->hwnd);
    }

    std::wstring title;
    std::wstring subtitle;
    std::wstring folderDescription;
    bool folderDescriptionTooltip = false;
    HICON icon = nullptr;
    bool hasSpace = false;
    ULONGLONG capacity = 0;
    ULONGLONG freeSpace = 0;

    bool isSelection = false;
    IShellItem* item = GetPaneItem(pane->defView, &isSelection);
    if (item) {
        title = GetItemText(item, SIGDN_NORMALDISPLAY);
        if (g_settings.showItemType) {
            subtitle = DescribeItem(item);
        }
        if (g_settings.showHeader) {
            icon = GetItemIcon(item, Scale(pane, g_settings.iconSize));
        }
        if (g_settings.showDriveSpace) {
            hasSpace = GetItemSpace(item, &capacity, &freeSpace);
        }
        if (!isSelection && g_settings.showFolderDescription) {
            SystemFolderDescription description =
                DescribeSystemFolder(item, pane->defView);
            folderDescription = std::move(description.text);
            folderDescriptionTooltip = description.tooltip;
        }
        item->Release();
    }

    if (pane->title == title && pane->subtitle == subtitle &&
        pane->folderDescription == folderDescription &&
        pane->folderDescriptionTooltip == folderDescriptionTooltip &&
        pane->showingSelection == isSelection && pane->hasSpace == hasSpace &&
        pane->capacity == capacity && pane->freeSpace == freeSpace &&
        pane->icon && icon) {
        // Same item, nothing to repaint - drop the icon that was just fetched.
        DestroyIcon(icon);
        return;
    }

    pane->hasSpace = hasSpace;
    pane->capacity = capacity;
    pane->freeSpace = freeSpace;

    if (pane->icon) {
        DestroyIcon(pane->icon);
    }
    pane->icon = icon;
    pane->title = title;
    pane->subtitle = subtitle;
    pane->folderDescription = folderDescription;
    pane->folderDescriptionTooltip = folderDescriptionTooltip;
    pane->showingSelection = isSelection;

    InvalidateRect(pane->hwnd, nullptr, TRUE);
}

// -----------------------------------------------------------------------------
// Painting
// -----------------------------------------------------------------------------

// The web view of Windows 2000 was drawn with pictures out of its
// %SystemRoot%\Web: clouds or coloured squares in the corner and a bar of colour
// under the name. Those come built into the mod; a file from the
// settings is loaded by the shell instead - which is what makes GIF, PNG and
// JPEG work next to BMP without dragging in an imaging library.
struct PaneImage {
    HBITMAP bitmap = nullptr;
    HICON icon = nullptr;
    int width = 0;
    int height = 0;
    bool alpha = false;
};

// The pane draws two pictures: the one in its corner and the coloured line
// under the name.
struct CachedImage {
    PaneImage image;
    std::wstring path;
    bool loaded = false;
};

std::mutex g_imageMutex;
CachedImage g_picture;
CachedImage g_dividerPicture;

static void FreeCachedImage(CachedImage& cache) {
    if (cache.image.bitmap) {
        DeleteObject(cache.image.bitmap);
    }
    if (cache.image.icon) {
        DestroyIcon(cache.image.icon);
    }
    cache = {};
}

static void FreeImage() {
    FreeCachedImage(g_picture);
    FreeCachedImage(g_dividerPicture);
}

// AlphaBlend wants the colour channels premultiplied by the alpha one, and what
// comes back from a file depends on where it came from: the shell hands over
// premultiplied bits, a 32 bit BMP usually carries an alpha channel that is
// either straight or plain unused. Both are told apart by looking at the bits -
// a channel brighter than its own alpha cannot be premultiplied, and an alpha
// channel that is zero everywhere is not one.
static bool NormalizeAlpha(const BITMAP& info) {
    if (info.bmBitsPixel != 32 || !info.bmBits) {
        return false;
    }

    BYTE* bits = (BYTE*)info.bmBits;
    bool anyAlpha = false;
    bool straight = false;

    for (int row = 0; row < info.bmHeight && !straight; row++) {
        BYTE* pixel = bits + (size_t)row * info.bmWidthBytes;
        for (int column = 0; column < info.bmWidth; column++, pixel += 4) {
            if (pixel[3]) {
                anyAlpha = true;
            }
            if (pixel[0] > pixel[3] || pixel[1] > pixel[3] ||
                pixel[2] > pixel[3]) {
                straight = true;
                break;
            }
        }
    }

    if (!anyAlpha) {
        return false;
    }
    if (!straight) {
        return true;
    }

    for (int row = 0; row < info.bmHeight; row++) {
        BYTE* pixel = bits + (size_t)row * info.bmWidthBytes;
        for (int column = 0; column < info.bmWidth; column++, pixel += 4) {
            BYTE alpha = pixel[3];
            pixel[0] = (BYTE)(pixel[0] * alpha / 255);
            pixel[1] = (BYTE)(pixel[1] * alpha / 255);
            pixel[2] = (BYTE)(pixel[2] * alpha / 255);
        }
    }

    return true;
}

static bool HasExtension(const std::wstring& path, PCWSTR extension) {
    size_t dot = path.rfind(L'.');
    return dot != std::wstring::npos &&
           _wcsicmp(path.c_str() + dot, extension) == 0;
}

// The built-in pictures are the Windows 2000 ones, 8 bit BMPs stored as PNG.
static const char kWin2000Clouds[] =
    "iVBORw0KGgoAAAANSUhEUgAAALYAAADtCAMAAAAlZ38CAAADAFBMVEX////3///v///W7/e1"
    "3u/e7/fG5/fn9/+93u+l1u+ExueUzu+MxufW7//O5/e13vet1u+czu+Exu97vefG3u+l1veM"
    "xu+93ve11u+Eveet1velzu+Uxu+Mvefe7//W5/fO5//v9//G3ve91u+11vetzu+lzvecxu+M"
    "ve+tzvfW5//G3v+91vfO3vfn7//39/8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB3"
    "7CuGAAARkklEQVR42u2de3/ayM/FhyVgSLiYbcLFBByMA9s8YL//d/dIR5qLSdKmLU3s38ei"
    "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";

static const char kWin2000Squares[] =
    "iVBORw0KGgoAAAANSUhEUgAAAH8AAAA4CAMAAADjPtf1AAADAFBMVEUAAACAAAAAgACAgAAA"
    "AICAAIAAgIDAwMDA3MCmyvAEBAQICAgMDAwREREWFhYcHBwiIiIpKSlVVVVNTU1CQkI5OTn/"
    "fID/UFDWAJPM7P/v1sbn59atqZAzAABmAACZAADMAAAAMwAzMwBmMwCZMwDMMwD/MwAAZgAz"
    "ZgBmZgCZZgDMZgD/ZgAAmQAzmQBmmQCZmQDMmQD/mQAAzAAzzABmzACZzADMzAD/zABm/wCZ"
    "/wDM/wAAADMzADNmADOZADPMADP/ADMAMzMzMzNmMzOZMzPMMzP/MzMAZjMzZjNmZjOZZjPM"
    "ZjP/ZjMAmTMzmTNmmTOZmTPMmTP/mTMAzDMzzDNmzDOZzDPMzDP/zDMz/zNm/zOZ/zPM/zP/"
    "/zMAAGYzAGZmAGaZAGbMAGb/AGYAM2YzM2ZmM2aZM2bMM2b/M2YAZmYzZmZmZmaZZmbMZmYA"
    "mWYzmWZmmWaZmWbMmWb/mWYAzGYzzGaZzGbMzGb/zGYA/2Yz/2aZ/2bM/2b/AMzMAP8AmZmZ"
    "M5mZAJnMAJkAAJkzM5lmAJnMM5n/AJkAZpkzZplmM5mZZpnMZpn/M5kzmZlmmZmZmZnMmZn/"
    "mZkAzJkzzJlmzGaZzJnMzJn/zJkA/5kz/5lmzJmZ/5nM/5n//5kAAMwzAJlmAMyZAMzMAMwA"
    "M5kzM8xmM8yZM8zMM8z/M8wAZswzZsxmZpmZZszMZsz/ZpkAmcwzmcxmmcyZmczMmcz/mcwA"
    "zMwzzMxmzMyZzMzMzMz/zMwA/8wz/8xm/5mZ/8zM/8z//8wzAMxmAP+ZAP8AM8wzM/9mM/+Z"
    "M//MM///M/8AZv8zZv9mZsyZZv/MZv//ZswAmf8zmf9mmf+Zmf/Mmf//mf8AzP8zzP9mzP+Z"
    "zP/MzP//zP8z//9m/8yZ///M////ZmZm/2b//2ZmZv//Zv9m//+lACFfX193d3eGhoaWlpbL"
    "y8uysrLX19fd3d3j4+Pq6urx8fH4+Pj/+/CgoKSAgID/AAAA/wD//wAAAP//AP8A//////9Y"
    "Ik63AAAF9UlEQVR42uVZTW7jNhT20r7AABRzA/cozsZZ9B52DEi6RhayBaTwIZLNFN3IZ+hs"
    "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";

static const char kWin2000Divider[] =
    "iVBORw0KGgoAAAANSUhEUgAAAKgAAAABCAMAAAB9hhWgAAADAFBMVEUAAACAAAAAgACAgAAA"
    "AICAAIAAgIDAwMDA3MCmyvAEBAQICAgMDAwREREWFhYcHBwiIiIpKSlVVVVNTU1CQkI5OTn/"
    "fID/UFDWAJPM7P/v1sbn59atqZAzAABmAACZAADMAAAAMwAzMwBmMwCZMwDMMwD/MwAAZgAz"
    "ZgBmZgCZZgDMZgD/ZgAAmQAzmQBmmQCZmQDMmQD/mQAAzAAzzABmzACZzADMzAD/zABm/wCZ"
    "/wDM/wAAADMzADNmADOZADPMADP/ADMAMzMzMzNmMzOZMzPMMzP/MzMAZjMzZjNmZjOZZjPM"
    "ZjP/ZjMAmTMzmTNmmTOZmTPMmTP/mTMAzDMzzDNmzDOZzDPMzDP/zDMz/zNm/zOZ/zPM/zP/"
    "/zMAAGYzAGZmAGaZAGbMAGb/AGYAM2YzM2ZmM2aZM2bMM2b/M2YAZmYzZmZmZmaZZmbMZmYA"
    "mWYzmWZmmWaZmWbMmWb/mWYAzGYzzGaZzGbMzGb/zGYA/2Yz/2aZ/2bM/2b/AMzMAP8AmZmZ"
    "M5mZAJnMAJkAAJkzM5lmAJnMM5n/AJkAZpkzZplmM5mZZpnMZpn/M5kzmZlmmZmZmZnMmZn/"
    "mZkAzJkzzJlmzGaZzJnMzJn/zJkA/5kz/5lmzJmZ/5nM/5n//5kAAMwzAJlmAMyZAMzMAMwA"
    "M5kzM8xmM8yZM8zMM8z/M8wAZswzZsxmZpmZZszMZsz/ZpkAmcwzmcxmmcyZmczMmcz/mcwA"
    "zMwzzMxmzMyZzMzMzMz/zMwA/8wz/8xm/5mZ/8zM/8z//8wzAMxmAP+ZAP8AM8wzM/9mM/+Z"
    "M//MM///M/8AZv8zZv9mZsyZZv/MZv//ZswAmf8zmf9mmf+Zmf/Mmf//mf8AzP8zzP9mzP+Z"
    "zP/MzP//zP8z//9m/8yZ///M////ZmZm/2b//2ZmZv//Zv9m//+lACFfX193d3eGhoaWlpbL"
    "y8uysrLX19fd3d3j4+Pq6urx8fH4+Pj/+/CgoKSAgID/AAAA/wD//wAAAP//AP8A//////9Y"
    "Ik63AAAAFElEQVR42mNQIxJYkAbCiAaXiQMAgy4+es9dmycAAAAASUVORK5CYII=";

static std::vector<BYTE> DecodeBase64(const char* text) {
    std::vector<BYTE> bytes;
    unsigned int buffer = 0;
    int bits = 0;
    for (; *text && *text != '='; text++) {
        char c = *text;
        int value = c >= 'A' && c <= 'Z'   ? c - 'A'
                    : c >= 'a' && c <= 'z' ? c - 'a' + 26
                    : c >= '0' && c <= '9' ? c - '0' + 52
                    : c == '+'             ? 62
                    : c == '/'             ? 63
                                           : -1;
        if (value < 0) {
            continue;
        }
        buffer = (buffer << 6) | value;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            bytes.push_back((BYTE)(buffer >> bits));
        }
    }
    return bytes;
}

// Decodes a built-in picture with WIC into a 32 bit DIB. Its fourth byte is
// cleared, so the picture counts as one without an alpha channel of its own and
// its white is let through like that of the original BMPs.
static PaneImage LoadBuiltInImage(const std::wstring& name) {
    PaneImage image;

    const char* data = name == kBuiltInClouds    ? kWin2000Clouds
                       : name == kBuiltInSquares ? kWin2000Squares
                       : name == kBuiltInDivider ? kWin2000Divider
                                                 : nullptr;
    if (!data) {
        return image;
    }

    std::vector<BYTE> png = DecodeBase64(data);
    IStream* stream = SHCreateMemStream(png.data(), (UINT)png.size());
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    UINT width = 0;
    UINT height = 0;

    if (stream &&
        SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&factory))) &&
        SUCCEEDED(factory->CreateDecoderFromStream(
            stream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder)) &&
        SUCCEEDED(decoder->GetFrame(0, &frame)) &&
        SUCCEEDED(factory->CreateFormatConverter(&converter)) &&
        SUCCEEDED(converter->Initialize(frame, GUID_WICPixelFormat32bppBGR,
                                        WICBitmapDitherTypeNone, nullptr, 0,
                                        WICBitmapPaletteTypeCustom)) &&
        SUCCEEDED(converter->GetSize(&width, &height)) && width > 0 &&
        height > 0 && width <= 4096 && height <= 4096) {
        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = (LONG)width;
        info.bmiHeader.biHeight = -(LONG)height;  // top down
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        BYTE* bits = nullptr;
        HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS,
                                          (void**)&bits, nullptr, 0);
        if (bitmap && SUCCEEDED(converter->CopyPixels(
                          nullptr, width * 4, width * 4 * height, bits))) {
            for (size_t i = 3; i < (size_t)width * height * 4; i += 4) {
                bits[i] = 0;
            }
            image.bitmap = bitmap;
            image.width = (int)width;
            image.height = (int)height;
        } else if (bitmap) {
            DeleteObject(bitmap);
        }
    }

    if (converter) {
        converter->Release();
    }
    if (frame) {
        frame->Release();
    }
    if (decoder) {
        decoder->Release();
    }
    if (factory) {
        factory->Release();
    }
    if (stream) {
        stream->Release();
    }

    return image;
}

static PaneImage LoadPaneImage(const std::wstring& path) {
    if (path[0] == L'*') {
        return LoadBuiltInImage(path);
    }

    PaneImage image;

    if (HasExtension(path, L".ico") || HasExtension(path, L".cur")) {
        image.icon = (HICON)LoadImageW(nullptr, path.c_str(), IMAGE_ICON, 0, 0,
                                       LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (image.icon) {
            ICONINFO info = {};
            if (GetIconInfo(image.icon, &info)) {
                BITMAP bitmap = {};
                HBITMAP source = info.hbmColor ? info.hbmColor : info.hbmMask;
                if (GetObjectW(source, sizeof(bitmap), &bitmap)) {
                    image.width = bitmap.bmWidth;
                    image.height =
                        info.hbmColor ? bitmap.bmHeight : bitmap.bmHeight / 2;
                }
                if (info.hbmColor) {
                    DeleteObject(info.hbmColor);
                }
                if (info.hbmMask) {
                    DeleteObject(info.hbmMask);
                }
            }
            return image;
        }
    }

    HBITMAP bitmap =
        (HBITMAP)LoadImageW(nullptr, path.c_str(), IMAGE_BITMAP, 0, 0,
                            LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!bitmap) {
        IShellItem* item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                                  IID_IShellItem,
                                                  (void**)&item)) &&
            item) {
            IShellItemImageFactory* factory = nullptr;
            if (SUCCEEDED(item->QueryInterface(IID_IShellItemImageFactory,
                                               (void**)&factory)) &&
                factory) {
                SIZE size = {512, 512};
                factory->GetImage(size, SIIGBF_BIGGERSIZEOK, &bitmap);
                factory->Release();
            }
            item->Release();
        }
    }

    if (bitmap) {
        BITMAP info = {};
        if (GetObjectW(bitmap, sizeof(info), &info) && info.bmWidth > 0 &&
            info.bmHeight > 0) {
            image.bitmap = bitmap;
            image.width = info.bmWidth;
            image.height = info.bmHeight;
            image.alpha = NormalizeAlpha(info);
        } else {
            DeleteObject(bitmap);
        }
    }

    return image;
}

// Loads what the path points at, once, and hands it back for as long as the
// setting keeps pointing there. The caller holds the lock.
static const PaneImage* EnsureImage(CachedImage& cache,
                                    const std::wstring& path) {
    if (path.empty()) {
        if (cache.loaded) {
            FreeCachedImage(cache);
        }
        return nullptr;
    }

    if (!cache.loaded || cache.path != path) {
        FreeCachedImage(cache);
        cache.image = LoadPaneImage(path);
        cache.path = path;
        cache.loaded = true;
        if (!cache.image.bitmap && !cache.image.icon) {
            Wh_Log(L"'%s' could not be loaded", path.c_str());
        }
    }

    if (!cache.image.width || !cache.image.height) {
        return nullptr;
    }

    return &cache.image;
}

// A picture with no alpha channel of its own - the Windows 2000 one is an 8 bit
// BMP - is drawn on white, and a white block is exactly what it turns into on a
// pane that is not white. Multiplying it into what is already there is the way
// out: white leaves the background alone, everything else tints it, and a
// gradient fading to white fades out instead of ending at an edge.
static void MultiplyIntoBackground(HDC dc,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   HBITMAP source,
                                   int sourceWidth,
                                   int sourceHeight) {
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;  // top down
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    BYTE* backgroundBits = nullptr;
    BYTE* pictureBits = nullptr;
    HDC backgroundDc = CreateCompatibleDC(dc);
    HDC pictureDc = CreateCompatibleDC(dc);
    HBITMAP background = CreateDIBSection(dc, &info, DIB_RGB_COLORS,
                                          (void**)&backgroundBits, nullptr, 0);
    HBITMAP picture = CreateDIBSection(dc, &info, DIB_RGB_COLORS,
                                       (void**)&pictureBits, nullptr, 0);

    if (backgroundDc && pictureDc && background && picture) {
        HBITMAP oldBackground = (HBITMAP)SelectObject(backgroundDc, background);
        HBITMAP oldPicture = (HBITMAP)SelectObject(pictureDc, picture);

        BitBlt(backgroundDc, 0, 0, width, height, dc, x, y, SRCCOPY);

        HDC sourceDc = CreateCompatibleDC(dc);
        HBITMAP oldSource = (HBITMAP)SelectObject(sourceDc, source);
        SetStretchBltMode(pictureDc,
                          g_settings.imageSmooth ? HALFTONE : COLORONCOLOR);
        StretchBlt(pictureDc, 0, 0, width, height, sourceDc, 0, 0, sourceWidth,
                   sourceHeight, SRCCOPY);
        SelectObject(sourceDc, oldSource);
        DeleteDC(sourceDc);

        size_t count = (size_t)width * height * 4;
        for (size_t i = 0; i < count; i++) {
            if (i % 4 == 3) {
                continue;  // the unused byte of an RGB DIB
            }
            backgroundBits[i] =
                (BYTE)(backgroundBits[i] * pictureBits[i] / 255);
        }

        BitBlt(dc, x, y, width, height, backgroundDc, 0, 0, SRCCOPY);

        SelectObject(backgroundDc, oldBackground);
        SelectObject(pictureDc, oldPicture);
    }

    if (background) {
        DeleteObject(background);
    }
    if (picture) {
        DeleteObject(picture);
    }
    if (backgroundDc) {
        DeleteDC(backgroundDc);
    }
    if (pictureDc) {
        DeleteDC(pictureDc);
    }
}

// The line under the name was a picture as well - a thin bar of colour. It is
// stretched across the pane and keeps the height it was drawn at. Returns that
// height, or nothing when there is no picture to draw.
static int PaintDividerImage(HDC dc, int x, int y, int width, int dpi) {
    std::lock_guard<std::mutex> guard(g_imageMutex);

    const PaneImage* line =
        EnsureImage(g_dividerPicture, g_settings.dividerImagePath);
    if (!line || !line->bitmap || width <= 0) {
        return 0;
    }

    int height = MulDiv(line->height, dpi, 96);
    if (height < 1) {
        height = 1;
    }

    if (!dc) {
        return height;
    }

    HDC source = CreateCompatibleDC(dc);
    HBITMAP old = (HBITMAP)SelectObject(source, line->bitmap);

    if (line->alpha) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, x, y, width, height, source, 0, 0, line->width,
                   line->height, blend);
    } else {
        int mode = SetStretchBltMode(
            dc, g_settings.imageSmooth ? HALFTONE : COLORONCOLOR);
        StretchBlt(dc, x, y, width, height, source, 0, 0, line->width,
                   line->height, SRCCOPY);
        SetStretchBltMode(dc, mode);
    }

    SelectObject(source, old);
    DeleteDC(source);

    return height;
}

// Draws a loaded picture into the given rectangle: an icon as it is, a picture
// with alpha by it, and one without either multiplied into the background or
// copied over it.
static void DrawPaneImage(HDC dc,
                          const PaneImage& image,
                          int x,
                          int y,
                          int width,
                          int height) {
    if (image.icon) {
        DrawIconEx(dc, x, y, image.icon, width, height, 0, nullptr, DI_NORMAL);
        return;
    }

    if (!image.alpha && g_settings.imageBlendWhite) {
        MultiplyIntoBackground(dc, x, y, width, height, image.bitmap,
                               image.width, image.height);
        return;
    }

    HDC source = CreateCompatibleDC(dc);
    HBITMAP old = (HBITMAP)SelectObject(source, image.bitmap);

    if (image.alpha) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, x, y, width, height, source, 0, 0, image.width,
                   image.height, blend);
    } else {
        int mode = SetStretchBltMode(
            dc, g_settings.imageSmooth ? HALFTONE : COLORONCOLOR);
        StretchBlt(dc, x, y, width, height, source, 0, 0, image.width,
                   image.height, SRCCOPY);
        SetStretchBltMode(dc, mode);
    }

    SelectObject(source, old);
    DeleteDC(source);
}

// Measures the picture, and draws it as well when a device context is given.
// The lock is held across the drawing so that a settings change cannot free the
// bitmap out from under a paint on another thread.
static SIZE PaintPaneImage(HDC dc, int x, int y, int dpi, int maxWidth) {
    SIZE size = {0, 0};

    std::lock_guard<std::mutex> guard(g_imageMutex);

    const PaneImage* picture = EnsureImage(g_picture, g_settings.imagePath);
    if (!picture) {
        return size;
    }
    const PaneImage& g_image = *picture;

    // Without a width of its own the picture is stretched across the pane, the
    // way the one of Windows 2000 spans the top of its web view however wide it
    // is. Either way the proportions of the picture are kept.
    size.cx = g_settings.imageWidth > 0 ? MulDiv(g_settings.imageWidth, dpi, 96)
                                        : maxWidth;
    if (size.cx > maxWidth) {
        size.cx = maxWidth;
    }
    size.cy = MulDiv(size.cx, g_image.height, g_image.width);

    if (size.cx <= 0 || size.cy <= 0) {
        size.cx = 0;
        size.cy = 0;
        return size;
    }

    if (dc) {
        DrawPaneImage(dc, g_image, x, y, size.cx, size.cy);
    }

    return size;
}


static void EnsureFonts(Pane* pane) {
    if (pane->font) {
        return;
    }

    NONCLIENTMETRICSW metrics = {sizeof(metrics)};
    if (!SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics),
                                    &metrics, 0, pane->dpi)) {
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);
    }

    LOGFONTW logFont = metrics.lfMessageFont;
    pane->font = CreateFontIndirectW(&logFont);

    LOGFONTW bold = logFont;
    bold.lfWeight = FW_BOLD;
    pane->fontBold = CreateFontIndirectW(&bold);

    // Windows 2000 set the name of the folder well above the rest of the pane.
    LOGFONTW title = bold;
    if (g_settings.titleFontSize > 0) {
        title.lfHeight = -MulDiv(g_settings.titleFontSize, pane->dpi, 72);
        title.lfWidth = 0;
    }
    pane->fontTitle = CreateFontIndirectW(&title);

    LOGFONTW underline = logFont;
    underline.lfUnderline = TRUE;
    pane->fontUnderline = CreateFontIndirectW(&underline);

    // NONCLIENTMETRICS exposes the font which the current Windows theme uses
    // for status text and tooltips. The classic description box follows it.
    pane->fontTooltip = CreateFontIndirectW(&metrics.lfStatusFont);
}

static void FreeFonts(Pane* pane) {
    if (pane->font) {
        DeleteObject(pane->font);
        pane->font = nullptr;
    }
    if (pane->fontBold) {
        DeleteObject(pane->fontBold);
        pane->fontBold = nullptr;
    }
    if (pane->fontTitle) {
        DeleteObject(pane->fontTitle);
        pane->fontTitle = nullptr;
    }
    if (pane->fontUnderline) {
        DeleteObject(pane->fontUnderline);
        pane->fontUnderline = nullptr;
    }
    if (pane->fontTooltip) {
        DeleteObject(pane->fontTooltip);
        pane->fontTooltip = nullptr;
    }
}

// Draws wrapped text and returns the height it took.
static int DrawWrapped(HDC dc,
                       PCWSTR text,
                       HFONT font,
                       COLORREF color,
                       int x,
                       int y,
                       int width) {
    if (!text || !*text) {
        return 0;
    }

    HFONT old = (HFONT)SelectObject(dc, font);
    SetTextColor(dc, color);

    RECT rect = {x, y, x + width, y};
    DrawTextW(dc, text, -1, &rect, DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);
    DrawTextW(dc, text, -1, &rect, DT_WORDBREAK | DT_NOPREFIX);

    SelectObject(dc, old);
    return rect.bottom - rect.top;
}

// Draws the small shell description as a native info-tip: its font, foreground
// and background all come from the active Windows theme.
static int DrawTooltipDescription(HDC dc,
                                  Pane* pane,
                                  PCWSTR text,
                                  int x,
                                  int y,
                                  int width) {
    if (!text || !*text || width <= 0) {
        return 0;
    }

    int paddingX = Scale(pane, 3);
    int paddingY = Scale(pane, 2);
    int innerWidth = std::max(1, width - paddingX * 2);

    HFONT oldFont =
        (HFONT)SelectObject(dc, pane->fontTooltip ? pane->fontTooltip : pane->font);
    SetTextColor(dc, GetSysColor(COLOR_INFOTEXT));

    RECT textRect = {x + paddingX, y + paddingY,
                     x + paddingX + innerWidth, y + paddingY};
    UINT flags = DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL;
    DrawTextW(dc, text, -1, &textRect, flags | DT_CALCRECT);

    RECT box = {x, y, x + width, textRect.bottom + paddingY};
    HBRUSH background = CreateSolidBrush(GetSysColor(COLOR_INFOBK));
    FillRect(dc, &box, background);
    DeleteObject(background);

    HBRUSH border = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
    FrameRect(dc, &box, border);
    DeleteObject(border);

    DrawTextW(dc, text, -1, &textRect, flags);
    SelectObject(dc, oldFont);
    return box.bottom - box.top;
}

static COLORREF MixColor(COLORREF first, COLORREF second, int firstWeight) {
    int secondWeight = 255 - firstWeight;
    return RGB((GetRValue(first) * firstWeight + GetRValue(second) * secondWeight) /
                   255,
               (GetGValue(first) * firstWeight + GetGValue(second) * secondWeight) /
                   255,
               (GetBValue(first) * firstWeight + GetBValue(second) * secondWeight) /
                   255);
}

// The classic disk page draws the wall as a region below the top ellipse.
// A second Pie would put its radial edges on the wall and leave gaps at the
// sides. Only the front half has a visible wall, split vertically at the
// sector's endpoint when the used space exceeds one half.
static void DrawDrivePie(HDC dc,
                         RECT bounds,
                         unsigned usedPer1000,
                         COLORREF usedColor,
                         COLORREF freeColor) {
    int availableWidth = bounds.right - bounds.left;
    int availableHeight = bounds.bottom - bounds.top;
    int width = std::min(availableWidth, availableHeight * 2);
    int height = width / 2;
    if (width < 20 || height < 10) {
        return;
    }

    bounds.left += (availableWidth - width) / 2;
    bounds.right = bounds.left + width;
    bounds.top += (availableHeight - height) / 2;
    bounds.bottom = bounds.top + height;

    int depth = std::max(2, height / 6);
    RECT top = bounds;
    top.bottom -= depth;
    int rx = (top.right - top.left) / 2;
    int ry = (top.bottom - top.top) / 2;
    top.right = top.left + 2 * rx;
    top.bottom = top.top + 2 * ry;
    int cx = top.left + rx;
    int cy = top.top + ry;

    usedPer1000 = std::min(usedPer1000, 1000u);
    double freeFraction = (1000.0 - usedPer1000) / 1000.0;
    constexpr double pi = 3.14159265358979323846;
    double angle = pi + freeFraction * 2.0 * pi;
    POINT end = {cx + (int)std::lround(rx * cos(angle)),
                 cy - (int)std::lround(ry * sin(angle))};
    // Pie accepts rays outside the ellipse. Keep their direction precise so
    // a tiny sector cannot round back to the starting ray and fill the pie.
    constexpr int rayLength = 10000;
    POINT ray = {cx + (int)std::lround(rx * rayLength * cos(angle)),
                 cy - (int)std::lround(ry * rayLength * sin(angle))};

    COLORREF frameColor = GetSysColor(COLOR_WINDOWFRAME);
    COLORREF usedShadow = MixColor(usedColor, RGB(0, 0, 0), 128);
    COLORREF freeShadow = MixColor(freeColor, RGB(0, 0, 0), 128);
    HPEN pen = CreatePen(PS_SOLID, 1, frameColor);
    HPEN oldPen = (HPEN)SelectObject(dc, pen);
    int oldDirection = SetArcDirection(dc, AD_COUNTERCLOCKWISE);

    HRGN ellipse = CreateEllipticRgnIndirect(&top);
    OffsetRgn(ellipse, 0, depth);
    HRGN wall = CreateRectRgn(top.left, cy, top.right, cy + depth);
    CombineRgn(wall, wall, ellipse, RGN_OR);
    OffsetRgn(ellipse, 0, -depth);
    CombineRgn(wall, wall, ellipse, RGN_DIFF);

    HBRUSH brush = CreateSolidBrush(usedPer1000 == 1000 ? usedShadow : freeShadow);
    FillRgn(dc, wall, brush);
    DeleteObject(brush);

    if (usedPer1000 > 500 && usedPer1000 < 1000) {
        HRGN usedWall = CreateRectRgn(end.x, cy, top.right, top.bottom + depth);
        CombineRgn(usedWall, wall, usedWall, RGN_AND);
        brush = CreateSolidBrush(usedShadow);
        FillRgn(dc, usedWall, brush);
        DeleteObject(brush);
        DeleteObject(usedWall);
    }
    DeleteObject(wall);
    DeleteObject(ellipse);

    brush = CreateSolidBrush(usedPer1000 == 0 ? freeColor : usedColor);
    HBRUSH oldBrush = (HBRUSH)SelectObject(dc, brush);
    Ellipse(dc, top.left, top.top, top.right, top.bottom);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);

    if (usedPer1000 > 0 && usedPer1000 < 1000) {
        brush = CreateSolidBrush(freeColor);
        oldBrush = (HBRUSH)SelectObject(dc, brush);
        Pie(dc, top.left, top.top, top.right, top.bottom,
            cx - rx * rayLength, cy,
            ray.x, ray.y);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        if (usedPer1000 > 500) {
            MoveToEx(dc, end.x, end.y, nullptr);
            LineTo(dc, end.x, end.y + depth);
        }
    }

    Arc(dc, top.left, top.top + depth, top.right - 1, top.bottom + depth - 1,
        top.left, cy + depth, top.right, cy + depth - 1);
    MoveToEx(dc, top.left, cy, nullptr);
    LineTo(dc, top.left, cy + depth);
    MoveToEx(dc, top.right - 1, cy, nullptr);
    LineTo(dc, top.right - 1, cy + depth);

    SelectObject(dc, oldPen);
    SetArcDirection(dc, oldDirection);
    DeleteObject(pen);
}

static void PaintPane(Pane* pane, HDC target, const RECT& client) {
    EnsureFonts(pane);

    int width = client.right - client.left;
    int height = client.bottom - client.top;

    HDC dc = CreateCompatibleDC(target);
    HBITMAP bitmap = CreateCompatibleBitmap(target, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, bitmap);

    HBRUSH background = CreateSolidBrush(g_settings.background.Get());
    FillRect(dc, &client, background);
    DeleteObject(background);

    SetBkMode(dc, TRANSPARENT);

    // The left, top and bottom of the frame that goes round the pane and the
    // file list together; the right of it belongs to the list, whose own left
    // edge the pane covers.
    int edge = 0;
    if (g_settings.paneBorder && !g_settings.removeViewBorder) {
        edge = GetSystemMetricsForDpi(SM_CXEDGE, pane->dpi);
        RECT frame = client;
        if (g_settings.onRight) {
            DrawEdge(dc, &frame, EDGE_SUNKEN, BF_RIGHT | BF_TOP | BF_BOTTOM);
        } else {
            DrawEdge(dc, &frame, EDGE_SUNKEN, BF_LEFT | BF_TOP | BF_BOTTOM);
        }
    }

    int padding = Scale(pane, 12);
    int x = padding + (g_settings.onRight ? 0 : edge);
    int y = padding + edge;
    int contentWidth = width - padding * 2 - edge;

    pane->linkRects.clear();

    // Narrower than its own padding, the pane is only a strip of background.
    if (contentWidth > 0) {
    // In Windows 2000 the picture sits right in the corner of the pane, with
    // the icon and the name over it, so it gets neither the padding nor a share
    // of the height.
    if (g_settings.imagePlace == decltype(g_settings.imagePlace)::Background) {
        PaintPaneImage(dc, g_settings.onRight ? 0 : edge, edge, pane->dpi,
                       width - edge);
    } else if (g_settings.imagePlace == decltype(g_settings.imagePlace)::Top) {
        SIZE image = PaintPaneImage(nullptr, 0, 0, pane->dpi, contentWidth);
        if (image.cy > 0) {
            PaintPaneImage(dc, x, y, pane->dpi, contentWidth);
            y += image.cy + Scale(pane, 8);
        }
    }

    // The icon sits on a line of its own with the name in bold under it, the
    // way the web view of Windows 2000 had it - not next to it, the way the
    // details pane of Windows 11 does.
    if (g_settings.showHeader) {
        if (pane->icon) {
            int iconSize = Scale(pane, g_settings.iconSize);
            DrawIconEx(dc, x, y, pane->icon, iconSize, iconSize, 0, nullptr,
                       DI_NORMAL);
            y += iconSize + Scale(pane, 4);
        }

        y += DrawWrapped(dc, pane->title.c_str(), pane->fontTitle,
                         g_settings.title.Get(), x, y, contentWidth);

        if (!pane->subtitle.empty()) {
            y += Scale(pane, 1);
            y += DrawWrapped(dc, pane->subtitle.c_str(), pane->font,
                             g_settings.text.Get(), x, y, contentWidth);
        }

        y += Scale(pane, 6);
    }

    {
        // The line runs the whole width of the pane, edge to edge, and is the
        // one thing in it that ignores the padding - it stops at the frame,
        // and at the strip that lies over the border of the file list.
        int lineLeft = edge;
        int lineRight = width - edge;
        int drawn =
            PaintDividerImage(dc, lineLeft, y, lineRight - lineLeft, pane->dpi);
        RECT line = {lineLeft, y, lineRight,
                     y + (drawn > 0 ? drawn : std::max(1, Scale(pane, 1)))};

        if (drawn > 0) {
            // The picture is the line.
        } else if (g_settings.dividerGradient) {
            // The line of Windows 2000 was a picture that started in colour and
            // faded out to the right; a gradient into the background of the
            // pane is the same thing without the file.
            COLORREF from = g_settings.divider.Get();
            COLORREF to = g_settings.background.Get();

            TRIVERTEX vertices[2] = {};
            vertices[0].x = line.left;
            vertices[0].y = line.top;
            vertices[0].Red = (USHORT)(GetRValue(from) * 257);
            vertices[0].Green = (USHORT)(GetGValue(from) * 257);
            vertices[0].Blue = (USHORT)(GetBValue(from) * 257);
            vertices[1].x = line.right;
            vertices[1].y = line.bottom;
            vertices[1].Red = (USHORT)(GetRValue(to) * 257);
            vertices[1].Green = (USHORT)(GetGValue(to) * 257);
            vertices[1].Blue = (USHORT)(GetBValue(to) * 257);

            GRADIENT_RECT band = {0, 1};
            GradientFill(dc, vertices, 2, &band, 1, GRADIENT_FILL_RECT_H);
        } else {
            HBRUSH divider = CreateSolidBrush(g_settings.divider.Get());
            FillRect(dc, &line, divider);
            DeleteObject(divider);
        }

        y = line.bottom + Scale(pane, 10);
    }

    // Windows 2000 put this instruction before the capacity and chart when a
    // drive itself was being viewed.
    if (pane->hasSpace && !g_settings.descriptionText.empty() &&
        !pane->showingSelection) {
        y += DrawWrapped(dc, g_settings.descriptionText.c_str(), pane->font,
                         g_settings.text.Get(), x, y, contentWidth);
        y += Scale(pane, 14);
    }

    // What a drive gets: capacity, the used/free legend and the classic 3-D
    // pie chart. The construction follows the Disk Pie Chart Windhawk mod and
    // the old shell implementation it was reconstructed from.
    if (pane->hasSpace && pane->capacity > 0) {
        HFONT oldFont = (HFONT)SelectObject(dc, pane->font);
        SetTextColor(dc, g_settings.text.Get());

        TEXTMETRICW metrics = {};
        GetTextMetricsW(dc, &metrics);
        int lineHeight = metrics.tmHeight;
        int rowGap = Scale(pane, 4);
        ULONGLONG used = pane->capacity > pane->freeSpace
                             ? pane->capacity - pane->freeSpace
                             : 0;
        COLORREF usedColor = g_settings.bar.Get();
        COLORREF freeColor =
            MixColor(usedColor, g_settings.background.Get(), 64);

        std::wstring capacity = g_settings.totalLabel;
        if (!capacity.empty()) {
            capacity += L" ";
        }
        capacity += FormatSize(pane->capacity);
        RECT capacityRect = {x, y, x + contentWidth, y + lineHeight};
        DrawTextW(dc, capacity.c_str(), -1, &capacityRect,
                  DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX |
                      DT_END_ELLIPSIS);
        y = capacityRect.bottom + Scale(pane, 12);

        const struct {
            const std::wstring* label;
            ULONGLONG value;
            COLORREF color;
        } legend[] = {
            {&g_settings.usedLabel, used, usedColor},
            {&g_settings.freeLabel, pane->freeSpace, freeColor},
        };

        int square = std::max(8, Scale(pane, 11));
        int legendX = x + Scale(pane, 16);
        for (const auto& item : legend) {
            RECT swatch = {legendX, y + (lineHeight - square) / 2,
                           legendX + square, y + (lineHeight - square) / 2 + square};
            HBRUSH swatchBrush = CreateSolidBrush(item.color);
            FillRect(dc, &swatch, swatchBrush);
            DeleteObject(swatchBrush);
            DrawEdge(dc, &swatch, BDR_SUNKENOUTER, BF_RECT);

            std::wstring text = *item.label;
            if (!text.empty()) {
                text += L" ";
            }
            text += FormatSize(item.value);
            RECT textRect = {swatch.right + Scale(pane, 5), y,
                             x + contentWidth, y + lineHeight};
            DrawTextW(dc, text.c_str(), -1, &textRect,
                      DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX |
                          DT_END_ELLIPSIS);
            y += lineHeight + rowGap;
        }

        y += Scale(pane, 10);
        int pieWidth = std::min(contentWidth, Scale(pane, 120));
        int pieHeight = std::max(Scale(pane, 60), pieWidth / 2);
        RECT pie = {x, y, x + pieWidth, y + pieHeight};
        unsigned usedPer1000 =
            (unsigned)((double)used * 1000.0 / (double)pane->capacity);
        DrawDrivePie(dc, pie, usedPer1000, usedColor, freeColor);
        y = pie.bottom;

        SelectObject(dc, oldFont);
        y += Scale(pane, 8);
    }

    // Ordinary system folders get their own Windows 2000 summary. Most used an
    // info-tip box below the generic instruction; Recycle Bin used a standalone
    // paragraph and therefore replaces that instruction.
    if (!pane->hasSpace && !pane->showingSelection) {
        bool standaloneDescription = !pane->folderDescription.empty() &&
                                     !pane->folderDescriptionTooltip;
        bool drewInstruction = false;
        if (!standaloneDescription && !g_settings.descriptionText.empty()) {
            y += DrawWrapped(dc, g_settings.descriptionText.c_str(), pane->font,
                             g_settings.text.Get(), x, y, contentWidth);
            drewInstruction = true;
        }

        if (!pane->folderDescription.empty()) {
            if (pane->folderDescriptionTooltip) {
                if (drewInstruction) {
                    y += Scale(pane, 8);
                }
                y += DrawTooltipDescription(dc, pane,
                                            pane->folderDescription.c_str(), x,
                                            y, contentWidth);
            } else {
                y += DrawWrapped(dc, pane->folderDescription.c_str(), pane->font,
                                 g_settings.text.Get(), x, y, contentWidth);
            }
        }
    }

    if (g_settings.showSeeAlso && !g_settings.links.empty()) {
        y += Scale(pane, 16);

        if (!g_settings.seeAlsoTitle.empty()) {
            y += DrawWrapped(dc, g_settings.seeAlsoTitle.c_str(), pane->fontBold,
                             g_settings.text.Get(), x, y, contentWidth);
            y += Scale(pane, 4);
        }

        for (size_t i = 0; i < g_settings.links.size(); i++) {
            const std::wstring& label = g_settings.links[i].label;
            HFONT font = g_settings.underlineLinks || (int)i == pane->hotLink
                             ? pane->fontUnderline
                             : pane->font;

            HFONT old = (HFONT)SelectObject(dc, font);
            SIZE size = {};
            GetTextExtentPoint32W(dc, label.c_str(), (int)label.length(), &size);
            SetTextColor(dc, g_settings.link.Get());
            RECT rect = {x, y, x + std::min<LONG>(size.cx, contentWidth), y + size.cy};
            DrawTextW(dc, label.c_str(), -1, &rect,
                      DT_SINGLELINE | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS);
            SelectObject(dc, old);

            pane->linkRects.push_back({rect, i});
            y = rect.bottom + Scale(pane, 3);
        }
    }

    if (g_settings.imagePlace == decltype(g_settings.imagePlace)::Bottom) {
        SIZE image = PaintPaneImage(nullptr, 0, 0, pane->dpi, contentWidth);
        int imageY = height - padding - image.cy;
        if (image.cy > 0 && imageY > y) {
            PaintPaneImage(dc, x, imageY, pane->dpi, contentWidth);
        }
    }
    }

    BitBlt(target, 0, 0, width, height, dc, 0, 0, SRCCOPY);

    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
}

// -----------------------------------------------------------------------------
// Clicking a link
// -----------------------------------------------------------------------------

static void FollowLink(Pane* pane, const SeeAlsoLink& link) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(link.target.c_str(), nullptr, &pidl, 0,
                                  nullptr)) ||
        !pidl) {
        Wh_Log(L"'%s' is not something the shell can navigate to",
               link.target.c_str());
        return;
    }

    IShellBrowser* browser = GetShellBrowser(pane->defView);
    if (browser) {
        browser->BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
    } else {
        SHELLEXECUTEINFOW info = {sizeof(info)};
        info.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
        info.lpIDList = pidl;
        info.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&info);
    }

    CoTaskMemFree(pidl);
}

static int LinkFromPoint(Pane* pane, POINT point) {
    for (const PaneLink& link : pane->linkRects) {
        if (PtInRect(&link.rect, point)) {
            return (int)link.index;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// The window itself
// -----------------------------------------------------------------------------

// Whether a window is shown as far as its own style goes. IsWindowVisible also
// looks at the parents, and a freshly opened Explorer window lays everything
// out before it is shown itself - to IsWindowVisible every pane in it is
// hidden then, and nothing tells the panes when the window appears.
static bool IsShown(HWND hWnd) {
    return (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_VISIBLE) != 0;
}

// The window among the host's children that holds the folder view - the one
// DirectUI moves, resizes and hides.
static HWND GetHostChildOf(HWND host, HWND hWnd) {
    while (hWnd && GetParent(hWnd) != host) {
        hWnd = GetParent(hWnd);
    }
    return hWnd;
}

// -----------------------------------------------------------------------------
// The spacer as a DirectUI element
// -----------------------------------------------------------------------------

// Windows 2000 let the WebView go first when a window got narrow: the file
// list kept its room and the pane disappeared. DirectUI does the opposite with
// the spacer - it has a fixed width, so the list is squeezed out before it. So
// the mod collapses the spacer to nothing itself when the list would get too
// little room, and gives it its width back when there is enough again.
//
// The element is reached through the DirectUIHWND: HWNDElement keeps itself in
// the first pointer of the window's extra bytes (HWNDElement::StaticWndProc
// reads it from there), and the spacer is found under it by its id.
struct DuiApi {
    bool ok = false;
    ATOM(WINAPI* StrToID)(PCWSTR) = nullptr;
    void*(__cdecl* FindDescendent)(void* element, ATOM id) = nullptr;
    int(__cdecl* GetWidth)(void* element) = nullptr;
    HRESULT(__cdecl* SetWidth)(void* element, int width) = nullptr;
    HWND(__cdecl* GetHWND)(void* hwndElement) = nullptr;
};

DuiApi g_dui;

static bool CanSyncSpacer() {
    return g_dui.ok;
}

// The host whose spacer OnSyncSpacer is resizing right now, on this thread,
// and the one window in it that is let paint meanwhile, see RepaintFileList.
thread_local HWND g_spacerTransitionHost;
thread_local HWND g_spacerTransitionPainting;

static void LoadDuiApi(HMODULE dui70) {
#ifdef _WIN64
    g_dui.StrToID = (decltype(g_dui.StrToID))GetProcAddress(dui70, "StrToID");
    g_dui.FindDescendent = (decltype(g_dui.FindDescendent))GetProcAddress(
        dui70, "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z");
    g_dui.GetWidth = (decltype(g_dui.GetWidth))GetProcAddress(
        dui70, "?GetWidth@Element@DirectUI@@QEAAHXZ");
    g_dui.SetWidth = (decltype(g_dui.SetWidth))GetProcAddress(
        dui70, "?SetWidth@Element@DirectUI@@QEAAJH@Z");
    g_dui.GetHWND = (decltype(g_dui.GetHWND))GetProcAddress(
        dui70, "?GetHWND@HWNDElement@DirectUI@@UEAAPEAUHWND__@@XZ");
    g_dui.ok = g_dui.StrToID && g_dui.FindDescendent && g_dui.GetWidth &&
               g_dui.SetWidth && g_dui.GetHWND;
    if (!g_dui.ok) {
        Wh_Log(L"DirectUI element functions not found, the pane will not give "
               L"way in narrow windows");
    }
#endif
}

// Only on the thread of the host.
static void* FindSpacer(HWND host) {
    if (!g_dui.ok || !IsClassName(host, L"DirectUIHWND") ||
        GetClassLongPtrW(host, GCL_CBWNDEXTRA) < (LONG_PTR)sizeof(void*)) {
        return nullptr;
    }

    void* root = (void*)GetWindowLongPtrW(host, 0);
    if (!root || g_dui.GetHWND(root) != host) {
        return nullptr;
    }

    // Looked up every time, never kept: StrToID is FindAtomW, and the id is an
    // atom DirectUI adds while elements carry it and deletes when the last of
    // them goes. Once every window with a spacer in it had been closed, the
    // next one got its spacer under a new number - a kept one found nothing
    // any more, and the pane gave up on its spacer for good (hidden, with a
    // blank strip where it was in a narrow window).
    ATOM paneAtom = g_dui.StrToID(L"ClassicWebViewPane");
    if (!paneAtom) {
        return nullptr;
    }

    return g_dui.FindDescendent(root, paneAtom);
}

// Whether the room DirectUI has for the file list and the pane together - all
// of the host but the folder tree with its splitter and the details pane - is
// enough for the pane and the minimum left to the list. It does not depend on
// the width of the spacer, so collapsing the spacer cannot flip it back.
static bool PaneFits(Pane* pane, HWND viewWindow) {
    if (g_settings.minListWidth <= 0 || !g_dui.ok) {
        return true;
    }

    RECT hostClient;
    if (!GetClientRect(pane->host, &hostClient) ||
        hostClient.right <= hostClient.left ||
        hostClient.bottom <= hostClient.top) {
        return false;
    }

    UINT dpi = GetDpiForWindow(pane->host);
    if (!dpi) {
        dpi = 96;
    }

    int left = hostClient.left;
    int right = hostClient.right;
    bool besideTree = false;
    for (HWND child = GetWindow(pane->host, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        if (child == pane->hwnd || child == viewWindow || !IsShown(child)) {
            continue;
        }

        RECT childRect;
        if (!GetWindowRect(child, &childRect)) {
            continue;
        }
        MapWindowPoints(nullptr, pane->host, (POINT*)&childRect, 2);
        if (childRect.right <= childRect.left) {
            continue;
        }

        // Docked to the left edge: the folder tree. To the right edge: the
        // details pane.
        if (childRect.left <= hostClient.left + 1) {
            left = std::max(left, (int)childRect.right);
            besideTree = true;
        } else if (childRect.right >= hostClient.right - 1) {
            right = std::min(right, (int)childRect.left);
        }
    }

    if (besideTree) {
        int gap = MulDiv(3, dpi, 96);
        if (pane->splitterGap >= 0 && pane->splitterGapDpi == (int)dpi) {
            gap = pane->splitterGap;
        }
        left += gap;
    }

    int needed = MulDiv(g_settings.paneWidth + g_settings.minListWidth, dpi, 96);
    return right - left >= needed;
}

// Asks the pane's own thread - the host's - to bring the spacer in line, with
// a timer: the layout may be run from another thread, and it is usually run in
// the middle of a DirectUI layout pass, which is no place to start another
// one. A timer rather than a posted message and a "pending" flag: a timer keeps
// firing until it is handled and several requests fold into one by
// themselves, so there is no state that could be left stuck - a lost message
// once left the spacer at its full width for good in a narrow window, the pane
// hidden and a blank strip in its place.
static void SyncSpacer(Pane* pane) {
    if (!g_dui.ok) {
        return;
    }

    if (GetWindowThreadProcessId(pane->hwnd, nullptr) == GetCurrentThreadId()) {
        // A newly attached view starts with a collapsed spacer. Wait for a
        // quiet layout interval before deciding how much room it has: its
        // folder tree and the restored window size may still be changing.
        SetTimer(pane->hwnd, kSpacerTimer,
                 pane->spacerReady ? USER_TIMER_MINIMUM : 50, nullptr);
    } else {
        PostMessageW(pane->hwnd, WM_PANE_SPACER, 0, 0);
    }
}

// Repaints the file list after the spacer has changed its width, slowest part
// first. The items take a good ten milliseconds, but the list draws them off
// screen and puts them up at once, and until then the old picture stays; its
// frame with the scroll bars and the column headers are quick and follow right
// after, and the pane after them. Painted the other way round, or all of it
// composited - which does not make a list with its headers one piece either -
// the headers and the pane stand next to the old items for those ten
// milliseconds.
static void RepaintFileList(HWND viewWindow) {
    HWND listView = nullptr;
    HWND defView = FindWindowExW(viewWindow, nullptr, L"SHELLDLL_DefView", nullptr);
    if (defView) {
        listView = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    }

    if (!listView) {
        RedrawWindow(viewWindow, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN |
                         RDW_UPDATENOW);
        return;
    }

    // The list repaints its column headers itself in the middle of that, so
    // everything else stays held until it is done.
    g_spacerTransitionHost = GetParent(viewWindow);
    g_spacerTransitionPainting = listView;
    RedrawWindow(listView, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_NOCHILDREN);
    g_spacerTransitionPainting = nullptr;
    g_spacerTransitionHost = nullptr;

    SendMessageW(listView, WM_NCPAINT, 1, 0);
    for (HWND child = GetWindow(listView, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        RedrawWindow(child, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN |
                         RDW_UPDATENOW);
    }
}

static BOOL CALLBACK HoldChildProc(HWND hWnd, LPARAM);

static void OnSyncSpacer(Pane* pane) {
    if (!pane->host || !pane->defView || !IsWindow(pane->defView)) {
        return;
    }

    void* spacer = FindSpacer(pane->host);
    if (!spacer) {
        // Right after a window opens, the view's layout is not always in the
        // element tree yet. Keep the pane hidden while looking for it: an
        // unconfirmed gap can belong to a folder tree that is still loading.
        pane->spacerReady = false;
        pane->spacerCollapsed = true;
        ShowWindow(pane->hwnd, SW_HIDE);
        if (pane->spacerRetries > 0) {
            pane->spacerRetries--;
            SetTimer(pane->hwnd, kSpacerTimer, 150, nullptr);
        }
        return;
    }

    pane->spacerRetries = kSpacerRetries;

    UINT dpi = GetDpiForWindow(pane->host);
    if (!dpi) {
        dpi = 96;
    }

    HWND viewWindow = GetHostChildOf(pane->host, pane->defView);

    int wanted = 0;
    if (!pane->suppressed && PaneFits(pane, viewWindow)) {
        wanted = MulDiv(g_settings.paneWidth, dpi, 96);
    }

    // A new view starts collapsed, even when the preceding one was expanded.
    int current = g_dui.GetWidth(spacer);
    bool wasCollapsed = pane->spacerCollapsed;
    bool wasReady = pane->spacerReady;
    pane->spacerReady = true;
    pane->spacerCollapsed = current == 0;
    if (current == wanted) {
        if (!wasReady || pane->spacerCollapsed != wasCollapsed) {
            LayOutPane(pane->hwnd);
        }
        return;
    }

    // DirectUI lays the host out again right inside SetWidth, moving and
    // resizing the file list. It is moved without its old picture (see
    // SiblingSubclassProc), so nothing on screen changes until the list, and
    // the pane, are repainted below - each in one go, rather than the pane
    // vanishing first and the list sliding over with its old picture.
    if (viewWindow) {
        EnumChildWindows(viewWindow, HoldChildProc, 0);
    }

    g_spacerTransitionHost = pane->host;
    pane->spacerTransition = true;
    HRESULT hr = g_dui.SetWidth(spacer, wanted);
    // SetWidth can run a nested layout pass. Recheck the resulting room
    // before showing anything, including a tree created by that pass.
    if (SUCCEEDED(hr) && wanted != 0 && !PaneFits(pane, viewWindow)) {
        wanted = 0;
        hr = g_dui.SetWidth(spacer, wanted);
    }
    pane->spacerTransition = false;
    g_spacerTransitionHost = nullptr;
    pane->spacerCollapsed = g_dui.GetWidth(spacer) == 0;
    pane->spacerReady = SUCCEEDED(hr) && g_dui.GetWidth(spacer) == wanted;

    if (wanted == 0 || !pane->spacerReady) {
        // Hidden without repainting what it uncovers: that is the file list
        // now, which is repainted as a whole right below.
        SetWindowPos(pane->hwnd, nullptr, 0, 0, 0, 0,
                     SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOACTIVATE | SWP_NOREDRAW);
    } else {
        // Placed and shown, but not painted yet (it has no background to
        // erase): the list goes first, it takes longer, and the pane right
        // after it.
        LayOutPane(pane->hwnd);
    }

    if (viewWindow) {
        RepaintFileList(viewWindow);
    }

    if (wanted != 0 && pane->spacerReady) {
        RedrawWindow(pane->hwnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_UPDATENOW);
    }

    // Whatever is left - the view's own background, all covered by the list.
    RedrawWindow(viewWindow ? viewWindow : pane->host, nullptr, nullptr,
                 RDW_UPDATENOW | RDW_ALLCHILDREN);
    UpdateWindow(pane->host);
}

static LRESULT CALLBACK PaneWndProc(HWND hWnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    Pane* pane = (Pane*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

    switch (message) {
        case WM_NCCREATE: {
            pane = new Pane();
            pane->hwnd = hWnd;
            pane->dpi = GetDpiForWindow(hWnd);
            if (!pane->dpi) {
                pane->dpi = 96;
            }
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pane);
            break;
        }

        case WM_PAINT: {
            if (pane) {
                PAINTSTRUCT paint;
                HDC dc = BeginPaint(hWnd, &paint);
                RECT client;
                GetClientRect(hWnd, &client);
                PaintPane(pane, dc, client);
                EndPaint(hWnd, &paint);
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            if (!pane) {
                break;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int hot = LinkFromPoint(pane, point);
            if (hot != pane->hotLink) {
                pane->hotLink = hot;
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            if (!pane->tracking) {
                TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, hWnd, 0};
                TrackMouseEvent(&track);
                pane->tracking = true;
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (pane) {
                pane->tracking = false;
                if (pane->hotLink != -1) {
                    pane->hotLink = -1;
                    InvalidateRect(hWnd, nullptr, TRUE);
                }
            }
            return 0;
        }

        case WM_SETCURSOR: {
            if (pane && pane->hotLink != -1) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONUP: {
            if (!pane) {
                break;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index = LinkFromPoint(pane, point);
            if (index >= 0 && (size_t)index < g_settings.links.size()) {
                FollowLink(pane, g_settings.links[index]);
            }
            return 0;
        }

        case WM_TIMER: {
            if (pane && wParam == kSpacerTimer) {
                KillTimer(hWnd, kSpacerTimer);
                OnSyncSpacer(pane);
                return 0;
            }
            if (pane && wParam == kRefreshTimer) {
                KillTimer(hWnd, kRefreshTimer);
                RefreshPane(pane);
                return 0;
            }
            break;
        }

        case WM_DPICHANGED_AFTERPARENT: {
            if (pane) {
                pane->dpi = GetDpiForWindow(hWnd);
                if (!pane->dpi) {
                    pane->dpi = 96;
                }
                FreeFonts(pane);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE: {
            if (pane) {
                FreeFonts(pane);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            break;
        }

        case WM_PANE_CLOSE: {
            // The spacer is part of the view's layout and stays when the mod
            // goes; it is collapsed so that no empty band is left in its place.
            if (pane && pane->host) {
                if (void* spacer = FindSpacer(pane->host)) {
                    g_dui.SetWidth(spacer, 0);
                }
            }
            DestroyWindow(hWnd);
            return 0;
        }

        case WM_PANE_SETTINGS: {
            if (pane) {
                FreeFonts(pane);
                ApplyViewBorder(pane->defView);
                RefreshPane(pane);
                LayOutPane(hWnd);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_PANE_SPACER: {
            if (pane) {
                SyncSpacer(pane);
            }
            return 0;
        }

        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> guard(g_panesMutex);
                std::erase(g_panes, hWnd);
            }
            if (pane) {
                if (pane->icon) {
                    DestroyIcon(pane->icon);
                }
                FreeFonts(pane);
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
                delete pane;
            }
            break;
        }
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

static ATOM g_paneClass;

static bool RegisterPaneClass() {
    WNDCLASSEXW wc = {sizeof(wc)};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = PaneWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kPaneClassName;
    g_paneClass = RegisterClassExW(&wc);
    return g_paneClass != 0;
}

// The gap the spacer opens up: everything between the file list and whatever
// sits on the other side of it, which is the folder tree when it is open.
static bool GetPaneRect(HWND host, HWND defView, HWND self, RECT* result) {
    RECT hostClient;
    RECT viewRect;
    if (!GetClientRect(host, &hostClient) || !GetWindowRect(defView, &viewRect)) {
        return false;
    }

    MapWindowPoints(nullptr, host, (POINT*)&viewRect, 2);

    UINT dpi = GetDpiForWindow(host);
    if (!dpi) {
        dpi = 96;
    }
    int paneWidth = MulDiv(g_settings.paneWidth, dpi, 96);

    // In a window too narrow for everything, DirectUI squeezes the file list
    // out - hides it. The position of a hidden list means nothing then.
    HWND viewWindow = GetHostChildOf(host, defView);
    bool viewShown = viewWindow && IsShown(viewWindow) && IsShown(defView) &&
                     viewRect.right > viewRect.left;

    // No file list yet - a window that is still being built - or none any more
    // in a window too narrow for it. Either way the room next to it is not the
    // pane's: in the first case the folder tree is not there yet and the pane
    // would cover the whole window, in the second the tree takes it.
    if (!viewShown) {
        return false;
    }

    // The pane has given way to the file list, see PaneFits.
    Pane* ownPane = (Pane*)GetWindowLongPtrW(self, GWLP_USERDATA);
    if (ownPane && !ownPane->suppressed && g_dui.ok &&
        !PaneFits(ownPane, viewWindow)) {
        return false;
    }

    // With the border on, the pane runs a couple of pixels into the file list,
    // over the edge of the frame the list draws for itself: that is what makes
    // the two frames read as one around the pane and the list together.
    int overlap = 0;
    if (g_settings.paneBorder && !g_settings.removeViewBorder) {
        overlap = GetSystemMetricsForDpi(SM_CXEDGE, dpi);
    }

    // The spacer is what makes the room, so the pane is anchored to the file
    // list and given the width the spacer was asked for, rather than taking
    // everything up to the next window - the folder tree of a freshly opened
    // view is not always there yet when the pane is first placed.
    RECT rect = viewRect;
    if (g_settings.onRight) {
        rect.left = viewRect.right;
        rect.right = std::min(hostClient.right, viewRect.right + paneWidth);
    } else {
        rect.right = viewRect.left;
        rect.left = std::max(hostClient.left, viewRect.left - paneWidth);
    }

    // Anything else in the host - the folder tree, the details pane - takes
    // its own share of that space first. The nearest edges on either side:
    int leftEdge = INT_MIN;
    int rightEdge = INT_MAX;
    for (HWND child = GetWindow(host, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        if (child == self || child == viewWindow || !IsShown(child)) {
            continue;
        }

        RECT childRect;
        if (!GetWindowRect(child, &childRect)) {
            continue;
        }
        MapWindowPoints(nullptr, host, (POINT*)&childRect, 2);

        if (childRect.right <= childRect.left) {
            continue;
        }

        if (childRect.right <= rect.right && childRect.left < rect.left) {
            leftEdge = std::max(leftEdge, (int)childRect.right);
        } else if (childRect.left >= rect.left &&
                   childRect.right > rect.right) {
            rightEdge = std::min(rightEdge, (int)childRect.left);
        } else if (childRect.right > rect.left && childRect.left < rect.right) {
            // Inside the room altogether: keep to the side it leans to.
            if (g_settings.onRight) {
                rightEdge = std::min(rightEdge, (int)childRect.left);
            } else {
                leftEdge = std::max(leftEdge, (int)childRect.right);
            }
        }
    }

    // Between the folder tree and the spacer there is the splitter, which is
    // not a window. While there is room for the whole pane, DirectUI puts the
    // spacer right after it, and the gap seen then is its width. When the
    // pane has to be narrower than asked for, it starts after the splitter too,
    // not right at the edge of the tree - that would cover the splitter.
    Pane* pane = ownPane;
    int gap = MulDiv(3, dpi, 96);
    if (pane && pane->splitterGap >= 0 && pane->splitterGapDpi == (int)dpi) {
        gap = pane->splitterGap;
    }

    if (leftEdge != INT_MIN) {
        if (!g_settings.onRight && rect.left >= leftEdge) {
            int seen = rect.left - leftEdge;
            if (pane && seen <= MulDiv(16, dpi, 96)) {
                pane->splitterGap = seen;
                pane->splitterGapDpi = dpi;
            }
        } else if (rect.left < leftEdge + gap) {
            rect.left = leftEdge + gap;
        }
    }

    if (rightEdge != INT_MAX && rect.right > rightEdge) {
        rect.right = rightEdge;
    }

    if (rect.right <= rect.left || rect.bottom <= rect.top) {
        return false;
    }

    if (g_settings.onRight) {
        rect.left -= overlap;
    } else {
        rect.right += overlap;
    }

    *result = rect;
    return true;
}

static HWND FindPaneOfHost(HWND host);

static bool IsSubclassed(HWND hWnd, WindhawkUtils::WH_SUBCLASSPROC proc) {
    std::lock_guard<std::mutex> guard(g_subclassesMutex);
    for (const Subclass& subclass : g_subclasses) {
        if (subclass.hWnd == hWnd && subclass.proc == proc) {
            return true;
        }
    }
    return false;
}

// The other windows in the host - the folder tree above all. DirectUI moves,
// resizes and hides them without touching the folder view: once the file list
// has been squeezed out of a narrow window, narrowing it further only shrinks
// the folder tree. The pane has to follow that too, or it stays where it was,
// over the splitter and eventually over the tree.
static LRESULT CALLBACK SiblingSubclassProc(HWND hWnd,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR refData) {
    switch (message) {
        case WM_WINDOWPOSCHANGING: {
            // While the spacer collapses or comes back, the file list is moved
            // without carrying its old picture along: carried along, the old
            // picture - scroll bar and all - is left showing next to the new
            // one until the list repaints. See OnSyncSpacer.
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            if (pos && g_spacerTransitionHost &&
                g_spacerTransitionHost == GetParent(hWnd)) {
                pos->flags |= SWP_NOCOPYBITS;
            }
            break;
        }

        case WM_PAINT:
        case WM_NCPAINT:
        case WM_ERASEBKGND:
            if (g_spacerTransitionHost &&
                g_spacerTransitionHost == GetParent(hWnd)) {
                return 0;
            }
            break;

        case WM_WINDOWPOSCHANGED: {
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            constexpr UINT kStill = SWP_NOMOVE | SWP_NOSIZE;
            if (pos && (pos->flags & (kStill | SWP_SHOWWINDOW | SWP_HIDEWINDOW)) !=
                           kStill) {
                HWND paneWindow = FindPaneOfHost(GetParent(hWnd));
                if (paneWindow) {
                    LayOutPane(paneWindow);
                }
            }
            break;
        }

        case WM_NCDESTROY:
            DropSubclass(hWnd, SiblingSubclassProc);
            break;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

// The windows inside the file list's host - the view, the list and its column
// headers - paint nothing while the spacer changes its width either (the
// headers repaint themselves as soon as they are moved), and are painted in
// one go afterwards. See OnSyncSpacer.
static LRESULT CALLBACK HoldSubclassProc(HWND hWnd,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         DWORD_PTR refData) {
    switch (message) {
        case WM_PAINT:
        case WM_NCPAINT:
        case WM_ERASEBKGND:
            if (g_spacerTransitionHost && hWnd != g_spacerTransitionPainting &&
                IsChild(g_spacerTransitionHost, hWnd)) {
                return 0;
            }
            break;

        case WM_NCDESTROY:
            DropSubclass(hWnd, HoldSubclassProc);
            break;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

static BOOL CALLBACK HoldChildProc(HWND hWnd, LPARAM) {
    if (!IsSubclassed(hWnd, HoldSubclassProc)) {
        AddSubclass(hWnd, HoldSubclassProc, 0);
    }
    return TRUE;
}

static void WatchSiblings(HWND host, HWND self) {
    for (HWND child = GetWindow(host, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        if (child != self && !IsSubclassed(child, SiblingSubclassProc)) {
            AddSubclass(child, SiblingSubclassProc, 0);
        }
    }
}

static void LayOutPane(HWND paneWindow) {
    Pane* pane = (Pane*)GetWindowLongPtrW(paneWindow, GWLP_USERDATA);
    if (!pane || !pane->host || !pane->defView || !IsWindow(pane->defView)) {
        return;
    }

    if (pane->suppressed) {
        ShowWindow(paneWindow, SW_HIDE);
        return;
    }

    // Windows come and go with navigation, so this runs every time.
    WatchSiblings(pane->host, paneWindow);

    // OnSyncSpacer lays the pane out itself once the spacer has its new width.
    if (pane->spacerTransition) {
        return;
    }

    SyncSpacer(pane);

    if (g_dui.ok && !pane->spacerReady) {
        ShowWindow(paneWindow, SW_HIDE);
        return;
    }

    // Until the spacer has followed, the pane stays as it is: shown where it
    // was when it is about to give way - it is hidden only once the file list
    // has taken its place - and hidden when it is about to come back, until
    // there is room for it.
    if (g_dui.ok && !pane->suppressed) {
        HWND viewWindow = GetHostChildOf(pane->host, pane->defView);
        bool fits = PaneFits(pane, viewWindow);
        if (!fits && !pane->spacerCollapsed && IsShown(paneWindow)) {
            return;
        }
        if (fits && pane->spacerCollapsed) {
            ShowWindow(paneWindow, SW_HIDE);
            return;
        }
    }

    RECT rect;
    if (!GetPaneRect(pane->host, pane->defView, paneWindow, &rect)) {
        ShowWindow(paneWindow, SW_HIDE);
        return;
    }

    // Above the file list, so that the strip of the pane that lies over the
    // left edge of its frame actually covers it.
    SetWindowPos(paneWindow, HWND_TOP, rect.left, rect.top,
                 rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

// -----------------------------------------------------------------------------
// Finding the folder view
// -----------------------------------------------------------------------------

static HWND FindPaneOfHost(HWND host) {
    for (HWND child = GetWindow(host, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        if (IsClassName(child, kPaneClassName)) {
            return child;
        }
    }
    return nullptr;
}

// The pane of a folder view, kept on the view itself so that the subclass can
// find it whichever thread put it there.
constexpr PCWSTR kPaneProperty = L"ClassicWebViewPane_Pane";

static void AttachToDefViewOnItsThread(HWND defView);

LRESULT CALLBACK DefViewSubclassProc(HWND hWnd,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR data) {
    HWND paneWindow = (HWND)GetPropW(hWnd, kPaneProperty);

    if (message == g_adoptMessage) {
        AttachToDefViewOnItsThread(hWnd);
        return 0;
    }

    switch (message) {

        case WM_NOTIFY: {
            NMHDR* header = (NMHDR*)lParam;
            if (header &&
                (header->code == (UINT)LVN_ITEMCHANGED ||
                 header->code == (UINT)LVN_ODSTATECHANGED ||
                 header->code == (UINT)LVN_DELETEALLITEMS ||
                 header->code == (UINT)LVN_INSERTITEM)) {
                if (IsWindow(paneWindow)) {
                    // Coalesce - a rubber band selection reports every item.
                    SetTimer(paneWindow, kRefreshTimer, 80, nullptr);
                }
            }
            break;
        }

        case WM_WINDOWPOSCHANGED: {
            // A folder view is created under the tab window and only put into
            // the DirectUI host afterwards, so the pane cannot be built when
            // the view appears - this is the first moment it can be.
            if (!IsWindow(paneWindow)) {
                AttachToDefViewOnItsThread(hWnd);
                paneWindow = (HWND)GetPropW(hWnd, kPaneProperty);
            }
            if (IsWindow(paneWindow)) {
                ApplyViewBorder(hWnd);
                LayOutPane(paneWindow);
            }
            break;
        }

        case WM_NCDESTROY: {
            RemovePropW(hWnd, kPaneProperty);
            DropSubclass(hWnd, DefViewSubclassProc);
            break;
        }
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

static void AttachToDefViewOnItsThread(HWND defView) {
    if (g_unloading) {
        return;
    }

    HWND host = GetParent(defView);
    while (host && !IsClassName(host, L"DirectUIHWND")) {
        host = GetParent(host);
    }
    if (!host) {
        return;
    }

    HWND paneWindow = FindPaneOfHost(host);
    if (!paneWindow) {
        paneWindow = CreateWindowExW(
            0, kPaneClassName, nullptr,
            WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, host, nullptr,
            GetModuleHandleW(nullptr), nullptr);
        if (!paneWindow) {
            Wh_Log(L"The pane window could not be created");
            return;
        }

        std::lock_guard<std::mutex> guard(g_panesMutex);
        g_panes.push_back(paneWindow);
    }

    Pane* pane = (Pane*)GetWindowLongPtrW(paneWindow, GWLP_USERDATA);
    if (!pane) {
        return;
    }

    if (pane->defView != defView) {
        // Navigation can reuse the host and pane with a new, not yet laid
        // out view. Its spacer must be checked before reusing a visible pane.
        pane->spacerReady = false;
        pane->spacerCollapsed = true;
        pane->spacerRetries = kSpacerRetries;
        ShowWindow(paneWindow, SW_HIDE);
    }
    pane->host = host;
    pane->defView = defView;
    SetPropW(defView, kPaneProperty, paneWindow);

    // Decided here as well as in RefreshPane, so a Control Panel window never
    // shows the pane for the moment before the first refresh runs.
    pane->suppressed =
        g_settings.skipControlPanel && IsControlPanelFolder(defView);

    ApplyViewBorder(defView);
    LayOutPane(paneWindow);

    // The view has only just been created; it needs a moment before it can
    // answer for its folder.
    SetTimer(paneWindow, kRefreshTimer, 120, nullptr);
}

// The window may belong to another thread - the mod is loaded into Explorer
// windows that are already open, and every Explorer window runs on one of its
// own. A window has to be created by the thread that pumps its parent, so the
// work is handed over to the view itself.
static void AttachToDefView(HWND defView) {
    if (!AddSubclass(defView, DefViewSubclassProc, 0)) {
        return;
    }

    if (GetWindowThreadProcessId(defView, nullptr) == GetCurrentThreadId()) {
        AttachToDefViewOnItsThread(defView);
    } else {
        PostMessageW(defView, g_adoptMessage, 0, 0);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle,
                                 PCWSTR className,
                                 PCWSTR windowName,
                                 DWORD style,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 HWND parent,
                                 HMENU menu,
                                 HINSTANCE instance,
                                 PVOID param) {
    HWND result = CreateWindowExW_Original(exStyle, className, windowName, style,
                                           x, y, width, height, parent, menu,
                                           instance, param);
    if (result && IsClassName(result, L"SHELLDLL_DefView")) {
        AttachToDefView(result);
    }

    return result;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void AdoptOpenWindows() {
    // Windows that were already open when the mod was loaded. Their layout has
    // no spacer in it, so the pane stays hidden until they are reopened, but
    // picking them up costs nothing and keeps the state consistent.
    struct Enumerator {
        // EnumChildWindows already walks every descendant.
        static BOOL CALLBACK Child(HWND hWnd, LPARAM param) {
            if (IsClassName(hWnd, L"SHELLDLL_DefView")) {
                AttachToDefView(hWnd);
            }
            return TRUE;
        }

        static BOOL CALLBACK Top(HWND hWnd, LPARAM param) {
            DWORD processId = 0;
            GetWindowThreadProcessId(hWnd, &processId);
            if (processId == GetCurrentProcessId()) {
                EnumChildWindows(hWnd, Child, param);
            }
            return TRUE;
        }
    };

    EnumWindows(Enumerator::Top, 0);
}

static void ClosePanes() {
    std::vector<HWND> panes;
    {
        std::lock_guard<std::mutex> guard(g_panesMutex);
        panes = g_panes;
    }

    // A plain SendMessage: the pane must be gone before the mod is, and its
    // thread never waits on this one, so there is nothing to time out for.
    for (HWND pane : panes) {
        if (IsWindow(pane)) {
            SendMessageW(pane, WM_PANE_CLOSE, 0, 0);
        }
    }
}

HMODULE g_dui70;

BOOL Wh_ModInit() {
    g_adoptMessage = RegisterWindowMessageW(L"ClassicWebViewPane_Adopt");

    g_dui70 = LoadLibraryExW(L"dui70.dll", nullptr,
                             LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dui70) {
        Wh_Log(L"dui70.dll could not be loaded");
        return FALSE;
    }

    LoadDuiApi(g_dui70);
    LoadSettings();

    // public: long __cdecl DirectUI::DUIXmlParser::SetXML(unsigned short const *,
    //     struct HINSTANCE__ *, struct HINSTANCE__ *)
    auto setXml = (SetXML_t)GetProcAddress(
        g_dui70,
        "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z");
    if (!setXml) {
        Wh_Log(L"DUIXmlParser::SetXML was not found in dui70.dll");
        FreeLibrary(g_dui70);
        g_dui70 = nullptr;
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(setXml, SetXML_Hook,
                                        &SetXML_Original) ||
        !WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        FreeLibrary(g_dui70);
        g_dui70 = nullptr;
        return FALSE;
    }

    // Last, so that no failure above can leave the class registered: without
    // Wh_ModUninit to unregister it, the next load could not register it again.
    if (!RegisterPaneClass()) {
        Wh_Log(L"The pane window class could not be registered");
        FreeLibrary(g_dui70);
        g_dui70 = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    AdoptOpenWindows();
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    std::vector<HWND> panes;
    {
        std::lock_guard<std::mutex> guard(g_panesMutex);
        panes = g_panes;
    }

    if (!g_settings.removeViewBorder) {
        RestoreViewBorders();
    }

    // Everything else - fonts, the border, the contents and the layout - is
    // redone by each pane on its own thread.
    for (HWND paneWindow : panes) {
        if (IsWindow(paneWindow)) {
            PostMessageW(paneWindow, WM_PANE_SETTINGS, 0, 0);
        }
    }
}

void Wh_ModUninit() {
    g_unloading = true;

    // A window still carrying a subclass of the mod, or a window whose window
    // procedure is in it, calls into a DLL that is about to be gone.
    ClosePanes();
    DropAllSubclasses();
    RestoreViewBorders();

    {
        std::lock_guard<std::mutex> guard(g_imageMutex);
        FreeImage();
    }

    if (g_paneClass) {
        UnregisterClassW(kPaneClassName, GetModuleHandleW(nullptr));
        g_paneClass = 0;
    }

    if (g_dui70) {
        FreeLibrary(g_dui70);
        g_dui70 = nullptr;
    }
}
