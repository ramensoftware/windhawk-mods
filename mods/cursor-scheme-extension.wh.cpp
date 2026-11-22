// ==WindhawkMod==
// @id              cursor-scheme-extension
// @name            Cursor Scheme Extension
// @description     Replaces built-in application pointers with pointers from the selected Windows scheme
// @description:ru-RU Заменяет встроенные курсоры приложений курсорами из выбранной схемы Windows
// @description:es-ES Sustituye los punteros propios de las aplicaciones por los del esquema seleccionado en Windows
// @description:de-DE Ersetzt anwendungseigene Mauszeiger durch die Zeiger des ausgewählten Windows-Schemas
// @description:pt-BR Substitui os ponteiros dos aplicativos pelos do esquema selecionado no Windows
// @description:zh-CN 将应用内置的鼠标指针替换为所选 Windows 方案中的指针
// @version         1.0.3
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         *
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lgdi32 -ladvapi32
// ==/WindhawkMod==
// Build: 26

// ==WindhawkModReadme==
/*
# Cursor Scheme Extension

**English** · [Русский](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Cursor_Scheme_Extension/README.ru.md)

Cursor Scheme Extension replaces built-in application pointers with pointers from the selected Windows scheme. This includes middle-button autoscroll, the pointing hand, resizing, and text and table selection.

The mod uses the scheme already selected in Windows and picks up changes to it automatically. Transformed pointers are created and cached in memory.

## Features

![Three pairs of application pointers without and with Cursor Scheme Extension: Windows Aero, W11 Concept, and Vision](https://raw.githubusercontent.com/CTravkin/DSN_Windhawk_Mods/main/Cursor_Scheme_Extension/Assets/Previews/Comparison.png)

Cursor artwork: Windows and Office by Microsoft, [W11 Concept](https://www.deviantart.com/jepricreations/art/Windows-11-Cursors-Concept-HD-v2-890672103) by jepriCreations, [Vision](https://www.deviantart.com/idarques/art/Vision-Cursor-911891424) by IDarques.

| Setting | Controls |
| --- | --- |
| Hand | Pointing hand over buttons, links, and other interactive elements |
| Resize | Resizing panels, rows, columns, and objects |
| Move | Dragging cells, ranges, and objects |
| Autoscroll activation | The pointer after a wheel-click, before choosing a scroll direction |
| Autoscroll arrows | All eight scrolling directions |
| Selection arrows | Selecting rows, columns, or an entire table |
| Cell selection | The cross over cells and when selecting ranges |
| Margin selection | Selecting text from the left page margin |
| Vertical text input | The horizontal mouse pointer in areas with vertical text |

At the start of autoscroll, the pointer indicates vertical, horizontal, or both axes. Direction arrows use **Alternate select** (`Up`) from the Windows scheme, rotated to match the scrolling direction. Selection arrows use the same role. Transforms account for the pointer hotspot, and animated pointers retain their frame order and timing.

The interface is available in English, Russian, Spanish, German, Brazilian Portuguese, and Simplified Chinese.

### Limitations

- Replacement applies to recognized pointers displayed through Windows cursor APIs. The application controls whether autoscroll is available, its speed, and its direction.
- Excel and Firefox draw their stationary autoscroll markers separately. These markers keep their original appearance.
- Monochrome pointers that invert the background retain their original pixel rendering without smoothing.
- Each mod load creates at most 128 replacement instances per process. Returning to a previously used scheme or smoothing setting reuses matching instances. When full, requests that need a new instance use the application's original pointer.
- After disabling, the application restores its pointer at its next cursor update, usually the next mouse move. Replacement cursor handles remain allocated until the process exits so that pointers still used by Windows stay valid.

## Platforms

Requires Windows 11. Runtime checks cover x86 and x64 processes on Windows 11 x64 with Windhawk 1.7.3. ARM64 has been compiled without a runtime check. Microsoft 365 and Qt WebEngine have not been verified.

### Compatibility

Some of this mod's features may overlap with the following mods:

- [Chromium Cursor Remap](https://windhawk.net/mods/chromium-cursor-remap) — additional cursor replacement in Chromium and Electron.
- [Cursor Override](https://windhawk.net/mods/cursor-override) — additional cursor replacement using `.cur` and `.ani` files.

Do not use multiple mods to replace the same cursors in the same application: the final cursor appearance may depend on the order in which replacements are applied.

## Usage

Select a pointer scheme in Windows, then open the mod's **Settings**. **Applications and processes** selects the application families, and **Pointer types** selects the shapes to replace. Both must be enabled for a replacement to apply.

| Family | Coverage |
| --- | --- |
| Windows | Native Windows pointers, standard controls, and Windows Forms |
| Office | Desktop Word, Excel, PowerPoint, and classic Outlook |
| Chromium | Chrome, Edge, Electron, and compatible embedded browsers |
| Gecko | Firefox and compatible apps: cell selection, resizing, and vertical text |
| Qt | Panel splitters in applications with a Qt interface |

Under **Application exclusions**, below the family switches, add one executable name or full path per row. A name matches any installation of that executable; a full path matches one location.

**Smoothing** has separate switches for **Autoscroll activation**, **Autoscroll arrows**, and **Selection arrows**. Autoscroll smoothing covers all eight directions.

## Links

- [Cursor Scheme Extension on GitHub](https://github.com/CTravkin/DSN_Windhawk_Mods/tree/main/Cursor_Scheme_Extension)
- [DSN Windhawk Mods](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/README.md)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targets:
    - windows: true
      $name: "Windows"
      $name:ru-RU: "Windows"
      $name:es-ES: "Windows"
      $name:de-DE: "Windows"
      $name:pt-BR: "Windows"
      $name:zh-CN: "Windows"
      $description: "System pointers in Windows and standard interface controls"
      $description:ru-RU: "Системные указатели Windows и стандартных элементов интерфейса"
      $description:es-ES: "Punteros del sistema en Windows y en los controles de interfaz estándar"
      $description:de-DE: "Systemzeiger von Windows und Standard-Bedienelementen"
      $description:pt-BR: "Ponteiros do sistema no Windows e nos controles de interface padrão"
      $description:zh-CN: "Windows 及标准界面控件中的系统指针"
    - office: false
      $name: "Office"
      $name:ru-RU: "Office"
      $name:es-ES: "Office"
      $name:de-DE: "Office"
      $name:pt-BR: "Office"
      $name:zh-CN: "Office"
      $description: "Desktop Word, Excel, PowerPoint, and classic Outlook"
      $description:ru-RU: "Настольные Word, Excel, PowerPoint и классический Outlook"
      $description:es-ES: "Versiones de escritorio de Word, Excel, PowerPoint y Outlook clásico"
      $description:de-DE: "Desktopversionen von Word, Excel, PowerPoint und das klassische Outlook"
      $description:pt-BR: "Versões para desktop do Word, Excel, PowerPoint e Outlook clássico"
      $description:zh-CN: "桌面版 Word、Excel、PowerPoint 和经典 Outlook"
    - chromium: true
      $name: "Chromium"
      $name:ru-RU: "Chromium"
      $name:es-ES: "Chromium"
      $name:de-DE: "Chromium"
      $name:pt-BR: "Chromium"
      $name:zh-CN: "Chromium"
      $description: "Chrome, Edge, Electron apps, and embedded Chromium browsers"
      $description:ru-RU: "Chrome, Edge, приложения на Electron и встроенные браузеры на Chromium"
      $description:es-ES: "Chrome, Edge, aplicaciones Electron y navegadores Chromium integrados"
      $description:de-DE: "Chrome, Edge, Electron-Apps und eingebettete Chromium-Browser"
      $description:pt-BR: "Chrome, Edge, aplicativos Electron e navegadores Chromium incorporados"
      $description:zh-CN: "Chrome、Edge、Electron 应用和嵌入式 Chromium 浏览器"
    - gecko: false
      $name: "Gecko"
      $name:ru-RU: "Gecko"
      $name:es-ES: "Gecko"
      $name:de-DE: "Gecko"
      $name:pt-BR: "Gecko"
      $name:zh-CN: "Gecko"
      $description: "Firefox and other applications based on Gecko"
      $description:ru-RU: "Firefox и другие приложения на движке Gecko"
      $description:es-ES: "Firefox y otras aplicaciones basadas en Gecko"
      $description:de-DE: "Firefox und andere Anwendungen auf Gecko-Basis"
      $description:pt-BR: "Firefox e outros aplicativos baseados no Gecko"
      $description:zh-CN: "Firefox 和其他基于 Gecko 的应用"
    - qt: true
      $name: "Qt"
      $name:ru-RU: "Qt"
      $name:es-ES: "Qt"
      $name:de-DE: "Qt"
      $name:pt-BR: "Qt"
      $name:zh-CN: "Qt"
      $description: "Applications with a Qt interface, such as VLC and VirtualBox"
      $description:ru-RU: "Приложения с интерфейсом на Qt, например VLC и VirtualBox"
      $description:es-ES: "Aplicaciones con interfaz Qt, como VLC y VirtualBox"
      $description:de-DE: "Anwendungen mit Qt-Oberfläche, etwa VLC und VirtualBox"
      $description:pt-BR: "Aplicativos com interface Qt, como VLC e VirtualBox"
      $description:zh-CN: "使用 Qt 界面的应用，例如 VLC 和 VirtualBox"
    - excludedApplications: [""]
      $name: "Application exclusions"
      $name:ru-RU: "Исключение приложений"
      $name:es-ES: "Exclusiones de aplicaciones"
      $name:de-DE: "Anwendungsausnahmen"
      $name:pt-BR: "Exclusões de aplicativos"
      $name:zh-CN: "应用排除列表"
      $description: "Executable name or full path, one per line"
      $description:ru-RU: "Имя файла запуска или полный путь, по одному на строку"
      $description:es-ES: "Nombre del ejecutable o ruta completa, uno por línea"
      $description:de-DE: "Name der ausführbaren Datei oder vollständiger Pfad, ein Eintrag pro Zeile"
      $description:pt-BR: "Nome do executável ou caminho completo, um por linha"
      $description:zh-CN: "每行填写一个可执行文件名或完整路径"
  $name: "Applications and processes"
  $name:ru-RU: "Приложения и процессы"
  $name:es-ES: "Aplicaciones y procesos"
  $name:de-DE: "Anwendungen und Prozesse"
  $name:pt-BR: "Aplicativos e processos"
  $name:zh-CN: "应用和进程"
- cursors:
    - link: true
      $name: "Hand"
      $name:ru-RU: "Рука"
      $name:es-ES: "Mano"
      $name:de-DE: "Hand"
      $name:pt-BR: "Mão"
      $name:zh-CN: "手形"
      $description: "Pointing hand over buttons, links, and other interactive elements"
      $description:ru-RU: "Указательный палец над кнопками, ссылками и другими активными элементами"
      $description:es-ES: "Mano sobre botones, enlaces y otros elementos interactivos"
      $description:de-DE: "Handzeiger über Schaltflächen, Links und anderen interaktiven Elementen"
      $description:pt-BR: "Mão sobre botões, links e outros elementos interativos"
      $description:zh-CN: "按钮、链接和其他交互元素上的手形指针"
    - resize: true
      $name: "Resize"
      $name:ru-RU: "Изменение размеров"
      $name:es-ES: "Cambiar tamaño"
      $name:de-DE: "Größe ändern"
      $name:pt-BR: "Redimensionar"
      $name:zh-CN: "调整大小"
      $description: "Resize panels, rows, columns, and objects"
      $description:ru-RU: "Изменение размеров панелей, строк, столбцов и объектов"
      $description:es-ES: "Cambiar el tamaño de paneles, filas, columnas y objetos"
      $description:de-DE: "Größe von Bereichen, Zeilen, Spalten und Objekten ändern"
      $description:pt-BR: "Redimensionamento de painéis, linhas, colunas e objetos"
      $description:zh-CN: "调整面板、行、列和对象的大小"
    - move: true
      $name: "Move"
      $name:ru-RU: "Перемещение"
      $name:es-ES: "Mover"
      $name:de-DE: "Verschieben"
      $name:pt-BR: "Mover"
      $name:zh-CN: "移动"
      $description: "Pointer for dragging cells, ranges, and objects"
      $description:ru-RU: "Указатель при перетаскивании ячеек, диапазонов и объектов"
      $description:es-ES: "Puntero para arrastrar celdas, rangos y objetos"
      $description:de-DE: "Zeiger beim Ziehen von Zellen, Bereichen und Objekten"
      $description:pt-BR: "Ponteiro ao arrastar células, intervalos e objetos"
      $description:zh-CN: "拖动单元格、区域和对象时的指针"
    - autoscrollOrigin: true
      $name: "Autoscroll activation"
      $name:ru-RU: "Включение автопрокрутки"
      $name:es-ES: "Inicio del desplazamiento automático"
      $name:de-DE: "Start des automatischen Scrollens"
      $name:pt-BR: "Início da rolagem automática"
      $name:zh-CN: "启动自动滚动"
      $description: "Pointer after pressing the mouse wheel, before choosing a scroll direction"
      $description:ru-RU: "Указатель после нажатия на колесико, до выбора направления прокрутки"
      $description:es-ES: "Puntero al pulsar la rueda del mouse, antes de elegir la dirección de desplazamiento"
      $description:de-DE: "Zeiger nach dem Drücken des Mausrads, vor der Wahl der Scrollrichtung"
      $description:pt-BR: "Ponteiro após pressionar a roda do mouse, antes de escolher a direção da rolagem"
      $description:zh-CN: "按下鼠标滚轮后、选择滚动方向前的指针"
    - autoscrollArrows: true
      $name: "Autoscroll arrows"
      $name:ru-RU: "Стрелки автопрокрутки"
      $name:es-ES: "Flechas de desplazamiento automático"
      $name:de-DE: "Pfeile für automatisches Scrollen"
      $name:pt-BR: "Setas de rolagem automática"
      $name:zh-CN: "自动滚动箭头"
      $description: "Arrows showing the autoscroll direction as the mouse moves away from the starting point"
      $description:ru-RU: "Стрелки направления автопрокрутки при отведении мыши"
      $description:es-ES: "Flechas que indican la dirección del desplazamiento automático al alejar el mouse del punto inicial"
      $description:de-DE: "Pfeile für die automatische Scrollrichtung beim Bewegen der Maus vom Startpunkt weg"
      $description:pt-BR: "Setas que indicam a direção da rolagem automática ao afastar o mouse do ponto inicial"
      $description:zh-CN: "鼠标移离起点时指示自动滚动方向的箭头"
    - selectionArrows: true
      $name: "Selection arrows"
      $name:ru-RU: "Стрелки выделения"
      $name:es-ES: "Flechas de selección"
      $name:de-DE: "Auswahlpfeile"
      $name:pt-BR: "Setas de seleção"
      $name:zh-CN: "选择箭头"
      $description: "Arrows for selecting rows, columns, or the entire table"
      $description:ru-RU: "Стрелки для выделения строк, столбцов или всей таблицы"
      $description:es-ES: "Flechas para seleccionar filas, columnas o toda la tabla"
      $description:de-DE: "Pfeile zur Auswahl von Zeilen, Spalten oder der gesamten Tabelle"
      $description:pt-BR: "Setas para selecionar linhas, colunas ou a tabela inteira"
      $description:zh-CN: "用于选择行、列或整个表格的箭头"
    - cell: false
      $name: "Cell selection"
      $name:ru-RU: "Выбор ячейки"
      $name:es-ES: "Selección de celdas"
      $name:de-DE: "Zellauswahl"
      $name:pt-BR: "Seleção de células"
      $name:zh-CN: "单元格选择"
      $description: "Cross-shaped pointer over cells and when selecting ranges"
      $description:ru-RU: "Крестик при наведении на ячейки и выделении диапазонов"
      $description:es-ES: "Puntero en forma de cruz sobre las celdas y al seleccionar rangos"
      $description:de-DE: "Kreuzzeiger über Zellen und beim Auswählen von Bereichen"
      $description:pt-BR: "Ponteiro em forma de cruz sobre células e ao selecionar intervalos"
      $description:zh-CN: "悬停在单元格上或选择区域时的十字指针"
    - marginSelection: true
      $name: "Margin selection"
      $name:ru-RU: "Выделение снаружи"
      $name:es-ES: "Selección desde el margen"
      $name:de-DE: "Auswahl vom Seitenrand"
      $name:pt-BR: "Seleção pela margem"
      $name:zh-CN: "从页边距选择"
      $description: "Arrow for selecting text from the left page margin"
      $description:ru-RU: "Стрелка для выделения текста при наведении на левое поле страницы"
      $description:es-ES: "Flecha para seleccionar texto desde el margen izquierdo de la página"
      $description:de-DE: "Pfeil zur Textauswahl vom linken Seitenrand"
      $description:pt-BR: "Seta para selecionar texto pela margem esquerda da página"
      $description:zh-CN: "从页面左侧边距选择文本时的箭头"
    - verticalText: true
      $name: "Vertical text input"
      $name:ru-RU: "Вертикальный ввод"
      $name:es-ES: "Entrada de texto vertical"
      $name:de-DE: "Vertikale Texteingabe"
      $name:pt-BR: "Entrada de texto vertical"
      $name:zh-CN: "竖排文本输入"
      $description: "Horizontal mouse pointer in areas with vertical text"
      $description:ru-RU: "Горизонтальный курсор мыши в областях с вертикальным текстом"
      $description:es-ES: "Puntero horizontal del mouse en áreas con texto vertical"
      $description:de-DE: "Waagerechter Mauszeiger in Bereichen mit vertikalem Text"
      $description:pt-BR: "Ponteiro horizontal do mouse em áreas com texto vertical"
      $description:zh-CN: "竖排文本区域中的水平鼠标指针"
  $name: "Pointer types"
  $name:ru-RU: "Виды курсоров"
  $name:es-ES: "Tipos de puntero"
  $name:de-DE: "Zeigertypen"
  $name:pt-BR: "Tipos de ponteiro"
  $name:zh-CN: "指针类型"
  $description: "Replace selected pointer types with pointers from the current Windows scheme"
  $description:ru-RU: "Подмена выбранных видов курсоров на курсоры текущей схемы Windows"
  $description:es-ES: "Sustituir los tipos de puntero seleccionados por los del esquema actual de Windows"
  $description:de-DE: "Ausgewählte Zeigertypen durch Zeiger des aktuellen Windows-Schemas ersetzen"
  $description:pt-BR: "Substituição dos tipos de ponteiro selecionados pelos do esquema atual do Windows"
  $description:zh-CN: "将选定类型的指针替换为当前 Windows 方案中的指针"
- smoothing:
    - autoscrollOrigin: true
      $name: "Autoscroll activation"
      $name:ru-RU: "Включение автопрокрутки"
      $name:es-ES: "Inicio del desplazamiento automático"
      $name:de-DE: "Start des automatischen Scrollens"
      $name:pt-BR: "Início da rolagem automática"
      $name:zh-CN: "启动自动滚动"
    - autoscrollArrows: true
      $name: "Autoscroll arrows"
      $name:ru-RU: "Стрелки автопрокрутки"
      $name:es-ES: "Flechas de desplazamiento automático"
      $name:de-DE: "Pfeile für automatisches Scrollen"
      $name:pt-BR: "Setas de rolagem automática"
      $name:zh-CN: "自动滚动箭头"
    - selectionArrows: true
      $name: "Selection arrows"
      $name:ru-RU: "Стрелки выделения"
      $name:es-ES: "Flechas de selección"
      $name:de-DE: "Auswahlpfeile"
      $name:pt-BR: "Setas de seleção"
      $name:zh-CN: "选择箭头"
  $name: "Smoothing"
  $name:ru-RU: "Сглаживание"
  $name:es-ES: "Suavizado"
  $name:de-DE: "Kantenglättung"
  $name:pt-BR: "Suavização"
  $name:zh-CN: "抗锯齿"
  $description: "Smooth the edges of replacement pointers"
  $description:ru-RU: "Сглаживание границ заменяемых курсоров"
  $description:es-ES: "Suavizado de los bordes de los punteros de sustitución"
  $description:de-DE: "Glättet die Ränder der ersetzten Mauszeiger"
  $description:pt-BR: "Suavização das bordas dos ponteiros substitutos"
  $description:zh-CN: "平滑替换后指针的边缘"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

enum class Role : size_t {
    Vertical, Horizontal, Move, Up, Arrow, Link, Cross, Text, ResizeNwse, ResizeNesw, Count,
};

enum class CursorType {
    AutoscrollOrigin,
    AutoscrollArrows,
    Link,
    Resize,
    MarginSelection,
    SelectionArrows,
    Cell,
    VerticalText,
    Move,
};

constexpr uint32_t TypeBit(CursorType type) {
    return uint32_t{1} << static_cast<unsigned>(type);
}

struct CursorMapping {
    WORD systemId;  // Zero for selection arrow shapes without a system ID.
    Role role;
    CursorType type;
    unsigned eighthTurns = 0;
    bool mirror = false;
};

constexpr WORD kFirstAutoscrollId = 32652;
constexpr WORD kLastAutoscrollId = 32662;
constexpr WORD kArrowId = 32512;
constexpr WORD kTextId = 32513;
constexpr WORD kCrossId = 32515;
constexpr WORD kNwseResizeId = 32642;
constexpr WORD kNeswResizeId = 32643;
constexpr WORD kHorizontalResizeId = 32644;
constexpr WORD kVerticalResizeId = 32645;
constexpr WORD kMoveId = 32646;
constexpr WORD kLinkId = 32649;
constexpr int kShapeSize = 32;

constexpr std::array kMappings = {
    CursorMapping{32652, Role::Vertical, CursorType::AutoscrollOrigin},
    CursorMapping{32653, Role::Horizontal, CursorType::AutoscrollOrigin},
    CursorMapping{32654, Role::Move, CursorType::AutoscrollOrigin},
    CursorMapping{32655, Role::Up, CursorType::AutoscrollArrows},
    CursorMapping{32656, Role::Up, CursorType::AutoscrollArrows, 4},
    CursorMapping{32657, Role::Up, CursorType::AutoscrollArrows, 6},
    CursorMapping{32658, Role::Up, CursorType::AutoscrollArrows, 2},
    CursorMapping{32659, Role::Up, CursorType::AutoscrollArrows, 7},
    CursorMapping{32660, Role::Up, CursorType::AutoscrollArrows, 1},
    CursorMapping{32661, Role::Up, CursorType::AutoscrollArrows, 5},
    CursorMapping{32662, Role::Up, CursorType::AutoscrollArrows, 3},
    CursorMapping{kArrowId, Role::Arrow, CursorType::MarginSelection, 0, true},
    CursorMapping{kLinkId, Role::Link, CursorType::Link},
    CursorMapping{kHorizontalResizeId, Role::Horizontal, CursorType::Resize},
    CursorMapping{kVerticalResizeId, Role::Vertical, CursorType::Resize},
    CursorMapping{kNwseResizeId, Role::ResizeNwse, CursorType::Resize},
    CursorMapping{kNeswResizeId, Role::ResizeNesw, CursorType::Resize},
    CursorMapping{kMoveId, Role::Move, CursorType::Move},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 4},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 2},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 6},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 3},
    CursorMapping{0, Role::Up, CursorType::SelectionArrows, 5},
    CursorMapping{kCrossId, Role::Cross, CursorType::Cell},
    CursorMapping{kTextId, Role::Text, CursorType::VerticalText, 2},
};

struct ResourceMapping {
    WORD id;
    WORD systemId;
};

constexpr std::array kCommonControlsResources = {
    ResourceMapping{109, 32652}, ResourceMapping{110, 32653},
    ResourceMapping{111, 32654}, ResourceMapping{112, 32655},
    ResourceMapping{113, 32656}, ResourceMapping{114, 32658},
    ResourceMapping{115, 32657}, ResourceMapping{116, 32660},
    ResourceMapping{117, 32659}, ResourceMapping{118, 32662},
    ResourceMapping{119, 32661},
};

struct CursorFingerprint {
    uint64_t pixels;
    WORD systemId;
    int hotX = kShapeSize / 2;
    int hotY = kShapeSize / 2;
};

// Chromium variants package the same Windows cursor images under different IDs.
// Keep the color plane as well as the mask to distinguish the three idle states.
constexpr std::array kChromiumCursors = {
    CursorFingerprint{0x357a05e1db72cda9, 32658},  // East
    CursorFingerprint{0x1c43369d8a027071, 32654},  // Both axes
    CursorFingerprint{0xfb403a4219749d01, 32653},  // Horizontal
    CursorFingerprint{0xd2923fde86022921, 32652},  // Vertical
    CursorFingerprint{0x4ca73a7cfff22179, 32655},  // North
    CursorFingerprint{0x7f5820a966f2b7d0, 32660},  // Northeast
    CursorFingerprint{0x44ab733a9503e37e, 32659},  // Northwest
    CursorFingerprint{0x01610cf653deb089, 32656},  // South
    CursorFingerprint{0xf438f4b2379ecc48, 32662},  // Southeast
    CursorFingerprint{0x10ea8f9d7a1c0876, 32661},  // Southwest
    CursorFingerprint{0x7ce71739ad53d0d5, 32657},  // West
    CursorFingerprint{0x702794672c4a6ec5, kHorizontalResizeId, 10, 8},
    CursorFingerprint{0x19747d1629ec5525, kVerticalResizeId, 9, 10},
    CursorFingerprint{0x5d9bc833e83e07c8, kCrossId, 7, 7},
    CursorFingerprint{0xae4a396c7f94f919, kTextId, 9, 3},
};

constexpr std::array kGeckoResources = {
    ResourceMapping{4103, kCrossId}, ResourceMapping{4108, kHorizontalResizeId},
    ResourceMapping{4109, kVerticalResizeId}, ResourceMapping{4110, kTextId},
};

constexpr std::array kWinFormsCursors = {
    CursorFingerprint{0x3d2b81e973cf56e9, 32658, 10, 14},
    CursorFingerprint{0xd93d0fd3350a3d65, kVerticalResizeId, 15, 15},
    CursorFingerprint{0x71eb275f9ab04048, 32660, 12, 18},
    CursorFingerprint{0xaf7c0cde3dde97f9, 32654, 16, 15},
    CursorFingerprint{0x7d323e4f02f00091, 32653, 16, 15},
    CursorFingerprint{0x13f5d35db6fc2c31, 32652, 16, 15},
    CursorFingerprint{0xadbd3ff7cbab4a91, 32655, 15, 20},
    CursorFingerprint{0x0f8885c50070a3a2, 32659, 20, 18},
    CursorFingerprint{0x32816356d17c25b8, 32662, 12, 11},
    CursorFingerprint{0xa968ca07a7fce9e9, 32656, 16, 8},
    CursorFingerprint{0x6d15dc3f672c03c8, 32661, 19, 10},
    CursorFingerprint{0x0c50ddb1fa40c055, kHorizontalResizeId, 15, 15},
    CursorFingerprint{0x46ea5b501abd3e51, 32657, 21, 14},
};

// Some Office sizing handles share an outline, with the direction in the XOR plane.
constexpr std::array kOfficeResizeCursors = {
    CursorFingerprint{0x2689c39224fa1451, kNeswResizeId, 10, 10},
    CursorFingerprint{0xb2b90ccbfdaee413, kNeswResizeId, 9, 9},
    CursorFingerprint{0xe80141f08ed56999, kNwseResizeId, 9, 9},
    CursorFingerprint{0x15fc4d693e6441d7, kNwseResizeId, 9, 9},
};

const std::array kRoleIds = {
    IDC_SIZENS, IDC_SIZEWE, IDC_SIZEALL, IDC_UPARROW, IDC_ARROW, IDC_HAND, IDC_CROSS, IDC_IBEAM,
    IDC_SIZENWSE, IDC_SIZENESW,
};
constexpr size_t kRoleCount = static_cast<size_t>(Role::Count);
constexpr size_t kUnknownSlot = kMappings.size();
constexpr int kMaxSourceSize = 512;

constexpr size_t SystemSlot(WORD id) {
    if (!id) {
        return kUnknownSlot;
    }
    for (size_t i = 0; i < kMappings.size(); ++i) {
        if (kMappings[i].systemId == id) {
            return i;
        }
    }
    return kUnknownSlot;
}

constexpr size_t SelectionSlot(unsigned eighthTurns) {
    for (size_t i = 0; i < kMappings.size(); ++i) {
        const auto& mapping = kMappings[i];
        if (mapping.type == CursorType::SelectionArrows && mapping.eighthTurns == eighthTurns) {
            return i;
        }
    }
    return kUnknownSlot;
}

struct CursorImage {
    int width = 0;
    int height = 0;
    int hotX = 0;
    int hotY = 0;
    // A monochrome cursor keeps the XOR plane below its AND plane.
    std::vector<uint32_t> mask;
    std::vector<uint32_t> color;
    std::vector<uint32_t> smoothPixels;

    bool operator==(const CursorImage&) const = default;
};

struct CursorFrame {
    CursorImage image;
    DWORD duration;

    bool operator==(const CursorFrame&) const = default;
};

struct CursorSource {
    std::vector<CursorFrame> frames;
    HCURSOR firstAnimationFrame = nullptr;

    bool operator==(const CursorSource&) const = default;
};

struct IconBitmaps : ICONINFO {
    IconBitmaps() : ICONINFO{} {}
    ~IconBitmaps() {
        if (hbmMask) {
            DeleteObject(hbmMask);
        }
        if (hbmColor) {
            DeleteObject(hbmColor);
        }
    }
};

struct MemoryDc {
    HDC value = CreateCompatibleDC(nullptr);

    ~MemoryDc() {
        if (value) {
            DeleteDC(value);
        }
    }
};

struct Replacement {
    HCURSOR cursor = nullptr;
    bool smooth = false;
    uint64_t generation = 0;
};

struct CursorInstance {
    HCURSOR cursor;
    HCURSOR original;
    uint64_t generation;
    size_t slot;
    bool smooth;
    std::shared_ptr<const CursorSource> source;
};

struct HostWindow {
    HWND root = nullptr;
    DWORD processId = 0;
    std::wstring path;
};

struct CursorShape {
    int width = 0;
    int height = 0;
    std::array<uint32_t, kShapeSize> rows{};

    bool operator==(const CursorShape&) const = default;
};

// Cropped silhouettes of shared Office pointers, independent of resource IDs.
constexpr std::array kOfficeShapes = {
    std::pair{SystemSlot(kArrowId), CursorShape{12, 20, {
        0x0c00, 0x0e00, 0x0f00, 0x0f80, 0x0fc0, 0x0fe0, 0x0ff0, 0x0ff8,
        0x0ffc, 0x0ffe, 0x0fff, 0x0ff0, 0x0ff0, 0x0e78, 0x0c78, 0x083c,
        0x003c, 0x001e, 0x001e, 0x000c,
    }}},
    std::pair{SystemSlot(kLinkId), CursorShape{17, 22, {
        0x0060, 0x00f0, 0x00f0, 0x00f0, 0x00f0, 0x03f0, 0x1ff0, 0x7ff0,
        0xfff0, 0x1fff7, 0x1ffff, 0x1ffff, 0x1fffe, 0x1fffc, 0x1fffc, 0x1fff8,
        0xfff8, 0xfff0, 0xfff0, 0x7fe0, 0x7fe0, 0x7fe0,
    }}},
    std::pair{SelectionSlot(4), CursorShape{9, 12, {
        0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x1ff, 0x1ff,
        0xfe, 0x7c, 0x38, 0x10,
    }}},
    std::pair{SelectionSlot(2), CursorShape{12, 9, {
        0xc0, 0x1c0, 0x3ff, 0x7ff, 0xfff, 0x7ff, 0x3ff, 0x1c0, 0xc0,
    }}},
    std::pair{SelectionSlot(6), CursorShape{12, 9, {
        0x30, 0x38, 0xffc, 0xffe, 0xfff, 0xffe, 0xffc, 0x38, 0x30,
    }}},
    std::pair{SelectionSlot(3), CursorShape{13, 13, {
        0x8, 0x1c, 0x3e, 0x7f, 0xfe, 0x19fc, 0x1ff8, 0x1ff0,
        0x1fe0, 0x1fc0, 0x1fc0, 0x1fe0, 0x1fe0,
    }}},
    std::pair{SelectionSlot(5), CursorShape{10, 10, {
        0x80, 0x1c0, 0x3e0, 0x1f3, 0xff, 0x7f, 0x3f, 0x3f, 0x7f, 0x7f,
    }}},
    std::pair{SystemSlot(kCrossId), CursorShape{15, 15, {
        0x3f0, 0x7f0, 0x7f0, 0x7f0, 0x3fff, 0x7fff, 0x7fff, 0x7fff,
        0x7fff, 0x7fff, 0x7ffe, 0x7f0, 0x7f0, 0x7f0, 0x7e0,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{17, 15, {
        0x100, 0x380, 0x7c0, 0xfe0, 0x27c8, 0x739c, 0xfffe, 0x1ffff,
        0xfffe, 0x739c, 0x27c8, 0xfe0, 0x7c0, 0x380, 0x100,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{18, 18, {
        0x300, 0x780, 0xfc0, 0x1fe0, 0x1fe0, 0x6798, 0xe79c, 0x1fffe,
        0x3ffff, 0x3ffff, 0x1fffe, 0xe79c, 0x6798, 0x1fe0, 0x1fe0, 0xfc0,
        0x780, 0x300,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{19, 19, {
        0x200, 0x700, 0xf80, 0x1fc0, 0x1fc0, 0x700, 0xc718, 0x1c71c,
        0x3fafe, 0x7ffff, 0x3fafe, 0x1c71c, 0xc718, 0x700, 0x1fc0, 0x1fc0,
        0xf80, 0x700, 0x200,
    }}},
    std::pair{SystemSlot(kMoveId), CursorShape{20, 28, {
        0x200, 0x700, 0xf80, 0x1fc0, 0x1fc0, 0x700, 0xc718, 0x1c71c,
        0x3f8fe, 0x7faff, 0x3fefe, 0x1ce1c, 0xde18, 0x3e00, 0x7fc0, 0xffc0,
        0x1ff80, 0x3ff00, 0x7fe00, 0xffe00, 0xfe00, 0x1ee00, 0x1e600, 0x3c200,
        0x3c000, 0x78000, 0x78000, 0x70000,
    }}},
    std::pair{SystemSlot(kNwseResizeId), CursorShape{13, 13, {
        0x7f, 0x7f, 0x3f, 0x1f, 0x3f, 0x77, 0x18e3, 0x1dc0,
        0x1f80, 0x1f00, 0x1f80, 0x1fc0, 0x1fc0,
    }}},
    std::pair{SystemSlot(kNeswResizeId), CursorShape{13, 13, {
        0x1fc0, 0x1fc0, 0x1f80, 0x1f00, 0x1f80, 0x1dc0, 0x18e3, 0x77,
        0x3f, 0x1f, 0x3f, 0x7f, 0x7f,
    }}},
    std::pair{SystemSlot(kHorizontalResizeId), CursorShape{17, 15, {
        0x6c0, 0x6c0, 0x6c0, 0x6c0, 0x36d8, 0x76dc, 0xfefe, 0x1feff,
        0xfefe, 0x76dc, 0x36d8, 0x6c0, 0x6c0, 0x6c0, 0x6c0,
    }}},
    std::pair{SystemSlot(kVerticalResizeId), CursorShape{15, 17, {
        0x80, 0x1c0, 0x3e0, 0x7f0, 0x7f0, 0x1c0, 0x7fff, 0x7fff,
        0x0, 0x7fff, 0x7fff, 0x1c0, 0x7f0, 0x7f0, 0x3e0, 0x1c0, 0x80,
    }}},
    // Worksheet headers have variants with different separator thicknesses.
    std::pair{SystemSlot(kHorizontalResizeId), CursorShape{18, 18, {
        0x780, 0x780, 0x780, 0x780, 0x780, 0x6798, 0xe79c, 0x1fffe,
        0x3ffff, 0x3ffff, 0x1fffe, 0xe79c, 0x6798, 0x780, 0x780, 0x780,
        0x780, 0x780,
    }}},
    std::pair{SystemSlot(kVerticalResizeId), CursorShape{18, 18, {
        0x300, 0x780, 0xfc0, 0x1fe0, 0x1fe0, 0x780, 0x780, 0x3ffff,
        0x3ffff, 0x3ffff, 0x3ffff, 0x780, 0x780, 0x1fe0, 0x1fe0, 0xfc0,
        0x780, 0x300,
    }}},
    std::pair{SystemSlot(kHorizontalResizeId), CursorShape{18, 18, {
        0xfc0, 0xfc0, 0xfc0, 0xfc0, 0xfc0, 0x6fd8, 0xefdc, 0x1fffe,
        0x3ffff, 0x3ffff, 0x1fffe, 0xefdc, 0x6fd8, 0xfc0, 0xfc0, 0xfc0,
        0xfc0, 0xfc0,
    }}},
    std::pair{SystemSlot(kVerticalResizeId), CursorShape{18, 18, {
        0x300, 0x780, 0xfc0, 0x1fe0, 0x1fe0, 0x780, 0x3ffff, 0x3ffff,
        0x3ffff, 0x3ffff, 0x3ffff, 0x3ffff, 0x780, 0x1fe0, 0x1fe0, 0xfc0,
        0x780, 0x300,
    }}},
};

enum class Target { Windows, Chromium, Gecko, Qt, Office };

struct CursorMatch {
    size_t slot = kUnknownSlot;
    Target target = Target::Windows;
};

std::array<CursorShape, kLastAutoscrollId - kFirstAutoscrollId + 1> g_referenceShapes;
std::array<std::optional<Replacement>, kMappings.size()> g_replacements;
std::array<std::shared_ptr<const CursorSource>, kRoleCount> g_sources;
std::array<bool, kRoleCount> g_needsRefresh;
std::array<ULONGLONG, kRoleCount> g_nextSourceProbe;
std::vector<HCURSOR> g_retired;
std::vector<CursorInstance> g_instances;
std::mutex g_mutex;
std::once_flag g_referenceShapesInit;
std::atomic<bool> g_hasInstances;
std::atomic<uint32_t> g_enabledTargets;
std::atomic<uint32_t> g_enabledTypes;
std::atomic<bool> g_isOffice;
std::atomic<bool> g_processExcluded;
std::atomic<bool> g_hasExclusions;
std::vector<std::wstring> g_excludedApplications;
HostWindow g_hostWindow;
uint32_t g_smoothingTypes;
bool g_stopping;
bool g_watchStarted;
unsigned g_activeCalls;
uint64_t g_nextGeneration;
HKEY g_cursorsKey;
HANDLE g_changeEvent;
decltype(&SetCursor) g_setCursorOriginal;
decltype(&GetCursor) g_getCursorOriginal;
template <size_t N>
size_t ResourceSlot(WORD id, const std::array<ResourceMapping, N>& resources) {
    for (const auto& resource : resources) {
        if (resource.id == id) {
            return SystemSlot(resource.systemId);
        }
    }
    return kUnknownSlot;
}

bool HasModuleName(std::wstring_view path, std::wstring_view module) {
    const auto slash = path.find_last_of(L"\\/");
    if (slash != path.npos) {
        path.remove_prefix(slash + 1);
    }
    return path.size() == module.size() &&
           CompareStringOrdinal(path.data(), static_cast<int>(path.size()),
                                module.data(), static_cast<int>(module.size()),
                                TRUE) == CSTR_EQUAL;
}

bool IsChromiumChildProcess() {
    std::wstring_view commandLine(GetCommandLineW());
    if (!commandLine.contains(L"--type=")) {
        return false;
    }
    // Parse argument boundaries instead of matching switches inside quoted paths
    // or document names. Keep Windows' quote/backslash rules without shell32.
    std::wstring argument;
    const auto nextArgument = [&]() {
        const auto first = commandLine.find_first_not_of(L" \t");
        if (first == std::wstring_view::npos) return false;
        commandLine.remove_prefix(first);
        argument.clear();
        bool quoted = false;
        size_t i = 0;
        while (i < commandLine.size() && (quoted || (commandLine[i] != L' ' && commandLine[i] != L'\t'))) {
            size_t slashes = 0;
            while (i < commandLine.size() && commandLine[i] == L'\\') {
                ++slashes;
                ++i;
            }
            if (i < commandLine.size() && commandLine[i] == L'"') {
                argument.append(slashes / 2, L'\\');
                if (slashes % 2) {
                    argument += L'"';
                } else if (quoted && i + 1 < commandLine.size() && commandLine[i + 1] == L'"') {
                    argument += L'"';
                    ++i;
                } else {
                    quoted = !quoted;
                }
                ++i;
            } else {
                argument.append(slashes, L'\\');
                if (i == commandLine.size() || (!quoted && (commandLine[i] == L' ' || commandLine[i] == L'\t'))) break;
                argument += commandLine[i++];
            }
        }
        commandLine.remove_prefix(i);
        return true;
    };
    nextArgument();  // Executable name is not a switch.
    bool type = false;
    bool chromiumHandle = false;
    while (nextArgument()) {
        type |= argument == L"--type=renderer" || argument == L"--type=gpu-process" ||
                argument == L"--type=utility";
        chromiumHandle |= argument.starts_with(L"--field-trial-handle=") ||
                          argument.starts_with(L"--mojo-platform-channel-handle=");
    }
    return type && chromiumHandle;
}

bool ObserveOffice() {
    if (g_isOffice.load(std::memory_order_relaxed)) {
        return true;
    }
    thread_local ULONGLONG nextProbe;
    const auto now = GetTickCount64();
    if (now < nextProbe) {
        return false;
    }
    nextProbe = now + 250;
    // The common Office runtime can be loaded after the mod starts.
    if (GetModuleHandleW(L"mso20win32client.dll")) {
        g_isOffice.store(true, std::memory_order_relaxed);
        return true;
    }
    return false;
}

BITMAPINFO ColorBitmapInfo(int width, int height) {
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    return info;
}

bool ReadBitmap(HDC dc,
                HBITMAP bitmap,
                int width,
                int height,
                std::vector<uint32_t>& pixels) {
    pixels.resize(size_t(width) * height);
    auto info = ColorBitmapInfo(width, height);
    return GetDIBits(dc, bitmap, 0, height, pixels.data(), &info,
                     DIB_RGB_COLORS) == height;
}

std::vector<uint32_t> DrawAlphaCursor(HCURSOR cursor, HDC dc, int width, int height) {
    auto info = ColorBitmapInfo(width, height);
    void* pixels = nullptr;
    IconBitmaps bitmap;
    bitmap.hbmColor = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bitmap.hbmColor || !pixels) {
        return {};
    }

    const size_t count = size_t(width) * height;
    std::vector<uint32_t> result(count);
    HGDIOBJ previous = SelectObject(dc, bitmap.hbmColor);
    if (!previous || previous == HGDI_ERROR) {
        return {};
    }
    std::memset(pixels, 0, count * sizeof(uint32_t));
    const bool drawn = DrawIconEx(dc, 0, 0, cursor, width, height, 0,
                                 nullptr, DI_NORMAL | DI_NOMIRROR);
    GdiFlush();
    SelectObject(dc, previous);
    if (!drawn) {
        return {};
    }
    // Drawing into a transparent surface gives premultiplied BGRA.
    std::memcpy(result.data(), pixels, count * sizeof(uint32_t));
    return result;
}

std::vector<uint32_t> PrepareSmoothPixels(HCURSOR cursor, HDC dc,
                                        const CursorImage& image) {
    if (std::any_of(image.color.begin(), image.color.end(),
                    [](uint32_t pixel) { return pixel >> 24; })) {
        return DrawAlphaCursor(cursor, dc, image.width, image.height);
    }

    const size_t count = size_t(image.width) * image.height;
    std::vector<uint32_t> result(count);
    for (size_t i = 0; i < count; ++i) {
        const uint32_t xorPixel = (image.color.empty() ? image.mask[count + i]
                                                      : image.color[i]) & 0x00FFFFFF;
        if (image.mask[i] & 0x00FFFFFF) {
            if (xorPixel) {
                // An inverted background cannot be represented with alpha blending.
                return {};
            }
        } else {
            result[i] = 0xFF000000 | xorPixel;
        }
    }
    return result;
}

std::optional<CursorImage> ReadCursorBitmaps(HCURSOR cursor, const ICONINFO& info,
                                            bool prepareSmoothing, int maxSize) {
    if (info.fIcon || !info.hbmMask) {
        return std::nullopt;
    }

    BITMAP mask{};
    if (!GetObjectW(info.hbmMask, sizeof(mask), &mask)) {
        return std::nullopt;
    }

    CursorImage image;
    image.width = mask.bmWidth;
    image.height = info.hbmColor ? mask.bmHeight : mask.bmHeight / 2;
    if (image.width < 1 || image.width > maxSize || image.height < 1 ||
        image.height > maxSize || info.xHotspot >= DWORD(image.width) ||
        info.yHotspot >= DWORD(image.height) ||
        (!info.hbmColor && mask.bmHeight != 2 * image.height)) {
        return std::nullopt;
    }

    image.hotX = static_cast<int>(info.xHotspot);
    image.hotY = static_cast<int>(info.yHotspot);
    MemoryDc dc;
    if (!dc.value || !ReadBitmap(dc.value, info.hbmMask, image.width,
                                 mask.bmHeight, image.mask)) {
        return std::nullopt;
    }

    if (info.hbmColor) {
        BITMAP color{};
        if (!GetObjectW(info.hbmColor, sizeof(color), &color) ||
            color.bmWidth != image.width || color.bmHeight != image.height ||
            !ReadBitmap(dc.value, info.hbmColor, image.width, image.height,
                        image.color)) {
            return std::nullopt;
        }
    }

    if (prepareSmoothing) {
        image.smoothPixels = PrepareSmoothPixels(cursor, dc.value, image);
    }
    return image;
}

std::optional<CursorImage> ReadCursor(HCURSOR cursor, bool prepareSmoothing,
                                     int maxSize = kMaxSourceSize) {
    IconBitmaps info;
    if (!cursor || !GetIconInfo(cursor, &info)) {
        return std::nullopt;
    }
    return ReadCursorBitmaps(cursor, info, prepareSmoothing, maxSize);
}

CursorShape MakeShape(const CursorImage& image) {
    const size_t plane = size_t(image.width) * image.height;
    const bool hasAlpha = std::any_of(image.color.begin(), image.color.end(),
                                     [](uint32_t pixel) { return pixel >> 24; });
    const auto visible = [&](int x, int y) {
        const size_t i = size_t(y) * image.width + x;
        const uint32_t pixel = image.color.empty() ? image.mask[plane + i]
                                                  : image.color[i];
        if (hasAlpha) {
            return (pixel >> 24) >= 128;
        }
        return !(image.mask[i] & 0x00FFFFFF) || (pixel & 0x00FFFFFF) != 0;
    };

    int left = image.width, right = -1;
    int top = image.height, bottom = -1;
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            if (visible(x, y)) {
                left = (std::min)(left, x);
                right = (std::max)(right, x);
                top = (std::min)(top, y);
                bottom = (std::max)(bottom, y);
            }
        }
    }
    if (right < 0 || right - left >= kShapeSize || bottom - top >= kShapeSize) {
        return {};
    }

    // Office editions can pad the same figure differently or move its hotspot.
    CursorShape shape{right - left + 1, bottom - top + 1};
    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            if (visible(x, y)) {
                shape.rows[y - top] |= uint32_t{1} << (x - left);
            }
        }
    }
    return shape;
}

std::optional<CursorImage> LoadResourceImage(HMODULE module, PCWSTR name) {
    const auto cursor = static_cast<HCURSOR>(LoadImageW(module, name, IMAGE_CURSOR,
                                                       kShapeSize, kShapeSize, 0));
    if (!cursor) {
        return std::nullopt;
    }

    std::optional<CursorImage> image;
    try {
        image = ReadCursor(cursor, false, kShapeSize);
    } catch (...) {
        DestroyCursor(cursor);
        throw;
    }
    DestroyCursor(cursor);
    return image;
}

CursorShape LoadResourceShape(HMODULE module, PCWSTR name) {
    const auto image = LoadResourceImage(module, name);
    return image ? MakeShape(*image) : CursorShape{};
}

size_t FindOfficeSlot(const CursorShape& shape) {
    if (!shape.width) {
        return kUnknownSlot;
    }
    for (const auto& [slot, reference] : kOfficeShapes) {
        if (reference == shape) {
            return slot;
        }
    }
    std::call_once(g_referenceShapesInit, [] {
        for (WORD id = kFirstAutoscrollId; id <= kLastAutoscrollId; ++id) {
            g_referenceShapes[id - kFirstAutoscrollId] =
                LoadResourceShape(nullptr, MAKEINTRESOURCEW(id));
        }
    });

    for (size_t i = 0; i < g_referenceShapes.size(); ++i) {
        if (g_referenceShapes[i] == shape) {
            return SystemSlot(static_cast<WORD>(kFirstAutoscrollId + i));
        }
    }
    return kUnknownSlot;
}

template <size_t N>
size_t FindFingerprintSlot(const CursorImage& image,
                           const std::array<CursorFingerprint, N>& fingerprints) {
    if (image.width != kShapeSize || image.height != kShapeSize) {
        return kUnknownSlot;
    }
    const auto matchesHotspot = [&](const CursorFingerprint& known) {
        return known.hotX == image.hotX && known.hotY == image.hotY;
    };
    if (std::none_of(fingerprints.begin(), fingerprints.end(), matchesHotspot)) {
        return kUnknownSlot;
    }
    uint64_t hash = 0xcbf29ce484222325;
    for (const auto* plane : {&image.mask, &image.color}) {
        for (uint32_t pixel : *plane) {
            hash = (hash ^ pixel) * 0x100000001b3;
        }
    }
    for (const auto& known : fingerprints) {
        if (known.pixels == hash && matchesHotspot(known)) {
            return SystemSlot(known.systemId);
        }
    }
    return kUnknownSlot;
}

size_t FindQtSplitterSlot(const CursorImage& image) {
    struct Splitter {
        int size;
        int hotX;
        int hotY;
        uint64_t mask;
        WORD systemId;
    };
    // Qt creates these handles from PNGs, so GetIconInfoEx has no resource ID.
    constexpr std::array splitters = {
        Splitter{32, 11, 11, 0xce3de8a32fb52cae, kVerticalResizeId},
        Splitter{48, 16, 17, 0xb08f48e137820f25, kVerticalResizeId},
        Splitter{64, 22, 22, 0x43f54dc870c43aa9, kVerticalResizeId},
        Splitter{32, 11, 11, 0x5ca6f5b51c206092, kHorizontalResizeId},
        Splitter{48, 16, 17, 0xf0c2ba92e0955965, kHorizontalResizeId},
        Splitter{64, 22, 22, 0x55eb77441590bd35, kHorizontalResizeId},
    };
    if (image.color.empty() || image.width != image.height) {
        return kUnknownSlot;
    }
    uint64_t mask = 0xcbf29ce484222325;
    for (uint32_t pixel : image.color) {
        mask = (mask ^ ((pixel >> 24) >= 128)) * 0x100000001b3;
    }
    for (const auto& splitter : splitters) {
        if (splitter.size == image.width && splitter.hotX == image.hotX &&
            splitter.hotY == image.hotY && splitter.mask == mask) {
            return SystemSlot(splitter.systemId);
        }
    }
    return kUnknownSlot;
}

BOOL CALLBACK FindChromiumPanningResource(HMODULE module, LPCWSTR, LPWSTR name, LONG_PTR data) {
    auto& found = *reinterpret_cast<bool*>(data);
    try {
        const auto image = LoadResourceImage(module, name);
        if (image) {
            const auto slot = FindFingerprintSlot(*image, kChromiumCursors);
            if (slot != kUnknownSlot && kMappings[slot].systemId >= kFirstAutoscrollId) {
                found = true;
                return FALSE;
            }
        }
        return TRUE;
    } catch (...) {
        return FALSE;
    }
}

bool HasChromiumPanningResources(PCWSTR name) {
    // IdentifyCursor already caches recognition; no heap-owned TLS may outlive
    // this DLL on a host UI thread.
    bool matches = false;
    HMODULE module = nullptr;
    if (*name && GetModuleHandleExW(0, name, &module)) {
        EnumResourceNamesW(module, MAKEINTRESOURCEW(12), FindChromiumPanningResource,
                           reinterpret_cast<LONG_PTR>(&matches));
        FreeLibrary(module);
    }
    return matches;
}

CursorMatch ReadResourceSlot(HCURSOR cursor, bool office) {
    ICONINFOEXW info{sizeof(info)};
    if (!GetIconInfoExW(cursor, &info)) {
        return {};
    }
    IconBitmaps bitmaps;
    bitmaps.fIcon = info.fIcon;
    bitmaps.xHotspot = info.xHotspot;
    bitmaps.yHotspot = info.yHotspot;
    bitmaps.hbmMask = info.hbmMask;
    bitmaps.hbmColor = info.hbmColor;

    if (info.fIcon) {
        return {};
    }
    if (HasModuleName(info.szModName, L"USER32") ||
        HasModuleName(info.szModName, L"user32.dll")) {
        // Native move and resize handles already use the active Windows scheme.
        return info.wResID >= kFirstAutoscrollId && info.wResID <= kLastAutoscrollId
            ? CursorMatch{SystemSlot(info.wResID), office ? Target::Office : Target::Windows}
            : CursorMatch{};
    }
    if (HasModuleName(info.szModName, L"comctl32.dll")) {
        return {ResourceSlot(info.wResID, kCommonControlsResources),
                office ? Target::Office : Target::Windows};
    }
    const bool named = *info.szModName;
    const bool gecko = HasModuleName(info.szModName, L"xul.dll");
    const size_t geckoSlot = gecko ? ResourceSlot(info.wResID, kGeckoResources) : kUnknownSlot;
    if (gecko && geckoSlot == kUnknownSlot) {
        return {};
    }
    const bool qtCandidate = !named && bitmaps.hbmColor &&
        (info.xHotspot == 11 || info.xHotspot == 16 || info.xHotspot == 22);
    const bool winForms = !named &&
        (GetModuleHandleW(L"clr.dll") || GetModuleHandleW(L"coreclr.dll"));
    if (!named && !office && !qtCandidate && !winForms) {
        return {};
    }

    const auto findSlot = [&](const CursorImage& image) -> CursorMatch {
        const auto chromiumSlot = named ? FindFingerprintSlot(image, kChromiumCursors)
                                        : kUnknownSlot;
        if (gecko) {
            return {chromiumSlot == geckoSlot ? geckoSlot : kUnknownSlot, Target::Gecko};
        }
        if (qtCandidate) {
            const auto qtSlot = FindQtSplitterSlot(image);
            if (qtSlot != kUnknownSlot) {
                return {qtSlot, Target::Qt};
            }
        }
        if (winForms) {
            const auto formsSlot = FindFingerprintSlot(image, kWinFormsCursors);
            if (formsSlot != kUnknownSlot) {
                return {formsSlot, Target::Windows};
            }
        }
        if (chromiumSlot != kUnknownSlot &&
            (!office || HasChromiumPanningResources(info.szModName))) {
            return {chromiumSlot, Target::Chromium};
        }
        if (office) {
            const auto slot = FindFingerprintSlot(image, kOfficeResizeCursors);
            return {slot != kUnknownSlot ? slot : FindOfficeSlot(MakeShape(image)), Target::Office};
        }
        return {};
    };
    const auto image = ReadCursorBitmaps(cursor, bitmaps, false, kShapeSize * 2);
    if (image) {
        const auto match = findSlot(*image);
        if (match.slot != kUnknownSlot ||
            (!office && image->width == kShapeSize && image->height == kShapeSize)) {
            return match;
        }
    }

    // A DPI-specific copy can differ from the original resource's 32px raster.
    PCWSTR name = info.wResID ? MAKEINTRESOURCEW(info.wResID) : info.szResName;
    HMODULE module = nullptr;
    if (!named || (!info.wResID && !*name) ||
        !GetModuleHandleExW(0, info.szModName, &module)) {
        return {};
    }

    std::optional<CursorImage> resource;
    try {
        resource = LoadResourceImage(module, name);
    } catch (...) {
        FreeLibrary(module);
        throw;
    }
    FreeLibrary(module);
    return resource ? findSlot(*resource) : CursorMatch{};
}

CursorMatch IdentifyCursor(HCURSOR cursor) {
    if (!cursor) {
        return {};
    }

    struct Lookup {
        HCURSOR cursor = nullptr;
        CursorMatch match;
        bool office = false;
    };
    // Bounded per GUI thread. Normal mouse movement usually hits the same slot.
    thread_local std::array<Lookup, 32> entries;
    const uintptr_t key = reinterpret_cast<uintptr_t>(cursor);
    auto& entry = entries[(key ^ (key >> 16)) % entries.size()];
    const bool office = ObserveOffice();
    if (entry.cursor != cursor || entry.office != office) {
        entry = {cursor, ReadResourceSlot(cursor, office), office};
    }
    return entry.match;
}

uint32_t SampleSmooth(const CursorImage& source, double centerX, double centerY,
                      double cosine, double sine) {
    // The outer taps reach adjacent pixels at exact right angles as well.
    constexpr std::array offsets = {-0.5625, -0.1875, 0.1875, 0.5625};
    constexpr std::array weights = {2u, 6u, 6u, 2u};
    constexpr unsigned totalWeight = 16 * 16;
    std::array<unsigned, 4> channels{};
    for (size_t j = 0; j < offsets.size(); ++j) {
        const double y = offsets[j];
        for (size_t k = 0; k < offsets.size(); ++k) {
            const double x = offsets[k];
            const int sx = static_cast<int>(std::lround(centerX + x * cosine + y * sine));
            const int sy = static_cast<int>(std::lround(centerY - x * sine + y * cosine));
            if (sx < 0 || sx >= source.width || sy < 0 || sy >= source.height) {
                continue;
            }
            const uint32_t pixel = source.smoothPixels[size_t(sy) * source.width + sx];
            const unsigned weight = weights[j] * weights[k];
            for (unsigned i = 0; i < channels.size(); ++i) {
                channels[i] += weight * ((pixel >> (8 * i)) & 0xFF);
            }
        }
    }

    uint32_t result = 0;
    for (unsigned i = 0; i < channels.size(); ++i) {
        result |= ((channels[i] + totalWeight / 2) / totalWeight) << (8 * i);
    }
    return result;
}

CursorImage Rotate(const CursorImage& source, unsigned eighthTurns, bool smooth) {
    const bool monochrome = source.color.empty() && !smooth;
    const double angle = eighthTurns * (3.14159265358979323846 / 4.0);
    // Round the exact right angles so trigonometric error cannot add a row.
    const double cosine = eighthTurns % 2 ? std::cos(angle) : std::round(std::cos(angle));
    const double sine = eighthTurns % 2 ? std::sin(angle) : std::round(std::sin(angle));

    double minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (int x : {-source.hotX, source.width - 1 - source.hotX}) {
        for (int y : {-source.hotY, source.height - 1 - source.hotY}) {
            const double rx = x * cosine - y * sine;
            const double ry = x * sine + y * cosine;
            minX = (std::min)(minX, rx);
            maxX = (std::max)(maxX, rx);
            minY = (std::min)(minY, ry);
            maxY = (std::max)(maxY, ry);
        }
    }

    CursorImage result;
    const int margin = smooth ? 1 : 0;
    result.hotX = margin - static_cast<int>(std::floor(minX));
    result.hotY = margin - static_cast<int>(std::floor(minY));
    result.width = result.hotX + static_cast<int>(std::ceil(maxX)) + 1 + margin;
    result.height = result.hotY + static_cast<int>(std::ceil(maxY)) + 1 + margin;
    const size_t plane = size_t(result.width) * result.height;
    const size_t sourcePlane = size_t(source.width) * source.height;
    result.mask.assign(plane, 0x00FFFFFF);
    if (monochrome) {
        result.mask.resize(plane * 2);
    } else {
        result.color.resize(plane);
    }

    for (int y = 0; y < result.height; ++y) {
        for (int x = 0; x < result.width; ++x) {
            const int dx = x - result.hotX;
            const int dy = y - result.hotY;
            const size_t to = size_t(y) * result.width + x;
            if (smooth) {
                const uint32_t pixel = SampleSmooth(source,
                    dx * cosine + dy * sine + source.hotX,
                    -dx * sine + dy * cosine + source.hotY, cosine, sine);
                result.color[to] = pixel;
                if (pixel >> 24) {
                    result.mask[to] = 0;
                }
                continue;
            }

            const int sx = static_cast<int>(std::lround(dx * cosine + dy * sine)) +
                           source.hotX;
            const int sy = static_cast<int>(std::lround(-dx * sine + dy * cosine)) +
                           source.hotY;
            if (sx < 0 || sx >= source.width || sy < 0 || sy >= source.height) {
                continue;
            }

            const size_t from = size_t(sy) * source.width + sx;
            result.mask[to] = source.mask[from];
            if (monochrome) {
                result.mask[plane + to] = source.mask[sourcePlane + from];
            } else {
                result.color[to] = source.color[from];
            }
        }
    }

    return result;
}

CursorImage Mirror(CursorImage image) {
    image.hotX = image.width - 1 - image.hotX;
    for (auto* plane : {&image.mask, &image.color}) {
        for (size_t row = 0; row < plane->size(); row += image.width) {
            std::reverse(plane->data() + row, plane->data() + row + image.width);
        }
    }
    return image;
}

HCURSOR MakeCursor(const CursorImage& image) {
    const int maskHeight = image.color.empty() ? image.height * 2 : image.height;
    // CreateBitmap takes scan lines aligned to a WORD for a monochrome DDB.
    const size_t stride = (size_t(image.width) + 15) / 16 * 2;
    std::vector<BYTE> maskBits(stride * maskHeight);
    for (int y = 0; y < maskHeight; ++y) {
        for (int x = 0; x < image.width; ++x) {
            if (image.mask[size_t(y) * image.width + x] & 0x00FFFFFF) {
                maskBits[size_t(y) * stride + x / 8] |= BYTE(0x80 >> (x % 8));
            }
        }
    }

    IconBitmaps info;
    info.xHotspot = image.hotX;
    info.yHotspot = image.hotY;
    info.hbmMask = CreateBitmap(image.width, maskHeight, 1, 1, maskBits.data());
    if (!info.hbmMask) {
        return nullptr;
    }

    if (!image.color.empty()) {
        auto dib = ColorBitmapInfo(image.width, image.height);
        void* pixels = nullptr;
        info.hbmColor = CreateDIBSection(nullptr, &dib, DIB_RGB_COLORS, &pixels,
                                         nullptr, 0);
        if (!info.hbmColor || !pixels) {
            return nullptr;
        }
        std::memcpy(pixels, image.color.data(), image.color.size() * sizeof(uint32_t));
    }

    return CreateIconIndirect(&info);
}

// User32 exposes the source step count, frame handles, and durations in jiffies.
// Resolve it dynamically so static cursors keep working if the export is absent.
using GetCursorFrameInfoFn = HCURSOR(WINAPI*)(HCURSOR, DWORD, DWORD, DWORD*, DWORD*);

GetCursorFrameInfoFn CursorFrameInfo() {
    static const auto frameInfo = reinterpret_cast<GetCursorFrameInfoFn>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetCursorFrameInfo"));
    return frameInfo;
}

CursorImage TransformCursor(const CursorImage& image, const CursorMapping& mapping,
                            bool smooth) {
    smooth = smooth && !image.smoothPixels.empty();
    if (mapping.mirror) {
        return Mirror(image);
    }
    return mapping.eighthTurns || smooth ? Rotate(image, mapping.eighthTurns, smooth) : image;
}

class RiffBuffer {
public:
    void WriteWord(WORD value) {
        bytes.push_back(static_cast<BYTE>(value));
        bytes.push_back(static_cast<BYTE>(value >> 8));
    }

    void WriteDword(DWORD value) {
        WriteWord(LOWORD(value));
        WriteWord(HIWORD(value));
    }

    size_t BeginChunk(const char (&id)[5]) {
        bytes.insert(bytes.end(), id, id + 4);
        const size_t sizeField = bytes.size();
        WriteDword(0);
        return sizeField;
    }

    void EndChunk(size_t sizeField) {
        const size_t size = bytes.size() - sizeField - sizeof(DWORD);
        for (size_t i = 0; i < sizeof(DWORD); ++i) {
            bytes[sizeField + i] = static_cast<BYTE>(size >> (8 * i));
        }
        if (size % 2) {
            bytes.push_back(0);
        }
    }

    std::vector<BYTE> bytes;
};

void AppendCursorFrame(RiffBuffer& riff, const CursorImage& image) {
    const auto chunk = riff.BeginChunk("icon");
    const size_t area = size_t(image.width) * image.height;
    const size_t maskStride = (size_t(image.width) + 31) / 32 * 4;
    const size_t bitmapSize = area * sizeof(DWORD) + maskStride * image.height;

    // A complete single-image CUR file inside the ANI frame chunk.
    riff.WriteWord(0);
    riff.WriteWord(2);
    riff.WriteWord(1);
    riff.bytes.push_back(static_cast<BYTE>(image.width));
    riff.bytes.push_back(static_cast<BYTE>(image.height));
    riff.WriteWord(0);
    riff.WriteWord(static_cast<WORD>(image.hotX));
    riff.WriteWord(static_cast<WORD>(image.hotY));
    riff.WriteDword(static_cast<DWORD>(sizeof(BITMAPINFOHEADER) + bitmapSize));
    riff.WriteDword(22);
    riff.WriteDword(sizeof(BITMAPINFOHEADER));
    riff.WriteDword(image.width);
    riff.WriteDword(image.height * 2);
    riff.WriteWord(1);
    riff.WriteWord(32);
    for (DWORD value : {DWORD{BI_RGB}, static_cast<DWORD>(bitmapSize), 0ul, 0ul, 0ul, 0ul}) {
        riff.WriteDword(value);
    }

    for (int y = image.height - 1; y >= 0; --y) {
        for (int x = 0; x < image.width; ++x) {
            const size_t i = size_t(y) * image.width + x;
            riff.WriteDword(image.color.empty() ? image.mask[area + i] & 0x00FFFFFF
                                               : image.color[i]);
        }
    }
    for (int y = image.height - 1; y >= 0; --y) {
        const size_t start = riff.bytes.size();
        riff.bytes.resize(start + maskStride);
        for (int x = 0; x < image.width; ++x) {
            if (image.mask[size_t(y) * image.width + x] & 0x00FFFFFF) {
                riff.bytes[start + x / 8] |= BYTE(0x80 >> (x % 8));
            }
        }
    }
    riff.EndChunk(chunk);
}

HCURSOR MakeReplacement(const CursorSource& source, const CursorMapping& mapping, bool smooth) {
    if (source.frames.empty()) {
        return nullptr;
    }
    if (source.frames.size() == 1) {
        return MakeCursor(TransformCursor(source.frames.front().image, mapping, smooth));
    }
    // RIFF ACON holds one transformed CUR per source step and the original rates.
    RiffBuffer riff;
    const auto container = riff.BeginChunk("RIFF");
    riff.bytes.insert(riff.bytes.end(), {'A', 'C', 'O', 'N'});
    const auto header = riff.BeginChunk("anih");
    const auto count = static_cast<DWORD>(source.frames.size());
    for (DWORD value : {36ul, count, count, 0ul, 0ul, 0ul, 0ul,
                        source.frames.front().duration, 1ul}) {
        riff.WriteDword(value);
    }
    riff.EndChunk(header);
    const auto rates = riff.BeginChunk("rate");
    for (const auto& frame : source.frames) {
        riff.WriteDword(frame.duration);
    }
    riff.EndChunk(rates);
    const auto frames = riff.BeginChunk("LIST");
    riff.bytes.insert(riff.bytes.end(), {'f', 'r', 'a', 'm'});
    for (const auto& frame : source.frames) {
        const auto image = TransformCursor(frame.image, mapping, smooth);
        if (image.width > 256 || image.height > 256) {
            return nullptr;
        }
        AppendCursorFrame(riff, image);
    }
    riff.EndChunk(frames);
    riff.EndChunk(container);
    return CreateIconFromResourceEx(riff.bytes.data(), static_cast<DWORD>(riff.bytes.size()),
                                    FALSE, 0x00030000, 0, 0, 0);
}

bool WatchCursorSettings() {
    return RegNotifyChangeKeyValue(g_cursorsKey, FALSE,
                                   REG_NOTIFY_CHANGE_LAST_SET |
                                       REG_NOTIFY_THREAD_AGNOSTIC,
                                   g_changeEvent, TRUE) == ERROR_SUCCESS;
}

void CloseSettingsWatch() {
    if (g_cursorsKey) {
        RegCloseKey(g_cursorsKey);
        g_cursorsKey = nullptr;
    }
    if (g_changeEvent) {
        CloseHandle(g_changeEvent);
        g_changeEvent = nullptr;
    }
}

void CheckCursorSettings() {
    if (!g_watchStarted) {
        g_watchStarted = true;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Cursors", 0,
                           KEY_NOTIFY, &g_cursorsKey) == ERROR_SUCCESS) {
            g_changeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            if (!g_changeEvent || !WatchCursorSettings()) {
                CloseSettingsWatch();
            }
        }
    }

    if (g_changeEvent && WaitForSingleObject(g_changeEvent, 0) == WAIT_OBJECT_0) {
        g_needsRefresh.fill(true);
        if (!WatchCursorSettings()) {
            CloseSettingsWatch();
        }
    }
}

bool RefreshSource(Role role) {
    const size_t index = static_cast<size_t>(role);
    const bool prepareSmoothing = std::any_of(
        kMappings.begin(), kMappings.end(), [role](const CursorMapping& mapping) {
            return mapping.role == role && (g_smoothingTypes & TypeBit(mapping.type));
        });
    const HCURSOR cursor = LoadCursorW(nullptr, kRoleIds[index]);
    if (!cursor) {
        g_needsRefresh[index] = true;
        return false;
    }
    const auto frameInfo = CursorFrameInfo();
    DWORD steps = 1;
    DWORD duration = 0;
    HCURSOR firstFrame = frameInfo ? frameInfo(cursor, 0, 0, &duration, &steps) : cursor;
    const bool animated = firstFrame && steps > 1;
    auto& previous = g_sources[index];
    if (!g_needsRefresh[index] && animated && previous &&
        previous->firstAnimationFrame == firstFrame) {
        g_nextSourceProbe[index] = GetTickCount64() + 1000;
        return true;
    }
    CursorSource source;
    if (animated) {
        constexpr DWORD maxSteps = 256;
        constexpr size_t maxPixels = 512 * 1024;
        size_t pixels = 0;
        source.firstAnimationFrame = firstFrame;
        for (DWORD step = 0; step < steps && steps <= maxSteps; ++step) {
            DWORD stepCount = 0;
            DWORD rate = 0;
            const HCURSOR frame = frameInfo(cursor, 0, step, &rate, &stepCount);
            auto image = ReadCursor(frame, prepareSmoothing, 256);
            if (!image || stepCount != steps) {
                break;
            }
            pixels += size_t(image->width) * image->height;
            if (pixels > maxPixels) {
                break;
            }
            source.frames.push_back({std::move(*image), rate});
        }
        if (source.frames.size() != steps) {
            source.frames.clear();
        }
    } else if (auto image = ReadCursor(cursor, prepareSmoothing)) {
        source.frames.push_back({std::move(*image), 0});
    }

    if (!previous || source != *previous) {
        for (size_t i = 0; i < kMappings.size(); ++i) {
            auto& entry = g_replacements[i];
            if (kMappings[i].role == role && entry) {
                if (entry->cursor) {
                    g_retired.push_back(entry->cursor);
                }
                entry.reset();
            }
        }
        previous = std::make_shared<const CursorSource>(std::move(source));
    }

    g_needsRefresh[index] = false;
    // Registry notifications refresh immediately. A bounded fallback also sees
    // SetSystemCursor changes and works when the registry watch is unavailable.
    g_nextSourceProbe[index] = GetTickCount64() + 1000;
    return true;
}

BOOL CALLBACK FindThreadWindow(HWND window, LPARAM data) {
    *reinterpret_cast<HWND*>(data) = window;
    return FALSE;
}

struct ChildWindowSearch {
    DWORD threadId;
    HWND window = nullptr;
};

BOOL CALLBACK FindOwnedChild(HWND window, LPARAM data) {
    auto& search = *reinterpret_cast<ChildWindowSearch*>(data);
    if (GetWindowThreadProcessId(window, nullptr) == search.threadId) {
        search.window = window;
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK FindChildInHost(HWND host, LPARAM data) {
    EnumChildWindows(host, FindOwnedChild, data);
    return !reinterpret_cast<ChildWindowSearch*>(data)->window;
}

HWND CursorWindowAtPointer() {
    POINT point{};
    return GetCursorPos(&point) ? WindowFromPoint(point) : nullptr;
}

HWND FindCursorWindow(DWORD threadId, HWND atPointer, HWND previous, bool enumerate) {
    // A window can set its cursor under the mouse without gaining focus.
    GUITHREADINFO info{sizeof(info)};
    GetGUIThreadInfo(threadId, &info);
    for (HWND window : {info.hwndCapture, atPointer, previous, info.hwndFocus, info.hwndActive}) {
        if (window && GetWindowThreadProcessId(window, nullptr) == threadId) {
            return window;
        }
    }
    if (!enumerate) {
        return nullptr;
    }
    HWND window = nullptr;
    EnumThreadWindows(threadId, FindThreadWindow, reinterpret_cast<LPARAM>(&window));
    if (window) {
        return window;
    }
    // An embedded engine might only own a child of another process's window.
    ChildWindowSearch search{threadId};
    EnumWindows(FindChildInHost, reinterpret_cast<LPARAM>(&search));
    return search.window;
}

void FreeRetired() {
    if (g_activeCalls || g_retired.empty()) {
        return;
    }

    // These are unpublished templates. Published instances survive the session.
    std::erase_if(g_retired, [](HCURSOR cursor) {
        return DestroyCursor(cursor) != FALSE;
    });
}

constexpr uint32_t TargetBit(Target target) {
    return uint32_t{1} << static_cast<unsigned>(target);
}

std::wstring ProcessImagePath(HANDLE process) {
    std::wstring path(MAX_PATH, L'\0');
    for (;;) {
        DWORD length = static_cast<DWORD>(path.size());
        if (QueryFullProcessImageNameW(process, 0, path.data(), &length)) {
            path.resize(length);
            return path;
        }
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || path.size() >= 32768) {
            return {};
        }
        path.resize((std::min)(path.size() * 2, size_t{32768}));
    }
}

bool IsExcluded(std::wstring_view path, const std::vector<std::wstring>& exclusions) {
    if (path.empty()) {
        return false;
    }
    for (const auto& entry : exclusions) {
        if (entry.find_first_of(L"\\:") == std::wstring::npos) {
            if (HasModuleName(path, entry)) {
                return true;
            }
        } else if (path.size() == entry.size() &&
                   CompareStringOrdinal(path.data(), static_cast<int>(path.size()),
                                        entry.data(), static_cast<int>(entry.size()),
                                        TRUE) == CSTR_EQUAL) {
            return true;
        }
    }
    return false;
}

std::vector<std::wstring> ReadExcludedApplications() {
    std::vector<std::wstring> entries;
    // The settings API exposes indexed values without an array length.
    for (int i = 0, empty = 0; empty < 32; ++i) {
        auto value = WindhawkUtils::StringSetting::make(L"targets.excludedApplications[%d]", i);
        const std::wstring_view text(value.get());
        const auto first = text.find_first_not_of(L" \t\r\n");
        if (first == std::wstring_view::npos) {
            ++empty;
            continue;
        }
        empty = 0;
        auto& entry = entries.emplace_back(text.substr(
            first, text.find_last_not_of(L" \t\r\n") - first + 1));
        std::replace(entry.begin(), entry.end(), L'/', L'\\');
    }
    return entries;
}

bool IsExcludedHostWindow(HWND window) {
    const HWND root = window ? GetAncestor(window, GA_ROOT) : nullptr;
    DWORD processId = 0;
    if (!root || !GetWindowThreadProcessId(root, &processId) ||
        processId == GetCurrentProcessId()) {
        return false;
    }

    // The caller holds g_mutex; this cache is cleared during unload.
    auto& host = g_hostWindow;
    if (host.root != root || host.processId != processId) {
        host = {root, processId, {}};
        if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId)) {
            try {
                host.path = ProcessImagePath(process);
            } catch (...) {
                CloseHandle(process);
                throw;
            }
            CloseHandle(process);
        }
    }
    return IsExcluded(host.path, g_excludedApplications);
}

bool ReplacementEnabled(CursorMatch match) {
    return match.slot != kUnknownSlot && !g_processExcluded.load(std::memory_order_acquire) &&
           (g_enabledTargets.load(std::memory_order_acquire) & TargetBit(match.target)) &&
           (g_enabledTypes.load(std::memory_order_acquire) & TypeBit(kMappings[match.slot].type));
}


HCURSOR LogicalCursor(HCURSOR cursor) {
    for (const auto& instance : g_instances) {
        if (instance.cursor == cursor) {
            return instance.original;
        }
    }
    return cursor;
}

HCURSOR InstanceFor(Replacement& image, HCURSOR original,
                    const CursorSource& source, const CursorMapping& mapping) {
    for (const auto& instance : g_instances) {
        if (instance.generation == image.generation && instance.original == original) {
            return instance.cursor;
        }
    }
    const size_t slot = static_cast<size_t>(&mapping - kMappings.data());
    for (auto& instance : g_instances) {
        if (instance.original == original && instance.slot == slot &&
            instance.smooth == image.smooth && instance.source->frames == source.frames) {
            // Reuse a previous appearance without invalidating any handle that
            // an attached or inherited input queue may still be displaying.
            instance.generation = image.generation;
            return instance.cursor;
        }
    }
    // Keep a pathological application from accumulating unlimited native handles.
    constexpr size_t maxInstances = 128;
    if (g_instances.size() >= maxInstances) {
        return nullptr;
    }
    // CopyImage flattens ANI, so animations must be instantiated from their frames.
    const bool animated = source.frames.size() > 1;
    const HCURSOR cursor = animated ? MakeReplacement(source, mapping, image.smooth)
        : static_cast<HCURSOR>(CopyImage(image.cursor, IMAGE_CURSOR, 0, 0, 0));
    if (cursor) {
        try {
            g_instances.push_back({cursor, original, image.generation, slot, image.smooth,
                                   g_sources[static_cast<size_t>(mapping.role)]});
        } catch (...) {
            DestroyCursor(cursor);
            throw;
        }
    }
    return cursor;
}

class ActiveCursorCall {
public:
    // Called while g_mutex is held, before publishing a handle to Win32.
    ActiveCursorCall() {
        ++g_activeCalls;
    }

    ~ActiveCursorCall() {
        std::lock_guard lock(g_mutex);
        --g_activeCalls;
        FreeRetired();
        g_hasInstances.store(!g_instances.empty());
    }
};

HCURSOR WINAPI GetCursorHook() {
    if (!g_hasInstances.load()) {
        return g_getCursorOriginal();
    }
    std::lock_guard lock(g_mutex);
    return LogicalCursor(g_getCursorOriginal());
}

HCURSOR SetCursorForMatch(HCURSOR cursor, CursorMatch match) {
    try {
        const bool enabled = ReplacementEnabled(match);
        if (!enabled && !g_hasInstances.load()) {
            return g_setCursorOriginal(cursor);
        }

        // Only embedded Chromium exclusions need a host window. Keep User32
        // queries outside the shared-state lock: they can synchronously reenter.
        HWND hostWindow = nullptr;
        bool hostReady = true;
        thread_local HWND previousWindow;
        thread_local bool findingHostWindow;
        thread_local ULONGLONG nextWindowProbe;
        const bool checkHost = enabled && match.target == Target::Chromium && g_hasExclusions.load();
        if (checkHost && findingHostWindow) {
            // A nested callback must not bypass an exclusion being resolved by
            // the outer query. Still translate its returned previous handle.
            hostReady = false;
        } else if (checkHost) {
            findingHostWindow = true;
            HWND atPointer = GetCapture();
            if (!atPointer) {
                atPointer = CursorWindowAtPointer();
            }
            const auto now = GetTickCount64();
            const bool enumerate = now >= nextWindowProbe;
            hostWindow = FindCursorWindow(GetCurrentThreadId(), atPointer, previousWindow, enumerate);
            if (!hostWindow && enumerate) {
                nextWindowProbe = now + 250;
            }
            previousWindow = hostWindow;
            findingHostWindow = false;
        }

        std::unique_lock lock(g_mutex);
        HCURSOR display = cursor;
        if (!g_stopping && ReplacementEnabled(match) && hostReady) {
            const bool excludedHost = hostWindow && match.target == Target::Chromium &&
                !g_excludedApplications.empty() && IsExcludedHostWindow(hostWindow);
            if (!excludedHost) {
                const auto& mapping = kMappings[match.slot];
                const size_t role = static_cast<size_t>(mapping.role);
                CheckCursorSettings();
                if ((!g_needsRefresh[role] && g_sources[role] &&
                     GetTickCount64() < g_nextSourceProbe[role]) ||
                    RefreshSource(mapping.role)) {
                    const auto& source = *g_sources[role];
                    const bool smooth = (g_smoothingTypes & TypeBit(mapping.type)) != 0;
                    auto& image = g_replacements[match.slot];
                    if (image && image->smooth != smooth) {
                        if (image->cursor) {
                            g_retired.push_back(image->cursor);
                        }
                        image.reset();
                    }
                    if (!image) {
                        image = Replacement{MakeReplacement(source, mapping, smooth),
                                            smooth, ++g_nextGeneration};
                    }
                    if (image->cursor) {
                        if (HCURSOR instance = InstanceFor(*image, cursor, source, mapping)) {
                            display = instance;
                            g_hasInstances.store(true);
                        }
                    }
                }
            }
        }

        ActiveCursorCall active;
        lock.unlock();
        // SetCursor emits synchronous WinEvents. No shared-state lock may span it.
        const HCURSOR previous = g_setCursorOriginal(display);
        lock.lock();
        const HCURSOR logicalPrevious = LogicalCursor(previous);
        lock.unlock();
        return logicalPrevious;
    } catch (...) {
        return g_setCursorOriginal(cursor);
    }
}

HCURSOR WINAPI SetCursorHook(HCURSOR cursor) {
    try {
        // Also accept handles obtained through a visual snapshot API.
        if (g_hasInstances.load()) {
            std::lock_guard lock(g_mutex);
            cursor = LogicalCursor(cursor);
        }
        const auto match = g_processExcluded.load(std::memory_order_acquire)
            ? CursorMatch{} : IdentifyCursor(cursor);
        return SetCursorForMatch(cursor, match);
    } catch (...) {
        return g_setCursorOriginal(cursor);
    }
}

void UpdateSettings() {
    constexpr std::array smoothingSettings = {
        std::pair{CursorType::AutoscrollOrigin, L"smoothing.autoscrollOrigin"},
        std::pair{CursorType::AutoscrollArrows, L"smoothing.autoscrollArrows"},
        std::pair{CursorType::SelectionArrows, L"smoothing.selectionArrows"},
    };
    uint32_t smoothingTypes = 0;
    for (const auto& [type, name] : smoothingSettings) {
        if (Wh_GetIntSetting(name)) {
            smoothingTypes |= TypeBit(type);
        }
    }
    constexpr std::array targetSettings = {
        std::pair{Target::Windows, L"targets.windows"},
        std::pair{Target::Chromium, L"targets.chromium"},
        std::pair{Target::Gecko, L"targets.gecko"},
        std::pair{Target::Qt, L"targets.qt"},
        std::pair{Target::Office, L"targets.office"},
    };
    constexpr std::array cursorSettings = {
        std::pair{CursorType::AutoscrollOrigin, L"cursors.autoscrollOrigin"},
        std::pair{CursorType::AutoscrollArrows, L"cursors.autoscrollArrows"},
        std::pair{CursorType::Link, L"cursors.link"},
        std::pair{CursorType::Move, L"cursors.move"},
        std::pair{CursorType::Resize, L"cursors.resize"},
        std::pair{CursorType::MarginSelection, L"cursors.marginSelection"},
        std::pair{CursorType::SelectionArrows, L"cursors.selectionArrows"},
        std::pair{CursorType::Cell, L"cursors.cell"},
        std::pair{CursorType::VerticalText, L"cursors.verticalText"},
    };
    uint32_t enabledTypes = 0;
    for (const auto& [type, name] : cursorSettings) {
        if (Wh_GetIntSetting(name)) {
            enabledTypes |= TypeBit(type);
        }
    }
    uint32_t enabledTargets = 0;
    for (const auto& [target, name] : targetSettings) {
        if (Wh_GetIntSetting(name)) {
            enabledTargets |= TargetBit(target);
        }
    }
    auto exclusions = ReadExcludedApplications();
    const bool processExcluded = !exclusions.empty() &&
        IsExcluded(ProcessImagePath(GetCurrentProcess()), exclusions);
    std::lock_guard lock(g_mutex);
    const uint32_t changedSmoothing = g_smoothingTypes ^ smoothingTypes;
    if (changedSmoothing) {
        for (const auto& mapping : kMappings) {
            if (changedSmoothing & TypeBit(mapping.type)) {
                g_needsRefresh[static_cast<size_t>(mapping.role)] = true;
            }
        }
        g_smoothingTypes = smoothingTypes;
    }
    g_excludedApplications = std::move(exclusions);
    g_hasExclusions.store(!g_excludedApplications.empty());
    g_processExcluded.store(processExcluded, std::memory_order_release);
    g_enabledTargets.store(enabledTargets, std::memory_order_release);
    g_enabledTypes.store(enabledTypes, std::memory_order_release);
}

}  // namespace

BOOL Wh_ModInit() {
    if (IsChromiumChildProcess()) {
        return FALSE;
    }
    g_needsRefresh.fill(true);
    ObserveOffice();
    UpdateSettings();

    if (!WindhawkUtils::SetFunctionHook(GetCursor, GetCursorHook, &g_getCursorOriginal) ||
        !WindhawkUtils::SetFunctionHook(SetCursor, SetCursorHook, &g_setCursorOriginal)) {
        CloseSettingsWatch();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    UpdateSettings();
}



void Wh_ModBeforeUninit() {
    std::lock_guard lock(g_mutex);
    g_stopping = true;
}

void Wh_ModUninit() {
    std::lock_guard lock(g_mutex);
    CloseSettingsWatch();
    // Templates were never handed to Win32.
    for (auto& image : g_replacements) {
        if (image && image->cursor) {
            DestroyCursor(image->cursor);
        }
        image.reset();
    }
    for (HCURSOR cursor : g_retired) {
        DestroyCursor(cursor);
    }
    g_retired.clear();
    for (auto& source : g_sources) {
        source.reset();
    }
    // Published native handles can belong to any attached or inherited input
    // queue. They contain no mod callbacks and remain valid until process exit.
    // Windhawk can unload this DLL normally; the next native SetCursor restores
    // the application's pointer without running code from an unloaded image.
    g_instances.clear();
    g_hasInstances.store(false);
    g_hostWindow = {};
}
