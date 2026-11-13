// ==WindhawkMod==
// @id              windows-border-switch
// @name            Windows Border Switch
// @description     Hide borders around windows, the Start menu, the taskbar, and other system elements
// @description:ru-RU Отключение рамок для окон, меню «Пуск», панели задач и других элементов системы
// @description:es-ES Oculta los bordes de las ventanas, el menú Inicio, la barra de tareas y otros elementos del sistema
// @description:de-DE Blendet die Rahmen von Fenstern, dem Startmenü, der Taskleiste und anderen Systemelementen aus
// @description:pt-BR Oculta as bordas das janelas, do menu Iniciar, da barra de tarefas e de outros elementos do sistema
// @description:zh-CN 隐藏窗口、“开始”菜单、任务栏和其他系统元素的边框
// @version         1.0.0
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
// Build: 15

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
| Active window border | The focused window's system border or its title bar accent color |
| Inactive window borders | System borders of windows without focus |
| Dialog borders | System borders of classic Win32 dialogs, with or without focus |
| Taskbar border | The taskbar edge and the virtual desktops strip at the bottom of Task View |
| Start menu border | The Start menu and its Phone Link panel |
| Search border | The Windows Search panel |
| Notification borders | The notification center, calendar, and notification banners on the desktop |
| Quick Settings border | Quick Settings and the media controls above them |
| Widgets board border | The outer border of the Widgets board |

The interface is available in English, Russian, Spanish, German, Brazilian Portuguese, and Simplified Chinese.

## Platforms

Requires Windows 11 x64 or ARM64. Shell panel support depends on the Windows build and Windows Web Experience Pack. Applications with custom-drawn frames and title bars control their own appearance.

## Usage

Open the mod's **Settings** to change each setting independently. Initially, the mod keeps the active window border, removes its title bar accent color, and hides the other listed borders. Choose **Default** to restore Windows styling for a setting.

## Links

