// ==WindhawkMod==
// @id              windows-border-switch
// @name            Windows Border Switch
// @description     Hide borders around windows, the Start menu, the taskbar, and other system elements
// @description:ru-RU Отключение рамок для окон, меню «Пуск», панели задач и других элементов системы
// @description:es-ES Oculta los bordes de las ventanas, el menú Inicio, la barra de tareas y otros elementos del sistema
// @description:de-DE Blendet die Rahmen von Fenstern, dem Startmenü, der Taskleiste und anderen Systemelementen aus
// @description:pt-BR Oculta as bordas das janelas, do menu Iniciar, da barra de tarefas e de outros elementos do sistema
// @description:zh-CN 隐藏窗口、“开始”菜单、任务栏和其他系统元素的边框
// @version         1.1.3
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         SearchApp.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         WidgetBoard.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -luuid -ladvapi32
// ==/WindhawkMod==
// Build: 44

// ==WindhawkModReadme==
/*
# Windows Border Switch

**English** · [Русский](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Windows_Border_Switch/README.ru.md)

Windows Border Switch hides system-drawn borders around windows and panels in Windows 11.

## Features

| ![Dark theme comparison, with default borders on the left and hidden borders on the right](https://raw.githubusercontent.com/CTravkin/DSN_Windhawk_Mods/main/Windows_Border_Switch/Assets/Screenshots/Dark_Border_Comparison.png) | ![Light theme comparison, with default borders on the left and hidden borders on the right](https://raw.githubusercontent.com/CTravkin/DSN_Windhawk_Mods/main/Windows_Border_Switch/Assets/Screenshots/Light_Border_Comparison.png) |
| :---: | :---: |

| Setting | Controls |
| --- | --- |
| Active window | The focused window's system border or its title bar accent color |
| Inactive windows | System borders of windows without focus |
| Dialogs | System borders of classic Win32 dialogs, with or without focus |
| Taskbar | The taskbar edge and the virtual desktops strip at the bottom of Task View |
| Start menu | The Start menu and its Phone Link panel |
| Search | The Windows Search panel |
| Notifications | The notification center, calendar, and notification banners on the desktop |
| Quick Settings | Quick Settings and the media controls above them |
| Widgets | The outer border of the Widgets board |
| Exceptions | Keep the usual border and title bar for active, inactive, or all windows of selected apps |

The interface is available in English, Russian, Spanish, German, Brazilian Portuguese, and Simplified Chinese.

### Limitations

- Shell borders depend on the Windows build and Windows Web Experience Pack. After enabling or updating the mod, some panels may require restarting Windows Explorer or signing out and back in.
- Apps running as administrator, including Windhawk, may retain their borders. Apps that draw their own frames control their appearance.
- Mods that change the same border or title bar can override each other, including Windows 11 Accent Window Border.

## Platforms

Requires Windows 11 x64 or ARM64.

## Usage

Open the mod's **Settings** to change each setting independently. Initially, the mod keeps the active window border, removes its title bar accent color, and hides the other listed borders. Choose **Windows-defined** to restore Windows styling for a setting.

Under **Exceptions**, add an executable name such as `steam.exe` and select which windows to exempt under **Do not hide border**.

## Links

- [Windows Border Switch on GitHub](https://github.com/CTravkin/DSN_Windhawk_Mods/tree/main/Windows_Border_Switch)
- [DSN Windhawk Mods](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/README.md)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- borders:
    - activeWindow: caption
      $name: Active window
      $name:ru-RU: Активное окно
      $name:es-ES: Ventana activa
      $name:de-DE: Aktives Fenster
      $name:pt-BR: Janela ativa
      $name:zh-CN: 活动窗口
      $description: Focused window border or title bar accent color
      $description:ru-RU: Рамка окна в фокусе или акцентный цвет его заголовка
      $description:es-ES: Borde de la ventana con el foco o color de énfasis de su barra de título
      $description:de-DE: Rahmen des fokussierten Fensters oder Akzentfarbe seiner Titelleiste
      $description:pt-BR: Borda da janela em foco ou cor de destaque da barra de título
      $description:zh-CN: 焦点窗口的边框或标题栏主题色
      $options:
      - default: Windows-defined
      - hide: Hide border
      - caption: Disable title bar accent color
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      - caption: Отключить акцент заголовка
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      - caption: Desactivar el color de énfasis de la barra de título
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      - caption: Akzentfarbe der Titelleiste deaktivieren
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      - caption: Desativar a cor de destaque da barra de título
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
      - caption: 关闭标题栏主题色
    - inactiveWindow: hide
      $name: Inactive windows
      $name:ru-RU: Неактивные окна
      $name:es-ES: Ventanas inactivas
      $name:de-DE: Inaktive Fenster
      $name:pt-BR: Janelas inativas
      $name:zh-CN: 非活动窗口
      $description: System-drawn borders of windows without focus
      $description:ru-RU: Системные рамки окон вне фокуса
      $description:es-ES: Bordes del sistema de las ventanas sin el foco
      $description:de-DE: Systemrahmen von Fenstern ohne Fokus
      $description:pt-BR: Bordas do sistema das janelas sem foco
      $description:zh-CN: 未获得焦点的窗口的系统边框
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
    - dialog: hide
      $name: Dialogs
      $name:ru-RU: Диалоговые окна
      $name:es-ES: Cuadros de diálogo
      $name:de-DE: Dialogfelder
      $name:pt-BR: Caixas de diálogo
      $name:zh-CN: 对话框
      $description: Classic Windows dialog borders, with or without focus
      $description:ru-RU: Системные рамки классических диалогов вне зависимости от фокуса
      $description:es-ES: Bordes de los cuadros de diálogo clásicos, con o sin el foco
      $description:de-DE: Rahmen klassischer Dialogfelder mit oder ohne Fokus
      $description:pt-BR: Bordas das caixas de diálogo clássicas, com ou sem foco
      $description:zh-CN: 经典 Windows 对话框的边框，无论是否有焦点
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
    - taskbar: hide
      $name: Taskbar
      $name:ru-RU: Панель задач
      $name:es-ES: Barra de tareas
      $name:de-DE: Taskleiste
      $name:pt-BR: Barra de tarefas
      $name:zh-CN: 任务栏
      $description: Taskbar edge and the desktops strip in Task View
      $description:ru-RU: Кромка панели задач и панель рабочих столов в представлении задач
      $description:es-ES: Borde de la barra de tareas y panel de escritorios en Vista de tareas
      $description:de-DE: Kante der Taskleiste und Leiste der virtuellen Desktops in der Aufgabenansicht
      $description:pt-BR: Borda da barra de tarefas e faixa de áreas de trabalho na Visão de tarefas
      $description:zh-CN: 任务栏边缘和任务视图中的桌面栏
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
    - startMenu: hide
      $name: Start menu
      $name:ru-RU: Меню «Пуск»
      $name:es-ES: Menú Inicio
      $name:de-DE: Startmenü
      $name:pt-BR: Menu Iniciar
      $name:zh-CN: “开始”菜单
      $description: Start menu and its Phone Link panel
      $description:ru-RU: Меню «Пуск» и его панель «Телефон»
      $description:es-ES: Menú Inicio y su panel Enlace Móvil
      $description:de-DE: Startmenü und sein Bereich Smartphone-Link
      $description:pt-BR: Menu Iniciar e seu painel Vincular ao Celular
      $description:zh-CN: “开始”菜单及其手机连接面板
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
    - search: hide
      $name: Search
      $name:ru-RU: Поиск
      $name:es-ES: Búsqueda
      $name:de-DE: Suche
      $name:pt-BR: Pesquisa
      $name:zh-CN: 搜索
      $description: Outer border of the Windows Search panel
      $description:ru-RU: Внешняя рамка панели поиска Windows
      $description:es-ES: Borde exterior del panel de búsqueda de Windows
      $description:de-DE: Außenrahmen des Windows-Suchbereichs
      $description:pt-BR: Borda externa do painel de pesquisa do Windows
      $description:zh-CN: Windows 搜索面板的外边框
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
    - notificationCenter: hide
      $name: Notifications
      $name:ru-RU: Уведомления
      $name:es-ES: Notificaciones
      $name:de-DE: Benachrichtigungen
      $name:pt-BR: Notificações
      $name:zh-CN: 通知
      $description: Notification center, calendar, and desktop banners
      $description:ru-RU: Центр уведомлений, календарь и баннеры на рабочем столе
      $description:es-ES: Centro de notificaciones, calendario y banners del escritorio
      $description:de-DE: Benachrichtigungscenter, Kalender und Desktopbanner
      $description:pt-BR: Central de notificações, calendário e banners na área de trabalho
      $description:zh-CN: 通知中心、日历和桌面横幅
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
    - quickSettings: hide
      $name: Quick Settings
      $name:ru-RU: Быстрые настройки
      $name:es-ES: Configuración rápida
      $name:de-DE: Schnelleinstellungen
      $name:pt-BR: Configurações rápidas
      $name:zh-CN: 快速设置
      $description: Quick Settings and the media controls above them
      $description:ru-RU: Панель быстрых настроек и управление воспроизведением над ней
      $description:es-ES: Configuración rápida y los controles multimedia situados encima
      $description:de-DE: Schnelleinstellungen und die Mediensteuerung darüber
      $description:pt-BR: Configurações rápidas e os controles de mídia acima delas
      $description:zh-CN: 快速设置及其上方的媒体控件
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
    - widgets: hide
      $name: Widgets
      $name:ru-RU: Мини-приложения
      $name:es-ES: Widgets
      $name:de-DE: Widgets
      $name:pt-BR: Widgets
      $name:zh-CN: 小组件
      $description: Outer border of the widgets board
      $description:ru-RU: Внешняя рамка панели мини-приложений
      $description:es-ES: Borde exterior del panel de widgets
      $description:de-DE: Außenrahmen des Widget-Boards
      $description:pt-BR: Borda externa do painel de widgets
      $description:zh-CN: 小组件面板的外边框
      $options:
      - default: Windows-defined
      - hide: Hide border
      $options:ru-RU:
      - default: Определено Windows
      - hide: Скрыть рамку
      $options:es-ES:
      - default: Definido por Windows
      - hide: Ocultar borde
      $options:de-DE:
      - default: Von Windows festgelegt
      - hide: Rahmen ausblenden
      $options:pt-BR:
      - default: Definido pelo Windows
      - hide: Ocultar borda
      $options:zh-CN:
      - default: 由 Windows 决定
      - hide: 隐藏边框
  $name: Border settings
  $name:ru-RU: Настройка рамок
  $name:es-ES: Configuración de bordes
  $name:de-DE: Rahmeneinstellungen
  $name:pt-BR: Configurações de borda
  $name:zh-CN: 边框设置
- processExceptions:
  - - process: ""
      $name: Process
      $name:ru-RU: Процесс
      $name:es-ES: Proceso
      $name:de-DE: Prozess
      $name:pt-BR: Processo
      $name:zh-CN: 进程
      $description: Executable name, e.g. steam.exe
      $description:ru-RU: Имя файла процесса, например steam.exe
      $description:es-ES: Nombre del ejecutable, p. ej. steam.exe
      $description:de-DE: Name der ausführbaren Datei, z. B. steam.exe
      $description:pt-BR: Nome do executável, por exemplo, steam.exe
      $description:zh-CN: 可执行文件名，例如 steam.exe
    - scope: both
      $name: Do not hide border
      $name:ru-RU: Не скрывать рамку
      $name:es-ES: No ocultar el borde
      $name:de-DE: Rahmen nicht ausblenden
      $name:pt-BR: Não ocultar a borda
      $name:zh-CN: 不隐藏边框
      $description: Selected windows keep their usual border and title bar
      $description:ru-RU: У выбранных окон сохраняется обычное оформление рамки и заголовка
      $description:es-ES: Las ventanas seleccionadas conservan su borde y su barra de título habituales
      $description:de-DE: Die ausgewählten Fenster behalten ihren üblichen Rahmen und ihre Titelleiste
      $description:pt-BR: As janelas selecionadas mantêm a borda e a barra de título habituais
      $description:zh-CN: 所选窗口保留原有的边框和标题栏
      $options:
      - both: All windows
      - active: Active windows
      - inactive: Inactive windows
      $options:ru-RU:
      - both: Все окна
      - active: Активные окна
      - inactive: Неактивные окна
      $options:es-ES:
      - both: Todas las ventanas
      - active: Ventanas activas
      - inactive: Ventanas inactivas
      $options:de-DE:
      - both: Alle Fenster
      - active: Aktive Fenster
      - inactive: Inaktive Fenster
      $options:pt-BR:
      - both: Todas as janelas
      - active: Janelas ativas
      - inactive: Janelas inativas
      $options:zh-CN:
      - both: 所有窗口
      - active: 活动窗口
      - inactive: 非活动窗口
  $name: Exceptions
  $name:ru-RU: Исключения
  $name:es-ES: Excepciones
  $name:de-DE: Ausnahmen
  $name:pt-BR: Exceções
  $name:zh-CN: 例外
  $description: Windows from the listed apps follow special rules
  $description:ru-RU: Окна указанных приложений следуют особым правилам
  $description:es-ES: Las ventanas de las aplicaciones indicadas siguen reglas especiales
  $description:de-DE: Für Fenster der angegebenen Apps gelten besondere Regeln
  $description:pt-BR: As janelas dos aplicativos indicados seguem regras especiais
  $description:zh-CN: 指定应用的窗口遵循特殊规则
*/
// ==/WindhawkModSettings==

