// ==WindhawkMod==
// @id              classic-webview-pane
// @name            Classic WebView Pane
// @description     Restores Explorer WebView with Windows 2000/98/Me layouts, file thumbnails, image viewing, metadata, classic disk charts and special folder actions
// @name:ru         Панель WebView как в Windows 2000
// @description:ru  Возвращает WebView с компоновкой Windows 2000/98/Me, миниатюрами, просмотром изображений, метаданными, диаграммами дисков и действиями специальных папок
// @version         2.17
// @author          appEW
// @license         MIT
// @github          https://github.com/appEW
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -lole32 -lshlwapi -luuid -lwindowscodecs -loleaut32 -lpropsys -lshell32 -luxtheme -lwinspool
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- classicLayout: true
  $name: Original WebView layout
  $name:ru: Исходная компоновка WebView
  $description: Use the reference DLL layout. Turn off to use the older custom appearance controls.
  $description:ru: Использовать компоновку DLL-эталона. Отключите для прежних настроек собственного оформления.
- visualProfile: '2000'
  $name: WebView style
  $name:ru: Стиль WebView
  $options:
  - '2000': Windows 2000
  - '98': Windows 98
  - me: Windows Me
- language: auto
  $name: Pane language
  $name:ru: Язык панели
  $options:
  - auto: Windows display language
  - en: English
  - ru: Русский
  $options:ru:
  - auto: Язык интерфейса Windows
  - en: English
  - ru: Русский
- autoPictureWidth: true
  $name: Set built-in picture width automatically
  $name:ru: Автоматическая ширина встроенных картинок
  $description: Draw clouds with width 0 (automatic) and squares with width 127 (the DLL's 127 x 56 image). Overrides the manual width without changing its saved value; also works in portable Windhawk.
  $description:ru: Рисовать облака с шириной 0 (автоматически), квадраты с шириной 127 (изображение 127 x 56, как в DLL). Применяется вместо ручной ширины без изменения её сохранённого значения; работает и в переносном Windhawk.
- filePreview: true
  $name: File thumbnails
  $name:ru: Миниатюры файлов
- imageViewer: true
  $name: Pictures folder image viewer
  $name:ru: Просмотр изображений в папках рисунков
- imageZoom: true
  $name: Image zoom and panning
  $name:ru: Масштабирование и перемещение изображения
- detachedPreview: true
  $name: Separate image preview window
  $name:ru: Отдельное окно просмотра изображения
- imagePrint: true
  $name: Image printing command
  $name:ru: Команда печати изображения
- fileMetadata: true
  $name: File dates, size and document metadata
  $name:ru: Даты, размер и метаданные файлов
- fileAttributes: true
  $name: File attributes
  $name:ru: Атрибуты файлов
- multiSelectionInfo: true
  $name: Multiple selection details
  $name:ru: Сведения о множественном выделении
- specialFolderTemplates: true
  $name: Special folder templates
  $name:ru: Шаблоны специальных папок
- recycleBinActions: true
  $name: Recycle Bin actions
  $name:ru: Действия с корзиной
- contentBarricades: true
  $name: Protected folder Show Files / Hide Contents
  $name:ru: Показать файлы / скрыть содержимое защищённых папок
- win98MiniBanner: true
  $name: Windows 98 narrow window banner
  $name:ru: Полоса Windows 98 в узком окне
- livePrinterInfo: true
  $name: Refresh printer information
  $name:ru: Обновление сведений о принтерах
- paneTooltips: true
  $name: Link tooltips and status text
  $name:ru: Подсказки ссылок и текст состояния
- imageViewerExecutable: ''
  $name: Program for the Separate Window button (empty uses built-in viewer)
  $name:ru: Программа для кнопки «Отдельное окно» (пусто — встроенный просмотр)
  $description: Full path to your image viewer's executable. Environment variables such as %ProgramFiles% are supported. Used when Separate image preview window is enabled.
  $description:ru: Полный путь к exe выбранной программы просмотра изображений. Поддерживаются переменные вроде %ProgramFiles%. Работает при включённом отдельном окне просмотра.
- imageViewerArguments: '%1'
  $name: Image viewer arguments
  $name:ru: Параметры программы просмотра
  $description: The selected file replaces %1 with automatic quoting. If %1 is absent, the file is appended. Example -new-window %1. Empty passes only the file.
  $description:ru: Вместо %1 передаётся выбранный файл с автоматическими кавычками. Если %1 нет, файл добавляется в конец. Пример -new-window %1. Пустое поле передаёт только файл.
- scrollImageDetails: true
  $name: Scroll image viewer details when space is insufficient
  $name:ru: Прокручивать сведения при нехватке места в просмотре изображений
  $description: Add horizontal and vertical scrollbars to the upper details area. The preview and its controls stay in the lower half. Requires Original WebView layout.
  $description:ru: Добавлять горизонтальную и вертикальную прокрутку верхней области сведений. Предпросмотр и его кнопки остаются в нижней половине. Требуется исходная компоновка WebView.
- compactImageHeader: false
  $name: Compact image viewer header (Windows 2000)
  $name:ru: Компактная шапка просмотра изображений (Windows 2000)
  $description: Hide the corner artwork and folder icon while the image viewer is active, keeping the folder title and divider. Turn off to retain the full header. Requires Original WebView layout.
  $description:ru: Убирать верхнюю картинку и значок папки при активном просмотре изображений, сохраняя название папки и разделитель. Отключите для полной шапки. Требуется исходная компоновка WebView.
- disableFontSmoothing: false
  $name: Disable pane font smoothing
  $name:ru: Отключить сглаживание шрифтов панели
- captions:
  - - original: 'Zoom In'
      $name: Original English caption
      $name:ru: Исходная английская подпись
    - text: ''
      $name: Your caption (empty means automatic)
      $name:ru: Ваша подпись (пусто — автоматически)
  $name: Custom text captions
  $name:ru: Собственные текстовые подписи
  $description: Replace any original English caption, including descriptions and toolbar labels. Keep format placeholders in descriptions.
  $description:ru: Заменить любую исходную английскую подпись, в том числе описания и кнопки. Сохраняйте обозначения формата в описаниях.

- paneWidth: 200
  $name: Pane width
  $name:ru: Ширина панели
  $description: "Applies only when Original WebView layout is off. Width of the pane in pixels, at 100% scaling. Windows 2000 used about 200."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Ширина панели в пикселях при масштабе 100%. В Windows 2000 она была около 200."
- minListWidth: 200
  $name: Room kept for the file list
  $name:ru: Место для списка файлов
  $description: "Applies only when Original WebView layout is off. When the window gets so narrow that the file list would be left with less than this many pixels (at 100% scaling), the pane gives way and the list gets its room, as in Windows 2000. 0 keeps the pane at any width."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Когда окно становится настолько узким, что списку файлов осталось бы меньше этого числа пикселей (при масштабе 100%), панель уступает место списку, как в Windows 2000. 0 - панель остаётся при любой ширине."
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
  $description: "Applies only when Original WebView layout is off. Size of the icon in the header, in pixels at 100% scaling."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Размер значка в шапке, в пикселях при масштабе 100%."
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
  $description: "Applies only when Original WebView layout is off. Use the selection colour supplied by the current Windows theme, or the custom colour below."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Использовать цвет выделения, установленный текущей темой Windows, либо заданный ниже собственный цвет."
  $options:
  - theme: Windows theme
  - custom: Custom colour
  $options:ru:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorBar: '#000080'
  $name: Custom colour of the chart
  $name:ru: Собственный цвет диаграммы
  $description: "Applies only when Original WebView layout is off. Used only when \"Custom colour\" is selected above."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Используется только при выборе «Собственный цвет» выше."
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
  $description: "In the original layout, only the labels of the three default links (My Documents, My Computer, My Network Places) are used. Custom targets and extra links apply when Original WebView layout is off."
  $description:ru: "В исходной компоновке используются только подписи трёх стандартных ссылок («Мои документы», «Мой компьютер», «Сетевое окружение»). Собственные адреса и дополнительные ссылки действуют при отключённой Исходной компоновке WebView."
- underlineLinks: true
  $name: Underline the links
  $name:ru: Подчёркивать ссылки
  $description: "Applies only when Original WebView layout is off. Keep the links underlined the way the web view of Windows 2000 did, rather than only under the pointer."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Держать ссылки подчёркнутыми, как в веб-виде Windows 2000, а не только под курсором."
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
  - style: Original artwork of the selected WebView style
  - win2000: Windows 2000 clouds
  - squares: Windows 2000 coloured squares
  - file: The picture file below
  - none: No picture
  $options:ru:
  - style: Исходная картинка выбранного стиля WebView
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
  $description: "Applies only when Original WebView layout is off. Windows 2000 had it in the top left corner of the pane, with the icon and the name drawn over it."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. В Windows 2000 она была в левом верхнем углу панели, а значок и имя рисовались поверх неё."
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
  $description: "Applies only when Original WebView layout is off. Off keeps the pixels as they are, which is what a picture out of Windows 2000 wants. On smooths them, which suits a photograph."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Выключено - пиксели остаются как есть, что и нужно картинке из Windows 2000. Включено - сглаживать, что подходит фотографии."
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
  $description: "Applies only when Original WebView layout is off. Size of the folder or item name in points, at 100% scaling. Windows 2000 set it well above the rest of the pane. 0 keeps the size of the other text."
  $description:ru: "Действует только при отключённой Исходной компоновке WebView. Размер имени папки или объекта в пунктах при масштабе 100%. В Windows 2000 оно заметно крупнее остального текста в панели. 0 - как остальной текст."
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
  $description: "Hide the pane on the pages of registered Control Panel items, such as Programs and Features, to avoid overlapping their task column. On by default. The Control Panel folder itself keeps its WebView template either way."
  $description:ru: "Скрывать панель на страницах зарегистрированных элементов панели управления, например «Программы и компоненты», чтобы не перекрывать их колонку задач. По умолчанию включено. Шаблон самой папки панели управления доступен при любом положении переключателя."
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

Restores the Explorer WebView with Windows 2000, Windows 98 and Windows Me
layouts. The folder icon and title stay above the divider; the selected item's
information appears below it. Drives show capacity, used/free space and the
classic three-dimensional chart.

Version 2.17 adds file thumbnails and the Pictures/imgview image viewer, with
zoom, pan, actual size, best fit, a separate preview window and the Shell's
printing command. It also adds document metadata, file attributes, multiple
selection details, special-folder descriptions and actions, printer updates,
Show Files/Hide Contents panels and the Windows 98 banner for narrow windows.
Each added feature has its own switch in the mod settings.

Choose **Original WebView layout** for the reference layout and one of the
three **WebView styles**. Disabling the reference layout restores the previous
appearance settings. Ordinary files use Shell thumbnails; Pictures folders and
folders with an imgview template use the image viewer. Thumbnails and decoding
run in the background. Unsupported files show the standard preview status.

The **Program for the Separate Window button** setting accepts the full path to
your image viewer's `.exe`. Leave it empty to use the built-in separate window.
**Image viewer arguments** accepts optional flags and `%1` for the selected
image. The image path is quoted automatically; if `%1` is omitted, it is added
at the end. Environment variables such as `%ProgramFiles%` work in the program
path. These settings require **Separate image preview window** to be enabled.

**Scroll image viewer details when space is insufficient** keeps long names
and metadata accessible with horizontal and vertical scrollbars in the upper
half. The image controls and preview stay below it. Disable it for clipped
details without scrollbars. **Compact image viewer header (Windows 2000)**
optionally hides the corner artwork and folder icon while this viewer is
active, retaining the title and divider. It is off by default.

**Pane language** defaults to the Windows display language: Russian on Russian
Windows, English otherwise. It can also be chosen manually. In **Custom text
captions**, enter an original English caption and your replacement. For example:
`Zoom In`, `Zoom Out`, `Actual Size`, `Best Fit`, `Full Screen`, `Print`,
`Properties`, `Capacity:` or `Select an item to view its description.`.
An empty replacement keeps the automatic text. For formatted descriptions,
keep the original placeholders such as `%ld`; incompatible replacements are
ignored. File and folder names continue to come from Explorer.

**Set built-in picture width automatically** sets **Picture width** to **0 for
clouds** and **127 for squares**. Automatic width overrides the manual drawing
width while preserving the manual value in settings.
The square artwork is 127 x 56 pixels, as in the reference DLL. Disable this
switch to enter a width manually. Custom pictures keep their chosen width.
The Picture option for the selected style uses that style's original artwork.

The native layout, templates, image controls and embedded assets were adapted
from the [matching ClassicExplorer WebView source](https://github.com/arceuss/ClassicExplorer/tree/f3b4d865d60d8f57b548a986b43b218875ec1d60/Win2KWebView).
The adapted WebView code is included under MIT with its author's permission.
The mod draws its own child pane and reserves space through Explorer's DirectUI
layout. A patched shell32 WebView should be disabled to avoid duplicate panes.

## Examples from the previous custom layout

![This PC](https://raw.githubusercontent.com/appEW/images/main/classic-webview-pane/my-computer.png)

![Drive capacity](https://raw.githubusercontent.com/appEW/images/main/classic-webview-pane/drive.png)

![Recycle Bin](https://raw.githubusercontent.com/appEW/images/main/classic-webview-pane/recycle-bin.png)

## По-русски

Возвращает WebView с компоновкой Windows 2000, Windows 98 и Windows Me. Значок
и заголовок папки остаются над разделителем, сведения о выбранном объекте
показываются ниже. Для дисков отображаются ёмкость, занятое/свободное место
и классическая объёмная диаграмма.

В 2.17 добавлены миниатюры файлов и просмотр изображений в папках рисунков:
масштабирование, перемещение, исходный размер, вписывание, отдельное окно и
команда печати Shell. Также добавлены метаданные, атрибуты, множественное
выделение, шаблоны специальных папок, действия с корзиной, обновление сведений
о принтерах, «Показать файлы»/«Скрыть содержимое» и полоса Windows 98 для узких
окон. У каждой добавленной функции есть отдельный переключатель в настройках.

Включите **Исходную компоновку WebView** и выберите **Стиль WebView**. При
отключении исходной компоновки работают прежние настройки оформления.
В обычных папках используются миниатюры Shell; в папках рисунков и папках
с шаблоном imgview — просмотр изображений. Загрузка выполняется в фоне.

В **Программе для кнопки «Отдельное окно»** укажите полный путь к `.exe`
просмотрщика. Пустое поле сохраняет встроенное отдельное окно.
**Аргументы программы просмотра** принимают параметры и `%1` для выбранного
изображения. Путь к изображению автоматически заключается в кавычки; без `%1`
он добавляется в конец. В пути программы можно использовать `%ProgramFiles%`
и другие переменные окружения. Включите **Отдельное окно просмотра изображения**.

**Прокручивать сведения при нехватке места в просмотре изображений** добавляет
горизонтальную и вертикальную прокрутку верхней половины для длинных имён и
метаданных. Кнопки и изображение остаются ниже. Отключение оставляет обрезанные
сведения без полос прокрутки. **Компактная шапка просмотра изображений (Windows
2000)** опционально убирает картинку и значок папки при активном просмотре,
сохраняя название и разделитель. По умолчанию шапка остаётся полной.

**Язык панели** автоматически следует языку интерфейса Windows: русский
для русской Windows, английский для остальных. Язык можно выбрать вручную.
В **Собственных текстовых подписях** укажите исходную английскую фразу и
свой вариант, например `Zoom In`, `Actual Size`, `Best Fit`, `Full Screen`,
`Print`, `Properties` или `Capacity:`. Пустая замена сохраняет автоматический
текст. В описаниях с обозначениями формата сохраняйте `%ld` и аналогичные
обозначения; несовместимая замена игнорируется. Имена файлов/папок берутся из Shell.

**Автоматическая ширина встроенных картинок** задаёт **Picture width = 0
для облаков**, **127 для квадратов**. Автоматическая ширина применяется при
отрисовке; ручное значение в настройках сохраняется.
Размер квадратов в DLL-эталоне — 127 x 56 пикселей. Для ручной ширины отключите
этот переключатель. Ширина собственной картинки сохраняется.
Вариант картинки «Исходная картинка выбранного стиля WebView» использует
аутентичное оформление соответствующего стиля.

> Tested on Windows 11 24H2 (26100). Other Windows versions have not been tested.
> Проверено на Windows 11 24H2 (26100). Другие версии Windows не проверялись.
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

#include <windows.h>
#include <commctrl.h>
#include <exdisp.h>
#include <shldisp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <olectl.h>
#include <propsys.h>
#include <propkey.h>
#include <propvarutil.h>
#include <strsafe.h>
#include <wincodec.h>
#include <winspool.h>
#include <uxtheme.h>
#include <windowsx.h>
#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cwctype>
#include <limits>
#include <memory>
#include <map>
#include <mutex>
#include <new>
#include <string>
#include <vector>
#include <utility>
#include <functional>

// Standalone COM ownership adapters: the mod does not depend on ATL or on the
// reference DLL at runtime. Interfaces always remain in their owning apartment.
namespace ce { namespace win2kwebview {
template<class A,class B> constexpr auto min(A a,B b) { return a<b ? a:b; }
template<class A,class B> constexpr auto max(A a,B b) { return a>b ? a:b; }
template<class T> class CComPtr {
public:
    T* p = nullptr;
    CComPtr() = default;
    CComPtr(T* v) : p(v) { if (p) p->AddRef(); }
    CComPtr(const CComPtr& v) : CComPtr(v.p) {}
    CComPtr(CComPtr&& v) noexcept : p(std::exchange(v.p, nullptr)) {}
    ~CComPtr() { Release(); }
    CComPtr& operator=(T* v) { if (v) v->AddRef(); Release(); p=v; return *this; }
    CComPtr& operator=(const CComPtr& v) { return operator=(v.p); }
    CComPtr& operator=(CComPtr&& v) noexcept {
        if (this != std::addressof(v)) { Release(); p=std::exchange(v.p,nullptr); }
        return *this;
    }
    T* operator->() const { return p; }
    operator T*() const { return p; }
    T** operator&() { Release(); return &p; }
    void Release() { T* old=std::exchange(p,nullptr); if (old) old->Release(); }
    void Attach(T* v) { Release(); p=v; }
    T* Detach() { return std::exchange(p,nullptr); }
    HRESULT CoCreateInstance(REFCLSID clsid) {
        Release(); return ::CoCreateInstance(clsid,nullptr,CLSCTX_INPROC_SERVER,
                                             __uuidof(T),reinterpret_cast<void**>(&p));
    }
    template<class U> HRESULT QueryInterface(U** result) const {
        return p ? p->QueryInterface(__uuidof(U),reinterpret_cast<void**>(result)) : E_POINTER;
    }
    HRESULT QueryInterface(REFIID iid, void** result) const {
        return p ? p->QueryInterface(iid,result) : E_POINTER;
    }
};
template<class T> class CComQIPtr : public CComPtr<T> {
public:
    CComQIPtr() = default;
    CComQIPtr(IUnknown* v) { operator=(v); }
    CComQIPtr& operator=(IUnknown* v) {
        this->Release();
        if (v) v->QueryInterface(__uuidof(T),reinterpret_cast<void**>(&this->p));
        return *this;
    }
};
class CComBSTR {
    BSTR p=nullptr;
public:
    CComBSTR() = default;
    CComBSTR(PCWSTR text) : p(SysAllocString(text)) {}
    CComBSTR(const CComBSTR&) = delete;
    ~CComBSTR() { SysFreeString(p); }
    operator BSTR() const { return p; }
    BSTR* operator&() { SysFreeString(p); p=nullptr; return &p; }
    UINT Length() const { return SysStringLen(p); }
};
struct CComVariant : VARIANT {
    CComVariant() { VariantInit(this); }
    CComVariant(LONG value) : CComVariant() { vt=VT_I4; lVal=value; }
    CComVariant(IDispatch* value) : CComVariant() {
        vt=VT_DISPATCH; pdispVal=value; if (value) value->AddRef();
    }
    CComVariant(PCWSTR value) : CComVariant() { vt=VT_BSTR; bstrVal=SysAllocString(value); }
    CComVariant(const CComVariant&) = delete;
    ~CComVariant() { VariantClear(this); }
    CComVariant& operator=(IDispatch* value) {
        VariantClear(this); vt=VT_DISPATCH; pdispVal=value;
        if (value) value->AddRef(); return *this;
    }
};

// Reference evidence logging has no UI behavior. Keep its fluent fields local
// rather than deploying the ClassicExplorer logger and its registry settings.
struct JsonFields {
    template<class T> JsonFields& Num(PCWSTR,T) { return *this; }
    template<class T> JsonFields& Bool(PCWSTR,T) { return *this; }
    template<class T> JsonFields& Str(PCWSTR,const T&) { return *this; }
    template<class T> JsonFields& Ptr(PCWSTR,T) { return *this; }
    template<class T> JsonFields& Hex(PCWSTR,T) { return *this; }
    template<class T> JsonFields& Hresult(PCWSTR,T) { return *this; }
};
struct EvidenceLog {
    static EvidenceLog& Instance() { static EvidenceLog log; return log; }
    void Write(PCWSTR, unsigned long long, const JsonFields&) {}
};
using NavigationGeneration = unsigned long long;

inline HINSTANCE ReferenceModule() {
    HMODULE module=nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<PCWSTR>(&ReferenceModule),&module);
    return module;
}
inline BOOL ReferenceIconFont(HWND window,LOGFONTW& font) {
    using ForDpi=BOOL(WINAPI*)(UINT,UINT,PVOID,UINT,UINT);
    const auto forDpi=reinterpret_cast<ForDpi>(GetProcAddress(GetModuleHandleW(L"user32.dll"),"SystemParametersInfoForDpi"));
    const UINT dpi=GetDpiForWindow(window);
    return forDpi ? forDpi(SPI_GETICONTITLELOGFONT,sizeof(font),&font,0,dpi ? dpi : 96) :
        SystemParametersInfoW(SPI_GETICONTITLELOGFONT,sizeof(font),&font,0);
}
inline std::vector<BYTE> ReferenceDecodeBase64(const char* text) {
    std::vector<BYTE> bytes;
    unsigned value=0;
    int bits=0;
    for (; *text; ++text) {
        unsigned char c=static_cast<unsigned char>(*text);
        int n=c>='A'&&c<='Z' ? c-'A' : c>='a'&&c<='z' ? c-'a'+26 :
              c>='0'&&c<='9' ? c-'0'+52 : c=='+' ? 62 : c=='/' ? 63 : -1;
        if (n<0) continue;
        value=(value<<6)|static_cast<unsigned>(n); bits+=6;
        if (bits>=8) { bits-=8; bytes.push_back(static_cast<BYTE>(value>>bits)); }
    }
    return bytes;
}
std::vector<BYTE> ReferenceAssetBytes(UINT id);
HCURSOR ReferenceCursor(UINT id);
HICON ReferenceIcon(UINT id);
HMENU ReferenceMenu();
} }

namespace ce { namespace win2kwebview {
static bool WindowHasClass(HWND window,PCWSTR expected) {
    wchar_t name[256]{};
    return window && GetClassNameW(window,name,ARRAYSIZE(name)) && wcscmp(name,expected)==0;
}
static bool IsExplorerFolderView(HWND window) {
    if(!IsWindow(window) || !WindowHasClass(window,L"SHELLDLL_DefView")) return false;
    // Desktop/tray views also use SHELLDLL_DefView. Only browser frames are
    // eligible; neither the desktop surface nor arbitrary Shell dialogs are.
    HWND root=GetAncestor(window,GA_ROOT);
    if(!WindowHasClass(root,L"CabinetWClass") && !WindowHasClass(root,L"ExploreWClass")) return false;
    for(HWND parent=GetParent(window);parent;parent=GetParent(parent)) {
        if(WindowHasClass(parent,L"Progman") || WindowHasClass(parent,L"WorkerW") ||
           WindowHasClass(parent,L"Shell_TrayWnd") || WindowHasClass(parent,L"Shell_SecondaryTrayWnd")) return false;
    }
    return true;
}
static bool ShellViewMatchesWindow(IShellView* view,HWND expected) {
    HWND actual=nullptr;
    return view && IsWindow(expected) && SUCCEEDED(view->GetWindow(&actual)) && actual &&
           (actual==expected || IsChild(expected,actual));
}
static IShellItem* FolderItemFromView(IShellView* view) {
    CComQIPtr<IFolderView> folderView(view);
    CComPtr<IPersistFolder2> folder;
    CComPtr<IShellItem> item;
    PIDLIST_ABSOLUTE pidl=nullptr;
    if(folderView && SUCCEEDED(folderView->GetFolder(IID_PPV_ARGS(&folder))) && folder &&
       SUCCEEDED(folder->GetCurFolder(&pidl)) && pidl) {
        SHCreateItemFromIDList(pidl,IID_PPV_ARGS(&item));
    }
    CoTaskMemFree(pidl);
    return item.Detach();
}
} }

// Readers hold one complete appearance snapshot throughout a callback. Only
// the loading thread builds a mutable unpublished copy. The thread-local value
// is a plain pointer; its lifetime is owned by the callback's stack scope, so no
// DLL-owned destructor is registered for Explorer threads at thread exit.
static int EffectivePictureWidth(const std::wstring& path,int profile,bool automatic,int manual) {
    if(!automatic) return manual;
    if(path==L"*win2000-clouds") return 0;
    if(path==L"*win2000-squares") return 127;
    if(path.starts_with(L"*profile")) return profile==1 ? 0 : profile==2 ? 150 : 127;
    return manual;
}
template<class T> class LegacySettingSnapshots {
    std::mutex mutex;
    std::shared_ptr<T> value=std::make_shared<T>();
    static inline thread_local T* active=nullptr;
    static inline T fallback{};
public:
    std::shared_ptr<T> load() { std::lock_guard lock(mutex); return value; }
    void store(std::shared_ptr<T> next) { std::lock_guard lock(mutex); value=std::move(next); }
    static T& current() { return active ? *active : fallback; }
    class Scope {
        std::shared_ptr<T> owner;
        T* previous;
    public:
        explicit Scope(std::shared_ptr<T> next) : owner(std::move(next)),previous(active) { active=owner.get(); }
        Scope(const Scope&)=delete;
        ~Scope() { active=previous; }
    };
};

// Immutable options are published once per settings change. Worker jobs capture
// their own values; they never read Windhawk settings or touch pane objects.
struct WebViewOptions {
    bool classicLayout=true, preview=true, imageViewer=true, zoom=true;
    bool scrollImageDetails=true, compactImageHeader=false;
    bool detached=true, print=true, metadata=true, attributes=true;
    bool multiSelection=true, specialFolders=true, recycleActions=true;
    bool barricades=true, miniBanner=true, printerRefresh=true, tooltips=true;
    bool disableSmoothing=false, autoPictureWidth=true;
    bool itemType=true,driveSpace=true,folderDescription=true,seeAlso=true;
    int profile=0; // 0=2000, 1=98, 2=Me
    std::wstring language=L"auto";
    std::wstring viewerExecutable,viewerArguments;
    std::map<std::wstring,std::wstring> captions;
};
class WebViewOptionSnapshot {
    std::mutex mutex;
    std::shared_ptr<const WebViewOptions> value=std::make_shared<const WebViewOptions>();
public:
    std::shared_ptr<const WebViewOptions> load() {
        std::lock_guard lock(mutex); return value;
    }
    void store(std::shared_ptr<const WebViewOptions> next) {
        std::lock_guard lock(mutex); value=std::move(next);
    }
};
static WebViewOptionSnapshot g_webOptions;
static bool WebViewRussian() {
    const auto options=g_webOptions.load();
    return options->language==L"ru" || (options->language!=L"en" &&
        PRIMARYLANGID(GetUserDefaultUILanguage())==LANG_RUSSIAN);
}
namespace ce { namespace win2kwebview {
struct CaptionTranslation { PCWSTR english; PCWSTR russian; };
// Generated from captions.json. File names and shell property values are never
// passed through this table, only the template's own literal text.
static const CaptionTranslation kCaptionTranslations[]={
    {L"Used:",L"Занято:"},
    {L"Used: ",L"Занято: "},
    {L"Free:",L"Свободно:"},
    {L"Free: ",L"Свободно: "},
    {L"Capacity:",L"Емкость:"},
    {L"Capacity: ",L"Емкость: "},
    {L"Total Size: ",L"Общий размер: "},
    {L"Total File Size: ",L"Общий размер файлов: "},
    {L"See also:",L"Перейти к:"},
    {L"Select an item to view its description.",L"Выберите объект для просмотра его описания."},
    {L"My Documents",L"Мои документы"},
    {L"My Computer",L"Мой компьютер"},
    {L"My Network Places",L"Сетевое окружение"},
    {L"Network and Dial-up Connections",L"Сеть и удалённый доступ к сети"},
    {L"Dial-Up Networking",L"Удалённый доступ к сети"},
    {L"My Documents contains your personal documents.",L"В папке «Мои документы» находятся ваши личные документы."},
    {L"My Computer contains your various local drive and mapped network drives.",L"В папке «Мой компьютер» находятся локальные и подключённые сетевые диски."},
    {L"My Computer contains your various local drives and mapped network drives.",L"В папке «Мой компьютер» находятся локальные и подключённые сетевые диски."},
    {L"My Network Places contains shortcuts to various locations on the corporate network and the Internet.",L"В папке «Сетевое окружение» находятся ссылки на ресурсы локальной сети и Интернета."},
    {L"Connects to other computers, networks and the Internet",L"Подключение к другим компьютерам, сетям и Интернету"},
    {L"Connects to other computers, networks, and the Internet",L"Подключение к другим компьютерам, сетям и Интернету"},
    {L"Displays the contents of your computer",L"Отображение содержимого компьютера"},
    {L"Displays the files and folders on your computer",L"Отображение файлов и папок на компьютере"},
    {L"Stores and manages documents",L"Хранение и управление документами"},
    {L"Displays the contents of the drive.",L"Отображение содержимого диска."},
    {L"Use the settings in Control Panel to personalize your computer.",L"Используйте панель управления для настройки компьютера."},
    {L"Use the links below to manage or access items stored on your local disk.",L"Используйте ссылки ниже для управления объектами на локальном диске."},
    {L"Use the links below to search for things within your corporation.",L"Используйте ссылки ниже для поиска ресурсов в вашей организации."},
    {L"Use this folder to open files and folders on other computers and to install network printers.",L"Используйте эту папку для доступа к файлам других компьютеров и установки сетевых принтеров."},
    {L"Modified: ",L"Изменён: "},
    {L"Size: ",L"Размер: "},
    {L"Attributes: ",L"Атрибуты: "},
    {L"Author: ",L"Автор: "},
    {L"Original Location:",L"Исходное расположение:"},
    {L"Date Deleted:",L"Дата удаления:"},
    {L"Original Location",L"Исходное расположение"},
    {L"Date Deleted",L"Дата удаления"},
    {L"Attributes",L"Атрибуты"},
    {L"Author",L"Автор"},
    {L"Title",L"Название"},
    {L"Subject",L"Тема"},
    {L"Category",L"Категория"},
    {L"Comments",L"Комментарии"},
    {L"Comment",L"Комментарий"},
    {L"Dimensions",L"Размеры"},
    {L"Modified",L"Изменён"},
    {L"Date modified",L"Дата изменения"},
    {L"Created",L"Создан"},
    {L"Accessed",L"Открыт"},
    {L"Owner",L"Владелец"},
    {L"Size",L"Размер"},
    {L"Type",L"Тип"},
    {L"Item type",L"Тип элемента"},
    {L"Read-only",L"Только чтение"},
    {L"Hidden",L"Скрытый"},
    {L"System",L"Системный"},
    {L"Archive",L"Архивный"},
    {L"Compressed",L"Сжатый"},
    {L"Encrypted",L"Зашифрованный"},
    {L"(normal)",L"(обычный)"},
    {L"Used Space",L"Занято"},
    {L"Free Space",L"Свободно"},
    {L"%ld items selected.",L"Выбрано объектов: %ld."},
    {L"%ld objects selected.",L"Выбрано объектов: %ld."},
    {L"bytes",L"байт"},
    {L"No file selected.",L"Файл не выбран."},
    {L"No preview available.",L"Предпросмотр недоступен."},
    {L"Multiple files selected.",L"Выбрано несколько файлов."},
    {L"Loading preview...",L"Загрузка предпросмотра..."},
    {L"Preview",L"Предпросмотр"},
    {L"Zoom In",L"Увеличить"},
    {L"Zoom Out",L"Уменьшить"},
    {L"Actual Size",L"Исходный размер"},
    {L"Best Fit",L"Подогнать"},
    {L"Full Screen",L"Отдельное окно"},
    {L"Print",L"Печать"},
    {L"Image Preview",L"Просмотр изображения"},
    {L"Empty Recycle Bin",L"Очистить корзину"},
    {L"Restore All",L"Восстановить всё"},
    {L"Restore",L"Восстановить"},
    {L"There are no items in the Recycle Bin.",L"В корзине нет объектов."},
    {L"This folder contains files and folders that you have deleted from your computer.",L"Эта папка содержит файлы и папки, удалённые с компьютера."},
    {L"This folder contains files and folders that you have deleted from ",L"Эта папка содержит файлы и папки, удалённые с "},
    {L"your computer.",L"вашего компьютера."},
    {L"To permanently remove all items and reclaim disk space, click:",L"Чтобы удалить все объекты и освободить место на диске, нажмите:"},
    {L"To permanently remove all items and reclaim disk space, click ",L"Чтобы удалить все объекты и освободить место на диске, нажмите "},
    {L"To move all items back to their original locations, click:",L"Чтобы вернуть все объекты в исходное расположение, нажмите:"},
    {L"To move all items back to their original locations, click ",L"Чтобы вернуть все объекты в исходное расположение, нажмите "},
    {L"To move these items back to their original locations, click:",L"Чтобы вернуть выбранные объекты в исходное расположение, нажмите:"},
    {L"To move this item back to its original location, click:",L"Чтобы вернуть этот объект в исходное расположение, нажмите:"},
    {L" these items back to their original locations.",L" эти объекты в исходное расположение."},
    {L" this item back to its original location.",L" этот объект в исходное расположение."},
    {L"To view the contents of this folder, click ",L"Чтобы увидеть содержимое папки, нажмите "},
    {L"Show Files",L"Показать файлы"},
    {L"Hide the contents of this drive",L"Скрыть содержимое диска"},
    {L"View the entire contents of this drive",L"Показать всё содержимое диска"},
    {L"entire contents",L"всё содержимое"},
    {L"You may also view the ",L"Вы также можете просмотреть "},
    {L" of the network.",L" сети."},
    {L"Warning",L"Предупреждение"},
    {L"Modifying the contents of this folder may cause your programs to stop ",L"Изменение содержимого этой папки может привести к тому, что программы перестанут "},
    {L"working correctly.",L"работать правильно."},
    {L"To add or remove programs, click Start, point to Settings, click ",L"Чтобы установить или удалить программы, нажмите «Пуск», выберите «Настройка», затем "},
    {L"Control Panel, and then click Add/Remove Programs.",L"«Панель управления» и «Установка и удаление программ»."},
    {L"Add/Remove Programs",L"Установка и удаление программ"},
    {L"Installs and removes programs and Windows components.",L"Установка и удаление программ и компонентов Windows."},
    {L"Search for files or folders",L"Поиск файлов и папок"},
    {L"Search for Files or Folders",L"Поиск файлов и папок"},
    {L"Search for computers",L"Поиск компьютеров"},
    {L"Search for printers",L"Поиск принтеров"},
    {L"Search for people",L"Поиск людей"},
    {L"Finds files and folders based on the criteria you specify.",L"Поиск файлов и папок по указанным условиям."},
    {L"Network Identification",L"Сетевая идентификация"},
    {L"Add Network",L"Добавить сеть"},
    {L"Technical Support",L"Техническая поддержка"},
    {L"Windows 2000 Support",L"Поддержка Windows 2000"},
    {L"Windows Update",L"Обновление Windows"},
    {L"Microsoft Home",L"Сайт Microsoft"},
    {L"Get More Info",L"Дополнительные сведения"},
    {L"Status",L"Состояние"},
    {L"Model",L"Модель"},
    {L"Location",L"Расположение"},
    {L"Waiting Time",L"Время ожидания"},
    {L"Ready",L"Готов"},
    {L"longer than 8 hours",L"более 8 часов"},
    {L"about ",L"около "},
    {L" hour(s)",L" ч"},
    {L" minute(s)",L" мин"},
    {L" Support",L" — поддержка"},
    {L"manufacturer",L"производитель"},
    {L"printer",L"принтер"},
    {L"Add Printer",L"Установка принтера"},
    {L"The Add Printer wizard walks you step-by-step through installing a ",L"Мастер установки принтера поможет вам установить "},
    {L"To install a new printer, click ",L"Чтобы установить новый принтер, нажмите "},
    {L"To install a new printer, click the ",L"Чтобы установить новый принтер, выберите "},
    {L" to start the Add Printer wizard.",L", чтобы запустить мастер установки принтера."},
    {L" to start the wizard.",L", чтобы запустить мастер."},
    {L" icon.",L"."},
    {L"icon.",L"значок."},
    {L"This folder contains information about printers that are currently installed, and a wizard to help you install new printers.",L"Эта папка содержит сведения об установленных принтерах и мастер установки новых принтеров."},
    {L"This folder contains information about your current printers and a ",L"Эта папка содержит сведения об установленных принтерах и "},
    {L"wizard to help you install new ones.",L"мастер установки новых принтеров."},
    {L"To get information about a printer that is currently installed, right-click the printer's icon.",L"Чтобы получить сведения об установленном принтере, щёлкните его значок правой кнопкой мыши."},
    {L"To get information about a printer that's already installed, ",L"Чтобы получить сведения об установленном принтере, "},
    {L"right-click the printer's icon.",L"щёлкните его значок правой кнопкой мыши."},
    {L"Connections",L"Подключения"},
    {L" Connection",L" — подключение"},
    {L"Connect",L"Подключить"},
    {L"Make New Connection",L"Создать новое подключение"},
    {L"Make New Connection.",L"Создать новое подключение."},
    {L"To create a new connection, click",L"Чтобы создать новое подключение, нажмите"},
    {L"The Network Connection wizard helps you create a new connection so that your computer can have access to other computers and networks.",L"Мастер сетевых подключений поможет создать подключение для доступа к другим компьютерам и сетям."},
    {L"This folder contains information about your dial-up networking ",L"Эта папка содержит сведения о ваших "},
    {L"connections, and a wizard to help you make a new connection.",L"подключениях и мастер создания нового подключения."},
    {L"This folder contains network",L"Эта папка содержит сетевые"},
    {L"connections for this computer, and a",L"подключения этого компьютера и"},
    {L"wizard to help you create a new",L"мастер создания нового"},
    {L"connection.",L"подключения."},
    {L"To make a new connection, click ",L"Чтобы создать новое подключение, нажмите "},
    {L"The Make New Connection wizard walks you step-by-step through adding ",L"Мастер создания подключения поможет добавить "},
    {L"Dial-Up Networking connections. Just follow the instructions on each ",L"подключение удалённого доступа. Следуйте указаниям на каждом "},
    {L"screen.",L"экране."},
    {L"To open a connection, click its icon.",L"Чтобы открыть подключение, щёлкните его значок."},
    {L"To get information about a connection, right-click the connection's ",L"Чтобы получить сведения о подключении, щёлкните правой кнопкой мыши его "},
    {L"To establish a dial-up connection to this network now, click ",L"Чтобы подключиться к этой сети сейчас, нажмите "},
    {L"This folder contains links to all the computers in your workgroup and ",L"Эта папка содержит ссылки на компьютеры вашей рабочей группы и "},
    {L"on the entire network.",L"всей сети."},
    {L"To identify your computer on the",L"Для идентификации компьютера в"},
    {L"network, click ",L"сети нажмите "},
    {L"To add additional networking",L"Чтобы добавить сетевые"},
    {L"components, click ",L"компоненты, нажмите "},
    {L"Components",L"Компоненты"},
    {L"To access settings and components of",L"Чтобы открыть настройки и компоненты"},
    {L"a connection, right-click its icon and",L"подключения, щёлкните его значок правой кнопкой мыши и"},
    {L"then click Properties.",L"выберите «Свойства»."},
    {L"To set up networking on your",L"Чтобы настроить сеть на вашем"},
    {L"computer, click ",L"компьютере, нажмите "},
    {L"To install a network printer from this folder, locate the printer in ",L"Чтобы установить сетевой принтер из этой папки, найдите его в "},
    {L"Network Neighborhood, right-click its icon, and then click Install.",L"сетевом окружении, щёлкните его правой кнопкой мыши и выберите «Установить»."},
    {L"To see the shared resources available on a specific computer, just ",L"Чтобы увидеть общие ресурсы компьютера, "},
    {L"click the computer icon.",L"щёлкните значок компьютера."},
    {L"Loading...",L"Загрузка..."},
    {L"Properties",L"Свойства"},
    {L"Hide Contents",L"Скрыть содержимое"},
    {L"Generating preview...",L"Создание предпросмотра..."},
    {L"Stores and manages pictures.",L"Хранит изображения и управляет ими."},
    {L"Unable to start the image viewer. Check its executable path and arguments.",L"Не удалось запустить программу просмотра. Проверьте путь к программе и её параметры."},
    {L"Unable to open the separate preview window.",L"Не удалось открыть отдельное окно просмотра."},
    {L"Dimensions: %dx%d pixels",L"Размеры: %dx%d пикселей"},
    {L"The Add Printer wizard walks you step-by-step through installing a printer. Just follow the instructions on each screen.",L"Мастер установки принтера поможет установить принтер. Следуйте указаниям на каждом экране."},
    {L"The Add Printer wizard gives you step-by-step instructions for installing a printer. To install a new printer, double-click the Add Printer icon.",L"Мастер установки принтера поможет вам установить принтер. Чтобы установить новый принтер, дважды щёлкните значок «Установка принтера»."},
    {L"Network and Dial-up",L"Сеть и удалённый доступ"},
    {L" - Image Preview",L" — Просмотр изображения"},
    {L"This folder contains information about your current printers and a wizard to help you install new ones.",L"Эта папка содержит сведения об установленных принтерах и мастер установки новых принтеров."},
    {L"To get information about a printer that's already installed, right-click the printer's icon.",L"Чтобы получить сведения об установленном принтере, щёлкните его значок правой кнопкой мыши."},
    {L"This folder contains information about your dial-up networking connections, and a wizard to help you make a new connection.",L"Эта папка содержит сведения о ваших подключениях и мастер создания нового подключения."},
    {L"To get information about a connection, right-click the connection's icon.",L"Чтобы получить сведения о подключении, щёлкните правой кнопкой мыши его значок."},
    {L"The Make New Connection wizard walks you step-by-step through adding Dial-Up Networking connections. Just follow the instructions on each screen.",L"Мастер создания подключения поможет добавить подключение удалённого доступа. Следуйте указаниям на каждом экране."},
    {L"This folder contains links to all the computers in your workgroup and on the entire network.",L"Эта папка содержит ссылки на компьютеры вашей рабочей группы и всей сети."},
    {L"To see the shared resources available on a specific computer, just click the computer icon.",L"Чтобы увидеть общие ресурсы компьютера, щёлкните значок компьютера."},
    {L"To install a network printer from this folder, locate the printer in Network Neighborhood, right-click its icon, and then click Install.",L"Чтобы установить сетевой принтер из этой папки, найдите его в сетевом окружении, щёлкните его правой кнопкой мыши и выберите «Установить»."},
    {L"\u00A0bytes",L"\u00A0байт"},
    {L"Modifying the contents of this folder may cause your programs to stop working correctly.",L"Изменение содержимого этой папки может привести к тому, что программы перестанут работать правильно."},
    {L"To add or remove programs, click Start, point to Settings, click Control Panel, and then click Add/Remove Programs.",L"Чтобы установить или удалить программы, нажмите «Пуск», выберите «Настройка», затем «Панель управления» и «Установка и удаление программ»."},
};
static std::mutex g_captionMutex;
static std::map<std::wstring,std::wstring> g_captionStorage;
static std::vector<std::wstring> CaptionFormats(PCWSTR text) {
    std::vector<std::wstring> result;
    for (const wchar_t* p=text;*p;++p) if (*p==L'%') {
        const wchar_t* start=p;
        if (p[1]==L'%') { ++p; continue; }
        ++p;
        while (*p && !wcschr(L"diouxXfFeEgGaAcCsSpn%",*p)) ++p;
        if (*p) result.emplace_back(start,p+1); else { result.emplace_back(start); break; }
    }
    return result;
}
PCWSTR ReferenceCaption(PCWSTR text) {
    if (!text) return L"";
    const auto options=g_webOptions.load();
    std::wstring value=text;
    std::wstring suffix;
    auto custom=options->captions.find(value);
    if(custom==options->captions.end()) {
        const size_t last=value.find_last_not_of(L" \t\r\n");
        if(last!=std::wstring::npos && last+1<value.size()) {
            suffix=value.substr(last+1); custom=options->captions.find(value.substr(0,last+1));
        }
    }
    if (custom!=options->captions.end() &&
        (!wcschr(text,L'%') || CaptionFormats(text)==CaptionFormats(custom->second.c_str()))) {
        value=custom->second;
        if(!suffix.empty() && !value.empty() && !iswspace(value.back())) value+=suffix;
    }
    else if (WebViewRussian()) {
        for (const auto& entry:kCaptionTranslations) {
            if (value==entry.english) { value=entry.russian; break; }
        }
    }
    // std::map node addresses are stable. This also makes pointers returned to
    // native APIs safe across another pane's settings/locale update.
    std::lock_guard lock(g_captionMutex);
    auto [entry,inserted]=g_captionStorage.try_emplace(value,value);
    return entry->second.c_str();
}
} }
static void LoadWebViewOptions() {
    auto options=std::make_shared<WebViewOptions>();
    #define READ_OPTION(member,key) options->member=Wh_GetIntSetting(L##key)!=0
    READ_OPTION(classicLayout,"classicLayout");
    READ_OPTION(preview,"filePreview"); READ_OPTION(imageViewer,"imageViewer");
    READ_OPTION(scrollImageDetails,"scrollImageDetails"); READ_OPTION(compactImageHeader,"compactImageHeader");
    READ_OPTION(zoom,"imageZoom"); READ_OPTION(detached,"detachedPreview");
    READ_OPTION(print,"imagePrint"); READ_OPTION(metadata,"fileMetadata");
    READ_OPTION(attributes,"fileAttributes"); READ_OPTION(multiSelection,"multiSelectionInfo");
    READ_OPTION(specialFolders,"specialFolderTemplates");
    READ_OPTION(recycleActions,"recycleBinActions"); READ_OPTION(barricades,"contentBarricades");
    READ_OPTION(miniBanner,"win98MiniBanner"); READ_OPTION(printerRefresh,"livePrinterInfo");
    READ_OPTION(tooltips,"paneTooltips"); READ_OPTION(disableSmoothing,"disableFontSmoothing");
    READ_OPTION(autoPictureWidth,"autoPictureWidth");
    READ_OPTION(itemType,"showItemType"); READ_OPTION(driveSpace,"showDriveSpace");
    READ_OPTION(folderDescription,"showFolderDescription"); READ_OPTION(seeAlso,"showSeeAlso");
    #undef READ_OPTION
    auto profile=WindhawkUtils::StringSetting::make(L"visualProfile");
    options->profile=wcscmp(profile.get(),L"98")==0 ? 1 :
                     wcscmp(profile.get(),L"me")==0 ? 2 : 0;
    auto language=WindhawkUtils::StringSetting::make(L"language");
    options->language=language.get();
    auto viewer=WindhawkUtils::StringSetting::make(L"imageViewerExecutable");
    auto viewerArgs=WindhawkUtils::StringSetting::make(L"imageViewerArguments");
    options->viewerExecutable=std::wstring(viewer.get()).substr(0,16384);
    const auto viewerStart=options->viewerExecutable.find_first_not_of(L" \t\r\n");
    options->viewerExecutable=viewerStart==std::wstring::npos ? L"" :
        options->viewerExecutable.substr(viewerStart,options->viewerExecutable.find_last_not_of(L" \t\r\n")-viewerStart+1);
    options->viewerArguments=std::wstring(viewerArgs.get()).substr(0,16384);
    for (int i=0;i<64;++i) {
        auto original=WindhawkUtils::StringSetting::make(L"captions[%d].original",i);
        auto replacement=WindhawkUtils::StringSetting::make(L"captions[%d].text",i);
        if (!*original.get()) break;
        if (*replacement.get()) options->captions[original.get()]=
            std::wstring(replacement.get()).substr(0,2048);
    }
    const struct { PCWSTR key; PCWSTR original; } oldCaptions[]={
        {L"usedLabel",L"Used:"},{L"freeLabel",L"Free:"},{L"totalLabel",L"Capacity:"},
        {L"seeAlsoTitle",L"See also:"},
        {L"descriptionText",L"Select an item to view its description."}};
    for (const auto& caption:oldCaptions) {
        auto value=WindhawkUtils::StringSetting::make(caption.key);
        if (*value.get() && wcscmp(value.get(),caption.original)!=0) {
            options->captions[caption.original]=value.get();
            options->captions[std::wstring(caption.original)+L" "]=std::wstring(value.get())+L" ";
        }
    }
    for (int i=0;i<64;++i) {
        auto label=WindhawkUtils::StringSetting::make(L"seeAlso[%d].label",i);
        auto target=WindhawkUtils::StringSetting::make(L"seeAlso[%d].target",i);
        if (!*label.get() && !*target.get()) break;
        PCWSTR original=wcscmp(target.get(),L"shell:Personal")==0 ? L"My Documents" :
            wcscmp(target.get(),L"shell:NetworkPlacesFolder")==0 ? L"My Network Places" :
            wcscmp(target.get(),L"shell:MyComputerFolder")==0 ? L"My Computer" : nullptr;
        if (original && *label.get() && wcscmp(label.get(),original)!=0)
            options->captions[original]=label.get();
    }
    g_webOptions.store(std::move(options));
}


// Adapted from the matching ClassicExplorer webview source: assets.inc
namespace ce { namespace win2kwebview {
constexpr UINT IDR_WEBVIEW_2K_WVLEFT=900;
static const char kAsset900[]=
"Qk04IAAAAAAAADYEAAAoAAAAfwAAADgAAAABAAgAAAAAAAAAAAASCwAAEgsAAAAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwADA3MAA"
"8MqmAAQEBAAICAgADAwMABEREQAWFhYAHBwcACIiIgApKSkAVVVVAE1NTQBCQkIAOTk5AIB8/wBQUP8AkwDWAP/szADG1u8A1ufnAJCprQAAADMAAABmAAAA"
"mQAAAMwAADMAAAAzMwAAM2YAADOZAAAzzAAAM/8AAGYAAABmMwAAZmYAAGaZAABmzAAAZv8AAJkAAACZMwAAmWYAAJmZAACZzAAAmf8AAMwAAADMMwAAzGYA"
"AMyZAADMzAAAzP8AAP9mAAD/mQAA/8wAMwAAADMAMwAzAGYAMwCZADMAzAAzAP8AMzMAADMzMwAzM2YAMzOZADMzzAAzM/8AM2YAADNmMwAzZmYAM2aZADNm"
"zAAzZv8AM5kAADOZMwAzmWYAM5mZADOZzAAzmf8AM8wAADPMMwAzzGYAM8yZADPMzAAzzP8AM/8zADP/ZgAz/5kAM//MADP//wBmAAAAZgAzAGYAZgBmAJkA"
"ZgDMAGYA/wBmMwAAZjMzAGYzZgBmM5kAZjPMAGYz/wBmZgAAZmYzAGZmZgBmZpkAZmbMAGaZAABmmTMAZplmAGaZmQBmmcwAZpn/AGbMAABmzDMAZsyZAGbM"
"zABmzP8AZv8AAGb/MwBm/5kAZv/MAMwA/wD/AMwAmZkAAJkzmQCZAJkAmQDMAJkAAACZMzMAmQBmAJkzzACZAP8AmWYAAJlmMwCZM2YAmWaZAJlmzACZM/8A"
"mZkzAJmZZgCZmZkAmZnMAJmZ/wCZzAAAmcwzAGbMZgCZzJkAmczMAJnM/wCZ/wAAmf8zAJnMZgCZ/5kAmf/MAJn//wDMAAAAmQAzAMwAZgDMAJkAzADMAJkz"
"AADMMzMAzDNmAMwzmQDMM8wAzDP/AMxmAADMZjMAmWZmAMxmmQDMZswAmWb/AMyZAADMmTMAzJlmAMyZmQDMmcwAzJn/AMzMAADMzDMAzMxmAMzMmQDMzMwA"
"zMz/AMz/AADM/zMAmf9mAMz/mQDM/8wAzP//AMwAMwD/AGYA/wCZAMwzAAD/MzMA/zNmAP8zmQD/M8wA/zP/AP9mAAD/ZjMAzGZmAP9mmQD/ZswAzGb/AP+Z"
"AAD/mTMA/5lmAP+ZmQD/mcwA/5n/AP/MAAD/zDMA/8xmAP/MmQD/zMwA/8z/AP//MwDM/2YA//+ZAP//zABmZv8AZv9mAGb//wD/ZmYA/2b/AP//ZgAhAKUA"
"X19fAHd3dwCGhoYAlpaWAMvLywCysrIA19fXAN3d3QDj4+MA6urqAPHx8QD4+PgA8Pv/AKSgoACAgIAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AP//"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////"
"//////////////////////////////////////////////8A////////////////////////////////////////////////////////////////////////"
"/////////////////////////////////////////////////////////////////////////////////////////////////wD/////9f/////////1////"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////"
"////////////////////////////AP/////////////1///////////1////////////////////////////////////////////////////////////////"
"//////////////////////////////////////////////////////////////////////////////8A//X19f/19f/1////////////////////////////"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////"
"/////////wD19fX19fX19fX19f/1//X////1////////////////////////////////////////////////////////////////////////////////////"
"////////////////////////////////////////////////////////////APX19fX19fX19fX19fX19fX1////////////////////////////////////"
"//////////////////////////////////////////////////////////////////////////////X/9f////////////////////////////8A9fX19fX1"
"9fX19fX19fX19fX19f//////////////////////////////////////////////////////////////////////////////////////////////////////"
"//////////X//////////////////////////////wD14vX19fX19fX19fX19fX19fX1////9v/2//b1//b/9v/2//b/9v/2//b/9v/2////////////////"
"//X19f/19fX29vb29vb29vb29vb29vb/9v/////1//b19vb29vb///b////1//X/////////////////////////////APT09PTi9fX19fX19fX19fX19fX1"
"eXp6eXp6eXp6eXp5enp5enl6eXp5enp5enl6eXp5enp5enlXcnhyeHN4lxx4mJiYmZiZCAgICAgbCBsIGwgbCBsIG8Ib8/T09PX19Pb19fX1//X1////////"
"//////////////////////8A4vTi9PT19PX14vX19fX19fX19fV6eXp5enl6enl6enl6eXp5enl6eXp5enl6enl6enl6eXp5enKXc5ccl3N4HHh4mHmYmAgI"
"CAgIGwjCCPHCGwjCGxsbG8Ib9PT0wvT1wvX19f/1//X//////////////////////////////wAZ9OL04vT04vX09fXi9fX19fX19Xl6eXp5enl6eXp5enp5"
"enp5enp5enp5enl6eXp5enp5enp5c5dzl3OYeJh4mJmYmJkICAgICBsIGwgbGwgbGxsI88LzG/P0wvT19PX19fX19vX/9f//////////////////////////"
"////ABkZ4hn04vT04vTi9fX14vXi9fX1enp5enp5enl6eXp5enl6eXp5enl6eXp5enl6eXp5enl6eXqXc5d4eHiYeJh4mHgImQgICAgbCMIb8fHCGxvCGxsb"
"Gxv0wvT09PX09PX19fX1//X2//////////////////////////////8AGRkZ4vMZ9OL09PTi9fX19fX19fV5enp5////////////////////////////////"
"////enl6epd4c3iYeJl4mAh5CAgICAgbCBsbCBvCGxsbGxsbwhvzwvMb9PT09PX09fX19fX/9f/1/////////////////////////////wAZGRkZGeIZ9OL0"
"4vT14vXi9fX1/3p5enn/////////////////////////////9f////95enl6cnh4mHiYeAh5CAgICAgIGwgbCMIbGxsbG8IbwvPzwhvzG/TC9PT09cL19fX2"
"9f/1////////////////////////////////ABkZGRniGeL04vTi9eL19fX19fX1eXp5evX/////////////////////////////9f/1/3p6eXp4eJh4mZgI"
"mQgICAgIGwgbCBsbGxvCGxsbGRsbGxsb88Lz9PT09ML19PX19fX/9v////////////////////////////////8AGRkZGRkZGRni9OL19eL19fX///96enl6"
"////////////////////////////////////eXp5enh4eJiYCAgICAgICBsI8cLxwhsZGxsbwhsb8xvCG8L0G/Qb9ML09PX09PX19fX/////////////////"
"/////////////////wAZGRkZGRniGfTi9eL19fX1/////3l6enn///////////////////////////////////96eXp5eJiYmQgICAgICBsICBsIGxsIGxvC"
"GxkbG8IbG/Mb8xvC8/P08/T09PX19fX1//X/////////////////////////////////ANwZGRkZ4hkZ4uL19eL19f//////enl6ef//////////////////"
"/////////////////3l6eXr/9f////X///////////X////19fX19fX19f/1///1//X///////////////////////////////////////////////////8A"
"GRkZGRkZGeIZ9eL19fX19f////95enl69f//////////////////////////////////eXp6ef////X////1///1//X///X19fX19fX19fX19fX1///////1"
"/////////////////////////////////////////////////wDhCeEZGRniGeL14vX19fX//////3p6eXr///////////////////////////////////V6"
"eXp5//////////////////X19fX19fX19fX19fX19fX19f/1////////////////////////////////////////////////////ABkZGRkZGRni9fXi9fX1"
"9fX/////eXqaef///////////////////////////////////3p6eXr1//////////X/9f/19fX19fX19fX19fX19fX19fX1////9f/////1////////////"
"//////////////////////////////8AGdwZGRniGRni4vXi9fX1//////WaeeV6////////////////////////////////////eXp5ev////X///X////1"
"9fX19fX19fX19fX19fX19fX19fX19fX/9f/1//////X//////////////////////////////////////wDhGRkZGRkZ4vT19fX19fX//////3oaeRr2////"
"//////////////////////////////96eXp5//////////X/9fX19fX19fX19fX19fX19fX19fX19fX///X/9fX/9f//////////////////////////////"
"////////////ABncGeEZGeLi4vX19fX1///////1oBqgmv///////////////////////////////////3l6meX2//////X///X19fX19fX19fX19fX19fX1"
"9fX19fX19fX1//X19fX19f/1//////////////////////////////////////8A4RkZGRni9PT19fX19fX///////8bmhqg9f//////////////////////"
"////////////GqCaGvb////1///19fX19fX19fX19fX19fX19fX19fX19fX19fX1//X19fX19fX1//X//////////////////////////////////wAZ3OEZ"
"GeLi4vX19fX/////////9hqgGxv///////////////////////////////////YamqAa////9f//9fX19fX19fX19fX19fX19fX19fX19fX19fX19fX19bS0"
"tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLT/////////////AOEZGRni9PX19f/19f//////////9vb0w/X//////////////////////////////////6AaoBr2"
"//////X19fX19fX19fX19fX19fX19fX19fX19fX19fX19fX1tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tP////////////8AGRkZGeLi9f//////////////"
"///////////////////////////////////////////2GqAaG/////X///X19fX19fX19fX19fX19fX19fX19fX19fX19fX19fW0tLS0tLS0tLS0tLS0tLS0"
"tLS0tLS0tLS0/////////////wAZGRni9OL19f////////////////////////////////////////////////////////8bwxug9f////X19fX19fX19fX1"
"9fX19fX19fX19fX19fX19fX19fX19bS0tPT19fX1//////////////////+0tLT/////////////ABkZ4vXi9fX/////////////////////////////////"
"////////////////////////9vTD9PT////19fX19fX19fX19fX19fX19fX19fX19fX19fX19fX19eL0tLS09OL19fX/9f///////////////7S0tP//////"
"//////8AGeLi9fX1////////////////////////////////////////////////////////////wxvDw/X/9f/19fX19fX19fX19fX19fX19fX19fX19fX1"
"9fX19fXi9PS0tLTi9PX19fX/////////////////tLS0/////////////wAZ4k0rTCtMKyxMK0wrTCtMK0wrTCsrTCv/////////////////////////////"
"//////b0wxv09f/19fX19fX19fX19fX19fX19fX19fX19fX19fX19fX14vX04rS0tPT04vX19f////////////////+0tLT/////////////ABkZK0wsTCxM"
"KyxMLCtNKyxMLCtNK00rTPb///////////////////////////////X1/xvDG6D/9fX19fX19fX19fX19fX19fX19fX19fX14vX19fX19fX19OL0tLS04vT1"
"9fX1/////////////////7S0tP////////////8AGeJNKytMK0wrTCtMK0wrTCtMK0wrK00r///////////////////////////////19fX1oBvDG/X19fX1"
"9fX19fX19fX19fX14vX19fX19fX19fX19fX19fXi9PS0tLT09fX19f/1////////////////tNu0/////////////wAZ4itMLOL19fX///////////////9M"
"K0z2///////////////////////////19fX19fUboBug9vX19fX14vX19fX19fX19fX19fXi9fXi9fX19fX19eL14vQZ4rS0tOL19fX1////////////////"
"//+0tLX/////////////ABniTStM9eL19f///////////////ytNK///////////////////////////9fX19fX19RugGhvi9fX19fX14vX14vX19fX19fX1"
"9fX19fX19fX19fX19fQZGeL0tLS09fX19fX1/////////////////7XbtP////////////8AGeIrTCzi9fX1////////////////KytM9v//////////////"
"///////////19fX19eL0GhugmvX19fX19fX19eL19fX19fX19fX19fX19fX19fX14vX04vTi9Bm0tLT19fX1////////////////////tNy0////////////"
"/wAZ4ixMK/Xi9f////////////////9MLEz////////////////////////19fX19fX19Bmgeht64vX19eL14vXi9fX14vX19fX19fX14vX19fX19fX19eL0"
"GRnz4rS0tOL19fX19f////////////////Xctdv/////////////ABn0TCtM9eL1/////////////////ytMK///////////////9v/29fb29fX/9fX19eLi"
"4np5enn04vT09fX19fX14vX19fX19fX19fX19fXi9eL19OL04vTi9OL03AkJ9fX19f///////////////////9vctfX///////////8A4uJMU0z19fX/////"
"////////////K00r//////////////b0oBrlG3rlenl6enl6eXp6eXp6eRkZGRni9fXi9eL19fX19fX19fX19fX19fX19eIZGRkZGRkZ4vQZGRn19fX/9f//"
"///////////////13LTc/////////////wD19XV0U/X1//////////////////9MK0z2//////////////QbGhp5Gnl6eXp5enl6eXp5enl6GRkZGRkZ4vX0"
"9OL19eL19fX14vXi9f/19eL0GRkZGRkZGRkZ9OL09fX19f/////////////////////c1rr1////////////AOL1dJqT////////////////////9VJNUv//"
"///////1////wxqgmuV6Gnp5enl6enl6eXp5enkZGRkZGRkZ9OL04vQZ9OL19fX08xkZ4vX19BkZGRkZGRkZ4hn04vTi9fX1////////////////////9da6"
"3P////////////8A9fUampr/////////////////////dXR19fX19fX1//X19fX1GxsaG3l6eXp5enl5enp5enl6eRkZGRkZGRkZ9OLzGRkZ9BkZGRkZGRnz"
"9OIZGRkZCRkZGRkZ9OL09fX19fX/////////////////////3NW79f///////////wD19fP08/////////////////////+TepP19fX19fTi9fX19fX14vX1"
"9fX19fX19fX0GRkZCRkZCRkZCRkJGRkZGfQZGRkZGRkZGRnxGRnxGd0ZCRkZGRkZGfPi9eL19fX1///////////////////////ctNz/////////////AOL1"
"9fX//////////////////////5oamvXi4hkZGRn19fXi9eL19fX19eL19eL19OIZGRkZGQkZGdzc4RkZGRkZGRkZeJdznXJ4nZd4nZd4nZd4cnhy4hn04vT0"
"4vX19fX//////////////////////7TctP////////////8A9fX19f////////////////////X/Ghoa9eIZGRkZGRniGeL09OL09eL19fT09OIZGRkZGQkZ"
"4dwJ4RncGQkZGRkZGfRyl3iXeJdzl3OXc5dzl3OXl3gZGeL04vX19fX19f//////////////////////tLS0/////////////wD19f//////////////////"
"////9fWavRriGRkZGRkZGRkZGRni9OL09OL04vTi9BkZGRkZGRncGRkJGQkZCRkZGRkZGZf3l3J4l3iXeJd4l3iXeJf3l/MZ9PT19eL19fX/////////////"
"//////////+03LT/////////////AP////////////////////////X19ZN5dRkZGRkZGRkJGRkZGRkZGRkZGfQZGRkZGRkZGRkJGQkZCRndGRkZGRkZGRny"
"eJd4GRkZGRkZGRkZGRkZc5eX4vTi9OL19fX19f//////GQm0tLS0tLS0tLS0tLS0tP////////////8A////////////////////////9fX1dRZ03RkZGRnh"
"CeHcGRkZGRkZGRkZGRkZGfQZGRkZ3RkZGRkZGRkZGRkZGRkZGRmXc5fzGRkZGRn0GeIZ8+KXmHL14vT19fX19fX///////8ZCbS0tLS0tLS0tLS0tLS0////"
"/////////wD/////////////9fb29v/////19f9SU1IZ4dwZ3NwZCdzh3BkJGRkZGRkZGRkZGRkZGRkZGfIZGRkZGRkZ8xkZ9BkZGXiXlxn0GeL0GfQZ9PPi"
"9HOXeOL14vX19fX/9fX//////xkJtLS0tLS0tLS0tLS0tLT/////////////AP////////////SamRZSKytMKytNKytMLOHcGeHc4dzh3BkZGeEZCRkZGRkZ"
"GRkZGRkZGRkZGRkZ8xn0GRnzGfQZ9PQZeJh44vT09PTi9PTi9fTil3hy9fX19fX19fX///////////////////////////////////////////8A////////"
"///28xp1dVJNK00rTStMLEwrGRkZ3BncGdwZGQkZCRndGRkZGRkZ9BkZGRkZ8xnzGfQZ9Bn09OL04vTi9OLCnsL04vXi9PXi9fXi9fR4cnj19fX19fX/9f//"
"/////////////////////////////////////////wD////////////zmpN1TCtMK0wrTCtMK0wZ3OEZ3BkZCRkJGRkZGRkZGRnzGfQZGfMZ8/QZ9PTi9OL0"
"9OL09PT19PX09fT19fX19fX19fX19fX19Zedl/X19fX1////////////////////////////////////////////////AP////////////////////X19fX1"
"9RkZGRkZ3BkJGQkZCBkZGRkZGRkZ4hn04hn04vPi9OL04vT09PT19fX19fX19fX19fX19fX19fX19fX19fX1eHJ49f/1////////////////////////////"
"//////////////////////8A//////////////////////X19OIZGRkZGRkZGRkZGRkZGRkZGRnzGfQZ9Bn09PT09PX09fX19fX19fX19fX19fX1//X1//X/"
"9f/1///1//X///WXeJf1/////////////////////////////////////////////////////wAAAA==";
constexpr UINT IDR_WEBVIEW_2K_WVLINE=901;
static const char kAsset901[]=
"R0lGODlhmAACAJH/AP///2aZzAAAAAAAACwAAAAAmAACAEACD4yPqcvtD6OctNqLs94cFQA7";
constexpr UINT IDR_WEBVIEW_2K_WVNET=902;
static const char kAsset902[]=
"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";
constexpr UINT IDR_WEBVIEW_2K_WVLOGO=903;
static const char kAsset903[]=
"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"
"J4XdUYxq1R6hFRQEBAA7";
constexpr UINT IDR_WEBVIEW_98_WVLEFT=910;
static const char kAsset910[]=
"Qk2OrgAAAAAAADYEAAAoAAAAtgAAAO0AAAABAAgAAAAAAFiqAAAAAAAAAAAAAAABAAAAAQAA////AP//9wD//+8A9+/WAO/etQD3794A9+fGAP/35wDv3r0A"
"79alAOfGhADvzpQA58aMAP/v1gD3584A9961AO/WrQDvzpwA78aEAOe9ewDv3sYA99alAO/GjAD33r0A79a1AOe9hAD31q0A786lAO/GlADnvYwA/+/eAPfn"
"1gD/584A//fvAPfexgDv1r0A99a1AO/OrQD3zqUA78acAO+9jAD3zq0A/+fWAP/exgD31r0A997OAP/v5wD/9/cAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALwABAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEvAAEAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAvAS8AAQAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAS8ALwAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC8BLwABAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAvLwEv"
"AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"LwEvAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAC8vLwEvAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAvLy8vAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAALy8vAS8AAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAC8vLy8BLwEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAvLy8BLwEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALy8vLy8BLwABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC8vLy8BLwEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAhLy8vLwEvAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALy8vLwEvAS8BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACEhLy8vLwEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAhLyEvLwEvLwEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAISEvLy8vLwEvAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACEhIS8vLy8vAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAhISEvIS8vAS8BAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAISEhIS8vLy8BLwEAAQAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACEhISEvIS8vLwEBAQAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAhISEhIS8vLwEvAQABAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAISEhISEhLy8vAS8BAAEAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACEhISEhIS8vLy8BLwEA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAuISEhISEvLy8B"
"LwEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABy4hISEh"
"ISEvLwEBLwEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC4H"
"LiEhISEvIS8vLwEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAuLiEuISEhIS8vLwEvAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAALi4uISEhISEvIS8vAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAC4uBy4uISEhIS8vAQEvAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAuLi4uIS4hISEhLy8vAQEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAALgUuBy4HISEhIS8vAS8BAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAC4uLi4uISEhISEvLy8BLwEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAuBS4uBwcuISEhIS8vLwEvAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABS4uBS4uIQchISEvLy8vAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAUeBS4uBy4hISEhIS8vAQEvAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAeHgUuBS4HLiEhISEvLy8vAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABQUeBS4uLgchISEhLy8vAQEvAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAUeBS4FLgcuISEhISEvLy8vAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFBQUFLgUuBy4hISEhLy8vAQEBAQAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABQUFHgUuLgcHIQchIS8vLy8vAQAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAUFBQUeBS4FLgchISEhIS8vAQEBAQAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAfBQUeBQUuLgcuISEhIS8vLy8vAQABAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABQUFBR4FLgUuBwchISEhIS8vAQEBAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAUfBR4FHgUuBS4HIQchIS8vLy8v"
"AQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAfBR8FHgUeBS4HBy4hISEh"
"IS8vAS8BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAx8FHgUFBR4FLi4H"
"ISEhIS8hLy8BAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB8DBQUFHgUF"
"LgUHLgchISEhLy8vLwEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAfHx8F"
"HgUFBQUuBQcHISEhIS8vLwEvAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"Hx8FBQUeBQUeBS4HLgchISEhLy8vAS8BAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAB8fAx8FBQUFBS4FLgcHISEhIS8vLy8BAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAfHx8fBQUFBR4FBS4FBwchISEhLy8BLwEBAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAHx8fAx8DBQUFBS4FLi4HISEhIS8vLwEvAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAA4fHx8fBR8FBR4FLgUHLiEhISEvIS8vAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAfDh8fHwMFAwUFHgUuLgcHISEhIS8hLy8BAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADh8fHx8fHwUFBQUFLi4FBwchISEhLy8BAQEBAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4fHx8fHwMfBQUFHh4FLi4HISEhISEvLy8BAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAODh8fHx8fAx8FBQUFLgUHLgUhISEhLy8BLwEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADg4fDh8fAx8DBQUFHgUuLgcHByEhISEvLwEvAQEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4ODh8fHx8fHwUfBQUeBS4HBSEhISEhLy8vAQEBAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAODg4fHx8fHwMfBQUeBR4uBQcuByEhIS8hAQEvAQEBAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADg4ODh8fHwMfAx8FBQUeBS4FBwchISEhLy8vAQEBAAEAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4tDh8OHx8fHx8DHwUFBR4FLgUHByEhISEvAS8BAQEAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAtDg4ODg4fHx8DHwMFBR4FHgUuBSEHISEhLyEvAQEBAQEA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADi0ODg4fDh8fHx8DHwUFHgUuBQcFISEhISEvAQEB"
"AQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACItDg4ODh8OHx8fHwMfBQUeBS4FLgchISEh"
"LyEvAQEBAQAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiDi0ODg4OHw4fHx8fAx8eBR4FLgcH"
"IQchISEvAS8BAQEBAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIiIOLQ4ODh8OHx8fAx8FBQUF"
"HgUuBQchISEhIS8BLwEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACIOIg4OLQ4OHw4fHx8f"
"Ax8FHgUeBQcHByEhISEvIQEvAQEBAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiIi0GLQ4ODg4f"
"Dh8fAx8DBQUeBS4uBSEHISEhIS8vAQEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIiIGLQYt"
"Dg4ODg4fHx8DHwUeBQUeBS4FIQchISEhAS8BAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACIi"
"LSItDg4ODg4OHw4fHwMfBR8FBR4FBy4hISEhIS8BAQEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAiIiIUIi0ODg4ODg4fHx8fAyoFBR4FLgUHByEhISEhLy8BAQEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAIhQiIi0OLQ4tDg4ODg4fHx8DHwUFHgUuBS4HISEhIS8CLwEvAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAACIiFCIUIg4tDg4ODg4fDh8fHx8FBQUeBS4HLiEhISEhLwEvAS8BAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAiCCIUIi0UDg4tDg4ODh8fHx8DHwUeBR4FLgcuISEhISEBAS8BLwEBAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAIhQUIhQiIi0iDg4ODg4OHw4fHwMfBR4FHgUuBy4hISEhLyEBLwEvAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAABcIIhQiIhQiDiIODg4ODg4fHx8fHwUDHgUuBS4HBSEhISEBLwEvAQEBAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIIhQiFCIiFCIOLQYODg4ODh8fHwMfHgUeBS4FByEHISEhIQEvAQEvAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgiFCIiFCIiDgYtBg4ODg4OHx8fAwMeBQUFLi4FIQchISEvLwEvAQEBAQAAAQAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgXFBciFCIUIiItBi0ODg4ODh8fHx8fBQ0FHgUuBy4hISEhIQIvLwEvAQEBAQABAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAXCAgiFCIUIhQiBi0GBg4ODg4OHx8fAyoFHgUFLgUHLiEhISEvLwEvAS8BAQABAAEAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCMXFCIUIiIiFCIGIi0GLQ4ODg4fDh8DDQUFHgUuBSEuISEhIQIvAS8BAQEBAAEA"
"AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgXIxcUIhQiFCIiIiIGIg4iDg4ODh8OAx8DHgUeBS4FByEhISEhAi8BAS8B"
"AQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIIwgIIggiIhQiIiIGIgYiDiIODg4OHw4DKgUeBR4FLgcHISEhIS8h"
"Ly8BLwEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgILAgIFxQiFCIiIiIUBiIOIg4ODg4fHwMNBR4FLgUuLiEh"
"ISEhASEBLwEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACMIIwgiCBQXIiIUIgYUIhQiIi0OLQ4ODh8fAwUFHgUu"
"BQcuByEhISEBLy8BLwEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIIwgjFwgXFCIUFyIiIiIiFAYiLQ4tDg4OHx8N"
"BQUeBS4FByEHISEhLyEBLwEvAQEBAAEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIwgYCCwICBcUIhQiIiIiFCIiIiIiDi0O"
"Dg4fAyoFBR4FLgcHISEhISEvIQEvAS8BAQEBAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABgIIxgILAgjCBQXIhciIhQUFCIi"
"IiIODg4OHx8DHgUFLgUuLgchISEhIS8hAS8BAQEBAQEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIGBgILAgjCBcIIhcUIggi"
"IiIUIiIOIg4ODg4fHwMFHgUFLgcuByEhISEhLyEBLwEBAQEBAQABAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALAQjGAgsCBcjCBcI"
"FwgiCBQUIiIiIiIGDg4ODgMfBQMFHgUuBwchISEhISEvIQEBLwEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABgjGAgkCBcj"
"CBcUFwgXFyIIFxQiFCIiBg4ODg4fAx8FHgUeLgcHBwchISEhIS8hLwEBAQEBAQEAAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYBBgY"
"IxcsCCwIFwgXCBcICAgXFCIiIiIGLQ4ODh8fAwUeBS4uBwchByEhISEhLwEvLwEvAQEBAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"GBgYGAQsCCwIFwgXCBcIFwgIFCIIIhQiBg4ODg4fAx8FBR4eLgcHByEhISEhISEvIQEvAS8BAQEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAABgYGBgjGCwILAgXFxcXFwgICBcIIhciIiIiDgYODh8DHwUFHi4uLgcHISEhISEhIS8hAS8BAS8BAQEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAYGBgYBBgXLAgsFxcXIxcIFwgIFxciFyIiBiIODg4fHwMfHgUeLgcuBwchISEhISEvIS8vAS8BLwEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAGBgYGBgYLAgkCAgXCBcIFxcIIxcUFyIiIiIGBg4ODh8fAwUeHi4eBwcHISEhISEhIS8hLyEBLwEvAQEBAQEAAQAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAABgYGBgYGCQjCBgsFxcsFwgIIwgIFyIXCCIiIiIGDg4fHx8DHgUeLgcuBwchISEhISEhISEvIQEhAQEvAQEBAQABAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAYGBgYBBgkIw8jCCwEFwgXIwgjCBcXCBcXIhcGDgYODh8fHwUeHi4eLgUHByEhISEhISEhISEhIS8hAS8BAQEBAAEAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGBgYGBgYGCMECA8XIwQsLAgjBCMXIxcXCBciIgYODg4OHwMDBR4eLgcuBQcHISEhISEhISEhISEhIS8BLwEBAQEA"
"AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAYGBgYGBgYGBgsGAQkGBcjCBgEIxcsCBcIIiIGIgYODh8fHw0FHh4eBy4HIQchByEhISEhISEhISEhIS8B"
"LwEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYGBAYGBgYJCMYBCwYGBgYLBgIGBgsCBcXFxciIgYOBg4OHwMfHgUeLh4HLgcHBwchIQchISEhISEh"
"ISEhIS8BLwEvAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEBAQGBgYGBgPGCQEGBgYJBgEGBgYIxcjCBcXFyIGBg4ODg4fAwMeHh4eHi4HBwcHBwchByEH"
"IQchByEhISEhLy8BAS8BAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAYJRgYEBgYGCQYJBgYJBgkGAQYFw8sCCwXFyIIIgYGDg4ODh8NBR4eHi4HLgcuBwcH"
"BwcHBy4HBy4hByEhISEhLy8BLwEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQEBAYEBgYECQYJBgYGBgkGBgYGBgPCCwIFxcXFyIiIg4ODg4fAw0eHh4uHgcu"
"By4HLi4HLi4HLgcHBQchISEhISEhLwEvAQEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEBAQEBAQJBgQGBgkGBAkGBgYGBgYDyQYFyMXFwgiIiIiDg4ODh8NAx4e"
"HgcuBy4HLi4HLi4HLgUuBS4HBQchISEhISEvAS8vAQEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACUQJRAQGBAYGBgkGBgQJBgaGCQYGA8YDyMkFxcIFyIiBiIODg4f"
"Hw0FHgUeBy4HLi4uLh4FLgUeBS4FLgcuByEhISEhISEvAS8BAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAbJRAQEBAaGBAYGCQQEBAaGBoYEBgkJBgkFywXFwgXIiIi"
"Bg4ODgMNKgUeHi4uHi4eHgUeHgUeBR4FHgUuBwUHByEhISEhIS8BLwEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGwklECUQEBAYEBgYGBAQEBoQEBgQJBgYDyMPFxcX"
"FxciIgYtDg4qHwMNBQUeBy4eHgUeHgUeBQUFBR4FBS4HLgcHISEhISEhLy8BLwEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABsbECUQEBAQEBAQEBAQEBAQEBAYEBgaDxgk"
"GBcPIxcXIiIiBg4ODgMqBQ0eBS4eHh4eBR4eBQMFBQMFBR4uBQcuBwchByEhISEvIQEvAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAbGyUQJRAQEBAlEBAlEBAQJRAQEBAQ"
"GBgkGCQYLBcsFxciFyIGDg4qAyoFBR4eHh4eHh8eAw0DAwMDBQUFBQUuBQcFByEHISEhIS8hAS8BAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGwkbJRAbJRAQEBslECUQJRAQ"
"ECUQEBAkGCQYJBgjFxcXFyIiBg4ODioDHw0FHh4FHgUNHw0DHwMfHwMDBR4FHi4FBwcFISEhISEhLyEBAS8BAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABsbGxsJGxAbECUQECUJ"
"ECUQEAkQECUaECQYJBgkJBcXFxcXIgYGIA4OAyoDHh4eHh4NHx8DHwMfAx8fHwMFBR4FLi4FLgcHISEhISEvAi8BAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARGxsJGxsbGxAb"
"CSUJEBsQGwkQCSUQEBAQECQYJBgkCBcXFyIiBg4gDioDHx4FHh4eDQMqHw0fDh8OHwMfAwUFHgUFLgcFIQchISEhIS8BLwEBAQEAAQAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEREbGxsb"
"GxsJGxsJGwkbGwkbCQkJJRAQGhAQGhgkGA8sFxcXIiIGDg4OKg0DHh4FDQ0fHx8ODg4ODh8fAx8DBQUFHi4FBwUhByEhISEhLwEvAQEBAQABAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABER"
"ERsbGxsbGxsbGxsbGxsbGxsbGwklEBoQEBoYJCQkDywXFxcGBg4ODiofHgUeDQMqDR8ODg4ODg4OHx8DHwMFBR4FLgUHBSEHISEhIS8vAS8BAQEBAAEAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAREScRERsRGxEbERsRGxEbGxEbGxsbCRsQEBAQGiQYJCQPFxcXFyIODiAODSoDDQMqAw4ODg4ODg4ODg4OHwMDAwUFHgUuBQcFIQchISEhIS8BLwEBAQEA"
"AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAEScRERERERERERERERERERERGxsbGxsbCSUQGhAYGiQkJBcXFyIiBg4OHyoDKgMqHyoODg4ODg4ODg4ODh8OHwMFHgUeBR4FBwUhByEhIS8CLwEv"
"AQEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAABwLHBERJxERERERERERERERERERGxEJGxsJEBAQGhgaJCQkFxciIgYODiofKg0qAx8ODg4OIgYtBiIODg4OHwMfAwMeBR4FLgUHBSEHISEh"
"LyEBAS8BAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAcHBwcJxEREREREREREScRERERGxEbGxsbGyUQGhAaGhgkFw8XIiIiDg4OKgMqAw0ODg4OIg4iBiIGIg4ODg4fHwMDBQ0FHgUHBQcH"
"ISEhISEvIS8BAQEBAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAHBwcEQsRERERJxERJxERJxEnERERGxEbGxsQJRAQGhgkJCQXFxcGIgYOHw0qAyofDg4GIgYiBiIiIiIGDg4ODh8DHwMFBQUe"
"BQcFByEhISEhIS8BLy8BAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAABwcERwnERwRJxEnJycRJxEnEREREREbGxsQGxoJGhAkECQPFxcXIgYGDg4qHyofDQ4OIgYiIiIiFyIiIgYODg4OHwMf"
"AwUeBR4FBy4HISEhISEvLy8BAQEBAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcHBwLHBwRHBEnERwRHBEnEREnERERGxsbGwkbGhAaECQaDyQXFxcGIg4gHw0fKg4ODgYiIiIXIhcXIiIiBg4O"
"Dh8OAwMfBR4FLgUHByEhISEhISEBLy8BAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHBwcHCcRJycnJycRJycnJxEnERERGxEbGxsbFRsaEBoQJCQPFxcXIg4ODh8fKg4ODgYiFyIXFxcXCBci"
"FAYODg4OHwMfAx4FHgUuLgcHISEhISEvLy8BLwEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACcnJycnJxEnEREREREREREREREREREbGxsbCSUaEBAQGhgPJBcXIgYGDg4fKgMgDg4iIhcXFxcY"
"LBcIFyIiBg4ODh8OAx8FHgUeBQcFIQchISEhISEBLwEBAQEAAQABAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARJxEnJxEnEREnERERERERERERERsbGxsbGxsJGxAaGhAkJA8PFxciDiAfKh8gDg4GBiIX"
"FxcYDw8IFxcIIgYODg4OHx8DHwUeBS4FBy4HISEhISEvLwEvAQEBAQABAAABAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAERERERERERERERERERERERERERsbGxsJGxsVGwkVCRAaGhoPFxciIgYODh8gDg4O"
"IiIXCCQYJCQYJAgXIiIGIg4OHx8DKgMeBR4FBwUHIQchISEhISEvAQEvAQEBAAEBAAEAAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABERERsRGxEbERsRGxsRGyYRGyYmGyYJJhsJGwkmCRsaEBokJA8XFwYGDg4q"
"Dg4GIiIXDyQYGhAYJBgXCCIGBg4ODg4fHwMFBR4FHgUHLgchByEhISEhLyEvAS8BAQEBAAEBAQABAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaJhsmGxsbGyYbJhsbJhsJGwkmCSYJJgkJJgkmCRsaGxokJA8XFyIG"
"Dg4ODi0iIhcXGCQQEBAQGhgPFyIXIg4ODg4fKgMfDR4FHgUeLi4HByEhISEhISEvLy8BLwEBAQEBAQEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGhAaEBAaFRoJCQkaFRAQGhAaEBoQGhoaKRAmCSYJCRoaGCQP"
"FxciDg4gDg4OBhcXGCQQEBAJEBoPGBcXIiIOBg4OHw4fDQMDHgUeBQUHLgcHByEhISEhIS8BLwEvAQEBAQEBAQEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABoaGhoaGhAaEBoaGhoaGhoaGhoaGhoQGhopCSkJJhoQ"
"GiQPDxcXBg4gDg4OIhcPECQQCRsbJRAQJBcXFyIGDg4ODg4fDgMfAwMeBR4eBQcuBwchISEhISEvIQEvAS8BLwEvAS8BAQEBAQEAAQAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAkJBokGhoaGhoaJCQkJCQkJBokJCQaGhAaEBoa"
"EBoaGhokJCwXIgYgBgYOIhckGhAbGxsbGwkaECQEFxciBg4ODg4ODh8qAw0fHgUeBS4FBwcHISEhISEhIS8hAS8BLwEvAS8BLwEBLwEBAQEAAQAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJCQkJCQkGiQkJCQkJA8kJCQkJBokGhoa"
"GhoQGhoaGhokJA8XFwYOBiAOIhcPGhsbERERERsbCSQPLBcXIiIGDg4ODg4ODh8fAx8NBR4FLh4HBwcHISEhISEhLyEvLwEvAS8BLwEvLwEBLwEBAQABAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABcPFw8XDxcPFw8XDxckJCQkJBok"
"GiQaGhoaGhoaGhokJA8PFyIiBiIOFxckGhsRCycRERsbGhAYJBcXIiIGDg4OIA4ODg4ODh8NHx4DHgUuBy4HBwchISEhISEvIS8hIS8hISEvIQEhLwEvAQEB"
"AQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAXFxcXFxcXFxcXDyQkJCQa"
"GhokGiQaJCQaGhoaJCQkJCQkFxcrBiIGIg8kFRsRCxwLHBEbCRAkJA8XFwYGBg4ODg4GDiIODg4OHx8DHgUeHi4HBy4HBwchISEhISEhIS8hISEvIS8hLwIv"
"AS8BAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIiIGIiIiIhcXJBoa"
"GhoaGhoaGhoaGhoaGhoaGiQkJCQkFxciBiIGIhckCRsRHBwcHCcRGxsaGA8PFyIXBg4ODg4ODgYGBg4ODg4fAyoFHgUuLi4HLgcHByEhISEhISEhISEhISEh"
"ISEvIS8BAS8BAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYGLQYGIhcX"
"JBoQJgkVEBUQFRAVECYJKRUQGiQkDxcXFxciIiIGFw8kGxERJxwcHBwRGxsaECQXFyIiDg4ODiAOBiIiIhciBg4ODh8DHwUeBR4FLgcuBwcHISEhISEhISEh"
"ISEhISEhIS8CLy8BLwEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOIgYG"
"IhckGhUbJhsmGyYbJhsmGxsbJgklGhokFxciIiIGIiIiIhckFRsRJxwMJxwnERsmECQkFxciDg4OIAYiFxcXFxciIiIGDg4OHwMDAx4eBQcFBwcHBwchISEh"
"ByEhIQchISEhISEhLyEBLwEvAQEBAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"BgYiFyQaFRsbERERERERERERERERERsbGiQkFxciBgYGDg4iIhcaFRsRJxwcHAwLEREbCRAkDxciBiAODiIXFyQQJCQYFxcXIgYtDg4fAx8eBQUeLh4FBy4H"
"By4hByEHBwchByEhISEhISEhLyEvAS8BAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAACIXJCQaJhsRERERERERERERJxEnEREbJhokLBciBiIODg4GIhcaFRsRESccHBwcJxERGxoaJBcXIg4GBiIXGBAQEBAYGCQIFxciBgYODh8NAw0FHh4F"
"HgUuBy4HBwcuBwcHBwcHByEHISEhISEvAi8BLwEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAkJBobJhsREScRJwsnCxwLHAscERERJhAaJBcXIiIGBgYGIhcaGhsbESccFhwcJxERGxUQGiQXFyIiIhckECUbGxsJEBAkJAQXFyIiDg4OHw0f"
"AwUeHgUeHi4HLgcHLgcuBwcHLgcHIQchISEhIS8CLwEBLwEBAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAGiYbERERHBwcHBwcHBwcHBwcERERGxUaGiQPFywXFxcXFyQaGhsRERwcFhwcJycRERsbGhAkFxcXFw8aEBsbEREbCRsQGhgkBBcXIgYt"
"Dg4fDR8DHgUeHh4FLi4uLi4uLgUuBQcuBwchISEhISEhLwEvLwEvAQEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAACYREREcHBwcFhYWFhYcHBwcCycRGyYQGiQaJCQPJBcXJCQaEBsRERwcHBwcHBERERsJGhAkDyQPJCQQCRsRERERERsJJRAQGBgP"
"CBciBg4ODiofAw0DHgUeHgUeLh4FHh4eBS4FBwcuIQchISEhIS8hLwEvAS8BAQEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAARJxwcHBwMFgwWFgwWFhwcCycRESYaFRoVGiQaJCQkJBoaJRsRJxwWHBwcJycREREbJhoaGiQkEBoQGxsRJxEREREbGxsQ"
"EBAYJAgPFyIiDg4gHx8NAwUDHgUeBR4FHgUeBQUeLi4uBwchByEhISEhLy8hAS8BLwEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHBwWDBkZGRkZGR0WFhwcHCcRERsbJgkVEBoaGhokGhoQGxERJxwcHBwcHAsREREbFQkaGhoaECYbGxEnEREREREb"
"GxsbJRsQECQELBciIgYODg4fHwMDHgMeBR4FHgUeBQUFHgUuBQcHISEhISEhISEhLyEBLwEBLwEBAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwZGRkZGRMZGRkKDBYcHCcRERsmGxsmGyYbGhoaGikbEREnHBwcHBwcHBERERERGxsVCRUVGxsbEREnERER"
"ERERGxsbCRsQGxAQJA8XIhcGDg4ODh8NHwMNHwMNAwMNAwMFBQUeBS4HLiEHISEhISEhLyEBIS8BLwEvAQEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAZGRkTGRMZExkZHRYcHBwRERERERERESYRJhsJJhsRERwcHBYWHBwcHAscERERERsmGyYbGxEREREn"
"ERERERsRGxsbGxsbGwkQGhgkLBciIgYODg4fDh8NHwMqHx8DHwMfAwUeBR4FLgcHISEhISEhISEvIS8BIQEvAS8BAQEBAQABAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGRMTGRMZGRkZGRYcHBwLJwsnCxwnERERGxEbJhERHBwcDBYMFhYcHBwcERwRERERERsRERER"
"JxEnEScRERERGxsbGxsbGwklEBAkGBcXFxcGDg4ODg4fAx8OHw4ODh8OAx8DBR4FLgcuIQchISEhISEhIS8hIS8vIS8BLwEvAQEBAAEAAQAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABMTExMTGRMZGRIMHBwcHBwcHBwcCycREREREREnHBwWDBYMFgwWHBwcCwsRJxERERER"
"EREnHCcnEScREREREREbGxsbGxsbJRAaGCQsLBciBiIGDg4ODh8ODg4ODg4ODh8fAx4FHgUHBwchISEhISEhISEhISEhISEvIQEvAQEBAQEBAQABAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATExMTExMZGRIdFhwWFhYWDBwcHBwLJxEcERwcHAwWDBYMFgwWFhwcHBwcHAsn"
"ERwRJxwcHBwnEScREScRERERERsbGxsbGxoQECQYJBcXFwYiBg4ODg4ODg4OBg4GDg4OHwMDHh4uBy4hISEhISEhISEhISEhISEhISEvASEvAS8BAQEBAAEA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAExMTExMTGRkZFhYdHRkKGRYWHBwcHBwcHBwcHBYWDBYMFgwWDBYcHBwc"
"HBwcEScLHBwcJxwnCycRJycRJxERERsbGxsbGxsQEBoQJA8XFxciIgYiBg4GDiIiBiIiBg4ODh8DHgUeLgcHByEhISEhISEhISEhISEhISEhISEvAi8BAQEB"
"AQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABMTExMTExkZGRkZGRkZGRkoDBYcHBwcHBwcFgwdDBYMFgwWDBYW"
"HBwcHBwcERwRJxwcHBwnHCccJxwnERERERERGxsbGxsbEBoQGiQkFxcXIiIiBgYGIgYiIiIiIgYGDg4fHwMeHgcHByEhISEhISEhIQcHBwchByEhISEhIS8v"
"AS8BAQEBAQABAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATExMTExkZGRkZGRMZGRkZDBYWDBYWFhYWHQoZChkdFgwW"
"DBYWFhwcHBwcCxwRCxwcHBwcHBwcHBwnHCcREREbGxsbGxsbGxoQGhgkFxcXFxciIgYiIhciFxcXFyIiIgYODh8NBR4uBwchISEhISEhBwcHBy4HByEHISEh"
"ISEhIS8BLwEBLwEBAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAExMTExkZGRkZExkZGRkZGRkdDBYdFh0ZGRkZGRkd"
"DB0WDBYMFhwcHBwcHAsRHBwcHBwcHBwcHBwcHBERERsRGxsbJhsbFRsaECQkDxcXFxciFyIiFxcIFxcIFxcIFyIGDg4fAx4eBy4HISEhBwcHLi4FLgUHLgcF"
"BwchByEhISEhIS8BLwEBLwEBAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABMTGRMZGRkTGRMZGRkZGRkZGRkZGRkZGRkZ"
"GRkZEh0WDBYMFhwcHBwcJwscHBwcHBwcHBwcFhwcHCccERsRGxsbCRsJJgkaGhokDxcPFxciIhcXFxcXFxcIJBcIFxciIg4OHw0FHi4HBwcHBy4uHh4FHgUe"
"BS4FBwcHByEHISEhISEhLy8BLwEBLwEBAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATGRMZExkTGRkZGRkZGRkZGRkZGRkT"
"GRkZGRkZGR0MDBYMFhwcHBwcHBwcHBwcHBwcHBwcFhwWHBwcERERGxsmCSYJJRAaGiQkDw8XFxcXFxcXFxcXFyQPGA8YDxcXIgYODh8DHh4uLgcuLh4FHgUF"
"HgUeBR4FLgUuBQcuByEhISEhISEhLyEBLwEvAQEBAQEAAQABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAExkZExkTGRkZGRkZGRkZGRkT"
"GRkTGRMZGRkZGRkSHRYMFhwcHBwcHBwcHBwWFhYWHBwWFhYWHBwcHBERERsmCSYQGhoaECQkDyQPDw8XFxcXFxcXFyQYGCQYJCQIFxciIg4OHw0eHh4eHgUe"
"AwMDHgMFBQUFHgUFLi4FBwcuByEHISEhISEhIS8vAS8BAQEBAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABMTExkZExkTGRkZGRkT"
"GRMTExMTExkTGRkZGRkKHR0WFhwcHBwcHBwcHBYMFhYWFhYWFhYMFgwcHAsRERsmCSkaGhoaGhoaJCQPFxcXFw8XFxcPFyQkGBoQJCQEJBcXIgYODh8qAx4D"
"HgMfAx8DHwMfBQUFBQUFHgUFLi4FBy4HISEhISEhISEhIS8vAS8BAQEBAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATExMTGRkTGRMZ"
"ExMTExMTExMTExMZGRMZGRkZGQwWDBYcHBwcHBwcHBYMKBYWFhYMKAwdFgwWHBwRERsVEBoaGhokGhoaJCQPLA8XFw8XDxcPJCQkGCQYGBgYJAgPFyIiBg4O"
"Dh8fHx8fDh8OHx8fAx8DBQMFBQUFHgUFLgUHBy4HByEHISEhISEhIS8vAi8BAS8BAQEBAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAExMTGRMZ"
"GRMTExMTExMTExMTExMTExkZGRkZGQwdFhYcHBwcHBwcHAwdHQodDB0KHQodDB0WFhwLERsJEBoaJCQkJBoaGiQPJA8sFxcXDxckJBgkGCQkGCQYJAQPFxcX"
"IgYtDg4ODg4ODg4ODg4fHx8DHwMFBQUFBQUFHgUuLgUHLgcHISEhISEhISEhIS8vAi8BAQEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABMT"
"ExkTExMTExMTExMTExMTExMTExkTGRkZGRIdFgwWFhwcHBwWDB0dGRkZGRkZGRkZGRkWDBwcEREbGhokJCQkJCQkGiQPJA8kDw8PDyQkJBAaGCQkGCQYJCQk"
"JBcXFxciBiIGBgYGIgYODg4ODh8OHwMfAx8FBQUFHgUFHgUuBQcuBQcFISEhISEhISEhIS8vAS8vAQEBAQEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAATExMTExMTExMTExMTExMTExMTExkTGRkZGRkdDBYWDBYWFhYMHRkZGRkZGRkZGRkZGRkKHRYcEREmGhokJCQsJCQkJBokJA8kJCQaJBoaGhAQEBoQGCQY"
"DxgkGBcsFxcXIhcXIiIiIgYiBgYODg4ODh8fHx8DHwMFBQUFBQUeBS4uBQcFBwUhByEHISEhISEhISEvAS8vAQEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAExMTExMTExMTExMTExMTExMTExMZGRMZGRkZDB0WDBYMKAwdGRkZExkZGRkZGRMTGRkZHRYcHBEmEBokJBckJCQkGhokJCQkGhoaGhoJGwkbCRoQ"
"EBoQJBgPJA8kDxckFxcXFwgXFxcXIiIGIg4ODg4ODh8fHwMfAwUFBQUeBQUeBS4FLgUHBSEHByEHISEhISEhISEvAS8BAQEBAQABAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAABMTExMTExMTExMTExMTExMTExkTExkZGRkZGRkKHQodGRkZGRkTGRkTGRMTExkZGRkZGQwcHBEbFRokJCQkJCQaGhoaJCQkGhoaGhsbGxsb"
"GxsbECUQGhAkJAQkDxckFyQPJAQkBBcIFwgiIgYtDg4ODg4OHw4fAx8DAwUFBQUeBR4FHgUuBQcFBwcHByEHISEhISEhISEvAS8BAQEBAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAATExMTExMTExMTExMTExMTExMTGRkTExkZGRkZGRkZGRkZGRMZExMTExMTExMTGRMZGRkWHBwRJhAaJCQkJCQaGiQkJCQaGhoaGxsb"
"ERsbGxsbGxslGxAQGhgkDyQkDyQkJBgYJCQEFwgXCCIiBiIGDg4ODg4fDh8fHwMDAwUNBR4FHgUeBS4FBy4uBy4HBwchISEhISEhISEBLwEvAQEBAAEAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAExMTExMTExMTExMTExMTExMTExkTGRkTGRkZGRkZGRkZExkTExMTExMTExMTExMZExkdFhwnEQkaGiQaJCQaGhoaJCQaGhoQ"
"GxsREREREREbGxsbGxslGhAaJBgkDyQYJBgYJBgEJAQkCBcIIiIGBgYOBg4ODg4fHwMfAx8DBR4FHgUeBR4FLi4FBwUHLgcuBwchISEhISEhISEvAQEvAQEA"
"AQAAAAAAAAAAAAAAAAAAAAAAAAAAABMTExMTExMTExMTExMTExMTExMTExkTGRMZGRkZExkTExMTExMTExMTExMTExMTExkZFhYcEREmEBUaJCQaGhoaGhoa"
"GhAaJhERJxEnEREREREREREbGyUaGhAkGCQkJBgkJBgEJBgkBBcXFxcXIiIOBg4ODg4ODh8fAx8DAw0FHgUeBR4FHgUFLgUuBQcFBwUHBwchByEhISEhISEh"
"AS8BAQEAAAAAAAAAAAAAAAAAAAAAAAAAAAATExMTExMTExMTExMTExMTExMTExMTExMTExMTGRkTExMTExMTExMTExMTExMTExkZGQwcCxERGyYQGhoaGhAa"
"CRoQGhAaGxsRJxEnEREnEScRERERERsQJRAaECQYGhgkGCQkJBgkGCQPIxcXIiIGBg4ODg4ODg4fDh8DHwMfAwUNBR4FHgUFHgUuBS4FBwUHBQcuBwcHISEh"
"ISEhLyEBLwEBAQEAAQAAAAAAAAAAAAAAAAAAAAAAExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMZGR0WHBwRESYbFRAV"
"EBUaCRoJGhoaGxsRERwnCycnEScRJxwREREbGxAlEBoQGhgaGCQPJBgkGCQEGBcXFyIGBg4ODg4ODg4ODh8DHwMfAx8DBR4FHgUeHgUeBR4FLgUuBS4HBwcH"
"IQchISEhISEvLwEvAQEAAQAAAAAAAAAAAAAAAAAAAAAAABMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMZGRkKHBwLJxEb"
"JhsaGhoaCRoaGhAaGxEREREnHCccJxwRHBwRJxERERsbEBAQGhAaJCQEJBgkGCQYJBcXFyIiIiIOBg4ODg4ODg4fHx8fAx8DAwMFDQUFHgUeBR4FHi4uBS4H"
"LgUHLgcHIQchISEhISEvAS8BAQEAAAEAAAAAAAAAAAAAAAAAAAATExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTGRkZHRYc"
"HCcRERsmGyYmGxoJGgkaJRsbEScRHBwcHCccHBwcHAsnEREbGxsbJRAaEBoQJBgkJCQYJBcXFxciIiIGIg4ODg4ODg4fHx8fAyoDHx8NAwUFHgUeBR4FHgUF"
"BS4FLgUHLgcuBwchISEhISEhIS8vAS8BAQEAAQAAAAAAAAAAAAAAAAAAExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMZExMTExMTGRkZ"
"ChYcHBERERsmGxsbGxoaGhoaGiYbERELHBwcHBwcHBwcHBwcERERGxsbGxsbEBoaJBokJA8kDxcXFxciBgYGDg4GDg4ODg4ODg4fHx8DHw0DAx4FHgUeBR4e"
"Hh4eLh4uLi4uLgcuBwcHBwchISEhISEhLy8BAQEBAQABAAAAAAAAAAAAAAAAAA==";
constexpr UINT IDR_WEBVIEW_98_WVLINE=911;
static const char kAsset911[]=
"R0lGODlhqAABAPcAAAAAAIAAAACAAICAAAAAgIAAgACAgMDAwMDcwKbK8AQEBAgICAwMDBERERYWFhwcHCIiIikpKVVVVU1NTUJCQjk5Of98gP9QUNYAk8zs"
"/+/Wxufn1q2pkDMAAGYAAJkAAMwAAAAzADMzAGYzAJkzAMwzAP8zAABmADNmAGZmAJlmAMxmAP9mAACZADOZAGaZAJmZAMyZAP+ZAADMADPMAGbMAJnMAMzM"
"AP/MAGb/AJn/AMz/AAAAMzMAM2YAM5kAM8wAM/8AMwAzMzMzM2YzM5kzM8wzM/8zMwBmMzNmM2ZmM5lmM8xmM/9mMwCZMzOZM2aZM5mZM8yZM/+ZMwDMMzPM"
"M2bMM5nMM8zMM//MMzP/M2b/M5n/M8z/M///MwAAZjMAZmYAZpkAZswAZv8AZgAzZjMzZmYzZpkzZswzZv8zZgBmZjNmZmZmZplmZsxmZgCZZjOZZmaZZpmZ"
"ZsyZZv+ZZgDMZjPMZpnMZszMZv/MZgD/ZjP/Zpn/Zsz/Zv8AzMwA/wCZmZkzmZkAmcwAmQAAmTMzmWYAmcwzmf8AmQBmmTNmmWYzmZlmmcxmmf8zmTOZmWaZ"
"mZmZmcyZmf+ZmQDMmTPMmWbMZpnMmczMmf/MmQD/mTP/mWbMmZn/mcz/mf//mQAAzDMAmWYAzJkAzMwAzAAzmTMzzGYzzJkzzMwzzP8zzABmzDNmzGZmmZlm"
"zMxmzP9mmQCZzDOZzGaZzJmZzMyZzP+ZzADMzDPMzGbMzJnMzMzMzP/MzAD/zDP/zGb/mZn/zMz/zP//zDMAzGYA/5kA/wAzzDMz/2Yz/5kz/8wz//8z/wBm"
"/zNm/2ZmzJlm/8xm//9mzACZ/zOZ/2aZ/5mZ/8yZ//+Z/wDM/zPM/2bM/5nM/8zM///M/zP//2b/zJn//8z///9mZmb/Zv//ZmZm//9m/2b//6UAIV9fX3d3"
"d4aGhpaWlsvLy7KystfX193d3ePj4+rq6vHx8fj4+P/78KCgpICAgP8AAAD/AP//AAAA//8A/wD//////ywAAAAAqAABAAAILQBNCBxIsKDBgwgJ4ljIsKHD"
"hxAjSlxopaLFixgzatyYcZrHjyBDihxJ8mNAAAA7";
constexpr UINT IDR_WEBVIEW_98_WVLOGO=912;
static const char kAsset912[]=
"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";
constexpr UINT IDR_WEBVIEW_ME_WVLEFT=920;
static const char kAsset920[]=
"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";
constexpr UINT IDR_WEBVIEW_ME_WVLINE=921;
static const char kAsset921[]=
"R0lGODlhmAACAJH/AP///2aZzAAAAAAAACwAAAAAmAACAEACD4yPqcvtD6OctNqLs94cFQA7";
constexpr UINT IDR_WEBVIEW_ME_WVLOGO=922;
static const char kAsset922[]=
"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";
constexpr UINT IDB_WIN2K_IMGVIEW_TOOLBAR=510;
static const char kAsset510[]=
"Qk02BwAAAAAAAHYAAAAoAAAAkAAAABgAAAABAAQAAAAAAMAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwACAgIAA"
"AAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AN3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3dAN3d3d3d3d3d3d3dAN3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d0AAAAAAAAAAN3d3d3d3d3d3d3d3QiA3d3d3d3d3d3d3QiA3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dB3d3d3d3d3Bw3d3d3d3d3d3d3d0IeA3d3d3d3d3d3d0IeA3d3QAAAAAAAAAAAN3d3d3d3d3Y3d"
"3d3d3d3d3d3d3d3d3d3d3dB3d3d3d3d3B3Dd3d3d3d3d3d3dCHgN3d3d3d3d3d3dCHgN3d3Yd3d3eId3d3cN3d3d3d3YiIiI3d3d3d3dAAAAAAAAAAAN3QAA"
"AAAAAAAAAHcN3d3d3d3d3d3Qh4Dd3d3d3d3d3d3Qh4Dd3d3Yd3d3iIh3d3cN3d3d3d3diIiN3d3d3d3Qd3d3d3d3d3dw3Qd3d3d3d3d3cAdw3d3d3dAAAN0I"
"eA3d3d3d3dAAAN0IeA3d3d3Yd3d4iIiHd3cN3d3d3d3d2Ijd3d3d3d3QeIiIiIiIiIhw3Qd3d3d3d4iHcHBw3d3d0Ad3dwCHgN3d3d3d0Ad3dwCHgN3d3d3Y"
"d3d3eId3d3cN3d3d3d3d3Y3d3d3d3d3QePf39/f39/hw3Qd3d3d3d4iHcHcA3d3dCHd3f/gIDd3d3d3dCHd3f/gIDd3d3d3Yd4d3eId3eHcN3d3djd0AAAAA"
"Dd2N3d3QeH9/f39/f3hw3QAAAAAAAAAAAAdw3d3Qh3dwB3+A3d3d3d3Qh3d3d3+A3d3d3d3YeId3eId3eIcN3d3diN2Hd3d3DdiN3d3QePeIiIiIh/hw3Qd3"
"d3d3d3d3cHB3Dd3Qd3dwB3fw3d3d3d3Qd3d3d3fw3d3d3d3YiIiIiIiIiIgN3d3diI2Hd3d3DYiN3d3QeH9/f39/f3hw3dB3d3d3d3d3cHcHDd0Hd3dwB3f3"
"Dd3d3d0Hd3d3d3f3Dd3d3d3YiIiIiIiIiIgN3d3YiIiHd3d3CIiI3d3QePeIiIiIh/hw3d0AAAAAAAAABwdwDd0H9wAAAAB3Dd3d3d0H9wAAAAB3Dd3d3d3Y"
"eId3eId3eIcN3d3diI2Hd3d3DYiN3d3QeH9/f39/f3hw3d3Qd3d3d3d3AHB3Dd0H9wAAAAB3Dd3d3d0H9wAAAAB3Dd3d3d3Yd4d3eId3eHcN3d3diN2IiIiI"
"jdiN3d3QePf39/f39/hw3d3QdwAAAAB3AAcHDd0H/3dwB3d3Dd3d3d0H/3d3d3d3Dd3d3d3Yd3d3eId3d3cN3d3djd3d3Y3d3d2N3d3QeH+IiH9/f3hw3d3d"
"B3d3d3d3cAAA3d0Hf/dwB3d3Dd3d3d0Hf/d3d3d3Dd3d3d3Yd3d4iIiHd3cN3d3d3d3d2Ijd3d3d3d3QePf39/f39/hw3d3dB3AAAAAHcN3d3d3Qf/9wB3dw"
"3d3d3d3Qf/93d3dw3d3d3d3Yd3d3iIh3d3cN3d3d3d3diIiN3d3d3d3QeH9/f39/f3hw3d3d0Hd3d3d3dw3d3d3QiP//d3eA3d3d3d3QiP//d3eA3d3d3d3Y"
"d3d3eId3d3cN3d3d3d3YiIiI3d3d3d3QeIiIiIiIiIhw3d3d0HcAAAAAdw3d3d3dCHf/93gN3d3d3d3dCHf/93gN3d3d3d3YiIiIiIiIiIiN3d3d3d3d3Y3d"
"3d3d3d3Qd3d3d3d3d3dw3d3d3Qd3d3d3d3Dd3d3d0Ad3dwDd3d3d3d3d0Ad3dwDd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dAAAAAAAAAAAN3d3d"
"3dAAAAAAAADd3d3d3dAAAN3d3d3d3d3d3dAAAN3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3Q==";
constexpr UINT IDB_WIN2K_IMGVIEW_TOOLBAR_HOT=511;
static const char kAsset511[]=
"Qk02BwAAAAAAAHYAAAAoAAAAkAAAABgAAAABAAQAAAAAAMAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwACAgIAA"
"AAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AN3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3dAN3d3d3d3d3d3d3dAN3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d0AAAAAAAAAAN3d3d3d3d3d3d3d3QzA3d3d3d3d3d3d3QzA3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dB3d3d3d3d3Bw3d3d3d3d3d3d3d0MzA3d3d3d3d3d3d0MzA3d3QAAAAAAAAAAAN3d3d3d3d3c3d"
"3d3d3d3d3d3d3d3d3d3d3dB3d3d3d3d3B3Dd3d3d3d3d3d3dDMwN3d3d3d3d3d3dDMwN3d3Qd3d3fMd3d3cN3d3d3d3czMzM3d3d3d3dAAAAAAAAAAAN3QAA"
"AAAAAAAAAHcN3d3d3d3d3d3QzMDd3d3d3d3d3d3QzMDd3d3Qd3d3zMx3d3cN3d3d3d3dzMzN3d3d3d3Qd3d3d3d3d3dw3Qd3d3d3d3d3cAdw3d3d3dAAAN0M"
"zA3d3d3d3dAAAN0MzA3d3d3Qd3d8zMzHd3cN3d3d3d3d3Mzd3d3d3d3QcAAAAAAAAABw3Qd3d3d3d7u3cHBw3d3d0Ad3dwDMwN3d3d3d0Ad3dwDMwN3d3d3Q"
"d3d3fMd3d3cN3d3d3d3d3c3d3d3d3d3QcP7+/v7+/vBw3Qd3d3d3d4iHcHcA3d3dCHd3f/gMDd3d3d3dCHd3f/gMDd3d3d3Qd8d3fMd3fHcN3d3dzd0AAAAA"
"Dd3N3d3QcO/v7+/v7+Bw3QAAAAAAAAAAAAdw3d3Qh3dwB3+A3d3d3d3Qh3d3d3+A3d3d3d3QfMd3fMd3fMcN3d3dzN0Hd3d3DdzN3d3QcP6IiIiIjvBw3Qd3"
"d3d3d3d3cHB3Dd3Qd3dwB3fw3d3d3d3Qd3d3d3fw3d3d3d3QzMzMzMzMzMwN3d3dzM0Hd3d3DczN3d3QcO/v7+/v7+Bw3dB3d3d3d3d3cHcHDd0Hd3dwB3f3"
"Dd3d3d0Hd3d3d3f3Dd3d3d3QzMzMzMzMzMwN3d3czMwHd3d3DMzM3d3QcP6IiIiIjvBw3d0AAAAAAAAABwdwDd0H9wAAAAB3Dd3d3d0H9wAAAAB3Dd3d3d3Q"
"fMd3fMd3fMcN3d3dzM0Hd3d3DczN3d3QcO/v7+/v7+Bw3d3Q////////AHB3Dd0H/gAAAAB3Dd3d3d0H/gAAAAB3Dd3d3d3Qd8d3fMd3fHcN3d3dzN0AAAAA"
"DdzN3d3QcP7+/v7+/vBw3d3Q/wAAAAD/AAcHDd0H/+dwB3d3Dd3d3d0H/+d3d3d3Dd3d3d3Qd3d3fMd3d3cN3d3dzd3d3c3d3d3N3d3QcO+IiO/v7+Bw3d3d"
"D///////8AAA3d0Hf+dwB3d3Dd3d3d0Hf+d3d3d3Dd3d3d3Qd3d8zMzHd3cN3d3d3d3d3Mzd3d3d3d3QcP7+/v7+/vBw3d3dD/AAAAAP8N3d3d3Qf/7gB3dw"
"3d3d3d3Qf/7nd3dw3d3d3d3Qd3d3zMx3d3cN3d3d3d3dzMzN3d3d3d3QcO/v7+/v7+Bw3d3d0P///////w3d3d3QiP/+d3eA3d3d3d3QiP/+d3eA3d3d3d3Q"
"d3d3fMd3d3cN3d3d3d3czMzM3d3d3d3QcAAAAAAAAABw3d3d0P8AAAAA/w3d3d3dCHf/93gN3d3d3d3dCHf/93gN3d3d3d3QAAAAAAAAAAAN3d3d3d3d3c3d"
"3d3d3d3Qd3d3d3d3d3dw3d3d3Q////////Dd3d3d0Ad3dwDd3d3d3d3d0Ad3dwDd3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3dAAAAAAAAAAAN3d3d"
"3dAAAAAAAADd3d3d3dAAAN3d3d3d3d3d3dAAAN3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d"
"3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3Q==";
constexpr UINT IDB_WIN2K_IMGVIEW_TOOLBAR_MASK=512;
static const char kAsset512[]=
"Qk0eAgAAAAAAAD4AAAAoAAAAkAAAABgAAAABAAEAAAAAAOABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP///wD///////////////////////8AAP//////////"
"/////////////wAA///n///n////////////wAA/AAD//8P//8P///////////+AAB8AAP//g///g+AAB//3/////4AADwAA//8H//8H4AAH/4D/8AAHAAAH"
"AAD//g///g/gAAf/wf/gAAMAAAMAAP4MH/4MH+AAB//j/+AAAwAAAwAA+AA/+AA/4AAH//f/4AADAAADAADwAH/wAH/gAAf3AHfgAAMAAAMAAOAA/+AA/+AA"
"B/MAZ+AAAwAAAQAA4AD/4AD/4AAH8QBH4AADgAABAADAAH/AAH/gAAfgAAPgAAPAAAEAAMAAf8AAf+AAB/EAR+AAA+AAAQAAwAB/wAB/4AAH8wBn4AAD4AAB"
"AADAAH/AAH/gAAf39/fgAAPwAAMAAMAAf8AAf+AAB//j/+AAA/AAPwAA4AD/4AD/4AAH/8H/4AAD+AAfAADgAP/gAP/gAAf/gP/gAAP4AB8AAPAB//AB/+AA"
"B//3/+AAA/wADwAA+AP/+AP/////////8AAH/gAPAAD+D//+D/////////////////8AAP///////////////////////wAA////////////////////////"
"AAA=";
constexpr UINT IDC_WIN2K_IMGVIEW_OPENHAND=513;
static const char kAsset513[]=
"AAACAAEAICAAABAADgDoAgAAFgAAACgAAAAgAAAAQAAAAAEABAAAAAAAgAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAACAAAAAgIAAgAAAAIAAgACAgAAA"
"gICAAMDAwAAAAP8AAP8AAAD//wD/AAAA/wD/AP//AAD///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATMREREQAAAAAAAAAAAAAAE"
"zEREREAAAAAAAAAAAAAABMxERERAAAAAAAAAAAAAAATMREREQAAAAAAAAAAAAAAEzEREREAAAAAAAAAAAAAAAEQAAAAAAAAAAAAAAAAAAAB////4AAAAAAAA"
"AAAAAAAAf///+AAAAAAAAAAAAAAAB/////+AAAAAAAAAAAAAAH//////gAAAAAAAAAAAAAf///////gAAAAAAAAAAAAH///////4AAAAAAAAAAAAf///////"
"+AAAAAAAAAAAAH////////+AAAAAAAAAAAf/////////gAAAAAAAAAAH/4j//////4AAAAAAAAAAf/gA//////+AAAAAAAAAAH+AB/////9/gAAAAAAAAAf4"
"AAf/f/f4D4AAAAAAAAAHcAAH+A+A+A+AAAAAAAAAAAAAB/gPgPgPgAAAAAAAAAAAAAf4D4D4D4AAAAAAAAAAAAAH+A+A+A+AAAAAAAAAAAAAB/gPgPgPgAAA"
"AAAAAAAAAAf4D4D4B3AAAAAAAAAAAAAH+A+A+AAAAAAAAAAAAAAAB/gPgPgAAAAAAAAAAAAAAAf4D4B3AAAAAAAAAAAAAAAAdw+AAAAAAAAAAAAAAAAAAAAH"
"cAAAAAAAAAD////////////gA///4AP//+AD///gA///4AP///AH///wB///8Af//+AD///AA///gAH//4AB//8AAf//AAD//gAA//4AAP/8AAD//CAA//hg"
"AP/44AD//+AA///gAP//4AD//+AA///gAf//4Af//+AH///gD///8j////5//w==";
constexpr UINT IDC_WIN2K_IMGVIEW_CLOSEDHAND=514;
static const char kAsset514[]=
"AAACAAEAICAAABAADgDoAgAAFgAAACgAAAAgAAAAQAAAAAEABAAAAAAAgAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAACAAAAAgIAAgAAAAIAAgACAgAAA"
"gICAAMDAwAAAAP8AAP8AAAD//wD/AAAA/wD/AP//AAD///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATMREREQAAAAAAAAAAAAAAE"
"zEREREAAAAAAAAAAAAAABMxERERAAAAAAAAAAAAAAATMREREQAAAAAAAAAAAAAAEzEREREAAAAAAAAAAAAAAAEQAAAAAAAAAAAAAAAAAAAB////4AAAAAAAA"
"AAAAAAAAf///+AAAAAAAAAAAAAAAB/////+AAAAAAAAAAAAAAH//////gAAAAAAAAAAAAAB///////gAAAAAAAAAAAAH///////4AAAAAAAAAAAAB///////"
"+AAAAAAAAAAAAH////////+AAAAAAAAAAAB/////////gAAAAAAAAAAAf////////4AAAAAAAAAAAH/3//////+AAAAAAAAAAAB/8P////9/gAAAAAAAAAAA"
"B4D4f/f4D4AAAAAAAAAAAABw+A+A+A+AAAAAAAAAAAAAB/gPgPgHcAAAAAAAAAAAAAf4D4D4AAAAAAAAAAAAAAAAdw+AdwAAAAAAAAAAAAAAAAAHcAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAD////////////gA///4AP//+AD///gA///4AP///AH///wB///8Af//+AD///AA///wAH//4AB//+AAf//AAD//wAA//8AAP//AAD//wAA//+A"
"AP//wAD//+AB///gB///8A////5//////////////////////////////////w==";
constexpr UINT IDC_WIN2K_IMGVIEW_ZOOMOUT=515;
static const char kAsset515[]=
"AAACAAEAICAAAAUABQAwAQAAFgAAACgAAAAgAAAAQAAAAAEAAQAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////AAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8AAAA/wAAAP8AAAH/gAABgYAAAYGAAAH/g"
"AAA/wAAAP8AAAA8AAAAAAAAA////////////////////////////////////////////////////////////////////////////////////////////+f//"
"//H////j///wx///wA///4Af//+AH///AA///wAP//8AD///AA///4Af//+AH///wD////D///8=";
constexpr UINT IDC_WIN2K_IMGVIEW_ZOOMIN=516;
static const char kAsset516[]=
"AAACAAEAICAAAAUABQAwAQAAFgAAACgAAAAgAAAAQAAAAAEAAQAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////AAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8AAAA/wAAAOcAAAHngAABgYAAAYGAAAHng"
"AAA5wAAAP8AAAA8AAAAAAAAA////////////////////////////////////////////////////////////////////////////////////////////+f//"
"//H////j///wx///wA///4Af//+AH///AA///wAP//8AD///AA///4Af//+AH///wD////D///8=";
constexpr UINT IDI_WIN2K_IMGVIEW_FULLSCREEN=517;
static const char kAsset517[]=
"AAABAAIAICAQAAAAAADoAgAAJgAAABAQEAAAAAAAKAEAAA4DAAAoAAAAIAAAAEAAAAABAAQAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAgAAA"
"AICAAIAAAACAAIAAgIAAAMDAwACAgIAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAATvwAAAAAAAAA"
"AAAAAAAABO/sAAAAAAAAAAAAAAAAAE7+zAAAAAAAAAAAAAAAAATv7MAAAAAAAAAAAAAAAABO/swAAAAAAAAAAAAAAAAE7+zAAAAAAAAAAAAAAAAATv7MAAAA"
"AAAAAAAAAAAABO/swAAAAAAAAAAAAAAAAIf+zAAAAAAAAAAAAAAAAAh/fMAAAAAAAAAAiHiIiIAI94gAAAAAAAAACHd3AAAAAAiAAAAAAAAAAId3AG/v7+AA"
"AAAAAAAAAAh3cG7+/v7+YAAAAAAAAACP9wbv7+/v7+YAAAAAAAAAj/Bu/v/+/v72AAAAAAAACP8G7+//7+/v5nAAAAAAAAj/Bv7//v7+/vZwAAAAAAAI8G/v"
"///v7+/mcAAAAAAACPBu///+/v7+9nAAAAAAAAjwb+//7+/v7+ZwAAAAAAAIcG7///7+/v5vcAAAAAAACHBv7//v7+/vb3AAAAAAAACAbv7//v7+9v8AAAAA"
"AAAAgG/v7+/v72//AAAAAAAAAAgG/v7+/vZ38AAAAAAAAAAABmbv7+ZndwAAAAAAAAAAAACIZmZnd3AAAAAAAAAAAAAAAIiIiHAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////4////8H///+B////Af///gP///wH///4D///8B///+A////Af/8BgP/8AAH/+AAD//AAB//gAAf/wAAH/8AA"
"B/+AAAP/gAAD/4AAA/+AAAP/gAAD/4AAA/+AAAP/wAAH/8AAB//gAA//8AAf//gAP//8AH///wH///////8oAAAAEAAAACAAAAABAAQAAAAAAIAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAgAAAgAAAAICAAIAAAACAAIAAgIAAAMDAwACAgIAAAAD/AAD/AAAA//8A/wAAAP8A/wD//wAA////AAAAAAAAAAQAAAAAAAAA"
"TsAAAAAAAATvwAAAAAAATvwAAAAAAATvwAAACIiISHwAAACHgAAAgAAACPCP72AAAACPDv7+9gAAAICP/+/mAAAAgP7+/vYAAACA7+/viAAAAID+/v5nAAAA"
"CG/vhnAAAAAAhmaHAAAAAAAIiIgAAAAA//kAAP/wAAD/4AAA/8EAAP+DAADgBwAAwA8AAIAfAAAAHwAAAB8AAAAfAAAAHwAAAB8AAIA/AADAfwAA4P8AAA==";
constexpr UINT ID_WIN2K_IMGVIEW_ZOOMIN=520;
constexpr UINT ID_WIN2K_IMGVIEW_ZOOMOUT=521;
constexpr UINT ID_WIN2K_IMGVIEW_ACTUALSIZE=522;
constexpr UINT ID_WIN2K_IMGVIEW_BESTFIT=523;
constexpr UINT ID_WIN2K_IMGVIEW_FULLSCREEN=524;
constexpr UINT ID_WIN2K_IMGVIEW_PRINT=525;
std::vector<BYTE> ReferenceAssetBytes(UINT id) { switch(id) {
case 900: return ReferenceDecodeBase64(kAsset900);
case 901: return ReferenceDecodeBase64(kAsset901);
case 902: return ReferenceDecodeBase64(kAsset902);
case 903: return ReferenceDecodeBase64(kAsset903);
case 910: return ReferenceDecodeBase64(kAsset910);
case 911: return ReferenceDecodeBase64(kAsset911);
case 912: return ReferenceDecodeBase64(kAsset912);
case 920: return ReferenceDecodeBase64(kAsset920);
case 921: return ReferenceDecodeBase64(kAsset921);
case 922: return ReferenceDecodeBase64(kAsset922);
case 510: return ReferenceDecodeBase64(kAsset510);
case 511: return ReferenceDecodeBase64(kAsset511);
case 512: return ReferenceDecodeBase64(kAsset512);
case 513: return ReferenceDecodeBase64(kAsset513);
case 514: return ReferenceDecodeBase64(kAsset514);
case 515: return ReferenceDecodeBase64(kAsset515);
case 516: return ReferenceDecodeBase64(kAsset516);
case 517: return ReferenceDecodeBase64(kAsset517);
default: return {}; } }
} }

// Adapted from the matching ClassicExplorer webview source: reference-resources.inc
namespace ce { namespace win2kwebview {
PCWSTR ReferenceCaption(PCWSTR original);
std::mutex g_referenceCursorMutex;
HCURSOR g_referenceCursors[4]={};
HICON g_referenceIcon=nullptr;
bool g_referenceZoomRegistered=false, g_referenceDetachedRegistered=false, g_referenceDetailsRegistered=false;
bool RegisterReferenceViewerClasses() noexcept;

HCURSOR ReferenceCursor(UINT id) {
    if (id<IDC_WIN2K_IMGVIEW_OPENHAND || id>IDC_WIN2K_IMGVIEW_ZOOMIN)
        return LoadCursorW(nullptr,IDC_ARROW);
    std::lock_guard<std::mutex> lock(g_referenceCursorMutex);
    HCURSOR& cached=g_referenceCursors[id-IDC_WIN2K_IMGVIEW_OPENHAND];
    if (!cached) {
        auto data=ReferenceAssetBytes(id);
        if (data.size()>=22) {
            DWORD offset=0,length=0;
            memcpy(&length,data.data()+14,4); memcpy(&offset,data.data()+18,4);
            if (offset<=data.size() && length<=data.size()-offset && length>0) {
                std::vector<BYTE> image(length+4);
                memcpy(image.data(),data.data()+10,4);
                memcpy(image.data()+4,data.data()+offset,length);
                cached=reinterpret_cast<HCURSOR>(CreateIconFromResourceEx(image.data(),
                    static_cast<DWORD>(image.size()),FALSE,0x30000,0,0,LR_DEFAULTCOLOR));
            }
        }
    }
    return cached ? cached : LoadCursorW(nullptr,IDC_ARROW);
}
HICON ReferenceIcon(UINT id) {
    std::lock_guard<std::mutex> lock(g_referenceCursorMutex);
    if (!g_referenceIcon) {
        auto data=ReferenceAssetBytes(id);
        if (data.size()>=22) {
            DWORD length=0,offset=0;
            memcpy(&length,data.data()+14,4); memcpy(&offset,data.data()+18,4);
            if (offset<=data.size() && length<=data.size()-offset && length>0)
                g_referenceIcon=CreateIconFromResourceEx(data.data()+offset,length,TRUE,0x30000,32,32,LR_DEFAULTCOLOR);
        }
    }
    return g_referenceIcon;
}
HMENU ReferenceMenu() {
    HMENU menu=CreateMenu(), popup=CreatePopupMenu();
    if (!menu||!popup) { if(menu) DestroyMenu(menu); if(popup) DestroyMenu(popup); return nullptr; }
    const struct { UINT id; PCWSTR text; } commands[]={
        {ID_WIN2K_IMGVIEW_ZOOMIN,L"Zoom In"},{ID_WIN2K_IMGVIEW_ZOOMOUT,L"Zoom Out"},
        {0,nullptr},{ID_WIN2K_IMGVIEW_ACTUALSIZE,L"Actual Size"},
        {ID_WIN2K_IMGVIEW_BESTFIT,L"Best Fit"},{ID_WIN2K_IMGVIEW_FULLSCREEN,L"Full Screen"},
        {ID_WIN2K_IMGVIEW_PRINT,L"Print"},
    };
    for (const auto& command:commands) {
        const auto options=g_webOptions.load();
        if (command.id && command.id<=ID_WIN2K_IMGVIEW_BESTFIT && !options->zoom) continue;
        if (command.id==ID_WIN2K_IMGVIEW_FULLSCREEN && !options->detached) continue;
        if (command.id==ID_WIN2K_IMGVIEW_PRINT && !options->print) continue;
        if (command.id) AppendMenuW(popup,MF_STRING,command.id,ReferenceCaption(command.text));
        else AppendMenuW(popup,MF_SEPARATOR,0,nullptr);
    }
    AppendMenuW(menu,MF_POPUP,reinterpret_cast<UINT_PTR>(popup),L"");
    return menu;
}
void FreeReferenceSharedResources() {
    std::lock_guard<std::mutex> lock(g_referenceCursorMutex);
    for(auto& cursor:g_referenceCursors) { if(cursor) DestroyCursor(cursor); cursor=nullptr; }
    if(g_referenceIcon) DestroyIcon(std::exchange(g_referenceIcon,nullptr));
    if(g_referenceZoomRegistered) UnregisterClassW(L"ClassicWebViewPane.Reference.ImgViewZoom",ReferenceModule());
    if(g_referenceDetachedRegistered) UnregisterClassW(L"ClassicWebViewPane.Reference.ImgViewDetached",ReferenceModule());
    if(g_referenceDetailsRegistered) UnregisterClassW(L"ClassicWebViewPane.Reference.ImgViewDetails",ReferenceModule());
    g_referenceZoomRegistered=false; g_referenceDetachedRegistered=false;
    g_referenceDetailsRegistered=false;
}
} }

// Adapted from the matching ClassicExplorer webview source: reference-renderer.inc
namespace ce::win2kwebview {
// The image is borrowed from the mod cache under its rendering lock.
struct ReferenceDivider {
    bool configured=false,alpha=false,gradient=false;
    HBITMAP bitmap=nullptr;
    SIZE size{};
    COLORREF color=RGB(0,0,255),background=RGB(255,255,255);
    void Set(HBITMAP image,SIZE dimensions,bool hasAlpha,COLORREF line,COLORREF backdrop,bool fade) noexcept {
        configured=true; bitmap=image; size=dimensions; alpha=hasAlpha;
        color=line; background=backdrop; gradient=fade;
    }
    void Paint(HDC dc,int x,int y,int width,int height) const noexcept {
        if(width<=0 || height<=0) return;
        if(bitmap && size.cx>0 && size.cy>0) {
            HDC source=CreateCompatibleDC(dc);
            if(!source) return;
            HGDIOBJ old=SelectObject(source,bitmap);
            if(alpha) {
                BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
                AlphaBlend(dc,x,y,width,height,source,0,0,size.cx,size.cy,blend);
            } else {
                const int mode=SetStretchBltMode(dc,HALFTONE);
                POINT origin{}; SetBrushOrgEx(dc,0,0,&origin);
                StretchBlt(dc,x,y,width,height,source,0,0,size.cx,size.cy,SRCCOPY);
                SetBrushOrgEx(dc,origin.x,origin.y,nullptr); SetStretchBltMode(dc,mode);
            }
            SelectObject(source,old); DeleteDC(source); return;
        }
        if(gradient) {
            const auto channel=[](BYTE value) { return static_cast<COLOR16>(value*257u); };
            TRIVERTEX vertices[]={{x,y,channel(GetRValue(color)),channel(GetGValue(color)),channel(GetBValue(color)),0},
                {x+width,y+height,channel(GetRValue(background)),channel(GetGValue(background)),channel(GetBValue(background)),0}};
            GRADIENT_RECT rectangle{0,1};
            GradientFill(dc,vertices,2,&rectangle,1,GRADIENT_FILL_RECT_H);
        } else {
            const RECT rectangle{x,y,x+width,y+height};
            HBRUSH brush=CreateSolidBrush(color); FillRect(dc,&rectangle,brush); DeleteObject(brush);
        }
    }
};
}

// Query colours on every paint rather than depending on USER's cached brushes.
namespace ce { namespace win2kwebview {
static void FillReferenceColour(HDC dc,const RECT& area,COLORREF colour) noexcept {
    const HBRUSH brush=CreateSolidBrush(colour);
    if(brush) { FillRect(dc,&area,brush);DeleteObject(brush); }
}
static void FillReferenceSystemColour(HDC dc,const RECT& area,int colour) noexcept {
    FillReferenceColour(dc,area,GetSysColor(colour));
}
// Own implementation of the mod's existing imageBlendWhite option. Opaque
// artwork is multiplied into the selected flat pane colour; real alpha stays
// on AlphaBlend's path. Work in source pixels so logical DPI mapping is intact.
static BOOL DrawReferenceWhiteBlend(HDC dc,HBITMAP image,const SIZE& size,
                                    int x,int y,int width,int height,COLORREF background) noexcept {
    if(!image || size.cx<=0 || size.cy<=0 || width<=0 || height<=0 ||
       size.cx>16384 || size.cy>16384 || size.cx>16777216/size.cy) return FALSE;
    HDC from=CreateCompatibleDC(dc),to=CreateCompatibleDC(dc);
    BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth=size.cx;info.bmiHeader.biHeight=-size.cy;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;void* pixels=nullptr;
    HBITMAP bitmap=to ? CreateDIBSection(to,&info,DIB_RGB_COLORS,&pixels,nullptr,0) : nullptr;
    BOOL drawn=FALSE;
    if(from && to && bitmap && pixels) {
        const auto oldFrom=SelectObject(from,image),oldTo=SelectObject(to,bitmap);
        if(BitBlt(to,0,0,size.cx,size.cy,from,0,0,SRCCOPY)) {
            GdiFlush();
            const BYTE tint[]{GetBValue(background),GetGValue(background),GetRValue(background)};
            BYTE* data=static_cast<BYTE*>(pixels);
            const size_t count=static_cast<size_t>(size.cx)*size.cy;
            for(size_t i=0;i<count;++i) for(size_t channel=0;channel<3;++channel)
                data[4*i+channel]=static_cast<BYTE>(static_cast<unsigned>(data[4*i+channel])*tint[channel]/255);
            drawn=StretchBlt(dc,x,y,width,height,to,0,0,size.cx,size.cy,SRCCOPY);
        }
        SelectObject(from,oldFrom);SelectObject(to,oldTo);
    }
    if(bitmap) DeleteObject(bitmap);if(from) DeleteDC(from);if(to) DeleteDC(to);
    return drawn;
}
} }

// The pre-DLL description followed the native tooltip/status font and colours.
namespace ce { namespace win2kwebview {
static int ReferenceDescriptionLineWidth(HDC dc,PCWSTR value,const RECT& text,UINT format) noexcept {
    TEXTMETRICW metrics{};
    const int saved=SaveDC(dc);
    if(!saved || !GetTextMetricsW(dc,&metrics)) { if(saved) RestoreDC(dc,saved);return text.right-text.left; }
    // Ask the documented formatter which characters belong to each row, then
    // exclude its consumed wrapping spaces/CRLF from that row's visible width.
    // An empty clip makes this also work during hidden scrollbar measurement.
    IntersectClipRect(dc,0,0,0,0);
    int widest=0;
    const size_t length=wcslen(value);
    for(size_t start=0;start<length;) {
        RECT row{text.left,text.top,text.right,text.top+std::max<LONG>(1,metrics.tmHeight)};
        DRAWTEXTPARAMS params{sizeof(params)};
        DrawTextExW(dc,const_cast<LPWSTR>(value+start),static_cast<int>(length-start),&row,format,&params);
        if(!params.uiLengthDrawn || params.uiLengthDrawn>length-start) {
            widest=text.right-text.left;break;
        }
        const size_t next=start+params.uiLengthDrawn;
        size_t end=next;
        while(end>start && (value[end-1]==L' ' || value[end-1]==L'\r' || value[end-1]==L'\n' || value[end-1]==L'\t')) --end;
        RECT run{text.left,text.top,text.left,text.top};
        if(end>start) {
            DrawTextW(dc,value+start,static_cast<int>(end-start),&run,
                DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_CALCRECT|DT_EDITCONTROL);
            widest=std::max<int>(widest,run.right-run.left);
        }
        start=next;
    }
    RestoreDC(dc,saved);return widest;
}
static RECT DrawReferenceDescription(HDC dc,HFONT font,PCWSTR value,int x,int y,int width) noexcept {
    if(!dc || !value || !*value || width<=0) return {x,y,x,y};
    const int saved=SaveDC(dc);
    if(!saved) return {x,y,x,y};
    if(font) SelectObject(dc,font);
    SetBkMode(dc,TRANSPARENT);SetTextColor(dc,GetSysColor(COLOR_INFOTEXT));
    RECT text{x+3,y+2,x+std::max(4,width-3),y+2};
    const UINT format=DT_LEFT|DT_WORDBREAK|DT_NOPREFIX|DT_EDITCONTROL;
    DrawTextW(dc,value,-1,&text,format|DT_CALCRECT);
    const int height=text.bottom;
    text.right=x+std::max(4,width-3);
    const int measuredWidth=ReferenceDescriptionLineWidth(dc,value,text,format);
    RECT box{x,y,std::min<LONG>(x+width,text.left+measuredWidth+3),height+2};
    // Query the colours themselves, as 2.16 did. Classic colour hooks can
    // replace GetSysColor while USER's cached GetSysColorBrush stays unchanged.
    HBRUSH background=CreateSolidBrush(GetSysColor(COLOR_INFOBK));
    HBRUSH border=CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
    if(background) { FillRect(dc,&box,background);DeleteObject(background); }
    if(border) { FrameRect(dc,&box,border);DeleteObject(border); }
    IntersectClipRect(dc,x+3,y+2,box.right-3,box.bottom-2);
    DrawTextW(dc,value,-1,&text,format);
    RestoreDC(dc,saved);
    return box;
}
} }

namespace ce
{
namespace win2kwebview
{
	/*
	 * Which Windows the pane reproduces.
	 *
	 * This is not a skin selector. A profile decides geometry, typography, collapse behaviour,
	 * which assets are loaded, and the content grammar — Windows 98 lays a percentage-width pane
	 * across the top when narrow, where Windows 2000/Me keep a fixed 200px column and simply
	 * collapse it. Mixing them (98 metrics with Me assets, say) is not a state this supports.
	 *
	 * Stored as one bounded integer rather than several booleans, so an unknown value has exactly
	 * one meaning and one fallback.
	 */
	enum class WebViewVisualProfile : DWORD
	{
		Windows2000 = 0,
		Windows98   = 1,
		WindowsME   = 2,
	};

	/*
	 * Which Windows 98 template is in play.
	 *
	 * Windows 98 ships a separate .htt per namespace, and they are genuinely different programs --
	 * different strings, different detail columns, different collapse thresholds, and in the
	 * system-folder case a different page structure entirely. The Windows 2000 and Me profiles get
	 * away with one builder and a few flags because their templates share a common body; that does
	 * not carry over here, so the profile dispatches on template kind and each kind gets a builder
	 * that can be read side by side with its .htt.
	 *
	 * Kinds are resolved from namespace identity (PIDL / known folder / CSIDL) or, for the system
	 * folders, from the desktop.ini that actually selects the template on the reference guest.
	 * Never from the display name -- those are localized and modernized.
	 */
	enum class Win98TemplateKind
	{
		Folder,                 // Web\FOLDER.HTT -- ordinary filesystem folder
		MyComputer,             // Web\MYCOMP.HTT
		ControlPanel,           // Web\CONTROLP.HTT
		Printers,               // Web\PRINTERS.HTT
		DialUpNetworking,       // Web\dialup.htt
		NetworkNeighborhood,    // Web\nethood.htt
		RecycleBin,             // Web\recycle.htt
		SystemFolderBarricade,  // C:\WINDOWS\folder.htt -- warning + Show Files, list hidden
		SystemFolderWarnOnly,   // C:\Program Files\folder.htt -- warning, list visible
		DefaultCustom,          // Web\default.htt
		Unsupported,            // identified but deliberately not rendered (FTP, Scheduled Tasks)
	};

	// FixSize()'s `var threshold`. Every shipped template uses 400 except the system-folder
	// barricade, which uses 450 -- verified against the reference guest's own folder.htt.
	inline int Win98CollapseThresholdFor(Win98TemplateKind kind) noexcept
	{
		return (kind == Win98TemplateKind::SystemFolderBarricade) ? 450 : 400;
	}

	// How the pane is laid out. Windows 2000 and Windows Me share the NT5 fixed column; only
	// Windows 98 scales with the window, which is why it needs its own resolver rather than a
	// flag inside the NT5 one.
	enum class WebViewLayoutFamily
	{
		Nt5FixedPane,
		Win98PercentPane,
	};

	inline WebViewLayoutFamily LayoutFamilyFor(WebViewVisualProfile profile) noexcept
	{
		return (profile == WebViewVisualProfile::Windows98)
			? WebViewLayoutFamily::Win98PercentPane : WebViewLayoutFamily::Nt5FixedPane;
	}


} }
namespace ce
{
namespace win2kwebview
{
	// Canonical 96-DPI layout from the updated retail Windows 2000 SP4/IE6 folder.htt probe.
	namespace panemetrics
	{
		constexpr int kPanelWidth       = 200; // #Panel {width: 200px}
		constexpr int kFlowRight        = 200; // measured FolderName/LogoLine/Details right edge
		constexpr int kCornerPadLeft    = 12;  // #Corner {padding-left: 12px}
		constexpr int kCornerPadTop     = 11;  // #Corner {padding-top: 11px}
		constexpr int kFolderIconSize   = 32;  // #FolderIcon {width: 32px; height: 32px}
		constexpr int kFolderNameTop    = 51;  // measured #FolderName top
		constexpr int kFolderNamePt     = 13;  // #FolderName {font: 13pt/13pt menu}
		constexpr int kFolderNameInkOffsetY = -2; // matched SP4/IE6 glyph ink inside the line box
		constexpr int kFolderNameLineAdvance = 17; // measured +17px per wrapped title row
		constexpr int kLogoLineTop      = 72;  // measured #LogoLine top
		[[maybe_unused]] constexpr int kLogoLineHeight   = 2;   // measured #LogoLine height
		constexpr int kDetailsPadLeft   = 12;  // #Details {padding-left: 12px}
		constexpr int kDetailsTop       = 89;  // measured Details text top
		constexpr int kDetailsRight     = 200; // measured Details right edge

		// The probe reports a 13px body line box, 12px paragraph margins, and 4px Half margins.
		constexpr int kParagraphTop    = 12;   // p {margin-top: 12px}
		constexpr int kHalfParagraphTop = 4;   // p.Half {margin-top: 4px}
		constexpr int kBodyLineHeight  = 13;   // measured IE5 line box used by an explicit <br>

		// .Legend {margin-left: 8px} plus the 12x12 bordered swatch the capacity rows put in
		// front of "Used:" and "Free:" — <table class=Legend width=12 height=12 border=1>.
		constexpr int kLegendIndent    = 16;  // measured effective offset in retail SP4/IE6
		constexpr int kLegendSize      = 12;
		constexpr int kThumbnailSize   = 120;  // #Thumbnail {width: 120px; height: 120px}
		constexpr int kDetailsContentWidth = kDetailsRight - kDetailsPadLeft;
		constexpr int kMessageWidth    = kDetailsContentWidth;

		// #FileList {left: expression((document.body.clientWidth < Panel.style.pixelWidth * 2)
		//                             ? 0 : Panel.style.pixelWidth)}
		// The pane collapses when the view is narrower than twice the panel.
		constexpr int kCollapseBelow   = kPanelWidth * 2;

		// nethood.htt's Entire Network barricade. ResizeBarricade() keeps Brand beside the panel
		// while `document.body.clientWidth >= Panel.style.pixelWidth + 314`, and below that
		// stretches it over the whole client with the soft-barrier text moved into it.
		constexpr int kBarricadeBesideAt = kPanelWidth + 314;
		// #Brand {position: absolute; left: 200px; width: 100%; height: 100%; padding-left: 12px}
		constexpr int kBrandPadLeft    = 12;

// Image folders share the ordinary fixed-width reservation in this mod.
		constexpr int kImgFolderNameTop = 8;
		constexpr int kImgLogoLineTop   = 29;
		constexpr int kImgDetailsTop    = 46;
		constexpr int kImgParagraphTop  = 8;
		[[maybe_unused]] constexpr int kImgHalfParagraphTop = 4;
		[[maybe_unused]] constexpr int kImgBodyLineHeight = kBodyLineHeight;
		constexpr int kImgFrameInset    = 1;
	}

	/*
	 * Windows 98, which is a different layout rather than a restyled one.
	 *
	 * Every value here comes from the Windows 98 WEB folder as shipped — `webview.css` and the
	 * inline <style> in FOLDER.HTT agree, and both are in me 98/assets/WEB98:
	 *
	 *     body        {font: 8pt/10pt verdana; margin: 0}
	 *     #Panel      {position: absolute; width: 30%; height: 100%; overflow: auto}
	 *     #FileList   {position: absolute; left: 30%; width: 70%; height: 100%}
	 *     p           {margin-left: 15px; margin-top: 15px; margin-right: 15px}
	 *     p.Title     {font: 16pt; font-weight: bold; margin-top: 5px}
	 *     p.LogoLine  {margin-left: 0; margin-top: -5px; margin-right: 0; margin-bottom: 20px}
	 *     p.Links     {margin-top: 5px}
	 *     #Thumbnail  {width: 160px; height: 160px; margin-top: 0px}
	 *     #PieChart   {width: 100px; height: 50px; margin-top: 10px}
	 *
	 * and FixSize() (FOLDER.HTT:53-73) supplies the collapse rule:
	 *
	 *     threshold = 400; miniHeight = 32;
	 *     cw < threshold -> Panel hidden, MiniBanner shown, FileList at (0, 32)
	 *     otherwise      -> MiniBanner hidden, Panel shown, FileList at (Panel.pixelWidth, 0)
	 *
	 * Two things stop this being a variation on the NT5 metrics. The panel is a percentage of
	 * the client, so it has no fixed width at all; and below the threshold the pane becomes a
	 * *horizontal* banner, with the file list moving down rather than left.
	 */
	namespace win98metrics
	{
		// #Panel {width: 30%} / #FileList {left: 30%; width: 70%}
		constexpr int kPanelPercent    = 30;
		// FixSize(): below this client width the panel is replaced by the mini banner.
		constexpr int kCollapseBelow   = 400;
		// #MiniBanner {position: absolute; width: 100%; height: 32px; background: window}
		constexpr int kMiniBannerHeight = 32;
		// The banner title is wrapped in <table><tr><td nowrap>. Quirks-mode table and cell
		// defaults push it one pixel right and one pixel down, measured on the reference guest.
		constexpr int kMiniBannerCellInset = 1;

		// p {margin-left: 15px; margin-top: 15px; margin-right: 15px}
		constexpr int kParagraphLeft   = 15;
		constexpr int kParagraphRight  = 15;
		constexpr int kParagraphTop    = 15;
		// p.Links {margin-top: 5px} and p.Title {margin-top: 5px}
		constexpr int kLinksTop        = 5;
		constexpr int kTitleTop        = 5;
		// p.LogoLine {margin-top: -5px; margin-bottom: 20px}, drawn from <img height=1px>
		constexpr int kLogoLineTop     = -5;
		constexpr int kLogoLineBottom  = 20;
		[[maybe_unused]] constexpr int kLogoLineHeight  = 1;
		// The 1px image does not set the paragraph's height: it sits on a text baseline, so the
		// paragraph is a full body line box and the rule is drawn part-way down it. Measured on the
		// reference guest (evidence/win98-ie6/PHASE1-GEOMETRY.md): p.LogoLine top 72, height 13,
		// and the wvline.gif img at y=82 -- 10px into the box. Advancing by the image height alone
		// pulled everything below the rule 12px too high.
		constexpr int kLogoLineBoxHeight = 13;
		constexpr int kLogoLineRuleOffset = 10;

		// body {font: 8pt/10pt verdana}. Unlike Windows 2000 and Me this names a face and a size
		// outright instead of `font: menu`, so this pane does not follow the user's metrics.
		constexpr int kBodyPt          = 8;
		// The `/10pt` is a line height in points, not pixels. At 96 DPI 10pt is 13.33px and IE6's
		// used line box rounds to 13 -- measured on the reference guest, where both the Info
		// paragraph and the Info span come back exactly 13 tall. Using the declared 10 as a pixel
		// count compressed every body line by 3px.
		constexpr int kBodyLineHeight  = 13;
		constexpr int kTitlePt         = 16;
		// p.Title {font: 16pt; font-weight: bold} with line-height: normal resolves to a 25px line
		// box on the reference guest. Windows 10's DrawText result for the same nominal font is not
		// the same number, and letting it drive the next block's origin couples the historical
		// layout to modern glyph metrics (session prompt §7.10).
		constexpr int kTitleLineHeight = 25;
		constexpr wchar_t kFaceName[]  = L"Verdana";

		// The folder icon is a 32x32 object inside an ordinary <p>.
		constexpr int kFolderIconSize  = 32;
		// #Thumbnail {width: 160px; height: 160px}, #PieChart {width: 100px; height: 50px}
		constexpr int kThumbnailSize   = 160;
		// FOLDER.HTT gates the request on `size && (size < 10000000)`. A flat byte count, not a
		// heuristic, and strictly less-than: a file of exactly 10,000,000 bytes gets no preview.
		constexpr LONG kThumbnailByteLimit = 10000000;
		constexpr int kPieWidth        = 100;
		constexpr int kPieHeight       = 50;
		// #PieChart {margin-top: 10px}, on top of the containing paragraph's own 15px.
		constexpr int kPieMarginTop    = 10;
		// Depth-to-height ratio of the capacity disc.
		//
		// The 9x webvw.dll (stuff/WEBVW.DLL, 1998) computes this as targetHeight / 6 -- `push 6`
		// at 0x78A88B4B feeding the `idiv esi` at 0x78A88B78 inside the only function that calls
		// GDI Pie. That is the same divisor XP's thumbctl.cpp uses, so the disc geometry did not
		// change between the two and this constant is shared rather than profile-specific.
		[[maybe_unused]] constexpr int kPieShadowScale  = 6;

		// MYCOMP.HTT's legend tables: width=12 height=12 border=1, both border colours black.
		// There is no .Legend class in the Windows 98 stylesheet, so unlike the NT5 legend the box
		// sits at the paragraph margin with no extra indent; only the "&nbsp;" separates it from
		// the label.
		constexpr int kLegendSize      = 12;
		// Gap from the swatch's right edge to the label's text origin. The template has only
		// "&nbsp;" there, which is ~4px in 8pt Verdana, but the table is align=left -- a float --
		// and IE adds its own separation beside it. Measured off the reference capture rather than
		// derived: panel left 10, swatch box x=25..36, and the label's text origin at x=44.
		constexpr int kLegendGap       = 7;

		// #Panel is a percentage, so this is the only place the width is decided.
		inline int PanelWidthFor(int clientWidth) noexcept
		{
			if (clientWidth < kCollapseBelow)
			{
				return 0;   // mini-banner mode: no side panel at all
			}
			// IE6 rounds the percentage to the nearest pixel, halves up -- it does not truncate.
			// Measured across the reference width matrix: 399 -> 120 (119.7), 1023 -> 307 (306.9),
			// 1025 -> 308 (307.5). Truncation gives 119, 306 and 307 respectively.
			return (clientWidth * kPanelPercent + 50) / 100;
		}

		// FileList takes the exact remainder rather than a separately rounded 70%. Confirmed at
		// every measured width: left + width equals the client width, so no seam can appear.
		inline int FileListWidthFor(int clientWidth) noexcept
		{
			return clientWidth - PanelWidthFor(clientWidth);
		}
	}

	enum class PaneProfile
	{
		Standard,
		ImgView,
	};

	enum class ImgPreviewState
	{
		NoSelection,
		Loading,
		Image,
		Multiple,
		Failed,
	};

	// nethood.htt hides the file list on Entire Network and shows the #Brand soft barrier in its
	// place. ResizeBarricade() picks between the two live shapes; `None` is every other folder.
	//
	// Windows Me's sysroot.htt uses the same mechanism for a drive root, with its own artwork.
	enum class BarricadeMode
	{
		None,
		Beside,   // Brand sits right of the panel; the barrier text stays in #Info
		Full,     // Brand covers the whole client and carries the barrier text itself
	};

	// Which image #Brand carries. nethood.htt says `background: window URL(wvnet.gif)`;
	// sysroot.htt says `URL(wvlogo.gif)`. Both are anchored right/bottom and never tiled.
	enum class BarricadeArt
	{
		Network,   // wvnet.gif — Entire Network
		Logo,      // wvlogo.gif — Windows Me drive root
	};

	// One line of pane text. The pane is a short, flat list of these rather than a layout
	// engine, because that is all the reference content needs.
	struct PaneLine
	{
		std::wstring text;
		// nethood.htt and dialup.htt place anchors inside sentences. These two optional
		// plain-text runs keep only the historical anchor clickable.
		std::wstring linkPrefix;
		std::wstring linkSuffix;
		// printers.htt has two mixed-font rows: the inline bold "Add Printer" phrase and
		// the bold, coloured status value. `text` is the bold run between these plain runs.
		std::wstring boldPrefix;
		std::wstring boldSuffix;
		COLORREF boldColor = CLR_INVALID; // CLR_INVALID keeps COLOR_WINDOWTEXT
		bool heading = false;    // 13pt bold, the folder name
		bool link = false;       // blue, underlined
		bool button = false;     // recycle.htt/default.htt HTML push button
		bool bold = false;       // ShowInfo() emits the selected item's name as <b>
		bool message = false;    // folder.htt .Message description box
		bool previewBefore = false; // place the 120px Thumbnail object before this line
		int marginTop = 0;
		// nethood.htt's soft barrier wraps its search anchors in
		// <div style="margin-left: 12px">, which is the only block indent the templates use.
		int indent = 0;
		int buttonWidth = 0;     // CSS px; zero keeps the IE intrinsic text width
		wchar_t buttonAccessKey = 0;

		// Capacity legend swatch drawn before the text, or Swatch::None. The reference draws a
		// 12x12 black-bordered box filled 3DFACE for used and 3DHIGHLIGHT for free, indented by
		// .Legend's 8px margin.
		enum class Swatch { None, Used, Free };
		Swatch swatch = Swatch::None;

		// Shown in the status bar while the pointer is over a link. standard.htt:629
		// OnWebviewLinkEnter sets window.status from the anchor's title and clears it on exit,
		// so an empty string here means "clear", not "leave whatever was there".
		std::wstring status;

		// HTML title text owned by this pane. File-list InfoTips remain owned by Explorer's
		// SysListView32; these rectangles cover only pane links and the two drive swatches.
		std::wstring tooltip;
		RECT tooltipBounds{};

		// Approved cross-links navigate by CSIDL. External and legacy-command targets are kept
		// only for evidence logging and are denied before activation. Recycle actions are handled
		// by the session so they can revalidate the live namespace and selection at click time.
		enum class LinkAction
		{
			BrowseCsidl,
			DeniedHttp,
			DeniedLegacyCommand,
			RecycleEmpty,
			RecycleRestoreSelection,
			RecycleRestoreAll,
			// nethood.htt's Entire Network soft barrier. ShowEntireContents is the template's own
			// ShowFiles() — it dismisses the barrier in place rather than navigating. The three
			// search actions map to the IShellDispatch/IShellDispatch2 methods the template called
			// through FileList.Folder.Application; its SearchAssistantOC people search has no
			// surviving canonical equivalent and stays denied.
			ShowEntireContents,
			FindComputer,
			FindPrinter,
			FindFiles,
			// sysroot.htt's L_Simple_Text: puts the Windows Me drive barricade back up.
			HideDriveContents,
			// The Windows 98 system-folder template's own ShowFiles(). Like nethood's, it reveals
			// the list in place rather than navigating, but it belongs to a different template
			// with a different threshold, so it gets its own action rather than sharing one.
			ShowSystemFolderFiles,
			// folder.htt makes the Attributes *heading* a link:
			//   title.link("JavaScript:onClick=Properties()")
			// whose handler is FileList.SelectedItems().Item(0).InvokeVerb(L_Properties_Text).
			// This resolves and invokes only the canonical "properties" verb on a revalidated
			// single selection -- it is not a general arbitrary-verb action.
			ItemProperties,
		};
		LinkAction linkAction = LinkAction::BrowseCsidl;
		std::wstring linkTarget;
		int csidl = -1;

		// Filled in by Paint. Hit-testing reads back the rectangle the text was actually drawn
		// into, so wrapped links stay clickable over their whole area and no layout maths has
		// to be duplicated outside the renderer.
		RECT bounds{};
	};

	class WebViewNativePane
	{
		public:
			WebViewNativePane() = default;
			~WebViewNativePane();

			WebViewNativePane(const WebViewNativePane &) = delete;
			WebViewNativePane &operator=(const WebViewNativePane &) = delete;

			// Which Windows this pane reproduces. Set before EnsureResources, because it decides
			// which embedded resource IDs are loaded. Changing it discards the loaded resources.
			void SetVisualProfile(WebViewVisualProfile profile) noexcept;
			WebViewVisualProfile VisualProfile() const noexcept { return m_visualProfile; }

			// Forces NONANTIALIASED_QUALITY on the pane fonts; see WebViewSettings.
			void SetFontSmoothingDisabled(bool disabled) noexcept;

			// Loads the original image bytes embedded in ClassicExplorer.dll as RCDATA. Safe to
			// call again. The source files remain under Win2KWebView\Assets only as rc.exe inputs.
			void EnsureResources() noexcept;
            void SetColours(COLORREF background,COLORREF text,COLORREF heading,COLORREF link) noexcept {
                m_backgroundColour=background; m_textColour=text; m_headingColour=heading; m_linkColour=link;
            }
            COLORREF BackgroundColour() const noexcept { return m_backgroundColour==CLR_INVALID ? GetSysColor(COLOR_WINDOW) : m_backgroundColour; }
            COLORREF TextColour() const noexcept { return m_textColour==CLR_INVALID ? GetSysColor(COLOR_WINDOWTEXT) : m_textColour; }
            COLORREF HeadingColour() const noexcept { return m_headingColour==CLR_INVALID ? TextColour() : m_headingColour; }
            COLORREF LinkColour() const noexcept { return m_linkColour==CLR_INVALID ? GetSysColor(COLOR_HOTLIGHT) : m_linkColour; }
            void SetPictureWhiteBlend(bool enabled) noexcept { m_pictureWhiteBlend=enabled; }
            void InvalidateFonts() noexcept { ReleaseFonts(); }
            void SetDecoration(HBITMAP corner, SIZE size, int width, bool divider, bool header, bool alpha=false, HICON icon=nullptr) noexcept {
                m_customCorner=corner; m_customCornerSize=size; m_customCornerWidth=width;
                m_customCornerIcon=icon;
                m_customCornerAlpha=alpha; m_useProfileCorner=false;
                m_showDivider=divider; m_showHeader=header;
            }
            void UseProfileDecoration(bool divider,bool header) noexcept {
                m_useProfileCorner=true; m_showDivider=divider; m_showHeader=header;
            }
            void SetDivider(HBITMAP image,SIZE size,bool alpha,COLORREF color,COLORREF background,bool gradient) noexcept {
                m_divider.Set(image,size,alpha,color,background,gradient);
            }
            void SetDimensionsVisible(bool show) noexcept { m_showDimensions=show; }
            void SetImgDetailsOptions(bool compactHeader,bool scroll) noexcept {
                m_compactImgHeader=compactHeader; m_scrollImgDetails=scroll;
            }
            SIZE PaintImgDetails(HDC dc,const RECT& viewport,UINT dpi,POINT scroll,int dividerWidth=0) noexcept;
            void PaintImgFrame(HDC dc,const RECT& paneRect) noexcept { PaintImgPreview(dc,paneRect); }

			// Content for the current folder and selection. Rebuilt per navigation, never cached
			// across sessions.
			void SetFolder(const std::wstring &displayName, HICON icon) noexcept;
			void SetLines(std::vector<PaneLine> lines) noexcept;
			void SetProfile(PaneProfile profile) noexcept { m_profile = profile; }
			PaneProfile Profile() const noexcept { return m_profile; }

			// nethood.htt's Entire Network barricade. In `Full` the panel chrome is suppressed and
			// the lines are laid out inside #Brand instead of #Details, which is what
			// ResizeBarricade() does by moving the barrier text between the two elements.
			void SetBarricade(BarricadeMode mode, BarricadeArt art = BarricadeArt::Network) noexcept
			{
				m_barricade = mode;
				m_barricadeArt = art;
			}
			BarricadeMode Barricade() const noexcept { return m_barricade; }

			// Paints the #Brand field: the panel colour with the profile's barricade image
			// anchored bottom right, per `background: ... no-repeat right bottom`. The caller
			// supplies the rectangle because Brand's extent is decided by ResizeBarricade, not by
			// the pane.
			void PaintBarricade(HDC dc, const RECT &brandRect) noexcept;

			// Which shape ResizeBarricade() would choose for a view of this width.
			static BarricadeMode BarricadeShapeFor(int viewWidth) noexcept
			{
				return (viewWidth < panemetrics::kBarricadeBesideAt)
					? BarricadeMode::Full : BarricadeMode::Beside;
			}

			// Windows 98's system-folder barricade switches shape on its own template threshold,
			// not on NT5's 514px. #Brand is declared at left:30%; width:70%; height:100%, so above
			// the threshold it covers exactly the file list; below it, FixSize() stretches it over
			// the whole client and moves the warning text into it.
			static BarricadeMode Win98BarricadeShapeFor(int viewWidth) noexcept
			{
				return (viewWidth < Win98CollapseThresholdFor(
				                        Win98TemplateKind::SystemFolderBarricade))
					? BarricadeMode::Full : BarricadeMode::Beside;
			}

			// Selected-file preview. The session host owns and deletes the bitmap; the pane
			// borrows it only while painting, just as it borrows the folder icon above.
			void SetThumbnail(HBITMAP bitmap) noexcept { m_thumbnail = bitmap; }

			// FOLDER.HTT's #Status. True only while a Windows 98 thumbnail request has been
			// outstanding for longer than the template's one-second timeout.
			void SetThumbnailPending(bool pending) noexcept { m_thumbnailPending = pending; }
			void SetImgPreview(ImgPreviewState state, SIZE sourceSize = {}) noexcept
			{
				m_imgPreviewState = state;
				m_imgSourceSize = (state == ImgPreviewState::Image) ? sourceSize : SIZE{};
			}
			void SetImgToolbarHeight(int height) noexcept
			{
				m_imgToolbarHeight = height > 0 ? height : 0;
			}

			// The drawable image/status canvas inside the lower imgview.htt Preview object:
			// ThumbDiv's one-pixel frame and the supplied child-toolbar height are excluded.
			RECT ImgPreviewRect(const RECT &paneRect) const noexcept;

			// Drive-capacity pie, in used-parts-per-thousand, or -1 for none. Drawn below the
			// text in the 120x120 box #Thumbnail reserved.
			//
			// `boxHeight` overrides that square for the one state that changes it: Windows Me's
			// sysroot.htt sets `Thumbnail.style.height = 60` for its drive graph, which is why the
			// disc reads as a flatter ellipse there than anywhere else. Zero keeps 120.
			void SetCapacityPie(int usedPer1000, int boxHeight = 0) noexcept
			{
				m_capacityPer1000 = usedPer1000;
				m_capacityPieHeight = (boxHeight > 0) ? boxHeight : panemetrics::kThumbnailSize;
			}

			// True when the pane is wide enough to be shown at all.
			//
			// Both families collapse at the same number, which is a coincidence rather than a
			// shared rule: Windows 2000 collapses below twice its 200px panel, Windows 98 below
			// FixSize()'s literal 400px threshold.
			static bool VisibleAt(int viewWidth) noexcept
			{
				return viewWidth >= panemetrics::kCollapseBelow;
			}
			int PanelWidthFor(int viewWidth) const noexcept;

			// The Windows 98 narrow-mode banner: a full-width 32px strip carrying the folder
			// name, in place of the side panel. Called by the host, which owns the rectangle.
			void PaintWin98MiniBanner(HDC dc, const RECT &bannerRect) noexcept;

			// Height of the Windows 98 mini banner for this view width, or 0.
			//
			// This is the part with no NT5 equivalent. Below the threshold Windows 2000 simply
			// hides the pane and gives the list the whole client; Windows 98 replaces the side
			// panel with a full-width 32px banner carrying the folder name, and pushes the list
			// *down* by that much (FOLDER.HTT:60-65).
			int MiniBannerHeightFor(int viewWidth) const noexcept
			{
				if (m_visualProfile != WebViewVisualProfile::Windows98)
				{
					return 0;
				}
				return (viewWidth < win98metrics::kCollapseBelow) ? win98metrics::kMiniBannerHeight : 0;
			}

			// `dpi` is the DPI of the window being painted, from GetDpiForWindow. Passing it in
			// rather than reading a global is the whole point: see EnsureFonts.
			void Paint(HDC dc, const RECT &paneRect, UINT dpi) noexcept;

			// Index of the link under `pt` (pane client coordinates), or -1. Valid only after a
			// Paint has run, because that is what fills PaneLine::bounds. Recycle Bin buttons
			// share this hit-test path but remain visually and semantically distinct from links.
			int HitTestLink(POINT pt) const noexcept;
            int HitTestTooltip(POINT pt) const noexcept {
                for(size_t i=0;i<m_lines.size();++i) if(!m_lines[i].tooltip.empty() &&
                    PtInRect(&m_lines[i].tooltipBounds,pt)) return static_cast<int>(i);
                return -1;
            }
			int FindButtonByAccessKey(wchar_t key) const noexcept;
			bool SetPressedButton(int index) noexcept
			{
				if (m_pressedButton == index)
				{
					return false;
				}
				m_pressedButton = index;
				return true;
			}

			const PaneLine *Line(int index) const noexcept
			{
				return (index >= 0 && static_cast<size_t>(index) < m_lines.size()) ? &m_lines[index] : nullptr;
			}
			size_t LineCount() const noexcept { return m_lines.size(); }

		private:
			void EnsureFonts(UINT dpi) noexcept;
			void ReleaseFonts() noexcept;
			void ReleaseResources() noexcept;
			void PaintImgPreview(HDC dc, const RECT &paneRect) noexcept;
            void PaintBody(HDC dc,const RECT& paneRect,UINT dpi,bool detailsOnly,int dividerWidth=0) noexcept;
            SIZE m_imgDetailsExtent{};
            bool m_compactImgHeader=false, m_scrollImgDetails=true;

			// Windows 98's whole pane. Kept apart from Paint rather than folded into it with
			// flags: the two share no vertical structure — different margins, a different
			// heading size, a 1px divider instead of 2px, and no See-also or Message block at
			// all — so interleaving them would make each harder to check against its own
			// template. It also guarantees the accepted Windows 2000 output cannot move.
			void PaintWin98(HDC dc, const RECT &paneRect) noexcept;

			HICON m_customCornerIcon=nullptr;
            COLORREF m_backgroundColour=CLR_INVALID,m_textColour=CLR_INVALID,m_headingColour=CLR_INVALID,m_linkColour=CLR_INVALID;
            bool m_pictureWhiteBlend=false;
            HBITMAP m_customCorner=nullptr; // borrowed from the mod cache
            SIZE m_customCornerSize{};
            ReferenceDivider m_divider;
            int m_customCornerWidth=0;
            bool m_showDivider=true, m_showHeader=true, m_showDimensions=true, m_customCornerAlpha=false, m_useProfileCorner=false;
            HBITMAP m_leftBitmap = nullptr;   // embedded wvleft.bmp/gif
			HBITMAP m_lineBitmap = nullptr;   // embedded wvline.gif
			HBITMAP m_netBitmap = nullptr;    // embedded nethood.htt #Brand artwork
			HBITMAP m_logoBitmap = nullptr;   // embedded system-folder #Brand artwork
			SIZE m_leftSize{};
			SIZE m_lineSize{};
			SIZE m_netSize{};
			SIZE m_logoSize{};

			HFONT m_bodyFont = nullptr;       // font: menu
			HFONT m_headingFont = nullptr;    // 13pt bold, same face
			HFONT m_boldFont = nullptr;       // body weight bumped, for the selected item's name
			HFONT m_linkFont = nullptr;       // body underlined
			HFONT m_boldLinkFont = nullptr;   // nethood.htt wraps a UNC anchor in <b>
			HFONT m_buttonFont = nullptr;     // button {font: 8pt Tahoma}
			HFONT m_descriptionFont=nullptr; // system tooltip/status font
            HFONT m_imgStatusFont = nullptr;  // SPI_GETICONTITLELOGFONT, as shimgvw uses
			int m_bodyHeight = 0;

			// The DPI the current fonts were built for. Metrics are per-DPI on Windows 10, so a
			// cached font is only valid while this matches the window.
			UINT m_fontDpi = 0;

			HICON m_folderIcon = nullptr;     // not owned
			HBITMAP m_thumbnail = nullptr;     // not owned; DiagnosticLeaseHost owns it
			bool m_thumbnailPending = false;   // FOLDER.HTT #Status, after its one-second timeout
			PaneProfile m_profile = PaneProfile::Standard;

			// Windows Me differs from Windows 2000 here by exactly two things in an LTR layout:
			// `background: window URL(wvleft.gif)` in place of `background: white URL(wvleft.bmp)`,
			// and `color: windowtext` in place of `color: black` (folder.htt:33 vs 34, and the
			// webview.css body rule). Everything else in its ordinary-folder template — geometry,
			// typography, the 200px panel, the collapse expression — is the Windows 2000 layout,
			// which is why Me shares the NT5 renderer rather than getting its own.
			WebViewVisualProfile m_visualProfile = WebViewVisualProfile::Windows2000;
			bool m_fontSmoothingDisabled = false;

			BarricadeMode m_barricade = BarricadeMode::None;
			BarricadeArt m_barricadeArt = BarricadeArt::Network;
			ImgPreviewState m_imgPreviewState = ImgPreviewState::NoSelection;
			SIZE m_imgSourceSize{};
			int m_imgToolbarHeight = 0;
			std::wstring m_folderName;
			std::vector<PaneLine> m_lines;
			int m_pressedButton = -1;
			bool m_resourcesLoaded = false;

			// Used parts-per-thousand for the capacity pie, or -1 when the selection is not a
			// drive. Kept as per-thousand rather than a percentage because that is the unit
			// CThumbCtl computed and Draw3dPie consumes.
			int m_capacityPer1000 = -1;

			// Height of the box the pie is drawn into. 120 everywhere except Windows Me's
			// sysroot.htt drive graph, which uses 60.
			int m_capacityPieHeight = panemetrics::kThumbnailSize;
	};
} // namespace win2kwebview
} // namespace ce
namespace ce
{
namespace win2kwebview
{
	namespace
	{
		using namespace panemetrics;

		// The pane's link colour. standard.htt does not restyle anchors, so they are the
		// browser default, which the reference captures confirm as pure blue with an underline.
		constexpr int kButtonMarginLeft = 12; // button {margin-left: 12px}
		constexpr int kButtonHeight = 20;     // measured SP4/IE6 Restore button at 96 DPI
		constexpr int kButtonExtraWidth = 19; // measured 57px box around the 38px Restore text

		struct PaneAssetSet
		{
			UINT left;
			UINT line;
			UINT network;
			UINT logo;
			const wchar_t *leftName;
			const wchar_t *networkName;
			const wchar_t *logoName;
		};

		PaneAssetSet AssetSetFor(WebViewVisualProfile profile) noexcept
		{
			switch (profile)
			{
				case WebViewVisualProfile::Windows98:
					// Windows 98 nethood.htt has no NT5 soft barrier, but its protected system-folder
					// template uses WVLOGO.GIF for the Show Files barricade.
					return { IDR_WEBVIEW_98_WVLEFT, IDR_WEBVIEW_98_WVLINE, 0,
					         IDR_WEBVIEW_98_WVLOGO, L"wvleft.bmp", L"", L"WVLOGO.GIF" };

				case WebViewVisualProfile::WindowsME:
					// Me nethood.htt and sysroot.htt both use wvlogo.gif. Load separate HBITMAP
					// copies because ReleaseResources owns the network and logo handles independently.
					return { IDR_WEBVIEW_ME_WVLEFT, IDR_WEBVIEW_ME_WVLINE,
					         IDR_WEBVIEW_ME_WVLOGO, IDR_WEBVIEW_ME_WVLOGO,
					         L"wvleft.gif", L"wvlogo.gif", L"wvlogo.gif" };

				case WebViewVisualProfile::Windows2000:
				default:
					return { IDR_WEBVIEW_2K_WVLEFT, IDR_WEBVIEW_2K_WVLINE,
					         IDR_WEBVIEW_2K_WVNET, IDR_WEBVIEW_2K_WVLOGO,
					         L"wvleft.bmp", L"wvnet.gif", L"wvlogo.gif" };
			}
		}

		CComPtr<IStream> OpenEmbeddedAsset(UINT resourceId) noexcept {
            CComPtr<IStream> stream;
            auto bytes=ReferenceAssetBytes(resourceId);
            if (!bytes.empty()) stream.Attach(SHCreateMemStream(bytes.data(),static_cast<UINT>(bytes.size())));
            return stream;
        }

		/*
		 * Loads an embedded image into an HBITMAP through OLE.
		 *
		 * OleLoadPicture handles BMP, GIF, and JPEG, which covers every asset the templates
		 * reference, and it is the same decoder path the era used. RCDATA preserves the original
		 * file bytes, so the artwork is consumed as shipped rather than converted at build time.
		 */
		HBITMAP LoadImageResource(UINT resourceId, SIZE &size) noexcept
		{
			size = {};

			CComPtr<IStream> stream = OpenEmbeddedAsset(resourceId);
			if (!stream)
			{
				return nullptr;
			}

			CComPtr<IPicture> picture;
			if (FAILED(OleLoadPicture(stream, 0, FALSE, IID_IPicture, reinterpret_cast<void **>(&picture))) ||
			    !picture)
			{
				return nullptr;
			}

			OLE_HANDLE handle = 0;
			if (FAILED(picture->get_Handle(&handle)) || !handle)
			{
				return nullptr;
			}

			// IPicture owns its bitmap and will free it on release, so take an unconditional copy.
			const HBITMAP source = reinterpret_cast<HBITMAP>(static_cast<UINT_PTR>(handle));
			BITMAP info{};
			if (!GetObjectW(source, sizeof(info), &info))
			{
				return nullptr;
			}

			size.cx = info.bmWidth;
			size.cy = info.bmHeight;

			return static_cast<HBITMAP>(CopyImage(source, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
		}

		/*
		 * Loads an embedded image as a 32-bit premultiplied DIB, so a transparent GIF stays
		 * transparent.
		 *
		 * OleLoadPicture flattens a GIF's transparent palette index to that entry's own RGB. For
		 * Windows Me's wvleft.gif that index is (192,192,192), which is why the corner artwork
		 * otherwise arrives as a grey block instead of letting `background: window` show through.
		 */
		HBITMAP LoadTransparentImageResource(UINT resourceId, SIZE &size) noexcept
		{
			size = {};

			CComPtr<IStream> stream = OpenEmbeddedAsset(resourceId);
			if (!stream)
			{
				return nullptr;
			}

			CComPtr<IWICImagingFactory> factory;
			if (FAILED(factory.CoCreateInstance(CLSID_WICImagingFactory)) || !factory)
			{
				return nullptr;
			}

			CComPtr<IWICBitmapDecoder> decoder;
			if (FAILED(factory->CreateDecoderFromStream(stream, nullptr,
			                                            WICDecodeMetadataCacheOnLoad, &decoder)) ||
			    !decoder)
			{
				return nullptr;
			}

			CComPtr<IWICBitmapFrameDecode> frame;
			if (FAILED(decoder->GetFrame(0, &frame)) || !frame)
			{
				return nullptr;
			}

			UINT width = 0;
			UINT height = 0;
			if (FAILED(frame->GetSize(&width, &height)) || width == 0 || height == 0 ||
			    width > static_cast<UINT>((std::numeric_limits<LONG>::max)()) ||
			    height > static_cast<UINT>((std::numeric_limits<LONG>::max)()) ||
			    width > (std::numeric_limits<UINT>::max)() / 4)
			{
				return nullptr;
			}

			const UINT stride = width * 4;
			if (height > (std::numeric_limits<UINT>::max)() / stride)
			{
				return nullptr;
			}

			CComPtr<IWICFormatConverter> converter;
			if (FAILED(factory->CreateFormatConverter(&converter)) || !converter ||
			    FAILED(converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA,
			                                 WICBitmapDitherTypeNone, nullptr, 0.0,
			                                 WICBitmapPaletteTypeCustom)))
			{
				return nullptr;
			}

			// Top-down, so the DIB rows match WIC's order without a flip.
			BITMAPINFO info{};
			info.bmiHeader.biSize = sizeof(info.bmiHeader);
			info.bmiHeader.biWidth = static_cast<LONG>(width);
			info.bmiHeader.biHeight = -static_cast<LONG>(height);
			info.bmiHeader.biPlanes = 1;
			info.bmiHeader.biBitCount = 32;
			info.bmiHeader.biCompression = BI_RGB;

			void *bits = nullptr;
			const HBITMAP decoded = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
			if (!decoded || !bits)
			{
				if (decoded)
				{
					DeleteObject(decoded);
				}
				return nullptr;
			}

			if (FAILED(converter->CopyPixels(nullptr, stride, stride * height,
			                                 static_cast<BYTE *>(bits))))
			{
				DeleteObject(decoded);
				return nullptr;
			}

			// Only claim the image when it actually has transparency. Opaque assets are handed
			// back to OleLoadPicture so accepted Windows 2000 output stays on its historical path.
			bool transparent = false;
			const BYTE *pixels = static_cast<const BYTE *>(bits);
			for (UINT row = 0; row < height && !transparent; ++row)
			{
				const BYTE *pixel = pixels + static_cast<size_t>(row) * stride;
				for (UINT column = 0; column < width; ++column, pixel += 4)
				{
					if (pixel[3] != 0xFF)
					{
						transparent = true;
						break;
					}
				}
			}

			if (!transparent)
			{
				DeleteObject(decoded);
				return nullptr;
			}

			size.cx = static_cast<LONG>(width);
			size.cy = static_cast<LONG>(height);
			return decoded;
		}

		// Transparency-aware first, historical decoder second.
		HBITMAP LoadPaneImageResource(UINT resourceId, SIZE &size) noexcept
		{
			if (!resourceId)
			{
				size = {};
				return nullptr;
			}
			if (const HBITMAP transparent = LoadTransparentImageResource(resourceId, size))
			{
				return transparent;
			}
			return LoadImageResource(resourceId, size);
		}

		void DrawBitmapAt(HDC dc, HBITMAP bitmap, const SIZE &size, int x, int y, int width = -1, int height = -1, bool alpha=true, bool blendWhite=false, COLORREF background=RGB(255,255,255)) noexcept
		{
			if (!bitmap || size.cx <= 0 || size.cy <= 0)
			{
				return;
			}

			const HDC memory = CreateCompatibleDC(dc);
			if (!memory)
			{
				return;
			}

			const HGDIOBJ previous = SelectObject(memory, bitmap);
			const int targetWidth = (width < 0) ? size.cx : width;
            const int targetHeight=(height<0) ? size.cy : height;
            if(!alpha && blendWhite && background!=RGB(255,255,255)) {
                // A bitmap can be selected into only one memory DC at a time.
                SelectObject(memory,previous);
                if(DrawReferenceWhiteBlend(dc,bitmap,size,x,y,targetWidth,targetHeight,background)) {
                    DeleteDC(memory);return;
                }
                SelectObject(memory,bitmap);
            }

			// A 32-bit source came from LoadTransparentImageResource and carries real alpha, so it
			// composites over whatever the caller already filled — which is how Windows Me's
			// wvleft.gif lets the panel's `background: window` show through its transparent
			// index. Anything else is the historical opaque blit, unchanged.
			BOOL drawn = FALSE;
			BITMAP info{};
			if (GetObjectW(bitmap, sizeof(info), &info) && info.bmBitsPixel == 32 && alpha)
			{
				BLENDFUNCTION blend{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
				drawn = AlphaBlend(dc, x, y, targetWidth, targetHeight, memory, 0, 0,
				                   size.cx, size.cy, blend);
			}

			if (!drawn)
			{
				if (targetWidth == size.cx && targetHeight == size.cy)
				{
					BitBlt(dc, x, y, size.cx, size.cy, memory, 0, 0, SRCCOPY);
				}
				else
				{
					// #LogoLine is styled width:100%, so the divider is stretched across the
					// panel rather than tiled.
					StretchBlt(dc, x, y, targetWidth, targetHeight, memory, 0, 0, size.cx, size.cy, SRCCOPY);
				}
			}

			SelectObject(memory, previous);
			DeleteDC(memory);
		}

		bool IsHeadingWhitespace(wchar_t value) noexcept
		{
			return value == L' ' || value == L'\t' || value == L'\r' || value == L'\n';
		}

		std::wstring ButtonCaption(const PaneLine &line)
		{
			std::wstring caption = line.text;
			if (!line.buttonAccessKey)
			{
				return caption;
			}

			const wchar_t wanted = static_cast<wchar_t>(towupper(line.buttonAccessKey));
			for (size_t index = 0; index < caption.size(); ++index)
			{
				if (towupper(caption[index]) == wanted)
				{
					caption.insert(index, 1, L'&');
					break;
				}
			}
			return caption;
		}

		int DrawFolderHeading(HDC dc, const std::wstring &heading, const RECT &bounds) noexcept
		{
			if (heading.empty() || bounds.right <= bounds.left)
			{
				return 1;
			}

			const int availableWidth = bounds.right - bounds.left;
			const size_t length = heading.size();
			size_t start = 0;
			int lineCount = 0;

			while (start < length)
			{
				while (start < length && IsHeadingWhitespace(heading[start]))
				{
					++start;
				}
				if (start >= length)
				{
					break;
				}

				size_t paragraphEnd = start;
				while (paragraphEnd < length && heading[paragraphEnd] != L'\r' && heading[paragraphEnd] != L'\n')
				{
					++paragraphEnd;
				}

				const int remaining = static_cast<int>(paragraphEnd - start);
				int fit = remaining;
				SIZE measured{};
				if (!GetTextExtentExPointW(dc, heading.c_str() + start, remaining, availableWidth,
				                            &fit, nullptr, &measured))
				{
					fit = remaining;
				}

				size_t lineEnd = paragraphEnd;
				if (fit < remaining)
				{
					// IE5's normal white-space handling wraps at the last word boundary that fits.
					// An unbroken word is allowed to overflow the line and is clipped by the panel.
					size_t boundary = start + static_cast<size_t>(fit);
					while (boundary > start && !IsHeadingWhitespace(heading[boundary - 1]))
					{
						--boundary;
					}
					if (boundary > start)
					{
						lineEnd = boundary;
					}
					else
					{
						lineEnd = start;
						while (lineEnd < paragraphEnd && !IsHeadingWhitespace(heading[lineEnd]))
						{
							++lineEnd;
						}
					}
				}

				size_t drawEnd = lineEnd;
				while (drawEnd > start && IsHeadingWhitespace(heading[drawEnd - 1]))
				{
					--drawEnd;
				}

				RECT lineBounds = bounds;
				lineBounds.top += lineCount * kFolderNameLineAdvance;
				DrawTextW(dc, heading.c_str() + start, static_cast<int>(drawEnd - start), &lineBounds,
				          DT_LEFT | DT_NOPREFIX | DT_SINGLELINE);
				++lineCount;
				start = lineEnd;
			}

			return lineCount > 0 ? lineCount : 1;
		}

		void DrawThumbnailAt(HDC dc, HBITMAP bitmap, const RECT &box) noexcept
		{
			// folder.htt gives the control a 120x120 COLOR_WINDOW surface. The extractor
			// returns an aspect-fitted bitmap; keep smaller results at their natural size and
			// centre them rather than stretching them up or cropping them.
			FillReferenceSystemColour(dc,box,COLOR_WINDOW);
			if (!bitmap)
			{
				return;
			}

			BITMAP info{};
			if (!GetObjectW(bitmap, sizeof(info), &info))
			{
				return;
			}

			const int sourceWidth = abs(info.bmWidth);
			const int sourceHeight = abs(info.bmHeight);
			const int boxWidth = box.right - box.left;
			const int boxHeight = box.bottom - box.top;
			if (sourceWidth <= 0 || sourceHeight <= 0 || boxWidth <= 0 || boxHeight <= 0)
			{
				return;
			}

			int targetWidth = sourceWidth;
			int targetHeight = sourceHeight;
			if (targetWidth > boxWidth || targetHeight > boxHeight)
			{
				if (static_cast<long long>(sourceWidth) * boxHeight >
				    static_cast<long long>(sourceHeight) * boxWidth)
				{
					targetWidth = boxWidth;
					targetHeight = max(1, MulDiv(sourceHeight, boxWidth, sourceWidth));
				}
				else
				{
					targetHeight = boxHeight;
					targetWidth = max(1, MulDiv(sourceWidth, boxHeight, sourceHeight));
				}
			}

			const int x = box.left + (boxWidth - targetWidth) / 2;
			const int y = box.top + (boxHeight - targetHeight) / 2;
			const HDC memory = CreateCompatibleDC(dc);
			if (!memory)
			{
				return;
			}

			const HGDIOBJ previous = SelectObject(memory, bitmap);

			// Shell thumbnails are commonly 32-bit premultiplied DIBs. Use their alpha only
			// when the bitmap actually carries non-zero alpha; older extractors often return
			// 32-bit RGB with the alpha byte cleared, which must remain an ordinary blit.
			bool hasAlpha = false;
			if (info.bmBitsPixel == 32 && info.bmBits && info.bmWidthBytes > 0)
			{
				const auto *bits = static_cast<const BYTE *>(info.bmBits);
				for (int row = 0; row < sourceHeight && !hasAlpha; ++row)
				{
					const BYTE *pixel = bits + static_cast<size_t>(row) * info.bmWidthBytes;
					for (int column = 0; column < sourceWidth; ++column, pixel += 4)
					{
						if (pixel[3] != 0)
						{
							hasAlpha = true;
							break;
						}
					}
				}
			}

			BOOL drawn = FALSE;
			if (hasAlpha)
			{
				BLENDFUNCTION blend{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
				drawn = AlphaBlend(dc, x, y, targetWidth, targetHeight, memory, 0, 0,
				                   sourceWidth, sourceHeight, blend);
			}
			if (!drawn)
			{
				if (targetWidth == sourceWidth && targetHeight == sourceHeight)
				{
					BitBlt(dc, x, y, targetWidth, targetHeight, memory, 0, 0, SRCCOPY);
				}
				else
				{
					const int oldMode = SetStretchBltMode(dc, COLORONCOLOR);
					StretchBlt(dc, x, y, targetWidth, targetHeight, memory, 0, 0,
					           sourceWidth, sourceHeight, SRCCOPY);
					SetStretchBltMode(dc, oldMode);
				}
			}

			SelectObject(memory, previous);
			DeleteDC(memory);
		}
	} // namespace

	WebViewNativePane::~WebViewNativePane()
	{
		ReleaseResources();
	}

	void WebViewNativePane::ReleaseResources() noexcept
	{
		if (m_leftBitmap) { DeleteObject(m_leftBitmap); m_leftBitmap = nullptr; }
		if (m_lineBitmap) { DeleteObject(m_lineBitmap); m_lineBitmap = nullptr; }
		if (m_netBitmap) { DeleteObject(m_netBitmap); m_netBitmap = nullptr; }
		if (m_logoBitmap) { DeleteObject(m_logoBitmap); m_logoBitmap = nullptr; }
		m_leftSize = {};
		m_lineSize = {};
		m_netSize = {};
		m_logoSize = {};
		ReleaseFonts();
		m_resourcesLoaded = false;
	}

	void WebViewNativePane::SetVisualProfile(WebViewVisualProfile profile) noexcept
	{
		if (m_visualProfile == profile)
		{
			return;
		}
		m_visualProfile = profile;
		// The asset set is profile-specific, so anything already loaded belongs to the old one.
		ReleaseResources();
	}

	void WebViewNativePane::SetFontSmoothingDisabled(bool disabled) noexcept
	{
		if (m_fontSmoothingDisabled == disabled)
		{
			return;
		}
		m_fontSmoothingDisabled = disabled;
		// Quality is baked into the LOGFONT, so the cached handles no longer describe what was
		// asked for. Dropping them makes the next paint rebuild at the new quality.
		ReleaseFonts();
	}

	void WebViewNativePane::EnsureResources() noexcept
	{
		if (m_resourcesLoaded)
		{
			return;
		}
		m_resourcesLoaded = true;

		const PaneAssetSet assets = AssetSetFor(m_visualProfile);
		m_leftBitmap = LoadPaneImageResource(assets.left, m_leftSize);
		m_lineBitmap = LoadPaneImageResource(assets.line, m_lineSize);
		m_netBitmap = LoadPaneImageResource(assets.network, m_netSize);
		m_logoBitmap = LoadPaneImageResource(assets.logo, m_logoSize);

		JsonFields fields;
		fields.Num(L"visualProfile", static_cast<unsigned long long>(m_visualProfile))
		      .Str(L"assetSource", L"embedded RCDATA")
		      .Str(L"cornerAsset", assets.leftName)
		      .Num(L"cornerResourceId", assets.left)
		      .Bool(L"wvleftLoaded", m_leftBitmap != nullptr)
		      .Num(L"wvleftWidth", static_cast<unsigned long long>(m_leftSize.cx))
		      .Num(L"wvleftHeight", static_cast<unsigned long long>(m_leftSize.cy))
		      .Num(L"lineResourceId", assets.line)
		      .Bool(L"wvlineLoaded", m_lineBitmap != nullptr)
		      .Num(L"wvlineHeight", static_cast<unsigned long long>(m_lineSize.cy))
		      .Str(L"networkAsset", assets.networkName)
		      .Num(L"networkResourceId", assets.network)
		      .Bool(L"wvnetLoaded", m_netBitmap != nullptr)
		      .Num(L"wvnetWidth", static_cast<unsigned long long>(m_netSize.cx))
		      .Num(L"wvnetHeight", static_cast<unsigned long long>(m_netSize.cy))
		      .Str(L"logoAsset", assets.logoName)
		      .Num(L"logoResourceId", assets.logo)
		      .Bool(L"wvlogoLoaded", m_logoBitmap != nullptr)
		      .Num(L"wvlogoWidth", static_cast<unsigned long long>(m_logoSize.cx))
		      .Num(L"wvlogoHeight", static_cast<unsigned long long>(m_logoSize.cy));
		EvidenceLog::Instance().Write(L"pane.resources", 0, fields);
	}

	void WebViewNativePane::ReleaseFonts() noexcept
	{
		if (m_bodyFont) { DeleteObject(m_bodyFont); m_bodyFont = nullptr; }
		if (m_headingFont) { DeleteObject(m_headingFont); m_headingFont = nullptr; }
		if (m_boldFont) { DeleteObject(m_boldFont); m_boldFont = nullptr; }
		if (m_linkFont) { DeleteObject(m_linkFont); m_linkFont = nullptr; }
		if (m_boldLinkFont) { DeleteObject(m_boldLinkFont); m_boldLinkFont = nullptr; }
		if (m_buttonFont) { DeleteObject(m_buttonFont); m_buttonFont = nullptr; }
		if (m_imgStatusFont) { DeleteObject(m_imgStatusFont); m_imgStatusFont = nullptr; }
		if(m_descriptionFont) DeleteObject(std::exchange(m_descriptionFont,nullptr));
        m_fontDpi = 0;
	}

	/*
	 * EnsureFonts: builds the pane fonts for a specific DPI.
	 *
	 * The first version of this used SystemParametersInfoW(SPI_GETNONCLIENTMETRICS) and took the
	 * heading's pixel height from GetDeviceCaps(GetDC(nullptr), LOGPIXELSY). Both are wrong on
	 * Windows 10 for a window inside Explorer:
	 *
	 *   - SPI_GETNONCLIENTMETRICS answers for the system DPI, not the window's;
	 *   - GetDC(nullptr) is the primary monitor, not the monitor the window is on.
	 *
	 * Explorer is per-monitor DPI aware, so those two sources could disagree with each other and
	 * with the actual window — and could change underneath a cached font when the window moved
	 * or the scaling changed. The visible symptom was the pane's font appearing to change at
	 * random.
	 *
	 * SystemParametersInfoForDpi and GetDpiForWindow (Windows 10 1607+) are the DPI-correct
	 * pair. They are resolved dynamically so an older target degrades to the system-DPI
	 * behaviour instead of failing to load.
	 */
	void WebViewNativePane::EnsureFonts(UINT dpi) noexcept
	{
		if (dpi == 0)
		{
			dpi = USER_DEFAULT_SCREEN_DPI;
		}

		// A cached font is only valid for the DPI it was built at.
		if (m_bodyFont && m_fontDpi == dpi)
		{
			return;
		}
		ReleaseFonts();

		using SystemParametersInfoForDpiFn = BOOL(WINAPI *)(UINT, UINT, PVOID, UINT, UINT);
		static const auto forDpi = reinterpret_cast<SystemParametersInfoForDpiFn>(
			GetProcAddress(GetModuleHandleW(L"user32.dll"), "SystemParametersInfoForDpi"));

		NONCLIENTMETRICSW metrics{};
		metrics.cbSize = sizeof(metrics);

		const BOOL gotMetrics = forDpi
			? forDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0, dpi)
			: SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);
		if (!gotMetrics)
		{
			return;
		}

		// `font: menu` in CSS2 is the system menu font — lfMenuFont of NONCLIENTMETRICS. Taking
		// it from the system rather than naming a face is what makes the pane follow the user's
		// metrics the way the original did.
		//
		// Preserve lfQuality. With DEFAULT_QUALITY, GDI follows the user's system font-smoothing
		// setting, just as IE5 did. The original jagged reference capture came from a VM where
		// "Smooth edges of screen fonts" was disabled; it is not a property of Folder WebView.
		//
		// The exception is an explicit request to turn smoothing off for this pane only. That is
		// a deliberate override of the rule above, not a correction to it: it reproduces the
		// un-smoothed look on a machine where smoothing is on system-wide, without touching the
		// system setting. Applied to lfMenuFont before anything derives from it, so every font
		// below inherits it.
		if (m_fontSmoothingDisabled)
		{
			metrics.lfMenuFont.lfQuality = NONANTIALIASED_QUALITY;
		}

		// Windows 98 does not say `font: menu`. Its body rule is `font: 8pt/10pt verdana`, a
		// named face at a fixed size, so this pane deliberately does *not* follow the user's
		// metrics the way the NT5 profiles do. Only the face, size and weight are replaced; the
		// rest of the LOGFONT — charset, quality, clipping — stays as the system supplied it, so
		// the smoothing rule above still applies.
		const bool win98 = m_visualProfile == WebViewVisualProfile::Windows98;
		if (win98)
		{
			StringCchCopyW(metrics.lfMenuFont.lfFaceName, ARRAYSIZE(metrics.lfMenuFont.lfFaceName),
			               win98metrics::kFaceName);
			metrics.lfMenuFont.lfHeight =
				-MulDiv(win98metrics::kBodyPt, static_cast<int>(dpi), 72);
			metrics.lfMenuFont.lfWeight = FW_NORMAL;
		}

		LOGFONTW description=metrics.lfStatusFont;
        if(m_fontSmoothingDisabled) description.lfQuality=NONANTIALIASED_QUALITY;
        m_descriptionFont=CreateFontIndirectW(&description);
        LOGFONTW body = metrics.lfMenuFont;
		m_bodyFont = CreateFontIndirectW(&body);
		m_bodyHeight = abs(metrics.lfMenuFont.lfHeight);

		// #FolderName {font: 13pt/13pt menu; font-weight: bold} — same face, 13 points, bold,
		// scaled against the window's own DPI rather than the primary monitor's. Its quality is
		// inherited from the same system menu font so it follows the smoothing option too.
		//
		// Windows 98's equivalent is p.Title {font: 16pt; font-weight: bold}, which inherits
		// Verdana from the body rule.
		LOGFONTW heading = metrics.lfMenuFont;
		heading.lfHeight = -MulDiv(win98 ? win98metrics::kTitlePt : kFolderNamePt,
		                           static_cast<int>(dpi), 72);
		heading.lfWeight = FW_BOLD;
		m_headingFont = CreateFontIndirectW(&heading);

		// ShowInfo() wraps the selected item's name in <b> (standard.htt:126) — body size, bold.
		LOGFONTW bold = body;
		bold.lfWeight = FW_BOLD;
		m_boldFont = CreateFontIndirectW(&bold);

		// Anchors carry no rule in webview.css, so they render as the IE5 default: blue and
		// underlined. Built once here rather than per-paint, which is what the old code did.
		LOGFONTW link = body;
		link.lfUnderline = TRUE;
		m_linkFont = CreateFontIndirectW(&link);

		LOGFONTW boldLink = link;
		boldLink.lfWeight = FW_BOLD;
		m_boldLinkFont = CreateFontIndirectW(&boldLink);

		// recycle.htt supplies an explicit 8pt Tahoma rule for its HTML buttons. Keep the
		// user's smoothing quality while replacing only the face, size and weight named by CSS.
		LOGFONTW button = body;
		StringCchCopyW(button.lfFaceName, ARRAYSIZE(button.lfFaceName), L"Tahoma");
		button.lfHeight = -MulDiv(8, static_cast<int>(dpi), 72);
		button.lfWeight = FW_NORMAL;
		button.lfUnderline = FALSE;
		m_buttonFont = CreateFontIndirectW(&button);

		// shimgvw's CZoomWnd::OnPaint asks USER for the icon-title LOGFONT each time it
		// draws a preview status. Cache that exact system font with the rest of this DPI
		// generation; its lfQuality deliberately continues to follow font smoothing, unless the
		// pane-wide override is on — "no smoothing in the WebView" has to include this text too,
		// or the preview status stays smoothed while everything around it is not.
		LOGFONTW iconTitle{};
		const BOOL gotIconTitleFont = forDpi ? forDpi(SPI_GETICONTITLELOGFONT, sizeof(iconTitle), &iconTitle, 0, dpi) :
            SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(iconTitle), &iconTitle, 0);
		if (gotIconTitleFont)
		{
			if (m_fontSmoothingDisabled)
			{
				iconTitle.lfQuality = NONANTIALIASED_QUALITY;
			}
			m_imgStatusFont = CreateFontIndirectW(&iconTitle);
		}

		m_fontDpi = dpi;

		JsonFields fields;
		fields.Num(L"dpi", dpi)
		      .Bool(L"usedForDpiApi", forDpi != nullptr)
		      .Str(L"menuFace", metrics.lfMenuFont.lfFaceName)
		      .Num(L"menuHeight", static_cast<unsigned long long>(abs(metrics.lfMenuFont.lfHeight)))
		      .Num(L"headingPt", kFolderNamePt)
		      .Num(L"headingHeight", static_cast<unsigned long long>(abs(heading.lfHeight)))
		      .Num(L"bodyQuality", static_cast<unsigned long long>(body.lfQuality))
		      .Num(L"headingQuality", static_cast<unsigned long long>(heading.lfQuality))
		      .Bool(L"imgStatusFont", m_imgStatusFont != nullptr);
		EvidenceLog::Instance().Write(L"pane.fonts", 0, fields);
	}

	void WebViewNativePane::SetFolder(const std::wstring &displayName, HICON icon) noexcept
	{
		m_folderName = displayName;
		m_folderIcon = icon;
	}

	void WebViewNativePane::SetLines(std::vector<PaneLine> lines) noexcept
	{
		m_pressedButton = -1;
		m_lines = std::move(lines);
	}

    int WebViewNativePane::PanelWidthFor(int viewWidth) const noexcept {
        if(m_visualProfile==WebViewVisualProfile::Windows98)
            return win98metrics::PanelWidthFor(viewWidth);
        // Preview changes only the contents, not the file list's origin.
        return VisibleAt(viewWidth) ? panemetrics::kPanelWidth : 0;
    }

	RECT WebViewNativePane::ImgPreviewRect(const RECT &paneRect) const noexcept
	{
		const int height = paneRect.bottom - paneRect.top;
		const int split = paneRect.top + ((height + 1) / 2);
		RECT preview{ paneRect.left + panemetrics::kImgFrameInset,
		              split + panemetrics::kImgFrameInset + m_imgToolbarHeight,
		              paneRect.right - panemetrics::kImgFrameInset,
		              paneRect.bottom - panemetrics::kImgFrameInset };
		if (preview.right < preview.left)
		{
			preview.right = preview.left;
		}
		if (preview.bottom < preview.top)
		{
			preview.top = preview.bottom;
		}
		return preview;
	}

	void WebViewNativePane::PaintImgPreview(HDC dc, const RECT &paneRect) noexcept
	{
		const int height = paneRect.bottom - paneRect.top;
		const int split = paneRect.top + ((height + 1) / 2);
		RECT frame{ paneRect.left, split, paneRect.right, paneRect.bottom };
		if (frame.right <= frame.left || frame.bottom <= frame.top)
		{
			return;
		}

		FillReferenceSystemColour(dc,frame,COLOR_WINDOWFRAME);
		RECT inner{ frame.left + panemetrics::kImgFrameInset,
		            frame.top + panemetrics::kImgFrameInset,
		            frame.right - panemetrics::kImgFrameInset,
		            frame.bottom - panemetrics::kImgFrameInset };
		FillReferenceColour(dc,inner,BackgroundColour());
		RECT canvas = ImgPreviewRect(paneRect);

		if (m_imgPreviewState == ImgPreviewState::Image && m_thumbnail)
		{
			DrawThumbnailAt(dc, m_thumbnail, canvas);
			return;
		}

		const wchar_t *status = nullptr;
		switch (m_imgPreviewState)
		{
			case ImgPreviewState::NoSelection: status = ReferenceCaption(L"No file selected."); break;
			case ImgPreviewState::Loading:     status = ReferenceCaption(L"Generating preview..."); break;
			case ImgPreviewState::Multiple:    status = ReferenceCaption(L"Multiple files selected."); break;
			case ImgPreviewState::Failed:      status = ReferenceCaption(L"No preview available."); break;
			case ImgPreviewState::Image:
				// An accepted image without a bitmap is a failed presentation, not a blank box.
				status = ReferenceCaption(L"No preview available.");
				break;
		}

		if (status && canvas.right > canvas.left && canvas.bottom > canvas.top)
		{
			const HFONT statusFont = m_imgStatusFont ? m_imgStatusFont : m_bodyFont;
			const HGDIOBJ previous = statusFont ? SelectObject(dc, statusFont) : nullptr;
			SetTextColor(dc, TextColour());
			SetBkColor(dc,BackgroundColour());
			DrawTextW(dc, status, -1, &canvas,
			          DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			if (previous)
			{
				SelectObject(dc, previous);
			}
		}
	}

	namespace
	{
		// Implemented below using sampled ellipse surfaces.
		void Draw3dPie(HDC hdc, RECT rc, DWORD dwPer1000) noexcept;
	}

	void WebViewNativePane::PaintBarricade(HDC dc, const RECT &brandRect) noexcept
	{
		// #Brand {background: <colour> URL(<art>) no-repeat right bottom}. The image is anchored,
		// not stretched, and a box smaller than the image clips it at the top and left — which is
		// what the browser does with a bottom-right anchor and no repeat.
		if (IsRectEmpty(&brandRect))
		{
			return;
		}

		FillReferenceColour(dc,brandRect,BackgroundColour());

		const HBITMAP art = (m_barricadeArt == BarricadeArt::Logo) ? m_logoBitmap : m_netBitmap;
		const SIZE artSize = (m_barricadeArt == BarricadeArt::Logo) ? m_logoSize : m_netSize;
		if (!art || artSize.cx <= 0 || artSize.cy <= 0)
		{
			return;
		}

		const int saved = SaveDC(dc);
		IntersectClipRect(dc, brandRect.left, brandRect.top, brandRect.right, brandRect.bottom);
		DrawBitmapAt(dc, art, artSize,
		             brandRect.right - artSize.cx, brandRect.bottom - artSize.cy);
		if (saved)
		{
			RestoreDC(dc, saved);
		}
	}

	void WebViewNativePane::PaintWin98MiniBanner(HDC dc, const RECT &bannerRect) noexcept
	{
        EnsureFonts(96);
		// <div ID="MiniBanner" style="position: absolute; width: 100%; height: 32px;
		//                             background: window">
		//   <table><tr><td nowrap><p class=Title style="margin-top: 0"> %THISDIRNAME%
		//
		// The inline margin-top:0 override is the only reason the title sits flush with the top
		// of the banner rather than 5px down, and `nowrap` is why a long folder name is clipped
		// rather than wrapped inside 32 pixels.
		if (IsRectEmpty(&bannerRect))
		{
			return;
		}

		FillReferenceColour(dc,bannerRect,BackgroundColour());
		if (m_folderName.empty() || !m_headingFont)
		{
			return;
		}

		const HGDIOBJ previous = SelectObject(dc, m_headingFont);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, HeadingColour());

		// The title paragraph lives inside <table><tr><td nowrap>, and in quirks mode the default
		// table and cell insets add one pixel on each axis on top of the paragraph's own 15px left
		// margin. Measured on the reference guest: the banner title box is at (16, 1), not (15, 0).
		RECT text{ bannerRect.left + win98metrics::kParagraphLeft + win98metrics::kMiniBannerCellInset,
		           bannerRect.top + win98metrics::kMiniBannerCellInset,
		           bannerRect.right, bannerRect.bottom };
		// No DT_END_ELLIPSIS: the template only asks for nowrap. Nothing in the reference capture
		// draws an ellipsis, so clipping is the authentic overflow behaviour.
		DrawTextW(dc, m_folderName.c_str(), -1, &text,
		          DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

		SelectObject(dc, previous);
	}

	void WebViewNativePane::PaintWin98(HDC dc, const RECT &paneRect) noexcept
	{
		// Deliberately aliased rather than pulled in with a using-directive: this file already
		// has `using namespace panemetrics` at namespace scope, and the two metric sets share
		// several names on purpose. Keeping the prefix means every constant below is visibly the
		// Windows 98 one.
		namespace w98 = win98metrics;

		FillReferenceColour(dc,paneRect,BackgroundColour());
		if(m_useProfileCorner) DrawBitmapAt(dc,m_leftBitmap,m_leftSize,paneRect.left,paneRect.top);
                else if(m_customCornerIcon) DrawIconEx(dc,paneRect.left,paneRect.top,m_customCornerIcon,
                    m_customCornerWidth>0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                    m_customCornerSize.cx>0 ? MulDiv(m_customCornerSize.cy,
                        m_customCornerWidth>0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                        m_customCornerSize.cx) : 0,0,nullptr,DI_NORMAL);
                else DrawBitmapAt(dc, m_customCorner, m_customCornerSize, paneRect.left, paneRect.top,
                    m_customCornerWidth > 0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                    m_customCornerSize.cx>0 ? MulDiv(m_customCornerSize.cy,
                        m_customCornerWidth>0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                        m_customCornerSize.cx) : 0,m_customCornerAlpha,m_pictureWhiteBlend,BackgroundColour());

		SetBkMode(dc, TRANSPARENT);

		const int contentLeft = paneRect.left + w98::kParagraphLeft;
		const int contentRight = paneRect.right - w98::kParagraphRight;
		if (contentRight <= contentLeft)
		{
			return;
		}

		// <p><object 32x32> — an ordinary paragraph, so the icon inherits p's 15px margins.
		int y = paneRect.top + w98::kParagraphTop;
		if (m_showHeader && m_folderIcon)
		{
			DrawIconEx(dc, contentLeft, y, m_folderIcon, w98::kFolderIconSize, w98::kFolderIconSize,
			           0, nullptr, DI_NORMAL);
		}
		y += w98::kFolderIconSize;

		// <p class=Title> {font: 16pt; font-weight: bold; margin-top: 5px}
		if (m_showHeader && !m_folderName.empty() && m_headingFont)
		{
			const HGDIOBJ previous = SelectObject(dc, m_headingFont);
			SetTextColor(dc, HeadingColour());

			y += w98::kTitleTop;
			RECT text{ contentLeft, y, contentRight, paneRect.bottom };
			// The side panel has room to wrap, unlike the mini banner.
			DrawTextW(dc, m_folderName.c_str(), -1, &text,
			          DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);

			// Advance by IE6's line box, not by what Windows 10 just rasterised. DrawText is asked
			// how many lines it wrapped into -- that part is genuinely a function of the current
			// font and width -- but each of those lines then occupies the measured 25px, so the
			// rule and everything under it land where the reference guest puts them.
			RECT measure{ contentLeft, y, contentRight, paneRect.bottom };
			const int drawn = DrawTextW(dc, m_folderName.c_str(), -1, &measure,
			                            DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);

			TEXTMETRICW tm{};
			GetTextMetricsW(dc, &tm);
			const int titleLines = (drawn > 0 && tm.tmHeight > 0)
			                       ? ((drawn + tm.tmHeight - 1) / tm.tmHeight)
			                       : 1;
			y += titleLines * w98::kTitleLineHeight;

			SelectObject(dc, previous);
		}

		// <p class=LogoLine> {margin-left: 0; margin-top: -5px; margin-right: 0;
		//                     margin-bottom: 20px} with <img width=100% height=1px>.
		// The negative top margin pulls the rule back under the title, and margin-left/right 0
		// is why this one element spans the full panel while everything else is inset by 15.
		y += w98::kLogoLineTop;
		if (m_showDivider && (m_lineBitmap || m_divider.configured))
		{
			// The rule is an inline image on a text baseline, so it sits 10px down inside the
			// paragraph's 13px line box rather than at its top edge.
			if(m_divider.configured) m_divider.Paint(dc,paneRect.left,y+w98::kLogoLineRuleOffset,
                paneRect.right-paneRect.left,1);
            else DrawBitmapAt(dc,m_lineBitmap,m_lineSize,paneRect.left,y+w98::kLogoLineRuleOffset,
                paneRect.right-paneRect.left);
		}
		y += w98::kLogoLineBoxHeight;

		// <p><span id=Info>. The rule's 20px bottom margin and the paragraph's 15px top margin
		// are adjoining, so they collapse to the larger of the two rather than summing.
		y += (w98::kLogoLineBottom > w98::kParagraphTop) ? w98::kLogoLineBottom : w98::kParagraphTop;

		if (!m_bodyFont)
		{
			return;
		}

		/*
		 * Inline runs.
		 *
		 * recycle.htt, printers.htt and dialup.htt all place a command anchor or a bold phrase
		 * *inside* a sentence:
		 *
		 *     To permanently remove all items and reclaim disk space, click
		 *     <a class=command ...>Empty Recycle Bin</a>.
		 *
		 * DrawText cannot change font part-way through a string, so such a line is laid out here
		 * as three runs in two fonts, wrapped word by word. Only the middle run's drawn rectangle
		 * becomes the hit rectangle, so the surrounding sentence stays unclickable -- rendering
		 * just `text` would drop the sentence entirely and leave a bare link.
		 */
		auto paintInlineRuns = [&](PaneLine &target, int startX) noexcept -> int
		{
			struct Run { const std::wstring *text; HFONT font; COLORREF colour; bool hit; };

			const bool linkRun = target.link;
			const std::wstring &prefix = linkRun ? target.linkPrefix : target.boldPrefix;
			const std::wstring &suffix = linkRun ? target.linkSuffix : target.boldSuffix;

			HFONT plain = m_bodyFont;
			// webview.css: `a.Command {font-weight: bold}`. Only the templates' command anchors
			// carry that class -- Empty Recycle Bin, Restore All, Restore, Connect, Show Files --
			// so they are bold *and* underlined. A plain <a>, such as Control Panel's Microsoft
			// Home or the Attributes and InfoTip links, is underlined only.
			HFONT accent = linkRun
				? (target.bold ? (m_boldLinkFont ? m_boldLinkFont : m_linkFont)
				               : (m_linkFont ? m_linkFont : m_bodyFont))
				: (m_boldFont ? m_boldFont : m_bodyFont);
			const COLORREF plainInk = TextColour();
			const COLORREF accentInk = linkRun
				? LinkColour()
				: (target.boldColor != CLR_INVALID ? target.boldColor : plainInk);

			const Run runs[3] = {
				{ &prefix,      plain,  plainInk,  false },
				{ &target.text, accent, accentInk, true  },
				{ &suffix,      plain,  plainInk,  false },
			};

			int x = startX;
			int lineTop = y;
			bool haveHit = false;
			RECT hit{};

			for (const Run &run : runs)
			{
				if (!run.text || run.text->empty())
				{
					continue;
				}
				SelectObject(dc, run.font);
				SetTextColor(dc, run.colour);

				const std::wstring &s = *run.text;
				size_t i = 0;
				while (i < s.size())
				{
					// One word plus the spaces that follow it, so a wrap lands between words and
					// the trailing space stays with the line it belongs to.
					size_t wordEnd = s.find(L' ', i);
					if (wordEnd == std::wstring::npos)
					{
						wordEnd = s.size();
					}
					else
					{
						while (wordEnd < s.size() && s[wordEnd] == L' ')
						{
							++wordEnd;
						}
					}

					const std::wstring piece = s.substr(i, wordEnd - i);
					SIZE extent{};
					GetTextExtentPoint32W(dc, piece.c_str(),
					                      static_cast<int>(piece.size()), &extent);

					if (x + extent.cx > contentRight && x > startX)
					{
						x = startX;
						lineTop += w98::kBodyLineHeight;
					}
					TextOutW(dc, x, lineTop, piece.c_str(), static_cast<int>(piece.size()));

					if (run.hit)
					{
						// Measure without the trailing spaces: they are not part of the anchor and
						// must not be clickable.
						std::wstring inked = piece;
						while (!inked.empty() && inked.back() == L' ')
						{
							inked.pop_back();
						}
						SIZE inkExtent{};
						GetTextExtentPoint32W(dc, inked.c_str(),
						                      static_cast<int>(inked.size()), &inkExtent);
						const RECT piecePos{ x, lineTop, x + inkExtent.cx,
						                     lineTop + w98::kBodyLineHeight };
						if (!haveHit)
						{
							hit = piecePos;
							haveHit = true;
						}
						else
						{
							if (piecePos.left < hit.left)     { hit.left = piecePos.left; }
							if (piecePos.top < hit.top)       { hit.top = piecePos.top; }
							if (piecePos.right > hit.right)   { hit.right = piecePos.right; }
							if (piecePos.bottom > hit.bottom) { hit.bottom = piecePos.bottom; }
						}
					}

					x += extent.cx;
					i = wordEnd;
				}
			}

			target.bounds = haveHit ? hit : RECT{};
			return lineTop + w98::kBodyLineHeight;
		};

		const HGDIOBJ previousFont = SelectObject(dc, m_bodyFont);
		for (PaneLine &line : m_lines)
		{
			line.tooltipBounds = RECT{};
			y += line.marginTop;
			if (y >= paneRect.bottom)
			{
				line.bounds = RECT{};
				continue;
			}

			// a.Command is bold as well as underlined; a plain anchor is underlined only.
			SelectObject(dc, line.link
				? (line.bold ? (m_boldLinkFont ? m_boldLinkFont : m_linkFont)
				             : (m_linkFont ? m_linkFont : m_bodyFont))
				: (line.bold ? m_boldFont : m_bodyFont));
			// p.Warning {font-weight: bold; color: red} is the only coloured body run Windows 98
			// uses, and the system-folder templates open with it.
			const COLORREF ink = line.link ? LinkColour()
			                   : (line.boldColor != CLR_INVALID ? line.boldColor
			                                                    : TextColour());
			SetTextColor(dc, ink);

			int lineX = contentLeft + line.indent;
			if (line.swatch != PaneLine::Swatch::None)
			{
				// MYCOMP.HTT:90-91:
				//   <table bgcolor=buttonface|buttonhighlight width=12 height=12 border=1
				//          bordercolordark=black bordercolorlight=black align=left><td></td></table>
				//   &nbsp;
				// Both border colours are black, so this is a flat 1px frame rather than a 3D
				// edge. Unlike the NT5 legend there is no .Legend class here, so the box starts at
				// the paragraph's own left margin with no extra indent.
				RECT box{ lineX, y, lineX + w98::kLegendSize, y + w98::kLegendSize };
				const int fill = (line.swatch == PaneLine::Swatch::Used)
					? COLOR_3DFACE : COLOR_3DHIGHLIGHT;
				FillReferenceSystemColour(dc,box,fill);
				FrameRect(dc, &box, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
				if (!line.tooltip.empty())
				{
					line.tooltipBounds = box;
				}
				// The "&nbsp;" between the table and the label.
				lineX = box.right + w98::kLegendGap;
			}

			// A line carrying prefix/suffix runs is laid out inline; everything else keeps the
			// single-run DrawText path, whose wrapping and spacing are already matched against
			// the reference captures.
			if (!line.linkPrefix.empty() || !line.linkSuffix.empty() ||
			    !line.boldPrefix.empty() || !line.boldSuffix.empty())
			{
				y = paintInlineRuns(line, lineX);
				if (line.link && !line.tooltip.empty())
				{
					line.tooltipBounds = line.bounds;
				}
				continue;
			}

			RECT text{ lineX, y, contentRight, paneRect.bottom };
			const UINT format = DT_LEFT | DT_NOPREFIX | DT_WORDBREAK;
			DrawTextW(dc, line.text.c_str(), -1, &text, format | DT_CALCRECT);
			line.bounds = text;
			if (line.link && !line.tooltip.empty())
			{
				line.tooltipBounds = line.bounds;
			}
			text.right = contentRight;
			DrawTextW(dc, line.text.c_str(), -1, &text, format);

			// A legend row is at least as tall as its swatch, or the next row would overlap it.
			y = (line.swatch != PaneLine::Swatch::None && text.bottom < y + w98::kLegendSize)
				? y + w98::kLegendSize : text.bottom;
		}
		SelectObject(dc, previousFont);

		/*
		 * FOLDER.HTT: <p><object id=Thumbnail> then <div id=Status>, both display:none until the
		 * control has something to show. MYCOMP.HTT: <p><object id=PieChart> in the same position.
		 * A given template has one or the other, never both.
		 */
		if (m_thumbnail)
		{
			// #Thumbnail {width: 160px; height: 160px; margin-top: 0px} -- no extra margin of its
			// own, unlike the pie, so only the paragraph's 15px applies.
			y += w98::kParagraphTop;
			RECT box{ contentLeft, y, contentLeft + w98::kThumbnailSize,
			          y + w98::kThumbnailSize };
			DrawThumbnailAt(dc, m_thumbnail, box);
		}
		else if (m_thumbnailPending && m_bodyFont)
		{
			// <div id=Status style="display: none">Generating preview...</div>, revealed by the
			// one-second setTimeout. #Status {margin-left: 15px}, and it is a div rather than a
			// paragraph, so it carries no top margin of its own.
			const HGDIOBJ previous = SelectObject(dc, m_bodyFont);
			SetTextColor(dc, TextColour());
			RECT status{ contentLeft, y + w98::kParagraphTop, contentRight, paneRect.bottom };
			DrawTextW(dc, ReferenceCaption(L"Generating preview..."), -1, &status,
			          DT_LEFT | DT_TOP | DT_NOPREFIX | DT_SINGLELINE);
			SelectObject(dc, previous);
		}
		else if (m_capacityPer1000 >= 0)
		{
			// <p><object id=PieChart> follows the Info span in its own paragraph
			// (MYCOMP.HTT:134-137), with #PieChart {width: 100px; height: 50px; margin-top: 10px}.
			// This is 100x50, not the NT5 120x120 square and not the Windows Me 60px graph.
			y += w98::kParagraphTop + w98::kPieMarginTop;
			RECT pie{ contentLeft, y, contentLeft + w98::kPieWidth, y + w98::kPieHeight };
			Draw3dPie(dc, pie, static_cast<DWORD>(m_capacityPer1000));
		}
	}

	void WebViewNativePane::PaintBody(HDC dc, const RECT &paneRect, UINT dpi, bool detailsOnly, int dividerWidth) noexcept
	{
		EnsureFonts(dpi);
        if(detailsOnly) m_imgDetailsExtent={paneRect.right-paneRect.left,0};

		// Windows 98 is a separate layout, not a restyled NT5 one; see PaintWin98.
		if (m_visualProfile == WebViewVisualProfile::Windows98)
		{
			PaintWin98(dc, paneRect);
			return;
		}
		const bool imgView = m_profile == PaneProfile::ImgView;
        const bool compactImg=imgView && m_compactImgHeader;

		// nethood.htt's narrow barricade: ResizeBarricade() stretches #Brand over the whole
		// client and moves the soft-barrier text into it, so #Panel and everything in it — the
		// folder icon, the heading and #LogoLine — end up covered rather than laid out.
		const bool barricadeFull = m_barricade == BarricadeMode::Full;
		const int paneHeight = paneRect.bottom - paneRect.top;
		const int contentBottom = detailsOnly && m_scrollImgDetails ? paneRect.top+1048576 :
            imgView ? (detailsOnly ? paneRect.bottom :
                paneRect.top+((paneHeight+1)/2)) : paneRect.bottom;

		if (barricadeFull)
		{
			PaintBarricade(dc, paneRect);
		}
		else
		{
			FillReferenceColour(dc,paneRect,BackgroundColour());
			if (!compactImg && (m_useProfileCorner || m_customCorner || m_customCornerIcon))
			{
				if(m_useProfileCorner) DrawBitmapAt(dc,m_leftBitmap,m_leftSize,paneRect.left,paneRect.top);
                else if(m_customCornerIcon) DrawIconEx(dc,paneRect.left,paneRect.top,m_customCornerIcon,
                    m_customCornerWidth>0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                    m_customCornerSize.cx>0 ? MulDiv(m_customCornerSize.cy,
                        m_customCornerWidth>0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                        m_customCornerSize.cx) : 0,0,nullptr,DI_NORMAL);
                else DrawBitmapAt(dc, m_customCorner, m_customCornerSize, paneRect.left, paneRect.top,
                    m_customCornerWidth > 0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                    m_customCornerSize.cx>0 ? MulDiv(m_customCornerSize.cy,
                        m_customCornerWidth>0 ? m_customCornerWidth : paneRect.right-paneRect.left,
                        m_customCornerSize.cx) : 0,m_customCornerAlpha,m_pictureWhiteBlend,BackgroundColour());
			}
		}

		// imgview.htt clips the upper Panel at 50%. Save the caller's DC so the lower
		// shimgvw surface can be drawn after restoring the unclipped region.
		const int upperDc = imgView && !detailsOnly ? SaveDC(dc) : 0;
		if (upperDc)
		{
			IntersectClipRect(dc, paneRect.left, paneRect.top, paneRect.right, contentBottom);
		}

		SetBkMode(dc, TRANSPARENT);

		// #Corner {padding-left: 12px; padding-top: 11px} holds the folder icon.
		int y = paneRect.top + (compactImg ? kImgFolderNameTop : kCornerPadTop);
		const int x = paneRect.left + kCornerPadLeft;
		const int flowRight = imgView ? paneRect.right
			: (((paneRect.left + kFlowRight) < paneRect.right)
				? paneRect.left + kFlowRight : paneRect.right);

		if (!compactImg && m_showHeader && !barricadeFull && m_folderIcon)
		{
			DrawIconEx(dc, x, y, m_folderIcon, kFolderIconSize, kFolderIconSize, 0, nullptr, DI_NORMAL);
		}

		// The updated SP4/IE6 probe fixes the one-line title box at y=51. A wrapped title adds
		// 17px per row even though Tahoma's GDI text cell is 21px high, because the template's
		// explicit 13pt line-height controls the line box. DrawText's built-in word wrapping
		// advances by tmHeight and is therefore four pixels too tall per additional row here.
		y = paneRect.top + (compactImg ? kImgFolderNameTop : kFolderNameTop);
		int folderNameLines = 1;
		if (m_showHeader && !m_folderName.empty() && m_headingFont && !barricadeFull)
		{
			const HGDIOBJ previous = SelectObject(dc, m_headingFont);
			SetTextColor(dc, HeadingColour());

			// Keep the measured line box at `y`; GDI's heading glyph ink sits two pixels below
			// the same Tahoma run in retail SP4/IE6.
			RECT text{ x, y + kFolderNameInkOffsetY, flowRight, contentBottom };
			folderNameLines = DrawFolderHeading(dc, m_folderName, text);

			SelectObject(dc, previous);
		}
		const int folderNameShift = (folderNameLines - 1) * kFolderNameLineAdvance;

		// #LogoLine {width: 100%; height: 2px; margin-top: 4px} — wvline.gif stretched across.
		y = paneRect.top + (compactImg ? kImgLogoLineTop : kLogoLineTop) + folderNameShift;
		if (m_showDivider && (m_lineBitmap || m_divider.configured) && !barricadeFull)
		{
			// Native scrollbars clip decoration; they do not rescale its colour positions.
            const int lineWidth=detailsOnly && dividerWidth>0 ? dividerWidth : flowRight-paneRect.left;
            if(m_divider.configured) m_divider.Paint(dc,paneRect.left,y,lineWidth,2);
            else DrawBitmapAt(dc,m_lineBitmap,m_lineSize,paneRect.left,y,lineWidth);
		}

		// #Details {padding-left: 12px; margin-top: 8px}, or #Brand's own
		// {padding-left: 12px} with no top padding once the barrier text has moved into it.
		y = barricadeFull
			? paneRect.top
			: paneRect.top + (compactImg ? kImgDetailsTop : kDetailsTop) + folderNameShift;
		const int detailsX = paneRect.left + (barricadeFull ? kBrandPadLeft : kDetailsPadLeft);
		const int detailsRight = barricadeFull ? paneRect.right : flowRight;

		if (!m_bodyFont)
		{
			if (upperDc)
			{
				RestoreDC(dc, upperDc);
			}
			if (imgView && !detailsOnly)
			{
				PaintImgPreview(dc, paneRect);
			}
			return;
		}

		const HGDIOBJ previousFont = SelectObject(dc, m_bodyFont);
		bool previewPainted = false;
		auto paintPreview = [&]() noexcept
		{
			if (imgView || previewPainted || (!m_thumbnail && m_capacityPer1000 < 0))
			{
				return;
			}

			// folder.htt puts one explicit body line before the 120px Thumbnail object.
			y += kBodyLineHeight;
			// The box is square except for Windows Me's sysroot.htt drive graph; see
			// SetCapacityPie. A thumbnail always uses the square.
			const int boxHeight = m_thumbnail ? kThumbnailSize : m_capacityPieHeight;
			RECT thumbnailBox{ detailsX, y, detailsX + kThumbnailSize, y + boxHeight };
			// The object keeps its full box in a short pane and the viewport clips it.
			if (m_thumbnail)
			{
				DrawThumbnailAt(dc, m_thumbnail, thumbnailBox);
			}
			else
			{
				Draw3dPie(dc, thumbnailBox, static_cast<DWORD>(m_capacityPer1000));
			}
			y = thumbnailBox.bottom;
			previewPainted = true;
		};

		for (PaneLine &line : m_lines)
		{
			line.tooltipBounds = RECT{};
			// folder.htt keeps #Links after #Thumbnail. Root-drive NoneSelected() is the
			// one state that has both media and later text, so it marks the See-also line.
			if (line.previewBefore)
			{
				paintPreview();
			}
			y += line.marginTop;
			if (y >= contentBottom)
			{
				// Off the bottom: clear the rectangle so a stale one from a taller pane cannot
				// keep answering hit tests for something that is no longer visible.
				line.bounds = RECT{};
				continue;
			}

			if (line.link)
			{
				HFONT linkFont = line.bold ? m_boldLinkFont : m_linkFont;
				SelectObject(dc, linkFont ? linkFont : (line.bold ? m_boldFont : m_bodyFont));
				SetTextColor(dc, LinkColour());
			}
			else
			{
				SelectObject(dc, line.heading ? m_headingFont : (line.bold ? m_boldFont : m_bodyFont));
				SetTextColor(dc, TextColour());
			}

            if(line.message) {
                line.bounds=DrawReferenceDescription(dc,m_descriptionFont ? m_descriptionFont : m_bodyFont,
                    line.text.c_str(),detailsX,y,std::max(1,std::min(kMessageWidth,detailsRight-detailsX)));
                y=line.bounds.bottom;
                continue;
            }

			if (line.button)
			{
				SelectObject(dc, m_buttonFont ? m_buttonFont : m_bodyFont);
				SetTextColor(dc,GetSysColor(COLOR_BTNTEXT));
				const int lineIndex = static_cast<int>(&line - m_lines.data());
				const bool pressed = m_pressedButton == lineIndex;

				const std::wstring caption = ButtonCaption(line);
				SIZE extent{};
				GetTextExtentPoint32W(dc, line.text.c_str(), static_cast<int>(line.text.size()), &extent);
				const int width = line.buttonWidth > 0
					? line.buttonWidth : extent.cx + kButtonExtraWidth;
				const int buttonX = detailsX + kButtonMarginLeft;
				RECT button{ buttonX, y, buttonX + width, y + kButtonHeight };

				// DrawFrameControl is the documented USER/GDI push-button primitive. It deliberately
				// bypasses current visual styles, while the inner white fill preserves recycle.htt's
				// explicit `background-color: white` rule.
				RECT frame = button;
				DrawFrameControl(dc, &frame, DFC_BUTTON,
				                 DFCS_BUTTONPUSH | (pressed ? DFCS_PUSHED : 0));
				RECT face = button;
				InflateRect(&face, -2, -2);
				FillReferenceSystemColour(dc,face,COLOR_3DFACE);

				RECT label = button;
				if (pressed)
				{
					OffsetRect(&label, 1, 1);
				}
				BOOL keyboardCues = TRUE;
				SystemParametersInfoW(SPI_GETKEYBOARDCUES, 0, &keyboardCues, 0);
				UINT format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				if (!keyboardCues)
				{
					format |= DT_HIDEPREFIX;
				}
				DrawTextW(dc, caption.c_str(), -1, &label, format);

				line.bounds = button;
				y = button.bottom;
				continue;
			}

			int lineX = detailsX + line.indent;
			if (line.swatch != PaneLine::Swatch::None)
			{
				// <table class=Legend width=12 height=12 border=1 bgcolor=threedface|threedhighlight
				//        bordercolordark=black bordercolorlight=black>
				// Both border colours are black, so it is a flat 1px black frame, not a 3D edge.
				const int swatchX = detailsX + kLegendIndent;
				RECT box{ swatchX, y, swatchX + kLegendSize, y + kLegendSize };

				const int fill = (line.swatch == PaneLine::Swatch::Used) ? COLOR_3DFACE : COLOR_3DHIGHLIGHT;
				FillReferenceSystemColour(dc,box,fill);
				FrameRect(dc, &box, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
				if (!line.tooltip.empty())
				{
					line.tooltipBounds = box;
				}

				// "&nbsp;" separates the table from the label in the original.
				lineX = box.right + 4;
			}

			if (!line.boldPrefix.empty() || !line.boldSuffix.empty())
			{
				// printers.htt mixes one bold run into otherwise ordinary paragraph text. DrawText
				// cannot change fonts inside a wrapped string, so retain its word-boundary behaviour
				// with a three-run layout rather than hard-coding the 96-DPI physical line breaks.
				//
				// nethood.htt's soft barrier puts an inline *anchor* in the same position, and its
				// text has to wrap at two different widths — #Details when Brand sits beside the
				// panel, the far wider #Brand when it covers the client. Setting `link` alongside
				// the two plain runs therefore selects the link font and colour for the middle run
				// and narrows the hit rectangle to the glyphs it actually drew.
				const bool linkRun = line.link;
				const HFONT boldFont = linkRun
					? (m_linkFont ? m_linkFont : m_bodyFont)
					: (m_boldFont ? m_boldFont : m_bodyFont);
				const COLORREF bodyColor = TextColour();
				const COLORREF boldColor = linkRun
					? LinkColour()
					: ((line.boldColor == CLR_INVALID) ? bodyColor : line.boldColor);
				const int firstY = y;
				int runX = lineX;
				int runY = y;
				int maxRight = lineX;
				HFONT pendingFont = nullptr;
				COLORREF pendingColor = bodyColor;
				bool pendingSpace = false;

				// Union of the pieces drawn while `capturing`, so an inline anchor's hit rectangle
				// covers only its own glyphs even when the surrounding sentence wraps around it.
				bool capturing = false;
				RECT captured{};
				bool capturedAny = false;

				auto measure = [&](HFONT font, const wchar_t *text, int length) noexcept
				{
					SIZE extent{};
					SelectObject(dc, font);
					GetTextExtentPoint32W(dc, text, length, &extent);
					return extent.cx;
				};
				auto nextLine = [&]() noexcept
				{
					pendingSpace = false;
					runX = lineX;
					runY += kBodyLineHeight;
				};
				auto drawPiece = [&](HFONT font, COLORREF color,
				                     const wchar_t *text, int length) noexcept
				{
					const int width = measure(font, text, length);
					SetTextColor(dc, color);
					if (runY < contentBottom)
					{
						TextOutW(dc, runX, runY, text, length);
					}
					if (capturing)
					{
						const RECT piece{ runX, runY, runX + width, runY + kBodyLineHeight };
						if (capturedAny)
						{
							UnionRect(&captured, &captured, &piece);
						}
						else
						{
							captured = piece;
							capturedAny = true;
						}
					}
					runX += width;
					maxRight = (runX > maxRight) ? runX : maxRight;
				};

				auto drawRun = [&](const std::wstring &value, HFONT font, COLORREF color) noexcept
				{
					size_t position = 0;
					while (position < value.length())
					{
						const wchar_t ch = value[position];
						if (ch == L'\r')
						{
							++position;
							continue;
						}
						if (ch == L'\n')
						{
							nextLine();
							++position;
							continue;
						}
						if (ch == L' ' || ch == L'\t')
						{
							if (runX != lineX)
							{
								pendingFont = font;
								pendingColor = color;
								pendingSpace = true;
							}
							do
							{
								++position;
							} while (position < value.length() &&
							         (value[position] == L' ' || value[position] == L'\t'));
							continue;
						}

						size_t wordEnd = position;
						while (wordEnd < value.length() && value[wordEnd] != L' ' &&
						       value[wordEnd] != L'\t' && value[wordEnd] != L'\r' &&
						       value[wordEnd] != L'\n')
						{
							++wordEnd;
						}

						const int wordLength = static_cast<int>(wordEnd - position);
						const int wordWidth = measure(font, value.c_str() + position, wordLength);
						const int spaceWidth = pendingSpace ? measure(pendingFont, L" ", 1) : 0;
						if (runX != lineX && runX + spaceWidth + wordWidth > detailsRight)
						{
							nextLine();
						}
						else if (pendingSpace)
						{
							drawPiece(pendingFont, pendingColor, L" ", 1);
							pendingSpace = false;
						}

						drawPiece(font, color, value.c_str() + position, wordLength);
						position = wordEnd;
					}
				};

				drawRun(line.boldPrefix, m_bodyFont, bodyColor);
				capturing = linkRun;
				drawRun(line.text, boldFont, boldColor);
				capturing = false;
				drawRun(line.boldSuffix, m_bodyFont, bodyColor);

				const RECT block{ lineX, firstY, maxRight, runY + kBodyLineHeight };
				line.bounds = (linkRun && capturedAny) ? captured : block;
				if (line.link && !line.tooltip.empty())
				{
					line.tooltipBounds = line.bounds;
				}
				y = block.bottom;
				continue;
			}

			if (line.link && (!line.linkPrefix.empty() || !line.linkSuffix.empty()))
			{
				// nethood.htt's setup sentence has one inline anchor. The 96-DPI template
				// wraps it into fixed physical lines; keep the surrounding text in the menu
				// font and expose only the anchor glyphs to link hit-testing.
				SIZE prefixSize{};
				SIZE linkSize{};

				SelectObject(dc, m_bodyFont);
				SetTextColor(dc, TextColour());
				if (!line.linkPrefix.empty())
				{
					GetTextExtentPoint32W(dc, line.linkPrefix.c_str(),
						static_cast<int>(line.linkPrefix.size()), &prefixSize);
					TextOutW(dc, lineX, y, line.linkPrefix.c_str(),
						static_cast<int>(line.linkPrefix.size()));
				}

				const int linkX = lineX + prefixSize.cx;
				SelectObject(dc, m_linkFont ? m_linkFont : m_bodyFont);
				SetTextColor(dc, LinkColour());
				GetTextExtentPoint32W(dc, line.text.c_str(), static_cast<int>(line.text.size()), &linkSize);
				TextOutW(dc, linkX, y, line.text.c_str(), static_cast<int>(line.text.size()));
				line.bounds = RECT{ linkX, y, linkX + linkSize.cx, y + kBodyLineHeight };
				if (!line.tooltip.empty())
				{
					line.tooltipBounds = line.bounds;
				}

				if (!line.linkSuffix.empty())
				{
					SelectObject(dc, m_bodyFont);
					SetTextColor(dc, TextColour());
					TextOutW(dc, linkX + linkSize.cx, y, line.linkSuffix.c_str(),
						static_cast<int>(line.linkSuffix.size()));
				}

				y += kBodyLineHeight;
				continue;
			}

			RECT text{ lineX, y, detailsRight, contentBottom };
			const UINT format = DT_LEFT | DT_NOPREFIX |
                ((detailsOnly && m_scrollImgDetails && line.bold) ? DT_SINGLELINE : DT_WORDBREAK);
            DrawTextW(dc, line.text.c_str(), -1, &text, format | DT_CALCRECT);

			// DT_CALCRECT shrinks `right` to the measured width. Keep that for the hit rectangle
			// so the pointer only counts as "over the link" across the glyphs, the way an inline
			// anchor behaves — then widen it back for the draw so wrapping is unchanged.
			line.bounds = text;
			if (line.link && !line.tooltip.empty())
			{
				line.tooltipBounds = line.bounds;
			}
			text.right = detailsRight;
			DrawTextW(dc, line.text.c_str(), -1, &text, format);

			y = text.bottom;
		}

		// imgview.htt adds Dimensions only after shimgvw has accepted and decoded the image.
		// PreviewReady concatenates cxImage + "x" + cyImage with no spaces around the x.
		if (m_showDimensions && imgView && m_imgPreviewState == ImgPreviewState::Image &&
		    m_imgSourceSize.cx > 0 && m_imgSourceSize.cy > 0)
		{
			y += kImgParagraphTop;
			if (y < contentBottom)
			{
				wchar_t dimensions[96]{};
				if (SUCCEEDED(StringCchPrintfW(dimensions, ARRAYSIZE(dimensions),
				                               ReferenceCaption(L"Dimensions: %dx%d pixels"),
				                               m_imgSourceSize.cx, m_imgSourceSize.cy)))
				{
					SelectObject(dc, m_bodyFont);
					SetTextColor(dc, TextColour());
					RECT text{ detailsX, y, detailsRight, contentBottom };
const UINT format=DT_LEFT | DT_NOPREFIX | DT_WORDBREAK;
                    DrawTextW(dc, dimensions, -1, &text,format | DT_CALCRECT);
                    const int measuredRight=text.right;
                    text.right=detailsRight;
                    DrawTextW(dc, dimensions, -1, &text,format);
                    y=text.bottom;
                    if(detailsOnly && measuredRight>detailsRight) m_imgDetailsExtent.cx=std::max(m_imgDetailsExtent.cx,
                        static_cast<LONG>(measuredRight-paneRect.left+12));
				}
			}
		}

		if (imgView && !detailsOnly)
		{
			// The overflowed Panel is not interactive below its 50% viewport. Clip the
			// recorded hit rectangles as well as the pixels so hidden links cannot activate.
			const RECT upper{ paneRect.left, paneRect.top, paneRect.right, contentBottom };
			for (PaneLine &line : m_lines)
			{
				RECT clipped{};
				IntersectRect(&clipped, &line.bounds, &upper);
				line.bounds = clipped;
				if (!IsRectEmpty(&line.tooltipBounds))
				{
					IntersectRect(&clipped, &line.tooltipBounds, &upper);
					line.tooltipBounds = clipped;
				}
			}
		}
		if(detailsOnly) {
            // Wrapped text and a description box already fit the viewport.
            // Only a genuinely overflowing run (such as the single-line filename)
            // needs horizontal scrolling; do not add a phantom right margin.
            for(const auto& line:m_lines) if(line.bounds.right>detailsRight)
                m_imgDetailsExtent.cx=std::max(m_imgDetailsExtent.cx,
                    static_cast<LONG>(line.bounds.right-paneRect.left+12));
            m_imgDetailsExtent.cy=std::max(0L,y-paneRect.top+8);
        }
        SelectObject(dc, previousFont);

        // Selected-file thumbnails have no later line marker, so they remain after all details.
		paintPreview();
		if (upperDc)
		{
			RestoreDC(dc, upperDc);
		}
		if (imgView && !detailsOnly)
		{
			PaintImgPreview(dc, paneRect);
		}
	}

// Independent renderer: sampled ellipse surfaces and radial sectors, using the
// public GDI Polygon/Ellipse APIs. The palette/2:1 silhouette/depth are visual
// requirements; this uses angular coverage and no legacy integer square root.
namespace {
void Draw3dPie(HDC dc,RECT box,DWORD used) noexcept {
    const int width=box.right-box.left,height=box.bottom-box.top;
    if(!dc || width<4 || height<4) return;
    const int discHeight=std::min(height,width/2),discWidth=discHeight*2;
    const int depth=std::max(1,discHeight/6);
    const RECT top{box.left+(width-discWidth)/2,box.top+(height-discHeight)/2,
        box.left+(width+discWidth)/2,box.top+(height+discHeight)/2-depth};
    const double rx=(top.right-top.left-1)*0.5,ry=(top.bottom-top.top-1)*0.5;
    if(rx<1 || ry<1) return;
    const double cx=top.left+rx,cy=top.top+ry;
    constexpr double pi=3.14159265358979323846;
    used=std::min<DWORD>(used,1000);
    const double freeArc=(1000-used)*2*pi/1000;
    const int saved=SaveDC(dc);
    if(!saved) return;
    const HPEN pen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWFRAME));
    if(!pen) { RestoreDC(dc,saved); return; }
    SelectObject(dc,pen);
    auto point=[&](double angle,int offset=0) -> POINT {
        return {static_cast<LONG>(std::lround(cx+rx*std::cos(angle))),
                static_cast<LONG>(std::lround(cy+ry*std::sin(angle)))+offset};
    };
    auto polygon=[&](const POINT* points,int count,COLORREF colour) {
        HBRUSH brush=CreateSolidBrush(colour);
        if(!brush) return;
        const HGDIOBJ old=SelectObject(dc,brush);
        Polygon(dc,points,count);
        SelectObject(dc,old); DeleteObject(brush);
    };
    auto front=[&](double last,COLORREF colour) {
        POINT points[258]{};
        constexpr int segments=128;
        for(int i=0;i<=segments;++i) points[i]=point(last*i/segments);
        for(int i=0;i<=segments;++i) points[segments+1+i]=point(last*(segments-i)/segments,depth);
        polygon(points,ARRAYSIZE(points),colour);
    };
    front(pi,GetSysColor(COLOR_3DFACE));
    if(used>500) front(pi-freeArc,GetSysColor(COLOR_3DSHADOW));
    HBRUSH topBrush=CreateSolidBrush(GetSysColor(used==0 ? COLOR_3DHILIGHT : COLOR_3DFACE));
    if(topBrush) {
        const HGDIOBJ old=SelectObject(dc,topBrush);
        Ellipse(dc,top.left,top.top,top.right,top.bottom);
        SelectObject(dc,old); DeleteObject(topBrush);
    }
    if(used>0 && used<1000) {
        POINT sector[514]{};
        const int segments=std::max(1,static_cast<int>(std::ceil(freeArc*512/(2*pi))));
        sector[0]={static_cast<LONG>(std::lround(cx)),static_cast<LONG>(std::lround(cy))};
        for(int i=0;i<=segments;++i) sector[i+1]=point(pi-freeArc*i/segments);
        polygon(sector,segments+2,GetSysColor(COLOR_3DHILIGHT));
    }
    RestoreDC(dc,saved); DeleteObject(pen);
}
}


	int WebViewNativePane::HitTestLink(POINT pt) const noexcept
	{
		for (size_t i = 0; i < m_lines.size(); ++i)
		{
			const PaneLine &line = m_lines[i];
			if (!line.link && !line.button)
			{
				continue;
			}
			// An all-zero rectangle is a line Paint skipped; PtInRect would reject it anyway,
			// but checking is cheaper than trusting that.
			if (line.bounds.right > line.bounds.left && PtInRect(&line.bounds, pt))
			{
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	int WebViewNativePane::FindButtonByAccessKey(wchar_t key) const noexcept
	{
		const wchar_t wanted = static_cast<wchar_t>(towupper(key));
		for (size_t index = 0; index < m_lines.size(); ++index)
		{
			const PaneLine &line = m_lines[index];
			if (line.button && line.buttonAccessKey &&
			    towupper(line.buttonAccessKey) == wanted)
			{
				return static_cast<int>(index);
			}
		}
		return -1;
	}
} // namespace win2kwebview
} // namespace ce

// Independent viewport support: the reference template puts the details and
// the image viewer in separate halves. Scroll only the upper half and keep the
// hit rectangles in the parent's coordinates, including clipped/scrolled links.
namespace ce { namespace win2kwebview {
void WebViewNativePane::Paint(HDC dc,const RECT& paneRect,UINT dpi) noexcept {
    PaintBody(dc,paneRect,dpi,false);
}
SIZE WebViewNativePane::PaintImgDetails(HDC dc,const RECT& viewport,UINT dpi,POINT scroll,int dividerWidth) noexcept {
    if(!dc || m_profile!=PaneProfile::ImgView) return {};
    const int saved=SaveDC(dc);
    if(!saved) return {};
    RECT visible=viewport;
    // The preview boundary is the viewport boundary. End-of-content padding
    // belongs to the scroll extent, not to a permanently blank clipping strip.
    IntersectClipRect(dc,visible.left,visible.top,visible.right,visible.bottom);
    OffsetWindowOrgEx(dc,scroll.x,scroll.y,nullptr);
    PaintBody(dc,viewport,dpi,true,dividerWidth);
    RestoreDC(dc,saved);
    for(auto& line:m_lines) {
        RECT clipped{};
        OffsetRect(&line.bounds,-scroll.x,-scroll.y);
        IntersectRect(&clipped,&line.bounds,&visible); line.bounds=clipped;
        if(!IsRectEmpty(&line.tooltipBounds)) {
            OffsetRect(&line.tooltipBounds,-scroll.x,-scroll.y);
            IntersectRect(&clipped,&line.tooltipBounds,&visible); line.tooltipBounds=clipped;
        }
    }
    return m_imgDetailsExtent;
}
} }

// Independent viewer support: viewer-paint-transaction.inc
// Retain only this mod's child-window frames while Shell callbacks can pump
// messages. There is no overlay and no visibility/style change to the file list.
namespace ce { namespace win2kwebview {
class RetainedViewerFrame {
    static constexpr PCWSTR kProperty=L"ClassicWebViewPane.RetainedViewerFrame";
    HWND m_window=nullptr;
    HDC m_dc=nullptr;
    HBITMAP m_bitmap=nullptr;
    HGDIOBJ m_original=nullptr;
    SIZE m_size{};
public:
    RetainedViewerFrame()=default;
    RetainedViewerFrame(const RetainedViewerFrame&)=delete;
    RetainedViewerFrame& operator=(const RetainedViewerFrame&)=delete;
    ~RetainedViewerFrame() { Release(); }
    void Capture(HWND window) noexcept {
        if(!window || GetPropW(window,kProperty)) return; // an outer transaction owns it
        RECT client{};
        if(!GetClientRect(window,&client) || client.right<=0 || client.bottom<=0) return;
        HDC screen=GetDC(window);
        if(!screen) return;
        m_dc=CreateCompatibleDC(screen);
        m_bitmap=CreateCompatibleBitmap(screen,client.right,client.bottom);
        ReleaseDC(window,screen);
        if(!m_dc || !m_bitmap) { Release();return; }
        m_original=SelectObject(m_dc,m_bitmap);m_size={client.right,client.bottom};
        SendMessageW(window,WM_PRINTCLIENT,reinterpret_cast<WPARAM>(m_dc),PRF_CLIENT);
        if(!IsWindow(window) || !SetPropW(window,kProperty,this)) { Release();return; }
        m_window=window;
    }
    void Release() noexcept {
        if(m_window && GetPropW(m_window,kProperty)==this) {
            RemovePropW(m_window,kProperty);
            RedrawWindow(m_window,nullptr,nullptr,RDW_INVALIDATE|RDW_FRAME|RDW_NOERASE);
        }
        m_window=nullptr;
        if(m_dc && m_original) SelectObject(m_dc,m_original);
        if(m_bitmap) DeleteObject(m_bitmap);
        if(m_dc) DeleteDC(m_dc);
        m_dc=nullptr;m_bitmap=nullptr;m_original=nullptr;m_size={};
    }
    static bool PaintMessage(HWND window,UINT message,WPARAM wParam) noexcept {
        auto* held=static_cast<RetainedViewerFrame*>(GetPropW(window,kProperty));
        if(!held) return false;
        if(message==WM_NCPAINT) return true;
        if(message!=WM_PAINT && message!=WM_PRINTCLIENT && message!=WM_ERASEBKGND) return false;
        PAINTSTRUCT paint{};
        HDC target=message==WM_PAINT ? BeginPaint(window,&paint) : reinterpret_cast<HDC>(wParam);
        if(target && held->m_dc)
            BitBlt(target,0,0,held->m_size.cx,held->m_size.cy,held->m_dc,0,0,SRCCOPY);
        if(message==WM_PAINT) EndPaint(window,&paint);
        return true;
    }
};
class ViewerPaintTransaction {
    RetainedViewerFrame m_details,m_image,m_toolbar;
public:
    ViewerPaintTransaction(HWND details,HWND image,HWND toolbar) noexcept {
        m_details.Capture(details);m_image.Capture(image);m_toolbar.Capture(toolbar);
    }
    void Commit() noexcept { m_details.Release();m_image.Release();m_toolbar.Release(); }
};
} }

// Independent viewer support: image-viewer-support.inc
namespace ce { namespace win2kwebview {
static std::wstring ViewerArgument(const std::wstring& value) {
    std::wstring quoted=L"\"";
    size_t slashes=0;
    for(const wchar_t ch:value) {
        if(ch==L'\\') { ++slashes; continue; }
        quoted.append(ch==L'\"' ? slashes*2+1 : slashes,L'\\');
        quoted+=ch; slashes=0;
    }
    quoted.append(slashes*2,L'\\'); quoted+=L'\"';
    return quoted;
}
static std::wstring ViewerArguments(const std::wstring& pattern,const std::wstring& file) {
    const auto quoted=ViewerArgument(file);
    std::wstring result;
    bool replaced=false;
    for(size_t i=0;i<pattern.size();) {
        if(pattern.compare(i,4,L"\"%1\"")==0) {
            result+=quoted; i+=4; replaced=true;
        } else if(pattern.compare(i,2,L"%1")==0) {
            result+=quoted; i+=2; replaced=true;
        } else result+=pattern[i++];
    }
    if(!replaced) { if(!result.empty()) result+=L' '; result+=quoted; }
    return result;
}
static HRESULT LaunchImageViewer(HWND owner,std::wstring executable,const std::wstring& arguments,
                                 const std::wstring& file) {
    const auto first=executable.find_first_not_of(L" \t\r\n");
    if(first==std::wstring::npos || file.empty()) return E_INVALIDARG;
    executable=executable.substr(first,executable.find_last_not_of(L" \t\r\n")-first+1);
    if(executable.size()>=2 && executable.front()==L'\"' && executable.back()==L'\"')
        executable=executable.substr(1,executable.size()-2);
    const DWORD required=ExpandEnvironmentStringsW(executable.c_str(),nullptr,0);
    if(!required || required>32767) return E_INVALIDARG;
    std::vector<wchar_t> expanded(required);
    if(ExpandEnvironmentStringsW(executable.c_str(),expanded.data(),required)!=required) return E_INVALIDARG;
    executable=expanded.data();
    std::wstring command=ViewerArgument(executable)+L" "+ViewerArguments(arguments,file);
    if(command.size()>=32767) return E_INVALIDARG;
    STARTUPINFOW startup{sizeof(startup)};
    // Placement is a hint; applications may retain their own saved window position.
    MONITORINFO monitor{sizeof(monitor)};
    if(owner && GetMonitorInfoW(MonitorFromWindow(owner,MONITOR_DEFAULTTONEAREST),&monitor)) {
        startup.dwFlags=STARTF_USEPOSITION;
        startup.dwX=monitor.rcWork.left; startup.dwY=monitor.rcWork.top;
    }
    PROCESS_INFORMATION process{};
    if(!CreateProcessW(executable.c_str(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&process))
        return HRESULT_FROM_WIN32(GetLastError());
    CloseHandle(process.hThread); CloseHandle(process.hProcess);
    return S_OK;
}
static HBITMAP ToolbarBitmap(HBITMAP original,SIZE source,int width,int height,bool mask) {
    HDC from=CreateCompatibleDC(nullptr),to=CreateCompatibleDC(nullptr);
    BITMAPINFO info{}; info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth=width; info.bmiHeader.biHeight=-height;
    info.bmiHeader.biPlanes=1; info.bmiHeader.biBitCount=24;
    void* bits=nullptr;
    HBITMAP bitmap=mask ? CreateBitmap(width,height,1,1,nullptr) :
        CreateDIBSection(nullptr,&info,DIB_RGB_COLORS,&bits,nullptr,0);
    if(!from || !to || !bitmap) {
        if(from) DeleteDC(from); if(to) DeleteDC(to); if(bitmap) DeleteObject(bitmap);
        return nullptr;
    }
    const auto oldFrom=SelectObject(from,original),oldTo=SelectObject(to,bitmap);
    SetBkColor(from,RGB(255,255,255));
    SetStretchBltMode(to,mask ? COLORONCOLOR : HALFTONE);
    const bool drawn=StretchBlt(to,0,0,width,height,from,0,0,source.cx,source.cy,SRCCOPY)!=FALSE;
    SelectObject(from,oldFrom); SelectObject(to,oldTo); DeleteDC(from); DeleteDC(to);
    if(!drawn) { DeleteObject(bitmap); return nullptr; }
    return bitmap;
}
static bool CreateReferenceToolbarImages(UINT dpi,HIMAGELIST& normal,HIMAGELIST& hover) {
    normal=nullptr; hover=nullptr;
    SIZE coldSize{},hotSize{},maskSize{};
    HBITMAP cold=LoadImageResource(IDB_WIN2K_IMGVIEW_TOOLBAR,coldSize);
    HBITMAP hot=LoadImageResource(IDB_WIN2K_IMGVIEW_TOOLBAR_HOT,hotSize);
    HBITMAP mask=LoadImageResource(IDB_WIN2K_IMGVIEW_TOOLBAR_MASK,maskSize);
    const bool valid=cold && hot && mask && coldSize.cx>=144 && coldSize.cx%24==0 && coldSize.cy==24 &&
        hotSize.cx==coldSize.cx && hotSize.cy==24 && maskSize.cx==coldSize.cx && maskSize.cy==24;
    const int cell=MulDiv(24,dpi ? dpi : 96,96);
    HBITMAP scaledCold=nullptr,scaledHot=nullptr,scaledMask=nullptr;
    if(valid && cell>0 && cell<=192) {
        const int width=cell*(coldSize.cx/24);
        scaledCold=ToolbarBitmap(cold,coldSize,width,cell,false);
        scaledHot=ToolbarBitmap(hot,hotSize,width,cell,false);
        scaledMask=ToolbarBitmap(mask,maskSize,width,cell,true);
        normal=ImageList_Create(cell,cell,ILC_COLOR24|ILC_MASK,6,1);
        hover=ImageList_Create(cell,cell,ILC_COLOR24|ILC_MASK,6,1);
    }
    const bool added=scaledCold && scaledHot && scaledMask && normal && hover &&
        ImageList_Add(normal,scaledCold,scaledMask)>=0 && ImageList_Add(hover,scaledHot,scaledMask)>=0;
    for(auto bitmap:{cold,hot,mask,scaledCold,scaledHot,scaledMask}) if(bitmap) DeleteObject(bitmap);
    if(!added) {
        if(normal) ImageList_Destroy(normal); if(hover) ImageList_Destroy(hover);
        normal=nullptr; hover=nullptr;
    }
    return added;
}
static void DrawPreviewToolbar(HWND toolbar,HDC dc,const RECT& client) {
    FillReferenceSystemColour(dc,client,COLOR_3DFACE);
    const auto normal=reinterpret_cast<HIMAGELIST>(SendMessageW(toolbar,TB_GETIMAGELIST,0,0));
    const auto hover=reinterpret_cast<HIMAGELIST>(SendMessageW(toolbar,TB_GETHOTIMAGELIST,0,0));
    int imageWidth=0,imageHeight=0;
    if(!normal || !ImageList_GetIconSize(normal,&imageWidth,&imageHeight)) return;
    const int count=static_cast<int>(SendMessageW(toolbar,TB_BUTTONCOUNT,0,0));
    const int hot=static_cast<int>(SendMessageW(toolbar,TB_GETHOTITEM,0,0));
    for(int index=0;index<count;++index) {
        TBBUTTON button{}; RECT rect{};
        if(!SendMessageW(toolbar,TB_GETBUTTON,index,reinterpret_cast<LPARAM>(&button)) ||
           (button.fsState&TBSTATE_HIDDEN) ||
           !SendMessageW(toolbar,TB_GETITEMRECT,index,reinterpret_cast<LPARAM>(&rect))) continue;
        if(button.fsStyle&BTNS_SEP) {
            const LONG middle=(rect.left+rect.right)/2;
            RECT line{middle,rect.top+4,middle+2,rect.bottom-4};
            DrawEdge(dc,&line,EDGE_ETCHED,BF_LEFT); continue;
        }
        const bool enabled=IsWindowEnabled(toolbar) && (button.fsState&TBSTATE_ENABLED);
        const bool pressed=(button.fsState&(TBSTATE_PRESSED|TBSTATE_CHECKED))!=0;
        if(enabled && (pressed || hot==index))
            DrawEdge(dc,&rect,pressed ? EDGE_SUNKEN : EDGE_RAISED,BF_RECT);
        const int offset=enabled && pressed ? 1 : 0;
        const int x=rect.left+(rect.right-rect.left-imageWidth)/2+offset;
        const int y=rect.top+(rect.bottom-rect.top-imageHeight)/2+offset;
        const auto images=enabled && hot==index && hover ? hover : normal;
        if(button.iBitmap>=0 && button.iBitmap<ImageList_GetImageCount(images))
            ImageList_DrawEx(images,button.iBitmap,dc,x,y,0,0,CLR_NONE,
                            GetSysColor(COLOR_3DFACE),enabled ? ILD_NORMAL : ILD_BLEND50);
    }
}
static void PaintPreviewToolbar(HWND toolbar,HDC target) {
    RECT client{}; GetClientRect(toolbar,&client);
    const int width=client.right-client.left,height=client.bottom-client.top;
    if(!target || width<=0 || height<=0) return;
    const int saved=SaveDC(target);
    HDC memory=CreateCompatibleDC(target);
    HBITMAP buffer=CreateCompatibleBitmap(target,width,height);
    if(memory && buffer) {
        const auto old=SelectObject(memory,buffer);
        DrawPreviewToolbar(toolbar,memory,client);
        BitBlt(target,0,0,width,height,memory,0,0,SRCCOPY);
        SelectObject(memory,old);
    } else DrawPreviewToolbar(toolbar,target,client);
    if(buffer) DeleteObject(buffer); if(memory) DeleteDC(memory);
    if(saved) RestoreDC(target,saved);
}
static bool PreviewToolbarPaintMessage(HWND toolbar,UINT message,WPARAM wParam) {
    if(RetainedViewerFrame::PaintMessage(toolbar,message,wParam)) return true;
    if(message==WM_PRINTCLIENT) {
        PaintPreviewToolbar(toolbar,reinterpret_cast<HDC>(wParam)); return true;
    }
    if(message==WM_PAINT) {
        PAINTSTRUCT paint{}; HDC dc=BeginPaint(toolbar,&paint);
        PaintPreviewToolbar(toolbar,dc); EndPaint(toolbar,&paint); return true;
    }
    return false;
}
} }

// Independent viewer support: image-details-window.inc
// Independent upper viewport. Native nonclient scrollbars belong to this
// child, so neither the image toolbar nor the image moves with the details.
namespace ce { namespace win2kwebview {
class ImgViewDetailsWindow {
    static constexpr PCWSTR kClass=L"ClassicWebViewPane.Reference.ImgViewDetails";
    HWND m_hwnd=nullptr;
    WebViewNativePane* m_renderer=nullptr; // owned by the containing ReferenceContent
    RECT m_bounds{};
    UINT m_dpi=96;
    bool m_scrollEnabled=true, m_updating=false, m_tracking=false, m_layoutValid=false;
    POINT m_scroll{}; // physical pixels, matching SCROLLINFO
    POINT m_wheel{}; // accumulate high-resolution wheel deltas per axis
    SIZE m_extent{};
    std::function<SIZE(HDC,const RECT&,POINT,int)> m_painter;

    SIZE Draw(HDC dc,bool measure) noexcept {
        RECT client{}; GetClientRect(m_hwnd,&client);
        if(client.right<=0 || client.bottom<=0 || !m_renderer) return {};
        const int saved=SaveDC(dc);
        if(!saved) return {};
        if(measure) IntersectClipRect(dc,0,0,0,0);
        else {
            FillReferenceColour(dc,client,m_renderer->BackgroundColour());
            IntersectClipRect(dc,0,0,client.right,client.bottom);
        }
        SetMapMode(dc,MM_ANISOTROPIC);
        SetWindowExtEx(dc,96,96,nullptr); SetViewportExtEx(dc,m_dpi,m_dpi,nullptr);
        SetViewportOrgEx(dc,-m_bounds.left,-m_bounds.top,nullptr);
        RECT logical{MulDiv(m_bounds.left,96,m_dpi),MulDiv(m_bounds.top,96,m_dpi),
            MulDiv(m_bounds.left+client.right,96,m_dpi),MulDiv(m_bounds.top+client.bottom,96,m_dpi)};
        POINT scroll{MulDiv(m_scroll.x,96,m_dpi),MulDiv(m_scroll.y,96,m_dpi)};
        // Include the nonclient scrollbar in the divider's fixed width.
        // Text still wraps in the smaller client viewport above.
        const int dividerWidth=std::max(0L,MulDiv(m_bounds.right,96,m_dpi)-logical.left);
        const SIZE extent=m_painter ? m_painter(dc,logical,scroll,dividerWidth) :
            m_renderer->PaintImgDetails(dc,logical,96,scroll,dividerWidth);
        RestoreDC(dc,saved);
        // A fitting logical width represents this exact native client. A
        // round trip through logical units can otherwise add a phantom pixel.
        const LONG extentWidth=extent.cx<=logical.right-logical.left ? client.right : MulDiv(extent.cx,m_dpi,96);
        return {extentWidth,MulDiv(extent.cy,m_dpi,96)};
    }
    void UpdateScrollbars() noexcept {
        if(!m_hwnd || m_updating) return;
        m_updating=true;
        HDC dc=GetDC(m_hwnd);
        if(dc) {
            // Adding either bar changes wrapping and can require the other.
            // Re-measure after each geometry change until both are stable.
            for(int pass=0;pass<4;++pass) {
                RECT client{}; GetClientRect(m_hwnd,&client);
                m_extent=Draw(dc,true);
                const LONG_PTR style=GetWindowLongPtrW(m_hwnd,GWL_STYLE);
                const bool horizontal=m_scrollEnabled && m_extent.cx>client.right;
                const bool vertical=m_scrollEnabled && m_extent.cy>client.bottom;
                const bool same=horizontal==((style&WS_HSCROLL)!=0) && vertical==((style&WS_VSCROLL)!=0);
                if(same) break;
                if(horizontal!=((style&WS_HSCROLL)!=0)) ShowScrollBar(m_hwnd,SB_HORZ,horizontal);
                if(vertical!=((style&WS_VSCROLL)!=0)) ShowScrollBar(m_hwnd,SB_VERT,vertical);
            }
            RECT client{}; GetClientRect(m_hwnd,&client);
            for(int bar:{SB_HORZ,SB_VERT}) {
                const int extent=bar==SB_HORZ ? m_extent.cx : m_extent.cy;
                const int page=bar==SB_HORZ ? client.right : client.bottom;
                LONG& position=bar==SB_HORZ ? m_scroll.x : m_scroll.y;
                position=m_scrollEnabled ? std::clamp<LONG>(position,0,std::max(0,extent-page)) : 0;
                SCROLLINFO info{sizeof(info),SIF_RANGE|SIF_PAGE|SIF_POS};
                info.nMax=m_scrollEnabled ? std::max(0,extent-1) : 0;
                info.nPage=static_cast<UINT>(std::max(0,page)); info.nPos=position;
                SetScrollInfo(m_hwnd,bar,&info,FALSE);
            }
            // Clamp/reset can move the text without a WM_PAINT yet. Keep hit
            // tests current, while measuring with an empty clip region.
            Draw(dc,true);
            ReleaseDC(m_hwnd,dc);
        }
        m_updating=false;
    }
    bool Scroll(int bar,UINT command,int wheel=0) noexcept {
        if(!m_scrollEnabled || !IsWindowEnabled(m_hwnd)) return false;
        SCROLLINFO info{sizeof(info),SIF_ALL};
        if(!GetScrollInfo(m_hwnd,bar,&info)) return false;
        const int limit=std::max(0,info.nMax-static_cast<int>(info.nPage)+1);
        const int step=std::max(1,MulDiv(bar==SB_HORZ ? 12 : panemetrics::kBodyLineHeight,m_dpi,96));
        int next=info.nPos;
        if(wheel) next-=wheel*step;
        else switch(command) {
            case SB_LINEUP:next-=step;break;
            case SB_LINEDOWN:next+=step;break;
            case SB_PAGEUP:next-=static_cast<int>(info.nPage);break;
            case SB_PAGEDOWN:next+=static_cast<int>(info.nPage);break;
            case SB_THUMBTRACK:case SB_THUMBPOSITION:next=info.nTrackPos;break;
            case SB_TOP:next=0;break;
            case SB_BOTTOM:next=limit;break;
            default:return false;
        }
        next=std::clamp(next,0,limit);
        if(next==info.nPos) return false;
        if(bar==SB_HORZ) m_scroll.x=next; else m_scroll.y=next;
        SetScrollPos(m_hwnd,bar,next,TRUE);
        HDC dc=GetDC(m_hwnd); if(dc) { Draw(dc,true); ReleaseDC(m_hwnd,dc); }
        SendMessageW(GetParent(m_hwnd),WM_MOUSELEAVE,0,0);
        InvalidateRect(m_hwnd,nullptr,FALSE);
        return true;
    }
    void Paint(HDC target) noexcept {
        RECT client{}; GetClientRect(m_hwnd,&client);
        HDC memory=CreateCompatibleDC(target);
        HBITMAP bitmap=CreateCompatibleBitmap(target,std::max(1L,client.right),std::max(1L,client.bottom));
        if(memory && bitmap) {
            const auto old=SelectObject(memory,bitmap);
            Draw(memory,false);
            BitBlt(target,0,0,client.right,client.bottom,memory,0,0,SRCCOPY);
            SelectObject(memory,old);
        } else Draw(target,false);
        if(bitmap) DeleteObject(bitmap); if(memory) DeleteDC(memory);
    }
    static LRESULT CALLBACK Proc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam) {
        auto* self=reinterpret_cast<ImgViewDetailsWindow*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
        if(message==WM_NCCREATE) {
            self=static_cast<ImgViewDetailsWindow*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
            self->m_hwnd=hwnd; SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(self));
        }
        if(!self) return DefWindowProcW(hwnd,message,wParam,lParam);
        if(RetainedViewerFrame::PaintMessage(hwnd,message,wParam)) return 0;
        switch(message) {
            case WM_ERASEBKGND:return TRUE;
            case WM_PAINT: {
                PAINTSTRUCT paint{}; HDC dc=BeginPaint(hwnd,&paint);
                self->Paint(dc); EndPaint(hwnd,&paint);return 0;
            }
            case WM_PRINTCLIENT:self->Paint(reinterpret_cast<HDC>(wParam));return 0;
            case WM_SIZE:
                self->UpdateScrollbars();
                RedrawWindow(hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_FRAME|RDW_NOERASE);
                return 0;
            case WM_HSCROLL:case WM_VSCROLL:
                self->Scroll(message==WM_HSCROLL ? SB_HORZ : SB_VERT,LOWORD(wParam));return 0;
            case WM_MOUSEWHEEL:case WM_MOUSEHWHEEL: {
                UINT lines=3; SystemParametersInfoW(SPI_GETWHEELSCROLLLINES,0,&lines,0);
                const int bar=message==WM_MOUSEHWHEEL || (GET_KEYSTATE_WPARAM(wParam)&MK_SHIFT) ? SB_HORZ : SB_VERT;
                LONG& pending=bar==SB_HORZ ? self->m_wheel.x : self->m_wheel.y;
                pending+=GET_WHEEL_DELTA_WPARAM(wParam)*(message==WM_MOUSEHWHEEL ? -1 : 1);
                const int delta=pending/WHEEL_DELTA;pending%=WHEEL_DELTA;
                if(!delta || !lines) return 0;
                if(lines==WHEEL_PAGESCROLL) self->Scroll(bar,delta>0 ? SB_PAGEUP : SB_PAGEDOWN);
                else self->Scroll(bar,0,delta*static_cast<int>(std::min(lines,100U)));
                return 0;
            }
            case WM_KEYDOWN: {
                int bar=SB_VERT; UINT command=SB_ENDSCROLL;
                switch(wParam) {
                    case VK_LEFT:bar=SB_HORZ;command=SB_LINELEFT;break;
                    case VK_RIGHT:bar=SB_HORZ;command=SB_LINERIGHT;break;
                    case VK_UP:command=SB_LINEUP;break;
                    case VK_DOWN:command=SB_LINEDOWN;break;
                    case VK_PRIOR:command=SB_PAGEUP;break;
                    case VK_NEXT:command=SB_PAGEDOWN;break;
                    case VK_HOME:command=SB_TOP;break;
                    case VK_END:command=SB_BOTTOM;break;
                }
                if(command!=SB_ENDSCROLL) { self->Scroll(bar,command);return 0; }
                break;
            }
            case WM_GETDLGCODE:return DLGC_WANTARROWS;
            case WM_SETCURSOR:
                if(LOWORD(lParam)==HTCLIENT) {
                    POINT point{}; GetCursorPos(&point); ScreenToClient(GetParent(hwnd),&point);
                    point={MulDiv(point.x,96,self->m_dpi),MulDiv(point.y,96,self->m_dpi)};
                    SetCursor(LoadCursorW(nullptr,IsWindowEnabled(hwnd) && self->m_renderer &&
                        self->m_renderer->HitTestLink(point)>=0 ? IDC_HAND : IDC_ARROW));return TRUE;
                }
                break;
            case WM_MOUSEMOVE:case WM_LBUTTONDOWN:case WM_LBUTTONUP:case WM_LBUTTONDBLCLK: {
                if(!IsWindowEnabled(hwnd)) return 0;
                if(message==WM_MOUSEMOVE && !self->m_tracking) {
                    TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,hwnd,0};
                    self->m_tracking=TrackMouseEvent(&track)!=FALSE;
                }
                if(message==WM_LBUTTONDOWN) SetFocus(hwnd);
                POINT point{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                const HWND parent=GetParent(hwnd); MapWindowPoints(hwnd,parent,&point,1);
                return SendMessageW(parent,message,wParam,MAKELPARAM(point.x,point.y));
            }
            case WM_MOUSELEAVE:
                self->m_tracking=false;return SendMessageW(GetParent(hwnd),message,wParam,lParam);
            case WM_NCDESTROY:
                self->m_hwnd=nullptr; self->m_layoutValid=false;
                SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);break;
        }
        return DefWindowProcW(hwnd,message,wParam,lParam);
    }
public:
    ~ImgViewDetailsWindow() { Destroy(); }
    static bool Register() noexcept {
        WNDCLASSEXW cls{sizeof(cls)}; cls.lpfnWndProc=Proc; cls.hInstance=ReferenceModule();
        cls.lpszClassName=kClass; cls.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        return RegisterClassExW(&cls)!=0;
    }
    bool Create(HWND parent,WebViewNativePane* renderer) noexcept {
        if(m_hwnd) return true;
        if(!g_referenceDetailsRegistered || !renderer) return false;
        m_renderer=renderer;
        m_hwnd=CreateWindowExW(0,kClass,nullptr,WS_CHILD|WS_CLIPSIBLINGS|WS_TABSTOP,
            0,0,0,0,parent,nullptr,ReferenceModule(),this);
        return m_hwnd!=nullptr;
    }
    void Destroy() noexcept {
        if(m_hwnd) DestroyWindow(m_hwnd);
        m_hwnd=nullptr; m_renderer=nullptr; m_painter={}; m_scroll={}; m_wheel={}; m_extent={}; m_tracking=false;
        m_layoutValid=false;
    }
    HWND Window() const noexcept { return m_hwnd; }
    POINT ScrollPosition() const noexcept { return m_scroll; }
    SIZE ContentSize() const noexcept { return m_extent; }
    void SetPainter(std::function<SIZE(HDC,const RECT&,POINT,int)> painter) { m_painter=std::move(painter); }
    void ResetScroll() noexcept { m_scroll={}; m_wheel={}; }
    void Layout(const RECT& bounds,UINT dpi,bool scrolling,bool enabled) noexcept {
        if(!m_hwnd) return;
        // Cached geometry belongs to one HWND, not the longer-lived content
        // object. Re-entering Pictures creates a zero-sized child even when
        // its requested bounds and DPI are identical to the previous visit.
        const bool changed=!m_layoutValid || !EqualRect(&m_bounds,&bounds) || m_dpi!=dpi || m_scrollEnabled!=scrolling;
        m_bounds=bounds; m_dpi=dpi ? dpi : 96; m_scrollEnabled=scrolling;
        if(changed) m_layoutValid=SetWindowPos(m_hwnd,nullptr,bounds.left,bounds.top,
            std::max(0L,bounds.right-bounds.left),std::max(0L,bounds.bottom-bounds.top),
            SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOCOPYBITS)!=FALSE;
        EnableWindow(m_hwnd,enabled); UpdateScrollbars();
        // Native bars are nonclient pixels. A client-only invalidation leaves
        // old tracks/corners behind after a size or scrollbar-style change.
        RedrawWindow(m_hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_FRAME|RDW_NOERASE);
    }
};
} }

// Adapted from the matching ClassicExplorer webview source: reference-zoom.inc
namespace ce
{
namespace win2kwebview
{
	UINT ImgViewAcceleratorCommand(WPARAM virtualKey) noexcept;

	class ImgViewZoomWindow
	{
		public:
			enum class Mode
			{
				Pan,
				ZoomIn,
				ZoomOut,
			};

			ImgViewZoomWindow() noexcept;
			~ImgViewZoomWindow();

			ImgViewZoomWindow(const ImgViewZoomWindow &) = delete;
			ImgViewZoomWindow &operator=(const ImgViewZoomWindow &) = delete;

			bool Create(HWND parent) noexcept;
			void Destroy() noexcept;
			HWND Window() const noexcept { return m_hwnd; }

			void SetColours(COLORREF background,COLORREF text) noexcept {
                if(m_backgroundColour==background && m_textColour==text) return;
                m_backgroundColour=background;m_textColour=text;
                if(m_hwnd) InvalidateRect(m_hwnd,nullptr,FALSE);
            }
            COLORREF BackgroundColour() const noexcept { return m_backgroundColour==CLR_INVALID ? GetSysColor(COLOR_WINDOW) : m_backgroundColour; }
            COLORREF TextColour() const noexcept { return m_textColour==CLR_INVALID ? GetSysColor(COLOR_WINDOWTEXT) : m_textColour; }
            void SetBitmap(HBITMAP bitmap) noexcept;
            void SetInteractionEnabled(bool enabled) noexcept { m_interactionEnabled=enabled; }
			void SetStatusText(const std::wstring &text) noexcept;
			bool SetMode(Mode mode) noexcept;
			void ZoomIn() noexcept;
			void ZoomOut() noexcept;
			void ActualSize() noexcept;
			void BestFit() noexcept;
			bool ShowContextMenu(HWND owner, HWND toolbar, bool showFullScreen,
			                     bool showPrint, bool printable, LPARAM position) noexcept;

		private:
			struct PointSize
			{
				LONG x = 0;
				LONG y = 0;
				LONG cx = 1;
				LONG cy = 1;
			};

			friend bool RegisterReferenceViewerClasses() noexcept;
            static bool EnsureClassRegistered() noexcept;
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam,
			                                   LPARAM lParam) noexcept;

			void AdjustRectPlacement() noexcept;
			void SetScrollBars() noexcept;
			void UpdateCursor() noexcept;
			void OnMouseDown(UINT message, WPARAM wParam, LPARAM lParam) noexcept;
			void OnMouseMove(WPARAM wParam, LPARAM lParam) noexcept;
			void OnMouseUp() noexcept;
			void OnScroll(UINT message, WPARAM wParam) noexcept;
			void OnSize(int width, int height) noexcept;
			void Paint(HDC target=nullptr) noexcept;
			bool EraseBackground(HDC dc) noexcept;

			HWND m_hwnd = nullptr;
			HBITMAP m_bitmap = nullptr; // borrowed; DiagnosticLeaseHost owns it
			int m_imageWidth = 1;
			int m_imageHeight = 1;
			int m_centerX = 1;
			int m_centerY = 1;
			int m_verticalScrollWidth = 0;
			int m_horizontalScrollHeight = 0;
			int m_windowWidth = 0;
			int m_windowHeight = 0;
			int m_mouseX = 0;
			int m_mouseY = 0;
			Mode m_defaultMode = Mode::ZoomIn;
			PointSize m_destination;
			bool m_bestFit = true;
			COLORREF m_backgroundColour=CLR_INVALID,m_textColour=CLR_INVALID;
            bool m_interactionEnabled=true;
            bool m_panning = false;
			bool m_controlDown = false;
			bool m_shiftDown = false;
			bool m_hasAlpha = false;
			std::wstring m_statusText = ReferenceCaption(L"No preview available.");
	};

	class ImgViewDetachedPreview
	{
		public:
			ImgViewDetachedPreview() = default;
			~ImgViewDetachedPreview();

			ImgViewDetachedPreview(const ImgViewDetachedPreview &) = delete;
			ImgViewDetachedPreview &operator=(const ImgViewDetachedPreview &) = delete;

			bool Show(HWND owner, HWND commandTarget, HIMAGELIST images, HIMAGELIST hotImages) noexcept;
			void Destroy() noexcept;
			void SetColours(COLORREF background,COLORREF text) noexcept { m_zoom.SetColours(background,text); }
            void SetBitmap(HBITMAP bitmap) noexcept;
			void SetStatusText(const std::wstring &text) noexcept;
			void SetSourcePath(const std::wstring &path) noexcept;
			void SetPrintable(bool printable) noexcept;

			HWND Window() const noexcept { return m_hwnd; }
			HWND ToolbarWindow() const noexcept { return m_toolbar; }
			bool HandleToolbarCommand(UINT command) noexcept;

		private:
			friend bool RegisterReferenceViewerClasses() noexcept;
            static bool EnsureClassRegistered() noexcept;
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam,
			                                   LPARAM lParam) noexcept;
			static LRESULT CALLBACK ToolbarSubclassProc(HWND hwnd, UINT message, WPARAM wParam,
			                                            LPARAM lParam, UINT_PTR id,
			                                            DWORD_PTR data) noexcept;

			bool OnCreate() noexcept;
			void Layout() noexcept;
			void UpdateTitle() noexcept;
			void UpdatePrintButton() noexcept;

			HWND m_hwnd = nullptr;
			HWND m_owner = nullptr;
			HWND m_commandTarget = nullptr;
			HWND m_toolbar = nullptr;
			HIMAGELIST m_images = nullptr;    // borrowed from the session toolbar
			HIMAGELIST m_hotImages = nullptr; // borrowed from the session toolbar
			HBITMAP m_bitmap = nullptr;        // borrowed from DiagnosticLeaseHost
			bool m_printable = false;
			std::wstring m_statusText = ReferenceCaption(L"No preview available.");
			std::wstring m_sourcePath;
			ImgViewZoomWindow m_zoom;
	};
} // namespace win2kwebview
} // namespace ce
namespace ce
{
namespace win2kwebview
{
	namespace
	{
		constexpr wchar_t kZoomClassName[] = L"ClassicWebViewPane.Reference.ImgViewZoom";
		constexpr wchar_t kDetachedClassName[] = L"ClassicWebViewPane.Reference.ImgViewDetached";
		constexpr UINT_PTR kDetachedToolbarSubclassId = 1;

		void FillToolbarBackground(HWND hwnd,HDC dc) noexcept {
            RECT fill{}; GetClientRect(hwnd,&fill);
            FillReferenceSystemColour(dc,fill,COLOR_3DFACE);
        }

		std::wstring FullScreenTitle(const std::wstring &path)
		{
			if (path.empty())
			{
				return ReferenceCaption(L"Image Preview");
			}

			const wchar_t *fileName = PathFindFileNameW(path.c_str());
			std::wstring title = (fileName && *fileName) ? fileName : path;
			SHELLFLAGSTATE flags{};
			SHGetSettings(&flags, SSF_SHOWEXTENSIONS);
			if (!flags.fShowExtensions)
			{
				const size_t extension = title.find_last_of(L'.');
				if (extension != std::wstring::npos && extension != 0)
				{
					title.erase(extension);
				}
			}
			title += ReferenceCaption(L" - Image Preview");
			return title;
		}
	} // namespace

	UINT ImgViewAcceleratorCommand(WPARAM virtualKey) noexcept
	{
		const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
		const bool control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		const bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
		const BYTE modifiers = static_cast<BYTE>((shift ? 1 : 0) |
		                                         (control ? 2 : 0) |
		                                         (alt ? 4 : 0));

		if (modifiers == 0)
		{
			switch (virtualKey)
			{
				case 'A': return ID_WIN2K_IMGVIEW_ACTUALSIZE;
				case 'B': return ID_WIN2K_IMGVIEW_BESTFIT;
				case 'F':
				case 'P': return ID_WIN2K_IMGVIEW_FULLSCREEN;
				case VK_ADD: return ID_WIN2K_IMGVIEW_ZOOMIN;
				case VK_SUBTRACT: return ID_WIN2K_IMGVIEW_ZOOMOUT;
			}
		}

		const HKL layout = GetKeyboardLayout(0);
		const auto matchesAscii = [virtualKey, modifiers, layout](wchar_t character) noexcept
		{
			const SHORT mapping = VkKeyScanExW(character, layout);
			return mapping != -1 && LOBYTE(mapping) == static_cast<BYTE>(virtualKey) &&
			       (HIBYTE(mapping) & 7) == modifiers;
		};
		if (matchesAscii(L'+')) return ID_WIN2K_IMGVIEW_ZOOMIN;
		if (matchesAscii(L'-')) return ID_WIN2K_IMGVIEW_ZOOMOUT;
		return 0;
	}

	ImgViewZoomWindow::ImgViewZoomWindow() noexcept
	{
		m_verticalScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
		m_horizontalScrollHeight = GetSystemMetrics(SM_CYHSCROLL);
	}

	ImgViewZoomWindow::~ImgViewZoomWindow()
	{
		Destroy();
	}

	bool ImgViewZoomWindow::EnsureClassRegistered() noexcept
	{


		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = &ImgViewZoomWindow::WindowProc;
		wc.hInstance = ReferenceModule();
		wc.lpszClassName = kZoomClassName;
		wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		if (!RegisterClassExW(&wc))
		{
			return false;
		}

		return true;
	}

	bool ImgViewZoomWindow::Create(HWND parent) noexcept
	{
		if (m_hwnd && IsWindow(m_hwnd))
		{
			return true;
		}
		if (!parent || !IsWindow(parent) || !g_referenceZoomRegistered)
		{
			return false;
		}

		m_defaultMode = Mode::ZoomIn;
		m_panning = false;
		m_controlDown = false;
		m_shiftDown = false;
		m_bestFit = true;
		m_hwnd = CreateWindowExW(
			0, kZoomClassName, nullptr,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
			0, 0, 0, 0, parent, nullptr, ReferenceModule(), this);
		return m_hwnd != nullptr;
	}

	void ImgViewZoomWindow::Destroy() noexcept
	{
		if (m_hwnd && IsWindow(m_hwnd))
		{
			DestroyWindow(m_hwnd);
		}
		m_hwnd = nullptr;
		m_bitmap = nullptr;
		m_panning = false;
	}

	void ImgViewZoomWindow::SetStatusText(const std::wstring &text) noexcept
	{
		m_statusText = text;
		if (m_hwnd && IsWindow(m_hwnd))
		{
			InvalidateRect(m_hwnd, nullptr, TRUE);
		}
	}

	void ImgViewZoomWindow::SetBitmap(HBITMAP bitmap) noexcept
	{
		m_bitmap = bitmap;
		m_hasAlpha = false;
		m_imageWidth = 1;
		m_imageHeight = 1;
		if (bitmap)
		{
			BITMAP info{};
			if (GetObjectW(bitmap, sizeof(info), &info))
			{
				m_imageWidth = abs(info.bmWidth);
				m_imageHeight = abs(info.bmHeight);
				// ImgView bitmaps are WIC 32bppPBGRA DIB sections. Treat the alpha channel as
				// authoritative even for a fully transparent image; scanning source-sized pixels
				// here would move decode work back onto Explorer's UI thread.
				m_hasAlpha = info.bmBitsPixel == 32 && info.bmBits && info.bmWidthBytes > 0;
			}
			else
			{
				m_bitmap = nullptr;
			}
		}

		m_centerX = m_imageWidth / 2;
		m_centerY = m_imageHeight / 2;
		if (m_hwnd && IsWindow(m_hwnd))
		{
			BestFit();
			InvalidateRect(m_hwnd, nullptr, TRUE);
		}
	}

	bool ImgViewZoomWindow::SetMode(Mode mode) noexcept
	{
		if (m_defaultMode == mode)
		{
			return false;
		}
		m_defaultMode = mode;
		UpdateCursor();
		return true;
	}

	void ImgViewZoomWindow::ZoomIn() noexcept
	{
		if (!m_bitmap)
		{
			return;
		}
		m_bestFit = false;
		m_destination.cy = static_cast<LONG>(m_destination.cy * 1.200);
		const LONG maximum = m_imageHeight > LONG_MAX / 16 ? LONG_MAX : m_imageHeight * 16;
		if (m_destination.cy >= maximum)
		{
			m_destination.cy = maximum;
		}
		m_destination.cx = MulDiv(m_destination.cy, m_imageWidth, m_imageHeight);
		AdjustRectPlacement();
	}

	void ImgViewZoomWindow::ZoomOut() noexcept
	{
		if (!m_bitmap)
		{
			return;
		}
		if (m_destination.cx <= min(m_windowWidth, m_imageWidth) &&
		    m_destination.cy <= min(m_windowHeight, m_imageHeight))
		{
			m_bestFit = true;
			return;
		}
		m_destination.cy = static_cast<LONG>(m_destination.cy * 0.833);
		m_destination.cx = MulDiv(m_destination.cy, m_imageWidth, m_imageHeight);
		AdjustRectPlacement();
	}

	void ImgViewZoomWindow::ActualSize() noexcept
	{
		m_bestFit = false;
		m_destination.cx = m_imageWidth;
		m_destination.cy = m_imageHeight;
		m_destination.x = (m_windowWidth - m_imageWidth) / 2;
		m_destination.y = (m_windowHeight - m_imageHeight) / 2;
		m_centerX = m_imageWidth / 2;
		m_centerY = m_imageHeight / 2;
		SetScrollBars();
		if (m_hwnd) InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	void ImgViewZoomWindow::BestFit() noexcept
	{
		m_bestFit = true;
		if (!m_hwnd)
		{
			return;
		}

		const DWORD style = static_cast<DWORD>(GetWindowLongPtrW(m_hwnd, GWL_STYLE));
		if (style & (WS_VSCROLL | WS_HSCROLL))
		{
			m_windowWidth += (style & WS_VSCROLL) ? m_verticalScrollWidth : 0;
			m_windowHeight += (style & WS_HSCROLL) ? m_horizontalScrollHeight : 0;
		}

		if (m_imageWidth <= m_windowWidth && m_imageHeight <= m_windowHeight)
		{
			m_destination.x = (m_windowWidth - m_imageWidth) / 2;
			m_destination.y = (m_windowHeight - m_imageHeight) / 2;
			m_destination.cx = m_imageWidth;
			m_destination.cy = m_imageHeight;
		}
		else if (static_cast<long long>(m_imageWidth) * m_windowHeight <
		         static_cast<long long>(m_windowWidth) * m_imageHeight)
		{
			const int width = MulDiv(m_windowHeight, m_imageWidth, m_imageHeight);
			m_destination.x = (m_windowWidth - width) / 2;
			m_destination.y = 0;
			m_destination.cx = width;
			m_destination.cy = m_windowHeight;
		}
		else
		{
			const int height = MulDiv(m_windowWidth, m_imageHeight, m_imageWidth);
			m_destination.x = 0;
			m_destination.y = (m_windowHeight - height) / 2;
			m_destination.cx = m_windowWidth;
			m_destination.cy = height;
		}

		if (style & (WS_VSCROLL | WS_HSCROLL))
		{
			SetScrollBars();
		}
		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	bool ImgViewZoomWindow::ShowContextMenu(HWND owner, HWND toolbar, bool showFullScreen,
	                                        bool showPrint, bool printable,
	                                        LPARAM position) noexcept
	{
		if (!m_hwnd || !IsWindow(m_hwnd) || !owner || !IsWindow(owner) ||
		    !toolbar || !IsWindow(toolbar))
		{
			return false;
		}

		HMENU menu = ReferenceMenu();
		if (!menu)
		{
			return false;
		}

		bool shown = false;
		HMENU popup = GetSubMenu(menu, 0);
		if (popup)
		{
			int x = GET_X_LPARAM(position);
			int y = GET_Y_LPARAM(position);
			if (position == -1)
			{
				RECT preview{};
				GetWindowRect(m_hwnd, &preview);
				x = preview.left;
				y = preview.top;
			}

			const int checked = static_cast<int>(
				SendMessageW(toolbar, TB_GETSTATE, ID_WIN2K_IMGVIEW_ZOOMOUT, 0)) &
				TBSTATE_CHECKED;
			CheckMenuRadioItem(popup, 0, 1, checked, MF_BYPOSITION);
			if (!showFullScreen)
			{
				RemoveMenu(popup, ID_WIN2K_IMGVIEW_FULLSCREEN, MF_BYCOMMAND);
			}
			if (!showPrint)
			{
				RemoveMenu(popup, ID_WIN2K_IMGVIEW_PRINT, MF_BYCOMMAND);
			}
			else if (!printable)
			{
				EnableMenuItem(popup, ID_WIN2K_IMGVIEW_PRINT, MF_BYCOMMAND | MF_GRAYED);
			}

			shown = true;
			TrackPopupMenuEx(popup, 0, x, y, owner, nullptr);
		}
		DestroyMenu(menu);
		return shown;
	}

	void ImgViewZoomWindow::AdjustRectPlacement() noexcept
	{
		if (!m_hwnd)
		{
			return;
		}
		const DWORD style = static_cast<DWORD>(GetWindowLongPtrW(m_hwnd, GWL_STYLE));
		if (style & (WS_VSCROLL | WS_HSCROLL))
		{
			if (m_destination.cx < m_windowWidth + ((style & WS_VSCROLL) ? m_verticalScrollWidth : 0) &&
			    m_destination.cy < m_windowHeight + ((style & WS_HSCROLL) ? m_horizontalScrollHeight : 0))
			{
				m_windowWidth += (style & WS_VSCROLL) ? m_verticalScrollWidth : 0;
				m_windowHeight += (style & WS_HSCROLL) ? m_horizontalScrollHeight : 0;
				SetScrollBars();
			}
		}

		if (m_destination.cx < m_windowWidth && m_destination.cy < m_windowHeight &&
		    m_destination.cx < m_imageWidth && m_destination.cy < m_imageHeight)
		{
			BestFit();
			return;
		}

		m_destination.x = (m_windowWidth / 2) - MulDiv(m_centerX, m_destination.cx, m_imageWidth);
		m_destination.y = (m_windowHeight / 2) - MulDiv(m_centerY, m_destination.cy, m_imageHeight);
		if (m_destination.cx < m_windowWidth)
		{
			m_destination.x = (m_windowWidth - m_destination.cx) / 2;
		}
		else
		{
			if (m_destination.x < m_windowWidth - m_destination.cx) m_destination.x = m_windowWidth - m_destination.cx;
			if (m_destination.x > 0) m_destination.x = 0;
		}
		if (m_destination.cy < m_windowHeight)
		{
			m_destination.y = (m_windowHeight - m_destination.cy) / 2;
		}
		else
		{
			if (m_destination.y < m_windowHeight - m_destination.cy) m_destination.y = m_windowHeight - m_destination.cy;
			if (m_destination.y > 0) m_destination.y = 0;
		}
		SetScrollBars();
		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	void ImgViewZoomWindow::SetScrollBars() noexcept
	{
		if (!m_hwnd)
		{
			return;
		}
		SCROLLINFO info{};
		info.cbSize = sizeof(info);
		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = m_destination.cx;
		info.nPage = static_cast<UINT>(m_windowWidth + 1);
		info.nPos = -m_destination.x;
		SetScrollInfo(m_hwnd, SB_HORZ, &info, TRUE);
		info.nMax = m_destination.cy;
		info.nPage = static_cast<UINT>(m_windowHeight + 1);
		info.nPos = -m_destination.y;
		SetScrollInfo(m_hwnd, SB_VERT, &info, TRUE);
	}

	void ImgViewZoomWindow::OnSize(int width, int height) noexcept
	{
		m_windowWidth = width;
		m_windowHeight = height;
		if (m_bestFit) BestFit(); else AdjustRectPlacement();
	}

	void ImgViewZoomWindow::OnMouseDown(UINT message, WPARAM wParam, LPARAM lParam) noexcept
	{
		m_mouseX = GET_X_LPARAM(lParam);
		m_mouseY = GET_Y_LPARAM(lParam);
		if (!m_bitmap)
		{
			return;
		}
		if ((wParam & MK_CONTROL) || message == WM_MBUTTONDOWN)
		{
			m_panning = true;
			UpdateCursor();
			SetCapture(m_hwnd);
			return;
		}

		const bool zoomIn = (m_defaultMode != Mode::ZoomOut) ^ ((wParam & MK_SHIFT) != 0);
		m_centerX = MulDiv(m_mouseX - m_destination.x, m_imageWidth, m_destination.cx);
		m_centerY = MulDiv(m_mouseY - m_destination.y, m_imageHeight, m_destination.cy);
		if (zoomIn) ZoomIn(); else ZoomOut();
	}

	void ImgViewZoomWindow::OnMouseMove(WPARAM wParam, LPARAM lParam) noexcept
	{
		m_controlDown = (wParam & MK_CONTROL) != 0;
		m_shiftDown = (wParam & MK_SHIFT) != 0;
		UpdateCursor();
		if (!(wParam & (MK_LBUTTON | MK_MBUTTON)) || !m_panning || !m_bitmap)
		{
			return;
		}

		const int x = GET_X_LPARAM(lParam);
		const int y = GET_Y_LPARAM(lParam);
		PointSize destination = m_destination;
		if (m_destination.cx > m_windowWidth) destination.x += x - m_mouseX;
		if (m_destination.cy > m_windowHeight) destination.y += y - m_mouseY;
		if (destination.cx < m_windowWidth) destination.x = (m_windowWidth - destination.cx) / 2;
		else
		{
			if (destination.x < m_windowWidth - destination.cx) destination.x = m_windowWidth - destination.cx;
			if (destination.x > 0) destination.x = 0;
		}
		if (destination.cy < m_windowHeight) destination.y = (m_windowHeight - destination.cy) / 2;
		else
		{
			if (destination.y < m_windowHeight - destination.cy) destination.y = m_windowHeight - destination.cy;
			if (destination.y > 0) destination.y = 0;
		}

		m_mouseX = x;
		m_mouseY = y;
		if (destination.x != m_destination.x || destination.y != m_destination.y)
		{
			m_destination = destination;
			SetScrollBars();
			InvalidateRect(m_hwnd, nullptr, TRUE);
		}
		m_centerX = MulDiv(m_windowWidth / 2 - m_destination.x, m_imageWidth, m_destination.cx);
		m_centerY = MulDiv(m_windowHeight / 2 - m_destination.y, m_imageHeight, m_destination.cy);
	}

	void ImgViewZoomWindow::OnMouseUp() noexcept
	{
		if (m_panning && GetCapture() == m_hwnd)
		{
			ReleaseCapture();
		}
		m_panning = false;
		UpdateCursor();
	}

	void ImgViewZoomWindow::OnScroll(UINT message, WPARAM wParam) noexcept
	{
		if (!m_bitmap)
		{
			return;
		}
		const int bar = (message == WM_HSCROLL) ? SB_HORZ : SB_VERT;
		const int window = (message == WM_HSCROLL) ? m_windowWidth : m_windowHeight;
		LONG &leading = (message == WM_HSCROLL) ? m_destination.x : m_destination.y;
		const LONG extent = (message == WM_HSCROLL) ? m_destination.cx : m_destination.cy;
		if (window >= extent)
		{
			return;
		}
		switch (LOWORD(wParam))
		{
			case SB_TOP: leading = 0; break;
			case SB_PAGEUP: leading += window; break;
			case SB_LINEUP: ++leading; break;
			case SB_LINEDOWN: --leading; break;
			case SB_PAGEDOWN: leading -= window; break;
			case SB_BOTTOM: leading = window - extent; break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
			{
				SCROLLINFO info{};
				info.cbSize = sizeof(info);
				info.fMask = SIF_TRACKPOS;
				GetScrollInfo(m_hwnd, bar, &info);
				leading = -info.nTrackPos;
				break;
			}
			case SB_ENDSCROLL: return;
		}
		if (leading > 0) leading = 0;
		else if (window - extent > leading) leading = window - extent;
		SetScrollPos(m_hwnd, bar, -leading, TRUE);
		if (message == WM_HSCROLL)
			m_centerX = MulDiv(m_windowWidth / 2 - m_destination.x, m_imageWidth, m_destination.cx);
		else
			m_centerY = MulDiv(m_windowHeight / 2 - m_destination.y, m_imageHeight, m_destination.cy);
		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	void ImgViewZoomWindow::UpdateCursor() noexcept
	{
		if (!m_bitmap)
		{
			return;
		}
		UINT cursorId = IDC_WIN2K_IMGVIEW_ZOOMOUT;
		if (m_panning) cursorId = IDC_WIN2K_IMGVIEW_CLOSEDHAND;
		else if (m_controlDown) cursorId = IDC_WIN2K_IMGVIEW_OPENHAND;
		else if ((m_defaultMode == Mode::ZoomIn) ^ m_shiftDown) cursorId = IDC_WIN2K_IMGVIEW_ZOOMIN;
		HCURSOR cursor = ReferenceCursor(cursorId);
		if (cursor) SetCursor(cursor);
	}

	bool ImgViewZoomWindow::EraseBackground(HDC dc) noexcept
	{
		if (!dc)
		{
			return false;
		}
		RECT fill{};
		if (m_hasAlpha || !m_bitmap)
		{
			GetClientRect(m_hwnd, &fill);
			FillReferenceColour(dc,fill,BackgroundColour());
			return true;
		}

		fill = { 0, m_destination.y, m_destination.x, m_destination.y + m_destination.cy };
		if (fill.right > fill.left) FillReferenceColour(dc,fill,BackgroundColour());
		fill = { m_destination.x + m_destination.cx, m_destination.y,
		         m_windowWidth, m_destination.y + m_destination.cy };
		if (fill.right > fill.left) FillReferenceColour(dc,fill,BackgroundColour());
		fill = { 0, 0, m_windowWidth, m_destination.y };
		if (fill.bottom > fill.top) FillReferenceColour(dc,fill,BackgroundColour());
		fill = { 0, m_destination.y + m_destination.cy, m_windowWidth, m_windowHeight };
		if (fill.bottom > fill.top) FillReferenceColour(dc,fill,BackgroundColour());
		return true;
	}



	LRESULT CALLBACK ImgViewZoomWindow::WindowProc(HWND hwnd, UINT message, WPARAM wParam,
	                                                LPARAM lParam) noexcept
	{
		auto *self = reinterpret_cast<ImgViewZoomWindow *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		if (message == WM_NCCREATE)
		{
			const auto *create = reinterpret_cast<const CREATESTRUCTW *>(lParam);
			self = static_cast<ImgViewZoomWindow *>(create->lpCreateParams);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
			if (self) self->m_hwnd = hwnd;
		}
		if (!self)
		{
			return DefWindowProcW(hwnd, message, wParam, lParam);
		}

		if(RetainedViewerFrame::PaintMessage(hwnd,message,wParam)) return 0;
        if (!self->m_interactionEnabled && (message==WM_MOUSEWHEEL || message==WM_KEYDOWN ||
            message==WM_KEYUP || message==WM_LBUTTONDOWN || message==WM_MBUTTONDOWN ||
            message==WM_HSCROLL || message==WM_VSCROLL || message==WM_MOUSEMOVE)) return 0;
        switch (message)
        {
            case WM_ERASEBKGND: return TRUE;
			case WM_PAINT: self->Paint(); return 0;
            case WM_PRINTCLIENT: self->Paint(reinterpret_cast<HDC>(wParam)); return 0;
			case WM_SIZE: self->OnSize(LOWORD(lParam), HIWORD(lParam)); return 0;
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN: self->OnMouseDown(message, wParam, lParam); return 0;
			case WM_LBUTTONUP:
			case WM_MBUTTONUP: self->OnMouseUp(); return 0;
			case WM_MOUSEMOVE: self->OnMouseMove(wParam, lParam); return 0;
			case WM_CAPTURECHANGED:
			case WM_CANCELMODE: self->OnMouseUp(); return 0;
			case WM_HSCROLL:
			case WM_VSCROLL: self->OnScroll(message, wParam); return 0;
			case WM_MOUSEWHEEL:
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) self->ZoomIn(); else self->ZoomOut();
				return TRUE;
			case WM_KEYDOWN:
				switch (wParam)
				{
					case VK_PRIOR: self->OnScroll(WM_VSCROLL, self->m_controlDown ? SB_TOP : SB_PAGEUP); break;
					case VK_NEXT: self->OnScroll(WM_VSCROLL, self->m_controlDown ? SB_BOTTOM : SB_PAGEDOWN); break;
					case VK_END: self->OnScroll(WM_HSCROLL, self->m_controlDown ? SB_BOTTOM : SB_PAGEDOWN); break;
					case VK_HOME: self->OnScroll(WM_HSCROLL, self->m_controlDown ? SB_TOP : SB_PAGEUP); break;
					case VK_CONTROL: if (!self->m_panning) self->m_controlDown = true; self->UpdateCursor(); break;
					case VK_SHIFT: if (!self->m_panning) self->m_shiftDown = true; self->UpdateCursor(); break;
				}
				break;
			case WM_KEYUP:
				if (wParam == VK_CONTROL) self->m_controlDown = false;
				else if (wParam == VK_SHIFT) self->m_shiftDown = false;
				self->UpdateCursor();
				break;
			case WM_SETCURSOR:
				if (LOWORD(lParam) == HTCLIENT && self->m_bitmap)
				{
					self->UpdateCursor();
					return TRUE;
				}
				break;
			case WM_SETFOCUS:
				if (HWND parent = GetParent(hwnd)) SetFocus(parent);
				return 0;
			case WM_NCDESTROY:
				SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
				self->m_hwnd = nullptr;
				self->m_bitmap = nullptr;
				self->m_panning = false;
				break;
		}
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	ImgViewDetachedPreview::~ImgViewDetachedPreview()
	{
		Destroy();
	}

	bool ImgViewDetachedPreview::EnsureClassRegistered() noexcept
	{

		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = &ImgViewDetachedPreview::WindowProc;
		wc.hInstance = ReferenceModule();
		wc.lpszClassName = kDetachedClassName;
		wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		if (!RegisterClassExW(&wc)) return false;

		return true;
	}

	bool ImgViewDetachedPreview::Show(HWND owner, HWND commandTarget, HIMAGELIST images,
	                                  HIMAGELIST hotImages) noexcept
	{
		m_owner = owner;
		m_commandTarget = commandTarget;
		m_images = images;
		m_hotImages = hotImages;
		if (m_hwnd && IsWindow(m_hwnd))
		{
			if (IsIconic(m_hwnd)) ShowWindow(m_hwnd, SW_RESTORE);
			SetForegroundWindow(m_hwnd);
			return true;
		}
		if (!owner || !IsWindow(owner) || !commandTarget || !IsWindow(commandTarget) ||
		    !images || !hotImages || !g_referenceDetachedRegistered)
		{
			return false;
		}

		// WS_EX_APPWINDOW: the preview keeps `owner` so it stays in front of the folder window and
		// is destroyed with it, but an owned window is normally left off the taskbar. Windows 2000
		// gave the image preview its own task button, and this is the documented way to keep both
		// properties at once.
		const std::wstring title = FullScreenTitle(m_sourcePath);
        MONITORINFO monitor{sizeof(monitor)};
        GetMonitorInfoW(MonitorFromWindow(owner,MONITOR_DEFAULTTONEAREST),&monitor);
		m_hwnd = CreateWindowExW(
			WS_EX_APPWINDOW, kDetachedClassName, title.c_str(),
			WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_VISIBLE,
			monitor.rcWork.left, monitor.rcWork.top, 640, 480, owner, nullptr,
			ReferenceModule(), this);
		return m_hwnd != nullptr;
	}

	bool ImgViewDetachedPreview::OnCreate() noexcept
	{
		INITCOMMONCONTROLSEX controls{ sizeof(controls), ICC_BAR_CLASSES };
		if (!InitCommonControlsEx(&controls)) return false;
		const HICON icon = ReferenceIcon(IDI_WIN2K_IMGVIEW_FULLSCREEN);
		if (icon)
		{
			SendMessageW(m_hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
			SendMessageW(m_hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
		}
		m_toolbar = CreateWindowExW(
			0, TOOLBARCLASSNAMEW, nullptr,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
			CCS_NODIVIDER | CCS_TOP | TBSTYLE_FLAT | (g_webOptions.load()->tooltips ? TBSTYLE_TOOLTIPS : 0),
			0, 0, 0, 0, m_hwnd, nullptr, ReferenceModule(), nullptr);
		if (!m_toolbar) return false;
		SetWindowTheme(m_toolbar, L"", L"");
		SendMessageW(m_toolbar, CCM_SETVERSION, 5, 0);
		SendMessageW(m_toolbar,CCM_SETUNICODEFORMAT,TRUE,0);
        SendMessageW(m_toolbar,TB_SETEXTENDEDSTYLE,0,TBSTYLE_EX_MIXEDBUTTONS);
        SendMessageW(m_toolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
		SendMessageW(m_toolbar, TB_SETMAXTEXTROWS, 0, 0);
		int imageWidth=24,imageHeight=24;
        ImageList_GetIconSize(m_images,&imageWidth,&imageHeight);
        SendMessageW(m_toolbar, TB_SETBITMAPSIZE, 0, MAKELONG(imageWidth, imageHeight));
		SendMessageW(m_toolbar, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(m_images));
		SendMessageW(m_toolbar, TB_SETHOTIMAGELIST, 0, reinterpret_cast<LPARAM>(m_hotImages));

		TBBUTTON buttons[7]{};
		buttons[0] = { 0, ID_WIN2K_IMGVIEW_ZOOMIN, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_CHECKGROUP };
		buttons[1] = { 1, ID_WIN2K_IMGVIEW_ZOOMOUT, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP };
		buttons[2] = { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP };
		buttons[3] = { 2, ID_WIN2K_IMGVIEW_ACTUALSIZE, TBSTATE_ENABLED, TBSTYLE_BUTTON };
		buttons[4] = { 3, ID_WIN2K_IMGVIEW_BESTFIT, TBSTATE_ENABLED, TBSTYLE_BUTTON };
		buttons[5] = { 4, ID_WIN2K_IMGVIEW_FULLSCREEN, TBSTATE_HIDDEN, TBSTYLE_BUTTON };
		buttons[6] = { 5, ID_WIN2K_IMGVIEW_PRINT,
		               static_cast<BYTE>(m_printable ? TBSTATE_ENABLED : 0), TBSTYLE_BUTTON };
		const wchar_t* labels[]={ReferenceCaption(ReferenceCaption(L"Zoom In")),ReferenceCaption(ReferenceCaption(L"Zoom Out")),L"",
            ReferenceCaption(ReferenceCaption(L"Actual Size")),ReferenceCaption(ReferenceCaption(L"Best Fit")),ReferenceCaption(ReferenceCaption(L"Full Screen")),ReferenceCaption(ReferenceCaption(L"Print"))};
        for (size_t i=0;i<ARRAYSIZE(buttons);++i) if(i!=2) buttons[i].iString=reinterpret_cast<INT_PTR>(labels[i]);
        if (!SendMessageW(m_toolbar, TB_ADDBUTTONS, ARRAYSIZE(buttons),
		                  reinterpret_cast<LPARAM>(buttons))) return false;
		if (!g_webOptions.load()->zoom) for (UINT id=ID_WIN2K_IMGVIEW_ZOOMIN;id<=ID_WIN2K_IMGVIEW_BESTFIT;++id)
            SendMessageW(m_toolbar,TB_HIDEBUTTON,id,TRUE);
        if (!g_webOptions.load()->print) SendMessageW(m_toolbar,TB_HIDEBUTTON,ID_WIN2K_IMGVIEW_PRINT,TRUE);
        SendMessageW(m_toolbar, TB_SETBUTTONWIDTH, 0, MAKELONG(imageWidth, imageWidth));
		SendMessageW(m_toolbar, TB_AUTOSIZE, 0, 0);
		if (!SetWindowSubclass(m_toolbar, &ImgViewDetachedPreview::ToolbarSubclassProc,
		                       kDetachedToolbarSubclassId, reinterpret_cast<DWORD_PTR>(this))) return false;
		if (!m_zoom.Create(m_hwnd)) return false;
		m_zoom.SetInteractionEnabled(g_webOptions.load()->zoom);
        m_zoom.SetStatusText(m_statusText);
		m_zoom.SetBitmap(m_bitmap);
		Layout();
		return true;
	}

	void ImgViewDetachedPreview::Destroy() noexcept
	{
		m_zoom.SetBitmap(nullptr);
		if (m_hwnd && IsWindow(m_hwnd)) DestroyWindow(m_hwnd);
		m_zoom.Destroy();
		m_hwnd = nullptr;
		m_toolbar = nullptr;
		m_bitmap = nullptr;
		m_owner = nullptr;
		m_commandTarget = nullptr;
		m_images = nullptr;
		m_hotImages = nullptr;
	}

	void ImgViewDetachedPreview::SetBitmap(HBITMAP bitmap) noexcept
	{
		m_bitmap = bitmap;
		m_zoom.SetBitmap(bitmap);
	}

	void ImgViewDetachedPreview::SetStatusText(const std::wstring &text) noexcept
	{
		m_statusText = text;
		m_zoom.SetStatusText(text);
	}

	void ImgViewDetachedPreview::SetSourcePath(const std::wstring &path) noexcept
	{
		m_sourcePath = path;
		UpdateTitle();
	}

	void ImgViewDetachedPreview::SetPrintable(bool printable) noexcept
	{
		m_printable = printable;
		UpdatePrintButton();
	}

	void ImgViewDetachedPreview::UpdateTitle() noexcept
	{
		if (m_hwnd && IsWindow(m_hwnd))
		{
			SetWindowTextW(m_hwnd, FullScreenTitle(m_sourcePath).c_str());
		}
	}

	void ImgViewDetachedPreview::UpdatePrintButton() noexcept
	{
		if (m_toolbar && IsWindow(m_toolbar))
		{
			SendMessageW(m_toolbar, TB_ENABLEBUTTON, ID_WIN2K_IMGVIEW_PRINT,
			             MAKELONG(m_printable ? TRUE : FALSE, 0));
		}
	}

	bool ImgViewDetachedPreview::HandleToolbarCommand(UINT command) noexcept
	{
		if (!g_webOptions.load()->zoom && command!=ID_WIN2K_IMGVIEW_PRINT) return false;
        switch (command)
        {
			case ID_WIN2K_IMGVIEW_ZOOMIN:
				if (!m_zoom.SetMode(ImgViewZoomWindow::Mode::ZoomIn)) m_zoom.ZoomIn();
				else
				{
					SendMessageW(m_toolbar, TB_SETSTATE, ID_WIN2K_IMGVIEW_ZOOMOUT, TBSTATE_ENABLED);
					SendMessageW(m_toolbar, TB_SETSTATE, ID_WIN2K_IMGVIEW_ZOOMIN,
					             TBSTATE_ENABLED | TBSTATE_CHECKED);
				}
				return true;
			case ID_WIN2K_IMGVIEW_ZOOMOUT:
				if (!m_zoom.SetMode(ImgViewZoomWindow::Mode::ZoomOut)) m_zoom.ZoomOut();
				else
				{
					SendMessageW(m_toolbar, TB_SETSTATE, ID_WIN2K_IMGVIEW_ZOOMIN, TBSTATE_ENABLED);
					SendMessageW(m_toolbar, TB_SETSTATE, ID_WIN2K_IMGVIEW_ZOOMOUT,
					             TBSTATE_ENABLED | TBSTATE_CHECKED);
				}
				return true;
			case ID_WIN2K_IMGVIEW_ACTUALSIZE: m_zoom.ActualSize(); return true;
			case ID_WIN2K_IMGVIEW_BESTFIT: m_zoom.BestFit(); return true;
			case ID_WIN2K_IMGVIEW_FULLSCREEN: return true;
			case ID_WIN2K_IMGVIEW_PRINT:
				if (m_printable && m_commandTarget && IsWindow(m_commandTarget))
				{
					SendMessageW(m_commandTarget, WM_COMMAND,
					             MAKEWPARAM(ID_WIN2K_IMGVIEW_PRINT, 0),
					             reinterpret_cast<LPARAM>(m_toolbar));
				}
				return true;
		}
		return false;
	}

	void ImgViewDetachedPreview::Layout() noexcept
	{
		if (!m_hwnd || !m_toolbar || !m_zoom.Window()) return;
		RECT client{};
		GetClientRect(m_hwnd, &client);
		SendMessageW(m_toolbar, TB_AUTOSIZE, 0, 0);
		RECT toolbar{};
		GetClientRect(m_toolbar, &toolbar);
		const int height = toolbar.bottom - toolbar.top;
		SetWindowPos(m_toolbar, nullptr, 0, 0, client.right - client.left, height,
		             SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
		SetWindowPos(m_zoom.Window(), nullptr, 0, height, client.right - client.left,
		             max(0, client.bottom - client.top - height),
		             SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}

	LRESULT CALLBACK ImgViewDetachedPreview::ToolbarSubclassProc(
		HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data) noexcept
	{
		auto *self = reinterpret_cast<ImgViewDetachedPreview *>(data);
		if(PreviewToolbarPaintMessage(hwnd,message,wParam)) return 0;
        if (message == WM_ERASEBKGND)
		{
			FillToolbarBackground(hwnd, reinterpret_cast<HDC>(wParam));
			return TRUE;
		}
		if ((message == WM_KEYDOWN || message == WM_KEYUP) && self && self->m_zoom.Window())
		{
			SendMessageW(self->m_zoom.Window(), message, wParam, lParam);
			if (message == WM_KEYDOWN)
			{
				const UINT command = ImgViewAcceleratorCommand(wParam);
				if (command)
				{
					self->HandleToolbarCommand(command);
					return 0;
				}
			}
		}
		if (message == WM_NCDESTROY)
		{
			RemoveWindowSubclass(hwnd, &ImgViewDetachedPreview::ToolbarSubclassProc, id);
		}
		return DefSubclassProc(hwnd, message, wParam, lParam);
	}

	LRESULT CALLBACK ImgViewDetachedPreview::WindowProc(HWND hwnd, UINT message, WPARAM wParam,
	                                                     LPARAM lParam) noexcept
	{
		auto *self = reinterpret_cast<ImgViewDetachedPreview *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		if (message == WM_NCCREATE)
		{
			const auto *create = reinterpret_cast<const CREATESTRUCTW *>(lParam);
			self = static_cast<ImgViewDetachedPreview *>(create->lpCreateParams);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
			if (self) self->m_hwnd = hwnd;
		}
		if (!self) return DefWindowProcW(hwnd, message, wParam, lParam);
		switch (message)
		{
			case WM_CREATE: return self->OnCreate() ? 0 : -1;
			case WM_ERASEBKGND:
			{
				// A TBSTYLE_FLAT toolbar is transparent: comctl32 draws the buttons but leaves the
				// strip around them to the parent, forwarding the erase with its own DC. Returning
				// TRUE without painting therefore left those pixels as whatever was already in the
				// DC, which is what showed up as a black band beside the buttons.
				//
				// The in-pane profile does not hit this because its toolbar sits on a host window
				// that paints the same strip (ImgViewToolbarHostSubclassProc). Here the frame is
				// the parent, so it has to do it. Only the toolbar's own rectangle is filled — the
				// zoom child owns everything below it and must not be erased under.
				if (self->m_toolbar && IsWindow(self->m_toolbar))
				{
					FillToolbarBackground(self->m_toolbar, reinterpret_cast<HDC>(wParam));
				}
				return TRUE;
			}
			case WM_SIZE: self->Layout(); return 0;
			case WM_SETFOCUS:
				if (self->m_toolbar) SetFocus(self->m_toolbar);
				return 0;
			case WM_MOUSEWHEEL:
				if (self->m_zoom.Window()) SendMessageW(self->m_zoom.Window(), message, wParam, lParam);
				return 0;
			case WM_CONTEXTMENU:
				if ((reinterpret_cast<HWND>(wParam) == self->m_zoom.Window() ||
				     reinterpret_cast<HWND>(wParam) == self->m_toolbar) &&
				    self->m_zoom.ShowContextMenu(self->m_hwnd, self->m_toolbar, false, g_webOptions.load()->print,
				                                 self->m_printable, lParam))
				{
					return 0;
				}
				break;
			case WM_COMMAND:
				if (reinterpret_cast<HWND>(lParam) == self->m_toolbar &&
				    self->HandleToolbarCommand(LOWORD(wParam))) return 0;
				if (!lParam && LOWORD(wParam) >= ID_WIN2K_IMGVIEW_ZOOMIN &&
				    LOWORD(wParam) <= ID_WIN2K_IMGVIEW_PRINT &&
				    self->HandleToolbarCommand(LOWORD(wParam))) return 0;
				break;
			case WM_NOTIFY:
			{
				const NMHDR *header = reinterpret_cast<const NMHDR *>(lParam);
				if (header && header->hwndFrom == self->m_toolbar)
				{
					if (header->code == TTN_GETDISPINFOW)
					{
						auto *info = reinterpret_cast<NMTTDISPINFOW *>(lParam);
						info->hinst = ReferenceModule();
						info->lpszText = MAKEINTRESOURCEW(header->idFrom);
						return 0;
					}
					if (header->code == TTN_GETDISPINFOA)
					{
						auto *info = reinterpret_cast<NMTTDISPINFOA *>(lParam);
						info->hinst = ReferenceModule();
						info->lpszText = MAKEINTRESOURCEA(header->idFrom);
						return 0;
					}
				}
				break;
			}
			case WM_CLOSE: DestroyWindow(hwnd); return 0;
			case WM_NCDESTROY:
				self->m_zoom.Destroy();
				SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
				self->m_toolbar = nullptr;
				self->m_hwnd = nullptr;
				break;
		}
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}
} // namespace win2kwebview
} // namespace ce

// Independent opaque painter: every paint supplies all margins and image
// pixels for the current size. No separate erase phase can expose old content.
namespace ce { namespace win2kwebview {
void ImgViewZoomWindow::Paint(HDC target) noexcept {
    PAINTSTRUCT paint{};
    HDC output=target ? target : BeginPaint(m_hwnd,&paint);
    if(!output) return;
    RECT client{}; GetClientRect(m_hwnd,&client);
    if(client.right>0 && client.bottom>0) {
        HDC memory=CreateCompatibleDC(output);
        HBITMAP bitmap=memory ? CreateCompatibleBitmap(output,client.right,client.bottom) : nullptr;
        HDC dc=memory && bitmap ? memory : output;
        const auto oldBitmap=bitmap ? SelectObject(memory,bitmap) : nullptr;
        const int saved=SaveDC(dc);
        if(saved) {
            SetMapMode(dc,MM_TEXT);
            FillReferenceColour(dc,client,BackgroundColour());
            if(m_bitmap && m_destination.cx>0 && m_destination.cy>0) {
                HDC source=CreateCompatibleDC(dc);
                if(source) {
                    const auto oldSource=SelectObject(source,m_bitmap);
                    SetStretchBltMode(dc,COLORONCOLOR);
                    const BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
                    const bool blended=m_hasAlpha && AlphaBlend(dc,m_destination.x,m_destination.y,
                        m_destination.cx,m_destination.cy,source,0,0,m_imageWidth,m_imageHeight,blend);
                    if(!blended) StretchBlt(dc,m_destination.x,m_destination.y,m_destination.cx,m_destination.cy,
                        source,0,0,m_imageWidth,m_imageHeight,SRCCOPY);
                    SelectObject(source,oldSource);DeleteDC(source);
                }
            } else if(!m_bitmap) {
                LOGFONTW description{};
                HFONT font=nullptr;
                if(ReferenceIconFont(m_hwnd,description)) {
                    if(g_webOptions.load()->disableSmoothing) description.lfQuality=NONANTIALIASED_QUALITY;
                    font=CreateFontIndirectW(&description);
                }
                const auto oldFont=font ? SelectObject(dc,font) : nullptr;
                SetTextColor(dc,TextColour());SetBkMode(dc,TRANSPARENT);
                DrawTextW(dc,m_statusText.c_str(),-1,&client,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
                if(oldFont) SelectObject(dc,oldFont);if(font) DeleteObject(font);
            }
            RestoreDC(dc,saved);
            if(bitmap) BitBlt(output,0,0,client.right,client.bottom,memory,0,0,SRCCOPY);
        }
        if(oldBitmap) SelectObject(memory,oldBitmap);
        if(bitmap) DeleteObject(bitmap);if(memory) DeleteDC(memory);
    }
    if(!target) EndPaint(m_hwnd,&paint);
}
} }

namespace ce { namespace win2kwebview {
bool RegisterReferenceViewerClasses() noexcept {
    if(g_referenceZoomRegistered || g_referenceDetachedRegistered || g_referenceDetailsRegistered) return false;
    if(!ImgViewDetailsWindow::Register()) return false;
    g_referenceDetailsRegistered=true;
    if(!ImgViewZoomWindow::EnsureClassRegistered()) { FreeReferenceSharedResources(); return false; }
    g_referenceZoomRegistered=true;
    if(!ImgViewDetachedPreview::EnsureClassRegistered()) {
        FreeReferenceSharedResources(); return false;
    }
    g_referenceDetachedRegistered=true;
    return true;
}
} }

namespace ce { namespace win2kwebview { namespace printerjobeta {
constexpr DWORD kError=MAXDWORD;
struct Job { DWORD status=0,totalPages=0,size=0; };
// A deliberately simple estimate. Idle queues remain valid with an unknown
// driver rate; byte-only jobs use the reference UI's nominal 4800-byte page.
inline DWORD WaitingMinutes(DWORD rate,const Job* jobs,size_t count) noexcept {
    if(!count) return 0;
    if(!jobs || !rate || rate==kError) return kError;
    unsigned long long pages=0;
    for(size_t i=0;i<count;++i) {
        const auto& job=jobs[i];
        constexpr DWORD inactive=JOB_STATUS_PAUSED|JOB_STATUS_PRINTED|JOB_STATUS_DELETING|
            JOB_STATUS_OFFLINE|JOB_STATUS_SPOOLING;
        if(job.status&inactive) continue;
        if(job.status&(JOB_STATUS_ERROR|JOB_STATUS_PAPEROUT)) return kError;
        pages+=job.totalPages ? job.totalPages : (job.size ? job.size/4800ULL+1 : 0);
    }
    if(!pages) return 0;
    return static_cast<DWORD>(std::min<unsigned long long>(pages/rate+1,kError-1ULL));
}
inline std::wstring FormatWaitingTime(DWORD minutes) {
    if(!minutes) return L"0";
    if(minutes>480) return ReferenceCaption(L"longer than 8 hours");
    return ReferenceCaption(L"about ")+std::to_wstring(minutes>60 ? (minutes+30)/60 : minutes)+
        (minutes>60 ? ReferenceCaption(L" hour(s)") : ReferenceCaption(L" minute(s)"));
}
} } }
namespace ce { namespace win2kwebview { namespace {
		unsigned long long ThumbnailPathHash(const std::wstring &path) noexcept
		{
			// Stable evidence correlation only. Never put the selected path itself in the log.
			unsigned long long hash = 14695981039346656037ULL;
			for (wchar_t character : path)
			{
				hash ^= static_cast<unsigned short>(character);
				hash *= 1099511628211ULL;
			}
			return hash;
		}
		const wchar_t *ThumbnailPathRejection(const std::wstring &path) noexcept
		{
			if (path.empty())
			{
				return L"no filesystem path";
			}
			if (PathIsRootW(path.c_str()))
			{
				return L"root path";
			}

			const DWORD attributes = GetFileAttributesW(path.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES)
			{
				return L"file unavailable";
			}
			if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				return L"directory";
			}
			if (attributes & FILE_ATTRIBUTE_OFFLINE)
			{
				return L"offline file";
			}
			wchar_t root[MAX_PATH]{};
			if (!GetVolumePathNameW(path.c_str(), root, ARRAYSIZE(root)))
			{
				return L"volume unresolved";
			}
			const UINT driveType = GetDriveTypeW(root);
			if (driveType == DRIVE_NO_ROOT_DIR)
			{
				return L"slow volume";
			}

			return nullptr;
		}
		std::wstring FormatShellProperty(IShellItem2 *item, REFPROPERTYKEY key) noexcept
		{
			if (!item)
			{
				return {};
			}

			PROPVARIANT value{};
			PropVariantInit(&value);
			if (FAILED(item->GetProperty(key, &value)))
			{
				return {};
			}

			PWSTR formatted = nullptr;
			const HRESULT hr = PSFormatForDisplayAlloc(key, value, PDFF_DEFAULT, &formatted);
			PropVariantClear(&value);
			if (FAILED(hr) || !formatted)
			{
				CoTaskMemFree(formatted);
				return {};
			}

			std::wstring result(formatted);
			CoTaskMemFree(formatted);
			return result;
		}
		std::wstring FormatShellProperty(IShellItem2 *item, PCWSTR canonicalName) noexcept
		{
			PROPERTYKEY key{};
			if (!canonicalName || FAILED(PSGetPropertyKeyFromName(canonicalName, &key)))
			{
				return {};
			}
			return FormatShellProperty(item, key);
		}
		HRESULT DecodeFullResolutionBitmap(const std::wstring &path, HBITMAP &bitmap,
		                                   SIZE &sourceSize) noexcept
		{
			bitmap = nullptr;
			sourceSize = {};

			CComPtr<IWICImagingFactory> factory;
			HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
			                              IID_PPV_ARGS(&factory));
			if (FAILED(hr) || !factory)
			{
				return FAILED(hr) ? hr : E_NOINTERFACE;
			}

			CComPtr<IWICBitmapDecoder> decoder;
			hr = factory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
			                                        WICDecodeMetadataCacheOnLoad, &decoder);
			if (FAILED(hr) || !decoder)
			{
				return FAILED(hr) ? hr : E_FAIL;
			}

			CComPtr<IWICBitmapFrameDecode> frame;
			hr = decoder->GetFrame(0, &frame);
			if (FAILED(hr) || !frame)
			{
				return FAILED(hr) ? hr : E_FAIL;
			}

			UINT width = 0;
			UINT height = 0;
			hr = frame->GetSize(&width, &height);
			if (FAILED(hr) || width == 0 || height == 0 ||
			    width > static_cast<UINT>((std::numeric_limits<LONG>::max)()) ||
			    height > static_cast<UINT>((std::numeric_limits<LONG>::max)()) ||
			    width > (std::numeric_limits<UINT>::max)() / 4)
			{
				return FAILED(hr) ? hr : HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
			}

			const UINT stride = width * 4;
			if (height > (std::numeric_limits<UINT>::max)() / stride)
			{
				return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
			}
			const UINT byteCount = stride * height;
            // Limit full-resolution pane images to 32 megapixels / 128 MiB.
            // Preserve actual-size semantics or fail without a huge allocation.
            if (width > 32768 || height > 32768 || byteCount > 128*1024*1024)
                return E_OUTOFMEMORY;

			CComPtr<IWICFormatConverter> converter;
			hr = factory->CreateFormatConverter(&converter);
			if (FAILED(hr) || !converter)
			{
				return FAILED(hr) ? hr : E_FAIL;
			}
			hr = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA,
			                           WICBitmapDitherTypeNone, nullptr, 0.0,
			                           WICBitmapPaletteTypeCustom);
			if (FAILED(hr))
			{
				return hr;
			}

			BITMAPINFO info{};
			info.bmiHeader.biSize = sizeof(info.bmiHeader);
			info.bmiHeader.biWidth = static_cast<LONG>(width);
			info.bmiHeader.biHeight = -static_cast<LONG>(height);
			info.bmiHeader.biPlanes = 1;
			info.bmiHeader.biBitCount = 32;
			info.bmiHeader.biCompression = BI_RGB;
			void *bits = nullptr;
			HBITMAP decoded = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
			if (!decoded || !bits)
			{
				if (decoded) DeleteObject(decoded);
				return E_OUTOFMEMORY;
			}

			hr = converter->CopyPixels(nullptr, stride, byteCount, static_cast<BYTE *>(bits));
			if (FAILED(hr))
			{
				DeleteObject(decoded);
				return hr;
			}

			bitmap = decoded;
			sourceSize.cx = static_cast<LONG>(width);
			sourceSize.cy = static_cast<LONG>(height);
			return S_OK;
		}
		constexpr wchar_t kAddPrinterSentinel[] = L"WinUtils_NewObject";

		// GetDetailsOf(item, column) — or the column's title when `item` is empty, which is how
		// the script gets localised labels (standard.htt:141 "GetDetailsOf(null, i)").
		std::wstring DetailOf(IShellFolderViewDual *pView, FolderItem *pItem, int column) noexcept
		{
			if (!pView)
			{
				return {};
			}
			CComPtr<Folder> folder;
			if (FAILED(pView->get_Folder(&folder)) || !folder)
			{
				return {};
			}

			CComVariant item;               // VT_EMPTY asks for the column heading
			if (pItem)
			{
				item = static_cast<IDispatch *>(pItem);
			}

			CComBSTR text;
			if (FAILED(folder->GetDetailsOf(item, column, &text)) || !text)
			{
				return {};
			}
			return std::wstring(text, text.Length());
		}

		int FindDetailColumn(IShellFolderViewDual *pView, PCWSTR title,
		                     PCWSTR alternateTitle = nullptr) noexcept
		{
			for (int column = 1; column < 32; ++column)
			{
				const std::wstring candidate = DetailOf(pView, nullptr, column);
				if (_wcsicmp(candidate.c_str(), title) == 0 ||
				    (alternateTitle && _wcsicmp(candidate.c_str(), alternateTitle) == 0))
				{
					return column;
				}
			}
			return -1;
		}

		std::wstring PrinterSupportValue(PCWSTR valueName) noexcept
		{
			// oleprn/prturl.cpp: the policy strings are REG_SZ values capped at 255 characters.
			constexpr wchar_t kPolicyPath[] = L"Software\\Policies\\Microsoft\\Windows NT\\Printers";
			wchar_t buffer[256]{};
			DWORD type = 0;
			DWORD bytes = sizeof(buffer);
			HKEY key = nullptr;
			if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, kPolicyPath, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
			{
				return {};
			}

			const LSTATUS status = RegQueryValueExW(key, valueName, nullptr, &type,
			                                             reinterpret_cast<BYTE *>(buffer), &bytes);
			RegCloseKey(key);
			if (status != ERROR_SUCCESS || type != REG_SZ)
			{
				return {};
			}

			buffer[ARRAYSIZE(buffer) - 1] = L'\0';
			return buffer;
		}

		std::wstring CommentOf(FolderItem* item) noexcept {
    try {
        CComBSTR path;
        CComPtr<IShellItem2> properties;
        if(item && SUCCEEDED(item->get_Path(&path)) && path &&
           SUCCEEDED(SHCreateItemFromParsingName(path,nullptr,IID_PPV_ARGS(&properties)))) {
            PWSTR value=nullptr;
            const HRESULT status=properties->GetString(PKEY_Comment,&value);
            const std::unique_ptr<wchar_t,decltype(&CoTaskMemFree)> owned(value,CoTaskMemFree);
            if(SUCCEEDED(status) && owned) return owned.get();
        }
        CComQIPtr<FolderItem2> automation(item);
        CComVariant value;
        if(automation && SUCCEEDED(automation->ExtendedProperty(CComBSTR(L"System.Comment"),&value)) && value.vt==VT_BSTR)
            return value.bstrVal ? value.bstrVal : L"";
    } catch(...) {}
    return {};
}

		std::wstring StringPropertyOf(FolderItem *pItem, const wchar_t *propertyName) noexcept
		{
			CComQIPtr<FolderItem2> item2(pItem);
			if (!item2 || !propertyName)
			{
				return {};
			}

			CComVariant value;
			CComBSTR property(propertyName);
			if (FAILED(item2->ExtendedProperty(property, &value)) || value.vt == VT_EMPTY || value.vt == VT_NULL)
			{
				return {};
			}
			if (value.vt == VT_BSTR && value.bstrVal)
			{
				return value.bstrVal;
			}

			CComVariant text;
			if (SUCCEEDED(VariantChangeType(&text, &value, 0, VT_BSTR)) && text.bstrVal)
			{
				return text.bstrVal;
			}
			return {};
		}

		bool TryBstrPropertyOf(FolderItem *pItem, const wchar_t *propertyName,
		                       std::wstring &result) noexcept
		{
			result.clear();
			CComQIPtr<FolderItem2> item2(pItem);
			if (!item2 || !propertyName)
			{
				return false;
			}

			CComVariant value;
			CComBSTR property(propertyName);
			if (FAILED(item2->ExtendedProperty(property, &value)) || value.vt != VT_BSTR)
			{
				return false;
			}
			if (value.bstrVal)
			{
				result.assign(value.bstrVal, SysStringLen(value.bstrVal));
			}
			return true;
		}

		std::wstring FormattedExtendedPropertyOf(FolderItem *pItem, const wchar_t *propertyName,
		                                         REFPROPERTYKEY key) noexcept
		{
			CComQIPtr<FolderItem2> item2(pItem);
			if (!item2 || !propertyName)
			{
				return {};
			}

			CComVariant value;
			CComBSTR property(propertyName);
			if (FAILED(item2->ExtendedProperty(property, &value)) || value.vt == VT_EMPTY ||
			    value.vt == VT_NULL)
			{
				return {};
			}

			PROPVARIANT propertyValue{};
			PropVariantInit(&propertyValue);
			if (FAILED(VariantToPropVariant(&value, &propertyValue)))
			{
				PropVariantClear(&propertyValue);
				return {};
			}

			PWSTR formatted = nullptr;
			const HRESULT hr = PSFormatForDisplayAlloc(key, propertyValue, PDFF_DEFAULT, &formatted);
			PropVariantClear(&propertyValue);
			if (FAILED(hr) || !formatted)
			{
				CoTaskMemFree(formatted);
				return {};
			}

			std::wstring result(formatted);
			CoTaskMemFree(formatted);
			return result;
		}

		std::wstring FolderCommentOf(IShellFolderViewDual *pView) noexcept
		{
			if (!pView)
			{
				return {};
			}

			CComPtr<Folder> folder;
			if (FAILED(pView->get_Folder(&folder)) || !folder)
			{
				return {};
			}

			CComQIPtr<Folder2> folder2(folder);
			CComPtr<FolderItem> self;
			if (!folder2 || FAILED(folder2->get_Self(&self)) || !self)
			{
				return {};
			}
			return CommentOf(self);
		}

		std::wstring FolderPathOf(IShellFolderViewDual *pView) noexcept
		{
			if (!pView)
			{
				return {};
			}

			CComPtr<Folder> folder;
			if (FAILED(pView->get_Folder(&folder)) || !folder)
			{
				return {};
			}

			CComQIPtr<Folder2> folder2(folder);
			CComPtr<FolderItem> self;
			if (!folder2 || FAILED(folder2->get_Self(&self)) || !self)
			{
				return {};
			}

			CComBSTR path;
			if (FAILED(self->get_Path(&path)) || !path)
			{
				return {};
			}
			return std::wstring(path, path.Length());
		}

		// The shell's own type string — "Local Disk", "Compact Disc". SHGFI_TYPENAME is what the
		// Type column shows, so this stays consistent with the view without depending on which
		// column index a given folder assigns to Type.
		std::wstring TypeNameOf(const std::wstring &path) noexcept
		{
			SHFILEINFOW info{};
			if (SHGetFileInfoW(path.c_str(), 0, &info, sizeof(info), SHGFI_TYPENAME))
			{
				return info.szTypeName;
			}
			return {};
		}

		// "7.98 GB" — the same rounding the shell uses, so the pane agrees with the properties
		// dialog rather than inventing its own arithmetic.
		std::wstring FormatBytes(ULONGLONG bytes) noexcept
		{
			// StrFormatByteSizeW, not the Ex form: Ex is gated behind a newer NTDDI than this
			// project targets. Only the ANSI flavour has a separate "64" name — the wide one
			// already takes a LONGLONG.
			wchar_t buffer[64]{};
			if (StrFormatByteSizeW(static_cast<LONGLONG>(bytes), buffer, ARRAYSIZE(buffer)))
			{
				return buffer;
			}
			return {};
		}

		// recycle.htt and standard.htt FormatNumber() group the exact decimal byte count with
		// commas and append a nonbreaking space plus "bytes".
		std::wstring FormatExactBytes(ULONGLONG bytes)
		{
			std::wstring digits = std::to_wstring(bytes);
			for (size_t position = digits.length(); position > 3; position -= 3)
			{
				digits.insert(position - 3, 1, L',');
			}
			return digits + ReferenceCaption(L"\u00A0bytes");
		}

		bool TryFolderItemCount(IShellFolderViewDual *pView, long &count) noexcept
		{
			count = 0;
			if (!pView)
			{
				return false;
			}

			CComPtr<Folder> folder;
			CComPtr<FolderItems> items;
			return SUCCEEDED(pView->get_Folder(&folder)) && folder &&
			       SUCCEEDED(folder->Items(&items)) && items &&
			       SUCCEEDED(items->get_Count(&count));
		}

		std::wstring FormatAttributes(const std::wstring &data) noexcept
		{
			// standard.htt FormatAttributes(): the lowercase 'a' deliberately suppresses the
			// ordinary uppercase Archive flag from the visible list.
			constexpr wchar_t codes[] = L"RHSaCE";
			const wchar_t *names[] = {
				ReferenceCaption(L"Read-only"), ReferenceCaption(L"Hidden"), ReferenceCaption(L"System"), ReferenceCaption(L"Archive"), ReferenceCaption(L"Compressed"), ReferenceCaption(L"Encrypted")
			};

			std::wstring result;
			for (size_t i = 0; i < ARRAYSIZE(codes) - 1; ++i)
			{
				if (data.find(codes[i]) == std::wstring::npos)
				{
					continue;
				}
				if (!result.empty())
				{
					result += L", ";
				}
				result += names[i];
			}
			return result.empty() ? std::wstring(ReferenceCaption(L"(normal)")) : result;
		}

bool TryFileAttributeCodes(FolderItem* item,IShellItem2* properties,std::wstring& codes) noexcept {
    ULONG flags=0;
    if(!properties || FAILED(properties->GetUInt32(PKEY_FileAttributes,&flags)))
        return TryBstrPropertyOf(item,L"Attributes",codes);
    const std::pair<DWORD,wchar_t> labels[]={{FILE_ATTRIBUTE_READONLY,L'R'},
        {FILE_ATTRIBUTE_HIDDEN,L'H'},{FILE_ATTRIBUTE_SYSTEM,L'S'},{FILE_ATTRIBUTE_ARCHIVE,L'A'},
        {FILE_ATTRIBUTE_COMPRESSED,L'C'},{FILE_ATTRIBUTE_ENCRYPTED,L'E'}};
    codes.clear();
    for(const auto& [mask,letter]:labels) if(flags&mask) codes.push_back(letter);
    return true;
}

		// `shownComment` is whatever the caller already rendered as the folder-description Message
		// box. The Comments detail below reads System.Comment, which is the very same property, so
		// without this the description prints twice -- once in the box and again as "Comments:".
		// Windows 2000 shows it once: see the My Computer reference capture, where the pane is
		// name, "System Folder", then the box and nothing further.
		void AddExtraFileDetails(std::vector<PaneLine> &lines, FolderItem *item,
		                         IShellItem2 *shellItem,
		                         const std::wstring &shownComment = {}) noexcept
		{
            const auto featureOptions=g_webOptions.load();

			VARIANT_BOOL isFolder = VARIANT_FALSE;
			VARIANT_BOOL isFileSystem = VARIANT_FALSE;
			const bool fileSystemFile = item && SUCCEEDED(item->get_IsFolder(&isFolder)) &&
			                            isFolder == VARIANT_FALSE &&
			                            SUCCEEDED(item->get_IsFileSystem(&isFileSystem)) &&
			                            isFileSystem == VARIANT_TRUE;

			// standard.htt consumes the Windows 2000 filesystem columns 4..9 in this order.
			// Windows 10 reuses those ordinals for a different schema, so resolve the historical
			// properties semantically instead of displaying Created/Accessed/Perceived type.
			if (fileSystemFile && featureOptions->attributes)
			{
				std::wstring attributeCodes;
				if (TryFileAttributeCodes(item, shellItem, attributeCodes))
				{
					PaneLine line;
					line.text = ReferenceCaption(L"Attributes: ") + FormatAttributes(attributeCodes);
					line.marginTop = panemetrics::kParagraphTop;
					lines.push_back(std::move(line));
				}
			}

			if (!featureOptions->metadata) return;
            std::wstring author = StringPropertyOf(item, L"Author");
			if (author.empty())
			{
				author = FormattedExtendedPropertyOf(item, L"System.Author", PKEY_Author);
			}
			if (author.empty())
			{
				author = FormatShellProperty(shellItem, PKEY_Author);
			}
			if (!author.empty())
			{
				PaneLine line;
				line.marginTop = panemetrics::kParagraphTop;
				if (author.find(L'@') != std::wstring::npos)
				{
					line.linkPrefix = ReferenceCaption(L"Author: ");
					line.text = std::move(author);
					line.link = true;
					line.status = L"mailto:" + line.text;
					line.linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
					line.linkTarget = line.status;
				}
				else
				{
					line.text = ReferenceCaption(L"Author: ") + author;
				}
				lines.push_back(std::move(line));
			}

			struct LegacyDetail
			{
				const wchar_t *heading;
				const wchar_t *canonicalName;
				const PROPERTYKEY *key;
			};
			const LegacyDetail details[] = {
				{ ReferenceCaption(L"Title"), L"System.Title", &PKEY_Title },
				{ ReferenceCaption(L"Subject"), L"System.Subject", &PKEY_Subject },
				{ ReferenceCaption(L"Category"), L"System.Category", &PKEY_Category },
				{ ReferenceCaption(L"Comments"), L"System.Comment", &PKEY_Comment },
			};

			for (const LegacyDetail &legacy : details)
			{
				std::wstring data = FormattedExtendedPropertyOf(item, legacy.canonicalName, *legacy.key);
				if (data.empty())
				{
					data = FormatShellProperty(shellItem, *legacy.key);
				}
				if (data.empty())
				{
					continue;
				}
				if (!shownComment.empty() && data == shownComment)
				{
					// Already on screen as the description box; see the note on shownComment.
					continue;
				}

				// standard.htt FormatDetail(): character count, followed by a literal <br>,
				// not a measured-width wrap.
				PaneLine detail;
				detail.marginTop = panemetrics::kParagraphTop;
				if (wcslen(legacy.heading) + data.length() > 32)
				{
					detail.text = std::wstring(legacy.heading) + L":";
					lines.push_back(std::move(detail));
					PaneLine continuation;
					continuation.text = data;
					lines.push_back(std::move(continuation));
				}
				else
				{
					detail.text = std::wstring(legacy.heading) + L": " + data;
					lines.push_back(std::move(detail));
				}
			}
		}

		void AddParagraph(std::vector<PaneLine> &lines, std::wstring text, bool bold = false) noexcept
		{
			if (text.empty())
			{
				return;
			}
			PaneLine line;
			line.text = std::move(text);
			line.bold = bold;
			line.marginTop = panemetrics::kParagraphTop;   // p {margin-top: 12px}
			lines.push_back(std::move(line));
		}

		void AddImgViewDetail(std::vector<PaneLine> &lines, const std::wstring &label,
		                      const std::wstring &data) noexcept
		{
            if (!g_webOptions.load()->metadata) return;

			if (label.empty() || data.empty())
			{
				return;
			}

			PaneLine detail;
			detail.marginTop = panemetrics::kImgParagraphTop;
			if (label.length() + data.length() > 32)
			{
				detail.text = label + L":";
				lines.push_back(std::move(detail));
				PaneLine continuation;
				continuation.text = data;
				lines.push_back(std::move(continuation));
			}
			else
			{
				detail.text = label + L": " + data;
				lines.push_back(std::move(detail));
			}
		}

		void AddBreak(std::vector<PaneLine> &lines, std::wstring text) noexcept
		{
			if (text.empty())
			{
				return;
			}
			PaneLine line;
			line.text = std::move(text);
			lines.push_back(std::move(line));   // <br> — no paragraph margin
		}

		void AddMessage(std::vector<PaneLine> &lines, std::wstring text) noexcept
		{
            if (!g_webOptions.load()->folderDescription) return;

			if (text.empty())
			{
				return;
			}
			PaneLine line;
			line.text = std::move(text);
			line.message = true;
			line.marginTop = panemetrics::kParagraphTop;
			lines.push_back(std::move(line));
		}

// Read a legacy customization as data; never execute an HTT template. This is
// a new parser over the documented GetPrivateProfileStringW API.
std::wstring FolderWebViewTemplate(const std::wstring& folder) noexcept {
    try {
        if(folder.empty()) return {};
        const std::wstring ini=folder+(folder.back()==L'\\' || folder.back()==L'/' ? L"" : L"\\")+L"desktop.ini";
        std::wstring value;
        for(PCWSTR key:{L"WebViewTemplate.NT5",L"PersistMoniker"}) {
            std::vector<wchar_t> buffer(512);
            for(;;) {
                const DWORD length=GetPrivateProfileStringW(L"{5984FFE0-28D4-11CF-AE66-08002B2E1262}",
                    key,L"",buffer.data(),static_cast<DWORD>(buffer.size()),ini.c_str());
                if(length<buffer.size()-1) { value.assign(buffer.data(),length); break; }
                if(buffer.size()>=32768) return {};
                buffer.resize(buffer.size()*2);
            }
            if(!value.empty()) break;
        }
        if(value.empty()) return {};
        const size_t slash=value.find_last_of(L"/\\");
        if(slash!=std::wstring::npos) value.erase(0,slash+1);
        CharLowerBuffW(value.data(),static_cast<DWORD>(value.size()));
        return value;
    } catch(...) { return {}; }
}




		// FileList.Folder.Application, which is where the templates reached the shell automation
		// object for GetSystemInformation and the Find* searches.
		CComPtr<IShellDispatch2> ShellAutomation(IDispatch *automation) noexcept
		{
			CComPtr<IShellDispatch2> shell;
			CComQIPtr<IShellFolderViewDual> view(automation);
			if (!view)
			{
				return shell;
			}

			CComPtr<IDispatch> application;
			if (FAILED(view->get_Application(&application)) || !application)
			{
				return shell;
			}

			application->QueryInterface(IID_PPV_ARGS(&shell));
			return shell;
		}

		void AddLink(std::vector<PaneLine> &lines, const wchar_t *text, int csidl,
		             const wchar_t *title) noexcept
		{
			PaneLine line;
			line.text = text;
			line.link = true;
			line.marginTop = panemetrics::kHalfParagraphTop;
			line.csidl = csidl;
			if (title)
			{
				// The coordinator put this string in the anchor's title. IE exposed the same
				// value both as a hover tooltip and through OnWebviewLinkEnter/window.status.
				line.status = title;
				line.tooltip = title;
			}
			lines.push_back(std::move(line));
		}

		void AddDeniedHttpLink(std::vector<PaneLine> &lines, const wchar_t *text,
		                       const wchar_t *url, int marginTop) noexcept
		{
			PaneLine line;
			line.text = text;
			line.link = true;
			line.marginTop = marginTop;
			line.status = url;
			line.linkAction = PaneLine::LinkAction::DeniedHttp;
			line.linkTarget = url;
			// controlp.htt's two HTTP targets are reproduced visually, but the branch security
			// policy does not permit Explorer-hosted HTTP navigation.
			line.csidl = -1;
			lines.push_back(std::move(line));
		}

// New read-only implementation over the documented Winspool structures.
struct PrinterEta {
    bool waitingTimeValid=false;
    DWORD waitingMinutes=0;
    std::wstring webUrl,oemUrl,manufacturer;
};
template<class Query> bool SpoolerBuffer(Query&& query,std::vector<BYTE>& bytes,size_t minimum) {
    DWORD needed=0;
    if(query(nullptr,0,&needed) && !needed) { bytes.clear(); return true; }
    for(int attempt=0;attempt<3;++attempt) {
        if(needed<minimum || needed>16*1024*1024) return false;
        bytes.resize(needed);
        if(query(bytes.data(),static_cast<DWORD>(bytes.size()),&needed)) return true;
        if(GetLastError()!=ERROR_INSUFFICIENT_BUFFER) return false;
    }
    return false;
}
DWORD PrinterPagesPerMinute(PCWSTR name) noexcept {
    const int direct=DeviceCapabilitiesW(name,nullptr,DC_PRINTRATEPPM,nullptr,nullptr);
    if(direct>0) return static_cast<DWORD>(direct);
    const int rate=DeviceCapabilitiesW(name,nullptr,DC_PRINTRATE,nullptr,nullptr);
    if(rate<=0) return printerjobeta::kError;
    const int unit=DeviceCapabilitiesW(name,nullptr,DC_PRINTRATEUNIT,nullptr,nullptr);
    const unsigned long long perMinute=unit==PRINTRATEUNIT_PPM ? rate :
        unit==PRINTRATEUNIT_CPS ? rate*60ULL/4800 : unit==PRINTRATEUNIT_LPM ? rate/66ULL : 0;
    return perMinute ? static_cast<DWORD>(std::min<unsigned long long>(perMinute,MAXDWORD-1ULL)) : printerjobeta::kError;
}
bool QueryPrinterEta(const std::wstring& name,PrinterEta& answer) noexcept {
    answer={};
    struct PrinterCloser { HANDLE value=nullptr; ~PrinterCloser() { if(value) ClosePrinter(value); } } printer;
    try {
        if(name.empty()) return false;
        PRINTER_DEFAULTSW access{}; access.DesiredAccess=PRINTER_ACCESS_USE;
        if(!OpenPrinterW(const_cast<PWSTR>(name.c_str()),&printer.value,&access)) return false;
        std::vector<BYTE> info,driver,queue;
        if(!SpoolerBuffer([&](BYTE* data,DWORD size,DWORD* needed) {
            return GetPrinterW(printer.value,2,data,size,needed);
        },info,sizeof(PRINTER_INFO_2W)) || info.empty()) return false;
        const auto& device=*reinterpret_cast<const PRINTER_INFO_2W*>(info.data());
        constexpr DWORD unavailable=PRINTER_STATUS_ERROR|PRINTER_STATUS_PAUSED|PRINTER_STATUS_OFFLINE|
            PRINTER_STATUS_PAPER_OUT|PRINTER_STATUS_PAPER_JAM|PRINTER_STATUS_PENDING_DELETION;
        DWORD count=0;
        const bool queueValid=SpoolerBuffer([&](BYTE* data,DWORD size,DWORD* needed) {
            return EnumJobsW(printer.value,0,65536,2,data,size,needed,&count);
        },queue,sizeof(JOB_INFO_2W));
        if(queueValid && count<=queue.size()/sizeof(JOB_INFO_2W) && !(device.Status&unavailable)) {
            std::vector<printerjobeta::Job> jobs;
            jobs.reserve(count);
            const auto* raw=reinterpret_cast<const JOB_INFO_2W*>(queue.data());
            for(DWORD i=0;i<count;++i) jobs.push_back({raw[i].Status,raw[i].TotalPages,raw[i].Size});
            const DWORD minutes=printerjobeta::WaitingMinutes(count ? PrinterPagesPerMinute(name.c_str()) : 0,jobs.data(),jobs.size());
            answer.waitingTimeValid=minutes!=printerjobeta::kError;
            if(answer.waitingTimeValid) answer.waitingMinutes=minutes;
        }
        if(SpoolerBuffer([&](BYTE* data,DWORD size,DWORD* needed) {
            return GetPrinterDriverW(printer.value,nullptr,6,data,size,needed);
        },driver,sizeof(DRIVER_INFO_6W)) && !driver.empty()) {
            const auto& details=*reinterpret_cast<const DRIVER_INFO_6W*>(driver.data());
            if(details.pszMfgName) answer.manufacturer=details.pszMfgName;
            if(details.pszOEMUrl) answer.oemUrl=details.pszOEMUrl;
        }
        if((device.Attributes&PRINTER_ATTRIBUTE_SHARED) && device.pShareName && *device.pShareName) {
            std::wstring server=device.pServerName ? device.pServerName : L"";
            server.erase(0,server.find_first_not_of(L'\\'));
            if(server.empty()) {
                wchar_t computer[MAX_COMPUTERNAME_LENGTH+1]{}; DWORD length=ARRAYSIZE(computer);
                if(GetComputerNameW(computer,&length)) server.assign(computer,length);
            }
            if(!server.empty()) {
                wchar_t share[2048]{}; DWORD length=ARRAYSIZE(share);
                if(SUCCEEDED(UrlEscapeW(device.pShareName,share,&length,URL_ESCAPE_SEGMENT_ONLY)))
                    answer.webUrl=L"http://"+server+L"/printers/"+share+L"/.printer";
            }
        }
        return true;
    } catch(...) { answer={}; return false; }
}

// Independent property reading with a valid zero size and 64-bit file sizes.
std::wstring ItemSizeText(IShellItem2* properties,FolderItem* item,IShellFolderViewDual* view) {
    SFGAOF attributes=0;
    if(properties && SUCCEEDED(properties->GetAttributes(SFGAO_FOLDER,&attributes)) &&
       (attributes&SFGAO_FOLDER)) return {};
    VARIANT_BOOL folder=VARIANT_FALSE;
    if(item && SUCCEEDED(item->get_IsFolder(&folder)) && folder!=VARIANT_FALSE) return {};
    ULONGLONG bytes=0;
    if(properties && SUCCEEDED(properties->GetUInt64(PKEY_Size,&bytes))) return FormatBytes(bytes);
    LONG legacyBytes=0;
    if(item && SUCCEEDED(item->get_Size(&legacyBytes)) && legacyBytes>=0) return FormatBytes(legacyBytes);
    return DetailOf(view,item,1);
}
std::wstring ProfileFolderComment(WebViewVisualProfile era,bool computer,bool documents,bool pictures=false) noexcept {
    if(era==WebViewVisualProfile::Windows98) return {};
    if(documents) return ReferenceCaption(L"Stores and manages documents");
    if(pictures) return ReferenceCaption(L"Stores and manages pictures.");
    if(!computer) return {};
    return era==WebViewVisualProfile::WindowsME ? ReferenceCaption(L"Displays the contents of your computer") :
        ReferenceCaption(L"Displays the files and folders on your computer");
}


		void AddPrinterSupportLink(std::vector<PaneLine> &lines, int marginTop) noexcept
		{
			std::wstring text = PrinterSupportValue(L"SupportLinkName");
			std::wstring url = PrinterSupportValue(L"SupportLink");
			if (text.empty() || url.empty())
			{
				text = ReferenceCaption(L"Windows 2000 Support");
				url = L"http://www.microsoft.com/isapi/redir.dll?prd=Win2000&ar=Support&sba=printing";
			}

			AddDeniedHttpLink(lines, text.c_str(), url.c_str(), marginTop);
		}

		void AddRecycleButton(std::vector<PaneLine> &lines, const wchar_t *text,
		                      PaneLine::LinkAction action, wchar_t accessKey,
		                      int width = 0) noexcept
		{
			PaneLine line;
			line.text = text;
			line.button = true;
			line.marginTop = panemetrics::kHalfParagraphTop;
			line.buttonWidth = width;
			line.buttonAccessKey = accessKey;
			line.linkAction = action;
			// recycle.htt supplies no title attributes. Leave status and tooltip empty.
			lines.push_back(std::move(line));
		}

		bool AppendDriveInfo(std::vector<PaneLine> &lines, const std::wstring &path,
		                     int &capacityPer1000) noexcept
		{
            if (!g_webOptions.load()->driveSpace) return false;

			ULARGE_INTEGER freeToCaller{}, total{}, rawFree{};
			if (!GetDiskFreeSpaceExW(path.c_str(), &freeToCaller, &total, &rawFree))
			{
				return false;
			}

			const ULONGLONG totalSpace = total.QuadPart;
			const ULONGLONG freeSpace = freeToCaller.QuadPart;
			const ULONGLONG usedSpace = (totalSpace >= freeSpace) ? totalSpace - freeSpace : 0ULL;

			PaneLine capacityLine;
			capacityLine.text = ReferenceCaption(L"Capacity: ") + FormatBytes(totalSpace);
			// DealWithDriveInfo emits <p><br> before Capacity.
			capacityLine.marginTop = panemetrics::kParagraphTop + panemetrics::kBodyLineHeight;
			lines.push_back(std::move(capacityLine));

			PaneLine usedLine;
			usedLine.text = ReferenceCaption(L"Used: ") + FormatBytes(usedSpace);
			usedLine.marginTop = panemetrics::kParagraphTop;
			usedLine.swatch = PaneLine::Swatch::Used;
			usedLine.tooltip = ReferenceCaption(L"Used Space");
			lines.push_back(std::move(usedLine));

			PaneLine freeLine;
			freeLine.text = ReferenceCaption(L"Free: ") + FormatBytes(freeSpace);
			freeLine.marginTop = panemetrics::kParagraphTop;
			freeLine.swatch = PaneLine::Swatch::Free;
			freeLine.tooltip = ReferenceCaption(L"Free Space");
			lines.push_back(std::move(freeLine));

			// CThumbCtl::ComputeFreeSpace uses the quota-aware free-to-caller value and
			// keeps a one-per-thousand sliver for nonempty/nonfull volumes.
			if (totalSpace > 0 && freeSpace <= totalSpace)
			{
				if (freeSpace == totalSpace)
				{
					capacityPer1000 = 0;
				}
				else if (freeSpace == 0)
				{
					capacityPer1000 = 1000;
				}
				else
				{
					capacityPer1000 = static_cast<int>(usedSpace * 1000 / totalSpace);
					if (capacityPer1000 == 0)
					{
						capacityPer1000 = 1;
					}
					else if (capacityPer1000 == 1000)
					{
						capacityPer1000 = 999;
					}
				}
			}
			return capacityPer1000 >= 0;
		}

		bool AppendWin98DriveInfo(std::vector<PaneLine> &lines, const std::wstring &path,
		                          int &capacityPer1000) noexcept
		{
            if (!g_webOptions.load()->driveSpace) return false;

			/*
			 * MYCOMP.HTT:88-93. The break grammar differs from the NT5 block above and is not
			 * interchangeable with it:
			 *
			 *   "<br><br><br>" + L_TotalSize_Text + PieChart.totalSpace
			 *   "<br><br>" + <12x12 buttonface table>   + "&nbsp;" + L_UsedSpace_Text + usedSpace
			 *   "<br><br>" + <12x12 buttonhighlight tbl> + "&nbsp;" + L_FreeSpace_Text + freeSpace
			 *
			 * so Capacity is preceded by three line breaks and each legend row by two, where the
			 * NT5 template uses paragraph margins.
			 */
			ULARGE_INTEGER freeToCaller{}, total{}, rawFree{};
			if (!GetDiskFreeSpaceExW(path.c_str(), &freeToCaller, &total, &rawFree))
			{
				return false;
			}

			const ULONGLONG totalSpace = total.QuadPart;
			const ULONGLONG freeSpace = freeToCaller.QuadPart;
			const ULONGLONG usedSpace = (totalSpace >= freeSpace) ? totalSpace - freeSpace : 0ULL;

			/*
			 * marginTop is applied after the previous line's own height, so it carries only the
			 * *extra* advance the breaks add. N consecutive <br> move the text down N line boxes
			 * in total, and one of those is the line the previous row already occupies -- so the
			 * margin is (N - 1) line boxes, not N. Counting it as N compounds down the block and
			 * pushes each row further out of place than the last.
			 */
			PaneLine capacityLine;
			capacityLine.text = ReferenceCaption(L"Capacity: ") + FormatBytes(totalSpace);
			capacityLine.marginTop = win98metrics::kBodyLineHeight * 2;   // <br><br><br>
			lines.push_back(std::move(capacityLine));

			// bgcolor=buttonface with a black border, floated left of the text by align=left.
			PaneLine usedLine;
			usedLine.text = ReferenceCaption(L"Used: ") + FormatBytes(usedSpace);
			usedLine.marginTop = win98metrics::kBodyLineHeight;           // <br><br>
			usedLine.swatch = PaneLine::Swatch::Used;
			lines.push_back(std::move(usedLine));

			// bgcolor=buttonhighlight, i.e. COLOR_3DHILIGHT, not the NT5 window colour.
			PaneLine freeLine;
			freeLine.text = ReferenceCaption(L"Free: ") + FormatBytes(freeSpace);
			freeLine.marginTop = win98metrics::kBodyLineHeight;           // <br><br>
			freeLine.swatch = PaneLine::Swatch::Free;
			lines.push_back(std::move(freeLine));

			// CThumbCtl::ComputeFreeSpace, shared with the NT5 path: the quota-aware free value,
			// and a one-per-thousand sliver kept for volumes that are neither empty nor full.
			if (totalSpace > 0 && freeSpace <= totalSpace)
			{
				if (freeSpace == totalSpace)
				{
					capacityPer1000 = 0;
				}
				else if (freeSpace == 0)
				{
					capacityPer1000 = 1000;
				}
				else
				{
					capacityPer1000 = static_cast<int>(usedSpace * 1000 / totalSpace);
					if (capacityPer1000 == 0)
					{
						capacityPer1000 = 1;
					}
					else if (capacityPer1000 == 1000)
					{
						capacityPer1000 = 999;
					}
				}
			}
			return capacityPer1000 >= 0;
		}
	} // namespace


} }

namespace ce { namespace win2kwebview {
constexpr UINT kPreviewReadyMessage=WM_APP+0x451;

struct PreviewJob {
    std::wstring path;
    SIZE size{120,120};
    bool fullResolution=false;
    unsigned long long serial=0;
};
struct PreviewResult {
    HBITMAP bitmap=nullptr;
    SIZE sourceSize{};
    HRESULT hr=E_PENDING;
    unsigned long long serial=0;
    ~PreviewResult() { if (bitmap) DeleteObject(bitmap); }
};
struct PreviewLifetime {
    std::mutex mutex;
    bool alive=true, running=false;
    HWND window=nullptr;
    unsigned long long serial=0;
    std::unique_ptr<PreviewJob> pending;
    std::unique_ptr<PreviewResult> ready;
    ~PreviewLifetime() = default;
};
struct PreviewWorkerContext {
    std::shared_ptr<PreviewLifetime> lifetime;
};
std::mutex g_referenceThreadsMutex;
std::vector<HANDLE> g_referenceThreads;

static HANDLE StartReferenceWorker(LPTHREAD_START_ROUTINE entry,void* context) noexcept {
    try {
        std::lock_guard lock(g_referenceThreadsMutex);
        std::erase_if(g_referenceThreads,[](HANDLE handle) {
            if(WaitForSingleObject(handle,0)!=WAIT_OBJECT_0) return false;
            CloseHandle(handle); return true;
        });
        // Reserve before starting: allocation failure cannot leave an untracked
        // thread executing in the module that Windhawk is about to unload.
        g_referenceThreads.reserve(g_referenceThreads.size()+1);
        HANDLE handle=CreateThread(nullptr,0,entry,context,0,nullptr);
        if(handle) g_referenceThreads.push_back(handle);
        return handle;
    } catch(...) { return nullptr; }
}
static void WaitForReferenceWorkers() {
    std::vector<HANDLE> handles;
    { std::lock_guard lock(g_referenceThreadsMutex); handles.swap(g_referenceThreads); }
    // Called on the engine thread after panes are destroyed. No UI/lifetime lock
    // is held, so a thumbnail provider can finish its Shell/COM callbacks.
    for(HANDLE handle:handles) { WaitForSingleObject(handle,INFINITE); CloseHandle(handle); }
}

static DWORD WINAPI PreviewWorker(void* parameter) {
    auto* raw=static_cast<PreviewWorkerContext*>(parameter);
    {
        std::unique_ptr<PreviewWorkerContext> context(raw);
        auto state=context->lifetime;
        const HRESULT init=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
        try {
            for (;;) {
                std::unique_ptr<PreviewJob> job;
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    if (!state->alive || !state->pending) {
                        state->running=false;
                        break;
                    }
                    job=std::move(state->pending);
                }
                auto result=std::make_unique<PreviewResult>();
                result->serial=job->serial;
                result->hr=init;
                if (SUCCEEDED(init)) {
                    CComPtr<IShellItem> item;
                    result->hr=SHCreateItemFromParsingName(job->path.c_str(),nullptr,
                                  IID_IShellItem,reinterpret_cast<void**>(&item));
                    if (SUCCEEDED(result->hr) && item) {
                        SFGAOF attributes=0;
                        result->hr=item->GetAttributes(SFGAO_FOLDER|SFGAO_ISSLOW,&attributes);
                        bool current=false;
                        {
                            std::lock_guard<std::mutex> lock(state->mutex);
                            current=state->alive && state->serial==job->serial;
                        }
                        if (!current || (attributes&(SFGAO_FOLDER|SFGAO_ISSLOW))) {
                            result->hr=HRESULT_FROM_WIN32(ERROR_CANCELLED);
                        } else if (SUCCEEDED(result->hr)) {
                            if (job->fullResolution) {
                                result->hr=DecodeFullResolutionBitmap(job->path,result->bitmap,
                                                                      result->sourceSize);
                            } else {
                                CComQIPtr<IShellItemImageFactory> factory(item);
                                result->hr=factory ? factory->GetImage(job->size,
                                    SIIGBF_THUMBNAILONLY,&result->bitmap) : E_NOINTERFACE;
                                CComQIPtr<IShellItem2> properties(item);
                                ULONG width=0,height=0;
                                if (properties && SUCCEEDED(properties->GetUInt32(PKEY_Image_HorizontalSize,&width)) &&
                                    SUCCEEDED(properties->GetUInt32(PKEY_Image_VerticalSize,&height)) &&
                                    width<=LONG_MAX && height<=LONG_MAX) {
                                    result->sourceSize={static_cast<LONG>(width),static_cast<LONG>(height)};
                                }
                            }
                        }
                    }
                }
                if (SUCCEEDED(result->hr) && !result->bitmap) result->hr=E_FAIL;
                // Publish under the lifetime lock. No COM pointer, Pane pointer,
                // or heap-owned payload is ever posted to a possibly reused HWND.
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    if (state->alive && state->serial==job->serial) {
                        state->ready=std::move(result);
                        PostMessageW(state->window,kPreviewReadyMessage,0,0);
                    }
                }
            }
        } catch (...) {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->running=false;
            state->pending.reset();
            if (state->alive) PostMessageW(state->window,kPreviewReadyMessage,1,0);
        }
        if (SUCCEEDED(init)) CoUninitialize();
    }
    return 0;
}

static bool QueuePreview(const std::shared_ptr<PreviewLifetime>& state,
                         const std::wstring& path,SIZE size,bool fullResolution) {
    std::lock_guard<std::mutex> lock(state->mutex);
    if (!state->alive) return false;
    ++state->serial;
    state->ready.reset();
    auto job=std::make_unique<PreviewJob>();
    job->path=path; job->size=size; job->fullResolution=fullResolution;
    job->serial=state->serial;
    state->pending=std::move(job);
    if (state->running) return true;
    auto context=std::make_unique<PreviewWorkerContext>();
    context->lifetime=state;
    state->running=true;
    HANDLE thread=StartReferenceWorker(PreviewWorker,context.get());
    if (!thread) {
        state->running=false; state->pending.reset();
        return false;
    }
    context.release();
    return true;
}
static void CancelPreview(const std::shared_ptr<PreviewLifetime>& state,bool destroying=false) {
    std::lock_guard<std::mutex> lock(state->mutex);
    ++state->serial;
    state->pending.reset(); state->ready.reset();
    if (destroying) { state->alive=false; state->window=nullptr; }
}
} }

namespace ce { namespace win2kwebview {
constexpr UINT kPrinterReadyMessage=WM_APP+0x452;
using PrinterQuery=bool(*)(const std::wstring&,PrinterEta&);
struct PrinterJob { std::wstring name; ULONGLONG serial=0; PrinterQuery query=QueryPrinterEta; };
struct PrinterLifetime {
    std::mutex mutex;
    bool alive=true,running=false,hasResult=false,success=false;
    HWND window=nullptr;
    ULONGLONG serial=0,readySerial=0,requestedAt=0;
    std::wstring name;
    PrinterEta value;
    std::unique_ptr<PrinterJob> pending;
};
static DWORD WINAPI PrinterWorker(void* parameter) {
    std::unique_ptr<std::shared_ptr<PrinterLifetime>> context(static_cast<std::shared_ptr<PrinterLifetime>*>(parameter));
    const auto state=*context;
    try {
        for(;;) {
            std::unique_ptr<PrinterJob> job;
            { std::lock_guard lock(state->mutex);
              if(!state->alive || !state->pending) { state->running=false; break; }
              job=std::move(state->pending); }
            PrinterEta value;
            const bool success=job->query(job->name,value);
            { std::lock_guard lock(state->mutex);
              if(state->alive && job->serial==state->serial) {
                  state->value=std::move(value); state->hasResult=true; state->success=success;
                  state->readySerial=job->serial;
                  state->requestedAt=GetTickCount64();
                  PostMessageW(state->window,kPrinterReadyMessage,0,0);
              } }
        }
    } catch(...) {
        std::lock_guard lock(state->mutex);
        state->running=false; state->pending.reset(); state->hasResult=false;
    }
    return 0;
}
static void CancelPrinterInfo(const std::shared_ptr<PrinterLifetime>& state,bool destroying=false) {
    std::lock_guard lock(state->mutex);
    ++state->serial; state->pending.reset(); state->hasResult=false; state->value={};
    state->requestedAt=0; state->name.clear();
    if(destroying) { state->alive=false; state->window=nullptr; }
}
static bool ReadCachedPrinter(const std::shared_ptr<PrinterLifetime>& state,HWND window,
    const std::wstring& name,bool refresh,PrinterEta& result,PrinterQuery query=QueryPrinterEta) {
    std::lock_guard lock(state->mutex);
    if(!state->alive || name.empty()) return false;
    state->window=window;
    const auto now=GetTickCount64();
    if(state->name!=name || !state->requestedAt || (refresh && now-state->requestedAt>=5000 && !state->running)) {
        auto job=std::make_unique<PrinterJob>(); job->name=name; job->serial=++state->serial; job->query=query;
        if(state->name!=name) { state->hasResult=false; state->value={}; }
        state->name=name; state->requestedAt=now; state->pending=std::move(job);
        if(!state->running) {
            auto context=std::make_unique<std::shared_ptr<PrinterLifetime>>(state);
            if(StartReferenceWorker(PrinterWorker,context.get())) { context.release(); state->running=true; }
            else { state->pending.reset(); state->requestedAt=0; }
        }
    }
    if(!state->hasResult || !state->success) return false;
    result=state->value; return true;
}
} }


namespace ce { namespace win2kwebview {
class ReferenceContent {
public:
    WebViewNativePane m_pane;
    CComPtr<IShellView> m_spView;
    CComPtr<IShellItemArray> m_selection;
    WebViewVisualProfile m_visualProfile=WebViewVisualProfile::Windows2000;
    Win98TemplateKind m_win98Template=Win98TemplateKind::Folder;
    NavigationGeneration m_generation=1;
    bool m_directoryService=false;
    bool m_isControlPanel=false, m_isEntireNetwork=false, m_isMeDriveRoot=false;
    bool m_isMyComputer=false, m_isMyDocuments=false, m_isMyNetworkPlaces=false;
    bool m_isNetworkConnections=false, m_isPrinters=false, m_isRecycleBin=false;
    bool m_isMyPictures=false, m_isImgViewTemplate=false, m_isDesktop=false;
    bool m_isWin98SystemFolder=false, m_isWin98ProgramFiles=false;
    bool m_showFiles=true;
    std::wstring folderKey;
    HWND window=nullptr, toolbar=nullptr;
    std::function<HRESULT(PCIDLIST_ABSOLUTE)> navigate;
    std::function<bool(IShellView*)> isActiveView;
    HIMAGELIST images=nullptr,hotImages=nullptr;
    ImgViewDetailsWindow details;
    ImgViewZoomWindow zoom;
    ImgViewDetachedPreview detached;
    std::shared_ptr<PreviewLifetime> lifetime=std::make_shared<PreviewLifetime>();
    std::shared_ptr<PrinterLifetime> printerLifetime=std::make_shared<PrinterLifetime>();
    bool CachedPrinterEta(const std::wstring& name,PrinterEta& result) {
        return ReadCachedPrinter(printerLifetime,window,name,g_webOptions.load()->printerRefresh,result);
    }
    std::unique_ptr<PreviewResult> preview;
    std::wstring previewPath, signature;
    bool fullResolution=false, printable=false;
    bool forceRefresh=true, viewReady=false;
    DWORD selectedCount=0;
    ULONGLONG refreshedAt=0,pendingSince=0;
    ImgPreviewState previewState=ImgPreviewState::NoSelection;
    int sideWidth=200,bannerHeight=0;
    ~ReferenceContent();
    void Refresh(IShellView*,IShellItem*,HWND,bool force=false);
    void AcceptPreview();
    void ClearPreview();
    void ResetView();
    void SuspendView();
    void PreviewStatus(ImgPreviewState state,SIZE size={});
    void EnsureViewer();
    void UpdateViewerButtons();
    void DestroyViewer();
    void LayoutViewer(const RECT& client,int dpi,bool visible);
    void Command(UINT command,HWND source=nullptr);
    void Activate(int index);
    bool SelectionMatches() const;
    HRESULT CanonicalVerb(UINT items,PCWSTR verb,bool invoke,HWND owner=nullptr);
    void FilterLines(std::vector<PaneLine>& lines);
    ULONGLONG SelectionBytesAt(DWORD index,FolderItem* fallback) const {
        CComPtr<IShellItem> item;
        if (m_selection && SUCCEEDED(m_selection->GetItemAt(index,&item))) {
            CComQIPtr<IShellItem2> properties(item); ULONGLONG bytes=0;
            if (properties && SUCCEEDED(properties->GetUInt64(PKEY_Size,&bytes))) return bytes;
        }
        LONG bytes=0; return fallback && SUCCEEDED(fallback->get_Size(&bytes)) && bytes>0 ? bytes : 0;
    }
    HRESULT GetAutomation(IDispatch** result) noexcept {
        *result=nullptr;
        return m_spView ? m_spView->GetItemObject(SVGIO_BACKGROUND,IID_IDispatch,
                                                 reinterpret_cast<void**>(result)) : E_FAIL;
    }
    bool UsesImgViewProfile() const noexcept {
        const auto options=g_webOptions.load();
        return options->preview && options->imageViewer && options->profile!=1 && !m_isDesktop &&
               (m_isMyPictures||m_isImgViewTemplate);
    }
    void SetImgViewPreview(ImgPreviewState state,SIZE size={}) noexcept { PreviewStatus(state,size); }
    BarricadeMode CurrentBarricade() const noexcept {
        return !m_showFiles && (m_isEntireNetwork||m_isMeDriveRoot||m_isWin98SystemFolder)
               ? BarricadeMode::Beside : BarricadeMode::None;
    }
    bool HasSelectedRealPrinter() noexcept;
    std::vector<PaneLine> BuildPaneLines(std::wstring& thumbnailPath) noexcept;
    std::vector<PaneLine> BuildWin98MyComputer(IShellFolderViewDual*,FolderItems*,long) noexcept;
    std::vector<PaneLine> BuildWin98ControlPanel(IShellFolderViewDual*,FolderItems*,long) noexcept;
    std::vector<PaneLine> BuildWin98Printers(IShellFolderViewDual*,FolderItems*,long) noexcept;
    std::vector<PaneLine> BuildWin98DialUp(IShellFolderViewDual*,FolderItems*,long) noexcept;
    std::vector<PaneLine> BuildWin98NetHood(IShellFolderViewDual*,FolderItems*,long) noexcept;
    std::vector<PaneLine> BuildWin98RecycleBin(IShellFolderViewDual*,FolderItems*,long) noexcept;
    std::vector<PaneLine> BuildWin98SystemFolder(IShellFolderViewDual*,FolderItems*,long) noexcept;
    static void AddWin98Line(std::vector<PaneLine>&,std::wstring,int) noexcept;
    static bool AddWin98CountAndNames(std::vector<PaneLine>&,FolderItems*,long) noexcept;
};
} }

namespace ce { namespace win2kwebview {
	bool ReferenceContent::HasSelectedRealPrinter() noexcept
	{
		if (!m_isPrinters)
		{
			return false;
		}

		CComPtr<IDispatch> automation;
		if (FAILED(GetAutomation(&automation)) || !automation)
		{
			return false;
		}
		CComQIPtr<IShellFolderViewDual> view(automation);
		CComPtr<FolderItems> items;
		long selected = 0;
		if (!view || FAILED(view->SelectedItems(&items)) || !items ||
		    FAILED(items->get_Count(&selected)) || selected != 1)
		{
			return false;
		}

		CComPtr<FolderItem> item;
		CComVariant index(0L);
		if (FAILED(items->Item(index, &item)) || !item)
		{
			return false;
		}

		const std::wstring printerName = DetailOf(view, item, 0);
		return !printerName.empty() && printerName != kAddPrinterSentinel;
	}

	void ReferenceContent::AddWin98Line(std::vector<PaneLine> &lines, std::wstring text,
	                                       int breaksBefore) noexcept
	{
		// N consecutive <br> advance N line boxes in total, one of which the previous row already
		// occupies, so the extra margin is (N - 1). See PHASE1-GEOMETRY.md.
		PaneLine line;
		line.text = std::move(text);
		line.marginTop = (breaksBefore > 1)
			? win98metrics::kBodyLineHeight * (breaksBefore - 1) : 0;
		lines.push_back(std::move(line));
	}

	bool ReferenceContent::AddWin98CountAndNames(std::vector<PaneLine> &lines,
	                                                FolderItems *items, long selected) noexcept
	{
		/*
		 * The shape every Windows 98 template shares for a multiple selection:
		 *
		 *     text = data + L_Multiple_Text + "<br>";
		 *     if (data <= 16)
		 *         for (...) text += "<br>" + Item(i).Name;
		 *
		 * The trailing "<br>" on the count line and the leading "<br>" on each name together put
		 * one blank line between the count and the first name, and none between names.
		 */
		wchar_t buffer[64]{};
		StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
		AddWin98Line(lines, buffer, 0);

		if (selected > 16)
		{
			return false;
		}

		bool firstName = true;
		for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
		{
			CComPtr<FolderItem> item;
			CComVariant index(itemIndex);
			if (FAILED(items->Item(index, &item)) || !item)
			{
				continue;
			}
			CComBSTR name;
			if (FAILED(item->get_Name(&name)) || !name)
			{
				continue;
			}
			AddWin98Line(lines, std::wstring(name, name.Length()), firstName ? 2 : 1);
			firstName = false;
		}
		return true;
	}

	std::vector<PaneLine> ReferenceContent::BuildWin98ControlPanel(
		IShellFolderViewDual *view, FolderItems *items, long selected) noexcept
	{
		// CONTROLP.HTT:44-64 for the selection body, and its <body> for the two links below.
		// No details beyond two columns, and no Windows Update or curated-task list -- those are
		// Windows 2000 and Me additions.
		std::vector<PaneLine> lines;
		m_pane.SetCapacityPie(-1);

		if (selected == 0)
		{
			AddWin98Line(lines,
			             ReferenceCaption(L"Use the settings in Control Panel to personalize your computer."), 0);
			AddWin98Line(lines, ReferenceCaption(L"Select an item to view its description."), 2);
		}
		else if (selected > 1)
		{
			AddWin98CountAndNames(lines, items, selected);
		}
		else
		{
			CComPtr<FolderItem> item;
			CComVariant index(0L);
			if (SUCCEEDED(items->Item(index, &item)) && item)
			{
				// "<b>" + GetDetailsOf(items, 0) + "</b><br>" + GetDetailsOf(items, 1)
				std::wstring name = DetailOf(view, item, 0);
				if (name.empty())
				{
					CComBSTR fallback;
					if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
					{
						name.assign(fallback, fallback.Length());
					}
				}
				AddWin98Line(lines, std::move(name), 0);
				if (!lines.empty())
				{
					lines.back().bold = true;
				}

				// Windows 98's Control Panel column 1 was the applet description. Windows 10's
				// Control Panel publishes only two columns -- Name and Category -- and Category is
				// a numeric list ("2; 5"), so reading ordinal 1 puts that in the description slot.
				// CommentOf is the same accessor the Windows 2000/Me path already uses for this.
				const std::wstring comment = CommentOf(item);
				if (!comment.empty())
				{
					AddWin98Line(lines, comment, 1);
				}
			}
		}

		/*
		 * The two links live in the template's <body>, after the #Info span rather than inside
		 * it, so the SelectionChanged handler never touches them and they are present in every
		 * state -- no selection, one, or many:
		 *
		 *     <p>
		 *     <br>
		 *     <a href="http://www.microsoft.com/">Microsoft Home</a>
		 *     <p class=Links>
		 *     <a href="http://support.microsoft.com/support">Technical Support</a>
		 *
		 * The first sits in an ordinary paragraph (15px top margin) after an explicit <br>, so it
		 * begins one body line into that paragraph. The second is p.Links {margin-top: 5px}.
		 *
		 * Both are external. They are drawn and hit-tested exactly as the source declares, but
		 * activation goes through the branch's external-navigation policy rather than launching a
		 * browser, so DeniedHttp carries the real target for the status bar, the tooltip and the
		 * evidence log.
		 */
		// Neither carries class=Command, so both are underlined but not bold -- unlike the
		// command anchors in recycle.htt and dialup.htt.
		AddDeniedHttpLink(lines, ReferenceCaption(L"Microsoft Home"), L"http://www.microsoft.com/",
		                  win98metrics::kParagraphTop + win98metrics::kBodyLineHeight);
		AddDeniedHttpLink(lines, ReferenceCaption(L"Technical Support"), L"http://support.microsoft.com/support",
		                  win98metrics::kLinksTop);
		return lines;
	}

	std::vector<PaneLine> ReferenceContent::BuildWin98Printers(
		IShellFolderViewDual *view, FolderItems *items, long selected) noexcept
	{
		// PRINTERS.HTT:50-90.
		std::vector<PaneLine> lines;
		m_pane.SetCapacityPie(-1);

		if (selected == 0)
		{
			AddWin98Line(lines,
			             ReferenceCaption(L"This folder contains information about your current printers and a wizard to help you install new ones."), 0);
			AddWin98Line(lines,
			             ReferenceCaption(L"To get information about a printer that's already installed, right-click the printer's icon."), 2);

			// "To install a new printer, click <b>Add Printer</b> to start the Add Printer
			// wizard." -- an inline bold run, not a link: the template emits <b>, not <a>.
			PaneLine add;
			add.boldPrefix = ReferenceCaption(L"To install a new printer, click ");
			add.text = ReferenceCaption(L"Add Printer");
			add.boldSuffix = ReferenceCaption(L" to start the Add Printer wizard.");
			add.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(add));

			AddWin98Line(lines, ReferenceCaption(L"Select an item to view its description."), 2);
			return lines;
		}

		if (selected > 1)
		{
			AddWin98CountAndNames(lines, items, selected);
			return lines;
		}

		CComPtr<FolderItem> item;
		CComVariant index(0L);
		if (FAILED(items->Item(index, &item)) || !item)
		{
			return lines;
		}

		std::wstring name;
		{
			CComBSTR value;
			if (SUCCEEDED(item->get_Name(&value)) && value)
			{
				name.assign(value, value.Length());
			}
		}
		AddWin98Line(lines, name, 0);
		if (!lines.empty())
		{
			lines.back().bold = true;
		}

		// Windows 10's Printers folder happens to publish the same first four columns Windows 98
		// did -- 1 Documents, 2 Status, 3 Comments -- but the ordinals are resolved by heading
		// rather than relying on that, since nothing guarantees it across shell versions.
		const int docsColumn    = FindDetailColumn(view, L"Documents");
		const int statusColumn  = FindDetailColumn(view, ReferenceCaption(L"Status"));
		const int commentColumn = FindDetailColumn(view, ReferenceCaption(L"Comments"), ReferenceCaption(L"Comment"));

		// comment (column 3), then "Documents" heading+value (column 1) on one line
		const std::wstring comment = DetailOf(view, item, commentColumn > 0 ? commentColumn : 3);
		if (!comment.empty())
		{
			AddWin98Line(lines, comment, 1);
		}

		const int docsOrdinal = docsColumn > 0 ? docsColumn : 1;
		const std::wstring docs = DetailOf(view, item, docsOrdinal);
		if (!docs.empty())
		{
			std::wstring heading = DetailOf(view, nullptr, docsOrdinal);
			if (heading.empty())
			{
				heading = L"Documents";
			}
			AddWin98Line(lines, heading + L": " + docs, 2);
		}

		// status (column 2) as bold red -- <b><font color=red>
		const std::wstring status = DetailOf(view, item, statusColumn > 0 ? statusColumn : 2);
		if (!status.empty())
		{
			PaneLine line;
			line.text = status;
			line.bold = true;
			line.boldColor = RGB(255, 0, 0);
			line.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(line));
		}

		const std::wstring tip = DetailOf(view, item, -1);
		if (!tip.empty() && tip != name)
		{
			AddWin98Line(lines, tip, 2);
		}

		// The Add Printer wizard item identifies itself by column 0, not by its display name.
		if (DetailOf(view, item, 0) == L"WinUtils_NewObject")
		{
			AddWin98Line(lines,
			             ReferenceCaption(L"The Add Printer wizard walks you step-by-step through installing a printer. Just follow the instructions on each screen."), 2);
		}
		return lines;
	}

	std::vector<PaneLine> ReferenceContent::BuildWin98DialUp(
		IShellFolderViewDual *view, FolderItems *items, long selected) noexcept
	{
		// dialup.htt:56-101. Windows 98's Dial-Up Networking, mapped onto the modern network
		// connections namespace -- the only live equivalent.
		std::vector<PaneLine> lines;
		m_pane.SetCapacityPie(-1);

		if (selected == 0)
		{
			AddWin98Line(lines,
			             ReferenceCaption(L"This folder contains information about your dial-up networking connections, and a wizard to help you make a new connection."), 0);
			AddWin98Line(lines,
			             ReferenceCaption(L"To get information about a connection, right-click the connection's icon."), 2);

			PaneLine make;
			make.boldPrefix = ReferenceCaption(L"To make a new connection, click ");
			make.text = ReferenceCaption(L"Make New Connection");
			make.boldSuffix = ReferenceCaption(L" to start the wizard.");
			make.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(make));

			AddWin98Line(lines, ReferenceCaption(L"Select an item to view its description."), 2);
			return lines;
		}

		if (selected > 1)
		{
			AddWin98CountAndNames(lines, items, selected);
			return lines;
		}

		CComPtr<FolderItem> item;
		CComVariant index(0L);
		if (FAILED(items->Item(index, &item)) || !item)
		{
			return lines;
		}

		std::wstring name = DetailOf(view, item, 0);
		if (name.empty())
		{
			CComBSTR fallback;
			if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
			{
				name.assign(fallback, fallback.Length());
			}
		}
		if (!name.empty())
		{
			AddWin98Line(lines, name, 0);
			lines.back().bold = true;
		}

		// Columns 1..4, each as "heading:<br>value", stopping at the first empty heading.
		for (int column = 1; column < 5; ++column)
		{
			const std::wstring heading = DetailOf(view, nullptr, column);
			if (heading.empty())
			{
				break;
			}
			const std::wstring value = DetailOf(view, item, column);
			if (!value.empty())
			{
				AddWin98Line(lines, heading + L":", 2);
				AddWin98Line(lines, value, 1);
			}
		}

		const std::wstring tip = DetailOf(view, item, -1);
		if (!tip.empty() && tip != name)
		{
			AddWin98Line(lines, tip, 2);
		}

		if (name == ReferenceCaption(L"Make New Connection"))
		{
			AddWin98Line(lines,
			             ReferenceCaption(L"The Make New Connection wizard walks you step-by-step through adding Dial-Up Networking connections. Just follow the instructions on each screen."), 2);
		}
		else
		{
			// L_ConnectNow_Text: an inline a.Command anchor inside the sentence, so only the
			// word "Connect" is clickable. The verb itself is resolved and validated at click
			// time rather than being an arbitrary InvokeVerb string.
			PaneLine connect;
			connect.linkPrefix = ReferenceCaption(L"To establish a dial-up connection to this network now, click ");
			connect.text = ReferenceCaption(L"Connect");
			connect.linkSuffix = L".";
			connect.link = true;
			connect.bold = true;
			connect.linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
			connect.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(connect));
		}
		return lines;
	}

	std::vector<PaneLine> ReferenceContent::BuildWin98NetHood(
		IShellFolderViewDual *view, FolderItems *items, long selected) noexcept
	{
		// nethood.htt:57-110. Windows 98's Network Neighborhood has no Entire Network barricade;
		// that is an NT5 addition and is explicitly not reproduced here.
		std::vector<PaneLine> lines;
		m_pane.SetCapacityPie(-1);

		if (selected == 0)
		{
			AddWin98Line(lines,
			             ReferenceCaption(L"This folder contains links to all the computers in your workgroup and on the entire network."), 0);
			AddWin98Line(lines,
			             ReferenceCaption(L"To see the shared resources available on a specific computer, just click the computer icon."), 2);
			AddWin98Line(lines,
			             ReferenceCaption(L"To install a network printer from this folder, locate the printer in Network Neighborhood, right-click its icon, and then click Install."), 2);
			AddWin98Line(lines, ReferenceCaption(L"Select an item to view its description."), 2);
			return lines;
		}

		if (selected > 1)
		{
			AddWin98CountAndNames(lines, items, selected);
			return lines;
		}

		CComPtr<FolderItem> item;
		CComVariant index(0L);
		if (FAILED(items->Item(index, &item)) || !item)
		{
			return lines;
		}

		const std::wstring name = DetailOf(view, item, 0);
		AddWin98Line(lines, name, 0);
		if (!lines.empty())
		{
			lines.back().bold = true;
			// A UNC name is emitted as name.link(name) -- the share path is its own target.
			if (name.rfind(L"\\\\", 0) == 0)
			{
				lines.back().link = true;
				lines.back().linkTarget = name;
				lines.back().linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
			}
		}

		/*
		 * nethood.htt takes the comment from column 1. On Windows 98 that column was the share's
		 * Comment; Windows 10's Network folder publishes
		 *
		 *     0 Name | 1 Category | 2 Workgroup | 3 Network location | 4 Discovery Method ...
		 *
		 * so ordinal 1 yields a device category ("Network Infrastructure") rather than a comment.
		 * The heading is looked up instead, then the item's own comment property, and if neither
		 * exists the line is simply omitted -- better than captioning a device with its category
		 * as though the template had asked for it.
		 */
		const int commentColumn = FindDetailColumn(view, ReferenceCaption(L"Comments"), ReferenceCaption(L"Comment"));
		std::wstring comment = (commentColumn > 0) ? DetailOf(view, item, commentColumn)
		                                           : std::wstring();
		if (comment.empty())
		{
			comment = CommentOf(item);
		}
		if (!comment.empty())
		{
			AddWin98Line(lines, comment, 1);
		}

		const std::wstring tip = DetailOf(view, item, -1);
		if (!tip.empty() && tip != name)
		{
			AddWin98Line(lines, tip, 2);
		}
		return lines;
	}

	std::vector<PaneLine> ReferenceContent::BuildWin98RecycleBin(
		IShellFolderViewDual *view, FolderItems *items, long selected) noexcept
	{
		// recycle.htt:60-160. Windows 98 uses bold a.Command anchors here, not the NT5 push
		// buttons -- see the stylesheet's `a.Command {font-weight: bold}`.
		std::vector<PaneLine> lines;
		m_pane.SetCapacityPie(-1);

		auto addCommand = [&](PCWSTR prefix, PCWSTR label, PCWSTR suffix,
		                      PaneLine::LinkAction action, int breaksBefore) noexcept
		{
			PaneLine line;
			line.linkPrefix = prefix;
			line.text = label;
			line.linkSuffix = suffix;
			line.link = true;
			line.bold = true;
			line.linkAction = action;
			line.marginTop = (breaksBefore > 1)
				? win98metrics::kBodyLineHeight * (breaksBefore - 1) : 0;
			lines.push_back(std::move(line));
		};

		if (selected == 0)
		{
			AddWin98Line(lines,
			             ReferenceCaption(L"This folder contains files and folders that you have deleted from your computer."), 0);
			addCommand(ReferenceCaption(L"To permanently remove all items and reclaim disk space, click "),
			           ReferenceCaption(L"Empty Recycle Bin"), L".",
			           PaneLine::LinkAction::RecycleEmpty, 2);
			addCommand(ReferenceCaption(L"To move all items back to their original locations, click "),
			           ReferenceCaption(L"Restore All"), L".",
			           PaneLine::LinkAction::RecycleRestoreAll, 2);
			AddWin98Line(lines, ReferenceCaption(L"Select an item to view its description."), 2);
			return lines;
		}

		if (selected > 1)
		{
			wchar_t buffer[64]{};
			StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
			AddWin98Line(lines, buffer, 0);

			bool listedNames = false;
			if (selected <= 100)
			{
				LONGLONG total = 0;
				for (long i = 0; i < selected; ++i)
				{
					CComPtr<FolderItem> entry;
					CComVariant index(i);
					LONG entrySize = 0;
					if (SUCCEEDED(items->Item(index, &entry)) && entry &&
					    SUCCEEDED(entry->get_Size(&entrySize)) && entrySize > 0)
					{
						total += entrySize;
					}
				}
				if (total > 0)
				{
					AddWin98Line(lines, ReferenceCaption(L"Total Size: ") +
					                    FormatExactBytes(static_cast<ULONGLONG>(total)), 2);
				}
				if (selected <= 16)
				{
					bool firstName = true;
					for (long i = 0; i < selected; ++i)
					{
						CComPtr<FolderItem> entry;
						CComVariant index(i);
						if (FAILED(items->Item(index, &entry)) || !entry)
						{
							continue;
						}
						CComBSTR entryName;
						if (FAILED(entry->get_Name(&entryName)) || !entryName)
						{
							continue;
						}
						AddWin98Line(lines, std::wstring(entryName, entryName.Length()),
						             firstName ? 2 : 1);
						firstName = false;
					}
					listedNames = true;
				}
			}

			addCommand(L"", ReferenceCaption(L"Restore"),
			           ReferenceCaption(L" these items back to their original locations."),
			           PaneLine::LinkAction::RecycleRestoreSelection, listedNames ? 3 : 2);
			return lines;
		}

		CComPtr<FolderItem> item;
		CComVariant index(0L);
		if (FAILED(items->Item(index, &item)) || !item)
		{
			return lines;
		}

		const std::wstring name = DetailOf(view, item, 0);
		AddWin98Line(lines, name, 0);
		if (!lines.empty())
		{
			lines.back().bold = true;
		}

		/*
		 * recycle.htt reads type from column 3, original location from 1, date deleted from 2 and
		 * size from 4. Windows 10's Recycle Bin publishes
		 *
		 *     0 Name | 1 Original Location | 2 Date Deleted | 3 Size | 4 Type | 5 Modified ...
		 *
		 * so Size and Type are *swapped* relative to Windows 98. Reading the raw ordinals prints
		 * the byte count where the type belongs and the type under the "Size" heading. The
		 * columns are therefore resolved by heading, keeping the Windows 98 ordinal as the
		 * fallback for a shell that does not publish one.
		 */
		const int typeColumn    = FindDetailColumn(view, ReferenceCaption(L"Type"), ReferenceCaption(L"Item type"));
		const int originColumn  = FindDetailColumn(view, ReferenceCaption(L"Original Location"));
		const int deletedColumn = FindDetailColumn(view, ReferenceCaption(L"Date Deleted"));
		const int binSizeColumn = FindDetailColumn(view, ReferenceCaption(L"Size"));

		// type (3), then original location (1) as a link, then date deleted (2)
		const std::wstring type = DetailOf(view, item, typeColumn > 0 ? typeColumn : 3);
		if (!type.empty())
		{
			AddWin98Line(lines, type, 1);
		}

		const int originOrdinal = originColumn > 0 ? originColumn : 1;
		const std::wstring origin = DetailOf(view, item, originOrdinal);
		if (!origin.empty())
		{
			std::wstring heading = DetailOf(view, nullptr, originOrdinal);
			if (heading.empty())
			{
				heading = ReferenceCaption(L"Original Location");
			}
			AddWin98Line(lines, heading + L":", 2);
			AddWin98Line(lines, origin, 1);
			lines.back().link = true;
			lines.back().linkTarget = origin;
			lines.back().linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
		}

		const int deletedOrdinal = deletedColumn > 0 ? deletedColumn : 2;
		const std::wstring deleted = DetailOf(view, item, deletedOrdinal);
		if (!deleted.empty())
		{
			std::wstring heading = DetailOf(view, nullptr, deletedOrdinal);
			if (heading.empty())
			{
				heading = ReferenceCaption(L"Date Deleted");
			}
			AddWin98Line(lines, heading + L":", 2);
			AddWin98Line(lines, deleted, 1);
		}

		LONG rawSize = 0;
		item->get_Size(&rawSize);
		if (rawSize > 0)
		{
			if (rawSize < 1000)
			{
				AddWin98Line(lines,
				             ReferenceCaption(L"Size: ") + std::to_wstring(rawSize) + ReferenceCaption(L"\u00A0bytes"), 2);
			}
			else
			{
				// recycle.htt reads its size from column 4, not column 1 as folder.htt does --
				// resolved by heading above, since Windows 10 puts Size at 3.
				const int sizeOrdinal = binSizeColumn > 0 ? binSizeColumn : 4;
				const std::wstring sizeValue = rawSize>0 ? FormatBytes(rawSize) : std::wstring();
				if (!sizeValue.empty())
				{
					std::wstring heading = DetailOf(view, nullptr, sizeOrdinal);
					if (heading.empty())
					{
						heading = ReferenceCaption(L"Size");
					}
					AddWin98Line(lines, heading + L": " + sizeValue, 2);
				}
				else
				{
					AddWin98Line(lines, ReferenceCaption(L"Size: ") +
					                    FormatExactBytes(static_cast<ULONGLONG>(rawSize)), 2);
				}
			}
		}

		const std::wstring tip = DetailOf(view, item, -1);
		if (!tip.empty() && tip != name)
		{
			AddWin98Line(lines, tip, 2);
		}

		addCommand(L"", ReferenceCaption(L"Restore"), ReferenceCaption(L" this item back to its original location."),
		           PaneLine::LinkAction::RecycleRestoreSelection, 2);
		return lines;
	}

	std::vector<PaneLine> ReferenceContent::BuildWin98MyComputer(
		IShellFolderViewDual *view, FolderItems *items, long selected) noexcept
	{
		/*
		 * Web\MYCOMP.HTT:55-95.
		 *
		 * This is not the ordinary folder template with a pie appended. It has no Modified, no
		 * Size, no extra detail columns, no thumbnail and no total-size arithmetic for multiple
		 * selections. It is name, type, tip, and -- only for something PieChart.displayFile()
		 * accepts -- the capacity block.
		 */
		std::vector<PaneLine> lines;
		int capacityPer1000 = -1;

		// PieChart.style.display = "none" runs first, before any selection test, so every path
		// that does not reach displayFile() leaves the graph hidden.
		m_pane.SetCapacityPie(-1);

		if (selected == 0)
		{
			// Info.innerHTML = L_Prompt_Text
			AddBreak(lines, ReferenceCaption(L"Select an item to view its description."));
			return lines;
		}

		if (selected > 1)
		{
			// text = data + L_Multiple_Text + "<br>", then names for <= 16. Note the absence of
			// the ordinary template's Total File Size block -- My Computer never computes it.
			wchar_t buffer[64]{};
			StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
			AddBreak(lines, buffer);

			if (selected <= 16)
			{
				bool firstName = true;
				for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
				{
					CComPtr<FolderItem> item;
					CComVariant index(itemIndex);
					if (FAILED(items->Item(index, &item)) || !item)
					{
						continue;
					}
					CComBSTR name;
					if (FAILED(item->get_Name(&name)) || !name)
					{
						continue;
					}
					PaneLine nameLine;
					nameLine.text.assign(name, name.Length());
					nameLine.marginTop = firstName ? win98metrics::kBodyLineHeight : 0;
					lines.push_back(std::move(nameLine));
					firstName = false;
				}
			}
			return lines;
		}

		CComPtr<FolderItem> item;
		CComVariant index(0L);
		if (FAILED(items->Item(index, &item)) || !item)
		{
			return lines;
		}

		// name: "<b>" + GetDetailsOf(items, 0) + "</b><br>"
		std::wstring name = DetailOf(view, item, 0);
		if (name.empty())
		{
			CComBSTR fallback;
			if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
			{
				name.assign(fallback, fallback.Length());
			}
		}
		AddBreak(lines, name);
		if (!lines.empty())
		{
			lines.back().bold = true;
		}

		// type: column 1 in My Computer, not column 2 as in an ordinary folder. The ordinals are
		// per-namespace and must not be shared between templates.
		const std::wstring type = DetailOf(view, item, 1);
		if (!type.empty())
		{
			AddBreak(lines, type);
		}

		// drive: PieChart.displayFile(items.Path) both decides whether the block appears and
		// supplies the formatted numbers. Only a real root drive that answers a free-space query
		// qualifies, which is the closest honest equivalent of the control's own test.
		std::wstring itemPath;
		{
			CComBSTR pathValue;
			if (SUCCEEDED(item->get_Path(&pathValue)) && pathValue)
			{
				itemPath.assign(pathValue, pathValue.Length());
			}
		}

		const bool isDrive = !itemPath.empty() &&
		                     PathIsRootW(itemPath.c_str()) && !PathIsUNCW(itemPath.c_str());

		// tip: GetDetailsOf(items, -1), suppressed when it merely repeats the name.
		//
		// It is also suppressed for a drive. On the reference guest a drive's InfoTip is empty, so
		// the historical pane went straight from the type to the capacity block; Windows 10
		// returns "Free Space: ... Total Size: ..." for the same call, which would print the
		// capacity figures twice, once in modern wording and again in the template's own. The
		// template's block is the one the profile is reproducing, so the tip gives way.
		const std::wstring tip = DetailOf(view, item, -1);
		if (!isDrive && !tip.empty() && tip != name)
		{
			PaneLine tipLine;
			tipLine.text = tip;
			tipLine.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(tipLine));
		}

		if (isDrive)
		{
			AppendWin98DriveInfo(lines, itemPath, capacityPer1000);
		}

		// #PieChart {width: 100px; height: 50px; margin-top: 10px} -- not the NT5 120x120 square
		// and not the Windows Me 60px graph.
		m_pane.SetCapacityPie(capacityPer1000, win98metrics::kPieHeight);
		return lines;
	}

	std::vector<PaneLine> ReferenceContent::BuildWin98SystemFolder(
		IShellFolderViewDual *view, FolderItems *items, long selected) noexcept
	{
		/*
		 * The per-folder templates the reference guest keeps in C:\WINDOWS (and SYSTEM,
		 * SYSTEM32) and in C:\Program Files. Both prepend L_Intro_Text to the ordinary body; the
		 * difference is that the C:\WINDOWS one also hides the file list behind a Show Files
		 * command anchor and collapses at 450 rather than 400.
		 *
		 * C:\ root has neither template on the reference guest, so a drive root must never reach
		 * here -- that is the Windows Me rule, not this one.
		 */
		std::vector<PaneLine> lines;
		m_pane.SetCapacityPie(-1);

		const bool barricade = (m_win98Template == Win98TemplateKind::SystemFolderBarricade);

		// L_Intro_Text: "<b><font color=red>Warning</font></b><br><br>" + the sentence.
		PaneLine warning;
		warning.text = ReferenceCaption(L"Warning");
		warning.bold = true;
		warning.boldColor = RGB(255, 0, 0);
		lines.push_back(std::move(warning));

		PaneLine body;
		body.text = ReferenceCaption(L"Modifying the contents of this folder may cause your programs to stop working correctly.");
		body.marginTop = win98metrics::kBodyLineHeight;
		lines.push_back(std::move(body));

		if (!barricade)
		{
			// The Program Files intro continues past the shared sentence.
			PaneLine addRemove;
			addRemove.text = ReferenceCaption(L"To add or remove programs, click Start, point to Settings, click Control Panel, and then click Add/Remove Programs.");
			addRemove.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(addRemove));
		}

		if (barricade && !m_showFiles)
		{
			// L_Prompt1_Text: "To view the contents of this folder, click <a class=command
			// ...>Show Files</a>". One sentence with the anchor inside it, so it is a single
			// inline-run line -- splitting it in two would leave a bare link on its own row. The
			// anchor sets window.status to "Show Files" on hover, and ShowFiles() reveals the
			// list in place rather than navigating.
			PaneLine showFiles;
			showFiles.linkPrefix = ReferenceCaption(L"To view the contents of this folder, click ");
			showFiles.text = ReferenceCaption(L"Show Files");
			showFiles.link = true;
			showFiles.bold = true;   // a.Command
			showFiles.linkAction = PaneLine::LinkAction::ShowSystemFolderFiles;
			showFiles.status = ReferenceCaption(L"Show Files");
			showFiles.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(showFiles));
			return lines;
		}

		// Revealed, or warn-only: the intro is followed by the ordinary folder body. Both
		// templates render L_Intro_Text + "<br><br>" + L_Prompt_Text when nothing is selected.
		if (selected == 0)
		{
			PaneLine prompt;
			prompt.text = ReferenceCaption(L"Select an item to view its description.");
			prompt.marginTop = win98metrics::kBodyLineHeight;
			lines.push_back(std::move(prompt));
		}

		return lines;
	}

	std::vector<PaneLine> ReferenceContent::BuildPaneLines(std::wstring &thumbnailPath) noexcept
	{
		std::vector<PaneLine> lines;
		thumbnailPath.clear();

		// Set only on the drive path; -1 leaves the pie unpainted.
		int capacityPer1000 = -1;

		CComPtr<IDispatch> automation;
		CComQIPtr<IShellFolderViewDual> view;
		if (SUCCEEDED(GetAutomation(&automation)) && automation)
		{
			view = automation;
		}

		long selected = 0;
		CComPtr<FolderItems> items;
		// SelectedItems is a method, not a property — the script called it as
		// FileList.SelectedItems() (standard.htt:122).
		if (view && SUCCEEDED(view->SelectedItems(&items)) && items)
		{
			items->get_Count(&selected);
		}

		if (m_visualProfile == WebViewVisualProfile::Windows98)
		{
			/*
			 * Windows 98 ships one .htt per namespace and they are not variations on a theme, so
			 * this dispatches on the resolved template kind rather than running the ordinary
			 * folder body and appending special cases to it.
			 *
			 * Anything not yet implemented deliberately falls through to the ordinary folder
			 * template below rather than being silently claimed as complete; the template
			 * completion matrix records which is which.
			 */
			switch (m_win98Template)
			{
			case Win98TemplateKind::MyComputer:
				return BuildWin98MyComputer(view, items, selected);

			case Win98TemplateKind::SystemFolderBarricade:
			case Win98TemplateKind::SystemFolderWarnOnly:
				/*
				 * Both system-folder templates are ordinary FOLDER.HTT with a different intro,
				 * and that intro only occupies #Info while nothing is selected. Their
				 * SelectionChanged handler is the stock one: the moment an item is selected it
				 * overwrites Info with the ordinary name/type/date/size body, and the warning
				 * goes away until the selection is cleared again.
				 *
				 * So this is handled here only while the barricade is still up, or while nothing
				 * is selected. Anything else falls through to the ordinary folder body below --
				 * otherwise selecting a file in C:\Windows shows the warning and nothing else.
				 */
				if (selected == 0 ||
				    (m_win98Template == Win98TemplateKind::SystemFolderBarricade && !m_showFiles))
				{
					return BuildWin98SystemFolder(view, items, selected);
				}
				break;

			case Win98TemplateKind::ControlPanel:
				return BuildWin98ControlPanel(view, items, selected);

			case Win98TemplateKind::Printers:
				return BuildWin98Printers(view, items, selected);

			case Win98TemplateKind::DialUpNetworking:
				return BuildWin98DialUp(view, items, selected);

			case Win98TemplateKind::NetworkNeighborhood:
				return BuildWin98NetHood(view, items, selected);

			case Win98TemplateKind::RecycleBin:
				return BuildWin98RecycleBin(view, items, selected);

			default:
				break;
			}

			/*
			 * Windows 98's ordinary folder, from WEB98/FOLDER.HTT:99-190.
			 *
			 * Its content grammar is much barer than the NT5 one and shares almost nothing with
			 * it beyond the wording of the prompt: there is no See-also list, no folder
			 * description box, no cross-links and no drive capacity block. The no-selection pane
			 * is one line.
			 *
			 * The whole template is script, so unlike Windows 2000's folder.htt every string and
			 * every break here is legible in the file itself rather than inferred from native
			 * code.
			 */
			m_pane.SetCapacityPie(-1);

			if (selected == 0)
			{
				// Info.innerHTML = L_Prompt_Text, and nothing else.
				AddBreak(lines, ReferenceCaption(L"Select an item to view its description."));
				return lines;
			}

			if (selected > 1)
			{
				// text = count + L_Multiple_Text + "<br>", then "<br>" + total, then each name
				// prefixed with "<br>". The size cap is 100 and the name cap is 16.
				wchar_t buffer[64]{};
				StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
				AddBreak(lines, buffer);

				if (selected <= 100)
				{
					LONGLONG totalSize = 0;
					for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
					{
						CComPtr<FolderItem> item;
						CComVariant index(itemIndex);
						if (SUCCEEDED(items->Item(index, &item)) && item) { totalSize += SelectionBytesAt(static_cast<DWORD>(itemIndex),item); }
					}

					if (totalSize > 0)
					{
						// Windows 98 formats this with its own FormatNumber, so the grouped byte
						// count is correct here — unlike the Windows 2000 retail folder, whose
						// body was native code using StrFormatByteSizeW.
						AddParagraph(lines, ReferenceCaption(L"Total File Size: ") +
						                    FormatExactBytes(static_cast<ULONGLONG>(totalSize)));
					}

					if (selected <= 16)
					{
						bool firstName = true;
						for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
						{
							CComPtr<FolderItem> item;
							CComVariant index(itemIndex);
							if (FAILED(items->Item(index, &item)) || !item)
							{
								continue;
							}
							CComBSTR name;
							if (FAILED(item->get_Name(&name)) || !name)
							{
								continue;
							}
							PaneLine nameLine;
							nameLine.text.assign(name, name.Length());
							nameLine.marginTop = firstName ? win98metrics::kBodyLineHeight : 0;
							lines.push_back(std::move(nameLine));
							firstName = false;
						}
					}
				}
				return lines;
			}

			// Single selection: bold name, type, then Modified and Size as labelled blocks.
			CComPtr<FolderItem> item;
			CComVariant index(0L);
			if (SUCCEEDED(items->Item(index, &item)) && item)
			{
				std::wstring name = DetailOf(view, item, 0);
				if (name.empty())
				{
					CComBSTR fallback;
					if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
					{
						name.assign(fallback, fallback.Length());
					}
				}
				AddBreak(lines, name);
				if (!lines.empty())
				{
					lines.back().bold = true;
				}

				/*
				 * FOLDER.HTT reads columns 2, 3 and 1 as Type, Modified and Size. Those ordinals
				 * were fixed on Windows 98, where every filesystem folder had the same four
				 * columns. Windows 10 varies them by folder type: in a Pictures library column 3
				 * is Size, not Modified, so the fixed ordinals rendered the size through the date
				 * branch and produced "Size:" on one line with the byte count on the next.
				 *
				 * The columns are therefore resolved by heading, with the historical ordinal kept
				 * only as the fallback for a folder that does not publish the heading.
				 */
				[[maybe_unused]] const int typeColumn = FindDetailColumn(view, ReferenceCaption(L"Type"));
				const int dateColumn = FindDetailColumn(view, ReferenceCaption(L"Date modified"), ReferenceCaption(L"Modified"));
				const int sizeColumn = FindDetailColumn(view, ReferenceCaption(L"Size"));

				// type: "<br>" + GetDetailsOf(item, 2)
				const std::wstring type = FormattedExtendedPropertyOf(item,L"System.ItemTypeText",PKEY_ItemTypeText);
				if (!type.empty())
				{
					AddBreak(lines, type);
				}

				// date: "<br><br>" + GetDetailsOf(null, 3) + ":<br>" + value. The label and the
				// value are on separate lines here, unlike the NT5 profiles.
				[[maybe_unused]] const int dateOrdinal = dateColumn > 0 ? dateColumn : 3;
				const std::wstring dateValue = FormattedExtendedPropertyOf(item,L"System.DateModified",PKEY_DateModified);
				if (g_webOptions.load()->metadata && !dateValue.empty())
				{
					// GetDetailsOf(null, 3) is the column heading; DetailOf returns it when the
					// item is null, which is the same call the template makes.
					std::wstring dateLabel = ReferenceCaption(L"Modified");
					if (dateLabel.empty())
					{
						dateLabel = ReferenceCaption(L"Modified");
					}
					PaneLine label;
					label.text = dateLabel + L":";
					label.marginTop = win98metrics::kBodyLineHeight;
					lines.push_back(std::move(label));
					AddBreak(lines, dateValue);
				}

				// size: under 1000 bytes it prints the raw count; otherwise the shell's own
				// Size column, falling back to the grouped byte count.
				ULONGLONG rawSize = SelectionBytesAt(0,item);
				if (rawSize > 0 && rawSize < 1000)
				{
					PaneLine size;
					size.text = ReferenceCaption(L"Size: ") + std::to_wstring(rawSize) + ReferenceCaption(L"\u00A0bytes");
					size.marginTop = win98metrics::kBodyLineHeight;
					lines.push_back(std::move(size));
				}
				else
				{
					[[maybe_unused]] const int sizeOrdinal = sizeColumn > 0 ? sizeColumn : 1;
					const std::wstring sizeValue = rawSize>0 ? FormatBytes(rawSize) : std::wstring();
					PaneLine size;
					size.marginTop = win98metrics::kBodyLineHeight;
					if (!sizeValue.empty())
					{
						std::wstring sizeLabel = ReferenceCaption(L"Size");
						if (sizeLabel.empty())
						{
							sizeLabel = ReferenceCaption(L"Size");
						}
						size.text = sizeLabel + L": " + sizeValue;
					}
					else if (rawSize > 0)
					{
						size.text = ReferenceCaption(L"Size: ") + FormatExactBytes(static_cast<ULONGLONG>(rawSize));
					}
					if (g_webOptions.load()->metadata && !size.text.empty())
					{
						lines.push_back(std::move(size));
					}
				}

				/*
				 * Extra details, FOLDER.HTT:
				 *
				 *   for (i = 4; i < 10; i++) {
				 *       title = fldr.GetDetailsOf(null, i);
				 *       if (!title) break;
				 *       data = fldr.GetDetailsOf(items, i);
				 *       if (title == L_Attributes_Text) { ...Properties link + RHSaCE... }
				 *       else if (data) text += "<br><br>" + title + ":<br>" + data;
				 *   }
				 *
				 * The first empty heading ends the loop, so a namespace with fewer columns simply
				 * stops early rather than probing all six.
				 */
				/*
				 * Not implemented, because on Windows 98 it produces nothing.
				 *
				 * Measured on the reference guest (evidence/win98-ie6/dom/win98-columns.txt): a
				 * Windows 98 filesystem folder publishes exactly four columns --
				 *
				 *     0 Name | 1 Size | 2 Type | 3 Modified
				 *
				 * and column 4 onwards are empty. So `title = fldr.GetDetailsOf(null, 4)` is
				 * empty, the loop breaks on its first iteration, and the reference pane ends
				 * after Size. That includes the Attributes branch and its Properties link: with
				 * no Attributes column to find, Windows 98 never reaches it on a filesystem
				 * folder, whatever the template's code allows for.
				 *
				 * Windows 10 publishes Date created, Date accessed, Attributes, Perceived type,
				 * Owner, Kind and more in that same range. Walking them because the modern shell
				 * offers them yields a pane several times longer than the one being reproduced,
				 * built from columns Windows 98 had no concept of -- so the loop is omitted
				 * rather than run against a column set the template was never written for.
				 *
				 * Namespaces whose extra columns *are* source-specified keep them, in their own
				 * builders and against their own measured ordinals: dialup.htt walks columns 1-4,
				 * printers.htt and recycle.htt name theirs individually.
				 */

				/*
				 * InfoTip, FOLDER.HTT:
				 *
				 *   data = fldr.GetDetailsOf(items, -1);
				 *   if (data && data != name) { ...linkify the first http:// or file://... }
				 *
				 * The template searches for "http://" first and only then "file://", linkifies
				 * the run up to the next space, and keeps the surrounding text plain. Its two
				 * substring bounds are off by one -- substring(0, start - 1) drops the character
				 * before the URL and substring(end + 1) drops the one at the break -- which is
				 * how the separating space disappears. Reproduced, since that is what the
				 * reference renders.
				 *
				 * The tip is parsed as text, never as markup: nothing here interprets HTML from
				 * an untrusted source.
				 */
				/*
				 * The tip comes from the item's comment, not from GetDetailsOf(item, -1).
				 *
				 * Those are different properties on the modern shell. Windows 98's column -1 was
				 * the OLE Summary Information "Comments" field -- PKEY_Comment,
				 * {F29F85E0-4FF9-1068-AB91-08002B27B3D9},6 in the SDK's propkey.h -- which is why
				 * the template talks about parsing lines "for Office files". It is empty for an
				 * ordinary executable, and the reference guest confirms it: tip(-1) comes back
				 * blank for files and folders alike.
				 *
				 * Windows 10 answers the same call with PKEY_InfoTipText
				 * ({C9944A21-A406-48FE-8225-AEC7E24C211B},17), a shell-synthesised block of
				 * "File description / Company / File version / Date created / Size". That has no
				 * Windows 98 counterpart, and it repeats fields already shown above.
				 *
				 * System.Comment is the era-correct property: empty for an executable, so the
				 * pane ends after Size as the reference does, and populated for a document that
				 * genuinely carries a comment, which is the case the template was written for.
				 */
				const std::wstring tip = g_webOptions.load()->metadata ? CommentOf(item) : std::wstring();
				if (!tip.empty() && tip != name)
				{
					size_t start = tip.find(L"http://");
					if (start == std::wstring::npos)
					{
						start = tip.find(L"file://");
					}

					if (start == std::wstring::npos)
					{
						// No link: each embedded newline becomes its own line, matching the
						// template's split("\n").join("<br>\n").
						bool firstSegment = true;
						size_t segmentStart = 0;
						while (segmentStart <= tip.size())
						{
							size_t breakAt = tip.find(L'\n', segmentStart);
							const size_t segmentEnd =
								(breakAt == std::wstring::npos) ? tip.size() : breakAt;
							std::wstring segment =
								tip.substr(segmentStart, segmentEnd - segmentStart);
							while (!segment.empty() && segment.back() == L'\r')
							{
								segment.pop_back();
							}
							AddWin98Line(lines, segment, firstSegment ? 2 : 1);
							firstSegment = false;
							if (breakAt == std::wstring::npos)
							{
								break;
							}
							segmentStart = breakAt + 1;
						}
					}
					else
					{
						size_t end = tip.find(L' ', start);
						if (end == std::wstring::npos)
						{
							end = tip.size();
						}

						PaneLine link;
						if (start > 0)
						{
							link.linkPrefix = tip.substr(0, start - 1);
						}
						link.text = tip.substr(start, end - start);
						if (end < tip.size())
						{
							link.linkSuffix = tip.substr(end + 1);
						}
						// Newlines cannot survive a single inline line; they become spaces so the
						// run layout can wrap the sentence normally.
						auto flatten = [](std::wstring &s) noexcept
						{
							for (wchar_t &ch : s)
							{
								if (ch == L'\n' || ch == L'\r')
								{
									ch = L' ';
								}
							}
						};
						flatten(link.linkPrefix);
						flatten(link.linkSuffix);

						link.link = true;
						link.linkTarget = link.text;
						// Routed through the branch's external-navigation policy, which denies
						// and logs rather than launching anything from folder metadata.
						link.linkAction = PaneLine::LinkAction::DeniedHttp;
						link.status = link.text;
						link.marginTop = win98metrics::kBodyLineHeight;
						lines.push_back(std::move(link));
					}
				}

				/*
				 * FOLDER.HTT's last act on a single selection:
				 *
				 *   if (size && (size < 10000000) && Thumbnail.displayFile(items.Path))
				 *       timer = window.setTimeout(... Status.style.display = "" ..., 1000);
				 *
				 * so the request is gated on a nonzero size strictly below ten million bytes --
				 * a flat byte count, not a "large file" heuristic -- and the "Generating
				 * preview..." status only appears if the control is still working a second later.
				 *
				 * wantMedia is false in the shipped template, so no media player is instantiated
				 * for AVI/WAV and those files simply take the ordinary thumbnail path.
				 */
				std::wstring itemPath;
				{
					CComBSTR pathValue;
					if (SUCCEEDED(item->get_Path(&pathValue)) && pathValue)
					{
						itemPath.assign(pathValue, pathValue.Length());
					}
				}

				if (!itemPath.empty() && rawSize > 0 &&
				    rawSize < win98metrics::kThumbnailByteLimit)
				{
					if (const wchar_t *rejection = ThumbnailPathRejection(itemPath))
					{
						EvidenceLog::Instance().Write(
							L"thumbnail.skipped", m_generation,
							JsonFields().Hex(L"pathHash", ThumbnailPathHash(itemPath))
							            .Str(L"reason", rejection)
							            .Str(L"template", L"FOLDER.HTT"));
					}
					else
					{
						thumbnailPath = itemPath;
					}
				}
				else if (!itemPath.empty())
				{
					EvidenceLog::Instance().Write(
						L"thumbnail.skipped", m_generation,
						JsonFields().Hex(L"pathHash", ThumbnailPathHash(itemPath))
						            .Str(L"reason", rawSize > 0 ? L"over 10,000,000 bytes"
						                                        : L"zero size")
						            .Str(L"template", L"FOLDER.HTT"));
				}
			}
			return lines;
		}

		if (m_isMeDriveRoot)
		{
			/*
			 * Windows Me sysroot.htt.
			 *
			 * Initialize() (sysroot.htt:414-415) seeds #Info with
			 * L_Intro_Text + ShowLinks() + L_Barricade_Text, and Load() then calls NoneSelected()
			 * purely for its side effects: SetLegend() fills #ThumbnailLegend and the drive graph
			 * is shown at height 60. The legend and the graph therefore appear in *both* states,
			 * because they live in their own elements rather than inside #Info — which is exactly
			 * what evidence/winme-reference/drive-c.png shows.
			 *
			 * ShowFiles() (sysroot.htt:495-501) replaces #Info with
			 * L_Simple_Text + "<p>" + L_Prompt_Text and reveals the list. The legend and graph
			 * are left alone.
			 */
			if (!m_showFiles)
			{
				AddBreak(lines,
				         ReferenceCaption(L"Use the links below to manage or access items stored on your local disk."));

				// ShowLinks(): a 12px-indented div of three <p class=Half> anchors.
				bool firstLink = true;
				auto addManageLink = [&](const wchar_t *text, const wchar_t *title,
				                         PaneLine::LinkAction action, int csidl) noexcept
				{
					PaneLine line;
					line.text = text;
					line.link = true;
					line.indent = panemetrics::kBrandPadLeft;
					line.marginTop = firstLink ? panemetrics::kParagraphTop
					                           : panemetrics::kHalfParagraphTop;
					line.linkAction = action;
					line.csidl = csidl;
					line.status = title;
					line.tooltip = title;
					lines.push_back(std::move(line));
					firstLink = false;
				};

				addManageLink(ReferenceCaption(L"My Documents"), ReferenceCaption(L"My Documents contains your personal documents."),
				              PaneLine::LinkAction::BrowseCsidl, CSIDL_PERSONAL);
				// InvokeARP() and InvokeSearchBand() drove shell entry points this branch does
				// not activate; the anchors are drawn and denied, as elsewhere.
				addManageLink(ReferenceCaption(L"Add/Remove Programs"),
				              ReferenceCaption(L"Installs and removes programs and Windows components."),
				              PaneLine::LinkAction::DeniedLegacyCommand, -1);
				addManageLink(ReferenceCaption(L"Search for Files or Folders"),
				              ReferenceCaption(L"Finds files and folders based on the criteria you specify."),
				              PaneLine::LinkAction::DeniedLegacyCommand, -1);

				// L_Barricade_Text closes the div and stands on its own paragraph. The trailing
				// full stop is outside the anchor.
				PaneLine reveal;
				reveal.text = ReferenceCaption(L"View the entire contents of this drive");
				reveal.linkSuffix = L".";
				reveal.link = true;
				reveal.linkAction = PaneLine::LinkAction::ShowEntireContents;
				reveal.marginTop = panemetrics::kParagraphTop;
				reveal.status = ReferenceCaption(L"Displays the contents of the drive.");
				reveal.tooltip = reveal.status;
				lines.push_back(std::move(reveal));
			}
			else
			{
				// L_Simple_Text, then the ordinary prompt.
				PaneLine hide;
				hide.text = ReferenceCaption(L"Hide the contents of this drive");
				hide.linkSuffix = L".";
				hide.link = true;
				hide.linkAction = PaneLine::LinkAction::HideDriveContents;
				lines.push_back(std::move(hide));

				AddParagraph(lines, ReferenceCaption(L"Select an item to view its description."));
			}

			// SetLegend() plus the height-60 graph, in both states.
			const std::wstring drivePath = FolderPathOf(view);
			if (!drivePath.empty())
			{
				AppendDriveInfo(lines, drivePath, capacityPer1000);
			}
			// This branch returns early, so it has to publish the pie itself; the shared exit at
			// the end of BuildPaneLines is what normally does this. sysroot.htt:350 sets the
			// graph's height to 60 for this state.
			m_pane.SetCapacityPie(capacityPer1000, 60);
			return lines;
		}

		if (m_isEntireNetwork && !m_showFiles)
		{
			// nethood.htt:75-90 InitSoftBarrierText(). The file list is hidden and this text is
			// the whole body; there is no selection to describe and ShowLinks() is never called,
			// so the See-also block stays empty until the barrier comes down.
			m_pane.SetCapacityPie(-1);
			AddBreak(lines,
			         ReferenceCaption(L"Use the links below to search for things within your corporation."));

			// <div style="margin-left: 12px"> holds the anchors; each ends in an explicit <br>,
			// so they stack one body line apart with no paragraph margin between them. The empty
			// <p> that L_SBIntro1_Text opens puts the div one paragraph margin below the intro.
			bool firstSearch = true;
			auto addSearch = [&](const wchar_t *text, PaneLine::LinkAction action,
			                     bool available) noexcept
			{
				PaneLine line;
				line.text = text;
				line.link = true;
				line.indent = panemetrics::kBrandPadLeft;
				line.marginTop = firstSearch ? panemetrics::kParagraphTop : 0;
				line.linkAction = available ? action : PaneLine::LinkAction::DeniedLegacyCommand;
				line.linkTarget = text;
				lines.push_back(std::move(line));
				firstSearch = false;
			};

			// gDirService gates the two directory-backed searches, exactly as the template does.
			if (m_directoryService)
			{
				addSearch(ReferenceCaption(L"Search for printers"), PaneLine::LinkAction::FindPrinter, true);
			}
			addSearch(ReferenceCaption(L"Search for computers"), PaneLine::LinkAction::FindComputer, true);
			if (m_directoryService)
			{
				// PersonSearch() went through SearchAssistantOC, which Windows 10 does not carry
				// and which has no canonical replacement verb. The link is drawn and denied.
				addSearch(ReferenceCaption(L"Search for people"), PaneLine::LinkAction::DeniedLegacyCommand, false);
			}
			addSearch(ReferenceCaption(L"Search for files or folders"), PaneLine::LinkAction::FindFiles, true);

			// L_SBIntro2_Text = "<p><br>You may also view the <a>entire contents</a> of the
			// network." The <br> immediately inside the paragraph adds one empty line box on top
			// of p's 12px margin.
			PaneLine contents;
			contents.boldPrefix = ReferenceCaption(L"You may also view the ");
			contents.text = ReferenceCaption(L"entire contents");
			contents.boldSuffix = ReferenceCaption(L" of the network.");
			contents.link = true;
			contents.linkAction = PaneLine::LinkAction::ShowEntireContents;
			contents.marginTop = panemetrics::kParagraphTop + panemetrics::kBodyLineHeight;
			lines.push_back(std::move(contents));
			return lines;
		}

		if (UsesImgViewProfile())
		{
			// imgview.htt:96-168,280-314 is a distinct selection profile. The upper
			// details half remains driven by the active shell view; shimgvw owns only the
			// lower preview/toolbar half, represented by the pane preview state below.
			m_pane.SetCapacityPie(-1);
			if (selected == 0)
			{
				AddBreak(lines, ReferenceCaption(L"Select an item to view its description."));
                std::wstring comment=FolderCommentOf(view);
                if(comment.empty()) comment=ProfileFolderComment(m_visualProfile,m_isMyComputer,m_isMyDocuments,m_isMyPictures);
                AddMessage(lines,std::move(comment));

				PaneLine seeAlso;
				seeAlso.text = ReferenceCaption(L"See also:");
				seeAlso.marginTop = panemetrics::kImgParagraphTop;
				lines.push_back(std::move(seeAlso));
				AddLink(lines, ReferenceCaption(L"My Documents"), CSIDL_PERSONAL,
				        ReferenceCaption(L"My Documents contains your personal documents."));
				AddLink(lines, ReferenceCaption(L"My Computer"), CSIDL_DRIVES,
				        ReferenceCaption(L"My Computer contains your various local drives and mapped network drives."));
				AddLink(lines, ReferenceCaption(L"My Network Places"), CSIDL_NETWORK,
				        ReferenceCaption(L"My Network Places contains shortcuts to various locations on the corporate network and the Internet."));
				SetImgViewPreview(ImgPreviewState::NoSelection, {});
				return lines;
			}

			if (selected > 1)
			{
				wchar_t countText[64]{};
				StringCchPrintfW(countText, ARRAYSIZE(countText), ReferenceCaption(L"%ld items selected."), selected);
				AddBreak(lines, countText);

				if (selected <= 100)
				{
					ULONGLONG totalSize = 0;
					CComQIPtr<IFolderView2> folderView(m_spView);
					CComPtr<IShellItemArray> shellSelection;
					if (folderView)
					{
						folderView->GetSelection(FALSE, &shellSelection);
					}

					for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
					{
						ULONGLONG itemSize = 0;
						CComPtr<IShellItem> selectedShellItem;
						if (shellSelection &&
						    SUCCEEDED(shellSelection->GetItemAt(static_cast<DWORD>(itemIndex),
						                                               &selectedShellItem)))
						{
							CComQIPtr<IShellItem2> shellItem(selectedShellItem);
							if (shellItem)
							{
								shellItem->GetUInt64(PKEY_Size, &itemSize);
							}
						}
						if (itemSize == 0)
						{
							CComPtr<FolderItem> item;
							CComVariant index(itemIndex);
							LONG automationSize = 0;
							if (SUCCEEDED(items->Item(index, &item)) && item &&
							    SUCCEEDED(item->get_Size(&automationSize)) && automationSize > 0)
							{
								itemSize = static_cast<ULONGLONG>(automationSize);
							}
						}
						totalSize += itemSize;
					}

					if (totalSize > 0)
					{
						PaneLine total;
						total.text = ReferenceCaption(L"Total File Size: ") + FormatExactBytes(totalSize);
						total.marginTop = panemetrics::kImgParagraphTop;
						lines.push_back(std::move(total));
					}

					if (selected <= 16)
					{
						for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
						{
							CComPtr<FolderItem> item;
							CComVariant index(itemIndex);
							CComBSTR name;
							if (FAILED(items->Item(index, &item)) || !item ||
							    FAILED(item->get_Name(&name)) || !name)
							{
								continue;
							}
							PaneLine nameLine;
							nameLine.text.assign(name, name.Length());
							if (itemIndex == 0)
							{
								nameLine.marginTop = panemetrics::kImgParagraphTop;
							}
							lines.push_back(std::move(nameLine));
						}
					}
				}

				SetImgViewPreview(ImgPreviewState::Multiple, {});
				return lines;
			}

			CComPtr<FolderItem> item;
			CComVariant index(0L);
			if (FAILED(items->Item(index, &item)) || !item)
			{
				SetImgViewPreview(ImgPreviewState::Failed, {});
				return lines;
			}

			CComPtr<IShellItem> selectedShellItem;
			CComQIPtr<IFolderView2> folderView(m_spView);
			CComPtr<IShellItemArray> shellSelection;
			if (folderView && SUCCEEDED(folderView->GetSelection(FALSE, &shellSelection)) && shellSelection)
			{
				shellSelection->GetItemAt(0, &selectedShellItem);
			}
			CComQIPtr<IShellItem2> shellItem(selectedShellItem);

			std::wstring name = DetailOf(view, item, 0);
			if (name.empty())
			{
				CComBSTR fallback;
				if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
				{
					name.assign(fallback, fallback.Length());
				}
			}
			AddBreak(lines, name);
			if (!lines.empty())
			{
				lines.back().bold = true;
			}

			std::wstring type = FormatShellProperty(shellItem, PKEY_ItemTypeText);
			if (type.empty())
			{
				type = DetailOf(view, item, 2);
			}
			AddBreak(lines, type);

			std::wstring modified = FormatShellProperty(shellItem, PKEY_DateModified);
			if (modified.empty())
			{
				modified = DetailOf(view, item, 3);
			}
			AddImgViewDetail(lines, ReferenceCaption(L"Date modified"), modified);

			ULONGLONG rawSize = 0;
			if (shellItem)
			{
				shellItem->GetUInt64(PKEY_Size, &rawSize);
			}
			if (rawSize == 0)
			{
				LONG automationSize = 0;
				if (SUCCEEDED(item->get_Size(&automationSize)) && automationSize > 0)
				{
					rawSize = static_cast<ULONGLONG>(automationSize);
				}
			}
			std::wstring size;
			if (rawSize > 0 && rawSize < 1000)
			{
				size = FormatExactBytes(rawSize);
			}
			else
			{
				size = FormatShellProperty(shellItem, PKEY_Size);
				if (size.empty())
				{
					size = DetailOf(view, item, 1);
				}
				if (size.empty() && rawSize > 0)
				{
					size = FormatExactBytes(rawSize);
				}
			}
			AddImgViewDetail(lines, ReferenceCaption(L"Size"), size);

			std::wstring attributeCodes;
			TryFileAttributeCodes(item, shellItem, attributeCodes);
			AddImgViewDetail(lines, ReferenceCaption(L"Attributes"), FormatAttributes(attributeCodes));
			AddImgViewDetail(lines, ReferenceCaption(L"Created"),
			                 FormatShellProperty(shellItem, PKEY_DateCreated));
			AddImgViewDetail(lines, ReferenceCaption(L"Accessed"),
			                 FormatShellProperty(shellItem, PKEY_DateAccessed));
			AddImgViewDetail(lines, ReferenceCaption(L"Owner"),
			                 FormatShellProperty(shellItem, L"System.FileOwner"));

			CComBSTR itemPath;
			std::wstring path;
			if (SUCCEEDED(item->get_Path(&itemPath)) && itemPath)
			{
				path.assign(itemPath, itemPath.Length());
			}
			if (const wchar_t *rejection = ThumbnailPathRejection(path))
			{
				EvidenceLog::Instance().Write(
					L"thumbnail.skipped", m_generation,
					JsonFields().Hex(L"pathHash", ThumbnailPathHash(path)).Str(L"reason", rejection));
				SetImgViewPreview(ImgPreviewState::Failed, {});
			}
			else
			{
				thumbnailPath = path;
				SetImgViewPreview(ImgPreviewState::Loading, {});
			}
			return lines;
		}

		if (m_isRecycleBin)
		{
			// recycle.htt:69-73 and 120-217. This namespace has its own no-selection,
			// single-selection and multiple-selection bodies; none of the generic folder.htt
			// Message, thumbnail, attributes, drive or See-also branches apply here.
			if (selected == 0)
			{
				AddBreak(lines,
					ReferenceCaption(L"This folder contains files and folders that you have deleted from your computer."));

				long folderItemCount = 0;
				if (!TryFolderItemCount(view, folderItemCount))
				{
					// Do not expose destructive controls when the source state cannot be observed.
					EvidenceLog::Instance().Write(L"pane.recycleStateUnknown", m_generation,
					                              JsonFields().Str(L"stage", L"Folder.Items.Count"));
					AddParagraph(lines, ReferenceCaption(L"Select an item to view its description."));
				}
				else if (folderItemCount == 0)
				{
					AddParagraph(lines, ReferenceCaption(L"There are no items in the Recycle Bin."));
				}
				else
				{
					AddParagraph(lines,
						ReferenceCaption(L"To permanently remove all items and reclaim disk space, click:"));
					AddRecycleButton(lines, ReferenceCaption(L"Empty Recycle Bin"),
					                 PaneLine::LinkAction::RecycleEmpty, L'B', 128);

					AddParagraph(lines,
						ReferenceCaption(L"To move all items back to their original locations, click:"));
					AddRecycleButton(lines, ReferenceCaption(L"Restore All"),
					                 PaneLine::LinkAction::RecycleRestoreAll, L'R', 128);

					AddParagraph(lines, ReferenceCaption(L"Select an item to view its description."));
				}
			}
			else if (selected == 1)
			{
				CComPtr<FolderItem> item;
				CComVariant index(0L);
				if (SUCCEEDED(items->Item(index, &item)) && item)
				{
					CComPtr<IShellItem> selectedShellItem;
					CComQIPtr<IFolderView2> folderView(m_spView);
					CComPtr<IShellItemArray> selection;
					if (folderView && SUCCEEDED(folderView->GetSelection(FALSE, &selection)) && selection)
					{
						selection->GetItemAt(0, &selectedShellItem);
					}
					CComQIPtr<IShellItem2> shellItem(selectedShellItem);

					std::wstring name = DetailOf(view, item, 0);
					if (name.empty())
					{
						CComBSTR fallback;
						if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
						{
							name.assign(fallback, fallback.Length());
						}
					}
					AddBreak(lines, name);
					if (!lines.empty())
					{
						lines.back().bold = true;
					}

					// recycle.htt used fixed Win2K column ordinals. The Windows 10 Recycle Bin
					// exposes a different order, so read the equivalent canonical properties
					// from the selected item rather than applying the historical ordinals to the
					// modern view.
					AddBreak(lines, FormatShellProperty(shellItem, PKEY_ItemTypeText));

					const std::wstring originalLocation =
						FormatShellProperty(shellItem, L"System.Recycle.DeletedFrom");
					if (!originalLocation.empty())
					{
						AddParagraph(lines, ReferenceCaption(L"Original Location:"));
						PaneLine location;
						location.text = originalLocation;
						location.link = true;
						location.status = originalLocation;
						location.linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
						location.linkTarget = originalLocation;
						// String.link(data) had no title attribute; tooltip stays empty.
						lines.push_back(std::move(location));
					}

					const std::wstring dateDeleted =
						FormatShellProperty(shellItem, L"System.Recycle.DateDeleted");
					if (!dateDeleted.empty())
					{
						AddParagraph(lines, ReferenceCaption(L"Date Deleted:"));
						AddBreak(lines, dateDeleted);
					}

					ULONGLONG itemSize = 0;
					HRESULT sizeResult = shellItem ? shellItem->GetUInt64(PKEY_Size, &itemSize) : E_NOINTERFACE;
					if (FAILED(sizeResult))
					{
						LONG automationSize = 0;
						if (SUCCEEDED(item->get_Size(&automationSize)) && automationSize > 0)
						{
							itemSize = static_cast<ULONGLONG>(automationSize);
							sizeResult = S_OK;
						}
					}
					if (SUCCEEDED(sizeResult) && itemSize > 0)
					{
						if (itemSize < 1000)
						{
							AddParagraph(lines, ReferenceCaption(L"Size: ") + FormatExactBytes(itemSize));
						}
						else
						{
							const std::wstring shellSize = FormatShellProperty(shellItem, PKEY_Size);
							if (!shellSize.empty())
							{
								AddParagraph(lines, ReferenceCaption(L"Size: ") + shellSize);
							}
							else
							{
								AddParagraph(lines, ReferenceCaption(L"Size: ") + FormatExactBytes(itemSize));
							}
						}
					}

					const std::wstring infoTip = DetailOf(view, item, -1);
					if (!infoTip.empty() && infoTip != name)
					{
						AddParagraph(lines, infoTip);
					}

					AddParagraph(lines,
						ReferenceCaption(L"To move this item back to its original location, click:"));
					AddRecycleButton(lines, ReferenceCaption(L"Restore"),
					                 PaneLine::LinkAction::RecycleRestoreSelection, L'e');
				}
			}
			else
			{
				wchar_t buffer[64]{};
				StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
				AddBreak(lines, buffer);

				if (selected <= 100)
				{
					LONGLONG totalSize = 0;
					for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
					{
						CComPtr<FolderItem> item;
						CComVariant index(itemIndex);
						if (SUCCEEDED(items->Item(index, &item)) && item) { totalSize += SelectionBytesAt(static_cast<DWORD>(itemIndex),item); }
					}

					if (totalSize > 0)
					{
						AddParagraph(lines,
							ReferenceCaption(L"Total Size: ") + FormatExactBytes(static_cast<ULONGLONG>(totalSize)));
					}

					if (selected <= 16)
					{
						bool firstName = true;
						for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
						{
							CComPtr<FolderItem> item;
							CComVariant index(itemIndex);
							CComBSTR itemName;
							if (FAILED(items->Item(index, &item)) || !item ||
							    FAILED(item->get_Name(&itemName)) || !itemName)
							{
								continue;
							}

							PaneLine nameLine;
							nameLine.text.assign(itemName, itemName.Length());
							nameLine.marginTop = firstName ? panemetrics::kParagraphTop : 0;
							lines.push_back(std::move(nameLine));
							firstName = false;
						}
					}
				}

				// recycle.htt emits two consecutive <p> tags before this sentence. IE6 places
				// the empty and following paragraph at the same top (also captured in dialup.htt),
				// so the visible result is one 12px paragraph margin, not two stacked margins.
				PaneLine restorePrompt;
				restorePrompt.text =
					ReferenceCaption(L"To move these items back to their original locations, click:");
				restorePrompt.marginTop = panemetrics::kParagraphTop;
				lines.push_back(std::move(restorePrompt));
				AddRecycleButton(lines, ReferenceCaption(L"Restore"),
				                 PaneLine::LinkAction::RecycleRestoreSelection, L'e');
			}

			m_pane.SetCapacityPie(-1);
			return lines;
		}

		if (m_isControlPanel)
		{
			// controlp.htt is a separate template. It never uses the generic type/date/size,
			// Message-box, or See-also branches below.
			if (selected == 1)
			{
				CComPtr<FolderItem> item;
				CComVariant index(0L);
				if (SUCCEEDED(items->Item(index, &item)) && item)
				{
					std::wstring name = DetailOf(view, item, 0);
					if (name.empty())
					{
						CComBSTR fallback;
						if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
						{
							name.assign(fallback, fallback.Length());
						}
					}
					AddBreak(lines, std::move(name));
					if (!lines.empty())
					{
						lines.back().bold = true;
					}

					// Windows 2000's Control Panel column 1 was the applet description. On
					// Windows 10 it is Category, so never feed that numeric/category value into
					// the historical description slot.
					AddBreak(lines, CommentOf(item));
				}
			}
			else if (selected > 1)
			{
				wchar_t buffer[64]{};
				StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
				AddBreak(lines, buffer);

				// controlp.htt prefixes every name with <br> when at most sixteen are selected.
				// Together with the count's trailing <br>, the first prefix leaves one blank line.
				if (selected <= 16)
				{
					for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
					{
						CComPtr<FolderItem> item;
						CComVariant index(itemIndex);
						if (FAILED(items->Item(index, &item)) || !item)
						{
							continue;
						}
						PaneLine nameLine;
						nameLine.text = DetailOf(view, item, 0);
						nameLine.marginTop = (itemIndex == 0) ? panemetrics::kBodyLineHeight : 0;
						lines.push_back(std::move(nameLine));
					}
				}
			}
			else
			{
				// L_Intro_Text is word-for-word the same in both profiles.
				AddBreak(lines, ReferenceCaption(L"Use the settings in Control Panel to personalize your computer."));

				PaneLine prompt;
				prompt.text = ReferenceCaption(L"Select an item to view its description.");
				prompt.marginTop = panemetrics::kBodyLineHeight; // L_Intro_Text ends in <br><br>
				lines.push_back(std::move(prompt));

				// Windows Me's Control Panel has two modes. Its default raises a barricade and
				// replaces the *view* with a curated eight-item list built from
				// gitemCanonicalName, each row an icon, a bold link and an InfoTip
				// (WEBME/controlp.htt:98-120, and see evidence/winme-reference/controlpanel.png).
				// The pane then offers "view all Control Panel options"; the full view offers
				// "Display only commonly used Control Panel options" back.
				//
				// This branch does not own the view — the live SysListView32 stays the folder
				// model — so the curated list cannot be produced, and raising the barricade would
				// leave an empty view area rather than that list. The full-view state is the one
				// reproduced, and its mode-toggle link is deliberately omitted rather than drawn
				// dead. Restoring it needs the curated list first.
			}

			// Both profiles carry the Windows Update link with the same target
			// (WEBME/controlp.htt:241 is byte-identical to the Windows 2000 one). Only the
			// second link differs.
			AddDeniedHttpLink(lines, ReferenceCaption(L"Windows Update"),
				L"http://www.microsoft.com/isapi/redir.dll?prd=Win2000&ar=WinUpdate",
				panemetrics::kParagraphTop);
			if (m_visualProfile == WebViewVisualProfile::WindowsME)
			{
				// WEBME/controlp.htt:243.
				AddDeniedHttpLink(lines, ReferenceCaption(L"Technical Support"),
					L"http://www.microsoft.com/isapi/redir.dll?prd=support&ar=millennium",
					panemetrics::kHalfParagraphTop);
			}
			else
			{
				AddDeniedHttpLink(lines, ReferenceCaption(L"Windows 2000 Support"),
					L"http://www.microsoft.com/isapi/redir.dll?prd=support&sbp=portal",
					panemetrics::kHalfParagraphTop);
			}
			m_pane.SetCapacityPie(-1);
			return lines;
		}

		if (m_isPrinters)
		{
			// printers.htt:217-264. The live shell view remains the item model and command target;
			// this branch only reproduces the template's selection-dependent left pane.
			constexpr int kStaticLinkRows = panemetrics::kBodyLineHeight * 3;

			auto displayNameOf = [&](FolderItem *item) noexcept
			{
				std::wstring name;
				CComBSTR itemName;
				if (item && SUCCEEDED(item->get_Name(&itemName)) && itemName)
				{
					name.assign(itemName, itemName.Length());
				}
				if (name.empty())
				{
					name = DetailOf(view, item, 0);
				}
				return name;
			};

			int supportMargin = kStaticLinkRows;
			if (selected == 0)
			{
				AddBreak(lines,
					ReferenceCaption(L"This folder contains information about printers that are currently installed, and a wizard to help you install new printers."));
				AddParagraph(lines,
					ReferenceCaption(L"To get information about a printer that is currently installed, right-click the printer's icon."));

				PaneLine install;
				install.text = ReferenceCaption(L"Add Printer");
				install.boldPrefix = ReferenceCaption(L"To install a new printer, click the ");
				install.boldSuffix = ReferenceCaption(L" icon.");
				install.marginTop = panemetrics::kParagraphTop;
				lines.push_back(std::move(install));

				AddParagraph(lines, ReferenceCaption(L"Select an item to view its description."));
			}
			else if (selected > 1)
			{
				wchar_t buffer[64]{};
				StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
				AddBreak(lines, buffer);

				if (selected <= 16)
				{
					bool firstName = true;
					for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
					{
						CComPtr<FolderItem> item;
						CComVariant index(itemIndex);
						if (FAILED(items->Item(index, &item)) || !item)
						{
							continue;
						}

						PaneLine name;
						name.text = displayNameOf(item);
						name.marginTop = firstName ? panemetrics::kBodyLineHeight : 0;
						lines.push_back(std::move(name));
						firstName = false;
					}
				}
				else
				{
					// The count string ends in <br>; with no names, that empty row remains visible.
					supportMargin += panemetrics::kBodyLineHeight;
				}
			}
			else
			{
				CComPtr<FolderItem> item;
				CComVariant index(0L);
				if (SUCCEEDED(items->Item(index, &item)) && item)
				{
					const std::wstring printerName = DetailOf(view, item, 0);
					AddBreak(lines, displayNameOf(item));
					if (!lines.empty())
					{
						lines.back().bold = true;
					}

					if (printerName == kAddPrinterSentinel)
					{
						AddParagraph(lines,
							ReferenceCaption(L"The Add Printer wizard gives you step-by-step instructions for installing a printer. To install a new printer, double-click the Add Printer icon."));
					}
					else if (!printerName.empty())
					{
						struct PrinterDetail
						{
							PCWSTR title;
							PCWSTR alternateTitle;
							bool status;
						};
						const PrinterDetail details[] =
						{
							{ L"Documents", nullptr, false },
							{ ReferenceCaption(L"Status"), nullptr, true },
							{ ReferenceCaption(L"Comment"), ReferenceCaption(L"Comments"), false },
							{ ReferenceCaption(L"Location"), nullptr, false },
							{ ReferenceCaption(L"Model"), nullptr, false },
						};

						for (const PrinterDetail &detail : details)
						{
							const int column = FindDetailColumn(view, detail.title, detail.alternateTitle);
							if (column < 0)
							{
								continue;
							}

							const std::wstring data = DetailOf(view, item, column);
							if (data.empty())
							{
								continue;
							}

							const std::wstring title(detail.title);
							const bool separate = title.length() + data.length() > 32;
							if (detail.status)
							{
								PaneLine statusLine;
								statusLine.text = data;
								statusLine.boldPrefix = title + (separate ? L":\n" : L": ");
								statusLine.boldColor = (data == ReferenceCaption(L"Ready"))
									? RGB(0, 128, 0) : RGB(255, 0, 0);
								statusLine.marginTop = panemetrics::kParagraphTop;
								lines.push_back(std::move(statusLine));
							}
							else
							{
								AddParagraph(lines, title + (separate ? L":\n" : L": ") + data);
							}
						}

						// printers.htt:276-309 OnInfoReady. ShowInfo() seeds JobInfo, MoreInfo and
						// OemUrlInfo with placeholders and the control fills them in
						// asynchronously; the pane resolves them up front instead, because
						// nothing here is slow enough to need a second pass.
						PrinterEta eta;
						const bool haveEta = CachedPrinterEta(printerName, eta);

						// Status & 1: the waiting time. Status & 4 blanks it again, which is what
						// an unusable device or an errored job produces.
						if (haveEta && eta.waitingTimeValid)
						{
							const std::wstring value =
								printerjobeta::FormatWaitingTime(eta.waitingMinutes);
							const std::wstring label = ReferenceCaption(L"Waiting Time");
							const bool separate = label.length() + value.length() > 32;
							AddParagraph(lines, label + (separate ? L":\n" : L": ") + value);
							supportMargin -= panemetrics::kBodyLineHeight;
						}

						// Status & 2: the printer's own web page.
						if (haveEta && !eta.webUrl.empty())
						{
							PaneLine moreInfo;
							moreInfo.text = ReferenceCaption(L"Get More Info");
							moreInfo.link = true;
							// The anchor's content is <b>, unlike every other link in this pane.
							moreInfo.bold = true;
							moreInfo.marginTop = panemetrics::kBodyLineHeight;
							moreInfo.status = eta.webUrl;
							moreInfo.linkAction = PaneLine::LinkAction::DeniedHttp;
							moreInfo.linkTarget = eta.webUrl;
							moreInfo.csidl = -1;
							lines.push_back(std::move(moreInfo));
							supportMargin -= panemetrics::kBodyLineHeight;
						}

						// Status & 8: the OEM support page, suppressed when a custom support link
						// is configured — the template's bCustomUrl, set in Load() from
						// PrinterURLObject and read back here.
						const bool customSupportUrl =
							!PrinterSupportValue(L"SupportLinkName").empty() &&
							!PrinterSupportValue(L"SupportLink").empty();
						if (haveEta && !customSupportUrl && !eta.oemUrl.empty() &&
						    !eta.manufacturer.empty())
						{
							AddDeniedHttpLink(lines, (eta.manufacturer + ReferenceCaption(L" Support")).c_str(),
							                  eta.oemUrl.c_str(), panemetrics::kBodyLineHeight);
							supportMargin -= panemetrics::kBodyLineHeight;
						}

						if (supportMargin < 0)
						{
							supportMargin = 0;
						}

						EvidenceLog::Instance().Write(
							L"printers.jobEta", m_generation,
							JsonFields().Str(ReferenceCaption(L"printer"), printerName)
							            .Bool(L"queried", haveEta)
							            .Bool(L"waitingTimeValid", eta.waitingTimeValid)
							            .Num(L"waitingMinutes", eta.waitingMinutes)
							            .Str(L"webUrl", eta.webUrl)
							            .Str(ReferenceCaption(L"manufacturer"), eta.manufacturer)
							            .Bool(L"hasOemUrl", !eta.oemUrl.empty())
							            .Bool(L"customSupportUrl", customSupportUrl));
					}
				}
			}

			AddPrinterSupportLink(lines, supportMargin);
			m_pane.SetCapacityPie(-1);
			return lines;
		}

		if (m_isNetworkConnections)
		{
			// dialup.htt is a separate template. Its body has no generic Message box,
			// thumbnail, attributes, total-size summary, or See-also list.
			if (selected == 0)
			{
				// The fixed 188px Details column wraps into these exact physical rows at
				// 96 DPI. The SP4/IE6 probe places their paragraph starts at y=170, 208,
				// 233, 284, 322 and 373 after the two-row folder heading moves Details to 106.
				AddBreak(lines, ReferenceCaption(L"This folder contains network"));
				AddBreak(lines, ReferenceCaption(L"connections for this computer, and a"));
				AddBreak(lines, ReferenceCaption(L"wizard to help you create a new"));
				AddBreak(lines, ReferenceCaption(L"connection."));

				AddParagraph(lines, ReferenceCaption(L"To create a new connection, click"));
				AddBreak(lines, ReferenceCaption(L"Make New Connection."));
				lines.back().bold = true;

				AddParagraph(lines, ReferenceCaption(L"To open a connection, click its icon."));

				AddParagraph(lines, ReferenceCaption(L"To access settings and components of"));
				AddBreak(lines, ReferenceCaption(L"a connection, right-click its icon and"));
				AddBreak(lines, ReferenceCaption(L"then click Properties."));

				AddParagraph(lines, ReferenceCaption(L"To identify your computer on the"));
				PaneLine networkIdentification;
				networkIdentification.linkPrefix = ReferenceCaption(L"network, click ");
				networkIdentification.text = ReferenceCaption(L"Network Identification");
				networkIdentification.linkSuffix = L".";
				networkIdentification.link = true;
				networkIdentification.linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
				networkIdentification.linkTarget =
					L"rundll32.exe shell32.dll,Control_RunDLL sysdm.cpl,,1";
				lines.push_back(std::move(networkIdentification));

				AddParagraph(lines, ReferenceCaption(L"To add additional networking"));
				PaneLine addNetwork;
				addNetwork.linkPrefix = ReferenceCaption(L"components, click ");
				addNetwork.text = ReferenceCaption(L"Add Network");
				addNetwork.link = true;
				addNetwork.linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
				addNetwork.linkTarget = L"rundll32.exe netshell.dll,HrLaunchNetworkOptionalComponents";
				lines.push_back(std::move(addNetwork));

				PaneLine addNetworkContinuation;
				addNetworkContinuation.text = ReferenceCaption(L"Components");
				addNetworkContinuation.linkSuffix = L".";
				addNetworkContinuation.link = true;
				addNetworkContinuation.linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
				addNetworkContinuation.linkTarget =
					L"rundll32.exe netshell.dll,HrLaunchNetworkOptionalComponents";
				lines.push_back(std::move(addNetworkContinuation));

				AddParagraph(lines, ReferenceCaption(L"Select an item to view its description."));
			}
			else if (selected > 1)
			{
				wchar_t buffer[64]{};
				StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
				AddBreak(lines, buffer);

				// dialup.htt opens one paragraph and lists names only when at most sixteen
				// items are selected. It never computes the generic folder total size.
				if (selected <= 16)
				{
					bool firstName = true;
					for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
					{
						CComPtr<FolderItem> item;
						CComVariant index(itemIndex);
						if (FAILED(items->Item(index, &item)) || !item)
						{
							continue;
						}

						CComBSTR itemName;
						if (FAILED(item->get_Name(&itemName)) || !itemName)
						{
							continue;
						}

						PaneLine nameLine;
						nameLine.text.assign(itemName, itemName.Length());
						nameLine.marginTop = firstName ? panemetrics::kParagraphTop : 0;
						lines.push_back(std::move(nameLine));
						firstName = false;
					}
				}
			}
			else
			{
				CComPtr<FolderItem> item;
				CComVariant index(0L);
				if (SUCCEEDED(items->Item(index, &item)) && item)
				{
					std::wstring name;
					CComBSTR itemName;
					if (SUCCEEDED(item->get_Name(&itemName)) && itemName)
					{
						name.assign(itemName, itemName.Length());
					}
					if (name.empty())
					{
						name = DetailOf(view, item, 0);
					}

					AddBreak(lines, name);
					if (!lines.empty())
					{
						lines.back().bold = true;
					}

					auto findDetailColumn = [&](PCWSTR expectedTitle) noexcept
					{
						for (int detailIndex = 1; detailIndex < 32; ++detailIndex)
						{
							const std::wstring title = DetailOf(view, nullptr, detailIndex);
							if (_wcsicmp(title.c_str(), expectedTitle) == 0)
							{
								return detailIndex;
							}
						}
						return -1;
					};

					// dialup.htt's Win2K ordinals were Type=1 and Status=2. Windows 10
					// orders this namespace as Status, Device Name, ..., Type, so locate the
					// same semantic columns and retain the historical output order.
					const struct
					{
						PCWSTR title;
						bool appendConnection;
					} details[] = {
						{ ReferenceCaption(L"Type"), true },
						{ ReferenceCaption(L"Status"), false },
					};

					for (const auto &detail : details)
					{
						const int detailIndex = findDetailColumn(detail.title);
						if (detailIndex < 0)
						{
							continue;
						}

						std::wstring data = DetailOf(view, item, detailIndex);
						if (data.empty())
						{
							continue;
						}
						if (detail.appendConnection)
						{
							data += ReferenceCaption(L" Connection");
						}

						// dialup.htt's FormatDetail uses character count, not measured width.
						const std::wstring title(detail.title);
						if (title.length() + data.length() > 32)
						{
							AddParagraph(lines, title + L":");
							AddBreak(lines, std::move(data));
						}
						else
						{
							AddParagraph(lines, title + L": " + data);
						}
					}

					const std::wstring infoTip = DetailOf(view, item, -1);
					if (!infoTip.empty() && infoTip != name)
					{
						AddParagraph(lines, infoTip);
					}

					if (name == ReferenceCaption(L"Make New Connection"))
					{
						AddParagraph(lines,
							ReferenceCaption(L"The Network Connection wizard helps you create a new connection so that your computer can have access to other computers and networks."));
					}
				}
			}

			m_pane.SetCapacityPie(-1);
			return lines;
		}

		// nethood.htt is the template for the whole NetHood namespace, so Entire Network uses this
		// same body once its barrier has come down — InitPanelText() and ShowLinks() are the ones
		// defined here, not folder.htt's.
		if (m_isMyNetworkPlaces || m_isEntireNetwork)
		{
			if (selected == 0)
			{
				// nethood.htt:10-18,115-120,171-179. Its no-selection panel is not the
				// generic folder.htt prompt/Message/three-link profile.
				AddBreak(lines,
					ReferenceCaption(L"Use this folder to open files and folders on other computers and to install network printers."));

				PaneLine setupFirst;
				setupFirst.text = ReferenceCaption(L"To set up networking on your");
				setupFirst.marginTop = panemetrics::kBodyLineHeight; // L_Intro_Text's <br><br>
				lines.push_back(std::move(setupFirst));

				PaneLine setupLink;
				setupLink.linkPrefix = ReferenceCaption(L"computer, click ");
				setupLink.text = ReferenceCaption(L"Network and Dial-up");
				setupLink.link = true;
				setupLink.csidl = CSIDL_CONNECTIONS;
				setupLink.status = ReferenceCaption(L"Connects to other computers, networks, and the Internet");
				setupLink.tooltip = setupLink.status;
				lines.push_back(std::move(setupLink));

				PaneLine setupLinkContinuation;
				setupLinkContinuation.text = ReferenceCaption(L"Connections");
				setupLinkContinuation.linkSuffix = L".";
				setupLinkContinuation.link = true;
				setupLinkContinuation.csidl = CSIDL_CONNECTIONS;
				setupLinkContinuation.status = ReferenceCaption(L"Connects to other computers, networks, and the Internet");
				setupLinkContinuation.tooltip = setupLinkContinuation.status;
				lines.push_back(std::move(setupLinkContinuation));

				PaneLine prompt;
				prompt.text = ReferenceCaption(L"Select an item to view its description.");
				prompt.marginTop = panemetrics::kParagraphTop;
				lines.push_back(std::move(prompt));

				PaneLine seeAlso;
				seeAlso.text = ReferenceCaption(L"See also:");
				seeAlso.marginTop = panemetrics::kParagraphTop;
				lines.push_back(std::move(seeAlso));

				AddLink(lines, ReferenceCaption(L"My Documents"), CSIDL_PERSONAL,
				        ReferenceCaption(L"My Documents contains your personal documents."));
				AddLink(lines, ReferenceCaption(L"My Computer"), CSIDL_DRIVES,
				        ReferenceCaption(L"My Computer contains your various local drives and mapped network drives."));
			}
			else if (selected > 1)
			{
				// nethood.htt:213-218 emits the count, one explicit blank line, and names
				// only when at most sixteen items are selected. It never computes size.
				wchar_t buffer[64]{};
				StringCchPrintfW(buffer, ARRAYSIZE(buffer), ReferenceCaption(L"%ld items selected."), selected);
				AddBreak(lines, buffer);
				if (selected <= 16)
				{
					bool firstName = true;
					for (long itemIndex = 0; itemIndex < selected; ++itemIndex)
					{
						CComPtr<FolderItem> item;
						CComVariant index(itemIndex);
						if (FAILED(items->Item(index, &item)) || !item)
						{
							continue;
						}

						CComBSTR itemName;
						if (FAILED(item->get_Name(&itemName)) || !itemName)
						{
							continue;
						}

						PaneLine nameLine;
						nameLine.text.assign(itemName, itemName.Length());
						nameLine.marginTop = firstName ? panemetrics::kBodyLineHeight : 0;
						lines.push_back(std::move(nameLine));
						firstName = false;
					}
				}
			}
			else
			{
				// nethood.htt:220-242: bold name, column-one comment, then a distinct
				// InfoTip. Do not reinterpret modern namespace columns as file metadata.
				CComPtr<FolderItem> item;
				CComVariant index(0L);
				if (SUCCEEDED(items->Item(index, &item)) && item)
				{
					std::wstring name = DetailOf(view, item, 0);
					if (name.empty())
					{
						CComBSTR itemName;
						if (SUCCEEDED(item->get_Name(&itemName)) && itemName)
						{
							name.assign(itemName, itemName.Length());
						}
					}

					const bool isUncName = name.rfind(L"\\\\", 0) == 0;
					PaneLine nameLine;
					nameLine.text = name;
					nameLine.bold = true;
					if (isUncName)
					{
						// The retail template makes a UNC name an anchor. Navigation remains
						// fail-closed because this branch does not approve arbitrary UNC roots.
						nameLine.link = true;
						nameLine.linkAction = PaneLine::LinkAction::DeniedLegacyCommand;
						nameLine.linkTarget = name;
						nameLine.status = name;
					}
					lines.push_back(std::move(nameLine));

					const std::wstring comment = DetailOf(view, item, 1);
					if (!comment.empty())
					{
						AddParagraph(lines, comment);
					}

					const std::wstring infoTip = DetailOf(view, item, -1);
					if (!infoTip.empty() && (comment.empty() || comment != infoTip) &&
					    (isUncName || name != infoTip))
					{
						AddParagraph(lines, infoTip);
					}
				}
			}

			m_pane.SetCapacityPie(-1);
			return lines;
		}

		if (selected == 1)
		{
			// ShowInfo(), single-item branch: bold name, then type, date and size.
			CComPtr<FolderItem> item;
			CComVariant index(0L);
			if (SUCCEEDED(items->Item(index, &item)) && item)
			{
				std::wstring path;
				CComBSTR itemPath;
				if (SUCCEEDED(item->get_Path(&itemPath)) && itemPath)
				{
					path.assign(itemPath, itemPath.Length());
				}

				std::wstring name = DetailOf(view, item, 0);   // column 0 is Name everywhere
				if (name.empty())
				{
					CComBSTR fallback;
					if (SUCCEEDED(item->get_Name(&fallback)) && fallback)
					{
						name.assign(fallback, fallback.Length());
					}
				}
				AddBreak(lines, name);
				if (!lines.empty())
				{
					lines.back().bold = true;
				}

				const std::wstring activeFolderPath = FolderPathOf(view);
				const bool folderPathIsSlow = !activeFolderPath.empty() &&
				                              PathIsSlowW(activeFolderPath.c_str(), -1);
				const bool isDrive = !path.empty() && PathIsRootW(path.c_str()) && !PathIsUNCW(path.c_str());

				if (isDrive)
				{
					// Capacity/Used/Free. The original read these off ThumbCtl
					// (Thumbnail.totalSpace/usedSpace/freeSpace); that control does not exist on
					// Windows 10, so they come from the volume directly. The strings are the
					// template's own: L_TotalSize_Text, L_UsedSpace_Text, L_FreeSpace_Text.
					AddBreak(lines, TypeNameOf(path));
					if (!folderPathIsSlow)
					{
						AddMessage(lines, CommentOf(item));
						AppendDriveInfo(lines, path, capacityPer1000);
					}
				}
				else
				{
					// The filesystem-folder columns ShowInfo() used: 2 is Type, 3 is Modified,
					// 1 is Size. These indices are per-folder — My Computer numbers its columns
					// differently — which is why they are only used off the drive path.
					//
					// Column 2 comes back empty for a shell object that is not a filesystem item
					// — This PC on the Desktop, for instance — where Windows 2000 showed
					// "System Folder". PKEY_ItemTypeText is the same value the shell puts in that
					// column, so it stands in when the ordinal yields nothing.
					std::wstring typeText = FormattedExtendedPropertyOf(item,L"System.ItemTypeText",PKEY_ItemTypeText);
					if (typeText.empty())
					{
						typeText = FormattedExtendedPropertyOf(item, L"System.ItemTypeText",
						                                       PKEY_ItemTypeText);
					}
					if (g_webOptions.load()->itemType) AddBreak(lines, typeText);
					if (!folderPathIsSlow)
					{
						const wchar_t *rejection = ThumbnailPathRejection(path);
						if (!rejection)
						{
							thumbnailPath = path;
						}
						else
						{
							JsonFields fields;
							fields.Hex(L"pathHash", ThumbnailPathHash(path)).Str(L"reason", rejection);
							EvidenceLog::Instance().Write(L"thumbnail.skipped", m_generation, fields);
						}

						/*
						 * The historical description belongs to the namespace, not to whichever
						 * folder happens to be open. Windows 2000 shows "Displays the files and
						 * folders on your computer" for My Computer both when you are inside it
						 * and when you have selected it on the Desktop.
						 *
						 * ProfileFolderComment is otherwise called with m_isMyComputer, which
						 * describes the *current folder*, so a selected namespace object never
						 * reached it and fell through to System.Comment -- Windows 10's own
						 * wording. Test the selected item as well.
						 */
						// The live value is kept separately from the one displayed: the Comments
						// detail below is System.Comment, so it must be suppressed against
						// System.Comment even when the box shows the historical string instead.
						const std::wstring liveComment = CommentOf(item);
						std::wstring shownComment;
						{
							const bool selectedMyComputer =
								!path.empty() &&
								StrStrIW(path.c_str(),
								         L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}") != nullptr;
							bool selectedMyDocuments = false;
							if (!selectedMyComputer && !path.empty())
							{
								wchar_t documents[MAX_PATH]{};
								selectedMyDocuments =
									SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr,
									                           SHGFP_TYPE_CURRENT, documents)) &&
									StrCmpIW(path.c_str(), documents) == 0;
							}
							if (selectedMyComputer || selectedMyDocuments)
							{
								shownComment = ProfileFolderComment(m_visualProfile,
								                                    selectedMyComputer,
								                                    selectedMyDocuments);
							}
						}
						if (shownComment.empty())
						{
							shownComment = liveComment;
						}
						AddMessage(lines, shownComment);

						const std::wstring dateValue = FormattedExtendedPropertyOf(item,L"System.DateModified",PKEY_DateModified);
						if (g_webOptions.load()->metadata && !dateValue.empty())
						{
							AddParagraph(lines, ReferenceCaption(L"Modified: ") + dateValue);
						}

						CComPtr<IShellItem> selectedShellItem;
						CComQIPtr<IFolderView2> folderView(m_spView);
						CComPtr<IShellItemArray> selection;
						if (folderView && SUCCEEDED(folderView->GetSelection(FALSE, &selection)) && selection)
						{
							selection->GetItemAt(0, &selectedShellItem);
						}
						CComQIPtr<IShellItem2> shellItem(selectedShellItem);

                        const std::wstring sizeValue=ItemSizeText(shellItem,item,view);
                        if(!sizeValue.empty()) AddParagraph(lines,ReferenceCaption(L"Size: ")+sizeValue);

						AddExtraFileDetails(lines, item, shellItem, liveComment);
					}
				}
			}
		}
        else if(selected>1) {
            wchar_t label[64]{};
            StringCchPrintfW(label,ARRAYSIZE(label),ReferenceCaption(L"%ld items selected."),selected);
            AddBreak(lines,label);
            if(selected<=100) {
                ULONGLONG total=0;
                std::vector<std::wstring> names;
                for(long index=0;index<selected;++index) {
                    CComPtr<FolderItem> item;
                    if(FAILED(items->Item(CComVariant(index),&item)) || !item) continue;
                    total+=SelectionBytesAt(index,item);
                    if(selected<=16) {
                        CComBSTR name;
                        if(SUCCEEDED(item->get_Name(&name)) && name) names.emplace_back(name,name.Length());
                    }
                }
                if(total) AddParagraph(lines,ReferenceCaption(L"Total File Size: ")+FormatBytes(total));
                for(size_t index=0;index<names.size();++index) {
                    PaneLine line; line.text=std::move(names[index]);
                    line.marginTop=index ? 0 : panemetrics::kParagraphTop+panemetrics::kBodyLineHeight;
                    lines.push_back(std::move(line));
                }
            }
        }
		else
		{
			// NoneSelected(): the prompt, the folder's description, then the See also list.
			AddBreak(lines, ReferenceCaption(L"Select an item to view its description."));

            std::wstring comment=ProfileFolderComment(m_visualProfile,m_isMyComputer,m_isMyDocuments,m_isMyPictures);
            if(comment.empty()) comment=FolderCommentOf(view);
			const bool infoEndsInMessage = !comment.empty();
			AddMessage(lines, std::move(comment));

			// NoneSelected() passes the current folder's Self to DealWithDriveInfo. A local
			// root therefore gets the same capacity block/pie as a selected drive item.
			bool previewBeforeLinks = false;
			const std::wstring folderPath = FolderPathOf(view);
			if (!folderPath.empty() && PathIsRootW(folderPath.c_str()) && !PathIsUNCW(folderPath.c_str()))
			{
				previewBeforeLinks = AppendDriveInfo(lines, folderPath, capacityPer1000);
			}

			PaneLine seeAlso;
			seeAlso.text = ReferenceCaption(L"See also:");
			seeAlso.previewBefore = previewBeforeLinks;
			// folder.htt places an explicit <br> after Info. When Info ends in the block-level
			// Message box, that break owns a full 13px line. After a visible Thumbnail object,
			// Links starts with its own 12px paragraph margin.
			seeAlso.marginTop = previewBeforeLinks ? panemetrics::kParagraphTop
				: (infoEndsInMessage ? panemetrics::kBodyLineHeight : panemetrics::kParagraphTop);
			lines.push_back(std::move(seeAlso));

            const bool me=m_visualProfile==WebViewVisualProfile::WindowsME;
            const struct { bool skip; int folder; PCWSTR name,description; } destinations[]={
                {m_isMyDocuments,CSIDL_PERSONAL,ReferenceCaption(L"My Documents"),ReferenceCaption(L"My Documents contains your personal documents.")},
                {m_isMyNetworkPlaces,CSIDL_NETWORK,ReferenceCaption(L"My Network Places"),
                    ReferenceCaption(L"My Network Places contains shortcuts to various locations on the corporate network and the Internet.")},
                {false,m_isMyComputer ? CSIDL_CONNECTIONS : CSIDL_DRIVES,
                    m_isMyComputer ? (me ? ReferenceCaption(L"Dial-Up Networking") : ReferenceCaption(L"Network and Dial-up Connections")) : ReferenceCaption(L"My Computer"),
                    m_isMyComputer ? ReferenceCaption(L"Connects to other computers, networks and the Internet") :
                        ReferenceCaption(L"My Computer contains your various local drive and mapped network drives.")}
            };
            for(const auto& destination:destinations) if(!destination.skip)
                AddLink(lines,destination.name,destination.folder,destination.description);

		}

		m_pane.SetCapacityPie(capacityPer1000);
		return lines;
	}


} }

namespace ce { namespace win2kwebview {
static std::wstring DisplayName(IShellItem* item,SIGDN kind) {
    PWSTR value=nullptr;
    if (!item || FAILED(item->GetDisplayName(kind,&value))) return {};
    std::wstring text=value ? value : L"";
    CoTaskMemFree(value); return text;
}
static bool SamePath(PCWSTR a,PCWSTR b) {
    return CompareStringOrdinal(a,-1,b,-1,TRUE)==CSTR_EQUAL;
}
static bool IsCsidl(IShellItem* item,int csidl) {
    PIDLIST_ABSOLUTE current=nullptr,expected=nullptr;
    bool same=SUCCEEDED(SHGetIDListFromObject(item,&current)) &&
              SUCCEEDED(SHGetFolderLocation(nullptr,csidl,nullptr,0,&expected)) &&
              ILIsEqual(current,expected);
    CoTaskMemFree(current); CoTaskMemFree(expected); return same;
}
static bool IsKnownFolderPath(IShellItem* item,REFKNOWNFOLDERID folder) {
    auto path=DisplayName(item,SIGDN_FILESYSPATH);
    if(path.empty()) return false;
    PWSTR value=nullptr;
    const HRESULT status=SHGetKnownFolderPath(folder,KF_FLAG_DONT_VERIFY,nullptr,&value);
    std::wstring expected=SUCCEEDED(status) && value ? value : L"";
    CoTaskMemFree(value);
    if(expected.empty()) return false;
    for(auto* text:{&path,&expected}) {
        std::replace(text->begin(),text->end(),L'/',L'\\');
        while(text->size()>3 && text->back()==L'\\') text->pop_back();
    }
    return SamePath(path.c_str(),expected.c_str());
}
static bool IsDocumentsFolder(IShellItem* item) {
    // The same user folder can have distinct filesystem and namespace PIDLs.
    // Use the configured path, including redirected Documents directories.
    return item && (IsCsidl(item,CSIDL_PERSONAL) || IsKnownFolderPath(item,FOLDERID_Documents));
}
static bool IsDesktopFolder(IShellItem* item) {
    if(!item) return false;
    if(IsCsidl(item,CSIDL_DESKTOP) || IsCsidl(item,CSIDL_DESKTOPDIRECTORY) ||
       IsCsidl(item,CSIDL_COMMON_DESKTOPDIRECTORY)) return true;
    // This PC and redirected-folder aliases can use a different PIDL for the
    // same Desktop directory. Recognize the known-folder path before accepting
    // the view's remembered Pictures type.
    return IsKnownFolderPath(item,FOLDERID_Desktop) || IsKnownFolderPath(item,FOLDERID_PublicDesktop);
}
ReferenceContent::~ReferenceContent() {
    CancelPreview(lifetime,true);
    CancelPrinterInfo(printerLifetime,true);
    ClearPreview(); DestroyViewer();
}
void ReferenceContent::ClearPreview() {
    zoom.SetBitmap(nullptr); detached.SetBitmap(nullptr);
    m_pane.SetThumbnail(nullptr); preview.reset();
}
void ReferenceContent::SuspendView() {
    // Keep the last complete picture until the replacement view is ready,
    // but release the retired Shell objects and reject all actions on it.
    viewReady=false; forceRefresh=true;
    CancelPrinterInfo(printerLifetime); CancelPreview(lifetime);
    pendingSince=0; printable=false;
    navigate={}; isActiveView={}; m_selection.Release(); m_spView.Release();
    zoom.SetInteractionEnabled(false);
    if(details.Window()) EnableWindow(details.Window(),FALSE);
    if(toolbar) EnableWindow(toolbar,FALSE);
    detached.Destroy();
}
void ReferenceContent::ResetView() {
    viewReady=false;
    CancelPrinterInfo(printerLifetime);
    CancelPreview(lifetime); ClearPreview(); DestroyViewer();
    previewPath.clear(); folderKey.clear(); signature.clear();
    pendingSince=0; selectedCount=0; fullResolution=false; printable=false; forceRefresh=true;
    m_isDesktop=false; m_isMyPictures=false; m_isImgViewTemplate=false;
    m_isEntireNetwork=false; m_isMeDriveRoot=false; m_isWin98SystemFolder=false;
    m_showFiles=true; bannerHeight=0;
    m_pane.SetProfile(PaneProfile::Standard); m_pane.SetCapacityPie(-1);
    m_pane.SetBarricade(BarricadeMode::None); m_pane.SetLines({});
    navigate={}; isActiveView={}; m_selection.Release(); m_spView.Release();
}
void ReferenceContent::PreviewStatus(ImgPreviewState state,SIZE size) {
    previewState=state;
    m_pane.SetImgPreview(state,size);
    PCWSTR status=L"";
    switch(state) {
        case ImgPreviewState::NoSelection:status=L"No file selected.";break;
        case ImgPreviewState::Loading:status=L"Generating preview...";break;
        case ImgPreviewState::Multiple:status=L"Multiple files selected.";break;
        case ImgPreviewState::Failed:status=L"No preview available.";break;
        case ImgPreviewState::Image:break;
    }
    zoom.SetStatusText(ReferenceCaption(status));
    detached.SetStatusText(ReferenceCaption(status));
}
void ReferenceContent::Refresh(IShellView* view,IShellItem* folder,HWND hwnd,bool force) {
    if(!view || !folder || !IsWindow(hwnd)) { ResetView(); return; }
    ViewerPaintTransaction held(details.Window(),zoom.Window(),toolbar);
    viewReady=true;
    window=hwnd;
    { std::lock_guard lock(lifetime->mutex); lifetime->window=hwnd; }
    const auto options=g_webOptions.load();
    const auto key=DisplayName(folder,SIGDN_DESKTOPABSOLUTEPARSING);
    m_pane.SetImgDetailsOptions(options->compactImageHeader,options->scrollImageDetails);
    const bool navigated=key!=folderKey || m_spView.p!=view;
    if(navigated) CancelPrinterInfo(printerLifetime);
    m_spView=view;
    CComQIPtr<IFolderView2> folderView(view);
    CComPtr<IShellItemArray> selection;
    selectedCount=0;
    if (folderView && SUCCEEDED(folderView->GetSelection(FALSE,&selection)) && selection)
        selection->GetCount(&selectedCount);
    m_selection=selection;
    std::wstring identity=key+L"\n"+std::to_wstring(selectedCount)+L"\n"+std::to_wstring(options->profile);
    for (DWORD i=0;i<min<DWORD,DWORD>(selectedCount,100);++i) {
        CComPtr<IShellItem> selected;
        if (SUCCEEDED(selection->GetItemAt(i,&selected))) {
            identity+=L"\n"+DisplayName(selected,SIGDN_DESKTOPABSOLUTEPARSING);
            CComQIPtr<IShellItem2> properties(selected);
            FILETIME time{}; ULONGLONG size=0;
            if (properties) { properties->GetFileTime(PKEY_DateModified,&time); properties->GetUInt64(PKEY_Size,&size); }
            identity+=L":"+std::to_wstring(size)+L":"+std::to_wstring(time.dwLowDateTime)+L":"+std::to_wstring(time.dwHighDateTime);
        }
    }
    const ULONGLONG now=GetTickCount64();
    if (!force && !forceRefresh && !navigated && identity==signature &&
        !(m_isPrinters && options->printerRefresh && now-refreshedAt>=5000)) {
        m_pane.SetThumbnailPending(options->preview && pendingSince && now-pendingSince>1000);
        return;
    }
    if(navigated || identity!=signature) details.ResetScroll();
    forceRefresh=false; signature=std::move(identity); refreshedAt=now;
    if (navigated || force) {
        folderKey=key; ++m_generation;
        m_visualProfile=options->profile==1 ? WebViewVisualProfile::Windows98 :
                        options->profile==2 ? WebViewVisualProfile::WindowsME : WebViewVisualProfile::Windows2000;
        m_pane.SetVisualProfile(m_visualProfile);
        m_pane.SetFontSmoothingDisabled(options->disableSmoothing);
        m_pane.SetDimensionsVisible(options->metadata);
        m_pane.EnsureResources();
        m_isDesktop=IsDesktopFolder(folder);
        m_isMyComputer=options->specialFolders && IsCsidl(folder,CSIDL_DRIVES);
        m_isMyDocuments=options->specialFolders && IsDocumentsFolder(folder);
        m_isMyNetworkPlaces=options->specialFolders && IsCsidl(folder,CSIDL_NETWORK);
        m_isRecycleBin=options->specialFolders && IsCsidl(folder,CSIDL_BITBUCKET);
        m_isPrinters=options->specialFolders && IsCsidl(folder,CSIDL_PRINTERS);
        m_isControlPanel=options->specialFolders && IsCsidl(folder,CSIDL_CONTROLS);
        m_isNetworkConnections=options->specialFolders && IsCsidl(folder,CSIDL_CONNECTIONS);
        const auto path=DisplayName(folder,SIGDN_FILESYSPATH);
        const auto name=DisplayName(folder,SIGDN_NORMALDISPLAY);
        m_isEntireNetwork=!m_isDesktop && options->specialFolders && path.empty() &&
            (name==L"Entire Network" || name==L"Вся сеть");
        m_isMyPictures=!m_isDesktop && IsCsidl(folder,CSIDL_MYPICTURES);
        m_isImgViewTemplate=!m_isDesktop && !path.empty() && FolderWebViewTemplate(path)==L"imgview.htt";
        // Same shell classification query as the reference; no ATL dependency.
        struct FolderType : IUnknown {
            virtual HRESULT STDMETHODCALLTYPE GetFolderType(FOLDERTYPEID*)=0;
        };
        const GUID folderTypeIID={0x053b4a86,0x0dc9,0x40a3,{0xb7,0xed,0xbc,0x6a,0x2e,0x95,0x1f,0x48}};
        CComPtr<FolderType> folderType;
        if (!m_isDesktop && SUCCEEDED(view->QueryInterface(folderTypeIID,reinterpret_cast<void**>(&folderType))) && folderType) {
            FOLDERTYPEID type{};
            if (SUCCEEDED(folderType->GetFolderType(&type)) && IsEqualGUID(type,FOLDERTYPEID_Pictures)) m_isMyPictures=true;
        }
        m_directoryService=false;
        if (m_isEntireNetwork) {
            CComPtr<IDispatch> automation;
            if (SUCCEEDED(GetAutomation(&automation))) {
                auto shell=ShellAutomation(automation);
                CComVariant available;
                if (shell && SUCCEEDED(shell->GetSystemInformation(CComBSTR(L"DirectoryServiceAvailable"),&available))) {
                    VARIANT boolean{}; VariantInit(&boolean);
                    if (SUCCEEDED(VariantChangeType(&boolean,&available,0,VT_BOOL))) m_directoryService=boolean.boolVal!=VARIANT_FALSE;
                    VariantClear(&boolean);
                }
            }
        }
        wchar_t windows[MAX_PATH]{},programFiles[MAX_PATH]{};
        GetWindowsDirectoryW(windows,ARRAYSIZE(windows));
        SHGetFolderPathW(nullptr,CSIDL_PROGRAM_FILES,nullptr,0,programFiles);
        m_isWin98SystemFolder=!m_isDesktop && options->specialFolders && options->profile==1 &&
            (SamePath(path.c_str(),windows) || SamePath(path.c_str(),(std::wstring(windows)+L"\\System").c_str()) ||
             SamePath(path.c_str(),(std::wstring(windows)+L"\\System32").c_str()));
        m_isWin98ProgramFiles=!m_isDesktop && options->specialFolders && options->profile==1 && SamePath(path.c_str(),programFiles);
        wchar_t root[MAX_PATH]{}; GetVolumePathNameW(windows,root,ARRAYSIZE(root));
        m_isMeDriveRoot=!m_isDesktop && options->specialFolders && options->profile==2 && SamePath(path.c_str(),root);
        m_win98Template=m_isMyComputer ? Win98TemplateKind::MyComputer :
            m_isRecycleBin ? Win98TemplateKind::RecycleBin : m_isPrinters ? Win98TemplateKind::Printers :
            m_isControlPanel ? Win98TemplateKind::ControlPanel : m_isNetworkConnections ? Win98TemplateKind::DialUpNetworking :
            m_isMyNetworkPlaces ? Win98TemplateKind::NetworkNeighborhood :
            m_isWin98SystemFolder ? Win98TemplateKind::SystemFolderBarricade :
            m_isWin98ProgramFiles ? Win98TemplateKind::SystemFolderWarnOnly : Win98TemplateKind::Folder;
        m_showFiles=!options->barricades || !(m_isEntireNetwork||m_isMeDriveRoot||m_isWin98SystemFolder);
        CancelPreview(lifetime); ClearPreview(); previewPath.clear();
    }
    if (!options->barricades) m_showFiles=true;
    m_pane.SetProfile(UsesImgViewProfile() ? PaneProfile::ImgView : PaneProfile::Standard);
    std::wstring path;
    auto lines=BuildPaneLines(path);
    FilterLines(lines);
    if (!options->preview || !options->classicLayout || (selectedCount>1 && !options->multiSelection)) path.clear();
    const bool full=UsesImgViewProfile();
    if (path!=previewPath || full!=fullResolution || force) {
        CancelPreview(lifetime); ClearPreview(); pendingSince=0;
        previewPath=path; fullResolution=full;
        if (!path.empty()) {
            PreviewStatus(ImgPreviewState::Loading);
            pendingSince=now;
            if (!QueuePreview(lifetime,path,{options->profile==1 ? 160 : 120,options->profile==1 ? 160 : 120},full)) {
                pendingSince=0; PreviewStatus(ImgPreviewState::Failed);
            }
        } else PreviewStatus(selectedCount>1 ? ImgPreviewState::Multiple : ImgPreviewState::NoSelection);
    } else if (preview && preview->bitmap) PreviewStatus(ImgPreviewState::Image,preview->sourceSize);
    m_pane.SetLines(std::move(lines));
    if (full) EnsureViewer(); else DestroyViewer();
    printable=full && options->print && !previewPath.empty() &&
              CanonicalVerb(SVGIO_SELECTION,L"print",false)==S_OK;
    detached.SetPrintable(printable); detached.SetSourcePath(previewPath);
    if (toolbar) SendMessageW(toolbar,TB_ENABLEBUTTON,ID_WIN2K_IMGVIEW_PRINT,MAKELONG(printable,0));
    UpdateViewerButtons();
    InvalidateRect(window,nullptr,FALSE);
}
void ReferenceContent::AcceptPreview() {
    std::unique_ptr<PreviewResult> result;
    { std::lock_guard lock(lifetime->mutex);
      if (lifetime->ready && lifetime->ready->serial==lifetime->serial) result=std::move(lifetime->ready); }
    if (!result) return;
    pendingSince=0; m_pane.SetThumbnailPending(false);
    if (!SelectionMatches() || !g_webOptions.load()->preview) return;
    ClearPreview(); preview=std::move(result);
    const bool ready=SUCCEEDED(preview->hr) && preview->bitmap;
    m_pane.SetThumbnail(ready && !fullResolution ? preview->bitmap : nullptr);
    zoom.SetBitmap(ready && fullResolution ? preview->bitmap : nullptr);
    detached.SetBitmap(ready && fullResolution ? preview->bitmap : nullptr);
    PreviewStatus(ready ? ImgPreviewState::Image : ImgPreviewState::Failed,preview->sourceSize);
    UpdateViewerButtons();
    InvalidateRect(window,nullptr,FALSE);
}
bool ReferenceContent::SelectionMatches() const {
    if(!viewReady) return false;
    if(isActiveView && !isActiveView(m_spView)) return false;
    CComQIPtr<IFolderView2> view(m_spView);
    CComPtr<IShellItemArray> selection; DWORD count=0; CComPtr<IShellItem> item;
    return view && SUCCEEDED(view->GetSelection(FALSE,&selection)) && selection &&
           SUCCEEDED(selection->GetCount(&count)) && count==1 &&
           SUCCEEDED(selection->GetItemAt(0,&item)) &&
           SamePath(DisplayName(item,SIGDN_FILESYSPATH).c_str(),previewPath.c_str());
}
HRESULT ReferenceContent::CanonicalVerb(UINT items,PCWSTR verb,bool invoke,HWND owner) {
    if(!viewReady) return S_FALSE;
    if (!m_spView) return E_FAIL;
    if(isActiveView && !isActiveView(m_spView)) return S_FALSE;
    if (items==SVGIO_SELECTION) {
        CComQIPtr<IFolderView2> view(m_spView); CComPtr<IShellItemArray> selection; DWORD count=0;
        if (!view || FAILED(view->GetSelection(FALSE,&selection)) || !selection ||
            FAILED(selection->GetCount(&count)) || !count) return S_FALSE;
        if ((SamePath(verb,L"print") || SamePath(verb,L"properties")) && count!=1) return S_FALSE;
        if (SamePath(verb,L"print") && !SelectionMatches()) return S_FALSE;
    }
    CComPtr<IContextMenu> menu;
    HRESULT hr=m_spView->GetItemObject(items,IID_IContextMenu,reinterpret_cast<void**>(&menu));
    if (FAILED(hr) || !menu) return FAILED(hr) ? hr : E_NOINTERFACE;
    HMENU popup=CreatePopupMenu(); if (!popup) return E_OUTOFMEMORY;
    hr=menu->QueryContextMenu(popup,0,1,0x7fff,CMF_NORMAL);
    bool found=false;
    if (SUCCEEDED(hr)) {
        const UINT count=HRESULT_CODE(hr);
        hr=S_FALSE;
        for(UINT offset=0;offset<count;++offset) {
            wchar_t wide[128]{};
            auto result=menu->GetCommandString(offset,GCS_VERBW,nullptr,reinterpret_cast<LPSTR>(wide),ARRAYSIZE(wide));
            if (FAILED(result) || !wide[0]) {
                char ansi[128]{};
                if (SUCCEEDED(menu->GetCommandString(offset,GCS_VERBA,nullptr,ansi,ARRAYSIZE(ansi))))
                    MultiByteToWideChar(CP_ACP,0,ansi,-1,wide,ARRAYSIZE(wide));
            }
            if (!SamePath(wide,verb)) continue;
            found=true; hr=S_OK;
            if (invoke) {
                CMINVOKECOMMANDINFOEX command{};
                command.cbSize=sizeof(command); command.fMask=CMIC_MASK_UNICODE;
                command.hwnd=owner ? owner : window; command.nShow=SW_SHOWNORMAL;
                command.lpVerb=MAKEINTRESOURCEA(offset); command.lpVerbW=MAKEINTRESOURCEW(offset);
                hr=menu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&command));
            }
            break;
        }
    }
    DestroyMenu(popup); return found ? hr : FAILED(hr) ? hr : S_FALSE;
}
void ReferenceContent::Activate(int index) {
    if(!viewReady) return;
    if(isActiveView && !isActiveView(m_spView)) return;
    const auto* selected=m_pane.Line(index); if (!selected) return;
    const PaneLine line=*selected; // nested Shell calls can trigger a refresh
    const auto options=g_webOptions.load();
    const HWND targetWindow=window;
    const auto life=lifetime;
    const auto browse=navigate;
    switch(line.linkAction) {
        case PaneLine::LinkAction::BrowseCsidl: {
            PIDLIST_ABSOLUTE pidl=nullptr;
            if (SUCCEEDED(SHGetFolderLocation(nullptr,line.csidl,nullptr,0,&pidl))) {
                CComPtr<IServiceProvider> services; CComPtr<IShellBrowser> browser;
                if(browse) browse(pidl);
                else if (m_spView && SUCCEEDED(m_spView->QueryInterface(IID_PPV_ARGS(&services))) && services &&
                    SUCCEEDED(services->QueryService(SID_STopLevelBrowser,IID_PPV_ARGS(&browser))))
                    browser->BrowseObject(pidl,SBSP_SAMEBROWSER|SBSP_ABSOLUTE);
                CoTaskMemFree(pidl);
            } break;
        }
        case PaneLine::LinkAction::RecycleEmpty:
        case PaneLine::LinkAction::RecycleRestoreAll:
        case PaneLine::LinkAction::RecycleRestoreSelection: {
            if (!options->recycleActions || !m_isRecycleBin) return;
            CComQIPtr<IFolderView> view(m_spView); CComPtr<IPersistFolder2> folder; PIDLIST_ABSOLUTE pidl=nullptr;
            CComPtr<IShellItem> item;
            if (!view || FAILED(view->GetFolder(IID_PPV_ARGS(&folder))) || !folder ||
                FAILED(folder->GetCurFolder(&pidl))) return;
            SHCreateItemFromIDList(pidl,IID_PPV_ARGS(&item)); CoTaskMemFree(pidl);
            if (!item || !IsCsidl(item,CSIDL_BITBUCKET)) return;
            if (line.linkAction==PaneLine::LinkAction::RecycleEmpty)
                SHEmptyRecycleBinW(window,nullptr,0); // keep the Shell's confirmation
            else CanonicalVerb(line.linkAction==PaneLine::LinkAction::RecycleRestoreAll ? SVGIO_ALLVIEW : SVGIO_SELECTION,L"undelete",true);
            { std::lock_guard lock(life->mutex); if(life->alive) forceRefresh=true; } break;
        }
        case PaneLine::LinkAction::ShowEntireContents:
        case PaneLine::LinkAction::ShowSystemFolderFiles:
            if (options->barricades) { m_showFiles=true; forceRefresh=true; } break;
        case PaneLine::LinkAction::HideDriveContents:
            if (options->barricades) { m_showFiles=false; forceRefresh=true; } break;
        case PaneLine::LinkAction::ItemProperties:
            if (options->attributes) CanonicalVerb(SVGIO_SELECTION,L"properties",true); break;
        case PaneLine::LinkAction::FindComputer:
        case PaneLine::LinkAction::FindPrinter:
        case PaneLine::LinkAction::FindFiles: {
            CComPtr<IShellDispatch2> shell; CComPtr<IDispatch> automation;
            CComQIPtr<IShellFolderViewDual> view;
            if (SUCCEEDED(GetAutomation(&automation))) view=automation;
            CComPtr<IDispatch> application;
            if (view && SUCCEEDED(view->get_Application(&application)))
                application->QueryInterface(IID_PPV_ARGS(&shell));
            if (shell) {
                if (line.linkAction==PaneLine::LinkAction::FindComputer) shell->FindComputer();
                else if (line.linkAction==PaneLine::LinkAction::FindPrinter) shell->FindPrinter(nullptr,nullptr,nullptr);
                else shell->FindFiles();
            } break;
        }
        default: break; // the DLL also denies retired commands / external links
    }
    InvalidateRect(targetWindow,nullptr,FALSE);
}
static LRESULT CALLBACK ReferenceToolbarProc(HWND toolbar,UINT message,WPARAM wParam,LPARAM lParam,
                                             UINT_PTR id,DWORD_PTR data) {
    if(PreviewToolbarPaintMessage(toolbar,message,wParam)) return 0;
    if(message==WM_ERASEBKGND) {
        FillToolbarBackground(toolbar,reinterpret_cast<HDC>(wParam)); return TRUE;
    }
    if(message==WM_NCDESTROY) RemoveWindowSubclass(toolbar,ReferenceToolbarProc,id);
    if(message==WM_KEYDOWN) {
        const UINT command=ImgViewAcceleratorCommand(wParam);
        if(command) { reinterpret_cast<ReferenceContent*>(data)->Command(command,toolbar); return 0; }
    }
    return DefSubclassProc(toolbar,message,wParam,lParam);
}
void ReferenceContent::EnsureViewer() {
    if (toolbar) return;
    const auto options=g_webOptions.load();
    if (!details.Create(window,&m_pane)) return;
    if (!zoom.Create(window)) { details.Destroy(); return; }
    toolbar=CreateWindowExW(0,TOOLBARCLASSNAMEW,nullptr,
        WS_CHILD|WS_CLIPSIBLINGS|CCS_NODIVIDER|CCS_NOPARENTALIGN|CCS_NORESIZE|TBSTYLE_FLAT|
            (options->tooltips ? TBSTYLE_TOOLTIPS : 0),
        0,0,0,0,window,nullptr,ReferenceModule(),nullptr);
    if (!toolbar) { zoom.Destroy(); details.Destroy(); return; }
    SetWindowSubclass(toolbar,ReferenceToolbarProc,1,reinterpret_cast<DWORD_PTR>(this));
    SetWindowTheme(toolbar,L"",L"");
    SendMessageW(toolbar,CCM_SETVERSION,5,0);
    SendMessageW(toolbar,CCM_SETUNICODEFORMAT,TRUE,0);
    SendMessageW(toolbar,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
    SendMessageW(toolbar,TB_SETEXTENDEDSTYLE,0,TBSTYLE_EX_MIXEDBUTTONS);
    SendMessageW(toolbar,TB_SETMAXTEXTROWS,0,0);
    const UINT dpi=GetDpiForWindow(window);
    const int cell=MulDiv(24,dpi ? dpi : 96,96);
    SendMessageW(toolbar,TB_SETBITMAPSIZE,0,MAKELONG(cell,cell));
    if (!CreateReferenceToolbarImages(dpi,images,hotImages)) { DestroyViewer(); return; }
    SendMessageW(toolbar,TB_SETIMAGELIST,0,reinterpret_cast<LPARAM>(images));
    SendMessageW(toolbar,TB_SETHOTIMAGELIST,0,reinterpret_cast<LPARAM>(hotImages));
    auto button=[&](int image,UINT command,PCWSTR label) {
        TBBUTTON entry{}; entry.iBitmap=image; entry.idCommand=command;
        entry.fsState=TBSTATE_ENABLED; entry.fsStyle=BTNS_BUTTON;
        entry.iString=reinterpret_cast<INT_PTR>(ReferenceCaption(label));
        SendMessageW(toolbar,TB_ADDBUTTONS,1,reinterpret_cast<LPARAM>(&entry));
    };
    if (options->zoom) {
        button(0,ID_WIN2K_IMGVIEW_ZOOMIN,L"Zoom In"); button(1,ID_WIN2K_IMGVIEW_ZOOMOUT,L"Zoom Out");
        TBBUTTON separator{}; separator.fsStyle=BTNS_SEP;
        SendMessageW(toolbar,TB_ADDBUTTONS,1,reinterpret_cast<LPARAM>(&separator));
        button(2,ID_WIN2K_IMGVIEW_ACTUALSIZE,L"Actual Size"); button(3,ID_WIN2K_IMGVIEW_BESTFIT,L"Best Fit");
    }
    if (options->detached) button(4,ID_WIN2K_IMGVIEW_FULLSCREEN,L"Full Screen");
    if (options->print) button(5,ID_WIN2K_IMGVIEW_PRINT,L"Print");
    SendMessageW(toolbar,TB_SETBUTTONWIDTH,0,MAKELONG(cell,cell));
    SendMessageW(toolbar,TB_AUTOSIZE,0,0);
    m_pane.SetImgToolbarHeight(30);
    zoom.SetBitmap(preview ? preview->bitmap : nullptr);
    zoom.SetInteractionEnabled(options->zoom);
    zoom.SetMode(options->zoom ? ImgViewZoomWindow::Mode::ZoomIn : ImgViewZoomWindow::Mode::Pan);
}
void ReferenceContent::UpdateViewerButtons() {
    if(!toolbar) return;
    const auto options=g_webOptions.load();
    const bool selected=viewReady && selectedCount==1 && !previewPath.empty();
    const bool ready=selected && preview && preview->bitmap && fullResolution;
    for(UINT command=ID_WIN2K_IMGVIEW_ZOOMIN;command<=ID_WIN2K_IMGVIEW_BESTFIT;++command)
        SendMessageW(toolbar,TB_ENABLEBUTTON,command,MAKELONG(ready && options->zoom,0));
    SendMessageW(toolbar,TB_ENABLEBUTTON,ID_WIN2K_IMGVIEW_FULLSCREEN,
        MAKELONG(options->detached && (ready || (selected && !options->viewerExecutable.empty())),0));
}
void ReferenceContent::DestroyViewer() {
    detached.Destroy(); zoom.Destroy(); details.Destroy();
    if (toolbar) DestroyWindow(std::exchange(toolbar,nullptr));
    if (images) ImageList_Destroy(std::exchange(images,nullptr));
    if (hotImages) ImageList_Destroy(std::exchange(hotImages,nullptr));
    m_pane.SetImgToolbarHeight(0);
}
static bool PositionReferenceViewerChild(HWND child,const RECT& bounds) {
    RECT previous{}; GetWindowRect(child,&previous);
    MapWindowPoints(nullptr,GetParent(child),reinterpret_cast<POINT*>(&previous),2);
    if(EqualRect(&previous,&bounds)) return false;
    return SetWindowPos(child,nullptr,bounds.left,bounds.top,std::max(0L,bounds.right-bounds.left),
        std::max(0L,bounds.bottom-bounds.top),SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOCOPYBITS)!=FALSE;
}
void ReferenceContent::LayoutViewer(const RECT& client,int dpi,bool visible) {
    visible=visible && UsesImgViewProfile() && !bannerHeight && CurrentBarricade()==BarricadeMode::None;
    if(!visible || !toolbar) {
        if(details.Window()) ShowWindow(details.Window(),SW_HIDE);
        if(zoom.Window()) ShowWindow(zoom.Window(),SW_HIDE);
        if(toolbar) ShowWindow(toolbar,SW_HIDE);
        return;
    }
    if(dpi<=0) dpi=96;
    RECT logical={MulDiv(client.left,96,dpi),MulDiv(client.top,96,dpi),
        MulDiv(client.right,96,dpi),MulDiv(client.bottom,96,dpi)};
    RECT canvas=m_pane.ImgPreviewRect(logical);
    RECT pixels={MulDiv(canvas.left,dpi,96),MulDiv(canvas.top,dpi,96),
                 MulDiv(canvas.right,dpi,96),MulDiv(canvas.bottom,dpi,96)};
    pixels.left=std::clamp(pixels.left,client.left,client.right);
    pixels.right=std::clamp(pixels.right,pixels.left,client.right);
    pixels.top=std::clamp(pixels.top,client.top,client.bottom);
    pixels.bottom=std::clamp(pixels.bottom,pixels.top,client.bottom);
    const int split=logical.top+(logical.bottom-logical.top+1)/2;
    const int frameTop=MulDiv(split+panemetrics::kImgFrameInset,dpi,96);
    const int toolbarTop=std::min<int>(pixels.top,std::max(frameTop,static_cast<int>(pixels.top)-MulDiv(30,dpi,96)));
    const RECT bar{pixels.left,toolbarTop,pixels.right,pixels.top};
    RECT upper=client;
    upper.bottom=std::max(upper.top,std::min(upper.bottom,bar.top-MulDiv(1,dpi,96)));
    RECT previousUpper{}; GetWindowRect(details.Window(),&previousUpper);
    MapWindowPoints(nullptr,window,reinterpret_cast<POINT*>(&previousUpper),2);
    const bool upperMoved=!EqualRect(&previousUpper,&upper);
    const auto oldBars=GetWindowLongPtrW(details.Window(),GWL_STYLE)&(WS_HSCROLL|WS_VSCROLL);
    // Reposition without copying stale pixels, then invalidate the complete
    // parent/children (including native bars) once the final geometry is known.
    const bool zoomMoved=PositionReferenceViewerChild(zoom.Window(),pixels);
    const bool toolbarMoved=PositionReferenceViewerChild(toolbar,bar);
    details.Layout(upper,dpi,g_webOptions.load()->scrollImageDetails,viewReady);
    zoom.SetColours(m_pane.BackgroundColour(),m_pane.TextColour());
    detached.SetColours(m_pane.BackgroundColour(),m_pane.TextColour());
    zoom.SetInteractionEnabled(viewReady && g_webOptions.load()->zoom);
    EnableWindow(toolbar,viewReady);
    ShowWindow(details.Window(),SW_SHOWNOACTIVATE);
    ShowWindow(zoom.Window(),SW_SHOWNOACTIVATE); ShowWindow(toolbar,SW_SHOWNOACTIVATE);
    if(zoomMoved || toolbarMoved || upperMoved || oldBars!=
        (GetWindowLongPtrW(details.Window(),GWL_STYLE)&(WS_HSCROLL|WS_VSCROLL)))
        RedrawWindow(window,nullptr,nullptr,RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN|RDW_NOERASE);
}
void ReferenceContent::Command(UINT command,HWND source) {
    if(!viewReady) return;
    const auto options=g_webOptions.load();
    if (!options->preview || !UsesImgViewProfile()) return;
    if (command==ID_WIN2K_IMGVIEW_PRINT) {
        if (options->print && printable) CanonicalVerb(SVGIO_SELECTION,L"print",true,source==detached.ToolbarWindow() ? detached.Window() : window);
        return;
    }
    if (command==ID_WIN2K_IMGVIEW_FULLSCREEN) {
        if (!options->detached || !SelectionMatches()) return;
        if(!options->viewerExecutable.empty()) {
            const HWND owner=GetAncestor(window,GA_ROOT);
            const HRESULT launched=LaunchImageViewer(owner,options->viewerExecutable,options->viewerArguments,previewPath);
            if(FAILED(launched)) MessageBoxW(owner,
                ReferenceCaption(L"Unable to start the image viewer. Check its executable path and arguments."),
                ReferenceCaption(L"Image Preview"),MB_OK|MB_ICONERROR);
            return;
        }
        if (!preview || !preview->bitmap) return;
        detached.SetColours(m_pane.BackgroundColour(),m_pane.TextColour());
        detached.SetSourcePath(previewPath); detached.SetBitmap(preview->bitmap); detached.SetPrintable(printable);
        const HWND owner=GetAncestor(window,GA_ROOT);
        if(!detached.Show(owner,window,images,hotImages)) MessageBoxW(owner,
            ReferenceCaption(L"Unable to open the separate preview window."),
            ReferenceCaption(L"Image Preview"),MB_OK|MB_ICONERROR);
        return;
    }
    if (!options->zoom) return;
    if (source==detached.ToolbarWindow()) { detached.HandleToolbarCommand(command); return; }
    switch(command) {
        case ID_WIN2K_IMGVIEW_ZOOMIN: if (!zoom.SetMode(ImgViewZoomWindow::Mode::ZoomIn)) zoom.ZoomIn();break;
        case ID_WIN2K_IMGVIEW_ZOOMOUT: if (!zoom.SetMode(ImgViewZoomWindow::Mode::ZoomOut)) zoom.ZoomOut();break;
        case ID_WIN2K_IMGVIEW_ACTUALSIZE:zoom.ActualSize();break;
        case ID_WIN2K_IMGVIEW_BESTFIT:zoom.BestFit();break;
    }
}
void ReferenceContent::FilterLines(std::vector<PaneLine>& lines) {
    const auto options=g_webOptions.load();
    if (selectedCount>1 && !options->multiSelection) {
        lines.clear(); AddBreak(lines,ReferenceCaption(L"Select an item to view its description."));
    }
    std::erase_if(lines,[&](const PaneLine& line) {
        if (!options->metadata && !line.bold && !line.button && line.swatch==PaneLine::Swatch::None) {
            const PCWSTR prefixes[]={L"Modified: ",L"Size: ",L"Created: ",L"Accessed: ",L"Owner: ",
                L"Total File Size: ",L"Date Deleted:",L"Original Location:"};
            for(const auto prefix:prefixes) if(line.text.starts_with(ReferenceCaption(prefix))) return true;
        }
        if (!options->attributes && (line.linkAction==PaneLine::LinkAction::ItemProperties ||
            line.text.starts_with(ReferenceCaption(L"Attributes: ")))) return true;
        if (!options->seeAlso && (line.text==ReferenceCaption(L"See also:") ||
            (line.link && line.linkAction==PaneLine::LinkAction::BrowseCsidl))) return true;
        if (!options->recycleActions && (line.linkAction==PaneLine::LinkAction::RecycleEmpty ||
            line.linkAction==PaneLine::LinkAction::RecycleRestoreAll ||
            line.linkAction==PaneLine::LinkAction::RecycleRestoreSelection)) return true;
        if (!options->barricades && (line.linkAction==PaneLine::LinkAction::ShowEntireContents ||
            line.linkAction==PaneLine::LinkAction::ShowSystemFolderFiles ||
            line.linkAction==PaneLine::LinkAction::HideDriveContents)) return true;
        return false;
    });
}
} }



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

struct LegacySettings {
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
};
static LegacySettingSnapshots<LegacySettings> g_legacySettings;
using LegacySettingsScope=LegacySettingSnapshots<LegacySettings>::Scope;
#define g_settings LegacySettingSnapshots<LegacySettings>::current()

static std::wstring Px(int value) {
    return std::to_wstring(value) + L"rp";
}

static bool IsRussianUi() {
    return WebViewRussian();
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
    auto next=std::make_shared<LegacySettings>();
    LegacySettingsScope settingsWriter{next};
    LoadWebViewOptions();
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
    } else if (wcscmp(picture.get(), L"style") == 0) {
        g_settings.imagePath=L"*profile"+std::to_wstring(g_webOptions.load()->profile);
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

    {
        const auto pictureOptions=g_webOptions.load();
        g_settings.imageWidth=EffectivePictureWidth(g_settings.imagePath,pictureOptions->profile,
            pictureOptions->autoPictureWidth,g_settings.imageWidth);
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
    g_legacySettings.store(std::move(next));

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
    LegacySettingsScope settingsReader{g_legacySettings.load()};
    if (!pszXML || g_unloading) {
        return SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    // Only folder layouts, and only ones the mod has not been through.
    const WCHAR* viewHost = wcsstr(pszXML, kViewHostContainer);
    if (!viewHost || wcsstr(pszXML, kPaneAtom)) {
        return SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    std::wstring modified(pszXML, viewHost);
    modified += L"<Element id=\"atom(ClassicWebViewBanner)\" layoutpos=\"top\" height=\"0rp\" visible=\"false\" background=\""+g_settings.background.dui+L"\"/>";
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
constexpr UINT_PTR kPreviewStatusTimer = 3;
constexpr UINT_PTR kPrinterRefreshTimer = 4;
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
    ce::win2kwebview::ReferenceContent reference;
    unsigned referenceRetries=12;
    bool referencePending=false,referenceUpdating=false,referenceLayoutCommitting=false,referenceLayoutChanged=false;
    HWND tooltip=nullptr;
    int hotTip=-1, heldLink=-1, syncedBannerHeight=0;
    std::wstring tooltipText;

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
    if (!ce::win2kwebview::IsExplorerFolderView(defView)) return nullptr;
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
static void RefreshReferencePane(Pane*,bool force=false);
static void BeginReferenceNavigation(Pane*);

static void RefreshPane(Pane* pane) {
    if (g_webOptions.load()->classicLayout) { RefreshReferencePane(pane); return; }
    ce::win2kwebview::CancelPreview(pane->reference.lifetime);
    pane->reference.ClearPreview(); pane->reference.DestroyViewer();
    KillTimer(pane->hwnd,kPreviewStatusTimer); KillTimer(pane->hwnd,kPrinterRefreshTimer);
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
    if(name.starts_with(L"*profile")) {
        PaneImage image; SIZE size{};
        const auto options=g_webOptions.load();
        image.bitmap=ce::win2kwebview::LoadPaneImageResource(options->profile==1 ?
            ce::win2kwebview::IDR_WEBVIEW_98_WVLEFT : options->profile==2 ?
            ce::win2kwebview::IDR_WEBVIEW_ME_WVLEFT : ce::win2kwebview::IDR_WEBVIEW_2K_WVLEFT,size);
        image.width=size.cx; image.height=size.cy;
        BITMAP bitmap{};
        image.alpha=image.bitmap && GetObjectW(image.bitmap,sizeof(bitmap),&bitmap) && bitmap.bmBitsPixel==32;
        return image;
    }
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
    int(__cdecl* GetHeight)(void* element)=nullptr;
    HRESULT(__cdecl* SetHeight)(void* element,int height)=nullptr;
    bool(__cdecl* GetVisible)(void* element)=nullptr;
    HRESULT(__cdecl* SetVisible)(void* element,bool visible)=nullptr;
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
    g_dui.GetHeight=(decltype(g_dui.GetHeight))GetProcAddress(dui70,"?GetHeight@Element@DirectUI@@QEAAHXZ");
    g_dui.SetHeight=(decltype(g_dui.SetHeight))GetProcAddress(dui70,"?SetHeight@Element@DirectUI@@QEAAJH@Z");
    g_dui.GetVisible=(decltype(g_dui.GetVisible))GetProcAddress(dui70,"?GetVisible@Element@DirectUI@@QEAA_NXZ");
    g_dui.SetVisible=(decltype(g_dui.SetVisible))GetProcAddress(dui70,"?SetVisible@Element@DirectUI@@QEAAJ_N@Z");
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
static void* FindSpacer(HWND host,PCWSTR name=L"ClassicWebViewPane") {
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
    ATOM paneAtom = g_dui.StrToID(name);
    if (!paneAtom) {
        return nullptr;
    }

    return g_dui.FindDescendent(root, paneAtom);
}

// Whether the room DirectUI has for the file list and the pane together - all
// of the host but the folder tree with its splitter and the details pane - is
// enough for the pane and the minimum left to the list. It does not depend on
// the width of the spacer, so collapsing the spacer cannot flip it back.
static int ReferencePanelWidth(Pane* pane);
static RECT ReferenceAvailableRect(Pane* pane);
static bool PaneFits(Pane* pane, HWND viewWindow) {
    if (g_webOptions.load()->classicLayout) return (pane->reference.viewReady || pane->referencePending) && ReferencePanelWidth(pane)>0;
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
        wanted = MulDiv(g_webOptions.load()->classicLayout ? pane->reference.sideWidth : g_settings.paneWidth, dpi, 96);
    }

    bool bannerChanged=false;
    const int oldBannerHeight=pane->syncedBannerHeight;
    pane->syncedBannerHeight=0;
    if (g_dui.GetHeight && g_dui.SetHeight && g_dui.GetVisible && g_dui.SetVisible) {
        if (void* banner=FindSpacer(pane->host,L"ClassicWebViewBanner")) {
            const auto bannerOptions=g_webOptions.load();
            const int height=bannerOptions->classicLayout && bannerOptions->profile==1 &&
                pane->reference.viewReady && !pane->suppressed ? MulDiv(pane->reference.bannerHeight,dpi,96) : 0;
            const bool visible=height>0;
            if (g_dui.GetHeight(banner)!=height || g_dui.GetVisible(banner)!=visible) {
                pane->spacerTransition=true;
                g_dui.SetVisible(banner,false);
                const HRESULT sized=g_dui.SetHeight(banner,height);
                if(SUCCEEDED(sized) && g_dui.GetHeight(banner)==height && visible)
                    g_dui.SetVisible(banner,true);
                pane->spacerTransition=false;
                bannerChanged=true;
            }
            if(g_dui.GetHeight(banner)==height && g_dui.GetVisible(banner)==visible)
                pane->syncedBannerHeight=height;
        }
    }
    bannerChanged= bannerChanged || oldBannerHeight!=pane->syncedBannerHeight;
    if(bannerChanged && pane->referenceUpdating) pane->referenceLayoutChanged=true;
    // A new view starts collapsed, even when the preceding one was expanded.
    int current = g_dui.GetWidth(spacer);
    bool wasCollapsed = pane->spacerCollapsed;
    bool wasReady = pane->spacerReady;
    pane->spacerReady = true;
    pane->spacerCollapsed = current == 0;
    if (current == wanted) {
        if (bannerChanged || !wasReady || pane->spacerCollapsed != wasCollapsed) {
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

    if(pane->referenceUpdating) { pane->referenceLayoutChanged=true; return; }
    if (viewWindow) {
        RepaintFileList(viewWindow);
    }

    if (g_webOptions.load()->classicLayout && pane->spacerReady &&
        (pane->reference.bannerHeight || pane->reference.CurrentBarricade()!=ce::win2kwebview::BarricadeMode::None)) LayOutPane(pane->hwnd);
    if ((wanted != 0 || pane->reference.bannerHeight) && pane->spacerReady) {
        RedrawWindow(pane->hwnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_UPDATENOW);
    }

    // Whatever is left - the view's own background, all covered by the list.
    RedrawWindow(viewWindow ? viewWindow : pane->host, nullptr, nullptr,
                 RDW_UPDATENOW | RDW_ALLCHILDREN);
    UpdateWindow(pane->host);
}

using namespace ce::win2kwebview;
static bool ReferenceViewHasLayout(Pane* pane) {
    const HWND viewWindow=GetHostChildOf(pane->host,pane->defView);
    RECT client{};
    return viewWindow && IsShown(viewWindow) && IsShown(pane->defView) &&
        GetClientRect(pane->defView,&client) && client.right>client.left && client.bottom>client.top;
}
static void BeginReferenceNavigation(Pane* pane) {
    pane->referencePending=g_webOptions.load()->classicLayout && !pane->suppressed &&
        IsShown(pane->hwnd) && pane->spacerReady && !pane->spacerCollapsed &&
        pane->reference.sideWidth>0 && !pane->reference.bannerHeight &&
        pane->reference.CurrentBarricade()==BarricadeMode::None;
    if(pane->referencePending) pane->reference.SuspendView();
    else pane->reference.ResetView();
    pane->heldLink=-1; pane->hotLink=-1; pane->hotTip=-1;
    pane->reference.m_pane.SetPressedButton(-1);
    if(GetCapture()==pane->hwnd) ReleaseCapture();
    KillTimer(pane->hwnd,kPreviewStatusTimer); KillTimer(pane->hwnd,kPrinterRefreshTimer);
}
static void RefreshReferencePane(Pane* pane,bool force) {
    if (!pane->defView || g_unloading || pane->referenceUpdating) return;
    CComPtr<IShellBrowser> browser=GetShellBrowser(pane->defView);
    CComPtr<IShellView> view; CComPtr<IShellItem> folder;
    if (browser) browser->QueryActiveShellView(&view);
    if(ReferenceViewHasLayout(pane) && IsExplorerFolderView(pane->defView) && ShellViewMatchesWindow(view,pane->defView))
        folder.Attach(FolderItemFromView(view));
    if (!view || !folder) {
        if(!pane->referencePending) BeginReferenceNavigation(pane);
        if(pane->referenceRetries>0) {
            --pane->referenceRetries;
            SetTimer(pane->hwnd,kRefreshTimer,120,nullptr);
        } else {
            pane->referencePending=false; pane->reference.ResetView();
        }
        LayOutPane(pane->hwnd);
        return;
    }
    // Shell/property callbacks can run nested message loops. Hold paints and
    // layout until both the new model and its final reservation are ready.
    const bool suppress=g_settings.skipControlPanel && IsControlPanelFolder(pane->defView);
    ViewerPaintTransaction held(pane->reference.details.Window(),pane->reference.zoom.Window(),pane->reference.toolbar);
    pane->referenceUpdating=true; pane->referenceLayoutChanged=false;
    pane->referenceRetries=12;
    const auto name=GetItemText(folder,SIGDN_NORMALDISPLAY);
    const auto key=GetItemText(folder,SIGDN_DESKTOPABSOLUTEPARSING);
    if (force || key!=pane->reference.folderKey || name!=pane->title) {
        if (pane->icon) DestroyIcon(std::exchange(pane->icon,nullptr));
        if (g_settings.showHeader) pane->icon=GetItemIcon(folder,Scale(pane,32));
        pane->title=name;
    }
    pane->reference.m_pane.SetFolder(name,pane->icon);
    pane->reference.navigate=[defView=pane->defView](PCIDLIST_ABSOLUTE pidl) {
        CComPtr<IShellBrowser> browser=GetShellBrowser(defView);
        return browser ? browser->BrowseObject(pidl,SBSP_SAMEBROWSER|SBSP_ABSOLUTE) : E_NOINTERFACE;
    };
    pane->reference.isActiveView=[defView=pane->defView](IShellView* expected) {
        CComPtr<IShellBrowser> browser=GetShellBrowser(defView);
        CComPtr<IShellView> current;
        if(!browser || FAILED(browser->QueryActiveShellView(&current)) || !ShellViewMatchesWindow(current,defView)) return false;
        CComQIPtr<IUnknown> currentIdentity(current), expectedIdentity(expected);
        return currentIdentity && currentIdentity.p==expectedIdentity.p;
    };
    pane->reference.Refresh(view,folder,pane->hwnd,force);
    pane->referencePending=false;
    KillTimer(pane->hwnd,kPreviewStatusTimer);
    if(pane->reference.pendingSince) {
        const ULONGLONG elapsed=GetTickCount64()-pane->reference.pendingSince;
        if(elapsed<1000) SetTimer(pane->hwnd,kPreviewStatusTimer,static_cast<UINT>(1000-elapsed),nullptr);
    }
    KillTimer(pane->hwnd,kPrinterRefreshTimer);
    if(pane->reference.m_isPrinters && g_webOptions.load()->printerRefresh)
        SetTimer(pane->hwnd,kPrinterRefreshTimer,5000,nullptr);
    // Existing switches still apply in the faithful layout.
    if (!g_settings.showDriveSpace) pane->reference.m_pane.SetCapacityPie(-1);
    pane->suppressed=suppress;
    pane->referenceLayoutCommitting=true;
    OnSyncSpacer(pane); LayOutPane(pane->hwnd);
    held.Commit();
    pane->referenceLayoutCommitting=false; pane->referenceUpdating=false;
    if(pane->referenceLayoutChanged) {
        const HWND viewWindow=GetHostChildOf(pane->host,pane->defView);
        if(viewWindow) RepaintFileList(viewWindow);
    }
    RedrawWindow(pane->hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
}
static void SetReferenceStatus(Pane* pane,int index) {
    CComPtr<IShellBrowser> browser=GetShellBrowser(pane->defView);
    const auto* line=pane->reference.m_pane.Line(index);
    if (browser) browser->SetStatusTextSB(line ? line->status.c_str() : L"");
    if (!g_webOptions.load()->tooltips) return;
    if (!pane->tooltip) {
        pane->tooltip=CreateWindowExW(WS_EX_TOPMOST,TOOLTIPS_CLASSW,nullptr,
            WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX,0,0,0,0,pane->hwnd,nullptr,ReferenceModule(),nullptr);
        if (pane->tooltip) {
            TOOLINFOW info{sizeof(info)}; info.uFlags=TTF_IDISHWND|TTF_TRACK|TTF_ABSOLUTE;
            info.hwnd=pane->hwnd; info.uId=reinterpret_cast<UINT_PTR>(pane->hwnd);
            SendMessageW(pane->tooltip,TTM_ADDTOOLW,0,reinterpret_cast<LPARAM>(&info));
            SendMessageW(pane->tooltip,TTM_SETMAXTIPWIDTH,0,300);
        }
    }
    if (pane->tooltip) {
        pane->tooltipText=line ? (!line->tooltip.empty() ? line->tooltip : line->status) : L"";
        TOOLINFOW info{sizeof(info)}; info.hwnd=pane->hwnd;
        info.uId=reinterpret_cast<UINT_PTR>(pane->hwnd); info.lpszText=pane->tooltipText.data();
        SendMessageW(pane->tooltip,TTM_UPDATETIPTEXTW,0,reinterpret_cast<LPARAM>(&info));
        POINT cursor{}; GetCursorPos(&cursor);
        SendMessageW(pane->tooltip,TTM_TRACKPOSITION,0,MAKELPARAM(cursor.x+12,cursor.y+18));
        SendMessageW(pane->tooltip,TTM_TRACKACTIVATE,!pane->tooltipText.empty(),reinterpret_cast<LPARAM>(&info));
    }
}
static int ReferenceLinkAt(Pane* pane,POINT point) {
    if(!pane->reference.viewReady) return -1;
    point.x=MulDiv(point.x,96,pane->dpi); point.y=MulDiv(point.y,96,pane->dpi);
    return pane->reference.m_pane.HitTestLink(point);
}
static int ReferenceTooltipAt(Pane* pane,POINT point) {
    if(!pane->reference.viewReady) return -1;
    point.x=MulDiv(point.x,96,pane->dpi); point.y=MulDiv(point.y,96,pane->dpi);
    return pane->reference.m_pane.HitTestTooltip(point);
}
static RECT ReferenceAvailableRect(Pane* pane) {
    RECT result{}; GetClientRect(pane->host,&result);
    HWND view=GetHostChildOf(pane->host,pane->defView);
    const RECT host=result;
    for(HWND child=GetWindow(pane->host,GW_CHILD);child;child=GetWindow(child,GW_HWNDNEXT)) {
        if (child==pane->hwnd || child==view || !IsShown(child)) continue;
        RECT rect{}; if (!GetWindowRect(child,&rect)) continue;
        MapWindowPoints(nullptr,pane->host,reinterpret_cast<POINT*>(&rect),2);
        // Full-height docked tree / details windows; toolbars cannot eat width.
        if (rect.bottom-rect.top < (host.bottom-host.top)*2/3) continue;
        if (rect.left<=host.left+1) result.left=std::max(result.left,rect.right+Scale(pane,3));
        else if (rect.right>=host.right-1) result.right=std::min(result.right,rect.left);
    }
    return result;
}
static int ReferencePanelWidth(Pane* pane) {
    const RECT available=ReferenceAvailableRect(pane);
    const int width=MulDiv(available.right-available.left,96,pane->dpi);
    int panel=pane->reference.m_pane.PanelWidthFor(width);
    const auto options=g_webOptions.load();
    if (options->profile==1 && pane->reference.m_isWin98SystemFolder && width<450) panel=0;
    pane->reference.sideWidth=panel;
    pane->reference.bannerHeight=options->miniBanner ? pane->reference.m_pane.MiniBannerHeightFor(width) : 0;
    if (pane->reference.CurrentBarricade()!=BarricadeMode::None) pane->reference.bannerHeight=0;
    return panel;
}
static RECT ReferencePaneContentRect(const Pane* pane,const RECT& client) {
    RECT content=client;
    if(g_settings.paneBorder && !g_settings.removeViewBorder && !pane->reference.bannerHeight) {
        // The pane covers the adjacent edge of the list. Keep the other three
        // edges and their insets exactly as in the pre-viewer layout.
        const int horizontal=GetSystemMetricsForDpi(SM_CXEDGE,pane->dpi);
        const int vertical=GetSystemMetricsForDpi(SM_CYEDGE,pane->dpi);
        if(g_settings.onRight) content.right-=horizontal;
        else content.left+=horizontal;
        content.top+=vertical; content.bottom-=vertical;
        content.right=std::max(content.left,content.right);
        content.bottom=std::max(content.top,content.bottom);
    }
    return content;
}
// Called with g_imageMutex held. Details paints also refresh these borrowed
// images, so a settings change cannot leave the child using freed cache data.
static void ConfigureReferenceDecoration(ce::win2kwebview::WebViewNativePane& renderer) {
    renderer.SetColours(g_settings.background.Get(),g_settings.text.Get(),
        g_settings.title.Get(),g_settings.link.Get());
    renderer.SetPictureWhiteBlend(g_settings.imageBlendWhite);
    const auto* image=EnsureImage(g_picture,g_settings.imagePath);
    const auto* divider=EnsureImage(g_dividerPicture,g_settings.dividerImagePath);
    renderer.SetDivider(divider ? divider->bitmap : nullptr,
        divider ? SIZE{divider->width,divider->height} : SIZE{},divider && divider->alpha,
        g_settings.divider.Get(),g_settings.background.Get(),g_settings.dividerGradient);
    renderer.SetDecoration(image ? image->bitmap : nullptr,
        image ? SIZE{image->width,image->height} : SIZE{},g_settings.imageWidth,
        true,g_settings.showHeader,image && image->alpha,image ? image->icon : nullptr);
    if(g_settings.imagePath.starts_with(L"*profile")) renderer.UseProfileDecoration(true,g_settings.showHeader);
}
// Run before BeginPaint: its child exclusion region must use the new bounds.
// Moving children after BitBlt leaves the previously excluded areas unpainted.
static void PrepareReferenceViewer(Pane* pane) {
    RECT client{}; GetClientRect(pane->hwnd,&client);
    auto& reference=pane->reference;
    reference.m_pane.EnsureResources();
    reference.details.SetPainter([pane](HDC detailsDc,const RECT& viewport,POINT scroll,int dividerWidth) {
        LegacySettingsScope settingsReader{g_legacySettings.load()};
        std::lock_guard lock(g_imageMutex);
        ConfigureReferenceDecoration(pane->reference.m_pane);
        return pane->reference.m_pane.PaintImgDetails(detailsDc,viewport,96,scroll,dividerWidth);
    });
    reference.LayoutViewer(ReferencePaneContentRect(pane,client),pane->dpi,IsWindowVisible(pane->hwnd));
}
static void PaintReferencePane(Pane* pane,HDC target,const RECT& client) {
    const int width=client.right-client.left,height=client.bottom-client.top;
    if (width<=0 || height<=0) return;
    HDC dc=CreateCompatibleDC(target); HBITMAP buffer=CreateCompatibleBitmap(target,width,height);
    if (!dc || !buffer) { if(dc) DeleteDC(dc); if(buffer) DeleteObject(buffer); return; }
    const auto old=SelectObject(dc,buffer);
    HBRUSH background=CreateSolidBrush(g_settings.background.Get());
    FillRect(dc,&client,background); DeleteObject(background);
    const RECT content=ReferencePaneContentRect(pane,client);
    const RECT logical{MulDiv(content.left,96,pane->dpi),MulDiv(content.top,96,pane->dpi),
        MulDiv(content.right,96,pane->dpi),MulDiv(content.bottom,96,pane->dpi)};
    const int saved=SaveDC(dc);
    if(!saved) { SelectObject(dc,old); DeleteObject(buffer); DeleteDC(dc); return; }
    SetMapMode(dc,MM_ANISOTROPIC); SetWindowExtEx(dc,96,96,nullptr); SetViewportExtEx(dc,pane->dpi,pane->dpi,nullptr);
    IntersectClipRect(dc,logical.left,logical.top,logical.right,logical.bottom);
    auto& reference=pane->reference;
    reference.m_pane.EnsureResources();
    {
        std::lock_guard lock(g_imageMutex);
        ConfigureReferenceDecoration(reference.m_pane);
        if (reference.bannerHeight) reference.m_pane.PaintWin98MiniBanner(dc,logical);
        else if (reference.CurrentBarricade()!=BarricadeMode::None) {
            const int viewWidth=logical.right;
            const auto shape=reference.m_isWin98SystemFolder ? WebViewNativePane::Win98BarricadeShapeFor(viewWidth) :
                             WebViewNativePane::BarricadeShapeFor(viewWidth);
            reference.m_pane.SetBarricade(shape,reference.m_isEntireNetwork ? BarricadeArt::Network : BarricadeArt::Logo);
            RECT panel=logical;
            if (shape==BarricadeMode::Beside) {
                panel.right=reference.sideWidth;
                RECT brand=logical; brand.left=panel.right;
                reference.m_pane.PaintBarricade(dc,brand);
            }
            reference.m_pane.Paint(dc,panel,96);
        } else {
            reference.m_pane.SetBarricade(BarricadeMode::None);
            if(reference.details.Window() && reference.UsesImgViewProfile())
                reference.m_pane.PaintImgFrame(dc,logical);
            else reference.m_pane.Paint(dc,logical,96);
        }
    }
    RestoreDC(dc,saved);
    if(g_settings.paneBorder && !g_settings.removeViewBorder && !reference.bannerHeight) {
        RECT frame=client;
        DrawEdge(dc,&frame,EDGE_SUNKEN,(g_settings.onRight ? BF_RIGHT : BF_LEFT)|BF_TOP|BF_BOTTOM);
    }
    BitBlt(target,0,0,width,height,dc,0,0,SRCCOPY);
    SelectObject(dc,old); DeleteObject(buffer); DeleteDC(dc);
}

static LRESULT CALLBACK PaneWndProc(HWND hWnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    LegacySettingsScope settingsReader{g_legacySettings.load()};
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

        case WM_SIZE:
            if(pane && g_webOptions.load()->classicLayout && !pane->referenceUpdating)
                PrepareReferenceViewer(pane);
            InvalidateRect(hWnd,nullptr,FALSE);
            return 0;
        case WM_PAINT: {
            if(pane && pane->referenceUpdating) {
                // Validate nested paints without exposing the partly updated model.
                // Commit below explicitly invalidates the final complete presentation.
                PAINTSTRUCT retained{};BeginPaint(hWnd,&retained);EndPaint(hWnd,&retained);
                return 0;
            }
            if(pane && g_webOptions.load()->classicLayout) PrepareReferenceViewer(pane);
            if (pane) {
                PAINTSTRUCT paint;
                HDC dc = BeginPaint(hWnd, &paint);
                RECT client;
                GetClientRect(hWnd, &client);
                if (g_webOptions.load()->classicLayout) PaintReferencePane(pane,dc,client);
                else PaintPane(pane,dc,client);
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
            int hot = g_webOptions.load()->classicLayout ? ReferenceLinkAt(pane,point) : LinkFromPoint(pane, point);
            if(g_webOptions.load()->classicLayout) {
                int tip=hot>=0 ? hot : ReferenceTooltipAt(pane,point);
                if(tip!=pane->hotTip) { pane->hotTip=tip; SetReferenceStatus(pane,tip); }
                if(pane->heldLink>=0 && pane->reference.m_pane.SetPressedButton(hot==pane->heldLink ? hot : -1))
                    InvalidateRect(hWnd,nullptr,FALSE);
            }
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
                if(pane->hotTip!=-1) { pane->hotTip=-1; SetReferenceStatus(pane,-1); }
                if(pane->heldLink>=0 && pane->reference.m_pane.SetPressedButton(-1))
                    InvalidateRect(hWnd,nullptr,FALSE);
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
            if (g_webOptions.load()->classicLayout) {
                int clicked=ReferenceLinkAt(pane,point);
                const int pressed=pane->heldLink;
                if(GetCapture()==hWnd) ReleaseCapture();
                pane->reference.m_pane.SetPressedButton(-1); pane->heldLink=-1;
                const auto* line=pane->reference.m_pane.Line(clicked);
                const auto life=pane->reference.lifetime;
                if(line && (!line->button || clicked==pressed)) pane->reference.Activate(clicked);
                bool alive=false; { std::lock_guard lock(life->mutex); alive=life->alive; }
                if(alive && IsWindow(hWnd)) RefreshReferencePane(pane,false); return 0;
            }
            int index = LinkFromPoint(pane, point);
            if (index >= 0 && (size_t)index < g_settings.links.size()) {
                FollowLink(pane, g_settings.links[index]);
            }
            return 0;
        }

        case ce::win2kwebview::kPreviewReadyMessage:
            if (pane) pane->reference.AcceptPreview();
            return 0;
        case ce::win2kwebview::kPrinterReadyMessage:
            if(pane) {
                pane->reference.forceRefresh=true;
                RefreshReferencePane(pane,false);
            }
            return 0;
        case WM_COMMAND:
            if (pane && g_webOptions.load()->classicLayout)
                pane->reference.Command(LOWORD(wParam),reinterpret_cast<HWND>(lParam));
            return 0;
        case WM_LBUTTONDOWN:
            if(pane && g_webOptions.load()->classicLayout) {
                POINT point{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                int hit=ReferenceLinkAt(pane,point);
                const auto* line=pane->reference.m_pane.Line(hit);
                if(line && line->button) {
                    pane->heldLink=hit; pane->reference.m_pane.SetPressedButton(hit);
                    SetCapture(hWnd); SetFocus(hWnd); InvalidateRect(hWnd,nullptr,FALSE);
                }
                return 0;
            }
            break;
        case WM_CAPTURECHANGED:
            if(pane) { pane->heldLink=-1; pane->reference.m_pane.SetPressedButton(-1); InvalidateRect(hWnd,nullptr,FALSE); }
            break;
        case WM_SYSCHAR:
            if(pane && g_webOptions.load()->classicLayout) {
                int index=pane->reference.m_pane.FindButtonByAccessKey(static_cast<wchar_t>(wParam));
                if(index>=0) {
                    const auto life=pane->reference.lifetime; pane->reference.Activate(index);
                    bool alive=false; { std::lock_guard lock(life->mutex); alive=life->alive; }
                    if(alive && IsWindow(hWnd)) RefreshReferencePane(pane,false); return 0;
                }
            }
            break;
        case WM_KEYDOWN:
            if (pane && g_webOptions.load()->classicLayout) {
                const UINT command=ce::win2kwebview::ImgViewAcceleratorCommand(wParam);
                if(command) { pane->reference.Command(command); return 0; }
            }
            break;
        case WM_CONTEXTMENU:
            if (pane && g_webOptions.load()->classicLayout && pane->reference.UsesImgViewProfile())
                pane->reference.zoom.ShowContextMenu(hWnd,pane->reference.toolbar,
                    g_webOptions.load()->detached,g_webOptions.load()->print,pane->reference.printable,lParam);
            return 0;
        case WM_TIMER: {
            if (pane && wParam==kPreviewStatusTimer) {
                KillTimer(hWnd,kPreviewStatusTimer);
                if(pane->reference.pendingSince) {
                    pane->reference.m_pane.SetThumbnailPending(true); InvalidateRect(hWnd,nullptr,FALSE);
                }
                return 0;
            }
            if (pane && wParam==kPrinterRefreshTimer) {
                KillTimer(hWnd,kPrinterRefreshTimer);
                RefreshReferencePane(pane,false); return 0;
            }
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
                pane->reference.m_pane.InvalidateFonts(); pane->reference.forceRefresh=true;
                if(g_webOptions.load()->classicLayout) RefreshReferencePane(pane,false);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE: {
            if (pane) {
                FreeFonts(pane);
                pane->reference.m_pane.InvalidateFonts(); pane->reference.forceRefresh=true;
                if(g_webOptions.load()->classicLayout) RefreshReferencePane(pane,false);
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
                    if (void* banner=FindSpacer(pane->host,L"ClassicWebViewBanner")) {
                        if(g_dui.SetVisible) g_dui.SetVisible(banner,false);
                        if(g_dui.SetHeight) g_dui.SetHeight(banner,0);
                    }
                }
            }
            DestroyWindow(hWnd);
            return 0;
        }

        case WM_PANE_SETTINGS: {
            if (pane) {
                FreeFonts(pane);
                ApplyViewBorder(pane->defView);
                pane->reference.DestroyViewer(); pane->reference.forceRefresh=true;
                if (pane->tooltip) DestroyWindow(std::exchange(pane->tooltip,nullptr));
                if (g_webOptions.load()->classicLayout) RefreshReferencePane(pane,true);
                else RefreshPane(pane);
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
                if (pane->tooltip) DestroyWindow(pane->tooltip);
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
    Pane* referencePane=(Pane*)GetWindowLongPtrW(self,GWLP_USERDATA);
    const int requested=g_webOptions.load()->classicLayout && referencePane ? referencePane->reference.sideWidth : g_settings.paneWidth;
    int paneWidth = MulDiv(requested, dpi, 96);

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
    LegacySettingsScope settingsReader{g_legacySettings.load()};
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
    LegacySettingsScope settingsReader{g_legacySettings.load()};
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

    if(pane->referenceUpdating && !pane->referenceLayoutCommitting) return;
    if (g_webOptions.load()->classicLayout && !pane->reference.viewReady && !pane->referencePending) {
        ShowWindow(paneWindow,SW_HIDE); SyncSpacer(pane); return;
    }
    if (pane->suppressed) {
        ShowWindow(paneWindow, SW_HIDE);
        return;
    }

    // The retired view can leave DirectUI before its replacement arrives.
    // Suspend it here, before its detached bounds can hide the ready panel.
    if(g_webOptions.load()->classicLayout && pane->reference.viewReady &&
        !GetHostChildOf(pane->host,pane->defView)) BeginReferenceNavigation(pane);

    // Windows come and go with navigation, so this runs every time.
    WatchSiblings(pane->host, paneWindow);

    // OnSyncSpacer lays the pane out itself once the spacer has its new width.
    if (pane->spacerTransition) {
        return;
    }

    SyncSpacer(pane);

    if(g_webOptions.load()->classicLayout && pane->referencePending && !pane->reference.viewReady) {
        // Explorer inserts the replacement view hidden and at 0x0 before
        // giving it its final bounds. Those bounds cannot place or hide the
        // last complete panel. Retain its geometry and its overlap above the
        // list frame until the new model and reservation can be committed.
        SetWindowPos(paneWindow,HWND_TOP,0,0,0,0,
            SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREDRAW);
        return;
    }

    if (g_dui.ok && !pane->spacerReady) {
        ShowWindow(paneWindow, SW_HIDE);
        return;
    }

    if (g_webOptions.load()->classicLayout && pane->spacerReady &&
        ((pane->reference.bannerHeight && pane->syncedBannerHeight==Scale(pane,pane->reference.bannerHeight)) ||
        pane->reference.CurrentBarricade()!=ce::win2kwebview::BarricadeMode::None)) {
        RECT rect=ReferenceAvailableRect(pane);
        if (pane->reference.bannerHeight) {
            RECT view{}; GetWindowRect(pane->defView,&view);
            MapWindowPoints(nullptr,pane->host,reinterpret_cast<POINT*>(&view),2);
            rect.bottom=view.top; rect.top=rect.bottom-Scale(pane,pane->reference.bannerHeight);
        }
        if (rect.right>rect.left && rect.bottom>rect.top) {
            SetWindowPos(paneWindow,HWND_TOP,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,SWP_NOACTIVATE|SWP_SHOWWINDOW);
            RECT client{}; GetClientRect(paneWindow,&client); pane->reference.LayoutViewer(client,pane->dpi,false);
            InvalidateRect(paneWindow,nullptr,FALSE); return;
        }
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
                 SWP_NOACTIVATE | SWP_SHOWWINDOW |
                     (pane->referenceUpdating ? SWP_NOREDRAW|SWP_NOCOPYBITS : 0));
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
    LegacySettingsScope settingsReader{g_legacySettings.load()};
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
    if (!ce::win2kwebview::IsExplorerFolderView(defView)) return;
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
            WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, host, nullptr,
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
        if(pane->defView) RemovePropW(pane->defView,kPaneProperty);
        BeginReferenceNavigation(pane);
        pane->referenceRetries=12;
        // Navigation can reuse the host and pane with a new, not yet laid
        // out view. Its spacer must be checked before reusing a visible pane.
        if(!pane->referencePending) {
            pane->spacerReady = false;
            pane->spacerCollapsed = true;
            ShowWindow(paneWindow, SW_HIDE);
        }
        pane->spacerRetries = kSpacerRetries;
    }
    pane->host = host;
    pane->defView = defView;
    SetPropW(defView, kPaneProperty, paneWindow);

    // Decided here as well as in RefreshPane, so a Control Panel window never
    // shows the pane for the moment before the first refresh runs.
    pane->suppressed =
        g_settings.skipControlPanel && IsControlPanelFolder(defView);

    ApplyViewBorder(defView);
    if(g_webOptions.load()->classicLayout) RefreshReferencePane(pane,false);
    else LayOutPane(paneWindow);

    // The view has only just been created; it needs a moment before it can
    // answer for its folder.
    SetTimer(paneWindow, kRefreshTimer, 120, nullptr);
}

// The window may belong to another thread - the mod is loaded into Explorer
// windows that are already open, and every Explorer window runs on one of its
// own. A window has to be created by the thread that pumps its parent, so the
// work is handed over to the view itself.
static void AttachToDefView(HWND defView) {
    if (!ce::win2kwebview::IsExplorerFolderView(defView)) return;
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
    LegacySettingsScope settingsReader{g_legacySettings.load()};
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
    g_unloading=false;
    g_adoptMessage = RegisterWindowMessageW(L"ClassicWebViewPane_Adopt");

    g_dui70 = LoadLibraryExW(L"dui70.dll", nullptr,
                             LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dui70) {
        Wh_Log(L"dui70.dll could not be loaded");
        return FALSE;
    }

    LoadDuiApi(g_dui70);
    LoadSettings();
    LegacySettingsScope settingsReader{g_legacySettings.load()};

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

    if(!ce::win2kwebview::RegisterReferenceViewerClasses()) {
        Wh_Log(L"Viewer window classes could not be registered");
        UnregisterClassW(kPaneClassName,GetModuleHandleW(nullptr)); g_paneClass=0;
        FreeLibrary(g_dui70); g_dui70=nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    LegacySettingsScope settingsReader{g_legacySettings.load()};
    AdoptOpenWindows();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    LegacySettingsScope settingsReader{g_legacySettings.load()};

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
    LegacySettingsScope settingsReader{g_legacySettings.load()};
    g_unloading = true;

    // A window still carrying a subclass of the mod, or a window whose window
    // procedure is in it, calls into a DLL that is about to be gone.
    ClosePanes();
    ce::win2kwebview::WaitForReferenceWorkers();
    DropAllSubclasses();
    RestoreViewBorders();
    ce::win2kwebview::FreeReferenceSharedResources();

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