- [Windows Border Switch on GitHub](https://github.com/CTravkin/DSN_Windhawk_Mods/tree/main/Windows_Border_Switch)
- [DSN Windhawk Mods](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/README.md)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activeWindow: caption
  $name: Active window border
  $name:ru-RU: Рамка активного окна
  $name:es-ES: Borde de la ventana activa
  $name:de-DE: Rahmen des aktiven Fensters
  $name:pt-BR: Borda da janela ativa
  $name:zh-CN: 活动窗口边框
  $options:
  - default: Default
  - hide: Hide border
  - caption: Disable title bar accent color
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  - caption: Отключить акцент заголовка
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  - caption: Desactivar el color de énfasis de la barra de título
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  - caption: Akzentfarbe der Titelleiste deaktivieren
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  - caption: Desativar a cor de destaque da barra de título
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
  - caption: 关闭标题栏主题色
- inactiveWindow: hide
  $name: Inactive window borders
  $name:ru-RU: Рамки неактивных окон
  $name:es-ES: Bordes de las ventanas inactivas
  $name:de-DE: Rahmen inaktiver Fenster
  $name:pt-BR: Bordas das janelas inativas
  $name:zh-CN: 非活动窗口边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- dialog: hide
  $name: Dialog borders
  $name:ru-RU: Рамки диалоговых окон
  $name:es-ES: Bordes de los cuadros de diálogo
  $name:de-DE: Rahmen von Dialogfeldern
  $name:pt-BR: Bordas das caixas de diálogo
  $name:zh-CN: 对话框边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- taskbar: hide
  $name: Taskbar border
  $name:ru-RU: Рамка панели задач
  $name:es-ES: Borde de la barra de tareas
  $name:de-DE: Taskleistenrahmen
  $name:pt-BR: Borda da barra de tarefas
  $name:zh-CN: 任务栏边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- startMenu: hide
  $name: Start menu border
  $name:ru-RU: Рамка меню «Пуск»
  $name:es-ES: Borde del menú Inicio
  $name:de-DE: Startmenürahmen
  $name:pt-BR: Borda do menu Iniciar
  $name:zh-CN: “开始”菜单边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- search: hide
  $name: Search border
  $name:ru-RU: Рамка поиска
  $name:es-ES: Borde de la búsqueda
  $name:de-DE: Rahmen der Suche
  $name:pt-BR: Borda da pesquisa
  $name:zh-CN: 搜索边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- notificationCenter: hide
  $name: Notification borders
  $name:ru-RU: Уведомления
  $name:es-ES: Bordes de las notificaciones
  $name:de-DE: Benachrichtigungsrahmen
  $name:pt-BR: Bordas das notificações
  $name:zh-CN: 通知边框
  $description: Includes the notification center, calendar, and notification banners
  $description:ru-RU: Центр уведомлений, календарь и баннеры уведомлений
  $description:es-ES: Incluye el centro de notificaciones, el calendario y los banners de notificación
  $description:de-DE: Umfasst das Benachrichtigungscenter, den Kalender und Benachrichtigungsbanner
  $description:pt-BR: Inclui a central de notificações, o calendário e os banners de notificação
  $description:zh-CN: 包括通知中心、日历和通知横幅
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- quickSettings: hide
  $name: Quick Settings border
  $name:ru-RU: Рамка быстрых настроек
  $name:es-ES: Borde de la configuración rápida
  $name:de-DE: Rahmen der Schnelleinstellungen
  $name:pt-BR: Borda das configurações rápidas
  $name:zh-CN: 快速设置边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- widgets: hide
  $name: Widgets board border
  $name:ru-RU: Рамка панели мини-приложений
  $name:es-ES: Borde del panel de widgets
  $name:de-DE: Rahmen des Widgetboards
  $name:pt-BR: Borda do quadro de widgets
  $name:zh-CN: 小组件板边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
*/
// ==/WindhawkModSettings==

#include <string_view>

namespace {

constexpr unsigned kMaxDiagnosticEndpoint = 10000;

template <typename Probe, typename ShouldStop, typename Result>
Result FindDiagnosticEndpoint(Probe probe, ShouldStop shouldStop,
                              Result notFound, Result cancelled) {
    for (unsigned index = 1; index <= kMaxDiagnosticEndpoint; ++index) {
        if (shouldStop()) {
            return cancelled;
        }
        Result result = probe(index);
        if (result != notFound) {
            return result;
        }
    }
    return notFound;
}

enum class WindowEffect { Default, HideBorder, RemoveCaptionAccent };

struct WindowSettings {
    WindowEffect active = WindowEffect::Default;
    bool hideInactive = false;
    bool hideDialog = false;

    bool AnyEnabled() const {
        return active != WindowEffect::Default || hideInactive || hideDialog;
    }
};

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

    bool operator==(const WindowFrame&) const = default;
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

}  // namespace

#ifndef BORDER_SWITCH_POLICY_TEST
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>
#include <ocidl.h>
#include <xamlom.h>
#include <windhawk_api.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

namespace Foundation = winrt::Windows::Foundation;
namespace Xaml = winrt::Windows::UI::Xaml;
namespace Core = winrt::Windows::UI::Core;
namespace System = winrt::Windows::System;
namespace WinUI = winrt::Microsoft::UI::Xaml;
namespace Dispatching = winrt::Microsoft::UI::Dispatching;

ShellHost g_shellHost;
unsigned g_shellBorders;

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
    } else if (g_shellHost == ShellHost::Widgets ||
               ClassIs(window, L"Progman") || ClassIs(window, L"Shell_TrayWnd") ||
               ClassIs(window, L"Windows.UI.Core.CoreWindow") ||
               ClassIs(window, L"Windows.UI.Composition.DesktopWindowContentBridge") ||
               ClassIs(window, L"Microsoft.UI.Content.DesktopChildSiteBridge") ||
               ClassIs(window, L"ControlCenterWindow")) {
        g_shellWindowChanged = true;
    }
}