#include <string_view>

namespace {

enum class WindowEffect { Default, HideBorder, RemoveCaptionAccent };

enum class WindowException : unsigned { None = 0, Active = 1, Inactive = 2, Both = 3 };

constexpr bool ExcludesWindow(WindowException exception, bool active) {
    auto state = active ? WindowException::Active : WindowException::Inactive;
    return (static_cast<unsigned>(exception) & static_cast<unsigned>(state)) != 0;
}

struct WindowSettings {
    WindowEffect active = WindowEffect::Default;
    bool hideInactive = false;
    bool hideDialog = false;

    constexpr bool AnyEnabled() const {
        return active != WindowEffect::Default || hideInactive || hideDialog;
    }

    bool operator==(const WindowSettings&) const = default;
};

enum class WindowUpdate { None, Start, Refresh, Stop };

constexpr WindowUpdate WindowSettingsUpdate(WindowSettings previous, WindowSettings next) {
    if (previous == next) {
        return WindowUpdate::None;
    }
    if (!next.AnyEnabled()) {
        return WindowUpdate::Stop;
    }
    return previous.AnyEnabled() ? WindowUpdate::Refresh : WindowUpdate::Start;
}

constexpr WindowEffect WindowEffectFor(WindowSettings settings, bool active, bool dialog) {
    if (dialog) {
        return settings.hideDialog ? WindowEffect::HideBorder : WindowEffect::Default;
    }
    if (active) {
        return settings.active;
    }
    return settings.hideInactive ? WindowEffect::HideBorder : WindowEffect::Default;
}

enum class CaptionEffect { Default, Transparent, Neutral };

struct WindowFrame {
    bool hideBorder = false;
    CaptionEffect caption = CaptionEffect::Default;

#ifdef BORDER_SWITCH_POLICY_TEST
    bool operator==(const WindowFrame&) const = default;
#endif
};

constexpr WindowFrame WindowFrameFor(WindowSettings settings, bool active, bool dialog,
                                     bool clientAtTop, bool captionAccentEnabled) {
    WindowEffect effect = WindowEffectFor(settings, active, dialog);
    WindowFrame frame{effect == WindowEffect::HideBorder};
    if (dialog || !captionAccentEnabled) {
        return frame;
    }
    if (effect == WindowEffect::RemoveCaptionAccent) {
        frame.caption = clientAtTop ? CaptionEffect::Transparent : CaptionEffect::Neutral;
    } else if (frame.hideBorder && clientAtTop) {
        // A caption backdrop can show through the client at the top edge,
        // even after the compositor's border has been hidden.
        frame.caption = CaptionEffect::Transparent;
    }
    return frame;
}

enum class ShellHost { Unknown, Explorer, Start, Search, Flyouts, Widgets };
enum class ShellBorder {
    None, Taskbar, TaskView, Start, Search, Notifications, QuickSettings, Widgets
};

struct ShellSetting {
    ShellHost host;
    ShellBorder border;
    const wchar_t* key;
};

constexpr ShellSetting kShellSettings[] = {
    {ShellHost::Explorer, ShellBorder::Taskbar, L"taskbar"},
    {ShellHost::Explorer, ShellBorder::TaskView, L"taskbar"},
    {ShellHost::Start, ShellBorder::Start, L"startMenu"},
    {ShellHost::Search, ShellBorder::Search, L"search"},
    {ShellHost::Flyouts, ShellBorder::Notifications, L"notificationCenter"},
    {ShellHost::Flyouts, ShellBorder::QuickSettings, L"quickSettings"},
    {ShellHost::Widgets, ShellBorder::Widgets, L"widgets"},
};

constexpr unsigned BorderBit(ShellBorder border) {
    return border == ShellBorder::None ? 0 : 1u << static_cast<unsigned>(border);
}

constexpr ShellBorder ShellBorderFor(ShellHost host,
                                     std::wstring_view type,
                                     std::wstring_view name) {
    if (host == ShellHost::Start && name == L"AcrylicBorder" &&
        type == L"Windows.UI.Xaml.Controls.Border") {
        return ShellBorder::Start;
    }
    if (host == ShellHost::Explorer && name == L"BackgroundStroke" &&
        type == L"Windows.UI.Xaml.Shapes.Rectangle") {
        return ShellBorder::Taskbar;
    }
    if (host == ShellHost::Explorer && type == L"Windows.UI.Xaml.Controls.Border" &&
        (name == L"VirtualDesktopBarBackground" || name == L"VirtualDesktopSwitcherBackground")) {
        return ShellBorder::TaskView;
    }
    if (host == ShellHost::Search && (name == L"AppBorder" || name == L"AccentAppBorder") &&
        type == L"Windows.UI.Xaml.Controls.Border") {
        return ShellBorder::Search;
    }
    if (host == ShellHost::Flyouts && type == L"Windows.UI.Xaml.Controls.Grid") {
        if (name == L"NotificationCenterGrid" || name == L"CalendarCenterGrid") {
            return ShellBorder::Notifications;
        }
        if (name == L"ControlCenterRegion" || name == L"MediaTransportControlsRegion") {
            return ShellBorder::QuickSettings;
        }
    }
    if (host == ShellHost::Flyouts && type == L"Windows.UI.Xaml.Controls.Border" &&
        (name == L"ToastBackgroundBorder" || name == L"ToastBackgroundBorder2" ||
         name == L"CalendarHeaderMinimizedOverlay")) {
        return ShellBorder::Notifications;
    }
    if (host == ShellHost::Widgets) {
        if (name == L"NavigationViewBorder" && type == L"Microsoft.UI.Xaml.Controls.Border") {
            return ShellBorder::Widgets;
        }
        if (name == L"WidgetBoardHost" && type == L"Microsoft.UI.Xaml.Controls.Grid") {
            return ShellBorder::Widgets;
        }
        if (name == L"WidgetBoardContent" &&
            (type == L"Microsoft.UI.Xaml.Controls.Grid" ||
             type == L"WidgetBoard.WidgetHostContentControl")) {
            return ShellBorder::Widgets;
        }
    }
    return ShellBorder::None;
}

constexpr bool IsStartPane(std::wstring_view name) {
    return name == L"MainMenu" || name == L"CompanionRoot";
}

constexpr bool IsStartBorderParent(std::wstring_view name, std::wstring_view type) {
    return IsStartPane(name) || (name.empty() && type == L"Windows.UI.Xaml.Controls.Grid");
}

}  // namespace

