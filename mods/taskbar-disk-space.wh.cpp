// ==WindhawkMod==
// @id              taskbar-disk-space
// @name            Taskbar Disk Space
// @name:ru-RU      Место на диске на панели задач
// @description     Show a local drive name and its free/total space on the Windows 11 taskbar.
// @description:ru-RU Имя локального диска и свободное/общее место на панели задач Windows 11.
// @version         0.11.0
// @author          fatal
// @github          https://github.com/Fatalko
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Taskbar XAML discovery is adapted from taskbar-multirow by Michael Maltsev
// and taskbar-system-info by Yevhenii Starychenko (both GPL-3.0):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-system-info.wh.cpp
// This program is free software under GPL v3; WITHOUT ANY WARRANTY.
// License: https://www.gnu.org/licenses/gpl-3.0.html

// ==WindhawkModReadme==
/*
# Место на диске на панели задач

При отображаемом имени — две строки; при скрытом имени — одна компактная строка:

```
Данные (D:)
Свободно 128,4 из 931,5 ГиБ
```

- Нажмите на индикатор, чтобы открыть список дисков прямо на панели задач.
  Выбранный диск сохраняется в локальном хранилище мода.
- В настройках можно задать начальный диск. По умолчанию — C:.
- Имя берётся из метки тома. Поле «Своё имя диска» позволяет его заменить.
- Настройка «Скрывать имя диска» выключена по умолчанию. При включении верхняя
  строка содержит только букву, например `(E:)`.
- При отображаемом имени используются две строки: имя сверху, объём снизу.
  При скрытом имени используется одна компактная строка: **(E:) X / Y ГиБ**,
  без слов «Свободно» и «из».
- При наведении бледная полоса показывает долю свободного и занятого места.
  Доступны десять заранее заготовленных пар цветов; по умолчанию используются
  зелёный для свободного и красный для занятого места.
- Значения обновляются автоматически, по умолчанию каждые 10 минут
  (600 секунд); интервал можно изменить в настройках.
- Проверка диска только читает метаданные и не записывает данные на накопитель.
- ГиБ = 1024³ байт, как при расчёте размеров в Проводнике.
- Недоступный, заблокированный или отсутствующий диск отображается как
  «Диск недоступен», а не как диск с нулевым свободным местом.

## Совместимость

Windhawk 1.7.3 и 2.0 используют один исходник. В 1.7.3 доступен список букв
A:–Z:. В 2.0 обнаруженные локальные диски дополнительно подписаны именами,
а ширина и отступ настраиваются ползунками. Новые аннотации помечены `#! `.
Неиспользуемые буквы остаются в списке, чтобы можно было выбрать временно
отключённый диск. Список имён обновляется раз в 30 секунд.

Штатная панель Windows 11 22H2 и новее. На сборках с изменёнными внутренними
символами панели задач может потребоваться обновление мода. Для первого
запуска Windhawk может загрузить отладочные символы Microsoft.
ExplorerPatcher и StartAllBack не поддерживаются.

## Размещение

Индикатор находится слева на основной панели задач. Нажатие по нему открывает
штатное XAML-меню со списком доступных локальных фиксированных дисков; остальные
области панели задач работают как обычно.
По умолчанию оставлен отступ 160, чтобы разместить его после «Виджетов».
Если «Виджеты» отключены, уменьшите отступ до 12.
При наложении на погоду увеличьте отступ. Настройка «Зарезервировать место»
добавляет отступ перед кнопками приложений; отключение мода его убирает.
Высота рамки совпадает с высотой рамки кнопки «Пуск»; режим с именем использует
две строки, а режим со скрытым именем — одну компактную строку.
Другие моды, меняющие отступы панели, могут потребовать ручной настройки.

Поддерживаются тома с буквами, которые Windows определяет как локальные
фиксированные диски, включая внешние SSD с таким типом. Сетевые диски и
съёмные флешки не включаются. При дисковых квотах показывается объём,
доступный текущему пользователю. Изменения настроек применяются без
перезапуска Проводника.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Drive: "C:"
  $name: Диск
  $description: Начальный диск. После запуска диск можно менять нажатием по индикатору на панели задач. В Windhawk 2.0 также видны метки обнаруженных томов.
  #! $dynamicSelect: true
  $options:
    - "A:": "A:"
    - "B:": "B:"
    - "C:": "C:"
    - "D:": "D:"
    - "E:": "E:"
    - "F:": "F:"
    - "G:": "G:"
    - "H:": "H:"
    - "I:": "I:"
    - "J:": "J:"
    - "K:": "K:"
    - "L:": "L:"
    - "M:": "M:"
    - "N:": "N:"
    - "O:": "O:"
    - "P:": "P:"
    - "Q:": "Q:"
    - "R:": "R:"
    - "S:": "S:"
    - "T:": "T:"
    - "U:": "U:"
    - "V:": "V:"
    - "W:": "W:"
    - "X:": "X:"
    - "Y:": "Y:"
    - "Z:": "Z:"
- DisplayName: ""
  $name: Своё имя диска
  $description: Оставьте пустым, чтобы использовать метку тома из Windows. Буква диска отображается всегда.
- HideDriveName: false
  $name: Скрывать имя диска
  $description: Оставляет только букву диска в верхней строке, например (E:). По умолчанию выключено.
- FreeColor: "green-red"
  $name: Цвета свободно/всего
  $description: Заранее заготовленная пара цветов для свободного и занятого места при наведении.
  $options:
    - "green-red": "Зелёный / красный — Green / Red"
    - "blue-orange": "Синий / оранжевый — Blue / Orange"
    - "cyan-purple": "Бирюзовый / фиолетовый — Cyan / Purple"
    - "violet-yellow": "Фиолетовый / жёлтый — Violet / Yellow"
    - "teal-pink": "Бирюзовый / розовый — Teal / Pink"
    - "lime-indigo": "Лаймовый / индиго — Lime / Indigo"
    - "amber-navy": "Янтарный / тёмно-синий — Amber / Navy"
    - "mint-coral": "Мятный / коралловый — Mint / Coral"
    - "sky-magenta": "Небесный / пурпурный — Sky / Magenta"
    - "white-gray": "Белый / серый — White / Gray"
- UpdateInterval: 600
  $name: Интервал обновления (секунды)
  #! $min: 1
  #! $max: 3600
- Width: 260
  $name: Максимальная ширина индикатора
  #! $min: 180
  #! $max: 600
  #! $format: slider
- LeftOffset: 160
  $name: Отступ от левого края
  $description: При включённой погоде оставьте место для неё. Если погода отключена, можно указать 12.
  #! $min: 0
  #! $max: 1200
  #! $format: slider
- ReserveSpace: false
  $name: Зарезервировать место перед кнопками приложений
  $description: Добавляет отступ, чтобы кнопки Пуск и приложений не перекрывали индикатор.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <limits>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

namespace {
constexpr wchar_t kWidgetName[] = L"WindhawkTaskbarDiskSpace";
struct Settings {
    std::wstring drive;
    std::wstring displayName;
    bool hideDriveName = false;
    std::wstring colorScheme = L"green-red";
    int interval = 600;
    int width = 260;
    int offset = 160;
    bool reserve = false;
};
struct Reading {
    std::wstring title;
    std::wstring capacity;
    double freeRatio = 0.0;
    bool hasRatio = false;
};
// XAML references must be released on their owning UI thread, never by static
// destructors during Explorer process shutdown.
struct UiState {
    Grid root{nullptr};
    Border surface{nullptr};
    LinearGradientBrush hoverBrush{nullptr};
    GradientStop freeStop{nullptr};
    GradientStop freeEndStop{nullptr};
    GradientStop usedStartStop{nullptr};
    GradientStop usedStop{nullptr};
    MenuFlyout driveMenu{nullptr};
    StackPanel widget{nullptr};
    TextBlock title{nullptr};
    TextBlock capacity{nullptr};
    FrameworkElement repeater{nullptr};
    double reserved = 0;
    double lastMargin = 0;
    bool marginApplied = false;
    DWORD thread = 0;
    winrt::event_token tappedToken{};
    bool hasTappedHandler = false;
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    bool hasPointerHandlers = false;
    bool hovered = false;
    double freeRatio = 0.0;
    bool hasRatio = false;
    std::wstring colorScheme = L"green-red";
};
[[clang::no_destroy]] UiState g_ui;
[[clang::no_destroy]] Settings g_settings;
std::mutex g_settingsMutex;
HANDLE g_stop = nullptr;
HANDLE g_changed = nullptr;
HANDLE g_worker = nullptr;
HMODULE g_taskbarModule = nullptr;
UINT g_dispatchMessage = 0;
constexpr wchar_t kSelectedDriveValue[] = L"SelectedDrive";
constexpr wchar_t kConfiguredDriveValue[] = L"ConfiguredDrive";

using GetHost_t = void*(WINAPI*)(void*, void*);
GetHost_t g_getHost = nullptr;
using FrameHeight_t = int(WINAPI*)(void*);
FrameHeight_t g_frameHeight = nullptr;
using Decref_t = void(WINAPI*)(void*);
Decref_t g_decref = nullptr;
void* g_siteVtable = nullptr;
size_t g_elementOffset = 0;

std::wstring StringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result(value ? value : L"");
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring LocalStringValue(PCWSTR name) {
    wchar_t value[32]{};
    if (!Wh_GetStringValue(name, value, ARRAYSIZE(value))) return {};
    return value;
}

// Only accept a drive letter, not an arbitrary path or a network share.
std::wstring NormalizeDrive(std::wstring value) {
    const auto first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return {};
    value = value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1);
    if (value.size() == 3 && (value[2] == L'\\' || value[2] == L'/')) value.pop_back();
    if (value.size() == 1) value += L':';
    if (value.size() != 2 || value[1] != L':') return {};
    value[0] = static_cast<wchar_t>(towupper(value[0]));
    if (value[0] < L'A' || value[0] > L'Z') return {};
    return value;
}

void LoadSettings() {
    Settings settings;
    const std::wstring configuredDrive = NormalizeDrive(StringSetting(L"Drive"));
    const std::wstring previousConfigured =
        NormalizeDrive(LocalStringValue(kConfiguredDriveValue));
    std::wstring selectedDrive = NormalizeDrive(LocalStringValue(kSelectedDriveValue));
    // The settings field is the initial/default choice. A panel click is kept
    // separately, but changing the settings field intentionally starts there.
    if (previousConfigured != configuredDrive || selectedDrive.empty()) {
        selectedDrive = configuredDrive;
        Wh_SetStringValue(kConfiguredDriveValue, configuredDrive.c_str());
        Wh_SetStringValue(kSelectedDriveValue, selectedDrive.c_str());
    }
    settings.drive = std::move(selectedDrive);
    settings.displayName = StringSetting(L"DisplayName");
    settings.hideDriveName = Wh_GetIntSetting(L"HideDriveName") != 0;
    const std::wstring colorScheme = StringSetting(L"FreeColor");
    if (colorScheme == L"green-red" || colorScheme == L"blue-orange" ||
        colorScheme == L"cyan-purple" || colorScheme == L"violet-yellow" ||
        colorScheme == L"teal-pink" || colorScheme == L"lime-indigo" ||
        colorScheme == L"amber-navy" || colorScheme == L"mint-coral" ||
        colorScheme == L"sky-magenta" || colorScheme == L"white-gray") {
        settings.colorScheme = colorScheme;
    } else if (colorScheme == L"green" || colorScheme.empty()) {
        // Keep old installations on the original green/red appearance.
        // Старые установки сохраняют исходную зелёно-красную палитру.
        settings.colorScheme = L"green-red";
    } else if (colorScheme == L"blue") {
        settings.colorScheme = L"blue-orange";
    } else if (colorScheme == L"cyan") {
        settings.colorScheme = L"cyan-purple";
    } else if (colorScheme == L"purple") {
        settings.colorScheme = L"violet-yellow";
    } else if (colorScheme == L"orange") {
        settings.colorScheme = L"amber-navy";
    }
    // Annotations are editor hints, so validate even on Windhawk 2.0.
    settings.interval = std::clamp(Wh_GetIntSetting(L"UpdateInterval"), 1, 3600);
    settings.width = std::clamp(Wh_GetIntSetting(L"Width"), 180, 600);
    settings.offset = std::clamp(Wh_GetIntSetting(L"LeftOffset"), 0, 1200);
    settings.reserve = Wh_GetIntSetting(L"ReserveSpace") != 0;
    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(settings);
}

Settings CurrentSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::wstring VolumeName(const std::wstring& root) {
    wchar_t label[MAX_PATH + 1]{};
    if (!GetVolumeInformationW(root.c_str(), label, ARRAYSIZE(label), nullptr,
                               nullptr, nullptr, nullptr, 0)) return {};
    std::wstring result(label);
    // Labels go into both XAML and the dynamic-selection local storage.
    std::replace(result.begin(), result.end(), L'\r', L' ');
    std::replace(result.begin(), result.end(), L'\n', L' ');
    return result;
}

std::wstring CapacityText(ULONGLONG freeBytes, ULONGLONG totalBytes, bool compact = false) {
    constexpr double gib = 1024.0 * 1024.0 * 1024.0;
    wchar_t text[128];
    swprintf_s(text, compact ? L"%.1f / %.1f ГиБ" : L"Свободно %.1f из %.1f ГиБ",
               freeBytes / gib, totalBytes / gib);
    std::wstring result(text);
    std::replace(result.begin(), result.end(), L'.', L',');
    return result;
}

Reading ReadDisk(const Settings& settings) {
    if (settings.drive.empty()) return {L"Выберите диск", L"Некорректная буква диска"};
    std::wstring root = settings.drive + L"\\";
    const bool fixed = GetDriveTypeW(root.c_str()) == DRIVE_FIXED;
    std::wstring title;
    if (settings.hideDriveName) {
        // Только буква диска / Drive letter only.
        title = L"(" + settings.drive + L")";
    } else {
        std::wstring label = settings.displayName;
        if (label.empty() && fixed) label = VolumeName(root);
        if (label.empty()) label = L"Локальный диск";
        title = label + L" (" + settings.drive + L")";
    }
    Reading result{title, L"Диск недоступен"};
    if (!fixed) return result;
    ULARGE_INTEGER available{}, total{};
    if (GetDiskFreeSpaceExW(root.c_str(), &available, &total, nullptr)) {
        result.capacity = CapacityText(available.QuadPart, total.QuadPart,
                                       settings.hideDriveName);
        if (total.QuadPart != 0) {
            result.freeRatio = std::clamp(
                static_cast<double>(available.QuadPart) /
                    static_cast<double>(total.QuadPart),
                0.0, 1.0);
            result.hasRatio = true;
        }
    }
    return result;
}

std::vector<std::wstring> FixedDrives() {
    std::vector<std::wstring> drives;
    const DWORD mask = GetLogicalDrives();
    if (!mask) return drives;
    for (int i = 0; i < 26; ++i) {
        std::wstring drive{static_cast<wchar_t>(L'A' + i), L':'};
        if ((mask & (1u << i)) &&
            GetDriveTypeW((drive + L"\\").c_str()) == DRIVE_FIXED) {
            drives.push_back(std::move(drive));
        }
    }
    return drives;
}

void SelectDrive(const std::wstring& drive) {
    const auto drives = FixedDrives();
    if (std::find(drives.begin(), drives.end(), drive) == drives.end()) return;
    Wh_SetStringValue(kSelectedDriveValue, drive.c_str());
    {
        std::lock_guard lock(g_settingsMutex);
        g_settings.drive = drive;
    }
    if (g_changed) SetEvent(g_changed);
    Wh_Log(L"Selected drive changed from the taskbar to %s", drive.c_str());
}

void ShowDriveMenu() {
    if (!g_ui.surface) return;
    const auto drives = FixedDrives();
    const Settings settings = CurrentSettings();
    MenuFlyout menu;
    for (const auto& drive : drives) {
        MenuFlyoutItem item;
        std::wstring label = VolumeName(drive + L"\\");
        if (label.empty()) label = L"Локальный диск";
        std::wstring text = (drive == settings.drive ? L"✓ " : L"  ") +
                            label + L" (" + drive + L")";
        item.Text(text);
        item.Click([drive](winrt::Windows::Foundation::IInspectable const&,
                           winrt::Windows::UI::Xaml::RoutedEventArgs const&) {
            SelectDrive(drive);
        });
        menu.Items().Append(item);
    }
    if (drives.empty()) {
        MenuFlyoutItem item;
        item.Text(L"Нет доступных локальных дисков");
        item.IsEnabled(false);
        menu.Items().Append(item);
    }
    // MenuFlyout is a native WinUI control, so its open/close animation,
    // shadows, corner radius and theme colors follow the taskbar automatically.
    g_ui.driveMenu = menu;
    g_ui.driveMenu.ShowAt(g_ui.surface);
}

struct ColorPair {
    Color free;
    Color used;
};

ColorPair ColorScheme(const std::wstring& name) {
    // Пары свободно/всего / Free/used color pairs.
    if (name == L"blue-orange") {
        return {Color{32, 70, 135, 220}, Color{32, 225, 135, 35}};
    }
    if (name == L"cyan-purple") {
        return {Color{32, 25, 180, 200}, Color{32, 145, 80, 190}};
    }
    if (name == L"violet-yellow") {
        return {Color{32, 145, 80, 190}, Color{32, 220, 195, 45}};
    }
    if (name == L"teal-pink") {
        return {Color{32, 40, 165, 145}, Color{32, 215, 90, 165}};
    }
    if (name == L"lime-indigo") {
        return {Color{32, 165, 205, 55}, Color{32, 95, 55, 175}};
    }
    if (name == L"amber-navy") {
        return {Color{32, 225, 165, 35}, Color{32, 90, 45, 25}};
    }
    if (name == L"mint-coral") {
        return {Color{32, 150, 210, 170}, Color{32, 230, 110, 85}};
    }
    if (name == L"sky-magenta") {
        return {Color{32, 95, 175, 235}, Color{32, 205, 45, 165}};
    }
    if (name == L"white-gray") {
        return {Color{26, 245, 245, 245}, Color{32, 100, 100, 100}};
    }
    // Default: green free space and red used space.
    return {Color{32, 45, 190, 70}, Color{32, 205, 55, 45}};
}

void ApplyHoverBackground() {
    if (!g_ui.surface) return;
    const bool light = g_ui.surface.ActualTheme() == ElementTheme::Light;
    Color freeColor{};
    Color usedColor{};
    if (g_ui.hovered) {
        if (g_ui.hasRatio) {
            // Цветовая пара выбирается в настройках / The color pair is
            // selected in settings. Alpha stays deliberately low so text
            // remains the primary visual.
            const ColorPair pair = ColorScheme(g_ui.colorScheme);
            freeColor = pair.free;
            usedColor = pair.used;
        } else {
            // Match the quiet neutral hover surface used by taskbar buttons
            // when disk capacity is unavailable.
            Color color{};
            HIGHCONTRASTW contrast{sizeof(contrast)};
            SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
            if (contrast.dwFlags & HCF_HIGHCONTRASTON) {
                const COLORREF highlight = GetSysColor(COLOR_HIGHLIGHT);
                color = Color{110, GetRValue(highlight), GetGValue(highlight),
                               GetBValue(highlight)};
            } else {
                bool resourceColor = false;
                try {
                    auto resource = Application::Current().Resources().Lookup(
                        winrt::box_value(L"SystemControlHighlightListLowBrush"));
                    if (auto brush = resource.try_as<SolidColorBrush>()) {
                        color = brush.Color();
                        resourceColor = true;
                    }
                } catch (...) {
                    // Older taskbar builds may not expose this WinUI resource.
                }
                if (!resourceColor) {
                    color = light ? Color{24, 0, 0, 0} : Color{32, 255, 255, 255};
                }
            }
            freeColor = color;
            usedColor = color;
        }
    }
    if (!g_ui.hoverBrush) {
        g_ui.hoverBrush = LinearGradientBrush();
        g_ui.hoverBrush.StartPoint(winrt::Windows::Foundation::Point{0, 0.5});
        g_ui.hoverBrush.EndPoint(winrt::Windows::Foundation::Point{1, 0.5});
        g_ui.freeStop = GradientStop();
        g_ui.freeEndStop = GradientStop();
        g_ui.usedStartStop = GradientStop();
        g_ui.usedStop = GradientStop();
        g_ui.freeStop.Offset(0);
        g_ui.usedStop.Offset(1);
        g_ui.hoverBrush.GradientStops().Append(g_ui.freeStop);
        g_ui.hoverBrush.GradientStops().Append(g_ui.freeEndStop);
        g_ui.hoverBrush.GradientStops().Append(g_ui.usedStartStop);
        g_ui.hoverBrush.GradientStops().Append(g_ui.usedStop);
        g_ui.surface.Background(g_ui.hoverBrush);
    }
    const double ratio = std::clamp(g_ui.freeRatio, 0.0, 1.0);
    g_ui.freeEndStop.Offset(ratio);
    g_ui.usedStartStop.Offset(ratio);
    g_ui.freeStop.Color(freeColor);
    g_ui.freeEndStop.Color(freeColor);
    g_ui.usedStartStop.Color(usedColor);
    g_ui.usedStop.Color(usedColor);
}

void PublishDrives() {
    DWORD mask = GetLogicalDrives();
    if (!mask) return;  // Do not discard the previous list on enumeration error.
    for (int i = 0; i < 26; ++i) {
        std::wstring drive{static_cast<wchar_t>(L'A' + i), L':'};
        std::wstring key = L"::wh_select_option::Drive::" + drive;
        std::wstring root = drive + L"\\";
        if ((mask & (1u << i)) && GetDriveTypeW(root.c_str()) == DRIVE_FIXED) {
            std::wstring label = VolumeName(root);
            if (label.empty()) label = L"Локальный диск";
            label += L" (" + drive + L")";
            Wh_SetStringValue(key.c_str(), label.c_str());
        } else {
            Wh_DeleteValue(key.c_str());
        }
    }
}

FrameworkElement FindElement(DependencyObject const& parent, PCWSTR name, int depth = 0) {
    if (!parent || depth > 24) return nullptr;
    auto element = parent.try_as<FrameworkElement>();
    if (element && element.Name() == name) return element;
    const int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto found = FindElement(VisualTreeHelper::GetChild(parent, i), name, depth + 1);
        if (found) return found;
    }
    return nullptr;
}

FrameworkElement FindFrame(DependencyObject const& parent, int depth = 0) {
    if (!parent || depth > 24) return nullptr;
    auto element = parent.try_as<FrameworkElement>();
    if (element && winrt::get_class_name(element) == L"Taskbar.TaskbarFrame") return element;
    for (int i = 0; i < VisualTreeHelper::GetChildrenCount(parent); ++i) {
        auto found = FindFrame(VisualTreeHelper::GetChild(parent, i), depth + 1);
        if (found) return found;
    }
    return nullptr;
}

FrameworkElement TaskbarFrame(HWND window, int* frameHeight = nullptr) {
    HWND bandWindow = reinterpret_cast<HWND>(GetPropW(window, L"TaskbandHWND"));
    if (!bandWindow) return nullptr;
    auto site = reinterpret_cast<void**>(GetWindowLongPtrW(bandWindow, 0));
    if (!site) return nullptr;
    int i = 0;
    for (; i < 20 && *site != g_siteVtable; ++i, ++site) {}
    if (i == 20) return nullptr;
    void* host[2]{};
    g_getHost(site, host);
    struct Guard {
        void* ref;
        ~Guard() { if (ref) g_decref(ref); }
    } guard{host[1]};
    if (!host[0] || !host[1]) return nullptr;
    if (frameHeight && g_frameHeight) *frameHeight = g_frameHeight(host[0]);
    auto unknown = *reinterpret_cast<::IUnknown**>(
        static_cast<BYTE*>(host[0]) + g_elementOffset);
    if (!unknown) return nullptr;
    FrameworkElement element{nullptr};
    if (FAILED(unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                      winrt::put_abi(element)))) return nullptr;
    auto root = element.XamlRoot();
    return root ? FindFrame(root.Content()) : nullptr;
}

void RemoveUi() {
    // Restore only the margin we actually own, preserving external changes.
    if (g_ui.repeater && g_ui.marginApplied) {
        try {
            auto margin = g_ui.repeater.Margin();
            if (std::abs(margin.Left - g_ui.lastMargin) < 0.01) {
                margin.Left -= g_ui.reserved;
                g_ui.repeater.Margin(margin);
            }
        } catch (...) { Wh_Log(L"Cannot restore taskbar margin"); }
    }
    if (g_ui.surface && g_ui.hasTappedHandler) {
        try {
            g_ui.surface.Tapped(g_ui.tappedToken);
        } catch (...) {
            Wh_Log(L"Cannot remove taskbar disk click handler");
        }
    }
    if (g_ui.surface && g_ui.hasPointerHandlers) {
        try {
            g_ui.surface.PointerEntered(g_ui.pointerEnteredToken);
            g_ui.surface.PointerExited(g_ui.pointerExitedToken);
        } catch (...) {
            Wh_Log(L"Cannot remove taskbar disk hover handlers");
        }
    }
    if (g_ui.driveMenu) {
        try {
            g_ui.driveMenu.Hide();
        } catch (...) {
            Wh_Log(L"Cannot close taskbar disk menu");
        }
    }
    if (g_ui.root && g_ui.surface) {
        try {
            uint32_t index;
            if (g_ui.root.Children().IndexOf(g_ui.surface, index)) g_ui.root.Children().RemoveAt(index);
        } catch (...) { Wh_Log(L"Taskbar visual tree was already removed"); }
    }
    g_ui = {};
}

void UpdateUi(HWND window, const Settings& settings, const Reading& reading) {
    int taskbarFrameHeight = 0;
    auto frame = TaskbarFrame(window, &taskbarFrameHeight);
    if (!frame) return;
    auto rootElement = FindElement(frame, L"RootGrid");
    auto root = rootElement ? rootElement.try_as<Grid>() : nullptr;
    if (!root) return;
    // Never access apartment-bound references from a different taskbar thread.
    if (g_ui.thread && g_ui.thread != GetCurrentThreadId()) return;
    if (g_ui.root != root) {
        RemoveUi();
        g_ui.thread = GetCurrentThreadId();
        g_ui.root = root;
        g_ui.repeater = FindElement(root, L"TaskbarFrameRepeater");
        g_ui.surface = Border();
        g_ui.surface.Name(kWidgetName);
        // Border gives the hover state the rounded, quiet Fluent surface used
        // by Windows 11 taskbar buttons. Its transparent background keeps the
        // complete configured width clickable, including empty space.
        g_ui.surface.IsHitTestVisible(true);
        g_ui.surface.CornerRadius(CornerRadius{8, 8, 8, 8});
        g_ui.surface.Padding(Thickness{8, 2, 8, 2});
        g_ui.surface.HorizontalAlignment(HorizontalAlignment::Left);
        g_ui.surface.VerticalAlignment(VerticalAlignment::Center);
        Canvas::SetZIndex(g_ui.surface, 1000);
        Grid::SetColumnSpan(g_ui.surface, std::max(1, static_cast<int>(root.ColumnDefinitions().Size())));
        g_ui.widget = StackPanel();
        g_ui.widget.IsHitTestVisible(false);
        g_ui.widget.Orientation(Orientation::Vertical);
        g_ui.widget.HorizontalAlignment(HorizontalAlignment::Stretch);
        g_ui.widget.VerticalAlignment(VerticalAlignment::Center);
        g_ui.title = TextBlock();
        g_ui.capacity = TextBlock();
        for (auto text : {g_ui.title, g_ui.capacity}) {
            text.FontFamily(FontFamily(L"Segoe UI Variable Text"));
            text.FontSize(12);
            text.VerticalAlignment(VerticalAlignment::Center);
            text.TextTrimming(TextTrimming::CharacterEllipsis);
            text.TextWrapping(TextWrapping::NoWrap);
            g_ui.widget.Children().Append(text);
        }
        g_ui.surface.Child(g_ui.widget);
        g_ui.title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        g_ui.tappedToken = g_ui.surface.Tapped(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::TappedRoutedEventArgs const&) {
                ShowDriveMenu();
            });
        g_ui.hasTappedHandler = true;
        g_ui.pointerEnteredToken = g_ui.surface.PointerEntered(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
                g_ui.hovered = true;
                ApplyHoverBackground();
            });
        g_ui.pointerExitedToken = g_ui.surface.PointerExited(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
                g_ui.hovered = false;
                ApplyHoverBackground();
            });
        g_ui.hasPointerHandlers = true;
        root.Children().Append(g_ui.surface);
        Wh_Log(L"Disk space widget added to taskbar");
    }
    // Match the Start button frame height reported by TaskbarHost.
    if (taskbarFrameHeight > 0 &&
        g_ui.surface.Height() != static_cast<double>(taskbarFrameHeight)) {
        g_ui.surface.Height(static_cast<double>(taskbarFrameHeight));
    }
    g_ui.widget.Orientation(settings.hideDriveName ? Orientation::Horizontal :
                            Orientation::Vertical);
    g_ui.capacity.Margin(settings.hideDriveName ? Thickness{8, 0, 0, 0} :
                         Thickness{0, 0, 0, 0});
    // Let the Border measure its content so the hover frame does not extend
    // far beyond the text. Width remains a maximum for long labels.
    double maxWidth = settings.width;
    if (root.ActualWidth() > 0) {
        maxWidth = std::max(0.0, std::min(maxWidth, root.ActualWidth() - settings.offset));
    }
    if (g_ui.surface.MaxWidth() != maxWidth) g_ui.surface.MaxWidth(maxWidth);
    if (!std::isnan(g_ui.surface.Width())) {
        g_ui.surface.Width(std::numeric_limits<double>::quiet_NaN());
    }
    double width = g_ui.surface.ActualWidth();
    if (width <= 0) width = maxWidth;
    Thickness placement{static_cast<double>(settings.offset), 0, 0, 0};
    if (g_ui.surface.Margin() != placement) g_ui.surface.Margin(placement);
    g_ui.freeRatio = reading.freeRatio;
    g_ui.hasRatio = reading.hasRatio;
    g_ui.colorScheme = settings.colorScheme;
    ApplyHoverBackground();
    if (g_ui.repeater) {
        auto margin = g_ui.repeater.Margin();
        double base = margin.Left;
        if (g_ui.marginApplied && std::abs(base - g_ui.lastMargin) < 0.01) base -= g_ui.reserved;
        g_ui.reserved = settings.reserve ? settings.offset + width + 12 : 0;
        double desired = base + g_ui.reserved;
        if (std::abs(margin.Left - desired) > 0.01) {
            margin.Left = desired;
            g_ui.repeater.Margin(margin);
        }
        g_ui.marginApplied = settings.reserve;
        g_ui.lastMargin = desired;
    }
    // No theme event handlers or timers are retained in Explorer's XAML tree.
    // The worker dispatch refreshes the theme, layout and text once per second.
    HIGHCONTRASTW contrast{sizeof(contrast)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
    COLORREF rgb = (contrast.dwFlags & HCF_HIGHCONTRASTON) ? GetSysColor(COLOR_WINDOWTEXT) :
        g_ui.widget.ActualTheme() == ElementTheme::Light ? RGB(24, 24, 24) : RGB(245, 245, 245);
    Color color{255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)};
    for (auto text : {g_ui.title, g_ui.capacity}) {
        auto brush = text.Foreground().try_as<SolidColorBrush>();
        if (!brush || brush.Color() != color) text.Foreground(SolidColorBrush(color));
    }
    if (g_ui.title.Text() != reading.title) g_ui.title.Text(reading.title);
    if (g_ui.capacity.Text() != reading.capacity) g_ui.capacity.Text(reading.capacity);
    auto accessible = reading.title + L". " + reading.capacity;
    Automation::AutomationProperties::SetName(g_ui.surface, accessible);
    Automation::AutomationProperties::SetHelpText(
        g_ui.surface, L"Нажмите для выбора локального диска");
}

struct Dispatch {
    HWND window;
    const Settings* settings;
    const Reading* reading;
    bool remove;
    bool invoked = false;
};
std::atomic<Dispatch*> g_pending{nullptr};

LRESULT CALLBACK DispatchHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        Dispatch* call = g_pending.load();
        // Never dereference pointers supplied by an arbitrary window message.
        if (call && message->message == g_dispatchMessage && message->hwnd == call->window &&
            message->lParam == reinterpret_cast<LPARAM>(call) &&
            g_pending.compare_exchange_strong(call, nullptr)) {
            try {
                if (call->remove) RemoveUi();
                else UpdateUi(call->window, *call->settings, *call->reading);
            } catch (...) {
                Wh_Log(L"Taskbar update failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
                // Do not retain a partially constructed widget after an error.
                RemoveUi();
            }
            call->invoked = true;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunOnTaskbar(Dispatch& call) {
    DWORD process = 0;
    DWORD thread = GetWindowThreadProcessId(call.window, &process);
    if (!thread || process != GetCurrentProcessId()) return false;
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, DispatchHook, nullptr, thread);
    if (!hook) return false;
    g_pending.store(&call);
    // Synchronous dispatch is essential: a timed-out stack context could be
    // accessed after return. The worker never holds a settings lock here.
    SendMessageW(call.window, g_dispatchMessage, 0, reinterpret_cast<LPARAM>(&call));
    g_pending.store(nullptr);
    UnhookWindowsHookEx(hook);
    return call.invoked;
}

HWND PrimaryTaskbar() {
    HWND result = nullptr;
    EnumWindows([](HWND window, LPARAM context) -> BOOL {
        DWORD process;
        GetWindowThreadProcessId(window, &process);
        if (process != GetCurrentProcessId()) return TRUE;
        wchar_t name[64];
        if (GetClassNameW(window, name, ARRAYSIZE(name)) && wcscmp(name, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(context) = window;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

DWORD WINAPI Worker(void*) {
    SetThreadErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX, nullptr);
    HWND lastWindow = nullptr;
    ULONGLONG nextRead = 0, nextEnumeration = 0;
    Reading reading;
    HANDLE events[] = {g_stop, g_changed};
    try {
        while (WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
            auto settings = CurrentSettings();
            auto now = GetTickCount64();
            if (now >= nextRead) {
                reading = ReadDisk(settings);
                nextRead = now + settings.interval * 1000ULL;
            }
            if (now >= nextEnumeration) {
                PublishDrives();
                nextEnumeration = now + 30000;
            }
            HWND window = PrimaryTaskbar();
            if (window) {
                Dispatch call{window, &settings, &reading, false};
                if (RunOnTaskbar(call)) lastWindow = window;
            }
            DWORD wait = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, 1000);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
            if (wait == WAIT_OBJECT_0 + 1) nextRead = nextEnumeration = 0;
        }
    } catch (...) {
        Wh_Log(L"Disk space worker failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
    }
    HWND window = IsWindow(lastWindow) ? lastWindow : PrimaryTaskbar();
    if (window) {
        Dispatch call{window, nullptr, nullptr, true};
        if (!RunOnTaskbar(call)) Wh_Log(L"Taskbar teardown dispatch failed");
    }
    return 0;
}

bool ResolveTaskbar() {
    g_taskbarModule = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_taskbarModule) return false;
    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK taskbarSymbols[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_siteVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_getHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_decref},
    };
    if (!WindhawkUtils::HookSymbols(g_taskbarModule, taskbarSymbols,
                                    ARRAYSIZE(taskbarSymbols))) return false;
    // Validate the implementation before extracting the FrameworkElement field.
#if defined(_M_X64)
    const BYTE* code = reinterpret_cast<const BYTE*>(g_frameHeight);
    if (code[0] != 0x48 || code[1] != 0x83 || code[2] != 0xEC ||
        code[4] != 0x48 || code[5] != 0x83 || code[6] != 0xC1 || code[7] > 0x7F) return false;
    g_elementOffset = code[7];
#elif defined(_M_ARM64)
    const DWORD* code = reinterpret_cast<const DWORD*>(g_frameHeight);
    if (code[0] != 0xD503237F || (code[1] & 0xFFC07FFF) != 0xA9807BFD ||
        code[2] != 0x910003FD || (code[3] & 0xFFF00FE0) != 0xF8400C00) return false;
    g_elementOffset = (code[3] >> 12) & 0xFF;
#else
#error Unsupported architecture
#endif
    return g_elementOffset != 0;
}

void ReleaseHandles() {
    if (g_worker) { CloseHandle(g_worker); g_worker = nullptr; }
    if (g_stop) { CloseHandle(g_stop); g_stop = nullptr; }
    if (g_changed) { CloseHandle(g_changed); g_changed = nullptr; }
    if (g_taskbarModule) { FreeLibrary(g_taskbarModule); g_taskbarModule = nullptr; }
}
} // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    if (!ResolveTaskbar()) {
        Wh_Log(L"Unsupported taskbar or unavailable symbols; Windows 11 22H2+ native taskbar required");
        ReleaseHandles();
        return FALSE;
    }
    g_dispatchMessage = RegisterWindowMessageW(L"Windhawk.TaskbarDiskSpace.Dispatch");
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_changed = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_dispatchMessage || !g_stop || !g_changed) {
        ReleaseHandles();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    g_worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_worker) Wh_Log(L"Cannot start disk space worker: %u", GetLastError());
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_changed) SetEvent(g_changed);
}

void Wh_ModBeforeUninit() {
    if (g_stop) SetEvent(g_stop);
    if (g_worker) WaitForSingleObject(g_worker, INFINITE);
}

void Wh_ModUninit() {
    ReleaseHandles();
    std::lock_guard lock(g_settingsMutex);
    Settings empty;
    std::swap(g_settings, empty);
}