bool HasFlyoutWindow() {
    return !EnumWindows([](HWND window, LPARAM) -> BOOL {
        DWORD process = 0;
        GetWindowThreadProcessId(window, &process);
        return process != GetCurrentProcessId() ||
               !(ClassIs(window, L"Windows.UI.Core.CoreWindow") ||
                 ClassIs(window, L"ControlCenterWindow"));
    }, 0);
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
    bool accent = hasCaption && !dialog && effect != WindowEffect::Default &&
                  CaptionAccentEnabled();
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
        const HANDLE events[] = {stop, themeChanged.get()};
        UINT_PTR themeTimer = 0;
        RefreshWindows();
        for (;;) {
            const DWORD count = watchingTheme ? ARRAYSIZE(events) : 1;
            const DWORD result = MsgWaitForMultipleObjects(count, events, FALSE, INFINITE,
                                                            QS_ALLINPUT);
            if (watchingTheme && result == WAIT_OBJECT_0 + 1) {
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

constexpr CLSID kTapClass = {0x50127d3e, 0xe980, 0x47b2,
                            {0xa4, 0xde, 0x33, 0xa0, 0x27, 0x85, 0xad, 0x67}};
constexpr GUID kDiagnosticsTestHooks = {0x735941a2, 0x3ee3, 0x495a,
                                        {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct DiagnosticsTestHooks : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
};

HMODULE OwnModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(&OwnModule), &module);
    return module;
}

using XamlInitialize = decltype(&InitializeXamlDiagnosticsEx);
using OpenKey = decltype(&RegOpenKeyExW);
using QueryValue = decltype(&RegQueryValueExW);

OpenKey g_openKey;
QueryValue g_queryValue;
thread_local HKEY g_compositionDebugKey;
thread_local bool g_advising;

LSTATUS WINAPI OpenKeyHook(HKEY key, PCWSTR name, DWORD options, REGSAM access, HKEY* result) {
    LSTATUS status = g_openKey(key, name, options, access, result);
    if (g_advising && key == HKEY_LOCAL_MACHINE && name && result &&
        _wcsicmp(name, L"Software\\Microsoft\\XAML\\Debug") == 0) {
        if (status == ERROR_FILE_NOT_FOUND) {
            status = g_openKey(key, L"Software\\Microsoft", options, access, result);
        }
        if (status == ERROR_SUCCESS) {
            g_compositionDebugKey = *result;
        }
    }
    return status;
}

LSTATUS WINAPI QueryValueHook(HKEY key, PCWSTR name, DWORD* reserved, DWORD* type,
                             BYTE* data, DWORD* size) {
    // Element callbacks do not need the process-wide composition diagnostics.
    if (!g_advising || !key || key != g_compositionDebugKey || !name ||
        _wcsicmp(name, L"DisableCompositionDiag") != 0 || !size) {
        return g_queryValue(key, name, reserved, type, data, size);
    }
    if (type) {
        *type = REG_DWORD;
    }
    if (data && *size < sizeof(DWORD)) {
        *size = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    if (data) {
        DWORD disabled = 1;
        memcpy(data, &disabled, sizeof(disabled));
    }
    *size = sizeof(DWORD);
    return ERROR_SUCCESS;
}

struct ShellEdit {
    DWORD thread;
    std::function<bool()> expired;
    std::function<void()> restore;
    std::function<bool(std::function<void()>)> dispatch;
};

template <typename Element, typename Property, typename Value>
ShellEdit TrackShellEdit(Element const& target, Property const& property,
                         Foundation::IInspectable const& unset,
                         Value const& replacement,
                         std::function<bool(std::function<void()>)> dispatch) {
    auto weak = winrt::make_weak(target);
    auto original = target.ReadLocalValue(property);
    if (original == unset) {
        original = nullptr;
    }
    return {
        GetCurrentThreadId(),
        [weak] { return !weak.get(); },
        [weak, property, original, replacement] {
            auto element = weak.get();
            if (!element) {
                return;
            }
            auto local = element.ReadLocalValue(property);
            bool matches;
            if constexpr (std::is_same_v<Value, Foundation::IInspectable>) {
                matches = local == replacement;
            } else {
                auto value = local.template try_as<Foundation::IReference<Value>>();
                matches = value && value.Value() == replacement;
            }
            if (!matches) {
                return;
            }
            if (original) {
                element.SetValue(property, original);
            } else {
                element.ClearValue(property);
            }
            Wh_Log(L"Restored shell border: %s", element.Name().c_str());
        },
        std::move(dispatch),
    };
}

void RestoreShellEdit(ShellEdit const& edit) {
    winrt::handle thread{OpenThread(SYNCHRONIZE, FALSE, edit.thread)};
    winrt::handle done{CreateEventW(nullptr, TRUE, FALSE, nullptr)};
    if (!thread || !done) {
        return;
    }
    // Keep the completion signal alive until the dispatcher releases the callback.
    HANDLE duplicate = nullptr;
    if (!DuplicateHandle(GetCurrentProcess(), done.get(), GetCurrentProcess(), &duplicate,
                         EVENT_MODIFY_STATE, FALSE, 0)) {
        return;
    }
    auto signal = std::make_shared<winrt::handle>(duplicate);
    if (edit.dispatch([restore = edit.restore, signal] {
            try {
                restore();
            } catch (...) {
                Wh_Log(L"Unable to restore XAML border: %08X", winrt::to_hresult());
            }
            SetEvent(signal->get());
        })) {
        HANDLE events[] = {done.get(), thread.get()};
        WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
    }
}

struct TreeWatcher : winrt::implements<TreeWatcher, IVisualTreeServiceCallback2> {
    explicit TreeWatcher(winrt::com_ptr<IXamlDiagnostics> diagnostics)
        : m_diagnostics(std::move(diagnostics)), m_borders(g_shellBorders) {
        m_diagnostics->QueryInterface(kDiagnosticsTestHooks, m_testHooks.put_void());
    }

    HRESULT Advise() {
        // Without UnregisterInstance, visiting an active shell can retain its XAML objects.
        if (!m_testHooks && g_shellHost != ShellHost::Widgets) {
            return E_NOINTERFACE;
        }
        auto service = m_diagnostics.as<IVisualTreeService3>();
        g_advising = true;
        HRESULT result = service->AdviseVisualTreeChange(this);
        g_advising = false;
        g_compositionDebugKey = nullptr;
        return result;
    }

    void Stop(bool advised) {
        m_stopping = true;
        if (advised) {
            HRESULT result = m_diagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
            if (FAILED(result)) {
                Wh_Log(L"Unable to unsubscribe from XAML notifications: %08X", result);
            }
        }

        DrainReleaseCallbacks();
        std::unordered_map<InstanceHandle, ShellEdit> edits;
        {
            std::lock_guard guard(m_mutex);
            edits.swap(m_edits);
        }
        for (auto& [handle, edit] : edits) {
            try {
                RestoreShellEdit(edit);
            } catch (...) {
                Wh_Log(L"Unable to restore XAML border: %08X", winrt::to_hresult());
            }
        }
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation,
                                                 VisualElement element,
                                                 VisualMutationType mutation) noexcept override {
        if (m_stopping) {
            return S_OK;
        }
        try {
            if (mutation == Add && element.Type && element.Name) {
                auto border = ShellBorderFor(g_shellHost, element.Type, element.Name);
                if (m_borders & BorderBit(border)) {
                    Apply(element.Handle, border);
                }
            }
            // XAML reports both handles and may keep them alive even if they were
            // never resolved. Release after the current visual-tree walk returns.
            if (m_testHooks && g_shellHost != ShellHost::Widgets) {
                QueueRelease(element.Handle, relation.Parent);
            }
        } catch (...) {
            Wh_Log(L"Unable to update XAML border: %08X", winrt::to_hresult());
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState,
                                                    PCWSTR) noexcept override {
        return S_OK;
    }

private:
    void Apply(InstanceHandle handle, ShellBorder border) {
        Foundation::IInspectable inspectable;
        if (FAILED(m_diagnostics->GetIInspectableFromHandle(
                handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(inspectable))))) {
            return;
        }
        std::lock_guard guard(m_mutex);
        if (m_stopping) {
            return;
        }
        std::erase_if(m_edits, [](const auto& entry) {
            return entry.second.thread == GetCurrentThreadId() && entry.second.expired();
        });
        if (m_edits.contains(handle)) {
            return;
        }
        if (border == ShellBorder::Widgets) {
            auto target = inspectable.try_as<WinUI::FrameworkElement>();
            if (!target) {
                return;
            }
            auto queue = target.DispatcherQueue();
            if (!queue) {
                return;
            }
            auto dispatch = [queue](std::function<void()> run) {
                return queue.TryEnqueue([run = std::move(run)] { run(); });
            };
            auto unset = WinUI::DependencyProperty::UnsetValue();
            // WidgetBoard uses the outline width for layout. Hide the brush to
            // preserve that geometry and the acrylic backdrop.
            auto brush = WinUI::Media::SolidColorBrush(winrt::Windows::UI::Color{});
            if (target.try_as<WinUI::Controls::Control>()) {
                ApplyProperty(handle, target, WinUI::Controls::Control::BorderBrushProperty(),
                              unset, Foundation::IInspectable{brush}, dispatch);
            } else if (target.try_as<WinUI::Controls::Grid>()) {
                ApplyProperty(handle, target, WinUI::Controls::Grid::BorderBrushProperty(),
                              unset, Foundation::IInspectable{brush}, dispatch);
            } else if (target.try_as<WinUI::Controls::Border>()) {
                ApplyProperty(handle, target, WinUI::Controls::Border::BorderBrushProperty(),
                              unset, Foundation::IInspectable{brush}, dispatch);
            }
            return;
        }

        auto target = inspectable.try_as<Xaml::FrameworkElement>();
        if (!target) {
            return;
        }
        if (border == ShellBorder::Start) {
            auto parent = Xaml::Media::VisualTreeHelper::GetParent(target)
                              .try_as<Xaml::FrameworkElement>();
            if (!parent || (!IsStartPane(parent.Name()) &&
                            !(parent.Name().empty() &&
                              winrt::get_class_name(parent) == L"Windows.UI.Xaml.Controls.Grid"))) {
                return;
            }
        }
        auto dispatcher = target.Dispatcher();
        if (!dispatcher) {
            return;
        }
        auto dispatch = [dispatcher](std::function<void()> run) {
            dispatcher.RunAsync(Core::CoreDispatcherPriority::Normal,
                                [run = std::move(run)] { run(); });
            return true;
        };
        auto unset = Xaml::DependencyProperty::UnsetValue();
        if (border == ShellBorder::Taskbar) {
            ApplyProperty(handle, target, Xaml::UIElement::VisibilityProperty(), unset,
                          Xaml::Visibility::Collapsed, dispatch);
            return;
        }
        auto property = target.try_as<Xaml::Controls::Grid>()
                            ? Xaml::Controls::Grid::BorderThicknessProperty()
                            : Xaml::Controls::Border::BorderThicknessProperty();
        ApplyProperty(handle, target, property, unset, Xaml::Thickness{}, dispatch);
    }

    template <typename Element, typename Property, typename Value>
    void ApplyProperty(InstanceHandle handle, Element const& target, Property const& property,
                       Foundation::IInspectable const& unset,
                       Value const& replacement,
                       std::function<bool(std::function<void()>)> dispatch) {
        auto edit = TrackShellEdit(target, property, unset, replacement, std::move(dispatch));
        auto [entry, inserted] = m_edits.emplace(handle, std::move(edit));
        if (!inserted) {
            return;
        }
        try {
            if constexpr (std::is_same_v<Value, Foundation::IInspectable>) {
                target.SetValue(property, replacement);
            } else {
                target.SetValue(property, winrt::box_value(replacement));
            }
        } catch (...) {
            m_edits.erase(entry);
            throw;
        }
        Wh_Log(L"Updated shell border: %s", target.Name().c_str());
    }

    void QueueRelease(InstanceHandle element, InstanceHandle parent) {
        DWORD thread = GetCurrentThreadId();
        std::lock_guard guard(m_releaseMutex);
        if (m_stopping) {
            return;
        }
        auto& batch = m_releases[thread];
        if (!batch.queue) {
            batch.queue = System::DispatcherQueue::GetForCurrentThread();
            if (!batch.queue) {
                m_releases.erase(thread);
                return;
            }
        }
        if (element) {
            batch.pending.push_back(element);
        }
        if (parent && parent != element) {
            batch.pending.push_back(parent);
        }
        if (batch.pending.empty() || batch.scheduled) {
            return;
        }
        batch.scheduled = batch.queue.TryEnqueue(
            System::DispatcherQueuePriority::Low,
            [self = get_strong(), thread] { self->ReleaseHandles(thread); });
    }

    void ReleaseHandles(DWORD thread) noexcept {
        std::vector<InstanceHandle> handles;
        {
            std::lock_guard guard(m_releaseMutex);
            auto& batch = m_releases.at(thread);
            handles.swap(batch.pending);
            batch.scheduled = false;
        }

        std::sort(handles.begin(), handles.end());
        handles.erase(std::unique(handles.begin(), handles.end()), handles.end());
        for (InstanceHandle handle : handles) {
            m_testHooks->UnregisterInstance(handle);
        }
    }

    void DrainReleaseCallbacks() {
        std::vector<std::pair<DWORD, System::DispatcherQueue>> queues;
        {
            std::lock_guard guard(m_releaseMutex);
            for (auto& [thread, batch] : m_releases) {
                queues.emplace_back(thread, batch.queue);
            }
        }
        for (auto& [thread, queue] : queues) {
            HANDLE uiThread = OpenThread(SYNCHRONIZE, FALSE, thread);
            if (!uiThread) {
                continue;
            }
            HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
            try {
                if (done && queue.TryEnqueue(System::DispatcherQueuePriority::Low,
                                             [done] { SetEvent(done); })) {
                    HANDLE events[] = {done, uiThread};
                    WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
                }
            } catch (...) {
                Wh_Log(L"XAML dispatcher is unavailable: %08X", winrt::to_hresult());
            }
            if (done) {
                CloseHandle(done);
            }
            CloseHandle(uiThread);
        }
    }

    struct ReleaseBatch {
        System::DispatcherQueue queue{nullptr};
        std::vector<InstanceHandle> pending;
        bool scheduled = false;
    };

    winrt::com_ptr<IXamlDiagnostics> m_diagnostics;
    const unsigned m_borders;
    winrt::com_ptr<DiagnosticsTestHooks> m_testHooks;
    std::atomic<bool> m_stopping = false;
    std::mutex m_mutex;
    std::unordered_map<InstanceHandle, ShellEdit> m_edits;
    std::mutex m_releaseMutex;
    std::unordered_map<DWORD, ReleaseBatch> m_releases;
};

Worker g_shellWorker;
std::mutex g_siteMutex;
winrt::com_ptr<IXamlDiagnostics> g_site;
HANDLE g_siteReady;

struct Tap : winrt::implements<Tap, IObjectWithSite, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) noexcept override {
        m_site.copy_from(site);
        if (site) {
            // The TAP loader adds a reference independently of Windhawk.
            FreeLibrary(OwnModule());
            std::lock_guard guard(g_siteMutex);
            if (g_siteReady) {
                g_site = m_site.try_as<IXamlDiagnostics>();
                SetEvent(g_siteReady);
            }
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void** result) noexcept override {
        return m_site ? m_site->QueryInterface(iid, result) : E_FAIL;
    }

    winrt::com_ptr<IUnknown> m_site;
};

struct TapFactory : winrt::implements<TapFactory, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID iid,
                                             void** result) noexcept override try {
        *result = nullptr;
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }
        return winrt::make_self<Tap>()->QueryInterface(iid, result);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL lock) noexcept override {
        if (lock) {
            ++winrt::get_module_lock();
        } else {
            --winrt::get_module_lock();
        }
        return S_OK;
    }
};