#ifndef BORDER_SWITCH_POLICY_TEST
#include <dwmapi.h>
#include <roapi.h>
#include <winstring.h>
#include <windhawk_utils.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

namespace Foundation = winrt::Windows::Foundation;
namespace Xaml = winrt::Windows::UI::Xaml;
namespace WinUI = winrt::Microsoft::UI::Xaml;

ShellHost g_shellHost;

struct Worker {
    HANDLE thread = nullptr;
    HANDLE stop = nullptr;

    void Start(LPTHREAD_START_ROUTINE run) {
        if (thread) {
            if (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0) {
                return;
            }
            Stop();
        }
        stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stop) {
            return;
        }
        thread = CreateThread(nullptr, 0, run, stop, 0, nullptr);
        if (!thread) {
            CloseHandle(std::exchange(stop, nullptr));
        }
    }

    void Stop() {
        if (!thread) {
            return;
        }
        SetEvent(stop);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(std::exchange(thread, nullptr));
        CloseHandle(std::exchange(stop, nullptr));
    }
};

WindowSettings g_windowSettings;
struct ProcessException {
    std::wstring name;
    WindowException scope;

    bool operator==(ProcessException const&) const = default;
};

std::vector<ProcessException> g_windowExceptions;
std::vector<ProcessException> g_requestedWindowExceptions;
struct WindowOwner {
    DWORD pid;
    WindowException exception;
};

std::unordered_map<HWND, WindowOwner> g_windowOwners;
std::mutex g_windowSettingsMutex;
WindowSettings g_requestedWindowSettings;
winrt::handle g_windowSettingsChanged;
Worker g_windowWorker;
struct AppliedFrame {
    COLORREF border = DWMWA_COLOR_DEFAULT;
    COLORREF caption = DWMWA_COLOR_DEFAULT;

    bool operator==(const AppliedFrame&) const = default;
};

std::unordered_map<HWND, AppliedFrame> g_changedWindows;
std::unordered_set<HWND> g_pendingWindows;
UINT_PTR g_frameTimer;
HWND g_foreground;

bool ClassIs(HWND window, std::wstring_view name) {
    WCHAR buffer[96];
    int length = GetClassNameW(window, buffer, ARRAYSIZE(buffer));
    return std::wstring_view(buffer, length) == name;
}

bool IsMainExplorer() {
    DWORD pid = 0;
    GetWindowThreadProcessId(GetShellWindow(), &pid);
    return pid == GetCurrentProcessId();
}

thread_local bool g_shellWindowChanged;
thread_local HWND g_previousShellWindow;

void CALLBACK OnShellWindowEvent(HWINEVENTHOOK, DWORD event, HWND window, LONG object, LONG child,
                                DWORD, DWORD) {
    if (object != OBJID_WINDOW || child != CHILDID_SELF || !window) {
        return;
    }
    if (event == EVENT_OBJECT_DESTROY) {
        if (window == g_previousShellWindow) {
            g_previousShellWindow = nullptr;
            g_shellWindowChanged = true;
        }
    } else if (ClassIs(window, L"Progman") || ClassIs(window, L"Shell_TrayWnd") ||
               ClassIs(window, L"Windows.UI.Core.CoreWindow") ||
               ClassIs(window, L"Windows.UI.Composition.DesktopWindowContentBridge") ||
               ClassIs(window, L"Microsoft.UI.Content.DesktopChildSiteBridge") ||
               ClassIs(window, L"ControlCenterWindow")) {
        g_shellWindowChanged = true;
    }
}

bool WaitForShellActivity(HANDLE stop, DWORD delay) {
    ULONGLONG deadline = GetTickCount64() + delay;
    while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT) {
        if (std::exchange(g_shellWindowChanged, false)) {
            return true;
        }
        ULONGLONG now = GetTickCount64();
        DWORD remaining = delay == INFINITE ? INFINITE
                          : now < deadline ? static_cast<DWORD>(deadline - now)
                                           : 0;
        DWORD result = MsgWaitForMultipleObjects(1, &stop, FALSE, remaining, QS_ALLINPUT);
        if (result == WAIT_TIMEOUT) {
            return true;
        }
        if (result != WAIT_OBJECT_0 + 1) {
            break;
        }
        MSG message;
        while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT &&
               PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    return false;
}

bool WaitForShellOwnership(HANDLE stop) {
    if (IsMainExplorer()) {
        return true;
    }
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, nullptr,
                                       OnShellWindowEvent, GetCurrentProcessId(), 0,
                                       WINEVENT_OUTOFCONTEXT);
    if (!hook) {
        Wh_Log(L"Unable to watch for the Explorer shell window");
        return false;
    }
    while (!IsMainExplorer() && WaitForSingleObject(stop, 0) == WAIT_TIMEOUT) {
        g_previousShellWindow = GetShellWindow();
        DWORD previousPid = 0;
        if (g_previousShellWindow) {
            GetWindowThreadProcessId(g_previousShellWindow, &previousPid);
        }
        HWINEVENTHOOK previousHook = nullptr;
        if (previousPid && previousPid != GetCurrentProcessId()) {
            previousHook = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr,
                                           OnShellWindowEvent, previousPid, 0,
                                           WINEVENT_OUTOFCONTEXT);
        }
        // A taskbar creation or the loss of the old desktop can precede
        // registration of the new desktop by several messages.
        constexpr DWORD settleDelays[] = {50, 150, 500, 1500, 5000};
        for (DWORD delay : settleDelays) {
            if (IsMainExplorer() || !WaitForShellActivity(stop, delay)) {
                break;
            }
        }
        bool changed = false;
        if (!IsMainExplorer()) {
            changed = g_previousShellWindow != GetShellWindow() ||
                      WaitForShellActivity(stop, INFINITE);
        }
        if (previousHook) {
            UnhookWinEvent(previousHook);
        }
        g_previousShellWindow = nullptr;
        if (!changed) {
            break;
        }
    }
    UnhookWinEvent(hook);
    return WaitForSingleObject(stop, 0) == WAIT_TIMEOUT && IsMainExplorer();
}

bool CaptionAccentEnabled() {
    HIGHCONTRASTW contrast{sizeof(contrast), 0, nullptr};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        return false;
    }
    DWORD value = 0;
    DWORD size = sizeof(value);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                        L"ColorPrevalence", RRF_RT_REG_DWORD, nullptr, &value,
                        &size) == ERROR_SUCCESS &&
           value != 0;
}

bool SetFrameColor(HWND window, DWORD attribute, COLORREF color) {
    HRESULT result = DwmSetWindowAttribute(window, attribute, &color, sizeof(color));
    if (SUCCEEDED(result)) {
        Wh_Log(L"DWM window %p attribute %lu = %08lX", window, attribute, color);
    }
    return SUCCEEDED(result);
}

bool ClientReachesTop(HWND window) {
    RECT frame{};
    POINT client{};
    return SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS,
                                           &frame, sizeof(frame))) &&
           ClientToScreen(window, &client) && client.y <= frame.top;
}

constexpr wchar_t kPersonalizeKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";

COLORREF NeutralCaptionColor(HWND window) {
    BOOL supportsDark = FALSE;
    DWORD light = 1;
    DWORD size = sizeof(light);
    DwmGetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &supportsDark,
                          sizeof(supportsDark));
    if (supportsDark && RegGetValueW(HKEY_CURRENT_USER, kPersonalizeKey,
                                    L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light,
                                    &size) == ERROR_SUCCESS && !light) {
        return RGB(32, 32, 32);
    }
    return GetSysColor(COLOR_BTNFACE);
}

