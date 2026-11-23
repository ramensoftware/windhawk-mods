// ==WindhawkMod==
// @id              quick-link-menu
// @name            Quick Link Menu
// @description     Customize the Win+X menu and add more commands (right-click the Start button)
// @description:ru-RU Кастомизация и расширение возможностей меню Win+X (правый клик по кнопке «Пуск»)
// @description:es-ES Personaliza el menú Win+X y añade más comandos (clic derecho en el botón Inicio)
// @description:de-DE Das Win+X-Menü anpassen und um weitere Befehle ergänzen (Rechtsklick auf die Startschaltfläche)
// @description:pt-BR Personalize o menu Win+X e adicione mais comandos (clique com o botão direito no botão Iniciar)
// @description:zh-CN 自定义 Win+X 菜单并添加更多命令（右键单击“开始”按钮）
// @version         1.1.1
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lcomctl32 -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -ladvapi32 -liphlpapi -luuid -ldwrite
// ==/WindhawkMod==
// Build: 43

// ==WindhawkModReadme==
/*
# Quick Link Menu

**English** · [Русский](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Quick_Link_Menu/README.ru.md)

Quick Link Menu customizes the native Windows 11 menu opened by **Win+X** or by
right-clicking the Start button.

## Features

![Windows 11 Win+X and three Quick Link Menu configurations showing the System, Shut down, and Event Viewer submenus](https://raw.githubusercontent.com/CTravkin/DSN_Windhawk_Mods/3b4e177fc36ebff3d3904821c6b85d9e9032583d/Quick_Link_Menu/Assets/Previews/Comparison.png)

### Customization and command availability

- **Menu contents.** Hide individual rows, submenu commands, or entire sections.
  Turning off a section preserves its settings. Turning off a row hides its submenu.
- **Automatic filtering.** The mod hides unavailable commands based on the Windows
  edition, installed components, hardware, and system policies. If a main action is
  unavailable, its available shortcuts remain. An empty menu falls back to standard Win+X.
- **User permissions.** An optional filter hides administrator commands that require
  credentials or whose elevation is blocked. Commands that only need UAC consent stay visible.
- **Submenu controls.** Open submenus on hover, after a delay, on arrow hover, or by
  clicking the arrow. A hand cursor can mark areas that run a command.
- **Terminal.** The main Terminal action can run as administrator.

### Menu sections

- **Settings.** Open Windows Settings categories and pages such as Display,
  Sound, VPN, Default apps, and Windows Update.
- **Control Panel.** Open classic tools such as Device Manager, Network
  Connections, Power Options, and Programs and Features.
- **Administration.** Open Computer Management, Services, Event Viewer,
  Registry Editor, Terminal, and related tools such as Task Scheduler and
  Command Prompt.
- **Basic commands.** Open Run, Task Manager, Search, and File Explorer, show
  the desktop, or access power and sign-in options. File Explorer opens its
  configured start page, with a submenu for locations such as This PC,
  Downloads, and the Recycle Bin.

### Quick access to power actions

The main power row shuts down Windows in one click by default. It can run another
action, such as Restart or Lock. Choose **None** to open the power submenu without
running a command. When Windows offers **Update and shut down** or **Update and
restart** in Win+X, the corresponding Shut down or Restart switch also controls
that entry.

### Labels and icons

The menu uses Windows labels in the system display language and Segoe Fluent Icons.
Settings shortcuts use the matching Windows icons where available.

Windhawk settings are available in English, Russian, Spanish, German, Brazilian
Portuguese, and Simplified Chinese.

## Platforms

Requires Windows 11 x64 or ARM64. GitHub Actions verified native ARM64
compilation and automated command availability tests. Win+X interaction was
tested on x64.

### Compatibility

Quick Link Menu works with the standard Windows 11 Win+X menu. Mods that replace
it are incompatible. One example is [Windows 10 Win+X restorer on Win11 24H2](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-10-winx-restorer.wh.cpp).

## Usage

Click a row's label or press **Enter** to run its main action. Click the arrow
or press **→** to open the submenu. Click the label if a row only has a submenu.

To change the menu, open **Quick Link Menu → Settings** in Windhawk. Section
switches are below their descriptions. Row and submenu switches follow the menu
order. Find submenu behavior and the administrator filter under **General settings**,
and Terminal and power actions under **Command settings**.

## Links

- [Quick Link Menu documentation](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Quick_Link_Menu/README.md)
- [DSN Windhawk Mods](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/README.md)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- general:
  - submenuHover: immediate
    $name: "Open submenus"
    $name:ru-RU: "Раскрытие подменю"
    $name:es-ES: "Apertura de submenús"
    $name:de-DE: "Untermenüs öffnen"
    $name:pt-BR: "Abrir submenus"
    $name:zh-CN: "打开子菜单"
    $description: "Controls how to open additional commands from the main menu"
    $description:ru-RU: "Определяет, как открывается список дополнительных команд из основного меню"
    $description:es-ES: "Determina cómo se abre la lista de comandos adicionales desde el menú principal"
    $description:de-DE: "Legt fest, wie sich die Liste zusätzlicher Befehle aus dem Hauptmenü öffnen lässt"
    $description:pt-BR: "Define como a lista de comandos adicionais é aberta a partir do menu principal"
    $description:zh-CN: "设置如何从主菜单打开附加命令列表"
    $options:
      - immediate: "Immediately"
      - delayed: "With a delay"
      - arrow: "On arrow hover"
      - click: "On arrow click"
    $options:ru-RU:
      - immediate: "Сразу"
      - delayed: "С задержкой"
      - arrow: "При наведении на стрелку"
      - click: "При нажатии на стрелку"
    $options:es-ES:
      - immediate: "Inmediatamente"
      - delayed: "Con retardo"
      - arrow: "Al pasar el puntero sobre la flecha"
      - click: "Al hacer clic en la flecha"
    $options:de-DE:
      - immediate: "Sofort"
      - delayed: "Mit Verzögerung"
      - arrow: "Beim Darüberfahren über den Pfeil"
      - click: "Beim Klicken auf den Pfeil"
    $options:pt-BR:
      - immediate: "Imediatamente"
      - delayed: "Com atraso"
      - arrow: "Ao passar o ponteiro sobre a seta"
      - click: "Ao clicar na seta"
    $options:zh-CN:
      - immediate: "立即打开"
      - delayed: "延迟打开"
      - arrow: "悬停在箭头上时打开"
      - click: "单击箭头时打开"
  - hideAdminCommands: false
    $name: "Hide administrator commands"
    $name:ru-RU: "Скрытие команд администратора"
    $name:es-ES: "Ocultar comandos de administrador"
    $name:de-DE: "Administratorbefehle ausblenden"
    $name:pt-BR: "Ocultar comandos de administrador"
    $name:zh-CN: "隐藏管理员命令"
    $description: "A command is hidden if the current user lacks permission to run it or if elevation is blocked"
    $description:ru-RU: "Если выполнение команды запрещено текущему пользователю или повышение прав запрещено, она скрывается"
    $description:es-ES: "Se oculta el comando si el usuario actual no tiene permiso para ejecutarlo o si se bloquea la elevación de privilegios"
    $description:de-DE: "Ein Befehl wird ausgeblendet, wenn dem aktuellen Benutzer die Berechtigung zur Ausführung fehlt oder die Rechteerhöhung gesperrt ist"
    $description:pt-BR: "O comando é ocultado se o usuário atual não tiver permissão para executá-lo ou se a elevação de privilégios estiver bloqueada"
    $description:zh-CN: "如果当前用户无权执行某条命令，或系统禁止提升权限，该命令将被隐藏"
  - showHandCursor: true
    $name: "Command indication"
    $name:ru-RU: "Индикация активных команд"
    $name:es-ES: "Indicación de comandos activos"
    $name:de-DE: "Aktive Befehle kennzeichnen"
    $name:pt-BR: "Indicação de comandos ativos"
    $name:zh-CN: "指示可执行命令"
    $description: "The pointer becomes a hand over an area where clicking runs a command"
    $description:ru-RU: "При наведении на область, нажатие в которой выполняет команду, указатель принимает форму руки"
    $description:es-ES: "El puntero toma la forma de una mano al pasar por un área donde un clic ejecuta un comando"
    $description:de-DE: "Der Mauszeiger wird zur Hand, sobald er sich über einem Bereich befindet, in dem ein Klick einen Befehl ausführt"
    $description:pt-BR: "O ponteiro assume a forma de uma mão ao passar sobre uma área em que um clique executa um comando"
    $description:zh-CN: "将指针移到单击即可执行命令的区域时，指针会变成手形"
  $name: "General settings"
  $name:ru-RU: "Общие настройки"
  $name:es-ES: "Configuración general"
  $name:de-DE: "Allgemeine Einstellungen"
  $name:pt-BR: "Configurações gerais"
  $name:zh-CN: "常规设置"
  $description: "Global settings for the menu and its commands"
  $description:ru-RU: "Глобальные параметры меню и команд"
  $description:es-ES: "Opciones globales del menú y sus comandos"
  $description:de-DE: "Globale Einstellungen für das Menü und seine Befehle"
  $description:pt-BR: "Configurações globais do menu e de seus comandos"
  $description:zh-CN: "菜单及其命令的全局设置"
- commandOptions:
  - terminalAsAdmin: false
    $name: "Terminal as administrator"
    $name:ru-RU: "Терминал от имени администратора"
    $name:es-ES: "Terminal como administrador"
    $name:de-DE: "Terminal als Administrator"
    $name:pt-BR: "Terminal como administrador"
    $name:zh-CN: "以管理员身份运行终端"
    $description: "Opening Terminal from the main menu runs it as administrator"
    $description:ru-RU: "Открытие терминала из основного меню запускает его от имени администратора"
    $description:es-ES: "Al abrir Terminal desde el menú principal, se ejecuta como administrador"
    $description:de-DE: "Beim Öffnen über das Hauptmenü wird das Terminal als Administrator gestartet"
    $description:pt-BR: "Ao abrir o Terminal pelo menu principal, ele é executado como administrador"
    $description:zh-CN: "从主菜单打开终端时，将以管理员身份运行"
  - primaryPowerAction: shutdown
    $name: "Power action"
    $name:ru-RU: "Завершение работы"
    $name:es-ES: "Acción de apagado"
    $name:de-DE: "Aktion beim Herunterfahren"
    $name:pt-BR: "Ação de energia"
    $name:zh-CN: "电源操作"
    $description: "Clicking the power menu item runs the selected action"
    $description:ru-RU: "Нажатие на список способов завершения работы выполняет выбранное действие"
    $description:es-ES: "Al hacer clic en la opción de apagado del menú, se ejecuta la acción seleccionada"
    $description:de-DE: "Ein Klick auf den Eintrag für Energieoptionen führt die ausgewählte Aktion aus"
    $description:pt-BR: "Um clique no item de energia do menu executa a ação selecionada"
    $description:zh-CN: "单击电源菜单项即可执行所选操作"
    $options:
      - none: "None"
      - lock: "Lock"
      - signOut: "Sign out"
      - sleep: "Sleep"
      - hibernate: "Hibernate"
      - shutdown: "Shut down"
      - restart: "Restart"
    $options:ru-RU:
      - none: "Нет"
      - lock: "Заблокировать"
      - signOut: "Выход"
      - sleep: "Спящий режим"
      - hibernate: "Гибернация"
      - shutdown: "Завершение работы"
      - restart: "Перезагрузка"
    $options:es-ES:
      - none: "Ninguna"
      - lock: "Bloquear"
      - signOut: "Cerrar sesión"
      - sleep: "Suspender"
      - hibernate: "Hibernar"
      - shutdown: "Apagar"
      - restart: "Reiniciar"
    $options:de-DE:
      - none: "Keine"
      - lock: "Sperren"
      - signOut: "Abmelden"
      - sleep: "Energie sparen"
      - hibernate: "Ruhezustand"
      - shutdown: "Herunterfahren"
      - restart: "Neu starten"
    $options:pt-BR:
      - none: "Nenhuma"
      - lock: "Bloquear"
      - signOut: "Sair"
      - sleep: "Suspender"
      - hibernate: "Hibernar"
      - shutdown: "Desligar"
      - restart: "Reiniciar"
    $options:zh-CN:
      - none: "无"
      - lock: "锁定"
      - signOut: "注销"
      - sleep: "睡眠"
      - hibernate: "休眠"
      - shutdown: "关机"
      - restart: "重启"
  $name: "Command settings"
  $name:ru-RU: "Настройки команд"
  $name:es-ES: "Configuración de comandos"
  $name:de-DE: "Befehlseinstellungen"
  $name:pt-BR: "Configurações de comandos"
  $name:zh-CN: "命令设置"
  $description: "Additional settings for individual commands"
  $description:ru-RU: "Дополнительные параметры конкретных команд"
  $description:es-ES: "Opciones adicionales para comandos específicos"
  $description:de-DE: "Zusätzliche Einstellungen für einzelne Befehle"
  $description:pt-BR: "Configurações adicionais para comandos específicos"
  $description:zh-CN: "特定命令的附加设置"
- windowsSettingsEnabled: true
  $name: "Settings"
  $name:ru-RU: "Параметры"
  $name:es-ES: "Configuración"
  $name:de-DE: "Einstellungen"
  $name:pt-BR: "Configurações"
  $name:zh-CN: "设置"
  $description: "Windows Settings categories and pages"
  $description:ru-RU: "Разделы и страницы стандартного приложения «Параметры»"
  $description:es-ES: "Categorías y páginas de Configuración de Windows"
  $description:de-DE: "Kategorien und Seiten der Windows-Einstellungen"
  $description:pt-BR: "Categorias e páginas das Configurações do Windows"
  $description:zh-CN: "Windows 设置的类别和页面"
- windowsSettings:
  - systemVisible: true
    $name: "System"
    $name:ru-RU: "Система"
    $name:es-ES: "Sistema"
    $name:de-DE: "System"
    $name:pt-BR: "Sistema"
    $name:zh-CN: "系统"
  - system:
    - display: true
      $name: "Display"
      $name:ru-RU: "Дисплей"
      $name:es-ES: "Pantalla"
      $name:de-DE: "Bildschirm"
      $name:pt-BR: "Tela"
      $name:zh-CN: "屏幕"
    - sound: true
      $name: "Sound"
      $name:ru-RU: "Звук"
      $name:es-ES: "Sonido"
      $name:de-DE: "Sound"
      $name:pt-BR: "Som"
      $name:zh-CN: "声音"
    - power: false
      $name: "Power"
      $name:ru-RU: "Питание"
      $name:es-ES: "Inicio/Apagado"
      $name:de-DE: "Leistung"
      $name:pt-BR: "Desligar"
      $name:zh-CN: "电源"
    - storage: true
      $name: "Storage"
      $name:ru-RU: "Память"
      $name:es-ES: "Almacenamiento"
      $name:de-DE: "Speicher"
      $name:pt-BR: "Armazenamento"
      $name:zh-CN: "存储"
    - advanced: false
      $name: "Advanced"
      $name:ru-RU: "Дополнительно"
      $name:es-ES: "Avanzado"
      $name:de-DE: "Erweitert"
      $name:pt-BR: "Avançado"
      $name:zh-CN: "高级"
    - about: true
      $name: "About"
      $name:ru-RU: "О системе"
      $name:es-ES: "Información"
      $name:de-DE: "Info"
      $name:pt-BR: "Sobre"
      $name:zh-CN: "系统信息"
    $name: " "
  - devicesVisible: true
    $name: "Bluetooth & devices"
    $name:ru-RU: "Bluetooth и устройства"
    $name:es-ES: "Bluetooth y dispositivos"
    $name:de-DE: "Bluetooth und Geräte"
    $name:pt-BR: "Bluetooth e dispositivos"
    $name:zh-CN: "蓝牙和其他设备"
  - devices:
    - devices: true
      $name: "Devices"
      $name:ru-RU: "Устройства"
      $name:es-ES: "Dispositivos"
      $name:de-DE: "Geräte"
      $name:pt-BR: "Dispositivos"
      $name:zh-CN: "设备"
    - printers: false
      $name: "Printers & scanners"
      $name:ru-RU: "Принтеры и сканеры"
      $name:es-ES: "Impresoras y escáneres"
      $name:de-DE: "Drucker und Scanner"
      $name:pt-BR: "Impressoras e scanners"
      $name:zh-CN: "打印机和扫描仪"
    - cameras: false
      $name: "Cameras"
      $name:ru-RU: "Камеры"
      $name:es-ES: "Cámaras"
      $name:de-DE: "Kameras"
      $name:pt-BR: "Câmeras"
      $name:zh-CN: "摄像头"
    - mouse: false
      $name: "Mouse"
      $name:ru-RU: "Мышь"
      $name:es-ES: "Mouse"
      $name:de-DE: "Maus"
      $name:pt-BR: "Mouse"
      $name:zh-CN: "鼠标"
    - keyboard: false
      $name: "Keyboard"
      $name:ru-RU: "Клавиатура"
      $name:es-ES: "Teclado"
      $name:de-DE: "Tastatur"
      $name:pt-BR: "Teclado"
      $name:zh-CN: "键盘"
    $name: " "
  - networkVisible: true
    $name: "Network & internet"
    $name:ru-RU: "Сеть и Интернет"
    $name:es-ES: "Red e Internet"
    $name:de-DE: "Netzwerk und Internet"
    $name:pt-BR: "Rede e Internet"
    $name:zh-CN: "网络和 Internet"
  - network:
    - ethernet: true
      $name: "Ethernet"
      $name:ru-RU: "Ethernet"
      $name:es-ES: "Ethernet"
      $name:de-DE: "Ethernet"
      $name:pt-BR: "Ethernet"
      $name:zh-CN: "以太网"
    - wifi: true
      $name: "Wi-Fi"
      $name:ru-RU: "Wi-Fi"
      $name:es-ES: "Wi-Fi"
      $name:de-DE: "WLAN"
      $name:pt-BR: "Wi-Fi"
      $name:zh-CN: "WLAN"
    - hotspot: true
      $name: "Mobile hotspot"
      $name:ru-RU: "Мобильный хот-спот"
      $name:es-ES: "Zona con cobertura inalámbrica móvil"
      $name:de-DE: "Mobiler Hotspot"
      $name:pt-BR: "Hotspot móvel"
      $name:zh-CN: "移动热点"
    - vpn: false
      $name: "VPN"
      $name:ru-RU: "VPN"
      $name:es-ES: "VPN"
      $name:de-DE: "VPN"
      $name:pt-BR: "VPN"
      $name:zh-CN: "VPN"
    - proxy: true
      $name: "Proxy"
      $name:ru-RU: "Прокси-сервер"
      $name:es-ES: "Proxy"
      $name:de-DE: "Proxy"
      $name:pt-BR: "Proxy"
      $name:zh-CN: "代理"
    - advancedNetwork: false
      $name: "Advanced network settings"
      $name:ru-RU: "Дополнительные сетевые параметры"
      $name:es-ES: "Configuración de red avanzada"
      $name:de-DE: "Erweiterte Netzwerkeinstellungen"
      $name:pt-BR: "Configurações avançadas de rede"
      $name:zh-CN: "高级网络设置"
    $name: " "
  - personalizationVisible: false
    $name: "Personalization"
    $name:ru-RU: "Персонализация"
    $name:es-ES: "Personalización"
    $name:de-DE: "Personalisierung"
    $name:pt-BR: "Personalização"
    $name:zh-CN: "个性化"
  - personalization:
    - background: true
      $name: "Background"
      $name:ru-RU: "Фон"
      $name:es-ES: "Fondo"
      $name:de-DE: "Hintergrund"
      $name:pt-BR: "Tela de fundo"
      $name:zh-CN: "背景"
    - colors: true
      $name: "Colors"
      $name:ru-RU: "Цвета"
      $name:es-ES: "Colores"
      $name:de-DE: "Farben"
      $name:pt-BR: "Cores"
      $name:zh-CN: "颜色"
    - themes: false
      $name: "Themes"
      $name:ru-RU: "Темы"
      $name:es-ES: "Temas"
      $name:de-DE: "Designs"
      $name:pt-BR: "Temas"
      $name:zh-CN: "主题"
    - lockScreen: true
      $name: "Lock screen"
      $name:ru-RU: "Экран блокировки"
      $name:es-ES: "Pantalla de bloqueo"
      $name:de-DE: "Sperrbildschirm"
      $name:pt-BR: "Tela de bloqueio"
      $name:zh-CN: "锁屏界面"
    - start: true
      $name: "Start"
      $name:ru-RU: "Пуск"
      $name:es-ES: "Inicio"
      $name:de-DE: "Start"
      $name:pt-BR: "Iniciar"
      $name:zh-CN: "开始"
    - taskbar: true
      $name: "Taskbar"
      $name:ru-RU: "Панель задач"
      $name:es-ES: "Barra de tareas"
      $name:de-DE: "Taskleiste"
      $name:pt-BR: "Barra de Tarefas"
      $name:zh-CN: "任务栏"
    $name: " "
  - appsVisible: true
    $name: "Apps"
    $name:ru-RU: "Приложения"
    $name:es-ES: "Aplicaciones"
    $name:de-DE: "Apps"
    $name:pt-BR: "Aplicativos"
    $name:zh-CN: "应用"
  - apps:
    - installed: true
      $name: "Installed apps"
      $name:ru-RU: "Установленные приложения"
      $name:es-ES: "Aplicaciones instaladas"
      $name:de-DE: "Installierte Apps"
      $name:pt-BR: "Aplicativos instalados"
      $name:zh-CN: "安装的应用"
    - defaults: true
      $name: "Default apps"
      $name:ru-RU: "Приложения по умолчанию"
      $name:es-ES: "Aplicaciones predeterminadas"
      $name:de-DE: "Standard-Apps"
      $name:pt-BR: "Aplicativos padrão"
      $name:zh-CN: "默认应用"
    - startup: true
      $name: "Startup"
      $name:ru-RU: "Автозагрузка"
      $name:es-ES: "Inicio"
      $name:de-DE: "Autostart"
      $name:pt-BR: "Inicialização"
      $name:zh-CN: "启动"
    $name: " "
  - accountsVisible: false
    $name: "Accounts"
    $name:ru-RU: "Учетные записи"
    $name:es-ES: "Cuentas"
    $name:de-DE: "Konten"
    $name:pt-BR: "Contas"
    $name:zh-CN: "帐户"
  - accounts:
    - signIn: true
      $name: "Sign-in options"
      $name:ru-RU: "Варианты входа"
      $name:es-ES: "Opciones de inicio de sesión"
      $name:de-DE: "Anmeldeoptionen"
      $name:pt-BR: "Opções de entrada"
      $name:zh-CN: "登录选项"
    - backup: false
      $name: "Windows Backup"
      $name:ru-RU: "Архивация Windows"
      $name:es-ES: "Copias de seguridad de Windows"
      $name:de-DE: "Windows-Sicherung"
      $name:pt-BR: "Backup do Windows"
      $name:zh-CN: "Windows 备份"
    - passkeys: true
      $name: "Passkeys"
      $name:ru-RU: "Ключи доступа"
      $name:es-ES: "Claves de acceso"
      $name:de-DE: "Hauptschlüssel"
      $name:pt-BR: "Chaves de acesso"
      $name:zh-CN: "通行密钥"
    $name: " "
  - timeLanguageVisible: false
    $name: "Time & language"
    $name:ru-RU: "Время и язык"
    $name:es-ES: "Hora e idioma"
    $name:de-DE: "Zeit und Sprache"
    $name:pt-BR: "Hora e idioma"
    $name:zh-CN: "时间和语言"
  - timeLanguage:
    - dateTime: true
      $name: "Date & time"
      $name:ru-RU: "Дата и время"
      $name:es-ES: "Fecha y hora"
      $name:de-DE: "Datum und Zeit"
      $name:pt-BR: "Data e hora"
      $name:zh-CN: "日期和时间"
    - language: true
      $name: "Language & region"
      $name:ru-RU: "Язык и регион"
      $name:es-ES: "Idioma y región"
      $name:de-DE: "Sprache und Region"
      $name:pt-BR: "Idioma e região"
      $name:zh-CN: "语言和区域"
    - typing: false
      $name: "Typing"
      $name:ru-RU: "Ввод"
      $name:es-ES: "Escritura"
      $name:de-DE: "Eingabe"
      $name:pt-BR: "Digitação"
      $name:zh-CN: "输入"
    $name: " "
  - gaming: false
    $name: "Gaming"
    $name:ru-RU: "Игры"
    $name:es-ES: "Juegos"
    $name:de-DE: "Spielen"
    $name:pt-BR: "Jogos"
    $name:zh-CN: "游戏"
  - accessibility: true
    $name: "Accessibility"
    $name:ru-RU: "Специальные возможности"
    $name:es-ES: "Accesibilidad"
    $name:de-DE: "Barrierefreiheit"
    $name:pt-BR: "Acessibilidade"
    $name:zh-CN: "辅助功能"
  - privacy: false
    $name: "Privacy & security"
    $name:ru-RU: "Конфиденциальность и защита"
    $name:es-ES: "Privacidad y seguridad"
    $name:de-DE: "Datenschutz und Sicherheit"
    $name:pt-BR: "Privacidade e segurança"
    $name:zh-CN: "隐私和安全性"
  - update: true
    $name: "Windows Update"
    $name:ru-RU: "Центр обновления Windows"
    $name:es-ES: "Windows Update"
    $name:de-DE: "Windows Update"
    $name:pt-BR: "Windows Update"
    $name:zh-CN: "Windows 更新"
  $name: " "
- controlPanelEnabled: true
  $name: "Control Panel"
  $name:ru-RU: "Панель управления"
  $name:es-ES: "Panel de control"
  $name:de-DE: "Systemsteuerung"
  $name:pt-BR: "Painel de Controle"
  $name:zh-CN: "控制面板"
  $description: "Classic Control Panel categories and windows"
  $description:ru-RU: "Категории и окна классической панели управления"
  $description:es-ES: "Categorías y ventanas clásicas del Panel de control"
  $description:de-DE: "Kategorien und klassische Fenster der Systemsteuerung"
  $description:pt-BR: "Categorias e janelas clássicas do Painel de Controle"
  $description:zh-CN: "控制面板类别和经典窗口"
- controlPanel:
  - securityVisible: true
    $name: "System and Security"
    $name:ru-RU: "Система и безопасность"
    $name:es-ES: "Sistema y seguridad"
    $name:de-DE: "System und Sicherheit"
    $name:pt-BR: "Sistema e Segurança"
    $name:zh-CN: "系统和安全"
  - security:
    - maintenance: false
      $name: "Security and Maintenance"
      $name:ru-RU: "Центр безопасности и обслуживания"
      $name:es-ES: "Seguridad y mantenimiento"
      $name:de-DE: "Sicherheit und Wartung"
      $name:pt-BR: "Segurança e Manutenção"
      $name:zh-CN: "安全和维护"
    - firewall: true
      $name: "Windows Defender Firewall"
      $name:ru-RU: "Брандмауэр Защитника Windows"
      $name:es-ES: "Firewall de Windows Defender"
      $name:de-DE: "Windows Defender Firewall"
      $name:pt-BR: "Windows Defender Firewall"
      $name:zh-CN: "Windows Defender 防火墙"
    - powerOptions: true
      $name: "Power Options"
      $name:ru-RU: "Электропитание"
      $name:es-ES: "Opciones de energía"
      $name:de-DE: "Energieoptionen"
      $name:pt-BR: "Opções de Energia"
      $name:zh-CN: "电源选项"
    - bitlocker: true
      $name: "BitLocker Drive Encryption"
      $name:ru-RU: "Шифрование диска BitLocker"
      $name:es-ES: "Cifrado de unidad BitLocker"
      $name:de-DE: "BitLocker-Laufwerkverschlüsselung"
      $name:pt-BR: "Criptografia de Unidade de Disco BitLocker"
      $name:zh-CN: "BitLocker 驱动器加密"
    $name: " "
  - networkVisible: false
    $name: "Network and Internet"
    $name:ru-RU: "Сеть и Интернет"
    $name:es-ES: "Redes e Internet"
    $name:de-DE: "Netzwerk und Internet"
    $name:pt-BR: "Rede e Internet"
    $name:zh-CN: "网络和 Internet"
  - network:
    - sharing: false
      $name: "Network and Sharing Center"
      $name:ru-RU: "Центр управления сетями и общим доступом"
      $name:es-ES: "Centro de redes y recursos compartidos"
      $name:de-DE: "Netzwerk- und Freigabecenter"
      $name:pt-BR: "Central de Rede e Compartilhamento"
      $name:zh-CN: "网络和共享中心"
    - connections: true
      $name: "Network Connections"
      $name:ru-RU: "Сетевые подключения"
      $name:es-ES: "Conexiones de red"
      $name:de-DE: "Netzwerkverbindungen"
      $name:pt-BR: "Conexões de Rede"
      $name:zh-CN: "网络连接"
    - internetOptions: true
      $name: "Internet Options"
      $name:ru-RU: "Свойства браузера"
      $name:es-ES: "Opciones de Internet"
      $name:de-DE: "Internetoptionen"
      $name:pt-BR: "Opções da Internet"
      $name:zh-CN: "Internet 选项"
    $name: " "
  - hardwareVisible: true
    $name: "Hardware and Sound"
    $name:ru-RU: "Оборудование и звук"
    $name:es-ES: "Hardware y sonido"
    $name:de-DE: "Hardware und Sound"
    $name:pt-BR: "Hardware e Sons"
    $name:zh-CN: "硬件和声音"
  - hardware:
    - deviceManager: true
      $name: "Device Manager"
      $name:ru-RU: "Диспетчер устройств"
      $name:es-ES: "Administrador de dispositivos"
      $name:de-DE: "Geräte-Manager"
      $name:pt-BR: "Gerenciador de Dispositivos"
      $name:zh-CN: "设备管理器"
    - sound: true
      $name: "Sound"
      $name:ru-RU: "Звук"
      $name:es-ES: "Sonido"
      $name:de-DE: "Sound"
      $name:pt-BR: "Som"
      $name:zh-CN: "声音"
    - mouse: true
      $name: "Mouse"
      $name:ru-RU: "Мышь"
      $name:es-ES: "Mouse"
      $name:de-DE: "Maus"
      $name:pt-BR: "Mouse"
      $name:zh-CN: "鼠标"
    - keyboard: false
      $name: "Keyboard"
      $name:ru-RU: "Клавиатура"
      $name:es-ES: "Teclado"
      $name:de-DE: "Tastatur"
      $name:pt-BR: "Teclado"
      $name:zh-CN: "键盘"
    - controllers: false
      $name: "Game Controllers"
      $name:ru-RU: "Игровые устройства"
      $name:es-ES: "Dispositivos de juego"
      $name:de-DE: "Gamecontroller"
      $name:pt-BR: "Controladores de Jogo"
      $name:zh-CN: "游戏控制器"
    $name: " "
  - programsVisible: false
    $name: "Programs"
    $name:ru-RU: "Программы"
    $name:es-ES: "Programas"
    $name:de-DE: "Programme"
    $name:pt-BR: "Programas"
    $name:zh-CN: "程序"
  - programs:
    - programsFeatures: true
      $name: "Programs and Features"
      $name:ru-RU: "Программы и компоненты"
      $name:es-ES: "Programas y características"
      $name:de-DE: "Programme und Features"
      $name:pt-BR: "Programas e Recursos"
      $name:zh-CN: "程序和功能"
    - windowsFeatures: true
      $name: "Turn Windows features on or off"
      $name:ru-RU: "Включение или отключение компонентов Windows"
      $name:es-ES: "Activar o desactivar las características de Windows"
      $name:de-DE: "Windows-Features aktivieren oder deaktivieren"
      $name:pt-BR: "Ativar ou desativar recursos do Windows"
      $name:zh-CN: "启用或关闭 Windows 功能"
    $name: " "
  - userAccountsVisible: false
    $name: "User Accounts"
    $name:ru-RU: "Учетные записи пользователей"
    $name:es-ES: "Cuentas de usuario"
    $name:de-DE: "Benutzerkonten"
    $name:pt-BR: "Contas de Usuário"
    $name:zh-CN: "用户帐户"
  - userAccounts:
    - credentials: true
      $name: "Credential Manager"
      $name:ru-RU: "Диспетчер учетных данных"
      $name:es-ES: "Administrador de credenciales"
      $name:de-DE: "Anmeldeinformationsverwaltung"
      $name:pt-BR: "Gerenciador de Credenciais"
      $name:zh-CN: "凭据管理器"
    - uac: true
      $name: "Change User Account Control settings"
      $name:ru-RU: "Изменение параметров контроля учетных записей"
      $name:es-ES: "Cambiar configuración de Control de cuentas de usuario"
      $name:de-DE: "Einstellungen der Benutzerkontensteuerung ändern"
      $name:pt-BR: "Alterar configurações de Controle de Conta de Usuário"
      $name:zh-CN: "更改用户帐户控制设置"
    $name: " "
  - appearanceVisible: true
    $name: "Appearance and Personalization"
    $name:ru-RU: "Оформление и персонализация"
    $name:es-ES: "Apariencia y personalización"
    $name:de-DE: "Darstellung und Anpassung"
    $name:pt-BR: "Aparência e Personalização"
    $name:zh-CN: "外观和个性化"
  - appearance:
    - explorerOptions: true
      $name: "File Explorer Options"
      $name:ru-RU: "Параметры Проводника"
      $name:es-ES: "Opciones del Explorador de archivos"
      $name:de-DE: "Explorer-Optionen"
      $name:pt-BR: "Opções do Explorador de Arquivos"
      $name:zh-CN: "文件资源管理器选项"
    - desktopIcons: true
      $name: "Desktop Icon Settings"
      $name:ru-RU: "Параметры значков рабочего стола"
      $name:es-ES: "Configuración de iconos de escritorio"
      $name:de-DE: "Desktopsymboleinstellungen"
      $name:pt-BR: "Configurações dos Ícones da Área de Trabalho"
      $name:zh-CN: "桌面图标设置"
    - colorManagement: false
      $name: "Color Management"
      $name:ru-RU: "Управление цветом"
      $name:es-ES: "Administración del color"
      $name:de-DE: "Farbverwaltung"
      $name:pt-BR: "Gerenciamento de Cores"
      $name:zh-CN: "颜色管理"
    $name: " "
  - clockRegionVisible: false
    $name: "Clock and Region"
    $name:ru-RU: "Часы и регион"
    $name:es-ES: "Reloj y región"
    $name:de-DE: "Zeit und Region"
    $name:pt-BR: "Relógio e Região"
    $name:zh-CN: "时钟和区域"
  - clockRegion:
    - dateTime: true
      $name: "Date and Time"
      $name:ru-RU: "Дата и время"
      $name:es-ES: "Fecha y hora"
      $name:de-DE: "Datum und Uhrzeit"
      $name:pt-BR: "Data e Hora"
      $name:zh-CN: "日期和时间"
    - region: true
      $name: "Region"
      $name:ru-RU: "Региональные стандарты"
      $name:es-ES: "Región"
      $name:de-DE: "Region"
      $name:pt-BR: "Região"
      $name:zh-CN: "区域"
    $name: " "
  - accessibility: false
    $name: "Ease of Access"
    $name:ru-RU: "Специальные возможности"
    $name:es-ES: "Accesibilidad"
    $name:de-DE: "Erleichterte Bedienung"
    $name:pt-BR: "Facilidade de Acesso"
    $name:zh-CN: "轻松使用"
  $name: " "
- administrationEnabled: true
  $name: "Administration"
  $name:ru-RU: "Администрирование"
  $name:es-ES: "Administración"
  $name:de-DE: "Verwaltung"
  $name:pt-BR: "Administração"
  $name:zh-CN: "管理"
  $description: "System consoles, diagnostics, and terminals"
  $description:ru-RU: "Программы для управления системой, диагностика и терминалы"
  $description:es-ES: "Consolas del sistema, diagnóstico y terminales"
  $description:de-DE: "Systemkonsolen, Diagnose und Terminals"
  $description:pt-BR: "Consoles do sistema, diagnóstico e terminais"
  $description:zh-CN: "系统控制台、诊断和终端"
- administration:
  - toolsVisible: false
    $name: "Windows Tools"
    $name:ru-RU: "Инструменты Windows"
    $name:es-ES: "Herramientas de Windows"
    $name:de-DE: "Windows-Tools"
    $name:pt-BR: "Ferramentas do Windows"
    $name:zh-CN: "Windows 工具"
  - tools:
    - userCertificates: true
      $name: "Certificates - Current User"
      $name:ru-RU: "Сертификаты - текущий пользователь"
      $name:es-ES: "Certificados: usuario actual"
      $name:de-DE: "Zertifikate - Aktueller Benutzer"
      $name:pt-BR: "Certificados - Usuário Atual"
      $name:zh-CN: "证书 - 当前用户"
    - machineCertificates: true
      $name: "Certificates (Local Computer)"
      $name:ru-RU: "Сертификаты (локальный компьютер)"
      $name:es-ES: "Certificados (equipo local)"
      $name:de-DE: "Zertifikate (Lokaler Computer)"
      $name:pt-BR: "Certificados (computador local)"
      $name:zh-CN: "证书(本地计算机)"
    - firewallAdvanced: true
      $name: "Windows Defender Firewall with Advanced Security"
      $name:ru-RU: "Монитор брандмауэра Защитника Windows в режиме повышенной безопасности"
      $name:es-ES: "Windows Defender Firewall con seguridad avanzada"
      $name:de-DE: "Windows Defender Firewall mit erweiterter Sicherheit"
      $name:pt-BR: "Windows Defender Firewall com Segurança Avançada"
      $name:zh-CN: "高级安全 Windows Defender 防火墙"
    $name: " "
  - computerManagementVisible: true
    $name: "Computer Management"
    $name:ru-RU: "Управление компьютером"
    $name:es-ES: "Administración de equipos"
    $name:de-DE: "Computerverwaltung"
    $name:pt-BR: "Gerenciamento do Computador"
    $name:zh-CN: "计算机管理"
  - computerManagement:
    - sharedFolders: true
      $name: "Shared Folders"
      $name:ru-RU: "Общие папки"
      $name:es-ES: "Carpetas compartidas"
      $name:de-DE: "Freigegebene Ordner"
      $name:pt-BR: "Pastas compartilhadas"
      $name:zh-CN: "共享文件夹"
    - localUsers: true
      $name: "Local Users and Groups"
      $name:ru-RU: "Локальные пользователи и группы"
      $name:es-ES: "Usuarios y grupos locales"
      $name:de-DE: "Lokale Benutzer und Gruppen"
      $name:pt-BR: "Usuários e Grupos Locais"
      $name:zh-CN: "本地用户和组"
    - wmi: false
      $name: "WMI Control"
      $name:ru-RU: "Управляющий элемент WMI"
      $name:es-ES: "Control WMI"
      $name:de-DE: "WMI-Steuerung"
      $name:pt-BR: "Controle WMI"
      $name:zh-CN: "WMI 控件"
    $name: " "
  - diskManagementVisible: true
    $name: "Disk Management"
    $name:ru-RU: "Управление дисками"
    $name:es-ES: "Administración de discos"
    $name:de-DE: "Datenträgerverwaltung"
    $name:pt-BR: "Gerenciamento de Disco"
    $name:zh-CN: "磁盘管理"
  - diskManagement:
    - optimize: true
      $name: "Defragment and Optimize Drives"
      $name:ru-RU: "Оптимизация дисков"
      $name:es-ES: "Desfragmentar y optimizar unidades"
      $name:de-DE: "Laufwerke defragmentieren und optimieren"
      $name:pt-BR: "Desfragmentar e Otimizar Unidades"
      $name:zh-CN: "碎片整理和优化驱动器"
    - cleanup: true
      $name: "Disk Cleanup"
      $name:ru-RU: "Очистка диска"
      $name:es-ES: "Liberador de espacio en disco"
      $name:de-DE: "Datenträgerbereinigung"
      $name:pt-BR: "Limpeza de Disco"
      $name:zh-CN: "磁盘清理"
    - recoveryDrive: false
      $name: "Recovery Drive"
      $name:ru-RU: "Диск восстановления"
      $name:es-ES: "Unidad de recuperación"
      $name:de-DE: "Wiederherstellungslaufwerk"
      $name:pt-BR: "Unidade de Recuperação"
      $name:zh-CN: "恢复驱动器"
    $name: " "
  - servicesVisible: true
    $name: "Services"
    $name:ru-RU: "Службы"
    $name:es-ES: "Servicios"
    $name:de-DE: "Dienste"
    $name:pt-BR: "Serviços"
    $name:zh-CN: "服务"
  - services:
    - taskScheduler: true
      $name: "Task Scheduler"
      $name:ru-RU: "Планировщик задач"
      $name:es-ES: "Programador de tareas"
      $name:de-DE: "Aufgabenplanung"
      $name:pt-BR: "Agendador de Tarefas"
      $name:zh-CN: "任务计划程序"
    - componentServices: true
      $name: "Component Services"
      $name:ru-RU: "Службы компонентов"
      $name:es-ES: "Servicios de componentes"
      $name:de-DE: "Komponentendienste"
      $name:pt-BR: "Serviços de Componentes"
      $name:zh-CN: "组件服务"
    $name: " "
  - eventViewerVisible: true
    $name: "Event Viewer"
    $name:ru-RU: "Просмотр событий"
    $name:es-ES: "Visor de eventos"
    $name:de-DE: "Ereignisanzeige"
    $name:pt-BR: "Visualizador de Eventos"
    $name:zh-CN: "事件查看器"
  - eventViewer:
    - performance: true
      $name: "Performance Monitor"
      $name:ru-RU: "Системный монитор"
      $name:es-ES: "Monitor de rendimiento"
      $name:de-DE: "Leistungsüberwachung"
      $name:pt-BR: "Monitor de Desempenho"
      $name:zh-CN: "性能监视器"
    - resources: true
      $name: "Resource Monitor"
      $name:ru-RU: "Монитор ресурсов"
      $name:es-ES: "Monitor de recursos"
      $name:de-DE: "Ressourcenmonitor"
      $name:pt-BR: "Monitor de Recursos"
      $name:zh-CN: "资源监视器"
    - reliability: false
      $name: "Reliability Monitor"
      $name:ru-RU: "Монитор стабильности системы"
      $name:es-ES: "Monitor de confiabilidad"
      $name:de-DE: "Zuverlässigkeitsüberwachung"
      $name:pt-BR: "Monitor de Confiabilidade"
      $name:zh-CN: "可靠性监视程序"
    - systemInfo: true
      $name: "System Information"
      $name:ru-RU: "Сведения о системе"
      $name:es-ES: "Información del sistema"
      $name:de-DE: "Systeminformationen"
      $name:pt-BR: "Informações do sistema"
      $name:zh-CN: "系统信息"
    - memoryDiagnostic: false
      $name: "Windows Memory Diagnostic"
      $name:ru-RU: "Средство проверки памяти Windows"
      $name:es-ES: "Diagnóstico de memoria de Windows"
      $name:de-DE: "Windows-Speicherdiagnose"
      $name:pt-BR: "Diagnóstico de Memória do Windows"
      $name:zh-CN: "Windows 内存诊断"
    $name: " "
  - registryVisible: true
    $name: "Registry Editor"
    $name:ru-RU: "Редактор реестра"
    $name:es-ES: "Editor del Registro"
    $name:de-DE: "Registrierungs-Editor"
    $name:pt-BR: "Editor do Registro"
    $name:zh-CN: "注册表编辑器"
  - registry:
    - systemConfiguration: true
      $name: "System Configuration"
      $name:ru-RU: "Конфигурация системы"
      $name:es-ES: "Configuración del sistema"
      $name:de-DE: "Systemkonfiguration"
      $name:pt-BR: "Configuração do Sistema"
      $name:zh-CN: "系统配置"
    - groupPolicy: true
      $name: "Local Group Policy Editor"
      $name:ru-RU: "Редактор локальной групповой политики"
      $name:es-ES: "Editor de directivas de grupo local"
      $name:de-DE: "Editor für lokale Gruppenrichtlinien"
      $name:pt-BR: "Editor de Política de Grupo Local"
      $name:zh-CN: "本地组策略编辑器"
    - localSecurity: false
      $name: "Local Security Policy"
      $name:ru-RU: "Локальная политика безопасности"
      $name:es-ES: "Directiva de seguridad local"
      $name:de-DE: "Lokale Sicherheitsrichtlinie"
      $name:pt-BR: "Política de Segurança Local"
      $name:zh-CN: "本地安全策略"
    $name: " "
  - terminalVisible: true
    $name: "Terminal"
    $name:ru-RU: "Терминал"
    $name:es-ES: "Terminal"
    $name:de-DE: "Terminal"
    $name:pt-BR: "Terminal"
    $name:zh-CN: "终端"
  - terminal:
    - terminalAdmin: true
      $name: "Terminal (Admin)"
      $name:ru-RU: "Терминал (Администратор)"
      $name:es-ES: "Terminal (Administrador)"
      $name:de-DE: "Terminal (Administrator)"
      $name:pt-BR: "Terminal (Administrador)"
      $name:zh-CN: "终端管理员"
    - powershell: true
      $name: "Windows PowerShell"
      $name:ru-RU: "Windows PowerShell"
      $name:es-ES: "Windows PowerShell"
      $name:de-DE: "Windows PowerShell"
      $name:pt-BR: "Windows PowerShell"
      $name:zh-CN: "Windows PowerShell"
    - commandPrompt: false
      $name: "Command Prompt"
      $name:ru-RU: "Командная строка"
      $name:es-ES: "Símbolo del sistema"
      $name:de-DE: "Eingabeaufforderung"
      $name:pt-BR: "Prompt de Comando"
      $name:zh-CN: "命令提示符"
    $name: " "
  $name: " "
- commandsEnabled: true
  $name: "Basic commands"
  $name:ru-RU: "Основные команды"
  $name:es-ES: "Comandos básicos"
  $name:de-DE: "Grundbefehle"
  $name:pt-BR: "Comandos básicos"
  $name:zh-CN: "基本命令"
  $description: "File Explorer, system actions, and power"
  $description:ru-RU: "Проводник, системные действия и питание"
  $description:es-ES: "Explorador de archivos, acciones del sistema y energía"
  $description:de-DE: "Datei-Explorer, Systemaktionen und Energie"
  $description:pt-BR: "Explorador de Arquivos, ações do sistema e energia"
  $description:zh-CN: "文件资源管理器、系统操作和电源"
- commands:
  - run: true
    $name: "Run"
    $name:ru-RU: "Выполнить"
    $name:es-ES: "Ejecutar"
    $name:de-DE: "Ausführen"
    $name:pt-BR: "Executar"
    $name:zh-CN: "运行"
  - taskManager: true
    $name: "Task Manager"
    $name:ru-RU: "Диспетчер задач"
    $name:es-ES: "Administrador de tareas"
    $name:de-DE: "Task-Manager"
    $name:pt-BR: "Gerenciador de Tarefas"
    $name:zh-CN: "任务管理器"
  - explorerVisible: true
    $name: "File Explorer"
    $name:ru-RU: "Проводник"
    $name:es-ES: "Explorador de archivos"
    $name:de-DE: "Explorer"
    $name:pt-BR: "Explorador de Arquivos"
    $name:zh-CN: "文件资源管理器"
  - explorer:
    - desktop: true
      $name: "Desktop"
      $name:ru-RU: "Рабочий стол"
      $name:es-ES: "Escritorio"
      $name:de-DE: "Desktop"
      $name:pt-BR: "Área de Trabalho"
      $name:zh-CN: "桌面"
    - computer: true
      $name: "This PC"
      $name:ru-RU: "Этот компьютер"
      $name:es-ES: "Este equipo"
      $name:de-DE: "Dieser PC"
      $name:pt-BR: "Este Computador"
      $name:zh-CN: "此电脑"
    - personal: true
      $name: "Personal Folder"
      $name:ru-RU: "Личная папка"
      $name:es-ES: "Carpeta personal"
      $name:de-DE: "Persönlicher Ordner"
      $name:pt-BR: "Pasta Pessoal"
      $name:zh-CN: "个人文件夹"
    - documents: false
      $name: "Documents"
      $name:ru-RU: "Документы"
      $name:es-ES: "Documentos"
      $name:de-DE: "Dokumente"
      $name:pt-BR: "Documentos"
      $name:zh-CN: "文档"
    - pictures: false
      $name: "Pictures"
      $name:ru-RU: "Изображения"
      $name:es-ES: "Imágenes"
      $name:de-DE: "Bilder"
      $name:pt-BR: "Imagens"
      $name:zh-CN: "图片"
    - downloads: false
      $name: "Downloads"
      $name:ru-RU: "Загрузки"
      $name:es-ES: "Descargas"
      $name:de-DE: "Downloads"
      $name:pt-BR: "Downloads"
      $name:zh-CN: "下载"
    - recycleBin: true
      $name: "Recycle Bin"
      $name:ru-RU: "Корзина"
      $name:es-ES: "Papelera de reciclaje"
      $name:de-DE: "Papierkorb"
      $name:pt-BR: "Lixeira"
      $name:zh-CN: "回收站"
    $name: " "
  - find: false
    $name: "Search"
    $name:ru-RU: "Найти"
    $name:es-ES: "Buscar"
    $name:de-DE: "Suchen"
    $name:pt-BR: "Pesquisar"
    $name:zh-CN: "搜索"
  - desktop: false
    $name: "Desktop"
    $name:ru-RU: "Рабочий стол"
    $name:es-ES: "Escritorio"
    $name:de-DE: "Desktop"
    $name:pt-BR: "Área de Trabalho"
    $name:zh-CN: "桌面"
  - powerVisible: true
    $name: "Shut down or sign out"
    $name:ru-RU: "Завершение работы или выход из системы"
    $name:es-ES: "Apagar o cerrar sesión"
    $name:de-DE: "Herunterfahren oder abmelden"
    $name:pt-BR: "Desligar ou sair"
    $name:zh-CN: "关机或注销"
  - power:
    - lock: true
      $name: "Lock"
      $name:ru-RU: "Заблокировать"
      $name:es-ES: "Bloquear"
      $name:de-DE: "Sperren"
      $name:pt-BR: "Bloquear"
      $name:zh-CN: "锁定"
    - signOut: true
      $name: "Sign out"
      $name:ru-RU: "Выход"
      $name:es-ES: "Cerrar sesión"
      $name:de-DE: "Abmelden"
      $name:pt-BR: "Sair"
      $name:zh-CN: "注销"
    - sleep: true
      $name: "Sleep"
      $name:ru-RU: "Спящий режим"
      $name:es-ES: "Suspender"
      $name:de-DE: "Energie sparen"
      $name:pt-BR: "Suspender"
      $name:zh-CN: "睡眠"
    - hibernate: true
      $name: "Hibernate"
      $name:ru-RU: "Гибернация"
      $name:es-ES: "Hibernar"
      $name:de-DE: "Ruhezustand"
      $name:pt-BR: "Hibernar"
      $name:zh-CN: "休眠"
    - shutdown: true
      $name: "Shut down"
      $name:ru-RU: "Завершение работы"
      $name:es-ES: "Apagar"
      $name:de-DE: "Herunterfahren"
      $name:pt-BR: "Desligar"
      $name:zh-CN: "关机"
    - restart: true
      $name: "Restart"
      $name:ru-RU: "Перезагрузка"
      $name:es-ES: "Reiniciar"
      $name:de-DE: "Neu starten"
      $name:pt-BR: "Reiniciar"
      $name:zh-CN: "重启"
    $name: " "
  $name: " "
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <dwrite.h>
#include <shellapi.h>
#include <ipifcons.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <netioapi.h>
#include <uiautomationclient.h>
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace X = winrt::Windows::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace I = X::Input;
namespace AP = X::Automation::Peers;
namespace AV = X::Automation::Provider;
using Key = winrt::Windows::System::VirtualKey;
using PointerType = winrt::Windows::Devices::Input::PointerDeviceType;

namespace {

struct CatalogItem {
    PCWSTR key;
    int group;
    int parent;
    wchar_t glyph;
    PCWSTR action;
    PCWSTR resource;
    std::array<PCWSTR, 6> names;
    std::array<unsigned, 2> updateCaptions{};
};

constexpr PCWSTR sectionEnabledKeys[] = {
    L"windowsSettingsEnabled",
    L"controlPanelEnabled",
    L"administrationEnabled",
    L"commandsEnabled",
};

constexpr CatalogItem catalog[] = {
    {L"windowsSettings.systemVisible", 0, -1, 0xe770, L"uri:system", L"settings:SettingsPageGroupPCSystem",
     {L"System", L"Система", L"Sistema", L"System", L"Sistema", L"系统"}},
    {L"windowsSettings.system.display", 0, 0, 0xe770, L"uri:display", L"settings:SettingsPagePCSystemDisplay",
     {L"Display", L"Дисплей", L"Pantalla", L"Bildschirm", L"Tela", L"屏幕"}},
    {L"windowsSettings.system.sound", 0, 0, 0xe767, L"uri:sound", L"settings:SettingsPageAudio",
     {L"Sound", L"Звук", L"Sonido", L"Sound", L"Som", L"声音"}},
    {L"windowsSettings.system.power", 0, 0, 0xe7e8, L"uri:powersleep", L"settings:SettingsPagePowerAndBattery",
     {L"Power", L"Питание", L"Inicio/Apagado", L"Leistung", L"Desligar", L"电源"}},
    {L"windowsSettings.system.storage", 0, 0, 0xeda2, L"uri:storagesense", L"settings:SettingsPageStorageSenseStorageOverview",
     {L"Storage", L"Память", L"Almacenamiento", L"Speicher", L"Armazenamento", L"存储"}},
    {L"windowsSettings.system.advanced", 0, 0, 0xec7a, L"uri:developers", L"settings:SettingsPageAdvancedSystemSettings",
     {L"Advanced", L"Дополнительно", L"Avanzado", L"Erweitert", L"Avançado", L"高级"}},
    {L"windowsSettings.system.about", 0, 0, 0xe946, L"uri:about", L"settings:SettingsPageAbout",
     {L"About", L"О системе", L"Información", L"Info", L"Sobre", L"系统信息"}},
    {L"windowsSettings.devicesVisible", 0, -1, 0xe702, L"uri:devices", L"settings:SettingsPageGroupDevices",
     {L"Bluetooth & devices", L"Bluetooth и устройства", L"Bluetooth y dispositivos", L"Bluetooth und Geräte", L"Bluetooth e dispositivos", L"蓝牙和其他设备"}},
    {L"windowsSettings.devices.devices", 0, 7, 0xe772, L"uri:connecteddevices", L"settings:SettingsPageDevices",
     {L"Devices", L"Устройства", L"Dispositivos", L"Geräte", L"Dispositivos", L"设备"}},
    {L"windowsSettings.devices.printers", 0, 7, 0xe749, L"uri:printers", L"settings:SettingsPageDevicesPrinters",
     {L"Printers & scanners", L"Принтеры и сканеры", L"Impresoras y escáneres", L"Drucker und Scanner", L"Impressoras e scanners", L"打印机和扫描仪"}},
    {L"windowsSettings.devices.cameras", 0, 7, 0xe722, L"uri:camera", L"settings:SettingsPageDevicesCameras",
     {L"Cameras", L"Камеры", L"Cámaras", L"Kameras", L"Câmeras", L"摄像头"}},
    {L"windowsSettings.devices.mouse", 0, 7, 0xe962, L"uri:mousetouchpad", L"settings:SettingsPagePCSystemDeviceSettings",
     {L"Mouse", L"Мышь", L"Mouse", L"Maus", L"Mouse", L"鼠标"}},
    {L"windowsSettings.devices.keyboard", 0, 7, 0xe765, L"uri:devices-keyboard", L"settings:SettingsPageDevicesKeyboard",
     {L"Keyboard", L"Клавиатура", L"Teclado", L"Tastatur", L"Teclado", L"键盘"}},
    {L"windowsSettings.networkVisible", 0, -1, 0xe701, L"uri:network-status", L"settings:SettingsPageGroupNetwork",
     {L"Network & internet", L"Сеть и Интернет", L"Red e Internet", L"Netzwerk und Internet", L"Rede e Internet", L"网络和 Internet"}},
    {L"windowsSettings.network.ethernet", 0, 13, 0xe839, L"uri:network-ethernet", L"settings:SettingsPageNetworkEthernet",
     {L"Ethernet", L"Ethernet", L"Ethernet", L"Ethernet", L"Ethernet", L"以太网"}},
    {L"windowsSettings.network.wifi", 0, 13, 0xe701, L"uri:network-wifi", L"settings:SettingsPageNetworkWiFi",
     {L"Wi-Fi", L"Wi-Fi", L"Wi-Fi", L"WLAN", L"Wi-Fi", L"WLAN"}},
    {L"windowsSettings.network.hotspot", 0, 13, 0xe704, L"uri:network-mobilehotspot", L"settings:SettingsPageNetworkMobileHotspot",
     {L"Mobile hotspot", L"Мобильный хот-спот", L"Zona con cobertura inalámbrica móvil", L"Mobiler Hotspot", L"Hotspot móvel", L"移动热点"}},
    {L"windowsSettings.network.vpn", 0, 13, 0xe705, L"uri:network-vpn", L"settings:SettingsPageNetworkVPN",
     {L"VPN", L"VPN", L"VPN", L"VPN", L"VPN", L"VPN"}},
    {L"windowsSettings.network.proxy", 0, 13, 0xe968, L"uri:network-proxy", L"settings:SettingsPageNetworkProxy",
     {L"Proxy", L"Прокси-сервер", L"Proxy", L"Proxy", L"Proxy", L"代理"}},
    {L"windowsSettings.network.advancedNetwork", 0, 13, 0xeda3, L"uri:network-advancedsettings", L"settings:SettingsPageNetworkManageAdapterOptions",
     {L"Advanced network settings", L"Дополнительные сетевые параметры", L"Configuración de red avanzada", L"Erweiterte Netzwerkeinstellungen", L"Configurações avançadas de rede", L"高级网络设置"}},
    {L"windowsSettings.personalizationVisible", 0, -1, 0xe771, L"uri:personalization", L"settings:SettingsPageGroupPersonalization",
     {L"Personalization", L"Персонализация", L"Personalización", L"Personalisierung", L"Personalização", L"个性化"}},
    {L"windowsSettings.personalization.background", 0, 20, 0xeb9f, L"uri:personalization-background", L"settings:SettingsPageBackground",
     {L"Background", L"Фон", L"Fondo", L"Hintergrund", L"Tela de fundo", L"背景"}},
    {L"windowsSettings.personalization.colors", 0, 20, 0xe2b1, L"uri:personalization-colors", L"settings:SettingsPageColors",
     {L"Colors", L"Цвета", L"Colores", L"Farben", L"Cores", L"颜色"}},
    {L"windowsSettings.personalization.themes", 0, 20, 0xe771, L"uri:themes", L"settings:SettingsPageThemes",
     {L"Themes", L"Темы", L"Temas", L"Designs", L"Temas", L"主题"}},
    {L"windowsSettings.personalization.lockScreen", 0, 20, 0xee3f, L"uri:lockscreen", L"settings:SettingsPageLockScreen",
     {L"Lock screen", L"Экран блокировки", L"Pantalla de bloqueo", L"Sperrbildschirm", L"Tela de bloqueio", L"锁屏界面"}},
    {L"windowsSettings.personalization.start", 0, 20, 0xe8fc, L"uri:personalization-start", L"settings:SettingsPageStart",
     {L"Start", L"Пуск", L"Inicio", L"Start", L"Iniciar", L"开始"}},
    {L"windowsSettings.personalization.taskbar", 0, 20, 0xe90e, L"uri:taskbar", L"settings:SettingsPageTaskbar",
     {L"Taskbar", L"Панель задач", L"Barra de tareas", L"Taskleiste", L"Barra de Tarefas", L"任务栏"}},
    {L"windowsSettings.appsVisible", 0, -1, 0xe8a9, L"category:installed-apps;SystemSettings_StorageSense_AppSizesListFilter_DisplayStringValue", L"settings:SettingsPageGroupApps",
     {L"Apps", L"Приложения", L"Aplicaciones", L"Apps", L"Aplicativos", L"应用"}},
    {L"windowsSettings.apps.installed", 0, 27, 0xe71d, L"uri:installed-apps", L"settings:SettingsPageInstalledApps",
     {L"Installed apps", L"Установленные приложения", L"Aplicaciones instaladas", L"Installierte Apps", L"Aplicativos instalados", L"安装的应用"}},
    {L"windowsSettings.apps.defaults", 0, 27, 0xec74, L"uri:defaultapps", L"settings:SettingsPageAppsDefaults",
     {L"Default apps", L"Приложения по умолчанию", L"Aplicaciones predeterminadas", L"Standard-Apps", L"Aplicativos padrão", L"默认应用"}},
    {L"windowsSettings.apps.startup", 0, 27, 0xe7b5, L"uri:startupapps", L"settings:SettingsPageStartup",
     {L"Startup", L"Автозагрузка", L"Inicio", L"Autostart", L"Inicialização", L"启动"}},
    {L"windowsSettings.accountsVisible", 0, -1, 0xe77b, L"uri:accounts", L"settings:SettingsPageGroupAccounts",
     {L"Accounts", L"Учетные записи", L"Cuentas", L"Konten", L"Contas", L"帐户"}},
    {L"windowsSettings.accounts.signIn", 0, 31, 0xe8d7, L"uri:signinoptions", L"settings:SettingsPageSignInOptions",
     {L"Sign-in options", L"Варианты входа", L"Opciones de inicio de sesión", L"Anmeldeoptionen", L"Opções de entrada", L"登录选项"}},
    {L"windowsSettings.accounts.backup", 0, 31, 0xe78c, L"uri:backup", L"app:MicrosoftWindows.Client.CBS_cw5n1h2txyewy!WindowsBackup",
     {L"Windows Backup", L"Архивация Windows", L"Copias de seguridad de Windows", L"Windows-Sicherung", L"Backup do Windows", L"Windows 备份"}},
    {L"windowsSettings.accounts.passkeys", 0, 31, 0xee7e, L"uri:savedpasskeys", L"settings:SettingsPagePasskeysManagement",
     {L"Passkeys", L"Ключи доступа", L"Claves de acceso", L"Hauptschlüssel", L"Chaves de acesso", L"通行密钥"}},
    {L"windowsSettings.timeLanguageVisible", 0, -1, 0xe775, L"category:dateandtime;SystemSettings_DateTime_IsTimeSetAutomaticallyEnabled_ToggleSwitch", L"settings:SettingsPageGroupTimeRegion",
     {L"Time & language", L"Время и язык", L"Hora e idioma", L"Zeit und Sprache", L"Hora e idioma", L"时间和语言"}},
    {L"windowsSettings.timeLanguage.dateTime", 0, 35, 0xec92, L"uri:dateandtime", L"settings:SettingsPageTimeRegionDateTime",
     {L"Date & time", L"Дата и время", L"Fecha y hora", L"Datum und Zeit", L"Data e hora", L"日期和时间"}},
    {L"windowsSettings.timeLanguage.language", 0, 35, 0xf2b7, L"uri:regionlanguage", L"settings:SettingsPageTimeRegionLanguage",
     {L"Language & region", L"Язык и регион", L"Idioma y región", L"Sprache und Region", L"Idioma e região", L"语言和区域"}},
    {L"windowsSettings.timeLanguage.typing", 0, 35, 0xe765, L"uri:typing", L"settings:SettingsPageTimeRegionSpelling",
     {L"Typing", L"Ввод", L"Escritura", L"Eingabe", L"Digitação", L"输入"}},
    {L"windowsSettings.gaming", 0, -1, 0xe7fc, L"category:gaming-gamemode;SystemSettings_Gaming_GameMode_Toggle_ToggleSwitch", L"settings:SettingsPageGroupGaming2",
     {L"Gaming", L"Игры", L"Juegos", L"Spielen", L"Jogos", L"游戏"}},
    {L"windowsSettings.accessibility", 0, -1, 0xe776, L"uri:easeofaccess", L"settings:SettingsPageGroupEaseOfAccess",
     {L"Accessibility", L"Специальные возможности", L"Accesibilidad", L"Barrierefreiheit", L"Acessibilidade", L"辅助功能"}},
    {L"windowsSettings.privacy", 0, -1, 0xea18, L"uri:privacy", L"settings:SettingsPageGroupPrivacy",
     {L"Privacy & security", L"Конфиденциальность и защита", L"Privacidad y seguridad", L"Datenschutz und Sicherheit", L"Privacidade e segurança", L"隐私和安全性"}},
    {L"windowsSettings.update", 0, -1, 0xe895, L"uri:windowsupdate", L"settings:SettingsPageGroupRestore",
     {L"Windows Update", L"Центр обновления Windows", L"Windows Update", L"Windows Update", L"Windows Update", L"Windows 更新"}},
    {L"controlPanel.securityVisible", 1, -1, 0xf760, L"panel:5", L"shell32.dll,-32014",
     {L"System and Security", L"Система и безопасность", L"Sistema y seguridad", L"System und Sicherheit", L"Sistema e Segurança", L"系统和安全"}},
    {L"controlPanel.security.maintenance", 1, 43, 0xe7c1, L"control:Microsoft.ActionCenter", L"shell:{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}",
     {L"Security and Maintenance", L"Центр безопасности и обслуживания", L"Seguridad y mantenimiento", L"Sicherheit und Wartung", L"Segurança e Manutenção", L"安全和维护"}},
    {L"controlPanel.security.firewall", 1, 43, 0xf5b4, L"exe:Firewall.cpl", L"shell:{4026492F-2F69-46B8-B9BF-5654FC07E423}",
     {L"Windows Defender Firewall", L"Брандмауэр Защитника Windows", L"Firewall de Windows Defender", L"Windows Defender Firewall", L"Windows Defender Firewall", L"Windows Defender 防火墙"}},
    {L"controlPanel.security.powerOptions", 1, 43, 0xee63, L"exe:powercfg.cpl", L"shell:{025A5937-A6BE-4686-A844-36FE4BEC8B6D}",
     {L"Power Options", L"Электропитание", L"Opciones de energía", L"Energieoptionen", L"Opções de Energia", L"电源选项"}},
    {L"controlPanel.security.bitlocker", 1, 43, 0xe72e, L"control:Microsoft.BitLockerDriveEncryption", L"shell:{D9EF8727-CAC2-4E60-809E-86F80A666C91}",
     {L"BitLocker Drive Encryption", L"Шифрование диска BitLocker", L"Cifrado de unidad BitLocker", L"BitLocker-Laufwerkverschlüsselung", L"Criptografia de Unidade de Disco BitLocker", L"BitLocker 驱动器加密"}},
    {L"controlPanel.networkVisible", 1, -1, 0xee77, L"panel:3", L"shell32.dll,-32010",
     {L"Network and Internet", L"Сеть и Интернет", L"Redes e Internet", L"Netzwerk und Internet", L"Rede e Internet", L"网络和 Internet"}},
    {L"controlPanel.network.sharing", 1, 48, 0xec27, L"control:Microsoft.NetworkAndSharingCenter", L"shell:{8E908FC9-BECC-40F6-915B-F4CA0E70D03D}",
     {L"Network and Sharing Center", L"Центр управления сетями и общим доступом", L"Centro de redes y recursos compartidos", L"Netzwerk- und Freigabecenter", L"Central de Rede e Compartilhamento", L"网络和共享中心"}},
    {L"controlPanel.network.connections", 1, 48, 0xeda3, L"exe:ncpa.cpl", L"twinui.pcshell.dll,-10912",
     {L"Network Connections", L"Сетевые подключения", L"Conexiones de red", L"Netzwerkverbindungen", L"Conexões de Rede", L"网络连接"}},
    {L"controlPanel.network.internetOptions", 1, 48, 0xe774, L"exe:inetcpl.cpl", L"shell:{A3DD4F92-658A-410F-84FD-6FBBBEF2FFFE}",
     {L"Internet Options", L"Свойства браузера", L"Opciones de Internet", L"Internetoptionen", L"Opções da Internet", L"Internet 选项"}},
    {L"controlPanel.hardwareVisible", 1, -1, 0xe7f3, L"panel:2", L"shell32.dll,-32008",
     {L"Hardware and Sound", L"Оборудование и звук", L"Hardware y sonido", L"Hardware und Sound", L"Hardware e Sons", L"硬件和声音"}},
    {L"controlPanel.hardware.deviceManager", 1, 52, 0xe912, L"exe:mmc.exe;devmgmt.msc", L"twinui.pcshell.dll,-10916",
     {L"Device Manager", L"Диспетчер устройств", L"Administrador de dispositivos", L"Geräte-Manager", L"Gerenciador de Dispositivos", L"设备管理器"}},
    {L"controlPanel.hardware.sound", 1, 52, 0xe767, L"exe:mmsys.cpl", L"shell:{F2DDFC82-8F12-4CDD-B7DC-D4FE1425AA4D}",
     {L"Sound", L"Звук", L"Sonido", L"Sound", L"Som", L"声音"}},
    {L"controlPanel.hardware.mouse", 1, 52, 0xe962, L"exe:main.cpl", L"shell:{6C8EEC18-8D75-41B2-A177-8831D59D2D50}",
     {L"Mouse", L"Мышь", L"Mouse", L"Maus", L"Mouse", L"鼠标"}},
    {L"controlPanel.hardware.keyboard", 1, 52, 0xe765, L"control:keyboard", L"shell:{725BE8F7-668E-4C7B-8F90-46BDB0936430}",
     {L"Keyboard", L"Клавиатура", L"Teclado", L"Tastatur", L"Teclado", L"键盘"}},
    {L"controlPanel.hardware.controllers", 1, 52, 0xe7fc, L"exe:joy.cpl", L"joy.cpl,-1076",
     {L"Game Controllers", L"Игровые устройства", L"Dispositivos de juego", L"Gamecontroller", L"Controladores de Jogo", L"游戏控制器"}},
    {L"controlPanel.programsVisible", 1, -1, 0xe71d, L"panel:8", L"shell32.dll,-32006",
     {L"Programs", L"Программы", L"Programas", L"Programme", L"Programas", L"程序"}},
    {L"controlPanel.programs.programsFeatures", 1, 58, 0xe71d, L"exe:appwiz.cpl", L"shell:{7B81BE6A-CE2B-4676-A29E-EB907A5126C5}",
     {L"Programs and Features", L"Программы и компоненты", L"Programas y características", L"Programme und Features", L"Programas e Recursos", L"程序和功能"}},
    {L"controlPanel.programs.windowsFeatures", 1, 58, 0xe7b8, L"exe:OptionalFeatures.exe", L"appwiz.cpl,-188",
     {L"Turn Windows features on or off", L"Включение или отключение компонентов Windows", L"Activar o desactivar las características de Windows", L"Windows-Features aktivieren oder deaktivieren", L"Ativar ou desativar recursos do Windows", L"启用或关闭 Windows 功能"}},
    {L"controlPanel.userAccountsVisible", 1, -1, 0xe716, L"panel:9", L"shell32.dll,-32002",
     {L"User Accounts", L"Учетные записи пользователей", L"Cuentas de usuario", L"Benutzerkonten", L"Contas de Usuário", L"用户帐户"}},
    {L"controlPanel.userAccounts.credentials", 1, 61, 0xf540, L"control:Microsoft.CredentialManager", L"shell:{1206F5F1-0569-412C-8FEC-3204630DFB70}",
     {L"Credential Manager", L"Диспетчер учетных данных", L"Administrador de credenciales", L"Anmeldeinformationsverwaltung", L"Gerenciador de Credenciais", L"凭据管理器"}},
    {L"controlPanel.userAccounts.uac", 1, 61, 0xe7ef, L"exe:UserAccountControlSettings.exe", L"shell32.dll,-24317",
     {L"Change User Account Control settings", L"Изменение параметров контроля учетных записей", L"Cambiar configuración de Control de cuentas de usuario", L"Einstellungen der Benutzerkontensteuerung ändern", L"Alterar configurações de Controle de Conta de Usuário", L"更改用户帐户控制设置"}},
    {L"controlPanel.appearanceVisible", 1, -1, 0xe7f9, L"panel:1", L"shell32.dll,-32018",
     {L"Appearance and Personalization", L"Оформление и персонализация", L"Apariencia y personalización", L"Darstellung und Anpassung", L"Aparência e Personalização", L"外观和个性化"}},
    {L"controlPanel.appearance.explorerOptions", 1, 64, 0xe8b7, L"control:folders", L"shell:{6DFD7C5C-2451-11D3-A299-00C04F8EF6AF}",
     {L"File Explorer Options", L"Параметры Проводника", L"Opciones del Explorador de archivos", L"Explorer-Optionen", L"Opções do Explorador de Arquivos", L"文件资源管理器选项"}},
    {L"controlPanel.appearance.desktopIcons", 1, 64, 0xe7fb, L"exe:rundll32.exe;shell32.dll,Control_RunDLL desk.cpl,,0", L"desk.cpl,-112",
     {L"Desktop Icon Settings", L"Параметры значков рабочего стола", L"Configuración de iconos de escritorio", L"Desktopsymboleinstellungen", L"Configurações dos Ícones da Área de Trabalho", L"桌面图标设置"}},
    {L"controlPanel.appearance.colorManagement", 1, 64, 0xe790, L"exe:colorcpl.exe", L"shell:{B2C761C6-29BC-4F19-9251-E6195265BAF1}",
     {L"Color Management", L"Управление цветом", L"Administración del color", L"Farbverwaltung", L"Gerenciamento de Cores", L"颜色管理"}},
    {L"controlPanel.clockRegionVisible", 1, -1, 0xe917, L"panel:6", L"shell32.dll,-32026",
     {L"Clock and Region", L"Часы и регион", L"Reloj y región", L"Zeit und Region", L"Relógio e Região", L"时钟和区域"}},
    {L"controlPanel.clockRegion.dateTime", 1, 68, 0xe823, L"exe:timedate.cpl", L"shell:{E2E7934B-DCE5-43C4-9576-7FE4F75E7480}",
     {L"Date and Time", L"Дата и время", L"Fecha y hora", L"Datum und Uhrzeit", L"Data e Hora", L"日期和时间"}},
    {L"controlPanel.clockRegion.region", 1, 68, 0xe774, L"exe:intl.cpl", L"shell:{62D8ED13-C9D0-4CE8-A914-47DD628FB1B0}",
     {L"Region", L"Региональные стандарты", L"Región", L"Region", L"Região", L"区域"}},
    {L"controlPanel.accessibility", 1, -1, 0xe776, L"panel:7", L"shell32.dll,-32000",
     {L"Ease of Access", L"Специальные возможности", L"Accesibilidad", L"Erleichterte Bedienung", L"Facilidade de Acesso", L"轻松使用"}},
    {L"administration.toolsVisible", 2, -1, 0xe90f, L"control:Microsoft.AdministrativeTools", L"tools:Windows Tools",
     {L"Windows Tools", L"Инструменты Windows", L"Herramientas de Windows", L"Windows-Tools", L"Ferramentas do Windows", L"Windows 工具"}},
    {L"administration.tools.userCertificates", 2, 72, 0xeb95, L"exe:mmc.exe;certmgr.msc", L"certmgr.dll,-246",
     {L"Certificates - Current User", L"Сертификаты - текущий пользователь", L"Certificados: usuario actual", L"Zertifikate - Aktueller Benutzer", L"Certificados - Usuário Atual", L"证书 - 当前用户"}},
    {L"administration.tools.machineCertificates", 2, 72, 0xeb95, L"exe:mmc.exe;certlm.msc", L"certmgr.dll,-193",
     {L"Certificates (Local Computer)", L"Сертификаты (локальный компьютер)", L"Certificados (equipo local)", L"Zertifikate (Lokaler Computer)", L"Certificados (computador local)", L"证书(本地计算机)"}},
    {L"administration.tools.firewallAdvanced", 2, 72, 0xf5b4, L"exe:mmc.exe;WF.msc", L"tools:Windows Defender Firewall with Advanced Security",
     {L"Windows Defender Firewall with Advanced Security", L"Монитор брандмауэра Защитника Windows в режиме повышенной безопасности", L"Windows Defender Firewall con seguridad avanzada", L"Windows Defender Firewall mit erweiterter Sicherheit", L"Windows Defender Firewall com Segurança Avançada", L"高级安全 Windows Defender 防火墙"}},
    {L"administration.computerManagementVisible", 2, -1, 0xe977, L"native:10918", L"twinui.pcshell.dll,-10918",
     {L"Computer Management", L"Управление компьютером", L"Administración de equipos", L"Computerverwaltung", L"Gerenciamento do Computador", L"计算机管理"}},
    {L"administration.computerManagement.sharedFolders", 2, 76, 0xe8b7, L"exe:mmc.exe;fsmgmt.msc", L"filemgmt.dll,-3500",
     {L"Shared Folders", L"Общие папки", L"Carpetas compartidas", L"Freigegebene Ordner", L"Pastas compartilhadas", L"共享文件夹"}},
    {L"administration.computerManagement.localUsers", 2, 76, 0xe716, L"exe:mmc.exe;lusrmgr.msc", L"localsec.dll,-5026",
     {L"Local Users and Groups", L"Локальные пользователи и группы", L"Usuarios y grupos locales", L"Lokale Benutzer und Gruppen", L"Usuários e Grupos Locais", L"本地用户和组"}},
    {L"administration.computerManagement.wmi", 2, 76, 0xec7a, L"exe:mmc.exe;WmiMgmt.msc", L"wbem\\wbemcntl.dll,-143",
     {L"WMI Control", L"Управляющий элемент WMI", L"Control WMI", L"WMI-Steuerung", L"Controle WMI", L"WMI 控件"}},
    {L"administration.diskManagementVisible", 2, -1, 0xeda2, L"native:10917", L"twinui.pcshell.dll,-10917",
     {L"Disk Management", L"Управление дисками", L"Administración de discos", L"Datenträgerverwaltung", L"Gerenciamento de Disco", L"磁盘管理"}},
    {L"administration.diskManagement.optimize", 2, 80, 0xf0e2, L"exe:dfrgui.exe", L"tools:dfrgui",
     {L"Defragment and Optimize Drives", L"Оптимизация дисков", L"Desfragmentar y optimizar unidades", L"Laufwerke defragmentieren und optimieren", L"Desfragmentar e Otimizar Unidades", L"碎片整理和优化驱动器"}},
    {L"administration.diskManagement.cleanup", 2, 80, 0xea99, L"exe:cleanmgr.exe", L"tools:Disk Cleanup",
     {L"Disk Cleanup", L"Очистка диска", L"Liberador de espacio en disco", L"Datenträgerbereinigung", L"Limpeza de Disco", L"磁盘清理"}},
    {L"administration.diskManagement.recoveryDrive", 2, 80, 0xebc4, L"exe:RecoveryDrive.exe", L"tools:RecoveryDrive",
     {L"Recovery Drive", L"Диск восстановления", L"Unidad de recuperación", L"Wiederherstellungslaufwerk", L"Unidade de Recuperação", L"恢复驱动器"}},
    {L"administration.servicesVisible", 2, -1, 0xe9f5, L"exe:mmc.exe;services.msc", L"tools:services",
     {L"Services", L"Службы", L"Servicios", L"Dienste", L"Serviços", L"服务"}},
    {L"administration.services.taskScheduler", 2, 84, 0xe787, L"exe:mmc.exe;taskschd.msc", L"tools:Task Scheduler",
     {L"Task Scheduler", L"Планировщик задач", L"Programador de tareas", L"Aufgabenplanung", L"Agendador de Tarefas", L"任务计划程序"}},
    {L"administration.services.componentServices", 2, 84, 0xe950, L"exe:mmc.exe;comexp.msc", L"tools:Component Services",
     {L"Component Services", L"Службы компонентов", L"Servicios de componentes", L"Komponentendienste", L"Serviços de Componentes", L"组件服务"}},
    {L"administration.eventViewerVisible", 2, -1, 0xf0e3, L"native:10914", L"twinui.pcshell.dll,-10914",
     {L"Event Viewer", L"Просмотр событий", L"Visor de eventos", L"Ereignisanzeige", L"Visualizador de Eventos", L"事件查看器"}},
    {L"administration.eventViewer.performance", 2, 87, 0xf42f, L"exe:perfmon.exe", L"tools:Performance Monitor",
     {L"Performance Monitor", L"Системный монитор", L"Monitor de rendimiento", L"Leistungsüberwachung", L"Monitor de Desempenho", L"性能监视器"}},
    {L"administration.eventViewer.resources", 2, 87, 0xec49, L"exe:resmon.exe", L"tools:Resource Monitor",
     {L"Resource Monitor", L"Монитор ресурсов", L"Monitor de recursos", L"Ressourcenmonitor", L"Monitor de Recursos", L"资源监视器"}},
    {L"administration.eventViewer.reliability", 2, 87, 0xe9d9, L"exe:perfmon.exe;/rel", L"werconcpl.dll,-408",
     {L"Reliability Monitor", L"Монитор стабильности системы", L"Monitor de confiabilidad", L"Zuverlässigkeitsüberwachung", L"Monitor de Confiabilidade", L"可靠性监视程序"}},
    {L"administration.eventViewer.systemInfo", 2, 87, 0xe946, L"exe:msinfo32.exe", L"tools:System Information",
     {L"System Information", L"Сведения о системе", L"Información del sistema", L"Systeminformationen", L"Informações do sistema", L"系统信息"}},
    {L"administration.eventViewer.memoryDiagnostic", 2, 87, 0xeea0, L"exe:MdSched.exe", L"tools:Memory Diagnostics Tool",
     {L"Windows Memory Diagnostic", L"Средство проверки памяти Windows", L"Diagnóstico de memoria de Windows", L"Windows-Speicherdiagnose", L"Diagnóstico de Memória do Windows", L"Windows 内存诊断"}},
    {L"administration.registryVisible", 2, -1, 0xeca5, L"exe:regedit.exe", L"tools:Registry Editor",
     {L"Registry Editor", L"Редактор реестра", L"Editor del Registro", L"Registrierungs-Editor", L"Editor do Registro", L"注册表编辑器"}},
    {L"administration.registry.systemConfiguration", 2, 93, 0xe912, L"exe:msconfig.exe", L"tools:System Configuration",
     {L"System Configuration", L"Конфигурация системы", L"Configuración del sistema", L"Systemkonfiguration", L"Configuração do Sistema", L"系统配置"}},
    {L"administration.registry.groupPolicy", 2, 93, 0xe8a6, L"exe:mmc.exe;gpedit.msc", L"gpedit.dll,-337",
     {L"Local Group Policy Editor", L"Редактор локальной групповой политики", L"Editor de directivas de grupo local", L"Editor für lokale Gruppenrichtlinien", L"Editor de Política de Grupo Local", L"本地组策略编辑器"}},
    {L"administration.registry.localSecurity", 2, 93, 0xe72e, L"exe:mmc.exe;secpol.msc", L"tools:Security Configuration Management",
     {L"Local Security Policy", L"Локальная политика безопасности", L"Directiva de seguridad local", L"Lokale Sicherheitsrichtlinie", L"Política de Segurança Local", L"本地安全策略"}},
    {L"administration.terminalVisible", 2, -1, 0xe756, L"native:10960", L"",
     {L"Terminal", L"Терминал", L"Terminal", L"Terminal", L"Terminal", L"终端"}},
    {L"administration.terminal.terminalAdmin", 2, 97, 0xe7ef, L"native:10961", L"",
     {L"Terminal (Admin)", L"Терминал (Администратор)", L"Terminal (Administrador)", L"Terminal (Administrator)", L"Terminal (Administrador)", L"终端管理员"}},
    {L"administration.terminal.powershell", 2, 97, 0xe943, L"exe:WindowsPowerShell/v1.0/powershell.exe", L"twinui.pcshell.dll,-10928",
     {L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell", L"Windows PowerShell"}},
    {L"administration.terminal.commandPrompt", 2, 97, 0xe756, L"exe:cmd.exe", L"twinui.pcshell.dll,-10919",
     {L"Command Prompt", L"Командная строка", L"Símbolo del sistema", L"Eingabeaufforderung", L"Prompt de Comando", L"命令提示符"}},
    {L"commands.run", 3, -1, 0xe8ad, L"native:10925", L"",
     {L"Run", L"Выполнить", L"Ejecutar", L"Ausführen", L"Executar", L"运行"}},
    {L"commands.taskManager", 3, -1, 0xf120, L"native:10921", L"",
     {L"Task Manager", L"Диспетчер задач", L"Administrador de tareas", L"Task-Manager", L"Gerenciador de Tarefas", L"任务管理器"}},
    {L"commands.explorerVisible", 3, -1, 0xec51, L"native:10923", L"",
     {L"File Explorer", L"Проводник", L"Explorador de archivos", L"Explorer", L"Explorador de Arquivos", L"文件资源管理器"}},
    {L"commands.explorer.desktop", 3, 103, 0xe7fb, L"folder:B4BFCC3A-DB2C-424C-B029-7FE99A87C641", L"shell32.dll,-21769",
     {L"Desktop", L"Рабочий стол", L"Escritorio", L"Desktop", L"Área de Trabalho", L"桌面"}},
    {L"commands.explorer.computer", 3, 103, 0xe770, L"folder:0AC0837C-BBF8-452A-850D-79D08E667CA7", L"windows.storage.dll,-9216",
     {L"This PC", L"Этот компьютер", L"Este equipo", L"Dieser PC", L"Este Computador", L"此电脑"}},
    {L"commands.explorer.personal", 3, 103, 0xec25, L"folder:F3CE0F7C-4901-4ACC-8648-D5D44B04EF8F", L"shell32.dll,-22076",
     {L"Personal Folder", L"Личная папка", L"Carpeta personal", L"Persönlicher Ordner", L"Pasta Pessoal", L"个人文件夹"}},
    {L"commands.explorer.documents", 3, 103, 0xe8a5, L"folder:FDD39AD0-238F-46AF-ADB4-6C85480369C7", L"windows.storage.dll,-21770",
     {L"Documents", L"Документы", L"Documentos", L"Dokumente", L"Documentos", L"文档"}},
    {L"commands.explorer.pictures", 3, 103, 0xe91b, L"folder:33E28130-4E1E-4676-835A-98395C3BC3BB", L"windows.storage.dll,-21779",
     {L"Pictures", L"Изображения", L"Imágenes", L"Bilder", L"Imagens", L"图片"}},
    {L"commands.explorer.downloads", 3, 103, 0xe896, L"folder:374DE290-123F-4565-9164-39C4925E467B", L"windows.storage.dll,-21798",
     {L"Downloads", L"Загрузки", L"Descargas", L"Downloads", L"Downloads", L"下载"}},
    {L"commands.explorer.recycleBin", 3, 103, 0xe74d, L"folder:B7534046-3ECB-4C18-BE4E-64CD4CB7D6AC", L"shell32.dll,-8964",
     {L"Recycle Bin", L"Корзина", L"Papelera de reciclaje", L"Papierkorb", L"Lixeira", L"回收站"}},
    {L"commands.find", 3, -1, 0xe721, L"native:10924", L"",
     {L"Search", L"Найти", L"Buscar", L"Suchen", L"Pesquisar", L"搜索"}},
    {L"commands.desktop", 3, -1, 0xe7fb, L"native:10926", L"",
     {L"Desktop", L"Рабочий стол", L"Escritorio", L"Desktop", L"Área de Trabalho", L"桌面"}},
    {L"commands.powerVisible", 3, -1, 0xe7e8, L"power:menu", L"",
     {L"Shut down or sign out", L"Завершение работы или выход из системы", L"Apagar o cerrar sesión", L"Herunterfahren oder abmelden", L"Desligar ou sair", L"关机或注销"}},
    {L"commands.power.lock", 3, 113, 0xe72e, L"lock:session", L"pri:windows.ui.logon/resources/Lock/Text",
     {L"Lock", L"Заблокировать", L"Bloquear", L"Sperren", L"Bloquear", L"锁定"}},
    {L"commands.power.signOut", 3, 113, 0xf3b1, L"power:signOut", L"shell32.dll,-51523",
     {L"Sign out", L"Выход", L"Cerrar sesión", L"Abmelden", L"Sair", L"注销"}},
    {L"commands.power.sleep", 3, 113, 0xe708, L"power:sleep", L"authui.dll,-3019",
     {L"Sleep", L"Спящий режим", L"Suspender", L"Energie sparen", L"Suspender", L"睡眠"}},
    {L"commands.power.hibernate", 3, 113, 0xe823, L"power:hibernate", L"authui.dll,-3022",
     {L"Hibernate", L"Гибернация", L"Hibernar", L"Ruhezustand", L"Hibernar", L"休眠"}},
    {L"commands.power.shutdown", 3, 113, 0xe7e8, L"power:shutdown", L"authui.dll,-3013",
     {L"Shut down", L"Завершение работы", L"Apagar", L"Herunterfahren", L"Desligar", L"关机"}, {3026, 3150}},
    {L"commands.power.restart", 3, 113, 0xe777, L"power:restart", L"authui.dll,-3016",
     {L"Restart", L"Перезагрузка", L"Reiniciar", L"Neu starten", L"Reiniciar", L"重启"}, {3030, 3152}},
};

// The estimated-duration resources use unsigned integers in otherwise exact captions.
bool MatchesPowerCaption(std::wstring_view caption, std::wstring_view pattern) {
    if (pattern.empty()) return false;
    for (auto slot = pattern.find(L"%u"); slot != pattern.npos; slot = pattern.find(L"%u")) {
        if (!caption.starts_with(pattern.substr(0, slot))) return false;
        caption.remove_prefix(slot);
        const auto digits = caption.find_first_not_of(L"0123456789");
        if (digits == 0) return false;
        caption.remove_prefix(digits == caption.npos ? caption.size() : digits);
        pattern.remove_prefix(slot + 2);
    }
    return caption == pattern;
}

enum class PowerPlacement { Hidden, Main, Submenu };

constexpr PowerPlacement PlacePowerAction(bool withUpdate, bool primary, bool enabled) {
    if (primary && !withUpdate) return PowerPlacement::Main;
    return enabled ? PowerPlacement::Submenu : PowerPlacement::Hidden;
}
class ModuleWorkQueue {
public:
    bool Start() {
        const std::lock_guard guard(lock_);
        group_ = CreateThreadpoolCleanupGroup();
        if (!group_) return false;
        InitializeThreadpoolEnvironment(&environment_);
        SetThreadpoolCallbackCleanupGroup(&environment_, group_, nullptr);
        return true;
    }

    bool Submit(PTP_SIMPLE_CALLBACK callback, void* context = nullptr) {
        const std::lock_guard guard(lock_);
        return group_ && TrySubmitThreadpoolCallback(callback, context, &environment_);
    }

    void Stop() {
        PTP_CLEANUP_GROUP group;
        {
            const std::lock_guard guard(lock_);
            group = std::exchange(group_, nullptr);
        }
        if (!group) return;
        // Run queued callbacks so they can release their own context.
        CloseThreadpoolCleanupGroupMembers(group, FALSE, nullptr);
        CloseThreadpoolCleanupGroup(group);
        DestroyThreadpoolEnvironment(&environment_);
    }

private:
    std::mutex lock_;
    PTP_CLEANUP_GROUP group_{};
    TP_CALLBACK_ENVIRON environment_{};
};

enum class HoverMode { Immediate, Delayed, Arrow, ArrowClick };
enum class HoverZone { Outside, Body, Arrow };

struct Settings {
    HoverMode mode{};
    bool hideAdminCommands{};
    bool showHandCursor{};
    bool terminalAsAdmin{};
    std::wstring primaryPowerAction;
    std::array<bool, std::size(sectionEnabledKeys)> enabledSections{};
    std::array<bool, std::size(catalog)> enabled{};
};

struct MenuSession;

struct Destination {
    CatalogItem const* item{};
    C::MenuFlyoutItem original{nullptr};
};

struct RowInteraction {
    MenuSession* owner{};
    Destination destination;
    C::MenuFlyoutSubItem split{nullptr};
    HoverMode mode{};
    C::Grid layout{nullptr};
    X::FrameworkElement chevron{nullptr};
    C::Canvas arrowLayer{nullptr};
    C::Border arrowBackground{nullptr};
    X::VisualStateGroup commonStates{nullptr};
    AV::IExpandCollapseProvider expand{nullptr};
    X::DispatcherTimer hoverUpdate{nullptr};
    HoverZone zone{HoverZone::Outside};
    bool openOnUpdate{};
    std::optional<uint32_t> pressedPointer;
    HoverZone pressedZone{HoverZone::Outside};
    X::UIElement::PreviewKeyDown_revoker keyDown;
    X::UIElement::PointerPressed_revoker pointerPressed;
    X::UIElement::PointerReleased_revoker pointerReleased;
    X::UIElement::PointerMoved_revoker pointerMoved;
    X::UIElement::PointerEntered_revoker pointerEntered;
    X::UIElement::PointerExited_revoker pointerExited;
    X::UIElement::PointerCanceled_revoker pointerCanceled;
    X::VisualStateGroup::CurrentStateChanged_revoker visualStateChanged;
    X::DispatcherTimer::Tick_revoker tick;
};

struct CursorEvents {
    X::UIElement::PointerEntered_revoker entered;
    X::UIElement::PointerMoved_revoker moved;
    X::UIElement::PointerExited_revoker exited;
};

struct MenuSession {
    C::MenuFlyout menu{nullptr};
    std::vector<C::MenuFlyoutItemBase> original;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<C::MenuFlyoutItemBase> originalPower;
    winrt::Windows::Foundation::IInspectable oldPowerIcon{nullptr};
    bool powerChanged{};
    bool menuChanged{};
    bool closing{};
    HCURSOR commandCursor{};
    HCURSOR previousCursor{};
    X::UIElement cursorElement{nullptr};
    std::vector<std::unique_ptr<RowInteraction>> rows;
    std::vector<C::MenuFlyoutItem::Click_revoker> clicks;
    std::vector<CursorEvents> cursorEvents;
    P::FlyoutBase::Opened_revoker opened;
    P::FlyoutBase::Closing_revoker closingEvent;
    P::FlyoutBase::Closed_revoker closed;
};

[[clang::no_destroy]] std::unique_ptr<MenuSession> session;
std::mutex captionLock;
std::array<std::wstring, std::size(catalog)> systemCaptionCache;
std::mutex settingsLock;
Settings settings;
std::atomic<bool> unloading;
ModuleWorkQueue moduleWorkQueue;
std::atomic<unsigned> settingsNavigationGeneration;
HWND trayWindow;
std::atomic<DWORD> trayThread;
UINT setupMessage;
bool subclassPresent;
constexpr DWORD taskbarRetryInterval = 250;
constexpr DWORD taskbarSlowRetryInterval = 2'000;
constexpr ULONGLONG taskbarFastRetryDuration = 60'000;
PTP_TIMER taskbarStartupTimer;
ULONGLONG taskbarFastRetryDeadline;
SRWLOCK taskbarStartupLock = SRWLOCK_INIT;

using ShowAt = HRESULT(WINAPI*)(void*, void*);
using ShowAtOptions = HRESULT(WINAPI*)(void*, void*, void*);
ShowAt originalShowAt;
ShowAtOptions originalShowAtOptions;

template<typename Function>
void OnRow(RowInteraction* row, Function&& function) noexcept {
    if (unloading || !session || session.get() != row->owner) return;
    try {
        function(*row);
    } catch (...) {
        Wh_Log(L"Menu input handler failed");
    }
}

void LoadSettings() {
    Settings next;
    const auto value = WindhawkUtils::StringSetting::make(L"general.submenuHover");
    const std::wstring_view option = value.get();
    next.mode = option == L"delayed" ? HoverMode::Delayed :
                option == L"arrow" ? HoverMode::Arrow :
                option == L"click" ? HoverMode::ArrowClick : HoverMode::Immediate;
    next.hideAdminCommands = Wh_GetIntSetting(L"general.hideAdminCommands") != 0;
    next.showHandCursor = Wh_GetIntSetting(L"general.showHandCursor") != 0;
    next.terminalAsAdmin = Wh_GetIntSetting(L"commandOptions.terminalAsAdmin") != 0;
    const auto power = WindhawkUtils::StringSetting::make(L"commandOptions.primaryPowerAction");
    const std::wstring_view action = power.get();
    if (action == L"lock") next.primaryPowerAction = L"lock:session";
    else if (!action.empty() && action != L"none") next.primaryPowerAction = L"power:" + std::wstring(action);
    for (size_t group = 0; group < std::size(sectionEnabledKeys); ++group)
        next.enabledSections[group] = Wh_GetIntSetting(sectionEnabledKeys[group]) != 0;
    for (size_t i = 0; i < std::size(catalog); ++i)
        next.enabled[i] = Wh_GetIntSetting(catalog[i].key) != 0;
    std::lock_guard guard(settingsLock);
    settings = std::move(next);
}

Settings ReadSettings() {
    std::lock_guard guard(settingsLock);
    return settings;
}

std::wstring MenuCaption(std::wstring_view text) {
    // Chinese Win+X resources put the access key at the end, for example (&R).
    auto suffix = text.rfind(L'(');
    if (suffix != text.npos && text.ends_with(L')')) {
        auto key = text.substr(suffix + 1, text.size() - suffix - 2);
        if (key.starts_with(L'&')) key.remove_prefix(1);
        if (key.size() == 1 && key.front() >= L'A' && key.front() <= L'Z') {
            while (suffix && text[suffix - 1] == L' ') --suffix;
            text = text.substr(0, suffix);
        }
    }
    std::wstring caption(text);
    std::erase(caption, L'&');
    return caption;
}

std::wstring Normalize(std::wstring_view text) {
    auto normalized = MenuCaption(text);
    std::erase_if(normalized, [](wchar_t ch) { return ch == L'\u200e' || ch == L'\u200f'; });
    CharLowerBuffW(normalized.data(), static_cast<DWORD>(normalized.size()));
    return normalized;
}

std::wstring SystemPath(std::wstring_view name, bool windowsRoot = false) {
    wchar_t directory[MAX_PATH]{};
    const auto size = windowsRoot ? GetWindowsDirectoryW(directory, ARRAYSIZE(directory)) :
                                    GetSystemDirectoryW(directory, ARRAYSIZE(directory));
    if (!size || size >= ARRAYSIZE(directory)) return {};
    auto path = std::wstring(directory) + L'\\' + std::wstring(name);
    std::replace(path.begin(), path.end(), L'/', L'\\');
    return path;
}

std::wstring Indirect(PCWSTR token, HRESULT* resultCode = nullptr) {
    wchar_t result[512]{};
    const auto status = SHLoadIndirectString(token, result, ARRAYSIZE(result), nullptr);
    if (resultCode) *resultCode = status;
    if (FAILED(status)) return {};
    return result;
}

std::wstring SystemString(std::wstring_view reference) {
    const auto token = L"@%SystemRoot%\\System32\\" + std::wstring(reference);
    auto result = Indirect(token.c_str());
    if (reference.starts_with(L"twinui.pcshell.dll,")) return MenuCaption(result);
    return result;
}

std::wstring Twinui(std::wstring_view id) {
    return SystemString(L"twinui.pcshell.dll,-" + std::wstring(id));
}

size_t DisplayLanguage() {
    wchar_t locale[LOCALE_NAME_MAX_LENGTH]{};
    if (!LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT), locale, ARRAYSIZE(locale), 0))
        return 0;
    const std::wstring_view code(locale);
    constexpr std::array<std::wstring_view, 6> languages = {L"en", L"ru", L"es", L"de", L"pt", L"zh"};
    for (size_t i = 0; i < languages.size(); ++i)
        if (code.starts_with(languages[i])) return i;
    return 0;
}

struct CoTaskMemory {
    void operator()(void* pointer) const noexcept { CoTaskMemFree(pointer); }
};

using Pidl = std::unique_ptr<ITEMIDLIST, CoTaskMemory>;

Pidl FolderId(std::wstring_view guid) {
    GUID id{};
    const auto literal = L"{" + std::wstring(guid) + L"}";
    if (FAILED(CLSIDFromString(literal.c_str(), &id))) return {};
    PIDLIST_ABSOLUTE result{};
    if (FAILED(SHGetKnownFolderIDList(id, KF_FLAG_DONT_VERIFY, nullptr, &result))) return {};
    return Pidl(result);
}

Pidl ShellId(std::wstring_view name) {
    PIDLIST_ABSOLUTE id{};
    if (FAILED(SHParseDisplayName(std::wstring(name).c_str(), nullptr, &id, 0, nullptr))) return {};
    return Pidl(id);
}

std::wstring ShellName(Pidl const& pidl) {
    PWSTR result{};
    if (!pidl || FAILED(SHGetNameFromIDList(pidl.get(), SIGDN_NORMALDISPLAY, &result))) return {};
    std::unique_ptr<wchar_t, CoTaskMemory> text(result);
    return text.get();
}

std::wstring ControlPath(std::wstring_view category) {
    return L"::{26EE0668-A00A-44D7-9371-BEB064C98683}\\" + std::wstring(category);
}

std::wstring ProgramPath(std::wstring_view program) {
    return SystemPath(program, program == L"regedit.exe");
}

std::pair<std::wstring_view, std::wstring_view> Split(std::wstring_view value, wchar_t divider) {
    const size_t pos = value.find(divider);
    return {value.substr(0, pos), pos == value.npos ? std::wstring_view{} : value.substr(pos + 1)};
}

std::wstring RegistryShellName(std::wstring_view guid) {
    const auto key = L"CLSID\\" + std::wstring(guid);
    wchar_t value[512]{};
    DWORD bytes = sizeof(value);
    if (RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), L"LocalizedString", RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                     nullptr, value, &bytes) == ERROR_SUCCESS) {
        if (auto name = Indirect(value); !name.empty()) return name;
    }
    return ShellName(ShellId(ControlPath(L"0") + L"\\::" + std::wstring(guid)));
}

std::wstring ToolsName(std::wstring_view shortcut) {
    PWSTR result{};
    if (FAILED(SHGetKnownFolderPath(FOLDERID_CommonAdminTools, KF_FLAG_DONT_VERIFY, nullptr, &result))) return {};
    std::unique_ptr<wchar_t, CoTaskMemory> root(result);
    const auto directory = std::wstring(root.get()) + L"\\";
    const auto fileName = std::wstring(shortcut) + L".lnk";
    const auto ini = directory + L"desktop.ini";
    wchar_t resource[512]{};
    if (GetPrivateProfileStringW(L"LocalizedFileNames", fileName.c_str(), L"", resource,
                                 ARRAYSIZE(resource), ini.c_str())) {
        if (auto title = Indirect(resource); !title.empty()) return title;
    }
    return ShellName(ShellId(directory + fileName));
}

constexpr bool PreferLegacyDeveloperTitle(DWORD build, std::optional<DWORD> revision);
DWORD ReadWindowsBuild();
std::optional<DWORD> ReadWindowsRevision();

std::wstring SettingsResource(std::wstring_view page, std::wstring_view field, HRESULT* resultCode = nullptr) {
    const auto token = L"@{windows?ms-resource://Windows.UI.SettingsAppThreshold/SystemSettings/Resources/" +
                       std::wstring(page) + L"/" + std::wstring(field) + L"}";
    return Indirect(token.c_str(), resultCode);
}

HRESULT ReadDeviceKeyboardResourceStatus() {
    HRESULT status;
    SettingsResource(L"SettingsPageDevicesKeyboard", L"DisplayName", &status);
    return status;
}

std::wstring SettingsCaption(std::wstring_view id) {
    // Windows uses the short title on systems without a battery.
    if (id == L"SettingsPagePowerAndBattery") {
        SYSTEM_POWER_STATUS status{};
        if (GetSystemPowerStatus(&status) && status.BatteryFlag == 128) {
            if (auto title = SettingsResource(id, L"AlternateDisplayName"); !title.empty()) return title;
        }
    }
    if (id == L"SettingsPageAdvancedSystemSettings") {
        constexpr std::wstring_view legacyId = L"SettingsPageRestoreDeveloperOptions";
        const auto build = ReadWindowsBuild();
        const auto revision = build == 26100 ? ReadWindowsRevision() : std::nullopt;
        if (PreferLegacyDeveloperTitle(build, revision)) {
            if (auto title = SettingsResource(legacyId, L"DisplayName"); !title.empty()) return title;
        }
        if (auto title = SettingsResource(id, L"DisplayName"); !title.empty()) return title;
        return SettingsResource(legacyId, L"DisplayName");
    }
    return SettingsResource(id, L"DisplayName");
}

std::wstring SystemCaption(CatalogItem const& item) {
    const auto [kind, target] = Split(item.action, L':');
    const auto [resourceKind, resource] = Split(item.resource, L':');
    if (resourceKind == L"shell") return RegistryShellName(resource);
    if (resourceKind == L"app") return ShellName(ShellId(L"shell:AppsFolder\\" + std::wstring(resource)));
    if (resourceKind == L"tools") {
        if (resource == L"Windows Tools") return RegistryShellName(L"{D20EA4E1-3957-11D2-A40B-0C5020524153}");
        return ToolsName(resource);
    }
    if (resourceKind == L"settings") return SettingsCaption(resource);
    if (resourceKind == L"pri") {
        return Indirect((L"@{windows?ms-resource://" + std::wstring(resource) + L"}").c_str());
    }
    if (*item.resource) return SystemString(item.resource);
    if (kind == L"folder") return ShellName(FolderId(target));
    if (kind == L"native") return Twinui(target);
    return {};
}

std::wstring Caption(Destination const& destination) {
    if (destination.original) return MenuCaption(destination.original.Text());
    const auto& item = *destination.item;
    {
        const std::lock_guard guard(captionLock);
        const auto& cached = systemCaptionCache[&item - catalog];
        if (!cached.empty()) return cached;
    }
    return item.names[DisplayLanguage()];
}

void CALLBACK WarmSystemCaptions(PTP_CALLBACK_INSTANCE, PVOID) noexcept {
    if (unloading) return;
    const auto initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initialized)) return;
    for (size_t index = 0; index < std::size(catalog) && !unloading; ++index) {
        try {
            auto caption = SystemCaption(catalog[index]);
            if (!caption.empty() && !unloading) {
                const std::lock_guard guard(captionLock);
                systemCaptionCache[index] = std::move(caption);
            }
        } catch (...) {
            // The catalog caption remains available.
        }
    }
    CoUninitialize();
}

template<typename Provider>
Provider GetProvider(X::UIElement const& item, AP::PatternInterface pattern) {
    auto peer = AP::FrameworkElementAutomationPeer::CreatePeerForElement(item);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

DWORD LaunchWithShell(SHELLEXECUTEINFOW& info) {
    info.cbSize = sizeof(info);
    info.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&info)) return ERROR_SUCCESS;
    const auto error = GetLastError();
    return error ? error : ERROR_GEN_FAILURE;
}

DWORD Launch(PCWSTR target, PCWSTR arguments = nullptr) {
    SHELLEXECUTEINFOW info{};
    info.lpVerb = L"open";
    info.lpFile = target;
    info.lpParameters = arguments;
    return LaunchWithShell(info);
}

DWORD LaunchPidl(Pidl const& target) {
    if (!target) return ERROR_FILE_NOT_FOUND;
    SHELLEXECUTEINFOW info{};
    info.fMask = SEE_MASK_IDLIST;
    info.lpIDList = target.get();
    return LaunchWithShell(info);
}

struct SettingsRoute {
    unsigned generation;
    std::wstring pageElementId;
    std::wstring appName;
};

winrt::com_ptr<IUIAutomationCondition> TextCondition(IUIAutomation& automation, PROPERTYID property, PCWSTR text) {
    VARIANT value{};
    value.vt = VT_BSTR;
    value.bstrVal = SysAllocString(text);
    winrt::com_ptr<IUIAutomationCondition> condition;
    if (value.bstrVal) automation.CreatePropertyCondition(property, value, condition.put());
    VariantClear(&value);
    return condition;
}

winrt::com_ptr<IUIAutomationInvokePattern> ParentBreadcrumb(IUIAutomationElement& bar,
                                                           IUIAutomationCondition& crumbType,
                                                           IUIAutomationCacheRequest& cache) {
    winrt::com_ptr<IUIAutomationElementArray> crumbs;
    if (FAILED(bar.FindAllBuildCache(TreeScope_Descendants, &crumbType, &cache, crumbs.put()))) {
        crumbs = nullptr;
        if (FAILED(bar.FindAll(TreeScope_Descendants, &crumbType, crumbs.put()))) return {};
    }
    int count{};
    winrt::com_ptr<IUIAutomationElement> parent;
    if (!crumbs || FAILED(crumbs->get_Length(&count)) || count != 2 ||
        FAILED(crumbs->GetElement(0, parent.put())) || !parent) return {};

    winrt::com_ptr<IUIAutomationInvokePattern> invoke;
    if (FAILED(parent->GetCachedPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                                          invoke.put_void())) || !invoke) {
        invoke = nullptr;
        if (FAILED(parent->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                                               invoke.put_void()))) return {};
    }
    return invoke;
}

void CALLBACK SelectSettingsCategory(PTP_CALLBACK_INSTANCE, PVOID data) noexcept {
    std::unique_ptr<SettingsRoute> route(static_cast<SettingsRoute*>(data));
    if (unloading || settingsNavigationGeneration != route->generation) return;
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initialized)) return;
    struct Apartment {
        ~Apartment() { CoUninitialize(); }
    } apartment;
    try {
        winrt::com_ptr<IUIAutomation2> automation;
        winrt::check_hresult(CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
                                             __uuidof(IUIAutomation2), automation.put_void()));
        winrt::check_hresult(automation->put_ConnectionTimeout(400));
        winrt::check_hresult(automation->put_TransactionTimeout(400));
        const auto pageId = TextCondition(*automation, UIA_AutomationIdPropertyId, route->pageElementId.c_str());
        const auto barId = TextCondition(*automation, UIA_AutomationIdPropertyId, L"PermanentNavigationViewBreadcrumbBar");
        const auto crumbType = TextCondition(*automation, UIA_ClassNamePropertyId,
                                            L"Microsoft.UI.Xaml.Controls.BreadcrumbBarItem");
        if (!pageId || !barId || !crumbType) return;
        winrt::com_ptr<IUIAutomationCacheRequest> crumbCache;
        winrt::check_hresult(automation->CreateCacheRequest(crumbCache.put()));
        winrt::check_hresult(crumbCache->AddPattern(UIA_InvokePatternId));

        // Wait for the URI's landing page, then invoke its parent breadcrumb.
        // Automation identifiers are independent of the Windows display language.
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(4);
        DWORD retryDelay = 75;
        while (!unloading && settingsNavigationGeneration == route->generation && std::chrono::steady_clock::now() < deadline) {
            const HWND hwnd = FindWindowW(L"ApplicationFrameWindow", route->appName.c_str());
            winrt::com_ptr<IUIAutomationElement> window, page, bar;
            if (hwnd && SUCCEEDED(automation->ElementFromHandle(hwnd, window.put())) && window &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, pageId.get(), page.put())) && page &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, barId.get(), bar.put())) && bar) {
                if (const auto invoke = ParentBreadcrumb(*bar, *crumbType, *crumbCache);
                    invoke && settingsNavigationGeneration == route->generation && !unloading &&
                    std::chrono::steady_clock::now() < deadline && SUCCEEDED(invoke->Invoke())) return;
            }
            if (unloading || settingsNavigationGeneration != route->generation) return;
            Sleep(retryDelay);
            retryDelay = std::min<DWORD>(retryDelay * 2, 200);
        }
    } catch (...) {
    }
}

void RunDestination(MenuSession& active, Destination destination) {
    const auto navigationGeneration = ++settingsNavigationGeneration;
    if (destination.original) {
        if (auto invoke = GetProvider<AV::IInvokeProvider>(destination.original, AP::PatternInterface::Invoke)) {
            auto menu = active.menu;
            invoke.Invoke();
            menu.Hide();
        } else Wh_Log(L"Could not invoke the original Win+X command %s", destination.item->key);
        return;
    }

    const auto& item = *destination.item;
    const auto [kind, target] = Split(item.action, L':');
    auto menu = active.menu;
    menu.Hide();
    DWORD error = ERROR_NOT_SUPPORTED;
    if (kind == L"uri" || kind == L"category") {
        const auto [page, pageElementId] = Split(target, L';');
        const auto address = L"ms-settings:" + std::wstring(page);
        error = Launch(address.c_str());
        if (error == ERROR_SUCCESS && kind == L"category") {
            auto route = std::make_unique<SettingsRoute>();
            route->generation = navigationGeneration;
            route->pageElementId = pageElementId;
            route->appName = Twinui(L"10922");
            if (route->appName.empty()) route->appName = L"Settings";
            auto* context = route.release();
            if (!moduleWorkQueue.Submit(SelectSettingsCategory, context)) {
                delete context;
                Wh_Log(L"Could not schedule Settings category navigation for %s", item.key);
            }
        }
    } else if (kind == L"folder") {
        error = LaunchPidl(FolderId(target));
    } else if (kind == L"panel") {
        error = LaunchPidl(ShellId(ControlPath(target)));
    } else if (kind == L"lock") {
        error = LockWorkStation() ? ERROR_SUCCESS : ERROR_GEN_FAILURE;
    } else if (kind == L"control") {
        const auto command = target.starts_with(L"Microsoft.") ? L"/name " + std::wstring(target) :
                                                                  std::wstring(target);
        error = Launch(SystemPath(L"control.exe").c_str(), command.c_str());
    } else if (kind == L"exe") {
        const auto [program, argument] = Split(target, L';');
        const auto path = ProgramPath(program);
        auto parameters = std::wstring(argument);
        if (argument.ends_with(L".msc")) parameters = L"\"" + SystemPath(argument) + L"\"";
        if (program.ends_with(L".cpl")) {
            parameters = L"\"" + path + L"\"";
            error = Launch(SystemPath(L"control.exe").c_str(), parameters.c_str());
        } else {
            error = Launch(path.c_str(), parameters.empty() ? nullptr : parameters.c_str());
        }
    }
    if (error != ERROR_SUCCESS && error != ERROR_CANCELLED)
        Wh_Log(L"Could not open Win+X command %s (Win32 error %lu)", item.key, error);
}

X::FrameworkElement FindNamed(X::DependencyObject const& node, std::wstring_view name) {
    auto element = node.try_as<X::FrameworkElement>();
    if (element && element.Name() == name) return element;
    for (int index = 0; index < X::Media::VisualTreeHelper::GetChildrenCount(node); ++index) {
        if (auto child = FindNamed(X::Media::VisualTreeHelper::GetChild(node, index), name)) return child;
    }
    return nullptr;
}

constexpr double arrowHitWidth = 32;

winrt::Windows::Foundation::Point ChevronCenter(RowInteraction const& active, X::UIElement const& relativeTo) {
    return active.chevron.TransformToVisual(relativeTo).TransformPoint({
        static_cast<float>(active.chevron.ActualWidth() / 2.0),
        static_cast<float>(active.chevron.ActualHeight() / 2.0),
    });
}

HoverZone PointerZone(RowInteraction const& active, I::PointerRoutedEventArgs const& args) {
    const auto x = args.GetCurrentPoint(active.layout).Position().X;
    return std::abs(x - ChevronCenter(active, active.layout).X) <= arrowHitWidth / 2.0 ?
        HoverZone::Arrow : HoverZone::Body;
}

void ClearCommandCursor(MenuSession& active) {
    if (!active.cursorElement) return;
    active.cursorElement = nullptr;
    if (GetCursor() == active.commandCursor) SetCursor(active.previousCursor);
}

void UpdateCommandCursor(MenuSession& active, X::UIElement const& element, bool command) {
    if (!active.commandCursor) return;
    if (!command) {
        if (active.cursorElement == element) ClearCommandCursor(active);
        return;
    }
    if (!active.cursorElement) active.previousCursor = GetCursor();
    active.cursorElement = element;
    if (GetCursor() != active.commandCursor) SetCursor(active.commandCursor);
}

void UpdateHoverVisual(RowInteraction const& active) {
    if (!active.arrowBackground) return;
    if (active.zone != HoverZone::Arrow) {
        active.arrowBackground.Opacity(0);
        if (active.zone == HoverZone::Body)
            X::VisualStateManager::GoToState(active.split, L"PointerOver", false);
        return;
    }

    if (!active.arrowBackground.Background()) {
        X::VisualStateManager::GoToState(active.split, L"PointerOver", false);
        active.arrowBackground.Background(active.layout.Background());
    }
    const auto row = active.layout.TransformToVisual(active.arrowLayer).TransformBounds({
        0, 0, static_cast<float>(active.layout.ActualWidth()), static_cast<float>(active.layout.ActualHeight()),
    });
    const double center = ChevronCenter(active, active.arrowLayer).X;
    const double width = (std::min)({static_cast<double>(row.Height),
                                    2 * (center - row.X), 2 * (row.X + row.Width - center)});
    if (width <= 0 || !active.arrowBackground.Background()) return;

    active.arrowBackground.Width(width);
    active.arrowBackground.Height(row.Height);
    C::Canvas::SetLeft(active.arrowBackground, center - width / 2);
    C::Canvas::SetTop(active.arrowBackground, row.Y);
    // Only CommonStates changes here. The native focus visuals stay active.
    X::VisualStateManager::GoToState(active.split, L"Normal", false);
    active.arrowBackground.Opacity(1);
}

void SetHoverZone(RowInteraction& active, HoverZone zone) {
    if (active.owner->closing) return;
    if (std::exchange(active.zone, zone) == zone) return;
    active.openOnUpdate = active.expand &&
        ((active.mode == HoverMode::Immediate && zone != HoverZone::Outside) ||
         (active.mode == HoverMode::Arrow && zone == HoverZone::Arrow));
    UpdateHoverVisual(active);
    if (active.openOnUpdate) active.hoverUpdate.Start();
}

void UpdatePointerZone(RowInteraction& active, I::PointerRoutedEventArgs const& args) {
    const auto zone = PointerZone(active, args);
    if (args.Pointer().PointerDeviceType() == PointerType::Mouse)
        UpdateCommandCursor(*active.owner, active.split, active.destination.item && zone == HoverZone::Body);
    SetHoverZone(active, zone);
}

void AddArrowBackground(RowInteraction& active) {
    auto states = X::VisualStateManager::GetVisualStateGroups(active.layout);
    for (const auto& group : states) {
        if (group.Name() == L"CommonStates") {
            active.commonStates = group;
            break;
        }
    }
    if (!active.commonStates) return;

    // An absolutely positioned child of Canvas cannot grow the menu's grid.
    C::Canvas overlay;
    overlay.IsHitTestVisible(false);
    C::Grid::SetColumnSpan(overlay, (std::max)(1u, active.layout.ColumnDefinitions().Size()));
    C::Grid::SetRowSpan(overlay, (std::max)(1u, active.layout.RowDefinitions().Size()));
    C::Canvas::SetZIndex(overlay, -1);
    C::Border background;
    background.CornerRadius(active.layout.CornerRadius());
    background.Opacity(0);
    overlay.Children().Append(background);
    active.layout.Children().Append(overlay);
    active.arrowLayer = overlay;
    active.arrowBackground = background;
    active.visualStateChanged = active.commonStates.CurrentStateChanged(winrt::auto_revoke,
        [row = &active](auto const&, X::VisualStateChangedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (!active.owner->closing && active.zone == HoverZone::Arrow && args.NewState() &&
                    args.NewState().Name() != L"Normal")
                    active.hoverUpdate.Start();
            });
        });
}

bool AttachPointerEvents(RowInteraction& active) {
    active.split.ApplyTemplate();
    active.layout = FindNamed(active.split, L"LayoutRoot").try_as<C::Grid>();
    active.chevron = FindNamed(active.split, L"SubItemChevron");
    if (!active.layout || !active.chevron) return false;
    active.expand = GetProvider<AV::IExpandCollapseProvider>(active.split, AP::PatternInterface::ExpandCollapse);
    if (active.mode == HoverMode::ArrowClick && !active.expand) return false;
    if (active.mode != HoverMode::Delayed && active.expand) {
        // Let XAML finish routing the enter event before opening its submenu.
        active.hoverUpdate = X::DispatcherTimer();
        active.hoverUpdate.Interval(std::chrono::milliseconds(1));
        active.tick = active.hoverUpdate.Tick(winrt::auto_revoke, [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.hoverUpdate.Stop();
                if (active.owner->closing) return;
                const bool open = std::exchange(active.openOnUpdate, false);
                const auto provider = active.expand;
                const auto owner = active.owner;
                UpdateHoverVisual(active);
                if (open && session.get() == owner && !owner->closing && owner->menu.IsOpen() &&
                    provider.ExpandCollapseState() == X::Automation::ExpandCollapseState::Collapsed)
                    provider.Expand();
            });
        });
        if (active.mode == HoverMode::Arrow || active.mode == HoverMode::ArrowClick) AddArrowBackground(active);
    }

    active.pointerEntered = active.layout.PointerEntered(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                // In arrow-only mode we drive the native PointerOver state explicitly
                // because the XAML handler pairs it with starting the submenu timer.
                if (active.mode == HoverMode::ArrowClick || (active.mode == HoverMode::Arrow && active.expand))
                    args.Handled(true);
                if (args.Pointer().PointerDeviceType() == PointerType::Touch) return;
                UpdatePointerZone(active, args);
            });
        });
    active.pointerMoved = active.layout.PointerMoved(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (args.Pointer().PointerDeviceType() != PointerType::Touch)
                    UpdatePointerZone(active, args);
            });
        });
    active.pointerExited = active.layout.PointerExited(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.pressedPointer.reset();
                UpdateCommandCursor(*active.owner, active.split, false);
                SetHoverZone(active, HoverZone::Outside);
            });
        });
    if (!active.destination.item && active.mode != HoverMode::ArrowClick) return true;

    active.pointerPressed = active.layout.PointerPressed(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                const auto zone = PointerZone(active, args);
                if ((zone == HoverZone::Body ||
                     (zone == HoverZone::Arrow && active.mode == HoverMode::ArrowClick)) &&
                    (args.Pointer().PointerDeviceType() == PointerType::Touch ||
                     args.GetCurrentPoint(active.layout).Properties().IsLeftButtonPressed())) {
                    active.pressedPointer = args.Pointer().PointerId();
                    active.pressedZone = zone;
                    args.Handled(true);
                }
            });
        });
    active.pointerReleased = active.layout.PointerReleased(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                const bool pressedHere = std::exchange(active.pressedPointer, std::nullopt) == args.Pointer().PointerId();
                const auto zone = PointerZone(active, args);
                const bool clickOnly = active.mode == HoverMode::ArrowClick;
                if (pressedHere || (!clickOnly && zone == HoverZone::Body)) args.Handled(true);
                if (!pressedHere || active.owner->closing || active.pressedZone != zone) return;
                if (clickOnly && (zone == HoverZone::Arrow ||
                                  (zone == HoverZone::Body && !active.destination.item))) {
                    auto provider = active.expand;
                    if (provider.ExpandCollapseState() == X::Automation::ExpandCollapseState::Collapsed)
                        provider.Expand();
                } else if (zone == HoverZone::Body && active.destination.item) {
                    RunDestination(*active.owner, active.destination);
                }
            });
        });
    active.pointerCanceled = active.layout.PointerCanceled(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.pressedPointer.reset();
                UpdateCommandCursor(*active.owner, active.split, false);
            });
        });
    return true;
}

constexpr wchar_t iconFont[] = L"Segoe Fluent Icons";
constexpr wchar_t shieldTaskGlyph = L'\uf760';

bool IconFontHasGlyph(wchar_t glyph) noexcept {
    winrt::com_ptr<IDWriteFactory> factory;
    winrt::com_ptr<IDWriteFontCollection> fonts;
    winrt::com_ptr<IDWriteFontFamily> family;
    winrt::com_ptr<IDWriteFont> font;
    UINT32 index{};
    BOOL found{}, hasGlyph{};
    return SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                         reinterpret_cast<IUnknown**>(factory.put()))) &&
           SUCCEEDED(factory->GetSystemFontCollection(fonts.put(), FALSE)) &&
           SUCCEEDED(fonts->FindFamilyName(iconFont, &index, &found)) && found &&
           SUCCEEDED(fonts->GetFontFamily(index, family.put())) &&
           SUCCEEDED(family->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                                  DWRITE_FONT_STYLE_NORMAL, font.put())) &&
           SUCCEEDED(font->HasCharacter(glyph, &hasGlyph)) && hasGlyph;
}

C::FontIcon MakeIcon(wchar_t glyph) {
    if (glyph == shieldTaskGlyph) {
        static const bool hasShieldTask = IconFontHasGlyph(shieldTaskGlyph);
        if (!hasShieldTask) glyph = L'\uea18';
    }
    C::FontIcon icon;
    icon.FontFamily(X::Media::FontFamily(iconFont));
    icon.FontSize(16);
    icon.Glyph(winrt::hstring(std::wstring_view(&glyph, 1)));
    return icon;
}

struct StockPowerAction {
    Destination destination;
    bool withUpdate;
};

struct StockMenu {
    std::vector<C::MenuFlyoutItemBase> rows;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<StockPowerAction> powerActions;
};

C::MenuFlyoutItem StockAction(StockMenu const& stock, std::wstring_view id) {
    const auto matches = [&](std::wstring_view candidate) -> C::MenuFlyoutItem {
        const auto title = Normalize(Twinui(candidate));
        if (title.empty()) return nullptr;
        for (auto const& item : stock.rows)
            if (auto command = item.try_as<C::MenuFlyoutItem>(); command && Normalize(command.Text()) == title)
                return command;
        return nullptr;
    };
    if (auto action = matches(id)) return action;
    if (id == L"10960") return matches(L"10944");
    if (id == L"10961") return matches(L"10945");
    return nullptr;
}

struct StockPowerCaption {
    CatalogItem const* item;
    std::wstring caption;
};

std::vector<StockPowerCaption> ReadPowerCaptions(bool withUpdate) {
    std::vector<StockPowerCaption> captions;
    const auto add = [&](CatalogItem const& item, std::wstring_view caption) {
        if (!caption.empty()) captions.push_back({&item, Normalize(caption)});
    };
    for (auto const& item : catalog) {
        if (!std::wstring_view(item.action).starts_with(L"power:") || !*item.resource) continue;
        if (!withUpdate) {
            add(item, SystemString(item.resource));
            // Some stock captions differ from the system resource, for example Russian Sign out.
            add(item, item.names[DisplayLanguage()]);
            continue;
        }
        const auto [plainId, estimatedId] = item.updateCaptions;
        if (!plainId) continue;
        const auto addResource = [&](unsigned id) {
            add(item, SystemString(L"shutdownux.dll,-" + std::to_wstring(id)));
        };
        addResource(plainId);
        // Each of the eight estimates has an access-key resource next to it.
        // Shut down and restart share the block, with four IDs per estimate.
        for (unsigned offset = 0; offset < 32; offset += 4) addResource(estimatedId + offset);
    }
    return captions;
}

bool ReadStockPowerActions(StockMenu& stock) {
    static const auto regular = ReadPowerCaptions(false);
    for (auto const& row : stock.power.Items()) {
        if (row.try_as<C::MenuFlyoutSeparator>()) continue;
        const auto command = row.try_as<C::MenuFlyoutItem>();
        if (!command) return false;
        const auto caption = Normalize(command.Text());
        if (caption.empty()) return false;

        const auto matches = [&](StockPowerCaption const& entry) { return MatchesPowerCaption(caption, entry.caption); };
        auto const* candidates = &regular;
        auto found = std::ranges::find_if(*candidates, matches);
        const bool withUpdate = found == candidates->end();
        if (withUpdate) {
            static const auto updates = ReadPowerCaptions(true);
            candidates = &updates;
            found = std::ranges::find_if(*candidates, matches);
        }
        if (found == candidates->end()) return false;
        const auto* item = found->item;
        if (std::any_of(found + 1, candidates->end(), [&](StockPowerCaption const& entry) {
                return entry.item != item && matches(entry);
            }) || std::ranges::any_of(stock.powerActions, [&](StockPowerAction const& entry) {
                return entry.destination.item == item && entry.withUpdate == withUpdate;
            })) {
            return false;
        }
        stock.powerActions.push_back({{item, command}, withUpdate});
    }
    return !stock.powerActions.empty();
}

bool ExplorerStartsOnThisPc() {
    DWORD destination{};
    DWORD bytes = sizeof(destination);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                        L"LaunchTo", RRF_RT_REG_DWORD, nullptr, &destination, &bytes) == ERROR_SUCCESS &&
           destination == 1;
}

bool IsHomeEdition() {
    DWORD edition{};
    if (!GetProductInfo(10, 0, 0, 0, &edition)) return false;
    return edition == PRODUCT_CORE || edition == PRODUCT_CORE_N ||
           edition == PRODUCT_CORE_SINGLELANGUAGE || edition == PRODUCT_CORE_COUNTRYSPECIFIC;
}

struct AvailabilitySnapshot {
    std::array<bool, std::size(catalog)> unavailable{};
    std::array<bool, std::size(catalog)> credentialsRequired{};
};

constexpr PCWSTR uacPolicyKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
constexpr PCWSTR settingsPolicyKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";

constexpr bool ConfirmedMissingError(LSTATUS status) {
    return status == ERROR_FILE_NOT_FOUND || status == ERROR_PATH_NOT_FOUND;
}

bool MissingFile(std::wstring const& path) {
    if (path.empty()) return false;
    const auto attributes = GetFileAttributesW(path.c_str());
    return attributes == INVALID_FILE_ATTRIBUTES ? ConfirmedMissingError(GetLastError()) :
                                                   (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool MissingControlRegistration(std::wstring_view resource) {
    const auto [kind, guid] = Split(resource, L':');
    if (kind != L"shell") return false;
    const auto key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\" +
                     std::wstring(guid);
    HKEY handle{};
    const auto status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_QUERY_VALUE, &handle);
    if (status == ERROR_SUCCESS) RegCloseKey(handle);
    return ConfirmedMissingError(status);
}

std::optional<DWORD> ReadMachineDword(PCWSTR key, PCWSTR name) {
    DWORD value{};
    DWORD bytes = sizeof(value);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, key, name, RRF_RT_REG_DWORD, nullptr, &value, &bytes) != ERROR_SUCCESS)
        return {};
    return value;
}

enum class SettingsPolicyMode { None, Hide, ShowOnly, Unknown };

struct SettingsPolicy {
    SettingsPolicyMode mode;
    std::vector<std::wstring> pages;
};

SettingsPolicy ParseSettingsPolicy(std::wstring value) {
    for (auto& ch : value) {
        if (ch >= L'A' && ch <= L'Z') ch += L'a' - L'A';
    }
    auto [prefix, list] = Split(value, L':');
    SettingsPolicy policy{SettingsPolicyMode::Unknown, {}};
    if (prefix == L"hide") policy.mode = SettingsPolicyMode::Hide;
    else if (prefix == L"showonly") policy.mode = SettingsPolicyMode::ShowOnly;
    else return policy;

    do {
        const auto [page, rest] = Split(list, L';');
        if (page.empty() || !std::all_of(page.begin(), page.end(), [](wchar_t ch) {
                return (ch >= L'a' && ch <= L'z') || (ch >= L'0' && ch <= L'9') || ch == L'-' || ch == L'_';
            })) return {SettingsPolicyMode::Unknown, {}};
        policy.pages.emplace_back(page);
        if (page.size() == list.size()) return policy;
        list = rest;
    } while (true);
}

SettingsPolicy ReadSettingsPolicy(HKEY hive, PCWSTR key, PCWSTR name) {
    DWORD bytes{};
    auto status = RegGetValueW(hive, key, name, RRF_RT_REG_SZ, nullptr, nullptr, &bytes);
    if (ConfirmedMissingError(status)) return {SettingsPolicyMode::None, {}};
    if (status != ERROR_SUCCESS || bytes < sizeof(wchar_t) || bytes > 32768 || bytes % sizeof(wchar_t))
        return {SettingsPolicyMode::Unknown, {}};

    std::wstring value(bytes / sizeof(wchar_t), L'\0');
    status = RegGetValueW(hive, key, name, RRF_RT_REG_SZ, nullptr, value.data(), &bytes);
    if (status != ERROR_SUCCESS || bytes < sizeof(wchar_t) || bytes % sizeof(wchar_t))
        return {SettingsPolicyMode::Unknown, {}};
    value.resize(bytes / sizeof(wchar_t) - 1);
    if (value.empty()) return {SettingsPolicyMode::None, {}};
    return ParseSettingsPolicy(std::move(value));
}

std::optional<bool> SettingsPolicyHides(SettingsPolicy const& policy, std::wstring_view page,
                                        bool categoryRoot) {
    if (policy.mode == SettingsPolicyMode::None || policy.mode == SettingsPolicyMode::Unknown)
        return {};
    const bool listed = std::find(policy.pages.begin(), policy.pages.end(), page) != policy.pages.end();
    if (policy.mode == SettingsPolicyMode::Hide) return listed;
    // A category can still be present through pages outside the mod's catalog.
    return !categoryRoot && !listed;
}

bool HiddenBySettingsPolicy(std::wstring_view page, bool categoryRoot,
                            std::array<SettingsPolicy, 2> const& policies) {
    bool hidden = false;
    for (auto const& policy : policies) {
        if (policy.mode == SettingsPolicyMode::None) continue;
        // When both scopes are configured, hide only if both confirm it.
        if (!SettingsPolicyHides(policy, page, categoryRoot).value_or(false)) return false;
        hidden = true;
    }
    return hidden;
}

constexpr bool SavedPasskeysAbsent(DWORD build, std::optional<DWORD> revision) {
    // The public Windows 11 rollout began with KB5030310 (22621.2361).
    return build < 22621 || (build == 22621 && revision && *revision < 2361);
}

DWORD ReadWindowsBuild() {
    using RtlGetVersionFn = LONG(WINAPI*)(OSVERSIONINFOW*);
    const auto module = GetModuleHandleW(L"ntdll.dll");
    if (!module) return 0;
    const auto getVersion = reinterpret_cast<RtlGetVersionFn>(GetProcAddress(module, "RtlGetVersion"));
    OSVERSIONINFOW version{};
    version.dwOSVersionInfoSize = sizeof(version);
    return getVersion && getVersion(&version) == 0 ? version.dwBuildNumber : 0;
}

std::optional<DWORD> ReadWindowsRevision() {
    return ReadMachineDword(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"UBR");
}

constexpr bool PreferLegacyDeveloperTitle(DWORD build, std::optional<DWORD> revision) {
    // The Advanced page began rolling out with KB5065789 (26100.6725).
    return build && (build < 26100 || (build == 26100 && revision && *revision < 6725));
}

struct NetworkInterfaces {
    bool ethernet{};
    bool wifi{};
    bool cellular{};
};

bool IsActiveSoftwareEthernet(MIB_IF_ROW2 const& row) {
    const auto& flags = row.InterfaceAndOperStatusFlags;
    return row.Type == IF_TYPE_ETHERNET_CSMACD && row.OperStatus == IfOperStatusUp &&
           !flags.HardwareInterface && !flags.FilterInterface;
}

bool IsAddressedSoftwareEthernet(MIB_IF_ROW2 const& row, MIB_UNICASTIPADDRESS_TABLE const& addresses) {
    return IsActiveSoftwareEthernet(row) &&
           std::any_of(addresses.Table, addresses.Table + addresses.NumEntries, [&](auto const& address) {
               return address.InterfaceLuid.Value == row.InterfaceLuid.Value;
           });
}

NetworkInterfaces ReadNetworkInterfaces() {
    MIB_IF_TABLE2* table{};
    if (GetIfTable2(&table) != NO_ERROR) return {true, true, true};
    NetworkInterfaces interfaces;
    for (DWORD i = 0; i < table->NumEntries; ++i) {
        auto const& entry = table->Table[i];
        interfaces.ethernet |= entry.Type == IF_TYPE_ETHERNET_CSMACD &&
                               entry.InterfaceAndOperStatusFlags.HardwareInterface;
        interfaces.wifi |= entry.Type == IF_TYPE_IEEE80211;
        interfaces.cellular |= entry.Type == IF_TYPE_WWANPP || entry.Type == IF_TYPE_WWANPP2;
    }
    const auto first = table->Table;
    const auto last = first + table->NumEntries;
    if (!interfaces.ethernet && std::any_of(first, last, IsActiveSoftwareEthernet)) {
        // Adapter tables also report Ethernet filters and software interfaces with no IP binding.
        MIB_UNICASTIPADDRESS_TABLE* addresses{};
        if (GetUnicastIpAddressTable(AF_UNSPEC, &addresses) != NO_ERROR) {
            interfaces.ethernet = true;
        } else {
            interfaces.ethernet = std::any_of(first, last, [&](auto const& entry) {
                return IsAddressedSoftwareEthernet(entry, *addresses);
            });
            FreeMibTable(addresses);
        }
    }
    FreeMibTable(table);
    return interfaces;
}

constexpr bool KeyboardPageKnownMissing(DWORD build, HRESULT resourceStatus) {
    return build == 26100 && resourceStatus == HRESULT_FROM_WIN32(ERROR_MRM_NAMED_RESOURCE_NOT_FOUND);
}

constexpr bool MissingDeviceSettingsPage(std::wstring_view page, NetworkInterfaces interfaces,
                                         bool keyboardPageMissing) {
    if (page == L"devices-keyboard") return keyboardPageMissing;
    if (page == L"network-ethernet") return !interfaces.ethernet;
    if (page == L"network-wifi") return !interfaces.wifi;
    if (page == L"network-mobilehotspot") return !interfaces.ethernet && !interfaces.wifi && !interfaces.cellular;
    return false;
}

std::optional<bool> TokenHasEnabledAdministrators(HANDLE token) {
    BYTE sid[SECURITY_MAX_SID_SIZE]{};
    DWORD sidSize = sizeof(sid);
    if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, sid, &sidSize)) return {};

    DWORD bytes{};
    if (GetTokenInformation(token, TokenGroups, nullptr, 0, &bytes) ||
        GetLastError() != ERROR_INSUFFICIENT_BUFFER || bytes < sizeof(TOKEN_GROUPS)) return {};
    std::vector<BYTE> buffer(bytes);
    if (!GetTokenInformation(token, TokenGroups, buffer.data(), bytes, &bytes)) return {};
    const auto groups = reinterpret_cast<TOKEN_GROUPS const*>(buffer.data());
    for (DWORD i = 0; i < groups->GroupCount; ++i) {
        auto const& group = groups->Groups[i];
        if ((group.Attributes & SE_GROUP_ENABLED) && EqualSid(group.Sid, sid)) return true;
    }
    return false;
}

constexpr bool CredentialsNeeded(TOKEN_ELEVATION_TYPE type, std::optional<bool> administrator,
                                 std::optional<DWORD> adminPrompt) {
    if (type == TokenElevationTypeFull || !administrator) return false;
    if (!*administrator) return type == TokenElevationTypeDefault || type == TokenElevationTypeLimited;
    return type == TokenElevationTypeLimited && adminPrompt && (*adminPrompt == 1 || *adminPrompt == 3);
}

constexpr bool LegacyAdminApprovalMode(LSTATUS status, DWORD mode) {
    return ConfirmedMissingError(status) || (status == ERROR_SUCCESS && mode == 1);
}

bool CredentialsNeeded() {
    HANDLE rawToken{};
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &rawToken)) return false;
    const std::unique_ptr<void, decltype(&CloseHandle)> token(rawToken, CloseHandle);
    TOKEN_ELEVATION_TYPE type{};
    DWORD bytes{};
    if (!GetTokenInformation(token.get(), TokenElevationType, &type, sizeof(type), &bytes)) return false;

    // Administrator protection uses a separate on-demand token and prompt policy.
    // The legacy token and policy checks cannot classify that mode reliably.
    DWORD mode{};
    bytes = sizeof(mode);
    const auto status = RegGetValueW(HKEY_LOCAL_MACHINE, uacPolicyKey, L"TypeOfAdminApprovalMode",
                                     RRF_RT_REG_DWORD, nullptr, &mode, &bytes);
    if (!LegacyAdminApprovalMode(status, mode)) return false;

    if (type == TokenElevationTypeDefault) {
        // With UAC off, administrators can get an unlinked token with full access.
        return CredentialsNeeded(type, TokenHasEnabledAdministrators(token.get()), {});
    }
    if (type != TokenElevationTypeLimited) return false;

    TOKEN_LINKED_TOKEN linked{};
    if (!GetTokenInformation(token.get(), TokenLinkedToken, &linked, sizeof(linked), &bytes)) return false;
    const std::unique_ptr<void, decltype(&CloseHandle)> fullToken(linked.LinkedToken, CloseHandle);
    return CredentialsNeeded(type, TokenHasEnabledAdministrators(fullToken.get()),
                             ReadMachineDword(uacPolicyKey, L"ConsentPromptBehaviorAdmin"));
}

bool RequiresAdministrator(std::wstring_view kind, std::wstring_view target) {
    // These Windows executables use requireAdministrator. Ordinary MMC routes
    // and tools that can also open with standard rights are kept visible.
    if (kind == L"exe") {
        const auto name = Split(target, L';').first;
        return name == L"OptionalFeatures.exe" || name == L"RecoveryDrive.exe" || name == L"MdSched.exe";
    }
    return kind == L"native" && target == L"10961";
}

AvailabilitySnapshot BuildAvailability() {
    AvailabilitySnapshot result;
    const bool home = IsHomeEdition();
    const auto interfaces = ReadNetworkInterfaces();
    const auto build = ReadWindowsBuild();
    const bool keyboardPageMissing = build == 26100 && KeyboardPageKnownMissing(build, ReadDeviceKeyboardResourceStatus());
    const bool startsOnThisPc = ExplorerStartsOnThisPc();
    const bool noPasskeys = build && SavedPasskeysAbsent(build, build == 22621 ? ReadWindowsRevision() : std::nullopt);
    const bool credentials = CredentialsNeeded();
    const bool missingControlHost = MissingFile(SystemPath(L"control.exe"));
    constexpr PCWSTR policyName = L"SettingsPageVisibility";
    const std::array policies{ReadSettingsPolicy(HKEY_LOCAL_MACHINE, settingsPolicyKey, policyName),
                              ReadSettingsPolicy(HKEY_CURRENT_USER, settingsPolicyKey, policyName)};

    for (size_t i = 0; i < std::size(catalog) && !unloading; ++i) {
        auto const& item = catalog[i];
        const auto [kind, target] = Split(item.action, L':');
        result.credentialsRequired[i] = credentials && RequiresAdministrator(kind, target);
        if (kind == L"exe") {
            const auto [program, args] = Split(target, L';');
            result.unavailable[i] = MissingFile(ProgramPath(program)) ||
                (program.ends_with(L".cpl") && missingControlHost) ||
                (args.ends_with(L".msc") && MissingFile(SystemPath(args))) ||
                (home && (args == L"gpedit.msc" || args == L"secpol.msc" || args == L"lusrmgr.msc"));
        } else if (kind == L"control") {
            result.unavailable[i] = missingControlHost || MissingControlRegistration(item.resource) ||
                                     (home && target == L"Microsoft.BitLockerDriveEncryption");
        } else if (kind == L"folder") {
            result.unavailable[i] = startsOnThisPc && target == L"0AC0837C-BBF8-452A-850D-79D08E667CA7";
        } else if (kind == L"uri" || kind == L"category") {
            const auto page = Split(target, L';').first;
            const bool categoryRoot = kind == L"uri" && item.parent < 0 && page != L"windowsupdate";
            result.unavailable[i] = MissingDeviceSettingsPage(page, interfaces, keyboardPageMissing) ||
                                     (page == L"savedpasskeys" && noPasskeys) ||
                                     (!home && HiddenBySettingsPolicy(page, categoryRoot, policies));
        }
    }
    return result;
}

// Coalesce rapid reopenings without keeping hardware and policy changes stale for 20 seconds.
constexpr ULONGLONG availabilityRefreshInterval = 1000;
SRWLOCK availabilityLock = SRWLOCK_INIT;
AvailabilitySnapshot availability;
std::atomic<bool> availabilityRefreshing{};
std::atomic<ULONGLONG> availabilityLastRefresh{};

AvailabilitySnapshot ReadAvailability() {
    AcquireSRWLockShared(&availabilityLock);
    const auto result = availability;
    ReleaseSRWLockShared(&availabilityLock);
    return result;
}

void CALLBACK RefreshAvailabilityWorker(PTP_CALLBACK_INSTANCE, PVOID) noexcept {
    try {
        if (!unloading) {
            const auto next = BuildAvailability();
            if (!unloading) {
                AcquireSRWLockExclusive(&availabilityLock);
                availability = next;
                ReleaseSRWLockExclusive(&availabilityLock);
            }
        }
    } catch (...) {
        // Keep the most recent complete snapshot when a refresh cannot finish.
    }
    availabilityLastRefresh = GetTickCount64();
    availabilityRefreshing = false;
}

void RefreshAvailability(bool force = false) {
    const auto isFresh = [] {
        const auto refreshedAt = availabilityLastRefresh.load();
        return refreshedAt && GetTickCount64() - refreshedAt < availabilityRefreshInterval;
    };
    if (unloading || (!force && isFresh()) || availabilityRefreshing.exchange(true)) return;
    if (unloading || (!force && isFresh())) {
        availabilityRefreshing = false;
        return;
    }

    if (moduleWorkQueue.Submit(RefreshAvailabilityWorker)) return;
    availabilityLastRefresh = GetTickCount64();
    availabilityRefreshing = false;
}

std::optional<Destination> Resolve(CatalogItem const& item, StockMenu const& stock,
                                   AvailabilitySnapshot const& availability, bool hideAdminCommands) {
    const auto index = &item - catalog;
    if (availability.unavailable[index] || (hideAdminCommands && availability.credentialsRequired[index]))
        return std::nullopt;
    auto [kind, target] = Split(item.action, L':');
    Destination destination{&item};
    if (kind == L"native") {
        destination.original = StockAction(stock, target);
        if (!destination.original || !destination.original.IsEnabled()) return std::nullopt;
    }
    return destination;
}

std::vector<P::Popup> FindSubmenuPopups(MenuSession const& active) {
    std::vector<P::Popup> owned;
    if (active.rows.empty()) return owned;
    const auto root = active.rows.front()->split.XamlRoot();
    if (!root) return owned;
    for (auto const& popup : X::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root)) {
        const auto presenter = popup.Child().try_as<C::MenuFlyoutPresenter>();
        if (!presenter || presenter.Items().Size() == 0) continue;
        const auto first = presenter.Items().GetAt(0).try_as<C::MenuFlyoutItemBase>();
        if (!first) continue;
        // The root lists rows, while a submenu lists the children of one row.
        uint32_t index;
        if (std::any_of(active.rows.begin(), active.rows.end(), [&](auto const& row) {
                return row->split.Items().IndexOf(first, index);
            })) owned.push_back(popup);
    }
    return owned;
}

void CloseSubmenus(MenuSession& active) noexcept {
    const bool firstClose = !std::exchange(active.closing, true);
    std::vector<P::Popup> popups;
    try { popups = FindSubmenuPopups(active); }
    catch (...) { Wh_Log(L"Could not identify the open Win+X submenu popups"); }

    std::vector<AV::IExpandCollapseProvider> expanded;
    if (firstClose) {
        for (auto const& row : active.rows) {
            row->openOnUpdate = false;
            try {
                if (row->hoverUpdate) row->hoverUpdate.Stop();
                if (row->expand && row->expand.ExpandCollapseState() == X::Automation::ExpandCollapseState::Expanded)
                    expanded.push_back(row->expand);
            } catch (...) {
                Wh_Log(L"Could not prepare the Win+X submenu to close");
            }
        }
    }
    // Closing can synchronously raise Closed on the parent. Use the references
    // captured above once XAML can dispatch events.
    for (auto const& provider : expanded) {
        try { provider.Collapse(); }
        catch (...) { Wh_Log(L"Could not close the Win+X submenu"); }
    }
    // XAML can hide the submenu HWND and report Collapsed while its Popup is
    // still open. Close that popup before restoring the original menu rows.
    for (auto const& popup : popups) {
        try { if (popup.IsOpen()) popup.IsOpen(false); }
        catch (...) { Wh_Log(L"Could not close the Win+X submenu popup"); }
    }
}

void RemoveRowVisual(RowInteraction& row) {
    if (row.arrowLayer && row.layout) {
        uint32_t index{};
        auto children = row.layout.Children();
        if (children.IndexOf(row.arrowLayer, index)) children.RemoveAt(index);
    }
}

void Restore() noexcept {
    auto old = std::move(session);
    if (!old) return;
    CloseSubmenus(*old);
    ClearCommandCursor(*old);
    for (auto const& row : old->rows) {
        try { RemoveRowVisual(*row); } catch (...) {}
    }
    try {
        if (old->powerChanged) {
            old->power.Items().ReplaceAll(old->originalPower);
            if (old->oldPowerIcon == X::DependencyProperty::UnsetValue())
                old->power.ClearValue(C::MenuFlyoutSubItem::IconProperty());
            else
                old->power.SetValue(C::MenuFlyoutSubItem::IconProperty(), old->oldPowerIcon);
        }
    } catch (...) {
        Wh_Log(L"Could not restore the original Win+X power submenu");
    }
    try {
        if (old->menuChanged) old->menu.Items().ReplaceAll(old->original);
    } catch (...) {
        Wh_Log(L"Could not restore the original Win+X menu");
    }
}

C::MenuFlyoutItem CreateLeaf(MenuSession& pending, Destination const& destination) {
    const auto caption = Caption(destination);
    if (caption.empty()) return nullptr;
    C::MenuFlyoutItem leaf;
    leaf.Text(caption);
    if (destination.item->glyph) leaf.Icon(MakeIcon(destination.item->glyph));
    auto owner = &pending;
    pending.clicks.push_back(leaf.Click(winrt::auto_revoke, [owner, destination](auto const&, auto const&) {
        if (unloading || session.get() != owner) return;
        try { RunDestination(*owner, destination); } catch (...) { Wh_Log(L"Win+X command failed"); }
    }));
    if (pending.commandCursor) {
        const auto enterOrMove = [owner](auto const& sender, I::PointerRoutedEventArgs const& args) {
            if (unloading || session.get() != owner) return;
            try {
                if (args.Pointer().PointerDeviceType() == PointerType::Mouse)
                    UpdateCommandCursor(*owner, sender.template as<X::UIElement>(), true);
            } catch (...) {
                Wh_Log(L"Could not update the Win+X command cursor");
            }
        };
        pending.cursorEvents.push_back({
            leaf.PointerEntered(winrt::auto_revoke, enterOrMove),
            leaf.PointerMoved(winrt::auto_revoke, enterOrMove),
            leaf.PointerExited(winrt::auto_revoke, [owner](auto const& sender, auto const&) {
                if (unloading || session.get() != owner) return;
                UpdateCommandCursor(*owner, sender.template try_as<X::UIElement>(), false);
            }),
        });
    }
    return leaf;
}

void AddInteraction(MenuSession& pending, C::MenuFlyoutSubItem const& split,
                    Destination const& destination, HoverMode mode) {
    auto row = std::make_unique<RowInteraction>();
    row->owner = &pending;
    row->split = split;
    row->destination = destination;
    row->mode = mode;
    if (destination.item) {
        row->keyDown = split.PreviewKeyDown(winrt::auto_revoke,
            [ptr = row.get()](auto const&, I::KeyRoutedEventArgs const& args) {
                if (args.Key() != Key::Enter) return;
                OnRow(ptr, [&](auto& active) {
                    args.Handled(true);
                    RunDestination(*active.owner, active.destination);
                });
            });
    }
    pending.rows.push_back(std::move(row));
}

std::optional<StockMenu> ReadStockMenu(C::MenuFlyout const& menu) {
    const auto items = menu.Items();
    if (items.Size() < 10) return std::nullopt;

    StockMenu stock;
    const auto powerTitle = Normalize(Twinui(L"10930"));
    if (powerTitle.empty()) return std::nullopt;
    for (auto const& item : items) {
        stock.rows.push_back(item);
        if (auto sub = item.try_as<C::MenuFlyoutSubItem>(); sub && Normalize(sub.Text()) == powerTitle)
            stock.power = sub;
    }
    if (!stock.power || !StockAction(stock, L"10921") || !StockAction(stock, L"10923") ||
        !StockAction(stock, L"10925")) return std::nullopt;
    return stock;
}

void TryPatchMenu(C::MenuFlyout const& menu) {
    auto nativeMenu = ReadStockMenu(menu);
    if (!nativeMenu) return;
    const bool recognizedPowerMenu = ReadStockPowerActions(*nativeMenu);
    const auto& stock = *nativeMenu;
    const auto items = menu.Items();
    const auto snapshot = ReadSettings();

    RefreshAvailability();
    const auto availability = ReadAvailability();

    auto pending = std::make_unique<MenuSession>();
    pending->menu = menu;
    pending->original = stock.rows;
    if (snapshot.showHandCursor) pending->commandCursor = LoadCursorW(nullptr, IDC_HAND);
    pending->power = stock.power;
    for (auto const& item : stock.power.Items()) pending->originalPower.push_back(item);
    pending->oldPowerIcon = stock.power.ReadLocalValue(C::MenuFlyoutSubItem::IconProperty());

    std::vector<C::MenuFlyoutItemBase> visible;
    std::vector<C::MenuFlyoutItemBase> powerChildren;
    wchar_t powerGlyph{};
    int previousGroup = -1;
    for (size_t index = 0; index < std::size(catalog); ++index) {
        const auto& item = catalog[index];
        if (item.parent >= 0 || !snapshot.enabledSections[item.group] || !snapshot.enabled[index]) continue;
        const std::wstring_view action(item.action);
        const bool power = action == L"power:menu";
        const bool elevateTerminal = snapshot.terminalAsAdmin && action == L"native:10960";
        auto destination = power ? std::optional<Destination>{} :
            Resolve(item, stock, availability, snapshot.hideAdminCommands);

        std::vector<C::MenuFlyoutItem> children;
        for (size_t child = index + 1; child < std::size(catalog) && catalog[child].parent == static_cast<int>(index); ++child) {
            const auto& command = catalog[child];
            // Stock power commands are appended below in the order Windows presents them.
            if (power && (!recognizedPowerMenu || std::wstring_view(command.action).starts_with(L"power:"))) continue;
            const bool elevatedTerminal = elevateTerminal && std::wstring_view(command.action) == L"native:10961";
            const bool primaryPower = power && snapshot.primaryPowerAction == command.action;
            if (!snapshot.enabled[child] && !elevatedTerminal && !primaryPower) continue;
            auto resolved = Resolve(command, stock, availability, snapshot.hideAdminCommands);
            if (primaryPower) {
                destination = std::move(resolved);
                continue;
            }
            if (!resolved) continue;
            if (elevatedTerminal) std::swap(resolved, destination);
            if (snapshot.enabled[child] && resolved) {
                if (auto leaf = CreateLeaf(*pending, *resolved)) children.push_back(leaf);
            }
        }
        if (power && recognizedPowerMenu) {
            for (auto const& command : stock.powerActions) {
                auto const& native = command.destination;
                if (!native.original.IsEnabled()) continue;
                const auto placement = PlacePowerAction(command.withUpdate,
                    snapshot.primaryPowerAction == native.item->action, snapshot.enabled[native.item - catalog]);
                if (placement == PowerPlacement::Main) destination = native;
                if (placement == PowerPlacement::Submenu) {
                    if (auto leaf = CreateLeaf(*pending, native)) children.push_back(leaf);
                }
            }
        }

        C::MenuFlyoutItemBase entry{nullptr};
        if (power && !recognizedPowerMenu) {
            entry = stock.power;
        } else if (power && !destination) {
            if (children.empty()) continue;
            powerChildren.assign(children.begin(), children.end());
            powerGlyph = item.glyph;
            AddInteraction(*pending, stock.power, {}, snapshot.mode);
            entry = stock.power;
        } else if (children.empty()) {
            if (!destination) continue;
            entry = CreateLeaf(*pending, *destination);
        } else {
            const auto caption = Caption(destination.value_or(Destination{&item}));
            if (caption.empty()) continue;
            C::MenuFlyoutSubItem split;
            split.Text(caption);
            const auto glyph = destination ? destination->item->glyph : item.glyph;
            if (glyph) split.Icon(MakeIcon(glyph));
            for (auto const& child : children) split.Items().Append(child);
            AddInteraction(*pending, split, destination.value_or(Destination{}), snapshot.mode);
            entry = split;
        }
        if (!entry) continue;
        if (previousGroup >= 0 && previousGroup != item.group) visible.push_back(C::MenuFlyoutSeparator());
        visible.push_back(entry);
        previousGroup = item.group;
    }
    if (visible.empty()) return;

    pending->opened = menu.Opened(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (unloading || session.get() != owner) return;
        try {
            for (auto const& row : owner->rows) {
                if (!AttachPointerEvents(*row)) {
                    Wh_Log(L"Incompatible Win+X submenu template; restoring the Windows menu");
                    Restore();
                    return;
                }
            }
        } catch (...) {
            Wh_Log(L"Could not initialize Win+X submenu interactions; restoring the Windows menu");
            Restore();
        }
    });
    pending->closingEvent = menu.Closing(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (session.get() == owner && !owner->closing) CloseSubmenus(*owner);
    });
    pending->closed = menu.Closed(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (session.get() == owner) Restore();
    });
    session = std::move(pending);
    if (!powerChildren.empty()) {
        session->powerChanged = true;
        stock.power.Items().ReplaceAll(powerChildren);
        if (powerGlyph) stock.power.Icon(MakeIcon(powerGlyph));
    }
    session->menuChanged = true;
    items.ReplaceAll(visible);
}

winrt::weak_ref<C::MenuFlyout> observedMenu;
[[clang::no_destroy]] P::FlyoutBase::Opening_revoker openingHandler;

template<typename Interface>
C::MenuFlyout Inspect(void* abi) noexcept {
    if (unloading || GetCurrentThreadId() != trayThread) return nullptr;
    C::MenuFlyout menu{nullptr};
    try {
        Interface flyout{nullptr};
        winrt::copy_from_abi(flyout, abi);
        menu = flyout.template try_as<C::MenuFlyout>();
        if (!menu || menu == observedMenu.get() || (session && menu == session->menu) ||
            menu.IsOpen() || !ReadStockMenu(menu)) return menu;

        // A new Win+X flyout can be shown before the old one has raised Closed.
        // Observe the new flyout's Opening independently of the closing session.
        auto weak = winrt::make_weak(menu);
        openingHandler = menu.Opening(winrt::auto_revoke, [weak](auto const&, auto const&) {
            if (unloading) return;
            C::MenuFlyout menu{nullptr};
            try {
                menu = weak.get();
                if (!menu) return;
                if (session && session->closing) Restore();
                if (!session) TryPatchMenu(menu);
            } catch (...) {
                if (menu && session && session->menu == menu) Restore();
                Wh_Log(L"Could not customize the Quick Link menu");
            }
        });
        observedMenu = std::move(weak);
    } catch (...) {
        Wh_Log(L"Could not observe the Quick Link menu");
    }
    return menu;
}

HRESULT WINAPI HookShowAt(void* self, void* target) {
    const auto menu = Inspect<P::IFlyoutBase>(self);
    const auto result = originalShowAt(self, target);
    if (FAILED(result) && menu && session && session->menu == menu) Restore();
    return result;
}

HRESULT WINAPI HookShowAtOptions(void* self, void* target, void* options) {
    const auto menu = Inspect<P::IFlyoutBase5>(self);
    const auto result = originalShowAtOptions(self, target, options);
    if (FAILED(result) && menu && session && session->menu == menu) Restore();
    return result;
}

enum class TaskbarSetup : LRESULT { Failed, Ready, WaitForXaml };

TaskbarSetup InstallHooks() {
    void* show;
    void* showOptions;
    try {
        C::MenuFlyout sample;
        const auto base = sample.as<P::IFlyoutBase>();
        const auto options = sample.as<P::IFlyoutBase5>();
        // The slots are the two ShowAt signatures in the Windows Runtime interfaces.
        show = (*reinterpret_cast<void***>(winrt::get_abi(base)))[14];
        showOptions = (*reinterpret_cast<void***>(winrt::get_abi(options)))[12];
    } catch (winrt::hresult_error const& error) {
        if (error.code() == RPC_E_WRONG_THREAD || error.code() == CO_E_NOTINITIALIZED)
            return TaskbarSetup::WaitForXaml;
        throw;
    }

    if (show == showOptions ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<ShowAt>(show), HookShowAt, &originalShowAt))
        return TaskbarSetup::Failed;
    const bool optionsHook = WindhawkUtils::SetFunctionHook(
        reinterpret_cast<ShowAtOptions>(showOptions), HookShowAtOptions, &originalShowAtOptions);
    if (optionsHook && Wh_ApplyHookOperations()) return TaskbarSetup::Ready;

    if (optionsHook) Wh_RemoveFunctionHook(showOptions);
    Wh_RemoveFunctionHook(show);
    Wh_ApplyHookOperations();
    return TaskbarSetup::Failed;
}

LRESULT CALLBACK TraySubclass(HWND window, UINT message, WPARAM action, LPARAM data, DWORD_PTR) {
    if (message == setupMessage) {
        try {
            if (action) return unloading ? 0 : static_cast<LRESULT>(InstallHooks());
            openingHandler.revoke();
            observedMenu = {};
            if (session) {
                auto menu = session->menu;
                menu.Hide();
            }
        } catch (...) {
            if (!action) Wh_Log(L"Quick Link menu taskbar operation failed");
        }
        if (!action) Restore();
        return 0;
    }
    return DefSubclassProc(window, message, action, data);
}

bool FinishTaskbarStartup() noexcept {
    const HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid{};
    const DWORD thread = window ? GetWindowThreadProcessId(window, &pid) : 0;
    if (!thread) return false;
    // A separate Explorer window process has no Win+X menu to customize.
    if (pid != GetCurrentProcessId() || unloading) return true;
    if (!GetModuleHandleW(L"Windows.UI.Xaml.dll")) return false;

    trayWindow = window;
    trayThread = thread;
    subclassPresent = WindhawkUtils::SetWindowSubclassFromAnyThread(trayWindow, TraySubclass, 0);
    const auto result = subclassPresent ? static_cast<TaskbarSetup>(SendMessageW(trayWindow, setupMessage, 1, 0))
                                        : TaskbarSetup::Failed;
    if (result == TaskbarSetup::Ready) {
        RefreshAvailability();
        if (!moduleWorkQueue.Submit(WarmSystemCaptions)) Wh_Log(L"Could not schedule Quick Link menu caption resolution");
        return true;
    }
    if (subclassPresent) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(trayWindow, TraySubclass);
        subclassPresent = false;
    }
    return false;
}

void ArmTaskbarTimer(PTP_TIMER timer, DWORD interval) {
    LARGE_INTEGER delay;
    delay.QuadPart = -static_cast<LONGLONG>(interval) * 10'000;
    FILETIME due{delay.LowPart, static_cast<DWORD>(delay.HighPart)};
    SetThreadpoolTimer(timer, &due, interval, 50);
}

void CALLBACK WaitForTaskbar(PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER timer) noexcept {
    if (!TryAcquireSRWLockExclusive(&taskbarStartupLock)) return;
    if (!unloading && IsThreadpoolTimerSet(timer)) {
        if (FinishTaskbarStartup()) {
            SetThreadpoolTimer(timer, nullptr, 0, 0);
        } else if (taskbarFastRetryDeadline && GetTickCount64() >= taskbarFastRetryDeadline) {
            taskbarFastRetryDeadline = 0;
            Wh_Log(L"Quick Link menu initialization is still pending; retrying every two seconds");
            ArmTaskbarTimer(timer, taskbarSlowRetryInterval);
        }
    }
    ReleaseSRWLockExclusive(&taskbarStartupLock);
}

}  // namespace

BOOL Wh_ModInit() {
    const HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid{};
    if (window && GetWindowThreadProcessId(window, &pid) && pid != GetCurrentProcessId()) return FALSE;
    setupMessage = RegisterWindowMessageW(L"Windhawk.QuickLinkMenu.Setup");
    if (!setupMessage) return FALSE;
    LoadSettings();
    return moduleWorkQueue.Start();
}

void Wh_ModAfterInit() {
    if (FinishTaskbarStartup()) return;

    // At sign-in Windhawk can initialize this mod before Explorer creates the taskbar.
    taskbarStartupTimer = CreateThreadpoolTimer(WaitForTaskbar, nullptr, nullptr);
    if (!taskbarStartupTimer) {
        Wh_Log(L"Could not schedule Quick Link menu initialization");
        return;
    }
    taskbarFastRetryDeadline = GetTickCount64() + taskbarFastRetryDuration;
    ArmTaskbarTimer(taskbarStartupTimer, taskbarRetryInterval);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshAvailability(true);
}

void Wh_ModBeforeUninit() {
    unloading = true;
    if (taskbarStartupTimer) {
        AcquireSRWLockExclusive(&taskbarStartupLock);
        SetThreadpoolTimer(taskbarStartupTimer, nullptr, 0, 0);
        ReleaseSRWLockExclusive(&taskbarStartupLock);
        WaitForThreadpoolTimerCallbacks(taskbarStartupTimer, TRUE);
        CloseThreadpoolTimer(taskbarStartupTimer);
        taskbarStartupTimer = nullptr;
    }
    if (subclassPresent && IsWindow(trayWindow)) {
        SendMessageW(trayWindow, setupMessage, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(trayWindow, TraySubclass);
    }
}

void Wh_ModUninit() {
    moduleWorkQueue.Stop();
}