HRESULT AttachTap(HANDLE stop) {
    bool widgets = g_shellHost == ShellHost::Widgets;
    auto xaml = GetModuleHandleW(widgets ? L"Microsoft.Internal.FrameworkUdk.dll"
                                        : L"Windows.UI.Xaml.dll");
    if (!xaml) {
        return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    }
    auto initialize = reinterpret_cast<XamlInitialize>(
        GetProcAddress(xaml, "InitializeXamlDiagnosticsEx"));
    if (!initialize) {
        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }
    WCHAR path[32768];
    DWORD length = GetModuleFileNameW(OwnModule(), path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    // The default connection is normally the first one. Multiple XAML islands
    // can have higher-numbered endpoints.
    DWORD pid = GetCurrentProcessId();
    return FindDiagnosticEndpoint(
        [&](unsigned index) {
            WCHAR connection[40];
            swprintf_s(connection, L"%s%u",
                       widgets ? L"WinUIVisualDiagConnection" : L"VisualDiagConnection", index);
            return initialize(connection, pid, L"", path, kTapClass, nullptr);
        },
        [stop] { return WaitForSingleObject(stop, 0) != WAIT_TIMEOUT; },
        HRESULT_FROM_WIN32(ERROR_NOT_FOUND), HRESULT_FROM_WIN32(ERROR_CANCELLED));
}

HRESULT ConnectShellTap(HANDLE stop, winrt::com_ptr<IXamlDiagnostics>& site) {
    HANDLE siteReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!siteReady) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    {
        std::lock_guard guard(g_siteMutex);
        g_siteReady = siteReady;
    }
    HRESULT attach = AttachTap(stop);
    // InitializeXamlDiagnosticsEx can return before the UI thread calls SetSite.
    if (SUCCEEDED(attach)) {
        const HANDLE events[] = {stop, siteReady};
        WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, 5000);
    }
    {
        std::lock_guard guard(g_siteMutex);
        g_siteReady = nullptr;
        site = std::move(g_site);
    }
    CloseHandle(siteReady);
    return site || FAILED(attach) ? attach : HRESULT_FROM_WIN32(ERROR_TIMEOUT);
}