WindowException FindProcessException(DWORD pid) {
    winrt::handle process{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)};
    if (!process) {
        return WindowException::None;
    }
    WCHAR path[MAX_PATH];
    DWORD length = ARRAYSIZE(path);
    std::wstring longPath;
    std::wstring_view filename;
    if (QueryFullProcessImageNameW(process.get(), 0, path, &length)) {
        filename = {path, length};
    } else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        longPath.resize(32768);
        length = static_cast<DWORD>(longPath.size());
        if (QueryFullProcessImageNameW(process.get(), 0, longPath.data(), &length)) {
            filename = {longPath.data(), length};
        }
    }
    if (filename.empty()) {
        return WindowException::None;
    }
    filename.remove_prefix(filename.find_last_of(L"\\/") + 1);
    unsigned scopes = 0;
    for (auto const& entry : g_windowExceptions) {
        if (CompareStringOrdinal(filename.data(), static_cast<int>(filename.size()),
                                 entry.name.c_str(), -1, TRUE) == CSTR_EQUAL) {
            scopes |= static_cast<unsigned>(entry.scope);
        }
    }
    return static_cast<WindowException>(scopes);
}

bool IsExcludedWindow(HWND window, bool active) {
    if (g_windowExceptions.empty()) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (!pid) {
        return false;
    }
    auto entry = g_windowOwners.find(window);
    if (entry == g_windowOwners.end() || entry->second.pid != pid) {
        entry = g_windowOwners.insert_or_assign(window, WindowOwner{pid, FindProcessException(pid)}).first;
    }
    return ExcludesWindow(entry->second.exception, active);
}

AppliedFrame DesiredFrame(HWND window) {
    if (!IsWindowVisible(window) || IsIconic(window)) {
        return {};
    }
    auto style = static_cast<DWORD>(GetWindowLongPtrW(window, GWL_STYLE));
    bool hasCaption = (style & WS_CAPTION) == WS_CAPTION;
    bool dialog = ClassIs(window, L"#32770");
    if ((style & WS_CHILD) || !(dialog || hasCaption || (style & WS_THICKFRAME))) {
        return {};
    }
    bool active = window == g_foreground;
    WindowEffect effect = WindowEffectFor(g_windowSettings, active, dialog);
    if (effect == WindowEffect::Default || IsExcludedWindow(window, active)) {
        return {};
    }
    bool accent = hasCaption && !dialog && CaptionAccentEnabled();
    auto frame = WindowFrameFor(g_windowSettings, active, dialog,
                               accent && ClientReachesTop(window), accent);
    AppliedFrame desired;
    if (frame.hideBorder) {
        desired.border = DWMWA_COLOR_NONE;
    }
    if (frame.caption == CaptionEffect::Transparent) {
        desired.caption = DWMWA_COLOR_NONE;
    } else if (frame.caption == CaptionEffect::Neutral) {
        desired.caption = NeutralCaptionColor(window);
    }
    return desired;
}

void ApplyWindowBorder(HWND window, bool reapply = false) {
    if (!window || GetAncestor(window, GA_ROOT) != window) {
        return;
    }
    AppliedFrame desired = DesiredFrame(window);
    auto changed = g_changedWindows.find(window);
    AppliedFrame applied = changed == g_changedWindows.end() ? AppliedFrame{} : changed->second;
    if ((applied.border != desired.border || (reapply && desired.border != DWMWA_COLOR_DEFAULT)) &&
        SetFrameColor(window, DWMWA_BORDER_COLOR, desired.border)) {
        applied.border = desired.border;
    }
    if ((applied.caption != desired.caption ||
         (reapply && desired.caption != DWMWA_COLOR_DEFAULT)) &&
        SetFrameColor(window, DWMWA_CAPTION_COLOR, desired.caption)) {
        applied.caption = desired.caption;
    }
    if (applied == AppliedFrame{}) {
        g_changedWindows.erase(window);
    } else {
        g_changedWindows.insert_or_assign(window, applied);
    }
}

void UpdateWindowFrame(HWND window) {
    if (!window || GetAncestor(window, GA_ROOT) != window) {
        return;
    }
    ApplyWindowBorder(window, true);
    if (g_changedWindows.contains(window)) {
        g_pendingWindows.insert(window);
        if (!g_frameTimer) {
            // Some applications update their DWM colors during activation.
            // Reapply once after that message sequence has settled.
            g_frameTimer = SetTimer(nullptr, 0, 80, nullptr);
            if (!g_frameTimer) {
                g_pendingWindows.clear();
            }
        }
    }
}

void RefreshWindows(bool reapply = false) {
    g_foreground = GetForegroundWindow();
    EnumWindows([](HWND window, LPARAM reapply) -> BOOL {
        ApplyWindowBorder(window, reapply != 0);
        return TRUE;
    }, reapply);
}

void CALLBACK OnWindowEvent(HWINEVENTHOOK, DWORD event, HWND window, LONG object, LONG child,
                            DWORD, DWORD) {
    if (object != OBJID_WINDOW || child != CHILDID_SELF) {
        return;
    }
    if (event == EVENT_OBJECT_DESTROY) {
        g_windowOwners.erase(window);
        g_changedWindows.erase(window);
        g_pendingWindows.erase(window);
    } else if (event == EVENT_OBJECT_HIDE) {
        g_pendingWindows.erase(window);
        ApplyWindowBorder(window);
    } else if (event == EVENT_OBJECT_SHOW || event == EVENT_SYSTEM_MINIMIZEEND ||
               event == EVENT_SYSTEM_MOVESIZEEND) {
        UpdateWindowFrame(window);
    } else if (event == EVENT_SYSTEM_FOREGROUND) {
        HWND previous = std::exchange(g_foreground, GetForegroundWindow());
        if (previous != g_foreground) {
            UpdateWindowFrame(previous);
        }
        UpdateWindowFrame(g_foreground);
    }
}

