// ==WindhawkMod==
// @id              desktop-app-groups
// @name            Desktop App Groups
// @description     Group desktop icons into Start-menu-style folders by dragging one onto another
// @description:de-DE  Desktopsymbole in Ordner wie im Startmenü gruppieren, indem man eines auf ein anderes zieht
// @description:es-ES  Agrupa los iconos del escritorio en carpetas como las del menú Inicio arrastrando uno sobre otro
// @description:fr-FR  Regroupez les icônes du bureau dans des dossiers comme ceux du menu Démarrer en faisant glisser l'une sur l'autre
// @description:it-IT  Raggruppa le icone del desktop in cartelle come quelle del menu Start trascinandone una sull'altra
// @description:ja-JP  デスクトップのアイコンを別のアイコンにドラッグして、スタート メニューのようなフォルダーにまとめます
// @description:ko-KR  바탕 화면 아이콘을 다른 아이콘 위로 끌어 시작 메뉴처럼 폴더로 묶습니다
// @description:nl-NL  Groepeer bureaubladpictogrammen in mappen zoals in het Startmenu door het ene op het andere te slepen
// @description:pl-PL  Grupuj ikony pulpitu w foldery jak w menu Start, przeciągając jedną na drugą
// @description:pt-BR  Agrupe ícones da área de trabalho em pastas como as do menu Iniciar arrastando um sobre o outro
// @description:ru-RU  Объединяйте значки рабочего стола в папки, как в меню «Пуск», перетаскивая один значок на другой
// @description:tr-TR  Masaüstü simgelerini birini diğerinin üzerine sürükleyerek Başlat menüsündeki gibi klasörlerde gruplayın
// @description:uk-UA  Об'єднуйте значки робочого столу в папки, як у меню «Пуск», перетягуючи один значок на інший
// @description:zh-CN  将一个桌面图标拖到另一个上，即可像“开始”菜单那样把它们归入文件夹
// @description:zh-TW  將一個桌面圖示拖曳到另一個上，即可像「開始」功能表一樣將它們歸入資料夾
// @version         1.0
// @author          Markc719
// @github          https://github.com/Markc719
// @homepage        https://github.com/Markc719/desktop-app-groups
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luuid -lole32 -lshell32 -lshlwapi -lcomctl32 -lgdi32 -lmsimg32 -ldwmapi -lshcore -ladvapi32 -luser32 -lcomdlg32 -ld2d1 -ldwrite
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop App Groups

Group your desktop icons into folders that work like Start menu folders.
Drag one app onto another to make a group, then click the group (or just
hover over it) to open it in a popup beside it. Your other desktop folders
can open in the same popup too.

![A group open on the desktop](https://raw.githubusercontent.com/Markc719/desktop-app-groups/main/screenshot-popup.png)

![The group settings panel](https://raw.githubusercontent.com/Markc719/desktop-app-groups/main/screenshot-settings.png)

![A folder in the list view](https://raw.githubusercontent.com/Markc719/desktop-app-groups/main/screenshot-list.png)

![Groups' icons on the desktop](https://raw.githubusercontent.com/Markc719/desktop-app-groups/main/screenshot-desktop.png)

## Features

- **Make a group:** drag an app onto another app, hold it there for a
  moment, and let go. The new group appears in that spot, and its icon
  shows a preview of the first four apps inside.
- **Open a group:** click it, or with the hover option, just rest the mouse
  on it. A popup opens beside it with a blurred background, growing out of
  the group's icon like a Start menu folder. Click an app to launch it.
  Click elsewhere, press Esc, or click the group again to close it. A popup
  opened by hovering closes when the mouse moves away, unless you click
  inside it first.
- **Big groups and folders scroll:** with the mouse wheel or the scroll bar.
  The popup shows up to 5 rows of icons (12 in the list view) and always
  fits on screen.
- **Icons or a list:** show apps as a grid of icons, or as a list with each
  file's size.
- **Other folders too:** turn on "Open other desktop folders in the popup
  too", and any folder on your desktop opens in the same popup, with the
  same options. Their icons aren't changed.
- **Open in File Explorer:** the folder button at the top of the popup opens
  the group as a normal folder window.
- **Add apps:** drop more apps onto the group.
- **Rearrange apps:** drag them around inside the popup. The group's icon
  shows the first four.
- **Take an app out:** drag it past the edge of the popup, onto the desktop
  or anywhere else you can drop files. A group that's down to its last app
  is removed, and the app goes back where the group was.
- **Rename a group:** click its name at the top of the popup, or press F2.
- **Right-click** an app in the popup for its usual menu: Open file
  location, Pin to taskbar, Properties and so on. Shift+right-click shows
  extra options. Right-click the group's name for the group's own menu.
- **Colour-code groups:** give a group a colour, from eight presets or any
  colour you pick. Its desktop icon and its popup are tinted to match, so
  groups are easy to tell apart at a glance.
- **Group settings:** the gear button next to the group's name opens a panel
  with the view (icons or list), apps per row, icon size, spacing between
  icons (compact, normal or roomy), app names, colour, sort order (your
  arrangement or A to Z), and whether a group is removed when one app is
  left. Each can be set for that group or for all groups.
- **In your language:** the popup, its settings panel and this mod's
  settings are translated into Chinese (Simplified and Traditional), Dutch,
  English, French, German, Italian, Japanese, Korean, Polish, Portuguese
  (Brazil), Russian, Spanish, Turkish and Ukrainian. It follows Windows'
  display language, or you can pick one in the Settings tab. Corrections from
  native speakers are welcome on the mod's GitHub page.
- **High contrast themes:** the popup uses the theme's colours and a clear
  outline.

## How groups are stored

A group is an ordinary folder on your desktop with your shortcuts in it, so
if you uninstall the mod, nothing is lost. Inside it are two hidden files
that Windows needs to show the group's icon: the icon itself and a
`desktop.ini` that points to it.

The mod's own data for each group (that it's a group, your app order and
the group's own settings) is kept in Windhawk's storage on this PC. If you'd
like groups to keep that data when your desktop is synced to another PC,
copied or backed up, turn on "Keep group data inside the group folders" in
the Settings tab: it's then kept in each group's `desktop.ini` instead (see
the setting's description for the side effects).

## Uninstalling

Groups are normal folders, so they stay on your desktop with your shortcuts
in them. Each one keeps what Windows needs to show its custom icon: its last
group icon, the hidden `desktop.ini` and `group-icon-....ico` files inside
it, and the folder's read-only flag (which, for folders, only tells Windows
to look for a `desktop.ini`; it doesn't make anything read-only). If you
turned on "Keep group data inside the group folders", each `desktop.ini`
also still has the group's app order and settings.

To turn a group back into a plain folder, either move the shortcuts out and
delete the folder, or:

1. Right-click the group, then Properties, then Customize, then Change Icon,
   and click Restore Defaults.
2. In File Explorer, turn on View, then Show, then Hidden items, open the
   folder, and delete `desktop.ini` and the `group-icon-....ico` file.

## Tips

- Ctrl+click a group to select it without opening it. Right-click it for
  the usual folder menu, including Open to see it as a normal folder.
- Ctrl+Z on the desktop undoes creating or renaming a group.
- With "Auto arrange icons" turned on (right-click the desktop, then View),
  Windows decides where icons go, so new groups appear in the next free spot.
- Some installers put shortcuts on the shared Public desktop, for all users.
  Moving one of those into a group needs administrator permission, and
  Windows will ask for it.

## Settings

In this Settings tab:

- **Language:** the same as Windows (the default), or any of the languages
  above.
- **How to create a group:** hover over an icon for a moment before
  dropping (the default), hold Shift while dropping, or always.
- **Hover time** for the hover option.
- **Open groups with** a single click (the default, like the Start menu), a
  double-click (like normal folders), or hovering, with its own **hover
  time**.
- **Open other desktop folders in the popup too:** off by default. Folders
  that aren't groups only ever have their popup settings and order kept in
  Windhawk's storage; the mod never writes into them.
- **Blurred popup background.** The popup uses a solid background if
  Windows' Transparency effects setting is off, or with a high contrast
  theme.
- **Background colour strength:** how see-through the blurred background is.
- **Rows before scrolling:** how many rows of icons (5 by default) and of
  the list view (12) the popup shows before it scrolls.
- **Animation length:** how long the popup takes to open (200 milliseconds
  by default; closing takes three quarters of that).
- **Animate the popup** when it opens and closes.
- **Keep group data inside the group folders:** off by default. Turn it on
  if your desktop is synced between PCs (for example with OneDrive) and you
  want your groups to stay groups everywhere.

Layout and behaviour settings for groups are in each popup's gear panel.

Made for Windows 11.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: "Language"
  $name:de-DE: "Sprache"
  $name:es-ES: "Idioma"
  $name:fr-FR: "Langue"
  $name:it-IT: "Lingua"
  $name:ja-JP: "言語"
  $name:ko-KR: "언어"
  $name:nl-NL: "Taal"
  $name:pl-PL: "Język"
  $name:pt-BR: "Idioma"
  $name:ru-RU: "Язык"
  $name:tr-TR: "Dil"
  $name:uk-UA: "Мова"
  $name:zh-CN: "语言"
  $name:zh-TW: "語言"
  $description: "The language of the popup, its settings panel and new group names."
  $description:de-DE: "Die Sprache des Popups, seines Einstellungsbereichs und neuer Gruppennamen."
  $description:es-ES: "El idioma de la ventana emergente, su panel de configuración y los nombres de los grupos nuevos."
  $description:fr-FR: "La langue de la fenêtre, de son panneau de paramètres et des noms des nouveaux groupes."
  $description:it-IT: "La lingua del popup, del suo pannello impostazioni e dei nomi dei nuovi gruppi."
  $description:ja-JP: "ポップアップ、設定パネル、新しいグループ名の言語です。"
  $description:ko-KR: "팝업, 설정 패널, 새 그룹 이름의 언어입니다."
  $description:nl-NL: "De taal van de pop-up, het instellingenpaneel en de namen van nieuwe groepen."
  $description:pl-PL: "Język okienka, jego panelu ustawień i nazw nowych grup."
  $description:pt-BR: "O idioma da janela pop-up, do painel de configurações e dos nomes de novos grupos."
  $description:ru-RU: "Язык всплывающего окна, панели настроек и названий новых групп."
  $description:tr-TR: "Açılır pencerenin, ayarlar panelinin ve yeni grup adlarının dili."
  $description:uk-UA: "Мова спливного вікна, панелі налаштувань і назв нових груп."
  $description:zh-CN: "弹出窗口、设置面板和新组名称所用的语言。"
  $description:zh-TW: "快顯視窗、設定面板和新群組名稱所用的語言。"
  $options:
  - auto: "Same as Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:de-DE:
  - auto: "Wie Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:es-ES:
  - auto: "Igual que Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:fr-FR:
  - auto: "Comme Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:it-IT:
  - auto: "Come Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:ja-JP:
  - auto: "Windows と同じ"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:ko-KR:
  - auto: "Windows와 같게"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:nl-NL:
  - auto: "Zoals Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:pl-PL:
  - auto: "Taki jak w Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:pt-BR:
  - auto: "Igual ao Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:ru-RU:
  - auto: "Как в Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:tr-TR:
  - auto: "Windows ile aynı"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:uk-UA:
  - auto: "Як у Windows"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:zh-CN:
  - auto: "与 Windows 相同"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
  $options:zh-TW:
  - auto: "與 Windows 相同"
  - en: "English"
  - de: "Deutsch"
  - es: "Español"
  - fr: "Français"
  - it: "Italiano"
  - ja: "日本語"
  - ko: "한국어"
  - nl: "Nederlands"
  - pl: "Polski"
  - pt: "Português (Brasil)"
  - ru: "Русский"
  - tr: "Türkçe"
  - uk: "Українська"
  - zh-CN: "简体中文"
  - zh-TW: "繁體中文"
- groupTrigger: hover
  $name: "How to create a group"
  $name:de-DE: "Gruppe erstellen durch"
  $name:es-ES: "Cómo crear un grupo"
  $name:fr-FR: "Créer un groupe"
  $name:it-IT: "Come creare un gruppo"
  $name:ja-JP: "グループの作成方法"
  $name:ko-KR: "그룹 만드는 방법"
  $name:nl-NL: "Een groep maken door"
  $name:pl-PL: "Jak tworzyć grupę"
  $name:pt-BR: "Como criar um grupo"
  $name:ru-RU: "Как создать группу"
  $name:tr-TR: "Grup oluşturma yöntemi"
  $name:uk-UA: "Як створити групу"
  $name:zh-CN: "如何创建组"
  $name:zh-TW: "如何建立群組"
  $options:
  - hover: "Hover over an icon for a moment, then drop"
  - shift: "Hold Shift while dropping"
  - always: "Always group when dropping onto an icon"
  $options:de-DE:
  - hover: "Kurz über einem Symbol verweilen, dann ablegen"
  - shift: "Beim Ablegen Umschalt gedrückt halten"
  - always: "Immer beim Ablegen auf ein Symbol"
  $options:es-ES:
  - hover: "Mantener el puntero un momento sobre un icono y soltar"
  - shift: "Mantener Mayús al soltar"
  - always: "Agrupar siempre al soltar sobre un icono"
  $options:fr-FR:
  - hover: "Survoler une icône un instant, puis déposer"
  - shift: "Maintenir Maj en déposant"
  - always: "Toujours grouper en déposant sur une icône"
  $options:it-IT:
  - hover: "Tieni il puntatore su un'icona per un momento, poi rilascia"
  - shift: "Tieni premuto Maiusc mentre rilasci"
  - always: "Raggruppa sempre rilasciando su un'icona"
  $options:ja-JP:
  - hover: "アイコンの上で少し待ってからドロップ"
  - shift: "Shift キーを押しながらドロップ"
  - always: "アイコンにドロップすると常にグループ化"
  $options:ko-KR:
  - hover: "아이콘 위에 잠시 머문 뒤 놓기"
  - shift: "놓을 때 Shift 키 누르기"
  - always: "아이콘 위에 놓으면 항상 그룹화"
  $options:nl-NL:
  - hover: "Even boven een pictogram te blijven en dan los te laten"
  - shift: "Shift ingedrukt te houden bij het loslaten"
  - always: "Altijd te groeperen bij loslaten op een pictogram"
  $options:pl-PL:
  - hover: "Przytrzymaj chwilę nad ikoną, potem upuść"
  - shift: "Przytrzymaj Shift podczas upuszczania"
  - always: "Zawsze grupuj po upuszczeniu na ikonę"
  $options:pt-BR:
  - hover: "Parar sobre um ícone por um instante e soltar"
  - shift: "Segurar Shift ao soltar"
  - always: "Sempre agrupar ao soltar sobre um ícone"
  $options:ru-RU:
  - hover: "Задержать значок над другим, затем отпустить"
  - shift: "Удерживать Shift при отпускании"
  - always: "Всегда группировать при отпускании на значок"
  $options:tr-TR:
  - hover: "Bir simgenin üzerinde kısa süre bekleyip bırakın"
  - shift: "Bırakırken Shift tuşunu basılı tutun"
  - always: "Bir simgenin üzerine bırakınca her zaman gruplayın"
  $options:uk-UA:
  - hover: "Затримати значок над іншим, потім відпустити"
  - shift: "Утримувати Shift під час відпускання"
  - always: "Завжди групувати під час відпускання на значок"
  $options:zh-CN:
  - hover: "在图标上停留片刻后松开"
  - shift: "松开时按住 Shift"
  - always: "拖到图标上松开时总是创建组"
  $options:zh-TW:
  - hover: "在圖示上停留片刻後放開"
  - shift: "放開時按住 Shift"
  - always: "拖曳到圖示上放開時一律建立群組"
- hoverDelay: 600
  $name: "Hover time (milliseconds)"
  $name:de-DE: "Verweildauer (Millisekunden)"
  $name:es-ES: "Tiempo sobre el icono (milisegundos)"
  $name:fr-FR: "Durée de survol (millisecondes)"
  $name:it-IT: "Tempo di attesa (millisecondi)"
  $name:ja-JP: "待ち時間 (ミリ秒)"
  $name:ko-KR: "머무는 시간(밀리초)"
  $name:nl-NL: "Wachttijd (milliseconden)"
  $name:pl-PL: "Czas przytrzymania (milisekundy)"
  $name:pt-BR: "Tempo parado (milissegundos)"
  $name:ru-RU: "Время задержки (миллисекунды)"
  $name:tr-TR: "Bekleme süresi (milisaniye)"
  $name:uk-UA: "Час затримки (мілісекунди)"
  $name:zh-CN: "停留时间（毫秒）"
  $name:zh-TW: "停留時間（毫秒）"
  $description: "Only used with the \"hover\" option. 1000 = one second."
  $description:de-DE: "Nur für die Option „verweilen“. 1000 = eine Sekunde."
  $description:es-ES: "Solo con la opción de mantener el puntero. 1000 = un segundo."
  $description:fr-FR: "Uniquement avec l'option « survoler ». 1000 = une seconde."
  $description:it-IT: "Usato solo con l'opzione del puntatore. 1000 = un secondo."
  $description:ja-JP: "「少し待つ」オプションでのみ使用します。1000 = 1 秒。"
  $description:ko-KR: "\"잠시 머물기\" 옵션에서만 사용합니다. 1000 = 1초."
  $description:nl-NL: "Alleen bij de optie om even te blijven. 1000 = één seconde."
  $description:pl-PL: "Tylko przy opcji przytrzymania. 1000 = jedna sekunda."
  $description:pt-BR: "Usado só com a opção de parar sobre o ícone. 1000 = um segundo."
  $description:ru-RU: "Только для варианта с задержкой. 1000 = одна секунда."
  $description:tr-TR: "Yalnızca bekleme seçeneğinde kullanılır. 1000 = bir saniye."
  $description:uk-UA: "Лише для варіанта із затримкою. 1000 = одна секунда."
  $description:zh-CN: "仅用于“停留片刻”选项。1000 = 1 秒。"
  $description:zh-TW: "僅用於「停留片刻」選項。1000 = 1 秒。"
- openWith: single
  $name: "Open groups with"
  $name:de-DE: "Gruppen öffnen mit"
  $name:es-ES: "Abrir los grupos con"
  $name:fr-FR: "Ouvrir les groupes par"
  $name:it-IT: "Apri i gruppi con"
  $name:ja-JP: "グループを開く方法"
  $name:ko-KR: "그룹 여는 방법"
  $name:nl-NL: "Groepen openen met"
  $name:pl-PL: "Otwieraj grupy"
  $name:pt-BR: "Abrir grupos com"
  $name:ru-RU: "Открывать группы"
  $name:tr-TR: "Grupları açma yöntemi"
  $name:uk-UA: "Відкривати групи"
  $name:zh-CN: "打开组的方式"
  $name:zh-TW: "開啟群組的方式"
  $options:
  - single: "A single click, like Start menu folders"
  - double: "A double-click, like normal folders"
  - hover: "Hovering over them (a click opens them too)"
  $options:de-DE:
  - single: "Einfachklick, wie Ordner im Startmenü"
  - double: "Doppelklick, wie normale Ordner"
  - hover: "Darauf zeigen (ein Klick öffnet sie auch)"
  $options:es-ES:
  - single: "Un clic, como las carpetas del menú Inicio"
  - double: "Doble clic, como las carpetas normales"
  - hover: "Pasando el puntero por encima (un clic también los abre)"
  $options:fr-FR:
  - single: "Un clic, comme les dossiers du menu Démarrer"
  - double: "Un double-clic, comme les dossiers normaux"
  - hover: "Le survol (un clic les ouvre aussi)"
  $options:it-IT:
  - single: "Un clic, come le cartelle del menu Start"
  - double: "Un doppio clic, come le cartelle normali"
  - hover: "Passandoci sopra (anche un clic li apre)"
  $options:ja-JP:
  - single: "シングルクリック (スタート メニューのフォルダーと同じ)"
  - double: "ダブルクリック (通常のフォルダーと同じ)"
  - hover: "マウスを重ねる (クリックでも開きます)"
  $options:ko-KR:
  - single: "한 번 클릭(시작 메뉴 폴더처럼)"
  - double: "두 번 클릭(일반 폴더처럼)"
  - hover: "마우스 올리기(클릭해도 열림)"
  $options:nl-NL:
  - single: "Eén klik, zoals mappen in het Startmenu"
  - double: "Dubbelklikken, zoals gewone mappen"
  - hover: "Erboven zweven (een klik opent ze ook)"
  $options:pl-PL:
  - single: "Jednym kliknięciem, jak foldery w menu Start"
  - double: "Dwukrotnym kliknięciem, jak zwykłe foldery"
  - hover: "Po najechaniu kursorem (kliknięcie też je otwiera)"
  $options:pt-BR:
  - single: "Um clique, como as pastas do menu Iniciar"
  - double: "Clique duplo, como pastas normais"
  - hover: "Passando o mouse por cima (um clique também abre)"
  $options:ru-RU:
  - single: "Одним щелчком, как папки в меню «Пуск»"
  - double: "Двойным щелчком, как обычные папки"
  - hover: "При наведении (щелчок тоже открывает)"
  $options:tr-TR:
  - single: "Tek tıklama, Başlat menüsü klasörleri gibi"
  - double: "Çift tıklama, normal klasörler gibi"
  - hover: "Üzerine gelme (tıklama da açar)"
  $options:uk-UA:
  - single: "Одним клацанням, як папки в меню «Пуск»"
  - double: "Подвійним клацанням, як звичайні папки"
  - hover: "Під час наведення (клацання теж відкриває)"
  $options:zh-CN:
  - single: "单击，像“开始”菜单中的文件夹"
  - double: "双击，像普通文件夹"
  - hover: "鼠标悬停（单击也能打开）"
  $options:zh-TW:
  - single: "按一下，就像「開始」功能表中的資料夾"
  - double: "按兩下，就像一般資料夾"
  - hover: "滑鼠停留（按一下也能開啟）"
- openHoverDelay: 400
  $name: "Hover time before opening (milliseconds)"
  $name:de-DE: "Verweildauer vor dem Öffnen (Millisekunden)"
  $name:es-ES: "Tiempo antes de abrir al pasar el puntero (milisegundos)"
  $name:fr-FR: "Durée de survol avant ouverture (millisecondes)"
  $name:it-IT: "Tempo prima dell'apertura al passaggio (millisecondi)"
  $name:ja-JP: "開くまでの待ち時間 (ミリ秒)"
  $name:ko-KR: "열기 전 머무는 시간(밀리초)"
  $name:nl-NL: "Wachttijd voor openen bij zweven (milliseconden)"
  $name:pl-PL: "Czas przed otwarciem po najechaniu (milisekundy)"
  $name:pt-BR: "Tempo antes de abrir ao passar o mouse (milissegundos)"
  $name:ru-RU: "Время наведения перед открытием (миллисекунды)"
  $name:tr-TR: "Üzerine gelince açılmadan önceki süre (milisaniye)"
  $name:uk-UA: "Час наведення перед відкриттям (мілісекунди)"
  $name:zh-CN: "悬停多久后打开（毫秒）"
  $name:zh-TW: "停留多久後開啟（毫秒）"
  $description: "Only used when opening by hovering. A popup opened this way closes again when the mouse moves away from it, unless you click inside it first."
  $description:de-DE: "Nur beim Öffnen durch Darauf-zeigen. Ein so geöffnetes Popup schließt sich wieder, wenn die Maus es verlässt, außer man klickt vorher hinein."
  $description:es-ES: "Solo al abrir pasando el puntero. Una ventana abierta así se cierra al apartar el ratón, salvo que antes hagas clic dentro."
  $description:fr-FR: "Uniquement pour l'ouverture au survol. Une fenêtre ouverte ainsi se referme quand la souris s'en éloigne, sauf si vous cliquez dedans avant."
  $description:it-IT: "Usato solo per l'apertura al passaggio del puntatore. Un popup aperto così si richiude quando il mouse si allontana, a meno che tu non ci clicchi dentro prima."
  $description:ja-JP: "マウスを重ねて開く場合のみ使用します。この方法で開いたポップアップは、先に中をクリックしない限り、マウスが離れると閉じます。"
  $description:ko-KR: "마우스를 올려 열 때만 사용합니다. 이렇게 연 팝업은 먼저 안을 클릭하지 않으면 마우스가 벗어날 때 닫힙니다."
  $description:nl-NL: "Alleen bij openen door te zweven. Een pop-up die zo geopend is, sluit weer als de muis weggaat, tenzij je er eerst in klikt."
  $description:pl-PL: "Tylko przy otwieraniu po najechaniu. Tak otwarte okienko zamyka się, gdy kursor się oddali, chyba że najpierw w nie klikniesz."
  $description:pt-BR: "Usado só ao abrir passando o mouse. Uma janela aberta assim fecha quando o mouse se afasta, a menos que você clique dentro dela antes."
  $description:ru-RU: "Только при открытии наведением. Окно, открытое так, закрывается, когда мышь уходит с него, если только вы сначала не щёлкнете внутри."
  $description:tr-TR: "Yalnızca üzerine gelerek açarken kullanılır. Bu şekilde açılan pencere, önce içine tıklamazsanız fare uzaklaşınca kapanır."
  $description:uk-UA: "Лише під час відкриття наведенням. Вікно, відкрите так, закривається, коли миша відходить від нього, якщо ви спершу не клацнете всередині."
  $description:zh-CN: "仅用于悬停打开。以这种方式打开的弹出窗口在鼠标移开时会关闭，除非你先在其中单击。"
  $description:zh-TW: "僅用於滑鼠停留開啟。以這種方式開啟的快顯視窗會在滑鼠移開時關閉，除非你先在其中按一下。"
- openAnyFolder: false
  $name: "Open other desktop folders in the popup too"
  $name:de-DE: "Auch andere Desktopordner im Popup öffnen"
  $name:es-ES: "Abrir también otras carpetas del escritorio en la ventana emergente"
  $name:fr-FR: "Ouvrir aussi les autres dossiers du bureau dans la fenêtre"
  $name:it-IT: "Apri anche le altre cartelle del desktop nel popup"
  $name:ja-JP: "デスクトップの他のフォルダーもポップアップで開く"
  $name:ko-KR: "바탕 화면의 다른 폴더도 팝업으로 열기"
  $name:nl-NL: "Ook andere bureaubladmappen in de pop-up openen"
  $name:pl-PL: "Otwieraj w okienku także inne foldery z pulpitu"
  $name:pt-BR: "Abrir também outras pastas da área de trabalho na janela"
  $name:ru-RU: "Открывать во всплывающем окне и другие папки рабочего стола"
  $name:tr-TR: "Diğer masaüstü klasörlerini de açılır pencerede aç"
  $name:uk-UA: "Відкривати у спливному вікні й інші папки робочого столу"
  $name:zh-CN: "在弹出窗口中也打开其他桌面文件夹"
  $name:zh-TW: "在快顯視窗中也開啟其他桌面資料夾"
  $description: "Folders of your own, not just groups, open in the same popup, with the same options. Their icons aren't changed."
  $description:de-DE: "Eigene Ordner, nicht nur Gruppen, öffnen sich im selben Popup mit denselben Optionen. Ihre Symbole bleiben unverändert."
  $description:es-ES: "Tus propias carpetas, no solo los grupos, se abren en la misma ventana con las mismas opciones. Sus iconos no cambian."
  $description:fr-FR: "Vos propres dossiers, pas seulement les groupes, s'ouvrent dans la même fenêtre, avec les mêmes options. Leurs icônes ne changent pas."
  $description:it-IT: "Le tue cartelle, non solo i gruppi, si aprono nello stesso popup con le stesse opzioni. Le loro icone non cambiano."
  $description:ja-JP: "グループだけでなく自分のフォルダーも、同じポップアップと同じオプションで開きます。アイコンは変更されません。"
  $description:ko-KR: "그룹뿐 아니라 내 폴더도 같은 팝업에서 같은 옵션으로 열립니다. 아이콘은 바뀌지 않습니다."
  $description:nl-NL: "Je eigen mappen, niet alleen groepen, openen in dezelfde pop-up met dezelfde opties. Hun pictogrammen veranderen niet."
  $description:pl-PL: "Twoje własne foldery, nie tylko grupy, otwierają się w tym samym okienku z tymi samymi opcjami. Ich ikony się nie zmieniają."
  $description:pt-BR: "Suas próprias pastas, não só os grupos, abrem na mesma janela, com as mesmas opções. Os ícones delas não mudam."
  $description:ru-RU: "Ваши собственные папки, а не только группы, открываются в том же окне с теми же параметрами. Их значки не меняются."
  $description:tr-TR: "Yalnızca gruplar değil kendi klasörleriniz de aynı pencerede aynı seçeneklerle açılır. Simgeleri değişmez."
  $description:uk-UA: "Ваші власні папки, а не лише групи, відкриваються в тому самому вікні з тими самими параметрами. Їхні значки не змінюються."
  $description:zh-CN: "不只是组，你自己的文件夹也会在同一个弹出窗口中以相同选项打开。它们的图标不会改变。"
  $description:zh-TW: "不只是群組，你自己的資料夾也會在同一個快顯視窗中以相同選項開啟。它們的圖示不會改變。"
- maxIconRows: 5
  $name: "Rows of icons before scrolling"
  $name:de-DE: "Symbolzeilen vor dem Scrollen"
  $name:es-ES: "Filas de iconos antes de desplazarse"
  $name:fr-FR: "Lignes d'icônes avant le défilement"
  $name:it-IT: "Righe di icone prima dello scorrimento"
  $name:ja-JP: "スクロールまでのアイコンの行数"
  $name:ko-KR: "스크롤 전 아이콘 행 수"
  $name:nl-NL: "Rijen pictogrammen voor het schuiven"
  $name:pl-PL: "Wiersze ikon przed przewijaniem"
  $name:pt-BR: "Linhas de ícones antes de rolar"
  $name:ru-RU: "Строк значков до прокрутки"
  $name:tr-TR: "Kaydırmadan önceki simge satırı sayısı"
  $name:uk-UA: "Рядків значків до прокручування"
  $name:zh-CN: "滚动前的图标行数"
  $name:zh-TW: "捲動前的圖示列數"
  $description: "The popup shows up to this many rows of icons, then scrolls. It never grows taller than the screen."
  $description:de-DE: "Das Popup zeigt bis zu so viele Symbolzeilen und scrollt dann. Es wird nie höher als der Bildschirm."
  $description:es-ES: "La ventana muestra hasta este número de filas de iconos y luego se desplaza. Nunca es más alta que la pantalla."
  $description:fr-FR: "La fenêtre affiche jusqu'à ce nombre de lignes d'icônes, puis défile. Elle ne dépasse jamais la hauteur de l'écran."
  $description:it-IT: "Il popup mostra fino a questo numero di righe di icone, poi scorre. Non diventa mai più alto dello schermo."
  $description:ja-JP: "ポップアップはこの行数までアイコンを表示し、それ以上はスクロールします。画面より高くなることはありません。"
  $description:ko-KR: "팝업은 이 행 수까지 아이콘을 보여 주고 그 이상은 스크롤합니다. 화면보다 커지지 않습니다."
  $description:nl-NL: "De pop-up toont tot zoveel rijen pictogrammen en schuift daarna. Hij wordt nooit hoger dan het scherm."
  $description:pl-PL: "Okienko pokazuje do tylu wierszy ikon, a potem się przewija. Nigdy nie jest wyższe niż ekran."
  $description:pt-BR: "A janela mostra até este número de linhas de ícones e depois rola. Nunca fica mais alta que a tela."
  $description:ru-RU: "Окно показывает до стольких строк значков, а дальше прокручивается. Оно никогда не бывает выше экрана."
  $description:tr-TR: "Pencere bu kadar simge satırı gösterir, sonra kaydırılır. Hiçbir zaman ekrandan uzun olmaz."
  $description:uk-UA: "Вікно показує до стількох рядків значків, а далі прокручується. Воно ніколи не буває вищим за екран."
  $description:zh-CN: "弹出窗口最多显示这么多行图标，超出后滚动。它永远不会比屏幕高。"
  $description:zh-TW: "快顯視窗最多顯示這麼多列圖示，超過後會捲動。它永遠不會比螢幕高。"
- maxListRows: 12
  $name: "Rows in the list view before scrolling"
  $name:de-DE: "Zeilen in der Listenansicht vor dem Scrollen"
  $name:es-ES: "Filas en la vista de lista antes de desplazarse"
  $name:fr-FR: "Lignes en affichage liste avant le défilement"
  $name:it-IT: "Righe in visualizzazione elenco prima dello scorrimento"
  $name:ja-JP: "一覧表示でスクロールまでの行数"
  $name:ko-KR: "목록 보기에서 스크롤 전 행 수"
  $name:nl-NL: "Rijen in de lijstweergave voor het schuiven"
  $name:pl-PL: "Wiersze w widoku listy przed przewijaniem"
  $name:pt-BR: "Linhas na exibição em lista antes de rolar"
  $name:ru-RU: "Строк в виде списка до прокрутки"
  $name:tr-TR: "Liste görünümünde kaydırmadan önceki satır sayısı"
  $name:uk-UA: "Рядків у вигляді списку до прокручування"
  $name:zh-CN: "列表视图中滚动前的行数"
  $name:zh-TW: "清單檢視中捲動前的列數"
  $description: "The same, for the list view."
  $description:de-DE: "Dasselbe für die Listenansicht."
  $description:es-ES: "Lo mismo, para la vista de lista."
  $description:fr-FR: "Même chose, pour l'affichage en liste."
  $description:it-IT: "Lo stesso, per la visualizzazione elenco."
  $description:ja-JP: "一覧表示の場合の同じ設定です。"
  $description:ko-KR: "목록 보기에 대한 같은 설정입니다."
  $description:nl-NL: "Hetzelfde, voor de lijstweergave."
  $description:pl-PL: "To samo dla widoku listy."
  $description:pt-BR: "O mesmo, para a exibição em lista."
  $description:ru-RU: "То же самое для вида списка."
  $description:tr-TR: "Liste görünümü için aynısı."
  $description:uk-UA: "Те саме для вигляду списку."
  $description:zh-CN: "列表视图的相同设置。"
  $description:zh-TW: "清單檢視的相同設定。"
- blurredPopup: true
  $name: "Blurred popup background"
  $name:de-DE: "Unscharfer Popup-Hintergrund"
  $name:es-ES: "Fondo desenfocado"
  $name:fr-FR: "Arrière-plan flou"
  $name:it-IT: "Sfondo sfocato"
  $name:ja-JP: "ぼかした背景"
  $name:ko-KR: "흐린 팝업 배경"
  $name:nl-NL: "Vervaagde achtergrond"
  $name:pl-PL: "Rozmyte tło"
  $name:pt-BR: "Fundo desfocado"
  $name:ru-RU: "Размытый фон"
  $name:tr-TR: "Bulanık arka plan"
  $name:uk-UA: "Розмитий фон"
  $name:zh-CN: "模糊背景"
  $name:zh-TW: "模糊背景"
  $description: "A see-through, blurred background like the Start menu's. Uses a solid background instead if Windows' Transparency effects setting is off, or with a high contrast theme."
  $description:de-DE: "Ein durchscheinender, unscharfer Hintergrund wie im Startmenü. Ist die Windows-Einstellung „Transparenzeffekte“ aus oder ein Kontrastdesign aktiv, wird ein einfarbiger Hintergrund verwendet."
  $description:es-ES: "Un fondo translúcido y desenfocado como el del menú Inicio. Usa un fondo sólido si los efectos de transparencia de Windows están desactivados o con un tema de contraste alto."
  $description:fr-FR: "Un arrière-plan translucide et flou comme celui du menu Démarrer. Un fond uni est utilisé si les effets de transparence de Windows sont désactivés, ou avec un thème à contraste élevé."
  $description:it-IT: "Uno sfondo trasparente e sfocato come quello del menu Start. Usa uno sfondo pieno se gli effetti di trasparenza di Windows sono disattivati o con un tema a contrasto elevato."
  $description:ja-JP: "スタート メニューのような半透明のぼかし背景です。Windows の透明効果がオフの場合やハイ コントラスト テーマでは、単色の背景になります。"
  $description:ko-KR: "시작 메뉴처럼 반투명하고 흐린 배경입니다. Windows 투명 효과가 꺼져 있거나 고대비 테마에서는 단색 배경을 사용합니다."
  $description:nl-NL: "Een doorschijnende, vervaagde achtergrond zoals die van het Startmenu. Gebruikt een effen achtergrond als de transparantie-effecten van Windows uit staan, of met een thema met hoog contrast."
  $description:pl-PL: "Przezroczyste, rozmyte tło jak w menu Start. Używa jednolitego tła, gdy efekty przezroczystości Windows są wyłączone lub przy motywie o dużym kontraście."
  $description:pt-BR: "Um fundo translúcido e desfocado como o do menu Iniciar. Usa um fundo sólido se os efeitos de transparência do Windows estiverem desativados ou com um tema de alto contraste."
  $description:ru-RU: "Полупрозрачный размытый фон, как у меню «Пуск». Если эффекты прозрачности Windows выключены или включена контрастная тема, используется сплошной фон."
  $description:tr-TR: "Başlat menüsündeki gibi yarı saydam, bulanık bir arka plan. Windows saydamlık efektleri kapalıysa veya yüksek karşıtlık teması kullanılıyorsa düz bir arka plan kullanılır."
  $description:uk-UA: "Напівпрозорий розмитий фон, як у меню «Пуск». Якщо ефекти прозорості Windows вимкнено або ввімкнено контрастну тему, використовується суцільний фон."
  $description:zh-CN: "像“开始”菜单那样的半透明模糊背景。如果 Windows 的透明效果已关闭或使用高对比度主题，则使用纯色背景。"
  $description:zh-TW: "像「開始」功能表那樣的半透明模糊背景。如果 Windows 的透明效果已關閉或使用高對比佈景主題，則使用純色背景。"
- popupOpacity: 60
  $name: "Background colour strength (%)"
  $name:de-DE: "Farbstärke des Hintergrunds (%)"
  $name:es-ES: "Intensidad del color de fondo (%)"
  $name:fr-FR: "Intensité de la couleur de fond (%)"
  $name:it-IT: "Intensità del colore di sfondo (%)"
  $name:ja-JP: "背景色の濃さ (%)"
  $name:ko-KR: "배경색 강도(%)"
  $name:nl-NL: "Sterkte van de achtergrondkleur (%)"
  $name:pl-PL: "Intensywność koloru tła (%)"
  $name:pt-BR: "Intensidade da cor de fundo (%)"
  $name:ru-RU: "Насыщенность цвета фона (%)"
  $name:tr-TR: "Arka plan rengi yoğunluğu (%)"
  $name:uk-UA: "Насиченість кольору фону (%)"
  $name:zh-CN: "背景颜色强度（%）"
  $name:zh-TW: "背景色彩強度（%）"
  $description: "How much colour is laid over the blurred background, from 0 (most see-through) to 100 (solid)."
  $description:de-DE: "Wie viel Farbe über dem unscharfen Hintergrund liegt, von 0 (am durchsichtigsten) bis 100 (deckend)."
  $description:es-ES: "Cuánto color se pone sobre el fondo desenfocado, de 0 (más transparente) a 100 (sólido)."
  $description:fr-FR: "La quantité de couleur posée sur l'arrière-plan flou, de 0 (le plus transparent) à 100 (opaque)."
  $description:it-IT: "Quanto colore viene steso sullo sfondo sfocato, da 0 (più trasparente) a 100 (pieno)."
  $description:ja-JP: "ぼかした背景に重ねる色の量です。0 (最も透明) から 100 (不透明) まで。"
  $description:ko-KR: "흐린 배경 위에 얹는 색의 양으로, 0(가장 투명)부터 100(불투명)까지입니다."
  $description:nl-NL: "Hoeveel kleur over de vervaagde achtergrond ligt, van 0 (het meest doorschijnend) tot 100 (effen)."
  $description:pl-PL: "Ile koloru nakłada się na rozmyte tło, od 0 (najbardziej przezroczyste) do 100 (jednolite)."
  $description:pt-BR: "Quanta cor é aplicada sobre o fundo desfocado, de 0 (mais transparente) a 100 (sólido)."
  $description:ru-RU: "Сколько цвета накладывается на размытый фон: от 0 (самый прозрачный) до 100 (сплошной)."
  $description:tr-TR: "Bulanık arka planın üzerine ne kadar renk konacağı; 0 (en saydam) ile 100 (düz) arası."
  $description:uk-UA: "Скільки кольору накладається на розмитий фон: від 0 (найпрозоріший) до 100 (суцільний)."
  $description:zh-CN: "叠加在模糊背景上的颜色多少，从 0（最透明）到 100（纯色）。"
  $description:zh-TW: "疊加在模糊背景上的色彩多寡，從 0（最透明）到 100（純色）。"
- animatePopup: true
  $name: "Animate the popup"
  $name:de-DE: "Popup animieren"
  $name:es-ES: "Animar la ventana emergente"
  $name:fr-FR: "Animer la fenêtre"
  $name:it-IT: "Anima il popup"
  $name:ja-JP: "ポップアップのアニメーション"
  $name:ko-KR: "팝업 애니메이션"
  $name:nl-NL: "Pop-up animeren"
  $name:pl-PL: "Animuj okienko"
  $name:pt-BR: "Animar a janela"
  $name:ru-RU: "Анимировать всплывающее окно"
  $name:tr-TR: "Açılır pencereyi canlandır"
  $name:uk-UA: "Анімувати спливне вікно"
  $name:zh-CN: "弹出窗口动画"
  $name:zh-TW: "快顯視窗動畫"
  $description: "Grow out of the group's icon when opening, and shrink back into it when closing, like Start menu folders."
  $description:de-DE: "Beim Öffnen aus dem Gruppensymbol herauswachsen und beim Schließen wieder hineinschrumpfen, wie Ordner im Startmenü."
  $description:es-ES: "Crece desde el icono del grupo al abrirse y vuelve a él al cerrarse, como las carpetas del menú Inicio."
  $description:fr-FR: "Elle sort de l'icône du groupe à l'ouverture et y retourne à la fermeture, comme les dossiers du menu Démarrer."
  $description:it-IT: "Esce dall'icona del gruppo all'apertura e vi rientra alla chiusura, come le cartelle del menu Start."
  $description:ja-JP: "スタート メニューのフォルダーのように、開くときはグループのアイコンから広がり、閉じるときはアイコンに戻ります。"
  $description:ko-KR: "시작 메뉴 폴더처럼 열 때는 그룹 아이콘에서 커지고 닫을 때는 다시 줄어듭니다."
  $description:nl-NL: "Groeit bij openen uit het groepspictogram en krimpt bij sluiten weer terug, zoals mappen in het Startmenu."
  $description:pl-PL: "Wyrasta z ikony grupy przy otwieraniu i wraca do niej przy zamykaniu, jak foldery w menu Start."
  $description:pt-BR: "Cresce a partir do ícone do grupo ao abrir e volta para ele ao fechar, como as pastas do menu Iniciar."
  $description:ru-RU: "При открытии вырастает из значка группы, а при закрытии сжимается обратно, как папки в меню «Пуск»."
  $description:tr-TR: "Başlat menüsü klasörleri gibi açılırken grup simgesinden büyür, kapanırken ona geri küçülür."
  $description:uk-UA: "Під час відкриття виростає зі значка групи, а під час закриття стискається назад, як папки в меню «Пуск»."
  $description:zh-CN: "像“开始”菜单中的文件夹一样，打开时从组图标中展开，关闭时缩回。"
  $description:zh-TW: "像「開始」功能表中的資料夾一樣，開啟時從群組圖示展開，關閉時縮回。"
- animationDuration: 200
  $name: "Animation length (milliseconds)"
  $name:de-DE: "Animationsdauer (Millisekunden)"
  $name:es-ES: "Duración de la animación (milisegundos)"
  $name:fr-FR: "Durée de l'animation (millisecondes)"
  $name:it-IT: "Durata dell'animazione (millisecondi)"
  $name:ja-JP: "アニメーションの長さ (ミリ秒)"
  $name:ko-KR: "애니메이션 길이(밀리초)"
  $name:nl-NL: "Duur van de animatie (milliseconden)"
  $name:pl-PL: "Długość animacji (milisekundy)"
  $name:pt-BR: "Duração da animação (milissegundos)"
  $name:ru-RU: "Длительность анимации (миллисекунды)"
  $name:tr-TR: "Animasyon süresi (milisaniye)"
  $name:uk-UA: "Тривалість анімації (мілісекунди)"
  $name:zh-CN: "动画时长（毫秒）"
  $name:zh-TW: "動畫長度（毫秒）"
  $description: "How long the popup takes to open. Closing takes three quarters of this time."
  $description:de-DE: "Wie lange das Öffnen des Popups dauert. Das Schließen dauert drei Viertel davon."
  $description:es-ES: "Cuánto tarda en abrirse la ventana. Al cerrarse tarda tres cuartas partes de ese tiempo."
  $description:fr-FR: "Le temps que met la fenêtre à s'ouvrir. La fermeture prend les trois quarts de ce temps."
  $description:it-IT: "Quanto impiega il popup ad aprirsi. La chiusura richiede tre quarti di questo tempo."
  $description:ja-JP: "ポップアップが開くまでの時間です。閉じるときはその 4 分の 3 の時間です。"
  $description:ko-KR: "팝업이 열리는 데 걸리는 시간입니다. 닫힐 때는 이 시간의 4분의 3이 걸립니다."
  $description:nl-NL: "Hoe lang het openen van de pop-up duurt. Sluiten duurt driekwart van die tijd."
  $description:pl-PL: "Jak długo otwiera się okienko. Zamykanie trwa trzy czwarte tego czasu."
  $description:pt-BR: "Quanto tempo a janela leva para abrir. Fechar leva três quartos desse tempo."
  $description:ru-RU: "Сколько времени открывается окно. Закрытие занимает три четверти этого времени."
  $description:tr-TR: "Pencerenin açılmasının ne kadar sürdüğü. Kapanma bu sürenin dörtte üçü kadar sürer."
  $description:uk-UA: "Скільки часу відкривається вікно. Закриття триває три чверті цього часу."
  $description:zh-CN: "弹出窗口打开所需的时间。关闭所需时间为其四分之三。"
  $description:zh-TW: "快顯視窗開啟所需的時間。關閉所需時間為其四分之三。"
- groupDataInFolder: false
  $name: "Keep group data inside the group folders (for syncing)"
  $name:de-DE: "Gruppendaten in den Gruppenordnern speichern (zum Synchronisieren)"
  $name:es-ES: "Guardar los datos de los grupos en sus carpetas (para sincronizar)"
  $name:fr-FR: "Garder les données des groupes dans leurs dossiers (pour la synchronisation)"
  $name:it-IT: "Conserva i dati dei gruppi nelle loro cartelle (per la sincronizzazione)"
  $name:ja-JP: "グループのデータをグループのフォルダー内に保存する (同期用)"
  $name:ko-KR: "그룹 데이터를 그룹 폴더 안에 저장(동기화용)"
  $name:nl-NL: "Groepsgegevens in de groepsmappen bewaren (voor synchronisatie)"
  $name:pl-PL: "Przechowuj dane grup w ich folderach (do synchronizacji)"
  $name:pt-BR: "Guardar os dados dos grupos nas pastas deles (para sincronizar)"
  $name:ru-RU: "Хранить данные групп в их папках (для синхронизации)"
  $name:tr-TR: "Grup verilerini grup klasörlerinin içinde tut (eşitleme için)"
  $name:uk-UA: "Зберігати дані груп у їхніх папках (для синхронізації)"
  $name:zh-CN: "将组数据保存在组文件夹中（用于同步）"
  $name:zh-TW: "將群組資料保存在群組資料夾中（用於同步）"
  $description: "Off (recommended): each group's app order, its own settings and its group status are kept in Windhawk's storage on this PC. On: they're kept in the hidden desktop.ini file inside each group instead, so they go with the folder when your desktop is synced (for example by OneDrive), copied or backed up, and the folders are still groups on another PC or after reinstalling the mod. Side effects of turning it on: the mod writes to that file every time you rearrange a group or change its settings, so syncing services upload it again each time; and the data stays in the folders if you uninstall the mod. Changing this moves your existing groups' data over."
  $description:de-DE: "Aus (empfohlen): Reihenfolge, eigene Einstellungen und Gruppenstatus jeder Gruppe werden im Windhawk-Speicher auf diesem PC abgelegt. Ein: Sie werden stattdessen in der versteckten Datei desktop.ini in jeder Gruppe gespeichert. So wandern sie mit dem Ordner, wenn der Desktop synchronisiert (etwa mit OneDrive), kopiert oder gesichert wird, und die Ordner bleiben auch auf einem anderen PC oder nach einer Neuinstallation des Mods Gruppen. Nebenwirkungen: Der Mod schreibt bei jedem Umordnen oder Ändern der Einstellungen in diese Datei, sodass Synchronisierungsdienste sie jedes Mal neu hochladen; und die Daten bleiben nach dem Deinstallieren des Mods in den Ordnern. Beim Umschalten werden die Daten vorhandener Gruppen übertragen."
  $description:es-ES: "Desactivado (recomendado): el orden, la configuración propia y el estado de grupo de cada grupo se guardan en el almacenamiento de Windhawk en este PC. Activado: se guardan en el archivo oculto desktop.ini de cada grupo, de modo que viajan con la carpeta cuando el escritorio se sincroniza (por ejemplo con OneDrive), se copia o se respalda, y las carpetas siguen siendo grupos en otro PC o tras reinstalar el mod. Efectos al activarlo: el mod escribe en ese archivo cada vez que reorganizas un grupo o cambias su configuración, por lo que los servicios de sincronización lo vuelven a subir cada vez; y los datos permanecen en las carpetas si desinstalas el mod. Al cambiar esta opción se trasladan los datos de los grupos existentes."
  $description:fr-FR: "Désactivé (recommandé) : l'ordre, les paramètres propres et le statut de groupe de chaque groupe sont conservés dans le stockage de Windhawk sur ce PC. Activé : ils sont conservés dans le fichier caché desktop.ini de chaque groupe, et suivent donc le dossier quand le bureau est synchronisé (par exemple avec OneDrive), copié ou sauvegardé ; les dossiers restent des groupes sur un autre PC ou après réinstallation du mod. Effets secondaires : le mod écrit dans ce fichier chaque fois que vous réorganisez un groupe ou changez ses paramètres, si bien que les services de synchronisation le renvoient à chaque fois ; et les données restent dans les dossiers si vous désinstallez le mod. Changer ce réglage déplace les données des groupes existants."
  $description:it-IT: "Disattivato (consigliato): l'ordine, le impostazioni proprie e lo stato di gruppo di ogni gruppo sono conservati nell'archivio di Windhawk su questo PC. Attivato: sono conservati nel file nascosto desktop.ini di ogni gruppo, quindi seguono la cartella quando il desktop viene sincronizzato (ad esempio con OneDrive), copiato o salvato, e le cartelle restano gruppi su un altro PC o dopo aver reinstallato la mod. Effetti collaterali: la mod scrive in quel file ogni volta che riordini un gruppo o ne cambi le impostazioni, quindi i servizi di sincronizzazione lo ricaricano ogni volta; e i dati restano nelle cartelle se disinstalli la mod. Cambiando questa opzione i dati dei gruppi esistenti vengono spostati."
  $description:ja-JP: "オフ (推奨): 各グループの並び順、独自の設定、グループであることは、この PC の Windhawk のストレージに保存されます。オン: 代わりに各グループ内の隠しファイル desktop.ini に保存されるため、デスクトップを同期 (OneDrive など)、コピー、バックアップしたときにフォルダーと一緒に移動し、別の PC や mod の再インストール後もグループのままになります。オンにした場合の影響: グループを並べ替えたり設定を変えたりするたびに mod がこのファイルに書き込むため、同期サービスが毎回アップロードし直します。また、mod をアンインストールしてもデータはフォルダーに残ります。この設定を変えると、既存のグループのデータが移動されます。"
  $description:ko-KR: "끔(권장): 각 그룹의 앱 순서, 자체 설정, 그룹 여부는 이 PC의 Windhawk 저장소에 보관됩니다. 켬: 대신 각 그룹 안의 숨김 파일 desktop.ini에 보관되므로, 바탕 화면을 동기화(예: OneDrive), 복사, 백업할 때 폴더와 함께 이동하고 다른 PC나 모드를 다시 설치한 뒤에도 그룹으로 남습니다. 켰을 때의 부작용: 그룹을 다시 정렬하거나 설정을 바꿀 때마다 모드가 이 파일에 쓰므로 동기화 서비스가 매번 다시 업로드합니다. 또한 모드를 제거해도 데이터가 폴더에 남습니다. 이 설정을 바꾸면 기존 그룹의 데이터가 옮겨집니다."
  $description:nl-NL: "Uit (aanbevolen): de volgorde, eigen instellingen en groepsstatus van elke groep worden bewaard in de opslag van Windhawk op deze pc. Aan: ze worden in plaats daarvan bewaard in het verborgen bestand desktop.ini in elke groep, zodat ze met de map meegaan als je bureaublad wordt gesynchroniseerd (bijvoorbeeld met OneDrive), gekopieerd of geback-upt, en de mappen ook op een andere pc of na het opnieuw installeren van de mod groepen blijven. Bijwerkingen: de mod schrijft naar dat bestand telkens als je een groep herschikt of de instellingen wijzigt, dus synchronisatiediensten uploaden het elke keer opnieuw; en de gegevens blijven in de mappen als je de mod verwijdert. Als je dit wijzigt, worden de gegevens van bestaande groepen overgezet."
  $description:pl-PL: "Wyłączone (zalecane): kolejność, własne ustawienia i status grupy są przechowywane w magazynie Windhawk na tym komputerze. Włączone: są przechowywane w ukrytym pliku desktop.ini w każdej grupie, więc przenoszą się z folderem, gdy pulpit jest synchronizowany (np. przez OneDrive), kopiowany lub archiwizowany, a foldery pozostają grupami na innym komputerze lub po ponownej instalacji moda. Skutki uboczne: mod zapisuje ten plik przy każdej zmianie kolejności lub ustawień grupy, więc usługi synchronizacji za każdym razem wysyłają go ponownie; a dane zostają w folderach po odinstalowaniu moda. Zmiana tej opcji przenosi dane istniejących grup."
  $description:pt-BR: "Desativado (recomendado): a ordem, as configurações próprias e o status de grupo de cada grupo ficam no armazenamento do Windhawk neste PC. Ativado: ficam no arquivo oculto desktop.ini dentro de cada grupo, então acompanham a pasta quando a área de trabalho é sincronizada (por exemplo pelo OneDrive), copiada ou salva em backup, e as pastas continuam sendo grupos em outro PC ou depois de reinstalar o mod. Efeitos colaterais: o mod grava nesse arquivo toda vez que você reorganiza um grupo ou muda as configurações dele, então os serviços de sincronização o enviam de novo a cada vez; e os dados ficam nas pastas se você desinstalar o mod. Mudar esta opção transfere os dados dos grupos existentes."
  $description:ru-RU: "Выключено (рекомендуется): порядок, собственные настройки и признак группы хранятся в хранилище Windhawk на этом ПК. Включено: они хранятся в скрытом файле desktop.ini внутри каждой группы, поэтому переносятся вместе с папкой при синхронизации рабочего стола (например, через OneDrive), копировании или резервном копировании, и папки остаются группами на другом ПК или после переустановки мода. Побочные эффекты: мод записывает этот файл при каждом изменении порядка или настроек группы, поэтому службы синхронизации каждый раз загружают его заново; а данные остаются в папках после удаления мода. При изменении этого параметра данные существующих групп переносятся."
  $description:tr-TR: "Kapalı (önerilir): her grubun uygulama sırası, kendi ayarları ve grup durumu bu bilgisayardaki Windhawk depolamasında tutulur. Açık: bunun yerine her grubun içindeki gizli desktop.ini dosyasında tutulur; böylece masaüstünüz eşitlendiğinde (örneğin OneDrive ile), kopyalandığında veya yedeklendiğinde klasörle birlikte taşınır ve klasörler başka bir bilgisayarda ya da modu yeniden yükledikten sonra da grup olarak kalır. Yan etkileri: bir grubu her yeniden düzenlediğinizde veya ayarlarını değiştirdiğinizde mod bu dosyaya yazar, bu yüzden eşitleme hizmetleri onu her seferinde yeniden yükler; ayrıca modu kaldırsanız da veriler klasörlerde kalır. Bu ayarı değiştirmek mevcut grupların verilerini taşır."
  $description:uk-UA: "Вимкнено (рекомендовано): порядок, власні налаштування й ознака групи зберігаються в сховищі Windhawk на цьому ПК. Увімкнено: вони зберігаються в прихованому файлі desktop.ini всередині кожної групи, тому переносяться разом із папкою під час синхронізації робочого столу (наприклад, через OneDrive), копіювання чи резервного копіювання, і папки лишаються групами на іншому ПК або після перевстановлення мода. Побічні ефекти: мод записує цей файл щоразу, коли ви змінюєте порядок чи налаштування групи, тож служби синхронізації щоразу завантажують його знову; а дані лишаються в папках після видалення мода. Зміна цього параметра переносить дані наявних груп."
  $description:zh-CN: "关闭（推荐）：每个组的应用顺序、自身设置和组状态保存在此电脑上的 Windhawk 存储中。开启：改为保存在每个组内的隐藏文件 desktop.ini 中，因此在桌面同步（例如通过 OneDrive）、复制或备份时会随文件夹一起移动，并且在另一台电脑上或重新安装本模组后，这些文件夹仍然是组。开启后的副作用：每次重新排列组或更改其设置时，模组都会写入该文件，因此同步服务每次都会重新上传；卸载模组后数据也会留在文件夹中。更改此设置会迁移现有组的数据。"
  $description:zh-TW: "關閉（建議）：每個群組的應用程式順序、自身設定和群組狀態保存在這台電腦的 Windhawk 儲存空間中。開啟：改為保存在每個群組內的隱藏檔案 desktop.ini 中，因此在桌面同步（例如透過 OneDrive）、複製或備份時會隨資料夾一起移動，而且在另一台電腦上或重新安裝此模組後，這些資料夾仍然是群組。開啟後的副作用：每次重新排列群組或變更其設定時，模組都會寫入該檔案，因此同步服務每次都會重新上傳；解除安裝模組後資料也會留在資料夾中。變更此設定會移轉現有群組的資料。"
*/
// ==/WindhawkModSettings==

// -----------------------------------------------------------------------------
// HOW THIS FILE IS ORGANISED
//
// 1. Settings      - values the user can change in Windhawk's Settings tab
// 2. Finding       - locating the desktop's icon list inside Explorer
// 3. Reading       - asking that list for icon names, positions, selection
// 4. Desktop icons as files - finding the real file behind each icon
// 5. Groups        - creating groups, recognising them, listing their apps
// 6. Group icons   - drawing the 2x2 preview icons
// 7. Watching groups - updating icons, removing one-app groups
// 8. Watching drags - spotting icon-onto-icon drops and making groups
// 9. The group popup - the Start-menu-style window, and its settings panel
// 10. Opening groups - catching double-clicks on groups
// 11. Worker thread - finds the desktop in the background at startup
// 12. Windhawk callbacks - the functions Windhawk calls to start/stop the mod
// -----------------------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>        // GET_X_LPARAM / GET_Y_LPARAM
#include <commctrl.h>        // list-view messages (LVM_...)
#include <ole2.h>            // drag and drop (DoDragDrop, IDropSource)
#include <shlobj.h>          // shell folders and known folders (the Desktop path)
#include <shobjidl.h>        // IFolderView, IFileOperation, IShellItemImageFactory
#include <exdisp.h>          // IShellWindows
#include <shlwapi.h>         // StrRetToBufW, StrCmpLogicalW
#include <dwmapi.h>          // rounded corners for the popup
#include <shellscalingapi.h> // GetDpiForMonitor
#include <sddl.h>            // ConvertSidToStringSidW: your user account's ID as text
#include <commdlg.h>         // ChooseColorW: Windows' colour picker
#include <d2d1.h>            // Direct2D and DirectWrite: drawing text the way
#include <dwrite.h>          // the Start menu does (see RenderTextMask)
#include <wincodec.h>        // WIC: saving pictures as PNG
#include <wrl/client.h>      // ComPtr: releases COM objects for us automatically

#include <windhawk_utils.h>  // WindhawkUtils::SetWindowSubclassFromAnyThread

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

// ---- Staying safe while unloading -------------------------------------------
//
// Windhawk can unload the mod at any time (when you disable or update it).
// Some of the mod's code can be in the middle of running then: a right-click
// menu, the colour picker, a drag, or a "Do you want to allow this?" prompt
// all wait for you inside the mod's code. If the mod's code were removed from
// Explorer while that's happening, Explorer would crash when it returned.
//
// So every entry point that Windows calls into (window message handlers, and
// the DoDragDrop hook) counts itself in and out with a BusyScope, and
// Wh_ModUninit waits until the count is back to zero before letting the mod
// be unloaded.

std::atomic<int> g_busyCount{0};

struct BusyScope {
    BusyScope() { g_busyCount++; }
    ~BusyScope() { g_busyCount--; }  // Runs when the function returns, however it returns.
};

// ---- Translations ---------------------------------------------------------
//
// Everything the mod shows (apart from file names) comes from this table,
// in the language chosen in the "Language" setting, or Windows' own
// display language. Tr(Text::Layout), for example, gives "Layout" in
// English and "Ansicht" in German. The Windhawk settings page is
// translated separately, in the settings block at the top of the file
// (the "$name:de-DE" lines and so on).
//
// To add a language: add a row to kTranslations (each text in the same
// order as the Text list), a matching option to the "language" setting,
// and its translations of the settings.

enum class Text {
    GroupSettings,
    FolderSettings,
    ThisGroup,
    ThisFolder,
    AllGroups,
    All,
    Layout,
    View,
    Icons,
    List,
    AppsPerRow,
    ItemsPerRow,
    IconSize,
    Small,
    Medium,
    Large,
    Spacing,
    Compact,
    Normal,
    Roomy,
    ShowAppNames,
    ShowNames,
    Colour,
    Behaviour,
    SortApps,
    SortItems,
    MyOrder,
    AToZ,
    RemoveWhenOneLeft,
    AppliesToGroups,
    AppliesToGroupsAndFolders,
    ResetToAllGroups,
    ResetToShared,
    GroupEmpty,
    FolderEmpty,
    FolderDetail,
    NewGroup,
    Count
};
constexpr int kTextCount = (int)Text::Count;

struct Translation {
    PCWSTR language;  // As in the "language" setting: "de", "zh-TW"...
    PCWSTR texts[kTextCount];
};

const Translation kTranslations[] = {
    {L"en",
     {
         L"Group settings",
         L"Folder settings",
         L"This group",
         L"This folder",
         L"All groups",
         L"All",
         L"Layout",
         L"View",
         L"Icons",
         L"List",
         L"Apps per row",
         L"Items per row",
         L"Icon size",
         L"Small",
         L"Medium",
         L"Large",
         L"Spacing",
         L"Compact",
         L"Normal",
         L"Roomy",
         L"Show app names",
         L"Show names",
         L"Colour",
         L"Behaviour",
         L"Sort apps",
         L"Sort items",
         L"My order",
         L"A to Z",
         L"Remove when one app is left",
         L"Changes apply to every group.",
         L"Changes apply to every group and folder.",
         L"Reset to all-groups settings",
         L"Reset to shared settings",
         L"This group is empty",
         L"This folder is empty",
         L"Folder",
         L"New group",
     }},
    {L"de",
     {
         L"Gruppeneinstellungen",
         L"Ordnereinstellungen",
         L"Diese Gruppe",
         L"Dieser Ordner",
         L"Alle Gruppen",
         L"Alle",
         L"Layout",
         L"Ansicht",
         L"Symbole",
         L"Liste",
         L"Apps pro Zeile",
         L"Elemente pro Zeile",
         L"Symbolgröße",
         L"Klein",
         L"Mittel",
         L"Groß",
         L"Abstand",
         L"Kompakt",
         L"Normal",
         L"Weit",
         L"App-Namen anzeigen",
         L"Namen anzeigen",
         L"Farbe",
         L"Verhalten",
         L"Apps sortieren",
         L"Elemente sortieren",
         L"Eigene Reihenfolge",
         L"A bis Z",
         L"Entfernen, wenn nur eine App übrig ist",
         L"Änderungen gelten für alle Gruppen.",
         L"Änderungen gelten für alle Gruppen und Ordner.",
         L"Auf Einstellungen aller Gruppen zurücksetzen",
         L"Auf gemeinsame Einstellungen zurücksetzen",
         L"Diese Gruppe ist leer",
         L"Dieser Ordner ist leer",
         L"Ordner",
         L"Neue Gruppe",
     }},
    {L"es",
     {
         L"Configuración del grupo",
         L"Configuración de la carpeta",
         L"Este grupo",
         L"Esta carpeta",
         L"Todos los grupos",
         L"Todos",
         L"Diseño",
         L"Vista",
         L"Iconos",
         L"Lista",
         L"Apps por fila",
         L"Elementos por fila",
         L"Tamaño de icono",
         L"Pequeño",
         L"Mediano",
         L"Grande",
         L"Espaciado",
         L"Compacto",
         L"Normal",
         L"Amplio",
         L"Mostrar nombres de apps",
         L"Mostrar nombres",
         L"Color",
         L"Comportamiento",
         L"Ordenar apps",
         L"Ordenar elementos",
         L"Mi orden",
         L"De la A a la Z",
         L"Quitar cuando quede una sola app",
         L"Los cambios se aplican a todos los grupos.",
         L"Los cambios se aplican a todos los grupos y carpetas.",
         L"Restablecer a la configuración de todos los grupos",
         L"Restablecer a la configuración compartida",
         L"Este grupo está vacío",
         L"Esta carpeta está vacía",
         L"Carpeta",
         L"Nuevo grupo",
     }},
    {L"fr",
     {
         L"Paramètres du groupe",
         L"Paramètres du dossier",
         L"Ce groupe",
         L"Ce dossier",
         L"Tous les groupes",
         L"Tous",
         L"Disposition",
         L"Affichage",
         L"Icônes",
         L"Liste",
         L"Applis par ligne",
         L"Éléments par ligne",
         L"Taille des icônes",
         L"Petite",
         L"Moyenne",
         L"Grande",
         L"Espacement",
         L"Compact",
         L"Normal",
         L"Aéré",
         L"Afficher le nom des applis",
         L"Afficher les noms",
         L"Couleur",
         L"Comportement",
         L"Trier les applis",
         L"Trier les éléments",
         L"Mon ordre",
         L"De A à Z",
         L"Supprimer s'il reste une seule appli",
         L"Les modifications s'appliquent à tous les groupes.",
         L"Les modifications s'appliquent à tous les groupes et dossiers.",
         L"Rétablir les paramètres de tous les groupes",
         L"Rétablir les paramètres communs",
         L"Ce groupe est vide",
         L"Ce dossier est vide",
         L"Dossier",
         L"Nouveau groupe",
     }},
    {L"it",
     {
         L"Impostazioni gruppo",
         L"Impostazioni cartella",
         L"Questo gruppo",
         L"Questa cartella",
         L"Tutti i gruppi",
         L"Tutti",
         L"Layout",
         L"Visualizzazione",
         L"Icone",
         L"Elenco",
         L"App per riga",
         L"Elementi per riga",
         L"Dimensione icone",
         L"Piccole",
         L"Medie",
         L"Grandi",
         L"Spaziatura",
         L"Compatta",
         L"Normale",
         L"Ampia",
         L"Mostra nomi app",
         L"Mostra nomi",
         L"Colore",
         L"Comportamento",
         L"Ordina app",
         L"Ordina elementi",
         L"Il mio ordine",
         L"Dalla A alla Z",
         L"Rimuovi quando resta una sola app",
         L"Le modifiche si applicano a tutti i gruppi.",
         L"Le modifiche si applicano a tutti i gruppi e le cartelle.",
         L"Ripristina impostazioni di tutti i gruppi",
         L"Ripristina impostazioni condivise",
         L"Questo gruppo è vuoto",
         L"Questa cartella è vuota",
         L"Cartella",
         L"Nuovo gruppo",
     }},
    {L"ja",
     {
         L"グループの設定",
         L"フォルダーの設定",
         L"このグループ",
         L"このフォルダー",
         L"すべてのグループ",
         L"すべて",
         L"レイアウト",
         L"表示",
         L"アイコン",
         L"一覧",
         L"1 行のアプリ数",
         L"1 行の項目数",
         L"アイコンのサイズ",
         L"小",
         L"中",
         L"大",
         L"間隔",
         L"狭い",
         L"標準",
         L"広い",
         L"アプリ名を表示",
         L"名前を表示",
         L"色",
         L"動作",
         L"アプリの並び順",
         L"項目の並び順",
         L"自分の順序",
         L"名前順",
         L"アプリが 1 つになったら削除",
         L"変更はすべてのグループに適用されます。",
         L"変更はすべてのグループとフォルダーに適用されます。",
         L"すべてのグループの設定に戻す",
         L"共通の設定に戻す",
         L"このグループは空です",
         L"このフォルダーは空です",
         L"フォルダー",
         L"新しいグループ",
     }},
    {L"ko",
     {
         L"그룹 설정",
         L"폴더 설정",
         L"이 그룹",
         L"이 폴더",
         L"모든 그룹",
         L"모두",
         L"레이아웃",
         L"보기",
         L"아이콘",
         L"목록",
         L"행당 앱 수",
         L"행당 항목 수",
         L"아이콘 크기",
         L"작게",
         L"보통",
         L"크게",
         L"간격",
         L"좁게",
         L"보통",
         L"넓게",
         L"앱 이름 표시",
         L"이름 표시",
         L"색",
         L"동작",
         L"앱 정렬",
         L"항목 정렬",
         L"내 순서",
         L"이름순",
         L"앱이 하나 남으면 제거",
         L"변경 내용은 모든 그룹에 적용됩니다.",
         L"변경 내용은 모든 그룹과 폴더에 적용됩니다.",
         L"모든 그룹 설정으로 재설정",
         L"공유 설정으로 재설정",
         L"이 그룹은 비어 있습니다",
         L"이 폴더는 비어 있습니다",
         L"폴더",
         L"새 그룹",
     }},
    {L"nl",
     {
         L"Groepsinstellingen",
         L"Mapinstellingen",
         L"Deze groep",
         L"Deze map",
         L"Alle groepen",
         L"Alle",
         L"Indeling",
         L"Weergave",
         L"Pictogrammen",
         L"Lijst",
         L"Apps per rij",
         L"Items per rij",
         L"Pictogramgrootte",
         L"Klein",
         L"Middel",
         L"Groot",
         L"Afstand",
         L"Compact",
         L"Normaal",
         L"Ruim",
         L"App-namen tonen",
         L"Namen tonen",
         L"Kleur",
         L"Gedrag",
         L"Apps sorteren",
         L"Items sorteren",
         L"Mijn volgorde",
         L"A tot Z",
         L"Verwijderen als er één app over is",
         L"Wijzigingen gelden voor alle groepen.",
         L"Wijzigingen gelden voor alle groepen en mappen.",
         L"Terugzetten naar instellingen van alle groepen",
         L"Terugzetten naar gedeelde instellingen",
         L"Deze groep is leeg",
         L"Deze map is leeg",
         L"Map",
         L"Nieuwe groep",
     }},
    {L"pl",
     {
         L"Ustawienia grupy",
         L"Ustawienia folderu",
         L"Ta grupa",
         L"Ten folder",
         L"Wszystkie grupy",
         L"Wszystkie",
         L"Układ",
         L"Widok",
         L"Ikony",
         L"Lista",
         L"Aplikacje w wierszu",
         L"Elementy w wierszu",
         L"Rozmiar ikon",
         L"Małe",
         L"Średnie",
         L"Duże",
         L"Odstępy",
         L"Wąskie",
         L"Normalne",
         L"Szerokie",
         L"Pokaż nazwy aplikacji",
         L"Pokaż nazwy",
         L"Kolor",
         L"Zachowanie",
         L"Sortuj aplikacje",
         L"Sortuj elementy",
         L"Moja kolejność",
         L"Od A do Z",
         L"Usuń, gdy zostanie jedna aplikacja",
         L"Zmiany dotyczą wszystkich grup.",
         L"Zmiany dotyczą wszystkich grup i folderów.",
         L"Przywróć ustawienia wszystkich grup",
         L"Przywróć wspólne ustawienia",
         L"Ta grupa jest pusta",
         L"Ten folder jest pusty",
         L"Folder",
         L"Nowa grupa",
     }},
    {L"pt",
     {
         L"Configurações do grupo",
         L"Configurações da pasta",
         L"Este grupo",
         L"Esta pasta",
         L"Todos os grupos",
         L"Todos",
         L"Layout",
         L"Exibição",
         L"Ícones",
         L"Lista",
         L"Apps por linha",
         L"Itens por linha",
         L"Tamanho dos ícones",
         L"Pequeno",
         L"Médio",
         L"Grande",
         L"Espaçamento",
         L"Compacto",
         L"Normal",
         L"Amplo",
         L"Mostrar nomes dos apps",
         L"Mostrar nomes",
         L"Cor",
         L"Comportamento",
         L"Ordenar apps",
         L"Ordenar itens",
         L"Minha ordem",
         L"De A a Z",
         L"Remover quando restar um só app",
         L"As alterações valem para todos os grupos.",
         L"As alterações valem para todos os grupos e pastas.",
         L"Redefinir para as configurações de todos os grupos",
         L"Redefinir para as configurações compartilhadas",
         L"Este grupo está vazio",
         L"Esta pasta está vazia",
         L"Pasta",
         L"Novo grupo",
     }},
    {L"ru",
     {
         L"Настройки группы",
         L"Настройки папки",
         L"Эта группа",
         L"Эта папка",
         L"Все группы",
         L"Все",
         L"Макет",
         L"Вид",
         L"Значки",
         L"Список",
         L"Приложений в строке",
         L"Элементов в строке",
         L"Размер значков",
         L"Мелкие",
         L"Средние",
         L"Крупные",
         L"Интервалы",
         L"Плотно",
         L"Обычно",
         L"Свободно",
         L"Показывать названия приложений",
         L"Показывать названия",
         L"Цвет",
         L"Поведение",
         L"Сортировка приложений",
         L"Сортировка элементов",
         L"Мой порядок",
         L"От А до Я",
         L"Удалять, когда остаётся одно приложение",
         L"Изменения применяются ко всем группам.",
         L"Изменения применяются ко всем группам и папкам.",
         L"Сбросить к настройкам всех групп",
         L"Сбросить к общим настройкам",
         L"Эта группа пуста",
         L"Эта папка пуста",
         L"Папка",
         L"Новая группа",
     }},
    {L"tr",
     {
         L"Grup ayarları",
         L"Klasör ayarları",
         L"Bu grup",
         L"Bu klasör",
         L"Tüm gruplar",
         L"Tümü",
         L"Düzen",
         L"Görünüm",
         L"Simgeler",
         L"Liste",
         L"Satır başına uygulama",
         L"Satır başına öğe",
         L"Simge boyutu",
         L"Küçük",
         L"Orta",
         L"Büyük",
         L"Aralık",
         L"Sıkı",
         L"Normal",
         L"Geniş",
         L"Uygulama adlarını göster",
         L"Adları göster",
         L"Renk",
         L"Davranış",
         L"Uygulamaları sırala",
         L"Öğeleri sırala",
         L"Benim sıram",
         L"A'dan Z'ye",
         L"Tek uygulama kalınca kaldır",
         L"Değişiklikler tüm gruplara uygulanır.",
         L"Değişiklikler tüm gruplara ve klasörlere uygulanır.",
         L"Tüm grupların ayarlarına sıfırla",
         L"Ortak ayarlara sıfırla",
         L"Bu grup boş",
         L"Bu klasör boş",
         L"Klasör",
         L"Yeni grup",
     }},
    {L"uk",
     {
         L"Налаштування групи",
         L"Налаштування папки",
         L"Ця група",
         L"Ця папка",
         L"Усі групи",
         L"Усі",
         L"Макет",
         L"Вигляд",
         L"Значки",
         L"Список",
         L"Програм у рядку",
         L"Елементів у рядку",
         L"Розмір значків",
         L"Малі",
         L"Середні",
         L"Великі",
         L"Інтервали",
         L"Щільно",
         L"Звичайно",
         L"Вільно",
         L"Показувати назви програм",
         L"Показувати назви",
         L"Колір",
         L"Поведінка",
         L"Сортування програм",
         L"Сортування елементів",
         L"Мій порядок",
         L"Від А до Я",
         L"Видаляти, коли лишається одна програма",
         L"Зміни застосовуються до всіх груп.",
         L"Зміни застосовуються до всіх груп і папок.",
         L"Скинути до налаштувань усіх груп",
         L"Скинути до спільних налаштувань",
         L"Ця група порожня",
         L"Ця папка порожня",
         L"Папка",
         L"Нова група",
     }},
    {L"zh-CN",
     {
         L"组设置",
         L"文件夹设置",
         L"此组",
         L"此文件夹",
         L"所有组",
         L"全部",
         L"布局",
         L"视图",
         L"图标",
         L"列表",
         L"每行应用数",
         L"每行项目数",
         L"图标大小",
         L"小",
         L"中",
         L"大",
         L"间距",
         L"紧凑",
         L"正常",
         L"宽松",
         L"显示应用名称",
         L"显示名称",
         L"颜色",
         L"行为",
         L"应用排序",
         L"项目排序",
         L"我的顺序",
         L"按名称",
         L"仅剩一个应用时删除",
         L"更改将应用于所有组。",
         L"更改将应用于所有组和文件夹。",
         L"重置为所有组的设置",
         L"重置为共享设置",
         L"此组为空",
         L"此文件夹为空",
         L"文件夹",
         L"新建组",
     }},
    {L"zh-TW",
     {
         L"群組設定",
         L"資料夾設定",
         L"此群組",
         L"此資料夾",
         L"所有群組",
         L"全部",
         L"版面配置",
         L"檢視",
         L"圖示",
         L"清單",
         L"每列應用程式數",
         L"每列項目數",
         L"圖示大小",
         L"小",
         L"中",
         L"大",
         L"間距",
         L"緊密",
         L"標準",
         L"寬鬆",
         L"顯示應用程式名稱",
         L"顯示名稱",
         L"色彩",
         L"行為",
         L"應用程式排序",
         L"項目排序",
         L"我的順序",
         L"依名稱",
         L"只剩一個應用程式時移除",
         L"變更會套用到所有群組。",
         L"變更會套用到所有群組和資料夾。",
         L"重設為所有群組的設定",
         L"重設為共用設定",
         L"此群組是空的",
         L"此資料夾是空的",
         L"資料夾",
         L"新群組",
     }},
};

std::atomic<int> g_languageIndex{0};  // Which row of kTranslations.

PCWSTR Tr(Text text) {
    return kTranslations[g_languageIndex].texts[(int)text];
}

// Finds the row for the "language" setting. For "auto", uses Windows' display
// language: Chinese in Taiwan, Hong Kong and Macao gets Traditional Chinese,
// other Chinese gets Simplified, and other languages are matched by their
// first part ("pt-PT" and "pt-BR" both get "pt"). English if nothing matches.
int FindLanguage(PCWSTR setting) {
    std::wstring wanted = setting ? setting : L"auto";
    if (wanted.empty() || wanted == L"auto") {
        WCHAR name[LOCALE_NAME_MAX_LENGTH] = L"";
        if (!LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT), name,
                              LOCALE_NAME_MAX_LENGTH, 0)) {
            return 0;
        }
        wanted = name;
        if (_wcsnicmp(name, L"zh", 2) == 0) {
            bool traditional = wcsstr(name, L"Hant") || wcsstr(name, L"TW") ||
                               wcsstr(name, L"HK") || wcsstr(name, L"MO");
            wanted = traditional ? L"zh-TW" : L"zh-CN";
        } else {
            wanted = wanted.substr(0, wanted.find(L'-'));
        }
    }
    for (int i = 0; i < (int)ARRAYSIZE(kTranslations); i++) {
        if (_wcsicmp(kTranslations[i].language, wanted.c_str()) == 0) {
            return i;
        }
    }
    return 0;
}

// ---- 1. Settings ------------------------------------------------------------

enum class GroupTrigger { Hover, Shift, Always };

struct {
    GroupTrigger trigger;
    int hoverDelayMs;
    bool openWithSingleClick;
    bool openWithHover;
    int openHoverDelayMs;
    bool openAnyFolder;
    bool blurredPopup;
    int popupOpacity;  // 0 to 100.
    bool animatePopup;
    int openMilliseconds;   // How long the popup takes to open (closing
                            // takes three quarters of this).
    int maxIconRows;        // Rows shown before scrolling, with icons...
    int maxListRows;        // ...and in the list view.
} g_settings;

// Whether each group's data is kept in its desktop.ini (true) or in
// Windhawk's storage (false). See section 5. "atomic" because the group
// watcher thread reads it too.
std::atomic<bool> g_dataInFolder{false};

void LoadSettings() {
    // Text settings must be freed after reading them.
    PCWSTR trigger = Wh_GetStringSetting(L"groupTrigger");
    if (wcscmp(trigger, L"shift") == 0) {
        g_settings.trigger = GroupTrigger::Shift;
    } else if (wcscmp(trigger, L"always") == 0) {
        g_settings.trigger = GroupTrigger::Always;
    } else {
        g_settings.trigger = GroupTrigger::Hover;
    }
    Wh_FreeStringSetting(trigger);

    g_settings.hoverDelayMs = Wh_GetIntSetting(L"hoverDelay");
    if (g_settings.hoverDelayMs < 0) {
        g_settings.hoverDelayMs = 0;
    }

    // Booleans come back from Windhawk as 0 or 1.
    // With "hover", a single click opens groups too.
    PCWSTR openWith = Wh_GetStringSetting(L"openWith");
    g_settings.openWithSingleClick = wcscmp(openWith, L"double") != 0;
    g_settings.openWithHover = wcscmp(openWith, L"hover") == 0;
    Wh_FreeStringSetting(openWith);
    g_settings.openHoverDelayMs = std::max(50, Wh_GetIntSetting(L"openHoverDelay"));
    g_settings.openAnyFolder = Wh_GetIntSetting(L"openAnyFolder") != 0;

    g_settings.blurredPopup = Wh_GetIntSetting(L"blurredPopup") != 0;
    g_settings.popupOpacity = std::clamp(Wh_GetIntSetting(L"popupOpacity"), 0, 100);
    g_settings.openMilliseconds = std::clamp(Wh_GetIntSetting(L"animationDuration"), 50, 2000);
    g_settings.maxIconRows = std::clamp(Wh_GetIntSetting(L"maxIconRows"), 1, 30);
    g_settings.maxListRows = std::clamp(Wh_GetIntSetting(L"maxListRows"), 1, 60);

    PCWSTR language = Wh_GetStringSetting(L"language");
    g_languageIndex = FindLanguage(language);
    Wh_FreeStringSetting(language);
    g_settings.animatePopup = Wh_GetIntSetting(L"animatePopup") != 0;
    g_dataInFolder = Wh_GetIntSetting(L"groupDataInFolder") != 0;
}

// ---- 2. Finding the desktop icon list ---------------------------------------
//
// The desktop is made of nested windows:
//
//   Progman (or sometimes a "WorkerW" window)   <- the desktop itself
//     └─ SHELLDLL_DefView                       <- Explorer's folder view
//          └─ SysListView32                     <- the list of icons

// Once found, these are remembered here. "atomic" makes it safe to set them on
// one thread (the worker) and read them on another (the desktop's).
std::atomic<HWND> g_desktopList{nullptr};
std::atomic<HWND> g_desktopDefView{nullptr};

// The search can end three ways: found it, "not yet" (try again shortly), or
// "never" (this explorer.exe doesn't own the desktop, so stop looking).
enum class SearchResult { Found, NotYet, NotThisProcess };

// When `explain` is true, the function writes to the log why it couldn't
// find the list. Messages like these are the main way to debug a mod.
SearchResult FindDesktopListView(HWND* listOut, bool explain) {
    // GetShellWindow() returns the main desktop window (Progman).
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        if (explain) Wh_Log(L"Step 1 failed: the desktop window doesn't exist yet");
        return SearchResult::NotYet;
    }

    // Windhawk loads this mod into every explorer.exe. Only one of them owns
    // the desktop, so the others can stop here.
    DWORD ownerProcessId = 0;
    GetWindowThreadProcessId(shellWindow, &ownerProcessId);
    if (ownerProcessId != GetCurrentProcessId()) {
        Wh_Log(L"The desktop belongs to explorer.exe process %u, not this one. "
               L"Nothing to do here.", (unsigned)ownerProcessId);
        return SearchResult::NotThisProcess;
    }

    HWND defView = FindWindowExW(shellWindow, nullptr, L"SHELLDLL_DefView", nullptr);

    if (!defView) {
        // Windows sometimes moves the icons into a "WorkerW" window (for
        // example after Win+Tab or with a wallpaper slideshow). Depending on
        // the Windows version, WorkerW is either a top-level window or a
        // child of Progman, so check both places.
        HWND parents[] = {nullptr, shellWindow};
        for (HWND parent : parents) {
            HWND workerW = nullptr;
            while (!defView &&
                   (workerW = FindWindowExW(parent, workerW, L"WorkerW", nullptr))) {
                defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
            }
            if (defView) {
                break;
            }
        }
    }

    if (!defView) {
        if (explain) Wh_Log(L"Step 2 failed: found the desktop window but not "
                            L"SHELLDLL_DefView inside it");
        return SearchResult::NotYet;
    }

    HWND list = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    if (!list) {
        if (explain) Wh_Log(L"Step 3 failed: found SHELLDLL_DefView but not "
                            L"SysListView32 inside it");
        return SearchResult::NotYet;
    }

    *listOut = list;
    return SearchResult::Found;
}

// ---- 3. Reading the icons ---------------------------------------------------
//
// Windows controls are driven by "messages". To ask the list how many icons
// it has, we send it LVM_GETITEMCOUNT; to get an icon's name, LVM_GETITEMTEXTW.
// We use SendMessageTimeoutW so that if Explorer is busy, we give up after a
// second instead of freezing.

bool SendToList(HWND list, UINT message, WPARAM wParam, LPARAM lParam,
                LRESULT* result = nullptr) {
    DWORD_PTR value = 0;
    if (!SendMessageTimeoutW(list, message, wParam, lParam,
                             SMTO_ABORTIFHUNG, 1000, &value)) {
        return false;
    }
    if (result) {
        *result = (LRESULT)value;
    }
    return true;
}

std::wstring GetIconName(HWND list, int index) {
    WCHAR name[MAX_PATH] = L"";

    LVITEMW item = {};
    item.iSubItem = 0;
    item.pszText = name;
    item.cchTextMax = ARRAYSIZE(name);
    SendToList(list, LVM_GETITEMTEXTW, index, (LPARAM)&item);

    return name;
}

// Returns the numbers of all currently selected icons. When you drag, the
// selected icons are the ones being dragged.
std::vector<int> GetSelectedIcons(HWND list) {
    std::vector<int> selected;
    LRESULT index = -1;
    while (SendToList(list, LVM_GETNEXTITEM, (WPARAM)index, LVNI_SELECTED, &index) &&
           index != -1) {
        selected.push_back((int)index);
    }
    return selected;
}

// Returns the number of the icon under the mouse pointer, or -1 if the
// pointer is over empty desktop.
int GetIconUnderCursor(HWND list) {
    LVHITTESTINFO hit = {};
    GetCursorPos(&hit.pt);           // Screen coordinates...
    ScreenToClient(list, &hit.pt);   // ...converted to the list's own.

    LRESULT index = -1;
    SendToList(list, LVM_HITTEST, 0, (LPARAM)&hit, &index);
    if (index >= 0 && (hit.flags & LVHT_ONITEM)) {
        return (int)index;
    }
    return -1;
}

void LogDesktopIcons(HWND list) {
    LRESULT count = 0;
    if (SendToList(list, LVM_GETITEMCOUNT, 0, 0, &count)) {
        Wh_Log(L"Found the desktop (%d icons)", (int)count);
    } else {
        Wh_Log(L"The desktop didn't respond");
    }
}

// ---- 4. Desktop icons as files ----------------------------------------------
//
// The icon list only knows each icon's label, like "Car Mechanic Simulator
// 2021". To move things we need the real file behind each icon, like
// "C:\Users\Mark\Desktop\Car Mechanic Simulator 2021.lnk".
//
// Explorer offers an official way to ask: the desktop's "folder view"
// (IFolderView). We reach it through IShellWindows, which lists every Explorer
// window, including the desktop.
//
// Objects like these are "COM objects". You use them through pointers, and
// must "Release" each one when finished. ComPtr does the releasing for us.

struct DesktopItem {
    std::wstring name;        // The label under the icon.
    std::wstring path;        // Full path on disk. Empty for Recycle Bin, This PC, etc.
    bool isDirectory = false; // True for folders (including groups).
};

ComPtr<IFolderView> GetDesktopFolderView() {
    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&shellWindows)))) {
        return nullptr;
    }

    // Ask for "the window showing the desktop".
    VARIANT location = {};
    location.vt = VT_I4;
    location.lVal = CSIDL_DESKTOP;
    VARIANT empty = {};
    empty.vt = VT_EMPTY;
    long windowHandle = 0;
    ComPtr<IDispatch> desktopWindow;
    if (shellWindows->FindWindowSW(&location, &empty, SWC_DESKTOP, &windowHandle,
                                   SWFO_NEEDDISPATCH, &desktopWindow) != S_OK ||
        !desktopWindow) {
        return nullptr;
    }

    // Window -> browser -> current view -> folder view.
    ComPtr<IServiceProvider> serviceProvider;
    ComPtr<IShellBrowser> browser;
    ComPtr<IShellView> view;
    ComPtr<IFolderView> folderView;
    if (FAILED(desktopWindow.As(&serviceProvider)) ||
        FAILED(serviceProvider->QueryService(SID_STopLevelBrowser,
                                             IID_PPV_ARGS(&browser))) ||
        FAILED(browser->QueryActiveShellView(&view)) ||
        FAILED(view.As(&folderView))) {
        return nullptr;
    }
    return folderView;
}

// Reads one item of the folder view: its label, and if it's a real file or
// folder, its path.
bool ReadFolderViewItem(IFolderView* folderView, int index, DesktopItem* out) {
    ComPtr<IShellFolder> folder;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
        return false;
    }

    // An "item ID" is how Explorer identifies an item, file or not.
    PITEMID_CHILD itemId = nullptr;
    if (FAILED(folderView->Item(index, &itemId))) {
        return false;
    }

    WCHAR buffer[MAX_PATH] = L"";
    STRRET text;

    if (SUCCEEDED(folder->GetDisplayNameOf(itemId, SHGDN_INFOLDER, &text))) {
        StrRetToBufW(&text, itemId, buffer, ARRAYSIZE(buffer));
    }
    out->name = buffer;

    // Is it a real file or folder on disk?
    PCUITEMID_CHILD items[] = {itemId};
    SFGAOF attributes = SFGAO_FILESYSTEM;
    folder->GetAttributesOf(1, items, &attributes);

    out->path.clear();
    out->isDirectory = false;
    if ((attributes & SFGAO_FILESYSTEM) &&
        SUCCEEDED(folder->GetDisplayNameOf(itemId, SHGDN_FORPARSING, &text))) {
        StrRetToBufW(&text, itemId, buffer, ARRAYSIZE(buffer));
        out->path = buffer;

        DWORD fileAttributes = GetFileAttributesW(buffer);
        out->isDirectory = fileAttributes != INVALID_FILE_ATTRIBUTES &&
                           (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    CoTaskMemFree(itemId);
    return true;
}

// Finds the file behind icon number `index` in the icon list.
//
// Usually icon N in the list is item N in the folder view, but we double-check
// by comparing labels. If they don't match, we search by label instead. Safety
// checks like this matter: moving the wrong file would be a real problem.
bool GetDesktopItem(IFolderView* folderView, HWND list, int index,
                    DesktopItem* out) {
    std::wstring label = GetIconName(list, index);

    if (ReadFolderViewItem(folderView, index, out) && out->name == label) {
        return true;
    }

    int count = 0;
    folderView->ItemCount(SVGIO_ALLVIEW, &count);
    for (int i = 0; i < count; i++) {
        if (ReadFolderViewItem(folderView, i, out) && out->name == label) {
            Wh_Log(L"(Found \"%s\" by its label instead of its position)",
                   label.c_str());
            return true;
        }
    }

    Wh_Log(L"Couldn't find the file behind \"%s\"", label.c_str());
    return false;
}

// Something can only go into a group if it's a real file that isn't a folder.
bool CanBeGrouped(const DesktopItem& item) {
    return !item.path.empty() && !item.isDirectory;
}

// --- Placing icons -----------------------------------------------------------
//
// Windows puts new desktop items in the next free spot. We'd rather a new
// group appear where you dropped, and a removed group's last app appear
// where the group was.
//
// The folder view can tell us where an item is (GetItemPosition) and move it
// (SelectAndPositionItems). But a new item only shows up in the view a moment
// after its file appears, so we keep a short to-do list, "put this item here
// once it shows up", and check it every 50 ms (for up to 3 seconds) using a
// timer on the desktop window.
//
// The folder view belongs to the desktop's thread, so this all runs there.
// The group watcher thread asks for it by sending the desktop window our own
// messages. Those message numbers come from RegisterWindowMessageW, which
// hands out numbers no other program uses. (Fixed numbers like WM_APP + 1
// are only safe in our own windows; the desktop window is Explorer's, and
// Explorer may already use them.)

UINT g_msgGetItemPosition = 0;  // lParam: std::shared_ptr<PositionRequest>*
UINT g_msgPlaceItem = 0;        // lParam: Placement*
UINT g_msgStopPlacing = 0;      // Clears the to-do list (when the mod stops).
constexpr UINT_PTR kPlacementTimer = 0x5DA6;  // An unusual number, to avoid
                                              // clashing with DefView's timers.

void RegisterPlacementMessages() {
    g_msgGetItemPosition = RegisterWindowMessageW(L"DesktopAppGroups_GetItemPosition");
    g_msgPlaceItem = RegisterWindowMessageW(L"DesktopAppGroups_PlaceItem");
    g_msgStopPlacing = RegisterWindowMessageW(L"DesktopAppGroups_StopPlacing");
}

struct Placement {
    std::wstring path;
    POINT position;
    ULONGLONG giveUpTime;
};
std::vector<Placement> g_placements;  // Only used on the desktop's thread.

// Finds an item in the desktop's folder view by its path. Returns its number,
// or -1.
int FindViewItem(IFolderView* folderView, const std::wstring& path) {
    int count = 0;
    folderView->ItemCount(SVGIO_ALLVIEW, &count);
    for (int i = 0; i < count; i++) {
        DesktopItem item;
        if (ReadFolderViewItem(folderView, i, &item) &&
            _wcsicmp(item.path.c_str(), path.c_str()) == 0) {
            return i;
        }
    }
    return -1;
}

// Where is this item on the desktop? (Desktop's thread only.)
bool GetViewItemPosition(const std::wstring& path, POINT* position) {
    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    int index = folderView ? FindViewItem(folderView.Get(), path) : -1;
    PITEMID_CHILD itemId = nullptr;
    if (index < 0 || FAILED(folderView->Item(index, &itemId))) {
        return false;
    }
    HRESULT result = folderView->GetItemPosition(itemId, position);
    CoTaskMemFree(itemId);
    return SUCCEEDED(result);
}

// Moves an item to a position, if it's in the view yet. (Desktop's thread.)
bool PlaceViewItem(const std::wstring& path, POINT position) {
    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    int index = folderView ? FindViewItem(folderView.Get(), path) : -1;
    PITEMID_CHILD itemId = nullptr;
    if (index < 0 || FAILED(folderView->Item(index, &itemId))) {
        return false;
    }
    PCUITEMID_CHILD items[] = {itemId};
    HRESULT result = folderView->SelectAndPositionItems(1, items, &position, SVSI_POSITIONITEM);
    CoTaskMemFree(itemId);
    return SUCCEEDED(result);
}

// Adds to the to-do list. (Desktop's thread; `desktopWindow` is DefView.)
void QueuePlacement(HWND desktopWindow, const std::wstring& path, POINT position) {
    g_placements.push_back({path, position, GetTickCount64() + 3000});
    SetTimer(desktopWindow, kPlacementTimer, 50, nullptr);
}

// Called by the timer: places whatever has shown up, drops anything that's
// taken too long, and stops the timer once the list is empty.
void ProcessPlacements(HWND desktopWindow) {
    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_placements.size();) {
        if (PlaceViewItem(g_placements[i].path, g_placements[i].position) ||
            now > g_placements[i].giveUpTime) {
            g_placements.erase(g_placements.begin() + i);
        } else {
            i++;
        }
    }
    if (g_placements.empty()) {
        KillTimer(desktopWindow, kPlacementTimer);
    }
}

// For other threads (the group watcher): the same, by asking the desktop's
// thread with a message.
//
// Careful: a message must never point at data the sender might throw away
// before the message is handled. SendMessageTimeoutW gives up waiting after
// 2 seconds if the desktop is busy, but the message stays queued, and the
// desktop's thread handles it later. So each request lives on the heap
// (created with "new"), and the desktop's thread frees it when it's done.

// The request and its answer. Both threads keep a reference to it (a
// shared_ptr, which counts its users), and it's freed when the last one lets
// go. So if the asker gives up, the desktop's thread can still answer safely.
struct PositionRequest {
    std::wstring path;
    POINT position = {};
    bool found = false;
};

bool AskDesktopForPosition(const std::wstring& path, POINT* position) {
    HWND desktopWindow = g_desktopDefView;
    if (!desktopWindow) {
        return false;
    }
    auto request = std::make_shared<PositionRequest>();
    request->path = path;
    // The desktop's thread's own reference, which it deletes once it answers.
    auto* message = new std::shared_ptr<PositionRequest>(request);

    DWORD_PTR ignored = 0;
    if (!SendMessageTimeoutW(desktopWindow, g_msgGetItemPosition, 0, (LPARAM)message,
                             SMTO_ABORTIFHUNG, 2000, &ignored)) {
        // Gave up waiting. If the message is handled later, the desktop's
        // thread frees it then. (If it never is, a few bytes are lost, which
        // is harmless.)
        return false;
    }
    *position = request->position;
    return request->found;
}

// No answer is needed here, so the message is posted (queued without
// waiting) instead of sent.
void AskDesktopToPlace(const std::wstring& path, POINT position) {
    HWND desktopWindow = g_desktopDefView;
    if (!desktopWindow) {
        return;
    }
    auto* request = new Placement{path, position, 0};
    if (!PostMessageW(desktopWindow, g_msgPlaceItem, 0, (LPARAM)request)) {
        delete request;  // Not queued, so nobody else will free it.
    }
}

// ---- 5. Groups --------------------------------------------------------------
//
// A group is a normal folder on your desktop. That keeps it simple and safe:
// if you uninstall the mod, your shortcuts are still in an ordinary folder.
//
// Each group also has some data of the mod's own: a marker saying "this
// folder is a group", the order you arranged its apps in, its own settings
// from the gear panel, and which icon it has. Where that's kept depends on
// the "Keep group data inside the group folders" setting:
//
//   - Off (the default): in the mod's storage in Windhawk, on this PC. Each
//     group is recognised by its folder's file ID, a number Windows gives
//     every file and folder, which stays the same when the folder is renamed
//     or moved around the same drive.
//   - On: in the group's hidden "desktop.ini" file (a standard Windows file
//     for folder settings), so the data goes wherever the folder goes.
//
// Either way, desktop.ini holds the line Windows needs to show the group's
// icon. With the setting on, the mod's data is in there too:
//
//   [.ShellClassInfo]
//   IconResource=C:\...\group-icon-123.ico,0   <- the group's icon
//   [DesktopAppGroups]                         <- only with the setting on:
//   IsGroup=1                                  <- "this folder is a group"
//   IconVersion=2                              <- which icon style it has
//   IconFingerprint=123456                     <- which apps the icon shows
//   Order=Notepad.lnk|Paint.lnk                <- your arrangement
//   Tint=255                                   <- the group's own settings
//
// When the setting changes, each group's data is moved over (MoveGroupData).
// If the drive has no file IDs (some network drives), the data stays in
// desktop.ini whatever the setting, since there's nothing else to recognise
// the folder by.

const WCHAR kGroupIniSection[] = L"DesktopAppGroups";

std::wstring ReadGroupIniValue(const std::wstring& folderPath, PCWSTR section,
                               PCWSTR key) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    // Big enough for a long list of app names (the saved order).
    std::vector<WCHAR> value(32 * 1024, L'\0');
    GetPrivateProfileStringW(section, key, L"", value.data(), (DWORD)value.size(),
                             iniPath.c_str());
    return value.data();
}

// Makes sure a group's desktop.ini is stored as Unicode, so app names in any
// alphabet (Japanese, Greek...) survive being saved in it. Windows decides by
// the first two bytes: 0xFF 0xFE (a "byte order mark") means Unicode.
// Otherwise it uses the older one-byte-per-letter format, which garbles
// letters outside your PC's language. So: create the file with the mark, or
// convert an existing old-format file.
void EnsureUnicodeGroupIni(const std::wstring& folderPath) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    std::vector<char> oldBytes;

    HANDLE file = CreateFileW(iniPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, 0, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER size = {};
        GetFileSizeEx(file, &size);
        if (size.QuadPart > 0 && size.QuadPart < 1024 * 1024) {
            oldBytes.resize((size_t)size.QuadPart);
            DWORD read = 0;
            ReadFile(file, oldBytes.data(), (DWORD)oldBytes.size(), &read, nullptr);
            oldBytes.resize(read);
        }
        CloseHandle(file);
        if (oldBytes.size() >= 2 && (BYTE)oldBytes[0] == 0xFF && (BYTE)oldBytes[1] == 0xFE) {
            return;  // Already Unicode.
        }
    }

    // Convert the old text (if any) to Unicode, and write it back after the mark.
    std::wstring text;
    if (!oldBytes.empty()) {
        int length = MultiByteToWideChar(CP_ACP, 0, oldBytes.data(), (int)oldBytes.size(),
                                         nullptr, 0);
        text.resize(length);
        MultiByteToWideChar(CP_ACP, 0, oldBytes.data(), (int)oldBytes.size(), text.data(),
                            length);
    }
    text.insert(text.begin(), (WCHAR)0xFEFF);

    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_NORMAL);
    file = CreateFileW(iniPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(file, text.data(), (DWORD)(text.size() * sizeof(WCHAR)), &written, nullptr);
        CloseHandle(file);
    }
    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

// Windows won't let programs overwrite hidden system files, so the hidden
// and system flags come off while we write, and go back on afterwards.
void WriteGroupIniValue(const std::wstring& folderPath, PCWSTR section,
                        PCWSTR key, PCWSTR value) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    EnsureUnicodeGroupIni(folderPath);
    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_NORMAL);
    WritePrivateProfileStringW(section, key, value, iniPath.c_str());
    WritePrivateProfileStringW(nullptr, nullptr, nullptr, iniPath.c_str());  // Save now.
    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

// Lists the names in a group's [DesktopAppGroups] section (none if it has
// no such section).
std::vector<std::wstring> ReadGroupIniNames(const std::wstring& folderPath) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    std::vector<WCHAR> buffer(8 * 1024, L'\0');
    // Asking for a value with no name (nullptr) gives the list of names,
    // each ending with a 0.
    DWORD length = GetPrivateProfileStringW(kGroupIniSection, nullptr, L"", buffer.data(),
                                            (DWORD)buffer.size(), iniPath.c_str());
    std::vector<std::wstring> names;
    for (DWORD start = 0; start < length;) {
        std::wstring name = buffer.data() + start;
        if (name.empty()) {
            break;
        }
        names.push_back(name);
        start += (DWORD)name.size() + 1;
    }
    return names;
}

// --- Group data in Windhawk's storage ----------------------------------------
//
// Each group's data is one stored text value, named "Group-" plus the
// folder's ID, holding "name=value" entries (the same names as in
// desktop.ini), each ending with "*":
//   IsGroup=1*Order=Notepad.lnk|Paint.lnk*Tint=255*
// "*" can't appear in a file name, so it can't appear in a value. (Not line
// breaks: portable Windhawk keeps its storage in an .ini file, which can't
// hold them.) A list of all the IDs is kept as well, in "GroupList", so the
// mod can tidy up after groups deleted while it wasn't running.
//
// Two things can get long: a folder's saved order (it names every item, and
// with "Open other desktop folders" a folder can hold thousands), and the
// list of IDs. Windhawk can't keep a value that long (portable Windhawk only
// reads back up to 32,767 characters), so those are kept separately, split
// into pieces (see WriteLongStoredText). That keeps each group's main value,
// with "IsGroup" and its settings, small.
//
// Windhawk's storage for a mod is shared by everyone who uses the PC, but
// each person has their own desktop and their own groups. So the names of
// these values start with the ID of your Windows account (your "SID", like
// "S-1-5-21-...-1001-"). Each account has exactly one Explorer showing its
// desktop, so that Explorer is the only one writing its values.
//
// Windhawk doesn't promise its storage functions are safe to use from two
// threads at once, and both the desktop's thread and the group watcher need
// group data. So every use of the storage holds g_storageLock, and a copy of
// all group data is kept in memory (g_groupData) for quick reading.

std::mutex g_storageLock;
using GroupValues = std::map<std::wstring, std::wstring>;
std::map<std::wstring, GroupValues> g_groupData;  // Folder ID -> its values.
// The order last saved (in pieces) for each folder, so it's only written
// again when it changes.
std::map<std::wstring, std::wstring> g_savedOrders;

// The start of the names of this account's values (see above). Set once in
// Wh_ModInit. "" if Windows couldn't tell us the account's ID.
std::wstring g_userPrefix;

std::wstring CurrentUserPrefix() {
    std::wstring prefix;
    HANDLE token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD size = 0;
        PWSTR sid = nullptr;
        if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buffer)->User.Sid, &sid)) {
            prefix = std::wstring(sid) + L"-";
            LocalFree(sid);
        }
        CloseHandle(token);
    }
    return prefix;
}

// The full stored name of one of this account's values.
std::wstring UserValueName(const std::wstring& name) {
    return g_userPrefix + name;
}

// Reads a stored text value ("" if there's none). Hold g_storageLock. Long
// text is saved in pieces (see WriteLongStoredText), so values stay well
// under this size.
std::wstring ReadStoredText(const std::wstring& name) {
    std::vector<WCHAR> buffer(64 * 1024, L'\0');
    Wh_GetStringValue(name.c_str(), buffer.data(), buffer.size());
    return buffer.data();
}

// Saves a text value, and checks it reads back the same. Hold g_storageLock.
bool WriteStoredText(const std::wstring& name, const std::wstring& text) {
    if (!Wh_SetStringValue(name.c_str(), text.c_str()) || ReadStoredText(name) != text) {
        Wh_Log(L"Couldn't save \"%s\" in Windhawk's storage", name.c_str());
        return false;
    }
    return true;
}

// --- Long text, in pieces ---
//
// Long text is split into pieces of at most 30,000 characters, saved as
// "<name>-<generation>-0", "<name>-<generation>-1" and so on. "<name>-Pieces"
// says which generation is current (0 or 1) and how many pieces it has, as
// generation x 1,000,000 + count. A save writes the *other* generation's
// pieces first, then switches "-Pieces" over in a single write, and only
// then deletes the old pieces. So if Explorer stops halfway through, the old
// text is still whole. Each piece is wrapped in "*" characters, so portable
// Windhawk's .ini file can't trim spaces off its ends.

constexpr size_t kPieceLength = 30000;
constexpr int kGenerationStep = 1000000;

// Reads long text. Sets `found` to whether it was ever saved. Hold
// g_storageLock.
std::wstring ReadLongStoredText(const std::wstring& name, bool* found = nullptr) {
    int pieces = Wh_GetIntValue((name + L"-Pieces").c_str(), -1);
    if (found) {
        *found = pieces >= 0;
    }
    if (pieces <= 0) {
        return L"";
    }
    int generation = pieces / kGenerationStep, count = pieces % kGenerationStep;
    std::wstring text;
    for (int i = 0; i < count; i++) {
        std::wstring piece = ReadStoredText(name + L"-" + std::to_wstring(generation) + L"-" +
                                            std::to_wstring(i));
        if (piece.size() < 2 || piece.front() != L'*' || piece.back() != L'*') {
            Wh_Log(L"A piece of \"%s\" is missing", name.c_str());
            return L"";
        }
        text += piece.substr(1, piece.size() - 2);
    }
    return text;
}

void DeletePieces(const std::wstring& name, int generation, int count) {
    for (int i = 0; i < count; i++) {
        Wh_DeleteValue(
            (name + L"-" + std::to_wstring(generation) + L"-" + std::to_wstring(i)).c_str());
    }
}

// Saves long text (see above). If it fails, the old text is kept. Hold
// g_storageLock.
bool WriteLongStoredText(const std::wstring& name, const std::wstring& text) {
    int old = std::max(0, Wh_GetIntValue((name + L"-Pieces").c_str(), 0));
    int oldGeneration = old / kGenerationStep, oldCount = old % kGenerationStep;
    int generation = 1 - oldGeneration;
    int count = (int)((text.size() + kPieceLength - 1) / kPieceLength);

    for (int i = 0; i < count; i++) {
        std::wstring piece = L"*" + text.substr(i * kPieceLength, kPieceLength) + L"*";
        if (!WriteStoredText(name + L"-" + std::to_wstring(generation) + L"-" +
                                 std::to_wstring(i),
                             piece)) {
            DeletePieces(name, generation, i + 1);
            return false;
        }
    }
    int pieces = generation * kGenerationStep + count;
    if (!Wh_SetIntValue((name + L"-Pieces").c_str(), pieces) ||
        Wh_GetIntValue((name + L"-Pieces").c_str(), -1) != pieces) {
        Wh_Log(L"Couldn't save \"%s\" in Windhawk's storage", name.c_str());
        DeletePieces(name, generation, count);
        return false;
    }
    DeletePieces(name, oldGeneration, oldCount);
    return true;
}

void DeleteLongStoredText(const std::wstring& name) {
    int old = Wh_GetIntValue((name + L"-Pieces").c_str(), -1);
    if (old < 0) {
        return;
    }
    DeletePieces(name, old / kGenerationStep, old % kGenerationStep);
    Wh_DeleteValue((name + L"-Pieces").c_str());
}

// For single values of this account's (UnfinishedGroup), without holding
// the lock.
std::wstring LoadStoredText(PCWSTR name) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    return ReadStoredText(UserValueName(name));
}

void StoreText(PCWSTR name, PCWSTR text) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    WriteStoredText(UserValueName(name), text);
}

void DeleteStoredValue(PCWSTR name) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    Wh_DeleteValue(UserValueName(name).c_str());
}

// A file or folder's ID, as text: "V", its drive's serial number (16
// hexadecimal digits), "F", and its file ID (32 hexadecimal digits). "" if
// Windows can't tell us (for example, the drive doesn't have file IDs).
std::wstring FileIdText(HANDLE file) {
    FILE_ID_INFO info = {};
    if (!GetFileInformationByHandleEx(file, FileIdInfo, &info, sizeof(info))) {
        return L"";
    }
    bool allZero = true;
    for (BYTE b : info.FileId.Identifier) {
        allZero = allZero && b == 0;
    }
    if (allZero) {
        return L"";  // Some drives answer, but with no real ID.
    }
    const WCHAR digits[] = L"0123456789ABCDEF";
    auto hex = [&](const BYTE* bytes, size_t count) {
        std::wstring text;
        for (size_t i = 0; i < count; i++) {
            text += digits[bytes[i] >> 4];
            text += digits[bytes[i] & 15];
        }
        return text;
    };
    return L"V" + hex((const BYTE*)&info.VolumeSerialNumber, sizeof(info.VolumeSerialNumber)) +
           L"F" + hex(info.FileId.Identifier, sizeof(info.FileId.Identifier));
}

// Opens a folder just to read its information. FILE_FLAG_BACKUP_SEMANTICS
// is what allows opening folders (not just files).
HANDLE OpenForInfo(const std::wstring& path) {
    return CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
                       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                       OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
}

std::wstring GroupIdOf(const std::wstring& folderPath) {
    HANDLE folder = OpenForInfo(folderPath);
    if (folder == INVALID_HANDLE_VALUE) {
        return L"";
    }
    std::wstring id = FileIdText(folder);
    CloseHandle(folder);
    return id;
}

// Turns a stored group back into its values.
GroupValues ParseStoredGroup(const std::wstring& text) {
    GroupValues values;
    for (size_t start = 0; start < text.size();) {
        // Entries end with "*". (Line breaks are accepted too, from test
        // versions of the mod.)
        size_t end = text.find_first_of(L"*\n", start);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        std::wstring entry = text.substr(start, end - start);
        start = end + 1;
        size_t equals = entry.find(L'=');
        if (equals != std::wstring::npos && equals > 0) {
            values[entry.substr(0, equals)] = entry.substr(equals + 1);
        }
    }
    return values;
}

// Splits a list of IDs separated by "|".
std::vector<std::wstring> SplitIdList(const std::wstring& list) {
    std::vector<std::wstring> ids;
    for (size_t start = 0; start < list.size();) {
        size_t end = list.find(L'|', start);
        if (end == std::wstring::npos) {
            end = list.size();
        }
        if (end > start) {
            ids.push_back(list.substr(start, end - start));
        }
        start = end + 1;
    }
    return ids;
}

bool SaveGroupIdList();
bool SaveStoredGroup(const std::wstring& id);

// Loads all of this account's stored group data into memory. Called once, at
// startup.
void LoadStoredGroups() {
    std::lock_guard<std::mutex> lock(g_storageLock);
    g_groupData.clear();
    g_savedOrders.clear();

    // The list of IDs. Earlier versions kept it as one value, "Groups"; it's
    // saved in pieces from now on.
    bool listInPieces = false;
    std::wstring list = ReadLongStoredText(UserValueName(L"GroupList"), &listInPieces);
    if (!listInPieces) {
        list = ReadStoredText(UserValueName(L"Groups"));
    }
    for (const std::wstring& id : SplitIdList(list)) {
        GroupValues values = ParseStoredGroup(ReadStoredText(UserValueName(L"Group-" + id)));
        // The order, kept in pieces. (Earlier versions kept it in the main
        // value, which ParseStoredGroup has already read; it moves to
        // pieces the next time the group is saved.)
        bool orderInPieces = false;
        std::wstring order = ReadLongStoredText(UserValueName(L"Order-" + id), &orderInPieces);
        if (orderInPieces && !order.empty()) {
            values[L"Order"] = order;
            g_savedOrders[id] = order;
        }
        if (!values.empty()) {
            g_groupData[id] = values;
        }
    }
    if (!listInPieces && !g_groupData.empty() && SaveGroupIdList()) {
        Wh_DeleteValue(UserValueName(L"Groups").c_str());
    }

    // Test versions of the mod saved groups without the account's ID in the
    // names. Take those over, under this account's names.
    if (g_userPrefix.empty()) {
        return;
    }
    std::vector<std::wstring> oldIds = SplitIdList(ReadStoredText(L"Groups"));
    if (oldIds.empty()) {
        return;
    }
    bool allSaved = true;
    for (const std::wstring& id : oldIds) {
        GroupValues values = ParseStoredGroup(ReadStoredText(L"Group-" + id));
        if (!values.empty() && g_groupData.find(id) == g_groupData.end()) {
            g_groupData[id] = values;
            allSaved = SaveStoredGroup(id) && allSaved;
        }
    }
    if (allSaved && SaveGroupIdList()) {
        for (const std::wstring& id : oldIds) {
            Wh_DeleteValue((L"Group-" + id).c_str());
        }
        Wh_DeleteValue(L"Groups");
        Wh_Log(L"Moved %d group(s) from an earlier test version", (int)oldIds.size());
    }
}

// Saves the list of IDs. Hold g_storageLock.
bool SaveGroupIdList() {
    std::wstring list;
    for (const auto& group : g_groupData) {
        if (!list.empty()) {
            list += L"|";
        }
        list += group.first;
    }
    return WriteLongStoredText(UserValueName(L"GroupList"), list);
}

// Saves one group's data from memory: the order in pieces (only if it
// changed), and everything else in the group's main value. The main value
// is always saved, even if the order couldn't be, so the group never loses
// "IsGroup" or its settings. Hold g_storageLock.
bool SaveStoredGroup(const std::wstring& id) {
    const GroupValues& values = g_groupData[id];
    auto order = values.find(L"Order");
    std::wstring orderText = order != values.end() ? order->second : L"";
    auto saved = g_savedOrders.find(id);
    bool orderSaved = true;
    if (saved == g_savedOrders.end() || saved->second != orderText) {
        std::wstring name = UserValueName(L"Order-" + id);
        if (orderText.empty()) {
            DeleteLongStoredText(name);
            g_savedOrders.erase(id);
        } else if (WriteLongStoredText(name, orderText)) {
            g_savedOrders[id] = orderText;
        } else {
            orderSaved = false;
        }
    }

    std::wstring text;
    for (const auto& value : values) {
        if (value.first != L"Order") {
            text += value.first + L"=" + value.second + L"*";
        }
    }
    return WriteStoredText(UserValueName(L"Group-" + id), text) && orderSaved;
}

// Removes a group's stored values (not its data in memory). Hold
// g_storageLock.
void DeleteStoredGroup(const std::wstring& id) {
    Wh_DeleteValue(UserValueName(L"Group-" + id).c_str());
    DeleteLongStoredText(UserValueName(L"Order-" + id));
    g_savedOrders.erase(id);
}

bool GetStoredGroupValue(const std::wstring& id, PCWSTR name, std::wstring* value) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    auto group = g_groupData.find(id);
    if (group == g_groupData.end()) {
        return false;
    }
    auto entry = group->second.find(name);
    if (entry == group->second.end()) {
        return false;
    }
    *value = entry->second;
    return true;
}

// Sets (or, with nullptr, removes) one value of a group's stored data.
void SetStoredGroupValue(const std::wstring& id, PCWSTR name, PCWSTR value) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    bool isNew = g_groupData.find(id) == g_groupData.end();
    if (value) {
        g_groupData[id][name] = value;
    } else if (isNew) {
        return;
    } else {
        g_groupData[id].erase(name);
        if (g_groupData[id].empty()) {
            g_groupData.erase(id);
            DeleteStoredGroup(id);
            SaveGroupIdList();
            return;
        }
    }
    // (A failed save is logged by WriteStoredText. The value stays in
    // memory, so it still works until Explorer restarts.)
    if (SaveStoredGroup(id) && isNew) {
        SaveGroupIdList();
    }
}

// Removes all of a group's stored data, when the group is removed.
void ForgetStoredGroup(const std::wstring& id) {
    if (id.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_storageLock);
    if (g_groupData.erase(id)) {
        DeleteStoredGroup(id);
        SaveGroupIdList();
    }
}

// --- Group data, wherever it's kept ------------------------------------------

// Where a folder's data belongs. A group's follows the "Keep group data
// inside the group folders" setting. Other folders (opened in the popup with
// "Open other desktop folders") only ever use Windhawk's storage, so the mod
// never writes into folders of your own; without a file ID, their settings
// just aren't kept.
enum class DataPlace { Folder, Storage, Nowhere };

DataPlace PlaceForData(const std::wstring& id, bool isGroup) {
    if (!isGroup) {
        return id.empty() ? DataPlace::Nowhere : DataPlace::Storage;
    }
    return (g_dataInFolder || id.empty()) ? DataPlace::Folder : DataPlace::Storage;
}

// Moves a group's data to where the setting says it belongs, if it's in the
// other place. Each value is copied first, and only removed from the old
// place once it's safely saved in the new one.
void MoveGroupData(const std::wstring& folderPath, const std::wstring& id, bool isGroup) {
    DataPlace place = PlaceForData(id, isGroup);
    if (place == DataPlace::Nowhere) {
        return;
    }
    if (place == DataPlace::Folder) {
        if (id.empty()) {
            return;
        }
        GroupValues values;
        {
            std::lock_guard<std::mutex> lock(g_storageLock);
            auto group = g_groupData.find(id);
            if (group == g_groupData.end()) {
                return;  // Nothing stored: already in the folder, or not a group.
            }
            values = group->second;
        }
        for (const auto& value : values) {
            WriteGroupIniValue(folderPath, kGroupIniSection, value.first.c_str(),
                               value.second.c_str());
        }
        if (ReadGroupIniNames(folderPath).size() < values.size()) {
            Wh_Log(L"Couldn't move the data for \"%s\" into its folder", folderPath.c_str());
            return;
        }
        ForgetStoredGroup(id);
        Wh_Log(L"Moved the data for \"%s\" into its desktop.ini", folderPath.c_str());
    } else {
        std::vector<std::wstring> names = ReadGroupIniNames(folderPath);
        if (names.empty()) {
            return;  // Nothing in desktop.ini: already in storage, or not a group.
        }
        {
            std::lock_guard<std::mutex> lock(g_storageLock);
            bool isNew = g_groupData.find(id) == g_groupData.end();
            for (const std::wstring& name : names) {
                g_groupData[id][name] = ReadGroupIniValue(folderPath, kGroupIniSection,
                                                          name.c_str());
            }
            if (!SaveStoredGroup(id)) {
                Wh_Log(L"Couldn't save the data for \"%s\" in storage", folderPath.c_str());
                return;
            }
            if (isNew) {
                SaveGroupIdList();
            }
        }
        // Writing "no name" (nullptr) removes the whole section.
        WriteGroupIniValue(folderPath, kGroupIniSection, nullptr, nullptr);
        Wh_Log(L"Moved the data for \"%s\" into Windhawk's storage", folderPath.c_str());
    }
}

// Reads one value of a group's data ("" if it has none).
std::wstring ReadGroupValue(const std::wstring& folderPath, PCWSTR name) {
    std::wstring id = GroupIdOf(folderPath);
    std::wstring value;
    if (!id.empty() && GetStoredGroupValue(id, name, &value)) {
        return value;
    }
    return ReadGroupIniValue(folderPath, kGroupIniSection, name);
}

bool IsGroupFolder(const std::wstring& folderPath) {
    return ReadGroupValue(folderPath, L"IsGroup") == L"1";
}

// Sets (or, with nullptr, removes) one value of a group's (or other
// folder's) data, in the place it belongs (see PlaceForData). Any data still
// in the other place is moved first, so it's never split between the two.
void WriteGroupValue(const std::wstring& folderPath, PCWSTR name, PCWSTR value) {
    std::wstring id = GroupIdOf(folderPath);
    bool markingAsGroup = wcscmp(name, L"IsGroup") == 0 && value && wcscmp(value, L"1") == 0;
    bool isGroup = markingAsGroup || IsGroupFolder(folderPath);
    MoveGroupData(folderPath, id, isGroup);
    switch (PlaceForData(id, isGroup)) {
        case DataPlace::Folder:
            WriteGroupIniValue(folderPath, kGroupIniSection, name, value);
            break;
        case DataPlace::Storage:
            SetStoredGroupValue(id, name, value);
            break;
        case DataPlace::Nowhere:
            break;
    }
}

void MarkAsGroup(const std::wstring& folderPath) {
    WriteGroupValue(folderPath, L"IsGroup", L"1");

    // Windows only reads a folder's desktop.ini (for the icon) if the folder
    // has the "read-only" flag. (For folders this flag doesn't make anything
    // read-only; it just means "has custom settings".)
    DWORD folderAttributes = GetFileAttributesW(folderPath.c_str());
    if (folderAttributes != INVALID_FILE_ATTRIBUTES) {
        SetFileAttributesW(folderPath.c_str(), folderAttributes | FILE_ATTRIBUTE_READONLY);
    }
}

// --- Group settings ----------------------------------------------------------
//
// The settings you change with the gear button in the popup. Each one can be
// set for all groups, or for one group only:
//
//   - The all-groups value is kept in the mod's own storage (Wh_SetIntValue),
//     which Windhawk keeps safe between restarts.
//   - A group's own value is kept with the rest of the group's data (see the
//     start of this section). A group with no value of its own uses the
//     all-groups one.

enum GroupSettingId {
    kSettingColumns,
    kSettingIconSize,
    kSettingShowNames,
    kSettingSort,
    kSettingRemoveWhenOneLeft,
    kSettingTint,
    kSettingView,
    kSettingSpacing,
    kGroupSettingCount
};

struct GroupSettingInfo {
    PCWSTR key;        // Its name in storage and in desktop.ini.
    int defaultValue;
};

const GroupSettingInfo kGroupSettings[kGroupSettingCount] = {
    {L"Columns", 4},            // Apps per row, 3 to 6.
    {L"IconSize", 48},          // 32, 48 or 64 pixels.
    {L"ShowNames", 1},          // 1 = show app names under the icons.
    {L"Sort", 0},               // 0 = your arrangement, 1 = A to Z.
    {L"RemoveWhenOneLeft", 1},  // 1 = remove a group that's down to one app.
    {L"Tint", -1},              // The group's colour, or -1 for none.
    {L"View", 0},               // 0 = icons in a grid, 1 = a list.
    {L"Spacing", 1},            // 0 = compact, 1 = normal, 2 = roomy.
};

// Group colours. A colour is stored as a COLORREF, a number of the form
// 0x00BBGGRR (blue, green, red), which is never negative, so -1 can mean
// "no colour". These are the preset swatches in the settings panel; any
// other colour can be picked with "Custom".
constexpr int kNoTint = -1;
constexpr int kCustomTint = -2;  // Only used for the "Custom" swatch itself.
const COLORREF kTintPresets[] = {
    RGB(209, 52, 56),   // Red
    RGB(247, 99, 12),   // Orange
    RGB(255, 185, 0),   // Yellow
    RGB(16, 137, 62),   // Green
    RGB(0, 153, 188),   // Teal
    RGB(0, 120, 212),   // Blue
    RGB(136, 23, 152),  // Purple
    RGB(227, 0, 140),   // Pink
};

bool IsPresetTint(int tint) {
    for (COLORREF preset : kTintPresets) {
        if ((int)preset == tint) {
            return true;
        }
    }
    return false;
}

// The all-groups values, kept in memory so they're quick to read from any
// thread. ("atomic" makes it safe for one thread to read while another
// writes.) Like group data, they're stored separately for each Windows
// account (see "Group data in Windhawk's storage" above).
std::atomic<int> g_allGroupsValues[kGroupSettingCount];

void LoadAllGroupsSettings() {
    std::lock_guard<std::mutex> lock(g_storageLock);
    for (int id = 0; id < kGroupSettingCount; id++) {
        // If this account hasn't saved one yet, start from the value saved by
        // an earlier version (which wasn't per account), or the default.
        int earlierValue =
            Wh_GetIntValue(kGroupSettings[id].key, kGroupSettings[id].defaultValue);
        g_allGroupsValues[id] =
            Wh_GetIntValue(UserValueName(kGroupSettings[id].key).c_str(), earlierValue);
    }
}

int GetAllGroupsSetting(GroupSettingId id) {
    return g_allGroupsValues[id];
}

void SetAllGroupsSetting(GroupSettingId id, int value) {
    g_allGroupsValues[id] = value;
    std::lock_guard<std::mutex> lock(g_storageLock);
    Wh_SetIntValue(UserValueName(kGroupSettings[id].key).c_str(), value);
}

// Does this group have its own value for a setting? If so, it's put in `value`.
bool GetGroupOwnSetting(const std::wstring& folderPath, GroupSettingId id, int* value) {
    std::wstring text = ReadGroupValue(folderPath, kGroupSettings[id].key);
    if (text.empty()) {
        return false;
    }
    *value = _wtoi(text.c_str());
    return true;
}

void SetGroupOwnSetting(const std::wstring& folderPath, GroupSettingId id, int value) {
    WriteGroupValue(folderPath, kGroupSettings[id].key, std::to_wstring(value).c_str());
}

void ClearGroupOwnSetting(const std::wstring& folderPath, GroupSettingId id) {
    WriteGroupValue(folderPath, kGroupSettings[id].key, nullptr);  // nullptr: remove it.
}

// The value that applies to a group: its own if it has one, otherwise the
// all-groups value.
int GetGroupSetting(const std::wstring& folderPath, GroupSettingId id) {
    int value;
    if (GetGroupOwnSetting(folderPath, id, &value)) {
        return value;
    }
    return GetAllGroupsSetting(id);
}

// (CreateGroup is at the end of section 6, because it draws the group's
// icon.)

struct GroupEntry {
    std::wstring name;  // What Explorer shows, e.g. "Notepad" (no ".lnk").
    std::wstring path;  // Full path on disk.
    bool isFolder = false;
    ULONGLONG size = 0;    // In bytes, for files.
    size_t position = 0;   // Its place in the saved order (used for sorting).
};

std::wstring FileNameOf(const std::wstring& path) {
    return path.substr(path.find_last_of(L'\\') + 1);
}

// Lists what's in a group, in the order you arranged them in the popup.
// Anything not in the saved order (like apps added since) goes at the end,
// folders first, then sorted the way Explorer sorts ("App 2" before
// "App 10"). Hidden files, like desktop.ini and the icon, are skipped.
//
// The order is saved with the group's data as file names separated by "|",
// a character that can't appear in file names:
//   Order=Notepad.lnk|Paint.lnk|Calculator.lnk
std::vector<GroupEntry> ListGroupFiles(const std::wstring& folderPath) {
    std::vector<GroupEntry> entries;

    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((folderPath + L"\\*").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return entries;
    }

    do {
        std::wstring fileName = found.cFileName;
        if (fileName == L"." || fileName == L".." ||
            (found.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            continue;
        }

        GroupEntry entry;
        entry.path = folderPath + L"\\" + fileName;
        entry.name = fileName;
        entry.isFolder = (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        entry.size = ((ULONGLONG)found.nFileSizeHigh << 32) | found.nFileSizeLow;

        ComPtr<IShellItem> shellItem;
        PWSTR displayName = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(entry.path.c_str(), nullptr,
                                                  IID_PPV_ARGS(&shellItem))) &&
            SUCCEEDED(shellItem->GetDisplayName(SIGDN_NORMALDISPLAY, &displayName))) {
            entry.name = displayName;
            CoTaskMemFree(displayName);
        }

        entries.push_back(entry);
    } while (FindNextFileW(search, &found));
    FindClose(search);

    // Turn the saved order into "file name -> position". Names are compared
    // without caring about capitals, like Windows does.
    // With the "A to Z" setting, the saved order is ignored.
    std::vector<std::wstring> savedOrder;
    std::wstring orderText;
    if (GetGroupSetting(folderPath, kSettingSort) == 0) {
        orderText = ReadGroupValue(folderPath, L"Order");
    }
    for (size_t start = 0; start < orderText.size();) {
        size_t end = orderText.find(L'|', start);
        if (end == std::wstring::npos) {
            end = orderText.size();
        }
        savedOrder.push_back(orderText.substr(start, end - start));
        start = end + 1;
    }
    // Look each entry up once (not on every comparison while sorting, which
    // would be slow for a big folder). Names are compared in lower case, so
    // capitals don't matter, as in Windows.
    auto lowerCase = [](std::wstring text) {
        CharLowerBuffW(text.data(), (DWORD)text.size());
        return text;
    };
    std::map<std::wstring, size_t> savedPositions;
    for (size_t i = 0; i < savedOrder.size(); i++) {
        savedPositions.emplace(lowerCase(savedOrder[i]), i);
    }
    for (GroupEntry& entry : entries) {
        auto saved = savedPositions.find(lowerCase(FileNameOf(entry.path)));
        // Not in the saved order: after the rest.
        entry.position = saved != savedPositions.end() ? saved->second : savedOrder.size();
    }

    std::sort(entries.begin(), entries.end(), [](const GroupEntry& a, const GroupEntry& b) {
        if (a.position != b.position) {
            return a.position < b.position;
        }
        if (a.isFolder != b.isFolder) {
            return a.isFolder;
        }
        return StrCmpLogicalW(a.name.c_str(), b.name.c_str()) < 0;
    });
    return entries;
}

// Gets an item's icon as a picture of the given size, with see-through edges.
HBITMAP GetItemIcon(const std::wstring& path, int pixels) {
    ComPtr<IShellItem> shellItem;
    ComPtr<IShellItemImageFactory> imageFactory;
    HBITMAP bitmap = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                              IID_PPV_ARGS(&shellItem))) &&
        SUCCEEDED(shellItem.As(&imageFactory))) {
        SIZE size = {pixels, pixels};
        imageFactory->GetImage(size, SIIGBF_ICONONLY, &bitmap);
    }
    return bitmap;
}

// ---- 6. Group icons ---------------------------------------------------------
//
// A group's icon is a rounded, see-through tile with the first four apps
// inside in a 2x2 grid, like a Start menu folder.
//
// Windows shows desktop icons at many sizes (small, medium, large, and scaled
// for high-DPI screens), so an .ico file holds several pictures: 16x16 up to
// 256x256. We draw each size separately, so small sizes stay sharp.
//
// We draw pixel by pixel instead of using GDI, because GDI can't draw
// see-through colours. Each pixel has four parts: blue, green, red, and alpha
// (how opaque it is). We use "premultiplied" alpha, the format Windows icons
// come in: each colour is already multiplied by the pixel's opacity. That
// makes layering one picture over another a simple formula (see BlendPixel).

// One pixel, in the order Windows stores them.
struct Pixel {
    BYTE b, g, r, a;
};

// Text drawn as a "coverage map": for each pixel, how much of it the text
// covers (0 = none, 255 = fully). See RenderTextMask in section 9.
struct TextMask {
    int width = 0;
    int height = 0;
    std::vector<BYTE> coverage;
};

// An app's icon, ready to draw: a square picture, in premultiplied pixels.
// See LoadItemIcon (after IconCanvas) for how it's made smooth.
struct IconImage {
    int size = 0;
    std::vector<Pixel> pixels;
};

// A picture we draw into pixel by pixel. Used for group icons (square), and
// for the popup and settings panel (any size), because it can draw
// see-through colours, which their blurred backgrounds need.
class IconCanvas {
public:
    explicit IconCanvas(int size) : IconCanvas(size, size) {}
    IconCanvas(int width, int height)
        : m_width(width), m_height(height),
          m_pixels((size_t)width * height, Pixel{0, 0, 0, 0}) {}

    int Size() const { return m_width; }  // For square icons.
    int Width() const { return m_width; }
    int Height() const { return m_height; }
    const std::vector<Pixel>& Pixels() const { return m_pixels; }

    // From now on, only draw on rows top to bottom (not including bottom),
    // fading out over `fadeTop` and `fadeBottom` rows at the edges. The
    // popup's scrolling area uses this, so apps scrolled partly out of view
    // fade softly instead of being cut off.
    void SetClip(int top, int bottom, int fadeTop, int fadeBottom) {
        m_clipped = true;
        m_clipTop = top;
        m_clipBottom = bottom;
        m_fadeTop = fadeTop;
        m_fadeBottom = fadeBottom;
    }

    void ClearClip() { m_clipped = false; }

    // Layer a (premultiplied) pixel over what's already there.
    void BlendPixel(int x, int y, Pixel source) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
            return;
        }
        if (m_clipped) {
            if (y < m_clipTop || y >= m_clipBottom) {
                return;
            }
            float strength = 1;
            if (m_fadeTop > 0 && y < m_clipTop + m_fadeTop) {
                strength = (y - m_clipTop + 0.5f) / m_fadeTop;
            }
            if (m_fadeBottom > 0 && y >= m_clipBottom - m_fadeBottom) {
                strength = std::min(strength, (m_clipBottom - y - 0.5f) / m_fadeBottom);
            }
            if (strength < 1) {
                source = {(BYTE)(source.b * strength), (BYTE)(source.g * strength),
                          (BYTE)(source.r * strength), (BYTE)(source.a * strength)};
            }
        }
        Pixel& target = m_pixels[(size_t)y * m_width + x];
        int keep = 255 - source.a;  // How much of the old pixel shows through.
        target.b = (BYTE)(source.b + target.b * keep / 255);
        target.g = (BYTE)(source.g + target.g * keep / 255);
        target.r = (BYTE)(source.r + target.r * keep / 255);
        target.a = (BYTE)(source.a + target.a * keep / 255);
    }

    // Layer a colour at a given strength (0 to 1) over a pixel.
    void BlendColor(int x, int y, COLORREF color, float opacity) {
        if (opacity <= 0) {
            return;
        }
        BYTE a = (BYTE)std::lround(std::min(opacity, 1.0f) * 255);
        BlendPixel(x, y, {(BYTE)(GetBValue(color) * a / 255),
                          (BYTE)(GetGValue(color) * a / 255),
                          (BYTE)(GetRValue(color) * a / 255), a});
    }

    // Paint every pixel one solid colour.
    void FillAll(COLORREF color) {
        for (Pixel& p : m_pixels) {
            p = {GetBValue(color), GetGValue(color), GetRValue(color), 255};
        }
    }

    // A rounded rectangle with soft (anti-aliased) edges, plus an optional
    // border. For each pixel we work out how far it is from the shape's
    // edge: inside is negative, outside positive. Pixels right on the edge
    // get partly coloured, which is what makes the edge look smooth.
    void FillRoundedRect(float left, float top, float right, float bottom,
                         float radius, COLORREF fill, float fillOpacity,
                         COLORREF border = 0, float borderOpacity = 0,
                         float borderWidth = 0) {
        float centerX = (left + right) / 2, centerY = (top + bottom) / 2;
        float halfWidth = (right - left) / 2, halfHeight = (bottom - top) / 2;

        // Only visit pixels near the shape, not the whole picture.
        int firstX = std::max(0, (int)std::floor(left) - 1);
        int lastX = std::min(m_width - 1, (int)std::ceil(right) + 1);
        int firstY = std::max(0, (int)std::floor(top) - 1);
        int lastY = std::min(m_height - 1, (int)std::ceil(bottom) + 1);

        for (int y = firstY; y <= lastY; y++) {
            for (int x = firstX; x <= lastX; x++) {
                float qx = std::fabs(x + 0.5f - centerX) - (halfWidth - radius);
                float qy = std::fabs(y + 0.5f - centerY) - (halfHeight - radius);
                float outside = std::hypot(std::max(qx, 0.0f), std::max(qy, 0.0f));
                float distance = outside + std::min(std::max(qx, qy), 0.0f) - radius;

                float inShape = std::clamp(0.5f - distance, 0.0f, 1.0f);
                float inInner = std::clamp(0.5f - (distance + borderWidth), 0.0f, 1.0f);

                BlendColor(x, y, fill, fillOpacity * inShape);
                if (borderOpacity > 0) {
                    BlendColor(x, y, border, borderOpacity * (inShape - inInner));
                }
            }
        }
    }

    // Draw text (as a coverage map) in a colour, with its top-left at
    // (left, top).
    void BlendMask(const TextMask& mask, int left, int top, COLORREF color) {
        for (int y = 0; y < mask.height; y++) {
            for (int x = 0; x < mask.width; x++) {
                BYTE covered = mask.coverage[(size_t)y * mask.width + x];
                if (covered) {
                    BlendColor(left + x, top + y, color, covered / 255.0f);
                }
            }
        }
    }

    // Draw an icon at (left, top), at its own size.
    void DrawImage(const IconImage& image, int left, int top) {
        for (int y = 0; y < image.size; y++) {
            for (int x = 0; x < image.size; x++) {
                const Pixel& pixel = image.pixels[(size_t)y * image.size + x];
                if (pixel.a) {
                    BlendPixel(left + x, top + y, pixel);
                }
            }
        }
    }

    // PNG files store colours not multiplied by opacity ("straight" alpha),
    // so undo the premultiplication.
    std::vector<Pixel> StraightPixels() const {
        std::vector<Pixel> result = m_pixels;
        for (Pixel& p : result) {
            if (p.a > 0 && p.a < 255) {
                p.b = (BYTE)std::min(255, p.b * 255 / p.a);
                p.g = (BYTE)std::min(255, p.g * 255 / p.a);
                p.r = (BYTE)std::min(255, p.r * 255 / p.a);
            }
        }
        return result;
    }

private:
    int m_width;
    int m_height;
    std::vector<Pixel> m_pixels;
    bool m_clipped = false;
    int m_clipTop = 0, m_clipBottom = 0, m_fadeTop = 0, m_fadeBottom = 0;
};

// --- Smooth icons -------------------------------------------------------------
//
// To look as smooth as the Start menu's, icons are made the way it makes
// them: from a big, detailed version of the icon, shrunk down smoothly.
//
//   1. Ask Windows for the icon at 256x256 (or, for small icons, at the exact
//      size: icons often have small versions drawn specially to stay crisp).
//   2. Windows gives icons in "straight" alpha, where the colours aren't
//      multiplied by how opaque each pixel is. We draw in "premultiplied"
//      alpha (see BlendPixel), so convert. Without this, the soft edge pixels
//      come out at full colour, and edges look jagged.
//   3. Shrink it to the size needed by averaging: each new pixel is the
//      average of all the source pixels it covers ("area averaging"), so
//      every detail contributes and edges stay soft.

// Reads a Windows bitmap's pixels (top row first). Empty if it fails.
std::vector<Pixel> ReadBitmapPixels(HBITMAP bitmap, int* width, int* height) {
    BITMAP info;
    if (!GetObjectW(bitmap, sizeof(info), &info) || info.bmWidth <= 0) {
        return {};
    }
    *width = info.bmWidth;
    *height = std::abs(info.bmHeight);
    std::vector<Pixel> pixels((size_t)*width * *height);
    BITMAPINFO format = {};
    format.bmiHeader.biSize = sizeof(format.bmiHeader);
    format.bmiHeader.biWidth = *width;
    format.bmiHeader.biHeight = -*height;  // Negative = top row first.
    format.bmiHeader.biPlanes = 1;
    format.bmiHeader.biBitCount = 32;
    format.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    int rows = GetDIBits(screen, bitmap, 0, *height, pixels.data(), &format, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    if (rows != *height) {
        return {};
    }
    return pixels;
}

// Step 2: makes sure the pixels are premultiplied. In premultiplied pixels a
// colour can never be brighter than the opacity, so if any is, these are
// straight. (Old icons with no transparency at all come with every opacity
// at 0; those are made fully opaque.)
void PremultiplyIfNeeded(std::vector<Pixel>& pixels) {
    bool anyOpacity = false, anyColour = false, straight = false;
    for (const Pixel& p : pixels) {
        anyOpacity = anyOpacity || p.a;
        anyColour = anyColour || p.r || p.g || p.b;
        straight = straight || p.r > p.a || p.g > p.a || p.b > p.a;
    }
    if (!anyOpacity && anyColour) {
        for (Pixel& p : pixels) {
            p.a = 255;
        }
        return;
    }
    if (straight) {
        for (Pixel& p : pixels) {
            p.r = (BYTE)((p.r * p.a + 127) / 255);
            p.g = (BYTE)((p.g * p.a + 127) / 255);
            p.b = (BYTE)((p.b * p.a + 127) / 255);
        }
    }
}

// Step 3: resizes a square picture. Shrinking averages every source pixel a
// new pixel covers (partly covered ones count partly). Growing, which only
// happens if Windows gave us a smaller picture than we asked for, blends the
// four nearest pixels instead.
std::vector<Pixel> ResizeSquare(const std::vector<Pixel>& source, int from, int to) {
    if (from == to) {
        return source;
    }
    std::vector<Pixel> result((size_t)to * to);
    if (from < to) {
        for (int y = 0; y < to; y++) {
            float sy = std::clamp((y + 0.5f) * from / to - 0.5f, 0.0f, (float)(from - 1));
            int y0 = (int)sy, y1 = std::min(y0 + 1, from - 1);
            float fy = sy - y0;
            for (int x = 0; x < to; x++) {
                float sx = std::clamp((x + 0.5f) * from / to - 0.5f, 0.0f, (float)(from - 1));
                int x0 = (int)sx, x1 = std::min(x0 + 1, from - 1);
                float fx = sx - x0;
                const Pixel& a = source[(size_t)y0 * from + x0];
                const Pixel& b = source[(size_t)y0 * from + x1];
                const Pixel& c = source[(size_t)y1 * from + x0];
                const Pixel& d = source[(size_t)y1 * from + x1];
                auto mix = [&](BYTE Pixel::*channel) {
                    float top = a.*channel + (b.*channel - a.*channel) * fx;
                    float bottom = c.*channel + (d.*channel - c.*channel) * fx;
                    return (BYTE)std::lround(top + (bottom - top) * fy);
                };
                result[(size_t)y * to + x] = {mix(&Pixel::b), mix(&Pixel::g), mix(&Pixel::r),
                                              mix(&Pixel::a)};
            }
        }
        return result;
    }

    // Shrinking, in two passes: across each row, then down each column.
    // `weights` lists, for each new position, which source positions it
    // covers and by how much.
    struct Weight {
        int index;
        float amount;
    };
    std::vector<std::vector<Weight>> weights(to);
    float scale = (float)from / to;
    for (int i = 0; i < to; i++) {
        float start = i * scale, end = (i + 1) * scale;
        for (int s = (int)start; s < from && s < end; s++) {
            float covered = std::min(end, s + 1.0f) - std::max(start, (float)s);
            if (covered > 0) {
                weights[i].push_back({s, covered / scale});
            }
        }
    }

    std::vector<float> across((size_t)to * from * 4);  // `to` wide, `from` tall.
    for (int y = 0; y < from; y++) {
        for (int x = 0; x < to; x++) {
            float sum[4] = {};
            for (const Weight& w : weights[x]) {
                const Pixel& p = source[(size_t)y * from + w.index];
                sum[0] += p.b * w.amount;
                sum[1] += p.g * w.amount;
                sum[2] += p.r * w.amount;
                sum[3] += p.a * w.amount;
            }
            std::copy(sum, sum + 4, &across[((size_t)y * to + x) * 4]);
        }
    }
    for (int y = 0; y < to; y++) {
        for (int x = 0; x < to; x++) {
            float sum[4] = {};
            for (const Weight& w : weights[y]) {
                const float* p = &across[((size_t)w.index * to + x) * 4];
                for (int c = 0; c < 4; c++) {
                    sum[c] += p[c] * w.amount;
                }
            }
            auto channel = [&](int c) {
                return (BYTE)std::clamp((int)std::lround(sum[c]), 0, 255);
            };
            result[(size_t)y * to + x] = {channel(0), channel(1), channel(2), channel(3)};
        }
    }
    return result;
}

// Asks Windows for an item's icon at `request` x `request`, as premultiplied
// pixels (steps 1 and 2 above). Sets `side` to the size it came back at.
// Empty if Windows has no icon for it.
std::vector<Pixel> LoadIconPixels(const std::wstring& path, int request, int* side) {
    HBITMAP bitmap = GetItemIcon(path, request);
    if (!bitmap) {
        return {};
    }
    int width = 0, height = 0;
    std::vector<Pixel> source = ReadBitmapPixels(bitmap, &width, &height);
    DeleteObject(bitmap);
    if (source.empty()) {
        return {};
    }
    if (width != height) {
        // Not square (unusual for icons): use the middle square of it.
        int square = std::min(width, height);
        std::vector<Pixel> middle((size_t)square * square);
        int left = (width - square) / 2, top = (height - square) / 2;
        for (int y = 0; y < square; y++) {
            for (int x = 0; x < square; x++) {
                middle[(size_t)y * square + x] = source[(size_t)(y + top) * width + x + left];
            }
        }
        source.swap(middle);
        width = square;
    }
    PremultiplyIfNeeded(source);
    *side = width;
    return source;
}

// How much is drawn in a picture: every pixel's opacity, added up.
double TotalOpacity(const std::vector<Pixel>& pixels) {
    double total = 0;
    for (const Pixel& p : pixels) {
        total += p.a;
    }
    return total;
}

// An item's icon at `pixels` x `pixels`, made smooth as described above.
// Empty (size 0) if Windows has no icon for it.
//
// Some icons only come in small sizes (game shortcuts made by Steam, for
// example, are often 32x32 or smaller). Asked for those at 256x256, Windows
// doesn't enlarge them: it puts the small icon in the middle of a big empty
// box. So for big icons we also ask at 48x48, which Windows does fill, and
// compare. Shrunk to the same size, the same icon covers about the same
// amount either way; if the big one covers much less, it's a small icon in a
// box, and we enlarge the 48x48 one smoothly instead, like the Start menu.
IconImage LoadItemIcon(const std::wstring& path, int pixels) {
    IconImage image;
    if (pixels <= 0) {
        return image;
    }
    int request = pixels <= 32 ? pixels : std::max(256, pixels);
    int side = 0;
    std::vector<Pixel> source = LoadIconPixels(path, request, &side);
    if (source.empty()) {
        return image;
    }

    if (request > 48) {
        int smallSide = 0;
        std::vector<Pixel> small = LoadIconPixels(path, 48, &smallSide);
        if (!small.empty()) {
            double bigCoverage = TotalOpacity(ResizeSquare(source, side, smallSide));
            double smallCoverage = TotalOpacity(small);
            if (smallCoverage > 0 && bigCoverage < 0.6 * smallCoverage) {
                source.swap(small);
                side = smallSide;
            }
        }
    }

    image.pixels = ResizeSquare(source, side, pixels);
    image.size = pixels;
    return image;
}

// Draws the whole group icon at one size.
// A colour `amount` (0 to 1) of the way from `from` to `to`.
COLORREF MixColors(COLORREF from, COLORREF to, float amount) {
    auto mix = [&](BYTE a, BYTE b) { return (BYTE)std::lround(a + (b - a) * amount); };
    return RGB(mix(GetRValue(from), GetRValue(to)), mix(GetGValue(from), GetGValue(to)),
               mix(GetBValue(from), GetBValue(to)));
}

// Draws the whole group icon at one size. `tint` is the group's colour, or
// kNoTint.
IconCanvas DrawGroupIcon(int size, const std::vector<GroupEntry>& apps, int tint) {
    IconCanvas canvas(size);
    float s = (float)size;

    // The tile: frosted white, so it shows up on dark and colourful
    // wallpapers. With a group colour, frosted in that colour instead, a bit
    // stronger so the colour is easy to tell apart.
    COLORREF fill = RGB(255, 255, 255), border = RGB(255, 255, 255);
    float fillOpacity = 0.30f, borderOpacity = 0.55f;
    if (tint != kNoTint) {
        fill = (COLORREF)tint;
        border = MixColors((COLORREF)tint, RGB(255, 255, 255), 0.5f);
        fillOpacity = 0.60f;
        borderOpacity = 0.70f;
    }
    float inset = s * 0.03f;
    canvas.FillRoundedRect(inset, inset, s - inset, s - inset, s * 0.2f, fill, fillOpacity,
                           border, borderOpacity, std::max(1.0f, s / 48.0f));

    // The 2x2 grid of app icons inside it.
    float padding = s * 0.13f;
    float gap = s * 0.06f;
    float cell = (s - 2 * padding - gap) / 2;
    int cellPixels = std::max(1, (int)cell);

    int count = std::min((int)apps.size(), 4);
    for (int i = 0; i < count; i++) {
        int column = i % 2, row = i / 2;
        int left = (int)std::lround(padding + column * (cell + gap));
        int top = (int)std::lround(padding + row * (cell + gap));

        canvas.DrawImage(LoadItemIcon(apps[i].path, cellPixels), left, top);
    }
    return canvas;
}

// Turns a picture into PNG file data, using WIC (Windows Imaging Component).
bool EncodePng(const IconCanvas& canvas, std::vector<BYTE>* pngOut) {
    std::vector<Pixel> pixels = canvas.StraightPixels();
    UINT size = canvas.Size();

    ComPtr<IWICImagingFactory> factory;
    ComPtr<IStream> stream;
    ComPtr<IWICBitmapEncoder> encoder;
    ComPtr<IWICBitmapFrameEncode> frame;
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;

    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))) ||
        FAILED(CreateStreamOnHGlobal(nullptr, TRUE, &stream)) ||
        FAILED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder)) ||
        FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)) ||
        FAILED(encoder->CreateNewFrame(&frame, nullptr)) ||
        FAILED(frame->Initialize(nullptr)) ||
        FAILED(frame->SetSize(size, size)) ||
        FAILED(frame->SetPixelFormat(&format)) ||
        format != GUID_WICPixelFormat32bppBGRA ||
        FAILED(frame->WritePixels(size, size * 4, size * size * 4,
                                  (BYTE*)pixels.data())) ||
        FAILED(frame->Commit()) || FAILED(encoder->Commit())) {
        return false;
    }

    // Copy the finished PNG out of the in-memory stream.
    HGLOBAL memory = nullptr;
    STATSTG stats = {};
    if (FAILED(GetHGlobalFromStream(stream.Get(), &memory)) ||
        FAILED(stream->Stat(&stats, STATFLAG_NONAME))) {
        return false;
    }
    BYTE* data = (BYTE*)GlobalLock(memory);
    if (!data) {
        return false;
    }
    pngOut->assign(data, data + stats.cbSize.LowPart);
    GlobalUnlock(memory);
    return true;
}

// An .ico file is a short header listing each picture's size and where its
// data starts, followed by the pictures themselves (PNGs, in our case).
#pragma pack(push, 2)
struct IconFileHeader {
    WORD reserved;  // Always 0.
    WORD type;      // 1 = icon.
    WORD count;     // How many pictures.
};
struct IconFileEntry {
    BYTE width;     // 0 means 256.
    BYTE height;
    BYTE colorCount;
    BYTE reserved;
    WORD planes;
    WORD bitCount;
    DWORD dataSize;
    DWORD dataOffset;
};
#pragma pack(pop)

// The .ico format needs these exact sizes. If they were wrong, Windows would
// misread the file, so make the compiler check them for us.
static_assert(sizeof(IconFileHeader) == 6, "icon header must be 6 bytes");
static_assert(sizeof(IconFileEntry) == 16, "icon entry must be 16 bytes");

// The classic way to store an icon picture, used for every size except
// 256x256: a BITMAPINFOHEADER, then the pixels starting from the bottom row,
// then a 1-bit "mask" marking fully see-through pixels. (The header's height
// is doubled because it counts the pixels and the mask together.)
std::vector<BYTE> EncodeIconBitmap(const IconCanvas& canvas) {
    int size = canvas.Size();
    std::vector<Pixel> pixels = canvas.StraightPixels();
    int maskRowBytes = ((size + 31) / 32) * 4;  // Rows are padded to 4 bytes.

    BITMAPINFOHEADER header = {};
    header.biSize = sizeof(header);
    header.biWidth = size;
    header.biHeight = size * 2;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = BI_RGB;
    header.biSizeImage = size * size * 4 + maskRowBytes * size;

    std::vector<BYTE> data(sizeof(header) + header.biSizeImage, 0);
    memcpy(data.data(), &header, sizeof(header));
    BYTE* colorData = data.data() + sizeof(header);
    BYTE* maskData = colorData + size * size * 4;

    for (int y = 0; y < size; y++) {
        int sourceRow = size - 1 - y;  // Bottom row first.
        memcpy(colorData + y * size * 4, &pixels[sourceRow * size], size * 4);
        for (int x = 0; x < size; x++) {
            if (pixels[sourceRow * size + x].a == 0) {
                maskData[y * maskRowBytes + x / 8] |= (BYTE)(0x80 >> (x % 8));
            }
        }
    }
    return data;
}

bool WriteGroupIconFile(const std::wstring& iconPath,
                        const std::vector<GroupEntry>& apps, int tint) {
    const int sizes[] = {16, 20, 24, 32, 40, 48, 64, 96, 128, 256};
    const int count = ARRAYSIZE(sizes);

    // 256x256 is stored as PNG to keep the file small; the others use the
    // classic format, which every part of Windows can read.
    std::vector<std::vector<BYTE>> pictures(count);
    for (int i = 0; i < count; i++) {
        IconCanvas canvas = DrawGroupIcon(sizes[i], apps, tint);
        if (sizes[i] == 256) {
            if (!EncodePng(canvas, &pictures[i])) {
                Wh_Log(L"Couldn't save the 256x256 picture as PNG");
                return false;
            }
        } else {
            pictures[i] = EncodeIconBitmap(canvas);
        }
    }

    IconFileHeader header = {0, 1, (WORD)count};
    std::vector<IconFileEntry> entries(count);
    DWORD offset = sizeof(header) + count * sizeof(IconFileEntry);
    for (int i = 0; i < count; i++) {
        entries[i] = {};
        entries[i].width = (BYTE)(sizes[i] == 256 ? 0 : sizes[i]);
        entries[i].height = entries[i].width;
        entries[i].planes = 1;
        entries[i].bitCount = 32;
        entries[i].dataSize = (DWORD)pictures[i].size();
        entries[i].dataOffset = offset;
        offset += entries[i].dataSize;
    }

    HANDLE file = CreateFileW(iconPath.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(file, &header, sizeof(header), &written, nullptr) &&
              WriteFile(file, entries.data(), count * sizeof(IconFileEntry),
                        &written, nullptr);
    for (int i = 0; ok && i < count; i++) {
        ok = WriteFile(file, pictures[i].data(), (DWORD)pictures[i].size(),
                       &written, nullptr);
    }
    CloseHandle(file);
    if (!ok) {
        DeleteFileW(iconPath.c_str());
    }
    return ok;
}

const WCHAR kIconFilePrefix[] = L"group-icon-";

// Bump this whenever the icon's look or format changes, and every group gets
// a fresh icon the next time the mod starts.
const int kIconVersion = 2;

// Does this group already have an up-to-date icon file that exists?
bool GroupHasIcon(const std::wstring& folderPath) {
    if (_wtoi(ReadGroupValue(folderPath, L"IconVersion").c_str()) != kIconVersion) {
        return false;
    }
    std::wstring resource = ReadGroupIniValue(folderPath, L".ShellClassInfo",
                                              L"IconResource");
    std::wstring iconPath = resource.substr(0, resource.find(L','));
    if (iconPath.empty()) {
        return false;
    }
    if (PathIsRelativeW(iconPath.c_str())) {
        iconPath = folderPath + L"\\" + iconPath;
    }
    return GetFileAttributesW(iconPath.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// A short "fingerprint" of what the icon shows: the first four apps' file
// names, and the group's colour. If it hasn't changed, the icon doesn't need
// redrawing. (std::hash turns any text into a number; equal text always gives
// the same number.)
std::wstring IconFingerprint(const std::vector<GroupEntry>& apps, int tint) {
    std::wstring shown = L"tint " + std::to_wstring(tint) + L"|";
    for (size_t i = 0; i < apps.size() && i < 4; i++) {
        shown += apps[i].path.substr(apps[i].path.find_last_of(L'\\') + 1) + L"|";
    }
    return std::to_wstring(std::hash<std::wstring>{}(shown));
}

// Draws an icon showing `apps`, saves it in the group folder, sets it as the
// folder's icon, and tells Explorer to show it straight away.
//
// Explorer remembers ("caches") icons by file name, so if we overwrote the
// same file it might keep showing the old picture. Instead each new icon gets
// a new name (with a number that changes every time), and old ones are
// deleted.
bool ApplyGroupIcon(const std::wstring& folderPath,
                    const std::vector<GroupEntry>& apps) {
    ULONGLONG startTime = GetTickCount64();
    int tint = GetGroupSetting(folderPath, kSettingTint);
    std::wstring fingerprint = IconFingerprint(apps, tint);
    std::wstring iconName = kIconFilePrefix + std::to_wstring(GetTickCount64()) + L".ico";
    std::wstring iconPath = folderPath + L"\\" + iconName;

    if (!WriteGroupIconFile(iconPath, apps, tint)) {
        Wh_Log(L"Couldn't save the icon for \"%s\"", folderPath.c_str());
        return false;
    }
    SetFileAttributesW(iconPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

    // Check Windows can actually read the file we just wrote.
    HICON testIcon = nullptr;
    HRESULT loaded = SHDefExtractIconW(iconPath.c_str(), 0, 0, &testIcon, nullptr, 32);
    if (testIcon) {
        DestroyIcon(testIcon);
    } else {
        Wh_Log(L"  Windows can't read the new icon file (error 0x%08X)", (unsigned)loaded);
    }

    // Set the folder's icon with Windows' official function for it, the one
    // behind Properties > Customize > Change Icon. It updates desktop.ini and
    // tells Explorer to forget its cached copy of the folder's settings.
    // (It only changes the icon line, so any [DesktopAppGroups] data stays.)
    EnsureUnicodeGroupIni(folderPath);  // Before Windows creates it the old way.
    std::vector<WCHAR> iconPathBuffer(iconPath.begin(), iconPath.end());
    iconPathBuffer.push_back(L'\0');
    SHFOLDERCUSTOMSETTINGS settings = {};
    settings.dwSize = sizeof(settings);
    settings.dwMask = FCSM_ICONFILE;
    settings.pszIconFile = iconPathBuffer.data();
    settings.iIconIndex = 0;
    HRESULT result = SHGetSetFolderCustomSettings(&settings, folderPath.c_str(),
                                                  FCS_FORCEWRITE);
    if (FAILED(result)) {
        Wh_Log(L"  Couldn't set the folder icon (error 0x%08X)", (unsigned)result);
        return false;
    }
    WriteGroupValue(folderPath, L"IconVersion", std::to_wstring(kIconVersion).c_str());
    WriteGroupValue(folderPath, L"IconFingerprint", fingerprint.c_str());

    // Delete older icon files.
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW(
        (folderPath + L"\\" + kIconFilePrefix + L"*.ico").c_str(), &found);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            if (iconName != found.cFileName) {
                std::wstring oldPath = folderPath + L"\\" + found.cFileName;
                SetFileAttributesW(oldPath.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(oldPath.c_str());
            }
        } while (FindNextFileW(search, &found));
        FindClose(search);
    }

    // Tell Explorer this folder's look has changed. Windows normally collects
    // these notices and delivers them in a batch a moment later;
    // SHCNF_FLUSHNOWAIT asks for delivery right now.
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, folderPath.c_str(),
                   nullptr);

    Wh_Log(L"Updated the icon for \"%s\" (%d apps) in %u ms", folderPath.c_str(),
           (int)apps.size(), (unsigned)(GetTickCount64() - startTime));
    return true;
}

// Redraws a group's icon, unless it already shows these apps. Skipping
// unneeded redraws also stops our own writes (which the watcher can see as
// changes) from causing endless redraws.
void RefreshGroupIcon(const std::wstring& folderPath,
                      const std::vector<GroupEntry>& apps) {
    if (GroupHasIcon(folderPath) &&
        ReadGroupValue(folderPath, L"IconFingerprint") ==
            IconFingerprint(apps, GetGroupSetting(folderPath, kSettingTint))) {
        return;
    }
    ApplyGroupIcon(folderPath, apps);
}

std::wstring GetDesktopFolderPath() {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path))) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

// Lists the names of all folders on the desktop (groups or not).
std::vector<std::wstring> FindAllFolders(const std::wstring& desktopPath) {
    std::vector<std::wstring> folders;
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((desktopPath + L"\\*").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return folders;
    }
    do {
        std::wstring name = found.cFileName;
        if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && name != L"." &&
            name != L"..") {
            folders.push_back(name);
        }
    } while (FindNextFileW(search, &found));
    FindClose(search);
    return folders;
}

// Lists the names of all groups on the desktop.
std::vector<std::wstring> FindAllGroups(const std::wstring& desktopPath) {
    std::vector<std::wstring> groups;
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((desktopPath + L"\\*").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return groups;
    }
    do {
        std::wstring name = found.cFileName;
        if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && name != L"." &&
            name != L".." && IsGroupFolder(desktopPath + L"\\" + name)) {
            groups.push_back(name);
        }
    } while (FindNextFileW(search, &found));
    FindClose(search);
    return groups;
}

// --- Creating a group --------------------------------------------------------
//
// The icon is drawn *before* the group appears, so it never shows up as a
// plain folder first:
//
//   1. Create the folder, hidden.
//   2. Draw its icon from the apps, while they're still on the desktop.
//   3. Move the apps in.
//   4. Mark it as a group and unhide it. It appears looking like a group.
//
// The apps keep their file names when they move, so the icon's fingerprint
// still matches afterwards and the group watcher won't redraw it.

// Takes flags off a file or folder, such as hidden or read-only.
void RemoveAttributes(const std::wstring& path, DWORD attributesToRemove) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return;
    }
    attributes &= ~(attributesToRemove | FILE_ATTRIBUTE_DIRECTORY);
    SetFileAttributesW(path.c_str(), attributes ? attributes : FILE_ATTRIBUTE_NORMAL);
}

// Tidies up a half-made group after something went wrong: our own files, then
// the folder if it's empty. If some apps did get moved in, the folder stays
// (unhidden), so nothing disappears from view.
void CleanUpFailedGroup(const std::wstring& groupPath) {
    ForgetStoredGroup(GroupIdOf(groupPath));  // Its icon details, if any were saved.

    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((groupPath + L"\\*").c_str(), &found);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            std::wstring name = found.cFileName;
            if (_wcsicmp(name.c_str(), L"desktop.ini") == 0 ||
                _wcsnicmp(name.c_str(), kIconFilePrefix, wcslen(kIconFilePrefix)) == 0) {
                std::wstring path = groupPath + L"\\" + name;
                SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(path.c_str());
            }
        } while (FindNextFileW(search, &found));
        FindClose(search);
    }
    RemoveAttributes(groupPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY);
    if (!RemoveDirectoryW(groupPath.c_str())) {
        Wh_Log(L"Kept \"%s\" because some files were moved into it", groupPath.c_str());
    }
}

void RequestGroupCheck(const std::wstring& folderPath);  // In section 7.

bool CreateGroup(const std::vector<DesktopItem>& items, std::wstring* groupPathOut) {
    std::wstring desktopPath = GetDesktopFolderPath();
    if (desktopPath.empty()) {
        Wh_Log(L"Couldn't find the Desktop folder");
        return false;
    }

    // 1. Create "New group" (in the mod's language), or "New group (2)" if
    //    that name is taken, etc., and hide it while we set it up.
    std::wstring groupPath;
    for (int number = 1; number < 100; number++) {
        std::wstring candidate = desktopPath + L"\\" + Tr(Text::NewGroup);
        if (number > 1) {
            candidate += L" (" + std::to_wstring(number) + L")";
        }
        if (CreateDirectoryW(candidate.c_str(), nullptr)) {
            groupPath = candidate;
            break;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            break;
        }
    }
    if (groupPath.empty()) {
        Wh_Log(L"Couldn't create the group folder");
        return false;
    }
    SetFileAttributesW(groupPath.c_str(), FILE_ATTRIBUTE_HIDDEN);

    // Note the folder in the mod's storage until it's finished. If Explorer
    // stops before then (say it crashes while Windows is asking for
    // permission), the folder would be left hidden with some shortcuts
    // already in it. At the next start, RecoverUnfinishedGroup finds this note
    // and puts things right.
    StoreText(L"UnfinishedGroup", groupPath.c_str());
    Wh_Log(L"Created \"%s\"", groupPath.c_str());

    // 2. Draw the icon from the apps where they are now, in the same order
    //    the popup will show them. If this fails, carry on: the group watcher
    //    will draw it a moment later instead.
    std::vector<GroupEntry> apps;
    for (const DesktopItem& item : items) {
        apps.push_back({item.name, item.path});
    }
    std::sort(apps.begin(), apps.end(), [](const GroupEntry& a, const GroupEntry& b) {
        return StrCmpLogicalW(a.name.c_str(), b.name.c_str()) < 0;
    });
    ApplyGroupIcon(groupPath, apps);

    // 3. Move the apps in using IFileOperation, the same engine Explorer uses
    //    when you move files yourself. It handles undo (Ctrl+Z), OneDrive,
    //    and asks for permission if a shortcut belongs to all users.
    ComPtr<IFileOperation> operation;
    ComPtr<IShellItem> destination;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&operation))) ||
        FAILED(SHCreateItemFromParsingName(groupPath.c_str(), nullptr,
                                           IID_PPV_ARGS(&destination)))) {
        Wh_Log(L"Couldn't start the file move");
        CleanUpFailedGroup(groupPath);
        DeleteStoredValue(L"UnfinishedGroup");
        return false;
    }

    operation->SetOperationFlags(FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR);

    for (const DesktopItem& item : items) {
        ComPtr<IShellItem> source;
        if (SUCCEEDED(SHCreateItemFromParsingName(item.path.c_str(), nullptr,
                                                  IID_PPV_ARGS(&source)))) {
            operation->MoveItem(source.Get(), destination.Get(), nullptr, nullptr);
        }
    }

    HRESULT result = operation->PerformOperations();
    BOOL aborted = FALSE;
    operation->GetAnyOperationsAborted(&aborted);

    if (FAILED(result) || aborted) {
        Wh_Log(L"The move didn't finish (error 0x%08X)", (unsigned)result);
        CleanUpFailedGroup(groupPath);
        DeleteStoredValue(L"UnfinishedGroup");
        return false;
    }

    // 4. Only mark it as a group once everything is safely inside (so the
    //    watcher never sees a "group" with one app mid-move and removes it),
    //    then unhide it.
    MarkAsGroup(groupPath);
    RemoveAttributes(groupPath, FILE_ATTRIBUTE_HIDDEN);
    *groupPathOut = groupPath;
    DeleteStoredValue(L"UnfinishedGroup");  // Finished.
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, groupPath.c_str(),
                   nullptr);

    // Have the group watcher look at the new group once it's complete. (It
    // also hears about the apps arriving, but that's before the group was
    // marked, so it might have skipped it.)
    RequestGroupCheck(groupPath);
    return true;
}

// ---- 7. Watching groups -----------------------------------------------------
//
// Group icons should update whenever a group's contents change, however that
// happens: our own drop, dragging something into a group, deleting something
// in an Explorer window...
//
// So a background thread asks Windows to report every file added, removed or
// renamed anywhere inside the Desktop folder (ReadDirectoryChangesW). When
// something changes inside a group, it waits until things have been quiet
// for 100 ms (one drop can cause several changes within a few
// milliseconds), then checks that group once.
//
// Writing a new icon file counts as a change too, so icon files are ignored.
// Otherwise each new icon would trigger another new icon, forever.
//
// Like Start menu folders, a group that's down to one app is removed (unless
// that's turned off in the group settings). The app goes back on the desktop
// and the empty group is deleted.

HANDLE g_watcherThread = nullptr;
HANDLE g_watcherStopEvent = nullptr;

// Asking the watcher to check a group straight away.
//
// The watcher only hears about files being added, removed or renamed. Some
// changes, like saving a new app order, don't add or remove any file, so it
// never hears about those. Instead, the popup asks it directly with
// RequestGroupCheck.
//
// The popup and the watcher run on different threads, at the same time. If
// both used the list of requests at once, it could get corrupted. A "mutex"
// (mutual exclusion) prevents that: whoever holds the lock has the list to
// themselves, and the other waits a moment. Then an "event" wakes the watcher
// up, like a doorbell.
std::mutex g_checkRequestsLock;
std::vector<std::wstring> g_checkRequests;  // Group folder paths to check.
HANDLE g_watcherWakeEvent = nullptr;

void RequestGroupCheck(const std::wstring& folderPath) {
    if (!g_watcherWakeEvent) {
        return;  // The watcher isn't running.
    }
    {
        std::lock_guard<std::mutex> lock(g_checkRequestsLock);  // Released at "}".
        g_checkRequests.push_back(folderPath);
    }
    SetEvent(g_watcherWakeEvent);
}

bool IsIconFile(const std::wstring& fileName) {
    return _wcsnicmp(fileName.c_str(), kIconFilePrefix, wcslen(kIconFilePrefix)) == 0;
}

// Removes a group with one app (or none) left. It only ever deletes the
// group's own hidden files (desktop.ini and the icon). If the folder holds
// anything else, like a subfolder or some other hidden file, the group is
// left alone rather than risk deleting something of yours.
void DissolveGroup(const std::wstring& folderPath,
                   const std::vector<GroupEntry>& entries) {
    ULONGLONG startTime = GetTickCount64();
    std::wstring id = GroupIdOf(folderPath);  // While the folder still exists.

    // Safety check: is everything in the folder either the last app, or one
    // of our own files?
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((folderPath + L"\\*").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return;
    }
    bool onlyExpectedFiles = true;
    size_t visibleFiles = 0;
    do {
        std::wstring name = found.cFileName;
        if (name == L"." || name == L"..") {
            continue;
        }
        bool isFolder = found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        bool isHidden = found.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
        bool isOurs = _wcsicmp(name.c_str(), L"desktop.ini") == 0 || IsIconFile(name);
        if (isFolder || (isHidden && !isOurs)) {
            onlyExpectedFiles = false;
        } else if (!isHidden) {
            visibleFiles++;
        }
    } while (FindNextFileW(search, &found));
    FindClose(search);

    // Also make sure nothing was added since we looked (visibleFiles should
    // match what CheckGroup saw).
    if (!onlyExpectedFiles || visibleFiles != entries.size()) {
        Wh_Log(L"Not removing \"%s\": it has other files in it", folderPath.c_str());
        return;
    }

    // Remember where the group is, so its last app can go there.
    POINT groupPosition = {};
    bool knowPosition = AskDesktopForPosition(folderPath, &groupPosition);

    // Hide the group first and tell Explorer right away, so
    // it disappears from the desktop the moment removal starts, instead of
    // when Explorer eventually notices it's gone.
    DWORD originalAttributes = GetFileAttributesW(folderPath.c_str());
    SetFileAttributesW(folderPath.c_str(),
                       (originalAttributes & ~FILE_ATTRIBUTE_DIRECTORY) | FILE_ATTRIBUTE_HIDDEN);
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, folderPath.c_str(),
                   nullptr);

    // If anything goes wrong from here, unhide the group again.
    auto unhide = [&]() {
        RemoveAttributes(folderPath, FILE_ATTRIBUTE_HIDDEN);
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
                       folderPath.c_str(), nullptr);
    };

    // Move the last app back onto the desktop. FOF_RENAMEONCOLLISION picks a
    // new name ("App (2)") if the desktop already has one with the same name.
    if (!entries.empty()) {
        std::wstring desktopPath = folderPath.substr(0, folderPath.find_last_of(L'\\'));
        ComPtr<IFileOperation> operation;
        ComPtr<IShellItem> source;
        ComPtr<IShellItem> destination;
        if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                    IID_PPV_ARGS(&operation))) ||
            FAILED(SHCreateItemFromParsingName(entries[0].path.c_str(), nullptr,
                                               IID_PPV_ARGS(&source))) ||
            FAILED(SHCreateItemFromParsingName(desktopPath.c_str(), nullptr,
                                               IID_PPV_ARGS(&destination)))) {
            unhide();
            return;
        }
        operation->SetOperationFlags(FOF_ALLOWUNDO | FOF_RENAMEONCOLLISION | FOF_SILENT);
        operation->MoveItem(source.Get(), destination.Get(), nullptr, nullptr);
        HRESULT result = operation->PerformOperations();
        BOOL aborted = FALSE;
        operation->GetAnyOperationsAborted(&aborted);
        if (FAILED(result) || aborted) {
            Wh_Log(L"Couldn't move the last app out of \"%s\" (error 0x%08X)",
                   folderPath.c_str(), (unsigned)result);
            unhide();
            return;
        }

        // Deliver the "app moved" notice now. (Event 0 means "no new notice,
        // just send the waiting ones".)
        SHChangeNotify(0, SHCNF_FLUSHNOWAIT, nullptr, nullptr);

        // Put it where the group was. (If the desktop already had an app with
        // the same name, Windows renamed this one, and it lands in the next
        // free spot instead.)
        if (knowPosition) {
            AskDesktopToPlace(desktopPath + L"\\" + FileNameOf(entries[0].path), groupPosition);
        }
    }

    // Delete our own files, then the (now empty) folder. RemoveDirectoryW
    // refuses to remove a folder that isn't empty, as a last safety net.
    search = FindFirstFileW((folderPath + L"\\*").c_str(), &found);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            std::wstring name = found.cFileName;
            if (_wcsicmp(name.c_str(), L"desktop.ini") == 0 || IsIconFile(name)) {
                std::wstring path = folderPath + L"\\" + name;
                SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(path.c_str());
            }
        } while (FindNextFileW(search, &found));
        FindClose(search);
    }
    DWORD folderAttributes = GetFileAttributesW(folderPath.c_str());
    if (folderAttributes != INVALID_FILE_ATTRIBUTES) {
        // Take off the read-only flag we added in MarkAsGroup.
        SetFileAttributesW(folderPath.c_str(),
                           folderAttributes & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_DIRECTORY));
    }
    if (RemoveDirectoryW(folderPath.c_str())) {
        ForgetStoredGroup(id);

        // Tell Explorer the folder is gone, right now. Without this, the
        // desktop waits for its own file watching to catch up, and if
        // Explorer still had the folder open, Windows only finishes deleting
        // it once Explorer lets go.
        SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, folderPath.c_str(),
                       nullptr);
        Wh_Log(L"Removed group \"%s\" (%s) in %u ms", folderPath.c_str(),
               entries.empty() ? L"it was empty" : L"its last app went back on the desktop",
               (unsigned)(GetTickCount64() - startTime));
    } else {
        Wh_Log(L"Couldn't remove the group folder (error %u)", (unsigned)GetLastError());
        unhide();
    }
}

// Looks at a group after it changed: remove it if it's down to one app,
// otherwise make sure its icon is up to date.
void CheckGroup(const std::wstring& folderPath) {
    if (!IsGroupFolder(folderPath)) {
        return;  // Not a group (or no longer exists).
    }
    // Make sure its data is where the setting says (it may have just been
    // changed, or the group may be from an older version of the mod).
    MoveGroupData(folderPath, GroupIdOf(folderPath), true);
    std::vector<GroupEntry> entries = ListGroupFiles(folderPath);
    if (GetGroupSetting(folderPath, kSettingRemoveWhenOneLeft) && entries.size() <= 1) {
        DissolveGroup(folderPath, entries);
        return;
    }
    RefreshGroupIcon(folderPath, entries);
}

DWORD WINAPI GroupWatcherThread(LPVOID) {
    // COM must be set up on each thread that uses it (for the icon drawing).
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    std::wstring desktopPath = GetDesktopFolderPath();
    if (desktopPath.empty()) {
        Wh_Log(L"Group watcher: couldn't find the Desktop folder");
        CoUninitialize();
        return 0;
    }

    // First, check every group once: draw icons for any that need one, and
    // remove any that are down to one app.
    for (const std::wstring& group : FindAllGroups(desktopPath)) {
        CheckGroup(desktopPath + L"\\" + group);
    }

    // FILE_FLAG_OVERLAPPED lets us wait for "changes arrived" and "stop" at
    // the same time, instead of being stuck waiting for changes.
    HANDLE directory = CreateFileW(
        desktopPath.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
    if (directory == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Group watcher: couldn't watch the Desktop folder");
        CoUninitialize();
        return 0;
    }

    alignas(DWORD) BYTE buffer[16 * 1024];
    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    std::set<std::wstring> changedGroups;
    bool reading = false;

    while (true) {
        if (!reading) {
            ResetEvent(overlapped.hEvent);
            if (!ReadDirectoryChangesW(directory, buffer, sizeof(buffer), TRUE,
                                       FILE_NOTIFY_CHANGE_FILE_NAME |
                                           FILE_NOTIFY_CHANGE_DIR_NAME,
                                       nullptr, &overlapped, nullptr)) {
                Wh_Log(L"Group watcher: stopped (error %u)", (unsigned)GetLastError());
                break;
            }
            reading = true;
        }

        // Wait for: the mod stopping, changes arriving, a request from the
        // popup, or (if changes are waiting to be handled) 100 ms of quiet.
        HANDLE events[] = {g_watcherStopEvent, overlapped.hEvent, g_watcherWakeEvent};
        DWORD timeout = changedGroups.empty() ? INFINITE : 100;
        DWORD woke = WaitForMultipleObjects(3, events, FALSE, timeout);

        if (woke == WAIT_OBJECT_0) {
            break;  // The mod is stopping.
        }

        if (woke == WAIT_OBJECT_0 + 2) {
            // The popup asked us to check some groups. Take the list (holding
            // the lock only briefly), then check them straight away.
            std::vector<std::wstring> requests;
            {
                std::lock_guard<std::mutex> lock(g_checkRequestsLock);
                requests.swap(g_checkRequests);
            }
            for (const std::wstring& groupPath : requests) {
                CheckGroup(groupPath);
            }
            continue;
        }

        if (woke == WAIT_TIMEOUT) {
            for (const std::wstring& group : changedGroups) {
                CheckGroup(desktopPath + L"\\" + group);
            }
            changedGroups.clear();
            continue;
        }

        // Changes arrived.
        reading = false;
        DWORD bytes = 0;
        if (!GetOverlappedResult(directory, &overlapped, &bytes, FALSE)) {
            break;
        }
        if (bytes == 0) {
            // Too many changes at once to list. Just check every group.
            for (const std::wstring& group : FindAllGroups(desktopPath)) {
                changedGroups.insert(group);
            }
            continue;
        }

        // The buffer holds a chain of FILE_NOTIFY_INFORMATION records, each
        // with a path relative to the Desktop, like "New group\Notepad.lnk".
        BYTE* position = buffer;
        while (true) {
            auto* change = (FILE_NOTIFY_INFORMATION*)position;
            std::wstring path(change->FileName, change->FileNameLength / sizeof(WCHAR));

            size_t firstSlash = path.find(L'\\');
            if (firstSlash == std::wstring::npos) {
                // Something directly on the desktop. If a group was renamed,
                // refresh it so it's tracked under its new name.
                if (change->Action == FILE_ACTION_RENAMED_NEW_NAME) {
                    changedGroups.insert(path);
                }
            } else {
                // Something inside a folder. Only direct contents matter, and
                // not icon files.
                std::wstring fileName = path.substr(firstSlash + 1);
                if (fileName.find(L'\\') == std::wstring::npos && !IsIconFile(fileName)) {
                    changedGroups.insert(path.substr(0, firstSlash));
                }
            }

            if (change->NextEntryOffset == 0) {
                break;
            }
            position += change->NextEntryOffset;
        }
    }

    if (reading) {
        CancelIoEx(directory, &overlapped);
        DWORD bytes = 0;
        GetOverlappedResult(directory, &overlapped, &bytes, TRUE);
    }
    CloseHandle(overlapped.hEvent);
    CloseHandle(directory);
    CoUninitialize();
    return 0;
}

void StartGroupWatcher() {
    if (g_watcherThread) {
        return;  // Already running.
    }
    if (!g_watcherStopEvent) {
        g_watcherStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }
    ResetEvent(g_watcherStopEvent);
    if (!g_watcherWakeEvent) {
        // "Auto-reset": it switches itself off again once the watcher wakes.
        g_watcherWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    }
    g_watcherThread = CreateThread(nullptr, 0, GroupWatcherThread, nullptr, 0, nullptr);
}

void StopGroupWatcher() {
    if (g_watcherThread) {
        SetEvent(g_watcherStopEvent);
        WaitForSingleObject(g_watcherThread, INFINITE);
        CloseHandle(g_watcherThread);
        g_watcherThread = nullptr;
    }
    if (g_watcherStopEvent) {
        CloseHandle(g_watcherStopEvent);
        g_watcherStopEvent = nullptr;
    }
    if (g_watcherWakeEvent) {
        CloseHandle(g_watcherWakeEvent);
        g_watcherWakeEvent = nullptr;
    }
}

// ---- 8. Watching drags ------------------------------------------------------
//
// Every drag-and-drop in Windows goes through one function, DoDragDrop. It
// takes an "IDropSource" object, which DoDragDrop keeps asking "should the
// drag continue?" every time the mouse moves or a key/button changes.
//
// We "hook" DoDragDrop so Windhawk sends every call to our function first. If
// the drag started on the desktop, we wrap Explorer's IDropSource in our own
// object (DropSourceSpy) that passes every question through to Explorer.
//
// When you let go over another icon and the group trigger is met, the spy
// answers "cancel" instead of "drop". Windows then does nothing, and we create
// the group ourselves after the drag ends.

class DropSourceSpy final : public IDropSource {
public:
    DropSourceSpy(IDropSource* explorerDropSource, HWND list,
                  IFolderView* folderView, std::vector<int> draggedIcons,
                  bool canGroup)
        : m_inner(explorerDropSource),
          m_list(list),
          m_folderView(folderView),
          m_draggedIcons(std::move(draggedIcons)),
          m_canGroup(canGroup) {
        m_inner->AddRef();
    }

    // Results, read after the drag finishes.
    bool dropped = false;
    int dropTargetIcon = -1;
    ULONGLONG hoveredMsAtDrop = 0;
    bool groupRequested = false;
    DesktopItem groupTarget;

    // --- IUnknown: reference counting, required by every COM object ---------

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        if (riid == IID_IUnknown || riid == IID_IDropSource) {
            *object = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        // Anything else we don't know about, Explorer's object can answer.
        return m_inner->QueryInterface(riid, object);
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return ++m_refCount; }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG count = --m_refCount;
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // --- IDropSource: the two questions DoDragDrop asks ---------------------

    // "Should the drag continue, drop, or cancel?"
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escapePressed,
                                                DWORD keyState) override {
        HRESULT answer = m_inner->QueryContinueDrag(escapePressed, keyState);

        // Keep track of which icon we're over, and since when.
        int hovered = GetOtherIconUnderCursor();
        if (hovered != m_lastHovered) {
            m_lastHovered = hovered;
            m_hoverStartTime = GetTickCount64();
        }

        if (answer == DRAGDROP_S_DROP) {
            dropped = true;
            dropTargetIcon = hovered;
            hoveredMsAtDrop = GetTickCount64() - m_hoverStartTime;

            if (hovered >= 0 && m_canGroup && TriggerMet(keyState)) {
                DesktopItem target;
                if (GetDesktopItem(m_folderView, m_list, hovered, &target) &&
                    CanBeGrouped(target)) {
                    groupRequested = true;
                    groupTarget = target;
                    return DRAGDROP_S_CANCEL;  // Stop Windows' normal drop.
                }
            }
        }
        return answer;
    }

    // "What should the mouse pointer look like?" We don't change it.
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect) override {
        return m_inner->GiveFeedback(effect);
    }

private:
    // "virtual" makes `delete this` in Release() safe. Without it, the
    // compiler warns that the wrong destructor could run.
    virtual ~DropSourceSpy() { m_inner->Release(); }

    bool HoveredLongEnough() {
        return GetTickCount64() - m_hoverStartTime >=
               (ULONGLONG)g_settings.hoverDelayMs;
    }

    // Has the user done what the "How to create a group" setting asks?
    bool TriggerMet(DWORD keyState) {
        switch (g_settings.trigger) {
            case GroupTrigger::Always:
                return true;
            case GroupTrigger::Shift:
                return (keyState & MK_SHIFT) != 0;
            case GroupTrigger::Hover:
                return HoveredLongEnough();
        }
        return false;
    }

    // Like GetIconUnderCursor, but ignores the icons being dragged, since the
    // pointer starts out on top of them.
    int GetOtherIconUnderCursor() {
        int icon = GetIconUnderCursor(m_list);
        for (int dragged : m_draggedIcons) {
            if (icon == dragged) {
                return -1;
            }
        }
        return icon;
    }

    IDropSource* m_inner;
    HWND m_list;
    IFolderView* m_folderView;
    std::vector<int> m_draggedIcons;
    bool m_canGroup;
    int m_lastHovered = -1;
    ULONGLONG m_hoverStartTime = 0;
    ULONG m_refCount = 1;
};

// The real DoDragDrop. Windhawk fills this in when it sets up the hook, and
// we call it to let the drag actually happen.
using DoDragDrop_t = HRESULT(WINAPI*)(IDataObject*, IDropSource*, DWORD, DWORD*);
DoDragDrop_t DoDragDrop_Original;

// True while an app is being dragged out of the group popup (section 9). The
// popup runs on the desktop's thread too, so without this the hook below
// would mistake that drag for a desktop icon drag.
bool g_draggingFromPopup = false;

HRESULT WINAPI DoDragDrop_Hook(IDataObject* dataObject, IDropSource* dropSource,
                               DWORD allowedEffects, DWORD* effect) {
    BusyScope busy;  // See "Staying safe while unloading" near the top.
    // Only interested in drags that start on the desktop. The desktop's list
    // runs on its own thread, so if we're on that thread, it's a desktop drag.
    HWND list = g_desktopList;
    bool isDesktopDrag = list && IsWindow(list) && !g_draggingFromPopup &&
                         GetWindowThreadProcessId(list, nullptr) == GetCurrentThreadId();
    if (!isDesktopDrag) {
        return DoDragDrop_Original(dataObject, dropSource, allowedEffects, effect);
    }

    // Work out which files are being dragged, and whether they can be grouped.
    std::vector<int> draggedIcons = GetSelectedIcons(list);
    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    if (!folderView) {
        Wh_Log(L"Couldn't reach the desktop's folder view, so grouping is off "
               L"for this drag");
    }

    std::vector<DesktopItem> draggedItems;
    bool canGroup = folderView && !draggedIcons.empty();
    for (int icon : draggedIcons) {
        DesktopItem item;
        if (folderView && GetDesktopItem(folderView.Get(), list, icon, &item)) {
            if (!CanBeGrouped(item)) {
                canGroup = false;  // A folder, or a special icon like the Recycle Bin.
            }
            draggedItems.push_back(item);
        } else {
            canGroup = false;
        }
    }

    DropSourceSpy* spy = new DropSourceSpy(dropSource, list, folderView.Get(),
                                           draggedIcons, canGroup);
    HRESULT result = DoDragDrop_Original(dataObject, spy, allowedEffects, effect);

    if (spy->groupRequested) {
        // The target goes first, then whatever was dragged onto it.
        std::vector<DesktopItem> members = {spy->groupTarget};
        members.insert(members.end(), draggedItems.begin(), draggedItems.end());

        // Remember where the target icon is, so the new group can take its
        // place.
        POINT targetPosition = {};
        bool knowPosition = GetViewItemPosition(spy->groupTarget.path, &targetPosition);

        std::wstring groupPath;
        if (CreateGroup(members, &groupPath) && knowPosition) {
            if (HWND desktopWindow = g_desktopDefView) {
                QueuePlacement(desktopWindow, groupPath, targetPosition);
            }
        }
    }

    spy->Release();
    return result;
}

// ---- 9. The group popup -----------------------------------------------------
//
// The popup is our own window. Creating a window in Windows takes three steps:
//
//   1. Register a "window class": a name plus the function that handles the
//      window's messages (PopupWindowProc below).
//   2. Create the window with CreateWindowExW.
//   3. Handle its messages: WM_PAINT to draw it, WM_MOUSEMOVE for hover
//      effects, WM_LBUTTONUP for clicks, and so on.
//
// We draw everything ourselves with GDI, Windows' classic drawing library.
// To avoid flicker we draw into an off-screen picture first, then copy it to
// the screen in one go ("double buffering").
//
// What you can do in it:
//   - Click the title (or press F2) to rename the group. The text box is a
//     standard Windows "EDIT" control, placed over the title.
//   - Press on an app and move the mouse to drag it out of the group, onto
//     the desktop or anywhere else you can drop files.
//   - Scroll, if there are more apps than fit (see "Scrolling" below).
//
// The same popup also shows other desktop folders, with the "Open other
// desktop folders in the popup too" setting. g_popup.isGroup tells them apart.

// The popup's window class name. It's made unique each time the mod loads
// (see RegisterWindowClasses in section 12).
std::wstring g_popupClassName;
std::wstring g_panelClassName;  // The settings panel's (further down).
bool g_windowClassesRegistered = false;
HMODULE g_module = nullptr;  // This mod's DLL, needed to register the class.
std::atomic<HWND> g_popupWindow{nullptr};

// Which group's popup closed most recently, and when. Used so that clicking a
// group while its popup is open closes it, instead of closing and reopening.
std::wstring g_lastClosedGroup;
ULONGLONG g_lastClosedTime = 0;

// With "Open groups with: hovering": the desktop icon the mouse is resting
// on (see "Opening by hovering" in section 10), or -1.
int g_hoverIcon = -1;

// A message of our own that the rename box sends the popup: "finish
// renaming". wParam is 1 to keep the new name, 0 to cancel. Numbers from
// WM_APP upwards are free for programs to use for their own messages.
constexpr UINT kMsgFinishRename = WM_APP + 1;

// Another message of our own: "close right now, no animation". Used when the
// mod is being unloaded, since its code is about to be removed from Explorer.
constexpr UINT kMsgCloseNow = WM_APP + 2;

// The settings panel (further down) is opened from the popup.
void ToggleSettingsPanel(HWND popup);
void CloseSettingsPanel();
void PositionSettingsPanel();
void RefreshSettingsPanel();
void RetintSettingsPanel();
bool IsSettingsPanelOpen();
HWND SettingsPanelWindow();

struct PopupItem {
    std::wstring name;
    std::wstring path;
    std::wstring detail;      // For the list view: its size, or "Folder".
    IconImage icon;
    bool iconLoaded = false;  // Icons are fetched when first shown (see
                              // DrawPopupApp), so big folders open quickly.
    float x = 0, y = 0;  // Where it's drawn right now (top-left corner, before
                         // scrolling). This differs from its slot while it
                         // slides into place.
    TextMask labelMask;  // The name, ready to draw (see RenderTextMask)...
    bool labelIsList = false;  // ...and whether it was laid out for the list.
    TextMask detailMask;
};

// Everything the popup needs. Only touched on the desktop's thread.
struct {
    std::wstring title;
    std::wstring folderPath;      // Which group (or folder) is showing.
    bool isGroup = true;          // False for other folders.
    ULONGLONG openedTime = 0;     // When it opened (GetTickCount64).
    bool hoverOpened = false;     // Opened by hovering over the group...
    bool closeOnLeave = false;    // ...so it closes when the mouse leaves it,
                                  // until you click inside it.
    ULONGLONG leftTime = 0;       // When the mouse left (0 = it hasn't).
    std::vector<PopupItem> items;
    int hovered = -1;
    int pressed = -1;             // The app the mouse button went down on, or -1.
    POINT pressPoint = {};        // Where it went down, to tell clicks from drags.
    bool titleHovered = false;
    bool titlePressed = false;
    HWND renameBox = nullptr;     // The text box, while renaming.
    HBRUSH renameBoxBrush = nullptr;
    bool draggingOut = false;     // An app is being dragged out.
    bool reordering = false;      // An app is lifted, being rearranged.
    int dragged = -1;             // Which app is lifted.
    int reorderSlot = -1;         // Where it would land if let go now.
    POINT grabOffset = {};        // Where on the app the mouse grabbed it.
    POINT cursor = {};            // The mouse position while rearranging.
    bool menuOpen = false;        // A right-click menu (or its command) is running.
    IContextMenu2* contextMenu2 = nullptr;  // The open right-click menu's
    IContextMenu3* contextMenu3 = nullptr;  // helpers for submenus (see below).
    UINT dpi = 96;
    bool dark = false;
    bool highContrast = false;    // A high contrast theme is on.
    bool glass = false;           // Using the blurred background.
    SIZE layoutSize = {};         // The popup's full size. While it's
                                  // animating, the window is smaller.
    int animation = 0;            // 0 = none, 1 = opening, 2 = closing.
    ULONGLONG animationStart = 0;
    RECT iconRect = {};           // The group's icon on screen...
    RECT fullRect = {};           // ...and the popup's full position.
    float contentOpacity = 1;     // Fades the contents in and out.
    RECT itemRect = {};           // The group's icon and label on screen.
    RECT workArea = {};           // The screen, minus the taskbar.
    int columns = 4;              // Layout settings for this group
    int iconSize = 48;            // (see "Group settings" in section 5).
    bool showLabels = true;
    int tint = -1;                // The group's colour (kNoTint for none).
    bool listView = false;
    int spacing = 1;              // 0 = compact, 1 = normal, 2 = roomy.
    bool gearHovered = false;
    bool gearPressed = false;
    bool openHovered = false;     // The "open in File Explorer" button.
    bool openPressed = false;
    int viewportHeight = 0;       // The height of the area apps scroll in.
    float scroll = 0;             // How far it's scrolled down, in pixels...
    float scrollTarget = 0;       // ...and where it's heading (it glides).
    bool scrollHovered = false;   // The mouse is over the scroll bar.
    bool scrollDragging = false;  // The scroll bar is being dragged...
    int scrollGrab = 0;           // ...held at this far down its thumb.
    TextMask titleMask;           // The title, ready to draw...
    std::wstring titleMaskText;   // ...and which text that was.
    bool trackingMouse = false;
    HFONT titleFont = nullptr;
    HFONT labelFont = nullptr;
    HFONT symbolFont = nullptr;   // Windows' icon font, for the pencil symbol.
} g_popup;

// Sizes, in pixels at 100% display scaling. Px() scales them for the
// monitor's actual scaling (150%, 200%, ...).
namespace Layout {
constexpr int padding = 16;
constexpr int titleHeight = 36;
constexpr int labelGap = 6;
constexpr int minWidth = 280;
constexpr int screenMargin = 8;
}  // namespace Layout

int Px(int value) {
    return MulDiv(value, g_popup.dpi, 96);
}

// Each app's space depends on the view, the icon size and whether names are
// shown (all adjustable in the settings panel). In sizes at 100% scaling.
// In the list view, icons are smaller: 20, 24 or 32 pixels.
int ItemIconSize() {
    if (g_popup.listView) {
        return g_popup.iconSize <= 32 ? 20 : (g_popup.iconSize >= 64 ? 32 : 24);
    }
    return g_popup.iconSize;
}

// The "Spacing" setting changes the room around each icon: the space above
// it (IconTop), and how much wider and taller than the icon its cell is. The
// space for the name (two lines) stays the same.
int IconTop() {
    static const int kTop[] = {6, 10, 16};
    return kTop[g_popup.spacing];
}

int CellWidth() {
    static const int kExtraWidth[] = {24, 44, 68};
    return g_popup.listView ? 300 : g_popup.iconSize + kExtraWidth[g_popup.spacing];
}

int CellHeight() {
    if (g_popup.listView) {
        static const int kExtraHeight[] = {8, 16, 24};
        return ItemIconSize() + kExtraHeight[g_popup.spacing];
    }
    // Icon, plus the space above it and the same below. With names: above
    // it, a gap, two lines of name, and a little below.
    static const int kBottomWithNames[] = {2, 4, 10};
    if (!g_popup.showLabels) {
        return g_popup.iconSize + 2 * IconTop();
    }
    return g_popup.iconSize + IconTop() + Layout::labelGap + 32 + kBottomWithNames[g_popup.spacing];
}

int PopupColumns() {
    if (g_popup.listView) {
        return 1;
    }
    return std::clamp((int)g_popup.items.size(), 1, g_popup.columns);
}

int PopupRows() {
    int count = (int)g_popup.items.size();
    return count == 0 ? 1 : (count + PopupColumns() - 1) / PopupColumns();
}

// --- Scrolling ---------------------------------------------------------------
//
// The popup shows at most 5 rows of icons (or 12 rows in the list view; both
// are settings), and never more than fits on screen. If there are more, they
// scroll, with the mouse wheel or the thin scroll bar on the right.
//
// The apps keep their positions as if nothing was scrolled ("content"
// positions), and are drawn `g_popup.scroll` pixels higher. Anything about
// the mouse (which is in window positions) adds the scroll back on.

// The top of the area the apps scroll in, below the title.
int ContentTop() {
    return Px(Layout::padding) + Px(Layout::titleHeight);
}

// The height of all the rows together.
int ContentHeight() {
    return PopupRows() * Px(CellHeight());
}

int MaxScroll() {
    return std::max(0, ContentHeight() - g_popup.viewportHeight);
}

RECT ViewportRect(int clientWidth) {
    return {0, ContentTop(), clientWidth, ContentTop() + g_popup.viewportHeight};
}

// Works out the popup's size, including how tall the scrolling area is.
SIZE PopupSize() {
    int gridWidth = PopupColumns() * Px(CellWidth());
    int visibleRows = std::min(PopupRows(), g_popup.listView ? g_settings.maxListRows
                                                             : g_settings.maxIconRows);
    int viewport = visibleRows * Px(CellHeight());

    // Never taller than the screen (minus the taskbar).
    int chrome = ContentTop() + Px(Layout::padding);
    int screenSpace = (g_popup.workArea.bottom - g_popup.workArea.top) -
                      2 * Px(Layout::screenMargin) - chrome;
    if (screenSpace > 0 && viewport > screenSpace) {
        viewport = std::max(Px(CellHeight()), screenSpace);
    }
    g_popup.viewportHeight = viewport;

    SIZE size;
    size.cx = std::max(Px(Layout::minWidth), gridWidth + 2 * Px(Layout::padding));
    size.cy = chrome + viewport;
    return size;
}

// Keeps the scroll position within range (after the contents change).
void ClampScroll() {
    float maxScroll = (float)MaxScroll();
    g_popup.scrollTarget = std::clamp(g_popup.scrollTarget, 0.0f, maxScroll);
    g_popup.scroll = std::clamp(g_popup.scroll, 0.0f, maxScroll);
}

// A mouse position (in the window) as a content position.
POINT ToContent(POINT point) {
    return {point.x, point.y + (LONG)std::lround(g_popup.scroll)};
}

// The scroll bar: a thin rounded bar at the right edge. The area that reacts
// to the mouse is wider than the bar itself, so it's easy to grab.
RECT ScrollTrackRect(int clientWidth) {
    RECT viewport = ViewportRect(clientWidth);
    return {clientWidth - Px(14), viewport.top + Px(4), clientWidth - Px(2),
            viewport.bottom - Px(4)};
}

// The part of the bar that shows where you are. False if nothing scrolls.
bool ScrollThumbRect(int clientWidth, RECT* thumb) {
    int maxScroll = MaxScroll();
    if (maxScroll <= 0) {
        return false;
    }
    RECT track = ScrollTrackRect(clientWidth);
    int trackHeight = track.bottom - track.top;
    int thumbHeight = std::max(Px(24), trackHeight * g_popup.viewportHeight / ContentHeight());
    int top = track.top + (int)std::lround((trackHeight - thumbHeight) * g_popup.scroll / maxScroll);
    *thumb = {track.left, top, track.right, top + thumbHeight};
    return true;
}

// The title area, which you click to rename the group. Two buttons sit to
// its right, on the same row: "open in File Explorer", and the gear.
RECT PopupTitleRect(int clientWidth) {
    RECT title;
    title.left = Px(Layout::padding);
    title.top = Px(Layout::padding) - Px(4);
    title.right = clientWidth - Px(Layout::padding) - Px(76);
    title.bottom = Px(Layout::padding) + Px(Layout::titleHeight) - Px(8);
    return title;
}

RECT PopupGearRect(int clientWidth) {
    RECT title = PopupTitleRect(clientWidth);
    RECT gear;
    gear.right = clientWidth - Px(Layout::padding);
    gear.left = gear.right - Px(32);
    gear.top = title.top;
    gear.bottom = title.bottom;
    return gear;
}

RECT PopupOpenRect(int clientWidth) {
    RECT gear = PopupGearRect(clientWidth);
    return {gear.left - Px(36), gear.top, gear.left - Px(4), gear.bottom};
}

// Where app number `index` goes inside the popup (a content position).
RECT PopupCellRect(int index, int clientWidth) {
    int columns = PopupColumns();
    int gridLeft = (clientWidth - columns * Px(CellWidth())) / 2;
    int column = index % columns;
    int row = index / columns;

    RECT cell;
    cell.left = gridLeft + column * Px(CellWidth());
    cell.top = ContentTop() + row * Px(CellHeight());
    cell.right = cell.left + Px(CellWidth());
    cell.bottom = cell.top + Px(CellHeight());
    return cell;
}

// Which app is at this mouse position (or -1). Only inside the scrolling
// area: an app scrolled up under the title can't be clicked there.
int PopupHitTest(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT viewport = ViewportRect(client.right);
    if (!PtInRect(&viewport, point)) {
        return -1;
    }
    POINT content = ToContent(point);
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        RECT cell = PopupCellRect(i, client.right);
        if (PtInRect(&cell, content)) {
            return i;
        }
    }
    return -1;
}

bool PopupTitleHitTest(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT title = PopupTitleRect(client.right);
    return PtInRect(&title, point) != FALSE;
}

bool PopupGearHitTest(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT gear = PopupGearRect(client.right);
    return PtInRect(&gear, point) != FALSE;
}

bool PopupOpenHitTest(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT button = PopupOpenRect(client.right);
    return PtInRect(&button, point) != FALSE;
}

bool PopupScrollBarHitTest(HWND window, POINT point) {
    if (MaxScroll() <= 0) {
        return false;
    }
    RECT client;
    GetClientRect(window, &client);
    RECT track = ScrollTrackRect(client.right);
    return PtInRect(&track, point) != FALSE;
}

// Match the Start menu, which follows the Windows (not app) light/dark mode.
bool IsSystemDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

// The popup's colours. Hover and pressed highlights are a see-through shade
// (white in dark mode, black in light mode) rather than a solid colour, so
// they look right on both the blurred and the solid background.
struct PopupColors {
    COLORREF background;    // Only used for the solid background.
    COLORREF field;         // Behind the rename box.
    COLORREF text;
    COLORREF dimText;
    COLORREF accent;
    COLORREF shade;
    float hoverOpacity;
    float pressedOpacity;
    COLORREF onAccent;      // Text on the accent colour.
};

// Is a high contrast theme on? Then the popup uses the theme's own colours,
// with no blur or group colours, and a clear border.
bool IsHighContrast() {
    HIGHCONTRASTW contrast = {};
    contrast.cbSize = sizeof(contrast);
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
           (contrast.dwFlags & HCF_HIGHCONTRASTON);
}

// The popup's background colour, mixed with the group's colour if it has
// one. Not at full strength, so text stays easy to read: a bit less in light
// mode, where strong colours would wash out the dark text.
COLORREF TintedBackground(COLORREF themeColor) {
    if (g_popup.tint == kNoTint || g_popup.highContrast) {
        return themeColor;
    }
    return MixColors(themeColor, (COLORREF)g_popup.tint, g_popup.dark ? 0.45f : 0.35f);
}

PopupColors GetPopupColors() {
    COLORREF accent = GetSysColor(COLOR_HIGHLIGHT);
    if (g_popup.highContrast) {
        COLORREF window = GetSysColor(COLOR_WINDOW);
        return {window, window, GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_GRAYTEXT),
                accent, accent, 0.45f, 0.30f, GetSysColor(COLOR_HIGHLIGHTTEXT)};
    }
    if (g_popup.dark) {
        return {TintedBackground(RGB(44, 44, 44)), RGB(50, 50, 50), RGB(255, 255, 255),
                RGB(170, 170, 170), accent, RGB(255, 255, 255), 0.10f, 0.05f, RGB(255, 255, 255)};
    }
    return {TintedBackground(RGB(249, 249, 249)), RGB(255, 255, 255), RGB(0, 0, 0),
            RGB(96, 96, 96), accent, RGB(0, 0, 0), 0.07f, 0.035f, RGB(255, 255, 255)};
}

void CreatePopupFonts() {
    // Start from the font Windows uses for message boxes (Segoe UI), at the
    // right size for this monitor's scaling.
    NONCLIENTMETRICSW metrics = {};
    metrics.cbSize = sizeof(metrics);
    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics,
                               0, g_popup.dpi);

    // Grayscale smoothing instead of ClearType's coloured edges: see
    // RenderTextMask below for why.
    LOGFONTW label = metrics.lfMessageFont;
    label.lfQuality = ANTIALIASED_QUALITY;
    g_popup.labelFont = CreateFontIndirectW(&label);

    LOGFONTW title = metrics.lfMessageFont;
    title.lfHeight = MulDiv(title.lfHeight, 4, 3);  // A third bigger.
    title.lfWeight = FW_SEMIBOLD;
    title.lfQuality = ANTIALIASED_QUALITY;
    g_popup.titleFont = CreateFontIndirectW(&title);

    // "Segoe Fluent Icons" is the Windows 11 font of symbols (pencil, gear...).
    // Windows 10 doesn't have it, but has the older "Segoe MDL2 Assets", with
    // the same symbols at the same codes. If Windows gave us a different font
    // than we asked for, it doesn't have the first one, so use the second.
    for (PCWSTR fontName : {L"Segoe Fluent Icons", L"Segoe MDL2 Assets"}) {
        if (g_popup.symbolFont) {
            DeleteObject(g_popup.symbolFont);
        }
        g_popup.symbolFont = CreateFontW(-Px(14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                         CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                         DEFAULT_PITCH, fontName);
        WCHAR actualName[LF_FACESIZE] = L"";
        HDC dc = GetDC(nullptr);
        HGDIOBJ previousFont = SelectObject(dc, g_popup.symbolFont);
        GetTextFaceW(dc, LF_FACESIZE, actualName);
        SelectObject(dc, previousFont);
        ReleaseDC(nullptr, dc);
        if (_wcsicmp(actualName, fontName) == 0) {
            break;
        }
    }
}

// What the list view shows beside an item: "Folder", or a file's size (like
// "1.2 MB"). Nothing for shortcuts, whose size means nothing to you.
std::wstring ItemDetail(const GroupEntry& entry) {
    if (entry.isFolder) {
        return Tr(Text::FolderDetail);
    }
    PCWSTR extension = PathFindExtensionW(entry.path.c_str());
    if (_wcsicmp(extension, L".lnk") == 0 || _wcsicmp(extension, L".url") == 0) {
        return L"";
    }
    WCHAR size[32] = L"";
    StrFormatByteSizeW((LONGLONG)entry.size, size, ARRAYSIZE(size));
    return size;
}

// Reads the group's apps (see ListGroupFiles in section 5), in the same order
// as the group's icon. Their icons are fetched later, when first shown.
void LoadGroupItems(const std::wstring& folderPath) {
    for (const GroupEntry& entry : ListGroupFiles(folderPath)) {
        PopupItem item;
        item.name = entry.name;
        item.path = entry.path;
        item.detail = ItemDetail(entry);
        g_popup.items.push_back(item);
    }
}

// Forgets the apps' icons, so they're fetched again at a new size.
void ResetItemIcons() {
    for (PopupItem& item : g_popup.items) {
        item.icon = {};
        item.iconLoaded = false;
    }
}

void FreePopupResources() {
    g_popup.items.clear();
    for (HFONT* font : {&g_popup.titleFont, &g_popup.labelFont, &g_popup.symbolFont}) {
        if (*font) {
            DeleteObject(*font);
            *font = nullptr;
        }
    }
    if (g_popup.renameBoxBrush) {
        DeleteObject(g_popup.renameBoxBrush);
        g_popup.renameBoxBrush = nullptr;
    }
    g_popup.titleMask = {};
    g_popup.titleMaskText.clear();
    g_popup.renameBox = nullptr;  // Windows destroys child windows for us.
    g_popup.hovered = -1;
    g_popup.pressed = -1;
    g_popup.titleHovered = false;
    g_popup.titlePressed = false;
    g_popup.openHovered = false;
    g_popup.openPressed = false;
    g_popup.scrollHovered = false;
    g_popup.scrollDragging = false;
    g_popup.draggingOut = false;
    g_popup.reordering = false;
    g_popup.dragged = -1;
    g_popup.menuOpen = false;
    g_popup.contextMenu2 = nullptr;
    g_popup.contextMenu3 = nullptr;
    g_popup.animation = 0;
    g_popup.contentOpacity = 1;
    g_popup.trackingMouse = false;
}

// --- Drawing text on a see-through background --------------------------------
//
// The popup's background is see-through, so its text has to be drawn as a
// see-through layer too: each pixel of a letter says how much of the pixel
// the letter covers (its "coverage"), and IconCanvas::BlendMask paints the
// text's colour in by that much.
//
// The coverage comes from DirectWrite (with Direct2D), the text engine the
// Start menu and the rest of Windows 11 use. Drawing onto a fully
// transparent picture, it works out each pixel's coverage directly, with the
// same smoothing Windows uses for text on see-through backgrounds, so the
// letters come out as crisp and even as the Start menu's. (If DirectWrite
// isn't available, for some reason, GDI's DrawText is used instead.)
//
// Working out coverage takes a little time, so each label's is worked out
// once and remembered (PopupItem::labelMask), which keeps the sliding
// animations smooth.

// The Direct2D and DirectWrite "factories", which make everything else.
// Created on first use, and released when the mod unloads
// (ReleaseTextFactories, called from Wh_ModUninit).
//
// [[clang::no_destroy]]: when Explorer itself exits (a restart, signing out
// or shutting down), Windhawk doesn't call Wh_ModUninit, and C++ would
// release these automatically at the very end. By then Windows has already
// stopped Explorer's other threads, and releasing them could get stuck on a
// lock one of those threads held, hanging or crashing Explorer as it exits.
// This tells C++ not to: Windows reclaims everything when Explorer exits
// anyway.
[[clang::no_destroy]] ComPtr<ID2D1Factory> g_d2dFactory;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_writeFactory;
std::mutex g_textLock;  // In case two threads draw text at once.

bool EnsureTextFactories() {
    if (g_d2dFactory && g_writeFactory) {
        return true;
    }
    if (!g_d2dFactory &&
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, g_d2dFactory.GetAddressOf()))) {
        return false;
    }
    if (!g_writeFactory &&
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   (IUnknown**)g_writeFactory.GetAddressOf()))) {
        return false;
    }
    return true;
}

void ReleaseTextFactories() {
    std::lock_guard<std::mutex> lock(g_textLock);
    g_writeFactory.Reset();
    g_d2dFactory.Reset();
}

// Lays out `text` in a `width` x `height` box, matching the DrawText flags
// the mod uses: DT_LEFT/DT_CENTER/DT_RIGHT, DT_VCENTER, DT_SINGLELINE or
// DT_WORDBREAK, and DT_END_ELLIPSIS (text that doesn't fit ends with "...").
// The font is the same one as the GDI font `font` (its name, size and
// weight). Hold g_textLock.
ComPtr<IDWriteTextLayout> CreateTextLayout(HFONT font, const std::wstring& text, float width,
                                           float height, UINT format) {
    LOGFONTW logFont = {};
    if (!GetObjectW(font, sizeof(logFont), &logFont)) {
        return nullptr;
    }
    // A negative height is the letters' size in pixels, which is what
    // DirectWrite wants (we draw at 96 DPI, where its units are pixels).
    float size = logFont.lfHeight < 0 ? (float)-logFont.lfHeight
                                      : std::max(1.0f, logFont.lfHeight * 0.8f);
    ComPtr<IDWriteTextFormat> textFormat;
    if (FAILED(g_writeFactory->CreateTextFormat(
            logFont.lfFaceName, nullptr,
            (DWRITE_FONT_WEIGHT)(logFont.lfWeight ? logFont.lfWeight : FW_NORMAL),
            logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, size, L"", &textFormat))) {
        return nullptr;
    }
    textFormat->SetTextAlignment((format & DT_CENTER)  ? DWRITE_TEXT_ALIGNMENT_CENTER
                                 : (format & DT_RIGHT) ? DWRITE_TEXT_ALIGNMENT_TRAILING
                                                       : DWRITE_TEXT_ALIGNMENT_LEADING);
    textFormat->SetParagraphAlignment((format & DT_VCENTER) ? DWRITE_PARAGRAPH_ALIGNMENT_CENTER
                                                            : DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    // Long words that don't fit on a line are broken, like DT_EDITCONTROL.
    textFormat->SetWordWrapping((format & DT_SINGLELINE) ? DWRITE_WORD_WRAPPING_NO_WRAP
                                                         : DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);

    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(g_writeFactory->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                                textFormat.Get(), width, height, &layout))) {
        return nullptr;
    }
    if (format & DT_END_ELLIPSIS) {
        // "..." at the end of the last line that fits.
        ComPtr<IDWriteInlineObject> ellipsis;
        if (SUCCEEDED(g_writeFactory->CreateEllipsisTrimmingSign(textFormat.Get(), &ellipsis))) {
            DWRITE_TRIMMING trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
            layout->SetTrimming(&trimming, ellipsis.Get());
        }
    }
    return layout;
}

// The older way, with GDI: draw the text white on black, and read how
// bright each pixel came out as its coverage. Only used if DirectWrite
// can't be.
TextMask RenderTextMaskWithGdi(HFONT font, const std::wstring& text, int width, int height,
                               UINT format) {
    TextMask mask;
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;  // Negative = top row first.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        return mask;
    }
    memset(bits, 0, (size_t)width * height * 4);  // Black.

    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ previousBitmap = SelectObject(dc, bitmap);
    HGDIOBJ previousFont = SelectObject(dc, font);
    SetTextColor(dc, RGB(255, 255, 255));
    SetBkMode(dc, TRANSPARENT);
    RECT area = {0, 0, width, height};
    DrawTextW(dc, text.c_str(), -1, &area, format);
    GdiFlush();  // Make sure GDI has finished drawing before we read the pixels.

    mask.width = width;
    mask.height = height;
    mask.coverage.resize((size_t)width * height);
    const Pixel* pixels = (const Pixel*)bits;
    for (size_t i = 0; i < mask.coverage.size(); i++) {
        mask.coverage[i] = std::max({pixels[i].r, pixels[i].g, pixels[i].b});
    }

    SelectObject(dc, previousFont);
    SelectObject(dc, previousBitmap);
    DeleteDC(dc);
    DeleteObject(bitmap);
    return mask;
}

// Works out the coverage of `text` drawn in a `width` x `height` box. `font`
// and `format` are as for GDI's DrawText (see CreateTextLayout).
TextMask RenderTextMask(HFONT font, const std::wstring& text, int width, int height,
                        UINT format) {
    TextMask mask;
    if (width <= 0 || height <= 0) {
        return mask;
    }

    // A transparent picture for Direct2D to draw into.
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;  // Negative = top row first.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        return mask;
    }
    memset(bits, 0, (size_t)width * height * 4);
    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ previousBitmap = SelectObject(dc, bitmap);

    bool drawn = false;
    {
        std::lock_guard<std::mutex> lock(g_textLock);
        ComPtr<ID2D1DCRenderTarget> target;
        ComPtr<ID2D1SolidColorBrush> white;
        ComPtr<IDWriteTextLayout> layout;
        // Drawn by the processor ("software"), not the graphics card: for small
        // pictures that's quicker than sending them to the card and back.
        D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_SOFTWARE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f,
            96.0f);
        RECT area = {0, 0, width, height};
        if (EnsureTextFactories() &&
            SUCCEEDED(g_d2dFactory->CreateDCRenderTarget(&properties, &target)) &&
            SUCCEEDED(target->BindDC(dc, &area)) &&
            SUCCEEDED(target->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &white)) &&
            (layout = CreateTextLayout(font, text, (float)width, (float)height, format))) {
            target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            target->BeginDraw();
            target->Clear(D2D1::ColorF(0, 0, 0, 0));
            target->DrawTextLayout(D2D1::Point2F(0, 0), layout.Get(), white.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_CLIP);
            drawn = SUCCEEDED(target->EndDraw());
        }
    }

    if (drawn) {
        GdiFlush();
        mask.width = width;
        mask.height = height;
        mask.coverage.resize((size_t)width * height);
        const Pixel* pixels = (const Pixel*)bits;
        for (size_t i = 0; i < mask.coverage.size(); i++) {
            mask.coverage[i] = pixels[i].a;  // White, so the opacity is the coverage.
        }
    }
    SelectObject(dc, previousBitmap);
    DeleteDC(dc);
    DeleteObject(bitmap);

    return drawn ? mask : RenderTextMaskWithGdi(font, text, width, height, format);
}

// Draws one app: an optional rounded background, the icon, and the name
// underneath (up to two lines). In the list view: the icon on the left, the
// name beside it, and its size or "Folder" on the right.
void DrawPopupApp(IconCanvas& canvas, PopupItem& item, const RECT& cell,
                  COLORREF fillColor, float fillOpacity, const PopupColors& colors) {
    if (fillOpacity > 0) {
        canvas.FillRoundedRect((float)cell.left + Px(2), (float)cell.top + Px(2),
                               (float)cell.right - Px(2), (float)cell.bottom - Px(2),
                               (float)Px(6), fillColor, fillOpacity);
    }

    int iconPixels = Px(ItemIconSize());
    if (!item.iconLoaded) {
        item.icon = LoadItemIcon(item.path, iconPixels);
        item.iconLoaded = true;
    }

    if (g_popup.listView) {
        int iconLeft = cell.left + Px(10);
        canvas.DrawImage(item.icon, iconLeft,
                         cell.top + (cell.bottom - cell.top - iconPixels) / 2);
        int detailWidth = item.detail.empty() ? 0 : Px(76);
        RECT label = {iconLeft + iconPixels + Px(10), cell.top,
                      cell.right - Px(10) - detailWidth, cell.bottom};
        int width = label.right - label.left, height = label.bottom - label.top;
        if (!item.labelIsList || item.labelMask.width != width ||
            item.labelMask.height != height) {
            item.labelMask = RenderTextMask(
                g_popup.labelFont, item.name, width, height,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            item.labelIsList = true;
        }
        canvas.BlendMask(item.labelMask, label.left, label.top, colors.text);

        if (detailWidth > 0) {
            RECT detail = {label.right, cell.top, cell.right - Px(10), cell.bottom};
            width = detail.right - detail.left;
            if (item.detailMask.width != width || item.detailMask.height != height) {
                item.detailMask = RenderTextMask(
                    g_popup.labelFont, item.detail, width, height,
                    DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            }
            canvas.BlendMask(item.detailMask, detail.left, detail.top, colors.dimText);
        }
        return;
    }

    canvas.DrawImage(item.icon, cell.left + (Px(CellWidth()) - iconPixels) / 2,
                     cell.top + Px(IconTop()));

    if (!g_popup.showLabels) {
        return;
    }
    RECT label = {cell.left + Px(4),
                  cell.top + Px(IconTop()) + iconPixels + Px(Layout::labelGap),
                  cell.right - Px(4), cell.bottom - Px(4)};
    int width = label.right - label.left, height = label.bottom - label.top;
    if (item.labelIsList || item.labelMask.width != width || item.labelMask.height != height) {
        item.labelMask = RenderTextMask(
            g_popup.labelFont, item.name, width, height,
            DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL | DT_NOPREFIX);
        item.labelIsList = false;
    }
    canvas.BlendMask(item.labelMask, label.left, label.top, colors.text);
}

// Where the lifted app is drawn while rearranging: under the mouse, held at
// the same spot you grabbed it.
RECT FloatingCellRect() {
    RECT cell;
    cell.left = g_popup.cursor.x - g_popup.grabOffset.x;
    cell.top = g_popup.cursor.y - g_popup.grabOffset.y;
    cell.right = cell.left + Px(CellWidth());
    cell.bottom = cell.top + Px(CellHeight());
    return cell;
}

// Draws the whole popup into a canvas, then copies it to the screen in one go
// (so there's no flicker).
// For the open/close animation: resizes the finished picture to the window's
// current size, and fades it. Each target pixel blends the four nearest
// source pixels ("bilinear" sampling), so it looks smooth rather than blocky.
std::vector<Pixel> ScaleAndFade(const IconCanvas& canvas, int width, int height,
                                float opacity) {
    std::vector<Pixel> result((size_t)width * height);
    const std::vector<Pixel>& source = canvas.Pixels();
    int sourceWidth = canvas.Width(), sourceHeight = canvas.Height();

    for (int y = 0; y < height; y++) {
        float sourceY = std::clamp((y + 0.5f) * sourceHeight / height - 0.5f, 0.0f,
                                   (float)(sourceHeight - 1));
        int y0 = (int)sourceY, y1 = std::min(y0 + 1, sourceHeight - 1);
        float fy = sourceY - y0;
        for (int x = 0; x < width; x++) {
            float sourceX = std::clamp((x + 0.5f) * sourceWidth / width - 0.5f, 0.0f,
                                       (float)(sourceWidth - 1));
            int x0 = (int)sourceX, x1 = std::min(x0 + 1, sourceWidth - 1);
            float fx = sourceX - x0;

            const Pixel& a = source[(size_t)y0 * sourceWidth + x0];
            const Pixel& b = source[(size_t)y0 * sourceWidth + x1];
            const Pixel& c = source[(size_t)y1 * sourceWidth + x0];
            const Pixel& d = source[(size_t)y1 * sourceWidth + x1];
            auto mix = [&](BYTE Pixel::*channel) {
                float top = a.*channel + (b.*channel - a.*channel) * fx;
                float bottom = c.*channel + (d.*channel - c.*channel) * fx;
                return (BYTE)std::lround((top + (bottom - top) * fy) * opacity);
            };
            result[(size_t)y * width + x] = {mix(&Pixel::b), mix(&Pixel::g),
                                             mix(&Pixel::r), mix(&Pixel::a)};
        }
    }
    return result;
}

void PaintPopup(HWND window, HDC screen) {
    RECT client;
    GetClientRect(window, &client);
    if (client.right <= 0 || client.bottom <= 0 || g_popup.layoutSize.cx <= 0) {
        return;
    }
    PopupColors colors = GetPopupColors();

    // Always draw the popup at its full size. While it's animating, the
    // window is smaller, and the finished picture is scaled down to fit.
    int fullWidth = g_popup.layoutSize.cx, fullHeight = g_popup.layoutSize.cy;
    IconCanvas canvas(fullWidth, fullHeight);

    // With the blur, the background is left fully see-through, and Windows
    // shows the blurred desktop there. Otherwise, fill it in.
    if (!g_popup.glass) {
        canvas.FillAll(colors.background);
    }

    // Title: the group's name. Hovering shows a highlight and a pencil, to
    // hint that you can click to rename. While renaming, the text box sits on
    // top, and we draw a field behind it with a coloured line underneath, like
    // Windows 11 text fields.
    RECT titleRect = PopupTitleRect(fullWidth);
    bool renaming = g_popup.renameBox != nullptr;
    if (renaming) {
        canvas.FillRoundedRect((float)titleRect.left, (float)titleRect.top,
                               (float)titleRect.right, (float)titleRect.bottom,
                               (float)Px(6), colors.field, 1.0f);
        canvas.FillRoundedRect((float)titleRect.left + Px(4), (float)titleRect.bottom - Px(2),
                               (float)titleRect.right - Px(4), (float)titleRect.bottom,
                               (float)Px(1), colors.accent, 1.0f);
    } else if (g_popup.titleHovered) {
        canvas.FillRoundedRect((float)titleRect.left, (float)titleRect.top,
                               (float)titleRect.right, (float)titleRect.bottom,
                               (float)Px(6), colors.shade,
                               g_popup.titlePressed ? colors.pressedOpacity
                                                    : colors.hoverOpacity);
    }

    if (!renaming) {
        RECT text = {titleRect.left + Px(8), titleRect.top, titleRect.right - Px(32),
                     titleRect.bottom};
        int width = text.right - text.left, height = text.bottom - text.top;
        if (g_popup.titleMaskText != g_popup.title || g_popup.titleMask.width != width ||
            g_popup.titleMask.height != height) {
            g_popup.titleMask = RenderTextMask(
                g_popup.titleFont, g_popup.title, width, height,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            g_popup.titleMaskText = g_popup.title;
        }
        canvas.BlendMask(g_popup.titleMask, text.left, text.top, colors.text);

        if (g_popup.titleHovered) {
            RECT pencil = {titleRect.right - Px(32), titleRect.top, titleRect.right - Px(8),
                           titleRect.bottom};
            TextMask symbol = RenderTextMask(g_popup.symbolFont, L"\xE70F",
                                             pencil.right - pencil.left,
                                             pencil.bottom - pencil.top,
                                             DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            canvas.BlendMask(symbol, pencil.left, pencil.top, colors.dimText);
        }
    }

    // The gear button: highlighted when hovered, and while its panel is open.
    RECT gear = PopupGearRect(fullWidth);
    bool panelOpen = IsSettingsPanelOpen();
    if (g_popup.gearHovered || panelOpen) {
        float strength = (g_popup.gearPressed || panelOpen) ? colors.pressedOpacity * 2
                                                            : colors.hoverOpacity;
        canvas.FillRoundedRect((float)gear.left, (float)gear.top, (float)gear.right,
                               (float)gear.bottom, (float)Px(6), colors.shade, strength);
    }
    TextMask gearSymbol = RenderTextMask(g_popup.symbolFont, L"\xE713", gear.right - gear.left,
                                         gear.bottom - gear.top,
                                         DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    canvas.BlendMask(gearSymbol, gear.left, gear.top,
                     (g_popup.gearHovered || panelOpen) ? colors.text : colors.dimText);

    // The "open in File Explorer" button: an open folder symbol.
    RECT openButton = PopupOpenRect(fullWidth);
    if (g_popup.openHovered) {
        canvas.FillRoundedRect((float)openButton.left, (float)openButton.top,
                               (float)openButton.right, (float)openButton.bottom, (float)Px(6),
                               colors.shade,
                               g_popup.openPressed ? colors.pressedOpacity : colors.hoverOpacity);
    }
    TextMask openSymbol = RenderTextMask(g_popup.symbolFont, L"\xE838",
                                         openButton.right - openButton.left,
                                         openButton.bottom - openButton.top,
                                         DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    canvas.BlendMask(openSymbol, openButton.left, openButton.top,
                     g_popup.openHovered ? colors.text : colors.dimText);

    if (g_popup.items.empty()) {
        RECT message = {0, ContentTop(), fullWidth, ContentTop() + g_popup.viewportHeight};
        TextMask text = RenderTextMask(g_popup.labelFont,
                                       Tr(g_popup.isGroup ? Text::GroupEmpty
                                                          : Text::FolderEmpty),
                                       message.right - message.left,
                                       message.bottom - message.top,
                                       DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        canvas.BlendMask(text, message.left, message.top, colors.dimText);
    }

    // Each app, drawn where it currently is, moved up by the scroll
    // position, and only inside the scrolling area (fading out at an edge
    // with more to scroll to). Apps out of view are skipped. The highlight
    // shows which app the mouse is over, a little fainter while the button
    // is held down on it, so you can see a click is in progress.
    int viewTop = ContentTop(), viewBottom = ContentTop() + g_popup.viewportHeight;
    int scrolled = (int)std::lround(g_popup.scroll);
    int fade = Px(16);
    canvas.SetClip(viewTop, viewBottom, scrolled > 0 ? fade : 0,
                   scrolled < MaxScroll() ? fade : 0);
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        if (g_popup.reordering && i == g_popup.dragged) {
            continue;  // The lifted app is drawn last, on top of the others.
        }
        PopupItem& item = g_popup.items[i];
        RECT cell;
        cell.left = (int)std::lround(item.x);
        cell.top = (int)std::lround(item.y) - scrolled;
        cell.right = cell.left + Px(CellWidth());
        cell.bottom = cell.top + Px(CellHeight());
        if (cell.bottom <= viewTop || cell.top >= viewBottom) {
            continue;
        }

        float highlight = 0;
        if (!g_popup.reordering && i == g_popup.hovered) {
            highlight = (i == g_popup.pressed) ? colors.pressedOpacity : colors.hoverOpacity;
        }
        DrawPopupApp(canvas, item, cell, colors.shade, highlight, colors);
    }
    canvas.ClearClip();

    // The scroll bar, if there's anything to scroll: thin, and wider while
    // the mouse is over it.
    RECT thumb;
    if (ScrollThumbRect(fullWidth, &thumb)) {
        bool active = g_popup.scrollHovered || g_popup.scrollDragging;
        int barWidth = Px(active ? 6 : 3);
        float right = (float)thumb.right - Px(2);
        canvas.FillRoundedRect(right - barWidth, (float)thumb.top, right, (float)thumb.bottom,
                               barWidth / 2.0f, colors.text, active ? 0.55f : 0.35f);
    }

    // The app being rearranged, "lifted" under the mouse. It gets a mostly
    // solid tile, so it stays readable as it passes over the other apps.
    if (g_popup.reordering) {
        DrawPopupApp(canvas, g_popup.items[g_popup.dragged], FloatingCellRect(),
                     colors.field, 0.92f, colors);
    }

    // High contrast themes rely on clear outlines.
    if (g_popup.highContrast) {
        canvas.FillRoundedRect(0, 0, (float)fullWidth, (float)fullHeight, (float)Px(8), 0, 0,
                               colors.text, 1.0f, (float)Px(1));
    }

    // Copy the canvas to the screen (scaled and faded, if animating). Its
    // see-through pixels stay see-through, which is what lets the blur show.
    std::vector<Pixel> scaled;
    const Pixel* finished = canvas.Pixels().data();
    if (client.right != fullWidth || client.bottom != fullHeight ||
        g_popup.contentOpacity < 1) {
        scaled = ScaleAndFade(canvas, client.right, client.bottom, g_popup.contentOpacity);
        finished = scaled.data();
    }

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = client.right;
    info.bmiHeader.biHeight = -client.bottom;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP buffer = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!buffer || !bits) {
        return;
    }
    memcpy(bits, finished, (size_t)client.right * client.bottom * sizeof(Pixel));
    HDC dc = CreateCompatibleDC(screen);
    HGDIOBJ previousBitmap = SelectObject(dc, buffer);
    BitBlt(screen, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, previousBitmap);
    DeleteDC(dc);
    DeleteObject(buffer);
}

void SetPopupHovered(HWND window, int index) {
    if (index != g_popup.hovered) {
        g_popup.hovered = index;
        InvalidateRect(window, nullptr, FALSE);  // Ask Windows to repaint.
    }
}

void LaunchPopupItem(HWND window, int index) {
    // Copy the path first: closing the popup frees the item list.
    std::wstring path = g_popup.items[index].path;
    DestroyWindow(window);

    SHELLEXECUTEINFOW info = {};
    info.cbSize = sizeof(info);
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"Couldn't launch it (error %u)", (unsigned)GetLastError());
    }
}

// Opens the group (or folder) as a normal File Explorer window.
void OpenFolderInExplorer(HWND window) {
    std::wstring path = g_popup.folderPath;
    DestroyWindow(window);

    SHELLEXECUTEINFOW info = {};
    info.cbSize = sizeof(info);
    info.lpVerb = L"open";
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"Couldn't open the folder (error %u)", (unsigned)GetLastError());
    }
}

// --- Renaming ----------------------------------------------------------------

// Windows doesn't allow some characters in file and folder names, or names
// that end in a space or dot. Tidy up whatever was typed.
std::wstring CleanGroupName(std::wstring name) {
    name.erase(std::remove_if(name.begin(), name.end(),
                              [](wchar_t c) {
                                  return c < 32 || wcschr(L"\\/:*?\"<>|", c) != nullptr;
                              }),
               name.end());
    size_t start = name.find_first_not_of(L' ');
    if (start == std::wstring::npos) {
        return L"";
    }
    name = name.substr(start);
    while (!name.empty() && (name.back() == L' ' || name.back() == L'.')) {
        name.pop_back();
    }
    return name;
}

// Renames the group folder. The desktop label updates by itself, and the
// group watcher (section 7) notices the rename and fixes up the icon.
void RenameGroup(const std::wstring& typedName) {
    std::wstring oldPath = g_popup.folderPath;
    size_t lastSlash = oldPath.find_last_of(L'\\');
    std::wstring parent = oldPath.substr(0, lastSlash);
    std::wstring oldName = oldPath.substr(lastSlash + 1);

    std::wstring newName = CleanGroupName(typedName);
    if (newName.empty() || newName == oldName) {
        return;
    }
    std::wstring newPath = parent + L"\\" + newName;

    // Changing only capital letters ("games" to "Games") is fine, but any
    // other name that's already taken isn't.
    if (_wcsicmp(newName.c_str(), oldName.c_str()) != 0 &&
        GetFileAttributesW(newPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Can't rename the group: \"%s\" already exists", newName.c_str());
        MessageBeep(MB_ICONWARNING);
        return;
    }

    // IFileOperation again, so Ctrl+Z on the desktop undoes the rename.
    ComPtr<IFileOperation> operation;
    ComPtr<IShellItem> folder;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&operation))) ||
        FAILED(SHCreateItemFromParsingName(oldPath.c_str(), nullptr,
                                           IID_PPV_ARGS(&folder)))) {
        Wh_Log(L"Couldn't start renaming the group");
        return;
    }
    operation->SetOperationFlags(FOF_ALLOWUNDO);
    operation->RenameItem(folder.Get(), newName.c_str(), nullptr);
    HRESULT result = operation->PerformOperations();
    BOOL aborted = FALSE;
    operation->GetAnyOperationsAborted(&aborted);
    if (FAILED(result) || aborted) {
        Wh_Log(L"Couldn't rename the group (error 0x%08X)", (unsigned)result);
        return;
    }

    Wh_Log(L"Renamed group \"%s\" to \"%s\"", oldName.c_str(), newName.c_str());
    g_popup.folderPath = newPath;
    g_popup.title = newName;
    // The apps' paths include the folder name, so update those too.
    for (PopupItem& item : g_popup.items) {
        item.path = newPath + item.path.substr(oldPath.size());
    }
}

// The rename box is a standard Windows EDIT control. It does almost
// everything for us (typing, selecting, copy and paste). We subclass it only
// to catch Enter (keep the name), Esc (cancel) and clicking away (keep).
// Instead of finishing straight away, it posts a message to the popup: a
// window shouldn't be destroyed while it's still handling one of its own
// messages.
LRESULT CALLBACK RenameBoxProc(HWND box, UINT message, WPARAM wParam, LPARAM lParam,
                               UINT_PTR id, DWORD_PTR) {
    BusyScope busy;
    switch (message) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                PostMessageW(GetParent(box), kMsgFinishRename, wParam == VK_RETURN, 0);
                return 0;
            }
            break;

        case WM_CHAR:
            // Stop the "ding" Windows plays for Enter/Esc in a one-line box.
            if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                return 0;
            }
            break;

        case WM_KILLFOCUS:
            PostMessageW(GetParent(box), kMsgFinishRename, 1, 0);
            break;

        case WM_NCDESTROY:
            RemoveWindowSubclass(box, RenameBoxProc, id);
            break;
    }
    return DefSubclassProc(box, message, wParam, lParam);
}

void StartRename(HWND window) {
    if (g_popup.renameBox) {
        return;
    }

    // Size the box to one line of title text, centred in the title area.
    RECT client;
    GetClientRect(window, &client);
    RECT titleRect = PopupTitleRect(client.right);
    HDC dc = GetDC(window);
    HGDIOBJ previousFont = SelectObject(dc, g_popup.titleFont);
    TEXTMETRICW metrics = {};
    GetTextMetricsW(dc, &metrics);
    SelectObject(dc, previousFont);
    ReleaseDC(window, dc);
    int height = metrics.tmHeight;
    int top = titleRect.top + (titleRect.bottom - titleRect.top - height) / 2;

    HWND box = CreateWindowExW(0, L"EDIT", g_popup.title.c_str(),
                               WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                               titleRect.left + Px(8), top,
                               titleRect.right - titleRect.left - Px(16), height,
                               window, nullptr, g_module, nullptr);
    if (!box) {
        return;
    }
    g_popup.renameBox = box;
    g_popup.renameBoxBrush = CreateSolidBrush(GetPopupColors().field);

    // The text box draws itself with GDI, which on the blurred background
    // would come out see-through and garbled. Making it a "layered" window
    // with full opacity (255) tells Windows to show it solid.
    if (g_popup.glass) {
        SetWindowLongPtrW(box, GWL_EXSTYLE, GetWindowLongPtrW(box, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(box, 0, 255, LWA_ALPHA);
    }

    SendMessageW(box, WM_SETFONT, (WPARAM)g_popup.titleFont, FALSE);
    SendMessageW(box, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, 0);
    SendMessageW(box, EM_LIMITTEXT, 100, 0);
    SetWindowSubclass(box, RenameBoxProc, 1, 0);
    SendMessageW(box, EM_SETSEL, 0, -1);  // Select all, ready to type over.
    SetFocus(box);
    InvalidateRect(window, nullptr, FALSE);
}

// Ends renaming. `keep` applies the typed name. `refocus` gives the keyboard
// back to the popup (so Esc closes it), which only makes sense if the popup
// is still the active window.
void FinishRename(HWND window, bool keep, bool refocus) {
    HWND box = g_popup.renameBox;
    if (!box) {
        return;  // Already finished (this can be asked twice; see above).
    }
    g_popup.renameBox = nullptr;

    WCHAR typed[MAX_PATH] = L"";
    GetWindowTextW(box, typed, ARRAYSIZE(typed));
    if (refocus) {
        SetFocus(window);
    }
    DestroyWindow(box);
    if (g_popup.renameBoxBrush) {
        DeleteObject(g_popup.renameBoxBrush);
        g_popup.renameBoxBrush = nullptr;
    }

    if (keep) {
        RenameGroup(typed);
    }
    InvalidateRect(window, nullptr, FALSE);
}

// --- Dragging apps out -------------------------------------------------------
//
// To drag a file, Windows needs an "IDataObject" describing it. The file's
// IShellItem can make one for us (BindToHandler with BHID_DataObject), and
// SHDoDragDrop runs the drag. Whatever you drop onto (the desktop, a folder,
// the taskbar...) decides what happens; dropping onto the desktop moves the
// app out of the group.
//
// The popup hides while you drag, so you can drop onto the desktop under it.

void DragAppOut(HWND window, int index) {
    std::wstring path = g_popup.items[index].path;
    g_popup.pressed = -1;
    if (GetCapture() == window) {
        ReleaseCapture();
    }

    ComPtr<IShellItem> item;
    ComPtr<IDataObject> data;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) ||
        FAILED(item->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&data)))) {
        Wh_Log(L"Couldn't start dragging \"%s\"", path.c_str());
        return;
    }

    // Show the app's icon under the pointer while dragging.
    ComPtr<IDragSourceHelper> dragImageHelper;
    if (SUCCEEDED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&dragImageHelper)))) {
        int size = Px(ItemIconSize());
        SHDRAGIMAGE image = {};
        image.sizeDragImage = {size, size};
        image.ptOffset = {size / 2, size / 2};
        image.hbmpDragImage = GetItemIcon(path, size);
        image.crColorKey = CLR_NONE;
        // On success the helper owns the picture; otherwise we delete it.
        if (image.hbmpDragImage &&
            FAILED(dragImageHelper->InitializeFromBitmap(&image, data.Get()))) {
            DeleteObject(image.hbmpDragImage);
        }
    }

    CloseSettingsPanel();
    g_popup.draggingOut = true;
    g_draggingFromPopup = true;
    ShowWindow(window, SW_HIDE);

    DWORD effect = DROPEFFECT_NONE;
    HRESULT result = SHDoDragDrop(window, data.Get(), nullptr,
                                  DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK,
                                  &effect);

    g_draggingFromPopup = false;
    (void)result;  // Whatever happened, the popup closes now.
    DestroyWindow(window);
}

// --- Rearranging apps --------------------------------------------------------
//
// Pressing on an app and moving the mouse "lifts" it: it follows the pointer,
// and the other apps slide aside to show where it would go. Letting go puts
// it there and saves the new order. Moving past the popup's edge switches to
// dragging the app out of the group instead (DragAppOut, above).
//
// The sliding is a simple animation. A timer fires about 60 times a second,
// and each time, every app moves a third of the way from where it's drawn to
// where it belongs. That gives a quick start and a gentle stop.

constexpr UINT_PTR kAnimationTimer = 1;

// The order apps are shown in right now. Normally that's the item list's
// order, but while rearranging, the lifted app is moved to the slot under the
// mouse and the others shuffle along to make room.
std::vector<int> DisplayOrder() {
    std::vector<int> order;
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        if (!g_popup.reordering || i != g_popup.dragged) {
            order.push_back(i);
        }
    }
    if (g_popup.reordering) {
        order.insert(order.begin() + g_popup.reorderSlot, g_popup.dragged);
    }
    return order;
}

// The slot whose centre is nearest the mouse (`point` is a content position).
int SlotNearest(POINT point, int clientWidth) {
    int nearest = 0;
    long long nearestDistance = LLONG_MAX;
    for (int slot = 0; slot < (int)g_popup.items.size(); slot++) {
        RECT cell = PopupCellRect(slot, clientWidth);
        long long dx = (cell.left + cell.right) / 2 - point.x;
        long long dy = (cell.top + cell.bottom) / 2 - point.y;
        if (dx * dx + dy * dy < nearestDistance) {
            nearestDistance = dx * dx + dy * dy;
            nearest = slot;
        }
    }
    return nearest;
}

// Moves every app one step closer to its slot. Returns false once they've
// all arrived.
bool StepAnimation(HWND window) {
    RECT client;
    GetClientRect(window, &client);
    bool stillMoving = false;
    auto step = [&](float& position, float goal) {
        float distance = goal - position;
        if (std::fabs(distance) < 0.5f) {
            position = goal;
        } else {
            position += distance * 0.35f;
            stillMoving = true;
        }
    };

    std::vector<int> order = DisplayOrder();
    for (int slot = 0; slot < (int)order.size(); slot++) {
        PopupItem& item = g_popup.items[order[slot]];
        RECT target = PopupCellRect(slot, client.right);
        step(item.x, (float)target.left);
        step(item.y, (float)target.top);
    }
    return stillMoving;
}

void StartAnimation(HWND window) {
    SetTimer(window, kAnimationTimer, 15, nullptr);  // Every 15 ms.
}

// --- Gliding while scrolling -------------------------------------------------
//
// The mouse wheel moves g_popup.scrollTarget, and a timer glides the scroll
// position towards it, like the animation above. While an app is lifted to
// be rearranged and held near the top or bottom of the scrolling area, the
// timer keeps scrolling that way (faster the closer to the edge), so it can
// go anywhere in a long list.

constexpr UINT_PTR kScrollTimer = 4;

void StartScrolling(HWND window) {
    SetTimer(window, kScrollTimer, 15, nullptr);
}

// The apps' highlight follows the mouse as the contents move under it.
void UpdateHoveredFromCursor(HWND window) {
    if (g_popup.pressed >= 0 || g_popup.reordering) {
        return;
    }
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(window, &cursor);
    SetPopupHovered(window, PopupHitTest(window, cursor));
}

// One step. Returns false once there's nothing left to do.
bool StepScroll(HWND window) {
    RECT client;
    GetClientRect(window, &client);
    float maxScroll = (float)MaxScroll();

    bool keepGoing = false;
    if (g_popup.reordering) {
        RECT viewport = ViewportRect(client.right);
        int band = Px(32);
        float speed = 0;
        if (g_popup.cursor.y < viewport.top + band) {
            speed = -(float)(viewport.top + band - g_popup.cursor.y) / band;
        } else if (g_popup.cursor.y > viewport.bottom - band) {
            speed = (float)(g_popup.cursor.y - (viewport.bottom - band)) / band;
        }
        speed = std::clamp(speed, -1.5f, 1.5f);
        g_popup.scrollTarget += speed * Px(14);
        keepGoing = (speed < 0 && g_popup.scrollTarget > 0) ||
                    (speed > 0 && g_popup.scrollTarget < maxScroll);
    }
    g_popup.scrollTarget = std::clamp(g_popup.scrollTarget, 0.0f, maxScroll);

    float distance = g_popup.scrollTarget - g_popup.scroll;
    bool moving = std::fabs(distance) >= 0.5f;
    g_popup.scroll = moving ? g_popup.scroll + distance * 0.35f : g_popup.scrollTarget;

    if (g_popup.reordering) {
        // The lifted app is over a different spot now.
        int slot = SlotNearest(ToContent(g_popup.cursor), client.right);
        if (slot != g_popup.reorderSlot) {
            g_popup.reorderSlot = slot;
            StartAnimation(window);
        }
    } else {
        UpdateHoveredFromCursor(window);
    }
    InvalidateRect(window, nullptr, FALSE);
    return moving || keepGoing;
}

// Scrolls by `amount` pixels (negative: up), gliding there.
void ScrollBy(HWND window, float amount) {
    g_popup.scrollTarget =
        std::clamp(g_popup.scrollTarget + amount, 0.0f, (float)MaxScroll());
    StartScrolling(window);
}

// Moves the scroll bar's thumb to follow the mouse while it's dragged.
void DragScrollBar(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT track = ScrollTrackRect(client.right);
    RECT thumb;
    if (!ScrollThumbRect(client.right, &thumb)) {
        return;
    }
    int travel = (track.bottom - track.top) - (thumb.bottom - thumb.top);
    if (travel <= 0) {
        return;
    }
    float position = (float)(point.y - g_popup.scrollGrab - track.top) / travel;
    g_popup.scroll = g_popup.scrollTarget =
        std::clamp(position, 0.0f, 1.0f) * (float)MaxScroll();
    InvalidateRect(window, nullptr, FALSE);
}

// --- Closing when the mouse leaves -------------------------------------------
//
// A popup opened by hovering is just a peek: it closes once the mouse has
// been away from it (and from the group's icon) for a moment. Clicking inside
// it makes it stay, like a popup opened by clicking. Nothing closes while
// something's in progress, like a right-click menu or renaming.

constexpr UINT_PTR kLeaveTimer = 5;
constexpr int kCloseAfterLeavingMilliseconds = 400;

void ClosePopupAnimated(HWND window);

void CheckMouseLeft(HWND window) {
    if (!g_popup.closeOnLeave || g_popup.animation == 2) {
        return;
    }
    if (GetForegroundWindow() == window) {
        // You clicked inside it: it stays, and closes when you click away.
        g_popup.closeOnLeave = false;
        KillTimer(window, kLeaveTimer);
        return;
    }
    if (g_popup.menuOpen || g_popup.renameBox || g_popup.reordering ||
        g_popup.draggingOut || g_popup.pressed >= 0 || g_popup.scrollDragging ||
        GetCapture() == window) {
        g_popup.leftTime = 0;
        return;
    }

    POINT cursor;
    GetCursorPos(&cursor);
    RECT popupRect, panelRect = {};
    GetWindowRect(window, &popupRect);
    if (HWND panel = SettingsPanelWindow()) {
        GetWindowRect(panel, &panelRect);
    }
    bool inside = PtInRect(&popupRect, cursor) || PtInRect(&g_popup.itemRect, cursor) ||
                  PtInRect(&panelRect, cursor);
    ULONGLONG now = GetTickCount64();
    if (inside) {
        g_popup.leftTime = 0;
    } else if (!g_popup.leftTime) {
        g_popup.leftTime = now;
    } else if (now - g_popup.leftTime >= kCloseAfterLeavingMilliseconds) {
        ClosePopupAnimated(window);
    }
}

void SaveGroupOrder(const std::wstring& folderPath, const std::vector<PopupItem>& items) {
    std::wstring order;
    for (const PopupItem& item : items) {
        if (!order.empty()) {
            order += L"|";
        }
        order += FileNameOf(item.path);
    }
    WriteGroupValue(folderPath, L"Order", order.c_str());

    // Ask the group watcher (section 7) to redraw the group's icon now, in
    // the background, so the popup's animation stays smooth. (It only
    // redraws if the first four apps are different.)
    RequestGroupCheck(folderPath);
}

void StartReorder(HWND window, int index, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT cell = PopupCellRect(index, client.right);

    g_popup.reordering = true;
    g_popup.dragged = index;
    g_popup.reorderSlot = index;
    POINT pressed = ToContent(g_popup.pressPoint);
    g_popup.grabOffset = {pressed.x - cell.left, pressed.y - cell.top};
    g_popup.cursor = point;
    g_popup.pressed = -1;
    g_popup.hovered = -1;
    InvalidateRect(window, nullptr, FALSE);
}

// Ends rearranging. `keep` applies the new order; otherwise the app goes
// back where it was.
void FinishReorder(HWND window, bool keep) {
    if (!g_popup.reordering) {
        return;
    }
    std::vector<int> order = DisplayOrder();

    // Let the lifted app glide from under the mouse into its slot.
    RECT floating = FloatingCellRect();
    g_popup.items[g_popup.dragged].x = (float)floating.left;
    g_popup.items[g_popup.dragged].y = (float)floating.top + g_popup.scroll;
    g_popup.reordering = false;
    g_popup.dragged = -1;

    bool changed = false;
    for (int slot = 0; slot < (int)order.size(); slot++) {
        changed = changed || order[slot] != slot;
    }
    if (keep && changed) {
        std::vector<PopupItem> rearranged;
        for (int index : order) {
            rearranged.push_back(g_popup.items[index]);
        }
        g_popup.items = rearranged;

        // Arranging apps by hand only makes sense with "My order", so if this
        // group was sorted A to Z, switch it. (Other folders' orders are
        // only kept in Windhawk's storage; see PlaceForData.)
        if (GetGroupSetting(g_popup.folderPath, kSettingSort) != 0) {
            SetGroupOwnSetting(g_popup.folderPath, kSettingSort, 0);
            RefreshSettingsPanel();
        }
        SaveGroupOrder(g_popup.folderPath, g_popup.items);
    }

    StartAnimation(window);
    InvalidateRect(window, nullptr, FALSE);
}

// --- Right-click menus -------------------------------------------------------
//
// Windows can build the same right-click menu Explorer shows for any file or
// folder. We ask the item's IShellItem for its "IContextMenu" object, have it
// fill an empty menu with its commands, show the menu, and then ask it to run
// whichever command was chosen.
//
// Some entries, like "Open with" and "Send to", only build their submenus
// when you open them, and draw their own icons. For that, the menu object
// needs to see a few of our window's messages while the menu is open
// (IContextMenu2 and IContextMenu3 are versions of it that can take them).
// See the top of PopupWindowProc.

// Command numbers the menu object may use. 0 means "nothing chosen".
constexpr UINT kFirstMenuCommand = 1;
constexpr UINT kLastMenuCommand = 0x7FFF;

void ShowContextMenu(HWND window, const std::wstring& path, POINT screenPoint) {
    ComPtr<IShellItem> item;
    ComPtr<IContextMenu> menu;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) ||
        FAILED(item->BindToHandler(nullptr, BHID_SFUIObject, IID_PPV_ARGS(&menu)))) {
        Wh_Log(L"Couldn't get the right-click menu for \"%s\"", path.c_str());
        return;
    }

    HMENU popupMenu = CreatePopupMenu();
    if (!popupMenu) {
        return;
    }

    // Shift+right-click shows extra commands, as it does on the desktop.
    bool shiftHeld = GetKeyState(VK_SHIFT) < 0;
    UINT flags = CMF_NORMAL | (shiftHeld ? CMF_EXTENDEDVERBS : 0);
    if (FAILED(menu->QueryContextMenu(popupMenu, 0, kFirstMenuCommand, kLastMenuCommand,
                                      flags))) {
        DestroyMenu(popupMenu);
        return;
    }

    ComPtr<IContextMenu2> menu2;
    ComPtr<IContextMenu3> menu3;
    menu.As(&menu2);
    menu.As(&menu3);
    g_popup.contextMenu2 = menu2.Get();
    g_popup.contextMenu3 = menu3.Get();
    g_popup.menuOpen = true;

    // Show the menu and wait until something is chosen (or it's dismissed).
    // TPM_RETURNCMD makes it return the chosen command's number.
    UINT command = TrackPopupMenuEx(popupMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                    screenPoint.x, screenPoint.y, window, nullptr);
    g_popup.contextMenu2 = nullptr;
    g_popup.contextMenu3 = nullptr;

    if (command < kFirstMenuCommand) {
        // Dismissed without choosing anything. If that was by clicking
        // somewhere else (so the popup isn't the active window any more),
        // close the popup too, as clicking away normally would. Otherwise
        // keep it open.
        DestroyMenu(popupMenu);
        g_popup.menuOpen = false;
        if (GetForegroundWindow() != window) {
            DestroyWindow(window);
        }
        return;
    }

    // Like the Start menu, the popup goes away once you pick a command. Hide
    // it now, and destroy it after the command has run.
    CloseSettingsPanel();
    ShowWindow(window, SW_HIDE);


    // Any windows the command opens (like Properties, or "Are you sure?")
    // belong to the desktop rather than the popup, so they don't vanish
    // when the popup is destroyed.
    HWND owner = g_desktopDefView ? (HWND)g_desktopDefView : GetShellWindow();

    CMINVOKECOMMANDINFOEX invoke = {};
    invoke.cbSize = sizeof(invoke);
    invoke.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE |
                   (shiftHeld ? CMIC_MASK_SHIFT_DOWN : 0) |
                   (GetKeyState(VK_CONTROL) < 0 ? CMIC_MASK_CONTROL_DOWN : 0);
    invoke.hwnd = owner;
    // The command is identified by its number, counted from the first one.
    invoke.lpVerb = MAKEINTRESOURCEA(command - kFirstMenuCommand);
    invoke.lpVerbW = MAKEINTRESOURCEW(command - kFirstMenuCommand);
    invoke.nShow = SW_SHOWNORMAL;
    invoke.ptInvoke = screenPoint;
    HRESULT result = menu->InvokeCommand((CMINVOKECOMMANDINFO*)&invoke);
    if (FAILED(result) && result != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        Wh_Log(L"  ...that command failed (error 0x%08X)", (unsigned)result);
    }

    DestroyMenu(popupMenu);
    DestroyWindow(window);
}

// --- Opening and closing animation -------------------------------------------
//
// Like a Start menu folder, the popup grows out of the group's icon as it
// opens, with its contents fading in, and shrinks back into the icon as it
// closes. A timer moves the window a step at a time (about 60 times a
// second). How far along it should be is worked out from the time since the
// animation started, so it takes the same time even if a step runs late.

constexpr UINT_PTR kOpenCloseTimer = 2;
// How long opening takes is a setting ("Animation length"); closing takes
// three quarters of that, as closing should feel quicker.
int OpenCloseMilliseconds(bool opening) {
    return opening ? g_settings.openMilliseconds : g_settings.openMilliseconds * 3 / 4;
}

// A point `progress` (0 to 1) of the way from one rectangle to another.
RECT BlendRects(const RECT& from, const RECT& to, float progress) {
    auto blend = [&](LONG a, LONG b) { return (LONG)std::lround(a + (b - a) * progress); };
    return {blend(from.left, to.left), blend(from.top, to.top), blend(from.right, to.right),
            blend(from.bottom, to.bottom)};
}

void StepOpenClose(HWND window) {
    bool opening = g_popup.animation == 1;
    int duration = OpenCloseMilliseconds(opening);
    float time = std::min(1.0f, (GetTickCount64() - g_popup.animationStart) / (float)duration);

    // "Easing" curves, so the motion doesn't start and stop abruptly:
    // opening starts fast and slows down as it settles; closing starts
    // slowly and speeds up as it disappears.
    float progress = opening ? 1 - std::pow(1 - time, 3.0f) : time * time;

    RECT rect = opening ? BlendRects(g_popup.iconRect, g_popup.fullRect, progress)
                        : BlendRects(g_popup.fullRect, g_popup.iconRect, progress);
    g_popup.contentOpacity = opening ? progress : 1 - progress;
    SetWindowPos(window, nullptr, rect.left, rect.top,
                 std::max(1L, rect.right - rect.left), std::max(1L, rect.bottom - rect.top),
                 SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(window, nullptr, FALSE);

    if (time >= 1) {
        KillTimer(window, kOpenCloseTimer);
        if (opening) {
            g_popup.animation = 0;
            g_popup.contentOpacity = 1;
        } else {
            DestroyWindow(window);
        }
    }
}

void StartOpenClose(HWND window, int animation) {
    g_popup.animation = animation;
    g_popup.animationStart = GetTickCount64();
    SetTimer(window, kOpenCloseTimer, 15, nullptr);
    StepOpenClose(window);
}

// Closes the popup by shrinking it back into the group's icon (or straight
// away, if animations are turned off).
void ClosePopupAnimated(HWND window) {
    if (g_popup.animation == 2) {
        return;  // Already closing.
    }
    CloseSettingsPanel();
    FinishReorder(window, false);
    if (GetCapture() == window) {
        ReleaseCapture();
    }
    if (!g_settings.animatePopup || !IsWindowVisible(window)) {
        DestroyWindow(window);
        return;
    }
    // If it's still opening, carry on from wherever it's got to.
    RECT current;
    GetWindowRect(window, &current);
    g_popup.fullRect = current;
    g_popup.hovered = -1;
    StartOpenClose(window, 2);
}

// --- The popup's message handler ---------------------------------------------

// Cancels any dialog box on this thread that belongs to the popup (directly,
// or through the settings panel), as if you'd clicked Cancel.
void CloseDialogsOwnedBy(HWND popup) {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM popupParam) -> BOOL {
            WCHAR className[16] = L"";
            GetClassNameW(window, className, ARRAYSIZE(className));
            if (wcscmp(className, L"#32770") != 0) {  // The class of dialog boxes.
                return TRUE;
            }
            for (HWND owner = GetWindow(window, GW_OWNER); owner;
                 owner = GetWindow(owner, GW_OWNER)) {
                if (owner == (HWND)popupParam) {
                    SendMessageW(window, WM_COMMAND, IDCANCEL, 0);
                    break;
                }
            }
            return TRUE;
        },
        (LPARAM)popup);
}

LRESULT CALLBACK PopupWindowProc(HWND window, UINT message, WPARAM wParam,
                                 LPARAM lParam) {
    BusyScope busy;  // Covers right-click menus, renaming and dragging out.
    // While a right-click menu is open, pass the messages it needs for its
    // submenus and icons to the menu object.
    if ((g_popup.contextMenu2 || g_popup.contextMenu3) &&
        (message == WM_INITMENUPOPUP || message == WM_DRAWITEM ||
         message == WM_MEASUREITEM || message == WM_MENUCHAR)) {
        LRESULT result = 0;
        if (g_popup.contextMenu3 &&
            SUCCEEDED(g_popup.contextMenu3->HandleMenuMsg2(message, wParam, lParam, &result))) {
            return result;
        }
        if (g_popup.contextMenu2 && message != WM_MENUCHAR &&
            SUCCEEDED(g_popup.contextMenu2->HandleMenuMsg(message, wParam, lParam))) {
            return message == WM_INITMENUPOPUP ? 0 : TRUE;
        }
    }

    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            PaintPopup(window, dc);
            EndPaint(window, &paint);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;  // WM_PAINT draws the background, so skip this step.

        case WM_CTLCOLOREDIT: {
            // The rename box asks what colours to use. Match the popup.
            HDC dc = (HDC)wParam;
            PopupColors colors = GetPopupColors();
            SetTextColor(dc, colors.text);
            SetBkColor(dc, colors.field);
            return (LRESULT)g_popup.renameBoxBrush;
        }

        case WM_MOUSEMOVE: {
            if (g_popup.animation) {
                return 0;  // Ignore the mouse while opening or closing.
            }
            // Ask for a WM_MOUSELEAVE message when the mouse leaves.
            if (!g_popup.trackingMouse) {
                TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, window, 0};
                TrackMouseEvent(&track);
                g_popup.trackingMouse = true;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT client;
            GetClientRect(window, &client);

            if (g_popup.scrollDragging) {
                DragScrollBar(window, point);
                return 0;
            }

            // While rearranging, the lifted app follows the mouse, and the
            // others make room. Past the popup's edge, it becomes a drag out
            // of the group. (We get mouse messages even outside the popup,
            // because the mouse is "captured" while the button is down.)
            if (g_popup.reordering) {
                if (!PtInRect(&client, point)) {
                    int index = g_popup.dragged;
                    FinishReorder(window, false);
                    DragAppOut(window, index);
                    return 0;
                }
                g_popup.cursor = point;
                int slot = SlotNearest(ToContent(point), client.right);
                if (slot != g_popup.reorderSlot) {
                    g_popup.reorderSlot = slot;
                    StartAnimation(window);
                }
                // Near the top or bottom: scroll that way (see StepScroll).
                RECT viewport = ViewportRect(client.right);
                if (MaxScroll() > 0 && (point.y < viewport.top + Px(32) ||
                                        point.y > viewport.bottom - Px(32))) {
                    StartScrolling(window);
                }
                InvalidateRect(window, nullptr, FALSE);
                return 0;
            }

            // Pressing on an app and moving a few pixels lifts it to be
            // rearranged (or drags it out, if the mouse is already outside the
            // popup). The distance is Windows' own drag threshold, so it feels
            // the same as dragging anything else.
            if (g_popup.pressed >= 0 && (wParam & MK_LBUTTON) &&
                (std::abs(point.x - g_popup.pressPoint.x) > GetSystemMetrics(SM_CXDRAG) ||
                 std::abs(point.y - g_popup.pressPoint.y) > GetSystemMetrics(SM_CYDRAG))) {
                if (PtInRect(&client, point)) {
                    StartReorder(window, g_popup.pressed, point);
                } else {
                    DragAppOut(window, g_popup.pressed);
                }
                return 0;
            }

            bool overScrollBar = PopupScrollBarHitTest(window, point);
            SetPopupHovered(window, overScrollBar ? -1 : PopupHitTest(window, point));
            bool overTitle = PopupTitleHitTest(window, point);
            bool overGear = PopupGearHitTest(window, point);
            bool overOpen = PopupOpenHitTest(window, point);
            if (overTitle != g_popup.titleHovered || overGear != g_popup.gearHovered ||
                overOpen != g_popup.openHovered || overScrollBar != g_popup.scrollHovered) {
                g_popup.titleHovered = overTitle;
                g_popup.gearHovered = overGear;
                g_popup.openHovered = overOpen;
                g_popup.scrollHovered = overScrollBar;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            // One row per notch of the wheel (three in the list view).
            // Touchpads send smaller amounts, for smooth scrolling.
            if (g_popup.animation || MaxScroll() <= 0) {
                return 0;
            }
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            float rowsPerNotch = g_popup.listView ? 3.0f : 1.0f;
            ScrollBy(window, -(float)delta / WHEEL_DELTA * rowsPerNotch * Px(CellHeight()));
            return 0;
        }

        case WM_MOUSELEAVE:
            g_popup.trackingMouse = false;
            g_popup.titleHovered = false;
            g_popup.gearHovered = false;
            g_popup.openHovered = false;
            g_popup.scrollHovered = false;
            SetPopupHovered(window, -1);
            InvalidateRect(window, nullptr, FALSE);
            return 0;

        // A click only counts if the button went down AND came back up on the
        // same thing, like a real button. This matters because the popup can
        // open right under the mouse: the release of the click that opened it
        // lands on the popup, and must not launch anything.
        case WM_LBUTTONDOWN: {
            if (g_popup.animation) {
                return 0;
            }
            // If you double-click a group out of habit, the first click opens
            // the popup and the second lands on it. Ignore presses that come
            // within Windows' double-click time of the popup opening. (Not
            // for a popup opened by hovering, which no click opened.)
            if (!g_popup.hoverOpened &&
                GetTickCount64() - g_popup.openedTime < GetDoubleClickTime()) {
                g_popup.pressed = -1;
                return 0;
            }

            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

            // The scroll bar: drag its thumb, or click above or below it to
            // move a page.
            if (PopupScrollBarHitTest(window, point)) {
                RECT client, thumb;
                GetClientRect(window, &client);
                if (ScrollThumbRect(client.right, &thumb)) {
                    if (point.y >= thumb.top && point.y < thumb.bottom) {
                        g_popup.scrollDragging = true;
                        g_popup.scrollGrab = point.y - thumb.top;
                        SetCapture(window);
                    } else {
                        float page = g_popup.viewportHeight * 0.9f;
                        ScrollBy(window, point.y < thumb.top ? -page : page);
                    }
                }
                InvalidateRect(window, nullptr, FALSE);
                return 0;
            }

            g_popup.pressPoint = point;
            g_popup.pressed = PopupHitTest(window, point);
            g_popup.titlePressed = g_popup.pressed < 0 && !g_popup.renameBox &&
                                   PopupTitleHitTest(window, point);
            g_popup.gearPressed = g_popup.pressed < 0 && PopupGearHitTest(window, point);
            g_popup.openPressed = g_popup.pressed < 0 && PopupOpenHitTest(window, point);
            if (g_popup.pressed >= 0 || g_popup.titlePressed || g_popup.gearPressed ||
                g_popup.openPressed) {
                // "Capture" the mouse, so we still hear about the release
                // even if the pointer is dragged outside the popup first.
                SetCapture(window);
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_popup.animation) {
                return 0;
            }
            if (g_popup.scrollDragging) {
                g_popup.scrollDragging = false;
                if (GetCapture() == window) {
                    ReleaseCapture();
                }
                InvalidateRect(window, nullptr, FALSE);
                return 0;
            }
            if (g_popup.reordering) {
                FinishReorder(window, true);  // Drop the lifted app here.
                if (GetCapture() == window) {
                    ReleaseCapture();
                }
                return 0;
            }

            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index = PopupHitTest(window, point);
            int pressed = g_popup.pressed;
            bool titlePressed = g_popup.titlePressed;
            bool gearPressed = g_popup.gearPressed;
            bool openPressed = g_popup.openPressed;
            g_popup.pressed = -1;
            g_popup.titlePressed = false;
            g_popup.gearPressed = false;
            g_popup.openPressed = false;
            if (GetCapture() == window) {
                ReleaseCapture();
            }

            if (openPressed && PopupOpenHitTest(window, point)) {
                OpenFolderInExplorer(window);
            } else if (gearPressed && PopupGearHitTest(window, point)) {
                ToggleSettingsPanel(window);
            } else if (titlePressed && PopupTitleHitTest(window, point)) {
                StartRename(window);
            } else if (index >= 0 && index == pressed) {
                LaunchPopupItem(window, index);
            } else {
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            // Something else took the mouse (e.g. a window popped up), so
            // the click or rearrange in progress is cancelled.
            FinishReorder(window, false);
            if (g_popup.pressed >= 0 || g_popup.titlePressed || g_popup.gearPressed ||
                g_popup.openPressed || g_popup.scrollDragging) {
                g_popup.pressed = -1;
                g_popup.titlePressed = false;
                g_popup.gearPressed = false;
                g_popup.openPressed = false;
                g_popup.scrollDragging = false;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE && g_popup.reordering) {
                // Esc while rearranging puts the app back where it was.
                FinishReorder(window, false);
                if (GetCapture() == window) {
                    ReleaseCapture();
                }
            } else if (wParam == VK_ESCAPE && IsSettingsPanelOpen()) {
                CloseSettingsPanel();  // Esc closes the settings panel first.
            } else if (wParam == VK_ESCAPE) {
                ClosePopupAnimated(window);
            } else if (wParam == VK_F2) {
                StartRename(window);  // F2 renames, as it does in Explorer.
            }
            return 0;

        case kMsgFinishRename:
            FinishRename(window, wParam != 0, true);
            return 0;

        case WM_CLOSE:
            // Posted when the popup loses focus, and sent when the group is
            // clicked again. Shrink back into the icon instead of vanishing.
            ClosePopupAnimated(window);
            return 0;

        case kMsgCloseNow:
            // Sent when the mod is unloading. A right-click menu or a window
            // Windows opened for us (the colour picker, a delete
            // confirmation) would keep the mod's code waiting underneath it,
            // so close those first; then the popup itself.
            EndMenu();
            CloseDialogsOwnedBy(window);
            DestroyWindow(window);
            return 0;

        case WM_TIMER:
            if (wParam == kOpenCloseTimer) {
                StepOpenClose(window);
                return 0;
            }
            if (wParam == kAnimationTimer) {
                if (!StepAnimation(window)) {
                    KillTimer(window, kAnimationTimer);  // Everything's in place.
                }
                InvalidateRect(window, nullptr, FALSE);
            }
            if (wParam == kScrollTimer && !StepScroll(window)) {
                KillTimer(window, kScrollTimer);
            }
            if (wParam == kLeaveTimer) {
                CheckMouseLeft(window);
            }
            return 0;

        case WM_CONTEXTMENU: {
            // Right-click (or the Menu key). On an app: that app's menu.
            // Anywhere else: the group folder's menu.
            if (g_popup.reordering || g_popup.menuOpen || g_popup.animation) {
                return 0;
            }
            FinishRename(window, true, true);

            POINT screenPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index;
            if (screenPoint.x == -1 && screenPoint.y == -1) {
                // From the keyboard, so there's no click position. Use the
                // app under the mouse, and show the menu over it.
                index = g_popup.hovered;
                RECT client;
                GetClientRect(window, &client);
                RECT area = PopupTitleRect(client.right);
                if (index >= 0) {
                    area = PopupCellRect(index, client.right);
                    OffsetRect(&area, 0, -(int)std::lround(g_popup.scroll));
                }
                screenPoint = {(area.left + area.right) / 2, (area.top + area.bottom) / 2};
                ClientToScreen(window, &screenPoint);
            } else {
                POINT clientPoint = screenPoint;
                ScreenToClient(window, &clientPoint);
                index = PopupHitTest(window, clientPoint);
            }

            std::wstring path = index >= 0 ? g_popup.items[index].path : g_popup.folderPath;
            ShowContextMenu(window, path, screenPoint);
            return 0;
        }

        case WM_ACTIVATE:
            // While a right-click menu or its command is running, the popup
            // may lose focus (to an "Are you sure?" box, say). It closes
            // itself afterwards, so don't close it here.
            if (LOWORD(wParam) == WA_INACTIVE && !g_popup.menuOpen) {
                // Clicking away while renaming keeps the new name, like in
                // Explorer. Then close, like a menu (unless an app is being
                // dragged out, which hides the popup on purpose). Posting the
                // close lets Windows finish switching windows first.
                FinishRename(window, true, false);
                if (!g_popup.draggingOut) {
                    PostMessageW(window, WM_CLOSE, 0, 0);
                }
            }
            break;

        case WM_NCDESTROY: {
            // The very last message a window gets. Clean up here.
            g_lastClosedGroup = g_popup.folderPath;
            g_lastClosedTime = GetTickCount64();

            // For opening by hovering: if the mouse is still resting on the
            // group's icon, it shouldn't open again until it moves off.
            // Otherwise, coming back to it should open it again.
            POINT cursor;
            GetCursorPos(&cursor);
            HWND list = g_desktopList;
            if (!list || WindowFromPoint(cursor) != list) {
                g_hoverIcon = -1;
            }
            FreePopupResources();
            g_popupWindow = nullptr;
            break;
        }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RegisterPopupClass() {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_DROPSHADOW;  // A soft shadow, like menus have.
    windowClass.lpfnWndProc = PopupWindowProc;
    windowClass.hInstance = g_module;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = g_popupClassName.c_str();
    return RegisterClassExW(&windowClass) != 0;
}

// --- The blurred background --------------------------------------------------
//
// Windows has an official way to ask for a blurred "acrylic" background
// (DWMWA_SYSTEMBACKDROP_TYPE), but on borderless popups like ours it can
// quietly fall back to a plain colour.
//
// Instead we use the "accent policy", an older method that Windows' own
// taskbar uses, as do many Windhawk mods. It's undocumented: Microsoft hasn't
// published it, so we declare its structures ourselves (below) and look the
// function up by name. If it ever disappears, GetProcAddress gives us
// nullptr and the popup simply uses the solid background.

struct AccentPolicy {
    int state;            // 4 = ACCENT_ENABLE_ACRYLICBLURBEHIND.
    DWORD flags;          // 2 = use our tint colour.
    DWORD tintColor;      // 0xAABBGGRR: alpha (AA) is how strong the tint is.
    DWORD animationId;
};

struct WindowCompositionAttributeData {
    int attribute;        // 19 = WCA_ACCENT_POLICY.
    void* data;
    SIZE_T dataSize;
};

using SetWindowCompositionAttribute_t =
    BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

// Is Windows' "Transparency effects" setting (Settings > Personalization >
// Colors) turned on? When it's off, Windows avoids blur everywhere.
bool IsTransparencyEnabled() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value != 0;
}

SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute() {
    // GetProcAddress returns a generic function pointer; going through void*
    // turns it into the specific kind we declared.
    return (SetWindowCompositionAttribute_t)(void*)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
}

// Sets (or changes) the blur and the colour laid over it: the theme's
// background colour, mixed with the group's colour if it has one, at the
// strength set in the "Background colour strength" setting (60% by
// default), so what's behind still shows through softly.
bool SetBlurTint(HWND window) {
    auto setCompositionAttribute = GetSetWindowCompositionAttribute();
    if (!setCompositionAttribute) {
        return false;
    }
    AccentPolicy accent = {};
    accent.state = 4;
    accent.flags = 2;
    DWORD alpha = (DWORD)std::lround(g_settings.popupOpacity * 2.55);
    accent.tintColor = (alpha << 24) |
                       TintedBackground(g_popup.dark ? RGB(32, 32, 32) : RGB(243, 243, 243));
    WindowCompositionAttributeData data = {19, &accent, sizeof(accent)};
    return setCompositionAttribute(window, &data) != FALSE;
}

bool EnableBlurBehind(HWND window) {
    if (!IsTransparencyEnabled()) {
        Wh_Log(L"Windows' Transparency effects setting is off, so the popup uses a "
               L"solid background");
        return false;
    }
    if (!GetSetWindowCompositionAttribute()) {
        Wh_Log(L"Blur isn't available on this version of Windows");
        return false;
    }

    // Let Windows use our see-through pixels across the whole window, so the
    // blur shows wherever we leave the canvas see-through.
    MARGINS everywhere = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(window, &everywhere);

    if (!SetBlurTint(window)) {
        Wh_Log(L"Couldn't turn on the blur (error %u)", (unsigned)GetLastError());
        return false;
    }
    return true;
}

// --- Applying group settings to the popup ------------------------------------

// Reads this group's layout settings (see "Group settings" in section 5).
void ReadPopupLayoutSettings() {
    g_popup.columns = std::clamp(GetGroupSetting(g_popup.folderPath, kSettingColumns), 3, 6);
    int iconSize = GetGroupSetting(g_popup.folderPath, kSettingIconSize);
    g_popup.iconSize = iconSize <= 32 ? 32 : (iconSize >= 64 ? 64 : 48);
    g_popup.showLabels = GetGroupSetting(g_popup.folderPath, kSettingShowNames) != 0;
    g_popup.tint = GetGroupSetting(g_popup.folderPath, kSettingTint);
    g_popup.listView = GetGroupSetting(g_popup.folderPath, kSettingView) == 1;
    g_popup.spacing = std::clamp(GetGroupSetting(g_popup.folderPath, kSettingSpacing), 0, 2);
}

// Where the popup goes: beside the group, with its top level with the top of
// the group's icon. To the right if there's room, otherwise to the left, and
// always fully on screen (above the taskbar).
RECT ComputePopupRect(SIZE size) {
    const RECT& work = g_popup.workArea;
    int margin = Px(Layout::screenMargin);
    int gap = Px(8);

    int x = g_popup.itemRect.right + gap;
    if (x + size.cx > work.right - margin) {
        int leftSide = g_popup.itemRect.left - gap - size.cx;
        if (leftSide >= work.left + margin) {
            x = leftSide;
        }
    }
    int y = g_popup.iconRect.top;

    x = std::max((int)(work.left + margin), std::min(x, (int)(work.right - size.cx - margin)));
    y = std::max((int)(work.top + margin), std::min(y, (int)(work.bottom - size.cy - margin)));
    return {x, y, x + size.cx, y + size.cy};
}

// After a setting changes: re-read the layout, fetch icons again if their
// size changed, put the apps in the right order, resize the popup, and let
// the apps slide into their new places.
void ApplyPopupSettings(HWND window) {
    int oldIconSize = ItemIconSize();
    int oldTint = g_popup.tint;
    ReadPopupLayoutSettings();

    // A new colour: change the colour over the blur, for the popup and the
    // settings panel. (Without the blur, the next repaint uses it.)
    if (g_popup.tint != oldTint) {
        if (g_popup.glass) {
            SetBlurTint(window);
        }
        RetintSettingsPanel();
    }

    if (ItemIconSize() != oldIconSize) {
        ResetItemIcons();  // Fetched again at the new size when shown.
    }

    // Reorder to match the group's sort setting, keeping each app's icon and
    // current position (so it can slide to its new place).
    std::vector<PopupItem> sorted;
    for (const GroupEntry& entry : ListGroupFiles(g_popup.folderPath)) {
        for (size_t i = 0; i < g_popup.items.size(); i++) {
            if (_wcsicmp(g_popup.items[i].path.c_str(), entry.path.c_str()) == 0) {
                sorted.push_back(g_popup.items[i]);
                g_popup.items.erase(g_popup.items.begin() + i);
                break;
            }
        }
    }
    sorted.insert(sorted.end(), g_popup.items.begin(), g_popup.items.end());
    g_popup.items = sorted;

    SIZE size = PopupSize();
    ClampScroll();
    g_popup.layoutSize = size;
    g_popup.fullRect = ComputePopupRect(size);
    SetWindowPos(window, nullptr, g_popup.fullRect.left, g_popup.fullRect.top, size.cx,
                 size.cy, SWP_NOZORDER | SWP_NOACTIVATE);

    StartAnimation(window);
    PositionSettingsPanel();
    InvalidateRect(window, nullptr, FALSE);
}

// --- The settings panel ------------------------------------------------------
//
// Clicking the gear button opens a panel beside the popup, with the group's
// settings. A switch at the top chooses whether changes are for "This group"
// or "All groups". Changes apply straight away, so you can watch the popup
// change as you go.
//
// The panel is a second window, "owned" by the popup: Windows keeps it above
// the popup and destroys it along with the popup. It's also
// WS_EX_NOACTIVATE, so clicking it doesn't take the focus from the popup
// (which would make the popup close itself, as if you'd clicked away).
//
// Everything in it (the switches, buttons and text) is drawn on a canvas,
// like the popup. To work out what's where, BuildPanelLayout lists every
// piece with its position. The same list is used for drawing and for
// working out what the mouse is over, so the two always agree.

constexpr UINT_PTR kPanelTimer = 3;
constexpr int kPanelOpenMilliseconds = 150;

enum class PanelAction {
    Scope, View, Columns, IconSize, Spacing, ShowNames, Tint, Sort, RemoveWhenOneLeft, Reset
};

// Something you can click: one option of a choice, a toggle switch, or the
// reset button.
struct PanelControl {
    PanelAction action;
    int value = 0;           // For choices: the value this option stands for.
    RECT rect = {};
    std::wstring caption;    // For choices and the reset button.
    bool isToggle = false;
    bool isSwatch = false;   // A colour swatch; `value` is its colour.
};

enum class PanelTextStyle { Title, Section, Row, Note };

struct PanelText {
    std::wstring text;
    RECT rect = {};
    PanelTextStyle style = PanelTextStyle::Row;
    int settingId = -1;      // For rows: which setting (to show the dot).
};

struct PanelLayout {
    SIZE size = {};
    std::vector<PanelControl> controls;
    std::vector<PanelText> texts;
    std::vector<RECT> choiceBackgrounds;  // Behind each set of choices.
};

struct {
    HWND window = nullptr;
    bool allGroups = false;   // The "This group / All groups" switch.
    int hovered = -1;         // Which control the mouse is over.
    int pressed = -1;         // Which control the button went down on.
    bool trackingMouse = false;
    bool glass = false;
    ULONGLONG openedTime = 0;
    float opacity = 1;
    RECT finalRect = {};
    int slideDirection = 1;   // +1: slides out to the right; -1: to the left.
} g_panel;

bool IsSettingsPanelOpen() {
    return g_panel.window != nullptr;
}

HWND SettingsPanelWindow() {
    return g_panel.window;
}

// Redraws the panel (if it's open), e.g. after a setting changed elsewhere.
void RefreshSettingsPanel() {
    if (g_panel.window) {
        InvalidateRect(g_panel.window, nullptr, FALSE);
    }
}

// Updates the colour over the panel's blur, after the group's colour changed.
void RetintSettingsPanel() {
    if (g_panel.window) {
        if (g_panel.glass) {
            SetBlurTint(g_panel.window);
        }
        InvalidateRect(g_panel.window, nullptr, FALSE);
    }
}

GroupSettingId SettingForAction(PanelAction action) {
    switch (action) {
        case PanelAction::View:
            return kSettingView;
        case PanelAction::Columns:
            return kSettingColumns;
        case PanelAction::IconSize:
            return kSettingIconSize;
        case PanelAction::Spacing:
            return kSettingSpacing;
        case PanelAction::ShowNames:
            return kSettingShowNames;
        case PanelAction::Sort:
            return kSettingSort;
        case PanelAction::Tint:
            return kSettingTint;
        default:
            return kSettingRemoveWhenOneLeft;
    }
}

// The value a control shows: for "All groups", the all-groups value; for
// "This group", whatever applies to this group.
int PanelValue(PanelAction action) {
    if (action == PanelAction::Scope) {
        return g_panel.allGroups ? 1 : 0;
    }
    GroupSettingId id = SettingForAction(action);
    return g_panel.allGroups ? GetAllGroupsSetting(id) : GetGroupSetting(g_popup.folderPath, id);
}

bool GroupHasAnyOwnSettings() {
    for (int id = 0; id < kGroupSettingCount; id++) {
        int value;
        if (GetGroupOwnSetting(g_popup.folderPath, (GroupSettingId)id, &value)) {
            return true;
        }
    }
    return false;
}

// How wide `text` is in `font`, in pixels. Measured with DirectWrite, the
// same way it's drawn (see RenderTextMask), so characters Windows takes from
// another font (like Chinese, Japanese and Korean) are measured right too.
int TextWidth(HFONT font, const std::wstring& text) {
    {
        std::lock_guard<std::mutex> lock(g_textLock);
        ComPtr<IDWriteTextLayout> layout;
        DWRITE_TEXT_METRICS metrics = {};
        if (EnsureTextFactories() &&
            (layout = CreateTextLayout(font, text, 100000.0f, 1000.0f,
                                       DT_LEFT | DT_SINGLELINE)) &&
            SUCCEEDED(layout->GetMetrics(&metrics))) {
            return (int)std::ceil(metrics.widthIncludingTrailingWhitespace);
        }
    }
    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ oldFont = SelectObject(dc, font);
    RECT rect = {0, 0, 0, 0};
    DrawTextW(dc, text.c_str(), (int)text.size(), &rect,
              DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, oldFont);
    DeleteDC(dc);
    return rect.right - rect.left;
}

// Lays the panel out at `width`. Sets `neededWidth` to the width everything
// would need to fit without being cut short: in some languages the words
// are longer, so BuildPanelLayout widens the panel to match.
PanelLayout BuildPanelLayoutAt(int width, int* neededWidth) {
    PanelLayout layout;
    int padding = Px(16);
    int right = width - padding;
    int y = padding;
    *neededWidth = 0;
    auto need = [&](int contentWidth) {
        *neededWidth = std::max(*neededWidth, contentWidth + 2 * padding);
    };
    HFONT font = g_popup.labelFont;

    auto addText = [&](const std::wstring& text, RECT rect, PanelTextStyle style,
                       int settingId = -1) {
        layout.texts.push_back({text, rect, style, settingId});
    };

    // A row of choices that fill `area`, one option per {value, caption}.
    auto addChoices = [&](PanelAction action,
                          const std::vector<std::pair<int, std::wstring>>& options, RECT area) {
        layout.choiceBackgrounds.push_back(area);
        int count = (int)options.size();
        for (int i = 0; i < count; i++) {
            PanelControl control;
            control.action = action;
            control.value = options[i].first;
            control.caption = options[i].second;
            control.rect = {area.left + (area.right - area.left) * i / count, area.top,
                            area.left + (area.right - area.left) * (i + 1) / count, area.bottom};
            layout.controls.push_back(control);
        }
    };

    // The widest caption among some choices, plus room around it.
    auto choiceWidth = [&](const std::vector<std::pair<int, std::wstring>>& options,
                           int minimum) {
        int widest = Px(minimum);
        for (const auto& option : options) {
            widest = std::max(widest, TextWidth(font, option.second) + Px(16));
        }
        return widest;
    };

    // A setting's row: its name on the left, its choices on the right.
    auto addChoiceRow = [&](const std::wstring& name, PanelAction action,
                            const std::vector<std::pair<int, std::wstring>>& options,
                            int optionWidth) {
        int choicesWidth = choiceWidth(options, optionWidth) * (int)options.size();
        need(TextWidth(font, name) + Px(12) + choicesWidth);
        addText(name, {padding, y, right - choicesWidth - Px(8), y + Px(40)},
                PanelTextStyle::Row, SettingForAction(action));
        addChoices(action, options, {right - choicesWidth, y + Px(6), right, y + Px(34)});
        y += Px(40);
    };

    auto addToggleRow = [&](const std::wstring& name, PanelAction action) {
        need(TextWidth(font, name) + Px(56));
        addText(name, {padding, y, right - Px(48), y + Px(40)}, PanelTextStyle::Row,
                SettingForAction(action));
        PanelControl toggle;
        toggle.action = action;
        toggle.isToggle = true;
        toggle.rect = {right - Px(40), y + Px(10), right, y + Px(30)};
        layout.controls.push_back(toggle);
        y += Px(40);
    };

    // Other folders (not groups) share the all-groups settings, so with
    // those turned on, "All" covers them too.
    bool isGroup = g_popup.isGroup;
    std::wstring title = Tr(isGroup ? Text::GroupSettings : Text::FolderSettings);
    need(TextWidth(g_popup.titleFont, title));
    addText(title, {padding, y, right, y + Px(28)}, PanelTextStyle::Title);
    y += Px(36);

    std::vector<std::pair<int, std::wstring>> scopes = {
        {0, Tr(isGroup ? Text::ThisGroup : Text::ThisFolder)},
        {1, Tr(g_settings.openAnyFolder ? Text::All : Text::AllGroups)}};
    need(2 * choiceWidth(scopes, 0) + Px(16));
    addChoices(PanelAction::Scope, scopes, {padding, y, right, y + Px(32)});
    y += Px(48);

    // The list view has no rows of icons or names under them, so those
    // settings are only shown for the icon view.
    bool listView = PanelValue(PanelAction::View) == 1;
    addText(Tr(Text::Layout), {padding, y, right, y + Px(24)}, PanelTextStyle::Section);
    y += Px(24);
    addChoiceRow(Tr(Text::View), PanelAction::View,
                 {{0, Tr(Text::Icons)}, {1, Tr(Text::List)}}, 60);
    if (!listView) {
        addChoiceRow(Tr(isGroup ? Text::AppsPerRow : Text::ItemsPerRow), PanelAction::Columns,
                     {{3, L"3"}, {4, L"4"}, {5, L"5"}, {6, L"6"}}, 36);
    }
    addChoiceRow(Tr(Text::IconSize), PanelAction::IconSize,
                 {{32, Tr(Text::Small)}, {48, Tr(Text::Medium)}, {64, Tr(Text::Large)}}, 60);
    addChoiceRow(Tr(Text::Spacing), PanelAction::Spacing,
                 {{0, Tr(Text::Compact)}, {1, Tr(Text::Normal)}, {2, Tr(Text::Roomy)}}, 60);
    if (!listView) {
        addToggleRow(Tr(isGroup ? Text::ShowAppNames : Text::ShowNames), PanelAction::ShowNames);
    }

    // A row of colour swatches: None, the presets, and Custom.
    y += Px(8);
    addText(Tr(Text::Colour), {padding, y, right, y + Px(24)}, PanelTextStyle::Section,
            kSettingTint);
    y += Px(28);
    {
        std::vector<int> swatches = {kNoTint};
        for (COLORREF preset : kTintPresets) {
            swatches.push_back((int)preset);
        }
        swatches.push_back(kCustomTint);

        int count = (int)swatches.size();
        int size = Px(24);
        int spare = (right - padding) - count * size;  // Shared out as gaps.
        for (int i = 0; i < count; i++) {
            int left = padding + i * size + spare * i / (count - 1);
            PanelControl swatch;
            swatch.action = PanelAction::Tint;
            swatch.value = swatches[i];
            swatch.isSwatch = true;
            swatch.rect = {left, y, left + size, y + size};
            layout.controls.push_back(swatch);
        }
        y += Px(32);
    }

    y += Px(8);
    addText(Tr(Text::Behaviour), {padding, y, right, y + Px(24)}, PanelTextStyle::Section);
    y += Px(24);
    addChoiceRow(Tr(isGroup ? Text::SortApps : Text::SortItems), PanelAction::Sort,
                 {{0, Tr(Text::MyOrder)}, {1, Tr(Text::AToZ)}}, 72);
    if (isGroup) {
        addToggleRow(Tr(Text::RemoveWhenOneLeft), PanelAction::RemoveWhenOneLeft);
    }

    y += Px(12);
    if (g_panel.allGroups) {
        std::wstring note =
            Tr(g_settings.openAnyFolder ? Text::AppliesToGroupsAndFolders : Text::AppliesToGroups);
        need(TextWidth(font, note));
        addText(note, {padding, y, right, y + Px(32)}, PanelTextStyle::Note);
    } else {
        PanelControl reset;
        reset.action = PanelAction::Reset;
        reset.caption = Tr(isGroup ? Text::ResetToAllGroups : Text::ResetToShared);
        need(TextWidth(font, reset.caption) + Px(32));
        reset.rect = {padding, y, right, y + Px(32)};
        layout.controls.push_back(reset);
    }
    y += Px(32) + padding;

    layout.size = {width, y};
    return layout;
}

// The panel is 320 wide (at 100% scaling), or wider if its text needs it,
// up to 520.
PanelLayout BuildPanelLayout() {
    int needed = 0;
    PanelLayout layout = BuildPanelLayoutAt(Px(320), &needed);
    if (needed > Px(320)) {
        layout = BuildPanelLayoutAt(std::min(needed, Px(520)), &needed);
    }
    return layout;
}

// A Windows 11-style on/off switch: a pill with a round knob.
void DrawToggle(IconCanvas& canvas, const RECT& rect, bool on, bool hovered,
                const PopupColors& colors) {
    float left = (float)rect.left, top = (float)rect.top;
    float right = (float)rect.right, bottom = (float)rect.bottom;
    float radius = (bottom - top) / 2;

    if (on) {
        canvas.FillRoundedRect(left, top, right, bottom, radius, colors.accent, 1.0f);
    } else {
        canvas.FillRoundedRect(left, top, right, bottom, radius, colors.shade,
                               hovered ? colors.hoverOpacity : colors.pressedOpacity,
                               colors.dimText, 1.0f, (float)Px(1));
    }

    float knob = (bottom - top) - Px(on ? 8 : 10);
    float centerX = on ? right - radius : left + radius;
    float centerY = (top + bottom) / 2;
    canvas.FillRoundedRect(centerX - knob / 2, centerY - knob / 2, centerX + knob / 2,
                           centerY + knob / 2, knob / 2, on ? colors.onAccent : colors.dimText,
                           1.0f);
}

// A colour swatch: a filled circle, with a ring around the chosen one. "None"
// is an empty circle. "Custom" shows a plus sign, or the custom colour once
// one has been picked.
void DrawSwatch(IconCanvas& canvas, const PanelControl& control, bool hovered,
                const PopupColors& colors) {
    const RECT& r = control.rect;
    float centerX = (r.left + r.right) / 2.0f, centerY = (r.top + r.bottom) / 2.0f;
    float outer = (r.right - r.left) / 2.0f;
    float inner = outer - Px(4);

    int current = PanelValue(PanelAction::Tint);
    bool customChosen = current != kNoTint && !IsPresetTint(current);
    bool selected = control.value == kCustomTint ? customChosen : current == control.value;

    // The ring. (A rounded rectangle with no fill, just a border, and a
    // radius of half its width, is a circle outline.)
    if (selected || hovered) {
        canvas.FillRoundedRect(centerX - outer, centerY - outer, centerX + outer,
                               centerY + outer, outer, 0, 0,
                               selected ? colors.text : colors.dimText, 1.0f,
                               (float)Px(selected ? 2 : 1));
    }

    int fill = control.value;
    if (control.value == kCustomTint && customChosen) {
        fill = current;
    }
    if (fill >= 0) {
        canvas.FillRoundedRect(centerX - inner, centerY - inner, centerX + inner,
                               centerY + inner, inner, (COLORREF)fill, 1.0f);
    } else {
        canvas.FillRoundedRect(centerX - inner, centerY - inner, centerX + inner,
                               centerY + inner, inner, 0, 0, colors.dimText, 1.0f,
                               (float)Px(1));
        if (control.value == kCustomTint) {
            TextMask plus = RenderTextMask(g_popup.symbolFont, L"\xE710", r.right - r.left,
                                           r.bottom - r.top,
                                           DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            canvas.BlendMask(plus, r.left, r.top, colors.dimText);
        }
    }
}

void PaintPanel(HWND window, HDC screen) {
    RECT client;
    GetClientRect(window, &client);
    if (client.right <= 0 || client.bottom <= 0) {
        return;
    }
    PopupColors colors = GetPopupColors();
    PanelLayout layout = BuildPanelLayout();
    IconCanvas canvas(client.right, client.bottom);
    if (!g_panel.glass) {
        canvas.FillAll(colors.background);
    }

    // Text: the title, section names, row names, and the note.
    for (const PanelText& text : layout.texts) {
        HFONT font = text.style == PanelTextStyle::Title ? g_popup.titleFont : g_popup.labelFont;
        COLORREF color = (text.style == PanelTextStyle::Section ||
                          text.style == PanelTextStyle::Note)
                             ? colors.dimText
                             : colors.text;
        TextMask mask = RenderTextMask(font, text.text, text.rect.right - text.rect.left,
                                       text.rect.bottom - text.rect.top,
                                       DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                                           DT_NOPREFIX);
        canvas.BlendMask(mask, text.rect.left, text.rect.top, color);

        // A small dot beside settings this group has its own value for.
        int ownValue;
        if (!g_panel.allGroups && text.settingId >= 0 &&
            GetGroupOwnSetting(g_popup.folderPath, (GroupSettingId)text.settingId, &ownValue)) {
            float centerX = (float)text.rect.left - Px(8);
            float centerY = (text.rect.top + text.rect.bottom) / 2.0f;
            float dot = (float)Px(3);
            canvas.FillRoundedRect(centerX - dot, centerY - dot, centerX + dot, centerY + dot,
                                   dot, colors.accent, 1.0f);
        }
    }

    // A faint rounded background behind each set of choices.
    for (const RECT& area : layout.choiceBackgrounds) {
        canvas.FillRoundedRect((float)area.left, (float)area.top, (float)area.right,
                               (float)area.bottom, (float)Px(6), colors.shade,
                               colors.pressedOpacity);
    }

    for (int i = 0; i < (int)layout.controls.size(); i++) {
        const PanelControl& control = layout.controls[i];
        bool hovered = (i == g_panel.hovered);
        const RECT& r = control.rect;

        if (control.isToggle) {
            DrawToggle(canvas, r, PanelValue(control.action) != 0, hovered, colors);
            continue;
        }
        if (control.isSwatch) {
            DrawSwatch(canvas, control, hovered, colors);
            continue;
        }

        COLORREF textColor = colors.text;
        if (control.action == PanelAction::Reset) {
            // A text button. Faded when there's nothing to reset.
            bool enabled = GroupHasAnyOwnSettings();
            if (enabled && hovered) {
                canvas.FillRoundedRect((float)r.left, (float)r.top, (float)r.right,
                                       (float)r.bottom, (float)Px(6), colors.shade,
                                       colors.hoverOpacity);
            }
            textColor = enabled ? colors.accent : colors.dimText;
        } else if (PanelValue(control.action) == control.value) {
            // The chosen option: filled with the accent colour.
            canvas.FillRoundedRect((float)r.left + Px(2), (float)r.top + Px(2),
                                   (float)r.right - Px(2), (float)r.bottom - Px(2),
                                   (float)Px(4), colors.accent, 1.0f);
            textColor = colors.onAccent;
        } else if (hovered) {
            canvas.FillRoundedRect((float)r.left + Px(2), (float)r.top + Px(2),
                                   (float)r.right - Px(2), (float)r.bottom - Px(2),
                                   (float)Px(4), colors.shade, colors.hoverOpacity);
        }

        TextMask caption = RenderTextMask(g_popup.labelFont, control.caption,
                                          r.right - r.left, r.bottom - r.top,
                                          DT_CENTER | DT_VCENTER | DT_SINGLELINE |
                                              DT_END_ELLIPSIS | DT_NOPREFIX);
        canvas.BlendMask(caption, r.left, r.top, textColor);
    }

    if (g_popup.highContrast) {
        canvas.FillRoundedRect(0, 0, (float)client.right, (float)client.bottom, (float)Px(8), 0, 0,
                               colors.text, 1.0f, (float)Px(1));
    }

    // Copy to the screen, faded while it slides open.
    std::vector<Pixel> faded;
    const Pixel* finished = canvas.Pixels().data();
    if (g_panel.opacity < 1) {
        faded = ScaleAndFade(canvas, client.right, client.bottom, g_panel.opacity);
        finished = faded.data();
    }
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = client.right;
    info.bmiHeader.biHeight = -client.bottom;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP buffer = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!buffer || !bits) {
        return;
    }
    memcpy(bits, finished, (size_t)client.right * client.bottom * sizeof(Pixel));
    HDC dc = CreateCompatibleDC(screen);
    HGDIOBJ previousBitmap = SelectObject(dc, buffer);
    BitBlt(screen, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, previousBitmap);
    DeleteDC(dc);
    DeleteObject(buffer);
}

// Where the panel goes: beside the popup, on the side away from the group if
// there's room, otherwise the other side. Always kept on screen.
RECT ComputePanelRect(SIZE size) {
    const RECT& popup = g_popup.fullRect;
    const RECT& work = g_popup.workArea;
    int gap = Px(8);
    int margin = Px(Layout::screenMargin);

    bool popupIsRightOfGroup = (popup.left + popup.right) / 2 >=
                               (g_popup.itemRect.left + g_popup.itemRect.right) / 2;
    int rightSide = popup.right + gap;
    int leftSide = popup.left - gap - size.cx;
    int x;
    if (popupIsRightOfGroup) {
        x = (rightSide + size.cx <= work.right - margin) ? rightSide : leftSide;
    } else {
        x = (leftSide >= work.left + margin) ? leftSide : rightSide;
    }
    g_panel.slideDirection = (x >= popup.right) ? 1 : -1;

    x = std::max((int)(work.left + margin), std::min(x, (int)(work.right - size.cx - margin)));
    int y = std::max((int)(work.top + margin),
                     std::min((int)popup.top, (int)(work.bottom - size.cy - margin)));
    return {x, y, x + size.cx, y + size.cy};
}

void PositionSettingsPanel() {
    if (!g_panel.window) {
        return;
    }
    PanelLayout layout = BuildPanelLayout();
    g_panel.finalRect = ComputePanelRect(layout.size);
    SetWindowPos(g_panel.window, nullptr, g_panel.finalRect.left, g_panel.finalRect.top,
                 layout.size.cx, layout.size.cy, SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(g_panel.window, nullptr, FALSE);
}

// Changes a setting, for this group or for all groups (whichever the switch
// says), then updates the popup so you can see the result.
void ChangeGroupSetting(GroupSettingId id, int value) {
    if (g_panel.allGroups) {
        SetAllGroupsSetting(id, value);
        // "All groups" means every group (and other folder), so any group's
        // own value for this setting is removed. Sorting and colour change
        // how group icons look, so ask the group watcher to redraw those (it
        // skips folders that aren't groups).
        std::wstring desktopPath = GetDesktopFolderPath();
        for (const std::wstring& folder : FindAllFolders(desktopPath)) {
            std::wstring groupPath = desktopPath + L"\\" + folder;
            int ownValue;
            if (GetGroupOwnSetting(groupPath, id, &ownValue)) {
                ClearGroupOwnSetting(groupPath, id);
            }
            if (id == kSettingSort || id == kSettingTint) {
                RequestGroupCheck(groupPath);
            }
        }
        Wh_Log(L"Set %s to %d for all groups", kGroupSettings[id].key, value);
    } else {
        SetGroupOwnSetting(g_popup.folderPath, id, value);
        if (id == kSettingSort || id == kSettingTint) {
            RequestGroupCheck(g_popup.folderPath);
        }
        Wh_Log(L"Set %s to %d for \"%s\"", kGroupSettings[id].key, value,
               g_popup.title.c_str());
    }

    if (HWND popup = g_popupWindow) {
        ApplyPopupSettings(popup);
    }
}

// Opens Windows' standard colour picker. While it's open, the popup loses
// focus, which would normally make it close; menuOpen tells it to stay open,
// as for right-click menus. Returns false if the picker was cancelled.
bool PickCustomTint(int* tint) {
    // The picker's 16 "custom colours" boxes. "static" keeps them between
    // uses (until Explorer restarts).
    static COLORREF customColors[16] = {};

    HWND popup = g_popupWindow;
    int current = PanelValue(PanelAction::Tint);

    CHOOSECOLORW picker = {};
    picker.lStructSize = sizeof(picker);
    picker.hwndOwner = popup;
    picker.lpCustColors = customColors;
    picker.rgbResult = current >= 0 ? (COLORREF)current : RGB(0, 120, 212);
    picker.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;

    g_popup.menuOpen = true;
    BOOL chosen = ChooseColorW(&picker);
    g_popup.menuOpen = false;
    if (popup && IsWindow(popup)) {
        SetForegroundWindow(popup);
    }

    if (!chosen) {
        return false;
    }
    *tint = (int)(picker.rgbResult & 0x00FFFFFF);
    return true;
}

void ActivatePanelControl(const PanelControl& control) {
    switch (control.action) {
        case PanelAction::Scope:
            g_panel.allGroups = (control.value == 1);
            break;

        case PanelAction::Reset:
            if (!GroupHasAnyOwnSettings()) {
                break;
            }
            for (int id = 0; id < kGroupSettingCount; id++) {
                ClearGroupOwnSetting(g_popup.folderPath, (GroupSettingId)id);
            }
            Wh_Log(L"\"%s\" now uses the all-groups settings", g_popup.title.c_str());
            RequestGroupCheck(g_popup.folderPath);
            if (HWND popup = g_popupWindow) {
                ApplyPopupSettings(popup);
            }
            break;

        case PanelAction::Tint: {
            int tint = control.value;
            if (tint == kCustomTint && !PickCustomTint(&tint)) {
                break;
            }
            ChangeGroupSetting(kSettingTint, tint);
            break;
        }

        default: {
            int value = control.isToggle ? (PanelValue(control.action) ? 0 : 1) : control.value;
            ChangeGroupSetting(SettingForAction(control.action), value);
            break;
        }
    }
    if (g_panel.window) {
        InvalidateRect(g_panel.window, nullptr, FALSE);
    }
}

int PanelHitTest(POINT point) {
    PanelLayout layout = BuildPanelLayout();
    for (int i = 0; i < (int)layout.controls.size(); i++) {
        if (PtInRect(&layout.controls[i].rect, point)) {
            return i;
        }
    }
    return -1;
}

// The panel slides a little way out from the popup while fading in.
void StepPanelAnimation(HWND window) {
    float time = std::min(1.0f, (GetTickCount64() - g_panel.openedTime) /
                                    (float)kPanelOpenMilliseconds);
    float progress = 1 - std::pow(1 - time, 3.0f);
    int offset = (int)std::lround((1 - progress) * Px(12)) * -g_panel.slideDirection;
    g_panel.opacity = progress;
    SetWindowPos(window, nullptr, g_panel.finalRect.left + offset, g_panel.finalRect.top, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(window, nullptr, FALSE);
    if (time >= 1) {
        KillTimer(window, kPanelTimer);
        g_panel.opacity = 1;
    }
}

LRESULT CALLBACK PanelWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    BusyScope busy;  // Covers the colour picker.
    switch (message) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;  // Clicking doesn't take the focus.

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            PaintPanel(window, dc);
            EndPaint(window, &paint);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            if (!g_panel.trackingMouse) {
                TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, window, 0};
                TrackMouseEvent(&track);
                g_panel.trackingMouse = true;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int hovered = PanelHitTest(point);
            if (hovered != g_panel.hovered) {
                g_panel.hovered = hovered;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_panel.trackingMouse = false;
            g_panel.hovered = -1;
            InvalidateRect(window, nullptr, FALSE);
            return 0;

        case WM_LBUTTONDOWN: {
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            g_panel.pressed = PanelHitTest(point);
            if (g_panel.pressed >= 0) {
                SetCapture(window);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index = PanelHitTest(point);
            int pressed = g_panel.pressed;
            g_panel.pressed = -1;
            if (GetCapture() == window) {
                ReleaseCapture();
            }
            // Same rule as the popup: press and release on the same control.
            if (index >= 0 && index == pressed) {
                PanelLayout layout = BuildPanelLayout();
                ActivatePanelControl(layout.controls[index]);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            g_panel.pressed = -1;
            return 0;

        case WM_TIMER:
            if (wParam == kPanelTimer) {
                StepPanelAnimation(window);
            }
            return 0;

        case WM_NCDESTROY:
            g_panel.window = nullptr;
            g_panel.hovered = -1;
            g_panel.pressed = -1;
            g_panel.trackingMouse = false;
            if (HWND popup = g_popupWindow) {
                InvalidateRect(popup, nullptr, FALSE);  // Un-highlight the gear.
            }
            break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RegisterPanelClass() {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_DROPSHADOW;
    windowClass.lpfnWndProc = PanelWindowProc;
    windowClass.hInstance = g_module;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = g_panelClassName.c_str();
    return RegisterClassExW(&windowClass) != 0;
}

void OpenSettingsPanel(HWND popup) {
    if (g_panel.window || !g_windowClassesRegistered) {
        return;
    }
    g_panel.allGroups = false;
    g_panel.hovered = -1;
    g_panel.pressed = -1;

    PanelLayout layout = BuildPanelLayout();
    g_panel.finalRect = ComputePanelRect(layout.size);
    RECT start = g_panel.finalRect;

    // The popup is the "owner" (the parent argument, for a WS_POPUP window).
    HWND panel = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, g_panelClassName.c_str(),
                                 L"Group settings", WS_POPUP, start.left, start.top,
                                 layout.size.cx, layout.size.cy, popup, nullptr, g_module,
                                 nullptr);
    if (!panel) {
        Wh_Log(L"Couldn't open the settings panel (error %u)", (unsigned)GetLastError());
        return;
    }
    g_panel.window = panel;

    // Same styling as the popup: rounded corners, dark border, and blur.
    DWORD cornerPreference = 2;
    DwmSetWindowAttribute(panel, 33, &cornerPreference, sizeof(cornerPreference));
    BOOL darkMode = g_popup.dark;
    DwmSetWindowAttribute(panel, 20, &darkMode, sizeof(darkMode));
    g_panel.glass = g_popup.glass && EnableBlurBehind(panel);

    if (g_settings.animatePopup) {
        g_panel.opacity = 0;
        g_panel.openedTime = GetTickCount64();
        SetTimer(panel, kPanelTimer, 15, nullptr);
        StepPanelAnimation(panel);
    } else {
        g_panel.opacity = 1;
    }
    ShowWindow(panel, SW_SHOWNOACTIVATE);
    InvalidateRect(popup, nullptr, FALSE);  // Highlight the gear.
}

void CloseSettingsPanel() {
    if (HWND panel = g_panel.window) {
        DestroyWindow(panel);
    }
}

void ToggleSettingsPanel(HWND popup) {
    if (g_panel.window) {
        CloseSettingsPanel();
    } else {
        OpenSettingsPanel(popup);
    }
}

// Opens the popup for a group (or other folder), beside it on the desktop.
// `anchor` is the group's icon and `item` is the icon plus its label, both
// in screen coordinates. `byHover`: opened by hovering, so it doesn't take
// the focus, and closes when the mouse leaves (see CheckMouseLeft).
void ShowGroupPopup(const std::wstring& folderPath, const std::wstring& title,
                    const RECT& anchor, const RECT& item, bool isGroup, bool byHover) {
    if (HWND existing = g_popupWindow) {
        DestroyWindow(existing);
    }
    if (!g_windowClassesRegistered) {
        return;
    }

    // Which monitor is the group on, and what's its display scaling?
    POINT center = {(anchor.left + anchor.right) / 2, (anchor.top + anchor.bottom) / 2};
    HMONITOR monitor = MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    g_popup.dpi = dpiX;
    g_popup.dark = IsSystemDarkMode();
    g_popup.highContrast = IsHighContrast();
    g_popup.title = title;
    g_popup.folderPath = folderPath;
    g_popup.isGroup = isGroup;
    g_popup.hoverOpened = byHover;
    g_popup.closeOnLeave = byHover;
    g_popup.leftTime = 0;
    g_popup.scroll = 0;
    g_popup.scrollTarget = 0;
    g_popup.openedTime = GetTickCount64();
    g_popup.hovered = -1;
    g_popup.pressed = -1;
    g_popup.titleHovered = false;
    g_popup.titlePressed = false;
    g_popup.draggingOut = false;
    g_popup.reordering = false;
    g_popup.dragged = -1;
    g_popup.gearHovered = false;
    g_popup.gearPressed = false;
    ReadPopupLayoutSettings();
    LoadGroupItems(folderPath);
    CreatePopupFonts();

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    g_popup.workArea = monitorInfo.rcWork;  // PopupSize needs this.
    SIZE size = PopupSize();

    // Every app starts in its own slot, so there's nothing to animate yet.
    // (A WS_POPUP window has no border, so its inside is as wide as it is.)
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        RECT cell = PopupCellRect(i, size.cx);
        g_popup.items[i].x = (float)cell.left;
        g_popup.items[i].y = (float)cell.top;
    }
    g_popup.iconRect = anchor;
    g_popup.itemRect = item;

    g_popup.layoutSize = size;
    g_popup.fullRect = ComputePopupRect(size);
    g_popup.animation = 0;
    g_popup.contentOpacity = 1;

    // With the animation, the window starts out exactly over the group's
    // icon, and grows from there.
    RECT start = g_settings.animatePopup ? anchor : g_popup.fullRect;

    // WS_POPUP: no title bar or border. WS_EX_TOOLWINDOW: no taskbar button.
    HWND window = CreateWindowExW(
        WS_EX_TOOLWINDOW, g_popupClassName.c_str(), title.c_str(), WS_POPUP, start.left, start.top,
        std::max(1L, start.right - start.left), std::max(1L, start.bottom - start.top),
        nullptr, nullptr, g_module, nullptr);
    if (!window) {
        Wh_Log(L"Couldn't create the popup (error %u)", (unsigned)GetLastError());
        FreePopupResources();
        return;
    }
    g_popupWindow = window;

    // Windows 11 window styling: rounded corners, and a dark border in dark
    // mode. The numbers are DWMWA_WINDOW_CORNER_PREFERENCE (33) with
    // DWMWCP_ROUND (2), and DWMWA_USE_IMMERSIVE_DARK_MODE (20).
    DWORD cornerPreference = 2;
    DwmSetWindowAttribute(window, 33, &cornerPreference, sizeof(cornerPreference));
    BOOL darkMode = g_popup.dark;
    DwmSetWindowAttribute(window, 20, &darkMode, sizeof(darkMode));

    // The blurred, see-through background, like the Start menu (see
    // EnableBlurBehind above). If it can't be turned on, the popup is solid.
    g_popup.glass = g_settings.blurredPopup && !g_popup.highContrast && EnableBlurBehind(window);

    if (g_settings.animatePopup) {
        g_popup.contentOpacity = 0;  // Fully faded until the first step.
    }
    if (byHover) {
        // Shown on top, but without taking the focus from whatever has it.
        ShowWindow(window, SW_SHOWNOACTIVATE);
        SetWindowPos(window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        SetTimer(window, kLeaveTimer, 100, nullptr);
    } else {
        ShowWindow(window, SW_SHOW);
        SetForegroundWindow(window);
    }
    if (g_settings.animatePopup) {
        StartOpenClose(window, 1);
    }
}

// ---- 10. Opening groups ----------------------------------------------------
//
// The icon list tells its parent window, SHELLDLL_DefView, about clicks by
// sending it WM_NOTIFY messages:
//
//   NM_CLICK         - an icon was clicked (pressed and released without
//                      dragging, so clicks that start a drag don't count)
//   LVN_ITEMACTIVATE - an icon was "activated": double-clicked, or Enter
//                      pressed. DefView opens the item when it gets this.
//
// We "subclass" DefView: put our own function in front of its message handler.
// If a group was clicked (or activated), we show our popup and don't pass the
// message on, so DefView never opens the folder window. Everything else passes
// straight through.

// Click: a single click (NM_CLICK). Activate: a double-click or Enter.
// Hover: the mouse rested on the icon (with "Open groups with: hovering").
enum class OpenReason { Click, Activate, Hover };

bool TryOpenGroup(HWND list, int index, OpenReason reason) {
    bool isClick = reason == OpenReason::Click;
    if (index < 0) {
        // Keyboard activation may not say which icon; use the focused one.
        LRESULT focused = -1;
        SendToList(list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_FOCUSED, &focused);
        index = (int)focused;
    }
    if (index < 0) {
        return false;
    }
    if (reason != OpenReason::Hover && GetSelectedIcons(list).size() != 1) {
        return false;  // Several icons opened at once: leave it.
    }

    // Groups, and with "Open other desktop folders" turned on, any folder.
    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    DesktopItem item;
    if (!folderView || !GetDesktopItem(folderView.Get(), list, index, &item) ||
        !item.isDirectory) {
        return false;  // Not a folder: let Windows open it normally.
    }
    bool isGroup = IsGroupFolder(item.path);
    if (!isGroup && !g_settings.openAnyFolder) {
        return false;
    }

    ULONGLONG now = GetTickCount64();
    HWND openPopup = g_popupWindow;
    if (reason == OpenReason::Hover && openPopup && !g_popup.closeOnLeave) {
        return false;  // A popup you clicked open stays until you're done.
    }
    if (openPopup && g_popup.folderPath == item.path) {
        if (reason == OpenReason::Hover) {
            return true;  // Already showing.
        }
        if (isClick && g_popup.closeOnLeave) {
            // It opened by hovering; clicking the group makes it stay.
            g_popup.closeOnLeave = false;
            SetForegroundWindow(openPopup);
            return true;
        }
        // This group's popup is already open. If it only just opened, this is
        // the same click arriving a second time (Windows can send both
        // NM_CLICK and LVN_ITEMACTIVATE for one click), so do nothing.
        // Otherwise it's a fresh click on the group: close the popup, like
        // clicking a Start menu folder again.
        if (now - g_popup.openedTime >= GetDoubleClickTime() && isClick) {
            PostMessageW(openPopup, WM_CLOSE, 0, 0);  // Shrinks back into the icon.
        }
        return true;
    }
    if (reason != OpenReason::Activate && item.path == g_lastClosedGroup &&
        now - g_lastClosedTime < 300) {
        // Clicking the desktop closed this group's popup a moment ago, and
        // this is that same click. Leave it closed.
        return true;
    }

    // Where's the group on screen? The popup opens beside it, and grows out
    // of its icon. LVIR_ICON asks for just the icon; LVIR_BOUNDS for the icon
    // and its label together.
    RECT iconRect = {};
    iconRect.left = LVIR_ICON;
    SendToList(list, LVM_GETITEMRECT, index, (LPARAM)&iconRect);
    MapWindowPoints(list, nullptr, (POINT*)&iconRect, 2);

    RECT itemRect = {};
    itemRect.left = LVIR_BOUNDS;
    SendToList(list, LVM_GETITEMRECT, index, (LPARAM)&itemRect);
    MapWindowPoints(list, nullptr, (POINT*)&itemRect, 2);

    ShowGroupPopup(item.path, item.name, iconRect, itemRect, isGroup,
                   reason == OpenReason::Hover);
    return true;
}

// --- Opening by hovering -----------------------------------------------------
//
// With "Open groups with: hovering", we also watch the icon list itself
// (subclassing it, like DefView). Each time the mouse moves onto a different
// icon, a timer starts; if the mouse is still on that icon when it fires, the
// group opens. Nothing happens while a mouse button is held (you might be
// dragging), or for icons that aren't groups (or folders, with "Open other
// desktop folders" on).

constexpr UINT_PTR kHoverOpenTimer = 0x5DA7;

LRESULT CALLBACK DesktopListSubclassProc(HWND list, UINT message, WPARAM wParam,
                                         LPARAM lParam, DWORD_PTR) {
    BusyScope busy;
    if (message == WM_MOUSEMOVE && g_settings.openWithHover) {
        int index = -1;
        if (!(wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))) {
            LVHITTESTINFO hit = {};
            hit.pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            LRESULT result = SendMessageW(list, LVM_HITTEST, 0, (LPARAM)&hit);
            if (result >= 0 && (hit.flags & LVHT_ONITEM)) {
                index = (int)result;
            }
        }
        if (index != g_hoverIcon) {
            g_hoverIcon = index;
            KillTimer(list, kHoverOpenTimer);
            if (index >= 0) {
                SetTimer(list, kHoverOpenTimer, g_settings.openHoverDelayMs, nullptr);
            }
        }
    } else if (message == WM_TIMER && wParam == kHoverOpenTimer) {
        KillTimer(list, kHoverOpenTimer);
        int index = g_hoverIcon;
        g_hoverIcon = -1;  // So moving back onto it later starts again.

        // Still there, with no buttons held, and the list is what's under
        // the mouse (not some window in front of it)?
        POINT cursor;
        GetCursorPos(&cursor);
        bool buttonHeld = (GetKeyState(VK_LBUTTON) | GetKeyState(VK_RBUTTON) |
                           GetKeyState(VK_MBUTTON)) & 0x8000;
        if (g_settings.openWithHover && index >= 0 && !buttonHeld &&
            WindowFromPoint(cursor) == list && GetIconUnderCursor(list) == index) {
            TryOpenGroup(list, index, OpenReason::Hover);
            g_hoverIcon = index;  // Don't start again until it moves off.
        }
        return 0;
    }
    return DefSubclassProc(list, message, wParam, lParam);
}

LRESULT CALLBACK DefViewSubclassProc(HWND window, UINT message, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR) {
    BusyScope busy;
    // Placing icons (section 4): requests from other threads, and the timer.
    // Registered message numbers aren't fixed, so they can't go in a
    // "switch"; plain "if"s instead.
    if (message == g_msgGetItemPosition && message) {
        auto* request = (std::shared_ptr<PositionRequest>*)lParam;
        (*request)->found = GetViewItemPosition((*request)->path, &(*request)->position);
        delete request;  // Our reference; see AskDesktopForPosition.
        return 0;
    }
    if (message == g_msgPlaceItem && message) {
        auto* request = (Placement*)lParam;
        QueuePlacement(window, request->path, request->position);
        delete request;  // See AskDesktopToPlace.
        return 0;
    }
    if (message == g_msgStopPlacing && message) {
        g_placements.clear();
        KillTimer(window, kPlacementTimer);
        if (HWND list = g_desktopList) {
            KillTimer(list, kHoverOpenTimer);  // Same thread, so allowed here.
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == kPlacementTimer) {
        ProcessPlacements(window);
        return 0;
    }

    if (message == WM_NOTIFY) {
        NMHDR* header = (NMHDR*)lParam;
        HWND list = g_desktopList;
        if (header && header->hwndFrom == list) {
            NMITEMACTIVATE* details = (NMITEMACTIVATE*)lParam;

            if (header->code == NM_CLICK && g_settings.openWithSingleClick &&
                details->iItem >= 0) {
                // Ctrl/Shift/Alt+click are for selecting, so leave those alone.
                bool modifierHeld = details->uKeyFlags &
                                    (LVKF_CONTROL | LVKF_SHIFT | LVKF_ALT);
                if (!modifierHeld && TryOpenGroup(list, details->iItem, OpenReason::Click)) {
                    return 0;  // Handled.
                }
            }

            if (header->code == LVN_ITEMACTIVATE &&
                TryOpenGroup(list, details->iItem, OpenReason::Activate)) {
                return 0;  // Handled: don't let DefView open a folder window.
            }
        }
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

// ---- 11. Worker thread ------------------------------------------------------

// If Explorer stopped while a group was being created, CreateGroup's note of
// the folder is still in the mod's storage. If that folder was finished (it's
// marked as a group), it only needs unhiding. Otherwise it's tidied up like a
// failed group: removed if it's empty, or unhidden if shortcuts were already
// moved into it, so they're never left out of sight.
void RecoverUnfinishedGroup() {
    std::wstring pathText = LoadStoredText(L"UnfinishedGroup");
    if (pathText.empty()) {
        return;
    }
    DeleteStoredValue(L"UnfinishedGroup");
    PCWSTR path = pathText.c_str();

    DWORD attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return;
    }
    if (IsGroupFolder(path)) {
        RemoveAttributes(path, FILE_ATTRIBUTE_HIDDEN);
    } else {
        CleanUpFailedGroup(path);
    }
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, path, nullptr);
    Wh_Log(L"Recovered \"%s\", left unfinished when Explorer stopped", path);
}

// Forgets the stored data of groups whose folders no longer exist: deleted
// while the mod wasn't running, or emptied from the Recycle Bin. (A folder
// in the Recycle Bin still exists, so a group you restore from there is
// still a group.)
//
// Windows can open a file by its ID (OpenFileById), which tells us whether a
// folder with that ID still exists, wherever it is on the drive. To be safe,
// this first checks that it works for the Desktop folder itself, and only
// looks at groups on the same drive as the Desktop.
void ForgetDeletedGroups() {
    HANDLE desktop = OpenForInfo(GetDesktopFolderPath());
    if (desktop == INVALID_HANDLE_VALUE) {
        return;
    }
    std::wstring desktopId = FileIdText(desktop);

    // Turns the hexadecimal digits back into an ID for OpenFileById.
    // (The parts are always in the same places: "V" + 16 digits + "F" + 32.)
    const size_t kDriveLength = 17;
    auto descriptorFor = [&](const std::wstring& id, FILE_ID_DESCRIPTOR* descriptor) {
        size_t start = kDriveLength;
        if (id.size() != kDriveLength + 1 + 32 || id[start] != L'F') {
            return false;
        }
        *descriptor = {};
        descriptor->dwSize = sizeof(*descriptor);
        descriptor->Type = ExtendedFileIdType;
        for (int i = 0; i < 16; i++) {
            std::wstring pair = id.substr(start + 1 + i * 2, 2);
            descriptor->ExtendedFileId.Identifier[i] = (BYTE)wcstoul(pair.c_str(), nullptr, 16);
        }
        return true;
    };
    // 1 = exists, 0 = doesn't, -1 = couldn't tell.
    auto exists = [&](const std::wstring& id) {
        FILE_ID_DESCRIPTOR descriptor;
        if (!descriptorFor(id, &descriptor)) {
            return -1;
        }
        HANDLE file = OpenFileById(desktop, &descriptor, FILE_READ_ATTRIBUTES,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, FILE_FLAG_BACKUP_SEMANTICS);
        if (file != INVALID_HANDLE_VALUE) {
            CloseHandle(file);
            return 1;
        }
        DWORD error = GetLastError();
        return (error == ERROR_INVALID_PARAMETER || error == ERROR_FILE_NOT_FOUND) ? 0 : -1;
    };

    std::vector<std::wstring> gone;
    if (!desktopId.empty() && exists(desktopId) == 1) {
        std::wstring drive = desktopId.substr(0, kDriveLength);
        std::vector<std::wstring> ids;
        {
            std::lock_guard<std::mutex> lock(g_storageLock);
            for (const auto& group : g_groupData) {
                ids.push_back(group.first);
            }
        }
        for (const std::wstring& id : ids) {
            if (id.compare(0, drive.size(), drive) == 0 && exists(id) == 0) {
                gone.push_back(id);
            }
        }
    }
    CloseHandle(desktop);

    for (const std::wstring& id : gone) {
        ForgetStoredGroup(id);
    }
    if (!gone.empty()) {
        Wh_Log(L"Forgot the data of %d deleted group(s)", (int)gone.size());
    }
}

//
// When Windows starts, Windhawk loads the mod before the desktop exists, so we
// keep checking every half second for up to a minute.

HANDLE g_stopEvent = nullptr;     // Signalled when the mod is being unloaded.
HANDLE g_workerThread = nullptr;

DWORD WINAPI WorkerThread(LPVOID) {
    const int maxAttempts = 120;  // 120 x 500 ms = 1 minute
    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        // Only explain failures on the last attempt, so the log isn't flooded.
        bool lastAttempt = (attempt == maxAttempts);

        HWND list = nullptr;
        switch (FindDesktopListView(&list, lastAttempt)) {
            case SearchResult::Found: {
                // Only the explorer.exe that owns the desktop gets here, so
                // only one copy of the mod does this.
                RecoverUnfinishedGroup();
                ForgetDeletedGroups();
                g_desktopList = list;
                LogDesktopIcons(list);

                // Start watching DefView for double-clicks. The window belongs
                // to another thread, which is why we need Windhawk's
                // "FromAnyThread" version of SetWindowSubclass.
                HWND defView = GetParent(list);
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                        defView, DefViewSubclassProc, 0)) {
                    g_desktopDefView = defView;
                } else {
                    Wh_Log(L"Couldn't watch the desktop for double-clicks");
                }
                // And the icon list itself, for opening groups by hovering.
                if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
                        list, DesktopListSubclassProc, 0)) {
                    Wh_Log(L"Couldn't watch the desktop for hovering");
                }

                // Start keeping group icons up to date. Only the explorer.exe
                // that owns the desktop gets here, so only one copy runs.
                StartGroupWatcher();
                return 0;
            }
            case SearchResult::NotThisProcess:
                return 0;
            case SearchResult::NotYet:
                break;  // Keep trying.
        }

        // Wait 500 ms, but wake up immediately if the mod is being unloaded.
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    Wh_Log(L"Gave up looking for the desktop after 1 minute");
    return 0;
}

void StartWorker() {
    ResetEvent(g_stopEvent);
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
}

void StopWorker() {
    if (!g_workerThread) {
        return;
    }
    SetEvent(g_stopEvent);
    WaitForSingleObject(g_workerThread, INFINITE);
    CloseHandle(g_workerThread);
    g_workerThread = nullptr;
}

// ---- 12. Windhawk callbacks -------------------------------------------------

// Registers the popup's and settings panel's window classes, once per load.
// The names get a number that's different every time the mod loads, so a
// class left behind by an earlier load (whose message handler no longer
// exists) can never be picked up by mistake.
void RegisterWindowClasses() {
    std::wstring suffix = std::to_wstring(GetTickCount64());
    g_popupClassName = L"DesktopAppGroups_Popup_" + suffix;
    g_panelClassName = L"DesktopAppGroups_Settings_" + suffix;
    g_windowClassesRegistered = RegisterPopupClass() && RegisterPanelClass();
    if (!g_windowClassesRegistered) {
        Wh_Log(L"Couldn't register the popup's window classes (error %u)",
               (unsigned)GetLastError());
    }
}

// Called first. Returning FALSE tells Windhawk to unload the mod.
BOOL Wh_ModInit() {
    Wh_Log(L"Desktop App Groups: starting in explorer.exe process %u",
           (unsigned)GetCurrentProcessId());
    LoadSettings();
    g_userPrefix = CurrentUserPrefix();
    LoadAllGroupsSettings();
    LoadStoredGroups();
    RegisterPlacementMessages();

    // Remember which DLL we are, for registering the popup's window class.
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&Wh_ModInit, &g_module);
    RegisterWindowClasses();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    // Find DoDragDrop inside ole32.dll and ask Windhawk to hook it.
    HMODULE ole32 = LoadLibraryW(L"ole32.dll");
    void* doDragDrop = ole32 ? (void*)GetProcAddress(ole32, "DoDragDrop") : nullptr;
    if (!doDragDrop ||
        !Wh_SetFunctionHook(doDragDrop, (void*)DoDragDrop_Hook,
                            (void**)&DoDragDrop_Original)) {
        Wh_Log(L"Couldn't hook DoDragDrop");
        return FALSE;
    }

    return TRUE;
}

// Called once Windhawk has finished setting the mod up, including the hook.
// A safe place to start background work.
void Wh_ModAfterInit() {
    StartWorker();
}

// Called when the user changes a setting. Re-read settings and list the icons
// again so you can see the difference straight away.
void Wh_ModSettingsChanged() {
    bool wasInFolder = g_dataInFolder;
    StopWorker();
    LoadSettings();
    StartWorker();

    // If "Keep group data inside the group folders" changed, have the group
    // watcher move every group's data over now. (If the watcher isn't running
    // yet, it checks every group when it starts anyway.)
    if (g_dataInFolder != wasInFolder && g_watcherThread) {
        std::wstring desktopPath = GetDesktopFolderPath();
        for (const std::wstring& group : FindAllGroups(desktopPath)) {
            RequestGroupCheck(desktopPath + L"\\" + group);
        }
    }
}

// Called when the mod is disabled or Explorer exits. Everything the mod set up
// must be undone here, because the mod's code is about to be removed from
// Explorer. (Windhawk removes the DoDragDrop hook for us.)
void Wh_ModUninit() {
    Wh_Log(L"Desktop App Groups: stopping");
    StopWorker();

    // Stop watching the desktop. First clear the placing to-do list and its
    // timer: timers can only be stopped from the window's own thread, which
    // SendMessage runs this on.
    if (HWND defView = g_desktopDefView) {
        SendMessageW(defView, g_msgStopPlacing, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(defView, DefViewSubclassProc);
        g_desktopDefView = nullptr;
    }
    if (HWND list = g_desktopList) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(list, DesktopListSubclassProc);
    }

    // Close the popup if it's open. SendMessage runs this on the popup's own
    // thread, which is required for destroying a window. Closing it also ends
    // its right-click menu or colour picker, if one is open.
    if (HWND popup = g_popupWindow) {
        SendMessageW(popup, kMsgCloseNow, 0, 0);
    }

    // Wait for any of the mod's code that's still running to finish (see
    // "Staying safe while unloading" near the top). Usually there's none; if
    // you're mid-drag or answering a prompt, this waits until you're done.
    while (g_busyCount > 0) {
        Sleep(100);
    }

    if (g_windowClassesRegistered) {
        UnregisterClassW(g_popupClassName.c_str(), g_module);
        UnregisterClassW(g_panelClassName.c_str(), g_module);
        g_windowClassesRegistered = false;
    }

    // Stop the watcher last, since the popup can send it requests.
    StopGroupWatcher();

    // Nothing's drawing text any more.
    ReleaseTextFactories();

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}