DWORD WINAPI RunShellWorker(void* stopParameter) {
    HANDLE stop = static_cast<HANDLE>(stopParameter);
    if (g_shellHost == ShellHost::Explorer && !WaitForShellOwnership(stop)) {
        return 0;
    }
    HRESULT apartment = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(apartment)) {
        return 0;
    }
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, nullptr,
                                       OnShellWindowEvent, GetCurrentProcessId(), 0,
                                       WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK uncloak = SetWinEventHook(EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED,
                                          nullptr, OnShellWindowEvent, GetCurrentProcessId(), 0,
                                          WINEVENT_OUTOFCONTEXT);
    winrt::com_ptr<IXamlDiagnostics> site;
    constexpr DWORD delays[] = {250, 1000, 4000, 10000};
    unsigned retry = 0;
    while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT) {
        // ShellHost and ShellExperienceHost create their flyout XAML lazily.
        // Wait until the UI window exists before attaching to its diagnostics.
        if (g_shellHost == ShellHost::Flyouts && !site && !HasFlyoutWindow()) {
            if (!WaitForShellActivity(stop, hook || uncloak ? INFINITE : 1000)) {
                break;
            }
            continue;
        }
        try {
            HRESULT result = site ? S_OK : ConnectShellTap(stop, site);
            if (SUCCEEDED(result) && WaitForSingleObject(stop, 0) == WAIT_TIMEOUT) {
                auto watcher = winrt::make_self<TreeWatcher>(site);
                result = watcher->Advise();
                if (SUCCEEDED(result)) {
                    Wh_Log(L"Subscribed to shell XAML");
                    if (hook) {
                        UnhookWinEvent(std::exchange(hook, nullptr));
                    }
                    if (uncloak) {
                        UnhookWinEvent(std::exchange(uncloak, nullptr));
                    }
                    WaitForSingleObject(stop, INFINITE);
                    watcher->Stop(true);
                    break;
                }
                watcher->Stop(false);
            }
            Wh_Log(L"Shell XAML is unavailable: %08X", result);
            if (result == E_NOINTERFACE ||
                result == HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND)) {
                break;
            }
        } catch (...) {
            Wh_Log(L"Shell worker failed: %08X", winrt::to_hresult());
            break;
        }

        DWORD delay = retry < ARRAYSIZE(delays) ? delays[retry++] : INFINITE;
        if (!WaitForShellActivity(stop, delay)) {
            break;
        }
        if (delay == INFINITE) {
            retry = 0;
        }
    }
    if (hook) {
        UnhookWinEvent(hook);
    }
    if (uncloak) {
        UnhookWinEvent(uncloak);
    }
    site = nullptr;
    CoUninitialize();
    return 0;
}