DWORD WINAPI RunWindowWorker(void* stopParameter) {
    HANDLE stop = static_cast<HANDLE>(stopParameter);
    if (!WaitForShellOwnership(stop)) {
        return 0;
    }
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const std::array hooks = {
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                        OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT),
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr,
                        OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT),
        SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND, nullptr,
                        OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT),
        SetWinEventHook(EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND, nullptr,
                        OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT),
    };

    if (hooks[0] && hooks[1]) {
        winrt::handle themeChanged{CreateEventW(nullptr, FALSE, FALSE, nullptr)};
        HKEY themeKey = nullptr;
        auto watchTheme = [&] {
            return RegNotifyChangeKeyValue(themeKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                           themeChanged.get(), TRUE) == ERROR_SUCCESS;
        };
        bool watchingTheme = themeChanged &&
            RegOpenKeyExW(HKEY_CURRENT_USER, kPersonalizeKey, 0, KEY_NOTIFY,
                          &themeKey) == ERROR_SUCCESS && watchTheme();
        if (!watchingTheme) {
            Wh_Log(L"Unable to subscribe to theme changes");
        }
        const HANDLE events[] = {stop, g_windowSettingsChanged.get(), themeChanged.get()};
        UINT_PTR themeTimer = 0;
        auto loadSettings = [] {
            std::lock_guard guard(g_windowSettingsMutex);
            g_windowSettings = g_requestedWindowSettings;
            if (g_windowExceptions != g_requestedWindowExceptions) {
                g_windowExceptions = g_requestedWindowExceptions;
                g_windowOwners.clear();
            }
        };
        loadSettings();
        RefreshWindows();
        for (;;) {
            const DWORD count = watchingTheme ? ARRAYSIZE(events) : 2;
            const DWORD result = MsgWaitForMultipleObjects(count, events, FALSE, INFINITE,
                                                            QS_ALLINPUT);
            if (result == WAIT_OBJECT_0 + 1) {
                loadSettings();
                RefreshWindows();
                continue;
            }
            if (watchingTheme && result == WAIT_OBJECT_0 + 2) {
                watchingTheme = watchTheme();
                if (!themeTimer) {
                    // Let applications apply the new theme before reading their DWM flags.
                    themeTimer = SetTimer(nullptr, 0, 250, nullptr);
                    if (!themeTimer) {
                        RefreshWindows(true);
                    }
                }
                continue;
            }
            if (result != WAIT_OBJECT_0 + count) {
                break;
            }
            MSG message;
            while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT &&
                   PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (!message.hwnd && message.message == WM_TIMER &&
                    themeTimer && message.wParam == themeTimer) {
                    KillTimer(nullptr, std::exchange(themeTimer, 0));
                    RefreshWindows(true);
                    continue;
                }
                if (!message.hwnd && message.message == WM_TIMER &&
                    g_frameTimer && message.wParam == g_frameTimer) {
                    KillTimer(nullptr, std::exchange(g_frameTimer, 0));
                    for (HWND window : g_pendingWindows) {
                        ApplyWindowBorder(window, true);
                    }
                    g_pendingWindows.clear();
                    continue;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        if (themeTimer) {
            KillTimer(nullptr, themeTimer);
        }
        if (themeKey) {
            RegCloseKey(themeKey);
        }
    } else {
        Wh_Log(L"Unable to subscribe to window events");
    }

    for (HWINEVENTHOOK hook : hooks) {
        if (hook) {
            UnhookWinEvent(hook);
        }
    }
    if (g_frameTimer) {
        KillTimer(nullptr, std::exchange(g_frameTimer, 0));
    }
    g_pendingWindows.clear();
    g_windowOwners.clear();
    for (auto [window, frame] : g_changedWindows) {
        if (frame.border != DWMWA_COLOR_DEFAULT) {
            SetFrameColor(window, DWMWA_BORDER_COLOR, DWMWA_COLOR_DEFAULT);
        }
        if (frame.caption != DWMWA_COLOR_DEFAULT) {
            SetFrameColor(window, DWMWA_CAPTION_COLOR, DWMWA_COLOR_DEFAULT);
        }
    }
    g_changedWindows.clear();
    return 0;
}

struct XamlBorder {
    winrt::weak_ref<Xaml::FrameworkElement> element;
    Xaml::DependencyProperty property;
    Foundation::IInspectable previous;
    ShellBorder border;
    DWORD thread;
};

std::mutex g_shellXamlMutex;
// Release the edited properties on their UI threads, never at process detach.
[[clang::no_destroy]] std::optional<std::vector<XamlBorder>> g_shellXamlEdits{std::in_place};
std::atomic<unsigned> g_shellXamlMask;
std::atomic<bool> g_shellXamlStopping;
Worker g_shellXamlWorker;

template<class Source>
struct XamlSources {
    std::vector<std::pair<DWORD, winrt::weak_ref<Source>>> sources;

    void Add(Source const& source) {
        const DWORD thread = GetCurrentThreadId();
        std::erase_if(sources, [thread](auto const& entry) {
            return entry.first == thread && !entry.second.get();
        });
        sources.emplace_back(thread, winrt::make_weak(source));
    }

    std::vector<Source> OnCurrentThread() {
        const DWORD thread = GetCurrentThreadId();
        std::vector<Source> current;
        std::erase_if(sources, [&](auto const& entry) {
            if (entry.first != thread) {
                return false;
            }
            if (auto source = entry.second.get()) {
                current.push_back(std::move(source));
                return false;
            }
            return true;
        });
        return current;
    }
};

using WidgetSource = WinUI::Hosting::DesktopWindowXamlSource;
using TaskbarSource = Xaml::Hosting::DesktopWindowXamlSource;
XamlSources<WidgetSource> g_widgetSources;
XamlSources<TaskbarSource> g_taskbarSources;
XamlSources<Xaml::FrameworkElement> g_taskViewRoots;
std::mutex g_sourcesMutex;

struct MeasuredTaskbar {
    DWORD thread;
    winrt::weak_ref<Xaml::FrameworkElement> frame;
    winrt::weak_ref<Xaml::FrameworkElement> stroke;
};
std::vector<MeasuredTaskbar> g_measuredTaskbars;

decltype(&RoGetActivationFactory) g_getActivationFactory;
using CreateXamlSource = HRESULT(WINAPI*)(void*, void*, void**, void**);
CreateXamlSource g_createXamlSource;
using ActivateInstance = HRESULT(WINAPI*)(void*, void**);
ActivateInstance g_createMetadata, g_activateXamlType;
using GetXamlType = HRESULT(WINAPI*)(void*, HSTRING, void**);
GetXamlType g_getXamlType;
std::mutex g_factoryMutex;

template<class Method, class Interface>
void HookFactoryMethod(Interface const& instance, size_t slot, Method hook, Method& original) {
    std::lock_guard guard(g_factoryMutex);
    if (!original) {
        auto table = *reinterpret_cast<void***>(winrt::get_abi(instance));
        if (WindhawkUtils::SetFunctionHook(reinterpret_cast<Method>(table[slot]), hook, &original)) {
            Wh_ApplyHookOperations();
        }
    }
}

template<class Source>
HRESULT WINAPI CaptureXamlSource(void* factory, void* outer, void** inner, void** result) {
    const HRESULT hr = g_createXamlSource(factory, outer, inner, result);
    if (SUCCEEDED(hr) && result && *result && !g_shellXamlStopping) {
        try {
            Source source{nullptr};
            winrt::copy_from_abi(source, *result);
            std::lock_guard guard(g_sourcesMutex);
            if constexpr (std::is_same_v<Source, WidgetSource>) {
                g_widgetSources.Add(source);
            } else {
                g_taskbarSources.Add(source);
            }
        } catch (...) {
            Wh_Log(L"Unable to track XAML source: %08X", winrt::to_hresult());
        }
    }
    return hr;
}

constexpr std::wstring_view kSwitcherMetadata =
    L"WindowsInternal.ComposableShell.Experiences.Switcher.Switcher_XamlTypeInfo.XamlMetaDataProvider";
constexpr std::wstring_view kVirtualDesktopBar =
    L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement";

HRESULT WINAPI CaptureTaskView(void* factory, void** result) {
    const HRESULT hr = g_activateXamlType(factory, result);
    if (SUCCEEDED(hr) && result && *result && !g_shellXamlStopping) {
        try {
            Foundation::IInspectable value{nullptr};
            winrt::copy_from_abi(value, *result);
            if (winrt::get_class_name(value) == kVirtualDesktopBar) {
                if (auto root = value.try_as<Xaml::FrameworkElement>()) {
                    std::lock_guard guard(g_sourcesMutex);
                    g_taskViewRoots.Add(root);
                }
            }
        } catch (...) {
            Wh_Log(L"Unable to track Task View content: %08X", winrt::to_hresult());
        }
    }
    return hr;
}

HRESULT WINAPI FindTaskViewType(void* provider, HSTRING name, void** result) {
    const HRESULT hr = g_getXamlType(provider, name, result);
    if (SUCCEEDED(hr) && name && result && *result && !g_shellXamlStopping &&
        std::wstring_view(WindowsGetStringRawBuffer(name, nullptr)) == kVirtualDesktopBar) {
        try {
            Xaml::Markup::IXamlType type{nullptr};
            winrt::copy_from_abi(type, *result);
            // IXamlType::ActivateInstance follows the twelve type properties.
            HookFactoryMethod(type, 18, CaptureTaskView, g_activateXamlType);
        } catch (...) {
            Wh_Log(L"Unable to hook Task View activation: %08X", winrt::to_hresult());
        }
    }
    return hr;
}

HRESULT WINAPI CaptureSwitcherMetadata(void* factory, void** result) {
    const HRESULT hr = g_createMetadata(factory, result);
    if (SUCCEEDED(hr) && result && *result && !g_shellXamlStopping) {
        try {
            Foundation::IInspectable value{nullptr};
            winrt::copy_from_abi(value, *result);
            if (auto provider = value.try_as<Xaml::Markup::IXamlMetadataProvider>()) {
                // GetXamlTypeByFullName is the second method after IInspectable.
                HookFactoryMethod(provider, 7, FindTaskViewType, g_getXamlType);
            }
        } catch (...) {
            Wh_Log(L"Unable to hook Task View metadata: %08X", winrt::to_hresult());
        }
    }
    return hr;
}

template<class Source, class Factory>
void HookXamlSource(Foundation::IInspectable const& object) {
    if (auto factory = object.try_as<Factory>()) {
        // The composable factory declares CreateInstance immediately after IInspectable.
        HookFactoryMethod(factory, 6, CaptureXamlSource<Source>, g_createXamlSource);
    }
}

HRESULT WINAPI FindShellFactory(HSTRING classId, REFIID iid, void** result) {
    const HRESULT hr = g_getActivationFactory(classId, iid, result);
    if (FAILED(hr) || !classId || !result || !*result || g_shellXamlStopping) {
        return hr;
    }
    try {
        const std::wstring_view name = WindowsGetStringRawBuffer(classId, nullptr);
        Foundation::IInspectable object{nullptr};
        if (g_shellHost == ShellHost::Widgets && name == winrt::name_of<WidgetSource>()) {
            winrt::copy_from_abi(object, *result);
            HookXamlSource<WidgetSource, WinUI::Hosting::IDesktopWindowXamlSourceFactory>(object);
        } else if (g_shellHost == ShellHost::Explorer && name == winrt::name_of<TaskbarSource>()) {
            winrt::copy_from_abi(object, *result);
            HookXamlSource<TaskbarSource, Xaml::Hosting::IDesktopWindowXamlSourceFactory>(object);
        } else if (g_shellHost == ShellHost::Explorer && name == kSwitcherMetadata) {
            winrt::copy_from_abi(object, *result);
            if (auto factory = object.try_as<Foundation::IActivationFactory>()) {
                HookFactoryMethod(factory, 6, CaptureSwitcherMetadata, g_createMetadata);
            }
        }
    } catch (...) {
        Wh_Log(L"Unable to hook shell XAML factory: %08X", winrt::to_hresult());
    }
    return hr;
}

struct WidgetEdit {
    winrt::weak_ref<WinUI::FrameworkElement> element;
    WinUI::DependencyProperty property;
    Foundation::IInspectable previous;
    WinUI::Media::SolidColorBrush brush;
    DWORD thread;
};
[[clang::no_destroy]] std::optional<std::vector<WidgetEdit>> g_widgetEdits{std::in_place};

void ScanWidgetTree(WinUI::DependencyObject const& node, unsigned depth = 0) {
    if (!node || depth == 36) {
        return;
    }
    if (auto element = node.try_as<WinUI::FrameworkElement>()) {
        auto border = ShellBorderFor(ShellHost::Widgets, winrt::get_class_name(element), element.Name());
        if (border == ShellBorder::Widgets) {
            WinUI::DependencyProperty property{nullptr};
            if (element.try_as<WinUI::Controls::Grid>()) {
                property = WinUI::Controls::Grid::BorderBrushProperty();
            } else if (element.try_as<WinUI::Controls::Border>()) {
                property = WinUI::Controls::Border::BorderBrushProperty();
            } else if (element.try_as<WinUI::Controls::Control>()) {
                property = WinUI::Controls::Control::BorderBrushProperty();
            }
            if (property) {
                auto local = element.ReadLocalValue(property);
                const DWORD thread = GetCurrentThreadId();
                std::lock_guard guard(g_shellXamlMutex);
                if (g_shellXamlStopping) {
                    return;
                }
                auto edit = std::find_if(g_widgetEdits->begin(), g_widgetEdits->end(), [&](auto const& entry) {
                    return entry.thread == thread && entry.element.get() == element;
                });
                if (edit == g_widgetEdits->end()) {
                    WinUI::Media::SolidColorBrush brush{winrt::Windows::UI::Color{}};
                    element.SetValue(property, brush);
                    g_widgetEdits->push_back({winrt::make_weak(element), property, local, brush, thread});
                    Wh_Log(L"Hid Widgets border: %s", element.Name().c_str());
                } else if (local != edit->brush) {
                    element.SetValue(property, edit->brush);
                    edit->previous = local;
                }
            }
        }
    }
    const int count = WinUI::Media::VisualTreeHelper::GetChildrenCount(node);
    for (int child = 0; child < count; ++child) {
        ScanWidgetTree(WinUI::Media::VisualTreeHelper::GetChild(node, child), depth + 1);
    }
}

void RefreshWidgets(bool hide) {
    if (hide) {
        {
            const DWORD thread = GetCurrentThreadId();
            std::lock_guard guard(g_shellXamlMutex);
            std::erase_if(*g_widgetEdits, [thread](auto const& edit) {
                return edit.thread == thread && !edit.element.get();
            });
        }
        std::vector<WidgetSource> sources;
        {
            std::lock_guard guard(g_sourcesMutex);
            sources = g_widgetSources.OnCurrentThread();
        }
        for (auto const& source : sources) {
            ScanWidgetTree(source.Content());
        }
        return;
    }
    std::vector<WidgetEdit> restore;
    {
        const DWORD thread = GetCurrentThreadId();
        std::lock_guard guard(g_shellXamlMutex);
        for (auto it = g_widgetEdits->begin(); it != g_widgetEdits->end();) {
            if (it->thread == thread) {
                restore.push_back(std::move(*it));
                it = g_widgetEdits->erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto const& edit : restore) {
        if (auto element = edit.element.get(); element && element.ReadLocalValue(edit.property) == edit.brush) {
            if (edit.previous == WinUI::DependencyProperty::UnsetValue()) {
                element.ClearValue(edit.property);
            } else {
                element.SetValue(edit.property, edit.previous);
            }
            Wh_Log(L"Restored Widgets border: %s", element.Name().c_str());
        }
    }
}

bool IsHidden(ShellBorder border, Foundation::IInspectable const& local) {
    if (border == ShellBorder::Taskbar) {
        auto opacity = local.try_as<Foundation::IReference<double>>();
        return opacity && opacity.Value() == 0;
    }
    auto thickness = local.try_as<Foundation::IReference<Xaml::Thickness>>();
    return thickness && thickness.Value() == Xaml::Thickness{};
}

void RestoreXamlBorders(unsigned mask) {
    std::vector<XamlBorder> restore;
    {
        std::lock_guard guard(g_shellXamlMutex);
        const DWORD thread = GetCurrentThreadId();
        for (auto it = g_shellXamlEdits->begin(); it != g_shellXamlEdits->end();) {
            if (it->thread == thread && !(mask & BorderBit(it->border))) {
                restore.push_back(std::move(*it));
                it = g_shellXamlEdits->erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto const& edit : restore) {
        try {
            if (auto element = edit.element.get()) {
                if (!IsHidden(edit.border, element.ReadLocalValue(edit.property))) {
                    continue;
                }
                if (edit.previous == Xaml::DependencyProperty::UnsetValue()) {
                    element.ClearValue(edit.property);
                } else {
                    element.SetValue(edit.property, edit.previous);
                }
                Wh_Log(L"Restored XAML border: %s", element.Name().c_str());
            }
        } catch (...) {
            Wh_Log(L"Unable to restore XAML border: %08X", winrt::to_hresult());
        }
    }
}

void HideXamlBorder(Xaml::FrameworkElement const& element, ShellBorder border) {
    auto property = border == ShellBorder::Taskbar
        ? Xaml::UIElement::OpacityProperty()
        : element.try_as<Xaml::Controls::Grid>()
        ? Xaml::Controls::Grid::BorderThicknessProperty()
        : Xaml::Controls::Border::BorderThicknessProperty();
    auto local = element.ReadLocalValue(property);
    if (IsHidden(border, local)) {
        return;
    }
    {
        const DWORD thread = GetCurrentThreadId();
        std::lock_guard guard(g_shellXamlMutex);
        if (g_shellXamlStopping) {
            return;
        }
        std::erase_if(*g_shellXamlEdits, [thread](XamlBorder const& edit) {
            return edit.thread == thread && !edit.element.get();
        });
        auto current = std::find_if(g_shellXamlEdits->begin(), g_shellXamlEdits->end(),
                                    [&](XamlBorder const& edit) {
            return edit.thread == thread && edit.element.get() == element;
        });
        if (current == g_shellXamlEdits->end()) {
            g_shellXamlEdits->push_back({winrt::make_weak(element), property, local, border, thread});
        } else {
            current->previous = local;
        }
    }
    element.SetValue(property, border == ShellBorder::Taskbar
                                   ? winrt::box_value(0.0)
                                   : winrt::box_value(Xaml::Thickness{}));
    Wh_Log(L"Hid XAML border: %s", element.Name().c_str());
}

void ScanShellTree(Xaml::DependencyObject const& node, unsigned mask, unsigned depth = 0,
                   Xaml::FrameworkElement* stroke = nullptr) {
    if (!node || depth == 36) {
        return;
    }
    if (auto element = node.try_as<Xaml::FrameworkElement>()) {
        auto border = ShellBorderFor(g_shellHost, winrt::get_class_name(element), element.Name());
        if (mask & BorderBit(border)) {
            auto parent = border == ShellBorder::Start
                ? Xaml::Media::VisualTreeHelper::GetParent(element).try_as<Xaml::FrameworkElement>()
                : nullptr;
            if (border != ShellBorder::Start ||
                (parent && IsStartBorderParent(parent.Name(), winrt::get_class_name(parent)))) {
                HideXamlBorder(element, border);
                if (stroke && border == ShellBorder::Taskbar && !*stroke) {
                    *stroke = element;
                }
            }
        }
    }
    const int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(node);
    for (int child = 0; child < count; ++child) {
        ScanShellTree(Xaml::Media::VisualTreeHelper::GetChild(node, child), mask, depth + 1, stroke);
    }
}

void ScanShellOnUiThread(bool refresh) noexcept try {
    const unsigned mask = g_shellXamlStopping ? 0 : g_shellXamlMask.load();
    if (g_shellHost == ShellHost::Widgets) {
        RefreshWidgets(mask != 0);
        return;
    }
    if (refresh) {
        RestoreXamlBorders(mask);
    }
    if (mask && g_shellHost == ShellHost::Explorer) {
        std::vector<TaskbarSource> taskbars;
        std::vector<Xaml::FrameworkElement> taskViews, measuredTaskbars;
        {
            std::lock_guard guard(g_sourcesMutex);
            taskbars = g_taskbarSources.OnCurrentThread();
            taskViews = g_taskViewRoots.OnCurrentThread();
            if (mask & BorderBit(ShellBorder::Taskbar)) {
                const DWORD thread = GetCurrentThreadId();
                for (auto const& entry : g_measuredTaskbars) {
                    if (entry.thread == thread) {
                        if (auto frame = entry.frame.get()) {
                            measuredTaskbars.push_back(std::move(frame));
                        }
                    }
                }
            }
        }
        for (auto const& taskbar : taskbars) {
            ScanShellTree(taskbar.Content(), mask);
        }
        for (auto const& frame : measuredTaskbars) {
            ScanShellTree(frame, BorderBit(ShellBorder::Taskbar));
        }
        for (auto const& taskView : taskViews) {
            ScanShellTree(taskView, mask);
        }
    } else if (mask) {
        if (auto window = Xaml::Window::Current()) {
            ScanShellTree(window.Content(), mask);
            for (auto popup : Xaml::Media::VisualTreeHelper::GetOpenPopups(window)) {
                ScanShellTree(popup.Child(), mask);
            }
        }
    }
} catch (...) {
    Wh_Log(L"Shell XAML scan failed: %08X", winrt::to_hresult());
}

using MeasureTaskbarFrame = HRESULT(WINAPI*)(IUnknown*, Foundation::Size, Foundation::Size*);
MeasureTaskbarFrame g_measureTaskbarFrame;
bool g_taskbarLayoutHookInstalled;

void ApplyMeasuredTaskbar(Xaml::FrameworkElement const& frame, unsigned mask) {
    const DWORD thread = GetCurrentThreadId();
    Xaml::FrameworkElement stroke{nullptr};
    {
        std::lock_guard guard(g_sourcesMutex);
        auto entry = std::find_if(g_measuredTaskbars.begin(), g_measuredTaskbars.end(),
                                  [&](auto const& item) { return item.thread == thread && item.frame.get() == frame; });
        if (entry != g_measuredTaskbars.end()) {
            stroke = entry->stroke.get();
            if (!stroke || !stroke.IsLoaded()) {
                stroke = nullptr;
                g_measuredTaskbars.erase(entry);
            }
        }
        if (!stroke) {
            std::erase_if(g_measuredTaskbars, [thread](auto const& item) {
                return item.thread == thread && !item.frame.get();
            });
        }
    }
    if (stroke) {
        HideXamlBorder(stroke, ShellBorder::Taskbar);
        return;
    }

    ScanShellTree(frame, mask, 0, &stroke);
    if (stroke) {
        std::lock_guard guard(g_sourcesMutex);
        g_measuredTaskbars.push_back({thread, winrt::make_weak(frame), winrt::make_weak(stroke)});
    }
}

HRESULT WINAPI MeasureTaskbarFrameHook(IUnknown* self, Foundation::Size available, Foundation::Size* result) {
    const HRESULT hr = g_measureTaskbarFrame(self, available, result);
    const unsigned mask = g_shellXamlStopping ? 0 : g_shellXamlMask.load() & BorderBit(ShellBorder::Taskbar);
    if (SUCCEEDED(hr) && mask) {
        try {
            Xaml::FrameworkElement frame{nullptr};
            if (SUCCEEDED(self->QueryInterface(winrt::guid_of<Xaml::FrameworkElement>(), winrt::put_abi(frame)))) {
                ApplyMeasuredTaskbar(frame, mask);
            }
        } catch (...) {
            Wh_Log(L"Unable to update the taskbar frame: %08X", winrt::to_hresult());
        }
    }
    return hr;
}

enum class TaskbarHookResult { Installed, Missing, Retry };

TaskbarHookResult HookTaskbarLayout(HMODULE module) {
    void* measure = nullptr;
    // Taskbar.View.dll, ExplorerExtensions.dll
    const WindhawkUtils::SYMBOL_HOOK hook[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
         &measure, nullptr, true},
    };
    if (!WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook))) {
        return TaskbarHookResult::Retry;
    }
    if (!measure) {
        return TaskbarHookResult::Missing;
    }
    std::lock_guard guard(g_factoryMutex);
    // Windhawk removes disabled hooks after applying, including failed installs.
    return WindhawkUtils::SetFunctionHook(reinterpret_cast<MeasureTaskbarFrame>(measure),
                                          MeasureTaskbarFrameHook, &g_measureTaskbarFrame) &&
                   Wh_ApplyHookOperations()
        ? TaskbarHookResult::Installed
        : TaskbarHookResult::Retry;
}

struct TaskbarLayoutRetry {
    std::pair<HMODULE, HMODULE> modules{};
    size_t retries = 0;
    ULONGLONG nextAttempt = 0;

    DWORD Update() {
        if (g_taskbarLayoutHookInstalled) {
            return INFINITE;
        }
        DWORD owner = 0;
        GetWindowThreadProcessId(FindWindowW(L"Shell_TrayWnd", nullptr), &owner);
        if (owner != GetCurrentProcessId()) {
            return INFINITE;
        }

        const std::pair loaded{GetModuleHandleW(L"Taskbar.View.dll"),
                               GetModuleHandleW(L"ExplorerExtensions.dll")};
        if (modules != loaded) {
            *this = TaskbarLayoutRetry{loaded};
        }
        const ULONGLONG now = GetTickCount64();
        if (nextAttempt > now) {
            return static_cast<DWORD>(nextAttempt - now);
        }

        // Windhawk can cache a lookup failure for one minute, or four hours on startup.
        constexpr DWORD delays[] = {2'000, 65'000, 4 * 60 * 60 * 1'000 + 2'000};
        if (retries > ARRAYSIZE(delays)) {
            return INFINITE;
        }
        for (HMODULE module : {modules.first, modules.second}) {
            if (!module) {
                continue;
            }
            switch (HookTaskbarLayout(module)) {
                case TaskbarHookResult::Installed:
                    g_taskbarLayoutHookInstalled = true;
                    Wh_Log(L"Taskbar layout hook installed");
                    return INFINITE;
                case TaskbarHookResult::Missing:
                    continue;
                case TaskbarHookResult::Retry:
                    Wh_Log(L"Taskbar layout hook could not be installed");
                    if (retries < ARRAYSIZE(delays)) {
                        const DWORD delay = delays[retries++];
                        nextAttempt = GetTickCount64() + delay;
                        return delay;
                    }
                    continue;
            }
        }
        retries = ARRAYSIZE(delays) + 1;
        return INFINITE;
    }
};

UINT ShellScanMessage() {
    static const UINT message = RegisterWindowMessageW(L"Windhawk.WindowsBorderSwitch.ScanShellXaml");
    return message;
}

std::mutex g_shellScanMutex;
std::atomic<HHOOK> g_shellScanHook;

LRESULT CALLBACK ShellScanHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const auto& message = *reinterpret_cast<const CWPSTRUCT*>(lParam);
        const HHOOK hook = reinterpret_cast<HHOOK>(message.lParam);
        if (message.message == ShellScanMessage() && hook && g_shellScanHook == hook &&
            UnhookWindowsHookEx(hook)) {
            // Remove the hook on its own thread before the synchronous send returns.
            g_shellScanHook = nullptr;
            ScanShellOnUiThread(message.wParam != 0);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void RequestShellScan(HWND window, bool refresh) {
    DWORD pid = 0;
    const DWORD thread = GetWindowThreadProcessId(window, &pid);
    const UINT message = ShellScanMessage();
    if (!thread || pid != GetCurrentProcessId() || !message) {
        return;
    }
    if (thread == GetCurrentThreadId()) {
        ScanShellOnUiThread(refresh);
        return;
    }
    std::lock_guard guard(g_shellScanMutex);
    if (HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, ShellScanHook, nullptr, thread)) {
        g_shellScanHook = hook;
        SendMessageW(window, message, refresh, reinterpret_cast<LPARAM>(hook));
        // The window may have gone away before the message reached it.
        if (g_shellScanHook.exchange(nullptr)) {
            UnhookWindowsHookEx(hook);
        }
    }
}

bool IsShellXamlWindow(HWND window) {
    if (g_shellHost == ShellHost::Explorer) {
        return ClassIs(window, L"Shell_TrayWnd") || ClassIs(window, L"Shell_SecondaryTrayWnd") ||
               ClassIs(window, L"XamlExplorerHostIslandWindow") ||
               ClassIs(window, L"Windows.UI.Core.CoreWindow");
    }
    if (g_shellHost == ShellHost::Widgets) {
        return ClassIs(window, L"WindowsDashboard");
    }
    return ClassIs(window, L"Windows.UI.Core.CoreWindow") || ClassIs(window, L"ControlCenterWindow");
}

bool RefreshShellXamlWindows(bool refresh, bool visibleOnly = false) {
    struct Request {
        bool refresh;
        bool visibleOnly;
        std::unordered_set<DWORD> scanned;
    } request{refresh, visibleOnly, {}};
    EnumWindows([](HWND window, LPARAM value) -> BOOL {
        auto& request = *reinterpret_cast<Request*>(value);
        DWORD pid = 0;
        const DWORD thread = GetWindowThreadProcessId(window, &pid);
        if (pid != GetCurrentProcessId() || !IsShellXamlWindow(window)) {
            return TRUE;
        }
        if (request.visibleOnly) {
            DWORD cloaked = 0;
            if (!IsWindowVisible(window) ||
                (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
                 cloaked)) {
                return TRUE;
            }
        }
        if (request.scanned.insert(thread).second) {
            RequestShellScan(window, request.refresh);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&request));
    return !request.scanned.empty();
}

bool g_shellXamlScanPending;
UINT_PTR g_shellXamlScanTimer;

void CALLBACK OnShellXamlWindow(HWINEVENTHOOK, DWORD event, HWND window, LONG object, LONG child, DWORD, DWORD) {
    if (g_shellHost == ShellHost::Explorer && event == EVENT_OBJECT_CREATE && window &&
        ClassIs(window, L"Windows.UI.Composition.DesktopWindowContentBridge")) {
        window = GetAncestor(window, GA_ROOT);
    }
    if (event != EVENT_OBJECT_DESTROY && object == OBJID_WINDOW && child == CHILDID_SELF &&
        window && IsShellXamlWindow(window) && !g_shellXamlScanPending) {
        g_shellXamlScanPending = true;
        if (g_shellXamlScanTimer) {
            KillTimer(nullptr, g_shellXamlScanTimer);
        }
        g_shellXamlScanTimer = SetTimer(nullptr, 0, 100, nullptr);
    }
}

DWORD WINAPI RunShellXamlWorker(void* parameter) {
    HANDLE stop = static_cast<HANDLE>(parameter);
    const DWORD pid = GetCurrentProcessId();
    const auto show = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, nullptr,
                                      OnShellXamlWindow, pid, 0, WINEVENT_OUTOFCONTEXT);
    const auto uncloak = SetWinEventHook(EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                                         OnShellXamlWindow, pid, 0, WINEVENT_OUTOFCONTEXT);
    const bool watchVisiblePanels = g_shellHost == ShellHost::Start || g_shellHost == ShellHost::Search ||
                                    g_shellHost == ShellHost::Flyouts;
    // Popups and deferred content can appear without another HWND event.
    constexpr UINT kVisiblePanelScanMs = 750;
    if (RefreshShellXamlWindows(false, watchVisiblePanels) && watchVisiblePanels) {
        g_shellXamlScanTimer = SetTimer(nullptr, 0, kVisiblePanelScanMs, nullptr);
    }
    TaskbarLayoutRetry taskbarLayout;
    while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT) {
        const DWORD timeout = g_shellHost == ShellHost::Explorer ? taskbarLayout.Update() : INFINITE;
        const DWORD result = MsgWaitForMultipleObjects(1, &stop, FALSE, timeout, QS_ALLINPUT);
        if (result == WAIT_TIMEOUT) {
            continue;
        }
        if (result != WAIT_OBJECT_0 + 1) {
            break;
        }
        MSG message;
        while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT &&
               PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (!message.hwnd && message.message == WM_TIMER && message.wParam == g_shellXamlScanTimer) {
                KillTimer(nullptr, std::exchange(g_shellXamlScanTimer, 0));
                g_shellXamlScanPending = true;
            } else {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        if (!g_shellXamlScanTimer && std::exchange(g_shellXamlScanPending, false)) {
            if (RefreshShellXamlWindows(false, watchVisiblePanels) && watchVisiblePanels) {
                g_shellXamlScanTimer = SetTimer(nullptr, 0, kVisiblePanelScanMs, nullptr);
            }
        }
    }
    if (show) {
        UnhookWinEvent(show);
    }
    if (uncloak) {
        UnhookWinEvent(uncloak);
    }
    if (g_shellXamlScanTimer) {
        KillTimer(nullptr, std::exchange(g_shellXamlScanTimer, 0));
    }
    g_shellXamlScanPending = false;
    return 0;
}

void ApplyShellXamlSettings(unsigned mask) {
    if (g_shellXamlMask.exchange(mask) != mask) {
        RefreshShellXamlWindows(true);
    }
    if (mask) {
        g_shellXamlWorker.Start(RunShellXamlWorker);
    } else {
        g_shellXamlWorker.Stop();
    }
}

WindowEffect ReadSetting(PCWSTR name) {
    auto value = WindhawkUtils::StringSetting::make(L"borders.%s", name);
    if (!*value.get()) {
        value = WindhawkUtils::StringSetting::make(name);
    }
    const std::wstring_view setting = value.get();
    if (setting == L"hide") {
        return WindowEffect::HideBorder;
    }
    if (setting == L"caption") {
        return WindowEffect::RemoveCaptionAccent;
    }
    return WindowEffect::Default;
}

std::vector<ProcessException> ReadProcessExceptions() {
    std::vector<ProcessException> entries;
    for (int i = 0;; ++i) {
        auto process = WindhawkUtils::StringSetting::make(L"processExceptions[%d].process", i);
        auto mode = WindhawkUtils::StringSetting::make(L"processExceptions[%d].scope", i);
        std::wstring_view name = process.get();
        std::wstring_view scope = mode.get();
        if (name.empty() && scope.empty()) {
            break;
        }
        auto first = name.find_first_not_of(L" \t\r\n");
        if (first != std::wstring_view::npos) {
            name = name.substr(first, name.find_last_not_of(L" \t\r\n") - first + 1);
            auto exception = scope == L"active" ? WindowException::Active
                           : scope == L"inactive" ? WindowException::Inactive
                                                  : WindowException::Both;
            entries.push_back({std::wstring(name), exception});
        }
    }
    return entries;
}

void ApplySettings() {
    if (g_windowSettingsChanged) {
        WindowSettings settings{
            ReadSetting(L"activeWindow"),
            ReadSetting(L"inactiveWindow") == WindowEffect::HideBorder,
            ReadSetting(L"dialog") == WindowEffect::HideBorder,
        };
        auto exceptions = settings.AnyEnabled() ? ReadProcessExceptions() : std::vector<ProcessException>{};
        WindowUpdate update;
        {
            std::lock_guard guard(g_windowSettingsMutex);
            update = WindowSettingsUpdate(g_requestedWindowSettings, settings);
            g_requestedWindowSettings = settings;
            if (update == WindowUpdate::None && g_requestedWindowExceptions != exceptions) {
                update = WindowUpdate::Refresh;
            }
            g_requestedWindowExceptions = std::move(exceptions);
        }
        if (update == WindowUpdate::Stop) {
            g_windowWorker.Stop();
        } else if (update == WindowUpdate::Refresh) {
            SetEvent(g_windowSettingsChanged.get());
        }
        if (settings.AnyEnabled()) {
            g_windowWorker.Start(RunWindowWorker);
        }
    }

    unsigned borders = 0;
    for (const auto& setting : kShellSettings) {
        if (setting.host == g_shellHost && ReadSetting(setting.key) == WindowEffect::HideBorder) {
            borders |= BorderBit(setting.border);
        }
    }
    ApplyShellXamlSettings(borders);
}

}  // namespace

BOOL Wh_ModInit() {
    WCHAR executable[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, executable, ARRAYSIZE(executable));
    if (!length || length == ARRAYSIZE(executable)) {
        return FALSE;
    }
    PCWSTR name = wcsrchr(executable, L'\\');
    if (!name) {
        return FALSE;
    }
    ++name;
    if (_wcsicmp(name, L"explorer.exe") == 0) {
        g_shellHost = ShellHost::Explorer;
        g_windowSettingsChanged.attach(CreateEventW(nullptr, FALSE, FALSE, nullptr));
    } else if (_wcsicmp(name, L"StartMenuExperienceHost.exe") == 0) {
        g_shellHost = ShellHost::Start;
    } else if (_wcsicmp(name, L"SearchHost.exe") == 0 || _wcsicmp(name, L"SearchApp.exe") == 0) {
        g_shellHost = ShellHost::Search;
    } else if (_wcsicmp(name, L"ShellExperienceHost.exe") == 0 || _wcsicmp(name, L"ShellHost.exe") == 0) {
        g_shellHost = ShellHost::Flyouts;
    } else if (_wcsicmp(name, L"WidgetBoard.exe") == 0) {
        g_shellHost = ShellHost::Widgets;
    }
    if (g_shellHost == ShellHost::Explorer || g_shellHost == ShellHost::Widgets) {
        auto combase = GetModuleHandleW(L"combase.dll");
        auto factory = combase ? reinterpret_cast<decltype(g_getActivationFactory)>(
                                    GetProcAddress(combase, "RoGetActivationFactory")) : nullptr;
        if (!factory || !WindhawkUtils::SetFunctionHook(factory, FindShellFactory, &g_getActivationFactory)) {
            return FALSE;
        }
    }
    return g_shellHost != ShellHost::Unknown;
}

void Wh_ModAfterInit() {
    ApplySettings();
}

void Wh_ModSettingsChanged() {
    ApplySettings();
}

void Wh_ModBeforeUninit() {
    g_shellXamlStopping = true;
    g_shellXamlWorker.Stop();
    std::unordered_set<DWORD> editedThreads;
    {
        std::lock_guard guard(g_shellXamlMutex);
        for (auto const& edit : *g_shellXamlEdits) {
            editedThreads.insert(edit.thread);
        }
        for (auto const& edit : *g_widgetEdits) {
            editedThreads.insert(edit.thread);
        }
    }
    for (DWORD thread : editedThreads) {
        EnumThreadWindows(thread, [](HWND window, LPARAM) -> BOOL {
            RequestShellScan(window, true);
            return FALSE;
        }, 0);
    }
    g_windowWorker.Stop();
    g_windowSettingsChanged.close();
}

void Wh_ModUninit() {
    // An unreachable UI thread may still own the remaining references.
    if (g_shellXamlEdits->empty()) {
        g_shellXamlEdits.reset();
    }
    if (g_widgetEdits->empty()) {
        g_widgetEdits.reset();
    }
}
#endif