bool g_shellHooks;

bool InstallShellHooks() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        return false;
    }
    auto openKey = reinterpret_cast<void*>(GetProcAddress(kernelBase, "RegOpenKeyExW"));
    auto queryValue = reinterpret_cast<void*>(GetProcAddress(kernelBase, "RegQueryValueExW"));
    return openKey && queryValue &&
           Wh_SetFunctionHook(openKey, reinterpret_cast<void*>(&OpenKeyHook),
                              reinterpret_cast<void**>(&g_openKey)) &&
           Wh_SetFunctionHook(queryValue, reinterpret_cast<void*>(&QueryValueHook),
                              reinterpret_cast<void**>(&g_queryValue));
}

WindowEffect ReadSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring_view setting = value ? value : L"";
    WindowEffect effect = WindowEffect::Default;
    if (setting == L"hide") {
        effect = WindowEffect::HideBorder;
    } else if (setting == L"caption") {
        effect = WindowEffect::RemoveCaptionAccent;
    }
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return effect;
}

void ApplySettings() {
    if (g_shellHost == ShellHost::Explorer) {
        WindowSettings settings{
            ReadSetting(L"activeWindow"),
            ReadSetting(L"inactiveWindow") == WindowEffect::HideBorder,
            ReadSetting(L"dialog") == WindowEffect::HideBorder,
        };
        if (settings.active != g_windowSettings.active ||
            settings.hideInactive != g_windowSettings.hideInactive ||
            settings.hideDialog != g_windowSettings.hideDialog) {
            g_windowWorker.Stop();
            g_windowSettings = settings;
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
    if (borders != g_shellBorders) {
        g_shellWorker.Stop();
        g_shellBorders = borders;
    }
    if (borders) {
        if (!g_shellHooks) {
            g_shellHooks = InstallShellHooks() && Wh_ApplyHookOperations();
        }
        if (g_shellHooks) {
            g_shellWorker.Start(RunShellWorker);
        }
    }
}

}  // namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) STDAPI DllGetClassObject(REFCLSID clsid, REFIID iid, void** result) try {
    *result = nullptr;
    if (clsid != kTapClass) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    return winrt::make_self<TapFactory>()->QueryInterface(iid, result);
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

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
    } else if (_wcsicmp(name, L"StartMenuExperienceHost.exe") == 0) {
        g_shellHost = ShellHost::Start;
    } else if (_wcsicmp(name, L"SearchHost.exe") == 0 || _wcsicmp(name, L"SearchApp.exe") == 0) {
        g_shellHost = ShellHost::Search;
    } else if (_wcsicmp(name, L"ShellExperienceHost.exe") == 0 || _wcsicmp(name, L"ShellHost.exe") == 0) {
        g_shellHost = ShellHost::Flyouts;
    } else if (_wcsicmp(name, L"WidgetBoard.exe") == 0) {
        g_shellHost = ShellHost::Widgets;
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
    g_shellWorker.Stop();
    g_windowWorker.Stop();
}
#endif
